#include "common.h"

extern u8 D_800780D8[];
extern u8 D_800D23D8[];

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009CCE8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009CDC4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009CDFC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009CE40);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009CE70);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009CFB4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D0F0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D16C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D1B8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D214);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D2D8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D3F4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D44C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D510);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D630);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D688);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D760);

/** @brief Compare input against two entity IDs, return 0x29, 0x18, or -1. */
s32 func_8009D7D8(s32 a0) {
    extern s32 D_800C9E68;
    extern s32 D_800C9E70;
    s32 result = -1;

    if (D_800C9E68 == a0) {
        result = 0x29;
    } else if (D_800C9E70 == a0) {
        result = 0x18;
    }
    return result;
}

/** Clears bit 0x40 on two related flag bytes. */
void func_8009D814(void) {
    D_800780D8[0x108] &= ~0x40;
    D_800D23D8[0x66] &= ~0x40;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D840);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D864);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D8A8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D8F0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D954);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009D9C8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009DA10);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009DA54);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009DAA8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009DB88);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009E5C8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009F594);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009F6EC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009FDA4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009FE80);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009FEBC);

/** @brief Clear a 15-byte animation structure and set type byte at offset 0xA. */
void func_8009FEDC(u8 *a0, u8 a1) {
    a0[0xE] = 0;
    a0[0xD] = 0;
    a0[0xC] = 0;
    a0[0xB] = 0;
    a0[0xA] = 0;
    a0[0x9] = 0;
    a0[0x8] = 0;
    *(u16 *)(a0 + 0x0) = 0;
    *(u16 *)(a0 + 0x2) = 0;
    *(u16 *)(a0 + 0x4) = 0;
    a0[0xA] = a1;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009FF0C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_8009FF70);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_800A0000);

/** Returns a value based on input comparison. */
s32 func_800A009C(s32 val) {
    if (val == 0x32) {
        return 0x1800;
    }
    return 0x1460;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_800A00B4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object2", func_800A017C);
