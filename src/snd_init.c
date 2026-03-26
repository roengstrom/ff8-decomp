#include "common.h"

/** @brief Initializes the SPU (Sound Processing Unit).
 *
 *  Wrapper that calls sndSpuInit (which runs the full SPU init sequence:
 *  SpuStart, SpuInitMalloc, transfer mode setup, IRQ, and root counter timer)
 *  and returns 0.
 */
s32 sndInit(void) {
    sndSpuInit();
    return 0;
}

/** @brief Shuts down the SPU and cleans up sound resources.
 *
 *  Wrapper that calls sndSpuShutdown (which stops the root counter timer,
 *  disables/closes the timer event, resets the SPU IRQ address to 0xFFFFFF,
 *  and reinitializes the SPU) and returns 0.
 */
s32 sndShutdown(void) {
    sndSpuShutdown();
    return 0;
}

/**
 * @brief Parses a sound bank header and sets up audio data pointers.
 *
 * Calls sndValidateBank to validate/process the sound bank at @p a0.
 * On success, partitions the memory after the 16-byte header into three
 * regions: D_80074ED0 (0x400 bytes), D_80074ED8 (0x200 bytes), and
 * D_80074EDC (remainder).
 *
 * @param a0 Pointer to the sound bank data loaded from CD.
 * @return 0 on success, non-zero error code from sndValidateBank on failure.
 */
s32 sndLoadBank(u8 *a0) {
    extern u8 *D_80074ED0;
    extern u8 *D_80074ED8;
    extern u8 *D_80074EDC;
    s32 result = sndValidateBank(a0);
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
 * over all 24 SPU voices setting their release mode via spuSetVoiceReleaseMode.
 * Finally resets the IRQ address and re-enables the event.
 *
 * @return Always returns 0.
 */
s32 sndStopAll(void) {
    extern s32 D_8005169C;
    extern s32 D_80074ED4;
    extern u8 D_800516B8[];
    s32 i;

    DisableEvent(D_8005169C);
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);
    if (D_80074ED4 == 1) {
        sndDmaWriteSpu((s32)D_800516B8, 0x40);
        sndDmaWait();
    }
    for (i = 0; i < 24; i++) {
        spuSetVoiceReleaseMode(i, 5, 3);
    }
    spuSetIrqAddr(0xFFFFFF);
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
s32 sndGetStatus(void) {
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


/**
 * @brief Compute the maximum volume from multiple sound sources.
 *
 * Combines volume values from up to three sources based on the bitmask
 * in @p a0. Bit 0: use D_80074F08->0x58 as initial volume. Bit 1: clamp
 * up to D_80073E62. Bit 2: clamp up to D_80073E60.
 *
 * @param a0 Bitmask selecting which volume sources to consider.
 * @return The maximum volume across the selected sources.
 */
s32 sndGetMaxVolume(s32 a0) {
    extern s16 D_80073E62;
    extern s16 D_80073E60;
    s32 vol = 0;
    if (a0 & 1) {
        vol = *(s16 *)((u8 *)D_80074F08 + 0x58);
    }
    if (a0 & 2) {
        s32 v = D_80073E62;
        if (vol < v) {
            vol = v;
        }
    }
    if (a0 & 4) {
        s32 v = D_80073E60;
        if (vol < v) {
            vol = v;
        }
    }
    return vol;
}

/** @brief Sends SPU command 0x10 with parameter @p a0 via the command buffer. */
void sndCmd10(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x10);
}

/** @brief Sends SPU command 0x11 with parameter @p a0 via the command buffer. */
void sndCmd11(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x11);
}

/**
 * @brief Sends SPU command 0x14 with three parameters via the command buffer.
 * @param a0 First command parameter (stored at D_80075058[0]).
 * @param a1 Second command parameter (stored at D_80075058[1]).
 * @param a2 Third command parameter (stored at D_80075058[2]).
 */
void sndCmd14(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2;
    func_8001A1E8(0x14);
}

/** @brief Sends SPU command 0x40. */
void sndCmd40(void) {
    func_8001A1E8(0x40);
}

/** @brief Stores a0 and masked a1 to SPU command buffer, sends command 0x19.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 7 bits.
 */
void sndCmd19(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0x19);
}

/** @brief Stores a0, a1, and a2 (masked to 7 bits) to SPU command buffer, sends command 0x1A.
 *  @param a0 First command parameter (voice/channel).
 *  @param a1 Second command parameter.
 *  @param a2 Third command parameter (masked to 0x7F).
 */
void sndCmd1A(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2 & 0x7F;
    func_8001A1E8(0x1A);
}

/** @brief Stores a0 and a1 to SPU command buffer, sends command 0x12.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 */
void sndCmd12(s32 a0, s32 a1) {
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
 */
INCLUDE_ASM("asm/nonmatchings/snd_init", func_80012FEC);

/**
 * @brief Sends SPU command 0x20 (play sound effect) with voice and playback parameters.
 * @param a0 Voice index, masked to 10 bits (0-1023).
 * @param a1 SPU address/offset, masked to 24 bits.
 * @param a2 Volume or envelope parameter, masked to 8 bits.
 * @param a3 Pan or pitch parameter, masked to 7 bits (0-127).
 */
void sndPlaySfx(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0 & 0x3FF;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0x20);
}

/**
 * @brief Validates a sound bank address and sends SPU command 0x24 to play from it.
 *
 * Calls sndValidateBank to validate @p a0 first. On failure, returns the error.
 * On success, writes playback parameters to the command buffer and dispatches
 * command 0x24.
 *
 * @param a0 Sound bank address to validate and play.
 * @param a1 SPU address/offset, masked to 24 bits.
 * @param a2 Volume or envelope parameter, masked to 8 bits.
 * @param a3 Pan or pitch parameter, masked to 7 bits (0-127).
 * @return @p a0 on success, non-zero error code on validation failure.
 */
s32 sndPlayBankSfx(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = sndValidateBank(a0);
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
void sndCmd21(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    func_8001A1E8(0x21);
}

/**
 * @brief Sends SPU command 0x30 (key on) with a 10-bit voice bitmask.
 * @param a0 Voice bitmask, masked to 10 bits.
 */
void sndKeyOn(s32 a0) {
    D_80075058 = a0 & 0x3FF;
    func_8001A1E8(0x30);
}

/**
 * @brief Sends SPU command 0x44 (stop/reset playback).
 * @note TODO: maspsx schedules addiu $sp into load delay slot instead of jr delay slot.
 */
void sndStopPlayback(void) {
    func_8001A1E8(0x44);
}

/** @brief Sends SPU command 0x45. */
void sndCmd45(void) {
    func_8001A1E8(0x45);
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_800131A8);

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80013210);

/** @brief Sends SPU command 0x81 if a0 == 1, else 0x80.
 *  @param a0 Selects command (1 = 0x81, else 0x80).
 */
void sndSelectMode(s32 a0) {
    if (a0 == 1) {
        func_8001A1E8(0x81);
    } else {
        func_8001A1E8(0x80);
    }
}

/** @brief Stores a0 to SPU command buffer, sends command 0x90.
 *  @param a0 Command parameter.
 */
void sndCmd90(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x90);
}

/** @brief Stores a0 to SPU command buffer, sends command 0x92.
 *  @param a0 Command parameter.
 */
void sndCmd92(s32 a0) {
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
void sndEnableReverb(u32 a0) {
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
void sndDisableReverb(u32 a0) {
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
 */
INCLUDE_ASM("asm/nonmatchings/snd_init", func_800133D8);

/** @brief If a0 is non-zero, sets bit 0x10 in D_80077288[1]; otherwise clears it. Returns 0. */
s32 sndSetEngineFlag(s32 a0) {
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
void sndSetMasterVolume(s32 a0) {
    D_80075058 = a0 & 0x7F;
    func_8001A1E8(0xA8);
}

/** @brief Stores a0 and 7-bit masked a1 to SPU command buffer, sends command 0xA9.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 7 bits.
 */
void sndSetChannelVolume(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0xA9);
}

/** @brief Stores a0, 24-bit masked a1, and 7-bit masked a2 to SPU command buffer, sends command 0xA0.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third parameter, masked to 7 bits.
 */
void sndSeqPlay7bit(s32 a0, s32 a1, s32 a2) {
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
void sndSeqPlayPan7bit(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0xA1);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xAA.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void sndSeqSetTempo(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xAA);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xAB.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void sndSeqSetChannelTempo(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xAB);
}

/** @brief Stores a0, 24-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xA2.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void sndSeqPlay8bit(s32 a0, s32 a1, s32 a2) {
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
void sndSeqPlayPan8bit(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2;
    *(&D_80075058 + 3) = a3 & 0xFF;
    func_8001A1E8(0xA3);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xAC.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void sndSeqSetTempoAlt(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xAC);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xAD.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void sndSeqSetChannelTempoAlt(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xAD);
}

/** @brief Stores a0, 24-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xA4.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 24 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void sndSeqStart(s32 a0, s32 a1, s32 a2) {
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
void sndSeqStartPan(s32 a0, s32 a1, s32 a2, s32 a3) {
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
void sndCmdC0(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0xC0);
}

/** @brief Stores a0, a1, and 7-bit masked a2 to SPU command buffer, sends command 0xC1.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 *  @param a2 Third parameter, masked to 7 bits.
 */
void sndCmdC1(s32 a0, s32 a1, s32 a2) {
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
void sndCmdC2(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2 & 0x7F;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0xC2);
}

/** @brief Stores a0 to SPU command buffer, sends command 0xC8.
 *  @param a0 Command parameter.
 */
void sndCmdC8(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0xC8);
}

/** @brief Stores a0 and a1 to SPU command buffer, sends command 0xC9.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 */
void sndCmdC9(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    func_8001A1E8(0xC9);
}

/** @brief Stores a0, a1 and a2 to SPU command buffer, sends command 0xCA.
 *  @param a0 First command parameter.
 *  @param a1 Second command parameter.
 *  @param a2 Third command parameter.
 */
void sndCmdCA(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1;
    *(&D_80075058 + 2) = a2;
    func_8001A1E8(0xCA);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xD0.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void sndCmdD0(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xD0);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xD1.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void sndCmdD1(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xD1);
}

/** @brief Stores a0, 8-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xD2.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void sndCmdD2(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xD2);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xD4.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void sndCmdD4(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xD4);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xD5.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void sndCmdD5(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xD5);
}

/** @brief Stores a0, 8-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xD6.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void sndCmdD6(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xD6);
}

/** @brief Stores u8-masked a0 to SPU command buffer, sends command 0xD8.
 *  @param a0 Command parameter, masked to 8 bits.
 */
void sndCmdD8(s32 a0) {
    D_80075058 = (u8)a0;
    func_8001A1E8(0xD8);
}

/** @brief Stores a0 and u8-masked a1 to SPU command buffer, sends command 0xD9.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 */
void sndCmdD9(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = (u8)a1;
    func_8001A1E8(0xD9);
}

/** @brief Stores a0, 8-bit masked a1, and 8-bit masked a2 to SPU command buffer, sends command 0xDA.
 *  @param a0 First command parameter.
 *  @param a1 Second parameter, masked to 8 bits.
 *  @param a2 Third parameter, masked to 8 bits.
 */
void sndCmdDA(s32 a0, s32 a1, s32 a2) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    func_8001A1E8(0xDA);
}

/** @brief Sends SPU command 0xF0. */
void sndCmdF0(void) {
    func_8001A1E8(0xF0);
}

/** @brief Sends SPU command 0xF1 (system/engine reset or flush). */
void sndCmdF1(void) {
    func_8001A1E8(0xF1);
}

/**
 * @brief Uploads sample data to SPU RAM, spinning until the transfer completes.
 *
 * Called during sound engine init to upload sound bank samples from main RAM
 * to SPU RAM. Blocks in a busy-wait loop until sndUploadSample indicates the
 * transfer is finished.
 *
 * @param a0 Source address of sample data in main RAM.
 * @param a1 Transfer mode or bank index.
 */
void sndUploadSamples(s32 a0, s32 a1) {
    do {
    } while (sndUploadSample(a0, a1) == 1);
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80013AA8);

/** @brief Returns the value of global D_80074ED4. */
s32 sndGetEngineState(void) {
    extern s32 D_80074ED4;
    return D_80074ED4;
}

/** @brief Clears D_8007735C, sets bit 0x1 in D_80077288, returns 0. */
s32 sndResetState(void) {
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
s32 sndProcessAudio(s32 a0, s32 a1) {
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
void sndSetPlaybackAddr(s32 a0, s32 a1) {
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
s32 sndSetCdMixVolume(s32 a0) {
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
 * Calls sndValidateBank first; if it returns 0, stores a0, (a1 & 0xFF) << 8,
 * and a2 into the SPU command buffer and dispatches command 0xE0.
 *
 * @param a0 First word written to D_80075058.
 * @param a1 Masked and shifted into second word of command buffer.
 * @param a2 Third word of command buffer.
 */
void sndCmdE0(s32 a0, s32 a1, s32 a2) {
    extern s32 D_80075058;
    if (sndValidateBank() == 0) {
        register s32 cmd asm("$4") = 0xE0; // FIXME: register+barrier forces li into bne delay slot
        REGALLOC_BARRIER(cmd);
        D_80075058 = a0;
        *(&D_80075058 + 1) = (a1 & 0xFF) << 8;
        *(&D_80075058 + 2) = a2;
        func_8001A1E8(cmd);
    }
}

/** @brief Sends SPU command 0xE2. */
void sndCmdE2(void) {
    func_8001A1E8(0xE2);
}

/** @brief Stores 7-bit masked a0, shifted left 8, to SPU command buffer. Sends command 0xE4.
 *  @param a0 Parameter, masked to 7 bits then shifted left 8.
 */
void sndCmdE4(s32 a0) {
    D_80075058 = (a0 & 0x7F) << 8;
    func_8001A1E8(0xE4);
}

/**
 * @brief Write a0 to SPU command buffer, write masked/shifted a1 to second word, dispatch 0xE5.
 * @param a0 First word stored directly to D_80075058.
 * @param a1 Second parameter, masked to 7 bits and shifted left 8.
 */
void sndCmdE5(s32 a0, s32 a1) {
    extern s32 D_80075058;
    D_80075058 = a0;
    *(&D_80075058 + 1) = (a1 & 0x7F) << 8;
    func_8001A1E8(0xE5);
}

/** @brief Stores u8-masked a0, shifted left 8, to SPU command buffer. Sends command 0xE6.
 *  @param a0 Parameter, masked to 8 bits then shifted left 8.
 */
void sndCmdE6(s32 a0) {
    D_80075058 = (u8)a0 << 8;
    func_8001A1E8(0xE6);
}

INCLUDE_ASM("asm/nonmatchings/snd_init", func_80014400);

/**
 * @brief Write masked volume to SPU command buffer and dispatch.
 * @param a0 Volume value (masked to 8 bits, shifted left 8).
 * @param a1 Secondary parameter stored at D_80075058+4.
 */
void sndCmdED(s32 a0, s32 a1) {
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
s32 sndInitIrq(s32 a0, s32 a1) {
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
s32 sndTickCounters(void) {
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

