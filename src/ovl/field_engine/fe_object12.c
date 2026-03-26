#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0098);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C00C8);

/**
 * Sets bit 0x20 in the byte at g_gameState+0xD22.
 */
void func_800C0384(void) {
    extern u8 g_gameState[];

    g_gameState[0xD22] = g_gameState[0xD22] | 0x20;
}

/**
 * Clears bit 0x20 in the byte at g_gameState+0xD22.
 */
void func_800C03A0(void) {
    extern u8 g_gameState[];

    g_gameState[0xD22] = g_gameState[0xD22] & ~0x20;
}

/**
 * Sets bit 0x10 in the byte at g_gameState+0xD22.
 */
void func_800C03BC(void) {
    extern u8 g_gameState[];

    g_gameState[0xD22] = g_gameState[0xD22] | 0x10;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C03DC);

/**
 * Sets bit 0x02 in the byte at g_gameState+0xD22.
 */
void func_800C03F4(void) {
    extern u8 g_gameState[];

    g_gameState[0xD22] = g_gameState[0xD22] | 0x02;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0410);

/**
 * Copies 0x40 bytes from D_80077BA8 - 0x98 to D_80077BA8 using memcopy,
 * then calls memzero16 with D_80077BA8 and mode 4.
 */
void func_800C0448(void) {
    extern u8 D_80077BA8[];

    memcopy(D_80077BA8, D_80077BA8 - 0x98, 0x40);
    memzero16(D_80077BA8, 4);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C048C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C0634);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C06A0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C2150);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C2834);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C292C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object12", func_800C33A0);
