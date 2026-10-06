#include "common.h"
#include "battle.h"
#include "battle/bc_object21.h"
#include "psxsdk/libapi.h"

extern u8 D_80103180[];
extern u8 D_80103182[];
extern u8 D_80103184[];
extern u8 D_80103188[];
extern u8 D_80103230[];
extern u8 D_80103340[];
extern u8 D_801031A0[];
extern u8 D_80103198[];
extern u8 D_80103191[];
void func_800D5C28(s32, s32, s32, s32);
void func_800D5D08(s32, s32);
void func_800DF4E4(void);
void func_800DF718(void);

/**
 * @brief Store a0 into indexed array and increment the index, inside a critical section.
 *
 * Stores the parameter as a byte into D_80103184 at the index given by
 * D_80103188, then increments the index.
 *
 * @param a0 Byte value to store in the array.
 */
void func_800DD1B0(s32 a0) {
    EnterCriticalSection();
    *(u8 *)(D_80103184 + *(volatile u8 *)D_80103188) = (u8)a0;
    *(volatile u8 *)D_80103188 = *(volatile u8 *)D_80103188 + 1;
    ExitCriticalSection();
}

/** @brief Clear D_80103188 inside a critical section. */
void func_800DD208(void) {
    EnterCriticalSection();
    *(volatile u8 *)D_80103188 = 0;
    ExitCriticalSection();
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

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DD700);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DD80C);

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
 * @brief Process entity data inside a critical section.
 *
 * Calls func_800DE8EC with D_80103340 and the entity parameter.
 *
 * @param a0 Entity parameter passed to func_800DE8EC.
 */
void func_800DEA58(s32 a0) {
    u8 *base = D_80103340;
    EnterCriticalSection();
    func_800DE8EC(base, a0);
    ExitCriticalSection();
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
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF6AC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF718);

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

/** @brief Wrapper for setDialogTextOrigin. */
void func_800DF804(s32 arg0, s32 arg1, s32 arg2) {
    setDialogTextOrigin(arg0, arg1, arg2);
}

/** @brief Wrapper for setDialogMessage. */
void func_800DF824(s32 arg0, u8* arg1) {
    setDialogMessage(arg0, arg1);
}

/** @brief Wrapper for closeDialogInstant. */
void func_800DF844(s32 arg0) {
    closeDialogInstant(arg0);
}

/** @brief Wrapper for openDialogInstant. */
void func_800DF864(s32 arg0) {
    openDialogInstant(arg0);
}

/** @brief Wrapper for setDialogRect. */
void func_800DF884(s32 arg0, RECT* arg1) {
    setDialogRect(arg0, arg1);
}

/** @brief Wrapper for setDialogTextSpeed. */
void func_800DF8A4(s32 arg0, s32 arg1) {
    setDialogTextSpeed(arg0, arg1);
}

/** @brief Wrapper for setDialogAnimSpeed. */
void func_800DF8C4(s32 arg0, s32 arg1) {
    setDialogAnimSpeed(arg0, arg1);
}

/** @brief Wrapper for setDialogCornerIcon. */
void func_800DF8E4(s32 arg0, s32 arg1) {
    setDialogCornerIcon(arg0, arg1);
}

/** @brief Wrapper for resetAllDialogs. */
void func_800DF904(void) {
    resetAllDialogs();
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800DF924);

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
 * returns a pointer to g_battleChars.chars[index].limitSlots.
 *
 * @param index Entity index.
 * @return Pointer to entity data.
 */
BattleMenuEntry* func_800E034C(s32 index) {
    return g_battleChars.chars[index].limitSlots;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E0370);

/** @brief Wrapper for func_800D90B4. */
void func_800E0458(void) {
    func_800D90B4();
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object21", func_800E0478);
