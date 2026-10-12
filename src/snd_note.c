#include "common.h"
#include "sound.h"
#include "snd_init.h"

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

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001A674);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001AA28);

extern void func_80017D14(SoundMusicConfig *config, SoundSeqTrack *tracks);

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

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B42C);

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
