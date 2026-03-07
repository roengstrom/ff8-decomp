#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"

INCLUDE_ASM("asm/nonmatchings/278AC", func_800370AC);


/**
 * @brief Set a bit in the global bitfield array D_8007809B.
 * @param a0 Bit index to set.
 */
void func_80037198(s32 a0) {
    extern u8 D_8007809B[];
    u8 *base = D_8007809B;
    s32 byteIdx = a0 / 8;
    base[byteIdx] |= (1 << (a0 & 7));
}


/**
 * @brief Clear a bit in the global bitfield array D_8007809B.
 * @param a0 Bit index to clear.
 */
void func_800371D0(s32 a0) {
    extern u8 D_8007809B[];
    u8 *base = D_8007809B;
    s32 byteIdx = a0 / 8;
    base[byteIdx] &= ~(1 << (a0 & 7));
}


/**
 * @brief Test a bit in the global bitfield array D_8007809B.
 * @param a0 Bit index to test.
 * @return Non-zero if bit is set, zero otherwise.
 */
s32 func_8003720C(s32 a0) {
    extern u8 D_8007809B[];
    u8 *base = D_8007809B;
    s32 byteIdx = a0 / 8;
    return base[byteIdx] & (1 << (a0 & 7));
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037240);


/** @brief Returns a pointer to global g_chocoboWorld. */
u8 *func_800372D0(void) {
    extern u8 g_chocoboWorld;
    return &g_chocoboWorld;
}


/** @brief Sets bit 0x1 in the byte at g_chocoboWorld. */
void func_800372DC(void) {
    u8 *p = func_800372D0();
    *p |= 0x1;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037308);


INCLUDE_ASM("asm/nonmatchings/278AC", func_800375A0);


/**
 * Wrapper for func_800375A0 with fixed 6th argument 0x64808080.
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 * @param arg4 Fifth argument passed through from caller's stack
 */
void func_80037678(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    func_800375A0(a0, a1, a2, a3, arg4, 0x64808080);
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_800376A8);


// mc_xor_checksum

/**
 * @brief Call func_800376A8 with constant 7th arg (0x64808080).
 *
 * Passes through all 6 caller args and appends 0x64808080
 * as the 7th argument.
 */
void func_8003777C(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4, s32 arg5) {
    func_800376A8(a0, a1, a2, a3, arg4, arg5, 0x64808080);
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_800377B4);


/**
 * @brief Compute an XOR checksum over 127 bytes (memory card frame header).
 *
 * XORs each of the first 127 bytes together and returns the low 8 bits.
 * Used to compute byte 0x7F of a memory card block header.
 *
 * @param a0 Pointer to the 128-byte memory card frame.
 * @return XOR checksum (0-255).
 */
u32 func_8003786C(u8 *a0) {
    u32 acc = 0;
    s32 i = 0;
    do {
        acc ^= *a0++;
        i++;
    } while ((u32)i < 0x7F);
    return acc & 0xFF;
}


/**
 * @brief Zero 128 bytes of memory (memory card frame header size).
 * @param ptr Pointer to the buffer to clear.
 */
void func_80037894(u8 *ptr) {
    s32 count = 128;
    do {
        *ptr++ = 0;
    } while (--count > 0);
}


/**
 * @brief Initialize a memory card header frame (block 0, "MC" magic).
 *
 * Zeroes the 128-byte frame, sets bytes 0-1 to 'M','C' (0x4D, 0x43),
 * then computes and stores the XOR checksum at byte 0x7F.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void func_800378B0(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0x4D;
    a0[1] = 0x43;
    a0[0x7F] = func_8003786C(a0);
}


/**
 * @brief Initialize a memory card directory frame (used save slot, type 0xA0).
 *
 * Zeroes the 128-byte frame, sets byte 0 to 0xA0 (in-use flag),
 * bytes 8-9 to 0xFF, then computes and stores the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void func_800378F4(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0xA0;
    a0[8] = 0xFF;
    a0[9] = 0xFF;
    a0[0x7F] = func_8003786C(a0);
}


/**
 * @brief Initialize a memory card free/unused directory frame.
 *
 * Zeroes the 128-byte frame, sets bytes 0-3 and 8-9 to 0xFF
 * (marks the directory entry as free), then computes and stores
 * the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void func_8003793C(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0xFF;
    a0[1] = 0xFF;
    a0[2] = 0xFF;
    a0[3] = 0xFF;
    a0[8] = 0xFF;
    a0[9] = 0xFF;
    a0[0x7F] = func_8003786C(a0);
}


/** @brief Fills 128 bytes at a0 with 0xFF.
 *  @param a0 Pointer to buffer.
 */
void func_8003798C(u8 *a0) {
    s32 i = 128;
    do {
        *a0++ = 0xFF;
    } while (--i > 0);
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_800379AC);


/** @brief Sets global D_80085218 to 1. */
void func_80037ACC(void) {
    extern u8 D_80085218;
    D_80085218 = 1;
}


/** @brief Returns the unsigned byte value of global D_80085218. */
u32 func_80037ADC(void) {
    extern u8 D_80085218;
    return D_80085218;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037AEC);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037B44);

INCLUDE_ASM("asm/nonmatchings/278AC", func_80037B7C);

/**
 * @brief Search g_gameState party slots (offset 0xD38) for a matching byte.
 *
 * Iterates through 3 bytes at g_gameState[0xD38..0xD3A] looking for
 * a match with the low byte of a0.
 *
 * @param a0 Value to search for (low 8 bits used).
 * @return Slot index (0-2) if found, 0xFF if not found.
 */
s32 func_80037BB0(s32 a0) {
    extern u8 g_gameState[];
    s32 i = 0;
    s32 base = (s32)g_gameState;
    a0 &= 0xFF;
    do {
        if (*(u8 *)(i + base + 0xD38) == a0) {
            return (u8)i;
        }
        i++;
    } while (i < 3);
    return 0xFF;
}


/**
 * @brief Search g_gameState slots (offset 0xAF4) for a matching byte.
 *
 * Iterates through 3 bytes at g_gameState[0xAF4..0xAF6] looking for
 * a match with the low byte of a0.
 *
 * @param a0 Value to search for (low 8 bits used).
 * @return Slot index (0-2) if found, 0xFF if not found.
 */
s32 func_80037BF0(s32 a0) {
    extern u8 g_gameState[];
    s32 i = 0;
    s32 base = (s32)g_gameState;
    a0 &= 0xFF;
    do {
        if (*(u8 *)(i + base + 0xAF4) == a0) {
            return (u8)i;
        }
        i++;
    } while (i < 3);
    return 0xFF;
}


/**
 * @brief Search for a card value in the hand slots.
 *
 * Iterates through 8 hand slots (stride 0x98 in g_gameState, offset 0x498)
 * looking for a match with the low byte of a0.
 *
 * @param a0 Card value to search for (low 8 bits used).
 * @return Slot index (0-7) if found, 0xFF if not found.
 */
s32 func_80037C30(s32 a0) {
    extern u8 g_gameState[];
    s32 i = 0;
    s32 base;
    a0 &= 0xFF;
    base = (s32)g_gameState;
    do {
        if (*(u8 *)(base + 0x498) == a0) {
            return (u8)i;
        }
        i++;
        base += 0x98;
    } while (i < 8);
    return 0xFF;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037C6C);


/**
 * @brief Stop all playing sound channels and reset sound state.
 *
 * Reads two sound handles from the entity pointer at D_800562C4 (offsets
 * 0x6C and 0x70), stops each via func_80013744. Then calls func_80013168
 * to flush sound state and func_80013478 to reset channel 0.
 */
void func_80037CD4(void) {
    extern u8 *D_800562C4;
    s32 val;
    func_80013744(*(s32 *)(D_800562C4 + 0x6C), 15, 0);
    val = *(s32 *)(D_800562C4 + 0x70);
    if (val != -1) {
        func_80013744(val, 15, 0);
    }
    func_80013168();
    func_80013478(0, 15);
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037D40);


/**
 * @brief Toggle the sound bank selector and return the corresponding bank table.
 *
 * XORs byte at D_800562C4[0xC9] with 1, then reloads and checks:
 * returns D_80063388 if the toggled value is non-zero, D_8005F388 otherwise.
 *
 * @return Pointer to the selected sound bank table.
 */
u8 *func_80037E14(void) {
    extern u8 *D_800562C4;
    extern u8 D_8005F388[];
    extern u8 D_80063388[];

    *(u8 *)(D_800562C4 + 0xC9) ^= 1;
    if (*(s8 *)(D_800562C4 + 0xC9) == 0) {
        return D_8005F388;
    }
    return D_80063388;
}


/**
 * @brief Load and apply sound data from disc (variant A).
 *
 * Polls func_80013CA4 until it returns 0, then reads sound data from
 * D_80085220 using func_80039728 and plays it via func_80013EE4.
 * Reads sound data a second time, selects a bank table (D_8005F388
 * or D_80063388) based on D_800562C4 field 0xC9, and calls
 * func_80039678. Sets the completion flag at D_800562C4 + 0xD6.
 */
void func_80037E60(void) {
    extern s32 D_80085220;
    extern u8 *D_800562C4;
    extern u8 D_8005F388[];
    extern u8 D_80063388[];
    s32 size;
    s32 result;
    u8 *table;

    do {
    } while (func_80013CA4() != 0);
    result = func_80039728(D_80085220, 1, &size);
    func_80013EE4(result, 1);
    result = func_80039728(D_80085220, 0, &size);
    if (*(s8 *)(D_800562C4 + 0xC9) != 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    func_80039678((s32)table, result, size);
    *(u8 *)(D_800562C4 + 0xD6) = 1;
}


/**
 * @brief Load and apply sound data from disc (variant B).
 *
 * Same as func_80037E60, but the bank table selection is inverted:
 * uses D_80063388 when D_800562C4 field 0xC9 is non-zero, and
 * D_8005F388 when it is zero.
 */
void func_80037F08(void) {
    extern s32 D_80085220;
    extern u8 *D_800562C4;
    extern u8 D_8005F388[];
    extern u8 D_80063388[];
    s32 size;
    s32 result;
    u8 *table;

    do {
    } while (func_80013CA4() != 0);
    result = func_80039728(D_80085220, 1, &size);
    func_80013EE4(result, 1);
    result = func_80039728(D_80085220, 0, &size);
    if (*(s8 *)(D_800562C4 + 0xC9) == 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    func_80039678((s32)table, result, size);
    *(u8 *)(D_800562C4 + 0xD6) = 1;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037FB0);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80038030);


INCLUDE_ASM("asm/nonmatchings/278AC", func_800381BC);


/**
 * @brief Clear entity flag bits 0x44 from active battle entities, then update.
 *
 * Iterates over D_80085388 entries at stride 0x264 (612 bytes) starting
 * from D_80085224 + 0x160, clearing bits 2 and 6 of the flags word at
 * each entry. Then calls func_800381BC to apply the changes.
 */
void func_80038308(void) {
    extern s32 D_80085224;
    extern u8 D_80085388;
    s32 base = D_80085224;
    s32 count = D_80085388;
    s32 i = 0;

    if (count != 0) {
        s32 mask = ~0x44;
        s32 ptr = base + 0x160;
        do {
            *(s32 *)ptr &= mask;
            i++;
            ptr += 0x264;
        } while (i < count);
    }
    func_800381BC();
}


/** @brief Returns bits 3-4 of the flags word at offset 0x68 through D_800562C4. */
s32 func_80038364(void) {
    extern u8 *D_800562C4;
    return *(s32 *)(D_800562C4 + 0x68) & 0x18;
}


/**
 * @brief Extract a 2-bit field from the packed bitfield array at D_800562C4+0x74.
 *
 * Treats the byte array as a packed 2-bit-per-entry table. Computes byte
 * index (a0/4) and bit position ((a0%4)*2), then extracts and returns
 * the 2-bit value.
 *
 * @param a0 Entry index (low 8 bits used).
 * @return The 2-bit value (0-3) at the given index.
 */
s32 func_8003837C(s32 a0) {
    extern u8 *D_800562C4;
    u8 *ptr = D_800562C4;
    s32 idx;

    a0 &= 0xFF;
    idx = a0 / 4;
    return (*(u8 *)(ptr + idx + 0x74) >> ((a0 - idx * 4) * 2)) & 3;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_800383B8);


/** @brief Looks up byte from D_8005644B table at index a0 (masked to 8 bits).
 *  @param a0 Table index (only low 8 bits used).
 *  @return The byte value at D_8005644B[a0 & 0xFF].
 */
u8 func_80038424(s32 a0) {
    extern u8 D_8005644B[];
    return D_8005644B[a0 & 0xFF];
}


/** @brief Returns halfword from D_800562C8 table indexed by D_80077E5A. */
u16 func_80038440(void) {
    extern u8 D_80077E5A;
    extern u16 D_800562C8[];
    return D_800562C8[D_80077E5A];
}


/** @brief Linear congruential generator: D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039.
 *  @return Bits 16-30 of the new state (0-32767).
 */
s32 func_80038464(void) {
    extern s32 D_800562D4;
    D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039;
    return ((u32)D_800562D4 >> 16) & 0x7FFF;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_80038490);


