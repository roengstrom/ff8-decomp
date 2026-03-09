#include "common.h"

extern u8 D_800ED148[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8B7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8CA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8D7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8E90);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8EFC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8F98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8FDC);

/**
 * @brief Initialize three type bytes in an entity structure.
 *
 * @param a0 Pointer to entity data.
 * @return Always 3.
 */
s32 func_800A9064(u8 *a0) {
    a0[0x32] = 0x41;
    a0[0x37] = 0x43;
    a0[0x3C] = 0x42;
    return 3;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9084);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9240);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9284);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9370);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9490);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A94E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9568);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A95A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A960C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A972C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9784);

/**
 * @brief Compute func_8009B15C(a0) modulo a0.
 *
 * @param a0 Divisor and argument to func_8009B15C.
 * @return Remainder of func_8009B15C(a0) / a0.
 */
s32 func_800A97A4(s32 a0) {
    return func_8009B15C(a0) % a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A97D4);

/**
 * @brief Compute a single-bit mask at the given position.
 *
 * @param bitPos Bit position (0-15).
 * @return 16-bit mask with bit at bitPos set.
 */
u16 func_800A97FC(s32 bitPos) {
    return (1 << bitPos) & 0xFFFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A980C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9888);

/**
 * @brief Compute a bitmask from an entity's byte at offset 0x98.
 *
 * @param a0 Entity index (stride 0xD0).
 * @return 16-bit mask with one bit set at the position given by entity[0x98].
 */
u16 func_800A9904(s32 a0) {
    u8 *base = D_800ED148;
    u8 *entity;
    asm("");
    entity = base + a0 * 0xD0;
    return (1 << entity[0x98]) & 0xFFFF;
}

extern u8 D_800EEBE0[];

/**
 * @brief Toggle the least significant bit of 7 bytes in D_800EEBE0.
 */
void func_800A9938(void) {
    s32 i = 0;
    u8 *base = D_800EEBE0;
    do {
        u8 *p = (u8 *)(i + (s32)base);
        *p = (~*p) & 1;
        i++;
    } while (i < 7);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9970);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A99E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9A6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9AC0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9E08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9F98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9FDC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA034);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA368);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA44C);

/** @brief Return constant 0x8007. */
s32 func_800AA4E0(void) {
    return 0x8007;
}

/** @brief Return constant 0x80F8. */
s32 func_800AA4E8(void) {
    return 0x80F8;
}

/** @brief Return constant 0x80FF. */
s32 func_800AA4F0(void) {
    return 0x80FF;
}

/**
 * @brief Search entity table for a matching byte at offset 0xCB.
 *
 * @param a0 Value to search for.
 * @return Entity index (0-6) if found, or 0xFF if not found.
 */
s32 func_800AA4F8(s32 a0) {
    s32 i = 0;
    u8 *entity = D_800ED148;
    do {
        if (entity[0xCB] == a0) {
            return i;
        }
        i++;
        entity += 0xD0;
    } while (i < 7);
    return 0xFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA530);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA57C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA68C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA71C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA768);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA7B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA840);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA88C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA930);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA980);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA9C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAA10);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAA50);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAA9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAB50);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AABEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AACD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAD5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAE10);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAE98);

/**
 * @brief Allocate structure via func_800AA57C and copy field +0x1C to +0x18.
 *
 * @param a0 Second parameter to func_800AA57C (first is 0xC8).
 */
void func_800AAF48(s32 a0) {
    u8 *result = func_800AA57C(0xC8, a0);
    *(s32 *)(result + 0x18) = *(s32 *)(result + 0x1C);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAF70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AAFB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB008);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB02C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB054);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB07C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB0C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB11C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB1A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB208);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB24C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB28C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB300);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB36C);

/**
 * @brief Set battle flag 0x5C2 and clear status bytes 0x12F9 and 0x12FD.
 */
void func_800AB3C4(void) {
    u8 *base = D_800ED148;
    base[0x5C2] = 1;
    base[0x12F9] = 0;
    base[0x12FD] = 0;
}

/**
 * @brief Clear battle flag 0x5C2 and set status bytes 0x12F9 and 0x12FD.
 */
void func_800AB3E0(void) {
    u8 *base = D_800ED148;
    base[0x5C2] = 0;
    base[0x12F9] = 1;
    base[0x12FD] = 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB3FC);

/**
 * @brief Call func_800AB3FC with a fixed duration of 0x1E.
 *
 * @param a0 First parameter passed through.
 * @param a1 Second parameter passed through.
 */
void func_800AB488(s32 a0, s32 a1) {
    func_800AB3FC(a0, a1, 0x1E);
}
