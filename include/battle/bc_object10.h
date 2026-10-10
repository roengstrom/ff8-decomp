#ifndef BATTLE_BC_OBJECT10_H
#define BATTLE_BC_OBJECT10_H


/**
* @file
* @brief Battle audio list structures.
*/
SoundCmd* func_800B8564(s16, u8);

/** @brief Initialize a linked-list array of @p count entries at @p a0. */
void func_800B8870(u8 *a0, s32 count);

/** @brief Allocate a handler for @c func_800B8BEC and store its parameters. */
void func_800B8F4C(s32 a0);

#endif /* BATTLE_BC_OBJECT10_H */
