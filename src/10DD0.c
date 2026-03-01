#include "common.h"

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800205D0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020608);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020644);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020670);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002069C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800206A8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800206B4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800206F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020740);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020778);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800207B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800207F0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020830);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020870);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800208B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800208E8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020920);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020960);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800209A0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020AD4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020C08);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020C6C);

s32 func_80020CE0(s32 a0) {
    extern u8 D_80078E00[];
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)D_80078E00;
        s32 idx = a0 - 0x21;
        param = *(u16 *)(base + idx * 4 + 0x3C48);
        arg2 = *(s32 *)(base + 0xA0);
    } else {
        s32 base = (s32)D_80078E00;
        param = *(u16 *)(base + a0 * 24 + 0x3930);
        arg2 = *(s32 *)(base + 0x9C);
    }
    return func_80020FBC(param, arg2);
}

s32 func_80020D4C(s32 a0) {
    extern u8 D_80078E00[];
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)D_80078E00;
        s32 idx = a0 - 0x21;
        param = *(u16 *)(base + idx * 4 + 0x3C4A);
        arg2 = *(s32 *)(base + 0xA0);
    } else {
        s32 base = (s32)D_80078E00;
        param = *(u16 *)(base + a0 * 24 + 0x3932);
        arg2 = *(s32 *)(base + 0x9C);
    }
    return func_80020FBC(param, arg2);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020DB8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020E4C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020EB4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020EF4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020F2C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020F64);

s32 func_80020F84(s32 a0) {
    extern u8 D_80078E00[];
    u8 *base = D_80078E00;
    return func_80020FBC(*(u16 *)(base + a0 * 2 + 0x4D08), *(s32 *)(base + 0xE0));
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020FBC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80020FEC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021034);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021108);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800211B4);

s32 func_80021290(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 D_80077378[];
    s32 base;
    u8 slot_id;
    s32 i;
    s32 ptr;

    if (a1 == 0) return 0;

    base = (s32)D_80077378;
    slot_id = *(u8 *)(a0 + base + 0xAF4);
    i = 0;
    ptr = base + slot_id * 152;
    while (i < 20) {
        if (*(u8 *)(ptr + i + 0x4EC) == a1) {
            return 1;
        }
        i++;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021300);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021358);

void func_800214E0(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    u8 *base = D_80077378 + a0;
    u8 idx = base[0xAF4];
    u8 *ptr = D_80077378 + 0x490 + idx * 152;
    *(s16 *)(ptr + 2) = func_800231C8(*(u16 *)(ptr + 2) + a1);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002153C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021628);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002166C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800216B0);

s32 func_8002172C(s32 a0, s32 a1) {
    extern u8 D_80078E00[];
    s32 base = (s32)D_80078E00;
    s32 entry = base + a1 * 132;
    u8 field1 = *(u8 *)(entry + 0xF8D);
    u8 field0 = *(u8 *)(entry + 0xF8C);
    u8 field2 = *(u8 *)(entry + 0xF8E);
    return (s16)(a0 * field0 + a0 * a0 * 10 / field1 + field2);
}

s32 func_8002178C(s32 a0, s32 a1) {
    extern u8 D_80078720[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80078720;
    s32 entry = base1 + a0 * 464;
    s32 base2;
    u8 idx;
    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x1C3);
    return func_80021628(a1, *(u8 *)(base2 + idx * 36 + 0x37AA), *(u8 *)(base2 + idx * 36 + 0x37AB));
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800217F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021894);

s32 func_80021944(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 D_80077378[];
    s32 i = 0;
    s32 base;

    if (a1 == 0) return 0;

    base = (s32)D_80077378;
    a0 = a0 * 152;
    while (i < 32) {
        u8 *ptr = (u8 *)(a0 + base);
        if (ptr[0x4A0] == a1) {
            return ptr[0x4A1];
        }
        a0 += 2;
        i++;
    }
    return 0;
}

// multiply
s32 func_800219A8(s32 a0, s32 a1) {
    return a0 * a1;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800219B8);

s32 func_800219E0(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 result = 100;
    s32 i = 0;
    s32 base1 = (s32)D_80077378;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x27;
        if ((u32)idx < 0x13) {
            s32 entry = idx * 8 + base2;
            if (*(u8 *)(entry + 0x421D) == a1) {
                result += *(u8 *)(entry + 0x421E);
            }
        }
        i++;
    } while (i < 4);
    return result;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021A64);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021B58);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021C10);

s32 func_80022028(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;
    s32 result1;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F3);
    val = *(u8 *)(base2 + idx * 60 + 0x23A);

    result1 = func_80021B58(a0, a1);
    return func_800231B0(*(u8 *)(base2 + result1 * 12 + 0x35BF) + func_800219B8(val, func_80021944(a0, idx)));
}

s32 func_800220E4(s32 a0, s32 a1) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F2);
    val = *(u8 *)(base2 + idx * 60 + 0x239);

    return func_800231B0((a1 >> 2) + func_800219B8(val, func_80021944(a0, idx)));
}

s32 func_8002216C(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    u8 idx = *(u8 *)(base1 + a0 * 152 + 0x4F5);
    s32 base2 = (s32)D_80078E00;
    return *(u8 *)(base2 + idx * 60 + 0x23C);
}

s32 func_800221B4(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F5);
    val = *(u8 *)(base2 + idx * 60 + 0x23D);

    return func_800219B8(val, func_80021944(a0, idx));
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022228);

s32 func_80022328(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    u8 idx = *(u8 *)(base1 + a0 * 152 + 0x4F6);
    s32 base2 = (s32)D_80078E00;
    return *(u16 *)(base2 + idx * 60 + 0x242) & 0x7F;
}

s32 func_80022370(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 base2;
    u8 idx;
    u16 flags;
    s32 val;
    s32 result;

    idx = *(u8 *)(base1 + a0 * 152 + 0x4F6);
    base2 = (s32)D_80078E00;
    flags = *(u16 *)(base2 + idx * 60 + 0x242);
    val = flags & 0x80;
    result = val != 0;
    if (flags & 0x100) result |= 0x4;
    if (flags & 0x200) result |= 0x8;
    if (flags & 0x400) result |= 0x200;
    if (flags & 0x800) result |= 0x4000;
    if (flags & 0x1000) result |= 0x8000;
    return result;
}

s32 func_80022404(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 base1 = (s32)D_80077378;
    s32 entry = base1 + a0 * 152;
    s32 base2;
    u8 idx;
    u8 val;

    base2 = (s32)D_80078E00;
    idx = *(u8 *)(entry + 0x4F6);
    val = *(u8 *)(base2 + idx * 60 + 0x240);

    return func_800219B8(val, func_80021944(a0, idx)) + 100;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002247C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002257C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002274C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800227F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800228F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002293C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800229FC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022B04);

s32 func_80022B48(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    s32 result = 0;
    s32 i = 0;
    s32 base1 = (s32)D_80077378;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x3A;
        if ((u32)idx < 0x14) {
            s32 entry = idx * 8 + base2;
            u8 b = *(u8 *)(entry + 0x42B7);
            u8 g = *(u8 *)(entry + 0x42B6);
            u8 r = *(u8 *)(entry + 0x42B5);
            result |= (b << 16) | (g << 8) | r;
        }
        i++;
    } while (i < 4);
    return result;
}

s32 func_80022BD0(u8 *a0) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == 6) return 1;
        a0 += 4;
    }
    return 0;
}

s32 func_80022C04(s32 a0) {
    extern u8 D_80082C10;
    s32 val = *(s32 *)(a0 + 0x190);
    s32 masked = val & 0x20000;
    s32 flag = masked != 0;
    if (func_80022BD0(a0)) {
        if (D_80082C10 & 8) {
            return flag;
        }
        flag |= 2;
    }
    return flag;
}

void func_80022C5C(s32 a0) {
    extern u8 D_80077378[];
    extern u8 D_80078E00[];
    extern u8 D_80078720[];
    s32 i = 0;
    s32 base1 = (s32)D_80077378;
    s32 base3;
    s32 base2;
    s32 off = base1 + a0 * 152;
    base3 = (s32)D_80078720;
    base2 = (s32)D_80078E00;
    do {
        s32 val = *(u8 *)(off + i + 0x4E4);
        s32 idx = val - 0x4E;
        if ((u32)idx < 5) {
            *(u8 *)(base3 + 0x6D8) |= *(u8 *)(idx * 8 + base2 + 0x4355);
        }
        i++;
    } while (i < 4);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022CDC);

void func_80022D78(s32 a0) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        *(u8 *)(a0 + i * 5 + 0x84) = 0;
        *(u8 *)(a0 + i * 5 + 0x85) = 0;
        *(u8 *)(a0 + i * 5 + 0x86) = 0;
        *(u8 *)(a0 + i * 5 + 0x83) = 0;
        *(u8 *)(a0 + i * 5 + 0x82) = 0;
    }

    for (i = 0; i < 0x10; i++) {
        *(u8 *)(a0 + i * 5 + 0x124) = 0;
        *(u8 *)(a0 + i * 5 + 0x125) = 0;
        *(u8 *)(a0 + i * 5 + 0x126) = 0;
        *(u8 *)(a0 + i * 5 + 0x123) = 0;
        *(u8 *)(a0 + i * 5 + 0x122) = 0;
    }

    for (i = 0; i < 4; i++) {
        *(u8 *)(a0 + i * 4 + 0x20) = 0;
        *(u8 *)(a0 + i * 4 + 0x21) = 0;
        *(u8 *)(a0 + i * 4 + 0x1F) = 0;
        *(u8 *)(a0 + i * 4 + 0x1E) = 0;
    }

    *(u8 *)(a0 + 0x1C) = 0;
    *(u8 *)(a0 + 0x1D) = 0;
    *(u16 *)(a0 + 0x14) = 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80022E08);

s32 func_80023180(u8 *a0, s32 a1) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (a0[0x1E] == a1) return i;
        a0 += 4;
    }
    return 0xFF;
}

// clamp_to_byte - clamps value to 0x00-0xFF range
s32 func_800231B0(s32 a0) {
    if (a0 >= 0x100) {
        return 0xFF;
    }
    return a0;
}

s32 func_800231C8(s32 a0) {
    if (a0 >= 0x2710) {
        return 0x270F;
    }
    return (u16)a0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800231E0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002363C);

void func_80023828(void) {
    extern u8 D_80077378[];
    s32 i = 0;
    u8 *ptr = D_80077378;
    do {
        if (ptr[0x61] & 1) {
            func_8002363C(i);
        }
        i++;
        ptr += 0x44;
    } while (i < 16);
}

void func_80023888(void) {
    extern u8 D_80078DF8;
    extern u8 D_80077378[];
    s32 i;
    s32 base;
    s32 ptr;

    D_80078DF8 = 0;
    i = 0;
    base = (s32)D_80077378;
    do {
        ptr = i + base;
        func_80022E08(*(u8 *)(ptr + 0xAF4), i);
        func_800231E0(*(u8 *)(ptr + 0xAF4), i);
        i++;
    } while (i < 3);
    func_80023828();
}

extern u8 D_80078658;

// get pointer to D_80078658
u8 *func_80023900(void) {
    return &D_80078658;
}

s32 func_8002390C(a0, a1)

s32 a0;
s32 a1;
{
    u8 *base = func_80023900();
    u8 *ptr;
    u8 val;

    if (a0 < 0x4D) {
        ptr = &base[a0];
        if (a1 == 0xF0) {
            val = *ptr;
            if ((val & 0x7F) < 100) {
                *ptr = val + 1;
            } else {
                return -1;
            }
        } else {
            val = *ptr;
            if (val & 0x7F) {
                *ptr = val - 1;
            } else {
                return -1;
            }
        }
    } else {
        base[a0] = (u8)a1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800239A8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023A34);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023A54);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023A88);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023B14);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023BB4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023C08);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023C48);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023D04);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80023D60);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80024064);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800242C8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025920);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800259C0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025D80);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025E4C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80025F58);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800262B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800263DC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026ADC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026CA0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026CF0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026D10);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026D8C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026E20);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026E70);

s32 func_80026EC4(s32 a0, s32 a1) {
    s32 result;
    func_800472E4(a0);
    result = OpenTh(a0, a1, 0);
    func_800472F4();
    return result;
}

void func_80026F14(s32 a0) {
    func_800472E4(a0);
    CloseTh(a0);
    func_800472F4();
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026F4C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026F90);

void func_80026FB4(void) { func_80047384(); }

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80026FD4);

void func_80026FE0(s32 a0) {
    if (a0 == 0) {
        a0 = (s32)0xFF000000;
    }
    if (func_80047384() & 4) {
        func_80026F4C(a0);
    } else {
        ChangeTh(a0);
    }
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027038);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800270B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027220);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027360);

extern u8 D_80082DD0[];

void func_800273D8(s32 idx) {
    u8 *entry = D_80082DD0 + idx * 196;
    entry[0x19] = 1;
    entry[0x0A] = 1;
}

void func_80027408(s32 a0) {
    func_80027220(a0, a0, 0);
    func_80027220(a0, a0 + 0xC4, 0x10);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027448);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027558);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800275A8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800275D4);

u16 func_8002795C(s32 a0, s32 a1) {
    u8 *base;
    u8 *entry;
    u8 *entry2;
    s32 sub_idx;
    a0 &= 1;
    base = D_80082DD0;
    entry = base + a0 * 196;
    entry2 = base + entry[0xC2] * 196;
    sub_idx = (entry2[0x18] - a1) & 7;
    return *(u16 *)(entry2 + sub_idx * 20 + 0x1E);
}

u16 func_800279CC(s32 a0, s32 a1) {
    u8 *base;
    u8 *entry;
    u8 *entry2;
    s32 sub_idx;
    s32 off;
    u8 *p;
    a0 &= 1;
    base = D_80082DD0;
    entry = base + a0 * 196;
    entry2 = base + entry[0xC2] * 196;
    sub_idx = (entry2[0x18] - a1) & 7;
    off = sub_idx * 20 + 0x1C;
    p = entry2 + off;
    return *(u16 *)(p + 8) | *(u16 *)(p + 0xA) | *(u16 *)(p + 0xC) | *(u16 *)(p + 0xE);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027A58);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027AC8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027B38);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027B7C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027BA8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027C00);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027C90);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027CF8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027DB4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027EC8);

void func_80027EF8(s32 a0, s32 a1) {
    extern u8 D_80082DD0[];
    u8 *entry = D_80082DD0 + (a0 & 1) * 196;
    if (a1 != 0) {
        entry[0x1B] = 0xFF;
    } else {
        entry[0x1B] = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027F38);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027F78);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027FBC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80027FDC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028098);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800280C0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800281A4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800281C4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002828C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800282F4);

extern u16 D_80083794;
// get D_80083794 (u16)

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002837C);

u16 func_800283CC(void) {
    return D_80083794;
}

extern u8 D_80082FB3;

s32 func_800283DC(s32 a0) {
    D_80083794 = a0;
    return a0;
}

void func_800283EC(s32 a0) {
    D_80082FB3 = a0;
}

void func_800283F8(void) {
    extern u8 D_80082DD0[];
    u8 *base = D_80082DD0;
    func_800982B8();
    func_8003BC24(base + 0x188, base + 0x1AC);
    func_8003AB84();
    func_800282F4();
    *(s16 *)(base + 0x9C4) = 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028444);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002848C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800284BC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028564);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800286BC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800286DC);

void func_800286FC(void) { func_8004DFF4(); }

// strcpy
void func_8002871C(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}

void func_80028738(u8 *dst, u8 *src) {
    do { } while (*dst++);
    func_8002871C(dst - 1, src);
}

// copy_bytes
void func_80028768(u8 *src, u8 *dst, s32 numBytes) {
    while (numBytes > 0) {
        *dst++ = *src++;
        numBytes--;
    }
}

void func_80028790(s32 a0) { VSync(); }

extern s8 D_80082FD4;
// set D_80082FD4 (s8)
void func_800287B0(s8 val) {
    D_80082FD4 = val;
}

s32 func_800287BC(s32 a0, s32 a1) {
    a1 %= 4;
    a0 %= 2;
    return (a0 << 4) | a1;
}

// get_bit4 - extracts bit 4 as 0 or 1
s32 func_800287F4(s32 a0) {
    return (a0 >> 4) % 2;
}

// mod4
s32 func_80028810(s32 a0) {
    return a0 % 4;
}

void func_8002882C(s32 a0) {
    _card_wait(func_800287F4(a0));
}

s32 func_80028854(s32 a0) { TestEvent(a0); }

s32 func_80028874(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (func_80028854(s0[0])) return 0;
    if (func_80028854(s0[1])) return 1;
    if (func_80028854(s0[2])) return 2;
    if (func_80028854(s0[3])) return 3;
    return -1;
}

s32 func_800288F8(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0xB4; i++) {
        result = func_80028874();
        if (result != -1) return result;
        func_80028790(0);
    }
    return 2;
}

void func_80028950(void) { func_80028874(); }

s32 func_80028970(void) {
    extern u8 D_80082FB4[];
    s32 *s0 = (s32 *)((s32)D_80082FB4);
    if (TestEvent(s0[4])) return 0;
    if (TestEvent(s0[5])) return 1;
    if (TestEvent(s0[6])) return 2;
    if (TestEvent(s0[7])) return 3;
    return -1;
}

s32 func_800289F8(void) {
    s32 i;
    s32 result;
    for (i = 0; i < 0x4000; i++) {
        result = func_80028970();
        if (result != -1) return result;
    }
    return 2;
}

u8 func_80028A44(a0)

s32 a0;
{
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    return *(u8 *)(r1 * 4 + base + r2 + 0x2C);
}

void func_80028A98(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    *(u8 *)(r1 * 4 + base + r2 + 0x2C) = a1;
}

void func_80028AF8(s32 a0, u8 a1) {
    extern u8 D_80082FB4[];
    s32 base = (s32)D_80082FB4;
    s32 r1 = func_800287F4(a0);
    s32 r2 = func_80028810(a0);
    *(u8 *)(r1 * 4 + base + r2 + 0x34) = a1;
}

void func_80028B58(s32 a0) { func_80028A98(a0, 1); }

void func_80028B78(void) { func_80028A44(); }

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028B98);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028CB4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028D20);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80028D80);

s32 func_80028F3C(s32 a0) {
    s32 counter;

    counter = 0;
    do {
        func_8002882C(a0);
        if (_card_load(a0) != 0) {
            s32 result = func_800288F8();
            switch (result) {
            case 0:
                func_80028AF8(a0, 0);
                func_80028A98(a0, 0);
                return 0;
            case 3:
                func_80028AF8(a0, 1);
                func_80028B58(a0);
                return 2;
            case 2:
                func_80028B58(a0);
                return 3;
            default:
                func_80028B58(a0);
                return 4;
            }
        }
        counter++;
    } while (counter < 0xB4);
    return 4;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029028);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800290C0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029150);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800291FC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800292AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029360);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800293F0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029400);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002947C);

void func_8002949C(s32 a0) {
    if (a0 != -1) {
        close(a0);
    }
}

void func_800294C4(s32 a0) {
    func_8002949C(a0);
}

s32 func_800294E4(s32 a0, s32 a1, s32 a2) {
    s32 buf[8];
    func_80028CB4();
    if (func_80029150(a0) != 0) return 0;
    func_80029360(a0, a1, (s32)buf);
    firstfile((s32)buf, a2);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029550);

void func_800295D0(void) { nextfile(); }

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800295F0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029660);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029680);

s32 func_80029778(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return write(a0, a1, a2);
}

s32 func_800297E4(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 result = lseek(a0, a3, 0);
    if (result == -1) return -1;
    return read(a0, a1, a2);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029850);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029A20);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029BA0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029C44);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029CB8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029D38);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029DAC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029E40);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029EE4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029F5C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80029FDC);

void func_8002A090(void) {
    extern s32 D_80082FB4[];
    func_800472E4();
    CloseEvent(D_80082FB4[0]);
    CloseEvent(D_80082FB4[1]);
    CloseEvent(D_80082FB4[2]);
    CloseEvent(D_80082FB4[3]);
    CloseEvent(D_80082FB4[4]);
    CloseEvent(D_80082FB4[5]);
    CloseEvent(D_80082FB4[6]);
    CloseEvent(D_80082FB4[7]);
    func_800472F4();
    func_8004D968();
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A128);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A150);

s32 func_8002A238(s32 a0, s32 a1) {
    extern u8 D_80052958;
    extern u8 D_800837A0[];
    extern s16 D_800837AC;
    extern u8 D_800837AE;
    if (D_80052958 != 0) {
        s32 result = func_8002E8DC(a0, a1, D_800837AC, D_800837AE, (s32)D_800837A0, 7);
        a1 = func_8002A45C(a0, result);
    }
    return a1;
}

// strcpy
void func_8002A2A8(u8 *dst, u8 *src) {
    s32 c;
    do {
        c = *src++;
        *dst++ = c;
    } while (c != 0);
}

// strcat
void func_8002A2C4(u8 *dst, u8 *src) {
    do { } while (*dst++);
    func_8002A2A8(dst - 1, src);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A2F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A318);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A340);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A36C);

void func_8002A3A8(void) { func_80020E4C(); }

void func_8002A3C8(void) { func_80020E4C(); }

void func_8002A3E8(void) { func_80020DB8(); }

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A408);

extern u8 *D_8005F134;
// copy_display_coords - copies x,y from D_8005F134 offsets 8/A
void func_8002A438(u8 *a0) {
    u8 *p = D_8005F134;
    *(u16 *)(a0 + 0) = *(u16 *)(p + 8);
    *(u16 *)(a0 + 2) = *(u16 *)(p + 0xA);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A45C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A528);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A588);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A5E8);

extern s32 D_800834C0;

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A834);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A868);

s32 func_8002A888(void) {
    return *(s32 *)D_800834C0;
}

s32 func_8002A8A0(void) {
    return *(s32 *)(D_800834C0 + 0x54);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A8B8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A91C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002A92C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AA18);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AAC0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AB5C);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002ACD8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002ACF4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AD04);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002AD3C);

// battle_entity_get_field_38 - D_80083210 stride 64

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002ADA4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002ADDC);

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

void func_8002AE90(s32 a0, s32 a1) {
    u8 *p = D_80083210 + a0 * 64;
    p[0x36] = a1;
}

u32 func_8002AEA8(s32 a0) {
    u8 *p = D_80083210 + a0 * 64;
    return p[0x36];
}

void func_8002AEC4(s32 a0, s32 a1) {
    u8 *p = D_80083210 + a0 * 64;
    p[0x35] = a1;
}

u32 func_8002AEDC(s32 a0) {
    u8 *p = D_80083210 + a0 * 64;
    return p[0x35];
}

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

void func_8002AF70(s32 a0, s32 a1) {
    u8 *p = D_80083210 + a0 * 64;
    *(s16 *)(p + 0x3C) = a1;
}

s32 func_8002AF88(s32 a0) {
    u8 *p = D_80083210 + a0 * 64;
    return *(s16 *)(p + 0x3C);
}

void func_8002AFA4(s32 a0) {
    s16 rect[4];
    s32 i;
    rect[0] = 0x40;
    rect[1] = 0x40;
    rect[2] = 0x80;
    rect[3] = 0x80;
    func_8002AD04(a0, rect);
    func_8002AD3C(a0, rect);
    func_8002AE30(a0, 6);
    func_8002AE78(a0, 0);
    func_8002AE60(a0, 0);
    func_8002AEF8(a0, 0);
    func_8002AC88(a0, 3);
    for (i = 0; i < 2; i++) {
        func_8002ACD8(a0, i, 0);
    }
    func_8002ACD8(a0, 1, a0);
    func_8002AF70(a0, 0x1000);
    func_8002AE90(a0, 0);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002B080);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002B16C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002B3A0);

s32 func_8002B898(s32 a0, s32 a1, s32 a2, s32 a3) {
    return func_8002B3A0(a0, a1, a2, a3, 3);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002B8BC);

INCLUDE_ASM("asm/nonmatchings/10DD0", __udivdi3);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002BAA0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002BC10);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002BC6C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002BE48);

s32 func_8002BEEC(s32 a0) {
    extern s32 D_800834C0;
    s32 base = D_800834C0 + 8;
    s32 result = func_8002ACBC(a0);
    return base + (result << 2);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002BF24);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C030);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C070);

void func_8002C0C4(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_8002AFA4(i);
    }
}

extern s32 D_800834CC;

void func_8002C100(s32 a0) {
    D_800834CC = a0;
}

s32 func_8002C10C(void) {
    return 8;
}

extern u8 D_80083840;
// get D_80083840 (u8)
u8 func_8002C114(void) {
    return D_80083840;
}

// set D_80083840 (u8)
void func_8002C124(u8 val) {
    D_80083840 = val;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C130);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C3AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C56C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C734);

extern u8 D_80082FF0[];
// sfx_entry_set_fields_29_2A_2C - D_80082FF0 stride 60

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C7BC);

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

void func_8002C868(s32 a0, s32 a1) {
    extern u8 D_80082FF0[];
    u8 *entry = D_80082FF0 + a0 * 60;
    *(s16 *)(entry + 0x1A) = a1;
    func_8002AF70(entry[0x18], a1);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C8A4);

void func_8002C920(void) {
    extern u8 D_80082FF0[];
    D_80082FF0[0x1E3]--;
    func_8002C8A4();
}

void func_8002C954(s32 a0) {
    extern s32 D_800831D4;
    a0 /= 32;
    a0 &= 0xFF;
    a0 |= (a0 << 16) | (a0 << 8);
    a0 |= 0x64000000;
    D_800831D4 = a0;
    func_8002C8A4();
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C9A4);

// sfx_entry_swap_field_16 - D_80082FF0 stride 60, returns old value

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002C9F0);

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

void func_8002CA9C(s32 a0, s32 a1) {
    u8 *p = D_80082FF0 + a0 * 60;
    *(s16 *)(p + 0x1E) = a1;
}

s32 func_8002CABC(s32 a0) {
    u8 *p = D_80082FF0 + a0 * 60;
    return *(s16 *)(p + 0x1C);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CAE0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CC4C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CDE4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CE68);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CE74);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CE84);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CECC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002CF54);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D040);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D6AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D744);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D784);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D818);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D8CC);

// sfx_entry_init_fields_16_1E_2D - D_80082FF0 stride 60

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002D970);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002DBF8);

void func_8002DCA4(s32 a0, s32 a1, s32 a2) {
    u8 *p = D_80082FF0 + a0 * 60;
    p[0x16] = 1;
    *(s16 *)(p + 0x1E) = a1;
    p[0x2D] = a2;
}

void func_8002DCD0(s32 a0) {
    func_8002DCA4(a0, 0x200, 0);
}

void func_8002DCF4(s32 a0) {
    func_8002DCA4(a0, 0x1000, 0);
}

void func_8002DD18(s32 a0, s32 a1) {
    func_8002CA9C(a0, a1);
}

void func_8002DD38(s32 a0) {
    func_8002DD18(a0, -0x200);
}

void func_8002DD58(s32 a0) {
    func_8002DD18(a0, -0x1000);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002DD78);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002DDD8);

void func_8002DDFC(s32 a0, s32 a1) {
    extern u8 D_80082FF0[];
    u8 *entry = D_80082FF0 + a0 * 60;
    func_8002AE30(entry[0x18], a1 | 8);
}

void func_8002DE38(s32 a0) {
    extern u8 D_80082FF0[];
    u8 *entry = D_80082FF0 + a0 * 60;
    func_8002AE14(entry[0x18]);
}

// sfx_entry_set_field_2F - D_80082FF0 stride 60
void func_8002DE74(s32 idx, s32 val) {
    u8 *p = D_80082FF0 + idx * 60;
    p[0x2F] = val;
}

// init_sfx_entity_slot - zeroes struct fields, calls 9 setup functions with defaults
void func_8002DE94(a0)

s32 a0;
{
    s32 a1 = 0;
    u8 *entry = D_80082FF0 + a0 * 60;
    *(s16 *)(entry + 0x14) = 0;
    *(u8 *)(entry + 0x19) = 0;
    *(u8 *)(entry + 0x2F) = 0;
    func_8002D6AC(a0, a1);
    func_8002CA58(a0, 0x1000);
    func_8002CA10(a0, 0);
    func_8002DD78(a0, 3);
    func_8002CA7C(a0, 0);
    func_8002CA9C(a0, 0);
    func_8002DDFC(a0, 6);
    {
        s16 buf[4];
        buf[0] = 0x40;
        buf[1] = 0x40;
        buf[2] = 0x80;
        buf[3] = 0x80;
        func_8002E064(a0, buf);
    }
    func_8002C868(a0, 0x1000);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002DF5C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E028);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E064);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E1B4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E1E8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E254);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E298);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E3A4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E428);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E454);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E4AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E680);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E744);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E764);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E784);

u16 func_8002E7A4(s32 a0) {
    return (u16)func_8002E4AC(a0, 0);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E7C4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E810);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002E8DC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002EAD0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002EE10);

void func_8002F23C(u32 a0, u8 *a1, s32 a2) {
    extern u32 D_800529F4[];
    u32 *t = D_800529F4;
    u32 d = *t++;

    while (d != 0) {
        s32 digit = 0;
        while (a0 >= d) {
            a0 -= d;
            digit++;
        }
        digit += a2;
        *a1++ = digit;
        d = *t++;
    }
    *a1 = 0;
}

void func_8002F294(u32 a0, u8 *a1, s32 a2) {
    extern u32 D_80052A08[];
    u32 *t = D_80052A08;
    u32 d = *t++;

    while (d != 0) {
        s32 digit = 0;
        while (a0 >= d) {
            a0 -= d;
            digit++;
        }
        digit += a2;
        *a1++ = digit;
        d = *t++;
    }
    *a1 = 0;
}

void func_8002F2EC(u8 *a0, s32 a1, s32 a2, s32 a3) {
    s32 i;
    for (i = 0; i < a1; i++) {
        if (*a0 != a2) return;
        *a0++ = a3;
    }
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F320);

extern u8 D_80052A20[];
// lookup_hex_char - D_80052A20 is "0123456789ABCDEF" (source: strings.csv 0x80052A20)
void func_8002F384(s32 idx, u8 *dst) {
    u8 *base = D_80052A20;
    *dst = base[idx & 0xF];
}

void func_8002F3A0(s32 byte, u8 *buf) {
    func_8002F384(byte >> 4, buf++);
    func_8002F384(byte & 0xF, buf++);
    *buf = 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F3F0);

// u32_to_hex_string - converts u32 to 8-char hex string with configurable base char

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F43C);

void func_8002F488(u32 val, u8 *dst, s32 base_char) {
    s32 shift = 28;
    do {
        *dst++ = ((val >> shift) & 0xF) + base_char;
        shift -= 4;
    } while (shift >= 0);
    *dst = 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F4B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F548);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F5B4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F610);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002F688);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002FD28);

void func_8002FD9C(s32 *a0, s32 a1) {
    s32 result = func_8002F548(a0[3]);
    a0[3] = result;
    func_8002F688(result, a1, -1);
}

void func_8002FDE8(s32 *a0, s32 a1) {
    func_8002F688(a0[3], a1, -1);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002FE0C);

// clear_rgb_fields - zeros bytes at offsets 0x20, 0x21, 0x22
void func_8002FF24(u8 *a0) {
    a0[0x20] = 0;
    a0[0x22] = 0;
    a0[0x21] = 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8002FF34);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030058);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030094);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800300F8);

void func_80030214(void) { func_80030058(0x1000); }

// empty stub
void func_80030234(void) {
}

extern s16 D_800834D4;
// set D_800834D4 (s16)
void func_8003023C(s32 val) {
    D_800834D4 = val;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030248);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030274);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030288);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800302DC);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030518);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030754);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800307F8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030848);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800308B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003093C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030A54);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030B2C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030CB0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030CFC);

void func_80030D48(s32 a0) {
    extern u8 D_80052A34[];
    func_8001313C(D_80052A34[a0]);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030D78);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030DB0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030E60);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030F10);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030FA0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80030FDC);

// empty stub
void func_80031044(void) {
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003104C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031188);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031224);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031364);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800316C4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800316D4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800318E0);

void func_800318EC(void) {
    D_80083754.f0 = 9;
    D_80083754.f4 = 0;
    D_80083754.f6 = 0;
    D_80083754.f8 = 0;
    D_80083754.f9 = 0;
}

s32 func_80031910(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a2 < a0) {
        return 0;
    }
    if (a2 >= a1) {
        return a3;
    }
    a2 -= a0;
    a1 -= a0;
    return a2 * a3 / a1;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031954);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031A18);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031CDC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031D68);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031D8C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031DF4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031E1C);

extern u8 D_80083772[];
// clear_animation_entries

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031EBC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80031EF4);

void func_80031F2C(void) {
    s32 i;
    u8 *ptr = D_80083772;
    for (i = 1; i >= 0; i--) {
        ptr[0xE] = 0;
        ptr += 0x10;
    }
}

extern u8 D_80085134[];
extern u8 D_80083938[];

u8 *func_80031F50(void) {
    return D_80085134;
}

u8 *func_80031F5C(void) {
    return D_80083938;
}

void func_80031F68(void) {
    do {
    } while (func_8004D208(1) == -1);
}

// get_battle_alloc_base - returns 0x801F4000
u32 func_80031F9C(void) {
    return 0x801F4000;
}

// get_battle_alloc_size - returns 0x4000 (16KB)
s32 func_80031FA8(void) {
    return 0x4000;
}

// double-buffer flip: switch OT buffer, clear tag, init linked list
void func_80031FB0(void) {
    extern s32 D_80083918;
    extern s32 D_80083920[];
    s32 buf;
    s32 ptr;

    buf = D_80083920[0];
    if (D_80083918 == buf) {
        buf = D_80083920[1];
    }
    D_80083918 = buf;
    ClearOTag((u32 *)(buf + 0x70), 2);
    ptr = D_80083918;
    *(s32 *)(ptr + 0x78) = ptr + 0x7C;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80032010);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800320BC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80032204);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003228C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80032350);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80032534);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80032688);

s32 func_800327B8(s32 a0, s32 retries) {
    s32 result = 0;
    a0 &= 0xFF;
    while (retries > 0) {
        result = func_800239A8(a0);
        if (result != 0) {
            break;
        }
        retries--;
    }
    return result;
}

s32 func_80032808(s32 a0) {
    if (a0 & 0x100) return 0;
    return func_80020D4C(a0);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003283C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800330F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033298);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800332C4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003331C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003334C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033380);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003346C);

void func_80033534(s32 a0, s32 a1, u16 *a2) {
    s32 save0 = *(s32*)a2;
    s32 save1 = *(s32*)(a2 + 2);
    a2[0] += 1;
    a2[1] += 1;
    a2[2] -= 2;
    a2[3] -= 2;
    func_8003346C(a0, a1, a2);
    *(s32*)a2 = save0;
    *(s32*)(a2 + 2) = save1;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800335AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033688);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033768);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800337FC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033A28);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033C7C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033D5C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80033F1C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003406C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800341BC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003431C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80034830);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800348C4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800349F4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80034C74);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80034DBC);

void func_80035118(void) {
    SetDispMask(1);
    func_80034DBC();
    func_8003283C();
}

extern u8 D_80083928;
// get D_80083928 (u8)
u8 func_80035148(void) {
    return D_80083928;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035158);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035294);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800352BC);

void func_800352EC(void) {
    short rect[4];
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x180;
    rect[3] = 0xE0;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
    rect[0] = 0x200;
    rect[1] = 0;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035360);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035A6C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035AA4);

extern u8 D_80085138;
// set D_80085138 (u8)

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035AE4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035B28);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035B70);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035BB4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035BC0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035BCC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035C10);

void func_80035C54(u8 val) {
    D_80085138 = val;
}

// get D_80085138 (u8)
u8 func_80035C60(void) {
    return D_80085138;
}

extern s32 D_8008513C;

void func_80035C70(s32 a0) {
    D_8008513C |= (1 << a0);
}

void func_80035C8C(s32 a0) {
    D_8008513C |= (a0 << 16);
    func_80035C70(2);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", SsUtReverbOff);

void func_80035CE0(void) {
}

void func_80035CE8(void) {
}

void func_80035CF0(void) { func_8003882C(); }

void func_80035D10(void) { func_80038868(); }

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035D30);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035E68);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035E8C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80035F70);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003631C);

void func_80036444(void) {
    func_80036104(0, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003646C);

// init_card_hand_slots - initializes 128 card slots (id=0, type=0xFF)

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036690);

void func_800366E8(u8 *ptr) {
    s32 i = 0;
    do {
        i++;
        ptr[1] = 0xFF;
        ptr[0] = 0;
        ptr += 2;
    } while (i < 128);
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036710);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003678C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003685C);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800369CC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036B90);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036C74);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036D44);

void func_80036E8C(s32 a0) {
    extern u8 D_80077378[];
    D_80077378[0xAF4] = a0;
    D_80077378[0xAF5] = 0xFF;
    D_80077378[0xAF6] = 0xFF;
    func_80023888();
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036EC0);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80036FE0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800370AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037198);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800371D0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003720C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037240);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800372D0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800372DC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037308);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800375A0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037678);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800376A8);

// mc_xor_checksum

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003777C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800377B4);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003798C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800379AC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037ACC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037ADC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037AEC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037B44);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037B7C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037BB0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037BF0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037C30);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037C6C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037CD4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037D40);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037E14);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037E60);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037F08);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80037FB0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038030);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800381BC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038308);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038364);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003837C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800383B8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038424);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038440);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038464);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038490);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800385B0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003863C);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800386F0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038708);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038720);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038760);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800387F8);

s32 func_8003882C(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_80038760(3, a0, a1, a2, a3);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038868);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800388CC);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800389CC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038A18);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038A60);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038D3C);

void func_80038D74(void) {
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038D7C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038DD4);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038ED4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80038F88);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039040);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039140);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039218);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039344);

void func_80039388(void) {
    extern u8 D_8008A3D9;
    if (CdSync(1, 0) == 2) {
        CdReadBreak();
        D_8008A3D9 = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800393C8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039444);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003947C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800394D0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039520);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039678);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800396E0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039728);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003974C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039764);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003987C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_800398A8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039910);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039AA0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039AB4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039B80);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039BC4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039EF8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_80039FE8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A1F8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A470);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A500);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A534);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A57C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A654);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A700);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003A7C4);

void func_8003AB64(void) { func_80039AA0(); }

void func_8003AB84(void) { func_80039AB4(); }

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003ABA4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003ABC4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003AC10);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003ACDC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003ADD4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003AEA8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003AF50);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003AF88);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003AFD0);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B0C4);

s32 func_8003B334(u8 *a0) {
    s32 v0 = a0[0xE3];
    s32 a1val = a0[0xE9];
    u16 ecval = *(u16 *)(a0 + 0xEC);
    s32 part1 = ((v0 + 1) >> 1) << 2;
    s32 part2 = ((a1val * 5 + 3) & 0xFFC) + 4;
    return part1 + part2 + ecval;
}

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B36C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B440);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B4E8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B8E0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B948);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003B964);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BA2C);

// snd_voice_cmd_play_note - cmd=0x43, sets payload byte

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BAC4);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BB18);

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

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BC0C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BC24);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BD84);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BDEC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BEE0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BEF0);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003BFAC);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C228);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C260);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C284);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C2B8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C3C8);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C62C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C70C);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C744);

INCLUDE_ASM("asm/nonmatchings/10DD0", func_8003C764);
