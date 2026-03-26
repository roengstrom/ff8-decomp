#include "common.h"

extern s32 *D_80074F08;

/**
 * @brief Clears D_80073E62, stores *a0 << 16 into D_80077284.
 * @param a0 Pointer to a byte value.
 */
void sndSetVolumeFade(s8 *a0) {
    extern u16 D_80073E62;
    extern s32 D_80077284;
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
    extern u16 D_80073E60;
    extern s32 D_80077280;
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
    extern u8 D_80070D60[];
    extern s32 D_80073CA8;
    extern u8 *D_80073C34;
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
    extern u8 D_80070D60[];
    extern s32 D_80073CA8;
    extern u8 *D_80073C34;
    func_80017410(D_80074F08, D_80070D60, *a0);
    if (D_80073CA8 != 0 && *a0 != 0) {
        func_80017410(D_80073CA8, D_80073C34, *a0);
    }
}

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019BC0);

/**
 * @brief Starts sound playback mode 1 for primary and optional secondary sources.
 *
 * Sets D_8007728C to 1, calls func_80017D14 for primary source (D_80074F08, D_80070D60),
 * optionally for secondary source (D_80073CA8, D_80073C34), then calls func_80017D5C.
 */
void sndStartPlaybackMode1(void) {
    extern u8 D_80070D60[];
    extern s32 D_80073CA8;
    extern u8 *D_80073C34;
    extern s32 D_8007728C;
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
    extern u8 D_80070D60[];
    extern s32 D_80073CA8;
    extern u8 *D_80073C34;
    extern s32 D_8007728C;
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
    extern u8 D_80070D60[];
    extern s32 D_8007507C;
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
    *(u16 *)((u8 *)D_80074F08 + 0x60) = *a0;
}

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019DB0);

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019EA0);

INCLUDE_ASM("asm/nonmatchings/snd_param", func_80019F3C);

INCLUDE_ASM("asm/nonmatchings/snd_param", func_8001A058);

/**
 * @brief Mutes two consecutive SPU voices if active flag is set.
 *
 * Checks the active flag at D_80077298+0xC. If non-zero, sets sample rate
 * to 0 for the voice at D_80077298+0x10 and the next consecutive voice.
 */
void sndMuteVoicePair(void) {
    extern u8 D_80077298[];
    u8 *base = D_80077298;
    if (*(s32 *)(base + 0xC) != 0) {
        spuSetVoicePitch(*(s32 *)(base + 0x10), 0);
        spuSetVoicePitch(*(s32 *)(base + 0x10) + 1, 0);
    }
}

/**
 * @brief Sets sample rate for two consecutive SPU voices from struct fields.
 *
 * Checks the active flag at D_80077298+0xC. If non-zero, sets sample rate
 * from D_80077298+0x58 for the voice at D_80077298+0x10 and the next voice.
 */
void sndRestoreVoicePair(void) {
    extern u8 D_80077298[];
    u8 *base = D_80077298;
    if (*(s32 *)(base + 0xC) != 0) {
        spuSetVoicePitch(*(s32 *)(base + 0x10), *(s32 *)(base + 0x58));
        spuSetVoicePitch(*(s32 *)(base + 0x10) + 1, *(s32 *)(base + 0x58));
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

