#include "common.h"
#include "sound.h"
#include "snd_init.h"

extern s32 D_80074EB8[];
extern s32 D_80074EB0;
extern VoicePoolEntry D_80074F20[12];

/**
 * @brief Adjusts instrument index upward if flag 0x400 is set and instrument is in range.
 *
 * If bit 10 of @p a0 is set and the track's instrument index is in [0x40, 0x80),
 * offsets the sample addresses by +0x20000 and the instrument by +0x20.
 *
 * @param a0 Flags bitfield.
 * @param a1 Pointer to the sequence track structure.
 * @return The (possibly adjusted) instrument index.
 */
s32 func_80016DB4(s32 a0, SoundSeqTrack *a1) {
    if (a0 & 0x400) {
        if ((u32)(a1->instrument - 0x40) < 0x40) {
            s32 adj = 0x20000;
            a1->voiceAttr.startAddr = a1->voiceAttr.startAddr + adj;
            a1->voiceAttr.repeatAddr = a1->voiceAttr.repeatAddr + adj;
            a1->instrument = a1->instrument + 0x20;
        }
    }
    return a1->instrument;
}

/**
 * @brief Adjusts instrument index downward if flag 0x400 is set and instrument is in range.
 *
 * If bit 10 of @p a0 is set and the track's instrument index is in [0x60, 0x80),
 * offsets the sample addresses by -0x20000 and the instrument by -0x20.
 *
 * @param a0 Flags bitfield.
 * @param a1 Pointer to the sequence track structure.
 * @return The (possibly adjusted) instrument index.
 */
s32 func_80016E08(s32 a0, SoundSeqTrack *a1) {
    if (a0 & 0x400) {
        if ((u32)(a1->instrument - 0x60) < 0x20) {
            s32 adj = (s32)0xFFFE0000;
            a1->voiceAttr.startAddr = a1->voiceAttr.startAddr + adj;
            a1->voiceAttr.repeatAddr = a1->voiceAttr.repeatAddr + adj;
            a1->instrument = a1->instrument - 0x20;
        }
    }
    return a1->instrument;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80016E5C);

/**
 * @brief Determines which sound bank group a given identifier belongs to.
 *
 * Compares @p a0 against entries in D_80074EB8 (a table of sound bank IDs).
 * Indices 1 and 4 map to group 1; indices 2 and 5 map to group 2.
 *
 * @param a0 Sound bank or instrument identifier to look up.
 * @return 1 if in group 1, 2 if in group 2, 0 if not found or @p a0 is 0.
 */
s32 sndGetBankGroup(s32 a0) {
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
 * entry 0 via sndTrackApplyInstrument.
 *
 * @param a0 Pointer to the track structure (stride 0x110).
 * @param a1 Instrument bank ID to assign to this track.
 */
void sndInitTrack(SoundSeqTrack *track, s32 bankId) {
    track->volumeMultiplier = 0x6E00;
    track->volumeLevel = 0x32000000;
    *(s32 *)track = bankId;
    track->detune = 0;
    track->fineTuning = 0;
    track->duration = 0;
    track->pitchSlide = 0;
    track->durationDelta2 = 0;
    track->pitchSlideSteps = 0;
    track->volumeAccum = 0;
    track->volumeDelta = 0;
    track->volSlideSteps = 0;
    track->fineTunePitch = 0;
    track->loopId = 0;
    track->flags = 0;
    track->panLfoVol = 0;
    track->timerActive = 0;
    track->unk6E = 0xFFFF;
    track->voiceAttr.volScale = 0;
    track->panLfoDepth = 0;
    track->tremoloDepth = 0;
    track->vibratoDepth = 0;
    track->panLfoDepthSlideSteps = 0;
    track->tremoloDepthSlideSteps = 0;
    track->vibratoDepthSlideSteps = 0;
    track->pitchLfoSwitchDelay = 0;
    track->noiseSwitchDelay = 0;
    sndTrackApplyInstrument(track, 0);
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
s32 sndBuildVoiceMask(u8 *a0, s32 a1) {
    u32 i = 0;
    s32 result = 0;
    s32 bit = 1;
    do {
        if (a1 & (bit << i)) {
            s32 val = ((SoundSeqTrack *)a0)->voiceAttr.voice;
            if ((u32)val < 0x18) {
                result |= (bit << val);
            }
        }
        i++;
        a0 += 0x110;
    } while (i < 0x20);
    return result;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_8001708C);

void func_80017410(SoundMusicConfig *config, SoundSeqTrack *tracks, s32 id) {
    SoundMusicConfig **owner;
    u32 i;

    if (config->activeMask == 0) {
        return;
    }
    if (id == 0 || id == config->musicId) {
        config->unk18 = -1;
        for (i = 32; i != 0; i--, tracks++) {
            tracks->volume = 3;
            tracks->volumeBase = 1;
            tracks->seqPtr = D_800517F0;
        }
        config->musicId = 0;
        config->unk14 = 0;
        config->unk10 = 0;
        owner = D_800772F8;
        for (i = 0; i < 24; i++, owner++) {
            if (*owner == config) {
                *owner = NULL;
                spuSetVoiceReleaseMode(i, 5, 3);
            }
        }
    }
}

void func_800174E4(s32 id, u32 mode) {
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    u32 i;
    s32 age;
    s32 flags;

    bit = 0x1000;
    track = D_80072F70;
    active = D_80075028.activeMask | D_80075028.activeMaskStored;
    if (mode & 0x0FFFFFFF) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if (active & bit) {
                if (id == -2) {
                    if ((track->keyOnMask & mode) && track->keyOffMask < 0) {
                        flags = track->flags;
                        if (flags & 0x100000) {
                            track->flags = flags | 0x200000;
                        } else {
                            D_80075028.unk0C |= bit;
                            sndTrackClearVoiceBits(track, bit);
                            track->flags = 0;
                        }
                    }
                } else if (track->keyOnMask & mode) {
                    flags = track->flags;
                    if (flags & 0x100000) {
                        track->flags = flags | 0x200000;
                    } else {
                        D_80075028.unk0C |= bit;
                        sndTrackClearVoiceBits(track, bit);
                        track->flags = 0;
                    }
                }
            }
        }
    } else if (mode & 0x80000000) {
        track = &D_80072F70[id];
        bit <<= id;
        if (active & bit) {
            func_800174E4(track->keyOffMask, 0);
        }
        bit <<= 1;
        track++;
        if (active & bit) {
            func_800174E4(track->keyOffMask, 0);
        }
        return;
    } else if (mode & 0x40000000) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if (track->keyOnMask != 0) {
                active &= ~bit;
            }
        }
        track = D_80072F70;
        bit = 0x1000;
        age = 0;
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && age < track->unk54) {
                age = track->unk54;
            }
        }
        track = D_80072F70;
        bit = 0x1000;
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && age == track->unk54) {
                flags = track->flags;
                if (flags & 0x100000) {
                    track->flags = flags | 0x200000;
                } else {
                    D_80075028.unk0C |= bit;
                    sndTrackClearVoiceBits(track, bit);
                    track->flags = 0;
                }
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if (active & bit) {
                if (id == -1) {
                    if (track->keyOffMask < 0) {
                        flags = track->flags;
                        if (flags & 0x100000) {
                            track->flags = flags | 0x200000;
                        } else {
                            D_80075028.unk0C |= bit;
                            sndTrackClearVoiceBits(track, bit);
                            track->flags = 0;
                        }
                    }
                } else if (track->keyOffMask == id) {
                    flags = track->flags;
                    if (flags & 0x100000) {
                        track->flags = flags | 0x200000;
                    } else {
                        D_80075028.unk0C |= bit;
                        sndTrackClearVoiceBits(track, bit);
                        track->flags = 0;
                    }
                }
            }
        }
    }
    D_80077288[2] |= 0x110;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017880);

/**
 * @brief Releases a voice from all active sequence tracks (duplicate of sndReleaseVoice).
 *
 * Iterates over 32 track entries in D_80070D60 (stride 0x110, voice at +0xF4).
 * Any track assigned to voice @p a0 is reassigned to 24 (none). Also checks
 * the secondary sequence array D_80073C34 if D_80073CA8 is set.
 *
 * @param a0 SPU voice index to release (0-23).
 */
void sndReleaseVoiceFromTracks(s32 voiceId) {
    s32 i;
    SoundSeqTrack *track;

    track = (SoundSeqTrack *)D_80070D60;
    for (i = 32; i != 0; i--) {
        if (track->voiceAttr.voice == voiceId)
            track->voiceAttr.voice = 0x18;
        track++;
    }

    if (D_80073CA8 != 0) {
        track = D_80073C34;
        for (i = 32; i != 0; i--) {
            if (track->voiceAttr.voice == voiceId)
                track->voiceAttr.voice = 0x18;
            track++;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017AAC);

/**
 * @brief Resolves two consecutive sample addresses from the instrument table.
 *
 * Looks up the halfword at D_80074ED0[a2*2] (first entry) and
 * D_80074ED0[a2*2+1] (second entry). For each, if the value is not
 * 0xFFFF, adds D_80074EDC as a base to produce a sample pointer;
 * otherwise returns 0. Stores results at *a0 and *a1.
 *
 * @param a0 Output pointer for first sample address.
 * @param a1 Output pointer for second sample address.
 * @param a2 Instrument index (low 10 bits used).
 */
void func_80017C9C(s32 *a0, s32 *a1, s32 a2) {
    s32 val;

    a2 &= 0x3FF;
    a2 <<= 1;
    if (D_80074ED0[a2] != 0xFFFF) {
        val = (s32)(D_80074EDC + D_80074ED0[a2]);
    } else {
        val = 0;
    }
    a2++;
    *a0 = val;
    if (D_80074ED0[a2] != 0xFFFF) {
        val = (s32)(D_80074EDC + D_80074ED0[a2]);
    } else {
        val = 0;
    }
    *a1 = val;
}

void func_80017D14(SoundMusicConfig *config, SoundSeqTrack *tracks) {
    s32 value = config->activeMask;
    s32 mask;
    s32 bit;

    if (value != 0) {
        mask = value;
        bit = 1;
        do {
            if (mask & bit) {
                mask ^= bit;
                tracks->voiceAttr.updateFlags |= 3;
            }
            tracks++;
            bit <<= 1;
        } while (mask != 0);
    }
}

void func_80017D5C(void) {
    s32 value = D_80075028.activeMask;
    s32 mask;
    s32 bit;
    SoundSeqTrack *tracks;
    s32 *flags;

    if (value != 0) {
        mask = value;
        bit = 0x1000;
        tracks = D_80072F70;
        flags = (s32 *)((s32)tracks + 0xF8);
        do {
            if (mask & bit) {
                mask ^= bit;
                *flags |= 3;
            }
            flags = (s32 *)((s32)flags + 0x110);
            bit <<= 1;
        } while (mask != 0);
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80017DB0);

/**
 * @brief Plays or stops a sound depending on the active sequence ID.
 *
 * Checks the halfword at D_80073DF0+0x5E (active sequence ID). If non-zero
 * and matches a0[8], calls func_80017DB0(a0[0], 0) to stop. Otherwise calls
 * func_8001708C(a0[0], -1) and stores a0[8] into the structure pointed to
 * by g_sndSeqState at offset 0x5E.
 *
 * @param a0 Pointer to a sound config structure (word 0 = track ID, halfword 8 = sequence ID).
 */
/**
 * @brief Plays or stops a sound depending on the active sequence ID.
 *
 * Checks the halfword at D_80073DF0+0x5E (active sequence ID). If non-zero
 * and matches a0[8], calls func_80017DB0(a0[0], 0) to stop. Otherwise calls
 * func_8001708C(a0[0], -1) and stores a0[8] into the structure pointed to
 * by g_sndSeqState at offset 0x5E.
 *
 * @param a0 Pointer to a sound config structure (word 0 = track ID, halfword 8 = sequence ID).
 */
void func_80018158(SndVoice *voice) {
    if (D_80073DF0.musicId != 0 && D_80073DF0.musicId == voice->params[2]) {
        func_80017DB0(voice->params[0], 0);
    } else {
        func_8001708C(voice->params[0], -1);
        g_sndSeqState->musicId = voice->params[2];
    }
}

/**
 * @brief Configure audio track and update playback globals.
 *
 * Calls func_8001708C with the first and fourth fields of the input struct,
 * copies field8 to g_sndSeqState->0x5E, and sets D_80074EB0 to field10-1
 * (or 0 if field10 is 0).
 *
 * @param a0 Pointer to audio config struct.
 */
void sndConfigureTrackPlayback(s32 *a0) {
    s32 val, result;
    func_8001708C(a0[0], a0[3]);
    g_sndSeqState->musicId = *(u16 *)((u8 *)a0 + 8);
    result = 0;
    /* The val load is block-extracted so result binds to $a0: a plain
       `val = a0[4];` here schedules result into $v1 and mismatches. */
    do {
        val = a0[4];
    } while (0);
    if (val != 0) {
        result = val - 1;
    }
    D_80074EB0 = result;
}

void func_80018234(void) {
    SoundSeqTrack *track;
    u32 mask;
    u32 i;

    if (g_sndSeqState->activeMask == 0) {
        return;
    }
    if (D_80073CA8 != NULL && D_80073CA8->activeMask != 0) {
        track = D_80073C34;
        i = 0;
        mask = D_80073CA8->activeMask;
        while (i < 32) {
            if ((mask & (1 << i)) && track->voiceAttr.voice != 0x18) {
                D_80075028.unk0C |= 1 << track->voiceAttr.voice;
            }
            i++;
            track++;
        }
        D_80073CA8 = NULL;
    }
    func_8001A57C(g_sndSeqState, &D_80073DF0, 0x70);
    func_8001A57C(D_80070D60, D_80075080, 0x2200);
    track = D_80075080;
    for (i = 0; i < 32; i++, track++) {
        func_80016E08(D_80073DF0.unk00, track);
    }
}

void func_80018358(s32 *args) {
    if (g_sndSeqState->activeMask != 0 && (D_80073CA8 == NULL || D_80073CA8->musicId == 0)) {
        D_80073CA8 = &D_80073DF0;
        D_80073C34 = D_80075080;
        func_8001A57C(g_sndSeqState, &D_80073DF0, 0x70);
        func_8001A57C(D_80070D60, D_80073C34, 0x2200);
    }
    func_8001708C(args[0], -1);
    g_sndSeqState->volMul = (args[4] & 0x7F) << 16;
    g_sndSeqState->volMulSlideSteps = 0;
    g_sndSeqState->musicId = args[2];
    func_80016E5C();
    D_80077288[1] &= ~0x100;
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018438);

/**
 * @brief Process a sound voice and compute the remaining play count.
 *
 * Calls func_80018158 on the voice structure, then reads the play count
 * at offset +0x0C. If non-zero, decrements by 1 and stores to D_80074EB0;
 * otherwise stores 0.
 *
 * @param a0 Pointer to the sound voice structure.
 */
void sndPlayVoiceWithCount(SndVoice *voice) {
    s32 val, result;
    func_80018158(voice);
    val = voice->playCount;
    result = 0;
    if (val != 0) {
        result = val - 1;
    }
    D_80074EB0 = result;
}

/**
 * @brief Saves current sound parameters, replaces with defaults, and triggers playback.
 *
 * Saves the first two words of the structure at @p a0, then overwrites
 * with default volume/pan parameters (0x400, 0x1000000, 0x80, 0x7F, 0)
 * and calls func_80017AAC with the saved values.
 *
 * @param a0 Pointer to a sound parameter structure (5 words).
 */
void sndPlayWithDefaults(s32 *a0) {
    s32 a1 = a0[0];
    s32 a2 = a0[1];
    a0[0] = 0x400;
    a0[1] = 0x1000000;
    a0[2] = 0x80;
    a0[3] = 0x7F;
    a0[4] = 0;
    func_80017AAC(a0, a1, a2, 0);
}

/**
 * @brief Set default sound parameters, look up voice, and play.
 *
 * Resolves the SPU voice address via func_80017C9C, sets default volume
 * (0x2000000), pan (0x80), and envelope (0x7F) on the parameter structure,
 * looks up the instrument group via D_80074ED8, and plays via func_80017AAC.
 *
 * @param a0 Pointer to the sound parameter structure.
 */
void sndPlaySoundEffect(s32 *a0) {
    s32 local10, local14;

    func_80017C9C(&local10, &local14, a0[0]);
    a0[1] = 0x2000000;
    a0[2] = 0x80;
    a0[3] = 0x7F;
    a0[4] = sndGetBankGroup(D_80074ED8[a0[0]]);
    func_80017AAC(a0, local10, local14, 0);
}

/**
 * @brief Look up and play a sound voice by index.
 *
 * Resolves the SPU voice address via func_80017C9C, reads the sample pitch
 * from D_80074ED8 table, processes it with sndGetBankGroup, stores the result
 * at offset +0x10 of the parameter structure, and plays via func_80017AAC.
 *
 * @param a0 Pointer to the sound parameter structure.
 */
void sndPlaySoundByIndex(s32 *a0) {
    s32 local10, local14;

    func_80017C9C(&local10, &local14, a0[0]);
    a0[4] = sndGetBankGroup(D_80074ED8[a0[0]]);
    func_80017AAC(a0, local10, local14, 0);
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018784);

/** @brief Calls func_800174E4 with two words loaded from the input pointer. */
void sndStopTrack(s32 *a0) {
    func_800174E4(a0[0], a0[1]);
}

/**
 * @brief Clears 12 entries (stride 16) in the D_80074F20 array.
 * Writes zero to offset 0 of each entry, iterating backward.
 */
void sndClearVoicePool(void) {
    s32 i;
    for (i = 12; i != 0; i--) {
        D_80074F20[i - 1].cmd = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_bank", func_80018908);

/**
 * @brief Sets volume/pan on the active sequence and marks dirty tracks.
 *
 * Checks if a0[0] matches the active sequence ID. If so, writes the
 * volume (a0[1] & 0x7F shifted) and clears the pan fade counter on the
 * appropriate bank. Then calls func_80017D14 with the track array to
 * mark matching tracks dirty.
 *
 * @param a0 Pointer to a 2-word config struct: [seqId, volumeParam].
 */
void func_80018A74(s32 *args) {
    SoundMusicConfig *config;
    SoundSeqTrack *tracks;
    s32 id;

    id = args[0];
    if (id == 0 || id == g_sndSeqState->musicId) {
        config = g_sndSeqState;
        tracks = D_80070D60;
        config->volMul = (args[1] & 0x7F) << 16;
        config->volMulSlideSteps = 0;
        func_80017D14(config, tracks);
    } else if (D_80073CA8 != 0 && id != 0 && id == D_80073CA8->musicId) {
        config = D_80073CA8;
        tracks = D_80073C34;
        config->volMul = (args[1] & 0x7F) << 16;
        config->volMulSlideSteps = 0;
        func_80017D14(config, tracks);
    }
}

void func_80018B28(s32 *args) {
    SoundMusicConfig *config;
    SoundSeqTrack *tracks;
    s32 step;
    s32 steps;
    s32 target;
    s32 id;

    steps = 1;
    if (args[1] != 0) {
        steps = args[1];
    }
    target = (args[2] & 0x7F) << 16;
    id = args[0];
    if (id == 0 || id == g_sndSeqState->musicId) {
        config = g_sndSeqState;
        tracks = D_80070D60;
        config->volMulSlideSteps = steps;
        target -= config->volMul;
        step = target / steps;
        config->volMulSlideStep = step;
        func_80017D14(config, tracks);
    } else if (D_80073CA8 != 0 && id != 0 && id == D_80073CA8->musicId) {
        config = D_80073CA8;
        tracks = D_80073C34;
        config->volMulSlideSteps = steps;
        target -= config->volMul;
        step = target / steps;
        config->volMulSlideStep = step;
        func_80017D14(config, tracks);
    }
}

void func_80018C48(s32 *args) {
    SoundMusicConfig *config;
    SoundSeqTrack *tracks;
    s32 steps = 1;
    s32 id;
    s32 start;

    if (args[1] != 0) {
        steps = args[1];
    }
    id = args[0];
    if (id == 0 || id == g_sndSeqState->musicId) {
        config = g_sndSeqState;
        tracks = D_80070D60;
    } else if (D_80073CA8 != NULL && id != 0 && id == D_80073CA8->musicId) {
        config = D_80073CA8;
        tracks = D_80073C34;
    } else {
        return;
    }
    start = (args[2] & 0x7F) << 16;
    config->volMul = start;
    config->volMulSlideStep = (((args[3] & 0x7F) << 16) - start) / steps;
    config->volMulSlideSteps = steps;
    func_80017D14(config, tracks);
}

/**
 * @brief Clears the sound fade counter and sets SPU transfer address.
 *
 * Sets D_80074FE4 to 0, loads the halfword at @p a0, shifts it left 16,
 * stores to D_80075078, and calls sndSetCdVolume.
 *
 * @param a0 Pointer to a halfword containing the SPU address high bits.
 */
void sndSetFadeAndTransfer(u16 *a0) {
    D_80074FE4 = 0;
    D_80075078 = *a0 << 16;
    sndSetCdVolume();
}

/**
 * @brief Computes fade step from input parameters and stores to globals.
 *
 * Reads a duration from a0[0] (default 1 if zero), computes the per-tick
 * fade delta as ((a0[4] << 16) - D_80075078) / duration, stores duration
 * to D_80074FE4 and delta to D_80074FE0.
 *
 * @param a0 Pointer to a fade config structure.
 */
void func_80018D74(s32 *args) {
    s32 steps = 1;

    if (args[0] != 0) {
        steps = args[0];
    }
    D_80074FE4 = steps;
    D_80074FE0 = (((u16)args[1] << 16) - D_80075078) / steps;
}

void func_80018DDC(s32 *args) {
    s32 steps = 1;
    s32 target;
    s32 start;

    if (args[0] != 0) {
        steps = args[0];
    }
    target = (u16)args[2] << 16;
    start = (u16)args[1] << 16;
    D_80074FE4 = steps;
    D_80075078 = start;
    D_80074FE0 = (target - start) / steps;
}

void func_80018E4C(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    u16 val;

    if (args[1] != 0) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && (track->keyOnMask & args[1])) {
                val = args[2];
                track->volBalanceSlideSteps = 0;
                track->volBalance = (val & 0x7F) << 8;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && track->keyOffMask == args[0]) {
                val = args[2];
                track->volBalanceSlideSteps = 0;
                track->volBalance = (val & 0x7F) << 8;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
}

void func_80018F34(s32 *args) {
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 bit = 0x1000;
    u32 i;
    s16 steps;

    if (args[1] != 0) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && (track->keyOnMask & args[1])) {
                steps = 1;
                if (args[2] != 0) {
                    steps = (u16)args[2];
                }
                track->volBalanceSlideStep = (s16)((((u16)args[3] & 0x7F) << 8) - track->volBalance) / steps;
                track->volBalanceSlideSteps = steps;
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && track->keyOffMask == args[0]) {
                steps = 1;
                if (args[2] != 0) {
                    steps = (u16)args[2];
                }
                track->volBalanceSlideStep = (s16)((((u16)args[3] & 0x7F) << 8) - track->volBalance) / steps;
                track->volBalanceSlideSteps = steps;
            }
        }
    }
}

void func_800190B4(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    u16 val;
    s32 flags;

    for (i = 0; i < 12; i++, track++, bit <<= 1) {
        if ((active & bit) && !(track->keyOnMask & 0x2000000)) {
            val = args[0];
            flags = track->voiceAttr.updateFlags;
            track->volBalanceSlideSteps = 0;
            track->volBalance = (val & 0x7F) << 8;
            track->voiceAttr.updateFlags = flags | 3;
        }
    }
}

void func_80019130(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    s16 steps;

    for (i = 0; i < 12; i++, track++, bit <<= 1) {
        if ((active & bit) && !(track->keyOnMask & 0x2000000)) {
            steps = 1;
            if (args[0] != 0) {
                steps = (u16)args[0];
            }
            track->volBalanceSlideStep = (s16)((((u16)args[1] & 0x7F) << 8) - track->volBalance) / steps;
            track->volBalanceSlideSteps = steps;
        }
    }
}

void func_800191F8(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    s32 val;

    if (args[1] != 0) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && (track->keyOnMask & args[1])) {
                val = (u8)args[2];
                track->pitchModCounter = 0;
                track->pitchModDepth = val << 8;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && track->keyOffMask == args[0]) {
                val = (u8)args[2];
                track->pitchModCounter = 0;
                track->pitchModDepth = val << 8;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
}

void func_800192D8(s32 *args) {
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 bit = 0x1000;
    u32 i;
    s16 steps;

    if (args[1] != 0) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && (track->keyOnMask & args[1])) {
                steps = 1;
                if (args[2] != 0) {
                    steps = (u16)args[2];
                }
                track->unkDC = (s16)(((u8)args[3] << 8) - track->pitchModDepth) / steps;
                track->pitchModCounter = steps;
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && track->keyOffMask == args[0]) {
                steps = 1;
                if (args[2] != 0) {
                    steps = (u16)args[2];
                }
                track->unkDC = (s16)(((u8)args[3] << 8) - track->pitchModDepth) / steps;
                track->pitchModCounter = steps;
            }
        }
    }
}

/**
 * @brief Applies expression value to all active tracks matching a bitmask.
 *
 * Iterates 12 tracks in D_80072F70 (stride 0x110). For each track whose
 * bit is set in D_80075028.activeMask and whose flags do NOT have bit 0x2000000,
 * reads the expression byte from a0, clears the halfword at +0x8A,
 * stores (byte & 0x7F) << 8 at +0x6A, and ORs 0x3 into updateFlags.
 *
 * @param a0 Pointer to the expression value byte.
 */
void func_80019450(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    s32 val;
    s32 flags;

    for (i = 0; i < 12; i++, track++, bit <<= 1) {
        if ((active & bit) && !(track->keyOnMask & 0x2000000)) {
            val = (u8)args[0];
            flags = track->voiceAttr.updateFlags;
            track->pitchModCounter = 0;
            track->pitchModDepth = val << 8;
            track->voiceAttr.updateFlags = flags | 3;
        }
    }
}

void func_800194C8(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    s16 steps;

    for (i = 0; i < 12; i++, track++, bit <<= 1) {
        if ((active & bit) && !(track->keyOnMask & 0x2000000)) {
            steps = 1;
            if (args[0] != 0) {
                steps = (u16)args[0];
            }
            track->unkDC = (s16)(((u8)args[1] << 8) - track->pitchModDepth) / steps;
            track->pitchModCounter = steps;
        }
    }
}

void func_8001958C(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    s32 val;

    if (args[1] != 0) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && (track->keyOnMask & args[1])) {
                val = (u8)args[2];
                track->pitchMulSoundSlideSteps = 0;
                track->pitchMulSound = val << 8;
                track->voiceAttr.updateFlags |= 0x10;
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && track->keyOffMask == args[0]) {
                val = (u8)args[2];
                track->pitchMulSoundSlideSteps = 0;
                track->pitchMulSound = val << 8;
                track->voiceAttr.updateFlags |= 0x10;
            }
        }
    }
}

void func_8001966C(s32 *args) {
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 bit = 0x1000;
    u32 i;
    s16 steps;

    if (args[1] != 0) {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && (track->keyOnMask & args[1])) {
                steps = 1;
                if (args[2] != 0) {
                    steps = (u16)args[2];
                }
                track->pitchMulSoundSlideStep = (s16)((s16)(((u8)args[3] << 8) - (u16)track->pitchMulSound) / steps);
                track->pitchMulSoundSlideSteps = steps;
            }
        }
    } else {
        for (i = 0; i < 12; i++, track++, bit <<= 1) {
            if ((active & bit) && track->keyOffMask == args[0]) {
                steps = 1;
                if (args[2] != 0) {
                    steps = (u16)args[2];
                }
                track->pitchMulSoundSlideStep = (s16)((s16)(((u8)args[3] << 8) - (u16)track->pitchMulSound) / steps);
                track->pitchMulSoundSlideSteps = steps;
            }
        }
    }
}

/**
 * @brief Applies a note byte to all active tracks without flag 0x2000000.
 *
 * Iterates 12 tracks in D_80072F70 (stride 0x110). For each track whose
 * flags at +0x24 do NOT have bit 0x2000000 set, reads the note byte from
 * a0, clears the halfword at +0x84, shifts the byte left 8 and stores
 * at +0x3C, and ORs 0x10 into updateFlags at +0xF8.
 *
 * @param a0 Pointer to a byte containing the note value.
 */
void func_800197F4(u8 *param) {
    s32 i;
    SoundSeqTrack *track = (SoundSeqTrack *)D_80072F70;

    for (i = 12; i != 0; i--) {
        if (!(track->keyOnMask & 0x2000000)) {
            track->pitchMulSound = *param << 8;
            track->pitchMulSoundSlideSteps = 0;
            track->voiceAttr.updateFlags |= 0x10;
        }
        track++;
    }
}

void func_8001984C(s32 *args) {
    u32 bit = 0x1000;
    SoundSeqTrack *track = D_80072F70;
    u32 active = D_80075028.activeMask;
    u32 i;
    s16 steps;

    for (i = 0; i < 12; i++, track++, bit <<= 1) {
        if ((active & bit) && !(track->keyOnMask & 0x2000000)) {
            steps = 1;
            if (args[0] != 0) {
                steps = (u16)args[0];
            }
            track->pitchMulSoundSlideStep = (s16)((s16)(((u8)args[1] << 8) - (u16)track->pitchMulSound) / steps);
            track->pitchMulSoundSlideSteps = steps;
        }
    }
}

