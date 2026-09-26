#include "common.h"
#include "psxsdk/libetc.h"
#include "gamestate.h"
#include "menu.h"
#include "menushop.h"
#include "menumain.h"
#include "game.h"
#include "btl_color.h"

#define SHOP_ITEM_COUNT 16
#define ITEM_SLOT_COUNT 198

/** @brief Actions performed at the item shop menu. */
typedef enum {
    SHOP_BUY = 0,
    SHOP_SELL = 1,
} ShopAction;

typedef struct {
    u8 pad00[0x10];
    u16 state;                  /**< 0x10: state machine current state (0-17). */
    u8 pad12[0xD];              /**< 0x12 */
    u8 *itemDesc;               /**< 0x20: pointer to the item description. */
    u8 *previousItemDesc;       /**< 0x24: previous pointer to the item description, retained for the page transition animation. */
    u32 gil;                    /**< 0x28: current player's gil. */
    ItemSlot *itemSlots;        /**< 0x2C: pointer to the item slot inventory. */
    u8 *shopkeeperMessage;      /**< 0x30: shopkeeper message displayed in the message panel. */
    u8 pad34[2];                /* 0x34 */
    s16 menuTransitionProgress; /**< 0x36: transition progress for the item list, item info and gil panels (slide-in/out animation). */
    s16 menuColorIntensity;     /**< 0x38: menu color intensity level (fade in/out animation). */
    s16 scrollOffset;           /**< 0x3A: scroll offset progress for the page transition animation (prev/next page). */
    s16 listIndex[2];           /**< 0x3C: index of the selected item for each list (0 = buy, 1 = sell). */
    s8 pageStart;               /**< 0x40: page start for the page transition animation. */
    s8 pageEnd;                 /**< 0x41: page end for the page transition animation. */
    s8 selectedMenuOption;      /**< 0x42: selected menu option (0 = buy, 1 = sell, 2 = exit). */
    u8 pad43[2];                /* 0x43 */
    u8 shopId;                  /**< 0x45: menu shop ID. */
    u8 shopAction;              /**< 0x46: shop action (0 = buy, 1 = sell). */
    s8 pageCount;               /**< 0x47: page count. */
    s8 selectedQuantity;        /**< 0x48: selected item quantity to buy or sell. */
    s8 inventoryCount;          /**< 0x49: item quantity in the inventory. */
    u8 unk4A;                   /* 0x4A */
    u8 pad4B[1];                /* 0x4B */
    s16 unk4C;                  /* 0x4C */
    u16 unk4E;                  /* 0x4E */
} ShopMenuState;

typedef struct {
    u8 itemId;      /**< 0x0: item ID. */
    u8 rarity;      /**< 0x1: item rarity (0x00 = Rare, 0xFF = Common). */
} ShopItemRarity; /* 0x2 = 2 bytes */

typedef struct {
    u8 itemId;      /**< 0x0: item ID. */
    u8 visible;     /**< 0x1: item visibility flag. */
} ShopItemVisibility; /* 0x2 = 2 bytes */

typedef struct {
    u16 basePrice;  /**< 0x0: base price. */
    u8 sellRate;    /**< 0x2: sell rate. */
} ShopItemPrice; /* 0x3 = 3 bytes */

typedef struct {
    u8 type;    /**< 0x0: item type. */
    u8 flags;   /**< 0x1: targeting and usability bitfield. */
    u8 param1;  /**< 0x2: type-dependent (heal amount, GF id, ability id, magazine start page). */
    u8 param2;  /**< 0x3: type-dependent (status mask, stat mask, compatibility amount, magazine end page). */
} MItemEntry; /* 0x4 = 4 bytes */

/**
 * @brief String codes for the item shop menu options.
 * 
 * The options are Buy, Sell, and Exit. The fourth entry is -1 and
 * marks the end of the array.
 */
extern s16 D_801E9B64[4];

/**
 * @brief Shop ID remap table.
 *
 * Maps a field shop ID to a menu shop ID. The index is the field
 * shop ID, ranging from 0 to 51, while the stored value is the menu
 * shop ID.
 *
 * Values 0..20 correspond to regular item shops, and value 21 is
 * reserved for junk shops.
 */
extern u8 D_801E9B6C[52];

extern ShopItemRarity D_801EA170[SHOP_COUNT][SHOP_ITEM_COUNT]; /**< Shop item rarity tables (shop.bin content). */
extern ShopItemPrice D_801EA3F0[ITEM_PRICE_COUNT]; /**< Item price table (price.bin content). */
extern MItemEntry D_801EA70C[199]; /**< Field-menu item table (mitem.bin content). */
extern ShopItemVisibility D_801EAA28[SHOP_ITEM_COUNT]; /**< Shop item visibility table. */
extern s32 D_801EAA48[ITEM_PRICE_COUNT]; /**< Item sell prices. */
extern s32 D_801EAD68[ITEM_PRICE_COUNT]; /**< Item buy prices. */

static s32 func_801E5800(ShopMenuState*, ShopAction, s32);
static s32 func_801E583C(ShopMenuState*, ShopAction, s32);
static u8* func_801E58A0(ShopMenuState*, ShopAction, s32);
static s32 func_801E5904(s32);
static void func_801E5930(s32, s32, ShopMenuState*);
static s32 func_801E59D8(s16*, s16*, s32);
static s32 func_801E5A8C(s32, s32, s32, s32, s32, s16*, s32);
static void func_801E5BA4(s32, s32);
static void func_801E5DBC(void);
static void func_801E5E88(u8);
static void func_801E5E90(ShopMenuState*);
static void func_801E6A68(s32);
static void func_801E6ACC(void);
static void func_801E6C3C(s32);
static void func_801E6D54(s32);
static s32 func_801E6E0C(s32, s32, s32, s32);
static s32 func_801E6EB0(s32, s32, s32, s32, s32);
static s32 func_801E6F60(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E6FD8(s32, s32, s32, s32, s32);
static s32 func_801E722C(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E7374(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E7508(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E7628(ShopMenuState*, s32, s32, s32, s32);
static void func_801E791C(u8*, s32, s32, s32, s32);
static s32 func_801E79D4(ShopMenuState*, s32, s32);
static void func_801E7B9C(s32);
static void func_801E7C8C(s32);
static s32 func_801E7E4C(s32);
static s32 func_801E7E98(s32, u32);

/**
 * @brief Look up an item ID from the shop list or the inventory.
 *
 * If index >= ITEM_SLOT_COUNT, returns 0. If the shop action is buy,
 * reads the item ID from the shop-item table. Otherwise reads from
 * the item inventory.
 *
 * @param s Shop context pointer.
 * @param action Shop action.
 * @param index A shop-item index or an item inventory index.
 * @return Item ID, or 0 if out of range.
 */
static s32 func_801E5800(ShopMenuState *s, ShopAction action, s32 index) {
    if (index >= ITEM_SLOT_COUNT) {
        return 0;
    }

    if (action == SHOP_BUY) {
        return D_801EAA28[index].itemId;
    }

    index *= 2;
    return ((u8 *)s->itemSlots)[index];
}

/**
 * @brief Look up a shop item visible flag or an item quantity.
 *
 * If the shop action is buy, returns the visible flag from the
 * shop-item table. Otherwise, calls func_801E5800 to get an item ID,
 * then returns the value from the item quantities table indexed by
 * the item ID, or 0 if the item ID is 0.
 *
 * @param s Shop context parameter.
 * @param action Shop action.
 * @param index A shop-item index or an item inventory index.
 * @return Visible flag or item quantity, or 0 if not found.
 */
static s32 func_801E583C(ShopMenuState *s, ShopAction action, s32 index) {
    s32 itemId;

    if (action == SHOP_BUY) {
        return D_801EAA28[index].visible;
    }

    itemId = func_801E5800(s, action, index);
    if (itemId == 0) {
        return 0;
    }

    return D_801EB088[itemId];
}

/**
 * @brief Look up shop item and get its description string.
 *
 * Calls func_801E583C to validate the item, then if valid, calls
 * func_801E5800 to get item ID and getStatDesc to get its
 * description string.
 *
 * @param s Shop context parameter.
 * @param action Shop action.
 * @param index A shop-item index or an item inventory index.
 * @return Description string pointer, or NULL if invalid.
 */
static u8* func_801E58A0(ShopMenuState *s, ShopAction action, s32 index) {
    s32 valid = func_801E583C(s, action, index);
    if (valid) {
        return getStatDesc(func_801E5800(s, action, index));
    }
    return NULL;
}

/**
 * @brief Returns the glyph icon offset for a shop item.
 *
 * Looks up the item's type in the MItemEntry table and uses it
 * to index the item glyph icon offset table.
 *
 * @param itemId Item ID.
 * @return Item glyph icon offset.
 */
static s32 func_801E5904(s32 itemId) {
    return D_801F7F98[D_801EA70C[itemId].type];
}

/**
 * @brief Renders a cursor at the current selected item.
 * 
 * Calculates the cursor's Y position from the item index and derives
 * its X position from the menu transition progress to apply the
 * slide-in/out animation. The cursor is spaced 13 pixels vertically
 * between rows.
 * 
 * @param cursorRenderMode Cursor render mode: 0 = blinking, 1 = normal.
 * @param index Index of the selected item.
 * @param s Pointer to the shop menu state.
 */
static void func_801E5930(s32 cursorRenderMode, s32 index, ShopMenuState *s) {
    s32 val;
    s32 x;
    s32 y;

    y = (index % 8) * 13 + 82;

    val = s->menuTransitionProgress;
    x = D_801FA3C8[(val < 0 ? -val : val) / 64];
    x = (x * 360) / 4096;
    x += 35;

    func_801F0A34(cursorRenderMode, 0, x, y);
}

/**
 * @brief Calculates cumulative X offsets for a list of strings.
 * 
 * Processes the string codes in stringCodes until the -1 terminator.
 * For each string, obtains its width, adds 10 pixels of spacing, and
 * stores the resulting cumulative X offset in xOffsets. The first
 * output entry is always 0.
 * 
 * @param stringCodes Array of string codes terminated by -1.
 * @param xOffsets Output array receiving the cumulative X offsets for each string.
 * @param arg2 Parameter used to retrieve the strings.
 * @return Number of strings processed.
 */
static s32 func_801E59D8(s16 *stringCodes, s16 *xOffsets, s32 arg2) {
    s32 xOffset;
    s32 count;

    xOffset = 0;
    count = 0;
    *xOffsets = 0;
    xOffsets++;

    while (1) {
        s16 stringCode;
        u8 *str;
        s32 width;

        stringCode = *stringCodes;
        stringCodes++;

        if (stringCode == -1) {
            break;
        }

        str = func_801F08D4(1, arg2, stringCode, 0);
        width = getGlyphStatusU16(str);
        width += 10;
        xOffset += width;
        *xOffsets = xOffset;
        xOffsets++;
        count++;
    }

    return count;
}

/**
 * @brief Renders a list of strings horizontally with per-string coloring.
 * 
 * Calculates the cumulative X offset for each string, then renders each
 * string at its corresponding position. A string is rendered in white when
 * its corresponding bit is set in the mask; otherwise, it is rendered in gray.
 * 
 * @param arg0 First callback parameter.
 * @param arg1 Second callback parameter.
 * @param arg2 Base X position for the strings.
 * @param arg3 Y position shared by all strings.
 * @param arg4 Parameter used to retrieve the strings.
 * @param stringCodes Array of string codes terminated by -1.
 * @param stringMask Bitmask indicating which strings should be rendered in white.
 * @return Updated value of the second callback parameter.
 */
static s32 func_801E5A8C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s16 *stringCodes, s32 stringMask) {
    s16 xOffsets[36];
    s32 i;
    s32 count;
    s32 x;
    s32 y;
    s32 color;
    s32 str;
    s32 mask;

    y = arg3;
    count = func_801E59D8(stringCodes, xOffsets, arg4);
    for (i = 0; i < count; i++) {
        x = arg2 + xOffsets[i];
        mask = 1 << i;
        color = COLOR_GRAY;
        if (stringMask & mask) {
            color = COLOR_WHITE;
        }
        str = func_801F08D4(1, arg4, stringCodes[i], 0);
        arg1 = func_801F0FEC(arg0, arg1, x, y, str, color);
    }
    return arg1;
}

/**
 * @brief Renders the cursor for the selected menu option.
 *
 * Decodes the option strings X offsets into a local buffer
 * using func_801E59D8, then renders the cursor at the X position
 * corresponding to the selected menu option.
 *
 * @param cursorRenderMode Cursor render mode: 0 = blinking, 1 = normal.
 * @param selectedMenuOption Selected menu option: 0 = buy, 1 = sell, 2 = exit.
 */
static void func_801E5BA4(s32 cursorRenderMode, s32 selectedMenuOption) {
    s16 buf[36];
    func_801E59D8(D_801E9B64, buf, 3);
    func_801F0A34(cursorRenderMode, 0, buf[selectedMenuOption] + 36, 34);
}

/**
 * @brief Updates the player's gil and synchronizes the shop inventory.
 *
 * Rebuilds item counts from the temporary purchase table, clears depleted
 * slots, and adds any remaining items to the inventory.
 *
 * @param gil The updated gil amount.
 */
void func_801E5C08(u32 gil) {
    ItemSlot *itemSlots;
    ItemSlot *p;
    s32 i;

    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        g_gameState.mainData.party.dreamGil = gil;
    } else {
        g_gameState.mainData.party.gil = gil;
    }

    itemSlots = g_gameState.mainData.itemSlots;
    p = itemSlots;
    
    for (i = 0; i < ITEM_SLOT_COUNT; i++, p++) {
        u8 itemId = p->id;
        u8 itemCount = D_801EB088[itemId];

        if (itemCount == 0) {
            p->id = 0;
        }

        p->count = itemCount;
        D_801EB088[itemId] = 0;
    }

    p = itemSlots;

    for (i = 0; i < ITEM_PRICE_COUNT; i++) {
        u8 itemCount = D_801EB088[i];

        if (itemCount == 0) {
            continue;
        }

        while (1) {
            if (p->id == 0 || p->count == 0) {
                p->id = i;
                p->count = itemCount;
                func_800370AC(i);
                break;
            }
            p++;
        }
    }
}

/**
 * @brief Loads the current gil amount and rebuilds the available item counts.
 *
 * Item counts are read from the shop inventory table and stored by item ID.
 * 
 * @return The current gil amount (dream gil if party is locked, otherwise normal gil).
 */
s32 func_801E5D28(void) {
    s32 gil;
    u8* itemSlots;
    s32 i;

    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        gil = g_gameState.mainData.party.dreamGil;
    } else {
        gil = g_gameState.mainData.party.gil;
    }
    
    itemSlots = (u8 *)g_gameState.mainData.itemSlots;
    
    for (i = 0; i < ITEM_PRICE_COUNT; i++) {
        D_801EB088[i] = 0;
    }

    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        s32 itemId;
        s32 itemCount;

        itemId = *itemSlots;
        itemSlots++;
        
        itemCount = *itemSlots;
        itemSlots++;
        
        if (itemId != 0) {
            D_801EB088[itemId] = itemCount;
        }
    }

    return gil;
}

/**
 * @brief Compacts the shop item list in place.
 *
 * Scans the 16 entries in D_801EAA28 and keeps only entries whose
 * itemId and visible fields are both non-zero. Valid entries are moved to
 * the beginning of the table in their original order, while all
 * remaining entries are cleared.
 */
static void func_801E5DBC(void) {
    u8 buffer[SHOP_ITEM_COUNT];
    s32 counter;
    s32 i;
    ShopItemVisibility *shopItems;

    shopItems = D_801EAA28;
    counter = 0;

    for (i = 0 ; i < SHOP_ITEM_COUNT; i++, shopItems++) {
        if (shopItems->itemId != 0 && shopItems->visible != 0) {
            buffer[counter] = i;
            counter++;
        }
    }

    shopItems = D_801EAA28;

    for (i = 0; i < counter; i++) {
        if (i != buffer[i]) {
            shopItems[i] = shopItems[buffer[i]];
        }
    }

    for (; i < SHOP_ITEM_COUNT; i++) {
        shopItems[i].itemId = 0;
        shopItems[i].visible = 0;
    }
}

static void func_801E5E88(u8 arg0) {
}

/**
 * @brief Updates the item shop menu state machine.
 *
 * Handles shop tab selection, buy and sell item navigation, quantity
 * selection, page transitions, and the menu fade in/out.
 *
 * @param s Pointer to the shop menu state.
 */
static void func_801E5E90(ShopMenuState *s) {
    u16 btnFlags;
    u32 cfgFlags;
    u16 *statePtr;
    u16 state;

    statePtr = &s->state;
    btnFlags = g_menuDisplayCfg.inputNew;
    cfgFlags = g_menuDisplayCfg.inputRepeat;
    state = s->state;

restart:
    switch (state) {
    case 0:
        s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_WELCOME);
        s->menuColorIntensity = 0;
        *statePtr = 1;
        break;

    case 1:
        s->menuColorIntensity += 256;
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            *statePtr = 2;
        }
        func_801E5BA4(1, s->selectedMenuOption);
        break;

    case 2:
        s->selectedMenuOption = 0;
        *statePtr = 3;
        func_801E5BA4(1, s->selectedMenuOption);
        break;

    case 3:
        if (btnFlags & PADLright) {
            sendSpuCommand(1);
            s->selectedMenuOption = func_80035B28(7, s->selectedMenuOption);
        }

        if (btnFlags & PADLleft) {
            sendSpuCommand(1);
            s->selectedMenuOption = func_80035B70(7, s->selectedMenuOption);
        }

        func_801E5BA4(1, s->selectedMenuOption);

        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 16;
        }

        if (!(cfgFlags & PADRdown)) {
            break;
        }

        sendSpuCommand(2);

        if (s->selectedMenuOption == 2) {
            state = 16;
            goto restart;
        }

        s->shopAction = s->selectedMenuOption;
        if (s->shopAction == SHOP_BUY) {
            s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_WHAT_DO_YOU_WANT_TO_BUY);
            s->pageCount = 2;
        } else {
            s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_WHAT_DO_YOU_WANT_TO_SELL);
            s->pageCount = 25;
        }

        s->pageStart = s->listIndex[s->shopAction] / 8;
        func_801E5C08(s->gil);
        s->gil = func_801E5D28();
        *statePtr = 4;
        break;

    case 4:
        s->menuTransitionProgress = 3840;
        *statePtr = 5;
        /* fallthrough */

    case 5:
        s->menuTransitionProgress -= 256;
        if ((s->menuTransitionProgress * 4096) <= 0) {
            s->menuTransitionProgress = 0;
            *statePtr = 6;
        }
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        break;

    case 6: {
        s32 msgCode;
        s16 listIndex;
        s32 page;
        s32 rowInPage;

        if (btnFlags != 0) {
            s->unk4E = 0;
        }

        msgCode = STRING_SHOP_WELCOME2;
        if (s->unk4E != 0) {
            s->unk4E--;
            s->shopkeeperMessage = func_801F6AA4(msgCode);
        } else {
            msgCode = STRING_SHOP_WHAT_DO_YOU_WANT_TO_SELL;
            if (s->shopAction == SHOP_BUY) {
                msgCode = STRING_SHOP_WHAT_DO_YOU_WANT_TO_BUY;
            }
            s->shopkeeperMessage = func_801F6AA4(msgCode);
        }

        listIndex = s->listIndex[s->shopAction];
        rowInPage = (s16)(listIndex % 8);
        page = listIndex / 8;
        s->listIndex[s->shopAction] = func_801F6768(btnFlags, 8, rowInPage) + page * 8;

        s->itemDesc = func_801E58A0(s, s->shopAction, s->listIndex[s->shopAction]);

        if (btnFlags & PADLleft) {
            state = 7;
            goto restart;
        }
        if (btnFlags & PADLright) {
            state = 9;
            goto restart;
        }
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->itemDesc = NULL;
            s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_WELCOME);
            *statePtr = 14;
        }
        if (cfgFlags & PADRdown) {
            if (func_801E583C(s, s->shopAction, s->listIndex[s->shopAction])) {
                if (s->shopAction == SHOP_SELL && s->listIndex[s->shopAction] >= ITEM_SLOT_COUNT) {
                    sendSpuCommand(5);
                    break;
                }
                state = 11;
                goto restart;
            }
            sendSpuCommand(5);
        }
        break;
    }

    case 11: {
        s32 index;
        u32 price;
        u32 count;

        index = func_801E5800(s, s->shopAction, s->listIndex[s->shopAction]);
        s->selectedQuantity = 1;
        price = D_801EAD68[index];

        if (s->shopAction == SHOP_BUY) {
            if (index == 0) {
                sendSpuCommand(5);
                break;
            }

            if (s->gil < price) {
                u8 *msg;
                sendSpuCommand(5);
                msg = func_801F6AA4(STRING_SHOP_NOT_ENOUGH_MONEY);
                initSfxPlayback(0, msg);
                func_801F23D0(0, 104, msg);
                setSfxPitch(0, 0);
                startSfxNormal(0);
                s->unk4C = 600;
                *statePtr = 15;
                break;
            }

            if (D_801EB088[index] >= 100) {
                u8 *msg;
                sendSpuCommand(5);
                msg = func_801F6AA4(STRING_SHOP_CANT_CARRY_ANY_MORE);
                func_801F23D0(0, 104, msg);
                initSfxPlayback(0, msg);
                setSfxPitch(0, 0);
                startSfxNormal(0);
                s->unk4C = 600;
                *statePtr = 15;
                break;
            }

            count = s->gil / price;
            if ((s32)count + D_801EB088[index] >= 100) {
                count = 100 - D_801EB088[index];
            }
        } else {
            if (index == 0) {
                sendSpuCommand(5);
                break;
            }
    
            count = D_801EB088[index];
        }

        sendSpuCommand(2);
        s->inventoryCount = count;
        s->unk4A = 64;
        if (s->shopAction == SHOP_BUY) {
            s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_BUY_HOW_MANY);
        } else {
            s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_SELL_HOW_MANY);
        }
        *statePtr = 12;
        break;
    }

    case 15:
        s->unk4C--;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if (s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 6;
        }
        break;

    case 12:
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(0, s->listIndex[s->shopAction], s);

        if (btnFlags & PADLright) {
            if (s->selectedQuantity < s->inventoryCount) {
                s->selectedQuantity++;
                sendSpuCommand(1);
            }
        }

        if (btnFlags & PADLleft) {
            if (s->selectedQuantity >= 2) {
                s->selectedQuantity--;
                sendSpuCommand(1);
            }
        }

        if (btnFlags & PADLdown) {
            if (s->selectedQuantity >= 2) {
                sendSpuCommand(1);
                s->selectedQuantity -= 10;
                if ((s->selectedQuantity * 16777216) <= 0) {
                    s->selectedQuantity = 1;
                }
            }
        }

        if (btnFlags & PADLup) {
            if (s->selectedQuantity < s->inventoryCount) {
                s->selectedQuantity += 10;
                sendSpuCommand(1);
                if (s->selectedQuantity > s->inventoryCount) {
                    s->selectedQuantity = s->inventoryCount;
                }
            }
        }

        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 13;
        }

        if (cfgFlags & PADRdown) {
            s32 index;
            s32 price;
            if (s->shopAction == SHOP_BUY) {
                playSoundEffect(20);
                index = func_801E5800(s, s->shopAction, s->listIndex[s->shopAction]);
                price = D_801EAD68[index];
                price *= s->selectedQuantity;
                s->gil -= price;
                D_801EB088[index] += s->selectedQuantity;
            } else {
                if (s->listIndex[s->shopAction] >= ITEM_SLOT_COUNT) {
                    sendSpuCommand(5);
                    break;
                }

                playSoundEffect(20);
                index = func_801E5800(s, s->shopAction, s->listIndex[s->shopAction]);
                price = D_801EAA48[index];
                price *= s->selectedQuantity;
                s->gil += price;
                if (s->gil > 99999999) {
                    s->gil = 99999999;
                }

                D_801EB088[index] -= s->selectedQuantity;
            }
            s->unk4E = 60;
            *statePtr = 13;
        }
        break;

    case 13:
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(0, s->listIndex[s->shopAction], s);
        s->unk4A = 0;
        *statePtr = 6;
        break;

    case 14:
        func_801E5BA4(0, s->selectedMenuOption);
        s->menuTransitionProgress += 256;
        if (s->menuTransitionProgress >= 4096) {
            s->menuTransitionProgress = 4096;
            *statePtr = 3;
        }
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        break;

    case 7: {
        s32 listIndex;
        s32 rowInPage;
        s32 page;
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        s->previousItemDesc = s->itemDesc;
        listIndex = s->listIndex[s->shopAction];
        rowInPage = (s16)(listIndex % 8);
        page = listIndex / 8;
        s->pageEnd = page;
        page--;
        if (page < 0) {
            page = (u8)s->pageCount - 1;
        }
        s->listIndex[s->shopAction] = rowInPage + page * 8;
        s->pageStart = page;
        s->itemDesc = func_801E58A0(s, s->shopAction, s->listIndex[s->shopAction]);
        s->scrollOffset = -3687;
        sendSpuCommand(1);
        *statePtr = 8;
        break;
    }

    case 8:
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        s->scrollOffset += 409;
        if (s->scrollOffset >= 0) {
            s->scrollOffset = 0;
            *statePtr = 6;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 7;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 9;
        }
        break;

    case 9: {
        s32 listIndex;
        s32 rowInPage;
        s32 page;
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        s->previousItemDesc = s->itemDesc;
        listIndex = s->listIndex[s->shopAction];
        rowInPage = (s16)(listIndex % 8);
        page = listIndex / 8;
        s->pageEnd = page;
        page++;
        if (page >= (u8)s->pageCount) {
            page = 0;
        }
        s->listIndex[s->shopAction] = rowInPage + page * 8;
        s->pageStart = page;
        s->itemDesc = func_801E58A0(s, s->shopAction, s->listIndex[s->shopAction]);
        s->scrollOffset = 3687;
        sendSpuCommand(1);
        *statePtr = 10;
        break;
    }

    case 10:
        func_801E5BA4(0, s->selectedMenuOption);
        func_801E5930(1, s->listIndex[s->shopAction], s);
        s->scrollOffset -= 409;
        if (s->scrollOffset <= 0) {
            s->scrollOffset = 0;
            *statePtr = 6;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 7;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 9;
        }
        break;

    case 16:
        s->shopkeeperMessage = func_801F6AA4(STRING_SHOP_COME_BACK_SOON);
        *statePtr = 17;
        /* fallthrough */

    case 17:
        s->menuColorIntensity -= 256;
        if (s->menuColorIntensity * 4096 <= 0) {
            s->menuColorIntensity = 0;
            func_801E5C08(s->gil);
            func_801E5E88(s->shopId);
            func_801F6888();
            func_801F7B60();
            func_801F18FC(s);
            func_801F0BB0();
        }
        func_801E5BA4(1, s->selectedMenuOption);
        break;
    }

    func_801F0948(s->menuColorIntensity);
}

/**
 * @brief Populate shop item visibility data for a shop.
 *
 * Copies item IDs from the shop's rarity table and visibility flags from
 * the shop inventory into the shared visibility table.
 * 
 * @param shopId Menu shop ID.
 */
static void func_801E6A68(s32 shopId) {
    ShopItemVisibility *visibility;
    ShopItemRarity *rarity;
    ShopData *shop;
    s32 i;

    visibility = D_801EAA28;

    rarity = D_801EA170[0];
    rarity += shopId * 16;

    shop = &g_gameState.shops[shopId];

    for (i = 0; i < SHOP_ITEM_COUNT; i++) {
        visibility->itemId = rarity[i].itemId;
        visibility->visible = shop->items[i];
        visibility++;
    }
}

/**
 * @brief Calculates and stores shop buy and sell prices.
 *
 * The flags returned by func_801F72B4 determine the price modifiers.
 * If bit 0 is set, applies a 25% discount to buy prices (75% of the normal price).
 * If bit 1 is set, increase sell prices by 50% (150% of the normal price).
 * The final calculated prices are written to the respective buy and sell price
 * arrays/lists and are clamped to a minimum value of 1.
 */
static void func_801E6ACC(void) {
    ShopItemPrice *itemPrice;
    s32 mask;
    s32 i;
    s32 tmp1;
    s32 tmp2;

    mask = func_801F72B4();
    itemPrice = D_801EA3F0;
    
    for (i = 0; i < ITEM_PRICE_COUNT; i++) {
        if (mask & 1) {
            D_801EAD68[i] = (itemPrice[i].basePrice * 15) / 2;
        } else {
            D_801EAD68[i] = itemPrice[i].basePrice * 10;
        }

        if (mask & 2) {
            tmp1 = itemPrice[i].sellRate * 10;
            tmp2 = itemPrice[i].basePrice * tmp1;
            D_801EAA48[i] = tmp2 * 3 / 40;
        } else {
            tmp1 = itemPrice[i].sellRate * 10;
            tmp2 = itemPrice[i].basePrice * tmp1;
            D_801EAA48[i] = tmp2 / 20;
        }
        
        if (D_801EAA48[i] == 0) {
            D_801EAA48[i] = 1;
        }
        if (D_801EAD68[i] == 0) {
            D_801EAD68[i] = 1;
        }
    }
}

/**
 * @brief Update shop item visibility based on rarity and flags.
 *
 * For each item in the shop, computes a value based on the item's rarity.
 * If the computed value exceeds 128, the item is marked as visible;
 * otherwise, it is hidden. The visibility flags are stored in both the
 * shop visibility table and the shop-item list.
 * 
 * @param shopId Menu shop ID.
 */
static void func_801E6C3C(s32 shopId) {
    ShopItemRarity* rarity;
    ShopItemVisibility* p;
    ShopData *shop;
    s32 i;
    s32 bitMask;

    p = D_801EAA28;
    rarity = D_801EA170[0];
    rarity += shopId * 16;
    shop = &g_gameState.shops[shopId];
    bitMask = (func_801F72B4() << 6) & 0x100;
    
    for (i = 0; i < SHOP_ITEM_COUNT; i++, p++) {
        if (p->itemId != 0) {
            s32 aux = rarity[i].rarity;
            s32 val = bitMask + aux;
            
            aux = 255;
            if (val == aux) {
                val = 256;
            }
            
            func_801F6A5C();
            
            aux = 128;
            if (val > aux) {
                aux = 1;
                p->visible = aux;
            } else {
                p->visible = 0;
            }
            
            if (val == 0) {
                p->visible = 0;
            }
            
            shop->items[i] = p->visible;
        }
    }
}

/**
 * @brief Marks a shop as visited.
 *
 * Skips if the shop ID is 21 (junk shop).
 *
 * @param shopId Menu shop ID.
 */
static void func_801E6D54(s32 shopId) {
    GameState *gs;
    u32 visited;
    s32 sum;

    if (shopId != 21) {
        gs = &g_gameState;

        sum = gs->mainData.fieldCDC;

        visited = gs->shops[shopId].visited;

        sum += gs->mainData.fieldCE2;
        sum += gs->mainData.fieldCE0;
        sum &= 0xFFFF;
        if (sum == 0) {
            sum = 1;
        }

        gs->shops[shopId].visited = 1;

        // Dead code retained to match with the original game binary.
        visited &= 0xFFFF;
        if (sum < visited) {
            sum += 0x10000;
        }
        sum -= visited;

        func_801E6A68(shopId);
        func_801E6C3C(shopId);
        func_801E6C3C(shopId);
        func_801E5DBC();
        func_801E6ACC();

        // Dead code added to match with the original game binary.
        if (sum) {
            sum++; sum--;
        }
    }
}

/**
 * @brief Draws the option panel for the item shop menu.
 * 
 * The option panel displays the Buy, Sell and Exit options.
 * 
 * @param arg0 First callback parameter.
 * @param arg1 Second callback parameter.
 * @param arg2 X position for the display configuration.
 * @param arg3 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E6E0C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    arg1 = func_801E5A8C(arg0, arg1, arg2 + 12, arg3 + 5, 3, D_801E9B64, 0x7);
    g_menuDisplayCfg.iconType = ICON_NONE;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg2;
    g_menuDisplayCfg.y = arg3;
    g_menuDisplayCfg.w = 336;
    g_menuDisplayCfg.h = 21;
    return func_801EF9AC(arg0, arg1, 4096, g_menuColor);
}

/**
 * @brief Draws a string text for a panel.
 *
 * Check whether dataPtr at the specified index contains a valid
 * string. If so, decodes the string and renders it at the menu
 * display position.
 * 
 * @param arg0 First callback parameter.
 * @param arg1 Second callback parameter.
 * @param arg2 Index for dataPtr.
 * @param arg3 Unused parameter.
 * @param arg4 X position for the display configuration.
 * @return The updated value of the second callback parameter.
 * 
 * @note Identical to func_801E8AB0, except for the Y offset.
 */
static s32 func_801E6EB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    u8 buffer[128];
    s32 msg;
    s32 x;
    s32 y;
    s32 xOffset;

    msg = ((s32 *)(g_menuDisplayCfg.dataPtr))[arg2];
    if (msg != 0) {
        xOffset = arg4 + 10;
        x = g_menuDisplayCfg.x + xOffset;
        y = g_menuDisplayCfg.y + 5;
        decodeMessage(msg, buffer, -1);
        arg1 = func_801F0FEC(arg0, arg1, x, y, buffer, COLOR_WHITE);
    }
    return arg1;
}

/**
 * @brief Draws the item description panel for the item shop menu.
 *
 * The item description panel displays the description for the currently
 * selected item in the shop. The draw of description is delegated
 * to func_801E6EB0.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for the display configuration.
 * @param a4 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E6F60(ShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 a4) {
    g_menuDisplayCfg.iconType = ICON_NONE;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 324;
    g_menuDisplayCfg.h = 20;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = a4;
    g_menuDisplayCfg.scrollOffset = s->scrollOffset;
    g_menuDisplayCfg.dataPtr = (s32)&s->itemDesc;
    return func_801EFBB4(a1, a2, (s32)&func_801E6EB0);
}

/**
 * @brief Draws an item row for the item shop menu.
 * 
 * Draws the item glyph and name. If the shop action is buy, draws the
 * item buy price. If the shop action is sell, draws the sell price and
 * the current item inventory quantity.
 * 
 * @param arg0 First callback parameter.
 * @param arg1 Second callback parameter.
 * @param arg2 Offset used to calculate the index in the shop item list.
 * @param arg3 Y position for the display configuration.
 * @param arg4 X position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E6FD8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    ShopMenuState *s;
    s32 itemId;
    s32 price;
    s32 count;
    s32 shopItemIdx;
    s32 xBase;
    s32 yBase;

    cfg = &g_menuDisplayCfg;
    shopItemIdx = arg2 * 8 + arg3;
    xBase = cfg->x + arg4;
    yBase = cfg->y + arg3 * 13;

    s = (ShopMenuState *)cfg->dataPtr;
    itemId = func_801E5800(s, s->shopAction, shopItemIdx);

    if (s->shopAction == SHOP_SELL) {
        price = D_801EAA48[itemId];
        count = D_801EB088[itemId];
        if (shopItemIdx >= ITEM_SLOT_COUNT) {
            return arg1;
        }
    }

    else {
        price = D_801EAD68[itemId];
        count = D_801EAA28[shopItemIdx].visible;
    }

    if (itemId != 0 && count != 0) {
        s32 color;
        s32 itemName;
        s32 itemGlyph;
        s32 x;
        s32 y;

        color = COLOR_WHITE;
        if (s->gil < D_801EAD68[itemId] && s->shopAction == SHOP_BUY) {
            color = COLOR_GRAY;
        }

        itemName = getStatName(itemId);
        itemGlyph = func_801E5904(itemId) + 223;

        arg1 = func_8002FF34(arg0, arg1, itemGlyph, xBase + 11, yBase + 8, g_menuColor);

        x = xBase + 25;
        y = yBase + 10;
        arg1 = func_801F0FEC(arg0, arg1, x, y, itemName, color);

        if (s->shopAction == SHOP_BUY) {
            x = xBase + 240;
            arg1 = drawColorByMenuPalette(arg0, arg1, (y << 0x10) | (x & 0xFFFF), price, color);
        }

        else {
            x = xBase + 200;
            arg1 = drawColorByMenuPalette(arg0, arg1, (y << 0x10) | (x & 0xFFFF), price, color);
            x = xBase + 240;
            arg1 = drawColorByMenuPalette(arg0, arg1, (y << 0x10) | (x & 0xFFFF), count, color);
        }
    }

    return arg1;
}

/**
 * @brief Draws the item info panel for the item shop menu.
 * 
 * The item info panel displays information about the selected item. If
 * the shop action is buy, draws the current inventory quantity for the
 * selected item. Otherwise, draws a blank panel.
 * 
 * @param s Pointer to the shop state.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E722C(ShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    s32 color;

    cfg = &g_menuDisplayCfg;
    color = COLOR_WHITE;

    if (s->shopAction == SHOP_BUY) {
        s32 x;
        s32 y;
        s32 result;
        s32 index;

        x = arg3 + 8;
        y = arg4 + 9;
        result = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(STRING_QUANTITY), color);

        // Dead code added to match with the original game binary.
        while (0);

        x = arg3 + 66;
        y = arg4 + 22;
        index = func_801E5800(s, s->shopAction, s->listIndex[s->shopAction]);
        arg2 = drawColorByMenuPalette(arg1, result, (y << 0x10) | (x & 0xFFFF), D_801EB088[index], color);
    }

    cfg->iconType = ICON_INFO;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->w = 80;
    cfg->y = arg4;
    cfg->h = 119;

    return func_801EF9AC(arg1, arg2, 4096, g_menuColor);
}

/**
 * @brief Draws the item list panel for the item shop menu.
 * 
 * The item list panel displays the list of items available for buy
 * or sell. If shop action is buy, the list contains the columns Item
 * and Price. Otherwise, the list contains the columns Item, Price
 * and Num. The draw of each row is delegated to func_801E6FD8.
 * 
 * @param s Pointer to the shop state.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E7374(ShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    g_menuDisplayCfg.iconType = ICON_ITEM;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 256;
    g_menuDisplayCfg.h = 119;
    g_menuDisplayCfg.columnCount = 8;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.pageStart = s->pageStart;
    g_menuDisplayCfg.pageEnd = s->pageEnd;
    g_menuDisplayCfg.scrollOffset = s->scrollOffset;
    g_menuDisplayCfg.dataPtr = (s32)s;

    if (s->shopAction == SHOP_BUY) {
        arg2 = func_8002FF34(arg1, arg2, ICON_PRICE, arg3 + 168, arg4, g_menuColor);
        arg2 = func_801F5F30(arg1, arg2, arg3 + 28, arg4, g_menuColor, s->pageStart);
    }
    else {
        arg2 = func_8002FF34(arg1, arg2, ICON_PRICE, arg3 + 128, arg4, g_menuColor);
        arg2 = func_8002FF34(arg1, arg2, ICON_NUM, arg3 + 214, arg4, g_menuColor);
        arg2 = func_801F5EFC(arg1, arg2, arg3 + 28, arg4, g_menuColor, s->pageStart);
    }
    
    arg2 = func_801F5F60(arg1, arg2, g_menuColor, 3);
    return func_801EFBB4(arg1, arg2, func_801E6FD8);
}

/**
 * @brief Draws the gil panel for the item shop menu.
 * 
 * The gil panel displays the "Money" label, the player's current
 * gil amount, and a small icon next to the gil amount.
 * 
 * @param s Pointer to the shop state.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E7508(ShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 x;
    s32 y;

    x = arg3 + 208;
    y = arg4 + 6;
    arg2 = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(STRING_MONEY), COLOR_WHITE);

    x = arg3 + 322;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), s->gil, COLOR_WHITE);

    x = arg3 + 323;
    y = arg4 + 8;
    arg2 = func_8002FF34(arg1, arg2, ICON_GIL, x, y, g_menuColor);

    g_menuDisplayCfg.iconType = ICON_INFO;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 336;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.h = 23;

    return func_801EF9AC(arg1, arg2, 4096, g_menuColor);
}

/**
 * @brief Draws the item quantity selection dialog.
 * 
 * Draws the "Quantity" and "Price" labels, the selected quantity
 * and the total price for the quantity of the currently selected
 * item. The total price is calculated by multiplying the unit price
 * of the item by the selected quantity.
 * 
 * @param s Pointer to the shop state.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E7628(ShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 x;
    s32 y;
    s32 index;
    s32 color;
    s32 price;
    MenuDisplayConfig *cfg;

    color = COLOR_WHITE;
    cfg = &g_menuDisplayCfg;
    if (s->unk4A == 0) {
        return arg2;
    }

    x = arg3 + 11;
    y = arg4 + 9;
    arg2 = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(STRING_QUANTITY2), color);

    y = arg4 + 35;
    arg2 = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(STRING_PRICE), color);

    x = arg3 + 96;
    y = arg4 + 24;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), s->selectedQuantity, color);

    y = arg4 + 50;

    index = func_801E5800(s, s->shopAction, s->listIndex[s->shopAction]);
    if (s->shopAction == SHOP_BUY) {
        price = D_801EAD68[index];
    } else {
        price = D_801EAA48[index];
    }

    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), price * s->selectedQuantity, color);

    cfg->iconType = ICON_PRICE;
    cfg->w = 104;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->y = arg4;
    cfg->h = 66;

    return func_801EF9AC(arg1, arg2, 4096, g_menuColor);
}

/**
 * @brief Draws the title panel for the shop menus.
 * 
 * The title panel displays the shop name. If the field shop ID is
 * negative, it displays "Junk Shop Ability" instead and uses a
 * narrower panel.
 * 
 * @param fieldShopId Field shop ID.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of the second parameter.
 */
s32 func_801E77EC(s32 fieldShopId, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    s32 name;
    s32 x;
    s32 y;
    s32 flag;
    s32 width;
    s32 result;

    result = func_801EFFB8();

    if (result == 23) {
        flag = 1;
        width = 336;
    } else {
        flag = 0;
        width = 244;
    }

    cfg = &g_menuDisplayCfg;

    if (fieldShopId >= 0) {
        name = func_801F6AA4(fieldShopId + 1);
    } else {
        name = func_801F6AA4(STRING_JUNK_SHOP_ABILITY);
    }

    x = (width - getGlyphStatusU16(name)) / 2;
    x += arg3;

    y = 5;
    y += arg4;

    arg2 = func_801F0FEC(arg1, arg2, x, y, name, COLOR_WHITE);

    cfg->iconType = ICON_NAME;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->y = arg4;

    if (flag != 0) {
        cfg->w = 336;
        cfg->h = 20;
    } else {
        cfg->w = 244;
        cfg->h = 20;
    }

    arg2 = func_801EF9AC(arg1, arg2, 4096, g_menuColor);
    return arg2;
}

/**
 * @brief Renders a message panel for the shop menu.
 * 
 * The message panel displays the message passed by parameter.
 * 
 * @param msg The message string to display.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for display config.
 * @param arg4 Y position for display config.
 * @return The updated value of the second callback parameter.
 * 
 * @note Identical to func_801E95DC, except for the panel width and height.
 */
static void func_801E791C(u8 *msg, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;

    cfg = &g_menuDisplayCfg;

    if (msg != NULL) {
        arg2 = func_801F0FEC(arg1, arg2, arg3 + 12, arg4 + 5, msg, COLOR_WHITE);
    }
    cfg->iconType = ICON_NONE;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->w = 218;
    cfg->y = arg4;
    cfg->h = 21;
    func_801EF9AC(arg1, arg2, 4096, g_menuColor);
}

/**
 * @brief Renders the item shop menu frame.
 *
 * Sets the menu color intensity, then draws the individual UI components: 
 * the title, option, item description, item list, item info, and gil panels,
 * along with the quantity selection dialog. Finally, the display list is
 * finalized for GPU submission.
 * 
 * The X offsets for the item list, item info, and gil panels are dynamically 
 * calculated based on the menu transition progress to apply the slide-in/out 
 * animation.
 *
 * @param s Item shop state.
 * @param arg1 Render context passed to the menu rendering helpers.
 * @param arg2 Current render cursor.
 * @return The updated render cursor after drawing the menu frame.
 */
static s32 func_801E79D4(ShopMenuState *s, s32 arg1, s32 arg2) {
    s32 pkt;
    s32 x;
    s32 y;
    s32 val;

    pkt = getDisplayListHead();

    val = s->menuTransitionProgress;
    val = D_801FA3C8[(val < 0 ? -val : val) / 64];

    func_801F1AFC();
    setMenuColorIntensity(s->menuColorIntensity);

    x = 24;
    y = 29;
    arg2 = func_801E6E0C(arg1, arg2, x, y);

    x = 24;
    y = 8;
    arg2 = func_801E77EC(func_801EFFF0(), arg1, arg2, x, y);

    x = 30;
    y = 51;
    pkt = func_801E6F60(s, arg1, pkt, x, y);

    x = 144;
    y = 80;
    pkt = func_801E7628(s, arg1, pkt, x, y);

    x = (val * 104 / 4096) + 280;
    y = 72;
    pkt = func_801E722C(s, arg1, pkt, x, y);

    x = (val * 360 / 4096) + 24;
    arg2 = func_801E7374(s, arg1, arg2, x, y);

    x = val * 360 / 4096;
    x += 24;
    y = 193;
    arg2 = func_801E7508(s, arg1, arg2, x, y);

    func_801F1B10();
    storeGpuPacket(pkt);
    return arg2;
}

/**
 * @brief Initializes and starts the regular item shop menu.
 *
 * Creates the shop task with its update and draw callbacks, loads the shop
 * catalog, price table, and item metadata, then initializes the temporary
 * inventory view and current gil amount. The current field shop ID is mapped
 * through the shop ID remap table, the shop data and prices are prepared,
 * and the item shop state machine is started.
 * 
 * @param arg0 Unused.
 */
void func_801E7B9C(s32 arg0) {
    ShopMenuState *s;

    s = func_801F179C((s32)func_801E5E90, (s32)func_801E79D4);
    func_801F1D2C(0, "shop.bin", (s32)D_801EA170);
    func_801F1D2C(0, "price.bin", (s32)D_801EA3F0);
    func_801F1D2C(0, "mitem.bin", (s32)D_801EA70C);
    if (s != NULL) {
        s->itemSlots = g_gameState.mainData.itemSlots;
        s->menuTransitionProgress = 4096;
        s->shopkeeperMessage = NULL;
        s->gil = func_801E5D28();
        s->shopId = D_801E9B6C[func_801EFFF0()];
        func_801E6D54(s->shopId);
        func_801E5E90(s);
        if (func_801EFFB8() == 23) {
            func_801F1D84();
        }
    }
}

/**
 * @brief Process shop state and dispatch to appropriate handler.
 *
 * Calls initialization functions, then maps a field shop ID obtained
 * from func_801EFFF0 into a menu shop ID using the shop ID remap table.
 * If the menu shop ID is 21 (junk shop), it dispatches to
 * func_801E9900; otherwise, it dispatches to func_801E7B9C.
 *
 * @param a0 Shop context parameter passed to the handler.
 */
static void func_801E7C8C(s32 a0) {
    func_801F0948(0);
    func_801F7B60();
    if (D_801E9B6C[func_801EFFF0()] == 21) {
        func_801E9900(a0);
    } else {
        func_801E7B9C(a0);
    }
}

/**
 * @brief Returns the name string for a given weapon.
 *
 * Masks out bits 0x40 and 0x80 only while calculating the recipe-table index;
 * the input value itself is not modified. It then resolves the recipe's
 * nameId into the weapon name table.
 *
 * @param weaponId Weapon ID.
 * @return Pointer to the weapon name string.
 */
u8* func_801E7CFC(s32 weaponId) {
    WeaponRecipe *recipePtr;
    u8 *namePtr;

    recipePtr = D_801E9BA0;
    namePtr = D_801E9D2C;

    recipePtr += weaponId & ~0xC0;
    namePtr += recipePtr->nameId;

    return namePtr;
}

/**
 * @brief Expands weapon and character placeholders in a shop message.
 *
 * Copies the null-terminated source string to the destination. The byte
 * sequence 0xA followed by 0x26 is replaced with the selected weapon name,
 * and 0xA followed by 0x27 is replaced with the selected character name.
 * Unknown placeholder codes are removed. The destination is always
 * null-terminated.
 *
 * @param src Source message containing optional placeholders.
 * @param dst Destination buffer for the expanded message.
 */
void func_801E7D30(u8 *src, u8 *dst) {
    u8 buffer[127];
    u8* bufferPtr;
    s32 srcVal;

    while (1) {
        srcVal = *src;
        src++;

        if (srcVal == 0) {
            break;
        }

        bufferPtr = buffer;

        if (srcVal != 0xA) {
            *dst = srcVal;
            dst++;
        } else {
            srcVal = *src;
            src++;
            *buffer = 0;

            switch (srcVal) {
            case 0x26:
                copyString(bufferPtr, getLevelCurveData(D_801EB2E4));
                break;
            case 0x27:
                copyString(bufferPtr, getCharName(D_801EB2E8));
                break;
            }

            while (*bufferPtr != 0) {
                *dst = *bufferPtr;
                dst++;
                bufferPtr++;
            }
        }
    }

    *dst = 0;
}

/**
 * @brief Compute a weapon upgrade price.
 *
 * Looks up the base price at the weapon recipe table, then returns
 * this value multiplied by 10.
 *
 * @param weaponId Weapon ID.
 * @return Computed price value.
 */
u32 func_801E7E1C(s32 weaponId) {
    WeaponRecipe* recipePtr;
    recipePtr = D_801E9BA0;
    recipePtr += weaponId;
    return  recipePtr->basePrice * 10;
}

/**
 * @brief Test if a weapon is set in the unlocked weapons bitmask.
 *
 * @param weaponId Weapon ID.
 * @return 1 if bit is set, 0 otherwise.
 */
static s32 func_801E7E4C(s32 weaponId) {
    s32 mask = 1 << weaponId;
    s32 val = g_gameState.mainData.party.unlockedWeapons & mask;
    return val != 0;
}

/**
 * @brief Check whether the player has enough gil for a weapon.
 *
 * Compares the available gil with the weapon recipe price returned by
 * func_801E7E1C.
 *
 * @param weaponId Weapon ID.
 * @param gil Player's available gil.
 * @return 1 if gil is at least the weapon price, otherwise 0.
 */
s32 func_801E7E68(s32 weaponId, u32 gil) {
    return gil >= func_801E7E1C(weaponId);
}

/**
 * @brief Unlock a weapon if its price and ingredients are available.
 *
 * Checks that the player has enough gil and owns the required quantities for
 * each ingredient in the weapon recipe. If all requirements are met, sets
 * the corresponding bit in the unlocked-weapons mask. This function does not
 * deduct gil or ingredients.
 *
 * @param weaponId Weapon recipe index and unlocked-weapons bit index.
 * @param gil Player's available gil.
 * @return 1 if the weapon was unlocked, otherwise 0.
 */
static s32 func_801E7E98(s32 weaponId, u32 gil) {
    WeaponRecipe *recipePtr;
    u8 *ptr;
    s32 i;

    if (func_801E7E68(weaponId, gil) == 0) {
        return 0;
    }

    recipePtr = D_801E9BA0;
    recipePtr += weaponId;

    ptr = (u8 *)recipePtr->ingredients;

    for (i = 0; i < WEAPON_RECIPE_INGREDIENT_COUNT; i++) {
        s32 itemId;
        s32 quantity;

        itemId = *ptr++;
        quantity = *ptr++;

        if (itemId != 0 && quantity > D_801EB088[itemId]) {
            return 0;
        }
    }

    g_gameState.mainData.party.unlockedWeapons |= 1 << weaponId;
    return 1;
}

/**
 * @brief Build the list of weapons available for a character.
 *
 * Initializes the eight-entry weapon list with 0xFF, then scans the weapon
 * table for entries belonging to characterId. Matching weapons are stored in
 * D_801EB150 with bit 6 indicating an already unlocked weapon and bit 7
 * indicating that its price and ingredients can be satisfied with gil.
 *
 * @param characterId Character index, restricted to the six playable bits.
 * @param gil Player's available gil.
 * @return Number of matching weapons written to D_801EB150.
 */
s32 func_801E7F4C(s32 characterId, u32 gil) {
    s32 count;
    s32 i;
    u8* weaponIdPtr1;
    u8* weaponIdPtr2;
    WeaponInfo *weapons;
    u8 value;

    count = 0;
    value = 0xFF;
    i = 7;

    weaponIdPtr1 = D_801EB150;
    weaponIdPtr1 += 7;

    for (; i >= 0; i--) {
        *weaponIdPtr1 = value;
        weaponIdPtr1--;
    }

    weaponIdPtr2 = D_801EB150;
    weapons = D_8007C3B8;

    if ((0x3F >> characterId) & 1) {
        for (i = 0; i < WEAPON_INFO_COUNT; i++) {
            if (characterId == weapons[i].characterId) {
                s32 mask;
                mask = (func_801E7E4C(i) != 0) << 6;
                if (func_801E7E98(i, gil) != 0) {
                    mask |= 0x80;
                }
                if (mask != 0) {
                    *weaponIdPtr2 = i | mask;
                    weaponIdPtr2++;
                    count++;
                }
            }
        }
    }

    return count;
}

/**
 * @brief Build a character mask for available weapon upgrades.
 *
 * Returns the character bits for characters in the current party that have
 * at least one weapon already unlocked or whose weapon requirements can be
 * met with gil and inventory items. If the weapon system is disabled, the
 * function returns zero. Calling func_801E7E98 may also update the unlocked
 * weapons mask when a weapon's requirements are satisfied.
 *
 * @param gil Player's available gil.
 * @return Bitmask of character IDs with an unlocked or available weapon.
 */
s32 func_801E8058(u32 gil) {
    WeaponInfo *weapons;
    s32 availableChars;
    s32 charBit;
    s32 ret;
    s32 i;

    if (g_gameState.mainData.partyLockFlag & 1) {
        return 0;
    }

    weapons = D_8007C3B8;
    availableChars = func_80036EC0() & 0x3F;
    ret = 0;

    for (i = 0; i < WEAPON_INFO_COUNT; i++) {
        charBit = 1 << weapons[i].characterId;
        if (availableChars & charBit) {
            s32 val;
            val = func_801E7E4C(i);
            if (val) {
                ret |= charBit;
                continue;
            } else {
                val = func_801E7E98(i, gil);
                if (val) {
                    ret |= charBit;
                }
            }
        }
    }

    return ret;
}

/**
 * @brief Render shop item entry at computed Y position with width 36.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 80 for Y position)
 */
void func_801E8134(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 36, a1 * 13 + 80);
}

/**
 * @brief Render shop item quantity at computed Y position with width 169.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 79 for Y position)
 */
void func_801E816C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 169, a1 * 13 + 79);
}
