#ifndef GAME_H
#define GAME_H

#include "common.h"

/** @brief Game-code per-frame VSync handler (dispatched for RENDER_GAME). */
void vsyncGameHandler(void);

/** @brief Main game state-machine loop, driven by g_vsyncRate. */
void gameStateLoop(void);

/** @brief Look up entry @p stringId of the kernel string table. */
u8 *getMenuString(s32 stringId);

/** @brief Look up the name string for stat/command @p statId. */
u8 *getStatName(s32 statId);

u8 *getAbilityName(s32 abilityId);
s32 getLevelCurveData(s32 curveId);
u8 *getAbilityDesc(s32 abilityId);
s32 getAbilityEntryDesc(s32 entryId);

/** @brief Zero @p count 16-byte units starting at @p ptr. */
void memzero16(s32 *ptr, s32 count);

#endif /* GAME_H */
