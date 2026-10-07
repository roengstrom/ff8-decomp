/**
 * @file effect_066.c
 * @brief Griever + Ultimecia Death
 */
#include "common.h"
#include "effect.h"
#include "effect/effect_066.h"

#define GRIEVER_DETACH_ORDER_RANGE 320
#define GRIEVER_DETACH_HEIGHT -6000

typedef struct {
    /* 0x00 */ EffectMeshTri *triangles;
    /* 0x04 */ SVECTOR *vertices;
    /* 0x08 */ u16 triangleCount;
    /* 0x0A */ u16 quadCount;
    /* 0x0C */ u8 pad00C[0x2C - 0x0C];
    /* 0x2C */ u16 *faceState;
    /* 0x30 */ u8 pad030[0x58 - 0x30];
    /* 0x58 */ SVECTOR faceVertices[4];
} GrieverMesh;

static void func_801A6EAC(GrieverMesh *mesh);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A0000);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A017C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A0688);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A0C8C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A131C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A1B3C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A1DD4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A20AC);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A23DC);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A2770);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A2B60);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A2FBC);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A350C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A3A1C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A402C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A4270);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A42A4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A42F4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A43EC);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A52CC);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A533C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A56E0);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A5B24);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A5CB8);

/** @brief Initializes the height-based order in which mesh faces detach. */
static void func_801A6EAC(GrieverMesh *mesh) {
    u16 *faceState = mesh->faceState;
    EffectMeshTri *triangles = mesh->triangles;
    SVECTOR *vertices = mesh->vertices;
    EffectMeshQuad *quads;
    s32 i;

    for (i = 0; i < mesh->triangleCount; i++, triangles++) {
        mesh->faceVertices[1] = vertices[triangles->idx1 & EFFECT_MESH_INDEX_MASK];
        *faceState++ |= GRIEVER_DETACH_ORDER_RANGE -
            mesh->faceVertices[1].vy * GRIEVER_DETACH_ORDER_RANGE / GRIEVER_DETACH_HEIGHT;
    }

    /* The quad records immediately follow the triangle records in the mesh stream. */
    quads = (EffectMeshQuad *)triangles;
    for (i = 0; i < mesh->quadCount; i++, quads++) {
        mesh->faceVertices[1] = vertices[quads->idx1 & EFFECT_MESH_INDEX_MASK];
        *faceState++ |= GRIEVER_DETACH_ORDER_RANGE -
            mesh->faceVertices[1].vy * GRIEVER_DETACH_ORDER_RANGE / GRIEVER_DETACH_HEIGHT;
    }
    mesh->faceState = faceState;
}

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7008);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A729C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7370);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A739C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A73C0);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A74F8);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7568);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A78D4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7974);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7BC8);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7E04);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A7E60);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A8480);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A85F8);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A8AA4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A905C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A9574);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A9904);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A9A08);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A9AB4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801A9F5C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AA0D4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AA16C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AA39C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AA5B8);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AA778);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AA828);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AAD08);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AB164);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AB228);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AB38C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AB3F4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AB540);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AB5A0);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801ABB60);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801ABBD8);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801ABE0C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801ABE6C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801ABFF4);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AC04C);

INCLUDE_ASM("asm/ovl/effect_066/nonmatchings/effect_066", func_801AC818);
