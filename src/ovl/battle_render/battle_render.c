#include "common.h"

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098000);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098688);

/**
 * @brief Initialize render state flags.
 *
 * Sets flag at offset 0x18 to 1 and clears flags at 0x19-0x1B.
 *
 * @param a0 Pointer to render state structure.
 */
void func_80098768(u8 *a0) {
    a0[0x18] = 1;
    a0[0x19] = 0;
    a0[0x1A] = 0;
    a0[0x1B] = 0;
}

void func_80098780(void) {
}

/**
 * @brief Two adjacent void stubs merged by splat.
 *
 * @note Non-matching: dual jr $ra — two void functions merged into one
 * INCLUDE_ASM by splat. Cannot decomp as single C function.
 */
INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098788);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098798);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_800988EC);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098920);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098958);

/**
 * @brief Set battle render mode to active.
 *
 * Sets D_800ABA0C to 1 and D_800D3C70 to -0x18.
 */
void func_80098C68(void) {
    extern u8 D_800ABA0C;
    extern s16 D_800D3C70;
    D_800ABA0C = 1;
    D_800D3C70 = -0x18;
}

/**
 * @brief Clear battle render mode.
 *
 * Clears D_800ABA0C and sets D_800D3C70 to 2.
 */
void func_80098C84(void) {
    extern u8 D_800ABA0C;
    extern s16 D_800D3C70;
    D_800ABA0C = 0;
    D_800D3C70 = 2;
}

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098C9C);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A368);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A40C);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A60C);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A630);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A66C);

/** @brief Call func_8009A66C with D_800A37D4 then D_8009B5B4. */
void func_8009A6CC(void) {
    extern u8 D_800A37D4[];
    extern u8 D_8009B5B4[];
    func_8009A66C(D_800A37D4);
    func_8009A66C(D_8009B5B4);
}

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A6FC);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A734);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009A8B4);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AA18);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AC18);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AE1C);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009AF64);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009B170);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009B394);
