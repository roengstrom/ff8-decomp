/**
 * @file effect_036.c
 * @brief Confuse
 */
#include "common.h"
#include "game.h"
#include "effect.h"
#include "psxsdk/libgpu.h"
#include "effect/effect_036.h"
#include "effect/lib/particle.h"
#include "effect/lib/entity.h"
#include "effect/lib/tables.h"
#include "effect/lib/drawscript.h"
#include "effect/lib/bankclear.h"
#include "effect/lib/common.h"
#include "effect/lib/tint.h"
#include "battle.h"
#include "battle/bc_object13.h"
#include "effect_params.h"

/** @brief Where in the scratchpad the effect keeps its view matrix. */
#define EFFECT_SCRATCHPAD ((MATRIX *)0x1F8002E0)

/** @brief @ref EffectEntity::unk05C -- the frame counter's low bit picks a bank. */
#define EFFECT_FRAME_ODD 0x1

/** @brief Sentinel ending @ref D_801A9DF8: the script stops on this entry. */
#define EFFECT_TABLE_END 0x7F

static void func_801A94B4(EffectEntity *entity);
static void func_801A9508(EffectEntity *entity);
static s32 func_801A9510(EffectEntity *entity);
static s32 func_801A9810(EffectEntity *entity);
static void func_801A99B4(EffectEntity *entity);
static void func_801A95A8(EffectEntity *entity);
static void func_801A9604(EffectEntity *entity);
static void func_801A964C(EffectEntity *entity);
static s32 func_801A9654(EffectEntity *entity);
static void func_801A96F4(EffectEntity *entity);
static void func_801A974C(EffectEntity *entity);
static void func_801A9780(EffectEntity *entity);
static void func_801A9808(EffectEntity *entity);
static void func_801A998C(EffectEntity *entity);
static void func_801A99A0(EffectEntity *entity);
static void func_801A9A24(EffectEntity *entity);
static void func_801A9A7C(EffectEntity *entity);
static void func_801A9ADC(EffectEntity *entity);
static void func_801A9AF0(EffectEntity *entity);
static void func_801A9B18(EffectEntity *entity);
static void func_801A9B2C(EffectEntity *entity);
static void func_801A9B40(EffectEntity *entity);
static void func_801A9B5C(EffectEntity *entity);

/** @brief Opcode handler: play this frame's table entry, or stop at the end. */
static void func_801A94B4(EffectEntity *entity) {
    if (D_801A9DF8[entity->unk024] == EFFECT_TABLE_END) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    } else {
        D_800F02C2 = D_801A9DF8[entity->unk024];
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A9508(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher for the table player.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9510(EffectEntity *entity) {
    EffectHandler handlers[2] = { func_801A94B4, func_801A9508 };

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
static void func_801A95A8(EffectEntity *entity) {
    EffectDrawScript *script = (EffectDrawScript *)entity;

    if (entity->unk024 >= script->stopFrame) {
        /* Called with no argument: the entity is already in $a0 and stays there. */
        ((void (*)())effectDrawScriptStart)();
        effectDrawScriptRun(script);
        entity->pc++;
    }
}

/** @brief Opcode handler: hold still until the draw script finishes. */
static void func_801A9604(EffectEntity *entity) {
    if (effectDrawScriptRun((EffectDrawScript *)entity) != 0) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A964C(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: run this frame's step, draw, and retire the entity.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9654(EffectEntity *entity) {
    EffectHandler handlers[3] = { func_801A95A8, func_801A9604, func_801A964C };

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
static void func_801A96F4(EffectEntity *entity) {
    effectStartChildScript(entity, func_801A9654, &D_801B2DE0, 0, 0x2D, 0);
    entity->pc++;
}

/** @brief Opcode handler: release the model once the count reaches 30. */
static void func_801A974C(EffectEntity *entity) {
    if (entity->unk024 == 30) {
        entity->unk010->unk063 = 0;
        entity->pc++;
    }
}

/** @brief Opcode handler: hand the current part to battle, then stop. */
static void func_801A9780(EffectEntity *entity) {
    if (entity->unk024 >= 30) {
        func_800BFE1C(&entity->animSet->anims[entity->unk02A].parts[entity->unk02B]);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

/** @brief Opcode handler: no-op. */
static void func_801A9808(EffectEntity *entity) {
}

/**
 * @brief Script dispatcher: pose the model, then run this frame's opcode.
 *
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
static s32 func_801A9810(EffectEntity *entity) {
    EffectHandler handlers[4] = { func_801A96F4, func_801A974C, func_801A9780,
                                  func_801A9808 };

    effectCacheSlotAnchors(entity);
    effectUpdateModelBounds(entity);
    handlers[entity->pc](entity);
    if (entity->unk024 == 0) {
        func_800C4764(D_801A9E10, 0, 0x80);
    }
    if (entity->unk024 == 20) {
        effectSpawnTask(&D_801E2590, func_801A9510, 0x40, entity);
    }
    if (entity->unk024 == 20) {
        EffectTintScript *child = effectSpawnTask(&D_801E30F0, effectTintPlay,
                                                  sizeof(EffectTintScript), entity);
        child->steps = D_801A9E04;
    }
    if (entity->unk024 == 18) {
        func_800C2BD0(&D_800EF2D0[entity->unk02D], 4);
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
static void func_801A998C(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A99A0(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: carve both record banks. */
static void func_801A99B4(EffectEntity *entity) {
    if (entity->wait == 0) {
        /* Both banks are carved out of the same byte arena behind the TIM. */
        g_effectEmitterBank = (EffectEmitter *)D_801E2300;
        D_801E2300 += EFFECT_EMITTER_BANK_BYTES;
        g_effectParticleBank = (EffectParticle *)D_801E2300;
        D_801E2300 += EFFECT_PARTICLE_BANK_BYTES;
        effectBanksClear();
        entity->pc++;
    }
}

/** @brief Opcode handler: start a task running @ref func_801A9810. */
static void func_801A9A24(EffectEntity *entity) {
    entity->scratch.timer.unk063 = 1;
    effectSpawnTask(&D_801E3020, func_801A9810, 0x58, entity);
    entity->pc++;
}

/** @brief Opcode handler: repeat the preceding opcode until the count runs out. */
static void func_801A9A7C(EffectEntity *entity) {
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
static void func_801A9ADC(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: stall here until @c unk05E reaches zero. */
static void func_801A9AF0(EffectEntity *entity) {
    if (entity->unk05E == 0) {
        entity->pc++;
    }
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9B18(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: consume the opcode and do nothing else. */
static void func_801A9B2C(EffectEntity *entity) {
    entity->pc++;
}

/** @brief Opcode handler: raise @ref EFFECT_FLAG_STOP and consume the opcode. */
static void func_801A9B40(EffectEntity *entity) {
    entity->flags |= EFFECT_FLAG_STOP;
    entity->pc++;
}

/** @brief Opcode handler: no-op. */
static void func_801A9B5C(EffectEntity *entity) {
}

/**
 * @brief Run one frame of the effect: its opcode, then every task pool it owns.
 *
 * @param entity The script's root entity.
 * @return 2 once the script has stopped and its children have drained, 0 while
 *         it is still running.
 */
s32 func_801A9B64(EffectEntity *entity) {
    EffectHandler handlers[11] = {
        func_801A998C, func_801A99A0, func_801A99B4, func_801A9A24, func_801A9A7C,
        func_801A9ADC, func_801A9AF0, func_801A9B18, func_801A9B2C, func_801A9B40,
        func_801A9B5C
    };
    MATRIX *view = EFFECT_SCRATCHPAD;

    *view = D_800F02C8;
    g_effectStackTop = (u8 *)view;
    g_effectStackBase = view;
    if (entity->unk05C & EFFECT_FRAME_ODD) {
        g_effectPrimCursor = D_801E2DC8;
        D_801E2DC4 = D_801E2DD0;
    } else {
        g_effectPrimCursor = D_801E2DCC;
        D_801E2DC4 = D_801E2DD4;
    }
    effectCacheAnimSlotAnchors(entity);
    handlers[entity->pc](entity);
    g_effectParticleTotal = 0;
    entity->unk05E = 0;
    g_effectLiveTotal = 0;
    entity->unk05E += func_800B2B68(&D_801E3020);
    entity->unk05E += func_800B2B68(&D_801E30F0);
    entity->unk05E += func_800B2B68(&g_effectChildPool);
    entity->unk05E += func_800B2B68(&D_801E2590);
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
