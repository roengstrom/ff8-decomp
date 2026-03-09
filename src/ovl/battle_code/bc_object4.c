#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800EE464[];
extern u8 D_80078720[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6184);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A61CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6218);

/**
 * @brief Call func_800D15B4 with display parameters and grid settings.
 *
 * @param a0 First display parameter.
 * @param a1 Brightness or alpha value.
 * @param a2 Size parameter.
 * @param a3 Grid configuration.
 */
void func_800A6288(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_800A62DC(a0, 0x12, 0x80, 0);
}

/**
 * @brief Initialize display with default bright settings.
 *
 * Calls func_800A62DC with a0=0, a1=0x41, a2=0x80, a3=0.
 */
void func_800A62B0(void) {
    func_800A62DC(0, 0x41, 0x80, 0);
}

/**
 * @brief Forward display parameters to func_800D15B4.
 *
 * @param a0 First parameter passed through.
 * @param a1 Second parameter passed through.
 * @param a2 Third parameter passed through.
 * @param a3 Fourth parameter passed through.
 */
void func_800A62DC(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_800D15B4(a0, a1, a2, a3);
}

/**
 * @brief Read a battle state byte from D_800ED148 offset 0x12EB.
 *
 * @return The byte value at D_800ED148[0x12EB].
 */
s32 func_800A62FC(void) {
    volatile u8 *base = D_800ED148;
    return base[0x12EB];
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6310);

/**
 * @brief Store four halfword values to D_800ED148 at offsets 0x1290-0x1296.
 *
 * @param a First halfword (offset 0x1290).
 * @param b Second halfword (offset 0x1292).
 * @param c Third halfword (offset 0x1294).
 * @param d Fourth halfword (offset 0x1296).
 */
void func_800A63C0(s16 a, s16 b, s16 c, s16 d) {
    u8 *base = D_800ED148;
    *(s16 *)(base + 0x1290) = a;
    *(s16 *)(base + 0x1292) = b;
    *(s16 *)(base + 0x1296) = d;
    *(s16 *)(base + 0x1294) = c;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A63DC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A64E4);

/**
 * @brief Store a byte to D_800ED148 offset 0x12EF.
 *
 * @param value Byte value to store.
 */
void func_800A6574(s32 value) {
    volatile u8 *base = D_800ED148;
    base[0x12EF] = value;
}

/**
 * @brief Clear D_800EE464 flag and call func_8009AE08 with mode 8.
 */
void func_800A6588(void) {
    *(u8 *)D_800EE464 = 0;
    func_8009AE08(8);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A65B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A66D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6724);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6780);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A67FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A68AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A69BC);

/**
 * @brief Process entities 1 and 2, then call func_800A67FC with 0.
 */
void func_800A6A58(void) {
    func_800A68AC(1);
    func_800A68AC(2);
    func_800A67FC(0);
}

/**
 * @brief Process battle entities 1 and 2 via func_800A68AC.
 */
void func_800A6A88(void) {
    func_800A68AC(1);
    func_800A68AC(2);
}

/**
 * @brief Process entities 0 and 3, then call func_800A67FC with 1.
 */
void func_800A6AB0(void) {
    func_800A68AC(0);
    func_800A68AC(3);
    func_800A67FC(1);
}

/**
 * @brief Process battle entities 0 and 3 via func_800A68AC.
 */
void func_800A6AE0(void) {
    func_800A68AC(0);
    func_800A68AC(3);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6B08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6B6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6BD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6C34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6D30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6DD8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6E2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6EBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6F64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A6FB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A700C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A703C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7080);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7154);

/**
 * @brief Clear five bytes of a battle entity structure.
 *
 * Zeroes bytes at offsets 0x88, 0x89, 0x8A, 0xC7, and 0xC8 of the
 * given entity pointer.
 *
 * @param entity Pointer to entity data.
 */
void func_800A7188(u8 *entity) {
    entity[0x8A] = 0;
    entity[0x89] = 0;
    entity[0x88] = 0;
    entity[0xC7] = 0;
    entity[0xC8] = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A71A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A71C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7518);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A774C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A779C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A77E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7884);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7934);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A79A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7A44);

/**
 * @brief Look up a byte attribute from D_80078720 entity table (stride 0x1D0).
 *
 * @param idx Entity index.
 * @return Byte at offset 0x1B9 within the entity entry.
 */
s32 func_800A7A8C(s32 idx) {
    u8 *base = D_80078720;
    u8 *entry;
    asm("");
    entry = base + idx * 0x1D0;
    return entry[0x1B9];
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7AB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7AF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7B48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7C64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7CEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7D8C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7EE0);

/**
 * @brief Look up a byte and multiply by 10.
 *
 * @param a0 Base pointer.
 * @param a1 Offset to add to base.
 * @return Byte at (a0 + a1 + 0x160) multiplied by 10.
 */
s32 func_800A7FB4(s32 a0, s32 a1) {
    return *(u8 *)(a0 + a1 + 0x160) * 10;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A7FD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A82A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8320);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8430);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A84CC);

/**
 * @brief Call func_800A84CC for each of 7 battle entities.
 */
void func_800A853C(void) {
    s32 i = 0;
    do {
        func_800A84CC(i);
        i++;
    } while (i < 7);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8578);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A864C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A86F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8794);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A890C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8948);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A89B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8A48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object4", func_800A8AFC);
