#include "common.h"
#include "battle.h"
#include "game.h"
#include "battle/bc_object8.h"

// returns 1-A as a string value
u8 func_800B00E8(s32 arg0) {
    switch (arg0) {
        case 0:
            return getMenuString(0xB)[1];

        case 1:
            return getMenuString(0xB)[2];
            
        case 2:
            return getMenuString(0xB)[3];

        case 3:
            return getMenuString(0xB)[4];
            
        case 4:
            return getMenuString(0xB)[5];

        case 5:
            return getMenuString(0xB)[6];

        case 6:
            return getMenuString(0xB)[7];

        case 7:
            return getMenuString(0xB)[8];

        case 8:
            return getMenuString(0xB)[9];
            
        case 9:
            return getMenuString(0xB)[10];
    }
}


/**
 * @brief Copy a null-terminated string from src to dst.
 *
 * @param dst Destination buffer.
 * @param src Source string.
 */
void func_800B01E8(u8 *dst, u8 *src) {
    u8 ch;
    do {
        ch = *src++;
        *dst++ = ch;
    } while (ch != 0);
}

/**
 * @brief Copy string with optional terminator replacement.
 *
 * Copies bytes from src to dst until a null byte is found, counting
 * the number of non-null bytes copied (added to initial len). After
 * copying, if the terminator byte (masked to 8 bits) equals 7, returns
 * the length. Otherwise, overwrites the null with the terminator byte
 * and returns length + 1.
 *
 * @param a0 Destination buffer.
 * @param a1 Source buffer (as integer).
 * @param a2 Initial length counter.
 * @param a3 Terminator byte (only low 8 bits used).
 * @return Final length of written data.
 */
s32 func_800B0204(u8* arg0, u8* arg1, s32 arg2, u8 arg3) {
    while (*arg0++ = *arg1++) {
        arg2++;
    }
    
    if (arg3 == 7) {
       return arg2;
    }
    
    *(arg0 - 1) = arg3;
    return arg2 + 1;
}

/**
 * @brief Build a string in D_800EEBE8 from two parts using func_800B0204.
 *
 * Writes the first part with a1 as length byte, then appends the
 * second part starting at the returned offset.
 *
 * @param a0 First part data.
 * @param a1 Length/type byte for first part (masked to 8 bits).
 * @param a2 Second part data.
 * @return Pointer to D_800EEBE8 buffer.
 */
u8* func_800B0248(u8* a0, u8 a1, u8* a2) {
    u8 *buf = D_800EEBE8;
    s32 offset = func_800B0204(buf, a0, 0, a1);
    func_800B0204(buf + offset, a2, offset, 0);
    return buf;
}