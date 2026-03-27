#ifndef RENDER_H
#define RENDER_H

#include "common.h"

/** @brief Screen vertex (8 bytes). Packed x,y screen coordinates from GTE. */
typedef struct {
    s32 xy;             /**< Packed x:16, y:16 screen coordinates. */
    s32 pad;
} ScreenVert;

/** @brief POLY_GT4: gouraud-textured 4-point polygon (52 bytes). */
typedef struct {
    /* 0x00 */ u8 tag[3];       /**< P_TAG address (24 bits). */
    /* 0x03 */ u8 len;          /**< P_TAG word count. */
    /* 0x04 */ s32 color0;      /**< Vertex 0 color (R,G,B,code). */
    /* 0x08 */ s32 vert0;       /**< Vertex 0 screen XY. */
    /* 0x0C */ u16 uv0;         /**< Vertex 0 UV coordinates. */
    /* 0x0E */ u16 clut;        /**< CLUT id. */
    /* 0x10 */ s32 color1;      /**< Vertex 1 color. */
    /* 0x14 */ s32 vert1;       /**< Vertex 1 screen XY. */
    /* 0x18 */ u16 uv1;         /**< Vertex 1 UV coordinates. */
    /* 0x1A */ u16 tpage;       /**< Texture page id. */
    /* 0x1C */ s32 color2;      /**< Vertex 2 color. */
    /* 0x20 */ s32 vert2;       /**< Vertex 2 screen XY. */
    /* 0x24 */ u16 uv2;         /**< Vertex 2 UV coordinates. */
    /* 0x26 */ u16 pad26;
    /* 0x28 */ s32 color3;      /**< Vertex 3 color. */
    /* 0x2C */ s32 vert3;       /**< Vertex 3 screen XY. */
    /* 0x30 */ u16 uv3;         /**< Vertex 3 UV coordinates. */
    /* 0x32 */ u16 pad32;
} PolyGT4; /* 0x34 = 52 bytes */

/** @brief Mesh render context for GTE transformation and GPU primitive generation. */
typedef struct {
    /* 0x00 */ s32 pad00;
    /* 0x04 */ PolyGT4 *primPtr;    /**< Current primitive write pointer. */
    /* 0x08 */ ScreenVert *vertices; /**< Vertex position array. */
    /* 0x0C */ s32 *normals;        /**< Vertex normal array. */
    /* 0x10 */ s32 *otBase;         /**< Ordering table base pointer. */
} MeshRenderCtx;

/**
 * Mesh renderer overlay working memory (fixed addresses).
 *
 * These are hardcoded because PSYLINK (the PsyQ linker) strips
 * redundant `addiu reg, reg, 0` when resolving %lo relocations to 0,
 * but the GNU linker does not. Using `&g_meshRenderCtx` would add
 * an extra instruction in our build pipeline.
 */
#define MESH_RENDER_CTX     ((MeshRenderCtx *)0x801F0000)
#define MESH_INPUT_VERTS    ((ScreenVert *)0x801F0400)
#define MESH_SCREEN_VERTS   ((ScreenVert *)0x801F1000)
#define MESH_OT_BASE        ((u32 *)0x801F6000)

/* --- render3d.c --- */

void transformMeshVertices(MeshRenderCtx *mesh);
PolyGT4 *renderMeshGrid(ScreenVert *vertices, PolyGT4 *primBuf, s32 *ot,
                         s32 intensity, s32 perVertex);

#endif /* RENDER_H */
