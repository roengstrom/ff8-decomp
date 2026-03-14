#include "common.h"

/** @brief Call func_80098C44 with D_801D3C58 and a0. */
void func_8009E248(s32 a0) {
    extern u8 D_801D3C58[];
    func_80098C44(D_801D3C58, a0);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E270);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E464);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E640);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009E904);

/**
 * @brief Allocate a node in D_801D3C68 list with a callback from D_80182E4C.
 *
 * Looks up a callback pointer from the D_80182E4C table at index @p a0,
 * allocates a node via func_80098CC0, and initializes several fields.
 *
 * @param a0 Index into the D_80182E4C callback table.
 */
void func_8009EB30(s32 a0) {
    extern u8 D_801D3C68[];
    extern s32 D_80182E4C[];
    u8 *node = (u8 *)func_80098CC0(D_801D3C68, D_80182E4C[a0]);
    if (node != 0) {
        node[0xE] = a0;
        *(s16 *)(node + 0x20) = 0;
        node[0x22] = 0;
        node[0xC] = 0;
        node[0xD] = 0;
    }
}

/**
 * @brief Store a byte value at offset 0x22 of a structure.
 *
 * @param a0 Pointer to structure base.
 * @param a1 Byte value to store at offset 0x22.
 */
void func_8009EB90(u8 *a0, s32 a1) {
    a0[0x22] = a1;
}

/**
 * @brief Initialize the D_801D3C68 list with node pool D_801D3C78.
 *
 * Sets up a linked list with node size 0x24 and capacity 0x4.
 */
void func_8009EB98(void) {
    extern u8 D_801D3C68[];
    extern u8 D_801D3C78[];
    func_80098BC0(D_801D3C68, D_801D3C78, 0x24, 0x4);
}

/**
 * @brief Call func_80098D28 with D_801D3C68 and store result in D_801D3D08.
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009EBCC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009EBF4);

/**
 * Checks if any active slot has a pending action.
 *
 * Searches through a 2D table (2 rows of 5 entries, each 22 bytes apart,
 * rows 110 bytes apart) for an entry where byte 0 is not 0xFF and byte 1
 * is non-zero.
 *
 * @return 1 if a pending action was found, 0 otherwise.
 */
s32 func_8009EF68(void) {
    extern u8 D_801D3EC0[];
    s32 row = 0;
    u8 *base = D_801D3EC0;
    s32 marker = 0xFF;
    s32 rowOff = row;
    s32 col;
    s32 colOff;
    do {
        col = 0;
        colOff = rowOff;
        do {
            u8 *entry = (u8 *)(colOff + (s32)base);
            if (entry[0] != marker) {
                if (entry[1] != 0) {
                    return 1;
                }
            }
            col++;
            colOff += 0x16;
        } while (col < 5);
        row++;
        rowOff += 0x6E;
    } while (row < 2);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009EFD4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F17C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F5F0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F844);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009F908);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009FAF8);

/**
 * @brief Add a rendering command entry based on the alternate screen index.
 *
 * Reads D_801C2DCA, XORs with 1 to get the alternate index, computes
 * an offset of index * 92 into D_801C2DD0, and calls func_80098A1C
 * with the resulting pointer and D_8012E66C.
 *
 * @return Always 0.
 */
s32 func_8009FC40(void) {
    extern u8 D_801C2DCA;
    extern u8 D_801C2DD0[];
    extern u8 D_8012E66C[];
    s32 idx = D_801C2DCA ^ 1;
    func_80098A1C(D_801C2DD0 + idx * 92, D_8012E66C);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009FC90);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_8009FED0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A00EC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A01DC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A030C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0370);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A03DC);

/**
 * @brief Check if any active entity in D_801D4308 has a pending callback.
 *
 * Iterates through 10 entries (stride 32) checking if bit 0 of the
 * word at offset 4 is set and the byte at offset 8 is non-zero.
 *
 * @return 1 if a pending callback was found, 0 otherwise.
 */
s32 func_800A0A88(void) {
    extern u8 D_801D4308[];
    s32 i = 0;
    u8 *entry = D_801D4308;
    do {
        if (*(s32 *)(entry + 4) & 1) {
            if (entry[8] != 0) {
                return 1;
            }
        }
        i++;
        entry += 0x20;
    } while (i < 10);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0AD4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0B24);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A0F0C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1080);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1260);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1374);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A15C8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A18D0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object3", func_800A1BC4);
