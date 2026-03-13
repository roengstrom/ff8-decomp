#include "common.h"

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5800);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E58C8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E592C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5988);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E59E0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E5A60);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6884);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E69B0);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6A8C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6B3C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6BB8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6C84);

/**
 * @brief Render a GF ability entry with conditional highlight.
 *
 * Tests a bit in the D_80077378 ability table at offset 0x4E8, indexed by
 * @p arg4 (stride 152) and shifted by @p arg6. If the bit is set, renders
 * a highlighted entry via func_8002FF34; otherwise skips. Then renders the
 * ability name and icon via func_801F0FEC and func_801F4EA8.
 *
 * @param a0    Entity/context pointer
 * @param a1    Display string or value (updated if highlighted)
 * @param a2    X position
 * @param a3    Y position
 * @param arg4  GF index (stride 152 into D_80077378)
 * @param arg5  Parameter passed to func_80020E4C
 * @param arg6  Bit shift amount for ability flag test
 * @param arg7  Icon type (u16)
 * @param arg8  Extra parameter for func_801F0FEC
 */
void func_801E6D20(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4, volatile unsigned int arg5, s32 arg6, u16 arg7, s32 arg8) {
    int new_var5;
    s32 new_var;
    s32 new_var4;
    extern u8 D_80077378[];
    extern s32 D_80083848;
    unsigned char new_var3;
    s32 base;
    s32 new_var2;
    s32 ret;
    s32 result;

    new_var = arg5;
    ret = func_80020E4C(new_var);
    new_var3 = 2;
    new_var5 = 0x4E8;
    base = (s32)D_80077378;
    new_var2 = arg4;

    if (((*((u16 *)((base + ((2 * new_var2) * 76)) + new_var5))) >> arg6) & 1) {
        new_var4 = D_80083848;
        result = (new_var2 = new_var4);
        a1 = func_8002FF34(a0, a1, 0xC0, a2 - 10, a3, result);
    }

    arg4 = a2;
    do { new_var2 = arg4; } while (0);
    arg6 = a3;
    new_var4 = arg6;
    result = func_801F0FEC(a0, a1, arg4, arg6, ret, arg8);
    func_801F4EA8(a0, ret = result, new_var2 + 0x6C, new_var4 + new_var3, arg7);
}

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E6E3C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E70A8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E733C);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7480);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E77B4);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7988);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7C20);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7CB8);

INCLUDE_ASM("asm/ovl/menugf/nonmatchings/menugf", func_801E7CF4);
