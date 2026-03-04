#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/131FC", func_800229FC);


INCLUDE_ASM("asm/nonmatchings/131FC", func_80022B04);


/**
 * @brief Accumulate status immunity flags as an RGB-packed bitmask from a character's equipped abilities.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Packed bitmask: (B << 16) | (G << 8) | R, OR'd across up to 4 matching ability slots.
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x3A..0x4D are looked up
 *       in D_80078E00 (stride 8, offsets 0x42B5/B6/B7 for R/G/B status immunity bytes).
 */
s32 func_80022B48(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    s32 result = 0;
    s32 i = 0;
    s32 base1 = (s32)g_gameState;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x3A;
        if ((u32)idx < 0x14) {
            s32 entry = idx * 8 + base2;
            u8 b = *(u8 *)(entry + 0x42B7);
            u8 g = *(u8 *)(entry + 0x42B6);
            u8 r = *(u8 *)(entry + 0x42B5);
            result |= (b << 16) | (g << 8) | r;
        }
        i++;
    } while (i < 4);
    return result;
}


/**
 * @brief Check if any of 4 command slots has command type 6 (likely "Magic" or a special command).
 * @param a0 Pointer to an array of command entries (4 bytes each, type field at offset 0x1E).
 * @return 1 if any slot has type == 6, 0 otherwise.
 */
s32 func_80022BD0(u8 *a0) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == 6) return 1;
        a0 += 4;
    }
    return 0;
}


/**
 * @brief Determine a character's magic availability flags based on status and command slots.
 * @param a0 Pointer to a character data structure.
 * @return Flags: bit 0 set if status bit 0x20000 is active; bit 1 set if magic commands present
 *         (unless D_80082C10 bit 3 is set, which suppresses the magic flag).
 */
s32 func_80022C04(s32 a0) {
    extern u8 D_80082C10;
    s32 val = *(s32 *)(a0 + 0x190);
    s32 masked = val & 0x20000;
    s32 flag = masked != 0;
    if (func_80022BD0(a0)) {
        if (D_80082C10 & 8) {
            return flag;
        }
        flag |= 2;
    }
    return flag;
}


/**
 * @brief Apply party ability flags from a character's equipped abilities to D_80078720.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x4E..0x52 are looked up
 *       in D_80078E00 (stride 8, offset 0x4355) and OR'd into D_80078720 offset 0x6D8.
 *       Likely enables field/world abilities (e.g., encounter-none, rare-item).
 */
void func_80022C5C(s32 a0) {
    extern u8 g_gameState[];
    extern u8 D_80078E00[];
    extern u8 D_80078720[];
    s32 i = 0;
    s32 base1 = (s32)g_gameState;
    s32 base3;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base3 = (s32)D_80078720;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x4E;
        if ((u32)idx < 5) {
            *(u8 *)(base3 + 0x6D8) |= *(u8 *)(idx * 8 + base2 + 0x4355);
        }
        i++;
    } while (i < 4);
}


INCLUDE_ASM("asm/nonmatchings/131FC", func_80022CDC);


/**
 * @brief Clear a character's magic, item, and command slot data.
 * @param a0 Pointer to a character data structure.
 * @note Zeroes out three arrays: 32 entries of 5 bytes at offset 0x82 (magic inventory),
 *       16 entries of 5 bytes at offset 0x122 (item inventory), 4 entries of 4 bytes at
 *       offset 0x1E (command slots), plus fields at 0x1C, 0x1D, and a u16 at 0x14.
 */
void func_80022D78(s32 a0) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        *(u8 *)(a0 + i * 5 + 0x84) = 0;
        *(u8 *)(a0 + i * 5 + 0x85) = 0;
        *(u8 *)(a0 + i * 5 + 0x86) = 0;
        *(u8 *)(a0 + i * 5 + 0x83) = 0;
        *(u8 *)(a0 + i * 5 + 0x82) = 0;
    }

    for (i = 0; i < 0x10; i++) {
        *(u8 *)(a0 + i * 5 + 0x124) = 0;
        *(u8 *)(a0 + i * 5 + 0x125) = 0;
        *(u8 *)(a0 + i * 5 + 0x126) = 0;
        *(u8 *)(a0 + i * 5 + 0x123) = 0;
        *(u8 *)(a0 + i * 5 + 0x122) = 0;
    }

    for (i = 0; i < 4; i++) {
        *(u8 *)(a0 + i * 4 + 0x20) = 0;
        *(u8 *)(a0 + i * 4 + 0x21) = 0;
        *(u8 *)(a0 + i * 4 + 0x1F) = 0;
        *(u8 *)(a0 + i * 4 + 0x1E) = 0;
    }

    *(u8 *)(a0 + 0x1C) = 0;
    *(u8 *)(a0 + 0x1D) = 0;
    *(u16 *)(a0 + 0x14) = 0;
}


INCLUDE_ASM("asm/nonmatchings/131FC", func_80022E08);


/**
 * @brief Find the index of a command slot matching a given command type.
 * @param a0 Pointer to an array of command entries (4 bytes each, type field at offset 0x1E).
 * @param a1 Command type to search for.
 * @return Index (0-3) of the matching slot, or 0xFF if not found.
 */
s32 func_80023180(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == a1) return i;
        a0 += 4;
    }
    return 0xFF;
}


/**
 * @brief Clamp a value to the 0-255 (u8) range.
 * @param a0 Value to clamp.
 * @return a0 if < 256, otherwise 255.
 */
s32 func_800231B0(s32 a0) {
    if (a0 >= 0x100) {
        return 0xFF;
    }
    return a0;
}


/**
 * @brief Clamp a value to the 0-9999 range.
 * @param a0 Value to clamp.
 * @return a0 as u16 if < 10000, otherwise 9999.
 * @note Commonly used to cap HP or similar stats at the FF8 maximum of 9999.
 */
s32 func_800231C8(s32 a0) {
    if (a0 >= 0x2710) {
        return 0x270F;
    }
    return (u16)a0;
}


INCLUDE_ASM("asm/nonmatchings/131FC", func_800231E0);


INCLUDE_ASM("asm/nonmatchings/131FC", func_8002363C);


/**
 * @brief Iterate over 16 GF entries and recalculate stats for each active one.
 * @note Checks g_gameState entries (stride 0x44) for bit 0 of field 0x61 (active flag).
 *       Calls func_8002363C for each active GF to recalculate its derived stats.
 */
void func_80023828(void) {
    extern u8 g_gameState[];
    s32 i = 0;
    u8 *ptr = g_gameState;
    do {
        if (ptr[0x61] & 1) {
            func_8002363C(i);
        }
        i++;
        ptr += 0x44;
    } while (i < 16);
}


/**
 * @brief Recalculate stats for all 3 party members and their GFs.
 * @note Resets D_80078DF8, then for each of the 3 party slots calls func_80022E08 and
 *       func_800231E0 to recalculate character stats. Finally calls func_80023828 for GFs.
 */
void func_80023888(void) {
    extern u8 D_80078DF8;
    extern u8 g_gameState[];
    s32 i;
    s32 base;
    s32 ptr;

    D_80078DF8 = 0;
    i = 0;
    base = (s32)g_gameState;
    do {
        ptr = i + base;
        func_80022E08(*(u8 *)(ptr + 0xAF4), i);
        func_800231E0(*(u8 *)(ptr + 0xAF4), i);
        i++;
    } while (i < 3);
    func_80023828();
}


