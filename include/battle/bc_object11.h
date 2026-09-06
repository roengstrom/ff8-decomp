#ifndef BATTLE_BC_OBJECT11_H
#define BATTLE_BC_OBJECT11_H

#include "common.h"

/**
 * @file
 * @brief Battle model animation: TIM uploads, posing and OT output.
 */

/** @brief Queue the TIM at @p tim for the renderer to upload. */
void func_800BB084(u8 *tim);

/**
 * @brief Link @p slot's posed model into an ordering table.
 *
 * @param slot One 0x9C-byte battle entity record.
 * @return The new head of the list @p head came from.
 */
s32 func_800BC060(void *slot, u8 *buf, s32 mode, s32 head);

/** @brief Step @p anim by one frame. */
void func_800BC420(u8 *anim);

/** @brief Report whether @p anim can be posed at an explicit frame. */
s32 func_800BCA3C(u8 *anim, u8 *state);

/** @brief Pose @p anim at @p frame. */
void func_800BCF6C(u8 *anim, u8 *state, s16 frame);

#endif /* BATTLE_BC_OBJECT11_H */
