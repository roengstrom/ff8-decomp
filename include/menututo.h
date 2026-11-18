/**
 * @file menututo.h
 * @brief Public symbols and types owned by the menututo overlay.
 *
 * The menututo overlay (loaded at @c 0x801E2800) implements the
 * in-game tutorial submenu. This header declares the structs used
 * by the tutorial state machine and externs the data tables in
 * menututo's own data segment.
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
 * The first 0x10 bytes are owned by the menu allocator (free-list links
 * and the two callbacks); fields from 0x10 onwards are the
 * tutorial-specific state.
 */
typedef struct {
    /** 0x00 */ s32 allocNext; // allocator free-list next pointer
    /** 0x04 */ s32 allocPrev; // allocator free-list prev pointer
    /** 0x08 */ s32 cbMain;
    /** 0x0C */ s32 cbRender;
    /** 0x10 */ u16 state;
    /** 0x12 */ u8 pad12[4];
    /** 0x16 */ u16 returnState; // state to resume after a sub-state
    /** 0x18 */ u8 pad18[8];
    /** 0x20 */ u16 fadeAlpha; // panel fade-in alpha, counts up to 0x1000
    /** 0x22 */ U16Split pageIndex; // lo = overlayCmd, hi = overlayParam
    /** 0x24 */ u8 *panelHandle; // message text
    /** 0x28 */ s16 fadePos; // fade animation position, counts up to 0x1000
    /** 0x2A */ s16 scrollPos; // section-list scroll position, counts down from 0x1000
    /** 0x2C */ s16 fadeProgress; // section-select fade progress
    /** 0x2E */ s16 scrollAnim; // page-change scroll animation offset
    /** 0x30 */ u8 targetPage; // page after a scroll-page transition
    /** 0x31 */ u8 prevPage; // page before a scroll-page transition
    /** 0x32 */ s8 entryIndex; // selected entry, page * 10 + slot
    /** 0x33 */ s8 pageCount;
    /** 0x34 */ u8 sectionIndex;
    /** 0x35 */ u8 cursorPos; // cursor position within the section list
    /** 0x36 */ u8 availCount; // number of available section entries
    /** 0x37 */ u8 loadParam; // saved bgParam for reload
    /** 0x38 */ u8 isReentry; // 0 = cold entry, 1 = warm entry
    /** 0x39 */ u8 availSlots[9]; // indices of available section entries
} TutoState;

/** @brief Tutorial entry (4 bytes). */
typedef struct {
    /** 0x00 */ u8 panelId;
    /** 0x01 */ u8 hasFlag;
    /** 0x02 */ u8 loadCmd;
    /** 0x03 */ u8 pad03;
} TutoEntry;

/** @brief Tutorial section entry (12 bytes). */
typedef struct {
    /** 0x00 */ u8 sectionId;
    /** 0x01 */ u8 bgParam;
    /** 0x02 */ u8 overlayCmd;
    /** 0x03 */ u8 gfDataId;
    /** 0x04 */ u8 charDataId;
    /** 0x05 */ u8 subOvlId1;
    /** 0x06 */ u8 subOvlId2;
    /** 0x07 */ u8 saveFlag;
    /** 0x08 */ u8 fieldFlagId;
    /** 0x09 */ u8 pad09[3];
} TutoSectionEntry;

/** @brief Tutorial page table entry (4 bytes). */
typedef struct {
    /** 0x00 */ u8 unk0;
    /** 0x01 */ u8 unk1;
    /** 0x02 */ u8 pad2[2];
} TutoPageEntry;

/* ======================================================================== */
/* menututo data (lives in this overlay's data segment)                     */
/* ======================================================================== */

/** @brief Tutorial entry table (panel + flag + load command per section). */
extern TutoEntry D_801E4E18[];

/** @brief Tutorial section table (section id, gf/char data ids, overlay cmds). */
extern TutoSectionEntry D_801E4E3C[];

/** @brief Tutorial fade panel preset. */
extern u8 D_801E4EA8[];

/** @brief Tutorial page table. */
extern TutoPageEntry D_801E4EB4[];

/** @brief Page sequence table terminated by 0xFFFF. */
extern u16 D_801E4EAC[];

/** @brief Tutorial page index byte. */
extern u8 D_801E4EC0;

/** @brief Tutorial column index 1. */
extern u8 D_801E4EC1;

/** @brief Tutorial column index 2. */
extern u8 D_801E4EC2;

#endif /* MENUTUTO_H */
