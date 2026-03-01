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
