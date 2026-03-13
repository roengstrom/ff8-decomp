#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern u8 g_tripleTriad;

/** @brief Return a pointer to the global item/key item inventory array g_tripleTriad. */
u8 *func_80023900(void) {
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
s32 func_8002390C(a0, a1)

s32 a0;
s32 a1;
{
    u8 *base = func_80023900();
    u8 *ptr;
    u8 val;

    if (a0 < 0x4D) {
        ptr = &base[a0];
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
        base[a0] = (u8)a1;
    }
    return 0;
}


/**
 * @brief Mark an inventory item as present and set its flag byte to 0xF0.
 *
 * For item slots < 0x4D: sets the high bit (0x80) of the item byte.
 * For key item slots >= 0x4D: sets the corresponding bit in the key item
 * bitfield (starting at offset 0x6E). Then calls func_8002390C(a0, 0xF0).
 *
 * @param a0 Inventory slot index.
 */
void func_800239A8(s32 a0) {
    u8 *base = func_80023900();
    if (a0 < 0x4D) {
        base[a0] |= 0x80;
    } else {
        s32 idx = a0 - 0x4D;
        s32 byte_idx = idx / 8;
        (base + byte_idx)[0x6E] |= 1 << (idx - byte_idx * 8);
    }
    func_8002390C(a0, 0xF0);
}


/** @brief Wrapper that calls func_8002390C with a1=0. */
s32 func_80023A34(s32 a0) {
    return func_8002390C(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/item", func_80023A54);


/**
 * @brief Check if an inventory item is present or a key item bit is set.
 *
 * For item slots < 0x4D: returns 1 if the high bit (0x80) of the item byte is set.
 * For key item slots >= 0x4D: returns 1 if the corresponding bit in the key item
 * bitfield (starting at offset 0x6E) is set.
 *
 * @param a0 Inventory slot index.
 * @return 1 if the item/key item is flagged, 0 otherwise.
 */
s32 func_80023A88(s32 a0) {
    u8 *base = func_80023900();
    s32 byte_idx;
    if (a0 < 0x4D) {
        if (base[a0] & 0x80) {
            return 1;
        }
    } else {
        a0 -= 0x4D;
        byte_idx = a0 / 8;
        if (((base + byte_idx)[0x6E] >> (a0 - byte_idx * 8)) & 1) {
            return 1;
        }
    }
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/item", func_80023B14);


/**
 * @brief Sum all positive results from func_80023B14 for indices 0..109.
 *
 * Iterates over indices 0 through 0x6D, calling func_80023B14 for each.
 * Accumulates the return value only when positive (> 0).
 *
 * @return Sum of all positive func_80023B14 results.
 */
s32 func_80023BB4(void) {
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
 * @brief Look up a byte from func_80023900 result table.
 *
 * Calls func_80023900(a0) to get a base pointer. If a0 >= 0x4D,
 * returns the byte at base[a0]; otherwise returns 0.
 *
 * @param a0 Index value (also passed through to func_80023900).
 * @return Byte at base[a0] if a0 >= 0x4D, else 0.
 */
s32 func_80023C08(s32 a0) {
    u8 *base = func_80023900();
    if (a0 >= 0x4D) {
        return base[a0];
    }
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/item", func_80023C48);


/**
 * @brief Advance the item PRNG and return a pseudo-random value.
 *
 * Uses a Linear Congruential Generator with multiplier 69069 and
 * increment 1. The state is stored at offset +0x7C of the structure
 * returned by func_80023900. Returns the upper 15 bits of the
 * updated state.
 *
 * @return Pseudo-random value in [0, 32767].
 *
 * @note Non-matching: Compiler allocates v1 for pointer (original uses a0).
 * All multiply-by-69069 decomposition and epilogue scheduling match correctly.
 */
INCLUDE_ASM("asm/nonmatchings/item", func_80023D04);


INCLUDE_ASM("asm/nonmatchings/item", func_80023D60);


INCLUDE_ASM("asm/nonmatchings/item", func_80024064);


INCLUDE_ASM("asm/nonmatchings/item", func_800242C8);


