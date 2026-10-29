#include "ability.h"
#include "ability_list.h"
#include "battle.h"
#include "battle_results/number.h"
#include "btl_anim.h"
#include "btl_anim_packet.h"
#include "card.h"
#include "common.h"
#include "game.h"
#include "gamestate.h"
#include "gf.h"
#include "kernel.h"
#include "menu.h"
#include "numstr.h"
#include "overlay.h"
#include "party_stats.h"
#include "psxsdk/libetc.h"
#include "snd_sfx.h"
#include "ui/dialog.h"
#include "ui/font.h"
#include "ui/icon.h"
#include "ui/sprite.h"
#include "ui/text.h"
#include "ui/window.h"

#define ALL_GFS 0xFF /**< Targets all GFs. */
#define INVALID_ID 0xFF /**< Invalid or absent ID. */
#define INVALID_BUTTON_ID 0xFFFF /** Invalid or absent button ID. */

enum {
    STRING_TO_SCROLL_TO_GO_TO_ITEM_SCREEN = 22,
};

/** @brief Item flags. */
enum {
    ITEM_FLAG_USABLE_IN_MENU   = 0x01,
    ITEM_FLAG_TARGET_CHARA     = 0x02,
    ITEM_FLAG_TARGET_GF        = 0x04,
    ITEM_FLAG_TARGET_ALL       = 0x08,
    ITEM_FLAG_TARGET_ALL_BOTH  = 0x10,
    ITEM_FLAG_TARGET_DEAD      = 0x20,
    ITEM_FLAG_QUISTIS_ONLY     = 0x40,
    ITEM_FLAG_GF_COMPAT_OTHERS = 0x80,
};

/** @brief Item types. */
enum {
    ITEM_TYPE_HEAL_CHARA        = 0,
    ITEM_TYPE_REVIVE_CHARA      = 1,
    ITEM_TYPE_REVIVE_FULL_CHARA = 2,
    ITEM_TYPE_HEAL_GF           = 3,
    ITEM_TYPE_REVIVE_GF         = 4,
    ITEM_TYPE_REVIVE_FULL_GF    = 5,
    ITEM_TYPE_FULL_RESTORE      = 6,
    ITEM_TYPE_BATTLE_ONLY       = 7,
    ITEM_TYPE_AMMO              = 8,
    ITEM_TYPE_MAGAZINE          = 9,
    ITEM_TYPE_REFINE_ONLY       = 10,
    ITEM_TYPE_RENAME_CARD       = 11,
    ITEM_TYPE_PET_NAMETAG       = 12,
    ITEM_TYPE_CHOCOBO_TAG       = 13,
    ITEM_TYPE_MAGICAL_LAMP      = 14,
    ITEM_TYPE_SOLOMON_RING      = 15,
    ITEM_TYPE_GF_COMPATIBILITY  = 16,
    ITEM_TYPE_TEACH_GF_ABILITY  = 17,
    ITEM_TYPE_FORGET_ABILITY    = 18,
    ITEM_TYPE_QUISTIS_LIMIT     = 19,
    ITEM_TYPE_STAT_UP           = 20,
    ITEM_TYPE_CURE_STATUS       = 21,
};

/** @brief Action item types (mapped through D_801EB1E4). */
enum {
    ACTION_ITEM_TYPE_HEAL              = 1,
    ACTION_ITEM_TYPE_REVIVE            = 2,
    ACTION_ITEM_TYPE_REVIVE_FULL       = 3,
    ACTION_ITEM_TYPE_FULL_RESTORE      = 4,
    ACTION_ITEM_TYPE_GF_COMPATIBILITY  = 5,
    ACTION_ITEM_TYPE_TEACH_GF_ABILITY  = 6,
    ACTION_ITEM_TYPE_STAT_UP           = 8,
    ACTION_ITEM_TYPE_CURE_STATUS       = 9,
};

/**
 * @brief Represents a string-location table (`strings_locationsXX.bin`) stored in `mngrp.bin`.
 *
 * The table contains offsets to strings relative to a common base address.
 * The number of offsets is specified by the `count` field.
 */
typedef struct {
    u16 count;      /**< 0x00: Number of strings. */
    u16 offsets[1]; /**< 0x02: String offsets. */
} Mngrp_string;

/**
 * @brief Picture/text overlay drawn in the magazine page.
 */
typedef struct {
    u16 x; /**< 0x00: X position. */
    u8 y;  /**< 0x02: Y position. */
    u8 id; /**< 0x03: ID. */
} MagazinePageOverlay; /* 0x4 bytes */

/**
 * @brief Magazine page. Each entry repesents one page view (mmag.bin content).
 */
typedef struct {
    u16 windowX; /**< 0x00: Window X. */
    u16 windowY; /**< 0x02: Window Y. */
    u16 windowWidth; /**< 0x04: Window width. */
    u16 windowHeight; /**< 0x06: Window height. */
    u16 paperMatX; /**< 0x08: Paper mat X. */
    u16 paperMatY; /**< 0x0A: Paper mat Y. */
    u16 paperMatWidth; /**< 0x0C: Paper mat width. */
    u16 paperMatHeight; /**< 0x0E: Paper mat height. */
    u8 paperMatRed; /**< 0x10: Paper mat red. */
    u8 paperMatGreen; /**< 0x11: Paper mat green. */
    u8 paperMatBlue; /**< 0x12: Paper mat blue. */
    u8 paperBgParamA; /**< 0x13: Paper Background parameter A. */
    u8 paperBgParamB; /**< 0x14: Paper Background parameter B. */
    u8 textFileIndex; /**< 0x15: Text file index. */
    u8 pageTextureCategory; /**< 0x16: Page texture category. */
    u8 pageTexture; /**< 0x17: Page texture page number. */
    u8 weaponId; /**< 0x18: Weapon ID. */
    u8 weaponLineSpacing; /**< 0x19: Weapon line spacing. */
    u8 duelId; /**< 0x1A: Zell limit break duel ID. */
    u8 angeloId; /**< 0x1B: Angelo ID. */
    u16 weaponListX; /**< 0x1C: Weapon list X. */
    u8 weaponListY; /**< 0x1E: Weapon list Y. */
    u8 weaponXOffset; /**< 0x1F: Weapon quantity column X offset. */
    u16 duelComboX; /**< 0x20: Duel combo X. */
    u8 duelComboY; /**< 0x22: Duel combo Y. */
    u8 footerFlag; /**< 0x23: Footer flag. */
    MagazinePageOverlay pictureOverlays[4]; /**< 0x24: Picture overlays in the page. */
    MagazinePageOverlay textOverlays[4]; /**< 0x34: Text overlays in the page. */
} MagazinePageEntry; /** 0x44 bytes */

/**
 * @brief GPU composite primitive for magazine paper mat.
 */
typedef struct {
    DR_TPAGE tpage; /**< 0x00: Drawing texture page primitive. */
    u8 r0;          /**< 0x08: Color red. */
    u8 g0;          /**< 0x09: Color green. */
    u8 b0;          /**< 0x0A: Color blue. */
    u8 code;        /**< 0x0B: GP0 code. */
    s16 x0;         /**< 0x0C: X coord. */
    s16 y0;         /**< 0x0E: Y coord. */
    s16 w;          /**< 0x10: Width. */
    s16 h;          /**< 0x12: Height. */
} PaperMatPrim; /* 0x14 bytes */

/**
 * @brief GPU composite primitive representing a slice of a magazine page.
 */
typedef struct {
    DR_TWIN twin; /**< 0x00: Drawing texture window primitive. */
    SPRT sprt;    /**< 0x0C: Sprite primitive. */
} PaperMatSlicePrim; /* 0x20 bytes */

typedef struct {
    u8 pad00[0x10];
    u16 state;                      /**< 0x10: Current menu state. */
    u8 pad12[0x4];
    u16 returnState;                /**< 0x16: Previous menu state. */
    u8 pad18[0x8];
    ItemSlot *itemSlots;            /**< 0x20: Item slots. */
    u8 *battleItemOrder;            /**< 0x24: Array defining the order of battle items. */
    u8 *helpMsg;                    /**< 0x28: Message displayed in the help panel. */
    u8 *previousHelpMsg;            /**< 0x2C: Previous help message (used for transition animations). */
    u8 *noticeMsg;                  /**< 0x30: Message displayed in the notice dialog when an item is used. */
    s32 entityMask;                 /**< 0x34: Entity mask. */
    s32 availableEntities;          /**< 0x38: Bitmask of the available characters and GFs. */
    s16 menuColorIntensity;         /**< 0x3C: Color intensity used for menu fade in/out animation. */
    s16 unk3E;
    s16 brightness;                 /**< 0x40: Text brightness, or menu brightness when rendering magazine pages. */
    u8 pad42[0x2];
    u16 availableCharacters;        /**< 0x44: Available characters bitmask. */
    u16 availableGfs;               /**< 0x46: Available GFs bitmask. */
    s16 primaryListScrollOffset;    /**< 0x48: Primary list scroll offset. */
    u8 primaryListPageStart;        /**< 0x4A: Primary list current page. */
    u8 primaryListPageEnd;          /**< 0x4B: Primary list previous page. */
    s16 battlePrimaryListScrollOffset; /**< 0x4C: Battle primary list scroll offset. */
    u8 battlePrimaryListPageStart;  /**< 0x4E: Battle primary list current page. */
    s8 battlePrimaryListPageEnd;    /**< 0x4F: Battle primary list previous page. */
    s16 scrollOffset;               /**< 0x50: Scroll offset for the battle secondary list or magazine page. */
    u8 pageStart;                   /**< 0x52: Current page for the battle secondary list or magazine page. */
    u8 pageEnd;                     /**< 0x53: Previous page for the battle secondary list or magazine page. */
    s16 primaryListIndex;           /**< 0x54: Index of the primary list in the "Use" or "Rearrange" tab. */
    u8 pad56[0x2];
    s16 secondaryListIndex;         /**< 0x58: Index of the secondary list in the "Use" or "Rearrange" tab. */
    s16 unk5A;
    s16 unk5C;
    s16 abilityListSlideProgress;   /**< 0x5E: Slide progress of the ability list. */
    s8 availableCharCount;          /**< 0x60: Number of available characters. */
    s8 menuActionId;                /**< 0x61: Selected menu action ID. */
    s8 entityId;                    /**< 0x62: Entity ID (0–15 = character; 16–31 = GF). */
    s8 entityVerticalPositionCount; /**< 0x63: Number of valid vertical positions for characters or GFs. */
    u8 targetGfsOnly;               /**< 0x64: Whether the selected item targets GFs only. */
    u8 itemId;                      /**< 0x65: Selected item ID. */
    s8 confirmationOption;          /**< 0x66: Confirmation dialog option: 0 = Yes, 1 = No. */
    s8 battlePrimaryListIndex;      /**< 0x67: Index of the battle primary list in the "Battle" tab. */
    s8 battleSecondaryListIndex;    /**< 0x68: Index of the battle secondary list in the "Battle" tab. */
    s8 abilityListIndex;            /**< 0x69: Index of the ability list. */
    s8 gfId;                        /**< 0x6A: Selected GF ID. */
    s8 statId;                      /**< 0x6B: Stat ID associated with the selected stat-up item. */
    s16 battlePrimaryListSlideProgress; /**< 0x6C: Slide progress of the primary list in the "Battle" tab. */
} ItemMenuState;

/**
 * @brief Defines the stat increase properties of a stat-up item.
 *
 * The bonus stat is increased, while the basic stat is used to check
 * the maximum allowed value.
 */
typedef struct {
    u16 bonusStatOffset; /**< 0x0: Bonus stat offset in CharacterData. */
    u16 basicStatOffset; /**< 0x2: Basic stat offset in BattleCharData. */
    u16 maxValue;        /**< 0x4: Maximum allowed stat value. */
    u8 statDataType;     /**< 0x6: Stat data type: 0 = u16 (HP), 1 = u8 (other stats). */
    u8 statId;           /**< 0x7: Stat ID. */
} StatUpItemData; /* 0x8 bytes */

/** 
 * @brief Represents an item ingredient to unlock Doomtrain GF (mthomas.bin content).
 */
typedef struct {
  u8 id;        /**< 0x0: Item id. */
  u8 count;     /**< 0x1: Item count. */
  u8 pad2[2];
} MThomasEntry; /* 0x4 bytes */

extern u8 D_801EB17C[]; /**< String "mwepon.bin". */
extern u8 D_801EB188[]; /**< String "mmag.bin". */
extern u8 D_801EB194[]; /**< String "mthomas.bin". */

/**
 * @brief String IDs for the item menu actions.
 * 
 * The IDs are 0 = "Use", 1 = "Rearrange", 2 = "Sort", 3 = "Battle".
 * The fourth entry is -1 and marks the end of the array.
 */
extern s16 D_801EB1D8[5];

/**
 * @brief Item type remap table.
 *
 * Maps an item type to an action item type. The index is the item
 * type (0-21), while the stored value is the action item type (0-9).
 *
 * Action item types 0 and 7 mean no effect.
 */
extern u8 D_801EB1E4[22];

extern StatUpItemData D_801EB1FC[]; /**< Stat-up item data table. */
extern u8 D_801EB234[]; /**< Weapon IDs indexed by character ID. */
extern WeaponRecipe D_801EB330[WEAPON_RECIPE_COUNT]; /**< Weapon recipes (mwepon.bin content). */
extern MagazinePageEntry D_801EB4BC[]; /**< Magazine page data (mmag.bin content). */
extern MThomasEntry D_801EC710[]; /**< Items required to unlock the Doomtrain GF (mthomas.bin content). */
extern ItemSlot D_801ECB20[32]; /**< Item slots in battle menu order, used for manual item sorting. */
extern AbilityListEntry D_801ECB60[]; /** Available abilities for the selected GF. */
extern s32 D_801ECC10; /**< Number of available abilities for the selected GF. */
extern u8 D_801ECC20[]; /**< Message displayed in the notice dialog when an item is used. */
extern u8 *D_801ECE20; /**< Encapsulated string pointer for the notice dialog. */
extern s32 D_801ECE24; /**< Blue magic ID associated with the used Blue Magic learn item. */
extern s32 D_801ECE28; /**< Character ID affected by the used item. */
extern s32 D_801ECE2C; /**< Ability ID used for message placeholder substitution. */
extern s32 D_801ECE30; /**< GF ID used for message placeholder substitution. */
extern s32 D_801ECE34; /**< Stat ID used for stat-up item message placeholder substitution. */
extern s32 D_801ECE38; /**< Actual increased amount used for message placeholder substitution. */
extern CharacterData D_801ECE40; /**< Character data used when processing a stat-up item. */
extern s32 D_801ECED8; /**< Basic stat value for the selected character and stat ID. */
extern s32 D_801ECEDC; /**< Hightlighted character ID for the selected stat-up item. */
extern s32 D_801ECEE0; /**< Stat increase amount of the selected stat-up item. */
extern s32 D_801ECEE4; /**< ID of the stat displayed in menu. */
extern s32 D_801ECEE8; /**< Result status for the stat-up item usage: -1 = uninitialized, 0 = unchanged, 1 = increased. */

static void func_801E2800(u8 *);
static u8 *func_801E280C(void);
static u8 *func_801E281C(s32);
static s32 func_801E2848(u8 *, u8 *);
static void func_801E2BA4(u8 *, u8 *, s32);
static void func_801E2BC8(u8 *, u8 *, s32, s32, s32, s32);
static void func_801E2C0C(u8 *, u8 *, s32, s32, s32);
static s32 func_801E2C44(ItemSlot *, s32, s32);
static u8 *func_801E2C80(s32);
static void func_801E2CCC(s32, s32);
static void func_801E2D54(ItemSlot *, u8 *);
static void func_801E2E04(u8 *);
static u8 func_801E2E38(s32);
static s32 func_801E2E54(s32);
static u8 func_801E2E70(s32);
static u8 func_801E2E8C(s32);
static u8 func_801E2EA8(s32);
static s32 func_801E2F88(s32);
static s32 func_801E302C(s32, s32, s32);
static s32 func_801E3158(s32, s32, s32);
static s32 func_801E3288(s32);
static s32 func_801E3314(s32);
static s32 func_801E338C(s32, s32, s32);
static s32 func_801E347C(s32, s32);
static s32 func_801E35B8(s32, s32);
static s32 func_801E37A4(s32);
static void func_801E3854(s32, s32);
static void func_801E38DC(s32, s32);
static void func_801E3968(s32, s32);
static s32 func_801E3C1C(s32, s32);
static void func_801E3E94(void);
static s32 func_801E3EBC(s32, s32, s32);
static s32 func_801E42F8(s32, s32, u8);
static s32 func_801E4394(s32, s32);
static s32 func_801E457C(s32, ItemSlot *, s32);
static void func_801E45B4(ItemSlot *, s32, s32);
static void func_801E4608(s32, ItemMenuState *);
static void func_801E46B8(s32, s32);
static void func_801E4708(s32, s32);
static void func_801E476C(s32, ItemMenuState *);
static void func_801E47E0(s32, s32);
static void func_801E4848(s32, ItemMenuState *);
static void func_801E4908(s32, s32);
static void func_801E4940(s32);
static void func_801E49FC(s32, s32);
static void func_801E4A58(u32);
static void func_801E4B38(s32, s32, s32);
static void func_801E4B80(s32, s32);
static void func_801E4BB4(ItemMenuState *);
static void func_801E4C14(ItemMenuState *);
static void func_801E4C74(u8 *, u8 *, s32);
static void func_801E4D40(u8 *, u8 *, s32, s32, s32, AbilityListEntry *);
static void func_801E4EA4(ItemMenuState *);
static void *func_801E7D18(void *, void *, s32, s32, s32);
static void *func_801E7E74(ItemMenuState *, void *, void *, s32, s32);
static void *func_801E7F4C(ItemMenuState *, void *, void *, s32, s32);
static void *func_801E8024(ItemMenuState *, void *, void *, s32, s32);
static void *func_801E80D0(void *, void *, s32, s32, s32);
static void *func_801E8180(ItemMenuState *, void *, void *, s32, s32);
static void *func_801E820C(void *, void *, s32, s32, s32);
static void *menuitem_drawGfPanel(void *, void *, s32, s32, s32, u16);
static void *func_801E83B4(void *, void *, s32, s32, s32, s32, u16, u16);
static void *func_801E84A4(void *, void *, s32, s32, s32);
static void *func_801E859C(ItemMenuState *, void *, void *, s32, s32);
static void *func_801E8684(ItemMenuState *, void *, void *);
static void *func_801E8780(void *, void *, s32, s32, s32);
static void *func_801E88AC(ItemMenuState *, void *, void *, s32, s32);
static u8 *func_801E89A4(s32);
static void *func_801E89C0(void *, void *, MenuDisplayConfig *, s32, MagazinePageEntry *);
static void *func_801E8AF0(void *, void *, MenuDisplayConfig *, s32, MagazinePageEntry *);
static void *func_801E8C88(void *, PaperMatPrim *, MenuDisplayConfig *, u32, MagazinePageEntry *);
static SpriteTable *GetMenuImageBufferTable(void);
static void *func_801E8DB0(void *, void *, MenuDisplayConfig *, u32, MagazinePageEntry *);
static void *func_801E8E98(void *, void *, MenuDisplayConfig *, u32, MagazinePageEntry *);
static void *func_801E8FA8(void *, PaperMatSlicePrim *, MenuDisplayConfig *, u32, MagazinePageEntry *);
static void *func_801E90D8(void *, void *, MenuDisplayConfig *, u32, MagazinePageEntry *);
static void *func_801E91E4(void *, void *, s32, s32, MagazinePageEntry *);
static void *func_801E9248(void *, void *, MagazinePageEntry *);
static void *func_801E934C(void *, void *, s32, s32, s32, s32);
static void *func_801E95C4(ItemMenuState *, void *, void *);
static void func_801E9AAC(ItemMenuState *);
static void func_801E9B98(void);


/** @brief Set the dialog message pointer. */
static void func_801E2800(u8 *str) {
    D_801ECE20 = str;
}

/** @brief Get the dialog message pointer. */
static u8 *func_801E280C(void) {
    return D_801ECE20;
}

/** @brief Get the stat name for the given stat string ID. */
static u8 *func_801E281C(s32 strId) {
    return func_801F08D4(1, 11, strId, 0);
}

/**
 * @brief Expand a menu message template into its final text.
 *
 * Decodes the source message stream, walks each line, and substitutes special
 * runtime placeholders for character names, magic names, ability names,
 * blue-magic names, stat names and numeric values using the global state set by
 * the calling menu code. The resulting text is written to @p dst and terminated
 * with a NUL byte.
 *
 * @param src Pointer to the encoded message template.
 * @param dst Output buffer that receives the expanded text.
 * @return Final return value from the scratch-buffer restore sequence.
 */
static s32 func_801E2848(u8 *src, u8 *dst) {
    s32 ch;
    u8 *name;
    u8 *pName;
    u8 *tempGp;
    u8 *savedGp;
    u8 *buf;
    s32 ret;
    u8* pos;
    u8 buffer[256];

    GP_SAVE_SCRATCH(tempGp);
    savedGp = tempGp;
    GP_ALLOC(buf, 128);

    if (src != NULL) {
        while (1) {
            if (src == NULL) {
                break;
            }

            decodeMessage(src, buf, -1);
            src = nextMessageLine(src);
            pos = buf;

            while (1) {
                ch = *pos++;

                if (ch == 1 || ch == 7 || ch == 0) {
                    goto end;
                }

                if (ch == 10) {
                    ch = *pos++;
                    name = buffer;
                    switch (ch) {
                    case 39:
                        name = getCharName(g_gameState.chars[D_801ECE28].characterId);
                        break;
                    case 40:
                        name = func_801E281C(D_801ECE34 + 4);
                        break;
                    case 36:
                        name = getMagicNamePtr(D_801ECE30 + 64);
                        break;
                    case 37:
                        name = getAbilityName(D_801ECE2C);
                        break;
                    case 41:
                        name = getBlueMagicName(D_801ECE24);
                        break;
                    case 34:
                        intToDecStringShort(D_801ECE38, name, 1);
                        replaceLeadingZeros(name, 4, 1, 16);
                        while (*name == 16) {
                            name++;
                        }
                        for (pName = name; *pName != 0; pName++) {
                            u8 tmp = getMenuString(11)[1] + 255;
                            *pName += tmp;
                        }
                    }
                    while (*name != 0) {
                        *dst++ = *name++;
                    }
                    continue;
                }

                *dst++ = ch;
                if (ch >= 25) {
                    s32 val;
                    if (ch >= 32) {
                        val = ch - 32;
                    } else if (ch < 28) {
                        val = ch * 224;
                        *dst++ = *pos;
                        val += *pos++;
                        val -= 5408;
                    } else {
                        val = ch * 224;
                        *dst++ = *pos;
                        val += *pos++;
                        val -= 6304;
                        val |= 1024;
                    }
                    getNibbleValue(val);
                } else if (ch == 2) {
                    break;
                } else if (ch < 16) {
                    *dst++ = *pos;
                    if (ch == 5) {
                        ch = *pos++;
                        getIconWidth(func_8002C734(ch));
                    } else {
                        pos++;
                    }
                }
            }
        }
    }
end:
    *dst = 0;
    GP_FREE(128);
    GP_RESTORE_RET(savedGp, ret);
    return ret;
}

/**
 * @brief Expands a message related to blue magic learn.
 *
 * @param src Encoded message template.
 * @param dst Output buffer for the expanded message.
 * @param blueMagicId Blue magic ID used for placeholder substitution.
 */
static void func_801E2BA4(u8 *src, u8 *dst, s32 blueMagicId) {
    D_801ECE24 = blueMagicId;
    func_801E2848(src, dst);
}

/**
 * @brief Expands a message related to GF compatibility increase.
 *
 * @param src Encoded message template.
 * @param dst Output buffer for the expanded message.
 * @param gfId GF ID used for placeholder substitution.
 * @param abilityId Ability ID used for placeholder substitution.
 * @param increasedAmount Actual amount increased, used for numeric placeholders.
 * @param charId Character ID used for placeholder substitution.
 */
static void func_801E2BC8(u8 *src, u8 *dst, s32 gfId, s32 abilityId, s32 increasedAmount, s32 charId) {
    D_801ECE30 = gfId;
    D_801ECE2C = abilityId;
    D_801ECE28 = charId;
    D_801ECE38 = increasedAmount;
    func_801E2848(src, dst);
}

/**
 * @brief Expands a message related to character stat-up item usage.
 *
 * @param src Encoded message template.
 * @param dst Output buffer for the expanded message.
 * @param charId Character ID used for placeholder substitution.
 * @param statId Stat ID used for placeholder substitution.
 * @param increasedAmount Actual amount by which the stat increased, used for numeric placeholder.
 */
static void func_801E2C0C(u8 *src, u8 *dst, s32 charId, s32 statId, s32 increasedAmount) {
    D_801ECE28 = charId;
    D_801ECE34 = statId;
    D_801ECE38 = increasedAmount;
    func_801E2848(src, dst);
}

/**
 * @brief Check whether an item slot matches the given ID and minimum count.
 *
 * Iterates through the item slots, returning 1 if a slot contains the specified
 * item with a count greater than or equal to the given minimum.
 *
 * @param itemSlots Array of item slots to search.
 * @param itemId Item ID to match.
 * @param itemCount Minimum required item count.
 * @return 1 if a matching slot is found, 0 otherwise.
 */
static s32 func_801E2C44(ItemSlot *itemSlots, s32 itemId, s32 itemCount) {
    u8 *items = &itemSlots->id;
    s32 i = 0;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        s32 id = *items++;
        s32 count = *items++;
        if (id == itemId && count >= itemCount) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Get the ability name string at the specified list index.
 *
 * If @p abilityListIndex is less than the number of available abilities,
 * retrieves the ability name using the corresponding ability ID.
 *
 * @param abilityListIndex Index into the available ability list.
 * @return Pointer to the ability name string, or NULL if the index is out of bounds.
 */
static u8 *func_801E2C80(s32 abilityListIndex) {
    if (abilityListIndex < D_801ECC10) {
        return getAbilityDesc(D_801ECB60[abilityListIndex].slotIndex);
    }
    return NULL;
}

/**
 * @brief Load a magazine page texture.
 *
 * Adjusts the texture ID based on the texture category, then loads
 * the texture using the overlay TIM callback.
 *
 * @param pageTextureCategory Page texture category.
 * @param pageTexture Base texture ID.
 */
static void func_801E2CCC(s32 pageTextureCategory, s32 pageTexture) {
    switch (pageTextureCategory) {
    case 0:
        pageTexture += 28;
        break;
    case 1:
        pageTexture += 20;
        break;
    case 2:
        pageTexture += 24;
        break;
    case 3:
        pageTexture += 44;
        break;
    case 4:
        pageTexture += 48;
        break;
    case 5:
        pageTexture += 71;
        break;
    case 6:
        pageTexture += 180;
        break;
    }
    loadOverlayWithTimCallback(pageTexture, MENU_SUBOVERLAY_ADDR);
}

/**
 * @brief Initialize the battle item slots in battle menu order.
 *
 * Builds an item quantity lookup table from the inventory, then initializes
 * D_801ECB20 according to the specified battle item order. Each slot stores
 * the item ID and its quantity in the inventory, or zero if the item is absent.
 *
 * @param itemSlots Inventory item slots.
 * @param battleItemOrder Item slot indices in battle menu order.
 */
static void func_801E2D54(ItemSlot *itemSlots, u8 *battleItemOrder) {
    u8 buffer[32];
    s32 i;
    u8 *pItemSlots;

    for (i = 0; i < 32; i++) {
        buffer[i] = 0;
    }

    pItemSlots = &itemSlots->id;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        s32 id = *pItemSlots++;
        s32 quantity = *pItemSlots++;

        if (id <= 32 && id != 0) {
            buffer[id - 1] = quantity;
        }
    }

    for (i = 0; i < 32; i++) {
        D_801ECB20[battleItemOrder[i]].id = i + 1;
        D_801ECB20[battleItemOrder[i]].count = buffer[i];
    }
}

/**
 * @brief Update the battle item order after manual sorting.
 *
 * Updates @p battleItemOrder to reflect the item order stored in D_801ECB20.
 * Each item ID maps to its position in the battle menu.
 *
 * @param battleItemOrder Array mapping item IDs to their positions in the battle menu.
 */
static void func_801E2E04(u8 *battleItemOrder) {
    s32 i;
    for (i = 0; i < 32; i++) {
        battleItemOrder[D_801ECB20[i].id - 1] = i;
    }
}

/** @brief Get the item type. */
static u8 func_801E2E38(s32 itemId) {
    return D_801F889C[itemId].type;
}

/** @brief Get the item flags. */
static s32 func_801E2E54(s32 itemId) {
    return D_801F889C[itemId].flags;
}

/** @brief Get the item first byte param. */
static u8 func_801E2E70(s32 itemId) {
    return D_801F889C[itemId].param1;
}

/** @brief Get the item second byte param. */
static u8 func_801E2E8C(s32 itemId) {
    return D_801F889C[itemId].param2;
}

/**
 * @brief Get item flags based on the item type, party lock flags, and available characters.
 *
 * @param itemId Item ID.
 * @return Flags associated with the item.
 */
static u8 func_801E2EA8(s32 itemId) {
    u8 ret;
    switch (func_801E2E38(itemId)) {
    case ITEM_TYPE_QUISTIS_LIMIT:
        if (!(func_80036EC0() & 8) || (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED)) {
            ret = func_801E2E54(itemId) & ~ITEM_FLAG_USABLE_IN_MENU;
        } else {
            ret = func_801E2E54(itemId);
        }
        break;
    case ITEM_TYPE_HEAL_CHARA:
    case ITEM_TYPE_REVIVE_CHARA:
    case ITEM_TYPE_REVIVE_FULL_CHARA:
    case ITEM_TYPE_HEAL_GF:
    case ITEM_TYPE_REVIVE_GF:
    case ITEM_TYPE_REVIVE_FULL_GF:
    case ITEM_TYPE_FULL_RESTORE:
    case ITEM_TYPE_BATTLE_ONLY:
    case ITEM_TYPE_AMMO:
    case ITEM_TYPE_REFINE_ONLY:
    case ITEM_TYPE_RENAME_CARD:
    case ITEM_TYPE_GF_COMPATIBILITY:
    case ITEM_TYPE_TEACH_GF_ABILITY:
    case ITEM_TYPE_FORGET_ABILITY:
    case ITEM_TYPE_STAT_UP:
    case ITEM_TYPE_CURE_STATUS:
        ret = func_801E2E54(itemId);
        break;
    case ITEM_TYPE_PET_NAMETAG:
        ret = 17;
        if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_FLAG_10) {
            ret = 16;
        }
        break;
    case ITEM_TYPE_MAGICAL_LAMP:
        ret = 16;
        if (D_8008520B == 0) {
            ret = 17;
        }
        break;
    case ITEM_TYPE_MAGAZINE:
    case ITEM_TYPE_CHOCOBO_TAG:
    case ITEM_TYPE_SOLOMON_RING:
        ret = 17;
        break;
    }
    return ret;
}

/**
 * @brief Check whether an item can be used.
 *
 * Checks the item type and applies additional restrictions. When
 * SEALED_RESURRECTION is active, items with revive types cannot be used.
 * Items with full restore type require bit 0 of func_801EFFD4() to be set.
 * Otherwise, returns bit 0 of the item flags returned by func_801E2EA8().
 *
 * @param itemId Item ID.
 * @return Nonzero if the item can be used, 0 otherwise.
 */
static s32 func_801E2F88(s32 itemId) {
    s32 type = D_801F889C[itemId].type;

    if (func_801F79F8(SEALED_RESURRECTION)) {
        if (type == ITEM_TYPE_REVIVE_CHARA) {
            return 0;
        }
        if (type == ITEM_TYPE_REVIVE_FULL_CHARA) {
            return 0;
        }
        if (type == ITEM_TYPE_REVIVE_GF) {
            return 0;
        }
        if (type == ITEM_TYPE_REVIVE_FULL_GF) {
            return 0;
        }
    }
    if (type == ITEM_TYPE_FULL_RESTORE) {
        if (!(func_801EFFD4() & ITEM_FLAG_USABLE_IN_MENU)) {
            return 0;
        }
    }
    return func_801E2EA8(itemId) & ITEM_FLAG_USABLE_IN_MENU;
}

/**
 * @brief Restore HP and remove specified status ailments from a character or GF.
 *
 * Called when an item of type ACTION_ITEM_TYPE_HEAL is used.
 *
 * @param entityId Character or GF ID.
 * @param healAmount Base healing amount, scaled by 50.
 * @param statusMask Mask of status flags to remove.
 * @return 1 if HP or status flags changed, 0 otherwise.
 * @note func_801E3158 duplicate.
 */
static s32 func_801E302C(s32 entityId, s32 healAmount, s32 statusMask) {
    s32 ret;
    s32 curHp;
    s32 maxHp;
    s32 status;
    s32 tmp;

    ret = 0;
    curHp = func_801F57DC(entityId);
    maxHp = func_801F58EC(entityId);
    status = func_801F57A4(entityId);
    healAmount *= 50;

    if ((status & STATUS_ZOMBIE) && !(statusMask & STATUS_ZOMBIE)) {
        return ret;
    }

    if (status & STATUS_KO) {
        healAmount = 0;
    }
    healAmount += curHp;

    if (healAmount >= 0) {
        tmp = maxHp;
        if (maxHp >= healAmount) {
            tmp = healAmount;
        }
    } else {
        tmp = 0;
    }
    healAmount = tmp;

    status = func_801F5150(healAmount, maxHp, status);
    
    if (status & statusMask) {
        status &= ~statusMask;
        ret = 1;
    }

    func_801F5868(entityId, healAmount);
    func_801F576C(entityId, status);
    
    if (curHp != healAmount) {
        ret = 1;
    }
    return ret;
}

/**
 * @brief Restore HP and remove specified status ailments from a character or GF.
 *
 * Called when an item of type ACTION_ITEM_TYPE_CURE_STATUS is used.
 *
 * @param entityId Character or GF ID.
 * @param healAmount Base healing amount, scaled by 50.
 * @param statusMask Mask of status flags to remove.
 * @return 1 if HP or status flags changed, 0 otherwise.
 * @note func_801E302C duplicate.
 */
static s32 func_801E3158(s32 charId, s32 healAmount, s32 statusMask) {
    s32 ret;
    s32 curHp;
    s32 maxHp;
    s32 status;
    s32 tmp;

    ret = 0;
    curHp = func_801F57DC(charId);
    maxHp = func_801F58EC(charId);
    status = func_801F57A4(charId);
    healAmount *= 50;

    if ((status & STATUS_ZOMBIE) && !(statusMask & STATUS_ZOMBIE)) {
        healAmount = 0;
    }
    if (status & STATUS_KO) {
        healAmount = 0;
    }
    healAmount += curHp;

    if (healAmount >= 0) {
        tmp = maxHp;
        if (maxHp >= healAmount) {
            tmp = healAmount;
        }
    } else {
        tmp = 0;
    }
    healAmount = tmp;

    status = func_801F5150(healAmount, maxHp, status);
    
    if (status & statusMask) {
        status &= ~statusMask;
        ret = 1;
    }

    func_801F5868(charId, healAmount);
    func_801F576C(charId, status);
    
    if (curHp != healAmount) {
        ret = 1;
    }
    return ret;
}

/**
 * @brief Revive a character or GF with one-eighth of their max HP.
 *
 * Called when an item of type ACTION_ITEM_TYPE_REVIVE is used.
 * 
 * @param entityId Character or GF ID.
 * @return 1 if the entity was revived, 0 if it was not dead.
 */
static s32 func_801E3288(s32 entityId) {
    s32 maxHp;
    s32 status;
    s32 newCurHp;

    maxHp = func_801F58EC(entityId);
    status = func_801F57A4(entityId);
    if (!(status & STATUS_KO)) {
        return 0;
    }

    newCurHp = maxHp / 8;
    func_801F576C(entityId, func_801F5150(newCurHp, maxHp, status));
    func_801F5868(entityId, newCurHp);
    return 1;
}

/**
 * @brief Fully restore the HP of a character or GF.
 *
 * Called when an item of type ACTION_ITEM_TYPE_REVIVE_FULL is used.
 *
 * @param entityId Character or GF ID.
 * @return 1 if the entity was revived, 0 if it was not dead.
 */
static s32 func_801E3314(s32 entityId) {
    s32 maxHp;
    s32 status;

    maxHp = func_801F58EC(entityId);
    status = func_801F57A4(entityId);
    if (!(status & STATUS_KO)) {
        return 0;
    }

    func_801F576C(entityId, func_801F5150(maxHp, maxHp, status));
    func_801F5868(entityId, maxHp);
    return 1;
}

/**
 * @brief Improve a character's compatibility with a GF using an item.
 *
 * Decreases the character's compatibility value with the specified GF by
 * the item's increase amount, clamping the result to the range [1000, 6000].
 * Lower values indicate better compatibility. If compatibility is already
 * at its best value (1000), makes no changes.
 *
 * @param charId Character ID.
 * @param itemId Item ID.
 * @param gfId GF ID.
 * @return 1 if the compatibility value changed, 0 otherwise.
 */
static s32 func_801E338C(s32 charId, s32 itemId, s32 gfId) {
    s32 ret;
    s32 increaseAmount;
    s32 gfCompatibility;
    s32 newGfCompatibility;
    s32 tmp;

    ret = 0;
    if (g_gameState.chars[charId].gfCompatibility[gfId] == 1000) {
        return ret;
    }

    increaseAmount = func_801E2E8C(itemId);
    gfCompatibility = g_gameState.chars[charId].gfCompatibility[gfId];
    newGfCompatibility = gfCompatibility - increaseAmount;
    if (newGfCompatibility < 1000) {
        tmp = 1000;
    } else if (newGfCompatibility <= 6000) {
        tmp = newGfCompatibility;
    } else {
        tmp = 6000;
    }
    newGfCompatibility = tmp;
    g_gameState.chars[charId].gfCompatibility[gfId] = newGfCompatibility;
    newGfCompatibility &= 0xFFFF;
    if (gfCompatibility != newGfCompatibility) {
        ret = 1;
    }
    return ret;
}

/**
 * @brief Improve a character's compatibility with all existing GFs using an item.
 *
 * Applies the item's compatibility increase to each existing GF. If no
 * compatibility values change, displays a message indicating that no
 * improvement was possible; otherwise, displays a message describing the
 * improvement.
 *
 * @param charId Character ID.
 * @param itemId Item ID.
 * @return Nonzero if compatibility with at least one GF changed, 0 otherwise.
 */
static s32 func_801E347C(s32 charId, s32 itemId) {
    s32 i;
    s32 gfMask;
    s32 increaseAmount;
    u8 *msg;

    gfMask = 0;
    increaseAmount = func_801E2E8C(itemId);

    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            gfMask |= func_801E338C(charId, itemId, i);
        }
    }

    if (gfMask == 0) {
        msg = func_801F08D4(1, 9, 32, 0);
        func_801E2BC8(msg, D_801ECC20, 0, 0, 0, charId);
        func_801E2800(D_801ECC20);
    } else {
        msg = func_801F08D4(1, 9, 33, 0);
        func_801E2BC8(msg, D_801ECC20, 0, 0, increaseAmount / 5, charId);
        func_801E2800(D_801ECC20);
    }
    
    return gfMask;
}

/**
 * @brief Improve a character's compatibility with one or all GFs using an item.
 *
 * Called when an item of type ACTION_ITEM_TYPE_GF_COMPATIBILITY is used.
 *
 * If the item targets all GFs (GF ID 255), delegates to func_801E347C().
 * Otherwise, checks whether the target GF exists and whether the character's
 * compatibility with it is already at its best value. If so, sets an info
 * dialog message and makes no changes.
 *
 * Improves compatibility with the target GF by decreasing its compatibility
 * value by @p increaseAmount. If the item flags specify a trade-off, worsens
 * compatibility with other GFs by increasing their values by half that amount.
 * All compatibility values are clamped to the range [1000, 6000], where lower
 * values indicate better compatibility.
 *
 * Sets an info dialog message describing the compatibility improvement.
 *
 * @param charId Character ID.
 * @param itemId Item ID.
 * @return 1 if compatibility was changed, 0 otherwise.
 */
static s32 func_801E35B8(s32 charId, s32 itemId) {
    s32 i;
    s32 itemFlags;
    s32 gfId;
    s32 increaseAmount;
    s32 decreaseAmount;
    u8 *msg;
    
    if (charId >= 16) {
        return 0;
    }
    
    itemFlags = func_801E2E54(itemId);
    gfId = func_801E2E70(itemId);
    increaseAmount = func_801E2E8C(itemId);
    
    if (gfId == ALL_GFS) {
        return func_801E347C(charId, itemId);
    }

    decreaseAmount = 0;
    if (itemFlags & ITEM_FLAG_GF_COMPAT_OTHERS) {
        decreaseAmount = (u32)increaseAmount / 2;
    }

    if (!(g_gameState.gfs[gfId].exists & GF_EXISTS)) {
        return 0;
    }
     
    if (g_gameState.chars[charId].gfCompatibility[gfId] == 1000) {
        msg = func_801F08D4(1, 9, 28, 0);
        func_801E2BC8(msg, D_801ECC20, gfId, 0, 0, charId);
        func_801E2800(D_801ECC20);
        return 0;
    }
        
    for (i = 0; i < GF_COUNT; i++) {
        s32 tmp;
        s32 newGfCompatibility;

        tmp = g_gameState.chars[charId].gfCompatibility[i];
        if (i == gfId) {
            tmp -= increaseAmount;
        } else {
            tmp += decreaseAmount;
        }

        if (tmp < 1000) {
            newGfCompatibility = 1000;
        } else if (tmp <= 6000) {
            newGfCompatibility = tmp;
        } else {
            newGfCompatibility = 6000;
        }
        
        g_gameState.chars[charId].gfCompatibility[i] = newGfCompatibility;
    }
    
    msg = func_801F08D4(1, 9, 27, 0);
    func_801E2BC8(msg, D_801ECC20, gfId, 0, increaseAmount / 5, charId);
    func_801E2800(D_801ECC20);
    return 1;
}

/**
 * @brief Fully restore HP and cure status ailments on a character or GF.
 *
 * Called when an item of type ACTION_ITEM_TYPE_FULL_RESTORE is used.
 *
 * @param entityId Character or GF ID.
 * @return 1 if HP or status flags changed, 0 otherwise.
 */
static s32 func_801E37A4(s32 entityId) {
    s32 maxHp;
    s32 curHp;
    s32 status;
    s32 oldStatus;

    maxHp = func_801F58EC(entityId);
    curHp = func_801F57DC(entityId);
    status = func_801F57A4(entityId);

    if (func_801F79F8(SEALED_RESURRECTION) && (status & STATUS_KO)) {
        return 0;
    }

    oldStatus = status;
    status &= 0x80;

    func_801F576C(entityId, status);    
    func_801F5868(entityId, maxHp);

    if (curHp != maxHp || status != oldStatus) {
        return 1;
    }

    return 0;
}

/**
 * @brief Set the learning bit for a GF ability.
 *
 * Called when the player confirms the selected ability to be unlearned by the
 * selected GF using Amnesia Greens.
 *
 * @param gfId GF ID.
 * @param abilityId Ability ID to search for.
 */
static void func_801E3854(s32 gfId, s32 abilityId) {
    s32 learning;
    JunctionableGfEntry *junctionableGf;
    s32 i;

    learning = *(u32 *)&g_gameState.gfs[gfId].learning;
    junctionableGf = &g_kernel.junctionableGfs[gfId];

    for (i = 0; i < 21; i++) {
        if (junctionableGf->abilities[i].abilityId == abilityId) {
            learning |= 1 << (i + 8);
            break;
        }
    }

    *(u32 *)&g_gameState.gfs[gfId].learning = learning;
}

/**
 * @brief Clear the learning bit for a GF ability.
 *
 * Called when the player uses a Teach GF Ability item.
 *
 * @param gfId GF ID.
 * @param abilityId Ability ID.
 */
static void func_801E38DC(s32 gfId, s32 abilityId) {
    s32 learning;
    JunctionableGfEntry *junctionableGf;
    s32 i;

    learning = *(u32 *)&g_gameState.gfs[gfId].learning;
    junctionableGf = &g_kernel.junctionableGfs[gfId];

    for (i = 0; i < 21; i++) {
        if (junctionableGf->abilities[i].abilityId == abilityId) {
            learning &= ~(1 << (i + 8));
            break;
        }
    }

    *(u32 *)&g_gameState.gfs[gfId].learning = learning;
}

/**
 * @brief Remove a learned ability from a GF and update affected characters.
 *
 * Called when the player confirms the selected ability to be unlearned by the
 * selected GF using Amnesia Greens.
 *
 * @param gfId GF ID.
 * @param abilityId Ability ID to remove.
 */
static void func_801E3968(s32 gfId, s32 abilityId) {
    u8 abilities[4];
    u8 *abilitiesPtr;
    s32 i;
    s32 j;
    s32 k;
    u32 junctionedGfAbilities;
    s32 hasAbility;
    s32 junctionMask;
    s32 abilityFlags;
    s32 abilityMask;
    s32 *completeAbilities;

    if (g_gameState.gfs[gfId].learning == abilityId) {
        g_gameState.gfs[gfId].learning = 0;
    }

    completeAbilities = g_gameState.gfs[gfId].completeAbilities;
    completeAbilities[abilityId / 32] &= ~(1 << (abilityId & 0x1F));

    func_801E3854(gfId, abilityId);

    for (i = 0; i < CHARACTER_COUNT; i++) {
        junctionedGfAbilities = 0;
        hasAbility = 0;

        for (j = 0; j < GF_COUNT; j++) {
            if ((g_gameState.chars[i].junctedGfs >> j) & 1) {
                junctionedGfAbilities |= g_gameState.gfs[j].completeAbilities[0];
                completeAbilities = g_gameState.gfs[j].completeAbilities;
                abilityFlags = completeAbilities[abilityId / 32];
                abilityMask = 1 << (abilityId & 0x1F);
                if (abilityFlags & abilityMask) {
                    hasAbility = 1;
                }
            }
        }

        junctionedGfAbilities /= 2;
        junctionedGfAbilities &= (1 << 19) - 1;
        k = 2;
        if (junctionedGfAbilities & 0x20000) {
            k = 3;
        }
        if (junctionedGfAbilities & 0x40000) {
            k = 4;
        }
        
        junctionedGfAbilities = func_801F7C20(junctionedGfAbilities);
        
        for (j = 0; j < 19; j++) {
            junctionMask = 1 << j;
            if (!(junctionedGfAbilities & junctionMask)) {
                g_gameState.chars[i].junctions[j] = 0;
            }
        }

        for (j = 0; j < 4; j++) {
            abilities[j] = 0;
        }
        
        abilitiesPtr = &abilities[0];
        for (j = 0; j < 4; j++) {
            s32 ability = g_gameState.chars[i].abilities[j];
            if (ability != 0 && (ability != abilityId || hasAbility)) {
                *abilitiesPtr++ = ability;
            }
        }
        
        abilitiesPtr = &abilities[0];
        for (j = 0; j < 4; j++) {
            g_gameState.chars[i].abilities[j] = *abilitiesPtr++;
        }

        while (k < 4) {
            g_gameState.chars[i].abilities[k++] = 0;
        }

        for (j = 0; j < 4; j++) {
            if (g_gameState.chars[i].commands[j] == abilityId && !hasAbility) {
                g_gameState.chars[i].commands[j] = 0;
            }
        }
        
        func_801F5400(i);
        func_801F1B4C(i);
    }
}

/**
 * @brief Teach a GF an ability using an item.
 *
 * Called when the player uses a Teach GF Ability item.
 *
 * @param gfId GF identifier with 16 added to it.
 * @param abilityId Ability ID to teach.
 * @return 1 if the ability was successfully learned, 0 otherwise.
 */
static s32 func_801E3C1C(s32 gfId, s32 abilityId) {
    s32 i;
    s32 msg;
    s32 *completeAbilities;
    AbilityListEntry *abilities;

    if (gfId < 16) {
        return 0;
    }

    gfId -= 16;
    completeAbilities = g_gameState.gfs[gfId].completeAbilities;

    abilities = D_801ECB60;
    D_801ECC10 = func_800369CC(gfId, abilities, 1);

    for (i = 0; i < D_801ECC10; i++) {
        if (abilities[i].slotIndex == abilityId) {
            switch (abilities[i].type) {
            case ABILITY_TYPE_COMMAND:
                completeAbilities[abilityId / 32] |= 1 << (abilityId & 0x1F);
                func_801E38DC(gfId, abilityId);
                msg = func_801F08D4(1, 9, 24, 0);
                func_801E2BC8(msg, D_801ECC20, gfId, abilityId, 0, 0);
                func_801E2800(D_801ECC20);
                recalcPartyStats();
                return 1;
            case ABILITY_TYPE_CHARACTER:
                msg = func_801F08D4(1, 9, 25, 0);
                func_801E2BC8(msg, D_801ECC20, gfId, abilities[i].slotIndex, 0, 0);
                func_801E2800(D_801ECC20);
                return 0;
            default:
                return 0;
            }
            break;
        }
    }

    if (D_801ECC10 < 22) {
        completeAbilities[abilityId / 32] |= 1 << (abilityId & 0x1F);
        func_801E38DC(gfId, abilityId);
        msg = func_801F08D4(1, 9, 24, 0);
        func_801E2BC8(msg, D_801ECC20, gfId, abilityId, 0, 0);
        func_801E2800(D_801ECC20);
        recalcPartyStats();
        return 1;
    }

    msg = func_801F08D4(1, 9, 26, 0);
    func_801E2BC8(msg, D_801ECC20, gfId, 0, 0, 0);
    func_801E2800(D_801ECC20);
    return 0;
}

/** @brief Clean the variables used on the character stat-up item. */
static void func_801E3E94(void) {
    D_801ECEDC = -1;
    D_801ECEE4 = -1;
    D_801ECEE8 = -1;
    D_801ECEE0 = -1;
}

/**
 * @brief Apply a character stat-up item.
 *
 * Calculates the character's base stat and updates the corresponding bonus
 * stat, clamping its value to [0, maxValue]. The stat is only modified when
 * @p increaseAmount is nonzero and the base stat is below the item's maximum.
 * Displays a message indicating the result when applying an increase.
 *
 * Called by the menu rendering callback with @p increaseAmount set to 0 to
 * retrieve the highlighted character's current stat value without increasing
 * it. Also called when a character is selected, with @p increaseAmount
 * nonzero, to apply the stat increase.
 *
 * @param charId Character ID.
 * @param increaseAmount Amount to add to the stat's bonus value.
 * @param statId Index into the stat-up item data table.
 * @return 1 if the stat can be increased, 0 otherwise.
 */
static s32 func_801E3EBC(s32 charId, s32 increaseAmount, s32 statId) {
    BattleCharData battleCharData;
    s32 ret;
    StatUpItemData *statUpItemData;
    s32 i;
    s16 *basicStatPtr16;
    s16 *bonusStatPtr16;
    u8 *basicStatPtr8;
    u8 *bonusStatPtr8;
    s32 oldBonusStat;
    s32 newBonusStat;
    s32 tmp;
    s32 msg;

    ret = 0;
    if (charId >= 16) {
        return ret;
    }

    if (D_801ECEDC == charId && D_801ECEE0 == increaseAmount && D_801ECEE4 == statId) {
        return D_801ECEE8;
    }

    statUpItemData = &D_801EB1FC[statId];

    D_801ECEDC = charId;
    D_801ECEE0 = increaseAmount;
    D_801ECEE4 = statId;

    D_801ECE40 = g_gameState.chars[charId];

    g_gameState.chars[charId].statusFlags = 0;

    for (i = 0; i < 20; i++) {
        g_gameState.chars[charId].junctions[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        g_gameState.chars[charId].commands[i] = 0;
        g_gameState.chars[charId].abilities[i] = 0;
    }

    g_gameState.chars[charId].weaponId = D_801EB234[g_gameState.chars[charId].characterId];

    func_801F537C(charId, &battleCharData);

    // Dead code added to match with the original game binary.
    if (msg) { msg++; msg--; }

    switch (statUpItemData->statDataType) {
    case 0:
        basicStatPtr16 = (s16 *)((s32)&battleCharData + statUpItemData->basicStatOffset);
        D_801ECED8 = *basicStatPtr16;
        bonusStatPtr16 = (s16 *)((s32)&D_801ECE40 + statUpItemData->bonusStatOffset);

        if (*basicStatPtr16 < statUpItemData->maxValue) {
            oldBonusStat = *bonusStatPtr16;

            if (oldBonusStat + increaseAmount >= 0) {
                tmp = oldBonusStat + increaseAmount;
                if (tmp > statUpItemData->maxValue) {
                    tmp = statUpItemData->maxValue;
                }
            } else {
                tmp = 0;
            }

            newBonusStat = tmp;
            oldBonusStat = newBonusStat - oldBonusStat;
            *bonusStatPtr16 = newBonusStat;
            
            ret = 1;

            if (increaseAmount != 0) {
                msg = func_801F08D4(1, 9, 29, 0);
                func_801E2C0C(msg, D_801ECC20, charId, statUpItemData->statId, oldBonusStat);
                func_801E2800(D_801ECC20);
            }
        } else {
            if (increaseAmount != 0) {
                msg = func_801F08D4(1, 9, 30, 0);
                func_801E2C0C(msg, D_801ECC20, charId, statUpItemData->statId, 0);
                func_801E2800(D_801ECC20);
            }
        }
        break;
    case 1:
        basicStatPtr8 = (u8 *)((s32)&battleCharData + statUpItemData->basicStatOffset);
        D_801ECED8 = *basicStatPtr8;
        bonusStatPtr8 = (u8 *)((s32)&D_801ECE40 + statUpItemData->bonusStatOffset);

        if (*basicStatPtr8 < statUpItemData->maxValue) {
            oldBonusStat = *bonusStatPtr8;

            if (oldBonusStat + increaseAmount >= 0) {
                tmp = oldBonusStat + increaseAmount;
                if (tmp > statUpItemData->maxValue) {
                    tmp = statUpItemData->maxValue;
                }
            } else {
                tmp = 0;
            }

            newBonusStat = tmp;
            oldBonusStat = newBonusStat - oldBonusStat;
            *bonusStatPtr8 = newBonusStat;

            ret = 1;

            if (increaseAmount != 0) {
                msg = func_801F08D4(1, 9, 29, 0);
                func_801E2C0C(msg, D_801ECC20, charId, statUpItemData->statId, oldBonusStat);
                func_801E2800(D_801ECC20);
            }
        } else {
            if (increaseAmount != 0) {
                msg = func_801F08D4(1, 9, 30, 0);
                func_801E2C0C(msg, D_801ECC20, charId, statUpItemData->statId, 0);
                func_801E2800(D_801ECC20);
            }
        }
        break;
    }

    g_gameState.chars[charId] = D_801ECE40;

    func_801F5400(charId);
    D_801ECEE8 = ret;
    return ret;
}

/**
 * @brief Apply a character stat-up item to the stats selected by a bitmask.
 *
 * Called when a stat-up item is used on a character.
 *
 * @param charId Character ID.
 * @param increaseAmount Amount to add to the stat's bonus value.
 * @param statMask Bitmask identifying the stats to process.
 * @return Bitwise OR of the results returned by func_801E3EBC().
 */
static s32 func_801E42F8(s32 charId, s32 increaseAmount, u8 statMask) {
    s32 accum;
    s32 i;

    accum = 0;
    for (i = 0; i < 8; i++) {
        if ((statMask >> i) & 1) {
            func_801E3E94();
            accum |= func_801E3EBC(charId, increaseAmount, i);
        }
    }
    func_801E3E94();
    return accum;
}

/**
 * @brief Apply an item effect to the targeted characters or GFs.
 * 
 * Called when an item is used on a character or GF.
 * 
 * @param itemId Item ID.
 * @param entityMask Bitmask identifying the targeted entities.
 * @return Nonzero if the item successfully affects at least one target, 0 otherwise.
 */
static s32 func_801E4394(s32 itemId, s32 entityMask) {
    s32 param1;
    s32 param2;
    s32 ret;
    s32 i;
    s32 mask;
    s32 actionItemType;

    param1 = D_801F889C[itemId].param1;
    param2 = D_801F889C[itemId].param2;
    actionItemType = func_801E2E38(itemId);
    actionItemType = D_801EB1E4[actionItemType];
    ret = 0;

    for (i = 0; i < 32; i++) {
        mask = 1 << i;
        if (entityMask & mask) {
            switch (actionItemType) {
            case ACTION_ITEM_TYPE_HEAL:
                ret |= func_801E302C(i, param1, param2);
                break;
            case ACTION_ITEM_TYPE_REVIVE:
                ret |= func_801E3288(i);
                break;
            case ACTION_ITEM_TYPE_REVIVE_FULL:
                ret |= func_801E3314(i);
                break;
            case ACTION_ITEM_TYPE_FULL_RESTORE:
                ret |= func_801E37A4(i);
                break;
            case ACTION_ITEM_TYPE_GF_COMPATIBILITY:
                ret |= func_801E35B8(i, itemId);
                break;
            case ACTION_ITEM_TYPE_TEACH_GF_ABILITY:
                ret |= func_801E3C1C(i, param1);
                break;
            case ACTION_ITEM_TYPE_STAT_UP:
                ret |= func_801E42F8(i, param1, param2);
                break;
            case ACTION_ITEM_TYPE_CURE_STATUS:
                ret |= func_801E3158(i, param1, param2);
                break;
            case 0:
                break;
            }
        }
    }

    if (ret != 0) {
        switch (actionItemType) {
        case ACTION_ITEM_TYPE_HEAL: 
        case ACTION_ITEM_TYPE_FULL_RESTORE: 
            playSoundEffect(10);
            break;
        case ACTION_ITEM_TYPE_REVIVE: 
        case ACTION_ITEM_TYPE_REVIVE_FULL: 
        case ACTION_ITEM_TYPE_GF_COMPATIBILITY: 
        case ACTION_ITEM_TYPE_TEACH_GF_ABILITY: 
        case ACTION_ITEM_TYPE_STAT_UP: 
        case ACTION_ITEM_TYPE_CURE_STATUS: 
            playSoundEffect(11);
            break;
        default:
            sendSpuCommand(8);
            break;
        }
    } else {
        sendSpuCommand(5);
    }

    // Dead code added to match with the original game binary.
    if (actionItemType) { actionItemType++; actionItemType--; }

    return ret;
}

/**
 * @brief Consume one item from an item slot.
 *
 * Decrements the slot's item count if it is positive. If the count reaches
 * zero, clears the item ID to mark the slot as empty.
 *
 * @param itemId Unused.
 * @param itemSlots Array of item slots.
 * @param index Index of the item slot to update.
 * @return 1 if the item count remains positive after decrementing, 0 otherwise.
 */
static s32 func_801E457C(s32 itemId, ItemSlot *itemSlots, s32 index) {
    ItemSlot *item;
    s32 count;
    s32 ret;

    ret = 0;
    item = &itemSlots[index];
    count = item->count;

    if (count > 0) {
        count--;
        item->count = count;
        if (count != 0) {
            return 1;
        }
        item->id = 0;
    }

    // Dead code added to match with the original game binary.
    if (ret) { ret++; ret--; }

    return ret;
}

/**
 * @brief Consume a specified quantity of an item from the inventory.
 *
 * Called when the Solomon Ring is used to obtain the Doomtrain GF.
 *
 * @param itemSlots Inventory item slots.
 * @param itemId Item ID to consume.
 * @param itemCount Quantity to subtract.
 */
static void func_801E45B4(ItemSlot *itemSlots, s32 itemId, s32 itemCount) {
    s32 i;
    for (i = 0; i < ITEM_SLOT_COUNT; i++, itemSlots++) {
        s32 id = itemSlots->id;
        s32 qty = itemSlots->count;

        if (id != 0 && qty != 0 && id == itemId) {
            qty -= itemCount;
            itemSlots->count = qty;
            if (qty == 0) {
                itemSlots->id = 0;
            }
            return;
        }
    }
}

/**
 * @brief Render the cursor at the highlighted item in the primary list of the "Battle" tab.
 *
 * @param cursorRenderMode Cursor rendering mode.
 * @param s Pointer to the item menu state.
 */
static void func_801E4608(s32 cursorRenderMode, ItemMenuState *s) {
    s32 listIndex;
    s32 xOffset;
    s32 yOffset;
    s32 xBase;
    s32 x;
    s32 y;

    if (s->battlePrimaryListSlideProgress == 0) {
        return;
    }

    listIndex = s->battlePrimaryListIndex;

    xOffset = D_801FA3C8[(4096 - s->battlePrimaryListSlideProgress) / 64];
    xOffset = (xOffset * 192) / 4096;

    // Dead code added to match with the original game binary.
    do { xBase = 65; } while (0);

    xBase = 65;

    yOffset = (listIndex % 4) * 13;

    x = xBase - xOffset;
    y = 68 + yOffset;

    func_801F0A34(cursorRenderMode, 0, x, y);
}

/**
 * @brief Render the cursor at the highlighted item in the secondary list of the "Battle" tab.
 *
 * Called when the highlighted item of the primary list is selected.
 *
 * @param cursorRenderMode Cursor rendering mode.
 * @param index Index in the secondary list.
 */
static void func_801E46B8(s32 cursorRenderMode, s32 index) {
    func_801F0A34(cursorRenderMode, 0, 193, (index % 4) * 13 + 141);
}

/**
 * @brief Render the cursor at the given action in the Item menu.
 *
 * Called continuously while displaying the Item menu.
 *
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param actionId Action ID.
 */
static void func_801E4708(s32 cursorRenderMode, s32 actionId) {
    s16 xOffsets[36];
    func_801F5984(D_801EB1D8, xOffsets, 9);
    func_801F0A34(cursorRenderMode, 0, xOffsets[actionId] + 50, 13);
}

/**
 * @brief Render the cursor in the secondary list of the "Rearrange" tab.
 *
 * Called when an item is selected in the primary list of the "Rearrange" tab.
 * 
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param s Pointer to the item menu state.
 */
static void func_801E476C(s32 cursorRenderMode, ItemMenuState *s) {
    s32 listIndex;
    s32 x;
    s32 y;
    
    listIndex = s->secondaryListIndex;

    y = listIndex % 11;
    y *= 13;
    y += 65;

    x = 205;
    
    func_801F0A34(cursorRenderMode, 0, x, y);
}

/**
 * @brief Render the cursor in the primary list of the "Use" or "Rearrange" tab.
 *
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param index Index of the primary list.
 */
static void func_801E47E0(s32 cursorRenderMode, s32 index) {
    s32 x;
    s32 y;

    y = index % 11;
    y *= 13;
    y += 65;

    x = 39;

    func_801F0A34(cursorRenderMode, 0, x, y);
}

/**
 * @brief Render the cursor at the selected GF ability.
 *
 * Called when a GF is selected after using Amnesia Greens.
 *
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param s Pointer to the item menu state.
 */
static void func_801E4848(s32 cursorRenderMode, ItemMenuState *s) {
    s32 abilityListSlideProgress;
    s32 abilityListIndex;
    s32 xOffset;
    s32 yOffset;
    s32 x;
    s32 y;

    abilityListSlideProgress = s->abilityListSlideProgress;
    abilityListIndex = s->abilityListIndex;

    xOffset = D_801FA3C8[abilityListSlideProgress / 64];
    xOffset = (xOffset * 190) / 4096;

    yOffset = abilityListIndex % 11;
    yOffset *= 13;
    
    x = 39;
    x -= xOffset;

    y = 65 + yOffset;

    func_801F0A34(cursorRenderMode, 0, x, y);
}

/**
 * @brief Render the cursor in the list of characters.
 * 
 * Called when an item for a single character is used.
 * 
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param index Index of the list of characters.
 */
static void func_801E4908(s32 cursorRenderMode, s32 index) {
    func_801F0A34(cursorRenderMode, 0, 200, index * 13 + 66);
}

/**
 * @brief Render blinking indicators for available character targets.
 *
 * Called when using an item that targets all characters. Renders an indicator
 * beside each available character, alternating visibility according to the
 * menu animation counter.
 *
 * @param availableEntities Bitmask of available target entities.
 */
static void func_801E4940(s32 availableEntities) {
    MenuDisplayConfig *cfg;
    s32 counter;
    s32 x;
    s32 i;
    s32 mask;

    cfg = &g_menuDisplayCfg;
    counter = 0;
    x = 200;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        mask = (1 << i);
        if (availableEntities & mask) {
            if ((counter + cfg->animCounter) & 1) {
                func_801F0994(0, x, 66 + (counter * 13));
            }
            counter++;
        }
    }
}

/**
 * @brief Render the cursor at the selected GF in the two-column list.
 *
 * Called when using an item on a single GF. Determines the cursor position
 * from the GF's index, with alternating columns and eight rows per column.
 *
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param index GF index in the list.
 */
static void func_801E49FC(s32 cursorRenderMode, s32 index) {
    func_801F0A34(cursorRenderMode, 0, (index & 1) * 82 + 196, (index / 2) * 13 + 64);
}

/**
 * @brief Render blinking indicators for available GF targets.
 *
 * Called when using a G-Mega-Potion to heal all GFs. Renders an indicator
 * beside each available GF in the two-column list, alternating visibility
 * according to the menu animation counter.
 *
 * @param availableEntities Bitmask of available entities (GF targets in bits 16–31).
 */
static void func_801E4A58(u32 availableEntities) {
    MenuDisplayConfig *cfg;
    s32 counter;
    s32 xBase;
    s32 yBase;
    s32 i;
    s32 mask;
    s32 x;
    s32 y;

    cfg = &g_menuDisplayCfg;
    counter = 0;
    xBase = 196;
    yBase = 64;
    availableEntities >>= 16;
    
    for (i = 0; i < GF_COUNT; i++) {
        mask = 1 << i;
        if (availableEntities & mask) {
            x = xBase + ((i & 1) * 82);
            y = yBase + ((i / 2) * 13);
            if ((counter + cfg->animCounter) & 1) {
                func_801F0994(0, x, y);
            }
            counter++;
        }
    }
}

/**
 * @brief Render the cursor according to the available target entities.
 *
 * Calls func_801E4908 if the upper 16 bits of @p availableEntities are zero,
 * or func_801E49FC if the lower 16 bits are zero. Does nothing if both halves
 * contain nonzero bits.
 *
 * @param cursorRenderMode Cursor rendering mode: 0 = blink, 1 = normal.
 * @param index Index used to determine the cursor position.
 * @param availableEntities Bitmask of available targets (bits 0–15 = characters, bits 16–31 = GFs).
 */
static void func_801E4B38(s32 cursorRenderMode, s32 index, s32 availableEntities) {
    if ((availableEntities & ~0xFFFF) == 0) {
        func_801E4908(cursorRenderMode, index);
    } else if ((availableEntities & 0xFFFF) == 0) {
        func_801E49FC(cursorRenderMode, index);
    }
}

/**
 * @brief Render blinking indicators for all available targets.
 *
 * @param targetGfsOnly Nonzero to render GF indicators, zero to render character indicators.
 * @param availableEntities Bitmask of available targets.
 */
static void func_801E4B80(s32 targetGfsOnly, s32 availableEntities) {
    if (targetGfsOnly != 0) {
        func_801E4A58(availableEntities);
    } else {
        func_801E4940(availableEntities);
    }
}

/** @brief Load the item ID and description for the highlighted item of the primary list. */
static void func_801E4BB4(ItemMenuState *s) {
    s32 id;
    s32 count;
    ItemSlot *item;
    
    item = &s->itemSlots[s->primaryListIndex];

    // Dead code added to match with the original game binary.
    item++; item--;

    id = item->id;
    count = item->count;

    s->itemId = id;

    if (id && count) {
        s->helpMsg = getItemDesc(id);
    } else {
        s->helpMsg = NULL;    
    }
}

/** @brief Load the item ID and description for the highlighted item of the secondary list. */
static void func_801E4C14(ItemMenuState *s) {
    s32 id;
    s32 count;
    ItemSlot *item;

    item = &s->itemSlots[s->secondaryListIndex];

    // Dead code added to match with the original game binary.
    item++; item--;

    id = item->id;
    count = item->count;

    s->itemId = id;

    if (id && count) {
        s->helpMsg = getItemDesc(id);
    } else {
        s->helpMsg = NULL;    
    }
}

/**
 * @brief Copy a string while replacing an ability-name placeholder.
 *
 * @param src Source string.
 * @param dst Destination buffer.
 * @param abilityId Ability ID used to retrieve the ability name.
 */
static void func_801E4C74(u8* src, u8* dst, s32 abilityId) {
    u8 buffer[64];
    s32 ch;
    u8* bufferPtr;
    
    while (1) {
        ch = *src++;
    
        if (ch == 0) {
            break;
        }
        
        bufferPtr = buffer;
        
        if (ch != 0xA) {
            *dst++ = ch;
        } else {
            ch = *src++;
            *buffer = 0;
            
            if (ch == 0x25) {
                copyString(bufferPtr, getAbilityName(abilityId));
            }
            
            while (*bufferPtr != 0) {
                *dst++ = *bufferPtr++;
            }
        }
    }
    
    *dst = 0;
}

/**
 * @brief Build a string containing an ability name and its related abilities.
 *
 * Copies @p src to @p dst, replacing ability-name placeholders with the name
 * of @p abilityId. Then searches the specified GF's ability list for abilities
 * whose level requirement references the slot of @p abilityId. For each
 * matching ability found in @p abilities with type 1, appends its name to
 * the destination string.
 *
 * @param src Source string containing ability-name placeholders.
 * @param dst Destination buffer for the resulting string.
 * @param gfId GF ID.
 * @param abilityId Ability ID used to build the initial string.
 * @param abilityCount Number of entries in @p abilities.
 * @param abilities Array of ability list entries.
 */
static void func_801E4D40(u8 *src, u8 *dst, s32 gfId, s32 abilityId, s32 abilityCount, AbilityListEntry *abilities) {
    JunctionableGfEntry *gf;
    s32 i;
    s32 j;
    s32 k;
    s32 prereqAbilityId;
    s32 upgradedAbilityId;

    gf = &g_kernel.junctionableGfs[gfId];
    func_801E4C74(src, dst, abilityId);
    dst = &dst[btlStrlen(dst)];

    for (i = 0; i < 21; i++) {
        if (abilityId != gf->abilities[i].abilityId) {
            continue;
        }

        for (j = 0; j < 21; j++) {
            prereqAbilityId = gf->abilities[j].levelReq;
            upgradedAbilityId = gf->abilities[j].abilityId;

            if (prereqAbilityId == INVALID_ID || prereqAbilityId < 101) {
                continue;
            }

            prereqAbilityId -= 101;
            if (prereqAbilityId != i) {
                continue;
            }

            for (k = 0; k < abilityCount; k++) {
                if (abilities[k].slotIndex == upgradedAbilityId && abilities[k].type == ABILITY_TYPE_COMMAND) {
                    func_801E4C74(src, dst, upgradedAbilityId);
                    dst = &dst[btlStrlen(dst)];
                }
            } 
        }
    }
}

/**
 * @brief Updates the item menu state machine.
 *
 * Handles item usage, rearrangement, action selection and menu fade in/out.
 *
 * @param s Pointer to the item menu state.
 */
static void func_801E4EA4(ItemMenuState *s) {
    u8 buffer1[256];
    u8 buffer2[208];
    u16 btnFlags;
    u16 cfgFlags;
    u16 *statePtr;
    u16 state;

    statePtr = &s->state;
    btnFlags = g_menuDisplayCfg.inputRepeat;
    cfgFlags = g_menuDisplayCfg.inputNew;
    state = *statePtr;

restart:
    switch (state) {
    case 0:
        s->primaryListIndex = 0;
        func_801E4BB4(s);
        *statePtr = 1;
        break;
        
    case 1:
        s->menuColorIntensity += 256;
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            *statePtr = 4;
        }
        func_801E4708(0, 0);
        func_801E47E0(1, 0);
        break;
        
    case 2:
        func_801E4708(1, s->menuActionId);
        *statePtr = 3;
        break;
        
    case 3:
        s->helpMsg = func_801F08D4(1, 9, D_801EB1D8[s->menuActionId], 1);
        s->menuActionId = func_801F76E0(btnFlags, 15, s->menuActionId);
        func_801E4708(1, s->menuActionId);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            state = 112;
            goto restart;
        }
        if (btnFlags & PADRdown) {
            sendSpuCommand(2);
            switch (s->menuActionId) {
            case 1:
                *statePtr = 94;
                break;
            case 2:
                *statePtr = 79;
                break;
            case 3:
                *statePtr = 26;
                break;
            case 0:
                *statePtr = 4;
                break;
            }
        }
        break;
        
    case 4:
        *statePtr = 5;
        /* fallthrough */
        
    case 5: {
        s32 page;
        func_801E2800(NULL);
        s->statId = -1;
        s->entityId = -1;
        func_801E4708(0, s->menuActionId);
        page = s->primaryListIndex / 11;
        s->primaryListIndex = func_801F6768(btnFlags, 11, s->primaryListIndex % 11) + (page * 11);
        func_801E47E0(1, s->primaryListIndex);
        func_801E4BB4(s);
        if (btnFlags & PADLleft) {
            s->returnState = 5;
            *statePtr = 6;
        }
        if (btnFlags & PADLright) {
            s->returnState = 5;
            *statePtr = 8;
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 2;
        }
        if (btnFlags & PADRdown) {
            u8 *item;
            s32 count;
            s32 itemFlags;
            item = &s->itemSlots->id;
            count = s->primaryListIndex;
            count *= 2;
            item += count;
            count = item[1];
            itemFlags = func_801E2EA8(s->itemId);
            if ((itemFlags & ITEM_FLAG_TARGET_GF) && !(itemFlags & ITEM_FLAG_TARGET_CHARA)) {
                if ((getGfAvailabilityMask() << 16) == 0) {
                    sendSpuCommand(5);
                    s->noticeMsg = func_801F08D4(1, 9, 3, 0);
                    *statePtr = 22;
                    break;
                }
            }
            if (count != 0) {
                s32 mask;
                switch (func_801E2E38(s->itemId)) {
                case ITEM_TYPE_GF_COMPATIBILITY:
                    mask = func_801E2E70(s->itemId);
                    if (mask != 0xFF) {
                        if (!((getGfAvailabilityMask() >> mask) & 1)) {
                            sendSpuCommand(5);
                            s->noticeMsg = func_801F6AFC(63);
                            *statePtr = 22;
                            setTextBrightness(s->brightness);
                            func_801F0948(s->menuColorIntensity);
                            if (D_8008520C != 0) {
                                func_801F1DB0(s->menuColorIntensity);
                            }
                            return;
                        }
                        break;
                    }
                    if (getGfAvailabilityMask() == 0) {
                        sendSpuCommand(5);
                        s->noticeMsg = func_801F6AFC(63);
                        *statePtr = 22;
                        setTextBrightness(s->brightness);
                        func_801F0948(s->menuColorIntensity);
                        if (D_8008520C != 0) {
                            func_801F1DB0(s->menuColorIntensity);
                        }
                        return;
                    }
                    break;
                case ITEM_TYPE_PET_NAMETAG:
                    if (func_801E2F88(s->itemId) != 0) {
                        sendSpuCommand(2);
                        func_801F728C(func_801F08D4(1, 9, 8, 0), 100);
                        func_801F0000(130);
                        state = 24;
                        goto restart;
                    }
                    sendSpuCommand(5);
                    s->noticeMsg = func_801F6AFC(63);
                    *statePtr = 22;
                    setTextBrightness(s->brightness);
                    func_801F0948(s->menuColorIntensity);
                    if (D_8008520C != 0) {
                        func_801F1DB0(s->menuColorIntensity);
                    }
                    return;
                case ITEM_TYPE_CHOCOBO_TAG:
                    if (*getChocoboWorldPtr() & 1) {
                        sendSpuCommand(2);
                        func_801F728C(func_801F08D4(1, 9, 7, 0), 100);
                        func_801F0000(147);
                        state = 24;
                        goto restart;
                    }
                    sendSpuCommand(5);
                    s->noticeMsg = func_801F6AFC(63);
                    *statePtr = 22;
                    setTextBrightness(s->brightness);
                    func_801F0948(s->menuColorIntensity);
                    if (D_8008520C != 0) {
                        func_801F1DB0(s->menuColorIntensity);
                    }
                    return;
                case ITEM_TYPE_MAGAZINE:
                    sendSpuCommand(2);
                    state = 80;
                    goto restart;
                case ITEM_TYPE_MAGICAL_LAMP:
                    state = 65;
                    if (!(g_gameState.gfs[GF_DIABLOS].exists & GF_EXISTS)) {
                        if (D_8008520B == 0) {
                            sendSpuCommand(2);
                            D_8008520C = 1;
                            func_801E457C(s->itemId, s->itemSlots, s->primaryListIndex);
                            setModeData(func_801E2E70(s->itemId) | (func_801E2E8C(s->itemId) << 8));
                            setGfExists(5);
                            state = 112;
                            goto restart;
                        }
                    }
                    goto restart;
                case ITEM_TYPE_SOLOMON_RING:
                    state = 50;
                    goto restart;
                }
            }
            if (func_801E2F88(s->itemId) != 0 && count != 0) {
                sendSpuCommand(2);
                *statePtr = 10;
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    }

    case 8: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->primaryListPageEnd = s->primaryListPageStart;
        page = s->primaryListIndex / 11;
        rowInPage = s->primaryListIndex % 11;
        page++;
        if (page >= 18) {
            page = 0;
        }
        s->primaryListIndex = rowInPage + page * 11;
        s->primaryListPageStart = page;
        s->primaryListScrollOffset = 3687;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        s->previousHelpMsg = s->helpMsg;
        func_801E4BB4(s);
        *statePtr = 9;
        break;
    }

    case 9:
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        s->primaryListScrollOffset -= 409;
        if (s->primaryListScrollOffset <= 0) {
            s->primaryListScrollOffset = 0;
            *statePtr = s->returnState;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 6;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 8;
        }
        break;

    case 6: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->primaryListPageEnd = s->primaryListPageStart;
        page = s->primaryListIndex / 11;
        rowInPage = s->primaryListIndex % 11;
        page--;
        if (page < 0) {
            page = 17;
        }
        s->primaryListIndex = rowInPage + page * 11;
        s->primaryListPageStart = page;
        s->primaryListScrollOffset = -3687;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        s->previousHelpMsg = s->helpMsg;
        func_801E4BB4(s);
        *statePtr = 7;
        break;
    }

    case 7:
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        s->primaryListScrollOffset += 409;
        if (s->primaryListScrollOffset >= 0) {
            s->primaryListScrollOffset = 0;
            *statePtr = s->returnState;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 6;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 8;
        }
        break;

    case 10:
        s->secondaryListIndex = 0;
        /* fallthrough */

    case 11: {
        s32 itemFlags;
        s32 availableEntities;
        s32 i;
        itemFlags = func_801E2EA8(s->itemId);
        if (func_801E2E38(s->itemId) == 20) {
            s->statId = findNthSetBit(func_801E2E8C(s->itemId), 0);
        } else {
            s->statId = -1;
        }
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        availableEntities = 0;
        if (itemFlags & ITEM_FLAG_TARGET_CHARA) {
            availableEntities = func_80036EC0();
        }
        if (itemFlags & ITEM_FLAG_TARGET_GF) {
            availableEntities |= getGfAvailabilityMask() << 0x10;
            if (availableEntities == 0) {
                sendSpuCommand(5);
                s->noticeMsg = func_801F08D4(1, 9, 3, 0);
                *statePtr = 22;
                break;
            }
        }
        if (itemFlags & ITEM_FLAG_QUISTIS_ONLY) {
            availableEntities &= 8;
            if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
                availableEntities = 0;
            }
        }
        if ((availableEntities & 0xFFFF0000) && !(itemFlags & ITEM_FLAG_TARGET_ALL_BOTH)) {
            s->targetGfsOnly = 1;
        } else {
            s->targetGfsOnly = 0;
        }
        s->availableEntities = availableEntities;
        if (itemFlags & ITEM_FLAG_TARGET_DEAD) {
            u32 deadEntities = ~(func_801F22F4() | (func_801F2370() << 0x10)) & availableEntities;
            if (deadEntities != 0) {
                if (s->targetGfsOnly != 0) {
                    s->secondaryListIndex = findNthSetBit(deadEntities >> 0x10, 0);
                } else {
                    s->secondaryListIndex = getBitRank(availableEntities, findNthSetBit(deadEntities, 0));
                }
            }
        }
        if (s->targetGfsOnly != 0) {
            s8 result;
            s32 entityId = 0;
            for (i = 16; i < 32; i++) {
                s32 mask = 1 << i;
                if (availableEntities & mask) {
                    entityId = i;
                }
            }
            result = entityId - 14;
            s->entityVerticalPositionCount = result / 2;
        } else {
            s->entityVerticalPositionCount = popcount(availableEntities);
        }
        state = 15;
        if (!(itemFlags & ITEM_FLAG_TARGET_ALL_BOTH)) {
            if (!(itemFlags & ITEM_FLAG_TARGET_ALL)) {
                state = 12;
            }
        }
        goto restart;
    }

    case 12:
        *statePtr = 14;
        /* fallthrough */

    case 14: {
        s32 mask;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        if (s->targetGfsOnly != 0) {
            s32 gfRow;
            s32 gfCol;
            s32 gfMask;
            gfCol = s->secondaryListIndex % 2;
            gfRow = func_801F6768(btnFlags, s->entityVerticalPositionCount, s->secondaryListIndex / 2);
            if ((btnFlags & PADLleft) && gfCol != 0) {
                sendSpuCommand(1);
                gfCol = 0;
            }
            if ((btnFlags & PADLright) && gfCol == 0) {
                sendSpuCommand(1);
                gfCol = 1;
            }
            s->secondaryListIndex = gfCol + (gfRow * 2);
            gfMask = 1 << s->secondaryListIndex;
            if (s->availableEntities & gfMask) {
                s->entityId = s->secondaryListIndex;
            } else {
                s->statId = -1;
                s->entityId = -1;
            }
        } else {
            s->secondaryListIndex = func_801F6768(btnFlags, s->entityVerticalPositionCount, s->secondaryListIndex);
        }
        func_801E4B38(1, s->secondaryListIndex, s->availableEntities);
        if (s->targetGfsOnly != 0) {
            s->entityId = s->secondaryListIndex + 16;
            mask = 1 << s->entityId;
            if (!(s->availableEntities & mask)) {
                s->entityId = -1;
            }
        } else {
            s->entityId = findNthSetBit(s->availableEntities, s->secondaryListIndex);
        }
        if (btnFlags & PADRdown) {
            state = 17;
            if (s->entityId >= 0) {
                s->entityMask = 1 << s->entityId;
                s->returnState = s->state;
                goto restart;
            }
            sendSpuCommand(5);
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 4;
        }
        break;
    }

    case 15:
        s->entityMask = s->availableEntities;
        *statePtr = 16;
        /* fallthrough */

    case 16:
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        func_801E4B80(s->targetGfsOnly, s->availableEntities);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 4;
        }
        state = 17;
        if (btnFlags & PADRdown) {
            s->returnState = s->state;
            goto restart;
        }
        break;

    case 17: {
        s32 itemEffectApplied;
        func_801E2800(NULL);
        *statePtr = 11;
        cfgFlags = 0;
        switch (func_801E2E38(s->itemId)) {
        case ITEM_TYPE_HEAL_CHARA:
        case ITEM_TYPE_REVIVE_CHARA:
        case ITEM_TYPE_REVIVE_FULL_CHARA:
        case ITEM_TYPE_HEAL_GF:
        case ITEM_TYPE_REVIVE_GF:
        case ITEM_TYPE_REVIVE_FULL_GF:
        case ITEM_TYPE_FULL_RESTORE:
        case ITEM_TYPE_BATTLE_ONLY:
        case ITEM_TYPE_AMMO:
        case ITEM_TYPE_MAGAZINE:
        case ITEM_TYPE_REFINE_ONLY:
        case ITEM_TYPE_GF_COMPATIBILITY:
        case ITEM_TYPE_TEACH_GF_ABILITY:
        case ITEM_TYPE_STAT_UP:
        case ITEM_TYPE_CURE_STATUS:
            itemEffectApplied = func_801E4394(s->itemId, s->entityMask);
            break;
        case ITEM_TYPE_RENAME_CARD: 
            sendSpuCommand(2);
            func_801F0000((findNthSetBit(s->entityMask & 0xFFFF0000, 0) - 13) | 0x80);
            state = 45;
            goto restart;
        case ITEM_TYPE_FORGET_ABILITY: 
            sendSpuCommand(2);
            state = 66;
            goto restart;
        case ITEM_TYPE_QUISTIS_LIMIT: 
            state = 107;
            goto restart;
        }
        if (itemEffectApplied == 1 && func_801E457C(s->itemId, s->itemSlots, s->primaryListIndex) == 0) {
            if (func_801E280C() != NULL) {
                s->returnState = 4;
                *statePtr = 18;
                break;
            }
            *statePtr = 4;
            break;
        }
        if (func_801E280C() != NULL) {
            *statePtr = 18;
        }
        break;
    }

    case 18:
        s->unk5C = 600;
        s->noticeMsg = func_801E280C();
        *statePtr = 19;
        break;

    case 19:
        s->unk5C--;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk5C = 0;
        }
        if (s->unk5C <= 0) {
            s->noticeMsg = NULL;
            *statePtr = s->returnState;
        }
        break;

    case 20:
    case 21:
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        break;

    case 22:
        s->unk5C = 600;
        *statePtr = 23;
        break;

    case 23:
        s->unk5C--;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk5C = 0;
            s->noticeMsg = NULL;
            *statePtr = 4;
        }
        if (s->unk5C <= 0) {
            s->unk5C = 0;
            s->noticeMsg = NULL;
            *statePtr = 4;
        }
        break;

    case 24:
        *statePtr = 25;
        s->confirmationOption = 1;
        break;

    case 25:
        s->confirmationOption = func_801F6768(btnFlags, 2, s->confirmationOption);
        func_801F6F88(s->confirmationOption);
        if (cfgFlags & PADRdown) {
            sendSpuCommand(2);
            if (s->confirmationOption != 0) {
                s->confirmationOption = -1;
                *statePtr = 5;
            } else {
                *statePtr = 45;
            }
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->confirmationOption = -1;
            *statePtr = 5;
        }
        break;

    case 26:
        s->battlePrimaryListPageStart = s->battlePrimaryListIndex / 4;
        s->pageStart = s->battleSecondaryListIndex / 4;
        s->battlePrimaryListScrollOffset = 0;
        s->scrollOffset = 0;
        func_801E2D54(s->itemSlots, s->battleItemOrder);
        *statePtr = 27;
        /* fallthrough */

    case 27:
        s->unk3E -= 256;
        if (s->unk3E <= 0) {
            s->unk3E = 0;
            *statePtr = 28;
        }
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        break;

    case 28:
        s->battlePrimaryListSlideProgress += 256;
        if (s->battlePrimaryListSlideProgress >= 4096) {
            s->battlePrimaryListSlideProgress = 4096;
            *statePtr = 29;
        }
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        break;

    case 29:
        *statePtr = 30;
        /* fallthrough */

    case 30: {
        s32 page = s->battlePrimaryListIndex / 4;
        s->battlePrimaryListIndex = (page * 4) + func_801F6768(btnFlags, 4, s->battlePrimaryListIndex % 4);
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        if (!(btnFlags & PADLleft)) {
            if (btnFlags & PADLright) {
                *statePtr = 33;
            } else {
                if (D_801ECB20[s->battlePrimaryListIndex].count != 0) {
                    s->helpMsg = getItemDesc(D_801ECB20[s->battlePrimaryListIndex].id);
                } else {
                    s->helpMsg = NULL;
                }
                if (cfgFlags & PADRup) {
                    sendSpuCommand(3);
                    *statePtr = 42;
                }
                if (cfgFlags & PADRdown) {
                    sendSpuCommand(2);
                    *statePtr = 35;
                }
            }
        } else {
            *statePtr = 31;
        }
        break;
    }

    case 31: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousHelpMsg = s->helpMsg;
        s->battlePrimaryListPageEnd = s->battlePrimaryListPageStart;
        page = s->battlePrimaryListIndex / 4;
        rowInPage = s->battlePrimaryListIndex % 4;
        page--;
        if (page < 0) {
            page = 7;
        }
        s->battlePrimaryListIndex = (page * 4) + rowInPage;
        s->battlePrimaryListPageStart = page;
        s->battlePrimaryListScrollOffset = -3687;
        if (D_801ECB20[s->battlePrimaryListIndex].count != 0) {
            s->helpMsg = getItemDesc(D_801ECB20[s->battlePrimaryListIndex].id);
        } else {
            s->helpMsg = NULL;
        }
        *statePtr = 32;
        /* fallthrough */
    }

    case 32:
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        s->battlePrimaryListScrollOffset += 409;
        if (s->battlePrimaryListScrollOffset >= 0) {
            s->battlePrimaryListScrollOffset = 0;
            *statePtr = 30;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 33;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 31;
        }
        break;

    case 33: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousHelpMsg = s->helpMsg;
        s->battlePrimaryListPageEnd = s->battlePrimaryListPageStart;
        page = s->battlePrimaryListIndex / 4;
        rowInPage = s->battlePrimaryListIndex % 4;
        page++;
        if (page >= 8) {
            page = 0;
        }
        s->battlePrimaryListIndex = (page * 4) + rowInPage;
        s->battlePrimaryListPageStart = page;
        s->battlePrimaryListScrollOffset = 3687;
        if (D_801ECB20[s->battlePrimaryListIndex].count != 0) {
            s->helpMsg = getItemDesc(D_801ECB20[s->battlePrimaryListIndex].id);
        } else {
            s->helpMsg = NULL;
        }
        *statePtr = 34;
        /* fallthrough */
    }

    case 34:
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        s->battlePrimaryListScrollOffset -= 409;
        if (s->battlePrimaryListScrollOffset <= 0) {
            s->battlePrimaryListScrollOffset = 0;
            *statePtr = 30;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 33;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 31;
        }
        break;

    case 35:
        *statePtr = 36;
        /* fallthrough */

    case 36: {
        s32 page = s->battleSecondaryListIndex / 4;
        s->battleSecondaryListIndex = (page * 4) + func_801F6768(btnFlags, 4, s->battleSecondaryListIndex % 4);
        func_801E4708(0, s->menuActionId);
        func_801E4608(0, s);
        func_801E46B8(1, s->battleSecondaryListIndex);
        if (!(btnFlags & PADLleft)) {
            if (btnFlags & PADLright) {
                *statePtr = 39;
            } else {
                if (D_801ECB20[s->battleSecondaryListIndex].count != 0) {
                    s->helpMsg = getItemDesc(D_801ECB20[s->battleSecondaryListIndex].id);
                } else {
                    s->helpMsg = NULL;
                }
                if (cfgFlags & PADRup) {
                    sendSpuCommand(3);
                    *statePtr = 30;
                }
                if (cfgFlags & PADRdown) {
                    s32 count = D_801ECB20[s->battlePrimaryListIndex].count;
                    if (count != D_801ECB20[s->battleSecondaryListIndex].count || count != 0) {
                        sendSpuCommand(2);
                        *statePtr = 41;
                    } else {
                        sendSpuCommand(5);
                    }
                }
            }
        } else {
            *statePtr = 37;
        }
        break;
    }

    case 37: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousHelpMsg = s->helpMsg;
        s->pageEnd = s->pageStart;
        page = s->battleSecondaryListIndex / 4;
        rowInPage = s->battleSecondaryListIndex % 4;
        page--;
        if (page < 0) {
            page = 7;
        }
        s->battleSecondaryListIndex = (page * 4) + rowInPage;
        s->pageStart = page;
        s->scrollOffset = -3687;
        if (D_801ECB20[s->battleSecondaryListIndex].count != 0) {
            s->helpMsg = getItemDesc(D_801ECB20[s->battleSecondaryListIndex].id);
        } else {
            s->helpMsg = NULL;
        }
        *statePtr = 38;
        /* fallthrough */
    }

    case 38:
        func_801E4708(0, s->menuActionId);
        func_801E4608(0, s);
        func_801E46B8(1, s->battleSecondaryListIndex);
        s->scrollOffset += 409;
        if (s->scrollOffset >= 0) {
            s->scrollOffset = 0;
            *statePtr = 36;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 39;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 37;
        }
        break;

    case 39: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->previousHelpMsg = s->helpMsg;
        s->pageEnd = s->pageStart;
        page = s->battleSecondaryListIndex / 4;
        rowInPage = s->battleSecondaryListIndex % 4;
        page++;
        if (page >= 8) {
            page = 0;
        }
        s->battleSecondaryListIndex = (page * 4) + rowInPage;
        s->pageStart = page;
        s->scrollOffset = 3687;
        if (D_801ECB20[s->battleSecondaryListIndex].count != 0) {
            s->helpMsg = getItemDesc(D_801ECB20[s->battleSecondaryListIndex].id);
        } else {
            s->helpMsg = NULL;
        }
        *statePtr = 40;
        /* fallthrough */
    }

    case 40:
        func_801E4708(0, s->menuActionId);
        func_801E4608(0, s);
        func_801E46B8(1, s->battleSecondaryListIndex);
        s->scrollOffset -= 409;
        if (s->scrollOffset <= 0) {
            s->scrollOffset = 0;
            *statePtr = 36;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 39;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 37;
        }
        break;

    case 41: {
        s32 id1;
        s32 id2;
        s32 count1;
        s32 count2;
        id1 = D_801ECB20[s->battlePrimaryListIndex].id;
        count1 = D_801ECB20[s->battlePrimaryListIndex].count;
        id2 = D_801ECB20[s->battleSecondaryListIndex].id;
        count2 = D_801ECB20[s->battleSecondaryListIndex].count;
        D_801ECB20[s->battleSecondaryListIndex].id = id1;
        D_801ECB20[s->battleSecondaryListIndex].count = count1;
        D_801ECB20[s->battlePrimaryListIndex].id = id2;
        D_801ECB20[s->battlePrimaryListIndex].count = count2;
        state = 29;
        goto restart;
    }

    case 42:
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        func_801E2E04(s->battleItemOrder);
        *statePtr = 43;
        break;

    case 43:
        s->battlePrimaryListSlideProgress -= 256;
        if (s->battlePrimaryListSlideProgress <= 0) {
            s->battlePrimaryListSlideProgress = 0;
            *statePtr = 44;
        }
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        break;

    case 44:
        s->unk3E += 256;
        if (s->unk3E >= 4096) {
            s->unk3E = 4096;
            *statePtr = 3;
        }
        func_801E4708(0, s->menuActionId);
        func_801E4608(1, s);
        break;

    case 45:
        func_801E457C(s->itemId, s->itemSlots, s->primaryListIndex);
        loadOverlayWithTimCallback(9, MENU_IMAGE_ADDR);
        loadOverlayWithTimCallback(10, MENU_UNK_801D5000_ADDR);
        *statePtr = 46;
        break;

    case 46:
        s->menuColorIntensity -= 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity <= 0) {
            s->menuColorIntensity = 0;
            func_801F1DB0(0);
            if (pollCdReadStatus() == 0) {
                func_801F0C5C(15, s);
                *statePtr = 47;
            }
        }
        break;

    case 47:
        if (func_801F0D84() == 2) {
            *statePtr = 48;
        }
        break;

    case 48:
        func_801F202C();
        func_801F1DBC(2);
        s->entityId = -1;
        s->statId = -1;
        s->confirmationOption = -1;
        func_801E4BB4(s);
        loadOverlayWithTimCallback(12, MENU_UNK_801D5000_ADDR);
        *statePtr = 49;
        break;

    case 49:
        s->menuColorIntensity += 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            func_801F1DB0(4096);
            *statePtr = 5;
        }
        break;

    case 50: {
        MThomasEntry *ingredients;
        s32 itemId;
        s32 itemCount;
        state = 65;
        if (!(g_gameState.gfs[GF_DOOMTRAIN].exists & GF_EXISTS)) {
            ingredients = D_801EC710;
            while (1) {
                itemId = ingredients->id;
                itemCount = ingredients->count;
                if (itemId == 0) {
                    state = 51;
                    break;
                }
                if (func_801E2C44(s->itemSlots, itemId, itemCount) == 0) {
                    state = 64;
                    break;
                }
                ingredients++;
            }
        }
        goto restart;
    }

    case 51: {
        MThomasEntry *ingredients;
        s32 itemId;
        s32 itemCount;
        playSoundEffect(26);
        playSoundEffect(27);
        playSoundEffect(28);
        ingredients = D_801EC710;
        while (1) {
            itemId = ingredients->id;
            itemCount = ingredients->count;
            if (itemId == 0) {
                break;
            }
            ingredients++;
            func_801E45B4(s->itemSlots, itemId, itemCount);
        }
        *statePtr = 52;
        /* fallthrough */
    }

    case 52: {
        u8 *msg = func_801F08D4(1, 9, 10, 0);
        setDialogMessage(0, msg);
        func_801F23D0(0, 104, msg);
        setDialogCornerIcon(0, 80);
        setDialogTextSpeed(0, 1024);
        openDialogAnimated(0);
        *statePtr = 53;
        break;
    }

    case 53:
        if ((getDialogTypingDone(0) != 0) && (cfgFlags & PADRdown)) {
            sendSpuCommand(2);
            closeDialogAnimated(0);
            *statePtr = 54;
        }
        break;

    case 54:
        if (getOpenDialogScale(0) == 0) {
            *statePtr = 55;
        }
        break;

    case 55: {
        u8 *msg = func_801F08D4(1, 9, 11, 0);
        setDialogMessage(0, msg);
        func_801F23D0(0, 104, msg);
        setDialogCornerIcon(0, 80);
        setDialogTextSpeed(0, 1024);
        openDialogAnimated(0);
        *statePtr = 56;
        break;
    }

    case 56:
        if ((getDialogTypingDone(0) != 0) && (cfgFlags & PADRdown)) {
            sendSpuCommand(2);
            *statePtr = 57;
        }
        break;

    case 57:
        s->menuColorIntensity -= 256;
        func_801F1DB0(s->menuColorIntensity);
        setDialogBrightness(0, s->menuColorIntensity);
        if (s->menuColorIntensity <= 0) {
            s->menuColorIntensity = 0;
            func_801F0000(14);
            func_801F0C5C(15, s);
            setDialogTextSpeed(0, 0);
            closeDialogInstant(0);
            setDialogBrightness(0, 4096);
            setGfExists(11);
            func_801E457C(s->itemId, s->itemSlots, s->primaryListIndex);
            *statePtr = 58;
        }
        break;

    case 58:
        if (func_801F0D84() == 2) {
            *statePtr = 59;
        }
        break;

    case 59:
        func_801F202C();
        func_801F1DBC(2);
        s->availableGfs = getGfAvailabilityMask();
        s->confirmationOption = -1;
        func_801E4BB4(s);
        *statePtr = 60;
        break;

    case 60:
        s->menuColorIntensity += 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            func_801F1DB0(4096);
            *statePtr = 61;
        }
        break;

    case 61: {
        u8 *msg = func_801F08D4(1, 9, 12, 0);
        setDialogMessage(0, msg);
        func_801F23D0(0, 104, msg);
        setDialogCornerIcon(0, 80);
        setDialogTextSpeed(0, 1024);
        openDialogAnimated(0);
        *statePtr = 62;
        break;
    }

    case 62:
        if ((getDialogTypingDone(0) != 0) && (cfgFlags & PADRdown)) {
            sendSpuCommand(8);
            closeDialogAnimated(0);
            *statePtr = 63;
        }
        break;

    case 63:
        if (getOpenDialogScale(0) == 0) {
            setDialogTextSpeed(0, 0);
            *statePtr = 4;
        }
        break;

    case 64:
        sendSpuCommand(5);
        s->noticeMsg = func_801F08D4(1, 9, 9, 0);
        *statePtr = 22;
        break;

    case 65:
        sendSpuCommand(5);
        s->noticeMsg = func_801F08D4(1, 9, 23, 0);
        *statePtr = 22;
        break;

    case 66:
        s->abilityListIndex = 0;
        s->battlePrimaryListPageEnd = 0;
        s->battlePrimaryListPageStart = 0;
        s->gfId = findNthSetBit(s->entityMask & 0xFFFF0000, 0) - 16;
        D_801ECC10 = func_800369CC(s->gfId, D_801ECB60, 1);
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        *statePtr = 67;
        /* fallthrough */

    case 67:
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        s->abilityListSlideProgress -= 256;
        if (s->abilityListSlideProgress <= 0) {
            s->abilityListSlideProgress = 0;
            *statePtr = 68;
        }
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        break;

    case 68:
        *statePtr = 69;
        /* fallthrough */

    case 69: {
        s32 page = s->abilityListIndex / 11;
        s->abilityListIndex = func_801F6768(btnFlags, 11, s->abilityListIndex % 11) + (page * 11);
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        s->helpMsg = func_801E2C80(s->abilityListIndex);
        if (D_801ECC10 >= 12) {
            if ((btnFlags & PADLleft) && page != 0) {
                *statePtr = 70;
            }
            if ((btnFlags & PADLright) && page == 0) {
                *statePtr = 72;
            }
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 77;
        }
        if (cfgFlags & PADRdown) {
            if (s->abilityListIndex < D_801ECC10) {
                sendSpuCommand(2);
                *statePtr = 74;
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    }

    case 70:
        sendSpuCommand(1);
        s->battlePrimaryListPageStart = 0;
        s->battlePrimaryListPageEnd = 1;
        s->abilityListIndex %= 11;
        s->battlePrimaryListScrollOffset = -3687;
        s->previousHelpMsg = s->helpMsg;
        s->helpMsg = func_801E2C80(s->abilityListIndex);
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        *statePtr = 71;
        break;

    case 71:
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        s->battlePrimaryListScrollOffset += 409;
        if (s->battlePrimaryListScrollOffset >= 0) {
            s->battlePrimaryListScrollOffset = 0;
            *statePtr = 69;
        }
        break;

    case 72: {
        s32 abilityListIndex;
        sendSpuCommand(1);
        s->battlePrimaryListPageEnd = 0;
        s->battlePrimaryListPageStart = 1;
        s->previousHelpMsg = s->helpMsg;
        abilityListIndex = (s->abilityListIndex % 11) + 11;
        s->abilityListIndex = abilityListIndex;
        s->helpMsg = func_801E2C80(s->abilityListIndex);
        s->battlePrimaryListScrollOffset = 3687;
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        *statePtr = 73;
        break;
    }

    case 73:
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        s->battlePrimaryListScrollOffset -= 409;
        if (s->battlePrimaryListScrollOffset <= 0) {
            s->battlePrimaryListScrollOffset = 0;
            *statePtr = 69;
        }
        break;

    case 74: {
        u8 *msg;
        s32 abilityId;
        s32 width;
        msg = func_801F08D4(1, 9, 34, 0);
        abilityId = D_801ECB60[s->abilityListIndex].slotIndex;
        func_801E4D40(msg, buffer1, s->gfId, abilityId, D_801ECC10, D_801ECB60);
        msg = func_801F08D4(1, 9, 14, 0);
        copyString(D_801ECC20, msg);
        btlStrcat2(D_801ECC20, buffer1);
        msg = func_801F08D4(1, 9, 17, 0);
        btlStrcat2(D_801ECC20, msg);
        width = measureMessage(D_801ECC20);
        func_801F728C(D_801ECC20, 100 - ((width >> 0x10) - 26) / 2);
        s->confirmationOption = 1;
        *statePtr = 75;
        break;
    }

    case 75: {
        s->confirmationOption = func_801F6768(btnFlags, 2, s->confirmationOption);
        func_801F6F88(s->confirmationOption);
        if (cfgFlags & PADRdown) {
            sendSpuCommand(2);
            if (s->confirmationOption != 0) {
                if (s->confirmationOption == 1) {
                    s->confirmationOption = -1;
                    *statePtr = 69;
                    break;
                }
                if (cfgFlags & PADRup) {
                    sendSpuCommand(3);
                    s->confirmationOption = -1;
                    *statePtr = 69;
                }
                break;
            }
            s->confirmationOption = -1;
            state = 76;
            goto restart;
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->confirmationOption = -1;
            *statePtr = 69;
        }
        break;
    }

    case 76: {
        s32 i;
        s32 exists;
        func_801E3968(s->gfId, D_801ECB60[s->abilityListIndex].slotIndex);
        D_801ECC10 = func_800369CC(s->gfId, D_801ECB60, 1);
        s->confirmationOption = -1;
        if (s->abilityListIndex >= D_801ECC10) {
            s->abilityListIndex = D_801ECC10 - 1;
            if (s->abilityListIndex < 0) {
                s->abilityListIndex = 0;
            }
            s->battlePrimaryListPageStart = s->abilityListIndex / 11;
        }
        exists = 0;
        i = 0;
        for (; i < D_801ECC10; i++) {
            if (D_801ECB60[i].slotIndex == g_gameState.gfs[s->gfId].learning) {
                exists = 1;
            }
        }
        if (exists == 0) {
            g_gameState.gfs[s->gfId].learning = 0;
        }
        if (func_801E457C(s->itemId, s->itemSlots, s->primaryListIndex) != 0) {
            *statePtr = 68;
        } else {
            s->abilityListSlideProgress = 4096;
            *statePtr = 4;
        }
        break;
    }

    case 77:
        *statePtr = 78;
        /* fallthrough */

    case 78:
        s->confirmationOption = -1;
        s->abilityListSlideProgress += 256;
        if (s->abilityListSlideProgress >= 4096) {
            s->abilityListSlideProgress = 4096;
            *statePtr = 12;
        }
        func_801E4708(0, s->menuActionId);
        func_801E4B38(0, s->secondaryListIndex, s->availableEntities);
        func_801E4848(1, s);
        break;

    case 79: {
        s32 i;
        u8 *item;
        s32 id;
        s32 count;
        for (i = 0; i < 202; i++) {
            buffer2[i] = 0;
        }
        item = &s->itemSlots->id;
        for (i = 0; i < 198; i++) {
            id = *item++;
            count = *item++;
            if (id != 0 && count != 0) {
                buffer2[id] = count;
            }
        }
        item = &s->itemSlots->id;
        for (i = 0; i < 198; i++) {
            *item++ = 0;
            *item++ = 0;
        }
        item = &s->itemSlots->id;
        for (i = 1; i < 199; i++) {
            if (buffer2[i] != 0) {
                *item++ = i;
                *item++ = buffer2[i];
            }
        }
        func_801F7B60();
        func_801E4708(1, s->menuActionId);
        s->menuActionId = 0;
        *statePtr = 3;
        break;
    }

    case 80: {
        MagazinePageEntry *magPage;
        magPage = D_801EB4BC;
        magPage += func_801E2E70(s->itemId);
        s->pageStart = func_801E2E70(s->itemId);
        func_801E2CCC(magPage->pageTextureCategory, magPage->pageTexture);
        loadSubOverlay(7, MENU_IMAGE_ADDR);
        loadSubOverlay(87, MENU_SUBOVERLAY_ADDR);
        *statePtr = 81;
        break;
    }

    case 81:
        s->menuColorIntensity -= 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity <= 0) {
            s->menuColorIntensity = 0;
            func_801F1DB0(0);
            if (pollCdReadStatus() == 0) {
                *statePtr = 82;
            }
        }
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        break;

    case 82:
        *statePtr = 83;
        break;

    case 83:
        s->brightness += 256;
        if (s->brightness >= 4096) {
            s->brightness = 4096;
            *statePtr = 84;
        }
        break;

    case 84:
        *statePtr = 85;
        /* fallthrough */

    case 85:
        if (func_801E2E70(s->itemId) != func_801E2E8C(s->itemId)) {
            if (btnFlags & PADLleft) {
                *statePtr = 86;
            } else {
                if (!(btnFlags & PADLright)) {
                    if (cfgFlags & PADRup) {
                        sendSpuCommand(3);
                        *statePtr = 90;
                    }
                    break;
                }
                *statePtr = 88;
            }
        } else {
            if (cfgFlags & PADRup) {
                sendSpuCommand(3);
                *statePtr = 90;
            }
        }
        break;

    case 86: {
        s32 page;
        sendSpuCommand(1);
        page = s->pageStart;
        s->pageEnd = page;
        page--;
        if (page < func_801E2E70(s->itemId)) {
            page = func_801E2E8C(s->itemId);
        }
        s->scrollOffset = -3968;
        s->pageStart = page;
        *statePtr = 87;
        break;
    }

    case 87:
        s->scrollOffset += 128;
        if (s->scrollOffset >= 0) {
            s->scrollOffset = 0;
            *statePtr = 85;
        }
        
        // Dead code added to match with the original game binary.
        s++; s--;
        
        if (cfgFlags & PADLleft) {
            *statePtr = 86;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 88;
        }
        break;

    case 88: {
        s32 page;
        sendSpuCommand(1);
        page = s->pageStart;
        s->pageEnd = page;
        page++;
        if (func_801E2E8C(s->itemId) < page) {
            page = func_801E2E70(s->itemId);
        }
        s->scrollOffset = 3968;
        s->pageStart = page;
        *statePtr = 89;
        break;
    }

    case 89:
        s->scrollOffset -= 128;
        if (s->scrollOffset <= 0) {
            s->scrollOffset = 0;
            *statePtr = 85;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 86;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 88;
        }
        break;
        
    case 90:
        *statePtr = 91;
        break;

    case 91:
        s->brightness -= 256;
        if (s->brightness <= 0) {
            s->brightness = 0;
            *statePtr = 92;
        }
        break;

    case 92:
        *statePtr = 93;
        break;

    case 93:
        s->menuColorIntensity += 256;
        func_801F1DB0(s->menuColorIntensity);
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            func_801F1DB0(4096);
            *statePtr = 5;
        }
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        break;

    case 94:
        s->pageStart = s->primaryListPageStart;
        s->secondaryListIndex = s->primaryListIndex;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        *statePtr = 95;
        /* fallthrough */

    case 95:                        
        s->unk5A -= 256;
        if (s->unk5A <= 0) {
            s->unk5A = 0;
            *statePtr = 96;
        }
        func_801E4708(0, s->menuActionId);
        func_801E47E0(1, s->primaryListIndex);
        break;

    case 96:
        *statePtr = 97;
        /* fallthrough */

    case 97: {
        s32 page;
        func_801E4708(0, s->menuActionId);
        page = s->primaryListIndex / 11;
        s->primaryListIndex = func_801F6768(btnFlags, 11, s->primaryListIndex % 11) + (page * 11);
        func_801E47E0(1, s->primaryListIndex);
        func_801E4BB4(s);
        if (btnFlags & PADLleft) {
            s->returnState = 97;
            *statePtr = 6;
        }
        if (btnFlags & PADLright) {
            s->returnState = 97;
            *statePtr = 8;
        }
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 105;
        }
        if (btnFlags & PADRdown) {
            sendSpuCommand(2);
            *statePtr = 98;
        }
        break;
    }

    case 98:
        *statePtr = 99;
        /* fallthrough */

    case 99: {
        s32 page;
        s32 rowInPage;
        page = s->secondaryListIndex / 11;
        rowInPage = s->secondaryListIndex % 11;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        s->secondaryListIndex = func_801F6768(btnFlags, 11, rowInPage) + (page * 11);
        func_801E476C(1, s);
        func_801E4C14(s);
        if (btnFlags & PADLleft) {
            *statePtr = 101;
        }
        if (btnFlags & PADLright) {
            *statePtr = 103;
        }
        if (!(cfgFlags & PADRdown)) {
            if (cfgFlags & PADRup) {
                sendSpuCommand(3);
                *statePtr = 97;
            }
        } else {
            state = 100;
            goto restart;
        }
        break;
    }

    case 100: {
        s32 id1;
        s32 id2;
        s32 count1;
        s32 count2;
        ItemSlot *item1;
        ItemSlot *item2;
        id1 = s->itemSlots[s->primaryListIndex].id;
        count1 = s->itemSlots[s->primaryListIndex].count;
        id2 = s->itemSlots[s->secondaryListIndex].id;
        count2 = s->itemSlots[s->secondaryListIndex].count;
        if (id1 == id2) {
            sendSpuCommand(5);
            *statePtr = 99;
        } else {
            item1 = &s->itemSlots[s->primaryListIndex];
            item2 = &s->itemSlots[s->secondaryListIndex];
            item1->id = id2;
            item1->count = count2;
            item2->id = id1;
            item2->count = count1;
            sendSpuCommand(2);
            *statePtr = 97;
        }
        break;
    }

    case 101: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->pageEnd = s->pageStart;
        page = s->secondaryListIndex / 11;
        rowInPage = s->secondaryListIndex % 11;
        page--;
        if (page < 0) {
            page = 17;
        }
        s->secondaryListIndex = rowInPage + (page * 11);
        s->pageStart = page;
        s->scrollOffset = -3687;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        func_801E476C(1, s);
        s->previousHelpMsg = s->helpMsg;
        func_801E4C14(s);
        *statePtr = 102;
        break;
    }

    case 102:
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        func_801E476C(1, s);
        s->scrollOffset += 409;
        if (s->scrollOffset >= 0) {
            s->scrollOffset = 0;
            *statePtr = 99;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 101;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 103;
        }
        break;

    case 103: {
        s32 page;
        s32 rowInPage;
        sendSpuCommand(1);
        s->pageEnd = s->pageStart;
        page = s->secondaryListIndex / 11;
        rowInPage = s->secondaryListIndex % 11;
        page++;
        if (page > 17) {
            page = 0;
        }
        s->secondaryListIndex = rowInPage + (page * 11);
        s->pageStart = page;
        s->scrollOffset = 3687;
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        func_801E476C(1, s);
        s->previousHelpMsg = s->helpMsg;
        func_801E4C14(s);
        *statePtr = 104;
        break;
    }

    case 104:
        func_801E4708(0, s->menuActionId);
        func_801E47E0(0, s->primaryListIndex);
        func_801E476C(1, s);
        s->scrollOffset -= 409;
        if (s->scrollOffset <= 0) {
            s->scrollOffset = 0;
            *statePtr = 99;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 101;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 103;
        }
        break;

    case 105:
        *statePtr = 106;
        /* fallthrough */

    case 106:
        func_801E4708(0, s->menuActionId);
        s->unk5A += 256;
        if (s->unk5A >= 4096) {
            s->unk5A = 4096;
            *statePtr = 2;
        }
        break;

    case 107: {
        s32 mask = func_801E2E70(s->itemId);
        mask = 1 << mask;
        state = 108;
        if (g_gameState.mainData.limitBreaks.quistisLimits & mask) {
            state = 109;
        }
        goto restart;
    }

    case 108: {
        u8 *msg;
        s32 mask;
        sendSpuCommand(8);
        func_801E457C(s->itemId, s->itemSlots, s->primaryListIndex);
        msg = func_801F08D4(1, 9, 15, 0);
        func_801E2BA4(msg, D_801ECC20, func_801E2E70(s->itemId));
        s->noticeMsg = D_801ECC20;
        mask = 1 << func_801E2E70(s->itemId);
        g_gameState.mainData.limitBreaks.quistisLimits |= mask;
        state = 110;
        goto restart;
    }

    case 109: {
        u8 *msg;
        sendSpuCommand(5);
        msg = func_801F08D4(1, 9, 16, 0);
        func_801E2BA4(msg, D_801ECC20, func_801E2E70(s->itemId));
        state = 110;
        s->noticeMsg = D_801ECC20;
        goto restart;
    }

    case 110:
        s->entityId = findNthSetBit(s->availableEntities, s->secondaryListIndex);
        s->unk5C = 600;
        *statePtr = 111;
        break;

    case 111:
        s->entityId = findNthSetBit(s->availableEntities, s->secondaryListIndex);
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk5C = 0;
            s->noticeMsg = NULL;
            *statePtr = 4;
        }
        if (s->unk5C < 0) {
            s->unk5C = 0;
            s->noticeMsg = NULL;
            *statePtr = 4;
        }
        break;

    case 112:
        loadOverlayWithTimCallback(9, MENU_IMAGE_ADDR);
        loadOverlayWithTimCallback(10, MENU_UNK_801D5000_ADDR);
        *statePtr = 113;
        break;

    case 113:
        s->menuColorIntensity -= 256;
        if (s->menuColorIntensity < 0) {
            s->menuColorIntensity = 0;
            if (pollCdReadStatus() == 0) {
                func_801F1CAC();
                func_801F72C4();
                func_801F18FC(s);
                func_801F0BB0();
            }
        }
        if (D_8008520C != 0) {
            func_801E4708(0, s->menuActionId);
            func_801E47E0(1, s->primaryListIndex);
            break;
        }
        func_801E4708(1, s->menuActionId);
        break;
    }

    setTextBrightness(s->brightness);
    func_801F0948(s->menuColorIntensity);
    if (D_8008520C != 0) {
        func_801F1DB0(s->menuColorIntensity);
    }
}

/**
 * @brief Draw one item row in the inventory list.
 *
 * Looks up the selected item slot for the current page and row, skips empty
 * entries, and renders the item's icon, name and quantity into the menu list.
 * The text color is white for usable items and gray for unusable ones.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param page Current inventory page index.
 * @param rowInPage Row index within the current page.
 * @param x Horizontal offset of the row in the menu.
 * @return Updated primitive pointer.
 */
static void *func_801E7D18(void *ot, void *prim, s32 page, s32 rowInPage, s32 x) {
    ItemSlot *item;
    s32 id;
    s32 count;
    s32 color;
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 x3;
    s32 y3;
    s32 x4;
    s32 y4;
    s32 x5;
    s32 y5;
    s32 tmp;

    item = g_menuDisplayCfg.dataPtr;
    item += page * 11 + rowInPage;

    id = item->id;
    count = item->count;
    if (id == 0 || count == 0) {
        return prim;
    }

    x1 = x + 13;
    x2 = g_menuDisplayCfg.x + x1;

    y1 = rowInPage * 13 + 9;
    y2 = g_menuDisplayCfg.y + y1;

    // Dead code added to match with the original game binary.
    tmp++; tmp--;

    if (func_801E2F88(id) != 0) {
        color = DIALOG_COLOR_WHITE;
    } else {
        color = DIALOG_COLOR_GRAY;
    }

    x3 = x2;
    y3 = y2 - 2;
    prim = drawIcon(ot, prim, func_801F6B28(id) + 223, x3, y3, g_menuTint[MENU_TINT_NORMAL]);

    tmp = y2;

    x4 = x2 + 13;
    y4 = tmp;
    prim = func_801F0FEC(ot, prim, x4, y4, getItemName(id), color);

    x5 = x2 + 145;
    y5 = tmp;
    return drawNumberMenuTint(ot, prim, (y5 << 0x10) | (x5 & 0xFFFF), count, color);
}

/** @brief Draw the primary item list panel. */
static void *func_801E7E74(ItemMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    g_menuDisplayCfg.iconType = ICON_ITEM;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 166;
    g_menuDisplayCfg.h = 160;
    g_menuDisplayCfg.columnCount = 11;
    g_menuDisplayCfg.pageStart = s->primaryListPageStart;
    g_menuDisplayCfg.pageEnd = s->primaryListPageEnd;
    g_menuDisplayCfg.scrollOffset = s->primaryListScrollOffset;
    g_menuDisplayCfg.dataPtr = s->itemSlots;
    prim = func_801F5EFC(ot, prim, x + 28, y, g_menuTint[MENU_TINT_NORMAL], s->primaryListPageStart);
    prim = func_801F5F60(ot, prim, g_menuTint[MENU_TINT_NORMAL], 3);
    return func_801EFBB4(ot, prim, func_801E7D18);
}

/** @brief Draw the secondary item list panel in the "Battle" tab. */
static void *func_801E7F4C(ItemMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    g_menuDisplayCfg.iconType = ICON_ITEM;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 166;
    g_menuDisplayCfg.h = 160;
    g_menuDisplayCfg.columnCount = 11;
    g_menuDisplayCfg.pageStart = s->pageStart;
    g_menuDisplayCfg.pageEnd = s->pageEnd;
    g_menuDisplayCfg.scrollOffset = s->scrollOffset;
    g_menuDisplayCfg.dataPtr = s->itemSlots;
    prim = func_801F5EFC(ot, prim, x + 28, y, g_menuTint[MENU_TINT_NORMAL], s->pageStart);
    prim = func_801F5F60(ot, prim, g_menuTint[MENU_TINT_NORMAL], 3);
    return func_801EFBB4(ot, prim, func_801E7D18);
}

/** @brief Draw the action panel with the following options: "Use", "Rearrange", "Sort" and "Battle". */
static void *func_801E8024(ItemMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    prim = func_801F5A38(ot, prim, x + 26, y + 6, 9, D_801EB1D8, 15);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 244;
    g_menuDisplayCfg.h = 18;
    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/** @brief Draw the string stored in dataPtr. */
static void *func_801E80D0(void *ot, void *prim, s32 dataIndex, s32 page, s32 x) {
    u8 str[128];
    s32 xOffset;
    s32 strX;
    s32 strY;
    u8 **ptr;
    ptr = g_menuDisplayCfg.dataPtr;
    if (ptr[dataIndex] != NULL) {
        xOffset = x + 10;
        strX = g_menuDisplayCfg.x + xOffset;
        strY = g_menuDisplayCfg.y + 9;
        decodeMessage(ptr[dataIndex], str, -1);
        prim = func_801F0FEC(ot, prim, strX, strY, str, 7);
    }
    return prim;
}

/** @brief Draw the help panel. */
static void *func_801E8180(ItemMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    g_menuDisplayCfg.iconType = ICON_HELP;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 324;
    g_menuDisplayCfg.h = 26;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.scrollOffset = s->primaryListScrollOffset + s->battlePrimaryListScrollOffset + s->scrollOffset;
    g_menuDisplayCfg.dataPtr = &s->helpMsg;
    return func_801EFBB4(ot, prim, func_801E80D0);
}

/** @brief Draws a GF name. */
static void *func_801E820C(void *ot, void *prim, s32 x, s32 y, s32 gfId) {
    s32 strX;
    s32 strY;
    s32 gfHpStatus;
    s32 color;
    u8 *magicName;
    strX = x + ((gfId & 1) * 82);
    strY = y + ((gfId / 2) * 13);
    gfHpStatus = func_801F2240(gfId);
    color = func_801F3FB4(gfHpStatus);
    magicName = getMagicNamePtr(gfId + 64);
    return drawDecodedText(ot, prim, strX, strY, magicName, color);
}

/** @brief Draws the GF panel. */
static void *menuitem_drawGfPanel(void *ot, void *prim, s32 x, s32 y, s32 arg4, u16 availableGfs) {
    MenuDisplayConfig *cfg;
    s32 gfX;
    s32 gfY;
    s32 i;
    cfg = &g_menuDisplayCfg;
    gfX = x + 4;
    gfY = y + 10;
    for (i = 0; i < GF_COUNT; i++) {
        if ((availableGfs >> i) & 1) {
            prim = func_801E820C(ot, prim, gfX, gfY, i);
        }
    } 
    cfg->iconType = ICON_NAME;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 168;
    cfg->h = 120;
    prim = DrawMenuWindow(ot, prim, arg4, g_menuTint[MENU_TINT_NORMAL]);
    return prim;
}

/**
 * @brief Draw the target panel for an item based on its targeting flags.
 *
 * Determines how the item can be used and chooses the correct panel renderer:
 * a character list panel or a GF list panel.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param x Horizontal position of the panel.
 * @param y Vertical position of the panel.
 * @param arg4 Render flags / tint configuration.
 * @param itemId Item identifier used to query its targeting flags.
 * @param arg6 Additional target panel parameter.
 * @param availableGfs Bitmask of available GF targets.
 * @return Updated primitive pointer.
 */
static void *func_801E83B4(void *ot, void *prim, s32 x, s32 y, s32 arg4, s32 itemId, u16 arg6, u16 availableGfs) {
    u8 itemFlags = func_801E2EA8(itemId);
    prim = func_801EF8D8(ot, prim);
    if (itemFlags & ITEM_FLAG_TARGET_ALL_BOTH) {
        prim = func_801F6C9C(ot, prim, x, y, arg4, arg6);
    } else if ((itemFlags & ITEM_FLAG_TARGET_CHARA) && (itemFlags & ITEM_FLAG_TARGET_GF)) {
        prim = func_801F6C9C(ot, prim, x, y, arg4, arg6);
    } else if (itemFlags & ITEM_FLAG_TARGET_GF) {
        prim = menuitem_drawGfPanel(ot, prim, x, y, arg4, availableGfs);
    } else {
        prim = func_801F6C9C(ot, prim, x, y, arg4, arg6);
    }
    return prim;
}

/**
 * @brief Draw one row of the battle item list.
 *
 * Resolves the item slot for the requested page and row, skips empty entries,
 * and renders the item name and quantity in the battle menu list.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param page Page index in the battle item list.
 * @param rowInPage Row index within the selected page.
 * @param x Horizontal offset for the row.
 * @return Updated primitive pointer.
 */
static void *func_801E84A4(void *ot, void *prim, s32 page, s32 rowInPage, s32 x) {
    MenuDisplayConfig *cfg;
    s32 color;
    ItemSlot *item;
    s32 id;
    s32 count;
    s32 xBase;
    s32 yBase;
    s32 xCoord;
    s32 yCoord;
    u8 *itemName;

    cfg = &g_menuDisplayCfg;
    color = DIALOG_COLOR_WHITE;

    item = D_801ECB20;
    item += page * 4 + rowInPage;
    count = item->count;
    id = item->id;

    if (count == 0) {
        return prim;
    }

    xBase = cfg->x + x;
    yBase = cfg->y + rowInPage * 13;

    itemName = getItemName(id);
    xCoord = xBase + 12;
    yCoord = yBase + 7;
    prim = func_801F0FEC(ot, prim, xCoord, yCoord, itemName, color);

    xCoord = xBase + 138;
    return drawNumberMenuTint(ot, prim, (yCoord << 0x10) | (xCoord & 0xFFFF), count, color);
}

/** @brief Draw the primary item list panel in the "Battle" tab. */
static void *func_801E859C(ItemMenuState *s, void* ot, void* prim, s32 x, s32 y) {
    prim = drawIcon(ot, prim, 77, x + 114, y, g_menuTint[MENU_TINT_NORMAL]);
    g_menuDisplayCfg.iconType = ICON_ITEM;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 150;
    g_menuDisplayCfg.h = 64;
    g_menuDisplayCfg.columnCount = 4;
    prim = func_801F5F30(ot, prim, x + 28, y, g_menuTint[MENU_TINT_NORMAL], g_menuDisplayCfg.pageStart);
    prim = func_801F5F60(ot, prim, g_menuTint[MENU_TINT_NORMAL], 3);
    return func_801EFBB4(ot, prim, func_801E84A4);
}

/** @brief Draw the two battle item list panels with a sliding offset. */
static void *func_801E8684(ItemMenuState *s, void *ot, void *prim) {
    s32 index;
    s32 xOffset;
    s32 x;
    s32 y;

    index = (4096 - s->battlePrimaryListSlideProgress) / 64;
    xOffset = D_801FA3C8[index];
    xOffset = xOffset * 192 / 4096;

    g_menuDisplayCfg.pageStart = s->battlePrimaryListPageStart;
    g_menuDisplayCfg.pageEnd = s->battlePrimaryListPageEnd;
    g_menuDisplayCfg.scrollOffset = s->battlePrimaryListScrollOffset;

    x = 53;
    x -= xOffset;
    y = 61;
    prim = func_801E859C(s, ot, prim, x, y);

    g_menuDisplayCfg.pageStart = s->pageStart;
    g_menuDisplayCfg.pageEnd = s->pageEnd;
    g_menuDisplayCfg.scrollOffset = s->scrollOffset;

    x = 181;
    x += xOffset;
    y = 134;
    return func_801E859C(s, ot, prim, x, y);
}

/**
 * @brief forget ability routine (Amnesia Greens item usage).
 */
/**
 * @brief Draw one row of the ability list.
 *
 * Resolves the ability entry for the requested page and row, skips invalid
 * entries, and renders its icon and name with the appropriate text color.
 * 
 * Called when a GF is selected after using Amnesia Greens.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param ablPage Ability page index.
 * @param ablRowInPage Row index within the page.
 * @param x Horizontal offset of the row.
 * @return Updated primitive pointer.
 */
static void *func_801E8780(void *ot, void *prim, s32 ablPage, s32 ablRowInPage, s32 x) {
    MenuDisplayConfig *cfg;
    AbilityListEntry *abl;
    s32 xCoord;
    s32 yCoord;
    s32 ablListIndex;
    s32 color;
    s32 slot;

    cfg = &g_menuDisplayCfg;
    ablListIndex = (ablPage * 11) + ablRowInPage;
    if (ablListIndex >= D_801ECC10) {
        return prim;
    }

    abl = &D_801ECB60[ablListIndex];
    slot = abl->slotIndex;

    color = DIALOG_COLOR_WHITE;
    if (abl->type == 1) {
        color = DIALOG_COLOR_GRAY;
    }

    xCoord = cfg->x + x;
    yCoord = cfg->y + (ablRowInPage * 13);

    xCoord += 13;
    yCoord += 9;

    prim = drawIcon(ot, prim, abl->category + 216, xCoord, yCoord - 2, g_menuTint[MENU_TINT_NORMAL]);
    return func_801F0FEC(ot, prim, xCoord + 13, yCoord, getAbilityName(slot), color);
}

/**
 * @brief Render the battle ability list panel.
 *
 * Called when a GF is selected after using Amnesia Greens.
 *
 * @param s Item menu state.
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param x Horizontal position of the list.
 * @param y Vertical position of the list.
 * @return Updated primitive pointer.
 */
static void *func_801E88AC(ItemMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    s32 arrows;
    g_menuDisplayCfg.iconType = ICON_ABILITY;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 166;
    g_menuDisplayCfg.h = 160;
    g_menuDisplayCfg.columnCount = 11;
    g_menuDisplayCfg.pageStart = s->battlePrimaryListPageStart;
    g_menuDisplayCfg.pageEnd = s->battlePrimaryListPageEnd;
    g_menuDisplayCfg.scrollOffset = s->battlePrimaryListScrollOffset;
    if (D_801ECC10 >= 12) {
        prim = func_801F5F30(ot, prim, x + 40, y, g_menuTint[MENU_TINT_NORMAL], g_menuDisplayCfg.pageStart);
        arrows = 2;
        if (s->battlePrimaryListPageStart != 0) {
            arrows = 1;
        }
        prim = func_801F5F60(ot, prim, g_menuTint[MENU_TINT_NORMAL], arrows);
    }
    return func_801EFBB4(ot, prim, func_801E8780);
}

/** @brief Get the string for the given ID. */
static u8 *func_801E89A4(s32 textId) {
    Mngrp_string *table = (Mngrp_string *)MENU_SUBOVERLAY_ADDR;
    return (u8 *)table + table->offsets[textId];
}

/**
 * @brief Draw the button-sequence combo for a Combat King magazine page.
 *
 * When the magazine page references a valid duel, it resolves the combo button
 * sequence and draws each button icon with separators between them. The
 * corresponding duel entry is also marked as unlocked in the save data.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param cfg Menu display config used to position the combo.
 * @param color Icon color to render with.
 * @param magPage Magazine entry describing the duel combo.
 * @return Updated primitive pointer.
 */
static void *func_801E89C0(void *ot, void *prim, MenuDisplayConfig *cfg, s32 color, MagazinePageEntry *magPage) {
    u16 *buttons;
    s32 x;
    s32 y;
    s32 i;
    u16 button;
    if (magPage->duelId != INVALID_ID) {
        g_gameState.mainData.limitBreaks.zellLimits |= 1 << magPage->duelId;
        buttons = g_kernel.duel[magPage->duelId].buttons;
        x = cfg->x + magPage->duelComboX;
        y = cfg->y + magPage->duelComboY;
        for (i = 0; i < 5; i++) {
            button = *buttons++;
            if (button == INVALID_BUTTON_ID) {
                break;
            }
            prim = drawIcon(ot, prim, findNthSetBit(button & 0xF0FF, 0) + 128, x, y, color);
            if (i > 0) {
                prim = drawIcon(ot, prim, ICON_DOT, x - 16, y, color);
            }
            x += 40;
        }
    }
    return prim;
}

/**
 * @brief Draw the weapon upgrade ingredients for a Weapons Monthly magazine page.
 *
 * Marks the weapon as unlocked and, for each non-empty recipe ingredient,
 * renders the item icon, name, and required quantity.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param cfg Menu display config used to position the ingredients.
 * @param color Text/icon color used for the rendered ingredients.
 * @param magPage Magazine page containing the layout and weapon ID.
 * @return Updated primitive pointer.
 */
static void *func_801E8AF0(void *ot, void *prim, MenuDisplayConfig *cfg, s32 color, MagazinePageEntry *magPage) {
    s32 xBase;
    s32 yBase;
    WeaponRecipe *recipe;
    u8 *ingredients;
    s32 i;
    s32 id;
    s32 count;
    s32 x;
    s32 y;
    u8 *itemName;
    s32 iconId;

    if (magPage->weaponId == INVALID_ID) {
        return prim;
    }

    g_gameState.mainData.party.unlockedWeapons |= 1 << magPage->weaponId;

    xBase = cfg->x;
    xBase += magPage->weaponListX;

    yBase = cfg->y;
    yBase += magPage->weaponListY;

    recipe = &D_801EB330;
    recipe += magPage->weaponId;
    ingredients = recipe->ingredients;
    
    for (i = 0; i < WEAPON_RECIPE_INGREDIENT_COUNT; i++) {
        id = *ingredients++;
        count = *ingredients++;

        if (id != 0) {
            x = xBase;
            y = yBase - 2;
            iconId = func_801F6B28(id) + 223;
            prim = drawIcon(ot, prim, iconId, x, y, g_menuTint[MENU_TINT_NORMAL]);
            
            x = xBase + 14;
            y = yBase;
            itemName = getItemName(id);
            prim = func_801F0FEC(ot, prim, x, y, itemName, 7);

            x = xBase + magPage->weaponXOffset;
            y = yBase;
            prim = drawNumberMenuTint(ot, prim, (y << 0x10) | (x & 0xFFFF), count, 7);
            yBase += magPage->weaponLineSpacing;
        }
    }

    return prim;
}

/**
 * @brief Draw the paper-mat overlay for a magazine page.
 *
 * Scales the provided base color by the page's paper-mat tint values and emits
 * a textured quad sized and positioned from the magazine metadata.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param cfg Menu display config used to place the overlay.
 * @param color Base color to tint the paper-mat.
 * @param magPage Magazine entry describing the paper-mat rectangle.
 * @return Updated primitive pointer.
 */
static void *func_801E8C88(void *ot, PaperMatPrim *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 r0;
    s32 g0;
    s32 b0;

    r0 = color & 0xFF;
    g0 = (color >> 8) & 0xFF;
    b0 = (color >> 0x10) & 0xFF;

    r0 = (r0 * magPage->paperMatRed) / 128;
    g0 = (g0 * magPage->paperMatGreen) / 128;
    b0 = (b0 * magPage->paperMatBlue) / 128;

    r0 &= 0xFF;
    g0 &= 0xFF;
    b0 &= 0xFF;

    if (magPage->paperMatHeight == 0 || magPage->paperMatWidth == 0) {
        return prim;
    }

    setlen(prim, 4);
    prim->tpage.code[0] = 0xE1000400;
    prim->code = 0x62;
    prim->r0 = r0;
    prim->g0 = g0;
    prim->b0 = b0;
    prim->x0 = cfg->x + magPage->paperMatX;
    prim->y0 = cfg->y + magPage->paperMatY;
    prim->w = magPage->paperMatWidth;
    prim->h = magPage->paperMatHeight;

    addPrimFast(ot, prim);

    return prim + 1;
}

/** @brief Return a SpriteTable pointer to whatever sprite table is currently loaded to the menu image buffer */
static SpriteTable *GetMenuImageBufferTable(void) {
    return (SpriteTable *)MENU_IMAGE_ADDR;
}

/**
 * @brief Draw picture overlays for a magazine page.
 *
 * Iterates up to four picture overlay entries and renders each non-empty sprite at
 * the configured offset within the page.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param cfg Menu display config used to place the overlays.
 * @param color Color tint applied to the sprites.
 * @param magPage Magazine page containing overlay metadata.
 * @return Updated primitive pointer.
 */
static void *func_801E8DB0(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 i;
    MagazinePageOverlay* picOverlay;

    picOverlay = magPage->pictureOverlays;
    for (i = 0; i < 4; i++, picOverlay++) {
        if (picOverlay->id != INVALID_ID) {
            s32 x = cfg->x + picOverlay->x;
            s32 y = cfg->y + picOverlay->y;
            prim = DrawSpriteFromTable(ot, prim, GetMenuImageBufferTable(), picOverlay->id, x, y, color);
        }
    }
    return prim;
}

/**
 * @brief Draw text overlays for a magazine page.
 *
 * Iterates up to four text overlay entries and renders each non-empty message at
 * the configured screen position, switching GPU packet state as needed.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param cfg Menu display config used to place the overlays.
 * @param color Color tint applied to the text.
 * @param magPage Magazine page containing overlay metadata.
 * @return Updated primitive pointer.
 */
static void *func_801E8E98(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 i;
    u32 pkt;
    MagazinePageOverlay *textOverlay;

    textOverlay = magPage->textOverlays;
    for (i = 0; i < 4; i++, textOverlay++) {
        if (textOverlay->id != INVALID_ID) {
            s32 x = cfg->x + textOverlay->x;
            s32 y = cfg->y + textOverlay->y;
            if (i & 1) {
                pkt = getDisplayListHead();
                storeGpuPacket(prim);
            }
            drawMessageText(ot, x, y, func_801E89A4(textOverlay->id));
            if (i & 1) {
                prim = getDisplayListHead();
                storeGpuPacket(pkt);
            }
        }
    }
    return prim;
}

/**
 * @brief Draw the sliced paper background for a magazine page.
 *
 * Computes the texture-window parameters for the page background and emits a
 * series of slice quads that cover the horizontal region described by the
 * magazine metadata.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Pointer to the primitive buffer.
 * @param cfg Menu display config used to place the background slice.
 * @param color Base color tint used for the slice.
 * @param magPage Magazine page describing the paper background layout.
 * @return Updated primitive pointer.
 */
static void *func_801E8FA8(void *ot, PaperMatSlicePrim *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 clut;
    s32 drawModeCmd;
    s32 texWindowCmd;
    s32 winXRaw;
    s32 winXSignAdjust;
    s32 winYSignAdjust;
    s32 sliceXOffset;
    s32 sliceWidth;
    s32 remainingWidth;
    s32 xOffset;

    clut = 0x36E0;
    winXRaw = (magPage->paperBgParamA + 896) * 4;
    drawModeCmd = (((magPage->paperBgParamA + 896) & 0x3FF) >> 6) | 0xE1000480;
    winXSignAdjust = winXRaw; 
    sliceXOffset = magPage->paperBgParamB;
    if (winXSignAdjust < 0) {
        winXSignAdjust += 255;
    }
    winYSignAdjust = sliceXOffset;
    winXRaw = ((winXRaw - (winXSignAdjust & 0x3F00)) >> 3) << 0xA;
    if (winYSignAdjust < 0) {
        winYSignAdjust += 255;
    }
    texWindowCmd = winXRaw | (((sliceXOffset - (winYSignAdjust & 0x100)) >> 3) << 0xF) | 0xE200039C;
    remainingWidth = magPage->windowWidth;

    setlen(prim, 2);
    prim->twin.code[0] = 0xE2000000;
    prim->twin.code[1] = 0;
    addPrimFast(ot, prim);
    prim = (PaperMatSlicePrim *)&prim->sprt.tag;

    xOffset = 0;
    while (remainingWidth > 0) {
        sliceWidth = remainingWidth;
        if (remainingWidth > 64) {
            sliceWidth = 64;
        }
        *(u32*)&prim->sprt.r0 = color;
        setlen(prim, 7);
        prim->twin.code[0] = drawModeCmd;
        prim->twin.code[1] = texWindowCmd;
        prim->sprt.tag = 0;
        prim->sprt.x0 = cfg->x + xOffset;
        prim->sprt.y0 = cfg->y;
        prim->sprt.w = sliceWidth;
        prim->sprt.h = cfg->h;
        prim->sprt.clut = clut;
        prim->sprt.u0 = 0;
        prim->sprt.v0 = 0;
        remainingWidth -= sliceWidth;
        addPrimFast(ot, prim);
        prim++;
        xOffset += 64;
    }
    return prim;
}

/**
 * @brief Render a complete magazine page.
 *
 * Composes the full visual payload for a magazine page by chaining several
 * render passes: text overlays, action-button icons, recipe/weapon info,
 * picture overlays, a tint overlay, the frame border, and the final sliced
 * paper background effect. If the page references an Angelo trick, it marks
 * that trick as known before the final overlay passes are drawn.
 *
 * @param ot Ordering-table pointer for the next primitive batch.
 * @param prim Current primitive-buffer write position.
 * @param cfg Active display configuration for the page.
 * @param color Base tint color for the rendered item.
 * @param magPage Magazine page to render.
 * @return Updated primitive-buffer write position after all layers are drawn.
 */
static void *func_801E90D8(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    prim = func_801E8E98(ot, prim, cfg, color, magPage);
    prim = func_801E89C0(ot, prim, cfg, color, magPage);
    prim = func_801E8AF0(ot, prim, cfg, color, magPage);

    if (magPage->angeloId != 0xFF) {
        s32 trick = magPage->angeloId;
        s32 known = g_gameState.mainData.limitBreaks.angeloKnown;
        s32 bit = 1 << trick;
        g_gameState.mainData.limitBreaks.angeloKnown = known | bit;
    }

    prim = func_801E8DB0(ot, prim, cfg, color, magPage);
    prim = func_801E8C88(ot, prim, cfg, color, magPage);
    prim = func_8002B898(ot, prim, cfg, color);
    return func_801E8FA8(ot, prim, cfg, color, magPage);
}

/**
 * @brief Configure the page rectangle and render the full magazine page.
 *
 * Copies the page's window rectangle from the magazine template, offsets it by
 * @p x and @p y, stores the result in g_menuDisplayCfg, and then invokes
 * func_801E90D8() to draw the complete page content at that position.
 *
 * @param ot Ordering table passed through to func_801E90D8().
 * @param prim Primitive-buffer pointer passed through to func_801E90D8().
 * @param x Horizontal offset added to the page's template X coordinate.
 * @param y Vertical offset added to the page's template Y coordinate.
 * @param magPage Magazine page providing the page layout and rendering data.
 * @return Updated primitive-buffer pointer after the page is rendered.
 */
static void *func_801E91E4(void *ot, void *prim, s32 x, s32 y, MagazinePageEntry *magPage) {
    g_menuDisplayCfg.x = magPage->windowX + x;
    g_menuDisplayCfg.y = magPage->windowY + y;
    g_menuDisplayCfg.w = magPage->windowWidth;
    g_menuDisplayCfg.h = magPage->windowHeight;
    return func_801E90D8(ot, prim, &g_menuDisplayCfg, g_menuTint[MENU_TINT_NORMAL], magPage);
}

/**
 * @brief Render a Weapons Monthly magazine page.
 * 
 * Displays the navigation hint at the bottom of the screen, draws
 * its tinted background, and renders the magazine page's paper
 * background using horizontal sprite slices.
 * 
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param magPage Pointer to the Weapons Monthly magazine page to be rendered.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801E9248(void *ot, void *prim, MagazinePageEntry *magPage) {
    u8* navigationHint;
    s32 x;
    s32 y;
    u16 width;
    u32 menuTint;

    // Dead code added to match with the original game binary.
    x = 0; if (x) { x++; x--; }

    menuTint = g_menuTint[MENU_TINT_NORMAL];
    navigationHint = func_801F08D4(1, 9, STRING_TO_SCROLL_TO_GO_TO_ITEM_SCREEN, 0);
    width = measureMessage(navigationHint);
    x = (336 - width) / 2;
    x += 24;
    y = 200;
    prim = func_801F0FEC(ot, prim, x, y, navigationHint, DIALOG_COLOR_WHITE);
    g_menuDisplayCfg.x = 24;
    g_menuDisplayCfg.y = 196;
    g_menuDisplayCfg.w = 336;
    g_menuDisplayCfg.h = 20;
    prim = func_8002B898(ot, prim, &g_menuDisplayCfg, menuTint);
    return func_801E8FA8(ot, prim, &g_menuDisplayCfg, menuTint, magPage);
}

/**
 * @brief Draw the status panel for a character or GF.
 *
 * Renders the entity's level, HP or selected stat value, active status icons,
 * and the final panel frame in the compact status layout.
 *
 * @param ot Ordering-table pointer for the primitive batch.
 * @param prim Current primitive-buffer write position.
 * @param x Horizontal panel offset.
 * @param y Vertical panel offset.
 * @param entityId Entity index to inspect, or a negative value to skip entity data.
 * @param charStatId Stat ID to display as a numeric value, or a negative value to show HP instead.
 * @return Updated primitive-buffer write position after the panel is drawn.
 */
static void *func_801E934C(void *ot, void *prim, s32 x, s32 y, s32 entityId, s32 charStatId) {
    u8 buffer[128];
    s32 curHp;
    s32 maxHp;
    u16 status;
    s32 color;
    s32 i;
    s32 xCoord;
    s32 yCoord;
    MenuDisplayConfig *cfg;
    s32 charStatVal;
    s32 level;
    u8 statusMask;
    u8 *msg;

    cfg = &g_menuDisplayCfg;
    if (entityId >= 0) {
        status = func_801F57A4(entityId);
        curHp = func_801F57DC(entityId);
        maxHp = func_801F58EC(entityId);
        level = func_801F5938(entityId);
        color = func_801F3FB4(status);
        xCoord = x + 8;
        yCoord = y + 8;
        prim = func_801F3ABC(level, ot, prim, xCoord, yCoord, color);
        xCoord = x + 72;
        yCoord = y + 6;
        prim = func_801F6418(entityId, ot, prim, xCoord, yCoord);
        if (charStatId >= 0) {
            xCoord = x + 8;
            yCoord = y + 23;
            msg = func_801F08D4(1, 9, 31, 0);
            func_801E2C0C(msg, buffer, 0, charStatId, 0);
            func_801E3EBC(entityId, 0, charStatId);
            charStatVal = D_801ECED8;
            prim = func_801F0FEC(ot, prim, xCoord, yCoord, buffer, color);
            xCoord = x + 164;
            prim = drawNumberMenuTint(ot, prim, (yCoord << 0x10) | (xCoord & 0xFFFF), charStatVal, color);
        } else {
            xCoord = x + 8;
            yCoord = y + 23;
            prim = func_801F3CE0(curHp, maxHp, ot, prim, xCoord, yCoord, color);
        }
        xCoord = x + 82;
        yCoord = y + 8;
        statusMask = status & (STATUS_POISON | STATUS_PETRIFY | STATUS_DARKNESS | STATUS_SILENCE | STATUS_BERSERK | STATUS_ZOMBIE);
        for (i = 0; i < 16; i++) {
            if ((statusMask >> i) & 1) {
                prim = drawIcon(ot, prim, i + 272, xCoord, yCoord, g_menuTint[MENU_TINT_NORMAL]);
                xCoord += 17;
            }
        }
    }
    cfg->iconType = ICON_STATUS;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 168;
    cfg->h = 40;
    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Render the item menu.
 *
 * Rendering is skipped unless the display mode is 2. A nonzero brightness
 * selects the magazine-page view, drawing the current page and, while
 * scrolling, the adjacent page. At zero brightness, the standard item-menu
 * panels are drawn instead, along with any active notices, confirmation, or
 * item-targeting panels.
 *
 * @param s Item menu state.
 * @param ot Ordering-table pointer for primitive generation.
 * @param prim Current primitive-buffer write position.
 * @return Updated primitive-buffer write position.
 */
static void *func_801E95C4(ItemMenuState *s, void *ot, void *prim) {
    s32 pkt;
    s32 aux;

    if (func_801F0D84() != 2) {
        return prim;
    }

    pkt = getDisplayListHead();

    if (s->brightness != 0) {
        MagazinePageEntry *magazinePage;
        s32 scrollOffset;
        s32 x;
        s32 y;

        func_801F1AFC();
        setMenuBrightness(s->brightness);
        x = 0;
        y = 0;
        scrollOffset = s->scrollOffset;
        if (scrollOffset != 0) {
            aux = D_801FA3C8[(scrollOffset < 0 ? -scrollOffset : scrollOffset) / 64];
            aux = scrollOffset < 0 ? -aux : aux;
            aux = aux * 384 / 4096;
            if (scrollOffset > 0) {
                x = aux - 384;
            } else {
                x = aux + 384;
            }
            magazinePage = D_801EB4BC;
            magazinePage += s->pageEnd;
            prim = func_801E91E4(ot, prim, x, y, magazinePage);
            x = aux;
        }
        magazinePage = D_801EB4BC;
        magazinePage += s->pageStart;
        prim = func_801E91E4(ot, prim, x, y, magazinePage);
        if (magazinePage->footerFlag != 0) {
            prim = func_801E9248(ot, prim, magazinePage);
        }
    } else {
        s32 brightness;
        s32 availableGfs;
        s32 mask;
        s32 x;
        s32 y;

        func_801F1AFC();
        setMenuBrightness(s->menuColorIntensity);

        if (s->noticeMsg != NULL) {
            pkt = func_801F4168(ot, pkt, s->noticeMsg, 192, 100, 4096, 86);
        }

        if (s->confirmationOption >= 0) {
            pkt = func_801F6FE4(ot, pkt, 1, 4096);
        }

        if (s->battlePrimaryListSlideProgress != 0) {
            prim = func_801E8684(s, ot, prim);
        }

        x = 24;
        y = 10;
        pkt = func_801E8024(s, ot, pkt, x, y);

        x = 30;
        y = 29;
        pkt = func_801E8180(s, ot, pkt, x, y);

        if (s->unk3E != 0) {
            aux = s->unk3E * s->menuColorIntensity / 4096;
            setMenuBrightness(aux);

            x = 26;
            y = 56;
            brightness = D_801FA3C8[s->abilityListSlideProgress / 64];
            x -= brightness * 190 / 4096;
            if (brightness < 4096) {
                prim = func_801E88AC(s, ot, prim, x, y);
            }

            if (s->unk5A < 4096) {
                x = 192 + D_801FA3C8[s->unk5A / 64] * 190 / 4096;
                func_801F0FD0(192, 358);
                prim = func_801E7F4C(s, ot, prim, x, y);
                func_801F0FD0(0, 0);
            }

            brightness = aux * s->unk5A / 4096;
            setMenuBrightness(brightness);

            x = 192;
            y = 56;
            availableGfs = s->availableGfs;

            mask = s->availableCharacters;
            if (func_801E2E54(s->itemId) & ITEM_FLAG_QUISTIS_ONLY) {
                mask &= 8;
                if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
                    mask = 0;
                }
            }
            
            prim = func_801E83B4(ot, prim, x, y, 4096, s->itemId, mask, availableGfs);
            setMenuBrightness(aux);
            func_801F0FD0(26, 192);
            y = 56;
            prim = func_801E7E74(s, ot, prim, 26, y);
            func_801F0FD0(0, 0);
            setMenuBrightness(brightness);
            pkt = func_801E934C(ot, pkt, 192, 176, s->entityId, s->statId);
            setMenuBrightness(s->menuColorIntensity);
        }
    }
    func_801F1B10();
    storeGpuPacket(pkt);
    return prim;
}

static void func_801E9AAC(ItemMenuState *s) {
    u16 availableCharacters;
    u16 availableGfs;
    s32 availableCharCount;
    s32 i;
    availableCharacters = func_80036EC0();
    availableGfs = getGfAvailabilityMask();
    s->availableCharacters = availableCharacters;
    s->availableGfs = availableGfs;
    availableCharCount = 0;
    for (i = 0; i < CHARACTER_COUNT; i++) {
        if ((availableCharacters >> i) & 1) {
            availableCharCount++;
        }
    }
    s->itemSlots = g_gameState.mainData.itemSlots;
    s->availableCharCount = availableCharCount;
    s->battleItemOrder = g_gameState.mainData.battleOrder;
    s->primaryListPageStart = 0;
    s->primaryListPageEnd = 0;
    s->primaryListScrollOffset = 0;
    s->pageStart = 0;
    s->pageEnd = 0;
    s->scrollOffset = 0;
    s->helpMsg = NULL;
    s->previousHelpMsg = NULL;
    s->menuActionId = 0;
    s->itemId = 0;
    s->unk3E = 4096;
    s->battlePrimaryListSlideProgress = 0;
    s->entityId = -1;
    s->noticeMsg = NULL;
    s->menuColorIntensity = 0;
    s->confirmationOption = -1;
    s->abilityListSlideProgress = 4096;
    s->unk5A = 4096;
    s->statId = -1;
    func_801E2D54(s->itemSlots, s->battleItemOrder);
}

/**
 * @brief Initialize item menu state.
 *
 * Sets up the main item handler and three data tables, clears six global
 * state words, calls display init, resets menu state. If the handler setup
 * returned a context pointer, initializes it with item data and renders.
 */
static void func_801E9B98(void) {
    ItemMenuState *s;

    s = func_801F179C(func_801E4EA4, func_801E95C4);
    func_801F1D2C(0, D_801EB17C, D_801EB330);
    func_801F1D2C(0, D_801EB188, D_801EB4BC);
    func_801F1D2C(0, D_801EB194, D_801EC710);
    D_801ECE24 = 0;
    D_801ECE28 = 0;
    D_801ECE2C = 0;
    D_801ECE30 = 0;
    D_801ECE34 = 0;
    D_801ECE38 = 0;
    func_801F0948(0);
    func_801E3E94();
    if (s != NULL) {
        func_801E9AAC(s);
        loadOverlayWithTimCallback(12, MENU_UNK_801D5000_ADDR);
        s->brightness = 0;
        func_801E4EA4(s);
        s->menuColorIntensity = 0;
    }
    func_801F7B60();
}
