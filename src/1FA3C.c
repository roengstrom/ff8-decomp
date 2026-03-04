#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/**
 * @brief Convert an unsigned integer to a decimal digit string using divisor table D_800529F4.
 *
 * Repeatedly divides the input by decreasing powers of 10 (from the divisor table),
 * producing one digit character per divisor. Each digit is offset by the base character
 * code so the output can be used for custom font rendering.
 *
 * @param a0 The unsigned integer value to convert.
 * @param a1 Destination buffer for the digit string (null-terminated on output).
 * @param a2 Base character code added to each digit (e.g. tile index for '0').
 */
void func_8002F23C(u32 a0, u8 *a1, s32 a2) {
    extern u32 D_800529F4[];
    u32 *t = D_800529F4;
    u32 d = *t++;

    while (d != 0) {
        s32 digit = 0;
        while (a0 >= d) {
            a0 -= d;
            digit++;
        }
        digit += a2;
        *a1++ = digit;
        d = *t++;
    }
    *a1 = 0;
}


/**
 * @brief Convert an unsigned integer to a decimal digit string using divisor table D_80052A08.
 *
 * Same algorithm as func_8002F23C but uses a different (likely shorter) divisor table,
 * producing fewer digits. Used for smaller number ranges.
 *
 * @param a0 The unsigned integer value to convert.
 * @param a1 Destination buffer for the digit string (null-terminated on output).
 * @param a2 Base character code added to each digit.
 */
void func_8002F294(u32 a0, u8 *a1, s32 a2) {
    extern u32 D_80052A08[];
    u32 *t = D_80052A08;
    u32 d = *t++;

    while (d != 0) {
        s32 digit = 0;
        while (a0 >= d) {
            a0 -= d;
            digit++;
        }
        digit += a2;
        *a1++ = digit;
        d = *t++;
    }
    *a1 = 0;
}


/**
 * @brief Replace leading characters in a digit string with a fill character.
 *
 * Scans up to a1 bytes from the start of the buffer. While the current byte
 * matches a2 (the "zero" digit character), it is replaced with a3 (e.g. a space
 * or blank tile). Stops at the first non-matching byte.
 *
 * @param a0 Pointer to the digit string buffer.
 * @param a1 Maximum number of leading characters to check.
 * @param a2 Character code to match (typically the '0' tile index).
 * @param a3 Replacement character code (typically a space/blank tile index).
 */
void func_8002F2EC(u8 *a0, s32 a1, s32 a2, s32 a3) {
    s32 i;
    for (i = 0; i < a1; i++) {
        if (*a0 != a2) return;
        *a0++ = a3;
    }
}


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F320);


extern u8 D_80052A20[];
/**
 * @brief Look up a single hex digit character from the table "0123456789ABCDEF".
 * @param idx Value 0-15 selecting the hex digit (masked to low nibble).
 * @param dst Pointer where the resulting ASCII character is stored.
 */
void func_8002F384(s32 idx, u8 *dst) {
    u8 *base = D_80052A20;
    *dst = base[idx & 0xF];
}


/**
 * @brief Convert a byte value to a 2-character null-terminated hex string.
 * @param byte The byte value to convert (high nibble first, then low nibble).
 * @param buf Destination buffer (must hold at least 3 bytes).
 */
void func_8002F3A0(s32 byte, u8 *buf) {
    func_8002F384(byte >> 4, buf++);
    func_8002F384(byte & 0xF, buf++);
    *buf = 0;
}


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F3F0);


// u32_to_hex_string - converts u32 to 8-char hex string with configurable base char

INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F43C);


/**
 * @brief Convert a 32-bit unsigned integer to an 8-character hex string.
 *
 * Extracts nibbles from most-significant to least-significant and adds
 * the base character code to produce font-specific digit characters.
 *
 * @param val The 32-bit value to convert.
 * @param dst Destination buffer (must hold at least 9 bytes for 8 chars + null).
 * @param base_char Character code for digit '0' (each nibble is added to this).
 */
void func_8002F488(u32 val, u8 *dst, s32 base_char) {
    s32 shift = 28;
    do {
        *dst++ = ((val >> shift) & 0xF) + base_char;
        shift -= 4;
    } while (shift >= 0);
    *dst = 0;
}


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F4B0);


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F548);


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F5B4);


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F610);


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002F688);


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002FD28);


/**
 * @brief Process and dispatch a rendering command.
 *
 * Calls func_8002F548 on the fourth element of the input array to update it,
 * then dispatches the result along with a1 and -1 via func_8002F688.
 *
 * @param a0 Pointer to a 4-element s32 array; element [3] is processed in-place.
 * @param a1 Second parameter passed to the dispatch function.
 * @note Purpose uncertain -- appears to advance and render a display list primitive.
 */
void func_8002FD9C(s32 *a0, s32 a1) {
    s32 result = func_8002F548(a0[3]);
    a0[3] = result;
    func_8002F688(result, a1, -1);
}


/**
 * @brief Dispatch a rendering command without processing.
 *
 * Calls func_8002F688 directly with the fourth element of the array, a1, and -1.
 * Unlike func_8002FD9C, does not call func_8002F548 to update element [3] first.
 *
 * @param a0 Pointer to a 4-element s32 array; element [3] is read but not modified.
 * @param a1 Second parameter passed to the dispatch function.
 */
void func_8002FDE8(s32 *a0, s32 a1) {
    func_8002F688(a0[3], a1, -1);
}


INCLUDE_ASM("asm/nonmatchings/1FA3C", func_8002FE0C);


