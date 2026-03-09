#include "common.h"

extern u8 D_800EF72C[];
extern u8 D_800F02C8[];
extern u8 D_800F05C8[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B49D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4A74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4BAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4DE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B4E54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B51F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B53F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B54A0);

/**
 * @brief Compute coordinate differences and call func_80041E84.
 *
 * @param a0 Pointer to first coordinate pair (s16 x at +0, s16 y at +4).
 * @param a1 Pointer to second coordinate pair (s16 x at +0, s16 y at +4).
 * @return Result of func_80041E84(dx, dy).
 */
s32 func_800B5528(s32 a0, s32 a1) {
    s32 dx = *(s16 *)a0 - *(s16 *)a1;
    s32 dy = *(s16 *)(a0 + 4) - *(s16 *)(a1 + 4);
    return func_80041E84(dx, dy);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B555C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5604);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B56B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5748);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B59CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5B48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5C10);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5C70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5CB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5EC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B5FD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6100);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6270);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6334);

/**
 * @brief Call func_800B2B68 if D_800EF72C pointer is non-null.
 *
 * Reads the pointer stored in D_800EF72C and calls func_800B2B68
 * with it if non-zero.
 */
void func_800B64E0(void) {
    s32 ptr = *(s32 *)D_800EF72C;
    if (ptr != 0) {
        func_800B2B68(ptr);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B650C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6584);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B66E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6764);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B67D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6858);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6954);

/**
 * @brief Call func_800B3650 with D_800F02C8 as the argument.
 */
void func_800B6A9C(void) {
    func_800B3650(D_800F02C8);
}

/**
 * @brief Process data through D_800F02C8 via stack buffer.
 *
 * @param a0 Input parameter for func_8003FC24.
 */
void func_800B6AC0(s32 a0) {
    u8 buf[0x20];
    func_8003FC24(D_800F02C8, a0, buf);
    func_800B3650(buf);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6AF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6C98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6D88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B6DBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B717C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B724C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B75C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B789C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B79B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7C48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7D20);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7D58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B7FD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B81B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8248);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8314);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B84C8);

/**
 * @brief Allocate from D_800F05C8 and clear byte at offset 0xD.
 *
 * @param a0 Allocation parameter.
 */
void func_800B853C(s32 a0) {
    u8 *result = func_800B2A84(D_800F05C8, a0);
    result[0xD] = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8564);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B8644);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object9", func_800B86C0);
