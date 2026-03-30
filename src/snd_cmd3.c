#include "common.h"

/** @brief Moves byte at offset 0x37 to offset 0x38 and clears 0x37. */
void sndVoiceCmdArchive(u8 *a0) {
    u8 tmp = a0[0x37];
    a0[0x37] = 0;
    a0[0x38] = tmp;
}


INCLUDE_ASM("asm/nonmatchings/snd_cmd3", func_8003BEF0);


INCLUDE_ASM("asm/nonmatchings/snd_cmd3", func_8003BFAC);


INCLUDE_ASM("asm/nonmatchings/snd_cmd3", func_8003C228);


INCLUDE_ASM("asm/nonmatchings/snd_cmd3", func_8003C260);
