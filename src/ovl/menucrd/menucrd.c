#include "common.h"

extern s32 D_80083848;
extern s16 D_801E7D64;
extern s16 D_801E7D66;
extern u8 D_801FAB00;
extern u8 D_801E7870;

void func_801E582C(s32 a0);
void func_801E5ABC(s32 a0);
void func_801E5F7C(void);
void func_801E67E4(void);
void func_801E69AC(s32 a0, s32 a1, s32 a2);

/**
 * Allocates a menu resource of type 0xD for the card menu.
 * @param a0 Subtype parameter passed as third argument
 * @return Result from func_801F08D4
 */
s32 func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E582C);

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

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E5980);

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
    s32 base = (s32)&D_801FAB00;
    *(u8 *)(base + 0x10) = 0x55;
    *(u8 *)(base + 0x11) = 0;
    *(s16 *)&D_801FAB00 = a3;
    *(s16 *)(base + 0x04) = 0xF5;
    *(s16 *)(base + 0x06) = 0x16;
    *(u8 *)(base + 0x13) = 1;
    *(u8 *)(base + 0x16) = 0;
    *(u8 *)(base + 0x17) = 1;
    *(s16 *)(base + 0x02) = stackArg;
    *(s16 *)(base + 0x14) = *(u16 *)(a0 + 0x28);
    *(s32 *)(base + 0x20) = a0 + 0x20;
    *(u8 *)(base + 0x1E) = *(u8 *)(a0 + 0x2E);
    *(u8 *)(base + 0x1F) = *(u8 *)(a0 + 0x2C);
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
    return func_800376A8(a1, a2, (s32)&D_801E7870, 0xB, a3, stackArg, D_80083848);
}

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
    s32 base = (s32)&D_801FAB00;
    *(u8 *)(base + 0x10) = 0;
    *(u8 *)(base + 0x11) = 0;
    *(s16 *)&D_801FAB00 = a3;
    *(s16 *)(base + 0x04) = 0x150;
    *(s16 *)(base + 0x06) = 0x16;
    *(u8 *)(base + 0x13) = 1;
    *(u8 *)(base + 0x16) = 0;
    *(u8 *)(base + 0x17) = 1;
    *(s16 *)(base + 0x02) = stackArg;
    *(s16 *)(base + 0x14) = *(u16 *)(a0 + 0x28);
    *(s32 *)(base + 0x20) = a0 + 0x20;
    *(u8 *)(base + 0x1E) = *(u8 *)(a0 + 0x2E);
    *(u8 *)(base + 0x1F) = *(u8 *)(a0 + 0x2C);
    func_801EFBB4(a1, a2, (s32)func_801E67E4);
}

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E69AC);

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

INCLUDE_ASM("asm/ovl/menucrd/nonmatchings/menucrd", func_801E6B40);
