#include "common.h"

extern u8 D_800F1A5C[];
extern u8 D_800F1A54[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF0E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF168);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF308);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF3D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF444);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF448);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF5A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF5C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF7E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF888);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF92C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF95C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF9CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BF9EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFA58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFBA8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFC14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFDA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFDB4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFE1C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFEC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFEF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800BFF38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0098);

/**
 * @brief Call func_800BFE1C for each of count entries at stride 0x18.
 *
 * Iterates count times, calling func_800BFE1C with a pointer that
 * advances by 0x18 bytes each iteration.
 *
 * @param ptr Base pointer to first entry.
 * @param count Number of entries to process.
 */
void func_800C00D8(u8 *ptr, s32 count) {
    s32 i = 0;
    while (i < count) {
        u8 *cur = ptr;
        ptr += 0x18;
        func_800BFE1C(cur);
        i++;
    }
}

extern u8 D_800F1940[];
extern u8 D_800F16C0[];

/**
 * @brief Initialize D_800F1940 buffer via func_800B2A00 and return it.
 *
 * Calls func_800B2A00 with D_800F1940, D_800F16C0, size 0x20, and count 0x14.
 *
 * @return Pointer to D_800F1940.
 */
u8 *func_800C0134(void) {
    u8 *buf = D_800F1940;
    func_800B2A00(buf, D_800F16C0, 0x20, 0x14);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0178);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C01A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C02D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C03DC);

/**
 * @brief Clear 9 halfwords in a 3x3 structure.
 *
 * Outer loop iterates 3 rows (stride 6). Inner loop clears 3 halfwords
 * per row starting at offset 4 and going backwards.
 *
 * @param a0 Base pointer to structure.
 */
void func_800C0780(s32 a0) {
    s32 j = 0;
outer:
    {
        s32 i = 2;
        s32 ptr = a0 + 4;
        do {
            *(s16 *)ptr = 0;
            i--;
            ptr -= 2;
        } while (i >= 0);
    }
    j++;
    a0 += 6;
    if (j < 3) goto outer;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C07B4);

/**
 * @brief Look up a sound entry and start playback if found.
 *
 * Sign-extends a0 to 16 bits and calls func_800C07B4 to look up
 * the entry. If not found (returns 0), returns 0. Otherwise fills
 * a local buffer with the result and calls func_80040564.
 *
 * @param a0 Sound ID (sign-extended to s16).
 * @param a1 Playback parameter.
 * @return a1 if playback started, 0 if entry not found.
 */
s32 func_800C0AE4(s32 a0, s32 a1) {
    s32 buf[3];
    s32 result = func_800C07B4((s16)a0);
    if (result != 0) {
        buf[2] = result;
        buf[1] = result;
        buf[0] = result;
        func_80040564(a1, buf);
        return a1;
    }
    return 0;
}

/**
 * @brief Look up sound, compute inverse ratio, and start playback.
 *
 * Sign-extends a0, looks up sound via func_800C07B4. If found,
 * calls func_80040FA4 with a1, computes 0x1000000 / result,
 * fills a buffer with the quotient, and calls func_80040564.
 *
 * @param a0 Sound ID (sign-extended to s16).
 * @param a1 Playback parameter.
 * @return a1 if playback started, 0 if entry not found.
 */
s32 func_800C0B3C(s32 a0, s32 a1) {
    s32 buf[3];
    s32 result = func_800C07B4((s16)a0);
    if (result != 0) {
        s32 quotient;
        func_80040FA4(a1, a1);
        quotient = 0x1000000 / result;
        buf[2] = quotient;
        buf[1] = quotient;
        buf[0] = quotient;
        func_80040564(a1, buf);
        return a1;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0BB8);

/**
 * @brief Check if a table entry at the given index is valid.
 *
 * Loads a table pointer from a0->0x64->0xC. If the table's first
 * halfword (count) is less than a1, returns 0. Otherwise checks
 * the halfword at index a1; returns 1 if non-zero, 0 if zero.
 *
 * @param a0 Entity pointer with table reference at offset 0x64.
 * @param a1 Table index to check.
 * @return 1 if entry is valid, 0 otherwise.
 *
 * @note Non-matching: CC1PSX converts `if (val != 0) return 1; return 0;`
 * into `sltu v0,$0,v0` (boolean conversion) and fills the first branch
 * delay slot with the sll computation instead of the return value.
 * Original uses separate bnez+delay patterns for each return path.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0CB8);

/**
 * @brief Store byte to D_800F1A5C target, then look up address from table.
 *
 * Stores a0 to the byte pointed to by D_800F1A5C. Then reads a table base
 * pointer from D_800F1A54->0x64->0xC, uses a0 as an index into a u16 offset
 * table, and returns the base pointer plus that offset.
 *
 * @param a0 Index value (also stored as byte).
 * @return Resolved pointer from table lookup.
 */
s32 func_800C0D00(s32 a0) {
    s32 ptr;
    s32 base;
    *(u8 *)(*(s32 *)D_800F1A5C) = a0;
    ptr = *(s32 *)(*(s32 *)D_800F1A54 + 0x64);
    a0 &= 0xFF;
    base = *(s32 *)(ptr + 0xC);
    return base + *(u16 *)(base + a0 * 2);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0D3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0DB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C0FD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C112C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C15EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C197C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C22A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2528);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C26B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2868);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2888);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C28B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C29C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2A18);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2A38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object12", func_800C2AB0);
