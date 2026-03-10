#include "common.h"

extern u8 D_800EBF24[];
extern u8 D_800FB408[];
extern u8 D_800FA5F8[];
extern u8 D_800E6658[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C7294);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C72EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C7354);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C74E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C7688);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C76CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C778C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C80F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8124);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8248);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8518);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8528);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C86B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C87C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C88B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C893C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8A78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8B2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8BD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8D0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8D48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8DB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8E54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8F10);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C8FDC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9014);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C90D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9180);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C922C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C92BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9374);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9424);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C94B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C94EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9628);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C96E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C97E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object15", func_800C9E10);
