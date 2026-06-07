#ifndef WORLD_WE_OBJECT1_H
#define WORLD_WE_OBJECT1_H

#include "common.h"
#include "world.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"

/* Projection scratch: func_800A40F8 writes @c proj and returns @c angle. The
   trailing @c pad keeps the buffer 0x20 bytes (gcc reserves the full slot). */
typedef struct {
    VECTOR proj;
    s16 angle;
    s16 pad[7];
} ProjBuf;

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
extern s32       D_800C97A0;
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
extern DRAWENV  *g_activeDrawEnv;
extern u32       D_800D2278[];
extern VECTOR    D_800980DC;   /* constant view offset {0, 0, -0x1800, 0} */
extern VECTOR    D_800C9748;   /* mirrored copy of the transformed position */
extern CmdDesc  *D_800C4D6C;
extern s16       D_800C9E38[3];
extern s32       D_800C9778[];     /* scratch buffer passed to the visibility check */
extern RECT      D_800C8698;
extern u8        D_800980CC[]; /* "x:\USPC\WORLD" — dev-filesystem prefix (13 chars + NUL) */
extern POLY_FT4  D_800C8648[2]; /* double-buffered worldmap quad primitive */

extern void func_80048DD4(RECT *r, s32 a, s32 b, s32 c);
extern void func_8009FEDC(u8 *work, u8 type);
extern void func_80042634(s32 a);
extern void func_80048FBC(RECT *r, s32 srcX, s32 srcY);
extern void func_80048C50(s32 a);
extern void func_800A5F78(s32 screen);
extern void func_800A5FD4(s32 screen);
extern void func_800A5D10(void);
extern void func_80048BB8(s32 a);
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
extern s32 func_800ACD38(MATRIX *out);
extern void func_8003FD84(MATRIX *xform, VECTOR *in, VECTOR *out);
extern void func_800BC544(VECTOR *src, VECTOR *dst);
extern s32 func_800A40F8(VECTOR *pos, VECTOR *out);
extern CmdDesc *func_800A3870(VECTOR *v, AngleSlot *out);
extern s32 func_800BEC1C(s32 kind);
extern s32 func_800A2D50(s32 a0, s32 a1, s32 *out, s32 a3, s32 a4, s32 a5);
extern void func_8009D630(void);
extern void func_800B3FD4(Slot *slot, s32 arg);
extern void fadeOutSfxFast(s32 idx);
extern void renderAndUpdateDisplay(s32 frames);
extern s32 renderBattleDisplayList(s32 *colorTag);
extern void sndSeqSetTempoAlt(s32 tempo);
extern void sndSetMasterVolume(s32 vol);
extern void sndCmdF1(void);
extern void sndSetChannelVolume(s32 channel, s32 vol);
extern void sndSeqStartPan(s32 a0, s32 a1, s32 a2, s32 a3);
extern void sndSeqPlayPan7bit(s32 a0, s32 a1, s32 a2, s32 a3);
extern void func_8004D604(POLY_FT4 *prim, s32 frame);

#endif /* WORLD_WE_OBJECT1_H */
