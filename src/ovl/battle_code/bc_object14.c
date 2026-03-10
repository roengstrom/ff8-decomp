#include "common.h"

extern u8 D_800FA4FC[];
extern u8 D_800FA5F0[];
extern u8 D_800E662C[];
extern u8 D_800F1B90[];
extern u8 D_800FA4F8[];
extern u8 D_800EF738[];
void func_800C5B1C(void);
s32 func_800B853C(void *);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4A64);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4ABC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4B78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4D70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C4E68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5188);

/**
 * @brief Store a word value to D_800F1B90.
 *
 * @param a0 Value to store.
 */
void func_800C52C4(s32 a0) {
    *(s32 *)D_800F1B90 = a0;
}

/**
 * @brief Set D_800F1B90 to -1 and register callback with mode 0.
 *
 * Stores -1 to D_800F1B90, then calls func_8009B5C4 with a2=0
 * and func_800C52C4 as the callback.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800C52D0(s32 a0, s32 a1) {
    *(s32 *)D_800F1B90 = -1;
    func_8009B5C4(a0, a1, 0, func_800C52C4);
}

/**
 * @brief Set D_800F1B90 to -1 and register callback with mode 1.
 *
 * Stores -1 to D_800F1B90, then calls func_8009B5C4 with a2=1
 * and func_800C52C4 as the callback.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800C5304(s32 a0, s32 a1) {
    *(s32 *)D_800F1B90 = -1;
    func_8009B5C4(a0, a1, 1, func_800C52C4);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5338);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C53F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5490);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C550C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C561C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C56AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5758);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5848);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C58D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C599C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5A34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5A94);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5B1C);

/**
 * @brief Allocate a handler with func_800C5B1C callback and initialize fields.
 *
 * Calls func_800B853C to allocate a handler with func_800C5B1C as callback.
 * Clears byte at offset 0xD, stores a0 at offset 0x10, returns 8.
 *
 * @param a0 Entity pointer to store in handler.
 * @return Always 8.
 */
s32 func_800C5B88(s32 a0) {
    u8 *handler = (u8 *)func_800B853C(func_800C5B1C);
    handler[0xD] = 0;
    *(s32 *)(handler + 0x10) = a0;
    return 8;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5BC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5CF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5D28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5D58);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5D78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5E68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5E74);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C5F98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6028);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6064);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C60A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6198);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C61E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C624C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C62EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C63D4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6480);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C65A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C66E4);

/**
 * @brief Store constant 2 to D_800FA4F8.
 */
void func_800C674C(void) {
    *(u8 *)D_800FA4F8 = 2;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C675C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6764);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6784);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C67A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C68DC);

/**
 * @brief Compute the difference between D_800FA4FC and D_800FA5F0.
 *
 * @return D_800FA4FC - D_800FA5F0.
 */
s32 func_800C6A8C(void) {
    return *(s32 *)D_800FA4FC - *(s32 *)D_800FA5F0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6AA4);

/**
 * @brief Look up a word from the D_800E662C array.
 *
 * @param a0 Array index.
 * @return The word at D_800E662C[a0].
 */
s32 func_800C6B1C(s32 a0) {
    return *(s32 *)(D_800E662C + a0 * 4);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6B38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6C44);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6D3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6DB4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6DEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6EAC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C6F88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C7134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object14", func_800C71D8);
