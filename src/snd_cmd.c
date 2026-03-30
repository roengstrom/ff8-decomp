#include "common.h"
#include "sound.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"

/**
 * @brief Send a "play note" command (0x43) to a sound voice.
 *
 * Sets the command byte to 0x43, points the data pointer to the
 * voice's own buffer at offset +0x24, writes the note value a1
 * there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Note value to play.
 */
void sndVoiceCmdPlayNote(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x43;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "stop" command (0x45) to a sound voice.
 *
 * Sets the command byte to 0x45, clears the data pointer to 0,
 * and sets the payload size to 0.
 *
 * @param a0 Pointer to the voice control structure.
 */
void sndVoiceCmdStop(u8 *a0) {
    *(u8 *)(a0 + 0x37) = 0x45;
    *(s32 *)(a0 + 0x2C) = 0;
    *(u8 *)(a0 + 0x36) = 0;
}


/**
 * @brief Send a "set program" command (0x4C) to a sound voice.
 *
 * Sets the command byte to 0x4C, points the data pointer to offset +0x24,
 * writes the program number a1 there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Program/instrument number.
 */
void sndVoiceCmdSetProgram(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x4C;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "set pitch" command (0x46) to a sound voice.
 *
 * Sets the command byte to 0x46, points the data pointer to offset +0x24,
 * writes the pitch value a1 there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Pitch value.
 */
void sndVoiceCmdSetPitch(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x46;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "set volume" command (0x47) to a sound voice.
 *
 * Sets the command byte to 0x47, points the data pointer to offset +0x24,
 * writes the volume value a1 there, and sets the payload size to 1.
 *
 * @param a0 Pointer to the voice control structure.
 * @param a1 Volume level.
 */
void sndVoiceCmdSetVolume(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x47;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}


/**
 * @brief Send a "stop" command (0x4B) to a sound voice.
 *
 * Sets the command byte to 0x4B, clears the data pointer and payload size.
 *
 * @param a0 Pointer to the voice control structure.
 */
void func_8003BC0C(SndVoice *voice) {
    voice->cmdType = 0x4B;
    voice->cmdDataPtr = 0;
    voice->cmdSize = 0;
}
/* FIXME: trailing nop is compilation unit padding; should be removed
   once splat supports proper segment address pinning in linker scripts. */
INCLUDE_ASM("src/asm", func_8003BC20);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BC24);


/**
 * @brief Reset a voice structure if its action flag (byte 0x49) is set.
 *
 * Clears various fields of the voice structure and fills the 6-byte
 * slot table (at offset 0x5D) with 0xFF. Does nothing if byte 0x49
 * is already zero.
 *
 * @param a0 Pointer to the voice structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BD84);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BDEC);


/** @brief Moves byte at offset 0x37 to offset 0x38 and clears 0x37. */
void sndVoiceCmdArchive(u8 *a0) {
    u8 tmp = a0[0x37];
    a0[0x37] = 0;
    a0[0x38] = tmp;
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


/**
 * @brief Store callback parameter and capture root counter 2 value.
 *
 * Reads the current value of root counter 2 (hardware timer at 0x1F801120),
 * stores the callback parameter @p a0 to D_8008A6A4, and saves the counter
 * value to D_8008A6A0.
 *
 * @param a0 Callback parameter to store.
 */
/**
 * @brief Store callback parameter and capture root counter 2 value.
 *
 * Reads the current value of root counter 2 (hardware timer at 0x1F801120),
 * stores the callback parameter @p a0 to D_8008A6A4, and saves the counter
 * value to D_8008A6A0.
 *
 * @param a0 Callback parameter to store.
 */
/**
 * @brief Store callback parameter and capture root counter 2 value.
 *
 * Reads the current value of root counter 2 (hardware timer at 0x1F801120),
 * stores the callback parameter @p a0 to D_8008A6A4, and saves the counter
 * value to D_8008A6A0.
 *
 * @param a0 Callback parameter to store.
 */
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C744);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C764);
