#include "common.h"

extern u8 D_80102E10[];
extern u8 D_80102E14[];
extern u8 D_801032F8[];

/**
 * @brief Call func_8002795C with a1=0, pass result to func_80030F10.
 *
 * @param a0 First argument passed through to func_8002795C.
 */
void func_800CFF2C(s32 a0) {
    func_80030F10(func_8002795C(a0, 0));
}

/**
 * @brief Call func_800279CC with a1=0, pass result to func_80030F10.
 *
 * @param a0 First argument passed through to func_800279CC.
 */
void func_800CFF54(s32 a0) {
    func_80030F10(func_800279CC(a0, 0));
}

/**
 * @brief Call func_80027A58 with a1=0, pass result to func_80030F10.
 *
 * @param a0 First argument passed through to func_80027A58.
 */
void func_800CFF7C(s32 a0) {
    func_80030F10(func_80027A58(a0, 0));
}

/**
 * @brief Call func_80027DB4 with a2 = 0.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800CFFA4(s32 a0, s32 a1) {
    func_80027DB4(a0, a1, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800CFFC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0530);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D056C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D05D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0608);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0714);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0760);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0814);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D082C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0844);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0868);

/**
 * @brief Wrapper for func_8002CE68.
 */
void func_800D08C4(void) {
    func_8002CE68();
}

/**
 * @brief Wrapper for func_8002A868.
 */
void func_800D08E4(void) {
    func_8002A868();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0904);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0920);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0970);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D09CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D09F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0B0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0B24);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0C3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0D54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0EF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0F74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0FB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D11D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D121C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D128C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D12E0);

/**
 * @brief Return the byte value at D_80102E14.
 *
 * @return Current value of D_80102E14 (unsigned byte).
 */
s32 func_800D1334(void) {
    return *(u8 *)D_80102E14;
}

/**
 * @brief Return constant 1.
 *
 * @return Always 1.
 */
s32 func_800D1344(void) {
    return 1;
}

/**
 * @brief Check if D_80102E10 is non-zero.
 *
 * @return 1 if D_80102E10 is non-zero, 0 otherwise.
 */
s32 func_800D134C(void) {
    return *(s32 *)D_80102E10 != 0;
}

/**
 * @brief Test if bit a0 is set in D_80102E10.
 *
 * Returns 0 if a0 is negative. Otherwise, checks if the bit
 * at position a0 in the D_80102E10 word is set.
 *
 * @param a0 Bit position to test (0-31). Returns 0 if negative.
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_800D135C(s32 a0) {
    s32 mask;
    s32 val;
    if (a0 < 0) {
        return 0;
    }
    mask = 1 << a0;
    val = *(s32 *)D_80102E10 & mask;
    return val != 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1388);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D13C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D13CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D15B4);

/**
 * @brief Clear D_80102E10 to zero.
 */
void func_800D1648(void) {
    *(s32 *)D_80102E10 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1654);

/**
 * @brief Clamp D_801032F8 to a minimum value of 2.
 *
 * If the current value is less than 2, sets it to 2.
 */
void func_800D16DC(void) {
    if (*(s32 *)D_801032F8 < 2) {
        *(s32 *)D_801032F8 = 2;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1700);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D17D8);

/**
 * @brief Clear D_801032F8 word and the halfword at offset 4.
 *
 * Zeros the 32-bit value at D_801032F8 and the 16-bit value at D_801032F8+4.
 */
void func_800D18B0(void) {
    s32 base = (s32)D_801032F8;
    *(s32 *)base = 0;
    *(s16 *)(base + 4) = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D18C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1940);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1A20);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1DAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1E74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D1F70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2044);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D22C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2424);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2464);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D261C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D26C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D29E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2A48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2DD4);

/**
 * @brief Call func_800D2DD4 with a1 = 0.
 *
 * @param a0 First argument passed through.
 */
void func_800D2ED4(s32 a0) {
    func_800D2DD4(a0, 0);
}

/**
 * @brief Call func_800D2DD4 with a1 = 1.
 *
 * @param a0 First argument passed through.
 */
void func_800D2EF4(s32 a0) {
    func_800D2DD4(a0, 1);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D2F14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D3044);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D3090);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D30B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D3164);
