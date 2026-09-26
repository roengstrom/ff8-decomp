#include "common.h"
#include "psxsdk/libetc.h"
#include "battle.h"
#include "menumain.h"
#include "menushop2.h"
#include "btl_color.h"
#include "btl_sfx.h"

#define SYMBOL_PERCENT 20 // Passed as argument to func_801F6AFC

typedef struct {
    u8 pad00[0x10];                 /* 0x00 */
    u16 state;                      /**< 0x10: state machine current state (0-16). */
    u8 pad12[0xD];                  /* 0x12 */
    u8 *weaponName;                 /**< 0x20: pointer to the weapon name. */
    u8 pad24[4];                    /* 0x24 */
    u32 gil;                        /**< 0x28: current player's gil. */
    u8 *shopkeeperMessage;          /**< 0x2C: shopkeeper message displayed in the message panel. */
    s16 priceMultiplier;            /**< 0x30: weapon upgrade multiplier (1000 = 100% full price, 750 = 75% discounted price). */
    s16 menuTransitionProgress;     /**< 0x32: transition progress for the ingredients panel (slide-in/out animation). */
    s16 menuColorIntensity;         /**< 0x34: menu color intensity level (fade in/out animation). */
    s16 scrollOffset;               /**< 0x36: scroll offset progress for page transition animation (unused in junk shop menu). */
    u16 availableCharactersMask;    /**< 0x38: bit mask of the available characters. */
    u16 listedCharactersMask;       /**< 0x3A: bit mask of the listed characters in the junk shop. */
    u8 pad3C[2];                    /* 0x3C */
    u8 characterCount;              /**< 0x3E: character list count. */
    u8 weaponCount;                 /**< 0x3F: weapon list count. */
    s8 selCharacterIndex;           /**< 0x40: selected index of the character list. */
    s8 selWeaponIndex;              /**< 0x41: selected index of the weapon list. */
    s8 cursorPosition;              /**< 0x42: cursor position. */
    s8 menuPhase;                   /**< 0x43: junk shop menu phase: 0 = character select, 1 = weapon select, 2 = disabled. */
    s8 equippedWeapons[8];          /**< 0x44: equipped weapons of the listed characters. */
    s16 unk4C;                      /* 0x4C */ 
} JunkShopMenuState;

extern u8 D_801EB160[256]; /**< Temporary buffer for the weapon remodel confirmation message. */
extern s32 D_801EB260[30]; /**< Strength per weapon id. */

static void func_801E81A4(JunkShopMenuState*);
static s32 func_801E8978(s32, s32, s32, s32, s32, s32);
static s32 func_801E8AB0(s32, s32, s32, s32, s32);
static s32 func_801E8B60(JunkShopMenuState*, s32, s32, s32, s32);
static s32 func_801E8BD8(JunkShopMenuState*, s32, s32, s32, s32);
static s32 func_801E8D84(JunkShopMenuState*, s32, s32, s32, s32);
static s32 func_801E8FF8(s32, s32);
static void func_801E9020(s32);
static void func_801E90BC(void);
static s32 func_801E90F8(JunkShopMenuState*, s32, s32, s32, s32);
static s32 func_801E9554(JunkShopMenuState*, s32, s32, s32, s32);
static s32 func_801E95DC(u8*, s32, s32, s32, s32);
static s32 func_801E9684(JunkShopMenuState*, s32, s32);

/**
 * @brief Updates the junk shop menu state machine.
 *
 * Handles menu transitions, character and weapon selection, weapon
 * remodel confirmation, message notifications, and the menu fade
 * in/out.
 *
 * @param s Pointer to the junk shop menu state.
 */
static void func_801E81A4(JunkShopMenuState *s) {
    u16 btnFlags;
    u32 cfgFlags;
    u16 *statePtr;

    btnFlags = g_menuDisplayCfg.inputNew;
    cfgFlags = g_menuDisplayCfg.inputRepeat;
    statePtr = &s->state;

    switch (*statePtr) {
    case 0:
        s->weaponName = NULL;
        s->menuColorIntensity = 0;
        *statePtr = 1;
        /* fallthrough */

    case 1:
        s->menuColorIntensity += 256;
        if (s->menuColorIntensity >= 4096) {
            s->menuColorIntensity = 4096;
            *statePtr = 2;
        }
        if (s->availableCharactersMask != 0) {
            func_801E8134(1, s->selCharacterIndex);
        }
        break;

    case 2:
        if (s->availableCharactersMask == 0) {
            *statePtr = 12;
            break;
        }
        s->shopkeeperMessage = func_801F6AA4(STRING_JUNK_SHOP_WELCOME_WHO_NEEDS_TO_REMODEL);
        *statePtr = 3;
        /* fallthrough */

    case 3:
        if (btnFlags & PADLdown) {
            sendSpuCommand(1);
            s->selCharacterIndex++;
            if (s->selCharacterIndex >= s->characterCount) {
                s->selCharacterIndex = 0;
            }
        }
        if (btnFlags & PADLup) {
            sendSpuCommand(1);
            s->selCharacterIndex--;
            if (s->selCharacterIndex << 0x18 < 0) {
                s->selCharacterIndex = s->characterCount - 1;
            }
        }
        s->weaponCount = func_801E7F4C(findNthSetBit(s->availableCharactersMask, s->selCharacterIndex), s->gil);
        func_801E8134(1, s->selCharacterIndex);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 15;
        }
        if (cfgFlags & PADRdown) {
            s32 charId;
            charId = findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);
            if ((g_gameState.mainData.partyLockFlag & 1) || !((s->listedCharactersMask >> charId) & 1)) {
                sendSpuCommand(5);
                if (!((0x3F >> charId) & 1)) {
                    u8 *msg;
                    msg = func_801F6AA4(STRING_JUNK_SHOP_CANT_REMODEL_YOUR_WEAPON);
                    initSfxPlayback(0, msg);
                    func_801F23D0(0, 0x68, msg);
                    setSfxPitch(0, 0);
                    startSfxNormal(0);
                    s->unk4C = 600;
                    *statePtr = 4;
                }
            } else {
                sendSpuCommand(2);
                *statePtr = 5;
            }
        }
        break;

    case 4:
        s->unk4C--;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if (s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 3;
        }
        break;

    case 5:
        s->menuPhase = 1;
        s->selWeaponIndex = 0;
        s->menuTransitionProgress = 4096;
        s->shopkeeperMessage = func_801F6AA4(STRING_JUNK_SHOP_REMODEL_TO_WHICH_WEAPON);
        *statePtr = 6;
        /* fallthrough */

    case 6:
        s->menuTransitionProgress -= 256;
        if ((s->menuTransitionProgress * 4096) <= 0) {
            s->menuTransitionProgress = 0;
            *statePtr = 7;
        }
        break;

    case 7:
        func_801E816C(1, s->selWeaponIndex);
        s->selWeaponIndex = func_801F6768(btnFlags, s->weaponCount, s->selWeaponIndex);
        s->weaponName = func_801E7CFC((s32)D_801EB150[s->selWeaponIndex]);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->menuPhase = 0;
            *statePtr = 8;
        }
        if (cfgFlags & PADRdown) {
            u8 selectedWeaponId;
            selectedWeaponId = D_801EB150[s->selWeaponIndex];
            if (selectedWeaponId & 0x80) {
                s32 equippedWeaponId;
                equippedWeaponId = s->equippedWeapons[s->selCharacterIndex];
                
                if ((selectedWeaponId & 0x3F) == equippedWeaponId) {
                    u8 *msg;
                    sendSpuCommand(5);
                    msg = func_801F6AA4(STRING_JUNK_SHOP_YOU_HAVE_IT_ALREADY);
                    initSfxPlayback(0, msg);
                    func_801F23D0(0, 0x68, msg);
                    setSfxPitch(0, 0);
                    startSfxNormal(0);
                    s->unk4C = 600;
                    *statePtr = 9;
                } else {
                    sendSpuCommand(2);
                    *statePtr = 10;
                }
            } else {
                u8 *msg;
                sendSpuCommand(5);
                if (func_801E7E68(selectedWeaponId & 0x3F, s->gil) != 0) {
                    msg = func_801F6AA4(STRING_JUNK_SHOP_NOT_ENOUGH_ITEMS);
                } else {
                    msg = func_801F6AA4(STRING_JUNK_SHOP_NOT_ENOUGH_MONEY);
                }
                initSfxPlayback(0, msg);
                func_801F23D0(0, 0x68, msg);
                setSfxPitch(0, 0);
                startSfxNormal(0);
                s->unk4C = 600;
                *statePtr = 9;
            }
        }
        break;

    case 9:
        s->unk4C--;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if (s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 7;
        }
        break;

    case 8:
        s->weaponName = NULL;
        s->menuTransitionProgress += 256;
        if (s->menuTransitionProgress >= 4096) {
            s->menuTransitionProgress = 4096;
            s->shopkeeperMessage = func_801F6AA4(STRING_JUNK_SHOP_WELCOME_WHO_NEEDS_TO_REMODEL);
            *statePtr = 3;
        }
        break;

    case 10: {
        s32 charId;
        s->cursorPosition = 1;
        charId = findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);
        D_801EB2E4 = D_801EB150[s->selWeaponIndex] & 0x3F;
        D_801EB2E8 = charId;
        func_801E7D30(func_801F6AA4(STRING_JUNK_SHOP_REMODEL_WEAPON_TO), D_801EB160);
        func_801F728C(D_801EB160, 75);
        *statePtr = 11;
        /* fallthrough */
    }

    case 11:
        s->cursorPosition = func_801F6768(btnFlags, 2, s->cursorPosition);
        func_801F6F88(s->cursorPosition);
        
        if (cfgFlags & PADRup) {
            s->cursorPosition = -1;
            sendSpuCommand(3);
            *statePtr = 7;
        }
        
        if (cfgFlags & PADRdown) {
            if (s->cursorPosition == 0) {
                s32 weaponId;
                weaponId = D_801EB150[s->selWeaponIndex];
                if (weaponId & 0x80) {
                    WeaponRecipe *recipePtr;
                    u8 *ingredients;
                    s32 charId;
                    s32 i;

                    playSoundEffect(25);
                    weaponId &= 0x3F;
                    s->gil -= (func_801E7E1C(weaponId) * s->priceMultiplier) / 1000;
                    charId = findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);
                    
                    recipePtr = D_801E9BA0;
                    recipePtr += weaponId;
                    
                    ingredients = recipePtr->ingredients;

                    g_gameState.chars[charId].weaponId = weaponId;
                    s->equippedWeapons[s->selCharacterIndex] = weaponId;

                    for (i = 0; i < WEAPON_RECIPE_INGREDIENT_COUNT; i++) {
                        s32 itemId;
                        s32 quantity;
                        
                        itemId = *ingredients++;
                        quantity = *ingredients++;
                        
                        if (itemId != 0) {
                            D_801EB088[itemId] -= quantity;
                        }
                    }
                    func_801E7F4C(charId, s->gil);
                } else {
                    sendSpuCommand(5);
                }
            } else {
                sendSpuCommand(2);
            }

            s->cursorPosition = -1;
            *statePtr = 7;
        }
                
        break;

    case 12:
        sendSpuCommand(5);
        s->menuPhase = 2;
        s->menuTransitionProgress = 4096;
        *statePtr = 13;
        break;

    case 13:
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            *statePtr = 14;
        }
        break;

    case 14:
        s->menuTransitionProgress -= 256;
        if ((s->menuTransitionProgress * 4096) <= 0) {
            s->menuTransitionProgress = 0;
            *statePtr = 15;
        }
        break;

    case 15:
        s->shopkeeperMessage = func_801F6AA4(STRING_JUNK_SHOP_COME_BACK_SOON);
        *statePtr = 16;
        /* fallthrough */

    case 16:
        s->menuColorIntensity -= 256;
        if ((s->menuColorIntensity * 4096) <= 0) {
            s->menuColorIntensity = 0;
            func_801E5C08(s->gil);
            func_801F7B60();
            func_801F18FC(s);
            func_801F0BB0();
        }
        if (s->availableCharactersMask != 0) {
            func_801E8134(1, s->selCharacterIndex);
        }
        break;
    }

    func_801F0948(s->menuColorIntensity);
}

/**
 * @brief Renders the character list for the junk shop menu.
 *
 * Iterates over the eight character slots selected by the available
 * characters mask and draws each character name. Characters whose
 * bit is also set in the listed characters mask are drawn in white;
 * available but unlisted characters are drawn in gray.
 *
 * @param arg0 First callback parameter.
 * @param arg1 Second callback parameter.
 * @param arg2 X position of the panel.
 * @param arg3 Y position of the panel.
 * @param availableCharacters Bit mask of the available characters.
 * @param listedCharacters Bit mask of the listed characters.
 * @return The updated second callback parameter.
 */
static s32 func_801E8978(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 availableCharacters, s32 listedCharacters) {
    MenuDisplayConfig *cfg;
    s32 x;
    s32 y;
    s32 i;
    s32 mask;
    s32 color;
    s32 charName;

    cfg = &g_menuDisplayCfg;
    x = arg2 + 12;
    y = arg3 + 9;

    for (i = 0; i < 8; i++) {
        mask = 1 << i;
        if (availableCharacters & mask) {
            color = COLOR_GRAY;
            if (listedCharacters & mask) {
                color = COLOR_WHITE;
            }
            charName = getCharName(g_gameState.chars[i].characterId);
            arg1 = func_801F0FEC(arg0, arg1, x, y, charName, color);
            y += 13;
        }
    }

    cfg->iconType = ICON_NAME;
    cfg->iconSubType = 0;
    cfg->x = arg2;
    cfg->y = arg3;
    cfg->w = 136;
    cfg->h = 105;

    arg1 = func_801EF9AC(arg0, arg1, 4096, g_menuColor);
    return arg1;
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
 * @param arg3 Unused.
 * @param arg4 X position for the display configuration.
 * @return The updated value of the second callback parameter.
 * 
 * @note Identical to func_801E6EB0, except for the Y offset.
 */
static s32 func_801E8AB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    u8 buffer[128];
    s32 msg;
    s32 x;
    s32 y;
    s32 xOffset;

    msg = ((s32 *)(g_menuDisplayCfg.dataPtr))[arg2];
    if (msg != 0) {
        xOffset = arg4 + 10;
        x = g_menuDisplayCfg.x + xOffset;
        y = g_menuDisplayCfg.y + 4;
        decodeMessage(msg, buffer, -1);
        arg1 = func_801F0FEC(arg0, arg1, x, y, buffer, COLOR_WHITE);
    }
    return arg1;
}

/**
 * @brief Renders the weapon name panel for the junk shop menu.
 *
 * The weapon name panel renders the name of currently selected weapon.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 Y position for the display configuration.
 * @param a4 X position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E8B60(JunkShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 a4) {
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
    g_menuDisplayCfg.dataPtr = (s32)&s->weaponName;
    return func_801EFBB4(a1, a2, (s32)&func_801E8AB0);
}

/**
 * @brief Renders the weapon list panel for the selected character in the junk shop menu.
 *
 * If the weapon upgrade is enabled, iterates over the weapon ID's
 * and skips entries marked with 0xFF. For each valid entry, draws
 * the weapon name and its remodel price. Bit 0x40 selects the alternate
 * availability color, while bit 0x80 selects white.
 *
 * @param s Pointer to the junk shop state.
 * @param arg1 Render context passed to the text and panel helpers.
 * @param arg2 Current render cursor / display-list position.
 * @param arg3 X position of the panel.
 * @param arg4 Y position of the panel.
 * @return The updated render cursor after drawing the list and panel.
 */
static s32 func_801E8BD8(JunkShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    s32 x;
    s32 y;
    s32 i;
    u8 *weaponIdPtr;
    s32 weaponId;
    s32 mask;
    s32 color;
    s32 price;

    cfg = &g_menuDisplayCfg;

    if (!(g_gameState.mainData.partyLockFlag & 1)) {
        y = arg4 + 8;
        weaponIdPtr = D_801EB150;

        for (i = 0; i < 8; i++) {
            weaponId = *weaponIdPtr;
            weaponIdPtr++;

            if (weaponId != 0xFF) {
                x = arg3 + 9;
                mask = weaponId & 0x40;
                color = mask != 0;
                if (weaponId & 0x80) {
                    color = COLOR_WHITE;
                }
                arg2 = func_801F0FEC(arg1, arg2, x, y, getLevelCurveData(weaponId & 0x3F), color);
                x = arg3 + 189;
                price = func_801E7E1C(weaponId & 0x3F) * s->priceMultiplier / 1000;
                arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), price, color);
                y += 13;
            }
        }
    }

    cfg->iconType = ICON_INFO;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->y = arg4;
    cfg->w = 200;
    cfg->h = 105;

    arg2 = func_801EF9AC(arg1, arg2, 4096, g_menuColor);
    return arg2;
}

/**
 * @brief Render the list of ingredients for the selected weapon.
 *
 * Draws each item name and quantity, followed by the amount
 * currently in the inventory for that item.
 *
 * @param s Pointer to source data structure.
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for the display configuration.
 * @param a4 Y position for the display configuration.
 * @return The updated value of a2 after rendering all items.
 */
static s32 func_801E8D84(JunkShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 a4) {
    u8 numStr[16];
    u8 buffer[32];
    MenuDisplayConfig *cfg;
    s32 i;
    WeaponRecipe *weaponRecipe;
    u8* itemsPtr;
    s32 x;
    s32 y;
    s32 y2;
    s32 itemId;
    s32 quantity;
    u8 *text;

    if (s->menuTransitionProgress == 4096) {
        return a2;
    }

    cfg = &g_menuDisplayCfg;
    weaponRecipe = D_801E9BA0;
    weaponRecipe += D_801EB150[s->selWeaponIndex] & 0x3F;
    itemsPtr = (u8 *)weaponRecipe->ingredients;
    x = a3 + 12;
    y = a4 + 9;

    for (i = 0; i < WEAPON_RECIPE_INGREDIENT_COUNT; i++) {
        itemId = *itemsPtr++;
        quantity = *itemsPtr++;

        if (itemId) {
            text = getStatName(itemId);
            a2 = func_801F0FEC(a1, a2, x, y, text, COLOR_WHITE);

            text = func_801F6AFC(50);

            intToDecStringShort(quantity, numStr, ((u8 *)getMenuString(11))[1]);
            replaceLeadingZeros(numStr, 4, ((u8 *)getMenuString(11))[1], ((u8 *)getMenuString(11))[0]);

            y2 = y + 13;
            y += 23;

            copyString(buffer, numStr + 2);
            btlStrcat2(buffer, text);

            intToDecStringShort(D_801EB088[itemId], numStr, ((u8 *)getMenuString(11))[1]);
            replaceLeadingZeros(numStr, 4, ((u8 *)getMenuString(11))[1], ((u8 *)getMenuString(11))[0]);

            btlStrcat2(buffer, numStr + 2);

            text = func_801F6AFC(51);
            btlStrcat2(buffer, text);
    
            a2 = func_8002C56C(a1, a2, x + 52, y2, buffer, COLOR_WHITE);
        }
    }

    cfg->iconType = ICON_ITEM;
    cfg->iconSubType = 0;
    cfg->x = a3;
    cfg->y = a4;
    cfg->w = 136;
    cfg->h = 105;

    a2 = func_801EF9AC(a1, a2, 4096, g_menuColor);
    return a2;
}

/** @brief Return color code: white (equal), yellow (a0 > a1), red (a0 < a1). */
static s32 func_801E8FF8(s32 a0, s32 a1) {
    s32 color = COLOR_WHITE;
    if (a0 > a1) color = COLOR_YELLOW;
    if (a0 < a1) color = COLOR_RED;
    return color;
}

/**
 * @brief Calculate and cache a weapon's strength value.
 *
 * Temporarily equips the given weapon on its associated character,
 * calculates the character's battle data, stores the resulting
 * strength in D_801EB260, and restores the character's previously
 * equipped weapon.
 *
 * @param weaponId Weapon index used for the strength calculation.
 */
static void func_801E9020(s32 weaponId) {
    BattleCharData charData;
    u8 charId;
    u8 currentWeaponId;

    charId = D_8007C3B8[weaponId].characterId;
    currentWeaponId = g_gameState.chars[charId].weaponId;
    g_gameState.chars[charId].weaponId = weaponId;
    func_801F537C(charId, &charData);
    D_801EB260[weaponId] = charData.stats[0];
    g_gameState.chars[charId].weaponId = currentWeaponId;
}

/**
 * @brief Initialize all 30 weapon strength values.
 *
 * Calls func_801E9020 for indices 0 through 29.
 */
static void func_801E90BC(void) {
    s32 i;
    for (i = 0; i < 30; i++) {
        func_801E9020(i);
    }
}

/**
 * @brief Draws the gil amount and the comparison values between weapons.
 * 
 * The color of the displayed weapon strength and hit values are determined by
 * the result of func_801E8FF8. Aditionally, if the selected weapon's value is
 * higher or lower, an arrow icon is drawn next to the value to indicate the
 * change.
 * 
 * @param s Pointer to source data structure.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E90F8(JunkShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 oldWeaponId;
    s32 newWeaponId;
    s32 oldWeaponStrength;
    s32 newWeaponStrength;
    s32 oldWeaponHit;
    s32 newWeaponHit;
    s32 color;
    s32 x;
    s32 y;
    s32 y2;
    s32 tmp;
    WeaponInfo *weapons;

    color = COLOR_WHITE;

    findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);

    oldWeaponId = s->equippedWeapons[s->selCharacterIndex];

    weapons = D_8007C3B8;
    oldWeaponStrength = D_801EB260[oldWeaponId];
    oldWeaponHit = weapons[oldWeaponId].hit;

    tmp = func_801F6AA4(STRING_MONEY);
    x = arg3 + 249;
    y = arg4 + 7;
    arg2 = func_801F0FEC(arg1, arg2, x, y, tmp, color);

    x = arg3 + 322;
    y = arg4 + 21;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), s->gil, color);

    x = arg3 + 323;
    y = arg4 + 23;
    arg2 = func_8002FF34(arg1, arg2, ICON_GIL, x, y, g_menuColor);

    x = arg3 + 128;
    y = arg4 + 5;
    arg2 = func_800300F8(arg1, arg2, ICON_STR, x, y, g_menuColor, 0x80);

    x = arg3 + 184;
    y = arg4 + 7;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), oldWeaponStrength, color);

    // Dead code added to match with the original game binary.
    while (0);

    func_801F6AA4(STRING_HIT);

    x = arg3 + 128;
    y = arg4 + 19;
    arg2 = func_800300F8(arg1, arg2, ICON_HIT, x, y, g_menuColor, 0x80);

    x = arg3 + 184;
    y = arg4 + 21;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), func_801F7BE4(oldWeaponHit), color);

    // Dead code added to match with the original game binary.
    if (color == COLOR_YELLOW) {
        color++; color--;
    } else if (color == COLOR_RED) {
        color++; color--;
    }

    tmp = func_801F6AFC(SYMBOL_PERCENT);
    y2 = arg4 + 23;
    arg2 = func_8002C56C(arg1, arg2, x, y2, tmp, color);

    tmp = getLevelCurveData(oldWeaponId);
    x = arg3 + 12;
    y = arg4 + 7;
    arg2 = func_801F0FEC(arg1, arg2, x, y, tmp, color);

    if (s->menuPhase & 1) {
        newWeaponId = D_801EB150[s->selWeaponIndex];
        newWeaponId &= 0x3F;

        // Unreachable code retained to match with the original game binary.
        if (newWeaponId == 0xFF) {
            return arg2;
        }

        newWeaponStrength = D_801EB260[newWeaponId];
        newWeaponHit = weapons[newWeaponId].hit;

        color = func_801E8FF8(oldWeaponStrength, newWeaponStrength);
    
        x = arg3 + 232;
        arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), newWeaponStrength, color);
        
        x = arg3 + 195;
        y = arg4 + 9;

        tmp = 0;
        if (color == COLOR_YELLOW) {
            tmp = ICON_ARROW_UP;
        }
        if (color == COLOR_RED) {
            tmp = ICON_ARROW_DOWN;
        }
        if (tmp != 0) {
            arg2 = func_800300F8(arg1, arg2, tmp, x, y, g_menuColor, (color * 64) + 2);
        }

        color = func_801E8FF8(oldWeaponHit, newWeaponHit);
    
        y = arg4 + 21;
        x = arg3 + 232;
        tmp = func_801F7BE4(newWeaponHit);
        arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), tmp, color);

        tmp = func_801F6AFC(SYMBOL_PERCENT);
        arg2 = func_8002C56C(arg1, arg2, x, y2, tmp, color);
        
        x = arg3 + 195;
        y = y2;
        
        tmp = 0;
        if (color == COLOR_YELLOW) {
            tmp = ICON_ARROW_UP;
        }
        if (color == COLOR_RED) {
            tmp = ICON_ARROW_DOWN;
        }
        if (tmp != 0) {
            arg2 = func_800300F8(arg1, arg2, tmp, x, y, g_menuColor, (color * 64) + 2);
        }
    }
    
    return arg2;
}

/**
 * @brief Draws the weapon comparison panel for the junk shop menu.
 *
 * The draw of panel content is delegated to func_801E90F8.
 *
 * @param s Pointer to the shop state.
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for display config.
 * @param a4 Y position for display config.
 * @return The updated value of the second callback parameter.
 */
static s32 func_801E9554(JunkShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 a4) {
    a2 = func_801E90F8(s, a1, a2, a3, a4);
    g_menuDisplayCfg.iconType = ICON_INFO;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 336;
    g_menuDisplayCfg.y = a4;
    g_menuDisplayCfg.h = 38;
    return func_801EF9AC(a1, a2, 4096, g_menuColor);
}

/**
 * @brief Draws the message panel for the junk shop menu.
 * 
 * The message panel displays the messages sent by the shopkeeper to
 * the player depending on the current shop state.
 * 
 * @param msg The message string to display.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for display config.
 * @param arg4 Y position for display config.
 * @return The updated value of the second callback parameter.
 * 
 * @note Identical to func_801E791C, except for the panel width and height.
 */
static s32 func_801E95DC(u8 *msg, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if (msg != NULL) {
        arg2 = func_801F0FEC(arg1, arg2, arg3 + 12, arg4 + 5, msg, COLOR_WHITE);
    }
    g_menuDisplayCfg.iconType = ICON_NONE;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 336;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.h = 20;
    return func_801EF9AC(arg1, arg2, 4096, g_menuColor);
}

/**
 * @brief Renders the junk shop menu frame.
 *
 * Sets the menu color intensity, then draws the title, message and
 * weapon name panels. If the weapon upgrade is disabled, displays a
 * "not enough items" dialog. Otherwise, it draws the cursor,
 * ingredients, character list, weapon list, and weapon comparison
 * panels. The completed display list is finalized and stored for GPU
 * submission.
 *
 * @param s Pointer to the junk shop state.
 * @param arg1 Render context passed to the menu rendering helpers.
 * @param arg2 Current render cursor.
 * @return The updated render cursor after drawing the menu frame.
 */
static s32 func_801E9684(JunkShopMenuState *s, s32 arg1, s32 arg2) {
    s32 fieldShopId;
    s32 pkt;
    s32 x;
    s32 y;

    pkt = getDisplayListHead();
    func_801F1AFC();
    setMenuColorIntensity(s->menuColorIntensity);
    
    if ((s->menuPhase & 2) && (s->menuTransitionProgress != 0) && (s->menuColorIntensity == 4096)) {
        u8 *msg;
        msg = func_801F6AA4(STRING_JUNK_SHOP_NOT_ENOUGH_ITEMS_COME_BACK_LATER);
        arg2 = func_801F4168(arg1, arg2, msg, 192, 82, s->menuTransitionProgress, 0);
    }

    x = 30;
    y = 50;
    arg2 = func_801E8B60(s, arg1, arg2, x, y);

    x = 24;
    y = 8;

    if (func_801EFFB8() == 23) {
        fieldShopId = func_801EFFF0();
    } else {
        fieldShopId = -1;
    }

    arg2 = func_801E77EC(fieldShopId, arg1, arg2, x, y);

    x = 24;
    y = 29;
    arg2 = func_801E95DC(s->shopkeeperMessage, arg1, arg2, x, y);

    if (!(s->menuPhase & 2)) {
        if (s->cursorPosition >= 0) {
            arg2 = func_801F6FE4(arg1, arg2, 1, 4096);
        }

        x = 24;
        y = 71;
        if (s->menuTransitionProgress != 4096) {
            s32 val;
            val = D_801FA3C8[s->menuTransitionProgress / 64] * 150;
            pkt = func_801E8D84(s, arg1, pkt, x - (val / 4096), y);
        }

        x = 24;
        y = 71;
        arg2 = func_801E8978(arg1, arg2, x, y, s->availableCharactersMask, s->listedCharactersMask);
        
        x = 160;
        y = 71;
        arg2 = func_801E8BD8(s, arg1, arg2, x, y);

        x = 24;
        y = 178;
        pkt = func_801E9554(s, arg1, pkt, x, y);
    }

    func_801F1B10();
    storeGpuPacket(pkt);
    return arg2;
}

/**
 * @brief Initializes and starts the junk shop menu.
 *
 * Creates the menu task with the junk shop update and draw callbacks, loads
 * the weapon recipe and message data, computes the gil and character masks,
 * initializes the weapon and menu animation state, caches the equipped
 * weapon ID's, and enters the initial state of the menu state machine.
 *
 * @param arg0 Unused.
 */
void func_801E9900(s32 arg0) {
    BattleCharData charData;
    JunkShopMenuState* s;
    s32 i;
    u8* ptr;

    s = func_801F179C((s32) func_801E81A4, (s32) &func_801E9684);
    func_801F1D2C(0, "mwepon.bin", (s32) D_801E9BA0);
    func_801F1D2C(0, "mwepon.msg", (s32) D_801E9D2C);
    func_801F0948(0);
    func_801E90BC();
    if (s != NULL) {
        s->gil = func_801E5D28();
        s->availableCharactersMask = func_80036EC0();
        s->listedCharactersMask = func_801E8058(s->gil);
        if (func_801EFFB8() == 23) {
            s->availableCharactersMask = s->listedCharactersMask;
        }
        s->characterCount = popcount(s->availableCharactersMask);
        func_801E7F4C(findNthSetBit(s->availableCharactersMask, 0), s->gil);
        s->priceMultiplier = 1000;
        s->menuTransitionProgress = 4096;
        s->cursorPosition = -1;
        if (s->availableCharactersMask != 0) {
            s->menuPhase = 0;
            s->shopkeeperMessage = func_801F6AA4(STRING_JUNK_SHOP_WELCOME_WHO_NEEDS_TO_REMODEL);
        } else {
            s->menuPhase = 2;
            s->shopkeeperMessage = NULL;
        }
        if (func_801F72B4() & 1) {
            s->priceMultiplier = 750;
        }

        for (i = 0, ptr = s->equippedWeapons; i < 8; i++) {
            if ((s->availableCharactersMask >> i) & 1) {
                func_801F537C(i, &charData);
                *ptr = charData.classId;
                ptr++;
            }
        }
        
        func_801E81A4(s);
    }
}
