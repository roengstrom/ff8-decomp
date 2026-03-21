#include "common.h"
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
void func_8003BB78(u8 *a0, s32 a1) {
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
void func_8003BB98(u8 *a0) {
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
void func_8003BBAC(u8 *a0, s32 a1) {
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
void func_8003BBCC(u8 *a0, s32 a1) {
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
void func_8003BBEC(u8 *a0, s32 a1) {
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
INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BC0C);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BC24);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BD84);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003BDEC);


/** @brief Moves byte at offset 0x37 to offset 0x38 and clears 0x37. */
void func_8003BEE0(u8 *a0) {
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


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C744);


INCLUDE_ASM("asm/nonmatchings/snd_cmd", func_8003C764);
