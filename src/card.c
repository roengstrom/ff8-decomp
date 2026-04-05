#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "overlay.h"
#include "gamestate.h"
#include "character.h"

/** @brief GF ability learn requirement (4 bytes). */
typedef struct {
    u8 levelReq;        /* 0x00: level required, or index for chained abilities (101+) */
    u8 prereq;          /* 0x01: prerequisite ability index (0xFF = none) */
    u8 slot;            /* 0x02: ability slot index */
    u8 pad03;
} GfAbilityEntry;

/** @brief GF learnable ability table (stride 0x84). */
typedef struct {
    u8 pad00[0x1C];
    GfAbilityEntry abilities[21];
    u8 pad70[0x14];
} GfLearnData;

extern GfLearnData D_80079D78[];  /* g_gfData + 0xF78: GF learn tables */

/**
 * @brief Initialize 128 card hand slots to empty.
 *
 * Sets each 2-byte slot: byte 0 (card ID) = 0, byte 1 (card type) = 0xFF.
 *
 * @param ptr Pointer to the card hand array (256 bytes).
 */
void initCardHandSlots(u8 *ptr) {
    s32 i = 0;
    do {
        i++;
        ptr[1] = 0xFF;
        ptr[0] = 0;
        ptr += 2;
    } while (i < 128);
}


/**
 * @brief Populate a card hand from GF learned abilities.
 *
 * Scans the 128-bit ability bitmask for the given GF. For each
 * set bit, writes 2 to the corresponding hand slot (marking it as
 * learned). Stops after 22 entries.
 *
 * @param index GF index.
 * @param dest  Card hand buffer (128 × 2-byte slots).
 * @param count Starting count of filled slots.
 * @return Updated count of filled slots.
 */
s32 func_80036710(s32 index, u8 *dest, s32 count) {
    u32 *bitmask = g_gameState.gfs[index].completeAbilities;
    s32 i;

    for (i = 0; i < 128; i++) {
        if (bitmask[i / 32] & (1 << (i & 0x1F))) {
            if (count >= 22) {
                return count;
            }
            *dest = 2;
            count++;
        }
        dest += 2;
    }
    return count;
}


/**
 * @brief Populate card hand slots from a GF's learnable abilities.
 *
 * Checks each of the 21 abilities against the GF's level, learned mask,
 * and existing hand entries. Adds unlearned, level-eligible abilities.
 *
 * @param gfIndex GF index.
 * @param dest    Card hand buffer (128 × 2-byte slots).
 * @param count   Starting count of filled slots.
 * @return Updated count of filled slots.
 */
s32 func_8003678C(s32 gfIndex, u8 *dest, s32 count) {
    s32 i;
    GfLearnData *learnData = &D_80079D78[gfIndex];
    u32 learnedMask;
    s32 level;

    learnedMask = *(u32 *)&g_gameState.gfs[gfIndex].learning; /* learning + forgotten1-3 packed */
    level = g_battleChars.levelEntries[gfIndex].level;
    learnedMask >>= 8;

    for (i = 0; i < 21; i++) {
        u8 reqLevel = learnData->abilities[i].levelReq;
        u8 slot = learnData->abilities[i].slot;

        if (reqLevel == 0xFF) continue;
        if (reqLevel >= 101) continue;
        if (count >= 22) return count;
        if (level < reqLevel) continue;
        if (dest[slot * 2] != 0) continue;
        if (learnedMask & (1 << i)) continue;

        count++;
        dest[slot * 2] = 1;
        dest[slot * 2 + 1] = i;
    }
    return count;
}


/**
 * @brief Populate card hand from chained GF abilities.
 *
 * Handles abilities with levelReq >= 101 (chained prerequisites).
 * For each chained ability, checks if the prerequisite ability is
 * already learned (state 2 in hand), and optionally checks a second
 * prerequisite. Adds eligible abilities to the hand buffer.
 *
 * @param gfIndex GF index.
 * @param dest    Card hand buffer (128 × 2-byte slots).
 * @param count   Starting count of filled slots.
 * @return Updated count of filled slots.
 */
s32 func_8003685C(s32 gfIndex, u8 *dest, s32 count) {
    u32 learnedMask;
    GfLearnData *learnData;
    s32 i;

    learnedMask = *(u32 *)&g_gameState.gfs[gfIndex].learning; /* learning + forgotten packed */
    learnData = &D_80079D78[gfIndex];
    learnedMask >>= 8;

    for (i = 0; i < 21; i++) {
        s32 levelReq = learnData->abilities[i].levelReq;
        u8 prereq = learnData->abilities[i].prereq;
        u8 slot = learnData->abilities[i].slot;
        u8 reqSlot;
        u8 reqState;

        if (levelReq == 0xFF) continue;
        levelReq -= 101;
        if ((u8)levelReq >= 21) continue;
        if (count >= 22) return count;

        reqState = levelReq;
        reqState = learnData->abilities[(u8)reqState].slot;
        reqSlot = reqState;
        if (prereq != 0xFF) {
            prereq = learnData->abilities[prereq].slot;
        }

        reqState = dest[reqSlot * 2];
        if (reqState != 2) continue;
        if (prereq != 0xFF && dest[prereq * 2] == reqState) continue;

        if (dest[slot * 2] != 0) continue;
        if (learnedMask & (1 << i)) continue;

        count++;
        dest[slot * 2] = 1;
        dest[slot * 2 + 1] = i;
    }
    return count;
}


/**
 * @brief Map a card ID to its rarity level (0-6).
 *
 * Divides the card ID range into 7 tiers:
 * 0-19 = level 0, 20-38 = level 1, 39-57 = level 2,
 * 58-77 = level 3, 78-82 = level 4, 83-91 = level 5, 92+ = level 6.
 *
 * @param cardId Card ID.
 * @return Rarity level (0-6).
 */
s32 getCardRarity(s32 cardId) {
    s32 result;
    if (cardId < 20) {
        result = 0;
    } else if (cardId < 39) {
        result = 1;
    } else if (cardId < 58) {
        result = 2;
    } else if (cardId < 78) {
        result = 3;
    } else if (cardId < 83) {
        result = 4;
    } else if (cardId < 92) {
        result = 5;
    } else {
        result = 6;
    }
    return result;
}


INCLUDE_ASM("asm/nonmatchings/card", func_800369CC);


INCLUDE_ASM("asm/nonmatchings/card", func_80036B90);


INCLUDE_ASM("asm/nonmatchings/card", func_80036C74);


INCLUDE_ASM("asm/nonmatchings/card", func_80036D44);


/**
 * @brief Set the active party leader and clear the other two party slots.
 *
 * Sets party slot 0 to the given character, empties slots 1 and 2,
 * then calls recalcPartyStats to apply the change.
 *
 * @param charId Character ID for the party leader.
 */
void setPartyLeader(s32 charId) {
    g_gameState.party.party[0] = charId;
    g_gameState.party.party[1] = 0xFF;
    g_gameState.party.party[2] = 0xFF;
    recalcPartyStats();
}


INCLUDE_ASM("asm/nonmatchings/card", func_80036EC0);


/**
 * @brief Build a 16-bit bitmask of GF availability flags.
 *
 * Iterates over 16 GFs, checking the exists flag. Sets the corresponding
 * bit in the result mask for each available GF.
 *
 * @return Bitmask where bit N is set if GF N is available.
 */
u16 getGfAvailabilityMask(void) {
    u16 mask = 0;
    s32 i;

    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].exists & 1) {
            mask |= (1 << i);
        }
    }
    return mask;
}


/**
 * @brief Copy a GF's current HP from the battle character table to the save data.
 *
 * @param gfIdx GF index (0-15).
 */
void copyGfHpToSave(s32 gfIdx) {
    g_gameState.gfs[gfIdx].hp = g_battleChars.gfEntries[gfIdx].hp;
}


/**
 * @brief Set a character as party leader, reset their HP and status,
 *        then restore original party slots.
 *
 * Saves the current party, clears it, sets the leader to trigger
 * stat recalculation, then writes D_80078894 to the character's
 * currentHp and clears all status flags except bit 7. Finally
 * restores the saved party and recalculates stats.
 *
 * @param charIdx Character index.
 */
void func_80036FE0(s32 charIdx) {
    extern u16 D_80078894;
    u8 saved[4];
    s32 i;

    for (i = 0; i < 3; i++) {
        saved[i] = g_gameState.party.party[i];
        g_gameState.party.party[i] = 0xFF;
    }

    setPartyLeader(charIdx);

    g_gameState.chars[charIdx].currentHp = D_80078894;
    g_gameState.chars[charIdx].statusFlags &= 0x80;

    for (i = 0; i < 3; i++) {
        g_gameState.party.party[i] = saved[i];
    }

    recalcPartyStats();
}


