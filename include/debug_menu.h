#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

#include "common.h"
#include "psxsdk/libgpu.h"

#define OT_LENGTH 0x1000

// A debug text request: x and y count 8-pixel character cells.
typedef struct {
    /** 0x00 */ u32 *ot;
    /** 0x04 */ s16 x;
    /** 0x06 */ s16 y;
    /** 0x08 */ s16 color; // CLUT row above 224
    /** 0x0A */ u16 pad0A;
    /** 0x0C */ u8 *format;
} DebugMenuText;

typedef struct {
    /** 0x00000 */ DRAWENV drawEnv;
    /** 0x0005C */ DISPENV dispEnv;
    /** 0x00070 */ u32 ot[OT_LENGTH];
    /** 0x04070 */ u32 topOt;
    /** 0x04074 */ u32 bottomOt;
    /** 0x04078 */ DR_MODE drawModes[0x400];
    /** 0x07078 */ SPRT_8 sprt8[0x400];
    /** 0x0B078 */ SPRT_16 sprt16[0x200];
    /** 0x0D078 */ SPRT sprt[0x200];
    /** 0x0F878 */ POLY_G4 polyG4[0x200];
} DebugMenuBuffer;

extern DebugMenuBuffer *g_debugMenuBuffer;
extern u8 g_debugMenuCurrentPage;
extern s32 g_debugMenuDrawModeCount;
extern s32 g_debugMenuSprtCount;
extern s32 g_debugMenuPolyG4Count;
extern s32 g_debugMenuSprt8Count;
extern s32 g_debugMenuSprt16Count;

void ShowFirstPage(void);
s32 UpdateDebugMenu(void);
void func_8009A60C(void);
s32 ComputeKernelChecksum(void);
void LoadDebugFonts(void);
void InitDebugMenuBuffer(DebugMenuBuffer *buf);

#endif
