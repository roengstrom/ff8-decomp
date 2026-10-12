#include "common.h"
#include "psxsdk/libspu.h"
#include "sound.h"
#include "snd_init.h"

void sndReleaseVoice(s32 a0);

void func_80014FFC(s32 voice, SndVoiceParam *param, s32 scale) {
    u16 *reg;
    s32 left;
    s32 right;

    param->updateFlags = 0;
    reg = (u16 *)(0x1F801C00 + (voice << 4));
    if (scale == 0) {
        left = param->volLeft;
        right = param->volRight;
    } else {
        left = param->volLeft * scale;
        left >>= 7;
        right = param->volRight * scale;
        right >>= 7;
    }
    *reg++ = left & 0x7FFF;
    *reg++ = right & 0x7FFF;
    *reg++ = param->sampleRate;
    *reg++ = param->startAddr >> 3;
    *reg++ = param->adsrLow;
    *reg = param->adsrHigh;
    reg[2] = param->repeatAddr >> 3;
}

void spuSetVoicePitch(s32 voice, s32 val);
void spuSetVoiceVolume(s32 voice, s32 vol_l, s32 vol_r, s32 scale);
void spuSetVoiceStartAddr(s32 voice, u32 val);
void spuSetVoiceRepeatAddr(s32 voice, u32 val);
void spuSetVoiceAdsrHigh(s32 voice, s32 val);
void spuSetVoiceAdsrLow(s32 voice, s32 val);

/**
 * @brief Applies pending SPU register updates for a voice.
 *
 * Reads the update flags from the voice parameter structure. For each
 * set flag, calls the corresponding SPU register function (pitch, volume,
 * sample address, repeat address, ADSR). Clears processed flags after each
 * call and exits early when all flags are handled.
 *
 * @param a0 SPU voice index (0-23).
 * @param a1 Pointer to the voice parameter structure.
 * @param a2 The owning track's flags (unused).
 */
void func_800150A8(s32 a0, SndVoiceParam *a1, s32 a2) {
    s32 flags = a1->updateFlags;

    if (flags == 0) {
        return;
    }
    a1->updateFlags = 0;

    if (flags & 0x10) {
        flags &= ~0x10;
        spuSetVoicePitch(a0, a1->sampleRate);
        if (flags == 0) {
            return;
        }
    }

    if (flags & 0x3) {
        flags &= ~0x3;
        spuSetVoiceVolume(a0, a1->volLeft, a1->volRight, a1->volScale);
        if (flags == 0) {
            return;
        }
    }

    if (flags & 0x80) {
        flags &= ~0x80;
        spuSetVoiceStartAddr(a0, a1->startAddr);
        if (flags == 0) {
            return;
        }
    }

    if (flags & 0x10000) {
        flags &= ~0x10000;
        spuSetVoiceRepeatAddr(a0, a1->repeatAddr);
        if (flags == 0) {
            return;
        }
    }

    if (flags & 0x6600) {
        flags &= ~0x6600;
        spuSetVoiceAdsrHigh(a0, a1->adsrHigh);
        if (flags == 0) {
            return;
        }
    }

    if (flags & 0x9900) {
        spuSetVoiceAdsrLow(a0, a1->adsrLow);
    }
}

void func_800151C4(SoundSeqTrack *track, u32 bit, s32 isSfx) {
    s32 val;
    u32 depth;
    u32 base;
    s16 *wave;

    if (track->volSlideSteps != 0) {
        track->volSlideSteps--;
        val = track->volumeLevel + track->volSlideStep;
        if ((val & 0xFFE00000) != (track->volumeLevel & 0xFFE00000)) {
            track->voiceAttr.updateFlags |= 3;
        }
        track->volumeLevel = val;
    }
    if (track->volPanSlideSteps != 0) {
        track->volPanSlideSteps--;
        val = track->volPan + track->volPanSlideStep;
        if ((val & 0xFF00) != (track->volPan & 0xFF00)) {
            track->voiceAttr.updateFlags |= 3;
        }
        track->volPan = val;
    }
    if (isSfx == 0) {
        if (track->volumeMultiplierSlideSteps != 0) {
            track->volumeMultiplierSlideSteps--;
            val = track->volumeMultiplier + track->volumeMultiplierSlideStep;
            if ((val & 0x7F00) != (track->volumeMultiplier & 0x7F00)) {
                track->voiceAttr.updateFlags |= 3;
            }
            track->volumeMultiplier = val;
        }
    }
    if (track->vibratoDelayCur != 0) {
        track->vibratoDelayCur--;
    }
    if (track->tremoloDelayCur != 0) {
        track->tremoloDelayCur--;
    }
    if (track->noiseSwitchDelay != 0 && --track->noiseSwitchDelay == 0) {
        if (isSfx == 0) {
            g_sndSeqState->noiseMask ^= bit;
        } else {
            D_80075028.noiseMask ^= bit;
        }
        D_80077288[2] |= 0x110;
    }
    if (track->pitchLfoSwitchDelay != 0 && --track->pitchLfoSwitchDelay == 0) {
        if (isSfx == 0) {
            g_sndSeqState->pitchLfoMask ^= bit;
        } else {
            D_80075028.pitchLfoMask ^= bit;
        }
        D_80077288[2] |= 0x100;
    }
    if (track->vibratoDepthSlideSteps != 0) {
        track->vibratoDepthSlideSteps--;
        track->vibratoDepth += track->vibratoDepthSlideStep;
        depth = (track->vibratoDepth & 0x7F00) >> 8;
        if (track->vibratoDepth & 0x8000) {
            base = (depth * track->basePitch) >> 7;
        } else {
            base = (depth * ((track->basePitch * 15) >> 8)) >> 7;
        }
        track->vibratoBase = base;
        if (track->vibratoDelayCur == 0 && track->vibratoRateCur != 1) {
            wave = track->vibratoWave;
            if (wave[0] == 0 && wave[1] == 0) {
                wave += wave[2];
            }
            val = (track->vibratoBase * wave[0]) >> 16;
            if (val != track->vibratoPitch) {
                track->vibratoPitch = val;
                track->voiceAttr.updateFlags |= 0x10;
                if (val >= 0) {
                    track->vibratoPitch = val * 2;
                }
            }
        }
    }
    if (track->tremoloDepthSlideSteps != 0) {
        track->tremoloDepthSlideSteps--;
        track->tremoloDepth += track->tremoloDepthSlideStep;
        if (track->tremoloDelayCur == 0 && track->tremoloRateCur != 1) {
            s32 vol;

            wave = track->tremoloWave;
            if (wave[0] == 0 && wave[1] == 0) {
                wave += wave[2];
            }
            vol = ((s16)(track->volumeLevel >> 16) * (track->volumeMultiplier >> 8)) >> 7;
            val = ((vol * (track->tremoloDepth >> 8)) << 9) >> 16;
            val = (val * wave[0]) >> 15;
            if (val != track->tremoloVol) {
                track->tremoloVol = val;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
    if (track->panLfoDepthSlideSteps != 0) {
        track->panLfoDepthSlideSteps--;
        track->panLfoDepth += track->panLfoDepthSlideStep;
        if (track->panLfoRateCur != 1) {
            wave = track->panLfoWave;
            if (wave[0] == 0 && wave[1] == 0) {
                wave += wave[2];
            }
            val = ((track->panLfoDepth >> 8) * wave[0]) >> 15;
            if (val != track->panLfoVol) {
                track->panLfoVol = val;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
    if (track->pitchSlideSteps != 0) {
        track->pitchSlideSteps--;
        val = track->pitchSlide + track->pitchSlideStep;
        if ((val & 0xFFFF0000) != (track->pitchSlide & 0xFFFF0000)) {
            track->voiceAttr.updateFlags |= 0x10;
        }
        track->pitchSlide = val;
    }
}

void func_80015668(SoundSeqTrack *track, u32 bit) {
    s32 vol;
    s32 pitch;
    s32 val;
    s32 flags;
    s16 *wave;
    s32 mono;
    s32 value;

    vol = ((s16)(track->volumeLevel >> 16) * (track->volumeMultiplier >> 8)) >> 7;
    flags = track->flags;
    if (flags & 1) {
        if (track->vibratoDelayCur == 0) {
            if (--track->vibratoRateCur == 0) {
                track->vibratoRateCur = track->vibratoRate;
                wave = track->vibratoWave;
                if (wave[0] == 0 && wave[1] == 0) {
                    track->vibratoWave = wave + wave[2];
                }
                value = *track->vibratoWave++;
                val = (track->vibratoBase * value) >> 16;
                if (val != track->vibratoPitch) {
                    track->vibratoPitch = val;
                    track->voiceAttr.updateFlags |= 0x10;
                    if (val >= 0) {
                        track->vibratoPitch = val * 2;
                    }
                }
            }
        }
    }
    if (flags & 2) {
        if (track->tremoloDelayCur == 0) {
            if (--track->tremoloRateCur == 0) {
                track->tremoloRateCur = track->tremoloRate;
                wave = track->tremoloWave;
                if (wave[0] == 0 && wave[1] == 0) {
                    track->tremoloWave = wave + wave[2];
                }
                value = *track->tremoloWave++;
                val = ((vol * (track->tremoloDepth >> 8)) << 9) >> 16;
                val = (val * value) >> 15;
                if (val != track->tremoloVol) {
                    track->tremoloVol = val;
                    track->voiceAttr.updateFlags |= 3;
                }
            }
        }
    }
    if (flags & 4) {
        if (--track->panLfoRateCur == 0) {
            track->panLfoRateCur = track->panLfoRate;
            wave = track->panLfoWave;
            if (wave[0] == 0 && wave[1] == 0) {
                track->panLfoWave = wave + wave[2];
            }
            value = *track->panLfoWave++;
            val = ((track->panLfoDepth >> 8) * value) >> 15;
            if (val != track->panLfoVol) {
                track->panLfoVol = val;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
    if (flags & 0x20) {
        vol = ((s16)(track[-1].voiceAttr.sampleRate << 1) * (track->volumeMultiplier >> 8)) >> 7;
        track->voiceAttr.updateFlags |= 3;
    }
    if (track->voiceAttr.updateFlags & 3) {
        vol += track->tremoloVol;
        vol = (vol * (s32)(((u32)g_sndSeqState->volMul >> 16) & 0x7F)) >> 7;
        val = ((track->volPan >> 8) + track->panLfoVol) & 0xFF;
        if (D_8007728C & 2) {
            mono = (vol * D_80051954) >> 15;
            track->voiceAttr.volRight = mono;
            track->voiceAttr.volLeft = mono;
        } else {
            track->voiceAttr.volLeft = (vol * D_80051854[val]) >> 15;
            track->voiceAttr.volRight = (vol * D_80051854[val ^ 0xFF]) >> 15;
        }
    }
    if (flags & 0x10) {
        pitch = track[-1].voiceAttr.sampleRate + track->vibratoPitch + (s16)(track->pitchSlide >> 16);
        val = D_80077280 & 0xFF0000;
        if (val != 0) {
            val >>= 16;
            if (val < 0x80) {
                pitch += (pitch * val) >> 7;
            } else {
                pitch = (pitch * val) >> 8;
            }
        }
        track->voiceAttr.sampleRate = ((u16)track->fineTunePitch + pitch) & 0x3FFF;
        track->voiceAttr.updateFlags |= 0x10;
    } else if (track->voiceAttr.updateFlags & 0x10) {
        pitch = track->basePitch + track->vibratoPitch + (s16)(track->pitchSlide >> 16);
        val = D_80077280 & 0xFF0000;
        if (val != 0) {
            val >>= 16;
            if (val < 0x80) {
                pitch += (pitch * val) >> 7;
            } else {
                pitch = (pitch * val) >> 8;
            }
        }
        track->voiceAttr.sampleRate = ((u16)track->fineTunePitch + pitch) & 0x3FFF;
    }
}

void func_80015AC8(SoundSeqTrack *track, u32 bit) {
    s32 vol;
    s32 pitch;
    s32 val;
    s32 mul;
    s32 pan;
    s32 flags;
    s16 *wave;
    s32 mono;
    s32 value;

    vol = ((s16)(track->volumeLevel >> 16) * (track->volumeMultiplier >> 8)) >> 7;
    flags = track->flags;
    if (flags & 1) {
        if (--track->vibratoRateCur == 0) {
            track->vibratoRateCur = track->vibratoRate;
            wave = track->vibratoWave;
            if (wave[0] == 0 && wave[1] == 0) {
                track->vibratoWave = wave + wave[2];
            }
            value = *track->vibratoWave++;
            val = (track->vibratoBase * value) >> 16;
            if (val != track->vibratoPitch) {
                track->vibratoPitch = val;
                track->voiceAttr.updateFlags |= 0x10;
                if (val >= 0) {
                    track->vibratoPitch = val * 2;
                }
            }
        }
    }
    if (flags & 2) {
        if (--track->tremoloRateCur == 0) {
            track->tremoloRateCur = track->tremoloRate;
            wave = track->tremoloWave;
            if (wave[0] == 0 && wave[1] == 0) {
                track->tremoloWave = wave + wave[2];
            }
            val = ((vol * (track->tremoloDepth >> 8)) << 9) >> 16;
            value = *track->tremoloWave++;
            val = (val * value) >> 15;
            if (val != track->tremoloVol) {
                track->tremoloVol = val;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
    if (flags & 4) {
        if (--track->panLfoRateCur == 0) {
            track->panLfoRateCur = track->panLfoRate;
            wave = track->panLfoWave;
            if (wave[0] == 0 && wave[1] == 0) {
                track->panLfoWave = wave + wave[2];
            }
            value = *track->panLfoWave++;
            val = ((track->panLfoDepth >> 8) * value) >> 15;
            if (val != track->panLfoVol) {
                track->panLfoVol = val;
                track->voiceAttr.updateFlags |= 3;
            }
        }
    }
    if (flags & 0x20) {
        vol = ((s16)(track[-1].voiceAttr.sampleRate << 1) * (track->volumeMultiplier >> 8)) >> 7;
        track->voiceAttr.updateFlags |= 3;
    }
    if (track->voiceAttr.updateFlags & 3) {
        vol += track->tremoloVol;
        if (track->keyOnMask & 0x2000000) {
            pan = 0x80;
        } else {
            vol = (vol * (s8)(track->volBalance >> 8)) >> 7;
            pan = ((track->volPan + track->pitchModDepth) >> 8) + track->panLfoVol + 0x80;
            pan &= 0xFF;
        }
        if (D_8007728C & 2) {
            mono = (vol * D_80051954) >> 15;
            track->voiceAttr.volRight = mono;
            track->voiceAttr.volLeft = mono;
        } else {
            track->voiceAttr.volLeft = (vol * D_80051854[pan]) >> 15;
            track->voiceAttr.volRight = (vol * D_80051854[pan ^ 0xFF]) >> 15;
        }
    }
    if (flags & 0x10) {
        pitch = track[-1].voiceAttr.sampleRate + track->vibratoPitch + (s16)(track->pitchSlide >> 16);
        mul = track->pitchMulSound & 0xFF00;
        if (!(track->keyOnMask & 0x2000000)) {
            if (mul != 0) {
                mul >>= 8;
                if (mul < 0x80) {
                    pitch += (pitch * mul) >> 7;
                } else {
                    pitch = (pitch * mul) >> 8;
                }
            }
        }
        track->voiceAttr.sampleRate = ((u16)track->fineTunePitch + pitch) & 0x3FFF;
        track->voiceAttr.updateFlags |= 0x10;
    } else if (track->voiceAttr.updateFlags & 0x10) {
        pitch = track->basePitch + track->vibratoPitch + (s16)(track->pitchSlide >> 16);
        mul = track->pitchMulSound & 0xFF00;
        if (!(track->keyOnMask & 0x2000000)) {
            if (mul != 0) {
                mul >>= 8;
                if (mul < 0x80) {
                    pitch += (pitch * mul) >> 7;
                } else {
                    pitch = (pitch * mul) >> 8;
                }
            }
        }
        track->voiceAttr.sampleRate = ((u16)track->fineTunePitch + pitch) & 0x3FFF;
    }
}

/**
 * @brief Finds the SPU voice with the lowest priority and frees it for reuse.
 *
 * Scans all 24 voice entries in the D_80073CB0 array (stride 8, priority
 * at offset +4) to find the voice with the lowest priority value. If found,
 * calls sndReleaseVoice to release it from all active sequences.
 *
 * @return The index of the freed voice, or 0x18 (24) if no voice is available.
 */
s32 sndAllocVoice(void) {
    u32 i;
    u16 min_val;
    u32 min_idx;
    SndVoiceEnvelope *p;

    i = 0;
    min_val = 0x7FFF;
    min_idx = 0x18;
    p = D_80073CB0;

    do {
        if (p->envelope < (s16)min_val) {
            min_val = p->envelope;
            min_idx = i;
        }
        i++;
        p++;
    } while (i < 0x18);

    if ((s16)min_val == 0x7FFF) {
        return 0x18;
    }
    if (min_idx < 0x18) {
        sndReleaseVoice(min_idx);
    }
    return min_idx;
}

void func_80015FE0(SoundSeqTrack *track, u32 active, u32 newVoices, s32 *keyOn) {
    SndVoiceEnvelope *env;
    u32 keyed;
    u32 bit;
    s32 i;
    u32 voice;

    bit = 1;
    i = 0;
    keyed = active & g_sndSeqState->unk10;
    do {
        if (active & bit) {
            func_80015668(track, bit);
            if (track->voiceAttr.updateFlags != 0) {
                if (D_8007507C & bit) {
                    track->voiceAttr.volRight = 0;
                    track->voiceAttr.volLeft = 0;
                }
                if (keyed & bit) {
                    if (newVoices & bit) {
                        *keyOn |= 1 << i;
                        track->voiceAttr.voice = i;
                    } else {
                        env = D_80073CB0;
                        voice = 0;
                        do {
                            if (env->envelope == 0) {
                                *keyOn |= 1 << voice;
                                track->voiceAttr.voice = voice;
                                env->envelope = 0x7FFF;
                                break;
                            }
                            voice++;
                            env++;
                            if (voice == 24) {
                                g_sndSeqState->unk00 |= 2;
                                voice = sndAllocVoice();
                                if (voice != 24) {
                                    *keyOn |= 1 << voice;
                                    track->voiceAttr.voice = voice;
                                    D_80073CB0[voice].envelope = 0x7FFF;
                                } else {
                                    track->voiceAttr.voice = voice;
                                    g_sndSeqState->unk00 |= 1;
                                }
                                break;
                            }
                        } while (voice < 24);
                    }
                    voice = track->voiceAttr.voice;
                    if (voice < 24) {
                        func_80014FFC(voice, &track->voiceAttr, track->voiceAttr.volScale);
                        D_800772F8[track->voiceAttr.voice] = g_sndSeqState;
                        D_80077288[2] |= 0x100;
                    }
                } else {
                    voice = track->voiceAttr.voice;
                    if (voice < 24) {
                        func_800150A8(voice, &track->voiceAttr, track->flags);
                    }
                }
            }
            active &= ~bit;
        }
        bit <<= 1;
        track++;
        i++;
    } while (active != 0);
}


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
void sndReleaseVoice(s32 a0) {
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
s32 sndCheckVoiceInMask(SoundSeqTrack *tracks, s32 mask, s32 voice) {
    u32 i = 0;
    s32 bit = 1;
    do {
        if (voice == tracks->voiceAttr.voice) {
            if (mask & (bit << i)) {
                return 1;
            }
        }
        i++;
        tracks++;
    } while (i < 0x20);
    return 0;
}

void func_80016344(u32 mask) {
    SndVoiceEnvelope *entry;
    u32 used = (g_sndSeqState->activeMask & g_sndSeqState->unk0C) | mask;
    u32 i;

    if (D_80073CA8 != NULL) {
        used |= D_80073CA8->activeMask & D_80073CA8->unk0C;
    }
    entry = D_80073CB0;
    for (i = 0; i < 24; entry++, i++) {
        if (used & (1 << i)) {
            entry->envelope = 0x7FFF;
        } else {
            SpuGetVoiceEnvelope(i, &entry->envelope);
            if (entry->envelope == 0) {
                sndReleaseVoice(i);
            } else if (g_sndSeqState->unk08 != 0) {
                if (sndCheckVoiceInMask(D_80070D60, g_sndSeqState->unk08, i)) {
                    entry->envelope = 0x7FFF;
                }
                if (D_80073CA8 != NULL && sndCheckVoiceInMask(D_80073C34, D_80073CA8->unk08, i)) {
                    entry->envelope = 0x7FFF;
                }
            }
        }
    }
}

void func_80016478(SoundMusicConfig *config, SoundSeqTrack *tracks) {
    s32 level = config->volMul;

    D_80074F10.unk0C = level;
    level >>= 16;
    level *= D_80074F10.unk00 >> 8;
    level /= 127;
    level <<= 8;
    config->volMul = level;
}

extern s32 D_80074F1C;
/**
 * @brief Copies D_80074F1C into @p config's volMul.
 *
 * @param config Music config to update.
 * @param tracks The config's track array (unused).
 */
void sndStoreTrackTiming(SoundMusicConfig *config, SoundSeqTrack *tracks) {
    config->volMul = D_80074F1C;
}

INCLUDE_ASM("asm/nonmatchings/snd_voice", func_800164D8);

/**
 * @brief Builds a voice bitmask from active tracks matching a track bitmask.
 *
 * For each set bit in @p trackMask, reads the corresponding track's voice
 * assignment (offset +0xF4). If the voice index is valid (< 24), sets that
 * voice's bit in *resultPtr. After processing, masks the result with @p finalMask.
 *
 * @param tracks Base pointer to the track array (stride 0x110).
 * @param resultPtr Pointer to the output bitmask (accumulated via OR).
 * @param trackMask Bitmask of tracks to check.
 * @param finalMask AND mask applied to the result after accumulation.
 */
void func_80016A1C(SoundSeqTrack *tracks, u32 *resultPtr, u32 trackMask, u32 finalMask) {
    s32 bit = 1;
    s32 one = 1;

    do {
        if (trackMask & bit) {
            s32 voice = tracks->voiceAttr.voice;
            if ((u32)voice < 0x18) {
                *resultPtr |= one << voice;
            }
        }
        trackMask &= ~bit;
        tracks++;
        bit <<= 1;
    } while (trackMask != 0);

    *resultPtr &= finalMask;
}

void func_80016A7C(void) {
    u32 result = 0;
    u32 finalMask;
    u32 pending2 = 0;
    u32 pending1;

    finalMask = ~((D_80075028.activeMask | D_80075028.activeMaskStored) | D_800772A4);
    if (D_80073CA8 != NULL) {
        pending2 = D_80073CA8->unk18;
        if (pending2 & D_80073CA8->unk08) {
            func_80016A1C(D_80073C34, &result, pending2 & D_80073CA8->unk08, finalMask);
            D_80073CA8->unk18 &= ~D_80073CA8->unk08;
            pending2 &= ~D_80073CA8->unk08;
        }
    }
    pending1 = g_sndSeqState->unk18;
    if (pending1 & g_sndSeqState->unk08) {
        func_80016A1C(D_80070D60, &result, pending1 & g_sndSeqState->unk08, finalMask);
        g_sndSeqState->unk18 &= ~g_sndSeqState->unk08;
        pending1 &= ~g_sndSeqState->unk08;
    }
    if (D_80073CA8 != NULL && pending2 != 0) {
        func_80016A1C(D_80073C34, &result, pending2, finalMask);
        D_80073CA8->unk18 = 0;
    }
    if (pending1 != 0) {
        func_80016A1C(D_80070D60, &result, pending1, finalMask);
        g_sndSeqState->unk18 = 0;
    }
    result |= D_80075028.unk0C;
    D_80075028.unk0C = 0;
    if (result != 0) {
        spuSetIrqAddr(result);
    }
}

void func_80016C28(u32 *out, u32 maskSecond, u32 maskMain, u32 extra) {
    u32 result = 0;
    u32 finalMask;
    u32 pending2 = 0;
    u32 pending1;

    finalMask = ~((D_80075028.activeMask | D_80075028.activeMaskStored) | D_800772A4);
    if (D_80073CA8 != NULL) {
        pending2 = D_80073CA8->activeMask & maskSecond;
        if (pending2 & D_80073CA8->unk08) {
            func_80016A1C(D_80073C34, &result, pending2 & D_80073CA8->unk08, finalMask);
            pending2 &= ~D_80073CA8->unk08;
        }
    }
    pending1 = g_sndSeqState->activeMask & maskMain;
    if (pending1 & g_sndSeqState->unk08) {
        func_80016A1C(D_80070D60, &result, pending1 & g_sndSeqState->unk08, finalMask);
        pending1 &= ~g_sndSeqState->unk08;
    }
    if (D_80073CA8 != NULL && pending2 != 0) {
        func_80016A1C(D_80073C34, &result, pending2, finalMask);
    }
    if (pending1 != 0) {
        func_80016A1C(D_80070D60, &result, pending1, finalMask);
    }
    result |= extra;
    *out = result;
    D_80077288[2] |= 0x100;
}

