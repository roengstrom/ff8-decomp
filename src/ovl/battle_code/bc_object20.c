#include "common.h"

extern u8 D_80102F50[];
extern u8 D_80103050[];
extern u8 D_80103054[];
extern u8 D_80103058[];
extern u8 D_8010305C[];
extern u8 D_80103238[];
extern u8 D_80103248[];
extern u8 D_801032A0[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D8FA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D8FE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9018);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9060);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D90B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9274);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9794);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9934);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9AD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9E84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA3C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA3F0);

/**
 * @brief Read byte at offset 0x27 from current entity table entry.
 *
 * Reads D_80103238 to get the current index, computes D_80103248 + index * 44,
 * and returns the byte at offset 0x27.
 *
 * @return Byte value at the computed address.
 */
s32 func_800DA574(void) {
    u8 idx = *(u8 *)D_80103238;
    u8 *entry = D_80103248 + idx * 44;
    return entry[0x27];
}

/**
 * @brief Advance render buffer index and update pointer (buffer A).
 *
 * Increments D_80103050 mod 4, computes the new buffer pointer
 * as D_80102F50 + index * 64, and stores it to D_80103058.
 */
void func_800DA5AC(void) {
    s32 idx = (*(s32 *)D_80103050 + 1) & 3;
    *(s32 *)D_80103058 = (s32)D_80102F50 + idx * 64;
    *(s32 *)D_80103050 = idx;
}

/**
 * @brief Advance render buffer index and update pointer (buffer B).
 *
 * Increments D_80103054 mod 4, computes the new buffer pointer
 * as D_80102F50 + index * 64, and stores it to D_8010305C.
 */
void func_800DA5DC(void) {
    s32 idx = (*(s32 *)D_80103054 + 1) & 3;
    *(s32 *)D_8010305C = (s32)D_80102F50 + idx * 64;
    *(s32 *)D_80103054 = idx;
}

/**
 * @brief Return the word value at D_80103058.
 *
 * @return Current value of D_80103058.
 */
s32 func_800DA60C(void) {
    return *(s32 *)D_80103058;
}

/**
 * @brief Return the word value at D_8010305C.
 *
 * @return Current value of D_8010305C.
 */
s32 func_800DA61C(void) {
    return *(s32 *)D_8010305C;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA62C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA634);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA650);

/**
 * @brief Check if bit 15 of a halfword at offset 0x1C is clear.
 *
 * Calls func_800DA61C to get a base pointer, reads the halfword at +0x1C,
 * and returns whether bit 15 (0x8000) is zero.
 *
 * @return 1 if bit 15 is clear, 0 if set.
 */
s32 func_800DA718(void) {
    s32 val = *(u16 *)(func_800DA61C() + 0x1C) & 0x8000;
    return val == 0;
}

/**
 * @brief Find the position of the lowest set bit in a value.
 *
 * @param val Bitmask to search.
 * @return Bit position of the lowest set bit, or 0 if val is 0.
 */
s32 func_800DA744(s32 val) {
    s32 mask = 1;
    s32 i;

    if (val == 0) {
        return 0;
    }
    i = 0;
    top:
    if (val & mask) {
        return i;
    }
    mask <<= 1;
    i++;
    goto top;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA778);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA938);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA984);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DAA30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DAD34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DAD78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DADB4);

/**
 * @brief Extract bits from entity byte and call func_800D32E8.
 *
 * Reads byte at a0+0x24, passes bit 7 inverted as first arg
 * and bit 6 as second arg to func_800D32E8.
 *
 * @param a0 Pointer to entity data.
 */
void func_800DB110(s32 a0) {
    s32 val = *(u8 *)(a0 + 0x24);
    s32 bit = val & 0x80;
    func_800D32E8(bit == 0, val & 0x40);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB140);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB1BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB248);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB280);

/**
 * @brief Store byte at offset 0x25 in current entity table entry.
 *
 * Reads D_80103238 to get the current index, computes D_80103248 + index * 44,
 * and stores a0 as a byte at offset 0x25.
 *
 * @param a0 Byte value to store.
 */
void func_800DBC88(s32 a0) {
    u8 idx = *(u8 *)D_80103238;
    u8 *entry = D_80103248 + idx * 44;
    entry[0x25] = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBCBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBF50);

/**
 * @brief Select a mask from a1 based on bit 6 of a0.
 *
 * @param a0 Selector value (bit 6 tested).
 * @param a1 Value to mask.
 * @return a1 & 0x78 if bit 6 is set, otherwise a1 & 0x7.
 */
s32 func_800DBF84(s32 a0, s32 a1) {
    s32 result = a1 & 0x78;
    if (!(a0 & 0x40)) {
        result = a1 & 0x7;
    }
    return result;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBF9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC030);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC080);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC0CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC334);

/**
 * @brief Clear position halfwords in current entity entry and reset state.
 *
 * Reads current entity index from D_80103238, computes entry in D_80103248
 * table (stride 44), clears halfwords at offsets 0x20 and 0x22, then calls
 * func_800DC334.
 */
void func_800DC3D0(void) {
    u8 idx = *(u8 *)D_80103238;
    u8 *entry = D_80103248 + idx * 44;
    *(u16 *)(entry + 0x22) = 0;
    *(u16 *)(entry + 0x20) = 0;
    func_800DC334();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC41C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC62C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC664);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC75C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC798);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC820);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC8B8);

/**
 * @brief Wrapper for func_800B6A9C.
 */
void func_800DC928(void) {
    func_800B6A9C();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC948);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC9A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC9E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DCAA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DCC08);

void func_800DD1A8(void) {
}
