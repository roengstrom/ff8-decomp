#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gamestate.h"

extern TripleTriadData g_tripleTriad;
extern u16 D_8005EC3E[];

/** @brief Return a pointer to the global Triple Triad / inventory data structure. */
TripleTriadData *getInventoryPtr(void) {
    return &g_tripleTriad;
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
    TripleTriadData *base = getInventoryPtr();
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
 * @brief Mark an inventory item as present and set its flag byte to 0xF0.
 *
 * For item slots < 0x4D: sets the high bit (0x80) of the card byte.
 * For key item slots >= 0x4D: sets the corresponding bit in the
 * rareCards bitfield. Then calls modifyItemQuantity(a0, 0xF0).
 *
 * @param a0 Inventory slot index.
 */
void markItemPresent(s32 a0) {
    TripleTriadData *base = getInventoryPtr();
    if (a0 < 0x4D) {
        base->cards[a0] |= 0x80;
    } else {
        s32 idx = a0 - 0x4D;
        s32 byte_idx = idx / 8;
        base->rareCards[byte_idx] |= 1 << (idx - byte_idx * 8);
    }
    modifyItemQuantity(a0, 0xF0);
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
    TripleTriadData *base = getInventoryPtr();
    s32 byte_idx;
    if (a0 < 0x4D) {
        if (base->cards[a0] & 0x80) {
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
 * @brief Get the quantity of an item, or check if a key item is present.
 *
 * For consumable items (index < 0x4D): returns the quantity (lower 7 bits)
 * if the item is marked present (high bit set), or -1 if not present.
 * For key items (index >= 0x4D): checks the key item bitfield. If the
 * bit is set, returns whether the item byte equals 0xF0 (1 or 0).
 * Returns -1 if the key item bit is not set.
 *
 * @param a0 Item slot index.
 * @return Item quantity, 0/1 for key items, or -1 if not present.
 */
s32 func_80023B14(s32 a0) {
    TripleTriadData *base = getInventoryPtr();
    s32 idx;
    s32 byte_idx;

    if (a0 < 0x4D) {
        if (base->cards[a0] & 0x80) {
            return base->cards[a0] & 0x7F;
        }
        return -1;
    }
    idx = a0 - 0x4D;
    byte_idx = idx / 8;
    if ((base->rareCards[byte_idx] >> (idx - byte_idx * 8)) & 1) {
        return base->cardLocations[a0 - 0x4D] == 0xF0;
    }
    return -1;
}


/**
 * @brief Sum all positive results from func_80023B14 for indices 0..109.
 *
 * Iterates over indices 0 through 0x6D, calling func_80023B14 for each.
 * Accumulates the return value only when positive (> 0).
 *
 * @return Sum of all positive func_80023B14 results.
 */
s32 sumItemQuantities(void) {
    s32 sum = 0;
    s32 i = sum;
    for (; i < 0x6E; i++) {
        s32 result = func_80023B14(i);
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
    TripleTriadData *base = getInventoryPtr();
    if (a0 >= 0x4D) {
        return base->cardLocations[a0 - 0x4D];
    }
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/item", func_80023C48);


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
    TripleTriadData *ptr = getInventoryPtr();
    s32 seed = ptr->rngState;
    ptr->rngState = (seed * 69069) + 1;
    return ((u32)((seed * 69069) + 1)) >> 17;
}


INCLUDE_ASM("asm/nonmatchings/item", func_80023D60);


INCLUDE_ASM("asm/nonmatchings/item", func_80024064);


INCLUDE_ASM("asm/nonmatchings/item", func_800242C8);


