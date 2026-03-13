#include "common.h"

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5800);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5828);

/** @brief Draw inner panel with section id 0x2 and clear flag. */
s32 func_801E5854(s32 a0) {
    return func_801F08D4(1, 2, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5880);

/**
 * @brief Return GF/summon data address if enabled, else return 0.
 * @param a0 Enable flag (non-zero to load D_801E8D80)
 * @return Address of D_801E8D80 after calling func_8002A2A8, or 0
 *
 * @note Non-matching: compiler omits the a0-to-a1 copy that the original
 * uses before the branch (original: addu a1,a0,zero + bnez a1;
 * compiled: beq a0,$0 directly). One instruction shorter.
 */
INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E595C);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E59A0);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5A1C);

/**
 * @brief Return pointer to extended menu data area.
 *
 * @return Address of D_801E9600.
 */
s32 func_801E5A88(void) {
    extern u8 D_801E9600[];
    return (s32)D_801E9600;
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5A94);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5AF8);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5B88);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5C08);

/**
 * @brief Render extension entry at Y position computed from row modulo 11.
 * @param a0 Render context pointer.
 * @param a1 Row index (modulo 11, multiplied by 13, offset by 0x41 for Y).
 *
 * @note Non-matching: compiler pre-allocates a3 for the *13 multiply
 * (sll/addu/sll/addu directly in a3) instead of using v0 as temp with
 * a final addu v1,v1,v0. Register preallocation to argument register.
 */
INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5CF8);

/**
 * @brief Render GF/summon entry at computed Y position with width 0x26.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E5D60(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x26, a1 * 13 + 0x42);
}

/**
 * @brief Render GF/summon entry at computed Y position with width 0xC8.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E5D98(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC8, a1 * 13 + 0x42);
}

/**
 * @brief Render extension entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x43 for Y)
 */
void func_801E5DD0(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x43);
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5E20);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5E88);

/**
 * @brief Dispatches render call based on type byte at offset 0x45
 *
 * Reads a type indicator from the data structure. If type is 0, calls
 * func_801E5D98; if type is 2, calls func_801E5DD0. Both use the byte
 * at offset 0x4A as the row parameter.
 *
 * @param a0 Pointer to data structure
 */
void func_801E5EF0(u8 *a0) {
    switch (a0[0x45]) {
    case 0:
        func_801E5D98(0, a0[0x4A]);
        break;
    case 2:
        func_801E5DD0(0, a0[0x4A]);
        break;
    }
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5F48);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5FA8);

/**
 * @brief Build a filtered list of valid extension entries.
 *
 * Iterates through entries 0 to 0x6D, calling func_80023B14 for each.
 * If the result is positive, stores the 1-based entry index and the
 * result value as consecutive bytes into D_801E8C20. Returns the count
 * of valid entries found.
 *
 * @return Number of valid entries found.
 */
s32 func_801E5FE8(void) {
    extern u8 D_801E8C20[];
    u8 *ptr = D_801E8C20;
    s32 count = 0;
    s32 i = count;

    do {
        s32 result = func_80023B14(i);
        if (result > 0) {
            *ptr = i + 1;
            ptr++;
            *ptr = result;
            ptr++;
            count++;
        }
        i++;
    } while (i < 0x6E);

    return count;
}

/**
 * @brief Compute page count for extension list and store to context.
 *
 * Calls func_801E5828 to set up the extension data, then func_801E5FE8
 * to get the total item count. Computes ceiling division by 11 for page
 * count and stores to offset 0x52. Clamps minimum to 1.
 *
 * @param a0 Extension menu context pointer.
 */
void func_801E6060(u8 *a0) {
    s32 val;
    s32 pages;
    func_801E5828(a0);
    val = func_801E5FE8();
    pages = (val + 10) / 11;
    a0[0x52] = pages;
    if ((u8)pages == 0) {
        a0[0x52] = 1;
    }
}

/**
 * @brief Wrapper that calls func_80023B14.
 */
void func_801E60C4(void) {
    func_80023B14();
}

/** @brief Call func_80021034 then func_800370AC with same argument. */
void func_801E60E4(s32 a0) {
    func_80021034(a0);
    func_800370AC(a0);
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E6114);

/**
 * @brief Advance through extension entries up to a1 times.
 *
 * Calls func_80023A34 with a0 in a loop up to a1 iterations.
 * Stops early if func_80023A34 returns a negative value.
 *
 * @param a0 Extension data pointer
 * @param a1 Maximum number of entries to advance
 */
void func_801E6168(s32 a0, s32 a1) {
    while (a1 > 0) {
        if (func_80023A34(a0) < 0) {
            break;
        }
        a1--;
    }
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E61B4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E6338);

/**
 * @brief Evaluate an extension entry's compatibility level.
 *
 * If the entry index is 0, returns 2 immediately. Otherwise, takes the absolute
 * value of the index, looks up a byte from the table at a1, calls func_801E5FA8,
 * and returns: 1 if result >= 100, 2 if entry index was negative, 0 otherwise.
 *
 * @param a0 Entry index (0 = no entry, negative = reverse lookup)
 * @param a1 Table base pointer
 * @param a2 Parameter passed to func_801E5FA8
 * @return 2 if no entry or negative index with low compat, 1 if high compat, 0 otherwise
 *
 * @note Non-matching: compiler copy-propagates s0 into negu (subu v0,zero,s0
 * instead of subu v0,zero,v0). One byte difference in negu source register.
 */
INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E63F4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E6458);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E64B4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E6748);

/**
 * @brief Return direction indicator based on sign of input.
 *
 * @param a0 Input value
 * @return 0 if a0 == 0, 7 if a0 > 0, 1 if a0 < 0
 */
s32 func_801E76A8(s32 a0) {
    s32 v0;
    if (a0 != 0) goto L1;
    v0 = 0;
L1:
    if (a0 <= 0) goto L2;
    v0 = 7;
L2:
    if (a0 >= 0) goto L3;
    v0 = 1;
L3:
    return v0;
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E76D4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7858);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E79C8);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7B00);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7C9C);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7D88);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7E38);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7EB4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E8058);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E81C8);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E82B4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E8598);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E8840);
