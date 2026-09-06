/**
 * @file effect_001.c
 * @brief Cure
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "psxsdk/libgpu.h"
#include "effect/effect_001.h"
#include "btl_entity.h"

static void func_801A0644(MATRIX *m, s32 angle);
static void func_801A0960(MATRIX *m, SVECTOR *dir, SVECTOR *up);
static EffectEntity *func_801A0B00(void *pool, void *task, s32 stride,
                                   EffectEntity *owner);
static s32 func_801A2E48(EffectSpark *spark);
static void func_801A351C(EffectEntity *entity);
static void func_801A3640(EffectEntity *entity);
static s32 func_801A3924(EffectEntity *entity);
static s32 func_801A3B6C(EffectEntity *entity);
static s32 func_801A3E64(EffectEntity *entity);
static s32 func_801A41C8(EffectEntity *entity);
static s32 func_801A4434(EffectEntity *entity);

/** @brief Stride between the prim and frame banks carved out of the TIM. */
#define EFFECT_BANK_SIZE 0x10000

/** @brief Scratch a model's bounding box is accumulated in. */
typedef struct {
    /* 0x00 */ SVECTOR v;
    /* 0x08 */ VECTOR pos;
    /* 0x18 */ SVECTOR min;
    /* 0x20 */ SVECTOR max;
    /* 0x28 */ s32 flag;
} EffectBoundsScratch; /* 0x2C */

/** @brief Scratch the billboard basis is built in. */
typedef struct {
    /* 0x00 */ VECTOR delta;
    /* 0x10 */ SVECTOR up;
    /* 0x18 */ SVECTOR dir;
    /* 0x20 */ MATRIX rot;
    /* 0x40 */ MATRIX out;
    /* 0x60 */ s32 distSq;
    /* 0x64 */ s32 dist;
    /* 0x68 */ SVECTOR pos;
} EffectAimScratch; /* 0x70 */

/** @brief Scratch a rotation is built in before being folded into a matrix. */
typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ s16 sin;
    /* 0x22 */ s16 cos;
} EffectRotScratch; /* 0x24 */

/**
 * @brief Start an effect script and hand back its task pool.
 *
 * Builds the four task pools, allocates the root entity, seeds it from the
 * animation set, and publishes the three frame banks the renderer reads from.
 *
 * @param animSet Animation set the effect plays.
 * @return The pool the root entity lives in.
 */
void *func_801A0000(EffectAnimSet *animSet) {
    u8 *bank0;
    EffectEntity *entity;
    u8 *bank1;
    u8 *frames;

    D_801C58E0 = 0;
    D_801C58E4 = 0;
    func_800B2A00(&D_801C59E0, &D_801C5910, 0x64, 2);
    entity = func_801A0B00(&D_801C59E0, func_801A4434, 0x64, NULL);
    entity->animSet = animSet;
    entity->unk02C = animSet->anims[entity->unk02A].unk000;
    entity->unk02D =
        entity->animSet->anims[entity->unk02A].parts[entity->unk02B].unk000;
    entity->pc = 0;
    entity->unk05A = animSet->anims->unk010;
    entity->unk058 = animSet->anims->unk011;
    entity->unk02F = entity->unk05A - 1;
    if (entity->unk02F < entity->unk058) {
        entity->unk02F = entity->unk058;
    }
    if (!(animSet->flags & EFFECT_ANIMSET_FLAG_LOADED)) {
        func_800C3BE0(&D_801A475C);
        func_800BB084(&D_801A4F0C);
    }
    D_801C58F0 = &D_801A4F0C;
    bank0 = &D_801A4F0C;
    D_801C58FC = bank0;
    bank1 = bank0 + EFFECT_BANK_SIZE;
    D_801C5904 = bank0 + EFFECT_BANK_SIZE;
    D_801C58F0 = bank0 + EFFECT_BANK_SIZE;
    frames = bank1;
    D_801C5900 = frames;
    frames += EFFECT_BANK_SIZE;
    D_801C58F0 = frames;
    D_801C5908 = frames;
    func_800B2A00(&D_801C5B50, &D_801C59F0, 0x58, 4);
    func_800B2A00(&D_801CD9F0, &D_801C5B60, 0x6C, 0x12C);
    func_800B2A00(&D_801D5CF0, &D_801D3900, 0x5C, 0x64);
    return &D_801C59E0;
}

/**
 * @brief Cache the sixteen hex digit glyphs and reset the debug text cursor.
 *
 * @note The lookup runs once per glyph -- the target holds 16 separate calls,
 *       so a loop or a cached pointer does not match.
 */
static void func_801A01F0(void) {
    D_801D5D00[0] = getMenuString(11)[1];
    D_801D5D00[1] = getMenuString(11)[2];
    D_801D5D00[2] = getMenuString(11)[3];
    D_801D5D00[3] = getMenuString(11)[4];
    D_801D5D00[4] = getMenuString(11)[5];
    D_801D5D00[5] = getMenuString(11)[6];
    D_801D5D00[6] = getMenuString(11)[7];
    D_801D5D00[7] = getMenuString(11)[8];
    D_801D5D00[8] = getMenuString(11)[9];
    D_801D5D00[9] = getMenuString(11)[10];
    D_801D5D00[10] = getMenuString(11)[11];
    D_801D5D00[11] = getMenuString(11)[12];
    D_801D5D00[12] = getMenuString(11)[13];
    D_801D5D00[13] = getMenuString(11)[14];
    D_801D5D00[14] = getMenuString(11)[15];
    D_801D5D00[15] = getMenuString(11)[16];
    D_801D5D00[16] = 0;
    D_801D5D14 = 10;
    D_801D5D18 = 20;
    D_801D5D1C = 3;
}

/** @brief Draw @p value as eight hex glyphs at the current debug text cursor. */
static void func_801A0340(u32 value) {
    u8 *font = D_800FA5E8->font;
    u8 text[9];
    s32 i;

    for (i = 7; i >= 0; i--) {
        text[i] = D_801D5D00[value & 0xF];
        value >>= 4;
    }
    text[8] = 0;
    D_801C58F4 = func_8002C56C(font, D_801C58F4, D_801D5D14, D_801D5D18,
                               text, D_801D5D1C);
    D_801D5D14 += 0x48;
}

/** @brief Opcode handler: arm the frame delay and advance the running total. */
static void func_801A03EC(void) {
    D_801D5D14 = 10;
    D_801D5D18 += 10;
}

/** @brief Opcode handler: no-op. */
static void func_801A040C(EffectEntity *entity) {
}

/**
 * @brief Emit a 128x128 textured quad at the head of the prim buffer.
 *
 * @param x     Left edge in screen space.
 * @param y     Top edge in screen space.
 * @param tx    Texture page X.
 * @param ty    Texture page Y.
 * @param clutX Palette X.
 * @param clutY Palette Y.
 * @param abr   Semi-transparency rate for the page.
 */
static void func_801A0414(s16 x, s16 y, s16 tx, s16 ty, u32 clutX, s32 clutY,
                   s32 abr) {
    POLY_FT4 *poly = D_801C58F4;

    setPolyFT4(poly);
    setSemiTrans(poly, 1);
    setShadeTex(poly, 1);
    poly->tpage = getTPage(1, abr, tx, ty);
    poly->clut = getClut(clutX, clutY);
    poly->u0 = poly->u2 = 0;
    poly->u1 = poly->u3 = 0xFF;
    poly->v0 = poly->v1 = 0;
    poly->v2 = poly->v3 = 0xFF;
    poly->x0 = poly->x2 = x;
    poly->x1 = poly->x3 = x + 0x7F;
    poly->y0 = poly->y1 = y;
    poly->y2 = poly->y3 = y + 0x7F;
    AddPrim(D_800FA5E8, poly);
    poly++;
    D_801C58F4 = poly;
}

/**
 * @brief Emit a 256x256 textured quad at the head of the prim buffer.
 *
 * @param x     Left edge in screen space.
 * @param y     Top edge in screen space.
 * @param tx    Texture page X.
 * @param ty    Texture page Y.
 * @param clutX Palette X.
 * @param clutY Palette Y.
 * @param abr   Semi-transparency rate for the page.
 */
static void func_801A0510(s16 x, s16 y, s16 tx, s16 ty, u32 clutX, s32 clutY,
                   s32 abr) {
    POLY_FT4 *poly = D_801C58F4;

    setPolyFT4(poly);
    setSemiTrans(poly, 1);
    setShadeTex(poly, 1);
    poly->tpage = getTPage(1, abr, tx, ty);
    poly->clut = getClut(clutX, clutY);
    poly->u0 = poly->u2 = 0;
    poly->u1 = poly->u3 = 0xFF;
    poly->v0 = poly->v1 = 0;
    poly->v2 = poly->v3 = 0xFF;
    poly->x0 = poly->x2 = x;
    poly->x1 = poly->x3 = x + 0xFF;
    poly->y0 = poly->y1 = y;
    poly->y2 = poly->y3 = y + 0xFF;
    AddPrim(D_800FA5E8, poly);
    poly++;
    D_801C58F4 = poly;
}

/** @brief Load the identity matrix. */
static void func_801A060C(MATRIX *m) {
    m->m[0][0] = ONE;
    m->m[1][0] = 0;
    m->m[2][0] = 0;
    m->m[0][1] = 0;
    m->m[1][1] = ONE;
    m->m[2][1] = 0;
    m->m[0][2] = 0;
    m->m[1][2] = 0;
    m->m[2][2] = ONE;
    m->t[0] = 0;
    m->t[1] = 0;
    m->t[2] = 0;
}

/** @brief Post-multiply @p m by a rotation of @p angle about X. */
static void func_801A0644(MATRIX *m, s32 angle) {
    EffectRotScratch *rot = func_800B3698(sizeof(EffectRotScratch));

    rot->sin = rsin(angle);
    rot->cos = rsin((angle + 0x400) & 0xFFF);
    rot->m.m[0][0] = ONE;
    rot->m.m[0][1] = 0;
    rot->m.m[0][2] = 0;
    rot->m.m[1][0] = 0;
    rot->m.m[1][1] = rot->cos;
    rot->m.m[1][2] = -rot->sin;
    rot->m.m[2][0] = 0;
    rot->m.m[2][1] = rot->sin;
    rot->m.m[2][2] = rot->cos;
    gte_MulMatrix0(m, &rot->m, m);
    func_800B36B8(sizeof(EffectRotScratch));
}

/**
 * @brief Compose a rotation about the Y axis into @p m.
 *
 * @param m     Matrix rotated in place.
 * @param angle Rotation, in the 0x1000-per-turn units @ref rsin takes.
 */
static void func_801A07D4(MATRIX *m, s32 angle) {
    EffectRotScratch *rot = func_800B3698(sizeof(EffectRotScratch));

    rot->sin = rsin(angle);
    rot->cos = rsin((angle + 0x400) & 0xFFF);
    rot->m.m[0][0] = rot->cos;
    rot->m.m[0][1] = 0;
    rot->m.m[0][2] = rot->sin;
    rot->m.m[1][0] = 0;
    rot->m.m[1][1] = ONE;
    rot->m.m[1][2] = 0;
    rot->m.m[2][0] = -rot->sin;
    rot->m.m[2][1] = 0;
    rot->m.m[2][2] = rot->cos;
    gte_MulMatrix0(m, &rot->m, m);
    func_800B36B8(sizeof(EffectRotScratch));
}

/** @brief Build a matrix in @p m that aims along @p dir with @p up as the roll. */
static void func_801A0960(MATRIX *m, SVECTOR *dir, SVECTOR *up) {
    MATRIX *basis = func_800B3698(sizeof(MATRIX));
    SVECTOR *row1 = (SVECTOR *)basis->m[1];
    SVECTOR *row2 = (SVECTOR *)basis->m[2];

    /* A matrix row is 6 bytes, so the whole-vector store spills one halfword
       into row 2 -- harmless, row 2 is written next. The width is load-bearing:
       it is the unaligned 8-byte copy the target does here, where the
       element-wise stores below are three halfwords. */
    *row1 = *up;
    row2->vx = dir->vx;
    row2->vy = dir->vy;
    row2->vz = dir->vz;
    gte_ldopv1SV(row1);
    gte_ldopv2SV(row2);
    gte_op12();
    gte_stsv(basis->m[0]);
    MatrixNormal_2(basis, basis);
    TransposeMatrix(basis, m);
    func_800B36B8(sizeof(MATRIX));
}

/** @brief Aim @p m along the vector from @p from to @p to, Y up. */
static void func_801A0A6C(MATRIX *m, SVECTOR *from, SVECTOR *to) {
    SVECTOR up;
    SVECTOR delta;

    up.vx = 0;
    up.vy = ONE;
    up.vz = 0;
    delta.vx = to->vx - from->vx;
    delta.vy = to->vy - from->vy;
    delta.vz = to->vz - from->vz;
    func_801A0960(m, &delta, &up);
}

/** @brief Opcode handler: release one frame of the linked script's wait. */
static void func_801A0AD8(EffectEntity *entity) {
    if (entity->unk018 != NULL) {
        entity->unk018->wait--;
    }
}

/**
 * @brief Allocate an effect entity from @p pool and hang it off @p owner.
 *
 * The pool keeps its own 12-byte task header ahead of the entity fields, so
 * only the remainder of @p stride is cleared. A child inherits the owner's
 * model, animation set and position; an entity with no owner becomes its own
 * model.
 *
 * @param pool   Task pool to allocate from.
 * @param task   Per-frame step to install on the new entity.
 * @param stride Total size of the allocation, header included.
 * @param owner  Parent entity, or NULL for a root.
 * @return The new entity, or NULL if the pool is full.
 */
static EffectEntity *func_801A0B00(void *pool, void *task, s32 stride,
                                   EffectEntity *owner) {
    EffectEntity *node = func_800B2A84(pool, task);
    s32 *clear;
    s32 words;
    s32 i;

    if (node == NULL) {
        return NULL;
    }
    clear = (s32 *)&node->animSet;
    stride = stride - 0xC;
    words = stride / 4;
    for (i = 0; i < words; i++) {
        *clear = 0;
        clear++;
    }
    if (owner == NULL) {
        node->unk018 = NULL;
        node->unk014 = NULL;
        /* A root drives its own geometry, so it is its own model. */
        node->unk010 = (EffectModel *)node;
    } else {
        if (owner->unk014 == NULL) {
            node->unk014 = (EffectModel *)node;
        } else {
            node->unk014 = owner->unk014;
        }
        node->unk018 = owner;
        node->unk010 = owner->unk010;
        node->animSet = owner->animSet;
        node->pos = owner->pos;
        node->unk02A = owner->unk02A;
        node->unk02B = owner->unk02B;
        node->unk02E = owner->unk02E;
        node->unk02F = owner->unk02F;
        owner->wait++;
        node->unk02C = node->animSet->anims[node->unk02A].unk000;
        node->unk02D =
            node->animSet->anims[node->unk02A].parts[node->unk02B].unk000;
    }
    return node;
}

/** @brief Cache the battle slot's two anchor points and their midpoint. */
static void func_801A0C8C(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];
    SVECTOR top;
    SVECTOR bottom;

    if (!(slot->flags & BATTLE_SLOT_FLAG_UNK02)) {
        return;
    }
    func_800B3960(slot, 0xF0, 0, &top);
    func_800B3960(slot, 0xF1, 0, &bottom);
    entity->unk040.vx = top.vx;
    entity->unk040.vy = top.vy;
    entity->unk040.vz = top.vz;
    entity->unk038.vx = (top.vx + bottom.vx) / 2;
    entity->unk038.vy = (top.vy + bottom.vy) / 2;
    entity->unk038.vz = (top.vz + bottom.vz) / 2;
    entity->unk030.vx = bottom.vx;
    entity->unk030.vy = slot->unk024;
    entity->unk030.vz = bottom.vz;
}

/** @brief Copy @c unk030 of the linked model out to @p out. */
static void func_801A0DC0(EffectEntity *entity, SVECTOR *out) {
    *out = entity->unk014->unk030;
}

/** @brief Copy @c unk038 of the linked model out to @p out. */
static void func_801A0DF0(EffectEntity *entity, SVECTOR *out) {
    *out = entity->unk014->unk038;
}

/** @brief Copy @c unk040 of the linked model out to @p out. */
static void func_801A0E20(EffectEntity *entity, SVECTOR *out) {
    *out = entity->unk014->unk040;
}

/** @brief As @ref func_801A0C8C, but for the slot the animation set names. */
static void func_801A0E50(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->animSet->slot];
    SVECTOR top;
    SVECTOR bottom;

    if (!(slot->flags & BATTLE_SLOT_FLAG_UNK02)) {
        return;
    }
    func_800B3960(slot, 0xF0, 0, &top);
    func_800B3960(slot, 0xF1, 0, &bottom);
    entity->unk040.vx = top.vx;
    entity->unk040.vy = top.vy;
    entity->unk040.vz = top.vz;
    entity->unk038.vx = (top.vx + bottom.vx) / 2;
    entity->unk038.vy = (top.vy + bottom.vy) / 2;
    entity->unk038.vz = (top.vz + bottom.vz) / 2;
    entity->unk030.vx = bottom.vx;
    entity->unk030.vy = slot->unk024;
    entity->unk030.vz = bottom.vz;
}

/** @brief Copy @c unk030 of the linked model out to @p out. */
static void func_801A0F8C(EffectEntity *entity, SVECTOR *out) {
    *out = entity->unk010->unk030;
}

/** @brief Copy @c unk038 of the linked model out to @p out. */
static void func_801A0FBC(EffectEntity *entity, SVECTOR *out) {
    *out = entity->unk010->unk038;
}

/** @brief Copy @c unk040 of the linked model out to @p out. */
static void func_801A0FEC(EffectEntity *entity, SVECTOR *out) {
    *out = entity->unk010->unk040;
}

/**
 * @brief Recompute the model's bounding box from its posed skeleton.
 *
 * Transforms every joint after the root through the GTE and keeps the running
 * minimum and maximum, then publishes the box back onto the entity.
 *
 * @param entity Entity whose model is measured.
 */
static void func_801A101C(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];
    EffectSkeleton *skeleton = *slot->skeleton;
    EffectJoint *joint = skeleton->joints;
    EffectBoundsScratch *b = func_800B3698(sizeof(EffectBoundsScratch));
    SVECTOR *bounds;
    s32 i;
    s16 x = joint->mtx.t[0];
    s16 y;
    s16 z;

    b->max.vx = x;
    b->min.vx = x;
    y = joint->mtx.t[1];
    b->max.vy = y;
    b->min.vy = y;
    z = joint->mtx.t[2];
    b->max.vz = z;
    b->min.vz = z;
    b->v.vx = 0;
    b->v.vy = 0;
    joint = &skeleton->joints[1];
    for (i = 1; i < skeleton->count; i++) {
        SetRotMatrix(&joint->mtx);
        SetTransMatrix(&joint->mtx);
        b->v.vz = -joint->unk002;
        gte_ldv0(&b->v);
        gte_mvmva(1, 0, 0, 0, 0);
        gte_stlvnl(&b->pos);
        gte_stflg(&b->flag);
        if (b->pos.vx < b->min.vx) {
            b->min.vx = b->pos.vx;
        } else if (b->max.vx < b->pos.vx) {
            b->max.vx = b->pos.vx;
        }
        if (b->pos.vy < b->min.vy) {
            b->min.vy = b->pos.vy;
        } else if (b->max.vy < b->pos.vy) {
            b->max.vy = b->pos.vy;
        }
        if (b->pos.vz < b->min.vz) {
            b->min.vz = b->pos.vz;
        } else if (b->max.vz < b->pos.vz) {
            b->max.vz = b->pos.vz;
        }
        joint++;
    }
    /* The box overlays unk048/unk04C, which the script opcodes use as a table
       pointer -- the two never overlap in time. */
    bounds = (SVECTOR *)entity->unk048;
    bounds[0] = b->min;
    bounds[1] = b->max;
    func_800B36B8(sizeof(EffectBoundsScratch));
}

/** @brief Copy the linked model's bounding box out to @p min and @p max. */
static void func_801A1268(EffectEntity *entity, SVECTOR *min, SVECTOR *max) {
    EffectModel *model = entity->unk014;

    *min = model->boundsMin;
    *max = model->boundsMax;
}

/**
 * @brief Half the longest side of the model's bounding box.
 *
 * @param entity Entity whose model is measured.
 * @return Half the largest of the box's three extents.
 */
static s32 func_801A12B8(EffectEntity *entity) {
    EffectModel *model = entity->unk014;
    s16 dx = model->boundsMax.vx - model->boundsMin.vx;
    s16 dy = model->boundsMax.vy - model->boundsMin.vy;
    s16 dz = model->boundsMax.vz - model->boundsMin.vz;
    if (dx > dy) {
        if (dx > dz) {
            return dx / 2;
        }
        return dz / 2;
    }
    if (dy > dz) {
        return dy / 2;
    }
    return dz / 2;
}

/** @brief Half the height of the linked model's bounding box. */
static s16 func_801A135C(EffectEntity *entity) {
    EffectModel *model = entity->unk014;

    return (s16)(model->boundsMax.vy - model->boundsMin.vy) / 2;
}

/** @brief Half the linked model's larger horizontal extent. */
static s16 func_801A138C(EffectEntity *entity) {
    EffectModel *model = entity->unk014;
    s16 spanX = model->boundsMax.vx - model->boundsMin.vx;
    s16 spanZ = model->boundsMax.vz - model->boundsMin.vz;
    /* Compared as `span` but returned as `spanX`: testing a separate copy is
       what puts the wider-X arm on the taken branch. */
    s16 span = spanX;

    return (span <= spanZ ? spanZ : spanX) / 2;
}

/** @brief Height of the linked model's bounding box. */
static s16 func_801A13E4(EffectEntity *entity) {
    return entity->unk014->boundsMax.vy - entity->unk014->boundsMin.vy;
}

/** @brief Y of the top of the linked model, in battle-entity space. */
static s16 func_801A1408(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];

    return slot->unk01E + entity->unk014->boundsMax.vy;
}

/** @brief Y of the bottom of the linked model, in battle-entity space. */
static s16 func_801A1450(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];

    return slot->unk01E + entity->unk014->boundsMin.vy;
}

/** @brief A random point up the linked model, in battle-entity space. */
static s16 func_801A1498(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];
    EffectModel *model = entity->unk014;
    s16 height = model->boundsMax.vy - model->boundsMin.vy;
    s16 offset = (rand() & 0xFFF) * height / 4096;

    return slot->unk01E + model->boundsMax.vy - offset / 2;
}

/** @brief Y of the centre of the linked model, in battle-entity space. */
static s16 func_801A1550(EffectEntity *entity) {
    EffectModel *model = entity->unk014;

    return D_800EF2D0[entity->unk02D].unk01E +
           (model->boundsMax.vy + model->boundsMin.vy) / 2;
}

/** @brief Centre of the linked model's bounding box. */
static void func_801A15A4(EffectEntity *entity, SVECTOR *out) {
    EffectModel *model = entity->unk014;

    out->vx = (model->boundsMax.vx + model->boundsMin.vx) / 2;
    out->vy = (model->boundsMax.vy + model->boundsMin.vy) / 2;
    out->vz = (model->boundsMax.vz + model->boundsMin.vz) / 2;
}

/** @brief Zero @p size bytes' worth of words starting at @p dst. */
static void func_801A1610(s32 *dst, s32 size) {
    s32 i;

    for (i = 0; i < size / 4; i++) {
        *dst = 0;
        dst++;
    }
}

/** @brief Build this effect's primitive and link it into the battle display list. */
static void func_801A1648(EffectEntity *entity) {
    s16 count;
    EffectPrim *prim;

    if (entity->flags & EFFECT_FLAG_DONE) {
        return;
    }
    prim = func_800B3698(0xB4);
    func_800C96E4(&entity->pos, ONE, entity->unk054);
    prim->unk000 = entity->unk04C;
    count = entity->unk050;
    prim->unk024 = 0;
    prim->unk004 = count;
    D_801C58F4 = func_800C9E10(prim, D_800FA5E8->ot, 2, D_801C58F4);
    func_800B36B8(0xB4);
}

/** @brief Advance the script's counter, clamping at its limit. */
static s32 func_801A16E4(EffectEntity *entity) {
    entity->unk050++;
    if (entity->unk052 < entity->unk050) {
        entity->unk050 = entity->unk052;
        entity->flags |= EFFECT_FLAG_DONE;
        return 1;
    }
    return 0;
}

/** @brief Pose @p model at the entity's position and link it into the OT. */
static void func_801A172C(EffectEntity *entity, void *model, CVECTOR *colour) {
    MATRIX m;
    void **head;
    EffectPrim *prim;

    if (entity->flags & EFFECT_FLAG_DONE) {
        return;
    }
    func_801A060C(&m);
    func_801A0644(&m, 0x400);
    m.t[0] = entity->pos.vx;
    m.t[1] = 0;
    m.t[2] = entity->pos.vz;
    CompMatrix(&D_800F02C8, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    prim = func_800B3698(0xB4);
    head = &D_801C58F4;
    prim->unk000 = model;
    prim->unk004 = 0;
    prim->unk024 = 4;
    prim->unk01C = *colour;
    *head = func_800C9E10(prim, D_800FA5E8->ot, 2, *head);
    func_800B36B8(0xB4);
}

/** @brief Refresh the render matrices from @p pose and apply the offset. */
static void func_801A181C(EffectRender *render, BattleEffectSlot *slot) {
    SVECTOR offset;

    render->unk014 = slot->mtx;
    render->unk054 = render->unk014;
    switch (render->unk0D2) {
    case 0:
        ApplyMatrixSV(&render->unk054, &render->unk0B4, &offset);
        render->unk054.t[0] += offset.vx;
        render->unk054.t[1] += offset.vy;
        render->unk054.t[2] += offset.vz;
        break;
    case 1:
        render->unk054.t[0] += render->unk0B4.vx;
        render->unk054.t[1] += render->unk0B4.vy;
        render->unk054.t[2] += render->unk0B4.vz;
        break;
    }
}

/** @brief Compose each joint's matrix through the pose and the per-joint offset. */
static void func_801A1960(EffectSkeletonRef *ref, EffectPose *pose) {
    MATRIX *offset = D_801D5D20;
    EffectSkeleton *skeleton = *ref->skeleton;
    EffectJoint *joint = skeleton->joints;
    s32 i;

    for (i = 0; i < skeleton->count; i++) {
        CompMatrix(&pose->world, &joint->mtx, offset);
        CompMatrix(&pose->view, &joint->mtx, &joint->mtx);
        joint++;
        offset++;
    }
}

/** @brief Rebase a table of absolute pointers into offsets from its own head. */
static void func_801A1A0C(s32 *table) {
    s32 *base;
    s32 count;
    s32 i;

    base = table;
    count = *table;
    for (i = 0; i < count; i++) {
        table++;
        *table -= (s32)base;
    }
}

/** @brief Aim the render's model at the camera reference point. */
static void func_801A1A4C(EffectRender *render) {
    EffectAimScratch *a = func_800B3698(sizeof(EffectAimScratch));
    s32 dx;
    s32 dy;
    s32 dz;

    a->pos.vx = render->unk054.t[0];
    a->pos.vy = render->unk054.t[1];
    a->pos.vz = render->unk054.t[2];
    dx = a->pos.vx - render->unk0DC;
    a->delta.vx = dx;
    dy = a->pos.vy - render->unk0DE;
    a->delta.vy = dy;
    dz = a->pos.vz - render->unk0E0;
    a->delta.vz = dz;
    a->distSq = dx * dx + dy * dy + dz * dz;
    a->dist = SquareRoot0(a->distSq);
    a->up.vx = 0;
    a->up.vy = ONE;
    a->up.vz = 0;
    VectorNormalS(&a->delta, &a->dir);
    func_801A0960(&a->rot, &a->dir, &a->up);
    TransposeMatrix(&a->rot, &a->out);
    a->out.t[0] = 0;
    a->out.t[1] = 0;
    a->out.t[2] = render->scale;
    a->rot = render->unk014;
    a->rot.t[0] = render->unk014.t[0] - render->unk054.t[0];
    a->rot.t[1] = render->unk014.t[1] - render->unk054.t[1];
    a->rot.t[2] = render->unk014.t[2] - render->unk054.t[2];
    gte_MulMatrix0(&a->out, &a->rot, &render->unk074);
    gte_SetTransMatrix(&a->out);
    gte_ldlv0(a->rot.t);
    gte_mvmva(1, 0, 0, 0, 0);
    gte_stlvnl(render->unk074.t);
    func_800B36B8(sizeof(EffectAimScratch));
}

/** @brief Reset a render request to its default pose, white, and unit scale. */
static void func_801A1D38(EffectRender *render, void *pose, EffectRenderPart *part,
                   void *texture, BattleEffectSlot *source, s32 frame) {
    render->unk00C = &D_800FA5F0;
    render->unk000 = pose;
    render->part = part;
    render->unk010 = &D_801C58F4;
    render->unk0B4.vx = 0;
    render->unk0B4.vy = 0;
    render->unk0B4.vz = 0;
    render->r = 0x80;
    render->g = 0x80;
    render->b = 0x80;
    render->scale = 0x2000;
    render->unk0D2 = 1;
    render->unk0CE = 0;
    render->unk0F2 = 0;
    render->unk0A4.vx = 0;
    render->unk0A4.vy = 0;
    render->unk0A4.vz = 0;
    render->unk0AC.vx = 0;
    render->unk0AC.vy = 0;
    render->unk0AC.vz = 0;
    render->unk0BC.vx = 0;
    render->unk0BC.vy = 0;
    render->unk0BC.vz = 0;
    render->slot = source;
    render->unk0DA = frame;
    part->unk018 = 0x140;
    part->unk004 = texture;
    part->unk014 = 0;
    part->unk016 = 0;
    part->unk01A = 0;
    part->r = 0x80;
    part->g = 0x80;
    part->b = 0x80;
    part->unk020 = -1;
    part->unk024 = 0;
}

/** @brief Point a render request at a texture page, CLUT and source rectangle. */
static void func_801A1DF4(EffectRender *render, SVECTOR *pos, s16 tx, s16 ty,
                   s16 clutX, s16 clutY, s16 w, s16 h) {
    POLY_FT3 poly;

    render->unk0DC = pos->vx;
    render->unk0DE = pos->vy;
    render->unk0E0 = pos->vz;
    setPolyFT3(&poly);
    setSemiTrans(&poly, 1);
    setShadeTex(&poly, 1);
    poly.tpage = getTPage(1, 1, tx, ty);
    poly.clut = getClut(clutX, clutY);
    render->tpage = poly.tpage;
    render->clut = poly.clut;
    render->unk0E4 = w;
    render->unk0E6 = h;
    /* The u coordinate within the page, doubled for the 4bpp texture. */
    render->unk0E8 = ((s16)tx - (s16)(tx & ~0x3F)) * 2;
    render->unk0EA = ty & 0xFF;
}

INCLUDE_ASM("asm/ovl/effect_001/nonmatchings/effect_001", func_801A1EBC);

/** @brief Pose the effect's model and link its skeletons into the battle OT. */
static void func_801A2D0C(EffectRender *render) {
    BattleEffectSlot *slot = render->slot;

    func_801A181C(render, slot);
    if (render->unk0DA != 0xFF) {
        func_800BC420(slot->unk060);
    } else if (func_800BCA3C(slot->unk060, slot->unk06C) != 0) {
        func_800BCF6C(slot->unk060, slot->unk06C, render->unk0D8);
    }
    CompMatrix(&D_800F02C8, &render->unk014, &render->unk034);
    if (!(slot->flags & BATTLE_SLOT_FLAG_UNK20)) {
        *render->unk00C = func_800BC060(slot, D_800FA5E8->unk4040, 0x10, *render->unk00C);
    }
    func_801A1A4C(render);
    func_801A1EBC(slot->skeleton, D_800FA5E8->ot, 4, render);
    if (slot->unk078 != NULL) {
        func_801A1EBC(slot->unk078->skeleton, D_800FA5E8->ot, 4, render);
    }
    if (render->unk0DA != 0xFF) {
        func_800BC420(slot->unk060);
    }
}

/**
 * @brief Per-frame step for one impact spark.
 *
 * The spark grows over its first phase and shrinks over its second, ramping two
 * colours as it goes, then draws itself as a spinning quad built from a
 * triangle, a gouraud quad and a draw-mode command.
 *
 * @param spark Spark being stepped.
 * @return 2 once the spark has stopped and drained, 0 while it is still live.
 */
static s32 func_801A2E48(EffectSpark *spark) {
    u32 *ot = (u32 *)D_800FA5E8->ot;
    POLY_G3 *prim = D_801C58F4;
    POLY_G4 *quad;
    DR_MODE *mode;
    void *next;
    MATRIX m;
    SVECTOR pts[5];
    SVECTOR out[5];
    DVECTOR sxy[5];
    s32 otz;
    s32 i;

    func_801A0DF0((EffectEntity *)spark, &spark->pos);
    spark->pos.vy = func_801A1550((EffectEntity *)spark);
    switch (spark->pc) {
    case 0:
        spark->unk038 = 0;
        spark->unk03A = 0;
        spark->unk054 = spark->unk056;
        spark->unk030.vy = rand() & 0xFFF;
        spark->unk030.vx = rand() & 0xFFF;
        spark->unk030.vz = rand() & 0xFFF;
        spark->unk048 = spark->unk049 = spark->unk04A = 0;
        spark->unk044 = spark->unk045 = spark->unk046 = 0;
        spark->pc++;
        break;
    case 1:
        spark->unk038 += spark->unk03C;
        spark->unk03A += spark->unk03E;
        spark->unk044 += spark->r;
        spark->unk045 += spark->g;
        spark->unk046 += spark->b;
        spark->unk048 += spark->unk050;
        spark->unk049 += spark->unk051;
        spark->unk04A += spark->unk052;
        spark->unk054--;
        if (spark->unk054 <= 0) {
            spark->unk054 = spark->unk056;
            spark->pc++;
        }
        break;
    case 2:
        spark->unk038 -= spark->unk040;
        spark->unk03A -= spark->unk042;
        spark->unk044 -= spark->r;
        spark->unk045 -= spark->g;
        spark->unk046 -= spark->b;
        spark->unk048 -= spark->unk050;
        spark->unk049 -= spark->unk051;
        spark->unk04A -= spark->unk052;
        spark->unk054--;
        if (spark->unk054 <= 0) {
            spark->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
            spark->pc++;
        }
        break;
    }
    spark->unk030.vy = (spark->unk030.vy + spark->unk058) & 0xFFF;
    if (!(spark->flags & EFFECT_FLAG_DONE)) {
        RotMatrixYXZ(&spark->unk030, &m);
        pts[0].vx = 0;
        pts[0].vy = 0;
        pts[0].vz = 0;
        pts[1].vx = spark->unk038 / 2;
        pts[1].vy = spark->unk03A / 2;
        pts[1].vz = 0;
        pts[2].vx = spark->unk038 / 2;
        pts[2].vy = -spark->unk03A / 2;
        pts[2].vz = 0;
        pts[3].vx = spark->unk038;
        pts[3].vy = spark->unk03A;
        pts[3].vz = 0;
        pts[4].vx = spark->unk038;
        pts[4].vy = -spark->unk03A;
        pts[4].vz = 0;
        for (i = 0; i < 5; i++) {
            ApplyMatrixSV(&m, &pts[i], &out[i]);
            out[i].vx += spark->pos.vx;
            out[i].vy += spark->pos.vy;
            out[i].vz += spark->pos.vz;
        }
        gte_SetTransMatrix(&D_800F02C8);
        gte_SetRotMatrix(&D_800F02C8);
        gte_ldv3(&out[0], &out[1], &out[2]);
        gte_rtpt();
        gte_stsxy3(&sxy[0], &sxy[1], &sxy[2]);
        gte_avsz3();
        gte_stotz(&otz);
        otz = otz >> 2;
        gte_ldv3(&out[2], &out[3], &out[4]);
        gte_rtpt();
        gte_stsxy3(&sxy[2], &sxy[3], &sxy[4]);
        setPolyG3(prim);
        setSemiTrans(prim, 1);
        prim->r0 = spark->unk044;
        prim->g0 = spark->unk045;
        prim->b0 = spark->unk046;
        prim->r1 = spark->unk048;
        prim->g1 = spark->unk049;
        prim->b1 = spark->unk04A;
        prim->r2 = spark->unk048;
        prim->g2 = spark->unk049;
        prim->b2 = spark->unk04A;
        prim->x0 = sxy[0].vx;
        prim->y0 = sxy[0].vy;
        prim->x1 = sxy[1].vx;
        prim->y1 = sxy[1].vy;
        prim->x2 = sxy[2].vx;
        prim->y2 = sxy[2].vy;
        AddPrim(&ot[otz], prim);
        quad = (POLY_G4 *)(prim + 1);
        setPolyG4(quad);
        setSemiTrans(quad, 1);
        quad->r0 = spark->unk048;
        quad->g0 = spark->unk049;
        quad->b0 = spark->unk04A;
        quad->r1 = spark->unk048;
        quad->g1 = spark->unk049;
        quad->b1 = spark->unk04A;
        quad->r3 = quad->g3 = quad->b3 = 0;
        quad->r2 = quad->g2 = quad->b2 = 0;
        quad->x0 = sxy[1].vx;
        quad->y0 = sxy[1].vy;
        quad->x1 = sxy[2].vx;
        quad->y1 = sxy[2].vy;
        quad->x2 = sxy[3].vx;
        quad->y2 = sxy[3].vy;
        quad->x3 = sxy[4].vx;
        quad->y3 = sxy[4].vy;
        AddPrim(&ot[otz], quad);
        mode = (DR_MODE *)(quad + 1);
        SetDrawMode(mode, 0, 0, GetTPage(0, 1, 0x280, 0), NULL);
        AddPrim(&ot[otz], mode);
        next = mode + 1;
        D_801C58F4 = next;
    }
    spark->unk024++;
    if (spark->flags & EFFECT_FLAG_STOP) {
        if (spark->wait == 0) {
            func_801A0AD8((EffectEntity *)spark);
            return 2;
        }
    }
    return 0;
}

/**
 * @brief Spawn this frame's sparks for the impact burst.
 *
 * Only the frames in the middle of the script spawn anything, and the burst
 * peaks two frames before it stops. Each spark drifts left or right at random.
 *
 * @param entity Script spawning the sparks.
 */
static void func_801A351C(EffectEntity *entity) {
    EffectSpark *spark;
    s32 count;
    s32 i;

    if (entity->unk024 < 4 || entity->unk024 > 10) {
        return;
    }
    if (entity->unk024 >= 11) {
        count = 2;
    } else if (entity->unk024 >= 9) {
        count = 4;
    } else if (entity->unk024 >= 7) {
        count = 3;
    } else {
        count = 2;
    }
    for (i = 0; i < count; i++) {
        spark = (EffectSpark *)func_801A0B00(&D_801D5CF0,
                                             func_801A2E48, 0x5C,
                                             entity);
        spark->unk056 = 8;
        spark->r = 0x10;
        spark->g = 0x10;
        spark->b = 0x10;
        spark->unk050 = 0;
        spark->unk051 = 0;
        spark->unk052 = 0x10;
        spark->unk03C = 0x100;
        spark->unk03E = 0x10;
        spark->unk040 = 0x40;
        spark->unk042 = 0x10;
        if (rand() & 1) {
            spark->unk058 = 0x40;
        } else {
            spark->unk058 = -0x40;
        }
    }
}

/**
 * @brief Spawn this frame's burst of sparks around the effect.
 *
 * The burst size ramps with the script's age. Each spark is placed at a random
 * bearing on a circle whose radius tracks the model, dropped by a random
 * fraction of the distance to the aim point, and inherits the parent's velocity.
 *
 * @param entity Script spawning the sparks.
 */
static void func_801A3640(EffectEntity *entity) {
    s32 count;
    s32 i;

    if (entity->unk024 >= 2 && entity->unk024 <= 0x7FFE) {
        if (entity->unk024 >= 11) {
            count = 2;
        } else if (entity->unk024 >= 9) {
            count = 3;
        } else if (entity->unk024 >= 7) {
            count = 2;
        } else {
            count = 1;
        }
        for (i = 0; i < count; i++) {
            s32 bearing = rand() & 0xFFF;
            EffectEntity *spark =
                func_801A0B00(&D_801CD9F0, func_801A3924, 0x6C, entity);
            s16 base;
            s16 angle;

            func_801A0DC0(spark, &spark->pos);
            spark->pos.vy = entity->pos.vy;
            base = func_801A1408(entity);
            spark->pos.vy -= (entity->pos.vy - base) * (rand() & 0x1F) / 48;
            angle = bearing;
            spark->unk068 = func_801A138C(entity) * 3 / 2;
            if (spark->unk068 >= 0x401) {
                spark->unk068 = 0x400;
            }
            spark->pos.vx += rsin(angle) * spark->unk068 / 4096;
            spark->pos.vz += rcos(angle) * spark->unk068 / 4096;
            *(SVECTOR *)&spark->unk058 = *(SVECTOR *)&entity->unk058;
        }
    }
}

/** @brief Opcode handler: start a downward drift with a randomised speed. */
static void func_801A3840(EffectEntity *entity) {
    entity->unk04C = &D_801C5410;
    entity->unk052 = 0x10;
    entity->unk05A = -(rand() & 0xF) - 8;
    entity->unk058 = 0;
    entity->unk05C = 0;
    entity->pc++;
}

/** @brief Opcode handler: drift by the current velocity until the count runs out. */
static void func_801A38A0(EffectEntity *entity) {
    entity->pos.vx += entity->unk058;
    entity->pos.vy += entity->unk05A;
    entity->pos.vz += entity->unk05C;
    if (func_801A16E4(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A391C(EffectEntity *entity) {
}

/** @brief Run one frame of this effect's script and report whether it ended. */
static s32 func_801A3924(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A3840, func_801A38A0, func_801A391C };

    handlers[entity->pc](entity);
    func_801A1648(entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_801A0AD8(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: spawn this frame's batch of scatter tasks. */
static void func_801A39CC(EffectEntity *entity) {
    s32 count;
    s32 i;

    if ((u16)(entity->unk024 - 1) >= 16) {
        return;
    }
    /* Every arm spawns one task here, but the four-way test has to be written
       out: gcc collapses the inner three into a single store and keeps the
       outer branch, whereas a flat `count = 1` folds the test away entirely.
       func_801A351C is the same shape with the arms actually differing. */
    if (entity->unk024 < 11) {
        if (entity->unk024 < 9) {
            if (entity->unk024 < 7) {
                count = 1;
            } else {
                count = 1;
            }
        } else {
            count = 1;
        }
    } else {
        count = 1;
    }
    for (i = 0; i < count; i++) {
        func_801A0B00(&D_801CD9F0, func_801A3B6C, 0x6C, entity);
    }
}

/** @brief Opcode handler: start a scatter with a randomised velocity. */
static void func_801A3A74(EffectEntity *entity) {
    entity->unk04C = &D_801C55EC;
    entity->unk052 = 8;
    entity->unk058 = (rand() & 0x1F) - 0x10;
    entity->unk05C = (rand() & 0x1F) - 0x10;
    entity->unk05A = (rand() & 0xF) + 0x10;
    entity->pc++;
}

/** @brief Opcode handler: drift by the current velocity until the count runs out. */
static void func_801A3AE8(EffectEntity *entity) {
    entity->pos.vx += entity->unk058;
    entity->pos.vy += entity->unk05A;
    entity->pos.vz += entity->unk05C;
    if (func_801A16E4(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A3B64(EffectEntity *entity) {
}

/** @brief Run one frame of this effect's script and report whether it ended. */
static s32 func_801A3B6C(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A3A74, func_801A3AE8, func_801A3B64 };

    handlers[entity->pc](entity);
    func_801A1648(entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_801A0AD8(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: aim the effect at the target and hand it to the renderer. */
static void func_801A3C14(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];
    s16 span;

    entity->unk04C = &D_801C532C;
    entity->flags |= EFFECT_FLAG_UNK02;
    /* The aim point overlays unk060/unk063, which the loop opcodes use as a
       counter -- the two never overlap in time. */
    func_801A0DC0(entity, (SVECTOR *)entity->unk060);
    ((SVECTOR *)entity->unk060)->vy = func_801A1408(entity);
    span = func_801A138C(entity);
    entity->unk068 = span * 2;
    if ((s16)(span * 2) >= 0x401) {
        entity->unk068 = 0x400;
    }
    entity->unk05A = -(func_801A13E4(entity) + 0x100) / 20;
    entity->unk05A = entity->unk05A / 4;
    func_801A1D38(&D_801D3800, &D_801D37F0, &D_801CDA00, &D_801CDA30,
                  &D_800EF2D0[entity->unk02D], entity->unk02D);
    entity->flags |= EFFECT_FLAG_UNK08;
    slot->flags |= BATTLE_SLOT_FLAG_UNK04;
    entity->pc++;
}

/** @brief Opcode handler: once the timer passes 20, latch the table and step on. */
static void func_801A3D94(EffectEntity *entity) {
    if (entity->unk024 >= 20) {
        entity->unk04C = &D_801C5380;
        entity->unk052 = 4;
        entity->pc++;
    }
}

/** @brief Opcode handler: release the battle slot once the count runs out. */
static void func_801A3DCC(EffectEntity *entity) {
    BattleEffectSlot *slot;
    EffectModel *model;

    if (func_801A16E4(entity) != 0) {
        slot = &D_800EF2D0[entity->unk02D];
        model = entity->unk010;
        entity->flags &= ~EFFECT_FLAG_UNK08;
        slot->flags &= ~BATTLE_SLOT_FLAG_UNK04;
        model->unk063 = 0;
        entity->flags &= ~EFFECT_FLAG_UNK02;
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A3E5C(EffectEntity *entity) {
}

/**
 * @brief Frame step for a script that sweeps its effect along a circle.
 *
 * Runs the opcode @c pc names, then four sub-steps per frame: each restores the
 * stored centre, advances the sweep angle and re-places the effect on the circle
 * of radius @c unk068 around it. The tint it draws with fades out over the
 * script's first twenty frames.
 *
 * @param entity Script being stepped.
 * @return 2 once the script has stopped and drained, 0 while it is still live.
 */
static s32 func_801A3E64(EffectEntity *entity) {
    EffectHandler handlers[] = {
        func_801A3C14, func_801A3D94, func_801A3DCC, func_801A3E5C,
    };
    /* Occupies sp+0x30: the original reserved this slot and never read it, but
       dropping it moves every local below and the frame no longer matches. */
    SVECTOR unused;
    CVECTOR tint;
    s32 i;

    handlers[entity->pc](entity);
    for (i = 0; i < 4; i++) {
        s32 angle;

        entity->pos = *(SVECTOR *)entity->unk060;
        angle = ((entity->unk024 * 4 + i) << 7) & 0xFFF;
        entity->unk040.vy = angle;
        entity->pos.vy += entity->unk05A * (entity->unk024 * 4 + i);
        entity->pos.vx += rsin(angle) * entity->unk068 / 4096;
        entity->pos.vz += rcos(entity->unk040.vy) * entity->unk068 / 4096;
        if (entity->flags & EFFECT_FLAG_UNK02) {
            func_801A39CC(entity);
        }
    }
    if (entity->flags & EFFECT_FLAG_UNK02) {
        func_801A3640(entity);
        func_801A351C(entity);
    }
    if (entity->flags & EFFECT_FLAG_UNK08) {
        D_801D3800.unk0B4.vy = entity->pos.vy;
        func_801A1DF4(&D_801D3800, &entity->pos, 0x200, 0x180, 0x140, 0xF1, 0x80,
                      0x80);
        func_801A2D0C(&D_801D3800);
    }
    if (entity->unk024 < 20) {
        tint.r = tint.g = tint.b = (20 - entity->unk024) * 3;
        func_801A1648(entity);
        func_801A172C(entity, D_801C58B8, &tint);
    } else {
        func_801A1648(entity);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait == 0) {
            func_801A0AD8(entity);
            return 2;
        }
    }
    return 0;
}

/** @brief Opcode handler: start a task running @ref func_801A3E64. */
static void func_801A40D4(EffectEntity *entity) {
    func_801A0B00(&D_801CD9F0, func_801A3E64, 0x6C, entity);
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A4124(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: hand the selected animation part to the battle renderer. */
static void func_801A4138(EffectEntity *entity) {
    if (entity->unk024 >= 15) {
        func_800BFE1C(&entity->animSet->anims[entity->unk02A].parts[entity->unk02B]);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A41C0(EffectEntity *entity) {
}

/** @brief Run one frame of this effect's script and report whether it ended. */
static s32 func_801A41C8(EffectEntity *entity) {
    EffectHandler handlers[4] = { func_801A40D4, func_801A4124, func_801A4138,
                                  func_801A41C0 };

    func_801A0C8C(entity);
    func_801A101C(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801A4678, 0, 0x80);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_801A0AD8(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A42A4(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A42B8(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: stall here until the wait counter runs out. */
static void func_801A42CC(EffectEntity *entity) {
    if (entity->wait == 0) {
        entity->pc++;
    }
}

/** @brief Opcode handler: start a task running @ref func_801A41C8. */
static void func_801A42F4(EffectEntity *entity) {
    entity->unk063 = 1;
    func_801A0B00(&D_801C5B50, func_801A41C8, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: repeat the preceding opcode until the count runs out. */
static void func_801A434C(EffectEntity *entity) {
    if (entity->unk063 != 0) {
        return;
    }
    if (entity->unk02A < entity->unk058) {
        entity->unk02A++;
        entity->unk02E++;
        entity->pc--;
    } else {
        entity->pc++;
    }
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A43AC(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: stall here until @c unk05E reaches zero. */
static void func_801A43C0(EffectEntity *entity) {
    if (entity->unk05E == 0) {
        entity->pc++;
    }
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A43E8(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A43FC(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: raise @ref EFFECT_FLAG_STOP and consume the opcode. */
static void func_801A4410(EffectEntity *entity) {
    entity->flags |= EFFECT_FLAG_STOP;
    entity->pc++;
}

/** @brief Opcode handler: no-op. */
static void func_801A442C(EffectEntity *entity) {
}

/**
 * @brief Advance one effect script by a single frame.
 *
 * Stages the world matrix in scratchpad so the opcode handlers and the three
 * task pools all share one copy, points the prim cursors at whichever of the
 * two buffer sets this frame's parity selects, runs the opcode @c pc names,
 * then ticks every sub-task the script has spawned.
 *
 * @param entity Script whose frame is being run.
 * @return 2 once the script has stopped and drained, 0 while it is still live.
 */
static s32 func_801A4434(EffectEntity *entity) {
    EffectHandler handlers[] = {
        func_801A42A4, func_801A42B8, func_801A42CC, func_801A42F4,
        func_801A434C, func_801A43AC, func_801A43C0, func_801A43E8,
        func_801A43FC, func_801A4410, func_801A442C,
    };
    MATRIX *world = (MATRIX *)EFFECT_SCRATCH_MATRIX;

    *world = D_800F02C8;
    D_801C58EC = world;
    D_801C58E8 = world;
    if (entity->unk05C & 1) {
        D_801C58F4 = D_801C58FC;
        D_801C58F8 = D_801C5904;
    } else {
        D_801C58F4 = D_801C5900;
        D_801C58F8 = D_801C5908;
    }
    func_801A0E50(entity);
    handlers[entity->pc](entity);
    entity->unk05E = 0;
    entity->unk05E += func_800B2B68(&D_801C5B50);
    entity->unk05E += func_800B2B68(&D_801CD9F0);
    entity->unk05E += func_800B2B68(&D_801D5CF0);
    entity->unk05C++;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait == 0) {
            func_801A0AD8(entity);
            return 2;
        }
    }
    return 0;
}
