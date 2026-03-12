#include "common.h"

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5800);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E58C8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E592C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5988);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E59E0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5A60);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6884);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E69B0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6A8C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6B3C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6BB8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6C84);

/**
 * @brief Render a GF ability entry with conditional highlight.
 *
 * Tests a bit in the D_80077378 ability table at offset 0x4E8, indexed by
 * @p arg4 (stride 152) and shifted by @p arg6. If the bit is set, renders
 * a highlighted entry via func_8002FF34; otherwise skips. Then renders the
 * ability name and icon via func_801F0FEC and func_801F4EA8.
 *
 * @param a0    Entity/context pointer
 * @param a1    Display string or value (updated if highlighted)
 * @param a2    X position
 * @param a3    Y position
 * @param arg4  GF index (stride 152 into D_80077378)
 * @param arg5  Parameter passed to func_80020E4C
 * @param arg6  Bit shift amount for ability flag test
 * @param arg7  Icon type (u16)
 * @param arg8  Extra parameter for func_801F0FEC
 */
INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6D20);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6E3C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E70A8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E733C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7480);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E77B4);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7988);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7C20);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7CB8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7CF4);
