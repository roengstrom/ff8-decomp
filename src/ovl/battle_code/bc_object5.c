#include "common.h"
#include "battle.h"

extern u8 D_800ED148[];
extern u8 D_800E3CEC[];
extern u8 D_800EE9E8[];
extern u8 D_800EE28C[];
void func_800AB054(void);
void func_800AB1AC(void);
s32 func_8009B3D0(void *);
s32 func_800B0398(s32);
extern u8 D_80078E00[];
extern u8 D_80077378[];
extern u8 D_8007809A[];
s32 func_800B0F9C(s32);
s32 func_800B0F7C(s32);
s32 func_800AE730(void);
void func_800AE6C0(void);
void func_800A59AC(s32, s32, s32);
s32 func_800AE788(void);
s32 func_800A97A4(s32);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8B7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8CA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8D7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8E90);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A8EFC);

/**
 * @brief Initialize entity ability fields from the ability table.
 *
 * Writes a1 as the ability ID at offset 0x32 of the entity entry
 * (computed as a0 + a2*5). Looks up ability data in D_80078E00
 * (offset 0x4A60, stride 8) and copies two bytes to offsets 0x34-0x35.
 * Clears offset 0x36 and sets offset 0x33 to 1.
 *
 * @param a0 Base entity pointer (as integer).
 * @param a1 Ability ID.
 * @param a2 Slot index (multiplied by 5 for stride).
 */
void func_800A8F98(s32 a0, s32 a1, s32 a2) {
    s32 entry = a0 + a2 * 5;
    s32 tbl = (s32)D_80078E00;
    s32 lookup = a1 * 8;
    s32 b;
    *(u8 *)(entry + 0x32) = a1;
    *(u8 *)(entry + 0x34) = *(u8 *)(tbl + lookup + 0x4A60);
    b = *(u8 *)(tbl + lookup + 0x4A61);
    *(u8 *)(entry + 0x36) = 0;
    *(u8 *)(entry + 0x33) = 1;
    *(u8 *)(entry + 0x35) = b;
}

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

/**
 * @brief Search D_80077378+0xB44 table for entry matching a given byte.
 *
 * Iterates up to 198 entries (stride 2) in D_80077378 at offset 0xB44.
 * If byte[0] matches a0, returns byte[1]. Returns 0 if not found.
 *
 * @param a0 Value to search for.
 * @return Unsigned byte at offset 1 of matching entry, or 0 if not found.
 *
 * @note Non-matching: CC1PSX inverts bne to beq and fills delay slot,
 * producing 15 instructions instead of 17. Branch direction inversion
 * in search loop.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9240);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9284);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9370);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9490);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A94E0);

/**
 * @brief Search D_800EE9E8 table for a matching byte value.
 *
 * Iterates up to 32 entries (stride 5) in D_800EE9E8, comparing
 * the first byte of each entry to a0.
 *
 * @param a0 Value to search for.
 * @return 1 if found, 0 if not found.
 *
 * @note Non-matching: beq delay slot filling. Original puts
 * `li v0, 1` (return value) in the beq delay slot; compiler puts
 * `addiu a1, 1` (loop increment) instead, adding an extra `j`
 * for the not-found path.
 *
 * @code
 * s32 func_800A9568(s32 a0) {
 *     s32 i = 0;
 *     u8 *ptr = D_800EE9E8;
 *     do {
 *         if (*ptr == a0) return 1;
 *         i++;
 *         ptr += 5;
 *     } while (i < 32);
 *     return 0;
 * }
 * @endcode
 */
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

/**
 * @brief Compute paired lookup results from entity table and return combined.
 *
 * Computes D_80078E00 + a0 * 20 as the base address, loads the byte at
 * offset 0x3EE7, calls func_800B0F9C and func_800B0F7C with it, and
 * returns the bitwise OR of both results masked to 16 bits.
 *
 * @param a0 Entity index (stride 20 in D_80078E00).
 * @return Combined result from both lookups, masked to u16.
 */
s32 func_800A972C(s32 a0) {
    s32 addr = (s32)D_80078E00;
    s32 base = addr + a0 * 20;
    s32 result = func_800B0F9C(*(u8 *)(base + 0x3EE7));
    result |= func_800B0F7C(*(u8 *)(base + 0x3EE7));
    return (u16)result;
}

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

/**
 * @brief Find a random available entity (among first 3) and return its bitmask.
 *
 * Calls func_800AE730 to get entity count. If 0xFF, returns 0.
 * Otherwise loops calling func_800A97A4(3) to pick random indices,
 * checking entity[0x90] bit 0 (busy flag). Returns (1 << idx) & 0xFFFF
 * when an available entity is found.
 *
 * @return Bitmask of selected entity, or 0 if none available.
 */
s32 func_800A980C(void) {
    s32 idx;
    u8 *table;
    if (func_800AE730() == 0xFF) {
        return 0;
    }
    table = D_800ED148;
    do {
        idx = func_800A97A4(3);
    } while (*(u16 *)(table + idx * 0xD0 + 0x90) & 1);
    return (1 << idx) & 0xFFFF;
}

/**
 * @brief Find a random available entity (among 4, offset by 3) and return its bitmask.
 *
 * Calls func_800AE788 to get entity count. If 0xFF, returns 8.
 * Otherwise loops calling func_800A97A4(4), adds 3 to the index,
 * checks entity[0x90] bit 0 (busy flag). Returns (1 << (idx+3)) & 0xFFFF
 * when an available entity is found.
 *
 * @return Bitmask of selected entity, or 8 if none available.
 */
s32 func_800A9888(void) {
    s32 idx;
    u8 *table;
    if (func_800AE788() == 0xFF) {
        return 8;
    }
    table = D_800ED148;
    do {
        idx = func_800A97A4(4) + 3;
    } while (*(u16 *)(table + idx * 0xD0 + 0x90) & 1);
    return (1 << idx) & 0xFFFF;
}

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

/**
 * @brief Populate entity alive flags array from status bits.
 *
 * Iterates over 7 entities at stride 0xD0 from D_800ED148. For each,
 * checks bit 8 (0x100) of the flags word at offset 0x8C. Writes 1 to
 * D_800EEBE0[i] if the bit is set, 0 otherwise. If a0 equals 0xC8,
 * calls func_800A9938 to toggle the result bits.
 *
 * @param a0 Trigger value — if 0xC8, post-processes the flags.
 */
void func_800A9970(s32 a0) {
    s32 i = 0;
    s32 one = 1;
    u8 *output = D_800EEBE0;
    u8 *entity = D_800ED148;
    do {
        if (*(s32 *)(entity + 0x8C) & 0x100) {
            *output = one;
        } else {
            *output = 0;
        }
        output++;
        i++;
        entity += 0xD0;
    } while (i < 7);
    if (a0 == 0xC8) {
        func_800A9938();
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A99E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9A6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9AC0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800A9E08);

/**
 * @brief Clear D_800EEBE0 entries for inactive entities.
 *
 * Loops over 7 entities at D_800ED148 (stride 0xD0). If bit 0 of the
 * word at offset 0x8C is not set, clears D_800EEBE0[i] to zero.
 */
void func_800A9F98(void) {
    s32 i = 0;
    s32 arr = (s32)D_800EEBE0;
    s32 ptr = (s32)D_800ED148;
    do {
        if (!(*(s32 *)(ptr + 0x8C) & 1)) {
            *(u8 *)(i + arr) = 0;
        }
        i++;
        ptr += 0xD0;
    } while (i < 7);
}

/**
 * @brief Clear D_800EEBE0 entries for inactive or dead entities.
 *
 * Iterates 7 entities (stride 0xD0 in D_800ED148). Clears
 * D_800EEBE0[i] to zero unless the entity is both active and alive.
 */
void func_800A9FDC(void) {
    s32 i = 0;
    s32 arr = (s32)D_800EEBE0;
    s32 ptr = (s32)D_800ED148;
top:
    if (!(*(s32 *)(ptr + 0x8C) & 1) || (*(u16 *)(ptr + 0x90) & 1)) {
        *(u8 *)(i + arr) = 0;
    }
    i++;
    ptr += 0xD0;
    if (i < 7) goto top;
}

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

/**
 * @brief Search entity table for active entity linked to index a0.
 *
 * @param a0 Link index to search for.
 * @return Entity index if found, 0xFF otherwise.
 */
s32 func_800AA530(s32 a0) {
    s32 i = 0;
    s32 base = (s32)D_800ED148;
    s32 ptr = base;
    s32 result;
top:
    if (!(*(u16 *)(ptr + 0x90) & 1)) {
        result = i;
        if (*(u8 *)(ptr + 0xCB) == a0) {
            return result;
        }
    }
    i++;
    ptr += 0xD0;
    if (i < 7) goto top;
    return 0xFF;
}

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

/**
 * @brief Test a specific bit in D_8007809A, optionally inverting.
 *
 * If a0 is 0, returns bit a1 of D_8007809A.
 * If a0 is 3, returns the inverse of bit a1 of D_8007809A.
 * Otherwise returns 0 (falls through without setting return value).
 *
 * @param a0 Mode selector (0 = normal, 3 = inverted).
 * @param a1 Bit position to test.
 * @return Bit value (0 or 1).
 */
s32 func_800AA9C8(s32 a0, s32 a1) {
    if (a0 == 0) {
        return (*(u8 *)D_8007809A >> a1) & 1;
    }
    if (a0 == 3) {
        return ((*(u8 *)D_8007809A >> a1) & 1) ^ 1;
    }
}

/**
 * @brief Check battle state flag at D_80077378+0xCD4.
 *
 * If a0 == 0, returns 1 when the flag is zero (sltiu pattern).
 * If a0 == 3, returns 1 when the flag is nonzero (sltu pattern).
 * Otherwise returns 0.
 *
 * @param a0 Query mode (0 or 3).
 * @return Boolean result based on the flag value.
 *
 * @note Non-matching: CC1PSX folds D_80077378+0xCD4 into a single
 * symbol+constant address (lui/lw, 2 instructions per access) instead
 * of the original's unfolded lui/addiu/lw (3 instructions). No runtime
 * component to prevent folding. Also changes delay slot fill and branch
 * structure.
 */
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

/**
 * @brief Clear control flags and reset animation state for a battle entity.
 *
 * Clears CTRL_FLAG_40, CTRL_FLAG_80, and CTRL_FLAG_02 from the entity's
 * controlFlags field, then calls func_800AE6C0 and func_800A59AC to
 * reset animation state.
 *
 * @param idx Entity index (stride 0xD0 in D_800ED148).
 */
void func_800AB11C(s16 idx) {
    BattleEntity *base = (BattleEntity *)D_800ED148;
    volatile s32 *flags = &base[idx].controlFlags;
    *flags &= ~CTRL_FLAG_40;
    *flags &= ~CTRL_FLAG_80;
    *flags &= ~CTRL_FLAG_02;
    func_800AE6C0();
    func_800A59AC(idx, 0, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object5", func_800AB1AC);

/**
 * @brief Allocate entry via func_8009B3D0 and store value at offset 8.
 *
 * Calls func_8009B3D0 with func_800AB1AC as callback, shifts result
 * left by 4 (multiply by 16), adds D_800EE28C as base, then stores a0
 * as halfword at offset 8 of the computed entry.
 *
 * @param a0 Value to store as halfword.
 */
void func_800AB208(s32 a0) {
    s32 idx = func_8009B3D0(func_800AB1AC);
    u8 *entry = (u8 *)((s32)D_800EE28C + idx * 16);
    *(s16 *)(entry + 8) = a0;
}

/**
 * @brief Find the first active entity (bit 0 set at offset 0x8C).
 *
 * Scans entities 3-6 in the D_800ED148 array (offset 0x270, stride 0xD0).
 * Returns the index of the first entity whose word at offset 0x8C has bit 0 set.
 * Returns 0xFF if no active entity is found.
 *
 * @return Entity index (3-6), or 0xFF if none active.
 */
s32 func_800AB24C(void) {
    s32 i = 3;
    s32 base = (s32)D_800ED148;
    s32 entry = base + 0x270;
    do {
        if (!(*(s32 *)(entry + 0x8C) & 1)) {
            return i;
        }
        i++;
        entry += 0xD0;
    } while (i < 7);
    return 0xFF;
}

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

/**
 * @brief Resolve animation data and display it with position and timing.
 *
 * Looks up animation data from the entity's sub-object table at offset 0x14,
 * resolves it through func_800A9784 and func_800B0398, then calls
 * func_8009AF3C to display with the given Y position, fixed params.
 *
 * @param a0 Entity index (stride 0xD0 in D_800ED148).
 * @param a1 Sub-animation index (multiplied by 2 for table lookup).
 * @param a2 Y position for display.
 */
void func_800AB3FC(s32 a0, s32 a1, s32 a2) {
    volatile u8 *base = D_800ED148;
    u8 *entity = (u8 *)base + a0 * 0xD0;
    s32 sub = *(s32 *)(entity + 0x14);
    s32 tbl = *(s32 *)sub;
    s32 offTab = *(s32 *)(tbl + 8) + tbl;
    s32 dataOff = *(s32 *)(tbl + 0xC);
    s32 result;
    a1 = a1 * 2 + offTab;
    result = func_800A9784(*(u16 *)a1, dataOff + tbl);
    result = func_800B0398(result);
    func_8009AF3C(result, a2, 3, 0xF0, 0);
}

/**
 * @brief Call func_800AB3FC with a fixed duration of 0x1E.
 *
 * @param a0 First parameter passed through.
 * @param a1 Second parameter passed through.
 */
void func_800AB488(s32 a0, s32 a1) {
    func_800AB3FC(a0, a1, 0x1E);
}
