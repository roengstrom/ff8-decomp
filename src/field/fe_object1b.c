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
#include "field.h"
#include "psxsdk/libapi.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "field/fe_object1.h"
#include "field/fe_object1b.h"
#include "field/fe_object2.h"

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

extern u32 D_800C3768;
extern s32 D_800C376C;

extern POLY_G3 *D_800D5EE0[2];
extern POLY_G4 *D_800D5EE8[2];
extern POLY_FT3 *D_800D6600[2];
extern POLY_FT4 *D_800D6610[2];
extern u16 D_800D6300[][6];
extern func_800A7564_arg0 *D_800D60A0;
extern MchBone *D_800D60A4;
extern u16 D_800D6720[];
extern u16 D_800D9328[][6];
extern u32 D_800D96B0[];
extern SVECTOR *D_800DD6C0;
extern long D_800DD6C8;
extern u8 D_800DD6D0[];

void func_800A63AC(FieldFrameBuf *arg0, FieldView *arg1, s32 arg2) {
    s32 unused[12];
    s32 i;
    s32 j;
    s32 k;
    s32 idx;
    s32 nFt3;
    s32 nFt4;
    POLY_G3 *src;
    POLY_G3 *dst;
    POLY_FT3 *ft3Src;
    POLY_FT3 *ft3Dst;
    POLY_FT4 *ft4Src;
    POLY_FT4 *ft4Dst;
    s32 offA;
    s32 offB;

    for (i = 0; i < g_fieldActorCount; i++) {
        if (D_800D9630[i]->unk00.modelId != NO_MODEL) {
            func_800AA5F8(i);
        }
    }

    for (i = 0; i < g_fieldActorCount; i++) {
        if (D_800D9630[i]->unk00.modelId != NO_MODEL) {
            if (D_800D9630[i]->unk60 & ENTITY_RENDER_FLAG_10) {
                D_800D9630[i]->unk60 &= ~ENTITY_RENDER_FLAG_10;
                D_800D9630[i]->unk60 |= ENTITY_RENDER_FLAG_8;
                dst = D_800D9630[i]->unk58[0];
                src = D_800D9630[i]->unk54;
                if (src == dst) {
                    dst = D_800D9630[i]->unk58[1];
                }
                idx = 0;
                nFt3 = 0;
                nFt4 = 0;
                offA = D_800D9630[i]->unk00.def->unk30 * sizeof(POLY_G3);
                offB = D_800D9630[i]->unk00.def->unk34 * sizeof(POLY_G4);
                ft3Src = (POLY_FT3 *)((u8 *)src + offA + offB);
                ft3Dst = (POLY_FT3 *)((u8 *)dst + offA + offB);
                for (k = 0; k < D_800D9630[i]->unk00.def->drawGroupCount; k++) {
                    for (j = 0; j < D_800D9630[i]->unk00.def->drawGroups[k].triangleCount; j++) {
                        if (D_800D9630[i]->unk80[idx] != 0) {
                            ft3Dst[nFt3] = ft3Src[nFt3];
                        }
                        nFt3++;
                        idx++;
                    }
                    ft4Src = (POLY_FT4 *)(ft3Src + D_800D9630[i]->unk00.def->unk38);
                    ft4Dst = (POLY_FT4 *)(ft3Dst + D_800D9630[i]->unk00.def->unk38);
                    for (j = 0; j < D_800D9630[i]->unk00.def->drawGroups[k].quadCount; j++) {
                        if (D_800D9630[i]->unk80[idx] != 0) {
                            ft4Dst[nFt4] = ft4Src[nFt4];
                        }
                        nFt4++;
                        idx++;
                    }
                }
            }
            if (!(D_800D9630[i]->unk60 & ENTITY_RENDER_FLAG_1) && D_800D9630[i]->unk08 == D_800D9630[i]->unk78 && D_800D9630[i]->unk52 == D_800D9630[i]->unk7C) {
                if (!(D_800D9630[i]->unk60 & (ENTITY_RENDER_FLAG_8 | ENTITY_RENDER_FLAG_10))) {
                    D_800D9630[i]->unk60 |= ENTITY_RENDER_FLAG_10;
                }
            } else {
                D_800D9630[i]->unk60 &= ~(ENTITY_RENDER_FLAG_8 | ENTITY_RENDER_FLAG_10);
                D_800D9630[i]->unk78 = D_800D9630[i]->unk08;
                D_800D9630[i]->unk7C = D_800D9630[i]->unk52;
            }
        }
    }

    func_800ACBD4(&arg0->ot[1], arg1);

    for (i = 0; i < g_fieldActorCount; i++) {
        if (D_800D9630[i]->unk00.modelId != NO_MODEL && (D_800D9630[i]->unk60 & ENTITY_RENDER_FLAG_8)) {
            idx = 0;
            nFt3 = 0;
            nFt4 = 0;
            ft3Src = (POLY_FT3 *)((POLY_G4 *)(D_800D9630[i]->unk54 + D_800D9630[i]->unk00.def->unk30) + D_800D9630[i]->unk00.def->unk34);
            for (k = 0; k < D_800D9630[i]->unk00.def->drawGroupCount; k++) {
                for (j = 0; j < D_800D9630[i]->unk00.def->drawGroups[k].triangleCount; j++) {
                    if (D_800D9630[i]->unk80[idx] != 0) {
                        setaddr(&ft3Src[nFt3], getaddr(&arg0->ot[D_800D9630[i]->unk80[idx]]));
                        setaddr(&arg0->ot[D_800D9630[i]->unk80[idx]], &ft3Src[nFt3]);
                    }
                    idx++;
                    nFt3++;
                }
                ft4Src = (POLY_FT4 *)(ft3Src + D_800D9630[i]->unk00.def->unk38);
                for (j = 0; j < D_800D9630[i]->unk00.def->drawGroups[k].quadCount; j++) {
                    if (D_800D9630[i]->unk80[idx] != 0) {
                        setaddr(&ft4Src[nFt4], getaddr(&arg0->ot[D_800D9630[i]->unk80[idx]]));
                        setaddr(&arg0->ot[D_800D9630[i]->unk80[idx]], &ft4Src[nFt4]);
                    }
                    idx++;
                    nFt4++;
                }
            }
        }
    }
}

void func_800A6A80(s32 arg0, s32 arg1, ModelInstance **arg2) {
    MATRIX m;
    SVECTOR step;
    VECTOR v;
    u16 nFt3;
    u16 nFt4;
    s32 indexOffset;
    s32 i;
    s32 vertCount;
    s32 partIdx;
    MchDrawGroup *rec;
    s32 j;
    ModelInstance *slot;
    Model *def;
    s32 vertIdx;

    nFt3 = 0;
    nFt4 = 0;
    indexOffset = 0;
    for (i = 0; i < (*arg2)->unk00.def->drawGroupCount; i++) {
        vertCount = 0;
        for (partIdx = (*arg2)->unk00.def->drawGroups[i].firstSkinObjectIndex; partIdx < (*arg2)->unk00.def->drawGroups[i].firstSkinObjectIndex + (*arg2)->unk00.def->drawGroups[i].skinObjectCount; partIdx++) {
            if ((*arg2)->unk00.def->drawGroups[i].unk8.pad & MCH_DRAW_GROUP_FLAG_1) {
                m.t[0] = 0;
                m.t[1] = 0;
                m.t[2] = 0;
                SetRotMatrix(&(*arg2)->bones[(*arg2)->unk00.def->skinObjects[partIdx].boneId].transform);
                SetTransMatrix(&m);
                rec = &(*arg2)->unk00.def->drawGroups[i];
                step.vx = (rec->unk4 * (rec->unk18.vx - rec->unk8.vx)) / rec->unk18.pad;
                step.vy = (rec->unk4 * (rec->unk18.vy - rec->unk8.vy)) / rec->unk18.pad;
                step.vz = (rec->unk4 * (rec->unk18.vz - rec->unk8.vz)) / rec->unk18.pad;
                RotTrans(&step, &v, &D_800DD6C8);
                m = (*arg2)->bones[(*arg2)->unk00.def->skinObjects[partIdx].boneId].transform;
                m.t[0] += v.vx;
                m.t[1] += v.vy;
                m.t[2] += v.vz;
                RotTrans(&rec->unk8, &v, &D_800DD6C8);
                m.t[0] += v.vx;
                m.t[1] += v.vy;
                m.t[2] += v.vz;
                SetRotMatrix(&m);
                SetTransMatrix(&m);
                if (rec->unk18.pad < ++rec->unk4) {
                    rec->unk8.pad = (rec->unk8.pad ^ MCH_DRAW_GROUP_FLAG_1) | MCH_DRAW_GROUP_FLAG_4;
                } else {
                    (*arg2)->unk78 = (SVECTOR *)-1;
                }
            } else if ((*arg2)->unk00.def->drawGroups[i].unk8.pad & MCH_DRAW_GROUP_FLAG_4) {
                m.t[0] = 0;
                m.t[1] = 0;
                m.t[2] = 0;
                SetRotMatrix(&(*arg2)->bones[(*arg2)->unk00.def->skinObjects[partIdx].boneId].transform);
                SetTransMatrix(&m);
                rec = &(*arg2)->unk00.def->drawGroups[i];
                RotTrans(&rec->unk18, &v, &D_800DD6C8);
                m = (*arg2)->bones[(*arg2)->unk00.def->skinObjects[partIdx].boneId].transform;
                m.t[0] += v.vx;
                m.t[1] += v.vy;
                m.t[2] += v.vz;
                SetRotMatrix(&m);
                SetTransMatrix(&m);
            } else {
                SetRotMatrix(&(*arg2)->bones[(*arg2)->unk00.def->skinObjects[partIdx].boneId].transform);
                SetTransMatrix(&(*arg2)->bones[(*arg2)->unk00.def->skinObjects[partIdx].boneId].transform);
            }
            slot = *arg2;
            def = slot->unk00.def;
            vertIdx = def->skinObjects[partIdx].firstVertexIndex;
            if (!(slot->unk60 & ENTITY_RENDER_FLAG_20) || def->skinObjects[partIdx].boneId != slot->unk73 + 1) {
                func_800AD048(&def->unk18[vertIdx], &D_800D96B0[vertCount], &D_800D6720[vertCount], def->skinObjects[partIdx].vertexCount);
            } else {
                for (j = 0; j < (*arg2)->unk00.def->skinObjects[partIdx].vertexCount; j++) {
                    D_800D96B0[vertCount + j] = 0;
                    D_800D6720[vertCount + j] = 0;
                }
            }
            vertCount += (*arg2)->unk00.def->skinObjects[partIdx].vertexCount;
        }
        func_800AD0E8((POLY_FT3 *)((POLY_G4 *)((*arg2)->unk54 + (*arg2)->unk00.def->unk30) + (*arg2)->unk00.def->unk34) + nFt3, &(*arg2)->unk00.def->unk0C[nFt3], &(*arg2)->unk80[indexOffset], (*arg2)->unk00.def->drawGroups[i].triangleCount);
        nFt3 += (*arg2)->unk00.def->drawGroups[i].triangleCount;
        indexOffset += (*arg2)->unk00.def->drawGroups[i].triangleCount;
        func_800AD300((POLY_FT4 *)((POLY_FT3 *)((POLY_G4 *)((*arg2)->unk54 + (*arg2)->unk00.def->unk30) + (*arg2)->unk00.def->unk34) + (*arg2)->unk00.def->unk38) + nFt4, &(*arg2)->unk00.def->unk10[nFt4], &(*arg2)->unk80[indexOffset], (*arg2)->unk00.def->drawGroups[i].quadCount);
        nFt4 += (*arg2)->unk00.def->drawGroups[i].quadCount;
        indexOffset += (*arg2)->unk00.def->drawGroups[i].quadCount;
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
    u8 *p = D_800DD6D0;

    do {
        memset(p, 0, 0x30);
        i++;
        p += 0x30;
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
 * of @c unk60 and refreshes the snapshot.
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
        slot->unk60 &= 0xE7;
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
 * of @c unk60 and refreshes the snapshot.
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
        slot->unk60 &= 0xE7;
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

void *func_800A7564(func_800A7564_arg0 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_800A7564_entry desc;
    void *ret;
    s32 count;
    s32 off;
    s32 *unk28p;
    func_800A7564_entry *src;
    s32 i;
    s32 j;
    s32 l;
    s32 k;
    s32 cursor;
    s32 idx;
    u32 packed;
    u16 *p;
    u16 *stream;
    u16 outer;
    u16 inner;
    u16 nFt3;
    u16 nFt4;

    if (arg1 == 0x12) {
        idx = arg3;
        p = (u16 *)arg0;
        k = 0;
        cursor = 0;
        for (i = 0; i < D_800D6620[idx]->unk52; i++) {
            outer = *p++;
            inner = *p++;
            D_800D6620[idx]->unk54[k++] = outer;
            D_800D6620[idx]->unk54[k++] = inner + 1;
            for (j = 0; j < outer; j++) {
                ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor].half.unk0 = *p++;
                ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor++].half.unk2 = *p++;
                ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor++].half.unk0 = *p++;
                for (l = 0; l < inner; l++) {
                    ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor].byte[0] = ((u8 *)p)[0];
                    ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor].byte[1] = ((u8 *)p)[1];
                    ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor].byte[2] = ((u8 *)p)[2];
                    ((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor].byte[3] = ((u8 *)p)[3];
                    p += 2;
                    cursor++;
                }
            }
        }
        return &((func_800A7564_unk58 *)D_800D6620[idx]->unk58)[cursor];
    }
    if (arg1 == 0x11) {
        D_800D6620[arg3]->drawGroupCount = arg0->unk10;
        D_800D60A0 = arg0;
        // matching hack
        unk28p = &D_800D6620[arg3]->unk28;
        *unk28p = D_800D60A0->unk14;
        D_800D6620[arg3]->unk2C = D_800D60A0->unk00;
        D_800D6620[arg3]->unk30 = D_800D60A0->unk18;
        D_800D6620[arg3]->unk34 = D_800D60A0->unk1A;
        D_800D6620[arg3]->unk38 = D_800D60A0->unk1C;
        D_800D6620[arg3]->unk3C = D_800D60A0->unk1E;
        count = D_800D60A0->unk0C;
        D_800C3768 = D_800D60A0->unk04;
        D_800C376C = D_800D60A0->unk08;
        idx = arg3;
        if (arg0->unk08 != 0) {
            D_800D6620[idx]->unk5F |= FIELD_OBJECT_FLAG_1;
        } else {
            D_800D6620[idx]->unk5F &= ~FIELD_OBJECT_FLAG_1;
        }
        D_800D6620[idx]->unk00 = (MchBone *)(D_800D6620[idx] + 1);
        D_800D6620[idx]->unk04 = (ModelUnk04 *)(D_800D6620[idx]->unk00 + D_800D60A0->unk00 + 1);
        D_800D6620[idx]->unk08 = (ModelUnk08 *)(D_800D6620[idx]->unk04 + D_800D6620[idx]->unk30);
        D_800D6620[idx]->unk0C = (ModelUnk0C *)(D_800D6620[idx]->unk08 + D_800D6620[idx]->unk34);
        D_800D6620[idx]->unk10 = (ModelUnk10 *)(D_800D6620[idx]->unk0C + D_800D6620[idx]->unk38);
        D_800D6620[idx]->anim = (MchTexAnim *)(D_800D6620[idx]->unk10 + D_800D6620[idx]->unk3C);
        D_800D6620[idx]->unk18 = (SVECTOR *)((u8 *)D_800D6620[idx]->anim + (D_800D60A0->unk08 << 3));
        D_800D6620[idx]->drawGroups = (MchDrawGroup *)(D_800D6620[idx]->unk18 + D_800D60A0->unk04);
        D_800D6620[idx]->skinObjects = (MchSkinObject *)(D_800D6620[idx]->drawGroups + D_800D60A0->unk10);
        D_800D60A4 = (MchBone *)((u8 *)arg0 + D_800D60A0->unk20);
        FastMemCpy((u8 *)(D_800D6620[idx]->unk00 + 1), (u8 *)D_800D60A4, D_800D6620[idx]->unk2C << 6);
        for (j = 0; j < D_800D6620[idx]->unk2C; j++) {
            D_800D6620[idx]->unk00[j + 1].boneVector.vz = (arg2 * D_800D60A4[j].boneVector.vz) >> 4;
            D_800D6620[idx]->unk00[j + 1].lockRotation = 0;
        }
        D_800DD6C0 = (SVECTOR *)((u8 *)arg0 + D_800D60A0->unk24);
        for (j = 0; j < D_800C3768; j++) {
            D_800D6620[idx]->unk18[j].vx = (arg2 * D_800DD6C0[j].vx) >> 4;
            D_800D6620[idx]->unk18[j].vy = (arg2 * D_800DD6C0[j].vy) >> 4;
            D_800D6620[idx]->unk18[j].vz = (arg2 * D_800DD6C0[j].vz) >> 4;
        }
        off = D_800D60A0->unk28;
        FastMemCpy((u8 *)D_800D6620[idx]->anim, (u8 *)arg0 + off, D_800D60A0->unk08);
        FastMemCpy((u8 *)D_800D6620[idx]->drawGroups, (u8 *)arg0 + D_800D60A0->unk30, D_800D6620[idx]->drawGroupCount << 5);
        FastMemCpy((u8 *)D_800D6620[idx]->skinObjects, (u8 *)arg0 + D_800D60A0->unk34, D_800D6620[idx]->unk28 << 3);
        stream = D_800D60A0->unk38;
        D_800D6620[idx]->unk52 = *stream++;
        D_800D6620[idx]->unk54 = (u16 *)(D_800D6620[idx]->skinObjects + D_800D60A0->unk14);
        D_800D6620[idx]->unk58 = (SVECTOR *)(D_800D6620[idx]->unk54 + D_800D6620[idx]->unk52 * 2);
        D_800D6620[idx]->unk40 = (SVECTOR *)(D_800D6620[idx]->unk54 + D_800D6620[idx]->unk52 * 2);
        ret = func_800A7564((func_800A7564_arg0 *)stream, 0x12, 0, idx);
        packed = D_800D60A0->unk3C;
        D_800D6620[idx]->unk5C = packed;
        D_800D6620[idx]->unk5D = packed >> 8;
        D_800D6620[idx]->unk5E = packed >> 16;
        src = (func_800A7564_entry *)((u8 *)arg0 + D_800D60A0->unk2C);
        nFt3 = 0;
        nFt4 = 0;
        if ((u32)src < (u32)ret) {
            SystemError(0x53, 0x14);
        }
        for (j = 0; j < count; j++) {
            desc = *src++;
            switch (desc.unk0) {
            case 0x25010607:
                D_800D6620[idx]->unk0C[nFt3].unk0 = desc.unkC;
                D_800D6620[idx]->unk0C[nFt3].unk2 = desc.unkE;
                D_800D6620[idx]->unk0C[nFt3].unk4 = desc.unk10;
                D_800D6620[idx]->unk0C[nFt3].unk6 = desc.unk2C + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk0C[nFt3].unk8 = desc.unk2E + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk0C[nFt3].unkA = desc.unk30 + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk0C[nFt3].unkC = D_800D9328[arg3][desc.unk36];
                D_800D6620[idx]->unk0C[nFt3].unkE = D_800D6300[arg3][desc.unk36];
                D_800D6620[idx]->unk0C[nFt3].unk10.r = 0x80;
                D_800D6620[idx]->unk0C[nFt3].unk10.g = 0x80;
                D_800D6620[idx]->unk0C[nFt3].unk10.b = 0x80;
                D_800D6620[idx]->unk0C[nFt3].unk10.cd = desc.unk1D;
                D_800D6620[idx]->unk0C[nFt3].unk14 = desc.unk8;
                nFt3++;
                break;
            case 0x2D010709:
                D_800D6620[idx]->unk10[nFt4].unk0 = desc.unkC;
                D_800D6620[idx]->unk10[nFt4].unk2 = desc.unkE;
                D_800D6620[idx]->unk10[nFt4].unk4 = desc.unk10;
                D_800D6620[idx]->unk10[nFt4].unk6 = desc.unk12;
                D_800D6620[idx]->unk10[nFt4].unk8 = desc.unk2C + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk10[nFt4].unkA = desc.unk2E + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk10[nFt4].unkC = desc.unk30 + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk10[nFt4].unkE = desc.unk32 + D_800D60E8[desc.unk36];
                D_800D6620[idx]->unk10[nFt4].unk10 = D_800D9328[arg3][desc.unk36];
                D_800D6620[idx]->unk10[nFt4].unk12 = D_800D6300[arg3][desc.unk36];
                D_800D6620[idx]->unk10[nFt4].unk14.r = 0x80;
                D_800D6620[idx]->unk10[nFt4].unk14.g = 0x80;
                D_800D6620[idx]->unk10[nFt4].unk14.b = 0x80;
                D_800D6620[idx]->unk10[nFt4].unk14.cd = desc.unk1D;
                D_800D6620[idx]->unk10[nFt4].unk18 = desc.unk8;
                nFt4++;
                break;
            }
        }
    }
    return ret;
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
void *func_800A8058(s32 idx, func_800A7564_arg0 *arg1, Model *newObj, u8 count) {
    void *ret;
    s32 i, j;

    if (D_800D6620[idx] != NULL) {
        return NULL;
    }
    if (count == 0) {
        count = 0x10;
    }
    D_800D6620[idx] = newObj;
    newObj->unk5F = 0;
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
    u16 *cursor;
    u32 misalign;
    POLY_G3 *g3;
    POLY_G4 *g4;
    POLY_FT4 *ft4;

    if (D_800D9630[arg0] != NULL) {
        return NULL;
    }
    D_800D9630[arg0] = arg2;
    arg2->unk00.def = D_800D6620[arg1];
    D_800D9630[arg0]->unk60 = 0;
    D_800D9630[arg0]->animFlags = 0;
    D_800D9630[arg0]->unk61 = 0x11;
    D_800D9630[arg0]->unk62 = 0;
    D_800D9630[arg0]->headBone = D_800D6620[arg1]->unk5C;
    D_800D9630[arg0]->bodyBone = D_800D6620[arg1]->unk5D;
    D_800D9630[arg0]->unk73 = D_800D6620[arg1]->unk5E;
    D_800D9630[arg0]->unk78 = (SVECTOR *)-1;
    D_800D9630[arg0]->unk7C = 0xFFFF;
    D_800D9630[arg0]->unk7E = 0;
    D_800D9630[arg0]->bones = (MchBone *)&D_800D9630[arg0]->subBuffer;
    FastMemCpy((u8 *)D_800D9630[arg0]->bones, (u8 *)D_800D9630[arg0]->unk00.def->unk00, D_800D9630[arg0]->unk00.def->unk2C * sizeof(MchBone) + sizeof(MchBone));
    D_800D9630[arg0]->unk64 = D_800D6620[arg1]->unk52;
    D_800D9630[arg0]->unk68 = D_800D6620[arg1]->unk54;
    D_800D9630[arg0]->unk08 = D_800D6620[arg1]->unk58;
    D_800D9630[arg0]->bones->rotation.vx = 0;
    D_800D9630[arg0]->bones->rotation.vy = 0x400;
    D_800D9630[arg0]->bones->rotation.vz = 0;
    D_800D9630[arg0]->bones->boneVector.vx = 0;
    D_800D9630[arg0]->bones->boneVector.vy = 0;
    D_800D9630[arg0]->bones->boneVector.vz = 0;
    cursor = (u16 *)&D_800D9630[arg0]->bones[D_800D9630[arg0]->unk00.def->unk2C + 2];
    D_800D9630[arg0]->unk80 = cursor;
    for (j = 0; j < D_800D6620[arg1]->unk30 + D_800D6620[arg1]->unk34 + D_800D6620[arg1]->unk38 + D_800D6620[arg1]->unk3C; j++) {
        D_800D9630[arg0]->unk80[0] = 0;
        D_800D9630[arg0]->unk80[j] = 0;
    }
    cursor += D_800D6620[arg1]->unk30 + D_800D6620[arg1]->unk34 + D_800D6620[arg1]->unk38 + D_800D6620[arg1]->unk3C;
    misalign = (u32)cursor & 3;
    if (misalign != 0) {
        cursor = (u16 *)((u8 *)cursor + (4 - misalign));
    }
    D_800D9630[arg0]->unk0C = D_800D9630[arg0]->unk68[0] << 4;
    D_800D9630[arg0]->unk0E = D_800D9630[arg0]->unk68[1];
    D_800D9630[arg0]->unk6C = (CVECTOR *)D_800D9630[arg0]->unk08;
    D_800D9630[arg0]->unk94 = D_800D9630[arg0]->unk08;
    ft4 = (POLY_FT4 *)cursor;
    D_800D9630[arg0]->unk54 = (POLY_G3 *)ft4;
    for (i = 0; i < 2; i++) {
        D_800D9630[arg0]->unk58[i] = (POLY_G3 *)ft4;
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
        for (j = 0; j < D_800D6620[arg1]->unk38; j++) {
            func_8004D664(ft4);
            ft4->r0 = D_800D6620[arg1]->unk0C[j].unk10.r;
            ft4->g0 = D_800D6620[arg1]->unk0C[j].unk10.g;
            ft4->b0 = D_800D6620[arg1]->unk0C[j].unk10.b;
            ft4->pad1 = D_800D6620[arg1]->unk0C[j].unk10.cd;
            ft4->tpage = D_800D6620[arg1]->unk0C[j].unkE;
            ft4->clut = D_800D6620[arg1]->unk0C[j].unkC;
            *(u16 *)&ft4->u0 = D_800D6620[arg1]->unk0C[j].unk6;
            *(u16 *)&ft4->u1 = D_800D6620[arg1]->unk0C[j].unk8;
            *(u16 *)&ft4->u2 = D_800D6620[arg1]->unk0C[j].unkA;
            if (D_800D6620[arg1]->unk0C[j].unk14 & MODEL_UNK0C_FLAG_4) {
                SetSemiTrans(ft4, 1);
            }
            ft4 = (POLY_FT4 *)((POLY_FT3 *)ft4 + 1); // matching hack
        }
        D_800D6610[i] = ft4;
        for (j = 0; j < D_800D6620[arg1]->unk3C; j++) {
            func_8004D6A4(ft4);
            ft4->r0 = D_800D6620[arg1]->unk10[j].unk14.r;
            ft4->g0 = D_800D6620[arg1]->unk10[j].unk14.g;
            ft4->b0 = D_800D6620[arg1]->unk10[j].unk14.b;
            ft4->pad1 = D_800D6620[arg1]->unk10[j].unk14.cd;
            ft4->tpage = D_800D6620[arg1]->unk10[j].unk12;
            ft4->clut = D_800D6620[arg1]->unk10[j].unk10;
            *(u16 *)&ft4->u0 = D_800D6620[arg1]->unk10[j].unk8;
            *(u16 *)&ft4->u1 = D_800D6620[arg1]->unk10[j].unkA;
            *(u16 *)&ft4->u2 = D_800D6620[arg1]->unk10[j].unkC;
            *(u16 *)&ft4->u3 = D_800D6620[arg1]->unk10[j].unkE;
            if (D_800D6620[arg1]->unk10[j].unk18 & MODEL_UNK10_FLAG_4) {
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
    D_800D9630[arg0]->unk52 = 0;
    return ft4;
}

/**
 * @brief Claim a render-slot pool entry and initialize its state.
 *
 * Acquires @c D_800D9630[idx] if currently @c NULL by installing @p slot
 * and writing @p firstWord to the slot's leading @c s32. The slot's state
 * fields (@c unk10/12/14/18/1A/1C/50/52) are zeroed except @c unk12 which
 * gets the default @c 0x190; the three @c field20/24/28 scale factors are
 * set to @c 0x1000 (unit scale); @c unk60 (active flag) is cleared.
 *
 * @return @p slot @c + @c 0x98 on successful claim, or @c NULL if the
 *         render-slot was already occupied.
 */
s32 *func_800A8CDC(s32 idx, s32 firstWord, ModelInstance *slot) {
    if (D_800D9630[idx] != NULL) {
        return NULL;
    }
    D_800D9630[idx] = slot;
    slot->unk00.modelId = firstWord;
    D_800D9630[idx]->unk60 = 0;
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
    D_800D9630[idx]->unk52 = 0;
    D_800D9630[idx]->unk60 = 0;
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
        offset = *D_800D9630[spatialIdx]->unk08;
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
        for (j = 0; j < D_800D9630[spatialIdx]->unk7E; j++) {
            first = D_800D9630[spatialIdx]->unk68[k - 2];
            second = D_800D9630[spatialIdx]->unk68[k - 1];
            sum += first;
            productSum += first * second;
            k += 2;
        }
        col = D_800D9630[spatialIdx]->unk6C[productSum + D_800D9630[spatialIdx]->headBone + sum + 2];
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

    if (D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk8.pad & (MCH_DRAW_GROUP_FLAG_1 | MCH_DRAW_GROUP_FLAG_2)) {
        return -1;
    }
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk8.pad = 0;
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.pad = arg4;
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk4 = 1;
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk8.vx = D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.vx;
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk8.vy = D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.vy;
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk8.vz = D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.vz;
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.vx += arg3[0];
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.vy += arg3[1];
    D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk18.vz += arg3[2];
    switch (arg1) {
    case 0x30:
        D_800D9630[arg0]->unk00.def->drawGroups[arg2].unk8.pad |= MCH_DRAW_GROUP_FLAG_1;
        func_800A97E4(arg0, 0x25, 0, 0);
        break;
    case 0x32:
        sumA = 0;
        sumB = 0;
        for (j = 0; j < arg2; j++) {
            sumA += D_800D9630[arg0]->unk00.def->drawGroups[j].triangleCount;
            sumB += D_800D9630[arg0]->unk00.def->drawGroups[j].quadCount;
        }
        for (k = 0; k < 2; k++) {
            ft3 = (POLY_FT3 *)((POLY_G4 *)(D_800D9630[arg0]->unk58[k] + D_800D9630[arg0]->unk00.def->unk30) + D_800D9630[arg0]->unk00.def->unk34);
            for (j = 0; j < D_800D9630[arg0]->unk00.def->drawGroups[arg2].triangleCount; j++) {
                ft3[sumA + j].pad1 = arg4;
            }
            ft4 = (POLY_FT4 *)((POLY_FT3 *)((POLY_G4 *)(D_800D9630[arg0]->unk58[k] + D_800D9630[arg0]->unk00.def->unk30) + D_800D9630[arg0]->unk00.def->unk34) + D_800D9630[arg0]->unk00.def->unk38);
            for (j = 0; j < D_800D9630[arg0]->unk00.def->drawGroups[arg2].quadCount; j++) {
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

    if (D_800D9630[spatialIdx]->unk00.modelId == NO_MODEL) {
        return -1;
    }
    switch (cmd) {
    case 0x27:
        if (!(D_800D9630[spatialIdx]->unk00.def->unk5F & FIELD_OBJECT_FLAG_1)) {
            return 1;
        }
        if ((s16)D_800D9630[spatialIdx]->unk00.def->anim->frames[0].y >= 0) {
            D_800D9630[spatialIdx]->unk00.def->anim->frames[0].y = arg3;
            if (arg2 != 0) {
                D_800D9630[spatialIdx]->frameTimer = arg2;
                D_800D9630[spatialIdx]->timerReload = arg2;
            } else {
                D_800D9630[spatialIdx]->frameTimer = rand() % 8 + 1;
                D_800D9630[spatialIdx]->timerReload = rand() % 8 + 1;
            }
        } else {
            D_800D9630[spatialIdx]->frameTimer = D_800D9630[spatialIdx]->unk00.def->anim->frames[0].y;
            D_800D9630[spatialIdx]->timerReload = D_800D9630[spatialIdx]->unk00.def->anim->frames[0].y;
        }
        D_800D9630[spatialIdx]->animFlags &= 0xF;
        D_800D9630[spatialIdx]->animFlags |= ENTITY_ANIM_FLAG_10;
        D_800D9630[spatialIdx]->frameIdx = D_800D9630[spatialIdx]->unk00.def->anim->frameCount;
        break;
    case 0x2F:
        if (!(D_800D9630[spatialIdx]->unk00.def->unk5F & FIELD_OBJECT_FLAG_1)) {
            return 1;
        }
        D_800D9630[spatialIdx]->animFlags &= 0xF;
        D_800D9630[spatialIdx]->animFlags |= ENTITY_ANIM_FLAG_20;
        rect.w = D_800D9630[spatialIdx]->unk00.def->anim->width;
        rect.h = D_800D9630[spatialIdx]->unk00.def->anim->height;
        rect.x = D_800D9630[spatialIdx]->unk00.def->anim->frames[1].x;
        rect.y = D_800D9630[spatialIdx]->unk00.def->anim->frames[1].y;
        MoveImage(&rect, D_800D9630[spatialIdx]->unk00.def->anim->destX, D_800D9630[spatialIdx]->unk00.def->anim->frames[0].x);
        break;
    case 0x2E:
        if (!(D_800D9630[spatialIdx]->unk00.def->unk5F & FIELD_OBJECT_FLAG_1)) {
            return 1;
        }
        D_800D9630[spatialIdx]->animFlags &= 0xF;
        D_800D9630[spatialIdx]->animFlags |= ENTITY_ANIM_FLAG_40;
        rect.w = D_800D9630[spatialIdx]->unk00.def->anim->width;
        rect.h = D_800D9630[spatialIdx]->unk00.def->anim->height;
        // matching hack
        rect.x = D_800D9630[spatialIdx]->unk00.def->anim->frameCount[D_800D9630[spatialIdx]->unk00.def->anim->frames].x;
        rect.y = D_800D9630[spatialIdx]->unk00.def->anim->frameCount[D_800D9630[spatialIdx]->unk00.def->anim->frames].y;
        MoveImage(&rect, D_800D9630[spatialIdx]->unk00.def->anim->destX, D_800D9630[spatialIdx]->unk00.def->anim->frames[0].x);
        break;
    case 0x25:
        D_800D9630[spatialIdx]->unk78 = (SVECTOR *)-1;
        break;
    case 0x23:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_40;
        break;
    case 0x24:
        D_800D9630[spatialIdx]->unk60 = 0;
        break;
    case 0x21:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_20;
        for (j = 0; j < 0x1003; j++) {
            D_800D6720[j] = 0xFFFF;
        }
        break;
    case 0x22:
        D_800D9630[spatialIdx]->unk60 &= ~ENTITY_RENDER_FLAG_20;
        break;
    case 0x13:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_2;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation = *(SVECTOR *)arg2;
        break;
    case 0x1C:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_2;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation = *(SVECTOR *)arg2;
        break;
    case 0x14:
    case 0x1D:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_4;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation = *(SVECTOR *)arg2;
        break;
    case 0x19:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_2;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation.vx += ((SVECTOR *)arg2)->vx;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation.vy += ((SVECTOR *)arg2)->vy;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->headBone + 1].rotation.vz += ((SVECTOR *)arg2)->vz;
        break;
    case 0x1A:
        D_800D9630[spatialIdx]->unk60 |= ENTITY_RENDER_FLAG_4;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation.vx = ((SVECTOR *)arg2)->vx;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation.vy = ((SVECTOR *)arg2)->vy;
        D_800D9630[spatialIdx]->bones[D_800D9630[spatialIdx]->bodyBone + 1].rotation.vz = ((SVECTOR *)arg2)->vz;
        break;
    case 0x15:
        D_800D9630[spatialIdx]->unk60 &= ~ENTITY_RENDER_FLAG_2;
        break;
    case 0x16:
        D_800D9630[spatialIdx]->unk60 &= ~ENTITY_RENDER_FLAG_4;
        break;
    case 0xF:
        for (i = 0; i < 2; i++) {
            g3 = D_800D9630[spatialIdx]->unk58[i];
            D_800D5EE0[i] = g3;
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk30; j++) {
                SetSemiTrans(g3, arg2);
                g3++;
            }
            g4 = (POLY_G4 *)g3;
            D_800D5EE8[i] = g4;
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk34; j++) {
                SetSemiTrans(g4, arg2);
                g4++;
            }
            ft3 = (POLY_FT3 *)g4;
            D_800D6600[i] = ft3;
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk38; j++) {
                SetSemiTrans(ft3, arg2);
                ft3++;
            }
            ft4 = (POLY_FT4 *)ft3;
            D_800D6610[i] = ft4;
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk3C; j++) {
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
            g3 = D_800D9630[spatialIdx]->unk58[i];
            D_800D5EE0[i] = g3;
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk30; j++) {
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
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk34; j++) {
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
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk38; j++) {
                ft3->r0 = color.r;
                ft3->g0 = color.g;
                ft3->b0 = color.b;
                ft3++;
            }
            ft4 = (POLY_FT4 *)ft3;
            D_800D6610[i] = ft4;
            for (j = 0; j < D_800D9630[spatialIdx]->unk00.def->unk3C; j++) {
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

    if (D_800D9630[spatialIdx]->unk00.modelId != NO_MODEL && cmd == 13) {
        if (arg >= D_800D9630[spatialIdx]->unk64) {
            func_800ACBCC(spatialIdx, D_800D9630[spatialIdx]->unk64, arg, 0);
            SystemError(0x53, 0x17);
        }
        j = 2;
        productSum = 0;
        sum = 0;
        D_800D9630[spatialIdx]->unk7E = arg;
        for (i = 0; i < arg; i++) {
            first = D_800D9630[spatialIdx]->unk68[j - 2];
            second = D_800D9630[spatialIdx]->unk68[j - 1];
            sum += first;
            productSum += first * second;
            j += 2;
        }
        D_800D9630[spatialIdx]->unk0C = D_800D9630[spatialIdx]->unk68[j - 2] << 4;
        D_800D9630[spatialIdx]->unk0E = D_800D9630[spatialIdx]->unk68[j - 1];
        D_800D9630[spatialIdx]->unk08 = (SVECTOR *)(D_800D9630[spatialIdx]->unk6C + (productSum + sum));
        D_800D9630[spatialIdx]->unk50 = 0;
        D_800D9630[spatialIdx]->unk50 = 0;
        D_800D9630[spatialIdx]->unk78 = (SVECTOR *)-1;
        D_800D9630[spatialIdx]->unk7C = 0xFFFF;
    }
}

/**
 * @brief Per-entity animation tick — advances the frame and stages the next
 *        sprite rectangle for upload.
 *
 * For render slot @p idx this:
 *  - returns 1 immediately if the entity's definition is not animated
 *    (@c def->unk5F bit 0 clear);
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

    period = D_800D9630[idx]->unk00.def->anim->frames[0].y;
    if (!(D_800D9630[idx]->unk00.def->unk5F & FIELD_OBJECT_FLAG_1)) {
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
        D_800D9630[idx]->frameIdx = D_800D9630[idx]->unk00.def->anim->frameCount;
    }

    D_800D5ED8.w = D_800D9630[idx]->unk00.def->anim->width;
    D_800D5ED8.h = D_800D9630[idx]->unk00.def->anim->height;
    /* frameIdx[frames] is frames[frameIdx]; the swapped index keeps the array
       index in the first addu operand, matching the original codegen. */
    D_800D5ED8.x = D_800D9630[idx]->frameIdx[D_800D9630[idx]->unk00.def->anim->frames].x;
    D_800D5ED8.y = D_800D9630[idx]->frameIdx[D_800D9630[idx]->unk00.def->anim->frames].y;

    MoveImage(&D_800D5ED8,
              D_800D9630[idx]->unk00.def->anim->destX,
              D_800D9630[idx]->unk00.def->anim->frames[0].x);
    return 0;
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800AA8A0);
