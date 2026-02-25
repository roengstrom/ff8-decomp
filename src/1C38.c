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

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011B60);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011BA8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011C68);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011CB0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011D0C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011D3C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80011E18);

INCLUDE_ASM("asm/nonmatchings/1C38", main);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800127F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800129A4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012B4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012CC8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012D08);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012D5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012E04);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012EAC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012ED4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80012F30);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001302C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001307C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013100);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001313C);

// calls func_8001A1E8 with 0x44
void func_80013168(void) {
    func_8001A1E8(0x44);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013188);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013300);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001336C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001344C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013478);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013710);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013744);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013A3C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013A5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013AA8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013CA4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80013EE4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014398);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800146F0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014740);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014778);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800147A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014834);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014860);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800148B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014974);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014C30);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014D20);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014E50);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014F18);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014F98);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80014FCC);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016F3C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80016FA8);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001A198);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B400);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B42C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B690);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001B820);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001BAA8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C1A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C1DC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C214);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C280);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001C2C8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D280);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D2E0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D348);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D3BC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D424);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D484);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D5E8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001D61C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8001DF70);

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

INCLUDE_ASM("asm/nonmatchings/1C38", func_800205D0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020608);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800209A0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020C08);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020CE0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020D4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020DB8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020E4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020F84);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80020FBC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021034);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021290);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021300);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021358);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800214E0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002153C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021628);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800216B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002172C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002178C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800217F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021894);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021944);

// multiply
s32 func_800219A8(s32 a0, s32 a1) {
    return a0 * a1;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800219B8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800219E0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021A64);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021B58);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80021C10);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022028);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800220E4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002216C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800221B4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022228);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022328);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022370);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022404);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002247C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002257C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002274C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800227F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800228F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002293C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800229FC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022B04);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022B48);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022BD0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022C04);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022C5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022CDC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022D78);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80022E08);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80023180);

// clamp_to_byte - clamps value to 0x00-0xFF range
s32 func_800231B0(s32 a0) {
    if (a0 >= 0x100) {
        return 0xFF;
    }
    return a0;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800231C8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800231E0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002363C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80023828);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80023888);

extern u8 D_80078658;

// get pointer to D_80078658
u8 *func_80023900(void) {
    return &D_80078658;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002390C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800239A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80023A54);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80023B14);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80023D60);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80024064);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800242C8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80025920);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800259C0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80025D80);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80025E4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80025F58);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800262B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800263DC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026ADC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026CA0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026D8C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026E20);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026E70);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026EC4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026F14);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026F4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026FB4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026FD4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80026FE0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027038);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800270B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027220);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027360);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800273D8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027408);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027448);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800275D4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002795C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800279CC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027A58);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027BA8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027CF8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027DB4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027EF8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80027F38);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800280C0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800281C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800282F4);

extern u16 D_80083794;
// get D_80083794 (u16)
u16 func_800283CC(void) {
    return D_80083794;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800283DC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800283F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028444);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800284BC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028564);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800286FC);

// strcpy
void func_8002871C(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028738);

// copy_bytes
void func_80028768(u8 *src, u8 *dst, s32 numBytes) {
    while (numBytes > 0) {
        *dst++ = *src++;
        numBytes--;
    }
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028790);

extern s8 D_80082FD4;
// set D_80082FD4 (s8)
void func_800287B0(s8 val) {
    D_80082FD4 = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800287BC);

// get_bit4 - extracts bit 4 as 0 or 1
s32 func_800287F4(s32 a0) {
    return (a0 >> 4) % 2;
}

// mod4
s32 func_80028810(s32 a0) {
    return a0 % 4;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002882C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028854);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028874);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800288F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028950);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028970);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800289F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028A44);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028A98);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028AF8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028B58);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028B78);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028B98);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028CB4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80028F3C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029028);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800290C0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029150);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029360);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029400);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002949C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800294E4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029550);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800295D0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800295F0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029660);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029778);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800297E4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029850);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029A20);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029C44);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029D38);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80029F5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A090);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A128);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A238);

// strcpy
void func_8002A2A8(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A2C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A2F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A3A8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A3C8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A3E8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A408);

extern u8 *D_8005F134;
// copy_display_coords - copies x,y from D_8005F134 offsets 8/A
void func_8002A438(u8 *a0) {
    u8 *p = D_8005F134;
    *(u16 *)(a0 + 0) = *(u16 *)(p + 8);
    *(u16 *)(a0 + 2) = *(u16 *)(p + 0xA);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A45C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A528);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A5E8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A888);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002A8B8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AAC0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AB5C);

// D_80083210 is an array of some datastructure that is 64 bytes in size
// Below are some functions that fetch entries given an index and some property from that datastructure.
extern u8 D_80083210[];
// battle_entity_get_ptr - returns &D_80083210[idx * 64]
u8 *func_8002AC74(s32 idx) {
    return D_80083210 + idx * 64;
}

// battle_entity_set_field_37_clamped - clamps val to [3, 11], D_80083210 stride 64
void func_8002AC88(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    s32 v;
    if (val >= 3) {
        if (val < 12) {
            v = val;
        } else {
            v = 11;
        }
    } else {
        v = 3;
    }
    p[0x37] = v;
}

// battle_entity_get_field_37 - D_80083210 stride 64
s32 func_8002ACBC(s32 idx) {
    u8 *p = D_80083210 + idx * 64;
    return p[0x37];
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002ACD8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AD04);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AD3C);

// battle_entity_get_field_38 - D_80083210 stride 64
s32 func_8002AE14(s32 idx) {
    u8 *p = D_80083210 + idx * 64;
    return p[0x38];
}

// battle_entity_set_field_38_and_30 - sets field 0x38, computes field 0x30 from bit 0
void func_8002AE30(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    s32 v;
    p[0x38] = val;
    v = 0x38;
    if (val & 1) {
        v = 0x3A;
    }
    *(s32 *)(p + 0x30) = v << 24;
}

// battle_entity_set_field_00 - D_80083210 stride 64
void func_8002AE60(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    *(s32 *)p = val;
}

// battle_entity_set_field_04 - D_80083210 stride 64
void func_8002AE78(s32 idx, s32 val) {
    u8 *p = D_80083210 + idx * 64;
    *(s32 *)(p + 4) = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AE90);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AEC4);

// battle_entity_set_field_34_and_clear - sets field 0x34, if val==0 also clears fields 0x36, 0x04, 0x00
void func_8002AE90(s32 idx, s32 a1);
void func_8002AE78(s32 idx, s32 a1);
void func_8002AE60(s32 idx, s32 a1);

void func_8002AEF8(s32 idx, s32 value) {

  u8 *p = (u8 *)D_80083210 + (idx << 6);

  p[0x34] = value;
  if (value == 0) {
      func_8002AE90(idx, 0);
      func_8002AE78(idx, 0);
      func_8002AE60(idx, 0);
  }
}

// battle_entity_get_field_34 - D_80083210 stride 64
s32 func_8002AF54(s32 idx) {
    u8 *p = D_80083210 + idx * 64;
    return p[0x34];
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AF70);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002AFA4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002B080);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002B16C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002B3A0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002B898);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002B8BC);

INCLUDE_ASM("asm/nonmatchings/1C38", __udivdi3);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002BC6C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002BE48);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002BEEC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002BF24);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C030);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C0C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C100);

extern u8 D_80083840;
// get D_80083840 (u8)
u8 func_8002C114(void) {
    return D_80083840;
}

// set D_80083840 (u8)
void func_8002C124(u8 val) {
    D_80083840 = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C130);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C3AC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C56C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C734);

extern u8 D_80082FF0[];
// sfx_entry_set_fields_29_2A_2C - D_80082FF0 stride 60
void func_8002C7E0(s32 idx, s32 a1, s32 a2, s32 a3) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x29] = a1;
    p[0x2A] = a2;
    p[0x2C] = a3;
}

// sfx_entry_set_field_2B - D_80082FF0 stride 60
void func_8002C808(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x2B] = val;
}

// sfx_entry_set_field_34 - D_80082FF0 stride 60
void func_8002C828(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s32 *)(p + 0x34) = val;
}

// sfx_entry_set_field_38 - D_80082FF0 stride 60
void func_8002C848(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s32 *)(p + 0x38) = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C868);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C8A4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C920);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C954);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002C9A4);

// sfx_entry_swap_field_16 - D_80082FF0 stride 60, returns old value
s32 func_8002CA10(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    s32 old = p[0x16];
    p[0x16] = val;
    return old;
}

// sfx_entry_get_field_16 - D_80082FF0 stride 60
s32 func_8002CA34(s32 idx) {
    u8 *p = D_80082FF0 + idx * 60;
    return p[0x16];
}

// sfx_entry_set_field_10_clear_14 - D_80082FF0 stride 60
void func_8002CA58(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s16 *)(p + 0x10) = val;
    *(s16 *)(p + 0x14) = 0;
}

// sfx_entry_set_field_1C - D_80082FF0 stride 60
void func_8002CA7C(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    *(s16 *)(p + 0x1C) = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002CA9C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002CAE0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002CC4C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002CDE4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002CECC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002CF54);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002D040);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002D6AC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002D8CC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DCA4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DD18);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DD78);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DDFC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DE38);

// sfx_entry_set_field_2F - D_80082FF0 stride 60
void func_8002DE74(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x2F] = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DE94);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002DF5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E064);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E1E8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E298);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E3A4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E428);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E4AC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E744);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E7A4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E7C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002E8DC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002EE10);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F23C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F294);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F2EC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F320);

extern u8 D_80052A20[];
// lookup_hex_char - D_80052A20 is "0123456789ABCDEF" (source: strings.csv 0x80052A20)
void func_8002F384(s32 idx, u8 *dst) {
    *dst = D_80052A20[idx & 0xF];
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F3A0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F3F0);

// u32_to_hex_string - converts u32 to 8-char hex string with configurable base char
void func_8002F488(u32 val, u8 *dst, s32 base_char) {
    s32 shift = 28;
    do {
        *dst++ = ((val >> shift) & 0xF) + base_char;
        shift -= 4;
    } while (shift >= 0);
    *dst = 0;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F4B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F548);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F5B4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F610);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002F688);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002FD9C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002FDE8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002FE0C);

// clear_rgb_fields - zeros bytes at offsets 0x20, 0x21, 0x22
void func_8002FF24(u8 *a0) {
    a0[0x20] = 0;
    a0[0x22] = 0;
    a0[0x21] = 0;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8002FF34);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030058);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800300F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030214);

// empty stub
void func_80030234(void) {
}

extern s16 D_800834D4;
// set D_800834D4 (s16)
void func_8003023C(s32 val) {
    D_800834D4 = val;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030248);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030288);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800302DC);

typedef struct {
    s16 f0;
    s8 f2;
    s8 f3;
    s16 f4;
    s8 f6;
    s8 f7;
} BattleCameraState;

extern BattleCameraState D_800834D0;

// init_battle_camera
void func_80030720(void) {
    D_800834D0.f3 = 0;
    D_800834D0.f4 = 0x1000;
    D_800834D0.f0 = 0;
    D_800834D0.f2 = 0;
    D_800834D0.f7 = 0;
    D_800834D0.f6 = 0;
}

extern u8 D_80083878;

// get pointer to D_80083878
u8 *func_80030748(void) {
    return &D_80083878;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030754);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030848);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030A54);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030B2C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030CB0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030CFC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030D48);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030D78);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030F10);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80030FDC);

// empty stub
void func_80031044(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003104C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031188);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031224);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031364);

typedef struct {
    s16 f0;
    s16 f2;
    s16 f4;
    s16 f6;
    u8 f8;
    u8 f9;
} Struct3754;
extern Struct3754 D_80083754;
// init_battle_transition - initializes D_80083754 (Struct3754)
void func_800318EC(void) {
    D_80083754.f0 = 9;
    D_80083754.f4 = 0;
    D_80083754.f6 = 0;
    D_80083754.f8 = 0;
    D_80083754.f9 = 0;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031910);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031954);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031A18);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031CDC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031DF4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031E1C);

extern u8 D_80083772[];
// clear_animation_entries
void func_80031F2C(void) {
    s32 i;
    u8 *ptr = D_80083772;
    for (i = 1; i >= 0; i--) {
        ptr[0xE] = 0;
        ptr += 0x10;
    }
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031F50);

// get_battle_alloc_base - returns 0x801F4000
u32 func_80031F9C(void) {
    return 0x801F4000;
}

// get_battle_alloc_size - returns 0x4000 (16KB)
s32 func_80031FA8(void) {
    return 0x4000;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80031FB0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80032010);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800320BC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80032204);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003228C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80032350);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80032534);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80032688);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800327B8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80032808);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003283C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800330F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033298);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003334C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033380);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003346C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033534);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800335AC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033688);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033768);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800337FC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033A28);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033C7C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033D5C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80033F1C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003406C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800341BC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003431C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80034830);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800348C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800349F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80034C74);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80034DBC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035118);

extern u8 D_80083928;
// get D_80083928 (u8)
u8 func_80035148(void) {
    return D_80083928;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035158);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035294);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800352EC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035360);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035AA4);

extern u8 D_80085138;
// set D_80085138 (u8)
void func_80035C54(u8 val) {
    D_80085138 = val;
}

// get D_80085138 (u8)
u8 func_80035C60(void) {
    return D_80085138;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035C70);

INCLUDE_ASM("asm/nonmatchings/1C38", SsUtReverbOff);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035CF0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035D10);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035D30);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035E00);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035E68);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80035FF4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800360D0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036104);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800361F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036254);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003631C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036444);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003646C);

// init_card_hand_slots - initializes 128 card slots (id=0, type=0xFF)
void func_800366E8(u8 *ptr) {
    s32 i = 0;
    do {
        i++;
        ptr[1] = 0xFF;
        ptr[0] = 0;
        ptr += 2;
    } while (i < 128);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036710);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003678C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003685C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036978);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800369CC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036C74);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036E8C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036EC0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036F60);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036FA4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80036FE0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800370AC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800372D0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800375A0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800376A8);

// mc_xor_checksum
u32 func_8003786C(u8 *a0) {
    u32 acc = 0;
    s32 i = 0;
    do {
        acc ^= *a0++;
        i++;
    } while ((u32)i < 0x7F);
    return acc & 0xFF;
}


// memzero_128 - zeros 128 bytes, matches MC header size (source: hyne)
void func_80037894(u8 *ptr) {
    s32 count = 128;
    do {
        *ptr++ = 0;
    } while (--count > 0);
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_800378B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800378F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003793C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003798C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80037ACC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80037BB0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80037E14);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80037FB0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038030);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800381BC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038308);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038490);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800385B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038668);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800386F0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038760);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800387F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003882C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038868);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038920);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038994);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800389CC);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038A60);

extern s8 D_8008A3DA;
// get D_8008A3DA (s8)
s8 func_80038CE0(void) {
    return D_8008A3DA;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038CF0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038D3C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038E28);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038ED4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80038F88);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039094);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039140);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039218);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039388);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800393C8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039444);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003947C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800394D0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039678);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039728);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039764);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003987C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039AA0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039AB4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039B80);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039BC4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039EF8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80039FE8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003A1F8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003A470);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003A500);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003AB64);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003AB84);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003ABA4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003AC10);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003ADD4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003AF50);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003AF88);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003AFD0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003B024);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003B040);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003B0C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003B334);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003B36C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003B8E0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003BA2C);

// snd_voice_cmd_play_note - cmd=0x43, sets payload byte
void func_8003BB78(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x43;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}

// snd_voice_cmd_stop - cmd=0x45, clears data pointer and flag
void func_8003BB98(u8 *a0) {
    *(u8 *)(a0 + 0x37) = 0x45;
    *(s32 *)(a0 + 0x2C) = 0;
    *(u8 *)(a0 + 0x36) = 0;
}

// snd_voice_cmd_set_program - cmd=0x4C, sets payload byte
void func_8003BBAC(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x4C;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}

// snd_voice_cmd_set_pitch - cmd=0x46, sets payload byte
void func_8003BBCC(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x46;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}

// snd_voice_cmd_set_volume - cmd=0x47, sets payload byte
void func_8003BBEC(u8 *a0, s32 a1) {
    *(u8 *)(a0 + 0x37) = 0x47;
    *(s32 *)(a0 + 0x2C) = (s32)(a0 + 0x24);
    *(u8 *)(a0 + 0x24) = a1;
    *(u8 *)(a0 + 0x36) = 1;
}

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003BC0C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003BC24);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003BEE0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003C284);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003C62C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003C744);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003C764);

INCLUDE_ASM("asm/nonmatchings/1C38", __addsf3);

INCLUDE_ASM("asm/nonmatchings/1C38", ADDSF3_OBJ_C4);

INCLUDE_ASM("asm/nonmatchings/1C38", ADDSF3_OBJ_190);

INCLUDE_ASM("asm/nonmatchings/1C38", ADDSF3_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/1C38", __divsf3);

INCLUDE_ASM("asm/nonmatchings/1C38", DIVSF3_OBJ_F0);

INCLUDE_ASM("asm/nonmatchings/1C38", DIVSF3_OBJ_130);

INCLUDE_ASM("asm/nonmatchings/1C38", DIVSF3_OBJ_134);

INCLUDE_ASM("asm/nonmatchings/1C38", __fixsfsi);

INCLUDE_ASM("asm/nonmatchings/1C38", FIXSFSI_OBJ_98);

INCLUDE_ASM("asm/nonmatchings/1C38", FIXSFSI_OBJ_A4);

INCLUDE_ASM("asm/nonmatchings/1C38", __floatsidf);

INCLUDE_ASM("asm/nonmatchings/1C38", FLTSIDF_OBJ_DC);

INCLUDE_ASM("asm/nonmatchings/1C38", _dbl_shift_us);

INCLUDE_ASM("asm/nonmatchings/1C38", DBSHIFTU_OBJ_20);

INCLUDE_ASM("asm/nonmatchings/1C38", __floatsisf);

INCLUDE_ASM("asm/nonmatchings/1C38", FLTSISF_OBJ_94);

INCLUDE_ASM("asm/nonmatchings/1C38", FLTSISF_OBJ_AC);

INCLUDE_ASM("asm/nonmatchings/1C38", __ltdf2);

INCLUDE_ASM("asm/nonmatchings/1C38", LTDF2_OBJ_D4);

INCLUDE_ASM("asm/nonmatchings/1C38", __mulsf3);

INCLUDE_ASM("asm/nonmatchings/1C38", MULSF3_OBJ_EC);

INCLUDE_ASM("asm/nonmatchings/1C38", MULSF3_OBJ_12C);

INCLUDE_ASM("asm/nonmatchings/1C38", MULSF3_OBJ_130);

INCLUDE_ASM("asm/nonmatchings/1C38", __subsf3);

INCLUDE_ASM("asm/nonmatchings/1C38", SUBSF3_OBJ_50);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003D0C4);

INCLUDE_ASM("asm/nonmatchings/1C38", _err_math);

INCLUDE_ASM("asm/nonmatchings/1C38", FERR_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/1C38", FERR_OBJ_54);

INCLUDE_ASM("asm/nonmatchings/1C38", _SpuInit);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuStart);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_init);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_D4);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_280);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_390);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_424);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_FiDMA);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_4EC);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_Fr_);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_t);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_78C);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_814);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_Fw);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_894);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_Fr);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_FsetRXX);

void SPU_OBJ_948(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_FsetRXXa);

void SPU_OBJ_9EC(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_FgetRXXa);

void SPU_OBJ_A28(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_FsetPCR);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_A7C);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_A88);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_AB0);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_Fw1ts);

INCLUDE_ASM("asm/nonmatchings/1C38", SPU_OBJ_B18);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003DDF4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003DE24);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003DE44);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuInitMalloc);

INCLUDE_ASM("asm/nonmatchings/1C38", S_M_INIT_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetNoiseClock);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetReverb);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SR_OBJ_B0);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SR_OBJ_B4);

INCLUDE_ASM("asm/nonmatchings/1C38", _SpuIsInAllocateArea);

INCLUDE_ASM("asm/nonmatchings/1C38", S_M_UTIL_OBJ_2C);

void S_M_UTIL_OBJ_78(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", _SpuIsInAllocateArea_);

INCLUDE_ASM("asm/nonmatchings/1C38", S_M_UTIL_OBJ_B8);

INCLUDE_ASM("asm/nonmatchings/1C38", S_M_UTIL_OBJ_104);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetIRQ);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SI_OBJ_F4);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SI_OBJ_12C);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetIRQAddr);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SIA_OBJ_30);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetIRQCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003E314);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuRead);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003E3A4);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetTransferStartAddr);

INCLUDE_ASM("asm/nonmatchings/1C38", S_STSA_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetTransferMode);

INCLUDE_ASM("asm/nonmatchings/1C38", S_STM_OBJ_1C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003E494);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuGetVoiceEnvelope);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuSetReverbModeType);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SRMT_OBJ_FC);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SRMT_OBJ_10C);

INCLUDE_ASM("asm/nonmatchings/1C38", S_SRMT_OBJ_1B4);

INCLUDE_ASM("asm/nonmatchings/1C38", _spu_setReverbAttr);

INCLUDE_ASM("asm/nonmatchings/1C38", SpuClearReverbWorkArea);

INCLUDE_ASM("asm/nonmatchings/1C38", S_CRWA_OBJ_9C);

INCLUDE_ASM("asm/nonmatchings/1C38", S_CRWA_OBJ_170);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8003ED24);

extern s32 D_80056640;
// copy D_80056640 to *a0
void func_8003ED54(s32 *a0) {
    *a0 = D_80056640;
}

INCLUDE_ASM("asm/nonmatchings/1C38", rsin);

INCLUDE_ASM("asm/nonmatchings/1C38", GEO_00_OBJ_2C);

INCLUDE_ASM("asm/nonmatchings/1C38", sin_1);

INCLUDE_ASM("asm/nonmatchings/1C38", GEO_00_OBJ_C4);

INCLUDE_ASM("asm/nonmatchings/1C38", rcos);

void GEO_01_OBJ_98(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", MatrixNormal_0);

INCLUDE_ASM("asm/nonmatchings/1C38", MatrixNormal_1);

INCLUDE_ASM("asm/nonmatchings/1C38", MatrixNormal_2);

INCLUDE_ASM("asm/nonmatchings/1C38", SetFogFar);

INCLUDE_ASM("asm/nonmatchings/1C38", SetFogNear);

INCLUDE_ASM("asm/nonmatchings/1C38", SetFogNearFar);

INCLUDE_ASM("asm/nonmatchings/1C38", InitGeom);

INCLUDE_ASM("asm/nonmatchings/1C38", SquareRoot0);

INCLUDE_ASM("asm/nonmatchings/1C38", InvSquareRoot);

INCLUDE_ASM("asm/nonmatchings/1C38", VectorNormalS);

INCLUDE_ASM("asm/nonmatchings/1C38", VectorNormal);

INCLUDE_ASM("asm/nonmatchings/1C38", VectorNormalSS);

INCLUDE_ASM("asm/nonmatchings/1C38", MSC02_OBJ_100);

INCLUDE_ASM("asm/nonmatchings/1C38", MatrixNormal);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadAverage12);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadAverage0);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadAverageShort12);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadAverageShort0);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadAverageByte);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadAverageCol);

INCLUDE_ASM("asm/nonmatchings/1C38", SquareRoot12);

INCLUDE_ASM("asm/nonmatchings/1C38", MulMatrix0);

INCLUDE_ASM("asm/nonmatchings/1C38", CompMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", ApplyMatrixLV);

INCLUDE_ASM("asm/nonmatchings/1C38", PushMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", PopMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", ScaleMatrixL);

INCLUDE_ASM("asm/nonmatchings/1C38", SetMulRotMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", MulMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", MulMatrix2);

INCLUDE_ASM("asm/nonmatchings/1C38", ApplyMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", ApplyMatrixSV);

INCLUDE_ASM("asm/nonmatchings/1C38", ScaleMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", SetRotMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", SetLightMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", SetColorMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", SetTransMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", SetVertex0);

INCLUDE_ASM("asm/nonmatchings/1C38", SetVertex1);

INCLUDE_ASM("asm/nonmatchings/1C38", SetVertex2);

INCLUDE_ASM("asm/nonmatchings/1C38", SetVertexTri);

INCLUDE_ASM("asm/nonmatchings/1C38", SetRGBfifo);

INCLUDE_ASM("asm/nonmatchings/1C38", SetIR123);

INCLUDE_ASM("asm/nonmatchings/1C38", SetIR0);

INCLUDE_ASM("asm/nonmatchings/1C38", SetSZfifo3);

INCLUDE_ASM("asm/nonmatchings/1C38", SetSZfifo4);

INCLUDE_ASM("asm/nonmatchings/1C38", SetSXSYfifo);

INCLUDE_ASM("asm/nonmatchings/1C38", SetRii);

INCLUDE_ASM("asm/nonmatchings/1C38", SetMAC123);

INCLUDE_ASM("asm/nonmatchings/1C38", SetData32);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDQA);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDQB);

INCLUDE_ASM("asm/nonmatchings/1C38", ReadGeomOffset);

INCLUDE_ASM("asm/nonmatchings/1C38", SetBackColor);

INCLUDE_ASM("asm/nonmatchings/1C38", SetFarColor);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800408C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800408E4);

INCLUDE_ASM("asm/nonmatchings/1C38", LocalLight);

INCLUDE_ASM("asm/nonmatchings/1C38", DpqColor);

INCLUDE_ASM("asm/nonmatchings/1C38", NormalColor);

INCLUDE_ASM("asm/nonmatchings/1C38", NormalColor3);

INCLUDE_ASM("asm/nonmatchings/1C38", NormalColorDpq);

INCLUDE_ASM("asm/nonmatchings/1C38", NormalColorDpq3);

INCLUDE_ASM("asm/nonmatchings/1C38", NormalColorCol);

INCLUDE_ASM("asm/nonmatchings/1C38", NormalColorCol3);

INCLUDE_ASM("asm/nonmatchings/1C38", ColorDpq);

INCLUDE_ASM("asm/nonmatchings/1C38", ColorCol);

INCLUDE_ASM("asm/nonmatchings/1C38", AverageSZ3);

INCLUDE_ASM("asm/nonmatchings/1C38", AverageSZ4);

INCLUDE_ASM("asm/nonmatchings/1C38", LightColor);

INCLUDE_ASM("asm/nonmatchings/1C38", DpqColorLight);

INCLUDE_ASM("asm/nonmatchings/1C38", DpqColor3);

INCLUDE_ASM("asm/nonmatchings/1C38", Intpl);

INCLUDE_ASM("asm/nonmatchings/1C38", Square12);

INCLUDE_ASM("asm/nonmatchings/1C38", Square0);

INCLUDE_ASM("asm/nonmatchings/1C38", AverageZ3);

INCLUDE_ASM("asm/nonmatchings/1C38", AverageZ4);

INCLUDE_ASM("asm/nonmatchings/1C38", OuterProduct12);

INCLUDE_ASM("asm/nonmatchings/1C38", OuterProduct0);

INCLUDE_ASM("asm/nonmatchings/1C38", Lzc);

INCLUDE_ASM("asm/nonmatchings/1C38", RotTransSV);

INCLUDE_ASM("asm/nonmatchings/1C38", SquareSS12);

INCLUDE_ASM("asm/nonmatchings/1C38", SquareSS0);

INCLUDE_ASM("asm/nonmatchings/1C38", SquareSL12);

INCLUDE_ASM("asm/nonmatchings/1C38", SquareSL0);

INCLUDE_ASM("asm/nonmatchings/1C38", RotTransPers);

INCLUDE_ASM("asm/nonmatchings/1C38", RotTransPers3);

INCLUDE_ASM("asm/nonmatchings/1C38", RotTrans);

INCLUDE_ASM("asm/nonmatchings/1C38", RotTransPers4);

INCLUDE_ASM("asm/nonmatchings/1C38", RotColorDpq);

INCLUDE_ASM("asm/nonmatchings/1C38", TransposeMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrix);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_01_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_01_OBJ_CC);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_01_OBJ_160);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrixYXZ);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_02_OBJ_68);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_02_OBJ_CC);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_02_OBJ_160);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrixZYX);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_03_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_03_OBJ_CC);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_03_OBJ_160);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrixX);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_04_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrixY);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_05_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrixZ);

INCLUDE_ASM("asm/nonmatchings/1C38", FGO_06_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", RotMatrix_gte);

INCLUDE_ASM("asm/nonmatchings/1C38", ratan2);

INCLUDE_ASM("asm/nonmatchings/1C38", RATAN_OBJ_B4);

INCLUDE_ASM("asm/nonmatchings/1C38", RATAN_OBJ_13C);

INCLUDE_ASM("asm/nonmatchings/1C38", RATAN_OBJ_150);

INCLUDE_ASM("asm/nonmatchings/1C38", _patch_gte);

INCLUDE_ASM("asm/nonmatchings/1C38", PATCHGTE_OBJ_DC);

INCLUDE_ASM("asm/nonmatchings/1C38", TransRot_32);

INCLUDE_ASM("asm/nonmatchings/1C38", TransRotPers);

INCLUDE_ASM("asm/nonmatchings/1C38", TransRotPers3);

INCLUDE_ASM("asm/nonmatchings/1C38", ApplyTransposeMatrixLV);

INCLUDE_ASM("asm/nonmatchings/1C38", PadInit);

INCLUDE_ASM("asm/nonmatchings/1C38", PadRead);

INCLUDE_ASM("asm/nonmatchings/1C38", PadStop);

INCLUDE_ASM("asm/nonmatchings/1C38", VSync);

INCLUDE_ASM("asm/nonmatchings/1C38", VSYNC_OBJ_A4);

INCLUDE_ASM("asm/nonmatchings/1C38", VSYNC_OBJ_164);

INCLUDE_ASM("asm/nonmatchings/1C38", VSYNC_OBJ_178);

INCLUDE_ASM("asm/nonmatchings/1C38", VSYNC_OBJ_200);

INCLUDE_ASM("asm/nonmatchings/1C38", ResetCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", InterruptCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", DMACallback);

INCLUDE_ASM("asm/nonmatchings/1C38", VSyncCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", VSyncCallbacks);

INCLUDE_ASM("asm/nonmatchings/1C38", StopCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", RestartCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", CheckCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", GetIntrMask);

INCLUDE_ASM("asm/nonmatchings/1C38", SetIntrMask);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_26C);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_410);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_43C);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_4EC);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_584);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_624);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_68C);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_OBJ_69C);

INCLUDE_ASM("asm/nonmatchings/1C38", startIntrVSync);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_VB_OBJ_58);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_VB_OBJ_C4);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_VB_OBJ_F0);

INCLUDE_ASM("asm/nonmatchings/1C38", startIntrDMA);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_DMA_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_DMA_OBJ_1CC);

void INTR_DMA_OBJ_270(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", INTR_DMA_OBJ_278);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800432D8);

INCLUDE_ASM("asm/nonmatchings/1C38", CdInit);

INCLUDE_ASM("asm/nonmatchings/1C38", EVENT_OBJ_74);

INCLUDE_ASM("asm/nonmatchings/1C38", EVENT_OBJ_84);

INCLUDE_ASM("asm/nonmatchings/1C38", EVENT_OBJ_AC);

INCLUDE_ASM("asm/nonmatchings/1C38", EVENT_OBJ_D4);

INCLUDE_ASM("asm/nonmatchings/1C38", CdStatus);

INCLUDE_ASM("asm/nonmatchings/1C38", CdMode);

INCLUDE_ASM("asm/nonmatchings/1C38", CdLastCom);

INCLUDE_ASM("asm/nonmatchings/1C38", CdLastPos);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReset);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_98);

INCLUDE_ASM("asm/nonmatchings/1C38", CdFlush);

INCLUDE_ASM("asm/nonmatchings/1C38", CdSetDebug);

INCLUDE_ASM("asm/nonmatchings/1C38", CdComstr);

void SYS_OBJ_108(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", CdIntstr);

void SYS_OBJ_13C(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", CdSync);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReady);

INCLUDE_ASM("asm/nonmatchings/1C38", CdSyncCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReadyCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", CdControl);

INCLUDE_ASM("asm/nonmatchings/1C38", CdControlF);

INCLUDE_ASM("asm/nonmatchings/1C38", CdControlB);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_538);

INCLUDE_ASM("asm/nonmatchings/1C38", CdMix);

INCLUDE_ASM("asm/nonmatchings/1C38", CdGetSector);

INCLUDE_ASM("asm/nonmatchings/1C38", CdGetSector2);

INCLUDE_ASM("asm/nonmatchings/1C38", CdDataCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", CdDataSync);

INCLUDE_ASM("asm/nonmatchings/1C38", CdIntToPos);

INCLUDE_ASM("asm/nonmatchings/1C38", CdPosToInt);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_0);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_25C);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_35C);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_3A8);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_428);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_4A8);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_548);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_sync);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_694);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_6C4);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_7AC);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_ready);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_914);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_944);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_A74);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_cw);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_D70);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_DA0);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_E88);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_vol);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_flush);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_initvol);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_initintr);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_init);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_1318);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_datasync);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_1440);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_getsector);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_getsector2);

INCLUDE_ASM("asm/nonmatchings/1C38", CD_set_test_parmnum);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_1688);

INCLUDE_ASM("asm/nonmatchings/1C38", BIOS_OBJ_16BC);

INCLUDE_ASM("asm/nonmatchings/1C38", CdSearchFile);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_108);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_2A4);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_2B0);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_2D8);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_2F8);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_594);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_5BC);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_640);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_660);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_7D4);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_7F0);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_8D4);

INCLUDE_ASM("asm/nonmatchings/1C38", ISO9660_OBJ_8FC);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_9C);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_158);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_260);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_32C);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_504);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReadBreak);

INCLUDE_ASM("asm/nonmatchings/1C38", CdRead);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_608);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_614);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReadSync);

INCLUDE_ASM("asm/nonmatchings/1C38", CDREAD_OBJ_740);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReadCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", CdReadMode);

INCLUDE_ASM("asm/nonmatchings/1C38", CdDiskReady);

INCLUDE_ASM("asm/nonmatchings/1C38", TYPE_OBJ_108);

INCLUDE_ASM("asm/nonmatchings/1C38", CdGetDiskType);

INCLUDE_ASM("asm/nonmatchings/1C38", TYPE_OBJ_178);

INCLUDE_ASM("asm/nonmatchings/1C38", TYPE_OBJ_250);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTReset);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTGetEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTPutEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTBufSize);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTin);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_1C0);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTout);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTinSync);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_22C);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCToutSync);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_268);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTinCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCToutCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_2C0);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_394);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_3A0);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_3B0);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_440);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_4CC);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_550);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_560);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_5E4);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_5F4);

INCLUDE_ASM("asm/nonmatchings/1C38", LIBPRESS_OBJ_60C);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTvlcSize2);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTvlc2);

INCLUDE_ASM("asm/nonmatchings/1C38", DecDCTvlcBuild);

INCLUDE_ASM("asm/nonmatchings/1C38", BUILD_OBJ_AC);

INCLUDE_ASM("asm/nonmatchings/1C38", InitHeap);

INCLUDE_ASM("asm/nonmatchings/1C38", FlushCache);

INCLUDE_ASM("asm/nonmatchings/1C38", _bu_init);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800471BC);

INCLUDE_ASM("asm/nonmatchings/1C38", SetMem);

INCLUDE_ASM("asm/nonmatchings/1C38", SystemError);

INCLUDE_ASM("asm/nonmatchings/1C38", DeliverEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", OpenEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", CloseEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", WaitEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", TestEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", EnableEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", DisableEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", OpenTh);

INCLUDE_ASM("asm/nonmatchings/1C38", CloseTh);

INCLUDE_ASM("asm/nonmatchings/1C38", ChangeTh);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_dr);

INCLUDE_ASM("asm/nonmatchings/1C38", ReturnFromException);

INCLUDE_ASM("asm/nonmatchings/1C38", ResetEntryInt);

INCLUDE_ASM("asm/nonmatchings/1C38", HookEntryInt);

INCLUDE_ASM("asm/nonmatchings/1C38", UnDeliverEvent);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800472E4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800472F4);

INCLUDE_ASM("asm/nonmatchings/1C38", open);

INCLUDE_ASM("asm/nonmatchings/1C38", lseek);

INCLUDE_ASM("asm/nonmatchings/1C38", read);

INCLUDE_ASM("asm/nonmatchings/1C38", write);

INCLUDE_ASM("asm/nonmatchings/1C38", close);

INCLUDE_ASM("asm/nonmatchings/1C38", nextfile);

INCLUDE_ASM("asm/nonmatchings/1C38", erase);

INCLUDE_ASM("asm/nonmatchings/1C38", ChangeClearPAD);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80047384);

INCLUDE_ASM("asm/nonmatchings/1C38", SysEnqIntRP);

INCLUDE_ASM("asm/nonmatchings/1C38", SysDeqIntRP);

INCLUDE_ASM("asm/nonmatchings/1C38", ChangeClearRCnt);

INCLUDE_ASM("asm/nonmatchings/1C38", SetRCnt);

INCLUDE_ASM("asm/nonmatchings/1C38", COUNTER_OBJ_74);

void COUNTER_OBJ_94(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", GetRCnt);

void COUNTER_OBJ_CC(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", StartRCnt);

INCLUDE_ASM("asm/nonmatchings/1C38", StopRCnt);

INCLUDE_ASM("asm/nonmatchings/1C38", ResetRCnt);

INCLUDE_ASM("asm/nonmatchings/1C38", COUNTER_OBJ_164);

INCLUDE_ASM("asm/nonmatchings/1C38", firstfile);

INCLUDE_ASM("asm/nonmatchings/1C38", FIRST_OBJ_D0);

INCLUDE_ASM("asm/nonmatchings/1C38", FIRST_OBJ_174);

INCLUDE_ASM("asm/nonmatchings/1C38", FIRST_OBJ_180);

INCLUDE_ASM("asm/nonmatchings/1C38", FIRST_OBJ_19C);

INCLUDE_ASM("asm/nonmatchings/1C38", FIRST_OBJ_25C);

INCLUDE_ASM("asm/nonmatchings/1C38", firstfile2);

INCLUDE_ASM("asm/nonmatchings/1C38", SetInitPadFlag);

INCLUDE_ASM("asm/nonmatchings/1C38", ReadInitPadFlag);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_init);

INCLUDE_ASM("asm/nonmatchings/1C38", InitPAD);

INCLUDE_ASM("asm/nonmatchings/1C38", StartPAD);

INCLUDE_ASM("asm/nonmatchings/1C38", StopPAD);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_OBJ_1A4);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_OBJ_21C);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_OBJ_254);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_OBJ_2BC);

INCLUDE_ASM("asm/nonmatchings/1C38", InitPAD2);

INCLUDE_ASM("asm/nonmatchings/1C38", StartPAD2);

INCLUDE_ASM("asm/nonmatchings/1C38", StopPAD2);

INCLUDE_ASM("asm/nonmatchings/1C38", PAD_init2);

INCLUDE_ASM("asm/nonmatchings/1C38", EnablePAD);

INCLUDE_ASM("asm/nonmatchings/1C38", DisablePAD);

INCLUDE_ASM("asm/nonmatchings/1C38", _patch_pad);

INCLUDE_ASM("asm/nonmatchings/1C38", _remove_ChgclrPAD);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80047C3C);

INCLUDE_ASM("asm/nonmatchings/1C38", strtol);

INCLUDE_ASM("asm/nonmatchings/1C38", setjmp);

INCLUDE_ASM("asm/nonmatchings/1C38", strcat);

INCLUDE_ASM("asm/nonmatchings/1C38", strcmp);

INCLUDE_ASM("asm/nonmatchings/1C38", strncmp);

INCLUDE_ASM("asm/nonmatchings/1C38", strcpy);

INCLUDE_ASM("asm/nonmatchings/1C38", strlen);

INCLUDE_ASM("asm/nonmatchings/1C38", bzero);

INCLUDE_ASM("asm/nonmatchings/1C38", memcpy);

INCLUDE_ASM("asm/nonmatchings/1C38", memset);

INCLUDE_ASM("asm/nonmatchings/1C38", rand);

INCLUDE_ASM("asm/nonmatchings/1C38", srand);

INCLUDE_ASM("asm/nonmatchings/1C38", printf);

INCLUDE_ASM("asm/nonmatchings/1C38", sprintf);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_94);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_1C0);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_25C);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_2A0);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_2C4);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_2D0);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_2DC);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_2E8);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_308);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_360);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_394);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_480);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_580);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_594);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_5A0);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_5A8);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_6A8);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_6C8);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_754);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_788);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_7A0);

INCLUDE_ASM("asm/nonmatchings/1C38", SPRINTF_OBJ_82C);

INCLUDE_ASM("asm/nonmatchings/1C38", memchr);

INCLUDE_ASM("asm/nonmatchings/1C38", memmove);

INCLUDE_ASM("asm/nonmatchings/1C38", MEMMOVE_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadTPage);

INCLUDE_ASM("asm/nonmatchings/1C38", EXT_OBJ_A8);

INCLUDE_ASM("asm/nonmatchings/1C38", EXT_OBJ_AC);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadClut);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadClut2);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDefDrawEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", EXT_OBJ_228);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDefDispEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", ResetGraph);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_110);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_160);

INCLUDE_ASM("asm/nonmatchings/1C38", SetGraphDebug);

INCLUDE_ASM("asm/nonmatchings/1C38", SetGraphQueue);

INCLUDE_ASM("asm/nonmatchings/1C38", GetGraphDebug);

INCLUDE_ASM("asm/nonmatchings/1C38", DrawSyncCallback);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDispMask);

INCLUDE_ASM("asm/nonmatchings/1C38", DrawSync);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_3E4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_4B4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_4F0);

INCLUDE_ASM("asm/nonmatchings/1C38", ClearImage);

INCLUDE_ASM("asm/nonmatchings/1C38", ClearImage2);

INCLUDE_ASM("asm/nonmatchings/1C38", LoadImage);

INCLUDE_ASM("asm/nonmatchings/1C38", StoreImage);

INCLUDE_ASM("asm/nonmatchings/1C38", MoveImage);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_788);

INCLUDE_ASM("asm/nonmatchings/1C38", ClearOTag);

INCLUDE_ASM("asm/nonmatchings/1C38", ClearOTagR);

INCLUDE_ASM("asm/nonmatchings/1C38", DrawPrim);

INCLUDE_ASM("asm/nonmatchings/1C38", DrawOTag);

INCLUDE_ASM("asm/nonmatchings/1C38", PutDrawEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", DrawOTagEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", GetDrawEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", PutDispEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_D4C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_F64);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_FF4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_100C);

INCLUDE_ASM("asm/nonmatchings/1C38", GetDispEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", GetODE);

INCLUDE_ASM("asm/nonmatchings/1C38", SetTexWindow);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawArea);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawOffset);

INCLUDE_ASM("asm/nonmatchings/1C38", SetPriority);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawStp);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1244);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawMode);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawEnv);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1400);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_149C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_15F8);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_16F0);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_170C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_172C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_17A8);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_17C4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1840);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_185C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1878);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_18F0);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_18F8);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1910);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_19F0);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1A48);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1A88);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1BF8);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1C20);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1C88);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1CC4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1E38);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1E5C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1EBC);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_1EF8);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_20BC);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_20DC);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2100);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2114);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2154);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_219C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_21CC);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_21F0);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_223C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_233C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_23F4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2484);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_24A0);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2700);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_281C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2850);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2870);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2888);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_297C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_298C);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_29C0);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2AF4);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2B04);

void SYS_OBJ_2B9C(void) {
}

INCLUDE_ASM("asm/nonmatchings/1C38", LoadImage2);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2C20);

INCLUDE_ASM("asm/nonmatchings/1C38", StoreImage2);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2D0C);

INCLUDE_ASM("asm/nonmatchings/1C38", MoveImage2);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2E00);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2EA8);

INCLUDE_ASM("asm/nonmatchings/1C38", DrawOTag2);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2F50);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2FBC);

INCLUDE_ASM("asm/nonmatchings/1C38", SYS_OBJ_2FE4);

INCLUDE_ASM("asm/nonmatchings/1C38", GPU_cw);

INCLUDE_ASM("asm/nonmatchings/1C38", OpenTIM);

INCLUDE_ASM("asm/nonmatchings/1C38", ReadTIM);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/1C38", OpenTMD);

INCLUDE_ASM("asm/nonmatchings/1C38", ReadTMD);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_35C);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_43C);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_45C);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_474);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_5E8);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_1854);

INCLUDE_ASM("asm/nonmatchings/1C38", TMD_OBJ_186C);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D174);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D208);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D268);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D4F0);

INCLUDE_ASM("asm/nonmatchings/1C38", GetTPage);

INCLUDE_ASM("asm/nonmatchings/1C38", GetClut);

INCLUDE_ASM("asm/nonmatchings/1C38", AddPrim);

INCLUDE_ASM("asm/nonmatchings/1C38", AddPrims);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D604);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D634);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D704);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawTPage);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawMove);

INCLUDE_ASM("asm/nonmatchings/1C38", SetDrawLoad);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_info);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_load);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_auto);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_write);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_wait);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_clear);

INCLUDE_ASM("asm/nonmatchings/1C38", _new_card);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004D968);

INCLUDE_ASM("asm/nonmatchings/1C38", InitCARD2);

INCLUDE_ASM("asm/nonmatchings/1C38", StartCARD2);

INCLUDE_ASM("asm/nonmatchings/1C38", StopCARD2);

INCLUDE_ASM("asm/nonmatchings/1C38", PATCH_OBJ_70);

INCLUDE_ASM("asm/nonmatchings/1C38", PATCH_OBJ_84);

INCLUDE_ASM("asm/nonmatchings/1C38", _patch_card);

INCLUDE_ASM("asm/nonmatchings/1C38", _patch_card2);

INCLUDE_ASM("asm/nonmatchings/1C38", _copy_memcard_patch);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004DB94);

INCLUDE_ASM("asm/nonmatchings/1C38", FORMAT_OBJ_0);

INCLUDE_ASM("asm/nonmatchings/1C38", FORMAT_OBJ_F0);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_format);

INCLUDE_ASM("asm/nonmatchings/1C38", FORMAT_OBJ_320);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_read);

INCLUDE_ASM("asm/nonmatchings/1C38", _card_status);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004DFF4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E034);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E2B0);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E3F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E4C8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E4E8);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E588);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E720);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E874);

INCLUDE_ASM("asm/nonmatchings/1C38", func_8004E8C4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80050874);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80050BC4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800512F4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80051354);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80051448);

INCLUDE_ASM("asm/nonmatchings/1C38", func_800514D4);

INCLUDE_ASM("asm/nonmatchings/1C38", func_80051514);

INCLUDE_ASM("asm/nonmatchings/1C38", index);
