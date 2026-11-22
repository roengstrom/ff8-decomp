#ifndef DEBUG_H
#define DEBUG_H

#include "common.h"

typedef enum {
    DEBUG_MENU_SCENE = 0,
    DEBUG_MENU_TIME = 1,
    DEBUG_MENU_RATE100 = 2,
    DEBUG_MENU_AP10 = 3,
    DEBUG_MENU_EXP10 = 4,
    DEBUG_MENU_DOGOUT = 5,
    DEBUG_MENU_ODIN = 6,
    DEBUG_MENU_FORMATION = 7,
    DEBUG_MENU_WITCH = 8,
    DEBUG_MENU_X_ITEM = 9,
    DEBUG_MENU_X_MAGIC = 10,
    DEBUG_MENU_X_GF = 11,
    DEBUG_MENU_X_DRAW = 12,
    DEBUG_MENU_X_COMMAND = 13,
    DEBUG_MENU_X_PINCH = 14,
    DEBUG_MENU_X_RESURRECT = 15,
    DEBUG_MENU_CHOCOBO = 16,
    DEBUG_MENU_CHOCOBO_LEVEL = 17,
    DEBUG_MENU_CHOCOBO_WAZA = 18,
    DEBUG_MENU_MOGURI = 19,
    DEBUG_MENU_RANNYU1 = 20,
    DEBUG_MENU_RANNYU2 = 21,
    DEBUG_MENU_RANNYU3 = 22,
    DEBUG_MENU_PET5 = 23,
    DEBUG_MENU_PET6 = 24,
    DEBUG_MENU_PET7 = 25,
    DEBUG_MENU_PINCH = 26,
    DEBUG_MENU_GILGAMESH = 27,
    DEBUG_MENU_SQUALL_FB = 28,
    DEBUG_MENU_OPTION_COUNT = 29
} DebugMenuOption;

extern u8 g_debugMenuValueText[][10];
extern s32 g_debugMenuOpenJunction;
extern u8 g_debugMenuOptions[];
extern u8 g_debugMenuStartBattle;
extern s32 g_debugMenuOpenMainMenu;
extern s16 g_debugMenuStartRow;
extern s32 g_debugMenuCursor;

void RunDebugMenu(void);
void PCSave(void);
void PCLoad(void);
s32 SampleController(s32 padIndex);
void SetDebugOptionOnOffText(s32 row, s32 enabled);
void LoadDebugOptionsValues(void);

#endif
