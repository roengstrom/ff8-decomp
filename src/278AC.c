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


/** @brief Returns a pointer to global D_800786D8. */
u8 *func_800372D0(void) {
    extern u8 D_800786D8;
    return &D_800786D8;
}


/** @brief Sets bit 0x1 in the byte at D_800786D8. */
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

INCLUDE_ASM("asm/nonmatchings/278AC", func_80037BB0);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037BF0);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037C30);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037C6C);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037CD4);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037D40);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037E14);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037E60);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037F08);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80037FB0);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80038030);


INCLUDE_ASM("asm/nonmatchings/278AC", func_800381BC);


INCLUDE_ASM("asm/nonmatchings/278AC", func_80038308);


/** @brief Returns bits 3-4 of the flags word at offset 0x68 through D_800562C4. */
s32 func_80038364(void) {
    extern u8 *D_800562C4;
    return *(s32 *)(D_800562C4 + 0x68) & 0x18;
}


INCLUDE_ASM("asm/nonmatchings/278AC", func_8003837C);


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


