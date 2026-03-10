#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800E3CEC[];
extern u8 D_800EE9E8[];
extern u8 D_800EE28C[];
void func_800AB054(void);
void func_800AB1A8(void);
s32 func_8009B3D0(void *);
s32 func_800B0398(s32);
extern u8 D_80078E00[];
s32 func_800B0F9C(s32);
s32 func_800B0F7C(s32);

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

/**
 * @brief Set up entity action with sound and visual effects.
 *
 * Calls func_800A97FC to get a value, then triggers entity action
 * via func_800B0754, sets animation via func_800AF4BC, and
 * conditionally calls func_800AE3D4 based on func_800AE390 result.
 *
 * @param a0 Entity index passed to func_800A97FC and func_800B0754.
 * @param a1 Mode parameter for func_800B0754 and subsequent calls.
 */
void func_800A95A0(s32 a0, s32 a1) {
    s32 val = func_800A97FC(a0);
    func_800B0754(a0, 4, a1, (u16)val);
    func_800AF4BC(a1, 1);
    if (func_800AE390(a1) == 0) {
        func_800AE3D4(a1);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A960C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A972C);

/**
 * @brief Resolve a 16-bit offset to an address.
 *
 * If the offset is 0xFFFF (sentinel), returns D_800E3CEC.
 * Otherwise returns offset + base.
 *
 * @param offset 16-bit offset value (0xFFFF = invalid).
 * @param base Base address to add offset to.
 * @return Resolved address.
 */
s32 func_800A9784(u16 offset, s32 base) {
    if ((offset & 0xFFFF) == 0xFFFF) {
        return (s32)D_800E3CEC;
    }
    return offset + base;
}

/**
 * @brief Compute func_8009B15C(a0) modulo a0.
 *
 * @param a0 Divisor and argument to func_8009B15C.
 * @return Remainder of func_8009B15C(a0) / a0.
 */
s32 func_800A97A4(s32 a0) {
    return func_8009B15C(a0) % a0;
}

/**
 * @brief Clear 8 entity word fields in the battle entity table.
 *
 * Zeros 8 consecutive words at offset 0x12B4 down to 0x1298
 * relative to D_800ED148.
 */
void func_800A97D4(void) {
    s32 i = 7;
    s32 base = (s32)D_800ED148;
    base += 0x1C;
top:
    *(s32 *)(base + 0x1298) = 0;
    i--;
    base -= 4;
    if (i >= 0) goto top;
}

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

/**
 * @brief Conditionally search entity table and check for 0xFF result.
 *
 * If a0 is 0, returns whether func_800AA4F8(a1) is not 0xFF.
 * If a0 is 3, returns whether func_800AA4F8(a1) is 0xFF.
 *
 * @param a0 Mode selector (0 = found check, 3 = not-found check).
 * @param a1 Search value passed to func_800AA4F8.
 * @return Boolean result based on mode.
 */
s32 func_800AA71C(s32 a0, s32 a1) {
    if (a0 == 0) {
        return func_800AA4F8(a1) != 0xFF;
    }
    if (a0 == 3) {
        return func_800AA4F8(a1) == 0xFF;
    }
}

/**
 * @brief Conditionally search via func_800AA530 and check for 0xFF result.
 *
 * If a0 is 0, returns whether func_800AA530(a1) is not 0xFF.
 * If a0 is 3, returns whether func_800AA530(a1) is 0xFF.
 *
 * @param a0 Mode selector (0 = found check, 3 = not-found check).
 * @param a1 Search value passed to func_800AA530.
 * @return Boolean result based on mode.
 */
s32 func_800AA768(s32 a0, s32 a1) {
    if (a0 == 0) {
        return func_800AA530(a1) != 0xFF;
    }
    if (a0 == 3) {
        return func_800AA530(a1) == 0xFF;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA7B4);

/**
 * @brief Conditionally search via func_800AA7B4 and check for 0xFF result.
 *
 * If a0 is 0, returns whether func_800AA7B4(a1) is not 0xFF.
 * If a0 is 3, returns whether func_800AA7B4(a1) is 0xFF.
 *
 * @param a0 Mode selector (0 = found check, 3 = not-found check).
 * @param a1 Search value passed to func_800AA7B4.
 * @return Boolean result based on mode.
 */
s32 func_800AA840(s32 a0, s32 a1) {
    if (a0 == 0) {
        return func_800AA7B4(a1) != 0xFF;
    }
    if (a0 == 3) {
        return func_800AA7B4(a1) == 0xFF;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA88C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AA930);

/**
 * @brief Conditionally call func_800AA930 and optionally invert the result.
 *
 * If a0 is 0, calls func_800AA930 with a1 and returns its result.
 * If a0 is 3, calls func_800AA930 with a1 and returns the inverted
 * lowest bit of the result.
 *
 * @param a0 Mode selector (0 = normal, 3 = inverted).
 * @param a1 Parameter passed to func_800AA930.
 * @return Result of func_800AA930, possibly with bit 0 inverted.
 */
s32 func_800AA980(s32 a0, s32 a1) {
    if (a0 == 0) {
        return func_800AA930(a1);
    }
    if (a0 == 3) {
        s32 result = func_800AA930(a1);
        return (~result) & 1;
    }
}

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

/**
 * @brief Allocate entry and add signed offset to its field at 0x18.
 *
 * Calls func_800AA57C with 0xC8 and a0, then sign-extends a1 to 16 bits
 * and adds it to the word at offset 0x18 of the returned entry.
 *
 * @param a0 Second parameter to func_800AA57C.
 * @param a1 Signed 16-bit offset to add.
 */
void func_800AAF70(s32 a0, s32 a1) {
    u8 *result = func_800AA57C(0xC8, a0);
    *(s32 *)(result + 0x18) += (s16)a1;
}

/**
 * @brief Look up entity entry by index, call animation init, set active flag.
 *
 * Computes D_800EE28C + a0 * 16 to get the entry, calls func_8009AF3C
 * with entry[4], duration 0x1E, mode 3, size 0x80, and zero flag.
 * Then sets entry[0xF] to 1.
 *
 * @param a0 Entity index (stride 16).
 */
void func_800AAFB8(s32 a0) {
    u8 *entry = (u8 *)((s32)D_800EE28C + a0 * 16);
    func_8009AF3C(*(s32 *)(entry + 4), 0x1E, 3, 0x80, 0);
    entry[0xF] = 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB008);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB02C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB054);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB07C);

/**
 * @brief Allocate entry via func_8009B3D0 and store battle data.
 *
 * Allocates an entry using func_800AB054 as callback, computes the
 * entry address from D_800EE28C + index * 16, stores the result of
 * func_800B0398(a0) at offset 4, and a1 as halfword at offset 8.
 *
 * @param a0 Parameter passed to func_800B0398.
 * @param a1 Halfword value stored at entry offset 8.
 */
void func_800AB0C0(s32 a0, s32 a1) {
    s32 idx = func_8009B3D0(func_800AB054);
    u8 *entry = (u8 *)((s32)D_800EE28C + idx * 16);
    *(s32 *)(entry + 4) = func_800B0398(a0);
    *(u16 *)(entry + 8) = a1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB11C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB1A8);

/**
 * @brief Allocate entry via func_8009B3D0 and store value at offset 8.
 *
 * Calls func_8009B3D0 with func_800AB1A8+4 as data pointer, shifts result
 * left by 4 (multiply by 16), adds D_800EE28C as base, then stores a0
 * as halfword at offset 8 of the computed entry.
 *
 * @param a0 Value to store as halfword.
 */
void func_800AB208(s32 a0) {
    s32 idx = func_8009B3D0(func_800AB1A8 + 4);
    u8 *entry = (u8 *)((s32)D_800EE28C + idx * 16);
    *(s16 *)(entry + 8) = a0;
}

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
