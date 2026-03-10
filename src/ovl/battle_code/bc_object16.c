#include "common.h"

extern u8 D_800FB42C[];
extern u8 D_800FB428[];
extern u8 D_800FB434[];
extern u8 D_800FB438[];
extern u8 D_800FB43C[];
extern u8 D_800FB444[];
extern u8 D_800FB448[];
extern u8 D_800F1B80[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800C9F50);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800C9F68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CA078);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CA344);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CA668);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CA9F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CADD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CB0DC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CB0F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CB31C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CB464);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CB820);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CBC68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CBEAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CBFA8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CC360);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CC4FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CC6F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CCB54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CCCB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CD1C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CD35C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CD594);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CD98C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CDB94);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CDD30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CDF3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CE158);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CE77C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CE7D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CE82C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CE918);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CEA7C);

void func_800CEBE4(void) {
}

/**
 * @brief Store a byte value to D_800FB42C.
 *
 * @param a0 Value to store (low byte).
 */
void func_800CEBEC(s32 a0) {
    *(u8 *)D_800FB42C = (u8)a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CEBF8);

/**
 * @brief Wrapper for func_800CEBF8.
 */
void func_800CEC38(void) {
    func_800CEBF8();
}

/**
 * @brief Store a word value to D_800FB428.
 *
 * @param a0 Value to store.
 */
void func_800CEC58(s32 a0) {
    *(s32 *)D_800FB428 = a0;
}

/**
 * @brief Set bit 31 of D_800F1B80 and call func_80030DB0.
 *
 * @param a0 Argument passed through to func_80030DB0.
 */
void func_800CEC64(s32 a0) {
    *(s32 *)D_800F1B80 |= 0x80000000;
    func_80030DB0(a0);
}

/**
 * @brief Call func_800B34B0, then clear bit 31 of D_800F1B80.
 */
void func_800CEC94(void) {
    func_800B34B0();
    *(s32 *)D_800F1B80 &= 0x7FFFFFFF;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CECC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CED0C);

/**
 * @brief Return the word value at D_800FB448.
 *
 * @return Current value of D_800FB448.
 */
s32 func_800CED3C(void) {
    return *(s32 *)D_800FB448;
}

/**
 * @brief Return the byte value at D_800FB444.
 *
 * @return Current value of D_800FB444 (unsigned byte).
 */
s32 func_800CED4C(void) {
    return *(u8 *)D_800FB444;
}

/**
 * @brief Increment D_800FB448 counter and set D_800FB444 flag to 1.
 */
void func_800CED5C(void) {
    s32 val = *(s32 *)D_800FB448;
    val += 1;
    *(volatile s32 *)D_800FB448 = val;
    *(volatile u8 *)D_800FB444 = 1;
}

/**
 * @brief Clear D_800FB448 word and D_800FB444 byte.
 */
void func_800CED84(void) {
    *(s32 *)D_800FB448 = 0;
    *(volatile u8 *)D_800FB444 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CED9C);

/**
 * @brief Read bit 1 from scratchpad control register at 0x1F8003AE.
 *
 * @return 1 if bit 1 is set, 0 otherwise.
 */
s32 func_800CEDA4(void) {
    return (*(u16 *)0x1F8003AE >> 1) & 1;
}

/**
 * @brief Read bit 2 from scratchpad control register at 0x1F8003AE.
 *
 * @return 1 if bit 2 is set, 0 otherwise.
 */
s32 func_800CEDBC(void) {
    return (*(u16 *)0x1F8003AE >> 2) & 1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CEDD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CEDF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CEE34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CEFA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF060);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF0B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF164);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF1C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF2D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF300);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF308);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF33C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF38C);

/**
 * @brief Return the word value at D_800FB434.
 *
 * @return Current value of D_800FB434.
 */
s32 func_800CF3E0(void) {
    return *(s32 *)D_800FB434;
}

/**
 * @brief Store a word value to D_800FB438.
 *
 * @param a0 Value to store.
 */
void func_800CF3F0(s32 a0) {
    *(s32 *)D_800FB438 = a0;
}

/**
 * @brief Return the word value at D_800FB438.
 *
 * @return Current value of D_800FB438.
 */
s32 func_800CF3FC(void) {
    return *(s32 *)D_800FB438;
}

/**
 * @brief Store a word value to D_800FB43C.
 *
 * @param a0 Value to store.
 */
void func_800CF40C(s32 a0) {
    *(s32 *)D_800FB43C = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF418);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF4D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF4F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object16", func_800CF630);
