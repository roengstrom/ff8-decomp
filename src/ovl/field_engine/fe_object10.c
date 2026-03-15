#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD1F4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD250);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD2AC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD318);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD3A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD5E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD64C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD6EC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD794);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD804);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD9C4);

/**
 * Returns the value of the global byte D_800DE4FD.
 *
 * @return The value of D_800DE4FD.
 */
u8 func_800BE264(void) {
    extern u8 D_800DE4FD;
    return D_800DE4FD;
}

/** @brief Call func_801E8B84 if D_800DE4FD bit 1 is set. Returns result or 0. */
s32 func_800BE274(void) {
    extern u8 D_800DE4FD[];
    if (*(u8 *)D_800DE4FD & 2) {
        return func_801E8B84();
    }
    return 0;
}

/** @brief Call func_801E8804 if D_800DE4FD bit 1 is set. */
void func_800BE2AC(void) {
    extern u8 D_800DE4FD[];
    if (*(u8 *)D_800DE4FD & 2) {
        func_801E8804();
    }
}

/** @brief Call func_801E888C if D_800DE4FD bit 1 is set. */
void func_800BE2DC(void) {
    extern u8 D_800DE4FD[];
    if (*(u8 *)D_800DE4FD & 2) {
        func_801E888C();
    }
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE30C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE36C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE44C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE4B0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE5E4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE7F4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BE924);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BEA84);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BEBD0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF080);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF230);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF28C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF3D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF448);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF4A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF5A8);
