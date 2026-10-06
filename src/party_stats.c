#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "gf.h"
#include "gf_curve.h"
#include "party_stats.h"
#include "game.h"

// First and one-past-last ID of each ability table in g_kernel
#define COMMAND_ABILITY_FIRST 0x14
#define COMMAND_ABILITY_END 0x27
#define CHARACTER_ABILITY_FIRST 0x3A
#define CHARACTER_ABILITY_END 0x4E
#define PARTY_ABILITY_FIRST 0x4E
#define PARTY_ABILITY_END 0x53
#define GF_ABILITY_FIRST 0x53
#define GF_ABILITY_END 0x5C

// Values of a GF ability's bonusField
#define GF_ABILITY_BONUS_SUMMAG 0
#define GF_ABILITY_BONUS_GFHP 1
#define GF_ABILITY_BONUS_NONE 0xFF

static void initCommandSlot(BattleCharData *bc, s32 index, s32 cmdType);
static s32 getCharAbilityFlags(s32 charIdx);
static s32 hasDrawCommand(BattleCharData *charData);
static s32 getFieldStatusFlags(BattleCharData *charData);
static void applyPartyAbilityFlags(s32 charIdx);
static s32 isCommandOff(s32 cmd);
static void clearCharSlotData(BattleCharData *charData);

/**
 * @brief Refresh the flags and kernel data of a party member's battle magic list.
 *
 * @param slot Party slot (0-2).
 */
void func_800229FC(s32 slot) {
    BattleCharData *bc = &g_battleChars.chars[slot];
    s32 i;

    for (i = 0; i < 32; i++) {
        bc->magicSlots[i].flags = 0;
        if (g_kernel.magic[bc->magicSlots[i].id].attackFlags & ATTACK_FLAG_TARGET_KO) {
            bc->magicSlots[i].flags = MENU_ENTRY_TARGETS_KO;
        }
        if (hasJunctionedAbility(slot, bc->magicSlots[i].id)) {
            bc->magicSlots[i].flags |= MENU_ENTRY_JUNCTIONED;
        }
        bc->magicSlots[i].targetInfo = g_kernel.magic[bc->magicSlots[i].id].targetInfo;
        bc->magicSlots[i].statusWindowFlags = g_kernel.magic[bc->magicSlots[i].id].statusWindowFlags;
    }
}


/**
 * @brief Fill in one of a party member's battle command slots.
 *
 * @param bc The party member's battle data.
 * @param index Command slot (0-3).
 * @param cmdType Battle command ID.
 */
static void initCommandSlot(BattleCharData *bc, s32 index, s32 cmdType) {
    bc->cmdSlots[index].cmdType = cmdType;
    bc->cmdSlots[index].menuFlags = g_kernel.battleCommands[bc->cmdSlots[index].cmdType].menuFlags;
    bc->cmdSlots[index].flags = 0;
    bc->cmdSlots[index].targetInfo = g_kernel.battleCommands[bc->cmdSlots[index].cmdType].targetInfo;
}


/**
 * @brief Combine the flags of the character abilities a character has equipped.
 *
 * @param charIdx Character ID (0-7).
 * @return CHAR_ABILITY_* bits.
 */
static s32 getCharAbilityFlags(s32 charIdx) {
    s32 result = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        s32 val = g_gameState.chars[charIdx].abilities[i];
        u32 idx = val - CHARACTER_ABILITY_FIRST;
        if (idx < CHARACTER_ABILITY_END - CHARACTER_ABILITY_FIRST) {
            u8 high = g_kernel.characterAbilities[idx].extraField;
            u8 mid = g_kernel.characterAbilities[idx].bonusField;
            u8 low = g_kernel.characterAbilities[idx].typeField;
            result |= (high << 16) | (mid << 8) | low;
        }
    }
    return result;
}


/**
 * @brief Check whether a party member has the Draw command.
 *
 * @param charData The party member's battle data.
 * @return 1 if one of their command slots holds Draw, 0 otherwise.
 */
static s32 hasDrawCommand(BattleCharData *charData) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (charData->cmdSlots[i].cmdType == BATTLE_CMD_DRAW) return 1;
    }
    return 0;
}


/**
 * @brief Work out a party member's fieldStatusByte.
 *
 * @param charData The party member's battle data.
 * @return FIELD_STATUS_* bits.
 */
static s32 getFieldStatusFlags(BattleCharData *charData) {
    s32 val = charData->statusFlags;
    s32 masked = val & CHAR_ABILITY_MOVE_HP_UP;
    s32 flag = masked ? FIELD_STATUS_MOVE_HP_UP : 0;
    if (hasDrawCommand(charData)) {
        if (g_battleConfig.unk8 & BATTLE_CMDS_OFF_DRAW) {
            return flag;
        }
        flag |= FIELD_STATUS_DRAW;
    }
    return flag;
}


/**
 * @brief Add the flags of a character's equipped party abilities to the party's ability flags.
 *
 * @param charIdx Character ID (0-7).
 */
static void applyPartyAbilityFlags(s32 charIdx) {
    s32 i;
    for (i = 0; i < 4; i++) {
        u8 ability = g_gameState.chars[charIdx].abilities[i];
        u32 idx = ability - PARTY_ABILITY_FIRST;
        if (idx < PARTY_ABILITY_END - PARTY_ABILITY_FIRST) {
            g_battleChars.abilityFlags |= g_kernel.partyAbilities[idx].typeField;
        }
    }
}


/**
 * @brief Check whether a battle command is turned off.
 *
 * @param cmd Battle command ID.
 * @return 1 if it's turned off, 0 otherwise.
 */
static s32 isCommandOff(s32 cmd) {
    switch (cmd) {
    case BATTLE_CMD_MAGIC:
        if (g_battleConfig.unk8 & BATTLE_CMDS_OFF_MAGIC) {
            return 1;
        }
        break;
    case BATTLE_CMD_GF:
        if (g_battleConfig.unk8 & BATTLE_CMDS_OFF_GF) {
            return 1;
        }
        break;
    case BATTLE_CMD_DRAW:
        if (g_battleConfig.unk8 & BATTLE_CMDS_OFF_DRAW) {
            return 1;
        }
        break;
    case BATTLE_CMD_ITEM:
    case BATTLE_CMD_UNK13:
        if (g_battleConfig.unk8 & BATTLE_CMDS_OFF_ITEM) {
            return 1;
        }
        break;
    case BATTLE_CMD_NONE:
        break;
    default:
        if (g_battleConfig.unk8 & BATTLE_CMDS_OFF_OTHER) {
            return 1;
        }
        break;
    }
    return 0;
}


/**
 * @brief Clear a party member's battle magic list, GF list and command slots.
 *
 * @param charData The party member's battle data.
 */
static void clearCharSlotData(BattleCharData *charData) {
    s32 i;

    for (i = 0; i < 32; i++) {
        charData->magicSlots[i].statusWindowFlags = 0;
        charData->magicSlots[i].targetInfo = 0;
        charData->magicSlots[i].flags = 0;
        charData->magicSlots[i].count = 0;
        charData->magicSlots[i].id = 0;
    }

    for (i = 0; i < 16; i++) {
        charData->gfSlots[i].statusWindowFlags = 0;
        charData->gfSlots[i].targetInfo = 0;
        charData->gfSlots[i].flags = 0;
        charData->gfSlots[i].count = 0;
        charData->gfSlots[i].id = 0;
    }

    for (i = 0; i < 4; i++) {
        charData->cmdSlots[i].targetInfo = 0;
        charData->cmdSlots[i].flags = 0;
        charData->cmdSlots[i].menuFlags = 0;
        charData->cmdSlots[i].cmdType = 0;
    }

    charData->unk1C = 0;
    charData->unk1D = 0;
    charData->unk14 = 0;
}


/**
 * @brief Set up a party member's battle data and battle menus from their save data.
 *
 * @param charIdx Character ID (0-7), or PARTY_SLOT_EMPTY.
 * @param slot Party slot (0-2).
 */
void func_80022E08(s32 charIdx, s32 slot) {
    CharacterData *cd = &g_gameState.chars[charIdx];
    BattleCharData *bc = &g_battleChars.chars[slot];
    u16 gfBits;
    s32 i;
    s32 j;
    s32 data;

    bc->characterId = cd->characterId;
    if (charIdx == PARTY_SLOT_EMPTY) {
        bc->characterId = PARTY_SLOT_EMPTY;
        return;
    }
    bc->unk172 = cd->currentHp;
    bc->exp = cd->experience;
    bc->xpToNext = getXpToNextLevel(cd->experience, charIdx);
    bc->level = findCharXpLevel(cd->experience, charIdx);
    bc->unk1B9 = cd->alternateModel;
    // The original passes no fallback, so func_80021B58 reads whatever is left in a1
    bc->classId = ((s32 (*)())func_80021B58)(charIdx);
    bc->displayStatus = cd->statusFlags;
    bc->unk188 = 0;
    bc->statusFlags = getCharAbilityFlags(charIdx);
    applyPartyAbilityFlags(charIdx);
    clearCharSlotData(bc);

    j = 0;
    gfBits = cd->junctedGfs;
    for (i = 0; i < GF_COUNT; i++) {
        if (gfBits & 1) {
            bc->gfSlots[j].id = BATTLE_GF_ID_BASE + i;
            bc->gfSlots[j].count = 1;
            bc->gfSlots[j].flags = 0;
            if (g_gameState.gfs[i].hp == 0) {
                bc->gfSlots[j].flags = MENU_ENTRY_UNAVAILABLE;
            }
            bc->gfSlots[j].targetInfo = g_kernel.junctionableGfs[i].targetInfo;
            bc->gfSlots[j].statusWindowFlags = g_kernel.junctionableGfs[i].statusWindowFlags;
            j++;
        }
        // Without the do/while the register allocator swaps gfBits and the kernel entry pointer
        do {
            gfBits >>= 1;
        } while (0);
    }

    for (i = 1, j = 0; i < 4; i++, j++) {
        if (cd->commands[j] >= COMMAND_ABILITY_FIRST && cd->commands[j] < COMMAND_ABILITY_END) {
            bc->cmdSlots[i].cmdType = g_kernel.commandAbilities[cd->commands[j] - COMMAND_ABILITY_FIRST].typeField;
            bc->cmdSlots[i].menuFlags = g_kernel.battleCommands[bc->cmdSlots[i].cmdType].menuFlags;
            bc->cmdSlots[i].flags = 0;
            bc->cmdSlots[i].targetInfo = g_kernel.battleCommands[bc->cmdSlots[i].cmdType].targetInfo;
            if (bc->cmdSlots[i].cmdType == BATTLE_CMD_UNK13) {
                bc->cmdSlots[i].flags |= CMD_SLOT_UNK08;
            }
            data = g_kernel.battleCommands[bc->cmdSlots[i].cmdType].abilityDataId;
            if (data != 0xFF && (g_kernel.commandAbilityData[data].attackFlags & ATTACK_FLAG_TARGET_KO)) {
                bc->cmdSlots[i].flags |= MENU_ENTRY_TARGETS_KO;
            }
            if (isCommandOff(bc->cmdSlots[i].cmdType)) {
                bc->cmdSlots[i].flags |= MENU_ENTRY_UNAVAILABLE;
            }
        }
    }

    i = BATTLE_CMD_ATTACK;
    if (bc->statusFlags & CHAR_ABILITY_MUG) {
        i = BATTLE_CMD_MUG;
    }
    initCommandSlot(bc, 0, i);

    bc->limitCmdSlot.cmdType = g_kernel.characters[bc->characterId].limitBreakId;
    bc->limitCmdSlot.targetInfo = g_kernel.battleCommands[bc->limitCmdSlot.cmdType].targetInfo;
    bc->limitCmdSlot.menuFlags = g_kernel.battleCommands[bc->limitCmdSlot.cmdType].menuFlags;
    bc->limitCmdSlot.flags = 0;

    for (i = 0; i < 9; i++) {
        bc->statCoefs[i] = getAbilityModifier(charIdx, i);
    }
    bc->fieldStatusByte = getFieldStatusFlags(bc);
}


/**
 * @brief Find which of a party member's command slots holds a command.
 *
 * @param bc The party member's battle data.
 * @param cmdType Battle command ID.
 * @return The slot (0-3), or 0xFF if none holds it.
 */
s32 findCommandSlot(BattleCharData *bc, s32 cmdType) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (bc->cmdSlots[i].cmdType == cmdType) return i;
    }
    return 0xFF;
}


/**
 * @brief Cap a value at 255.
 *
 * @param value Value to cap.
 * @return @p value, or 255 if it's larger.
 */
s32 clampToByte(s32 value) {
    if (value >= 0x100) {
        return 0xFF;
    }
    return value;
}


/**
 * @brief Cap a value at 9999.
 *
 * @param value Value to cap.
 * @return @p value truncated to 16 bits, or 9999 if it's larger.
 */
s32 clampToMaxHp(s32 value) {
    if (value >= 10000) {
        return 9999;
    }
    return (u16)value;
}


/**
 * @brief Recalculate a party member's battle stats from their save data.
 *
 * @param charIdx Character ID (0-7), or PARTY_SLOT_EMPTY.
 * @param battleSlot Party slot (0-2).
 */
void func_800231E0(s32 charIdx, s32 battleSlot) {
    CharacterData *cd = &g_gameState.chars[charIdx];
    BattleCharData *bc = &g_battleChars.chars[battleSlot];
    s32 i;
    s32 hp;

    if (charIdx == PARTY_SLOT_EMPTY) return;

    for (i = 0; i < 32; i++) {
        bc->magicSlots[i].id = cd->magic[i].magicId;
        bc->magicSlots[i].count = cd->magic[i].quantity;
    }

    func_800229FC(battleSlot);

    bc->xpToNext = getXpToNextLevel(cd->experience, charIdx);
    bc->level = findCharXpLevel(cd->experience, charIdx);

    hp = clampToMaxHp(bc->statCoefs[JUNCTION_HP] * calcHpFromLevel(bc->level, charIdx) / 100);
    bc->hpRegenCap = hp;
    if (bc->hpRegenCap < cd->currentHp) {
        cd->currentHp = hp;
        bc->unk172 = hp;
    }

    bc->stats[0] = clampToByte(bc->statCoefs[JUNCTION_STR] * func_80021C10(bc->level, charIdx, JUNCTION_STR) / 100);
    bc->stats[1] = clampToByte(bc->statCoefs[JUNCTION_VIT] * func_80021C10(bc->level, charIdx, JUNCTION_VIT) / 100);
    bc->stats[2] = clampToByte(bc->statCoefs[JUNCTION_MAG] * func_80021C10(bc->level, charIdx, JUNCTION_MAG) / 100);
    bc->stats[3] = clampToByte(bc->statCoefs[JUNCTION_SPR] * func_80021C10(bc->level, charIdx, JUNCTION_SPR) / 100);
    bc->stats[4] = clampToByte(bc->statCoefs[JUNCTION_SPD] * func_80021C10(bc->level, charIdx, JUNCTION_SPD) / 100);
    bc->stats[5] = clampToByte(bc->statCoefs[JUNCTION_LCK] * func_80021C10(bc->level, charIdx, JUNCTION_LCK) / 100);
    // The original passes calcHitStat one argument, so the call can't go through its prototype
    bc->stats[7] = clampToByte(bc->statCoefs[JUNCTION_HIT] * ((s32 (*)())calcHitStat)(charIdx) / 100);
    bc->stats[6] = clampToByte(bc->statCoefs[JUNCTION_EVA] * calcEvaStat(charIdx, bc->stats[4]) / 100);

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

    if (bc->unk188 & (BATTLE_STATUS_DOUBLE | BATTLE_STATUS_TRIPLE)) {
        s32 idx;
        if (findCommandSlot(bc, BATTLE_CMD_MAGIC) == 0xFF) return;
        idx = findCommandSlot(bc, BATTLE_CMD_MAGIC);
        bc->cmdSlots[idx].flags |= CMD_SLOT_MULTICAST;
    } else {
        s32 idx;
        if (findCommandSlot(bc, BATTLE_CMD_MAGIC) == 0xFF) return;
        idx = findCommandSlot(bc, BATTLE_CMD_MAGIC);
        bc->cmdSlots[idx].flags &= ~CMD_SLOT_MULTICAST;
    }
}


/**
 * @brief Recalculate a GF's battle stats from its save data.
 *
 * @param gfIdx GF index (0-15).
 */
void func_8002363C(s32 gfIdx) {
    BattleLevelEntry *gs = &g_battleChars.levelEntries[gfIdx];
    GfSaveData *save = &g_gameState.gfs[gfIdx];
    u32 bits;
    s32 ability;
    s32 k;
    s32 b;
    s32 hp;

    gs->hpPercent = 100;
    gs->flags = 0;
    gs->sumMagBonus = 0;
    gs->level = findAbilityLevel(save->exp, gfIdx);
    gs->exp = save->exp;
    ability = 2 * 32;
    for (k = 2; k < 4; k++) {
        bits = save->completeAbilities[k];
        for (b = 0; b < 32; b++) {
            if ((bits & 1) && ability >= GF_ABILITY_FIRST && ability < GF_ABILITY_END) {
                gs->flags |= g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].typeField;
                if (g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].bonusField == GF_ABILITY_BONUS_SUMMAG) {
                    gs->sumMagBonus += g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].extraField;
                }
                if (g_kernel.gfAbilities[ability - GF_ABILITY_FIRST].bonusField == GF_ABILITY_BONUS_GFHP) {
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
 * @brief Recalculate the battle stats of every GF the player owns.
 */
void recalcAllGfStats(void) {
    s32 i;
    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            func_8002363C(i);
        }
    }
}


/**
 * @brief Recalculate the battle data of the three party members and of every GF the player owns.
 */
void recalcPartyStats(void) {
    s32 i;

    g_battleChars.abilityFlags = 0;

    for (i = 0; i < 3; i++) {
        func_80022E08(g_gameState.mainData.party.partyMembers[i], i);
        func_800231E0(g_gameState.mainData.party.partyMembers[i], i);
    }

    recalcAllGfStats();
}
