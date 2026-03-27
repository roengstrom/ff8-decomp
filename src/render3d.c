#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/** @brief Vertex position pair (XY screen coordinates, 8 bytes). */
typedef struct {
    s32 xy;             /**< Packed x:16, y:16 screen coordinates. */
    s32 pad;
} ScreenVert;

#define GRID_SIZE   8   /**< Quads per row/column in the mesh grid. */
#define GRID_VERTS  9   /**< Vertices per row (GRID_SIZE + 1). */

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

void RotTransPers(s32 *v0, s32 *v1, s32 *sxy, s32 *p);

extern u8 D_80052924[];  /**< Intensity table A (indexed by col, then row). */
extern u8 D_80052925[];  /**< Intensity table B (indexed by col, then row). */
extern u8 D_80052930[];  /**< U coord per column. */
extern u8 D_80052938[];  /**< V coord per column. */
extern u8 D_80052940[];  /**< U page offset per row (shifted <<8). */
extern u8 D_80052941[];  /**< V page offset per row (shifted <<8). */
extern u8 D_8005294C[];  /**< Texture page ID per column. */

void AddPrim(s32 *ot, void *prim);

/**
 * @brief Transform 81 vertex/normal pairs through the GTE.
 * @param mesh Mesh data containing vertex and normal array pointers.
 */
void func_80025920(MeshRenderCtx *mesh) {
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
PolyGT4 *func_800259C0(ScreenVert *vertices, PolyGT4 *primBuf, s32 *ot,
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
            prim->tpage = D_8005294C[col] | 0x120;

            if (perVertex) {
                /* Vertex 0: min(tableA[col], tableA[row]) * intensity */
                r = D_80052924[col];
                minVal = D_80052924[row];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color0 = r | (0x3E << 24);

                /* Vertex 1: min(tableB[col], tableA[row]) * intensity */
                r = D_80052925[col];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color1 = r;

                /* Vertex 2: min(tableA[col], tableB[row]) * intensity */
                r = D_80052924[col];
                minVal = D_80052925[row];
                if (minVal < r) r = minVal;
                r = (r * intensity) / 256;
                r &= 0xFF;
                r = (r | (r << 8)) | (r << 16);
                prim->color2 = r;

                /* Vertex 3: min(tableB[col], tableB[row]) * intensity */
                r = D_80052925[col];
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
            r = D_80052930[col];
            minVal = D_80052938[col];
            uvHiU = D_80052940[row] << 8;
            uvHiV = D_80052941[row] << 8;

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
