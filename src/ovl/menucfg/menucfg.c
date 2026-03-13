#include "common.h"

/** @brief Config menu entry point — delegates to func_801F798C. */
void func_801E5800(s32 a0) {
    func_801F798C(a0);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E5820);

/**
 * @brief Initialize config menu availability flags.
 *
 * Sets both config availability flags at @p a0[0x2D] and @p a0[0x2E]
 * to 1, then validates them. Calls func_80027DB4(0,0,0) to check
 * memory card status; if negative, clears a0[0x2E]. Then calls
 * func_80028098(); if it returns 0, clears a0[0x2D]. If nonzero,
 * calls func_800275A8(0) and clears a0[0x2D] if that returns 0.
 *
 * @param a0 Pointer to config menu context.
 */
void func_801E587C(u8 *a0) {
    s32 val = 1;
    a0[0x2E] = val;
    a0[0x2D] = val;
    if (func_80027DB4(0, 0, 0) < 0) {
        a0[0x2E] = 0;
    }
    if (func_80028098() == 0 || func_800275A8(0) == 0) {
        a0[0x2D] = 0;
    }
}

/**
 * @brief Render a config option label at a grid position.
 *
 * Computes y-coordinate from @p a1 (row * 16 + 0x24) and renders
 * a string of width 0x30 at that position via func_801F0A34.
 *
 * @param a0 Render context pointer.
 * @param a1 Row index (0-based).
 */
void func_801E58EC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x30, a1 * 16 + 0x24);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E5918);

/** @brief Draw inner panel with section id 0x2 and clear flag. */
s32 func_801E59A0(s32 a0) {
    return func_801F08D4(1, 2, a0, 0);
}

/** @brief Draw inner panel with section id 0x2 and set flag. */
s32 func_801E59CC(s32 a0) {
    return func_801F08D4(1, 2, a0, 1);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E59F8);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E61A0);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E625C);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6438);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6538);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E67A8);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6804);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E68E4);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E698C);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6D34);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6E58);
