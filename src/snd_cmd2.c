#include "common.h"
#include "sound.h"
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BC24);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BD84);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BDEC);
/** @brief Moves byte at offset 0x37 to offset 0x38 and clears 0x37. */
void sndVoiceCmdArchive(u8 *a0) { u8 tmp = a0[0x37]; a0[0x37] = 0; a0[0x38] = tmp; }
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BEF0);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003BFAC);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C228);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C260);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C284);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C2B8);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C3C8);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C62C);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C70C);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C744);
INCLUDE_ASM("asm/nonmatchings/snd_cmd2", func_8003C764);
