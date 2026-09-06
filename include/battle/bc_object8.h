#ifndef BATTLE_BC_OBJECT8_H
#define BATTLE_BC_OBJECT8_H

#include "common.h"
#include "psxsdk/libgte.h"

/**
 * @file
 * @brief Battle task pools and the scratchpad allocator.
 */

/** @brief Build a task pool of @p count entries of @p stride bytes. */
void func_800B2A00(void *header, void *data, s32 stride, s32 count);

/**
 * @brief Take one task off @p pool and install @p task as its per-frame step.
 * @return The task's storage, or NULL if the pool is full.
 */
void *func_800B2A84(void *pool, void *task);

/**
 * @brief Run one frame of every task in @p pool.
 * @return The number of tasks still live afterwards.
 */
s32 func_800B2B68(void *pool);

/** @brief Take @p size bytes off the scratchpad stack. */
void *func_800B3698(s32 size);

/**
 * @brief Take one task off the shared pool and install @p task on it.
 * @return The task's storage, or NULL if the pool is full.
 */
void *func_800B2C58(void *task);

/** @brief Submit @p m to the GTE as the pose for the next model. */
void func_800B3650(MATRIX *m);

/**
 * @brief Read the point @p id out of @p slot's model into @p out.
 *
 * @param slot One 0x9C-byte battle entity record; the effect overlays hold a
 *             struct view of it, battle indexes @c D_800EF2D0 by stride.
 */
void func_800B3960(void *slot, s32 id, s32 arg2, SVECTOR *out);

#endif /* BATTLE_BC_OBJECT8_H */
