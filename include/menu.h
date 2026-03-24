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

/**
 * @brief Per-character junction menu state (g_junctionChars, stride 28).
 *
 * Tracks junction availability, cached character data, and backup
 * copies of commands/abilities for the junction menu UI.
 * Array of 8 entries (one per character).
 */
typedef struct {
    /* 0x00 */ u32 availFlags;            /**< Junction availability bitmask. */
    /* 0x04 */ u16 currentHp;             /**< Cached character HP. */
    /* 0x06 */ u16 junctedGfs;            /**< Cached juncted GFs bitmask. */
    /* 0x08 */ u8 abilityCount[2];        /**< Ability counts per sub-slot. */
    /* 0x0A */ u8 unk0A;                  /**< Unknown. */
    /* 0x0B */ u8 gfCompat;               /**< GF compatibility byte. */
    /* 0x0C */ u8 commandsBackup[2][4];   /**< Command backups (2 sub-slots × 4). */
    /* 0x14 */ u8 abilitiesBackup[2][4];  /**< Ability backups (2 sub-slots × 4). */
} JunctionMenuEntry; /* 0x1C = 28 bytes */

/**
 * @brief Junction menu context (passed to most menujnc2 functions).
 *
 * Contains UI state for the junction menu screen: selected character,
 * current slot, navigation state, and display parameters.
 * @note Many fields still unmapped — extend as functions are decomped.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x10];         /**< Unknown. */
    /* 0x10 */ u16 state;              /**< Current state machine state (0x00-0x49). */
    /* 0x12 */ u8 pad12[4];            /**< Unknown. */
    /* 0x16 */ u16 returnState;        /**< State to return to after sub-menu. */
    /* 0x18 */ u16 exitState;          /**< State to transition to on exit. */
    /* 0x1A */ u8 pad1A[6];            /**< Unknown. */
    /* 0x20 */ u8 itemData[8];         /**< Item/ability data passed to rendering callbacks. */
    /* 0x28 */ s32 dataPtr;            /**< Pointer to ability data table. */
    /* 0x2C */ s32 unk2C;               /**< Secondary data pointer (for ability list callbacks). */
    /* 0x30 */ u16 parentParam;        /**< Parameter from parent menu context. */
    /* 0x32 */ u16 discId;             /**< Current disc ID. */
    /* 0x34 */ u16 unk34;               /**< Scroll offset source (copied to g_menuDisplayCfg.scrollOffset). */
    /* 0x36 */ s16 unk36;              /**< Panel animation scale. */
    /* 0x38 */ s16 unk38;              /**< Offset added to Y position in stat panel rendering. */
    /* 0x3A */ s16 unk3A;              /**< Fade/transition scale value. */
    /* 0x3C */ s16 statScale;          /**< Stat scale value (0x1000 = 1.0). */
    /* 0x3E */ u16 unk3E;              /**< Stat delta value for scaling panel. */
    /* 0x40 */ s16 unk40;              /**< Unknown s16 (scaling factor, similar to statScale). */
    /* 0x42 */ u8 unk42;               /**< Panel display mode. */
    /* 0x43 */ u8 charIdx;             /**< Selected character index (0-7). */
    /* 0x44 */ u8 unk44;               /**< Page/row index for rendering. */
    /* 0x45 */ u8 unk45;               /**< Page offset for rendering. */
    /* 0x46 */ u8 unk46;               /**< Slot navigation mode. */
    /* 0x47 */ u8 slotCount;           /**< Number of junction slots (2 or 3). */
    /* 0x48 */ u8 unk48;               /**< Ability navigation limit. */
    /* 0x49 */ u8 pad49;               /**< Unknown. */
    /* 0x4A */ s8 unk4A;               /**< Stat delta column index. */
    /* 0x4B */ s8 unk4B;               /**< Sub-slot selection index. */
    /* 0x4C */ s8 unk4C;               /**< Auto-junction table index. */
    /* 0x4D */ u8 pad4D;               /**< Unknown. */
    /* 0x4E */ s8 unk4E;               /**< Stat index for delta bar rendering. */
    /* 0x4F */ s8 statSlot;             /**< Current stat slot index (-1 = none). */
    /* 0x50 */ s8 unk50;               /**< Magic list cursor position. */
    /* 0x51 */ u8 unk51;               /**< Unknown. */
    /* 0x52 */ u8 unk52;               /**< Ability page slot value (per-type). */
    /* 0x53 */ u8 unk53;               /**< Ability page slot value 2. */
    /* 0x54 */ u8 unk54;               /**< Unknown. */
    /* 0x55 */ u8 unk55;               /**< Unknown. */
    /* 0x56 */ u8 unk56;               /**< Ability list type (0=none, 1=commands, 2=abilities). */
    /* 0x57 */ u8 unk57;               /**< Ability count for current navigation. */
    /* 0x58 */ u8 unk58;               /**< Header panel category index. */
    /* 0x59 */ u8 unk59;               /**< Junction slot count for current type. */
    /* 0x5A */ u8 unk5A;               /**< Stat display type byte. */
    /* 0x5B */ u8 discCount;           /**< Number of discs (from func_80035A6C). */
    /* 0x5C */ s8 unk5C;               /**< GF navigation index. */
    /* 0x5D */ u8 unk5D;               /**< Character index for scaling panel comparison. */
    /* 0x5E */ s8 unk5E;               /**< Column/stat selection index. */
    /* 0x5F */ u8 unk5F;               /**< GF ability slot count. */
    /* 0x60 */ u8 unk60;               /**< Junction modification flags. */
    /* 0x61 */ u8 unk61;               /**< Modified flag (set when junction changed). */
    /* 0x62 */ u8 unk62;               /**< Sub-mode flag. */
    /* 0x63 */ u8 unk63;               /**< Unjunction-all flag. */
    /* 0x64 */ u16 unk64;              /**< GF confirm timer. */
    /* 0x66 */ u16 unk66;              /**< Message display timer. */
} JunctionMenuCtx; /* 0x68 bytes */

/**
 * @brief Parent menu context passed to junction menu entry points.
 *
 * Passed from the main menu system into the junction overlay.
 * @note Only fields accessed by menujnc2 are mapped.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x20];         /**< Unknown. */
    /* 0x20 */ u16 param;              /**< Menu parameter (copied to JunctionMenuCtx.parentParam). */
    /* 0x22 */ u8 charIdx;             /**< Character index to junction. */
    /* 0x23 */ u8 pad23;               /**< Unknown. */
} MenuParentCtx; /* 0x24 bytes (minimum) */

/** @brief Junction menu slot type IDs (passed to rendering dispatch). */
#define JUNC_SLOT_DEF_ELEM   0xB  /**< Defense element junction slots. */
#define JUNC_SLOT_DEF_STATUS 0xC  /**< Defense status junction slots. */

/**
 * @brief Auto-junction mode selection.
 *
 * Indexes into g_autoJunctionPriority[] to select which stat priority
 * ordering to use when auto-junctioning magic. Each mode fills stats
 * in a different order:
 *   - ATK: STR, SPD, EVA, HIT, LCK first
 *   - MAG: MAG, SPD, HP first
 *   - DEF: HP, VIT, SPR, EVA first
 */
typedef enum {
    AUTO_JUNC_ATK = 0,  /**< Attack-focused: prioritizes offensive stats. */
    AUTO_JUNC_MAG = 1,  /**< Magic-focused: prioritizes magic stats. */
    AUTO_JUNC_DEF = 2   /**< Defense-focused: prioritizes defensive stats. */
} AutoJunctionMode;

/**
 * @brief GF entry in junction menu state (stride 12).
 *
 * Each entry corresponds to one of the 16 GFs. Stores the GF's
 * junction ability flags and slot counts for the junction menu.
 */
typedef struct {
    /* 0x00 */ u32 abilityFlags;      /**< OR'd into g_junctionChars[].availFlags. */
    /* 0x04 */ u8 pad04;              /**< Unknown. */
    /* 0x05 */ u8 charIdx;            /**< Character this GF is junctioned to. */
    /* 0x06 */ u8 cmdSlotCount;       /**< Command slot count from this GF. */
    /* 0x07 */ u8 ablSlotCount;       /**< Ability slot count from this GF. */
    /* 0x08 */ u8 maxAbilitySlots;    /**< Max ability slots from this GF. */
    /* 0x09 */ u8 pad09[3];           /**< Padding. */
} JunctionGfEntry; /* 0x0C = 12 bytes */

/**
 * @brief Character display info for menu rendering (stride 32).
 *
 * Cached summary of character data used by the menu system.
 * Array of 8 entries indexed by character ID, located at D_801FABC8.
 */
typedef struct {
    /* 0x00 */ u8 pad00[8];        /**< Unknown. */
    /* 0x08 */ s16 currentHp;      /**< Cached current HP. */
    /* 0x0A */ s16 unk0A;          /**< Unknown stat value. */
    /* 0x0C */ u8 unk0C;           /**< Unknown byte. */
    /* 0x0D */ u8 pad0D;           /**< Padding. */
    /* 0x0E */ u16 statusFlags;    /**< Character status flags (synced from CharacterData). */
    /* 0x10 */ u8 pad10[2];        /**< Unknown. */
    /* 0x12 */ u8 unk12;           /**< Unknown byte. */
    /* 0x13 */ u8 pad13[13];       /**< Unknown. */
} CharMenuInfo; /* 0x20 = 32 bytes */

extern CharMenuInfo g_charMenuInfo[];

#endif /* MENU_H */
