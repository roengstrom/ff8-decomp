#include "common.h"

extern u8 D_800EE9E8[];

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7004);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D7038);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8F54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object19", func_800D8F90);
