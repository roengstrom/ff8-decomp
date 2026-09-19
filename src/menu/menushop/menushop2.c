#include "common.h"
#include "menushop2.h"

void func_801E81A4(ShopMenuState *s) {
    u16 btnFlags;
    u32 cfgFlags;
    u16 *statePtr;

    btnFlags = g_menuDisplayCfg.inputNew;
    cfgFlags = g_menuDisplayCfg.inputRepeat;
    statePtr = &s->state;

    switch (*statePtr) {
    case 0:
        s->field_20 = NULL;
        s->unk34 = 0;
        *statePtr = 1;
        /* fallthrough */
    case 1:
        s->unk34 += 0x100;
        if ((s16) s->unk34 >= 0x1000) {
            s->unk34 = 0x1000U;
            *statePtr = 2;
        }
        if (s->unk38 != 0) {
            func_801E8134(1, (s32) (s8) s->unk40);
        }
        break;
    case 2:
        if (s->unk38 == 0) {
            *statePtr = 0xC;
            break;
        }
        s->unk2C = (u8*)func_801F6AA4(0x45);
        *statePtr = 3;
    case 3:
        if (btnFlags & 0x4000) {
            sendSpuCommand(1);
            s->unk40++;
            if ((s8) s->unk40 >= (s32) (u8)s->union3C.unk3C_s8[2]) {
                s->unk40 = 0;
            }
        }
        if (btnFlags & 0x1000) {
            sendSpuCommand(1);
            s->unk40--;
            if ((s8)s->unk40 << 0x18 < 0) {
                s->unk40 = s->union3C.unk3C_s8[2] - 1;
            }
        }
        s->union3C.unk3C_s8[3] = func_801E7F4C(findNthSetBit(s->unk38, (s8) s->unk40), s->gil);
        func_801E8134(1, (s32) (s8) s->unk40);
        if (cfgFlags & 0x10) {
            sendSpuCommand(3);
            *statePtr = 0xF;
        }
        if (cfgFlags & 0x40) {
            s32 result;
            result = findNthSetBit(s->unk38, (s8) s->unk40);
            if ((D_8007809A & 1) || !(((u16) s->unk3A >> result) & 1)) {
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
        if (cfgFlags & 0x50) {
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
        s->unk41 = 0;
        s->union30.unk30_s16[1] = 0x1000U;
        s->unk2C = (u8*)func_801F6AA4(0x4B);
        *statePtr = 6;
        /* fallthrough */
    case 6:
        s->union30.unk30_s16[1] -= 0x100;
        if ((s->union30.unk30_s16[1] << 0x10) <= 0) {
            s->union30.unk30_s16[1] = 0U;
            *statePtr = 7;
        }
        break;
    case 7:
        func_801E816C(1, (s8) s->unk41);
        s->unk41 = func_801F6768(btnFlags, (u8) s->union3C.unk3C_s8[3], (s8) s->unk41);
        s->field_20 = func_801E7CFC((s32)D_801EB150[(s8)s->unk41]);
        if (cfgFlags & 0x10) {
            sendSpuCommand(3);
            s->unk43 = 0;
            *statePtr = 8;
        }
        if (cfgFlags & 0x40) {
            u8 val1;
            val1 = D_801EB150[(s8)s->unk41];
            if (val1 & 0x80) {
                s32 val2;
                val2 = *(s8*)(((s32)s + (s8)s->unk40) + 0x44);
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
        if (cfgFlags & 0x50) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if ((s16) s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 7;
        }
        break;
    case 8:
        s->field_20 = NULL;
        s->union30.unk30_s16[1] += 0x100;
        if ((s16) s->union30.unk30_s16[1] >= 0x1000) {
            s32 k = 0x45;
            s->union30.unk30_s16[1] = 0x1000;
            s->unk2C = (u8*)func_801F6AA4(k);
            *statePtr = 3;
        }
        break;
    case 10: {
        s32 ret;
        s32 *ptrE4;
        s32 *ptrE8;
        s->unk42 = 1;
        ptrE4 = &D_801EB2E4;
        ptrE8 = &D_801EB2E8;
        ret = findNthSetBit(s->unk38, (s8) s->unk40);
        *ptrE4 = D_801EB150[(s8)s->unk41] & 0x3F;
        *ptrE8 = ret;
        func_801E7D30((u8 *)func_801F6AA4(0x3B), (u8 *)&D_801EB160);
        func_801F728C((s32) &D_801EB160, 0x4B);
        *statePtr = 0xB;
        /* fallthrough */
    }
    case 11:
        s->unk42 = func_801F6768(btnFlags, 2, (s8) s->unk42);
        func_801F6F88((s32)s->unk42);
        
        if (cfgFlags & 0x10) {
            s->unk42 = -1;
            sendSpuCommand(3);
            *statePtr = 7;
        }
        
        if (cfgFlags & 0x40) {
            if (s->unk42 == 0) {
                u8 val1;
                val1 = D_801EB150[(s8)s->unk41];
                if (val1 & 0x80) {
                    Struct_func_801E7CFC *basePtr;
                    u8 *ptr;
                    s32 val2;
                    s32 charIdx;
                    s32 i;

                    playSoundEffect(0x19);
                    val2 = val1 & 0x3F;
                    s->gil -= ((u32)func_801E7E1C(val2) * s->union30.unk30_s16[0]) / 1000;
                    charIdx = findNthSetBit(s->unk38, (s8) s->unk40);
                    
                    basePtr = D_801E9BA0;
                    basePtr += val2;
                    
                    ptr = basePtr->unk4;
                    
                    g_gameState.chars[charIdx].weaponId = (u8) val2;
                    *(s8*)(((s32)s + (s8)s->unk40) + 0x44) = (s8) val2;
                    
                    for (i = 0; i < 4; i++) {
                        s32 unk0;
                        s32 unk1;
                        
                        unk0 = *ptr;
                        ptr++;
                        unk1 = *ptr;
                        ptr++;
                        
                        if (unk0 != 0) {
                            D_801EB088[unk0] -= unk1;
                        }
                    }
                    func_801E7F4C(charIdx, s->gil);
                } else {
                    sendSpuCommand(5);
                }
            } else {
                sendSpuCommand(2);
            }

            s->unk42 = -1;
            *statePtr = 7;
        }
                
        break;
    case 12:
        sendSpuCommand(5);
        s->unk43 = 2;
        s->union30.unk30_s16[1] = 0x1000U;
        *statePtr = 0xD;
        break;
    case 13:
        if (cfgFlags & 0x50) {
            func_801F7BEC(cfgFlags);
            *statePtr = 0xE;
        }
        break;
    case 14:
        s->union30.unk30_s16[1] -= 0x100;
        if ((s->union30.unk30_s16[1] << 0x10) <= 0) {
            s->union30.unk30_s16[1] = 0U;
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
        if (s->unk38 != 0) {
            func_801E8134(1, (s32) (s8) s->unk40);
        }
        break;
    }

    func_801F0948((s32) (s16) s->unk34);
}

s32 func_801E8978(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
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

s32 func_801E8AB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
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
s32 func_801E8B60(ShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 a4) {
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
    g_menuDisplayCfg.dataPtr = (s32)&s->field_20;
    {
        return func_801EFBB4(a1, a2, (s32)&func_801E8AB0);
    }
}

s32 func_801E8BD8(ShopMenuState* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
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

    if (!(D_8007809A & 1)) {
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
                result = ((u32)func_801E7E1C(val & 0x3F) * arg0->union30.unk30_s16[0]) / 1000;
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

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop2", func_801E8D84);

/** @brief Return color code: 7 (equal), 3 (a0 > a1), 2 (a0 < a1). */
s32 func_801E8FF8(s32 a0, s32 a1) {
    s32 color = 7;
    if (a0 > a1) color = 3;
    if (a0 < a1) color = 2;
    return color;
}

void func_801E9020(s32 arg0) {
    u8 buf[460];
    u8 charId;
    u8 weaponId;

    charId = D_8007C3B8[arg0].unk4;
    weaponId = g_gameState.chars[charId].weaponId;
    g_gameState.chars[charId].weaponId = arg0;
    func_801F537C(charId, buf);
    D_801EB260[arg0] = buf[443];
    g_gameState.chars[charId].weaponId = weaponId;
}

/**
 * @brief Initialize all 30 shop item entries.
 *
 * Calls func_801E9020 for indices 0 through 29.
 */
void func_801E90BC(void) {
    s32 i;
    for (i = 0; i < 30; i++) {
        func_801E9020(i);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop2", func_801E90F8);

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
s32 func_801E9554(ShopMenuState *a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
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

s32 func_801E95DC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
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

s32 func_801E9684(ShopMenuState *s, s32 arg1, s32 arg2) {
    s32 val1;
    s16 idx;
    s32 pkt;
    s32 param1;
    s32 param2;

    pkt = getDisplayListHead();
    func_801F1AFC();
    setMenuColorIntensity(s->unk34);
    
    if ((s->unk43 & 2) && (s->union30.unk30_s16[1] != 0) && (s->unk34 == 0x1000)) {
        arg2 = func_801F4168(arg1, arg2, func_801F6AA4(0x3D), 0xC0, 0x52, s->union30.unk30_s16[1], 0);
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
        if (s->unk42 >= 0) {
            arg2 = func_801F6FE4(arg1, arg2, 1, 0x1000);
        }

        param1 = 0x18;
        param2 = 0x47;
        idx = s->union30.unk30_s16[1];
        if (idx != 0x1000) {
            val1 = D_801FA3C8[idx / 64] * 0x96;
            if (val1 < 0) {
                val1 += 0xFFF;
            }
            pkt = func_801E8D84(s, arg1, pkt, param1 - (val1 >> 0xC), param2);
        }

        param1 = 0x18;
        param2 = 0x47;
        arg2 = func_801E8978(arg1, arg2, param1, param2, s->unk38, (u16)s->unk3A);
        
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

void func_801E9900(void) {
    u8 buffer[460];
    ShopMenuState* s;
    s32 i;
    u8* ptr;

    s = (ShopMenuState *)func_801F179C((s32) func_801E81A4, (s32) &func_801E9684);
    func_801F1D2C(0, "mwepon.bin", (s32) D_801E9BA0);
    func_801F1D2C(0, "mwepon.msg", (s32) D_801E9D2C);
    func_801F0948(0);
    func_801E90BC();
    if (s != NULL) {
        s->gil = func_801E5D28();
        s->unk38 = func_80036EC0();
        s->unk3A = func_801E8058((s32) s->gil);
        if (func_801EFFB8() == 0x17) {
            s->unk38 = (u16) s->unk3A;
        }
        s->union3C.unk3C_s8[2] = popcount(s->unk38);
        func_801E7F4C(findNthSetBit(s->unk38, 0), (s32) s->gil);
        s->union30.unk30_s16[0] = 0x3E8;
        s->union30.unk30_s16[1] = 0x1000;
        s->unk42 = -1;
        if (s->unk38 != 0) {
            s->unk43 = 0;
            s->unk2C = (u8 *)func_801F6AA4(0x45);
        } else {
            s->unk43 = 2;
            s->unk2C = NULL;
        }
        if (func_801F72B4() & 1) {
            s->union30.unk30_s16[0] = 0x2EE;
        }

        for (i = 0, ptr = &s->unk44; i < 8; i++) {
            if (((s32) s->unk38 >> i) & 1) {
                func_801F537C(i, buffer);
                *ptr = buffer[442];
                ptr++;
            }
        }
        
        func_801E81A4(s);
    }
}
