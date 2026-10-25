#include "common.h"
#include "ability.h"
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

static void initCommandSlot(BattleCharData *bc, s32 index, s32 commandId);
static s32 getCharAbilityFlags(s32 charIdx);
static s32 hasDrawCommand(BattleCharData *charData);
static s32 getFieldAbilities(BattleCharData *charData);
static void applyPartyAbilityFlags(s32 charIdx);
static s32 isCommandOff(s32 cmd);
static void clearCharSlotData(BattleCharData *charData);


void func_800229FC(s32 slot) {
    BattleCharData *bc = &g_battleChars.chars[slot];
    s32 i;

    for (i = 0; i < 32; i++) {
        bc->magicArgs[i].flags = 0;
        if (g_kernel.magic[bc->magicArgs[i].id].attackFlags & ATTACK_FLAG_TARGET_KO) {
            bc->magicArgs[i].flags = BTL_CMD_ARG_TARGETS_KO;
        }
	// Should this be called isMagicJunctioned()?
        if (hasJunctionedAbility(slot, bc->magicArgs[i].id)) {
            bc->magicArgs[i].flags |= BTL_CMD_ARG_JUNCTIONED;
        }
        bc->magicArgs[i].targetInfo = g_kernel.magic[bc->magicArgs[i].id].targetInfo;
        bc->magicArgs[i].statusWindowFlags = g_kernel.magic[bc->magicArgs[i].id].statusWindowFlags;
    }
}


static void initCommandSlot(BattleCharData *bc, s32 index, s32 commandId) {
    bc->abilityCommands[index].commandId = commandId;
    bc->abilityCommands[index].subMenuFlags = g_kernel.battleCommands[bc->abilityCommands[index].commandId].subMenuFlags;
    bc->abilityCommands[index].flags = 0;
    bc->abilityCommands[index].targetInfo = g_kernel.battleCommands[bc->abilityCommands[index].commandId].targetInfo;
}


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


static s32 hasDrawCommand(BattleCharData *charData) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (charData->abilityCommands[i].commandId == BATTLE_CMD_DRAW) return 1;
    }
    return 0;
}


static s32 getFieldAbilities(BattleCharData *charData) {
    s32 val = charData->characterAbilities;
    s32 masked = val & CHAR_ABILITY_MOVE_HP_UP;
    s32 flag = masked ? FIELD_ABILITY_MOVE_HP_UP : 0;
    if (hasDrawCommand(charData)) {
        if (g_battleConfig.sealedAbilities & SEALED_DRAW) {
            return flag;
        }
        flag |= FIELD_ABILITY_DRAW;
    }
    return flag;
}


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


static s32 isCommandOff(s32 cmd) {
    switch (cmd) {
    case BATTLE_CMD_MAGIC:
        if (g_battleConfig.sealedAbilities & SEALED_MAGIC) {
            return 1;
        }
        break;
    case BATTLE_CMD_GF:
        if (g_battleConfig.sealedAbilities & SEALED_GF) {
            return 1;
        }
        break;
    case BATTLE_CMD_DRAW:
        if (g_battleConfig.sealedAbilities & SEALED_DRAW) {
            return 1;
        }
        break;
    case BATTLE_CMD_ITEM:
    case BATTLE_CMD_UNK13:
        if (g_battleConfig.sealedAbilities & SEALED_ITEM) {
            return 1;
        }
        break;
    case BATTLE_CMD_NONE:
        break;
    default:
        if (g_battleConfig.sealedAbilities & SEALED_COMMAND_ABILITIES) {
            return 1;
        }
        break;
    }
    return 0;
}


static void clearCharSlotData(BattleCharData *charData) {
    s32 i;

    for (i = 0; i < 32; i++) {
        charData->magicArgs[i].statusWindowFlags = 0;
        charData->magicArgs[i].targetInfo = 0;
        charData->magicArgs[i].flags = 0;
        charData->magicArgs[i].count = 0;
        charData->magicArgs[i].id = 0;
    }

    for (i = 0; i < 16; i++) {
        charData->gfArgs[i].statusWindowFlags = 0;
        charData->gfArgs[i].targetInfo = 0;
        charData->gfArgs[i].flags = 0;
        charData->gfArgs[i].count = 0;
        charData->gfArgs[i].id = 0;
    }

    for (i = 0; i < 4; i++) {
        charData->abilityCommands[i].targetInfo = 0;
        charData->abilityCommands[i].flags = 0;
        charData->abilityCommands[i].subMenuFlags = 0;
        charData->abilityCommands[i].commandId = 0;
    }

    charData->summoningCountDownInProgress = 0;
    charData->summoningGfId = 0;
    charData->summoningTimeLeft = 0;
}


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
    bc->currentHp = cd->currentHp;
    bc->currentExp = cd->experience;
    bc->expToNextLevel = getXpToNextLevel(cd->experience, charIdx);
    bc->currentLevel = findCharXpLevel(cd->experience, charIdx);
    bc->modelVariant = cd->modelVariant;
    bc->weaponId = func_80021B58(charIdx);
    bc->persistentStatus = cd->statusFlags;
    bc->battleStatus = 0;
    bc->characterAbilities = getCharAbilityFlags(charIdx);
    applyPartyAbilityFlags(charIdx);
    clearCharSlotData(bc);

    j = 0;
    gfBits = cd->junctedGfs;
    for (i = 0; i < GF_COUNT; i++) {
        if (gfBits & 1) {
            bc->gfArgs[j].id = BATTLE_GF_ID_BASE + i;
            bc->gfArgs[j].count = 1;
            bc->gfArgs[j].flags = 0;
            if (g_gameState.gfs[i].hp == 0) {
                bc->gfArgs[j].flags = BTL_CMD_ARG_UNAVAILABLE;
            }
            bc->gfArgs[j].targetInfo = g_kernel.junctionableGfs[i].targetInfo;
            bc->gfArgs[j].statusWindowFlags = g_kernel.junctionableGfs[i].statusWindowFlags;
            j++;
        }
        // Without the do/while the register allocator swaps gfBits and the kernel entry pointer
        do {
            gfBits >>= 1;
        } while (0);
    }

    for (i = 1, j = 0; i < 4; i++, j++) {
        if (cd->commands[j] >= COMMAND_ABILITY_FIRST && cd->commands[j] < COMMAND_ABILITY_END) {
            bc->abilityCommands[i].commandId = g_kernel.commandAbilities[cd->commands[j] - COMMAND_ABILITY_FIRST].typeField;
            bc->abilityCommands[i].subMenuFlags = g_kernel.battleCommands[bc->abilityCommands[i].commandId].subMenuFlags;
            bc->abilityCommands[i].flags = 0;
            bc->abilityCommands[i].targetInfo = g_kernel.battleCommands[bc->abilityCommands[i].commandId].targetInfo;
            if (bc->abilityCommands[i].commandId == BATTLE_CMD_UNK13) {
                bc->abilityCommands[i].flags |= BTL_CMD_SINGLE_OR_DOUBLE;
            }
            data = g_kernel.battleCommands[bc->abilityCommands[i].commandId].abilityDataId;
            if (data != 0xFF && (g_kernel.commandAbilityData[data].attackFlags & ATTACK_FLAG_TARGET_KO)) {
                bc->abilityCommands[i].flags |= BTL_CMD_TARGETS_KO;
            }
            if (isCommandOff(bc->abilityCommands[i].commandId)) {
                bc->abilityCommands[i].flags |= BTL_CMD_UNAVAILABLE;
            }
        }
    }

    i = BATTLE_CMD_ATTACK;
    if (bc->characterAbilities & CHAR_ABILITY_MUG) {
        i = BATTLE_CMD_MUG;
    }
    initCommandSlot(bc, 0, i);

    bc->limitBreakCommand.commandId = g_kernel.characters[bc->characterId].limitBreakId;
    bc->limitBreakCommand.targetInfo = g_kernel.battleCommands[bc->limitBreakCommand.commandId].targetInfo;
    bc->limitBreakCommand.subMenuFlags = g_kernel.battleCommands[bc->limitBreakCommand.commandId].subMenuFlags;
    bc->limitBreakCommand.flags = 0;

    for (i = 0; i < 9; i++) {
        bc->statMultipliers[i] = getStatMultiplier(charIdx, i);
    }
    bc->fieldAbilities = getFieldAbilities(bc);
}


s32 findCommandSlot(BattleCharData *bc, s32 commandId) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (bc->abilityCommands[i].commandId == commandId) return i;
    }
    return 0xFF;
}


s32 clampToByte(s32 value) {
    if (value >= 0x100) {
        return 0xFF;
    }
    return value;
}


s32 clampToMaxHp(s32 value) {
    if (value >= 10000) {
        return 9999;
    }
    return (u16)value;
}


void func_800231E0(s32 charIdx, s32 battleSlot) {
    CharacterData *cd = &g_gameState.chars[charIdx];
    BattleCharData *bc = &g_battleChars.chars[battleSlot];
    s32 i;
    s32 hp;

    if (charIdx == PARTY_SLOT_EMPTY) return;

    for (i = 0; i < 32; i++) {
        bc->magicArgs[i].id = cd->magic[i].magicId;
        bc->magicArgs[i].count = cd->magic[i].quantity;
    }

    func_800229FC(battleSlot);

    bc->expToNextLevel = getXpToNextLevel(cd->experience, charIdx);
    bc->currentLevel = findCharXpLevel(cd->experience, charIdx);

    hp = clampToMaxHp(bc->statMultipliers[JUNCTION_HP] * calcHpFromLevel(bc->currentLevel, charIdx) / 100);
    bc->maxHp = hp;
    if (bc->maxHp < cd->currentHp) {
        cd->currentHp = hp;
        bc->currentHp = hp;
    }

    bc->stats[BATTLE_STAT_STR] = clampToByte(bc->statMultipliers[JUNCTION_STR] * func_80021C10(bc->currentLevel, charIdx, JUNCTION_STR) / 100);
    bc->stats[BATTLE_STAT_VIT] = clampToByte(bc->statMultipliers[JUNCTION_VIT] * func_80021C10(bc->currentLevel, charIdx, JUNCTION_VIT) / 100);
    bc->stats[BATTLE_STAT_MAG] = clampToByte(bc->statMultipliers[JUNCTION_MAG] * func_80021C10(bc->currentLevel, charIdx, JUNCTION_MAG) / 100);
    bc->stats[BATTLE_STAT_SPR] = clampToByte(bc->statMultipliers[JUNCTION_SPR] * func_80021C10(bc->currentLevel, charIdx, JUNCTION_SPR) / 100);
    bc->stats[BATTLE_STAT_SPD] = clampToByte(bc->statMultipliers[JUNCTION_SPD] * func_80021C10(bc->currentLevel, charIdx, JUNCTION_SPD) / 100);
    bc->stats[BATTLE_STAT_LCK] = clampToByte(bc->statMultipliers[JUNCTION_LCK] * func_80021C10(bc->currentLevel, charIdx, JUNCTION_LCK) / 100);
    bc->stats[BATTLE_STAT_HIT] = clampToByte(bc->statMultipliers[JUNCTION_HIT] * calcHitStat(charIdx) / 100);
    bc->stats[BATTLE_STAT_EVA] = clampToByte(bc->statMultipliers[JUNCTION_EVA] * calcEvaStat(charIdx, bc->stats[BATTLE_STAT_SPD]) / 100);

    bc->atkElements = getAtkElements(charIdx);
    bc->atkElementPercent = getAtkElementPercent(charIdx);

    for (i = 0; i < 8; i++) {
        bc->elemResistances[i] = getElemResistance(charIdx, i);
    }

    bc->atkBattleStatus = decodeAtkStatusMask(charIdx);
    bc->atkPersistentStatus = getAtkStatusFlags(charIdx);
    bc->atkStatusHitChance = calcAtkStatusHit(charIdx);

    for (i = 0; i < 13; i++) {
        bc->statusResistances[i] = getStatusResistance(charIdx, i);
    }

    if (bc->battleStatus & (BATTLE_STATUS_DOUBLE | BATTLE_STATUS_TRIPLE)) {
        s32 idx;
        if (findCommandSlot(bc, BATTLE_CMD_MAGIC) == 0xFF) return;
        idx = findCommandSlot(bc, BATTLE_CMD_MAGIC);
        bc->abilityCommands[idx].flags |= BTL_CMD_MULTICAST;
    } else {
        s32 idx;
        if (findCommandSlot(bc, BATTLE_CMD_MAGIC) == 0xFF) return;
        idx = findCommandSlot(bc, BATTLE_CMD_MAGIC);
        bc->abilityCommands[idx].flags &= ~BTL_CMD_MULTICAST;
    }
}


void func_8002363C(s32 gfIdx) {
    BattleGfEntry *gs = &g_battleChars.gfs[gfIdx];
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


void recalcAllGfStats(void) {
    s32 i;
    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            func_8002363C(i);
        }
    }
}


void recalcPartyStats(void) {
    s32 i;

    g_battleChars.abilityFlags = 0;

    for (i = 0; i < 3; i++) {
        func_80022E08(g_gameState.mainData.party.partyMembers[i], i);
        func_800231E0(g_gameState.mainData.party.partyMembers[i], i);
    }

    recalcAllGfStats();
}
