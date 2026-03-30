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
