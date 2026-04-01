#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/color", func_800330F4);


/**
 * Wrapper for func_800330F4 with fixed 6th argument (mode) of 7.
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 * @param arg4 Fifth argument passed through from caller's stack
 */
void drawColorDefault(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    func_800330F4(a0, a1, a2, a3, arg4, 7);
}


/**
 * @brief Call func_800330F4 with a color from g_menuColor selected by arg4.
 *
 * If arg4 >= 8, subtracts 8 and uses g_menuColor[1]; otherwise uses
 * g_menuColor[0]. Passes the selected color as the 5th arg and the
 * modified arg4 as the 6th.
 *
 * @param a0 First argument passed through.
 * @param a1 Second argument passed through.
 * @param a2 Third argument passed through.
 * @param a3 Fourth argument passed through.
 * @param arg4 Mode index; values >= 8 select the alternate color table.
 */
void drawColorByMenuPalette(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    extern s32 g_menuColor[];
    s32 idx;
    if (arg4 >= 8) {
        arg4 -= 8;
        idx = 1;
    } else {
        idx = 0;
    }
    func_800330F4(a0, a1, a2, a3, g_menuColor[idx], arg4);
}


/**
 * Calls func_800330F4 with g_menuColor as the 5th arg and 7 as the 6th (mode).
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 */
void drawMenuColorDefault(s32 a0, s32 a1, s32 a2, s32 a3) {
    extern s32 g_menuColor;
    func_800330F4(a0, a1, a2, a3, g_menuColor, 7);
}


INCLUDE_ASM("asm/nonmatchings/color", func_8003334C);


INCLUDE_ASM("asm/nonmatchings/color", func_80033380);


INCLUDE_ASM("asm/nonmatchings/color", func_8003346C);


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
void drawInsetRect(s32 a0, s32 a1, u16 *a2) {
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


INCLUDE_ASM("asm/nonmatchings/color", func_800335AC);


INCLUDE_ASM("asm/nonmatchings/color", func_80033688);


INCLUDE_ASM("asm/nonmatchings/color", func_80033768);


INCLUDE_ASM("asm/nonmatchings/color", func_800337FC);


INCLUDE_ASM("asm/nonmatchings/color", func_80033A28);


INCLUDE_ASM("asm/nonmatchings/color", func_80033C7C);


INCLUDE_ASM("asm/nonmatchings/color", func_80033D5C);


INCLUDE_ASM("asm/nonmatchings/color", func_80033F1C);


INCLUDE_ASM("asm/nonmatchings/color", func_8003406C);


INCLUDE_ASM("asm/nonmatchings/color", func_800341BC);


INCLUDE_ASM("asm/nonmatchings/color", func_8003431C);


/**
 * @brief Render three rows of stat delta bars.
 *
 * Calls func_8003431C three times with incrementing row index (0, 1, 2)
 * and a y-offset that advances by 0x32 each row. Returns the result of
 * the last call.
 *
 * @param a0 Render context passed through to func_8003431C.
 * @param a1 Initial bar value, threaded through each call's return.
 * @param a2 Color/palette parameter passed on stack to func_8003431C.
 * @return Result of the final func_8003431C call.
 */
s32 func_80034830(s32 a0, s32 a1, s32 a2) {
    s32 rowWidth = 0x1E;
    s32 yOffset = 0x22;
    s32 i = 0;
    s32 scale = 0x1000;
    do {
        a1 = func_8003431C(a0, a1, i, rowWidth, yOffset, scale, a2);
        i++;
        yOffset += 0x32;
    } while (i < 3);
    return a1;
}


INCLUDE_ASM("asm/nonmatchings/color", func_800348C4);


INCLUDE_ASM("asm/nonmatchings/color", func_800349F4);


INCLUDE_ASM("asm/nonmatchings/color", func_80034C74);


INCLUDE_ASM("asm/nonmatchings/color", func_80034DBC);


/**
 * @brief Enable the display and execute a full rendering pass.
 *
 * Calls SetDispMask(1) to make the framebuffer visible, then calls
 * func_80034DBC (main rendering) followed by func_8003283C (scene submission).
 */
void enableDisplayAndRender(void) {
    SetDispMask(1);
    func_80034DBC();
    func_8003283C();
}


extern u8 D_80083928;
/**
 * @brief Get the current value of the global flag D_80083928.
 * @return The flag value as an unsigned byte.
 */
u8 getRenderCompleteFlag(void) {
    return D_80083928;
}


INCLUDE_ASM("asm/nonmatchings/color", func_80035158);


