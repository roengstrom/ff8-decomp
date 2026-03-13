#include "common.h"

/**
 * @brief Clear 8 bytes of extension state at D_801E8C10.
 *
 * Iterates backwards from index 7 to 0, setting each byte to zero.
 */
void func_801E5800(void) {
    extern u8 D_801E8C10[];
    s32 i = 7;
    s32 base = (s32)D_801E8C10;
    u8 *ptr = (u8 *)(base + 7);
top:
    *ptr = 0;
    i--;
    ptr--;
    if (i >= 0) goto top;
}

/**
 * @brief Clear D_801E8C20 array (0x6E entries of 2 bytes each) to zero.
 */
void func_801E5828(s32 a0) {
    extern u8 D_801E8C20[];
    s32 i;
    u8 *ptr;

    i = 0;
    ptr = D_801E8C20;

    for (; i < 0x6E; i++) {
        ptr[0] = 0;
        ptr[1] = 0;
        ptr += 2;
    }
}

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

/**
 * @brief Copy extension string data from table to output buffer.
 *
 * Gets the base address from func_801E5A88, adds the offset from *a1,
 * then copies bytes until a sentinel value of 2 is found.
 * Terminates the output by calling func_8002A2A8.
 *
 * @param a0 Unused
 * @param a1 Pointer to u16 offset into the string table
 * @param a2 Output buffer pointer
 *
 * @note Non-matching: compiler puts combined pointer (base+offset) in $v0
 * instead of $a1, and fills load delay slots that original leaves as nop.
 */
INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E5A94);

/**
 * @brief Filter string data from extension table, removing sentinel bytes.
 *
 * Calls func_801E5A88 to get the base address, adds the u16 offset from *a1,
 * skips past leading sentinel bytes (value 2), then copies non-sentinel,
 * non-zero bytes into D_801E8D00. Returns pointer to D_801E8D00.
 *
 * @param a0 Unused
 * @param a1 Pointer to u16 offset into string table
 * @return Pointer to filtered string in D_801E8D00
 *
 * @note Non-matching: addu operand order for combined pointer differs by
 * one byte. Original: addu v1,v0,v1 (base+offset). Compiled: addu v1,v1,v0
 * (offset+base). Same result, different encoding. All other 35 instructions
 * match including scrambled prologue (s0,s2,s1 save order).
 */
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

/**
 * @brief Search a 152-byte entry for a matching id, return its value.
 *
 * Computes the entry address as D_80077818 + a0 * 152, then searches
 * up to 32 byte-pairs for a match with a1. Returns the value byte
 * following the matched id, or 0 if not found.
 *
 * @param a0 Entry index (multiplied by 152 for offset)
 * @param a1 Target id to find
 * @return Value byte after matched id, or 0 if not found
 */
s32 func_801E5F48(s32 a0, s32 a1) {
    extern u8 D_80077818[];
    u8 *ptr = D_80077818 + a0 * 152;
    s32 i = 0;
top:
    {
        s32 id = ptr[0];
        ptr++;
        if (a1 != id) goto skip;
        {
            s32 val = ptr[0];
            if (val != 0) {
                return val;
            }
        }
    }
skip:
    i++;
    ptr++;
    if (i < 0x20) goto top;
    return 0;
}

/**
 * @brief Search byte-pair table for matching id, return its value.
 *
 * Iterates through up to 0xC6 byte-pairs. Each pair is (id, value).
 * If a pair's id matches a0, returns the value byte. Returns 0 if
 * no match is found.
 *
 * @param a0 Target id to find
 * @param a1 Pointer to byte-pair table
 * @return Value byte after matched id, or 0 if not found
 */
s32 func_801E5FA8(s32 a0, u8 *a1) {
    s32 i = 0;
top:
    {
        s32 id = a1[0];
        a1++;
        if (a0 != id) goto skip;
        {
            s32 val = a1[0];
            if (val != 0) {
                return val;
            }
        }
    }
skip:
    i++;
    a1++;
    if (i < 0xC6) goto top;
    return 0;
}

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

/**
 * @brief Decrease compatibility value for a matching entry in table.
 *
 * Searches a byte-pair table (id, value) for an entry matching a1.
 * Subtracts a2 from the value, clamping to 0. If value reaches 0,
 * clears the id byte as well. Stops after the first match.
 *
 * @param a0 Pointer to byte-pair table (id byte followed by value byte)
 * @param a1 Target id to match
 * @param a2 Amount to subtract from value
 */
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

/**
 * @brief Look up entry by absolute index, store type byte, and dispatch.
 *
 * Takes an index (negated if negative), decrements by 1, looks up an 8-byte
 * entry from the table at a0+0x24, stores the type byte (offset 7) to a0+0x4B,
 * then calls func_801E6338 with that type and the a2 parameter.
 *
 * @param a0 Menu context pointer
 * @param a1 Entry index (absolute value used, decremented by 1)
 * @param a2 Parameter passed through to func_801E6338
 * @return 3 if func_801E6338 returns 0, otherwise 0
 *
 * @note Non-matching: compiler fills bgez delay slot with sw ra instead of
 * leaving nop, making the function 4 bytes shorter than the original.
 */
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

/**
 * @brief Render text entry from data table at computed position.
 *
 * Looks up a pointer from the D_801FAB00 table at index a2, decodes it
 * with func_8002F688, then renders at position computed from D_801FAB00
 * base coordinates plus offsets.
 *
 * @param a0 Render context
 * @param a1 Return value if no entry found
 * @param a2 Table index for entry lookup
 * @param a3 Unused
 * @param arg5 X offset added to base position
 * @return Result of func_801F0FEC if entry exists, otherwise a1
 *
 * @note Non-matching: compiler assigns D_801FAB00 lui result to $v1 and
 * full address to $a3, but original has lui in $a3 and full address in $v1.
 * This swaps all subsequent register uses for D_801FAB00 access and changes
 * prologue save interleaving. 13 of 44 instructions differ.
 */
INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7D88);

/**
 * @brief Configure display parameters and invoke callback for extension rendering.
 *
 * Sets up D_801FAB00 display configuration with the given position and size,
 * stores the pointer at a0+0x28 as the data source, reads scroll offset from
 * a0+0x3A, then calls func_801EFBB4 with func_801E7D88 as the render callback.
 *
 * @param a0 Pointer to source data structure
 * @param a1 First callback parameter (passed as a0 to func_801EFBB4)
 * @param a2 Second callback parameter (passed as a1 to func_801EFBB4)
 * @param a3 X position for the display configuration
 * @param arg5 Y position for the display configuration
 */
void func_801E7E38(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    extern u8 D_801FAB00[];
    s32 cfg = (s32)D_801FAB00;
    *(u8 *)(cfg + 0x10) = 0x55;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)&D_801FAB00[0] = a3;
    *(s16 *)(cfg + 0x04) = 0x144;
    *(s16 *)(cfg + 0x06) = 0x1A;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x02) = arg5;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x3A);
    *(s32 *)(cfg + 0x20) = (s32)(a0 + 0x28);
    {
        extern s32 func_801E7D88;
        func_801EFBB4(a1, a2, (s32)&func_801E7D88);
    }
}

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E7EB4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E8058);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E81C8);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E82B4);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E8598);

INCLUDE_ASM("asm/ovl/menuext/nonmatchings/menuext", func_801E8840);
