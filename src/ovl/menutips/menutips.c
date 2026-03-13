#include "common.h"

/**
 * @brief Render a tips panel entry.
 *
 * Calls func_801F08D4 with mode 1, section 0xD, the provided context,
 * and flag 0.
 *
 * @param a0 Render context pointer.
 */
void func_801E5800(s32 a0) {
    func_801F08D4(1, 0xD, a0, 0);
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E582C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E58C0);

/**
 * @brief Render a tips entry at a table-derived position.
 *
 * Checks D_801EC410; if positive, looks up coordinates from
 * D_801EC310[a1] (8-byte stride), applies offsets (+0x22, +0x23),
 * and calls func_801F0A34 to render.
 *
 * @param a0 Render context pointer.
 * @param a1 Index into the coordinate table.
 *
 * @note Non-matching: Compiler schedules sll (a1*8) before lui/addiu
 * for D_801EC310 table base, changing register allocation (v1 vs v0
 * for result pointer).
 */
INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E590C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E5958);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E597C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E5A10);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6018);

/**
 * @brief Check tutorial availability and save current selection.
 *
 * Calls func_80035E00 to check if the tips system is ready. If not ready
 * (returns 0), copies D_801ED422 to D_801ED420 (saves current selection)
 * and returns 1. Otherwise returns 0.
 *
 * @return 1 if tips system was not ready (selection saved), 0 otherwise.
 */
s32 func_801E6474(void) {
    extern u16 D_801ED422;
    extern u16 D_801ED420;
    if (func_80035E00() == 0) {
        D_801ED420 = D_801ED422;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E64B0);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6514);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6668);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6768);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E67F4);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E688C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E696C);
