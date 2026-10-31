#include "common.h"
#include "psxsdk/libgpu.h"
#include "game.h"
#include "battle.h"
#include "ui/countdown.h"
#include "gf.h"
#include "gamestate.h"
#include "ability.h"
#include "battle_render.h"
#include "battle_results/result.h"
#include "btl_transition.h"
#include "memlayout.h"
#include "cd.h"
#include "cdrom.h"

#define OVERLAY_PTR ((void *)OVERLAY_ADDR)

u8 *resolveKernelPtr(u16 a0, s32 a1);

extern volatile s16 g_renderMode;
extern s32 D_800974C0[2];
extern s32 D_800974C8[2];
extern s32 D_800974B8[2];
extern u8 D_800762C8[];
extern u8 D_80052898[];
extern u16 g_bossBattleScenes[]; /**< Battle scene IDs of the boss battles, ended by 0xFFFF. */

void func_8001F5C8(void);
void func_80099D30(void);
void tripleTriadMainLoop(void);

/**
 * @brief Empty stub at the start of game.c (no-op return).
 */
void func_800205C8(void) {
}

/**
 * @brief Game code VSync handler. Clears render mode if isResultsThreadDone signals completion.
 *
 * Called from the VSync dispatch (g_renderMode == RENDER_BATTLE_RESULTS). Invokes switchToResultsThread
 * for per-frame processing, then checks isResultsThreadDone's return. If non-zero,
 * sets g_renderMode to 0 (RENDER_IDLE) to signal the main loop.
 */
void vsyncGameHandler(void) {
    switchToResultsThread();
    if (isResultsThreadDone() != 0) {
        g_renderMode = RENDER_IDLE;
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


/** @brief Returns the name of Rinoa limit break (part 2) @p id. */
u8 *getRinoaLimitBreak2Name(s32 id) {
    return resolveKernelPtr(g_kernel.rinoaLimitBreaks2[id].nameOffset, g_kernel.rinoaLimitBreaks2Text);
}


/**
 * @brief Returns the name of Rinoa limit break (part 1) @p id.
 * @param id Entry index; 0 returns Angelo's name from the save.
 */
u8 *getRinoaLimitBreak1Name(s32 id) {
    u8 *result;

    if (id != 0) {
        result = resolveKernelPtr(g_kernel.rinoaLimitBreaks1[id].nameOffset, g_kernel.rinoaLimitBreaks1Text);
    } else {
        result = g_gameState.angeloName;
    }
    return result;
}


/** @brief Returns the name of duel entry @p id. */
u8 *getDuelName(s32 id) {
    return resolveKernelPtr(g_kernel.duel[id].nameOffset, g_kernel.duelText);
}


/** @brief Returns the description of duel entry @p id. */
u8 *getDuelDesc(s32 id) {
    return resolveKernelPtr(g_kernel.duel[id].descOffset, g_kernel.duelText);
}


/** @brief Returns the name of shot entry @p id. */
u8 *getShotName(s32 id) {
    return resolveKernelPtr(g_kernel.shot[id].nameOffset, g_kernel.shotText);
}


/** @brief Returns the description of shot entry @p id. */
u8 *getShotDesc(s32 id) {
    return resolveKernelPtr(g_kernel.shot[id].descOffset, g_kernel.shotText);
}


/** @brief Returns the name of renzokuken finisher entry @p id. */
u8 *getRenzokukenFinisherName(s32 id) {
    return resolveKernelPtr(g_kernel.renzokukenFinishers[id].nameOffset, g_kernel.renzokukenFinishersText);
}


/** @brief Returns the description of renzokuken finisher entry @p id. */
u8 *getRenzokukenFinisherDesc(s32 id) {
    return resolveKernelPtr(g_kernel.renzokukenFinishers[id].descOffset, g_kernel.renzokukenFinishersText);
}


/** @brief Returns the name of blue magic entry @p id. */
u8 *getBlueMagicName(s32 id) {
    return resolveKernelPtr(g_kernel.blueMagic[id].nameOffset, g_kernel.blueMagicText);
}


/** @brief Returns the description of blue magic entry @p id. */
u8 *getBlueMagicDesc(s32 id) {
    return resolveKernelPtr(g_kernel.blueMagic[id].descOffset, g_kernel.blueMagicText);
}


/** @brief Returns the name of temporary limit break entry @p id. */
u8 *getTempLimitBreakName(s32 id) {
    return resolveKernelPtr(g_kernel.tempLimitBreaks[id].nameOffset, g_kernel.tempLimitBreaksText);
}


/** @brief Returns the description of temporary limit break entry @p id. */
u8 *getTempLimitBreakDesc(s32 id) {
    return resolveKernelPtr(g_kernel.tempLimitBreaks[id].descOffset, g_kernel.tempLimitBreaksText);
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
        param = g_kernel.junctionAbilities[abilityId].statParam0;
        base = g_kernel.junctionAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_MAGIC) < 19) {
        s32 idx = abilityId - ABILITY_MAGIC;
        param = g_kernel.commandAbilities[idx].statParam0;
        base = g_kernel.commandAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_HP_20) < 19) {
        s32 idx = abilityId - ABILITY_HP_20;
        param = g_kernel.statPercentAbilities[idx].statParam0;
        base = g_kernel.statPercentAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_MUG) < 20) {
        s32 idx = abilityId - ABILITY_MUG;
        param = g_kernel.characterAbilities[idx].statParam0;
        base = g_kernel.characterAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_ALERT) < 5) {
        s32 idx = abilityId - ABILITY_ALERT;
        param = g_kernel.partyAbilities[idx].statParam0;
        base = g_kernel.partyAbilitiesText;
    } else {
        s32 idx = abilityId - ABILITY_SUMMAG_10;
        if ((u32)idx >= 9) {
            param = g_kernel.menuAbilities[abilityId - ABILITY_HAGGLE].statParam0;
            base = g_kernel.menuAbilitiesText;
        } else {
            param = g_kernel.gfAbilities[idx].statParam0;
            base = g_kernel.gfAbilitiesText;
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
        param = g_kernel.junctionAbilities[abilityId].statParam1;
        base = g_kernel.junctionAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_MAGIC) < 19) {
        s32 idx = abilityId - ABILITY_MAGIC;
        param = g_kernel.commandAbilities[idx].statParam1;
        base = g_kernel.commandAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_HP_20) < 19) {
        s32 idx = abilityId - ABILITY_HP_20;
        param = g_kernel.statPercentAbilities[idx].statParam1;
        base = g_kernel.statPercentAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_MUG) < 20) {
        s32 idx = abilityId - ABILITY_MUG;
        param = g_kernel.characterAbilities[idx].statParam1;
        base = g_kernel.characterAbilitiesText;
    } else if ((u32)(abilityId - ABILITY_ALERT) < 5) {
        s32 idx = abilityId - ABILITY_ALERT;
        param = g_kernel.partyAbilities[idx].statParam1;
        base = g_kernel.partyAbilitiesText;
    } else {
        s32 idx = abilityId - ABILITY_SUMMAG_10;
        if ((u32)idx >= 9) {
            param = g_kernel.menuAbilities[abilityId - ABILITY_HAGGLE].statParam1;
            base = g_kernel.menuAbilitiesText;
        } else {
            param = g_kernel.gfAbilities[idx].statParam1;
            base = g_kernel.gfAbilitiesText;
        }
    }
    return resolveKernelPtr(param, base);
}


/**
 * @brief Get a pointer to a magic spell's name string.
 *
 * If @p a0 < 0x40, indexes into Kernel.magic (stride 60) at offset
 * 0x21C to get a 16-bit index, then resolves it via resolveKernelPtr against
 * Kernel.magicText (+0x84). If @p a0 >= 0x40, returns directly from
 * D_800762C8 at stride 68.
 *
 * @param a0 Magic spell ID.
 * @return Pointer to the spell's encoded name string.
 */
u8 *getMagicNamePtr(s32 magicId) {
    if (magicId < 0x40) {
        return resolveKernelPtr(g_kernel.magic[magicId].nameOffset, g_kernel.magicText);
    }
    return D_800762C8 + magicId * 68;
}


/**
 * @brief Returns the description of a magic spell or junctionable GF.
 * @param spellId Magic ID below 0x40; from 0x40, junctionable GF @p spellId - 0x40.
 */
u8 *getSpellDesc(s32 spellId) {
    if (spellId < 0x40) {
        return resolveKernelPtr(g_kernel.magic[spellId].descOffset, g_kernel.magicText);
    }
    return resolveKernelPtr(g_kernel.junctionableGfs[spellId - 0x40].descOffset, g_kernel.junctionableGfsText);
}


/**
 * @brief Returns an item's name.
 * @param itemId Battle item below 0x21; from 0x21, non-battle item @p itemId - 0x21.
 */
u8 *getItemName(s32 itemId) {
    u16 param;
    s32 base;

    if (itemId >= 0x21) {
        param = g_kernel.nonBattleItems[itemId - 0x21].param0;
        base = g_kernel.nonBattleItemsText;
    } else {
        param = g_kernel.battleItems[itemId].nameOffset;
        base = g_kernel.battleItemsText;
    }
    return resolveKernelPtr(param, base);
}


/**
 * @brief Returns an item's description.
 * @param itemId Battle item below 0x21; from 0x21, non-battle item @p itemId - 0x21.
 */
u8 *getItemDesc(s32 itemId) {
    u16 param;
    s32 base;

    if (itemId >= 0x21) {
        param = g_kernel.nonBattleItems[itemId - 0x21].param1;
        base = g_kernel.nonBattleItemsText;
    } else {
        param = g_kernel.battleItems[itemId].descOffset;
        base = g_kernel.battleItemsText;
    }
    return resolveKernelPtr(param, base);
}


/**
 * @brief Get a battle entity's name string pointer.
 *
 * Squall and Rinoa have custom names stored in the save header.
 * All other characters use the default name from kernel.bin.
 *
 * @param entityIdx Battle entity index into g_battleChars.
 * @return Pointer to the character's name string.
 */
u8 *getBattleCharName(s32 entityIdx) {
    if (g_battleChars.chars[entityIdx].characterId == CHAR_SQUALL) {
        return g_gameState.squallName;
    }
    if (g_battleChars.chars[entityIdx].characterId == CHAR_RINOA) {
        return g_gameState.rinoaName;
    }
    return resolveKernelPtr(
        g_kernel.characters[g_battleChars.chars[entityIdx].characterId].nameOffset,
        g_kernel.charactersText);
}


/**
 * @brief Get a pointer to a character's name string.
 *
 * Special cases: characterId 0 returns g_gameState+0x18, characterId 4
 * returns g_gameState+0x24. All others index into Kernel.characters
 * (stride 36) at offset 0x37A4 and resolve via resolveKernelPtr against
 * Kernel.charactersText (+0x98).
 *
 * @param a0 Character ID (see CharacterId).
 * @return Pointer to the character's encoded name string.
 */
u8 *getCharName(CharacterId charId) {
    if (charId == CHAR_SQUALL) {
        return g_gameState.squallName;
    }
    if (charId == CHAR_RINOA) {
        return g_gameState.rinoaName;
    }
    return resolveKernelPtr(g_kernel.characters[charId].nameOffset, g_kernel.charactersText);
}


/** @brief Returns the name of weapon @p id. */
u8 *getWeaponName(s32 id) {
    return resolveKernelPtr(g_kernel.weapons[id].nameOffset, g_kernel.weaponsText);
}


/** @brief Returns the name of battle command @p id. */
u8 *getBattleCommandName(s32 id) {
    return resolveKernelPtr(g_kernel.battleCommands[id].nameOffset, g_kernel.battleCommandsText);
}


/** @brief Returns the description of battle command @p id. */
u8 *getBattleCommandDesc(s32 id) {
    return resolveKernelPtr(g_kernel.battleCommands[id].descOffset, g_kernel.battleCommandsText);
}


/** @brief Wrapper that calls getMenuString with argument 3. */
static u8 *getDefaultMenuLabel(void) {
    return getMenuString(3);
}


/**
 * @brief Look up a u16 from Kernel.miscTextPointers[stringId] (stride 2) and resolve via resolveKernelPtr.
 * @param a0 Index into subTableV.
 * @return Resolved data pointer.
 */
u8 *getMenuString(s32 stringId) {
    return resolveKernelPtr(g_kernel.miscTextPointers[stringId].param0, g_kernel.miscText);
}


/**
 * @brief Resolve an offset within the kernel data region to a pointer.
 * @param offset Byte offset from g_kernel base (0xFFFF = invalid, returns default).
 * @param tableBase Base offset of the containing table within g_kernel.
 * @return Pointer to g_kernel + tableBase + offset, or D_80052898 if offset is 0xFFFF.
 */
u8 *resolveKernelPtr(u16 offset, s32 tableBase) {
    u8 *result;
    if (offset != 0xFFFF) {
        result = tableBase + (offset + (u8 *)&g_kernel);
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
        if (g_gameState.mainData.itemSlots[i].id == itemId) {
            g_gameState.mainData.itemSlots[i].count--;
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
    u8 *base = (u8 *)g_gameState.mainData.itemSlots;
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
 * @brief Classify the stock state of a spell in a character's magic inventory.
 *
 * Scans the 32-entry @ref MagicSlot magic array of
 * @c g_gameState.chars[charId] (offset 0x4A0) for @p magicId.
 *
 * @param charId  Character index into @c g_gameState.chars.
 * @param magicId Magic spell ID to look for.
 * @return 0 if @p magicId is 0, the spell is stocked but below max, or it is
 *           not stocked yet a free slot remains;
 *         1 if the spell is stocked at max quantity (>= 100);
 *         2 if the spell is not stocked and all 32 slots are full.
 */
s32 func_80021108(s32 charId, s32 magicId) {
    s32 i;

    if (magicId == 0) {
        return 0;
    }
    for (i = 0; i < 32; i++) {
        if (g_gameState.chars[charId].magic[i].magicId == magicId) {
            return g_gameState.chars[charId].magic[i].quantity >= 100;
        }
    }
    for (i = 0; i < 32; i++) {
        if (g_gameState.chars[charId].magic[i].magicId == 0) {
            return 0;
        }
    }
    return 2;
}


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
s32 hasJunctionedAbility(s32 partySlot, s32 abilityId) {
    u8 slot_id;
    s32 i;

    if (abilityId == 0) return 0;

    slot_id = g_gameState.mainData.party.partyMembers[partySlot];
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
 * @brief Test whether the upcoming battle is a boss battle.
 *
 * Looks @c g_battleConfig.battleSceneId up in @ref g_bossBattleScenes. Boss
 * battles get their own encounter sound and screen transition.
 *
 * @return 1 for a boss battle, 0 otherwise.
 */
s32 isBossBattle(void) {
    s32 found = 0;
    s32 i;

    for (i = 0; g_bossBattleScenes[i] != 0xFFFF; i++) {
        if (g_battleConfig.battleSceneId == g_bossBattleScenes[i]) {
            found = 1;
            break;
        }
    }
    return found;
}


/**
 * @brief Main state machine loop driven by g_gameMode.
 * Runs a battle (3), the battle debug menu (4), the battle results screen (5) or Triple Triad (8).
 * Any other value leaves the loop, which sets g_gameMode to 4 and returns.
 */
void gameStateLoop(void) {
    s32 mode = 4;
    s16 state;

top:
    state = g_gameMode;
    if (state == mode) goto case4;
    if (state < 5) {
        if (state == GAME_MODE_BATTLE) goto case3;
        goto default_case;
    }
    if (state == GAME_MODE_BATTLE_RESULTS) goto case5;
    if (state == GAME_MODE_CARD) goto case8;
    goto default_case;

case4:
    cdReadSync(D_800974C0[0], D_800974C0[1], OVERLAY_PTR, NULL);
    func_8001F5C8();
    func_80098238();
    goto top;

case3:
    setHudBrightness(0);
    setCountdownVisible(0);
    startBattleTransition(isBossBattle());
    memzero16(OVERLAY_PTR, 0xA400);
    cdReadSync(D_800974C8[0], D_800974C8[1], OVERLAY_PTR, NULL);
    func_8001F5C8();
    func_80099D30();
    goto top;

case8:
    g_renderMode = RENDER_IDLE;
    cdReadAsyncSync(D_800974B8[0], D_800974B8[1], OVERLAY_PTR, NULL);
    func_8001F5C8();
    tripleTriadMainLoop();
    goto top;

case5:
    startBattleResults();
    g_renderMode = mode;
    func_8001F5C8();
    g_gameMode = GAME_MODE_EXIT;
    goto top;

default_case:
    g_gameMode = mode;
}


/**
 * @brief Add a value to a character's stat at offset 0x02, clamping the result to 9999.
 * @param a0 Character index used to resolve a slot ID via g_gameState.
 * @param a1 Amount to add to the stat.
 * @note The stat at ptr+2 (likely HP or experience) is read as u16, added to a1, then clamped by clampToMaxHp.
 */
void addCharMaxHp(s32 partyIdx, s32 amount) {
    u8 idx = g_gameState.mainData.party.partyMembers[partyIdx];
    CharacterData *ch = &g_gameState.chars[idx];
    ch->maxHpBonus = clampToMaxHp(ch->maxHpBonus + amount);
}


