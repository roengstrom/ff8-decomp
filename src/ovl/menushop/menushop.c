#include "common.h"

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5800);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E583C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E58A0);

/**
 * @brief Look up a shop item property via double indirection.
 *
 * Loads a byte from D_801EA70C[a0*4], uses it to index into D_801F7F98,
 * and returns that byte.
 *
 * @param a0 Shop item index.
 * @return Property byte value.
 */
s32 func_801E5904(s32 a0) {
    extern u8 D_801EA70C[];
    extern u8 D_801F7F98[];
    u8 idx = *(u8 *)(D_801EA70C + a0 * 4);
    return D_801F7F98[idx];
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5930);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E59D8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5A8C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5BA4);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5C08);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5D28);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5DBC);

void func_801E5E88(void) {
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5E90);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6A68);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6ACC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6C3C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6D54);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6E0C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6EB0);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6F60);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6FD8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E722C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7374);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7508);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7628);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E77EC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E791C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E79D4);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7B9C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7C8C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7CFC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7D30);

/**
 * @brief Compute shop item price from table.
 *
 * Looks up byte at D_801E9BA0[a0*12 + 3], then returns (byte * 5) * 2.
 *
 * @param a0 Shop item index.
 * @return Computed price value.
 *
 * @note Non-matching: Leaf register allocation puts table address in
 * v0 (compiled) instead of v1 (original).
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7E1C);

/**
 * @brief Test if bit a0 is set in D_80077E70.
 *
 * @param a0 Bit index.
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_801E7E4C(s32 a0) {
    extern s32 D_80077E70;
    s32 mask = 1 << a0;
    s32 val = D_80077E70 & mask;
    return val != 0;
}

/** @brief Return whether a1 >= func_801E7E1C(a0) (unsigned). */
s32 func_801E7E68(s32 a0, u32 a1) {
    return a1 >= (u32)func_801E7E1C(a0);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7E98);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8058);

/**
 * @brief Render shop item entry at computed Y position with width 0x24.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x50 for Y position)
 */
void func_801E8134(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x24, a1 * 13 + 0x50);
}

/**
 * @brief Render shop item quantity at computed Y position with width 0xA9.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x4F for Y position)
 */
void func_801E816C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xA9, a1 * 13 + 0x4F);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E81A4);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8978);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8AB0);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8B60);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8BD8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8D84);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8FF8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9020);

/**
 * @brief Initialize all 30 shop item entries.
 *
 * Calls func_801E9020 for indices 0 through 29.
 */
void func_801E90BC(void) {
    s32 i;
    for (i = 0; i < 30; i++) {
        func_801E9020(i);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E90F8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9554);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E95DC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9684);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9900);
