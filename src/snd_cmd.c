#include "common.h"
#include "sound.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"

/**
 * @brief Send a "play note" command (0x43) to a sound voice.
 *
 * Sets the command byte to 0x43, points the data pointer to the
 * voice's own inline buffer, writes the note value there, and sets
 * the payload size to 1.
 *
 * @param voice Pointer to the voice control structure.
 * @param note  Note value to play.
 */
void sndVoiceCmdPlayNote(SndVoice *voice, s32 note) {
    voice->cmdType = 0x43;
    voice->cmdDataPtr = (s32)voice->cmdData;
    voice->cmdData[0] = note;
    voice->cmdSize = 1;
}


/**
 * @brief Send a "stop" command (0x45) to a sound voice.
 *
 * Sets the command byte to 0x45, clears the data pointer to 0,
 * and sets the payload size to 0.
 *
 * @param voice Pointer to the voice control structure.
 */
void sndVoiceCmdStop(SndVoice *voice) {
    voice->cmdType = 0x45;
    voice->cmdDataPtr = 0;
    voice->cmdSize = 0;
}


/**
 * @brief Send a "set program" command (0x4C) to a sound voice.
 *
 * Sets the command byte to 0x4C, points the data pointer to the inline
 * buffer, writes the program number there, and sets the payload size to 1.
 *
 * @param voice   Pointer to the voice control structure.
 * @param program Program/instrument number.
 */
void sndVoiceCmdSetProgram(SndVoice *voice, s32 program) {
    voice->cmdType = 0x4C;
    voice->cmdDataPtr = (s32)voice->cmdData;
    voice->cmdData[0] = program;
    voice->cmdSize = 1;
}


/**
 * @brief Send a "set pitch" command (0x46) to a sound voice.
 *
 * Sets the command byte to 0x46, points the data pointer to the inline
 * buffer, writes the pitch value there, and sets the payload size to 1.
 *
 * @param voice Pointer to the voice control structure.
 * @param pitch Pitch value.
 */
void sndVoiceCmdSetPitch(SndVoice *voice, s32 pitch) {
    voice->cmdType = 0x46;
    voice->cmdDataPtr = (s32)voice->cmdData;
    voice->cmdData[0] = pitch;
    voice->cmdSize = 1;
}


/**
 * @brief Send a "set volume" command (0x47) to a sound voice.
 *
 * Sets the command byte to 0x47, points the data pointer to the inline
 * buffer, writes the volume value there, and sets the payload size to 1.
 *
 * @param voice  Pointer to the voice control structure.
 * @param volume Volume level.
 */
void sndVoiceCmdSetVolume(SndVoice *voice, s32 volume) {
    voice->cmdType = 0x47;
    voice->cmdDataPtr = (s32)voice->cmdData;
    voice->cmdData[0] = volume;
    voice->cmdSize = 1;
}


/**
 * @brief Send a "key off" command (0x4B) to a sound voice.
 *
 * Sets the command byte to 0x4B, clears the data pointer, and sets
 * payload size to 0. Blocked by trailing nop at 0x8003BC20
 * (compilation unit padding).
 *
 * @param voice Pointer to the voice control structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BC0C);

INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BC24);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BD84);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BDEC);
/**
 * @brief Archive the current command type and clear it.
 *
 * Copies the current command type to cmdTypePrev and zeroes cmdType.
 *
 * @param voice Pointer to the voice control structure.
 */
void sndVoiceCmdArchive(SndVoice *voice) {
    u8 tmp = voice->cmdType;
    voice->cmdType = 0;
    voice->cmdTypePrev = tmp;
}
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BEF0);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BFAC);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C228);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C260);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C284);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C2B8);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C3C8);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C62C);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C70C);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C744);
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C764);
