#include "common.h"

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E5800);

/**
 * @brief Render party member stat label at computed position.
 *
 * Decompresses coordinate table from D_801E9528, looks up a position
 * by the byte at a0+0x40, adds 0x32, and renders at that x-coordinate
 * with y=0xD.
 *
 * @param a0 Party context pointer.
 * @param a1 Render context pointer.
 */
void func_801E58EC(u8 *a0, s32 a1) {
    extern u8 D_801E9528[];
    s16 buf[36];
    func_801F5984(D_801E9528, buf, 4);
    func_801F0A34(a1, 0, buf[a0[0x40]] + 0x32, 0xD);
}

/** @brief Draw inner panel with section id 0x4 and clear flag. */
s32 func_801E5954(s32 a0) {
    return func_801F08D4(1, 4, a0, 0);
}

/** @brief Draw inner panel with section id 0x4 and set flag. */
s32 func_801E5980(s32 a0) {
    return func_801F08D4(1, 4, a0, 1);
}

/**
 * @brief Look up a party member attribute byte from a conditional table.
 *
 * If byte at a0+0x41 is non-zero, uses the index at a0+0x43 to read
 * from offset 0x47. Otherwise uses the index at a0+0x42 to read
 * from offset 0x44.
 *
 * @param a0 Party context pointer.
 * @return Attribute byte value.
 *
 * @note Non-matching: CC1PSX fills bnez delay slot with the first lbu
 * of the fall-through path and reverses addu operand order (v0+a0
 * instead of a0+v0).
 */
INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E59AC);

/**
 * @brief Compute party display offset based on mode flag.
 *
 * Returns a0 + a2 + 0x44 if a1 is zero, or a0 + a2 + 0x47 otherwise.
 *
 * @param a0 Base offset.
 * @param a1 Mode flag (0 = normal, non-zero = alternate).
 * @param a2 Additional offset.
 * @return Computed display position.
 */
s32 func_801E59EC(s32 a0, s32 a1, s32 a2) {
    s32 v0;
    if (a1 != 0) {
        v0 = a0 + 0x47;
    } else {
        v0 = a0 + 0x44;
    }
    return v0 + a2;
}

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E5A08);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E5B6C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E5CE8);

/** @brief Calls func_801E5B6C then func_801F5440 in sequence. */
void func_801E5DE4(void) {
    func_801E5B6C();
    func_801F5440();
}

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E5E0C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E5F94);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6898);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6948);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E69F4);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6AA0);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6B6C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6C68);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6D34);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6DDC);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E6F54);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E708C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E71A0);

/**
 * @brief Render party member display with position and graphics lookup.
 *
 * Calls func_801E5954(7) to get a graphics source, then calls func_801F4274
 * with position parameters, the graphics result, and fixed size constants.
 *
 * @param a0 Unused
 * @param a1 X position
 * @param a2 Y position
 */
void func_801E72BC(s32 a0, s32 a1, s32 a2) {
    s32 result = func_801E5954(7);

    func_801F4274(a1, a2, result, 0xC0, 0x6B, 0x1000);
}

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7318);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E74DC);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7530);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7584);

/**
 * @brief Initialize party menu and conditionally reset cursor.
 *
 * Clears D_801E9540 flag, calls func_801EFFB8. If result is 1,
 * calls func_80036690(0) to reset cursor position. Then calls
 * func_801E7584 with the party context.
 *
 * @param a0 Party context pointer.
 */
void func_801E77AC(u8 *a0) {
    extern u8 D_801E9540;
    s32 result;
    D_801E9540 = 0;
    result = func_801EFFB8();
    if (result == 1) {
        func_80036690(0);
    }
    func_801E7584(a0);
}

/**
 * @brief Initialize party menu: set mode 0x35, configure display, enable flag.
 *
 * Sets up the party menu display by calling initialization functions,
 * configuring display areas, setting the active flag D_801E9540 to 1,
 * then entering the main party menu handler.
 *
 * @param a0 Menu context pointer
 */
void func_801E77F8(s32 a0) {
    extern u8 D_801E9540;

    func_801F1DBC(0x35);
    func_801E2ABC(a0);
    func_801F1210(0x801D1000, 0x801CD000);
    D_801E9540 = 1;
    func_801E7584(a0);
}

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7854);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E78BC);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7990);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E79F8);

/**
 * @brief Call func_801E79F8 with party member attributes.
 *
 * If byte at a0+0x43 equals 3, passes attributes from offsets
 * 0x33, 0x37, 0x36. Otherwise passes from 0x32, 0x35, 0x34.
 *
 * @param a0 Party context pointer.
 */
void func_801E7A1C(u8 *a0) {
    if (a0[0x43] == 3) {
        func_801E79F8(a0, a0[0x33], a0[0x37], a0[0x36]);
    } else {
        func_801E79F8(a0, a0[0x32], a0[0x35], a0[0x34]);
    }
}

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7A6C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E7B9C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E88B4);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8964);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8A10);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8AEC);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8BF4);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8C8C);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8D58);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E8E00);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E9074);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E90D0);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E92B8);

INCLUDE_ASM("asm/ovl/menupty/nonmatchings/menupty", func_801E92FC);
