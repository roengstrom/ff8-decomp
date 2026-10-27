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

/** @brief VSync rendering dispatch mode. */
typedef enum {
    RENDER_IDLE    = 0,
    RENDER_FADE    = 1,
    RENDER_BATTLE  = 2,
    RENDER_OVERLAY = 3,
    RENDER_GAME    = 4
} RenderMode;

/* Display / render state owned by main.c. */
extern s16 g_curFieldId; /**< Global id of the current field map. */
extern TILE           g_clearTiles[];
extern volatile u16   g_bufferIndex; /* volatile for codegen match (forces sign extension, prevents CSE) */
extern volatile u8    g_fadeMode;    /* volatile for codegen match (forces reload each access) */
extern u32            g_orderingTablePtrs[];
extern DRAWENV       *g_activeDrawEnv;  /**< Draw env of the buffer currently being built. */
extern s32            D_8005F138;       /**< Active display-environment window (holds a @c DISPENV*). */

/* VSync callback state owned by main.c. */
extern u8             g_vsyncSkip;
extern volatile s32   D_8005F154; /**< VSync frame-counter timing accumulator (+0x88F/VSync). */
extern volatile s32   D_8005F15C; /**< VSync countdown-timer timing accumulator (+0x88F/VSync). */
extern u16            D_8005F11E; /**< VSync-done / status flag. */

/* CD file-table descriptors + scratch buffers loaded/managed by main.c. */
extern CdFileDesc     g_fileTableDesc[];
extern CdFileDesc     D_80097400[];
extern CdFileDesc     D_80097410[];
extern CdFileDesc     D_800974D0[];
extern CdFileDesc     D_800974D8[];
extern CdFileDesc     g_kernelFileDesc;
extern u32 D_800974F0[];
extern CdFileDesc     D_80097808[];
extern u8             D_80067468[];
extern u8             D_8006A468[];
extern u8             D_8005F188[];
extern u8             D_80070468[];

void flushGpuOt(void);
void VsyncHandler(void);
void InitHardware(void);
void func_80011870(void);
void RestoreSnapshot(void);
void LoadFieldOverlay(void);
void LoadWorldOverlay(void);
void LoadWm2FieldTable(void);
void initSoundEngine(void);
void loadCdOverlay(void);
void loadAndUploadTextures(void);
void initFieldModule(void);
void initBattleAssets(void);
void loadFileTable(void);
void SetupDrawMode(s32 mode);
void BuildPrimList(s32 brightness);
void ProcessFade(void);

#endif /* MAIN_H */
