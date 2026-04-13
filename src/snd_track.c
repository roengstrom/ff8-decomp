#include "common.h"
#include "sound.h"

extern s32 *D_80074F08;
extern s32 D_80075028[];
extern s32 D_80077288[];
extern s32 D_80077298[];
extern s32 D_80074EE8[];
extern u8 D_80051824;

void sndStreamSetupMono(void);
void sndStreamCallbackMonoLow(void);
void sndStreamCallbackStereoLow(void);
void sndStreamCallbackStereoAltLow(void);

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
    D_80075028[0] &= mask;
    D_80075028[4] &= mask;
    D_80075028[7] &= mask;
    D_80075028[8] &= mask;
    D_80075028[9] &= mask;
    D_80075028[1] &= mask;
    D_80075028[2] &= mask;
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
 * bits from multiple fields in the D_80074F08 state structure. If the
 * remaining active voice mask becomes zero, also clears the sequence ID
 * and playback state. Otherwise delegates to sndTrackClearVoiceBits.
 * Zeroes the track flags and marks the SPU dirty flags.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to clear.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C2C8);

/**
 * @brief Read 2 bytes from stream, build 32-bit tempo value, store to bank.
 *
 * Reads byte0 from stream, shifts left 16 and stores to D_80074F08+0x20.
 * Then reads byte1, shifts left 24, ORs with previous value, stores again.
 * Advances stream pointer by 2. Clears halfword at D_80074F08+0x5C.
 *
 * @param a0 Pointer to stream state (first word is stream cursor).
 */
void sndTrackReadTempo(SoundSeqTrack *track) {
    s32 byte0 = *(u8 *)(*(s32 *)track) << 16;
    s32 *bank = D_80074F08;
    *(s32 *)((u8 *)bank + 0x20) = byte0;
    byte0 |= *(u8 *)(*(s32 *)track + 1) << 24;
    *(s32 *)((u8 *)bank + 0x20) = byte0;
    *(s32 *)track = *(s32 *)track + 2;
    *(u16 *)((u8 *)bank + 0x5C) = 0;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C3E8);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C490);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C530);

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
 * (D_80074F08+0x60), reads a 16-bit signed offset from the next two bytes
 * and jumps the stream cursor by that offset. Otherwise skips the 2-byte
 * offset field. Both paths end with storing the new cursor and returning.
 *
 * @param a0 Pointer to the stream state (cursor at +0x00).
 */
/**
 * @brief Conditional branch in music sequence: reads voice index and optional offset.
 *
 * Reads a byte from the stream. If the byte is within the valid voice count
 * (D_80074F08+0x60), reads a 16-bit signed offset from the next two bytes
 * and jumps the stream cursor by that offset. Otherwise skips the 2-byte
 * offset field.
 *
 * @param a0 Pointer to the stream state (cursor at +0x00).
 */
void func_8001C604(SoundSeqTrack *track) {
    s32 *bank = D_80074F08;
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    ptr++;
    *(u8 **)track = ptr;
    if (!(*(u16 *)((u8 *)bank + 0x60) < val)) {
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
    track->panFade = 0;
    track->updateFlags = flags;
    track->panFadeTarget = 0;
    track->panShifted = val << 23;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C738);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001C7C4);

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
    track->exprFade = 0;
    track->expression = (val << 8) & 0xFFFF;
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
    track->notePitchFade = 0;
    track->updateFlags = flags | 3;
    val += 0x40;
    val &= 0xFF;
    val <<= 8;
    track->notePitch = val;
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

    if (track->voiceActive == 0) {
        inst = sndAdjustNoteOctave(*D_80074F08, byte);
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
    s32 *bankPtr = D_80074F08;
    u8 *ptr = *(u8 **)track;
    s32 byte = *ptr;
    s32 inst;
    *(u8 **)track = ptr + 1;
    inst = sndAdjustNoteOctave(*bankPtr, byte);
    sndTrackSetInstrumentParams(track, &D_80073E68[inst], 0x1010);
    track->instrument = inst;
    track->instOverride = 0;
    track->flags &= (s32)0xE6FFEFF7;
}

/**
 * @brief Reads byte from stream, looks up instrument override in bank pointer table.
 *
 * If D_80074F08+0x30 (bank pointer) is zero, returns. Otherwise looks up
 * the byte as an index in the table. If the entry is negative, clears
 * instOverride and masks flags. Otherwise stores the pointer, sets 0xE8
 * to 0xFF, masks and ORs 0x1000 into flags.
 *
 * @param a0 Pointer to the track structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CBA4);

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

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CD50);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CDB0);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CE14);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CF0C);

/**
 * @brief Reads volume LFO fade parameters from the stream.
 *
 * Reads a byte for the fade counter (0 becomes 0x100), then reads the
 * next byte as target volume. Computes delta = (target << 8 - current) / counter.
 *
 * @param a0 Pointer to the track structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CF6C);

/** @brief Clears bit 0x1 in a0+0x30, sets bit 0x10 in a0+0xF8, zeroes a0+0xEE. */
void sndTrackStopPortamento(SoundSeqTrack *track) {
    track->flags &= ~0x1;
    track->updateFlags |= 0x10;
    track->portamento = 0;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001CFFC);

/** @brief Reads one byte from stream, advances cursor, masks to 7 bits, shifts left 8, stores to halfword at a0+0xBC.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadVolumeLfo(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->volumeLfo = (val & 0x7F) << 8;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D0D0);

/** @brief Clears bit 0x2 in a0+0x30, sets bits 0x3 in a0+0xF8, zeroes a0+0xF0. */
void sndTrackStopVolumeLfo(SoundSeqTrack *track) {
    track->flags &= ~0x2;
    track->updateFlags |= 0x3;
    track->volumeLfoStop = 0;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D164);

/** @brief Reads one byte from stream, advances cursor, shifts left 7, stores to halfword at a0+0xCA.
 *  @param a0 Pointer to stream state.
 */
void sndTrackReadPanLfo(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    track->panLfo = val << 7;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D1F0);

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
 * state (D_80074F08[0x3C/4]); otherwise sets bits in D_80075028[0x1C/4].
 * Marks the SPU dirty flags (D_80077288) with 0x110 to schedule an update.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to enable noise mode for.
 */
void sndTrackEnableNoise(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->voiceActive == 0) {
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
void sndTrackDisableNoise(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->voiceActive == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] & ~a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
    ((SoundSeqTrack *)a0)->noiseDuration = 0;
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
void sndTrackEnableReverb(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->voiceActive == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] | a1;
    } else if (((SoundSeqTrack *)a0)->flags & 0x10000) {
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
void sndTrackDisableReverb(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->voiceActive == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] & ~a1;
    } else {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
    ((SoundSeqTrack *)a0)->reverbDuration = 0;
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
void sndTrackEnablePitchMod(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->voiceActive == 0) {
        s32 *p = D_80074F08;
        p[0x40 / 4] = p[0x40 / 4] | a1;
    } else {
        D_80075028[0x20 / 4] = D_80075028[0x20 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

/**
 * @brief Disables pitch modulation for the given voice bitmask.
 *
 * If the track's channel (offset +0x60) is 0, clears bits in the primary SPU
 * state (D_80074F08[0x40/4]); otherwise clears bits in D_80075028[0x20/4].
 * Marks SPU dirty flags with 0x100.
 *
 * @param a0 Pointer to the sequence track structure.
 * @param a1 Bitmask of SPU voices to disable pitch modulation for.
 */
void func_8001D484(s32 *a0, s32 a1) {
    if (((SoundSeqTrack *)a0)->voiceActive == 0) {
        s32 *p = D_80074F08;
        p[0x40 / 4] = p[0x40 / 4] & ~a1;
    } else {
        D_80075028[0x20 / 4] = D_80075028[0x20 / 4] & ~a1;
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

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D508);

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
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D714);

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
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D7EC);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D83C);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D8D0);

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
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D93C);

/**
 * @brief Advance to next voice entry: increment counter, update stream ptr and voice.
 *
 * Increments the 16-bit counter at a0 + D4*2 + 0x70, sets the stream
 * pointer (a0[0]) from the table at a0 + D4*4 + 4, and copies the
 * voice index from a0 + D4*2 + 0x78 to a0 + 0x6E.
 *
 * @param a0 Pointer to the track structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001D9B8);

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
 * @brief Conditionally updates field at +0x30 based on D_80074F08->0x34.
 *
 * If the word at D_80074F08+0x34 is nonzero, clears bits 0x19001008 and sets
 * bit 3 (0x8) in the word at a0+0x30.
 *
 * @param a0 Pointer to stream state.
 */
void sndTrackCheckBankFlags(SoundSeqTrack *track) {
    s32 check = *(s32 *)((u8 *)D_80074F08 + 0x34);
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
 * @brief Reads two bytes from stream, stores to D_80074F08+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 */
/**
 * @brief Reads two bytes from stream, stores to D_80074F08+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 */
/**
 * @brief Reads two bytes from stream, stores to D_80074F08+0x68 and +0x64,
 *        then clears +0x6A and +0x66.
 *
 * @param a0 Pointer to the stream state.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001DACC);

/**
 * @brief Reads two bytes from stream and combines into 16-bit value at
 *        D_80074F08+0x6C (low byte first, high byte shifted left 8).
 *
 * @param a0 Pointer to the stream state.
 */
/**
 * @brief Reads two bytes from stream and combines into 16-bit value at
 *        D_80074F08+0x6C (low byte first, high byte shifted left 8).
 *
 * @param a0 Pointer to the stream state.
 */
INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001DB04);

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
    D_80074EE8[2] = track->notePitch >> 8;
    D_80074EE8[3] = track->panShifted >> 23;
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
    sndTrackEnablePitchMod((s32 *)track, a1);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001DE18);

/** @brief Sets bit 0x100000 in word at a0+0x30. */
void sndTrackSetSustainFlag(SoundSeqTrack *track) {
    track->flags |= 0x100000;
}

/**
 * @brief Reads one byte from stream, advances cursor, adds D_80051824 base
 *        to get address, stores into field +0x38 of D_80074F08.
 * @param a0 Pointer to stream state.
 */
void sndTrackReadBankAddress(SoundSeqTrack *track) {
    u8 *ptr = *(u8 **)track;
    s32 val = *ptr;
    *(u8 **)track = ptr + 1;
    *(s32 *)((u8 *)D_80074F08 + 0x38) = val + (s32)&D_80051824;
}

/** @brief ORs a1 into the word at D_80074F08+0x8 (set flags).
 *  @param a0 Unused.
 *  @param a1 Bits to set.
 */
void sndTrackSetBankFlags(s32 a0, s32 a1) {
    *(s32 *)((u8 *)D_80074F08 + 0x8) |= a1;
}

/** @brief ANDs ~a1 into the word at D_80074F08+0x8 (clear flags).
 *  @param a0 Unused.
 *  @param a1 Bits to clear.
 */
void sndTrackClearBankFlags(s32 a0, s32 a1) {
    *(s32 *)((u8 *)D_80074F08 + 0x8) &= ~a1;
}

/** @brief Calls func_8001C2C8. */
void sndTrackResetSequence(void) {
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
void sndStreamDisableIrq(void) {
    s32 *s0 = D_80077298;
    if (s0[3] != 0) {
        SpuSetIRQ(0);
        SpuSetIRQCallback(0);
        spuSetIrqAddr(s0[3]);
        D_80075028[8] &= ~s0[3];
        D_80077288[2] |= 0x100;
        s0[3] = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001DFF4);

/**
 * @brief Sets up SPU transfer at address 0x2100, registers callback
 *        sndStreamSetupMono, and initiates DMA transfer of 0x800 bytes.
 */
void sndStreamBeginTransfer(void) {
    s32 addr = D_80077298[0] + 0x800;
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
    s32 *base = D_80077298;
    func_8001E308(base[4], 0, 0x1100, 0x2100);
    func_8001E308(base[4] + 1, 0, 0x1100, 0x2100);
    func_8001E4C4(0x1000, 0x2100, sndStreamCallbackMonoLow);
}

/**
 * @brief Sets up SPU voice parameters for dual-channel streaming, then registers
 *        callback for 0x2000-byte streaming at addresses 0x1100-0x2900.
 */
void sndStreamSetupStereo(void) {
    s32 *base = D_80077298;
    func_8001E308(base[4], 1, 0x1100, 0x2100);
    func_8001E308(base[4] + 1, 2, 0x1900, 0x2900);
    func_8001E4C4(0x2000, 0x2100, sndStreamCallbackStereoLow);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E65C);

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
 *        the IRQ voice bits from D_80075028[0].
 * @param a0 Pointer to 3-word struct: [data_ptr, loop_flag, callback].
 */
void sndStreamStartPlayback(s32 *a0) {
    func_8001E0CC(a0[0], a0[1], a0[2]);
    D_80075028[0] &= ~D_80077298[3];
}

/** @brief Calls sndStreamDisableIrq. */
void sndStreamStop(void) {
    sndStreamDisableIrq();
}

/**
 * @brief Sets stereo panning volumes for two consecutive SPU voices.
 *
 * Dereferences @p a0 for the raw volume value, stores it at D_80077298+0x40,
 * clears D_80077298+0x48. If the active flag at D_80077298+0xC is set,
 * sets left volume on the first voice and right volume on the second.
 *
 * @param a0 Pointer to the raw volume/pan value.
 */
void sndStreamSetPanVolume(s32 *a0) {
    u8 *base = (u8 *)D_80077298;
    s32 rawVal = *a0;
    *(s32 *)(base + 0x48) = 0;
    *(s32 *)(base + 0x40) = rawVal;
    if (*(s32 *)(base + 0xC) != 0) {
        s32 vol = (rawVal << 15) >> 16;
        spuSetVoiceVolume(*(s32 *)(base + 0x10), vol, 0, 0);
        spuSetVoiceVolume(*(s32 *)(base + 0x10) + 1, 0, (*(s32 *)(base + 0x40) << 15) >> 16, 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E940);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001E9C0);

/** @brief Sets SPU IRQ address to 0x1038 and registers sndStreamDisableIrq as the IRQ callback. */
void sndStreamSetupIrq(void) {
    SpuSetIRQAddr(0x1038);
    SpuSetIRQCallback(sndStreamDisableIrq);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001EB38);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001EC0C);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001EDD4);

/**
 * @brief Unpacks struct fields into 4 args, calls func_8001EC0C, then clears
 *        the IRQ voice bits from D_80075028[0].
 * @param a0 Pointer to 4-word struct.
 */
void sndStreamStartPlayback4(s32 *a0) {
    func_8001EC0C(a0[0], a0[1], a0[2], a0[3]);
    D_80075028[0] &= ~D_80077298[3];
}

/**
 * @brief Unpacks struct fields into 2 args, calls func_8001EDD4, then clears
 *        the IRQ voice bits from D_80075028[0].
 * @param a0 Pointer to 2-word struct.
 */
void sndStreamStartPlayback2(s32 *a0) {
    func_8001EDD4(a0[0], a0[1]);
    D_80075028[0] &= ~D_80077298[3];
}

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
s32 sndStreamAdvanceLoopCounter(s32 *counter) {
    s32 base = (s32)D_80077298;
    *(s32 *)(base + 0x28) += 1;
    (*counter)++;
    if ((u32)(*(s32 *)(base + 0x3C) - 1) < (u32)*counter) {
        *counter = 0;
    }
    return *counter;
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001F118);

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001F2A8);

/**
 * @brief Sets up SPU voice parameters for dual-channel streaming, then calls
 *        func_8001F2A8 with callback sndStreamCallbackStereoAltLow.
 */
void sndStreamSetupStereoAlt(void) {
    s32 *base = D_80077298;
    func_8001E308(base[4], 1, 0x1100, 0x2100);
    func_8001E308(base[4] + 1, 2, 0x1900, 0x2900);
    func_8001F2A8(0x2000, 0x2100, sndStreamCallbackStereoAltLow);
}

INCLUDE_ASM("asm/nonmatchings/snd_track", func_8001F3D4);

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
    s32 *base;
    sndStreamDisableIrq();
    base = D_80077298;
    base[2] = 0x1000000;
    base[11] = a0[0];
    base[12] = a0[1];
}

