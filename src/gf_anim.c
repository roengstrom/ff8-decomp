#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gamestate.h"
#include "gf.h"
#include "gf_anim.h"
#include "game.h"

/** @brief A GF's battle stats (12 bytes). */
typedef struct {
    /* 0x0 */ s16 hp;
    /* 0x2 */ s16 maxHp;
    /* 0x4 */ u32 exp;
    /* 0x8 */ u8 level;
    /* 0x9 */ u8 unk9;
    /* 0xA */ u8 hpPercent; /**< Max HP as a percentage of the level's curve value: 100 plus the learned GF HP+ abilities. */
    /* 0xB */ u8 flags;
} BattleGfStats;

/** BattleGfStats.flags: HP is below a quarter of max. */
#define GF_STATS_LOW_HP 0x80
/** Bit of BattleCharData.statusFlags: the first command is 0x0C instead of 1. */
#define ABILITY_FIRST_CMD_0C 0x01
/** Ability IDs that index g_kernel.commandAbilities and g_kernel.gfAbilities. */
#define COMMAND_ABILITY_FIRST 0x14
#define COMMAND_ABILITY_END 0x27
#define GF_ABILITY_FIRST 0x53
#define GF_ABILITY_END 0x5C

extern u8 D_80082C10;
extern CharacterData g_characters[];
extern BattleGfStats D_80078D38[16]; /**< One per GF, inside g_battleChars. */

extern s32 getXpToNextLevel(u32 exp, s32 charIdx);
extern s32 findCharXpLevel(u32 exp, s32 charIdx);
extern s32 calcHpFromLevel(s32 level, s32 charIdx);
extern s32 func_80021C10(s32 level, s32 charIdx, s32 kind);
extern s32 getElemResistance(s32 charIdx, s32 shiftBit);
extern s32 getStatusResistance(s32 charIdx, s32 shiftBit);
extern s32 calcHitStat(s32 charIdx);
extern s32 calcEvaStat(s32 charIdx, s32 hit);
extern s32 getAtkElemBase(s32 charIdx);
extern s32 getAtkElemBonus(s32 charIdx);
extern s32 decodeAtkStatusMask(s32 charIdx);
extern s32 getAtkStatusFlags(s32 charIdx);
extern s32 calcAtkStatusHit(s32 charIdx);
extern s32 getAbilityModifier(s32 charIdx, s32 a1);
extern s32 findAbilityLevel(s32 a0, s32 a1);
extern s32 evalStatCurve(s32 a0, s32 a1);
/** gf_curve.c defines it with a fallback argument too; func_80022E08 calls it with one, as the original does. */
extern s32 func_80021B58(s32 charIdx);

static void initCommandSlot(BattleCharData *bc, s32 index, s32 cmdType);
static s32 getStatusImmunityFlags(u32 a0);
static s32 hasCommandType6(BattleCharData *charData);
static s32 getMagicAvailFlags(BattleCharData *charData);
static void applyPartyAbilityFlags(s32 charIdx);
static s32 func_80022CDC(s32 cmd);
static void clearCharSlotData(BattleCharData *charData);

/**
 * @brief Refresh the flags and kernel bytes of a party member's battle magic list.
 *
 * For each of the 32 magic slots of @c g_battleChars.chars[slot], the flag byte
 * becomes 1 when bit 0x80 of the spell's kernel attack flags is set, and gets
 * bit 2 when the spell is junctioned to one of the character's stats. The
 * spell's target info and status window flags are copied from the kernel.
 *
 * @param slot Party slot (0-2).
 */
void func_800229FC(s32 slot) {
    BattleCharData *bc = &g_battleChars.chars[slot];
    s32 i;

    for (i = 0; i < 32; i++) {
        bc->magicSlots[i].unk4 = 0;
        if (g_kernel.magic[bc->magicSlots[i].unk0].attackFlags & ATTACK_FLAG_TARGET_KO) {
            bc->magicSlots[i].unk4 = MENU_ENTRY_TARGETS_KO;
        }
        if (hasJunctionedAbility(slot, bc->magicSlots[i].unk0)) {
            bc->magicSlots[i].unk4 |= MENU_ENTRY_JUNCTIONED;
        }
        bc->magicSlots[i].unk3 = g_kernel.magic[bc->magicSlots[i].unk0].targetInfo;
        bc->magicSlots[i].unk2 = g_kernel.magic[bc->magicSlots[i].unk0].statusWindowFlags;
    }
}


/**
 * @brief Fill in one of a party member's battle command slots.
 *
 * @param bc Battle character data.
 * @param index Command slot index.
 * @param cmdType Battle command ID; the slot's menu flags and target info come from g_kernel.
 */
static void initCommandSlot(BattleCharData *bc, s32 index, s32 cmdType) {
    bc->cmdSlots[index].cmdType = cmdType;
    bc->cmdSlots[index].unk1 = g_kernel.battleCommands[bc->cmdSlots[index].cmdType].menuFlags;
    bc->cmdSlots[index].unk3 = 0;
    bc->cmdSlots[index].unk2 = g_kernel.battleCommands[bc->cmdSlots[index].cmdType].targetInfo;
}


/**
 * @brief Accumulate status immunity flags as an RGB-packed bitmask from a character's equipped abilities.
 * @param a0 Character slot index into g_gameState.chars[].
 * @return Packed bitmask: (B << 16) | (G << 8) | R, OR'd across up to 4 matching ability slots.
 * @note Abilities in range 0x3A..0x4D are status immunity abilities; each has R/G/B
 *       immunity bytes looked up from g_kernel.characterAbilities[].
 */
static s32 getStatusImmunityFlags(u32 a0) {
    s32 result = 0;
    s32 i = 0;
    do {
        s32 val = g_gameState.chars[a0].abilities[i];
        u32 idx = val - 0x3A;
        if (idx < 0x14) {
            u8 b = g_kernel.characterAbilities[idx].extraField;
            u8 g = g_kernel.characterAbilities[idx].bonusField;
            u8 r = g_kernel.characterAbilities[idx].typeField;
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
static s32 hasCommandType6(BattleCharData *charData) {
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
static s32 getMagicAvailFlags(BattleCharData *charData) {
    s32 val = charData->statusFlags;
    s32 masked = val & 0x20000;
    s32 flag = masked != 0;
    if (hasCommandType6(charData)) {
        if (D_80082C10 & BATTLE_CMDS_OFF_6) {
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
 *       in g_kernel.partyAbilities[] and OR'd into g_battleChars party ability flags
 *       at offset 0x6D8. Likely enables field/world abilities (encounter-none, rare-item).
 */
static void applyPartyAbilityFlags(s32 charIdx) {
    s32 i;
    for (i = 0; i < 4; i++) {
        u8 ability = g_gameState.chars[charIdx].abilities[i];
        if ((u32)(ability - 0x4E) < 5) {
            g_battleChars.levelEntries[15].abilityFlags |= g_kernel.partyAbilities[ability - 0x4E].typeField;
        }
    }
}


/**
 * @brief Test whether the battle's command flags turn off command @p cmd.
 *
 * The flag byte D_80082C10 (g_battleConfig.unk8) has one bit per group of
 * commands: 0x02 for command 2, 0x04 for 3, 0x08 for 6, 0x01 for 4 and 13,
 * and 0x10 for every other command except 0, which is never turned off.
 *
 * @param cmd Battle command ID.
 * @return 1 if the command is turned off, 0 otherwise.
 */
static s32 func_80022CDC(s32 cmd) {
    switch (cmd) {
    case 2:
        if (D_80082C10 & BATTLE_CMDS_OFF_2) {
            return 1;
        }
        break;
    case 3:
        if (D_80082C10 & BATTLE_CMDS_OFF_3) {
            return 1;
        }
        break;
    case 6:
        if (D_80082C10 & BATTLE_CMDS_OFF_6) {
            return 1;
        }
        break;
    case 4:
    case 13:
        if (D_80082C10 & BATTLE_CMDS_OFF_4_13) {
            return 1;
        }
        break;
    case 0:
        break;
    default:
        if (D_80082C10 & BATTLE_CMDS_OFF_OTHER) {
            return 1;
        }
        break;
    }
    return 0;
}


/**
 * @brief Clear a character's magic, item, and command slot data.
 * @param a0 Pointer to a character data structure.
 * @note Zeroes out three arrays: 32 entries of 5 bytes at offset 0x82 (magic inventory),
 *       16 entries of 5 bytes at offset 0x122 (item inventory), 4 entries of 4 bytes at
 *       offset 0x1E (command slots), plus fields at 0x1C, 0x1D, and a u16 at 0x14.
 */
static void clearCharSlotData(BattleCharData *charData) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        charData->magicSlots[i].unk2 = 0;
        charData->magicSlots[i].unk3 = 0;
        charData->magicSlots[i].unk4 = 0;
        charData->magicSlots[i].unk1 = 0;
        charData->magicSlots[i].unk0 = 0;
    }

    for (i = 0; i < 0x10; i++) {
        charData->itemSlots[i].unk2 = 0;
        charData->itemSlots[i].unk3 = 0;
        charData->itemSlots[i].unk4 = 0;
        charData->itemSlots[i].unk1 = 0;
        charData->itemSlots[i].unk0 = 0;
    }

    for (i = 0; i < 4; i++) {
        charData->cmdSlots[i].unk2 = 0;
        charData->cmdSlots[i].unk3 = 0;
        charData->cmdSlots[i].unk1 = 0;
        charData->cmdSlots[i].cmdType = 0;
    }

    charData->unk1C = 0;
    charData->unk1D = 0;
    charData->unk14 = 0;
}


/**
 * @brief Set up a party member's battle data from their save data.
 *
 * Copies the character's ID, HP, experience, level and statuses into
 * @c g_battleChars.chars[slot], then builds its battle menus: the junctioned
 * GFs (one at 0 HP is unavailable), the three junctioned commands with their
 * kernel bytes and flags, the first command (command 1, or 0x0C when bit 0 of
 * the ability flags is set) and the limit break. An empty slot (@p charIdx
 * 0xFF) only gets its character ID set to 0xFF.
 *
 * @param charIdx Character ID (0-7) into g_gameState.chars[], or 0xFF if empty.
 * @param slot Party slot (0-2) into g_battleChars.chars[].
 */
void func_80022E08(s32 charIdx, s32 slot) {
    CharacterData *cd = &g_gameState.chars[charIdx];
    BattleCharData *bc = &g_battleChars.chars[slot];
    u16 gfBits;
    s32 i;
    s32 j;
    s32 data;

    bc->characterId = cd->characterId;
    if (charIdx == 0xFF) {
        bc->characterId = 0xFF;
        return;
    }
    bc->unk172 = cd->currentHp;
    bc->exp = cd->experience;
    bc->xpToNext = getXpToNextLevel(cd->experience, charIdx);
    bc->level = findCharXpLevel(cd->experience, charIdx);
    bc->unk1B9 = cd->alternateModel;
    bc->classId = func_80021B58(charIdx);
    bc->displayStatus = cd->statusFlags;
    bc->unk188 = 0;
    bc->statusFlags = getStatusImmunityFlags(charIdx);
    applyPartyAbilityFlags(charIdx);
    clearCharSlotData(bc);

    j = 0;
    gfBits = cd->junctedGfs;
    for (i = 0; i < 16; i++) {
        if (gfBits & 1) {
            bc->itemSlots[j].unk0 = BATTLE_GF_ID_BASE + i;
            bc->itemSlots[j].unk1 = 1;
            bc->itemSlots[j].unk4 = 0;
            if (g_gameState.gfs[i].hp == 0) {
                bc->itemSlots[j].unk4 = MENU_ENTRY_UNAVAILABLE;
            }
            bc->itemSlots[j].unk3 = g_kernel.junctionableGfs[i].targetInfo;
            bc->itemSlots[j].unk2 = g_kernel.junctionableGfs[i].statusWindowFlags;
            j++;
        }
        /* The do/while adds a loop level to gfBits's uses: without it the
         * register allocator swaps gfBits and the kernel entry pointer ($a1/$a2). */
        do {
            gfBits >>= 1;
        } while (0);
    }

    for (i = 1, j = 0; i < 4; i++, j++) {
        if (cd->commands[j] >= COMMAND_ABILITY_FIRST && cd->commands[j] < COMMAND_ABILITY_END) {
            bc->cmdSlots[i].cmdType = g_kernel.commandAbilities[cd->commands[j] - COMMAND_ABILITY_FIRST].typeField;
            bc->cmdSlots[i].unk1 = g_kernel.battleCommands[bc->cmdSlots[i].cmdType].menuFlags;
            bc->cmdSlots[i].unk3 = 0;
            bc->cmdSlots[i].unk2 = g_kernel.battleCommands[bc->cmdSlots[i].cmdType].targetInfo;
            if (bc->cmdSlots[i].cmdType == 0xD) {
                bc->cmdSlots[i].unk3 |= MENU_ENTRY_UNK08;
            }
            data = g_kernel.battleCommands[bc->cmdSlots[i].cmdType].abilityDataId;
            if (data != 0xFF && (g_kernel.commandAbilityData[data].attackFlags & ATTACK_FLAG_TARGET_KO)) {
                bc->cmdSlots[i].unk3 |= MENU_ENTRY_TARGETS_KO;
            }
            if (func_80022CDC(bc->cmdSlots[i].cmdType)) {
                bc->cmdSlots[i].unk3 |= MENU_ENTRY_UNAVAILABLE;
            }
        }
    }

    i = 1;
    if (bc->statusFlags & ABILITY_FIRST_CMD_0C) {
        i = 0xC;
    }
    initCommandSlot(bc, 0, i);

    bc->limitSlot.cmdType = g_kernel.characters[bc->characterId].limitBreakId;
    bc->limitSlot.unk2 = g_kernel.battleCommands[bc->limitSlot.cmdType].targetInfo;
    bc->limitSlot.unk1 = g_kernel.battleCommands[bc->limitSlot.cmdType].menuFlags;
    bc->limitSlot.unk3 = 0;

    for (i = 0; i < 9; i++) {
        bc->statCoefs[i] = getAbilityModifier(charIdx, i);
    }
    bc->fieldStatusByte = getMagicAvailFlags(bc);
}


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
        bc->magicSlots[i].unk0 = cd->magic[i].magicId;
        bc->magicSlots[i].unk1 = cd->magic[i].quantity;
    }

    func_800229FC(battleSlot);

    bc->xpToNext = getXpToNextLevel(cd->experience, charIdx);
    bc->level = findCharXpLevel(cd->experience, charIdx);

    hp = clampToMaxHp(bc->statCoefs[0] * calcHpFromLevel(bc->level, charIdx) / 100);
    bc->hpRegenCap = hp;
    if ((s16)hp < (s32)cd->currentHp) {
        cd->currentHp = hp;
        bc->unk172 = hp;
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
        bc->elemResistances[i] = getElemResistance(charIdx, i);
    }

    bc->abilityFlags = decodeAtkStatusMask(charIdx);
    bc->abilityValue = getAtkStatusFlags(charIdx);
    bc->atkStatusHit = calcAtkStatusHit(charIdx);

    for (i = 0; i < 13; i++) {
        bc->statusResistances[i] = getStatusResistance(charIdx, i);
    }

    if (bc->unk188 & 0x60000) {
        s32 idx;
        if (findCommandSlot((u8 *)bc, 2) == 0xFF) return;
        idx = findCommandSlot((u8 *)bc, 2);
        bc->cmdSlots[idx].unk3 |= 0x10;
    } else {
        s32 idx;
        if (findCommandSlot((u8 *)bc, 2) == 0xFF) return;
        idx = findCommandSlot((u8 *)bc, 2);
        bc->cmdSlots[idx].unk3 &= ~0x10;
    }
}


/**
 * @brief Recalculate one GF's battle stats from its save data.
 *
 * Sets the GF's level from its experience, then goes through the GF
 * abilities it has learned (IDs 0x53 to 0x5B, in the third and fourth words of
 * the learned-ability bits): each ORs its kernel type bits into the flags and
 * adds its value to unk9 or to the HP percentage. Max HP is the level's curve
 * value times that percentage, capped at 9999. The saved HP is clamped to it
 * and copied in, and flag 0x80 marks HP below a quarter of max.
 *
 * @param gfIdx GF index (0-15).
 */
void func_8002363C(s32 gfIdx) {
    BattleGfStats *gs = &D_80078D38[gfIdx];
    GfSaveData *save = &g_gameState.gfs[gfIdx];
    u32 bits;
    s32 ability;
    s32 k;
    s32 b;
    s32 hp;

    gs->hpPercent = 100;
    gs->flags = 0;
    gs->unk9 = 0;
    gs->level = findAbilityLevel(save->exp, gfIdx);
    gs->exp = save->exp;
    ability = 2 * 32;
    for (k = 2; k < 4; k++) {
        bits = save->completeAbilities[k];
        for (b = 0; b < 32; b++) {
            if ((bits & 1) && ability >= GF_ABILITY_FIRST && ability < GF_ABILITY_END) {
                gs->flags |= g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].typeField;
                if (g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].bonusField == 0) {
                    gs->unk9 += g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].extraField;
                }
                if (g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].bonusField == 1) {
                    gs->hpPercent += g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].extraField;
                }
            }
            bits >>= 1;
            ability++;
        }
    }
    gs->maxHp = clampToMaxHp(gs->hpPercent * evalStatCurve(gs->level, gfIdx) / 100);
    if (gs->maxHp < save->hp) {
        save->hp = gs->maxHp;
    }
    gs->hp = save->hp;
    hp = gs->hp;
    if (hp < gs->maxHp >> 2) {
        gs->flags |= GF_STATS_LOW_HP;
    }
}


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

    g_battleChars.levelEntries[15].abilityFlags = 0;

    for (i = 0; i < 3; i++) {
        func_80022E08(g_gameState.mainData.party.party[i], i);
        func_800231E0(g_gameState.mainData.party.party[i], i);
    }

    recalcAllGfStats();
}


