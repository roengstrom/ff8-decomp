#ifndef BATTLE_BC_OBJECT9_H
#define BATTLE_BC_OBJECT9_H

#include "common.h"

/**
 * @file
 * @brief Battle audio buffers and their task pool.
 */

s32 func_800B5604(u8 *a0);

void func_800B5C10(u8 *a0, s32 a1);

/** @brief Pose the world matrix @c D_800F02C8 through @c func_800B3650. */
void func_800B6A9C(void);

/**
 * @brief Allocate a task from the audio pool and clear its byte at 0xD.
 * @return The task's storage.
 */
void *func_800B853C(void *task);

#endif /* BATTLE_BC_OBJECT9_H */
