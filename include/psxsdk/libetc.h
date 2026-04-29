#ifndef LIBETC_H
#define LIBETC_H

#include "common.h"

/* --- Scratch pad (0x1F800000 - 0x1F800400) --- */
#define getScratchAddr(offset)  ((u32 *)(0x1F800000 + (offset) * 4))

/* --- Video mode --- */
#define MODE_NTSC 0
#define MODE_PAL  1

/* --- Pad / callback / vsync --- */
extern s32 VSync(s32 mode);
extern s32 VSyncCallback(void (*f)(void));
extern s32 ResetCallback(void);
extern s32 StopCallback(void);
extern s32 RestartCallback(void);
extern s32 CheckCallback(void);
extern void PadInit(s32 mode);
extern void PadStop(void);
extern u32 PadRead(s32 id);
extern s32 GetVideoMode(void);
extern s32 SetVideoMode(s32 mode);

#endif /* LIBETC_H */
