#ifndef LIBCD_H
#define LIBCD_H

#include "common.h"

#define CdlModeSpeed 0x80
#define CdlModeSize1 0x20

#define CdlStatPlay 0x80
#define CdlStatSeek 0x40
#define CdlStatShellOpen 0x10

#define CdlNop 0x01
#define CdlSetloc 0x02
#define CdlReadN 0x06
#define CdlPause 0x09
#define CdlSetmode 0x0E

#define CdlDataReady 0x01
#define CdlComplete 0x02

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
typedef void (*CdlCB)(u8 intr, u8 *result);

/* --- libcd function declarations (signatures match PsyQ 4.6 LIBCD.H) --- */

s32 CdControl(u8 com, u8 *param, u8 *result);
s32 CdControlB(u8 com, u8 *param, u8 *result);
s32 CdControlF(u8 com, u8 *param);
void CdMix(CdlATV *vol);
s32 CdGetSector(void *madr, s32 size);
s32 CdPosToInt(CdlLOC *p);
CdlLOC *CdIntToPos(s32 i, CdlLOC *p);
CdlCB CdSyncCallback(CdlCB func);
CdlCB CdReadyCallback(CdlCB func);
s32 CdSync(s32 mode, u8 *result);
s32 CdReady(s32 mode, u8 *result);
s32 CdStatus(void);
s32 CdMode(void);
u8 *CdLastPos(void);
void CdFlush(void);

#endif /* LIBCD_H */
