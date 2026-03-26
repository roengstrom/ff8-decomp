#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/entity", func_8003228C);


INCLUDE_ASM("asm/nonmatchings/entity", func_80032350);


INCLUDE_ASM("asm/nonmatchings/entity", func_80032534);


INCLUDE_ASM("asm/nonmatchings/entity", func_80032688);


/**
 * @brief Poll markItemPresent with retries until it returns a non-zero result.
 *
 * Masks the input to 8 bits and repeatedly calls markItemPresent up to the
 * specified number of attempts. Returns the first non-zero result, or 0
 * if all attempts fail.
 *
 * @param a0 Entity or resource identifier (low 8 bits used).
 * @param retries Maximum number of poll attempts.
 * @return Non-zero result from markItemPresent, or 0 on timeout.
 */
s32 pollItemPresent(s32 a0, s32 retries) {
    s32 result = 0;
    a0 &= 0xFF;
    while (retries > 0) {
        result = markItemPresent(a0);
        if (result != 0) {
            break;
        }
        retries--;
    }
    return result;
}


/**
 * @brief Guarded entity lookup: return 0 if bit 8 is set, otherwise call getStatDesc.
 *
 * Checks whether bit 0x100 is set in the input. If so, returns 0 immediately
 * (invalid/out-of-range entity). Otherwise delegates to getStatDesc for the
 * actual lookup.
 *
 * @param a0 Entity index or ID to look up.
 * @return 0 if bit 8 is set, otherwise result of getStatDesc(a0).
 */
s32 guardedEntityLookup(s32 a0) {
    if (a0 & 0x100) return 0;
    return getStatDesc(a0);
}


INCLUDE_ASM("asm/nonmatchings/entity", func_8003283C);


