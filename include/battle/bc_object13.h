#ifndef BATTLE_BC_OBJECT13_H
#define BATTLE_BC_OBJECT13_H

#include "common.h"

/**
 * @file
 * @brief Battle sound sequences.
 */

/** @brief Publish the two tables inside @p base for the sequencer to read. */
void func_800C3BE0(u8 *base);

/** @brief Queue the AKAO sequence at @p sequence for playback. */
void func_800C4764(u8 *sequence, s32 arg1, s32 arg2);

#endif /* BATTLE_BC_OBJECT13_H */
