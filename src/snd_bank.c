#include "common.h"

extern s32 D_80074EB8[];
extern u8 D_80070D60[];
extern s32 D_80073CA8;
extern u8 *D_80073C34;

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80016DB4);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80016E08);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80016E5C);

/**
 * @brief Determines which sound bank group a given identifier belongs to.
 *
 * Compares @p a0 against entries in D_80074EB8 (a table of sound bank IDs).
 * Indices 1 and 4 map to group 1; indices 2 and 5 map to group 2.
 *
 * @param a0 Sound bank or instrument identifier to look up.
 * @return 1 if in group 1, 2 if in group 2, 0 if not found or @p a0 is 0.
 */
s32 func_80016F3C(s32 a0) {
    s32 ret = 0;
    if (a0 == 0) return ret;
    if (a0 == D_80074EB8[1] || a0 == D_80074EB8[4]) {
        ret = 1;
    } else if (a0 == D_80074EB8[2] || a0 == D_80074EB8[5]) {
        ret = 2;
    }
    return ret;
}

/**
 * @brief Initializes a sequence track structure to default values.
 *
 * Sets the track's instrument bank ID at offset +0x00, initializes
 * default pitch bend (0x6E00 at +0x80), default tempo (0x32000000 at +0x44),
 * sentinel voice value (0xFFFF at +0x6E), and zeroes all modulation,
 * panning, volume, and effect fields. Finally applies instrument table
 * entry 0 via func_8001C1DC.
 *
 * @param a0 Pointer to the track structure (stride 0x110).
 * @param a1 Instrument bank ID to assign to this track.
 */
void func_80016FA8(void *a0, s32 a1) {
    *(s16*)((u8*)a0 + 0x80) = 0x6E00;
    *(s32*)((u8*)a0 + 0x44) = 0x32000000;
    *(s32*)a0 = a1;
    *(s16*)((u8*)a0 + 0xE4) = 0;
    *(s16*)((u8*)a0 + 0xE6) = 0;
    *(s16*)((u8*)a0 + 0x98) = 0;
    *(s32*)((u8*)a0 + 0x2C) = 0;
    *(s16*)((u8*)a0 + 0xEA) = 0;
    *(s16*)((u8*)a0 + 0x90) = 0;
    *(s16*)((u8*)a0 + 0xD8) = 0;
    *(s16*)((u8*)a0 + 0xD6) = 0;
    *(s16*)((u8*)a0 + 0x86) = 0;
    *(s32*)((u8*)a0 + 0x50) = 0;
    *(s16*)((u8*)a0 + 0xD4) = 0;
    *(s32*)((u8*)a0 + 0x30) = 0;
    *(s16*)((u8*)a0 + 0xF2) = 0;
    *(s16*)((u8*)a0 + 0x9A) = 0;
    *(u16*)((u8*)a0 + 0x6E) = 0xFFFF;
    *(s16*)((u8*)a0 + 0x10A) = 0;
    *(s16*)((u8*)a0 + 0xCA) = 0;
    *(s16*)((u8*)a0 + 0xBC) = 0;
    *(s16*)((u8*)a0 + 0xAA) = 0;
    *(s16*)((u8*)a0 + 0xCC) = 0;
    *(s16*)((u8*)a0 + 0xBE) = 0;
    *(s16*)((u8*)a0 + 0xAC) = 0;
    *(s16*)((u8*)a0 + 0xD2) = 0;
    *(s16*)((u8*)a0 + 0xD0) = 0;
    func_8001C1DC(a0, 0);
}

/**
 * @brief Builds a voice bitmask from a track bitmask.
 *
 * Iterates over 32 track entries (stride 0x110). For each track whose bit
 * is set in @p a1, reads the assigned voice index at offset +0xF4. If the
 * voice index is valid (< 24), sets the corresponding bit in the result.
 *
 * @param a0 Base pointer to the sequence track array.
 * @param a1 Bitmask of tracks to include (bit N = track N).
 * @return Bitmask of SPU voices used by the selected tracks.
 */
s32 func_80017040(u8 *a0, s32 a1) {
    u32 i = 0;
    s32 result = 0;
    s32 bit = 1;
    do {
        if (a1 & (bit << i)) {
            s32 val = *(s32 *)(a0 + 0xF4);
            if ((u32)val < 0x18) {
                result |= (bit << val);
            }
        }
        i++;
        a0 += 0x110;
    } while (i < 0x20);
    return result;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001708C);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017410);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800174E4);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017880);

/**
 * @brief Releases a voice from all active sequence tracks (duplicate of func_80016280).
 *
 * Iterates over 32 track entries in D_80070D60 (stride 0x110, voice at +0xF4).
 * Any track assigned to voice @p a0 is reassigned to 24 (none). Also checks
 * the secondary sequence array D_80073C34 if D_80073CA8 is set.
 *
 * @param a0 SPU voice index to release (0-23).
 */
void func_80017A2C(s32 a0) {
    s32 base = (s32)D_80070D60;
    s32 i = 32;
    s32 replacement = 24;
    s32 ptr = base + 0xF4;
    do {
        if (*(s32 *)ptr == a0) {
            *(s32 *)ptr = replacement;
        }
        i--;
        ptr += 0x110;
    } while (i != 0);
    if (D_80073CA8 != 0) {
        base = (s32)D_80073C34;
        i = 32;
        replacement = 24;
        ptr = base + 0xF4;
        do {
            if (*(s32 *)ptr == a0) {
                *(s32 *)ptr = replacement;
            }
            i--;
            ptr += 0x110;
        } while (i != 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017AAC);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017C9C);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017D14);

/**
 * @brief Set voice control bits for tracks matching D_80075028[0] mask.
 *
 * Loads a track bitmask from D_80075028[0]. For each set bit (starting
 * from bit 12), sets bits 0 and 1 in the track's voice control word at
 * D_80072F70 + track*0x110 + 0xF8.
 */
void func_80017D5C(void) {
    extern u8 D_80072F70[];
    extern s32 D_80075028[];
    s32 tmp = D_80075028[0];
    s32 mask;
    s32 bit;
    s32 base;
    u8 *ptr;
    if (tmp == 0) {
        return;
    }
    mask = tmp;
    bit = 0x1000;
    base = (s32)D_80072F70;
    ptr = (u8 *)(base + 0xF8);
    do {
        if (mask & bit) {
            *(s32 *)ptr |= 3;
            mask ^= bit;
        }
        ptr += 0x110;
        bit <<= 1;
    } while (mask != 0);
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017DB0);

/**
 * @brief Plays or stops a sound depending on the active sequence ID.
 *
 * Checks the halfword at D_80073DF0+0x5E (active sequence ID). If non-zero
 * and matches a0[8], calls func_80017DB0(a0[0], 0) to stop. Otherwise calls
 * func_8001708C(a0[0], -1) and stores a0[8] into the structure pointed to
 * by D_80074F08 at offset 0x5E.
 *
 * @param a0 Pointer to a sound config structure (word 0 = track ID, halfword 8 = sequence ID).
 *
 * @note Non-matching: GCC 2.8.0 folds symbol+constant even with (s32) cast
 * local variable trick. Multiple scheduling and register allocation differences.
 */
INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018158);

/**
 * @brief Configure audio track and update playback globals.
 *
 * Calls func_8001708C with the first and fourth fields of the input struct,
 * copies field8 to D_80074F08->0x5E, and sets D_80074EB0 to field10-1
 * (or 0 if field10 is 0).
 *
 * @param a0 Pointer to audio config struct.
 */
void func_800181D4(s32 *a0) {
    extern s32 *D_80074F08;
    extern s32 D_80074EB0;
    register s32 result asm("$4");
    s32 val;
    func_8001708C(a0[0], a0[3]);
    *(u16 *)((u8 *)D_80074F08 + 0x5E) = *(u16 *)((u8 *)a0 + 8);
    val = a0[4];
    result = 0;
    if (val != 0) {
        result = val - 1;
    }
    D_80074EB0 = result;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018234);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018358);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018438);

/**
 * @brief Process a sound voice and compute the remaining play count.
 *
 * Calls func_80018158 on the voice structure, then reads the play count
 * at offset +0x0C. If non-zero, decrements by 1 and stores to D_80074EB0;
 * otherwise stores 0.
 *
 * @param a0 Pointer to the sound voice structure.
 */
void func_80018610(u8 *a0) {
    extern s32 D_80074EB0;
    s32 val, result;
    func_80018158(a0);
    val = *(s32 *)(a0 + 0xC);
    result = 0;
    if (val != 0) {
        result = val - 1;
    }
    D_80074EB0 = result;
}

/**
 * @brief Saves current sound parameters, replaces with defaults, and triggers playback.
 *
 * Saves the first two words of the structure at @p a0, then overwrites
 * with default volume/pan parameters (0x400, 0x1000000, 0x80, 0x7F, 0)
 * and calls func_80017AAC with the saved values.
 *
 * @param a0 Pointer to a sound parameter structure (5 words).
 */
void func_80018650(s32 *a0) {
    s32 a1 = a0[0];
    s32 a2 = a0[1];
    a0[0] = 0x400;
    a0[1] = 0x1000000;
    a0[2] = 0x80;
    a0[3] = 0x7F;
    a0[4] = 0;
    func_80017AAC(a0, a1, a2, 0);
}

/**
 * @brief Set default sound parameters, look up voice, and play.
 *
 * Resolves the SPU voice address via func_80017C9C, sets default volume
 * (0x2000000), pan (0x80), and envelope (0x7F) on the parameter structure,
 * looks up the instrument group via D_80074ED8, and plays via func_80017AAC.
 *
 * @param a0 Pointer to the sound parameter structure.
 */
void func_8001869C(s32 *a0) {
    extern u8 *D_80074ED8;
    s32 local10, local14;

    func_80017C9C(&local10, &local14, a0[0]);
    a0[1] = 0x2000000;
    a0[2] = 0x80;
    a0[3] = 0x7F;
    a0[4] = func_80016F3C(*(u16 *)(D_80074ED8 + a0[0] * 2));
    func_80017AAC(a0, local10, local14, 0);
}

/**
 * @brief Look up and play a sound voice by index.
 *
 * Resolves the SPU voice address via func_80017C9C, reads the sample pitch
 * from D_80074ED8 table, processes it with func_80016F3C, stores the result
 * at offset +0x10 of the parameter structure, and plays via func_80017AAC.
 *
 * @param a0 Pointer to the sound parameter structure.
 */
void func_8001871C(s32 *a0) {
    extern u8 *D_80074ED8;
    s32 local10, local14;

    func_80017C9C(&local10, &local14, a0[0]);
    a0[4] = func_80016F3C(*(u16 *)(D_80074ED8 + a0[0] * 2));
    func_80017AAC(a0, local10, local14, 0);
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018784);

/** @brief Calls func_800174E4 with two words loaded from the input pointer. */
void func_800188B4(s32 *a0) {
    func_800174E4(a0[0], a0[1]);
}

/**
 * @brief Clears 12 entries (stride 16) in the D_80074F20 array.
 * Writes zero to offset 0 of each entry, iterating backward.
 */
void func_800188E0(void) {
    extern u8 D_80074F20;
    s32 i = 12;
    s32 base = (s32)&D_80074F20;
top:
    i--;
    *(s32 *)(base + i * 16) = 0;
    if (i != 0) goto top;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018908);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018A74);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018B28);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018C48);

/**
 * @brief Clears the sound fade counter and sets SPU transfer address.
 *
 * Sets D_80074FE4 to 0, loads the halfword at @p a0, shifts it left 16,
 * stores to D_80075078, and calls func_8001A55C.
 *
 * @param a0 Pointer to a halfword containing the SPU address high bits.
 */
void func_80018D40(u16 *a0) {
    extern u16 D_80074FE4;
    extern s32 D_80075078;
    D_80074FE4 = 0;
    D_80075078 = *a0 << 16;
    func_8001A55C();
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018D74);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018DDC);

/**
 * @brief Apply a masked pitch value from a parameter struct to track entries.
 *
 * If a0[1] is nonzero, iterates 12 entries checking D_80075028[0] bit
 * AND a0[1] against entry flags at +0x24. If both match, reads halfword
 * from a0+8, masks to 7 bits, shifts left 8, stores to entry +0xDE,
 * clears +0x8A, sets bits 0x3 in entry +0xF8.
 *
 * If a0[1] is zero, iterates 12 entries checking D_80075028[0] bit
 * AND comparing entry +0x38 to a0[0]. If both match, same processing.
 *
 * @param a0 Pointer to a parameter structure (word 0 = match ID,
 *           word 1 = flag mask, halfword 8 = pitch value).
 */
void func_80018E4C(u8 *a0) {
    extern u8 D_80072F70[];
    extern s32 D_80075028[];
    s32 base = (s32)D_80072F70;
    s32 mask = D_80075028[0];
    s32 flagCheck = *(s32 *)(a0 + 4);

    if (flagCheck != 0) {
        s32 bit = 0x1000;
        s32 i = 0;
        s32 ptr = base + 0xF8;
    top1:
        if (mask & bit) {
            s32 f24 = *(s32 *)(ptr - 0xD4);
            s32 chk = *(s32 *)(a0 + 4);
            if (f24 & chk) {
                s32 val = *(u16 *)(a0 + 8);
                s32 f = *(s32 *)ptr;
                *(u16 *)(ptr - 0x6E) = 0;
                val &= 0x7F;
                *(u16 *)(ptr - 0x1A) = val << 8;
                *(s32 *)ptr = f | 0x3;
            }
        }
        i++;
        ptr += 0x110;
        bit <<= 1;
        if ((u32)i < 0xC) goto top1;
    } else {
        s32 bit = 0x1000;
        s32 i = 0;
        s32 ptr = base + 0xF8;
    top2:
        if (mask & bit) {
            if (*(s32 *)(ptr - 0xC0) == *(s32 *)a0) {
                s32 val = *(u16 *)(a0 + 8);
                s32 f = *(s32 *)ptr;
                *(u16 *)(ptr - 0x6E) = 0;
                val &= 0x7F;
                *(u16 *)(ptr - 0x1A) = val << 8;
                *(s32 *)ptr = f | 0x3;
            }
        }
        i++;
        ptr += 0x110;
        bit <<= 1;
        if ((u32)i < 0xC) goto top2;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018F34);

/**
 * @brief Apply a masked pitch value to track entries matching the active voice mask.
 *
 * Iterates over 12 entries at D_80072F70 (stride 0x110). For each entry
 * whose corresponding bit is set in D_80075028[0] (starting from bit 12)
 * and whose flags at offset +0x24 do not have bit 0x2000000 set, reads
 * a halfword from @p a0, masks to 7 bits, shifts left by 8, and stores
 * to entry offset +0xDE. Also clears the halfword at +0x8A and sets
 * bits 0x3 in the flags at +0xF8.
 *
 * @param a0 Pointer to a halfword value to apply to eligible tracks.
 */
void func_800190B4(u16 *a0) {
    extern u8 D_80072F70[];
    extern s32 D_80075028[];
    s32 bit = 0x1000;
    s32 mask = D_80075028[0];
    s32 i = 0;
    s32 flag = 0x2000000;
    s32 base = (s32)D_80072F70;
    s32 ptr = base + 0xF8;
top:
    if (mask & bit) {
        if (!(*(s32 *)(ptr - 0xD4) & flag)) {
            s32 val = *a0;
            s32 f = *(s32 *)ptr;
            *(u16 *)(ptr - 0x6E) = 0;
            val &= 0x7F;
            *(u16 *)(ptr - 0x1A) = val << 8;
            *(s32 *)ptr = f | 0x3;
        }
    }
    i++;
    ptr += 0x110;
    bit <<= 1;
    if ((u32)i < 0xC) goto top;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80019130);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800191F8);

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800192D8);

/**
 * @brief Apply a pitch byte to track entries matching the active voice mask.
 *
 * Iterates over 12 entries at D_80072F70 (stride 0x110). For each entry
 * whose corresponding bit is set in D_80075028[0] (starting from bit 12,
 * shifting left) and whose flags at offset +0x24 do not have bit 0x2000000
 * set, reads a byte from @p a0, shifts it left by 8, and stores to entry
 * offset +0x6A. Also clears the halfword at +0x6C and sets bits 0x3 in
 * the flags at +0xF8.
 *
 * @param a0 Pointer to a byte value to apply to eligible tracks.
 */
void func_80019450(u8 *a0) {
    extern u8 D_80072F70[];
    extern s32 D_80075028[];
    s32 bit = 0x1000;
    s32 mask = D_80075028[0];
    s32 i = 0;
    s32 flag = 0x2000000;
    s32 base = (s32)D_80072F70;
    s32 ptr = base + 0xF8;
top:
    if (mask & bit) {
        if (!(*(s32 *)(ptr - 0xD4) & flag)) {
            s32 val = *(u8 *)a0;
            s32 f = *(s32 *)ptr;
            *(u16 *)(ptr - 0x8C) = 0;
            *(u16 *)(ptr - 0x8E) = val << 8;
            *(s32 *)ptr = f | 0x3;
        }
    }
    i++;
    ptr += 0x110;
    bit <<= 1;
    if ((u32)i < 0xC) goto top;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_800194C8);

/**
 * @brief Apply pitch from a parameter struct to track entries based on flags.
 *
 * If a0[1] is nonzero, iterates 12 entries checking D_80075028[0] bit
 * AND a0[1] against entry flags at +0x24. If both match, reads byte
 * from a0+8, shifts left 8, stores to entry +0x3C, clears +0x84, sets
 * bit 0x10 in entry +0xF8.
 *
 * If a0[1] is zero, iterates 12 entries checking D_80075028[0] bit
 * AND comparing entry +0x38 to a0[0]. If both match, same processing.
 *
 * @param a0 Pointer to a parameter structure (word 0 = match ID,
 *           word 1 = flag mask, byte 8 = pitch value).
 */
void func_8001958C(u8 *a0) {
    extern u8 D_80072F70[];
    extern s32 D_80075028[];
    s32 base = (s32)D_80072F70;
    s32 mask = D_80075028[0];
    s32 flagCheck = *(s32 *)(a0 + 4);

    if (flagCheck != 0) {
        s32 bit = 0x1000;
        s32 i = 0;
        s32 ptr = base + 0xF8;
    top1:
        if (mask & bit) {
            s32 f24 = *(s32 *)(ptr - 0xD4);
            s32 chk = *(s32 *)(a0 + 4);
            if (f24 & chk) {
                s32 val = *(u8 *)(a0 + 8);
                s32 f = *(s32 *)ptr;
                *(u16 *)(ptr - 0x74) = 0;
                *(s32 *)(ptr - 0xBC) = val << 8;
                *(s32 *)ptr = f | 0x10;
            }
        }
        i++;
        ptr += 0x110;
        bit <<= 1;
        if ((u32)i < 0xC) goto top1;
    } else {
        s32 bit = 0x1000;
        s32 i = 0;
        s32 ptr = base + 0xF8;
    top2:
        if (mask & bit) {
            if (*(s32 *)(ptr - 0xC0) == *(s32 *)a0) {
                s32 val = *(u8 *)(a0 + 8);
                s32 f = *(s32 *)ptr;
                *(u16 *)(ptr - 0x74) = 0;
                *(s32 *)(ptr - 0xBC) = val << 8;
                *(s32 *)ptr = f | 0x10;
            }
        }
        i++;
        ptr += 0x110;
        bit <<= 1;
        if ((u32)i < 0xC) goto top2;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001966C);

/**
 * @brief Apply a pitch byte to all 12 track entries if not flagged.
 *
 * Iterates over 12 entries at D_80072F70 (stride 0x110). For each entry
 * whose flags at offset +0x24 do not have bit 0x2000000 set, reads a
 * byte from @p a0, shifts it left by 8, and stores to entry offset +0x3C.
 * Also clears the halfword at +0x84 and sets bit 0x10 in the flags at +0xF8.
 *
 * @param a0 Pointer to a byte value to apply to all eligible tracks.
 */
void func_800197F4(u8 *a0) {
    extern u8 D_80072F70[];
    s32 i = 12;
    s32 flag = 0x2000000;
    s32 base = (s32)D_80072F70;
    s32 ptr = base + 0xF8;
top:
    if (!(*(s32 *)(ptr - 0xD4) & flag)) {
        s32 val = *a0;
        s32 f = *(s32 *)ptr;
        *(u16 *)(ptr - 0x74) = 0;
        *(s32 *)(ptr - 0xBC) = val << 8;
        *(s32 *)ptr = f | 0x10;
    }
    i--;
    ptr += 0x110;
    if (i != 0) goto top;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001984C);

