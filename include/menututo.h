/**
 * @file menututo.h
 * @brief Public symbols and types owned by the menututo overlay.
 *
 * The menututo overlay (loaded at @c 0x801E2800) implements the
 * in-game tutorial submenu. This header declares the structs used
 * by the tutorial state machine and externs the data tables (in
 * menututo's own data segment) plus the menu-system functions and
 * globals (in menumain) that menututo links against.
 *
 * Cross-overlay shared types (e.g. @c MenuDisplayConfig) live in
 * @c include/menu.h.
 */
#ifndef MENUTUTO_H
#define MENUTUTO_H

#include "common.h"
#include "menu.h"

/* ======================================================================== */
/* Types                                                                    */
/* ======================================================================== */

/**
 * @brief Tutorial callback context structure.
 *
 * Allocated by func_801F179C from the menumain callback pool. The first
 * 0x10 bytes are owned by the menu allocator (free-list links + the two
 * function pointer callbacks); fields from 0x10 onwards are the
 * tutorial-specific state.
 */
typedef struct {
    /* 0x00 */ s32 allocNext;        /**< Allocator free-list next pointer. */
    /* 0x04 */ s32 allocPrev;        /**< Allocator free-list prev pointer. */
    /* 0x08 */ s32 cbMain;           /**< Main callback (func_801E3140). */
    /* 0x0C */ s32 cbRender;         /**< Render callback (func_801E4598). */
    /* 0x10 */ u16 state;            /**< State machine current state. */
    /* 0x12 */ u8 pad12[4];          /**< Unknown — touched by func_801F179C? */
    /* 0x16 */ u16 returnState;      /**< Stashed state to resume after a sub-state. */
    /* 0x18 */ u8 pad18[8];          /**< Unknown. */
    /* 0x20 */ u16 fadeAlpha;        /**< Panel fade-in alpha (counts up to 0x1000). */
    /* 0x22 */ U16Split pageIndex;   /**< Page index (lo=overlayCmd, hi=overlayParam). */
    /* 0x24 */ s32 panelHandle;      /**< Panel handle returned by func_801F08D4 / func_801F6AD0. */
    /* 0x28 */ s16 fadePos;          /**< Fade animation position (counts up to 0x1000). */
    /* 0x2A */ s16 scrollPos;        /**< Section-list scroll position (counts down from 0x1000). */
    /* 0x2C */ s16 fadeProgress;     /**< Section-select fade progress. */
    /* 0x2E */ s16 scrollAnim;       /**< Page-change scroll animation offset. */
    /* 0x30 */ u8 targetPage;        /**< Target page after scroll-page transition. */
    /* 0x31 */ u8 prevPage;          /**< Page before scroll-page transition. */
    /* 0x32 */ s8 entryIndex;        /**< Currently selected entry (page * 10 + slot). */
    /* 0x33 */ s8 pageCount;         /**< Total page count (computed from D_800780AB). */
    /* 0x34 */ u8 sectionIndex;      /**< Currently selected section index. */
    /* 0x35 */ u8 cursorPos;         /**< Cursor position within section list. */
    /* 0x36 */ u8 availCount;        /**< Number of available section entries. */
    /* 0x37 */ u8 loadParam;         /**< Saved bgParam for reload. */
    /* 0x38 */ u8 isReentry;         /**< 0 = cold entry, 1 = warm entry. */
    /* 0x39 */ u8 availSlots[9];     /**< Indices of available section entries. */
} TutoState;

/** @brief Tutorial entry (4 bytes). */
typedef struct {
    u8 panelId;
    u8 hasFlag;
    u8 loadCmd;
    u8 pad03;
} TutoEntry;

/** @brief Tutorial section entry (12 bytes). */
typedef struct {
    u8 sectionId;
    u8 bgParam;
    u8 overlayCmd;
    u8 gfDataId;
    u8 charDataId;
    u8 subOvlId1;
    u8 subOvlId2;
    u8 saveFlag;
    u8 fieldFlagId;
    u8 pad09[3];
} TutoSectionEntry;

/* ======================================================================== */
/* menututo data (lives in this overlay's data segment)                     */
/* ======================================================================== */

/** @brief Tutorial entry table (panel + flag + load command per section). */
extern TutoEntry D_801E4E18[];

/** @brief Tutorial section table (section id, gf/char data ids, overlay cmds). */
extern TutoSectionEntry D_801E4E3C[];

/** @brief Tutorial fade panel preset passed to func_801F1D34. */
extern u8 D_801E4EA8[];

/** @brief Tutorial page table; 4-byte stride, two bytes copied per row. */
extern u8 D_801E4EB4[];

/** @brief Page sequence table terminated by 0xFFFF, scanned by func_801E48C0. */
extern u16 D_801E4EAC[];

/** @brief Tutorial page index byte (set on page change). */
extern u8 D_801E4EC0;

/** @brief Tutorial column index 1 (read by func_801E2800). */
extern u8 D_801E4EC1;

/** @brief Tutorial column index 2 (read by func_801E2810). */
extern u8 D_801E4EC2;

/* ======================================================================== */
/* External shared menu globals (live in menumain overlay)                  */
/* ======================================================================== */

/** @brief Tutorial save flag controlling the party-availability scan. */
extern u8 D_801FABC7;

/* ======================================================================== */
/* External menu functions (defined in menumain overlay)                    */
/* ======================================================================== */

extern s32 func_801EF9AC(s32, s32, s32, s32);
extern u32 func_801EFBB4(s32, s32, s32);
extern s32 func_801F7A54(void);
extern s32 drawColorByMenuPalette(s32, s32, s32, s32, s32);

/* ======================================================================== */
/* Forward declarations (defined within menututo.c)                         */
/* ======================================================================== */

void func_801E48C0(TutoState *self);
s32 func_801E4BD0(s32, s32, s32);

#endif /* MENUTUTO_H */
