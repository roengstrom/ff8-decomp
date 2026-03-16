#include "common.h"

extern u8 D_800786D8;

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD6EC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD754);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD7A4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD7E4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD82C);

/**
 * @brief Copy 10 flag bytes from D_800780D8 to destination struct.
 *
 * Copies bytes at D_800780D8 offsets 0x108-0x10F to dst offsets 0x66-0x6D,
 * and 0x100-0x101 to dst offsets 0x6E-0x6F.
 *
 * @param dst Destination buffer pointer.
 */
void func_800BD918(u8 *dst) {
    extern u8 D_800780D8[];
    u8 *src = D_800780D8;

    dst[0x66] = src[0x108];
    dst[0x67] = src[0x109];
    dst[0x68] = src[0x10A];
    dst[0x69] = src[0x10B];
    dst[0x6A] = src[0x10C];
    dst[0x6B] = src[0x10D];
    dst[0x6C] = src[0x10E];
    dst[0x6D] = src[0x10F];
    dst[0x6E] = src[0x100];
    dst[0x6F] = src[0x101];
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BD998);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDA08);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDA78);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDAE4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDB5C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDBE0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDC94);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDDC4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BDEF4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE040);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE158);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE1A8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE284);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE3D0);

/**
 * @brief Linearly interpolate between two 3D vectors.
 *
 * Computes out[i] = b[i] + (a[i] - b[i]) * t / 4096 for each of 3
 * components, using fixed-point arithmetic with rounding toward zero.
 *
 * @param a Source vector (3 x s32).
 * @param b Base vector (3 x s32).
 * @param t Interpolation factor (s16, fixed-point 12-bit).
 * @param out Output vector (3 x s32). If NULL, no operation.
 */
void func_800BE4D8(s32 *a, s32 *b, s32 t, s32 *out) {
    s32 diff;
    s32 val;
    s16 frac;

    if (out == 0) {
        return;
    }

    diff = a[0] - b[0];
    frac = t;
    val = diff * frac;
    if (val < 0) {
        val += 0xFFF;
    }
    out[0] = b[0] + (val >> 12);

    val = (a[1] - b[1]) * frac;
    if (val < 0) {
        val += 0xFFF;
    }
    out[1] = b[1] + (val >> 12);

    val = (a[2] - b[2]) * frac;
    if (val < 0) {
        val += 0xFFF;
    }
    out[2] = b[2] + (val >> 12);
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE578);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE5F8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE63C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE69C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE720);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE7FC);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE8B0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE958);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BE9F8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEA34);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEA7C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEAF4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEB84);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEC1C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEC60);

/**
 * @brief Search D_80077378 array for an active entry with type 0xA2.
 */
s32 func_800BED90(s32 *outIndex, s32 *outCount) {
    extern u8 D_80077378[];
    s32 found = 0;
    s32 i = found;
    s32 target = 0xA2;
    u8 *ptr = D_80077378;
    s32 type;
    s32 count;

    top:
        type = *(u8 *)(ptr + 0xB44);
        count = *(u8 *)(ptr + 0xB45);
        if (type != target) goto skip;
        if (count <= 0) goto skip;
        found = 1;
        if (outIndex != 0) {
            *outIndex = i;
        }
        if (outCount != 0) {
            *outCount = count;
            goto end;
        }
        goto end;
    skip:
        i += 1;
        if (i < 0xC6) {
            ptr += 2;
            goto top;
        }
    end:
    return found;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEDF0);

void func_800BEECC(void) {
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEED4);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BEF6C);

/** Checks two flag bits and returns status. */
s32 func_800BEFC4(void) {
    u8 val = D_800786D8;
    s32 result = 0;
    if (val & 1) {
        s32 bit = val & 2;
        result = (u32)bit < 1;
    }
    return result;
}

/** Initializes nested array with ramp values. */
void func_800BEFEC(u8 *base) {
    s32 outer = 7;
    s32 inner;
    s32 val;
    s32 *ptr;
    do {
        inner = 4;
        val = -0x1000;
        ptr = (s32 *)(base + 0x10);
        do {
            *(s32 *)((u8 *)ptr + 0x38) = val;
            val += 0x400;
            inner--;
            ptr = (s32 *)((u8 *)ptr - 4);
        } while (inner >= 0);
        outer--;
        base += 0x4C;
    } while (outer >= 0);
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF024);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF20C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF2E8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF3D8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object10", func_800BF5A8);
