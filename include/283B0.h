#ifndef UNIT_283B0_H
#define UNIT_283B0_H

#include "common.h"

/** @brief Resolve a character ID (e.g. party slot) to its global character code. */
extern s32 func_80037C6C(s32 charId);

/** @brief Stop all sound playback; installed as the VSync callback during field-engine init. */
extern void stopAllSounds(void);

extern void func_80037D40(void);

/** @brief Toggle the current sound-bank-selector flag and return a pointer to the new bank table. */
extern u8 *toggleSoundBank(void);

extern void func_80037FB0(s32 variant, s32 bank, u8 *musicBuffer);
extern void func_80038030(u8 *musicBuffer);
extern void clearEntityFlags(void);

/** @brief Read the 2-bit packed flag at the given key (256-entry table). */
extern s32 getPackedField2Bit(s32 key);

/** @brief Update one packed-flag table slot from a step tick. */
extern void func_800383B8(s32 key, s32 status);

/** @brief Field-engine PRNG; consumed by random encounter / step ticks. */
extern s32 fieldRandom(void);

#endif
