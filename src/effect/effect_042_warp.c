/**
 * @file effect_042_warp.c
 * @brief Shoot: the warping mesh and its per-frame handlers.
 */
#include "common.h"
#include "effect.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/inline_c.h"
#include "effect/effect_042_warp.h"
#include "effect/lib/common.h"

/** @brief A posed vertex and the projected coordinates reused by its faces. */
typedef struct {
    /* 0x00 */ SVECTOR pos;
    /* 0x08 */ u32 screen;
    /* 0x0C */ s32 depth;
    /* 0x10 */ u32 unk010;
} ShootWarpVertex; /* 0x14 */

typedef struct {
    /* 0x00 */ u32 unk000;
    /* 0x04 */ ShootWarpVertex *vertices;
} ShootWarpPart;

/** @brief Matrices, deformation parameters and packet cursor for Shoot's mesh. */
typedef struct {
    /* 0x00 */ MATRIX rotation;
    /* 0x20 */ u8 pad020[0x40 - 0x20];
    /* 0x40 */ MATRIX view;
    /* 0x60 */ u32 unk060;
    /* 0x64 */ ShootWarpPart *part;
    /* 0x68 */ BattleEffectSlot *slot;
    /* 0x6C */ POLY_FT3 **prims;
    /* 0x70 */ u32 unk070;
    /* 0x74 */ SVECTOR origin;
    /* 0x7C */ u8 pad07C[0x9C - 0x7C];
    /* 0x9C */ SVECTOR angles;
    /* 0xA4 */ u8 pad0A4[0xC8 - 0xA4];
    /* 0xC8 */ u16 blend;
    /* 0xCA */ s16 radiusX;
    /* 0xCC */ s16 radiusY;
    /* 0xCE */ s16 radiusZ;
} ShootWarpRender;

/** @brief Battle scratchpad record, with the offsets used by the mesh stream. */
typedef struct {
    /* 0x00 */ MATRIX rotation;
    /* 0x20 */ u32 unk020;
    /* 0x24 */ u32 screen[4];
    /* 0x34 */ SVECTOR projected;
    /* 0x3C */ u8 pad03C[0x4C - 0x3C];
    /* 0x4C */ SVECTOR delta;
    /* 0x54 */ SVECTOR original;
    /* 0x5C */ SVECTOR origin;
    /* 0x64 */ u8 pad064[0x78 - 0x64];
    /* 0x78 */ s32 otz;
    /* 0x7C */ s32 nclip;
    /* 0x80 */ s32 idx0;
    /* 0x84 */ s32 idx1;
    /* 0x88 */ s32 idx2;
    /* 0x8C */ s32 idx3;
    /* 0x90 */ u32 triColour;
    /* 0x94 */ u32 quadColour;
    /* 0x98 */ u32 visible;
    /* 0x9C */ u16 keepWeight;
    /* 0x9E */ u16 warpWeight;
    /* 0xA0 */ s16 radiusX;
    /* 0xA2 */ s16 radiusY;
    /* 0xA4 */ s16 radiusZ;
    /* 0xA6 */ s16 unk0A6;
} ShootWarpScratch; /* 0xA8 */

static void func_801A990C(EffectMesh *mesh, u32 *ot, s32 otShift,
                        ShootWarpRender *render);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801A97F0);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801A98AC);

static void func_801A990C(EffectMesh *mesh, u32 *ot, s32 otShift,
                        ShootWarpRender *render) {
    POLY_FT3 *ft3;
    ShootWarpVertex *vbuf;
    EffectJoint *joints;
    u32 *parts;
    s32 count;
    BattleEffectSlot *slot;
    ShootWarpScratch *s;
    s32 part;
    POLY_FT4 *ft4;

    parts = mesh->parts;
    ft3 = *render->prims;
    joints = mesh->skeleton->joints;
    vbuf = render->part->vertices;
    count = *parts++;
    slot = render->slot;
    s = func_800B3698(sizeof(ShootWarpScratch));

    s->warpWeight = render->blend;
    s->keepWeight = ONE - s->warpWeight;
    s->origin = render->origin;
    s->radiusX = render->radiusX;
    s->radiusY = render->radiusY;
    s->radiusZ = render->radiusZ;
    s->visible = slot->unk07C;
    s->triColour = slot->unk028 & EFFECT_PRIM_RGB;
    s->quadColour = s->triColour | EFFECT_PRIM_CODE(0x2C);
    s->triColour |= EFFECT_PRIM_CODE(0x24);
    s->rotation = render->rotation;
    if (render->angles.vx != 0) {
        effectMatrixRotX(&s->rotation, render->angles.vx);
    }
    if (render->angles.vz != 0) {
        effectMatrixRotZ(&s->rotation, render->angles.vz);
    }
    if (render->angles.vy != 0) {
        effectMatrixRotY(&s->rotation, render->angles.vy);
    }
    gte_SetColorMatrix(&s->rotation);
    gte_SetRotMatrix(&render->view);
    gte_SetTransMatrix(&render->view);
    for (part = 0; part < count; part++) {
        ShootWarpVertex *vb = vbuf;
        s16 *stream = (s16 *)((u8 *)mesh->parts + *parts++);
        s16 *start = stream;
        s32 groups;
        s32 n;
        s32 tris;
        s32 quads;
        s32 g;
        s32 v;
        MATRIX *joint;
        EffectMeshTri *tri;
        EffectMeshQuad *quad;
        POLY_FT3 *ft;
        u32 *screen;

        if (!((s->visible >> part) & 1)) {
            continue;
        }
        groups = *stream++;
        for (g = 0; g < groups; g++) {
            joint = &joints[*stream++].mtx;
            gte_SetLightMatrix(joint);
            gte_ldbkdir(joint->t[0], joint->t[1], joint->t[2]);
            n = *stream++;
            for (v = 0; v < n; v++) {
                s->delta.vx = *stream++;
                s->delta.vy = *stream++;
                s->delta.vz = *stream++;
                gte_ldv0(&s->delta);
                gte_mvmva(1, 1, 0, 1, 0);
                gte_stsv(&s->delta);
                s->delta.vx -= s->origin.vx;
                s->delta.vy -= s->origin.vy;
                s->delta.vz -= s->origin.vz;
                s->original = s->delta;
                VectorNormalSS(&s->delta, &s->delta);
                s->delta.vx = s->delta.vx * s->radiusX / ONE;
                s->delta.vy = s->delta.vy * s->radiusY / ONE;
                s->delta.vz = s->delta.vz * s->radiusZ / ONE;
                gte_lddp(s->keepWeight);
                gte_ldsv(&s->original);
                gte_gpf1();
                gte_lddp(s->warpWeight);
                gte_ldsv(&s->delta);
                gte_gpl1();
                gte_stsv(&s->delta);
                gte_ldv0(&s->delta);
                gte_mvmva(1, 2, 0, 3, 0);
                gte_stsv(&s->delta);
                s->delta.vx += s->origin.vx;
                s->delta.vy += s->origin.vy;
                s->delta.vz += s->origin.vz;
                vb->pos = s->delta;
                vb++;
            }
        }
        /* Project the posed vertices, preserving the mesh stream traversal. */
        stream = start;
        vb = vbuf;
        groups = *stream++;
        for (g = 0; g < groups; g++) {
            stream++;
            n = *stream++;
            for (v = 0; v < n; v++) {
                s->projected = vb->pos;
                stream += 3;
                gte_ldv0(&s->projected);
                gte_rtps();
                gte_stsxy(&vb->screen);
                gte_stsz(&vb->depth);
                vb++;
            }
        }
        /* The face header begins on a word boundary after the vertex groups. */
        stream = (s16 *)(((u32)stream + 3) & ~3);
        tris = *stream++;
        quads = *stream++;
        stream += 4;
        tri = (EffectMeshTri *)stream;
        vb = vbuf;
        ft = ft3;
        for (g = 0; g < tris; g++) {
            s->idx0 = tri->idx0 & EFFECT_MESH_INDEX_MASK;
            s->idx1 = tri->idx1 & EFFECT_MESH_INDEX_MASK;
            s->idx2 = tri->idx2 & EFFECT_MESH_INDEX_MASK;
            screen = s->screen;
            s->screen[0] = vb[s->idx0].screen;
            screen[1] = vb[s->idx1].screen;
            screen[2] = vb[s->idx2].screen;
            gte_ldsxy3(screen[0], screen[1], screen[2]);
            gte_nclip();
            gte_stopz(&s->nclip);
            if (s->nclip > 0) {
                ft->tag = EFFECT_PRIM_TAG(POLY_FT3);
                *(u32 *)&ft->x0 = s->screen[0];
                *(u32 *)&ft->x1 = screen[1];
                *(u32 *)&ft->x2 = screen[2];
                *(u32 *)&ft->u0 = tri->uv0;
                *(u32 *)&ft->u1 = *(u32 *)&tri->uv1;
                *(u16 *)&ft->u2 = tri->uv2;
                *(u32 *)&ft->r0 = s->triColour;
                if (tri->tpage & EFFECT_MESH_TPAGE_ABE) {
                    setSemiTrans(ft, 1);
                }
                s->otz = (vb[s->idx0].depth + vb[s->idx1].depth +
                          vb[s->idx2].depth) / 3 >> otShift;
                addPrim(&ot[s->otz], ft);
                ft++;
            }
            tri++;
        }
        quad = (EffectMeshQuad *)tri;
        ft4 = (POLY_FT4 *)ft;
        for (g = 0; g < quads; g++) {
            s->idx0 = quad->idx0 & EFFECT_MESH_INDEX_MASK;
            s->idx1 = quad->idx1 & EFFECT_MESH_INDEX_MASK;
            s->idx2 = quad->idx2 & EFFECT_MESH_INDEX_MASK;
            screen = s->screen;
            s->screen[0] = vb[s->idx0].screen;
            screen[1] = vb[s->idx1].screen;
            screen[2] = vb[s->idx2].screen;
            gte_ldsxy3(screen[0], screen[1], screen[2]);
            gte_nclip();
            gte_stopz(&s->nclip);
            if (s->nclip > 0) {
                s->idx3 = quad->idx3 & EFFECT_MESH_INDEX_MASK;
                screen[3] = vb[s->idx3].screen;
                ft4->tag = EFFECT_PRIM_TAG(POLY_FT4);
                *(u32 *)&ft4->x0 = s->screen[0];
                *(u32 *)&ft4->x1 = screen[1];
                *(u32 *)&ft4->x2 = screen[2];
                *(u32 *)&ft4->x3 = screen[3];
                *(u32 *)&ft4->u0 = quad->uv0;
                *(u32 *)&ft4->u1 = *(u32 *)&quad->uv1;
                *(u16 *)&ft4->u2 = quad->uv2;
                *(u16 *)&ft4->u3 = quad->uv3;
                *(u32 *)&ft4->r0 = s->quadColour;
                if (quad->tpage & EFFECT_MESH_TPAGE_ABE) {
                    setSemiTrans(ft4, 1);
                }
                s->otz = (vb[s->idx0].depth + vb[s->idx1].depth +
                          vb[s->idx2].depth + vb[s->idx3].depth) >> (otShift + 2);
                addPrim(&ot[s->otz], ft4);
                ft4++;
            }
            quad++;
        }
        ft3 = (POLY_FT3 *)ft4;
    }
    *render->prims = ft3;
    func_800B36B8(sizeof(ShootWarpScratch));
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AA420);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AA68C);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AA6B0);

void func_801AA800(void) {
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AA808);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AA908);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AAAD4);

void func_801AAAF0(void) {
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AAAF8);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AAB98);

void func_801AAD10(void) {
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AAD18);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AAE10);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB0B0);

void func_801AB0CC(void) {
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB0D4);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB174);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB1D0);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB3BC);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB528);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AB788);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABAA8);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABC18);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABC8C);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABD18);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABD88);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABE14);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801ABFB4);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC154);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC190);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC264);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC33C);

void func_801AC37C(void) {
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC384);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC4C0);

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC520);

void func_801AC53C(void) {
}

INCLUDE_ASM("asm/ovl/effect_042/nonmatchings/effect_042_warp", func_801AC544);
