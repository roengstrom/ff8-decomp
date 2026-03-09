#include "common.h"

extern u8 D_800ED148[];
extern u8 D_800EEED8[];
void func_800B304C();
extern u8 D_8007DADB[];
extern u8 D_800EE42C[];
extern u8 D_800EEEC4[];
extern u8 D_800E3D0C[];
extern u8 D_800EEEC8[];
extern u8 D_800EEECC[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1624);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B16C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B17B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1828);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B18A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1930);

/**
 * @brief Get entity from func_800AE6F8 and call func_800A59AC with mode 7.
 */
void func_800B1A48(void) {
    s32 val = func_800AE6F8();
    func_800A59AC(val, 7, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1A78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1ACC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1B1C);

/**
 * @brief Process battle entity: compute type, store to D_800ED148[0x1314],
 *        trigger entity action, then set flag at D_800ED148[0x131D].
 */
void func_800B1B68(void) {
    s32 val = func_800B1B1C();
    u8 *base = D_800ED148;
    base[0x1314] = val + 7;
    func_800B1A48();
    base[0x131D] = 1;
}

/**
 * @brief Store battle type and value, then trigger entity action mode 8.
 *
 * @param a0 Type byte to store at D_800ED148[0x1314].
 * @param a1 Value to store at D_800ED148[0x12E6] as halfword.
 */
void func_800B1BA8(s32 a0, s32 a1) {
    u8 *base = D_800ED148;
    base[0x1314] = a0;
    *(u16 *)(base + 0x12E6) = a1;
    func_800A59AC(func_800AE6F8(), 8, 0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1BE4);

/**
 * @brief Call func_800B0754 with rearranged parameters.
 *
 * @param a0 First parameter (passed through as a0).
 * @param a1 Becomes a2 in the callee.
 * @param a2 Masked to 16 bits, becomes a3 in the callee.
 */
void func_800B1C3C(s32 a0, s32 a1, s32 a2) {
    func_800B0754(a0, 0xF0, a1, (u16)a2);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1C68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1D4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B1DFC);

/**
 * @brief Copy a byte from D_8007DADB to D_800EE42C as a halfword.
 */
void func_800B2024(void) {
    *(u16 *)D_800EE42C = *(u8 *)D_8007DADB;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2038);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2084);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B20D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2128);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B21B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B21EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2224);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B228C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2338);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2388);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B243C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B24C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B25E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B26B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B27AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2848);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B28C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2A00);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2A38);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2A84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2B00);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2B68);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2C14);

extern u8 D_800EEDC8[];

/**
 * @brief Call func_800B2A84 with D_800EEDC8 and the given parameter.
 *
 * @param a0 Second argument to func_800B2A84.
 * @return Result from func_800B2A84.
 */
s32 func_800B2C58(s32 a0) {
    return func_800B2A84(D_800EEDC8, a0);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2C80);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2D0C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2E04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2EDC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2F3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B2FF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B304C);

/**
 * @brief Allocate a handler for func_800B304C, initialize fields, and clear target byte.
 *
 * @param a0 Pointer whose first byte will be cleared; stored at result[0x14].
 */
void func_800B3128(u8 *a0) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800B304C);
    result[0xC] = 0;
    *(s32 *)(result + 0x14) = (s32)a0;
    *a0 = 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3164);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3270);

/**
 * @brief Set D_800EEEC4 to 1, D_800E3D0C to 3, and store a0/a1 to D_800EEEC8/CC.
 *
 * @param a0 Value to store to D_800EEEC8.
 * @param a1 Value to store to D_800EEECC.
 */
void func_800B32E0(s32 a0, s32 a1) {
    *(u8 *)D_800EEEC4 = 1;
    *(u8 *)D_800E3D0C = 3;
    *(s32 *)D_800EEEC8 = a0;
    *(s32 *)D_800EEECC = a1;
}

/**
 * @brief Set D_800EEEC4 to 1, D_800E3D0C to 4, and store a0 to D_800EEEC8.
 *
 * @param a0 Value to store to D_800EEEC8.
 */
void func_800B330C(s32 a0) {
    *(u8 *)D_800EEEC4 = 1;
    *(u8 *)D_800E3D0C = 4;
    *(s32 *)D_800EEEC8 = a0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3330);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3470);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B34B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3534);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3574);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3650);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3698);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B36B8);

/**
 * @brief Set D_800EEED8 to the scratchpad base address 0x1F800000.
 */
void func_800B36D8(void) {
    *(s32 *)D_800EEED8 = 0x1F800000;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B36E8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3738);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B37B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B37E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3960);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3AE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3B54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3C2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B3E54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B4248);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B42B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B44D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object8", func_800B4920);
