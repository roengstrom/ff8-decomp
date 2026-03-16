#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B085C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B08CC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0924);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0A08);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0A7C);

/**
 * Clears the byte at offset 0x24B in the object.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0B04(u8 *a0) {
    *(u8 *)(a0 + 0x24B) = 0;
    return 2;
}

/**
 * Sets the byte at offset 0x24B in the object to 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0B10(u8 *a0) {
    *(u8 *)(a0 + 0x24B) = 1;
    return 2;
}

/**
 * Clears the byte at offset 0x249 in the object.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0B20(u8 *a0) {
    *(u8 *)(a0 + 0x249) = 0;
    return 2;
}

/**
 * Sets the byte at offset 0x249 in the object to 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0B2C(u8 *a0) {
    *(u8 *)(a0 + 0x249) = 1;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0B3C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0BE4);

/**
 * Sets the byte at offset 0x24C in the object to 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0C48(u8 *a0) {
    *(u8 *)(a0 + 0x24C) = 1;
    return 2;
}

/**
 * Clears the byte at offset 0x24C in the object.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0C58(u8 *a0) {
    *(u8 *)(a0 + 0x24C) = 0;
    return 2;
}

/**
 * Pops a value, looks up in D_80085230 table, calls func_8009F74C
 * with entity pointer and lookup result, stores return value at 0x140.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0C64(u8 *a0) {
    extern u8 D_80085230[];
    u8 idx;
    s32 val;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    val = *(s32 *)(a0 + (s8)idx * 4);
    *(s32 *)(a0 + 0x140) = func_8009F74C(a0, *(s32 *)(D_80085230 + val * 4));
    return 2;
}

/** @brief Pop halfword from stack and store to offset 0x1F8. Returns 2. */
s32 func_800B0CCC(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u16 *)(a0 + 0x1F8) = *(u16 *)(a0 + (s8)idx * 4);
    return 2;
}

/** @brief Pop halfword from stack and store to offset 0x1F6. Returns 2. */
s32 func_800B0CFC(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u16 *)(a0 + 0x1F6) = *(u16 *)(a0 + (s8)idx * 4);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0D2C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0D94);

/** @brief Pop value from stack, call func_80037C30, store result at 0x140. Returns 2. */
s32 func_800B0E68(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + 0x140) = func_80037C30(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B0EBC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1034);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B10F8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B11BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B12A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B13EC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B14C8);

/**
 * Calls func_801E8B84, returns 1 if result is nonzero, else 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 1 if func_801E8B84 returns nonzero, 2 otherwise.
 */
s32 func_800B158C(u8 *a0) {
    s32 result;
    result = func_801E8B84(a0);
    if (result != 0) {
        return 1;
    }
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B15BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B16B0);

/**
 * Returns 2, indicating continue processing.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B1730(u8 *a0) {
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1738);

/**
 * Returns 2, indicating continue processing.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B17A0(u8 *a0) {
    return 2;
}

/**
 * Calls func_800393C8, returns 1 if result is nonzero, else 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 1 if func_800393C8 returns nonzero, 2 otherwise.
 */
s32 func_800B17A8(u8 *a0) {
    s32 result;
    result = func_800393C8(a0);
    if (result == 0) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B17D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1870);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B18A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B19D4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1A20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1AA0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1B10);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1BB8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1C7C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1D40);

/**
 * Pops a parameter and calls func_8001340C, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B1DF4(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_8001340C(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1E34);

/**
 * Calls func_80012E04 with the object pointer, stores result at offset 0x140, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B1ED4(u8 *a0) {
    *(s32 *)(a0 + 0x140) = func_80012E04(a0);
    return 2;
}

/**
 * Calls func_80012FEC, splits result into high 16 bits (stored at 0x140)
 * and low 16 bits (stored at 0x144), returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B1F04(u8 *a0) {
    s32 result;
    result = func_80012FEC(a0);
    *(s32 *)(a0 + 0x144) = result;
    *(s32 *)(a0 + 0x140) = result >> 16;
    *(s32 *)(a0 + 0x144) = *(u16 *)(a0 + 0x144);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1F48);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B1FE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B2090);

/**
 * Calls func_80012E40 with argument 1, returns 1 if result is nonzero, else 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 1 if func_80012E40 returns nonzero, 2 otherwise.
 */
s32 func_800B2158(u8 *a0) {
    s32 result;
    result = func_80012E40(1);
    if (result == 0) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B2188);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B21E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B2248);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object5", func_800B22C0);
