#include "common.h"

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011438);

// empty stub
void func_8001152C(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", start);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800115F0);

void InitGeom(void);
void ResetCallback(void);
void ResetGraph(u8 a);
void SetDispMask(u8 a);
void SetGraphDebug(u8 a);
void SetMem(u8 a);
void StopCallback(void);
void VSyncCallback(void (*cb)(void));
void func_8003DE24(void);

extern void D_8001167C(void);
extern s8 D_8005F10C;
extern s16 D_8005F146;

void func_800117FC(void) {
    SetMem(2);
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    func_8003DE24();
    D_8005F10C = 0;
    D_8005F146 = 0;
    VSyncCallback(D_8001167C);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

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

void func_80011BA8(void) {
    extern u8 D_80067468[];
    extern u32 D_800974D8[];

    func_80012CC8();

    func_8003882C(D_800974D8[0], D_800974D8[1], (s32)D_80067468, 0);
    while (func_800393C8() != 0)
        ;

    func_80012D08((s32)D_80067468);

    func_8003882C(D_800974D8[2], D_800974D8[3], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80013A5C(0x801B0000, 1);

    func_8003882C(D_800974D8[4], D_800974D8[5], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80013A5C(0x801B0000, 1);
}

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

void func_80011D3C(void) {
    extern u32 D_80097808[];
    extern u8 D_8006A468[];
    extern u8 D_8005F188[];

    func_80011C68();
    func_8002AB5C((s32)D_8006A468, 0x6000);
    func_80011CB0();

    func_8003882C(D_80097808[2], D_80097808[3], 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    func_80028564(0x801B0000);

    func_8003882C(D_80097808[6], D_80097808[7], 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_80039678((s32)D_8005F188, 0x80090000, 0x200);

    func_8003882C(D_80097808[4], D_80097808[5], 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_8002C100(0x80090000);
}

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
