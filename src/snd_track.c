#include "common.h"
#include "psxsdk/libspu.h"
#include "sound.h"
#include "snd_dma.h"
#include "snd_init.h"

extern s32 D_80077288[];
extern s32 D_80074EE8[];
extern u8 D_80051824;

void sndStreamSetupMono(void);
void sndStreamCallbackMonoLow(void);
void sndStreamCallbackStereoLow(void);
void sndStreamCallbackStereoAltLow(void);
void sndStreamSetupStereoAlt(void);
void sndStreamSetupStereo(void);
void func_8001EB38(void);

/**
 * @brief Copy instrument parameters from a table entry into a track structure.
 *
 * Stores the initial instrument value, copies pitch and envelope fields
 * from the instrument entry, and sets the sustain/loop flags in the track.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Pointer to the instrument table entry (16 bytes).
 * @param a2 Initial instrument configuration value.
 */
void sndTrackSetInstrumentParams(SoundSeqTrack *track, SndInstrument *inst, s32 a2) {
    u16 v;
    track->sampleAddr = a2;
    track->sampleLoop = inst->loopAddr;
    track->adsrLow = inst->adsrLow;
    v = inst->adsrHigh;
    track->updateFlags |= 0x1FF80;
    track->adsrHigh = v;
}

extern SndInstrument D_80073E68[];

/**
 * @brief Applies an instrument table entry to a sequence track.
 *
 * Stores the instrument index at track offset +0x66, then looks up the
 * instrument in D_80073E68 and copies its parameters into the track.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Instrument table index to apply.
 */
void sndTrackApplyInstrument(SoundSeqTrack *track, s32 a1) {
    *(s16 *)&track->instrument = a1;
    sndTrackSetInstrumentParams((s32)track, (s32)&D_80073E68[a1], D_80073E68[a1].sampleAddr);
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
void sndTrackClearVoiceBits(SoundSeqTrack *track, s32 a1) {
    s32 mask = ~a1;
    D_80075028.activeMask &= mask;
    D_80075028.activeMaskStored &= mask;
    D_80075028.noiseMask &= mask;
    D_80075028.reverbMask &= mask;
    D_80075028.pitchLfoMask &= mask;
    D_80075028.unk04 &= mask;
    D_80075028.unk08 &= mask;
    track->keyOnMask = 0;
    track->keyOffMask = 0;
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
s32 sndTrackTransposePercussion(s32 a0, s32 a1) {
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

/**
 * @brief Resets a sequence track and clears voice bits from all SPU control fields.
 *
 * If the track's voice channel is 0 (primary), directly clears the voice
 * bits from multiple fields in the g_sndSeqState state structure. If the
 * remaining active voice mask becomes zero, also clears the sequence ID
 * and playback state. Otherwise delegates to sndTrackClearVoiceBits.
 * Zeroes the track flags and marks the SPU dirty flags.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to clear.
 */
void func_8001C2C8(SoundSeqTrack *track, u32 mask) {
    u32 keep;

    if (track->playingType == 0) {
        keep = ~mask;
        g_sndSeqState->activeMask &= keep;
        if (g_sndSeqState->activeMask == 0) {
            D_80074EB0 = 0;
            g_sndSeqState->musicId = 0;
            g_sndSeqState->unk00 = 0;
        }
        g_sndSeqState->unk14 &= keep;
        g_sndSeqState->unk08 &= keep;
        g_sndSeqState->unk0C &= keep;
        g_sndSeqState->noiseMask &= keep;
        g_sndSeqState->reverbMask &= keep;
        g_sndSeqState->pitchLfoMask &= keep;
    } else {
        sndTrackClearVoiceBits(track, mask);
    }
    track->flags = 0;
    D_80077288[2] |= 0x110;
}

/**
 * @brief Read 2 bytes from stream, build 32-bit tempo value, store to bank.
 *
 * Reads byte0 from stream, shifts left 16 and stores to g_sndSeqState+0x20.
 * Then reads byte1, shifts left 24, ORs with previous value, stores again.
 * Advances stream pointer by 2. Clears halfword at g_sndSeqState+0x5C.
 *
 * @param a0 Pointer to stream state (first word is stream cursor).
 */
void sndTrackReadTempo(SoundSeqTrack *track) {
    s32 byte0 = *(u8 *)(*(s32 *)track) << 16;
    SoundMusicConfig *bank = g_sndSeqState;
    bank->tempo = byte0;
    byte0 |= *(u8 *)(*(s32 *)track + 1) << 24;
    bank->tempo = byte0;
    *(s32 *)track = *(s32 *)track + 2;
    bank->tempoSlideSteps = 0;
}

void func_8001C3E8(SoundSeqTrack *track) {
    u8 *ptr;
    s32 tempo;

    g_sndSeqState->tempoSlideSteps = *track->seqPtr++;
    if (g_sndSeqState->tempoSlideSteps == 0) {
        g_sndSeqState->tempoSlideSteps = 0x100;
    }
    ptr = track->seqPtr;
    tempo = ptr[0] << 16;
    tempo |= ptr[1] << 24;
    track->seqPtr = ptr + 2;
    g_sndSeqState->tempo &= 0xFFFF0000;
    g_sndSeqState->tempoSlideStep = (tempo - g_sndSeqState->tempo) / g_sndSeqState->tempoSlideSteps;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C490);

void func_8001C530(SoundSeqTrack *track) {
    u8 *ptr;
    s32 depth;

    g_sndSeqState->reverbDepthSlideSteps = *track->seqPtr++;
    if (g_sndSeqState->reverbDepthSlideSteps == 0) {
        g_sndSeqState->reverbDepthSlideSteps = 0x100;
    }
    ptr = track->seqPtr;
    depth = (((s8 *)ptr)[1] << 20) | (ptr[0] << 12);
    track->seqPtr = ptr + 2;
    g_sndSeqState->reverbDepth &= ~0xFFF;
    g_sndSeqState->reverbDepthSlideStep = (depth - g_sndSeqState->reverbDepth) / g_sndSeqState->reverbDepthSlideSteps;
}

/** @brief Reads a signed 16-bit little-endian offset from the stream cursor and advances cursor by that offset.
 *  @param a0 Pointer to stream state (a0[0] = cursor pointer).
 */
void sndTrackBranch(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 offset = (s16)(ptr[0] | (ptr[1] << 8));
    *(u8 **)track = ptr + offset;
}

/**
 * @brief Conditional branch in music sequence: reads voice index and optional offset.
 *
 * Reads a byte from the stream. If the byte is within the valid voice count
 * (g_sndSeqState+0x60), reads a 16-bit signed offset from the next two bytes
 * and jumps the stream cursor by that offset. Otherwise skips the 2-byte
 * offset field. Both paths end with storing the new cursor and returning.
 *
 * @param a0 Pointer to the stream state (cursor at +0x00).
 */
/**
 * @brief Conditional branch in music sequence: reads voice index and optional offset.
 *
 * Reads a byte from the stream. If the byte is within the valid voice count
 * (g_sndSeqState+0x60), reads a 16-bit signed offset from the next two bytes
 * and jumps the stream cursor by that offset. Otherwise skips the 2-byte
 * offset field.
 *
 * @param a0 Pointer to the stream state (cursor at +0x00).
 */
void func_8001C604(SoundSeqTrack *track) {
    SoundMusicConfig *bank = g_sndSeqState;
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    ptr++;
    *(u8 **)track = ptr;
    if (!(bank->condition < val)) {
        s32 offset = (s16)(ptr[0] | (ptr[1] << 8));
        *(u8 **)track = ptr + offset;
    } else {
        *(u8 **)track = ptr + 2;
    }
}

/** @brief Reads one byte from stream, advances cursor, ORs 3 into flags, stores byte << 8 as halfword at +0x80.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadPitchBend(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    u8 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->updateFlags |= 3;
    track->pitchBend = val << 8;
}

/**
 * @brief Reads pitch bend target and fade counter from the stream.
 *
 * Reads a byte for the fade counter (0 becomes 0x100), then reads a
 * second byte as the target pitch bend value. Computes the per-tick
 * delta as (target - current) / counter and stores it.
 *
 * @param a0 Pointer to the track structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C684);

/**
 * @brief Reads one byte from stream, advances cursor, ORs 3 into flags,
 *        clears halfwords at +0x86 and +0x88, stores byte << 23 at +0x44.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadPan(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *(s8 *)ptr;
    s32 flags = track->updateFlags | 3;
    *(u8 **)track = ptr + 1;
    track->volSlideSteps = 0;
    track->updateFlags = flags;
    track->pitchValueSlideSteps = 0;
    track->volumeLevel = val << 23;
}

void func_8001C738(SoundSeqTrack *track) {
    u8 *ptr = track->seqPtr;
    s32 steps = *ptr;
    s32 slide;

    track->seqPtr = ptr + 1;
    track->volSlideSteps = steps;
    if (steps == 0) {
        track->volSlideSteps = 0x100;
    }
    slide = *(s8 *)track->seqPtr++ << 23;
    track->volumeLevel &= 0xFFFF0000;
    track->volSlideStep = (slide - track->volumeLevel) / track->volSlideSteps;
    track->pitchValueSlideSteps = 0;
}

void func_8001C7C4(SoundSeqTrack *track) {
    s32 steps;
    s32 slide;

    track->pitchValue = *(s8 *)track->seqPtr++ << 23;
    steps = *track->seqPtr++;
    track->pitchValueSlideSteps = steps;
    if (steps == 0) {
        track->pitchValueSlideSteps = 0x100;
    }
    slide = *(s8 *)track->seqPtr++ << 23;
    track->pitchValueSlideStep = (slide - track->pitchValue) / track->pitchValueSlideSteps;
}

/** @brief Sets bit 6 (0x40) in the flags word at offset 0x30 of a0. */
void sndTrackSetLegato(SoundSeqTrack *track) {
    track->flags |= 0x40;
}

/** @brief Clears bit 6 (0x40) in the flags word at offset 0x30 of a0. */
void sndTrackClearLegato(SoundSeqTrack *track) {
    track->flags &= ~0x40;
}

/** @brief Increments the word at a0 by 2. */
void sndTrackSkip2Bytes(s32 *a0) {
    *a0 += 2;
}

/** @brief Empty stub — no operation. */
void sndTrackNop1(void) {
}

/**
 * @brief Reads one byte from stream, advances cursor. Clears halfword at +0x8A,
 *        stores byte << 8 at +0xDE. If bit 8 of flags at +0x30 is set, ORs 3
 *        into flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadExpression(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    s32 flags30;
    *(u8 **)track = ptr + 1;
    flags30 = track->flags;
    track->volBalanceSlideSteps = 0;
    track->volBalance = (val << 8) & 0xFFFF;
    if (flags30 & 0x100) {
        track->updateFlags = track->updateFlags | 3;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C8DC);

/**
 * @brief Reads a note byte from the stream and sets pitch/flag fields.
 *
 * Reads one byte from the stream cursor, advances it, sets flag bits 0-1
 * at offset +0xF8, clears +0x8E, and stores (byte + 0x40) << 8 at +0x8C.
 *
 * @param a0 Pointer to the track structure (stream ptr at +0x00).
 */
void sndTrackReadNotePitch(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    u32 val = *ptr;
    s32 flags = track->updateFlags;
    *(u8 **)track = ptr + 1;
    track->volPanSlideSteps = 0;
    track->updateFlags = flags | 3;
    val += 0x40;
    val &= 0xFF;
    val <<= 8;
    track->volPan = val;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C99C);

/**
 * @brief Reads one byte from stream, advances cursor, stores byte as
 *        halfword at +0x92.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadPanpot(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    u16 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->panpot = val;
}

/** @brief Increments halfword at a0+0x92, wraps modulo 16. */
void sndTrackIncPanpot(SoundSeqTrack *track) {
    track->panpot = (track->panpot + 1) & 0xF;
}

/** @brief Decrements halfword at a0+0x92, wraps modulo 16. */
void sndTrackDecPanpot(SoundSeqTrack *track) {
    track->panpot = (track->panpot - 1) & 0xF;
}

/**
 * @brief Read instrument index from stream with optional transposition.
 *
 * Reads one byte from the stream cursor. If the track's voice count
 * (offset +0x60) is zero, resolves the instrument via sndAdjustNoteOctave.
 * Otherwise transposes via sndTrackTransposePercussion using the track's flags.
 * Applies the resolved instrument via sndTrackSetInstrumentParams, stores the
 * instrument index, clears field 0x10A, and masks field 0x30.
 *
 * @param a0 Pointer to the track structure.
 */
void sndTrackReadInstrumentTransposed(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 byte = *ptr;
    s32 inst;
    *(u8 **)track = ptr + 1;

    if (track->playingType == 0) {
        inst = sndAdjustNoteOctave(g_sndSeqState->unk00, byte);
    } else {
        inst = sndTrackTransposePercussion(track->bankPtr, byte);
    }
    sndTrackSetInstrumentParams(track, &D_80073E68[inst], D_80073E68[inst].sampleAddr);
    track->instrument = inst;
    track->instOverride = 0;
    track->flags &= (s32)0xE6FFEFF7;
}

/**
 * @brief Read instrument index from stream and apply to track.
 *
 * Reads one byte from the stream cursor, resolves the instrument via
 * sndAdjustNoteOctave, applies the instrument table entry via sndTrackSetInstrumentParams,
 * stores the instrument index, clears field 0x10A, and masks field 0x30.
 *
 * @param a0 Pointer to the track structure.
 */
void sndTrackReadInstrument(SoundSeqTrack *track) {
    SoundMusicConfig *bankPtr = g_sndSeqState;
    u8 *ptr = *(u8 **)track;
    s32 byte = *ptr;
    s32 inst;
    *(u8 **)track = ptr + 1;
    inst = sndAdjustNoteOctave(bankPtr->unk00, byte);
    sndTrackSetInstrumentParams(track, &D_80073E68[inst], 0x1010);
    track->instrument = inst;
    track->instOverride = 0;
    track->flags &= (s32)0xE6FFEFF7;
}

/**
 * @brief Reads byte from stream, looks up instrument override in bank pointer table.
 *
 * If g_sndSeqState+0x30 (bank pointer) is zero, returns. Otherwise looks up
 * the byte as an index in the table. If the entry is negative, clears
 * instOverride and masks flags. Otherwise stores the pointer, sets 0xE8
 * to 0xFF, masks and ORs 0x1000 into flags.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001CBA4(SoundSeqTrack *track) {
    u8 idx = *track->seqPtr++;
    s16 *table = g_sndSeqState->unk30;
    s32 offset;

    if (table != NULL) {
        offset = table[idx];
        if (offset < 0) {
            track->instOverride = 0;
            track->flags &= ~0x1000;
        } else {
            track->drumOffset = (u8 *)table + offset + 0x20;
            track->unkE8 = 0xFF;
            track->flags = (track->flags & 0xE6FFEFF7) | 0x1000;
        }
    }
}

/** @brief Looks up D_80073E68 table by a0[0x66] index, copies fields to a0, ORs flags, masks a0[0x30].
 *  @param a0 Pointer to stream state.
 */
void sndTrackRefreshEnvelope(SoundSeqTrack *track) {
    SndInstrument *inst = &D_80073E68[track->instrument];
    track->adsrLow = inst->adsrLow;
    track->adsrHigh = inst->adsrHigh;
    track->updateFlags |= 0xFF00;
    track->flags &= (s32)0xE6FFFFFF;
}

/** @brief Reads one byte from stream, sign-extends to s8, stores as halfword at a0+0xE4.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadDetune(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s8 val = *ptr;
    *(u8 **)track = ptr + 1;
    *(s16 *)&track->detune = val;
}

/** @brief Reads one byte from stream, sign-extends, adds to halfword at a0+0xE4.
 *  @param a0 Pointer to stream state.
 */
void sndTrackAdjustDetune(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s8 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->detune = track->detune + val;
}

/**
 * @brief Reads duration and delta from stream and stores to track fields.
 *
 * Reads a byte for duration, storing at +0x94 (or 0x100 if the byte was 0).
 * Then reads a signed byte for delta, storing sign-extended at +0xEA.
 *
 * @param a0 Pointer to the track structure.
 */
void sndTrackReadDurationDelta(SoundSeqTrack *track) {
    u8 *ptr;
    s32 val;

    ptr = *(u8 **)track;
    val = *ptr;
    *(u8 **)track = ptr + 1;
    track->durationDelta = val;
    if (val == 0) {
        track->durationDelta = 0x100;
    }
    ptr = *(u8 **)track;
    val = *ptr;
    *(u8 **)track = ptr + 1;
    val <<= 24;
    val >>= 24;
    track->durationDelta2 = val;
}

/** @brief Reads one byte from stream as duration. If zero, stores 0x100. Clears tempo/timer fields.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadDuration(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->duration = val;
    if (val != 0) goto skip;
    track->duration = 0x100;
skip:
    track->ecField = 0;
    track->durationCounter = 0;
    track->timerActive = 1;
}

/** @brief Clears the halfword at offset 0x98 of a0. */
void sndTrackClearDuration(SoundSeqTrack *track) {
    track->duration = 0;
}

void func_8001CD50(SoundSeqTrack *track) {
    u32 scaled;

    track->fineTuning = (s8)*track->seqPtr++;
    scaled = track->basePitch * (u8)track->fineTuning;
    if (track->fineTuning < 0) {
        scaled = (scaled >> 8) - track->basePitch;
    } else {
        scaled >>= 7;
    }
    track->fineTunePitch = scaled;
    track->updateFlags |= 0x10;
}

void func_8001CDB0(SoundSeqTrack *track) {
    u32 scaled;

    track->fineTuning += (s8)*track->seqPtr++;
    scaled = track->basePitch * (u8)track->fineTuning;
    if (track->fineTuning < 0) {
        scaled = (scaled >> 8) - track->basePitch;
    } else {
        scaled >>= 7;
    }
    track->fineTunePitch = scaled;
    track->updateFlags |= 0x10;
}

void func_8001CE14(SoundSeqTrack *track) {
    s32 depth;
    s32 rate;
    s32 pitch;
    u32 amplitude;
    u32 base;

    track->flags |= 1;
    if (track->playingType != 0) {
        track->vibratoDelay = 0;
        depth = *track->seqPtr++;
        if (depth != 0) {
            track->vibratoDepth = depth << 8;
        }
    } else {
        track->vibratoDelay = *track->seqPtr++;
    }
    rate = *track->seqPtr++;
    track->vibratoRate = rate;
    if (rate == 0) {
        track->vibratoRate = 0x100;
    }
    track->vibratoType = *track->seqPtr++;
    amplitude = (track->vibratoDepth & 0x7F00) >> 8;
    pitch = track->basePitch & 0xFFFF;
    if (!(track->vibratoDepth & 0x8000)) {
        base = amplitude * ((pitch * 15) >> 8);
    } else {
        base = amplitude * pitch;
    }
    track->vibratoBase = base >> 7;
    track->vibratoWave = D_80052258[track->vibratoType];
    track->vibratoDelayCur = track->vibratoDelay;
    track->vibratoRateCur = 1;
}

void func_8001CF0C(SoundSeqTrack *track) {
    u16 depth;
    u32 amplitude;
    s32 pitch;
    u32 base;

    track->vibratoDepth = *track->seqPtr++ << 8;
    pitch = track->basePitch;
    depth = track->vibratoDepth;
    amplitude = (depth & 0x7F00) >> 8;
    if (!(depth & 0x8000)) {
        base = amplitude * ((pitch * 15) >> 8);
    } else {
        base = amplitude * pitch;
    }
    track->vibratoBase = base >> 7;
}

/**
 * @brief Reads volume LFO fade parameters from the stream.
 *
 * Reads a byte for the fade counter (0 becomes 0x100), then reads the
 * next byte as target volume. Computes delta = (target << 8 - current) / counter.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001CF6C(SoundSeqTrack *track) {
    u16 steps;
    u8 *ptr = track->seqPtr;

    steps = *ptr++;
    track->seqPtr = ptr;
    if (steps == 0) {
        steps = 0x100;
    }
    track->vibratoDepthSlideStep = ((*ptr << 8) - track->vibratoDepth) / steps;
    track->vibratoDepthSlideSteps = steps;
    track->seqPtr = ptr + 1;
}

/** @brief Clears bit 0x1 in a0+0x30, sets bit 0x10 in a0+0xF8, zeroes a0+0xEE. */
void sndTrackStopPortamento(SoundSeqTrack *track) {
    track->flags &= ~0x1;
    track->updateFlags |= 0x10;
    track->portamento = 0;
}

void func_8001CFFC(SoundSeqTrack *track) {
    s32 depth;
    s32 rate;

    track->flags |= 2;
    depth = *track->seqPtr++;
    if (track->playingType != 0) {
        track->tremoloDelay = 0;
        if (depth != 0) {
            track->tremoloDepth = (depth & 0x7F) << 8;
        }
    } else {
        track->tremoloDelay = depth;
    }
    rate = *track->seqPtr++;
    track->tremoloRate = rate;
    if (rate == 0) {
        track->tremoloRate = 0x100;
    }
    track->tremoloType = *track->seqPtr++;
    track->tremoloWave = D_80052258[track->tremoloType];
    track->tremoloDelayCur = track->tremoloDelay;
    track->tremoloRateCur = 1;
}

/** @brief Reads one byte from stream, advances cursor, masks to 7 bits, shifts left 8, stores to halfword at a0+0xBC.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadVolumeLfo(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->tremoloDepth = (val & 0x7F) << 8;
}

void func_8001D0D0(SoundSeqTrack *track) {
    u8 *ptr = track->seqPtr;
    s32 steps = *ptr++;

    track->seqPtr = ptr;
    if (steps == 0) {
        steps = 0x100;
    }
    track->tremoloDepthSlideStep = (((*ptr & 0x7F) << 8) - track->tremoloDepth) / steps;
    track->tremoloDepthSlideSteps = steps;
    track->seqPtr = ptr + 1;
}

/** @brief Clears bit 0x2 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF0. */
void sndTrackStopVolumeLfo(SoundSeqTrack *track) {
    track->flags &= ~0x2;
    track->updateFlags |= 0x3;
    track->volumeLfoStop = 0;
}

void func_8001D164(SoundSeqTrack *track) {
    s32 rate;

    track->flags |= 4;
    rate = *track->seqPtr++;
    track->panLfoRate = rate;
    if (rate == 0) {
        track->panLfoRate = 0x100;
    }
    track->panLfoType = *track->seqPtr++;
    track->panLfoWave = D_80052258[track->panLfoType];
    track->panLfoRateCur = 1;
}

/** @brief Reads one byte from stream, advances cursor, shifts left 7, stores to halfword at a0+0xCA.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadPanLfo(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->panLfoDepth = val << 7;
}

void func_8001D1F0(SoundSeqTrack *track) {
    u16 steps;
    u8 *ptr = track->seqPtr;

    steps = *ptr++;
    track->seqPtr = ptr;
    if (steps == 0) {
        steps = 0x100;
    }
    track->panLfoDepthSlideStep = ((*ptr << 7) - track->panLfoDepth) / steps;
    track->panLfoDepthSlideSteps = steps;
    track->seqPtr = ptr + 1;
}

/** @brief Clears bit 0x4 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF2. */
void sndTrackStopPanLfo(SoundSeqTrack *track) {
    track->flags &= ~0x4;
    track->updateFlags |= 0x3;
    track->panLfoStop = 0;
}

/**
 * @brief Enables noise mode for the given voice bitmask on the appropriate SPU channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (g_sndSeqState[0x3C/4]); otherwise sets bits in D_80075028.noiseMask.
 * Marks the SPU dirty flags (D_80077288) with 0x110 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable noise mode for.
 */
void sndTrackEnableNoise(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->playingType == 0) {
        SoundMusicConfig *p = g_sndSeqState;
        p->noiseMask = p->noiseMask | a1;
    } else {
        D_80075028.noiseMask = D_80075028.noiseMask | a1;
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
void sndTrackDisableNoise(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->playingType == 0) {
        SoundMusicConfig *p = g_sndSeqState;
        p->noiseMask = p->noiseMask & ~a1;
    } else {
        D_80075028.noiseMask = D_80075028.noiseMask & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
    ((SoundSeqTrack *)a0)->noiseDuration = 0;
}

/**
 * @brief Enables reverb for the given voice bitmask on the appropriate SPU channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (g_sndSeqState[0x44/4]); otherwise checks flag 0x10000 at offset +0x30
 * before setting bits in D_80075028.pitchLfoMask. Marks dirty flags with 0x100.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable reverb for.
 */
void sndTrackEnableReverb(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->playingType == 0) {
        SoundMusicConfig *p = g_sndSeqState;
        p->pitchLfoMask = p->pitchLfoMask | a1;
    } else if (((SoundSeqTrack *)a0)->flags & 0x10000) {
        D_80075028.pitchLfoMask = D_80075028.pitchLfoMask | a1;
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
void sndTrackDisableReverb(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->playingType == 0) {
        SoundMusicConfig *p = g_sndSeqState;
        p->pitchLfoMask = p->pitchLfoMask & ~a1;
    } else {
        D_80075028.pitchLfoMask = D_80075028.pitchLfoMask & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
    ((SoundSeqTrack *)a0)->reverbDuration = 0;
}

/**
 * @brief Enables pitch modulation for the given voice bitmask on the appropriate channel.
 *
 * If the track's channel (offset +0x60) is 0, sets bits in the primary SPU
 * state (g_sndSeqState[0x40/4]); otherwise sets bits in D_80075028.reverbMask.
 * Marks SPU dirty flags with 0x100 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable pitch modulation for.
 */
void sndTrackEnablePitchMod(SoundSeqTrack *track, s32 a1) {
    if (track->playingType == 0) {
        SoundMusicConfig *p = g_sndSeqState;
        p->reverbMask = p->reverbMask | a1;
    } else {
        D_80075028.reverbMask = D_80075028.reverbMask | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

/**
 * @brief Disables pitch modulation for the given voice bitmask.
 *
 * If the track's channel (offset +0x60) is 0, clears bits in the primary SPU
 * state (g_sndSeqState[0x40/4]); otherwise clears bits in D_80075028.reverbMask.
 * Marks SPU dirty flags with 0x100.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to disable pitch modulation for.
 */
void func_8001D484(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->playingType == 0) {
        SoundMusicConfig *p = g_sndSeqState;
        p->reverbMask = p->reverbMask & ~a1;
    } else {
        D_80075028.reverbMask = D_80075028.reverbMask & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

/** @brief Sets the halfword at offset 0x9A of a0 to 1. */
void sndTrackSetTimerActive(SoundSeqTrack *track) {
    track->timerActive = 1;
}

/** @brief Empty stub — no operation. */
void sndTrackNop2(void) {
}

/** @brief Empty stub — no operation. */
void sndTrackNop3(void) {
}

/** @brief Empty stub — no operation. */
void sndTrackNop4(void) {
}

void func_8001D508(SoundSeqTrack *track) {
    s32 freq = *track->seqPtr++;

    if (track->playingType == 0) {
        if (freq & 0xC0) {
            g_sndSeqState->noiseClock = (g_sndSeqState->noiseClock + (freq & 0x3F)) & 0x3F;
        } else {
            g_sndSeqState->noiseClock = freq;
        }
    } else if (freq & 0xC0) {
        D_80075028.noiseClock = (D_80075028.noiseClock + (freq & 0x3F)) & 0x3F;
    } else {
        D_80075028.noiseClock = freq;
    }
    D_80077288[2] |= 0x10;
}

/**
 * @brief Reads byte from stream. ORs 0x900 into flags at +0xF8, ORs
 *        0x1000000 into +0x30, masks +0x106 to keep bits 0-7 and 15,
 *        ORs in byte shifted left 8.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadAdsrAttack(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 byte = *ptr;
    *(u8 **)track = ptr + 1;
    track->updateFlags |= 0x900;
    track->flags |= 0x1000000;
    track->adsrLow = (track->adsrLow & 0x80FF) | (byte << 8);
}

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
void sndTrackReadAdsrDecay(s32 a0, s32 a1) {
    s32 byte = *(u8 *)(*(s32 *)a0);
    *(s32 *)a0 = *(s32 *)a0 + 1;
    ((SoundSeqTrack *)a0)->adsrLow = (((SoundSeqTrack *)a0)->adsrLow & 0xFF0F) | (byte << 4);
    ((SoundSeqTrack *)a0)->updateFlags = ((SoundSeqTrack *)a0)->updateFlags | 0x1000;
}

/**
 * @brief Reads one byte from stream, advances cursor. Masks field at +0x106
 *        to clear low 4 bits and ORs in the byte. Sets bit 15 (0x8000) in
 *        flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadAdsrSustainLvl(SoundSeqTrack *track, s32 a1) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->updateFlags |= 0x8000;
    track->adsrLow = (track->adsrLow & 0xFFF0) | val;
}

/**
 * @brief Reads byte from stream. ORs 0x2200 into flags at +0xF8, ORs
 *        0x8000000 into +0x30, masks +0x108 to keep bits 0-5 and 13-15,
 *        ORs in byte shifted left 6.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadAdsrSustainRate(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 byte = *ptr;
    *(u8 **)track = ptr + 1;
    track->updateFlags |= 0x2200;
    track->flags |= 0x8000000;
    track->adsrHigh = (track->adsrHigh & 0xE03F) | (byte << 6);
}

/**
 * @brief Reads one byte from stream, advances cursor. ORs 0x4400 into flags
 *        at +0xF8, ORs 0x10000000 into field at +0x30, masks +0x108 to keep
 *        bits 5-15, ORs in byte (low 5 bits).
 * @param a0 Pointer to stream state.
 */
void sndTrackReadAdsrRelease(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 flags = track->updateFlags;
    s32 val;
    s32 f30;
    u16 f108;
    val = *ptr;
    *(u8 **)track = ptr + 1;
    track->updateFlags = flags | 0x4400;
    f30 = track->flags;
    f108 = track->adsrHigh;
    track->flags = f30 | 0x10000000;
    track->adsrHigh = (f108 & 0xFFE0) | val;
}

/**
 * @brief Reads one byte from stream, advances cursor. Clears bit 15 of +0x106.
 *        If byte == 5, sets bit 15 of +0x106. ORs 0x100 into flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadAdsrAttackMode(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    u16 f106 = track->adsrLow;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    f106 &= 0x7FFF;
    track->adsrLow = f106;
    if (val == 5) {
        track->adsrLow = f106 | 0x8000;
    }
    track->updateFlags = track->updateFlags | 0x100;
}

/**
 * @brief Reads one byte from stream, sets sustain mode bits in adsrHigh.
 *
 * Clears bits 15-14 of +0x108, then sets them based on the byte value:
 * 3 → 0x4000, 5 → 0x8000, 7 → 0xC000. ORs 0x200 into flags at +0xF8.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001D714(SoundSeqTrack *track) {
    u8 *ptr = track->seqPtr;
    u16 f108 = track->adsrHigh;
    u16 mode = *ptr;

    track->seqPtr = ptr + 1;
    f108 &= 0x3FFF;
    track->adsrHigh = f108;
    switch (mode) {
    case 3:
        track->adsrHigh = f108 | 0x4000;
        break;
    case 5:
        track->adsrHigh = f108 | 0x8000;
        break;
    case 7:
        track->adsrHigh = f108 | 0xC000;
        break;
    }
    track->updateFlags |= 0x200;
}

/**
 * @brief Reads one byte from stream, advances cursor. Clears bit 5 of +0x108.
 *        If byte == 7, sets bit 5 of +0x108. ORs 0x400 into flags at +0xF8.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadAdsrReleaseMode(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    u16 f108 = track->adsrHigh;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    f108 &= 0xFFDF;
    track->adsrHigh = f108;
    if (val == 7) {
        track->adsrHigh = f108 | 0x20;
    }
    track->updateFlags = track->updateFlags | 0x400;
}

/** @brief Increments the word at a0 by 1. */
void sndTrackSkip1Byte(s32 *a0) {
    *a0 += 1;
}

/** @brief Empty stub — no operation. */
void sndTrackNop5(void) {
}

/**
 * @brief Advance to the next voice slot in the circular buffer.
 *
 * Increments the slot index (D4, mod 4), saves the current stream pointer
 * to the slot's table entry, clears the slot's counter, and copies the
 * current voice index to the new slot.
 *
 * @param a0 Pointer to the track structure.
 */
/**
 * @brief Advance to the next voice slot in the circular buffer.
 *
 * Increments the slot index (D4, mod 4), saves the current stream pointer
 * to the slot's table entry, clears the slot's counter, and copies the
 * current voice index to the new slot.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001D7EC(SoundSeqTrack *track) {
    track->loopId = (track->loopId + 1) & 3;
    track->loopPoint[track->loopId] = track->seqPtr;
    track->loopTimes[track->loopId] = 0;
    track->loopUnk6E[track->loopId] = track->unk6E;
}

void func_8001D83C(SoundSeqTrack *track) {
    u16 times = *track->seqPtr++;

    if (times == 0) {
        times = 0x100;
    }
    if (++track->loopTimes[track->loopId] != times) {
        track->seqPtr = track->loopPoint[track->loopId];
        track->unk6E = track->loopUnk6E[track->loopId];
    } else {
        track->loopId = (track->loopId - 1) & 3;
    }
}

void func_8001D8D0(SoundSeqTrack *track) {
    u8 *ptr = track->seqPtr;
    u16 times = *ptr++;

    track->seqPtr = ptr;
    if (times == 0) {
        times = 0x100;
    }
    if (track->loopTimes[track->loopId] + 1 != times) {
        track->seqPtr = ptr + 2;
    } else {
        track->seqPtr = ptr + (s16)(ptr[0] | (ptr[1] << 8));
    }
}

/**
 * @brief Check loop counter and conditionally branch backward in the voice buffer.
 *
 * Reads a byte as the repeat count (0 becomes 0x100). Increments the
 * counter at slot D4*2+0x70. If counter reaches repeat count, reads a
 * 16-bit signed offset and branches, then decrements the slot index mod 4.
 * Otherwise skips the 2-byte offset field.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001D93C(SoundSeqTrack *track) {
    u8 *ptr = track->seqPtr;
    u16 times = *ptr++;

    track->seqPtr = ptr;
    if (times == 0) {
        times = 0x100;
    }
    if (track->loopTimes[track->loopId] + 1 != times) {
        track->seqPtr = ptr + 2;
    } else {
        track->seqPtr = ptr + (s16)(ptr[0] | (ptr[1] << 8));
        track->loopId = (track->loopId - 1) & 3;
    }
}

/**
 * @brief Advance to next voice entry: increment counter, update stream ptr and voice.
 *
 * Increments the 16-bit counter at a0 + D4*2 + 0x70, sets the stream
 * pointer (a0[0]) from the table at a0 + D4*4 + 4, and copies the
 * voice index from a0 + D4*2 + 0x78 to a0 + 0x6E.
 *
 * @param a0 Pointer to the track structure.
 */
void func_8001D9B8(SoundSeqTrack *track) {
    track->loopTimes[track->loopId]++;
    track->seqPtr = track->loopPoint[track->loopId];
    track->unk6E = track->loopUnk6E[track->loopId];
}

/** @brief Reads byte from stream, stores to three halfword fields (0x64, 0x62, 0xD6), clears 0xD8.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadVolume(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->volumeAccum = 0;
    track->volumeBase = val;
    track->volume = val;
    track->volumeDelta = val;
}

/**
 * @brief Read a signed delta byte from the stream and adjust the volume field.
 *
 * Loads a signed byte from the stream. If non-zero, adds it to the current
 * volume at offset 0xD6 and clamps the result to [1, 255]. Stores the
 * result (or 0 if delta was zero) at offset 0xD8.
 *
 * @param a0 Pointer to the track state structure.
 */
void sndTrackAdjustVolume(SoundSeqTrack *track) {
    u8 *ptr;
    s32 delta;

    ptr = *(u8 **)track;
    delta = *(s8 *)ptr;
    *(u8 **)track = ptr + 1;
    if (delta != 0) {
        delta += *(s16 *)&track->volumeDelta;
        if (delta <= 0) {
            delta = 1;
        } else if (delta >= 256) {
            delta = 255;
        }
    }
    track->volumeAccum = delta;
}

/**
 * @brief Conditionally updates field at +0x30 based on g_sndSeqState->0x34.
 *
 * If the word at g_sndSeqState+0x34 is nonzero, clears bits 0x19001008 and sets
 * bit 3 (0x8) in the word at a0+0x30.
 *
 * @param a0 Pointer to stream state.
 */
void sndTrackCheckBankFlags(SoundSeqTrack *track) {
    s32 check = g_sndSeqState->drumMap;
    if (check != 0) {
        track->flags = (track->flags & (s32)0xE6FFEFF7) | 8;
    }
}

/** @brief Clears bit 0x8 in a0+0x30 and zeroes halfword at a0+0x10A. */
void sndTrackClearOverride(SoundSeqTrack *track) {
    track->flags &= ~0x8;
    track->instOverride = 0;
}

/**
 * @brief Reads two bytes from stream, stores to g_sndSeqState+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 */
/**
 * @brief Reads two bytes from stream, stores to g_sndSeqState+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 */
/**
 * @brief Reads two bytes from stream, stores to g_sndSeqState+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 */
void func_8001DACC(SoundSeqTrack *track) {
    g_sndSeqState->timerLower = *track->seqPtr++;
    g_sndSeqState->timerUpper = *track->seqPtr++;
    g_sndSeqState->timerLowerCur = 0;
    g_sndSeqState->timerUpperCur = 0;
}

/**
 * @brief Reads two bytes from stream and combines into 16-bit value at
 *        g_sndSeqState+0x6C (low byte first, high byte shifted left 8).
 *
 * @param a0 Pointer to the stream state.
 */
/**
 * @brief Reads two bytes from stream and combines into 16-bit value at
 *        g_sndSeqState+0x6C (low byte first, high byte shifted left 8).
 *
 * @param a0 Pointer to the stream state.
 */
void func_8001DB04(SoundSeqTrack *track) {
    g_sndSeqState->timerTopCur = *track->seqPtr++;
    g_sndSeqState->timerTopCur |= *track->seqPtr++ << 8;
}

/**
 * @brief Reads a byte from stream and sets both hi and lo nibble track parameters.
 *
 * Calls sndTrackReadAdsrDecay to set bits [7:4] of offset +0x106, then calls
 * sndTrackReadAdsrSustainLvl to set bits [3:0] of offset +0x106.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Voice bitmask (passed through to sndTrackReadAdsrDecay).
 */
void sndTrackReadAdsrBoth(s32 a0, s32 a1) {
    sndTrackReadAdsrDecay(a0, a1);
    sndTrackReadAdsrSustainLvl((u8 *)a0, a1);
}

/**
 * @brief Reads byte from stream; computes duration (byte+1 or 0x101 if zero),
 *        stores to +0xD0, then calls sndTrackEnableNoise with voice bitmask.
 * @param a0 Pointer to stream state.
 * @param a1 Voice bitmask passed through to sndTrackEnableNoise.
 */
void sndTrackReadNoiseDuration(SoundSeqTrack *track, s32 a1) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    if (val != 0) {
        track->noiseDuration = val + 1;
    } else {
        track->noiseDuration = 0x101;
    }
    sndTrackEnableNoise((s32 *)track, a1);
}

/** @brief Reads byte from stream; if non-zero stores byte+1 to a0+0xD0, else stores 0x101.
 *  @param a0 Pointer to stream state.
 */
void sndTrackSetNoiseDuration(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    if (val != 0) {
        track->noiseDuration = val + 1;
    } else {
        track->noiseDuration = 0x101;
    }
}

/**
 * @brief Reads byte from stream; computes duration (byte+1 or 0x101 if zero),
 *        stores to +0xD2, then calls sndTrackEnableReverb with voice bitmask.
 * @param a0 Pointer to stream state.
 * @param a1 Voice bitmask passed through to sndTrackEnableReverb.
 */
void sndTrackReadReverbDuration(SoundSeqTrack *track, s32 a1) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    if (val != 0) {
        track->reverbDuration = val + 1;
    } else {
        track->reverbDuration = 0x101;
    }
    sndTrackEnableReverb((s32 *)track, a1);
}

/** @brief Reads byte from stream; if non-zero stores byte+1 to a0+0xD2, else stores 0x101.
 *  @param a0 Pointer to stream state.
 */
void sndTrackSetReverbDuration(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    if (val != 0) {
        track->reverbDuration = val + 1;
    } else {
        track->reverbDuration = 0x101;
    }
}

/**
 * @brief Clears bits 0x30 and 0x08 in flags at +0x30, then calls
 *        sndTrackDisableNoise, sndTrackDisableReverb, func_8001D484, and clears
 *        bits 0x5 in halfword at +0x9A.
 * @param a0 Pointer to stream state.
 * @param a1 Second argument passed to sndTrackDisableReverb and func_8001D484.
 */
void sndTrackClearEffects(SoundSeqTrack *track, s32 a1) {
    track->flags &= ~0x37;
    sndTrackDisableNoise((s32 *)track, a1);
    sndTrackDisableReverb((s32 *)track, a1);
    func_8001D484((s32 *)track, a1);
    track->timerActive = track->timerActive & 0xFFFA;
}

/** @brief Sets bit 0x10 in word at a0+0x30. */
void sndTrackSetMonoMode(SoundSeqTrack *track) {
    track->flags |= 0x10;
}

/** @brief Clears bit 0x10 in word at a0+0x30. */
void sndTrackClearMonoMode(SoundSeqTrack *track) {
    track->flags &= ~0x10;
}

/** @brief Sets bit 0x20 in word at a0+0x30. */
void sndTrackSetOverlap(SoundSeqTrack *track) {
    track->flags |= 0x20;
}

/** @brief Clears bit 0x20 in word at a0+0x30. */
void sndTrackClearOverlap(SoundSeqTrack *track) {
    track->flags &= ~0x20;
}

/**
 * @brief Reads two 16-bit LE offsets from the stream, resolves pointers,
 *        sets up D_80074EE8 with volume/pan data, and calls func_80017AAC.
 *
 * For each offset, if non-zero, computes a pointer as (stream_base + offset + 2);
 * otherwise passes NULL. Sets D_80074EE8[0..1] to 0, [2] to the upper byte of
 * the halfword at a0+0x8C, [3] to a0+0x44 >> 23. Advances the stream pointer by 4.
 *
 * @param a0 Pointer to the stream/voice state structure.
 */
void sndTrackReadVolumePanCurve(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    u16 off;
    s32 a5;
    s32 a6;

    {
        s32 hi = ptr[1];
        s32 lo = ptr[0];
        off = (hi << 8) | lo;
    }
    if (off != 0) {
        a5 = (s32)(ptr + off + 2);
    } else {
        a5 = 0;
    }

    ptr += 2;

    {
        s32 hi = ptr[1];
        s32 lo = ptr[0];
        off = (hi << 8) | lo;
    }
    if (off != 0) {
        a6 = (s32)(ptr + off + 2);
    } else {
        a6 = 0;
    }

    D_80074EE8[0] = 0;
    D_80074EE8[1] = 0;
    D_80074EE8[2] = track->volPan >> 8;
    D_80074EE8[3] = track->volumeLevel >> 23;
    func_80017AAC(D_80074EE8, a5, a6, 0);

    *(u8 **)track = *(u8 **)track + 4;
}

/**
 * @brief Reads byte from stream, sets flags |= 0x800, clears +0x6C,
 *        stores byte<<8 to +0x6A, then calls sndTrackEnablePitchMod.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadPitchModDepth(SoundSeqTrack *track, s32 a1) {
    u8 *ptr = *(u8 **)track;
    s32 flags = track->flags;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->pitchModCounter = 0;
    track->flags = flags | 0x800;
    track->pitchModDepth = val << 8;
    sndTrackEnablePitchMod(track, a1);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001DE18);

/** @brief Sets bit 0x100000 in word at a0+0x30. */
void sndTrackSetSustainFlag(SoundSeqTrack *track) {
    track->flags |= 0x100000;
}

/**
 * @brief Reads one byte from stream, advances cursor, adds D_80051824 base
 *        to get address, stores into field +0x38 of g_sndSeqState.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadBankAddress(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    g_sndSeqState->unk38 = val + (s32)&D_80051824;
}

/** @brief ORs a1 into the word at g_sndSeqState+0x8 (set flags).
 *  @param a0 Unused.
 *  @param a1 Bits to set.
 */
void sndTrackSetBankFlags(s32 a0, s32 a1) {
    g_sndSeqState->unk08 |= a1;
}

/** @brief ANDs ~a1 into the word at g_sndSeqState+0x8 (clear flags).
 *  @param a0 Unused.
 *  @param a1 Bits to clear.
 */
void sndTrackClearBankFlags(s32 a0, s32 a1) {
    g_sndSeqState->unk08 &= ~a1;
}

/** @brief Calls func_8001C2C8. */
void sndTrackResetSequence(SoundSeqTrack *track, u32 mask) {
    func_8001C2C8(track, mask);
}

/**
 * @brief Disables SPU IRQ and clears the IRQ voice mask from the engine state.
 *
 * If g_sndStream.active (the IRQ voice bitmask) is non-zero, disables
 * SPU IRQ, clears the IRQ callback, resets the IRQ address to the stored
 * value, clears the corresponding bits in D_80075028.reverbMask, marks dirty flags,
 * and zeroes the IRQ voice mask.
 */
void sndStreamDisableIrq(void) {
    SoundStream *s0 = &g_sndStream;
    if (s0->active != 0) {
        SpuSetIRQ(0);
        SpuSetIRQCallback(0);
        spuSetIrqAddr(s0->active);
        D_80075028.reverbMask &= ~s0->active;
        D_80077288[2] |= 0x100;
        s0->active = 0;
    }
}

s32 func_8001DFF4(void) {
    u32 mask;
    u32 bit;
    s32 i;

    while (1) {
        mask = D_80075028.activeMask | D_80075028.activeMaskStored;
        bit = 0xC00000;
        for (i = 11; i != 0; i--) {
            if (!(mask & bit)) {
                break;
            }
            bit >>= 1;
        }
        if (i != 0) {
            return i + 11;
        }
        func_800174E4(0, 0x40000000);
        if (mask == (D_80075028.activeMask | D_80075028.activeMaskStored)) {
            return -1;
        }
    }
}

/**
 * @brief Sets up SPU transfer at address 0x2100, registers callback
 *        sndStreamSetupMono, and initiates DMA transfer of 0x800 bytes.
 */
void sndStreamBeginTransfer(void) {
    s32 addr = g_sndStream.src + 0x800;
    SpuSetTransferStartAddr(0x2100);
    func_8003E494(sndStreamSetupMono);
    func_8003E3A4(addr, 0x800);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E0CC);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E308);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E4C4);

/**
 * @brief Sets up SPU voice parameters for both channels, then registers
 *        callback for 0x1000-byte streaming at addresses 0x1100/0x2100.
 */
void sndStreamSetupMono(void) {
    SoundStream *base = &g_sndStream;
    func_8001E308(base->voiceIdx, 0, 0x1100, 0x2100);
    func_8001E308(base->voiceIdx + 1, 0, 0x1100, 0x2100);
    func_8001E4C4(0x1000, 0x2100, sndStreamCallbackMonoLow);
}

/**
 * @brief Sets up SPU voice parameters for dual-channel streaming, then registers
 *        callback for 0x2000-byte streaming at addresses 0x1100-0x2900.
 */
void sndStreamSetupStereo(void) {
    SoundStream *base = &g_sndStream;
    func_8001E308(base->voiceIdx, 1, 0x1100, 0x2100);
    func_8001E308(base->voiceIdx + 1, 2, 0x1900, 0x2900);
    func_8001E4C4(0x2000, 0x2100, sndStreamCallbackStereoLow);
}

void func_8001E65C(s32 addr, s32 repeatAddr, s32 size, void (*callback)(void)) {
    if (g_sndStream.active != 0 && g_sndStream.remaining != 0) {
        SpuSetTransferStartAddr(addr);
        sndDmaSetActive();
        func_8003E3A4(g_sndStream.src, size);
        SpuSetIRQ(0);
        if (g_sndStream.remaining > 0x800) {
            SpuSetIRQCallback(callback);
            g_sndStream.remaining -= 0x800;
            g_sndStream.src += size;
        } else if (g_sndStream.loopSrc != 0) {
            SpuSetIRQCallback(callback);
            g_sndStream.src = g_sndStream.loopSrc;
            g_sndStream.remaining = g_sndStream.loopSize;
        } else {
            SpuSetIRQCallback(sndStreamDisableIrq);
            repeatAddr = 0x1030;
            addr = repeatAddr;
            g_sndStream.remaining = 0;
        }
        spuSetVoiceRepeatAddr(g_sndStream.voiceIdx, addr);
        spuSetVoiceRepeatAddr(g_sndStream.voiceIdx + 1, repeatAddr);
        SpuSetIRQAddr(addr + 8);
        SpuSetIRQ(1);
    }
}

void sndStreamCallbackMonoHigh(void);

/** @brief Calls func_8001E65C with SPU transfer params (0x1100, 0x1100, 0x800) and callback sndStreamCallbackMonoHigh. */
void sndStreamCallbackMonoLow(void) {
    func_8001E65C(0x1100, 0x1100, 0x800, sndStreamCallbackMonoHigh);
}

/** @brief Calls func_8001E65C with SPU transfer params (0x2100, 0x2100, 0x800) and callback sndStreamCallbackMonoLow. */
void sndStreamCallbackMonoHigh(void) {
    func_8001E65C(0x2100, 0x2100, 0x800, sndStreamCallbackMonoLow);
}

void sndStreamCallbackStereoHigh(void);

/** @brief Calls func_8001E65C with SPU transfer params (0x1100, 0x1900, 0x1000) and callback sndStreamCallbackStereoHigh. */
void sndStreamCallbackStereoLow(void) {
    func_8001E65C(0x1100, 0x1900, 0x1000, sndStreamCallbackStereoHigh);
}

/** @brief Calls func_8001E65C with SPU transfer params (0x2100, 0x2900, 0x1000) and callback sndStreamCallbackStereoLow. */
void sndStreamCallbackStereoHigh(void) {
    func_8001E65C(0x2100, 0x2900, 0x1000, sndStreamCallbackStereoLow);
}

/**
 * @brief Unpacks struct fields into args, calls func_8001E0CC, then clears
 *        the IRQ voice bits from D_80075028.activeMask.
 * @param a0 Pointer to 3-word struct: [data_ptr, loop_flag, callback].
 */
void sndStreamStartPlayback(s32 *a0) {
    func_8001E0CC(a0[0], a0[1], a0[2]);
    D_80075028.activeMask &= ~g_sndStream.active;
}

/** @brief Calls sndStreamDisableIrq. */
void sndStreamStop(void) {
    sndStreamDisableIrq();
}

/**
 * @brief Sets stereo panning volumes for two consecutive SPU voices.
 *
 * Dereferences @p a0 for the raw volume value, stores it in g_sndStream.panVolume,
 * clears g_sndStream.panTarget. If g_sndStream.active is set,
 * sets left volume on the first voice and right volume on the second.
 *
 * @param a0 Pointer to the raw volume/pan value.
 */
void sndStreamSetPanVolume(s32 *a0) {
    SoundStream *base = &g_sndStream;
    s32 rawVal = *a0;
    base->panTarget = 0;
    base->panVolume = rawVal;
    if (base->active != 0) {
        s32 vol = (rawVal << 15) >> 16;
        spuSetVoiceVolume(base->voiceIdx, vol, 0, 0);
        spuSetVoiceVolume(base->voiceIdx + 1, 0, (base->panVolume << 15) >> 16, 0);
    }
}

void func_8001E940(s32 *args) {
    s16 steps = 1;

    if (args[0] != 0) {
        steps = ((u16 *)args)[0];
    }
    g_sndStream.panStep = (s16)((s16)(((u16 *)args)[2] - (u16)g_sndStream.panVolume) / steps);
    g_sndStream.panTarget = steps;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E9C0);

/** @brief Sets SPU IRQ address to 0x1038 and registers sndStreamDisableIrq as the IRQ callback. */
void sndStreamSetupIrq(void) {
    SpuSetIRQAddr(0x1038);
    SpuSetIRQCallback(sndStreamDisableIrq);
}

void func_8001EB38(void) {
    s32 irqAddr;

    if (g_sndStream.remaining > 0x2000) {
        SpuSetTransferStartAddr(g_sndStream.unk2C + 0x2000);
        sndDmaWriteSpu(g_sndStream.src, g_sndStream.remaining - 0x2000);
        irqAddr = g_sndStream.unk2C + 0x1FF8;
    } else {
        irqAddr = g_sndStream.unk2C + (g_sndStream.remaining >> 1) + 8;
    }
    func_8001E308(g_sndStream.voiceIdx, 0, g_sndStream.unk2C, 0x1030);
    func_8001E308(g_sndStream.voiceIdx + 1, 0, g_sndStream.unk2C, 0x1030);
    SpuSetIRQAddr(irqAddr);
    SpuSetIRQCallback(sndStreamSetupIrq);
    spuSetReverbWorkAddr(g_sndStream.active);
    SpuSetIRQ(1);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001EC0C);

void func_8001EDD4(s32 unused, s32 reverb) {
    SndStreamHeader *hdr;
    s32 voice;
    s32 mask;
    s32 prev;
    s32 flags;

    if (D_80073C38.unk20 == 0) {
        return;
    }
    voice = func_8001DFF4();
    if (voice == -1) {
        return;
    }
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);
    mask = (1 << voice) | (1 << (voice + 1));
    prev = g_sndStream.active;
    g_sndStream.voiceIdx = voice;
    g_sndStream.active = mask;
    spuSetIrqAddr(mask | prev);
    hdr = &D_80073C38;
    g_sndStream.flags = hdr->flags;
    g_sndStream.savedPitch = hdr->pitch;
    if (reverb != 0) {
        D_80075028.reverbMask |= g_sndStream.active;
    } else {
        D_80075028.reverbMask &= ~g_sndStream.active;
    }
    D_80075028.pitchLfoMask &= ~g_sndStream.active;
    D_80075028.noiseMask &= ~g_sndStream.active;
    D_80077288[2] |= 0x100;
    SpuSetIRQAddr(hdr->unk20 + (hdr->size >> 1) + 8);
    SpuSetIRQCallback(sndStreamSetupIrq);
    flags = hdr->flags;
    if (flags & 1) {
        if (flags & 2) {
            func_8001E308(g_sndStream.voiceIdx, 1, hdr->unk20, hdr->unk20 + hdr->loopOffset);
            func_8001E308(g_sndStream.voiceIdx + 1, 2, hdr->unk20 + hdr->unk24, hdr->unk20 + hdr->unk24 + hdr->loopOffset);
        } else {
            func_8001E308(g_sndStream.voiceIdx, 1, hdr->unk20, 0x1030);
            func_8001E308(g_sndStream.voiceIdx + 1, 2, hdr->unk20 + hdr->unk24, 0x1030);
        }
    } else {
        if (flags & 2) {
            func_8001E308(g_sndStream.voiceIdx, 3, hdr->unk20, hdr->unk20 + hdr->loopOffset);
            func_8001E308(g_sndStream.voiceIdx + 1, 3, hdr->unk20, hdr->unk20 + hdr->loopOffset);
        } else {
            func_8001E308(g_sndStream.voiceIdx, 3, hdr->unk20, 0x1030);
            func_8001E308(g_sndStream.voiceIdx + 1, 3, hdr->unk20, 0x1030);
        }
    }
    spuSetReverbWorkAddr(g_sndStream.active);
    SpuSetIRQ(1);
}

/**
 * @brief Unpacks struct fields into 4 args, calls func_8001EC0C, then clears
 *        the IRQ voice bits from D_80075028.activeMask.
 * @param a0 Pointer to 4-word struct.
 */
void sndStreamStartPlayback4(s32 *a0) {
    func_8001EC0C(a0[0], a0[1], a0[2], a0[3]);
    D_80075028.activeMask &= ~g_sndStream.active;
}

/**
 * @brief Unpacks struct fields into 2 args, calls func_8001EDD4, then clears
 *        the IRQ voice bits from D_80075028.activeMask.
 * @param a0 Pointer to 2-word struct.
 */
void sndStreamStartPlayback2(s32 *a0) {
    func_8001EDD4(a0[0], a0[1]);
    D_80075028.activeMask &= ~g_sndStream.active;
}

/**
 * @brief Advances a sequence loop/repeat counter with wraparound.
 *
 * Increments g_sndStream.tickCount, then increments @p counter. If @p counter
 * exceeds the loop limit (g_sndStream.loopLimit - 1), wraps it back to 0.
 *
 * @param counter Pointer to the current loop/repeat index.
 * @return The new value of @p counter after increment and possible wraparound.
 */
s32 sndStreamAdvanceLoopCounter(s32 *counter) {
    SoundStream *base = &g_sndStream;
    base->tickCount++;
    (*counter)++;
    if ((u32)(base->loopLimit - 1) < (u32)*counter) {
        *counter = 0;
    }
    return *counter;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001F118);

void func_8001F2A8(s32 offset, s32 addr, void (*callback)(void)) {
    if (g_sndStream.active != 0) {
        func_8003E494(0);
        D_80074ED4 = 0;
        if (g_sndStream.remaining > 0xD20) {
            g_sndStream.src += offset;
            SpuSetIRQCallback(callback);
        } else {
            SpuSetIRQCallback(sndStreamDisableIrq);
            addr = 0x1030;
        }
        SpuSetIRQAddr(addr + 8);
        spuSetReverbWorkAddr(g_sndStream.active);
        SpuSetIRQ(1);
    }
}

/**
 * @brief Sets up SPU voice parameters for dual-channel streaming, then calls
 *        func_8001F2A8 with callback sndStreamCallbackStereoAltLow.
 */
void sndStreamSetupStereoAlt(void) {
    SoundStream *base = &g_sndStream;
    func_8001E308(base->voiceIdx, 1, 0x1100, 0x2100);
    func_8001E308(base->voiceIdx + 1, 2, 0x1900, 0x2900);
    func_8001F2A8(0x2000, 0x2100, sndStreamCallbackStereoAltLow);
}

void func_8001F3D4(s32 addr, s32 repeatAddr, s32 size, void (*callback)(void)) {
    SndStreamHeader *hdr;

    if (g_sndStream.active != 0) {
        hdr = (SndStreamHeader *)(g_sndStream.src + 0x100);
        if (hdr->magic == 0x4F414B41) {
            SpuSetIRQ(0);
            SpuSetTransferStartAddr(addr);
            sndDmaSetActive();
            sndStreamAdvanceLoopCounter(&g_sndStream.unk34);
            func_8003E3A4(g_sndStream.src + 0x170, size - 0x170);
            g_sndStream.unk20 = hdr->unk04;
            if (hdr->size > 0x690) {
                SpuSetIRQCallback(callback);
                g_sndStream.src += size;
                if (g_sndStream.unk34 == 0) {
                    g_sndStream.src = g_sndStream.unk2C;
                }
            } else {
                SpuSetIRQCallback(sndStreamDisableIrq);
                repeatAddr = 0x1030;
                addr = repeatAddr;
            }
            spuSetVoiceRepeatAddr(g_sndStream.voiceIdx, addr);
            spuSetVoiceRepeatAddr(g_sndStream.voiceIdx + 1, repeatAddr);
            SpuSetIRQAddr(addr + 8);
            SpuSetIRQ(1);
        }
    }
}

void sndStreamCallbackStereoAltHigh(void);

/** @brief Calls func_8001F3D4 with SPU transfer params (0x1100, 0x1900, 0x1000) and callback sndStreamCallbackStereoAltHigh. */
void sndStreamCallbackStereoAltLow(void) {
    func_8001F3D4(0x1100, 0x1900, 0x1000, sndStreamCallbackStereoAltHigh);
}

/** @brief Calls func_8001F3D4 with SPU transfer params (0x2100, 0x2900, 0x1000) and callback sndStreamCallbackStereoAltLow. */
void sndStreamCallbackStereoAltHigh(void) {
    func_8001F3D4(0x2100, 0x2900, 0x1000, sndStreamCallbackStereoAltLow);
}

/**
 * @brief Disables SPU IRQ, then sets up engine state from the given param struct.
 * @param a0 Pointer to 2-word struct: [field_0x2C, field_0x30].
 */
void sndStreamInitEngine(s32 *a0) {
    SoundStream *base;
    sndStreamDisableIrq();
    base = &g_sndStream;
    base->flags = 0x1000000;
    base->unk2C = a0[0];
    base->unk30 = a0[1];
}

