#include "common.h"
#include "input/gamepad.h"
#include "menu.h"
#include "psxsdk/libc.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "gamestate.h"
#include "kernel.h"
#include "ability.h"
#include "battle.h"
#include "ui/countdown.h"
#include "cdrom.h"
#include "imgtoc.h"
#include "memlayout.h"
#include "snd_init.h"
#include "sound.h"
#include "battle_render.h"
#include "battle_render_2.h"

typedef struct {
    /** 0x00 */ u32 *ot;
    /** 0x04 */ s16 x;
    /** 0x06 */ s16 y;
    /** 0x08 */ u8 u;
    /** 0x09 */ u8 pad09;
    /** 0x0A */ u8 v;
    /** 0x0B */ u8 pad0B;
    /** 0x0C */ s16 color;
} DebugChar;

typedef struct {
    /** 0x00 */ u32 *ot;
    /** 0x04 */ s16 x;
    /** 0x06 */ s16 y;
    /** 0x08 */ s16 u;
    /** 0x0A */ s16 v;
    /** 0x0C */ s16 w;
    /** 0x0E */ s16 h;
    /** 0x10 */ u16 clut;
    /** 0x12 */ u16 pad12;
    /** 0x14 */ s32 tpage;
} DebugSprite;

typedef struct {
    /** 0x00 */ char text[0x100];
} func_8009B170_scratch;

extern u8 g_debugFontTim[];
extern u8 g_debugFontLargeTim[];

static u8 g_debugFontLargeWidths[96] = {
    4, 5, 4, 11, 8, 12, 11, 4, 4, 4, 8, 8, 4, 8, 4, 8,
    9, 8, 9, 9, 10, 9, 9, 9, 9, 9, 5, 5, 8, 9, 8, 9,
    4, 9, 9, 9, 9, 8, 8, 9, 9, 6, 9, 10, 8, 13, 10, 9,
    9, 9, 9, 8, 9, 9, 9, 13, 9, 9, 9, 6, 11, 6, 8, 4,
    8, 9, 9, 8, 9, 9, 8, 9, 9, 5, 8, 9, 5, 13, 9, 9,
    9, 9, 8, 8, 7, 9, 9, 13, 9, 9, 9, 4, 4, 4, 4, 4
};

static void ShowSecondPage(void);
static void func_8009A664(void);
static void LoadDebugTim(u8 *a0);
static void InitDebugPrimFlags(void *prim);
static void InitTestParty(void);
static void InitTestPartyMaxed(void);
static void DrawDebugSprite(DebugSprite *arg);
static void DrawDebugSpriteWindowed(DebugSprite *arg);
static void DrawDebugSpriteTiled(DebugSprite *arg);
static void DrawDebugChar(DebugChar *arg);
static void PrintDebugTextLarge(DebugMenuText *text, s32 arg1, s32 arg2, s32 arg3);
static void DrawDebugGradientRect(u32 *ot, s32 x, s32 y, s32 w, s32 h, u8 *colors);

static void ShowSecondPage(void) {
    g_debugMenuCurrentPage = 1;
    g_debugMenuStartRow = -24;
}

void ShowFirstPage(void) {
    g_debugMenuCurrentPage = 0;
    g_debugMenuStartRow = 2;
}

#define DEBUG_TEXT(p, px, py, col, fmt) \
    (p)->ot = &g_debugMenuBuffer->topOt, (p)->x = (px), (p)->y = (py), (p)->color = (col), (p)->format = (fmt)

s32 UpdateDebugMenu(void) {
    ChocoboWorldData *chocobo = &g_gameState.chocobo;
    DebugMenuText *text;
    DebugSprite *cursor;
    s16 colors[DEBUG_MENU_OPTION_COUNT];
    s32 pressed;
    s32 result;
    s32 i;

    result = 0;
    pressed = SampleController(0);
    pressed |= SampleController(1);
    LoadDebugOptionsValues();
    if (pressed & PADR2) {
        DebugMenuText *ok;

        InitTestParty();
        ok = (DebugMenuText *)getScratchAddr(0);
        DEBUG_TEXT(ok, 32, 13, 2, "OK");
        PrintDebugText(ok);
    }
    if (pressed & PADR1) {
        DebugMenuText *ok;

        PCSave();
        ok = (DebugMenuText *)getScratchAddr(0);
        DEBUG_TEXT(ok, 32, 15, 2, "OK");
        PrintDebugText(ok);
    }
    if (pressed & PADi) {
        g_debugMenuOpenJunction = 1;
    }
    if (pressed & PADL2) {
        DebugMenuText *ok;

        InitTestPartyMaxed();
        ok = (DebugMenuText *)getScratchAddr(0);
        DEBUG_TEXT(ok, 32, 19, 2, "OK");
        PrintDebugText(ok);
    }
    if (pressed & PADL1) {
        DebugMenuText *ok;

        PCLoad();
        ok = (DebugMenuText *)getScratchAddr(0);
        DEBUG_TEXT(ok, 32, 21, 2, "OK");
        PrintDebugText(ok);
    }
    if (g_debugMenuStartBattle != 0) {
        g_debugMenuStartBattle = 0;
        result = -1;
    }
    if (pressed & PADstart) {
        result = -1;
    }
    if (pressed & PADselect) {
        result = -2;
    }
    if (pressed & PADLup) {
        g_debugMenuCursor--;
    }
    if (pressed & PADLdown) {
        g_debugMenuCursor++;
    }
    if (pressed & PADLright) {
        switch (g_debugMenuCursor) {
        case DEBUG_MENU_SCENE:
            g_battleConfig.battleSceneId++;
            break;
        case DEBUG_MENU_TIME:
            g_gameState.mainData.countdownTimer += 4;
            break;
        case DEBUG_MENU_CHOCOBO_LEVEL:
            chocobo->level++;
            break;
        case DEBUG_MENU_CHOCOBO_WAZA:
            chocobo->bokoAttack++;
            break;
        default:
            g_debugMenuOptions[g_debugMenuCursor]++;
            break;
        }
    }
    if (pressed & PADLleft) {
        switch (g_debugMenuCursor) {
        case DEBUG_MENU_SCENE:
            g_battleConfig.battleSceneId--;
            break;
        case DEBUG_MENU_TIME:
            g_gameState.mainData.countdownTimer -= 4;
            break;
        case DEBUG_MENU_CHOCOBO_LEVEL:
            chocobo->level--;
            break;
        case DEBUG_MENU_CHOCOBO_WAZA:
            chocobo->bokoAttack--;
            break;
        default:
            g_debugMenuOptions[g_debugMenuCursor]++;
            break;
        }
    }
    if (pressed & PADRup) {
        g_debugMenuOpenMainMenu = 1;
    }
    for (i = 0; i < DEBUG_MENU_OPTION_COUNT; i++) {
        colors[i] = 1;
    }
    g_battleConfig.battleSceneId &= 0x3FF;
    if (chocobo->level == 0xFF) {
        chocobo->level = 100;
    }
    if (chocobo->level > 100) {
        chocobo->level = 1;
    }
    chocobo->bokoAttack &= 3;
    if (g_debugMenuCursor == DEBUG_MENU_PINCH) {
        ShowSecondPage();
    }
    if (g_debugMenuCursor == DEBUG_MENU_PET7) {
        ShowFirstPage();
    }
    if (g_debugMenuCursor < 0) {
        g_debugMenuCursor = 0;
    }
    if (g_debugMenuCursor >= DEBUG_MENU_SQUALL_FB) {
        g_debugMenuCursor = DEBUG_MENU_SQUALL_FB;
    }
    g_debugMenuOptions[g_debugMenuCursor] &= 1;
    colors[g_debugMenuCursor] = 4;
    SetDebugOptionOnOffText(g_debugMenuCursor, g_debugMenuOptions[g_debugMenuCursor]);
    if (g_debugMenuOptions[g_debugMenuCursor] != 0) {
        switch (g_debugMenuCursor) {
        case DEBUG_MENU_RATE100:
        case DEBUG_MENU_AP10:
        case DEBUG_MENU_EXP10:
            break;
        case DEBUG_MENU_DOGOUT:
            g_gameState.mainData.partyLockFlag |= PARTY_LOCK_FLAG_10;
            break;
        case DEBUG_MENU_ODIN:
            g_gameState.mainData.partyLockFlag |= PARTY_LOCK_FLAG_02;
            break;
        case DEBUG_MENU_WITCH:
            g_gameState.mainData.partyLockFlag |= PARTY_LOCK_FLAG_20;
            break;
        case DEBUG_MENU_X_ITEM:
            g_gameState.config.sealedAbilities |= SEALED_ITEM;
            break;
        case DEBUG_MENU_X_MAGIC:
            g_gameState.config.sealedAbilities |= SEALED_MAGIC;
            break;
        case DEBUG_MENU_X_GF:
            g_gameState.config.sealedAbilities |= SEALED_GF;
            break;
        case DEBUG_MENU_X_DRAW:
            g_gameState.config.sealedAbilities |= SEALED_DRAW;
            break;
        case DEBUG_MENU_X_COMMAND:
            g_gameState.config.sealedAbilities |= SEALED_COMMAND_ABILITIES;
            break;
        case DEBUG_MENU_X_PINCH:
            g_gameState.config.sealedAbilities |= SEALED_LIMIT_BREAK;
            break;
        case DEBUG_MENU_X_RESURRECT:
            g_gameState.config.sealedAbilities |= SEALED_RESURRECTION;
            break;
        case DEBUG_MENU_RANNYU1:
            g_gameState.mainData.limitBreaks.angeloCompleted |= ANGELO_COMPLETED_FLAG_02;
            break;
        case DEBUG_MENU_RANNYU2:
            g_gameState.mainData.limitBreaks.angeloCompleted |= ANGELO_COMPLETED_FLAG_04;
            break;
        case DEBUG_MENU_RANNYU3:
            g_gameState.mainData.limitBreaks.angeloCompleted |= ANGELO_COMPLETED_FLAG_08;
            break;
        case DEBUG_MENU_PET5:
            g_gameState.mainData.limitBreaks.angeloCompleted |= ANGELO_COMPLETED_FLAG_20;
            break;
        case DEBUG_MENU_PET6:
            g_gameState.mainData.limitBreaks.angeloCompleted |= ANGELO_COMPLETED_FLAG_40;
            break;
        case DEBUG_MENU_PET7:
            g_gameState.mainData.limitBreaks.angeloCompleted |= ANGELO_COMPLETED_FLAG_80;
            break;
        }
    } else {
        switch (g_debugMenuCursor) {
        case DEBUG_MENU_RATE100:
        case DEBUG_MENU_AP10:
        case DEBUG_MENU_EXP10:
            break;
        case DEBUG_MENU_DOGOUT:
            g_gameState.mainData.partyLockFlag &= ~PARTY_LOCK_FLAG_10;
            break;
        case DEBUG_MENU_ODIN:
            g_gameState.mainData.partyLockFlag &= ~PARTY_LOCK_FLAG_02;
            break;
        case DEBUG_MENU_WITCH:
            g_gameState.mainData.partyLockFlag &= ~PARTY_LOCK_FLAG_20;
            break;
        case DEBUG_MENU_X_ITEM:
            g_gameState.config.sealedAbilities &= ~SEALED_ITEM;
            break;
        case DEBUG_MENU_X_MAGIC:
            g_gameState.config.sealedAbilities &= ~SEALED_MAGIC;
            break;
        case DEBUG_MENU_X_GF:
            g_gameState.config.sealedAbilities &= ~SEALED_GF;
            break;
        case DEBUG_MENU_X_DRAW:
            g_gameState.config.sealedAbilities &= ~SEALED_DRAW;
            break;
        case DEBUG_MENU_X_COMMAND:
            g_gameState.config.sealedAbilities &= ~SEALED_COMMAND_ABILITIES;
            break;
        case DEBUG_MENU_X_PINCH:
            g_gameState.config.sealedAbilities &= ~SEALED_LIMIT_BREAK;
            break;
        case DEBUG_MENU_X_RESURRECT:
            g_gameState.config.sealedAbilities &= ~SEALED_RESURRECTION;
            break;
        case DEBUG_MENU_RANNYU1:
            g_gameState.mainData.limitBreaks.angeloCompleted &= ~ANGELO_COMPLETED_FLAG_02;
            break;
        case DEBUG_MENU_RANNYU2:
            g_gameState.mainData.limitBreaks.angeloCompleted &= ~ANGELO_COMPLETED_FLAG_04;
            break;
        case DEBUG_MENU_RANNYU3:
            g_gameState.mainData.limitBreaks.angeloCompleted &= ~ANGELO_COMPLETED_FLAG_08;
            break;
        case DEBUG_MENU_PET5:
            g_gameState.mainData.limitBreaks.angeloCompleted &= ~ANGELO_COMPLETED_FLAG_20;
            break;
        case DEBUG_MENU_PET6:
            g_gameState.mainData.limitBreaks.angeloCompleted &= ~ANGELO_COMPLETED_FLAG_40;
            break;
        case DEBUG_MENU_PET7:
            g_gameState.mainData.limitBreaks.angeloCompleted &= ~ANGELO_COMPLETED_FLAG_80;
            break;
        }
    }
    text = (DebugMenuText *)getScratchAddr(0);
    DEBUG_TEXT(text, 29, 12, 1, "R2 INIT");
    PrintDebugText(text);
    DEBUG_TEXT(text, 29, 14, 1, "R1 PC SAVE");
    PrintDebugText(text);
    DEBUG_TEXT(text, 29, 16, 1, "L3 PAST");
    PrintDebugText(text);
    DEBUG_TEXT(text, 29, 18, 1, "L2 DEBUG");
    PrintDebugText(text);
    DEBUG_TEXT(text, 29, 20, 1, "L1 PC LOAD");
    PrintDebugText(text);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_SCENE, colors[DEBUG_MENU_SCENE], "SCENE ");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_SCENE, 1, "%3d");
    PrintDebugText(text, g_battleConfig.battleSceneId);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_TIME, colors[DEBUG_MENU_TIME], "TIME ");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_TIME, 1, "%3d");
    PrintDebugText(text, g_gameState.mainData.countdownTimer);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_RATE100, colors[DEBUG_MENU_RATE100], "RATE100");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_RATE100, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_RATE100]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_AP10, colors[DEBUG_MENU_AP10], "AP*10");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_AP10, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_AP10]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_EXP10, colors[DEBUG_MENU_EXP10], "EXP*10");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_EXP10, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_EXP10]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_DOGOUT, colors[DEBUG_MENU_DOGOUT], "DOGOUT");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_DOGOUT, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_DOGOUT]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_ODIN, colors[DEBUG_MENU_ODIN], "ODIN");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_ODIN, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_ODIN]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_FORMATION, colors[DEBUG_MENU_FORMATION], "FORMATION");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_FORMATION, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_FORMATION]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_WITCH, colors[DEBUG_MENU_WITCH], "WITCH");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_WITCH, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_WITCH]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_ITEM, colors[DEBUG_MENU_X_ITEM], "X_ITEM");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_ITEM, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_ITEM]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_MAGIC, colors[DEBUG_MENU_X_MAGIC], "X_MAGIC");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_MAGIC, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_MAGIC]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_GF, colors[DEBUG_MENU_X_GF], "X_GF");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_GF, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_GF]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_DRAW, colors[DEBUG_MENU_X_DRAW], "X_DRAW");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_DRAW, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_DRAW]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_COMMAND, colors[DEBUG_MENU_X_COMMAND], "X_COMMAND");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_COMMAND, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_COMMAND]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_PINCH, colors[DEBUG_MENU_X_PINCH], "X_PINCH");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_PINCH, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_PINCH]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_X_RESURRECT, colors[DEBUG_MENU_X_RESURRECT], "X_RESURRECT");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_X_RESURRECT, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_X_RESURRECT]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_CHOCOBO, colors[DEBUG_MENU_CHOCOBO], "CHOCOBO");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_CHOCOBO, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_CHOCOBO]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_CHOCOBO_LEVEL, colors[DEBUG_MENU_CHOCOBO_LEVEL], "CHOCOBO LEVEL");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_CHOCOBO_LEVEL, 1, "%3d");
    PrintDebugText(text, chocobo->level);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_CHOCOBO_WAZA, colors[DEBUG_MENU_CHOCOBO_WAZA], "CHOCOBO WAZA");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_CHOCOBO_WAZA, 1, "%3d");
    PrintDebugText(text, chocobo->bokoAttack);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_MOGURI, colors[DEBUG_MENU_MOGURI], "MOGURI");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_MOGURI, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_MOGURI]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_RANNYU1, colors[DEBUG_MENU_RANNYU1], "RANNYU1");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_RANNYU1, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_RANNYU1]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_RANNYU2, colors[DEBUG_MENU_RANNYU2], "RANNYU2");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_RANNYU2, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_RANNYU2]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_RANNYU3, colors[DEBUG_MENU_RANNYU3], "RANNYU3");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_RANNYU3, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_RANNYU3]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_PET5, colors[DEBUG_MENU_PET5], "PET5");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_PET5, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_PET5]);
    if (g_debugMenuCurrentPage == 0) { // This is strange
        DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_PET6, colors[DEBUG_MENU_PET6], "PET6");
        PrintDebugText(text);
        DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_PET6, 1, "%s");
        PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_PET6]);
        DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_PET7, colors[DEBUG_MENU_PET7], "PET7");
        PrintDebugText(text);
        DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_PET7, 1, "%s");
        PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_PET7]);
    }
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_PINCH, colors[DEBUG_MENU_PINCH], "PINCH");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_PINCH, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_PINCH]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_GILGAMESH, colors[DEBUG_MENU_GILGAMESH], "GILGAMESH");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_GILGAMESH, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_GILGAMESH]);
    DEBUG_TEXT(text, 3, g_debugMenuStartRow + DEBUG_MENU_SQUALL_FB, colors[DEBUG_MENU_SQUALL_FB], "SQUALL FB");
    PrintDebugText(text);
    DEBUG_TEXT(text, 17, g_debugMenuStartRow + DEBUG_MENU_SQUALL_FB, 1, "%s");
    PrintDebugText(text, g_debugMenuValueText[DEBUG_MENU_SQUALL_FB]);
    if (g_gameState.mainData.countdownTimer != 0) {
        setCountdownPosition(32, 32);
        g_battleConfig.unk2 |= BATTLE_FLAG_COUNTDOWN;
    } else {
        g_battleConfig.unk2 &= ~BATTLE_FLAG_COUNTDOWN;
    }
    cursor = (DebugSprite *)getScratchAddr(0);
    cursor->x = 0;
    cursor->u = 16;
    cursor->v = 0;
    cursor->w = 24;
    cursor->h = 16;
    g_battleConfig.sealedAbilities = g_gameState.config.sealedAbilities;
    cursor->ot = &g_debugMenuBuffer->topOt;
    cursor->y = (g_debugMenuStartRow + g_debugMenuCursor) * 8;
    cursor->clut = GetClut(16, 232);
    cursor->tpage = GetTPage(0, 0, 768, 256);
    DrawDebugSprite(cursor);
    return result;
}

static void InitTestParty(void) {
    s32 id;
    s32 i;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i].exists |= CHAR_FLAG_PRESENT | CHAR_FLAG_UNK08;
    }
    g_gameState.mainData.party.partyMembers[0] = CHAR_SQUALL;
    g_gameState.mainData.party.partyMembers[1] = CHAR_ZELL;
    g_gameState.mainData.party.partyMembers[2] = CHAR_IRVINE;
    g_gameState.mainData.party.partyMembers[3] = PARTY_SLOT_EMPTY;
    g_gameState.gfs[0].exists |= GF_EXISTS;
    g_gameState.gfs[1].exists |= GF_EXISTS;
    g_gameState.gfs[2].exists |= GF_EXISTS;
    id = 1;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        g_gameState.mainData.itemSlots[i].id = id++;
        g_gameState.mainData.itemSlots[i].count = 10;
    }
}

static void InitTestPartyMaxed(void) {
    ChocoboWorldData *chocobo = &g_gameState.chocobo;
    s32 id;
    s32 i;
    s32 j;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i].exists |= CHAR_FLAG_PRESENT | CHAR_FLAG_UNK08;
    }
    g_gameState.chars[CHAR_SQUALL].weaponId = WEAPON_LION_HEART;
    g_gameState.chars[CHAR_ZELL].weaponId = WEAPON_EHRGEIZ;
    g_gameState.chars[CHAR_IRVINE].weaponId = WEAPON_EXETER;
    g_gameState.chars[CHAR_QUISTIS].weaponId = WEAPON_SAVE_THE_QUEEN;
    g_gameState.chars[CHAR_RINOA].weaponId = WEAPON_SHOOTING_STAR;
    g_gameState.chars[CHAR_SELPHIE].weaponId = WEAPON_STRANGE_VISION;
    for (i = 0; i < GF_COUNT; i++) {
        g_gameState.gfs[i].exists |= GF_EXISTS;
    }
    for (i = 0; i < GF_COUNT; i++) {
        g_gameState.gfs[i].completeAbilities[0] = -2;
        g_gameState.gfs[i].completeAbilities[1] = -1;
        g_gameState.gfs[i].completeAbilities[2] = -1;
        g_gameState.gfs[i].completeAbilities[3] = 0x3FFFFFF;
    }
    g_gameState.mainData.party.partyMembers[0] = CHAR_ZELL;
    g_gameState.mainData.party.partyMembers[1] = CHAR_SQUALL;
    g_gameState.mainData.party.partyMembers[2] = CHAR_SELPHIE;
    g_gameState.mainData.party.partyMembers[3] = PARTY_SLOT_EMPTY;
    g_gameState.mainData.limitBreaks.quistisLimits = 0xFFFF;
    chocobo->level = 10;
    chocobo->bokoAttack = 3;
    chocobo->flags |= CHOCOBO_FLAG_ENABLED | CHOCOBO_FLAG_MINIMOG_FOUND | CHOCOBO_FLAG_DEMON_KING_DEFEATED;
    chocobo->flags &= ~CHOCOBO_FLAG_IN_WORLD;
    for (j = 0; j < CHARACTER_COUNT; j += 2) {
        id = 1;
        for (i = 0; i < 30; i++) {
            g_gameState.chars[j].magic[i].magicId = id++;
            g_gameState.chars[j].magic[i].quantity = 50;
        }
        id = 31;
        for (i = 0; i < 20; i++) {
            g_gameState.chars[j + 1].magic[i].magicId = id++;
            g_gameState.chars[j + 1].magic[i].quantity = 50;
        }
    }
    g_gameState.chars[CHAR_SQUALL].commands[0] = ABILITY_MAGIC;
    g_gameState.chars[CHAR_SQUALL].commands[1] = ABILITY_DRAW;
    g_gameState.chars[CHAR_SQUALL].commands[2] = ABILITY_ITEM;
    g_gameState.chars[CHAR_ZELL].commands[0] = ABILITY_MAGIC;
    g_gameState.chars[CHAR_ZELL].commands[1] = ABILITY_DRAW;
    g_gameState.chars[CHAR_ZELL].commands[2] = ABILITY_ITEM;
    g_gameState.chars[CHAR_RINOA].commands[0] = ABILITY_MAGIC;
    g_gameState.chars[CHAR_RINOA].commands[1] = ABILITY_GF;
    g_gameState.chars[CHAR_RINOA].commands[2] = ABILITY_ITEM;
    id = 1;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        g_gameState.mainData.itemSlots[i].id = id++;
        g_gameState.mainData.itemSlots[i].count = 10;
    }
}

void func_8009A60C(void) {
    RunMenu(MENU_PARTY_JUNCTION, JUNCTION_EXCHANGE_LAGUNA | JUNCTION_EXCHANGE_KIROS | JUNCTION_EXCHANGE_WARD);
}

s32 ComputeKernelChecksum(void) {
    s32 sum;
    u8 *p;
    u32 i;

    sum = 0;
    p = (u8 *)&g_kernel;
    for (i = 0; i < KERNEL_BUFFER_SIZE; i++) {
        sum += *p++;
    }
    return sum;
}

static void func_8009A664(void) {
}

static void LoadDebugTim(u8 *a0) {
    s32 buf[6];
    OpenTIM(a0);
    ReadTIM(&buf);
    LoadImage(buf[3], buf[4]);
    if (((u32)buf[0] >> 3) & 1) {
        LoadImage(buf[1], buf[2]);
    }
}

void LoadDebugFonts(void) {
    LoadDebugTim(g_debugFontLargeTim);
    LoadDebugTim(g_debugFontTim);
}

static void InitDebugPrimFlags(void *prim) {
    SetSemiTrans(prim, 0);
    SetShadeTex(prim, 1);
}

void InitDebugMenuBuffer(DebugMenuBuffer *buf) {
    SPRT_16 *sprt16;
    SPRT_8 *sprt8;
    SPRT *sprt;
    POLY_G4 *poly;
    s32 i;

    sprt16 = buf->sprt16;
    for (i = 0; i < 0x200; i++, sprt16++) {
        setSprt16(sprt16);
        InitDebugPrimFlags(sprt16);
        setRGB0(sprt16, 0x80, 0x80, 0x80);
    }
    sprt8 = buf->sprt8;
    for (i = 0; i < 0x400; i++, sprt8++) {
        setSprt8(sprt8);
        InitDebugPrimFlags(sprt8);
        setRGB0(sprt8, 0x80, 0x80, 0x80);
    }
    sprt = buf->sprt;
    for (i = 0; i < 0x200; i++, sprt++) {
        setSprt(sprt);
        InitDebugPrimFlags(sprt);
        setRGB0(sprt, 0x80, 0x80, 0x80);
    }
    poly = buf->polyG4;
    for (i = 0; i < 0x200; i++, poly++) {
        setPolyG4(poly);
        InitDebugPrimFlags(poly);
        setRGB0(poly, 0x80, 0x80, 0x80);
    }
}

static void DrawDebugSprite(DebugSprite *arg) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    SPRT *sprt = &buf->sprt[g_debugMenuSprtCount++];

    SetDrawMode(mode, 0, 0, arg->tpage, NULL);
    sprt->clut = arg->clut;
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->w = arg->w;
    sprt->h = arg->h;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

/**
 * Unused. A copy of DrawDebugSprite that also sets a texture window before the
 * sprite and resets it after. The window here is 255x255, which the GPU treats as
 * no window, so it draws the same as DrawDebugSprite.
 */
static void DrawDebugSpriteWindowed(DebugSprite *arg) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    DR_MODE *restore = &buf->drawModes[g_debugMenuDrawModeCount++];
    SPRT *sprt = &buf->sprt[g_debugMenuSprtCount++];
    RECT window;
    RECT full;

    window.x = window.y = 0;
    window.w = window.h = 255;
    full.x = full.y = 0;
    full.w = full.h = 255;
    SetDrawMode(mode, 0, 0, arg->tpage, &window);
    SetDrawMode(restore, 0, 0, arg->tpage, &full);
    sprt->clut = arg->clut;
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->w = arg->w;
    sprt->h = arg->h;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, restore);
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

/**
 * Unused. A copy of DrawDebugSprite that sets a 32x32 texture window before the
 * sprite and resets it after. The sprite repeats the 32x32 block at the top left
 * of the texture page across its whole size.
 */
static void DrawDebugSpriteTiled(DebugSprite *arg) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    DR_MODE *restore = &buf->drawModes[g_debugMenuDrawModeCount++];
    SPRT *sprt = &buf->sprt[g_debugMenuSprtCount++];
    RECT window;
    RECT full;

    window.x = window.y = 0;
    window.w = window.h = 32;
    full.x = full.y = 0;
    full.w = full.h = 255;
    SetDrawMode(mode, 0, 0, arg->tpage, &window);
    SetDrawMode(restore, 0, 0, arg->tpage, &full);
    sprt->clut = arg->clut;
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->w = arg->w;
    sprt->h = arg->h;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, restore);
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

static void DrawDebugChar(DebugChar *arg) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    SPRT_8 *sprt = &buf->sprt8[g_debugMenuSprt8Count++];

    SetDrawMode(mode, 0, 0, getTPage(0, 0, 768, 256), NULL);
    sprt->clut = GetClut(16, arg->color + 224);
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

void PrintDebugText(DebugMenuText *text, s32 arg1, s32 arg2, s32 arg3) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    SPRT_8 *sprt = &buf->sprt8[g_debugMenuSprt8Count];
    u8 *str = (u8 *)getScratchAddr(64);
    s32 col;
    u32 c;

    SetDrawMode(mode, 0, 0, getTPage(0, 0, 768, 256), NULL);
    sprintf((char *)str, text->format, arg1, arg2, arg3);
    for (col = 0; str[col] != '\0'; col++) {
        c = str[col];
        if (c != ' ') {
            sprt->x0 = (text->x + col) * 8;
            sprt->y0 = text->y * 8;
            sprt->u0 = (c & 0xF) * 8;
            sprt->v0 = (c >> 4) * 8;
            sprt->clut = GetClut(16, text->color + 224);
            addPrim(text->ot, sprt);
            g_debugMenuSprt8Count++;
            sprt++;
        }
    }
    addPrim(text->ot, mode);
}

/**
 * Unused. Like PrintDebugText, but draws 16x16 characters from g_debugFontLargeTim
 * and moves each character on by its own width. It takes up to three values for
 * the format string and does not handle line breaks.
 */
static void PrintDebugTextLarge(DebugMenuText *text, s32 arg1, s32 arg2, s32 arg3) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    SPRT_16 *sprt = &buf->sprt16[g_debugMenuSprt16Count];
    s32 col = 0;
    s32 px;
    u8 c;

    SetDrawMode(mode, 0, 0, getTPage(0, 0, 832, 256), NULL);
    sprintf(((func_8009B170_scratch *)getScratchAddr(64))->text, text->format, arg1, arg2, arg3);
    px = 0;
    while ((c = ((func_8009B170_scratch *)getScratchAddr(64))->text[col]) != '\0') {
        if (c != ' ') {
            sprt->clut = GetClut(0, text->color + 224);
            setXY0(sprt, text->x * 8 + px, text->y * 8);
            setUV0(sprt, (c % 16) * 16, c / 16 * 16);
            addPrim(text->ot, sprt);
            g_debugMenuSprt16Count++;
            sprt++;
        }
        col++;
        px += g_debugFontLargeWidths[c - ' '];
    }
    addPrim(text->ot, mode);
}

/**
 * Unused. Draws a grey rectangle that fades from colors[0] at the top to
 * colors[1] at the bottom. It is blended additively, so it brightens what is
 * under it and never darkens it.
 */
static void DrawDebugGradientRect(u32 *ot, s32 x, s32 y, s32 w, s32 h, u8 *colors) {
    DebugMenuBuffer *buf = g_debugMenuBuffer;
    DR_MODE *mode = &buf->drawModes[g_debugMenuDrawModeCount++];
    POLY_G4 *poly = &buf->polyG4[g_debugMenuPolyG4Count++];

    SetDrawMode(mode, 1, 0, getTPage(0, BLEND_ADD, 0, 0), NULL);
    SetSemiTrans(poly, 1);
    poly->r0 = poly->g0 = poly->b0 = poly->r1 = poly->g1 = poly->b1 = *colors++;
    poly->r2 = poly->g2 = poly->b2 = poly->r3 = poly->g3 = poly->b3 = *colors;
    poly->x0 = x;
    poly->y0 = y;
    poly->x1 = x + w;
    poly->y1 = y;
    y += h;
    poly->x2 = x;
    poly->y2 = y;
    poly->x3 = x + w;
    poly->y3 = y;
    addPrim(ot, poly);
    addPrim(ot, mode);
}
