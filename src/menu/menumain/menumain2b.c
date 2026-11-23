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
extern s32 D_801FABBC;
extern s32 D_801FABC0;
extern u8 D_801FABC4[4];
extern u8 *D_801FACC8;
extern u8 *D_801FACCC;
extern u8 *D_801FACD0;
extern RECT D_801FACD8;
extern u16 D_801FACDA;
extern s16 D_801FACE0;
extern s16 D_801FACE2;
extern s16 D_801FACE4;
extern s32 D_801FACE8;

static void func_801F2458(MainMenuCtx *ctx);
static void *drawMenuClock(MainMenuCtx *s, void *ot, TSPRT *prim, s32 x, s32 y, u32 time, s32 isPlayTime);
static void *func_801F36E8(void *ot, void *prim, s32 x, s32 y);
static void *func_801F39D0(s32, s32, s32, s32, s32, s32);
static void *func_801F3B64(s32 value1, s32 value2, void *ot, void *prim, s32 x, s32 y, u32 color);
static void *func_801F3FE8(MainMenuCtx *s, void *ot, void *prim, s32 arg3);
static void *func_801F42A4(MainMenuCtx *ctx, void *ot, void *prim, s32 partySlot);
static void *func_801F4454(MainMenuCtx *ctx, void *ot, void *prim, s32 x, s32 y, s32 index);
static void *func_801F4744(MainMenuCtx *s, void *ot, void *prim);
static void *func_801F49A4(MainMenuCtx *s, void *ot, void *prim);
static void *func_801F4A98(MainMenuCtx *s, void *ot, void *prim);
static void *func_801F4C60(void *ot, void *prim, u32 value, s32 x, s32 y);
static void *func_801F4CE8(void *ot, void *prim, u32 value, s32 x, s32 y);
static void func_801F5490(MainMenuCtx *ctx);
static s32 func_801F5B54(s32, s32, s32, s32, s32, u16 *, s32);
static s32 func_801F5C84(s32, s32, s32, s32, s32, u16 *, u32);
static s32 func_801F5D5C(s32, DR_AREA *, s32, s32, s32, u16 *, s32, s32);
static s32 func_801F5E0C(s32, s32, s32, s32, s32, s32, s32);
static s32 func_801F6358(s32, s32, s32, s32, s32);
static void *func_801F6B54(void *ot, void *prim, s32 x, s32 y, s32 charId);

#define gte_dpcs_get_rgb(result, rgb, value)  \
    __asm__ volatile(                         \
        "mtc2   %1, $6\n"                     \
        "mtc2   %2, $8\n"                     \
        "nop\n"                               \
        "nop\n"                               \
        "c2     0x780010\n"                   \
        "mfc2   %0, $22\n"                    \
        : "=r"(result)                        \
        : "r"(rgb), "r"(value)                \
        : "memory"                            \
    )

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

            if ((val & 0x20 && g_charMenuInfo[s->unk22].statusFlags & 1) || s->unk22 == 0xFF) {
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

DR_AREA *func_801F2FAC(P_TAG *ot, DR_AREA *prim) {
    u32 oldBrightness;
    MenuDisplayConfig *cfg;
    s32 xBase;
    s32 yBase;
    s32 x;
    s32 y;
    s32 h;
    s32 i;
    ST_D_801FAB98 *entry;
    s32 y1;
    s32 y2;
    s32 h1;
    s32 h2;
    s32 brightness;

    xBase = 270;
    yBase = 6;

    if (g_activeDrawEnv->clip.w < 384) {
        return prim;
    }

    brightness = D_801FAB28;
    if (brightness == 0) {
        return prim;
    }

    ot++;
    prim = func_801F03E8(ot, prim);
    oldBrightness = g_menuBrightness;
    setMenuBrightness(brightness);
    
    cfg = &g_menuDisplayCfg;
    x = 277;
    y = 12;

    y1 = D_801FAB31;
    y2 = (ONE - D_801FAB2A) * 14;
    y -= (y1 * y2) / 4096;

    entry = D_801FAB2C;
    if (entry != NULL) {
        for (i = 0; i < D_801FAB30; i++, entry++) {
            u32 color = (entry->unk1 != 0) ? DIALOG_COLOR_WHITE : DIALOG_COLOR_GRAY;
            u8 *text = func_801F6AFC(entry->unk0);
            if (entry->unk0 == 6 && D_801FAB7C != 0) {
                s32 w = getFirstLineWidth(text);
                prim = drawIcon(ot, prim, 213, x + w, y + 1, g_menuTint[MENU_TINT_NORMAL]);
            }
            prim = func_801F0FEC(ot, prim, x, y, text, color);
            y += 14;
        }
    }

    h = D_801FAB30 * 14;
    h1 = h * D_801FAB2A;
    h2 = (ONE - D_801FAB2A) * 12;
    h = (h1 + h2) / ONE;

    cfg->x = xBase + 2;
    cfg->y = yBase + 6;
    cfg->w = 88;
    cfg->h = h;

    prim = func_801EF800(ot, prim, cfg);
    h += 10;

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = xBase;
    cfg->y = yBase;
    cfg->w = 90;
    cfg->h = h;

    prim = DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
    setMenuBrightness(oldBrightness);
    return func_801F03E8(ot, prim);
}

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
 * @param s The main menu's context.
 * @param ot Ordering table.
 * @param prim Packet cursor.
 * @param x Left edge of the clock icon; the digits follow it.
 * @param y Top of the clock.
 * @param time The play time, divided by 60 (50 on PAL) into minutes, or the
 * countdown in seconds.
 * @param isPlayTime Non-zero for the play time.
 * @return The packet cursor after the clock.
 */
static void *drawMenuClock(MainMenuCtx *s, void *ot, TSPRT *prim, s32 x, s32 y, u32 time, s32 isPlayTime) {
    u8 buffer[16];
    u8 *ptr;
    u8 *text;
    u32 iconId;
    u32 seconds;
    u32 color;
    u32 timeOld;

    timeOld = time;
    if (isPlayTime != 0) {
        if (GetVideoMode() == MODE_NTSC) {
            time /= 60;
        } else {
            time /= 50;
        }
    }
    time = CLAMP(time, time, 35999);
    seconds = time;
    time = seconds / 60;
    seconds %= 60;
    iconId = isPlayTime != 0 ? ICON_PLAY : ICON_TIME_YELLOW;
    prim = drawIconClut(ot, prim, iconId, x, y, g_menuTint[MENU_TINT_NORMAL], 450);
    x += 32;
    color = D_801F7F90[time / 100];
    ptr = &buffer[0];
    intToDecStringShort(time, ptr, g_numberFormat.digits[0]);
    ptr = &buffer[3];
    if (*ptr == g_numberFormat.digits[0]) {
        *ptr = getMenuString(11)[0];
    }
    prim = func_8002C56C(ot, prim, x, y, ptr, color);
    x += 16;
    text = func_801F6AFC(13);
    if (s->colonTimer != 0) {
        setMenuBrightness(s->brightness);
    } else {
        setMenuBrightness((s->brightness * 2) / 3);
    }
    if (isPlayTime == 0 && timeOld == 0) {
        setMenuBrightness(s->brightness);
    }
    prim = func_8002C56C(ot, prim, x, y, text, color);
    x += 8;
    setMenuBrightness(s->brightness);
    ptr = &buffer[0];
    intToDecStringShort(seconds, ptr, g_numberFormat.digits[0]);
    ptr = &buffer[3];
    prim = func_8002C56C(ot, prim, x, y, ptr, color);
    return prim;
}

static void *func_801F36E8(void *ot, void *prim, s32 x, s32 y) {
    u8 buffer[16];
    s32 digitBase = g_numberFormat.digits[0];
    s32 x2 = x + 48;
    s32 val = func_801F7A54();
    if (val >= 0) {
        prim = drawIconClut(ot, prim, ICON_SEED, x, y, g_menuTint[MENU_TINT_NORMAL], 450);
        if (val != 31) {
            intToDecStringShort(val, &buffer[0], digitBase);
            replaceLeadingZeros(&buffer[2], 2, digitBase, getMenuString(11)[0]);
            prim = func_8002C56C(ot, prim, x2, y, &buffer[2], 7);
        } else {
            prim = drawIconClut(ot, prim, ICON_A_YELLOW, x2, y, g_menuTint[MENU_TINT_NORMAL], 450);
        }
    }
    return prim;
}

void *func_801F3824(void *ot, void *prim, s32 x, s32 y, u32 value) {
    u8 buffer[16];
    s32 digitBase = g_numberFormat.digits[0];
    intToDecString(value, &buffer[0], digitBase);
    replaceLeadingZeros(&buffer[2], 7, digitBase, getMenuString(11)[0]);
    prim = func_8002C56C(ot, prim, x, y, &buffer[2], 7);
    x += 64;
    return drawIcon(ot, prim, 11, x, y, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Draw character name panel with icon.
 *
 * Looks up the character name via func_801F6AD0, renders it with
 * func_801F0FEC, configures g_menuDisplayCfg (icon 0x55), and draws
 * the panel border via DrawMenuWindow.
 */
void *func_801F38F8(s32 a0, s32 a1, s32 a2) {
    u8 *ret1;
    s32 ret2;

    ret1 = func_801F6AD0(*(u8 *)(a0 + 0x46));
    ret2 = func_801F0FEC(a1, a2, 0x22, 0xF, ret1, 7);
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 7;
    *(s32 *)&g_menuDisplayCfg.w = 0x001900F4; /* w=0xF4, h=0x19 packed */
    return DrawMenuWindow(a1, ret2, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/** @brief Render text with explicit parameters (arg-reorder wrapper for func_801F0FEC). */
void *func_801F3994(u8 *text, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    return func_801F0FEC(a1, a2, a3, a4, text, a5);
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
static void *func_801F39D0(s32 val, s32 ctx, s32 dl, s32 y, s32 a4, s32 a5) {
    u8 buf[16];
    s32 digits = g_numberFormat.digits[0];
    s32 cursor;
    u8 *str;

    cursor = (s32)drawIconClut((void *)ctx, (TSPRT *)dl, ICON_LV_YELLOW, y, a4, g_menuTint[MENU_TINT_NORMAL], (a5 << 6) + 2);
    y += 0x12;
    intToDecStringShort(val, buf, digits);
    str = (u8 *)getMenuString(0xB);
    replaceLeadingZeros(buf + 2, 2, digits, str[0]);
    return func_8002C56C(ctx, cursor, y, a4, buf + 2, a5);
}

void *func_801F3ABC(s32 value, void *ot, void *prim, s32 x, s32 y, u32 color) {
    u8 *str = func_801F6AFC(18);
    prim = func_801F0FEC(ot, prim, x, y, str, color);
    x += 62;
    return drawNumberMenuTint(ot, prim, (y << 16) | (x & 0xFFFF), value, color);
}

static void *func_801F3B64(s32 value1, s32 value2, void *ot, void *prim, s32 x, s32 y, u32 color) {
    u8 buffer[16];
    u8 *ptr;
    s32 digitBase;
    s32 text;
    digitBase = g_numberFormat.digits[0];
    prim = drawIconClut(ot, prim, ICON_HP_YELLOW, x, y, g_menuTint[MENU_TINT_NORMAL], (color * 64) + 2);
    x += 20;
    ptr = &buffer[0];
    intToDecStringShort(value1, ptr, digitBase);
    ptr = &buffer[1];
    replaceLeadingZeros(ptr, 3, digitBase, getMenuString(11)[0]);
    ptr = &buffer[1];
    prim = func_8002C56C(ot, prim, x, y, ptr, color);
    text = func_801F6AFC(16);
    x += 33;
    prim = func_8002C56C(ot, prim, x, y, text, color);
    x += 9;
    ptr = &buffer[0];
    intToDecStringShort(value2, ptr, digitBase);
    ptr = &buffer[1];
    replaceLeadingZeros(ptr, 3, digitBase, getMenuString(11)[0]);
    ptr = &buffer[1];
    return func_8002C56C(ot, prim, x, y, ptr, color);
}

void *func_801F3CE0(s32 value1, s32 value2, void *ot, void *prim, s32 x, s32 y, u32 color) {
    prim = func_801F0FEC(ot, prim, x, y, func_801F6AFC(17), color);
    x += 62;
    prim = drawNumberMenuTint(ot, prim, (y << 0x10) | (x & 0xFFFF), value1, color);
    prim = func_801F0FEC(ot, prim, x, y, func_801F6AFC(16), color);
    x += 48;
    return drawNumberMenuTint(ot, prim, (y << 0x10) | (x & 0xFFFF), value2, color);
}

void *func_801F3DE4(s32 value1, s32 value2, void *ot, void *prim, s32 x, s32 y, u32 color) {
    u8 buffer[16];
    u8 *ptr;
    s32 digitBase;
    u8 *text;
    s32 x2;
    s32 y2;
    void *prim2;
    void *prim3;
    digitBase = g_numberFormat.digits[0];
    x2 = x;
    y2 = y;
    text = func_801F6AFC(14);
    y -= 3;
    prim2 = drawDecodedText(ot, prim, x, y, text, color);
    x += 86;
    ptr = &buffer[0];
    intToDecString(value1, ptr, digitBase);
    ptr = &buffer[2];
    replaceLeadingZeros(ptr, 7, digitBase, getMenuString(11)[0]);
    ptr = &buffer[2];
    prim = func_8002C56C(ot, prim2, x, y + 2, ptr, color);
    prim = drawIcon(ot, prim, ICON_LETTER_P, x + 64, y + 2, g_menuTint[MENU_TINT_NORMAL]);
    text = func_801F6AFC(15);
    prim3 = drawDecodedText(ot, prim, x2, y2 + 8, text, color);
    x = x2 + 86;
    ptr = &buffer[0];
    intToDecString(value2, ptr, digitBase);
    ptr = &buffer[2];
    replaceLeadingZeros(ptr, 7, digitBase, getMenuString(11)[0]);
    ptr = &buffer[2];
    prim = func_8002C56C(ot, prim3, x, y2 + 10, ptr, color);
    return drawIcon(ot, prim, ICON_LETTER_P, x2 + 150, y2 + 10, g_menuTint[MENU_TINT_NORMAL]);
}

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

static void *func_801F3FE8(MainMenuCtx *s, void *ot, void *prim, s32 arg3) {
    s32 charId = s->party[arg3];
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 y = arg3 * 26;
    s32 y2 = y + 34;
    s32 width = 232;
    s32 height = 26;
    s32 *packedWH = &cfg->w;
    if (charId != 255) {
        CharMenuInfo *charMenuInfo = &g_charMenuInfo[charId];
        CharacterData *charData = &g_gameState.chars[charId];
        u32 color = func_801F3FB4(charMenuInfo->statusFlags);
        u8 *charName = getCharName(charData->characterId);
        s32 y3 = y + 44;
        prim = func_801F3994(charName, ot, prim, 40, y3, color);
        y3 = y + 46;
        prim = func_801F3B64(charMenuInfo->currentHp, charMenuInfo->maxHp, ot, prim, 162, y3, color);
        prim = func_801F39D0(charMenuInfo->currentLevel, ot, prim, 114, y3, color);
    }
    cfg->iconType = ICON_STATUS;
    cfg->iconSubType = 0;
    cfg->x = 32;
    cfg->y = y2;

    // Stores width and height together in a single 32-bit value.
    *packedWH = height << 16 | width;

    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

void *func_801F4168(void *ot, void *prim, u8 *text, s32 x, s32 y, s32 progress, s32 iconType) {
    u32 width;
    u32 height;
    s32 x2;
    if (progress == 0) {
        return prim;
    }
    prim = func_801EF8D8(ot, prim);
    width = getTextSize(text);
    height = width >> 16;
    width &= 0xFFFF;
    width += 20;
    x2 = x - (width / 2);
    prim = func_801F0FEC(ot, prim, x2 + 10, y + 8, text, DIALOG_COLOR_WHITE);
    g_menuDisplayCfg.iconType = iconType;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x2;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = width;
    g_menuDisplayCfg.h = height + 16;
    return DrawMenuWindow(ot, prim, progress, g_menuTint[MENU_TINT_NORMAL]);
}

/** @brief Draw ability/status list line (default mode, last param = 0). */
void func_801F4274(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    func_801F4168(a0, a1, a2, a3, a4, a5, 0);
}

static void *func_801F42A4(MainMenuCtx *ctx, void *ot, void *prim, s32 partySlot) {
    u8 charId = ctx->party[partySlot];
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 yOffset = partySlot * 52;
    s32 panelY = yOffset + 34;
    if (charId != PARTY_SLOT_EMPTY) {
        s32 x;
        s32 itemY = yOffset + 44;
        CharMenuInfo *charMenuInfo = &g_charMenuInfo[charId];
        CharacterData *charData = &g_gameState.chars[charId];
        s32 color = func_801F3FB4(charMenuInfo->statusFlags);
        u8 *charName = getCharName(charData->characterId);
        prim = func_801F3994(charName, ot, prim, 40, itemY, color);
        x = 162;
        itemY = yOffset + 46;
        prim = func_801F3B64(charMenuInfo->currentHp, charMenuInfo->maxHp, ot, prim, x, itemY, color);
        prim = func_801F39D0(charMenuInfo->currentLevel, ot, prim, 114, itemY, color);
        itemY = yOffset + 59;
        x = 98;
        prim = func_801F3DE4(charMenuInfo->currentExp, charMenuInfo->expToNextLevel, ot, prim, x, itemY, DIALOG_COLOR_WHITE);
    }
    cfg->iconType = ICON_STATUS;
    cfg->iconSubType = 0;
    cfg->x = 32;
    cfg->y = panelY;
    cfg->w = 232;
    cfg->h = 50;
    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

static void *func_801F4454(MainMenuCtx *ctx, void *ot, void *prim, s32 x, s32 y, s32 index) {
    u8 buffer[16];
    CharacterData *character;
    CharMenuInfo *menuInfo;
    LINE_F2 *line;
    u8 *ptr;
    u8 *charName;
    s32 charId;
    s32 digitBase;
    u32 color;
    s32 drawX;
    s32 drawY;
    u32 clut;
    u32 barWidth;
    charId = ctx->reserve[index];
    if (charId == PARTY_SLOT_EMPTY) {
        return prim;
    }
    character = &g_gameState.chars[charId];
    digitBase = g_numberFormat.digits[0];
    menuInfo = &g_charMenuInfo[charId];
    color = func_801F3FB4(character->statusFlags);
    x += index % 2 * 120;
    y += index / 2 * 24;
    drawX = x + 9;
    drawY = y + 4;
    charName = getCharName(character->characterId);
    prim = func_801F0FEC(ot, prim, drawX, drawY, charName, color);
    clut = (color * 64) + 2;
    drawY = y + 17;
    prim = drawIconClut(ot, prim, ICON_LV_YELLOW, x + 12, drawY, g_menuTint[MENU_TINT_NORMAL], clut);
    drawX = x + 28;
    ptr = &buffer[0];
    intToDecStringShort(menuInfo->currentLevel, ptr, digitBase);
    ptr = &buffer[2];
    replaceLeadingZeros(ptr, 2, digitBase, getMenuString(11)[0]);
    prim = func_8002C56C(ot, prim, drawX, drawY, ptr, color);
    drawX = x + 59;
    prim = drawIconClut(ot, prim, ICON_HP_YELLOW, drawX, drawY, g_menuTint[MENU_TINT_NORMAL], clut);
    drawX = x + 75;
    ptr = &buffer[0];
    intToDecStringShort(menuInfo->currentHp, ptr, digitBase);
    ptr = &buffer[1];
    replaceLeadingZeros(ptr, 3, digitBase, getMenuString(11)[0]);
    prim = func_8002C56C(ot, prim, drawX, drawY, ptr, color);
    drawX = x + 59;
    drawY = y + 25;
    barWidth = menuInfo->hpRatio;
    if (barWidth != 0) {
        line = (LINE_F2 *)prim;
        setLineF2(line);
        setXY2(line, drawX, drawY, drawX + barWidth, drawY);
        *(u32 *)&line->r0 = D_801FABBC;
        addPrimFast(ot, line);
        prim += sizeof(LINE_F2);
    }
    line = (LINE_F2 *)prim;
    setLineF2(line);
    setXY2(line, drawX + barWidth, drawY, x + 107, drawY);
    *(u32 *)&line->r0 = D_801FABC0;
    addPrimFast(ot, line);
    return prim + sizeof(LINE_F2);
}

static void *func_801F4744(MainMenuCtx *s, void *ot, void *prim) {
    s32 i;
    MenuDisplayConfig *cfg;
    s32 value;
    s32 result;
    s32 x;
    s32 y;
    cfg = &g_menuDisplayCfg;
    value = s->brightness;
    value = ONE - value;
    gte_dpcs_get_rgb(result, 0x400000EC, value);
    NOP();
    D_801FABBC = result;
    gte_dpcs_get_rgb(result, 0x40000000, value);
    NOP();
    D_801FABC0 = result;
    x = 32;
    y = 112;
    for (i = 0; i < 8; i++) {
        do {
            prim = func_801F4454(s, ot, prim, x, y, i);
        } while (0);
    }
    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 232;
    cfg->h = 77;
    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Draw character command abilities panel.
 *
 * If a0[0x34] == 0, renders 3 command slots via func_801F42A4;
 * otherwise renders via func_801F3FE8 plus func_801F4744.
 */
s32 func_801F486C(u8 *a0, s32 a1, void *result) {
    s32 i;

    if (a0[0x34] == 0) {
        for (i = 0; i < 3; i++) {
            result = func_801F42A4(a0, a1, result, i);
        }
    } else {
        for (i = 0; i < 3; i++) {
            result = func_801F3FE8(a0, a1, result, i);
        }
        result = func_801F4744(a0, a1, result);
    }
    return result;
}

/**
 * @brief Draw Gil (money) display panel.
 *
 * Renders the Gil label string, sets up g_menuDisplayCfg rendering params,
 * and draws the panel decoration via DrawMenuWindow.
 */
void *func_801F4918(s32 a0, s32 a1, s32 a2) {
    s32 ret;
    ret = func_801F6358(a1, a2, 0x22, 0xC6, g_gameState.unk004);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 0xBE;
    g_menuDisplayCfg.w = 0xF4;
    g_menuDisplayCfg.h = 0x1A;
    return DrawMenuWindow(a1, ret, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/* ======================================================================== */
/* Party Status & HP Display                                                */
/* ======================================================================== */

static void *func_801F49A4(MainMenuCtx *s, void *ot, void *prim) {
    MenuDisplayConfig *cfg;
    u8 *text;
    s32 width;
    s32 panelX;

    if (s->field44 <= 0) {
        return prim;
    }

    cfg = &g_menuDisplayCfg;
    text = func_801F6AFC(42);

    width = getFirstLineWidth(text) + 20;
    cfg->w = width;

    width = (248 - width) / 2;
    panelX = width + 20;
    cfg->h = 26;

    prim = func_801F0FEC(ot, prim, width + 30, 102, text, DIALOG_COLOR_WHITE);

    cfg->iconType = ICON_NOTICE;
    cfg->iconSubType = 0;
    cfg->x = panelX;
    cfg->y = 95;

    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);   
}

static void *func_801F4A98(MainMenuCtx *s, void *ot, void *prim) {
    u32 time;
    u8 isPlayTime;
    u32 gil;
    MenuDisplayConfig *cfg;
    u32 x;
    u32 y;
    cfg = &g_menuDisplayCfg;
    if (func_801F0D84() != 1) {
        return prim;
    }
    x = 271;
    y = 178;
    func_801F1AFC();
    setMenuBrightness(s->brightness);
    prim = func_801F49A4(s, ot, prim);
    prim = func_801F38F8(s, ot, prim);
    prim = func_801F486C(s, ot, prim);
    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        gil = g_gameState.mainData.party.dreamGil;
    } else {
        gil = g_gameState.mainData.party.gil;
    }
    prim = func_801F36E8(ot, prim, x + 10, y + 16);
    prim = func_801F3824(ot, prim, x + 10, y + 27, gil);
    if (g_battleConfig.unk2 & BATTLE_FLAG_COUNTDOWN) {
        time = g_gameState.mainData.countdownTimer;
        isPlayTime = 0;
    } else {
        time = g_gameState.mainData.playTimeSeconds;
        isPlayTime = 1;
    }
    prim = drawMenuClock(s, ot, prim, x + 10, y + 5, time, isPlayTime);
    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 90;
    cfg->h = 38;
    prim = DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
    prim = func_801F4918(s, ot, prim);
    func_801F1B10();
    return prim;
}

static void *func_801F4C60(void *ot, void *prim, u32 value, s32 x, s32 y) {
    u8 buffer[16];
    u32 clut = 0x64808080;
    intToDecStringShort(value, &buffer[0], 56);
    prim = func_801F0884(ot, prim, (y << 0x10) | (x & 0xFFFF), &buffer[2], clut);
    return prim;
}

static void *func_801F4CE8(void *ot, void *prim, u32 value, s32 x, s32 y) {
    u8 buffer[16];
    u32 clut = 0x64808080;
    intToDecStringShort(value, &buffer[0], 56);
    prim = func_801F0884(ot, prim, (y << 0x10) | (x & 0xFFFF), &buffer[0], clut);
    return prim;
}

void *func_801F4D70(void *ot, void *prim, s32 x, s32 y, u32 width, u32 clut) {
    u32 color;
    DR_TWIN *tw;
    ModeSprt *p;

    // Dead code added to match with the original game binary.
    clut++; clut--;

    color = (clut & 0xFFFFFF) | SPRT_CODE;
    prim = drawIcon(ot, prim, ICON_TINY_LEFT_CAP, x, y, color);
    prim = drawIcon(ot, prim, ICON_TINY_RIGHT_CAP, x + width + 8, y, color);
    tw = prim;
    setlen(tw, 2);
    tw->code[0] = TEXWINDOW_OFF;
    tw->code[1] = 0;
    addPrimFast(ot, tw);
    p = (void *)tw + sizeof(DR_TWIN);
    setlen(p, 7);
    *(u32 *)&p->r0 = color;
    p->code = 0x64;
    p->drawMode = 0xE100041E;
    p->texWindow[0] = 0xE20207FF;
    p->texWindow[1] = 0;
    p->x0 = x + 8;
    p->y0 = y;
    p->w = width;
    p->h = 8;
    *(u32 *)&p->u0 = 0x38900000;
    addPrimFast(ot, p);
    return (void *)p + sizeof(ModeSprt);
}

void *func_801F4EA8(void *ot, LINE_F2 *prim, s32 x, s32 y, s32 arg4) {
    u8 buffer[16];
    u8 *bufferPtr;
    s32 x2;
    s32 y2;
    u32 rgbc;
    s32 value;
    arg4 = (6000 - arg4) / 5;
    arg4 = CLAMP(arg4, 0, 1000);
    intToDecStringShort(arg4, &buffer[0], 40);
    bufferPtr = &buffer[0];
    replaceLeadingZeros(bufferPtr, 4, 40, 7);
    x2 = x + 11;
    y2 = y + 3;
    while (1) {
        s32 iconId = *bufferPtr++;
        if (iconId == 0) {
            break;
        }
        if (iconId != ICON_BLANK) {
            prim = drawIcon(ot, prim, iconId, x2, y2, g_menuTint[MENU_TINT_NORMAL]);
        }
        x2 += 6;
    }
    value = ONE - g_menuBrightness;
    value = CLAMP(value, 0, ONE);
    x2 = x + 8;
    y2 = y + 2;
    gte_dpcs_get_rgb(rgbc, 0x402C2CFF, value);
    if (arg4 != 0) {
        s32 x3;
        s32 y3;
        x3 = x2 + ((arg4 * 30) / 1000);
        setLineF2(prim);
        *(s32 *)&prim->r0 = rgbc;
        setXY2(prim, x2, y2, x3, y2);
        addPrimFast(ot, prim);
        prim++;
        y3 = y + 3;
        y2 = x3;
        setLineF2(prim);
        *(s32 *)&prim->r0 = rgbc;
        setXY2(prim, x2, y3, y2, y3);
        addPrimFast(ot, prim);
        prim++;
    }
    return func_801F4D70(ot, prim, x, y, 30, g_menuTint[MENU_TINT_NORMAL]);
}

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
    CharMenuInfo *data;
    BattleCharData *btlChars;
    s32 statusFlags;
    s32 i;

    data = &g_charMenuInfo[charId];
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

static void func_801F5490(MainMenuCtx *ctx) {
    s32 i;
    s32 j;
    u32 val1 = 0;
    D_801FAB7C = func_80023C48();
    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        ctx->unk34 = 0;
    } else {
        s32 val2 = ctx->field20;
        for (i = 0; i < PARTY_SLOT_COUNT; i++) {
            s32 member = g_gameState.mainData.party.partyMembers[i];
            if (member != PARTY_SLOT_EMPTY) {
                val2 &= ~(1 << member);
            }
        }
        if (val2 != 0) {
            ctx->unk34 = 1;
        } else {
            ctx->unk34 = 0;
        }
    }
    for (i = 0; i < PARTY_SLOT_COUNT; i++) {
        s32 member = g_gameState.mainData.party.partyMembers[i];
        if (member != PARTY_SLOT_EMPTY && ((ctx->field20 >> member) & 1)) {
            val1 |= 1 << member;
            ctx->party[i] = member;
        } else {
            ctx->party[i] = PARTY_SLOT_EMPTY;
        }
    }
    if (ctx->unk34 == 0) {
        for (i = 0; i < CHARACTER_COUNT; i++) {
            ctx->reserve[i] = PARTY_SLOT_EMPTY;
        }
        ctx->unk4A = 3;
    } else {
        for (j = 0, i = 0; i < CHARACTER_COUNT; i++) {
            if (((ctx->field20 >> i) & 1) && !(val1 & (1 << i))) {
                ctx->reserve[j++] = i;
            }
        }
        ctx->unk4A = ((j + 1) / 2) + 3;
        while (j < CHARACTER_COUNT) {
            ctx->reserve[j++] = PARTY_SLOT_EMPTY;
        }
    }
    func_801F5440();
    func_801F1F98(ctx);
}

u32 func_801F565C(u32 charId, u32 otherChars) {
    s32 i;
    s32 max;
    s32 val;
    i = 0;
    while (1) {
        if (D_801FAB88[i] == PARTY_SLOT_EMPTY) {
            return charId;
        }
        if (charId == D_801FAB88[i]) {
            break;
        }
        i++;
    }
    max = i;
restart:
    do {
        i--;
        if (max == i) {
            return charId;
        }
        if (i < 0) {
            i = 8;
        }
        val = D_801FAB88[i];
        if (val != PARTY_SLOT_EMPTY && otherChars & (1 << val)) {
            return val;
        }
        goto restart;
    } while (0);
}

u32 func_801F56E4(u32 charId, u32 otherChars) {
    s32 i;
    s32 max;
    s32 val;
    i = 0;
    while (1) {
        if (D_801FAB88[i] == PARTY_SLOT_EMPTY) {
            return charId;
        }
        if (charId == D_801FAB88[i]) {
            break;
        }
        i++;
    }
    max = i++;
    while (1) {
        if (max == i) {
            return charId;
        }
        val = D_801FAB88[i];
        if (val == PARTY_SLOT_EMPTY) {
            i = 0;
            continue;
        }
        i++;
        if (otherChars & (1 << val)) {
            return val;
        }
    }
}

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
s32 func_801F57DC(s32 a0) {
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
        g_charMenuInfo[entityId].currentHp = hp;
    }
}

/** @brief Get entity max HP. */
s32 func_801F58EC(s32 entityId) {
    if (entityId < 16) {
        return g_charMenuInfo[entityId].maxHp;
    } else {
        s32 gfId = entityId - 16;
        return g_battleChars.gfs[gfId].maxHp;
    }
}

/** @brief Get entity level. */
s32 func_801F5938(s32 entityId) {
    if (entityId < 16) {
        return g_charMenuInfo[entityId].currentLevel;
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

void *func_801F5A38(void *ot, void *prim, s32 x, s32 y, u32 stringType, s16 *stringIds, u32 availableStrings) {
    s16 xOffsets[36];
    s32 i;
    s32 y2 = y;
    s32 count = func_801F5984(stringIds, xOffsets, stringType);
    y2 -= 2;
    for (i = 0; i < count; i++) {
        s32 x2 = x + xOffsets[i];
        u32 mask = 1 << i;
        u32 color = (availableStrings & mask) ? DIALOG_COLOR_WHITE : DIALOG_COLOR_GRAY;
        u8 *text = func_801F08D4(1, stringType, stringIds[i], 0);
        prim = drawDecodedText(ot, prim, x2, y2, text, color);
    }
    return prim;
}

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

void *func_801F605C(void* ot, void* prim, s32 x, s32 y, CharMenuInfo *info) {
    s32 i;
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 x2 = x;
    s32 y2 = y;
    cfg->iconType = ICON_COMMAND;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 84;
    cfg->h = 64;
    x2 += 8;
    y2 += 7;
    for (i = 0; i < 4; i++) {
        u32 cmdId = info->commandIds[i];
        u32 color = DIALOG_COLOR_WHITE;
        if (func_801F79F8(SEALED_ITEM)
                && (cmdId == BATTLE_CMD_UNK13 || cmdId == BATTLE_CMD_ITEM)) {
            color = DIALOG_COLOR_GRAY;
        }
        if (func_801F79F8(SEALED_MAGIC) && cmdId == BATTLE_CMD_MAGIC) {
            color = DIALOG_COLOR_GRAY;
        }
        if (func_801F79F8(SEALED_GF) && cmdId == BATTLE_CMD_GF) {
            color = DIALOG_COLOR_GRAY;
        }
        if (func_801F79F8(SEALED_DRAW) && cmdId == BATTLE_CMD_DRAW) {
            color = DIALOG_COLOR_GRAY;
        }
        if (func_801F79F8(SEALED_COMMAND_ABILITIES)
                && i != 0 && cmdId - 2 >= 2 && cmdId != BATTLE_CMD_DRAW
                && cmdId != BATTLE_CMD_UNK13 && cmdId != BATTLE_CMD_ITEM) {
            color = DIALOG_COLOR_GRAY;
        }
        if (cmdId != BATTLE_CMD_NONE) {
            u8 *cmdName = getBattleCommandName(cmdId);
            prim = func_801F0FEC(ot, prim, x2, y2, cmdName, color);
        }
        y2 += 13;
    }
    return DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

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

void *func_801F64A4(void *ot, void *prim, s32 x, s32 y, CharacterData *chr, CharMenuInfo *info, s32 arg6) {
    u32 color;
    u8 *chrName;
    s32 x2 = x + 40;
    s32 y2 = y + 57;
    prim = MenuDrawPortrait(chr->characterId, ot, prim, x2, y2, g_menuTint[0]);
    x2 = x + 79;
    color = func_801F3FB4(info->statusFlags);
    chrName = getCharName(chr->characterId);
    prim = func_801F3994(chrName, ot, prim, x2, y2, color);
    y2 = y + 75;
    prim = func_801F3ABC(info->currentLevel, ot, prim, x2, y2, color);
    if (arg6 != 0) {
        x2 = x + 143;
        y2 = y + 73;
        prim = func_801F6418(info->charId, ot, prim, x2, y2);
    }
    x2 = x + 79;
    y2 = y + 93;
    return func_801F3CE0(info->currentHp, info->maxHp, ot, prim, x2, y2, color);
}

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

void func_801F6888(void) {
    u8 *item;
    s32 i;
    s32 minAmmo = 255;
    s32 maxAmmo = 0;
    ShotEntry *shot = g_kernel.shot;
    for (i = 7; i >= 0; i--, shot++) {
        s32 ammoItem = shot->ammoItem;
        if (ammoItem < minAmmo) {
            minAmmo = ammoItem;
        }
        if (maxAmmo < ammoItem) {
            maxAmmo = ammoItem;
        }
    }
    item = &g_gameState.mainData.itemSlots[0].id;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        s32 id = *item++;
        s32 count = *item++;
        if (count != 0 && id >= minAmmo && maxAmmo >= id) {
            u8 newLimits = g_gameState.mainData.limitBreaks.irvineLimits;
            id = 1 << (id - minAmmo);
            newLimits |= id;
            g_gameState.mainData.limitBreaks.irvineLimits = newLimits;
        }
    }
}

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

static void *func_801F6B54(void *ot, void *prim, s32 x, s32 y, s32 charId) {
    u8 buffer[16];
    u32 color;
    u8 *chrName;
    void *prim2;
    CharacterData *chr = &g_gameState.chars[charId];
    charId = chr->characterId;
    color = func_801F3FB4(chr->statusFlags);
    chrName = getCharName(charId);
    prim2 = func_801F0FEC(ot, prim, x, y, chrName, color);
    prim = drawIconClut(ot, prim2, ICON_HP_YELLOW, x + 104, y + 2, g_menuTint[MENU_TINT_NORMAL], (color * 64) + 2);
    intToDecStringShort(chr->currentHp, buffer, getMenuString(11)[1]);
    replaceLeadingZeros(buffer, 4, getMenuString(11)[1], getMenuString(11)[0]);
    return func_8002C56C(ot, prim, x + 116, y + 2, buffer, color);
}

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

void *func_801F6D88(void *ot, void *prim, s32 x, s32 y, s32 gfId) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    if (gfId >= 0) {
        s32 i;
        u16 statusFlags = func_801F57A4(gfId);
        u32 curHp = func_801F57DC(gfId);
        u32 maxHp = func_801F58EC(gfId);
        u32 level = func_801F5938(gfId);
        u32 color = func_801F3FB4(statusFlags);
        s32 x2 = x + 8;
        s32 y2 = y + 8;
        prim = func_801F3ABC(level, ot, prim, x2, y2, color);
        x2 = x + 72;
        y2 = y + 6;
        prim = func_801F6418(gfId, ot, prim, x2, y2);
        x2 = x + 8;
        y2 = y + 23;
        prim = func_801F3CE0(curHp, maxHp, ot, prim, x2, y2, color);
        x2 = x + 82;
        y2 = y + 8;
        statusFlags &= STATUS_ICON_MASK;
        for (i = 0; i < GF_COUNT; i++) {
            if (statusFlags >> i & 1) {
                prim = drawIcon(ot, prim, i + ICON_STATUS_KO, x2, y2, g_menuTint[MENU_TINT_NORMAL]);
                x2 += 17;
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

/* ======================================================================== */
/* Config/Settings Display                                                  */
/* ======================================================================== */

/** @brief Get packed cursor coordinates (D_801FACE4 << 16 | D_801FACE2). */
s32 func_801F6F6C(void) {
    return ((u16) D_801FACE4 << 16) | (u16) D_801FACE2;
}

/** @brief Render menu item at grid position derived from hi/lo nibbles of a0. */
void func_801F6F88(s32 a0) {
    s32 hi = a0 >> 4;
    s32 lo = a0 & 0xF;
    s32 val = func_801F6F6C();

    func_801F0994(lo | hi, (u16)val, (val >> 16) + lo * 13);
}

void *func_801F6FE4(void *ot, void *prim, s32 arg2, s32 progress) {
    MenuDisplayConfig *cfg;
    s32 x;
    s32 y;
    u8 *ptr;
    s32 color;
    if (progress <= 0) {
        return prim;
    }
    prim = func_801F03E8(ot, prim);
    cfg = &g_menuDisplayCfg;
    color = DIALOG_COLOR_WHITE;
    x = D_801FACD8.x + 10;
    y = D_801FACD8.y + 7;
    ptr = D_801FACC8;
    if (ptr != NULL) {
        prim = func_801F0FEC(ot, prim, x, y, ptr, color);
    }
    x = D_801FACE2;
    y = D_801FACE4;
    ptr = D_801FACCC;
    prim = func_801F0FEC(ot, prim, x, y, ptr, color);
    y += 13;
    ptr = D_801FACD0;
    prim = func_801F0FEC(ot, prim, x, y, ptr, color);
    cfg->iconType = ICON_NOTICE;
    cfg->iconSubType = 0;
    *(RECT *)&cfg->x = D_801FACD8;
    return DrawMenuWindow(ot, prim, progress, g_menuTint[MENU_TINT_NORMAL]);
}

void func_801F7148(u8 *arg0, u8 *arg1, u8 *arg2, s32 arg3) {
    u32 h;
    u32 w;
    if (arg1 == NULL) {
        arg1 = func_801F6AFC(47);
    }
    if (arg2 == NULL) {
        arg2 = func_801F6AFC(48);
    }
    if (arg0 != NULL) {
        D_801FACC8 = arg0;
        w = getTextSize(arg0);
    } else {
        w = 0;
    }
    h = w >> 0x10;
    w &= 0xFFFF;
    D_801FACCC = arg1;
    D_801FACD0 = arg2;
    D_801FACD8.x = (364 - w) / 2;
    D_801FACD8.w = w + 20;
    D_801FACD8.y = arg3;
    D_801FACD8.h = h + 44;
    D_801FACE0 = (384 - w) / 2;
    w = getFirstLineWidth(arg1);
    w = CLAMP(w, getFirstLineWidth(arg2), w);
    D_801FACE2 = (384 - w) / 2;
    D_801FACE4 = h + 11 + D_801FACDA;
}

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
