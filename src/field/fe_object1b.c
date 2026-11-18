/**
 * @file fe_object1b.c
 *
 * The PsyQ 4.3 (gcc 2.8.0) island that sits between @c func_800A63AC and
 * the end of the original fe_object1 translation unit. The functions in
 * this file are originally compiled with the PsyQ 4.3 toolchain — their
 * FILLED branch-delay-slot epilogues (`jr ra; addiu sp` instead of PsyQ
 * 4.1's `addiu sp; jr ra; nop`) are the dead giveaway. The rest of
 * fe_object1 is PsyQ 4.1, so these had to live in a separate translation
 * unit (added to @c PSYQ43_SRCS in the Makefile).
 *
 * Everything below was originally INCLUDE_ASM in fe_object1.c with notes
 * to "split the file" — this is that split.
 */

#include "common.h"
#include "cdrom.h"
#include "field.h"
#include "lzs.h"
#include "psxsdk/libapi.h"
#include "psxsdk/libc.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "field/fe_object1.h"
#include "field/fe_object1b.h"
#include "field/fe_object2.h"
#include "field/fe_object3.h"
#include "field/fe_object4.h"

typedef struct {
    /** 0x00 */ s32 unk0;
    /** 0x04 */ u8 pad4[0x04];
    /** 0x08 */ s16 unk8;
    /** 0x0A */ u8 padA[0x02];
    /** 0x0C */ u16 unkC;
    /** 0x0E */ u16 unkE;
    /** 0x10 */ u16 unk10;
    /** 0x12 */ u16 unk12;
    /** 0x14 */ u8 pad14[0x09];
    /** 0x1D */ u8 unk1D;
    /** 0x1E */ u8 pad1E[0x0E];
    /** 0x2C */ u16 unk2C;
    /** 0x2E */ u16 unk2E;
    /** 0x30 */ u16 unk30;
    /** 0x32 */ u16 unk32;
    /** 0x34 */ u8 pad34[0x02];
    /** 0x36 */ s16 unk36;
    /** 0x38 */ u8 pad38[0x08];
} func_800A7564_entry;

typedef union {
    struct {
        /** 0x0 */ u16 unk0;
        /** 0x2 */ u16 unk2;
    } half;
    u8 byte[4];
} func_800A7564_unk58;

typedef struct {
    /** 0x00 */ s32 unk0;
    /** 0x04 */ s32 unk4;
    /** 0x08 */ s32 unk8;
    /** 0x0C */ s32 unkC;
    /** 0x10 */ s32 unk10[6];
    /** 0x28 */ s32 unk28;
    /** 0x2C */ s32 unk2C;
} func_800AA8A0_node;

typedef struct {
    /** 0x0 */ s16 unk0;
    /** 0x2 */ s16 unk2;
    /** 0x4 */ s16 unk4;
    /** 0x6 */ s16 pad6;
} func_800AA8A0_pos;

typedef struct {
    /** 0x0 */ u8 pad0[4];
    /** 0x4 */ s16 unk4;
    /** 0x6 */ s16 unk6;
} func_800AA8A0_tim_unkC;

typedef struct {
    /** 0x00 */ s32 unk0;
    /** 0x04 */ s32 unk4;
    /** 0x08 */ u32 *unk8;
    /** 0x0C */ func_800AA8A0_tim_unkC *unkC;
    /** 0x10 */ u32 *unk10;
} func_800AA8A0_tim;

#define FUNC_800AA8A0_COLOR_BIT15 0x8000

extern u32 D_800C3768;
extern s32 D_800C376C;

extern POLY_G3 *D_800D5EE0[2];
extern POLY_G4 *D_800D5EE8[2];
extern POLY_FT3 *D_800D6600[2];
extern POLY_FT4 *D_800D6610[2];
extern u16 D_800D6300[][6];
extern MchModelHeader *D_800D60A0;
extern MchBone *D_800D60A4;
extern u16 D_800D6720[];
extern u16 D_800D9328[][6];
extern u32 D_800D96B0[];
extern SVECTOR *D_800DD6C0;
extern long D_800DD6C8;
extern func_800AA8A0_node D_800DD6D0[];
extern u8 D_800D9628[];
extern u8 D_800DD6C4[];
extern u16 D_800D8728[][2][0x100];
extern u16 D_800DE2D0[];
extern u8 D_800D5EF0[];
extern u8 D_800D60A8[];
extern s32 D_800DD6BC;
extern s16 D_800D60F8[];

void func_800A63AC(FieldFrameBuf *arg0, FieldView *arg1, s32 arg2) {
    s32 unused[12];
    s32 i;
    s32 j;
    s32 groupIndex;
    s32 primIndex;
    s32 triangleIndex;
    s32 quadIndex;
    POLY_G3 *currentPrims;
    POLY_G3 *otherPrims;
    POLY_FT3 *trianglePrims;
    POLY_FT3 *otherTrianglePrims;
    POLY_FT4 *quadPrims;
    POLY_FT4 *otherQuadPrims;
    s32 untexturedTriangleBytes;
    s32 untexturedQuadBytes;

    for (i = 0; i < g_fieldActorCount; i++) {
        if (D_800D9630[i]->modelSlot.modelId != NO_MODEL) {
            func_800AA5F8(i);
        }
    }

    for (i = 0; i < g_fieldActorCount; i++) {
        if (D_800D9630[i]->modelSlot.modelId != NO_MODEL) {
            if (D_800D9630[i]->renderFlags & ENTITY_RENDER_FLAG_10) {
                D_800D9630[i]->renderFlags &= ~ENTITY_RENDER_FLAG_10;
                D_800D9630[i]->renderFlags |= ENTITY_RENDER_FLAG_8;
                otherPrims = D_800D9630[i]->primBuffers[0];
                currentPrims = D_800D9630[i]->prims;
                if (currentPrims == otherPrims) {
                    otherPrims = D_800D9630[i]->primBuffers[1];
                }
                primIndex = 0;
                triangleIndex = 0;
                quadIndex = 0;
                untexturedTriangleBytes = D_800D9630[i]->modelSlot.model->unk30 * sizeof(POLY_G3);
                untexturedQuadBytes = D_800D9630[i]->modelSlot.model->unk34 * sizeof(POLY_G4);
                trianglePrims = (POLY_FT3 *)((u8 *)currentPrims + untexturedTriangleBytes + untexturedQuadBytes);
                otherTrianglePrims = (POLY_FT3 *)((u8 *)otherPrims + untexturedTriangleBytes + untexturedQuadBytes);
                for (groupIndex = 0; groupIndex < D_800D9630[i]->modelSlot.model->drawGroupCount; groupIndex++) {
                    for (j = 0; j < D_800D9630[i]->modelSlot.model->drawGroups[groupIndex].triangleCount; j++) {
                        if (D_800D9630[i]->primOrderIndices[primIndex] != 0) {
                            otherTrianglePrims[triangleIndex] = trianglePrims[triangleIndex];
                        }
                        triangleIndex++;
                        primIndex++;
                    }
                    quadPrims = (POLY_FT4 *)(trianglePrims + D_800D9630[i]->modelSlot.model->triangleCount);
                    otherQuadPrims = (POLY_FT4 *)(otherTrianglePrims + D_800D9630[i]->modelSlot.model->triangleCount);
                    for (j = 0; j < D_800D9630[i]->modelSlot.model->drawGroups[groupIndex].quadCount; j++) {
                        if (D_800D9630[i]->primOrderIndices[primIndex] != 0) {
                            otherQuadPrims[quadIndex] = quadPrims[quadIndex];
                        }
                        quadIndex++;
                        primIndex++;
                    }
                }
            }
            if (!(D_800D9630[i]->renderFlags & RENDER_FLAG_HIDDEN) && D_800D9630[i]->animationStart == D_800D9630[i]->cachedAnimationStart && D_800D9630[i]->animationPhase == D_800D9630[i]->cachedPhase) {
                if (!(D_800D9630[i]->renderFlags & (ENTITY_RENDER_FLAG_8 | ENTITY_RENDER_FLAG_10))) {
                    D_800D9630[i]->renderFlags |= ENTITY_RENDER_FLAG_10;
                }
            } else {
                D_800D9630[i]->renderFlags &= ~(ENTITY_RENDER_FLAG_8 | ENTITY_RENDER_FLAG_10);
                D_800D9630[i]->cachedAnimationStart = D_800D9630[i]->animationStart;
                D_800D9630[i]->cachedPhase = D_800D9630[i]->animationPhase;
            }
        }
    }

    func_800ACBD4(&arg0->ot[1], arg1);

    for (i = 0; i < g_fieldActorCount; i++) {
        if (D_800D9630[i]->modelSlot.modelId != NO_MODEL && (D_800D9630[i]->renderFlags & ENTITY_RENDER_FLAG_8)) {
            primIndex = 0;
            triangleIndex = 0;
            quadIndex = 0;
            trianglePrims = (POLY_FT3 *)((POLY_G4 *)(D_800D9630[i]->prims + D_800D9630[i]->modelSlot.model->unk30) + D_800D9630[i]->modelSlot.model->unk34);
            for (groupIndex = 0; groupIndex < D_800D9630[i]->modelSlot.model->drawGroupCount; groupIndex++) {
                for (j = 0; j < D_800D9630[i]->modelSlot.model->drawGroups[groupIndex].triangleCount; j++) {
                    if (D_800D9630[i]->primOrderIndices[primIndex] != 0) {
                        setaddr(&trianglePrims[triangleIndex], getaddr(&arg0->ot[D_800D9630[i]->primOrderIndices[primIndex]]));
                        setaddr(&arg0->ot[D_800D9630[i]->primOrderIndices[primIndex]], &trianglePrims[triangleIndex]);
                    }
                    primIndex++;
                    triangleIndex++;
                }
                quadPrims = (POLY_FT4 *)(trianglePrims + D_800D9630[i]->modelSlot.model->triangleCount);
                for (j = 0; j < D_800D9630[i]->modelSlot.model->drawGroups[groupIndex].quadCount; j++) {
                    if (D_800D9630[i]->primOrderIndices[primIndex] != 0) {
                        setaddr(&quadPrims[quadIndex], getaddr(&arg0->ot[D_800D9630[i]->primOrderIndices[primIndex]]));
                        setaddr(&arg0->ot[D_800D9630[i]->primOrderIndices[primIndex]], &quadPrims[quadIndex]);
                    }
                    primIndex++;
                    quadIndex++;
                }
            }
        }
    }
}

void func_800A6A80(s32 arg0, s32 arg1, ModelInstance **arg2) {
    MATRIX m;
    SVECTOR groupMoveStep;
    VECTOR rotatedOffset;
    u16 firstTriangleIndex;
    u16 firstQuadIndex;
    s32 primIndex;
    s32 i;
    s32 groupVertexIndex;
    s32 skinObjectIndex;
    MchDrawGroup *group;
    s32 j;
    ModelInstance *inst;
    Model *model;
    s32 firstVertexIndex;

    firstTriangleIndex = 0;
    firstQuadIndex = 0;
    primIndex = 0;
    for (i = 0; i < (*arg2)->modelSlot.model->drawGroupCount; i++) {
        groupVertexIndex = 0;
        for (skinObjectIndex = (*arg2)->modelSlot.model->drawGroups[i].firstSkinObjectIndex; skinObjectIndex < (*arg2)->modelSlot.model->drawGroups[i].firstSkinObjectIndex + (*arg2)->modelSlot.model->drawGroups[i].skinObjectCount; skinObjectIndex++) {
            if ((*arg2)->modelSlot.model->drawGroups[i].unk8.pad & MCH_DRAW_GROUP_FLAG_1) {
                m.t[0] = 0;
                m.t[1] = 0;
                m.t[2] = 0;
                SetRotMatrix(&(*arg2)->bones[(*arg2)->modelSlot.model->skinObjects[skinObjectIndex].boneId].transform);
                SetTransMatrix(&m);
                group = &(*arg2)->modelSlot.model->drawGroups[i];
                groupMoveStep.vx = (group->unk4 * (group->unk18.vx - group->unk8.vx)) / group->unk18.pad;
                groupMoveStep.vy = (group->unk4 * (group->unk18.vy - group->unk8.vy)) / group->unk18.pad;
                groupMoveStep.vz = (group->unk4 * (group->unk18.vz - group->unk8.vz)) / group->unk18.pad;
                RotTrans(&groupMoveStep, &rotatedOffset, &D_800DD6C8);
                m = (*arg2)->bones[(*arg2)->modelSlot.model->skinObjects[skinObjectIndex].boneId].transform;
                m.t[0] += rotatedOffset.vx;
                m.t[1] += rotatedOffset.vy;
                m.t[2] += rotatedOffset.vz;
                RotTrans(&group->unk8, &rotatedOffset, &D_800DD6C8);
                m.t[0] += rotatedOffset.vx;
                m.t[1] += rotatedOffset.vy;
                m.t[2] += rotatedOffset.vz;
                SetRotMatrix(&m);
                SetTransMatrix(&m);
                if (group->unk18.pad < ++group->unk4) {
                    group->unk8.pad = (group->unk8.pad ^ MCH_DRAW_GROUP_FLAG_1) | MCH_DRAW_GROUP_FLAG_4;
                } else {
                    (*arg2)->cachedAnimationStart = (SVECTOR *)-1;
                }
            } else if ((*arg2)->modelSlot.model->drawGroups[i].unk8.pad & MCH_DRAW_GROUP_FLAG_4) {
                m.t[0] = 0;
                m.t[1] = 0;
                m.t[2] = 0;
                SetRotMatrix(&(*arg2)->bones[(*arg2)->modelSlot.model->skinObjects[skinObjectIndex].boneId].transform);
                SetTransMatrix(&m);
                group = &(*arg2)->modelSlot.model->drawGroups[i];
                RotTrans(&group->unk18, &rotatedOffset, &D_800DD6C8);
                m = (*arg2)->bones[(*arg2)->modelSlot.model->skinObjects[skinObjectIndex].boneId].transform;
                m.t[0] += rotatedOffset.vx;
                m.t[1] += rotatedOffset.vy;
                m.t[2] += rotatedOffset.vz;
                SetRotMatrix(&m);
                SetTransMatrix(&m);
            } else {
                SetRotMatrix(&(*arg2)->bones[(*arg2)->modelSlot.model->skinObjects[skinObjectIndex].boneId].transform);
                SetTransMatrix(&(*arg2)->bones[(*arg2)->modelSlot.model->skinObjects[skinObjectIndex].boneId].transform);
            }
            inst = *arg2;
            model = inst->modelSlot.model;
            firstVertexIndex = model->skinObjects[skinObjectIndex].firstVertexIndex;
            if (!(inst->renderFlags & RENDER_FLAG_EXTRA_BONE_HIDDEN) || model->skinObjects[skinObjectIndex].boneId != inst->extraBone + 1) {
                func_800AD048(&model->vertices[firstVertexIndex], &D_800D96B0[groupVertexIndex], &D_800D6720[groupVertexIndex], model->skinObjects[skinObjectIndex].vertexCount);
            } else {
                for (j = 0; j < (*arg2)->modelSlot.model->skinObjects[skinObjectIndex].vertexCount; j++) {
                    D_800D96B0[groupVertexIndex + j] = 0;
                    D_800D6720[groupVertexIndex + j] = 0;
                }
            }
            groupVertexIndex += (*arg2)->modelSlot.model->skinObjects[skinObjectIndex].vertexCount;
        }
        func_800AD0E8((POLY_FT3 *)((POLY_G4 *)((*arg2)->prims + (*arg2)->modelSlot.model->unk30) + (*arg2)->modelSlot.model->unk34) + firstTriangleIndex, &(*arg2)->modelSlot.model->triangles[firstTriangleIndex], &(*arg2)->primOrderIndices[primIndex], (*arg2)->modelSlot.model->drawGroups[i].triangleCount);
        firstTriangleIndex += (*arg2)->modelSlot.model->drawGroups[i].triangleCount;
        primIndex += (*arg2)->modelSlot.model->drawGroups[i].triangleCount;
        func_800AD300((POLY_FT4 *)((POLY_FT3 *)((POLY_G4 *)((*arg2)->prims + (*arg2)->modelSlot.model->unk30) + (*arg2)->modelSlot.model->unk34) + (*arg2)->modelSlot.model->triangleCount) + firstQuadIndex, &(*arg2)->modelSlot.model->quads[firstQuadIndex], &(*arg2)->primOrderIndices[primIndex], (*arg2)->modelSlot.model->drawGroups[i].quadCount);
        firstQuadIndex += (*arg2)->modelSlot.model->drawGroups[i].quadCount;
        primIndex += (*arg2)->modelSlot.model->drawGroups[i].quadCount;
    }
}

/**
 * @brief Reset three field-engine tables.
 *
 * 1. Calls @c memset on each of 64 @c D_800DD6D0
 *    entries (stride 0x30 = 48 bytes), zeroing each in turn.
 * 2. Clears 64 @c s32 entries at @c D_800D6620.
 * 3. Clears 32 @c ModelInstance* pointer entries at @c D_800D9630.
 *
 * The forward for-loops in the scratch source compile to backward
 * @c bgez loops — gcc strength-reduces the index since each iteration
 * is independent.
 */
void func_800A7194(void) {
    s32 i = 0;
    func_800AA8A0_node *p = D_800DD6D0;

    do {
        memset(p, 0, sizeof(func_800AA8A0_node));
        i++;
        p++;
    } while (i < 64);
    for (i = 0; i < 64; i++) {
        D_800D6620[i] = 0;
    }
    for (i = 0; i < 32; i++) {
        D_800D9630[i] = NULL;
    }
}

/** @brief 8-byte (2-aligned) block copied into a @ref ModelInstance's
 *         @c unk10 region by @c func_800A7224 mode 0. The 2-byte alignment is
 *         what makes the copy emit as an unaligned @c lwl/lwr/swl/swr pair. */
typedef struct {
    u16 a, b, c, d;
} RenderSlotVec4;

/**
 * @brief Per-mode update of a @ref ModelInstance's @c unk10 vector, then a
 *        change-detect that re-snapshots and clears flags when it moved.
 *
 * Dispatches on @p mode over @c D_800D9630[idx]:
 *  - @c mode @c 0: overwrite the 8-byte @c unk10 block from @p vals.
 *  - @c mode @c 1: add @p vals[0..2] into @c unk10 / @c unk12 / @c unk14.
 *  - any other @p mode: leave the vector unchanged.
 *
 * Then (all modes) compares the @c unk10 vector against its snapshot
 * (@c field84 / @c field86 / @c field88); if it changed, clears bits @c 0x18
 * of @c renderFlags and refreshes the snapshot.
 *
 * @param idx  Render-slot index into @ref D_800D9630.
 * @param vals Source vector (mode 0 uses 8 bytes; mode 1 uses the first three).
 * @param mode Selects overwrite (0), accumulate (1), or compare-only.
 */
void func_800A7224(s32 idx, u16 *vals, s32 mode) {
    ModelInstance *slot;

    switch (mode) {
    case 0:
        *(RenderSlotVec4 *)&D_800D9630[idx]->unk10 = *(RenderSlotVec4 *)vals;
        break;
    case 1:
        D_800D9630[idx]->unk10 += vals[0];
        D_800D9630[idx]->unk12 += vals[1];
        D_800D9630[idx]->unk14 += vals[2];
        break;
    }

    slot = D_800D9630[idx];
    if ((*(u32 *)&slot->field84 != *(u32 *)&slot->unk10) |
        (slot->field88 != (s16)slot->unk14)) {
        slot->renderFlags &= 0xE7;
        D_800D9630[idx]->field84 = D_800D9630[idx]->unk10;
        D_800D9630[idx]->field86 = D_800D9630[idx]->unk12;
        D_800D9630[idx]->field88 = D_800D9630[idx]->unk14;
    }
}

/**
 * @brief Twin of @c func_800A7224 for the @c unk18 vector of an
 *        @ref ModelInstance (same switch + change-detect, offset +8).
 *
 * Dispatches on @p mode over @c D_800D9630[idx]:
 *  - @c mode @c 0: overwrite the 8-byte @c unk18 block from @p vals.
 *  - @c mode @c 1: add @p vals[0..2] into @c unk18 / @c unk1A / @c unk1C.
 *  - any other @p mode: leave the vector unchanged.
 *
 * Then (all modes) compares the @c unk18 vector against its snapshot
 * (@c field8C / @c field8E / @c field90); if it changed, clears bits @c 0x18
 * of @c renderFlags and refreshes the snapshot.
 *
 * @param idx  Render-slot index into @ref D_800D9630.
 * @param vals Source vector (mode 0 uses 8 bytes; mode 1 uses the first three).
 * @param mode Selects overwrite (0), accumulate (1), or compare-only.
 */
void func_800A736C(s32 idx, u16 *vals, s32 mode) {
    ModelInstance *slot;

    switch (mode) {
    case 0:
        *(RenderSlotVec4 *)&D_800D9630[idx]->unk18 = *(RenderSlotVec4 *)vals;
        break;
    case 1:
        D_800D9630[idx]->unk18 += vals[0];
        D_800D9630[idx]->unk1A += vals[1];
        D_800D9630[idx]->unk1C += vals[2];
        break;
    }

    slot = D_800D9630[idx];
    if ((*(u32 *)&slot->field8C != *(u32 *)&slot->unk18) |
        (slot->field90 != (s16)slot->unk1C)) {
        slot->renderFlags &= 0xE7;
        D_800D9630[idx]->field8C = D_800D9630[idx]->unk18;
        D_800D9630[idx]->field8E = D_800D9630[idx]->unk1A;
        D_800D9630[idx]->field90 = D_800D9630[idx]->unk1C;
    }
}

/**
 * @brief Per-mode write/accumulate of three or four @c s32 values into
 *        a @ref ModelInstance.
 *
 * Dispatches on @p mode over the render slot @c D_800D9630[idx]:
 *  - @c mode @c 0: overwrite the whole @c xform block — a single
 *    @ref EntityRenderXform aggregate copy from @p vals (all four words).
 *  - @c mode @c 1: add @p vals into @c xform.field20 / @c field24 / @c field28
 *    (each read-modify-write reloads the slot pointer; @c field2C untouched).
 *  - any other @p mode: no-op.
 *
 * @param idx   Render-slot index into @ref D_800D9630.
 * @param vals  Transform values to store (mode 0) or accumulate (mode 1).
 * @param mode  Selects overwrite (0), accumulate (1), or no-op.
 */
void func_800A74B4(s32 idx, EntityRenderXform *vals, s32 mode) {
    ModelInstance *slot;

    switch (mode) {
    case 0:
        slot = D_800D9630[idx];
        slot->xform = *vals;
        break;
    case 1:
        D_800D9630[idx]->xform.field20 += vals->field20;
        D_800D9630[idx]->xform.field24 += vals->field24;
        D_800D9630[idx]->xform.field28 += vals->field28;
        break;
    }
}

void *func_800A7564(MchModelHeader *arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_800A7564_entry faceRecord;
    void *animationDataEnd;
    s32 faceCount;
    s32 textureAnimationFileOffset;
    s32 *skinCountPtr;
    func_800A7564_entry *nextFaceRecord;
    s32 i;
    s32 j;
    s32 l;
    s32 animationTableIndex;
    s32 poseIndex;
    s32 modelIndex;
    u32 specialBones;
    u16 *animationStream;
    u16 *animationSection;
    u16 frameCount;
    u16 boneCount;
    u16 triangleIndex;
    u16 quadIndex;

    if (arg1 == 0x12) {
        modelIndex = arg3;
        animationStream = (u16 *)arg0;
        animationTableIndex = 0;
        poseIndex = 0;
        for (i = 0; i < D_800D6620[modelIndex]->animationCount; i++) {
            frameCount = *animationStream++;
            boneCount = *animationStream++;
            D_800D6620[modelIndex]->animationTable[animationTableIndex++] = frameCount;
            D_800D6620[modelIndex]->animationTable[animationTableIndex++] = boneCount + 1;
            for (j = 0; j < frameCount; j++) {
                ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex].half.unk0 = *animationStream++;
                ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex++].half.unk2 = *animationStream++;
                ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex++].half.unk0 = *animationStream++;
                for (l = 0; l < boneCount; l++) {
                    ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex].byte[0] = ((u8 *)animationStream)[0];
                    ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex].byte[1] = ((u8 *)animationStream)[1];
                    ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex].byte[2] = ((u8 *)animationStream)[2];
                    ((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex].byte[3] = ((u8 *)animationStream)[3];
                    animationStream += 2;
                    poseIndex++;
                }
            }
        }
        return &((func_800A7564_unk58 *)D_800D6620[modelIndex]->animations)[poseIndex];
    }
    if (arg1 == 0x11) {
        D_800D6620[arg3]->drawGroupCount = arg0->drawGroupCount;
        D_800D60A0 = arg0;
        // matching hack
        skinCountPtr = &D_800D6620[arg3]->skinObjectCount;
        *skinCountPtr = D_800D60A0->skinObjectCount;
        D_800D6620[arg3]->boneCount = D_800D60A0->boneCount;
        D_800D6620[arg3]->unk30 = D_800D60A0->untexturedTriangleCount;
        D_800D6620[arg3]->unk34 = D_800D60A0->untexturedQuadCount;
        D_800D6620[arg3]->triangleCount = D_800D60A0->texturedTriangleCount;
        D_800D6620[arg3]->quadCount = D_800D60A0->texturedQuadCount;
        faceCount = D_800D60A0->faceCount;
        D_800C3768 = D_800D60A0->vertexCount;
        D_800C376C = D_800D60A0->textureAnimationSectionSize;
        modelIndex = arg3;
        if (arg0->textureAnimationSectionSize != 0) {
            D_800D6620[modelIndex]->flags |= MODEL_FLAG_TEXTURE_ANIMATION;
        } else {
            D_800D6620[modelIndex]->flags &= ~MODEL_FLAG_TEXTURE_ANIMATION;
        }
        D_800D6620[modelIndex]->boneTemplate = (MchBone *)(D_800D6620[modelIndex] + 1);
        D_800D6620[modelIndex]->unk04 = (ModelUnk04 *)(D_800D6620[modelIndex]->boneTemplate + D_800D60A0->boneCount + 1);
        D_800D6620[modelIndex]->unk08 = (ModelUnk08 *)(D_800D6620[modelIndex]->unk04 + D_800D6620[modelIndex]->unk30);
        D_800D6620[modelIndex]->triangles = (ModelTriangle *)(D_800D6620[modelIndex]->unk08 + D_800D6620[modelIndex]->unk34);
        D_800D6620[modelIndex]->quads = (ModelQuad *)(D_800D6620[modelIndex]->triangles + D_800D6620[modelIndex]->triangleCount);
        D_800D6620[modelIndex]->textureAnimation = (MchTextureAnimation *)(D_800D6620[modelIndex]->quads + D_800D6620[modelIndex]->quadCount);
        D_800D6620[modelIndex]->vertices = (SVECTOR *)((u8 *)D_800D6620[modelIndex]->textureAnimation + (D_800D60A0->textureAnimationSectionSize << 3));
        D_800D6620[modelIndex]->drawGroups = (MchDrawGroup *)(D_800D6620[modelIndex]->vertices + D_800D60A0->vertexCount);
        D_800D6620[modelIndex]->skinObjects = (MchSkinObject *)(D_800D6620[modelIndex]->drawGroups + D_800D60A0->drawGroupCount);
        D_800D60A4 = (MchBone *)((u8 *)arg0 + D_800D60A0->boneSectionOffset);
        FastMemCpy((u8 *)(D_800D6620[modelIndex]->boneTemplate + 1), (u8 *)D_800D60A4, D_800D6620[modelIndex]->boneCount << 6);
        for (j = 0; j < D_800D6620[modelIndex]->boneCount; j++) {
            D_800D6620[modelIndex]->boneTemplate[j + 1].boneVector.vz = (arg2 * D_800D60A4[j].boneVector.vz) >> 4;
            D_800D6620[modelIndex]->boneTemplate[j + 1].lockRotation = 0;
        }
        D_800DD6C0 = (SVECTOR *)((u8 *)arg0 + D_800D60A0->vertexSectionOffset);
        for (j = 0; j < D_800C3768; j++) {
            D_800D6620[modelIndex]->vertices[j].vx = (arg2 * D_800DD6C0[j].vx) >> 4;
            D_800D6620[modelIndex]->vertices[j].vy = (arg2 * D_800DD6C0[j].vy) >> 4;
            D_800D6620[modelIndex]->vertices[j].vz = (arg2 * D_800DD6C0[j].vz) >> 4;
        }
        textureAnimationFileOffset = D_800D60A0->textureAnimationSectionOffset;
        FastMemCpy((u8 *)D_800D6620[modelIndex]->textureAnimation, (u8 *)arg0 + textureAnimationFileOffset, D_800D60A0->textureAnimationSectionSize);
        FastMemCpy((u8 *)D_800D6620[modelIndex]->drawGroups, (u8 *)arg0 + D_800D60A0->drawGroupSectionOffset, D_800D6620[modelIndex]->drawGroupCount << 5);
        FastMemCpy((u8 *)D_800D6620[modelIndex]->skinObjects, (u8 *)arg0 + D_800D60A0->skinObjectSectionOffset, D_800D6620[modelIndex]->skinObjectCount << 3);
        animationSection = D_800D60A0->animationSection;
        D_800D6620[modelIndex]->animationCount = *animationSection++;
        D_800D6620[modelIndex]->animationTable = (u16 *)(D_800D6620[modelIndex]->skinObjects + D_800D60A0->skinObjectCount);
        D_800D6620[modelIndex]->animations = (SVECTOR *)(D_800D6620[modelIndex]->animationTable + D_800D6620[modelIndex]->animationCount * 2);
        D_800D6620[modelIndex]->ownAnimations = (SVECTOR *)(D_800D6620[modelIndex]->animationTable + D_800D6620[modelIndex]->animationCount * 2);
        animationDataEnd = func_800A7564((MchModelHeader *)animationSection, 0x12, 0, modelIndex);
        specialBones = D_800D60A0->specialBones;
        D_800D6620[modelIndex]->headBone = specialBones;
        D_800D6620[modelIndex]->bodyBone = specialBones >> 8;
        D_800D6620[modelIndex]->extraBone = specialBones >> 16;
        nextFaceRecord = (func_800A7564_entry *)((u8 *)arg0 + D_800D60A0->faceSectionOffset);
        triangleIndex = 0;
        quadIndex = 0;
        if ((u32)nextFaceRecord < (u32)animationDataEnd) {
            SystemError(0x53, 0x14);
        }
        for (j = 0; j < faceCount; j++) {
            faceRecord = *nextFaceRecord++;
            switch (faceRecord.unk0) {
            case 0x25010607:
                D_800D6620[modelIndex]->triangles[triangleIndex].unk0 = faceRecord.unkC;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk2 = faceRecord.unkE;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk4 = faceRecord.unk10;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk6 = faceRecord.unk2C + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->triangles[triangleIndex].unk8 = faceRecord.unk2E + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->triangles[triangleIndex].unkA = faceRecord.unk30 + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->triangles[triangleIndex].unkC = D_800D9328[arg3][faceRecord.unk36];
                D_800D6620[modelIndex]->triangles[triangleIndex].unkE = D_800D6300[arg3][faceRecord.unk36];
                D_800D6620[modelIndex]->triangles[triangleIndex].unk10.r = 0x80;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk10.g = 0x80;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk10.b = 0x80;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk10.cd = faceRecord.unk1D;
                D_800D6620[modelIndex]->triangles[triangleIndex].unk14 = faceRecord.unk8;
                triangleIndex++;
                break;
            case 0x2D010709:
                D_800D6620[modelIndex]->quads[quadIndex].unk0 = faceRecord.unkC;
                D_800D6620[modelIndex]->quads[quadIndex].unk2 = faceRecord.unkE;
                D_800D6620[modelIndex]->quads[quadIndex].unk4 = faceRecord.unk10;
                D_800D6620[modelIndex]->quads[quadIndex].unk6 = faceRecord.unk12;
                D_800D6620[modelIndex]->quads[quadIndex].unk8 = faceRecord.unk2C + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->quads[quadIndex].unkA = faceRecord.unk2E + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->quads[quadIndex].unkC = faceRecord.unk30 + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->quads[quadIndex].unkE = faceRecord.unk32 + D_800D60E8[faceRecord.unk36];
                D_800D6620[modelIndex]->quads[quadIndex].unk10 = D_800D9328[arg3][faceRecord.unk36];
                D_800D6620[modelIndex]->quads[quadIndex].unk12 = D_800D6300[arg3][faceRecord.unk36];
                D_800D6620[modelIndex]->quads[quadIndex].unk14.r = 0x80;
                D_800D6620[modelIndex]->quads[quadIndex].unk14.g = 0x80;
                D_800D6620[modelIndex]->quads[quadIndex].unk14.b = 0x80;
                D_800D6620[modelIndex]->quads[quadIndex].unk14.cd = faceRecord.unk1D;
                D_800D6620[modelIndex]->quads[quadIndex].unk18 = faceRecord.unk8;
                quadIndex++;
                break;
            }
        }
    }
    return animationDataEnd;
}

/**
 * @brief Allocate and initialize the field-object slot @p idx from @p newObj.
 *
 * If the slot is already occupied, returns NULL without touching it. Otherwise it
 * installs @p newObj into @c D_800D6620[idx], stamps the @c 0x12345678 signature
 * and the @c D_800D60E8 seed, invokes @c func_800A7564 to build the object, and
 * clears the per-part @c unk8.pad and per-skin-object @c unk6 accumulators across
 * every part's sub-range.
 *
 * @param idx    Object slot index into @ref D_800D6620 (0..63).
 * @param arg1   Opaque handle forwarded to @c func_800A7564 as its first arg.
 * @param newObj Object instance to install into the slot.
 * @param count  Build count for @c func_800A7564; defaults to 0x10 when zero.
 * @return @c func_800A7564's result, or NULL if the slot was already occupied.
 */
void *func_800A8058(s32 idx, MchModelHeader *arg1, Model *newObj, u8 count) {
    void *ret;
    s32 i, j;

    if (D_800D6620[idx] != NULL) {
        return NULL;
    }
    if (count == 0) {
        count = 0x10;
    }
    D_800D6620[idx] = newObj;
    newObj->flags = 0;
    D_800D6620[idx]->signature = 0x12345678;
    D_800D6620[idx]->field50 = D_800D60E8[0];
    ret = func_800A7564(arg1, 0x11, count, idx);
    for (i = 0; i < D_800D6620[idx]->drawGroupCount; i++) {
        D_800D6620[idx]->drawGroups[i].unk8.pad = 0;
        for (j = D_800D6620[idx]->drawGroups[i].firstSkinObjectIndex;
             j < D_800D6620[idx]->drawGroups[i].firstSkinObjectIndex + D_800D6620[idx]->drawGroups[i].skinObjectCount;
             j++) {
            D_800D6620[idx]->skinObjects[j].unk6 = 0;
        }
    }
    return ret;
}

POLY_FT4 *func_800A81AC(s32 arg0, s32 arg1, ModelInstance *arg2) {
    s32 i;
    s32 j;
    u16 *primCursor;
    u32 misalign;
    POLY_G3 *g3;
    POLY_G4 *g4;
    POLY_FT4 *ft4;

    if (D_800D9630[arg0] != NULL) {
        return NULL;
    }
    D_800D9630[arg0] = arg2;
    arg2->modelSlot.model = D_800D6620[arg1];
    D_800D9630[arg0]->renderFlags = 0;
    D_800D9630[arg0]->animFlags = 0;
    D_800D9630[arg0]->unk61 = 0x11;
    D_800D9630[arg0]->heightOffset = 0;
    D_800D9630[arg0]->headBone = D_800D6620[arg1]->headBone;
    D_800D9630[arg0]->bodyBone = D_800D6620[arg1]->bodyBone;
    D_800D9630[arg0]->extraBone = D_800D6620[arg1]->extraBone;
    D_800D9630[arg0]->cachedAnimationStart = (SVECTOR *)-1;
    D_800D9630[arg0]->cachedPhase = 0xFFFF;
    D_800D9630[arg0]->animationIndex = 0;
    D_800D9630[arg0]->bones = (MchBone *)&D_800D9630[arg0]->subBuffer;
    FastMemCpy((u8 *)D_800D9630[arg0]->bones, (u8 *)D_800D9630[arg0]->modelSlot.model->boneTemplate, D_800D9630[arg0]->modelSlot.model->boneCount * sizeof(MchBone) + sizeof(MchBone));
    D_800D9630[arg0]->animationCount = D_800D6620[arg1]->animationCount;
    D_800D9630[arg0]->animationTable = D_800D6620[arg1]->animationTable;
    D_800D9630[arg0]->animationStart = D_800D6620[arg1]->animations;
    D_800D9630[arg0]->bones->rotation.vx = 0;
    D_800D9630[arg0]->bones->rotation.vy = 0x400;
    D_800D9630[arg0]->bones->rotation.vz = 0;
    D_800D9630[arg0]->bones->boneVector.vx = 0;
    D_800D9630[arg0]->bones->boneVector.vy = 0;
    D_800D9630[arg0]->bones->boneVector.vz = 0;
    primCursor = (u16 *)&D_800D9630[arg0]->bones[D_800D9630[arg0]->modelSlot.model->boneCount + 2];
    D_800D9630[arg0]->primOrderIndices = primCursor;
    for (j = 0; j < D_800D6620[arg1]->unk30 + D_800D6620[arg1]->unk34 + D_800D6620[arg1]->triangleCount + D_800D6620[arg1]->quadCount; j++) {
        D_800D9630[arg0]->primOrderIndices[0] = 0;
        D_800D9630[arg0]->primOrderIndices[j] = 0;
    }
    primCursor += D_800D6620[arg1]->unk30 + D_800D6620[arg1]->unk34 + D_800D6620[arg1]->triangleCount + D_800D6620[arg1]->quadCount;
    misalign = (u32)primCursor & 3;
    if (misalign != 0) {
        primCursor = (u16 *)((u8 *)primCursor + (4 - misalign));
    }
    D_800D9630[arg0]->animationLength = D_800D9630[arg0]->animationTable[0] << 4;
    D_800D9630[arg0]->posesPerFrame = D_800D9630[arg0]->animationTable[1];
    D_800D9630[arg0]->animations = (CVECTOR *)D_800D9630[arg0]->animationStart;
    D_800D9630[arg0]->unk94 = D_800D9630[arg0]->animationStart;
    ft4 = (POLY_FT4 *)primCursor;
    D_800D9630[arg0]->prims = (POLY_G3 *)ft4;
    for (i = 0; i < 2; i++) {
        D_800D9630[arg0]->primBuffers[i] = (POLY_G3 *)ft4;
        D_800D5EE0[i] = (POLY_G3 *)ft4;
        g3 = (POLY_G3 *)ft4;
        for (j = 0; j < D_800D6620[arg1]->unk30; j++) {
            func_8004D684(g3);
            g3->r0 = D_800D6620[arg1]->unk04[j].unkC.r;
            g3->g0 = D_800D6620[arg1]->unk04[j].unkC.g;
            g3->b0 = D_800D6620[arg1]->unk04[j].unkC.b;
            g3->r1 = D_800D6620[arg1]->unk04[j].unk10.r;
            g3->g1 = D_800D6620[arg1]->unk04[j].unk10.g;
            g3->b1 = D_800D6620[arg1]->unk04[j].unk10.b;
            g3->r2 = D_800D6620[arg1]->unk04[j].unk14.r;
            g3->g2 = D_800D6620[arg1]->unk04[j].unk14.g;
            g3->b2 = D_800D6620[arg1]->unk04[j].unk14.b;
            g3++;
        }
        g4 = (POLY_G4 *)g3;
        D_800D5EE8[i] = g4;
        for (j = 0; j < D_800D6620[arg1]->unk34; j++) {
            func_8004D6C4(g4);
            g4->r0 = D_800D6620[arg1]->unk08[j].unk10.r;
            g4->g0 = D_800D6620[arg1]->unk08[j].unk10.g;
            g4->b0 = D_800D6620[arg1]->unk08[j].unk10.b;
            g4->r1 = D_800D6620[arg1]->unk08[j].unk14.r;
            g4->g1 = D_800D6620[arg1]->unk08[j].unk14.g;
            g4->b1 = D_800D6620[arg1]->unk08[j].unk14.b;
            g4->r2 = D_800D6620[arg1]->unk08[j].unk18.r;
            g4->g2 = D_800D6620[arg1]->unk08[j].unk18.g;
            g4->b2 = D_800D6620[arg1]->unk08[j].unk18.b;
            g4->r3 = D_800D6620[arg1]->unk08[j].unk1C.r;
            g4->g3 = D_800D6620[arg1]->unk08[j].unk1C.g;
            g4->b3 = D_800D6620[arg1]->unk08[j].unk1C.b;
            g4++;
        }
        ft4 = (POLY_FT4 *)g4;
        D_800D6600[i] = (POLY_FT3 *)ft4;
        for (j = 0; j < D_800D6620[arg1]->triangleCount; j++) {
            func_8004D664(ft4);
            ft4->r0 = D_800D6620[arg1]->triangles[j].unk10.r;
            ft4->g0 = D_800D6620[arg1]->triangles[j].unk10.g;
            ft4->b0 = D_800D6620[arg1]->triangles[j].unk10.b;
            ft4->pad1 = D_800D6620[arg1]->triangles[j].unk10.cd;
            ft4->tpage = D_800D6620[arg1]->triangles[j].unkE;
            ft4->clut = D_800D6620[arg1]->triangles[j].unkC;
            *(u16 *)&ft4->u0 = D_800D6620[arg1]->triangles[j].unk6;
            *(u16 *)&ft4->u1 = D_800D6620[arg1]->triangles[j].unk8;
            *(u16 *)&ft4->u2 = D_800D6620[arg1]->triangles[j].unkA;
            if (D_800D6620[arg1]->triangles[j].unk14 & MODEL_TRIANGLE_FLAG_4) {
                SetSemiTrans(ft4, 1);
            }
            ft4 = (POLY_FT4 *)((POLY_FT3 *)ft4 + 1); // matching hack
        }
        D_800D6610[i] = ft4;
        for (j = 0; j < D_800D6620[arg1]->quadCount; j++) {
            func_8004D6A4(ft4);
            ft4->r0 = D_800D6620[arg1]->quads[j].unk14.r;
            ft4->g0 = D_800D6620[arg1]->quads[j].unk14.g;
            ft4->b0 = D_800D6620[arg1]->quads[j].unk14.b;
            ft4->pad1 = D_800D6620[arg1]->quads[j].unk14.cd;
            ft4->tpage = D_800D6620[arg1]->quads[j].unk12;
            ft4->clut = D_800D6620[arg1]->quads[j].unk10;
            *(u16 *)&ft4->u0 = D_800D6620[arg1]->quads[j].unk8;
            *(u16 *)&ft4->u1 = D_800D6620[arg1]->quads[j].unkA;
            *(u16 *)&ft4->u2 = D_800D6620[arg1]->quads[j].unkC;
            *(u16 *)&ft4->u3 = D_800D6620[arg1]->quads[j].unkE;
            if (D_800D6620[arg1]->quads[j].unk18 & MODEL_QUAD_FLAG_4) {
                SetSemiTrans(ft4, 1);
            }
            ft4++;
        }
    }
    D_800D9630[arg0]->unk10 = 0;
    D_800D9630[arg0]->unk12 = 0x190;
    D_800D9630[arg0]->unk14 = 0;
    D_800D9630[arg0]->unk18 = 0;
    D_800D9630[arg0]->unk1A = 0;
    D_800D9630[arg0]->unk1C = 0;
    D_800D9630[arg0]->xform.field20 = 0x1000;
    D_800D9630[arg0]->xform.field24 = 0x1000;
    D_800D9630[arg0]->xform.field28 = 0x1000;
    D_800D9630[arg0]->unk50 = 0;
    D_800D9630[arg0]->animationPhase = 0;
    return ft4;
}

/**
 * @brief Claim a render-slot pool entry and initialize its state.
 *
 * Acquires @c D_800D9630[idx] if currently @c NULL by installing @p slot
 * and writing @p firstWord to the slot's leading @c s32. The slot's state
 * fields (@c unk10/12/14/18/1A/1C/50/52) are zeroed except @c unk12 which
 * gets the default @c 0x190; the three @c field20/24/28 scale factors are
 * set to @c 0x1000 (unit scale); @c renderFlags (active flag) is cleared.
 *
 * @return @p slot @c + @c 0x98 on successful claim, or @c NULL if the
 *         render-slot was already occupied.
 */
s32 *func_800A8CDC(s32 idx, s32 firstWord, ModelInstance *slot) {
    if (D_800D9630[idx] != NULL) {
        return NULL;
    }
    D_800D9630[idx] = slot;
    slot->modelSlot.modelId = firstWord;
    D_800D9630[idx]->renderFlags = 0;
    D_800D9630[idx]->unk10 = 0;
    D_800D9630[idx]->unk12 = 0x190;
    D_800D9630[idx]->unk14 = 0;
    D_800D9630[idx]->unk18 = 0;
    D_800D9630[idx]->unk1A = 0;
    D_800D9630[idx]->unk1C = 0;
    D_800D9630[idx]->xform.field20 = 0x1000;
    D_800D9630[idx]->xform.field24 = 0x1000;
    D_800D9630[idx]->xform.field28 = 0x1000;
    D_800D9630[idx]->unk50 = 0;
    D_800D9630[idx]->animationPhase = 0;
    D_800D9630[idx]->renderFlags = 0;
    return &slot->subBuffer;
}

void *func_800A8DAC(s32 spatialIdx, s32 cmd, void *arg, void *out) {
    SVECTOR vec;
    SVECTOR offset;
    SVECTOR unused;
    VECTOR rotated;
    VECTOR origin;
    MATRIX transposed;
    MATRIX view;
    CVECTOR col;
    void *ret;
    s32 j;
    s32 k;
    s32 sum;
    s32 productSum;
    u32 first;
    u32 second;

    switch (cmd) {
    case 0x1C:
        ret = &D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation;
        break;
    case 0x1D:
        ret = &D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation;
        break;
    case 0x1E:
        view = *(MATRIX *)arg;
        transposed.m[0][0] = view.m[0][0];
        transposed.m[0][1] = view.m[1][0];
        transposed.m[0][2] = view.m[2][0];
        transposed.m[1][0] = view.m[0][1];
        transposed.m[1][1] = view.m[1][1];
        transposed.m[1][2] = view.m[2][1];
        transposed.m[2][0] = view.m[0][2];
        transposed.m[2][1] = view.m[1][2];
        transposed.m[2][2] = view.m[2][2];
        SetRotMatrix(&transposed);
        SetTransMatrix(&transposed);
        vec.vx = D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].transform.t[0];
        vec.vy = D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].transform.t[1];
        vec.vz = D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].transform.t[2];
        RotTrans(&vec, &rotated, &D_800DD6C8);
        vec.vx = D_800D9630[spatialIdx]->bones[0].transform.t[0];
        vec.vy = D_800D9630[spatialIdx]->bones[0].transform.t[1];
        vec.vz = D_800D9630[spatialIdx]->bones[0].transform.t[2];
        RotTrans(&vec, &origin, &D_800DD6C8);
        vec.vx = rotated.vx - origin.vx;
        vec.vy = rotated.vy - origin.vy;
        vec.vz = rotated.vz - origin.vz;
        offset = *D_800D9630[spatialIdx]->animationStart;
        vec.vx += offset.vx;
        vec.vy += offset.vy;
        vec.vz += offset.vz;
        *(SVECTOR *)out = vec;
        break;
    case 0x1F:
        ret = &D_800D9630[spatialIdx]->unk30;
        break;
    case 0x20:
        k = 2;
        productSum = 0;
        sum = 0;
        for (j = 0; j < D_800D9630[spatialIdx]->animationIndex; j++) {
            first = D_800D9630[spatialIdx]->animationTable[k - 2];
            second = D_800D9630[spatialIdx]->animationTable[k - 1];
            sum += first;
            productSum += first * second;
            k += 2;
        }
        col = D_800D9630[spatialIdx]->animations[productSum + D_800D9630[spatialIdx]->headBone + sum + 2];
        ((s16 *)arg)[0] = ((col.cd << 10) & 0xC00) | (col.r << 2);
        ((s16 *)arg)[1] = ((col.cd << 8) & 0xC00) | (col.g << 2);
        ((s16 *)arg)[2] = ((col.cd << 6) & 0xC00) | (col.b << 2);
        break;
    }
    return ret;
}

void func_800A91C8(s32 arg0, s32 arg1, s32 arg2, SVECTOR *arg3) {
    SVECTOR delta;
    s32 i;

    i = arg2 + 2;
    switch (arg1) {
    case 0x2A:
        D_800D9630[arg0]->bones[i].parent = (s32)arg3;
        break;
    case 0x28:
        D_800D9630[arg0]->bones[i].boneVector = *arg3;
        break;
    case 0x29:
        delta = *arg3;
        D_800D9630[arg0]->bones[i].boneVector.vx += delta.vx;
        D_800D9630[arg0]->bones[i].boneVector.vy += delta.vy;
        D_800D9630[arg0]->bones[i].boneVector.vz += delta.vz;
        break;
    case 0x2B:
        D_800D9630[arg0]->bones[i].lockRotation = 1;
        D_800D9630[arg0]->bones[i].rotation = *arg3;
        break;
    case 0x2C:
        D_800D9630[arg0]->bones[i].lockRotation = 1;
        delta = *arg3;
        D_800D9630[arg0]->bones[i].rotation.vx += delta.vx;
        D_800D9630[arg0]->bones[i].rotation.vy += delta.vy;
        D_800D9630[arg0]->bones[i].rotation.vz += delta.vz;
        break;
    }
}

s32 func_800A9434(s32 arg0, s32 arg1, u32 arg2, s16 *arg3, s32 arg4) {
    s32 sumA;
    s32 sumB;
    s32 k;
    s32 j;
    POLY_FT3 *ft3;
    POLY_FT4 *ft4;
    s32 unused[2];

    if (D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk8.pad & (MCH_DRAW_GROUP_FLAG_1 | MCH_DRAW_GROUP_FLAG_2)) {
        return -1;
    }
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk8.pad = 0;
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.pad = arg4;
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk4 = 1;
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk8.vx = D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.vx;
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk8.vy = D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.vy;
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk8.vz = D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.vz;
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.vx += arg3[0];
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.vy += arg3[1];
    D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk18.vz += arg3[2];
    switch (arg1) {
    case 0x30:
        D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].unk8.pad |= MCH_DRAW_GROUP_FLAG_1;
        func_800A97E4(arg0, 0x25, 0, 0);
        break;
    case 0x32:
        sumA = 0;
        sumB = 0;
        for (j = 0; j < arg2; j++) {
            sumA += D_800D9630[arg0]->modelSlot.model->drawGroups[j].triangleCount;
            sumB += D_800D9630[arg0]->modelSlot.model->drawGroups[j].quadCount;
        }
        for (k = 0; k < 2; k++) {
            ft3 = (POLY_FT3 *)((POLY_G4 *)(D_800D9630[arg0]->primBuffers[k] + D_800D9630[arg0]->modelSlot.model->unk30) + D_800D9630[arg0]->modelSlot.model->unk34);
            for (j = 0; j < D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].triangleCount; j++) {
                ft3[sumA + j].pad1 = arg4;
            }
            ft4 = (POLY_FT4 *)((POLY_FT3 *)((POLY_G4 *)(D_800D9630[arg0]->primBuffers[k] + D_800D9630[arg0]->modelSlot.model->unk30) + D_800D9630[arg0]->modelSlot.model->unk34) + D_800D9630[arg0]->modelSlot.model->triangleCount);
            for (j = 0; j < D_800D9630[arg0]->modelSlot.model->drawGroups[arg2].quadCount; j++) {
                ft4[sumB + j].pad1 = arg4;
            }
        }
        break;
    }
}

s32 func_800A97E4(s32 spatialIdx, s32 cmd, s32 arg2, s32 arg3) {
    CVECTOR color;
    RECT rect;
    s32 ret;
    s32 i;
    s32 j;
    POLY_G3 *g3;
    POLY_G4 *g4;
    POLY_FT3 *ft3;
    POLY_FT4 *ft4;

    if (D_800D9630[spatialIdx]->modelSlot.modelId == NO_MODEL) {
        return -1;
    }
    switch (cmd) {
    case 0x27:
        if (!(D_800D9630[spatialIdx]->modelSlot.model->flags & MODEL_FLAG_TEXTURE_ANIMATION)) {
            return 1;
        }
        if ((s16)D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->unkA >= 0) {
            D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->unkA = arg3;
            if (arg2 != 0) {
                D_800D9630[spatialIdx]->frameTimer = arg2;
                D_800D9630[spatialIdx]->timerReload = arg2;
            } else {
                D_800D9630[spatialIdx]->frameTimer = rand() % 8 + 1;
                D_800D9630[spatialIdx]->timerReload = rand() % 8 + 1;
            }
        } else {
            D_800D9630[spatialIdx]->frameTimer = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->unkA;
            D_800D9630[spatialIdx]->timerReload = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->unkA;
        }
        D_800D9630[spatialIdx]->animFlags &= 0xF;
        D_800D9630[spatialIdx]->animFlags |= ENTITY_ANIM_FLAG_10;
        D_800D9630[spatialIdx]->frameIdx = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCount;
        break;
    case 0x2F:
        if (!(D_800D9630[spatialIdx]->modelSlot.model->flags & MODEL_FLAG_TEXTURE_ANIMATION)) {
            return 1;
        }
        D_800D9630[spatialIdx]->animFlags &= 0xF;
        D_800D9630[spatialIdx]->animFlags |= ENTITY_ANIM_FLAG_20;
        rect.w = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->width;
        rect.h = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->height;
        rect.x = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCoords[0];
        rect.y = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCoords[1];
        MoveImage(&rect, D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->destX, D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->unk8);
        break;
    case 0x2E:
        if (!(D_800D9630[spatialIdx]->modelSlot.model->flags & MODEL_FLAG_TEXTURE_ANIMATION)) {
            return 1;
        }
        D_800D9630[spatialIdx]->animFlags &= 0xF;
        D_800D9630[spatialIdx]->animFlags |= ENTITY_ANIM_FLAG_40;
        rect.w = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->width;
        rect.h = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->height;
        rect.x = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCoords[D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCount * 2 - 2];
        rect.y = D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCoords[D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->frameCount * 2 - 1];
        MoveImage(&rect, D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->destX, D_800D9630[spatialIdx]->modelSlot.model->textureAnimation->unk8);
        break;
    case 0x25:
        D_800D9630[spatialIdx]->cachedAnimationStart = (SVECTOR *)-1;
        break;
    case 0x23:
        D_800D9630[spatialIdx]->renderFlags |= ENTITY_RENDER_FLAG_40;
        break;
    case 0x24:
        D_800D9630[spatialIdx]->renderFlags = 0;
        break;
    case 0x21:
        D_800D9630[spatialIdx]->renderFlags |= RENDER_FLAG_EXTRA_BONE_HIDDEN;
        for (j = 0; j < 0x1003; j++) {
            D_800D6720[j] = 0xFFFF;
        }
        break;
    case 0x22:
        D_800D9630[spatialIdx]->renderFlags &= ~RENDER_FLAG_EXTRA_BONE_HIDDEN;
        break;
    case 0x13:
        D_800D9630[spatialIdx]->renderFlags |= ENTITY_RENDER_FLAG_2;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation = *(SVECTOR *)arg2;
        break;
    case 0x1C:
        D_800D9630[spatialIdx]->renderFlags |= ENTITY_RENDER_FLAG_2;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation = *(SVECTOR *)arg2;
        break;
    case 0x14:
    case 0x1D:
        D_800D9630[spatialIdx]->renderFlags |= ENTITY_RENDER_FLAG_4;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation = *(SVECTOR *)arg2;
        break;
    case 0x19:
        D_800D9630[spatialIdx]->renderFlags |= ENTITY_RENDER_FLAG_2;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation.vx += ((SVECTOR *)arg2)->vx;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation.vy += ((SVECTOR *)arg2)->vy;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation.vz += ((SVECTOR *)arg2)->vz;
        break;
    case 0x1A:
        D_800D9630[spatialIdx]->renderFlags |= ENTITY_RENDER_FLAG_4;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation.vx = ((SVECTOR *)arg2)->vx;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation.vy = ((SVECTOR *)arg2)->vy;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation.vz = ((SVECTOR *)arg2)->vz;
        break;
    case 0x15:
        D_800D9630[spatialIdx]->renderFlags &= ~ENTITY_RENDER_FLAG_2;
        break;
    case 0x16:
        D_800D9630[spatialIdx]->renderFlags &= ~ENTITY_RENDER_FLAG_4;
        break;
    case 0xF:
        for (i = 0; i < 2; i++) {
            g3 = D_800D9630[spatialIdx]->primBuffers[i];
            D_800D5EE0[i] = g3;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->unk30; j++) {
                SetSemiTrans(g3, arg2);
                g3++;
            }
            g4 = (POLY_G4 *)g3;
            D_800D5EE8[i] = g4;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->unk34; j++) {
                SetSemiTrans(g4, arg2);
                g4++;
            }
            ft3 = (POLY_FT3 *)g4;
            D_800D6600[i] = ft3;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->triangleCount; j++) {
                SetSemiTrans(ft3, arg2);
                ft3++;
            }
            ft4 = (POLY_FT4 *)ft3;
            D_800D6610[i] = ft4;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->quadCount; j++) {
                SetSemiTrans(ft4, arg2);
                ft4++;
            }
        }
        break;
    case 0x10:
        color.r = ((CVECTOR *)arg2)->r;
        color.g = ((CVECTOR *)arg2)->g;
        color.b = ((CVECTOR *)arg2)->b;
        for (i = 0; i < 2; i++) {
            g3 = D_800D9630[spatialIdx]->primBuffers[i];
            D_800D5EE0[i] = g3;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->unk30; j++) {
                g3->r0 = color.r;
                g3->g0 = color.g;
                g3->b0 = color.b;
                g3->r1 = color.r;
                g3->g1 = color.g;
                g3->b1 = color.b;
                g3->r2 = color.r;
                g3->g2 = color.g;
                g3->b2 = color.b;
                g3++;
            }
            g4 = (POLY_G4 *)g3;
            D_800D5EE8[i] = g4;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->unk34; j++) {
                g4->r0 = color.r;
                g4->g0 = color.g;
                g4->b0 = color.b;
                g4->r1 = color.r;
                g4->g1 = color.g;
                g4->b1 = color.b;
                g4->r2 = color.r;
                g4->g2 = color.g;
                g4->b2 = color.b;
                g4++;
            }
            ft3 = (POLY_FT3 *)g4;
            D_800D6600[i] = ft3;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->triangleCount; j++) {
                ft3->r0 = color.r;
                ft3->g0 = color.g;
                ft3->b0 = color.b;
                ft3++;
            }
            ft4 = (POLY_FT4 *)ft3;
            D_800D6610[i] = ft4;
            for (j = 0; j < D_800D9630[spatialIdx]->modelSlot.model->quadCount; j++) {
                ft4->r0 = color.r;
                ft4->g0 = color.g;
                ft4->b0 = color.b;
                ft4++;
            }
        }
        break;
    }
    return ret;
}

void func_800AA46C(s32 spatialIdx, s32 cmd, u32 arg, s32 arg4) {
    u32 i;
    s32 j;
    s32 sum;
    s32 productSum;
    u32 first;
    u32 second;

    if (D_800D9630[spatialIdx]->modelSlot.modelId != NO_MODEL && cmd == 13) {
        if (arg >= D_800D9630[spatialIdx]->animationCount) {
            func_800ACBCC(spatialIdx, D_800D9630[spatialIdx]->animationCount, arg, 0);
            SystemError(0x53, 0x17);
        }
        j = 2;
        productSum = 0;
        sum = 0;
        D_800D9630[spatialIdx]->animationIndex = arg;
        for (i = 0; i < arg; i++) {
            first = D_800D9630[spatialIdx]->animationTable[j - 2];
            second = D_800D9630[spatialIdx]->animationTable[j - 1];
            sum += first;
            productSum += first * second;
            j += 2;
        }
        D_800D9630[spatialIdx]->animationLength = D_800D9630[spatialIdx]->animationTable[j - 2] << 4;
        D_800D9630[spatialIdx]->posesPerFrame = D_800D9630[spatialIdx]->animationTable[j - 1];
        D_800D9630[spatialIdx]->animationStart = (SVECTOR *)(D_800D9630[spatialIdx]->animations + (productSum + sum));
        D_800D9630[spatialIdx]->unk50 = 0;
        D_800D9630[spatialIdx]->unk50 = 0;
        D_800D9630[spatialIdx]->cachedAnimationStart = (SVECTOR *)-1;
        D_800D9630[spatialIdx]->cachedPhase = 0xFFFF;
    }
}

/**
 * @brief Per-entity animation tick — advances the frame and stages the next
 *        sprite rectangle for upload.
 *
 * For render slot @p idx this:
 *  - returns 1 immediately if the entity's definition is not animated
 *    (@c model->flags bit 0 clear);
 *  - while the retrigger flag (@c animFlags @c 0x80) is set, marks the slot
 *    active (@c 0x10) on the frames where @c rand() is an exact multiple of
 *    the loop period;
 *  - returns 0 while the per-frame countdown @c frameTimer has not expired;
 *  - on expiry reloads @c frameTimer, steps @c frameIdx, and when that wraps
 *    to 0 re-arms the retrigger flag and reloads the frame count;
 *  - builds the destination sprite @ref RECT in @c D_800D5ED8 from the current
 *    frame's coordinates and the animation rect size, then uploads it with
 *    @c MoveImage.
 *
 * Originally emitted as two INCLUDE_ASM stubs (@c func_800AA5F8 plus a
 * spurious @c func_800AA870 tail label) because splat cut the function at the
 * load-delay @c nop preceding the final @c MoveImage call.
 *
 * @param idx Entity render-slot index into @ref D_800D9630.
 * @return 1 if the entity is not animated, otherwise 0.
 */
s32 func_800AA5F8(s32 idx) {
    s16 period;
    u8 flags;
    s16 per;

    period = D_800D9630[idx]->modelSlot.model->textureAnimation->unkA;
    if (!(D_800D9630[idx]->modelSlot.model->flags & MODEL_FLAG_TEXTURE_ANIMATION)) {
        return 1;
    }

    flags = D_800D9630[idx]->animFlags;
    if (flags & 0x80) {
        per = period;
        if (per > 0) {
            if (rand() % per == 0) {
                D_800D9630[idx]->animFlags &= 0x0F;
                D_800D9630[idx]->animFlags |= 0x10;
            }
        } else {
            D_800D9630[idx]->animFlags = flags & 0x0F;
            D_800D9630[idx]->animFlags |= 0x10;
        }
    }

    if ((D_800D9630[idx]->animFlags & 0x10) == 0) {
        return 0;
    }

    D_800D9630[idx]->frameTimer--;
    if (D_800D9630[idx]->frameTimer != 0) {
        return 0;
    }

    if (period < 0) {
        D_800D9630[idx]->frameTimer = period;
    } else {
        D_800D9630[idx]->frameTimer = D_800D9630[idx]->timerReload;
    }

    D_800D9630[idx]->frameIdx--;
    if (D_800D9630[idx]->frameIdx == 0) {
        D_800D9630[idx]->animFlags &= 0x0F;
        D_800D9630[idx]->animFlags |= 0x80;
        D_800D9630[idx]->frameIdx = D_800D9630[idx]->modelSlot.model->textureAnimation->frameCount;
    }

    D_800D5ED8.w = D_800D9630[idx]->modelSlot.model->textureAnimation->width;
    D_800D5ED8.h = D_800D9630[idx]->modelSlot.model->textureAnimation->height;
    D_800D5ED8.x = D_800D9630[idx]->modelSlot.model->textureAnimation->frameCoords[D_800D9630[idx]->frameIdx * 2 - 2];
    D_800D5ED8.y = D_800D9630[idx]->modelSlot.model->textureAnimation->frameCoords[D_800D9630[idx]->frameIdx * 2 - 1];

    MoveImage(&D_800D5ED8,
              D_800D9630[idx]->modelSlot.model->textureAnimation->destX,
              D_800D9630[idx]->modelSlot.model->textureAnimation->unk8);
    return 0;
}

void *func_800AA8A0(void *arg0, u8 *arg1, u32 *arg2, u32 *arg3, CdFileDesc *arg4, s32 arg5, CdFileDesc *arg6, u8 *arg7) {
    func_800AA8A0_tim tim;
    u8 modelTable[0x400];
    u8 modelUnk6F[0x40];
    u16 texturePageX[8];
    u16 texturePageY[8];
    func_800AA8A0_pos colorOffsets[0x40];
    u32 texturePagePositions[16];
    u32 clutPositions[16];
    u16 texturePagePositionIndex;
    u16 clutPositionIndex;
    void *freeSpace;
    Actor *actor;
    func_800AA8A0_node *node;
    s32 i, modelIndex;
    s32 j, k;
    Actor *current;
    MchModelHeader *header;
    s32 cacheIndex;
    u8 *loadCursor;
    u32 word;
    s32 cacheSlotIndex;
    u16 *palette;
    s32 *cursor;

    for (i = 0; i < 0x10; i++) {
        texturePagePositions[i] = arg3[i];
        clutPositions[i] = arg2[i];
    }

    D_800DD6BC = 0;

    for (i = 0; i < 0x4E; i++) {
        D_800D60F8[i] = -1;
    }

    for (j = 0; j < 3; j++) {
        s16 acc;
        if ((u32)D_800D61A8[j].tag < 0x4E) {
            acc = 0;
            for (k = 0; k < D_800D61A8[j].unk6C; k++) {
                acc |= D_800D61A8[j].unkC[k].y;
            }
            if (acc < 0x100) {
                D_800D60F8[D_800D61A8[j].tag] = D_800D61A8[j].unk6E;
            }
        }
        D_800D5F50[j].tag = -1;
    }
    freeSpace = arg0;
    texturePagePositionIndex = 0;
    clutPositionIndex = 0;
    cdReadSync(arg4->sector, 0x400, arg1, 0);
    FastMemCpy(modelTable, arg1, 0x400);
    loadCursor = arg1;

    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    for (i = 0; i < 0x40; i++) {
        D_800D60A8[i] = 0;
    }

    actor = g_fieldActors;
    modelIndex = 0;
    for (j = 0, current = actor; j < g_fieldActorCount; j++, current++) {
        node = &D_800DD6D0[modelIndex];
        if (current->modelId != -1) {
            u8 *modelTableIndex = &D_800D5EF0[current->modelId];
            cursor = (s32 *)modelTable;
            if (*modelTableIndex == 0xFF) {
                *modelTableIndex = modelIndex;
                for (i = 0; i < current->modelId; i++) {
                    while ((*cursor & 0xF0000000) != 0xD0000000 && (*cursor & 0xF0000000) != 0xF0000000) {
                        cursor++;
                    }
                    cursor += 5;
                }
                node->unk0 = *cursor++;
                node->unk4 = *cursor++;
                node->unk8 = *cursor++;
                if ((*cursor & 0xF0000000) == 0xD0000000) {
                    node->unkC = *cursor & 0xFFFF;
                    modelUnk6F[modelIndex] = ((u8 *)cursor)[2];
                    cursor++;
                    node->unk2C = *cursor;
                    for (k = 0; k < 6; k++) {
                        D_800DD6D0[modelIndex].unk10[k] = -1;
                    }
                    cursor++;
                } else if ((*cursor & 0xF0000000) == 0xA0000000) {
                    D_800DD6D0[modelIndex].unk10[0] = *cursor;
                    cursor++;
                    for (k = 1; k < 6; k++) {
                        D_800DD6D0[modelIndex].unk10[k] = -1;
                    }
                    cursor++;
                    D_800DD6D0[modelIndex].unk28 = *cursor++;
                    node->unkC = -1;
                    modelUnk6F[modelIndex] = 0x10;
                } else {
                    node->unkC = -1;
                    modelUnk6F[modelIndex] = 0x10;
                    for (k = 0; *cursor >= 0; k++) {
                        D_800DD6D0[modelIndex].unk10[k] = *cursor++ & 0xFFFFFFF;
                    }
                    for (; k < 6; k++) {
                        D_800DD6D0[modelIndex].unk10[k] = -1;
                    }
                    cursor++;
                    D_800DD6D0[modelIndex].unk28 = *cursor++;
                }
                {
                    func_800AA8A0_pos *colorOffset = &colorOffsets[modelIndex];
                    word = cursor[1];
                    modelIndex++;
                    colorOffset->unk0 = word & 0xFF;
                    colorOffset->unk2 = (word >> 8) & 0xFF;
                    colorOffset->unk4 = (word >> 0x10) & 0xFF;
                    colorOffset->unk0 -= 0x80;
                    colorOffset->unk2 -= 0x80;
                    colorOffset->unk4 -= 0x80;
                }
            }
        }
    }
    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    actor = g_fieldActors;
    modelIndex = 0;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        if (actor->modelId != -1 && D_800D5EF0[actor->modelId] == 0xFF) {
            D_800D5EF0[actor->modelId] = modelIndex;
            if (D_800DD6D0[modelIndex].unkC != -1) {
                if (D_800D61A8[D_800D60F8[D_800DD6D0[modelIndex].unkC]].unk6F != modelUnk6F[modelIndex]) {
                    D_800D60F8[D_800DD6D0[modelIndex].unkC] = -1;
                }
                if (D_800D60F8[D_800DD6D0[modelIndex].unkC] != -1) {
                    FastMemCpy(freeSpace, D_800D6198[D_800D60F8[D_800DD6D0[modelIndex].unkC]], D_800D61A8[D_800D60F8[D_800DD6D0[modelIndex].unkC]].size);
                    D_800DD6D0[modelIndex].unk28 = (s32)freeSpace;
                    D_800D6620[modelIndex] = freeSpace;
                    freeSpace = (u8 *)freeSpace + D_800D61A8[D_800D60F8[D_800DD6D0[modelIndex].unkC]].size;
                }
            }
            modelIndex++;
        }
    }

    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    actor = g_fieldActors;
    modelIndex = 0;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        if (actor->modelId != -1 && D_800D5EF0[actor->modelId] == 0xFF) {
            D_800D5EF0[actor->modelId] = modelIndex;
            func_800ADAF4(arg4->sector + ((u32)D_800DD6D0[modelIndex].unk0 >> 0xB), D_800DD6D0[modelIndex].unk8, D_800DD6D0[modelIndex].unk4);
            modelIndex++;
        }
    }
    func_800ADB68(loadCursor, 0);

    {
        s32 loadedSize = 0;
        for (i = 0; i < 0x40; i++) {
            D_800D5EF0[i] = 0xFF;
        }
        actor = g_fieldActors;
        modelIndex = 0;
        for (i = 0; i < g_fieldActorCount; i++, actor++) {
            if (actor->modelId != -1 && D_800D5EF0[actor->modelId] == 0xFF) {
                D_800D5EF0[actor->modelId] = modelIndex;
                if (D_800DD6D0[modelIndex].unkC == -1) {
                    header = (MchModelHeader *)(D_800DD6D0[modelIndex].unk28 = D_800DD6D0[modelIndex].unk28 + (s32)loadCursor + loadedSize);
                    D_800DD6D0[modelIndex].unk2C = (s32)header->animationSection;
                    header->animationSection = (u16 *)((u8 *)header->animationSection + D_800DD6D0[modelIndex].unk28);
                } else {
                    D_800DD6D0[modelIndex].unk2C = (s32)loadCursor + loadedSize;
                }
                for (k = 0; D_800DD6D0[modelIndex].unk10[k] != -1 && (D_800DD6D0[modelIndex].unk10[k] & 0xF0000000) != 0xA0000000 && k < 6; k++) {
                    D_800DD6D0[modelIndex].unk10[k] = (D_800DD6D0[modelIndex].unk10[k] & 0xFFFFFFF) + (s32)loadCursor + loadedSize;
                }
                loadedSize += D_800DD6D0[modelIndex].unk4;
                modelIndex++;
            }
        }
        loadCursor += loadedSize;
    }

    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    actor = g_fieldActors;
    modelIndex = 0;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        if (actor->modelId != -1 && D_800D5EF0[actor->modelId] == 0xFF) {
            D_800D5EF0[actor->modelId] = modelIndex;
            if (D_800DD6D0[modelIndex].unkC != -1 && D_800D60F8[D_800DD6D0[modelIndex].unkC] == -1) {
                s32 fileSize;
                cdReadSync(arg6[D_800DD6D0[modelIndex].unkC].sector,
                    arg6[D_800DD6D0[modelIndex].unkC].size, loadCursor, 0);
                cursor = (s32 *)loadCursor;
                fileSize = arg6[D_800DD6D0[modelIndex].unkC].size;
                for (k = 0; *cursor >= 0; k++, cursor++) {
                    D_800DD6D0[modelIndex].unk10[k] = (s32)loadCursor + (*cursor & 0xFFFFFFF);
                }
                for (; k < 6; k++) {
                    D_800DD6D0[modelIndex].unk10[k] = -1;
                }
                D_800DD6D0[modelIndex].unk28 = (s32)loadCursor + (cursor[1] & 0xFFFFFFF);
                ((MchModelHeader *)D_800DD6D0[modelIndex].unk28)->animationSection = (u16 *)D_800DD6D0[modelIndex].unk2C;
                loadCursor += fileSize;
            }
            modelIndex++;
        }
    }

    if (arg7 < loadCursor) {
        SystemError(0x53, 0x13);
    }
    {
        word = arg7 - loadCursor;
        if (word & 3) {
            word -= (word & 3);
        }
        memmove(arg1 + word, arg1, loadCursor - arg1);

        for (i = 0; i < 0x40; i++) {
            D_800D5EF0[i] = 0xFF;
        }
        actor = g_fieldActors;
        modelIndex = 0;
        for (i = 0; i < g_fieldActorCount; i++, actor++) {
            if (D_800D5EF0[actor->modelId] == 0xFF) {
                D_800D5EF0[actor->modelId] = modelIndex;
                {
                    header = (MchModelHeader *)(D_800DD6D0[modelIndex].unk28 = D_800DD6D0[modelIndex].unk28 + word);
                    D_800DD6D0[modelIndex].unk2C += word;
                    header->animationSection = (u16 *)((u8 *)header->animationSection + word);
                }
                for (k = 0; k < 6; k++) {
                    if (D_800DD6D0[modelIndex].unk10[k] != -1 && (D_800DD6D0[modelIndex].unk10[k] & 0xF0000000) != 0xA0000000) {
                        D_800DD6D0[modelIndex].unk10[k] += word;
                    }
                }
                modelIndex++;
            }
        }
    }

    for (i = 0; i < 3; i++) {
        D_800D9628[i] = 0xFF;
        D_800DD6C4[i] = 0xFF;
    }

    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    actor = g_fieldActors;
    modelIndex = 0;
    cacheSlotIndex = 0;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        if (cacheSlotIndex >= 3) {
            break;
        }
        if (D_800D5EF0[actor->modelId] == 0xFF) {
            D_800D5EF0[actor->modelId] = modelIndex;
            if (D_800DD6D0[modelIndex].unkC != -1 && D_800D60F8[D_800DD6D0[modelIndex].unkC] != -1) {
                D_800D9628[D_800D60F8[D_800DD6D0[modelIndex].unkC]] = D_800DD6D0[modelIndex].unkC;
                D_800DD6C4[D_800D60F8[D_800DD6D0[modelIndex].unkC]] = D_800D60F8[D_800DD6D0[modelIndex].unkC];
                cacheIndex = D_800D60F8[D_800DD6D0[modelIndex].unkC];
                for (k = 0; k < D_800D61A8[cacheIndex].unk6C; k++) {
                    word = D_800D61A8[cacheIndex].unkC[k].x | (D_800D61A8[cacheIndex].unkC[k].y << 16);
                    for (j = 0; j < 0x10; j++) {
                        if (texturePagePositions[j] == word) {
                            texturePagePositions[j] = -1;
                        }
                    }
                    word = D_800D61A8[cacheIndex].unk3C[k].x | (D_800D61A8[cacheIndex].unk3C[k].y << 16);
                    for (j = 0; j < 0x10; j++) {
                        if (clutPositions[j] == word) {
                            clutPositions[j] = -1;
                        }
                    }
                }
            }
            modelIndex++;
        }
    }

    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    actor = g_fieldActors;
    modelIndex = 0;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        if (D_800D5EF0[actor->modelId] == 0xFF) {
            D_800D5EF0[actor->modelId] = modelIndex;
            if (D_800DD6D0[modelIndex].unkC != -1 && D_800D60F8[D_800DD6D0[modelIndex].unkC] == -1) {
                func_800AA8A0_node *node;
                cacheSlotIndex = 0;
                node = &D_800DD6D0[modelIndex];
                for (; cacheSlotIndex < 3; cacheSlotIndex++) {
                    if (D_800D9628[cacheSlotIndex] == 0xFF) {
                        D_800D9628[cacheSlotIndex] = node->unkC;
                        D_800DD6C4[cacheSlotIndex] = 0xFF;
                        break;
                    }
                }
            }
            modelIndex++;
        }
    }

    for (i = 0; i < 0x40; i++) {
        D_800D5EF0[i] = 0xFF;
    }
    actor = g_fieldActors;
    modelIndex = 0;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        s32 p2C;
        SoundBufDesc *cacheSlot;
        if (D_800D5EF0[actor->modelId] != 0xFF) {
            continue;
        }
        D_800D5EF0[actor->modelId] = modelIndex;
        for (D_800DD6BC = 0; D_800DD6BC < 3; D_800DD6BC++) {
            if (D_800DD6D0[modelIndex].unkC == D_800D9628[D_800DD6BC] && D_800DD6C4[D_800DD6BC] != 0xFF) {
                break;
            }
        }
        if (D_800DD6BC < 3) {
            cacheIndex = D_800DD6C4[D_800DD6BC];
            for (k = 0; k < D_800D61A8[cacheIndex].unk6C; k++) {
                D_800D6300[modelIndex][k] = GetTPage(1, 1, D_800D61A8[cacheIndex].unkC[k].x, D_800D61A8[cacheIndex].unkC[k].y);
                texturePageX[k] = D_800D61A8[cacheIndex].unkC[k].x;
                texturePageY[k] = D_800D61A8[cacheIndex].unkC[k].y;
                D_800D9328[modelIndex][k] = GetClut(D_800D61A8[cacheIndex].unk3C[k].x, D_800D61A8[cacheIndex].unk3C[k].y);
                D_800D60E8[k] = D_800D61A8[cacheIndex].unkC[k].y | ((D_800D61A8[cacheIndex].unkC[k].x % 0x40) << 1);
                D_800D5F50[D_800DD6BC].tag = D_800DD6D0[modelIndex].unkC;
                D_800D5F50[D_800DD6BC].unkC[k].x = D_800D61A8[cacheIndex].unkC[k].x;
                D_800D5F50[D_800DD6BC].unkC[k].y = D_800D61A8[cacheIndex].unkC[k].y;
                D_800D5F50[D_800DD6BC].unkC[k].w = D_800D61A8[cacheIndex].unkC[k].w;
                D_800D5F50[D_800DD6BC].unkC[k].h = D_800D61A8[cacheIndex].unkC[k].h;
                D_800D5F50[D_800DD6BC].unk3C[k].x = D_800D61A8[cacheIndex].unk3C[k].x;
                D_800D5F50[D_800DD6BC].unk3C[k].y = D_800D61A8[cacheIndex].unk3C[k].y;
                D_800D5F50[D_800DD6BC].unk3C[k].w = 0x100;
                D_800D5F50[D_800DD6BC].unk3C[k].h = 1;
                D_800D5F50[D_800DD6BC].unk6C = D_800D61A8[cacheIndex].unk6C;
                for (j = 0; j < 0x100; j++) {
                    s16 red, green, blue;
                    s32 r, g, b;
                    u16 color;
                    color = D_800D8728[cacheIndex][k][j];
                    if (color != 0) {
                        r = color & 0x1F;
                        g = (color >> 5) & 0x1F;
                        b = (color >> 10) & 0x1F;
                        red = r + (colorOffsets[modelIndex].unk0 >> 3);
                        if (red <= 0) {
                            red = 1;
                        }
                        if (red >= 0x20) {
                            red = 0x1F;
                        }
                        green = g + (colorOffsets[modelIndex].unk2 >> 3);
                        if (green <= 0) {
                            green = 1;
                        }
                        if (green >= 0x20) {
                            green = 0x1F;
                        }
                        blue = b + (colorOffsets[modelIndex].unk4 >> 3);
                        if (blue <= 0) {
                            blue = 1;
                        }
                        if (blue >= 0x20) {
                            blue = 0x1F;
                        }
                        D_800DE2D0[j] = red | (green << 5) | (blue << 10) | FUNC_800AA8A0_COLOR_BIT15;
                    } else {
                        D_800DE2D0[j] = 0;
                    }
                }
                LoadClut((u32 *)D_800DE2D0, D_800D61A8[cacheIndex].unk3C[k].x, D_800D61A8[cacheIndex].unk3C[k].y);
                while (DrawSync(1) != 0) {
                }
            }
        } else {
            k = 0;
            if (D_800DD6D0[modelIndex].unk10[k] != -1) {
                for (; D_800DD6D0[modelIndex].unk10[k] != -1; k++) {
                    if (k >= 6) {
                        break;
                    }
                    if ((D_800DD6D0[modelIndex].unk10[k] & 0xF0000000) == 0xA0000000) {
                        D_800D6300[modelIndex][k] = D_800D6300[D_800D5EF0[((u32)D_800DD6D0[modelIndex].unk10[k] & 0xFF00000) >> 20]][k];
                        D_800D9328[modelIndex][k] = D_800D9328[D_800D5EF0[((u32)D_800DD6D0[modelIndex].unk10[k] & 0xFF00000) >> 20]][k];
                        D_800D60E8[k] = D_800D6620[D_800D5EF0[((u32)D_800DD6D0[modelIndex].unk10[k] & 0xFF00000) >> 20]]->field50;
                    } else {
                        while (texturePagePositions[texturePagePositionIndex] == -1) {
                            texturePagePositionIndex++;
                        }
                        while (clutPositions[clutPositionIndex] == -1) {
                            clutPositionIndex++;
                        }
                        OpenTIM((u32 *)D_800DD6D0[modelIndex].unk10[k]);
                        ReadTIM(&tim);
                        VSync(0);
                        {
                            u32 *texturePagePosition = &texturePagePositions[texturePagePositionIndex];
                            D_800D6300[modelIndex][k] = LoadTPage(tim.unk10, tim.unk0 & 7, 1, *texturePagePosition & 0xFFFF, *texturePagePosition >> 16,
                                tim.unkC->unk4 << (2 - (tim.unk0 & 3)), tim.unkC->unk6);
                            texturePageX[k] = texturePagePositions[texturePagePositionIndex];
                            texturePageY[k] = texturePagePositions[texturePagePositionIndex] >> 16;
                        }
                        palette = (u16 *)tim.unk8;
                        for (D_800DD6BC = 0; D_800DD6BC < 3; D_800DD6BC++) {
                            if (D_800DD6D0[modelIndex].unkC == D_800D9628[D_800DD6BC] && D_800DD6C4[D_800DD6BC] == 0xFF) {
                                break;
                            }
                        }
                        if (D_800DD6BC < 3 && D_800DD6D0[modelIndex].unkC != -1) {
                            for (cacheIndex = 0; cacheIndex < 0x100; cacheIndex++, palette++) {
                                D_800D8728[D_800DD6BC][k][cacheIndex] = *palette;
                            }
                        }
                        palette = (u16 *)tim.unk8;
                        for (cacheIndex = 0; cacheIndex < 0x100; cacheIndex++, palette++) {
                            s16 red, green, blue;
                            s32 r, g, b;
                            u16 color;
                            color = *palette;
                            if (color != 0) {
                                r = color & 0x1F;
                                g = (color >> 5) & 0x1F;
                                b = (color >> 10) & 0x1F;
                                red = r + (colorOffsets[modelIndex].unk0 >> 3);
                                if (red <= 0) {
                                    red = 1;
                                }
                                if (red >= 0x20) {
                                    red = 0x1F;
                                }
                                green = g + (colorOffsets[modelIndex].unk2 >> 3);
                                if (green <= 0) {
                                    green = 1;
                                }
                                if (green >= 0x20) {
                                    green = 0x1F;
                                }
                                blue = b + (colorOffsets[modelIndex].unk4 >> 3);
                                if (blue <= 0) {
                                    blue = 1;
                                }
                                if (blue >= 0x20) {
                                    blue = 0x1F;
                                }
                                *palette = red | (green << 5) | (blue << 10) | FUNC_800AA8A0_COLOR_BIT15;
                            }
                        }
                        D_800D9328[modelIndex][k] = LoadClut(tim.unk8, clutPositions[clutPositionIndex] & 0xFFFF, clutPositions[clutPositionIndex] >> 16);
                        D_800D60E8[k] = ((texturePagePositions[texturePagePositionIndex] & 0x3F) << (2 - (tim.unk0 & 3))) | ((texturePagePositions[texturePagePositionIndex] >> 8) & 0xFF00);
                        for (D_800DD6BC = 0; D_800DD6BC < 3; D_800DD6BC++) {
                            if (D_800DD6D0[modelIndex].unkC == D_800D9628[D_800DD6BC] && D_800DD6C4[D_800DD6BC] == 0xFF) {
                                break;
                            }
                        }
                        if (D_800DD6BC < 3 && D_800DD6D0[modelIndex].unkC != -1) {
                            D_800D5F50[D_800DD6BC].tag = -1;
                            D_800D5F50[D_800DD6BC].unkC[k].x = texturePagePositions[texturePagePositionIndex];
                            D_800D5F50[D_800DD6BC].unkC[k].y = texturePagePositions[texturePagePositionIndex] >> 16;
                            D_800D5F50[D_800DD6BC].unkC[k].w = tim.unkC->unk4 << (2 - (tim.unk0 & 3));
                            D_800D5F50[D_800DD6BC].unkC[k].h = tim.unkC->unk6;
                            D_800D5F50[D_800DD6BC].unk3C[k].x = clutPositions[clutPositionIndex];
                            D_800D5F50[D_800DD6BC].unk3C[k].y = clutPositions[clutPositionIndex] >> 16;
                            D_800D5F50[D_800DD6BC].unk3C[k].w = 0x100;
                            D_800D5F50[D_800DD6BC].unk3C[k].h = 1;
                            D_800D5F50[D_800DD6BC].unk6C = k + 1;
                        }
                        texturePagePositionIndex++;
                        clutPositionIndex++;
                        while (DrawSync(1) != 0) {
                        }
                    }
                }
            }

        }
        if (D_800DD6BC < 3 && D_800DD6D0[modelIndex].unkC != -1) {
            if (D_800D5F50[D_800DD6BC].tag == -1) {
                u8 *unk6FPtr;
                Model *model;
                header = (MchModelHeader *)D_800DD6D0[modelIndex].unk28;
                if ((void *)header->animationSection < freeSpace) {
                    SystemError(0x53, 0x1A);
                }
                if ((u32)D_800DD6D0[modelIndex].unk28 < (u32)freeSpace) {
                    SystemError(0x53, 0x12);
                }
                unk6FPtr = &modelUnk6F[modelIndex];
                j = (s32)freeSpace; // matching hack
                freeSpace = func_800A8058(modelIndex, (MchModelHeader *)D_800DD6D0[modelIndex].unk28, freeSpace, *unk6FPtr);
                cacheSlot = &D_800D5F50[D_800DD6BC];
                cacheSlot->tag = D_800DD6D0[modelIndex].unkC;
                cacheSlot->src = (u8 *)j;
                model = D_800D6620[modelIndex];
                cacheSlot->size = ((u8 *)model->animationTable - (u8 *)model->boneTemplate) + 0x60;
                cacheSlot->unk6E = D_800DD6BC;
                if (*unk6FPtr != 0) {
                    D_800D5F50[D_800DD6BC].unk6F = *unk6FPtr;
                } else {
                    D_800D5F50[D_800DD6BC].unk6F = 0x10;
                }
                if (D_800D6620[modelIndex]->flags & MODEL_FLAG_TEXTURE_ANIMATION) {
                    D_800D6620[modelIndex]->textureAnimation->destX /= 0x10 / D_800D6620[modelIndex]->textureAnimation->unk2;
                    D_800D6620[modelIndex]->textureAnimation->destX += texturePageX[0];
                    D_800D6620[modelIndex]->textureAnimation->unk8 += texturePageY[0];
                    D_800D6620[modelIndex]->textureAnimation->width /= 0x10 / D_800D6620[modelIndex]->textureAnimation->unk2;
                    for (k = j = 0; k < D_800D6620[modelIndex]->textureAnimation->frameCount; k++) {
                        D_800D6620[modelIndex]->textureAnimation->frameCoords[j] /= 0x10 / D_800D6620[modelIndex]->textureAnimation->unk2;
                        D_800D6620[modelIndex]->textureAnimation->frameCoords[j] += texturePageX[0];
                        j++;
                        D_800D6620[modelIndex]->textureAnimation->frameCoords[j] += texturePageY[0];
                        j++;
                    }
                }
            } else {
                D_800D5F50[D_800DD6BC].src = (u8 *)D_800D6620[modelIndex];
                D_800D5F50[D_800DD6BC].size = D_800D61A8[D_800D60F8[D_800DD6D0[modelIndex].unkC]].size;
                D_800D5F50[D_800DD6BC].unk6E = D_800DD6BC;
                if (modelUnk6F[modelIndex] != 0) {
                    D_800D5F50[D_800DD6BC].unk6F = modelUnk6F[modelIndex];
                } else {
                    D_800D5F50[D_800DD6BC].unk6F = 0x10;
                }
                j = D_800D61A8[D_800D60F8[D_800DD6D0[modelIndex].unkC]].src - (u8 *)D_800D6620[modelIndex];
                D_800D6620[modelIndex]->boneTemplate = (MchBone *)((u8 *)D_800D6620[modelIndex]->boneTemplate - j);
                D_800D6620[modelIndex]->unk04 = (ModelUnk04 *)((u8 *)D_800D6620[modelIndex]->unk04 - j);
                D_800D6620[modelIndex]->unk08 = (ModelUnk08 *)((u8 *)D_800D6620[modelIndex]->unk08 - j);
                D_800D6620[modelIndex]->triangles = (ModelTriangle *)((u8 *)D_800D6620[modelIndex]->triangles - j);
                D_800D6620[modelIndex]->quads = (ModelQuad *)((u8 *)D_800D6620[modelIndex]->quads - j);
                D_800D6620[modelIndex]->textureAnimation = (MchTextureAnimation *)((u8 *)D_800D6620[modelIndex]->textureAnimation - j);
                D_800D6620[modelIndex]->vertices = (SVECTOR *)((u8 *)D_800D6620[modelIndex]->vertices - j);
                D_800D6620[modelIndex]->drawGroups = (MchDrawGroup *)((u8 *)D_800D6620[modelIndex]->drawGroups - j);
                D_800D6620[modelIndex]->skinObjects = (MchSkinObject *)((u8 *)D_800D6620[modelIndex]->skinObjects - j);
                cursor = (s32 *)D_800DD6D0[modelIndex].unk2C;
                D_800D6620[modelIndex]->animationCount = *(u16 *)cursor;
                D_800D6620[modelIndex]->animationTable = freeSpace;
                D_800D6620[modelIndex]->animations = (SVECTOR *)(D_800D6620[modelIndex]->animationTable + D_800D6620[modelIndex]->animationCount * 2);
                freeSpace = func_800A7564((MchModelHeader *)((s16 *)cursor + 1), 0x12, 0, modelIndex);
            }
        } else {
            header = (MchModelHeader *)D_800DD6D0[modelIndex].unk28;
            if ((void *)header->animationSection < freeSpace) {
                SystemError(0x53, 0x15);
            }
            if ((u32)D_800DD6D0[modelIndex].unk28 < (u32)freeSpace) {
                SystemError(0x53, 0x12);
            }
            freeSpace = func_800A8058(modelIndex, (MchModelHeader *)D_800DD6D0[modelIndex].unk28, freeSpace, modelUnk6F[modelIndex]);
            if (D_800D6620[modelIndex]->flags & MODEL_FLAG_TEXTURE_ANIMATION) {
                D_800D6620[modelIndex]->textureAnimation->destX /= 0x10 / D_800D6620[modelIndex]->textureAnimation->unk2;
                D_800D6620[modelIndex]->textureAnimation->destX += texturePageX[0];
                D_800D6620[modelIndex]->textureAnimation->unk8 += texturePageY[0];
                D_800D6620[modelIndex]->textureAnimation->width /= 0x10 / D_800D6620[modelIndex]->textureAnimation->unk2;
                for (k = j = 0; k < D_800D6620[modelIndex]->textureAnimation->frameCount; k++) {
                    D_800D6620[modelIndex]->textureAnimation->frameCoords[j] /= 0x10 / D_800D6620[modelIndex]->textureAnimation->unk2;
                    D_800D6620[modelIndex]->textureAnimation->frameCoords[j] += texturePageX[0];
                    j++;
                    D_800D6620[modelIndex]->textureAnimation->frameCoords[j] += texturePageY[0];
                    j++;
                }
            }
        }
        modelIndex++;
    }

    for (cacheIndex = 0; cacheIndex < 3; cacheIndex++) {
        D_800D61A8[cacheIndex] = D_800D5F50[cacheIndex];
    }

    actor = g_fieldActors;
    for (i = 0; i < g_fieldActorCount; i++, actor++) {
        if (actor->modelId < 0) {
            freeSpace = func_800A8CDC(i, actor->modelId, freeSpace);
        } else {
            freeSpace = func_800A81AC(i, D_800D5EF0[actor->modelId], freeSpace);
        }
    }

    return freeSpace;
}

