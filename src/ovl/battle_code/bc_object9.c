#include "common.h"

extern u8 D_800EF72C[];
extern u8 D_800F02C8[];
extern u8 D_800F05C8[];
extern u8 D_800F0290[];
void func_800B5B48(void);
extern u8 D_800F02F8[];
extern u8 D_800F0308[];
extern u8 D_800F0408[];
extern u8 D_800F0578[];
extern u8 D_800EEC5C[];
extern u8 D_800EF2D0[];
void func_800B8314(void);
void func_800B8870(u8 *, s32);
void func_80040FA4(s32, s32);
void func_8003FD84(s32, s32, s32);
void func_80040264(s32, s32);
s32 func_8013E000(s32);
void func_800B8F4C(s32);
s32 func_800B5604(u8 *);
void func_800C2B88(u8 *);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B49D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4A74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4BAC);

/**
 * @brief Set or clear a bit in an entity's flags word and update state.
 *
 * Computes the entity at D_800EF2D0 + a0 * 0x9C. Creates a bitmask
 * from 1 << a1. Loads the word at entity+8, clears the bit. If a2 is
 * nonzero, sets the bit instead. Calls func_800B5C10 to update the
 * entity, then func_800B56B8 to finalize.
 *
 * @param a0 Entity index (stride 0x9C in D_800EF2D0).
 * @param a1 Bit position to modify.
 * @param a2 If nonzero, set the bit; if zero, clear it.
 */
void func_800B4DE8(s32 a0, s32 a1, s32 a2) {
    u8 *entity = D_800EF2D0 + a0 * 0x9C;
    s32 mask = 1 << a1;
    s32 flags = *(s32 *)(entity + 8) & ~mask;
    if (a2 != 0) {
        flags |= mask;
    }
    func_800B5C10(entity, flags);
    func_800B56B8(entity);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4E54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B51F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B53F8);

/**
 * @brief Compute 3D vector differences and initialize transformation data.
 *
 * Computes differences of 3 words at offsets 0x14, 0x18, 0x1C between
 * a1 and a0, stores them in a2 at the same offsets. Then calls three
 * transformation functions to finalize the setup.
 *
 * @param a0 Source position data.
 * @param a1 Target position data.
 * @param a2 Output delta structure.
 */
void func_800B54A0(s32 a0, s32 a1, s32 a2) {
    s32 dst = a2;
    *(s32 *)(dst + 0x14) = *(s32 *)(a1 + 0x14) - *(s32 *)(a0 + 0x14);
    *(s32 *)(dst + 0x18) = *(s32 *)(a1 + 0x18) - *(s32 *)(a0 + 0x18);
    *(s32 *)(dst + 0x1C) = *(s32 *)(a1 + 0x1C) - *(s32 *)(a0 + 0x1C);
    func_80040FA4(a0, dst);
    func_8003FD84(dst, dst + 0x14, dst + 0x14);
    func_80040264(dst, a1);
}

/**
 * @brief Compute coordinate differences and call func_80041E84.
 *
 * @param a0 Pointer to first coordinate pair (s16 x at +0, s16 y at +4).
 * @param a1 Pointer to second coordinate pair (s16 x at +0, s16 y at +4).
 * @return Result of func_80041E84(dx, dy).
 */
s32 func_800B5528(s32 a0, s32 a1) {
    s32 dx = *(s16 *)a0 - *(s16 *)a1;
    s32 dy = *(s16 *)(a0 + 4) - *(s16 *)(a1 + 4);
    return func_80041E84(dx, dy);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B555C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5604);

/**
 * @brief Update entity render state based on computed attribute.
 *
 * If entity byte at offset 4 is >= 16, returns immediately. Otherwise
 * calls func_800B5604 to compute the current attribute. Loads the
 * entity's render pointer at offset 0x74 and compares byte[3] and
 * byte[0] against the computed attribute:
 * - If byte[3] is non-zero and byte[0] matches, clears byte[3].
 * - If byte[3] is non-zero and byte[0] differs, sets byte[3] to attribute.
 * - If byte[3] is zero and byte[0] differs, calls func_800C2B88.
 *
 * @param a0 Pointer to entity structure.
 */
void func_800B56B8(u8 *a0) {
    u8 *ptr;
    s32 attr;

    if (a0[4] >= 0x10) {
        return;
    }
    attr = func_800B5604(a0);
    ptr = *(u8 **)(a0 + 0x74);
    if (ptr[3] != 0) {
        if (ptr[0] == attr) {
            ptr[3] = 0;
        } else {
            ptr[3] = attr;
        }
    } else {
        if (ptr[0] != attr) {
            func_800C2B88(a0);
        }
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5748);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B59CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5B48);

/**
 * @brief Register entity callback or call initialization routine.
 *
 * If the entity's word at offset 0x8C is zero, calls func_800B59CC.
 * Otherwise registers func_800B5B48 as a callback via func_800B2C58,
 * storing the entity pointer and a1 into the result structure.
 *
 * @param a0 Entity pointer.
 * @param a1 Value to store at result offset 0x10.
 */
void func_800B5C10(u8 *a0, s32 a1) {
    if (*(s32 *)(a0 + 0x8C) == 0) {
        func_800B59CC();
    } else {
        u8 *result = (u8 *)func_800B2C58((s32)func_800B5B48);
        *(s32 *)(result + 0xC) = (s32)a0;
        *(s32 *)(result + 0x10) = a1;
    }
}

/**
 * @brief Compute sprite attributes and apply to entity.
 *
 * Calls func_800B555C with a 16-bit truncated a1 and a2, then
 * combines the result with bits 24-25 from the entity's word at +8,
 * and passes it to func_800B5C10.
 *
 * @param a0 Entity pointer.
 * @param a1 Sprite parameter (truncated to 16-bit).
 * @param a2 Second sprite parameter.
 */
void func_800B5C70(s32 a0, s32 a1, s32 a2) {
    s32 result = func_800B555C((u16)a1, a2);
    s32 val = *(s32 *)(a0 + 8) & 0x01800000;
    func_800B5C10(a0, val | result);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5CB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5EC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5FD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6100);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6270);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6334);

/**
 * @brief Call func_800B2B68 if D_800EF72C pointer is non-null.
 *
 * Reads the pointer stored in D_800EF72C and calls func_800B2B68
 * with it if non-zero.
 */
void func_800B64E0(void) {
    s32 ptr = *(s32 *)D_800EF72C;
    if (ptr != 0) {
        func_800B2B68(ptr);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B650C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6584);

/**
 * @brief State machine for resource allocation and initialization.
 *
 * State 0: Allocates a resource via func_8013E000(2), stores result in
 * D_800EF72C, increments state, falls through.
 * State 1: Stores 0xFF at ptr[1], calls func_800B8F4C(0xF), returns 2.
 * Default: Returns 0.
 *
 * @param a0 State control structure (state at +0xD, pointer at +0x10).
 * @return 2 when initialization complete, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B66E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6764);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B67D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6858);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6954);

/**
 * @brief Call func_800B3650 with D_800F02C8 as the argument.
 */
void func_800B6A9C(void) {
    func_800B3650(D_800F02C8);
}

/**
 * @brief Process data through D_800F02C8 via stack buffer.
 *
 * @param a0 Input parameter for func_8003FC24.
 */
void func_800B6AC0(s32 a0) {
    u8 buf[0x20];
    func_8003FC24(D_800F02C8, a0, buf);
    func_800B3650(buf);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6AF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6C98);

/**
 * @brief Decode a compressed index value.
 *
 * If bit 12 of a0 is set, uses the lower 12 bits as an index into the
 * D_800F0290 halfword table. Otherwise sign-extends a0 to 16 bits.
 *
 * @param a0 Compressed value with bit 12 as format flag.
 * @return Decoded signed 16-bit value.
 */
s16 func_800B6D88(s32 a0) {
    if (a0 & 0x1000) {
        return *(s16 *)(D_800F0290 + (a0 & 0xFFF) * 2);
    }
    return (s16)a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6DBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B717C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B724C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B75C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B789C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B79B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7C48);

/**
 * @brief Set or clear bit 8 (0x100) of D_800EEC5C.
 *
 * If a0 is non-zero, sets bit 8. If a0 is zero, clears bit 8.
 *
 * @param a0 Non-zero to set, zero to clear.
 */
void func_800B7D20(s32 a0) {
    if (a0 != 0) {
        *(s32 *)D_800EEC5C |= 0x100;
    } else {
        *(s32 *)D_800EEC5C &= ~0x100;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7D58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7FD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B81B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8248);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8314);

/**
 * @brief Initialize sound system buffers and register playback handler.
 *
 * Calls func_800B86C0 to set up the primary audio buffer, func_800B2A00
 * to initialize D_800F05C8 with D_800F0578 entries, and func_800B2A84 to
 * register func_800B8314 as the playback callback.
 *
 * @return Pointer to D_800F05C8 buffer.
 */
u8 *func_800B84C8(void) {
    u8 *buf;
    func_800B86C0(D_800F02F8, D_800F0308, D_800F0408, 0x1E);
    buf = D_800F05C8;
    func_800B2A00(buf, D_800F0578, 0x14, 4);
    func_800B2A84(buf, (s32)func_800B8314);
    return buf;
}

/**
 * @brief Allocate from D_800F05C8 and clear byte at offset 0xD.
 *
 * @param a0 Allocation parameter.
 */
void func_800B853C(s32 a0) {
    u8 *result = func_800B2A84(D_800F05C8, a0);
    result[0xD] = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8564);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8644);

/**
 * @brief Initialize a linked list structure and clear data entries.
 *
 * Sets up the header fields (head pointer, zero field, data pointer,
 * count), calls func_800B8870 to initialize the list, then clears
 * byte at offset 1 for each entry (stride 0xC).
 *
 * @param a0 Pointer to the list header structure.
 * @param a1 Head pointer to store and pass to func_800B8870.
 * @param a2 Data array pointer.
 * @param a3 Number of entries.
 */
void func_800B86C0(u8 *a0, u8 *a1, u8 *a2, s32 a3) {
    *(s32 *)a0 = (s32)a1;
    *(s32 *)(a0 + 4) = 0;
    *(s32 *)(a0 + 8) = (s32)a2;
    *(u16 *)(a0 + 0xC) = a3;
    func_800B8870(a1, a3);
    if (a3 > 0) {
        s32 i = 0;
        u8 *ptr = a2;
        do {
            ptr[1] = 0;
            i++;
            ptr += 0xC;
        } while (i < a3);
    }
}
