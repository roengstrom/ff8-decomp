#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gf.h"
#include "gamestate.h"

INCLUDE_ASM("asm/nonmatchings/gf_curve", func_8002153C);


/**
 * @brief Quadratic XP/stat curve evaluation.
 * @param a0 Level input.
 * @param a1 Linear coefficient.
 * @param a2 Quadratic coefficient.
 * @return a1 * (a0 * 10) + (a0 * a0 * a2) / 256.
 */
s32 func_80021628(s32 a0, s32 a1, s32 a2) {
    s32 sq = a0 * a0 * a2;
    s32 x = a0 * 10;
    return a1 * x + sq / 256;
}


/**
 * @brief Look up GF ability XP curve parameters and compute a value.
 * @param a0 GF index (stride 132 in g_gfData ability table).
 * @param a1 Level value passed through as first arg to func_80021628.
 * @return Result of func_80021628(a1, linearCoeff, quadDivisor).
 */
s32 func_8002166C(s32 a0, s32 a1) {
    return func_80021628(a1, g_gfData.abilityTable132[a0].xpParamB, g_gfData.abilityTable132[a0].xpParamC);
}


/**
 * @brief Find the level at which a GF ability curve reaches a given XP threshold.
 * @param a0 XP threshold to check against.
 * @param a1 GF index (stride 132 in g_gfData ability table).
 * @return Level (1–100) where the curve value first exceeds a0, or 100 if never exceeded.
 */
s32 func_800216B0(s32 a0, s32 a1) {
    s32 i = 1;
    do {
        if (a0 < func_80021628(i, g_gfData.abilityTable132[a1].xpParamB, g_gfData.abilityTable132[a1].xpParamC)) {
            return i;
        }
        i++;
    } while (i < 100);
    return i;
}


/**
 * @brief Compute an experience or stat curve value using a quadratic formula.
 * @param a0 Level or input value for the formula.
 * @param a1 Entry index into the g_gfData data table (stride 132 bytes).
 * @return Computed value as s16: a0*field0 + a0*a0*10/field1 + field2.
 * @note Uses GfAbilityTableEntry fields xpLinear (+0x12), xpQuadDiv (+0x13), xpConst (+0x14).
 */
s32 func_8002172C(s32 a0, s32 a1) {
    s32 base = (s32)&g_gfData;
    s32 entry = base + a1 * 132;
    u8 field1 = *(u8 *)(entry + 0xF8D);
    u8 field0 = *(u8 *)(entry + 0xF8C);
    u8 field2 = *(u8 *)(entry + 0xF8E);
    return (s16)(a0 * field0 + a0 * a0 * 10 / field1 + field2);
}


/**
 * @brief Compute a stat value for an entity by looking up parameters from two data tables.
 * @param a0 Entity index into g_battleChars (stride 464 bytes).
 * @param a1 Level or modifier value passed to func_80021628.
 * @return Result of func_80021628 using two u8 fields from g_gfData indexed by a secondary ID.
 * @note Reads a sub-index from g_battleChars offset 0x1C3, then looks up linearCoeff/quadDivisor in g_gfData.xpCurves36.
 */
s32 func_8002178C(s32 entityIdx, s32 a1) {
    u8 idx = g_battleChars[entityIdx].characterId;
    return func_80021628(a1, g_gfData.xpCurves36[idx].linearCoeff, g_gfData.xpCurves36[idx].quadDivisor);
}


/**
 * @brief Find the level at which a GF XP curve (via character slot) reaches a threshold.
 * @param a0 XP threshold to check against.
 * @param a1 Character slot index (stride 152 in g_gameState).
 * @return Level (1–100) where the curve value first exceeds a0, or 100 if never exceeded.
 * @note Reads a GF index from g_gameState offset 0x498, then uses g_gfData.xpCurves36 linearCoeff/quadDivisor.
 */
s32 func_800217F4(s32 a0, s32 a1) {
    s32 i = 1;
    u8 idx = g_gameState.chars[a1].characterId;
    do {
        if (a0 < func_80021628(i, g_gfData.xpCurves36[idx].linearCoeff, g_gfData.xpCurves36[idx].quadDivisor)) {
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
 * @note Similar to func_800217F4 but returns the difference between the curve value and a0,
 *       or 0 if level 100 is reached.
 */
s32 func_80021894(s32 a0, s32 a1) {
    s32 i = 1;
    u8 idx = g_gameState.chars[a1].characterId;
    s32 curveVal;
    do {
        curveVal = func_80021628(i, g_gfData.xpCurves36[idx].linearCoeff, g_gfData.xpCurves36[idx].quadDivisor);
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
s32 func_80021944(s32 charIdx, MagicId magicId) {
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
s32 func_800219A8(s32 a0, s32 a1) {
    return a0 * a1;
}


/**
 * @brief Compute (a0 * a1) / 100, using the compiler's integer division optimization.
 * @param a0 First multiplicand.
 * @param a1 Second multiplicand.
 * @return Product divided by 100 (truncated toward zero).
 */
s32 func_800219B8(s32 a0, s32 a1) {
    return a0 * a1 / 100;
}


/**
 * @brief Compute an elemental or status modifier percentage from a character's equipped abilities.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Element or status type to match against.
 * @return Base value of 100 plus any matching ability bonuses from g_gfData table.
 * @note Checks 4 ability slots (offsets 0x4E4..0x4E7). Abilities in range 0x27..0x39 are looked up
 *       in g_gfData (stride 8, offset 0x421D for type, 0x421E for bonus).
 */
s32 func_800219E0(s32 charIdx, s32 a1) {
    s32 result = 100;
    s32 i;
    for (i = 0; i < 4; i++) {
        s32 val = g_gameState.chars[charIdx].abilities[i];
        s32 idx = val - 0x27;
        if ((u32)idx < 0x13) {
            if (g_gfData.abilityRangeK[idx].typeField == a1) {
                result += g_gfData.abilityRangeK[idx].bonusField;
            }
        }
    }
    return result;
}


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_80021A64);


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_80021B58);


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_80021C10);


/**
 * @brief Compute a derived stat (likely magic power) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Modifier or stat type passed to func_80021B58.
 * @return Clamped u8 result combining a base value, a junction bonus, and a level-based lookup.
 * @note Reads GF/junction index from offset 0x4F3, looks up base value from g_gfData (stride 60),
 *       then adds a bonus from func_800219B8 scaled by junction quantity.
 */
s32 func_80022028(s32 charIdx, s32 a1) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_HIT];
    u8 val = g_gfData.junctionData[idx].magicParam;
    s32 result1 = func_80021B58(charIdx, a1);
    return func_800231B0(g_gfData.levelCurve12[result1].field07 + func_800219B8(val, func_80021944(charIdx, idx)));
}


/**
 * @brief Compute a derived stat (likely spirit/magic defense) for a character, clamped to 0-255.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @param a1 Base value that is divided by 4 before adding junction bonus.
 * @return Clamped u8 result combining (a1/4) with a junction-scaled bonus from g_gfData.
 * @note Reads GF/junction index from offset 0x4F2, looks up multiplier from g_gfData (stride 60, offset 0x239).
 */
s32 func_800220E4(s32 charIdx, s32 a1) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_EVA];
    u8 val = g_gfData.junctionData[idx].spiritParam;
    return func_800231B0((a1 >> 2) + func_800219B8(val, func_80021944(charIdx, idx)));
}


/**
 * @brief Get a base stat value for a character's junctioned GF.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return u8 value from g_gfData at stride 60, offset 0x23C for the character's GF index at slot 0x4F5.
 * @note Purpose uncertain -- appears to retrieve a GF compatibility or stat modifier base value.
 */
s32 func_8002216C(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_ELEM];
    return g_gfData.junctionData[idx].statParamA;
}


/**
 * @brief Compute a junction-scaled stat bonus for a character.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Product of a GF multiplier (from g_gfData offset 0x23D) and the junction quantity.
 * @note Reads GF index from offset 0x4F5, then computes multiplier * func_80021944(a0, idx).
 */
s32 func_800221B4(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_ELEM];
    u8 val = g_gfData.junctionData[idx].statParamB;
    return func_800219B8(val, func_80021944(charIdx, idx));
}


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_80022228);


/**
 * @brief Get the lower 7 bits of a GF's flags field for a character.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Lower 7 bits (0x7F mask) of a u16 flags field from g_gfData at offset 0x242.
 * @note Reads GF index from character slot offset 0x4F6, then looks up flags in GF data (stride 60).
 */
s32 func_80022328(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    return g_gfData.junctionData[idx].statusFlags & 0x7F;
}


/**
 * @brief Decode a GF's status immunity/attribute flags into a game-engine bitmask.
 * @param a0 Character slot index (stride 152 in g_gameState).
 * @return Bitmask with remapped flag bits from the GF's u16 flags field at g_gfData offset 0x242.
 * @note Maps source bits to result bits: 0x80->0x1, 0x100->0x4, 0x200->0x8,
 *       0x400->0x200, 0x800->0x4000, 0x1000->0x8000.
 */
s32 func_80022370(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    u16 flags = g_gfData.junctionData[idx].statusFlags;
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
 * @return 100 + junction-scaled bonus from GF index at offset 0x4F6, g_gfData offset 0x240.
 * @note Calls func_800219B8 to scale the GF multiplier by junction quantity.
 */
s32 func_80022404(s32 charIdx) {
    u8 idx = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    u8 val = g_gfData.junctionData[idx].hitParam;
    return func_800219B8(val, func_80021944(charIdx, idx)) + 100;
}


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_8002247C);


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_8002257C);


INCLUDE_ASM("asm/nonmatchings/gf_curve", func_8002274C);


