#include "common.h"

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2800);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E28B4);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2920);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2934);

/**
 * @brief Look up an ability name string by index.
 *
 * If @p a0 is within bounds (less than D_801E3D9C), uses it to index
 * into D_801E3D84 to get an ability ID, then calls func_80020AD4
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Ability list index.
 * @return Pointer to ability name string, or NULL if index out of bounds.
 *
 * @note Non-matching: Compiler inverts branch direction (bnez into lookup
 * vs original beqz over lookup).
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2944);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2990);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2A34);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3530);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3580);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3630);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E36AC);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E381C);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3904);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E39E0);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3AE0);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3C28);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3C9C);
