#include "common.h"


void func_80035CE0(void) {
}


void func_80035CE8(void) {
}


void func_80035CF0(void) { func_8003882C(); }


void func_80035D10(void) { func_80038868(); }


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035D30);


extern s32 D_8008514C;
extern volatile u16 D_80085208;

void func_80035E00(void) {
    func_800393C8();
}


void func_80035E20(void) {
    func_800389CC();
}


s32 func_80035E40(void) {
    return D_8008514C;
}


s32 func_80035E50(void) {
    return (s16)D_80085208;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035E68);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035E8C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80035F70);


void func_80035FF4(s32 cmd, s32 overlay_id, s32 param, s32 load_addr, s32 callback1, s32 callback2) {
    extern s32 D_80085140;
    extern s32 D_80085144;
    extern u8 D_80085168[];
    u8 *slot;
    s32 write_idx;
    s32 was_equal;

    write_idx = D_80085140;
    slot = D_80085168 + write_idx * 20;
    *(s16 *)(slot + 0x0) = cmd;
    *(s16 *)(slot + 0x2) = param;
    *(s32 *)(slot + 0x8) = load_addr;
    *(s32 *)(slot + 0xC) = callback1;
    *(s32 *)(slot + 0x10) = callback2;
    *(s16 *)(slot + 0x4) = overlay_id;
    func_800472E4();
    was_equal = D_80085140;
    D_80085140 = (was_equal + 1) & 7;
    was_equal = was_equal == D_80085144;
    func_800472F4();
    if (was_equal == 1) {
        func_80035D30(cmd, overlay_id, param, load_addr);
    }
}


void func_800360D0(s32 a0, s32 a1) {
    func_80035FF4(0, 0x11, a0, a1, 0, 0);
}


void func_80036104(s32 overlay_id, s32 a1, s32 a2) {
    extern u32 load_table[]; // D_80053C58
    extern s32 D_8008514C;
    extern u8 D_8008520A;
    u32 descriptor;
    s32 dep;
    u32 *p;

    p = load_table + overlay_id * 2;
    descriptor = *p;
    D_8008514C = -2;
    dep = descriptor & 0xFF;
    descriptor &= 0xFFFFFF00;
    if (dep != D_8008520A) {
        if (dep != 0) {
            func_800360D0(dep, 0x801E0000);
            D_8008520A = dep;
        }
    }
    func_80035FF4(0, overlay_id, -1, descriptor, a1, a2);
}


void func_800361C0(s32 a0, s32 a1) {
    func_80035FF4(0, a0, -1, a1, 0, 0);
}


extern u8 D_80035F70[];
extern s32 D_80085140;
extern s32 D_80085144;

void func_800361F8(s32 a0, s32 a1) {
    func_80035FF4(0, 0x11, a0, a1, (s32)D_80035F70, a1);
}


s32 func_80036234(void) {
    return D_80085140 == D_80085144;
}


void func_80036254(s32 a0) {
    extern u8 D_80053CF0[];
    DrawSync(0);
    VSync(0);
    StoreImage(D_80053CF0, (u32 *)0x801BF000);
    DrawSync(0);
    VSync(0);
    ClearImage(D_80053CF0, 0, 0, 0);
    DrawSync(0);
    VSync(0);
    if (a0 < 0) {
        s16 rect[4];
        rect[0] = 0x300;
        rect[1] = 0;
        rect[2] = 0x80;
        rect[3] = 0xE0;
        MoveImage(rect, 0x180, 0);
    }
    DrawSync(0);
    VSync(0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003631C);


void func_80036444(void) {
    func_80036104(0, 0, 0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003646C);


// init_card_hand_slots - initializes 128 card slots (id=0, type=0xFF)

INCLUDE_ASM("asm/nonmatchings/264E0", func_80036690);


void func_800366E8(u8 *ptr) {
    s32 i = 0;
    do {
        i++;
        ptr[1] = 0xFF;
        ptr[0] = 0;
        ptr += 2;
    } while (i < 128);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036710);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003678C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003685C);


s32 func_80036978(s32 a0) {
    s32 result;
    if (a0 < 0x14) {
        result = 0;
    } else if (a0 < 0x27) {
        result = 1;
    } else if (a0 < 0x3A) {
        result = 2;
    } else if (a0 < 0x4E) {
        result = 3;
    } else if (a0 < 0x53) {
        result = 4;
    } else if (a0 < 0x5C) {
        result = 5;
    } else {
        result = 6;
    }
    return result;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800369CC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036B90);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036C74);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036D44);


void func_80036E8C(s32 a0) {
    extern u8 D_80077378[];
    D_80077378[0xAF4] = a0;
    D_80077378[0xAF5] = 0xFF;
    D_80077378[0xAF6] = 0xFF;
    func_80023888();
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036EC0);


u16 func_80036F60(void) {
    extern u8 D_80077378[];
    u16 mask;
    s32 i;
    s32 bit;
    u8 *p;

    mask = 0;
    i = 0;
    bit = 1;
    p = D_80077378;
    do {
        if (p[0x61] & 1) {
            mask |= (bit << i);
        }
        p += 0x44;
        i++;
    } while (i < 16);
    return mask;
}


void func_80036FA4(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078720[];
    s32 base1 = (s32)D_80077378;
    s32 off1 = a0 * 68;
    s32 base2 = (s32)D_80078720;
    *(s16 *)(base1 + off1 + 0x62) = *(u16 *)(base2 + a0 * 12 + 0x61A);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80036FE0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800370AC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037198);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800371D0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003720C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037240);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800372D0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800372DC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037308);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800375A0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037678);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800376A8);


// mc_xor_checksum

INCLUDE_ASM("asm/nonmatchings/264E0", func_8003777C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800377B4);


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


void func_800378B0(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0x4D;
    a0[1] = 0x43;
    a0[0x7F] = func_8003786C(a0);
}


void func_800378F4(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0xA0;
    a0[8] = 0xFF;
    a0[9] = 0xFF;
    a0[0x7F] = func_8003786C(a0);
}


void func_8003793C(u8 *a0) {
    func_80037894(a0);
    a0[0] = 0xFF;
    a0[1] = 0xFF;
    a0[2] = 0xFF;
    a0[3] = 0xFF;
    a0[8] = 0xFF;
    a0[9] = 0xFF;
    a0[0x7F] = func_8003786C(a0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003798C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800379AC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037ACC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037ADC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037AEC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037B44);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037B7C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037BB0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037BF0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037C30);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037C6C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037CD4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037D40);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037E14);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037E60);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037F08);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80037FB0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038030);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800381BC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038308);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038364);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003837C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800383B8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038424);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038440);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038464);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038490);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800385B0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003863C);


void func_80038668(s32 a0) {
    extern u8 D_8008A3D8;
    extern s16 D_8008A3D0;
    extern u8 D_8008A3DA;
    s32 result;
    u8 buf;

    D_8008A3D8 = 0;
    (&D_8008A3D8)[1] = 0;
    D_8008A3D0 = 0;

    if (a0 != 0) {
        do { } while (!CdInit());
        CdSetDebug(0);
        buf = 0x80;
        CdControlB(0xE, &buf, 0);
        VSync(3);
        result = func_800385B0();
        D_8008A3DA = result;
        func_80038CF0((s8)result);
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800386F0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038708);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038720);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038760);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800387F8);


s32 func_8003882C(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_80038760(3, a0, a1, a2, a3);
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038868);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800388CC);


s32 func_80038920(s32 a0, s32 a1, s32 a2, s32 a3) {
    do {
    } while (func_8003882C(a0, a1, a2, a3) != 0);
    do {
    } while (func_800393C8() != 0);
    return 0;
}


s32 func_80038994(void) {
    s32 saved = func_80038868();
    do {
    } while (func_800393C8() != 0);
    return saved;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800389CC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038A18);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038A60);


extern s8 D_8008A3DA;
// get D_8008A3DA (s8)
s8 func_80038CE0(void) {
    return D_8008A3DA;
}


extern u16 D_8008A3D2;
extern u8 D_8008A3D9;

void func_80038CF0(s32 a0) {
    D_8008A3D2 = a0;
}


u32 func_80038CFC(void) {
    return D_8008A3D9;
}


void func_80038D0C(void) {
    CdFlush();
    CdControl(9, 0, 0);
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038D3C);


void func_80038D74(void) {
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038D7C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038DD4);


void func_80038E28(void) {
    extern u32 D_8008A3C8;
    extern u8 D_8008A3D9;

    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8 = 0;
        D_8008A3D9 = 5;
        func_80038ED4();
        break;
    case 5:
        D_8008A3C8++;
        if (D_8008A3C8 >= 0x708) {
            D_8008A3C8 = 0;
            func_8001313C(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D9 = 3;
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038ED4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80038F88);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039040);


void func_80039094(void) {
    extern u32 D_8008A3C8;
    extern u8 D_8008A3D9;

    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8 = 0;
        D_8008A3D9 = 9;
        func_80039140();
        break;
    case 5:
        D_8008A3C8++;
        if (D_8008A3C8 >= 0x708) {
            D_8008A3C8 = 0;
            func_8001313C(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D9 = 7;
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039140);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039218);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039344);


void func_80039388(void) {
    extern u8 D_8008A3D9;
    if (CdSync(1, 0) == 2) {
        CdReadBreak();
        D_8008A3D9 = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_800393C8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039444);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003947C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800394D0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039520);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039678);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800396E0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039728);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003974C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039764);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003987C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_800398A8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039910);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039AA0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039AB4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039B80);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039BC4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039EF8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_80039FE8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A1F8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A470);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A500);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A534);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A57C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A654);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A700);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003A7C4);


void func_8003AB64(void) { func_80039AA0(); }


void func_8003AB84(void) { func_80039AB4(); }


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ABA4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ABC4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AC10);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ACDC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003ADD4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AEA8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AF50);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AF88);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003AFD0);


void func_8003B024(u8 *a0, s32 a1, s32 a2) {
    *(s32 *)(a0 + 0x28) = a1;
    a0[0x34] = a2;
}


void func_8003B030(u8 *a0, s32 a1, s32 a2, s32 a3) {
    a0[0x37] = a1;
    *(s32 *)(a0 + 0x2C) = a2;
    a0[0x36] = a3;
}


void func_8003B040(u8 *a0) {
    switch (a0[0x46]) {
        case 2:
            func_8003BB98(a0);
            break;
        case 3:
            func_8003BBAC(a0, a0[0xE4]);
            break;
        case 4:
            func_8003BBEC(a0, a0[0x47]);
            break;
    }
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B0C4);


s32 func_8003B334(u8 *a0) {
    s32 v0 = a0[0xE3];
    s32 a1val = a0[0xE9];
    u16 ecval = *(u16 *)(a0 + 0xEC);
    s32 part1 = ((v0 + 1) >> 1) << 2;
    s32 part2 = ((a1val * 5 + 3) & 0xFFC) + 4;
    return part1 + part2 + ecval;
}


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B36C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B440);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B4E8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B8E0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B948);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003B964);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BA2C);


// snd_voice_cmd_play_note - cmd=0x43, sets payload byte

INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BAC4);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BB18);


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


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BC0C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BC24);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BD84);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BDEC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BEE0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BEF0);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003BFAC);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C228);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C260);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C284);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C2B8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C3C8);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C62C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C70C);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C744);


INCLUDE_ASM("asm/nonmatchings/264E0", func_8003C764);
