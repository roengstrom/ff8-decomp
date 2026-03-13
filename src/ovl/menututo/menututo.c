#include "common.h"

/**
 * @brief Read tutorial column index 1.
 *
 * @return Byte value of D_801E4EC1.
 */
s32 func_801E2800(void) {
    extern u8 D_801E4EC1;
    return D_801E4EC1;
}

/**
 * @brief Read tutorial column index 2.
 *
 * @return Byte value of D_801E4EC2.
 */
s32 func_801E2810(void) {
    extern u8 D_801E4EC2;
    return D_801E4EC2;
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2820);

/**
 * @brief Load tutorial page indices from table.
 *
 * Reads two bytes from D_801E4EB4[a0] (4-byte stride) and stores
 * them into D_801E4EC1 and D_801E4EC2 respectively.
 *
 * @param a0 Tutorial page table index.
 *
 * @note Non-matching: CC1PSX schedules sll before lui for base address
 * and uses different register (v1 vs a0) for loaded byte.
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E28A8);

/**
 * @brief Read tutorial page index.
 *
 * @return Byte value of D_801E4EC0.
 */
s32 func_801E28D4(void) {
    extern u8 D_801E4EC0;
    return D_801E4EC0;
}

/** @brief Draw inner panel with section id 0xD and clear flag. */
s32 func_801E28E4(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/** @brief Draw inner panel with section id 0xD and set flag. */
s32 func_801E2910(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 1);
}

/** @brief Call func_801F0A34 with a0, 0, 0x22, and a1 * 15 + 0x3E. */
void func_801E293C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x22, a1 * 15 + 0x3E);
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E296C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E29F8);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2ABC);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2D3C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2EF0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E30C4);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3140);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3EC0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E3F8C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4080);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4214);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E431C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E43D4);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4598);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E46DC);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E47F8);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E48C0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4BD0);

/**
 * @brief Conditionally call func_801E4BD0 if field 0x20 is non-zero.
 * @param a0 Pointer to structure with s16 field at offset 0x20
 * @param a1 First argument passed to func_801E4BD0
 * @param a2 Second argument passed to func_801E4BD0
 *
 * @note Non-matching: register allocation — compiler loads field 0x20 into
 * v0 instead of a2. Original saves a2 to v0, loads into a2, tests a2,
 * then restores from v0. Compiled version keeps a2 intact and uses v0
 * for the loaded value.
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4CB0);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4CE4);
