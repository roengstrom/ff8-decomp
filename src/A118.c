#include "common.h"

extern s32 *D_80074F08;

INCLUDE_ASM("asm/nonmatchings/A118", func_80019918);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019934);

INCLUDE_ASM("asm/nonmatchings/A118", func_8001999C);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019A10);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019A2C);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019A94);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019B08);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019B58);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019BC0);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019C9C);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019CFC);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019D5C);

/** @brief Copies halfword from a0 to offset 0x60 in struct pointed to by D_80074F08. */
void func_80019D9C(u16 *a0) {
    *(u16 *)((u8 *)D_80074F08 + 0x60) = *a0;
}

INCLUDE_ASM("asm/nonmatchings/A118", func_80019DB0);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019EA0);

INCLUDE_ASM("asm/nonmatchings/A118", func_80019F3C);

INCLUDE_ASM("asm/nonmatchings/A118", func_8001A058);

INCLUDE_ASM("asm/nonmatchings/A118", func_8001A0EC);

INCLUDE_ASM("asm/nonmatchings/A118", func_8001A13C);

/** @brief Empty stub — no operation. */
void func_8001A190(void) {
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
void func_8001A198(s32 a0) {
    s32 current;
    func_8003ED54(&current);
    if (current != a0) {
        SpuSetReverb(0);
        SpuSetReverbModeType(a0 | 0x100);
        SpuSetReverb(1);
    }
}

INCLUDE_ASM("asm/nonmatchings/A118", func_8001A1E8);

