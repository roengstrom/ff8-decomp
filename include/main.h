#ifndef MAIN_H
#define MAIN_H

#include "common.h"
#include "psxsdk/libgpu.h"

void InitClearTiles(void);

void CopyFrameBuffer(void);

void SetFadeBlendMode(BlendMode blendMode);

typedef struct {
    /** 0x00 */ DR_MODE mode;
    /** 0x0C */ TILE tile;
    /** 0x1C */ u32 unk1C; // Unused
} ClearTile;

extern s16 g_curFieldId;
extern ClearTile g_clearTiles[2];
extern volatile s16 g_bufferIndex;
extern volatile u8 g_fadeMode;
extern u32 g_orderingTablePtrs[];

extern volatile s32 g_secondTicker;
extern u16 g_vsyncDone;

#endif /* MAIN_H */
