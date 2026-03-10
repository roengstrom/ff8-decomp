#include "common.h"

extern u8 D_800F1A90[];
extern u8 D_800F1A94[];
extern u8 D_800F1B70[];
extern u8 D_800F1B74[];
extern u8 D_800F1B7C[];
extern u8 D_800F1B80[];
extern u8 D_800F02F4[];
extern u8 D_800F02E8[];
extern u8 D_800EF724[];
void func_800C372C(void);
void func_800C3998(void);
void func_800C39F8(void);
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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3694);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3998);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3DD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3E64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3EFC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3F88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3FE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C40B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4114);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4228);

/**
 * @brief Initialize three battle state globals.
 *
 * Sets D_800F1B70 to 0, D_800F1B74 to 0x10000, and D_800F1B80 to 0.
 */
void func_800C42DC(void) {
    *(s32 *)D_800F1B70 = 0;
    *(s32 *)D_800F1B74 = 0x10000;
    *(s32 *)D_800F1B80 = 0;
}

/**
 * @brief Return the word value at D_800F1B80.
 *
 * @return Current value of D_800F1B80.
 */
s32 func_800C42FC(void) {
    return *(s32 *)D_800F1B80;
}

/**
 * @brief Check callback completion and set done flag.
 *
 * Calls func_80013CA4 with the entity. If it returns non-zero (still busy),
 * returns 0. Otherwise, sets the byte at entity->0x14 to 1 and returns 2.
 *
 * @param a0 Entity pointer.
 * @return 0 if busy, 2 if done.
 */
s32 func_800C430C(s32 a0) {
    if (func_80013CA4(a0) != 0) {
        return 0;
    }
    *(u8 *)(*(s32 *)(a0 + 0x14)) = 1;
    return 2;
}

/**
 * @brief Initialize sound request and register completion callback.
 *
 * Clears the byte at a1, calls func_80014034 to start sound with a1=0,
 * then allocates a handler via func_800B2C58 with func_800C430C as callback,
 * storing a1 pointer at handler offset 0x14.
 *
 * @param a0 Sound request parameter.
 * @param a1 Pointer to completion flag byte.
 */
void func_800C434C(s32 a0, u8 *a1) {
    s32 result;
    *a1 = 0;
    func_80014034(a0, 0);
    result = func_800B2C58(func_800C430C);
    *(s32 *)(result + 0x14) = (s32)a1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C438C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C443C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C44E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4538);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4588);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C45FC);

/**
 * @brief Wrapper for func_800C443C.
 */
void func_800C4764(void) {
    func_800C443C();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4784);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C47BC);

/**
 * @brief Wrapper for func_800C44E4.
 */
void func_800C47CC(void) {
    func_800C44E4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C47EC);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C48A8);

/**
 * @brief Wrapper for func_800C4588.
 */
void func_800C4900(void) {
    func_800C4588();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4920);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4954);

/**
 * @brief Check callback completion and set done flag (duplicate).
 *
 * Same logic as func_800C430C: calls func_80013CA4, returns 0 if busy,
 * otherwise sets the byte at entity->0x14 to 1 and returns 2.
 *
 * @param a0 Entity pointer.
 * @return 0 if busy, 2 if done.
 */
s32 func_800C4968(s32 a0) {
    if (func_80013CA4(a0) != 0) {
        return 0;
    }
    *(u8 *)(*(s32 *)(a0 + 0x14)) = 1;
    return 2;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C49A8);

/**
 * @brief Wrapper for func_800C4538.
 */
void func_800C4A00(void) {
    func_800C4538();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4A20);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4A58);
