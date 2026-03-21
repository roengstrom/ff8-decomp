#include "common.h"

void func_801E5800(s32 a0);
void func_801E6170(s32 a0, s32 a1, s32 a2);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5800);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5AF0);

/**
 * @brief Look up a triple magic menu entry address.
 *
 * Indexes into D_801E764C by a0 (halfword stride), loads the offset
 * at +2, and returns base + offset.
 *
 * @param a0 Menu entry index.
 * @return Pointer into D_801E764C data.
 */
s32 func_801E5C0C(s32 a0) {
    extern u8 D_801E764C[];
    u16 *table = (u16 *)D_801E764C;
    return table[a0 + 1] + (s32)D_801E764C;
}

/**
 * @brief Return triple magic data buffer address.
 *
 * @return Constant 0x801CD000.
 */
s32 func_801E5C28(void) {
    return (s32)0x801CD000;
}

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5C34);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5D1C);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E5E2C);

/**
 * @brief Render a complete triple magic menu entry through multiple rendering passes.
 *
 * Chains five rendering calls in sequence: icon rendering (func_801E5D1C),
 * text rendering (func_801E5C34), color/shade rendering (func_801E5AF0),
 * border rendering (func_8002B898), and GPU primitive setup (func_801E5E2C).
 * Each call's return value becomes the next call's second argument.
 *
 * @param ctx Render context.
 * @param a1 Initial buffer/primitive pointer.
 * @param display Display configuration pointer.
 * @param colorData Color/shading data.
 * @param src Source entry data pointer (passed as 5th arg to sub-calls).
 */
void func_801E5F5C(s32 ctx, s32 a1, s32 display, s32 colorData, s32 src) {
    a1 = func_801E5D1C(ctx, a1, display, colorData, src);
    a1 = func_801E5C34(ctx, a1, display, colorData, src);
    a1 = func_801E5AF0(ctx, a1, display, colorData, src);
    a1 = func_8002B898(ctx, a1, display, colorData);
    func_801E5E2C(ctx, a1, display, colorData, src);
}

/**
 * @brief Set up display config from a source struct and render triple magic entry.
 *
 * Copies four halfwords from the source struct to g_menuDisplayCfg, adding x and y
 * offsets to the first two fields. Then calls func_801E5F5C to perform the
 * actual rendering with the configured display parameters.
 *
 * @param a0 Render context parameter.
 * @param a1 Entry index parameter.
 * @param xOff X offset to add to position.
 * @param yOff Y offset to add to position.
 * @param src Pointer to source coordinate/size struct (4 halfwords).
 */
void func_801E6008(s32 a0, s32 a1, s32 xOff, s32 yOff, u16 *src) {
    extern u16 g_menuDisplayCfg[];
    extern s32 g_menuColor;

    g_menuDisplayCfg[0] = src[0] + xOff;
    g_menuDisplayCfg[1] = src[1] + yOff;
    g_menuDisplayCfg[2] = src[2];
    g_menuDisplayCfg[3] = src[3];
    func_801E5F5C(a0, a1, g_menuDisplayCfg, g_menuColor, src);
}

/**
 * @brief Render the triple magic menu title/header area.
 *
 * Creates a display box for the menu header using func_801F08D4, measures the
 * title text width to center it, renders the border and GPU primitives.
 *
 * @param ctx Render context.
 * @param buf Buffer/primitive pointer.
 * @param entryData Entry data pointer (passed to GPU primitive setup).
 *
 * @note Non-matching: register allocation differs. Original assigns ctx to s2
 * and buf to s0, but the compiler assigns ctx to s1 and buf to s3 due to
 * different use-count priority ordering.
 */
INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E606C);

INCLUDE_ASM("asm/ovl/menutmag/nonmatchings/menutmag", func_801E6170);

/**
 * @brief Initialize and run the triple magic menu overlay.
 *
 * Registers the overlay's main handler and render callback, loads the
 * "mmag.bin" data file, and if an active menu context is returned, looks up
 * the current entry, configures palette and display parameters, and invokes
 * the main state machine (func_801E5800).
 */
void func_801E6290(void) {
    extern u8 D_801E63EC[];
    extern u8 D_801E63F8[];
    extern u8 D_801E764C[];

    s32 ctx;
    s32 entryIdx;
    u8 *entry;

    ctx = func_801F179C(func_801E5800, func_801E6170);
    func_801F1D2C(0, D_801E63EC, D_801E63F8);

    if (ctx != 0) {
        entryIdx = func_801E2800();
        *(s32 *)(ctx + 0x28) = entryIdx;
        *(s32 *)(ctx + 0x2C) = entryIdx;
        entry = D_801E63F8 + entryIdx * 68;
        func_800360D0(*(u8 *)(entry + 0x15) + 0x57, D_801E764C);
        func_801E2820(*(u8 *)(entry + 0x16), *(u8 *)(entry + 0x17));
        func_801E5800(ctx);
    }
}

/**
 * @brief Wrapper that calls func_801E6290 (triple magic menu init/update).
 */
void func_801E6348(void) {
    func_801E6290();
}

/**
 * @brief Wrapper that calls func_801E6290 (triple magic menu init/update).
 */
void func_801E6368(void) {
    func_801E6290();
}

/**
 * @brief Wrapper that calls func_801E6290 (triple magic menu init/update).
 */
void func_801E6388(void) {
    func_801E6290();
}
