#include "common.h"


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BAA0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BC10);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BC6C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BE48);


s32 func_8002BEEC(s32 a0) {
    extern s32 D_800834C0;
    s32 base = D_800834C0 + 8;
    s32 result = func_8002ACBC(a0);
    return base + (result << 2);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002BF24);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C030);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C070);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C130);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C3AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C56C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C734);


extern u8 D_80082FF0[];
// sfx_entry_set_fields_29_2A_2C - D_80082FF0 stride 60

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C7BC);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C8A4);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C9A4);


// sfx_entry_swap_field_16 - D_80082FF0 stride 60, returns old value

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002C9F0);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CAE0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CC4C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CDE4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CE68);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CE74);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CE84);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CECC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002CF54);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D040);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D6AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D744);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D784);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D818);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D8CC);


// sfx_entry_init_fields_16_1E_2D - D_80082FF0 stride 60

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002D970);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DBF8);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DD78);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DDD8);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002DF5C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E028);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E064);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E1B4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E1E8);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E254);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E298);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E3A4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E428);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E454);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E4AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E680);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E744);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E764);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E784);


u16 func_8002E7A4(s32 a0) {
    return (u16)func_8002E4AC(a0, 0);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E7C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E810);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002E8DC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002EAD0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002EE10);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F320);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F3F0);


// u32_to_hex_string - converts u32 to 8-char hex string with configurable base char

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F43C);


void func_8002F488(u32 val, u8 *dst, s32 base_char) {
    s32 shift = 28;
    do {
        *dst++ = ((val >> shift) & 0xF) + base_char;
        shift -= 4;
    } while (shift >= 0);
    *dst = 0;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F4B0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F548);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F5B4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F610);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002F688);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002FD28);


void func_8002FD9C(s32 *a0, s32 a1) {
    s32 result = func_8002F548(a0[3]);
    a0[3] = result;
    func_8002F688(result, a1, -1);
}


void func_8002FDE8(s32 *a0, s32 a1) {
    func_8002F688(a0[3], a1, -1);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002FE0C);


// clear_rgb_fields - zeros bytes at offsets 0x20, 0x21, 0x22
void func_8002FF24(u8 *a0) {
    a0[0x20] = 0;
    a0[0x22] = 0;
    a0[0x21] = 0;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8002FF34);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030058);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030094);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800300F8);


void func_80030214(void) { func_80030058(0x1000); }


// empty stub
void func_80030234(void) {
}


extern s16 D_800834D4;
// set D_800834D4 (s16)
void func_8003023C(s32 val) {
    D_800834D4 = val;
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030248);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030274);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030288);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800302DC);


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

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030518);


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

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030754);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800307F8);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030848);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800308B0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003093C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030A54);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030B2C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030CB0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030CFC);


void func_80030D48(s32 a0) {
    extern u8 D_80052A34[];
    func_8001313C(D_80052A34[a0]);
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030D78);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030DB0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030E60);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030F10);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030FA0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80030FDC);


// empty stub
void func_80031044(void) {
}


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003104C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031188);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031224);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031364);


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

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800316C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800316D4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800318E0);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031954);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031A18);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031CDC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031D68);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031D8C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031DF4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031E1C);


extern u8 D_80083772[];
// clear_animation_entries

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031EBC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80031EF4);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032010);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800320BC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032204);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003228C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032350);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032534);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80032688);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003283C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800330F4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033298);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800332C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003331C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003334C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033380);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003346C);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800335AC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033688);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033768);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800337FC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033A28);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033C7C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033D5C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80033F1C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003406C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800341BC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_8003431C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80034830);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800348C4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800349F4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80034C74);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80034DBC);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035158);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035294);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_800352BC);


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


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035360);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035A6C);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035AA4);


extern u8 D_80085138;
// set D_80085138 (u8)

INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035AE4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035B28);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035B70);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035BB4);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035BC0);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035BCC);


INCLUDE_ASM("asm/nonmatchings/1C2A0", func_80035C10);


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
