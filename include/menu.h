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
    /* 0x1A */ u16 inputRepeat;   /**< Held buttons with auto-repeat applied (func_801F0E5C). */
    /* 0x1C */ u16 inputNew;      /**< Newly pressed buttons: (cur ^ prev) & cur. */
    /* 0x1E */ u8 itemId;         /**< Current item/card ID for rendering */
    /* 0x1F */ u8 itemAttr;       /**< Item attribute byte */
    /* 0x20 */ s32 dataPtr;       /**< Pointer to item data array */
} MenuDisplayConfig; /* 0x24 bytes */

/** @brief Shared menu display state (lives in menumain BSS at 0x801FAB00). */
extern MenuDisplayConfig g_menuDisplayCfg;

/** @brief Current menu text color (RGB packed, lives in main exe data at 0x80083848). */
extern s32 g_menuColor;

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
    /* 0x0A */ u8 abilityRows;            /**< Ability rows to render (was GfCompatEntry.count @D_801EEDF0). */
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
    /* 0x20 */ s32 itemPtr;            /**< Item/ability data pointer passed to rendering callbacks. */
    /* 0x24 */ s32 itemPtr2;           /**< Secondary item/ability data pointer. */
    /* 0x28 */ s32 dataPtr;            /**< Pointer to ability data table. */
    /* 0x2C */ s32 dataPtr2;           /**< Magic-availability mask / secondary data pointer. */
    /* 0x30 */ u16 parentParam;        /**< Parameter from parent menu context. */
    /* 0x32 */ u16 discId;             /**< Current disc / GF availability mask. */
    /* 0x34 */ s16 unk34;              /**< Scroll offset source (copied to g_menuDisplayCfg.scrollOffset). */
    /* 0x36 */ s16 unk36;              /**< Panel animation scale. */
    /* 0x38 */ s16 unk38;              /**< Offset added to Y position in stat panel rendering. */
    /* 0x3A */ s16 unk3A;              /**< Fade/transition scale value. */
    /* 0x3C */ s16 statScale;          /**< Stat scale value (0x1000 = 1.0). */
    /* 0x3E */ s16 slideOffset;        /**< Character-switch panel slide offset (±0xF80, stepped toward 0). */
    /* 0x40 */ s16 unk40;              /**< Unknown s16 (scaling factor, similar to statScale). */
    /* 0x42 */ u8 unk42;               /**< Panel display mode. */
    /* 0x43 */ u8 charIdx;             /**< Selected character index (0-7). */
    /* 0x44 */ u8 unk44;               /**< Page/row index for rendering. */
    /* 0x45 */ u8 unk45;               /**< Page offset for rendering. */
    /* 0x46 */ u8 statInfo[4];         /**< Per-stat info bytes; statInfo[1] = junction slot count. */
    /* 0x4A */ s8 unk4A;               /**< Auto-junction (magic) menu cursor index. */
    /* 0x4B */ s8 unk4B;               /**< Magic/junction list cursor index. */
    /* 0x4C */ s8 unk4C;               /**< Auto-junction mode cursor index. */
    /* 0x4D */ u8 pad4D;               /**< Padding. */
    /* 0x4E */ s8 unk4E;               /**< Stat index for delta bar rendering. */
    /* 0x4F */ s8 statSlot;             /**< Current stat slot index (-1 = none). */
    /* 0x50 */ s8 unk50;               /**< Magic list cursor position. */
    /* 0x51 */ u8 pad51;               /**< Padding. */
    /* 0x52 */ s8 statByte[4];         /**< Per-type ability page slot values (signed; index by unk56). */
    /* 0x56 */ u8 unk56;               /**< Ability list type (0=none, 1=commands, 2=abilities). */
    /* 0x57 */ u8 unk57;               /**< Ability count for current navigation. */
    /* 0x58 */ u8 unk58;               /**< Header panel category index. */
    /* 0x59 */ u8 unk59;               /**< Junction slot count for current type. */
    /* 0x5A */ u8 unk5A;               /**< Stat display type byte. */
    /* 0x5B */ u8 discCount;           /**< Number of discs (from popcount). */
    /* 0x5C */ s8 unk5C;               /**< GF navigation index. */
    /* 0x5D */ u8 prevCharIdx;         /**< Outgoing character during the character-switch slide. */
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
    /* 0x04 */ u8 level;              /**< GF level (from g_battleChars.levelEntries). */
    /* 0x05 */ u8 charIdx;            /**< Character this GF is junctioned to. */
    /* 0x06 */ u8 cmdSlotCount;       /**< Command slot count from this GF. */
    /* 0x07 */ u8 ablSlotCount;       /**< Ability slot count from this GF. */
    /* 0x08 */ u8 maxAbilitySlots;    /**< Max ability slots from this GF. */
    /* 0x09 */ u8 pad09[3];           /**< Padding. */
} JunctionGfEntry; /* 0x0C = 12 bytes */

/**
 * @brief One stat row of the junction slot-detail table (9 entries, HP..LCK).
 *
 * statOffset indexes the battle character stat block (g_battleChars /
 * g_junctionPreview) in bytes; HP (entry 0) is a u16 stat, STR..LCK are u8.
 * junctionSlot is the JunctionType index of the stat's junction slot.
 */
typedef struct {
    /* 0x00 */ u16 statOffset;    /**< Byte offset of the stat within battle char data. */
    /* 0x02 */ u16 labelId;       /**< Stat label string id (0x130-0x138). */
    /* 0x04 */ u8 flags;          /**< 0x01 right value column, 0x20/0x40 percent transforms, 0x80 show max. */
    /* 0x05 */ u8 junctionSlot;   /**< JunctionType slot index for this stat. */
    /* 0x06 */ u8 pad06[2];       /**< Padding. */
} JunctionSlotDetail; /* 0x08 = 8 bytes */

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

/**
 * @brief Tutorial section entry count (g_gameState.mainData.tutoEntryCount alias).
 *
 * Number of available tutorial entries in the current section. Read by
 * the menututo overlay. Divided by 10 (rounded up) gives the page count.
 * Declared u8 to match the underlying gamestate field; cast to s8 in
 * specific contexts where the original code used signed-byte arithmetic.
 */
extern u8 D_800780AB;



/* menumain-owned shared symbols (canonical signatures from menumain.c). */
s32 func_801F6AFC(s32);
u32 func_801F0FEC(s32, s32, s32, s32, s32, s32);
s32 func_801F3FB4(s32);
s32 func_801F5104(s32);
s32 func_801F510C(s32);
s32 func_801F5144(s32);
s32 func_801F5F30(s32, s32, s32, s32, s32, s32);
s32 func_801F5F60(s32, s32, s32, s32);
s32 func_801F65F0(s32, s32, s32, s32, s32, s32);
s32 func_801F776C(s32, s32);
s32 func_801F79F8(s32);
s32 func_801F7BAC(s32);
s32 func_801F7BE4(s32);
extern u8 D_801EF1A4;
extern u8 D_801EF1A5;
extern u8 D_801EF1B0[];
extern u16 D_801FA3C8[];  /**< 65-entry falloff table, built by menumain func_801EFF64. */

#endif /* MENU_H */
