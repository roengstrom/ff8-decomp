#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/entity", func_8003228C);


INCLUDE_ASM("asm/nonmatchings/entity", func_80032350);


INCLUDE_ASM("asm/nonmatchings/entity", func_80032534);


INCLUDE_ASM("asm/nonmatchings/entity", func_80032688);


/**
 * @brief Poll func_800239A8 with retries until it returns a non-zero result.
 *
 * Masks the input to 8 bits and repeatedly calls func_800239A8 up to the
 * specified number of attempts. Returns the first non-zero result, or 0
 * if all attempts fail.
 *
 * @param a0 Entity or resource identifier (low 8 bits used).
 * @param retries Maximum number of poll attempts.
 * @return Non-zero result from func_800239A8, or 0 on timeout.
 */
s32 func_800327B8(s32 a0, s32 retries) {
    s32 result = 0;
    a0 &= 0xFF;
    while (retries > 0) {
        result = func_800239A8(a0);
        if (result != 0) {
            break;
        }
        retries--;
    }
    return result;
}


/**
 * @brief Guarded entity lookup: return 0 if bit 8 is set, otherwise call func_80020D4C.
 *
 * Checks whether bit 0x100 is set in the input. If so, returns 0 immediately
 * (invalid/out-of-range entity). Otherwise delegates to func_80020D4C for the
 * actual lookup.
 *
 * @param a0 Entity index or ID to look up.
 * @return 0 if bit 8 is set, otherwise result of func_80020D4C(a0).
 */
s32 func_80032808(s32 a0) {
    if (a0 & 0x100) return 0;
    return func_80020D4C(a0);
}


INCLUDE_ASM("asm/nonmatchings/entity", func_8003283C);


