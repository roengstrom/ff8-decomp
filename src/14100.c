#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern u8 D_80078658;

/** @brief Return a pointer to the global item/key item inventory array D_80078658. */
u8 *func_80023900(void) {
    return &D_80078658;
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


INCLUDE_ASM("asm/nonmatchings/14100", func_800239A8);


/** @brief Wrapper that calls func_8002390C with a1=0. */
s32 func_80023A34(s32 a0) {
    return func_8002390C(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/14100", func_80023A54);


INCLUDE_ASM("asm/nonmatchings/14100", func_80023A88);


INCLUDE_ASM("asm/nonmatchings/14100", func_80023B14);


INCLUDE_ASM("asm/nonmatchings/14100", func_80023BB4);


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


INCLUDE_ASM("asm/nonmatchings/14100", func_80023C48);


INCLUDE_ASM("asm/nonmatchings/14100", func_80023D04);


INCLUDE_ASM("asm/nonmatchings/14100", func_80023D60);


INCLUDE_ASM("asm/nonmatchings/14100", func_80024064);


INCLUDE_ASM("asm/nonmatchings/14100", func_800242C8);


