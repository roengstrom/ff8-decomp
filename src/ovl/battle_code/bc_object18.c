#include "common.h"

extern u8 D_80102E40[];
extern u8 D_80102E70[];
extern u8 D_80102E78[];
extern u8 D_80103240[];
extern u8 D_80103308[];
extern u8 D_80077E5C[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D325C);

/**
 * @brief Store battle command parameters to D_80103240.
 *
 * Always stores a0 at offset 7. If a0 is zero, clears the
 * D_80103240 halfword. If a0 is non-zero, stores a1 at offset 6.
 *
 * @param a0 Command type (stored at offset 7).
 * @param a1 Command parameter (stored at offset 6 if a0 != 0).
 */
void func_800D32E8(s32 a0, s32 a1) {
    s32 base = (s32)D_80103240;
    *(u8 *)(base + 7) = a0;
    if (a0 == 0) {
        *(s16 *)D_80103240 = 0;
    } else {
        *(u8 *)(base + 6) = a1;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D330C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D3344);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D33C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D33F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D365C);

/**
 * @brief Clamp a0 and call func_80020F84 with offset.
 *
 * If a0 >= 0x20, subtracts 0x10 to wrap it back. Then calls
 * func_80020F84 with the adjusted value plus 0x39.
 *
 * @param a0 Input index, clamped if >= 0x20.
 */
void func_800D3708(s32 a0) {
    if (a0 >= 0x20) {
        a0 -= 0x10;
    }
    func_80020F84(a0 + 0x39);
}

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

/**
 * @brief Read battle state value or return -1.
 *
 * If the halfword at D_80103308+0x10 equals 0x1000, returns the
 * signed byte at D_80103308+0x14. Otherwise returns -1.
 *
 * @return Battle state byte or -1.
 */
s32 func_800D40EC(void) {
    s32 base = (s32)D_80103308;
    if (*(s16 *)(base + 0x10) == 0x1000) {
        return *(s8 *)(base + 0x14);
    }
    return -1;
}

/**
 * @brief Read battle state value or return -1 (zero check).
 *
 * If the halfword at D_80103308+0x10 is non-zero, returns the
 * signed byte at D_80103308+0x14. Otherwise returns -1.
 *
 * @return Battle state byte or -1.
 */
s32 func_800D4110(void) {
    s32 base = (s32)D_80103308;
    if (*(s16 *)(base + 0x10) != 0) {
        return *(s8 *)(base + 0x14);
    }
    return -1;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D4134);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D422C);

/**
 * @brief Conditionally call func_800D422C based on D_80077E5C flag.
 *
 * If bit 2 of the halfword at D_80077E5C is clear, calls func_800D422C.
 */
void func_800D4264(void) {
    if ((*(u16 *)D_80077E5C & 4) == 0) {
        func_800D422C();
    }
}

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

/**
 * @brief Call func_800D568C with a2 = D_80103308.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 */
void func_800D581C(s32 a0, s32 a1) {
    func_800D568C(a0, a1, D_80103308);
}

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

/**
 * @brief Read a signed byte from an array element at stride 20.
 *
 * Reads the signed byte at offset 0x10 within the a0-th entry
 * (stride 20) of the D_80102E78 array.
 *
 * @param a0 Array index.
 * @return Signed byte value.
 */
s32 func_800D5CE4(s32 a0) {
    u8 *base = D_80102E78;
    base = base + a0 * 20;
    return *(s8 *)(base + 0x10);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object18", func_800D5D08);

/**
 * @brief Store a byte into an array element at stride 20.
 *
 * Writes a1 as a byte to offset 0x10 within the a0-th entry
 * (stride 20) of the D_80102E78 array.
 *
 * @param a0 Array index.
 * @param a1 Byte value to store.
 */
void func_800D5D64(s32 a0, s32 a1) {
    u8 *base = D_80102E78;
    base = base + a0 * 20;
    base[0x10] = a1;
}

/**
 * @brief Set entry fields to -1 and 1 at stride-20 offsets.
 *
 * Stores -1 at offset 0x11 and 1 at offset 0x12 in the a0-th entry
 * (stride 20) of D_80102E78.
 *
 * @param a0 Array index.
 */
void func_800D5D84(s32 a0) {
    u8 *base = D_80102E78;
    base = base + a0 * 20;
    *(s8 *)(base + 0x11) = -1;
    base[0x12] = 1;
}

/**
 * @brief Store a value and set flag at stride-20 offsets.
 *
 * Stores a1 at offset 0x11 and 1 at offset 0x12 in the a0-th entry
 * (stride 20) of D_80102E78.
 *
 * @param a0 Array index.
 * @param a1 Value to store at offset 0x11.
 */
void func_800D5DB0(s32 a0, s32 a1) {
    s32 base = (s32)D_80102E78;
    base = base + a0 * 20;
    *(u8 *)(base + 0x11) = a1;
    *(u8 *)(base + 0x12) = 1;
}

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
