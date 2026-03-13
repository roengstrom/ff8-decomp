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

/**
 * @brief Look up a tips data pointer from the entry table.
 *
 * Indexes into D_801EC420 by a0 (4-byte stride), reads the halfword
 * at offset +4, and returns it added to 0x801D1000.
 *
 * @param a0 Tips entry index.
 * @return Pointer to tips data (as s32).
 */
s32 func_801E5958(s32 a0) {
    extern u8 D_801EC420[];
    s32 base = (s32)D_801EC420;
    return *(u16 *)(base + a0 * 4 + 4) + 0x801D1000;
}

/**
 * @brief Initialize tips page data and render entry text.
 *
 * Calls func_801E5958 to get a tips data buffer. Copies the first three
 * halfwords from the buffer header into D_801EC412-D_801EC416, clears
 * D_801EC410, then calls func_8002A2A8 to set up the tips table, measures
 * the text length with func_8002A2F4, and finalizes via func_801E6018.
 * Stores the result in D_801EC410.
 *
 * @note Non-matching: register allocation differs for the store temp
 * variable. The original reuses a2 for both global stores and the
 * func_8002A2A8 third argument, but the compiler allocates a separate
 * register (a3) for the stores and sets a2 with a move instruction.
 */
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

/**
 * @brief Load tips page data if the current selection has changed.
 *
 * Indexes into D_801EC420 to find the entry for the given page index.
 * If the entry's ID differs from D_801ED420, calls func_800360D0 to
 * load the new page data from 0x801D1000, then updates D_801ED422 with
 * the new ID.
 *
 * @param a0 Page index into the tips entry table.
 */
void func_801E64B0(s32 a0) {
    extern u8 D_801EC420[];
    extern s16 D_801ED420;
    extern u16 D_801ED422;
    s32 base = (s32)D_801EC420;
    u16 *entry;
    u16 id;
    a0 = a0 * 4 + 4;
    entry = (u16 *)(a0 + base);
    id = entry[1];
    if (D_801ED420 != id) {
        func_800360D0(id + 0x80, 0x801D1000);
        D_801ED422 = entry[1];
    }
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6514);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6668);

/**
 * @brief Initialize and render tips display panel.
 *
 * Calls func_8002EAD0 to set up a 0x24 x 0xC display region using
 * the D_801E7B10 data table, then configures D_801FAB00 with fixed
 * panel dimensions and calls func_801EF9AC to render.
 *
 * @param a0 First render context parameter.
 * @param a1 Second render context parameter.
 *
 * @note Non-matching: s-reg allocation is swapped (a0->s0, a1->s1 instead
 * of a0->s1, a1->s0). a0 has 2 uses (both jal calls) while a1 has 1 use
 * (second jal only), so the compiler gives a0 the lower s-reg. The original
 * has them reversed. s16 param type flips the allocation but adds unwanted
 * sll/sra sign extension instructions.
 */
INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6768);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E67F4);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E688C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E696C);
