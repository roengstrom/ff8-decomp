#ifndef PSXSDK_LIBPRESS_H
#define PSXSDK_LIBPRESS_H

#include "common.h"

/** @brief Reset the MDEC decoder; @p mode 0 also reinstalls the reset callback. */
extern void DecDCTReset(s32 mode);
extern void DecDCTout(u32 *buf, s32 size);
extern s32 DecDCTinCallback(void (*func)());
extern s32 DecDCToutCallback(void (*func)());

#endif /* PSXSDK_LIBPRESS_H */
