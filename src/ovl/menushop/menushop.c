#include "common.h"

/**
 * @brief Look up a shop item byte from a table or item data.
 *
 * If a2 >= 0xC6, returns 0. If a1 is non-zero, reads a base pointer
 * from a0+0x2C and returns the byte at offset a2*2. Otherwise reads
 * from D_801EAA28 at offset a2*2.
 *
 * @param a0 Shop context pointer.
 * @param a1 Source selector (0 = static table, non-zero = dynamic).
 * @param a2 Item index.
 * @return Item byte value, or 0 if out of range.
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5800);

/**
 * @brief Look up a shop item's category byte.
 *
 * If a1 is zero, returns the byte at D_801EAA28[a2*2 + 1] directly.
 * Otherwise, calls func_801E5800 to get an item data pointer, then
 * returns the byte at that offset in D_801EB088, or 0 if the pointer
 * is null.
 *
 * @param a0 Shop context parameter.
 * @param a1 Item category selector (0 = direct lookup).
 * @param a2 Item index.
 * @return Category byte value, or 0 if not found.
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E583C);

/**
 * @brief Look up shop item and get its description string.
 *
 * Calls func_801E583C to validate the item, then if valid, calls
 * func_801E5800 to get item data and func_80020D4C to get its
 * description string.
 *
 * @param a0 Shop context parameter
 * @param a1 Item category/type
 * @param a2 Item index
 * @return Description string pointer, or 0 if invalid
 */
s32 func_801E58A0(s32 a0, s32 a1, s32 a2) {
    s32 result = func_801E583C(a0, a1, a2);
    if (result != 0) {
        return func_80020D4C(func_801E5800(a0, a1, a2));
    }
    return 0;
}

/**
 * @brief Look up a shop item property via double indirection.
 *
 * Loads a byte from D_801EA70C[a0*4], uses it to index into D_801F7F98,
 * and returns that byte.
 *
 * @param a0 Shop item index.
 * @return Property byte value.
 */
s32 func_801E5904(s32 a0) {
    extern u8 D_801EA70C[];
    extern u8 D_801F7F98[];
    u8 idx = *(u8 *)(D_801EA70C + a0 * 4);
    return D_801F7F98[idx];
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5930);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E59D8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5A8C);

/**
 * @brief Render a shop item at a position from a decoded table.
 *
 * Decodes a position table from D_801E9B64 into a local buffer via
 * func_801E59D8, then uses the halfword at index a1 (plus 0x24) as
 * the Y position for func_801F0A34.
 *
 * @param a0 Render context / X position parameter.
 * @param a1 Index into the decoded position table.
 */
void func_801E5BA4(s32 a0, s32 a1) {
    extern u8 D_801E9B64[];
    s16 buf[36];
    func_801E59D8(D_801E9B64, buf, 3);
    func_801F0A34(a0, 0, buf[a1] + 0x24, 0x22);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5C08);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5D28);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5DBC);

void func_801E5E88(void) {
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5E90);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6A68);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6ACC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6C3C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6D54);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6E0C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6EB0);

/**
 * @brief Configure display parameters and invoke callback for shop list rendering.
 *
 * Sets up the g_menuDisplayCfg display configuration structure with the given
 * position and size values, stores the pointer at a0+0x20 as the data source,
 * reads a halfword at a0+0x3A as the display ID, then calls func_801EFBB4
 * with func_801E6EB0 as the render callback.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter (passed as a0 to func_801EFBB4).
 * @param a2 Second callback parameter (passed as a1 to func_801EFBB4).
 * @param a3 Y position for the display configuration.
 * @param arg5 X position for the display configuration.
 */
void func_801E6F60(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern u8 g_menuDisplayCfg[];
    s32 cfg = (s32)g_menuDisplayCfg;
    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)&g_menuDisplayCfg[0] = a3;
    *(s16 *)(cfg + 0x04) = 0x144;
    *(s16 *)(cfg + 0x06) = 0x14;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x02) = arg5;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x3A);
    *(s32 *)(cfg + 0x20) = (s32)(a0 + 0x20);
    {
        extern s32 func_801E6EB0;
        func_801EFBB4(a1, a2, (s32)&func_801E6EB0);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6FD8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E722C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7374);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7508);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7628);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E77EC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E791C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E79D4);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7B9C);

/**
 * @brief Process shop state and dispatch to appropriate handler.
 *
 * Calls initialization functions, then reads a state byte from
 * D_801E9B6C at the offset returned by func_801EFFF0. If the byte
 * equals 0x15, dispatches to func_801E9900; otherwise dispatches
 * to func_801E7B9C.
 *
 * @param a0 Shop context parameter passed to the handler.
 */
void func_801E7C8C(s32 a0) {
    extern u8 D_801E9B6C[];
    s32 off;
    func_801F0948(0);
    func_801F7B60();
    off = func_801EFFF0();
    if (*(u8 *)(off + (s32)D_801E9B6C) == 0x15) {
        func_801E9900(a0);
    } else {
        func_801E7B9C(a0);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7CFC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7D30);

/**
 * @brief Compute shop item price from table.
 *
 * Looks up byte at D_801E9BA0[a0*12 + 3], then returns (byte * 5) * 2.
 *
 * @param a0 Shop item index.
 * @return Computed price value.
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7E1C);

/**
 * @brief Test if bit a0 is set in D_80077E70.
 *
 * @param a0 Bit index.
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_801E7E4C(s32 a0) {
    extern s32 D_80077E70;
    s32 mask = 1 << a0;
    s32 val = D_80077E70 & mask;
    return val != 0;
}

/** @brief Return whether a1 >= func_801E7E1C(a0) (unsigned). */
s32 func_801E7E68(s32 a0, u32 a1) {
    return a1 >= (u32)func_801E7E1C(a0);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7E98);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8058);

/**
 * @brief Render shop item entry at computed Y position with width 0x24.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x50 for Y position)
 */
void func_801E8134(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x24, a1 * 13 + 0x50);
}

/**
 * @brief Render shop item quantity at computed Y position with width 0xA9.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x4F for Y position)
 */
void func_801E816C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xA9, a1 * 13 + 0x4F);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E81A4);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8978);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8AB0);

/**
 * @brief Configure display parameters and invoke callback for shop sell rendering.
 *
 * Sets up the g_menuDisplayCfg display configuration structure with the given
 * position and size values, stores the pointer at a0+0x20 as the data source,
 * reads a halfword at a0+0x36 as the display ID, then calls func_801EFBB4
 * with func_801E8AB0 as the render callback.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter (passed as a0 to func_801EFBB4).
 * @param a2 Second callback parameter (passed as a1 to func_801EFBB4).
 * @param a3 Y position for the display configuration.
 * @param arg5 X position for the display configuration.
 */
void func_801E8B60(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern u8 g_menuDisplayCfg[];
    s32 cfg = (s32)g_menuDisplayCfg;
    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)&g_menuDisplayCfg[0] = a3;
    *(s16 *)(cfg + 0x04) = 0x144;
    *(s16 *)(cfg + 0x06) = 0x14;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x02) = arg5;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x36);
    *(s32 *)(cfg + 0x20) = (s32)(a0 + 0x20);
    {
        extern s32 func_801E8AB0;
        func_801EFBB4(a1, a2, (s32)&func_801E8AB0);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8BD8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8D84);

/** @brief Return color code: 7 (equal), 3 (a0 > a1), 2 (a0 < a1). */
s32 func_801E8FF8(s32 a0, s32 a1) {
    s32 color = 7;
    if (a0 > a1) color = 3;
    if (a0 < a1) color = 2;
    return color;
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9020);

/**
 * @brief Initialize all 30 shop item entries.
 *
 * Calls func_801E9020 for indices 0 through 29.
 */
void func_801E90BC(void) {
    s32 i;
    for (i = 0; i < 30; i++) {
        func_801E9020(i);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E90F8);

/**
 * @brief Configure shop display and render with g_menuDisplayCfg settings.
 *
 * Calls func_801E90F8 with all parameters, then sets up g_menuDisplayCfg
 * display config (icon 0x57, 0x150 x 0x26, x=a3, y=arg5) and calls
 * func_801EF9AC to render.
 *
 * @param a0 Context pointer for func_801E90F8.
 * @param a1 Render context passed to func_801EF9AC.
 * @param a2 Parameter for func_801E90F8.
 * @param a3 X position for display config.
 * @param arg4 Y position for display config.
 */
void func_801E9554(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    extern u8 g_menuDisplayCfg[];
    extern s32 g_menuColor;
    s32 result;
    u8 *cfg;

    result = func_801E90F8(a0, a1, a2, a3, arg4);
    cfg = g_menuDisplayCfg;
    *(u8 *)(cfg + 0x10) = 0x57;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)&g_menuDisplayCfg[0] = a3;
    *(s16 *)(cfg + 0x04) = 0x150;
    *(s16 *)(cfg + 0x02) = arg4;
    *(s16 *)(cfg + 0x06) = 0x26;
    func_801EF9AC(a1, result, 0x1000, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E95DC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9684);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9900);
