#include "common.h"

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5800);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E59A4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5C5C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5D60);

/**
 * @brief Set junction slot count based on character GF compatibility.
 *
 * Reads the character index from @p a0[0x43], looks up the corresponding
 * entry in D_801EEDF0 (stride 28), and checks byte 0xB (GF compatibility
 * flag). If nonzero, sets @p a0[0x47] to 3 (full junction slots); otherwise
 * sets it to 2 (limited slots).
 *
 * @param a0 Pointer to junction menu context.
 */
void func_801E5EE8(u8 *a0) {
    extern u8 D_801EEDF0[];
    s32 base = (s32)D_801EEDF0;
    s32 val = *(u8 *)(base + a0[0x43] * 28 + 0xB);
    if (val != 0) {
        a0[0x47] = 3;
    } else {
        a0[0x47] = 2;
    }
}

/**
 * @brief Save character ability data to buffer.
 *
 * Copies 20 bytes from character @p a0's ability data (D_80077378 at
 * offset 0x4EC, stride 152) into the junction buffer D_801EEFA0.
 *
 * @param a0 Character index (0-7).
 */
void func_801E5F24(s32 a0) {
    extern u8 D_801EEFA0[];
    extern u8 D_80077378[];
    s32 i = 0;
    s32 dst = (s32)D_801EEFA0;
    s32 src = (s32)D_80077378;
    a0 = src + a0 * 152;
    do {
        *(u8 *)(i + dst) = *(u8 *)(a0 + i + 0x4EC);
    } while (++i < 0x14);
}

/**
 * @brief Restore character ability data from buffer.
 *
 * Copies 20 bytes from the junction buffer D_801EEFA0 back into
 * character @p a0's ability data (D_80077378 at offset 0x4EC, stride 152).
 *
 * @param a0 Character index (0-7).
 */
/**
 * @brief Restore character ability data from buffer.
 *
 * Copies 20 bytes from the junction buffer D_801EEFA0 back into
 * character @p a0's ability data (D_80077378 at offset 0x4EC, stride 152).
 *
 * @param a0 Character index (0-7).
 *
 * @note Non-matching: Compiler swaps register allocation for D_80077378
 * and D_801EEFA0 base addresses (a2/v1 vs v1/a2).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5F78);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5FCC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E61F8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6350);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E63FC);

/**
 * @brief Render junction entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x3F for Y)
 */
void func_801E6534(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x5A, (a1 % 4) * 13 + 0x3F);
}

/**
 * @brief Render junction entry at computed grid position.
 *
 * If row index a1 < 3, renders in left column (width 0x2B, Y from row+1).
 * Otherwise renders in right column (width 0x46, Y from row-3).
 *
 * @param a0 X position parameter
 * @param a1 Row index (0-5)
 * @param a2 Width offset to add
 */
void func_801E6584(s32 a0, s32 a1, s32 a2) {
    s32 width;
    s32 y;
    if (a1 < 3) {
        width = 0x2B;
        y = (a1 + 1) * 13 + 0x51;
    } else {
        a1 -= 3;
        width = 0x46;
        y = a1 * 13 + 0x99;
    }
    func_801F0A34(a0, 0, width + a2, y);
}

/**
 * @brief Render a junction list entry at a Y position based on grid slot.
 *
 * Computes Y = (slotIdx % 11) * 13 + 0x42, then calls func_801F0A34
 * to render the entry with width 0xDC.
 *
 * @param a0 Entry data pointer.
 * @param slotIdx Slot index (modulo 11 for row position).
 */
void func_801E65F0(s32 a0, s32 slotIdx) {
    slotIdx %= 11;
    func_801F0A34(a0, 0, 0xDC, slotIdx * 13 + 0x42);
}

/**
 * @brief Look up junction ability availability mask for a given slot.
 *
 * Indexes into D_801EEAC0 to get an ability type (0-18), then checks
 * corresponding bit(s) in the junction flags word at D_801EEDF0[a0 * 28].
 * Cases 0-8 test individual bits via (1 << type), case 9 tests 0x200,
 * case 10 tests 0x400, cases 11-14 test 0x6800, cases 15-18 test 0x19000.
 *
 * @param a0 Character/slot index.
 * @param a1 Ability slot offset into D_801EEAC0.
 * @return Masked flags value, or 0 if type >= 19.
 *
 * @note Non-matching: compiler adds extra andi for u8 masking, uses
 * different register allocation (a2 vs a0 for type), reorders switch
 * case bodies, and generates jump table at different rodata offset.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6658);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E66F0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E676C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E67EC);

/**
 * @brief Compute negative scroll offset from page index.
 *
 * Divides @p a0 by 5 to get the page number, clamps to a maximum of 2,
 * then returns the negative offset as -(page * 160).
 *
 * @param a0 Linear item index.
 * @return Negative pixel offset for scrolling (0, -160, or -320).
 */
s32 func_801E68AC(s32 a0) {
    s32 page = a0 / 5;
    if (page >= 3) {
        page = 2;
    }
    return -(page * 160);
}

/** @brief Draw inner panel with section id 0xB and clear flag. */
s32 func_801E68EC(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

/**
 * @brief Draw inner panel with section id 0xB and set flag.
 * @param a0 Panel position parameter
 * @return Result of func_801F08D4
 */
s32 func_801E6918(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 1);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6944);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E69E0);

/**
 * @brief Full junction menu refresh sequence.
 *
 * Calls func_801E61F8, func_801E6350, func_801E6944, and
 * func_801E69E0 in sequence with the party context.
 *
 * @param a0 Party context pointer.
 */
void func_801E6B88(u8 *a0) {
    func_801E61F8();
    func_801E6350(a0);
    func_801E6944(a0);
    func_801E69E0(a0);
}

/**
 * @brief Copy ability value from junction table to character data and update.
 *
 * Reads a 16-bit value from D_801EEDF0[a0 * 28 + 4] and writes it to
 * D_80077378[a0 * 152 + 0x490], then calls func_801F5190 to update.
 *
 * @param a0 Character/slot index
 */
void func_801E6BC8(s32 a0) {
    extern u8 D_801EEDF0[];
    extern u8 D_80077378[];
    s32 v0 = (s32)D_801EEDF0;
    s32 v1 = a0 * 28 + v0;
    s32 a1 = (s32)D_80077378;

    *(s16 *)(a1 + a0 * 152 + 0x490) = *(u16 *)(v1 + 4);
    func_801F5190();
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6C24);

/**
 * @brief Copy ability value from junction table to character data and refresh.
 *
 * Reads a 16-bit value from D_801EEDF0[a0 * 28 + 6] and writes it to
 * D_80077378[a0 * 152 + 0x4E8], then calls func_801E6B88 to refresh display.
 *
 * @param a0 Character/slot index
 */
void func_801E6CCC(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_801EEDF0[];
    s32 base1 = (s32)D_80077378;
    s32 off1 = a0 * 152;
    s32 base2 = (s32)D_801EEDF0;

    *(s16 *)(off1 + base1 + 0x4E8) = *(u16 *)(base2 + a0 * 28 + 6);
    func_801E6B88(a0);
}

/**
 * @brief Copy ability halfword from character data to junction table.
 *
 * Reads a 16-bit value from D_80077378[a0 * 152 + 0x4E8] and stores it
 * to D_801EEDF0[a0 * 28 + 6].
 *
 * @param a0 Character/slot index.
 */
void func_801E6D28(s32 a0) {
    extern u8 D_801EEDF0[];
    extern u8 D_80077378[];
    s32 base1 = (s32)D_801EEDF0;
    s32 v1 = a0 * 28;
    s32 base2 = (s32)D_80077378;
    *(s16 *)(v1 + base1 + 6) = *(u16 *)(base2 + a0 * 152 + 0x4E8);
}

/**
 * @brief Store a halfword into junction table entry.
 *
 * Indexes into D_801EEDF0 at stride 28 (a0 * 28), and stores
 * a1 as a halfword at offset +4.
 *
 * @param a0 Junction entry index.
 * @param a1 Value to store.
 */
void func_801E6D6C(s32 a0, s32 a1) {
    extern u8 D_801EEDF0[];
    s32 base = (s32)D_801EEDF0;
    *(u16 *)(base + a0 * 28 + 4) = a1;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6D8C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6E0C);

/**
 * @brief Initialize junction entry and refresh display.
 *
 * Calls func_801E6D28, then func_801E6E0C with the context and
 * flag 0, then func_801E5F78 to refresh.
 *
 * @param a0 Junction context pointer.
 */
void func_801E6E88(u8 *a0) {
    func_801E6D28((s32)a0);
    func_801E6E0C(a0, 0);
    func_801E5F78(a0);
}

/**
 * @brief Reset junction slots and copy ability value from character data.
 *
 * Calls func_801E6D8C twice to clear both junction slots, then reads
 * the ability value from D_80077378[a0*152 + 0x490] and stores it
 * to the junction table via func_801E6D6C. Finally refreshes display
 * via func_801E5F24.
 *
 * @param a0 Character/slot index
 */
void func_801E6EC4(s32 a0) {
    extern u8 D_80077378[];
    s32 base;
    u16 val;
    func_801E6D8C(a0, 0);
    func_801E6D8C(a0, 1);
    base = (s32)D_80077378;
    val = *(u16 *)(base + a0 * 152 + 0x490);
    func_801E6D6C(a0, val);
    func_801E5F24(a0);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6F30);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7004);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E70C8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E717C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7228);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E72CC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7370);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7498);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E75C0);

/**
 * @brief Conditionally executes a sequence of calls if func_801E75C0 succeeds
 *
 * Calls func_801E75C0 and if the result is non-zero, calls func_801E6C24,
 * func_801E6B88, then func_801E6C24 again, all with the same parameter.
 *
 * @param a0 Parameter passed to the three conditional calls
 * @return Result from func_801E75C0
 */
s32 func_801E76DC(s32 a0) {
    s32 result = func_801E75C0();

    if (result != 0) {
        func_801E6C24(a0);
        func_801E6B88(a0);
        func_801E6C24(a0);
    }
    return result;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7734);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E79A4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7A80);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7B1C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7BA4);

/**
 * @brief Build junction ability flags from character data.
 *
 * Reads the base ability value (lower 7 bits of halfword at +0x1B4) and
 * tests 6 flag bits in the word at +0x18C to compose a bitmask:
 *   bit 0x01 -> 0x80, bit 0x04 -> 0x100, bit 0x08 -> 0x200,
 *   bit 0x200 -> 0x400, bit 0x4000 -> 0x800, bit 0x8000 -> 0x1000.
 *
 * @param data Pointer to character/junction data block.
 * @return Composed ability flags word.
 */
s32 func_801EA838(u8 *data) {
    s32 result = *(u16 *)(data + 0x1B4) & 0x7F;
    s32 flags = *(s32 *)(data + 0x18C);

    if (flags & 0x1) result |= 0x80;
    if (flags & 0x4) result |= 0x100;
    if (flags & 0x8) result |= 0x200;
    if (flags & 0x200) result |= 0x400;
    if (flags & 0x4000) result |= 0x800;
    if (flags & 0x8000) result |= 0x1000;
    return result;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EA898);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EAB84);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EAE3C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB118);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB3F4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB59C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB740);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB91C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB9CC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBA78);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBBF4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBD90);

/**
 * @brief Check junction ability mask compatibility.
 *
 * If a0 has any bits in common with a2, returns 7 (incompatible).
 * Otherwise returns whether a1 shares any bits with a2.
 *
 * @param a0 Current junction mask.
 * @param a1 Available abilities mask.
 * @param a2 Ability bit to check.
 * @return 7 if already junctioned, 1 if available, 0 if not.
 *
 * @note Non-matching: Compiler inverts branch direction (bnez falls
 * through to and+sltu+j instead of jumping to li v0,7 directly).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBEBC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBED8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC280);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC358);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC434);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC50C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC5E4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC914);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECC4C);

/**
 * @brief Configure display panel and invoke rendering callback.
 *
 * Sets up D_801FAB00 with the given position, fixed size (0x150 x 0x48),
 * clears icon fields, and calls func_801EF9AC with D_80083848 and a
 * caller-supplied parameter.
 *
 * @param a0 First parameter passed through to func_801EF9AC.
 * @param a1 Second parameter passed through to func_801EF9AC.
 * @param a2 X position for the display panel.
 * @param a3 Y position for the display panel.
 * @param a4 Fifth parameter passed as a2 to func_801EF9AC (on stack).
 */
void func_801ECE2C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    extern u8 D_801FAB00[];
    extern u8 D_80083848[];
    s32 cfg = (s32)D_801FAB00;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)(cfg + 0) = a2;
    *(s16 *)(cfg + 4) = 0x150;
    *(s16 *)(cfg + 6) = 0x48;
    *(s16 *)(cfg + 2) = a3;
    func_801EF9AC(a0, a1, a4, *(s32 *)D_80083848);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECE80);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED078);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED150);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED314);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED454);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED68C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED7F4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDAA0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDC88);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDDF8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDF04);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EE494);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EE718);

/**
 * @brief Initialize junction menu: set mode 1, configure display, enable flag.
 *
 * Sets up the junction menu display by calling initialization functions,
 * configuring display areas, setting the active flag D_801EED04 to 1,
 * then entering the main junction menu handler.
 *
 * @param a0 Menu context pointer
 */
void func_801EE82C(s32 a0) {
    extern u8 D_801EED04;

    func_801F1DBC(1);
    func_801E2ABC(a0);
    func_801F1210(0x801D1000, 0x801CD000);
    D_801EED04 = 1;
    func_801EE718(a0);
}

/**
 * @brief Reset junction menu state and reinitialize.
 *
 * Calls func_801F1DBC(1), clears D_801EED04, then calls
 * func_801EE718 with the context.
 *
 * @param a0 Junction context pointer.
 */
void func_801EE888(u8 *a0) {
    extern u8 D_801EED04;
    func_801F1DBC(1);
    D_801EED04 = 0;
    func_801EE718(a0);
}
