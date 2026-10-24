/**
 * @file effect_039.c
 * @brief Drain
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "effect/effect_039.h"
#include "effect/lib/particle.h"
#include "effect/lib/entity.h"
#include "effect/lib/tint.h"
#include "effect/lib/bankclear.h"
#include "effect/lib/tables.h"
#include "effect/lib/drawscript.h"
#include "effect/lib/common.h"
#include "effect_params.h"

/** @brief Where in the scratchpad the effect keeps its view matrix. */
#define EFFECT_SCRATCHPAD ((MATRIX *)0x1F8002E0)

/** @brief @ref EffectEntity::unk05C -- the frame counter's low bit picks a bank. */
#define EFFECT_FRAME_ODD 0x1

static s32 func_801A9994(EffectEntity *entity);
static void func_801A94B4(EffectEntity *entity);
static void func_801A9500(EffectEntity *entity);
static void func_801A9568(EffectEntity *entity);
static s32 func_801A9570(EffectEntity *entity);
static void func_801A9610(EffectEntity *entity);
static void func_801A9628(EffectEntity *entity);
static void func_801A9684(EffectEntity *entity);
static s32 func_801A968C(EffectEntity *entity);
static void func_801A972C(EffectEntity *entity);
static void func_801A9788(EffectEntity *entity);
static void func_801A97D0(EffectEntity *entity);
static s32 func_801A97D8(EffectEntity *entity);
static void func_801A9878(EffectEntity *entity);
static void func_801A98D0(EffectEntity *entity);
static void func_801A9904(EffectEntity *entity);
static void func_801A998C(EffectEntity *entity);
static void func_801A9ADC(EffectEntity *entity);
static void func_801A9AF0(EffectEntity *entity);
static void func_801A9B04(EffectEntity *entity);
static void func_801A9B94(EffectEntity *entity);
static void func_801A9BEC(EffectEntity *entity);
static void func_801A9C50(EffectEntity *entity);
static void func_801A9CB8(EffectEntity *entity);
static void func_801A9CE0(EffectEntity *entity);
static void func_801A9CF4(EffectEntity *entity);
static void func_801A9D08(EffectEntity *entity);
static void func_801A9D24(EffectEntity *entity);

/** @brief Opcode handler: clear the screen tint. */
static void func_801A94B4(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx = 0;
    for (i = 0; i < 4; i++) {
        tint->level = entity->pos.vx;
        tint->b = 0;
        tint->g = 0;
        tint->r = 0;
        tint++;
    }
    entity->pc++;
}

/** @brief Opcode handler: fade the screen tint up to full, then stop. */
static void func_801A9500(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx += 0x100;
    if (entity->pos.vx >= 0x400) {
        entity->pos.vx = 0x400;
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
    for (i = 0; i < 4; i++) {
        tint->level = entity->pos.vx;
        tint++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A9568(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9570(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A94B4, func_801A9500, func_801A9568 };

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

/** @brief Opcode handler: hold the tint at a quarter. */
static void func_801A9610(EffectEntity *entity) {
    entity->pos.vx = 0x400;
    entity->pc++;
}

/** @brief Opcode handler: fade the screen tint back out, then stop. */
static void func_801A9628(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx -= 0x100;
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
static void func_801A9684(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A968C(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A9610, func_801A9628, func_801A9684 };

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

/** @brief Opcode handler: draw and retire once the frame count is reached. */
static void func_801A972C(EffectEntity *entity) {
    EffectDrawScript *script = (EffectDrawScript *)entity;

    if (entity->unk024 >= script->stopFrame) {
        /* Called with no argument: the entity is already in $a0 and stays there. */
        ((void (*)())effectDrawScriptStart)();
        effectDrawScriptRun(script);
        entity->pc++;
    }
}

/** @brief Opcode handler: hold still until the count runs out. */
static void func_801A9788(EffectEntity *entity) {
    if (effectDrawScriptRun((EffectDrawScript *)entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A97D0(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A97D8(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A972C, func_801A9788, func_801A97D0 };

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

/** @brief Opcode handler: start the pose script on the taken model. */
static void func_801A9878(EffectEntity *entity) {
    effectStartChildScript(entity, func_801A97D8, &D_801AC3C8, 0, 0x2D, 0);
    entity->pc++;
}

/** @brief Opcode handler: release the model once the count reaches 30. */
static void func_801A98D0(EffectEntity *entity) {
    if (entity->unk024 == 30) {
        entity->unk010->unk063 = 0;
        entity->pc++;
    }
}

/** @brief Opcode handler: hand the current part to battle, then stop. */
static void func_801A9904(EffectEntity *entity) {
    if (entity->unk024 >= 30) {
        func_800BFE1C(&entity->animSet->anims[entity->unk02A].parts[entity->unk02B]);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A998C(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: the take-model script, with its three timed cues.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9994(EffectEntity *entity) {
    EffectHandler handlers[4] = { func_801A9878, func_801A98D0, func_801A9904, func_801A998C };

    effectCacheSlotAnchors(entity);
    effectUpdateModelBounds(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801AA010, 0, 0x80);
    }
    if (entity->unk024 == 4) {
        EffectTintScript *child =
            effectSpawnTask(&D_801DA544, effectTintPlay, sizeof(EffectTintScript), entity);

        child->steps = D_801A9FE4;
    }
    if (entity->unk024 == 20) {
        EffectTintScript *child =
            effectSpawnTask(&D_801DA544, effectTintPlay, sizeof(EffectTintScript), entity);

        child->steps = D_801A9FF0;
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
static void func_801A9ADC(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9AF0(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: carve both record banks and start the draw script. */
static void func_801A9B04(EffectEntity *entity) {
    if (entity->wait == 0) {
        /* Both banks are carved out of the same byte arena behind the TIM. */
        g_effectEmitterBank = (EffectEmitter *)D_801D9754;
        D_801D9754 += EFFECT_EMITTER_BANK_BYTES;
        g_effectParticleBank = (EffectParticle *)D_801D9754;
        D_801D9754 += EFFECT_PARTICLE_BANK_BYTES;
        effectBanksClear();
        effectSpawnTask(&D_801D99E4, func_801A9570, 0x40, entity);
        entity->pc++;
    }
}

/** @brief Opcode handler: start a task running @ref func_801A9994. */
static void func_801A9B94(EffectEntity *entity) {
    entity->scratch.timer.unk063 = 1;
    effectSpawnTask(&D_801DA474, func_801A9994, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: repeat the step until the loop counter runs out. */
static void func_801A9BEC(EffectEntity *entity) {
    if (entity->scratch.timer.unk063 == 0) {
        if (entity->unk02A < entity->unk058) {
            entity->unk02A++;
            entity->unk02E++;
            entity->pc--;
        } else {
            entity->scratch.timer.countdown = 10;
            entity->pc++;
        }
    }
}

/** @brief Opcode handler: spawn the trailing spark once the wait runs out. */
static void func_801A9C50(EffectEntity *entity) {
    if (--entity->scratch.timer.countdown <= 0) {
        effectSpawnTask(&D_801D99E4, func_801A968C, 0x40, entity);
        entity->pc++;
    }
}

/** @brief Opcode handler: stall here until @c unk05E reaches zero. */
static void func_801A9CB8(EffectEntity *entity) {
    if (entity->unk05E == 0) {
        entity->pc++;
    }
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9CE0(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9CF4(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: raise @ref EFFECT_FLAG_STOP and consume the opcode. */
static void func_801A9D08(EffectEntity *entity) {
    entity->flags |= EFFECT_FLAG_STOP;
    entity->pc++;
}

/** @brief Opcode handler: no-op. */
static void func_801A9D24(EffectEntity *entity) {
}

/**
 * @brief Run one frame of the effect: its opcode, then every task pool it owns.
 *
 * @return 2 once the script has stopped and its children have drained.
 */
s32 func_801A9D2C(EffectEntity *entity) {
    EffectHandler handlers[11] = {
        func_801A9ADC, func_801A9AF0, func_801A9B04, func_801A9B94, func_801A9BEC,
        func_801A9C50, func_801A9CB8, func_801A9CE0, func_801A9CF4, func_801A9D08,
        func_801A9D24
    };
    MATRIX *view = EFFECT_SCRATCHPAD;

    *view = D_800F02C8;
    g_effectStackTop = (u8 *)view;
    g_effectStackBase = view;
    if (entity->unk05C & EFFECT_FRAME_ODD) {
        g_effectPrimCursor = D_801DA21C;
        D_801DA218 = D_801DA224;
    } else {
        g_effectPrimCursor = D_801DA220;
        D_801DA218 = D_801DA228;
    }
    effectCacheAnimSlotAnchors(entity);
    handlers[entity->pc](entity);
    g_effectParticleTotal = 0;
    entity->unk05E = 0;
    g_effectLiveTotal = 0;
    entity->unk05E += func_800B2B68(&D_801DA474);
    entity->unk05E += func_800B2B68(&D_801DA544);
    entity->unk05E += func_800B2B68(&g_effectChildPool);
    entity->unk05E += func_800B2B68(&D_801D99E4);
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
