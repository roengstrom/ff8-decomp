#include "common.h"

extern u8 D_80103180[];
extern u8 D_80103182[];
extern u8 D_80103184[];
extern u8 D_80103188[];
extern u8 D_80078752[];
extern u8 D_80103230[];
extern u8 D_80103340[];
void func_800D5C28(s32, void *, s32, s32);
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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DD28C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DD700);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DD80C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DD86C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DDCC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DDD70);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DDEE0);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DDF98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DE004);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DE0C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DE410);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DE550);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DE6FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DE8EC);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DEAA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DEB4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DEC6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DED4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF16C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF1E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF2E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF2F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF310);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF4E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF6AC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF718);

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
 * Calls func_8002A8B8 with a1, func_8002AA18 with a0, then func_8002A888.
 *
 * @param a0 Second animation parameter.
 * @param a1 First animation parameter.
 */
void func_800DF7C8(s32 a0, s32 a1) {
    func_8002A8B8(a1);
    func_8002AA18(a0);
    func_8002A888();
}

/** @brief Wrapper for func_8002C7BC. */
void func_800DF804(void) {
    func_8002C7BC();
}

/** @brief Wrapper for func_8002D6AC. */
void func_800DF824(void) {
    func_8002D6AC();
}

/** @brief Wrapper for func_8002DD58. */
void func_800DF844(void) {
    func_8002DD58();
}

/** @brief Wrapper for func_8002DCF4. */
void func_800DF864(void) {
    func_8002DCF4();
}

/** @brief Wrapper for func_8002E064. */
void func_800DF884(void) {
    func_8002E064();
}

/** @brief Wrapper for func_8002CA58. */
void func_800DF8A4(void) {
    func_8002CA58();
}

/** @brief Wrapper for func_8002DD78. */
void func_800DF8C4(void) {
    func_8002DD78();
}

/** @brief Wrapper for func_8002DE74. */
void func_800DF8E4(void) {
    func_8002DE74();
}

/** @brief Wrapper for func_8002E1E8. */
void func_800DF904(void) {
    func_8002E1E8();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF924);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DF9A4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DFCD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DFCF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DFEE0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800DFF80);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800E0084);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800E01C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800E0214);

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

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800E0370);

/** @brief Wrapper for func_800D90B4. */
void func_800E0458(void) {
    func_800D90B4();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object21", func_800E0478);
