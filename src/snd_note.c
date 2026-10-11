#include "common.h"
#include "sound.h"

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

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001A57C);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001A5FC);

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

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B690);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001B820);

INCLUDE_ASM("asm/nonmatchings/snd_note", func_8001BAA8);
