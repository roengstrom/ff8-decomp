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
extern u8 D_801FAB38[];
extern u8 D_80078720[];
extern u8 D_801F7F78[];
extern u8 D_801FAB12;
extern s16 D_801FAA1E;
extern u8 D_801FABC4[];
extern u8 D_801F8BB8[];
extern u16 D_800780E8;
extern u8 D_80077864[];
extern u8 D_80056290[];
extern u8 D_801F7E6C[];
extern u8 D_801FAB88[];
extern u16 D_8007737C;
extern s32 D_80083848;
extern u8 D_801FAB00[];

s32 func_8002E744(s32);
void func_801F1A40(s32);
s32 func_801F179C(void *, void *);
s32 func_801F42A4();
s32 func_801F3FE8();
s32 func_801F4744();
void func_801EF9AC(s32, s32, s32, s32);
s32 func_801F6358(s32, s32, s32, s32, s32);
s32 func_801F0FEC(s32, s32, s32, s32, s32, s32);
s32 func_801F6AD0(s32);
s32 func_801F5F60(s32, s32, s32, s32);
void func_801EFBB4(s32, s32, s32);
s32 func_801F64A4(s32, s32, s32, s32, s32, s32, s32);
s32 func_801F3DE4(s32, s32, s32, s32, s32, s32, s32);
s32 func_801F6234(s32, s32, s32, s32, s32);
void func_801F605C(s32, s32, s32, s32, s32);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF800);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801EF8D8);

void func_801EF934(s32 a0, s32 a1, s32 *a2) {
    s32 save0 = a2[0];
    s32 save1 = a2[1];
    u16 *rect = (u16 *)a2;
    rect[0] += 1;
    rect[1] += 1;
    rect[2] -= 2;
    rect[3] -= 2;
    func_801EF800(a0, a1);
    a2[0] = save0;
    a2[1] = save1;
}

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

void func_801F0464(s32 a0) {
    s32 buf[2];
    buf[0] = 0;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    func_80048DD4(buf, 0, 0, 0);
    func_80048C50(0);
    buf[0] = 0x200;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    func_80048DD4(buf, 0, 0, 0);
    func_80048C50(0);
}

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

void func_801F09C4(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (!((D_801FAB12 + D_801FAA1E) & 1)) {
        a0 = -1;
    }
    func_801F0954(a0, a1, a2, a3);
}

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

s32 func_801F0BB0(void) {
    s8 val;

    D_801FAA11--;
    val = D_801FAA11;
    if (val < 0) {
        D_801FAA11 = 0;
        return -1;
    }
    return D_801FAA18[val];
}

void func_801F0BF8(s32 a0) {
    u8 *entry = D_801F7E6C + a0 * 8;
    s32 val = *(s32 *)(entry + 4);
    if (val == 0xFF) return;
    if (val <= 0) return;
    if (func_80035E40() == val) return;
    func_80036104(val, 0, 0);
}

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

void func_801F177C(s32 *a0) {
    s32 i;

    for (i = 21; i >= 0; i--) {
        a0[i + 8] = 0;
    }
}

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

void func_801F1AAC(void) {
    s32 result = func_801F179C(func_801F1A40, func_801F1AA4);

    if (result != 0) {
        func_801F1A40(result);
    }
}

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

void func_801F1D34(u8 *a0) {
    s32 count;
    s32 sentinel;

    D_801FAB2C = (s32)a0;
    D_801FAB31 = 0;
    if (a0 == 0) {
        D_801FAB30 = 0;
    } else {
        count = 0;
        sentinel = 0xFF;
    top:
        if (*a0 == sentinel) goto out;
        a0 += 2;
        count++;
        goto top;
    out:
        D_801FAB30 = count;
    }
}

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

void func_801F1DBC(s32 a0) {
    u8 *ptr = D_801F7F78;
    s32 i;

    for (i = 0; ; i++, ptr += 2) {
        if (*ptr == 0xFF) {
            return;
        }
        if (*ptr == a0) {
            func_801F1D34(D_801F7F78);
            D_801FAB31 = i;
            return;
        }
    }
}

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

void func_801F1F98(u8 *a0) {
    u8 *dst = D_801FAB88;
    s32 i;
    u8 *p;
    s32 fill;

    fill = 0xFF;
    i = 8;
    p = dst + 8;
    do {
        *p = fill;
        i--;
        p--;
    } while (i >= 0);

    for (i = 0; i < 3; i++) {
        u8 val = *(a0 + i + 0x35);
        if (val != 0xFF) {
            *dst++ = val;
        }
    }

    for (i = 0; i < 8; i++) {
        u8 val = *(a0 + i + 0x38);
        if (val != 0xFF) {
            *dst++ = val;
        }
    }
}

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

void func_801F22A8(void) {
    s32 result = 0;
    s32 i;
    s32 base;
    REGALLOC_BARRIER(result);
    i = 0;
    base = (s32)D_80077378;

    for (; i < 3; i++) {
        u8 val = *(u8 *)(i + base + 0xAF4);
        if (val != 0xFF) {
            result |= (1 << val);
        }
    }
    D_801FABB8 = result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F22F4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2370);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F23D0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2458);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F2FAC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3270);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3464);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F36E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3824);

void func_801F38F8(s32 a0, s32 a1, s32 a2) {
    s32 ret1;
    s32 ret2;

    ret1 = func_801F6AD0(*(u8 *)(a0 + 0x46));
    ret2 = func_801F0FEC(a1, a2, 0x22, 0xF, ret1, 7);
    D_801FAB00[0x10] = 0x55;
    D_801FAB00[0x11] = 0;
    *(u16 *)D_801FAB00 = 0x18;
    *(u16 *)(D_801FAB00 + 2) = 7;
    *(s32 *)(D_801FAB00 + 4) = 0x001900F4;
    func_801EF9AC(a1, ret2, 0x1000, D_80083848);
}

void func_801F3994(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F0FEC(a1, a2, a3, a4, a0, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F39D0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3ABC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3B64);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3CE0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3DE4);

s32 func_801F3FB4(s32 a0) {
    s32 v1 = 7;

    if (a0 & 0x100) {
        v1 = 2;
    }
    if (a0 & 0xFE) {
        v1 = 5;
    }
    if (a0 & 1) {
        v1 = 1;
    }
    return v1;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F3FE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4168);

void func_801F4274(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F4168(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F42A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4454);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4744);

s32 func_801F486C(u8 *a0, s32 a1) {
    s32 i;
    s32 result;

    if (a0[0x34] == 0) {
        for (i = 0; i < 3; i++) {
            result = func_801F42A4(a0, a1, result, i);
        }
    } else {
        for (i = 0; i < 3; i++) {
            result = func_801F3FE8(a0, a1, result, i);
        }
        result = func_801F4744(a0, a1);
    }
    return result;
}

void func_801F4918(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F6358(a1, a2, 0x22, 0xC6, (s32)D_8007737C);
    D_801FAB00[0x10] = 0;
    D_801FAB00[0x11] = 0;
    *(u16 *)D_801FAB00 = 0x18;
    *(u16 *)(D_801FAB00 + 2) = 0xBE;
    *(u16 *)(D_801FAB00 + 4) = 0xF4;
    *(u16 *)(D_801FAB00 + 6) = 0x1A;
    func_801EF9AC(a1, ret, 0x1000, D_80083848);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F49A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4A98);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4C60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4CE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4D70);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F4EA8);

s32 func_801F5104(s32 a0) {
    return a0 - 0x64;
}

s32 func_801F510C(s32 a0) {
    s32 v1;

    if (a0 < 0x385) {
        a0 -= 0x320;
    } else {
        a0 -= 0x384;
    }
    v1 = 0;
    if (a0 >= 0) {
        v1 = 0x64;
        if (a0 < 0x65) {
            v1 = a0;
        }
    }
    return v1;
}

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

void func_801F5300(void) {
    s32 i = 0;
    s32 dstBase = (s32)D_801FABC4;
    s32 srcBase = (s32)D_80077378;

    for (; i < 3; i++) {
        *(u8 *)(i + dstBase) = *(u8 *)(i + srcBase + 0xAF4);
        *(u8 *)(i + srcBase + 0xAF4) = 0xFF;
    }
}

void func_801F5340(void) {
    s32 i = 0;
    s32 dstBase = (s32)D_80077378;
    s32 srcBase = (s32)D_801FABC4;

    for (; i < 3; i++) {
        *(u8 *)(i + dstBase + 0xAF4) = *(u8 *)(i + srcBase);
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F537C);

void func_801F5400(s32 a0) {
    func_801F5300();
    func_801F5190(a0);
    func_801F5340();
    func_80023888();
}

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

void func_801F5868(s32 a0, s16 a1) {
    if (a0 >= 16) {
        s32 idx = a0 - 16;
        s32 base1 = (s32)D_80077378;
        s32 base2 = (s32)D_80078720;
        *(s16 *)(base1 + idx * 68 + 0x62) = a1;
        *(s16 *)(base2 + idx * 12 + 0x618) = a1;
    } else {
        s32 base1 = (s32)D_80077378;
        s32 base2 = (s32)D_801FABC8;
        *(s16 *)(base1 + a0 * 152 + 0x490) = a1;
        *(s16 *)(base2 + a0 * 32 + 0x8) = a1;
    }
}

s32 func_801F58EC(s32 a0) {
    if (a0 < 16) {
        s32 base = (s32)D_801FABC8;
        return *(s16 *)(base + a0 * 32 + 0xA);
    } else {
        s32 idx = a0 - 16;
        s32 base = (s32)D_80078720;
        return *(s16 *)(base + idx * 12 + 0x61A);
    }
}

s32 func_801F5938(s32 a0) {
    if (a0 < 16) {
        s32 base = (s32)D_801FABC8;
        return *(u8 *)(base + a0 * 32 + 0xC);
    } else {
        s32 idx = a0 - 16;
        s32 base = (s32)D_80078720;
        return *(u8 *)(base + idx * 12 + 0x620);
    }
}

s32 func_801F5984(u16 *src, u16 *dst, s32 a2) {
    s32 accum = 0;
    s32 count = 0;
    s16 val;
    s32 ret;
    *dst++ = 0;
    while (1) {
        val = (s16)*src++;
        if (val == -1) break;
        ret = func_801F08D4(1, a2, val, 0);
        ret = func_8002E744(ret) + 12;
        accum += ret;
        *dst++ = accum;
        count++;
    }
    return count;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5A38);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5B54);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5C84);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5D5C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5E0C);

void func_801F5EFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 1);
}

s32 func_801F5F30(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F5F60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F605C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6234);

s32 func_801F6324(s32 a0) {
    u16 *table = (u16 *)D_801F8BB8;

    if (a0 >= table[0]) {
        a0 = 0;
    }
    a0 += 1;
    return table[a0] + (s32)table;
}

s32 func_801F6358(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    u8 buf[256];
    s32 ptr = func_801F6324(a4);
    func_8002F688(ptr, buf, -1);
    func_801F0FEC(a0, a1, a2, a3, buf, 7);
}

void func_801F63DC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_800375A0(a1, a2, a0, a3, a4, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6418);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F64A4);

void func_801F65F0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 ret;

    ret = func_801F64A4(a0, a1, a2, a3, a4, a5, 1);
    ret = func_801F3DE4(*(s32 *)a5, *(s32 *)(a5 + 4), a0, ret, a2 + 0x20, a3 + 0x7C, 7);
    ret = func_801F6234(a0, ret, a2 + 0xD0, a3 + 0x39, *(u16 *)(a5 + 0xE));
    func_801F605C(a0, ret, a2 + 0x10E, a3 + 0x38, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F66B0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6768);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6800);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6888);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6934);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F6A5C);

void func_801F6AA4(s32 a0) {
    func_801F08D4(1, 3, a0, 0);
}

s32 func_801F6AD0(s32 a0) {
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

void func_801F6F88(s32 a0) {
    s32 hi = a0 >> 4;
    s32 lo = a0 & 0xF;
    s32 val = func_801F6F6C();

    func_801F0994(lo | hi, (u16)val, (val >> 16) + lo * 13);
}

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

void func_801F739C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 ret1;
    s32 ret2;

    D_801FAB00[0x10] = 0x4A;
    D_801FAB00[0x11] = 0;
    *(u16 *)D_801FAB00 = a2;
    *(u16 *)(D_801FAB00 + 4) = 0x9A;
    *(u16 *)(D_801FAB00 + 2) = a3;
    *(u16 *)(D_801FAB00 + 6) = 0x40;
    D_801FAB00[0x13] = 4;
    D_801FAB00[0x1E] = a4;

    ret1 = func_801F5F30(a0, a1, a2 + 0x24, a3, D_80083848, D_801FAB00[0x16]);
    ret2 = func_801F5F60(a0, ret1, D_80083848, 3);
    func_801EFBB4(a0, ret2, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F7454);

s32 func_801F75A4(s32 a0) {
    s32 result = func_801F7454(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F75EC);

void func_801F76A8(s32 a0) {
    func_801F75EC(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
}

s32 func_801F76E0(s32 flags, s32 a1, s32 a2) {
    s32 result = a2;
    s32 orig = a2;

    if (flags & 0x2000) {
        result = func_80035B28(a1, result);
    }
    if (flags & 0x8000) {
        result = func_80035B70(a1, orig);
    }
    if (result != orig) {
        func_80030D48(1);
    }
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F776C);

s32 func_801F77F8(s32 a0, s32 a1, s32 a2) {
    s32 offset = a0 * 152;
    u8 *ptr = (u8 *)((s32)D_80077864 + offset);
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*ptr++ != a1) continue;
        return i;
    }
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain", func_801F784C);

void func_801F78D8(s32 a0, s32 a1) {
    s32 offset = a0 * 152;
    u8 *ptr = (u8 *)((s32)D_80077864 + offset);
    s32 i;
    for (i = 0; i < 19; i++) {
        if (*ptr == a1) {
            *ptr = 0;
        }
        ptr++;
    }
}

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

s32 func_801F7C20(s32 a0) {
    s32 result = a0 & 0x7FF;
    if (a0 & 0x800)   result |= 0x800;
    if (a0 & 0x2000)  result |= 0x1800;
    if (a0 & 0x4000)  result |= 0x7800;
    if (a0 & 0x1000)  result |= 0x8000;
    if (a0 & 0x8000)  result |= 0x18000;
    if (a0 & 0x10000) {
        s32 mask = 0x10000;
        result |= 0x8000;
        result |= mask;
        result |= 0x20000;
        result |= 0x40000;
    }
    return result;
}

void func_801F7C98(s32 a0, s32 a1) {
    u8 buf[16];
    func_8002F294(a0, buf, 0x30);
    func_8002A2A8(a1, D_80056290);
    func_8002A2C4(a1, &buf[3]);
}
