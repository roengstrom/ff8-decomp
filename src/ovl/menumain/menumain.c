#include "common.h"

extern u8 D_801FA278;
extern u8 D_801FA279;
extern u8 D_801FA27A;
extern u8 D_801FA27B;
extern u8 D_801FA27C;
extern s8 D_801FAA11;
extern u8 D_801FAA18[];
extern s16 D_801FAAE0;
extern u8 D_801FAB7A;
extern u8 D_801FAB7B;
extern s16 D_801FAB78;
extern u16 D_80083850;
extern u8 D_801FAA10;
extern u16 D_801FAA12;
extern u16 D_801FAA14;
extern s16 D_801FAB28;
extern s16 D_801FAB2A;
extern s32 D_801FAB2C;
extern u8 D_801FAB30;
extern u8 D_801FAB31;
extern u8 D_801FAAF0;
extern u8 D_801FAAF8;
extern u16 D_801FABB8;
extern s32 D_801FACE8;
extern u16 D_801FACE2;
extern u16 D_801FACE4;
extern s32 D_801FAAE4;
extern s32 D_801FAAE8;
extern u16 D_801FAAF2;
extern u8 D_80077E5F;
extern u16 D_80077E5C;
extern u8 D_801F889C[];
extern u8 D_801F7F98[];
extern u8 D_80077378[];
extern u8 D_80077E6C[];
extern u8 D_801FABC8[];
extern u8 D_80077818[];
extern u8 D_80077EBC[];

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF800);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF8D8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF934);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF9AC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EFBB4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EFF64);

s32 func_801EFFB8(void) {
    return D_801FA27C;
}

void func_801EFFC8(s32 a0) {
    D_801FA27B = a0;
}

s32 func_801EFFD4(void) {
    return D_801FA27B;
}

void func_801EFFE4(s32 a0) {
    D_801FA27A = a0;
}

s32 func_801EFFF0(void) {
    return D_801FA27A;
}

void func_801F0000(s32 a0) {
    D_801FA278 = a0;
}

s32 func_801F000C(void) {
    return D_801FA278;
}

void func_801F001C(s32 a0) {
    D_801FA279 = a0;
}

s32 func_801F0028(void) {
    return D_801FA279;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0038);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F00A0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F010C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0224);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0274);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F03E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0464);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F04E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F07D8);

void func_801F0884(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_801F07D8(a0, a1, a2, a3, a4, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F08AC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F08D4);

void func_801F0948(s32 a0) {
    D_801FAAE0 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0954);

void func_801F0994(s32 a0, s32 a1, s32 a2) {
    func_801F0954(a0, D_801FAAE0, a1, a2);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F09C4);

void func_801F0A04(s32 a0, s32 a1, s32 a2) {
    func_801F09C4(a0, D_801FAAE0, a1, a2);
}

void func_801F0A34(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a0) {
        func_801F0994(a1, a2, a3);
    } else {
        func_801F0A04(a1, a2, a3);
    }
}

void func_801F0A78(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    if (a0) {
        func_801F0954(a1, a2, a3, a4);
    } else {
        func_801F09C4(a1, a2, a3, a4);
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0AC8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0BB0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0BF8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0C5C);

s32 func_801F0D84(void) {
    return D_801FAA18[(s8)D_801FAA11];
}

void func_801F0DA4(s32 a0) {
    D_801FAA10 = a0;
}

s32 func_801F0DB0(void) {
    return D_801FAA10;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0DC0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0E5C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0F20);

void func_801F0FD0(s32 a0, s32 a1) {
    if (!a1) {
        a1 = 0x180;
    }
    D_801FAA12 = a1;
    D_801FAA14 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F0FEC);

s32 func_801F1200(void) {
    return D_801FAAF8;
}

void func_801F1210(s32 a0, s32 a1) {
    D_801FAAE4 = a0;
    D_801FAAE8 = a1;
    D_801FAAF2 = 0;
    D_801FAAF0 = 1;
    D_801FAAF8 = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1240);

void func_801F12F0(void) {
    D_801FAAF0 = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F12FC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1584);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F16AC);

void func_801F175C(void) {
    func_801F16AC();
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F177C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F179C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1850);

void func_801F18FC(s32 *a0) {
    s32 *next = (s32 *)a0[1];
    s32 *prev = (s32 *)a0[0];
    *next = (s32)prev;
    prev[1] = (s32)next;
    *(u8 *)((u8 *)a0 + 0x12) = 0;
    a0[2] = 0;
    a0[3] = 0;
    *(s16 *)((u8 *)a0 + 0x10) = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1924);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1A40);

s32 func_801F1AA4(s32 a0, s32 a1, s32 a2) {
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1AAC);

void func_801F1AE8(s32 a0, s32 a1) {
    D_801FAB7A = a0;
    D_801FAB7B = a1;
}

void func_801F1AFC(void) {
    D_801FAB78 = D_80083850;
}

void func_801F1B10(void) {
    func_8002E7C4(D_801FAB78);
    func_80030058(D_801FAB78);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1B4C);

void func_801F1CAC(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_801F1B4C(i);
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1CE8);

void func_801F1D2C(void) {
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1D34);

void func_801F1D84(void) {
    D_801FAB28 = 0;
    D_801FAB2A = 0;
    D_801FAB2C = 0;
    D_801FAB30 = 0;
    D_801FAB31 = 0;
}

void func_801F1DB0(s32 a0) {
    D_801FAB28 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1DBC);

void func_801F1E20(u8 *a0) {
    u8 *src = D_80077E6C;
    u8 *dst = a0 + 0x35;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

void func_801F1E54(u8 *a0) {
    u8 *dst = D_80077E6C;
    u8 *src = a0 + 0x35;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1E88);

void func_801F1F58(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 1);
}

void func_801F1F78(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F1F98);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F202C);

s32 func_801F21D0(s32 a0) {
    s32 base = (s32)D_80077378;
    return *(u16 *)(base + a0 * 152 + 0x526);
}

void func_801F21FC(s32 a0, s32 a1) {
    s32 base = (s32)D_80077378;
    s32 base2 = (s32)D_801FABC8;
    *(s16 *)(base + a0 * 152 + 0x526) = a1;
    *(s16 *)(base2 + a0 * 32 + 0xE) = a1;
}

s32 func_801F2238(s32 a0) {
    return 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2240);

s32 func_801F2298(void) {
    return D_801FABB8;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F22A8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F22F4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2370);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F23D0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2458);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2FAC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3270);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3464);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F36E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3824);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F38F8);

void func_801F3994(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F0FEC(a1, a2, a3, a4, a0, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F39D0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3ABC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3B64);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3CE0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3DE4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3FB4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3FE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4168);

void func_801F4274(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F4168(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F42A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4454);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4744);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F486C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4918);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F49A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4A98);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4C60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4CE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4D70);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4EA8);

s32 func_801F5104(s32 a0) {
    return a0 - 0x64;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F510C);

s32 func_801F5144(s32 a0) {
    return a0 >= 0x385;
}

s32 func_801F5150(s32 a0, s32 a1, s32 a2) {
    a2 &= ~0x301;
    if (a0 < (a1 >> 1)) {
        a2 |= 0x200;
    }
    if (a0 < (a1 >> 2)) {
        a2 |= 0x300;
    }
    if (a0 <= 0) {
        a2 = 1;
    }
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5190);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5300);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5340);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F537C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5400);

void func_801F5440(void) {
    s32 i;
    func_801F5300();
    for (i = 0; i < 8; i++) {
        func_801F5190(i);
    }
    func_801F5340();
    func_80023888();
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5490);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F565C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F56E4);

void func_801F576C(s32 a0, s32 a1) {
    if (a0 >= 16) {
        func_801F2238(a0 - 16);
    } else {
        func_801F21FC(a0, a1);
    }
}

s32 func_801F57A4(s32 a0) {
    if (a0 >= 16) {
        return func_801F2240(a0 - 16);
    } else {
        return func_801F21D0(a0);
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F57DC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5868);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F58EC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5938);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5984);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5A38);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5B54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5C84);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5D5C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5E0C);

void func_801F5EFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 1);
}

void func_801F5F30(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5F60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F605C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6234);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6324);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6358);

void func_801F63DC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_800375A0(a1, a2, a0, a3, a4, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6418);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F64A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F65F0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F66B0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6768);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6800);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6888);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6934);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6A5C);

void func_801F6AA4(s32 a0) {
    func_801F08D4(1, 3, a0, 0);
}

void func_801F6AD0(s32 a0) {
    func_801F08D4(0, 0, a0, 1);
}

void func_801F6AFC(s32 a0) {
    func_801F08D4(0, 0, a0, 0);
}

s32 func_801F6B28(s32 a0) {
    return D_801F7F98[D_801F889C[a0 * 4]];
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6B54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6C9C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6D88);

s32 func_801F6F6C(void) {
    return (D_801FACE4 << 16) | D_801FACE2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6F88);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6FE4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7148);

void func_801F728C(s32 a0, s32 a1) {
    func_801F7148(a0, 0, 0, a1);
}

s32 func_801F72B4(void) {
    return D_801FACE8;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F72C4);

s32 func_801F738C(s32 a0) {
    return a0 + 0x10;
}

s32 func_801F7394(s32 a0) {
    return a0 + 0x11;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F739C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7454);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F75A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F75EC);

void func_801F76A8(s32 a0) {
    func_801F75EC(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F76E0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F776C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F77F8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F784C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F78D8);

void func_801F7928(void) {
    s32 val = D_80077E5C & 2;
    func_8001327C(val != 0);
}

void func_801F7954(void) {
    s32 a1 = 0;
    if (D_80077E5C & 0x40) {
        a1 = 0xFF;
    }
    func_80027EF8(0, a1);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F798C);

s32 func_801F79F8(s32 a0) {
    return D_80077E5F & a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7A08);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7A54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7AD4);

void func_801F7B10(s32 a0) {
    s32 offset = a0 * 152;
    s32 base = (s32)D_80077818;
    u8 *ptr = (u8 *)(base + offset);
    s32 i;
    for (i = 0; i < 32; i++) {
        if (ptr[1] == 0) {
            ptr[0] = 0;
        }
        ptr += 2;
    }
}

void func_801F7B60(void) {
    u8 *a0 = D_80077EBC;
    s32 a2 = 0;
    u8 *v1 = a0 + 1;
    do {
        u8 b = *v1;
        u8 a = *a0;
        if (b == 0) {
            *a0 = 0;
        }
        if (a == 0) {
            *v1 = 0;
        }
        a2++;
        v1 += 2;
        a0 += 2;
    } while (a2 < 198);
}

s32 func_801F7BAC(s32 a0) {
    return a0 * 100 / 255;
}

s32 func_801F7BE4(s32 a0) {
    return a0;
}

void func_801F7BEC(s32 a0) {
    if (a0 & 0x40) {
        func_80030D48(2);
    } else {
        func_80030D48(3);
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7C20);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7C98);
