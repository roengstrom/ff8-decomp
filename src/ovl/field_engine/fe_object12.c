#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0098);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C00C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0384);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03A0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03DC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03F4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0410);

/**
 * Copies 0x40 bytes from D_80077BA8 - 0x98 to D_80077BA8 using func_80020644,
 * then calls func_80020608 with D_80077BA8 and mode 4.
 */
void func_800C0448(void) {
    extern u8 D_80077BA8[];

    func_80020644(D_80077BA8, D_80077BA8 - 0x98, 0x40);
    func_80020608(D_80077BA8, 4);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C048C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0634);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C06A0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C2150);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C2834);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C292C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C33A0);
