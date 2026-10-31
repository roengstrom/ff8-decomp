#ifndef LIBSPU_H
#define LIBSPU_H

#include "common.h"

typedef void (*SpuIRQCallbackProc)(void);
typedef void (*SpuTransferCallbackProc)(void);

extern void SpuInit(void);
extern void SpuStart(void);
extern void SpuQuit(void);
extern s32 SpuSetReverb(s32 on_off);
extern u32 SpuWrite(u8 *addr, u32 size);
extern u32 SpuRead(u8 *addr, u32 size);
extern s32 SpuSetTransferMode(s32 mode);
extern u32 SpuSetTransferStartAddr(u32 addr);
extern SpuTransferCallbackProc SpuSetTransferCallback(SpuTransferCallbackProc func);
extern s32 SpuSetIRQ(s32 on_off);
extern u32 SpuSetIRQAddr(u32 addr);
extern SpuIRQCallbackProc SpuSetIRQCallback(SpuIRQCallbackProc func);
extern s32 SpuInitMalloc(s32 num, char *top);
extern s32 SpuSetNoiseClock(s32 n_clock);
extern void SpuGetVoiceEnvelope(s32 vNum, s16 *envx);
extern s32 SpuSetReverbModeType(s32 mode);
extern void SpuSetReverbModeDepth(s16 depth_left, s16 depth_right);
extern void SpuGetReverbModeType(s32 *mode);

#endif /* LIBSPU_H */
