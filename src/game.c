#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gf.h"
#include "gamestate.h"

u8 *resolveKernelPtr(u16 a0, s32 a1);

/** @brief Empty stub at the end of the sound engine region.
 *  Followed by ~4KB of zero padding before the game code resumes.
 */
INCLUDE_ASM("asm/nonmatchings/game", func_8001F5C8);

/**
 * @brief Game code VSync handler. Clears render mode if getRenderCompleteFlag signals completion.
 *
 * Called from the VSync dispatch (g_renderMode == 4). Invokes dispatchScratchpadThread
 * for per-frame processing, then checks getRenderCompleteFlag's return. If non-zero,
 * sets g_renderMode to 0 (RENDER_IDLE) to signal the main loop.
 */
void vsyncGameHandler(void) {
    extern s16 g_renderMode;
    void dispatchScratchpadThread(void);
    s32 getRenderCompleteFlag(void);

    dispatchScratchpadThread();
    if (getRenderCompleteFlag() != 0) {
        g_renderMode = 0;
    }
}


/**
 * @brief Zero memory in 16-byte (4-word) chunks.
 * @param ptr Pointer to memory to clear.
 * @param count Number of 16-byte iterations.
 */
void memzero16(s32 *ptr, s32 count) {
    s32 i = 0;
    if (count <= 0) return;
    do {
        *ptr = 0;
        ptr++;
        *ptr = 0;
        ptr++;
        *ptr = 0;
        ptr++;
        *ptr = 0;
        ptr++;
    } while (++i < count);
}


/**
 * @brief Copy a0 bytes from src to dst.
 * @param src Source byte pointer.
 * @param dst Destination byte pointer.
 * @param len Number of bytes to copy.
 */
void memcopy(u8 *src, u8 *dst, s32 len) {
    s32 i = 0;
    if (len <= 0) return;
    do {
        *dst = *src;
        src++;
        dst++;
    } while (++i < len);
}


/** @brief Mark a GF as existing (sets exists flag).
 *  @param a0 GF index (0-15).
 */
void setGfExists(s32 a0) {
    g_gameState.gfs[a0].exists |= 1;
}


/** @brief Returns a pointer to the Boko name field in the save header. */
u8 *getBokoName(void) {
    return g_gameState.bokoName;
}


/** @brief Returns a pointer to the Angelo name field in the save header. */
u8 *getAngeloName(void) {
    return g_gameState.angeloName;
}


/** @brief Resolves param from GfData.subTableU[a0] (stride 20) via resolveKernelPtr. */
s32 getGfSummonData(s32 a0) {
    /* subTableU (+0x4A6C), ptrSubTableU (+0xD8) */
    return resolveKernelPtr(g_gfData.subTableU[a0].param0, g_gfData.ptrSubTableU);
}


/**
 * @brief Resolve GF name pointer from GfData.subTableT[a0] (stride 8).
 * @param a0 GF index (0 returns default D_800773A8 pointer).
 * @return Pointer from resolveKernelPtr lookup, or &D_800773A8 if a0 is 0.
 */
u8 *getGfName(s32 a0) {
    u8 *result;

    if (a0 != 0) {
        /* subTableT (+0x4A5C), ptrSubTableT (+0xD4) */
        result = resolveKernelPtr(g_gfData.subTableT[a0].param0, g_gfData.ptrSubTableT);
    } else {
        result = g_gameState.angeloName;
    }
    return result;
}


/** @brief Resolves param0 from GfData.subTableS[a0] (stride 32) via resolveKernelPtr. */
s32 getMagicEffectName(s32 a0) {
    /* subTableS (+0x48B8), ptrSubTableS (+0xD0) */
    return resolveKernelPtr(g_gfData.subTableS[a0].param0, g_gfData.ptrSubTableS);
}


/** @brief Resolves param1 from GfData.subTableS[a0] (stride 32, +2) via resolveKernelPtr. */
s32 getMagicEffectDesc(s32 a0) {
    /* subTableS (+0x48BA), ptrSubTableS (+0xD0) */
    return resolveKernelPtr(g_gfData.subTableS[a0].param1, g_gfData.ptrSubTableS);
}


/** @brief Resolves param0 from GfData.subTableR[a0] (stride 24) via resolveKernelPtr. */
s32 getStatusEffectName(s32 a0) {
    /* subTableR (+0x47F8), ptrSubTableR (+0xCC) */
    return resolveKernelPtr(g_gfData.subTableR[a0].param0, g_gfData.ptrSubTableR);
}


/** @brief Resolves param1 from GfData.subTableR[a0] (stride 24, +2) via resolveKernelPtr. */
s32 getStatusEffectDesc(s32 a0) {
    /* subTableR (+0x47FA), ptrSubTableR (+0xCC) */
    return resolveKernelPtr(g_gfData.subTableR[a0].param1, g_gfData.ptrSubTableR);
}


/** @brief Resolves param0 from GfData.elementData24[a0] (stride 24) via resolveKernelPtr. */
s32 getElementName(s32 a0) {
    /* elementData24 (+0x3744), ptrElementData24 (+0x94) */
    return resolveKernelPtr(g_gfData.elementData24[a0].param0, g_gfData.ptrElementData24);
}


/** @brief Resolves param1 from GfData.elementData24[a0] (stride 24, +2) via resolveKernelPtr. */
s32 getElementDesc(s32 a0) {
    /* elementData24 (+0x3746), ptrElementData24 (+0x94) */
    return resolveKernelPtr(g_gfData.elementData24[a0].param1, g_gfData.ptrElementData24);
}


/** @brief Resolves param0 from GfData.subTableQ[a0] (stride 16) via resolveKernelPtr. */
s32 getJuncEffectName(s32 a0) {
    /* subTableQ (+0x44F8), ptrSubTableQ (+0xC8) */
    return resolveKernelPtr(g_gfData.subTableQ[a0].param0, g_gfData.ptrSubTableQ);
}


/** @brief Resolves param1 from GfData.subTableQ[a0] (stride 16, +2) via resolveKernelPtr. */
s32 getJuncEffectDesc(s32 a0) {
    /* subTableQ (+0x44FA), ptrSubTableQ (+0xC8) */
    return resolveKernelPtr(g_gfData.subTableQ[a0].param1, g_gfData.ptrSubTableQ);
}


/** @brief Resolves param0 from GfData.subTableP[a0] (stride 24) via resolveKernelPtr. */
s32 getJuncCategoryName(s32 a0) {
    /* subTableP (+0x4480), ptrSubTableP (+0xC4) */
    return resolveKernelPtr(g_gfData.subTableP[a0].param0, g_gfData.ptrSubTableP);
}


/** @brief Resolves param1 from GfData.subTableP[a0] (stride 24, +2) via resolveKernelPtr. */
s32 getJuncCategoryDesc(s32 a0) {
    /* subTableP (+0x4482), ptrSubTableP (+0xC4) */
    return resolveKernelPtr(g_gfData.subTableP[a0].param1, g_gfData.ptrSubTableP);
}


/**
 * @brief Resolve AbilityEntry.statParam0 from the appropriate ability range table.
 * @param a0 Ability ID that selects the sub-table (see AbilityEntry ranges in gf.h).
 * @return Result of resolveKernelPtr with the entry's statParam0 and the range base pointer.
 */
u8 *getAbilityName(s32 a0) {
    u16 param;
    s32 arg2;

    if (a0 < 0x14) {
        /* abilityRangeI (+0x40E0), AbilityEntry.statParam0 */
        param = g_gfData.abilityRangeI[a0].statParam0;
        arg2 = g_gfData.ptrAbilityRangeI;
    } else if ((u32)(a0 - 0x14) < 0x13) {
        s32 idx = a0 - 0x14;
        /* abilityRangeJ (+0x4180) */
        param = g_gfData.abilityRangeJ[idx].statParam0;
        arg2 = g_gfData.ptrAbilityRangeJ;
    } else if ((u32)(a0 - 0x27) < 0x13) {
        s32 idx = a0 - 0x27;
        /* abilityRangeK (+0x4218) */
        param = g_gfData.abilityRangeK[idx].statParam0;
        arg2 = g_gfData.ptrAbilityRangeK;
    } else if ((u32)(a0 - 0x3A) < 0x14) {
        s32 idx = a0 - 0x3A;
        /* abilityRangeL (+0x42B0) */
        param = g_gfData.abilityRangeL[idx].statParam0;
        arg2 = g_gfData.ptrAbilityRangeL;
    } else if ((u32)(a0 - 0x4E) < 0x5) {
        s32 idx = a0 - 0x4E;
        /* abilityRangeM (+0x4350) */
        param = g_gfData.abilityRangeM[idx].statParam0;
        arg2 = g_gfData.ptrAbilityRangeM;
    } else {
        s32 idx = a0 - 0x53;
        if ((u32)idx >= 0x9) {
            s32 base = (s32)&g_gfData;
            s32 idx2 = a0 - 0x5C;
            /* abilityRangeO (+0x43C0) */
            param = *(u16 *)(base + 0x43C0 + idx2 * 8);
            arg2 = g_gfData.ptrAbilityRangeO;
        } else {
            /* abilityRangeN (+0x4378) */
            param = g_gfData.abilityRangeN[idx].statParam0;
            arg2 = g_gfData.ptrAbilityRangeN;
        }
    }
    return resolveKernelPtr(param, arg2);
}


/**
 * @brief Resolve AbilityEntry.statParam1 from the appropriate ability range table.
 * @param a0 Ability ID that selects the sub-table (see AbilityEntry ranges in gf.h).
 * @return Result of resolveKernelPtr with the entry's statParam1 and the range base pointer.
 */
u8 *getAbilityDesc(s32 a0) {
    u16 param;
    s32 arg2;

    if (a0 < 0x14) {
        /* abilityRangeI (+0x40E2), AbilityEntry.statParam1 */
        param = g_gfData.abilityRangeI[a0].statParam1;
        arg2 = g_gfData.ptrAbilityRangeI;
    } else if ((u32)(a0 - 0x14) < 0x13) {
        s32 idx = a0 - 0x14;
        /* abilityRangeJ (+0x4182) */
        param = g_gfData.abilityRangeJ[idx].statParam1;
        arg2 = g_gfData.ptrAbilityRangeJ;
    } else if ((u32)(a0 - 0x27) < 0x13) {
        s32 idx = a0 - 0x27;
        /* abilityRangeK (+0x421A) */
        param = g_gfData.abilityRangeK[idx].statParam1;
        arg2 = g_gfData.ptrAbilityRangeK;
    } else if ((u32)(a0 - 0x3A) < 0x14) {
        s32 idx = a0 - 0x3A;
        /* abilityRangeL (+0x42B2) */
        param = g_gfData.abilityRangeL[idx].statParam1;
        arg2 = g_gfData.ptrAbilityRangeL;
    } else if ((u32)(a0 - 0x4E) < 0x5) {
        s32 idx = a0 - 0x4E;
        /* abilityRangeM (+0x4352) */
        param = g_gfData.abilityRangeM[idx].statParam1;
        arg2 = g_gfData.ptrAbilityRangeM;
    } else {
        s32 idx = a0 - 0x53;
        if ((u32)idx >= 0x9) {
            s32 base = (s32)&g_gfData;
            s32 idx2 = a0 - 0x5C;
            /* abilityRangeO (+0x43C2) */
            param = *(u16 *)(base + 0x43C0 + idx2 * 8 + 2);
            arg2 = g_gfData.ptrAbilityRangeO;
        } else {
            /* abilityRangeN (+0x437A) */
            param = g_gfData.abilityRangeN[idx].statParam1;
            arg2 = g_gfData.ptrAbilityRangeN;
        }
    }
    return resolveKernelPtr(param, arg2);
}


/**
 * @brief Get a pointer to a magic spell's name string.
 *
 * If @p a0 < 0x40, indexes into GfData.junctionData (stride 60) at offset
 * 0x21C to get a 16-bit index, then resolves it via resolveKernelPtr against
 * GfData.ptrGfSpellData (+0x84). If @p a0 >= 0x40, returns directly from
 * D_800762C8 at stride 68.
 *
 * @param a0 Magic spell ID.
 * @return Pointer to the spell's encoded name string.
 */
u8 *getMagicNamePtr(s32 a0) {
    extern u8 D_800762C8[];

    if (a0 < 0x40) {
        /* junctionData (+0x21C, stride 60), ptrGfSpellData (+0x84) */
        return (u8 *)resolveKernelPtr(g_gfData.junctionData[a0].nameParam0, g_gfData.ptrGfSpellData);
    }
    return D_800762C8 + a0 * 68;
}


/**
 * @brief Resolve a secondary data pointer for a character or GF entity.
 * @param a0 Entity index; < 0x40 uses GfJunctionEntry[a0]+2 in junctionData,
 *           >= 0x40 uses GfAbilityTableEntry[idx] in abilityTable132.
 * @return Resolved data pointer via resolveKernelPtr.
 */
s32 getSpellEntityData(s32 a0) {
    if (a0 < 0x40) {
        u8 *gfBase = (u8 *)&g_gfData;
        /* junctionData (+0x21E, stride 60), ptrGfSpellData (+0x84) */
        u16 param = *(u16 *)(gfBase + 0x21E + a0 * 60);
        s32 arg2 = *(s32 *)(gfBase + 0x84);
        return resolveKernelPtr(param, arg2);
    }
    {
        u8 *gfBase = (u8 *)&g_gfData;
        s32 idx = a0 - 0x40;
        /* abilityTable132 (+0xF7A, stride 132), ptrAbilityTable132 (+0x88) */
        u16 param = *(u16 *)(gfBase + 0xF7A + idx * 132);
        s32 arg2 = *(s32 *)(gfBase + 0x88);
        return resolveKernelPtr(param, arg2);
    }
}


/**
 * @brief Resolve stat param0 from GfData stat tables.
 * @param a0 Stat index; >= 0x21 uses statTable4 (stride 4), otherwise statTable24 (stride 24).
 * @return Result of resolveKernelPtr.
 */
s32 getStatName(s32 a0) {
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)&g_gfData;
        s32 idx = a0 - 0x21;
        /* statTable4 (+0x3C48), ptrStatTable4 (+0xA0) */
        param = *(u16 *)(base + 0x3C48 + idx * 4);
        arg2 = g_gfData.ptrStatTable4;
    } else {
        /* statTable24 (+0x3930), ptrStatTable24 (+0x9C) */
        param = g_gfData.statTable24[a0].param0;
        arg2 = g_gfData.ptrStatTable24;
    }
    return resolveKernelPtr(param, arg2);
}


/**
 * @brief Resolve stat param1 from GfData stat tables (+2 offset from func_80020CE0).
 * @param a0 Stat index; >= 0x21 uses statTable4 (stride 4), otherwise statTable24 (stride 24).
 * @return Result of resolveKernelPtr.
 */
s32 getStatDesc(s32 a0) {
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)&g_gfData;
        s32 idx = a0 - 0x21;
        /* statTable4 (+0x3C4A), ptrStatTable4 (+0xA0) */
        param = *(u16 *)(base + 0x3C48 + idx * 4 + 2);
        arg2 = g_gfData.ptrStatTable4;
    } else {
        /* statTable24 (+0x3932), ptrStatTable24 (+0x9C) */
        param = g_gfData.statTable24[a0].param1;
        arg2 = g_gfData.ptrStatTable24;
    }
    return resolveKernelPtr(param, arg2);
}


/**
 * @brief Get a battle entity's name string pointer.
 *
 * Squall and Rinoa have custom names stored in the save header.
 * All other characters use the default name from GfData lookup.
 *
 * @param entityIdx Battle entity index into g_battleChars.
 * @return Pointer to the character's name string.
 */
u8 *getBattleCharName(s32 entityIdx) {
    if (g_battleChars[entityIdx].characterId == CHAR_SQUALL) {
        return g_gameState.squallName;
    }
    if (g_battleChars[entityIdx].characterId == CHAR_RINOA) {
        return g_gameState.rinoaName;
    }
    return resolveKernelPtr(
        g_gfData.xpCurves36[g_battleChars[entityIdx].characterId].lookupParam,
        g_gfData.ptrGfCurve36);
}


/**
 * @brief Get a pointer to a character's name string.
 *
 * Special cases: characterId 0 returns g_gameState+0x18, characterId 4
 * returns g_gameState+0x24. All others index into GfData.xpCurves36
 * (stride 36) at offset 0x37A4 and resolve via resolveKernelPtr against
 * GfData.ptrGfCurve36 (+0x98).
 *
 * @param a0 Character ID (see CharacterId).
 * @return Pointer to the character's encoded name string.
 */
u8 *getCharNamePtr(CharacterId charId) {
    if (charId == CHAR_SQUALL) {
        return g_gameState.squallName;
    }
    if (charId == CHAR_RINOA) {
        return g_gameState.rinoaName;
    }
    return resolveKernelPtr(g_gfData.xpCurves36[charId].lookupParam, g_gfData.ptrGfCurve36);
}


/** @brief Resolves param from GfData.levelCurve12[a0] (stride 12) via resolveKernelPtr. */
s32 getLevelCurveData(s32 a0) {
    /* levelCurve12 (+0x35B8, stride 12), ptrLevelCurve12 (+0x90) */
    return resolveKernelPtr(g_gfData.levelCurve12[a0].param0, g_gfData.ptrLevelCurve12);
}


/** @brief Resolves AbilityEntry.statParam0 from GfData.statTable8[a0] via resolveKernelPtr. */
s32 getAbilityEntryName(s32 a0) {
    /* statTable8 (+0xE4, stride 8), ptrStatTable8 (+0x80) */
    return resolveKernelPtr(g_gfData.statTable8[a0].statParam0, g_gfData.ptrStatTable8);
}


/** @brief Resolves AbilityEntry.statParam1 from GfData.statTable8[a0] via resolveKernelPtr. */
s32 getAbilityEntryDesc(s32 a0) {
    /* statTable8 (+0xE6, stride 8), ptrStatTable8 (+0x80) */
    return resolveKernelPtr(g_gfData.statTable8[a0].statParam1, g_gfData.ptrStatTable8);
}


/** @brief Wrapper that calls func_80020F84 with argument 3. */
s32 getDefaultMenuLabel(void) {
    return getMenuString(3);
}


/**
 * @brief Look up a u16 from GfData.subTableV[a0] (stride 2) and resolve via resolveKernelPtr.
 * @param a0 Index into subTableV.
 * @return Resolved data pointer.
 */
s32 getMenuString(s32 a0) {
    /* subTableV (+0x4D08, stride 2), ptrSubTableV (+0xE0) */
    return resolveKernelPtr(g_gfData.subTableV[a0].param0, g_gfData.ptrSubTableV);
}


/**
 * @brief Resolve a data pointer for a given entity index.
 * @param a0 Entity index (0xFFFF = no entity, returns default pointer).
 * @param a1 Base offset into the data region.
 * @return Pointer into g_gfData at offset a1+a0, or D_80052898 if a0 is 0xFFFF.
 */
u8 *resolveKernelPtr(u16 a0, s32 a1) {
    extern u8 D_80052898[];
    u8 *result;
    if (a0 != 0xFFFF) {
        result = a1 + (a0 + (u8 *)&g_gfData);
    } else {
        result = D_80052898;
    }
    return result;
}


/**
 * @brief Decrement the count byte for all entries matching itemId in the
 * status array at g_gameState + 0xB44.
 * @param itemId The ID to match. If 0, the function returns immediately.
 */
void decrementItemByType(s32 itemId) {
    s32 i;

    if (itemId == 0) {
        return;
    }

    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        if (g_gameState.itemSlots[i].id == itemId) {
            g_gameState.itemSlots[i].count--;
        }
    }
}


/**
 * @brief Add an item to the inventory array D_80077EBC.
 * Searches for an existing entry with matching ID to increment count,
 * or places the item in the first empty slot. Count is capped at 100.
 * @param itemId Item identifier to add (0 = no-op).
 * @param amount Quantity to add.
 * @return 0 if added successfully (count < 100), 1 if capped or inventory full.
 */
s32 addItemToInventory(s32 itemId, s32 amount) {
    extern u8 D_80077EBC[];
    u8 *base = D_80077EBC;
    u8 *ptr;
    s32 i;

    if (itemId == 0) return 0;

    ptr = base;
    i = 0;

    do {
        if (*ptr == itemId) {
            s32 newCount;
            ptr++;
            newCount = *ptr + amount;
            *ptr = newCount;
            if ((u32)(newCount & 0xFF) < 100) return 0;
            *ptr = 100;
            return 1;
        }
        i++;
        ptr += 2;
    } while (i < 0xC6);

    ptr = base;
    i = 0;

    do {
        if (*ptr == 0) {
            s32 newCount;
            *ptr = itemId;
            ptr++;
            newCount = *ptr + amount;
            *ptr = newCount;
            if ((u32)(newCount & 0xFF) < 100) return 0;
            *ptr = 100;
            return 1;
        }
        i++;
        ptr += 2;
    } while (i < 0xC6);

    return 1;
}


/**
 * @brief Check if an entity has a specific ability and whether its count is maxed.
 * Searches 32 ability slots (2 bytes each: ID + count) in g_gameState at offset 0x4A0.
 * @param entityIdx Entity index (stride 152 in g_gameState).
 * @param abilityId Ability ID to search for.
 * @return 0 if abilityId is 0 or empty slot exists, 1 if ability count >= 100, 2 if all slots full.
 */
INCLUDE_ASM("asm/nonmatchings/game", func_80021108);


/**
 * @brief Add or increment an ability in an entity's ability slot table.
 * Searches 32 slots (2 bytes each: ID + count) in g_gameState at offset 0x4A0.
 * @param entityIdx Entity index (stride 152 in g_gameState).
 * @param abilityId Ability ID to add/increment.
 * @return 0 if added/incremented, 1 if count already >= 100, 2 if all slots full.
 */
/**
 * @brief Give a magic spell to a character.
 *
 * If the character already has the spell, increments its quantity.
 * Otherwise finds the first empty slot and places it there.
 *
 * @param charIdx Character index (0-7).
 * @param magicId Magic spell ID to give (0 = no-op).
 * @return 0 if given successfully, 1 if already at max quantity (100), 2 if inventory full.
 */
s32 giveCharacterMagic(CharacterId charIdx, MagicId magicId) {
    s32 i;

    if (magicId == 0) return 0;

    for (i = 0; i < MAGIC_SLOT_COUNT; i++) {
        if (g_gameState.chars[charIdx].magic[i].magicId == magicId) {
            if (g_gameState.chars[charIdx].magic[i].quantity < 100) {
                g_gameState.chars[charIdx].magic[i].quantity++;
                return 0;
            }
            return 1;
        }
    }

    for (i = 0; i < MAGIC_SLOT_COUNT; i++) {
        if (g_gameState.chars[charIdx].magic[i].magicId == 0) {
            g_gameState.chars[charIdx].magic[i].magicId = magicId;
            g_gameState.chars[charIdx].magic[i].quantity++;
            return 0;
        }
    }

    return 2;
}


/**
 * @brief Check if a character's ability list contains a specific ability ID.
 * @param a0 Character index used to look up a slot ID in g_gameState at offset 0xAF4.
 * @param a1 Ability ID to search for; returns 0 immediately if a1 is 0.
 * @return 1 if the ability is found in the character's 20-entry ability list, 0 otherwise.
 */
s32 hasJunctionedAbility(a0, a1)

s32 a0;
s32 a1;
{
    u8 slot_id;
    s32 i;

    if (a1 == 0) return 0;

    slot_id = g_gameState.party.party[a0];
    i = 0;
    while (i < 20) {
        if (g_gameState.chars[slot_id].junctions[i] == a1) {
            return 1;
        }
        i++;
    }
    return 0;
}


/**
 * @brief Search D_8005289C array for the value stored at D_80082C08.
 * @return 1 if found, 0 if not found or array is empty (0xFFFF terminated).
 */
INCLUDE_ASM("asm/nonmatchings/game", func_80021300);


/**
 * @brief Main state machine loop driven by g_vsyncRate.
 * Processes rendering/audio states: 4=render, 3=init+render, 5=transition, 8=alt render.
 * Loops until an unhandled state value is encountered, which sets g_vsyncRate=4 and returns.
 */
void gameStateLoop(void) {
    extern volatile u16 g_vsyncRate;
    extern volatile s16 g_renderMode;
    extern s32 D_800974C0[2];
    extern s32 D_800974C8[2];
    extern s32 D_800974B8[2];

    void cdReadSync(s32, s32, s32, s32);
    void func_8001F5C8(void);
    void func_80098238(void);
    void setCameraVibrateIntensity(s32);
    void setCameraVibrateState(s32);
    s32 func_80021300(void);
    void func_80023D60(s32);
    void cdReadAsyncSync(s32, s32, s32, s32);
    void func_80099D30(void);
    void func_80098304(void);
    void func_80035360(void);

    s32 mode = 4;
    s16 state;

top:
    state = (s16)g_vsyncRate;
    if (state == mode) goto case4;
    if (state < 5) {
        if (state == 3) goto case3;
        goto default_case;
    }
    if (state == 5) goto case5;
    if (state == 8) goto case8;
    goto default_case;

case4:
    cdReadSync(D_800974C0[0], D_800974C0[1], 0x80098000, 0);
    func_8001F5C8();
    func_80098238();
    goto top;

case3:
    setCameraVibrateIntensity(0);
    setCameraVibrateState(0);
    func_80023D60(func_80021300());
    memzero16((s32 *)0x80098000, 0xA400);
    cdReadSync(D_800974C8[0], D_800974C8[1], 0x80098000, 0);
    func_8001F5C8();
    func_80099D30();
    goto top;

case8:
    g_renderMode = 0;
    cdReadAsyncSync(D_800974B8[0], D_800974B8[1], 0x80098000, 0);
    func_8001F5C8();
    func_80098304();
    goto top;

case5:
    func_80035360();
    g_renderMode = mode;
    func_8001F5C8();
    g_vsyncRate = 100;
    goto top;

default_case:
    g_vsyncRate = mode;
}


/**
 * @brief Add a value to a character's stat at offset 0x02, clamping the result to 9999.
 * @param a0 Character index used to resolve a slot ID via g_gameState.
 * @param a1 Amount to add to the stat.
 * @note The stat at ptr+2 (likely HP or experience) is read as u16, added to a1, then clamped by clampToMaxHp.
 */
void addCharMaxHp(s32 a0, s32 a1) {
    u8 idx = g_gameState.party.party[a0];
    CharacterData *ch = &g_gameState.chars[idx];
    ch->maxHp = clampToMaxHp(ch->maxHp + a1);
}


