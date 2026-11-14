#include "ability_list.h"
#include "btl_anim.h"
#include "btl_anim_packet.h"
#include "card.h"
#include "common.h"
#include "game.h"
#include "gamestate.h"
#include "gf.h"
#include "main.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menumain2b.h"
#include "menumain3.h"
#include "numstr.h"
#include "overlay.h"
#include "party_stats.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "render.h"
#include "snd_sfx.h"
#include "ui/font.h"
#include "ui/icon.h"
#include "ui/text.h"

/** @brief The persistent statuses the menu draws as icons: all but KO. */
#define STATUS_ICON_MASK (STATUS_POISON | STATUS_PETRIFY | STATUS_DARKNESS | STATUS_SILENCE | \
    STATUS_BERSERK | STATUS_ZOMBIE)

extern u8 D_800562A4;
extern u8 D_801F7DF4;
extern u8 D_801F7E00;
extern u8 D_801F7E0C;
extern u8 D_801F7F90[];
extern u8 D_801F8BB8[];
extern u8 D_801FAB7C;
extern u8 D_801FABC4[4];
extern u16 D_801FACE2;
extern u16 D_801FACE4;
extern s32 D_801FACE8;

static void func_801F2458(MainMenuCtx *ctx);
static s32 func_801F2FAC(s32, s32);
static void func_801F39D0(s32, s32, s32, s32, s32, s32);
static s32 func_801F3FE8(u8 *, s32, s32, s32);
static s32 func_801F42A4(u8 *, s32, s32, s32);
static s32 func_801F4744(u8 *, s32);
static s32 func_801F4A98(s32, s32, s32);
static void func_801F5490(s32);
static s32 func_801F5B54(s32, s32, s32, s32, s32, u16 *, s32);
static s32 func_801F5C84(s32, s32, s32, s32, s32, u16 *, u32);
static s32 func_801F5D5C(s32, DR_AREA *, s32, s32, s32, u16 *, s32, s32);
static s32 func_801F5E0C(s32, s32, s32, s32, s32, s32, s32);
static s32 func_801F6358(s32, s32, s32, s32, s32);
static s32 func_801F6B54(s32, s32, s32, s32, s32);

static void func_801F2458(MainMenuCtx *s) {
    u16 inputRaw;
    u16 inputRepeat;
    u16 inputNew;
    u16 *statePtr;
    u16 state;

    inputRaw = g_menuDisplayCfg.inputRaw;
    inputRepeat = g_menuDisplayCfg.inputRepeat;
    inputNew = g_menuDisplayCfg.inputNew;
    statePtr = &s->state;
    state = s->state;

restart:
    switch (state) {
    case 0:
        s->field40 = 0;
        s->unk48 = 277;
        s->brightness = 0;
        s->unk42 = 0xFF;
        s->unk47 = (s->field40 * 14) + 12;
        *statePtr = 1;
        break;

    default:
        break;

    case 1:
        s->brightness += 256;
        if (s->brightness >= ONE) {
            s->brightness = ONE;
            *statePtr = 2;
        }
        func_801F1DB0(s->brightness);
        func_801F0954(0, s->brightness, s->unk48, s->unk47);
        break;

    case 2:
        s->unk42 = 0xFF;
        *statePtr = 3;
        func_801F0954(0, s->brightness, s->unk48, s->unk47);
        break;

    case 3: {
        s32 val1 = func_801F6768(inputRepeat, s->field41, s->field40);
        s->field40 = val1;
        D_801FAB31 = val1;
        if ((inputNew & PADLleft) && !(inputRaw & (PADLup | PADLdown))) {
            sendSpuCommand(1);
            *statePtr = 13;
        } else {
            s->unk48 = 277;
            s->unk47 = (s->field40 * 14) + 12;
            s->unk46 = D_801FAB98[s->field40].unk0;
            if (inputRepeat & PADRdown) {
                s32 val2;
                s32 val3;
                s32 val4;
                val2 = D_801FAB98[s->field40].unk1;
                val4 = val2 & 0x80;

                // Dead code added to match with the original game binary.
                val4++; val4--;

                val3 = val2 & 0x1F;
                s->unk42 = val3;
                if (val4) {
                    sendSpuCommand(2);
                    *statePtr = 9;
                } else if (val3 != 0) {
                    sendSpuCommand(2);
                    func_801F0BF8(val3);
                    *statePtr = 4;
                } else {
                    sendSpuCommand(5);
                    func_801F0954(0, s->brightness, s->unk48, s->unk47);
                    break;
                }
            } else {
                if (inputNew & PADRup) {
                    sendSpuCommand(3);
                    *statePtr = 21;
                }
                func_801F0954(0, s->brightness, s->unk48, s->unk47);
                break;
            }
        }
        break;
    }

    case 9:
        func_801F09C4(0, s->brightness, s->unk48, s->unk47);
        *statePtr = 10;
        state = 10;
        goto restart;

    case 10:
        func_801F09C4(0, s->brightness, s->unk48, s->unk47);
        if (s->unk34 == 1) {
            state = 12;
            goto restart;
        }
        if (inputRepeat & PADLup) {
            sendSpuCommand(1);
            s->field43--;
            if (s->field43 < 0) {
                s->field43 = 2;
            }
        } else if (inputRepeat & PADLdown) {
            sendSpuCommand(1);
            s->field43++;
            if (s->field43 >= 3) {
                s->field43 = 0;
            }
        }

    case 11: {
        s32 index;
        s32 val;
        s32 tmp;

        if (inputRepeat & PADRdown) {
            index = s->field43;
            val = s->unk42;

            tmp = index;
            
            // Dead code added to match with the original game binary.
            if (tmp) { tmp++; tmp--; }

            if (index < 3) {
                s->unk22 = s->party[index];
            } else {
                index = tmp;
                s->unk22 = s->party[index];
            }

            if ((val & 0x20 && D_801FABC8[s->unk22].statusFlags & 1) || s->unk22 == 0xFF) {
               *statePtr = 18;
                break;
            }
            val &= ~0xE0;
            if (val) {
                sendSpuCommand(2);
                func_801F0BF8(val);
                s->unk42 = val;
                *statePtr = 4;
            } else {
                func_801F1F58(s, s->field43);
            }
        } else if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 2;
        } else {
            func_801F1F58(s, s->field43);
        }
        break;
    }

    case 12: {
        s32 val1;
        s32 val2;
        s32 val3;
        s32 val4;
        s32 val5;
        val1 = D_801FAB98[s->field40].unk1;
        val1 &= 0x40;
        if (val1 != 0 && s->field43 >= 3) {
            s->field43 = 2;
        }
        val2 = s->field43;
        if (val2 >= 3) {
            val2 -= 3;
            val3 = (val2 / 2) + 3;
            val4 = val2 % 2;
            val5 = 1;
        } else {
            val3 = val2;
            val4 = 0;
            val5 = 0;
        }
        if (inputRepeat & PADLup) {
            sendSpuCommand(1);
            val3--;
            if (val3 < 0) {
                val3 = 2;
                if (val1 == 0) {
                    val4 = s->field4B;
                    val3 = s->unk4A - 1;
                }
            }
        } else if (inputRepeat & PADLdown) {
            sendSpuCommand(1);
            val3++;
            if (val1 != 0) {
                if (val3 >= 3) {
                    val3 = 0;
                }
            } else {
                if (val3 >= s->unk4A) {
                    val3 = 0;
                }
                if (val3 == 3) {
                    val4 = s->field4B;
                }
            }
        }

        // Dead code added to match with the original game binary.
        statePtr++; statePtr--;

        if (val5 != 0) {
            if (inputRepeat & PADLleft && val4 != 0) {
                sendSpuCommand(1);
                val4 = 0;
            }
            if (inputRepeat & PADLright && val4 == 0) {
                sendSpuCommand(1);
                val4 = 1;
            }
        }
        if (val3 < 3) {
            s->field43 = val3;
        } else {
            s->field43 = ((val3 - 3) * 2) + val4 + 3;
            s->field4B = val4;
        }
        state = 11;
        goto restart;
    }

    case 18:
        sendSpuCommand(5);
        s->field44 = 600;
        *statePtr = 19;
        break;

    case 19: {
        s->field44--;
        if (inputNew & (PADRup | PADRdown)) {
            s->field44 = 0;
            func_801F7BEC(inputNew);
            *statePtr = 20;
        }
        if (s->field44 < 0) {
            s->field44 = 0;
            *statePtr = 20;
        }
        break;
    }

    case 20:
        *statePtr = 9;
        break;

    case 4:
        s->field44 = 0;
        *statePtr = 5;
        /* fallthrough */

    case 5:
        s->brightness -= 256;
        if (s->brightness < 0) {
            D_801FAB2A = 0;
            s->brightness = 0;
            *statePtr = 6;
        }
        D_801FAB2A = s->brightness;
        break;

    case 6:
        D_8008520C = 0;
        D_801FAB7A = 0;
        D_801FAB2A = s->brightness;
        func_801F0C5C(s->unk42, s);
        *statePtr = 7;
        break;

    case 7: {
        s32 val = func_801F0D84();
        if (val == 1) {
            func_801F1E20(s);
            s->field20 = func_80036EC0();
            s->field32 = func_801F22F4();
            s->brightness = 0;
            func_801F5490(s);
            func_801F202C();
            D_801FAB31 = s->field40;
            D_801FAB2A = s->brightness;
            func_801F0948(ONE);
            func_801F1CAC();
            func_801F7B60();
            s->field43 = 0;
            s->field4B = 0;
            if (D_801FAB7A != 0) {
                s32 tmp = D_801FAB7A;
                s->unk22 = D_801FAB7B;
                D_801FAB7A = 0;
                D_801FAB7B = 0;
                func_801F0C5C(tmp, s);
                *statePtr = 7;
                break;
            }
            if (D_8008520C == val) {
                state = 21;
                goto restart;
            }
            *statePtr = 8;
        }
        break;
    }

    case 8:
        s->brightness += 256;
        if (s->brightness >= ONE) {
            s->brightness = ONE;
            D_801FAB2A = ONE;
            *statePtr = 2;
        }
        D_801FAB2A = s->brightness;
        break;

    case 13: {
        s32 i;
        s32 tmp;
        u8 *partyMembers1;
        s32 limit1;
        u8 *partyMembers2;
        s32 limit2;

        s->unk46 = 7;
        limit1 = 0;
        partyMembers1 = g_gameState.mainData.party.partyMembers;
        i = 0;

        do {
            i++;
            do {
                i %= 3;
            } while (0);
        } while (i != limit1 && partyMembers1[i] == 0xFF);

        tmp = i;
        
        // Dead code added to match with the original game binary.
        if (tmp) { tmp++; tmp--; }
        
        s->unk30 = tmp;

        limit2 = s->unk30;
        limit2 %= 3;
        i = limit2;
        partyMembers2 = g_gameState.mainData.party.partyMembers;

        do {
            i--;
            while (i < 0) {
                i += 3;
            }
            i %= 3;
        } while (i != limit2 && partyMembers2[i] == 0xFF);

        tmp = i;
        s->unk30 = tmp;
        s->unk31 = tmp;

        *statePtr = 14;
        break;
    }

    case 14:
        *statePtr = 15;
        /* fallthrough */

    case 15: {
        s32 val = s->unk30;
        if (inputRepeat & PADLdown) {
            sendSpuCommand(1);
            val++;
            val %= 3;
        }
        if (inputRepeat & PADLup) {
            sendSpuCommand(1);
            val--;
            if (val < 0) {
                val = 2;
            }
        }
        s->unk30 = val;
        func_801F1F58(s, s->unk30);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            func_801F1F98(s);
            *statePtr = 2;
        } else if (inputNew & PADLright) {
            sendSpuCommand(1);
            func_801F1F98(s);
            *statePtr = 2;
        } else if (inputRepeat & PADRdown) {
            sendSpuCommand(2);
            s->unk31 = s->unk30;
            *statePtr = 16;
        }
        break;
    }

    case 16: {
        s32 val = s->unk31;
        if (inputRepeat & PADLdown) {
            sendSpuCommand(1);
            val++;
            val %= 3;
        }
        if (inputRepeat & PADLup) {
            sendSpuCommand(1);
            val--;
            if (val < 0) {
                val = 2;
            }
        }
        s->unk31 = val;
        func_801F1F78(s, s->unk30);
        func_801F1F58(s, s->unk31);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 15;
        } else if (inputRepeat & PADRdown) {
            sendSpuCommand(2);
            *statePtr = 17;
        }
        break;
    }

    case 17: {
        s32 tmp = s->party[s->unk30];
        s->party[s->unk30] = s->party[s->unk31];
        s->party[s->unk31] = tmp;
        func_801F1E54(s);
        func_801F1F78(s, s->unk30);
        func_801F1F58(s, s->unk31);
        recalcPartyStats();
        s->unk30 = s->unk31;
        *statePtr = 15;
        break;
    }

    case 21:
        func_801F1E54(s);
        *statePtr = 22;
        /* fallthrough */

    case 22:
        s->brightness -= 256;
        if (s->brightness < 0) {
            s->brightness = 0;
            func_801F1DB0(0);
            func_801F18FC(s);
            func_801F0BB0();
        }
        func_801F1DB0(s->brightness);
        if (D_8008520C == 0) {
            func_801F0954(0, s->brightness, s->unk48, s->unk47);
        }
        break;

    }

    if (s->lastPlayTime != g_gameState.mainData.playTimeSeconds) {
        s->colonTimer = GetVideoMode() == MODE_NTSC ? 30 : 25;
        s->lastPlayTime = g_gameState.mainData.playTimeSeconds;
    }
    if (s->colonTimer != 0) {
        s->colonTimer--;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F2FAC);

void func_801F3270(void *ot, void *prim, s32 x, s32 y, u32 arg4) {
    u8 buffer[16];
    u8 *ptr;
    u32 seconds;
    u32 minutes;
    u32 color;

    if (GetVideoMode() == MODE_NTSC) {
        arg4 /= 60;
    } else {
        arg4 /= 50;
    }
    arg4 = CLAMP(arg4, arg4, 35999);
    seconds = arg4;
    minutes = seconds / 60;
    seconds %= 60;
    prim = drawIconClut(ot, prim, ICON_PLAY, x, y, g_menuTint[MENU_TINT_NORMAL], 450);
    x += 32;
    color = D_801F7F90[minutes / 100];
    ptr = &buffer[0];
    intToDecStringShort(minutes, ptr, g_numberFormat.digits[0]);
    ptr = &buffer[3];
    if (*ptr == g_numberFormat.digits[0]) {
        *ptr = getMenuString(11)[0];
    }
    prim = func_8002C56C(ot, prim, x, y, ptr, color);
    x += 16;
    prim = func_8002C56C(ot, prim, x, y, func_801F6AFC(13), color);
    x += 8;
    ptr = &buffer[0];
    intToDecStringShort(seconds, ptr, g_numberFormat.digits[0]);
    ptr = &buffer[3];
    func_8002C56C(ot, prim, x, y, ptr, color);
}

/**
 * @brief Draw the menu clock: the play time as hours:minutes, or the running
 * countdown as minutes:seconds.
 *
 * The colon is drawn at the menu brightness while @c colonTimer runs and at 2/3
 * of it otherwise, so it blinks once a second; a countdown at 0 keeps it bright.
 *
 * @param ctx The main menu's context.
 * @param ot Ordering table.
 * @param pkt Packet cursor.
 * @param x Left edge of the clock icon; the digits follow it.
 * @param y Top of the clock.
 * @param time The play time, divided by 60 (50 on PAL) into minutes, or the
 * countdown in seconds.
 * @param isPlayTime Non-zero for the play time.
 * @return The packet cursor after the clock.
 */
INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", drawMenuClock);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F36E8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F3824);

/**
 * @brief Draw character name panel with icon.
 *
 * Looks up the character name via func_801F6AD0, renders it with
 * func_801F0FEC, configures g_menuDisplayCfg (icon 0x55), and draws
 * the panel border via DrawMenuWindow.
 */
void func_801F38F8(s32 a0, s32 a1, s32 a2) {
    u8 *ret1;
    s32 ret2;

    ret1 = func_801F6AD0(*(u8 *)(a0 + 0x46));
    ret2 = func_801F0FEC(a1, a2, 0x22, 0xF, ret1, 7);
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 7;
    *(s32 *)&g_menuDisplayCfg.w = 0x001900F4; /* w=0xF4, h=0x19 packed */
    DrawMenuWindow(a1, ret2, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/** @brief Render text with explicit parameters (arg-reorder wrapper for func_801F0FEC). */
void func_801F3994(u8 *text, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F0FEC(a1, a2, a3, a4, text, a5);
}

/**
 * @brief Draw an icon glyph plus a two-digit zero-trimmed number.
 *
 * Renders @c ICON_LV_YELLOW via drawIconClut with a (a5 << 6) + 2 selection
 * parameter, then formats @p val as two digits (leading zeros replaced by
 * the first char of menu string 0xB) and prints them 18 pixels below via
 * func_8002C56C.
 *
 * @note getMenuString (main exe) is deliberately called without a prototype
 *       here, as in the original build; bc_object2.h carries the battle-side
 *       declaration and menumain must not include battle-overlay headers.
 *
 * @param val Number to render.
 * @param ctx Render context.
 * @param dl  Display-list cursor.
 * @param y   Anchor Y for the glyph; digits render at y + 18.
 * @param a4  X anchor (passed through to both renderers).
 * @param a5  Color/selection index for the digit pass.
 */
static void func_801F39D0(s32 val, s32 ctx, s32 dl, s32 y, s32 a4, s32 a5) {
    u8 buf[16];
    s32 digits = g_numberFormat.digits[0];
    s32 cursor;
    u8 *str;

    cursor = (s32)drawIconClut((void *)ctx, (TSPRT *)dl, ICON_LV_YELLOW, y, a4, g_menuTint[MENU_TINT_NORMAL], (a5 << 6) + 2);
    y += 0x12;
    intToDecStringShort(val, buf, digits);
    str = (u8 *)getMenuString(0xB);
    replaceLeadingZeros(buf + 2, 2, digits, str[0]);
    func_8002C56C(ctx, cursor, y, a4, buf + 2, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F3ABC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F3B64);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F3CE0);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F3DE4);

/**
 * @brief Map status flags to display text color.
 *
 * Returns: 7 (white/normal), 2 (yellow/critical HP),
 * 5 (blue/status ailment), 1 (gray/dead).
 */
s32 func_801F3FB4(s32 a0) {
    s32 v1 = 7;

    if (a0 & 0x100) {
        v1 = 2;
    }
    if (a0 & 0xFE) {
        v1 = 5;
    }
    if (a0 & 1) {
        v1 = 1;
    }
    return v1;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F3FE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4168);

/** @brief Draw ability/status list line (default mode, last param = 0). */
void func_801F4274(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F4168(a0, a1, a2, a3, a4, a5, 0);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F42A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4454);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4744);

/**
 * @brief Draw character command abilities panel.
 *
 * If a0[0x34] == 0, renders 3 command slots via func_801F42A4;
 * otherwise renders via func_801F3FE8 plus func_801F4744.
 */
s32 func_801F486C(u8 *a0, s32 a1) {
    s32 i;
    s32 result;

    if (a0[0x34] == 0) {
        for (i = 0; i < 3; i++) {
            result = func_801F42A4(a0, a1, result, i);
        }
    } else {
        for (i = 0; i < 3; i++) {
            result = func_801F3FE8(a0, a1, result, i);
        }
        result = func_801F4744(a0, a1);
    }
    return result;
}

/**
 * @brief Draw Gil (money) display panel.
 *
 * Renders the Gil label string, sets up g_menuDisplayCfg rendering params,
 * and draws the panel decoration via DrawMenuWindow.
 */
void func_801F4918(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F6358(a1, a2, 0x22, 0xC6, g_gameState.unk004);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 0xBE;
    g_menuDisplayCfg.w = 0xF4;
    g_menuDisplayCfg.h = 0x1A;
    DrawMenuWindow(a1, ret, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/* ======================================================================== */
/* Party Status & HP Display                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F49A4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4A98);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4C60);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4CE8);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4D70);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F4EA8);

/** @brief Convert raw ability ID to menu display index (subtract 100). */
s32 func_801F5104(s32 a0) {
    return a0 - 0x64;
}

/**
 * @brief Compute ability learn percentage (0-100).
 *
 * Subtracts 800 or 900 threshold depending on value, then clamps
 * the result to the 0-100 range.
 */
s32 func_801F510C(s32 a0) {
    s32 v1;

    if (a0 < 0x385) {
        a0 -= 0x320;
    } else {
        a0 -= 0x384;
    }
    v1 = 0;
    if (a0 >= 0) {
        v1 = 0x64;
        if (a0 < 0x65) {
            v1 = a0;
        }
    }
    return v1;
}

/** @brief Check if ability is fully mastered (value >= 901). */
s32 func_801F5144(s32 a0) {
    return a0 >= 0x385;
}

/**
 * @brief Compute HP color flags from current HP and max HP.
 *
 * Clears low bits of a2, sets 0x200 if HP < 50% max, 0x300 if
 * HP < 25% max. Returns 1 if HP <= 0 (dead).
 */
s32 func_801F5150(s32 a0, s32 a1, s32 a2) {
    a2 &= ~0x301;
    if (a0 < (a1 >> 1)) {
        a2 |= 0x200;
    }
    if (a0 < (a1 >> 2)) {
        a2 |= 0x300;
    }
    if (a0 <= 0) {
        a2 = 1;
    }
    return a2;
}

/**
 * @brief Updates a character's data for display in the main menu.
 *
 * Called for each of the eight characters when the main menu is displayed.
 *
 * @param charId Character ID.
 */
void func_801F5190(s32 charId) {
    MenuCharData *data;
    BattleCharData *btlChars;
    s32 statusFlags;
    s32 i;

    data = &D_801FABC8[charId];
    btlChars = &g_battleChars.chars[0];
    setPartyLeader(charId);
    data->currentLevel = btlChars->currentLevel;
    data->currentHp = btlChars->currentHp;
    data->maxHp = btlChars->maxHp;
    data->currentExp = btlChars->currentExp;
    data->expToNextLevel = btlChars->expToNextLevel;
    data->hpRatio = (data->currentHp * 48) / data->maxHp;
    data->mag = btlChars->stats[BATTLE_STAT_MAG];
    data->statusFlags = func_801F21D0(charId);
    statusFlags = func_801F5150(data->currentHp, data->maxHp, data->statusFlags);
    data->statusFlags = statusFlags;
    g_gameState.chars[charId].statusFlags = statusFlags;
    data->charId = charId;

    for (i = 0; i < 4; i++) {
        data->commandIds[i] = btlChars->abilityCommands[i].commandId;
    }

    for (i = 0; i < 4; i++) {
        s32 abilityId;
        s32 abilityCategory;

        abilityId = g_gameState.chars[charId].abilities[i];
        data->abilities[i].id = abilityId;

        abilityCategory = getAbilityCategory(abilityId) + 216;
        data->abilities[i].category = abilityCategory;
    }
}

/** @brief Save 3 active party slots to D_801FABC4 and clear originals to 0xFF. */
void func_801F5300(void) {
    s32 i = 0;

    for (; i < 3; i++) {
        D_801FABC4[i] = g_gameState.mainData.party.partyMembers[i];
        g_gameState.mainData.party.partyMembers[i] = PARTY_SLOT_EMPTY;
    }
}

/** @brief Restore 3 active party slots from D_801FABC4 backup. */
void func_801F5340(void) {
    s32 i = 0;

    for (; i < 3; i++) {
        g_gameState.mainData.party.partyMembers[i] = D_801FABC4[i];
    }
}

/**
 * @brief Recalculate party slot @p a0's stats and copy the result to @p dst.
 *
 * @param a0  Party slot to recalculate.
 * @param dst Receives the computed stat sheet (g_battleChars' first entry).
 */
void func_801F537C(s32 a0, BattleCharData *dst) {
    func_801F5300();
    func_801F5190(a0);
    *dst = g_battleChars.chars[0];
    func_801F5340();
    recalcPartyStats();
}

/** @brief Recalculate stats for a single party slot after swap. */
void func_801F5400(s32 a0) {
    func_801F5300();
    func_801F5190(a0);
    func_801F5340();
    recalcPartyStats();
}

/** @brief Recalculate stats for all 8 party slots. */
void func_801F5440(void) {
    s32 i;
    func_801F5300();
    for (i = 0; i < 8; i++) {
        func_801F5190(i);
    }
    func_801F5340();
    recalcPartyStats();
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F5490);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F565C);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F56E4);

/** @brief Set entity status flags, dispatched by type (character < 16, GF >= 16). */
void func_801F576C(s32 a0, s32 a1) {
    if (a0 >= 16) {
        func_801F2238(a0 - 16);
    } else {
        func_801F21FC(a0, a1);
    }
}

/** @brief Get entity status flags, dispatched by type (character < 16, GF >= 16). */
s32 func_801F57A4(s32 a0) {
    if (a0 >= 16) {
        return func_801F2240(a0 - 16);
    } else {
        return func_801F21D0(a0);
    }
}

/**
 * @brief Get entity current HP.
 *
 * For GF (a0 >= 16): reads from g_gameState at stride 68 + 0x62.
 * For character: returns 0 if dead, else reads at stride 152 + 0x490.
 */
u16 func_801F57DC(s32 a0) {
    if (a0 >= 16) {
        a0 -= 16;
        return g_gameState.gfs[a0].hp;
    } else {
        if (func_801F57A4(a0) & 1) {
            return 0;
        }
        return g_gameState.chars[a0].currentHp;
    }
}

/** @brief Set entity current HP (updates both primary and cache tables). */
void func_801F5868(s32 entityId, s32 hp) {
    if (entityId >= 16) {
        s32 gfId = entityId - 16;
        g_gameState.gfs[gfId].hp = hp;
        g_battleChars.gfs[gfId].hp = hp;
    } else {
        g_gameState.chars[entityId].currentHp = hp;
        D_801FABC8[entityId].currentHp = hp;
    }
}

/** @brief Get entity max HP. */
s32 func_801F58EC(s32 entityId) {
    if (entityId < 16) {
        return D_801FABC8[entityId].maxHp;
    } else {
        s32 gfId = entityId - 16;
        return g_battleChars.gfs[gfId].maxHp;
    }
}

/** @brief Get entity level. */
s32 func_801F5938(s32 entityId) {
    if (entityId < 16) {
        return D_801FABC8[entityId].currentLevel;
    } else {
        s32 gfId = entityId - 16;
        return g_battleChars.gfs[gfId].level;
    }
}

/**
 * @brief Build cumulative pixel-width table for menu item strings.
 *
 * Iterates a -1-terminated u16 source list, measures each string's
 * pixel width via getTextSize, and accumulates offsets into dst.
 * Returns the item count.
 */
s32 func_801F5984(u16 *src, u16 *dst, s32 a2) {
    s32 accum = 0;
    s32 count = 0;
    s16 val;
    s32 ret;
    *dst++ = 0;
    while (1) {
        val = (s16)*src++;
        if (val == -1) break;
        ret = func_801F08D4(1, a2, val, 0);
        ret = getTextSize((u8 *)ret) + 12;
        accum += ret;
        *dst++ = accum;
        count++;
    }
    return count;
}

/* ======================================================================== */
/* Number & String Rendering                                                */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F5A38);

/**
 * @brief Size and draw a text-list panel from a -1-terminated id list.
 *
 * Measures every entry of @p ids (looked up in text category @p textCat via
 * func_801F08D4) to find the widest string, then sizes g_menuDisplayCfg to
 * fit — width = 8 pixels per character + 16 padding, height = 12 per row
 * plus 10 — and draws the panel frame with DrawMenuWindow.
 *
 * @note Declaring @c count before @c maxWidth is load-bearing: the two have
 *       identical allocator priority, so the tie breaks on declaration order.
 *
 * @param ctx     Render context.
 * @param dl      Display-list cursor.
 * @param x       Panel X.
 * @param y       Panel Y.
 * @param textCat Text category for func_801F08D4 lookups.
 * @param ids     Entry ids, terminated by a negative value.
 * @param param   Intensity/param passed to DrawMenuWindow.
 * @return Display-list cursor after the panel.
 */
static s32 func_801F5B54(s32 ctx, s32 dl, s32 x, s32 y, s32 textCat, u16 *ids, s32 param) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 count;
    s32 maxWidth;
    s16 id;
    u8 *str;
    s32 len;

    maxWidth = 0;
    count = maxWidth;
    while (1) {
        id = *ids++;
        if (id < 0) {
            break;
        }
        str = func_801F08D4(1, textCat, id, 0);
        len = 0;
        while (*str++ != 0) {
            len++;
        }
        len *= 8;
        if (maxWidth < len) {
            maxWidth = len;
        }
        count++;
    }
    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = maxWidth + 0x10;
    cfg->h = count * 12 + 0xA;
    dl = DrawMenuWindow(ctx, dl, param, g_menuTint[MENU_TINT_NORMAL]);
    return dl;
}

/**
 * @brief Draw a text-list panel's entries, greying out disabled rows.
 *
 * Renders each entry of the -1-terminated @p ids list (looked up in text
 * category @p textCat) down the panel at 12-pixel row pitch. Bit i of
 * @p enableMask selects the colour of row i: 7 when set (enabled), 1 when
 * clear (greyed out).
 *
 * @note @p enableMask must be unsigned so the per-row shift is a logical
 *       one, and the @c tx / @c ty cursor copies (assigned in that order)
 *       are what give x, textCat and ctx their saved registers.
 *
 * @param ctx        Render context.
 * @param dl         Display-list cursor.
 * @param x          Row X.
 * @param y          Y of the first row.
 * @param textCat    Text category for func_801F08D4 lookups.
 * @param ids        Entry ids, terminated by a negative value.
 * @param enableMask Per-row enabled bits, LSB = first row.
 * @return Display-list cursor after the rows.
 */
static s32 func_801F5C84(s32 ctx, s32 dl, s32 x, s32 y, s32 textCat, u16 *ids, u32 enableMask) {
    s32 tx;
    s32 ty;
    s16 id;
    u8 *str;
    s32 color;

    tx = x;
    ty = y;
    while (1) {
        id = *ids++;
        if (id < 0) {
            break;
        }
        str = func_801F08D4(1, textCat, id, 0);
        color = 1;
        if (enableMask & 1) {
            color = 7;
        }
        dl = func_8002C56C(ctx, dl, tx, ty, str, color);
        ty += 0xC;
        enableMask >>= 1;
    }
    return dl;
}

/**
 * @brief Draw a complete menu panel: clip restore, border, then body.
 *
 * Chains three packet builders on the display list: func_801F03E8
 * (draw-area restore), func_801F5C84 at (x+8, y+7) with @p a7, then
 * func_801F5B54 at (x, y) with @p a6. Each call's return value is the
 * next call's packet cursor.
 *
 * @param ctx        Render context (also the ordering-table slot).
 * @param prim       Packet cursor.
 * @param x          Panel X.
 * @param y          Panel Y.
 * @param textCat    Text category for both panel builders.
 * @param ids        Entry ids for both panel builders.
 * @param bodyParam  Intensity/param for the body pass (func_801F5B54).
 * @param insetParam Intensity/param for the inset pass (func_801F5C84).
 * @return Packet cursor after the body pass.
 */
static s32 func_801F5D5C(s32 ctx, DR_AREA *prim, s32 x, s32 y, s32 textCat, u16 *ids, s32 bodyParam, s32 insetParam) {
    s32 dl = (s32)func_801F03E8((P_TAG *)ctx, prim);

    dl = func_801F5C84(ctx, dl, x + 8, y + 7, textCat, ids, insetParam);
    return func_801F5B54(ctx, dl, x, y, textCat, ids, bodyParam);
}

/**
 * @brief Draw a two-digit page/slot counter as glyphs.
 *
 * Renders @c ICON_PAGE, then the digits of @p val + 1: the tens
 * digit only when @p flag is set, then the units digit. Each glyph advances
 * the cursor.
 *
 * @note intToDecStringShort formats into five digit glyphs (its divisor table
 *       runs 10000..1), so buf[3] is the tens place and buf[4] the units;
 *       replaceLeadingZeros blanks the tens glyph when it would be a zero.
 *
 * @param ctx   Render context.
 * @param dl    Display-list cursor.
 * @param x     Start X (advances 9 then 6 pixels per drawn glyph).
 * @param y     Screen Y.
 * @param color Glyph color.
 * @param val   Zero-based value; displayed as @p val + 1.
 * @param flag  Nonzero to draw the tens digit.
 * @return Display-list cursor after the glyphs.
 */
static s32 func_801F5E0C(s32 ctx, s32 dl, s32 x, s32 y, s32 color, s32 val, s32 flag) {
    u8 buf[16];

    intToDecStringShort(val + 1, buf, ICON_SMALL_DIGIT_0);
    replaceLeadingZeros(buf + 3, 1, ICON_SMALL_DIGIT_0, ICON_BLANK);
    dl = drawIcon(ctx, dl, ICON_PAGE, x, y, color);
    x += 9;
    if (flag != 0) {
        dl = drawIcon(ctx, dl, buf[3], x, y, color);
        x += 6;
    }
    dl = drawIcon(ctx, dl, buf[4], x, y, color);
    return dl;
}

/** @brief Render number right-aligned (last param = 1). */
s32 func_801F5EFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 1);
}

/** @brief Render number left-aligned (last param = 0). */
s32 func_801F5F30(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F5E0C(a0, a1, a2, a3, a4, a5, 0);
}

/**
 * @brief Draw the scroll arrows along a panel's bottom edge.
 *
 * Renders the left arrow (@c ICON_ARROW_LEFT) at the panel's bottom-left and the
 * right arrow (@c ICON_ARROW_RIGHT) at its bottom-right, each gated by a bit of
 * @p arrows. Both blink together: on alternate 8-frame phases of
 * g_menuDisplayCfg.animCounter the selection parameter drops to 0.
 *
 * @note The shared @c bottom temp is what evaluates the y/h pair before
 *       the x load (scheduling match); a per-branch local does not.
 *
 * @param ctx    Render context.
 * @param dl     Display-list cursor.
 * @param color  Color passed through to both glyphs.
 * @param arrows Bit 0: draw left arrow. Bit 1: draw right arrow.
 * @return Display-list cursor after the drawn arrows.
 */
s32 func_801F5F60(s32 ctx, s32 dl, s32 color, s32 arrows) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 blink = 0x140;
    s32 bottom;

    if ((cfg->animCounter >> 3) & 1) {
        blink = 0;
    }
    if (arrows & 1) {
        bottom = cfg->y + cfg->h - 10;
        dl = (s32)drawIconClut((void *)ctx, (TSPRT *)dl, ICON_ARROW_LEFT, g_menuDisplayCfg.x + 2, bottom, color, blink);
    }
    if (arrows & 2) {
        bottom = cfg->y + cfg->h - 10;
        dl = (s32)drawIconClut((void *)ctx, (TSPRT *)dl, ICON_ARROW_RIGHT, g_menuDisplayCfg.x + cfg->w - 9, bottom, color, blink);
    }
    return dl;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F605C);

/**
 * @brief Draw the status icons of @p mask in a 3-column grid.
 *
 * For each set bit i of @p mask among @c STATUS_ICON_MASK, draws icon
 * @c ICON_STATUS_KO + i at an 18x18 grid cell, filling left-to-right then
 * top-to-bottom.
 *
 * @param ctx  Render context.
 * @param dl   Display-list cursor (threaded through the glyph calls).
 * @param x    Grid origin X.
 * @param y    Grid origin Y.
 * @param mask Persistent status bits.
 * @return Display-list cursor after the drawn glyphs.
 */
s32 func_801F6234(s32 ctx, s32 dl, s32 x, s32 y, s32 mask) {
    s32 drawn = 0;
    s32 i;

    mask &= STATUS_ICON_MASK;
    for (i = 0; i < 8; i++) {
        s32 bit = 1 << i;

        if (mask & bit) {
            s32 row = drawn / 3;
            s32 col = drawn - row * 3;

            dl = drawIcon(ctx, dl, i + ICON_STATUS_KO, x + col * 18, y + row * 18, g_menuTint[MENU_TINT_NORMAL]);
            drawn++;
        }
    }
    return dl;
}

/* ======================================================================== */
/* String Resource Lookup                                                   */
/* ======================================================================== */

/** @brief Look up string resource pointer by index from D_801F8BB8 table. */
u8 *func_801F6324(s32 a0) {
    u16 *table = (u16 *)D_801F8BB8;

    if (a0 >= table[0]) {
        a0 = 0;
    }
    a0 += 1;
    return (u8 *)table + table[a0];
}

/** @brief Decode and render an indexed string resource to screen. */
static s32 func_801F6358(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    u8 buf[256];
    u8 *ptr = func_801F6324(a4);
    decodeMessage(ptr, buf, -1);
    func_801F0FEC(a0, a1, a2, a3, buf, 7);
}

void *MenuDrawPortrait(s32 idx, void *ot, void *head, s32 x, s32 y, u32 color) {
    return DrawPortrait(ot, head, idx, x, y, color);
}

/**
 * @brief Conditionally render highlighted entry if character bit is set.
 *
 * Checks if the bit at position a0 in the party mask is set.
 * If so, renders the entry via drawIcon with a highlight color
 * of 0xD6. Otherwise returns the input OT pointer unchanged.
 *
 * @param a0 Bit position to check in party mask.
 * @param a1 OT base pointer.
 * @param a2 Current draw pointer (returned if bit not set).
 * @param a3 Y coordinate.
 * @param a4 Additional render parameter (5th arg on stack).
 * @return Updated draw pointer.
 */
s32 func_801F6418(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    s32 mask = menumain_getPartyMemberMask();

    if (((mask & 0xFFFF) >> a0) & 1) {
        a2 = drawIcon(a1, a2, ICON_PARTY_MEMBER, a3, a4, g_menuTint[MENU_TINT_NORMAL]);
    }
    return a2;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F64A4);

/**
 * @brief Draw full character info panel.
 *
 * Chains: func_801F64A4 (header/portrait), func_801F3DE4 (stats block),
 * func_801F6234 (HP/status bar), func_801F605C (ability/junction summary).
 */
s32 func_801F65F0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 ret;

    ret = func_801F64A4(a0, a1, a2, a3, a4, a5, 1);
    ret = func_801F3DE4(*(s32 *)a5, *(s32 *)(a5 + 4), a0, ret, a2 + 0x20, a3 + 0x7C, 7);
    ret = func_801F6234(a0, ret, a2 + 0xD0, a3 + 0x39, *(u16 *)(a5 + 0xE));
    return func_801F605C(a0, ret, a2 + 0x10E, a3 + 0x38, a5);
}

/**
 * @brief Draw a weapon's name in its status panel slot.
 *
 * Anchors g_menuDisplayCfg at (x+200, y+121) with a 154x22 box and no
 * icon, draws the name from getWeaponName(weaponId) at (x+209, y+128)
 * via func_801F0FEC, then draws the panel frame with DrawMenuWindow at
 * full intensity.
 *
 * @note @p x and @p y are advanced in place — reassigning the parameters
 *       restores their true live lengths so px/py keep s0/s1 (allocation
 *       match).
 *
 * @param ctx     Render context.
 * @param dl      Display-list cursor.
 * @param x       Panel base X.
 * @param y       Panel base Y.
 * @param weaponId Weapon to display.
 */
void func_801F66B0(s32 ctx, s32 dl, s32 x, s32 y, s32 weaponId) {
    u8 *name;

    x += 0xC8;
    y += 0x79;
    g_menuDisplayCfg.x = x;
    x += 9;
    g_menuDisplayCfg.w = 0x9A;
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.h = 0x16;
    y += 7;
    name = getWeaponName(weaponId);
    dl = func_801F0FEC(ctx, dl, x, y, name, 7);
    DrawMenuWindow(ctx, dl, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Step a cursor down or up the D-pad, wrapping at both ends.
 *
 * If max == 1 (single option), returns 0 immediately.
 * Down advances the cursor and wraps to 0; up retreats it and wraps to max-1.
 * Plays a sound effect on each valid input.
 *
 * @param flags   One of the g_menuDisplayCfg input words.
 * @param max     Maximum position value (exclusive).
 * @param current Current cursor position.
 * @return Updated cursor position, or 0 if max == 1.
 */
s32 func_801F6768(u16 flags, s32 max, s32 current) {
    if (max == 1) {
        return 0;
    }
    if (flags & PADLdown) {
        sendSpuCommand(1);
        current++;
        if (current >= max) {
            current = 0;
        }
    }
    if (flags & PADLup) {
        sendSpuCommand(1);
        current--;
        if (current < 0) {
            current = max - 1;
        }
    }
    return current;
}

/**
 * @brief Step a cursor right or left the D-pad, wrapping at both ends.
 *
 * Right advances the cursor and wraps to 0; left retreats it and wraps to max-1.
 * Plays a sound effect on each valid input.
 *
 * @param flags   One of the g_menuDisplayCfg input words.
 * @param max     Maximum position value (exclusive).
 * @param current Current cursor position.
 * @return Updated cursor position.
 */
s32 func_801F6800(u16 flags, s32 max, s32 current) {
    if (flags & PADLright) {
        sendSpuCommand(1);
        current++;
        if (current >= max) {
            current = 0;
        }
    }
    if (flags & PADLleft) {
        sendSpuCommand(1);
        current--;
        if (current < 0) {
            current = max - 1;
        }
    }
    return current;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F6888);

/**
 * @brief Initialize the main menu state.
 *
 * Clears the menu flag, recalculates party stats, sets up the panel
 * callback chain, initializes display lists, and populates the menu
 * context structure with GF availability, character masks, and defaults.
 */
void func_801F6934(void) {
    MainMenuCtx *ctx;

    D_801FAB7C = 0;
    recalcPartyStats();
    ctx = func_801F179C(func_801F2458, func_801F4A98);
    func_801F1D2C((s32)&D_800562A4, (s32)&D_801F7DF4, (s32)D_801F8BB8);
    func_801F1D2C(0, (s32)&D_801F7E00, (s32)D_801F889C);
    func_801F1D2C(0, (s32)&D_801F7E0C, (s32)&D_801F87B8);
    func_801F1CAC();
    if (ctx != NULL) {
        D_801FAB28 = 0x1000;
        ctx->brightness = 0;
        D_801FAB2A = 0x1000;
        ctx->field20 = func_80036EC0();
        ctx->field32 = func_801F22F4();
        ctx->field44 = 0;
        ctx->field43 = 0;
        ctx->field4B = 0;
        func_801F5490((s32)ctx);
        func_801F1E54(ctx);
        func_801F202C();
        ctx->brightness = 0;
        func_801F2458(ctx);
        ctx->field23 = popcount(ctx->field20);
        {
            u8 tmp = D_801FAB30;
            ctx->field40 = 0;
            ctx->colonTimer = 0;
            ctx->field41 = tmp;
        }
    }
    func_801F1DB0(0);
    func_801F7B60();
}

/** @brief Advance pseudo-random number generator (LCG: val*125+14 mod 32768). */
s32 func_801F6A5C(void) {
    s32 base = (s32)&g_engine;
    s32 val = *(u16 *)(base + 0x9C2);
    val = (val * 125 + 14) % 32768;
    *(u16 *)(base + 0x9C2) = val;
    return val;
}

/** @brief Look up string @p a0 in menu text category 3. */
u8 *func_801F6AA4(s32 a0) {
    return func_801F08D4(1, 3, a0, 0);
}

/** @brief Look up string @p a0 in menu text category 0, with the last flag set. */
u8 *func_801F6AD0(s32 a0) {
    return func_801F08D4(0, 0, a0, 1);
}

/** @brief Look up character description string. */
u8 *func_801F6AFC(s32 a0) {
    return func_801F08D4(0, 0, a0, 0);
}

/** @brief Map ability index to display category via double indirection. */
s32 func_801F6B28(s32 a0) {
    return D_801F7F98[D_801F889C[a0].type];
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F6B54);

/**
 * @brief Render a column of bitmask-selected entries inside a 168x120 panel.
 *
 * For each set bit i (0..7) of @p mask, renders entry i at (x+8, ty) via
 * func_801F6B54, advancing ty by 13 per drawn row. Then anchors
 * g_menuDisplayCfg at (x, y) with size 168x120 and icon 0x49 and draws the
 * panel frame with DrawMenuWindow.
 *
 * @param ctx  Render context.
 * @param dl   Display-list cursor (threaded through the row calls).
 * @param x    Panel X.
 * @param y    Panel Y.
 * @param a4   Passed to DrawMenuWindow (intensity/param).
 * @param mask Bitmask of rows to draw, bit index = entry index.
 */
void *func_801F6C9C(s32 ctx, s32 dl, s32 x, s32 y, s32 a4, u16 mask) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 tx = x + 8;
    s32 ty = y + 0xA;
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((mask >> i) & 1) {
            dl = func_801F6B54(ctx, dl, tx, ty, i);
            ty += 0xD;
        }
    }
    cfg->iconType = 0x49;
    cfg->w = 0xA8;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->h = 0x78;
    return DrawMenuWindow(ctx, dl, a4, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F6D88);

/* ======================================================================== */
/* Config/Settings Display                                                  */
/* ======================================================================== */

/** @brief Get packed cursor coordinates (D_801FACE4 << 16 | D_801FACE2). */
s32 func_801F6F6C(void) {
    return (D_801FACE4 << 16) | D_801FACE2;
}

/** @brief Render menu item at grid position derived from hi/lo nibbles of a0. */
void func_801F6F88(s32 a0) {
    s32 hi = a0 >> 4;
    s32 lo = a0 & 0xF;
    s32 val = func_801F6F6C();

    func_801F0994(lo | hi, (u16)val, (val >> 16) + lo * 13);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F6FE4);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F7148);

/** @brief Render config option (simplified wrapper for func_801F7148). */
void func_801F728C(s32 a0, s32 a1) {
    func_801F7148(a0, 0, 0, a1);
}

/** @brief Get current config value. */
s32 func_801F72B4(void) {
    return D_801FACE8;
}

/**
 * @brief Collect the party-wide junction-ability bitmask into D_801FACE8.
 *
 * For every owned GF (GF_EXISTS), enumerates its available abilities via
 * func_800369CC and sets bit (slotIndex - 0x5C) in D_801FACE8 for each
 * entry in the junction-ability slot range 0x5C..0x73.
 */
void func_801F72C4(void) {
    AbilityListEntry list[22];
    s32 i;

    D_801FACE8 = 0;
    for (i = 0; i < 16; i++) {
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            s32 n = func_800369CC(i, list, 0);
            AbilityListEntry *p = list;

            for (; n > 0; n--, p++) {
                u32 c = p->slotIndex - 0x5C;

                if (c < 0x18) {
                    D_801FACE8 |= 1 << c;
                }
            }
        }
    }
}

/* ======================================================================== */
/* Junction/Ability Management                                              */
/* ======================================================================== */

/** @brief Convert character index to GF table index (add 0x10). */
s32 func_801F738C(s32 a0) {
    return a0 + 0x10;
}

/** @brief Convert character index to GF table index + 1 (add 0x11). */
s32 func_801F7394(s32 a0) {
    return a0 + 0x11;
}

/**
 * @brief Render scrollable panel with footer.
 *
 * Sets up g_menuDisplayCfg rendering params (icon 0x4A), then renders
 * the list, scroll indicator, and footer/help text.
 */
void func_801F739C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, MenuRowCallback a5) {
    s32 base = (s32)&g_menuDisplayCfg;
    s32 ret1;
    s32 ret2;

    *(u8 *)(base + 0x10) = 0x4A;  /* iconType */
    *(u8 *)(base + 0x11) = 0;     /* iconSubType */
    *(s16 *)&g_menuDisplayCfg = a2;     /* x */
    *(s16 *)(base + 4) = 0x9A;    /* w */
    *(s16 *)(base + 2) = a3;      /* y */
    *(s16 *)(base + 6) = 0x40;    /* h */
    *(u8 *)(base + 0x13) = 4;     /* columnCount */
    *(u8 *)(base + 0x1E) = a4;    /* itemId */

    ret1 = func_801F5F30(a0, a1, a2 + 0x24, a3, g_menuTint[MENU_TINT_NORMAL], *(u8 *)(base + 0x16) /* pageStart */);
    ret2 = func_801F5F60(a0, ret1, g_menuTint[MENU_TINT_NORMAL], 3);
    func_801EFBB4(a0, ret2, a5);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F7454);

/** @brief Apply junction change, then update cursor and recalculate party stats. */
s32 func_801F75A4(s32 a0) {
    s32 result = func_801F7454(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
    return result;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2b", func_801F75EC);

/** @brief Remove junction, then update cursor and recalculate party stats. */
void func_801F76A8(s32 a0) {
    func_801F75EC(a0);
    func_801F1B4C(a0);
    func_801F5400(a0);
}

/**
 * @brief Nudge a value right or left with the D-pad.
 *
 * Plays a sound effect if the value changed.
 */
s32 func_801F76E0(s32 flags, s32 a1, s32 a2) {
    s32 result = a2;
    s32 orig = a2;

    if (flags & PADLright) {
        result = func_80035B28(a1, result);
    }
    if (flags & PADLleft) {
        result = func_80035B70(a1, orig);
    }
    if (result != orig) {
        sendSpuCommand(1);
    }
    return result;
}
