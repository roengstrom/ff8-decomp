#ifndef MENU_H
#define MENU_H

#include "common.h"

/**
 * @brief Display/rendering configuration for menu panels.
 *
 * Shared across menu overlays (menumain, menucrd, menutest, etc.).
 * Written before calling rendering callbacks (func_801EF9AC,
 * func_801EFBB4, func_801EF800) to configure the next panel draw.
 *
 * Lives at 0x801FAB00 in menumain BSS. Overlays reference it via
 * undefined_syms as an extern.
 */
typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 w;
    /* 0x06 */ s16 h;
    /* 0x08 */ s16 x2;            /**< Secondary X (computed by func_801EFBB4) */
    /* 0x0A */ s16 y2;            /**< Secondary Y */
    /* 0x0C */ s16 w2;            /**< Secondary width */
    /* 0x0E */ s16 h2;            /**< Secondary height */
    /* 0x10 */ u8 iconType;       /**< Panel icon ID (0x00=none, 0x4A, 0x52, 0x55) */
    /* 0x11 */ u8 iconSubType;    /**< Icon sub-parameter (always 0) */
    /* 0x12 */ u8 animCounter;    /**< Animation tick counter (aliased as D_801FAB12) */
    /* 0x13 */ u8 columnCount;    /**< Number of columns/items for rendering loop */
    /* 0x14 */ s16 scrollOffset;  /**< Scroll position passed to rendering callback */
    /* 0x16 */ u8 pageStart;      /**< First visible page/row index */
    /* 0x17 */ u8 pageEnd;        /**< Last visible page/row index */
    /* 0x18 */ u16 inputRaw;      /**< Raw button input state */
    /* 0x1A */ u16 inputNew;      /**< Newly pressed buttons (edges) */
    /* 0x1C */ u16 inputRepeat;   /**< Buttons with auto-repeat applied */
    /* 0x1E */ u8 itemId;         /**< Current item/card ID for rendering */
    /* 0x1F */ u8 itemAttr;       /**< Item attribute byte */
    /* 0x20 */ s32 dataPtr;       /**< Pointer to item data array */
} MenuDisplayConfig; /* 0x24 bytes */

#endif /* MENU_H */
