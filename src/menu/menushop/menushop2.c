#include "common.h"
#include "psxsdk/libetc.h"
#include "battle.h"
#include "menumain.h"
#include "btl_anim.h"
#include "btl_anim_packet.h"
#include "menushop2.h"

#define WHITE  7
#define RED    2
#define YELLOW 3

#define ICON_UP_ARROW   109
#define ICON_DOWN_ARROW 110

typedef struct {
    u8 pad00[0x10];                 /* 0x00 */
    u16 state;                      /**< 0x10: state machine current state (0-16). */
    u8 pad12[0xD];                  /* 0x12 */
    u8 *weaponName;                 /**< 0x20: pointer to the weapon name. */
    u8 pad24[4];                    /* 0x24 */
    u32 gil;                        /**< 0x28: gil */
    u8 *unk2C;                      /**< 0x2C: pointer to a string */
    s16 unk30;                      /* 0x30 */
    s16 unk32;                      /* 0x32 */
    s16 unk34;                      /* 0x34 */
    u16 unk36;                      /* 0x36 */
    u16 availableCharactersMask;    /**< 0x38: bit mask of the available characters. */
    u16 listedCharactersMask;       /**< 0x3A: bit mask of the listed characters in the junk shop. */
    u8 pad3C[2];                    /* 0x3C */
    u8 characterCount;              /**< 0x3E: character list count. */
    u8 weaponCount;                 /**< 0x3F: weapon list count. */
    s8 selCharacterIndex;           /**< 0x40: selected index of the character list. */
    s8 selWeaponIndex;              /**< 0x41: selected index of the weapon list. */
    s8 cursorPosition;              /**< 0x42: cursor position. */
    u8 unk43;                       /* 0x43 */
    s8 equippedWeapons[8];          /**< 0x44: equipped weapons of the listed characters. */
    u16 unk4C;                      /* 0x4C */ 
} JunkShopMenuState;

extern s32 D_801EB160;
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
static s32 func_801E95DC(s32, s32, s32, s32, s32);
static s32 func_801E9684(JunkShopMenuState*, s32, s32);

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
        s->unk34 = 0;
        *statePtr = 1;
        /* fallthrough */
    case 1:
        s->unk34 += 0x100;
        if ((s16) s->unk34 >= 0x1000) {
            s->unk34 = 0x1000U;
            *statePtr = 2;
        }
        if (s->availableCharactersMask != 0) {
            func_801E8134(1, s->selCharacterIndex);
        }
        break;
    case 2:
        if (s->availableCharactersMask == 0) {
            *statePtr = 0xC;
            break;
        }
        s->unk2C = (u8*)func_801F6AA4(0x45);
        *statePtr = 3;
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
            *statePtr = 0xF;
        }
        if (cfgFlags & PADRdown) {
            s32 result;
            result = findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);
            if ((g_gameState.mainData.partyLockFlag & 1) || !(((u16) s->listedCharactersMask >> result) & 1)) {
                sendSpuCommand(5);
                if (!((0x3F >> result) & 1)) {
                    s32 tmp;
                    tmp = func_801F6AA4(0x4E);
                    initSfxPlayback(0, tmp);
                    func_801F23D0(0, 0x68, (void *)tmp);
                    setSfxPitch(0, 0);
                    startSfxNormal(0);
                    s->unk4C = 0x258;
                    *statePtr = 4;
                }
            } else {
                sendSpuCommand(2);
                *statePtr = 5;
            }
        }
        break;
    case 4:
        s->unk4C -= 1;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if ((s16) s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 3;
        }
        break;
    case 5:
        s->unk43 = 1;
        s->selWeaponIndex = 0;
        s->unk32 = 0x1000U;
        s->unk2C = (u8*)func_801F6AA4(0x4B);
        *statePtr = 6;
        /* fallthrough */
    case 6:
        s->unk32 -= 0x100;
        if ((s->unk32 << 0x10) <= 0) {
            s->unk32 = 0U;
            *statePtr = 7;
        }
        break;
    case 7:
        func_801E816C(1, (s8) s->selWeaponIndex);
        s->selWeaponIndex = func_801F6768(btnFlags, s->weaponCount, s->selWeaponIndex);
        s->weaponName = func_801E7CFC((s32)D_801EB150[s->selWeaponIndex]);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->unk43 = 0;
            *statePtr = 8;
        }
        if (cfgFlags & PADRdown) {
            u8 val1;
            val1 = D_801EB150[(s8)s->selWeaponIndex];
            if (val1 & 0x80) {
                s32 val2;
                val2 = s->equippedWeapons[s->selCharacterIndex];
                
                if ((val1 & 0x3F) == val2) {
                    s32 tmp;
                    sendSpuCommand(5);
                    tmp = func_801F6AA4(0x4C);
                    initSfxPlayback(0, tmp);
                    func_801F23D0(0, 0x68, (u8 *)tmp);
                    setSfxPitch(0, 0);
                    startSfxNormal(0);
                    s->unk4C = 0x258;
                    *statePtr = 9;
                } else {
                    sendSpuCommand(2);
                    *statePtr = 0xA;
                }
            } else {
                s32 tmp;
                sendSpuCommand(5);
                if (func_801E7E68(val1 & 0x3F, s->gil) != 0) {
                    tmp = func_801F6AA4(0x3E);
                } else {
                    tmp = func_801F6AA4(0x3F);
                }
                initSfxPlayback(0, tmp);
                func_801F23D0(0, 0x68, (u8 *)tmp);
                setSfxPitch(0, 0);
                startSfxNormal(0);
                s->unk4C = 0x258;
                *statePtr = 9;
            }
        }
        break;
    case 9:
        s->unk4C -= 1;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if ((s16) s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 7;
        }
        break;
    case 8:
        s->weaponName = NULL;
        s->unk32 += 0x100;
        if ((s16) s->unk32 >= 0x1000) {
            s32 k = 0x45;
            s->unk32 = 0x1000;
            s->unk2C = (u8*)func_801F6AA4(k);
            *statePtr = 3;
        }
        break;
    case 10: {
        s32 ret;
        s32 *ptrE4;
        s32 *ptrE8;
        s->cursorPosition = 1;
        ptrE4 = &D_801EB2E4;
        ptrE8 = &D_801EB2E8;
        ret = findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);
        *ptrE4 = D_801EB150[(s8)s->selWeaponIndex] & 0x3F;
        *ptrE8 = ret;
        func_801E7D30((u8 *)func_801F6AA4(0x3B), (u8 *)&D_801EB160);
        func_801F728C((s32) &D_801EB160, 0x4B);
        *statePtr = 0xB;
        /* fallthrough */
    }
    case 11:
        s->cursorPosition = func_801F6768(btnFlags, 2, (s8) s->cursorPosition);
        func_801F6F88((s32)s->cursorPosition);
        
        if (cfgFlags & PADRup) {
            s->cursorPosition = -1;
            sendSpuCommand(3);
            *statePtr = 7;
        }
        
        if (cfgFlags & PADRdown) {
            if (s->cursorPosition == 0) {
                u8 val1;
                val1 = D_801EB150[(s8)s->selWeaponIndex];
                if (val1 & 0x80) {
                    WeaponRecipe *basePtr;
                    u8 *ptr;
                    s32 val2;
                    s32 charIdx;
                    s32 i;

                    playSoundEffect(0x19);
                    val2 = val1 & 0x3F;
                    s->gil -= ((u32)func_801E7E1C(val2) * s->unk30) / 1000;
                    charIdx = findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);
                    
                    basePtr = D_801E9BA0;
                    basePtr += val2;
                    
                    ptr = basePtr->ingredients;

                    g_gameState.chars[charIdx].weaponId = val2;
                    s->equippedWeapons[s->selCharacterIndex] = val2;

                    for (i = 0; i < 4; i++) {
                        s32 itemId;
                        s32 quantity;
                        
                        itemId = *ptr++;
                        quantity = *ptr++;
                        
                        if (itemId != 0) {
                            D_801EB088[itemId] -= quantity;
                        }
                    }
                    func_801E7F4C(charIdx, s->gil);
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
        s->unk43 = 2;
        s->unk32 = 0x1000U;
        *statePtr = 0xD;
        break;
    case 13:
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            *statePtr = 0xE;
        }
        break;
    case 14:
        s->unk32 -= 0x100;
        if ((s->unk32 << 0x10) <= 0) {
            s->unk32 = 0U;
            *statePtr = 0xF;
        }
        break;
    case 15:
        s->unk2C = (u8 *)func_801F6AA4(0x4D);
        *statePtr = 0x10;
        /* fallthrough */
    case 16:
        s->unk34 -= 0x100;
        if ((s->unk34 << 0x10) <= 0) {
            s->unk34 = 0U;
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

    func_801F0948(s->unk34);
}

static s32 func_801E8978(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    MenuDisplayConfig *cfg;
    s32 x;
    s32 y;
    s32 i;
    s32 mask;
    s32 color;
    s32 charName;

    cfg = &g_menuDisplayCfg;
    x = arg2 + 0xC;
    y = arg3 + 9;

    for (i = 0; i < 8; i++) {
        mask = 1 << i;
        if (arg4 & mask) {
            color = 1;
            if (arg5 & mask) {
                color = 7;
            }
            charName = getCharName(g_gameState.chars[i].characterId);
            arg1 = func_801F0FEC(arg0, arg1, x, y, charName, color);
            y += 0xD;
        }
    }

    cfg->iconType = 0x49;
    cfg->iconSubType = 0;
    cfg->x = arg2;
    cfg->y = arg3;
    cfg->w = 0x88;
    cfg->h = 0x69;

    arg1 = func_801EF9AC(arg0, arg1, 0x1000, g_menuColor);
    return arg1;
}

static s32 func_801E8AB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    u8 buffer[128];
    s32 dataVal;
    s32 x;
    s32 y;
    s32 tmp;

    cfg = &g_menuDisplayCfg;
    dataVal = ((s32*)cfg->dataPtr)[arg2];
    
    if (dataVal != 0) {
        tmp = arg4 + 0xA;
        x = cfg->x + tmp;
        y = cfg->y + 4;
        decodeMessage(dataVal, buffer, -1);
        arg1 = func_801F0FEC(arg0, arg1, x, y, (s32)buffer, 7);
    }

    return arg1;
}

/**
 * @brief Configure display parameters and invoke callback for shop sell rendering.
 *
 * Sets up the g_menuDisplayCfg display configuration structure with the given
 * position and size values, stores the pointer at a0+0x20 as the data source,
 * reads a halfword at a0+0x36 as the display ID, then calls func_801EFBB4
 * with func_801E8AB0 as the render callback.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter (passed as a0 to func_801EFBB4).
 * @param a2 Second callback parameter (passed as a1 to func_801EFBB4).
 * @param a3 Y position for the display configuration.
 * @param a4 X position for the display configuration.
 */
static s32 func_801E8B60(JunkShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 a4) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x14;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = a4;
    g_menuDisplayCfg.scrollOffset = s->unk36;
    g_menuDisplayCfg.dataPtr = (s32)&s->weaponName;
    {
        return func_801EFBB4(a1, a2, (s32)&func_801E8AB0);
    }
}

static s32 func_801E8BD8(JunkShopMenuState* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    s32 x;
    s32 y;
    s32 i;
    u8 *ptr;
    s32 val;
    s32 mask;
    s32 color;
    s32 result;

    cfg = &g_menuDisplayCfg;

    if (!(g_gameState.mainData.partyLockFlag & 1)) {
        y = arg4 + 8;
        ptr = D_801EB150;

        for (i = 0; i < 8; i++) {
            val = *ptr;
            ptr++;

            if (val != 0xff) {
                x = arg3 + 9;
                mask = val & 0x40;
                color = mask != 0;
                if (val & 0x80) {
                    color = 7;
                }
                arg2 = func_801F0FEC(arg1, arg2, x, y, getLevelCurveData(val & 0x3F), color);
                x = arg3 + 0xBD;
                result = ((u32)func_801E7E1C(val & 0x3F) * arg0->unk30) / 1000;
                arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), result, color);
                y += 0xD;
            }
        }
    }

    cfg->iconType = 0x57;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->y = arg4;
    cfg->w = 0xC8;
    cfg->h = 0x69;

    arg2 = func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
    return arg2;
}

/**
 * @brief Render the list of required materials for the selected weapon.
 *
 * Draws each item name and the required quantity, followed by the amount
 * currently in the inventory for that item.
 *
 * @param s Pointer to source data structure.
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for the display configuration.
 * @param a4 Y position for the display configuration.
 * @return The updated value of a1 after rendering all items.
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

    if (s->unk32 == 0x1000) {
        return a2;
    }

    cfg = &g_menuDisplayCfg;
    weaponRecipe = D_801E9BA0;
    weaponRecipe += D_801EB150[s->selWeaponIndex] & 0x3F;
    itemsPtr = (u8 *)weaponRecipe->ingredients;
    x = a3 + 12;
    y = a4 + 9;

    for (i = 0; i < 4; i++) {
        itemId = *itemsPtr++;
        quantity = *itemsPtr++;

        if (itemId) {
            text = getStatName(itemId);
            a2 = func_801F0FEC(a1, a2, x, y, text, 7);

            text = func_801F6AFC(0x32);

            intToDecStringShort(quantity, numStr, ((u8 *)getMenuString(0xB))[1]);
            replaceLeadingZeros(numStr, 4, ((u8 *)getMenuString(0xB))[1], ((u8 *)getMenuString(0xB))[0]);

            y2 = y + 13;
            y += 23;

            copyString(buffer, numStr + 2);
            btlStrcat2(buffer, text);

            intToDecStringShort(D_801EB088[itemId], numStr, ((u8 *)getMenuString(0xB))[1]);
            replaceLeadingZeros(numStr, 4, ((u8 *)getMenuString(0xB))[1], ((u8 *)getMenuString(0xB))[0]);

            btlStrcat2(buffer, numStr + 2);

            text = func_801F6AFC(0x33);
            btlStrcat2(buffer, text);
    
            a2 = func_8002C56C(a1, a2, x + 52, y2, buffer, 7);
        }
    }

    cfg->iconType = 76;
    cfg->iconSubType = 0;
    cfg->x = a3;
    cfg->y = a4;
    cfg->w = 136;
    cfg->h = 105;

    a2 = func_801EF9AC(a1, a2, 0x1000, g_menuColor);
    return a2;
}

/** @brief Return color code: 7 (equal), 3 (a0 > a1), 2 (a0 < a1). */
static s32 func_801E8FF8(s32 a0, s32 a1) {
    s32 color = 7;
    if (a0 > a1) color = 3;
    if (a0 < a1) color = 2;
    return color;
}

static void func_801E9020(s32 arg0) {
    BattleCharData charData;
    u8 charId;
    u8 weaponId;

    charId = D_8007C3B8[arg0].characterId;
    weaponId = g_gameState.chars[charId].weaponId;
    g_gameState.chars[charId].weaponId = arg0;
    func_801F537C(charId, &charData);
    D_801EB260[arg0] = charData.stats[0];
    g_gameState.chars[charId].weaponId = weaponId;
}

/**
 * @brief Initialize all 30 shop item entries.
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
 * @param s Pointer to source data structure.
 * @param arg1 First callback parameter.
 * @param arg2 Second callback parameter.
 * @param arg3 X position for the display configuration.
 * @param arg4 Y position for the display configuration.
 * @return The updated value of arg1 after rendering all items.
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

    color = WHITE;

    findNthSetBit(s->availableCharactersMask, s->selCharacterIndex);

    oldWeaponId = s->equippedWeapons[s->selCharacterIndex];

    weapons = D_8007C3B8;
    oldWeaponStrength = D_801EB260[oldWeaponId];
    oldWeaponHit = weapons[oldWeaponId].hit;

    tmp = func_801F6AA4(0x33);
    x = arg3 + 249;
    y = arg4 + 7;
    arg2 = func_801F0FEC(arg1, arg2, x, y, tmp, color);

    x = arg3 + 322;
    y = arg4 + 21;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), s->gil, color);

    x = arg3 + 323;
    y = arg4 + 23;
    arg2 = func_8002FF34(arg1, arg2, 0xB, x, y, g_menuColor);

    x = arg3 + 128;
    y = arg4 + 5;
    arg2 = func_800300F8(arg1, arg2, 0x131, x, y, g_menuColor, 0x80);

    x = arg3 + 184;
    y = arg4 + 7;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), oldWeaponStrength, color);

    // Dead code added to match with the original game binary.
    while (0);

    func_801F6AA4(0x3A);

    x = arg3 + 128;
    y = arg4 + 19;
    arg2 = func_800300F8(arg1, arg2, 311, x, y, g_menuColor, 0x80);

    x = arg3 + 184;
    y = arg4 + 21;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), func_801F7BE4(oldWeaponHit), color);

    // Dead code added to match with the original game binary.
    if (color == YELLOW) {
        color++; color--;
    } else if (color == RED) {
        color++; color--;
    }

    tmp = func_801F6AFC(0x14);
    y2 = arg4 + 23;
    arg2 = func_8002C56C(arg1, arg2, x, y2, tmp, color);

    tmp = getLevelCurveData(oldWeaponId);
    x = arg3 + 12;
    y = arg4 + 7;
    arg2 = func_801F0FEC(arg1, arg2, x, y, tmp, color);

    if (s->unk43 & 1) {
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
        if (color == YELLOW) {
            tmp = ICON_UP_ARROW;
        }
        if (color == RED) {
            tmp = ICON_DOWN_ARROW;
        }
        if (tmp != 0) {
            arg2 = func_800300F8(arg1, arg2, tmp, x, y, g_menuColor, (color * 64) + 2);
        }

        color = func_801E8FF8(oldWeaponHit, newWeaponHit);
    
        y = arg4 + 21;
        x = arg3 + 232;
        tmp = func_801F7BE4(newWeaponHit);
        arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), tmp, color);

        tmp = func_801F6AFC(0x14);
        arg2 = func_8002C56C(arg1, arg2, x, y2, tmp, color);
        
        x = arg3 + 195;
        y = y2;
        
        tmp = 0;
        if (color == YELLOW) {
            tmp = ICON_UP_ARROW;
        }
        if (color == RED) {
            tmp = ICON_DOWN_ARROW;
        }
        if (tmp != 0) {
            arg2 = func_800300F8(arg1, arg2, tmp, x, y, g_menuColor, (color * 64) + 2);
        }
    }
    
    return arg2;
}

/**
 * @brief Configure shop display and render with g_menuDisplayCfg settings.
 *
 * Calls func_801E90F8 with all parameters, then sets up g_menuDisplayCfg
 * display config (icon 0x57, 0x150 x 0x26, x=a3, y=arg5) and calls
 * func_801EF9AC to render.
 *
 * @param a0 Context pointer for func_801E90F8.
 * @param a1 Render context passed to func_801EF9AC.
 * @param a2 Parameter for func_801E90F8.
 * @param a3 X position for display config.
 * @param arg4 Y position for display config.
 */
static s32 func_801E9554(JunkShopMenuState *a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    s32 result;

    result = func_801E90F8(a0, a1, a2, a3, arg4);
    g_menuDisplayCfg.iconType = 0x57;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.h = 0x26;
    return func_801EF9AC(a1, result, 0x1000, g_menuColor);
}

static s32 func_801E95DC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    if (arg0 != 0) {
        arg2 = func_801F0FEC(arg1, arg2, arg3 + 0xC, arg4 + 5, arg0, 7);
    }
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.y = (s16) arg4;
    g_menuDisplayCfg.h = 0x14;
    return func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

static s32 func_801E9684(JunkShopMenuState *s, s32 arg1, s32 arg2) {
    s32 val1;
    s16 idx;
    s32 pkt;
    s32 param1;
    s32 param2;

    pkt = getDisplayListHead();
    func_801F1AFC();
    setMenuColorIntensity(s->unk34);
    
    if ((s->unk43 & 2) && (s->unk32 != 0) && (s->unk34 == 0x1000)) {
        arg2 = func_801F4168(arg1, arg2, func_801F6AA4(0x3D), 0xC0, 0x52, s->unk32, 0);
    }

    param2 = 0x32;
    arg2 = func_801E8B60(s, arg1, arg2, 0x1E, param2);

    param1 = 0x18;
    param2 = 8;

    if (func_801EFFB8() == 0x17) {
        val1 = func_801EFFF0();
    } else {
        val1 = -1;
    }

    arg2 = func_801E77EC(val1, arg1, arg2, param1, param2);

    param1 = 0x18;
    param2 = 0x1D;
    arg2 = func_801E95DC((s32)s->unk2C, arg1, arg2, param1, param2);

    if (!(s->unk43 & 2)) {
        if (s->cursorPosition >= 0) {
            arg2 = func_801F6FE4(arg1, arg2, 1, 0x1000);
        }

        param1 = 0x18;
        param2 = 0x47;
        idx = s->unk32;
        if (idx != 0x1000) {
            val1 = D_801FA3C8[idx / 64] * 0x96;
            if (val1 < 0) {
                val1 += 0xFFF;
            }
            pkt = func_801E8D84(s, arg1, pkt, param1 - (val1 >> 0xC), param2);
        }

        param1 = 0x18;
        param2 = 0x47;
        arg2 = func_801E8978(arg1, arg2, param1, param2, s->availableCharactersMask, (u16)s->listedCharactersMask);
        
        param1 = 0xA0;
        param2 = 0x47;
        arg2 = func_801E8BD8(s, arg1, arg2, param1, param2);

        param1 = 0x18;
        param2 = 0xB2;
        pkt = func_801E9554(s, arg1, pkt, param1, param2);
    }

    func_801F1B10();
    storeGpuPacket(pkt);
    return arg2;
}

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
        s->listedCharactersMask = func_801E8058((s32) s->gil);
        if (func_801EFFB8() == 0x17) {
            s->availableCharactersMask = s->listedCharactersMask;
        }
        s->characterCount = popcount(s->availableCharactersMask);
        func_801E7F4C(findNthSetBit(s->availableCharactersMask, 0), (s32) s->gil);
        s->unk30 = 0x3E8;
        s->unk32 = 0x1000;
        s->cursorPosition = -1;
        if (s->availableCharactersMask != 0) {
            s->unk43 = 0;
            s->unk2C = (u8 *)func_801F6AA4(0x45);
        } else {
            s->unk43 = 2;
            s->unk2C = NULL;
        }
        if (func_801F72B4() & 1) {
            s->unk30 = 0x2EE;
        }

        for (i = 0, ptr = s->equippedWeapons; i < 8; i++) {
            if (((s32) s->availableCharactersMask >> i) & 1) {
                func_801F537C(i, &charData);
                *ptr = charData.classId;
                ptr++;
            }
        }
        
        func_801E81A4(s);
    }
}
