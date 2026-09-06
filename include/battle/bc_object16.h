#ifndef BATTLE_BC_OBJECT16_H
#define BATTLE_BC_OBJECT16_H

#include "common.h"

/**
 * @file
 * @brief Battle particle system: the 0x58-byte primitive packets.
 */

/**
 * @brief Process one primitive packet and link its output into an ordering
 *        table.
 *
 * Dispatches on the packet's own offset list; see @ref BattleEffectPrim.
 *
 * @return The new head of the list @p a3 came from.
 */
extern s32 func_800CBC68(s32 prim, s32 a1, s32 a2, s32 a3);

/** @brief Per-frame step installed on the particle tasks. */
extern void func_800CE158(void);

#endif /* BATTLE_BC_OBJECT16_H */
