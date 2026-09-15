#include "common.h"
#include "psxsdk/libgpu.h"

extern u8 D_80103180[];
extern u8 D_80103182[];
extern u8 D_80103184[];
extern u8 D_80103188[];
extern u8 D_80078752[];
extern u8 D_80103230[];
extern u8 D_80103420[];
extern u8 D_80103340[];
extern u8 D_801031A0[];
extern u8 D_80103198[];
extern u8 D_80103191[];
void func_800D5C28(s32, s32, s32, s32);
void func_800D5D08(s32, s32);
void func_800DF4E4(void);
u16 func_800DA62C(s32);
void func_800DF718(void);

/**
 * @brief Disable display, store a0 into indexed array, increment index, enable display.
 *
 * Stores the parameter as a byte into D_80103184 at the index given by
 * D_80103188, increments the index, then re-enables display.
 *
 * @param a0 Byte value to store in the array.
 */
void func_800DD1B0(s32 a0) {
    func_800472E4();
    *(u8 *)(D_80103184 + *(volatile u8 *)D_80103188) = (u8)a0;
    *(volatile u8 *)D_80103188 = *(volatile u8 *)D_80103188 + 1;
    func_800472F4();
}

/**
 * @brief Disable display, clear D_80103188, then enable display.
 *
 * Calls func_800472E4 (display off), zeros D_80103188,
 * then calls func_800472F4 (display on).
 */
void func_800DD208(void) {
    func_800472E4();
    *(volatile u8 *)D_80103188 = 0;
    func_800472F4();
}

/**
 * @brief Check if D_80103182 is nonzero, but only if D_80103180 is nonzero.
 *
 * @return 1 if both D_80103180 and D_80103182 are nonzero, 0 otherwise.
 */
s32 func_800DD238(void) {
    s32 result = 0;
    if (*(u8 *)D_80103180 != 0) {
        result = *(u8 *)D_80103182 != 0;
    }
    return result;
}

/**
 * @brief Return the byte value at D_80103182.
 *
 * @return Current value of D_80103182 (unsigned byte).
 */
s32 func_800DD264(void) {
    return *(u8 *)D_80103182;
}

/**
 * @brief Store a byte value to D_80103180.
 *
 * @param a0 Value to store (low byte).
 */
void func_800DD274(s32 a0) {
    *(u8 *)D_80103180 = (u8)a0;
}

/**
 * @brief Store a byte value to D_80103182.
 *
 * @param a0 Value to store (low byte).
 */
void func_800DD280(s32 a0) {
    *(u8 *)D_80103182 = (u8)a0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DD28C);

u8 *func_800DD700(u8 *ot, u8 *p, u16 *xy, u32 color) {
    register s32 t1 __asm__("$9") = 3;
    register s32 t0 __asm__("$8") = 0x60;
    register s32 t2 __asm__("$10");
    register s32 v1 __asm__("$3");
    register u16 v0 __asm__("$2");

    p[3] = t1;
    p[7] = t0;
    *(u32 *)(p + 4) = color;
    v0 = xy[0];
    t2 = 0x126;
    *(u16 *)(p + 8) = v0;
    v0 = xy[1];
    v1 = 2;
    *(u16 *)(p + 0xC) = t2;
    *(u16 *)(p + 0xE) = v1;
    *(u16 *)(p + 0xA) = v0;
    addPrimFast(ot, p, t3);
    p += 0x10;

    p[3] = t1;
    p[7] = t0;
    *(u32 *)(p + 4) = color;
    v0 = xy[0];
    *(u16 *)(p + 8) = v0;
    v0 = xy[3];
    *(u16 *)(p + 0xC) = t2;
    *(u16 *)(p + 0xE) = v1;
    *(u16 *)(p + 0xA) = v0;
    addPrimFast(ot, p, t4);
    p += 0x10;

    p[3] = t1;
    p[7] = t0;
    *(u32 *)(p + 4) = color;
    v0 = xy[0];
    *(u16 *)(p + 8) = v0;
    v0 = xy[1];
    t2 = 0x18;
    *(u16 *)(p + 0xC) = v1;
    *(u16 *)(p + 0xE) = t2;
    v0 = v0 + 2;
    *(u16 *)(p + 0xA) = v0;
    addPrimFast(ot, p, t5);
    p += 0x10;

    p[3] = t1;
    p[7] = t0;
    *(u32 *)(p + 4) = color;
    v0 = xy[2];
    *(u16 *)(p + 8) = v0;
    v0 = xy[1];
    *(u16 *)(p + 0xC) = v1;
    *(u16 *)(p + 0xE) = t2;
    v0 = v0 + 2;
    *(u16 *)(p + 0xA) = v0;
    addPrimFast(ot, p, t6);
    return p + 0x10;
}

u8 *func_800DD80C(u8 *ot, u8 *p, u16 *xywh) {
    setlen(p, 3);
    setcode(p, 0x60);
    *(u32 *)(p + 4) = 0x60000000;
    *(u16 *)(p + 8) = xywh[0];
    *(u16 *)(p + 0xA) = xywh[1];
    *(u16 *)(p + 0xC) = xywh[2];
    *(u16 *)(p + 0xE) = xywh[3];
    addPrimFast(ot, p, v1);
    return p + 0x10;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DD86C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DDCC8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DDD70);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DDEE0);

/**
 * @brief Reset status: clear D_80103182 and D_80103180.
 *
 * Calls func_800DD280(0) to clear D_80103182, then directly clears D_80103180.
 */
void func_800DDF6C(void) {
    func_800DD280(0);
    *(u8 *)D_80103180 = 0;
}

/**
 * @brief Return constant 0x20 (32).
 *
 * @return Always 0x20.
 */
s32 func_800DDF90(void) {
    return 0x20;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DDF98);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DE004);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DE0C0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DE410);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DE550);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DE6FC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DE8EC);

/**
 * @brief Lock rendering, process entity data, then unlock.
 *
 * Calls func_800472E4 to lock, then func_800DE8EC with D_80103340
 * and the entity parameter, then func_800472F4 to unlock.
 *
 * @param a0 Entity parameter passed to func_800DE8EC.
 */
void func_800DEA58(s32 a0) {
    u8 *base = D_80103340;
    func_800472E4();
    func_800DE8EC(base, a0);
    func_800472F4();
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DEAA4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DEB4C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DEC6C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DED4C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF16C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF1E4);



INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF310);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF4E4);

/**
 * @brief Initialize display list handler and set screen parameters.
 *
 * Registers func_800DF4E4 as a handler via func_800D5C28 with size 8,
 * configures display dimensions via func_800D5D08, and sets screen
 * parameters: D_801031A0=0xC5, D_80103198=0x5F, D_8010319C=0x80,
 * D_8010319E=0x10, D_80103191=0.
 */
/**
 * @brief Register DF4E4 task on slot 8 and seed UI params.
 *
 * Calls func_800D5C28(8, 0, func_800DF4E4, 0) and func_800D5D08(8, 3),
 * then stores D_801031A0=0xC5, D_80103198=0x5F, D_8010319C=0x80,
 * D_8010319E=0x10, D_80103191=0.
 */
void func_800DF6AC(void) {
    u8 *p;
    s32 new_var;

    func_800D5C28(8, 0, (s32)func_800DF4E4, 0);
    func_800D5D08(8, 3);
    new_var = 0x80;
    *(s16 *)D_801031A0 = 0xC5;
    *(s16 *)D_80103198 = 0x5F;
    p = D_80103198;
    *(s16 *)(p + 4) = new_var;
    *(s16 *)(p + 6) = 0x10;
    *D_80103191 = 0;
}

void func_800DF718(void) {
    u16 cur = func_800DA62C(0);
    s32 i = 0;
    u16 prev = *(u16 *)D_80103230;
    s32 bits;
    *(u16 *)D_80103230 = cur;
    bits = (s32)((cur ^ prev) & 0x78) >> 3;
    do {
        if ((bits >> i) & 1) {
            u8 *p = D_80103420;
            s32 j = 2;
            do {
                (p + i)[0x64] = 0;
                j -= 1;
                p += 0x6C;
            } while (j >= 0);
        }
        i += 1;
    } while (i < 4);
}


/**
 * @brief Register callback func_800DF718 and clear D_80103230.
 *
 * Calls func_800D5C28 with mode=1, callback=func_800DF718, and two zero args.
 * Then clears the halfword at D_80103230.
 */
void func_800DF794(void) {
    func_800D5C28(1, func_800DF718, 0, 0);
    *(u16 *)D_80103230 = 0;
}

/**
 * @brief Call three animation functions in sequence.
 *
 * Calls storeGpuPacket with a1, addPrimitive with a0, then getDisplayListHead.
 *
 * @param a0 Second animation parameter.
 * @param a1 First animation parameter.
 */
void func_800DF7C8(s32 a0, s32 a1) {
    storeGpuPacket(a1);
    addPrimitive(a0);
    getDisplayListHead();
}

/** @brief Wrapper for setSfxEntryParams. */
void func_800DF804(void) {
    setSfxEntryParams();
}

/** @brief Wrapper for initSfxPlayback. */
void func_800DF824(void) {
    initSfxPlayback();
}

/** @brief Wrapper for fadeOutSfxFast. */
void func_800DF844(void) {
    fadeOutSfxFast();
}

/** @brief Wrapper for startSfxNormal. */
void func_800DF864(void) {
    startSfxNormal();
}

/** @brief Wrapper for func_8002E064. */
void func_800DF884(void) {
    func_8002E064();
}

/** @brief Wrapper for setSfxPitch. */
void func_800DF8A4(void) {
    setSfxPitch();
}

/** @brief Wrapper for setSfxReverbMode. */
void func_800DF8C4(void) {
    setSfxReverbMode();
}

/** @brief Wrapper for setSfxField2F. */
void func_800DF8E4(void) {
    setSfxField2F();
}

/** @brief Wrapper for resetAllSfx. */
void func_800DF904(void) {
    resetAllSfx();
}

s32 func_800DF924(void) {
    s32 v0 = func_800CEBF8();
    s32 a1 = v0 / (s32)0x2AAA;
    s32 v1;
    s32 tmp = a1 / 3;
    a1 -= tmp * 3;
    a1 += 1;
    if (a1 <= 0) {
        v1 = 1;
        goto done;
    }
    if (a1 < 4) {
        v1 = a1;
        goto done;
    }
    v1 = 3;
done:
    return v1;
}


INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF9A4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DFCD0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DFCF4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DFEE0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DFF80);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E0084);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E01C8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E0214);

/**
 * @brief Get pointer to entity data at given index.
 *
 * Computes D_80078752 + index * 464 (0x1D0 stride per entity).
 *
 * @param index Entity index.
 * @return Pointer to entity data.
 */
u8 *func_800E034C(s32 index) {
    return D_80078752 + index * 464;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E0370);

/** @brief Wrapper for func_800D90B4. */
void func_800E0458(void) {
    func_800D90B4();
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E0478);
