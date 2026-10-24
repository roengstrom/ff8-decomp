#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "ui/dialog.h"
#include "ui/text.h"
#include "ui/window.h"
#include "ui/font.h"
#include "btl_anim.h"
#include "ui/icon.h"
#include "numstr.h"
#include "menu_tint.h"
#include "psxsdk/libetc.h"
#include "btl_anim_packet.h"
#include "snd_sfx.h"

/** @brief r, g and b of a colour word shifted right once, without the bits
 * each channel took from the one above. */
#define RGB_HALF_MASK 0x7F7F7F

/** @brief Semi-transparency option bit of a primitive in the colour word. */
#define SPRT_ABE (SPRT_CODE_ABE << SPRT_CODE_SHIFT)

/** @brief Draw-offset command word: the code, then 11-bit x and y fields. */
#define DR_OFFSET_CODE 0xE5000000
#define DR_OFFSET_COORD_MASK 0x7FF
#define DR_OFFSET_Y_SHIFT 11

/** @brief The next-page marker sits this many pixels in from the window's bottom-right corner. */
#define NEXT_PAGE_MARKER_INSET 24

/** @brief Values of @c Dialog.seqState, the text state machine run by updateDialog. */
enum {
    DIALOG_SEQ_START, /**< Clear the button auto-repeat state. */
    DIALOG_SEQ_TICK, /**< Advance the character timer by the text speed. */
    DIALOG_SEQ_PRINT, /**< Wait for the character timer, then print. */
    DIALOG_SEQ_NEXT_CHAR, /**< Decode the next character or command. */
    DIALOG_SEQ_NEWLINE, /**< Scroll first if the window is full. */
    DIALOG_SEQ_SCROLL, /**< Scroll up by one line. */
    DIALOG_SEQ_END, /**< End of the message. */
    DIALOG_SEQ_DONE, /**< Finished; nothing left to run. */
    DIALOG_SEQ_PAGE, /**< Page break: set up the next-page marker. */
    DIALOG_SEQ_PAGE_RELEASE, /**< Wait until only the d-pad is held. */
    DIALOG_SEQ_PAGE_WAIT, /**< Wait for Cross or Square, then start the next page. */
    DIALOG_SEQ_CHOICE_START,
    DIALOG_SEQ_CHOICE_RELEASE, /**< Wait until no button is pressed. */
    DIALOG_SEQ_CHOICE, /**< Move the choice cursor, confirm or cancel. */
    DIALOG_SEQ_WAIT_START,
    DIALOG_SEQ_WAIT, /**< Count down @c Dialog.waitTimer. */
    DIALOG_SEQ_DPAD_RELEASE_START,
    DIALOG_SEQ_DPAD_RELEASE /**< Wait for the d-pad to be released, then offer the choice. */
};

/** @brief Command arguments are stored offset by this value (' '). */
#define MSG_ARG_BASE 0x20

/** @brief Bit of @c DialogGlobalState.textBlinkClock that dims blinking text; set for 16 of every 32 frames. */
#define DIALOG_TEXT_BLINK_DIM 0x10

/** @brief @c Dialog.firstChoice / @c lastChoice when the message offers no choice. */
#define DIALOG_NO_CHOICE 0xFF

/** @brief The clamp rect of a message window sits this many pixels inside its bound rect. */
#define DIALOG_CLAMP_INSET 6

/** @brief Position of the choice cursor within its line. */
#define DIALOG_CURSOR_X 4
#define DIALOG_CURSOR_Y 5

/**
 * @brief Bit of @c Dialog.ctrl.raw: @c ctrl.bits.nextPageMarker, the window shows its
 * blinking next-page marker.
 */
#define DIALOG_CTRL_NEXT_PAGE_MARKER 0x00800000

/** @brief Position of @c Dialog.ctrl.bits.nextPageMarkerBlink inside @c ctrl.raw. */
#define DIALOG_CTRL_NEXT_PAGE_MARKER_BLINK_SHIFT 16

/**
 * @brief Bit of the 7-bit blink counter @c Dialog.ctrl.bits.nextPageMarkerBlink.
 *
 * Set for 16 of every 32 ticks; the next-page marker is blanked while it is set.
 */
#define DIALOG_NEXT_PAGE_MARKER_BLINK_OFF 0x10

static void applyWindowBrightness(s32 index);
static void drawNextPageMarker(P_TAG *ot, Dialog *entry);
static inline void updateOpenDialogScale(s32 index);
static void updateDialog(s32 index, u32 input, u32 repeat);
static void drawDialogContents(s32 index, P_TAG *ot);
static s32 autoRepeatPadChannel(EngineState *engine, GamePad *port, DialogSystem *sys, u16 newVal, s32 channel);
static void drawDialog(P_TAG *ot, s32 index);
static void *renderDialogEntity(void *ot, BattleDisplayEntity *entity, void *pkt);
static u8 *updateDialogEntity(BattleDisplayEntity *entity, u32 input, u32 repeat);
static void scaleDialogRect(RECT *rect, s32 scale, s32 arg2);
static void initDialog(s32 idx);


/**
 * @brief Set a dialog's text origin.
 * @param idx Dialog index.
 * @param x Text origin x in pixels (@c textX).
 * @param y Text origin y in pixels (@c textY).
 */
void setDialogTextOrigin(s32 idx, s32 x, s32 y) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->textX = x;
    entry->textY = y;
}


/**
 * @brief Set a dialog's choice lines.
 * @param idx Dialog index.
 * @param first First choice line (@c firstChoice).
 * @param last Last choice line (@c lastChoice).
 * @param cancel Line Triangle moves the cursor to, negative for none (@c cancelChoice).
 */
void setDialogChoices(s32 idx, s32 first, s32 last, s32 cancel) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->firstChoice = first;
    entry->lastChoice = last;
    entry->ctrl.bits.cancelChoice = cancel;
}


/**
 * @brief Put a dialog's choice cursor on a line (@c choiceCursor).
 * @param idx Dialog index.
 * @param val Choice line.
 */
void setDialogChoiceCursor(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->choiceCursor = val;
}


/**
 * @brief Set a dialog's draw hook.
 * @param idx Dialog index.
 * @param val Hook to run, or NULL for none.
 */
void setDialogDrawCallback(s32 idx, DialogDrawCallback val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->drawCallback = val;
}


/**
 * @brief Set a dialog's per-frame update hook.
 * @param idx Dialog index.
 * @param val Hook to run, or NULL for none.
 */
void setDialogUpdateCallback(s32 idx, DialogCallback val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->updateCallback = val;
}


/**
 * @brief Set a message window's brightness and copy it to the window's display entity.
 * @param idx Dialog index.
 * @param val Brightness (0x1000 = full).
 * @return Nothing meaningful. It is int, not void: the field script's MESMODE command
 * (fe_object9) only matches with an int-returning declaration in scope.
 */
s32 setDialogBrightness(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->brightness = val;
    setBattleEntityBrightness(entry->entityIdx, val);
}


/**
 * @brief Refresh the colours that blinking text is tinted with.
 *
 * Text colours 8-15 are the blinking variants of colours 0-7: message text
 * takes its tint from @c state.textBlinkTint instead of @c state.textTint, and menu
 * text from @c g_menuTint[MENU_TINT_BLINK] instead of @c g_menuTint[MENU_TINT_NORMAL]. While
 * @ref DIALOG_TEXT_BLINK_DIM is set in @c state.textBlinkClock each copy is a grey at
 * 75% of its base's red channel (the brightness setters only ever store greys);
 * otherwise it equals the base. The clock steps once per frame, so the text blinks
 * between full and 75% brightness every 16 frames.
 */
void updateTextBlinkColors(void) {
    u32 textBlinkTint = g_dialogs.state.textTint;
    u8 blinkClock = g_dialogs.state.textBlinkClock;
    u32 menuBlinkTint = g_menuTint[MENU_TINT_NORMAL];

    if (blinkClock & DIALOG_TEXT_BLINK_DIM) {
        textBlinkTint &= 0xFF;
        textBlinkTint = (textBlinkTint * 3) >> 2;
        textBlinkTint = textBlinkTint | ((textBlinkTint << 16) | (textBlinkTint << 8));
        textBlinkTint |= SPRT_CODE;

        menuBlinkTint &= 0xFF;
        menuBlinkTint = (menuBlinkTint * 3) >> 2;
        menuBlinkTint = menuBlinkTint | ((menuBlinkTint << 16) | (menuBlinkTint << 8));
        menuBlinkTint |= SPRT_CODE;
    }

    g_dialogs.state.textBlinkTint = textBlinkTint;
    g_menuTint[MENU_TINT_BLINK] = menuBlinkTint;
}


/**
 * @brief Step the text blink clock by one frame.
 *
 * Counts @c state.textBlinkClock down and refreshes the blink colours
 * (updateTextBlinkColors).
 */
void tickTextBlink(void) {
    g_dialogs.state.textBlinkClock--;
    updateTextBlinkColors();
}


/**
 * @brief Set the text brightness: store a grey in @c state.textTint and refresh the blink copies.
 * @param brightness Brightness, @ref BRIGHTNESS_NORMAL = normal; divided by 32, its
 * low 8 bits become r, g and b (0x80 draws the font unmodulated).
 */
void setTextBrightness(s32 brightness) {
    brightness /= 32;
    brightness &= 0xFF;
    brightness |= (brightness << 16) | (brightness << 8);
    brightness |= SPRT_CODE;
    g_dialogs.state.textTint = brightness;
    updateTextBlinkColors();
}


/**
 * @brief Apply a message window's @c brightness (0x1000 = normal) to its text and next-page marker.
 *
 * Sets the text brightness (setTextBrightness) and the tint of the window's
 * next-page marker (setNextPageMarkerBrightness).
 *
 * @param index Dialog index.
 */
static void applyWindowBrightness(s32 index) {
    Dialog *entry = &g_dialogs.entries[index];
    s32 val = entry->brightness;
    setTextBrightness(val);
    setNextPageMarkerBrightness(val);
}


/**
 * @brief Set the linked battle entity index on a dialog.
 * @param idx Dialog index.
 * @param val Battle entity index.
 */
void setDialogEntityIndex(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->entityIdx = val;
}


/**
 * @brief Swap the state of a dialog, returning the old value.
 * @param idx Dialog index.
 * @param val New state value.
 * @return Previous state value.
 */
s32 swapDialogState(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    s32 old = entry->state;
    entry->state = val;
    return old;
}


/**
 * @brief Get the state of a dialog.
 * @param idx Dialog index.
 * @return State value (0 = inactive, 1 = active).
 */
s32 getDialogState(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];
    return entry->state;
}


/**
 * @brief Set a dialog's text speed and restart its character timer (@c charTimer).
 * @param idx Dialog index.
 * @param val Text speed: added to the timer each frame (0x1000 = a character a frame).
 */
void setDialogTextSpeed(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->textSpeed = val;
    entry->charTimer = 0;
}


/**
 * @brief Set a dialog's open scale.
 * @param idx Dialog index.
 * @param val Open scale: 0 shut, 0x1000 fully open.
 */
void setOpenDialogScale(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->openDialogScale = val;
}


/**
 * @brief Set a dialog's open step, added to its open scale each frame.
 * @param idx Dialog index.
 * @param val Open step; negative closes.
 */
void setOpenDialogStep(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->openDialogStep = val;
}


/**
 * @brief Get a dialog's open scale.
 * @param idx Dialog index.
 * @return The open scale: 0 once shut, 0x1000 once fully open.
 */
s32 getOpenDialogScale(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];
    return entry->openDialogScale;
}


/**
 * @brief Draw the blinking next-page marker of a message window.
 *
 * While the entry's marker bit is set and its blink counter is in the visible
 * half of its cycle, emits icon @ref ICON_NEXT_PAGE_MARKER of @c g_iconTable
 * @ref NEXT_PAGE_MARKER_INSET pixels in from the window's bottom-right
 * corner, tinted with @c g_engine.nextPageMarkerColor. Every cell of the icon becomes one
 * @c TSPRT taken from the display-list packet buffer and linked into @p ot;
 * the advanced packet cursor is stored back afterwards.
 *
 * Per cell: the u/v/CLUT word is the cell's own plus the font CLUT; the
 * texture page is the font page with the cell's blend rate; the colour word
 * gets the cell's semi-transparency bit and is forced to a SPRT code;
 * width/height are copied and the cell's signed offsets are added to the
 * position.
 *
 * @note Same emitter as @ref drawTextIcon plus the colour masking, with the
 * same two load-bearing spellings. The @c (u8) narrowing of the blend
 * rate is a no-op on the value (at most 0x60) but hides its range from
 * the compiler, which otherwise proves @c _get_mode's 0x9FF mask
 * redundant and drops it; the original keeps it. The colour word is
 * built one operation per statement because the original computes the
 * whole chain in the result's own register, which a single expression
 * does not. @c head is handed to @c p and taken back after the loop,
 * the way btl_anim.c threads its packet cursor.
 *
 * @param ot Ordering-table slot the sprites are linked into.
 * @param entry Dialog the marker belongs to.
 */
static void drawNextPageMarker(P_TAG *ot, Dialog *entry) {
    IconTable *table;
    IconCell *cell;
    TSPRT *p;
    s32 head;
    u32 flags;
    u32 word;
    s32 tpage;
    u32 val;
    u32 color;
    s32 n;
    s32 x;
    s32 y;

    /* On the bitfields gcc folds these two tests into one masked compare, or
     * extracts the marker bit with a shift; only the whole word gives the
     * original and/branch pair. */
    flags = entry->ctrl.raw;
    if (!(flags & DIALOG_CTRL_NEXT_PAGE_MARKER) ||
        ((flags >> DIALOG_CTRL_NEXT_PAGE_MARKER_BLINK_SHIFT) & DIALOG_NEXT_PAGE_MARKER_BLINK_OFF)) {
        return;
    }

    table = &g_iconTable;
    head = getDisplayListHead();
    cell = (IconCell *)table; /* seeding the cursor from its own copy keeps the offset add in the delay slot */
    p = (TSPRT *)head;
    word = table->descriptors[ICON_NEXT_PAGE_MARKER];
    n = word >> 16;
    word &= 0xFFFF;
    cell = (IconCell *)((u8 *)cell + word);
    x = entry->rect.w - NEXT_PAGE_MARKER_INSET;
    y = entry->rect.h - NEXT_PAGE_MARKER_INSET;
    color = g_engine.nextPageMarkerColor;

    for (; n > 0; p++, cell++, n--) {
        word = cell->texInfo;
        val = word & ICON_UVCLUT_MASK;
        val += getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16;
        setIconUVClut(p, val);

        val = (word >> ICON_ABR_SHIFT) & ICON_ABR_MASK;
        val = (u8)getTPage(0, val, 0, 0);
        tpage = val;
        tpage |= getTPage(0, 0, ICON_TPAGE_X, ICON_TPAGE_Y);

        val = word >> ICON_ABE_SHIFT;
        val &= SPRT_CODE_ABE;
        val <<= SPRT_CODE_SHIFT;
        val |= color;
        setTSprt(p, 1, 0, tpage);
        val &= SPRT_RGB_MASK;
        val |= SPRT_CODE;
        setIconRGBC(p, val);

        word = cell->metrics;
        val = word & ICON_WH_MASK;
        setIconWH(p, val);
        val = (s8)(word >> 24); /* signed Y offset, byte 3 */
        word <<= 16;
        word = (s8)(word >> 24); /* signed X offset, byte 1 */
        setXY0(p, x + word, y + val);

        addPrimFast(ot, p);
    }
    head = (s32)p;
    storeGpuPacket(head);
}


/**
 * @brief Draw a message window's choice cursor, next-page marker and text.
 *
 * Once the whole message has been typed (@c typingDone), icon
 * @ref ICON_CHOICE_CURSOR goes in front of the selected line @c choiceCursor, grey
 * at the window's @c brightness (0x1000 = full), followed by a
 * draw-area packet for the window entity's clipped bound rect. Then the next-page
 * marker (drawNextPageMarker), the text (drawDialogText) and a draw-mode packet that
 * resets the texture page. A window without a message draws nothing.
 *
 * @param index Dialog index.
 * @param ot Ordering table the packets are linked into.
 */
static void drawDialogContents(s32 index, P_TAG *ot) {
    Dialog *entry = &g_dialogs.entries[index];
    IconTable *table;
    DR_AREA *area;
    DR_TPAGE *tpage;
    BattleDisplayEntity *ent;
    s32 brightness;
    s32 colour;
    s32 y;
    s32 val;
    s32 line;

    if (entry->dataPtr == NULL) {
        return;
    }
    table = &g_iconTable;
    brightness = entry->brightness;
    if (entry->firstChoice != DIALOG_NO_CHOICE) {
        if (entry->typingDone == 1) {
            /* val holds the entity index here and the mode word below: two
             * variables swap the registers of the closing packet. */
            val = entry->entityIdx;
            ent = getBattleEntity(val);
            line = entry->choiceCursor;
            y = line * DIALOG_LINE_HEIGHT + DIALOG_CURSOR_Y;
            area = (DR_AREA *)getDisplayListHead();
            if (table != NULL) { /* the original tests the fixed table address */
                colour = brightness / 32;
                colour = SPRT_CODE | (colour << 16) | (colour << 8) | colour;
                area = drawIcon(ot, area, ICON_CHOICE_CURSOR, DIALOG_CURSOR_X, y, colour);
            }
            SetDrawArea(area, &ent->clipBound.rect);
            addPrimFast(ot, area);
            storeGpuPacket((u32)(area + 1));
        }
    }
    drawNextPageMarker(ot, entry);
    drawDialogText(ot, entry);
    tpage = (DR_TPAGE *)getDisplayListHead();
    setlen(tpage, 1);
    val = _get_mode(1, 0, 0);
    tpage->code[0] = val;
    addPrimFast(ot, tpage);
    storeGpuPacket((u32)(tpage + 1));
}


/**
 * @brief Scale a dialog rectangle about its center by a fixed-point factor.
 *
 * @p scale is a signed Q12 fixed-point multiplier where @c 0x1000 (4096 == ONE)
 * is 1.0; at exactly 1.0 the rectangle is left unchanged. Otherwise each axis is
 * recentered: the origin moves to @c center @c - @c (dimension @c * @c scale) @c /
 * @c 8192 (unscale the Q12 product, then halve) and the dimension becomes
 * @c (dimension @c * @c scale) @c / @c 4096.
 *
 * Written in the in-place register-reuse style of the sibling RECT helpers
 * (cf. @ref func_8002B080, and @c func_800A3398 which scales a RECT the same
 * way): the packed @c s16 pairs are read once as words and each value is
 * progressively transformed in its own variable, which keeps gcc 2.7.2 from
 * spilling copies. The @c do/while(0) wraps the arithmetic as a single basic
 * block so the compiler emits both extractions before the multiplies rather
 * than interleaving them.
 *
 * @param rect Rectangle to scale in place.
 * @param scale Q12 fixed-point scale factor (@c 0x1000 == 1.0).
 * @param arg2 Unused by this routine (the caller passes @c openDialogStep).
 */
static void scaleDialogRect(RECT *rect, s32 scale, s32 arg2) {
    s32 x, y, w, h, prodW, prodH;

    if (scale == 0x1000) {
        return;
    }

    x = *(s32 *)&rect->x;
    y = x >> 16;
    x = x << 16;
    x = x >> 16;
    w = *(s32 *)&rect->w;
    h = w >> 16;
    w = w << 16;
    do {
        w = w >> 16;
        x = x + ((u32)w / 2);
        y = y + ((u32)h / 2);
        prodW = w * scale;
        prodH = h * scale;
        w = (u32)prodW / 4096;
        rect->x = x - ((u32)prodW / 8192);
        rect->w = w;
        h = (u32)prodH / 4096;
        rect->y = y - ((u32)prodH / 8192);
        rect->h = h;
    } while (0);
}


/**
 * @brief Give a dialog the input focus (@c state.focusedDialog): only the focused
 * dialog reads the pad, to turn pages and pick a choice.
 * @param idx Dialog index, or -1 for none.
 */
void setFocusedDialog(s32 idx) {
    g_dialogs.state.focusedDialog = idx;
}


/**
 * @brief Get the dialog with the input focus (@c state.focusedDialog).
 * @return Dialog index, or -1 for none.
 */
s32 getFocusedDialog(void) {
    return g_dialogs.state.focusedDialog;
}


/**
 * @brief Get the choice the player confirmed in a dialog.
 *
 * Returns -1 until the player has confirmed (@c choiceMade), then the selected
 * line (@c choiceCursor) minus the first choice line (@c firstChoice).
 *
 * @param idx Dialog index.
 * @return The chosen answer (0 = the first choice), or -1.
 */
s32 getDialogChoice(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];
    Dialog *e2;
    s32 always;
    e2 = entry;
    always = 1; /* Regalloc */
    if (always) {
        if (entry->choiceMade == 0) {
            return -1;
        }
        return e2->choiceCursor - e2->firstChoice;
    }
}


/**
 * @brief Keyboard-style auto-repeat for one pad channel's edge bits.
 *
 * Latches @p newVal into @c sys->state.repeatLatched[channel] and, using the per-channel
 * mask, decides whether the (masked) edge bits should fire this frame. If the new
 * and previous masked bits overlap (a held cue) it ticks the per-channel countdown
 * @c sys->state.repeatCounters[channel]: while it is running the event is suppressed
 * (returns 0), and when it expires the countdown reloads to the repeat interval and
 * fires. When the bits do not overlap the countdown resets to the restart delay and
 * fires. Mirrors @c func_800A29D4 (the Triple Triad edge auto-repeat).
 *
 * @param engine Engine state; @c repeatDelays packs the two delays.
 * @param port Pad port; @c autoRepeatButtonMasks[channel] is the channel's button mask.
 * @param sys Dialog system (@c &engine->dialogs); holds the stored bits and counters.
 * @param newVal Raw new edge bitmask for this frame.
 * @param channel Pad channel index, 0..3.
 * @return The masked edge bits that should fire this frame, or 0 while suppressed.
 */
static s32 autoRepeatPadChannel(EngineState *engine, GamePad *port, DialogSystem *sys, u16 newVal, s32 channel) {
    s32 counter;
    s32 restartDelay;
    s32 repeatInterval;
    u16 mask;
    u16 prevMasked;

    prevMasked = sys->state.repeatLatched[channel];
    sys->state.repeatLatched[channel] = newVal;
    restartDelay = engine->repeatDelays.hword;
    counter = sys->state.repeatCounters[channel];
    mask = port->autoRepeatButtonMasks[channel];

    repeatInterval = restartDelay >> 8;
    restartDelay &= 0xFF;
    newVal &= mask;
    prevMasked &= mask;
    if (newVal & prevMasked) {
        if (newVal != prevMasked) {
            counter = restartDelay;
        }
        counter--;
        if (counter < 0) {
            counter = repeatInterval;
        } else {
            newVal = 0;
        }
    } else {
        counter = restartDelay;
    }
    sys->state.repeatCounters[channel] = counter;
    return newVal;
}


/**
 * @brief Auto-repeat the four channels of this frame's pad bits.
 *
 * Runs autoRepeatPadChannel on @p input for channels 0-3 against the port linked
 * to port 0, and ORs the bits that fire.
 *
 * @param input Pad bits of this frame.
 * @return The bits that fire this frame.
 */
s32 autoRepeatPad(s32 input) {
    DialogSystem *sys = &g_engine.dialogs;
    EngineState *engine = &g_engine;
    u16 bits = input;
    GamePad *port = &engine->gamePads[engine->gamePads[0].inputSourceIndex];
    u16 result;

    result = 0;
    result |= autoRepeatPadChannel(engine, port, sys, bits, 0);
    result |= autoRepeatPadChannel(engine, port, sys, bits, 1);
    result |= autoRepeatPadChannel(engine, port, sys, bits, 2);
    result |= autoRepeatPadChannel(engine, port, sys, bits, 3);
    return result;
}


/**
 * @brief Step a dialog's open/close animation and size its entity to it.
 *
 * Adds @c openDialogStep to @c openDialogScale, clamped to 0..ONE (1.0).
 * While the dialog is open, its entity is enabled and its bound rect is the
 * dialog rect scaled by @c openDialogScale, with the clamp rect
 * @ref DIALOG_CLAMP_INSET pixels inside that. At 0 the entity is disabled.
 *
 * @param index Dialog index.
 */
static inline void updateOpenDialogScale(s32 index) {
    Dialog *entry = &g_dialogs.entries[index];
    s32 scale;
    s32 entityId;

    scale = entry->openDialogScale;
    scale += entry->openDialogStep;
    scale = CLAMP(scale, 0, ONE);
    entityId = entry->entityIdx;
    entry->openDialogScale = scale;

    if (scale != 0) {
        RECT rect;

        setBattleEntityField35(entityId, 1);
        rect = entry->rect;
        scaleDialogRect(&rect, scale, entry->openDialogStep);
        setBattleEntityBoundRect(entityId, &rect);
        rect.x += DIALOG_CLAMP_INSET;
        rect.y += DIALOG_CLAMP_INSET;
        rect.w -= DIALOG_CLAMP_INSET * 2;
        rect.h -= DIALOG_CLAMP_INSET * 2;
        setBattleEntityRectClamp(entityId, &rect);
    } else {
        setBattleEntityField35(entityId, 0);
        entry->state = 0;
    }
}

/**
 * @brief Per-frame update of one message window: open/close step, then run its text.
 *
 * After the open/close step (updateOpenDialogScale), a fully open window with a message runs
 * the @c DIALOG_SEQ_* state machine in @c seqState. The message is decoded into
 * a @ref DIALOG_MSG_BUF_SIZE-byte buffer taken from the $gp area: characters appear at the text
 * speed (@c textSpeed added to the timer @c charTimer each frame), full
 * windows scroll, page breaks wait for Cross or Square, and at the end a
 * message with choices lets the player move the cursor @c choiceCursor between
 * @c firstChoice and @c lastChoice. Cross or Square confirms (@c choiceMade = 1);
 * Triangle jumps to the cancel choice in @c ctrl.bits.cancelChoice, if one is set.
 *
 * Only the focused window (@c state.focusedDialog) reads the buttons.
 *
 * @param index Dialog index.
 * @param input Pressed buttons: bits 0-15 drive paging, bits 16-31 the choice
 * (the split is a guess from the tests made on each half).
 * @param repeat Auto-repeating buttons; Up and Down move the choice cursor.
 */
static void updateDialog(s32 index, u32 input, u32 repeat) {
    Dialog *entry;
    s32 *seqState;
    u8 *msgBuf;
    u32 pressed;
    u32 choicePressed;
    s32 state;
    s32 ch;
    s32 cmd;

    updateOpenDialogScale(index);

    entry = &g_dialogs.entries[index];
    if (g_dialogs.state.focusedDialog == index) {
        pressed = input & 0xFFFF;
        choicePressed = input >> 16;
    } else {
        pressed = 0;
        repeat = 0;
        choicePressed = 0;
    }

    if (entry->dataPtr != NULL && entry->openDialogScale >= ONE) {
        GP_ALLOC(msgBuf, DIALOG_MSG_BUF_SIZE);
        seqState = &entry->seqState;
        state = *seqState;
        /* Re-entering the switch is a goto, not a while/for: gcc treats a C
         * loop as one and hoists the constants 1 and 7 into saved registers,
         * which the original does not (93.99% with while (1) and continue). */
    dispatch:
        switch (state) {
        case DIALOG_SEQ_START:
            g_dialogs.state.repeatLatched[0] = 0;
            g_dialogs.state.repeatLatched[1] = 0;
            g_dialogs.state.repeatCounters[0] = 0;
            g_dialogs.state.repeatCounters[1] = 0;
            *seqState = DIALOG_SEQ_TICK;
            /* fallthrough */
        case DIALOG_SEQ_TICK:
            entry->charTimer += entry->textSpeed;
            /* fallthrough */
        case DIALOG_SEQ_PRINT:
            if (entry->textSpeed == 0 || entry->charTimer >= ONE) {
                entry->charTimer -= ONE;
                decodeMessageDirect(entry, msgBuf);
                state = DIALOG_SEQ_NEXT_CHAR;
                goto dispatch;
            }
            break;

        case DIALOG_SEQ_NEXT_CHAR:
            ch = msgBuf[entry->typedChars];
            if (ch == MSG_NEW_PAGE || ch == MSG_NEW_PAGE_MARKED) {
                state = DIALOG_SEQ_PAGE;
                goto dispatch;
            }
            ch = nextDialogChar(entry, msgBuf);
            cmd = ch >> 8;
            ch &= 0xFF;
            if (ch == MSG_NEWLINE) {
                state = DIALOG_SEQ_NEWLINE;
                goto dispatch;
            }
            if (ch == MSG_END) {
                state = DIALOG_SEQ_END;
                goto dispatch;
            }
            if (cmd == MSG_CMD_SPEED) {
                s32 speed = ch - MSG_ARG_BASE;

                if (speed == 0) {
                    speed = ONE;
                } else if (speed == 1) {
                    speed = 0;
                } else {
                    speed = ONE / (ch - (MSG_ARG_BASE + 1));
                }
                entry->textSpeed = speed;
            } else if (cmd == MSG_CMD_WAIT) {
                /* Through an s32 local: assigned straight to the u8 field,
                 * gcc folds the (u8) away and loses the andi. */
                s32 frames = (u8)ch - MSG_ARG_BASE;

                entry->waitTimer = frames;
                state = DIALOG_SEQ_WAIT_START;
                goto dispatch;
            } else if (cmd == MSG_CMD_COLOR) {
                entry->color = ch;
                state = DIALOG_SEQ_NEXT_CHAR;
                goto dispatch;
            } else {
                state = DIALOG_SEQ_PRINT;
                if (entry->textSpeed == 0) {
                    state = DIALOG_SEQ_NEXT_CHAR;
                }
                goto dispatch;
            }
            state = DIALOG_SEQ_NEXT_CHAR;
            goto dispatch;

        case DIALOG_SEQ_NEWLINE:
            state = DIALOG_SEQ_NEXT_CHAR;
            if (entry->typingRow >= entry->visibleRows) {
                entry->charTimer += ONE;
                entry->typingRow--;
                *seqState = DIALOG_SEQ_SCROLL;
                state = DIALOG_SEQ_SCROLL;
            }
            goto dispatch;

        case DIALOG_SEQ_SCROLL:
            entry->scrollY++;
            if (entry->scrollY % DIALOG_LINE_HEIGHT == 0) {
                *seqState = DIALOG_SEQ_TICK;
            }
            break;

        case DIALOG_SEQ_PAGE:
            if (ch == MSG_NEW_PAGE_MARKED) {
                entry->ctrl.bits.nextPageMarker = 1;
                entry->ctrl.bits.nextPageMarkerBlink = 0;
            } else {
                entry->ctrl.bits.nextPageMarker = 0;
            }
            *seqState = DIALOG_SEQ_PAGE_RELEASE;
            break;

        case DIALOG_SEQ_PAGE_RELEASE:
            entry->ctrl.bits.nextPageMarkerBlink++;
            pressed &= ~(PADLup | PADLright | PADLdown | PADLleft);
            if (pressed == 0) {
                *seqState = DIALOG_SEQ_PAGE_WAIT;
            }
            break;

        case DIALOG_SEQ_PAGE_WAIT:
            entry->ctrl.bits.nextPageMarkerBlink++;
            if (pressed & (PADRdown | PADRleft)) {
                entry->ctrl.bits.nextPageMarker = 0;
                entry->pageColor = entry->color;
                decodeMessageDirect(entry, msgBuf);
                nextDialogChar(entry, msgBuf);
                decodeMessageDirect(entry, msgBuf);
                *seqState = DIALOG_SEQ_TICK;
            }
            break;

        case DIALOG_SEQ_END:
            entry->typingDone = 1;
            state = DIALOG_SEQ_CHOICE_START;
            if (entry->firstChoice == DIALOG_NO_CHOICE) {
                *seqState = DIALOG_SEQ_DONE;
                state = DIALOG_SEQ_DONE;
            }
            goto dispatch;

        case DIALOG_SEQ_CHOICE_START:
            *seqState = DIALOG_SEQ_CHOICE_RELEASE;
            state = DIALOG_SEQ_CHOICE_RELEASE;
            goto dispatch;

        case DIALOG_SEQ_CHOICE_RELEASE:
            if (pressed == 0) {
                *seqState = DIALOG_SEQ_CHOICE;
            }
            break;

        case DIALOG_SEQ_WAIT_START:
            *seqState = DIALOG_SEQ_WAIT;
            /* fallthrough */
        case DIALOG_SEQ_WAIT:
            if (entry->waitTimer == 0) {
                *seqState = DIALOG_SEQ_TICK;
                state = DIALOG_SEQ_PRINT;
                goto dispatch;
            }
            entry->waitTimer--;
            break;

        case DIALOG_SEQ_CHOICE:
            if (choicePressed & (PADRdown | PADRleft)) {
                entry->choiceMade = 1;
                sendSpuCommand(2);
                *seqState = DIALOG_SEQ_DONE;
                state = DIALOG_SEQ_DONE;
                goto dispatch;
            }
            if (choicePressed & PADRup) {
                s8 cancel = entry->ctrl.bits.cancelChoice;
                s32 cancelChoice;

                if (cancel >= 0) {
                    cancelChoice = cancel;
                    sendSpuCommand(3);
                }
                /* With no cancel choice this clamps whatever cancelChoice
                 * held; the original reads the register uninitialised too. */
                entry->choiceCursor = CLAMP(cancelChoice, entry->firstChoice, entry->lastChoice);
            } else if (repeat & PADLup) {
                s32 cursor;
                s32 prev;

                cursor = entry->choiceCursor;
                prev = cursor;
                cursor--;
                if (cursor < entry->firstChoice) {
                    cursor = entry->lastChoice;
                }
                if (prev != cursor) {
                    sendSpuCommand(1);
                }
                entry->choiceCursor = cursor;
            } else if (repeat & PADLdown) {
                s32 cursor;
                s32 prev;

                cursor = entry->choiceCursor;
                prev = cursor;
                cursor++;
                if (entry->lastChoice < cursor) {
                    cursor = entry->firstChoice;
                }
                if (prev != cursor) {
                    sendSpuCommand(1);
                }
                entry->choiceCursor = cursor;
            }
            break;

        case DIALOG_SEQ_DPAD_RELEASE_START:
            *seqState = DIALOG_SEQ_DPAD_RELEASE;
            /* fallthrough */
        case DIALOG_SEQ_DPAD_RELEASE:
            if (!(pressed & (PADLup | PADLright | PADLdown | PADLleft))) {
                *seqState = DIALOG_SEQ_CHOICE_START;
            }
            break;
        }
        GP_FREE(DIALOG_MSG_BUF_SIZE);
    }
}


/**
 * @brief Start a message on a dialog.
 *
 * Clears fields, stores the data pointer, starts both text colours at white,
 * calls resetDialogTyping, and marks the message as offering no choice.
 *
 * @param index Dialog index.
 * @param data The message (or NULL).
 */
void setDialogMessage(s32 index, u8 *data) {
    Dialog *entry = &g_dialogs.entries[index];

    entry->typingDone = 0;
    entry->dataPtr = data;
    entry->linePtr = data;
    entry->scrollY = 0;
    entry->seqState = DIALOG_SEQ_START;
    entry->pageColor = DIALOG_COLOR_WHITE;
    entry->color = entry->pageColor;

    resetDialogTyping(entry);

    entry->firstChoice = DIALOG_NO_CHOICE;
    entry->lastChoice = DIALOG_NO_CHOICE;
    entry->choiceMade = 0;
}


/**
 * @brief Skips past a given number of null-terminated strings, then calls setDialogMessage.
 *
 * Advances @p a1 past @p a2 null-terminated strings by scanning bytes until
 * null is found for each string. After skipping, calls setDialogMessage with
 * the original @p a0 and @p a1.
 *
 * @param index Dialog index.
 * @param data Pointer to the start of the string data.
 * @param count Number of strings to skip.
 */
void setDialogMessageAfterStrings(s32 index, u8 *data, s32 count) {
    while (count > 0) {
        while (*data++ != 0) {
        }
        count--;
    }
    setDialogMessage(index, data);
}


/**
 * @brief Start a message with choices on a dialog.
 *
 * Lines @p min to @p max of the message are the choices; the cursor starts on
 * @p val, clamped to them, and Triangle jumps to the choice @p arg5.
 *
 * @param arg0 Dialog index.
 * @param data The message.
 * @param min First choice line.
 * @param max Last choice line.
 * @param val Line the cursor starts on.
 * @param arg5 Cancel choice (@c ctrl.bits.cancelChoice).
 */
void setDialogChoiceMessage(s32 arg0, u8 *data, s32 min, s32 max, s32 val, s32 arg5) {
    val = CLAMP(val, min, max);

    setDialogMessage(arg0, data);
    setDialogChoices(arg0, min, max, arg5);
    setDialogChoiceCursor(arg0, val);
}


/**
 * @brief Start a message with choices on a dialog, after skipping @p count
 * null-terminated strings of @p str (see setDialogChoiceMessage).
 *
 * @param arg0 Dialog index.
 * @param str The strings; the message is the one after the first @p count.
 * @param count Strings to skip.
 * @param min First choice line.
 * @param max Last choice line.
 * @param val Line the cursor starts on.
 * @param arg6 Cancel choice (@c ctrl.bits.cancelChoice).
 */
void setDialogChoiceMessageAfterStrings(s32 arg0, u8 *str, s32 count, s32 min, s32 max, s32 val, s32 arg6) {
    s32 clamped;
    while (count > 0) {
        while (*str++) { }
        count--;
    }

    clamped = val;
    clamped = CLAMP(clamped, min, max);

    setDialogMessage(arg0, str);
    setDialogChoices(arg0, min, max, arg6);
    setDialogChoiceCursor(arg0, clamped);
}


/**
 * @brief Draw an active message window.
 *
 * If the entry is active (state != 0), switches GP to scratchpad,
 * runs the entry's @c drawCallback if set, applies the window's brightness
 * to its text and marker (applyWindowBrightness), and draws the window's cursor,
 * marker and text via drawDialogContents. Restores GP before returning.
 *
 * @param ot Ordering table, passed to the hook and drawDialogContents.
 * @param index Dialog index.
 */
static void drawDialog(P_TAG *ot, s32 index) {
    Dialog *entry = &g_dialogs.entries[index];

    if (entry->state != 0) {
        s32 savedGp;
        s16 saved;
        s32 ret;

        GP_SAVE_SCRATCH(savedGp);
        /* Narrowing the saved GP to s16 here (it never leaves a register, so no
         * truncation happens) forces the same GP-save register routing the
         * original compiler emitted. */
        saved = savedGp;
        if (entry->drawCallback != NULL) {
            entry->drawCallback(entry, ot);
        }
        applyWindowBrightness(index);
        drawDialogContents(index, ot);
        GP_RESTORE_RET(saved, ret);
    }
}


/**
 * @brief Render callback of a message window's battle entity.
 *
 * Draws the window contents (drawDialog) when the clamp rect has room for
 * a glyph, then links a draw area for the entity's clamp rect and a draw
 * offset at the window's inner corner (@ref DIALOG_CLAMP_INSET in from @c rect).
 * Links prepend to the OT, so the GPU runs these before the contents.
 * When the linked entity has both @ref BATTLE_ENTITY_BOX and
 * @ref BATTLE_ENTITY_DIALOG, the window also gets its @c cornerIcon (if
 * non-zero) at the window origin and two bars, grey at the entity's @c brightness
 * (the second one halved and semi-transparent for a
 * @ref BATTLE_ENTITY_SEMI_TRANS entity), then a draw area for the bound rect
 * and a draw offset back at the clip origin.
 *
 * @param ot Ordering table.
 * @param entity The window's battle entity.
 * @param pkt Packet cursor, stored back before drawing.
 * @return The packet cursor after the last packet.
 */
static void *renderDialogEntity(void *ot, BattleDisplayEntity *entity, void *pkt) {
    Dialog *entry;
    RECT *winRect;
    BattleDisplayEntity *ent;
    DR_AREA *p;
    DR_OFFSET *offset;
    RECT r;
    u32 head;
    u32 colour;
    u32 code;
    s32 index;
    s32 n;
    s32 y;

    index = entity->subFields[0];
    storeGpuPacket((u32)pkt); /* storeGpuPacket takes the cursor as an address value */
    if (entity->clipClamp.rect.w >= TEXT_GLYPH_SIZE && entity->clipClamp.rect.h >= TEXT_GLYPH_SIZE) {
        drawDialog(ot, index);
    }
    p = (DR_AREA *)getDisplayListHead();
    index = entity->subFields[0];
    entry = &g_dialogs.entries[index];
    winRect = &entry->rect;
    getAddrNewFast(ot, head);

    SetDrawArea(p, &entity->clipClamp.rect);
    head = linkPacket(head, p);
    p++;

    offset = (DR_OFFSET *)p;
    copyDisplayRect(&r);
    r.x += entry->rect.x;
    r.y += entry->rect.y;
    r.x += DIALOG_CLAMP_INSET;
    r.y += DIALOG_CLAMP_INSET;
    SetDrawOffset(offset, (u16 *)&r); /* r's x and y are the offset */
    head = linkPacket(head, offset);
    ent = getBattleEntity(entry->entityIdx);
    p = (DR_AREA *)(offset + 1);

    if ((ent->entityType & BATTLE_ENTITY_BOX) && (ent->entityType & BATTLE_ENTITY_DIALOG)) {
        colour = entity->brightness;
        colour >>= 5;
        code = SPRT_CODE;
        colour |= (colour << 16) | ((colour << 8) | code);
        setAddrFast(ot, head);
        n = entry->ctrl.bits.cornerIcon;
        if (n != 0) {
            p = drawIcon(ot, p, n, entry->rect.x, entry->rect.y, colour);
        }
        p = func_8002B898(ot, p, &entity->boundRect, colour);
        if (ent->entityType & BATTLE_ENTITY_SEMI_TRANS) {
            colour >>= 1;
            colour &= RGB_HALF_MASK;
            colour |= code;
            colour |= SPRT_ABE;
        }
        p = drawWindowBackground(ot, p, winRect, colour);
        getAddrNewFast(ot, head);

        SetDrawArea(p, &entity->clipBound.rect);
        head = linkPacket(head, p);
        p++;

        offset = (DR_OFFSET *)p;
        copyDisplayRect(&r);
        setlen(offset, 2);
        /* y is read before code[1] is cleared and x after: gcc keeps both
         * reads on their side of that store. */
        y = r.y & DR_OFFSET_COORD_MASK;
        offset->code[1] = 0;
        offset->code[0] = DR_OFFSET_CODE | (y << DR_OFFSET_Y_SHIFT) | (r.x & DR_OFFSET_COORD_MASK);
        head = linkPacket(head, offset);
        p = (DR_AREA *)(offset + 1);
    }
    setAddrFast(ot, head);
    return p;
}


/**
 * @brief Update callback of a message window's battle entity.
 *
 * With $gp on the scratchpad, runs the window's @c updateCallback, if any,
 * and its text (updateDialog) while the window is active.
 *
 * @param entity The window's battle entity.
 * @param input Pressed buttons.
 * @param repeat Auto-repeating buttons.
 * @return The scratchpad $gp.
 */
static u8 *updateDialogEntity(BattleDisplayEntity *entity, u32 input, u32 repeat) {
    u8 *tempGp;
    u8 *savedGp;
    u8 *ret;
    s32 index;

    GP_SAVE_SCRATCH(tempGp);
    index = entity->subFields[0];
    savedGp = tempGp;
    if (getDialogState(index) != 0) {
        Dialog *entry = &g_dialogs.entries[index];

        if (entry->updateCallback != NULL) {
            entry->updateCallback(entry, input, repeat);
        }
        updateDialog(index, input, repeat);
    }
    GP_RESTORE_RET(savedGp, ret);
    return ret;
}


/**
 * @brief Open a dialog: activate it and set its open step and mode.
 * @param idx Dialog index.
 * @param step Open step: 0x200 opens over 8 frames, 0x1000 at once.
 * @param mode Mode byte (@c ctrl.bits.mode).
 */
void openDialog(s32 idx, s32 step, s32 mode) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->state = 1;
    entry->openDialogStep = step;
    entry->ctrl.bits.mode = mode;
}


/** @brief Open a dialog with its animation: step 0x200, fully open after 8 frames. */
void openDialogAnimated(s32 idx) {
    openDialog(idx, 0x200, 0);
}


/** @brief Open a dialog at once: step 0x1000. */
void openDialogInstant(s32 idx) {
    openDialog(idx, 0x1000, 0);
}


/**
 * @brief Close a dialog by giving it a negative open step.
 * @param idx Dialog index.
 * @param step Open step: -0x200 closes over 8 frames, -0x1000 at once.
 */
void closeDialog(s32 idx, s32 step) {
    setOpenDialogStep(idx, step);
}


/** @brief Close a dialog with its animation: step -0x200, shut after 8 frames. */
void closeDialogAnimated(s32 idx) {
    closeDialog(idx, -0x200);
}


/** @brief Close a dialog at once: step -0x1000. */
void closeDialogInstant(s32 idx) {
    closeDialog(idx, -0x1000);
}


/**
 * @brief Set the anim speed of a dialog's linked entity: @p val + 3, clamped to [3, 11].
 *
 * Adds 3 to @p val, clamps to [3, 11], then calls setBattleEntityAnimSpeed with
 * the entry's entityIdx and the clamped value.
 *
 * @param idx Dialog index.
 * @param val Anim speed before the +3.
 */
void setDialogAnimSpeed(s32 idx, s32 val) {
    Dialog *entry;
    s32 clamped;
    val += 3;
    if (val >= 3) {
        clamped = 11;
        if (val < 12) {
            clamped = val;
        }
    } else {
        clamped = 3;
    }
    entry = &g_dialogs.entries[idx];
    setBattleEntityAnimSpeed(entry->entityIdx, clamped);
}


/**
 * @brief Get whether a dialog has typed its whole message.
 * @param idx Dialog index.
 * @return @c typingDone: 1 once the end of the message has been typed.
 */
s32 getDialogTypingDone(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];
    return entry->typingDone;
}


/**
 * @brief Set entity type flags on the battle entity linked to a dialog.
 *
 * Reads the entity index from the dialog, then sets entity type
 * (and derived draw mode) on that battle entity, adding @ref BATTLE_ENTITY_DIALOG.
 *
 * @param idx Dialog index.
 * @param val Entity type bits (BATTLE_ENTITY_*).
 */
void setDialogEntityType(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    setBattleEntityType(entry->entityIdx, val | BATTLE_ENTITY_DIALOG);
}


/**
 * @brief Read the entity type of the battle entity linked to a dialog.
 * @param idx Dialog index.
 * @return The entity type byte of the linked battle entity.
 */
s32 readDialogEntityType(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];
    return getBattleEntityType(entry->entityIdx);
}


/**
 * @brief Set the icon drawn at a dialog's top-left corner (@c cornerIcon).
 * @param idx Dialog index.
 * @param val Icon number, 0 for none.
 */
void setDialogCornerIcon(s32 idx, s32 val) {
    Dialog *entry = &g_dialogs.entries[idx];
    entry->ctrl.bits.cornerIcon = val;
}


/**
 * @brief Initialize a dialog to default values.
 *
 * Zeros out charTimer, choiceMade, cornerIcon, then configures defaults:
 * text speed = 0x1000, state = 0, anim speed = 3, open scale = 0, open step = 0,
 * entity flags = 6|8, display rect = (64,64,128,128), brightness = 0x1000.
 *
 * @param idx Dialog index.
 */
void initDialogSlot(s32 idx) {
    u8 *nullData = 0;
    Dialog *entry = &g_dialogs.entries[idx];
    entry->charTimer = 0;
    entry->choiceMade = 0;
    entry->ctrl.bits.cornerIcon = 0;
    setDialogMessage(idx, nullData);
    setDialogTextSpeed(idx, 0x1000);
    swapDialogState(idx, 0);
    setDialogAnimSpeed(idx, 3);
    setOpenDialogScale(idx, 0);
    setOpenDialogStep(idx, 0);
    setDialogEntityType(idx, 6);
    {
        RECT buf;
        buf.x = 0x40;
        buf.y = 0x40;
        buf.w = 0x80;
        buf.h = 0x80;
        setDialogRect(idx, &buf);
    }
    setDialogBrightness(idx, 0x1000);
}


/**
 * @brief Initialize a dialog slot: set active flag, callbacks, entity index, and clear fields.
 *
 * Activates the battle entity, assigns its render (renderDialogEntity) and update
 * (updateDialogEntity) callbacks, links the entity index, configures the sub-field, calls
 * initDialogSlot for default values, then clears sequence state and status bits.
 *
 * @param idx Dialog index.
 */
static void initDialog(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];

    setBattleEntityActive(idx, 1);
    entry->entityIdx = idx;
    setBattleEntityField04(idx, renderDialogEntity);
    setBattleEntityField00(idx, (s32)updateDialogEntity);
    setBattleEntityField35(idx, 0);
    setBattleEntitySubField(idx, 0, idx);
    initDialogSlot(idx);

    entry->seqState = DIALOG_SEQ_START;
    entry->textX = 0;
    entry->textY = 0;
    entry->ctrl.bits.nextPageMarker = 0;

    setDialogDrawCallback(idx, NULL);
    setDialogUpdateCallback(idx, NULL);
}


/**
 * @brief Copy a dialog's source rectangle to destination.
 * @param idx Dialog index.
 * @param dst Destination RECT.
 */
void getDialogRect(s32 idx, RECT *dst) {
    Dialog *entry = &g_dialogs.entries[idx];
    *dst = entry->rect;
}


/**
 * @brief Configure a dialog's rectangle and propagate it to its battle entity.
 *
 * Insets the source rect by @ref DIALOG_CLAMP_INSET on each side, sets visibleRows to
 * the number of text lines that fit (inset height / @ref DIALOG_LINE_HEIGHT,
 * minimum 1), copies the
 * original source rect into the entry, and forwards the original rect (offset by
 * the entry's stored offsets) to the entity's bound rect. The inset rect is then
 * passed to the entity's clamp rect.
 *
 * @param index Dialog index.
 * @param srcRect Source rectangle.
 */
void setDialogRect(s32 index, RECT *srcRect) {
    Dialog *entry = &g_dialogs.entries[index];
    RECT rect;
    s32 entityId;
    Dialog *ep = entry;

    entityId = ep->entityIdx;

    rect.x = srcRect->x;
    rect.y = srcRect->y;
    rect.w = srcRect->w;
    rect.h = srcRect->h;
    rect.x += DIALOG_CLAMP_INSET;
    rect.y += DIALOG_CLAMP_INSET;
    rect.w -= DIALOG_CLAMP_INSET * 2;
    rect.h -= DIALOG_CLAMP_INSET * 2;

    ep->visibleRows = rect.h / DIALOG_LINE_HEIGHT;
    if (ep->visibleRows == 0) {
        ep->visibleRows = 1;
    }

    ep->rect = *srcRect;
    rect = *srcRect;

    scaleDialogRect(&rect, ep->openDialogScale, ep->openDialogStep);
    setBattleEntityBoundRect(entityId, &rect);

    rect.x += DIALOG_CLAMP_INSET;
    rect.y += DIALOG_CLAMP_INSET;
    rect.w -= DIALOG_CLAMP_INSET * 2;
    rect.h -= DIALOG_CLAMP_INSET * 2;
    setBattleEntityRectClamp(entityId, &rect);
}


/**
 * @brief Store a numeric value into one of the 8 message-value slots.
 *
 * The index is clamped to [0, 7]. Used by message formatting to pass
 * numeric arguments into decodeMessage.
 *
 * @param index Slot index (clamped to 0..7).
 * @param value Value to store.
 */
void setMessageValue(s32 index, s32 value) {
    DialogSystem *data = &g_dialogs;
    s32 clamped = CLAMP(index, 0, 7);
    data->msgValues[clamped] = value;
}


/**
 * @brief Reset all dialogs and clear global dialog state.
 *
 * Marks the global state as inactive, reinitializes all 8 dialog slots,
 * clears the text blink clock and the pad auto-repeat state of channels 0 and 1,
 * then calls func_8002C130 to finalize.
 */
void resetAllDialogs(void) {
    DialogSystem *sys = &g_dialogs;
    s32 i;
    sys->state.focusedDialog = -1;
    for (i = 0; i < 8; i++) {
        initDialog(i);
    }
    /* Through the global, not sys: the original stores it with its own %hi/%lo pair. */
    g_dialogs.state.textBlinkClock = 0;
    sys->state.repeatLatched[0] = 0;
    sys->state.repeatLatched[1] = 0;
    sys->state.repeatCounters[0] = 0;
    sys->state.repeatCounters[1] = 0;
    func_8002C130();
}


/**
 * @brief Look up linked entity's animation speed and dispatch.
 *
 * Reads the entity index from the dialog, gets its animation speed,
 * then passes the result to getEntityTablePtr.
 *
 * @param idx Dialog index.
 */
void dispatchDialogAnimSpeed(s32 idx) {
    Dialog *entry = &g_dialogs.entries[idx];
    s32 val = getBattleEntityAnimSpeed(entry->entityIdx);
    getEntityTablePtr(val);
}
