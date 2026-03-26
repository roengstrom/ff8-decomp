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

/**
 * @brief Load sprite data for save menu slot 0x12.
 *
 * Clears the sprite buffer, initiates a DMA read of sprite data
 * to 0x801D1000, waits for completion, then copies the data to
 * the game state region using table entry 0x12 from D_80097424.
 */
void func_801E2860(void) {
    extern u8 D_80097424[];
    extern u8 D_800773C8[];
    s32 v0;
    s32 idx = 0x12;

    func_801E283C();
    loadOverlayDirect(idx, 0x801D1000);
    do {
        v0 = pollCdReadStatus();
    } while (v0 != 0);
    {
        s32 base = D_80097424;
        s32 entry = *(s32 *)(base + idx * 8);
        btlMemcpyForward(0x801D1000, D_800773C8, entry);
    }
}

/**
 * @brief Encode a two-digit number as character codes into a buffer.
 *
 * Divides a1 by 10 to get tens and ones digits, adds 0x824F offset
 * to each, then stores as 4 bytes (high/low of tens, high/low of ones)
 * into buffer at a0.
 *
 * @param a0 Output buffer (4 bytes written, pointer advanced to 5th byte).
 * @param a1 Value to encode (0-99).
 * @return Pointer past the last written byte.
 */
s32 func_801E28C8(u8 *a0, s32 a1) {
    s32 v1 = a1 / 10;
    a1 = a1 - v1 * 10;
    v1 += 0x824F;
    a1 += 0x824F;
    *a0 = v1 >> 8;
    a0++;
    *a0 = v1;
    a0++;
    *a0 = a1 >> 8;
    a0++;
    *a0 = a1;
    return (s32)(a0 + 1);
}

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
void func_801E2D78(s32 a0) {
    func_801F0994(0, 0x22, (a0 % 3) * 53 + 0x3C);
}

/**
 * @brief Process memory card read result and update save slot status.
 *
 * Reads card data via func_801EAE98 using a0[0x57] as the read mode.
 * If read succeeds, extracts status bits and stores them:
 * - Mode 0 (first read): stores to a0[0x2E], a0[0x2C], clears
 *   D_80085148 if no data, sets a0[0x57] to 0x10
 * - Mode != 0 (second read): stores to a0[0x2F], a0[0x2D], clears
 *   D_80085149 if no data, sets a0[0x57] to 0
 *
 * @param a0 Save context pointer.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2DDC);

/**
 * @brief Check save slot validity flags and store result.
 *
 * Sets a0[0x4B] to 1 if either a0[0x2E] or a0[0x2F] is less than 2,
 * otherwise stores (a0[0x2E] < 2).
 *
 * @param a0 Save context pointer.
 * @return The stored flag value.
 */
/**
 * @brief Check if either save slot flag indicates availability.
 *
 * If byte at a0+0x2F >= 2, result is 1. Otherwise result is
 * whether byte at a0+0x2E < 2. Stores and returns the result.
 *
 * @param a0 Save context pointer.
 * @return 1 if available, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2E9C);

/**
 * @brief Select and decode a string ID based on save slot status.
 *
 * Reads the active slot index from a0[0x58], checks the status byte
 * at offset 0x2E of that slot. Depending on a1 (mode selector):
 * - a1 != 0: checks slot status and sub-status byte at 0x2C
 *   to determine string ID (0x20 or 0x25)
 * - a1 == 0: checks slot status and flag at a0[0x49]
 *   to determine string ID (0x28, 0x1F, or 0x0A)
 *
 * @param a0 Save context pointer.
 * @param a1 Mode selector (0 or non-zero).
 * @return Decoded string pointer.
 */
s32 func_801E2ECC(u8 *a0, s32 a1) {
    s32 v0;
    u8 *v1;

    v0 = a0[0x58];
    v1 = a0 + v0;
    v0 = v1[0x2E];

    if (a1 != 0) {
        if ((u32)v0 < 2) {
            if (v1[0x2C] == 1) {
                return func_801E2CBC(0x20);
            }
            return func_801E2C90(0x25);
        }
        return func_801E2C90(0x25);
    } else {
        if ((u32)v0 < 2) {
            s32 id = 0x1F;
            if (a0[0x49] == 1) {
                id = 0x28;
            }
            return func_801E2CBC(id);
        }
        return func_801E2C90(0x0A);
    }
}

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

/**
 * @brief Conditionally render a save menu entry based on status.
 *
 * Checks the signed halfword at a0[0x3C]. If positive, selects
 * string ID 0x29 (if a0[0x53] == 2) or 0x1C (otherwise), and
 * renders via func_801E2C90 + func_801F4274.
 *
 * @param a0 Save context pointer.
 * @param a1 X position for rendering.
 * @param a2 Y position for rendering.
 * @return Render result, or a2 if skipped.
 */
s32 func_801E5AE8(u8 *a0, s32 a1, s32 a2) {
    s32 s0 = a2;
    if (*(s16 *)(a0 + 0x3C) > 0) {
        s32 id = 0x1C;
        if (a0[0x53] == 2) {
            id = 0x29;
        }
        s0 = func_801F4274(a1, s0, func_801E2C90(id), 0xC0, 0x6B, 0x1000);
    }
    return s0;
}

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

/**
 * @brief Look up save slot data and render with func_801F3824.
 *
 * Calls func_801E2800 with the 5th argument to get a save slot pointer,
 * loads the word at offset 0xC, then passes it as the 5th stack arg
 * to func_801F3824 along with adjusted copies of the original arguments.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 Third render parameter (0x100 added).
 * @param a3 Fourth render parameter (0x14 added).
 * @param stack_arg Save slot index for func_801E2800.
 */
/**
 * @brief Look up save slot data and render with func_801F3824.
 *
 * Calls func_801E2800 with the 5th argument to get a save slot pointer,
 * loads the word at offset 0xC, then passes it as the 5th stack arg
 * to func_801F3824 along with adjusted copies of the original arguments.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 Third render parameter (0x100 added).
 * @param a3 Fourth render parameter (0x14 added).
 * @param stack_arg Save slot index for func_801E2800.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5BCC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5C38);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5D04);

/**
 * @brief Look up save slot data and render with func_801F3270.
 *
 * Calls func_801E2800 with the 5th argument to get a save slot pointer,
 * loads the word at offset 0x10, then passes it as the 5th stack arg
 * to func_801F3270 along with adjusted copies of the original arguments.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 Third render parameter (0x100 added).
 * @param a3 Fourth render parameter (0x8 added).
 * @param stack_arg Save slot index for func_801E2800.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5DC4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5E30);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5EE0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6060);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E613C);

/**
 * @brief Build a parameter struct on the stack and call func_8002B898.
 *
 * Constructs a 3-field struct: {u16 a2+0x80, u16 a3+0x1F, u32 0x1600D0}
 * and passes it along with the g_menuColor global to func_8002B898.
 *
 * @param a0 First parameter (passed through).
 * @param a1 Second parameter (passed through).
 * @param a2 Width value (0x80 added).
 * @param a3 Height value (0x1F added).
 */
void func_801E61C0(s32 a0, s32 a1, s32 a2, s32 a3) {
    extern s32 g_menuColor;
    s32 buf[2];
    *(u16 *)((u8 *)buf + 0) = a2 + 0x80;
    *(u16 *)((u8 *)buf + 2) = a3 + 0x1F;
    *(s32 *)((u8 *)buf + 4) = 0x1600D0;
    func_8002B898(a0, a1, (s32)buf, g_menuColor);
}

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

/** @brief Call memzero16 with g_gameState and 0x13A. */
void func_801E73C0(void) {
    extern u8 g_gameState[];
    memzero16(g_gameState, 0x13A);
}

/**
 * @brief Wrapper that calls func_801E7428 (save menu handler).
 */
void func_801E73E8(void) {
    func_801E7428();
}

/**
 * @brief Wrapper that calls recalcPartyStats (save menu input handler).
 */
void func_801E7408(void) {
    recalcPartyStats();
}

/**
 * @brief Initialize save menu handler state.
 *
 * Gets the save handler context, clears the first 0x6E bytes and bytes
 * 0x6E-0x72, clears bit 24 of the flags word at offset 0x70, sets
 * the word at 0x7C to 1, then registers 0x21 menu items via
 * modifyItemQuantity.
 */
void func_801E7428(void) {
    s32 s0;
    u8 *a1 = (u8 *)getInventoryPtr();
    s32 v0;

    s0 = 0x6D;
    v0 = (s32)a1 + 0x6D;
    do {
        *(u8 *)v0 = 0;
        s0--;
        v0--;
    } while (s0 >= 0);

    s0 = 4;
    v0 = (s32)a1 + 4;
    do {
        *(u8 *)(v0 + 0x6E) = 0;
        s0--;
        v0--;
    } while (s0 >= 0);

    *(s32 *)(a1 + 0x70) = *(s32 *)(a1 + 0x70) & (s32)0xFEFFFFFF;
    *(s32 *)(a1 + 0x7C) = 1;

    s0 = 0;
    do {
        modifyItemQuantity(s0 + 0x4D, s0 + 0xC8);
        s0++;
    } while (s0 < 0x21);
}

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

/**
 * @brief Compute save menu state from input flags.
 *
 * Returns -1 if input is -1, clears bit 4 and returns the result
 * if bit 4 is set, otherwise returns 4.
 *
 * @param a0 Input flags value.
 * @return Processed state value.
 */
s32 func_801E7524(s32 a0) {
    if (a0 == -1) {
        return -1;
    }
    if ((a0 & 0x10) == 0) {
        return 4;
    }
    return a0 & ~0x10;
}

/**
 * @brief Compute XOR checksum from game state fields.
 *
 * XORs three values from the g_gameState region (at offsets 0xCD0, 0xB0C,
 * and 0xCDC with the latter shifted left 16), then XORs with the result
 * of func_801E74BC.
 *
 * @return XOR checksum value.
 */
s32 func_801E7550(void) {
    extern u8 g_gameState[];
    s32 base = g_gameState;
    s32 s0 = *(s32 *)(base + 0xCD0);
    s32 v1 = *(s32 *)(base + 0xB0C);
    s32 v0 = *(s32 *)(base + 0xCDC);
    s0 ^= v1;
    s0 ^= v0 << 16;
    v0 = func_801E74BC();
    return s0 ^ v0;
}

/**
 * @brief Compute and store save data checksum fields.
 *
 * Gets save data pointer, checks bit 15 of halfword at offset 0xC.
 * If not set, calls several computation functions and stores results
 * at offsets 0x28 (XOR checksum) and 0x2C (combined checksum byte).
 */
void func_801E7598(void) {
    s32 s0;
    u8 *s1 = (u8 *)getChocoboWorldPtr();
    if ((*(u16 *)(s1 + 0xC) & 0x8000) == 0) {
        func_801EB928();
        func_801EB890();
        *(s32 *)(s1 + 0x28) = func_801E7550();
        s0 = func_801EB0B8();
        s0 += func_801E74BC();
        s1[0x2C] = s0;
    }
}

/**
 * @brief Process save menu input and update state flags.
 *
 * Reads a controller input byte from s0[0x3C], shifts left 4,
 * passes through func_801EAE98 then func_801E7524. Based on result:
 * - 0: checks flags at s1[0] and updates s0[0x45] with bit manipulation
 * - -1: stores 8 to s0[0x45]
 * - positive: stores result to s0[0x45]
 *
 * @param a0 Save menu context pointer.
 * @return Processed input result.
 */
/**
 * @brief Process save menu input and update state flags.
 *
 * Reads a controller input byte from a0[0x3C], shifts left 4,
 * passes through func_801EAE98 then func_801E7524. Based on result:
 * - 0: checks flags at save data and updates a0[0x45] with bit manipulation
 * - -1: stores 8 to a0[0x45]
 * - positive: stores result to a0[0x45]
 *
 * @param a0 Save menu context pointer.
 * @return Processed input result.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E760C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E76C4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7830);

/**
 * @brief Initialize save data structure with checksum and set active flag.
 *
 * Calls getChocoboWorldPtr to get the save data pointer, then func_801E74BC to
 * compute a checksum/value which is stored at offset 0x14. Sets bit 0x2
 * in the flags byte at offset 0x0.
 */
void func_801E78FC(void) {
    u8 *s0 = (u8 *)getChocoboWorldPtr();
    s32 v0 = func_801E74BC();
    *(s32 *)(s0 + 0x14) = v0;
    s0[0] |= 2;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7938);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E79CC);

/**
 * @brief Poll for card data availability, up to 8 attempts.
 *
 * Tries func_8004F200 up to 8 times. If it succeeds, extracts
 * data via func_8004E720 and stores status 2. Returns the
 * result from the stack local.
 *
 * @param a0 Save context pointer (byte at 0x3C used as port).
 * @param a1 Second parameter for func_8004F200.
 * @return Card data result, or 2 if successful.
 */
s32 func_801E7A84(u8 *a0, s32 a1) {
    s32 s0 = 0;
    s32 result;

    do {
        if (func_8004F200(a0[0x3C] << 4, a1, 0x280, 0x80) != 0) {
            func_8004E720(0, 0, &result);
            break;
        }
        s0++;
    } while (s0 < 8);

    if (s0 == 8) {
        result = 2;
    }

    return result;
}

/** @brief Call loadSubOverlay with a0 + 0xB4 and 0x801D3000. */
void func_801E7B18(u8 *a0) {
    loadSubOverlay(a0 + 0xB4, 0x801D3000);
}

/**
 * @brief Compute pointer to save slot data entry by index.
 * @param a0 Save slot index (stride 68).
 * @return Address of D_801EBD5C + a0 * 68.
 *
 */
s32 func_801E7B40(s32 a0) {
    extern u8 D_801EBD5C[];
    s32 base = D_801EBD5C;
    return base + a0 * 68;
}

/**
 * @brief Advance save data loading state machine.
 *
 * Polls pollCdReadStatus for DMA completion. When complete, advances
 * the state byte at a0[0x44]:
 * - State 0: transition to 1
 * - State 1: initiate read via loadSubOverlay, transition to 2
 * - State 2: transition to 3
 * - State 3+: no-op
 *
 * @param a0 Save context pointer.
 */
/**
 * @brief Advance save data loading state machine.
 *
 * Polls pollCdReadStatus for DMA completion. When complete, advances
 * the state byte at a0[0x44]:
 * - State 0: transition to 1
 * - State 1: initiate read via loadSubOverlay, transition to 2
 * - State 2: transition to 3
 * - State 3+: no-op
 *
 * @param a0 Save context pointer.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7B5C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7BE8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9804);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E98C8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9988);

/**
 * @brief Render a centered text string in the save menu.
 *
 * If the string pointer a2 is non-null, computes the pixel width
 * via func_8002E680 and func_801F7394, centers it within 0xD8 pixels,
 * and renders via func_801F4274.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter / default return value.
 * @param a2 String pointer (null to skip rendering).
 * @return Render result from func_801F4274, or a1 if a2 is null.
 */
s32 func_801E9A30(s32 a0, s32 a1, s32 a2) {
    s32 s0 = a1;
    if (a2 != 0) {
        s32 v0 = func_8002E680(a2);
        v0 = func_801F7394((u32)v0 >> 16);
        s0 = func_801F4274(a0, s0, a2, 0xC0, (u32)(0xD8 - v0) >> 1, 0x1000);
    }
    return s0;
}

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

/**
 * @brief Copy rectangle parameters with offset and dispatch rendering.
 *
 * Copies 4 halfwords from the 5th arg pointer into g_menuDisplayCfg,
 * adding a2 to the X coordinate and a3 to the Y coordinate.
 * Then calls func_801E9CD4 with the modified display config.
 *
 * @param a0 First render parameter (passed through).
 * @param a1 Second render parameter (passed through).
 * @param a2 X offset added to source X coordinate.
 * @param a3 Y offset added to source Y coordinate.
 * @param src Pointer to source rectangle (4 halfwords: x, y, w, h).
 */
void func_801E9D84(s32 a0, s32 a1, s32 a2, s32 a3, u16 *src) {
    extern s32 g_menuColor;
    extern u16 g_menuDisplayCfg[];
    g_menuDisplayCfg[0] = src[0] + a2;
    g_menuDisplayCfg[1] = src[1] + a3;
    g_menuDisplayCfg[2] = src[2];
    g_menuDisplayCfg[3] = src[3];
    func_801E9CD4(a0, a1, (s32)g_menuDisplayCfg, g_menuColor, (s32)src);
}

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
 */
/**
 * @brief Call func_801EAE4C then clear D_801EC301.
 */
void func_801EAE74(void) {
    extern u8 D_801EC301;
    func_801EAE4C();
    D_801EC301 = 0;
}

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

/**
 * @brief Convert a value to variable-radix digit string using D_801EBCF8 table.
 *
 * For each entry in the radix table, repeatedly subtracts the divisor from
 * a0 to extract a digit, adds the character offset a2, and stores the
 * result into output buffer a1. Null-terminates the output.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
/**
 * @brief Convert a value to variable-radix digit string using D_801EBCF8 table.
 *
 * For each entry in the radix table, repeatedly subtracts the divisor from
 * a0 to extract a digit, adds the character offset a2, and stores the
 * result into output buffer a1. Null-terminates the output.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
/**
 * @brief Convert a value to variable-radix digit string using D_801EBCF8 table.
 *
 * For each entry in the radix table, repeatedly subtracts the divisor from
 * a0 to extract a digit, adds the character offset a2, and stores the
 * result into output buffer a1. Null-terminates the output.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB0F4);

/**
 * @brief Convert a value to variable-radix digit string using D_801EBD14 table.
 *
 * Same algorithm as func_801EB0F4 but uses the D_801EBD14 radix table.
 *
 * @param a0 Value to convert.
 * @param a1 Output character buffer.
 * @param a2 Character offset added to each digit.
 */
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
 */
s32 func_801EB1DC(s32 a0) {
    u8 buf[8];
    s32 result;
    s32 i;
    u8 *ptr;

    func_801EB150(a0, buf, 0);
    ptr = buf;
    result = 0;
    i = 2;
    do {
        result = result << 4;
        result = result + *ptr;
        ptr++;
        i--;
    } while (i >= 0);
    return result;
}

/**
 * @brief Convert BCD value, add offset, clamp to 99, and encode.
 *
 * Converts a0 from BCD (upper nibble * 10 + lower nibble), adds a1,
 * clamps to maximum 99 (0x63), then encodes via func_801EB1DC.
 *
 * @param a0 BCD-encoded input value
 * @param a1 Offset to add
 * @return Encoded result masked to byte
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
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB270);

/**
 * @brief Convert BCD-encoded byte to decimal, or return 100 if zero.
 *
 * If a0 is zero, returns 100 (0x64). Otherwise extracts high nibble,
 * multiplies by 10, adds low nibble for BCD-to-binary conversion.
 *
 * @param a0 BCD-encoded value.
 * @return Decimal value, or 100 if input is zero.
 */
/**
 * @brief Convert BCD-encoded byte to decimal, or return 100 if zero.
 *
 * If a0 is zero, returns 100 (0x64). Otherwise extracts high nibble,
 * multiplies by 10, adds low nibble for BCD-to-binary conversion.
 *
 * @param a0 BCD-encoded value.
 * @return Decimal value, or 100 if input is zero.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB2B8);

/**
 * @brief Convert save checksum to packed color value.
 *
 * Calls func_801E74BC to get checksum, masks to 24 bits, converts
 * via func_801EB0F4 to RGB bytes, then packs as (R << 8) + (G << 4) + B.
 *
 * @return Packed 12-bit color value.
 *
 */
s32 func_801EB2E4(void) {
    u8 buf[16];
    s32 v0;

    v0 = func_801E74BC();
    func_801EB0F4(v0 & 0xFFFFFF, buf, 0);
    return (buf[7] << 8) + (buf[8] << 4) + buf[9];
}

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
s32 func_801EB408(s32 a0, s32 a1) {
    extern u16 D_801EBD24[];
    s32 v0;
    s32 a0_new;

    v0 = func_801EB2B8(a0 & 0xFF);
    a0_new = D_801EBD24[a1];
    v0 = (v0 >> 2) + 6;
    return func_801EB1DC(a0_new + v0);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB458);

/**
 * @brief Find a unique coordinate pair by repeated random sampling.
 *
 * Extracts target x from a0>>10 and target y from a1>>10. Repeatedly
 * calls func_801EB0B8 to get random values, converting to coordinates
 * via (val * 9) >> 7. Loops until the generated coordinates differ
 * from the targets.
 *
 * @param a0 Target x coordinate (encoded in bits 10+).
 * @param a1 Target y coordinate (encoded in bits 10+).
 * @return Combined coordinate: x | (y << 8).
 */
s32 func_801EB578(s32 a0, s32 a1) {
    s32 targetX = a0 >> 10;
    s32 targetY = a1 >> 10;
    s32 x, y;

    do {
        x = (u32)((func_801EB0B8() & 0xFF) * 9) >> 7;
        y = (u32)((func_801EB0B8() & 0xFF) * 9) >> 7;
    } while (x == targetX && y == targetY);

    return x | (y << 8);
}

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
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB5F0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB630);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB6B4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB768);

/**
 * @brief Update save data byte at offset 3 based on bytes at offsets 1 and 6.
 *
 * Gets save data pointer via getChocoboWorldPtr, refreshes via func_801EB6B4,
 * then computes a new value from bytes at offsets 1 and 6 using func_801EB408.
 */
void func_801EB850(void) {
    u8 *s0 = (u8 *)getChocoboWorldPtr();
    func_801EB6B4();
    s0[3] = func_801EB408(s0[1], s0[6]);
}

/**
 * @brief Compute and store save data timing values.
 *
 * Gets save data pointer, computes values from func_801EB408 and
 * func_801EB0B8, transforms with multiply-shift (val * 9 >> 7),
 * and stores at offsets 0x03, 0x10, 0x12, 0x32 in the save data.
 * Finally calls func_801EB850 to update checksum.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB890);

/**
 * @brief Initialize save data structure with default values.
 *
 * Gets save data pointer, computes initial color and timing values
 * via func_801EB2E4, func_801EB334, func_801EB408, func_801EB458.
 * Sets various flag bytes and clears timing fields.
 */
void func_801EB928(void) {
    s32 s0 = 1;
    u8 *s1 = (u8 *)getChocoboWorldPtr();
    s32 v0;
    s32 a0;

    v0 = func_801EB2E4();
    v0 |= 0x8000;
    *(u16 *)(s1 + 0xC) = v0;
    a0 = *(u16 *)(s1 + 0xC);
    s1[1] = s0;
    v0 = func_801EB334(a0);
    s1[6] = v0;
    v0 = func_801EB408(s1[1], s1[6]);
    s1[3] = v0;
    s1[2] = v0;
    v0 = func_801EB458(s1[3]);
    *(u16 *)(s1 + 4) = v0;
    s1[7] = 4;
    s1[0xF] = 2;
    s1[0] = s0;
    s1[0x2F] = 7;
    s1[0x2C] = 0;
    {
        s32 i = 3;
        u8 *p = s1 + 3;
        do {
            *(p + 0x14) = 0;
            i--;
            p--;
        } while (i >= 0);
    }
    s1[0x2E] = 0;
    *(s32 *)(s1 + 8) = 0;
    s1[0x2D] = 0;
}
