#include "common.h"
#include "psxsdk/libgpu.h"


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BAA0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BC10);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BC6C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BE48);


/**
 * @brief Compute a pointer into a table indexed by a battle entity's field 0x37.
 * @param a0 Index into the battle entity array (D_80083210, stride 64).
 * @return Address within the table at D_800834C0, offset by 8 + (field_0x37 << 2).
 */
s32 func_8002BEEC(s32 a0) {
    extern s32 D_800834C0;
    s32 base = D_800834C0 + 8;
    s32 result = func_8002ACBC(a0);
    return base + (result << 2);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BF24);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C030);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C070);


/**
 * @brief Initialize all 8 battle entity slots by calling func_8002AFA4 on each.
 *
 * Iterates slots 0 through 7, resetting each entity's display rects, flags,
 * position fields, and volume to default values.
 */
void func_8002C0C4(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_8002AFA4(i);
    }
}


extern s32 D_800834CC;

/**
 * @brief Store a base address into D_800834CC.
 * @param a0 Value to store (typically a memory address such as 0x80090000).
 */
void func_8002C100(s32 a0) {
    D_800834CC = a0;
}


/** @brief Return the maximum number of battle entity slots (always 8). */
s32 func_8002C10C(void) {
    return 8;
}


extern u8 D_80083840;
/**
 * @brief Get the current value of the global flag D_80083840.
 * @return The flag value as an unsigned byte.
 */
u8 func_8002C114(void) {
    return D_80083840;
}


/**
 * @brief Set the global flag D_80083840.
 * @param val The new flag value to store.
 */
void func_8002C124(u8 val) {
    D_80083840 = val;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C130);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C3AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C56C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C734);


extern u8 D_80082FF0[];
// sfx_entry_set_fields_29_2A_2C - D_80082FF0 stride 60

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C7BC);


/**
 * @brief Set three byte fields on an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param a1 Value to store at offset 0x29.
 * @param a2 Value to store at offset 0x2A.
 * @param a3 Value to store at offset 0x2C.
 */
void func_8002C7E0(s32 idx, s32 a1, s32 a2, s32 a3) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x29] = a1;
    p[0x2A] = a2;
    p[0x2C] = a3;
}


/**
 * @brief Set field 0x2B on an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param val Value to store at offset 0x2B.
 */
void func_8002C808(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x2B] = val;
}


/**
 * @brief Set the 32-bit field at offset 0x34 on an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param val 32-bit value to store at offset 0x34.
 */
void func_8002C828(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s32 *)(p + 0x34) = val;
}


/**
 * @brief Set the 32-bit field at offset 0x38 on an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param val 32-bit value to store at offset 0x38.
 */
void func_8002C848(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s32 *)(p + 0x38) = val;
}


/**
 * @brief Set the volume/pitch field (0x1A) on an SFX entry and propagate to its
 *        linked battle entity's field 0x3C.
 * @param a0 Index into the SFX entry array (D_80082FF0, stride 60).
 * @param a1 Volume/pitch value to set (e.g. 0x1000 = default).
 */
void func_8002C868(s32 a0, s32 a1) {
    extern u8 D_80082FF0[];
    u8 *entry = D_80082FF0 + a0 * 60;
    *(s16 *)(entry + 0x1A) = a1;
    func_8002AF70(entry[0x18], a1);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C8A4);


/**
 * @brief Decrement byte at offset 0x1E3 in D_80082FF0 and trigger an SFX update.
 *
 * Offset 0x1E3 falls within entry index 8 (8 * 60 = 480 = 0x1E0, +3 = 0x1E3),
 * at field offset 0x03 of that entry. After decrementing, calls func_8002C8A4
 * to process the change.
 */
void func_8002C920(void) {
    extern u8 D_80082FF0[];
    D_80082FF0[0x1E3]--;
    func_8002C8A4();
}


/**
 * @brief Build a packed RGBA-like color value from a scalar and store it as an
 *        SFX control word.
 *
 * Divides the input by 32, masks to 8 bits, replicates across the low 24 bits
 * (R=G=B), sets the high byte to 0x64 (alpha/command), and stores the result
 * in D_800831D4 before triggering an SFX update.
 *
 * @param a0 Scalar intensity value (divided by 32, clamped to 0-255).
 */
void func_8002C954(s32 a0) {
    extern s32 D_800831D4;
    a0 /= 32;
    a0 &= 0xFF;
    a0 |= (a0 << 16) | (a0 << 8);
    a0 |= 0x64000000;
    D_800831D4 = a0;
    func_8002C8A4();
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C9A4);


// sfx_entry_swap_field_16 - D_80082FF0 stride 60, returns old value

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C9F0);


/**
 * @brief Swap field 0x16 of an SFX entry, returning the old value.
 * @param idx Index into the SFX entry array (D_80082FF0, stride 60).
 * @param val New value to write at offset 0x16.
 * @return Previous value of field 0x16.
 */
s32 func_8002CA10(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    s32 old = p[0x16];
    p[0x16] = val;
    return old;
}


/**
 * @brief Get field 0x16 of an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @return Value of the byte at offset 0x16 in the entry.
 */
s32 func_8002CA34(s32 idx) {
    u8 *p = D_80082FF0 + idx * 60;
    return p[0x16];
}


/**
 * @brief Set field 0x10 and clear field 0x14 of an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param val 16-bit value to store at offset 0x10.
 */
void func_8002CA58(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s16 *)(p + 0x10) = val;
    *(s16 *)(p + 0x14) = 0;
}


/**
 * @brief Set the 16-bit field at offset 0x1C of an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param val 16-bit value to store at offset 0x1C.
 */
void func_8002CA7C(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s16 *)(p + 0x1C) = val;
}


/**
 * @brief Set the 16-bit field at offset 0x1E of an SFX entry (D_80082FF0, stride 60).
 * @param a0 Index into the SFX entry array.
 * @param a1 16-bit value to store at offset 0x1E.
 */
void func_8002CA9C(s32 a0, s32 a1) {
    u8 *p = D_80082FF0 + a0 * 60;
    *(s16 *)(p + 0x1E) = a1;
}


/**
 * @brief Get the signed 16-bit field at offset 0x1C of an SFX entry (D_80082FF0, stride 60).
 * @param a0 Index into the SFX entry array.
 * @return Signed 16-bit value from offset 0x1C of the entry.
 */
s32 func_8002CABC(s32 a0) {
    u8 *p = D_80082FF0 + a0 * 60;
    return *(s16 *)(p + 0x1C);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CAE0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CC4C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CDE4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CE68);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CE74);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CE84);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CECC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CF54);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D040);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D6AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D744);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D784);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D818);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D8CC);


// sfx_entry_init_fields_16_1E_2D - D_80082FF0 stride 60

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D970);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DBF8);


/**
 * @brief Configure an SFX entry for playback: mark as active, set rate, and set mode.
 * @param a0 Index into the SFX entry array (D_80082FF0, stride 60).
 * @param a1 Playback rate value stored at offset 0x1E (e.g. 0x200, 0x1000).
 * @param a2 Mode byte stored at offset 0x2D.
 */
void func_8002DCA4(s32 a0, s32 a1, s32 a2) {
    u8 *p = D_80082FF0 + a0 * 60;
    p[0x16] = 1;
    *(s16 *)(p + 0x1E) = a1;
    p[0x2D] = a2;
}


/** @brief Start SFX entry playback at slow rate (0x200). */
void func_8002DCD0(s32 a0) {
    func_8002DCA4(a0, 0x200, 0);
}


/** @brief Start SFX entry playback at normal rate (0x1000). */
void func_8002DCF4(s32 a0) {
    func_8002DCA4(a0, 0x1000, 0);
}


/**
 * @brief Set the rate delta (field 0x1E) for an SFX entry.
 * @param a0 Index into the SFX entry array.
 * @param a1 Rate delta value (negative values = fade out).
 */
void func_8002DD18(s32 a0, s32 a1) {
    func_8002CA9C(a0, a1);
}


/** @brief Set SFX entry to fade out at slow rate (-0x200). */
void func_8002DD38(s32 a0) {
    func_8002DD18(a0, -0x200);
}


/** @brief Set SFX entry to fade out at fast rate (-0x1000). */
void func_8002DD58(s32 a0) {
    func_8002DD18(a0, -0x1000);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DD78);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DDD8);


/**
 * @brief Set flags on the battle entity linked to an SFX entry.
 *
 * Reads the battle entity index from the SFX entry's field 0x18, then calls
 * func_8002AE30 to set field 0x38 (and derived field 0x30) on that entity
 * with the given value OR'd with 8.
 *
 * @param a0 Index into the SFX entry array (D_80082FF0, stride 60).
 * @param a1 Flag value to OR with 8 before storing.
 */
void func_8002DDFC(s32 a0, s32 a1) {
    extern u8 D_80082FF0[];
    u8 *entry = D_80082FF0 + a0 * 60;
    func_8002AE30(entry[0x18], a1 | 8);
}


/**
 * @brief Read field 0x38 of the battle entity linked to an SFX entry.
 *
 * Looks up the battle entity index from the SFX entry's field 0x18, then
 * returns that entity's field 0x38 via func_8002AE14.
 *
 * @param a0 Index into the SFX entry array (D_80082FF0, stride 60).
 */
void func_8002DE38(s32 a0) {
    extern u8 D_80082FF0[];
    u8 *entry = D_80082FF0 + a0 * 60;
    func_8002AE14(entry[0x18]);
}


/**
 * @brief Set field 0x2F on an SFX entry (D_80082FF0, stride 60).
 * @param idx Index into the SFX entry array.
 * @param val Byte value to store at offset 0x2F.
 */
void func_8002DE74(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x2F] = val;
}


/**
 * @brief Initialize an SFX entity slot to default values.
 *
 * Zeros out several fields in the SFX entry (offsets 0x14, 0x19, 0x2F),
 * then configures defaults: pitch = 0x1000, state = 0, reverb mode = 3,
 * rate = 0, delta = 0, entity flags = 6|8, display rect = (64,64,128,128),
 * and volume = 0x1000.
 *
 * @param a0 Index into the SFX entry array (D_80082FF0, stride 60).
 */
void func_8002DE94(a0)

s32 a0;
{
    s32 a1 = 0;
    u8 *entry = D_80082FF0 + a0 * 60;
    *(s16 *)(entry + 0x14) = 0;
    *(u8 *)(entry + 0x19) = 0;
    *(u8 *)(entry + 0x2F) = 0;
    func_8002D6AC(a0, a1);
    func_8002CA58(a0, 0x1000);
    func_8002CA10(a0, 0);
    func_8002DD78(a0, 3);
    func_8002CA7C(a0, 0);
    func_8002CA9C(a0, 0);
    func_8002DDFC(a0, 6);
    {
        s16 buf[4];
        buf[0] = 0x40;
        buf[1] = 0x40;
        buf[2] = 0x80;
        buf[3] = 0x80;
        func_8002E064(a0, buf);
    }
    func_8002C868(a0, 0x1000);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DF5C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E028);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E064);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E1B4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E1E8);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E254);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E298);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E3A4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E428);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E454);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E4AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E680);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E744);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E764);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E784);


/**
 * @brief Wrapper that calls func_8002E4AC with second argument 0 and returns result as u16.
 * @param a0 First argument passed through to func_8002E4AC.
 * @return Result of func_8002E4AC truncated to 16 bits.
 * @note Purpose uncertain -- appears to query a 16-bit status value.
 */
u16 func_8002E7A4(s32 a0) {
    return (u16)func_8002E4AC(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E7C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E810);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E8DC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002EAD0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002EE10);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F320);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F3F0);


// u32_to_hex_string - converts u32 to 8-char hex string with configurable base char

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F43C);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F4B0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F548);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F5B4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F610);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F688);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002FD28);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002FE0C);


/**
 * @brief Clear the RGB color bytes at offsets 0x20, 0x21, and 0x22 of a structure.
 * @param a0 Pointer to the base of the structure whose color fields are zeroed.
 */
void func_8002FF24(u8 *a0) {
    a0[0x20] = 0;
    a0[0x22] = 0;
    a0[0x21] = 0;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002FF34);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030058);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030094);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800300F8);


/** @brief Call func_80030058 with the default parameter value 0x1000. */
void func_80030214(void) { func_80030058(0x1000); }


/** @brief Empty stub -- no operation. */
void func_80030234(void) {
}


extern s16 D_800834D4;
/**
 * @brief Set the global 16-bit value D_800834D4.
 * @param val Value to store.
 */
void func_8003023C(s32 val) {
    D_800834D4 = val;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030248);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030274);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030288);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800302DC);


typedef struct {
    s16 f0;
    s8 f2;
    s8 f3;
    s16 f4;
    s8 f6;
    s8 f7;
} BattleCameraState;

extern BattleCameraState D_800834D0;

// init_battle_camera

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030518);


/**
 * @brief Reset the BattleCameraState struct (D_800834D0) to default values.
 *
 * Clears all fields to zero except f4, which is set to 0x1000 (default zoom/distance).
 */
void func_80030720(void) {
    D_800834D0.f3 = 0;
    D_800834D0.f4 = 0x1000;
    D_800834D0.f0 = 0;
    D_800834D0.f2 = 0;
    D_800834D0.f7 = 0;
    D_800834D0.f6 = 0;
}


extern u8 D_80083878;

/**
 * @brief Get a pointer to the global byte D_80083878.
 * @return Address of D_80083878.
 */
u8 *func_80030748(void) {
    return &D_80083878;
}

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030754);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800307F8);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030848);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800308B0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003093C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030A54);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030B2C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030CB0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030CFC);


/**
 * @brief Send an SPU command looked up from table D_80052A34.
 *
 * Uses the input as an index into D_80052A34 to retrieve a command byte,
 * then passes it to func_8001313C which writes it to the SPU command buffer
 * (D_80075058) and triggers SPU processing.
 *
 * @param a0 Index into the D_80052A34 lookup table.
 */
void func_80030D48(s32 a0) {
    extern u8 D_80052A34[];
    func_8001313C(D_80052A34[a0]);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030D78);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030DB0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030E60);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030F10);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030FA0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030FDC);


/** @brief Empty stub -- no operation. */
void func_80031044(void) {
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003104C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031188);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031224);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031364);


typedef struct {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
    u8 f8;
    u8 f9;
} Struct3754;
extern Struct3754 D_80083754;
// init_battle_transition - initializes D_80083754 (Struct3754)

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800316C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800316D4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800318E0);


/**
 * @brief Initialize the battle transition struct D_80083754 to its starting state.
 *
 * Sets the transition phase to 9, clears the timer fields (f4, f6), and resets
 * the flag bytes (f8, f9) to zero.
 */
void func_800318EC(void) {
    D_80083754.f0 = 9;
    D_80083754.f4 = 0;
    D_80083754.f6 = 0;
    D_80083754.f8 = 0;
    D_80083754.f9 = 0;
}


/**
 * @brief Perform linear interpolation within a range.
 *
 * Returns 0 if a2 < a0 (below range start), a3 if a2 >= a1 (at or above range end),
 * or a proportional value between 0 and a3 based on where a2 falls in [a0, a1).
 *
 * @param a0 Range start (minimum input value).
 * @param a1 Range end (maximum input value).
 * @param a2 Current input value to interpolate.
 * @param a3 Maximum output value (returned when a2 >= a1).
 * @return Interpolated value in [0, a3].
 */
s32 func_80031910(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a2 < a0) {
        return 0;
    }
    if (a2 >= a1) {
        return a3;
    }
    a2 -= a0;
    a1 -= a0;
    return a2 * a3 / a1;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031954);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031A18);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031CDC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031D68);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031D8C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031DF4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031E1C);


extern u8 D_80083772[];
// clear_animation_entries

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031EBC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031EF4);


/**
 * @brief Clear animation entry active flags.
 *
 * Iterates over 2 entries in D_80083772 (stride 0x10) and sets the byte at
 * offset 0x0E to zero for each, marking them as inactive.
 */
void func_80031F2C(void) {
    s32 i;
    u8 *ptr = D_80083772;
    for (i = 1; i >= 0; i--) {
        ptr[0xE] = 0;
        ptr += 0x10;
    }
}


extern u8 D_80085134[];
extern u8 D_80083938[];

/**
 * @brief Get a pointer to the global buffer D_80085134.
 * @return Address of D_80085134.
 */
u8 *func_80031F50(void) {
    return D_80085134;
}

/**
 * @brief Get a pointer to the global buffer D_80083938.
 * @return Address of D_80083938.
 */
u8 *func_80031F5C(void) {
    return D_80083938;
}

/**
 * @brief Spin-wait until a VSync event is received.
 *
 * Polls func_8004D208 (VSync) with argument 1 in a busy loop until it
 * returns a value other than -1, indicating the vertical blank has occurred.
 */
void func_80031F68(void) {
    do {
    } while (func_8004D208(1) == -1);
}


/**
 * @brief Return the base address of the battle allocation region.
 * @return 0x801F4000 (fixed address in PS1 RAM).
 */
u32 func_80031F9C(void) {
    return 0x801F4000;
}


/**
 * @brief Return the size of the battle allocation region.
 * @return 0x4000 (16384 bytes / 16 KB).
 */
s32 func_80031FA8(void) {
    return 0x4000;
}


/**
 * @brief Flip the double-buffered ordering table and initialize the new back buffer.
 *
 * Selects the OT buffer that is not currently active (from D_80083920[0..1]),
 * sets it as the active buffer in D_80083918, clears its 2-entry ordering tag
 * at offset 0x70 via ClearOTag, and initializes the primitive allocation pointer
 * at offset 0x78 to point to offset 0x7C (start of free space).
 */
void func_80031FB0(void) {
    extern s32 D_80083918;
    extern s32 D_80083920[];
    s32 buf;
    s32 ptr;

    buf = D_80083920[0];
    if (D_80083918 == buf) {
        buf = D_80083920[1];
    }
    D_80083918 = buf;
    ClearOTag((u32 *)(buf + 0x70), 2);
    ptr = D_80083918;
    *(s32 *)(ptr + 0x78) = ptr + 0x7C;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032010);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800320BC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032204);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003228C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032350);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032534);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032688);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003283C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800330F4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033298);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800332C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003331C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003334C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033380);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003346C);


/**
 * @brief Inset a rectangle by 1 pixel on each side and dispatch for rendering.
 *
 * Saves the original rectangle coordinates (4 u16 values), adjusts them
 * (x+1, y+1, w-2, h-2), calls func_8003346C to render with the inset rect,
 * then restores the original values.
 *
 * @param a0 First rendering parameter passed to func_8003346C.
 * @param a1 Second rendering parameter passed to func_8003346C.
 * @param a2 Pointer to a 4-element u16 rectangle {x, y, w, h} (modified temporarily).
 */
void func_80033534(s32 a0, s32 a1, u16 *a2) {
    s32 save0 = *(s32*)a2;
    s32 save1 = *(s32*)(a2 + 2);
    a2[0] += 1;
    a2[1] += 1;
    a2[2] -= 2;
    a2[3] -= 2;
    func_8003346C(a0, a1, a2);
    *(s32*)a2 = save0;
    *(s32*)(a2 + 2) = save1;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800335AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033688);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033768);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800337FC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033A28);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033C7C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033D5C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033F1C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003406C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800341BC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003431C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80034830);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800348C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800349F4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80034C74);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80034DBC);


/**
 * @brief Enable the display and execute a full rendering pass.
 *
 * Calls SetDispMask(1) to make the framebuffer visible, then calls the
 * main rendering function (func_80034DBC) followed by the scene submission
 * function (func_8003283C).
 */
void func_80035118(void) {
    SetDispMask(1);
    func_80034DBC();
    func_8003283C();
}


extern u8 D_80083928;
/**
 * @brief Get the current value of the global flag D_80083928.
 * @return The flag value as an unsigned byte.
 */
u8 func_80035148(void) {
    return D_80083928;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035158);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035294);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800352BC);


/**
 * @brief Clear both VRAM framebuffers to black.
 *
 * Clears two 384x224 (0x180 x 0xE0) regions in VRAM: the first at (0,0)
 * and the second at (0x200,0). Each clear is followed by DrawSync(0) to
 * wait for completion.
 */
void func_800352EC(void) {
    short rect[4];
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x180;
    rect[3] = 0xE0;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
    rect[0] = 0x200;
    rect[1] = 0;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035360);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035A6C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035AA4);


extern u8 D_80085138;
// set D_80085138 (u8)

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035AE4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035B28);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035B70);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035BB4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035BC0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035BCC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035C10);


/**
 * @brief Set the global flag D_80085138.
 * @param val Value to store.
 */
void func_80035C54(u8 val) {
    D_80085138 = val;
}


/**
 * @brief Get the current value of the global flag D_80085138.
 * @return The flag value as an unsigned byte.
 */
u8 func_80035C60(void) {
    return D_80085138;
}


extern s32 D_8008513C;

/**
 * @brief Set a single bit in the global bitmask D_8008513C.
 * @param a0 Bit position (0-31) to set.
 */
void func_80035C70(s32 a0) {
    D_8008513C |= (1 << a0);
}


/**
 * @brief Store a value in the upper 16 bits of D_8008513C and set bit 2.
 *
 * Shifts a0 left by 16 and ORs it into D_8008513C, then calls func_80035C70(2)
 * to also set bit 2 as a "data present" flag.
 *
 * @param a0 Value to store in the upper halfword of D_8008513C.
 */
void func_80035C8C(s32 a0) {
    D_8008513C |= (a0 << 16);
    func_80035C70(2);
}
