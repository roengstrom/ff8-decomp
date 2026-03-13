#include "common.h"

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2800);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E28B4);

/**
 * @brief Compute pointer to ability data entry by index.
 * @param a0 Ability entry index (stride 8).
 * @return Address of D_8007CEE0 + a0 * 8.
 */
s32 func_801E2920(s32 a0) {
    extern u8 D_8007CEE0[];
    return D_8007CEE0 + a0 * 8;
}

/**
 * @brief Read ability menu display count.
 * @return Value of D_801E3DB8.
 */
s32 func_801E2934(void) {
    extern u8 D_801E3DB8;
    return D_801E3DB8;
}

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

/**
 * @brief Scan ability bitmask and build list of available ability indices.
 *
 * Calls func_801F72B4 to get an ability bitmask, then iterates bits 0-23.
 * For each set bit, stores (bit_index + 0x5C) into D_801E3D84 array and
 * increments the count in D_801E3D9C.
 */
/**
 * @brief Scan ability bitmask and build list of available ability indices.
 *
 * Calls func_801F72B4 to get an ability bitmask, then iterates bits 0-23.
 * For each set bit, stores (bit_index + 0x5C) into D_801E3D84 array and
 * increments the count in D_801E3D9C.
 *
 * @note Non-matching: instruction scheduling — compiler puts i=0 and one=1
 * before the lui for D_801E3D9C, while the original interleaves them after
 * the lui to fill its load delay slot.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3C28);

/**
 * @brief Initialize ability menu: register callbacks and set up display state.
 *
 * Registers func_801E2A34 and func_801E3AE0 as callbacks via func_801F179C.
 * If registration succeeds, clears state fields (0x20, 0x2C), stores the
 * ability bitmask (0x28) and display size (0x2E = 0x1000), builds the ability
 * list, initializes data, and enters via func_801E2A34.
 */
void func_801E3C9C(void) {
    extern void func_801E2A34();
    extern void func_801E3AE0();
    s32 result = func_801F179C((s32)func_801E2A34, (s32)func_801E3AE0);

    if (result != 0) {
        *(s16 *)(result + 0x2C) = 0;
        *(s32 *)(result + 0x20) = 0;
        *(s32 *)(result + 0x28) = func_801F72B4();
        *(s16 *)(result + 0x2E) = 0x1000;
        func_801E3C28();
        func_801E2990();
        func_801E2A34(result);
    }
}
