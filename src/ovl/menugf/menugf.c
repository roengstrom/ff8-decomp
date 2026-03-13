#include "common.h"

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5800);

/**
 * @brief Display a GF stat value from a lookup table.
 *
 * Copies 8 halfwords from D_801E7DD0 into a local buffer, indexes
 * by @p a1 to get a base value, adds 0x32, and renders via
 * func_801F0A34 at position (a0, 0) with width 0xD.
 *
 * @param a0 Render context pointer.
 * @param a1 Index into the stat table (0-7).
 */
void func_801E58C8(u8 *a0, s32 a1) {
    extern u8 D_801E7DD0[];
    s16 buf[32];
    func_801F5984(D_801E7DD0, buf, 8);
    func_801F0A34(a0, 0, buf[a1] + 0x32, 0xD);
}

/**
 * @brief Render a GF ability icon at a grid position.
 *
 * Computes screen coordinates from a linear index @p a0 using an
 * 8-column grid layout: column = a0 % 8 (x = col * 40 + 0x24),
 * row = a0 / 8 (y = row * 72 + 0x46). Calls func_801F0994 to draw.
 *
 * @param a0 Linear index into the ability grid.
 *
 * @note Non-matching: Register allocation differs — compiler puts row in
 * v1 directly instead of a1 with copy to v1 as in original.
 */
INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E592C);

/**
 * @brief Configure GF ability display by looking up base and alternate addresses.
 *
 * Calls func_80020F84(0x79) for a base lookup, passes the result with @p a1
 * to func_8002A2A8. Then calls func_80020C08 on @p a0+0x40 for an alternate
 * lookup, passing that result with @p a1 to func_8002A2C4.
 *
 * @param a0 Pointer to GF ability data (offset +0x40 used for alternate lookup).
 * @param a1 Destination or context pointer for ability storage.
 */
void func_801E5988(u8 *a0, u8 *a1) {
    s32 val;
    val = func_80020F84(0x79);
    func_8002A2A8(a1, val);
    val = func_80020C08(a0 + 0x40);
    func_8002A2C4(a1, val);
}

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E59E0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5A60);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6884);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E69B0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6A8C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6B3C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6BB8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6C84);

/**
 * @brief Render a GF ability entry with conditional highlight.
 *
 * Tests a bit in the D_80077378 ability table at offset 0x4E8, indexed by
 * @p arg4 (stride 152) and shifted by @p arg6. If the bit is set, renders
 * a highlighted entry via func_8002FF34; otherwise skips. Then renders the
 * ability name and icon via func_801F0FEC and func_801F4EA8.
 *
 * @param a0    Entity/context pointer
 * @param a1    Display string or value (updated if highlighted)
 * @param a2    X position
 * @param a3    Y position
 * @param arg4  GF index (stride 152 into D_80077378)
 * @param arg5  Parameter passed to func_80020E4C
 * @param arg6  Bit shift amount for ability flag test
 * @param arg7  Icon type (u16)
 * @param arg8  Extra parameter for func_801F0FEC
 */
void func_801E6D20(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4, volatile unsigned int arg5, s32 arg6, u16 arg7, s32 arg8) {
    int new_var5;
    s32 new_var;
    s32 new_var4;
    extern u8 D_80077378[];
    extern s32 D_80083848;
    unsigned char new_var3;
    s32 base;
    s32 new_var2;
    s32 ret;
    s32 result;

    new_var = arg5;
    ret = func_80020E4C(new_var);
    new_var3 = 2;
    new_var5 = 0x4E8;
    base = (s32)D_80077378;
    new_var2 = arg4;

    if (((*((u16 *)((base + ((2 * new_var2) * 76)) + new_var5))) >> arg6) & 1) {
        new_var4 = D_80083848;
        result = (new_var2 = new_var4);
        a1 = func_8002FF34(a0, a1, 0xC0, a2 - 10, a3, result);
    }

    arg4 = a2;
    do { new_var2 = arg4; } while (0);
    arg6 = a3;
    new_var4 = arg6;
    result = func_801F0FEC(a0, a1, arg4, arg6, ret, arg8);
    func_801F4EA8(a0, ret = result, new_var2 + 0x6C, new_var4 + new_var3, arg7);
}

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6E3C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E70A8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E733C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7480);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E77B4);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7988);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7C20);

/**
 * @brief Reset GF menu state and redraw.
 *
 * Calls func_801F1DBC to set menu mode to 4 (GF submenu reset),
 * clears the D_801E7E88 flag, and calls func_801E7C20 to redraw.
 *
 * @param a0 Pointer to GF menu context.
 */
void func_801E7CB8(u8 *a0) {
    extern u8 D_801E7E88;
    func_801F1DBC(4);
    D_801E7E88 = 0;
    func_801E7C20(a0);
}

/**
 * @brief Initialize GF menu state and load resources.
 *
 * Sets menu mode to 4 via func_801F1DBC, enables the D_801E7E88 flag,
 * calls func_801E2ABC for setup, loads resources between 0x801D1000
 * and 0x801CD000 via func_801F1210, and redraws via func_801E7C20.
 *
 * @param a0 Pointer to GF menu context.
 */
void func_801E7CF4(u8 *a0) {
    extern u8 D_801E7E88;
    func_801F1DBC(4);
    D_801E7E88 = 1;
    func_801E2ABC(a0);
    func_801F1210(0x801D1000, 0x801CD000);
    func_801E7C20(a0);
}
