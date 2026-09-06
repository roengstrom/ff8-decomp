#ifndef BATTLE_BC_OBJECT15_H
#define BATTLE_BC_OBJECT15_H

#include "common.h"
#include "psxsdk/libgte.h"

/**
 * @file
 * @brief Battle sprite primitives.
 */

/** @brief Load the GTE matrices a prim at @p pos is emitted through. */
void func_800C96E4(SVECTOR *pos, s32 scale, s16 angle);

/**
 * @brief Emit @p prim into @p ot.
 * @return The new head of the prim buffer @p head came from.
 */
void *func_800C9E10(void *prim, u8 *ot, s32 mode, void *head);

#endif /* BATTLE_BC_OBJECT15_H */
