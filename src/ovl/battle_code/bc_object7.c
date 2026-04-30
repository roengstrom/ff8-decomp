#include "common.h"
#include "battle.h"

extern u8 D_800ED148[];
extern u8 D_800EE490[];
extern u8 D_80082C10[];
extern u8 D_80077EBC[];
extern u8 D_800EE9E8[];
extern u8 D_800EEBE8[];
extern u8 D_80077E58[];
s32 func_800B0204(u8 *, s32, s32, s32);
void func_800A4C84(s32);
void func_800AE524(s32);
void func_8009AE08(s32);
extern u8 D_800ED158[];
extern u8 g_gameState[];
extern u8 D_800E3CF0[];
extern u8 D_800EE4E8[];
void func_800AE4A0(void);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF358);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF4BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF5E0);

/**
 * @brief Initialize 32 entity entries by calling func_800AF5E0.
 *
 * Iterates over 32 entries at stride 5 in D_800EE9E8, passing the
 * unsigned byte at offset 0 and signed byte at offset 1 along with
 * D_80077EBC to func_800AF5E0.
 */
void func_800AF654(void) {
    u8 *constPtr = D_80077EBC;
    s32 i = 0;
    u8 *base = D_800EE9E8;
    do {
        func_800AF5E0(base[0], *(s8 *)(base + 1), constPtr);
        base += 5;
        i++;
    } while (i < 0x20);
}

/**
 * @brief Copy entity animation data to lookup table and clear a flag.
 *
 * Computes entity pointer from D_800ED158 + a0*0xD0. Reads an index
 * byte from g_gameState + a0 + 0xAF4, multiplies by 0x98 to find a
 * table entry at g_gameState + 0x490. Copies entity halfword at 0x18
 * to the table entry. Clears bit 5 of entity halfword at 0x80 and
 * stores the result at table entry + 0x96.
 *
 * @param a0 Entity index (stride 0xD0).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF6BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF740);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF7C4);

/**
 * @brief Clear bits 3 and 2 of entity's 0x8C word and call func_800A5778.
 *
 * Calls func_800A565C with entity index, then clears bit 3 (0x8) and
 * bit 2 (0x4) of the word at D_800ED148 + a0*0xD0 + 0x8C. Finally
 * calls func_800A5778 with the entity index.
 *
 * @param a0 Entity index (stride 0xD0 in D_800ED148).
 */
void func_800AF8A4(s32 a0) {
    func_800A565C(a0);
    {
        volatile u8 *base = D_800ED148;
        volatile s32 *flags = (volatile s32 *)((u8 *)base + a0 * 0xD0 + 0x8C);
        *flags &= ~0x8;
        *flags &= ~0x4;
    }
    func_800A5778(a0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF918);

/**
 * @brief Get a byte field from an entity's nested pointer chain.
 *
 * Looks up D_800ED148[a0] (stride 208), follows the pointer at +0x10,
 * dereferences it, then returns the byte at offset +0x14F.
 *
 * @param a0 Entity index.
 * @return Byte value at the end of the pointer chain.
 */
s32 func_800AF988(s32 a0) {
    s32 base = (s32)D_800ED148;
    s32 entry = base + a0 * 208;
    s32 ptr = *(s32 *)(entry + 0x10);
    ptr = *(s32 *)ptr;
    return *(u8 *)(ptr + 0x14F);
}

/**
 * @brief Clamp a 16-bit unsigned value to a maximum of 60000.
 *
 * @param a0 Input value (low 16 bits used).
 * @return min(a0 & 0xFFFF, 60000).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF9C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AF9E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFA64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFB5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800AFD0C);

/**
 * @brief Look up entity field and call resolveKernelPtr.
 *
 * Computes D_80078E00 + a0 * 20 to find the entity entry, reads
 * the halfword at offset 0x3EE0 and the word at D_80078E00 + 0xA4,
 * then calls resolveKernelPtr with them.
 *
 * @param a0 Entity index (stride 20).
 */
void func_800AFF30(s32 a0) {
    s32 base = (s32)&D_80078E00;
    u8 *entry = (u8 *)(base + a0 * 20);
    resolveKernelPtr(*(u16 *)(entry + 0x3EE0), *(s32 *)(base + 0xA4));
}

/**
 * @brief Look up entity field and call resolveKernelPtr (stride 132).
 *
 * Subtracts 0x40 from a0, computes D_80078E00 + adjusted * 132,
 * reads halfword at offset 0xF78 and word at D_80078E00 + 0x88.
 *
 * @param a0 Entity index (offset by 0x40, stride 132).
 */
void func_800AFF70(s32 a0) {
    s32 base = (s32)&D_80078E00;
    u8 *entry;
    a0 -= 0x40;
    entry = (u8 *)(base + a0 * 132);
    resolveKernelPtr(*(u16 *)(entry + 0xF78), *(s32 *)(base + 0x88));
}

/**
 * @brief Look up entity field and call resolveKernelPtr (alternate offsets).
 *
 * Same pattern as func_800AFF30 but reads halfword at offset 0x17B8
 * and word at D_80078E00 + 0x8C.
 *
 * @param a0 Entity index (stride 20).
 */
void func_800AFFB4(s32 a0) {
    s32 base = (s32)&D_80078E00;
    u8 *entry = (u8 *)(base + a0 * 20);
    resolveKernelPtr(*(u16 *)(entry + 0x17B8), *(s32 *)(base + 0x8C));
}

/**
 * @brief Call getMenuString with argument 0xA.
 */
void func_800AFFF4(void) {
    getMenuString(0xA);
}

/**
 * @brief Call getMenuString with argument 0xC.
 */
void func_800B0014(void) {
    getMenuString(0xC);
}

/**
 * @brief Call getMenuString with argument 0xD.
 */
void func_800B0034(void) {
    getMenuString(0xD);
}

/**
 * @brief Call getMenuString with argument 0xE.
 */
void func_800B0054(void) {
    getMenuString(0xE);
}

/**
 * @brief Read a pointer from entity data and dereference it.
 *
 * @param idx Entity index (stride 0xD0).
 * @return First word of the data pointed to by entity[0x10].
 */
s32 func_800B0074(s32 idx) {
    u8 *base = D_800ED148;
    u8 *entity;
    asm("");
    entity = base + idx * 0xD0;
    return **(s32 **)(entity + 0x10);
}

/**
 * @brief Call getMenuString with argument 0xF.
 */
void func_800B00A8(void) {
    getMenuString(0xF);
}

/**
 * @brief Call getMenuString with argument 0x10.
 */
void func_800B00C8(void) {
    getMenuString(0x10);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B00E8);

/**
 * @brief Copy a null-terminated string from src to dst.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 */
void func_800B01E8(u8 *dst, u8 *src) {
    u8 ch;
    do {
        ch = *src++;
        *dst++ = ch;
    } while (ch != 0);
}

/**
 * @brief Copy string with optional terminator replacement.
 *
 * Copies bytes from src to dst until a null byte is found, counting
 * the number of non-null bytes copied (added to initial len). After
 * copying, if the terminator byte (masked to 8 bits) equals 7, returns
 * the length. Otherwise, overwrites the null with the terminator byte
 * and returns length + 1.
 *
 * @param a0 Destination buffer.
 * @param a1 Source buffer (as integer).
 * @param a2 Initial length counter.
 * @param a3 Terminator byte (only low 8 bits used).
 * @return Final length of written data.
 */
s32 func_800B0204(u8 *a0, s32 a1, s32 a2, s32 a3) {
    u8 ch;
    goto mid;
copy:
    a2++;
mid:
    ch = *(u8 *)a1;
    a1++;
    *a0 = ch;
    a0++;
    if (ch) goto copy;
    if ((u8)a3 == 7) {
        return a2;
    }
    *(a0 - 1) = (u8)a3;
    return a2 + 1;
}

/**
 * @brief Build a string in D_800EEBE8 from two parts using func_800B0204.
 *
 * Writes the first part with a1 as length byte, then appends the
 * second part starting at the returned offset.
 *
 * @param a0 First part data.
 * @param a1 Length/type byte for first part (masked to 8 bits).
 * @param a2 Second part data.
 * @return Pointer to D_800EEBE8 buffer.
 */
u8 *func_800B0248(s32 a0, s32 a1, s32 a2) {
    u8 *buf = D_800EEBE8;
    s32 offset = func_800B0204(buf, a0, 0, (u8)a1);
    func_800B0204(buf + offset, a2, offset, 0);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B02AC);

/**
 * @brief Copy a string to D_800EE490 and return the buffer pointer.
 *
 * @param src Source string to copy.
 * @return Pointer to D_800EE490.
 */
u8 *func_800B0328(u8 *src) {
    u8 *dst = D_800EE490;
    func_800B01E8(dst, src);
    return dst;
}

/**
 * @brief Look up a halfword from entity table and call resolveKernelPtr.
 *
 * @param a0 Entity index (stride 8 in table).
 */
void func_800B0360(s32 a0) {
    u8 *base = (u8 *)&D_80078E00;
    u16 val = *(u16 *)(base + a0 * 8 + 0x4A5E);
    s32 arg = *(s32 *)(base + 0xD4);
    resolveKernelPtr(val, arg);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0398);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0414);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B04A0);

/**
 * @brief Find the bit position of the lowest set bit.
 *
 * @param a0 Value to scan (should have exactly one bit set).
 * @return Bit position (0-31), or 32 if no single bit found.
 */
s32 func_800B054C(s32 a0) {
    s32 i = 0;
    do {
        if (a0 == 1) {
            return i;
        }
        i++;
        a0 = (u32)a0 >> 1;
    } while (i < 0x20);
    return i;
}

/**
 * @brief Store scaled animation value at entity's bit position offset.
 *
 * Calls func_800B054C to find the lowest set bit in a1. If the result
 * is less than 14, computes a scale factor from D_80077E58 and a table
 * value from D_80078E00, multiplies them, and stores the result at the
 * entity's bit-indexed halfword slot.
 *
 * @param a0 Entity index (stride 0xD0).
 * @param a1 Bitmask to find lowest set bit.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0574);

/**
 * @brief Store animation reset value at entity's bit position offset.
 *
 * Calls func_800B054C to find the lowest set bit in a1. If the result
 * is less than 14, computes the entity at D_800ED148 + a0 * 208 and
 * stores -0x457 (0xFBA9) as a signed halfword at entity + bitPos * 2 + 0x64.
 *
 * @param a0 Entity index (stride 208).
 * @param a1 Bitmask to find lowest set bit.
 *
 */
void func_800B0600(s32 a0, s32 a1) {
    s32 bitPos = func_800B054C(a1);
    if (bitPos < 14) {
        volatile u8 *base = D_800ED148;
        u8 *entity = (u8 *)base + a0 * 0xD0;
        *(s16 *)(entity + bitPos * 2 + 0x64) = -0x457;
    }
}

/**
 * @brief Check if an entity's bit slot contains the sentinel value -0x457.
 *
 * Calls func_800B054C to find the lowest set bit in a1. If the result
 * is less than 14, loads the halfword at entity[result * 2 + 0x64] and
 * returns 1 if it equals -0x457, 0 otherwise.
 *
 * @param a0 Entity index (stride 0xD0).
 * @param a1 Bitmask to find lowest set bit.
 * @return 1 if the slot matches -0x457, 0 otherwise.
 */
s32 func_800B0668(s32 a0, s32 a1) {
    s32 bitPos = func_800B054C(a1);
    if (bitPos < 14) {
        volatile u8 *base = D_800ED148;
        u8 *entity = (u8 *)base + a0 * 0xD0;
        if (*(s16 *)(entity + bitPos * 2 + 0x64) == -0x457) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Process entity ability and trigger state transitions.
 *
 * Masks a0 to 16 bits and calls func_800A4C84. If byte at
 * D_800ED148[0xE] is zero, transitions to state 5, processes
 * the entity ability, clears a table entry, then transitions
 * to state 6.
 *
 * @param a0 Entity bitmask (16-bit).
 */
void func_800B06DC(s32 a0) {
    volatile u8 *base;
    a0 &= 0xFFFF;
    func_800A4C84(a0);
    base = D_800ED148;
    if (base[0xE] != 0) {
        return;
    }
    func_8009AE08(5);
    func_800AE524(base[0x5C0] - 1);
    {
        s32 idx = base[0x5C0] - 1;
        u8 *entry = (u8 *)base + idx * 20;
        entry[0x5D5] = 0;
    }
    func_8009AE08(6);
}

/**
 * @brief Set up extended parameters and call two processing functions.
 *
 * Saves the 16-bit truncation of a3, calls func_800A30F8 with 7 args
 * (a0, a1, a2 passed through, a3 zeroed, plus a0, truncated a3, and 0
 * on the stack), then calls func_800B06DC with the truncated a3 value.
 *
 * @param a0 First parameter (also passed as 5th arg).
 * @param a1 Second parameter passed through.
 * @param a2 Third parameter passed through.
 * @param a3 Fourth parameter (16-bit truncated, passed as 6th arg).
 */
void func_800B0754(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 val = (u16)a3;
    func_800A30F8(a0, a1, a2, 0, a0, val, 0);
    func_800B06DC(val);
}

/**
 * @brief Handle special battle action flags for an entity.
 *
 * If a1 has bit 0x400 set, calls func_800A59AC with mode 5 and returns 1.
 * If a1 has bit 0x1000 set, sets bit 2 of the entity's status halfword
 * at offset 0x90 in D_800ED148 and calls func_800A2520, returns 0.
 * Otherwise returns 0.
 *
 * @param a0 Entity index (stride 0xD0 in D_800ED148).
 * @param a1 Action flags bitmask.
 * @return 1 if bit 0x400 action taken, 0 otherwise.
 */
s32 func_800B0794(s32 a0, s32 a1) {
    if (a1 & 0x400) {
        func_800A59AC(a0, 5, 0);
        return 1;
    }
    if (a1 & 0x1000) {
        volatile u8 *base = D_800ED148;
        u8 *entity = (u8 *)base + a0 * 0xD0;
        *(u16 *)(entity + 0x90) |= 4;
        func_800A2520(a0);
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0808);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B08AC);

/**
 * @brief Call func_800A59AC with a1=6 and a2=0.
 *
 * @param a0 First argument passed through.
 */
void func_800B095C(s32 a0) {
    func_800A59AC(a0, 6, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0980);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B09F0);

/**
 * @brief Process entities that are not busy or disabled.
 *
 * Iterates over 7 entities at stride 0xD0 from D_800ED148.
 * For each entity, checks if bits 0 and 2 of the halfword at
 * offset 0x90 are clear. If so, calls func_800B09F0 with the index.
 */
void func_800B0C08(void) {
    s32 i = 0;
    u8 *base = D_800ED148;
    do {
        if ((*(u16 *)(base + 0x90) & 5) == 0) {
            func_800B09F0(i);
        }
        i++;
        base += 0xD0;
    } while (i < 7);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0CC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0D8C);

/**
 * @brief Dispatch call based on D_80082C10 flag bit 1.
 *
 * If bit 1 of D_80082C10 is set, passes 0xFF to func_800B0CC4.
 * Otherwise calls func_800B0D8C with a0 and mode 2, then passes
 * the result to func_800B0CC4.
 *
 * @param a0 Entity parameter for func_800B0D8C and func_800B0CC4.
 */
s32 func_800B0DDC(s32 a0) {
    s32 val;
    if (*(u8 *)D_80082C10 & 2) {
        val = 0xFF;
    } else {
        val = func_800B0D8C(a0, 2);
    }
    return func_800B0CC4(a0, val);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0E30);

/**
 * @brief Dispatch call based on D_80082C10 flag bit 0.
 *
 * If bit 0 of D_80082C10 is set, passes 0xFF to func_800B0E30.
 * Otherwise calls func_800B0D8C with a0 and mode 4, then passes
 * the result to func_800B0E30.
 *
 * @param a0 Entity parameter for func_800B0D8C.
 */
s32 func_800B0F3C(s32 a0) {
    s32 val;
    if (*(u8 *)D_80082C10 & 1) {
        val = 0xFF;
    } else {
        val = func_800B0D8C(a0, 4);
    }
    return func_800B0E30(val);
}

/**
 * @brief Convert ability flag bits to GF compatibility bitmask.
 *
 * Bit 0 of the input maps to bit 14 (0x4000) of the result,
 * and bit 1 maps to bit 13 (0x2000).
 *
 * @param arg0 Ability flags.
 * @return Bitmask with bits 14 and/or 13 set.
 */
s32 func_800B0F7C(s32 arg0) {
    s32 temp_v1;
    int new_var;
    s32 var_v0;

    temp_v1 = (arg0 & 1) << 0xE;
    new_var = arg0 & 2;
    var_v0 = temp_v1;
    if (new_var) {
        var_v0 = temp_v1 | 0x2000;
        var_v0 = temp_v1;
        var_v0 = var_v0 | 0x2000;
    }
    return var_v0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B0F9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1050);

/**
 * @brief Compute combined ability flags for the spell record at the given ID.
 *
 * Indexes into the spell array at offset 0x226 of D_80078E00 (stride 60),
 * reads the magicId byte, passes it to func_800B1050 and func_800B0F7C,
 * and returns the OR of both results masked to 16 bits.
 *
 * @param a0 Spell ID (index into BattleSceneSpells.spells).
 * @return Combined 16-bit ability flags.
 */
u16 func_800B1104(s32 a0) {
    s32 result;
    result = func_800B1050(D_80078E00.spells[a0].magicId);
    result |= func_800B0F7C(D_80078E00.spells[a0].magicId);
    return (u16)result;
}

/**
 * @brief Resolve the action ID and flags for one of the player's command slots.
 *
 * Picks a deterministic-but-pseudorandom variant via func_8009B15C() % 3 and
 * dispatches on the command type stored at g_battleChars.chars[selfIdx].cmdSlots[cmdIdx].
 *
 * - cmd 1 / 12 (Attack-like): writes only *outFlags (no ID resolved).
 * - cmd 2 (Magic): resolves a spell ID via func_800B0DDC; combined element/status flags
 *   are read from D_80078E00.spells[id].magicId via func_800B0F9C/F7C/1104.
 * - cmd 4 (GF/Item): resolves an ability ID via func_800B0F3C, calls func_800AF4BC(id, 1)
 *   to consume a charge, then reads flags from D_80078E00.abilities[id].abilityId via
 *   func_800B1050/F9C/F7C.
 *
 * @param selfIdx     Party slot index into g_battleChars.chars (0..2).
 * @param cmdIdx      Command slot index (0..3) within the chosen char.
 * @param outId       Output: resolved action ID, or 0xFF on lookup failure.
 * @param outFlags    Output: combined 16-bit element/status flags.
 * @return The command type that was dispatched, or 0 if no match / lookup failed.
 */
s32 func_800B115C(s32 selfIdx, s32 cmdIdx, s32 *outId, u16 *outFlags) {
    u8 m = func_8009B15C() % 3;
    s32 cmd = g_battleChars.chars[selfIdx].cmdSlots[cmdIdx].cmdType;
    s32 a;
    s32 v1;

    *outId = 0;

    switch (cmd) {
    case 1:
    case 12:
        if ((m & 0xFF) != 0) {
            *outFlags = func_800A980C();
        } else {
            *outFlags = func_800A9888();
        }
        return cmd;
    case 2:
        a = func_800B0DDC(selfIdx);
        *outId = a;
        if (a == 0xFF) {
            return 0;
        }
        if ((m & 0xFF) != 0) {
            *outFlags = func_800B1104(a);
        } else {
            v1 = func_800B0F9C(D_80078E00.spells[a].magicId);
            v1 |= func_800B0F7C(D_80078E00.spells[*outId].magicId);
            *outFlags = v1;
        }
        return cmd;
    case 4:
        a = func_800B0F3C(selfIdx);
        *outId = a;
        if (a == 0xFF) {
            return 0;
        }
        func_800AF4BC(a, 1);
        if ((m & 0xFF) != 0) {
            v1 = func_800B1050(D_80078E00.abilities[*outId].abilityId);
            v1 |= func_800B0F7C(D_80078E00.abilities[*outId].abilityId);
        } else {
            v1 = func_800B0F9C(D_80078E00.abilities[*outId].abilityId);
            v1 |= func_800B0F7C(D_80078E00.abilities[*outId].abilityId);
        }
        *outFlags = v1;
        return cmd;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B13A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1438);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object7", func_800B1564);
