#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800EE424[];
extern u8 D_800EE43C[];
extern u8 D_800EE462[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A18E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1940);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A19BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1AB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1C98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1CFC);

/**
 * @brief Subtract delta from a halfword value, clamping to zero.
 *
 * @param unused0 Unused parameter.
 * @param unused1 Unused parameter.
 * @param delta Amount to subtract.
 * @param ptr Pointer to structure; halfword at offset 0x18 is modified.
 */
void func_800A1D78(s32 unused0, s32 unused1, s32 delta, u8 *ptr) {
    s16 val = *(u16 *)(ptr + 0x18) - delta;
    *(u16 *)(ptr + 0x18) = val;
    if (val <= 0) {
        *(u16 *)(ptr + 0x18) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1DA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1E04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1EC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A1F74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2008);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A20AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2150);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A21B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2310);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2360);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A240C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2480);

/**
 * @brief Call func_8009B924 with constants 0x7A and 0x030E77FF.
 *
 * @param a0 First argument passed through.
 */
void func_800A2520(s32 a0) {
    func_8009B924(a0, 0x7A, 0x030E77FF);
}

/**
 * @brief Call func_8009B924 with constants 0x7E and 0x0180560D.
 *
 * @param a0 First argument passed through.
 */
void func_800A2548(s32 a0) {
    func_8009B924(a0, 0x7E, 0x0180560D);
}

/**
 * @brief Call func_8009B924 with constants 0x37E and 0x038E7FFF.
 *
 * @param a0 First argument passed through.
 */
void func_800A2570(s32 a0) {
    func_8009B924(a0, 0x37E, 0x038E7FFF);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2598);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2638);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A26A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2724);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2CE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2D24);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2E04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2E48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2EB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2EF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2F54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A2FC8);

/**
 * @brief Call func_800DEA58 with D_800EE462 byte value.
 */
void func_800A302C(void) {
    func_800DEA58(*(u8 *)D_800EE462);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A3054);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A3094);

/**
 * @brief Clear two battle state bytes at D_800ED148 offsets 0x5C0 and 0x5C1.
 */
void func_800A30E4(void) {
    u8 *base = D_800ED148;
    base[0x5C1] = 0;
    base[0x5C0] = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A30F8);

/**
 * @brief Initialize battle state: set flag, clear params, and store constants.
 *
 * @param a Byte pointer to clear.
 * @param b Byte pointer to set to 0xA.
 * @param c Halfword pointer to clear.
 * @param d Halfword pointer to set to 9.
 */
void func_800A42B4(u8 *a, u8 *b, u16 *c, u16 *d) {
    *(u8 *)D_800EE43C = 1;
    *a = 0;
    *d = 9;
    *b = 0xA;
    *c = 0;
}

/**
 * @brief Initialize battle state and call func_800A432C.
 *
 * Rearranges parameters: a1-a3+stack become a0-a3 for func_800A42B4,
 * then original a0 is passed to func_800A432C.
 *
 * @param a0 Argument for func_800A432C.
 * @param a1 Byte pointer (becomes a0 for func_800A42B4).
 * @param a2 Byte pointer (becomes a1 for func_800A42B4).
 * @param a3 Halfword pointer (becomes a2 for func_800A42B4).
 * @param stack0 Halfword pointer (becomes a3 for func_800A42B4).
 */
void func_800A42DC(s32 a0, u8 *a1, u8 *a2, u16 *a3, u16 *stack0) {
    func_800A42B4(a1, a2, a3, stack0);
    func_800A432C(a0);
}

/**
 * @brief Store a value to the global D_800EE424.
 *
 * @param value Value to store.
 */
void func_800A4320(s32 value) {
    *(s32 *)D_800EE424 = value;
}

/**
 * @brief Call func_80020F84 and store the result to D_800EE424.
 *
 * @param a0 Argument passed to func_80020F84.
 */
void func_800A432C(s32 a0) {
    *(s32 *)D_800EE424 = func_80020F84(a0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4350);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A43C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4434);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A44FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4618);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A475C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4798);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A47E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4844);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4898);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A493C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4A74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4B68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4B88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4C84);

/**
 * @brief Append a value to the battle command queue and set the current command.
 *
 * Increments the queue index at D_800ED148[0x5C0], stores the value at the
 * computed queue slot (stride 20), and also stores it at D_800ED148[0x1305].
 *
 * @param a0 Command value to store.
 */
void func_800A4DD4(s32 a0) {
    u8 *base = D_800ED148;
    u8 idx = base[0x5C0];
    base[0x5C0] = idx + 1;
    *(u8 *)(base + idx * 20 + 0x5D4) = a0;
    base[0x1305] = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4E08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4EA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4F28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A4FC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5004);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A517C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5210);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A52E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A53C4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5454);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A554C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A559C);

/**
 * @brief Clear the word at offset 0x24 of a battle entity.
 *
 * @param idx Entity index (stride 0xD0).
 */
void func_800A565C(s32 idx) {
    u8 *base = D_800ED148;
    u8 *entity;
    asm("");
    entity = base + idx * 0xD0;
    *(volatile s32 *)(entity + 0x24) = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5688);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5778);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A57E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A584C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A589C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5948);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A59AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5A7C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5AF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5BC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5C48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object3", func_800A5F24);
