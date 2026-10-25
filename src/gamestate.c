#include "common.h"
#include "gamestate.h"
#include "battle.h"
#include "party_stats.h"

extern s32 findNthSetBit(s32, s32);

/** @brief 0x20-byte free-id table immediately before the D_80077EBC pair list. */
typedef struct {
    /* 0x000 */ u8 freeIds[0x20];
} FreeIdTable; /* sizeof == 0x20 */

static void copyGfHpToSave(s32 gfIdx);
static void func_80036FE0(s32 charIdx);

/**
 * @brief Reset a character's junctions and recalculate stats.
 *
 * Clears junctioned GFs, commands, abilities, and junction slots for the
 * given character. Temporarily sets the character as party leader to trigger
 * stat recalculation, clears status (preserving bit 7), sets HP from
 * g_battleChars.chars[0].maxHp, then restores the original party slot.
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

    chr->currentHp = g_battleChars.chars[0].maxHp;

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
u32 func_80036EC0(void) {
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
static void copyGfHpToSave(s32 gfIdx) {
    g_gameState.gfs[gfIdx].hp = g_battleChars.gfs[gfIdx].maxHp;
}


/**
 * @brief Set a character as party leader, reset their HP and status,
 *        then restore original party slots.
 *
 * Saves the current party, clears it, sets the leader to trigger
 * stat recalculation, then writes g_battleChars.chars[0].maxHp to the character's
 * currentHp and clears all status flags except bit 7. Finally
 * restores the saved party and recalculates stats.
 *
 * @param charIdx Character index.
 */
static void func_80036FE0(s32 charIdx) {
    u8 saved[4];
    s32 i;

    for (i = 0; i < 3; i++) {
        saved[i] = g_gameState.mainData.party.partyMembers[i];
        g_gameState.mainData.party.partyMembers[i] = 0xFF;
    }

    setPartyLeader(charIdx);

    g_gameState.chars[charIdx].currentHp = g_battleChars.chars[0].maxHp;
    g_gameState.chars[charIdx].statusFlags &= 0x80;

    for (i = 0; i < 3; i++) {
        g_gameState.mainData.party.partyMembers[i] = saved[i];
    }

    recalcPartyStats();
}


void func_800370AC(s32 itemId) {
    s32 i;
    u8 *inventory;
    u8 *battleOrder;
    s32 usedOrderMask;    
    s32 inventoryItemId;
    s32 unusedOrder;

    if (itemId < BATTLE_ITEM_ID_LIMIT) {
        inventory = &g_gameState.mainData.itemSlots[0].id;
        battleOrder = g_gameState.mainData.battleOrder;
        usedOrderMask = 0;

        for (i = 0; i < ITEM_SLOT_COUNT; i++) {
            inventoryItemId = *inventory++; // inventoryItemId = inventory->id
            
            if (*inventory++ == 0) { // if inventory->quantity == 0, inventoryItemId = 0
                inventoryItemId = 0;
            }
            
            if (inventoryItemId != 0 && inventoryItemId < BATTLE_ITEM_ID_LIMIT) {
                if (itemId == inventoryItemId) {
                    return;
                }

                usedOrderMask |= 1 << battleOrder[inventoryItemId - 1];
            }
        }

        unusedOrder = findNthSetBit(~usedOrderMask, 0);
        itemId--;

        for (i = 0; i < BATTLE_ITEM_COUNT; i++) {
            if (unusedOrder == battleOrder[i]) {
                u8 tmp = battleOrder[itemId];

                battleOrder[itemId] = battleOrder[i];
                battleOrder[i] = tmp;
                return;
            }
        }
    }
}


/**
 * @brief Set a bit in the global bitfield array g_gameState.mainData.unk2F.
 * @param a0 Bit index to set.
 */
void setFieldFlag(s32 bitIdx) {
    u8 *base = g_gameState.mainData.unk2F;
    s32 byteIdx = bitIdx / 8;
    base[byteIdx] |= (1 << (bitIdx & 7));
}


/**
 * @brief Clear a bit in the global bitfield array g_gameState.mainData.unk2F.
 * @param a0 Bit index to clear.
 */
void clearFieldFlag(s32 bitIdx) {
    u8 *base = g_gameState.mainData.unk2F;
    s32 byteIdx = bitIdx / 8;
    base[byteIdx] &= ~(1 << (bitIdx & 7));
}


/**
 * @brief Test a bit in the global bitfield array g_gameState.mainData.unk2F.
 * @param a0 Bit index to test.
 * @return Non-zero if bit is set, zero otherwise.
 */
s32 testFieldFlag(s32 bitIdx) {
    u8 *base = g_gameState.mainData.unk2F;
    s32 byteIdx = bitIdx / 8;
    return base[byteIdx] & (1 << (bitIdx & 7));
}


/**
 * @brief Synchronize HP values for all available characters and GFs to the save data.
 *
 * Gets the character availability bitmask, iterates bits 0-7 calling
 * func_80036FE0 for each set bit. Then gets the GF availability bitmask
 * and iterates bits 0-15 calling copyGfHpToSave for each set bit.
 */
void func_80037240(void) {
    s32 i;
    u16 mask;

    mask = func_80036EC0();
    i = 0;
    do {
        if ((mask >> i) & 1) {
            func_80036FE0(i);
        }
        i++;
    } while (i < 8);

    mask = getGfAvailabilityMask();
    i = 0;
    do {
        if ((mask >> i) & 1) {
            copyGfHpToSave(i);
        }
        i++;
    } while (i < 16);
}


/** @brief Returns a pointer to global g_gameState.chocobo. */
u8 *getChocoboWorldPtr(void) {
    return &g_gameState.chocobo;
}


/** @brief Sets bit 0x1 in the byte at g_gameState.chocobo. */
void enableChocoboWorld(void) {
    u8 *p = getChocoboWorldPtr();
    *p |= 0x1;
}
