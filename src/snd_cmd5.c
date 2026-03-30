#include "common.h"

/**
 * @brief Store callback parameter and capture root counter 2 value.
 *
 * Reads the current value of root counter 2 (hardware timer at 0x1F801120),
 * stores the callback parameter @p a0 to D_8008A6A4, and saves the counter
 * value to D_8008A6A0.
 *
 * @param a0 Callback parameter to store.
 */
INCLUDE_ASM("asm/nonmatchings/snd_cmd5", func_8003C744);


INCLUDE_ASM("asm/nonmatchings/snd_cmd5", func_8003C764);
