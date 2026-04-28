#include "common.h"

extern u8 D_80102F50[];
extern u8 D_80103050[];
extern u8 D_80103054[];
extern u8 D_80103058[];
extern u8 D_8010305C[];
extern u8 D_80103238[];
extern u8 D_80103248[];
extern u8 D_801032A0[];
extern u8 D_800EF2D0[];
extern u8 D_80103070[];
extern u8 D_801030F0[];
extern u8 D_80103160[];
extern u8 D_80103162[];
extern u8 D_800EF724[];
void func_800B3960(s32, s32, s32, s32);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D8FA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D8FE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9018);

/**
 * @brief Adjust and dispatch entity action based on field 0x27.
 *
 * If the byte at a0+0x27 is >= 2, subtracts the byte at a0+0x26
 * and calls func_800D0814 twice with channels 0 and 2.
 *
 * @param a0 Entity pointer with action fields at 0x26 and 0x27.
 */
void func_800D9060(u8 *a0) {
    s32 val = a0[0x27];
    if (val >= 2) {
        val -= a0[0x26];
        func_800D0814(0, val);
        func_800D0814(2, val);
    }
}

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

/**
 * @brief Read halfword at offset 0xE from current entity table entry.
 *
 * Reads D_80103238 to get the current index, computes D_80103248 + index * 44,
 * and returns the halfword at offset 0xE.
 *
 * @return Halfword value at the computed address.
 */
s32 func_800DB248(void) {
    u8 idx = *(u8 *)D_80103238;
    u8 *entry = D_80103248 + idx * 44;
    return *(u16 *)(entry + 0xE);
}

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

/**
 * @brief Decode mode bits from a1 and store type and raw value.
 *
 * Stores a1 as byte at a0[4], then computes a type value based on
 * bits 2-3 and bit 6 of a1, storing the result at a0[0xB].
 * Type = 3 if bits 2-3 == 1, type = 1 if bit 6 set, type = 2 otherwise.
 *
 * @param a0 Destination entry pointer.
 * @param a1 Source value with encoded mode bits.
 */
void func_800DBF50(u8 *a0, s32 a1) {
    s32 bits = (u32)(a1 & 0xC) >> 2;
    s32 type;
    a0[4] = a1;
    if (bits == 1) {
        type = 3;
    } else if (a1 & 0x40) {
        type = 1;
    } else {
        type = 2;
    }
    a0[0xB] = type;
}

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

/**
 * @brief Apply mode mask and find matching bit position.
 *
 * Calls func_800DBF84 to select a mask based on a0's mode bit,
 * then calls func_800DAD34 to find a matching bit in a1 using
 * that mask. Returns the result truncated to 16 bits.
 *
 * @param a0 Mode selector (bit 6 tested, low byte used).
 * @param a1 Value to search (16-bit).
 * @param a2 Mask source value (16-bit).
 * @return Matched bit position, masked to 16 bits.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC030);

/**
 * @brief Initialize 7 entries of D_801030F0 table (stride 0x10).
 *
 * For each entry: clears word at +0, clears bytes at +0xD and +0xE,
 * stores -128 as halfwords at +0x4, +0x6, +0x8, and +0xA.
 */
void func_800DC080(void) {
    s32 base = (s32)D_801030F0;
    s32 i = 0;
    do {
        i++;
        *(s32 *)base = 0;
        *(u8 *)(base + 0xD) = 0;
        *(u8 *)(base + 0xE) = 0;
        *(s16 *)(base + 0x8) = -0x80;
        *(s16 *)(base + 0xA) = -0x80;
        *(s16 *)(base + 0x4) = -0x80;
        *(s16 *)(base + 0x6) = -0x80;
        base += 0x10;
    } while (i < 7);
}

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

/**
 * @brief Clear display slots and reset counter fields.
 *
 * Zeroes D_80103160 and D_80103162, then clears 4 bytes at stride 0x20
 * starting from D_80103070 + 0x60 downward.
 */
void func_800DC62C(void) {
    s32 i = 3;
    s32 base = (s32)D_80103070;
    u8 *ptr = (u8 *)(base + 0x60);
    *(u16 *)D_80103160 = 0;
    *(u16 *)D_80103162 = 0;
    do {
        *ptr = 0;
        i--;
        ptr -= 0x20;
    } while (i >= 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC664);

/**
 * @brief Wrapper for func_800DC41C and func_800DB280.
 *
 * Passes all 6 arguments to func_800DC41C (with a1 and a2 truncated
 * to 8 bits), then calls func_800DB280.
 *
 * @param a0 First parameter.
 * @param a1 Second parameter (truncated to u8).
 * @param a2 Third parameter (truncated to u8).
 * @param a3 Fourth parameter.
 * @param a4 Fifth parameter (passed on stack).
 * @param a5 Sixth parameter (passed on stack).
 */
void func_800DC75C(s32 a0, u8 a1, u8 a2, s32 a3, s32 a4, s32 a5) {
    func_800DC41C(a0, a1, a2, a3, a4, a5);
    func_800DB280();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC798);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC820);

/**
 * @brief Initialize action entry and dispatch effect.
 *
 * Reads the current entity index from D_80103238, computes the entry
 * in D_80103248 at that index (stride 44). Calls func_800DC41C with
 * the provided arguments (masking a1 and a2 to bytes), sets the active
 * flag at entry + 0x28 to 1, then calls func_800DB280.
 *
 * @param a0 First parameter (passed through).
 * @param a1 Second parameter (masked to u8).
 * @param a2 Third parameter (masked to u8).
 * @param a3 Fourth parameter (passed through).
 * @param arg5 Fifth parameter (stack, passed through).
 * @param arg6 Sixth parameter (stack, passed through).
 */
void func_800DC8B8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    u8 idx = *(u8 *)D_80103238;
    u8 *entry = D_80103248 + idx * 44;
    func_800DC41C(a0, a1 & 0xFF, a2 & 0xFF, a3, arg5, arg6);
    entry[0x28] = 1;
    func_800DB280();
}

/**
 * @brief Wrapper for func_800B6A9C.
 */
void func_800DC928(void) {
    func_800B6A9C();
}

/**
 * @brief Check if entity at index has an active flag bit and status bit set.
 *
 * Tests bit a0 of D_800EF724 flag halfword. If set, checks bit 1 of the
 * entity's first halfword in D_800EF2D0 table (stride 156).
 *
 * @param a0 Entity index (also used as bit position in flags).
 * @return 1 if both bits are set, 0 otherwise.
 */
s32 func_800DC948(s32 a0) {
    s32 base = D_800EF2D0;
    s32 offset = a0 * 156;
    s32 flags;
    base += offset;
    flags = *(u16 *)D_800EF724;
    if ((flags >> a0) & 1) {
        if (*(u16 *)base & 2) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Compute entity entry from index and call func_800B3960.
 *
 * Multiplies index by 156 to get the entity entry in D_800EF2D0,
 * then calls func_800B3960 with the entry pointer, 0xF0, the
 * passthrough parameter, and the original second argument.
 *
 * @param index Entity index (stride 156).
 * @param a1 Parameter passed as 4th arg to func_800B3960.
 * @param a2 Passthrough parameter for func_800B3960's 3rd arg.
 */
void func_800DC9A0(s32 index, s32 a1, s32 a2) {
    s32 base = (s32)D_800EF2D0;
    func_800B3960(base + index * 156, 0xF0, a2, a1);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC9E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DCAA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DCC08);

void func_800DD1A8(void) {
}
