#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/238F4", func_800330F4);


/**
 * Wrapper for func_800330F4 with fixed 6th argument of 7.
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 * @param arg4 Fifth argument passed through from caller's stack
 */
void func_80033298(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    func_800330F4(a0, a1, a2, a3, arg4, 7);
}


INCLUDE_ASM("asm/nonmatchings/238F4", func_800332C4);


/**
 * Calls func_800330F4 with D_80083848 as the 5th arg and 7 as the 6th.
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 */
void func_8003331C(s32 a0, s32 a1, s32 a2, s32 a3) {
    extern s32 D_80083848;
    func_800330F4(a0, a1, a2, a3, D_80083848, 7);
}


INCLUDE_ASM("asm/nonmatchings/238F4", func_8003334C);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033380);


INCLUDE_ASM("asm/nonmatchings/238F4", func_8003346C);


/**
 * @brief Inset a rectangle by 1 pixel on each side and dispatch for rendering.
 *
 * Saves the original rectangle coordinates (4 u16 values), adjusts them
 * (x+1, y+1, w-2, h-2), calls func_8003346C to render with the inset rect,
 * then restores the original values.
 *
 * @param a0 First rendering parameter passed to func_8003346C.
 * @param a1 Second rendering parameter passed to func_8003346C.
 * @param a2 Pointer to a 4-element u16 rectangle {x, y, w, h} (modified temporarily).
 */
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


INCLUDE_ASM("asm/nonmatchings/238F4", func_800335AC);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033688);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033768);


INCLUDE_ASM("asm/nonmatchings/238F4", func_800337FC);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033A28);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033C7C);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033D5C);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80033F1C);


INCLUDE_ASM("asm/nonmatchings/238F4", func_8003406C);


INCLUDE_ASM("asm/nonmatchings/238F4", func_800341BC);


INCLUDE_ASM("asm/nonmatchings/238F4", func_8003431C);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80034830);


INCLUDE_ASM("asm/nonmatchings/238F4", func_800348C4);


INCLUDE_ASM("asm/nonmatchings/238F4", func_800349F4);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80034C74);


INCLUDE_ASM("asm/nonmatchings/238F4", func_80034DBC);


/**
 * @brief Enable the display and execute a full rendering pass.
 *
 * Calls SetDispMask(1) to make the framebuffer visible, then calls the
 * main rendering function (func_80034DBC) followed by the scene submission
 * function (func_8003283C).
 */
void func_80035118(void) {
    SetDispMask(1);
    func_80034DBC();
    func_8003283C();
}


extern u8 D_80083928;
/**
 * @brief Get the current value of the global flag D_80083928.
 * @return The flag value as an unsigned byte.
 */
u8 func_80035148(void) {
    return D_80083928;
}


INCLUDE_ASM("asm/nonmatchings/238F4", func_80035158);


