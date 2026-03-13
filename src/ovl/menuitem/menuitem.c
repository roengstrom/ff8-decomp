#include "common.h"

/** @brief Store item menu state pointer. */
void func_801E2800(s32 a0) {
    extern s32 D_801ECE20;
    D_801ECE20 = a0;
}

/**
 * @brief Read item menu state pointer.
 *
 * @return Value of D_801ECE20.
 */
s32 func_801E280C(void) {
    extern s32 D_801ECE20;
    return D_801ECE20;
}

/** @brief Draw inner panel with section id 0xB and clear flag. */
s32 func_801E281C(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2848);

/** @brief Store a2 to D_801ECE24 and call func_801E2848. */
void func_801E2BA4(s32 a0, s32 a1, s32 a2) {
    extern s32 D_801ECE24;
    D_801ECE24 = a2;
    func_801E2848();
}

/**
 * @brief Store 4 configuration values to globals and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE30
 * @param a3 Value stored to D_801ECE2C
 * @param arg5 Value stored to D_801ECE38
 * @param arg6 Value stored to D_801ECE28
 */
void func_801E2BC8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    extern s32 D_801ECE30;
    extern s32 D_801ECE2C;
    extern s32 D_801ECE28;
    extern s32 D_801ECE38;

    D_801ECE30 = a2;
    D_801ECE2C = a3;
    D_801ECE28 = arg6;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Store 3 configuration values and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE28
 * @param a3 Value stored to D_801ECE34
 * @param arg5 Value stored to D_801ECE38
 */
void func_801E2C0C(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern s32 D_801ECE28;
    extern s32 D_801ECE34;
    extern s32 D_801ECE38;

    D_801ECE28 = a2;
    D_801ECE34 = a3;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Search byte pair array for a matching entry.
 *
 * Iterates through up to 198 pairs of consecutive bytes starting at @p a0.
 * For each pair, checks if the first byte equals @p a1 and the second byte
 * is greater than or equal to @p a2. Returns 1 on the first match found.
 *
 * @param a0 Pointer to byte pair array.
 * @param a1 Value to match against the first byte of each pair.
 * @param a2 Minimum threshold for the second byte of the matching pair.
 * @return 1 if a matching pair is found, 0 otherwise.
 */
s32 func_801E2C44(u8 *a0, s32 a1, s32 a2) {
    s32 i = 0;
    do {
        s32 b0 = *a0++;
        s32 b1 = *a0++;
        if (b0 == a1 && b1 >= a2) {
            return 1;
        }
    } while (++i < 0xC6);
    return 0;
}

/**
 * @brief Look up an item name string by index.
 *
 * If @p a0 is within bounds (less than D_801ECC10), uses it to index
 * into D_801ECB60 to get an item ID, then calls func_80020AD4
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Item list index.
 * @return Pointer to item name string, or NULL if index out of bounds.
 */
s32 func_801E2C80(s32 a0) {
    extern s32 D_801ECC10;
    extern u8 D_801ECB60[];
    if (a0 < D_801ECC10) {
        return func_80020AD4(D_801ECB60[a0 * 8]);
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2CCC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2D54);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2E04);

/**
 * @brief Read byte 0 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return First byte of the 4-byte entry.
 */
s32 func_801E2E38(s32 a0) {
    extern u8 D_801F889C[];
    return D_801F889C[a0 * 4];
}

typedef struct {
    u8 b0, b1, b2, b3;
} ItemEntry4;

/**
 * @brief Read byte 1 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Second byte of the 4-byte entry.
 */
s32 func_801E2E54(s32 a0) {
    extern ItemEntry4 D_801F889C[];
    return D_801F889C[a0].b1;
}

/**
 * @brief Read byte 2 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Third byte of the 4-byte entry.
 */
s32 func_801E2E70(s32 a0) {
    extern ItemEntry4 D_801F889C[];
    return D_801F889C[a0].b2;
}

/**
 * @brief Read byte 3 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Fourth byte of the 4-byte entry.
 */
s32 func_801E2E8C(s32 a0) {
    extern ItemEntry4 D_801F889C[];
    return D_801F889C[a0].b3;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2EA8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2F88);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E302C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3158);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3288);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3314);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E338C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E347C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E35B8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E37A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3854);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E38DC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3968);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3C1C);

/**
 * @brief Reset four item menu state words to -1.
 *
 * Sets D_801ECEDC, D_801ECEE4, D_801ECEE8, and D_801ECEE0 all to -1.
 *
 * @note Non-matching: compiler schedules li v1,-1 before the first lui
 * for D_801ECEDC. Original has lui first with li filling the delay slot.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3E94);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3EBC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E42F8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4394);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E457C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E45B4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4608);

/**
 * @brief Render item entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x8D for Y)
 */
void func_801E46B8(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC1, (a1 % 4) * 13 + 0x8D);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4708);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E476C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E47E0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4848);

/**
 * @brief Render item at computed Y position based on row index.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E4908(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC8, a1 * 13 + 0x42);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4940);

/**
 * @brief Render item entry at position derived from index parity and half-index
 *
 * Uses the low bit of a1 to select a column (multiplied by 82 + 0xC4 for width),
 * and a1/2 to select a row (multiplied by 13 + 0x40 for Y position).
 *
 * @param a0 X position parameter
 * @param a1 Linear index (bit 0 = column, upper bits / 2 = row)
 */
void func_801E49FC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, (a1 & 1) * 82 + 0xC4, (a1 / 2) * 13 + 0x40);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4A58);

/**
 * @brief Dispatch based on upper/lower 16 bits of flags.
 *
 * If upper 16 bits are zero, calls func_801E4908 (normal render).
 * If upper bits non-zero but lower 16 bits are zero, calls func_801E49FC.
 *
 * @param a0 X position parameter
 * @param a1 Row index parameter
 * @param a2 Combined flags (upper 16 = type, lower 16 = subtype)
 */
void func_801E4B38(s32 a0, s32 a1, s32 a2) {
    if ((a2 & ~0xFFFF) == 0) {
        func_801E4908(a0, a1);
    } else if ((a2 & 0xFFFF) == 0) {
        func_801E49FC(a0, a1);
    }
}

/**
 * @brief Dispatch to different handler based on mode flag.
 * @param a0 Mode flag: non-zero calls func_801E4A58, zero calls func_801E4940
 * @param a1 Parameter passed to the selected handler
 */
void func_801E4B80(s32 a0, s32 a1) {
    if (a0 != 0) {
        func_801E4A58(a1);
    } else {
        func_801E4940(a1);
    }
}

/**
 * @brief Load item entry data for the primary list index.
 *
 * Reads the current list index from @p a0[0x54], uses it to look up
 * a byte pair from the item table at @p a0[0x20]. Stores the first
 * byte (item ID) at @p a0[0x65]. If both bytes are nonzero, calls
 * func_80020D4C to get the item description and stores it at @p a0[0x28].
 *
 * @param a0 Pointer to item menu context.
 */
/**
 * @brief Load item entry data for the primary list index.
 *
 * Reads the current list index from @p a0[0x54], uses it to look up
 * a byte pair from the item table at @p a0[0x20]. Stores the first
 * byte (item ID) at @p a0[0x65]. If both bytes are nonzero, calls
 * func_80020D4C to get the item description and stores it at @p a0[0x28].
 *
 * @param a0 Pointer to item menu context.
 *
 * @note Non-matching: Compiler swaps v0/v1 register allocation for
 * the halfword index (lh) and base pointer (lw) loads.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4BB4);

/**
 * @brief Load item entry data for the secondary list index.
 *
 * Same as func_801E4BB4 but uses the secondary index at @p a0[0x58].
 *
 * @param a0 Pointer to item menu context.
 *
 * @note Non-matching: Same v0/v1 swap as func_801E4BB4.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4C14);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4C74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4D40);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4EA4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7D18);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7E74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8024);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E80D0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8180);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E820C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E82CC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E83B4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E84A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E859C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8684);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8780);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E88AC);

/**
 * @brief Look up sprite data address from table at 0x801D1000.
 *
 * Reads halfword offset at index a0 from the table, adds to base.
 *
 * @param a0 Sprite index.
 * @return Base address + halfword offset from table entry.
 */
s32 func_801E89A4(s32 a0) {
    s32 base = 0x801D1000;
    return *(u16 *)(base + a0 * 2 + 2) + base;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E89C0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8AF0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8C88);

/** @brief Return base address of item sprite data (0x801CD000). */
s32 func_801E8DA4(void) {
    return 0x801CD000;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8DB0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8E98);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8FA8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E90D8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E91E4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9248);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E934C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E95C4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9AAC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9B98);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9C90);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9CD4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9DE4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9E10);

/** @brief Draw inner panel with section id 0x5 and clear flag. */
s32 func_801E9E7C(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9EA8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9F94);

/**
 * @brief Call func_800375A0 with rearranged args and D_80083848 as 6th arg.
 * @param a0 First parameter passed through
 * @param a1 Second parameter passed through
 * @param a2 Becomes 4th argument to callee
 * @param a3 Becomes 5th argument (on stack) to callee
 * @param arg5 Becomes 3rd argument to callee
 */
void func_801EA500(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern s32 D_80083848;

    func_800375A0(a0, a1, arg5, a2, a3, D_80083848);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA538);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA714);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA7E0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EA8F0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAA04);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAB00);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAB8C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAC54);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801EAD64);
