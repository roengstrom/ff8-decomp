#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gf.h"
#include "gamestate.h"
#include "ability.h"

/**
 * @brief Look up an ability's AP cap from the kernel data.
 * @param abilityId Ability ID (0-120, see AbilityId enum).
 * @return The AP cap value for that ability.
 */
s32 GetAbilityCap(s32 abilityId) {
    if (abilityId < ABILITY_MAGIC) {
        return g_gfData.abilityRangeI[abilityId].cap;
    }
    if ((u32)(abilityId - ABILITY_MAGIC) < 19) {
        return g_gfData.abilityRangeJ[abilityId - ABILITY_MAGIC].cap;
    }
    if ((u32)(abilityId - ABILITY_HP_20) < 19) {
        return g_gfData.abilityRangeK[abilityId - ABILITY_HP_20].cap;
    }
    if ((u32)(abilityId - ABILITY_MUG) < 20) {
        return g_gfData.abilityRangeL[abilityId - ABILITY_MUG].cap;
    }
    if ((u32)(abilityId - ABILITY_ALERT) < 5) {
        return g_gfData.abilityRangeM[abilityId - ABILITY_ALERT].cap;
    }
    if ((u32)(abilityId - ABILITY_SUMMAG_10) < 9) {
        return g_gfData.abilityRangeN[abilityId - ABILITY_SUMMAG_10].cap;
    }
    return g_gfData.abilityRangeO[abilityId - ABILITY_HAGGLE].cap;
}


/**
 * @brief Search a GF's ability slot table for a given ability ID.
 * @param gfIdx GF index (0-15).
 * @param abilityId The ability ID to search for.
 * @return Slot index (0-20) if found, 0xFF if not found.
 */
s32 FindAbilitySlot(s32 gfIdx, s32 abilityId) {
    s32 i = 0;

    for (i = 0; i < GF_ABILITY_SLOT_COUNT; i++)
    {
        if (g_gfData.abilityTable132[gfIdx].abilities[i].abilityId == abilityId) {
            return i;
        }
    }

    return 0xFF;
}

/**
 * @brief Add AP to a GF's currently-learning ability, clamping at 255.
 * @param gfIdx GF index (0-15).
 * @param amount AP to add.
 * @return The ability ID if AP reached the cap, 0 otherwise.
 */
s32 AddAbilityExp(s32 gfIdx, s32 amount) {
    GfSaveData *gf = &g_gameState.gfs[gfIdx];
    s32 slot;
    s32 newVal;
    s32 cap;

    if (gf->learning == 0) {
        return 0;
    }

    slot = FindAbilitySlot(gfIdx, gf->learning);
    if (slot == 0xFF) {
        return 0;
    }
    {
        newVal = gf->aps[slot] + (u16)amount;
        cap = GetAbilityCap(gf->learning);

        if (newVal >= 256) {
            gf->aps[slot] = 0xFF;
        } else {
            gf->aps[slot] = newVal;
        }

        if (newVal < cap) {
            return 0;
        }
    }
    return gf->learning;
}
