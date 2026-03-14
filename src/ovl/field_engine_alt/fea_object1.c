#include "common.h"

extern u8 D_800780D8[];
extern u8 D_800C4DCC[];
extern u8 D_800C4FD3;
extern u8 D_800C4FD4;
extern u8 D_800C4FD5;
extern u8 D_800C4FD6;
extern u8 D_800C4FD7;
extern u8 D_800D23D8[];

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_800997E8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_80099B48);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_80099C84);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_80099EDC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_80099F78);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009A4DC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009A638);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009A7C0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009A954);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009AD3C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009AEE4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009B358);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009B550);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009B748);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009B840);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009B954);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009BFA0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C070);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C1A4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C294);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C478);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C528);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C54C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C5FC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C69C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C6CC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C6F0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C738);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C780);

/** Sets bit 0x20 on two related flag bytes. */
void func_8009C7DC(void) {
    D_800780D8[0x108] |= 0x20;
    D_800D23D8[0x66] |= 0x20;
}

/** Clears bit 0x20 on two related flag bytes. */
void func_8009C808(void) {
    D_800780D8[0x108] &= ~0x20;
    D_800D23D8[0x66] &= ~0x20;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C834);

/** Advances index and returns difference from table lookup. */
u8 func_8009C870(void) {
    u8 idx;
    D_800C4FD6++;
    idx = D_800C4FD6;
    if (idx == 0) {
        D_800C4FD5 += 0xD;
    }
    return (D_800C4DCC[D_800C4FD6] - D_800C4FD5) & 0xFF;
}

/** Sets two related byte values. */
void func_8009C8CC(s32 val) {
    D_800C4FD6 = val;
    D_800C4FD5 = val;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009C8E0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009CA34);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009CAE0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009CB70);

void func_8009CC34(void) {
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009CC3C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object1", func_8009CC98);

/** Sets two related byte values. */
void func_8009CCC8(s32 val) {
    D_800C4FD4 = val;
    D_800C4FD3 = val;
}

/** Sets a single byte value. */
void func_8009CCDC(s32 val) {
    D_800C4FD7 = val;
}
