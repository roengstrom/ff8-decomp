#include "common.h"

INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BC24);


/**
 * @brief Reset a voice structure if its action flag (byte 0x49) is set.
 *
 * Clears various fields of the voice structure and fills the 6-byte
 * slot table (at offset 0x5D) with 0xFF. Does nothing if byte 0x49
 * is already zero.
 *
 * @param a0 Pointer to the voice structure.
 */
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BD84);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BDEC);


/** @brief Moves byte at offset 0x37 to offset 0x38 and clears 0x37. */
void sndVoiceCmdArchive(u8 *a0) {
    u8 tmp = a0[0x37];
    a0[0x37] = 0;
    a0[0x38] = tmp;
}


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BEF0);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BFAC);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C228);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C260);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C284);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C2B8);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C3C8);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C62C);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C70C);


/**
 * @brief Store callback parameter and capture root counter 2 value.
 *
 * Reads the current value of root counter 2 (hardware timer at 0x1F801120),
 * stores the callback parameter @p a0 to D_8008A6A4, and saves the counter
 * value to D_8008A6A0.
 *
 * @param a0 Callback parameter to store.
 */
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C744);


INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C764);
