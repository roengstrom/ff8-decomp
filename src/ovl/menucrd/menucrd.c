#include "common.h"
#include "menu.h"

extern s32 g_menuColor;
extern s16 D_801E7D64;
extern s16 D_801E7D66;
extern MenuDisplayConfig g_menuDisplayCfg;
extern u8 D_801E7870;

extern s32 func_80035E00(void);
extern s16 func_80035E50(void);
extern void func_800361F8(s16 id, void *addr);

void func_801E582C(s32 a0);
void func_801E5ABC(s32 a0);
void func_801E5F7C(void);
void func_801E67E4(void);
s32 func_801E69AC(s32 a0, s32 a1, s32 a2);

/**
 * Allocates a menu resource of type 0xD for the card menu.
 * @param a0 Subtype parameter passed as third argument
 * @return Result from func_801F08D4
 */
s32 func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/**
 * @brief Update card display position after a transition completes.
 *
 * Checks if the card transition animation is done via func_80035E00.
 * If not done, gets the new position via func_80035E50. If the
 * target position (D_801E7D66) is valid and differs from the new
 * position, triggers a card load at 0x801CD000 and resets both
 * position trackers to -1.
 */
void func_801E582C(s32 a0) {
    int new_var;
    s16 value;
    if (func_80035E00() == 0) {
        value = func_80035E50();
        new_var = -1;
        D_801E7D64 = value;
        if (((D_801E7D66 >= 0) && (D_801E7D66 != value)) && (D_801E7D66 >= 0)) {
            func_800361F8(D_801E7D66, (void *) 0x801CD000);
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
 * Selects a menu icon based on card count ranges.
 * Validates the card with func_80023B14, then maps the count
 * to one of four icon indices (9-12) based on thresholds.
 * @param a0 Card identifier
 * @return func_801E5800 result for the selected icon, or 0 if invalid
 */
s32 func_801E591C(s32 a0) {
    if (func_80023B14(a0) < 0) {
        return 0;
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
 * @brief Tally card counts by rarity tier and store totals.
 *
 * Iterates through all card indices calling func_80023B14 to get
 * each card's count. Accumulates counts into per-tier totals stored
 * at offsets 0x32-0x38, and a grand total at offset 0x3A.
 * Tier ranges: 0-0x36 (common), 0x37-0x4C (uncommon),
 * 0x4D-0x62 (rare), 0x63-0x6D (legendary).
 *
 * @param a0 Card data structure pointer
 */
void func_801E5980(s32 a0) {
    s32 sum = 0;
    s32 i = sum;
    s32 cardIdx = i;
    s32 count;

    *(s16 *)(a0 + 0x32) = 0;
    *(s16 *)(a0 + 0x34) = 0;
    *(s16 *)(a0 + 0x36) = 0;
    *(s16 *)(a0 + 0x38) = 0;
    *(s16 *)(a0 + 0x3A) = 0;

    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            *(u16 *)(a0 + 0x32) = *(u16 *)(a0 + 0x32) + count;
        }
        i++;
        cardIdx++;
    } while (i < 0x37);

    i = 0;
    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            *(u16 *)(a0 + 0x34) = *(u16 *)(a0 + 0x34) + count;
        }
        i++;
        cardIdx++;
    } while (i < 0x16);

    i = 0;
    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            *(u16 *)(a0 + 0x36) = *(u16 *)(a0 + 0x36) + count;
        }
        i++;
        cardIdx++;
    } while (i < 0x16);

    i = 0;
    do {
        count = func_80023B14(cardIdx);
        if (count > 0) {
            sum += count;
            *(u16 *)(a0 + 0x38) = *(u16 *)(a0 + 0x38) + count;
        }
        i++;
        cardIdx++;
    } while (i < 0x0B);

    *(s16 *)(a0 + 0x3A) = sum;
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E5ABC);

/**
 * Processes a card menu entry by running the state machine
 * and then updating the display.
 * @param a0 Card entry pointer
 */
void func_801E5F4C(s32 a0) {
    func_801E5ABC(a0);
    func_801E582C(a0);
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E5F7C);

/**
 * Initializes a card dialog box with scroll parameters and
 * registers func_801E5F7C as the update callback.
 * @param a0 Source card data pointer
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Dialog type identifier
 * @param stackArg Scroll offset
 */
s32 func_801E6058(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0xF5;
    g_menuDisplayCfg.h = 0x16;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = stackArg;
    g_menuDisplayCfg.scrollOffset = *(u16 *)(a0 + 0x28);
    g_menuDisplayCfg.dataPtr = a0 + 0x20;
    g_menuDisplayCfg.itemId = *(u8 *)(a0 + 0x2E);
    g_menuDisplayCfg.itemAttr = *(u8 *)(a0 + 0x2C);
    func_801EFBB4(a1, a2, (s32)func_801E5F7C);
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E60E8);

/**
 * Draws a text string at a given position with standard card palette.
 * Passes through coordinates and string data to the text renderer
 * with palette index 0xB and the shared string table.
 * @param a0 Unused
 * @param a1 X position
 * @param a2 Y position
 * @param a3 Text index (passed as 5th arg to renderer)
 * @param stackArg Color/attribute (passed as 6th arg)
 * @return Result from func_800376A8
 */
s32 func_801E6228(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    return func_800376A8(a1, a2, (s32)&D_801E7870, 0xB, a3, stackArg, g_menuColor);
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
 * @return Result from func_800376A8
 */
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
 * @return Result from func_800376A8
 */
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
 * @return Result from func_800376A8
 *
 * @note Non-matching: Temp register assignment for saved arguments
 * differs — compiler assigns a0→t2/a3→t1 instead of a0→t0/a1→t1/a3→t2.
 * The cross-jumping of the two func_800376A8 calls matches structurally.
 */
INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E6270);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E634C);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E645C);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E66AC);

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E67E4);

/**
 * Initializes a card info dialog box and registers func_801E67E4
 * as the update callback.
 * @param a0 Source card data pointer
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Dialog type identifier
 * @param stackArg Scroll offset
 */
s32 func_801E6920(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.h = 0x16;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = stackArg;
    g_menuDisplayCfg.scrollOffset = *(u16 *)(a0 + 0x28);
    g_menuDisplayCfg.dataPtr = a0 + 0x20;
    g_menuDisplayCfg.itemId = *(u8 *)(a0 + 0x2E);
    g_menuDisplayCfg.itemAttr = *(u8 *)(a0 + 0x2C);
    func_801EFBB4(a1, a2, (s32)func_801E67E4);
}

/**
 * @brief Render the full card detail screen layout.
 *
 * Sets up the rendering context, loads card data, then draws each
 * panel in sequence: the card image area, stat bars, card info
 * dialog, card name dialog, and the summary row. Returns the
 * accumulated render result.
 *
 * @param a0 Card entry pointer
 * @param a1 X base position
 * @param a2 Y base position
 * @return Final render chain result
 */
s32 func_801E69AC(s32 a0, s32 a1, s32 a2) {
    s32 result;
    s32 saved;
    s32 v1;

    saved = func_8002A888();
    func_801F1AFC();
    func_8002E7C4(*(s16 *)(a0 + 0x30));
    v1 = 0x1E;
    result = func_801E645C(a0, a1, a2, 0xC0, v1);
    v1 = 0x6A;
    result = func_801E66AC(a0, a1, result, 0xC0, v1);
    v1 = 0xC0;
    result = func_801E6920(a0, a1, result, 0x18, v1);
    v1 = 0x6;
    result = func_801E6058(a0, a1, result, 0x18, v1);
    result = func_801E634C(a0, a1, result, 0x1E, 0x1E);
    func_801F1B10();
    func_8002A8B8(saved);
    return result;
}

/**
 * Initializes the card menu: registers update/render callbacks,
 * clears position state, runs the setup loop, then optionally
 * initializes the first card entry.
 */
void func_801E6AA8(void) {
    s32 result = func_801F179C((s32)func_801E5F4C, (s32)func_801E69AC);

    D_801E7D64 = -1;
    D_801E7D66 = -1;
    do {
    } while (func_80035E00() != 0);
    func_801F0948(0);
    if (result != 0) {
        *(u8 *)(result + 0x42) = 1;
        *(s16 *)(result + 0x30) = 0;
        D_801E7D64 = -1;
        func_801E5980(result);
        func_801E58A4(0);
        func_801E5F4C(result);
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
 *
 * @note Non-matching: Contains a 7-case switch with a jump table in .rodata
 * (jtbl_801E6CDC). Decomping to C generates a new jump table, but the
 * original asm jump table in rodata references .L labels that no longer
 * exist, causing linker errors. Requires rodata segment splitting in the
 * yaml to separate the jump table from other rodata.
 */
INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E6B40);
