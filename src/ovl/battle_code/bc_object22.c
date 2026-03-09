#include "common.h"

extern u8 D_8010334C[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E060C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0650);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0694);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E06D8);

/**
 * @brief Call func_80020F84 with a0 + 2.
 *
 * @param a0 Base value; 2 is added before passing.
 */
void func_800E071C(s32 a0) {
    func_80020F84(a0 + 2);
}

/**
 * @brief Wrapper for func_80020F84.
 */
void func_800E073C(void) {
    func_80020F84();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E075C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E07E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E07F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0898);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E08D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0A5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0AD8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0BB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0BB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0C50);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0DD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0EFC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0F5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E0FBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1074);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E113C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E116C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E11E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1218);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1244);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1270);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E12B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1480);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1640);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E173C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E17F4);

/**
 * @brief Disable display, set flag, re-enable display.
 *
 * Calls func_800472E4 to disable display, sets D_8010334C to 1,
 * then calls func_800472F4 to re-enable display.
 */
void func_800E1850(void) {
    func_800472E4();
    *(u8 *)D_8010334C = 1;
    func_800472F4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1880);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E19B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1A14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1A94);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E1AFC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E5174);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E5268);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E53E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E5440);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E5474);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E54BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E5524);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object22", func_800E5584);
