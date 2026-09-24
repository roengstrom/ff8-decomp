#include "common.h"
#include "battle.h"
#include "battle/scene_data.h"
#include "gamestate.h"
#include "battle/bc_object7.h"

static s32 func_800B054C(u32 arg0);

extern u8 D_800EE490[];
extern u8 D_80077EBC[];
extern u8 D_800EEBE8[];
s32 func_800B0204(u8 *, s32, s32, s32);
void func_800A4C84(s32);
void func_800AE524(s32);
extern u8 D_800E3CF0[];
extern u8 D_800EE4E8[];
void func_800AE4A0(void);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF358);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF4BC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF5E0);

/**
 * @brief Re-init the 32-slot anim init table at @c D_800EE9E8 by feeding
 *        each slot's @c (id, value) pair plus @c D_80077EBC into
 *        @c func_800AF5E0.
 */
void func_800AF654(void) {
    u8 *constPtr = D_80077EBC;
    s32 i;
    for (i = 0; i < 0x20; i++) {
        func_800AF5E0(D_800EE9E8.animSlots[i].id, D_800EE9E8.animSlots[i].value, constPtr);
    }
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
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF6BC);

extern void func_800AF6BC(s32 a0);

/**
 * @brief For each of the 3 party slots, mirror the entity's display status
 *        into the matching @c BattleCharData and refresh its anim table entry.
 *
 * Walks @c D_800ED148.entities[0..2] (BattleSystem block) — for any slot whose
 * @c linkedIdx is not 0xFF, calls @c func_800AF6BC(i) (which copies the
 * entity's animation halfwords into the per-character anim cache) and then
 * mirrors @c entity->status into @c g_battleChars.chars[i].displayStatus.
 * Finishes by calling @c func_800AF654 to rebuild the global anim list.
 */
void func_800AF740(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_800ED148.entities[i].linkedIdx != 0xFF) {
            func_800AF6BC(i);
            g_battleChars.chars[i].displayStatus = D_800ED148.entities[i].status;
        }
    }
    func_800AF654();
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF7C4);

/**
 * @brief Clear two flag bits in the entity's @c controlFlags and bracket
 *        the update with @c func_800A565C / @c func_800A5778 calls.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 */
void func_800AF8A4(s32 a0) {
    func_800A565C(a0);
    D_800ED148.entities[a0].controlFlags &= ~0x8;
    D_800ED148.entities[a0].controlFlags &= ~0x4;
    func_800A5778(a0);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF918);

/**
 * @brief Read the byte at offset 0x14F of the entity's linked data block.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @return Byte at @c (*entities[a0].linkedPtr)[0x14F].
 */
s32 func_800AF988(s32 a0) {
    return (*D_800ED148.entities[a0].entityData)->unk14F;
}

/**
 * @brief Clamp a 16-bit unsigned value to a maximum of 60000.
 *
 * @param a0 Input value (low 16 bits used).
 * @return min(a0 & 0xFFFF, 60000).
 */
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF9C4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AF9E8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AFA64);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AFB5C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800AFD0C);

/**
 * @brief Resolve the kernel pointer keyed by @c entriesA0[a0].lookupId.
 *
 * @param a0 Index into @c g_gfData.entriesA0.
 */
u8* func_800AFF30(s32 a0) {
  return resolveKernelPtr(g_gfData.entriesA0[a0].lookupId, g_gfData.entriesA0Arg);
}

/**
 * @brief Resolve the kernel pointer keyed by @c rows132[a0-0x40].lookupId.
 *
 * @param a0 Index into @c g_gfData.rows132, offset by @c 0x40.
 */
u8* func_800AFF70(s32 a0) {
    BattleSceneData *scene = &g_gfData;
    a0 -= 0x40;
    return resolveKernelPtr(scene->rows132[a0].lookupId, scene->rows132Arg);
}

/**
 * @brief Resolve the kernel pointer keyed by @c entries17[a0].lookupId.
 *
 * @param a0 Index into @c g_gfData.entries17.
 */
u8* func_800AFFB4(s32 a0) {
    return resolveKernelPtr(g_gfData.entries17[a0].lookupId, g_gfData.entries17Arg);
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
 * @brief Return the first word of the data linked from a battle entity.
 *
 * @param idx Entity index into D_800ED148.entities.
 * @return First s32 word at @c entities[idx].linkedPtr.
 */
s32 func_800B0074(s32 idx) {
    return (s32)*D_800ED148.entities[idx].entityData;
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

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B00E8);

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
u8 *func_800B0248(u8* a0, u8 a1, u8* a2) {
    u8 *buf = D_800EEBE8;
    s32 offset = func_800B0204(buf, a0, 0, (u8)a1);
    func_800B0204(buf + offset, a2, offset, 0);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B02AC);

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
 * @brief Resolve the kernel pointer keyed by @c rows8[a0].lookupId.
 *
 * @param a0 Index into @c g_gfData.rows8.
 */
u8* func_800B0360(s32 a0) {
    return resolveKernelPtr(g_gfData.rows8[a0].lookupId, g_gfData.rows8Arg);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0398);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0414);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B04A0);

static s32 func_800B054C(u32 arg0) {
    s32 i;

    for (i = 0; i < 32; i++) {
        if (arg0 == 1) {
            return i;
        }

        arg0 >>= 1;
    }
    
    return i;
}

/**
 * @brief Store scaled animation value at entity's bit position offset.
 *
 * Calls func_800B054C to find the lowest set bit in a1. If the result
 * is less than 14, computes a scale factor from @c GameConfig.battleSpeed and a table
 * value from g_gfData, multiplies them, and stores the result at the
 * entity's bit-indexed halfword slot.
 *
 * @param a0 Entity index (stride 0xD0).
 * @param a1 Bitmask to find lowest set bit.
 */
void func_800B0574(s32 arg0, u32 arg1) {
    s32 temp_v0;
    
    temp_v0 = func_800B054C(arg1);
    if (temp_v0 < 14) {
        u8 val = g_gfData.unk4CCC[temp_v0];
        s32 temp = ((g_gameState.config.battleSpeed + 1) * 4);
        D_800ED148.entities[arg0].field64.perBit[temp_v0] = val * temp;
    }
}

/**
 * @brief Store the reset sentinel @c -0x457 in the entity's per-bit
 *        halfword slot indexed by the lowest set bit of @p a1.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @param a1 Bitmask whose lowest set bit selects the slot in @c field64.
 */
void func_800B0600(s32 a0, s32 a1) {
    s32 bitPos = func_800B054C(a1);
    if (bitPos < 14) {
        D_800ED148.entities[a0].field64.perBit[bitPos] = -0x457;
    }
}

/**
 * @brief Test whether the entity's per-bit halfword slot (selected by the
 *        lowest set bit of @p a1) currently holds the reset sentinel.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @param a1 Bitmask whose lowest set bit selects the slot in @c field64.
 * @return 1 if @c field64[bitPos] == -0x457, 0 otherwise.
 */
s32 func_800B0668(s32 a0, s32 a1) {
    s32 bitPos = func_800B054C(a1);
    if (bitPos < 14) {
        if (D_800ED148.entities[a0].field64.perBit[bitPos] == -0x457) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Process entity ability and trigger state transitions.
 *
 * Masks @p arg0 to 16 bits and calls @c func_800A4C84. If @c sys->unkE
 * is zero, transitions to state 5, calls @c func_800AE524 with the
 * preceding entry index (@c sys->unk5C0 - 1), clears that entry's
 * @c unk10 byte, then transitions to state 6.
 *
 * @param arg0 Entity bitmask (16-bit).
 */
void func_800B06DC(u16 arg0) {
    func_800A4C84(arg0);
    if (D_800ED148.entities[0].unkE == 0) {
        func_8009AE08(5);
        func_800AE524(D_800ED148.unk5C0 - 1);
        D_800ED148.entries[D_800ED148.unk5C0 - 1].unk11 = 0;
        func_8009AE08(6);
    }
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
void func_800B0754(s32 a0, s32 a1, s32 a2, u16 a3) {
    func_800A30F8(a0, a1, a2, 0, a0, a3, 0);
    func_800B06DC(a3);
}

/**
 * @brief Handle special battle action flags for an entity.
 *
 * If @p a1 has bit @c 0x400 set, calls @c func_800A59AC with mode 5 and
 * returns 1. If @p a1 has bit @c 0x1000 set, sets bit 2 in the entity's
 * @c status field and calls @c func_800A2520. Otherwise returns 0.
 *
 * @param a0 Entity index into @c D_800ED148.entities.
 * @param a1 Action flags bitmask.
 * @return 1 if bit @c 0x400 action taken, 0 otherwise.
 */
s32 func_800B0794(s32 a0, s32 a1) {
    if (a1 & 0x400) {
        func_800A59AC(a0, 5, 0);
        return 1;
    }
    if (a1 & 0x1000) {
        D_800ED148.entities[a0].status |= 4;
        func_800A2520(a0);
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0808);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B08AC);

/**
 * @brief Call func_800A59AC with a1=6 and a2=0.
 *
 * @param a0 First argument passed through.
 */
void func_800B095C(s32 a0) {
    func_800A59AC(a0, 6, 0);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0980);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B09F0);

/**
 * @brief Process entities whose @c status has neither bit 0 nor bit 2 set.
 *
 * Walks @c D_800ED148.entities[0..6]; for each slot whose @c status & 5
 * is zero, calls @c func_800B09F0(i).
 */
void func_800B0C08(void) {
    s32 i;
    for (i = 0; i < 7; i++) {
        if ((D_800ED148.entities[i].status & 5) == 0) {
            func_800B09F0(i);
        }
    }
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0C68);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0CC4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0D8C);

/**
 * @brief Dispatch call based on @ref SEALED_FLAG_02 in D_80082C10.
 *
 * If @ref SEALED_FLAG_02 is set, passes 0xFF to func_800B0CC4.
 * Otherwise calls func_800B0D8C with a0 and mode 2, then passes
 * the result to func_800B0CC4.
 *
 * @param a0 Entity parameter for func_800B0D8C and func_800B0CC4.
 */
s32 func_800B0DDC(s32 a0) {
    s32 val;
    if (D_80082C10 & SEALED_FLAG_02) {
        val = 0xFF;
    } else {
        val = func_800B0D8C(a0, 2);
    }
    return func_800B0CC4(a0, val);
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0E30);

/**
 * @brief Dispatch call based on @ref SEALED_FLAG_01 in D_80082C10.
 *
 * If @ref SEALED_FLAG_01 is set, passes 0xFF to func_800B0E30.
 * Otherwise calls func_800B0D8C with a0 and mode 4, then passes
 * the result to func_800B0E30.
 *
 * @param a0 Entity parameter for func_800B0D8C.
 */
s32 func_800B0F3C(s32 a0) {
    s32 val;
    if (D_80082C10 & SEALED_FLAG_01) {
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
u16 func_800B0F7C(s32 arg0) {
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

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B0F9C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B1050);

/**
 * @brief Compute combined ability flags for the spell record at the given ID.
 *
 * Indexes into the spell array at offset 0x226 of g_gfData (stride 60),
 * reads the magicId byte, passes it to func_800B1050 and func_800B0F7C,
 * and returns the OR of both results masked to 16 bits.
 *
 * @param a0 Spell ID (index into BattleSceneSpells.spells).
 * @return Combined 16-bit ability flags.
 */
u16 func_800B1104(s32 a0) {
    return func_800B1050(g_gfData.spells[a0].magicId) | func_800B0F7C(g_gfData.spells[a0].magicId);
}

/**
 * @brief Resolve the action ID and flags for one of the player's command slots.
 *
 * Picks a deterministic-but-pseudorandom variant via func_8009B15C() % 3 and
 * dispatches on the command type stored at g_battleChars.chars[selfIdx].cmdSlots[cmdIdx].
 *
 * - cmd 1 / 12 (Attack-like): writes only *outFlags (no ID resolved).
 * - cmd 2 (Magic): resolves a spell ID via func_800B0DDC; combined element/status flags
 *   are read from g_gfData.spells[id].magicId via func_800B0F9C/F7C/1104.
 * - cmd 4 (GF/Item): resolves an ability ID via func_800B0F3C, calls func_800AF4BC(id, 1)
 *   to consume a charge, then reads flags from g_gfData.abilities[id].abilityId via
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
    u16 v1;

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
            *outFlags = func_800B0F9C(g_gfData.spells[a].magicId) | func_800B0F7C(g_gfData.spells[*outId].magicId);
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
            *outFlags = func_800B1050(g_gfData.abilities[*outId].abilityId) | func_800B0F7C(g_gfData.abilities[*outId].abilityId);
        } else {
            *outFlags = func_800B0F9C(g_gfData.abilities[*outId].abilityId) | func_800B0F7C(g_gfData.abilities[*outId].abilityId);
        }
        
        return cmd;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B13A0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B1438);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object7", func_800B1564);
