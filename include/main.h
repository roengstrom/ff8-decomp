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

void SetupDrawMode(s32 mode);

/* =============================================================== *
 *  Private — types, globals and helpers internal to main.c.       *
 * =============================================================== */

/** @brief Fade effect mode for the rendering system. */
typedef enum {
    FADE_NONE = 0,
    FADE_IN   = 1,
    FADE_OUT  = 2
} FadeMode;

typedef struct {
    /** 0x00 */ DR_MODE mode;
    /** 0x0C */ TILE tile;
    /** 0x1C */ u32 unk1C;
} ClearTile;

/* Display / render state owned by main.c. */
extern s16 g_curFieldId; /**< Global id of the current field map. */
extern ClearTile g_clearTiles[2];
extern volatile s16 g_bufferIndex; // volatile for codegen match (prevents CSE)
extern volatile u8    g_fadeMode;    /* volatile for codegen match (forces reload each access) */
extern u32            g_orderingTablePtrs[];
extern DRAWENV       *g_activeDrawEnv;  /**< Draw env of the buffer currently being built. */
extern s32            D_8005F138;       /**< Active display-environment window (holds a @c DISPENV*). */

extern u8 g_vsyncSkip;
extern volatile s32 g_secondTicker;
extern volatile s32 g_countdownTicker;
extern u16 g_vsyncDone;

extern u8 g_soundEffectData[];
#define PACKET_BUFFER_SIZE 0x6000
extern u8 g_packetBuffer[PACKET_BUFFER_SIZE];
#define CHOCOBO_WORLD_MUSIC_SIZE 0x200
extern u8 g_chocoboWorldMusic[CHOCOBO_WORLD_MUSIC_SIZE];

#endif /* MAIN_H */
