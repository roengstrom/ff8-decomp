#include "common.h"

extern u8 D_800EE9E8[];
extern u8 D_801032A0[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6AF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6BAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6D00);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6D64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6D80);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6E34);

/**
 * @brief Return the address of D_800EE9E8.
 *
 * @return Address of D_800EE9E8 as an integer.
 */
s32 func_800D6E8C(void) {
    return (s32)D_800EE9E8;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6E98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6F10);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D6FA8);

/**
 * @brief Count the last nonzero entry position in a 4-element array.
 *
 * Iterates 4 entries at stride 4, checking byte[0] of each.
 * Returns the 1-based index of the last nonzero entry, or 0 if all zero.
 *
 * @param a0 Pointer to array of entries (stride 4, first byte checked).
 * @return 1-based position of last nonzero entry, or 0.
 */
s32 func_800D7004(u8 *a0) {
    s32 count = 0;
    s32 i = count;
    do {
        if (*a0 != 0) {
            count = i + 1;
        }
        i++;
        a0 += 4;
    } while (i < 4);
    return count;
}

/**
 * @brief Find last non-zero entry index in a 2-element array (stride 4).
 *
 * Iterates 2 entries at stride 4. Tracks the index of the last non-zero
 * byte found, and returns that index + 1.
 *
 * @param a0 Pointer to array of entries (stride 4, first byte checked).
 * @return Index of last non-zero entry + 1.
 */
s32 func_800D7038(u8 *a0) {
    s32 result = 0;
    s32 i = 0;
    do {
        if (*a0 != 0) {
            result = i;
        }
        i++;
        a0 += 4;
    } while (i < 2);
    return result + 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D706C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7110);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D71DC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D72C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D741C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D74A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D75D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7604);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7884);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7A74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7B14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7C24);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7D84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8034);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8264);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8374);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8484);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D84D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8564);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D85B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D868C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D86A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8734);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D87B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D87E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D88A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D88C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D88E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8918);

/**
 * @brief Set bit 14 (0x4000) of the scratchpad control halfword at 0x1F8003AE.
 *
 * Uses base pointer 0x1F800390 with offset 0x1E to access the control register.
 */
void func_800D8A78(void) {
    s32 base = 0x1F800390;
    REGALLOC_BARRIER(base);
    *(u16 *)(base + 0x1E) |= 0x4000;
}

/**
 * @brief Set bit 13 (0x2000) of the scratchpad control halfword at 0x1F8003AE.
 *
 * Uses base pointer 0x1F800390 with offset 0x1E to access the control register.
 */
void func_800D8A94(void) {
    s32 base = 0x1F800390;
    REGALLOC_BARRIER(base);
    *(u16 *)(base + 0x1E) |= 0x2000;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8AB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8B4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8C18);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8CB4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8D7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8DE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8E74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8E88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8EA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8EAC);

/**
 * @brief Clear bytes at offsets 0x34+ in D_801032A0 based on count at offset 0x24.
 *
 * Reads the count from D_801032A0[0x24]. For i = 0 to count-1, sets
 * D_801032A0[0x34 + i] to zero.
 */
void func_800D8F54(void) {
    u8 *base = D_801032A0;
    s32 i = 0;
    if (base[0x24] != 0) {
        do {
            *(u8 *)(base + i + 0x34) = 0;
            i++;
        } while (i < base[0x24]);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8F90);
