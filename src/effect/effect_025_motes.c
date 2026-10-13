/**
 * @file effect_025_motes.c
 * @brief Cura: the rising, falling and arc mote scripts.
 */
#include "common.h"
#include "effect.h"
#include "psxsdk/libc.h"
#include "effect/lib/entity.h"
#include "effect/lib/sprite.h"
#include "effect/lib/step.h"
#include "effect/lib/scatter.h"
#include "effect/effect_025.h"
#include "effect/effect_025_anim.h"
#include "effect/effect_025_motes.h"

static void func_801A524C(EffectEntity *entity);
static void func_801A527C(EffectEntity *entity);
static void func_801A52D8(EffectEntity *entity);
static void func_801A5334(EffectEntity *entity);
static void func_801A5404(EffectEntity *entity);
static void func_801A548C(EffectEntity *entity);
static void func_801A54D4(EffectEntity *entity);
static void func_801A55C0(EffectEntity *entity);
static void func_801A5620(EffectEntity *entity);
static void func_801A56A8(EffectEntity *entity);
static s32 func_801A56B0(EffectEntity *entity);
static void func_801A5778(EffectEntity *entity);
static void func_801A581C(EffectEntity *entity);
static void func_801A5884(EffectEntity *entity);
static void func_801A58CC(EffectEntity *entity);
static s32 func_801A58D4(EffectEntity *entity);
static void func_801A5AE8(EffectEntity *entity);
static void func_801A5BD4(EffectEntity *entity);
static void func_801A5C1C(EffectEntity *entity);

/** @brief Advance the animation frame, wrapping at its limit. */
static void func_801A524C(EffectEntity *entity) {
    entity->unk050++;
    if (entity->unk052 < entity->unk050) {
        entity->unk050 = 0;
    }
}

/** @brief Opcode handler: start a rise with a randomised speed. */
static void func_801A527C(EffectEntity *entity) {
    entity->unk04C = &D_801A8820;
    entity->unk052 = 0xA;
    entity->unk054 = -0x400;
    entity->unk05A = (rand() & 7) + 4;
    entity->pc++;
}

/** @brief Opcode handler: rise by the current speed until the count runs out. */
static void func_801A52D8(EffectEntity *entity) {
    entity->pos.vy += entity->unk05A;
    if (effectStepCounter(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A5334(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
s32 func_801A533C(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A527C, func_801A52D8, func_801A5334 };

    handlers[entity->pc](entity);
    if (entity->pos.vy > 0) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
    }
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

/** @brief Opcode handler: pick one of two table pairs at random and start a fall. */
static void func_801A5404(EffectEntity *entity) {
    if (rand() & 1) {
        entity->unk04C = &D_801A84E8;
        entity->unk070 = &D_801A8AF8;
    } else {
        entity->unk04C = &D_801A894C;
        entity->unk070 = &D_801A8CA4;
    }
    entity->unk07A = rand() & 0xFFF;
    entity->unk052 = 0xF;
    entity->unk054 = -0x200;
    entity->pc++;
}

/** @brief Opcode handler: hold still until the count runs out. */
static void func_801A548C(EffectEntity *entity) {
    if (effectStepCounter(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A54D4(EffectEntity *entity) {
}

/** @brief Script dispatcher: the falling-mote script, drawn spun and tinted. */
s32 func_801A54DC(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A5404, func_801A548C, func_801A54D4 };
    CVECTOR colour;

    handlers[entity->pc](entity);
    effectDrawSprite(entity);
    entity->unk07A = (entity->unk07A + 0x40) & 0xFFF;
    colour.b = 0x30;
    colour.g = 0x30;
    colour.r = 0x30;
    func_801A50D4(entity, entity->unk070, &colour, entity->unk07A, 0x2000);
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

/** @brief Opcode handler: start a fast rise that gravity pulls back. */
static void func_801A55C0(EffectEntity *entity) {
    entity->unk04C = &D_801A8820;
    entity->unk050 = 0;
    entity->unk052 = 0xA;
    entity->unk05A = (rand() & 7) + 0x20;
    entity->scratch.accel.vy = -3;
    entity->pc++;
}

/** @brief Opcode handler: apply gravity, then rise until the count runs out. */
static void func_801A5620(EffectEntity *entity) {
    entity->unk05A += entity->scratch.accel.vy;
    if (entity->unk05A < 2) {
        entity->unk05A = 2;
    }
    entity->pos.vy += entity->unk05A;
    if (effectStepCounter(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A56A8(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A56B0(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A55C0, func_801A5620, func_801A56A8 };

    handlers[entity->pc](entity);
    if (entity->pos.vy > 0) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
    }
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

/** @brief Opcode handler: keep the start point and start a randomised arc. */
static void func_801A5778(EffectEntity *entity) {
    entity->unk078 = rand() & 0xFFF;
    *(SVECTOR *)&entity->unk068 = entity->pos;
    entity->unk04C = &D_801A8708;
    entity->unk052 = 1;
    entity->unk05A = (rand() & 0x3F) - 0x30;
    entity->unk07C = (rand() & 0x1F) + 0x50;
    entity->unk07E = -entity->unk07C / 16;
    entity->pc++;
}

/** @brief Opcode handler: switch to the burst table once the frame count passes 16. */
static void func_801A581C(EffectEntity *entity) {
    func_801A524C(entity);
    if (entity->unk024 >= 0x10) {
        entity->unk04C = &D_801A875C;
        entity->unk052 = 6;
        entity->unk050 = 0;
        entity->flags |= EFFECT_FLAG_UNK10;
        entity->pc++;
    }
}

/** @brief Opcode handler: hold still until the count runs out. */
static void func_801A5884(EffectEntity *entity) {
    if (effectStepCounter(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A58CC(EffectEntity *entity) {
}

/** @brief Script dispatcher: an arc under gravity that trails motes behind it. */
static s32 func_801A58D4(EffectEntity *entity) {
    EffectHandler handlers[4] = { func_801A5778, func_801A581C, func_801A5884,
                                  func_801A58CC };
    EffectEntity *spark;
    CVECTOR colour;

    handlers[entity->pc](entity);
    entity->unk07C += entity->unk07E;
    if (entity->unk07C < 0) {
        entity->unk07C = 0;
    }
    entity->unk068 += rsin(entity->unk040.vy) * entity->unk07C / 4096;
    entity->unk06C += rcos(entity->unk040.vy) * entity->unk07C / 4096;
    entity->pos.vx = entity->unk068;
    entity->pos.vz = entity->unk06C;
    entity->unk05A += 0xA;
    entity->pos.vy += entity->unk05A;
    if (!(entity->flags & EFFECT_FLAG_UNK10) && (entity->unk024 & 1)) {
        spark = effectSpawnTask(&D_801E74F4, func_801A56B0, 0x84, entity);
        effectScatter(spark, 0x80);
        spark->pos.vy = entity->pos.vy;
        ((VECTOR *)&spark->unk030)->vx = ONE;
        ((VECTOR *)&spark->unk030)->vy = ONE;
        ((VECTOR *)&spark->unk030)->vz = ONE;
    }
    if (entity->pos.vy > 0) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
    }
    effectDrawSprite(entity);
    colour.b = 0x50;
    colour.g = 0x50;
    colour.r = 0x50;
    func_801A4FE0(entity, entity->unk04C, &colour);
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

/** @brief Opcode handler: ring eight sparks around the model's first point. */
static void func_801A5AE8(EffectEntity *entity) {
    EffectEntity *spark;
    s32 angle;
    s32 i;

    effectSlotAnchor0(entity, &entity->pos);
    entity->unk04C = &D_801A8694;
    entity->unk052 = 3;
    entity->pos.vy -= 0x600;
    angle = rand() & 0xFFF;
    for (i = 0; i < 8; i++) {
        spark = effectSpawnTask(&D_801E74F4, func_801A58D4, 0x84, entity);
        ((VECTOR *)&spark->unk030)->vx = ONE;
        ((VECTOR *)&spark->unk030)->vy = ONE;
        ((VECTOR *)&spark->unk030)->vz = ONE;
        spark->unk040.vy = angle + (rand() & 0x7F);
        angle = (angle + 0x200) & 0xFFF;
    }
    entity->pc++;
}

/** @brief Opcode handler: hold still until the count runs out. */
static void func_801A5BD4(EffectEntity *entity) {
    if (effectStepCounter(entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A5C1C(EffectEntity *entity) {
}

/** @brief Script dispatcher: the rising-mote script, drawn tinted. */
s32 func_801A5C24(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A5AE8, func_801A5BD4, func_801A5C1C };
    CVECTOR colour;

    handlers[entity->pc](entity);
    effectDrawSprite(entity);
    colour.b = 0x20;
    colour.g = 0x20;
    colour.r = 0x20;
    func_801A4FE0(entity, entity->unk04C, &colour);
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
