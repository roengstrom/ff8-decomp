#ifndef WORLD_WE_OBJECT2_H
#define WORLD_WE_OBJECT2_H

#include "common.h"
#include "world.h"
#include "psxsdk/libgte.h"

typedef struct {
    s16 a;
    s16 b;
} HalfwordPair;

/**
 * @brief 16-byte world-map view/projection buffer (setupWorldMapView fills two).
 *
 * @note Field roles are partly inferred. @c func_800A40F8 projects a VECTOR
 *       into @c [0x0..0x4] of one buffer; @c func_8009FF70 writes a per-map
 *       halfword triple into @c [0x0..0x4] of the other. @c setupWorldMapView
 *       reads @c [0x2] as the scroll input, stores a per-map scroll offset at
 *       @c [0x8], and clears the flag at @c [0xC]. Exact semantics uncertain.
 */
typedef struct {
    /* 0x00 */ s16 unk00;
    /* 0x02 */ s16 unk02;   /**< Read from arg1 as func_800A0000's scroll input. */
    /* 0x04 */ s16 unk04;
    /* 0x06 */ s16 unk06;
    /* 0x08 */ s16 scroll;  /**< Per-map scroll offset (func_800A0000) / 0. */
    /* 0x0A */ s16 unk0A;
    /* 0x0C */ s16 flag;    /**< Cleared to 0. */
    /* 0x0E */ s16 unk0E;
} WorldView;               /* 0x10 */

extern s32             D_800C9E68;
extern s32             D_800C9E70;
extern s32             D_800C97EC;
extern s32             D_800C97F0;
extern s16             D_800C8CEA;
extern BattleSceneCtx *D_800D244C;
extern KeyBuffer      *D_800C9880;
extern u32            *D_800C9728;        /* script offset-table blob (u32 offsets + byte records) */
extern DISPENV         D_80082C18;        /* active display environment */
extern u16             D_800C5354[3];
extern u16             D_800C535C[3];
extern u16             D_800C5364[3];
extern s16             D_800C534C;
extern s16             D_800C5344;
extern u16             D_800C5346;        /* per-map yaw target (func_8009F6EC bit 0x8). */
extern s32             D_800C9730;        /* GTE projection-plane distance (SetGeomScreen arg). */
extern s32             D_800D2448;        /* world-ready gate flag (func_8009F6EC bit 0x10). */

/* VSync is declared here with a void-returning view for this file's
 * matching codegen (psxsdk/libetc.h's s32-returning prototype is not used). */
extern void VSync(s32 mode);

extern void func_800A5F78(s32 screen);
extern void func_800A5FD4(s32 screen);
extern void func_80099EDC(s32 idx);
extern void func_800B3FD4(Slot *a0, s32 a1);
extern void func_8009D630(void);
extern void func_800C4450(void);
extern void renderAndUpdateDisplay(s32 mode);
extern s32  renderBattleDisplayList(s32 *colorTag);
extern s32  func_800BD380(s16 *outLow, s16 *outHigh);
extern s32  func_800BD2A0(s16 *outLow, s16 *outHigh);
extern s32  func_800BD460(s16 *outLow, s16 *outHigh);
extern s32  func_8002CE84(s32 idx);
extern s32  func_800A017C(SVECTOR *v);

/* Shared world helper: projects @p pos through the GTE (writes screen x/y/z to @p out),
 * returns a derived angle. Canonical signature — must match we_object1.h. */
extern s32 func_800A40F8(VECTOR *pos, VECTOR *out);
/* Reports whether a script is active and writes its current key to @p output. */
extern s32  func_800BEDF0(u8 *output);

/* Helpers used by setupWorldRender (world-map sound/display setup). */
extern void func_80048C50(s32 a);
extern void func_800488D4(s32 a);
extern void func_800C3DB0(void (*cb)(void));
extern void func_800A47A4(void);
extern void sndEnableReverb(u32 a);
extern void sndDisableReverb(u32 a);

/* Defined in we_object2.c — shortest signed angular difference (0x1000 == 360°). */
extern s32  getAngleDelta(s32 a, s32 b);

#endif /* WORLD_WE_OBJECT2_H */
