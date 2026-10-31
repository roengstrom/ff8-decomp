#include "common.h"
#include "gamestate.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menucrd.h"
#include "overlay.h"
#include "ui/sprite.h"
#include "ui/text.h"

/**
 * @brief Card menu task state, allocated by func_801F179C.
 *
 * The first 0x10 bytes belong to the menumain allocator (task links and the
 * tick/draw callbacks).
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x20];
    /* 0x20 */ u8 *rowText[2];      /**< Row text the list panels draw (MenuDisplayConfig.dataPtr). */
    /* 0x28 */ u16 scroll;
    /* 0x2A */ u8 pad2A[2];
    /* 0x2C */ u8 unk2C;
    /* 0x2D */ u8 pad2D;
    /* 0x2E */ s16 card;            /**< Card under the cursor. */
    /* 0x30 */ s16 intensity;       /**< Menu color intensity. */
    /* 0x32 */ u16 groupCounts[4];  /**< Card copies owned per level group: 1-5, 6-7, 8-9, 10. */
    /* 0x3A */ u16 totalCount;      /**< Card copies owned in all. */
    /* 0x3C */ u8 pad3C[6];
    /* 0x42 */ u8 unk42;
} CardMenuState;

#define CARD_BACKSIDE_INDEX 11

extern s16 D_801E7D64;
extern s16 D_801E7D66;
extern SpriteTable g_currentPageCardTable;

extern s16 getGameStateS16(void);

void func_801E582C(CardMenuState *state);
void func_801E5ABC(CardMenuState *state);
s32 func_801E5F7C(s32, s32, s32, s32, s32);
s32 func_801E67E4(s32, s32, s32, s32, s32);
s32 func_801E69AC(CardMenuState *state, s32 a1, s32 a2);
static void *DrawCardBacksideSprite(s32, void *, void *, s32, s32);

/** @brief Look up string @p a0 in menu text category 0xD. */
u8 *func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/**
 * @brief Update card display position after a transition completes.
 *
 * Checks if the card transition animation is done via pollCdReadStatus.
 * If not done, gets the new position via getGameStateS16. If the
 * target position (D_801E7D66) is valid and differs from the new
 * position, loads that card image to MENU_IMAGE_ADDR and resets both
 * position trackers to -1.
 */
void func_801E582C(CardMenuState *state) {
    int new_var;
    s16 value;
    if (pollCdReadStatus() == 0) {
        value = getGameStateS16();
        new_var = -1;
        D_801E7D64 = value;
        if (((D_801E7D66 >= 0) && (D_801E7D66 != value)) && (D_801E7D66 >= 0)) {
            loadOverlayWithTimCallback(D_801E7D66, MENU_IMAGE_ADDR);
            D_801E7D66 = new_var;
            D_801E7D64 = -1;
        }
    }
}

/**
 * Stores a0 + 0x30 as a card position offset into D_801E7D66.
 * @param a0 Card index or base position
 */
void func_801E58A4(s32 a0) {
    a0 += 0x30;
    D_801E7D66 = a0;
}

/**
 * Computes screen position for a card using divmod by 11.
 * The remainder selects the column (remainder * 13 + 0x26).
 * @param a0 Display mode parameter
 * @param a1 Card index (divided by 11 to get row/column)
 */
void func_801E58B4(s32 a0, s32 a1) {
    a1 %= 11;
    func_801F0A34(a0, 0, 0x25, a1 * 13 + 0x26);
}

/**
 * @brief Look up the menu text for card @p a0's level group.
 *
 * Picks entry 9 of menu text category 0xD for cards 0-54 (levels 1-5),
 * 10 for 55-76 (levels 6-7), 11 for 77-98 (levels 8-9) and 12 for the rest
 * (level 10).
 *
 * @param a0 Card ID.
 * @return The text, or NULL when func_80023B14 returns a negative value for the card.
 */
u8 *func_801E591C(s32 a0) {
    if (func_80023B14(a0) < 0) {
        return NULL;
    }
    if (a0 < 0x37) {
        return func_801E5800(9);
    }
    if (a0 < 0x4D) {
        return func_801E5800(0xA);
    }
    if (a0 < 0x63) {
        return func_801E5800(0xB);
    }
    return func_801E5800(0xC);
}

/**
 * @brief Count the card copies owned, per level group and in all.
 *
 * Adds up func_80023B14 over cards 0-54 (levels 1-5), 55-76 (levels 6-7),
 * 77-98 (levels 8-9) and 99-109 (level 10), skipping cards it reports
 * as zero or negative.
 *
 * @param state Card menu state.
 */
void func_801E5980(CardMenuState *state) {
    s32 sum = 0;
    s32 i = sum;
    s32 cardIdx = i;
    s32 count;

    state->groupCounts[0] = 0;
    state->groupCounts[1] = 0;
    state->groupCounts[2] = 0;
    state->groupCounts[3] = 0;
    state->totalCount = 0;

    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            state->groupCounts[0] += count;
        }
        i++;
        cardIdx++;
    } while (i < 55);

    i = 0;
    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            state->groupCounts[1] += count;
        }
        i++;
        cardIdx++;
    } while (i < 22);

    i = 0;
    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            state->groupCounts[2] += count;
        }
        i++;
        cardIdx++;
    } while (i < 22);

    i = 0;
    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            state->groupCounts[3] += count;
        }
        i++;
        cardIdx++;
    } while (i < 11);

    state->totalCount = sum;
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E5ABC);

/**
 * @brief Card menu tick: run the state machine, then the card image loader.
 * @param state Card menu state.
 */
void func_801E5F4C(CardMenuState *state) {
    func_801E5ABC(state);
    func_801E582C(state);
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E5F7C);

/**
 * Initializes a card dialog box with scroll parameters and
 * registers func_801E5F7C as the update callback.
 * @param state Card menu state.
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Dialog type identifier
 * @param stackArg Scroll offset
 */
s32 func_801E6058(CardMenuState *state, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0xF5;
    g_menuDisplayCfg.h = 0x16;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = stackArg;
    g_menuDisplayCfg.scrollOffset = state->scroll;
    g_menuDisplayCfg.dataPtr = (s32)state->rowText;
    g_menuDisplayCfg.itemId = state->card;
    g_menuDisplayCfg.itemAttr = state->unk2C;
    func_801EFBB4(a1, a2, func_801E5F7C);
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E60E8);

/**
 * @brief Draw the card backside (sprite 11 of the card table) at a screen position.
 *
 * @param ot Ordering-table entry the packet is linked into.
 * @param head Next free position in the packet buffer.
 * @param x Screen x of the card back.
 * @param y Screen y of the card back.
 * @return Next free position in the packet buffer, after the packet written.
 */
static void *DrawCardBacksideSprite(s32 unused, void *ot, void *head, s32 x, s32 y) {
    return DrawSpriteFromTable(ot, head, &g_currentPageCardTable, CARD_BACKSIDE_INDEX, x, y, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Render a card entry text with highlight detection.
 *
 * Computes divmod-by-11 on the card index at offset 0x2E to get
 * page (quotient) and column (remainder). If the page+0x30 matches
 * D_801E7D64 and the card is inactive (byte 0x42 == 0), renders
 * the text using the card remainder as the palette selector.
 * Otherwise renders with palette index 0xB.
 *
 * @param a0 Card entry pointer
 * @param a1 X position
 * @param a2 Y position
 * @param a3 Text index
 * @param stackArg Color/attribute
 * @return Result from DrawSpriteFromTable
 */
INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E6270);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E634C);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E645C);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E66AC);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E67E4);

/**
 * Initializes a card info dialog box and registers func_801E67E4
 * as the update callback.
 * @param state Card menu state.
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Dialog type identifier
 * @param stackArg Scroll offset
 */
s32 func_801E6920(CardMenuState *state, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.h = 0x16;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = stackArg;
    g_menuDisplayCfg.scrollOffset = state->scroll;
    g_menuDisplayCfg.dataPtr = (s32)state->rowText;
    g_menuDisplayCfg.itemId = state->card;
    g_menuDisplayCfg.itemAttr = state->unk2C;
    func_801EFBB4(a1, a2, func_801E67E4);
}

/**
 * @brief Render the full card detail screen layout.
 *
 * Sets up the rendering context, loads card data, then draws each
 * panel in sequence: the card image area, stat bars, card info
 * dialog, card name dialog, and the summary row. Returns the
 * accumulated render result.
 *
 * @param state Card menu state.
 * @param a1 X base position
 * @param a2 Y base position
 * @return Final render chain result
 */
s32 func_801E69AC(CardMenuState *state, s32 a1, s32 a2) {
    s32 result;
    s32 saved;
    s32 v1;

    saved = getDisplayListHead();
    func_801F1AFC();
    setMenuBrightness(state->intensity);
    v1 = 0x1E;
    result = func_801E645C(state, a1, a2, 0xC0, v1);
    v1 = 0x6A;
    result = func_801E66AC(state, a1, result, 0xC0, v1);
    v1 = 0xC0;
    result = func_801E6920(state, a1, result, 0x18, v1);
    v1 = 0x6;
    result = func_801E6058(state, a1, result, 0x18, v1);
    result = func_801E634C(state, a1, result, 0x1E, 0x1E);
    func_801F1B10();
    storeGpuPacket(saved);
    return result;
}

/**
 * Initializes the card menu: registers update/render callbacks,
 * clears position state, runs the setup loop, then optionally
 * initializes the first card entry.
 */
void func_801E6AA8(void) {
    CardMenuState *state = func_801F179C(func_801E5F4C, func_801E69AC);

    D_801E7D64 = -1;
    D_801E7D66 = -1;
    do {
    } while (pollCdReadStatus() != 0);
    func_801F0948(0);
    if (state != NULL) {
        state->unk42 = 1;
        state->intensity = 0;
        D_801E7D64 = -1;
        func_801E5980(state);
        func_801E58A4(0);
        func_801E5F4C(state);
    }
}

/**
 * @brief Look up card data by index and field selector.
 *
 * Uses a0 to index into D_801E6D20 (8-byte stride) and a1 to select
 * which field to return:
 *   1: Page number (a0 / 11 + 1)
 *   2-6: Byte fields 0-4 from the table entry
 *   7: Card name string pointer, with sub-dispatch based on card type
 *
 * @param a0 Card index
 * @param a1 Field selector (1-7)
 * @return Requested field value, or 0 if a1 is out of range
 */
INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E6B40);
