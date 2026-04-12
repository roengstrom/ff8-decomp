#include "common.h"

typedef struct {
    u8 pad00[3];
    s8 flag;
} D_800EBF24_Type;

extern D_800EBF24_Type *D_800EBF24[];
extern u8 D_800FB408[];
extern u8 D_800FA5F8[];
extern u8 D_800E6658[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C7294); /* 0x58 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C72EC); /* 0x68 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C7354); /* 0xF8 */

/**
 * @brief Register a particle effect with a callback from D_800E6658 table.
 *
 * Looks up a callback function pointer from D_800E6658[idx], allocates
 * an entry from D_800FB408, clears initial fields, and stores the
 * source entity pointer.
 *
 * @param a0 Source entity pointer (stored at result offset 0x20).
 * @param idx Index into D_800E6658 callback table.
 */
void func_800C744C(s32 a0, s32 idx) {
    u8 *entry = (u8 *)func_800B2A84(D_800FB408, *(s32 *)(D_800E6658 + idx * 4));
    *(u16 *)(entry + 0xC) = 0;
    *(u16 *)(entry + 0xE) = 0;
    *(s32 *)(entry + 0x20) = a0;
}

/**
 * @brief Initialize D_800FB408 buffer via func_800B2A00 and return it.
 *
 * Calls func_800B2A00 with D_800FB408, D_800FA5F8, size 0x24, and count 0x64.
 *
 * @return Pointer to D_800FB408.
 */
u8 *func_800C749C(void) {
    u8 *buf = D_800FB408;
    func_800B2A00(buf, D_800FA5F8, 0x24, 0x64);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C74E0); /* 0x1A8 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C7688); /* 0x44 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C76CC); /* 0xC0 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C778C); /* 0x96C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C80F8); /* 0x2C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8124); /* 0x124 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8248); /* 0x2D0 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8518); /* 0x10 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8528); /* 0x18C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C86B4); /* 0x114 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C87C8); /* 0xE8 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C88B0); /* 0x8C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C893C); /* 0x13C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8A78); /* 0xB0 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8B2C); /* 0xA8 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8BD4); /* 0x138 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8D0C); /* 0x3C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8D48); /* 0x68 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8DB0); /* 0xA4 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8E54); /* 0xBC */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8F10); /* 0xCC */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8FDC); /* 0x38 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9014); /* 0xBC */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C90D0); /* 0xB0 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9180); /* 0xAC */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C922C); /* 0x90 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C92BC); /* 0xB8 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9374); /* 0xB0 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9424); /* 0x94 */

/**
 * @brief Look up a pointer from D_800EBF24 array with fallback.
 *
 * Loads the entry at D_800EBF24[index]. If the entry's flag byte is
 * non-zero, returns D_800EBF24[0] as fallback.
 *
 * @param index Array index.
 * @return Entry from D_800EBF24[index] or D_800EBF24[0] on fallback.
 */
D_800EBF24_Type *func_800C94B8(s32 index) {
    D_800EBF24_Type *entry = D_800EBF24[index];

    if (entry->flag == 0)
        return entry;

    return D_800EBF24[0];
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C94EC); /* 0x13C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9628); /* 0xBC */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C96E4); /* 0x100 */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C97E4); /* 0x62C */

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9E10); /* 0x13C */
