#ifndef LIBETC_H
#define LIBETC_H

#include "common.h"

/* --- Scratch pad (0x1F800000 - 0x1F800400) --- */
#define getScratchAddr(offset)  ((u32 *)(0x1F800000 + (offset) * 4))

/* --- Video mode --- */
#define MODE_NTSC 0
#define MODE_PAL  1

/* --- PAD I/O (SIO Pad) --- */
#define PADLup     (1<<12)
#define PADLdown   (1<<14)
#define PADLleft   (1<<15)
#define PADLright  (1<<13)
#define PADRup     (1<< 4)
#define PADRdown   (1<< 6)
#define PADRleft   (1<< 7)
#define PADRright  (1<< 5)
#define PADi       (1<< 9)
#define PADj       (1<<10)
#define PADk       (1<< 8)
#define PADl       (1<< 3)
#define PADm       (1<< 1)
#define PADn       (1<< 2)
#define PADo       (1<< 0)
#define PADh       (1<<11)
#define PADL1      PADn
#define PADL2      PADo
#define PADR1      PADl
#define PADR2      PADm
#define PADstart   PADh
#define PADselect  PADk

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
