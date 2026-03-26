#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/** @brief Input mesh data for GTE vertex transformation. */
typedef struct {
    s32 pad00;
    s32 pad04;
    s32 *vertices;    /**< +0x08: Vertex position array (stride 8 bytes). */
    s32 *normals;     /**< +0x0C: Vertex normal array (stride 8 bytes). */
} GteMeshData;

void RotTransPers(s32 *v0, s32 *v1, s32 *sxy, s32 *p);

/**
 * @brief Transform 81 vertex/normal pairs through the GTE.
 *
 * Iterates over 81 entries, calling RotTransPers on each
 * vertex/normal pair to project 3D coordinates to screen space.
 *
 * @param mesh Mesh data containing vertex and normal array pointers.
 */
void func_80025920(GteMeshData *mesh) {
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


INCLUDE_ASM("asm/nonmatchings/16120", func_800259C0);


INCLUDE_ASM("asm/nonmatchings/16120", func_80025D80);


INCLUDE_ASM("asm/nonmatchings/16120", func_80025E4C);


INCLUDE_ASM("asm/nonmatchings/16120", func_80025F58);


INCLUDE_ASM("asm/nonmatchings/16120", func_800262B0);


INCLUDE_ASM("asm/nonmatchings/16120", func_800263DC);
