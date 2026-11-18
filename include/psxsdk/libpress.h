#ifndef PSXSDK_LIBPRESS_H
#define PSXSDK_LIBPRESS_H

#include "common.h"

typedef u16 DECDCTTAB[34816];

/** @brief Reset the MDEC decoder; @p mode 0 also reinstalls the reset callback. */
extern void DecDCTReset(s32 mode);
extern s32 DecDCTvlc2(u32 *bs, u32 *buf, DECDCTTAB table);
extern s32 DecDCTvlcSize2(s32 size);
extern void DecDCTvlcBuild(u16 *table);
extern void DecDCTin(u32 *buf, s32 mode);
extern void DecDCTout(u32 *buf, s32 size);
extern s32 DecDCToutCallback(void (*func)());

#endif /* PSXSDK_LIBPRESS_H */
