#ifndef LIBGPU_H
#define LIBGPU_H

#include "common.h"

/**
 * @brief GPU primitive tag (ordering table link).
 *
 * Bitfield layout matching the PS1 GPU OT format: 24-bit next-pointer
 * and 8-bit packet length.
 */
typedef struct {
    unsigned addr : 24;
    unsigned len : 8;
    u8 r0, g0, b0, code;
} P_TAG;

/* --- Ordering table macros --- */

#define setlen(p, _len)    (((P_TAG *)(p))->len = (u8)(_len))
#define getlen(p)          (u8)(((P_TAG *)(p))->len)
#define setaddr(p, _addr)  (((P_TAG *)(p))->addr = (u32)(_addr))
#define getaddr(p)         (u32)(((P_TAG *)(p))->addr)
#define setcode(p, _code)  (((P_TAG *)(p))->code = (u8)(_code))
#define getcode(p)         (u8)(((P_TAG *)(p))->code)

#define addPrim(ot, p)     setaddr(p, getaddr(ot)), setaddr(ot, p)
#define addPrims(ot, p0, p1) setaddr(p1, getaddr(ot)), setaddr(ot, p0)
#define termPrim(p)        setaddr(p, 0xFFFFFF)

/**
 * @brief Rectangle (position + size), used by GPU primitives.
 */
typedef struct {
    s16 x, y, w, h;
} RECT;

/**
 * @brief GPU TILE primitive (flat-shaded rectangle).
 *
 * 16-byte GPU packet: 4-byte tag + RGB/code + position + size.
 * Code 0x60 identifies this as a TILE primitive.
 */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u16 w, h;
} TILE;

/**
 * @brief GPU draw environment command packet.
 *
 * Contains pre-built GP0 commands for setting the draw environment.
 * 64 bytes: 4-byte tag + 15 command words.
 */
typedef struct {
    u32 tag;
    u32 code[15];
} DR_ENV;

/**
 * @brief Display environment configuration.
 *
 * 20 bytes. Defines the VRAM region shown on screen.
 */
typedef struct {
    RECT disp;
    RECT screen;
    u8 isinter;
    u8 isrgb24;
    u8 pad0, pad1;
} DISPENV;

/**
 * @brief Drawing environment configuration.
 *
 * 92 bytes. Defines the GPU drawing region, texture window, and
 * background clear settings. Includes a DR_ENV with pre-built
 * GP0 commands.
 */
typedef struct {
    RECT clip;
    s16 ofs[2];
    RECT tw;
    u16 tpage;
    u8 dtd;
    u8 dfe;
    u8 isbg;
    u8 r0, g0, b0;
    DR_ENV dr_env;
} DRAWENV;

/* --- GPU function declarations --- */

void ResetGraph(s32 mode);
void SetGraphDebug(s32 level);
s32 DrawSync(s32 mode);
void SetDispMask(s32 mask);
void ClearOTag(u32 *ot, s32 n);
void ClearOTagR(u32 *ot, s32 n);
void DrawOTag(void *p);
void PutDrawEnv(void *env);
void PutDispEnv(void *env);
void ClearImage(void *rect, u8 r, u8 g, u8 b);
void SetDrawStp(u32 *p, s32 dfe);
void AddPrim(s32 *ot, void *p);
void AddPrims(s32 *ot, void *p0, void *p1);

#endif /* LIBGPU_H */
