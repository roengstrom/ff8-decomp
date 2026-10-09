#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libc.h"
#include "battle.h"
#include "ui/window.h"
#include "ui/font.h"
#include "ui/dialog.h"
#include "game.h"
#include "gamestate.h"
#include "numstr.h"
#include "btl_anim.h"

extern u8 D_80052A30[];
extern u8 D_8008369C[];
extern u8 *getMagicNamePtr(s32 magicId);
extern u8 *getBattleCharNameWrapper(s32 entityIdx);
extern u8 *getCharNameWrapper(s32 charId);
extern u8 *getCharNameWrapper2(s32 charId);
extern void copyString(u8 *dst, u8 *src);
extern u32 D_800529F4[];
extern u32 D_80052A08[];

/** @brief Reference kinds carried in bits 8 and up of an insertArgString code. */
enum {
    MSG_ARG_BATTLE_CHAR = 0,    /* low 5 bits: battle character slot */
    MSG_ARG_NAME = 3,           /* low byte: character, Angelo, Griever or Boko name */
    MSG_ARG_NUMBER = 4 /* low byte: format and message value slot */
};

static inline u8 *appendString(u8 *dst, u8 *str);
static inline u8 *getNameString(s32 code, u8 *buf);
static inline u8 *getNumberString(s32 code, u8 *buf);
static inline u8 *insertArgString(u8 *dst, s32 code, u8 *buf);
static u8 *nextMessagePage(u8 *str);
static void func_8002F4B0(u8 *buf, s32 separator);

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


/**
 * @brief Put a separator between every three digits of a number string.
 *
 * Works in place from the end: the terminator moves to the new end, then each
 * group of three digits after the first moves right with @p separator in
 * front of it.
 *
 * @param buf Digit string, with room for the separators.
 * @param separator Character put between the groups.
 */
static void func_8002F4B0(u8 *buf, s32 separator) {
    u8 *src;
    u8 *dst;
    s32 len;
    s32 n;

    len = btlStrlen(buf);
    n = (len - 1) / 3;
    src = buf + len;
    dst = src + n;
    *dst = 0;
    /* Load-bearing: n counts down (an ascending index does not match), and dst
     * steps before src (gcc sets the two pointers up in the reverse order). */
    for (; n > 0; n--) {
        dst[-1] = src[-1];
        dst[-2] = src[-2];
        dst[-3] = src[-3];
        dst[-4] = separator;
        dst -= 4;
        src -= 3;
    }
}


/**
 * @brief Step to the next line of a message.
 *
 * Scans past the next newline (2) or page break (1 or 7). A colour command (6)
 * on the way stores its argument in @c g_messageColor.
 *
 * @param str Message in the game's encoding, or NULL.
 * @return The first byte of the next line, or NULL at the end of the message.
 */
u8 *nextMessageLine(u8 *str) {
    s32 ch;
    u8 *colorPtr;

    if (str == NULL)
        return NULL;

    colorPtr = &g_messageColor;

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

    return NULL;
}


/**
 * @brief Step to the next page of a message.
 *
 * Scans past the next page break (1 or 7); newlines do not stop it. A colour
 * command (6) on the way stores its argument in @c g_messageColor.
 *
 * @param str Message in the game's encoding, or NULL.
 * @return The first byte of the next page, or NULL at the end of the message.
 */
static u8 *nextMessagePage(u8 *str) {
    s32 ch;
    u8 *colorPtr;

    if (str == NULL)
        return NULL;

    colorPtr = &g_messageColor;

    do {
        ch = *str++;

        if (ch == 6)
            *colorPtr = *str++;

        if (ch == 0)
            return NULL;

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
 * @brief Copy @p str to @p dst and return the position just past the copy.
 *
 * @param dst Destination write position.
 * @param str Null-terminated string to copy.
 * @return @p dst advanced by the length of @p str.
 */
static inline u8 *appendString(u8 *dst, u8 *str) {
    copyString(dst, str);
    return dst + btlStrlen(str);
}

/**
 * @brief Resolve a name reference code to its string.
 *
 * MSG_ARG_BATTLE_CHAR is a battle character slot. MSG_ARG_NAME selects by the
 * low byte a character name (0x20-0x22, 0x30-0x3F), the player-chosen Angelo
 * (0x40), Griever (0x50) or Boko (0x60) name, or D_80052A30 for anything
 * else. Any other kind leaves @p buf untouched.
 *
 * @param code Reference code: kind in bits 8+, selector in the low byte.
 * @param buf  Caller's scratch buffer, returned when nothing is looked up.
 * @return The resolved string, or @p buf.
 *
 * @note Every arm assigns through @p buf and the function has a single
 *       return. A parameter that is written to makes gcc copy the argument
 *       into a fresh pseudo when it expands the inline, which is the
 *       `v0 = s1` copy at the head of each expansion; the single return is
 *       what gives the four direct-pointer arms their one shared `s1 = v0`.
 */
static inline u8 *getNameString(s32 code, u8 *buf) {
    s32 low;

    low = code & 0xFF;
    switch (code >> 8) {
    case MSG_ARG_BATTLE_CHAR:
        buf = getBattleCharNameWrapper(code & 0x1F);
        break;
    case MSG_ARG_NAME:
        switch (low) {
        case 0x20: case 0x21: case 0x22:
            buf = getCharNameWrapper2((low - 0x20) & 0x1F);
            break;
        case 0x30: case 0x31: case 0x32: case 0x33:
        case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3A: case 0x3B:
        case 0x3C: case 0x3D: case 0x3E: case 0x3F:
            buf = getCharNameWrapper((low - 0x30) & 0x1F);
            break;
        case 0x40:
            buf = g_gameState.angeloName;
            break;
        case 0x50:
            buf = g_gameState.mainData.party.grieverName;
            break;
        case 0x60:
            buf = g_gameState.bokoName;
            break;
        default:
            buf = D_80052A30;
            break;
        }
        break;
    }
    return buf;
}

/**
 * @brief Format one of the dialogs' message values into @p buf.
 *
 * The low byte of @p code picks both the format and the value slot:
 * 0x20-0x27 decimal with thousands separator, 0x30-0x37 plain decimal,
 * 0x40-0x47 hexadecimal. u32ToHexTiles is asked for tiles 1-16 so that a zero
 * nibble does not terminate the string, which is why the glyph table is
 * indexed from one entry before its start.
 *
 * @param code Reference code; only the low byte is used.
 * @param buf  Buffer that receives the digits (left empty for other codes).
 * @return @p buf.
 */
static inline u8 *getNumberString(s32 code, u8 *buf) {
    DialogSystem *dialogs;
    s32 valIdx;
    u8 *hexPtr;

    dialogs = &g_dialogs;
    getDigitBaseCode();
    valIdx = code & 0xFF;
    *buf = 0;
    switch (valIdx) {
    case 0x20: case 0x21: case 0x22: case 0x23:
    case 0x24: case 0x25: case 0x26: case 0x27:
        valIdx -= 0x20;
        intToDecString(dialogs->msgValues[valIdx], buf, g_numberFormat.digits[0]);
        func_8002F320(buf, 10, g_numberFormat.digits[0]);
        func_8002F4B0(buf, g_numberFormat.separator);
        break;
    case 0x30: case 0x31: case 0x32: case 0x33:
    case 0x34: case 0x35: case 0x36: case 0x37:
        valIdx -= 0x30;
        intToDecString(dialogs->msgValues[valIdx], buf, g_numberFormat.digits[0]);
        func_8002F320(buf, 10, g_numberFormat.digits[0]);
        break;
    case 0x40: case 0x41: case 0x42: case 0x43:
    case 0x44: case 0x45: case 0x46: case 0x47:
        valIdx -= 0x40;
        u32ToHexTiles(dialogs->msgValues[valIdx], buf, 1);
        for (hexPtr = buf; *hexPtr != 0; hexPtr++) {
            *hexPtr = (g_numberFormat.digits - 1)[*hexPtr];
        }
        break;
    }
    return buf;
}

/**
 * @brief Expand a name or number reference into the output text.
 *
 * @param dst  Write position in the decoded text.
 * @param code Reference code: kind in bits 8+, selector in the low byte.
 * @param buf  Scratch buffer for formatted numbers.
 * @return The write position after the inserted string.
 *
 * @note `src` has to be its own local: selecting through @p buf directly
 *       changes which register carries the string across the two calls.
 */
static inline u8 *insertArgString(u8 *dst, s32 code, u8 *buf) {
    u8 *src;

    src = buf;
    switch (code >> 8) {
    case MSG_ARG_BATTLE_CHAR:
    case MSG_ARG_NAME:
        src = getNameString(code, src);
        break;
    case MSG_ARG_NUMBER:
        src = getNumberString(code, src);
        break;
    }
    if (src != 0) {
        dst = appendString(dst, src);
    }
    return dst;
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
 * 0x04 + byte — Two-byte numeric value format (type 4: the dialogs' message values)
 *   0x05-0x06, 0x08-0x0B + byte — Escape: next byte stored literally
 *   0x0C + byte — Magic spell name lookup via getMagicNamePtr
 *   0x0D + byte — Item name lookup via getItemName
 *   0x0E + byte — Character name table set 0 (idx * 224 + subByte)
 *   0x0F + byte — Character name table set 1 (idx * 224 + subByte)
 *   0x10-0x18   — Direct name lookup via getBattleCharNameWrapper (type 0);
 *                 the test is ch < 0x20, but 0x19-0x1F are already printable
 *   0xE8-0xFF   — Double-byte character from D_8008369C lookup table
 *
 * Types 3, 4, and 0x10-0x18 all expand a reference code through
 * insertArgString, which dispatches on code >> 8:
 *   MSG_ARG_BATTLE_CHAR: getBattleCharNameWrapper(code & 0x1F) — direct name pointer
 *   MSG_ARG_NAME: getNameString's lookup switch (65-entry jump table, 0x20-0x60):
 *     0x20-0x22 → getCharNameWrapper2 (character name type A)
 *     0x30-0x3F → getCharNameWrapper (character name type B)
 *     0x40 → Angelo's name, 0x50 → Griever's name, 0x60 → Boko's name
 *     default  → D_80052A30
 *   MSG_ARG_NUMBER: getNumberString's format switch (40-entry jump table, 0x20-0x47):
 *     0x20-0x27 → Decimal with separator (intToDecString + F320 + F4B0)
 *     0x30-0x37 → Decimal plain (intToDecString + F320)
 * 0x40-0x47 → Hex, remapped to the g_numberFormat digit glyphs (u32ToHexTiles)
 *
 * insertArgString is inlined at its three call sites, producing 6 separate
 * jump tables. Handler code is shared across the expansions via
 * cross-jumping, except the hex remap handler (contains a loop, 3 copies).
 *
 * @note The control byte itself has already been stored when its handler
 *       runs, so every handler that expands a code overwrites it and writes
 *       from `output - 1`; the two-byte escape codes keep it and append their
 *       argument after it.
 *
 * Overflow: if output >= end, writes null to *end. If maxLen < 0, prints
 * "MESSAGE DATA OVER RUN" via printf before returning.
 *
 * @param input   Source byte stream with embedded control codes.
 * @param output  Destination buffer for decoded text.
 * @param maxLen  Maximum output length, or -1 for default limit (128 bytes).
 */
void decodeMessage(u8 *input, u8 *output, s32 maxLen) {
    u8 tmpBuf[16];
    u8 *end;
    u8 *cmdPair;
    s32 ch;
    s32 lowCmd;

    if (input == 0) {
        *output = 0;
        return;
    }
    if (maxLen >= 0) {
        end = output + maxLen;
    } else {
        end = output + 128;
    }
    while (1) {
        ch = *input++;
        if (output >= end) {
            *end = 0;
            if (maxLen < 0) {
                printf("MESSAGE DATA OVER RUN\n");
            }
            return;
        }
        *output++ = ch;
        if (ch >= 0x19 && ch < 0xE8) {
            continue;
        }
        if (ch == 2 || ch == 0 || ch == 1 || ch == 7) {
            return;
        }
        if (ch < 0x10) {
            if (ch == 3) {
                ch = *input++ | (MSG_ARG_NAME << 8);
                output = insertArgString(output - 1, ch, tmpBuf);
            } else if (ch == 4) {
                ch = *input++ | (MSG_ARG_NUMBER << 8);
                output = insertArgString(output - 1, ch, tmpBuf);
            } else if (ch == 0xC) {
                ch = *input++;
                output = appendString(output - 1, getMagicNamePtr(ch - 0x20));
            } else if (ch == 0xD) {
                ch = *input++;
                output = appendString(output - 1, getItemName(ch - 0x20));
            } else if (ch == 0xE || ch == 0xF) {
                ch = (ch - 0xE) * 224;
                lowCmd = *input++;
                output = func_8002F610(ch + ((lowCmd - 0x20) & 0xFF), output - 1);
            } else {
                *output++ = *input++;
            }
        } else if (ch >= 0x10 && ch < 0x20) {
            output = insertArgString(output - 1, ch, tmpBuf);
        } else if (ch >= 0xE8) {
            cmdPair = D_8008369C;
            cmdPair += (ch - 0xE8) * 2;
            output--;
            output[0] = cmdPair[0];
            output[1] = cmdPair[1];
            output += 2;
        }
    }
}


/**
 * @brief Decode the line a dialog is typing.
 *
 * Advances past @c typingLine line breaks with nextMessageLine, decodes from
 * there, and records where that line starts in @c linePtr.
 *
 * @param dialog Dialog.
 * @param output Output buffer for decodeMessage.
 */
void decodeDialogLine(Dialog *dialog, u8 *output) {
    s32 skip = dialog->typingLine;
    u8 *stream = dialog->dataPtr;
    while (skip > 0) {
        stream = nextMessageLine(stream);
        skip--;
    }
    decodeMessage(stream, output, -1);
    dialog->linePtr = stream;
}


/**
 * @brief Step a dialog to its next line and decode it.
 *
 * @param dialog Dialog; @c linePtr is advanced with nextMessageLine.
 * @param output Output buffer for decodeMessage.
 */
void advanceAndDecodeMessage(Dialog *dialog, u8 *output) {
    u8 *next = nextMessageLine(dialog->linePtr);
    dialog->linePtr = next;
    decodeMessage(next, output, -1);
}


/**
 * @brief Decode a dialog's current line without advancing.
 *
 * @param dialog Dialog; @c linePtr is read but not modified.
 * @param output Output buffer for decodeMessage.
 */
void decodeMessageDirect(Dialog *dialog, u8 *output) {
    decodeMessage(dialog->linePtr, output, -1);
}


/**
 * @brief Take the next character of the line a dialog is typing.
 *
 * Reads the decoded line at @c typedChars and moves it on: one byte for codes
 * 0x10-0x18 and 0x20 up, two for a command and its argument (0x03-0x06 and
 * 0x08-0x0F) or a two-byte glyph (0x19-0x1F). A newline (0x02) decodes the
 * next line (@c typedChars back to 0, @c typingLine and @c typingRow up by
 * one); a page break (0x01 or 0x07) decodes the next page and resets those
 * counters and @c scrollY. The end of the message (0x00) is not stepped past.
 *
 * @param dialog The dialog.
 * @param output Its decoded-line buffer.
 * @return The byte read; for a two-byte code, the first byte in bits 8-15 and
 * the second in bits 0-7.
 */
s32 nextDialogChar(Dialog *dialog, u8 *output) {
    u8 *p;
    s32 c;

    p = output + dialog->typedChars;
    c = *p++;
    if (c < 0x20) {
        if (c >= 0x19) {
            c <<= 8;
            c |= *p++;
        } else if (c == 2) {
            p = output;
            dialog->typingRow++;
            dialog->typingLine++;
            advanceAndDecodeMessage(dialog, output);
        } else if (c == 0) {
            p--;
        } else if (c == 1 || c == 7) {
            dialog->typingRow = 0;
            dialog->scrollY = 0;
            dialog->typingLine = 0;
            dialog->dataPtr = nextMessagePage(dialog->dataPtr);
            dialog->linePtr = dialog->dataPtr;
            p = output;
            decodeMessage(dialog->dataPtr, output, -1);
        } else if (c < 0x10) {
            c <<= 8;
            c |= *p++;
        }
    }
    dialog->typedChars = p - output;
    return c;
}


/**
 * @brief Reset a dialog's typing progress: @c typedChars, @c typingRow and @c typingLine.
 * @param entry The dialog.
 */
void resetDialogTyping(Dialog *entry) {
    entry->typedChars = 0;
    entry->typingLine = 0;
    entry->typingRow = 0;
}
