#include "common.h"

extern u8 D_800F1A90[];
extern u8 D_800F1A94[];

typedef struct {
    s8 unk0;
    s8 unk1;
    u8 pad2[1];
    s8 unk3;
    s32 unk4;
    u8 pad8[4];
} D_800F1AB0_Type;

extern D_800F1AB0_Type D_800F1AB0[];
extern s32 D_800F1B70;
extern u8 D_800F1B74[];
extern u8 D_800F1B7C[];
extern s32 D_800F1B78;
extern s32 D_800F1B80;
extern u8 D_800F02F4[];
extern u8 D_800F0290[];
extern u8 D_800F1AA0[];
extern u8 D_800F02E8[];
extern u8 D_800EF724[];
void func_800C372C(void);
void func_800C3998(s16, s16);
void func_800C39F8(void);
void func_800C3C0C(void);
void func_800C3EFC(void);
void func_800C3FE8(void);
s32 func_800C2CBC(s32, void *, void *, void *);
s32 func_800C3418(s32);

/**
 * @brief Set entity rendering mode based on status flags.
 *
 * Loads the sub-entity from a0->0x74, masks a1 to clear bit 12.
 * If bit 6 of the halfword at sub-entity+0x2C is set, calls func_800C2AB0
 * to perform a full reset. Otherwise stores the masked a1 to sub-entity byte 3.
 *
 * @param a0 Entity pointer.
 * @param a1 Rendering mode value (bit 12 cleared before use).
 */
void func_800C2B88(s32 a0, s32 a1) {
    u8 *entity = *(u8 **)(a0 + 0x74);
    a1 &= ~0x1000;
    if (*(u16 *)(entity + 0x2C) & 0x40) {
        func_800C2AB0(a0);
    } else {
        entity[3] = a1;
    }
}

/**
 * @brief Check entity status flags and reset if certain bits set.
 *
 * Reads halfword at entity->0x74->0x2C, tests bits 0x140.
 * If any of those bits are set, calls func_800C2AB0 and returns 0.
 * Otherwise returns -1.
 *
 * @param a0 Entity pointer.
 * @return 0 if flags triggered reset, -1 otherwise.
 */
s32 func_800C2BD0(s32 a0) {
    u16 flags = *(u16 *)(*(s32 *)(a0 + 0x74) + 0x2C);
    if ((flags & 0x140) != 0) {
        func_800C2AB0(a0);
        return 0;
    }
    return -1;
}

/**
 * @brief Read a signed byte from the given address.
 *
 * @param a0 Pointer to a byte.
 * @return The value sign-extended to s32.
 */
s32 func_800C2C10(u8 *a0) {
    return *(s8 *)a0;
}

/**
 * @brief Read an unsigned byte from the given address.
 *
 * @param a0 Pointer to a byte.
 * @return The value zero-extended to s32.
 */
s32 func_800C2C1C(u8 *a0) {
    return *a0;
}

/**
 * @brief Read a little-endian 16-bit signed value from two bytes.
 *
 * Combines ptr[0] (low byte) and ptr[1] (high byte) into a signed 16-bit value.
 *
 * @param ptr Pointer to two bytes in little-endian order.
 * @return The sign-extended 16-bit value.
 */
s32 func_800C2C28(u8 *ptr) {
    return (s16)((ptr[1] << 8) | ptr[0]);
}

/**
 * @brief Advance script pointer by 1-byte signed offset or fixed 2.
 *
 * If a0 is zero, advances a1 by 2 bytes. Otherwise reads a signed
 * byte at a1+1 and advances a1 by that amount.
 *
 * @param a0 Mode flag (0 = fixed advance, nonzero = variable).
 * @param a1 Current script pointer.
 * @return Updated script pointer.
 */
s32 func_800C2C44(s32 a0, u8 *a1) {
    if (a0 != 0) {
        return (s32)a1 + func_800C2C10(a1 + 1);
    }
    return (s32)a1 + 2;
}

/**
 * @brief Advance script pointer by 2-byte signed offset or fixed 3.
 *
 * If a0 is zero, advances a1 by 3 bytes. Otherwise reads a signed
 * 16-bit value at a1+1 and advances a1 by that amount.
 *
 * @param a0 Mode flag (0 = fixed advance, nonzero = variable).
 * @param a1 Current script pointer.
 * @return Updated script pointer.
 */
s32 func_800C2C80(s32 a0, u8 *a1) {
    if (a0 != 0) {
        return (s32)a1 + func_800C2C28(a1 + 1);
    }
    return (s32)a1 + 3;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C2CBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3014);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C33A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3418);

/**
 * @brief Allocate a handler via func_800B2C58 and initialize its fields.
 *
 * Allocates a handler with func_800C3418 as callback, then stores
 * a0 at offset 0xC, a1 as byte at 0x10, zero at 0x12, and 0xFF at 0x1C.
 *
 * @param a0 Value stored at handler offset 0xC.
 * @param a1 Byte stored at handler offset 0x10.
 */
void func_800C3518(s32 a0, s32 a1) {
    u8 *result = func_800B2C58(func_800C3418);
    *(s32 *)(result + 0xC) = a0;
    result[0x10] = a1;
    result[0x12] = 0;
    *(s32 *)(result + 0x1C) = 0xFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3568);

/**
 * @brief Increment progress counter and compute pitch ratio.
 *
 * Increments the byte at a0[0xC], divides (a0[0xC] << 10) by
 * a0[0xD] to get a ratio, passes it through func_8003ED64,
 * and stores the result in D_800F02E8. Returns 2 if the counter
 * has reached or exceeded the limit, 0 otherwise.
 *
 * @param a0 Pointer to state struct with counter at 0xC and limit at 0xD.
 * @return 2 if a0[0xC] >= a0[0xD], else 0.
 */
s32 func_800C3694(u8 *a0) {
    s32 result;
    a0[0xC]++;
    result = func_8003ED64((a0[0xC] << 10) / a0[0xD]);
    *(u16 *)D_800F02E8 = result;
    return (a0[0xC] >= a0[0xD]) << 1;
}

/**
 * @brief Reset sound state: clear volume, set default pitch, and disable flag.
 *
 * Clears D_800F02F4, sets D_800F02E8 to 0x1000, and clears bit 15 of D_800EF724.
 */
void func_800C3704(void) {
    *(u16 *)D_800F02F4 = 0;
    *(u16 *)D_800F02E8 = 0x1000;
    *(u16 *)D_800EF724 &= 0x7FFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C372C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3898);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C38BC);

/**
 * @brief Store a value to one of several sound parameter tables.
 *
 * If a0 < 8, stores a1 to D_800F0290[a0] (low channel table).
 * If a0 >= 120, stores a1 to D_800F1AA0[127-a0] (high channel table).
 * If a0 == 16, stores a1 to D_800F02F4.
 * Otherwise does nothing.
 *
 * @param a0 Channel/register index (sign-extended to s16).
 * @param a1 Value to store.
 */
void func_800C3998(s16 a0, s16 a1) {
    if (a0 >= 0x78) {
        *(s16 *)((0x7F - a0) * 2 + (s32)D_800F1AA0) = a1;
    } else if (a0 < 8) {
        *(s16 *)(a0 * 2 + (s32)D_800F0290) = a1;
    } else if (a0 == 0x10) {
        *(s16 *)D_800F02F4 = a1;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C39F8);

/**
 * @brief Process D_800F1A90 if non-null, updating it with callback results.
 *
 * If the pointer stored in D_800F1A90 is non-null, calls func_800C2CBC
 * with it and three callback function pointers, storing the result back.
 */
void func_800C3B6C(void) {
    s32 ptr = *(s32 *)D_800F1A90;
    if (ptr != 0) {
        *(s32 *)D_800F1A90 = func_800C2CBC(ptr, func_800C39F8, func_800C372C, func_800C3998);
    }
}

/**
 * @brief Compute two pointers from halfword offsets and store to globals.
 *
 * Reads halfwords at a0+2 and a0+4, adds each to a0, and stores
 * the results to D_800F1A90 and D_800F1A94 respectively.
 *
 * @param a0 Base pointer.
 */
void func_800C3BBC(s32 a0) {
    *(s32 *)D_800F1A90 = a0 + *(u16 *)(a0 + 2);
    *(s32 *)D_800F1A94 = a0 + *(u16 *)(a0 + 4);
}

/**
 * @brief Wrapper for func_800C3BBC.
 *
 * Passes through a0 to func_800C3BBC.
 *
 * @param a0 Base pointer passed through.
 */
void func_800C3BE0(s32 a0) {
    func_800C3BBC(a0);
}

/**
 * @brief Clear D_800F1A90 to zero.
 */
void func_800C3C00(void) {
    *(s32 *)D_800F1A90 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3C0C);

/**
 * @brief Allocate handler for func_800C3C0C and store five parameters.
 *
 * Registers func_800C3C0C as callback via func_800B2C58. If allocation
 * succeeds, stores a0-a4 to the handler struct and sets bit 5 in the
 * halfword at a1+2.
 *
 * @param a0 Word stored at handler offset 0xC.
 * @param a1 Word stored at handler offset 0x10; bit 5 set in halfword at a1+2.
 * @param a2 Byte stored at handler offset 0x14.
 * @param a3 Halfword stored at handler offset 0x16.
 * @param a4 Word stored at handler offset 0x18.
 */
void func_800C3DD4(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800C3C0C);
    if (result != 0) {
        *(s32 *)(result + 0xC) = a0;
        *(s32 *)(result + 0x10) = a1;
        *(u8 *)(result + 0x14) = a2;
        *(u8 *)(result + 0x15) = 0xFF;
        *(u16 *)(result + 0x16) = a3;
        *(s32 *)(result + 0x18) = a4;
        *(u16 *)(a1 + 2) |= 0x20;
    }
}

/**
 * @brief Allocate handler for func_800C3C0C and store six parameters.
 *
 * Similar to func_800C3DD4 but stores a3 at +0x15 instead of 0xFF,
 * and has a 6th argument stored at +0x18 via a4, with a 5th halfword via a4.
 *
 * @param a0 Word stored at handler offset 0xC.
 * @param a1 Word stored at handler offset 0x10; bit 5 set in halfword at a1+2.
 * @param a2 Byte stored at handler offset 0x14.
 * @param a3 Byte stored at handler offset 0x15.
 * @param a4 Halfword stored at handler offset 0x16.
 * @param a5 Word stored at handler offset 0x18.
 */
void func_800C3E64(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800C3C0C);
    if (result != 0) {
        *(s32 *)(result + 0xC) = a0;
        *(s32 *)(result + 0x10) = a1;
        *(u8 *)(result + 0x14) = a2;
        *(u8 *)(result + 0x15) = a3;
        *(u16 *)(result + 0x16) = a4;
        *(s32 *)(result + 0x18) = a5;
        *(u16 *)(a1 + 2) |= 0x20;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3EFC);

/**
 * @brief Allocate a handler for func_800C3EFC and store three halfword params.
 *
 * Calls func_800B2C58 with func_800C3EFC as callback. If allocation
 * succeeds, stores a0/a1/a2 as halfwords at offsets 0xC/0xE/0x10
 * and zeros offset 0x12.
 *
 * @param a0 First halfword value.
 * @param a1 Second halfword value.
 * @param a2 Third halfword value.
 */
void func_800C3F88(s32 a0, s32 a1, s32 a2) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800C3EFC);
    if (result != 0) {
        *(u16 *)(result + 0xC) = a0;
        *(u16 *)(result + 0xE) = a1;
        *(u16 *)(result + 0x10) = a2;
        *(u16 *)(result + 0x12) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3FE8);

/**
 * @brief Allocate a handler for func_800C3FE8 and store two words + halfword.
 *
 * Calls func_800B2C58 with func_800C3FE8 as callback. If allocation
 * succeeds, stores a0/a1 as words at offsets 0xC/0x10, a2 as halfword
 * at 0x14, and zeros offset 0x16.
 *
 * @param a0 First word value.
 * @param a1 Second word value.
 * @param a2 Halfword value.
 */
void func_800C40B4(s32 a0, s32 a1, s32 a2) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800C3FE8);
    if (result != 0) {
        *(s32 *)(result + 0xC) = a0;
        *(s32 *)(result + 0x10) = a1;
        *(u16 *)(result + 0x14) = a2;
        *(u16 *)(result + 0x16) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4114);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4228);

/**
 * @brief Initialize three battle state globals.
 *
 * Sets D_800F1B70 to 0, D_800F1B74 to 0x10000, and D_800F1B80 to 0.
 */
void func_800C42DC(void) {
    D_800F1B70 = 0;
    *(s32 *)D_800F1B74 = 0x10000;
    D_800F1B80 = 0;
}

/**
 * @brief Return the word value at D_800F1B80.
 *
 * @return Current value of D_800F1B80.
 */
s32 func_800C42FC(void) {
    return D_800F1B80;
}

/**
 * @brief Check callback completion and set done flag.
 *
 * Calls sndGetEngineState with the entity. If it returns non-zero (still busy),
 * returns 0. Otherwise, sets the byte at entity->0x14 to 1 and returns 2.
 *
 * @param a0 Entity pointer.
 * @return 0 if busy, 2 if done.
 */
s32 func_800C430C(s32 a0) {
    if (sndGetEngineState(a0) != 0) {
        return 0;
    }
    *(u8 *)(*(s32 *)(a0 + 0x14)) = 1;
    return 2;
}

/**
 * @brief Initialize sound request and register completion callback.
 *
 * Clears the byte at a1, calls sndSetPlaybackAddr to start sound with a1=0,
 * then allocates a handler via func_800B2C58 with func_800C430C as callback,
 * storing a1 pointer at handler offset 0x14.
 *
 * @param a0 Sound request parameter.
 * @param a1 Pointer to completion flag byte.
 */
void func_800C434C(s32 a0, u8 *a1) {
    s32 result;
    *a1 = 0;
    sndSetPlaybackAddr(a0, 0);
    result = func_800B2C58(func_800C430C);
    *(s32 *)(result + 0x14) = (s32)a1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C438C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C443C);

/**
 * @brief Enqueue a type-1 command to the command table.
 *
 * If the command count (D_800F1B70) is under 15, increments it and
 * writes a 12-byte entry at D_800F1AB0 + count * 12 with type=1,
 * the three byte parameters, and the word parameter.
 *
 * @param a0 Word parameter stored at entry offset 8.
 * @param a1 Byte parameter stored at entry offset 1.
 * @param a2 Byte parameter stored at entry offset 2.
 * @param a3 Byte parameter stored at entry offset 3.
 */
void func_800C44E4(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 count = D_800F1B70;
    if (count < 15) {
        u8 *entry;
        D_800F1B70 = count + 1;
        entry = (u8 *)&D_800F1AB0[count];
        entry[0] = 1;
        entry[1] = a1;
        entry[2] = a2;
        entry[3] = a3;
        *(s32 *)(entry + 8) = a0;
    }
}

/**
 * @brief Enqueue a type-2 command to the command table.
 *
 * If the command count (D_800F1B70) is under 15, increments it and
 * writes a 12-byte entry at D_800F1AB0 + count * 12 with type=2
 * and the three byte parameters.
 *
 * @param a0 Byte parameter stored at entry offset 1.
 * @param a1 Byte parameter stored at entry offset 2.
 * @param a2 Byte parameter stored at entry offset 3.
 */
void func_800C4538(s32 a0, s32 a1, s32 a2) {
    s32 count = D_800F1B70;
    if (count < 15) {
        u8 *entry;
        D_800F1B70 = count + 1;
        entry = (u8 *)&D_800F1AB0[count];
        entry[0] = 2;
        entry[1] = a0;
        entry[2] = a1;
        entry[3] = a2;
    }
}

/**
 * @brief Enqueue a type-3 command to the command table and update bitmasks.
 *
 * If the command count (D_800F1B70) is under 15, increments it and
 * writes a 12-byte entry with type=3. Also ORs the arg1 bitmask into
 * both D_800F1B80 and D_800F1B78.
 *
 * @param arg0 Byte parameter stored at entry offset 3.
 * @param arg1 Bitmask stored at entry offset 4 and OR'd into globals.
 * @param arg2 Byte parameter stored at entry offset 1.
 */
void func_800C4588(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_a1;
    D_800F1AB0_Type *temp_v1;

    temp_a1 = D_800F1B70;
    if (temp_a1 < 0xF) {
        D_800F1B70 = temp_a1 + 1;
        temp_v1 = &D_800F1AB0[temp_a1];
        temp_v1->unk3 = arg0;
        temp_v1->unk0 = 3;
        temp_v1->unk1 = arg2;
        temp_v1->unk4 = arg1;
        D_800F1B80 |= arg1;
        D_800F1B78 |= arg1;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C45FC);

/**
 * @brief Wrapper for func_800C443C.
 */
void func_800C4764(void) {
    func_800C443C();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4784);


/**
 * @brief Wrapper for func_800C44E4.
 */
void func_800C47CC(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_800C44E4(a0, a1, a2, a3);
}

/**
 * @brief Process animation: call func_800C438C then func_800C44E4.
 *
 * @param a0 Entity pointer (passed as 1st arg to func_800C44E4).
 * @param a1 Animation ID (passed to func_800C438C).
 * @param a2 Parameter (passed as 3rd arg to func_800C44E4).
 * @param a3 Parameter (passed as 4th arg to func_800C44E4).
 */
void func_800C47EC(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = func_800C438C(a1);
    func_800C44E4(a0, result, a2, a3);
}

/**
 * @brief Look up a table entry and compute a pointer offset.
 *
 * Loads a table pointer from a0->0x84->0x28, reads a halfword at
 * table[a1*2 + 2], and returns table + offset.
 *
 * @param a0 Pointer to structure with nested table.
 * @param a1 Table index.
 * @return Computed pointer as integer.
 */
s32 func_800C4844(u8 *a0, s32 a1) {
    u8 *base = *(u8 **)(a0 + 0x84);
    u8 *table = *(u8 **)(base + 0x28);
    u16 offset = *(u16 *)(table + a1 * 2 + 2);
    return (s32)table + offset;
}

/**
 * @brief Look up table entry and process it.
 *
 * Calls func_800C4844 to get a table entry pointer from a0/a1,
 * then passes it to func_800C443C along with a2 and a3.
 *
 * @param a0 Entity pointer with nested table.
 * @param a1 Table index.
 * @param a2 Processing parameter.
 * @param a3 Processing parameter.
 */
void func_800C4864(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 entry = func_800C4844(a0, a1);
    func_800C443C(entry, a2, a3);
}

/**
 * @brief Process animation: call func_800C438C then func_800C4864.
 *
 * @param a0 Entity pointer (passed as 1st arg to func_800C4864).
 * @param a1 Table index (passed as 2nd arg to func_800C4864).
 * @param a2 Parameter (passed as 3rd arg to func_800C4864).
 * @param a3 Animation ID (passed to func_800C438C, result becomes 4th arg).
 */
void func_800C48A8(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = func_800C438C(a3);
    func_800C4864(a0, a1, a2, result);
}

/**
 * @brief Wrapper for func_800C4588.
 */
void func_800C4900(s32 a0, s32 a1, s32 a2) {
    func_800C4588(a0, a1, a2);
}

/**
 * @brief Process animation: call func_800C438C then func_800C4588.
 *
 * @param a0 Entity pointer (passed as 1st arg to func_800C4588).
 * @param a1 Parameter (passed as 2nd arg to func_800C4588).
 * @param a2 Animation ID (passed to func_800C438C, result becomes 3rd arg).
 */
void func_800C4920(s32 a0, s32 a1, s32 a2) {
    s32 result = func_800C438C(a2);
    func_800C4588(a0, a1, result);
}

/**
 * @brief Check callback completion and set done flag (duplicate).
 *
 * Same logic as func_800C430C: calls sndGetEngineState, returns 0 if busy,
 * otherwise sets the byte at entity->0x14 to 1 and returns 2.
 *
 * @param a0 Entity pointer.
 * @return 0 if busy, 2 if done.
 */
s32 func_800C4968(s32 a0) {
    if (sndGetEngineState(a0) != 0) {
        return 0;
    }
    *(u8 *)(*(s32 *)(a0 + 0x14)) = 1;
    return 2;
}

/**
 * @brief Start sound effect and register completion callback.
 *
 * Calls sndCmdE2 to set up the request, func_80014400 to start
 * the sound with D_800F1B7C as volume, then allocates a handler via
 * func_800B2C58 with func_800C4968 as callback, storing the completion
 * flag pointer.
 *
 * @param a0 Sound request parameter.
 * @param a1 Pointer to completion flag byte.
 */
void func_800C49A8(s32 a0, u8 *a1) {
    s32 result;
    sndCmdE2(a0);
    func_80014400(a0, *(u8 *)D_800F1B7C);
    result = func_800B2C58(func_800C4968);
    *(s32 *)(result + 0x14) = (s32)a1;
    *a1 = 0;
}

/**
 * @brief Wrapper for func_800C4538.
 */
void func_800C4A00(s32 a0, s32 a1, s32 a2) {
    func_800C4538(a0, a1, a2);
}

/**
 * @brief Process animation: call func_800C438C then func_800C4538.
 *
 * @param a0 Animation ID (passed to func_800C438C, result becomes 1st arg).
 * @param a1 Entity pointer (passed as 2nd arg to func_800C4538).
 * @param a2 Parameter (passed as 3rd arg to func_800C4538).
 */
void func_800C4A20(s32 a0, s32 a1, s32 a2) {
    s32 result = func_800C438C(a0);
    func_800C4538(result, a1, a2);
}
