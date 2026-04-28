#include "common.h"
#include "field.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB2A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB3D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB510);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB5E0);

s32 func_800BB650(FieldEntity *entity) { s16 buf[4]; func_800A8DAC(*((u8 *)entity + 0x256), 0x20, (u8 *)buf, 0); *((u16 *)((u8 *)entity + 0x228)) = 0; *((u16 *)((u8 *)entity + 0x22C)) = buf[1] / 16; *((u16 *)((u8 *)entity + 0x230)) = buf[2] / 16; return 2; }

/**
 * @brief Copy 12 animation halfwords from D_800704A8 table to entity.
 *
 * Copies halfwords at D_800704A8 offsets 0x108-0x11E to the entity
 * at offsets 0xD8-0xEE (matching pairs: src+0x108->dst+0xD8, etc).
 */
void func_800BB6C8(void) {
    extern u8 D_800704A8[];
    extern u8 D_800562C4[];
    u8 *dst = (u8 *)*(s32 *)D_800562C4;
    u8 *src = D_800704A8;

    *(u16 *)(dst + 0xD8) = *(u16 *)(src + 0x108);
    *(u16 *)(dst + 0xDC) = *(u16 *)(src + 0x10C);
    *(u16 *)(dst + 0xDA) = *(u16 *)(src + 0x10A);
    *(u16 *)(dst + 0xDE) = *(u16 *)(src + 0x10E);
    *(u16 *)(dst + 0xE0) = *(u16 *)(src + 0x110);
    *(u16 *)(dst + 0xE2) = *(u16 *)(src + 0x112);
    *(u16 *)(dst + 0xE4) = *(u16 *)(src + 0x114);
    *(u16 *)(dst + 0xE6) = *(u16 *)(src + 0x116);
    *(u16 *)(dst + 0xE8) = *(u16 *)(src + 0x118);
    *(u16 *)(dst + 0xEA) = *(u16 *)(src + 0x11A);
    *(u16 *)(dst + 0xEC) = *(u16 *)(src + 0x11C);
    *(u16 *)(dst + 0xEE) = *(u16 *)(src + 0x11E);
}

s32 func_800BB768(void) { extern u8 D_800DE8D2; extern u8 D_800704A8[]; u8 *src = D_800704A8; D_800DE8D2 = 2; *(u16 *)(src + 0x108) = 2; *(u16 *)(src + 0x10A) = 0xFF; *(u16 *)(src + 0x10C) = 0x10; *(u16 *)(src + 0x10E) = 0xFF; *(u16 *)(src + 0x110) = 0xFF; *(u16 *)(src + 0x112) = 0xFF; func_800BB6C8(); return 2; }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB7BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB810);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB888);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB8B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB90C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB958);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB9A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBA3C);


INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBB20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBBB4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBC08);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBC64);

s32 func_800BBDA8(void) { extern u8 D_800704A8[]; extern u8 D_800562C4[]; u8 *src = D_800704A8; if (*(u16 *)(src + 0x10C) != *(u16 *)(src + 0x10A)) { return 1; } *(u16 *)((u8 *)*(s32 *)D_800562C4 + 0xD8) = *(u16 *)(src + 0x108); return 2; }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBDE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBE50);

s32 func_800BBE78(void) { extern u8 D_800704A8[]; extern u8 D_800562C4[]; u8 *src = D_800704A8; *(volatile u16 *)(src + 0x108) = 4; *(u16 *)((u8 *)*(s32 *)D_800562C4 + 0xD8) = *(volatile u16 *)(src + 0x108); return 2; }

/**
 * Pops two parameters from the stack and calls func_8002E1B4(val2 & 7, val1).
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BBEA4(u8 *a0) {
    u8 idx;
    s32 val1, val2;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    val1 = *(s32 *)(a0 + (s8)idx * 4);
    *(u8 *)(a0 + 0x184) = idx - 2;
    val2 = *(s32 *)(a0 + (s8)(idx - 1) * 4);
    func_8002E1B4(val2 & 7, val1);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBEFC);

/**
 * Reads two parameters from the stack using the current index at 0x184,
 * calls setSfxPitch with them, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BBFFC(u8 *a0) {
    s8 idx;

    idx = *(s8 *)(a0 + 0x184);
    setSfxPitch(*(s32 *)(a0 + idx * 4 - 4), *(s32 *)(a0 + idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC034);

void func_800BC12C(s32 a0, s32 a1, u8 *a2) { extern u8 D_80085300[]; u8 *base = D_80085300; u8 *entry; a0 <<= 4; entry = base + a0; *(s32 *)(entry + 0x8) = a1; *(u16 *)(entry + 0x0) = *(u16 *)(a2 + 0x0); *(u16 *)(entry + 0x2) = *(u16 *)(a2 + 0x2); *(u16 *)(entry + 0x4) = *(u16 *)(a2 + 0x4); *(u16 *)(entry + 0x6) = *(u16 *)(a2 + 0x6); }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC170);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC258);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC2E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC44C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC58C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC6F0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC8CC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCB14);

/**
 * Pops a parameter and calls setSfxGlobalFlag, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BCC6C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    setSfxGlobalFlag(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCCAC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCDA0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCE44);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCECC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BD024);

/**
 * @brief Pop value, clear D_80085398 table entry, call sound handler.
 *
 * Pops the stack to get an index, clears the halfword at D_80085398[idx * 16],
 * then calls clearAnimEntryActive with the popped value.
 *
 * @param entity Script entity context.
 * @return 2.
 */
s32 func_800BD1A4(FieldEntity *entity) {
    extern u8 D_80085398[];
    u8 *a0 = (u8 *)entity;
    u8 idx = entity->stackIdx;
    s32 val;

    entity->stackIdx = idx - 1;
    val = *(s32 *)(a0 + (s8)idx * 4);
    *(u16 *)(D_80085398 + val * 16) = 0;
    clearAnimEntryActive(val);
    return 2;
}
