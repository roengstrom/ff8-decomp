#ifndef GAME_H
#define GAME_H

#include "common.h"

/** @brief Game-code per-frame VSync handler (dispatched for RENDER_GAME). */
void vsyncGameHandler(void);

/** @brief Main game state-machine loop, driven by g_vsyncRate. */
void gameStateLoop(void);

s32 isBossBattle(void);
s32 hasJunctionedAbility(s32 partySlot, s32 abilityId);

u8 *getBattleCommandName(s32 id);

/** @brief Look up entry @p stringId of the kernel's misc text table. */
u8 *getMenuString(s32 stringId);

/** @brief Name of battle party member @p entityIdx (Squall's and Rinoa's are the player's). */
u8 *getBattleCharName(s32 entityIdx);

/** @brief Look up the description of magic spell @p spellId. */
u8 *getSpellDesc(s32 spellId);

/** @brief Look up the name of item @p itemId. */
u8 *getItemName(s32 itemId);
u8 *getItemDesc(s32 itemId);

u8 *getRinoaLimitBreak2Name(s32 id);
u8 *getDuelName(s32 id);
u8 *getShotName(s32 id);
u8 *getRenzokukenFinisherName(s32 id);
u8 *getBlueMagicName(s32 id);
u8 *getTempLimitBreakName(s32 id);

u8 *getAbilityName(s32 abilityId);
u8 *getWeaponName(s32 id);
u8 *getAbilityDesc(s32 abilityId);
u8 *getBattleCommandDesc(s32 id);

/** @brief Zero @p count 16-byte units starting at @p ptr. */
void memzero16(s32 *ptr, s32 count);

#endif /* GAME_H */
