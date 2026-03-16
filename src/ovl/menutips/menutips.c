#include "common.h"

/**
 * @brief Render a tips panel entry.
 *
 * Calls func_801F08D4 with mode 1, section 0xD, the provided context,
 * and flag 0.
 *
 * @param a0 Render context pointer.
 */
s32 func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E582C);

/**
 * @brief Pop the last value pair from the tips history ring buffer.
 *
 * Decrements the counter in D_801ED630 and returns the entry at the
 * new position as a packed s32 (lo | hi << 16). Returns -1 if the
 * buffer is empty (counter <= 0).
 *
 * @return Packed value pair, or -1 if empty.
 */
s32 func_801E58C0(void) {
    extern u8 D_801ED630[];
    extern u8 D_801ED430[];
    s16 sv;
    u16 uv;
    s32 off;
    s32 base;
    s16 *entry;
    s32 lo;
    s32 hi;

    sv = *(s16 *)D_801ED630;
    uv = *(u16 *)D_801ED630;
    if (sv <= 0) {
        return -1;
    }
    uv--;
    base = (s32)D_801ED430;
    off = (s16)uv * 4;
    entry = (s16 *)(base + off);
    lo = *(s16 *)entry;
    hi = *(s16 *)(entry + 1);
    *(u16 *)D_801ED630 = uv;
    return lo | (hi << 16);
}

/**
 * @brief Render a tips entry at a table-derived position.
 *
 * Checks D_801EC410; if positive, looks up coordinates from
 * D_801EC310[a1] (8-byte stride), applies offsets (+0x22, +0x23),
 * and calls func_801F0A34 to render.
 *
 * @param a0 Render context pointer.
 * @param a1 Index into the coordinate table.
 *
 * @note Non-matching: Compiler schedules sll (a1*8) before lui/addiu
 * for D_801EC310 table base, changing register allocation (v1 vs v0
 * for result pointer).
 */
INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E590C);

/**
 * @brief Look up a tips data pointer from the entry table.
 *
 * Indexes into D_801EC420 by a0 (4-byte stride), reads the halfword
 * at offset +4, and returns it added to 0x801D1000.
 *
 * @param a0 Tips entry index.
 * @return Pointer to tips data (as s32).
 */
s32 func_801E5958(s32 a0) {
    extern u8 D_801EC420[];
    s32 base = (s32)D_801EC420;
    return *(u16 *)(base + a0 * 4 + 4) + 0x801D1000;
}

/**
 * @brief Initialize tips page data and render entry text.
 *
 * Calls func_801E5958 to get a tips data buffer. Copies the first three
 * halfwords from the buffer header into D_801EC412-D_801EC416, clears
 * D_801EC410, then calls func_8002A2A8 to set up the tips table, measures
 * the text length with func_8002A2F4, and finalizes via func_801E6018.
 * Stores the result in D_801EC410.
 *
 * @note Non-matching: register allocation differs for the store temp
 * variable. The original reuses a2 for both global stores and the
 * func_8002A2A8 third argument, but the compiler allocates a separate
 * register (a3) for the stores and sets a2 with a move instruction.
 */
INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E597C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E5A10);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6018);

/**
 * @brief Check tutorial availability and save current selection.
 *
 * Calls func_80035E00 to check if the tips system is ready. If not ready
 * (returns 0), copies D_801ED422 to D_801ED420 (saves current selection)
 * and returns 1. Otherwise returns 0.
 *
 * @return 1 if tips system was not ready (selection saved), 0 otherwise.
 */
s32 func_801E6474(void) {
    extern u16 D_801ED422;
    extern u16 D_801ED420;
    if (func_80035E00() == 0) {
        D_801ED420 = D_801ED422;
        return 1;
    }
    return 0;
}

/**
 * @brief Load tips page data if the current selection has changed.
 *
 * Indexes into D_801EC420 to find the entry for the given page index.
 * If the entry's ID differs from D_801ED420, calls func_800360D0 to
 * load the new page data from 0x801D1000, then updates D_801ED422 with
 * the new ID.
 *
 * @param a0 Page index into the tips entry table.
 */
void func_801E64B0(s32 a0) {
    extern u8 D_801EC420[];
    extern s16 D_801ED420;
    extern u16 D_801ED422;
    s32 base = (s32)D_801EC420;
    u16 *entry;
    u16 id;
    a0 = a0 * 4 + 4;
    entry = (u16 *)(a0 + base);
    id = entry[1];
    if (D_801ED420 != id) {
        func_800360D0(id + 0x80, 0x801D1000);
        D_801ED422 = entry[1];
    }
}

/**
 * @brief Render tips navigation panel with scroll indicators.
 *
 * @param a0 Display list pointer.
 * @param a1 OT pointer.
 * @return Updated OT pointer from func_801EF9AC.
 */
s32 func_801E6514(s32 a0, s32 a1) {
    extern u8 D_801FAB00[];
    extern s16 D_801EC410;
    extern u16 D_801EC414;
    extern u16 D_801EC416;
    extern s32 D_80083848;
    s32 ot = a1;
    s32 resource;
    s32 tw;
    s32 width;
    s32 xPos = 0x18;
    s32 yPos;
    u8 *cfg = D_801FAB00;
    yPos = 0xC4;
    if (D_801EC410 != 0) {
        resource = func_801E5800(0xE);
    } else {
        resource = func_801E5800(0xF);
        if ((D_801EC416 != 0xFFFF) || (D_801EC414 != D_801EC416)) {
            resource = func_801E5800(0x20);
        }
    }
    if (D_801EC416 != 0xFFFF) {
        s32 arg = 0x1F;
        if (D_801EC410 == 0) {
            arg = 0x1D;
        }
        resource = func_801E5800(arg);
    }
    tw = func_8002E680(resource);
    tw = (0x150 - (u16)tw) / 2;
    func_8002EAD0(a0, xPos + tw, yPos + 4, resource);
    width = 0x150;
    cfg[0x10] = 0;
    cfg[0x11] = 0;
    *(s16 *)(cfg + 0x0) = xPos;
    *(s16 *)(cfg + 0x2) = yPos;
    *(s16 *)(cfg + 0x4) = width;
    *(s16 *)(cfg + 0x6) = 0x14;
    return func_801EF9AC(a0, ot, 0x1000, D_80083848);
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E6668);

/**
 * @brief Render tips header panel.
 *
 * Calls func_8002EAD0 to set up the display region using the D_801E7B10
 * data table, then configures D_801FAB00 with fixed panel dimensions
 * and calls func_801EF9AC to render.
 *
 * @param a0 Display list pointer.
 * @param a1 OT pointer.
 * @return Updated OT pointer from func_801EF9AC.
 */
s32 func_801E6768(s32 a0, s32 a1) {
    extern u8 D_801E7B10[];
    extern u8 D_801FAB00[];
    extern s32 D_80083848;
    u8 *new_var;
    s32 ot;
    s32 disp;
    s32 cfg;
    if (1) {
        disp = a0;
        ot = a1;
        new_var = D_801E7B10;
        func_8002EAD0(disp, 0x24, 0xC, (((s32)new_var) + ot) - ot);
        cfg = (s32)D_801FAB00;
        *(u8 *)(cfg + 0x10) = 0;
        *(u8 *)(cfg + 0x11) = 0;
        *(s16 *)&D_801FAB00[0] = 0x18;
        *(s16 *)(cfg + 2) = 6;
    }
    *(s16 *)(cfg + 4) = 0xF4;
    *(s16 *)(cfg + 6) = 0x16;
    return func_801EF9AC(disp, ot, 0x1000, D_80083848);
}

/**
 * @brief Render tips display pipeline.
 *
 * If the tips state has an active display (field 0x2A non-zero),
 * sets up rendering context, draws the header, content, and navigation
 * panels in sequence.
 *
 * @param a0 Tips state pointer.
 * @param a1 Display list pointer.
 * @param a2 OT pointer.
 * @return Updated OT pointer, or a2 if no active display.
 */
s32 func_801E67F4(s32 a0, s32 a1, s32 a2) {
    s32 state = a0;
    s32 disp = a1;
    s32 ot = a2;

    if (*(s16 *)(state + 0x2A) != 0) {
        func_801F1AFC();
        func_8002E7C4(*(s16 *)(state + 0x24));
        func_80030058(*(s16 *)(state + 0x24));
        ot = func_801E6768(disp, ot);
        ot = func_801E6668(disp, ot);
        ot = func_801E6514(disp, ot);
        func_801F1B10();
    }
    return ot;
}

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E688C);

INCLUDE_ASM("asm/ovl/menutips/nonmatchings/menutips", func_801E696C);
