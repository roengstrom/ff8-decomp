#ifndef LIBGPU_H
#define LIBGPU_H

#include "common.h"

/**
 * @brief GPU primitive tag (ordering table link).
 *
 * Uses bitfields to pack a 24-bit next-pointer and 8-bit packet word count
 * into a single 32-bit word. This matches the codegen produced by the
 * original PsyQ toolchain for addPrim/setaddr/getaddr operations.
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
    s16 dispX;
    s16 dispY;
    RECT tw;
    u16 tpage;
    u8 dtd;
    u8 dfe;
    u8 isbg;
    u8 r0, g0, b0;
    DR_ENV dr_env;
} DRAWENV;

/* --- GPU primitive types --- */

/** @brief Draw mode command (GP0(E1h)). Sets texture page, semi-transparency, dithering. */
typedef struct {
    u32 tag;
    u32 code[1];
} DR_MODE;

/** @brief Flat-shaded triangle. Code 0x20. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    s16 x1, y1;
    s16 x2, y2;
} POLY_F3;

/** @brief Flat-shaded quad. Code 0x28. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    s16 x1, y1;
    s16 x2, y2;
    s16 x3, y3;
} POLY_F4;

/** @brief Textured triangle. Code 0x24. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
    s16 x1, y1;
    u8 u1, v1; u16 tpage;
    s16 x2, y2;
    u8 u2, v2; u16 pad1;
} POLY_FT3;

/** @brief Textured quad. Code 0x2C. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
    s16 x1, y1;
    u8 u1, v1; u16 tpage;
    s16 x2, y2;
    u8 u2, v2; u16 pad1;
    s16 x3, y3;
    u8 u3, v3; u16 pad2;
} POLY_FT4;

/** @brief Gouraud-shaded triangle. Code 0x30. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 r1, g1, b1, pad1;
    s16 x1, y1;
    u8 r2, g2, b2, pad2;
    s16 x2, y2;
} POLY_G3;

/** @brief Gouraud-shaded quad. Code 0x38. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 r1, g1, b1, pad1;
    s16 x1, y1;
    u8 r2, g2, b2, pad2;
    s16 x2, y2;
    u8 r3, g3, b3, pad3;
    s16 x3, y3;
} POLY_G4;

/** @brief Gouraud-shaded textured triangle. Code 0x34. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
    u8 r1, g1, b1, pad1;
    s16 x1, y1;
    u8 u1, v1; u16 tpage;
    u8 r2, g2, b2, pad2;
    s16 x2, y2;
    u8 u2, v2; u16 pad3;
} POLY_GT3;

/** @brief Gouraud-shaded textured quad. Code 0x3C. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
    u8 r1, g1, b1, pad1;
    s16 x1, y1;
    u8 u1, v1; u16 tpage;
    u8 r2, g2, b2, pad2;
    s16 x2, y2;
    u8 u2, v2; u16 pad3;
    u8 r3, g3, b3, pad4;
    s16 x3, y3;
    u8 u3, v3; u16 pad5;
} POLY_GT4;

/** @brief Straight line. Code 0x40. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    s16 x1, y1;
} LINE_F2;

/** @brief Gouraud-shaded line. Code 0x50. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 r1, g1, b1, pad1;
    s16 x1, y1;
} LINE_G2;

/** @brief Free-size sprite. Code 0x64. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
    u16 w, h;
} SPRT;

/** @brief 16x16 sprite. Code 0x7C. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
} SPRT_16;

/** @brief 8x8 sprite. Code 0x74. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
} SPRT_8;

/** @brief Combined texture page + sprite primitive (24 bytes).
 *  Single P_TAG covers the whole packet; no separate SPRT tag. */
typedef struct {
    u32 tag;                  /* +0x00: P_TAG */
    u32 drawMode;             /* +0x04: GP0(E1) texture page command */
    u8 r0, g0, b0, code;     /* +0x08: SPRT color + code (0x64) */
    s16 x0, y0;              /* +0x0C: sprite position */
    u8 u0, v0; u16 clut;     /* +0x10: texture UV + CLUT */
    u16 w, h;                 /* +0x14: sprite dimensions */
} TSPRT;

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
void SetDrawArea(u8 *p, RECT *rect);
void SetDrawOffset(u8 *p, RECT *rect);

#endif /* LIBGPU_H */
