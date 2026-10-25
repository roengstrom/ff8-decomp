#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "overlay.h"
#include "gamestate.h"
#include "ability_list.h"
#include "character.h"
#include "card.h"
#include "kernel.h"

static void initAbilitySlots(u8 *ptr);
static s32 func_80036710(s32 index, u8 *dest, s32 count);
static s32 func_8003678C(s32 gfIndex, u8 *dest, s32 count);
static s32 func_8003685C(s32 gfIndex, u8 *dest, s32 count);

/**
 * @brief Initialize 128 ability slots to empty.
 *
 * Sets each 2-byte entry: byte 0 (state) = 0, byte 1 (index) = 0xFF.
 *
 * @param ptr Pointer to the ability slot buffer (256 bytes).
 */
static void initAbilitySlots(u8 *ptr) {
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
static s32 func_80036710(s32 index, u8 *dest, s32 count) {
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
static s32 func_8003678C(s32 gfIndex, u8 *dest, s32 count) {
    s32 i;
    JunctionableGfEntry *learnData = &g_kernel.junctionableGfs[gfIndex];
    u32 learnedMask;
    s32 level;

    learnedMask = *(u32 *)&g_gameState.gfs[gfIndex].learning; /* learning + forgotten1-3 packed */
    level = g_battleChars.gfs[gfIndex].level;
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
static s32 func_8003685C(s32 gfIndex, u8 *dest, s32 count) {
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
