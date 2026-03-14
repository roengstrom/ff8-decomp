#include "common.h"

extern u8 D_800786D8;

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD6EC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD754);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD7A4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD7E4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD82C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD918);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD998);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDA08);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDA78);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDAE4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDB5C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDBE0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDC94);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDDC4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDEF4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE040);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE158);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE1A8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE284);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE3D0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE4D8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE578);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE5F8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE63C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE69C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE720);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE7FC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE8B0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE958);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE9F8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEA34);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEA7C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEAF4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEB84);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEC1C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEC60);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BED90);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEDF0);

void func_800BEECC(void) {
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEED4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEF6C);

/** Checks two flag bits and returns status. */
s32 func_800BEFC4(void) {
    u8 val = D_800786D8;
    s32 result = 0;
    if (val & 1) {
        s32 bit = val & 2;
        result = (u32)bit < 1;
    }
    return result;
}

/** Initializes nested array with ramp values. */
void func_800BEFEC(u8 *base) {
    s32 outer = 7;
    s32 inner;
    s32 val;
    s32 *ptr;
    do {
        inner = 4;
        val = -0x1000;
        ptr = (s32 *)(base + 0x10);
        do {
            *(s32 *)((u8 *)ptr + 0x38) = val;
            val += 0x400;
            inner--;
            ptr = (s32 *)((u8 *)ptr - 4);
        } while (inner >= 0);
        outer--;
        base += 0x4C;
    } while (outer >= 0);
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF024);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF20C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF2E8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF3D8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF5A8);
