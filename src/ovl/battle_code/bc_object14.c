#include "common.h"

extern u8 D_800FA4FC[];
extern u8 D_800FA5F0[];
extern u8 D_800E662C[];
extern u8 D_800F1B90[];
extern u8 D_800FA4F8[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4A64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4ABC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4B78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4D70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4E68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5188);

/**
 * @brief Store a word value to D_800F1B90.
 *
 * @param a0 Value to store.
 */
void func_800C52C4(s32 a0) {
    *(s32 *)D_800F1B90 = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C52D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5304);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5338);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C53F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5490);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C550C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C561C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C56AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5758);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5848);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C58D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C599C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5A34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5A94);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5B1C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5B88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5BC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5CF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5D28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5D58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5D78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5E68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5E74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5F98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6028);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6064);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C60A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6198);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C61E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C624C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C62EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C63D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6480);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C65A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C66E4);

/**
 * @brief Store constant 2 to D_800FA4F8.
 */
void func_800C674C(void) {
    *(u8 *)D_800FA4F8 = 2;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C675C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6764);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6784);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C67A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C68DC);

/**
 * @brief Compute the difference between D_800FA4FC and D_800FA5F0.
 *
 * @return D_800FA4FC - D_800FA5F0.
 */
s32 func_800C6A8C(void) {
    return *(s32 *)D_800FA4FC - *(s32 *)D_800FA5F0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6AA4);

/**
 * @brief Look up a word from the D_800E662C array.
 *
 * @param a0 Array index.
 * @return The word at D_800E662C[a0].
 */
s32 func_800C6B1C(s32 a0) {
    return *(s32 *)(D_800E662C + a0 * 4);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6B38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6C44);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6D3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6DB4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6DEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6EAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6F88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C7134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C71D8);
