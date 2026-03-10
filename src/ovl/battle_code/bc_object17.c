#include "common.h"

extern u8 D_80102E10[];
extern u8 D_80102E14[];
extern u8 D_801032F8[];
extern u8 D_80103420[];

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

/**
 * @brief Disable display, set bit 10 of scratchpad control register, enable display.
 *
 * Sets bit 0x400 in the halfword at scratchpad address 0x1F8003AE.
 */
void func_800D0530(void) {
    s32 base;
    func_800472E4();
    base = 0x1F800390;
    *(u16 *)(base + 0x1E) |= 0x400;
    func_800472F4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D056C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D05D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0608);

extern u8 D_800FB440[];

/**
 * @brief Store pointer, run two init functions, then poll until complete.
 *
 * Stores a0 to D_800FB440, calls func_800CF3F0 with a1, func_800CF40C
 * with a2, then polls func_800CF3FC in a loop until it returns 0.
 *
 * @param a0 Pointer to store in D_800FB440.
 * @param a1 First init parameter.
 * @param a2 Second init parameter.
 */
void func_800D0714(s32 a0, s32 a1, s32 a2) {
    *(s32 *)D_800FB440 = a0;
    func_800CF3F0(a1);
    func_800CF40C(a2);
    do {
    } while (func_800CF3FC() != 0);
}

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

/**
 * @brief Disable display, set bit 3 of scratchpad control register, enable display.
 *
 * Sets bit 0x8 in the halfword at scratchpad address 0x1F8003AE.
 */
void func_800D0F74(void) {
    s32 base;
    func_800472E4();
    base = 0x1F800390;
    *(u16 *)(base + 0x1E) |= 0x8;
    func_800472F4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D0FB0);

/**
 * @brief Disable display, check condition, optionally call handler, re-enable display.
 *
 * Calls func_800472E4 to disable display, then checks func_800CEDA4.
 * If it returns 0, calls func_80030D48 with a0. Finally re-enables display
 * via func_800472F4.
 *
 * @param a0 Parameter passed to func_80030D48 if condition met.
 */
void func_800D11D4(s32 a0) {
    func_800472E4();
    if (func_800CEDA4() == 0) {
        func_80030D48(a0);
    }
    func_800472F4();
}

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

/**
 * @brief Read fields from a0 and call func_800D1E74 with rearranged args.
 *
 * Reads halfwords at offsets 8 and 0xA from a0, adds constants, and calls
 * func_800D1E74 with shuffled parameters.
 *
 * @param a0 Pointer to source data with halfwords at +8 and +0xA.
 * @param a1 Passed as first arg to func_800D1E74.
 * @param a2 Passed as second arg to func_800D1E74.
 */
void func_800D2424(u8 *a0, s32 a1, s32 a2) {
    u16 field8 = *(u16 *)(a0 + 8);
    u16 fieldA = *(u16 *)(a0 + 0xA);
    func_800D1E74(a1, a2, (s32)a0, field8 + 0x61, fieldA + 2);
}

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

/**
 * @brief Initialize entity fields and call func_800D422C.
 *
 * Sets various bytes and halfwords in the entity structure pointed to by a0.
 * Stores a1 to offset 0x48, clears multiple status fields, sets byte at 0x51
 * to 1, then calls func_800D422C.
 *
 * @param a0 Pointer to entity data.
 * @param a1 Value for the command byte at offset 0x48.
 */
void func_800D3044(u8 *a0, s32 a1) {
    a0[0x48] = a1;
    a0[0x3F] = 0;
    a0[0x50] = 0;
    a0[0x51] = 1;
    *(u16 *)(a0 + 0x40) = 0;
    *(u16 *)(a0 + 0x42) = 0;
    a0[0x3C] = 0;
    a0[0x3D] = 0;
    a0[0x3E] = 0;
    *(u16 *)(a0 + 0x42) = 0;
    a0[0x49] = 0;
    func_800D422C(a0);
}

/**
 * @brief Store a byte value at index into the D_80103420 table.
 *
 * Computes D_80103420 + index * 108 + 0x51, stores val there.
 *
 * @param index Table index (stride 108).
 * @param val Byte value to store.
 */
void func_800D3090(s32 index, s32 val) {
    s32 base = (s32)D_80103420;
    base += index * 108;
    *(u8 *)(base + 0x51) = val;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D30B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object17", func_800D3164);
