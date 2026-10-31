#ifndef LIBCD_H
#define LIBCD_H

#include "common.h"

/** @brief CD-ROM disc location (BCD minute / second / sector). */
typedef struct {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
} CdlLOC;

/** @brief CD audio attenuation (mixing) levels. */
typedef struct {
    u8 val0; /**< L-to-L attenuation. */
    u8 val1; /**< L-to-R attenuation. */
    u8 val2; /**< R-to-R attenuation. */
    u8 val3; /**< R-to-L attenuation. */
} CdlATV;

/** @brief CD callback function pointer. */
typedef void (*CdlCB)(s32 intr, u8 *result);

/* --- libcd function declarations (signatures match PsyQ 4.6 LIBCD.H) --- */

s32 CdControl(u8 com, u8 *param, u8 *result);
s32 CdControlB(u8 com, u8 *param, u8 *result);
s32 CdControlF(u8 com, u8 *param);
s32 CdMix(CdlATV *vol);
s32 CdGetSector(void *madr, s32 size);
s32 CdPosToInt(CdlLOC *p);
CdlLOC *CdIntToPos(s32 i, CdlLOC *p);
CdlCB CdSyncCallback(CdlCB func);
CdlCB CdReadyCallback(CdlCB func);
s32 CdSync(s32 mode, u8 *result);
s32 CdDataSync(s32 mode);
s32 CdStatus(void);
s32 CdMode(void);
CdlLOC *CdLastPos(void);
void CdFlush(void);

#endif /* LIBCD_H */
