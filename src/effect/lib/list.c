/**
 * @file list.c
 * @brief The draw list itself, and the per-frame setup of the emitters on it.
 */
#include "common.h"
#include "effect.h"
#include "effect/lib/common.h"
#include "effect/lib/pose.h"
#include "effect/lib/emitteralloc.h"
#include "effect/lib/draw.h"
#include "effect/lib/spawn.h"
#include "effect/lib/particle.h"
#include "effect/lib/list.h"

/**
 * @brief Give @p particle the count @p value and put it on the tail of the draw list.
 *
 * @param node Either record kind: both start with the same three link fields,
 *             and @c unk008 is the tag the walker later dispatches on.
 */
void effectListAppend(EffectParticle *particle, s16 value) {
    EffectDrawList *list = g_effectDrawList;

    particle->unk008 = value;
    if (list->head == NULL) {
        list->head = particle;
    } else {
        EffectParticle *tail = list->tail;

        tail->next = particle;
        particle->prev = tail;
    }
    list->tail = particle;
}

/**
 * @brief Take @p particle off the draw list, mending both links.
 *
 * @param node Either record kind; see @ref effectListAppend.
 */
void effectListRemove(EffectParticle *particle) {
    EffectParticle *prev = particle->prev;
    EffectParticle *next = particle->next;

    if (prev == NULL) {
        g_effectDrawList->head = next;
    } else {
        prev->next = next;
    }
    if (next == NULL) {
        g_effectDrawList->tail = prev;
    } else {
        next->prev = prev;
    }
}

/** @brief Empty the draw list. */
void effectListClear(void) {
    g_effectDrawList->head = NULL;
    g_effectDrawList->tail = NULL;
}

/** @brief Run every record on the draw list through the step its kind wants. */
void effectListRun(void) {
    EffectParticle *particle = g_effectDrawList->head;

    while (particle != NULL) {
        /* Read signed here and unsigned where it is stepped; it is also the
           tag saying which record kind the walker is standing on. */
        switch (particle->unk008) {
        case 0:
            effectEmitterStep((EffectEmitter *)particle);
            break;
        case 1:
            effectParticleStep(particle);
            break;
        }
        particle = particle->next;
    }
}

/** @brief Set @p emitter up for this frame: turn it, place it, and reset its life. */
void effectEmitterSetup(EffectEmitter *emitter) {
    EffectPoseStep *step = g_effectDrawList->sources[emitter->unk06A];
    EffectParticle *owner = emitter->owner;
    s32 angle;

    emitter->kind = step->unk011;
    switch (step->unk01C) {
    case 0:
        effectMatrixIdentity(&emitter->mtx);
        break;
    case 1:
        effectMatrixIdentity(&emitter->mtx);
        angle = ratan2(
            g_effectDrawList->trail[1].vx - g_effectDrawList->strands[0][emitter->unk06B].vx,
            g_effectDrawList->trail[1].vz - g_effectDrawList->strands[0][emitter->unk06B].vz);
        if (angle != 0) {
            effectMatrixRotY(&emitter->mtx, angle);
        }
        break;
    case 2:
        effectMatrixIdentity(&emitter->mtx);
        angle = ratan2(
            g_effectDrawList->strands[0][emitter->unk06B].vx - g_effectDrawList->trail[1].vx,
            g_effectDrawList->strands[0][emitter->unk06B].vz - g_effectDrawList->trail[1].vz);
        if (angle != 0) {
            effectMatrixRotY(&emitter->mtx, angle);
        }
        break;
    case 3:
        emitter->mtx = owner->orient;
        break;
    case 4:
        effectMatrixIdentity(&emitter->mtx);
        angle = D_800EF2D0[g_effectDrawList->slot].facing;
        if (angle != 0) {
            effectMatrixRotY(&emitter->mtx, angle);
        }
        break;
    }
    switch (emitter->kind) {
    case 0:
        effectEmitterOnStrand(emitter);
        break;
    case 1:
        effectEmitterOnTrail(emitter);
        break;
    case 2:
        effectEmitterOnOwner(emitter);
        break;
    case 3:
        effectEmitterOnOwner(emitter);
        break;
    case 4:
        effectEmitterSlide(emitter);
        break;
    }
    emitter->life = step->unk012;
}

/** @brief Fire each of @p step's four slots that is armed. */
void effectStepFireSlots(EffectParticle *particle, EffectPoseStep *step) {
    if (step->unk028 == 1) {
        effectEmitterAlloc(particle, step->unk02C, particle->unk1D9);
    }
    if (step->unk029 == 1) {
        effectEmitterAlloc(particle, step->unk02D, particle->unk1D9);
    }
    if (step->unk02A == 1) {
        effectEmitterAlloc(particle, step->unk02E, particle->unk1D9);
    }
    if (step->unk02B == 1) {
        effectEmitterAlloc(particle, step->unk02F, particle->unk1D9);
    }
}

/** @brief Allocate an emitter for every source whose cue falls on this frame. */
void effectEmittersOnCue(void) {
    s32 source;

    for (source = 0; source < 0x10; source++) {
        EffectPoseStep *step = g_effectDrawList->sources[source];

        if (step != NULL && !((g_effectDrawList->held >> source) & 1)) {
            if (step->unk011 < 2 || step->unk011 == 4) {
                if (step->unk013 == g_effectDrawList->age) {
                    if (step->unk037 == 1) {
                        s32 i;

                        for (i = 0; i < g_effectDrawList->strandLen; i++) {
                            effectEmitterAlloc(NULL, source, i);
                        }
                    } else {
                        effectEmitterAlloc(NULL, source, 0);
                    }
                }
            }
        }
    }
}

/** @brief Push the script's current position onto the head of the trail. */
void effectTrailPush(EffectDrawScript *script) {
    EffectDrawList *list = g_effectDrawList;

    list->trail[0].vx = script->unk290 << 16;
    list->trail[0].vy = script->unk292 << 16;
    list->trail[0].vz = script->unk294 << 16;
    list->trail[5] = list->trail[0];
    list->trail[4] = list->trail[5];
    list->trail[3] = list->trail[4];
    list->trail[2] = list->trail[3];
    list->trail[1] = list->trail[2];
}
