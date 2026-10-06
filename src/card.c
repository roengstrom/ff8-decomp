#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "overlay.h"
#include "gamestate.h"
#include "ability_list.h"
#include "character.h"
#include "card.h"
#include "kernel.h"

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
    JunctionableGfEntry *learnData = &g_kernel.junctionableGfs[gfIndex];
    u32 learnedMask;
    s32 level;

    learnedMask = *(u32 *)&g_gameState.gfs[gfIndex].learning; /* learning + forgotten1-3 packed */
    level = g_battleChars.levelEntries[gfIndex].level;
    learnedMask >>= 8;

    for (i = 0; i < 21; i++) {
        u8 reqLevel = learnData->abilities[i].levelReq;
        u8 slot = learnData->abilities[i].abilityId;

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
    JunctionableGfEntry *learnData;
    s32 i;

    learnedMask = *(u32 *)&g_gameState.gfs[gfIndex].learning; /* learning + forgotten packed */
    learnData = &g_kernel.junctionableGfs[gfIndex];
    learnedMask >>= 8;

    for (i = 0; i < 21; i++) {
        s32 levelReq = learnData->abilities[i].levelReq;
        u8 prereq = learnData->abilities[i].prereq;
        u8 slot = learnData->abilities[i].abilityId;
        u8 reqSlot;
        u8 reqState;

        if (levelReq == 0xFF) continue;
        levelReq -= 101;
        if ((u8)levelReq >= 21) continue;
        if (count >= 22) return count;

        reqState = levelReq;
        reqState = learnData->abilities[reqState].abilityId;
        reqSlot = reqState;
        if (prereq != 0xFF) {
            prereq = learnData->abilities[prereq].abilityId;
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
 * @brief Map an ability slot index to its category (0-6).
 *
 * Divides the slot index range into 7 tiers used for looking up
 * ability data in D_80053C3C:
 * 0-19 = category 0, 20-38 = category 1, 39-57 = category 2,
 * 58-77 = category 3, 78-82 = category 4, 83-91 = category 5,
 * 92+ = category 6.
 *
 * @param slotIndex Ability slot index (0-127).
 * @return Category (0-6).
 */
s32 getAbilityCategory(s32 slotIndex) {
    s32 result;
    if (slotIndex < 20) {
        result = 0;
    } else if (slotIndex < 39) {
        result = 1;
    } else if (slotIndex < 58) {
        result = 2;
    } else if (slotIndex < 78) {
        result = 3;
    } else if (slotIndex < 83) {
        result = 4;
    } else if (slotIndex < 92) {
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
 * category, data lookup value from g_kernel, and current ability level from
 * the GF's save data. Returns the total number of available abilities.
 *
 * @param gfIndex          GF index (0-15).
 * @param output           Output array of AbilityListEntry structs (or NULL to just count).
 * @param includeJunction  If nonzero, includes level-eligible and chained abilities.
 * @return Total number of available abilities.
 */
s32 func_800369CC(s32 gfIndex, AbilityListEntry *output, s32 includeJunction) {
    AbilitySlot slots[128];
    s32 totalCount;
    s32 slotIndex;

    initAbilitySlots((u8 *)slots);
    totalCount = func_80036710(gfIndex, (u8 *)slots, 0);
    if (includeJunction) {
        totalCount = func_8003678C(gfIndex, (u8 *)slots, totalCount);
        totalCount = func_8003685C(gfIndex, (u8 *)slots, totalCount);
    }

    if (output != 0) {
        s32 j;
        slotIndex = 0;
        for (j = slotIndex; j < totalCount; j++) {
            while (slots[slotIndex].type == 0) {
                slotIndex++;
            }

            output->slotIndex = slotIndex;
            output->type = slots[slotIndex].type;
            output->abilityIndex = slots[slotIndex].abilityIndex;
            output->category = getAbilityCategory(slotIndex);

            if (output->abilityIndex < 0xFF) {
                AbilityCategoryInfo *info = &D_80053C3C[output->category];
                u8 *entry = (u8*)&g_kernel;
                entry = (u8 *)(info->dataOffset + (s32)entry);
                entry += info->stride * (slotIndex - info->startIndex);
                output->gfDataValue = entry[4];
                output->gsValue = g_gameState.gfs[gfIndex].aps[output->abilityIndex];
            }

            output++;
            slotIndex++;
        }
    }

    return totalCount;
}


/**
 * @brief Reset a character's junctions and recalculate stats.
 *
 * Clears junctioned GFs, commands, abilities, and junction slots for the
 * given character. Temporarily sets the character as party leader to trigger
 * stat recalculation, clears status (preserving bit 7), sets HP from
 * g_battleChars.chars[0].hpRegenCap, then restores the original party slot.
 *
 * @param charIndex Character index (clamped to 0-7).
 */
void func_80036B90(s32 charIndex) {
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
    chr = &g_gameState.chars[charIndex];
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

    savedSlot = g_gameState.mainData.party.partyMembers[0];
    g_gameState.mainData.party.partyMembers[0] = charIndex;
    recalcPartyStats();

    do {
        chr->statusFlags &= 0x80;
    } while (0);

    chr->currentHp = g_battleChars.chars[0].hpRegenCap;

    g_gameState.mainData.party.partyMembers[0] = savedSlot;
    recalcPartyStats();
}


/**
 * @brief Swap the first two non-zero party members.
 *
 * Returns early if any slot is empty (0xFF). Finds the first and second
 * non-zero party members and swaps their positions.
 */
void func_80036C74(void) {
    s32 i;
    s32 second;
    s32 first;
    u8 tmp;

    for (i = 0; i < 3; i++) {
        if (g_gameState.mainData.party.partyMembers[i] == 0xFF) {
            return;
        }
    }

    for (i = 0; i < 3; i++) {
        if (g_gameState.mainData.party.partyMembers[i] != 0) {
            first = i;
            break;
        }
    }

    for (i = 0; i < 3; i++) {
        if ((g_gameState.mainData.party.partyMembers[i] != 0) && (first != i)) {
            second = i;
            break;
        }
    }

    tmp = g_gameState.mainData.party.partyMembers[second];
    g_gameState.mainData.party.partyMembers[second] = g_gameState.mainData.party.partyMembers[first];
    g_gameState.mainData.party.partyMembers[first] = tmp;
}


/**
 * @brief Rebuild party slots based on a bitmask of eligible characters.
 *
 * Clears and rebuilds the party slot assignments. For each non-empty,
 * non-Squall party member, checks if they have the required flag (bit 3
 * of exists). Eligible members whose bit is set in @p mask stay;
 * others are removed. Copies the result to g_gameState.battleParty and recalculates.
 *
 * @param mask Bitmask of characters allowed to remain in the party.
 */
void func_80036D44(s32 mask) {
    u8 newSlots[3];
    u8 val;
    u8 *p;
    s32 new_var;
    s32 i;
    s32 slotCount;
    int new_var2;
    s32 bit;
    s32 abilityId;
    CharacterData *chr;
    new_var2 = 0xFF;

    val = new_var2;
    i = 2;
    p = &newSlots[i];
    for (i = 2; i >= 0; i--) {
        *(p--) = val;
    }

    abilityId = 9;
    slotCount = 1;
    bit = slotCount;
    g_gameState.chars[0].characterId = 8;
    newSlots[0] = 8;
    for (i = 0; i < 3; i++) {
        u8 slot = g_gameState.mainData.party.partyMembers[i];
        if (slot == 0xFF) {
            continue;
        }
        if (0 == slot) {
            continue;
        }
        chr = &g_gameState.chars[slot];
        if (chr->exists & 0x8) {
            chr->characterId = abilityId;
            new_var = mask & (1 << (abilityId - 8));
            if (new_var) {
                newSlots[slotCount] = abilityId;
                slotCount++;
            } else {
                g_gameState.mainData.party.partyMembers[i] = 0xFF;
            }
            abilityId++;
        } else {
            g_gameState.mainData.party.partyMembers[i] = 0xFF;
        }
    }

    {
        u8 *p = g_gameState.battleParty;
        u8 val = new_var2;
        for (i = 2; i >= 0; i--) {
            *(p++) = val;
        }
    }
    {
        u8 *dst = g_gameState.battleParty;
        u8 *src = (u8 *)((u32)newSlots + (u32)dst - (u32)dst);
        for (i = 0; i < 3; i++) {
            *dst++ = *src++;
        }
    }
    func_80036C74();
    recalcPartyStats();
}


/**
 * @brief Set the active party leader and clear the other two party slots.
 *
 * Sets party slot 0 to the given character, empties slots 1 and 2,
 * then calls recalcPartyStats to apply the change.
 *
 * @param charId Character ID for the party leader.
 */
void setPartyLeader(s32 charId) {
    g_gameState.mainData.party.partyMembers[0] = charId;
    g_gameState.mainData.party.partyMembers[1] = 0xFF;
    g_gameState.mainData.party.partyMembers[2] = 0xFF;
    recalcPartyStats();
}


/**
 * @brief Build a bitmask of available characters, optionally filtered by party.
 *
 * If @ref PARTY_LOCK_LOCKED is set, only characters currently in the party
 * are eligible. Otherwise all existing characters are included.
 *
 * @return Bitmask where bit N is set if character N is available.
 */
s32 func_80036EC0(void) {
    s32 i;
    u16 charMask;
    u16 partyMask;

    partyMask = 0xFFFF;
    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        partyMask = 0;
        for (i = 0; i < 3; i++) {
            u8 slot = g_gameState.mainData.party.partyMembers[i];
            if (slot != 0xFF) {
                partyMask |= (1 << slot);
            }
        }
    }

    charMask = 0;
    for (i = 0; i < 8; i++) {
        if (g_gameState.chars[i].exists & 1) {
            do { charMask |= 1 << i; } while (0);
        }
    }
    charMask = charMask & partyMask;

    return charMask;
}


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
 * @brief Set a GF's saved HP to its max HP.
 *
 * @param gfIdx GF index (0-15).
 */
void copyGfHpToSave(s32 gfIdx) {
    g_gameState.gfs[gfIdx].hp = g_battleChars.levelEntries[gfIdx].maxHp;
}


/**
 * @brief Set a character as party leader, reset their HP and status,
 *        then restore original party slots.
 *
 * Saves the current party, clears it, sets the leader to trigger
 * stat recalculation, then writes g_battleChars.chars[0].hpRegenCap to the character's
 * currentHp and clears all status flags except bit 7. Finally
 * restores the saved party and recalculates stats.
 *
 * @param charIdx Character index.
 */
void func_80036FE0(s32 charIdx) {
    u8 saved[4];
    s32 i;

    for (i = 0; i < 3; i++) {
        saved[i] = g_gameState.mainData.party.partyMembers[i];
        g_gameState.mainData.party.partyMembers[i] = 0xFF;
    }

    setPartyLeader(charIdx);

    g_gameState.chars[charIdx].currentHp = g_battleChars.chars[0].hpRegenCap;
    g_gameState.chars[charIdx].statusFlags &= 0x80;

    for (i = 0; i < 3; i++) {
        g_gameState.mainData.party.partyMembers[i] = saved[i];
    }

    recalcPartyStats();
}


