/**
 * @file effect_001.c
 * @brief Cure
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/inline_c.h"
#include "effect/effect_001.h"
#include "effect/effect_001_anim.h"
#include "effect/lib/common.h"
#include "effect/lib/entity.h"
#include "effect/lib/sprite.h"
#include "effect/lib/step.h"
#include "effect/lib/render.h"

static s32 func_801A2E48(EffectSpark *spark);
static void func_801A351C(EffectEntity *entity);
static void func_801A3640(EffectEntity *entity);
static s32 func_801A3924(EffectEntity *entity);
static s32 func_801A3B6C(EffectEntity *entity);
static s32 func_801A3E64(EffectEntity *entity);
static s32 func_801A41C8(EffectEntity *entity);

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
    u32 *ot = D_800FA5E8->ot;
    POLY_G3 *prim = g_effectPrimCursor;
    POLY_G4 *quad;
    DR_MODE *mode;
    void *next;
    MATRIX m;
    SVECTOR pts[5];
    SVECTOR out[5];
    DVECTOR sxy[5];
    s32 otz;
    s32 i;

    effectSlotAnchor1((EffectEntity *)spark, &spark->pos);
    spark->pos.vy = effectModelCentreY((EffectEntity *)spark);
    switch (spark->pc) {
    case 0:
        spark->unk038 = 0;
        spark->unk03A = 0;
        spark->unk054 = spark->unk056;
        spark->unk030.vy = rand() & EFFECT_MESH_INDEX_MASK;
        spark->unk030.vx = rand() & EFFECT_MESH_INDEX_MASK;
        spark->unk030.vz = rand() & EFFECT_MESH_INDEX_MASK;
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
    spark->unk030.vy = (spark->unk030.vy + spark->unk058) & EFFECT_MESH_INDEX_MASK;
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
        g_effectPrimCursor = next;
    }
    spark->unk024++;
    if (spark->flags & EFFECT_FLAG_STOP) {
        if (spark->wait == 0) {
            effectReleaseWait((EffectEntity *)spark);
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
        spark = effectSpawnTask(&D_801D5CF0,
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
            s32 bearing = rand() & EFFECT_MESH_INDEX_MASK;
            EffectEntity *spark =
                effectSpawnTask(&D_801CD9F0, func_801A3924, 0x6C, entity);
            s16 base;
            s16 angle;

            effectSlotAnchor0(spark, &spark->pos);
            spark->pos.vy = entity->pos.vy;
            base = effectModelTop(entity);
            spark->pos.vy -= (entity->pos.vy - base) * (rand() & 0x1F) / 48;
            angle = bearing;
            spark->unk068 = effectModelHalfWidth(entity) * 3 / 2;
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
    if (effectStepCounter(entity) != 0) {
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
    effectDrawSprite(entity);
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
        effectSpawnTask(&D_801CD9F0, func_801A3B6C, 0x6C, entity);
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
    if (effectStepCounter(entity) != 0) {
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
    effectDrawSprite(entity);
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

/** @brief Opcode handler: aim the effect at the target and hand it to the renderer. */
static void func_801A3C14(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];
    s16 span;

    entity->unk04C = &D_801C532C;
    entity->flags |= EFFECT_FLAG_UNK02;
    /* The aim point overlays unk060/unk063, which the loop opcodes use as a
       counter -- the two never overlap in time. */
    effectSlotAnchor0(entity, &entity->scratch.anchor);
    entity->scratch.anchor.vy = effectModelTop(entity);
    span = effectModelHalfWidth(entity);
    entity->unk068 = span * 2;
    if ((s16)(span * 2) >= 0x401) {
        entity->unk068 = 0x400;
    }
    entity->unk05A = -(effectModelHeight(entity) + 0x100) / 20;
    entity->unk05A = entity->unk05A / 4;
    effectRenderReset(&D_801D3800, &D_801D37F0, &D_801CDA00, &D_801CDA30,
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

    if (effectStepCounter(entity) != 0) {
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

        entity->pos = entity->scratch.anchor;
        angle = ((entity->unk024 * 4 + i) << 7) & EFFECT_MESH_INDEX_MASK;
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
        effectRenderSetTexture(&D_801D3800, &entity->pos, 0x200, 0x180, 0x140, 0xF1, 0x80,
                               0x80);
        effectRenderModel(&D_801D3800);
    }
    if (entity->unk024 < 20) {
        tint.r = tint.g = tint.b = (20 - entity->unk024) * 3;
        effectDrawSprite(entity);
        func_801A172C(entity, &D_801C58B8, &tint);
    } else {
        effectDrawSprite(entity);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait == 0) {
            effectReleaseWait(entity);
            return 2;
        }
    }
    return 0;
}

/** @brief Opcode handler: start a task running @ref func_801A3E64. */
static void func_801A40D4(EffectEntity *entity) {
    effectSpawnTask(&D_801CD9F0, func_801A3E64, 0x6C, entity);
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

    effectCacheSlotAnchors(entity);
    effectUpdateModelBounds(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801A4678, 0, 0x80);
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
    entity->scratch.timer.unk063 = 1;
    effectSpawnTask(&D_801C5B50, func_801A41C8, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: repeat the preceding opcode until the count runs out. */
static void func_801A434C(EffectEntity *entity) {
    if (entity->scratch.timer.unk063 != 0) {
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
s32 func_801A4434(EffectEntity *entity) {
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
        g_effectPrimCursor = D_801C58FC;
        D_801C58F8 = D_801C5904;
    } else {
        g_effectPrimCursor = D_801C5900;
        D_801C58F8 = D_801C5908;
    }
    effectCacheAnimSlotAnchors(entity);
    handlers[entity->pc](entity);
    entity->unk05E = 0;
    entity->unk05E += func_800B2B68(&D_801C5B50);
    entity->unk05E += func_800B2B68(&D_801CD9F0);
    entity->unk05E += func_800B2B68(&D_801D5CF0);
    entity->unk05C++;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait == 0) {
            effectReleaseWait(entity);
            return 2;
        }
    }
    return 0;
}
