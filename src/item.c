#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gamestate.h"

#define CARD_FLAG_80 0x80

extern u16 D_8005EC3E[];

/**
 * @brief Return a pointer to the global Triple Triad card collection data.
 *
 * @c g_gameState.cards is the same memory as @c g_gameState.cards (offset
 * @c 0x12E0); it is a symbol of its own so accesses emit a clean
 * @c lui + @c addiu instead of folding the @c g_gameState symbol and the
 * @c 0x12E0 offset together.
 */
TripleTriadData *getTripleTriadData(void) {
    return &g_gameState.cards;
}

/**
 * @brief Modify an inventory item quantity or set a key item flag.
 * @param a0 Item slot index. Slots < 0x4D are consumable items; >= 0x4D are key items.
 * @param a1 For consumable items: 0xF0 to increment, anything else to decrement.
 *           For key items: value to set directly.
 * @return 0 on success, -1 if increment/decrement would overflow (max 100) or underflow (min 0).
 * @note Item quantities stored in lower 7 bits (0x7F mask). Upper bit may be a flag.
 */
s32 modifyItemQuantity(s32 a0, s32 a1) {
    TripleTriadData *base = getTripleTriadData();
    u8 *ptr;
    u8 val;

    if (a0 < 0x4D) {
        ptr = &base->cards[a0];
        if (a1 == 0xF0) {
            val = *ptr;
            if ((val & 0x7F) < 100) {
                *ptr = val + 1;
            } else {
                return -1;
            }
        } else {
            val = *ptr;
            if (val & 0x7F) {
                *ptr = val - 1;
            } else {
                return -1;
            }
        }
    } else {
        base->cardLocations[a0 - 0x4D] = (u8)a1;
    }
    return 0;
}


/**
 * @brief Give a Triple Triad card to the player.
 *
 * Marks the card as obtained, then adds one copy of a monster card or moves
 * a rare card to Squall.
 *
 * @param a0 Card ID.
 * @return 0, or -1 when a monster card already has 100 copies.
 */
s32 markItemPresent(s32 a0) {
    TripleTriadData *base = getTripleTriadData();
    if (a0 < 0x4D) {
        base->cards[a0] |= CARD_FLAG_80;
    } else {
        s32 idx = a0 - 0x4D;
        s32 byte_idx = idx / 8;
        base->rareCards[byte_idx] |= 1 << (idx % 8);
    }
    return modifyItemQuantity(a0, 0xF0);
}


/** @brief Wrapper that calls modifyItemQuantity with a1=0. */
s32 decrementItem(s32 a0) {
    return modifyItemQuantity(a0, 0);
}


/**
 * @brief Look up the name string pointer for an item.
 *
 * Uses the D_8005EC3E offset table to compute a pointer to the
 * item's name string. Each table entry is a u16 offset relative
 * to D_8005EC3E - 2.
 *
 * @param itemId Item index (0..0x6D).
 * @return Pointer to item name string, or 0 if itemId >= 0x6E.
 */
u8 *func_80023A54(s32 itemId) {
    if (itemId < 0x6E) {
        return (u8 *)(D_8005EC3E[itemId] + (s32)D_8005EC3E - 2);
    }
    return 0;
}


/**
 * @brief Check if an inventory item is present or a key item bit is set.
 *
 * For item slots < 0x4D: returns 1 if the high bit (0x80) of the card byte is set.
 * For key item slots >= 0x4D: returns 1 if the corresponding bit in the
 * rareCards bitfield is set.
 *
 * @param a0 Inventory slot index.
 * @return 1 if the item/key item is flagged, 0 otherwise.
 */
s32 isItemPresent(s32 a0) {
    TripleTriadData *base = getTripleTriadData();
    s32 byte_idx;
    if (a0 < 0x4D) {
        if (base->cards[a0] & CARD_FLAG_80) {
            return 1;
        }
    } else {
        a0 -= 0x4D;
        byte_idx = a0 / 8;
        if ((base->rareCards[byte_idx] >> (a0 - byte_idx * 8)) & 1) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Get how many copies of a Triple Triad card the player has.
 *
 * For a monster card (below 77) this is the count in the player's collection.
 * For a rare card it is 1 when Squall has it and 0 when someone else does.
 *
 * @param card Card ID.
 * @return The count, or -1 if the player has never had the card.
 */
s32 GetCardCount(s32 card) {
    TripleTriadData *base = getTripleTriadData();
    s32 idx;
    s32 byte_idx;

    if (card < 0x4D) {
        if (base->cards[card] & CARD_FLAG_80) {
            return base->cards[card] & 0x7F;
        }
        return -1;
    }
    idx = card - 0x4D;
    byte_idx = idx / 8;
    if ((base->rareCards[byte_idx] >> (idx % 8)) & 1) {
        return base->cardLocations[card - 0x4D] == 0xF0;
    }
    return -1;
}


/**
 * @brief Sum all positive results from GetCardCount for indices 0..109.
 *
 * Iterates over indices 0 through 0x6D, calling GetCardCount for each.
 * Accumulates the return value only when positive (> 0).
 *
 * @return Sum of all positive GetCardCount results.
 */
s32 sumItemQuantities(void) {
    s32 sum = 0;
    s32 i = sum;
    for (; i < 0x6E; i++) {
        s32 result = GetCardCount(i);
        if (result > 0) {
            sum += result;
        }
    }
    return sum;
}


/**
 * @brief Look up a card location value.
 *
 * Returns the card location byte for slot indices >= 0x4D;
 * returns 0 for regular card slots.
 *
 * @param a0 Card slot index.
 * @return Card location value if a0 >= 0x4D, else 0.
 */
s32 getKeyItemValue(s32 a0) {
    TripleTriadData *base = getTripleTriadData();
    if (a0 >= 0x4D) {
        return base->cardLocations[a0 - 0x4D];
    }
    return 0;
}


/**
 * @brief Check whether the player owns all 110 Triple Triad cards.
 *
 * Walks every common card (77 entries) and every rare card bit (33 entries)
 * to confirm completion. On the first call where every card is owned, sets
 * bit 0 of @c TripleTriadData.pad73 as a "gold star" latch so subsequent
 * calls short-circuit on that flag without re-walking the arrays. The latch
 * drives the gold star that appears next to the @c Card menu entry once
 * the player has collected every card.
 *
 * @return 1 if all 110 cards are (or have ever been) owned, 0 otherwise.
 *
 * @note The latch lives in @c pad73 bit 0, but the function reads and
 *       writes it as a @c u32 at @c rareCards[2] (i.e. @c TripleTriadData
 *       offset @c 0x70, four contiguous bytes covering the upper three
 *       bytes of the rare-card bitmap plus @c pad73). gcc 2.7.2 emits the
 *       check as @c lw @c + @c lui @c 0x0100 @c + @c and — the constant
 *       @c 1 @c << @c 24 fits a single @c lui immediate with no @c ori, so
 *       this is one instruction shorter than @c lbu @c 0x73 @c + @c andi.
 *       The cast is the only way to express that idiom in C; the bytes it
 *       touches at @c 0x70..0x72 are masked off and don't matter.
 */
s32 func_80023C48(void) {
    TripleTriadData *base = getTripleTriadData();
    s32 i;

    if (*(u32 *)&base->rareCards[2] & 0x01000000) {
        return 1;
    }

    for (i = 0; i < 0x4D; i++) {
        if (!(base->cards[i] & CARD_FLAG_80)) {
            return 0;
        }
    }

    for (i = 0; i < 0x21; i++) {
        if (!((base->rareCards[i / 8] >> (i & 7)) & 1)) {
            return 0;
        }
    }

    *(u32 *)&base->rareCards[2] |= 0x01000000;
    return 1;
}


/**
 * @brief Advance the item PRNG and return a pseudo-random value.
 *
 * Uses a Linear Congruential Generator with multiplier 69069 and
 * increment 1. The state is stored in the rngState field of the
 * TripleTriadData structure. Returns the upper 15 bits of the
 * updated state.
 *
 * @return Pseudo-random value in [0, 32767].
 */
s32 func_80023D04(void) {
    TripleTriadData *ptr = getTripleTriadData();
    s32 seed = ptr->rngState;
    ptr->rngState = (seed * 69069) + 1;
    return ((u32)((seed * 69069) + 1)) >> 17;
}
