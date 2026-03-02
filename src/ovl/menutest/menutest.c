#include "common.h"

extern s16 D_801E7ABC;
extern u8 D_801E7ABE;
extern s32 D_801E7ACC;
extern u8 D_801E7ADC;
extern u8 D_801E69BC;
extern u8 D_801E71BC;
extern u8 D_801E79BC;
extern u16 D_801FA3C8;
extern u8 D_801FAB00;
extern u8 D_801FABD4;
extern s32 D_80083848;
extern u8 D_800780AB;
extern u8 D_80077378;
extern u32 D_801E69B8;

void func_801E5D74(s32 a0);

/**
 * Allocates a menu resource of type 0xD.
 * @param a0 Subtype parameter passed as third argument
 * @return Result from func_801F08D4
 */
s32 func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E582C);

/**
 * Decodes a string using func_801F7A54 key and func_8002F294/func_8002F2EC,
 * then copies it to D_801E7ADC via func_8002A2A8.
 */
void func_801E58B8(void) {
    u8 buf[0x20];
    u8 *p = buf;
    s32 v0;

    v0 = func_801F7A54();
    if (v0 == 0x1F) {
        v0 = func_801F6AFC(0x39);
        func_8002A2A8((s32)&D_801E7ADC, v0);
        return;
    }
    v0 = func_801F7A54();
    func_8002F294(v0, (s32)p, 1);
    func_8002F2EC((s32)p, 4, 1, 0x10);
    if (*p == 0x10) {
        s32 c = 0x10;
        do {
            p++;
        } while (*p == c);
    }
    {
        u8 *q = p;
        if (*p != 0) {
            do {
                v0 = func_80020F84(0xB);
                v0 = *(u8 *)(v0 + 1) + 0xFF;
                *q += v0;
                q++;
            } while (*q != 0);
        }
    }
    func_8002A2A8((s32)&D_801E7ADC, (s32)p);
}

INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E59B4);

/**
 * Reads a string descriptor from a table and dispatches to func_801E59B4.
 * @param a0 Base pointer to string table
 * @param a1 Index into the table
 */
void func_801E5D18(s32 a0, s32 a1) {
    s32 offset;
    u8 code;

    offset = *(u16 *)(a0 + a1 * 2 + 2);
    code = *(u8 *)(a0 + offset);
    *(u8 *)&D_801E7ABE = code;
    *(s16 *)&D_801E7ABC = func_801E59B4(a0 + offset + 1, &D_801E69BC, &D_801E79BC);
}

INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E5D74);

INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E64B4);

INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E6570);

INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E66A8);

/**
 * Sets up menu display rendering pipeline.
 * @param a0 Menu state structure
 * @param a1 Display buffer 1
 * @param a2 Display buffer 2
 * @return Final display buffer pointer from func_801F1B10
 */
s32 func_801E6760(s32 a0, s32 a1, s32 a2) {
    s32 v0;

    func_801F1AFC();
    func_8002E7C4(*(s16 *)(a0 + 0x24));
    func_80030058(*(s16 *)(a0 + 0x24));
    v0 = func_801E64B4(a1, a2);
    v0 = func_801E6570(a0, a1, v0);
    v0 = func_801E66A8(a0, a1, v0);
    func_801F1B10();
    return v0;
}

/**
 * Main entry point for the menu test overlay.
 * Initializes menu state, loads resources, and runs the main menu loop.
 */
void func_801E67F0(void) {
    s32 s0;
    s32 v0;

    s0 = func_801F179C((s32)func_801E5D74, (s32)func_801E6760);
    func_801F0948(0);
    func_801F5440();
    do {
        v0 = func_80035E00();
    } while (v0 != 0);
    if (s0 == 0) {
        return;
    }
    *(s16 *)(s0 + 0x26) = 0;
    func_801F1D34((s32)&D_801E69B8);
    func_801F1DB0(0);
    v0 = func_801E28D4();
    if (v0 == 0xFF) {
        *(u8 *)(s0 + 0x2C) = 0;
        *(u8 *)(s0 + 0x2D) = *(u8 *)&D_800780AB;
        v0 = func_801E5800(0x11);
        func_801E59B4(v0, (s32)&D_801E71BC, (s32)&D_801E79BC);
        v0 = func_801E5800(0x1A);
    } else {
        *(u8 *)(s0 + 0x2C) = 1;
        v0 = func_801E28D4();
        *(u8 *)(s0 + 0x2D) = v0;
        *(s32 *)&D_801E7ACC = *(u8 *)(s0 + 0x2D);
        *(s32 *)((s32)&D_801E7ACC + 4) = *(u8 *)(s0 + 0x2D) + 1;
        v0 = func_801E5800(0x16);
        func_801E59B4(v0, (s32)&D_801E71BC, (s32)&D_801E79BC);
        v0 = func_801E5800(0x1B);
    }
    *(s32 *)(s0 + 0x20) = v0;
    *(u8 *)(s0 + 0x2E) = 0;
    *(s16 *)(s0 + 0x26) = 0;
    *(u8 *)&D_801E69BC = 0;
    if (*(u8 *)(s0 + 0x2D) < 0x1E) {
        func_800360D0(*(u8 *)(s0 + 0x2D) + 0x60, 0x801D1000);
    }
    *(s16 *)(s0 + 0x2A) = 0;
    func_801E5D74(s0);
}
