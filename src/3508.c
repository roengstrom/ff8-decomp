#include "common.h"

s32 func_80012D08(u8 *a0) {
    extern u8 *D_80074ED0;
    extern u8 *D_80074ED8;
    extern u8 *D_80074EDC;
    s32 result = func_80014740(a0);
    if (result == 0) {
        a0 += 0x10;
        D_80074ED0 = (u8 *)a0;
        a0 += 0x400;
        D_80074ED8 = (u8 *)a0;
        a0 += 0x200;
        D_80074EDC = (u8 *)a0;
    }
    return result;
}

s32 func_80012D5C(void) {
    extern s32 D_8005169C;
    extern s32 D_80074ED4;
    extern u8 D_800516B8[];
    s32 i;

    DisableEvent(D_8005169C);
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);
    if (D_80074ED4 == 1) {
        func_800147A8((s32)D_800516B8, 0x40);
        func_80014834();
    }
    for (i = 0; i < 24; i++) {
        func_80014FCC(i, 5, 3);
    }
    func_80014DE0(0xFFFFFF);
    func_80014974();
    EnableEvent(D_8005169C);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012E04);

extern s32 D_80075058;
extern s32 *D_80074F08;
extern s32 D_80075028[];
extern s32 D_80077288[];
extern s32 D_80074EB8[];
extern s32 D_80077298[];

void func_80012EAC(s32 a0) {
    D_80075058 = a0;
    func_8001A1E8(0x10);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012ED4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012F30);

void func_8001302C(s32 a0, s32 a1, s32 a2, s32 a3) {
    D_80075058 = a0 & 0x3FF;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0x20);
}

s32 func_8001307C(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = func_80014740(a0);
    if (result != 0) {
        return result;
    }
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    *(&D_80075058 + 2) = a2 & 0xFF;
    *(&D_80075058 + 3) = a3 & 0x7F;
    func_8001A1E8(0x24);
    return a0;
}

void func_80013100(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0xFFFFFF;
    func_8001A1E8(0x21);
}

void func_8001313C(s32 a0) {
    D_80075058 = a0 & 0x3FF;
    func_8001A1E8(0x30);
}

// calls func_8001A1E8 with 0x44
// TODO: maspsx schedules addiu $sp into load delay slot instead of jr delay slot
void func_80013168(void) {
    func_8001A1E8(0x44);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013188);

void func_80013300(u32 a0) {
    s32 val;
    switch (a0) {
        case 1: val = 0x9B; break;
        case 2: val = 0x9D; break;
        case 3: val = 0x9F; break;
        default: val = 0x99; break;
    }
    func_8001A1E8(val);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001336C);

void func_8001344C(s32 a0) {
    D_80075058 = a0 & 0x7F;
    func_8001A1E8(0xA8);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013478);

void func_80013710(s32 a0, s32 a1) {
    D_80075058 = a0;
    *(&D_80075058 + 1) = a1 & 0x7F;
    func_8001A1E8(0xC0);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013744);

void func_80013A3C(void) {
    func_8001A1E8(0xF1);
}

void func_80013A5C(s32 a0, s32 a1) {
    do {
    } while (func_80014860(a0, a1) == 1);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013AA8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013CA4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013EE4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014398);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800146F0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014740);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014778);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800147A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014834);

s32 func_80014860(s32 a0, s32 a1) {
    if (func_80014740(a0) != 0) {
        return -1;
    }
    func_800148B0(a0, a1, *(s32 *)(a0 + 0x18), *(s32 *)(a0 + 0x10));
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800148B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014974);

void func_80014C30(void) {
    extern s32 D_80074E88;
    extern u8 D_800516B8[];
    extern s32 D_8001AD60;
    extern s32 D_8005169C;

    SpuStart();
    SpuInitMalloc(4, (s32)&D_80074E88);
    SpuSetTransferMode(0);
    SpuSetTransferStartAddr(0x1010);
    func_800147A8((s32)D_800516B8, 0x40);
    func_80014834();
    func_80014974();
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);

    do {
    } while (SetRCnt(0xF2000002, 0x44E8, 0x1000) == 0);

    do {
    } while (StartRCnt(0xF2000002) == 0);

    do {
        D_8005169C = OpenEvent(0xF2000002, 2, 0x1000, (s32)&D_8001AD60);
    } while (D_8005169C == -1);

    do {
    } while (EnableEvent(D_8005169C) == 0);
}

void func_80014D20(void) {
    extern s32 D_80074ED4;
    extern u8 D_800516B8[];
    extern s32 D_8005169C;

    if (D_80074ED4 == 1) {
        func_800147A8((s32)D_800516B8, 0x40);
        func_80014834();
    }

    do {
    } while (StopRCnt(0xF2000002) == 0);

    UnDeliverEvent(0xF2000002, 2);

    do {
    } while (DisableEvent(D_8005169C) == 0);

    do {
    } while (CloseEvent(D_8005169C) == 0);

    func_80014DE0(0xFFFFFF);
    func_8003DE44();
}

// spu_set_reverb_work_area_start - writes SPU register 0x1F801D88 (source: nocash PSX specs)
void func_80014DC4(u32 val) {
    *(s16 *)0x1F801D88 = val;
    *(s16 *)0x1F801D8A = val >> 16;
}

// spu_set_irq_addr - writes SPU register 0x1F801D8C (source: nocash PSX specs)
void func_80014DE0(u32 val) {
    *(s16 *)0x1F801D8C = val;
    *(s16 *)0x1F801D8E = val >> 16;
}

// spu_set_transfer_control - writes SPU register 0x1F801D98 (source: nocash PSX specs)
void func_80014DFC(u32 val) {
    *(s16 *)0x1F801D98 = val;
    *(s16 *)0x1F801D9A = val >> 16;
}

// spu_set_data_fifo - writes SPU register 0x1F801D94 (source: nocash PSX specs)
void func_80014E18(u32 val) {
    *(s16 *)0x1F801D94 = val;
    *(s16 *)0x1F801D96 = val >> 16;
}

// spu_set_transfer_addr - writes SPU register 0x1F801D90 (source: nocash PSX specs)
void func_80014E34(u32 val) {
    *(s16 *)0x1F801D90 = val;
    *(s16 *)0x1F801D92 = val >> 16;
}

void func_80014E50(s32 voice, s32 vol_l, s32 vol_r, s32 scale) {
    s16 *p;
    if (scale != 0) {
        vol_l *= scale;
        vol_l = (u32)vol_l >> 7;
        vol_r *= scale;
        vol_r = (u32)vol_r >> 7;
    }
    p = (s16 *)(0x1F801C00 + voice * 16);
    p[0] = vol_l & 0x7FFF;
    p[1] = vol_r & 0x7FFF;
}

// spu_set_voice_pitch - writes voice register +0x04 (source: nocash PSX specs)
void func_80014E98(s32 voice, s32 val) {
    *(s16 *)(0x1F801C04 + voice * 16) = val;
}

// spu_set_voice_start_addr - writes voice register +0x06, addr>>3 (source: nocash PSX specs)
void func_80014EB0(s32 voice, u32 val) {
    *(s16 *)(0x1F801C06 + voice * 16) = val >> 3;
}

// spu_set_voice_repeat_addr - writes voice register +0x0E, addr>>3 (source: nocash PSX specs)
void func_80014ECC(s32 voice, u32 val) {
    *(s16 *)(0x1F801C0E + voice * 16) = val >> 3;
}

// spu_set_voice_adsr_low - writes voice register +0x08 (source: nocash PSX specs)
void func_80014EE8(s32 voice, s32 val) {
    *(s16 *)(0x1F801C08 + voice * 16) = val;
}

// spu_set_voice_adsr_high - writes voice register +0x0A (source: nocash PSX specs)
void func_80014F00(s32 voice, s32 val) {
    *(s16 *)(0x1F801C0A + voice * 16) = val;
}

void func_80014F18(s32 voice, u32 a1, u32 a2) {
    u8 *addr = (u8 *)(0x1F801C08 + voice * 16);
    s32 val = *addr;
    val |= ((a2 >> 2) << 15) | (a1 << 8);
    *(s16 *)addr = val;
}

// spu_set_voice_adsr_decay_rate - modifies ADSR low bits [7:4] (source: nocash PSX specs)
void func_80014F48(s32 voice, u32 a1) {
    s16 *addr = (s16 *)(0x1F801C08 + voice * 16);
    s32 val = *(u16 *)addr;
    val = (val & 0xFF0F) | (a1 << 4);
    *addr = val;
}

// spu_set_voice_adsr_sustain_mode - modifies ADSR low bits [3:0] (source: nocash PSX specs)
void func_80014F70(s32 voice, u32 a1) {
    s16 *addr = (s16 *)(0x1F801C08 + voice * 16);
    s32 val = *(u16 *)addr;
    val = (val & 0xFFF0) | a1;
    *addr = val;
}

void func_80014F98(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 1) << 14) | (a1 << 6);
    s32 val = *(u16 *)addr;
    val = (val & 0x3F) | packed;
    *addr = val;
}

void func_80014FCC(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 2) << 5) | a1;
    s32 val = *(u16 *)addr;
    val = (val & 0xFFC0) | packed;
    *addr = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014FFC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800150A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800151C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80015668);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80015AC8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80015F4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80015FE0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016280);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016300);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016344);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016478);

extern s32 D_80074F1C;
// store D_80074F1C into struct field at offset 0x50
void func_800164C8(u8 *a0) {
    *(s32 *)(a0 + 0x50) = D_80074F1C;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800164D8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016A1C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016A7C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016C28);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016DB4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016E08);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016E5C);

s32 func_80016F3C(s32 a0) {
    s32 ret = 0;
    if (a0 == 0) return ret;
    if (a0 == D_80074EB8[1] || a0 == D_80074EB8[4]) {
        ret = 1;
    } else if (a0 == D_80074EB8[2] || a0 == D_80074EB8[5]) {
        ret = 2;
    }
    return ret;
}

void func_80016FA8(void *a0, s32 a1) {
    *(s16*)((u8*)a0 + 0x80) = 0x6E00;
    *(s32*)((u8*)a0 + 0x44) = 0x32000000;
    *(s32*)a0 = a1;
    *(s16*)((u8*)a0 + 0xE4) = 0;
    *(s16*)((u8*)a0 + 0xE6) = 0;
    *(s16*)((u8*)a0 + 0x98) = 0;
    *(s32*)((u8*)a0 + 0x2C) = 0;
    *(s16*)((u8*)a0 + 0xEA) = 0;
    *(s16*)((u8*)a0 + 0x90) = 0;
    *(s16*)((u8*)a0 + 0xD8) = 0;
    *(s16*)((u8*)a0 + 0xD6) = 0;
    *(s16*)((u8*)a0 + 0x86) = 0;
    *(s32*)((u8*)a0 + 0x50) = 0;
    *(s16*)((u8*)a0 + 0xD4) = 0;
    *(s32*)((u8*)a0 + 0x30) = 0;
    *(s16*)((u8*)a0 + 0xF2) = 0;
    *(s16*)((u8*)a0 + 0x9A) = 0;
    *(u16*)((u8*)a0 + 0x6E) = 0xFFFF;
    *(s16*)((u8*)a0 + 0x10A) = 0;
    *(s16*)((u8*)a0 + 0xCA) = 0;
    *(s16*)((u8*)a0 + 0xBC) = 0;
    *(s16*)((u8*)a0 + 0xAA) = 0;
    *(s16*)((u8*)a0 + 0xCC) = 0;
    *(s16*)((u8*)a0 + 0xBE) = 0;
    *(s16*)((u8*)a0 + 0xAC) = 0;
    *(s16*)((u8*)a0 + 0xD2) = 0;
    *(s16*)((u8*)a0 + 0xD0) = 0;
    func_8001C1DC(a0, 0);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017040);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001708C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017410);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800174E4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017880);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017A2C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017AAC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017C9C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017D14);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017D5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80017DB0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80018158);

void func_8001A198(s32 a0) {
    s32 current;
    func_8003ED54(&current);
    if (current != a0) {
        SpuSetReverb(0);
        SpuSetReverbModeType(a0 | 0x100);
        SpuSetReverb(1);
    }
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001A1E8);

extern s16 D_8007507A;
// spu_set_cd_volume - writes D_8007507A to SPU CD audio volume L/R (0x1F801DB0/DB2) (source: nocash PSX specs)
void func_8001A55C(void) {
    s32 val = D_8007507A;
    *(s16 *)0x1F801DB0 = val;
    *(s16 *)0x1F801DB2 = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001A57C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001A5FC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001A674);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001AA28);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001ACCC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B1F4);

s32 func_8001B400(s32 a0, s32 a1) {
    if ((a0 & 0x400) && (u32)a1 >= 0x40 && (u32)a1 < 0x80) {
        return a1 + 0x20;
    }
    return a1;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B42C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B690);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B820);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001BAA8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C1A8);

extern u8 D_80073E68[];

void func_8001C1DC(u8 *a0, s32 a1) {
    *(s16 *)(a0 + 0x66) = a1;
    func_8001C1A8((s32)a0, (s32)(D_80073E68 + a1 * 16), *(s32 *)(D_80073E68 + a1 * 16));
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C214);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C280);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C2C8);

void func_8001D280(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] | a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
}

void func_8001D2E0(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x3C / 4] = p[0x3C / 4] & ~a1;
    } else {
        D_80075028[0x1C / 4] = D_80075028[0x1C / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x110;
    *(u16 *)((s32)a0 + 0xD0) = 0;
}

void func_8001D348(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] | a1;
    } else if (*(s32 *)((s32)a0 + 0x30) & 0x10000) {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

void func_8001D3BC(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x44 / 4] = p[0x44 / 4] & ~a1;
    } else {
        D_80075028[0x24 / 4] = D_80075028[0x24 / 4] & ~a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
    *(u16 *)((s32)a0 + 0xD2) = 0;
}

void func_8001D424(s32 *a0, s32 a1) {
    if (*(u16 *)((s32)a0 + 0x60) == 0) {
        s32 *p = D_80074F08;
        p[0x40 / 4] = p[0x40 / 4] | a1;
    } else {
        D_80075028[0x20 / 4] = D_80075028[0x20 / 4] | a1;
    }
    D_80077288[0x8 / 4] = D_80077288[0x8 / 4] | 0x100;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D484);

void func_8001D5E8(s32 a0, s32 a1) {
    s32 byte = *(u8 *)(*(s32 *)a0);
    *(s32 *)a0 = *(s32 *)a0 + 1;
    *(u16 *)(a0 + 0x106) = (*(u16 *)(a0 + 0x106) & 0xFF0F) | (byte << 4);
    *(s32 *)(a0 + 0xF8) = *(s32 *)(a0 + 0xF8) | 0x1000;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D61C);

void func_8001DF70(void) {
    s32 *s0 = D_80077298;
    if (s0[3] != 0) {
        SpuSetIRQ(0);
        SpuSetIRQCallback(0);
        func_80014DE0(s0[3]);
        D_80075028[8] &= ~s0[3];
        D_80077288[2] |= 0x100;
        s0[3] = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001DFF4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001E0CC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001E308);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001E4C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001E65C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001EC0C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001EDD4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001F0C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001F118);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001F2A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001F3D4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001F5C8);

