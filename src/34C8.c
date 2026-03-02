#include "common.h"

/** @brief Initializes the SPU (Sound Processing Unit).
 *
 *  Wrapper that calls func_80014C30 (which runs the full SPU init sequence:
 *  SpuStart, SpuInitMalloc, transfer mode setup, IRQ, and root counter timer)
 *  and returns 0.
 */
s32 func_80012CC8(void) {
    func_80014C30();
    return 0;
}

/** @brief Shuts down the SPU and cleans up sound resources.
 *
 *  Wrapper that calls func_80014D20 (which stops the root counter timer,
 *  disables/closes the timer event, resets the SPU IRQ address to 0xFFFFFF,
 *  and reinitializes the SPU) and returns 0.
 */
s32 func_80012CE8(void) {
    func_80014D20();
    return 0;
}

/**
 * @brief Parses a sound bank header and sets up audio data pointers.
 *
 * Calls func_80014740 to validate/process the sound bank at @p a0.
 * On success, partitions the memory after the 16-byte header into three
 * regions: D_80074ED0 (0x400 bytes), D_80074ED8 (0x200 bytes), and
 * D_80074EDC (remainder).
 *
 * @param a0 Pointer to the sound bank data loaded from CD.
 * @return 0 on success, non-zero error code from func_80014740 on failure.
 */
s32 func_80012D08(u8 *a0) {
    extern u8 *D_80074ED0;
    extern u8 *D_80074ED8;
    extern u8 *D_80074EDC;
    s32 result = func_80014740(a0);
    if (result == 0) {
        a0 += 0x10;
        D_80074ED0 = (u8 *)a0;
        a0 += 0x400;
        D_80074ED8 = (u8 *)a0;
        a0 += 0x200;
        D_80074EDC = (u8 *)a0;
    }
    return result;
}

/**
 * @brief Stops all sound engine activity and resets all 24 SPU voices.
 *
 * Disables the SPU IRQ event, clears the IRQ callback, optionally sends a
 * stop command if D_80074ED4 indicates playback is active, then iterates
 * over all 24 SPU voices setting their release mode via func_80014FCC.
 * Finally resets the IRQ address and re-enables the event.
 *
 * @return Always returns 0.
 */
s32 func_80012D5C(void) {
    extern s32 D_8005169C;
    extern s32 D_80074ED4;
    extern u8 D_800516B8[];
    s32 i;

    DisableEvent(D_8005169C);
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);
    if (D_80074ED4 == 1) {
        func_800147A8((s32)D_800516B8, 0x40);
        func_80014834();
    }
    for (i = 0; i < 24; i++) {
        func_80014FCC(i, 5, 3);
    }
    func_80014DE0(0xFFFFFF);
    func_80014974();
    EnableEvent(D_8005169C);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80012E04);

extern s32 D_80075058;
extern s32 *D_80074F08;
extern s32 D_80075028[];
extern s32 D_80077288[];
extern s32 D_80074EB8[];
extern s32 D_80077298[];


INCLUDE_ASM("asm/nonmatchings/34C8", func_80012E40);

/** @brief Sends SPU command 0x10 with parameter @p a0 via the command buffer. */
void func_80012EAC(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x10);
}

/** @brief Sends SPU command 0x11 with parameter @p a0 via the command buffer. */
void func_80012ED4(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x11);
}

/**
 * @brief Sends SPU command 0x14 with three parameters via the command buffer.
 * @param a0 First command parameter (stored at D_80075058[0]).
 * @param a1 Second command parameter (stored at D_80075058[1]).
 * @param a2 Third command parameter (stored at D_80075058[2]).
 */
void func_80012EFC(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2;
    func_8001A1E8(0x14);
}

/** @brief Sends SPU command 0x40. */
void func_80012F30(void) {
    func_8001A1E8(0x40);
}

/** @brief Stores a0 and masked a1 to SPU command buffer, sends command 0x19.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 7 bits.
 */
void func_80012F50(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0x19);
}

/** @brief Stores a0, a1, and a2 (masked to 7 bits) to SPU command buffer, sends command 0x1A.
 *  @param a0 First command parameter (voice/channel).
 *  @param a1 Second command parameter.
 *  @param a2 Third command parameter (masked to 0x7F).
 */
void func_80012F84(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2 & 0x7F;
    func_8001A1E8(0x1A);
}

/** @brief Stores a0 and a1 to SPU command buffer, sends command 0x12.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 */
void func_80012FBC(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    func_8001A1E8(0x12);
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80012FEC);

/**
 * @brief Sends SPU command 0x20 (play sound effect) with voice and playback parameters.
 * @param a0 Voice index, masked to 10 bits (0-1023).
 * @param a1 SPU address/offset, masked to 24 bits.
 * @param a2 Volume or envelope parameter, masked to 8 bits.
 * @param a3 Pan or pitch parameter, masked to 7 bits (0-127).
 */
void func_8001302C(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0 & 0x3FF;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0x20);
}

/**
 * @brief Validates a sound bank address and sends SPU command 0x24 to play from it.
 *
 * Calls func_80014740 to validate @p a0 first. On failure, returns the error.
 * On success, writes playback parameters to the command buffer and dispatches
 * command 0x24.
 *
 * @param a0 Sound bank address to validate and play.
 * @param a1 SPU address/offset, masked to 24 bits.
 * @param a2 Volume or envelope parameter, masked to 8 bits.
 * @param a3 Pan or pitch parameter, masked to 7 bits (0-127).
 * @return @p a0 on success, non-zero error code on validation failure.
 */
s32 func_8001307C(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = func_80014740(a0);
    if (result != 0) {
        return result;
    }
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0x24);
    return a0;
}

/**
 * @brief Sends SPU command 0x21 with an address and a 24-bit parameter.
 * @param a0 First parameter (stored at D_80075058[0]).
 * @param a1 Second parameter, masked to 24 bits (stored at D_80075058[1]).
 */
void func_80013100(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    func_8001A1E8(0x21);
}

/**
 * @brief Sends SPU command 0x30 (key on) with a 10-bit voice bitmask.
 * @param a0 Voice bitmask, masked to 10 bits.
 */
void func_8001313C(s32 a0) {
    D_80075058 = a0 & 0x3FF;
    func_8001A1E8(0x30);
}

/**
 * @brief Sends SPU command 0x44 (stop/reset playback).
 * @note TODO: maspsx schedules addiu $sp into load delay slot instead of jr delay slot.
 */
void func_80013168(void) {
    func_8001A1E8(0x44);
}

/** @brief Sends SPU command 0x45. */
void func_80013188(void) {
    func_8001A1E8(0x45);
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_800131A8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80013210);

/** @brief Sends SPU command 0x81 if a0 == 1, else 0x80.
 *  @param a0 Selects command (1 = 0x81, else 0x80).
 */
void func_8001327C(s32 a0) {
    if (a0 == 1) {
        func_8001A1E8(0x81);
    } else {
        func_8001A1E8(0x80);
    }
}

/** @brief Stores a0 to SPU command buffer, sends command 0x90.
 *  @param a0 Command parameter.
 */
void func_800132B0(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x90);
}

/** @brief Stores a0 to SPU command buffer, sends command 0x92.
 *  @param a0 Command parameter.
 */
void func_800132D8(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x92);
}

/**
 * @brief Selects and sends a reverb mode command based on a mode index.
 *
 * Maps mode index to SPU command codes:
 *   1 -> 0x9B, 2 -> 0x9D, 3 -> 0x9F, default -> 0x99.
 *
 * @param a0 Reverb mode selector (1-3 for specific modes, other for default).
 */
void func_80013300(u32 a0) {
    s32 val;
    switch (a0) {
        case 1: val = 0x9B; break;
        case 2: val = 0x9D; break;
        case 3: val = 0x9F; break;
        default: val = 0x99; break;
    }
    func_8001A1E8(val);
}

/** @brief Sends SPU command based on mode selector (1→0x9A, 2→0x9C, 3→0x9E, default→0x98).
 *  @param a0 Mode selector.
 */
void func_8001336C(u32 a0) {
    s32 val;
    switch (a0) {
        case 1: val = 0x9A; break;
        case 2: val = 0x9C; break;
        case 3: val = 0x9E; break;
        default: val = 0x98; break;
    }
    func_8001A1E8(val);
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_800133D8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001340C);

/**
 * @brief Sends SPU command 0xA8 with a 7-bit volume parameter.
 * @param a0 Volume value, masked to 7 bits (0-127).
 */
void func_8001344C(s32 a0) {
    D_80075058 = a0 & 0x7F;
    func_8001A1E8(0xA8);
}

/** @brief Stores a0 and 7-bit masked a1 to SPU command buffer, sends command 0xA9.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 7 bits.
 */
void func_80013478(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0xA9);
}

/** @brief Stores a0, 24-bit masked a1, and 7-bit masked a2 to SPU command buffer, sends command 0xA0.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third parameter, masked to 7 bits.
 */
void func_800134AC(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0x7F;
    func_8001A1E8(0xA0);
}

/** @brief Stores a0, 24-bit masked a1, a2, and 7-bit masked a3 to SPU command buffer, sends command 0xA1.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third command parameter.
 *  @param a3 Fourth parameter, masked to 7 bits.
 */
void func_800134F0(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0xA1);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xAA.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void func_80013538(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xAA);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xAB.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void func_80013564(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xAB);
}

/** @brief Stores a0, 24-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xA2.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void func_80013598(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xA2);
}

/** @brief Stores a0, 24-bit masked a1, a2, and 8-bit masked a3 to SPU command buffer, sends command 0xA3.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third command parameter.
 *  @param a3 Fourth parameter, masked to 8 bits.
 */
void func_800135DC(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2;
    *(&D_80075058 + 3) = a3 & 0xFF;
    func_8001A1E8(0xA3);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xAC.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void func_80013624(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xAC);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xAD.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void func_80013650(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xAD);
}

/** @brief Stores a0, 24-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xA4.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void func_80013684(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xA4);
}

/** @brief Stores a0, 24-bit masked a1, a2, and 8-bit masked a3 to SPU command buffer, sends command 0xA5.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third command parameter.
 *  @param a3 Fourth parameter, masked to 8 bits.
 */
void func_800136C8(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2;
    *(&D_80075058 + 3) = a3 & 0xFF;
    func_8001A1E8(0xA5);
}

/**
 * @brief Sends SPU command 0xC0 with a channel index and 7-bit parameter.
 * @param a0 Channel or voice identifier.
 * @param a1 Parameter value, masked to 7 bits (0-127).
 */
void func_80013710(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0xC0);
}

/** @brief Stores a0, a1, and 7-bit masked a2 to SPU command buffer, sends command 0xC1.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 *  @param a2 Third parameter, masked to 7 bits.
 */
void func_80013744(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2 & 0x7F;
    func_8001A1E8(0xC1);
}

/** @brief Stores a0, a1, 7-bit masked a2, and 7-bit masked a3 to SPU command buffer, sends command 0xC2.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 *  @param a2 Third parameter, masked to 7 bits.
 *  @param a3 Fourth parameter, masked to 7 bits.
 */
void func_8001377C(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2 & 0x7F;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0xC2);
}

/** @brief Stores a0 to SPU command buffer, sends command 0xC8.
 *  @param a0 Command parameter.
 */
void func_800137BC(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0xC8);
}

/** @brief Stores a0 and a1 to SPU command buffer, sends command 0xC9.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 */
void func_800137E4(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    func_8001A1E8(0xC9);
}

/** @brief Stores a0, a1 and a2 to SPU command buffer, sends command 0xCA.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 *  @param a2 Third command parameter.
 */
void func_80013814(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2;
    func_8001A1E8(0xCA);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xD0.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void func_80013848(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xD0);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xD1.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void func_80013874(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xD1);
}

/** @brief Stores a0, 8-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xD2.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void func_800138A8(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xD2);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xD4.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void func_800138E4(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xD4);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xD5.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void func_80013910(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xD5);
}

/** @brief Stores a0, 8-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xD6.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void func_80013944(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xD6);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xD8.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void func_80013980(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xD8);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xD9.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void func_800139AC(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xD9);
}

/** @brief Stores a0, 8-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xDA.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void func_800139E0(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xDA);
}

/** @brief Sends SPU command 0xF0. */
void func_80013A1C(void) {
    func_8001A1E8(0xF0);
}

/** @brief Sends SPU command 0xF1 (system/engine reset or flush). */
void func_80013A3C(void) {
    func_8001A1E8(0xF1);
}

/**
 * @brief Uploads sample data to SPU RAM, spinning until the transfer completes.
 *
 * Called during sound engine init to upload sound bank samples from main RAM
 * to SPU RAM. Blocks in a busy-wait loop until func_80014860 indicates the
 * transfer is finished.
 *
 * @param a0 Source address of sample data in main RAM.
 * @param a1 Transfer mode or bank index.
 */
void func_80013A5C(s32 a0, s32 a1) {
    do {
    } while (func_80014860(a0, a1) == 1);
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80013AA8);

/** @brief Returns the value of global D_80074ED4. */
s32 func_80013CA4(void) {
    extern s32 D_80074ED4;
    return D_80074ED4;
}

/** @brief Clears D_8007735C, sets bit 0x1 in D_80077288, returns 0. */
s32 func_80013CB4(void) {
    extern s32 D_8007735C;
    D_8007735C = 0;
    D_80077288[0] |= 0x1;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80013CD4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80013EE4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80013F38);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014034);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014094);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014190);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014250);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800142E4);

/** @brief Sends SPU command 0xE2. */
void func_80014348(void) {
    func_8001A1E8(0xE2);
}

/** @brief Stores 7-bit masked a0, shifted left 8, to SPU command buffer. Sends command 0xE4.
 *  @param a0 Parameter, masked to 7 bits then shifted left 8.
 */
void func_80014368(s32 a0) {
    D_80075058 = (a0 & 0x7F) << 8;
    func_8001A1E8(0xE4);
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014398);

/** @brief Stores u8-masked a0, shifted left 8, to SPU command buffer. Sends command 0xE6.
 *  @param a0 Parameter, masked to 8 bits then shifted left 8.
 */
void func_800143D0(s32 a0) {
    D_80075058 = (u8)a0 << 8;
    func_8001A1E8(0xE6);
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014400);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800144E4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001451C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800145E0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001466C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800146F0);

/** @brief Adds magic constant 0xB0BEB4BF to value at pointer. */
u32 func_80014740(u32 *a0) {
    return *a0 + 0xB0BEB4BF;
}

/** @brief Calls func_8003E494(0) and clears D_80074ED4. */
void func_80014754(void) {
    extern s32 D_80074ED4;
    func_8003E494(0);
    D_80074ED4 = 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014778);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800147A8);

/** @brief Calls func_80014778, then SpuRead with a0/a1 as arguments.
 *  @param a0 SPU read address.
 *  @param a1 SPU read size.
 */
void func_800147F8(s32 a0, s32 a1) {
    func_80014778();
    SpuRead(a0, a1);
}

/**
 * @brief Busy-waits until SPU DMA transfer completes.
 *
 * Spins on volatile flag D_80074ED4 until it is no longer 1, indicating
 * the active SPU transfer has finished.
 */
void func_80014834(void) {
    extern volatile s32 D_80074ED4;
    while (D_80074ED4 == 1) {}
}

/**
 * @brief Initiates a single SPU sample upload step.
 *
 * Validates @p a0 via func_80014740, then reads the sample's transfer
 * size (offset 0x18) and start address (offset 0x10) to call func_800148B0,
 * which performs the actual DMA transfer.
 *
 * @param a0 Pointer to a sound bank entry or sample descriptor.
 * @param a1 Transfer mode or bank index.
 * @return 0 on success, -1 if validation of @p a0 fails.
 */
s32 func_80014860(s32 a0, s32 a1) {
    if (func_80014740(a0) != 0) {
        return -1;
    }
    func_800148B0(a0, a1, *(s32 *)(a0 + 0x18), *(s32 *)(a0 + 0x10));
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_800148B0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014974);

/**
 * @brief Initializes the SPU hardware and sets up the sound engine timer.
 *
 * Performs the full SPU startup sequence:
 *  1. Calls SpuStart() to initialize the SPU subsystem.
 *  2. Configures SPU malloc with 4 allocation slots.
 *  3. Sets transfer mode to manual and start address to 0x1010.
 *  4. Sends an initial command buffer and waits for DMA completion.
 *  5. Resets voice state via func_80014974.
 *  6. Disables SPU IRQ.
 *  7. Sets up root counter 2 (RCnt2, 0xF2000002) as a timer at interval 0x44E8.
 *  8. Opens and enables an event on the root counter for the sound engine tick
 *     callback (D_8001AD60).
 */
void func_80014C30(void) {
    extern s32 D_80074E88;
    extern u8 D_800516B8[];
    extern s32 D_8001AD60;
    extern s32 D_8005169C;

    SpuStart();
    SpuInitMalloc(4, (s32)&D_80074E88);
    SpuSetTransferMode(0);
    SpuSetTransferStartAddr(0x1010);
    func_800147A8((s32)D_800516B8, 0x40);
    func_80014834();
    func_80014974();
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);

    do {
    } while (SetRCnt(0xF2000002, 0x44E8, 0x1000) == 0);

    do {
    } while (StartRCnt(0xF2000002) == 0);

    do {
        D_8005169C = OpenEvent(0xF2000002, 2, 0x1000, (s32)&D_8001AD60);
    } while (D_8005169C == -1);

    do {
    } while (EnableEvent(D_8005169C) == 0);
}

/**
 * @brief Shuts down the sound engine and releases SPU resources.
 *
 * If playback is active (D_80074ED4 == 1), sends a stop command and waits
 * for DMA completion. Stops root counter 2, undelivers and disables/closes
 * the associated event, resets all voice IRQ addresses, and calls
 * func_8003DE44 (SpuQuit) for final SPU cleanup.
 */
void func_80014D20(void) {
    extern s32 D_80074ED4;
    extern u8 D_800516B8[];
    extern s32 D_8005169C;

    if (D_80074ED4 == 1) {
        func_800147A8((s32)D_800516B8, 0x40);
        func_80014834();
    }

    do {
    } while (StopRCnt(0xF2000002) == 0);

    UnDeliverEvent(0xF2000002, 2);

    do {
    } while (DisableEvent(D_8005169C) == 0);

    do {
    } while (CloseEvent(D_8005169C) == 0);

    func_80014DE0(0xFFFFFF);
    func_8003DE44();
}

/**
 * @brief Writes the reverb work area start address to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D88 and high 16 bits to 0x1F801D8A.
 * (SPU register: Reverb Work Area Start Address)
 *
 * @param val 32-bit reverb work area start address.
 */
void func_80014DC4(u32 val) {
    *(s16 *)0x1F801D88 = val;
    *(s16 *)0x1F801D8A = val >> 16;
}

/**
 * @brief Writes the SPU IRQ address to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D8C and high 16 bits to 0x1F801D8E.
 * (SPU register: Sound RAM IRQ Address)
 *
 * @param val 32-bit SPU IRQ address.
 */
void func_80014DE0(u32 val) {
    *(s16 *)0x1F801D8C = val;
    *(s16 *)0x1F801D8E = val >> 16;
}

/**
 * @brief Writes the SPU transfer control value to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D98 and high 16 bits to 0x1F801D9A.
 * (SPU register: Sound RAM Data Transfer Control)
 *
 * @param val 32-bit transfer control value.
 */
void func_80014DFC(u32 val) {
    *(s16 *)0x1F801D98 = val;
    *(s16 *)0x1F801D9A = val >> 16;
}

/**
 * @brief Writes a value to the SPU data FIFO register.
 *
 * Writes low 16 bits to 0x1F801D94 and high 16 bits to 0x1F801D96.
 * (SPU register: Sound RAM Data Transfer FIFO)
 *
 * @param val 32-bit value to push into the SPU data FIFO.
 */
void func_80014E18(u32 val) {
    *(s16 *)0x1F801D94 = val;
    *(s16 *)0x1F801D96 = val >> 16;
}

/**
 * @brief Writes the SPU transfer start address to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D90 and high 16 bits to 0x1F801D92.
 * (SPU register: Sound RAM Data Transfer Address)
 *
 * @param val 32-bit SPU RAM transfer start address.
 */
void func_80014E34(u32 val) {
    *(s16 *)0x1F801D90 = val;
    *(s16 *)0x1F801D92 = val >> 16;
}

/**
 * @brief Sets the left and right volume for an SPU voice, with optional scaling.
 *
 * If @p scale is non-zero, both volumes are multiplied by @p scale and
 * divided by 128. The resulting volumes are masked to 15 bits and written
 * to the voice's volume registers at 0x1F801C00 + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param vol_l Left volume (0-0x7FFF).
 * @param vol_r Right volume (0-0x7FFF).
 * @param scale Volume scale factor (0 = no scaling, otherwise vol * scale / 128).
 */
void func_80014E50(s32 voice, s32 vol_l, s32 vol_r, s32 scale) {
    s16 *p;
    if (scale != 0) {
        vol_l *= scale;
        vol_l = (u32)vol_l >> 7;
        vol_r *= scale;
        vol_r = (u32)vol_r >> 7;
    }
    p = (s16 *)(0x1F801C00 + voice * 16);
    p[0] = vol_l & 0x7FFF;
    p[1] = vol_r & 0x7FFF;
}

/**
 * @brief Sets the sample rate (pitch) for an SPU voice.
 *
 * Writes to voice register offset +0x04 at 0x1F801C04 + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param val Sample rate value (0x1000 = 44100 Hz).
 */
void func_80014E98(s32 voice, s32 val) {
    *(s16 *)(0x1F801C04 + voice * 16) = val;
}

/**
 * @brief Sets the ADPCM sample start address for an SPU voice.
 *
 * The address is right-shifted by 3 (divided by 8) before writing to
 * voice register offset +0x06 at 0x1F801C06 + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param val SPU RAM byte address of the ADPCM sample start.
 */
void func_80014EB0(s32 voice, u32 val) {
    *(s16 *)(0x1F801C06 + voice * 16) = val >> 3;
}

/**
 * @brief Sets the ADPCM sample loop/repeat address for an SPU voice.
 *
 * The address is right-shifted by 3 (divided by 8) before writing to
 * voice register offset +0x0E at 0x1F801C0E + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param val SPU RAM byte address of the ADPCM loop point.
 */
void func_80014ECC(s32 voice, u32 val) {
    *(s16 *)(0x1F801C0E + voice * 16) = val >> 3;
}

/**
 * @brief Sets the ADSR low halfword for an SPU voice.
 *
 * Writes the full 16-bit ADSR low value to voice register offset +0x08
 * at 0x1F801C08 + voice * 16. Contains Attack Mode, Attack Shift, Attack
 * Step, Decay Shift, and Sustain Level fields.
 *
 * @param voice SPU voice index (0-23).
 * @param val 16-bit ADSR low value.
 */
void func_80014EE8(s32 voice, s32 val) {
    *(s16 *)(0x1F801C08 + voice * 16) = val;
}

/**
 * @brief Sets the ADSR high halfword for an SPU voice.
 *
 * Writes the full 16-bit ADSR high value to voice register offset +0x0A
 * at 0x1F801C0A + voice * 16. Contains Sustain and Release parameters.
 *
 * @param voice SPU voice index (0-23).
 * @param val 16-bit ADSR high value.
 */
void func_80014F00(s32 voice, s32 val) {
    *(s16 *)(0x1F801C0A + voice * 16) = val;
}

/**
 * @brief Sets the attack mode and attack shift fields in ADSR low for an SPU voice.
 *
 * Reads the current ADSR low register value, ORs in the attack mode bit
 * (bit 15, from @p a2 >> 2) and attack shift (bits [12:8], from @p a1),
 * then writes back.
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Attack shift value (written to bits [12:8]).
 * @param a2 Attack mode (bit 2 selects exponential mode in bit 15).
 */
void func_80014F18(s32 voice, u32 a1, u32 a2) {
    u8 *addr = (u8 *)(0x1F801C08 + voice * 16);
    s32 val = *addr;
    val |= ((a2 >> 2) << 15) | (a1 << 8);
    *(s16 *)addr = val;
}

/**
 * @brief Sets the decay shift field in ADSR low for an SPU voice.
 *
 * Reads the current ADSR low register, clears bits [7:4], and writes
 * @p a1 into that field. (ADSR low register offset +0x08)
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Decay shift value (4 bits, written to bits [7:4]).
 */
void func_80014F48(s32 voice, u32 a1) {
    s16 *addr = (s16 *)(0x1F801C08 + voice * 16);
    s32 val = *(u16 *)addr;
    val = (val & 0xFF0F) | (a1 << 4);
    *addr = val;
}

/**
 * @brief Sets the sustain level field in ADSR low for an SPU voice.
 *
 * Reads the current ADSR low register, clears bits [3:0], and writes
 * @p a1 into that field. (ADSR low register offset +0x08)
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Sustain level value (4 bits, written to bits [3:0]).
 */
void func_80014F70(s32 voice, u32 a1) {
    s16 *addr = (s16 *)(0x1F801C08 + voice * 16);
    s32 val = *(u16 *)addr;
    val = (val & 0xFFF0) | a1;
    *addr = val;
}

/**
 * @brief Sets the sustain mode/shift and direction fields in ADSR high for an SPU voice.
 *
 * Reads the current ADSR high register, preserves the low 6 bits (release),
 * and packs @p a2 (sustain direction, bit 14 via >>1) and @p a1 (sustain
 * shift, bits [12:6]) into the upper bits.
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Sustain shift value (written to bits [12:6]).
 * @param a2 Sustain direction (bit 1 selects decrease mode in bit 14).
 */
void func_80014F98(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 1) << 14) | (a1 << 6);
    s32 val = *(u16 *)addr;
    val = (val & 0x3F) | packed;
    *addr = val;
}

/**
 * @brief Sets the release mode and release shift fields in ADSR high for an SPU voice.
 *
 * Reads the current ADSR high register, preserves the upper 10 bits
 * (sustain fields), and packs @p a2 (release mode, bit 5 via >>2) and
 * @p a1 (release shift, bits [4:0]) into the low 6 bits.
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Release shift value (5 bits, written to bits [4:0]).
 * @param a2 Release mode (bit 2 selects exponential mode in bit 5).
 */
void func_80014FCC(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 2) << 5) | a1;
    s32 val = *(u16 *)addr;
    val = (val & 0xFFC0) | packed;
    *addr = val;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80014FFC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800150A8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800151C4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80015668);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80015AC8);

/**
 * @brief Finds the SPU voice with the lowest priority and frees it for reuse.
 *
 * Scans all 24 voice entries in the D_80073CB0 array (stride 8, priority
 * at offset +4) to find the voice with the lowest priority value. If found,
 * calls func_80016280 to release it from all active sequences.
 *
 * @return The index of the freed voice, or 0x18 (24) if no voice is available.
 */
s32 func_80015F4C(void) {
    extern u8 D_80073CB0[];
    u32 i;
    u16 min_val;
    u32 min_idx;
    u8 *p;

    i = 0;
    min_val = 0x7FFF;
    min_idx = 0x18;
    p = D_80073CB0;

    do {
        if (*(s16 *)(p + 4) < (s16)min_val) {
            min_val = *(u16 *)(p + 4);
            min_idx = i;
        }
        i++;
        p += 8;
    } while (i < 0x18);

    if ((s16)min_val == 0x7FFF) {
        return 0x18;
    }
    if (min_idx < 0x18) {
        func_80016280(min_idx);
    }
    return min_idx;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80015FE0);

extern u8 D_80070D60[];
extern s32 D_80073CA8;
extern u8 *D_80073C34;

/**
 * @brief Releases a voice from all active sequence tracks.
 *
 * Iterates over 32 track entries in the D_80070D60 array (stride 0x110,
 * voice assignment at offset +0xF4). Any track assigned to voice @p a0
 * is reassigned to voice 24 (invalid/none). Also checks the secondary
 * sequence array D_80073C34 if D_80073CA8 is set.
 *
 * @param a0 SPU voice index to release (0-23).
 */
void func_80016280(s32 a0) {
    s32 base = (s32)D_80070D60;
    s32 i = 32;
    s32 replacement = 24;
    s32 ptr = base + 0xF4;
    do {
        if (a0 == *(s32 *)ptr) {
            *(s32 *)ptr = replacement;
        }
        i--;
        ptr += 0x110;
    } while (i != 0);
    if (D_80073CA8 != 0) {
        base = (s32)D_80073C34;
        i = 32;
        replacement = 24;
        ptr = base + 0xF4;
        do {
            if (a0 == *(s32 *)ptr) {
                *(s32 *)ptr = replacement;
            }
            i--;
            ptr += 0x110;
        } while (i != 0);
    }
}

/**
 * @brief Checks if a voice is assigned to any active track matching a bitmask.
 *
 * Iterates over 32 track entries (stride 0x110). For each track whose voice
 * assignment (offset +0xF4) equals @p a2, checks whether the corresponding
 * bit in @p a1 is set.
 *
 * @param a0 Base pointer to the sequence track array.
 * @param a1 Bitmask of tracks to check (bit N = track N).
 * @param a2 SPU voice index to search for.
 * @return 1 if any matching track has its bit set in @p a1, 0 otherwise.
 */
s32 func_80016300(u8 *a0, s32 a1, s32 a2) {
    u32 i = 0;
    s32 bit = 1;
    do {
        if (a2 == *(s32 *)(a0 + 0xF4)) {
            if (a1 & (bit << i)) {
                return 1;
            }
        }
        i++;
        a0 += 0x110;
    } while (i < 0x20);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016344);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016478);

extern s32 D_80074F1C;
/**
 * @brief Stores the current global tempo/timing value into a track structure.
 *
 * Copies D_80074F1C (a global timing or tick counter) into the track
 * structure field at offset 0x50.
 *
 * @param a0 Pointer to a sequence track structure.
 */
void func_800164C8(u8 *a0) {
    *(s32 *)(a0 + 0x50) = D_80074F1C;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_800164D8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016A1C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016A7C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016C28);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016DB4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016E08);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80016E5C);

/**
 * @brief Determines which sound bank group a given identifier belongs to.
 *
 * Compares @p a0 against entries in D_80074EB8 (a table of sound bank IDs).
 * Indices 1 and 4 map to group 1; indices 2 and 5 map to group 2.
 *
 * @param a0 Sound bank or instrument identifier to look up.
 * @return 1 if in group 1, 2 if in group 2, 0 if not found or @p a0 is 0.
 */
s32 func_80016F3C(s32 a0) {
    s32 ret = 0;
    if (a0 == 0) return ret;
    if (a0 == D_80074EB8[1] || a0 == D_80074EB8[4]) {
        ret = 1;
    } else if (a0 == D_80074EB8[2] || a0 == D_80074EB8[5]) {
        ret = 2;
    }
    return ret;
}

/**
 * @brief Initializes a sequence track structure to default values.
 *
 * Sets the track's instrument bank ID at offset +0x00, initializes
 * default pitch bend (0x6E00 at +0x80), default tempo (0x32000000 at +0x44),
 * sentinel voice value (0xFFFF at +0x6E), and zeroes all modulation,
 * panning, volume, and effect fields. Finally applies instrument table
 * entry 0 via func_8001C1DC.
 *
 * @param a0 Pointer to the track structure (stride 0x110).
 * @param a1 Instrument bank ID to assign to this track.
 */
void func_80016FA8(void *a0, s32 a1) {
    *(s16*)((u8*)a0 + 0x80) = 0x6E00;
    *(s32*)((u8*)a0 + 0x44) = 0x32000000;
    *(s32*)a0 = a1;
    *(s16*)((u8*)a0 + 0xE4) = 0;
    *(s16*)((u8*)a0 + 0xE6) = 0;
    *(s16*)((u8*)a0 + 0x98) = 0;
    *(s32*)((u8*)a0 + 0x2C) = 0;
    *(s16*)((u8*)a0 + 0xEA) = 0;
    *(s16*)((u8*)a0 + 0x90) = 0;
    *(s16*)((u8*)a0 + 0xD8) = 0;
    *(s16*)((u8*)a0 + 0xD6) = 0;
    *(s16*)((u8*)a0 + 0x86) = 0;
    *(s32*)((u8*)a0 + 0x50) = 0;
    *(s16*)((u8*)a0 + 0xD4) = 0;
    *(s32*)((u8*)a0 + 0x30) = 0;
    *(s16*)((u8*)a0 + 0xF2) = 0;
    *(s16*)((u8*)a0 + 0x9A) = 0;
    *(u16*)((u8*)a0 + 0x6E) = 0xFFFF;
    *(s16*)((u8*)a0 + 0x10A) = 0;
    *(s16*)((u8*)a0 + 0xCA) = 0;
    *(s16*)((u8*)a0 + 0xBC) = 0;
    *(s16*)((u8*)a0 + 0xAA) = 0;
    *(s16*)((u8*)a0 + 0xCC) = 0;
    *(s16*)((u8*)a0 + 0xBE) = 0;
    *(s16*)((u8*)a0 + 0xAC) = 0;
    *(s16*)((u8*)a0 + 0xD2) = 0;
    *(s16*)((u8*)a0 + 0xD0) = 0;
    func_8001C1DC(a0, 0);
}

/**
 * @brief Builds a voice bitmask from a track bitmask.
 *
 * Iterates over 32 track entries (stride 0x110). For each track whose bit
 * is set in @p a1, reads the assigned voice index at offset +0xF4. If the
 * voice index is valid (< 24), sets the corresponding bit in the result.
 *
 * @param a0 Base pointer to the sequence track array.
 * @param a1 Bitmask of tracks to include (bit N = track N).
 * @return Bitmask of SPU voices used by the selected tracks.
 */
s32 func_80017040(u8 *a0, s32 a1) {
    u32 i = 0;
    s32 result = 0;
    s32 bit = 1;
    do {
        if (a1 & (bit << i)) {
            s32 val = *(s32 *)(a0 + 0xF4);
            if ((u32)val < 0x18) {
                result |= (bit << val);
            }
        }
        i++;
        a0 += 0x110;
    } while (i < 0x20);
    return result;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001708C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017410);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800174E4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017880);

/**
 * @brief Releases a voice from all active sequence tracks (duplicate of func_80016280).
 *
 * Iterates over 32 track entries in D_80070D60 (stride 0x110, voice at +0xF4).
 * Any track assigned to voice @p a0 is reassigned to 24 (none). Also checks
 * the secondary sequence array D_80073C34 if D_80073CA8 is set.
 *
 * @param a0 SPU voice index to release (0-23).
 */
void func_80017A2C(s32 a0) {
    s32 base = (s32)D_80070D60;
    s32 i = 32;
    s32 replacement = 24;
    s32 ptr = base + 0xF4;
    do {
        if (*(s32 *)ptr == a0) {
            *(s32 *)ptr = replacement;
        }
        i--;
        ptr += 0x110;
    } while (i != 0);
    if (D_80073CA8 != 0) {
        base = (s32)D_80073C34;
        i = 32;
        replacement = 24;
        ptr = base + 0xF4;
        do {
            if (*(s32 *)ptr == a0) {
                *(s32 *)ptr = replacement;
            }
            i--;
            ptr += 0x110;
        } while (i != 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017AAC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017C9C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017D14);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017D5C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80017DB0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018158);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800181D4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018234);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018358);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018438);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018610);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018650);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001869C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001871C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018784);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800188B4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800188E0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018908);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018A74);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018B28);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018C48);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018D40);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018D74);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018DDC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018E4C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80018F34);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800190B4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019130);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800191F8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800192D8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019450);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800194C8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001958C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001966C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_800197F4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001984C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019918);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019934);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001999C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019A10);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019A2C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019A94);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019B08);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019B58);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019BC0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019C9C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019CFC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019D5C);

/** @brief Copies halfword from a0 to offset 0x60 in struct pointed to by D_80074F08. */
void func_80019D9C(u16 *a0) {
    *(u16 *)((u8 *)D_80074F08 + 0x60) = *a0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019DB0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019EA0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_80019F3C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A058);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A0EC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A13C);

/** @brief Empty stub — no operation. */
void func_8001A190(void) {
}

/**
 * @brief Configures the SPU reverb mode if it differs from the current setting.
 *
 * Queries the current reverb mode via func_8003ED54. If the current mode
 * differs from @p a0, disables reverb, sets the new mode type (with 0x100
 * flag to apply immediately), and re-enables reverb.
 *
 * @param a0 Reverb mode type to set (e.g. off, room, hall, etc.).
 */
void func_8001A198(s32 a0) {
    s32 current;
    func_8003ED54(&current);
    if (current != a0) {
        SpuSetReverb(0);
        SpuSetReverbModeType(a0 | 0x100);
        SpuSetReverb(1);
    }
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A1E8);

extern s16 D_8007507A;
/**
 * @brief Sets the CD audio input volume on the SPU to the current global value.
 *
 * Reads the volume level from D_8007507A and writes it to both the left
 * (0x1F801DB0) and right (0x1F801DB2) CD audio volume registers, producing
 * equal stereo volume.
 */
void func_8001A55C(void) {
    s32 val = D_8007507A;
    *(s16 *)0x1F801DB0 = val;
    *(s16 *)0x1F801DB2 = val;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A57C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A5FC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001A674);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001AA28);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001ACCC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001AD60);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001B1F4);

/**
 * @brief Adjusts a note/instrument value based on a flag and range check.
 *
 * If bit 10 of @p a0 is set and @p a1 is in range [0x40, 0x80), adds 0x20
 * to @p a1 (shifting the value into a higher register/octave).
 *
 * @param a0 Flags value; bit 10 enables the adjustment.
 * @param a1 Note or instrument index to potentially adjust.
 * @return Adjusted value, or @p a1 unchanged if conditions are not met.
 */
s32 func_8001B400(s32 a0, s32 a1) {
    if ((a0 & 0x400) && (u32)a1 >= 0x40 && (u32)a1 < 0x80) {
        return a1 + 0x20;
    }
    return a1;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001B42C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001B690);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001B820);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001BAA8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C1A8);

extern u8 D_80073E68[];

/**
 * @brief Applies an instrument table entry to a sequence track.
 *
 * Stores the instrument index at track offset +0x66, then looks up the
 * instrument in D_80073E68 (stride 16 bytes) and calls func_8001C1A8 to
 * copy the instrument parameters into the track structure.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Instrument table index to apply.
 */
void func_8001C1DC(u8 *a0, s32 a1) {
    *(s16 *)(a0 + 0x66) = a1;
    func_8001C1A8((s32)a0, (s32)(D_80073E68 + a1 * 16), *(s32 *)(D_80073E68 + a1 * 16));
}

/**
 * @brief Clears voice bits from multiple SPU control bitmasks and resets track state.
 *
 * Applies ~a1 mask to clear bits in D_80075028 entries [0], [1], [2], [4],
 * [7], [8], and [9] (key-on, reverb, noise, and other voice control masks).
 * Also zeroes the track's status fields at offsets +0x24 and +0x38.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to clear from all control registers.
 */
void func_8001C214(u8 *a0, s32 a1) {
    s32 mask = ~a1;
    D_80075028[0] &= mask;
    D_80075028[4] &= mask;
    D_80075028[7] &= mask;
    D_80075028[8] &= mask;
    D_80075028[9] &= mask;
    D_80075028[1] &= mask;
    D_80075028[2] &= mask;
    *(s32 *)(a0 + 0x24) = 0;
    *(s32 *)(a0 + 0x38) = 0;
}

/**
 * @brief Transposes a note value based on flag bits, for percussion mapping.
 *
 * If @p a1 falls in range [0x80, 0xB0) or [0xD0, 0x100), applies an offset
 * based on bits in @p a0: bit 1 adds +0x10, bit 2 adds +0x20.
 *
 * @note Purpose uncertain -- appears to select alternate percussion banks
 *       or drum kit variations based on channel flags.
 *
 * @param a0 Channel/track flags; bits 1-2 select the transposition amount.
 * @param a1 Note or percussion instrument index.
 * @return Transposed note value, or @p a1 unchanged if outside valid ranges.
 */
s32 func_8001C280(s32 a0, s32 a1) {
    if ((u32)(a1 - 0x80) < 0x30 || (u32)(a1 - 0xD0) < 0x30) {
        if (a0 & 2) {
            return a1 + 0x10;
        }
        if (a0 & 4) {
            return a1 + 0x20;
        }
    }
    return a1;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C2C8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C39C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C3E8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C490);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C530);

/** @brief Reads a signed 16-bit little-endian offset from the stream cursor and advances cursor by that offset.
 *  @param a0 Pointer to stream state (a0[0] = cursor pointer).
 */
void func_8001C5D8(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 offset = (s16)(ptr[0] | (ptr[1] << 8));
    *(u8 **)a0 = ptr + offset;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C604);

/** @brief Reads one byte from stream, advances cursor, ORs 3 into flags, stores byte << 8 as halfword at +0x80.
 *  @param a0 Pointer to stream state.
 */
void func_8001C65C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    u8 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s32 *)(a0 + 0xF8) |= 3;
    *(u16 *)(a0 + 0x80) = val << 8;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C684);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C708);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C738);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C7C4);

/** @brief Sets bit 6 (0x40) in the flags word at offset 0x30 of a0. */
void func_8001C854(u8 *a0) {
    *(u32 *)(a0 + 0x30) |= 0x40;
}

/** @brief Clears bit 6 (0x40) in the flags word at offset 0x30 of a0. */
void func_8001C868(u8 *a0) {
    *(u32 *)(a0 + 0x30) &= ~0x40;
}

/** @brief Increments the word at a0 by 2. */
void func_8001C87C(s32 *a0) {
    *a0 += 2;
}

/** @brief Empty stub — no operation. */
void func_8001C890(void) {
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C898);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C8DC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C968);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001C99C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CA28);

/** @brief Increments halfword at a0+0x92, wraps modulo 16. */
void func_8001CA44(u8 *a0) {
    *(u16 *)(a0 + 0x92) = (*(u16 *)(a0 + 0x92) + 1) & 0xF;
}

/** @brief Decrements halfword at a0+0x92, wraps modulo 16. */
void func_8001CA5C(u8 *a0) {
    *(u16 *)(a0 + 0x92) = (*(u16 *)(a0 + 0x92) - 1) & 0xF;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CA74);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CB1C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CBA4);

/** @brief Looks up D_80073E68 table by a0[0x66] index, copies fields to a0, ORs flags, masks a0[0x30].
 *  @param a0 Pointer to stream state.
 */
void func_8001CC30(u8 *a0) {
    extern u8 D_80073E68[];
    u8 *entry = D_80073E68 + *(u16 *)(a0 + 0x66) * 16;
    *(u16 *)(a0 + 0x106) = *(u16 *)(entry + 0xC);
    *(u16 *)(a0 + 0x108) = *(u16 *)(entry + 0xE);
    *(s32 *)(a0 + 0xF8) |= 0xFF00;
    *(s32 *)(a0 + 0x30) &= (s32)0xE6FFFFFF;
}

/** @brief Reads one byte from stream, sign-extends to s8, stores as halfword at a0+0xE4.
 *  @param a0 Pointer to stream state.
 */
void func_8001CC78(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s8 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(s16 *)(a0 + 0xE4) = val;
}

/** @brief Reads one byte from stream, sign-extends, adds to halfword at a0+0xE4.
 *  @param a0 Pointer to stream state.
 */
void func_8001CC9C(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s8 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xE4) = *(u16 *)(a0 + 0xE4) + val;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CCC8);

/** @brief Reads one byte from stream as duration. If zero, stores 0x100. Clears tempo/timer fields.
 *  @param a0 Pointer to stream state.
 */
void func_8001CD10(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0x98) = val;
    if (val != 0) goto skip;
    *(u16 *)(a0 + 0x98) = 0x100;
skip:
    *(u16 *)(a0 + 0xEC) = 0;
    *(u16 *)(a0 + 0x96) = 0;
    *(u16 *)(a0 + 0x9A) = 1;
}

/** @brief Clears the halfword at offset 0x98 of a0. */
void func_8001CD48(u8 *a0) {
    *(u16 *)(a0 + 0x98) = 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CD50);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CDB0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CE14);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CF0C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CF6C);

/** @brief Clears bit 0x1 in a0+0x30, sets bit 0x10 in a0+0xF8, zeroes a0+0xEE. */
void func_8001CFD8(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x1;
    *(s32 *)(a0 + 0xF8) |= 0x10;
    *(u16 *)(a0 + 0xEE) = 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001CFFC);

/** @brief Reads one byte from stream, advances cursor, masks to 7 bits, shifts left 8, stores to halfword at a0+0xBC.
 *  @param a0 Pointer to stream state.
 */
void func_8001D0AC(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xBC) = (val & 0x7F) << 8;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D0D0);

/** @brief Clears bit 0x2 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF0. */
void func_8001D140(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x2;
    *(s32 *)(a0 + 0xF8) |= 0x3;
    *(u16 *)(a0 + 0xF0) = 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D164);

/** @brief Reads one byte from stream, advances cursor, shifts left 7, stores to halfword at a0+0xCA.
 *  @param a0 Pointer to stream state.
 */
void func_8001D1D0(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    *(u16 *)(a0 + 0xCA) = val << 7;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D1F0);

/** @brief Clears bit 0x4 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF2. */
void func_8001D25C(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x4;
    *(s32 *)(a0 + 0xF8) |= 0x3;
    *(u16 *)(a0 + 0xF2) = 0;
}

/**
 * @brief Enables noise mode for the given voice bitmask on the appropriate SPU channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (D_80074F08[0x3C/4]); otherwise sets bits in D_80075028[0x1C/4].
 * Marks the SPU dirty flags (D_80077288) with 0x110 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable noise mode for.
 */
void func_8001D280(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] | a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
}

/**
 * @brief Disables noise mode for the given voice bitmask and clears track state.
 *
 * Clears the voice bits from the noise enable register (primary or secondary
 * depending on channel at offset +0x60). Marks SPU dirty flags with 0x110
 * and zeroes the track's noise state field at offset +0xD0.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to disable noise mode for.
 */
void func_8001D2E0(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] & ~a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
    *(u16 *)((s32)a0 + 0xD0) = 0;
}

/**
 * @brief Enables reverb for the given voice bitmask on the appropriate SPU channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (D_80074F08[0x44/4]); otherwise checks flag 0x10000 at offset +0x30
 * before setting bits in D_80075028[0x24/4]. Marks dirty flags with 0x100.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable reverb for.
 */
void func_8001D348(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] | a1;
    } else if (*(s32 *)((s32)a0 + 0x30) & 0x10000) {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

/**
 * @brief Disables reverb for the given voice bitmask and clears track state.
 *
 * Clears voice bits from the reverb enable register (primary or secondary
 * depending on channel at offset +0x60). Marks SPU dirty flags with 0x100
 * and zeroes the track's reverb state field at offset +0xD2.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to disable reverb for.
 */
void func_8001D3BC(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] & ~a1;
    } else {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
    *(u16 *)((s32)a0 + 0xD2) = 0;
}

/**
 * @brief Enables pitch modulation for the given voice bitmask on the appropriate channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (D_80074F08[0x40/4]); otherwise sets bits in D_80075028[0x20/4].
 * Marks SPU dirty flags with 0x100 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable pitch modulation for.
 */
void func_8001D424(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x40 / 4] = p[0x40 / 4] | a1;
    } else {
        D_80075028[0x20 / 4] = D_80075028[0x20 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D484);

/** @brief Sets the halfword at offset 0x9A of a0 to 1. */
void func_8001D4E4(u8 *a0) {
    *(u16 *)(a0 + 0x9A) = 1;
}

/** @brief Empty stub — no operation. */
void func_8001D4F0(void) {
}

/** @brief Empty stub — no operation. */
void func_8001D4F8(void) {
}

/** @brief Empty stub — no operation. */
void func_8001D500(void) {
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D508);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D5A4);

/**
 * @brief Reads a byte from the sequence data stream and sets a track parameter.
 *
 * Reads one byte from the current position in the sequence data (pointer
 * at offset +0x00), advances the read pointer, and writes the byte into
 * bits [7:4] of the track's control field at offset +0x106. Also sets
 * flag 0x1000 in the track's dirty flags at offset +0xF8.
 *
 * @param a0 Pointer to the sequence track structure (as s32 for pointer arithmetic).
 * @param a1 Unused parameter.
 */
void func_8001D5E8(s32 a0, s32 a1) {
    s32 byte = *(u8 *)(*(s32 *)a0);
    *(s32 *)a0 = *(s32 *)a0 + 1;
    *(u16 *)(a0 + 0x106) = (*(u16 *)(a0 + 0x106) & 0xFF0F) | (byte << 4);
    *(s32 *)(a0 + 0xF8) = *(s32 *)(a0 + 0xF8) | 0x1000;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D61C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D64C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D690);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D6D0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D714);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D78C);

/** @brief Increments the word at a0 by 1. */
void func_8001D7D0(s32 *a0) {
    *a0 += 1;
}

/** @brief Empty stub — no operation. */
void func_8001D7E4(void) {
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D7EC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D83C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D8D0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D93C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001D9B8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DA0C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DA34);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DA7C);

/** @brief Clears bit 0x8 in a0+0x30 and zeroes halfword at a0+0x10A. */
void func_8001DAB4(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x8;
    *(u16 *)(a0 + 0x10A) = 0;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DACC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DB04);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DB40);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DB7C);

/** @brief Reads byte from stream; if non-zero stores byte+1 to a0+0xD0, else stores 0x101.
 *  @param a0 Pointer to stream state.
 */
void func_8001DBC0(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    if (val != 0) {
        *(u16 *)(a0 + 0xD0) = val + 1;
    } else {
        *(u16 *)(a0 + 0xD0) = 0x101;
    }
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DBF0);

/** @brief Reads byte from stream; if non-zero stores byte+1 to a0+0xD2, else stores 0x101.
 *  @param a0 Pointer to stream state.
 */
void func_8001DC34(u8 *a0) {
    u8 *ptr = *(u8 **)a0;
    s32 val = *ptr;
    *(u8 **)a0 = ptr + 1;
    if (val != 0) {
        *(u16 *)(a0 + 0xD2) = val + 1;
    } else {
        *(u16 *)(a0 + 0xD2) = 0x101;
    }
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DC64);

/** @brief Sets bit 0x10 in word at a0+0x30. */
void func_8001DCCC(u8 *a0) {
    *(s32 *)(a0 + 0x30) |= 0x10;
}

/** @brief Clears bit 0x10 in word at a0+0x30. */
void func_8001DCE0(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x10;
}

/** @brief Sets bit 0x20 in word at a0+0x30. */
void func_8001DCF4(u8 *a0) {
    *(s32 *)(a0 + 0x30) |= 0x20;
}

/** @brief Clears bit 0x20 in word at a0+0x30. */
void func_8001DD08(u8 *a0) {
    *(s32 *)(a0 + 0x30) &= ~0x20;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DD1C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DDD4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DE18);

/** @brief Sets bit 0x100000 in word at a0+0x30. */
void func_8001DECC(u8 *a0) {
    *(s32 *)(a0 + 0x30) |= 0x100000;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DEE0);

/** @brief ORs a1 into the word at D_80074F08+0x8 (set flags).
 *  @param a0 Unused.
 *  @param a1 Bits to set.
 */
void func_8001DF10(s32 a0, s32 a1) {
    *(s32 *)((u8 *)D_80074F08 + 0x8) |= a1;
}

/** @brief ANDs ~a1 into the word at D_80074F08+0x8 (clear flags).
 *  @param a0 Unused.
 *  @param a1 Bits to clear.
 */
void func_8001DF30(s32 a0, s32 a1) {
    *(s32 *)((u8 *)D_80074F08 + 0x8) &= ~a1;
}

/** @brief Calls func_8001C2C8. */
void func_8001DF50(void) {
    func_8001C2C8();
}

/**
 * @brief Disables SPU IRQ and clears the IRQ voice mask from the engine state.
 *
 * If D_80077298[3] (the active IRQ voice bitmask) is non-zero, disables
 * SPU IRQ, clears the IRQ callback, resets the IRQ address to the stored
 * value, clears the corresponding bits in D_80075028[8], marks dirty flags,
 * and zeroes the IRQ voice mask.
 */
void func_8001DF70(void) {
    s32 *s0 = D_80077298;
    if (s0[3] != 0) {
        SpuSetIRQ(0);
        SpuSetIRQCallback(0);
        func_80014DE0(s0[3]);
        D_80075028[8] &= ~s0[3];
        D_80077288[2] |= 0x100;
        s0[3] = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001DFF4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E084);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E0CC);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E308);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E4C4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E594);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E5F8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E65C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E7A8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E7D8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E808);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E838);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E868);

/** @brief Calls func_8001DF70. */
void func_8001E8B0(void) {
    func_8001DF70();
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E8D0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E940);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001E9C0);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001EB0C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001EB38);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001EC0C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001EDD4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F034);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F080);

/**
 * @brief Advances a sequence loop/repeat counter with wraparound.
 *
 * Increments the global tick count at D_80077298 offset +0x28, then
 * increments @p counter. If @p counter exceeds the loop limit
 * (D_80077298[0x3C/4] - 1), wraps it back to 0.
 *
 * @param counter Pointer to the current loop/repeat index.
 * @return The new value of @p counter after increment and possible wraparound.
 */
s32 func_8001F0C4(s32 *counter) {
    s32 base = (s32)D_80077298;
    *(s32 *)(base + 0x28) += 1;
    (*counter)++;
    if ((u32)(*(s32 *)(base + 0x3C) - 1) < (u32)*counter) {
        *counter = 0;
    }
    return *counter;
}

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F118);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F2A8);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F370);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F3D4);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F51C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F54C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F57C);

INCLUDE_ASM("asm/nonmatchings/34C8", func_8001F5C8);
