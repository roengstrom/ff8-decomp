#include "common.h"

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011438);

// empty stub
void func_8001152C(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", start);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800115F0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800117FC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011870);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800119D4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011A60);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011AE0);

extern u32 D_80097410[];

void func_80011B60(void) {
    func_8003882C(D_80097410[2], D_80097410[3], 0x80097940, 0);
    while (func_800393C8() != 0)
        ;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011BA8);

extern u32 D_80097400[];

void func_80011C68(void) {
    func_8003882C(D_80097400[0], D_80097400[1], 0x80098000, 0);
    while (func_800393C8() != 0)
        ;
}

extern u32 D_80097808[];

void func_80011CB0(void) {
    func_8003882C(D_80097808[0], D_80097808[1], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;
    func_8002C3AC(0x801B0000, 0);
    while (DrawSync(1) != 0)
        ;
}

void func_80011D0C(void) {
    func_80011C68();
    func_80098390();
    func_80028444();
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011D3C);

extern u32 D_80051694[];

void func_80011E18(void) {
    func_8003882C(D_80051694[0], D_80051694[1], 0x80097400, 0);
    while (func_800393C8() != 0)
        ;
}

INCLUDE_ASM("asm/nonmatchings/1C38", main);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800127F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800129A4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012B4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012CC8);
