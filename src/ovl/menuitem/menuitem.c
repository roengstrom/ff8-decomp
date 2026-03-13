#include "common.h"

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2800);

/**
 * @brief Read item menu state pointer.
 *
 * @return Value of D_801ECE20.
 */
s32 func_801E280C(void) {
    extern s32 D_801ECE20;
    return D_801ECE20;
}

/** @brief Draw inner panel with section id 0xB and clear flag. */
s32 func_801E281C(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2848);

/** @brief Store a2 to D_801ECE24 and call func_801E2848. */
void func_801E2BA4(s32 a0, s32 a1, s32 a2) {
    extern s32 D_801ECE24;
    D_801ECE24 = a2;
    func_801E2848();
}

/**
 * @brief Store 4 configuration values to globals and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE30
 * @param a3 Value stored to D_801ECE2C
 * @param arg5 Value stored to D_801ECE38
 * @param arg6 Value stored to D_801ECE28
 */
void func_801E2BC8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    extern s32 D_801ECE30;
    extern s32 D_801ECE2C;
    extern s32 D_801ECE28;
    extern s32 D_801ECE38;

    D_801ECE30 = a2;
    D_801ECE2C = a3;
    D_801ECE28 = arg6;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Store 3 configuration values and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE28
 * @param a3 Value stored to D_801ECE34
 * @param arg5 Value stored to D_801ECE38
 */
void func_801E2C0C(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern s32 D_801ECE28;
    extern s32 D_801ECE34;
    extern s32 D_801ECE38;

    D_801ECE28 = a2;
    D_801ECE34 = a3;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2C44);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2C80);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2CCC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2D54);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2E04);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2E38);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2E54);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2E70);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2E8C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2EA8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2F88);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E302C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3158);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3288);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3314);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E338C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E347C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E35B8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E37A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3854);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E38DC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3968);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3C1C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3E94);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3EBC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E42F8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4394);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E457C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E45B4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4608);

/**
 * @brief Render item entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x8D for Y)
 */
void func_801E46B8(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC1, (a1 % 4) * 13 + 0x8D);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4708);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E476C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E47E0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4848);

/**
 * @brief Render item at computed Y position based on row index.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E4908(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC8, a1 * 13 + 0x42);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4940);

/**
 * @brief Render item entry at position derived from index parity and half-index
 *
 * Uses the low bit of a1 to select a column (multiplied by 82 + 0xC4 for width),
 * and a1/2 to select a row (multiplied by 13 + 0x40 for Y position).
 *
 * @param a0 X position parameter
 * @param a1 Linear index (bit 0 = column, upper bits / 2 = row)
 */
void func_801E49FC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, (a1 & 1) * 82 + 0xC4, (a1 / 2) * 13 + 0x40);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4A58);

/**
 * @brief Dispatch based on upper/lower 16 bits of flags.
 *
 * If upper 16 bits are zero, calls func_801E4908 (normal render).
 * If upper bits non-zero but lower 16 bits are zero, calls func_801E49FC.
 *
 * @param a0 X position parameter
 * @param a1 Row index parameter
 * @param a2 Combined flags (upper 16 = type, lower 16 = subtype)
 */
void func_801E4B38(s32 a0, s32 a1, s32 a2) {
    if ((a2 & ~0xFFFF) == 0) {
        func_801E4908(a0, a1);
    } else if ((a2 & 0xFFFF) == 0) {
        func_801E49FC(a0, a1);
    }
}

/**
 * @brief Dispatch to different handler based on mode flag.
 * @param a0 Mode flag: non-zero calls func_801E4A58, zero calls func_801E4940
 * @param a1 Parameter passed to the selected handler
 */
void func_801E4B80(s32 a0, s32 a1) {
    if (a0 != 0) {
        func_801E4A58(a1);
    } else {
        func_801E4940(a1);
    }
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4BB4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4C14);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4C74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4D40);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4EA4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7D18);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7E74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8024);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E80D0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8180);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E820C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E82CC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E83B4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E84A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E859C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8684);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8780);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E88AC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E89A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E89C0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8AF0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8C88);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8DA4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8DB0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8E98);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8FA8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E90D8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E91E4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9248);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E934C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E95C4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9AAC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9B98);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9C90);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9CD4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9DE4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9E10);

/** @brief Draw inner panel with section id 0x5 and clear flag. */
s32 func_801E9E7C(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9EA8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9F94);

/**
 * @brief Call func_800375A0 with rearranged args and D_80083848 as 6th arg.
 * @param a0 First parameter passed through
 * @param a1 Second parameter passed through
 * @param a2 Becomes 4th argument to callee
 * @param a3 Becomes 5th argument (on stack) to callee
 * @param arg5 Becomes 3rd argument to callee
 */
void func_801EA500(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern s32 D_80083848;

    func_800375A0(a0, a1, arg5, a2, a3, D_80083848);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA538);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA714);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA7E0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA8F0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAA04);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAB00);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAB8C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAC54);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAD64);
