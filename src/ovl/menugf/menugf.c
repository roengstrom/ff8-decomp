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
 */
INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E592C);

/**
 * @brief Configure GF ability display by looking up base and alternate addresses.
 *
 * Calls func_80020F84(0x79) for a base lookup, passes the result with @p a1
 * to copyString. Then calls getMagicNamePtr on @p a0+0x40 for an alternate
 * lookup, passing that result with @p a1 to func_8002A2C4.
 *
 * @param a0 Pointer to GF ability data (offset +0x40 used for alternate lookup).
 * @param a1 Destination or context pointer for ability storage.
 */
void func_801E5988(u8 *a0, u8 *a1) {
    s32 val;
    val = func_80020F84(0x79);
    copyString(a1, val);
    val = getMagicNamePtr(a0 + 0x40);
    func_8002A2C4(a1, val);
}

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E59E0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5A60);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6884);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E69B0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6A8C);

/**
 * @brief Configure GF ability list display and register callback.
 *
 * Sets up g_menuDisplayCfg display config with icon 0x55, dimensions 0x144 x 0x1A,
 * scroll enabled, and page mode. Reads total item count from offset 0x28
 * and data pointer from offset 0x20. Registers func_801E6A8C as callback.
 *
 * @param a0 Source data structure.
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for display config.
 * @param arg4 Y position for display config.
 */
void func_801E6B3C(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    extern u8 g_menuDisplayCfg[];
    extern void func_801E6A8C();
    u8 *cfg = g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0x55;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)&g_menuDisplayCfg[0] = a3;
    *(s16 *)(cfg + 0x04) = 0x144;
    *(s16 *)(cfg + 0x06) = 0x1A;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x02) = arg4;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x28);
    *(s32 *)(cfg + 0x20) = (s32)(a0 + 0x20);
    func_801EFBB4(a1, a2, (s32)func_801E6A8C);
}

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6BB8);

/**
 * @brief Set up GF summary display with icon and border rendering.
 *
 * Calls func_80036F60 to get a status value, then passes it along with
 * position constants to func_801E69B0 for rendering. Configures g_menuDisplayCfg
 * with icon 0x50, position (0x18, 0x38), dimensions (0x150 x 0xA0), and
 * calls func_801EF9AC for final display.
 *
 * @param a0 GF context pointer.
 * @param a1 Render context.
 */
void func_801E6C84(s32 a0, s32 a1) {
    extern u8 g_menuDisplayCfg[];
    extern s32 g_menuColor;
    s32 ctx = a0;
    s32 renderCtx = a1;
    s32 result;
    u8 *cfg;

    result = func_80036F60() & 0xFFFF;
    result = func_801E69B0(ctx, renderCtx, 0x18, 0x38, result);
    cfg = g_menuDisplayCfg;
    *(u8 *)(cfg + 0x10) = 0x50;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)&g_menuDisplayCfg[0] = 0x18;
    *(s16 *)(cfg + 0x02) = 0x38;
    *(s16 *)(cfg + 0x04) = 0x150;
    *(s16 *)(cfg + 0x06) = 0xA0;
    func_801EF9AC(ctx, result, 0x1000, g_menuColor);
}

/**
 * @brief Render a GF ability entry with conditional highlight.
 *
 * Tests a bit in the g_gameState ability table at offset 0x4E8, indexed by
 * @p arg4 (stride 152) and shifted by @p arg6. If the bit is set, renders
 * a highlighted entry via func_8002FF34; otherwise skips. Then renders the
 * ability name and icon via func_801F0FEC and func_801F4EA8.
 *
 * @param a0    Entity/context pointer
 * @param a1    Display string or value (updated if highlighted)
 * @param a2    X position
 * @param a3    Y position
 * @param arg4  GF index (stride 152 into g_gameState)
 * @param arg5  Parameter passed to getCharNamePtr
 * @param arg6  Bit shift amount for ability flag test
 * @param arg7  Icon type (u16)
 * @param arg8  Extra parameter for func_801F0FEC
 */
void func_801E6D20(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4, volatile unsigned int arg5, s32 arg6, u16 arg7, s32 arg8) {
    int new_var5;
    s32 new_var;
    s32 new_var4;
    extern u8 g_gameState[];
    extern s32 g_menuColor;
    unsigned char new_var3;
    s32 base;
    s32 new_var2;
    s32 ret;
    s32 result;

    new_var = arg5;
    ret = getCharNamePtr(new_var);
    new_var3 = 2;
    new_var5 = 0x4E8;
    base = (s32)g_gameState;
    new_var2 = arg4;

    if (((*((u16 *)((base + ((2 * new_var2) * 76)) + new_var5))) >> arg6) & 1) {
        new_var4 = g_menuColor;
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

/**
 * @brief Initialize GF menu overlay and register callbacks.
 *
 * Registers func_801E5A60 as the update callback and func_801E7988 as
 * the render callback via func_801F179C. Calls func_801F0948(0) to
 * initialize shared state. If registration succeeds, clears all menu
 * context fields (positions, flags, scroll state), loads the current
 * GF's data via func_80036F60/func_80035A6C, and enters the update
 * callback.
 */
void func_801E7C20(s32 unused) {
    extern void func_801E5A60();
    extern void func_801E7988();
    s32 ctx;

    ctx = func_801F179C((s32)func_801E5A60, (s32)func_801E7988);
    func_801F0948(0);
    if (ctx != 0) {
        *(u8 *)(ctx + 0x30) = 0;
        *(u8 *)(ctx + 0x31) = 0;
        *(u8 *)(ctx + 0x36) = 0;
        *(u8 *)(ctx + 0x37) = 0;
        *(s16 *)(ctx + 0x28) = 0;
        *(u8 *)(ctx + 0x35) = 0;
        *(s32 *)(ctx + 0x20) = 0;
        *(s32 *)(ctx + 0x24) = 0;
        *(s16 *)(ctx + 0x2A) = 0;
        *(u8 *)(ctx + 0x34) = 0;
        *(s16 *)(ctx + 0x2E) = 0;
        *(u8 *)(ctx + 0x33) = func_80035A6C(func_80036F60());
        *(u8 *)(ctx + 0x56) = 0;
        *(u8 *)(ctx + 0x57) = 0xFF;
        func_801E5A60(ctx);
    }
}

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
