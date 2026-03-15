#include "common.h"

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_80098000);

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_8009818C);

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_80098338);

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_80098378);

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_80098440);

/**
 * @brief Load and play a specific music track for the display init overlay.
 *
 * Reads CD file entry at offset 0x110 (entry 34) from D_800990B8 and
 * calls func_80038994 to load it to address 0x8017D000.
 */
void func_800985B4(void) {
    extern u32 D_800990B8[];

    u32 *base = D_800990B8;

    func_80038994(*(u32 *)((u8 *)base + 0x110),
                  *(u32 *)((u8 *)base + 0x114),
                  (s32)0x8017D000, 0);
}

/**
 * @brief Transfer display rows from VRAM buffer at 0x8017D008.
 *
 * Computes a VRAM source offset using D_8009928C and copies scanlines
 * starting at y = (u16)D_8009928C + 0x26, incrementing by 2 each iteration,
 * until y >= 0x1BC.
 */
void func_800985EC(void) {
    extern u32 D_8009928C;
    s16 rect[4];
    s32 val;
    s16 y;
    u8 *ptr;

    val = D_8009928C;
    rect[0] = 0x1E;
    rect[2] = 0x244;
    rect[3] = 1;
    y = (u16)D_8009928C + 0x26;
    rect[1] = y;
    ptr = (u8 *)((s32)0x8017D008 + val * 0x488);

    if ((s16)y < 0x1BC) {
        do {
            func_80048EFC(&rect, ptr);
            y = rect[1] + 2;
            rect[1] = y;
            ptr += 0x910;
        } while ((s16)y < 0x1BC);
    }
}

/**
 * @brief Perform a display fade-out and fade-in transition.
 *
 * Fades display brightness from 255 down to 0 (by 4 each step),
 * waits 300 frames, then fades back up from 0 to 255. Updates
 * D_80099290 state flag and D_8009928C/D_80099294 during the transition.
 */
void func_8009869C(void) {
    extern s32 D_80099290;
    extern s32 D_8009928C;
    extern s32 D_80099294;
    s16 rect[4];
    s32 brightness;
    s32 i;

    D_80099290 = 2;
    func_80042634(0);

    i = func_800499AC();
    brightness = 0xFF;
    D_8009928C = (u32)i < 1;

    rect[0] = 0x1E;
    rect[1] = 0x26;
    rect[2] = 0x244;
    rect[3] = 0x196;

    /* Fade out: brightness 255 -> 0 by 4 */
    do {
        func_80098440(brightness, 2, &rect);
        brightness -= 4;
        func_8009818C();
        D_80099294 = 1;
        func_80048BB8(1);
    } while (brightness >= 0);

    /* Wait 300 frames */
    i = 0;
    do {
        i++;
        func_8009818C();
    } while (i < 0x12C);

    /* Fade in: brightness 0 -> 255 by 4 */
    i = 0;
    do {
        func_80098440(i, 2, &rect);
        i += 4;
        func_8009818C();
    } while (i < 0xFF);

    func_80048BB8(0);
    D_80099290 = 1;
}

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_8009879C);

INCLUDE_ASM("asm/ovl/display_init/nonmatchings/display_init", func_80098974);

/**
 * @brief Display initialization entry point with mode selection.
 *
 * Configures display buffers, selects a display mode (0 or 1), runs
 * the corresponding intro sequence, then resets the display back.
 *
 * @param mode 0 for the standard intro sequence (func_80098974),
 *             1 for the alternate intro sequence (func_8009879C).
 */
void func_80098FD4(s32 mode) {
    extern s32 D_80099290;
    s16 rect[4];

    func_800488D4(1);

    rect[0] = 0x100;
    rect[1] = 0xE0;
    rect[2] = 0x40;
    rect[3] = 0x20;
    func_80048FBC(&rect, 0, 0x1E0);
    func_80048C50(0);
    func_80042634(0);

    switch (mode) {
    case 0:
        D_80099290 = 0;
        func_80098974();
        break;
    case 1:
        D_80099290 = mode;
        func_8009879C();
        break;
    }

    func_80048BB8(0);

    rect[1] = 0x1E0;
    rect[2] = 0x40;
    rect[0] = 0;
    rect[3] = 0x20;
    func_80048FBC(&rect, 0x100, 0xE0);
    func_80048C50(0);
    func_80042634(0);
}
