#include "common.h"

extern u8 D_800786D8;

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD6EC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD754);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD7A4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD7E4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD82C);

/**
 * @brief Copy 10 flag bytes from D_800780D8 to destination struct.
 *
 * Copies bytes at D_800780D8 offsets 0x108-0x10F to dst offsets 0x66-0x6D,
 * and 0x100-0x101 to dst offsets 0x6E-0x6F.
 *
 * @param dst Destination buffer pointer.
 */
void func_800BD918(u8 *dst) {
    extern u8 D_800780D8[];
    u8 *src = D_800780D8;

    dst[0x66] = src[0x108];
    dst[0x67] = src[0x109];
    dst[0x68] = src[0x10A];
    dst[0x69] = src[0x10B];
    dst[0x6A] = src[0x10C];
    dst[0x6B] = src[0x10D];
    dst[0x6C] = src[0x10E];
    dst[0x6D] = src[0x10F];
    dst[0x6E] = src[0x100];
    dst[0x6F] = src[0x101];
}

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
