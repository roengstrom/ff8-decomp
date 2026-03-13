#include "common.h"

/** @brief Config menu entry point — delegates to func_801F798C. */
void func_801E5800(s32 a0) {
    func_801F798C(a0);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E5820);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E587C);

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
