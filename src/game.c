#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gf.h"
#include "gamestate.h"
#include "ability.h"

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
void setGfExists(s32 gfId) {
    g_gameState.gfs[gfId].exists |= 1;
}


/** @brief Returns a pointer to the Boko name field in the save header. */
u8 *getBokoName(void) {
    return g_gameState.bokoName;
}


/** @brief Returns a pointer to the Angelo name field in the save header. */
u8 *getAngeloName(void) {
    return g_gameState.angeloName;
}


/** @brief Resolves param from GfData.subTableU[gfId] (stride 20) via resolveKernelPtr. */
s32 getGfSummonData(s32 gfId) {
    return resolveKernelPtr(g_gfData.subTableU[gfId].param0, g_gfData.ptrSubTableU);
}


/**
 * @brief Resolve GF name pointer from GfData.subTableT[gfId] (stride 8).
 * @param a0 GF index (0 returns default D_800773A8 pointer).
 * @return Pointer from resolveKernelPtr lookup, or &D_800773A8 if a0 is 0.
 */
u8 *getGfName(s32 gfId) {
    u8 *result;

    if (gfId != 0) {
        result = resolveKernelPtr(g_gfData.subTableT[gfId].param0, g_gfData.ptrSubTableT);
    } else {
        result = g_gameState.angeloName;
    }
    return result;
}


/** @brief Resolves param0 from GfData.subTableS[entityId] (stride 32) via resolveKernelPtr. */
s32 getMagicEffectName(s32 entityId) {
    return resolveKernelPtr(g_gfData.subTableS[entityId].param0, g_gfData.ptrSubTableS);
}


/** @brief Resolves param1 from GfData.subTableS[entityId] (stride 32, +2) via resolveKernelPtr. */
s32 getMagicEffectDesc(s32 entityId) {
    return resolveKernelPtr(g_gfData.subTableS[entityId].param1, g_gfData.ptrSubTableS);
}


/** @brief Resolves param0 from GfData.subTableR[statusId] (stride 24) via resolveKernelPtr. */
s32 getStatusEffectName(s32 statusId) {
    return resolveKernelPtr(g_gfData.subTableR[statusId].param0, g_gfData.ptrSubTableR);
}


/** @brief Resolves param1 from GfData.subTableR[statusId] (stride 24, +2) via resolveKernelPtr. */
s32 getStatusEffectDesc(s32 statusId) {
    return resolveKernelPtr(g_gfData.subTableR[statusId].param1, g_gfData.ptrSubTableR);
}


/** @brief Resolves param0 from GfData.elementData24[elemId] (stride 24) via resolveKernelPtr. */
s32 getElementName(s32 elemId) {
    return resolveKernelPtr(g_gfData.elementData24[elemId].param0, g_gfData.ptrElementData24);
}


/** @brief Resolves param1 from GfData.elementData24[elemId] (stride 24, +2) via resolveKernelPtr. */
s32 getElementDesc(s32 elemId) {
    return resolveKernelPtr(g_gfData.elementData24[elemId].param1, g_gfData.ptrElementData24);
}


/** @brief Resolves param0 from GfData.subTableQ[effectId] (stride 16) via resolveKernelPtr. */
s32 getJuncEffectName(s32 effectId) {
    return resolveKernelPtr(g_gfData.subTableQ[effectId].param0, g_gfData.ptrSubTableQ);
}


/** @brief Resolves param1 from GfData.subTableQ[effectId] (stride 16, +2) via resolveKernelPtr. */
s32 getJuncEffectDesc(s32 effectId) {
    return resolveKernelPtr(g_gfData.subTableQ[effectId].param1, g_gfData.ptrSubTableQ);
}


/** @brief Resolves param0 from GfData.subTableP[catId] (stride 24) via resolveKernelPtr. */
s32 getJuncCategoryName(s32 catId) {
    return resolveKernelPtr(g_gfData.subTableP[catId].param0, g_gfData.ptrSubTableP);
}


/** @brief Resolves param1 from GfData.subTableP[catId] (stride 24, +2) via resolveKernelPtr. */
s32 getJuncCategoryDesc(s32 catId) {
    return resolveKernelPtr(g_gfData.subTableP[catId].param1, g_gfData.ptrSubTableP);
}


/**
 * @brief Look up the name string for a given ability ID.
 *
 * Abilities are organized into 7 range tables within the kernel data,
 * one per ability type (junction, command, character A/B, party, GF, menu).
 *
 * @param abilityId Ability ID (0-120, see AbilityId enum).
 * @return Pointer to the ability's name string.
 */
u8 *getAbilityName(s32 abilityId) {
    u16 param;
    s32 base;

    if (abilityId < ABILITY_MAGIC) {
        param = g_gfData.abilityRangeI[abilityId].statParam0;
        base = g_gfData.ptrAbilityRangeI;
    } else if ((u32)(abilityId - ABILITY_MAGIC) < 19) {
        s32 idx = abilityId - ABILITY_MAGIC;
        param = g_gfData.abilityRangeJ[idx].statParam0;
        base = g_gfData.ptrAbilityRangeJ;
    } else if ((u32)(abilityId - ABILITY_HP_20) < 19) {
        s32 idx = abilityId - ABILITY_HP_20;
        param = g_gfData.abilityRangeK[idx].statParam0;
        base = g_gfData.ptrAbilityRangeK;
    } else if ((u32)(abilityId - ABILITY_MUG) < 20) {
        s32 idx = abilityId - ABILITY_MUG;
        param = g_gfData.abilityRangeL[idx].statParam0;
        base = g_gfData.ptrAbilityRangeL;
    } else if ((u32)(abilityId - ABILITY_ALERT) < 5) {
        s32 idx = abilityId - ABILITY_ALERT;
        param = g_gfData.abilityRangeM[idx].statParam0;
        base = g_gfData.ptrAbilityRangeM;
    } else {
        s32 idx = abilityId - ABILITY_SUMMAG_10;
        if ((u32)idx >= 9) {
            param = g_gfData.abilityRangeO[abilityId - ABILITY_HAGGLE].statParam0;
            base = g_gfData.ptrAbilityRangeO;
        } else {
            param = g_gfData.abilityRangeN[idx].statParam0;
            base = g_gfData.ptrAbilityRangeN;
        }
    }
    return resolveKernelPtr(param, base);
}


/**
 * @brief Look up the description string for a given ability ID.
 * @param abilityId Ability ID (0-120, see AbilityId enum).
 * @return Pointer to the ability's description string.
 */
u8 *getAbilityDesc(s32 abilityId) {
    u16 param;
    s32 base;

    if (abilityId < ABILITY_MAGIC) {
        param = g_gfData.abilityRangeI[abilityId].statParam1;
        base = g_gfData.ptrAbilityRangeI;
    } else if ((u32)(abilityId - ABILITY_MAGIC) < 19) {
        s32 idx = abilityId - ABILITY_MAGIC;
        param = g_gfData.abilityRangeJ[idx].statParam1;
        base = g_gfData.ptrAbilityRangeJ;
    } else if ((u32)(abilityId - ABILITY_HP_20) < 19) {
        s32 idx = abilityId - ABILITY_HP_20;
        param = g_gfData.abilityRangeK[idx].statParam1;
        base = g_gfData.ptrAbilityRangeK;
    } else if ((u32)(abilityId - ABILITY_MUG) < 20) {
        s32 idx = abilityId - ABILITY_MUG;
        param = g_gfData.abilityRangeL[idx].statParam1;
        base = g_gfData.ptrAbilityRangeL;
    } else if ((u32)(abilityId - ABILITY_ALERT) < 5) {
        s32 idx = abilityId - ABILITY_ALERT;
        param = g_gfData.abilityRangeM[idx].statParam1;
        base = g_gfData.ptrAbilityRangeM;
    } else {
        s32 idx = abilityId - ABILITY_SUMMAG_10;
        if ((u32)idx >= 9) {
            param = g_gfData.abilityRangeO[abilityId - ABILITY_HAGGLE].statParam1;
            base = g_gfData.ptrAbilityRangeO;
        } else {
            param = g_gfData.abilityRangeN[idx].statParam1;
            base = g_gfData.ptrAbilityRangeN;
        }
    }
    return resolveKernelPtr(param, base);
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
u8 *getMagicNamePtr(s32 magicId) {
    extern u8 D_800762C8[];

    if (magicId < 0x40) {
        return (u8 *)resolveKernelPtr(g_gfData.junctionData[magicId].nameParam0, g_gfData.ptrGfSpellData);
    }
    return D_800762C8 + magicId * 68;
}


/**
 * @brief Resolve a secondary data pointer for a spell or GF ability.
 *
 * For magic spells (< 64), reads nameParam1 from the junction data table.
 * For GF abilities (>= 64), reads nameParam0 from the ability table.
 *
 * @param spellId Spell/ability index.
 * @return Resolved data pointer via resolveKernelPtr.
 */
s32 getSpellEntityData(s32 spellId) {
    if (spellId < 0x40) {
        return resolveKernelPtr(g_gfData.junctionData[spellId].nameParam1, g_gfData.ptrGfSpellData);
    }
    return resolveKernelPtr(g_gfData.abilityTable132[spellId - 0x40].nameParam0, g_gfData.ptrAbilityTable132);
}


/**
 * @brief Look up the name string for a stat/command ID.
 * @param statId Stat index; >= 0x21 uses statTable4, otherwise statTable24.
 * @return Pointer to the stat's name string.
 */
s32 getStatName(s32 statId) {
    u16 param;
    s32 base;

    if (statId >= 0x21) {
        param = g_gfData.statTable4[statId - 0x21].param0;
        base = g_gfData.ptrStatTable4;
    } else {
        param = g_gfData.statTable24[statId].param0;
        base = g_gfData.ptrStatTable24;
    }
    return resolveKernelPtr(param, base);
}


/**
 * @brief Look up the description string for a stat/command ID.
 * @param statId Stat index; >= 0x21 uses statTable4, otherwise statTable24.
 * @return Pointer to the stat's description string.
 */
s32 getStatDesc(s32 statId) {
    u16 param;
    s32 base;

    if (statId >= 0x21) {
        param = g_gfData.statTable4[statId - 0x21].param1;
        base = g_gfData.ptrStatTable4;
    } else {
        param = g_gfData.statTable24[statId].param1;
        base = g_gfData.ptrStatTable24;
    }
    return resolveKernelPtr(param, base);
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


/** @brief Resolves param from GfData.levelCurve12[curveId] (stride 12) via resolveKernelPtr. */
s32 getLevelCurveData(s32 curveId) {
    return resolveKernelPtr(g_gfData.levelCurve12[curveId].param0, g_gfData.ptrLevelCurve12);
}


/** @brief Resolves AbilityEntry.statParam0 from GfData.statTable8[entryId] via resolveKernelPtr. */
s32 getAbilityEntryName(s32 entryId) {
    return resolveKernelPtr(g_gfData.statTable8[entryId].statParam0, g_gfData.ptrStatTable8);
}


/** @brief Resolves AbilityEntry.statParam1 from GfData.statTable8[entryId] via resolveKernelPtr. */
s32 getAbilityEntryDesc(s32 entryId) {
    return resolveKernelPtr(g_gfData.statTable8[entryId].statParam1, g_gfData.ptrStatTable8);
}


/** @brief Wrapper that calls func_80020F84 with argument 3. */
s32 getDefaultMenuLabel(void) {
    return getMenuString(3);
}


/**
 * @brief Look up a u16 from GfData.subTableV[stringId] (stride 2) and resolve via resolveKernelPtr.
 * @param a0 Index into subTableV.
 * @return Resolved data pointer.
 */
s32 getMenuString(s32 stringId) {
    return resolveKernelPtr(g_gfData.subTableV[stringId].param0, g_gfData.ptrSubTableV);
}


/**
 * @brief Resolve an offset within the kernel data region to a pointer.
 * @param offset Byte offset from g_gfData base (0xFFFF = invalid, returns default).
 * @param tableBase Base offset of the containing table within g_gfData.
 * @return Pointer to g_gfData + tableBase + offset, or D_80052898 if offset is 0xFFFF.
 */
u8 *resolveKernelPtr(u16 offset, s32 tableBase) {
    extern u8 D_80052898[];
    u8 *result;
    if (offset != 0xFFFF) {
        result = tableBase + (offset + (u8 *)&g_gfData);
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
    u8 *base = (u8 *)g_gameState.itemSlots;
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
        ptr += sizeof(ItemSlot);
    } while (i < ITEM_SLOT_COUNT);

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
        ptr += sizeof(ItemSlot);
    } while (i < ITEM_SLOT_COUNT);

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
 * @brief Check if a character has a specific ability junctioned.
 * @param partySlot Party slot index (0-2) to look up the character ID.
 * @param abilityId Ability ID to search for; returns 0 if 0.
 * @return 1 if the ability is found in the character's junction list, 0 otherwise.
 */
s32 hasJunctionedAbility(partySlot, abilityId)

s32 partySlot;
s32 abilityId;
{
    u8 slot_id;
    s32 i;

    if (abilityId == 0) return 0;

    slot_id = g_gameState.party.party[partySlot];
    i = 0;
    while (i < 20) {
        if (g_gameState.chars[slot_id].junctions[i] == abilityId) {
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
void addCharMaxHp(s32 partyIdx, s32 amount) {
    u8 idx = g_gameState.party.party[partyIdx];
    CharacterData *ch = &g_gameState.chars[idx];
    ch->maxHp = clampToMaxHp(ch->maxHp + amount);
}


