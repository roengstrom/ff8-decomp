/**
 * @file effect_027.c
 * @brief Full-Life
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "psxsdk/libgte.h"
#include "effect/effect_027.h"
#include "effect/lib/particle.h"
#include "effect/lib/entity.h"
#include "effect/lib/tables.h"
#include "effect/lib/drawscript.h"
#include "effect/lib/bankclear.h"
#include "effect/lib/common.h"
#include "battle.h"
#include "effect_params.h"

/** @brief Where in the scratchpad the effect keeps its view matrix. */
#define EFFECT_SCRATCHPAD ((MATRIX *)0x1F8002E0)

/** @brief @ref EffectEntity::unk05C -- the frame counter's low bit picks a bank. */
#define EFFECT_FRAME_ODD 0x1

/** @brief Ceiling the tint ramp clamps to, four @ref EFFECT_TINT_STEP steps up. */
#define EFFECT_TINT_MAX 0x400

/** @brief How far the tint ramp moves per frame. */
#define EFFECT_TINT_STEP 0x100

static void func_801A91BC(EffectEntity *entity);
static void func_801A9208(EffectEntity *entity);
static void func_801A9270(EffectEntity *entity);
static s32 func_801A9278(EffectEntity *entity);
static void func_801A9318(EffectEntity *entity);
static void func_801A9330(EffectEntity *entity);
static void func_801A938C(EffectEntity *entity);
static s32 func_801A9394(EffectEntity *entity);
static void func_801A9434(EffectEntity *entity);
static void func_801A9490(EffectEntity *entity);
static void func_801A94D8(EffectEntity *entity);
static s32 func_801A94E0(EffectEntity *entity);
static void func_801A9580(EffectEntity *entity);
static void func_801A95D8(EffectEntity *entity);
static void func_801A960C(EffectEntity *entity);
static void func_801A9694(EffectEntity *entity);
static s32 func_801A969C(EffectEntity *entity);
static void func_801A9778(EffectEntity *entity);
static void func_801A978C(EffectEntity *entity);
static void func_801A97A0(EffectEntity *entity);
static void func_801A9830(EffectEntity *entity);
static void func_801A9888(EffectEntity *entity);
static void func_801A98EC(EffectEntity *entity);
static void func_801A9954(EffectEntity *entity);
static void func_801A997C(EffectEntity *entity);
static void func_801A9990(EffectEntity *entity);
static void func_801A99A4(EffectEntity *entity);
static void func_801A99C0(EffectEntity *entity);

/** @brief Opcode handler: clear the screen tint. */
static void func_801A91BC(EffectEntity *entity) {
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

/** @brief Opcode handler: ramp the screen tint up to the maximum, then stop. */
static void func_801A9208(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx += EFFECT_TINT_STEP;
    if (entity->pos.vx >= EFFECT_TINT_MAX) {
        entity->pos.vx = EFFECT_TINT_MAX;
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
    for (i = 0; i < 4; i++) {
        tint->level = entity->pos.vx;
        tint++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A9270(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher for the tint fade-in.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9278(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A91BC, func_801A9208, func_801A9270 };

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

/** @brief Opcode handler: hold the screen tint at the maximum. */
static void func_801A9318(EffectEntity *entity) {
    entity->pos.vx = EFFECT_TINT_MAX;
    entity->pc++;
}

/** @brief Opcode handler: ramp the screen tint back to zero, then stop. */
static void func_801A9330(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx -= EFFECT_TINT_STEP;
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
static void func_801A938C(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher for the tint fade-out.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9394(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A9318, func_801A9330, func_801A938C };

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
static void func_801A9434(EffectEntity *entity) {
    EffectDrawScript *script = (EffectDrawScript *)entity;

    if (entity->unk024 >= script->stopFrame) {
        /* Called with no argument: the entity is already in $a0 and stays there. */
        ((void (*)())effectDrawScriptStart)();
        effectDrawScriptRun(script);
        entity->pc++;
    }
}

/** @brief Opcode handler: hold still until the draw script finishes. */
static void func_801A9490(EffectEntity *entity) {
    if (effectDrawScriptRun((EffectDrawScript *)entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A94D8(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher for the draw script.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A94E0(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A9434, func_801A9490, func_801A94D8 };

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
static void func_801A9580(EffectEntity *entity) {
    effectStartChildScript(entity, func_801A94E0, &D_801D4314, 0, 0x2D, 0);
    entity->pc++;
}

/** @brief Opcode handler: release the model once the count reaches 30. */
static void func_801A95D8(EffectEntity *entity) {
    if (entity->unk024 == 30) {
        entity->unk010->unk063 = 0;
        entity->pc++;
    }
}

/** @brief Opcode handler: hand the current part to battle, then stop. */
static void func_801A960C(EffectEntity *entity) {
    if (entity->unk024 >= 30) {
        func_800BFE1C(&entity->animSet->anims[entity->unk02A].parts[entity->unk02B]);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A9694(EffectEntity *entity) {
}

/** @brief Run one frame of this effect's script and report whether it ended. */
static s32 func_801A969C(EffectEntity *entity) {
    EffectHandler handlers[4] = { func_801A9580, func_801A95D8, func_801A960C,
                                  func_801A9694 };

    effectCacheSlotAnchors(entity);
    effectUpdateModelBounds(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801A9C5C, 0, 0x80);
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
static void func_801A9778(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A978C(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: carve both record banks and start the draw script. */
static void func_801A97A0(EffectEntity *entity) {
    if (entity->wait == 0) {
        /* Both banks are carved out of the same byte arena behind the TIM. */
        g_effectEmitterBank = (EffectEmitter *)D_801D35C4;
        D_801D35C4 += EFFECT_EMITTER_BANK_BYTES;
        g_effectParticleBank = (EffectParticle *)D_801D35C4;
        D_801D35C4 += EFFECT_PARTICLE_BANK_BYTES;
        effectBanksClear();
        effectSpawnTask(&D_801D3854, func_801A9278, 0x40, entity);
        entity->pc++;
    }
}

/** @brief Opcode handler: start a task running @ref func_801A969C. */
static void func_801A9830(EffectEntity *entity) {
    entity->scratch.timer.unk063 = 1;
    effectSpawnTask(&D_801D42E4, func_801A969C, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: repeat the step until the loop counter runs out. */
static void func_801A9888(EffectEntity *entity) {
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

/** @brief Opcode handler: spawn the tint fade-out once the wait runs out. */
static void func_801A98EC(EffectEntity *entity) {
    if (--entity->scratch.timer.countdown <= 0) {
        effectSpawnTask(&D_801D3854, func_801A9394, 0x40, entity);
        entity->pc++;
    }
}

/** @brief Opcode handler: stall here until @c unk05E reaches zero. */
static void func_801A9954(EffectEntity *entity) {
    if (entity->unk05E == 0) {
        entity->pc++;
    }
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A997C(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9990(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: raise @ref EFFECT_FLAG_STOP and consume the opcode. */
static void func_801A99A4(EffectEntity *entity) {
    entity->flags |= EFFECT_FLAG_STOP;
    entity->pc++;
}

/** @brief Opcode handler: no-op. */
static void func_801A99C0(EffectEntity *entity) {
}

/**
 * @brief Run one frame of the effect: its opcode, then every task pool it owns.
 *
 * @param entity The script's root entity.
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
s32 func_801A99C8(EffectEntity *entity) {
    EffectHandler handlers[11] = {
        func_801A9778, func_801A978C, func_801A97A0, func_801A9830, func_801A9888,
        func_801A98EC, func_801A9954, func_801A997C, func_801A9990, func_801A99A4,
        func_801A99C0
    };
    MATRIX *view = EFFECT_SCRATCHPAD;

    *view = D_800F02C8;
    g_effectStackTop = (u8 *)view;
    g_effectStackBase = view;
    if (entity->unk05C & EFFECT_FRAME_ODD) {
        g_effectPrimCursor = D_801D408C;
        D_801D4088 = D_801D4094;
    } else {
        g_effectPrimCursor = D_801D4090;
        D_801D4088 = D_801D4098;
    }
    effectCacheAnimSlotAnchors(entity);
    handlers[entity->pc](entity);
    g_effectParticleTotal = 0;
    entity->unk05E = 0;
    g_effectLiveTotal = 0;
    entity->unk05E += func_800B2B68(&D_801D42E4);
    entity->unk05E += func_800B2B68(&g_effectChildPool);
    entity->unk05E += func_800B2B68(&D_801D3854);
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
