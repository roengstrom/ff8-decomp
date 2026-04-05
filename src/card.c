#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "overlay.h"
#include "gamestate.h"
#include "character.h"

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


INCLUDE_ASM("asm/nonmatchings/card", func_80036710);


INCLUDE_ASM("asm/nonmatchings/card", func_8003678C);


INCLUDE_ASM("asm/nonmatchings/card", func_8003685C);


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
    u16 mask;
    s32 i;
    s32 bit;
    u8 *p;

    mask = 0;
    i = 0;
    bit = 1;
    p = (u8 *)&g_gameState;
    do {
        if (p[0x61] & 1) { /* gfs[i].exists (0x50 + 0x11) */
            mask |= (bit << i);
        }
        p += sizeof(GfSaveData);
        i++;
    } while (i < 16);
    return mask;
}


/**
 * @brief Copy a GF's current HP from the battle character table to the save data.
 *
 * Reads the GF's HP from g_battleChars and writes it to the GF's save entry.
 *
 * @param gfIdx GF index (0-15).
 */
void copyGfHpToSave(s32 gfIdx) {
    s32 base1 = (s32)&g_gameState;
    s32 off1 = gfIdx * sizeof(GfSaveData);
    s32 base2 = (s32)g_battleChars;
    *(s16 *)(base1 + off1 + 0x62) = *(u16 *)(base2 + gfIdx * 12 + 0x61A);
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


