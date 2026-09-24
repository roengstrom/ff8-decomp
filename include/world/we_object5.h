#ifndef WORLD_WE_OBJECT5_H
#define WORLD_WE_OBJECT5_H

#include "common.h"
#include "world.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"

/**
 * @brief 8-byte short DR_MODE-like used by @c D_800D4FA0[2] — a P_TAG word
 *        (containing @c len) followed by a single GPU command word.
 */
typedef struct {
    /* 0x00 */ u32 tag;   /**< P_TAG-compatible: addr:24 + len:8. */
    /* 0x04 */ u32 cmd;   /**< Raw GP0 command word (e.g. @c 0xE1000220 TPAGE). */
} DrSetMode8;

/**
 * @brief Per-frame world-view transform record: an actor's offset/angles
 *        inputs followed by its composed rotation matrix output.
 *
 * @c func_800AC468 reads @c offset / @c angles and writes @c matrix; its two
 * pointer params normally alias the same actor record. The layout mirrors
 * the leading 0x30 bytes a world actor reserves for its camera transform.
 */
typedef struct {
    /* 0x00 */ SVECTOR offset;
    /* 0x08 */ SVECTOR angles;
    /* 0x10 */ MATRIX  matrix;
} WorldViewXform;  /* 0x30 */

/**
 * @brief 32-byte input passed to @ref func_800ACA70. Contains two
 *        16-byte halves: a 4-word @c a block (opaque, forwarded as-is
 *        to @c ApplyTransposeMatrixLV) and a @c b block whose first u32 is
 *        kept while the trailing 3 s32s are negated to invert a
 *        position before the call.
 */
typedef struct {
    /* 0x00 */ VECTOR a;       /**< Opaque 16-byte block, passed through. */
    /* 0x10 */ s32    b_pre;   /**< Untouched (likely a tag / mode byte). */
    /* 0x14 */ s32    b_x;     /**< Negated before @c ApplyTransposeMatrixLV. */
    /* 0x18 */ s32    b_y;     /**< Negated. */
    /* 0x1C */ s32    b_z;     /**< Negated. */
} Input32;

extern POLY_FT4   D_800D8810[2];
extern POLY_FT4   D_800D8860[2];
extern s32        D_800D23D0;
/* The four gouraud quads that back the map-view gradient: func_800ABC98
   seeds all four, func_800A8A28 fills [0..1] for the active scene and
   [2..3] (aliased as D_800D4F58) for the D_800CA040 sentinel. */
extern POLY_G4    D_800D4F10[4];
extern POLY_G4    D_800D4F58[2];
extern DrSetMode8 D_800D4FA0[2];
extern u8         D_800C5448[];
extern POLY_FT4   D_800D4EC0[2];
extern CVECTOR    D_8009811C;
extern VECTOR     D_800C9858;     /* live camera world position (VECTOR view)     */
extern VECTOR     D_800DD658;     /* source position for func_800BC51C            */

extern void func_800A84D0(void);
extern s32  LoadAverageCol(CVECTOR *input, CVECTOR *cue, s32 w1, s32 w2, CVECTOR *out);
extern s32  func_800A4700(s32 a, s32 b);
extern s32  func_800A475C(s32 a, s32 b);
extern void ApplyTransposeMatrixLV(VECTOR *a, s32 *b_pos, VECTOR *out);
/* Rotates four corner offsets into place and projects them; @c outSXY takes
 * the four packed screen points and @c outOTZ their average depth. */
extern void func_800ABEF0(SVECTOR *src, SVECTOR *rot, SVECTOR *trans,
                          DVECTOR *outSXY, u32 *outOTZ);
extern void func_800B5C60(s32 ctx, s16 count, MATRIX *outMat, SVECTOR *outAngles,
                          SVECTOR *rotBuf, u8 *xform, ActorRecord *recs);

/* Depth-cue a CVECTOR run into BGR555 halfwords (see the definition for the
   per-pixel blend and the STP-bit packing). */
extern void func_800ABDD8(CVECTOR *input, u16 *output, s32 z, s16 count);

extern s32  func_800AC0A0(s32 type, VECTOR *pos, SVECTOR *vec, u16 flags);

extern void func_800AC2B8(void);

#endif /* WORLD_WE_OBJECT5_H */
