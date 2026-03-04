#include "common.h"

extern s32 D_80074EB8[];
extern u8 D_80070D60[];
extern s32 D_80073CA8;
extern u8 *D_80073C34;

INCLUDE_ASM("asm/nonmatchings/75B4", func_80016DB4);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80016E08);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80016E5C);

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

INCLUDE_ASM("asm/nonmatchings/75B4", func_8001708C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017410);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800174E4);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017880);

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

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017AAC);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017C9C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017D14);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017D5C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80017DB0);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018158);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800181D4);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018234);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018358);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018438);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018610);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018650);

INCLUDE_ASM("asm/nonmatchings/75B4", func_8001869C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_8001871C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018784);

/** @brief Calls func_800174E4 with two words loaded from the input pointer. */
void func_800188B4(s32 *a0) {
    func_800174E4(a0[0], a0[1]);
}

INCLUDE_ASM("asm/nonmatchings/75B4", func_800188E0);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018908);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018A74);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018B28);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018C48);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018D40);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018D74);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018DDC);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018E4C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80018F34);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800190B4);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80019130);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800191F8);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800192D8);

INCLUDE_ASM("asm/nonmatchings/75B4", func_80019450);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800194C8);

INCLUDE_ASM("asm/nonmatchings/75B4", func_8001958C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_8001966C);

INCLUDE_ASM("asm/nonmatchings/75B4", func_800197F4);

INCLUDE_ASM("asm/nonmatchings/75B4", func_8001984C);

