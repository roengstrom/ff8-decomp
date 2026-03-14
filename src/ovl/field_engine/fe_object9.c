#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB2A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB3D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB510);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB5E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB650);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB6C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB768);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB7BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB810);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB888);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB8B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB90C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB958);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB9A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBA3C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBB14);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBB20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBBB4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBC08);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBC64);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBDA8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBDE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBE50);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBE78);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBEA4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBEFC);

/**
 * Reads two parameters from the stack using the current index at 0x184,
 * calls func_8002CA58 with them, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BBFFC(u8 *a0) {
    s8 idx;

    idx = *(s8 *)(a0 + 0x184);
    func_8002CA58(*(s32 *)(a0 + idx * 4 - 4), *(s32 *)(a0 + idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC034);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC12C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC170);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC258);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC2E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC44C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC58C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC6F0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC8CC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCB14);

/**
 * Pops a parameter and calls func_8002CE68, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BCC6C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_8002CE68(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCCAC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCDA0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCE44);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCECC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BD024);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BD1A4);
