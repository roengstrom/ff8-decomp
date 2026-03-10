#include "common.h"

extern u8 D_8010334C[];
extern u8 D_80103340[];
extern u8 D_800EEBD0[];

/**
 * @brief Call func_800E0478 with mode 0 and forwarded args.
 */
void func_800E060C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_800E0478(0, a0, a1, a2, a3, a4);
}

/**
 * @brief Call func_800E0478 with mode 1 and forwarded args.
 */
void func_800E0650(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_800E0478(1, a0, a1, a2, a3, a4);
}

/**
 * @brief Call func_800E0478 with mode 2 and forwarded args.
 */
void func_800E0694(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_800E0478(2, a0, a1, a2, a3, a4);
}

/**
 * @brief Call func_800E0478 with mode 3 and forwarded args.
 */
void func_800E06D8(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_800E0478(3, a0, a1, a2, a3, a4);
}

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

/**
 * @brief Process entity through display pipeline.
 *
 * Calls func_800DBCBC for preprocessing, then func_800E173C with
 * display table parameters from D_80103340, then func_800E1640
 * with the accumulated results and D_800EEBD0 config byte.
 *
 * @param a0 Entity parameter.
 */
void func_800E17F4(s32 a0) {
    s32 val1 = func_800DBCBC(a0);
    s16 *tbl = (s16 *)D_80103340;
    s32 val2 = func_800E173C(a0, val1, tbl[1], tbl[0]);
    func_800E1640(a0, a0, val2, *(u8 *)D_800EEBD0);
}

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
