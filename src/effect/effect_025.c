/**
 * @file effect_025.c
 * @brief Cura
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/inline_c.h"
#include "effect/effect_025.h"
#include "effect/effect_025_anim.h"
#include "effect/effect_025_motes.h"
#include "effect/effect_025_scroll.h"
#include "effect/lib/common.h"
#include "effect/lib/heading.h"
#include "effect/lib/entity.h"
#include "effect/lib/sprite.h"
#include "effect/lib/step.h"
#include "effect/lib/render.h"
#include "effect/lib/pose.h"
#include "effect/lib/interpolate.h"
#include "effect/lib/scatter.h"
#include "effect/lib/emit_mode.h"

/**
 * @name Per-part draw flags -- @ref EffectDrawRequest::colours
 *
 * The low half of each word is the prim's own flag word; the top bits steer
 * how @ref func_801A6A00 places and shades the part.
 * @{
 */
#define EFFECT_PART_WORLD_MATRIX 0x10000000
#define EFFECT_PART_SCROLL_UV 0x20000000
#define EFFECT_PART_DEPTH_RAMP 0x40000000
/** @} */

/**
 * @brief Build and submit one posed part's primitive packet.
 *
 * Skips the part outright when it has no scale, or when it is fully faded and
 * carries no colour. Otherwise it takes a packet off the scratchpad, points it
 * at the part's vertex animation and at the frame the pose asks for -- one of
 * the two keyed frames, or the interpolated buffer when the pose sits between
 * them -- builds the part's matrix, and hands the packet to the battle
 * particle system.
 *
 * @param pose The pose script and its current frame.
 * @param part The part as the pose walk left it.
 * @param req  Draw state: the parent matrix, the per-part colours and flags.
 */
static void func_801A6A00(s32 *pose, EffectPartPose *part,
                          EffectDrawRequest *req) {
    EffectPrimBuild *prim;
    EffectVertexAnim *anim;
    MATRIX m;
    VECTOR scale;
    u32 partFlags;
    s32 depth;
    void *head;

    /* The two scale halves and the four colour bytes are tested a word at a
       time; no single field type spells that. */
    if ((*(u32 *)&part->scale[0] | part->scale[2]) == 0) {
        return;
    }
    if (part->depth >= ONE && *(u32 *)&part->colour == 0) {
        return;
    }
    prim = func_800B3698(0x58);
    anim = (EffectVertexAnim *)((u8 *)pose[0] + ((s32 *)pose[0])[part->mesh + 2]);
    prim->stream = (s32 *)anim;
    if (part->frameA == part->frameB) {
        if (part->frameB == 0) {
            anim = (EffectVertexAnim *)anim->frames;
        } else {
            anim = (EffectVertexAnim *)&anim->frames[part->frameB * anim->count];
        }
        prim->verts = (u32 *)anim;
    } else if (part->weight == 0) {
        if (part->frameA == 0) {
            anim = (EffectVertexAnim *)anim->frames;
        } else {
            anim = (EffectVertexAnim *)&anim->frames[part->frameA * anim->count];
        }
        prim->verts = (u32 *)anim;
    } else if (part->weight == ONE) {
        if (part->frameB == 0) {
            anim = (EffectVertexAnim *)anim->frames;
        } else {
            anim = (EffectVertexAnim *)&anim->frames[part->frameB * anim->count];
        }
        prim->verts = (u32 *)anim;
    } else {
        effectInterpolateFrames((EffectVertexAnim *)prim->stream, part->frameA, part->frameB,
                                part->weight, req->unk08);
        prim->verts = (u32 *)req->unk08;
    }
    RotMatrixYXZ((SVECTOR *)part->rot, &m);
    m.t[0] = part->pos[0];
    m.t[1] = part->pos[1];
    m.t[2] = part->pos[2];
    partFlags = req->colours[part->index];
    if (partFlags & EFFECT_PART_WORLD_MATRIX) {
        ApplyMatrixLV(req->m, (VECTOR *)&m.t[0], (VECTOR *)&m.t[0]);
        m.t[0] += req->m->t[0];
        m.t[1] += req->m->t[1];
        m.t[2] += req->m->t[2];
    } else {
        CompMatrix(req->m, &m, &m);
    }
    if (*(u32 *)&part->scale[0] != ((ONE << 16) | ONE) || part->scale[2] != ONE) {
        scale.vx = part->scale[0];
        scale.vy = part->scale[1];
        scale.vz = part->scale[2];
        ScaleMatrix(&m, &scale);
    }
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    prim->flags = (u16)req->colours[part->index] | EFFECT_EMIT_UNK2000;
    depth = part->depth;
    prim->depth = depth;
    if (req->colours[part->index] & EFFECT_PART_DEPTH_RAMP) {
        prim->depth = depth + (ONE - depth) * req->unk010 / 4096;
    }
    if (prim->depth != 0) {
        prim->flags |= EFFECT_EMIT_DEPTH_CUE | EFFECT_EMIT_G_DEPTH_CUE;
        *(u32 *)&prim->r = *(u32 *)&part->colour;
    }
    if (req->colours[part->index] & EFFECT_PART_SCROLL_UV) {
        func_801A4D9C(prim->stream, ((s16 *)req->unk00C)[part->index]);
    }
    head = g_effectPrimCursor;
    g_effectPrimCursor = func_800CBC68(prim, D_800FA5E8->ot, 2, head);
    func_800B36B8(0x58);
}

/** @brief Opcode handler: load the 0x34-point aim set. */
static void func_801A6D68(EffectEntity *entity) {
    effectPoseInit(&D_801D20B8, entity->scratch.pose, 0x34);
    entity->pc++;
}

/** @brief Opcode handler: pose the model at the entity and draw it. */
static void func_801A6DB0(EffectEntity *entity) {
    EffectDrawRequest req;
    MATRIX m;
    u32 colours[1];
    s32 i;

    for (i = 0; i <= 0; i++) {
        colours[i] = 0x30;
    }
    effectSlotAnchor1(entity, &entity->pos);
    RotMatrixYXZ((SVECTOR *)&entity->unk050, &m);
    req.m = &m;
    m.t[0] = entity->pos.vx;
    m.t[1] = entity->pos.vy;
    m.t[2] = entity->pos.vz;
    ScaleMatrix(&m, (VECTOR *)&entity->unk030);
    CompMatrix(&D_800F02C8, &m, &m);
    req.colours = colours;
    req.unk08 = &D_801D3EA4;
    if (effectPoseWalk(entity->scratch.pose, func_801A6A00, &req) == 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A6E9C(EffectEntity *entity) {
}

/** @brief Script dispatcher: the orbiting script, whose two angles wrap. */
static s32 func_801A6EA4(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A6D68, func_801A6DB0, func_801A6E9C };

    handlers[entity->pc](entity);
    entity->unk050 += entity->unk058;
    entity->unk054 += entity->unk05C;
    entity->unk024++;
    entity->unk050 &= 0xFFF;
    entity->unk054 &= 0xFFF;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: reset the scale and load the 0x64-point aim set. */
static void func_801A6F7C(EffectEntity *entity) {
    ((VECTOR *)&entity->unk030)->vx = ONE;
    ((VECTOR *)&entity->unk030)->vy = ONE;
    ((VECTOR *)&entity->unk030)->vz = ONE;
    effectPoseInit(&D_801D238C, entity->scratch.pose, 0x64);
    entity->pc++;
}

/** @brief Opcode handler: draw the model upright in the mode @c unk0C4 names. */
static void func_801A6FD4(EffectEntity *entity) {
    MATRIX m;
    EffectDrawRequest req;
    u32 colours[1];
    s32 i;

    for (i = 0; i <= 0; i++) {
        colours[i] = 0x40000030;
    }
    req.unk010 = entity->unk0C4;
    effectSlotAnchor0(entity, &entity->pos);
    effectMatrixIdentity(&m);
    req.m = &m;
    m.t[0] = entity->pos.vx;
    m.t[1] = entity->pos.vy;
    m.t[2] = entity->pos.vz;
    ScaleMatrix(&m, (VECTOR *)&entity->unk030);
    CompMatrix(&D_800F02C8, &m, &m);
    req.colours = colours;
    req.unk08 = &D_801D3EA4;
    if (effectPoseWalk(entity->scratch.pose, func_801A6A00, &req) == 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A70C8(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A70D0(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A6F7C, func_801A6FD4, func_801A70C8 };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: clear the screen tint. */
static void func_801A7170(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx = 0;
    for (i = 0; i < 4; i++) {
        tint->level = 0;
        tint->b = 0;
        tint->g = 0;
        tint->r = 0;
        tint++;
    }
    entity->pc++;
}

/** @brief Opcode handler: fade the screen tint up to full. */
static void func_801A71B8(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx += 0xC0;
    if (entity->pos.vx >= 0x600) {
        entity->pos.vx = 0x600;
        entity->pc++;
    }
    for (i = 0; i < 4; i++) {
        tint->level = entity->pos.vx;
        tint++;
    }
}

/** @brief Opcode handler: drop back to the ground once the count passes 20. */
static void func_801A7214(EffectEntity *entity) {
    if (entity->unk024 >= 0x14) {
        entity->pos.vy = 0;
        entity->pc++;
    }
}

/** @brief Opcode handler: fade the screen tint back out, then stop. */
static void func_801A7240(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx -= 0xC0;
    if (entity->pos.vx <= 0) {
        entity->pos.vx = 0;
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
    for (i = 0; i < 4; i++) {
        tint->level = entity->pos.vx;
        tint++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A729C(EffectEntity *entity) {
}

/** @brief Script dispatcher: the five-step screen-tint script. */
static s32 func_801A72A4(EffectEntity *entity) {
    EffectHandler handlers[5] = { func_801A7170, func_801A71B8, func_801A7214,
                                  func_801A7240, func_801A729C };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: move the entity onto the model's second point. */
static void func_801A7354(EffectEntity *entity) {
    effectSlotAnchor1(entity, &entity->pos);
    entity->pc++;
}

/** @brief Opcode handler: spawn the frame's share of orbiting children. */
static void func_801A7390(EffectEntity *entity) {
    EffectEntity *child;
    s32 i;

    for (i = 0; i < D_801D3DF0[entity->unk024]; i++) {
        child = effectSpawnTask(&D_801D7314, func_801A6EA4, 0xC8, entity);
        ((VECTOR *)&child->unk030)->vx = ONE;
        ((VECTOR *)&child->unk030)->vy = ONE;
        ((VECTOR *)&child->unk030)->vz = ONE;
        child->unk050 = rand() & 0xFFF;
        child->unk054 = rand() & 0xFFF;
        child->unk058 = (rand() & 0x3F) + 0x20;
        child->unk05C = (rand() & 0x3F) + 0x20;
        if (rand() & 1) {
            child->unk058 = ONE - child->unk058;
        }
        if (rand() & 1) {
            child->unk05C = ONE - child->unk05C;
        }
    }
    if (entity->unk024 >= 0x12) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A74F8(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A7500(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A7354, func_801A7390, func_801A74F8 };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: sit on the model's first point and aim at the ground. */
static void func_801A75A0(EffectEntity *entity) {
    s16 y;

    effectSlotAnchor0(entity, &entity->pos);
    y = effectModelBottom(entity);
    entity->pos.vy = y;
    entity->unk040.vy = (0x100 - y) / 18;
    entity->pc++;
}

/** @brief Opcode handler: rise, spawning the frame's share of falling motes. */
static void func_801A7614(EffectEntity *entity) {
    EffectEntity *spark;
    s32 i;

    entity->pos.vy += entity->unk040.vy;
    for (i = 0; i < D_801D3E2C[entity->unk024]; i++) {
        spark = effectSpawnTask(&D_801E74F4, func_801A54DC, 0x84, entity);
        effectScatter(spark, 0x200);
        spark->pos.vy = entity->pos.vy;
        ((VECTOR *)&spark->unk030)->vx = ONE;
        ((VECTOR *)&spark->unk030)->vy = ONE;
        ((VECTOR *)&spark->unk030)->vz = ONE;
    }
    if (entity->unk024 >= 0x12) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A7728(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A7730(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A75A0, func_801A7614, func_801A7728 };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief As @ref func_801A75A0, over twenty frames rather than eighteen. */
static void func_801A77D0(EffectEntity *entity) {
    s16 y;

    effectSlotAnchor0(entity, &entity->pos);
    y = effectModelBottom(entity);
    entity->pos.vy = y;
    entity->unk040.vy = (0x100 - y) / 20;
    entity->pc++;
}

/** @brief Opcode handler: rise, spawning the frame's share of rising motes. */
static void func_801A7844(EffectEntity *entity) {
    EffectEntity *spark;
    s32 i;

    entity->pos.vy += entity->unk040.vy;
    for (i = 0; i < D_801D3E68[entity->unk024]; i++) {
        spark = effectSpawnTask(&D_801E74F4, func_801A533C, 0x84, entity);
        spark->pos.vy = entity->pos.vy - 0x180;
        effectScatter(spark, 0x200);
    }
    if (entity->unk024 >= 0x14) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A7940(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A7948(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A77D0, func_801A7844, func_801A7940 };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: take the model over and set it up to be drawn dim. */
static void func_801A79E8(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];
    EffectRender *render = &D_801ED304;
    s16 y;

    effectSlotAnchor0(entity, &entity->pos);
    y = entity->pos.vy - 0x600;
    entity->pos.vy = y;
    entity->unk040.vy = (0x100 - y) / 18;
    effectRenderReset(render, &D_801ED2F4, &D_801E7504, &D_801E7534,
                      &D_800EF2D0[entity->unk02D], entity->unk02D);
    render->r = 0x40;
    render->g = 0x40;
    render->b = 0x40;
    entity->flags |= EFFECT_FLAG_UNK08;
    slot->flags |= BATTLE_SLOT_FLAG_UNK04;
    entity->pc++;
}

/** @brief Opcode handler: fall at the armed speed, then release the model. */
static void func_801A7B1C(EffectEntity *entity) {
    BattleEffectSlot *slot;
    EffectModel *model;

    entity->pos.vy += entity->unk040.vy;
    if (entity->unk024 >= 0x12) {
        slot = &D_800EF2D0[entity->unk02D];
        model = entity->unk010;
        entity->flags &= ~EFFECT_FLAG_UNK08;
        slot->flags &= ~BATTLE_SLOT_FLAG_UNK04;
        model->unk063 = 0;
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A7BA0(EffectEntity *entity) {
}

/** @brief Script dispatcher: the model-taking script, redrawn each frame. */
static s32 func_801A7BA8(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A79E8, func_801A7B1C, func_801A7BA0 };
    EffectRender *render;
    /* Occupies sp+0x30: the original reserved this slot and never read it, but
       dropping it moves every local below and the frame no longer matches. */
    SVECTOR unused;
    SVECTOR point;

    handlers[entity->pc](entity);
    if (entity->flags & EFFECT_FLAG_UNK08) {
        render = &D_801ED304;
        effectSlotAnchor0(entity, &point);
        entity->unk052 = effectMatrixHeading(&D_800F02C8);
        entity->pos.vx = point.vx + rsin(entity->unk052) / 4;
        entity->pos.vz = point.vz + rcos(entity->unk052) / 4;
        render->unk0B4.vy = entity->pos.vy;
        effectRenderSetTexture(render, &entity->pos, 0x280, 0x1C0, 0x140, 0xF4, 0x80, 0x40);
        effectRenderModel(render);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: start the five child scripts the effect runs. */
static void func_801A7D18(EffectEntity *entity) {
    EffectEntity *child;

    effectSpawnTask(&D_801E74F4, func_801A5C24, 0x84, entity);
    child = effectSpawnTask(&D_801D7314, func_801A70D0, 0xC8, entity);
    child->unk0C4 = 0;
    effectSpawnTask(&D_801D7314, func_801A7500, 0xC8, entity);
    effectSpawnTask(&D_801D7314, func_801A7730, 0xC8, entity);
    effectSpawnTask(&D_801D7314, func_801A72A4, 0xC8, entity);
    entity->pc++;
}

/** @brief Opcode handler: start the two child scripts this step drives. */
static void func_801A7DDC(EffectEntity *entity) {
    EffectEntity *child = effectSpawnTask(&D_801D7314, func_801A70D0, 0xC8, entity);

    child->unk0C4 = 0x800;
    effectSpawnTask(&D_801D7314, func_801A7BA8, 0xC8, entity);
    entity->pc++;
}

/** @brief Opcode handler: start a child script in mode @c 0xC00. */
static void func_801A7E58(EffectEntity *entity) {
    EffectEntity *child = effectSpawnTask(&D_801D7314, func_801A70D0, 0xC8, entity);

    child->unk0C4 = 0xC00;
    entity->pc++;
}

/** @brief Opcode handler: start a child script in mode @c 0xE00. */
static void func_801A7EB0(EffectEntity *entity) {
    EffectEntity *child = effectSpawnTask(&D_801D7314, func_801A70D0, 0xC8, entity);

    child->unk0C4 = 0xE00;
    entity->pc++;
}

/** @brief Opcode handler: start the third child script once the count passes 12. */
static void func_801A7F08(EffectEntity *entity) {
    if (entity->unk024 >= 0xC) {
        effectSpawnTask(&D_801D7314, func_801A7948, 0xC8, entity);
        entity->pc++;
    }
}

/** @brief Opcode handler: hand the animation's part list to the battle renderer. */
static void func_801A7F68(EffectEntity *entity) {
    if (entity->unk024 >= 0x1F) {
        func_800BFE1C(entity->animSet->anims[entity->unk02A].parts);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A7FE0(EffectEntity *entity) {
}

/** @brief Script dispatcher: the seven-step main script, which starts the sound. */
static s32 func_801A7FE8(EffectEntity *entity) {
    EffectHandler handlers[7] = { func_801A7D18, func_801A7DDC, func_801A7E58,
                                  func_801A7EB0, func_801A7F08, func_801A7F68,
                                  func_801A7FE0 };

    effectCacheSlotAnchors(entity);
    effectUpdateModelBounds(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801A83D0, 0, 0x80);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A80DC(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: start a task running @ref func_801A7FE8. */
static void func_801A80F0(EffectEntity *entity) {
    entity->scratch.timer.unk063 = 1;
    effectSpawnTask(&D_801D4104, func_801A7FE8, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: loop back one step until the animation list runs out. */
static void func_801A8148(EffectEntity *entity) {
    if (entity->scratch.timer.unk063 != 0) {
        return;
    }
    if (entity->unk02A < entity->unk058) {
        entity->unk02A++;
        entity->pc--;
    } else {
        entity->pc++;
    }
}

/** @brief Opcode handler: stop once @c unk05E reaches zero. */
static void func_801A8198(EffectEntity *entity) {
    if (entity->unk05E == 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A81C8(EffectEntity *entity) {
}

/**
 * @brief Root script dispatcher: swap the prim bank, run one step, then run
 *        every child pool.
 *
 * @return 2 once the script has stopped and its children have drained.
 */
s32 func_801A81D0(EffectEntity *entity) {
    EffectHandler handlers[5] = { func_801A80DC, func_801A80F0, func_801A8148,
                                  func_801A8198, func_801A81C8 };

    if (entity->unk05C & 1) {
        g_effectPrimCursor = D_801D3EBC;
    } else {
        g_effectPrimCursor = D_801D3EC0;
    }
    effectCacheAnimSlotAnchors(entity);
    handlers[entity->pc](entity);
    entity->unk05E = func_800B2B68(&D_801D4104);
    entity->unk05E += func_800B2B68(&D_801D7314);
    entity->unk05E += func_800B2B68(&D_801E74F4);
    entity->unk05C++;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        effectReleaseWait(entity);
        return 2;
    }
    return 0;
}
