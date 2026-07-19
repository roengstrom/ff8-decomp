#ifndef TT_TEXT_H
#define TT_TEXT_H

#include "common.h"
#include "tripletriad.h"
#include "psxsdk/libgpu.h"
#include "tripletriad/tt_main.h"   /* RGB, ResHeader (g_textBufferRes) */

/* Declarations for tt_text.c: the debug-text overlay + printf-style
   formatter, the tetrahedral 3D icon renderer, and the per-match card-hand
   setup / card-flip animation (the second half of the original object-1 CU). */

/* ── Typedefs ─────────────────────────────────────────────────────────── */

/* CardFlipState (node->state values) lives in tt_main.h beside HandlerNode:
   the match-flow driver (tt_match.c) also uses it to arm the flip. */

/** @brief 0x28 per-frame transform scratch for the card-flip handler: a
 *  scratch position vector followed by the composed rotation+translation
 *  matrix handed to the GTE. Allocated/freed each frame (scratchAlloc/BA0). */
typedef struct {
    SVECTOR vec;   /* 0x00 — scratch position (morph target)        */
    MATRIX  mat;   /* 0x08 — composed YXZ rotation + translation     */
} TransformBuf;    /* 0x28 */

/**
 * @brief One face of the tetrahedral 3D icon model (@c g_triadIconFaces[4]).
 *
 * The 4 entries pair with the 4-vertex SVECTOR table @c g_triadIconVerts to describe
 * a tetrahedral 3D icon (3 yellow faces and one white face). The 3 vertex
 * indices select corners from the transformed vertex output; the three color
 * words pre-pack the @c POLY_G3 @c r/g/b/code byte quartets for direct
 * word-store into the primitive.
 */
typedef struct {
    u8  v0;             /**< Index 0..3 into the transformed vertex table. */
    u8  v1;
    u8  v2;
    u8  pad03;
    u32 color0Word;     /**< Packed @c r0|g0|b0|code (with @c 0x30 = G3 code). */
    u32 color1Word;     /**< Packed @c r1|g1|b1|pad1. */
    u32 color2Word;     /**< Packed @c r2|g2|b2|pad2. */
} TriadFaceDesc;        /* 0x10 bytes */

/* ── Prototypes ───────────────────────────────────────────────────────── */

/* Debug-text overlay + printf-style formatter. */
extern void initTextBuffer(void);
extern void setTextOrigin(s32 x, s32 y);
extern void drawTextChar(u8 ch);
extern void drawText(u8 *str);
extern u8  *intToDecStr(s32 value, u8 *out);
extern u8  *intToHexStr(s32 value, u8 *out);
extern u8  *intToBinStr(s32 value, u8 *out);
extern s32  formatString(char *dst, s32 *args);
extern s32  ttSprintf(s32 dst, s32 fmt, ...);
extern void drawTextf(s32 fmt, ...);
extern void flipTextBuffer(void);

/* Tetrahedral 3D icon renderer. */
extern POLY_G3 *drawTriadIcon(void *ot, POLY_G3 *prims);

/* ── Data ─────────────────────────────────────────────────────────────── */

/* Card-flip transform scratch. */
extern SVECTOR       g_cardFlipUpVec;    /**< +Z unit scratch vector (morph source). */
extern SVECTOR       g_cardFlipTarget;   /**< Scratch target vector. */
extern SVECTOR       g_cardFlipAngles;   /**< Per-frame YXZ rotation angles. */
extern s32           g_cardFlipSpin;     /**< Spin direction delta (+/-0x400). */
extern TransformBuf *g_cardFlipXform;    /**< Current frame's transform scratch. */

/* Tetrahedral 3D icon model. */
extern SVECTOR       g_triadIconVerts[4];  /**< 4-vertex tetrahedron model. */
extern TriadFaceDesc g_triadIconFaces[4];  /**< 4 G3 face descriptors. */
extern u32          *g_triadIconScratch;   /**< Scratch buffer for RotTransPers4 outputs. */

/* Debug-text buffers / state. */
extern ResHeader     g_textBufferRes;      /**< Text-font resource header (queued to VRAM at init). */
extern u8            g_textOTs[2][8];
extern u8            g_textFrameBufs[2][0x8000];
extern u8           *g_textFbPtr;
extern u8           *g_textOtPtr;
extern u8            g_hexDigits[];
extern s16           g_textBufferIndex;
extern s16           g_textLineX;
extern s16           g_textCursorY;
extern s16           g_textCursorX;
extern RGB           g_textColor;          /**< Debug-text rgb color. */
extern u32           g_textPalette[9];     /**< '#0'..'#8' escape colors, indexed digit - '0'. */

/* SDK / main-binary math helpers used by the icon + card-flip code. */
extern void func_8003F884(SVECTOR *a, SVECTOR *b, s32 wa, s32 wb, SVECTOR *out);
extern void func_80041794(s32 angle, MATRIX *m);

#endif /* TT_TEXT_H */
