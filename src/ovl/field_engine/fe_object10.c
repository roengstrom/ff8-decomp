#include "common.h"
#include "field.h"

/**
 * @brief Pop value from script stack and branch to one of two handlers.
 *
 * Calls func_800C03BC if popped value is nonzero, func_800C03D8 if zero.
 * @param entity Script entity context.
 * @return 2.
 */
s32 func_800BD1F4(FieldEntity *entity) {
    u8 *a0 = (u8 *)entity;
    u8 idx = entity->stackIdx;
    entity->stackIdx = idx - 1;

    if (*(s32 *)(a0 + (s8)idx * 4) != 0) {
        func_800C03BC();
    } else {
        func_800C03D8();
    }
    return 2;
}

/**
 * @brief Set camera direction parameters based on direction code.
 *
 * Sets halfword at a1+4 to 0xCF and a1+2 to 0. Based on a0:
 * 0 -> a1+0 = 0, 1 -> a1+0 = 0x20, 2 -> a1+0 = -0x20.
 *
 * @param a0 Direction code (0, 1, or 2).
 * @param a1 Output parameter buffer.
 */
void func_800BD250(s32 a0, u8 *a1) {
    *(u16 *)(a1 + 4) = 0xCF;
    *(u16 *)(a1 + 2) = 0;
    switch (a0) {
    case 0:
        *(u16 *)(a1 + 0) = 0;
        break;
    case 1:
        *(u16 *)(a1 + 0) = 0x20;
        break;
    case 2:
        *(s16 *)(a1 + 0) = -0x20;
        break;
    }
}

s32 func_800BD2AC(FieldEntity *entity) { u8 *a0 = (u8 *)entity; u8 idx = entity->stackIdx; s16 buf[4]; entity->stackIdx = idx - 1; func_800BD250(*(s32 *)(a0 + (s8)idx * 4), (u8 *)buf); func_800A9434(*(u8 *)(a0 + 0x256), 0x30, 1, (u8 *)buf, 0x1E); return 2; }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD318);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD3A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD5E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD64C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BD6EC);

void func_800BD794(void) { s32 i = 0; do { s32 status = getPackedField2Bit(i) - 1; if ((u32)status < 2) { if (fieldRandom() & 1) { func_800383B8(i, status); } } i++; } while (i < 0x100); }

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

void func_800BF230(FieldEntity *entity) { extern u8 D_800D9630[]; u8 *a0 = (u8 *)entity; u8 *obj; func_800AA46C(*(u8 *)(a0 + 0x256), 0xD, *(u8 *)(a0 + 0x24E), 0); obj = *(u8 **)(D_800D9630 + *(u8 *)(a0 + 0x256) * 4); *(u16 *)(obj + 0x52) = *(u16 *)(a0 + 0x206); }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF28C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF3D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF448);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF4A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object10", func_800BF5A8);
