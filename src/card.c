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
 * @brief Initialize 128 ability slots to empty.
 *
 * Sets each 2-byte entry: byte 0 (state) = 0, byte 1 (index) = 0xFF.
 *
 * @param ptr Pointer to the ability slot buffer (256 bytes).
 */
void initAbilitySlots(u8 *ptr) {
    s32 i = 0;
    do {
        i++;
        ptr[1] = 0xFF;
        ptr[0] = 0;
        ptr += 2;
    } while (i < 128);
}


/**
 * @brief Mark learned abilities in an ability slot buffer.
 *
 * Scans the 128-bit ability bitmask for the given GF. For each
 * set bit, writes 2 (learned) to the corresponding slot.
 * Stops after 22 entries.
 *
 * @param index GF index.
 * @param dest  Ability slot buffer (128 × 2-byte entries).
 * @param count Starting count of filled entries.
 * @return Updated count of filled entries.
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
 * @brief Populate ability slots from a GF's learnable abilities.
 *
 * Checks each of the 21 abilities against the GF's level, learned mask,
 * and existing hand entries. Adds unlearned, level-eligible abilities.
 *
 * @param gfIndex GF index.
 * @param dest    Ability slot buffer (128 × 2-byte slots).
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
 * @brief Populate ability slots from chained GF abilities.
 *
 * Handles abilities with levelReq >= 101 (chained prerequisites).
 * For each chained ability, checks if the prerequisite ability is
 * already learned (state 2), and optionally checks a second
 * prerequisite. Adds eligible abilities to the buffer.
 *
 * @param gfIndex GF index.
 * @param dest    Ability slot buffer (128 × 2-byte slots).
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
s32 getAbilityCategory(s32 cardId) {
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


/**
 * @brief Build a list of available GF abilities for the junction menu.
 *
 * Populates a slot buffer with learned, level-eligible, and chained abilities,
 * then iterates the filled slots to build output entries with ability index,
 * category, data lookup value from g_gfData, and current ability level from
 * the GF's save data. Returns the total number of available abilities.
 *
 * @param gfIndex          GF index.
 * @param output           Output array of CardHandEntry structs (or NULL to just count).
 * @param includeJunction  If nonzero, includes level-eligible and chained abilities.
 * @return Total number of available abilities.
 * @see https://decomp.me/scratch/bpAxD
 */
INCLUDE_ASM("asm/nonmatchings/card", func_800369CC);


/**
 * @brief Reset a character's junctions and recalculate stats.
 *
 * Clears junctioned GFs, commands, abilities, and junction slots for the
 * given character. Temporarily sets the character as party leader to trigger
 * stat recalculation, clears status (preserving bit 7), sets HP from
 * D_80078894, then restores the original party slot.
 *
 * @param charIndex Character index (clamped to 0-7).
 */
void func_80036B90(s32 charIndex) {
    extern CharacterData g_characters[];
    extern u16 D_80078894;
    CharacterData *chr;
    s32 i;
    u8 savedSlot;
    s32 clamped;

    if (charIndex < 0)
        goto clamp_zero;
    clamped = 7;
    if (charIndex < 8)
        clamped = charIndex;
    goto clamped_done;
clamp_zero:
    clamped = 0;
clamped_done:
    charIndex = clamped;
    chr = &g_characters[charIndex];
    chr->junctedGfs = 0;

    for (i = 0; i < 4; i++) {
        chr->commands[i] = 0;
        chr->abilities[i] = 0;
    }

    {
        u8 *p = (u8 *)chr + 19;
        for (i = 19; i >= 0; i--) {
            p[0x5C] = 0;
            p--;
        }
    }

    savedSlot = g_gameState.party.party[0];
    g_gameState.party.party[0] = charIndex;
    recalcPartyStats();

    do {
        chr->statusFlags &= 0x80;
    } while (0);

    chr->currentHp = D_80078894;

    g_gameState.party.party[0] = savedSlot;
    recalcPartyStats();
}


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


