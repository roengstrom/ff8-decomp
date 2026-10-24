/**
 * @file effect_052.c
 * @brief X-Potion
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "effect/effect_052.h"
#include "effect/lib/particle.h"
#include "effect/lib/entity.h"
#include "effect/lib/tint.h"
#include "psxsdk/libgte.h"
#include "effect/lib/bankclear.h"
#include "effect/lib/tables.h"
#include "effect/lib/drawscript.h"
#include "effect/lib/common.h"

/** @brief Where in the scratchpad the effect keeps its view matrix. */
#define EFFECT_SCRATCHPAD ((MATRIX *)0x1F8002E0)

/** @brief @ref EffectEntity::unk05C -- the frame counter's low bit picks a bank. */
#define EFFECT_FRAME_ODD 0x1

static void func_801A9498(EffectEntity *entity);
static void func_801A94F4(EffectEntity *entity);
static void func_801A953C(EffectEntity *entity);
static s32 func_801A9544(EffectEntity *entity);
static void func_801A95E4(EffectEntity *entity);
static void func_801A963C(EffectEntity *entity);
static void func_801A9670(EffectEntity *entity);
static void func_801A96F8(EffectEntity *entity);
static s32 func_801A9700(EffectEntity *entity);
static void func_801A9810(EffectEntity *entity);
static void func_801A9824(EffectEntity *entity);
static void func_801A9838(EffectEntity *entity);
static void func_801A98AC(EffectEntity *entity);
static void func_801A9904(EffectEntity *entity);
static void func_801A9964(EffectEntity *entity);
static void func_801A9978(EffectEntity *entity);
static void func_801A99A0(EffectEntity *entity);
static void func_801A99B4(EffectEntity *entity);
static void func_801A99C8(EffectEntity *entity);
static void func_801A99E4(EffectEntity *entity);

/** @brief Opcode handler: draw and retire once the frame count is reached. */
static void func_801A9498(EffectEntity *entity) {
    EffectDrawScript *script = (EffectDrawScript *)entity;

    if (entity->unk024 >= script->stopFrame) {
        /* Called with no argument: the entity is already in $a0 and stays there. */
        ((void (*)())effectDrawScriptStart)();
        effectDrawScriptRun(script);
        entity->pc++;
    }
}

/** @brief Opcode handler: hold still until the count runs out. */
static void func_801A94F4(EffectEntity *entity) {
    if (effectDrawScriptRun((EffectDrawScript *)entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A953C(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9544(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A9498, func_801A94F4, func_801A953C };

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
static void func_801A95E4(EffectEntity *entity) {
    effectStartChildScript(entity, func_801A9544, &D_801D4524, 0, 0x2D, 0);
    entity->pc++;
}

/** @brief Opcode handler: release the model once the count reaches 30. */
static void func_801A963C(EffectEntity *entity) {
    if (entity->unk024 == 30) {
        entity->unk010->unk063 = 0;
        entity->pc++;
    }
}

/** @brief Opcode handler: hand the current part to battle, then stop. */
static void func_801A9670(EffectEntity *entity) {
    if (entity->unk024 >= 30) {
        func_800BFE1C(&entity->animSet->anims[entity->unk02A].parts[entity->unk02B]);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A96F8(EffectEntity *entity) {
}

/** @brief Script dispatcher: the take-model script, with its two timed cues. */
static s32 func_801A9700(EffectEntity *entity) {
    EffectHandler handlers[4] = { func_801A95E4, func_801A963C, func_801A9670, func_801A96F8 };

    effectCacheSlotAnchors(entity);
    effectUpdateModelBounds(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801A9C74, 0, 0x80);
    }
    if (entity->unk024 == 20) {
        EffectTintScript *child =
            effectSpawnTask(&D_801D44F4, effectTintPlay, sizeof(EffectTintScript), entity);

        child->steps = D_801A9C68;
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
static void func_801A9810(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9824(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: carve both record banks and start the draw script. */
static void func_801A9838(EffectEntity *entity) {
    if (entity->wait == 0) {
        /* Both banks are carved out of the same byte arena behind the TIM. */
        g_effectEmitterBank = (EffectEmitter *)D_801D39A4;
        D_801D39A4 += EFFECT_EMITTER_BANK_BYTES;
        g_effectParticleBank = (EffectParticle *)D_801D39A4;
        D_801D39A4 += EFFECT_PARTICLE_BANK_BYTES;
        effectBanksClear();
        entity->pc++;
    }
}

/** @brief Opcode handler: start a task running @ref func_801A9700. */
static void func_801A98AC(EffectEntity *entity) {
    entity->scratch.timer.unk063 = 1;
    effectSpawnTask(&D_801D4424, func_801A9700, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: repeat the preceding opcode until the count runs out. */
static void func_801A9904(EffectEntity *entity) {
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
static void func_801A9964(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: stall here until @c unk05E reaches zero. */
static void func_801A9978(EffectEntity *entity) {
    if (entity->unk05E == 0) {
        entity->pc++;
    }
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A99A0(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A99B4(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: raise @ref EFFECT_FLAG_STOP and consume the opcode. */
static void func_801A99C8(EffectEntity *entity) {
    entity->flags |= EFFECT_FLAG_STOP;
    entity->pc++;
}

/** @brief Opcode handler: no-op. */
static void func_801A99E4(EffectEntity *entity) {
}

/**
 * @brief Run one frame of the effect: its opcode, then every task pool it owns.
 *
 * @return 2 once the script has stopped and its children have drained.
 */
s32 func_801A99EC(EffectEntity *entity) {
    EffectHandler handlers[11] = {
        func_801A9810, func_801A9824, func_801A9838, func_801A98AC, func_801A9904,
        func_801A9964, func_801A9978, func_801A99A0, func_801A99B4, func_801A99C8,
        func_801A99E4
    };
    MATRIX *view = EFFECT_SCRATCHPAD;

    *view = D_800F02C8;
    g_effectStackTop = (u8 *)view;
    g_effectStackBase = view;
    if (entity->unk05C & EFFECT_FRAME_ODD) {
        g_effectPrimCursor = D_801D41CC;
        D_801D41C8 = D_801D41D4;
    } else {
        g_effectPrimCursor = D_801D41D0;
        D_801D41C8 = D_801D41D8;
    }
    effectCacheAnimSlotAnchors(entity);
    handlers[entity->pc](entity);
    g_effectParticleTotal = 0;
    entity->unk05E = 0;
    g_effectLiveTotal = 0;
    entity->unk05E += func_800B2B68(&D_801D4424);
    entity->unk05E += func_800B2B68(&D_801D44F4);
    entity->unk05E += func_800B2B68(&g_effectChildPool);
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
