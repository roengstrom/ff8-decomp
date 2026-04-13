#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libc.h"
#include "battle.h"

extern u8 D_8008386C;

extern u8 D_80052A30[];
extern u8 D_800773A8[];
extern u8 D_80077E74[];
extern u8 D_800773B4[];
extern u8 D_80083857[];
extern u8 D_80083858;
extern u8 D_80083868;
extern u8 D_8008369C[];
extern SfxSystem g_sfxEntries;
extern u8 *getMagicNamePtr(s32 magicId);
extern s32 getStatName(s32 statId);
extern u8 *getBattleCharNameWrapper(s32 entityIdx);
extern u8 *getCharNamePtrWrapper(s32 charId);
extern u8 *getCharNamePtrWrapper2(s32 charId);
extern u8 getDigitBaseCode(void);
extern void copyString(u8 *dst, u8 *src);
extern s32 btlStrlen(u8 *str);
extern void func_8002F4B0(u8 *buf, s32 separator);

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
void intToDecString(u32 a0, u8 *a1, s32 a2) {
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
 * Same algorithm as intToDecString but uses a different (likely shorter) divisor table,
 * producing fewer digits. Used for smaller number ranges.
 *
 * @param a0 The unsigned integer value to convert.
 * @param a1 Destination buffer for the digit string (null-terminated on output).
 * @param a2 Base character code added to each digit.
 */
void intToDecStringShort(u32 a0, u8 *a1, s32 a2) {
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
void replaceLeadingZeros(u8 *a0, s32 a1, s32 a2, s32 a3) {
    s32 i;
    for (i = 0; i < a1; i++) {
        if (*a0 != a2) return;
        *a0++ = a3;
    }
}


/**
 * @brief Strip up to @p count leading occurrences of @p ch from a string.
 *
 * Scans the first @p count bytes of @p str, skipping those equal to @p ch.
 * Copies the remainder (from the first non-matching byte) back to the start
 * of @p str and null-terminates.
 *
 * @param str String to modify in place.
 * @param count Maximum number of leading characters to strip.
 * @param ch Character value to strip.
 */
void func_8002F320(u8 *str, s32 count, s32 ch) {
    u8 *src = str;
    u8 *dst = str;
    s32 i;

    for (i = 0; i < count; i++) {
        src = str;
        str++;
        if (*src != ch)
            break;
    }

    while (*src != 0) {
        *dst++ = *src++;
    }

    *dst = 0;
}


extern u8 D_80052A20[];
/**
 * @brief Look up a single hex digit character from the table "0123456789ABCDEF".
 * @param idx Value 0-15 selecting the hex digit (masked to low nibble).
 * @param dst Pointer where the resulting ASCII character is stored.
 */
void lookupHexChar(s32 idx, u8 *dst) {
    u8 *base = D_80052A20;
    *dst = base[idx & 0xF];
}


/**
 * @brief Convert a byte value to a 2-character null-terminated hex string.
 * @param byte The byte value to convert (high nibble first, then low nibble).
 * @param buf Destination buffer (must hold at least 3 bytes).
 */
void byteToHexString(s32 byte, u8 *buf) {
    lookupHexChar(byte >> 4, buf++);
    lookupHexChar(byte & 0xF, buf++);
    *buf = 0;
}


/**
 * @brief Convert a 16-bit value to a 2-character hex string via byteToHexString.
 *
 * Splits the 16-bit value into high and low bytes, converts each to a
 * 2-character hex string, and null-terminates the result.
 *
 * @param a0 16-bit value to convert.
 * @param a1 Output buffer (at least 5 bytes).
 */
void u16ToHexString(s32 a0, u8 *a1) {
    byteToHexString(a0 >> 8, a1);
    a1 += 2;
    byteToHexString(a0 & 0xFF, a1);
    a1[2] = 0;
}


/**
 * @brief Convert a 32-bit value to a 4-character hex string via u16ToHexString.
 *
 * Splits the 32-bit value into high and low 16-bit halves, converts each
 * to a 4-character hex string, and null-terminates the result.
 *
 * @param a0 32-bit value to convert.
 * @param a1 Output buffer (at least 9 bytes).
 */
void u32ToHexString(s32 a0, u8 *a1) {
    u16ToHexString(a0 >> 16, a1);
    a1 += 4;
    u16ToHexString(a0 & 0xFFFF, a1);
    a1[4] = 0;
}


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
void u32ToHexTiles(u32 val, u8 *dst, s32 base_char) {
    s32 shift = 28;
    do {
        *dst++ = ((val >> shift) & 0xF) + base_char;
        shift -= 4;
    } while (shift >= 0);
    *dst = 0;
}


INCLUDE_ASM("asm/nonmatchings/numstr", func_8002F4B0);


/**
 * @brief Advance through a control-coded string to the next segment.
 *
 * Scans @p str byte-by-byte, handling control codes:
 *  - 0: end of string (returns NULL).
 *  - 1: line break (returns pointer past it).
 *  - 2: page break (returns pointer past it).
 *  - 6: color code (reads next byte into D_8008386C, then continues).
 *  - 7: section end (returns pointer past it).
 *
 * @param str Pointer to control-coded string, or NULL.
 * @return Pointer to the next unprocessed byte, or NULL on end/null input.
 */
u8 *func_8002F548(u8 *str) {
    s32 ch;
    u8 *colorPtr;

    if (str == 0)
        return 0;

    colorPtr = &D_8008386C;

    do {
        ch = *str++;

        if (ch == 2)
            return str;

        if (ch == 6)
            *colorPtr = *str++;

        if (ch == 1)
            return str;

        if (ch == 7)
            return str;
    } while (ch != 0);

    return 0;
}


/**
 * @brief Advance through a control-coded string, processing embedded commands.
 *
 * Scans @p str byte-by-byte, handling control codes:
 *  - 0: end of string (returns NULL).
 *  - 1: line/segment break (returns pointer past the break).
 *  - 6: color code (reads next byte into D_8008386C, then continues).
 *  - 7: section end (returns pointer past it). 
 *
 * @param str Pointer to control-coded string, or NULL.
 * @return Pointer to the next unprocessed byte, or NULL on end/null input.
 */
u8 *func_8002F5B4(u8 *str) {
    s32 ch;
    u8 *colorPtr;

    if (str == 0)
        return 0;

    colorPtr = &D_8008386C;

    do {
        ch = *str++;

        if (ch == 6)
            *colorPtr = *str++;

        if (ch == 0)
            return 0;

        if (ch == 1)
            return str;
    } while (ch != 7);

    return str;
}


/**
 * @brief Look up a string by index from the global string table and copy it.
 *
 * Reads the string table pointer from D_800834CC. If null or index out of
 * range, returns dst unchanged. Otherwise copies the indexed string to dst
 * via copyString and returns dst + length.
 *
 * @param index Index into the string table.
 * @param dst Destination buffer for the copied string.
 * @return Pointer past the end of the copied string, or dst if not found.
 */
u8 *func_8002F610(s32 index, u8 *dst) {
    extern s32 D_800834CC;
    u16 *table = (u16 *)D_800834CC;
    u8 *src;
    if (table == 0) {
        return dst;
    }
    if (index >= table[0]) {
        return dst;
    }
    src = (u8 *)table + table[index + 1];
    copyString(dst, src);
    return dst + btlStrlen(src);
}


/**
 * @brief Decode a control-code-encoded text string into an output buffer.
 *
 * Processes an input byte stream containing printable characters (0x19-0xE7)
 * and embedded control codes. Writes decoded text to the output buffer.
 *
 * Control codes:
 *   0x00, 0x01, 0x02, 0x07 — String terminators
 *   0x03 + byte — Two-byte name/string reference (type 3: names, locations)
 *   0x04 + byte — Two-byte numeric value format (type 4: SFX entry values)
 *   0x05-0x06, 0x08-0x0B + byte — Escape: next byte stored literally
 *   0x0C + byte — Magic spell name lookup via getMagicNamePtr
 *   0x0D + byte — GF/item stat name lookup via getStatName
 *   0x0E + byte — Character name table set 0 (idx * 224 + subByte)
 *   0x0F + byte — Character name table set 1 (idx * 224 + subByte)
 *   0x10-0x18   — Direct name lookup via getBattleCharNameWrapper (type 0)
 *   0xE8-0xFF   — Double-byte character from D_8008369C lookup table
 *
 * Types 3, 4, and 0x10-0x18 share a sub-command dispatch based on cmd >> 8:
 *   hiCmd 0: getBattleCharNameWrapper(cmd & 0x1F) — direct name pointer
 *   hiCmd 3: Name lookup switch (65-entry jump table, 0x20-0x60):
 *     0x20-0x22 → getCharNamePtrWrapper2 (character name type A)
 *     0x30-0x3F → getCharNamePtrWrapper (character name type B)
 *     0x40 → D_800773A8, 0x5C → D_80077E74, 0x60 → D_800773B4
 *     default  → D_80052A30
 *   hiCmd 4: SFX numeric format switch (40-entry jump table, 0x20-0x47):
 *     0x20-0x27 → Decimal with separator (intToDecString + F320 + F4B0)
 *     0x30-0x37 → Decimal plain (intToDecString + F320)
 *     0x40-0x47 → Hex with D_80083857 char remap (u32ToHexTiles)
 *
 * The dispatch is repeated 3 times (for types 3, 4, 0x10-0x18), producing
 * 6 separate jump tables. Handler code is shared across dispatches via
 * cross-jumping, except the hex remap handler (contains a loop, 3 copies).
 *
 * After dispatch, the result string is copied to writePos via copyString,
 * and the output pointer advances by the string length.
 *
 * Overflow: if output >= end, writes null to *end. If maxLen < 0, prints
 * "MESSAGE DATA OVER RUN" via printf before returning.
 *
 * @param input   Source byte stream with embedded control codes.
 * @param output  Destination buffer for decoded text.
 * @param maxLen  Maximum output length, or -1 for default limit (128 bytes).
 * @see https://decomp.me/scratch/7Ruwy
 */
INCLUDE_ASM("asm/nonmatchings/numstr", decodeMessage);


/**
 * @brief Skip past control codes in the message stream and decode the remaining message.
 *
 * Reads skipCount and streamPtr from the MsgState structure.
 * Calls func_8002F548 skipCount times to advance past that many type-2
 * delimiters. Then calls decodeMessage with the current position and stores
 * the result in storedPtr.
 *
 * @param a0 Pointer to the message state structure.
 * @param a1 Output buffer for decodeMessage.
 */
void func_8002FD28(s32 *a0, u8 *a1) {
    s32 skip = ((MsgState *)a0)->skipCount;
    u8 *stream = (u8 *)((MsgState *)a0)->streamPtr;
    while (skip > 0) {
        stream = func_8002F548(stream);
        skip--;
    }
    decodeMessage((s32)stream, (s32)a1, -1);
    ((MsgState *)a0)->storedPtr = (s32)stream;
}


/**
 * @brief Process and dispatch a rendering command.
 *
 * Calls func_8002F548 on the fourth element of the input array to update it,
 * then dispatches the result along with a1 and -1 via decodeMessage.
 *
 * @param a0 Pointer to a 4-element s32 array; element [3] is processed in-place.
 * @param a1 Second parameter passed to the dispatch function.
 * @note Purpose uncertain -- appears to advance and render a display list primitive.
 */
void advanceAndDecodeMessage(s32 *a0, s32 a1) {
    s32 result = func_8002F548(a0[3]);
    a0[3] = result;
    decodeMessage(result, a1, -1);
}


/**
 * @brief Dispatch a rendering command without processing.
 *
 * Calls decodeMessage directly with the fourth element of the array, a1, and -1.
 * Unlike advanceAndDecodeMessage, does not call func_8002F548 to update element [3] first.
 *
 * @param a0 Pointer to a 4-element s32 array; element [3] is read but not modified.
 * @param a1 Second parameter passed to the dispatch function.
 */
void decodeMessageDirect(s32 *a0, s32 a1) {
    decodeMessage(a0[3], a1, -1);
}


INCLUDE_ASM("asm/nonmatchings/numstr", func_8002FE0C);


