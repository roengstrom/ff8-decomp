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

/* --- Ordering table macros (PsyQ SDK) --- */

#define setlen(p, _len)    (((P_TAG *)(p))->len = (u8)(_len))
#define getlen(p)          (u8)(((P_TAG *)(p))->len)
#define setaddr(p, _addr)  (((P_TAG *)(p))->addr = (u32)(_addr))
#define getaddr(p)         (u32)(((P_TAG *)(p))->addr)
#define setcode(p, _code)  (((P_TAG *)(p))->code = (u8)(_code))
#define getcode(p)         (u8)(((P_TAG *)(p))->code)

#define nextPrim(p)        (void *)((((P_TAG *)(p))->addr) | 0x80000000)
#define isendprim(p)       ((((P_TAG *)(p))->addr) == 0xffffff)

#define addPrim(ot, p)     setaddr(p, getaddr(ot)), setaddr(ot, p)
#define addPrims(ot, p0, p1) setaddr(p1, getaddr(ot)), setaddr(ot, p0)
#define catPrim(p0, p1)    setaddr(p0, p1)
#define termPrim(p)        setaddr(p, 0xffffffff)

/* --- Primitive field-setting macros (PsyQ SDK) --- */

/* Set RGB color components on a primitive (one set per vertex). */
#define setRGB0(p, _r0, _g0, _b0) \
    ((p)->r0 = (_r0), (p)->g0 = (_g0), (p)->b0 = (_b0))
#define setRGB1(p, _r1, _g1, _b1) \
    ((p)->r1 = (_r1), (p)->g1 = (_g1), (p)->b1 = (_b1))
#define setRGB2(p, _r2, _g2, _b2) \
    ((p)->r2 = (_r2), (p)->g2 = (_g2), (p)->b2 = (_b2))
#define setRGB3(p, _r3, _g3, _b3) \
    ((p)->r3 = (_r3), (p)->g3 = (_g3), (p)->b3 = (_b3))

/* Set screen position vertices (1 / 2 / 3 / 4 corners). */
#define setXY0(p, _x0, _y0) \
    ((p)->x0 = (_x0), (p)->y0 = (_y0))
#define setXY2(p, _x0, _y0, _x1, _y1) \
    ((p)->x0 = (_x0), (p)->y0 = (_y0), \
     (p)->x1 = (_x1), (p)->y1 = (_y1))
#define setXY3(p, _x0, _y0, _x1, _y1, _x2, _y2) \
    ((p)->x0 = (_x0), (p)->y0 = (_y0), \
     (p)->x1 = (_x1), (p)->y1 = (_y1), \
     (p)->x2 = (_x2), (p)->y2 = (_y2))
#define setXY4(p, _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3) \
    ((p)->x0 = (_x0), (p)->y0 = (_y0), \
     (p)->x1 = (_x1), (p)->y1 = (_y1), \
     (p)->x2 = (_x2), (p)->y2 = (_y2), \
     (p)->x3 = (_x3), (p)->y3 = (_y3))

/* Set the four corners of a rectangle from origin (x,y) and size (w,h). */
#define setXYWH(p, _x0, _y0, _w, _h) \
    ((p)->x0 = (_x0),       (p)->y0 = (_y0),       \
     (p)->x1 = (_x0) + (_w), (p)->y1 = (_y0),      \
     (p)->x2 = (_x0),       (p)->y2 = (_y0) + (_h), \
     (p)->x3 = (_x0) + (_w), (p)->y3 = (_y0) + (_h))

/* Set primitive width/height. */
#define setWH(p, _w, _h)   ((p)->w = (_w), (p)->h = (_h))

/* Set texture UV vertices (1 / 3 / 4 corners). */
#define setUV0(p, _u0, _v0) \
    ((p)->u0 = (_u0), (p)->v0 = (_v0))
#define setUV3(p, _u0, _v0, _u1, _v1, _u2, _v2) \
    ((p)->u0 = (_u0), (p)->v0 = (_v0), \
     (p)->u1 = (_u1), (p)->v1 = (_v1), \
     (p)->u2 = (_u2), (p)->v2 = (_v2))
#define setUV4(p, _u0, _v0, _u1, _v1, _u2, _v2, _u3, _v3) \
    ((p)->u0 = (_u0), (p)->v0 = (_v0), \
     (p)->u1 = (_u1), (p)->v1 = (_v1), \
     (p)->u2 = (_u2), (p)->v2 = (_v2), \
     (p)->u3 = (_u3), (p)->v3 = (_v3))

/* Set the four UV corners of a textured rectangle. */
#define setUVWH(p, _u0, _v0, _w, _h) \
    ((p)->u0 = (_u0),       (p)->v0 = (_v0),       \
     (p)->u1 = (_u0) + (_w), (p)->v1 = (_v0),      \
     (p)->u2 = (_u0),       (p)->v2 = (_v0) + (_h), \
     (p)->u3 = (_u0) + (_w), (p)->v3 = (_v0) + (_h))

/* Initialise a textured 3-vertex polygon (len=7 words, code=0x24). */
#define setPolyFT3(p)    setlen(p, 7),  setcode(p, 0x24)

/* Initialise a flat-shaded 4-vertex polygon (len=5 words, code=0x28). */
#define setPolyF4(p)     setlen(p, 5),  setcode(p, 0x28)

/* Initialise a textured 4-vertex polygon primitive (len=9 words, code=0x2C). */
#define setPolyFT4(p)    setlen(p, 9),  setcode(p, 0x2c)

/* Initialise a gouraud-shaded 3-vertex polygon (len=6 words, code=0x30). */
#define setPolyG3(p)     setlen(p, 6),  setcode(p, 0x30)

/* Initialise a gouraud-shaded 4-vertex polygon (len=8 words, code=0x38). */
#define setPolyG4(p)     setlen(p, 8),  setcode(p, 0x38)

/* Initialise a 1x1 tile primitive (len=2 words, code=0x68). */
#define setTile1(p)      setlen(p, 2),  setcode(p, 0x68)

/* Initialise a gouraud-shaded textured 3-vertex polygon (len=9 words, code=0x34). */
#define setPolyGT3(p)    setlen(p, 9),  setcode(p, 0x34)

/* Initialise a gouraud-shaded textured 4-vertex polygon (len=12 words, code=0x3C). */
#define setPolyGT4(p)    setlen(p, 12), setcode(p, 0x3c)

/* Initialise a 16x16 sprite primitive (len=3 words, code=0x7C). */
#define setSprt16(p)     setlen(p, 3),  setcode(p, 0x7c)

/* Toggle semi-transparency / shading bits in the primitive's code byte. */
#define setSemiTrans(p, abe) \
    ((abe) ? setcode(p, getcode(p) | 0x02) : setcode(p, getcode(p) & ~0x02))
#define setShadeTex(p, tge) \
    ((tge) ? setcode(p, getcode(p) | 0x01) : setcode(p, getcode(p) & ~0x01))

/* Pack a texture-page descriptor (GP0 0xE1 payload). */
#define getTPage(tp, abr, x, y) \
    ((((tp) & 0x3) << 7) | (((abr) & 0x3) << 5) | \
     (((y) & 0x100) >> 4) | (((x) & 0x3ff) >> 6) | (((y) & 0x200) << 2))

/* Pack a CLUT descriptor. */
#define getClut(x, y) \
    (((y) << 6) | (((x) >> 4) & 0x3f))

/* Store a tpage / clut into a primitive's tpage / clut field. */
#define setTPage(p, tp, abr, x, y) \
    ((p)->tpage = getTPage((tp), (abr), (x), (y)))
#define setClut(p, x, y) \
    ((p)->clut = getClut((x), (y)))

/**
 * @brief Rectangle (position + size), used by GPU primitives.
 */
typedef struct {
    s16 x, y, w, h;
} RECT;

/** @brief Populate a RECT with position (x,y) and size (w,h). */
#define setRECT(r, _x, _y, _w, _h) \
    ((r)->x = (_x), (r)->y = (_y), (r)->w = (_w), (r)->h = (_h))

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
    s16 w, h;
} TILE;

/** @brief 1x1 tile — fixed-size TILE with no w/h. Code 0x68. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
} TILE_1;

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
    u32 code[2];
} DR_MODE;

/** @brief Texture window command (GP0(E2h)). */
typedef struct {
    u32 tag;
    u32 code[2];
} DR_TWIN;

/** @brief Drawing area command (GP0(E3h)/E4h). */
typedef struct {
    u32 tag;
    u32 code[2];
} DR_AREA;

/** @brief Drawing offset command (GP0(E5h)). */
typedef struct {
    u32 tag;
    u32 code[2];
} DR_OFFSET;

/** @brief Drawing STP mask command (GP0(E6h)). */
typedef struct {
    u32 tag;
    u32 code[2];
} DR_STP;

/** @brief Drawing tpage command (single u32 payload). */
typedef struct {
    u32 tag;
    u32 code[1];
} DR_TPAGE;

/** @brief Packed VRAM-to-VRAM move command, built by @ref SetDrawMove. */
typedef struct {
    u32 tag;
    u32 code[5];
} DR_MOVE;

/** @brief Packed VRAM upload command, built by @ref SetDrawLoad. */
typedef struct {
    u32 tag;
    u32 code[3];
    u32 p[13];
} DR_LOAD;

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

/** @brief Gouraud-shaded 4-point poly-line strip. Code 0x58. */
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
} LINE_G4;

/** @brief Free-size sprite. Code 0x64. */
typedef struct {
    u32 tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0; u16 clut;
    s16 w, h;
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
void DrawPrim(void *p);
void LoadImage(RECT *rect, u32 *data);
void StoreImage(RECT *rect, u32 *data);
/* Pack a VRAM-to-VRAM move of @p rect to (@p x, @p y) into the primitive @p p. */
void SetDrawMove(DR_MOVE *p, RECT *rect, s32 x, s32 y);
/* Pack an upload of @p rect's pixels into the primitive @p p. */
void SetDrawLoad(DR_LOAD *p, RECT *rect);
/* Pack the drawing environment @p env into the primitive @p dr_env. */
void SetDrawEnv(DR_ENV *dr_env, DRAWENV *env);
DRAWENV *SetDefDrawEnv(DRAWENV *env, s32 x, s32 y, s32 w, s32 h);
DISPENV *SetDefDispEnv(DISPENV *env, s32 x, s32 y, s32 w, s32 h);
void PutDrawEnv(void *env);
void PutDispEnv(void *env);
void ClearImage(void *rect, u8 r, u8 g, u8 b);
void SetDrawStp(u32 *p, s32 dfe);
void AddPrim(void *ot, void *p);
void AddPrims(s32 *ot, void *p0, void *p1);
void SetDrawArea(DR_AREA *p, RECT *rect);
void SetDrawOffset(DR_OFFSET *p, RECT *rect);
s32 MoveImage(RECT *rect, s32 x, s32 y);
s32 OpenTIM(u32 *addr);
void *ReadTIM(void *timimg);
s32 GetODE(void);
void SetSemiTrans(void *p, s32 abe);
void SetShadeTex(void *p, s32 tge);
void SetDrawTPage(void *p, s32 dfe, s32 dtd, u16 tpage);
void SetTile(void *p);
void SetSprt(SPRT *p);
u16 GetTPage(s32 tp, s32 abr, s32 x, s32 y);

#endif /* LIBGPU_H */
