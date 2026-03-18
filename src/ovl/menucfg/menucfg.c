#include "common.h"

/** @brief Config menu entry point — delegates to func_801F798C. */
void func_801E5800(s32 a0) {
    func_801F798C(a0);
}

typedef struct {
    u8 id;
    u8 state;
    u8 unk02;
    u8 unk03;
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
} CfgEntry;

typedef struct {
    u8 unk00[0x2E];
    u8 flag_2E;
} CfgContext;

extern CfgEntry D_801E7094[];

/**
 * @brief Count available config menu entries.
 *
 * Iterates through the D_801E7094 config entry table (8-byte stride,
 * 0xFF-terminated). Counts entries where either flag_2E is set or the
 * entry's state is not 1.
 *
 * @param arg0 Pointer to config menu context (byte 0x2E is availability flag).
 * @return Number of available config entries.
 */
s32 func_801E5820(CfgContext *arg0)
{
    int count;
    int new_var;
    CfgEntry *entry;
    s32 flag;
    s32 one;
    count = 0;
    if (D_801E7094[0].id != 0xFF)
    {
        flag = arg0->flag_2E;
        one = 1;
        new_var = 0xFF;
        do { } while (0);
        entry = D_801E7094;
        do
        {
            if ((flag != 0) || (entry->state != one))
            {
                count++;
            }
            entry++;
        }
        while (entry->id != new_var);
    }
    return count;
}

/**
 * @brief Initialize config menu availability flags.
 *
 * Sets both config availability flags at @p a0[0x2D] and @p a0[0x2E]
 * to 1, then validates them. Calls func_80027DB4(0,0,0) to check
 * memory card status; if negative, clears a0[0x2E]. Then calls
 * func_80028098(); if it returns 0, clears a0[0x2D]. If nonzero,
 * calls func_800275A8(0) and clears a0[0x2D] if that returns 0.
 *
 * @param a0 Pointer to config menu context.
 */
void func_801E587C(u8 *a0) {
    s32 val = 1;
    a0[0x2E] = val;
    a0[0x2D] = val;
    if (func_80027DB4(0, 0, 0) < 0) {
        a0[0x2E] = 0;
    }
    if (func_80028098() == 0 || func_800275A8(0) == 0) {
        a0[0x2D] = 0;
    }
}

/**
 * @brief Render a config option label at a grid position.
 *
 * Computes y-coordinate from @p a1 (row * 16 + 0x24) and renders
 * a string of width 0x30 at that position via func_801F0A34.
 *
 * @param a0 Render context pointer.
 * @param a1 Row index (0-based).
 */
void func_801E58EC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x30, a1 * 16 + 0x24);
}

/**
 * @brief Render a config option value at a computed grid position.
 *
 * Computes a horizontal offset by looking up a volume/level table entry
 * from D_801FA3C8 (indexed by a2 / 64), scaling it by 150/4096, and
 * adding 0x5A. Reads the y-offset from the D_801E7094 config entry table
 * at index a1 (byte at offset 3) and adds 0x2F. Then renders via
 * func_801F0A34.
 *
 * @param a0 Render context pointer.
 * @param a1 Config entry index.
 * @param a2 Raw value (divided by 64 to index the table).
 *
 * @note Non-matching: Compiler schedules `sw ra` into bgez delay slot
 * instead of `addu v1, a2, zero`. Original copies a2 to v1 for the
 * division rounding in the delay slot; compiled version defers this.
 * Result is 1 instruction shorter than original (33 vs 34 instructions).
 */
INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E5918);

/** @brief Draw inner panel with section id 0x2 and clear flag. */
s32 func_801E59A0(s32 a0) {
    return func_801F08D4(1, 2, a0, 0);
}

/** @brief Draw inner panel with section id 0x2 and set flag. */
s32 func_801E59CC(s32 a0) {
    return func_801F08D4(1, 2, a0, 1);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E59F8);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E61A0);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E625C);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6438);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6538);

/**
 * @brief Configure display panel and invoke rendering callback.
 *
 * Sets up D_801FAB00 with the given position, fixed size (0x11C x 0x25),
 * clears icon fields, and calls func_801EF9AC with D_80083848 and a
 * caller-supplied 0x1000 parameter.
 *
 * @param a0 Unused.
 * @param a1 First parameter passed through to func_801EF9AC.
 * @param a2 Second parameter passed through to func_801EF9AC.
 * @param a3 X position for the display panel.
 * @param arg4 Y position for the display panel.
 */
void func_801E67A8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    extern u8 D_801FAB00[];
    extern u8 D_80083848[];
    s32 cfg = (s32)D_801FAB00;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)(cfg + 0) = a3;
    *(s16 *)(cfg + 4) = 0x11C;
    *(s16 *)(cfg + 6) = 0x25;
    *(s16 *)(cfg + 2) = arg4;
    func_801EF9AC(a1, a2, 0x1000, *(s32 *)D_80083848);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6804);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E68E4);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E698C);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6D34);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6E58);
