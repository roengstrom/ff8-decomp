#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gamestate.h"
#include "gf.h"

extern u8 D_80078DF8;
extern u8 D_80082C10;
extern CharacterData g_characters[];

extern s32 getXpToNextLevel(u32 exp, s32 charIdx);
extern s32 findCharXpLevel(u32 exp, s32 charIdx);
extern s32 func_80021A64(s32 level, s32 charIdx);
extern s32 func_80021C10(s32 level, s32 charIdx, s32 kind);
extern s32 func_80022228(s32 charIdx, s32 idx);
extern s32 func_8002247C(s32 charIdx, s32 idx);
extern s32 calcHitStat(s32 charIdx);
extern s32 calcEvaStat(s32 charIdx, s32 hit);
extern s32 getAtkElemBase(s32 charIdx);
extern s32 getAtkElemBonus(s32 charIdx);
extern s32 decodeAtkStatusMask(s32 charIdx);
extern s32 getAtkStatusFlags(s32 charIdx);
extern s32 calcAtkStatusHit(s32 charIdx);

INCLUDE_ASM("asm/nonmatchings/gf_anim", func_800229FC);


/** @brief GF animation slot (4 bytes, inside GfAnimState at +0x1E). */
typedef struct {
    u8 gfId;            /* 0x00: GF/animation index. */
    u8 frameStart;      /* 0x01: start frame from g_gfData. */
    u8 frameEnd;        /* 0x02: end frame from g_gfData. */
    u8 frameCounter;    /* 0x03: current frame counter. */
} GfAnimSlot;

/** @brief GF animation state block. */
typedef struct {
    u8 pad[0x1E];
    GfAnimSlot slots[16];
} GfAnimState;

/**
 * @brief Initialize a GF animation slot with look-up data from g_gfData.
 *
 * @param state Animation state block.
 * @param index Slot index.
 * @param gfId GF/animation index to set.
 */
void initGfAnimEntry(GfAnimState *state, s32 index, s32 gfId) {
    state->slots[index].gfId = gfId;
    state->slots[index].frameStart = g_gfData.statTable8[state->slots[index].gfId].typeField;
    state->slots[index].frameCounter = 0;
    state->slots[index].frameEnd = g_gfData.statTable8[state->slots[index].gfId].bonusField;
}


/**
 * @brief Accumulate status immunity flags as an RGB-packed bitmask from a character's equipped abilities.
 * @param a0 Character slot index into g_gameState.chars[].
 * @return Packed bitmask: (B << 16) | (G << 8) | R, OR'd across up to 4 matching ability slots.
 * @note Abilities in range 0x3A..0x4D are status immunity abilities; each has R/G/B
 *       immunity bytes looked up from g_gfData.abilityRangeL[].
 */
s32 getStatusImmunityFlags(u32 a0) {
    s32 result = 0;
    s32 i = 0;
    do {
        s32 val = g_gameState.chars[a0].abilities[i];
        u32 idx = val - 0x3A;
        if (idx < 0x14) {
            u8 b = g_gfData.abilityRangeL[idx].extraField;
            u8 g = g_gfData.abilityRangeL[idx].bonusField;
            u8 r = g_gfData.abilityRangeL[idx].typeField;
            result |= (b << 16) | (g << 8) | r;
        }
        i++;
    } while (i < 4);
    return result;
}


/**
 * @brief Check if any of 4 command slots has command type 6 (Magic).
 * @param charData Battle character data.
 * @return 1 if any slot has type == 6, 0 otherwise.
 */
s32 hasCommandType6(BattleCharData *charData) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (charData->cmdSlots[i].cmdType == 6) return 1;
    }
    return 0;
}


/**
 * @brief Determine a character's magic availability flags based on status and command slots.
 * @param a0 Pointer to a character data structure.
 * @return Flags: bit 0 set if status bit 0x20000 is active; bit 1 set if magic commands present
 *         (unless D_80082C10 bit 3 is set, which suppresses the magic flag).
 */
s32 getMagicAvailFlags(BattleCharData *charData) {
    s32 val = charData->statusFlags;
    s32 masked = val & 0x20000;
    s32 flag = masked != 0;
    if (hasCommandType6(charData)) {
        if (D_80082C10 & 8) {
            return flag;
        }
        flag |= 2;
    }
    return flag;
}


/**
 * @brief Apply party ability flags from a character's equipped abilities to g_battleChars.
 * @param a0 Character slot index into g_gameState.chars[].
 * @note Abilities in range 0x4E..0x52 are party abilities; each value is looked up
 *       in g_gfData.abilityRangeM[] and OR'd into g_battleChars party ability flags
 *       at offset 0x6D8. Likely enables field/world abilities (encounter-none, rare-item).
 */
void applyPartyAbilityFlags(s32 charIdx) {
    s32 i;
    for (i = 0; i < 4; i++) {
        u8 ability = g_gameState.chars[charIdx].abilities[i];
        if ((u32)(ability - 0x4E) < 5) {
            g_battleChars.levelEntries[15].abilityFlags |= g_gfData.abilityRangeM[ability - 0x4E].typeField;
        }
    }
}


INCLUDE_ASM("asm/nonmatchings/gf_anim", func_80022CDC);


/**
 * @brief Clear a character's magic, item, and command slot data.
 * @param a0 Pointer to a character data structure.
 * @note Zeroes out three arrays: 32 entries of 5 bytes at offset 0x82 (magic inventory),
 *       16 entries of 5 bytes at offset 0x122 (item inventory), 4 entries of 4 bytes at
 *       offset 0x1E (command slots), plus fields at 0x1C, 0x1D, and a u16 at 0x14.
 */
void clearCharSlotData(BattleCharData *charData) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        charData->magicSlots[i].field2 = 0;
        charData->magicSlots[i].field3 = 0;
        charData->magicSlots[i].field4 = 0;
        charData->magicSlots[i].field1 = 0;
        charData->magicSlots[i].field0 = 0;
    }

    for (i = 0; i < 0x10; i++) {
        charData->itemSlots[i].field2 = 0;
        charData->itemSlots[i].field3 = 0;
        charData->itemSlots[i].field4 = 0;
        charData->itemSlots[i].field1 = 0;
        charData->itemSlots[i].field0 = 0;
    }

    for (i = 0; i < 4; i++) {
        charData->cmdSlots[i].field2 = 0;
        charData->cmdSlots[i].field3 = 0;
        charData->cmdSlots[i].field1 = 0;
        charData->cmdSlots[i].cmdType = 0;
    }

    charData->field01C = 0;
    charData->field01D = 0;
    charData->field014 = 0;
}


INCLUDE_ASM("asm/nonmatchings/gf_anim", func_80022E08);


/**
 * @brief Find the index of a command slot matching a given command type.
 * @param a0 Pointer to an array of command entries (4 bytes each, type field at offset 0x1E).
 * @param a1 Command type to search for.
 * @return Index (0-3) of the matching slot, or 0xFF if not found.
 */
s32 findCommandSlot(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == a1) return i;
        a0 += 4;
    }
    return 0xFF;
}


/**
 * @brief Clamp a value to the 0-255 (u8) range.
 * @param a0 Value to clamp.
 * @return a0 if < 256, otherwise 255.
 */
s32 clampToByte(s32 a0) {
    if (a0 >= 0x100) {
        return 0xFF;
    }
    return a0;
}


/**
 * @brief Clamp a value to the 0-9999 range.
 * @param a0 Value to clamp.
 * @return a0 as u16 if < 10000, otherwise 9999.
 * @note Commonly used to cap HP or similar stats at the FF8 maximum of 9999.
 */
s32 clampToMaxHp(s32 a0) {
    if (a0 >= 0x2710) {
        return 0x270F;
    }
    return (u16)a0;
}


/**
 * @brief Refresh battle render data for one party slot from save data.
 *
 * Copies a character's magic inventory, level/XP, derived stats, element
 * resistances, and status data from g_characters[charIdx] into the battle
 * character render block at g_battleChars.chars[battleSlot]. Returns
 * immediately if charIdx is 0xFF (empty slot). Finally toggles bit 4 of a
 * matching command slot's status byte based on bit 0x60000 of field188.
 *
 * @param charIdx    Character ID (0-7) into g_characters[], or 0xFF if empty.
 * @param battleSlot Party slot (0-2) into g_battleChars.chars[].
 */
void func_800231E0(s32 charIdx, s32 battleSlot)
{
    CharacterData *cd = &g_characters[charIdx];
    BattleCharData *bc = &g_battleChars.chars[battleSlot];
    s32 i;
    s32 hp;

    if (charIdx == 0xFF) return;

    for (i = 0; i < 32; i++) {
        bc->magicSlots[i].field0 = cd->magic[i].magicId;
        bc->magicSlots[i].field1 = cd->magic[i].quantity;
    }

    func_800229FC(battleSlot);

    bc->xpToNext = getXpToNextLevel(cd->experience, charIdx);
    bc->level = findCharXpLevel(cd->experience, charIdx);

    hp = clampToMaxHp(bc->statCoefs[0] * func_80021A64(bc->level, charIdx) / 100);
    bc->hpRegenCap = hp;
    if ((s16)hp < (s32)cd->currentHp) {
        cd->currentHp = hp;
        bc->field172 = hp;
    }

    bc->stats[0] = clampToByte(bc->statCoefs[1] * func_80021C10(bc->level, charIdx, 1) / 100);
    bc->stats[1] = clampToByte(bc->statCoefs[2] * func_80021C10(bc->level, charIdx, 2) / 100);
    bc->stats[2] = clampToByte(bc->statCoefs[3] * func_80021C10(bc->level, charIdx, 3) / 100);
    bc->stats[3] = clampToByte(bc->statCoefs[4] * func_80021C10(bc->level, charIdx, 4) / 100);
    bc->stats[4] = clampToByte(bc->statCoefs[5] * func_80021C10(bc->level, charIdx, 5) / 100);
    bc->stats[5] = clampToByte(bc->statCoefs[8] * func_80021C10(bc->level, charIdx, 8) / 100);
    bc->stats[7] = clampToByte(bc->statCoefs[7] * calcHitStat(charIdx) / 100);
    bc->stats[6] = clampToByte(bc->statCoefs[6] * calcEvaStat(charIdx, bc->stats[4]) / 100);

    bc->atkElemBase = getAtkElemBase(charIdx);
    bc->atkElemBonus = getAtkElemBonus(charIdx);

    for (i = 0; i < 8; i++) {
        bc->elemResistances[i] = func_80022228(charIdx, i);
    }

    bc->abilityFlags = decodeAtkStatusMask(charIdx);
    bc->abilityValue = getAtkStatusFlags(charIdx);
    bc->atkStatusHit = calcAtkStatusHit(charIdx);

    for (i = 0; i < 13; i++) {
        bc->statusResistances[i] = func_8002247C(charIdx, i);
    }

    if (bc->field188 & 0x60000) {
        s32 idx;
        if (findCommandSlot((u8 *)bc, 2) == 0xFF) return;
        idx = findCommandSlot((u8 *)bc, 2);
        bc->cmdSlots[idx].field3 |= 0x10;
    } else {
        s32 idx;
        if (findCommandSlot((u8 *)bc, 2) == 0xFF) return;
        idx = findCommandSlot((u8 *)bc, 2);
        bc->cmdSlots[idx].field3 &= ~0x10;
    }
}


INCLUDE_ASM("asm/nonmatchings/gf_anim", func_8002363C);


/**
 * @brief Iterate over 16 GF entries and recalculate stats for each active one.
 * @note Checks each GfSaveData.exists for bit 0 (active flag).
 *       Calls func_8002363C for each active GF to recalculate its derived stats.
 */
void recalcAllGfStats(void) {
    s32 i;
    for (i = 0; i < 16; i++) {
        if (g_gameState.gfs[i].exists & 1) {
            func_8002363C(i);
        }
    }
}


/**
 * @brief Recalculate stats for all 3 party members and their GFs.
 * @note Resets D_80078DF8, then for each of the 3 party slots reads the
 *       character ID from g_gameState.mainData.party.party[i] and calls
 *       func_80022E08 and func_800231E0. Finally calls recalcAllGfStats for GFs.
 */
void recalcPartyStats(void) {
    s32 i;

    D_80078DF8 = 0;

    for (i = 0; i < 3; i++) {
        func_80022E08(g_gameState.mainData.party.party[i], i);
        func_800231E0(g_gameState.mainData.party.party[i], i);
    }

    recalcAllGfStats();
}


