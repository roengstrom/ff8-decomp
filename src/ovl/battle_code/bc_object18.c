#include "common.h"

extern u8 D_80102E40[];
extern u8 D_80102E70[];
extern u8 D_80102E78[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D325C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D32E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D330C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3344);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D33C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D33F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D365C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3708);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3734);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D37D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3A00);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3BB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3C14);

/**
 * @brief Return the address of D_80102E40.
 *
 * @return Address of D_80102E40 as an integer.
 */
s32 func_800D3C50(void) {
    return (s32)D_80102E40;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3C5C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3C98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3CD4);

/**
 * @brief Clear D_80102E70 to zero.
 */
void func_800D3D10(void) {
    *(s32 *)D_80102E70 = 0;
}

/**
 * @brief Decrement D_80102E70 if it is positive.
 */
void func_800D3D1C(void) {
    s32 val = *(s32 *)D_80102E70;
    if (val > 0) {
        *(s32 *)D_80102E70 = val - 1;
    }
}

/**
 * @brief Return the word value at D_80102E70.
 *
 * @return Current value of D_80102E70.
 */
s32 func_800D3D3C(void) {
    return *(s32 *)D_80102E70;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3D4C);

/**
 * @brief Call func_800D3D4C with a1 = 1.
 *
 * @param a0 First argument passed through.
 */
void func_800D3E18(s32 a0) {
    func_800D3D4C(a0, 1);
}

/**
 * @brief Call func_800D3D4C with a1 = 0.
 *
 * @param a0 First argument passed through.
 */
void func_800D3E38(s32 a0) {
    func_800D3D4C(a0, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3E58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3EE8);

void func_800D3F70(void) {
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3F78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4038);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D40EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4110);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D422C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4264);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4294);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4630);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D475C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4804);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D49B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4B04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4BC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4D84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4E44);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4F24);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5064);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5580);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D568C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D581C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5840);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5ADC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5B3C);

/**
 * @brief Return the address of D_80102E78.
 *
 * @return Address of D_80102E78 as an integer.
 */
s32 func_800D5C00(void) {
    return (s32)D_80102E78;
}

/**
 * @brief Compute a pointer into the D_80102E78 array with stride 20.
 *
 * @param a0 Array index.
 * @return Pointer to the entry at a0 * 20 + D_80102E78.
 */
s32 func_800D5C0C(s32 a0) {
    return a0 * 20 + (s32)D_80102E78;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5C28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5C60);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5CE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5D08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5D64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5D84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5DB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5DD8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5E48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5F98);

/**
 * @brief Read bit 0 from scratchpad control register at 0x1F8003AE.
 *
 * @return 1 if bit 0 is set, 0 otherwise.
 */
s32 func_800D6090(void) {
    return *(u16 *)0x1F8003AE & 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D60A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D60F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D6294);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D6310);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D636C);

/**
 * @brief Wrapper for func_800DBCBC.
 */
void func_800D672C(void) {
    func_800DBCBC();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D674C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D67B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D6938);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D69BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D6AC8);

/**
 * @brief Return constant 0xA2 (162).
 *
 * @return Always 0xA2.
 */
s32 func_800D6AEC(void) {
    return 0xA2;
}
