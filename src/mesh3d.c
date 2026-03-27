#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "render.h"

extern MATRIX g_meshBaseMatrix;
extern MeshRenderCtx g_meshRenderCtx;  /**< Global mesh render context (overlay area). */

/**
 * @brief Set up GTE matrices and render a mesh grid.
 *
 * Applies rotation/translation matrix, transforms vertices through
 * the GTE, then renders an 8x8 textured quad grid via renderMeshGrid.
 *
 * @param mesh      Mesh render context with vertices, primitives, and OT.
 * @param matrix    Rotation matrix to apply (translation set from tx/ty params).
 * @param intensity Brightness scale factor.
 * @param tx        Translation X (stored to matrix->t[0]).
 * @param ty        Translation Y (stored to matrix->t[1], from stack).
 */
void renderMeshPanel(MeshRenderCtx *mesh, MATRIX *matrix, s32 intensity,
                    s32 tx, s32 ty) {
    register s32 *otPtr;
    s32 unused1;
    s32 unused2;
    s32 unused3;

    matrix->t[0] = tx;
    matrix->t[1] = ty;
    SetRotMatrix(matrix);
    SetTransMatrix(matrix);
    transformMeshVertices(mesh);
    otPtr = (s32 *)mesh->otBase;
    otPtr += 4;
    mesh->primPtr = renderMeshGrid(mesh->vertices, mesh->primPtr, otPtr, intensity, 1);
}


/**
 * @brief Copy global rotation matrix, apply uniform scale, and render mesh.
 *
 * Copies the 32-byte global rotation matrix g_meshBaseMatrix into a local copy,
 * applies a uniform scale factor, sets the GTE matrices, transforms vertices,
 * and renders the mesh grid with no per-vertex shading.
 *
 * @param mesh      Mesh render context.
 * @param ot        GPU ordering table.
 * @param scale     Uniform scale factor (applied to all 3 axes).
 * @param intensity Brightness scale factor.
 */
void renderScaledMesh(MeshRenderCtx *mesh, s32 *ot, s32 scale, s32 intensity) {
    register s32 i;
    register s32 *dst;
    register s32 *src;
    s32 scaleVec[3];
    MATRIX localMatrix;
    s32 unused1;
    s32 unused2;

    src = (s32 *)&g_meshBaseMatrix;
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
    transformMeshVertices(mesh);
    mesh->primPtr = renderMeshGrid(mesh->vertices, mesh->primPtr, ot, intensity, 0);
}


INCLUDE_ASM("asm/nonmatchings/mesh3d", renderFlatMesh);


/**
 * @brief Initialize 9x9 vertex grid and ordering tables for mesh rendering.
 *
 * Sets up the mesh render context at g_meshRenderCtx with buffer pointers,
 * clears two 8-entry ordering tables at 0x801F6000, and fills a 9x9 grid
 * of screen-space vertex positions with uniform spacing (40px h, 27px v).
 */
void initMeshRenderer(void) {
    register MeshRenderCtx *ctx;
    register s32 *ot;
    register s32 col;
    register s32 row;
    register ScreenVert *verts;
    s32 unused1;
    s32 unused2;
    s32 unused3;
    s32 unused4;

    ctx = MESH_RENDER_CTX;
    ctx->pad00 = 0;
    ctx->normals = (s32 *)MESH_INPUT_VERTS;
    ctx->vertices = MESH_SCREEN_VERTS;
    ot = (s32 *)MESH_OT_BASE;
    ClearOTag((u32 *)ot, 8);
    ClearOTag((u32 *)(ot + 8), 8);
    verts = (ScreenVert *)ctx->normals;
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


INCLUDE_ASM("asm/nonmatchings/mesh3d", meshRenderTick);
