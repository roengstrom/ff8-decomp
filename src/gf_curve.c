#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gf.h"
#include "gamestate.h"
#include "gf_curve.h"
#include "game.h"

/**
 * @brief Raise one of a party member's stats by 1, capped at 255.
 * @param partySlot Party slot (0-2).
 * @param stat 0 STR, 1 VIT, 2 MAG, 3 SPR, 4 SPD, 5 LCK.
 */
void func_8002153C(s32 partySlot, s32 stat) {
    CharacterData *cd = &g_gameState.chars[g_gameState.mainData.party.party[partySlot]];

    switch (stat) {
    case 0:
        cd->str = clampToByte(cd->str + 1);
        break;
    case 1:
        cd->vit = clampToByte(cd->vit + 1);
        break;
    case 2:
        cd->mag = clampToByte(cd->mag + 1);
        break;
    case 3:
        cd->spr = clampToByte(cd->spr + 1);
        break;
    case 4:
        cd->spd = clampToByte(cd->spd + 1);
        break;
    case 5:
        cd->lck = clampToByte(cd->lck + 1);
        break;
    }
}


/**
 * @brief Quadratic XP/stat curve evaluation.
 * @param a0 Level input.
 * @param a1 Linear coefficient.
 * @param a2 Quadratic coefficient.
 * @return a1 * (a0 * 10) + (a0 * a0 * a2) / 256.
 */
s32 evalQuadraticCurve(s32 a0, s32 a1, s32 a2) {
    s32 sq = a0 * a0 * a2;
    s32 x = a0 * 10;
    return a1 * x + sq / 256;
}


/**
 * @brief Look up GF ability XP curve parameters and compute a value.
 * @param a0 GF index (stride 132 in g_kernel ability table).
 * @param a1 Level value passed through as first arg to evalQuadraticCurve.
 * @return Result of evalQuadraticCurve(a1, linearCoeff, quadDivisor).
 */
s32 evalAbilityCurve(s32 a0, s32 a1) {
    return evalQuadraticCurve(a1, g_kernel.junctionableGfs[a0].xpParamB, g_kernel.junctionableGfs[a0].xpParamC);
}


/**
 * @brief Find the level at which a GF ability curve reaches a given XP threshold.
 * @param a0 XP threshold to check against.
 * @param a1 GF index (stride 132 in g_kernel ability table).
 * @return Level (1–100) where the curve value first exceeds a0, or 100 if never exceeded.
 */
s32 findAbilityLevel(s32 a0, s32 a1) {
    s32 i = 1;
    do {
        if (a0 < evalQuadraticCurve(i, g_kernel.junctionableGfs[a1].xpParamB, g_kernel.junctionableGfs[a1].xpParamC)) {
            return i;
        }
        i++;
    } while (i < 100);
    return i;
}


/**
 * @brief Compute an experience or stat curve value using a quadratic formula.
 * @param a0 Level or input value for the formula.
 * @param a1 Entry index into the g_kernel data table (stride 132 bytes).
 * @return Computed value as s16: a0*field0 + a0*a0*10/field1 + field2.
 */
s32 evalStatCurve(s32 a0, s32 a1) {
    u8 field1 = g_kernel.junctionableGfs[a1].xpQuadDiv;
    u8 field0 = g_kernel.junctionableGfs[a1].xpLinear;
    u8 field2 = g_kernel.junctionableGfs[a1].xpConst;
    return (s16)(a0 * field0 + a0 * a0 * 10 / field1 + field2);
}


/**
 * @brief Compute a stat value for an entity by looking up parameters from two data tables.
 * @param a0 Entity index into g_battleChars (stride 464 bytes).
 * @param a1 Level or modifier value passed to evalQuadraticCurve.
 * @return Result of evalQuadraticCurve using two u8 fields from g_kernel indexed by a secondary ID.
 * @note Reads a sub-index from g_battleChars offset 0x1C3, then looks up linearCoeff/quadDivisor in g_kernel.characters.
 */
s32 evalEntityXpCurve(s32 entityIdx, s32 a1) {
    u8 idx = g_battleChars.chars[entityIdx].characterId;
    return evalQuadraticCurve(a1, g_kernel.characters[idx].linearCoeff, g_kernel.characters[idx].quadDivisor);
}


/**
 * @brief Find the level at which a GF XP curve (via character slot) reaches a threshold.
 * @param a0 XP threshold to check against.
 * @param a1 Character slot index (stride 152 in g_gameState).
 * @return Level (1–100) where the curve value first exceeds a0, or 100 if never exceeded.
 * @note Reads a GF index from g_gameState offset 0x498, then uses g_kernel.characters linearCoeff/quadDivisor.
 */
s32 findCharXpLevel(s32 a0, s32 a1) {
    s32 i = 1;
    u8 idx = g_gameState.chars[a1].characterId;
    do {
        if (a0 < evalQuadraticCurve(i, g_kernel.characters[idx].linearCoeff, g_kernel.characters[idx].quadDivisor)) {
            return i;
        }
        i++;
    } while (i < 100);
    return i;
}


/**
 * @brief Compute an XP-to-next-level value for a GF, via a character slot lookup.
 * @param a0 Current XP value.
 * @param a1 Character slot index (stride 152 in g_gameState).
 * @return XP needed to reach the next level, or 0 if at max level (100).
 * @note Similar to findCharXpLevel but returns the difference between the curve value and a0,
 *       or 0 if level 100 is reached.
 */
s32 getXpToNextLevel(s32 a0, s32 a1) {
    s32 i = 1;
    u8 idx = g_gameState.chars[a1].characterId;
    s32 curveVal;
    do {
        curveVal = evalQuadraticCurve(i, g_kernel.characters[idx].linearCoeff, g_kernel.characters[idx].quadDivisor);
        if (a0 < curveVal) {
            break;
        }
        i++;
    } while (i < 100);
    if (i == 100) {
        return 0;
    }
    return curveVal - a0;
}


/**
 * @brief Search a character's junction list for a specific junction ID and return its value.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Junction ID to search for; returns 0 immediately if a1 is 0.
 * @return The junction value (byte at offset +1 from matching entry), or 0 if not found.
 * @note Scans up to 32 entries (2 bytes each: ID at +0x4A0, value at +0x4A1).
 */
s32 getMagicQuantity(s32 charIdx, MagicId magicId) {
    s32 i;

    if (magicId == MAGIC_NONE) return 0;

    for (i = 0; i < MAGIC_SLOT_COUNT; i++) {
        if (g_gameState.chars[charIdx].magic[i].magicId == magicId) {
            return g_gameState.chars[charIdx].magic[i].quantity;
        }
    }
    return 0;
}


/**
 * @brief Multiply two signed 32-bit integers.
 * @param a0 First operand.
 * @param a1 Second operand.
 * @return Product of a0 and a1.
 */
s32 multiply(s32 a0, s32 a1) {
    return a0 * a1;
}


/**
 * @brief Compute (a0 * a1) / 100, using the compiler's integer division optimization.
 * @param a0 First multiplicand.
 * @param a1 Second multiplicand.
 * @return Product divided by 100 (truncated toward zero).
 */
s32 multiplyDiv100(s32 a0, s32 a1) {
    return a0 * a1 / 100;
}


/**
 * @brief Compute an elemental or status modifier percentage from a character's equipped abilities.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Element or status type to match against.
 * @return Base value of 100 plus any matching ability bonuses from g_kernel table.
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x27..0x39 are looked up
 *       in g_kernel (stride 8, offset 0x421D for type, 0x421E for bonus).
 */
s32 getAbilityModifier(s32 charIdx, s32 a1) {
    s32 result = 100;
    s32 i;
    for (i = 0; i < 4; i++) {
        s32 val = g_gameState.chars[charIdx].abilities[i];
        s32 idx = val - 0x27;
        if ((u32)idx < 0x13) {
            if (g_kernel.statPercentAbilities[idx].typeField == a1) {
                result += g_kernel.statPercentAbilities[idx].bonusField;
            }
        }
    }
    return result;
}



/**
* @brief Compute a character's base HP from level, class curve, and HP junction bonus.
* @param level Character level.
* @param charIdx Character slot index (stride 152 in g_gameState).
* @return HP value computed as: level*coef - level²×10/div + base + maxHp + junctionBonus.
*/
s32 calcHpFromLevel(s32 level, s32 charIdx) {
    u8 hpJunc;
    u8 charId;
    s32 count;
    s32 juncMult;
    u8 _div;
    u8 coef;
    u8 addBase;
    u16 maxHp;
    s32 result;

    hpJunc = g_gameState.chars[charIdx].junctions[0];
    charId = g_gameState.chars[charIdx].characterId;
    count = getMagicQuantity(charIdx, hpJunc);
    juncMult = multiply(g_kernel.magic[hpJunc].statJunction[JUNCTION_HP], count);
    _div = g_kernel.characters[charId].hpCurve[1];
    coef = g_kernel.characters[charId].hpCurve[0];
    addBase = g_kernel.characters[charId].hpCurve[2];
    maxHp = g_gameState.chars[charIdx].maxHp;
    result = level * coef - (level * level * 10) / _div + addBase + maxHp + juncMult;
    return result;
}


/**
 * @brief Resolve GF index for a character (used by calcHitStat).
 * @param charIdx Character slot index (stride 152 in g_gameState).
 * @param fallback Fallback value returned when characterId is not 8, 9, or 10 in locked mode.
 * @return GF index from PartyData if party is locked and characterId is 8/9/10,
 *         weaponId if party is unlocked, or fallback otherwise.
 * @note When @ref PARTY_LOCK_LOCKED is set, maps characterId 8->gfIndex0, 9->gfIndex1, 10->gfIndex2.
 *       When party is unlocked, returns the character's weaponId.
 */
s32 func_80021B58(s32 charIdx, s32 fallback) {
    s32 val;
    s32 charId;
    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        charId = g_gameState.chars[charIdx].characterId;
        val = fallback;
        switch (charId) {
            case 8:
                val = g_gameState.mainData.party.gfIndex0;
                break;
            case 9:
                val = g_gameState.mainData.party.gfIndex1;
                break;
            case 10:
                val = g_gameState.mainData.party.gfIndex2;
                break;
        }
    } else {
        val = g_gameState.chars[charIdx].weaponId;
    }
    return val;
}


/**
 * @brief A party member's STR, VIT, MAG, SPR, SPD or LCK before the percentage abilities.
 *
 * The stat's growth curve at @p level, plus the character's own bonus to the
 * stat, the junctioned spell's value scaled by its stock and, for STR, the
 * weapon's STR bonus. SPD and LCK grow linearly; the other four with a
 * quadratic term, a quarter as fast.
 *
 * @param level Character level.
 * @param charIdx Character index into g_gameState.chars.
 * @param kind JUNCTION_STR..JUNCTION_SPD or JUNCTION_LCK.
 * @return The stat, clamped to 255.
 */
s32 func_80021C10(s32 level, s32 charIdx, s32 kind) {
    u8 charId = g_gameState.chars[charIdx].characterId;
    u8 *curve;
    s32 magicId;
    s32 jValue;
    s32 bonus;
    s32 weaponStr = 0;

    switch (kind) {
    case JUNCTION_STR:
        curve = g_kernel.characters[charId].strCurve;
        magicId = g_gameState.chars[charIdx].junctions[JUNCTION_STR];
        bonus = g_gameState.chars[charIdx].str;
        jValue = g_kernel.magic[magicId].statJunction[JUNCTION_STR];
        /* The original passes no fallback, so func_80021B58 reads whatever is left in a1. */
        weaponStr = g_kernel.weapons[((s32 (*)())func_80021B58)(charIdx)].strBonus;
        break;
    case JUNCTION_VIT:
        curve = g_kernel.characters[charId].vitCurve;
        magicId = g_gameState.chars[charIdx].junctions[JUNCTION_VIT];
        bonus = g_gameState.chars[charIdx].vit;
        jValue = g_kernel.magic[magicId].statJunction[JUNCTION_VIT];
        break;
    case JUNCTION_MAG:
        curve = g_kernel.characters[charId].magCurve;
        magicId = g_gameState.chars[charIdx].junctions[JUNCTION_MAG];
        bonus = g_gameState.chars[charIdx].mag;
        jValue = g_kernel.magic[magicId].statJunction[JUNCTION_MAG];
        break;
    case JUNCTION_SPR:
        curve = g_kernel.characters[charId].sprCurve;
        magicId = g_gameState.chars[charIdx].junctions[JUNCTION_SPR];
        bonus = g_gameState.chars[charIdx].spr;
        jValue = g_kernel.magic[magicId].statJunction[JUNCTION_SPR];
        break;
    case JUNCTION_SPD:
        curve = g_kernel.characters[charId].spdCurve;
        magicId = g_gameState.chars[charIdx].junctions[JUNCTION_SPD];
        bonus = g_gameState.chars[charIdx].spd;
        jValue = g_kernel.magic[magicId].statJunction[JUNCTION_SPD];
        break;
    case JUNCTION_LCK:
        curve = g_kernel.characters[charId].lckCurve;
        magicId = g_gameState.chars[charIdx].junctions[JUNCTION_LCK];
        bonus = g_gameState.chars[charIdx].lck;
        jValue = g_kernel.magic[magicId].statJunction[JUNCTION_LCK];
        break;
    }
    if (kind == JUNCTION_SPD || kind == JUNCTION_LCK) {
        return clampToByte(level * curve[0] + level / curve[1] + curve[2] - level / curve[3] + bonus + multiplyDiv100(jValue, getMagicQuantity(charIdx, magicId)) + weaponStr);
    }
    return clampToByte((level * curve[0] / 10 + level / curve[1] + curve[2] - level * level / curve[3] / 2) / 4 + bonus + multiplyDiv100(jValue, getMagicQuantity(charIdx, magicId)) + weaponStr);
}


/**
 * @brief Compute a derived stat (likely magic power) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Modifier or stat type passed to func_80021B58.
 * @return Clamped u8 result combining a base value, a junction bonus, and a level-based lookup.
 * @note Reads GF/junction index from offset 0x4F3, looks up base value from g_kernel (stride 60),
 *       then adds a bonus from multiplyDiv100 scaled by junction quantity.
 */
s32 calcHitStat(s32 charIdx, s32 a1) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_HIT];
    u8 val = g_kernel.magic[idx].statJunction[JUNCTION_HIT];
    s32 result1 = func_80021B58(charIdx, a1);
    return clampToByte(g_kernel.weapons[result1].hitRate + multiplyDiv100(val, getMagicQuantity(charIdx, idx)));
}


/**
 * @brief Compute a derived stat (likely spirit/magic defense) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Base value that is divided by 4 before adding junction bonus.
 * @return Clamped u8 result combining (a1/4) with a junction-scaled bonus from g_kernel.
 * @note Reads GF/junction index from offset 0x4F2, looks up multiplier from g_kernel (stride 60, offset 0x239).
 */
s32 calcEvaStat(s32 charIdx, s32 a1) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_EVA];
    u8 val = g_kernel.magic[idx].statJunction[JUNCTION_EVA];
    return clampToByte((a1 >> 2) + multiplyDiv100(val, getMagicQuantity(charIdx, idx)));
}


/**
 * @brief Get a base stat value for a character's junctioned GF.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return u8 value from g_kernel at stride 60, offset 0x23C for the character's GF index at slot 0x4F5.
 * @note Purpose uncertain -- appears to retrieve a GF compatibility or stat modifier base value.
 */
s32 getAtkElemBase(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_ELEM];
    return g_kernel.magic[idx].atkElement;
}


/**
 * @brief Compute a junction-scaled stat bonus for a character.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Product of a GF multiplier (from g_kernel offset 0x23D) and the junction quantity.
 * @note Reads GF index from offset 0x4F5, then computes multiplier * getMagicQuantity(a0, idx).
 */
s32 getAtkElemBonus(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_ELEM];
    u8 val = g_kernel.magic[idx].atkElementValue;
    return multiplyDiv100(val, getMagicQuantity(charIdx, idx));
}


/**
 * @brief Compute element defense resistance for a character.
 * @param charIdx Character slot index (stride 152 in g_gameState).
 * @param shiftBit Element type bit index to check (0-7 for 8 elements).
 * @return Element resistance percentage (capped at 1000).
 */
s32 getElemResistance(s32 charIdx, s32 shiftBit) {
    s32 result = 0;
    s32 i;
    for (i = 0; i < 4; i++) {
        u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_DEF_ELEM_0 + i];
        if ((g_kernel.magic[idx].defElement >> shiftBit) & 1) {
            u8 mult = g_kernel.magic[idx].defElementValue;
            result += multiplyDiv100(mult, getMagicQuantity(charIdx, idx));
        }
    }
    result += 800;
    if (result > 1000) result = 1000;
    return result;
}


/**
 * @brief Get the lower 7 bits of a GF's flags field for a character.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Lower 7 bits (0x7F mask) of a u16 flags field from g_kernel at offset 0x242.
 * @note Reads GF index from character slot offset 0x4F6, then looks up flags in GF data (stride 60).
 */
s32 getAtkStatusFlags(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    return g_kernel.magic[idx].atkStatuses & 0x7F;
}


/**
 * @brief Decode a GF's status immunity/attribute flags into a game-engine bitmask.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Bitmask with remapped flag bits from the GF's u16 flags field at g_kernel offset 0x242.
 * @note Maps source bits to result bits: 0x80->0x1, 0x100->0x4, 0x200->0x8,
 *       0x400->0x200, 0x800->0x4000, 0x1000->0x8000.
 */
s32 decodeAtkStatusMask(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    u16 flags = g_kernel.magic[idx].atkStatuses;
    s32 val;
    s32 result;
    val = flags & 0x80;
    result = val != 0;
    if (flags & 0x100) result |= 0x4;
    if (flags & 0x200) result |= 0x8;
    if (flags & 0x400) result |= 0x200;
    if (flags & 0x800) result |= 0x4000;
    if (flags & 0x1000) result |= 0x8000;
    return result;
}


/**
 * @brief Compute a hit/accuracy percentage for a character, starting at 100 and adding a junction bonus.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return 100 + junction-scaled bonus from GF index at offset 0x4F6, g_kernel offset 0x240.
 * @note Calls multiplyDiv100 to scale the GF multiplier by junction quantity.
 */
s32 calcAtkStatusHit(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    u8 val = g_kernel.magic[idx].atkStatusValue;
    return multiplyDiv100(val, getMagicQuantity(charIdx, idx)) + 100;
}


/**
 * @brief Compute status defense resistance for a character.
 * @param charIdx Character slot index (stride 152 in g_gameState).
 * @param shiftBit Status type bit index to check (0-12 for 13 statuses).
 * @return Status resistance percentage (capped at 200).
 */
s32 getStatusResistance(s32 charIdx, s32 shiftBit) {
    s32 result = 0;
    s32 i;
    for (i = 0; i < 4; i++) {
        u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_DEF_STATUS_0 + i];
        if ((g_kernel.magic[idx].defStatuses >> shiftBit) & 1) {
            u8 base = g_kernel.magic[idx].defStatusValue;
            result += multiplyDiv100(base, getMagicQuantity(charIdx, idx));
        }
    }
    result += 100;
    if (result > 200) result = 200;
    return result;
}


/**
 * @brief Add battle EXP to a party member and apply the level-up bonuses.
 *
 * At level 100 the EXP is set to exactly what level 100 needs. For each level
 * gained, HP Bonus adds 30 max HP and Str/Vit/Mag/Spr Bonus raise their stat by 1.
 *
 * @param partySlot Party slot (0-2).
 * @param exp EXP to add.
 * @return The new level, or PARTY_SLOT_EMPTY for an empty slot.
 */
s32 func_8002257C(s32 partySlot, u16 exp) {
    s32 charIdx = g_gameState.mainData.party.party[partySlot];
    CharacterData *cd;
    BattleCharData *bc;
    s32 oldLevel;
    s32 level;
    s32 i;

    if (charIdx == PARTY_SLOT_EMPTY) {
        return PARTY_SLOT_EMPTY;
    }
    cd = &g_gameState.chars[charIdx];
    bc = &g_battleChars.chars[partySlot];
    oldLevel = findCharXpLevel(cd->experience, charIdx);
    cd->experience += exp;
    bc->exp = cd->experience;
    level = findCharXpLevel(cd->experience, charIdx);
    if (level >= 100) {
        level = 100;
        cd->experience = evalQuadraticCurve(99, g_kernel.characters[charIdx].linearCoeff, g_kernel.characters[charIdx].quadDivisor);
        bc->exp = cd->experience;
    }
    if (oldLevel != 100) {
        for (i = 0; i < level - oldLevel; i++) {
            if (bc->statusFlags & CHAR_ABILITY_HP_BONUS) {
                addCharMaxHp(partySlot, 30);
            }
            if (bc->statusFlags & CHAR_ABILITY_STR_BONUS) {
                func_8002153C(partySlot, 0);
            }
            if (bc->statusFlags & CHAR_ABILITY_VIT_BONUS) {
                func_8002153C(partySlot, 1);
            }
            if (bc->statusFlags & CHAR_ABILITY_MAG_BONUS) {
                func_8002153C(partySlot, 2);
            }
            if (bc->statusFlags & CHAR_ABILITY_SPR_BONUS) {
                func_8002153C(partySlot, 3);
            }
        }
    }
    return level;
}


/**
 * @brief Add XP to a GF ability and return the new ability level.
 * @param gfIdx GF index into g_gameState.gfs.
 * @param delta XP amount to add (only lower 16 bits used).
 * @return New ability level, capped at 100.
 */
s32 func_8002274C(s32 gfIdx, u16 delta) {
    GfSaveData *gf = &g_gameState.gfs[gfIdx];
    s32 level = findAbilityLevel(gf->exp += (delta & 0xFFFF), gfIdx);
    if (level >= 100) {
        level = 100;
        gf->exp = evalQuadraticCurve(99, g_kernel.junctionableGfs[gfIdx].xpParamB,
                                     g_kernel.junctionableGfs[gfIdx].xpParamC);
    }
    return level;
}
