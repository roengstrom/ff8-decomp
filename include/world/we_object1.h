#ifndef WORLD_WE_OBJECT1_H
#define WORLD_WE_OBJECT1_H

#include "common.h"
#include "world.h"
#include "gamestate.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "main.h"

/* Projection scratch: worldPosToCell writes @c proj and returns @c angle. The
   trailing @c pad keeps the buffer 0x20 bytes (gcc reserves the full slot).

   @c proj is an SVECTOR in a VECTOR-sized slot: worldPosToCell declares its
   out parameter SVECTOR*, and func_800A3EE4 -- which inlines that projection
   -- writes the three components as halfwords. */
typedef struct {
    SVECTOR proj;
    u8      pad08[8];
    s16 angle;
    s16 pad[7];
} ProjBuf;

/* Argument to glyphAt: a world position followed by its
   projection. @c buf.proj is the query point and @c buf.angle the cell key;
   the caller stages these as adjacent locals (a @c VECTOR and a @c ProjBuf). */
typedef struct {
    VECTOR  pos;   /* 0x00 */
    ProjBuf buf;   /* 0x10 */
} GlyphQuery;      /* 0x30 */

/**
 * @brief 16-byte CD load list entry — NULL-terminated by @c marker = 0.
 *
 * @c marker holds an arbitrary non-zero value while the entry is live; the
 * walker stops on the first entry whose @c marker is 0.
 */
typedef struct {
    /* 0x00 */ s32 marker;
    /* 0x04 */ u8 *dest;
    /* 0x08 */ s32 lba;
    /* 0x0C */ u32 size;
} CdLoadEntry;

/** Section descriptor: provides the base sector that object ids index from. */
typedef struct {
    u8  pad0[8];
    s32 baseLba; /* sector number of id 0; cdRead reads baseLba + id */
} CdSection;

extern u8        D_800C4DCC[];
extern u8        D_800C4FD3;
extern u8        D_800C4FD4;
extern u8        D_800C4FD5;
extern u8        D_800C4FD6;
extern u8        D_800C4FD7;
extern s32       D_800C4FBC;       /* current sequence handle (set from getScrollState out) */
extern s32       D_800C4FC0;       /* current sequence id */
extern s32      *D_800C97A8;       /* sample-bank header (offset table + data) */
extern s32       D_800C97AC;       /* first sample-bank data pointer */
extern s32       D_800C9EDC;       /* default sample-bank handle */
extern s32       D_800C9EE0;       /* alt sample-bank handle (scene cmd 0x40..0x42) */
extern u8        D_800D2442;       /* audio scatter key (vs FieldVars.audioChannel0State) */
extern FieldVars *g_fieldVars;     /* field-engine variables (in gamestate.h) */
extern POLY_F4   D_800C89A8[];    /* dim-overlay quad buffer (sentinel ctx)  */
extern POLY_F4   D_800C86A8[];    /* dim-overlay quad buffer (normal ctx)    */
extern DR_TPAGE  D_800C8CA8[2];   /* draw-mode tpage paired with each buffer  */
extern s16       D_800C4D04;
extern s32       D_800C4D18;
extern s32       D_800C4D1C;
extern s32       D_800C4D24;
extern s32       D_800C4D34;
extern s16       D_800C4D48;
extern s32       D_800C4D50;          /* external trigger flag */
extern s32       D_800C4D54;
extern CmdDesc  *D_800C4D68;
extern s32       D_800C4D78;
extern s32       D_800C4D7C;
extern s32       D_800C4D90;
extern s32       D_800C4D94;
extern s32       D_800C9714;
extern u8        D_800C9758[];        /* 15-byte light-matrix work buffer */
extern u32      *D_800C97A0;
extern s32       D_800D212C;
extern s32       D_800D2458;
extern s32       D_800C4CA4[];   /* source config table */
extern s32       D_800C4F2C[];   /* destination slot table A */
extern s32       D_800C4F4C[];   /* destination slot table B (0x10-byte stride records) */
extern s32       D_800C97E0, D_800C97E4, D_800C9E58, D_800C9E80, D_800C4F14;
extern s32       D_800C9FE0, D_800C97DC;
extern s32       D_800C9FC8, D_800C9FB0, D_800C9FCC, D_800C9FB4, D_800C9FD0, D_800C9FB8;
extern s32       D_800C9FD8, D_800C9FBC, D_800C9FDC, D_800C9FC0, D_800C9FE4, D_800C9FC4;
extern DRAWENV   D_80082C30;      /* active draw environment */
extern DISPENV   D_80082C18;      /* active display environment */
extern u32       D_800D2278[];
extern VECTOR    D_800980DC;   /* constant view offset {0, 0, -0x1800, 0} */
extern VECTOR    D_800C9748;   /* mirrored copy of the transformed position */
extern CmdDesc  *D_800C4D6C;
extern SVECTOR   D_800C9778[];     /* base angles passed to the fan-placement probe */
extern RECT      D_800C8698;
extern u8        D_800980CC[]; /* "x:\USPC\WORLD" — dev-filesystem prefix (13 chars + NUL) */
extern POLY_FT4  D_800C8648[2]; /* double-buffered worldmap quad primitive */

extern void func_8009C528(s32 rc);
extern void func_8009FEDC(u8 *work, u8 type);
extern s32  VSync(s32 a);
extern void func_800A5D10(void);
extern s32  getCurrentFieldMusic(void); /* defined u16 in btl_sfx; used full-width here */
extern void setSfxPitch(s32 idx, s32 val);
extern void setSfxEntityType(s32 idx, s32 val);
extern void setSfxReverbMode(s32 idx, s32 val);
extern void setSfxGlobalFlag(s32 val);
extern void startSfxSlow(s32 idx);
extern void func_8002D784(s32 sfxIdx, u8 *data, s32 paramY, s32 paramZ, s32 paramW, s32 paramV);
extern void func_8002E064(s32 index, RECT *srcRect);
extern s32  func_8002E680(u8 *text);
extern void fadeOutSfxSlow(s32 idx);
extern void initSfxPlayback(s32 index, u8 *data);
extern s32  sndProcessAudio(s32 a, s32 b);
extern s32  sndGetStatus(void);
extern s32  getScrollState(s32 key, u8 *p18, s32 *p1C, s32 *p20, s32 *p24, s32 *p28);
extern void func_80039678(s32 a, s32 b, s32 c);
extern void func_8009CDC4(s32 a, s32 b);
extern void func_8009CE40(void);
extern s32  func_8009D7D8(s32 a);
extern void func_8009C478(s32 *src, s32 x, s32 y);
extern CmdDesc *glyphAt(GlyphQuery *v, AngleSlot *out);
extern s32 func_800BEC1C(s32 kind);
extern void sndSeqSetTempoAlt(s32 tempo);
extern void sndSetMasterVolume(s32 vol);
extern void sndCmdF1(void);
extern void sndSetChannelVolume(s32 channel, s32 vol);
extern void sndSeqStartPan(s32 a0, s32 a1, s32 a2, s32 a3);
extern void sndSeqPlayPan7bit(s32 a0, s32 a1, s32 a2, s32 a3);

extern s32  func_800997E8(u16 *out);
extern void func_80099F78(void);
extern void func_8009A954(void);
extern void func_8009AEE4(s32 id);
extern void func_8009B954(s32 a, s32 b, s32 c);

extern s32 func_8009B358(s32 slotIdx, s32 strIdx, u8 *text);

extern s32 func_8009CC3C(void);

#endif /* WORLD_WE_OBJECT1_H */
