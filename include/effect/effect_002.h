#ifndef EFFECT_EFFECT_002_H
#define EFFECT_EFFECT_002_H

#include "effect.h"

/**
 * @file
 * @brief Fire: the overlay body.
 */

/** @brief The frame data the effect alternates between two halves of. */
extern u8 D_801C4254[];

/** @brief Which half the frame being built reads from. */
extern u8 *D_801D8254;

/** @brief The animation the flame is drawn from. */
extern BattleSpriteAnim D_801C2AAC;

/** @brief The animation the flame's sparks are drawn from. */
extern BattleSpriteAnim D_801C2824;

/** @brief The animations a burst of the flame and the glow beneath it are drawn from. */
extern BattleSpriteAnim D_801C24CC;
extern BattleSpriteAnim D_801C2678;

/** @brief Pool the effect's children run in, and its storage. */
extern s32 D_801C4244;
extern s32 D_801C2D24;

/** @brief Pool the effect's frame task runs in, and its storage. */
extern s32 D_801C2D14;
extern s32 D_801C2D04;

/** @brief The animation set the effect was started with. */
extern EffectAnimSet *D_801C2CF4;

/** @brief Which slot cast the effect. */
extern s32 D_801C2CFC;

/** @brief One word per battle slot, raised while the flame covers it. */
extern s32 D_801C2CD8[];

/** @brief The sound the effect's cue plays when it lands. */
extern u8 D_801C29D0[];

/** @brief The data carried in the overlay image behind the code. */
extern u8 D_801A1EAC;

/** @brief Table of offsets battle.bin resolves when the effect starts. */
extern u8 D_801A187C;

#endif /* EFFECT_EFFECT_002_H */
