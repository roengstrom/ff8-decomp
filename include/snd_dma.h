/**
 * @file snd_dma.h
 * @brief SPU DMA / voice-register interface (src/snd_dma.c).
 *
 * Sample-bank validation and upload, the SPU DMA read/write helpers, and
 * the thin spuSet* wrappers over the SPU voice registers. @c SndBankDesc
 * lives in sound.h.
 */
#ifndef SND_DMA_H
#define SND_DMA_H

#include "sound.h"

extern u32 sndValidateBank(u32 *a0);
extern void sndDmaCompleteCallback(void);
extern void sndDmaSetActive(void);
extern void sndDmaWriteSpu(u8 *a0, u32 a1);
extern void sndDmaReadSpu(u8 *a0, u32 a1);
extern void sndDmaWait(void);
extern s32 sndUploadSample(SndBankDesc *a0, s32 a1);
extern void sndSpuInit(void);
extern void sndSpuShutdown(void);
extern void spuSetReverbWorkAddr(u32 val);
extern void spuSetIrqAddr(u32 val);
extern void spuSetTransferControl(u32 val);
extern void spuSetDataFifo(u32 val);
extern void spuSetTransferAddr(u32 val);
extern void spuSetVoiceVolume(s32 voice, s32 vol_l, s32 vol_r, s32 scale);
extern void spuSetVoicePitch(s32 voice, s32 val);
extern void spuSetVoiceStartAddr(s32 voice, u32 val);
extern void spuSetVoiceRepeatAddr(s32 voice, u32 val);
extern void spuSetVoiceAdsrLow(s32 voice, s32 val);
extern void spuSetVoiceAdsrHigh(s32 voice, s32 val);
extern void spuSetVoiceAttack(s32 voice, u32 a1, u32 a2);
extern void spuSetVoiceDecayShift(s32 voice, u32 a1);
extern void spuSetVoiceSustainLevel(s32 voice, u32 a1);
extern void spuSetVoiceSustainMode(s32 voice, u32 a1, u32 a2);
extern void spuSetVoiceReleaseMode(s32 voice, u32 a1, u32 a2);

#endif /* SND_DMA_H */
