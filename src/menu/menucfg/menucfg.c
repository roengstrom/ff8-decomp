#include "common.h"
#include "menu.h"
#include "menucfg.h"
#include "input/gamepad.h"
#include "psxsdk/libgte.h"

/** One row of the config table @c D_801E7094: an option id and its state. */
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

/** The config screen's context block. The two trailing bytes gate whether an
    option is offered: @c flag_2D is cleared when no battle animation is running,
    @c flag_2E when the analog-axis probe reports no pad. */
typedef struct {
    u8 unk00[0x2D];
    u8 flag_2D;
    u8 flag_2E;
} CfgContext;

/** Terminator id closing the config option table. */
#define CFG_ENTRY_END 0xFF

/** The config option table, CFG_ENTRY_END-terminated. */
extern CfgEntry D_801E7094[];

/* All file-local; the overlay is a single translation unit and only
   func_801E5800 is reached from outside it. */
static s32  func_801E5820(CfgContext *arg0);
static void func_801E587C(CfgContext *cfg);
static void func_801E58EC(s32 a0, s32 a1);
static void func_801E5918(s32 a0, s32 a1, s32 a2);
static u8  *func_801E59A0(s32 a0);
static u8  *func_801E59CC(s32 a0);
static void func_801E61A0(u8 *text, s32 data, s32 value, s32 x, s32 y);
static s32  func_801E67A8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4);

/** @brief Config menu entry point — delegates to func_801F798C. */
void func_801E5800(s32 a0) {
    func_801F798C();
}

/**
 * @brief Count available config menu entries.
 *
 * Walks the @c D_801E7094 table to its @c CFG_ENTRY_END terminator, counting
 * entries that are offered: every one of them when @c flag_2E is set, otherwise
 * only those whose state is not 1.
 *
 * @param arg0 Config menu context.
 * @return Number of available config entries.
 */
static s32 func_801E5820(CfgContext *arg0)
{
    s32 count;
    s32 endId;
    CfgEntry *entry;
    s32 flag;
    s32 stateOn;
    count = 0;
    if (D_801E7094[0].id != CFG_ENTRY_END)
    {
        flag = arg0->flag_2E;
        stateOn = 1;
        endId = CFG_ENTRY_END;
        /* The cached endId/stateOn and this empty statement are load-bearing:
           spelling the loop with plain literals costs the match. */
        do { } while (0);
        entry = D_801E7094;
        do
        {
            if ((flag != 0) || (entry->state != stateOn))
            {
                count++;
            }
            entry++;
        }
        while (entry->id != endId);
    }
    return count;
}

/**
 * @brief Initialize config menu availability flags.
 *
 * Sets both availability flags, then clears whichever the hardware does not
 * support. @c GetAnalogStickAxis reads an analog axis, so a negative result means no
 * analog pad answered and @c flag_2E is cleared. @c flag_2D is cleared unless a
 * battle animation is both active and reports a nonzero field 0x0B.
 *
 * @param cfg Config menu context.
 */
static void func_801E587C(CfgContext *cfg) {
    s32 val = 1;
    cfg->flag_2E = val;
    cfg->flag_2D = val;
    if (GetAnalogStickAxis(0, PAD_AXIS_X2, 0) < 0) {
        cfg->flag_2E = 0;
    }
    if (IsFirstGamePadConnected() == 0 || GetActuatorCountForGamePad(0) == 0) {
        cfg->flag_2D = 0;
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
static void func_801E58EC(s32 a0, s32 a1) {
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
 */
static void func_801E5918(s32 a0, s32 a1, s32 a2) {
    a2 = D_801FA3C8[a2 / 64];
    a2 = a2 * 150 / 4096;
    func_801F0A34(a0, 0, a2 + 0x5A, D_801E7094[a1].unk03 + 0x2F);
}

/** @brief Look up string @p a0 in menu text category 2. */
static u8 *func_801E59A0(s32 a0) {
    return func_801F08D4(1, 2, a0, 0);
}

/** @brief Look up string @p a0 in menu text category 2, with the last flag set. */
static u8 *func_801E59CC(s32 a0) {
    return func_801F08D4(1, 2, a0, 1);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E59F8);

/**
 * @brief Render a bordered panel at the given position.
 *
 * If @p text is set, draws it with func_801F0FEC at (x+10, y+7), which
 * advances @p value. Then configures g_menuDisplayCfg
 * with the given position (fixed size 0xF4 x 0x16, iconType=0x55,
 * iconSubType=0) and calls DrawMenuWindow to draw the panel.
 *
 * @param text Text to draw in the panel, or NULL for none.
 * @param data Passed on to the rendering functions.
 * @param value Value passed to rendering functions.
 * @param x X position of the panel.
 * @param y Y position of the panel.
 */
static void func_801E61A0(u8 *text, s32 data, s32 value, s32 x, s32 y)
{
    MenuDisplayConfig *s = &g_menuDisplayCfg;
    s32 xoff = x + 10;
    s32 yoff = y + 7;

    if (text != NULL)
    {
        value = func_801F0FEC(data, value, xoff, yoff, text, 7);
    }

    s->iconType = 0x55;
    s->iconSubType = 0;
    s->x = x;
    s->w = 0xF4;
    s->y = y;
    s->h = 0x16;

    DrawMenuWindow(data, value, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E625C);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6438);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6538);

/**
 * @brief Configure display panel and invoke rendering callback.
 *
 * Sets up g_menuDisplayCfg with the given position, fixed size (0x11C x 0x25),
 * clears icon fields, and calls DrawMenuWindow with g_menuTint[MENU_TINT_NORMAL]
 * and a caller-supplied 0x1000 parameter.
 *
 * @param a0 Unused.
 * @param a1 First parameter passed through to DrawMenuWindow.
 * @param a2 Second parameter passed through to DrawMenuWindow.
 * @param a3 X position for the display panel.
 * @param arg4 Y position for the display panel.
 */
static s32 func_801E67A8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    s32 cfg = (s32)&g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)(cfg + 0) = a3;
    *(s16 *)(cfg + 4) = 0x11C;
    *(s16 *)(cfg + 6) = 0x25;
    *(s16 *)(cfg + 2) = arg4;
    return DrawMenuWindow(a1, a2, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6804);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E68E4);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E698C);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6D34);

INCLUDE_ASM("asm/ovl/menucfg/nonmatchings/menucfg", func_801E6E58);
