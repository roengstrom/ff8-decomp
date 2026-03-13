#include "common.h"

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5800);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5AF0);

/**
 * @brief Look up a triple magic menu entry address.
 *
 * Indexes into D_801E764C by a0 (halfword stride), loads the offset
 * at +2, and returns base + offset.
 *
 * @param a0 Menu entry index.
 * @return Pointer into D_801E764C data.
 */
s32 func_801E5C0C(s32 a0) {
    extern u8 D_801E764C[];
    u16 *table = (u16 *)D_801E764C;
    return table[a0 + 1] + (s32)D_801E764C;
}

/**
 * @brief Return triple magic data buffer address.
 *
 * @return Constant 0x801CD000.
 */
s32 func_801E5C28(void) {
    return (s32)0x801CD000;
}

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5C34);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5D1C);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5E2C);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5F5C);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E6008);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E606C);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E6170);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E6290);

/**
 * @brief Wrapper that calls func_801E6290 (triple magic menu init/update).
 */
void func_801E6348(void) {
    func_801E6290();
}

/**
 * @brief Wrapper that calls func_801E6290 (triple magic menu init/update).
 */
void func_801E6368(void) {
    func_801E6290();
}

/**
 * @brief Wrapper that calls func_801E6290 (triple magic menu init/update).
 */
void func_801E6388(void) {
    func_801E6290();
}
