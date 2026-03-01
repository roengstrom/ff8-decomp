#include "common.h"


INCLUDE_ASM("asm/nonmatchings/psxsdk/libgcc", func_8002B3A0);


/**
 * @brief Performs a 64-bit modulo operation (unsigned remainder).
 *
 * Wrapper around the internal 64-bit division routine func_8002B3A0, passing
 * mode 3 to request the remainder. Used by GCC-generated code for the %
 * operator on 64-bit (long long) values.
 *
 * @param a0 Low 32 bits of the dividend.
 * @param a1 High 32 bits of the dividend.
 * @param a2 Low 32 bits of the divisor.
 * @param a3 High 32 bits of the divisor.
 * @return The 64-bit remainder (low 32 bits in return value, high in $v1).
 */
s32 func_8002B898(s32 a0, s32 a1, s32 a2, s32 a3) {
    return func_8002B3A0(a0, a1, a2, a3, 3);
}


INCLUDE_ASM("asm/nonmatchings/psxsdk/libgcc", func_8002B8BC);


INCLUDE_ASM("asm/nonmatchings/psxsdk/libgcc", __udivdi3);
