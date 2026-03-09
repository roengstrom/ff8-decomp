#include "common.h"

extern u8 D_800F1A90[];
extern u8 D_800F1B80[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C2B88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C2BD0);

/**
 * @brief Read a signed byte from the given address.
 *
 * @param a0 Pointer to a byte.
 * @return The value sign-extended to s32.
 */
s32 func_800C2C10(u8 *a0) {
    return *(s8 *)a0;
}

/**
 * @brief Read an unsigned byte from the given address.
 *
 * @param a0 Pointer to a byte.
 * @return The value zero-extended to s32.
 */
s32 func_800C2C1C(u8 *a0) {
    return *a0;
}

/**
 * @brief Read a little-endian 16-bit signed value from two bytes.
 *
 * Combines ptr[0] (low byte) and ptr[1] (high byte) into a signed 16-bit value.
 *
 * @param ptr Pointer to two bytes in little-endian order.
 * @return The sign-extended 16-bit value.
 */
s16 func_800C2C28(u8 *ptr) {
    return (s16)((ptr[1] << 8) | ptr[0]);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C2C44);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C2C80);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C2CBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3014);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C33A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3418);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3518);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3568);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3694);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3704);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C372C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3898);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C38BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3998);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C39F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3B6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3BBC);

/**
 * @brief Wrapper for func_800C3BBC.
 */
void func_800C3BE0(void) {
    func_800C3BBC();
}

/**
 * @brief Clear D_800F1A90 to zero.
 */
void func_800C3C00(void) {
    *(s32 *)D_800F1A90 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3C0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3DD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3E64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3EFC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3F88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C3FE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C40B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4114);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4228);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C42DC);

/**
 * @brief Return the word value at D_800F1B80.
 *
 * @return Current value of D_800F1B80.
 */
s32 func_800C42FC(void) {
    return *(s32 *)D_800F1B80;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C430C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C434C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C438C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C443C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C44E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4538);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4588);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C45FC);

/**
 * @brief Wrapper for func_800C443C.
 */
void func_800C4764(void) {
    func_800C443C();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4784);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C47BC);

/**
 * @brief Wrapper for func_800C44E4.
 */
void func_800C47CC(void) {
    func_800C44E4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C47EC);

/**
 * @brief Look up a table entry and compute a pointer offset.
 *
 * Loads a table pointer from a0->0x84->0x28, reads a halfword at
 * table[a1*2 + 2], and returns table + offset.
 *
 * @param a0 Pointer to structure with nested table.
 * @param a1 Table index.
 * @return Computed pointer as integer.
 */
s32 func_800C4844(u8 *a0, s32 a1) {
    u8 *base = *(u8 **)(a0 + 0x84);
    u8 *table = *(u8 **)(base + 0x28);
    u16 offset = *(u16 *)(table + a1 * 2 + 2);
    return (s32)table + offset;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4864);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C48A8);

/**
 * @brief Wrapper for func_800C4588.
 */
void func_800C4900(void) {
    func_800C4588();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4920);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4954);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4968);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C49A8);

/**
 * @brief Wrapper for func_800C4538.
 */
void func_800C4A00(void) {
    func_800C4538();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4A20);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object13", func_800C4A58);
