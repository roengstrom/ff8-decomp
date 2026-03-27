#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "render.h"

#define GRID_SIZE   8   /**< Quads per row/column in the mesh grid. */
#define GRID_VERTS  9   /**< Vertices per row (GRID_SIZE + 1). */

extern u8 g_meshIntensityA[];  /**< Intensity table A (indexed by col, then row). */
extern u8 g_meshIntensityB[];  /**< Intensity table B (indexed by col, then row). */
extern u8 g_meshUCoord[];  /**< U coord per column. */
extern u8 g_meshVCoord[];  /**< V coord per column. */
extern u8 g_meshUPage[];  /**< U page offset per row (shifted <<8). */
extern u8 g_meshVPage[];  /**< V page offset per row (shifted <<8). */
extern u8 g_meshTpage[];  /**< Texture page ID per column. */

/**
 * @brief Transform 81 vertex/normal pairs through the GTE.
 * @param mesh Mesh data containing vertex and normal array pointers.
 */
void transformMeshVertices(MeshRenderCtx *mesh) {
    register s32 *normals = mesh->normals;
    register s32 *vertices = mesh->vertices;
    register s32 i = 0;
    s32 screenXY;
    s32 flag;
    s32 interpZ;

    while (i < 81) {
        RotTransPers(normals, vertices, &screenXY, &screenXY);
        normals += 2;
        vertices += 2;
        i++;
    }
}


/**
 * @brief Render an 8x8 grid of gouraud-textured quads.
 *
 * Generates 64 POLY_GT4 GPU primitives from a 9x9 vertex grid,
 * applying per-vertex or uniform lighting from intensity tables.
 *
 * @param vertices  9x9 vertex position grid (stride 8 bytes per vertex).
 * @param primBuf   Output primitive buffer.
 * @param ot        GPU ordering table to insert primitives into.
 * @param intensity Brightness scale factor (8.8 fixed point).
 * @param perVertex If nonzero, use per-vertex shading; else uniform gray.
 * @return Pointer past the last written primitive.
 */
PolyGT4 *renderMeshGrid(ScreenVert *vertices, PolyGT4 *primBuf, s32 *ot,
                        s32 intensity, s32 perVertex) {
    register PolyGT4 *prim = primBuf;
    register s32 r;
    register s32 minVal;
    register s32 uvHiU;
    register s32 uvHiV;
    register s32 col;
    register s32 row;
    register ScreenVert *mesh;
    char pad;

    mesh = vertices;
    row = 0;

    while (row < GRID_SIZE) {
        col = 0;
        while (col < GRID_SIZE) {
            prim->len = 0x0C;
            prim->tpage = g_meshTpage[col] | 0x120;

            if (perVertex) {
                /* Vertex 0: min(tableA[col], tableA[row]) * intensity */
                r = g_meshIntensityA[col];
                minVal = g_meshIntensityA[row];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color0 = r | (0x3E << 24);

                /* Vertex 1: min(tableB[col], tableA[row]) * intensity */
                r = g_meshIntensityB[col];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color1 = r;

                /* Vertex 2: min(tableA[col], tableB[row]) * intensity */
                r = g_meshIntensityA[col];
                minVal = g_meshIntensityB[row];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color2 = r;

                /* Vertex 3: min(tableB[col], tableB[row]) * intensity */
                r = g_meshIntensityB[col];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color3 = r;
            } else {
                /* Uniform gray */
                r = (intensity / 2) & 0xFF;
                r = (r | (r << 8)) | (r << 16);
                r = r | (0x3E << 24);
                prim->color0 = prim->color1 = prim->color2 = prim->color3 = r;
            }

            /* Copy vertex positions from the 9-wide grid */
            prim->vert0 = mesh[0].xy;
            prim->vert1 = mesh[1].xy;
            prim->vert2 = mesh[GRID_VERTS].xy;
            prim->vert3 = mesh[GRID_VERTS + 1].xy;

            /* UV coordinates from lookup tables */
            r = g_meshUCoord[col];
            minVal = g_meshVCoord[col];
            uvHiU = g_meshUPage[row] << 8;
            uvHiV = g_meshVPage[row] << 8;

            prim->uv0 = r | uvHiU;
            prim->uv1 = minVal | uvHiU;
            prim->uv2 = r | uvHiV;
            prim->uv3 = minVal | uvHiV;

            AddPrim(ot, prim);
            prim++;
            mesh++;
            col++;
        }
        mesh++;
        row++;
    }
    return prim;
}
