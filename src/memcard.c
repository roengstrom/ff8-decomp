#include "common.h"
#include "memcard.h"

extern u8 g_gameVersion;

static u32 mcXorChecksum(u8 *frame);
static void mcInitHeader(u8 *frame);
static void mcInitDirUsed(u8 *frame);
static void mcInitDirFree(u8 *frame);
static void mcFillFF(u8 *buf);

s32 func_800377B4(s32 len, u8 *buf)
{
    u16 table[256];
    u32 crc;
    u32 i;
    u32 val;
    u32 j;
    s32 ci;
    ci = 0xFF;
    do { table[ci] = 0; ci--; } while (ci >= 0);
    crc = 0xFFFF;
    i = 0;
    do {
        val = i << 8;
        j = 0;
        do {
            if (val & 0x8000) val = (val << 1) ^ 0x1021;
            else val <<= 1;
            j++;
            table[i] = val;
        } while (j < 8);
        i++;
    } while (i < 0xFF);
    if (len > 0) {
        do {
            u32 idx;
            idx = *buf++;
            len--;
            idx ^= (crc & 0xFFFF) >> 8;
            crc = table[idx] ^ (crc << 8);
        } while (len > 0);
    }
    return ~crc & 0xFFFF;
}


/**
 * @brief Compute an XOR checksum over 127 bytes (memory card frame header).
 *
 * XORs each of the first 127 bytes together and returns the low 8 bits.
 * Used to compute byte 0x7F of a memory card block header.
 *
 * @param a0 Pointer to the 128-byte memory card frame.
 * @return XOR checksum (0-255).
 */
static u32 mcXorChecksum(u8 *frame) {
    u32 acc = 0;
    s32 i = 0;
    do {
        acc ^= *frame++;
        i++;
    } while ((u32)i < 0x7F);
    return acc & 0xFF;
}


/**
 * @brief Zero 128 bytes of memory (memory card frame header size).
 * @param ptr Pointer to the buffer to clear.
 */
void mcZeroFrame(u8 *ptr) {
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
static void mcInitHeader(u8 *frame) {
    mcZeroFrame(frame);
    frame[0] = 0x4D;
    frame[1] = 0x43;
    frame[0x7F] = mcXorChecksum(frame);
}


/**
 * @brief Initialize a memory card directory frame (used save slot, type 0xA0).
 *
 * Zeroes the 128-byte frame, sets byte 0 to 0xA0 (in-use flag),
 * bytes 8-9 to 0xFF, then computes and stores the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
static void mcInitDirUsed(u8 *frame) {
    mcZeroFrame(frame);
    frame[0] = 0xA0;
    frame[8] = 0xFF;
    frame[9] = 0xFF;
    frame[0x7F] = mcXorChecksum(frame);
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
static void mcInitDirFree(u8 *frame) {
    mcZeroFrame(frame);
    frame[0] = 0xFF;
    frame[1] = 0xFF;
    frame[2] = 0xFF;
    frame[3] = 0xFF;
    frame[8] = 0xFF;
    frame[9] = 0xFF;
    frame[0x7F] = mcXorChecksum(frame);
}


/** @brief Fills 128 bytes at a0 with 0xFF.
 *  @param a0 Pointer to buffer.
 */
static void mcFillFF(u8 *buf) {
    s32 i = 128;
    do {
        *buf++ = 0xFF;
    } while (--i > 0);
}


s32 func_800379AC(s32 arg0)
{
    u8 buffer[0x80];
    s32 i;
    if (pollCardReady() != 2)
        return 0;
    mcFillFF(buffer);
    if (!writeCardBlock(arg0, buffer, 0))
        return 0;
    mcInitDirUsed(buffer);
    for (i = 0; i < 0xF; i++)
        if (!writeCardBlock(arg0, buffer, i + 1))
            return 0;
    mcInitDirFree(buffer);
    for (i = 0; i < 0x14; i++)
        if (!writeCardBlock(arg0, buffer, i + 0x10))
            return 0;
    mcFillFF(buffer);
    for (i = 0; i < 0x1C; i++)
        if (!writeCardBlock(arg0, buffer, i + 0x24))
            return 0;
    mcInitHeader(buffer);
    if (!writeCardBlock(arg0, buffer, 0))
        return 0;
    markCardBusy(arg0);
    return 1;
}


/** @brief Sets global g_gameVersion to VERSION_USA. */
void InitGameVersion(void) {
    g_gameVersion = VERSION_USA;
}


/** @brief Returns the unsigned byte value of global g_gameVersion. */
GameVersion GetGameVersion(void) {
    return g_gameVersion;
}
