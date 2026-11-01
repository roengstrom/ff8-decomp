#ifndef MAIN_H
#define MAIN_H

#include "common.h"
#include "cd.h"
#include "psxsdk/libgpu.h"

/* =============================================================== *
 *  Public interface — functions in main.c called by other units. *
 * =============================================================== */

/** @brief Initializes two full-screen black TILE primitives for screen clearing. */
void InitClearTiles(void);

/** @brief Copies the framebuffer from the current display buffer to the other. */
void copyFramebuffer(void);

/* =============================================================== *
 *  Private — types, globals and helpers internal to main.c.       *
 * =============================================================== */

/** @brief Fade effect mode for the rendering system. */
typedef enum {
    FADE_NONE = 0,
    FADE_IN   = 1,
    FADE_OUT  = 2
} FadeMode;

/* Display / render state owned by main.c. */
extern s16 g_curFieldId; /**< Global id of the current field map. */
extern TILE           g_clearTiles[];
extern volatile u16   g_bufferIndex; /* volatile for codegen match (forces sign extension, prevents CSE) */
extern volatile u8    g_fadeMode;    /* volatile for codegen match (forces reload each access) */
extern u32            g_orderingTablePtrs[];
extern DRAWENV       *g_activeDrawEnv;  /**< Draw env of the buffer currently being built. */
extern s32            D_8005F138;       /**< Active display-environment window (holds a @c DISPENV*). */

extern u8 g_vsyncSkip;
extern volatile s32 g_secondTicker;
extern volatile s32 g_countdownTicker;
extern u16 g_vsyncDone;

extern u8 g_soundEffectData[];
extern u8             D_8006A468[];
extern u8             D_8005F188[];
extern u8             D_80070468[];

void VsyncHandler(void);
void InitHardware(void);
void LoadFieldOverlay(void);
void LoadWorldOverlay(void);
void LoadWm2FieldTable(void);
void InitSoundEngine(void);
void LoadBootOverlay(void);
void LoadSystemFont(void);
void initFieldModule(void);
void initBattleAssets(void);
void loadFileTable(void);
void SetupDrawMode(s32 mode);
void BuildPrimList(s32 brightness);
void ProcessFade(void);

#endif /* MAIN_H */
