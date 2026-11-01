#include "common.h"
#include "psxsdk/libspu.h"
#include "snd_init.h"
#include "snd_dma.h"
#include "snd_note.h"
#include "snd_param.h"
#include "snd_track.h"


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
 * @brief Validates a sound bank and caches pointers to its data regions.
 *
 * Validates the bank via sndValidateBank. On success, walks past the
 * @ref SoundBank header and caches a pointer to each following region: the
 * sample-offset table (D_80074ED0), the instrument-group table (D_80074ED8),
 * and the ADPCM sample data (D_80074EDC). The increments are taken from the
 * @ref SoundBank field sizes so the partition tracks the documented layout.
 *
 * @param bank Pointer to the sound bank data loaded from CD (see @ref SoundBank).
 * @return 0 on success, non-zero error code from sndValidateBank on failure.
 */
s32 sndLoadBank(u8 *bank) {
    s32 result = sndValidateBank(bank);
    if (result == 0) {
        /*
         * Publish the base of each region after the header. Conceptually
         * (viewing bank as a SoundBank *) this is simply:
         *     D_80074ED0 = bank->sampleOffsets;
         *     D_80074ED8 = bank->instrumentGroups;
         *     D_80074EDC = bank->sampleData;
         * but it must be written as a running-pointer walk (each step advances
         * past the region it just published) to match: the original was compiled
         * from a chained pointer (+= 0x10, += 0x400, += 0x200), whereas direct
         * field access emits three independent absolute offsets and does not match.
         */
        bank += sizeof(((SoundBank *)0)->header);
        D_80074ED0 = (u16 *)bank;
        bank += sizeof(((SoundBank *)0)->sampleOffsets);
        D_80074ED8 = (u16 *)bank;
        bank += sizeof(((SoundBank *)0)->instrumentGroups);
        D_80074EDC = bank;
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
    s32 i;

    DisableEvent(D_8005169C);
    SpuSetIRQ(0);
    SpuSetIRQCallback(NULL);
    if (D_80074ED4 == 1) {
        sndDmaWriteSpu(D_800516B8, 0x40);
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


/**
 * @brief Check sound subsystem status flags.
 *
 * Returns a bitfield: bit 0 set if g_sndSeqState->instParams is nonzero,
 * bit 1 set if D_80073CA8 is non-NULL and its word at +4 is nonzero.
 *
 * @return Status bitfield.
 */
s32 sndGetStatus(void) {
    SoundMusicConfig *ptr = g_sndSeqState;
    SoundMusicConfig *ptr2 = D_80073CA8;
    s32 result = ptr->unk04 != 0;
    if (ptr2 != NULL && ptr2->unk04 != 0) {
        result |= 2;
    }
    return result;
}


/**
 * @brief Compute the maximum volume from multiple sound sources.
 *
 * Combines volume values from up to three sources based on the bitmask
 * in @p mask. Bit 0: use g_sndSeqState->pitchValue as initial volume. Bit 1: clamp
 * up to D_80073E62. Bit 2: clamp up to D_80073E60.
 *
 * @param mask Bitmask selecting which volume sources to consider.
 * @return The maximum volume across the selected sources.
 */
s32 sndGetMaxVolume(s32 mask) {
    s32 vol = 0;
    if (mask & 1) {
        vol = (s16)g_sndSeqState->unk58;
    }
    if (mask & 2) {
        s32 v = D_80073E62;
        if (vol < v) {
            vol = v;
        }
    }
    if (mask & 4) {
        s32 v = D_80073E60;
        if (vol < v) {
            vol = v;
        }
    }
    return vol;
}

/** @brief Sends SPU command 0x10 with parameter @p arg0 via the command buffer. */
s32 sndCmd10(s32 arg0) {
    g_sndCmdArgs[0] = arg0;
    return func_8001A1E8(0x10);
}

/** @brief Sends SPU command 0x11 with parameter @p arg0 via the command buffer. */
s32 sndCmd11(s32 arg0) {
    g_sndCmdArgs[0] = arg0;
    return func_8001A1E8(0x11);
}

/**
 * @brief Sends SPU command 0x14 with three parameters via the command buffer.
 * @param arg0 First command parameter (stored at g_sndCmdArgs[0]).
 * @param arg1 Second command parameter (stored at g_sndCmdArgs[1]).
 * @param arg2 Third command parameter (stored at g_sndCmdArgs[2]).
 */
s32 sndCmd14(s32 arg0, s32 arg1, s32 arg2) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1;
    g_sndCmdArgs[2] = arg2;
    return func_8001A1E8(0x14);
}

/** @brief Sends SPU command 0x40. */
void sndCmd40(void) {
    func_8001A1E8(0x40);
}

/** @brief Stores arg0 and masked pan to SPU command buffer, sends command 0x19.
 *  @param arg0 First command parameter.
 *  @param pan Second parameter, masked to 7 bits.
 */
s32 sndCmd19(s32 arg0, s32 pan) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = pan & 0x7F;
    return func_8001A1E8(0x19);
}

/** @brief Stores voice, arg1, and pan (masked to 7 bits) to SPU command buffer, sends command 0x1A.
 *  @param voice First command parameter (voice/channel).
 *  @param arg1 Second command parameter.
 *  @param pan Third command parameter (masked to 0x7F).
 */
s32 sndCmd1A(s32 voice, s32 arg1, s32 pan) {
    g_sndCmdArgs[0] = voice;
    g_sndCmdArgs[1] = arg1;
    g_sndCmdArgs[2] = pan & 0x7F;
    return func_8001A1E8(0x1A);
}

/** @brief Stores arg0 and arg1 to SPU command buffer, sends command 0x12.
 *  @param arg0 First command parameter.
 *  @param arg1 Second command parameter.
 */
s32 sndCmd12(s32 arg0, s32 arg1) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1;
    return func_8001A1E8(0x12);
}

/**
 * @brief Query the current SPU transfer position as a packed 32-bit value.
 *
 * Returns 0 if neither @c instParams nor @c keyOnPending of the sound engine
 * state (g_sndSeqState) is set. Otherwise returns the current position as
 * (pitchModCounter << 16) | (instrument + 1).
 *
 * @return Packed position value, or 0 if inactive.
 */
s32 sndGetSeqPosition(void) {
    SoundMusicConfig *ptr = g_sndSeqState;
    s32 result = 0;
    if ((ptr->unk04 | ptr->unk1C) != 0) {
        result = ptr->unk6C << 16;
        result |= ptr->unk66 + 1;
    }
    return result;
}

/**
 * @brief Sends SPU command 0x20 (play sound effect) with voice and playback parameters.
 * @param voiceMask Voice index, masked to 10 bits (0-1023).
 * @param addr SPU address/offset, masked to 24 bits.
 * @param volume Volume or envelope parameter, masked to 8 bits.
 * @param pan Pan or pitch parameter, masked to 7 bits (0-127).
 */
void sndPlaySfx(s32 voiceMask, s32 addr, s32 volume, s32 pan) {
    g_sndCmdArgs[0] = voiceMask & 0x3FF;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume & 0xFF;
    g_sndCmdArgs[3] = pan & 0x7F;
    func_8001A1E8(0x20);
}

/**
 * @brief Validates a sound bank address and sends SPU command 0x24 to play from it.
 *
 * Calls sndValidateBank to validate @p bank first. On failure, returns the error.
 * On success, writes playback parameters to the command buffer and dispatches
 * command 0x24.
 *
 * @param bank Sound bank address to validate and play.
 * @param addr SPU address/offset, masked to 24 bits.
 * @param volume Volume or envelope parameter, masked to 8 bits.
 * @param pan Pan or pitch parameter, masked to 7 bits (0-127).
 * @return @p bank on success, non-zero error code on validation failure.
 */
s32 sndPlayBankSfx(s32 bank, s32 addr, s32 volume, s32 pan) {
    s32 result = sndValidateBank(bank);
    if (result != 0) {
        return result;
    }
    g_sndCmdArgs[0] = bank;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume & 0xFF;
    g_sndCmdArgs[3] = pan & 0x7F;
    func_8001A1E8(0x24);
    return bank;
}

/**
 * @brief Sends SPU command 0x21 with an address and a 24-bit parameter.
 * @param arg0 First parameter (stored at g_sndCmdArgs[0]).
 * @param addr Second parameter, masked to 24 bits (stored at g_sndCmdArgs[1]).
 */
void sndCmd21(s32 arg0, s32 addr) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    func_8001A1E8(0x21);
}

/**
 * @brief Sends SPU command 0x30 (key on) with a 10-bit voice bitmask.
 * @param voiceMask Voice bitmask, masked to 10 bits.
 */
void sndKeyOn(s32 voiceMask) {
    g_sndCmdArgs[0] = voiceMask & 0x3FF;
    func_8001A1E8(0x30);
}

/** @brief Sends SPU command 0x44 (stop/reset playback). */
void sndStopPlayback(void) {
    func_8001A1E8(0x44);
}

/** @brief Sends SPU command 0x45. */
void sndCmd45(void) {
    func_8001A1E8(0x45);
}

/**
 * @brief Collect key-on voice masks from all active tracks.
 *
 * Iterates over sound sequence tracks starting from D_80072F70. For each
 * track whose corresponding bit in D_80075028 is set, ORs the track's
 * keyOnMask field into the result. The bit mask starts at 0x1000 and
 * shifts left each iteration.
 *
 * @return Combined key-on mask (24-bit), or 0 if D_80075028 is 0.
 */
s32 sndGatherKeyOnMask(void) {
    s32 mask = D_80075028.unk00;
    SoundSeqTrack *track;
    s32 bit;
    s32 result;

    if (mask == 0) {
        return 0;
    }

    track = D_80072F70;
    result = 0;
    for (bit = 0x1000; bit & 0xFFFFFF; bit <<= 1, track++) {
        if (mask & bit) {
            result |= track->keyOnMask;
        }
    }

    result &= 0xFFFFFF;
    return result;
}

/**
 * @brief Test whether any active CD-audio track has a given key-on mask.
 *
 * Scans the 12 CD-audio tracks (D_80072F70, voices 12-23) and, for each
 * track whose voice bit is set in D_80075028, compares its keyOnMask field
 * against @p keyOnMask.
 *
 * @param keyOnMask Key-on mask value to search for (nonzero).
 * @return 1 if a matching active track is found; 0 otherwise (including when
 *         @p keyOnMask is 0 or no voices are active).
 */
s32 sndFindKeyOnMask(s32 keyOnMask) {
    s32 mask;
    SoundSeqTrack *track;
    s32 bit;

    if (keyOnMask == 0) {
        return 0;
    }
    mask = D_80075028.unk00;
    if (mask == 0) {
        return 0;
    }

    track = D_80072F70;
    for (bit = 0x1000; bit & 0xFFFFFF; bit <<= 1, track++) {
        if (mask & bit) {
            if (keyOnMask == track->keyOnMask) {
                return 1;
            }
        }
    }

    return 0;
}

/** @brief Sends SPU command 0x81 if mode == 1, else 0x80.
 *  @param mode Selects command (1 = 0x81, else 0x80).
 */
void sndSelectMode(s32 mode) {
    if (mode == 1) {
        func_8001A1E8(0x81);
    } else {
        func_8001A1E8(0x80);
    }
}

/** @brief Stores arg0 to SPU command buffer, sends command 0x90.
 *  @param arg0 Command parameter.
 */
void sndCmd90(s32 arg0) {
    g_sndCmdArgs[0] = arg0;
    func_8001A1E8(0x90);
}

/** @brief Stores arg0 to SPU command buffer, sends command 0x92.
 *  @param arg0 Command parameter.
 */
void sndCmd92(s32 arg0) {
    g_sndCmdArgs[0] = arg0;
    func_8001A1E8(0x92);
}

/**
 * @brief Selects and sends a reverb mode command based on a mode index.
 *
 * Maps mode index to SPU command codes:
 *   1 -> 0x9B, 2 -> 0x9D, 3 -> 0x9F, default -> 0x99.
 *
 * @param mode Reverb mode selector (1-3 for specific modes, other for default).
 */
void sndEnableReverb(u32 mode) {
    s32 val;
    switch (mode) {
        case 1: val = 0x9B; break;
        case 2: val = 0x9D; break;
        case 3: val = 0x9F; break;
        default: val = 0x99; break;
    }
    func_8001A1E8(val);
}

/** @brief Sends SPU command based on mode selector (1→0x9A, 2→0x9C, 3→0x9E, default→0x98).
 *  @param mode Mode selector.
 */
void sndDisableReverb(u32 mode) {
    s32 val;
    switch (mode) {
        case 1: val = 0x9A; break;
        case 2: val = 0x9C; break;
        case 3: val = 0x9E; break;
        default: val = 0x98; break;
    }
    func_8001A1E8(val);
}

/**
 * @brief Multiply input by 256, call SpuSetReverbModeDepth with sign-extended 16-bit value,
 *        return full shifted result.
 *
 * Shifts the input left by 8, sign-extends the lower 16 bits, passes
 * the sign-extended value to SpuSetReverbModeDepth as both arguments, then
 * returns the original (non-truncated) shifted value.
 *
 * @param a0 Input value to shift.
 * @return a0 * 256 (full 32-bit result, not truncated).
 */
s32 func_800133D8(s32 arg0) {
    arg0 <<= 8;
    SpuSetReverbModeDepth((s16)arg0, (s16)arg0);
    return arg0;
}

/** @brief If enable is non-zero, sets bit 0x10 in D_80077288[1]; otherwise clears it. Returns 0. */
s32 sndSetEngineFlag(s32 enable) {
    if (enable != 0) {
        D_80077288[1] |= 0x10;
    } else {
        D_80077288[1] &= ~0x10;
    }
    return 0;
}

/**
 * @brief Sends SPU command 0xA8 with a 7-bit volume parameter.
 * @param volume Volume value, masked to 7 bits (0-127).
 */
void sndSetMasterVolume(s32 volume) {
    g_sndCmdArgs[0] = volume & 0x7F;
    func_8001A1E8(0xA8);
}

/** @brief Stores channel and 7-bit masked volume to SPU command buffer, sends command 0xA9.
 *  @param channel First command parameter.
 *  @param volume Second parameter, masked to 7 bits.
 */
void sndSetChannelVolume(s32 channel, s32 volume) {
    g_sndCmdArgs[0] = channel;
    g_sndCmdArgs[1] = volume & 0x7F;
    func_8001A1E8(0xA9);
}

/** @brief Stores seqId, 24-bit masked addr, and 7-bit masked volume to SPU command buffer, sends command 0xA0.
 *  @param seqId First command parameter.
 *  @param addr Second parameter, masked to 24 bits.
 *  @param volume Third parameter, masked to 7 bits.
 */
void sndSeqPlay7bit(s32 seqId, s32 addr, s32 volume) {
    g_sndCmdArgs[0] = seqId;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume & 0x7F;
    func_8001A1E8(0xA0);
}

/** @brief Stores seqId, 24-bit masked addr, volume, and 7-bit masked pan to SPU command buffer, sends command 0xA1.
 *  @param seqId First command parameter.
 *  @param addr Second parameter, masked to 24 bits.
 *  @param volume Third command parameter.
 *  @param pan Fourth parameter, masked to 7 bits.
 */
void sndSeqPlayPan7bit(s32 seqId, s32 addr, s32 volume, s32 pan) {
    g_sndCmdArgs[0] = seqId;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume;
    g_sndCmdArgs[3] = pan & 0x7F;
    func_8001A1E8(0xA1);
}

/** @brief Stores u8-masked tempo to SPU command buffer, sends command 0xAA.
 *  @param tempo Command parameter, masked to 8 bits.
 */
void sndSeqSetTempo(s32 tempo) {
    g_sndCmdArgs[0] = (u8)tempo;
    func_8001A1E8(0xAA);
}

/** @brief Stores channel and u8-masked tempo to SPU command buffer, sends command 0xAB.
 *  @param channel First command parameter.
 *  @param tempo Second parameter, masked to 8 bits.
 */
void sndSeqSetChannelTempo(s32 channel, s32 tempo) {
    g_sndCmdArgs[0] = channel;
    g_sndCmdArgs[1] = (u8)tempo;
    func_8001A1E8(0xAB);
}

/** @brief Stores seqId, 24-bit masked addr, and 8-bit masked volume to SPU command buffer, sends command 0xA2.
 *  @param seqId First command parameter.
 *  @param addr Second parameter, masked to 24 bits.
 *  @param volume Third parameter, masked to 8 bits.
 */
void sndSeqPlay8bit(s32 seqId, s32 addr, s32 volume) {
    g_sndCmdArgs[0] = seqId;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume & 0xFF;
    func_8001A1E8(0xA2);
}

/** @brief Stores seqId, 24-bit masked addr, volume, and 8-bit masked pan to SPU command buffer, sends command 0xA3.
 *  @param seqId First command parameter.
 *  @param addr Second parameter, masked to 24 bits.
 *  @param volume Third command parameter.
 *  @param pan Fourth parameter, masked to 8 bits.
 */
void sndSeqPlayPan8bit(s32 seqId, s32 addr, s32 volume, s32 pan) {
    g_sndCmdArgs[0] = seqId;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume;
    g_sndCmdArgs[3] = pan & 0xFF;
    func_8001A1E8(0xA3);
}

/** @brief Stores u8-masked tempo to SPU command buffer, sends command 0xAC.
 *  @param tempo Command parameter, masked to 8 bits.
 */
void sndSeqSetTempoAlt(s32 tempo) {
    g_sndCmdArgs[0] = (u8)tempo;
    func_8001A1E8(0xAC);
}

/** @brief Stores channel and u8-masked tempo to SPU command buffer, sends command 0xAD.
 *  @param channel First command parameter.
 *  @param tempo Second parameter, masked to 8 bits.
 */
void sndSeqSetChannelTempoAlt(s32 channel, s32 tempo) {
    g_sndCmdArgs[0] = channel;
    g_sndCmdArgs[1] = (u8)tempo;
    func_8001A1E8(0xAD);
}

/** @brief Stores seqId, 24-bit masked addr, and 8-bit masked volume to SPU command buffer, sends command 0xA4.
 *  @param seqId First command parameter.
 *  @param addr Second parameter, masked to 24 bits.
 *  @param volume Third parameter, masked to 8 bits.
 */
void sndSeqStart(s32 seqId, s32 addr, s32 volume) {
    g_sndCmdArgs[0] = seqId;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume & 0xFF;
    func_8001A1E8(0xA4);
}

/** @brief Stores seqId, 24-bit masked addr, volume, and 8-bit masked pan to SPU command buffer, sends command 0xA5.
 *  @param seqId First command parameter.
 *  @param addr Second parameter, masked to 24 bits.
 *  @param volume Third command parameter.
 *  @param pan Fourth parameter, masked to 8 bits.
 */
void sndSeqStartPan(s32 seqId, s32 addr, s32 volume, s32 pan) {
    g_sndCmdArgs[0] = seqId;
    g_sndCmdArgs[1] = addr & 0xFFFFFF;
    g_sndCmdArgs[2] = volume;
    g_sndCmdArgs[3] = pan & 0xFF;
    func_8001A1E8(0xA5);
}

/**
 * @brief Sends SPU command 0xC0 with a channel index and 7-bit parameter.
 * @param channel Channel or voice identifier.
 * @param value Parameter value, masked to 7 bits (0-127).
 */
s32 sndCmdC0(s32 channel, s32 value) {
    g_sndCmdArgs[0] = channel;
    g_sndCmdArgs[1] = value & 0x7F;
    return func_8001A1E8(0xC0);
}

/** @brief Stores arg0, arg1, and 7-bit masked value to SPU command buffer, sends command 0xC1.
 *  @param arg0 First command parameter.
 *  @param arg1 Second command parameter.
 *  @param value Third parameter, masked to 7 bits.
 */
s32 sndCmdC1(s32 arg0, s32 arg1, s32 value) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1;
    g_sndCmdArgs[2] = value & 0x7F;
    return func_8001A1E8(0xC1);
}

/** @brief Stores arg0, arg1, 7-bit masked arg2, and 7-bit masked arg3 to SPU command buffer, sends command 0xC2.
 *  @param arg0 First command parameter.
 *  @param arg1 Second command parameter.
 *  @param arg2 Third parameter, masked to 7 bits.
 *  @param arg3 Fourth parameter, masked to 7 bits.
 */
s32 sndCmdC2(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1;
    g_sndCmdArgs[2] = arg2 & 0x7F;
    g_sndCmdArgs[3] = arg3 & 0x7F;
    return func_8001A1E8(0xC2);
}

/** @brief Stores arg0 to SPU command buffer, sends command 0xC8.
 *  @param arg0 Command parameter.
 */
void sndCmdC8(s32 arg0) {
    g_sndCmdArgs[0] = arg0;
    func_8001A1E8(0xC8);
}

/** @brief Stores arg0 and arg1 to SPU command buffer, sends command 0xC9.
 *  @param arg0 First command parameter.
 *  @param arg1 Second command parameter.
 */
void sndCmdC9(s32 arg0, s32 arg1) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1;
    func_8001A1E8(0xC9);
}

/** @brief Stores arg0, arg1 and arg2 to SPU command buffer, sends command 0xCA.
 *  @param arg0 First command parameter.
 *  @param arg1 Second command parameter.
 *  @param arg2 Third command parameter.
 */
void sndCmdCA(s32 arg0, s32 arg1, s32 arg2) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1;
    g_sndCmdArgs[2] = arg2;
    func_8001A1E8(0xCA);
}

/** @brief Stores u8-masked arg0 to SPU command buffer, sends command 0xD0.
 *  @param arg0 Command parameter, masked to 8 bits.
 */
void sndCmdD0(s32 arg0) {
    g_sndCmdArgs[0] = (u8)arg0;
    func_8001A1E8(0xD0);
}

/** @brief Stores arg0 and u8-masked arg1 to SPU command buffer, sends command 0xD1.
 *  @param arg0 First command parameter.
 *  @param arg1 Second parameter, masked to 8 bits.
 */
void sndCmdD1(s32 arg0, s32 arg1) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = (u8)arg1;
    func_8001A1E8(0xD1);
}

/** @brief Stores arg0, 8-bit masked arg1, and 8-bit masked arg2 to SPU command buffer, sends command 0xD2.
 *  @param arg0 First command parameter.
 *  @param arg1 Second parameter, masked to 8 bits.
 *  @param arg2 Third parameter, masked to 8 bits.
 */
void sndCmdD2(s32 arg0, s32 arg1, s32 arg2) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1 & 0xFF;
    g_sndCmdArgs[2] = arg2 & 0xFF;
    func_8001A1E8(0xD2);
}

/** @brief Stores u8-masked arg0 to SPU command buffer, sends command 0xD4.
 *  @param arg0 Command parameter, masked to 8 bits.
 */
void sndCmdD4(s32 arg0) {
    g_sndCmdArgs[0] = (u8)arg0;
    func_8001A1E8(0xD4);
}

/** @brief Stores arg0 and u8-masked arg1 to SPU command buffer, sends command 0xD5.
 *  @param arg0 First command parameter.
 *  @param arg1 Second parameter, masked to 8 bits.
 */
void sndCmdD5(s32 arg0, s32 arg1) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = (u8)arg1;
    func_8001A1E8(0xD5);
}

/** @brief Stores arg0, 8-bit masked arg1, and 8-bit masked arg2 to SPU command buffer, sends command 0xD6.
 *  @param arg0 First command parameter.
 *  @param arg1 Second parameter, masked to 8 bits.
 *  @param arg2 Third parameter, masked to 8 bits.
 */
void sndCmdD6(s32 arg0, s32 arg1, s32 arg2) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1 & 0xFF;
    g_sndCmdArgs[2] = arg2 & 0xFF;
    func_8001A1E8(0xD6);
}

/** @brief Stores u8-masked arg0 to SPU command buffer, sends command 0xD8.
 *  @param arg0 Command parameter, masked to 8 bits.
 */
void sndCmdD8(s32 arg0) {
    g_sndCmdArgs[0] = (u8)arg0;
    func_8001A1E8(0xD8);
}

/** @brief Stores arg0 and u8-masked arg1 to SPU command buffer, sends command 0xD9.
 *  @param arg0 First command parameter.
 *  @param arg1 Second parameter, masked to 8 bits.
 */
void sndCmdD9(s32 arg0, s32 arg1) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = (u8)arg1;
    func_8001A1E8(0xD9);
}

/** @brief Stores arg0, 8-bit masked arg1, and 8-bit masked arg2 to SPU command buffer, sends command 0xDA.
 *  @param arg0 First command parameter.
 *  @param arg1 Second parameter, masked to 8 bits.
 *  @param arg2 Third parameter, masked to 8 bits.
 */
void sndCmdDA(s32 arg0, s32 arg1, s32 arg2) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = arg1 & 0xFF;
    g_sndCmdArgs[2] = arg2 & 0xFF;
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
 * @param srcAddr Source address of sample data in main RAM.
 * @param mode Non-zero waits for the SPU transfer to finish.
 */
s32 sndUploadSamples(SndBankDesc *srcAddr, s32 mode) {
    s32 result;
    do {
        result = sndUploadSample(srcAddr, mode);
    } while (result == 1);
    return result;
}


/**
 * @brief Choose the SPU transfer level/address for a sound bank and track it.
 *
 * Clears the pending flag (bit 0x10) of @c D_80077288 and, when no sequence is
 * playing (@c g_sndSeqState->instParams == 0), clears engine flags 0x600. A
 * bank is small enough to load when its @c sampleCount is < 0x21 and its
 * @c dataSize is <= 0x20000 and the busy bit (0x10) was clear; otherwise it
 * sends @c sndCmd11(0) and the alternate region is not used.
 *
 * The active region is chosen from the engine flags (@c field00 bits 0x200 /
 * 0x400) and the @c D_80077288[1] reservation bit (0x10000):
 *   - region 0 -> level 0x40, address 0x2B000, slot @c D_80073DE0[0]
 *   - region 1 -> level 0x60, address 0x4B000, slot @c D_80073DE0[1]
 * The bank's @c bankId is recorded into its region slot; if it already matches
 * the other slot, that slot is cleared. When the bank is too large the busy bit
 * is re-set in @c D_80077288.
 *
 * @param desc     Sound bank descriptor (@c SndBankDesc *).
 * @param outLevel Out: chosen SPU transfer level (0x40 or 0x60).
 * @param outAddr  Out: chosen SPU transfer address (0x2B000 or 0x4B000).
 * @return Region index: 0 for the primary region, 1 for the alternate.
 */
s32 sndSelectBankSlot(SndBankDesc *desc, s32 *outLevel, s32 *outAddr) {
    s32 flag;
    s32 ctrl;
    SoundMusicConfig *seq;

    ctrl = D_80077288[0];
    seq = g_sndSeqState;
    D_80077288[0] = ctrl & ~0x10;

    if (seq->unk04 == 0) {
        seq->unk00 &= ~0x600;
    }

    flag = 0;
    if (desc->sampleCount >= 0x21 || desc->dataSize > 0x20000 || (ctrl & 0x10)) {
        sndCmd11(0);
        D_80077288[1] &= ~0x10000;
    } else {
        s32 f = g_sndSeqState->unk00;
        if ((f & 0x200) || (!(f & 0x400) && (D_80077288[1] & 0x10000))) {
            flag = 1;
            D_80077288[1] &= ~0x10000;
        } else {
            D_80077288[1] |= 0x10000;
        }
    }

    if (flag == 0) {
        *outLevel = 0x40;
        *outAddr = 0x2B000;
        D_80073DE0[0] = desc->bankId;
        if (D_80073DE0[1] == desc->bankId) {
            D_80073DE0[1] = 0;
        }
    } else {
        *outLevel = 0x60;
        *outAddr = 0x4B000;
        D_80073DE0[1] = desc->bankId;
        if (D_80073DE0[0] == desc->bankId) {
            D_80073DE0[0] = 0;
        }
    }

    if (desc->sampleCount >= 0x21 || desc->dataSize > 0x20000) {
        D_80077288[0] |= 0x10;
    }

    return flag;
}

/** @brief Returns the value of global D_80074ED4. */
s32 sndGetEngineState(void) {
    return D_80074ED4;
}

/** @brief Clears D_8007735C, sets bit 0x1 in D_80077288, returns 0. */
s32 sndResetState(void) {
    D_8007735C = 0;
    D_80077288[0] |= 0x1;
    return 0;
}

/**
 * @brief Stream a sound bank to SPU in chunks (the streaming/DMA driver).
 *
 * Runs only while streaming is active (@c D_80077288 bit 0). On the first chunk
 * (when @c D_80077358 has no pending transfer) it validates the bank, derives
 * the SPU level/address via @c sndSelectBankSlot, parses the header into
 * @c D_80074FE8 via @c func_8001A57C, and seeds the @c D_80077358 streaming
 * state (instrument table pointer, SPU address, and the decode/DMA byte
 * counts). It then decodes up to the remaining bank-decode bytes through
 * @c func_800146F0 and DMAs up to the remaining SPU bytes via
 * @c sndDmaWriteSpu, advancing the source pointer and counters. When @p waitDma is
 * set it waits for the DMA. The streaming-active bit is cleared once the bank
 * finishes or when @c D_80077360 is 0.
 *
 * @param srcPtr Source pointer into the sound-bank data.
 * @param numBytes Number of bytes to process this call.
 * @param waitDma If nonzero, wait for the SPU DMA to complete (@c sndDmaWait).
 * @return @c D_80077360 (the active streaming handle, 0 when idle).
 *
 */
s32 sndStreamBank(u8 *srcPtr, u32 numBytes, s32 waitDma) {
    s32 val1;
    s32 val2;

    if (D_80077288[0] & 1) {
        if (D_80077358.spuAddr == 0) {
            if (sndValidateBank((u32 *)srcPtr) == 0) {
                sndSelectBankSlot(srcPtr, &val1, &val2);
                func_8001A57C((u32 *)srcPtr, (u32 *)D_80074FE8, 0x40);
                srcPtr += 0x40;
                D_80074FE8[4] = val2;
                D_80077358.spuAddr = val2;
                D_80074FE8[6] = val1;
                numBytes -= 0x40;
                D_80077358.spuBytes = D_80074FE8[5];
                D_80077358.decodePtr = (s32)&D_80073E68[val1];
                D_80077358.decodeBytes = D_80074FE8[7] << 4;
            } else {
                numBytes = 0;
                D_80077358.spuBytes = 0;
                D_80077358.decodeBytes = 0;
            }
        }

        if (D_80077358.decodeBytes != 0) {
            u32 chunk = D_80077358.decodeBytes;
            if (chunk >= numBytes) {
                chunk = numBytes;
            }
            /* Spill the clamped count to val2 (dead since the setup block)
               so it survives the decode call; the compiler reloads it for
               the post-call counter updates. */
            val2 = chunk;
            func_800146F0(srcPtr, D_80077358.decodePtr, D_80074FF8, chunk >> 4);
            srcPtr += ((u32)val2 >> 2) << 2;
            numBytes -= val2;
            D_80077358.decodePtr += ((u32)val2 >> 4) << 4;
            D_80077358.decodeBytes -= val2;
        }

        if (numBytes != 0 && D_80077358.spuBytes == 0) {
            /* Out of SPU bytes mid-bank: stop streaming. */
            D_80077288[0] &= ~1;
        } else {
            if (numBytes != 0) {
                u32 n = D_80077358.spuBytes;
                if (n >= numBytes) {
                    n = numBytes;
                }
                numBytes = n;
                SpuSetTransferStartAddr(D_80077358.spuAddr);
                sndDmaWriteSpu(srcPtr, numBytes);
                D_80077358.spuAddr += numBytes;
                D_80077358.spuBytes -= numBytes;
                if (waitDma != 0) {
                    sndDmaWait();
                }
            }
            /* Stop streaming once this bank finishes (D_80077360 idle). */
            if (D_80077360 == 0) {
                D_80077288[0] &= ~1;
            }
        }
    }
    return D_80077360;
}

/**
 * @brief Process audio parameters via sndSelectBankSlot and dispatch to func_800148B0.
 *
 * Calls sndSelectBankSlot to compute two intermediate values (stored on the stack),
 * then passes those along with the original parameters to func_800148B0.
 * Returns the result of sndSelectBankSlot, not func_800148B0.
 *
 * @param bank First parameter (passed through to both calls).
 * @param arg1 Second parameter (passed through to func_800148B0).
 * @return Result of sndSelectBankSlot.
 */
s32 sndProcessAudio(s32 bank, s32 arg1) {
    s32 val1, val2;
    s32 result = sndSelectBankSlot(bank, &val1, &val2);
    func_800148B0(bank, arg1, val1, val2);
    return result;
}

/**
 * @brief Validate/track a sound bank and dispatch its SPU transfer (command via
 *        func_800148B0), choosing the transfer address from engine state.
 *
 * If a sequence is active (@c g_sndSeqState->field5E nonzero), defers the
 * address/level computation to @c sndSelectBankSlot. Otherwise picks a fixed SPU
 * region based on the current transfer address @c D_80074968:
 *   - @c 0x5D000  -> level 0x40, address 0x2B000, slot @c D_80073DE0[0]
 *   - otherwise   -> level 0x60, address 0x4B000, slot @c D_80073DE0[1]
 * The bank's @c bankId is written into its region slot; if it already matches
 * the other slot, that slot is cleared. The chosen level/address are forwarded
 * to @c func_800148B0.
 *
 * @param desc Sound bank descriptor (@c SndBankDesc *).
 * @param arg1 Passthrough parameter forwarded to @c func_800148B0.
 * @return @c sndSelectBankSlot's result when a sequence is active; otherwise 0 for
 *         the 0x5D000 region or 1 for the alternate region.
 */
s32 sndUploadBank(SndBankDesc *desc, s32 arg1) {
    s32 level;
    s32 addr;
    s32 result;

    if (g_sndSeqState->unk5E != 0) {
        result = sndSelectBankSlot(desc, &level, &addr);
    } else if (D_80074968 == 0x5D000) {
        result = 0;
        D_80073DE0[0] = desc->bankId;
        level = 0x40;
        addr = 0x2B000;
        if (D_80073DE0[1] == desc->bankId) {
            D_80073DE0[1] = 0;
        }
    } else {
        result = 1;
        D_80073DE0[1] = desc->bankId;
        level = 0x60;
        addr = 0x4B000;
        if (D_80073DE0[0] == desc->bankId) {
            D_80073DE0[0] = 0;
        }
    }
    func_800148B0((u32 *)desc, arg1, level, addr);
    return result;
}

/**
 * @brief Configure audio playback address based on sound structure state.
 *
 * If the sound structure has active fields (field04|field1C nonzero) and
 * field00 bit 10 is set, uses address 0x3D000; otherwise uses 0x5D000.
 * Calls func_800148B0 with the determined address and level 0xB0.
 *
 * @param arg0 First parameter (passed through to func_800148B0).
 * @param arg1 Second parameter (passed through to func_800148B0).
 */
void sndSetPlaybackAddr(s32 arg0, s32 arg1) {
    SoundMusicConfig *ptr = g_sndSeqState;
    s32 addr = 0x5D000;
    if ((ptr->unk04 | ptr->unk1C) && (ptr->unk00 & 0x400)) {
        addr = 0x3D000;
    }
    func_800148B0(arg0, arg1, 0xB0, addr);
}

/**
 * @brief Clears matching bank-ID entries and uploads a sound bank (variant of
 *        func_80014190 with a different SPU address/level set).
 *
 * Scans the 6-entry bank-ID table @c D_80074EB8 and zeroes every slot whose
 * value equals @c a0->bankId. Then, based on @p a1, selects an SPU transfer
 * address and level (1 -> 0x51000/0x90, 2 -> 0x57000/0xA0, else 0x4B000/0x80),
 * records @c a0->bankId into the corresponding slot (@c D_80074EBC / @c D_80074EC0
 * / @c D_80074EB8), adjusts the address by -0x20000 when the engine is active
 * with flag bit 10 set (@c g_sndSeqState), and dispatches via @c func_800148B0.
 *
 * @param desc Sound bank descriptor (@c SndBankDesc *); @c bankId is at offset 0x04.
 * @param mode Mode selector (1, 2, or default) choosing the SPU address/level.
 * @param arg2 Passthrough parameter forwarded to @c func_800148B0.
 */
void func_80014094(SndBankDesc *desc, s32 mode, s32 arg2) {
    SndBankDesc *p = desc;
    u32 i;
    s32 *q;
    s32 vol;
    s32 addr;
    SoundMusicConfig *ptr;

    for (i = 0, q = D_80074EB8; i < 6; i++, q++) {
        if (*q == p->bankId) {
            *q = 0;
        }
    }

    switch (mode) {
    case 1:
        addr = 0x51000;
        vol = 0x90;
        D_80074EBC = p->bankId;
        break;
    case 2:
        addr = 0x57000;
        vol = 0xA0;
        D_80074EC0 = p->bankId;
        break;
    default:
        addr = 0x4B000;
        vol = 0x80;
        D_80074EB8[0] = p->bankId;
        break;
    }

    ptr = g_sndSeqState;
    /* Read unk00 through the loop pointer q (it is dead here); reusing it
       keeps q live past the loop, which is what pins the loop's counter to
       $a3 and the array pointer to $t0 to match the original codegen. */
    q = &ptr->unk00;
    if ((ptr->unk04 | ptr->unk1C) && (*q & 0x400)) {
        addr -= 0x20000;
    }
    func_800148B0((u32 *)desc, arg2, vol, addr);
}

/**
 * @brief Clears any matching bank-ID entries and uploads a sound bank.
 *
 * Scans the 6-entry bank-ID table @c D_80074EB8 and zeroes every slot whose
 * value equals @c a0->bankId. Then, based on @p a1, selects an SPU transfer
 * address and level (1 -> 0x6F800/0xE0, 2 -> 0x74000/0xF0, else 0x6B000/0xD0),
 * records @c a0->bankId into the corresponding slot (@c D_80074EC8 / @c D_80074ECC
 * / @c D_80074EC4), and dispatches the transfer via @c func_800148B0.
 *
 * @param desc Sound bank descriptor (@c SndBankDesc *); @c bankId is at offset 0x04.
 * @param mode Mode selector (1, 2, or default) choosing the SPU address/level.
 * @param arg2 Passthrough parameter forwarded to @c func_800148B0.
 */
void func_80014190(SndBankDesc *desc, s32 mode, s32 arg2) {
    SndBankDesc *p = desc;
    u32 i;
    s32 vol;

    for (i = 0; i < 6; i++) {
        if (D_80074EB8[i] == p->bankId) {
            D_80074EB8[i] = 0;
        }
    }

    /* The selected SPU address reuses the now-dead loop counter i: in the
       original it shares $a3 with the counter, so one variable carries both
       (the twin func_80014094 instead frees $a3 via its trailing pointer
       read). Without the reuse the loop counter and array pointer swap
       registers. */
    switch (mode) {
    case 1:
        i = 0x6F800;
        vol = 0xE0;
        D_80074EC8 = p->bankId;
        break;
    case 2:
        i = 0x74000;
        vol = 0xF0;
        D_80074ECC = p->bankId;
        break;
    default:
        i = 0x6B000;
        vol = 0xD0;
        D_80074EC4 = p->bankId;
        break;
    }

    func_800148B0((u32 *)desc, arg2, vol, i);
}

/**
 * @brief Set CD audio volume mixing levels and apply via CdMix.
 *
 * If D_8007728C bit 1 is set, scales @p volume by a fixed factor
 * ((volume * 46448) >> 17) and sets all four attenuation bytes equally.
 * Otherwise, sets left-to-left and right-to-right = volume, crosstalk = 0.
 * Calls CdMix to apply the settings.
 *
 * @param volume Volume level (0-128).
 * @return Always 0.
 */
s32 sndSetCdMixVolume(s32 volume) {
    if (D_8007728C & 2) {
        CdlATV *atv = &g_cdMixVolume;
        s32 vol = (u32)(volume * 46448) >> 17;
        atv->val3 = vol;
        atv->val1 = vol;
        atv->val2 = vol;
        atv->val0 = vol;
    } else {
        CdlATV *atv = &g_cdMixVolume;
        atv->val2 = volume;
        atv->val0 = volume;
        atv->val3 = 0;
        atv->val1 = 0;
    }
    CdMix(&g_cdMixVolume);
    return 0;
}

/**
 * @brief Conditionally dispatch SPU command 0xE0 with three parameters.
 *
 * Calls sndValidateBank first; if it returns 0, stores a0, (a1 & 0xFF) << 8,
 * and a2 into the SPU command buffer and dispatches command 0xE0.
 *
 * @param bank First word written to g_sndCmdArgs.
 * @param instrument Masked and shifted into second word of command buffer.
 * @param arg2 Third word of command buffer.
 */
void sndCmdE0(s32 bank, s32 instrument, s32 arg2) {
    if (sndValidateBank((u32 *)bank) == 0) {
        g_sndCmdArgs[0] = bank;
        g_sndCmdArgs[1] = (instrument & 0xFF) << 8;
        g_sndCmdArgs[2] = arg2;
        func_8001A1E8(0xE0);
    }
}

/** @brief Sends SPU command 0xE2. */
void sndCmdE2(void) {
    func_8001A1E8(0xE2);
}

/** @brief Stores 7-bit masked value, shifted left 8, to SPU command buffer. Sends command 0xE4.
 *  @param value Parameter, masked to 7 bits then shifted left 8.
 */
void sndCmdE4(s32 value) {
    g_sndCmdArgs[0] = (value & 0x7F) << 8;
    func_8001A1E8(0xE4);
}

/**
 * @brief Write arg0 to SPU command buffer, write masked/shifted value to second word, dispatch 0xE5.
 * @param arg0 First word stored directly to g_sndCmdArgs.
 * @param value Second parameter, masked to 7 bits and shifted left 8.
 */
void sndCmdE5(s32 arg0, s32 value) {
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = (value & 0x7F) << 8;
    func_8001A1E8(0xE5);
}

/** @brief Stores u8-masked value, shifted left 8, to SPU command buffer. Sends command 0xE6.
 *  @param value Parameter, masked to 8 bits then shifted left 8.
 */
void sndCmdE6(s32 value) {
    g_sndCmdArgs[0] = (u8)value << 8;
    func_8001A1E8(0xE6);
}

/**
 * @brief Upload instrument sample data to SPU RAM.
 *
 * Validates the sound bank, waits for DMA, determines the SPU target
 * address (0x51000 or 0x4B000 based on slot, adjusted by -0x20000 if
 * bit 10 of the sound engine flags is set), then transfers sample data
 * and copies voice parameters to D_80073C38.
 *
 * @param bank Pointer to sound bank descriptor (SndBankDesc).
 * @param slot Bank slot selector (0 = primary, nonzero = alternate).
 * @return 0 on success, nonzero error code on validation failure.
 */
s32 sndUploadSampleBank(u8 *bank, s32 slot) {
    s32 spuAddr;
    s32 result;
    SndBankDesc *desc;

    result = sndValidateBank(bank);
    if (result == 0) {
        sndDmaWait();
        spuAddr = 0x51000;
        if (slot == 0) {
            spuAddr = 0x4B000;
        }
        {
            SoundMusicConfig *ptr = g_sndSeqState;
            /* desc = bank is set in both branches below rather than hoisted after
               the if: the original stores the descriptor pointer twice, and
               collapsing it to a single assignment drops the match. */
            if ((ptr->unk04 | ptr->unk1C) && (ptr->unk00 & 0x400)) {
                spuAddr -= 0x20000;
                desc = (SndBankDesc *)bank;
            } else {
                desc = (SndBankDesc *)bank;
            }
            bank += 0x40;
            SpuSetTransferStartAddr(spuAddr);
            sndDmaWriteSpu(bank, desc->spuAddr);
            desc->spuLoadedAddr = spuAddr;
            func_8001A57C((u32 *)desc, (u32 *)&D_80073C38, 0x70);
        }
    } else {
        D_80073C58 = 0;
    }
    return result;
}

/**
 * @brief Write masked volume to SPU command buffer and dispatch.
 * @param volume Volume value (masked to 8 bits, shifted left 8).
 * @param arg1 Secondary parameter stored at g_sndCmdArgs[1].
 */
void sndCmdED(s32 volume, s32 arg1) {
    g_sndCmdArgs[0] = (volume & 0xFF) << 8;
    g_sndCmdArgs[1] = arg1;
    func_8001A1E8(0xED);
}

/**
 * @brief Validate bank and dispatch SPU command 0xEC with encoded parameters.
 *
 * Validates the sound bank at @p a0. If valid, determines SPU address
 * (0x51000 or 0x4B000, adjusted if engine flag bit 10 set), then writes
 * a0, (a1 & 0xFF) << 8, spuAddr, and a3 to g_sndCmdArgs and dispatches
 * command 0xEC.
 *
 * @param bank Sound bank address.
 * @param instrument Instrument parameter (masked to 8 bits, shifted left 8).
 * @param slot Bank slot selector (0 = primary, nonzero = alternate).
 * @param arg3 Additional parameter stored at g_sndCmdArgs[3].
 */
void sndCmdEC(s32 bank, s32 instrument, s32 slot, s32 arg3) {
    s32 spuAddr;
    SoundMusicConfig *ptr;

    if (sndValidateBank((u32 *)bank) == 0) {
        spuAddr = 0x51000;
        if (slot == 0) {
            spuAddr = 0x4B000;
        }
        ptr = g_sndSeqState;
        if ((ptr->unk04 | ptr->unk1C) && (ptr->unk00 & 0x400)) {
            spuAddr -= 0x20000;
        }
        g_sndCmdArgs[0] = bank;
        g_sndCmdArgs[1] = (instrument & 0xFF) << 8;
        g_sndCmdArgs[2] = spuAddr;
        g_sndCmdArgs[3] = arg3;
        func_8001A1E8(0xEC);
    }
}

/**
 * @brief Initialize SPU IRQ and sound engine state for playback.
 *
 * If @p value is zero, returns -1. Otherwise disables SPU IRQ, clears the IRQ
 * address, stores both parameters into the SPU command buffer, initializes
 * several sound engine counters in g_sndStream, sets the frame limit from
 * @p value >> 12, and issues command 0xE8 via func_8001A1E8.
 *
 * @param arg0 First SPU command parameter (stored at g_sndCmdArgs).
 * @param value Second SPU command parameter / frame limit source (stored at g_sndCmdArgs[1]).
 * @return 0 on success, -1 if @p value is zero.
 */
s32 sndInitIrq(s32 arg0, s32 value) {
    if (value == 0) {
        return -1;
    }
    SpuSetIRQ(0);
    SpuSetIRQAddr(0);
    g_sndCmdArgs[0] = arg0;
    g_sndCmdArgs[1] = value;
    g_sndStream.unk34 = -1;
    g_sndStream.unk20 = 0;
    g_sndStream.tickCounter = 0;
    g_sndStream.tickCount = 0;
    g_sndStream.frameCounter = 0;
    g_sndStream.loopLimit = (u32)value >> 12;
    func_8001A1E8(0xE8);
    return 0;
}

/**
 * @brief Advance sound engine tick and frame counters; trigger IRQ callback if needed.
 *
 * Increments @c tickCounter and wraps @c frameCounter back to 0 at
 * @c loopLimit. If bit 24 of @c flags is set and @c frameCounter reaches 3,
 * calls func_8001F118.
 *
 * @return The value of D_800772CC (current sound engine state word).
 */
s32 sndTickCounters(void) {
    SoundStream *stream = &g_sndStream;
    s32 counter;

    stream->tickCounter++;
    counter = stream->frameCounter + 1;
    stream->frameCounter = counter;
    if ((u32)counter > (u32)(stream->loopLimit - 1)) {
        stream->frameCounter = 0;
    }
    if ((stream->flags & 0x1000000) && (u32)stream->frameCounter >= 3) {
        func_8001F118();
    }
    return D_800772CC;
}

