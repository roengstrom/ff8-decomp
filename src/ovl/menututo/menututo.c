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
 * @note Non-matching: CC1PSX hoists sll before lui for base address
 * (scheduling difference) and uses v1 instead of a0 for the first
 * loaded byte. Same instruction count (11) but different register
 * allocation and ordering.
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

/**
 * @brief Draw tutorial text with computed CLUT and Y position.
 *
 * Computes a CLUT index from 0x1000 minus field 0x2C via lookup table
 * D_801FA3C8, then calls func_801F0A34 with the computed x/y coordinates.
 *
 * @param a0 OT pointer
 * @param a1 Pointer to tutorial state structure
 *
 * @note Non-matching: register allocation — compiler loads field 0x2C into v1
 * and computes subtraction in v0, while original uses a2 for both the load
 * and subtraction result. Only 2 bytes differ (lh a2 vs lh v1, subu a2 vs subu v0).
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E296C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E29F8);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2ABC);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2D3C);

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E2EF0);

/**
 * @brief Scan tutorial entry table and build list of available entries.
 *
 * Iterates over 9 entries in D_801E4E3C (12-byte stride), calls
 * func_8003720C to check if each entry's item (byte at offset 8) is
 * available. Available entries' indices are stored sequentially at
 * a0+0x39, and the total count is stored at a0+0x36.
 *
 * @param a0 Pointer to tutorial state structure
 */
void func_801E30C4(u8 *a0) {
    extern u8 D_801E4E3C[];
    s32 i = 0;
    s32 count = 0;
    u8 *table = D_801E4E3C;

    do {
        if (func_8003720C(table[8]) != 0) {
            *(u8 *)(a0 + count + 0x39) = i;
            count++;
        }
        i++;
        table += 0xC;
    } while (i < 9);

    *(u8 *)(a0 + 0x36) = count;
}

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
 * @note Non-matching: register allocation — compiler is one instruction shorter.
 * Original saves a2 to v0 before loading field 0x20 into a2 (addu v0,a2,zero;
 * lh a2,0x20(a0); beqz a2; addu a1,v0,zero). Compiled version loads into v0
 * instead (lh v0,0x20(a0); beqz v0; move a1,a2), eliminating the save
 * instruction. 13 vs 12 instructions — size mismatch.
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4CB0);

/**
 * @brief Initialize tutorial menu: register callbacks, clear state, and enter.
 *
 * Registers func_801E48C0 and func_801E4CB0 as callbacks via func_801F179C.
 * If registration succeeds, clears the 16-bit fields at offsets 0x20 and 0x22,
 * calls func_801F010C(0x140) for display setup, then enters via func_801E48C0.
 */
void func_801E4CE4(void) {
    extern void func_801E48C0();
    extern void func_801E4CB0();
    s32 result = func_801F179C((s32)func_801E48C0, (s32)func_801E4CB0);

    if (result != 0) {
        *(s16 *)(result + 0x20) = 0;
        *(s16 *)(result + 0x22) = 0;
        func_801F010C(0x140);
        func_801E48C0(result);
    }
}
