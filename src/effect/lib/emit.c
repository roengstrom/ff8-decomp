/**
 * @file emit.c
 * @brief The mesh prim emitters that write four-word prims.
 */
#include "common.h"
#include "effect.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/inline_c.h"
#include "effect/lib/emit.h"

/**
 * @name Prim emitter clip result
 *
 * One bit per projected vertex per axis, accumulated in the emitters' @c reject.
 * A prim is dropped only when every one of its vertices left the screen on the
 * same axis, so a prim straddling an edge still draws.
 * @{
 */
#define EFFECT_CLIP_X0 0x1
#define EFFECT_CLIP_X1 0x2
#define EFFECT_CLIP_X2 0x4
#define EFFECT_CLIP_X3 0x8
#define EFFECT_CLIP_Y0 0x10
#define EFFECT_CLIP_Y1 0x20
#define EFFECT_CLIP_Y2 0x40
#define EFFECT_CLIP_Y3 0x80
#define EFFECT_CLIP_TRI_X (EFFECT_CLIP_X0 | EFFECT_CLIP_X1 | EFFECT_CLIP_X2)
#define EFFECT_CLIP_TRI_Y (EFFECT_CLIP_Y0 | EFFECT_CLIP_Y1 | EFFECT_CLIP_Y2)
#define EFFECT_CLIP_QUAD_X (EFFECT_CLIP_TRI_X | EFFECT_CLIP_X3)
#define EFFECT_CLIP_QUAD_Y (EFFECT_CLIP_TRI_Y | EFFECT_CLIP_Y3)
/** @} */

/**
 * @name Prim emitter clip test
 *
 * A projected vertex is off screen once its unsigned screen coordinate reaches
 * these; a prim is dropped only when all of its vertices fail the same axis.
 * @{
 */
#define EFFECT_EMIT_CLIP_X 0x141
#define EFFECT_EMIT_CLIP_Y 0xD9
/** @} */

/** @brief GTE @c FLAG bits 18 and 17: SZ3/OTZ saturated, divide overflow. */
#define EFFECT_GTE_OUT_OF_RANGE ((1 << 18) | (1 << 17))

/** @brief One flat triangle in the mesh stream: a colour and three vertex slots. */
typedef struct {
    /* 0x00 */ u32 colour;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u16 idx2;
    /* 0x0A */ u8 pad00A[0xC - 0xA];
} EffectEmitTri; /* 0xC */

/** @brief The flat quad an emitter writes; seven words including its tag. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ u16 x1;
    /* 0x0E */ u16 y1;
    /* 0x10 */ u16 x2;
    /* 0x12 */ u16 y2;
    /* 0x14 */ u16 x3;
    /* 0x16 */ u16 y3;
} EffectQuad; /* 0x18 */

/** @brief One flat quad in the mesh stream: a colour and four vertex slots. */
typedef struct {
    /* 0x00 */ u32 colour;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u16 idx2;
    /* 0x0A */ u16 idx3;
} EffectEmitQuad; /* 0xC */

/**
 * @brief A prim's UV pair and the id that shares its word.
 *
 * The stream carries both halves together, so the emitters add the build's
 * offset to the whole word and only then patch the id half.
 */
typedef union {
    u32 word;
    struct {
        /* 0x00 */ u16 uv;   /**< U in the low byte, V in the high. */
        /* 0x02 */ u16 id;   /**< CLUT id or texture page, by slot. */
    } h;
} EffectPrimUV; /* 0x4 */

/** @brief The textured triangle an emitter writes; nine words including its tag. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ EffectPrimUV uv0; /**< Its id half is the CLUT. */
    /* 0x10 */ u16 x1;
    /* 0x12 */ u16 y1;
    /* 0x14 */ EffectPrimUV uv1; /**< Its id half is the texture page. */
    /* 0x18 */ u16 x2;
    /* 0x1A */ u16 y2;
    /* 0x1C */ EffectPrimUV uv2;
} EffectTexTri; /* 0x20 */

/** @brief One textured triangle in the mesh stream. */
typedef struct {
    /* 0x00 */ u32 colour;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u32 idx2uv2; /**< Third vertex index, with its UV pair above it. */
    /* 0x0C */ u32 uv0;
    /* 0x10 */ u32 uv1;
} EffectEmitTexTri; /* 0x14 */

/** @brief The gouraud triangle an emitter writes; eight words including its tag. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb0;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ u32 rgb1;
    /* 0x10 */ u16 x1;
    /* 0x12 */ u16 y1;
    /* 0x14 */ u32 rgb2;
    /* 0x18 */ u16 x2;
    /* 0x1A */ u16 y2;
} EffectGouraudTri; /* 0x1C */

/** @brief One gouraud triangle in the mesh stream: three corner colours. */
typedef struct {
    /* 0x00 */ u32 colour0;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u16 idx2;
    /* 0x0A */ u8 pad00A[0xC - 0xA];
    /* 0x0C */ u32 colour1;
    /* 0x10 */ u32 colour2;
} EffectEmitGouraudTri; /* 0x14 */

/** @brief The gouraud quad an emitter writes; ten words including its tag. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb0;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ u32 rgb1;
    /* 0x10 */ u16 x1;
    /* 0x12 */ u16 y1;
    /* 0x14 */ u32 rgb2;
    /* 0x18 */ u16 x2;
    /* 0x1A */ u16 y2;
    /* 0x1C */ u32 rgb3;
    /* 0x20 */ u16 x3;
    /* 0x22 */ u16 y3;
} EffectGouraudQuad; /* 0x24 */

/** @brief One gouraud quad in the mesh stream: four corner colours. */
typedef struct {
    /* 0x00 */ u32 colour0;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u16 idx2;
    /* 0x0A */ u16 idx3;
    /* 0x0C */ u32 colour1;
    /* 0x10 */ u32 colour2;
    /* 0x14 */ u32 colour3;
} EffectEmitGouraudQuad; /* 0x18 */

/** @brief The gouraud textured triangle an emitter writes; eleven words. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb0;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ EffectPrimUV uv0; /**< Its id half is the CLUT. */
    /* 0x10 */ u32 rgb1;
    /* 0x14 */ u16 x1;
    /* 0x16 */ u16 y1;
    /* 0x18 */ EffectPrimUV uv1; /**< Its id half is the texture page. */
    /* 0x1C */ u32 rgb2;
    /* 0x20 */ u16 x2;
    /* 0x22 */ u16 y2;
    /* 0x24 */ EffectPrimUV uv2;
} EffectGouraudTexTri; /* 0x28 */

/** @brief One gouraud textured triangle in the mesh stream. */
typedef struct {
    /* 0x00 */ u32 colour0;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u32 idx2uv2; /**< Third vertex index, with its UV pair above it. */
    /* 0x0C */ u32 uv0;
    /* 0x10 */ u32 uv1;
    /* 0x14 */ u32 colour1;
    /* 0x18 */ u32 colour2;
} EffectEmitGouraudTexTri; /* 0x1C */

/** @brief The gouraud textured quad an emitter writes; fourteen words. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb0;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ EffectPrimUV uv0; /**< Its id half is the CLUT. */
    /* 0x10 */ u32 rgb1;
    /* 0x14 */ u16 x1;
    /* 0x16 */ u16 y1;
    /* 0x18 */ EffectPrimUV uv1; /**< Its id half is the texture page. */
    /* 0x1C */ u32 rgb2;
    /* 0x20 */ u16 x2;
    /* 0x22 */ u16 y2;
    /* 0x24 */ EffectPrimUV uv2;
    /* 0x28 */ u32 rgb3;
    /* 0x2C */ u16 x3;
    /* 0x2E */ u16 y3;
    /* 0x30 */ EffectPrimUV uv3;
} EffectGouraudTexQuad; /* 0x34 */

/**
 * @brief One gouraud textured quad in the mesh stream.
 *
 * The last two UV pairs share a word, so the build's offset is doubled into
 * both halves and the sum split again on the way out.
 */
typedef struct {
    /* 0x00 */ u32 colour0;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u16 idx2;
    /* 0x0A */ u16 idx3;
    /* 0x0C */ u32 uv0;
    /* 0x10 */ u32 uv1;
    /* 0x14 */ u32 uv23;
    /* 0x18 */ u32 colour1;
    /* 0x1C */ u32 colour2;
    /* 0x20 */ u32 colour3;
} EffectEmitGouraudTexQuad; /* 0x24 */

/** @brief The textured quad an emitter writes; eleven words including its tag. */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 rgb;
    /* 0x08 */ u16 x0;
    /* 0x0A */ u16 y0;
    /* 0x0C */ EffectPrimUV uv0; /**< Its id half is the CLUT. */
    /* 0x10 */ u16 x1;
    /* 0x12 */ u16 y1;
    /* 0x14 */ EffectPrimUV uv1; /**< Its id half is the texture page. */
    /* 0x18 */ u16 x2;
    /* 0x1A */ u16 y2;
    /* 0x1C */ EffectPrimUV uv2;
    /* 0x20 */ u16 x3;
    /* 0x22 */ u16 y3;
    /* 0x24 */ EffectPrimUV uv3;
} EffectTexQuad; /* 0x28 */

/** @brief One textured quad in the mesh stream; its last two UV pairs share a word. */
typedef struct {
    /* 0x00 */ u32 colour;
    /* 0x04 */ u16 idx0;
    /* 0x06 */ u16 idx1;
    /* 0x08 */ u16 idx2;
    /* 0x0A */ u16 idx3;
    /* 0x0C */ u32 uv0;
    /* 0x10 */ u32 uv1;
    /* 0x14 */ u32 uv23;
} EffectEmitTexQuad; /* 0x18 */

static EffectTri *effectEmitTris(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                 EffectTri *poly);
static EffectTri *effectEmitQuads(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                  EffectTri *poly);
static EffectTri *effectEmitTexTris(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                    EffectTri *poly);
static EffectTri *effectEmitTexQuads(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                     EffectTri *poly);
static EffectTri *effectEmitGouraudTris(EffectPrimBuild *s, u32 *ot,
                                        s32 otShift, EffectTri *poly);
static EffectTri *effectEmitGouraudQuads(EffectPrimBuild *s, u32 *ot,
                                         s32 otShift, EffectTri *poly);
static EffectTri *effectEmitGouraudTexTris(EffectPrimBuild *s, u32 *ot,
                                           s32 otShift, EffectTri *poly);
static EffectTri *effectEmitGouraudTexQuads(EffectPrimBuild *s, u32 *ot,
                                            s32 otShift, EffectTri *poly);

/**
 * @brief Emit the flat triangles the stream cursor points at.
 *
 * The entry starts with a triangle count; the cursor is stepped past it before
 * anything is drawn, so the caller always resumes at the next entry.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 *
 * @note Two spellings here are load-bearing for the whole emitter family. The
 *       empty @c do/while(0) wraps carry no code: they raise the loop depth
 *       flow.c weights register references by, which is what puts each
 *       variable in the register the original picked, and which statement
 *       needs one differs per emitter. The prim and stream cursors are cast on
 *       the way in and out because the driver chains one cursor type through
 *       eight emitters that each write a different prim.
 */
static EffectTri *effectEmitTris(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                 EffectTri *poly) {
    EffectEmitTri *tri;
    EffectTri *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    tri = (EffectEmitTri *)next;
    if (count > 0) {
        do {
            do {
                gte_ldv3(&verts[tri->idx0], &verts[tri->idx1],
                         &verts[tri->idx2]);
            } while (0);
            gte_rtpt();
            setlen(prim, 4);
            colour = tri->colour;
            prim->rgb = colour;
            if (s->flags & EFFECT_EMIT_SEMITRANS) {
                prim->rgb = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_OPAQUE) {
                prim->rgb &= ~EFFECT_PRIM_CODE(2);
            }
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_avsz3();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    if ((reject & EFFECT_CLIP_TRI_X) != EFFECT_CLIP_TRI_X &&
                        (reject & EFFECT_CLIP_TRI_Y) != EFFECT_CLIP_TRI_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_DEPTH_CUE) {
                            gte_ldrgb(&prim->rgb);
                            gte_lddp(s->depth);
                            gte_dpcs();
                            gte_strgb(&prim->rgb);
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            tri++;
        } while (i < count);
    }
    s->cursor = (s32 *)tri;
    return prim;
}

/**
 * @brief Emit the flat quads the stream cursor points at.
 *
 * Same walk as @ref effectEmitTris with a fourth vertex: the first three are
 * projected together, the fourth on its own, and the average Z takes all four.
 * A quad is dropped when every vertex leaves the screen on the same axis.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitQuads(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                  EffectTri *poly) {
    EffectEmitQuad *quad;
    EffectQuad *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectQuad *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    quad = (EffectEmitQuad *)next;
    if (count > 0) {
        do {
            gte_ldv3(&verts[quad->idx0], &verts[quad->idx1],
                     &verts[quad->idx2]);
            gte_rtpt();
            setlen(prim, 5);
            colour = quad->colour;
            prim->rgb = colour;
            if (s->flags & EFFECT_EMIT_SEMITRANS) {
                prim->rgb = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_OPAQUE) {
                prim->rgb &= ~EFFECT_PRIM_CODE(2);
            }
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_ldv0(&verts[quad->idx3]);
                    gte_rtps();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    gte_stsxy(&prim->x3);
                    gte_avsz4();
                    if (prim->x3 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X3;
                    }
                    if (prim->y3 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y3;
                    }
                    if ((reject & EFFECT_CLIP_QUAD_X) != EFFECT_CLIP_QUAD_X &&
                        (reject & EFFECT_CLIP_QUAD_Y) != EFFECT_CLIP_QUAD_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_DEPTH_CUE) {
                            gte_ldrgb(&prim->rgb);
                            gte_lddp(s->depth);
                            gte_dpcs();
                            gte_strgb(&prim->rgb);
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            quad++;
        } while (i < count);
    }
    s->cursor = (s32 *)quad;
    return (EffectTri *)prim;
}

/**
 * @brief Emit the textured triangles the stream cursor points at.
 *
 * Same walk as @ref effectEmitTris with texture coordinates: each UV word comes
 * from the stream biased by the build's offset, and the texture page and CLUT
 * that share the first two UV words are either left alone, offset by the
 * build's own, or replaced by it.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitTexTris(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                    EffectTri *poly) {
    EffectEmitTexTri *tri;
    EffectTexTri *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectTexTri *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    tri = (EffectEmitTexTri *)next;
    if (count > 0) {
        do {
            do {
                gte_ldv3(&verts[tri->idx0], &verts[tri->idx1],
                         &verts[(u16)tri->idx2uv2]);
            } while (0);
            gte_rtpt();
            setlen(prim, 7);
            colour = tri->colour;
            prim->rgb = colour;
            if (s->flags & EFFECT_EMIT_SEMITRANS) {
                prim->rgb = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_OPAQUE) {
                prim->rgb &= ~EFFECT_PRIM_CODE(2);
            }
            prim->uv0.word = tri->uv0 + s->unk018;
            prim->uv1.word = tri->uv1 + s->unk018;
            prim->uv2.word = (tri->idx2uv2 >> 16) + s->unk018;
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                if (s->flags & EFFECT_EMIT_TPAGE_ADD) {
                    prim->uv1.h.id = prim->uv1.h.id + s->tpage;
                } else if (s->flags & EFFECT_EMIT_TPAGE_SET) {
                    prim->uv1.h.id = s->tpage;
                }
                if (s->flags & EFFECT_EMIT_CLUT_ADD) {
                    prim->uv0.h.id = prim->uv0.h.id + s->clut;
                } else if (s->flags & EFFECT_EMIT_CLUT_SET) {
                    prim->uv0.h.id = s->clut;
                }
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_avsz3();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    if ((reject & EFFECT_CLIP_TRI_X) != EFFECT_CLIP_TRI_X &&
                        (reject & EFFECT_CLIP_TRI_Y) != EFFECT_CLIP_TRI_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_DEPTH_CUE) {
                            gte_ldrgb(&prim->rgb);
                            gte_lddp(s->depth);
                            gte_dpcs();
                            gte_strgb(&prim->rgb);
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            tri++;
        } while (i < count);
    }
    s->cursor = (s32 *)tri;
    return (EffectTri *)prim;
}

/**
 * @brief Emit the textured quads the stream cursor points at.
 *
 * @ref effectEmitQuads with texture coordinates, laid out like
 * @ref effectEmitGouraudTexQuads's but with one flat colour for the whole quad.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitTexQuads(EffectPrimBuild *s, u32 *ot, s32 otShift,
                                     EffectTri *poly) {
    EffectEmitTexQuad *quad;
    EffectTexQuad *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    u32 uv;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectTexQuad *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    quad = (EffectEmitTexQuad *)next;
    if (count > 0) {
        do {
            gte_ldv3(&verts[quad->idx0], &verts[quad->idx1],
                     &verts[quad->idx2]);
            gte_rtpt();
            setlen(prim, 9);
            colour = quad->colour;
            prim->rgb = colour;
            if (s->flags & EFFECT_EMIT_SEMITRANS) {
                prim->rgb = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_OPAQUE) {
                prim->rgb &= ~EFFECT_PRIM_CODE(2);
            }
            prim->uv0.word = quad->uv0 + s->unk018;
            prim->uv1.word = quad->uv1 + s->unk018;
            uv = quad->uv23 + (s->unk018 + (s->unk018 << 16));
            prim->uv2.word = uv;
            prim->uv3.word = uv >> 16;
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                if (s->flags & EFFECT_EMIT_TPAGE_ADD) {
                    prim->uv1.h.id = prim->uv1.h.id + s->tpage;
                } else if (s->flags & EFFECT_EMIT_TPAGE_SET) {
                    prim->uv1.h.id = s->tpage;
                }
                if (s->flags & EFFECT_EMIT_CLUT_ADD) {
                    prim->uv0.h.id = prim->uv0.h.id + s->clut;
                } else if (s->flags & EFFECT_EMIT_CLUT_SET) {
                    prim->uv0.h.id = s->clut;
                }
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_ldv0(&verts[quad->idx3]);
                    gte_rtps();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    gte_stsxy(&prim->x3);
                    gte_avsz4();
                    if (prim->x3 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X3;
                    }
                    if (prim->y3 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y3;
                    }
                    if ((reject & EFFECT_CLIP_QUAD_X) != EFFECT_CLIP_QUAD_X &&
                        (reject & EFFECT_CLIP_QUAD_Y) != EFFECT_CLIP_QUAD_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_DEPTH_CUE) {
                            gte_ldrgb(&prim->rgb);
                            gte_lddp(s->depth);
                            gte_dpcs();
                            gte_strgb(&prim->rgb);
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            quad++;
        } while (i < count);
    }
    s->cursor = (s32 *)quad;
    return (EffectTri *)prim;
}

/**
 * @brief Emit the gouraud triangles the stream cursor points at.
 *
 * Same walk as @ref effectEmitTris with a colour per corner. When the build
 * asks for a depth cue all three corners go through the GTE together, which
 * also refreshes the first corner; otherwise the two extra corners are copied
 * straight out of the stream.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitGouraudTris(EffectPrimBuild *s, u32 *ot,
                                        s32 otShift, EffectTri *poly) {
    EffectEmitGouraudTri *tri;
    EffectGouraudTri *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectGouraudTri *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    tri = (EffectEmitGouraudTri *)next;
    if (count > 0) {
        do {
            do {
                gte_ldv3(&verts[tri->idx0], &verts[tri->idx1],
                         &verts[tri->idx2]);
            } while (0);
            gte_rtpt();
            setlen(prim, 6);
            colour = tri->colour0;
            prim->rgb0 = colour;
            if (s->flags & EFFECT_EMIT_G_SEMITRANS) {
                prim->rgb0 = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_G_OPAQUE) {
                prim->rgb0 &= ~EFFECT_PRIM_CODE(2);
            }
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_G_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_avsz3();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    if ((reject & EFFECT_CLIP_TRI_X) != EFFECT_CLIP_TRI_X &&
                        (reject & EFFECT_CLIP_TRI_Y) != EFFECT_CLIP_TRI_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_G_DEPTH_CUE) {
                            gte_ldrgb3(&tri->colour1, &tri->colour2,
                                       &prim->rgb0);
                            gte_lddp(s->depth);
                            gte_dpct();
                            gte_strgb3(&prim->rgb1, &prim->rgb2, &prim->rgb0);
                        } else {
                            prim->rgb1 = tri->colour1;
                            prim->rgb2 = tri->colour2;
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            tri++;
        } while (i < count);
    }
    s->cursor = (s32 *)tri;
    return (EffectTri *)prim;
}

/**
 * @brief Emit the gouraud quads the stream cursor points at.
 *
 * @ref effectEmitGouraudTris with a fourth corner: the first three vertices are
 * projected together and the fourth on its own, and the depth cue runs over
 * the three stream colours together before the prim's own colour separately.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitGouraudQuads(EffectPrimBuild *s, u32 *ot,
                                         s32 otShift, EffectTri *poly) {
    EffectEmitGouraudQuad *quad;
    EffectGouraudQuad *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectGouraudQuad *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    quad = (EffectEmitGouraudQuad *)next;
    if (count > 0) {
        do {
            do {
                gte_ldv3(&verts[quad->idx0], &verts[quad->idx1],
                         &verts[quad->idx2]);
            } while (0);
            gte_rtpt();
            setlen(prim, 8);
            colour = quad->colour0;
            prim->rgb0 = colour;
            if (s->flags & EFFECT_EMIT_G_SEMITRANS) {
                prim->rgb0 = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_G_OPAQUE) {
                prim->rgb0 &= ~EFFECT_PRIM_CODE(2);
            }
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_G_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_ldv0(&verts[quad->idx3]);
                    gte_rtps();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    gte_stsxy(&prim->x3);
                    gte_avsz4();
                    if (prim->x3 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X3;
                    }
                    if (prim->y3 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y3;
                    }
                    if ((reject & EFFECT_CLIP_QUAD_X) != EFFECT_CLIP_QUAD_X &&
                        (reject & EFFECT_CLIP_QUAD_Y) != EFFECT_CLIP_QUAD_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_G_DEPTH_CUE) {
                            gte_ldrgb3(&quad->colour1, &quad->colour2,
                                       &quad->colour3);
                            gte_lddp(s->depth);
                            gte_dpct();
                            gte_strgb3(&prim->rgb1, &prim->rgb2, &prim->rgb3);
                            gte_ldrgb(&prim->rgb0);
                            gte_dpcs();
                            gte_strgb(&prim->rgb0);
                        } else {
                            prim->rgb1 = quad->colour1;
                            prim->rgb2 = quad->colour2;
                            prim->rgb3 = quad->colour3;
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            quad++;
        } while (i < count);
    }
    s->cursor = (s32 *)quad;
    return (EffectTri *)prim;
}

/**
 * @brief Emit the gouraud textured triangles the stream cursor points at.
 *
 * @ref effectEmitGouraudTris with texture coordinates: the UV words come from the
 * stream biased by the build's offset, and the texture page and CLUT sharing
 * the first two are left alone, offset, or replaced as the flags ask.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitGouraudTexTris(EffectPrimBuild *s, u32 *ot,
                                           s32 otShift, EffectTri *poly) {
    EffectEmitGouraudTexTri *tri;
    EffectGouraudTexTri *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectGouraudTexTri *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    tri = (EffectEmitGouraudTexTri *)next;
    if (count > 0) {
        do {
            do {
                gte_ldv3(&verts[tri->idx0], &verts[tri->idx1],
                         &verts[(u16)tri->idx2uv2]);
            } while (0);
            gte_rtpt();
            setlen(prim, 9);
            colour = tri->colour0;
            prim->rgb0 = colour;
            if (s->flags & EFFECT_EMIT_G_SEMITRANS) {
                prim->rgb0 = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_G_OPAQUE) {
                prim->rgb0 &= ~EFFECT_PRIM_CODE(2);
            }
            prim->uv0.word = tri->uv0 + s->unk018;
            prim->uv1.word = tri->uv1 + s->unk018;
            prim->uv2.word = (tri->idx2uv2 >> 16) + s->unk018;
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                if (s->flags & EFFECT_EMIT_TPAGE_ADD) {
                    prim->uv1.h.id = prim->uv1.h.id + s->tpage;
                } else if (s->flags & EFFECT_EMIT_TPAGE_SET) {
                    prim->uv1.h.id = s->tpage;
                }
                if (s->flags & EFFECT_EMIT_CLUT_ADD) {
                    prim->uv0.h.id = prim->uv0.h.id + s->clut;
                } else if (s->flags & EFFECT_EMIT_CLUT_SET) {
                    prim->uv0.h.id = s->clut;
                }
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_G_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_avsz3();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    if ((reject & EFFECT_CLIP_TRI_X) != EFFECT_CLIP_TRI_X &&
                        (reject & EFFECT_CLIP_TRI_Y) != EFFECT_CLIP_TRI_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_G_DEPTH_CUE) {
                            gte_ldrgb3(&tri->colour1, &tri->colour2,
                                       &prim->rgb0);
                            gte_lddp(s->depth);
                            gte_dpct();
                            gte_strgb3(&prim->rgb1, &prim->rgb2, &prim->rgb0);
                        } else {
                            prim->rgb1 = tri->colour1;
                            prim->rgb2 = tri->colour2;
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            tri++;
        } while (i < count);
    }
    s->cursor = (s32 *)tri;
    return (EffectTri *)prim;
}

/**
 * @brief Emit the gouraud textured quads the stream cursor points at.
 *
 * The widest of the family: four corners, each with its own colour and UV
 * pair. @ref effectEmitGouraudTexTris with a fourth vertex.
 *
 * @param s        Emitter state: the stream cursor, the vertex buffer and the flags.
 * @param ot       Ordering table the prims are linked into.
 * @param otShift  Right shift applied to the GTE's average Z to pick the OT slot.
 * @param poly     Prim cursor to write from.
 * @return The prim cursor advanced past everything written.
 */
static EffectTri *effectEmitGouraudTexQuads(EffectPrimBuild *s, u32 *ot,
                                            s32 otShift, EffectTri *poly) {
    EffectEmitGouraudTexQuad *quad;
    EffectGouraudTexQuad *prim;
    s32 *cursor;
    s32 *next;
    u32 *verts;
    u32 colour;
    u32 uv;
    s32 count;
    s32 reject;
    s32 i;

    i = 0;
    prim = (EffectGouraudTexQuad *)poly;
    cursor = s->cursor;
    verts = s->verts;
    count = cursor[0];
    next = cursor + 1;
    s->cursor = next;
    quad = (EffectEmitGouraudTexQuad *)next;
    if (count > 0) {
        do {
            gte_ldv3(&verts[quad->idx0], &verts[quad->idx1],
                     &verts[quad->idx2]);
            gte_rtpt();
            setlen(prim, 0xC);
            colour = quad->colour0;
            prim->rgb0 = colour;
            if (s->flags & EFFECT_EMIT_G_SEMITRANS) {
                prim->rgb0 = colour | EFFECT_PRIM_CODE(2);
            }
            if (s->flags & EFFECT_EMIT_G_OPAQUE) {
                prim->rgb0 &= ~EFFECT_PRIM_CODE(2);
            }
            prim->uv0.word = quad->uv0 + s->unk018;
            prim->uv1.word = quad->uv1 + s->unk018;
            uv = quad->uv23 + (s->unk018 + (s->unk018 << 16));
            prim->uv2.word = uv;
            prim->uv3.word = uv >> 16;
            gte_stflg(&s->gteFlag);
            if (!(s->gteFlag & EFFECT_GTE_OUT_OF_RANGE)) {
                gte_nclip();
                if (s->flags & EFFECT_EMIT_TPAGE_ADD) {
                    prim->uv1.h.id = prim->uv1.h.id + s->tpage;
                } else if (s->flags & EFFECT_EMIT_TPAGE_SET) {
                    prim->uv1.h.id = s->tpage;
                }
                if (s->flags & EFFECT_EMIT_CLUT_ADD) {
                    prim->uv0.h.id = prim->uv0.h.id + s->clut;
                } else if (s->flags & EFFECT_EMIT_CLUT_SET) {
                    prim->uv0.h.id = s->clut;
                }
                reject = 0;
                gte_stopz(&s->nclip);
                if (s->nclip >= 0 || (s->flags & EFFECT_EMIT_G_TWO_SIDED)) {
                    gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
                    gte_ldv0(&verts[quad->idx3]);
                    gte_rtps();
                    if (prim->x0 >= EFFECT_EMIT_CLIP_X) {
                        reject = EFFECT_CLIP_X0;
                    }
                    if (prim->x1 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X1;
                    }
                    if (prim->x2 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X2;
                    }
                    if (prim->y0 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y0;
                    }
                    if (prim->y1 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y1;
                    }
                    if (prim->y2 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y2;
                    }
                    gte_stsxy(&prim->x3);
                    gte_avsz4();
                    if (prim->x3 >= EFFECT_EMIT_CLIP_X) {
                        reject |= EFFECT_CLIP_X3;
                    }
                    if (prim->y3 >= EFFECT_EMIT_CLIP_Y) {
                        reject |= EFFECT_CLIP_Y3;
                    }
                    if ((reject & EFFECT_CLIP_QUAD_X) != EFFECT_CLIP_QUAD_X &&
                        (reject & EFFECT_CLIP_QUAD_Y) != EFFECT_CLIP_QUAD_Y) {
                        gte_stotz(&s->otz);
                        if (s->flags & EFFECT_EMIT_G_DEPTH_CUE) {
                            gte_ldrgb3(&quad->colour1, &quad->colour2,
                                       &quad->colour3);
                            gte_lddp(s->depth);
                            gte_dpct();
                            gte_strgb3(&prim->rgb1, &prim->rgb2, &prim->rgb3);
                            gte_ldrgb(&prim->rgb0);
                            gte_dpcs();
                            gte_strgb(&prim->rgb0);
                        } else {
                            prim->rgb1 = quad->colour1;
                            prim->rgb2 = quad->colour2;
                            prim->rgb3 = quad->colour3;
                        }
                        addPrim(&ot[s->otz >> otShift], prim);
                        prim++;
                    }
                }
            }
            do {
                i++;
            } while (0);
            quad++;
        } while (i < count);
    }
    s->cursor = (s32 *)quad;
    return (EffectTri *)prim;
}

/**
 * @brief Walk @p prim's mesh stream, letting each emitter draw its own prims.
 *
 * @return The prim list head, advanced past everything the emitters wrote.
 */
EffectTri *effectEmitStream(EffectPrimBuild *prim, u32 *ot, s32 otShift, EffectTri *head) {
    if (!(prim->flags & EFFECT_EMIT_VERTS_SET)) {
        prim->verts = (u32 *)(prim->stream + 2);
    }
    prim->cursor = (s32 *)((u8 *)prim->stream + prim->stream[0]);
    if (!(prim->flags & EFFECT_EMIT_KEEP_DEPTH)) {
        prim->unk018 = 0;
    }
    gte_SetFarColor(prim->color.r, prim->color.g, prim->color.b);
    if (*prim->cursor != 0) {
        head = effectEmitTris(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitQuads(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitTexTris(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitTexQuads(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitGouraudTris(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitGouraudQuads(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitGouraudTexTris(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    if (*prim->cursor != 0) {
        head = effectEmitGouraudTexQuads(prim, ot, otShift, head);
    } else {
        prim->cursor++;
    }
    return head;
}
