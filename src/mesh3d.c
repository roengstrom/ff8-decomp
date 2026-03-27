#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/** @brief Vertex position pair (XY screen coordinates, 8 bytes). */
typedef struct {
    s32 xy;             /**< Packed x:16, y:16 screen coordinates. */
    s32 pad;
} ScreenVert;

/** @brief PS1 POLY_GT4 GPU primitive (gouraud-textured 4-point polygon, 52 bytes). */
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
    /* 0x04 */ PolyGT4 *primPtr;    /**< Current primitive write pointer (advances after rendering). */
    /* 0x08 */ ScreenVert *vertices; /**< Vertex position array (stride 8 bytes). */
    /* 0x0C */ s32 *normals;        /**< Vertex normal array (stride 8 bytes). */
    /* 0x10 */ s32 *otBase;         /**< Ordering table base pointer. */
} MeshRenderCtx;

/** @brief Rotation matrix with translation vector. */
typedef struct {
    /* 0x00 */ s16 m[3][3];    /**< 3x3 rotation matrix (fixed point). */
    /* 0x12 */ s16 pad12;
    /* 0x14 */ s32 tx;         /**< Translation X. */
    /* 0x18 */ s32 ty;         /**< Translation Y. */
    /* 0x1C */ s32 tz;         /**< Translation Z. */
} RotMatrix;

extern RotMatrix D_800528F4;
extern MeshRenderCtx D_801F0000;  /**< Global mesh render context (overlay area). */
void ScaleMatrix(void *m, void *scale);
void SetRotMatrix(void *m);
void SetTransMatrix(void *m);
void func_80025920(MeshRenderCtx *mesh);
PolyGT4 *func_800259C0(ScreenVert *verts, PolyGT4 *prims, s32 *ot, s32 intensity, s32 perVertex);

/**
 * @brief Set up GTE matrices and render a mesh grid.
 *
 * Applies rotation/translation matrix, transforms vertices through
 * the GTE, then renders an 8x8 textured quad grid via func_800259C0.
 *
 * @param mesh      Mesh render context with vertices, primitives, and OT.
 * @param matrix    Rotation matrix to apply (translation set from tx/ty params).
 * @param intensity Brightness scale factor.
 * @param tx        Translation X (stored to matrix->tx).
 * @param ty        Translation Y (stored to matrix->ty, from stack).
 */
void func_80025D80(MeshRenderCtx *mesh, RotMatrix *matrix, s32 intensity,
                    s32 tx, s32 ty) {
    register s32 *otPtr;
    s32 unused1;
    s32 unused2;
    s32 unused3;

    matrix->tx = tx;
    matrix->ty = ty;
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    func_80025920(mesh);
    otPtr = (s32 *)mesh->otBase;
    otPtr += 4;
    mesh->primPtr = func_800259C0(mesh->vertices, mesh->primPtr, otPtr, intensity, 1);
}


/**
 * @brief Copy global rotation matrix, apply uniform scale, and render mesh.
 *
 * Copies the 32-byte global rotation matrix D_800528F4 into a local copy,
 * applies a uniform scale factor, sets the GTE matrices, transforms vertices,
 * and renders the mesh grid with no per-vertex shading.
 *
 * @param mesh      Mesh render context.
 * @param ot        GPU ordering table.
 * @param scale     Uniform scale factor (applied to all 3 axes).
 * @param intensity Brightness scale factor.
 */
void func_80025E4C(MeshRenderCtx *mesh, s32 *ot, s32 scale, s32 intensity) {
    register s32 i;
    register s32 *dst;
    register s32 *src;
    s32 scaleVec[3];
    RotMatrix localMatrix;
    s32 unused1;
    s32 unused2;

    src = (s32 *)&D_800528F4;
    dst = (s32 *)&localMatrix;
    i = 0;
    while (i < 32) {
        *(s32 *)((s32)dst + i) = *(s32 *)((s32)src + i);
        i += 4;
    }

    scaleVec[0] = scaleVec[1] = scaleVec[2] = scale;
    ScaleMatrix(dst, scaleVec);
    SetRotMatrix(dst);
    SetTransMatrix(dst);
    func_80025920(mesh);
    mesh->primPtr = func_800259C0(mesh->vertices, mesh->primPtr, ot, intensity, 0);
}


INCLUDE_ASM("asm/nonmatchings/mesh3d", func_80025F58);


/**
 * @brief Initialize 9x9 vertex grid and ordering tables for mesh rendering.
 *
 * Sets up the mesh render context at D_801F0000 with buffer pointers,
 * clears two 8-entry ordering tables at 0x801F6000, and fills a 9x9 grid
 * of screen-space vertex positions with uniform spacing (40px h, 27px v).
 */
void func_800262B0(void) {
    register s32 base;
    register s32 *ot;
    register s32 col;
    register s32 row;
    register ScreenVert *verts;
    s32 unused1;
    s32 unused2;
    s32 unused3;
    s32 unused4;

    base = (s32)0x801F0000;     /* D_801F0000 — MeshRenderCtx */
    *(s32 *)base = 0;           /* pad00 (page index) = 0 */
    *(s32 *)(base + 0xC) = (s32)0x801F0400;  /* normals buffer */
    *(s32 *)(base + 0x8) = (s32)0x801F1000;  /* vertex buffer */
    ot = (s32 *)0x801F6000;     /* OT base */
    ClearOTag((u32 *)ot, 8);
    ClearOTag((u32 *)(ot + 8), 8);
    verts = (ScreenVert *)*(s32 *)(base + 0xC);
    row = 0;
    while (row < 9) {
        col = 0;
        while (col < 9) {
            *(s16 *)&verts->xy = col * 40 - 160;       /* x */
            *((s16 *)&verts->xy + 1) = row * 27 - 108; /* y */
            verts->pad = 0;
            verts++;
            col++;
        }
        row++;
    }
}


INCLUDE_ASM("asm/nonmatchings/mesh3d", func_800263DC);
