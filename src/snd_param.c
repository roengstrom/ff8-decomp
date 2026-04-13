#include "common.h"
#include "sound.h"

extern s32 *D_80074F08;
extern s32 D_80075028;
extern u16 D_80073E62;
extern s32 D_80077284;
extern u16 D_80073E60;
extern s32 D_80077280;
extern u8 D_80070D60[];
extern s32 D_80073CA8;
extern u8 *D_80073C34;
extern s32 D_8007728C;
extern u8 D_80072F70[];
extern s32 D_800772A4;
extern s32 D_800772F4;
extern u8 D_80077298[];
extern s32 D_8007507C;

/**
 * @brief Clears D_80073E62, stores *a0 << 16 into D_80077284.
 * @param a0 Pointer to a byte value.
 */
void sndSetVolumeFade(s8 *a0) {
    s32 val = *a0;
    D_80073E62 = 0;
    D_80077284 = val << 16;
}

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019934);

INCLUDE_ASM("asm/nonmatchings/snd_param", func_8001999C);

/**
 * @brief Clears D_80073E60, stores *a0 << 16 into D_80077280.
 * @param a0 Pointer to a byte value.
 */
void sndSetPanFade(s8 *a0) {
    s32 val = *a0;
    D_80073E60 = 0;
    D_80077280 = val << 16;
}

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019A2C);

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019A94);

/**
 * @brief Transfers sound data for main and optional secondary sound sources.
 *
 * Calls func_80017410 for the primary sound source (D_80074F08 with D_80070D60),
 * then conditionally for a secondary source (D_80073CA8 with D_80073C34) if set.
 */
void sndTransferData(void) {
    func_80017410(D_80074F08, D_80070D60, 0);
    if (D_80073CA8 != 0) {
        func_80017410(D_80073CA8, D_80073C34, 0);
    }
}

/**
 * @brief Transfers sound data with a given mode for primary and optional secondary sources.
 *
 * Calls func_80017410 for the primary sound source with the mode from *a0.
 * If both D_80073CA8 and *a0 are non-zero, also calls for the secondary source.
 *
 * @param a0 Pointer to the transfer mode value.
 */
void sndTransferDataWithMode(s32 *a0) {
    func_80017410(D_80074F08, D_80070D60, *a0);
    if (D_80073CA8 != 0 && *a0 != 0) {
        func_80017410(D_80073CA8, D_80073C34, *a0);
    }
}

/**
 * @brief Clears voice bits for inactive CD-audio tracks.
 *
 * Iterates over 12 CD-audio tracks (D_80072F70, stride 0x110). For each
 * track whose voice bit is set in D_80075028 but does NOT have the
 * 0x2000000 flag in its keyOnMask, ORs the bit into D_80075028+0xC
 * (pending clear mask), calls sndTrackClearVoiceBits, and zeroes the
 * track's flags field at offset 0x30. Sets hardware update flag 0x110
 * in D_80077288+8.
 */
INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019BC0);

/**
 * @brief Starts sound playback mode 1 for primary and optional secondary sources.
 *
 * Sets D_8007728C to 1, calls func_80017D14 for primary source (D_80074F08, D_80070D60),
 * optionally for secondary source (D_80073CA8, D_80073C34), then calls func_80017D5C.
 */
void sndStartPlaybackMode1(void) {
    D_8007728C = 1;
    func_80017D14(D_80074F08, D_80070D60);
    if (D_80073CA8 != 0) {
        func_80017D14(D_80073CA8, D_80073C34);
    }
    func_80017D5C();
}

/**
 * @brief Starts sound playback mode 2 for primary and optional secondary sources.
 *
 * Sets D_8007728C to 2, calls func_80017D14 for primary source (D_80074F08, D_80070D60),
 * optionally for secondary source (D_80073CA8, D_80073C34), then calls func_80017D5C.
 */
void sndStartPlaybackMode2(void) {
    D_8007728C = 2;
    func_80017D14(D_80074F08, D_80070D60);
    if (D_80073CA8 != 0) {
        func_80017D14(D_80073CA8, D_80073C34);
    }
    func_80017D5C();
}

/**
 * @brief Stores *a0 to D_8007507C and sets bits 0+1 on all 32 track entries.
 *
 * Iterates over 32 entries in D_80070D60 (stride 0x110), ORing 0x3 into
 * the word at offset +0xF8 of each entry.
 *
 * @param a0 Pointer to word value to store in D_8007507C.
 */
void sndSetTempoAllTracks(s32 *a0) {
    s32 i = 0;
    s32 base = (s32)D_80070D60;
    s32 *ptr = (s32 *)(base + 0xF8);
    D_8007507C = *a0;
    do {
        *ptr |= 3;
        i++;
        ptr = (s32 *)((u8 *)ptr + 0x110);
    } while ((u32)i < 0x20);
}

/** @brief Copies halfword from a0 to offset 0x60 in struct pointed to by D_80074F08. */
void sndSetSequenceOffset(u16 *a0) {
    ((SoundSeqTrack *)D_80074F08)->voiceActive = *a0;
}

/**
 * @brief Silences unused SPU voices and transfers active voice mask.
 *
 * Identifies voices that are neither in use (D_80075028) nor masked
 * (D_800772A4). For each such voice, zeroes its volume and pitch, sets
 * maximum attack/sustain to produce silence, then clears the bit.
 * Copies instParams to field 0x1C and clears instParams.
 * Sets bit 0 of D_800772F4 to signal the update.
 */
void func_80019DB0(void) {
    s32 unusedMask;
    s32 bit;
    s32 voice;
    s32 *ptr;
    s32 tmp;

    if (*(s32 *)((s32)D_80074F08 + 4) != 0) {
        unusedMask = ~(D_80075028 | D_800772A4) & 0xFFFFFF;
        if (unusedMask != 0) {
            bit = 1;
            voice = 0;
            do {
                if (unusedMask & bit) {
                    spuSetVoiceVolume(voice, 0, 0, 0);
                    spuSetVoicePitch(voice, 0);
                    spuSetVoiceAttack(voice, 0x7F, 1);
                    spuSetVoiceSustainMode(voice, 0x7F, 3);
                    unusedMask &= ~bit;
                }
                bit <<= 1;
                voice++;
            } while (unusedMask != 0);
        }
        ptr = (s32 *)D_80074F08;
        tmp = ptr[1];
        ptr[1] = 0;
        ptr[7] = tmp;
    }
    D_800772F4 |= 1;
}

/**
 * @brief Applies pending key-on mask to track update flags.
 *
 * Reads the pending key-on mask from D_80074F08+0x1C. For each set bit,
 * ORs 0x2B13 into the corresponding track's updateFlags field.
 * Moves the mask from field 0x1C to field 0x04 and sets the hardware
 * update flag. Clears bit 0 of D_800772F4.
 */
INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019EA0);

/**
 * @brief Silences CD-audio voices and clears pending voice bits.
 *
 * Reads the voice activation bitmask from D_80075028. For each bit, checks
 * if the corresponding CD-audio track (D_80072F70, stride 0x110) has the
 * 0x2000000 flag set in its keyOnMask (offset 0x24). If so, clears that
 * bit from the mask. Then stores the remaining mask to D_80075028+0x10,
 * clears those bits from D_80075028, and silences each remaining voice
 * (starting from voice 12) via SPU register writes.
 * Sets bit 1 of D_800772F4.
 */
void func_80019F3C(void) {
    s32 voiceMask;
    s32 bit;
    s32 voice;
    s32 counter;
    s32 *trackPtr;
    s32 *ctrlPtr;

    if (D_80075028 != 0) {
        voiceMask = D_80075028;
        trackPtr = (s32 *)((s32)D_80072F70);
        bit = 0x1000;
        counter = 0;
        do {
            if (voiceMask & bit) {
                if (trackPtr[9] & 0x2000000) {
                    voiceMask &= ~bit;
                }
            }
            counter++;
            trackPtr = (s32 *)((u8 *)trackPtr + 0x110);
            bit <<= 1;
        } while ((u32)counter < 12);

        bit = 0x1000;
        voice = 12;
        ctrlPtr = (s32 *)((s32)&D_80075028);
        ctrlPtr[4] = voiceMask;
        D_80075028 &= ~voiceMask;
        if (voiceMask != 0) {
            do {
                if (voiceMask & bit) {
                    spuSetVoiceVolume(voice, 0, 0, 0);
                    spuSetVoicePitch(voice, 0);
                    spuSetVoiceAttack(voice, 0x7F, 1);
                    spuSetVoiceSustainMode(voice, 0x7F, 3);
                    voiceMask &= ~bit;
                }
                bit <<= 1;
                voice++;
            } while (voiceMask != 0);
        }
    }
    D_800772F4 |= 2;
}

INCLUDE_ASM("asm/nonmatchings/snd_param", func_8001A058);

/**
 * @brief Mutes two consecutive SPU voices if the stream is active.
 *
 * Checks the SoundStream active flag. If non-zero, sets sample rate
 * to 0 for the stream's voice and the next consecutive voice.
 */
void sndMuteVoicePair(void) {
    SoundStream *stream = (SoundStream *)D_80077298;
    if (stream->active != 0) {
        spuSetVoicePitch(stream->voiceIdx, 0);
        spuSetVoicePitch(stream->voiceIdx + 1, 0);
    }
}

/**
 * @brief Restores pitch for two consecutive SPU voices from saved state.
 *
 * Checks the SoundStream active flag. If non-zero, restores the saved
 * pitch for the stream's voice and the next consecutive voice.
 */
void sndRestoreVoicePair(void) {
    SoundStream *stream = (SoundStream *)D_80077298;
    if (stream->active != 0) {
        spuSetVoicePitch(stream->voiceIdx, stream->savedPitch);
        spuSetVoicePitch(stream->voiceIdx + 1, stream->savedPitch);
    }
}

/** @brief Empty stub — no operation. */
void sndStub(void) {
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
void sndSetReverbMode(s32 a0) {
    s32 current;
    func_8003ED54(&current);
    if (current != a0) {
        SpuSetReverb(0);
        SpuSetReverbModeType(a0 | 0x100);
        SpuSetReverb(1);
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_param", func_8001A1E8);

