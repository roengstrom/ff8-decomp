#include "common.h"

INCLUDE_ASM("asm/nonmatchings/57FC", func_80014FFC);

INCLUDE_ASM("asm/nonmatchings/57FC", func_800150A8);

INCLUDE_ASM("asm/nonmatchings/57FC", func_800151C4);

INCLUDE_ASM("asm/nonmatchings/57FC", func_80015668);

INCLUDE_ASM("asm/nonmatchings/57FC", func_80015AC8);

/**
 * @brief Finds the SPU voice with the lowest priority and frees it for reuse.
 *
 * Scans all 24 voice entries in the D_80073CB0 array (stride 8, priority
 * at offset +4) to find the voice with the lowest priority value. If found,
 * calls func_80016280 to release it from all active sequences.
 *
 * @return The index of the freed voice, or 0x18 (24) if no voice is available.
 */
s32 func_80015F4C(void) {
    extern u8 D_80073CB0[];
    u32 i;
    u16 min_val;
    u32 min_idx;
    u8 *p;

    i = 0;
    min_val = 0x7FFF;
    min_idx = 0x18;
    p = D_80073CB0;

    do {
        if (*(s16 *)(p + 4) < (s16)min_val) {
            min_val = *(u16 *)(p + 4);
            min_idx = i;
        }
        i++;
        p += 8;
    } while (i < 0x18);

    if ((s16)min_val == 0x7FFF) {
        return 0x18;
    }
    if (min_idx < 0x18) {
        func_80016280(min_idx);
    }
    return min_idx;
}

INCLUDE_ASM("asm/nonmatchings/57FC", func_80015FE0);

extern u8 D_80070D60[];
extern s32 D_80073CA8;
extern u8 *D_80073C34;

/**
 * @brief Releases a voice from all active sequence tracks.
 *
 * Iterates over 32 track entries in the D_80070D60 array (stride 0x110,
 * voice assignment at offset +0xF4). Any track assigned to voice @p a0
 * is reassigned to voice 24 (invalid/none). Also checks the secondary
 * sequence array D_80073C34 if D_80073CA8 is set.
 *
 * @param a0 SPU voice index to release (0-23).
 */
void func_80016280(s32 a0) {
    s32 base = (s32)D_80070D60;
    s32 i = 32;
    s32 replacement = 24;
    s32 ptr = base + 0xF4;
    do {
        if (a0 == *(s32 *)ptr) {
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
            if (a0 == *(s32 *)ptr) {
                *(s32 *)ptr = replacement;
            }
            i--;
            ptr += 0x110;
        } while (i != 0);
    }
}

/**
 * @brief Checks if a voice is assigned to any active track matching a bitmask.
 *
 * Iterates over 32 track entries (stride 0x110). For each track whose voice
 * assignment (offset +0xF4) equals @p a2, checks whether the corresponding
 * bit in @p a1 is set.
 *
 * @param a0 Base pointer to the sequence track array.
 * @param a1 Bitmask of tracks to check (bit N = track N).
 * @param a2 SPU voice index to search for.
 * @return 1 if any matching track has its bit set in @p a1, 0 otherwise.
 */
s32 func_80016300(u8 *a0, s32 a1, s32 a2) {
    u32 i = 0;
    s32 bit = 1;
    do {
        if (a2 == *(s32 *)(a0 + 0xF4)) {
            if (a1 & (bit << i)) {
                return 1;
            }
        }
        i++;
        a0 += 0x110;
    } while (i < 0x20);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/57FC", func_80016344);

INCLUDE_ASM("asm/nonmatchings/57FC", func_80016478);

extern s32 D_80074F1C;
/**
 * @brief Stores the current global tempo/timing value into a track structure.
 *
 * Copies D_80074F1C (a global timing or tick counter) into the track
 * structure field at offset 0x50.
 *
 * @param a0 Pointer to a sequence track structure.
 */
void func_800164C8(u8 *a0) {
    *(s32 *)(a0 + 0x50) = D_80074F1C;
}

INCLUDE_ASM("asm/nonmatchings/57FC", func_800164D8);

INCLUDE_ASM("asm/nonmatchings/57FC", func_80016A1C);

INCLUDE_ASM("asm/nonmatchings/57FC", func_80016A7C);

INCLUDE_ASM("asm/nonmatchings/57FC", func_80016C28);

