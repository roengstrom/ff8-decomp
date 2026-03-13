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

extern s32 D_80075058;
extern s32 *D_80074F08;
extern s32 *D_80073CA8;

/**
 * @brief Check sound subsystem status flags.
 *
 * Returns a bitfield: bit 0 set if D_80074F08->field04 is nonzero,
 * bit 1 set if D_80073CA8 is non-NULL and its field04 is nonzero.
 *
 * @return Status bitfield.
 */
s32 func_80012E04(void) {
    s32 *ptr = D_80074F08;
    s32 *ptr2 = D_80073CA8;
    s32 result = ptr[1] != 0;
    if (ptr2 != 0 && ptr2[1] != 0) {
        result |= 2;
    }
    return result;
}
extern s32 D_80075028[];
extern s32 D_80077288[];
extern s32 D_80074EB8[];
extern s32 D_80077298[];
extern s32 D_800772CC;


INCLUDE_ASM("asm/nonmatchings/snd_init", func_80012E40);

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

/**
 * @brief Query the current SPU transfer position as a packed 32-bit value.
 *
 * Returns 0 if neither field +0x04 nor +0x1C of the sound engine state
 * (D_80074F08) is set. Otherwise returns the current position as
 * (field_0x6C << 16) | (field_0x66 + 1).
 *
 * @return Packed position value, or 0 if inactive.
 *
 * @note Non-matching: branch direction inversion (original uses beqz to skip
 *       computation, compiler generates bnez to jump to it) and register
 *       allocation (pointer in $5 original vs $4 ours).
 */
INCLUDE_ASM("asm/nonmatchings/snd_init", func_80012FEC);

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

INCLUDE_ASM("asm/nonmatchings/snd_init", func_800131A8);

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80013210);

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

/**
 * @brief Multiply input by 256, call func_8003ED24 with sign-extended 16-bit value, return full shifted result.
 *
 * Shifts the input left by 8, sign-extends the lower 16 bits, passes
 * the sign-extended value to func_8003ED24 as both arguments, then
 * returns the original (non-truncated) shifted value.
 *
 * @param a0 Input value to shift.
 * @return a0 * 256 (full 32-bit result, not truncated).
 *
 * @note Non-matching: GCC 2.8.0 combines (a0<<8)<<16 into a0<<24,
 * reading a0 directly instead of the saved s0 value. Original has
 * sll a0, s0, 16 (reads shifted from s0); compiler emits sll a0, a0, 24.
 */
INCLUDE_ASM("asm/nonmatchings/snd_init", func_800133D8);

/** @brief If a0 is non-zero, sets bit 0x10 in D_80077288[1]; otherwise clears it. Returns 0. */
s32 func_8001340C(s32 a0) {
    if (a0 != 0) {
        D_80077288[1] |= 0x10;
    } else {
        D_80077288[1] &= ~0x10;
    }
    return 0;
}

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

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80013AA8);

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

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80013CD4);

/**
 * @brief Process audio parameters via func_80013AA8 and dispatch to func_800148B0.
 *
 * Calls func_80013AA8 to compute two intermediate values (stored on the stack),
 * then passes those along with the original parameters to func_800148B0.
 * Returns the result of func_80013AA8, not func_800148B0.
 *
 * @param a0 First parameter (passed through to both calls).
 * @param a1 Second parameter (passed through to func_800148B0).
 * @return Result of func_80013AA8.
 */
s32 func_80013EE4(s32 a0, s32 a1) {
    s32 val1, val2;
    s32 result = func_80013AA8(a0, &val1, &val2);
    func_800148B0(a0, a1, val1, val2);
    return result;
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80013F38);

/**
 * @brief Configure audio playback address based on sound structure state.
 *
 * If the sound structure has active fields (field04|field1C nonzero) and
 * field00 bit 10 is set, uses address 0x3D000; otherwise uses 0x5D000.
 * Calls func_800148B0 with the determined address and level 0xB0.
 *
 * @param a0 First parameter (passed through to func_800148B0).
 * @param a1 Second parameter (passed through to func_800148B0).
 */
void func_80014034(s32 a0, s32 a1) {
    s32 *ptr = D_80074F08;
    s32 addr = 0x5D000;
    if ((ptr[1] | ptr[7]) && (ptr[0] & 0x400)) {
        addr = 0x3D000;
    }
    func_800148B0(a0, a1, 0xB0, addr);
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80014094);

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80014190);

/**
 * @brief Set CD audio volume mixing levels and apply via CdMix.
 *
 * If D_8007728C bit 1 is set, scales @p a0 by a fixed factor
 * ((a0 * 46448) >> 17) and sets all four attenuation bytes equally.
 * Otherwise, sets left-to-left and right-to-right = a0, crosstalk = 0.
 * Calls CdMix to apply the settings.
 *
 * @param a0 Volume level (0-128).
 * @return Always 0.
 */
s32 func_80014250(s32 a0) {
    extern s32 D_8007728C;
    extern u8 D_80073C30[];

    if (D_8007728C & 2) {
        s32 base = (s32)D_80073C30;
        s32 vol = (u32)(a0 * 46448) >> 17;
        *(u8 *)(base + 3) = vol;
        *(u8 *)(base + 1) = vol;
        *(u8 *)(base + 2) = vol;
        *(u8 *)(base + 0) = vol;
    } else {
        s32 base = (s32)D_80073C30;
        *(u8 *)(base + 2) = a0;
        *(u8 *)(base + 0) = a0;
        *(u8 *)(base + 3) = 0;
        *(u8 *)(base + 1) = 0;
    }
    CdMix((s32 *)D_80073C30);
    return 0;
}

/**
 * @brief Conditionally dispatch SPU command 0xE0 with three parameters.
 *
 * Calls func_80014740 first; if it returns 0, stores a0, (a1 & 0xFF) << 8,
 * and a2 into the SPU command buffer and dispatches command 0xE0.
 *
 * @param a0 First word written to D_80075058.
 * @param a1 Masked and shifted into second word of command buffer.
 * @param a2 Third word of command buffer.
 */
void func_800142E4(s32 a0, s32 a1, s32 a2) {
    extern s32 D_80075058;
    if (func_80014740() == 0) {
        register s32 cmd asm("$4") = 0xE0; // FIXME: register+barrier forces li into bne delay slot
        REGALLOC_BARRIER(cmd);
        D_80075058 = a0;
        *(&D_80075058 + 1) = (a1 & 0xFF) << 8;
        *(&D_80075058 + 2) = a2;
        func_8001A1E8(cmd);
    }
}

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

/**
 * @brief Write a0 to SPU command buffer, write masked/shifted a1 to second word, dispatch 0xE5.
 * @param a0 First word stored directly to D_80075058.
 * @param a1 Second parameter, masked to 7 bits and shifted left 8.
 */
void func_80014398(s32 a0, s32 a1) {
    extern s32 D_80075058;
    D_80075058 = a0;
    *(&D_80075058 + 1) = (a1 & 0x7F) << 8;
    func_8001A1E8(0xE5);
}

/** @brief Stores u8-masked a0, shifted left 8, to SPU command buffer. Sends command 0xE6.
 *  @param a0 Parameter, masked to 8 bits then shifted left 8.
 */
void func_800143D0(s32 a0) {
    D_80075058 = (u8)a0 << 8;
    func_8001A1E8(0xE6);
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80014400);

/**
 * @brief Write masked volume to SPU command buffer and dispatch.
 * @param a0 Volume value (masked to 8 bits, shifted left 8).
 * @param a1 Secondary parameter stored at D_80075058+4.
 */
void func_800144E4(s32 a0, s32 a1) {
    extern s32 D_80075058;
    D_80075058 = (a0 & 0xFF) << 8;
    *(&D_80075058 + 1) = a1;
    func_8001A1E8(0xED);
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_8001451C);

/**
 * @brief Initialize SPU IRQ and sound engine state for playback.
 *
 * If @p a1 is zero, returns -1. Otherwise disables SPU IRQ, clears the IRQ
 * address, stores both parameters into the SPU command buffer, initializes
 * several sound engine counters in D_80077298, sets the frame limit from
 * @p a1 >> 12, and issues command 0xE8 via func_8001A1E8.
 *
 * @param a0 First SPU command parameter (stored at D_80075058).
 * @param a1 Second SPU command parameter / frame limit source (stored at D_80075058+4).
 * @return 0 on success, -1 if @p a1 is zero.
 */
s32 func_800145E0(s32 a0, s32 a1) {
    if (a1 == 0) {
        return -1;
    }
    SpuSetIRQ(0);
    SpuSetIRQAddr(0);
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    D_80077298[13] = -1;
    D_80077298[8] = 0;
    D_80077298[9] = 0;
    D_80077298[10] = 0;
    D_80077298[14] = 0;
    D_80077298[15] = (u32)a1 >> 12;
    func_8001A1E8(0xE8);
    return 0;
}

/**
 * @brief Advance sound engine tick and frame counters; trigger IRQ callback if needed.
 *
 * Increments the tick counter (offset 0x24) and wraps the frame counter
 * (offset 0x38) at the limit (offset 0x3C). If bit 24 of the flags word
 * (offset 0x08) is set and the frame counter >= 3, calls func_8001F118.
 *
 * @return The value of D_800772CC (current sound engine state word).
 */
s32 func_8001466C(void) {
    s32 *ptr = D_80077298;
    s32 counter;

    ptr[9]++;
    counter = ptr[14] + 1;
    ptr[14] = counter;
    if ((u32)counter > (u32)(ptr[15] - 1)) {
        ptr[14] = 0;
    }
    if ((ptr[2] & 0x1000000) && (u32)ptr[14] >= 3) {
        func_8001F118();
    }
    return D_800772CC;
}

