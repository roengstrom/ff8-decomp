#include "common.h"
#include "battle.h"
#include "menu.h"
#include "menumain.h"
#include "ability_list.h"
#include "gamestate.h"
#include "game.h"
#include "gf.h"
#include "btl_anim.h"
#include "btl_color.h"
#include "btl_sfx.h"
#include "numstr.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "main.h"

/* Main-executable symbols without an owner header yet. */
extern s32 D_8008384C;
extern u16 D_80083850;
extern u16 g_configFlags;
extern u8 D_80077E6C[];
extern u8 D_80077EBC[];
extern u16 D_800780E8;
extern u8 D_80056290[];
extern u8 D_800562A4;
extern u8 D_80078D38[];
extern s32 D_8005F138;

/* ======================================================================== */
/* Panel/Window Rendering                                                   */
/* ======================================================================== */

/**
 * @brief Build a draw-area primitive from a caller rectangle and link it.
 *
 * Copies the (possibly unaligned) RECT at @p src, offsets its origin by the
 * active draw environment's clip origin, clamps width/height to at least 2,
 * packs the @c SetDrawArea GP0 command into @p prim, links it via
 * @c addPrimFast (hand-picked temp @c $s1), and returns the next packet slot.
 *
 * @note @p src is a byte pointer and the copy is a builtin memcpy — the
 *       original's lwl/lwr pairs come from the unaligned 8-byte expansion.
 *       The clip additions go through u16 views of the s16 fields.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @param src  Source RECT (unaligned byte pointer).
 * @return Cursor for the next primitive (@c prim + 1).
 */
DR_AREA *func_801EF800(P_TAG *ot, DR_AREA *prim, u8 *src) {
    RECT r;
    DRAWENV *env;

    memcpy(&r, src, sizeof(RECT));
    env = g_activeDrawEnv;
    *(u16 *)&r.x += *(u16 *)&env->clip.x;
    *(u16 *)&r.y += *(u16 *)&env->clip.y;
    if (r.w < 2) {
        r.w = 2;
    }
    if (r.h < 2) {
        r.h = 2;
    }
    SetDrawArea(prim, &r);
    addPrimFast(ot, prim, s1);
    return ++prim;
}

/**
 * @brief Build a draw-area primitive for the active draw environment's clip
 *        and link it.
 *
 * Same as func_801F03E8 but keeps the draw environment's own clip rectangle
 * (no size override): packs the @c SetDrawArea GP0 command for
 * @c g_activeDrawEnv->clip into @p prim, links it via @c addPrimFast
 * (hand-picked temp @c $s2), and returns the next packet slot.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @return Cursor for the next primitive (@c prim + 1).
 */
DR_AREA *func_801EF8D8(P_TAG *ot, DR_AREA *prim) {
    SetDrawArea(prim, &g_activeDrawEnv->clip);
    addPrimFast(ot, prim, s2);
    return ++prim;
}

/**
 * @brief Draw inner panel with 1px inset border.
 *
 * Shrinks the rect by 1 pixel on each side, draws the panel via
 * func_801EF800, then restores the original rect dimensions.
 */
void func_801EF934(s32 a0, s32 a1, s32 *a2) {
    s32 save0 = a2[0];
    s32 save1 = a2[1];
    u16 *rect = (u16 *)a2;
    rect[0] += 1;
    rect[1] += 1;
    rect[2] -= 2;
    rect[3] -= 2;
    ((DR_AREA *(*)())func_801EF800)(a0, a1);
    a2[0] = save0;
    a2[1] = save1;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF9AC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EFBB4);

/**
 * @brief Build the 65-entry falloff table in D_801FA3C8.
 *
 * Fills D_801FA3C8[0..64] back-to-front with a decay curve starting at
 * 0x1000 (4096 = 1.0 fixed point) and multiplying by 9/10 per step toward
 * index 0, then clears the halfword just before the table.
 *
 * @note Purpose uncertain — the 0x1000 top value and *0.9 steps suggest a
 *       volume or fade lookup table.
 */
void func_801EFF64(void) {
    u16 *p = D_801FA3C8;
    s32 val = 0x1000;
    s32 i;

    p += 64;
    *p = val;
    for (i = 0; i < 64; i++) {
        p--;
        *p = val;
        val = val * 9 / 10;
    }
    p[-1] = 0;
}

/* ======================================================================== */
/* Menu State Getters/Setters                                               */
/* ======================================================================== */

/** @brief Get current menu phase/mode. */
s32 func_801EFFB8(void) {
    return D_801FA27C;
}

/** @brief Set menu sub-state. */
void func_801EFFC8(s32 a0) {
    D_801FA27B = a0;
}

/** @brief Get menu sub-state. */
s32 func_801EFFD4(void) {
    return D_801FA27B;
}

/** @brief Set menu state A. */
void func_801EFFE4(s32 a0) {
    D_801FA27A = a0;
}

/** @brief Get menu state A. */
s32 func_801EFFF0(void) {
    return D_801FA27A;
}

/** @brief Set menu state B. */
void func_801F0000(s32 a0) {
    D_801FA278 = a0;
}

/** @brief Get menu state B. */
s32 func_801F000C(void) {
    return D_801FA278;
}

/** @brief Set menu flag. */
void func_801F001C(s32 a0) {
    D_801FA279 = a0;
}

/** @brief Get menu flag. */
s32 func_801F0028(void) {
    return D_801FA279;
}

/* ======================================================================== */
/* Menu Panel/Window Management                                             */
/* ======================================================================== */

/**
 * @brief Open/advance the current menu panel window.
 *
 * If the active window pointer (D_801FA3C0) equals D_801FA280 base,
 * advances it by 0xA0 bytes. Sets up the draw-list entry at
 * offset 0x74 and links the panel into the global list (g_activeDrawEnv).
 */
void func_801F0038(void)
{
    s32 val = D_801FA3C0;
    s32 base = D_801FA280;
    s32 new_var;
    s32 temp;
    if (val == base) {
        base = (s32)D_801FA280 + 0xA0;
    }
    D_801FA3C0 = base;
    ClearOTag(base + 0x74, 9);
    new_var = D_801FA3C0;
    temp = *((s32 *) (new_var + 0x98));
    *((s32 *) (new_var + 0x70)) = new_var + 0x74;
    g_activeDrawEnv = (DRAWENV *)(new_var + 0x14);
    *((s32 *) (new_var + 0x9C)) = temp;
}

/**
 * @brief Initialize display system and link the active panel.
 *
 * Resets the display pipeline, sets D_801FA3C0 as both
 * the active window (D_8005F138) and draw-list head (g_activeDrawEnv),
 * then initializes rendering from the panel's draw-list at offset 0x70.
 */
void func_801F00A0(void)
{
    s32 val;
    s32 arg;
    DrawSync(0);
    VSync(0);
    PutDispEnv(D_801FA3C0);
    PutDrawEnv(D_801FA3C0 + 0x14);
    val = D_801FA3C0;
    D_8005F138 = val;
    arg = *(s32 *)(val + 0x70);
    g_activeDrawEnv = (DRAWENV *)(val + 0x14);
    DrawOTag(arg);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F010C);

/**
 * @brief Initialize main menu panel system.
 *
 * Sets display width to 0x180, assigns VRAM buffer pointers at
 * offsets 0x98 and 0x138 in D_801FA280, then resets state.
 */
void func_801F0224(void) {
    s32 base;
    func_801F010C(0x180);
    base = (s32)D_801FA280;
    *(s32 *)(base + 0x98) = (s32)0x801B2000;
    *(s32 *)(base + 0x138) = (s32)0x801B8800;
    setAnimEntityParams(0, 0, 0);
}

/* ======================================================================== */
/* VRAM/Display Setup                                                       */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0274);

/**
 * @brief Build a draw-area primitive for the active draw environment and link it.
 *
 * Copies the active draw environment's clip origin into a local RECT, forces
 * the 384x216 menu draw size, packs the @c SetDrawArea GP0 command for it
 * into the @c DR_AREA at @p prim, links @p prim into the ordering-table slot
 * @p tag via @c addPrimFast (hand-picked temp @c $s2), and returns the next
 * packet slot.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @return Cursor for the next primitive (@c prim + 1).
 *
 * @note The clip reads go through a u16 view of the s16 RECT fields (lhu, not
 *       lh) and w/h are filled by one word store — both match-required.
 */
DR_AREA *func_801F03E8(P_TAG *ot, DR_AREA *prim) {
    RECT r;
    DRAWENV *env = g_activeDrawEnv;

    r.x = *(u16 *)&env->clip.x;
    r.y = *(u16 *)&env->clip.y;
    *(u32 *)&r.w = (216 << 16) | 384;
    SetDrawArea(prim, &r);
    addPrimFast(ot, prim, s2);
    return ++prim;
}

/**
 * @brief Upload two display regions to VRAM.
 *
 * Configures two 0x180-wide rectangular regions (at x=0 and x=0x200)
 * and transfers them via ClearImage/DrawSync. Initializes
 * double-buffered VRAM areas for the menu background.
 */
void func_801F0464(s32 a0) {
    s32 buf[2];
    buf[0] = 0;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    ClearImage(buf, 0, 0, 0);
    DrawSync(0);
    buf[0] = 0x200;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    ClearImage(buf, 0, 0, 0);
    DrawSync(0);
}

/* ======================================================================== */
/* Text/String Lookup & Rendering                                           */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F04E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F07D8);

/** @brief Render text string with default style (last param = 0). */
void func_801F0884(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_801F07D8(a0, a1, a2, a3, a4, 0);
}

/**
 * @brief Resolve an entry in a relative-offset table.
 *
 * The table is an array of u16 byte offsets (relative to @p base) with a
 * 2-byte header word at +0; entry @p index lives at base + index * 2 + 2.
 * An offset of 0 marks an empty slot.
 *
 * @param base Table base pointer.
 * @param index Entry index.
 * @return base + offset for the entry, or NULL if the slot is empty.
 */
u8 *func_801F08AC(u8 *base, s32 index) {
    u16 *p = (u16 *)(base + index * 2);

    /* Separate p++ and two *p reads (not p[1]): keeps the +2 as its own
     * addiu instead of folding into the load offset. */
    p++;
    if (*p == 0) {
        return 0;
    }
    return base + *p;
}

/**
 * @brief Look up a text string by category and ID.
 *
 * Navigates a two-level string table. Uses 0x801E0000 for type 1,
 * D_801F7FB0 otherwise. Returns D_801F7F74 (fallback) on failure.
 */
u8 *func_801F08D4(s32 a0, s32 a1, s32 a2, s32 a3) {
    u8 *ptr;
    if (a0) {
        ptr = (u8 *)0x801E0000;
    } else {
        ptr = D_801F7FB0;
    }
    if (!ptr) return D_801F7F74;
    ptr = func_801F08AC(ptr, a1);
    if (!ptr) return D_801F7F74;
    ptr = func_801F08AC(ptr, a2 * 2 + a3);
    if (ptr) return ptr;
    return D_801F7F74;
}

/* ======================================================================== */
/* Text Rendering Helpers (blink, color, conditional)                       */
/* ======================================================================== */

/** @brief Set current text color/font parameter for rendering. */
void func_801F0948(s32 a0) {
    D_801FAAE0 = a0;
}

/**
 * @brief Queue a text glyph for rendering.
 *
 * Appends a TextQueueEntry to D_801FAA20: position is anchored at
 * (x - 26, y + 3), together with the glyph code and the current
 * color/font parameter. D_801FAA1E is the queue count; the entry index
 * reads it as s16 while the increment goes through a u16 view (both
 * halves of the original's mixed-signedness access).
 *
 * @param code  Character/glyph code (stored as a byte).
 * @param color Color/font parameter (usually D_801FAAE0).
 * @param x     Screen X before the -26 anchor offset.
 * @param y     Screen Y before the +3 anchor offset.
 */
void func_801F0954(s32 code, s32 color, s32 x, s32 y) {
    TextQueueEntry *e = &D_801FAA20[D_801FAA1E];

    e->code = code;
    e->x = x - 26;
    e->y = y + 3;
    e->color = color;
    *(u16 *)&D_801FAA1E += 1;
}

/** @brief Render text at stored Y with default color. */
void func_801F0994(s32 a0, s32 a1, s32 a2) {
    func_801F0954(a0, D_801FAAE0, a1, a2);
}

/**
 * @brief Render text with blink effect.
 *
 * If (g_menuDisplayCfg.animCounter + D_801FAA1E) is odd, sets color to -1 (hidden),
 * creating a blinking text effect for highlighted menu items.
 */
void func_801F09C4(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (!((g_menuDisplayCfg.animCounter + D_801FAA1E) & 1)) {
        a0 = -1;
    }
    func_801F0954(a0, a1, a2, a3);
}

/** @brief Render blinking text at stored Y with default color. */
void func_801F0A04(s32 a0, s32 a1, s32 a2) {
    func_801F09C4(a0, D_801FAAE0, a1, a2);
}

/** @brief Render text, normal if a0 is nonzero, blinking otherwise. */
void func_801F0A34(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a0) {
        func_801F0994(a1, a2, a3);
    } else {
        func_801F0A04(a1, a2, a3);
    }
}

/** @brief Render text with explicit Y, normal if a0 nonzero, blinking otherwise. */
void func_801F0A78(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    if (a0) {
        func_801F0954(a1, a2, a3, a4);
    } else {
        func_801F09C4(a1, a2, a3, a4);
    }
}

/* ======================================================================== */
/* Menu Navigation Stack                                                    */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0AC8);

/**
 * @brief Pop top entry from menu navigation stack.
 *
 * Decrements the stack pointer and returns the popped screen ID.
 * Returns -1 if the stack is empty.
 */
s32 func_801F0BB0(void) {
    s8 val;

    D_801FAA11--;
    val = D_801FAA11;
    if (val < 0) {
        D_801FAA11 = 0;
        return -1;
    }
    return D_801FAA18[val];
}

/**
 * @brief Play music/sound for a menu screen.
 *
 * Looks up audio entry in D_801F7E6C (stride 8). If the track
 * differs from what is currently playing, loads and plays it.
 */
void func_801F0BF8(s32 a0) {
    u8 *entry = D_801F7E6C + a0 * 8;
    s32 val = *(s32 *)(entry + 4);
    if (val == 0xFF) return;
    if (val <= 0) return;
    if (getOverlayLoadStatus() == val) return;
    loadOverlay(val, 0, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0C5C);

/** @brief Peek at top of menu navigation stack without popping. */
s32 func_801F0D84(void) {
    return D_801FAA18[(s8)D_801FAA11];
}

/** @brief Set current menu screen ID. */
void func_801F0DA4(s32 a0) {
    D_801FAA10 = a0;
}

/** @brief Get current menu screen ID. */
s32 func_801F0DB0(void) {
    return D_801FAA10;
}

/* ======================================================================== */
/* Animation/Transition                                                     */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0DC0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0E5C);

/**
 * @brief Build a 12x12 menu-font sprite primitive and prepend it to the OT chain.
 *
 * Fills the SPRT at @p spr: CLUT selected by the low 3 bits of
 * @p clutFlags (base 0x3812), color from D_8008384C when any higher flag
 * bit is set, else g_menuColor; UV computed from the glyph index in the
 * 21-glyphs-per-row 12px font atlas. Links the sprite via addOtFast and
 * returns the new chain head.
 *
 * @note @c clutFlags is reused for the color word after its flag bits are
 *       consumed — the reuse gives color a3 (regalloc match).
 *
 * @param head      Current OT chain head (prim address image << 8).
 * @param spr       Storage for the SPRT primitive.
 * @param glyph     Glyph index into the font atlas.
 * @param clutFlags Bits 0..2: CLUT row; bits 3+: use alternate color.
 * @param xy        Packed x0 | y0 << 16 sprite position.
 * @return New OT chain head (@p spr's address image).
 */
s32 func_801F0F20(s32 head, SPRT *spr, s32 glyph, u32 clutFlags, s32 xy) {
    u32 hi;

    setlen(spr, 4);
    addOtFast(spr, head);
    hi = clutFlags >> 3;
    clutFlags &= 7;
    spr->clut = (clutFlags << 6) + 0x3812;
    if (hi != 0) {
        clutFlags = D_8008384C;
    } else {
        clutFlags = g_menuColor;
    }
    /* Word stores: w/h pair (12x12) and the r0g0b0+code word. */
    *(u32 *)&spr->w = 0xC000C;
    *(u32 *)&spr->r0 = clutFlags;
    *(u32 *)&spr->x0 = xy;
    *(u16 *)&spr->u0 = (glyph % 21 | (glyph / 21) << 8) * 12;
    return head;
}

/** @brief Set menu display dimensions (width defaults to 0x180). */
void func_801F0FD0(s32 a0, s32 a1) {
    if (!a1) {
        a1 = 0x180;
    }
    D_801FAA12 = a1;
    D_801FAA14 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0FEC);

/** @brief Get animation completion flag. */
s32 func_801F1200(void) {
    return D_801FAAF8;
}

/**
 * @brief Start a timed animation/transition.
 *
 * Sets source value, destination value, resets counter, and enables
 * the animation. D_801FAAF8 will be set when the transition completes.
 */
void func_801F1210(s32 a0, s32 a1) {
    D_801FAAE4 = a0;
    D_801FAAE8 = a1;
    D_801FAAF2 = 0;
    D_801FAAF0 = 1;
    D_801FAAF8 = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1240);

/** @brief Stop the current animation/transition. */
void func_801F12F0(void) {
    D_801FAAF0 = 0;
}

/* ======================================================================== */
/* Panel Iteration & Callbacks                                              */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F12FC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1584);

/**
 * @brief Per-frame menu render driver.
 *
 * Advances the menu animation counter, resets the color intensity, renders
 * the base menu layer (func_801F0AC8), submits the frame GPU packet
 * (func_801F2FAC over the current display-list head), then walks the
 * MenuTask list at D_801FA450 calling each task's handler; every handler
 * returns the advanced display-list cursor.
 *
 * @param ctx Render context.
 * @param dl  Display-list cursor.
 * @return Display-list cursor after all task handlers ran.
 */
s32 func_801F16AC(s32 ctx, s32 dl) {
    MenuTask *t;
    MenuTask *cur;
    s32 (*drawCb)(MenuTask *, s32, s32);

    g_menuDisplayCfg.animCounter++;
    setMenuColorIntensity(0x1000);
    t = D_801FA450;
    dl = func_801F0AC8(ctx, dl);
    storeGpuPacket(func_801F2FAC(ctx, getDisplayListHead()));
    while (1) {
        if (t == 0) {
            break;
        }
        cur = t;
        drawCb = t->drawCb;
        t = t->next;
        if (drawCb != 0) {
            dl = drawCb(cur, ctx, dl);
        }
    }
    return dl;
}

/** @brief Process all panel callbacks (wrapper for func_801F16AC).
 *
 * @note Calls through an unspecified-args cast with no arguments — the
 *       original passes whatever $a0/$a1 hold (the callee's ctx/dl inputs
 *       are dead this frame); an ANSI call would emit argument setup. */
void func_801F175C(void) {
    ((s32 (*)())func_801F16AC)();
}

/** @brief Clear 22 panel callback entries starting at a0[8]. */
void func_801F177C(s32 *a0) {
    s32 i;

    for (i = 21; i >= 0; i--) {
        a0[i + 8] = 0;
    }
}

/**
 * @brief Allocate a menu state task and link it at the head of the list.
 *
 * Claims the first free slot of the 10-entry pool D_801FA550, marks it in
 * use, and inserts it at the front of the D_801FA450 list: the head pointer
 * doubles as a sentinel node whose @c next field is the pointer itself, so
 * the insert is the ordinary four-store list splice. Clears the task's
 * callback table via func_801F177C, then stores @p tag, @p handler and a
 * zeroed state.
 *
 * @param tickCb Update callback stored in the task (func_801F1584).
 * @param drawCb Per-frame draw callback (func_801F16AC).
 * @return The claimed task as an s32, or 0 when the pool is full.
 */
s32 func_801F179C(s32 tickCb, s32 drawCb) {
    MenuTask *node;
    MenuTask *n = D_801FA550;
    MenuTask *first;
    MenuTask *head;
    s32 i;

    i = 0;
    while (1) {
        if (n->inUse == 0) {
            n->inUse = 1;
            node = n;
            break;
        }
        i++;
        if (i >= 10) {
            node = 0;
            break;
        }
        n++;
    }
    if (node == 0) {
        return 0;
    }
    head = (MenuTask *)&D_801FA450;
    first = head->next;
    node->prev = head;
    node->next = first;
    head->next = node;
    first->prev = node;
    func_801F177C((s32 *)node);
    node->tickCb = tickCb;
    node->drawCb = (s32 (*)(MenuTask *, s32, s32))drawCb;
    node->state = 0;
    return (s32)node;
}

/**
 * @brief Allocate a menu task and link it after the list head.
 *
 * Claims the first free slot of the 10-entry pool D_801FA550, marks it in
 * use, splices it in directly after D_801FA4D0 (the list head), clears its
 * callback table via func_801F177C, and stores @p tag and @p handler.
 *
 * @note The explicit exhaust-break (rather than a for loop with node
 *       pre-initialized) is what places the @c node @c = @c 0 on the
 *       loop-exit path, as the original does.
 *
 * @param tickCb Update callback stored in the task (func_801F1584).
 * @param drawCb Per-frame draw callback (func_801F16AC).
 * @return The claimed task, or NULL when the pool is full.
 */
MenuTask *func_801F1850(s32 tickCb, s32 drawCb) {
    MenuTask *node;
    MenuTask *n = D_801FA550;
    MenuTask *next;
    s32 i;

    i = 0;
    while (1) {
        if (n->inUse == 0) {
            n->inUse = 1;
            node = n;
            break;
        }
        i++;
        if (i >= 10) {
            node = 0;
            break;
        }
        n++;
    }
    if (node == 0) {
        return 0;
    }
    next = node->next;
    D_801FA4D0.prev = node;
    D_801FA4D0.next = next;
    node->next = &D_801FA4D0;
    next->prev = &D_801FA4D0;
    func_801F177C((s32 *)&D_801FA4D0);
    node->tickCb = tickCb;
    node->drawCb = (s32 (*)(MenuTask *, s32, s32))drawCb;
    return node;
}

/**
 * @brief Remove a node from the panel doubly-linked list.
 *
 * Unlinks the node, patches prev/next pointers, and clears the
 * node's status byte and data fields.
 */
void func_801F18FC(s32 *a0) {
    s32 *next = (s32 *)a0[1];
    s32 *prev = (s32 *)a0[0];
    *next = (s32)prev;
    prev[1] = (s32)next;
    *(u8 *)((u8 *)a0 + 0x12) = 0;
    a0[2] = 0;
    a0[3] = 0;
    *(s16 *)((u8 *)a0 + 0x10) = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1924);

/* ======================================================================== */
/* Cursor/Input Handling                                                    */
/* ======================================================================== */

/** @brief Panel input state machine: advance from idle to active. */
s32 func_801F1A40(s32 a0) {
    s32 ret = 1;
    switch (*(u16 *)(a0 + 0x10)) {
    case 0:
        *(u16 *)(a0 + 0x10) = ret;
        break;
    case 1:
        if (func_801F0D84() != 0) {
            break;
        }
        setRenderFlag(1);
        break;
    default:
        break;
    }
    return ret;
}

/** @brief Default panel callback (identity, returns a2 unchanged). */
s32 func_801F1AA4(s32 a0, s32 a1, s32 a2) {
    return a2;
}

/** @brief Iterate all panels and dispatch via callback. */
void func_801F1AAC(void) {
    s32 result = func_801F179C(func_801F1A40, func_801F1AA4);

    if (result != 0) {
        func_801F1A40(result);
    }
}

/* ======================================================================== */
/* Party Member Data & Input                                                */
/* ======================================================================== */

/** @brief Set cursor dimensions (row count, column count). */
void func_801F1AE8(s32 a0, s32 a1) {
    D_801FAB7A = a0;
    D_801FAB7B = a1;
}

/** @brief Snapshot current controller input state. */
void func_801F1AFC(void) {
    D_801FAB78 = D_80083850;
}

/** @brief Restore saved controller input state for menu processing. */
void func_801F1B10(void) {
    setMenuColorIntensity(D_801FAB78);
    buildGrayscaleGpuColor(D_801FAB78);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1B4C);

/** @brief Update all 8 party member cursor/panel states. */
void func_801F1CAC(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_801F1B4C(i);
    }
}

/** @brief Test if ability bit a1 is set for character a0. */
s32 func_801F1CE8(s32 a0, s32 a1) {
    u32 *base = (u32 *)(D_801FAB38 + a0 * 8);
    return (base[a1 / 32] & (1 << (a1 & 0x1F))) != 0;
}

/* ======================================================================== */
/* Menu Item List                                                           */
/* ======================================================================== */

/** @brief Empty stub (placeholder for panel init callback). */
void func_801F1D2C(s32 a0, s32 a1, s32 a2) {
}

/**
 * @brief Initialize menu item list from 0xFF-terminated data.
 *
 * Counts entries (stride 2 bytes) and stores the data pointer,
 * entry count, and resets the selection index.
 */
void func_801F1D34(u8 *a0) {
    s32 count;
    s32 sentinel;

    D_801FAB2C = (s32)a0;
    D_801FAB31 = 0;
    if (a0 == 0) {
        D_801FAB30 = 0;
    } else {
        count = 0;
        sentinel = 0xFF;
    top:
        if (*a0 == sentinel) goto out;
        a0 += 2;
        count++;
        goto top;
    out:
        D_801FAB30 = count;
    }
}

/** @brief Reset all menu item list state (scroll, pointer, count, index). */
void func_801F1D84(void) {
    D_801FAB28 = 0;
    D_801FAB2A = 0;
    D_801FAB2C = 0;
    D_801FAB30 = 0;
    D_801FAB31 = 0;
}

/** @brief Set menu list scroll offset. */
void func_801F1DB0(s32 a0) {
    D_801FAB28 = a0;
}

/**
 * @brief Select a menu item list by ID.
 *
 * Searches D_801F7F78 for the matching entry, then initializes the
 * list and sets the selection index to the found position.
 */
void func_801F1DBC(s32 a0) {
    u8 *ptr = D_801F7F78;
    s32 i;

    for (i = 0; ; i++, ptr += 2) {
        if (*ptr == 0xFF) {
            return;
        }
        if (*ptr == a0) {
            func_801F1D34(D_801F7F78);
            D_801FAB31 = i;
            return;
        }
    }
}

/* ======================================================================== */
/* Party Member Switch                                                      */
/* ======================================================================== */

/** @brief Save 3 active party slot IDs from D_80077E6C to buffer at a0+0x35. */
void func_801F1E20(u8 *a0) {
    u8 *src = D_80077E6C;
    u8 *dst = a0 + 0x35;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

/** @brief Restore 3 active party slot IDs from buffer at a0+0x35 to D_80077E6C. */
void func_801F1E54(u8 *a0) {
    u8 *dst = D_80077E6C;
    u8 *src = a0 + 0x35;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1E88);

/** @brief Swap party members (forward direction). */
void func_801F1F58(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 1);
}

/** @brief Swap party members (backward direction). */
void func_801F1F78(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 0);
}

/**
 * @brief Build sorted available character list from party data.
 *
 * Fills D_801FAB88 with 0xFF, then collects valid (non-0xFF) entries
 * from 3 active slots (a0+0x35) and 8 reserve slots (a0+0x38).
 */
void func_801F1F98(u8 *a0) {
    u8 *dst = D_801FAB88;
    s32 i;
    u8 *p;
    s32 fill;

    fill = 0xFF;
    i = 8;
    p = dst + 8;
    do {
        *p = fill;
        i--;
        p--;
    } while (i >= 0);

    for (i = 0; i < 3; i++) {
        u8 val = *(a0 + i + 0x35);
        if (val != 0xFF) {
            *dst++ = val;
        }
    }

    for (i = 0; i < 8; i++) {
        u8 val = *(a0 + i + 0x38);
        if (val != 0xFF) {
            *dst++ = val;
        }
    }
}

/* ======================================================================== */
/* Character/Entity Data Access                                             */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F202C);

/** @brief Get character status flags. */
s32 func_801F21D0(s32 a0) {
    return g_gameState.chars[a0].statusFlags;
}

/** @brief Set character status flags and sync to secondary table. */
void func_801F21FC(s32 a0, s32 a1) {
    s32 base2 = (s32)D_801FABC8;
    g_gameState.chars[a0].statusFlags = a1;
    *(s16 *)(base2 + a0 * 32 + 0xE) = a1;
}

/** @brief Get GF status (stub, always returns 0). */
s32 func_801F2238(s32 a0) {
    return 0;
}

/**
 * @brief Get entity health condition from D_80078D38 table.
 *
 * Returns 1 if dead (HP <= 0), 0x100 if critical (HP < 25% max),
 * or 0 for normal health.
 */
s32 func_801F2240(s32 a0) {
    u8 *entry = D_80078D38 + a0 * 12;
    s16 val = *(s16 *)(entry);
    s32 result = 0;
    if (val <= 0) {
        result = 1;
    } else {
        s32 limit = (s16)(*(u16 *)(entry + 2)) >> 2;
        if (val < limit) {
            result = 0x100;
        }
    }
    return result;
}

/** @brief Get party presence bitmask. */
s32 menumain_getPartyMemberMask(void) {
    return g_menumain_partyMemberMask;
}

/**
 * @brief Compute party presence bitmask.
 *
 * Iterates 3 active party slots at g_gameState + 0xAF4, sets a bit
 * for each valid (non-0xFF) member ID. Stores result in g_menumain_partyMemberMask.
 */
void func_801F22A8(void) {
    s32 result = 0;
    s32 i;
    REGALLOC_BARRIER(result);
    i = 0;

    for (; i < 3; i++) {
        u8 val = g_gameState.mainData.party.party[i];
        if (val != PARTY_SLOT_EMPTY) {
            result |= (1 << val);
        }
    }
    g_menumain_partyMemberMask = result;
}

/* ======================================================================== */
/* Character Panel Rendering                                                */
/* ======================================================================== */

/**
 * @brief Build the bitmask of characters that are alive and available.
 *
 * Sets bit i for every character whose status does not include
 * @c STATUS_KO, then intersects with the character availability mask
 * from func_80036EC0.
 *
 * @return u16 bitmask of usable characters, bit index = character ID.
 *
 * @note func_80036EC0 (src/card.c) is deliberately called without a
 *       prototype, as in the original build: card.h's u16 declaration must
 *       not be visible here or the call gains a spurious @c andi truncation
 *       (same ABI split as getGfAvailabilityMask — see card.h).
 */
u16 func_801F22F4(void) {
    s32 avail = func_80036EC0();
    u16 mask = 0;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (!(func_801F21D0(i) & STATUS_KO)) {
            mask |= 1 << i;
        }
    }
    mask &= avail;
    return mask;
}

/**
 * @brief Build the bitmask of GFs that are alive and available.
 *
 * Sets bit i for every GF whose saved HP is nonzero, then intersects
 * with getGfAvailabilityMask() (story/junction availability).
 *
 * @return u16 bitmask of usable GFs, bit index = GfId.
 */
u16 func_801F2370(void) {
    s32 avail = getGfAvailabilityMask();
    u16 mask = 0;
    s32 i;

    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].hp != 0) {
            mask |= 1 << i;
        }
    }
    mask &= avail;
    return mask;
}

/**
 * @brief Register a horizontally centered text region.
 *
 * Measures text @p textId via func_8002E680 (packed w | h << 16), maps
 * both extents through func_801F738C/func_801F7394, then registers the
 * RECT — x centered on the 384-wide menu screen at row @p y — with
 * func_8002E064 under @p idx.
 *
 * @note @c w holds the packed measurement first and is then reassigned to
 *       the mapped width — the reuse is what allocates s0/s1/s2 like the
 *       original.
 * @note func_8002E680 (src/btl_sfx.c) is called without a prototype here,
 *       as in the original build; field.h/we_object1.h carry the u8*
 *       declaration for their units until a btl_sfx.h consolidation pass.
 *
 * @param idx  Region slot index (passed to func_8002E064).
 * @param y    Screen Y for the region.
 * @param text Text to measure.
 */
void func_801F23D0(s32 idx, s32 y, u8 *text) {
    RECT r;
    s32 w = func_8002E680(text);
    s32 upper = w >> 16;
    s32 h;

    w = func_801F738C(w & 0xFFFF);
    h = func_801F7394(upper);
    r.x = (0x180 - w) / 2;
    r.y = y;
    r.w = w;
    r.h = h;
    func_8002E064(idx, &r);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2458);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2FAC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3270);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3464);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F36E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3824);

/**
 * @brief Draw character name panel with icon.
 *
 * Looks up the character name via func_801F6AD0, renders it with
 * func_801F0FEC, configures g_menuDisplayCfg (icon 0x55), and draws
 * the panel border via func_801EF9AC.
 */
void func_801F38F8(s32 a0, s32 a1, s32 a2) {
    s32 ret1;
    s32 ret2;

    ret1 = func_801F6AD0(*(u8 *)(a0 + 0x46));
    ret2 = func_801F0FEC(a1, a2, 0x22, 0xF, ret1, 7);
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 7;
    *(s32 *)&g_menuDisplayCfg.w = 0x001900F4; /* w=0xF4, h=0x19 packed */
    func_801EF9AC(a1, ret2, 0x1000, g_menuColor);
}

/** @brief Render text with explicit parameters (arg-reorder wrapper for func_801F0FEC). */
void func_801F3994(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F0FEC(a1, a2, a3, a4, a0, a5);
}

/**
 * @brief Draw an icon glyph plus a two-digit zero-trimmed number.
 *
 * Renders glyph 0x145 via func_800300F8 with a (a5 << 6) + 2 selection
 * parameter, then formats @p val as two digits (leading zeros replaced by
 * the first char of menu string 0xB) and prints them 18 pixels below via
 * func_8002C56C.
 *
 * @note getMenuString (main exe) is deliberately called without a prototype
 *       here, as in the original build; bc_object2.h carries the battle-side
 *       declaration and menumain must not include battle-overlay headers.
 *
 * @param val Number to render.
 * @param ctx Render context.
 * @param dl  Display-list cursor.
 * @param y   Anchor Y for the glyph; digits render at y + 18.
 * @param a4  X anchor (passed through to both renderers).
 * @param a5  Color/selection index for the digit pass.
 */
void func_801F39D0(s32 val, s32 ctx, s32 dl, s32 y, s32 a4, s32 a5) {
    u8 buf[16];
    s32 digits = D_80083858.digitBase;
    s32 cursor;
    u8 *str;

    cursor = func_800300F8(ctx, dl, 0x145, y, a4, g_menuColor, (a5 << 6) + 2);
    y += 0x12;
    intToDecStringShort(val, buf, digits);
    str = (u8 *)getMenuString(0xB);
    replaceLeadingZeros(buf + 2, 2, digits, str[0]);
    func_8002C56C(ctx, cursor, y, a4, buf + 2, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3ABC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3B64);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3CE0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3DE4);

/**
 * @brief Map status flags to display text color.
 *
 * Returns: 7 (white/normal), 2 (yellow/critical HP),
 * 5 (red/status ailment), 1 (gray/dead).
 */
s32 func_801F3FB4(s32 a0) {
    s32 v1 = 7;

    if (a0 & 0x100) {
        v1 = 2;
    }
    if (a0 & 0xFE) {
        v1 = 5;
    }
    if (a0 & 1) {
        v1 = 1;
    }
    return v1;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3FE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4168);

/** @brief Draw ability/status list line (default mode, last param = 0). */
void func_801F4274(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F4168(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F42A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4454);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4744);

/**
 * @brief Draw character command abilities panel.
 *
 * If a0[0x34] == 0, renders 3 command slots via func_801F42A4;
 * otherwise renders via func_801F3FE8 plus func_801F4744.
 */
s32 func_801F486C(u8 *a0, s32 a1) {
    s32 i;
    s32 result;

    if (a0[0x34] == 0) {
        for (i = 0; i < 3; i++) {
            result = func_801F42A4(a0, a1, result, i);
        }
    } else {
        for (i = 0; i < 3; i++) {
            result = func_801F3FE8(a0, a1, result, i);
        }
        result = func_801F4744(a0, a1);
    }
    return result;
}

/**
 * @brief Draw Gil (money) display panel.
 *
 * Renders the Gil label string, sets up g_menuDisplayCfg rendering params,
 * and draws the panel decoration via func_801EF9AC.
 */
void func_801F4918(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F6358(a1, a2, 0x22, 0xC6, (s32)D_8007737C);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 0xBE;
    g_menuDisplayCfg.w = 0xF4;
    g_menuDisplayCfg.h = 0x1A;
    func_801EF9AC(a1, ret, 0x1000, g_menuColor);
}

/* ======================================================================== */
/* Party Status & HP Display                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F49A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4A98);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4C60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4CE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4D70);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4EA8);

/** @brief Convert raw ability ID to menu display index (subtract 100). */
s32 func_801F5104(s32 a0) {
    return a0 - 0x64;
}

/**
 * @brief Compute ability learn percentage (0-100).
 *
 * Subtracts 800 or 900 threshold depending on value, then clamps
 * the result to the 0-100 range.
 */
s32 func_801F510C(s32 a0) {
    s32 v1;

    if (a0 < 0x385) {
        a0 -= 0x320;
    } else {
        a0 -= 0x384;
    }
    v1 = 0;
    if (a0 >= 0) {
        v1 = 0x64;
        if (a0 < 0x65) {
            v1 = a0;
        }
    }
    return v1;
}

/** @brief Check if ability is fully mastered (value >= 901). */
s32 func_801F5144(s32 a0) {
    return a0 >= 0x385;
}

/**
 * @brief Compute HP color flags from current HP and max HP.
 *
 * Clears low bits of a2, sets 0x200 if HP < 50% max, 0x300 if
 * HP < 25% max. Returns 1 if HP <= 0 (dead).
 */
s32 func_801F5150(s32 a0, s32 a1, s32 a2) {
    a2 &= ~0x301;
    if (a0 < (a1 >> 1)) {
        a2 |= 0x200;
    }
    if (a0 < (a1 >> 2)) {
        a2 |= 0x300;
    }
    if (a0 <= 0) {
        a2 = 1;
    }
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5190);

/** @brief Save 3 active party slots to D_801FABC4 and clear originals to 0xFF. */
void func_801F5300(void) {
    s32 i = 0;

    for (; i < 3; i++) {
        D_801FABC4[i] = g_gameState.mainData.party.party[i];
        g_gameState.mainData.party.party[i] = PARTY_SLOT_EMPTY;
    }
}

/** @brief Restore 3 active party slots from D_801FABC4 backup. */
void func_801F5340(void) {
    s32 i = 0;

    for (; i < 3; i++) {
        g_gameState.mainData.party.party[i] = D_801FABC4[i];
    }
}

/** @brief Recalculate stats for a party slot and copy result table to dst. */
void func_801F537C(s32 a0, CopyBlock16 *a1) {
    CopyBlock16 *src;
    CopyBlock16 *end;

    func_801F5300();
    func_801F5190(a0);

    src = (CopyBlock16 *)&g_battleChars;
    end = (CopyBlock16 *)((u8 *)&g_battleChars + 0x1D0);
    do {
        *a1++ = *src++;
    } while (src != end);

    func_801F5340();
    recalcPartyStats();
}

/** @brief Recalculate stats for a single party slot after swap. */
void func_801F5400(s32 a0) {
    func_801F5300();
    func_801F5190(a0);
    func_801F5340();
    recalcPartyStats();
}

/** @brief Recalculate stats for all 8 party slots. */
void func_801F5440(void) {
    s32 i;
    func_801F5300();
    for (i = 0; i < 8; i++) {
        func_801F5190(i);
    }
    func_801F5340();
    recalcPartyStats();
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5490);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F565C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F56E4);

/** @brief Set entity status flags, dispatched by type (character < 16, GF >= 16). */
void func_801F576C(s32 a0, s32 a1) {
    if (a0 >= 16) {
        func_801F2238(a0 - 16);
    } else {
        func_801F21FC(a0, a1);
    }
}

/** @brief Get entity status flags, dispatched by type (character < 16, GF >= 16). */
s32 func_801F57A4(s32 a0) {
    if (a0 >= 16) {
        return func_801F2240(a0 - 16);
    } else {
        return func_801F21D0(a0);
    }
}

/**
 * @brief Get entity current HP.
 *
 * For GF (a0 >= 16): reads from g_gameState at stride 68 + 0x62.
 * For character: returns 0 if dead, else reads at stride 152 + 0x490.
 */
u16 func_801F57DC(s32 a0) {
    if (a0 >= 16) {
        a0 -= 16;
        return g_gameState.gfs[a0].hp;
    } else {
        if (func_801F57A4(a0) & 1) {
            return 0;
        }
        return g_gameState.chars[a0].currentHp;
    }
}

/** @brief Set entity current HP (updates both primary and cache tables). */
void func_801F5868(s32 a0, s16 a1) {
    if (a0 >= 16) {
        s32 idx = a0 - 16;
        s32 base2 = (s32)&g_battleChars;
        g_gameState.gfs[idx].hp = a1;
        *(s16 *)(base2 + idx * 12 + 0x618) = a1;
    } else {
        s32 base2 = (s32)D_801FABC8;
        g_gameState.chars[a0].currentHp = a1;
        *(s16 *)(base2 + a0 * 32 + 0x8) = a1;
    }
}

/** @brief Get entity max HP. */
s32 func_801F58EC(s32 a0) {
    if (a0 < 16) {
        s32 base = (s32)D_801FABC8;
        return *(s16 *)(base + a0 * 32 + 0xA);
    } else {
        s32 idx = a0 - 16;
        s32 base = (s32)&g_battleChars;
        return *(s16 *)(base + idx * 12 + 0x61A);
    }
}

/** @brief Get entity level. */
s32 func_801F5938(s32 a0) {
    if (a0 < 16) {
        s32 base = (s32)D_801FABC8;
        return *(u8 *)(base + a0 * 32 + 0xC);
    } else {
        s32 idx = a0 - 16;
        s32 base = (s32)&g_battleChars;
        return *(u8 *)(base + idx * 12 + 0x620);
    }
}

/**
 * @brief Build cumulative pixel-width table for menu item strings.
 *
 * Iterates a -1-terminated u16 source list, measures each string's
 * pixel width via getGlyphWidthA, and accumulates offsets into dst.
 * Returns the item count.
 */
s32 func_801F5984(u16 *src, u16 *dst, s32 a2) {
    s32 accum = 0;
    s32 count = 0;
    s16 val;
    s32 ret;
    *dst++ = 0;
    while (1) {
        val = (s16)*src++;
        if (val == -1) break;
        ret = func_801F08D4(1, a2, val, 0);
        ret = getGlyphWidthA((u8 *)ret) + 12;
        accum += ret;
        *dst++ = accum;
        count++;
    }
    return count;
}

/* ======================================================================== */
/* Number & String Rendering                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5A38);

/**
 * @brief Size and draw a text-list panel from a -1-terminated id list.
 *
 * Measures every entry of @p ids (looked up in text category @p textCat via
 * func_801F08D4) to find the widest string, then sizes g_menuDisplayCfg to
 * fit — width = 8 pixels per character + 16 padding, height = 12 per row
 * plus 10 — and draws the panel frame with func_801EF9AC.
 *
 * @note Declaring @c count before @c maxWidth is load-bearing: the two have
 *       identical allocator priority, so the tie breaks on declaration order.
 *
 * @param ctx     Render context.
 * @param dl      Display-list cursor.
 * @param x       Panel X.
 * @param y       Panel Y.
 * @param textCat Text category for func_801F08D4 lookups.
 * @param ids     Entry ids, terminated by a negative value.
 * @param param   Intensity/param passed to func_801EF9AC.
 * @return Display-list cursor after the panel.
 */
s32 func_801F5B54(s32 ctx, s32 dl, s32 x, s32 y, s32 textCat, u16 *ids, s32 param) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 count;
    s32 maxWidth;
    s16 id;
    u8 *str;
    s32 len;

    maxWidth = 0;
    count = maxWidth;
    while (1) {
        id = *ids++;
        if (id < 0) {
            break;
        }
        str = func_801F08D4(1, textCat, id, 0);
        len = 0;
        while (*str++ != 0) {
            len++;
        }
        len *= 8;
        if (maxWidth < len) {
            maxWidth = len;
        }
        count++;
    }
    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = maxWidth + 0x10;
    cfg->h = count * 12 + 0xA;
    dl = func_801EF9AC(ctx, dl, param, g_menuColor);
    return dl;
}

/**
 * @brief Draw a text-list panel's entries, greying out disabled rows.
 *
 * Renders each entry of the -1-terminated @p ids list (looked up in text
 * category @p textCat) down the panel at 12-pixel row pitch. Bit i of
 * @p enableMask selects the colour of row i: 7 when set (enabled), 1 when
 * clear (greyed out).
 *
 * @note @p enableMask must be unsigned so the per-row shift is a logical
 *       one, and the @c tx / @c ty cursor copies (assigned in that order)
 *       are what give x, textCat and ctx their saved registers.
 *
 * @param ctx        Render context.
 * @param dl         Display-list cursor.
 * @param x          Row X.
 * @param y          Y of the first row.
 * @param textCat    Text category for func_801F08D4 lookups.
 * @param ids        Entry ids, terminated by a negative value.
 * @param enableMask Per-row enabled bits, LSB = first row.
 * @return Display-list cursor after the rows.
 */
s32 func_801F5C84(s32 ctx, s32 dl, s32 x, s32 y, s32 textCat, u16 *ids, u32 enableMask) {
    s32 tx;
    s32 ty;
    s16 id;
    u8 *str;
    s32 color;

    tx = x;
    ty = y;
    while (1) {
        id = *ids++;
        if (id < 0) {
            break;
        }
        str = func_801F08D4(1, textCat, id, 0);
        color = 1;
        if (enableMask & 1) {
            color = 7;
        }
        dl = func_8002C56C(ctx, dl, tx, ty, str, color);
        ty += 0xC;
        enableMask >>= 1;
    }
    return dl;
}

/**
 * @brief Draw a complete menu panel: clip restore, border, then body.
 *
 * Chains three packet builders on the display list: func_801F03E8
 * (draw-area restore), func_801F5C84 at (x+8, y+7) with @p a7, then
 * func_801F5B54 at (x, y) with @p a6. Each call's return value is the
 * next call's packet cursor.
 *
 * @param ctx        Render context (also the ordering-table slot).
 * @param prim       Packet cursor.
 * @param x          Panel X.
 * @param y          Panel Y.
 * @param textCat    Text category for both panel builders.
 * @param ids        Entry ids for both panel builders.
 * @param bodyParam  Intensity/param for the body pass (func_801F5B54).
 * @param insetParam Intensity/param for the inset pass (func_801F5C84).
 * @return Packet cursor after the body pass.
 */
s32 func_801F5D5C(s32 ctx, DR_AREA *prim, s32 x, s32 y, s32 textCat, u16 *ids, s32 bodyParam, s32 insetParam) {
    s32 dl = (s32)func_801F03E8((P_TAG *)ctx, prim);

    dl = func_801F5C84(ctx, dl, x + 8, y + 7, textCat, ids, insetParam);
    return func_801F5B54(ctx, dl, x, y, textCat, ids, bodyParam);
}

/**
 * @brief Draw a two-digit page/slot counter as glyphs.
 *
 * Renders the separator glyph 0x32, then the digits of @p val + 1: the tens
 * digit only when @p flag is set, then the units digit. Each glyph advances
 * the cursor.
 *
 * @note intToDecStringShort formats into five digit glyphs (its divisor table
 *       runs 10000..1), so buf[3] is the tens place and buf[4] the units;
 *       replaceLeadingZeros blanks the tens glyph when it would be a zero.
 *
 * @param ctx   Render context.
 * @param dl    Display-list cursor.
 * @param x     Start X (advances 9 then 6 pixels per drawn glyph).
 * @param y     Screen Y.
 * @param color Glyph color.
 * @param val   Zero-based value; displayed as @p val + 1.
 * @param flag  Nonzero to draw the tens digit.
 * @return Display-list cursor after the glyphs.
 */
s32 func_801F5E0C(s32 ctx, s32 dl, s32 x, s32 y, s32 color, s32 val, s32 flag) {
    u8 buf[16];

    intToDecStringShort(val + 1, buf, 0x28);
    replaceLeadingZeros(buf + 3, 1, 0x28, 7);
    dl = func_8002FF34(ctx, dl, 0x32, x, y, color);
    x += 9;
    if (flag != 0) {
        dl = func_8002FF34(ctx, dl, buf[3], x, y, color);
        x += 6;
    }
    dl = func_8002FF34(ctx, dl, buf[4], x, y, color);
    return dl;
}

/** @brief Render number right-aligned (last param = 1). */
s32 func_801F5EFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 1);
}

/** @brief Render number left-aligned (last param = 0). */
s32 func_801F5F30(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 0);
}

/**
 * @brief Draw the scroll arrows along a panel's bottom edge.
 *
 * Renders the left arrow (glyph 0x5C) at the panel's bottom-left and the
 * right arrow (0x5D) at its bottom-right, each gated by a bit of
 * @p arrows. Both blink together: on alternate 8-frame phases of
 * g_menuDisplayCfg.animCounter the selection parameter drops to 0.
 *
 * @note The shared @c bottom temp is what evaluates the y/h pair before
 *       the x load (scheduling match); a per-branch local does not.
 *
 * @param ctx    Render context.
 * @param dl     Display-list cursor.
 * @param color  Color passed through to both glyphs.
 * @param arrows Bit 0: draw left arrow. Bit 1: draw right arrow.
 * @return Display-list cursor after the drawn arrows.
 */
s32 func_801F5F60(s32 ctx, s32 dl, s32 color, s32 arrows) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 blink = 0x140;
    s32 bottom;

    if ((cfg->animCounter >> 3) & 1) {
        blink = 0;
    }
    if (arrows & 1) {
        bottom = cfg->y + cfg->h - 10;
        dl = func_800300F8(ctx, dl, 0x5C, g_menuDisplayCfg.x + 2, bottom, color, blink);
    }
    if (arrows & 2) {
        bottom = cfg->y + cfg->h - 10;
        dl = func_800300F8(ctx, dl, 0x5D, g_menuDisplayCfg.x + cfg->w - 9, bottom, color, blink);
    }
    return dl;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F605C);

/**
 * @brief Draw face glyphs for masked party members in a 3-column grid.
 *
 * For each set bit i in @p mask (restricted to bits 1..6), renders face
 * glyph 0x110 + i at an 18x18 grid cell, filling left-to-right then
 * top-to-bottom in draw order.
 *
 * @param ctx  Render context.
 * @param dl   Display-list cursor (threaded through the glyph calls).
 * @param x    Grid origin X.
 * @param y    Grid origin Y.
 * @param mask Bitmask of members to draw (FACE_GRID_MEMBERS honored).
 * @return Display-list cursor after the drawn glyphs.
 */
s32 func_801F6234(s32 ctx, s32 dl, s32 x, s32 y, s32 mask) {
    s32 drawn = 0;
    s32 i;

    mask &= FACE_GRID_MEMBERS;
    for (i = 0; i < 8; i++) {
        s32 bit = 1 << i;

        if (mask & bit) {
            s32 row = drawn / 3;
            s32 col = drawn - row * 3;

            dl = func_8002FF34(ctx, dl, i + 0x110, x + col * 18, y + row * 18, g_menuColor);
            drawn++;
        }
    }
    return dl;
}

/* ======================================================================== */
/* String Resource Lookup                                                   */
/* ======================================================================== */

/** @brief Look up string resource pointer by index from D_801F8BB8 table. */
s32 func_801F6324(s32 a0) {
    u16 *table = (u16 *)D_801F8BB8;

    if (a0 >= table[0]) {
        a0 = 0;
    }
    a0 += 1;
    return table[a0] + (s32)table;
}

/** @brief Decode and render an indexed string resource to screen. */
s32 func_801F6358(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    u8 buf[256];
    s32 ptr = func_801F6324(a4);
    decodeMessage(ptr, buf, -1);
    func_801F0FEC(a0, a1, a2, a3, buf, 7);
}

/** @brief Draw icon/sprite at position (arg-reorder wrapper for func_800375A0). */
void func_801F63DC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_800375A0(a1, a2, a0, a3, a4, a5);
}

/**
 * @brief Conditionally render highlighted entry if character bit is set.
 *
 * Checks if the bit at position a0 in the party mask is set.
 * If so, renders the entry via func_8002FF34 with a highlight color
 * of 0xD6. Otherwise returns the input OT pointer unchanged.
 *
 * @param a0 Bit position to check in party mask.
 * @param a1 OT base pointer.
 * @param a2 Current draw pointer (returned if bit not set).
 * @param a3 Y coordinate.
 * @param a4 Additional render parameter (5th arg on stack).
 * @return Updated draw pointer.
 */
s32 func_801F6418(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    s32 mask = menumain_getPartyMemberMask();

    if (((mask & 0xFFFF) >> a0) & 1) {
        a2 = func_8002FF34(a1, a2, 0xD6, a3, a4, g_menuColor);
    }
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F64A4);

/**
 * @brief Draw full character info panel.
 *
 * Chains: func_801F64A4 (header/portrait), func_801F3DE4 (stats block),
 * func_801F6234 (HP/status bar), func_801F605C (ability/junction summary).
 */
s32 func_801F65F0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 ret;

    ret = func_801F64A4(a0, a1, a2, a3, a4, a5, 1);
    ret = func_801F3DE4(*(s32 *)a5, *(s32 *)(a5 + 4), a0, ret, a2 + 0x20, a3 + 0x7C, 7);
    ret = func_801F6234(a0, ret, a2 + 0xD0, a3 + 0x39, *(u16 *)(a5 + 0xE));
    return func_801F605C(a0, ret, a2 + 0x10E, a3 + 0x38, a5);
}

/**
 * @brief Draw a character's level number in its status panel slot.
 *
 * Anchors g_menuDisplayCfg at (x+200, y+121) with a 154x22 box and no
 * icon, renders the level value from getLevelCurveData(charIdx) as a
 * 7-glyph field at (x+209, y+128) via func_801F0FEC, then draws the
 * panel frame with func_801EF9AC at full intensity.
 *
 * @note @p x and @p y are advanced in place — reassigning the parameters
 *       restores their true live lengths so px/py keep s0/s1 (allocation
 *       match).
 *
 * @param ctx     Render context.
 * @param dl      Display-list cursor.
 * @param x       Panel base X.
 * @param y       Panel base Y.
 * @param charIdx Level-curve entry to display.
 */
void func_801F66B0(s32 ctx, s32 dl, s32 x, s32 y, s32 charIdx) {
    s32 lvl;

    x += 0xC8;
    y += 0x79;
    g_menuDisplayCfg.x = x;
    x += 9;
    g_menuDisplayCfg.w = 0x9A;
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.h = 0x16;
    y += 7;
    lvl = getLevelCurveData(charIdx);
    dl = func_801F0FEC(ctx, dl, x, y, lvl, 7);
    func_801EF9AC(ctx, dl, 0x1000, g_menuColor);
}

/**
 * @brief Adjust cursor position based on D-pad input with wrapping and guard.
 *
 * If max == 1 (single option), returns 0 immediately.
 * Checks bit flags for up (0x4000) and down (0x1000) input.
 * On up: increments position, wraps to 0 if >= max.
 * On down: decrements position, wraps to max-1 if < 0.
 * Plays a sound effect on each valid input.
 *
 * @param flags   Input button flags.
 * @param max     Maximum position value (exclusive).
 * @param current Current cursor position.
 * @return Updated cursor position, or 0 if max == 1.
 */
s32 func_801F6768(u16 flags, s32 max, s32 current) {
    if (max == 1) {
        return 0;
    }
    if (flags & 0x4000) {
        sendSpuCommand(1);
        current++;
        if (current >= max) {
            current = 0;
        }
    }
    if (flags & 0x1000) {
        sendSpuCommand(1);
        current--;
        if (current < 0) {
            current = max - 1;
        }
    }
    return current;
}

/**
 * @brief Adjust cursor position based on D-pad input with wrapping.
 *
 * Checks bit flags for right (0x2000) and left (0x8000) input.
 * On right: increments position, wraps to 0 if >= max.
 * On left: decrements position, wraps to max-1 if < 0.
 * Plays a sound effect on each valid input.
 *
 * @param flags   Input button flags.
 * @param max     Maximum position value (exclusive).
 * @param current Current cursor position.
 * @return Updated cursor position.
 */
s32 func_801F6800(u16 flags, s32 max, s32 current) {
    if (flags & 0x2000) {
        sendSpuCommand(1);
        current++;
        if (current >= max) {
            current = 0;
        }
    }
    if (flags & 0x8000) {
        sendSpuCommand(1);
        current--;
        if (current < 0) {
            current = max - 1;
        }
    }
    return current;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6888);

/**
 * @brief Initialize the main menu state.
 *
 * Clears the menu flag, recalculates party stats, sets up the panel
 * callback chain, initializes display lists, and populates the menu
 * context structure with GF availability, character masks, and defaults.
 */
void func_801F6934(void) {
    u8 *ctx;

    D_801FAB7C = 0;
    recalcPartyStats();
    ctx = (u8 *)func_801F179C(func_801F2458, func_801F4A98);
    func_801F1D2C((s32)&D_800562A4, (s32)&D_801F7DF4, (s32)D_801F8BB8);
    func_801F1D2C(0, (s32)&D_801F7E00, (s32)D_801F889C);
    func_801F1D2C(0, (s32)&D_801F7E0C, (s32)&D_801F87B8);
    func_801F1CAC();
    if (ctx != NULL) {
        D_801FAB28 = 0x1000;
        *(u16 *)(ctx + 0x2C) = 0;
        D_801FAB2A = 0x1000;
        *(u16 *)(ctx + 0x20) = func_80036EC0();
        *(u16 *)(ctx + 0x32) = func_801F22F4();
        *(u16 *)(ctx + 0x44) = 0;
        *(u8 *)(ctx + 0x43) = 0;
        *(u8 *)(ctx + 0x4B) = 0;
        func_801F5490((s32)ctx);
        func_801F1E54(ctx);
        func_801F202C();
        *(u16 *)(ctx + 0x2C) = 0;
        func_801F2458((s32)ctx);
        *(u8 *)(ctx + 0x23) = popcount(*(u16 *)(ctx + 0x20));
        {
            u8 tmp = D_801FAB30;
            *(u8 *)(ctx + 0x40) = 0;
            *(s32 *)(ctx + 0x24) = 0;
            *(u8 *)(ctx + 0x41) = tmp;
        }
    }
    func_801F1DB0(0);
    func_801F7B60();
}

/** @brief Advance pseudo-random number generator (LCG: val*125+14 mod 32768). */
s32 func_801F6A5C(void) {
    s32 base = (s32)&g_battleAnims;
    s32 val = *(u16 *)(base + 0x9C2);
    val = (val * 125 + 14) % 32768;
    *(u16 *)(base + 0x9C2) = val;
    return val;
}

/** @brief Look up ability/command name string (category 3). */
void func_801F6AA4(s32 a0) {
    func_801F08D4(1, 3, a0, 0);
}

/** @brief Look up character name string. */
s32 func_801F6AD0(s32 a0) {
    func_801F08D4(0, 0, a0, 1);
}

/** @brief Look up character description string. */
s32 func_801F6AFC(s32 a0) {
    return func_801F08D4(0, 0, a0, 0);
}

/** @brief Map ability index to display category via double indirection. */
s32 func_801F6B28(s32 a0) {
    return D_801F7F98[D_801F889C[a0].b0];
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6B54);

/**
 * @brief Render a column of bitmask-selected entries inside a 168x120 panel.
 *
 * For each set bit i (0..7) of @p mask, renders entry i at (x+8, ty) via
 * func_801F6B54, advancing ty by 13 per drawn row. Then anchors
 * g_menuDisplayCfg at (x, y) with size 168x120 and icon 0x49 and draws the
 * panel frame with func_801EF9AC.
 *
 * @param ctx  Render context.
 * @param dl   Display-list cursor (threaded through the row calls).
 * @param x    Panel X.
 * @param y    Panel Y.
 * @param a4   Passed to func_801EF9AC (intensity/param).
 * @param mask Bitmask of rows to draw, bit index = entry index.
 */
void func_801F6C9C(s32 ctx, s32 dl, s32 x, s32 y, s32 a4, u16 mask) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 tx = x + 8;
    s32 ty = y + 0xA;
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((mask >> i) & 1) {
            dl = func_801F6B54(ctx, dl, tx, ty, i);
            ty += 0xD;
        }
    }
    cfg->iconType = 0x49;
    cfg->w = 0xA8;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->h = 0x78;
    func_801EF9AC(ctx, dl, a4, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6D88);

/* ======================================================================== */
/* Config/Settings Display                                                  */
/* ======================================================================== */

/** @brief Get packed cursor coordinates (D_801FACE4 << 16 | D_801FACE2). */
s32 func_801F6F6C(void) {
    return (D_801FACE4 << 16) | D_801FACE2;
}

/** @brief Render menu item at grid position derived from hi/lo nibbles of a0. */
void func_801F6F88(s32 a0) {
    s32 hi = a0 >> 4;
    s32 lo = a0 & 0xF;
    s32 val = func_801F6F6C();

    func_801F0994(lo | hi, (u16)val, (val >> 16) + lo * 13);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6FE4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7148);

/** @brief Render config option (simplified wrapper for func_801F7148). */
void func_801F728C(s32 a0, s32 a1) {
    func_801F7148(a0, 0, 0, a1);
}

/** @brief Get current config value. */
s32 func_801F72B4(void) {
    return D_801FACE8;
}

/**
 * @brief Collect the party-wide junction-ability bitmask into D_801FACE8.
 *
 * For every owned GF (GF_EXISTS), enumerates its available abilities via
 * func_800369CC and sets bit (slotIndex - 0x5C) in D_801FACE8 for each
 * entry in the junction-ability slot range 0x5C..0x73.
 */
void func_801F72C4(void) {
    AbilityListEntry list[22];
    s32 i;

    D_801FACE8 = 0;
    for (i = 0; i < 16; i++) {
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            s32 n = func_800369CC(i, list, 0);
            AbilityListEntry *p = list;

            for (; n > 0; n--, p++) {
                u32 c = p->slotIndex - 0x5C;

                if (c < 0x18) {
                    D_801FACE8 |= 1 << c;
                }
            }
        }
    }
}

/* ======================================================================== */
/* Junction/Ability Management                                              */
/* ======================================================================== */

/** @brief Convert character index to GF table index (add 0x10). */
s32 func_801F738C(s32 a0) {
    return a0 + 0x10;
}

/** @brief Convert character index to GF table index + 1 (add 0x11). */
s32 func_801F7394(s32 a0) {
    return a0 + 0x11;
}

/**
 * @brief Render scrollable panel with footer.
 *
 * Sets up g_menuDisplayCfg rendering params (icon 0x4A), then renders
 * the list, scroll indicator, and footer/help text.
 */
void func_801F739C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 base = (s32)&g_menuDisplayCfg;
    s32 ret1;
    s32 ret2;

    *(u8 *)(base + 0x10) = 0x4A;  /* iconType */
    *(u8 *)(base + 0x11) = 0;     /* iconSubType */
    *(s16 *)&g_menuDisplayCfg = a2;     /* x */
    *(s16 *)(base + 4) = 0x9A;    /* w */
    *(s16 *)(base + 2) = a3;      /* y */
    *(s16 *)(base + 6) = 0x40;    /* h */
    *(u8 *)(base + 0x13) = 4;     /* columnCount */
    *(u8 *)(base + 0x1E) = a4;    /* itemId */

    ret1 = func_801F5F30(a0, a1, a2 + 0x24, a3, g_menuColor, *(u8 *)(base + 0x16) /* pageStart */);
    ret2 = func_801F5F60(a0, ret1, g_menuColor, 3);
    func_801EFBB4(a0, ret2, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7454);

/** @brief Apply junction change, then update cursor and recalculate party stats. */
s32 func_801F75A4(s32 a0) {
    s32 result = func_801F7454(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F75EC);

/** @brief Remove junction, then update cursor and recalculate party stats. */
void func_801F76A8(s32 a0) {
    func_801F75EC(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
}

/**
 * @brief Handle left/right D-pad input for value adjustment.
 *
 * If right pressed (0x2000), increments; if left (0x8000), decrements.
 * Plays a sound effect if the value changed.
 */
s32 func_801F76E0(s32 flags, s32 a1, s32 a2) {
    s32 result = a2;
    s32 orig = a2;

    if (flags & 0x2000) {
        result = func_80035B28(a1, result);
    }
    if (flags & 0x8000) {
        result = func_80035B70(a1, orig);
    }
    if (result != orig) {
        sendSpuCommand(1);
    }
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F776C);

/**
 * @brief Find ability slot by ID in character data.
 *
 * Searches 19 ability slots in g_characterAbilities for value a1.
 * Returns the slot index if found, or a2 (default) if not.
 */
s32 func_801F77F8(s32 a0, s32 a1, s32 a2) {
    s32 offset = a0 * 152;
    u8 *ptr = (u8 *)((s32)g_characterAbilities + offset);
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*ptr++ != a1) continue;
        return i;
    }
    return a2;
}

/** @brief Equip magic a1 to character a0's junction slot of type a2. */
void func_801F784C(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F776C(a1, a2);
    if (ret) {
        ret = func_801F77F8(a0, a1, a2);
        g_gameState.chars[a0].junctions[ret] = a1;
    }
    func_801F1B4C(a0);
}

/** @brief Remove ability a1 from character a0's junction slots. */
void func_801F78D8(s32 a0, s32 a1) {
    s32 offset = a0 * 152;
    u8 *ptr = (u8 *)((s32)g_characterAbilities + offset);
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*ptr == a1) {
            *ptr = 0;
        }
        ptr++;
    }
}

/* ======================================================================== */
/* Misc/Utility                                                             */
/* ======================================================================== */

/** @brief Apply vibration config setting from g_configFlags bit 1. */
void func_801F7928(void) {
    s32 val = g_configFlags & 2;
    sndSelectMode(val != 0);
}

/** @brief Apply ATB/screen brightness setting from g_configFlags bit 6. */
void func_801F7954(void) {
    s32 a1 = 0;
    if (g_configFlags & 0x40) {
        a1 = 0xFF;
    }
    setAnimEntityOpacity(0, a1);
}

/**
 * @brief Push the analog-volume and controller-mode config to the sound layer.
 *
 * Sends (analogVolume + 5) * 12 as the volume argument of func_80027C00,
 * then tells func_80027C90 whether the analog flag is active — only
 * honored when the controller is in customize mode (CONFIG_CONTROLLER).
 *
 * @note func_80027C00/func_80027C90 are main-executable sound-layer
 *       routines that are not decompiled yet.
 */
void func_801F798C(void) {
    s32 v;

    v = g_gameState.config.analogVolume;
    v += 5;
    func_80027C00(0, v * 12);
    v = g_gameState.config.flags & CONFIG_ANALOG;
    v = v != 0;
    if (!(g_gameState.config.flags & CONFIG_CONTROLLER)) {
        v = 0;
    }
    func_80027C90(0, v);
}

/** @brief Test sealed-features bits (@c GameConfig.sealedFeatures & @p a0). */
s32 func_801F79F8(s32 a0) {
    return g_gameState.config.sealedFeatures & a0;
}

/** @brief Update config vibration flag based on slot 0 status. */
void func_801F7A08(void) {
    if (getBattleAnimOpacity(0) == 0xFF) {
        g_gameState.config.flags |= CONFIG_VIBRATION;
    } else {
        g_gameState.config.flags &= ~CONFIG_VIBRATION;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7A54);

/**
 * @brief Scale a value by 100 and store it clamped to [100, 3100] in D_800780E8.
 *
 * @note Purpose uncertain — appears to set a percentage-derived global
 *       (e.g. a speed or magnification parameter).
 *
 * @param n Input value; stored as n*100 clamped to 100..3100.
 */
void func_801F7AD4(s32 n) {
    s32 val = n * 100;
    s32 clamped;

    if (val >= 100) {
        if (val <= 3100) {
            clamped = val;
        } else {
            clamped = 3100;
        }
    } else {
        clamped = 100;
    }
    /* Fold-away bump: raises clamped's allocno priority above val's so
     * clamped takes v1 and val keeps a0 (allocation-order match). */
    clamped++;
    clamped--;
    D_800780E8 = clamped;
}

/**
 * @brief Clear magic slots with zero quantity.
 *
 * Iterates 32 magic entries (stride 2) for character a0. If the
 * quantity byte is 0, clears the magic ID byte to 0.
 */
void func_801F7B10(s32 a0) {
    s32 offset = a0 * 152;
    s32 base = (s32)g_characterMagic;
    u8 *ptr = (u8 *)(base + offset);
    s32 i;
    for (i = 0; i < 32; i++) {
        if (ptr[1] == 0) {
            ptr[0] = 0;
        }
        ptr += 2;
    }
}

/**
 * @brief Sync magic IDs with quantities (bidirectional zero-clear).
 *
 * Iterates 198 entries in D_80077EBC. If quantity is 0, clears the
 * ID; if ID is 0, clears the quantity.
 */
void func_801F7B60(void) {
    u8 *a0 = D_80077EBC;
    s32 a2 = 0;
    u8 *v1 = a0 + 1;
    do {
        u8 b = *v1;
        u8 a = *a0;
        if (b == 0) {
            *a0 = 0;
        }
        if (a == 0) {
            *v1 = 0;
        }
        a2++;
        v1 += 2;
        a0 += 2;
    } while (a2 < 198);
}

/** @brief Convert 0-255 value to 0-100 percentage. */
s32 func_801F7BAC(s32 a0) {
    return a0 * 100 / 255;
}

/** @brief Identity function (returns a0 unchanged, compatibility placeholder). */
s32 func_801F7BE4(s32 a0) {
    return a0;
}

/** @brief Play toggle sound effect (sound 2 if bit 6 set, else sound 3). */
void func_801F7BEC(s32 a0) {
    if (a0 & 0x40) {
        sendSpuCommand(2);
    } else {
        sendSpuCommand(3);
    }
}

/**
 * @brief Expand compressed status flags to rendering icon bitmask.
 *
 * Maps individual status bits from a compact internal format to a
 * wider bitmask with spacing for display icon positioning.
 */
s32 func_801F7C20(s32 a0) {
    s32 result = a0 & 0x7FF;
    if (a0 & 0x800)   result |= 0x800;
    if (a0 & 0x2000)  result |= 0x1800;
    if (a0 & 0x4000)  result |= 0x7800;
    if (a0 & 0x1000)  result |= 0x8000;
    if (a0 & 0x8000)  result |= 0x18000;
    if (a0 & 0x10000) {
        s32 mask = 0x10000;
        result |= 0x8000;
        result |= mask;
        result |= 0x20000;
        result |= 0x40000;
    }
    return result;
}

/** @brief Render save/card-related text (convert value to string, draw at position). */
void func_801F7C98(s32 a0, s32 a1) {
    u8 buf[16];
    intToDecStringShort(a0, buf, 0x30);
    copyString(a1, D_80056290);
    btlStrcat2(a1, &buf[3]);
}
