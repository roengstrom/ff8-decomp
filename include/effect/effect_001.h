#ifndef EFFECT_EFFECT_001_H
#define EFFECT_EFFECT_001_H

#include "effect.h"

/**
 * @brief Entry point of the effect_001 overlay.
 *
 * battle.bin calls this at the overlay's load address once the image is in
 * place; it builds the effect's root entity and its task pools.
 *
 * @param animSet Animation table the effect draws its models from.
 * @return The task pool the effect's scripts run out of.
 */
void *func_801A0000(EffectAnimSet *animSet);

#endif /* EFFECT_EFFECT_001_H */
