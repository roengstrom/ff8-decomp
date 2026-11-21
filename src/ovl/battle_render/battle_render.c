#include "common.h"
#include "input/gamepad.h"
#include "menu.h"
#include "psxsdk/libc.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "gamestate.h"
#include "kernel.h"
#include "battle.h"
#include "ui/countdown.h"
#include "cdrom.h"
#include "imgtoc.h"
#include "memlayout.h"
#include "snd_init.h"
#include "sound.h"
#include "battle_render.h"
#include "battle_render_2.h"

extern DebugMenuBuffer g_debugMenuBuffers[2];
extern s32 g_debugMenuVSyncDelta;
extern s32 g_debugMenuRepeatDelay[];
extern s32 g_debugMenuRepeatTimer[];
extern s32 g_kernelChecksum;

// D_8009B53C is unused, but best guess is that they are predefined values passed as color to DrawDebugGradientRect
static u8 D_8009B53C[16] = { 0x00, 0x80, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0xFF, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00 };
static s32 g_debugMenuPrevButtons[2] = { 0, 0 };

static void PrintBuildInfo(void);
static void SetBlackBackground(DRAWENV *env);
static void func_80098790(void);
static void SetDebugOptionValueText(s32 row, u8 *src);

void RunDebugMenu(void) {
    s32 result;
    s32 menuResult;
    s32 i;
    RECT rect;

    while (g_renderMode != 0);
    g_debugMenuStartBattle = 0;
restart:
    result = 0;
    D_8005F170 = 1;
    g_debugMenuCursor = 0;
    g_battleConfig.unk9 = 0;
    g_battleConfig.unk2 = 0;
    g_battleConfig.unkC = 1234;
    ShowFirstPage();
    g_debugMenuStartRow = 2;
    InitGeom();
    SetGraphDebug(0);
    SetGeomOffset(160, 112);
    SetGeomScreen(512);
    SetDefDrawEnv(&g_debugMenuBuffers[0].drawEnv, 0, 0, 320, 224);
    SetDefDrawEnv(&g_debugMenuBuffers[1].drawEnv, 320, 0, 320, 224);
    SetDefDispEnv(&g_debugMenuBuffers[0].dispEnv, 320, 0, 320, 224);
    SetDefDispEnv(&g_debugMenuBuffers[1].dispEnv, 0, 0, 320, 224);
    g_debugMenuBuffers[0].dispEnv.screen.y = g_debugMenuBuffers[1].dispEnv.screen.y = 8;
    g_debugMenuBuffers[0].dispEnv.screen.h = g_debugMenuBuffers[1].dispEnv.screen.h = 224;
    SetBlackBackground(&g_debugMenuBuffers[0].drawEnv);
    SetBlackBackground(&g_debugMenuBuffers[1].drawEnv);
    setRECT(&rect, 0, 0, 640, 224);
    ClearImage(&rect, 0, 0, 0);
    InitDebugMenuBuffer(&g_debugMenuBuffers[0]);
    InitDebugMenuBuffer(&g_debugMenuBuffers[1]);
    LoadDebugFonts();
    g_kernelChecksum = ComputeKernelChecksum();
    for (i = 0; i < 3; i++) {
        if (g_battleConfig.obtainedGf[i] != GF_NONE) {
            RunMenu(g_battleConfig.obtainedGf[i] + MENU_NAME_GF);
        }
    }
    for (i = 0; i < 3; i++) {
        g_battleConfig.obtainedGf[i] = GF_NONE;
    }
    DrawSync(0);
    VSync(0);
    SetDispMask(1);
    while (result == 0) {
        g_debugMenuPolyG4Count = g_debugMenuSprt16Count = g_debugMenuSprt8Count = g_debugMenuSprtCount = g_debugMenuDrawModeCount = 0;
        g_debugMenuBuffer = (g_debugMenuBuffer == &g_debugMenuBuffers[0]) ? &g_debugMenuBuffers[1] : &g_debugMenuBuffers[0];
        ClearOTagR(g_debugMenuBuffer->ot, OT_LENGTH);
        ClearOTagR(&g_debugMenuBuffer->topOt, 1);
        ClearOTagR(&g_debugMenuBuffer->bottomOt, 1);
        g_debugMenuOpenMainMenu = 0;
        g_debugMenuOpenJunction = 0;
        result = UpdateDebugMenu();
        if (g_debugMenuOpenMainMenu != 0) {
            menuResult = RunMenu(MENU_MAIN, 0xFFFF);
            g_debugMenuOpenMainMenu = 0;
            if (menuResult & MENU_RESULT_START_BATTLE) {
                g_battleConfig.battleSceneId = menuResult >> BATTLE_SCENE_ID_SHIFT;
                g_debugMenuStartBattle = 1;
            }
            goto restart;
        }
        if (g_debugMenuOpenJunction != 0) {
            func_8009A60C();
            result = 0;
            g_debugMenuOpenJunction = 0;
            goto restart;
        }
        PrintBuildInfo();
        DrawSync(0);
        g_debugMenuVSyncDelta = VSync(0);
        PutDispEnv(&g_debugMenuBuffer->dispEnv);
        PutDrawEnv(&g_debugMenuBuffer->drawEnv);
        g_activeDrawEnv = &g_debugMenuBuffer->drawEnv;
        g_activeDispEnv = &g_debugMenuBuffer->dispEnv;
        DrawOTag(&g_debugMenuBuffer->bottomOt);
        DrawOTag(&g_debugMenuBuffer->ot[OT_LENGTH - 1]);
        DrawOTag(&g_debugMenuBuffer->topOt);
    }
    i = g_battleConfig.battleSceneId & 3;
    cdReadSync(D_800974F0[i * 4], D_800974F0[i * 4 + 1], (u8 *)LOAD_BUFFER_ADDR, NULL);
    sndCmd11(0);
    sndProcessAudio(LOAD_BUFFER_ADDR, 1);
    cdReadSync(D_800974F0[i * 4 + 2], D_800974F0[i * 4 + 3], D_8005F388, NULL);
    D_8005F11C = sndCmd10((s32)D_8005F388);
    sndCmdC0(D_8005F11C, 0x7F);
    if (result == -1) {
        g_gameMode = GAME_MODE_BATTLE;
    } else {
        g_gameMode = GAME_MODE_CARD;
    }
}

static void PrintBuildInfo(void) {
    DebugMenuText *text = (DebugMenuText *)getScratchAddr(0);

    text->x = 26;
    text->y = 22;
    text->color = 1;
    text->ot = &g_debugMenuBuffer->topOt;
    text->format = "Jul  4 1999"; // FIXME: Should probably be provided by the build system
    PrintDebugText(text);

    text->x = 29;
    text->y = 23;
    text->color = 1;
    text->ot = &g_debugMenuBuffer->topOt;
    text->format = "21:03:43"; // FIXME: Should probably be provided by the build system
    PrintDebugText(text);

    text->x = 26;
    text->y = 26;
    text->ot = &g_debugMenuBuffer->topOt;
    text->color = 1;
    text->format = " %x";
    PrintDebugText(text, g_kernelChecksum);
}

static void SetBlackBackground(DRAWENV *env) {
    env->isbg = 1;
    setRGB0(env, 0, 0, 0);
}

void PCSave(void) {
    // This originally probably had code that sent debug config to a PC and stored it there
    // but was not included in the retail build.
}

void PCLoad(void) {
    // Likely originally fetched data from a pc on the network
    // and populated a bunch of fields for testing.
}

static void func_80098790(void) {
    // Unused
}

s32 SampleController(s32 padIndex) {
    s32 prevArr[2];
    s32 currArr[2];
    s32 prevVal;
    s32 v;

    RecordGamePadInput();
    v = GetPressedButtons(padIndex, 0);
    currArr[padIndex] = v;
    prevVal = g_debugMenuPrevButtons[padIndex];

    if (prevVal == v) {
        if (g_debugMenuRepeatDelay[padIndex] == 0) {
            if (g_debugMenuRepeatTimer[padIndex] == 0) {
                prevArr[padIndex] = prevVal;
                if (currArr[padIndex] & PADRdown) {
                    g_debugMenuRepeatTimer[padIndex] = 0;
                } else {
                    g_debugMenuRepeatTimer[padIndex] = 2;
                }
            } else {
                g_debugMenuRepeatTimer[padIndex] = g_debugMenuRepeatTimer[padIndex] - 1;
                prevArr[padIndex] = 0;
            }
        } else {
            g_debugMenuRepeatDelay[padIndex] = g_debugMenuRepeatDelay[padIndex] - 1;
            prevArr[padIndex] = 0;
        }
    } else {
        g_debugMenuRepeatDelay[padIndex] = 8;
        if (currArr[padIndex] & PADRdown) {
            g_debugMenuRepeatTimer[padIndex] = 0;
        } else {
            g_debugMenuRepeatTimer[padIndex] = 2;
        }
        prevArr[padIndex] = currArr[padIndex];
    }
    g_debugMenuPrevButtons[padIndex] = currArr[padIndex];
    return prevArr[padIndex];
}

static void SetDebugOptionValueText(s32 row, u8 *src) {
    u8 *dst = g_debugMenuValueText[row];

    while ((*dst++ = *src++) != 0);
}

void SetDebugOptionOnOffText(s32 row, s32 enabled) {
    if (enabled == 0) {
        SetDebugOptionValueText(row, "OFF");
    } else {
        SetDebugOptionValueText(row, " ON");
    }
}

void LoadDebugOptionsValues(void) {
    s32 i;

    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_FLAG_10) {
        g_debugMenuOptions[DEBUG_MENU_DOGOUT] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_DOGOUT] = 0;
    }

    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_FLAG_02) {
        g_debugMenuOptions[DEBUG_MENU_ODIN] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_ODIN] = 0;
    }

    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_FLAG_20) {
        g_debugMenuOptions[DEBUG_MENU_WITCH] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_WITCH] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_ITEM) {
        g_debugMenuOptions[DEBUG_MENU_X_ITEM] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_ITEM] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_MAGIC) {
        g_debugMenuOptions[DEBUG_MENU_X_MAGIC] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_MAGIC] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_GF) {
        g_debugMenuOptions[DEBUG_MENU_X_GF] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_GF] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_DRAW) {
        g_debugMenuOptions[DEBUG_MENU_X_DRAW] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_DRAW] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_COMMAND_ABILITIES) {
        g_debugMenuOptions[DEBUG_MENU_X_COMMAND] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_COMMAND] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_LIMIT_BREAK) {
        g_debugMenuOptions[DEBUG_MENU_X_PINCH] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_PINCH] = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_RESURRECTION) {
        g_debugMenuOptions[DEBUG_MENU_X_RESURRECT] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_X_RESURRECT] = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & ANGELO_COMPLETED_FLAG_02) {
        g_debugMenuOptions[DEBUG_MENU_RANNYU1] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_RANNYU1] = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & ANGELO_COMPLETED_FLAG_04) {
        g_debugMenuOptions[DEBUG_MENU_RANNYU2] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_RANNYU2] = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & ANGELO_COMPLETED_FLAG_08) {
        g_debugMenuOptions[DEBUG_MENU_RANNYU3] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_RANNYU3] = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & ANGELO_COMPLETED_FLAG_20) {
        g_debugMenuOptions[DEBUG_MENU_PET5] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_PET5] = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & ANGELO_COMPLETED_FLAG_40) {
        g_debugMenuOptions[DEBUG_MENU_PET6] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_PET6] = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & ANGELO_COMPLETED_FLAG_80) {
        g_debugMenuOptions[DEBUG_MENU_PET7] = 1;
    } else {
        g_debugMenuOptions[DEBUG_MENU_PET7] = 0;
    }

    for (i = 0; i < DEBUG_MENU_OPTION_COUNT; i++) {
        SetDebugOptionOnOffText(i, g_debugMenuOptions[i]);
    }
}
