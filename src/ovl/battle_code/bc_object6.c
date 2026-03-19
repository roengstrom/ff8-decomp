#include "common.h"

extern u8 D_80078720[];
extern u8 D_800ED148[];
extern u8 D_80082C0F[];
extern u8 D_800EE449[];
extern u8 D_800ED70C[];
extern u8 D_800EE441[];
extern u8 D_800EE9E8[];
extern u8 D_80077EBC[];
s32 func_800A980C(void);
s32 func_800A9888(void);
void func_8009AD7C(void);
void func_8009AF14(void *);
extern u8 D_80077E59[];
void func_800AD4A4(s32);
void func_800AE6C0(void);
s32 func_8009AF3C(s32, s32, s32, s32, s32);
void func_80048BB8(s32);
void func_80012D5C(void);
extern u8 D_80082C08[];
extern u8 g_gameState[];
void func_800389CC(void);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB4A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB570);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB668);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB6F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB744);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB844);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB914);

/**
 * @brief Read a little-endian 16-bit signed value from two bytes.
 *
 * Combines ptr[0] (low byte) and ptr[1] (high byte) into a signed 16-bit value.
 *
 * @param ptr Pointer to two bytes in little-endian order.
 * @return The sign-extended 16-bit value.
 */
s16 func_800AB998(u8 *ptr) {
    return (s16)(ptr[0] + (ptr[1] << 8));
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB9B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ABA3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ABCA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC034);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC094);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC190);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC2E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC348);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC368);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC400);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC4E4);

/**
 * @brief Check if an entity is available (no blocking status flags).
 *
 * Checks halfword at +0x90 for bits 0 and 2 (mask 0x5),
 * word at +0x18 for bits 0, 3, 14 (mask 0x4009),
 * and word at +0x8C for bit 14 (0x4000).
 *
 * @param a0 Entity index (stride 0xD0).
 * @return 1 if entity is available, 0 otherwise.
 *
 * @note Non-matching: CC1PSX uses srl+xori for the 0x4000 bit test
 * instead of andi+sltiu. Both bnez branches share the same return
 * label with v0=0 in delay slots; CC1PSX generates separate basic blocks.
 * Also, multiply chain is emitted before lui/addiu for D_800ED148.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ACED4);

/**
 * @brief Check if an entity is available (stricter status check).
 *
 * Checks halfword at +0x90 for bits 0, 2, 5 (mask 0x25),
 * word at +0x18 for bits 0, 3 (mask 0x9),
 * and word at +0x8C for bit 14 (0x4000).
 *
 * @param a0 Entity index (stride 0xD0).
 * @return 1 if entity is available, 0 otherwise.
 *
 * @note Non-matching: Same issues as func_800ACED4 (srl+xori bit test,
 * separate return basic blocks, multiply-before-lui ordering).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ACF2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ACF84);

/**
 * @brief Set bit 6 of entity flags at 0x8C, call func_800AE6C0, then set
 * bit 0 of entity halfword at 0x90.
 *
 * Computes entity address at D_800ED148 + a0 * 0xD0, sets bit 6 (0x40)
 * of the word at entity offset 0x8C, calls func_800AE6C0, then sets
 * bit 0 (0x1) of the halfword at entity offset 0x90.
 *
 * @param a0 Entity index (stride 0xD0).
 */
void func_800AD4A4(s32 a0) {
    u8 *entityBase;
    s32 entity;
    entityBase = D_800ED148;
    entity = (s32)entityBase + a0 * 0xD0;
    *(volatile s32 *)(entity + 0x8C) |= 0x40;
    func_800AE6C0();
    *(u16 *)(entity + 0x90) |= 0x1;
}

/**
 * @brief Call func_800AD4A4 then clear bit 0 of entity word at offset 0x8C.
 *
 * Calls func_800AD4A4 with the entity index, then clears the least
 * significant bit of the 32-bit word at D_800ED148 + a0 * 208 + 0x8C.
 *
 * @param a0 Entity index (stride 0xD0).
 */
void func_800AD50C(s32 a0) {
    func_800AD4A4(a0);
    {
        volatile u8 *base = D_800ED148;
        u8 *entity = (u8 *)base + a0 * 0xD0;
        *(s32 *)(entity + 0x8C) &= ~1;
    }
}

s32 func_800A9784(s32, s32);
void func_800B0398(s32);

/**
 * @brief Look up animation data for entity and process it.
 *
 * Computes entity pointer from D_800ED148 + a0 * 0xD0. Loads a
 * sub-object pointer at entity+0x14, dereferences it to get a base
 * pointer, then uses relative offsets at base+8 and base+0xC to
 * look up a u16 entry and a data pointer. Calls func_800A9784 with
 * the u16 entry and data pointer, then func_800B0398 with the result.
 *
 * @param a0 Entity index (stride 0xD0).
 * @param a1 Sub-index for u16 table lookup.
 */
void func_800AD564(s32 a0, s32 a1) {
    volatile u8 *base = D_800ED148;
    u8 *entity = (u8 *)base + a0 * 0xD0;
    s32 sub = *(s32 *)(entity + 0x14);
    s32 tbl = *(s32 *)sub;
    s32 offTab = *(s32 *)(tbl + 8) + tbl;
    s32 dataOff;
    a1 = a1 * 2 + offTab;
    dataOff = *(s32 *)(tbl + 0xC);
    a0 = func_800A9784(*(u16 *)a1, dataOff + tbl);
    func_800B0398(a0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD5D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD6D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD7A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD8E4);

/**
 * @brief Dispatch queued entity action and clear the queue.
 *
 * If the entity action pointer at D_800ED148+0x12DC is non-zero,
 * loads the byte from D_80077E59, computes palette offset (byte*8+8),
 * calls func_8009AF3C with the entity pointer and palette params,
 * then clears the action pointer.
 */
void func_800AD960(void) {
    s32 base = (s32)D_800ED148;
    if (*(volatile s32 *)(base + 0x12DC) != 0) {
        u8 byte = *(u8 *)D_80077E59;
        func_8009AF3C(*(volatile s32 *)(base + 0x12DC), (s32)byte * 8 + 8, 3, 0x80, 0x56);
        *(s32 *)(base + 0x12DC) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD9C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADAC0);

/**
 * @brief Call one of two processing functions based on entity count.
 *
 * If the input is less than 3, calls func_800A9888; otherwise calls
 * func_800A980C. Returns the lower 16 bits of the result.
 *
 * @param count Entity count.
 * @return Result masked to 16 bits.
 */
u16 func_800ADC10(s32 count) {
    s32 result;
    if (count >= 3) {
        result = func_800A980C();
    } else {
        result = func_800A9888();
    }
    return (u16)result;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADC48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADDAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADEA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADF08);

/**
 * @brief Search D_800EE9E8 table for an entry matching the given value.
 *
 * Iterates 32 entries at stride 5 in D_800EE9E8. If byte[0] matches a0,
 * returns the signed byte at offset 1. Returns 0 if no match found.
 *
 * @param a0 Value to search for.
 * @return Signed byte at offset 1 of matching entry, or 0 if not found.
 */
/**
 * @note Non-matching: CC1PSX inverts bne to beq and fills delay slot,
 * producing 15 instructions instead of 17. Branch direction inversion
 * in search loop.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE390);

/**
 * @brief Clear the entry in D_80077EBC that matches the given value.
 *
 * Searches up to 198 entries at stride 2 in D_80077EBC. If byte[0]
 * matches a0, clears both bytes of that entry and returns.
 *
 * @param a0 Value to search for and clear.
 */
void func_800AE3D4(s32 a0) {
    u8 *base = D_80077EBC;
    s32 i = 0;
    do {
        if (base[0] == a0) {
            base[0] = 0;
            base[1] = 0;
            return;
        }
        i++;
        base += 2;
    } while (i < 0xC6);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE414);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE4A0);

/**
 * @brief Compute entity pointer from index and call init functions.
 *
 * Computes D_800ED70C + a0 * 20 to get entity pointer, then calls
 * func_8009B134 with constants 0x68 and 0x80, followed by func_800AD960.
 *
 * @param a0 Entity index (stride 20).
 */
void func_800AE524(s32 a0) {
    u8 *entry = (u8 *)((s32)D_800ED70C + a0 * 20);
    func_8009B134(0x68, 0x80, entry);
    func_800AD960();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE568);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE5D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE64C);

/**
 * @brief Store two display values from func_800AE5D8 and func_800AE64C.
 *
 * Calls func_800AE5D8, stores result to D_80078720[0x570] as u16,
 * then calls func_800AE64C, stores result to D_80078720[0x572].
 */
void func_800AE6C0(void) {
    s32 val = func_800AE5D8();
    u8 *base = D_80078720;
    *(u16 *)(base + 0x570) = val;
    *(u16 *)(base + 0x572) = func_800AE64C();
}

/**
 * @brief Find the first active entity among slots 0-2.
 *
 * Scans up to 3 entities (stride 0xD0) in D_800ED148. Returns
 * the index of the first entity with bit 0 of the word at offset
 * 0x8C set.
 *
 * @return Entity index (0-2) if found.
 *
 * @note Non-matching: CC1PSX fills beqz delay slot with loop increment
 * instead of return value copy, generating a separate addu for the
 * return path.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE6F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE730);

/**
 * @brief Find first available entity in slots 3-6.
 *
 * Scans entities 3-6 (stride 0xD0) at D_800ED148+0x270. Returns
 * the index of the first entity where bits 0 and 2 of the halfword
 * at offset 0x90 are both clear.
 *
 * @return Entity index (3-6), or 0xFF if none available.
 *
 * @note Non-matching: CC1PSX combines `!(flags & 1) && !(flags & 4)`
 * into a single `andi v0,v0,5` test. Original tests bits separately,
 * scheduling the second andi in the first branch's delay slot.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE788);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE7D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE83C);

/**
 * @brief Search first 3 entities for one matching a condition.
 *
 * Iterates entities 0-2 at stride 0xD0 from D_800ED148. For each
 * entity with bit 0 of the word at offset 0x8C set, calls
 * func_800ACED4. Returns 1 on the first match, 0 if none found.
 *
 * @return 1 if a matching entity was found, 0 otherwise.
 */
s32 func_800AE8A0(void) {
    s32 i = 0;
    u8 *base = D_800ED148;
    do {
        if (*(s32 *)(base + 0x8C) & 1) {
            if (func_800ACED4(i) != 0) {
                return 1;
            }
        }
        i++;
        base += 0xD0;
    } while (i < 3);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE90C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEA0C);

extern u8 D_800EE446[];
s32 func_80020F84(s32);

/**
 * @brief Start battle sequence with optional character animation.
 *
 * Sets D_800EE446 to 1, calls func_8009B134(0x70, 0x80, 0) to
 * allocate a message entry, then func_8009AE08(0xA) to set mode.
 * If a0 is not -1, calls func_80020F84 to look up the character,
 * then func_8009AF3C with animation parameters derived from
 * D_80077E59 and a stack argument of 0x56.
 *
 * @param a0 Character index, or -1 to skip animation setup.
 */
void func_800AEACC(s32 a0) {
    s32 saved = a0;
    *(u8 *)D_800EE446 = 1;
    func_8009B134(0x70, 0x80, 0);
    func_8009AE08(0xA);
    if (saved != -1) {
        s32 result = func_80020F84(saved);
        s32 idx = *(u8 *)D_80077E59;
        func_8009AF3C(result, idx * 8 + 8, 3, 0x80, 0x56);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEB50);

/**
 * @brief Check conditions and trigger callback mode 3 for entity system.
 *
 * Returns early if D_80082C08[7] is non-zero, or if bit 2 of the
 * halfword at D_80082C08+2 is clear, or if g_gameState[0xCD4] is
 * non-zero, or if the halfword at D_80082C08 equals 0x13D.
 * Otherwise calls func_800AEACC(-1), sets mode to 3, stores 3 in
 * D_800EE449, and registers func_8009AD7C as callback.
 */
void func_800AEC04(void) {
    u8 *base = D_80082C08;

    if (base[7] != 0) {
        return;
    }
    if (!(*(u16 *)(base + 2) & 4)) {
        return;
    }
    {
        s32 gstate = (s32)g_gameState;
        REGALLOC_BARRIER(gstate);
        if (*(s32 *)(gstate + 0xCD4) != 0) {
            return;
        }
    }
    if (*(u16 *)base == 0x13D) {
        return;
    }
    func_800AEACC(-1);
    base[7] = 3;
    *(u8 *)D_800EE449 = 3;
    func_8009AF14(func_8009AD7C);
}

/**
 * @brief Set callback mode 3 and register func_8009AD7C.
 *
 * Stores 1 to D_80082C0F, stores 3 to D_800EE449, then calls
 * func_8009AF14 with func_8009AD7C as the callback.
 */
void func_800AEC98(void) {
    *(u8 *)D_80082C0F = 1;
    *(u8 *)D_800EE449 = 3;
    func_8009AF14(func_8009AD7C);
}

/**
 * @brief Check entity state and trigger retreat if conditions met.
 *
 * Returns early if D_80082C0F is non-zero, or if entity at D_800ED148
 * offset 0x12F9 equals 1, or if byte at 0x132D is zero. Otherwise
 * calls func_800AEACC(-1) and func_800AEC98.
 */
void func_800AECD4(void) {
    s32 base;
    if (*(u8 *)D_80082C0F != 0) {
        return;
    }
    base = (s32)D_800ED148;
    if (*(u8 *)(base + 0x12F9) == 1) {
        return;
    }
    if (*(u8 *)(base + 0x132D) == 0) {
        return;
    }
    func_800AEACC(-1);
    func_800AEC98();
}

/**
 * @brief Check entity state and trigger action if conditions met.
 *
 * Returns early if D_80082C0F is non-zero, or if D_800EE441 equals 1.
 * Calls func_800AE730 and checks if result is 0xFF. If not, returns.
 * Calls func_800B2128 and if result is non-zero, returns. Otherwise
 * calls func_800AEACC(0) and func_800AEC98.
 */
void func_800AED30(void) {
    if (*(u8 *)D_80082C0F != 0) {
        return;
    }
    if (*(u8 *)D_800EE441 == 1) {
        return;
    }
    if (func_800AE730() != 0xFF) {
        return;
    }
    if (func_800B2128() != 0) {
        return;
    }
    func_800AEACC(0);
    func_800AEC98();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AED9C);

/**
 * @brief Trigger battle end sequence.
 *
 * Calls func_80048BB8(0) to stop processing, sets D_80082C0F to 5,
 * clears byte at D_800ED148 + 0xC, then calls func_80012D5C and
 * func_800389CC for cleanup.
 */
/**
 * @brief Trigger battle end sequence.
 *
 * Calls func_80048BB8(0) to stop processing, sets D_80082C0F to 5,
 * clears byte at D_800ED148 + 0xC, then calls func_80012D5C and
 * func_800389CC for cleanup.
 */
void func_800AEE64(void) {
    func_80048BB8(0);
    *(u8 *)D_80082C0F = 5;
    {
        volatile u8 *base = D_800ED148;
        base[0xC] = 0;
    }
    func_80012D5C();
    func_800389CC();
}

extern u8 D_80078DF8[];
s32 func_8009B15C(void);

/**
 * @brief Classify the current animation frame into a range bucket.
 *
 * Calls func_8009B15C to get the current frame value. Checks bit 2
 * of D_80078DF8 to select between two sets of range thresholds.
 * Returns 0-3 based on which range the frame falls into.
 *
 * @return Range classification: 0 (low), 1 (mid), 2 (high), 3 (very high).
 *
 * @note Non-matching: compiler allocates result to $v1 instead of $v0,
 * adding an extra `move v0, v1` at end. Original reuses $v0 for both
 * slti destination and result variable.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEEAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEF34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AF068);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AF134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AF254);
