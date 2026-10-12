#include "common.h"
#include "sound.h"
#include "snd_init.h"
#include "snd_dma.h"

extern s16 D_8007507A;
/**
 * @brief Sets the CD audio input volume on the SPU to the current global value.
 *
 * Reads the volume level from D_8007507A and writes it to both the left
 * (0x1F801DB0) and right (0x1F801DB2) CD audio volume registers, producing
 * equal stereo volume.
 */
void sndSetCdVolume(void) {
    s32 val = D_8007507A;
    *(s16 *)0x1F801DB0 = val;
    *(s16 *)0x1F801DB2 = val;
}

void func_8001A57C(u32 *src, u32 *dst, u32 size) {
    u32 w0;
    u32 w1;
    u32 w2;
    u32 w3;

    size >>= 2;
    while (size >> 2) {
        w0 = src[0];
        w1 = src[1];
        w2 = src[2];
        w3 = src[3];
        dst[0] = w0;
        dst[1] = w1;
        dst[2] = w2;
        dst[3] = w3;
        src += 4;
        dst += 4;
        size -= 4;
    }
    while (size != 0) {
        *dst++ = *src++;
        size--;
    }
}

void func_8001A5FC(u32 *a, u32 *b, u32 size) {
    u32 a0;
    u32 b0;
    u32 a1;
    u32 b1;

    size >>= 2;
    while (size >> 1) {
        a0 = a[0];
        b0 = b[0];
        a1 = a[1];
        b1 = b[1];
        b[0] = a0;
        b[1] = a1;
        a[0] = b0;
        a[1] = b1;
        a += 2;
        b += 2;
        size -= 2;
    }
    if (size != 0) {
        a0 = *a;
        b0 = *b;
        *b = a0;
        *a = b0;
    }
}

void func_8001A674(void) {
    SoundMusicConfig *config;
    SoundStream *stream;
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    s32 val;
    s32 vol;

    if (D_80074FE4 != 0) {
        D_80074FE4--;
        D_80075078 += D_80074FE0;
        sndSetCdVolume();
    }
    stream = &g_sndStream;
    if (stream->active != 0 && stream->panTarget != 0) {
        stream->panTarget--;
        vol = stream->panVolume;
        val = vol + stream->panStep;
        if ((val & 0xFF00) != (vol & 0xFF00)) {
            if (D_8007728C & 2) {
                vol = (vol * D_80051954) >> 16;
                spuSetVoiceVolume(stream->voiceIdx, vol, vol, 0);
                spuSetVoiceVolume(stream->voiceIdx + 1, vol, vol, 0);
            } else {
                vol = val << 15;
                vol >>= 16;
                spuSetVoiceVolume(stream->voiceIdx, vol, 0, 0);
                spuSetVoiceVolume(stream->voiceIdx + 1, 0, vol, 0);
            }
        }
        D_800772D8 = val & 0xFFFF;
    }
    if (D_80073E62 != 0) {
        D_80073E62--;
        D_80077284 += D_80073DEC;
    }
    if (D_80073E60 != 0) {
        D_80073E60--;
        val = D_80077280 + D_80073DE8;
        if ((val & 0xFF0000) != (D_80077280 & 0xFF0000)) {
            for (bit = 32, track = D_80070D60; bit != 0; bit--, track++) {
                track->voiceAttr.updateFlags |= 0x10;
            }
        }
        D_80077280 = val;
    }
    if (g_sndSeqState->activeMask != 0 && g_sndSeqState->volMulSlideSteps != 0) {
        g_sndSeqState->volMulSlideSteps--;
        val = g_sndSeqState->volMul + g_sndSeqState->volMulSlideStep;
        if ((val & 0x7F0000) != (g_sndSeqState->volMul & 0x7F0000)) {
            func_80017D14(g_sndSeqState, D_80070D60);
        }
        g_sndSeqState->volMul = val;
    }
    config = D_80073CA8;
    if (config != 0 && config->activeMask != 0 && config->volMulSlideSteps != 0) {
        config->volMulSlideSteps--;
        val = config->volMul + config->volMulSlideStep;
        if ((val & 0x7F0000) != (config->volMul & 0x7F0000)) {
            func_80017D14(config, D_80073C34);
        }
        D_80073CA8->volMul = val;
    }
    if (D_80075028.activeMask != 0) {
        active = D_80075028.activeMask;
        track = D_80072F70;
        bit = 0x1000;
        do {
            if (active & bit) {
                if (track->volBalanceSlideSteps != 0) {
                    track->volBalanceSlideSteps--;
                    val = (s16)track->volBalance + track->volBalanceSlideStep;
                    if ((val & 0xFF00) != ((s16)track->volBalance & 0xFF00)) {
                        track->voiceAttr.updateFlags |= 3;
                    }
                    track->volBalance = val;
                }
                if (track->pitchModCounter != 0) {
                    track->pitchModCounter--;
                    val = track->pitchModDepth + track->unkDC;
                    if ((val & 0xFF00) != (track->pitchModDepth & 0xFF00)) {
                        track->voiceAttr.updateFlags |= 3;
                    }
                    track->pitchModDepth = val;
                }
                if (track->pitchMulSoundSlideSteps != 0) {
                    track->pitchMulSoundSlideSteps--;
                    val = track->pitchMulSound + track->pitchMulSoundSlideStep;
                    if ((val & 0xFF00) != (track->pitchMulSound & 0xFF00)) {
                        track->voiceAttr.updateFlags |= 0x10;
                    }
                    track->pitchMulSound = val;
                }
                active ^= bit;
            }
            bit <<= 1;
            track++;
        } while (active != 0);
    }
}

u32 func_8001AA28(SoundSeqTrack *tracks, s32 isSecond) {
    SoundMusicConfig *config;
    SoundSeqTrack *track;
    u32 active;
    u32 bit;
    u32 tempo;
    u32 mul;
    u32 update;
    u32 flags;

    tempo = (u16)(g_sndSeqState->tempo >> 16);
    mul = D_80077286;
    if (mul != 0) {
        if (mul < 0x80) {
            tempo += (tempo * mul) >> 7;
        } else {
            tempo = (tempo * mul) >> 8;
        }
    }
    config = g_sndSeqState;
    update = config->unk28 + tempo;
    config->unk28 = update;
    if ((update & 0xFFFF0000) != 0 || (D_800772F4 & 4)) {
        config->unk28 = update & 0xFFFF;
        do {
            track = tracks;
            bit = 1;
            active = g_sndSeqState->activeMask;
            do {
                if (active & bit) {
                    track->volume--;
                    track->volumeBase--;
                    if (track->volume == 0) {
                        func_8001BAA8(track, bit);
                    } else if (track->volumeBase == 0) {
                        g_sndSeqState->unk18 |= bit;
                    }
                    func_800151C4(track, bit, 0);
                    active &= ~bit;
                }
                track++;
                bit <<= 1;
            } while (active != 0);
            config = g_sndSeqState;
            if (config->tempoSlideSteps != 0) {
                config->tempoSlideSteps--;
                config->tempo += config->tempoSlideStep;
            }
            config = g_sndSeqState;
            if (config->reverbDepthSlideSteps != 0) {
                config->reverbDepthSlideSteps--;
                config->reverbDepth += config->reverbDepthSlideStep;
                flags = D_80077288[2];
                if (isSecond == 0) {
                    do { D_80077288[2] = flags | 0x80; } while (0);
                }
            }
            if (g_sndSeqState->timerLower != 0) {
                if (++g_sndSeqState->timerLowerCur == g_sndSeqState->timerLower) {
                    g_sndSeqState->timerLowerCur = 0;
                    if (++g_sndSeqState->timerUpperCur == g_sndSeqState->timerUpper) {
                        g_sndSeqState->timerUpperCur = 0;
                        g_sndSeqState->timerTopCur++;
                        if (isSecond == 0 && D_80074EB0 != 0) {
                            D_80074EB0--;
                        }
                    }
                }
            }
        } while (isSecond == 0 && D_80074EB0 != 0);
    }
    return g_sndSeqState->activeMask;
}

void func_8001ACCC(SoundMusicConfig *config, SoundSeqTrack *tracks) {
    s32 savedUnk04;
    s32 newVal;

    if (D_80074F10.unk08 != 0) {
        if (--D_80074F10.unk08 == 0) {
            savedUnk04 = config->activeMask;
            config->musicId = 0;
            config->activeMask = 0;
            config->unk10 = 0;
            config->unk14 = 0;
            config->unk18 = savedUnk04;
        } else {
            newVal = D_80074F10.unk00 + D_80074F10.unk04;

            if ((newVal & 0xFFFF0000) != (D_80074F10.unk00 & 0xFFFF0000)) {
                func_80017D14(config, tracks);
            }

            D_80074F10.unk00 = newVal;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001AD60);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B1F4);

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
s32 sndAdjustNoteOctave(s32 a0, s32 a1) {
    if ((a0 & 0x400) && (u32)a1 >= 0x40 && (u32)a1 < 0x80) {
        return a1 + 0x20;
    }
    return a1;
}

void func_8001B42C(SoundSeqTrack *track, u32 key) {
    SndKeySplitEntry *entry;
    SndInstrument *inst;
    s32 flags;
    s32 id;

    if ((u32)track->unkE8 < key || track->unkE8 == 0xFF) {
        entry = (SndKeySplitEntry *)track->drumOffset;
        if (entry[1].mode != 0) {
            while (1) {
                if (!(entry->keyLow < key)) {
                    break;
                }
                entry++;
                if (entry[1].mode == 0) {
                    break;
                }
                // Matching hack, never true
                if (!track) {
                }
            }
        }
    } else if (key < (u32)track->unkE8) {
        entry = (SndKeySplitEntry *)track->drumOffset;
        if (entry[1].mode != 0) {
            while (1) {
                if (key < entry[1].keyHigh) {
                    break;
                }
                entry++;
                if (entry[1].mode == 0) {
                    break;
                }
            }
        }
    } else {
        return;
    }
    flags = track->flags;
    id = sndAdjustNoteOctave(g_sndSeqState->unk00, entry->instrument);
    track->instrument = id;
    inst = &D_80073E68[id];
    track->voiceAttr.startAddr = inst->sampleAddr;
    track->voiceAttr.repeatAddr = inst->loopAddr;
    if (!(flags & 0x1000000)) {
        track->voiceAttr.adsrLow = entry->adsr0 << 8;
    } else {
        track->voiceAttr.adsrLow &= 0x7F00;
    }
    track->voiceAttr.adsrLow |= inst->adsrLow & 0x80FF;
    if (!(flags & 0x8000000)) {
        track->voiceAttr.adsrHigh &= 0x201F;
        track->voiceAttr.adsrHigh |= entry->adsr1 << 6;
    } else {
        track->voiceAttr.adsrHigh &= 0x3FDF;
    }
    switch (entry->mode) {
    case 3:
        track->voiceAttr.adsrHigh |= 0x4000;
        break;
    case 5:
        track->voiceAttr.adsrHigh |= 0x8000;
        break;
    case 7:
        track->voiceAttr.adsrHigh |= 0xC000;
        break;
    }
    if (!(flags & 0x10000000)) {
        track->voiceAttr.adsrHigh &= 0xFFE0;
        track->voiceAttr.adsrHigh |= entry->adsr2;
    }
    track->voiceAttr.adsrHigh |= inst->adsrHigh & 0x20;
    track->voiceAttr.volScale = entry->volScale;
}

s32 func_8001B690(SndInstrument *inst, s32 key, u32 fineTuning, s32 *out) {
    s32 pitch;
    s32 note;
    s32 octave;
    s32 scale;
    note = key - inst->unk0A;
    while (note < 0) {
        note += 12;
    }
    octave = note / 12;
    note -= octave * 12;
    scale = inst->unk08;
    if (scale == 0) {
        pitch = D_800517F4[note] << 8;
    } else if (scale < 0) {
        pitch = ((u32)D_800517F4[note] * (u16)inst->unk08) >> 8;
    } else {
        pitch = ((u32)D_800517F4[note] * scale) >> 7;
        pitch += D_800517F4[note] << 8;
    }
    fineTuning &= 0xFF;
    if (fineTuning != 0) {
        if (fineTuning < 0x80) {
            *out = ((u32)pitch * fineTuning) >> 7;
        } else {
            *out = (((u32)pitch * fineTuning) >> 8) - pitch;
        }
    }
    if (key < inst->unk0A) {
        do {
            *out >>= 1;
            pitch >>= 1;
            key += 12;
        } while (key < inst->unk0A);
    } else {
        note = (key - inst->unk0A) / 12;
        if (note != 0) {
            *out <<= note;
            pitch <<= note;
        }
    }
    *out >>= 8;
    pitch >>= 8;
    return pitch & 0xFFFF;
}

s32 func_8001B820(SoundSeqTrack *track, u32 bit, s32 idx) {
    SndDrumEntry *entry;
    SndInstrument *inst;
    s32 flags;
    s32 id;
    s32 pitch;

    entry = g_sndSeqState->drumMap;
    entry += idx;
    g_sndSeqState->unk10 |= bit;
    if (g_sndSeqState->unk14 & bit) {
        g_sndSeqState->unk18 |= bit;
    }
    flags = track->flags;
    id = sndAdjustNoteOctave(g_sndSeqState->unk00, entry->instrument);
    track->instrument = id;
    inst = &D_80073E68[id];
    track->voiceAttr.startAddr = inst->sampleAddr;
    track->voiceAttr.repeatAddr = inst->loopAddr;
    if (!(flags & 0x1000000)) {
        track->voiceAttr.adsrLow = entry->adsr0 << 8;
    } else {
        track->voiceAttr.adsrLow &= 0x7F00;
    }
    track->voiceAttr.adsrLow |= inst->adsrLow & 0x80FF;
    if (!(flags & 0x8000000)) {
        track->voiceAttr.adsrHigh &= 0x201F;
        track->voiceAttr.adsrHigh |= entry->adsr1 << 6;
    } else {
        track->voiceAttr.adsrHigh &= 0x3FDF;
    }
    switch (entry->mode) {
    case 3:
        track->voiceAttr.adsrHigh |= 0x4000;
        break;
    case 5:
        track->voiceAttr.adsrHigh |= 0x8000;
        break;
    case 7:
        track->voiceAttr.adsrHigh |= 0xC000;
        break;
    }
    if (!(flags & 0x10000000)) {
        track->voiceAttr.adsrHigh &= 0xFFE0;
        track->voiceAttr.adsrHigh |= entry->adsr2;
    }
    track->voiceAttr.adsrHigh |= inst->adsrHigh & 0x20;
    pitch = func_8001B690(inst, entry->key, track->fineTuning, &track->fineTunePitch);
    track->voiceAttr.volScale = entry->volScale;
    track->volPan = ((entry->pan & 0x7F) + 0x40) << 8;
    if ((s8)entry->pan < 0) {
        g_sndSeqState->reverbMask |= bit;
    } else {
        g_sndSeqState->reverbMask &= ~bit;
    }
    D_80077288[2] |= 0x100;
    return pitch;
}

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001BAA8);
