#include "common.h"

extern u8 D_80102F50[];
extern u8 D_80103050[];
extern u8 D_80103054[];
extern u8 D_80103058[];
extern u8 D_8010305C[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D8FA4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D8FE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9018);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9060);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D90B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9274);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9794);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9934);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9AD4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800D9E84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA3C0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA3F0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA574);

/**
 * @brief Advance render buffer index and update pointer (buffer A).
 *
 * Increments D_80103050 mod 4, computes the new buffer pointer
 * as D_80102F50 + index * 64, and stores it to D_80103058.
 */
void func_800DA5AC(void) {
    s32 idx = (*(s32 *)D_80103050 + 1) & 3;
    *(s32 *)D_80103058 = (s32)D_80102F50 + idx * 64;
    *(s32 *)D_80103050 = idx;
}

/**
 * @brief Advance render buffer index and update pointer (buffer B).
 *
 * Increments D_80103054 mod 4, computes the new buffer pointer
 * as D_80102F50 + index * 64, and stores it to D_8010305C.
 */
void func_800DA5DC(void) {
    s32 idx = (*(s32 *)D_80103054 + 1) & 3;
    *(s32 *)D_8010305C = (s32)D_80102F50 + idx * 64;
    *(s32 *)D_80103054 = idx;
}

/**
 * @brief Return the word value at D_80103058.
 *
 * @return Current value of D_80103058.
 */
s32 func_800DA60C(void) {
    return *(s32 *)D_80103058;
}

/**
 * @brief Return the word value at D_8010305C.
 *
 * @return Current value of D_8010305C.
 */
s32 func_800DA61C(void) {
    return *(s32 *)D_8010305C;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA62C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA634);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA650);

/**
 * @brief Check if bit 15 of a halfword at offset 0x1C is clear.
 *
 * Calls func_800DA61C to get a base pointer, reads the halfword at +0x1C,
 * and returns whether bit 15 (0x8000) is zero.
 *
 * @return 1 if bit 15 is clear, 0 if set.
 */
s32 func_800DA718(void) {
    s32 val = *(u16 *)(func_800DA61C() + 0x1C) & 0x8000;
    return val == 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA744);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA778);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA938);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DA984);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DAA30);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DAD34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DAD78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DADB4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB110);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB140);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB1BC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB248);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DB280);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBC88);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBCBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBF50);

/**
 * @brief Select a mask from a1 based on bit 6 of a0.
 *
 * @param a0 Selector value (bit 6 tested).
 * @param a1 Value to mask.
 * @return a1 & 0x78 if bit 6 is set, otherwise a1 & 0x7.
 */
s32 func_800DBF84(s32 a0, s32 a1) {
    s32 result = a1 & 0x78;
    if (!(a0 & 0x40)) {
        result = a1 & 0x7;
    }
    return result;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DBF9C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC030);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC080);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC0CC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC334);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC3D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC41C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC62C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC664);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC75C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC798);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC820);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC8B8);

/**
 * @brief Wrapper for func_800B6A9C.
 */
void func_800DC928(void) {
    func_800B6A9C();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC948);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC9A0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DC9E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DCAA0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object20", func_800DCC08);

void func_800DD1A8(void) {
}
