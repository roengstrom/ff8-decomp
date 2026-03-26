#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "overlay.h"

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
 * @param a0 Card ID.
 * @return Rarity level (0-6).
 */
s32 getCardRarity(s32 a0) {
    s32 result;
    if (a0 < 0x14) {
        result = 0;
    } else if (a0 < 0x27) {
        result = 1;
    } else if (a0 < 0x3A) {
        result = 2;
    } else if (a0 < 0x4E) {
        result = 3;
    } else if (a0 < 0x53) {
        result = 4;
    } else if (a0 < 0x5C) {
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
 * Writes a0 to g_gameState+0xAF4 (party slot 0), sets slots 1 and 2 to
 * 0xFF (empty), then calls recalcPartyStats to apply the party change.
 *
 * @param a0 Character ID for the party leader.
 */
void setPartyLeader(s32 a0) {
    extern u8 g_gameState[];
    g_gameState[0xAF4] = a0;      /* party.party[0] = leader */
    g_gameState[0xAF5] = 0xFF;    /* party.party[1] = empty */
    g_gameState[0xAF6] = 0xFF;    /* party.party[2] = empty */
    recalcPartyStats();
}


INCLUDE_ASM("asm/nonmatchings/card", func_80036EC0);


/**
 * @brief Build a 16-bit bitmask of GF availability flags.
 *
 * Iterates over 16 GF entries in g_gameState (stride 0x44 = 68 bytes),
 * checking bit 0 of byte +0x61 (availability flag). Sets the corresponding
 * bit in the result mask for each available GF.
 *
 * @return Bitmask where bit N is set if GF N is available.
 */
u16 getGfAvailabilityMask(void) {
    extern u8 g_gameState[];
    u16 mask;
    s32 i;
    s32 bit;
    u8 *p;

    mask = 0;
    i = 0;
    bit = 1;
    p = g_gameState;
    do {
        if (p[0x61] & 1) {
            mask |= (bit << i);
        }
        p += 0x44;
        i++;
    } while (i < 16);
    return mask;
}


/**
 * @brief Copy a GF's current HP from the character table to the GF table.
 *
 * Reads a u16 from g_battleChars + a0*12 + 0x61A and writes it to
 * g_gameState + a0*68 + 0x62.
 *
 * @param a0 GF index (0-15).
 */
void copyGfHpToSave(s32 a0) {
    extern u8 g_gameState[];
    s32 base1 = (s32)g_gameState;
    s32 off1 = a0 * 68;
    s32 base2 = (s32)g_battleChars;
    *(s16 *)(base1 + off1 + 0x62) = *(u16 *)(base2 + a0 * 12 + 0x61A);
}


INCLUDE_ASM("asm/nonmatchings/card", func_80036FE0);


