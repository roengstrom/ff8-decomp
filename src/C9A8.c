#include "common.h"

extern s32 *D_80074F08;
extern s32 D_80075028[];
extern s32 D_80077288[];
extern s32 D_80077298[];

void func_8001E594(void);
void func_8001E7A8(void);
void func_8001E808(void);
void func_8001F51C(void);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C1A8);

extern u8 D_80073E68[];

/**
 * @brief Applies an instrument table entry to a sequence track.
 *
 * Stores the instrument index at track offset +0x66, then looks up the
 * instrument in D_80073E68 (stride 16 bytes) and calls func_8001C1A8 to
 * copy the instrument parameters into the track structure.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Instrument table index to apply.
 */
void func_8001C1DC(u8 *a0, s32 a1) {
    *(s16 *)(a0 + 0x66) = a1;
    func_8001C1A8((s32)a0, (s32)(D_80073E68 + a1 * 16), *(s32 *)(D_80073E68 + a1 * 16));
}

/**
 * @brief Clears voice bits from multiple SPU control bitmasks and resets track state.
 *
 * Applies ~a1 mask to clear bits in D_80075028 entries [0], [1], [2], [4],
 * [7], [8], and [9] (key-on, reverb, noise, and other voice control masks).
 * Also zeroes the track's status fields at offsets +0x24 and +0x38.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to clear from all control registers.
 */
void func_8001C214(u8 *a0, s32 a1) {
    s32 mask = ~a1;
    D_80075028[0] &= mask;
    D_80075028[4] &= mask;
    D_80075028[7] &= mask;
    D_80075028[8] &= mask;
    D_80075028[9] &= mask;
    D_80075028[1] &= mask;
    D_80075028[2] &= mask;
    *(s32 *)(a0 + 0x24) = 0;
    *(s32 *)(a0 + 0x38) = 0;
}

/**
 * @brief Transposes a note value based on flag bits, for percussion mapping.
 *
 * If @p a1 falls in range [0x80, 0xB0) or [0xD0, 0x100), applies an offset
 * based on bits in @p a0: bit 1 adds +0x10, bit 2 adds +0x20.
 *
 * @note Purpose uncertain -- appears to select alternate percussion banks
 *       or drum kit variations based on channel flags.
 *
 * @param a0 Channel/track flags; bits 1-2 select the transposition amount.
 * @param a1 Note or percussion instrument index.
 * @return Transposed note value, or @p a1 unchanged if outside valid ranges.
 */
s32 func_8001C280(s32 a0, s32 a1) {
    if ((u32)(a1 - 0x80) < 0x30 || (u32)(a1 - 0xD0) < 0x30) {
        if (a0 & 2) {
            return a1 + 0x10;
        }
        if (a0 & 4) {
            return a1 + 0x20;
        }
    }
    return a1;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C2C8);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C39C);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C3E8);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C490);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C530);

/** @brief Reads a signed 16-bit little-endian offset from the stream cursor and advances cursor by that offset.
 *  @param a0 Pointer to stream state (a0[0] = cursor pointer).
 */
void func_8001C5D8(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 offset = (s16)(ptr[0] | (ptr[1] << 8));
    *(u8 **)a0 = ptr + offset;
}

/**
 * @brief Conditional branch in music sequence: reads voice index and optional offset.
 *
 * Reads a byte from the stream. If the byte is within the valid voice count
 * (D_80074F08+0x60), reads a 16-bit signed offset from the next two bytes
 * and jumps the stream cursor by that offset. Otherwise skips the 2-byte
 * offset field. Both paths end with storing the new cursor and returning.
 *
 * @param a0 Pointer to the stream state (cursor at +0x00).
 *
 * @note Non-matching: compiler adds unnecessary andi for zero-extension after
 *       lhu/lbu, uses sltu instead of slt, and schedules the cursor store
 *       into the branch delay slot instead of before the conditional block.
 */
INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C604);

/** @brief Reads one byte from stream, advances cursor, ORs 3 into flags, stores byte << 8 as halfword at +0x80.
 *  @param a0 Pointer to stream state.
 */
void func_8001C65C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    u8 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)(a0 + 0xF8) |= 3;
    *(u16 *)(a0 + 0x80) = val << 8;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C684);

/**
 * @brief Reads one byte from stream, advances cursor, ORs 3 into flags,
 *        clears halfwords at +0x86 and +0x88, stores byte << 23 at +0x44.
 * @param a0 Pointer to stream state.
 */
void func_8001C708(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *(s8 *)ptr;
    s32 flags = *(s32 *)(a0 + 0xF8) | 3;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x86) = 0;
    *(s32 *)(a0 + 0xF8) = flags;
    *(u16 *)(a0 + 0x88) = 0;
    *(s32 *)(a0 + 0x44) = val << 23;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C738);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C7C4);

/** @brief Sets bit 6 (0x40) in the flags word at offset 0x30 of a0. */
void func_8001C854(u8 *a0) {
    *(u32 *)(a0 + 0x30) |= 0x40;
}

/** @brief Clears bit 6 (0x40) in the flags word at offset 0x30 of a0. */
void func_8001C868(u8 *a0) {
    *(u32 *)(a0 + 0x30) &= ~0x40;
}

/** @brief Increments the word at a0 by 2. */
void func_8001C87C(s32 *a0) {
    *a0 += 2;
}

/** @brief Empty stub — no operation. */
void func_8001C890(void) {
}

/**
 * @brief Reads one byte from stream, advances cursor. Clears halfword at +0x8A,
 *        stores byte << 8 at +0xDE. If bit 8 of flags at +0x30 is set, ORs 3
 *        into flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void func_8001C898(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    s32 flags30;
    *(u8 **)a0 = ptr + 1;
    flags30 = *(s32 *)(a0 + 0x30);
    *(u16 *)(a0 + 0x8A) = 0;
    *(u16 *)(a0 + 0xDE) = (val << 8) & 0xFFFF;
    if (flags30 & 0x100) {
        *(s32 *)(a0 + 0xF8) = *(s32 *)(a0 + 0xF8) | 3;
    }
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C8DC);

/**
 * @brief Reads a note byte from the stream and sets pitch/flag fields.
 *
 * Reads one byte from the stream cursor, advances it, sets flag bits 0-1
 * at offset +0xF8, clears +0x8E, and stores (byte + 0x40) << 8 at +0x8C.
 *
 * @param a0 Pointer to the track structure (stream ptr at +0x00).
 */
void func_8001C968(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    u32 val = *ptr;
    s32 flags = *(s32 *)(a0 + 0xF8);
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x8E) = 0;
    *(s32 *)(a0 + 0xF8) = flags | 3;
    val += 0x40;
    val &= 0xFF;
    val <<= 8;
    *(u16 *)(a0 + 0x8C) = val;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001C99C);

/**
 * @brief Reads one byte from stream, advances cursor, stores byte as
 *        halfword at +0x92.
 * @param a0 Pointer to stream state.
 */
void func_8001CA28(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    u16 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x92) = val;
}

/** @brief Increments halfword at a0+0x92, wraps modulo 16. */
void func_8001CA44(u8 *a0) {
    *(u16 *)(a0 + 0x92) = (*(u16 *)(a0 + 0x92) + 1) & 0xF;
}

/** @brief Decrements halfword at a0+0x92, wraps modulo 16. */
void func_8001CA5C(u8 *a0) {
    *(u16 *)(a0 + 0x92) = (*(u16 *)(a0 + 0x92) - 1) & 0xF;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CA74);

/**
 * @brief Read instrument index from stream and apply to track.
 *
 * Reads one byte from the stream cursor, resolves the instrument via
 * func_8001B400, applies the instrument table entry via func_8001C1A8,
 * stores the instrument index, clears field 0x10A, and masks field 0x30.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001CB1C(u8 *a0) {
    extern s32 *D_80074F08;
    extern u8 D_80073E68[];
    s32 *bankPtr = D_80074F08;
    u8 *ptr = *(u8 **)a0;
    s32 byte = *ptr;
    s32 inst;
    *(u8 **)a0 = ptr + 1;
    inst = func_8001B400(*bankPtr, byte);
    func_8001C1A8(a0, D_80073E68 + inst * 16, 0x1010);
    *(u16 *)(a0 + 0x66) = inst;
    *(u16 *)(a0 + 0x10A) = 0;
    *(s32 *)(a0 + 0x30) &= (s32)0xE6FFEFF7;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CBA4);

/** @brief Looks up D_80073E68 table by a0[0x66] index, copies fields to a0, ORs flags, masks a0[0x30].
 *  @param a0 Pointer to stream state.
 */
void func_8001CC30(u8 *a0) {
    extern u8 D_80073E68[];
    u8 *entry = D_80073E68 + *(u16 *)(a0 + 0x66) * 16;
    *(u16 *)(a0 + 0x106) = *(u16 *)(entry + 0xC);
    *(u16 *)(a0 + 0x108) = *(u16 *)(entry + 0xE);
    *(s32 *)(a0 + 0xF8) |= 0xFF00;
    *(s32 *)(a0 + 0x30) &= (s32)0xE6FFFFFF;
}

/** @brief Reads one byte from stream, sign-extends to s8, stores as halfword at a0+0xE4.
 *  @param a0 Pointer to stream state.
 */
void func_8001CC78(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s8 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s16 *)(a0 + 0xE4) = val;
}

/** @brief Reads one byte from stream, sign-extends, adds to halfword at a0+0xE4.
 *  @param a0 Pointer to stream state.
 */
void func_8001CC9C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s8 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xE4) = *(u16 *)(a0 + 0xE4) + val;
}

/**
 * @brief Reads duration and delta from stream and stores to track fields.
 *
 * Reads a byte for duration, storing at +0x94 (or 0x100 if the byte was 0).
 * Then reads a signed byte for delta, storing sign-extended at +0xEA.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001CCC8(u8 *a0) {
    u8 *ptr;
    s32 val;

    ptr = *(u8 **)a0;
    val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x94) = val;
    if (val == 0) {
        *(u16 *)(a0 + 0x94) = 0x100;
    }
    ptr = *(u8 **)a0;
    val = *ptr;
    *(u8 **)a0 = ptr + 1;
    val <<= 24;
    val >>= 24;
    *(u16 *)(a0 + 0xEA) = val;
}

/** @brief Reads one byte from stream as duration. If zero, stores 0x100. Clears tempo/timer fields.
 *  @param a0 Pointer to stream state.
 */
void func_8001CD10(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x98) = val;
    if (val != 0) goto skip;
    *(u16 *)(a0 + 0x98) = 0x100;
skip:
    *(u16 *)(a0 + 0xEC) = 0;
    *(u16 *)(a0 + 0x96) = 0;
    *(u16 *)(a0 + 0x9A) = 1;
}

/** @brief Clears the halfword at offset 0x98 of a0. */
void func_8001CD48(u8 *a0) {
    *(u16 *)(a0 + 0x98) = 0;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CD50);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CDB0);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CE14);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CF0C);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CF6C);

/** @brief Clears bit 0x1 in a0+0x30, sets bit 0x10 in a0+0xF8, zeroes a0+0xEE. */
void func_8001CFD8(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x1;
    *(s32 *)(a0 + 0xF8) |= 0x10;
    *(u16 *)(a0 + 0xEE) = 0;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001CFFC);

/** @brief Reads one byte from stream, advances cursor, masks to 7 bits, shifts left 8, stores to halfword at a0+0xBC.
 *  @param a0 Pointer to stream state.
 */
void func_8001D0AC(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xBC) = (val & 0x7F) << 8;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D0D0);

/** @brief Clears bit 0x2 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF0. */
void func_8001D140(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x2;
    *(s32 *)(a0 + 0xF8) |= 0x3;
    *(u16 *)(a0 + 0xF0) = 0;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D164);

/** @brief Reads one byte from stream, advances cursor, shifts left 7, stores to halfword at a0+0xCA.
 *  @param a0 Pointer to stream state.
 */
void func_8001D1D0(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xCA) = val << 7;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D1F0);

/** @brief Clears bit 0x4 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF2. */
void func_8001D25C(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x4;
    *(s32 *)(a0 + 0xF8) |= 0x3;
    *(u16 *)(a0 + 0xF2) = 0;
}

/**
 * @brief Enables noise mode for the given voice bitmask on the appropriate SPU channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (D_80074F08[0x3C/4]); otherwise sets bits in D_80075028[0x1C/4].
 * Marks the SPU dirty flags (D_80077288) with 0x110 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable noise mode for.
 */
void func_8001D280(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] | a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
}

/**
 * @brief Disables noise mode for the given voice bitmask and clears track state.
 *
 * Clears the voice bits from the noise enable register (primary or secondary
 * depending on channel at offset +0x60). Marks SPU dirty flags with 0x110
 * and zeroes the track's noise state field at offset +0xD0.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to disable noise mode for.
 */
void func_8001D2E0(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] & ~a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
    *(u16 *)((s32)a0 + 0xD0) = 0;
}

/**
 * @brief Enables reverb for the given voice bitmask on the appropriate SPU channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (D_80074F08[0x44/4]); otherwise checks flag 0x10000 at offset +0x30
 * before setting bits in D_80075028[0x24/4]. Marks dirty flags with 0x100.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable reverb for.
 */
void func_8001D348(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] | a1;
    } else if (*(s32 *)((s32)a0 + 0x30) & 0x10000) {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

/**
 * @brief Disables reverb for the given voice bitmask and clears track state.
 *
 * Clears voice bits from the reverb enable register (primary or secondary
 * depending on channel at offset +0x60). Marks SPU dirty flags with 0x100
 * and zeroes the track's reverb state field at offset +0xD2.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to disable reverb for.
 */
void func_8001D3BC(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] & ~a1;
    } else {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
    *(u16 *)((s32)a0 + 0xD2) = 0;
}

/**
 * @brief Enables pitch modulation for the given voice bitmask on the appropriate channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (D_80074F08[0x40/4]); otherwise sets bits in D_80075028[0x20/4].
 * Marks SPU dirty flags with 0x100 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable pitch modulation for.
 */
void func_8001D424(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x40 / 4] = p[0x40 / 4] | a1;
    } else {
        D_80075028[0x20 / 4] = D_80075028[0x20 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D484);

/** @brief Sets the halfword at offset 0x9A of a0 to 1. */
void func_8001D4E4(u8 *a0) {
    *(u16 *)(a0 + 0x9A) = 1;
}

/** @brief Empty stub — no operation. */
void func_8001D4F0(void) {
}

/** @brief Empty stub — no operation. */
void func_8001D4F8(void) {
}

/** @brief Empty stub — no operation. */
void func_8001D500(void) {
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D508);

/**
 * @brief Reads byte from stream. ORs 0x900 into flags at +0xF8, ORs
 *        0x1000000 into +0x30, masks +0x106 to keep bits 0-7 and 15,
 *        ORs in byte shifted left 8.
 * @param a0 Pointer to stream state.
 */
void func_8001D5A4(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 byte = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)(a0 + 0xF8) |= 0x900;
    *(s32 *)(a0 + 0x30) |= 0x1000000;
    *(u16 *)(a0 + 0x106) = (*(u16 *)(a0 + 0x106) & 0x80FF) | (byte << 8);
}

/**
 * @brief Reads a byte from the sequence data stream and sets a track parameter.
 *
 * Reads one byte from the current position in the sequence data (pointer
 * at offset +0x00), advances the read pointer, and writes the byte into
 * bits [7:4] of the track's control field at offset +0x106. Also sets
 * flag 0x1000 in the track's dirty flags at offset +0xF8.
 *
 * @param a0 Pointer to the sequence track structure (as s32 for pointer arithmetic).
 * @param a1 Unused parameter.
 */
void func_8001D5E8(s32 a0, s32 a1) {
    s32 byte = *(u8 *)(*(s32 *)a0);
    *(s32 *)a0 = *(s32 *)a0 + 1;
    *(u16 *)(a0 + 0x106) = (*(u16 *)(a0 + 0x106) & 0xFF0F) | (byte << 4);
    *(s32 *)(a0 + 0xF8) = *(s32 *)(a0 + 0xF8) | 0x1000;
}

/**
 * @brief Reads one byte from stream, advances cursor. Masks field at +0x106
 *        to clear low 4 bits and ORs in the byte. Sets bit 15 (0x8000) in
 *        flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void func_8001D61C(u8 *a0, s32 a1) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)(a0 + 0xF8) |= 0x8000;
    *(u16 *)(a0 + 0x106) = (*(u16 *)(a0 + 0x106) & 0xFFF0) | val;
}

/**
 * @brief Reads byte from stream. ORs 0x2200 into flags at +0xF8, ORs
 *        0x8000000 into +0x30, masks +0x108 to keep bits 0-5 and 13-15,
 *        ORs in byte shifted left 6.
 * @param a0 Pointer to stream state.
 */
void func_8001D64C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 byte = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)(a0 + 0xF8) |= 0x2200;
    *(s32 *)(a0 + 0x30) |= 0x8000000;
    *(u16 *)(a0 + 0x108) = (*(u16 *)(a0 + 0x108) & 0xE03F) | (byte << 6);
}

/**
 * @brief Reads one byte from stream, advances cursor. ORs 0x4400 into flags
 *        at +0xF8, ORs 0x10000000 into field at +0x30, masks +0x108 to keep
 *        bits 5-15, ORs in byte (low 5 bits).
 * @param a0 Pointer to stream state.
 */
void func_8001D690(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 flags = *(s32 *)(a0 + 0xF8);
    s32 val;
    s32 f30;
    u16 f108;
    val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)(a0 + 0xF8) = flags | 0x4400;
    f30 = *(s32 *)(a0 + 0x30);
    f108 = *(u16 *)(a0 + 0x108);
    *(s32 *)(a0 + 0x30) = f30 | 0x10000000;
    *(u16 *)(a0 + 0x108) = (f108 & 0xFFE0) | val;
}

/**
 * @brief Reads one byte from stream, advances cursor. Clears bit 15 of +0x106.
 *        If byte == 5, sets bit 15 of +0x106. ORs 0x100 into flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void func_8001D6D0(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    u16 f106 = *(u16 *)(a0 + 0x106);
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    f106 &= 0x7FFF;
    *(u16 *)(a0 + 0x106) = f106;
    if (val == 5) {
        *(u16 *)(a0 + 0x106) = f106 | 0x8000;
    }
    *(s32 *)(a0 + 0xF8) = *(s32 *)(a0 + 0xF8) | 0x100;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D714);

/**
 * @brief Reads one byte from stream, advances cursor. Clears bit 5 of +0x108.
 *        If byte == 7, sets bit 5 of +0x108. ORs 0x400 into flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void func_8001D78C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    u16 f108 = *(u16 *)(a0 + 0x108);
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    f108 &= 0xFFDF;
    *(u16 *)(a0 + 0x108) = f108;
    if (val == 7) {
        *(u16 *)(a0 + 0x108) = f108 | 0x20;
    }
    *(s32 *)(a0 + 0xF8) = *(s32 *)(a0 + 0xF8) | 0x400;
}

/** @brief Increments the word at a0 by 1. */
void func_8001D7D0(s32 *a0) {
    *a0 += 1;
}

/** @brief Empty stub — no operation. */
void func_8001D7E4(void) {
}

/**
 * @brief Advance to the next voice slot in the circular buffer.
 *
 * Increments the slot index (D4, mod 4), saves the current stream pointer
 * to the slot's table entry, clears the slot's counter, and copies the
 * current voice index to the new slot.
 *
 * @param a0 Pointer to the track structure.
 */
INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D7EC);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D83C);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D8D0);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D93C);

/**
 * @brief Advance to next voice entry: increment counter, update stream ptr and voice.
 *
 * Increments the 16-bit counter at a0 + D4*2 + 0x70, sets the stream
 * pointer (a0[0]) from the table at a0 + D4*4 + 4, and copies the
 * voice index from a0 + D4*2 + 0x78 to a0 + 0x6E.
 *
 * @param a0 Pointer to the track structure.
 */
INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001D9B8);

/** @brief Reads byte from stream, stores to three halfword fields (0x64, 0x62, 0xD6), clears 0xD8.
 *  @param a0 Pointer to stream state.
 */
void func_8001DA0C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xD8) = 0;
    *(u16 *)(a0 + 0x64) = val;
    *(u16 *)(a0 + 0x62) = val;
    *(u16 *)(a0 + 0xD6) = val;
}

/**
 * @brief Read a signed delta byte from the stream and adjust the volume field.
 *
 * Loads a signed byte from the stream. If non-zero, adds it to the current
 * volume at offset 0xD6 and clamps the result to [1, 255]. Stores the
 * result (or 0 if delta was zero) at offset 0xD8.
 *
 * @param a0 Pointer to the track state structure.
 */
void func_8001DA34(u8 *a0) {
    u8 *ptr;
    s32 delta;

    ptr = *(u8 **)a0;
    delta = *(s8 *)ptr;
    *(u8 **)a0 = ptr + 1;
    if (delta != 0) {
        delta += *(s16 *)(a0 + 0xD6);
        if (delta <= 0) {
            delta = 1;
        } else if (delta >= 256) {
            delta = 255;
        }
    }
    *(u16 *)(a0 + 0xD8) = delta;
}

/**
 * @brief Conditionally updates field at +0x30 based on D_80074F08->0x34.
 *
 * If the word at D_80074F08+0x34 is nonzero, clears bits 0x19001008 and sets
 * bit 3 (0x8) in the word at a0+0x30.
 *
 * @param a0 Pointer to stream state.
 */
void func_8001DA7C(u8 *a0) {
    s32 check = *(s32 *)((u8 *)D_80074F08 + 0x34);
    if (check != 0) {
        *(s32 *)(a0 + 0x30) = (*(s32 *)(a0 + 0x30) & (s32)0xE6FFEFF7) | 8;
    }
}

/** @brief Clears bit 0x8 in a0+0x30 and zeroes halfword at a0+0x10A. */
void func_8001DAB4(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x8;
    *(u16 *)(a0 + 0x10A) = 0;
}

/**
 * @brief Reads two bytes from stream, stores to D_80074F08+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 *
 * @note Non-matching: register allocation — compiler assigns dest to $3 (v1)
 *       instead of $5 (a1). Declaration order doesn't affect assignment.
 */
INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001DACC);

/**
 * @brief Reads two bytes from stream and combines into 16-bit value at
 *        D_80074F08+0x6C (low byte first, high byte shifted left 8).
 *
 * @param a0 Pointer to the stream state.
 *
 * @note Non-matching: register allocation — compiler assigns ptr to $3 (v1)
 *       instead of $5 (a1). Same leaf register allocation issue as func_8001DACC.
 */
INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001DB04);

/**
 * @brief Reads a byte from stream and sets both hi and lo nibble track parameters.
 *
 * Calls func_8001D5E8 to set bits [7:4] of offset +0x106, then calls
 * func_8001D61C to set bits [3:0] of offset +0x106.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Voice bitmask (passed through to func_8001D5E8).
 */
void func_8001DB40(s32 a0, s32 a1) {
    func_8001D5E8(a0, a1);
    func_8001D61C((u8 *)a0, a1);
}

/**
 * @brief Reads byte from stream; computes duration (byte+1 or 0x101 if zero),
 *        stores to +0xD0, then calls func_8001D280 with voice bitmask.
 * @param a0 Pointer to stream state.
 * @param a1 Voice bitmask passed through to func_8001D280.
 */
void func_8001DB7C(u8 *a0, s32 a1) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    if (val != 0) {
        *(u16 *)(a0 + 0xD0) = val + 1;
    } else {
        *(u16 *)(a0 + 0xD0) = 0x101;
    }
    func_8001D280((s32 *)a0, a1);
}

/** @brief Reads byte from stream; if non-zero stores byte+1 to a0+0xD0, else stores 0x101.
 *  @param a0 Pointer to stream state.
 */
void func_8001DBC0(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    if (val != 0) {
        *(u16 *)(a0 + 0xD0) = val + 1;
    } else {
        *(u16 *)(a0 + 0xD0) = 0x101;
    }
}

/**
 * @brief Reads byte from stream; computes duration (byte+1 or 0x101 if zero),
 *        stores to +0xD2, then calls func_8001D348 with voice bitmask.
 * @param a0 Pointer to stream state.
 * @param a1 Voice bitmask passed through to func_8001D348.
 */
void func_8001DBF0(u8 *a0, s32 a1) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    if (val != 0) {
        *(u16 *)(a0 + 0xD2) = val + 1;
    } else {
        *(u16 *)(a0 + 0xD2) = 0x101;
    }
    func_8001D348((s32 *)a0, a1);
}

/** @brief Reads byte from stream; if non-zero stores byte+1 to a0+0xD2, else stores 0x101.
 *  @param a0 Pointer to stream state.
 */
void func_8001DC34(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    if (val != 0) {
        *(u16 *)(a0 + 0xD2) = val + 1;
    } else {
        *(u16 *)(a0 + 0xD2) = 0x101;
    }
}

/**
 * @brief Clears bits 0x30 and 0x08 in flags at +0x30, then calls
 *        func_8001D2E0, func_8001D3BC, func_8001D484, and clears
 *        bits 0x5 in halfword at +0x9A.
 * @param a0 Pointer to stream state.
 * @param a1 Second argument passed to func_8001D3BC and func_8001D484.
 */
void func_8001DC64(u8 *a0, s32 a1) {
    *(s32 *)(a0 + 0x30) &= ~0x37;
    func_8001D2E0((s32 *)a0, a1);
    func_8001D3BC((s32 *)a0, a1);
    func_8001D484((s32 *)a0, a1);
    *(u16 *)(a0 + 0x9A) = *(u16 *)(a0 + 0x9A) & 0xFFFA;
}

/** @brief Sets bit 0x10 in word at a0+0x30. */
void func_8001DCCC(u8 *a0) {
    *(s32 *)(a0 + 0x30) |= 0x10;
}

/** @brief Clears bit 0x10 in word at a0+0x30. */
void func_8001DCE0(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x10;
}

/** @brief Sets bit 0x20 in word at a0+0x30. */
void func_8001DCF4(u8 *a0) {
    *(s32 *)(a0 + 0x30) |= 0x20;
}

/** @brief Clears bit 0x20 in word at a0+0x30. */
void func_8001DD08(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x20;
}

/**
 * @brief Reads two 16-bit LE offsets from the stream, resolves pointers,
 *        sets up D_80074EE8 with volume/pan data, and calls func_80017AAC.
 *
 * For each offset, if non-zero, computes a pointer as (stream_base + offset + 2);
 * otherwise passes NULL. Sets D_80074EE8[0..1] to 0, [2] to the upper byte of
 * the halfword at a0+0x8C, [3] to a0+0x44 >> 23. Advances the stream pointer by 4.
 *
 * @param a0 Pointer to the stream/voice state structure.
 */
void func_8001DD1C(u8 *a0) {
    extern s32 D_80074EE8[];
    u8 *ptr = *(u8 **)a0;
    u16 off;
    s32 a5;
    s32 a6;

    {
        s32 hi = ptr[1];
        s32 lo = ptr[0];
        off = (hi << 8) | lo;
    }
    if (off != 0) {
        a5 = (s32)(ptr + off + 2);
    } else {
        a5 = 0;
    }

    ptr += 2;

    {
        s32 hi = ptr[1];
        s32 lo = ptr[0];
        off = (hi << 8) | lo;
    }
    if (off != 0) {
        a6 = (s32)(ptr + off + 2);
    } else {
        a6 = 0;
    }

    D_80074EE8[0] = 0;
    D_80074EE8[1] = 0;
    D_80074EE8[2] = *(u16 *)(a0 + 0x8C) >> 8;
    D_80074EE8[3] = *(s32 *)(a0 + 0x44) >> 23;
    func_80017AAC(D_80074EE8, a5, a6, 0);

    *(u8 **)a0 = *(u8 **)a0 + 4;
}

/**
 * @brief Reads byte from stream, sets flags |= 0x800, clears +0x6C,
 *        stores byte<<8 to +0x6A, then calls func_8001D424.
 * @param a0 Pointer to stream state.
 */
void func_8001DDD4(u8 *a0, s32 a1) {
    u8 *ptr = *(u8 **)a0;
    s32 flags = *(s32 *)(a0 + 0x30);
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x6C) = 0;
    *(s32 *)(a0 + 0x30) = flags | 0x800;
    *(u16 *)(a0 + 0x6A) = val << 8;
    func_8001D424((s32 *)a0, a1);
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001DE18);

/** @brief Sets bit 0x100000 in word at a0+0x30. */
void func_8001DECC(u8 *a0) {
    *(s32 *)(a0 + 0x30) |= 0x100000;
}

/**
 * @brief Reads one byte from stream, advances cursor, adds D_80051824 base
 *        to get address, stores into field +0x38 of D_80074F08.
 * @param a0 Pointer to stream state.
 */
void func_8001DEE0(u8 *a0) {
    extern u8 D_80051824;
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)((u8 *)D_80074F08 + 0x38) = val + (s32)&D_80051824;
}

/** @brief ORs a1 into the word at D_80074F08+0x8 (set flags).
 *  @param a0 Unused.
 *  @param a1 Bits to set.
 */
void func_8001DF10(s32 a0, s32 a1) {
    *(s32 *)((u8 *)D_80074F08 + 0x8) |= a1;
}

/** @brief ANDs ~a1 into the word at D_80074F08+0x8 (clear flags).
 *  @param a0 Unused.
 *  @param a1 Bits to clear.
 */
void func_8001DF30(s32 a0, s32 a1) {
    *(s32 *)((u8 *)D_80074F08 + 0x8) &= ~a1;
}

/** @brief Calls func_8001C2C8. */
void func_8001DF50(void) {
    func_8001C2C8();
}

/**
 * @brief Disables SPU IRQ and clears the IRQ voice mask from the engine state.
 *
 * If D_80077298[3] (the active IRQ voice bitmask) is non-zero, disables
 * SPU IRQ, clears the IRQ callback, resets the IRQ address to the stored
 * value, clears the corresponding bits in D_80075028[8], marks dirty flags,
 * and zeroes the IRQ voice mask.
 */
void func_8001DF70(void) {
    s32 *s0 = D_80077298;
    if (s0[3] != 0) {
        SpuSetIRQ(0);
        SpuSetIRQCallback(0);
        func_80014DE0(s0[3]);
        D_80075028[8] &= ~s0[3];
        D_80077288[2] |= 0x100;
        s0[3] = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001DFF4);

/**
 * @brief Sets up SPU transfer at address 0x2100, registers callback
 *        func_8001E594, and initiates DMA transfer of 0x800 bytes.
 */
void func_8001E084(void) {
    s32 addr = D_80077298[0] + 0x800;
    SpuSetTransferStartAddr(0x2100);
    func_8003E494(func_8001E594);
    func_8003E3A4(addr, 0x800);
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001E0CC);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001E308);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001E4C4);

/**
 * @brief Sets up SPU voice parameters for both channels, then registers
 *        callback for 0x1000-byte streaming at addresses 0x1100/0x2100.
 */
void func_8001E594(void) {
    s32 *base = D_80077298;
    func_8001E308(base[4], 0, 0x1100, 0x2100);
    func_8001E308(base[4] + 1, 0, 0x1100, 0x2100);
    func_8001E4C4(0x1000, 0x2100, func_8001E7A8);
}

/**
 * @brief Sets up SPU voice parameters for dual-channel streaming, then registers
 *        callback for 0x2000-byte streaming at addresses 0x1100-0x2900.
 */
void func_8001E5F8(void) {
    s32 *base = D_80077298;
    func_8001E308(base[4], 1, 0x1100, 0x2100);
    func_8001E308(base[4] + 1, 2, 0x1900, 0x2900);
    func_8001E4C4(0x2000, 0x2100, func_8001E808);
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001E65C);

void func_8001E7D8(void);

/** @brief Calls func_8001E65C with SPU transfer params (0x1100, 0x1100, 0x800) and callback func_8001E7D8. */
void func_8001E7A8(void) {
    func_8001E65C(0x1100, 0x1100, 0x800, func_8001E7D8);
}

/** @brief Calls func_8001E65C with SPU transfer params (0x2100, 0x2100, 0x800) and callback func_8001E7A8. */
void func_8001E7D8(void) {
    func_8001E65C(0x2100, 0x2100, 0x800, func_8001E7A8);
}

void func_8001E838(void);

/** @brief Calls func_8001E65C with SPU transfer params (0x1100, 0x1900, 0x1000) and callback func_8001E838. */
void func_8001E808(void) {
    func_8001E65C(0x1100, 0x1900, 0x1000, func_8001E838);
}

/** @brief Calls func_8001E65C with SPU transfer params (0x2100, 0x2900, 0x1000) and callback func_8001E808. */
void func_8001E838(void) {
    func_8001E65C(0x2100, 0x2900, 0x1000, func_8001E808);
}

/**
 * @brief Unpacks struct fields into args, calls func_8001E0CC, then clears
 *        the IRQ voice bits from D_80075028[0].
 * @param a0 Pointer to 3-word struct: [data_ptr, loop_flag, callback].
 */
void func_8001E868(s32 *a0) {
    func_8001E0CC(a0[0], a0[1], a0[2]);
    D_80075028[0] &= ~D_80077298[3];
}

/** @brief Calls func_8001DF70. */
void func_8001E8B0(void) {
    func_8001DF70();
}

/**
 * @brief Sets stereo panning volumes for two consecutive SPU voices.
 *
 * Dereferences @p a0 for the raw volume value, stores it at D_80077298+0x40,
 * clears D_80077298+0x48. If the active flag at D_80077298+0xC is set,
 * sets left volume on the first voice and right volume on the second.
 *
 * @param a0 Pointer to the raw volume/pan value.
 */
void func_8001E8D0(s32 *a0) {
    u8 *base = (u8 *)D_80077298;
    s32 rawVal = *a0;
    *(s32 *)(base + 0x48) = 0;
    *(s32 *)(base + 0x40) = rawVal;
    if (*(s32 *)(base + 0xC) != 0) {
        s32 vol = (rawVal << 15) >> 16;
        func_80014E50(*(s32 *)(base + 0x10), vol, 0, 0);
        func_80014E50(*(s32 *)(base + 0x10) + 1, 0, (*(s32 *)(base + 0x40) << 15) >> 16, 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001E940);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001E9C0);

/** @brief Sets SPU IRQ address to 0x1038 and registers func_8001DF70 as the IRQ callback. */
void func_8001EB0C(void) {
    SpuSetIRQAddr(0x1038);
    SpuSetIRQCallback(func_8001DF70);
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001EB38);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001EC0C);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001EDD4);

/**
 * @brief Unpacks struct fields into 4 args, calls func_8001EC0C, then clears
 *        the IRQ voice bits from D_80075028[0].
 * @param a0 Pointer to 4-word struct.
 */
void func_8001F034(s32 *a0) {
    func_8001EC0C(a0[0], a0[1], a0[2], a0[3]);
    D_80075028[0] &= ~D_80077298[3];
}

/**
 * @brief Unpacks struct fields into 2 args, calls func_8001EDD4, then clears
 *        the IRQ voice bits from D_80075028[0].
 * @param a0 Pointer to 2-word struct.
 */
void func_8001F080(s32 *a0) {
    func_8001EDD4(a0[0], a0[1]);
    D_80075028[0] &= ~D_80077298[3];
}

/**
 * @brief Advances a sequence loop/repeat counter with wraparound.
 *
 * Increments the global tick count at D_80077298 offset +0x28, then
 * increments @p counter. If @p counter exceeds the loop limit
 * (D_80077298[0x3C/4] - 1), wraps it back to 0.
 *
 * @param counter Pointer to the current loop/repeat index.
 * @return The new value of @p counter after increment and possible wraparound.
 */
s32 func_8001F0C4(s32 *counter) {
    s32 base = (s32)D_80077298;
    *(s32 *)(base + 0x28) += 1;
    (*counter)++;
    if ((u32)(*(s32 *)(base + 0x3C) - 1) < (u32)*counter) {
        *counter = 0;
    }
    return *counter;
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001F118);

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001F2A8);

/**
 * @brief Sets up SPU voice parameters for dual-channel streaming, then calls
 *        func_8001F2A8 with callback func_8001F51C.
 */
void func_8001F370(void) {
    s32 *base = D_80077298;
    func_8001E308(base[4], 1, 0x1100, 0x2100);
    func_8001E308(base[4] + 1, 2, 0x1900, 0x2900);
    func_8001F2A8(0x2000, 0x2100, func_8001F51C);
}

INCLUDE_ASM("asm/nonmatchings/C9A8", func_8001F3D4);

void func_8001F54C(void);

/** @brief Calls func_8001F3D4 with SPU transfer params (0x1100, 0x1900, 0x1000) and callback func_8001F54C. */
void func_8001F51C(void) {
    func_8001F3D4(0x1100, 0x1900, 0x1000, func_8001F54C);
}

/** @brief Calls func_8001F3D4 with SPU transfer params (0x2100, 0x2900, 0x1000) and callback func_8001F51C. */
void func_8001F54C(void) {
    func_8001F3D4(0x2100, 0x2900, 0x1000, func_8001F51C);
}

/**
 * @brief Disables SPU IRQ, then sets up engine state from the given param struct.
 * @param a0 Pointer to 2-word struct: [field_0x2C, field_0x30].
 */
void func_8001F57C(s32 *a0) {
    s32 *base;
    func_8001DF70();
    base = D_80077298;
    base[2] = 0x1000000;
    base[11] = a0[0];
    base[12] = a0[1];
}

