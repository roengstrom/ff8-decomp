/**
 * @file gf_curve.h
 * @brief Public prototypes owned by @c src/gf_curve.c.
 *
 * Stat/experience curve evaluation for characters and GFs: the quadratic
 * curve evaluator all the others build on, level<->XP inversion, and the
 * derived combat stats (HP, hit, evade, elemental and status attack/defence)
 * that the battle code reads out of @c g_gameState and @c g_kernel.
 */
#ifndef GF_CURVE_H
#define GF_CURVE_H

#include "common.h"
#include "character.h"

/** @brief Raise party member @p partySlot's stat @p stat (0 STR .. 5 LCK) by 1. */
void func_8002153C(s32 partySlot, s32 stat);

/** @brief Evaluate the shared quadratic stat curve. */
s32 evalQuadraticCurve(s32 a0, s32 a1, s32 a2);

/** @brief Evaluate an ability-growth curve for @p a1. */
s32 evalAbilityCurve(s32 a0, s32 a1);

/** @brief Invert an ability curve: the level reached at value @p a1. */
s32 findAbilityLevel(s32 a0, s32 a1);

/** @brief Evaluate a character stat curve. */
s32 evalStatCurve(s32 a0, s32 a1);

/** @brief Evaluate the XP curve for entity @p entityIdx. */
s32 evalEntityXpCurve(s32 entityIdx, s32 a1);

/** @brief Invert the character XP curve: the level for a given XP total. */
s32 findCharXpLevel(s32 a0, s32 a1);

/** @brief Add @p exp to party member @p partySlot's EXP; returns the new level. */
s32 func_8002257C(s32 partySlot, u16 exp);

/** @brief XP remaining until the next level. */
s32 getXpToNextLevel(s32 a0, s32 a1);

/** @brief Stocked quantity of @p magicId held by character @p charIdx. */
s32 getMagicQuantity(s32 charIdx, MagicId magicId);

/** @brief Saturating multiply used by the stat pipeline. */
s32 multiply(s32 a0, s32 a1);

/** @brief Multiply then scale down by 100 (percentage application). */
s32 multiplyDiv100(s32 a0, s32 a1);

/** @brief Junctioned-ability modifier for character @p charIdx. */
s32 getAbilityModifier(s32 charIdx, s32 a1);

/** @brief Character HP at @p level. */
s32 calcHpFromLevel(s32 level, s32 charIdx);

/** @brief Party member @p charIdx's stat @p kind (a JunctionType, STR..LCK) at @p level. */
s32 func_80021C10(s32 level, s32 charIdx, s32 kind);

/** @brief Hit rate stat for character @p charIdx. */
s32 calcHitStat(s32 charIdx, s32 a1);

/** @brief Evade stat for character @p charIdx. */
s32 calcEvaStat(s32 charIdx, s32 a1);

/** @brief Base elemental-attack value for character @p charIdx. */
s32 getAtkElemBase(s32 charIdx);

/** @brief Elemental-attack bonus for character @p charIdx. */
s32 getAtkElemBonus(s32 charIdx);

/** @brief Elemental resistance for the element selected by @p shiftBit. */
s32 getElemResistance(s32 charIdx, s32 shiftBit);

/** @brief Status-attack flags for character @p charIdx. */
s32 getAtkStatusFlags(s32 charIdx);

/** @brief Decode the status-attack mask for character @p charIdx. */
s32 decodeAtkStatusMask(s32 charIdx);

/** @brief Status-attack hit rate for character @p charIdx. */
s32 calcAtkStatusHit(s32 charIdx);

/** @brief Status resistance for the status selected by @p shiftBit. */
s32 getStatusResistance(s32 charIdx, s32 shiftBit);

s32 func_80021B58(s32 charIdx, s32 fallback);
s32 func_8002274C(s32 gfIdx, u16 delta);

#endif /* GF_CURVE_H */
