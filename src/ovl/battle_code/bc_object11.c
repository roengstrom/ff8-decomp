#include "common.h"

extern u8 D_800F16A4[];
extern u8 D_800F16A8[];
extern u8 D_800EF724[];
extern u8 D_800F02F4[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BAEE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB024);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB084);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB0DC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB140);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB180);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB1B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB1B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB1E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB368);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB3F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB63C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB828);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB9A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBA54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBBF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBD00);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBD04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBE48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC060);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC3B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC420);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC7D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC818);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC88C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC908);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC9C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BCA3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BCF6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD040);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD06C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD0B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD1B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD230);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD260);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD2AC);

/**
 * @brief Read byte at offset 0x11 from pointer stored in D_800F16A4.
 *
 * @return The byte value.
 */
s32 func_800BD2D4(void) {
    return *(u8 *)(*(s32 *)D_800F16A4 + 0x11);
}

/**
 * @brief Read byte at offset 0x3 from pointer in D_800F16A8, mask with 0x2.
 *
 * @return Bit 1 of byte at offset 3.
 */
s32 func_800BD2EC(void) {
    return *(u8 *)(*(s32 *)D_800F16A8 + 3) & 2;
}

/**
 * @brief Read byte at offset 0x3 from pointer in D_800F16A8, mask with 0x4.
 *
 * @return Bit 2 of byte at offset 3.
 */
s32 func_800BD304(void) {
    return *(u8 *)(*(s32 *)D_800F16A8 + 3) & 4;
}

/**
 * @brief Read byte at offset 0x1 from pointer stored in D_800F16A8.
 *
 * @return The byte value.
 */
s32 func_800BD31C(void) {
    return *(u8 *)(*(s32 *)D_800F16A8 + 1);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD334);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD374);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD3A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD434);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD464);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD508);

/**
 * @brief Set bit 15 of D_800EF724 halfword and clear D_800F02F4 halfword.
 */
void func_800BD594(void) {
    *(u16 *)D_800EF724 |= 0x8000;
    *(u16 *)D_800F02F4 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD5B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD5FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD658);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD77C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD7A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD804);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BDE28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BDF94);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BE104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BE89C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BECB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEE78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEEC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEF14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEFD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEFE0);
