#include "common.h"

extern u8 D_80078720[];
extern u8 D_800ED148[];
extern u8 D_80082C0F[];
extern u8 D_800EE449[];
extern u8 D_800ED70C[];
s32 func_800A980C(void);
s32 func_800A9888(void);
void func_8009AD7C(void);
void func_8009AF14(void *);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB4A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB570);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB668);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB6F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB744);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB844);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB914);

/**
 * @brief Read a little-endian 16-bit signed value from two bytes.
 *
 * Combines ptr[0] (low byte) and ptr[1] (high byte) into a signed 16-bit value.
 *
 * @param ptr Pointer to two bytes in little-endian order.
 * @return The sign-extended 16-bit value.
 */
s16 func_800AB998(u8 *ptr) {
    return (s16)(ptr[0] + (ptr[1] << 8));
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AB9B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ABA3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ABCA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC034);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC094);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC190);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC2E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC348);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC368);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC400);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AC4E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ACED4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ACF2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ACF84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD4A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD50C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD564);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD5D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD6D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD7A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD8E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD960);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AD9C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADAC0);

/**
 * @brief Call one of two processing functions based on entity count.
 *
 * If the input is less than 3, calls func_800A9888; otherwise calls
 * func_800A980C. Returns the lower 16 bits of the result.
 *
 * @param count Entity count.
 * @return Result masked to 16 bits.
 */
u16 func_800ADC10(s32 count) {
    s32 result;
    if (count >= 3) {
        result = func_800A980C();
    } else {
        result = func_800A9888();
    }
    return (u16)result;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADC48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADDAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADEA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800ADF08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE390);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE3D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE414);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE4A0);

/**
 * @brief Compute entity pointer from index and call init functions.
 *
 * Computes D_800ED70C + a0 * 20 to get entity pointer, then calls
 * func_8009B134 with constants 0x68 and 0x80, followed by func_800AD960.
 *
 * @param a0 Entity index (stride 20).
 */
void func_800AE524(s32 a0) {
    u8 *entry = (u8 *)((s32)D_800ED70C + a0 * 20);
    func_8009B134(0x68, 0x80, entry);
    func_800AD960();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE568);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE5D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE64C);

/**
 * @brief Store two display values from func_800AE5D8 and func_800AE64C.
 *
 * Calls func_800AE5D8, stores result to D_80078720[0x570] as u16,
 * then calls func_800AE64C, stores result to D_80078720[0x572].
 */
void func_800AE6C0(void) {
    s32 val = func_800AE5D8();
    u8 *base = D_80078720;
    *(u16 *)(base + 0x570) = val;
    *(u16 *)(base + 0x572) = func_800AE64C();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE6F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE730);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE788);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE7D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE83C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE8A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AE90C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEA0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEACC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEB50);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEC04);

/**
 * @brief Set callback mode 3 and register func_8009AD7C.
 *
 * Stores 1 to D_80082C0F, stores 3 to D_800EE449, then calls
 * func_8009AF14 with func_8009AD7C as the callback.
 */
void func_800AEC98(void) {
    *(u8 *)D_80082C0F = 1;
    *(u8 *)D_800EE449 = 3;
    func_8009AF14(func_8009AD7C);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AECD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AED30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AED9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEE64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEEAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AEF34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AF068);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AF134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object6", func_800AF254);
