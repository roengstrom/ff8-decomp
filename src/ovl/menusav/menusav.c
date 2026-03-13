#include "common.h"

/**
 * @brief Compute save slot data address.
 *
 * Returns a pointer into the D_801D3000 region at offset a0 * 128.
 *
 * @param a0 Save slot index.
 * @return Pointer to save slot data.
 */
s32 func_801E2800(s32 a0) {
    return (s32)0x801D3000 + (a0 << 7);
}

/**
 * @brief Set D_801EC294 flag to 1.
 */
void func_801E2814(void) {
    extern u8 D_801EC294;
    D_801EC294 = 1;
}

/**
 * @brief Return save data buffer size constant.
 *
 * @return Always 0x180.
 */
s32 func_801E2824(void) {
    return 0x180;
}

/**
 * @brief Read save system state pointer.
 *
 * @return Value of D_8005620C.
 */
s32 func_801E282C(void) {
    extern s32 D_8005620C;
    return D_8005620C;
}

/**
 * @brief Clear 0x2000 bytes of sprite buffer at 0x801D1000.
 */
void func_801E283C(void) {
    u8 *ptr = (u8 *)0x801D1000;
    s32 count = 0x2000;
    do {
        *ptr = 0;
        count--;
        ptr++;
    } while (count > 0);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2860);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E28C8);

/**
 * @brief Check if save slot halfword equals 0x8FF.
 *
 * Calls func_801E2800 to get save slot address, loads halfword
 * at offset +2, and returns whether it equals 0x8FF.
 *
 * @param a0 Save slot index.
 * @return 1 if halfword equals 0x8FF, 0 otherwise.
 */
s32 func_801E292C(s32 a0) {
    s32 result = func_801E2800(a0);
    return *(u16 *)(result + 2) == 0x8FF;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2958);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2AB0);

/** @brief Draw inner panel with section id 0x5 and clear flag. */
s32 func_801E2C90(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

/** @brief Draw inner panel with section id 0x5 and set flag. */
s32 func_801E2CBC(s32 a0) {
    return func_801F08D4(1, 5, a0, 1);
}

/**
 * @brief Render save slot label at computed Y position.
 * @param a0 Save data pointer (halfwords at offsets 0x32 and 0x36)
 * @param a1 Width selector (non-zero = 0x98, zero = 0x64)
 * @param a2 Offset selector (non-zero = use offset 0x36, zero = use 0x32)
 */
void func_801E2CE8(s32 a0, s32 a1, s32 a2) {
    s32 width = 0x64;
    s32 ypos;

    if (a1 != 0) {
        width = 0x98;
    }
    if (a2 != 0) {
        ypos = *(u16 *)(a0 + 0x36) + 7;
    } else {
        ypos = *(u16 *)(a0 + 0x32) + 7;
    }
    func_801F0994(0, ypos, width);
}

/**
 * @brief Render save slot label at computed Y position.
 * @param a0 First parameter passed through to func_801F0954
 * @param a1 Second parameter passed through to func_801F0954
 * @param a2 Row index (multiplied by 24 and offset by 0xA3 for Y position)
 */
void func_801E2D38(s32 a0, s32 a1, s32 a2) {
    func_801F0954(1, a0, a1, a2 * 24 + 0xA3);
}

/**
 * @brief Render a save slot indicator icon at a grid position.
 *
 * Divides @p a0 by 3 to get the remainder, computes a y-coordinate
 * as remainder * 53 + 0x3C, and renders via func_801F0994 at
 * position (0, 0x22, y).
 *
 * @param a0 Linear save slot index.
 */
/**
 * @brief Render a save slot indicator icon at a grid position.
 *
 * Divides @p a0 by 3 to get the remainder, computes a y-coordinate
 * as remainder * 53 + 0x3C, and renders via func_801F0994 at
 * position (0, 0x22, y).
 *
 * @param a0 Linear save slot index.
 *
 * @note Non-matching: Compiler assigns sign bit to v0 instead of v1
 * in the division-by-3 sequence, cascading through the remainder.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2D78);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2DDC);

/**
 * @brief Check save slot validity flags and store result.
 *
 * Sets a0[0x4B] to 1 if either a0[0x2E] or a0[0x2F] is less than 2,
 * otherwise stores (a0[0x2E] < 2).
 *
 * @param a0 Save context pointer.
 * @return The stored flag value.
 *
 * @note Non-matching: CC1PSX schedules the two lbu loads together
 * and puts sltiu in the beqz delay slot, eliminating the nop between
 * loads that the original has.
 */
/**
 * @brief Check if either save slot flag indicates availability.
 *
 * If byte at a0+0x2F >= 2, result is 1. Otherwise result is
 * whether byte at a0+0x2E < 2. Stores and returns the result.
 *
 * @param a0 Save context pointer.
 * @return 1 if available, 0 otherwise.
 *
 * @note Non-matching: compiler fills first lbu delay with second lbu,
 * reorders sltiu comparisons, and puts sltiu v1 in branch delay slot.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2E9C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2ECC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2F6C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E3098);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5464);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5510);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E55C4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5688);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E579C);

/**
 * @brief Render save menu entry with string ID 0x26 (save slot label).
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E58C0(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x26);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x0F.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E591C(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x0F);
    func_801F4274(a1, a2, v0, 0xC0, 0x87, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x12.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5978(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x12);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x18.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E59D4(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x18);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x27.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5A30(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x27);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x13.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5A8C(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x13);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5AE8);

/**
 * @brief Render save menu entry with string ID 0x17.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5B70(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x17);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5BCC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5C38);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5D04);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5DC4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5E30);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5EE0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6060);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E613C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E61C0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6204);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E653C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6754);

/**
 * @brief Render save menu entry with string ID 0x0D.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E6804(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x0D);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x0E.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E6860(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x0E);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x16.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E68BC(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x16);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6918);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6A80);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6B1C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6BE0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E700C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7190);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7268);

/** @brief Call func_80020608 with D_80077378 and 0x13A. */
void func_801E73C0(void) {
    extern u8 D_80077378[];
    func_80020608(D_80077378, 0x13A);
}

/**
 * @brief Wrapper that calls func_801E7428 (save menu handler).
 */
void func_801E73E8(void) {
    func_801E7428();
}

/**
 * @brief Wrapper that calls func_80023888 (save menu input handler).
 */
void func_801E7408(void) {
    func_80023888();
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7428);

/**
 * @brief Read save menu checksum value.
 * @return Value of D_801EC2E4.
 */
s32 func_801E74BC(void) {
    extern s32 D_801EC2E4;
    return D_801EC2E4;
}

/** @brief Draw inner panel with section id 0xE and clear flag. */
s32 func_801E74CC(s32 a0) {
    return func_801F08D4(1, 0xE, a0, 0);
}

/** @brief Draw inner panel with section id 0xE and set flag. */
s32 func_801E74F8(s32 a0) {
    return func_801F08D4(1, 0xE, a0, 1);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7524);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7550);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7598);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E760C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E76C4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7830);

/**
 * @brief Initialize save data structure with checksum and set active flag.
 *
 * Calls func_800372D0 to get the save data pointer, then func_801E74BC to
 * compute a checksum/value which is stored at offset 0x14. Sets bit 0x2
 * in the flags byte at offset 0x0.
 */
void func_801E78FC(void) {
    u8 *s0 = (u8 *)func_800372D0();
    s32 v0 = func_801E74BC();
    *(s32 *)(s0 + 0x14) = v0;
    s0[0] |= 2;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7938);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E79CC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7A84);

/** @brief Call func_800360D0 with a0 + 0xB4 and 0x801D3000. */
void func_801E7B18(u8 *a0) {
    func_800360D0(a0 + 0xB4, 0x801D3000);
}

/**
 * @brief Compute pointer to save slot data entry by index.
 * @param a0 Save slot index (stride 68).
 * @return Address of D_801EBD5C + a0 * 68.
 *
 * @note Non-matching: compiler schedules sll/addu/sll multiply chain
 * before lui/addiu base address load. Original has lui/addiu first.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7B40);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7B5C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7BE8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9804);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E98C8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9988);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9A30);

/**
 * @brief Look up sprite data address from table at 0x801CE800.
 *
 * Reads halfword offset at index a0 from the table, adds to base.
 *
 * @param a0 Sprite index.
 * @return Base address + halfword offset from table entry.
 */
s32 func_801E9AB4(s32 a0) {
    s32 base = 0x801CE800;
    return *(u16 *)(base + a0 * 2 + 2) + base;
}

/** @brief Return base address of save sprite data (0x801CD000). */
s32 func_801E9AD0(void) {
    return 0x801CD000;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9ADC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9BC4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9CD4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9D84);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9DE8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9F10);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA034);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA180);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA278);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA37C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA444);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA4E0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA640);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA904);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA9AC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAD28);

/** @brief Call func_80050BC4 with all arguments set to zero. */
void func_801EAE4C(void) {
    func_80050BC4(0, 0, 0);
}

/**
 * @brief Call func_801EAE4C then clear D_801EC301.
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAE74);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAE98);

/**
 * @brief Conditionally read card data and extract value.
 * @param a0 Enable flag (non-zero to proceed)
 * @param a1 First parameter for func_800502C8
 * @param a2 Second parameter for func_800502C8
 * @param a3 Third parameter for func_800502C8
 * @return Extracted card data value, or 0 if disabled or read fails
 */
s32 func_801EB00C(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = 0;
    if (a0 != 0) {
        if (func_800502C8(a1, a2, a3) != 0) {
            func_8004E720(0, 0, &result);
        }
    }
    return result;
}

/**
 * @brief Conditionally check card status and extract value.
 * @param a0 Enable flag (non-zero to proceed)
 * @param a1 Parameter for func_800503F8
 * @return Extracted card status value, or 0 if disabled or check fails
 */
s32 func_801EB054(s32 a0, s32 a1) {
    s32 result = 0;
    if (a0 != 0) {
        if (func_800503F8(a1) != 0) {
            func_8004E720(0, 0, &result);
        }
    }
    return result;
}

/**
 * @brief Convert a BCD-encoded byte to binary.
 *
 * Extracts high nibble, multiplies by 10, adds low nibble.
 *
 * @param a0 BCD-encoded byte value.
 * @return Binary value (0-99).
 */
s32 func_801EB094(s32 a0) {
    s32 hi = (u32)(a0 & 0xFF) >> 4;
    return (hi * 10 + (a0 & 0xF)) & 0xFF;
}

/**
 * @brief Get low byte of save data value via signed modulo.
 * @return Low byte (0-255) of the value returned by func_801F6A5C
 */
s32 func_801EB0B8(void) {
    return (u8)(func_801F6A5C() % 256);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB0F4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB150);

/**
 * @brief Convert a 32-bit value to 8 hex digit characters.
 *
 * Extracts nibbles from a0 (LSB first) and writes them as characters
 * (with offset a2) into buffer a1, right-to-left. Null-terminates.
 *
 * @param a0 Value to convert.
 * @param a1 Output buffer pointer (writes 9 bytes: 8 chars + null).
 * @param a2 Character offset added to each nibble value.
 */
void func_801EB1AC(s32 a0, u8 *a1, s32 a2) {
    s32 i;
    a1 += 8;
    *a1 = 0;
    i = 7;
    do {
        a1--;
        *a1 = (a0 & 0xF) + a2;
        i--;
        a0 = (u32)a0 >> 4;
    } while (i >= 0);
}

/**
 * @brief Accumulate 3 nibbles into a 12-bit value from func_801EB150 output.
 *
 * Calls func_801EB150 to fill a 3-byte buffer, then combines the bytes as:
 * result = (byte0 << 8) | (byte1 << 4) | byte2.
 *
 * @param a0 Parameter passed to func_801EB150
 * @return 12-bit accumulated value
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB1DC);

/**
 * @brief Convert BCD value, add offset, clamp to 99, and encode.
 *
 * Converts a0 from BCD (upper nibble * 10 + lower nibble), adds a1,
 * clamps to maximum 99 (0x63), then encodes via func_801EB1DC.
 *
 * @param a0 BCD-encoded input value
 * @param a1 Offset to add
 * @return Encoded result masked to byte
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB224);

/**
 * @brief Convert BCD value, subtract offset, clamp to 0, and encode.
 *
 * Converts a0 from BCD (upper nibble * 10 + lower nibble), subtracts a1,
 * clamps to minimum 0, then encodes via func_801EB1DC.
 *
 * @param a0 BCD-encoded input value
 * @param a1 Offset to subtract
 * @return Encoded result masked to byte
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB270);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB2B8);

/**
 * @brief Convert save checksum to packed color value.
 *
 * Calls func_801E74BC to get checksum, masks to 24 bits, converts
 * via func_801EB0F4 to RGB bytes, then packs as (R << 8) + (G << 4) + B.
 *
 * @return Packed 12-bit color value.
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB2E4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB334);

/**
 * @brief Look up save icon display value from table.
 *
 * Converts a0 to BCD via func_801EB2B8 (masked to byte), shifts right by 2
 * and adds 6, then adds the halfword from D_801EBD24[a1] to compute
 * the final value passed to func_801EB1DC.
 *
 * @param a0 Input value (masked to byte)
 * @param a1 Table index into D_801EBD24
 * @return Result of func_801EB1DC
 */
/**
 * @note Non-matching: instruction scheduling differs — compiler puts
 * sll (a1*2) before lui/addiu (D_801EBD24 address) and sra (r>>2)
 * after lhu instead of before. Logic is correct.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB408);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB458);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB578);

/**
 * @brief Search byte pair array for a matching entry.
 *
 * Searches through @p a2 consecutive byte pairs in array @p a3.
 * Each pair consists of two bytes read sequentially. Returns 0 if
 * a pair is found where the first byte equals @p a0 and the second
 * byte equals @p a1. Returns 1 if no match is found or if @p a2 <= 0.
 *
 * @param a0 First byte to match.
 * @param a1 Second byte to match.
 * @param a2 Number of pairs to search.
 * @param a3 Pointer to byte pair array.
 * @return 0 if match found, 1 otherwise.
 *
 * @note Non-matching: Compiler allocates loop counter to t0 instead of v1,
 * and schedules return value setup (li v0,1) into blez delay slot
 * instead of counter init.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB5F0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB630);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB6B4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB768);

/**
 * @brief Update save data byte at offset 3 based on bytes at offsets 1 and 6.
 *
 * Gets save data pointer via func_800372D0, refreshes via func_801EB6B4,
 * then computes a new value from bytes at offsets 1 and 6 using func_801EB408.
 */
void func_801EB850(void) {
    u8 *s0 = (u8 *)func_800372D0();
    func_801EB6B4();
    s0[3] = func_801EB408(s0[1], s0[6]);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB890);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB928);
