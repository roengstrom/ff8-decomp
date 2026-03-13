#include "common.h"

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5800);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E59A4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5C5C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5D60);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5EE8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5F24);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5F78);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5FCC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E61F8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6350);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E63FC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6534);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6584);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E65F0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6658);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E66F0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E676C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E67EC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E68AC);

/** @brief Draw inner panel with section id 0xB and clear flag. */
s32 func_801E68EC(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

/**
 * @brief Draw inner panel with section id 0xB and set flag.
 * @param a0 Panel position parameter
 * @return Result of func_801F08D4
 */
s32 func_801E6918(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 1);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6944);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E69E0);

/**
 * @brief Full junction menu refresh sequence.
 *
 * Calls func_801E61F8, func_801E6350, func_801E6944, and
 * func_801E69E0 in sequence with the party context.
 *
 * @param a0 Party context pointer.
 */
void func_801E6B88(u8 *a0) {
    func_801E61F8();
    func_801E6350(a0);
    func_801E6944(a0);
    func_801E69E0(a0);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6BC8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6C24);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6CCC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6D28);

/**
 * @brief Store a halfword into junction table entry.
 *
 * Indexes into D_801EEDF0 at stride 28 (a0 * 28), and stores
 * a1 as a halfword at offset +4.
 *
 * @param a0 Junction entry index.
 * @param a1 Value to store.
 */
void func_801E6D6C(s32 a0, s32 a1) {
    extern u8 D_801EEDF0[];
    s32 base = (s32)D_801EEDF0;
    *(u16 *)(base + a0 * 28 + 4) = a1;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6D8C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6E0C);

/**
 * @brief Initialize junction entry and refresh display.
 *
 * Calls func_801E6D28, then func_801E6E0C with the context and
 * flag 0, then func_801E5F78 to refresh.
 *
 * @param a0 Junction context pointer.
 */
void func_801E6E88(u8 *a0) {
    func_801E6D28();
    func_801E6E0C(a0, 0);
    func_801E5F78(a0);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6EC4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6F30);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7004);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E70C8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E717C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7228);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E72CC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7370);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7498);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E75C0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E76DC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7734);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E79A4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7A80);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7B1C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7BA4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EA838);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EA898);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EAB84);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EAE3C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB118);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB3F4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB59C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB740);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB91C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB9CC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBA78);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBBF4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBD90);

/**
 * @brief Check junction ability mask compatibility.
 *
 * If a0 has any bits in common with a2, returns 7 (incompatible).
 * Otherwise returns whether a1 shares any bits with a2.
 *
 * @param a0 Current junction mask.
 * @param a1 Available abilities mask.
 * @param a2 Ability bit to check.
 * @return 7 if already junctioned, 1 if available, 0 if not.
 *
 * @note Non-matching: Compiler inverts branch direction (bnez falls
 * through to and+sltu+j instead of jumping to li v0,7 directly).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBEBC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBED8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC280);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC358);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC434);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC50C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC5E4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC914);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECC4C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECE2C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECE80);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED078);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED150);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED314);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED454);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED68C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED7F4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDAA0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDC88);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDDF8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDF04);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EE494);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EE718);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EE82C);

/**
 * @brief Reset junction menu state and reinitialize.
 *
 * Calls func_801F1DBC(1), clears D_801EED04, then calls
 * func_801EE718 with the context.
 *
 * @param a0 Junction context pointer.
 */
void func_801EE888(u8 *a0) {
    extern u8 D_801EED04;
    func_801F1DBC(1);
    D_801EED04 = 0;
    func_801EE718(a0);
}
