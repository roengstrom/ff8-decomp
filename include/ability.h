#ifndef ABILITY_H
#define ABILITY_H

/**
 * @file ability.h
 * @brief GF ability IDs and types.
 *
 * All GF-learnable abilities share a single ID space (0–120).
 * The ID determines what type of ability it is:
 *   - Junction (1–19): stat junction slots
 *   - Command (20–38): battle menu commands
 *   - Character (39–77): passive character abilities
 *   - Party (78–82): passive party-wide abilities
 *   - GF (83–91): GF enhancement abilities
 *   - Menu (92–120): refinement and shop abilities
 *
 * These IDs are stored in:
 *   - CharacterData.commands[3]: equipped command abilities
 *   - CharacterData.abilities[4]: equipped character/party abilities
 *   - GfSaveData.learning: currently learning ability
 *   - GfSaveData.completeAbilities[]: learned ability bitfield
 *
 * Ability names verified against the Hyne save editor (src/Data.cpp):
 *   https://github.com/myst6re/hyne
 */

/** @brief GF ability IDs (shared across all ability types). */
enum AbilityId {
    ABILITY_NONE           = 0,

    /* Junction abilities (1–19) */
    ABILITY_HP_J           = 1,
    ABILITY_STR_J          = 2,
    ABILITY_VIT_J          = 3,
    ABILITY_MAG_J          = 4,
    ABILITY_SPR_J          = 5,
    ABILITY_SPD_J          = 6,
    ABILITY_EVA_J          = 7,
    ABILITY_HIT_J          = 8,
    ABILITY_LUCK_J         = 9,
    ABILITY_ELEM_ATK_J     = 10,
    ABILITY_ST_ATK_J       = 11,
    ABILITY_ELEM_DEF_J     = 12,
    ABILITY_ST_DEF_J       = 13,
    ABILITY_ELEM_DEF_J2    = 14,
    ABILITY_ELEM_DEF_J4    = 15,
    ABILITY_ST_DEF_J2      = 16,
    ABILITY_ST_DEF_J4      = 17,
    ABILITY_ABILITY_X3     = 18,
    ABILITY_ABILITY_X4     = 19,

    /* Command abilities (20–38) */
    ABILITY_MAGIC          = 20,
    ABILITY_GF             = 21,
    ABILITY_DRAW           = 22,
    ABILITY_ITEM           = 23,
    ABILITY_COMMAND_UNK    = 24,  /**< Unknown/unused command slot. */
    ABILITY_CARD           = 25,
    ABILITY_DOOM           = 26,
    ABILITY_MAD_RUSH       = 27,
    ABILITY_TREATMENT      = 28,
    ABILITY_DEFEND         = 29,
    ABILITY_DARKSIDE       = 30,
    ABILITY_RECOVER        = 31,
    ABILITY_ABSORB         = 32,
    ABILITY_REVIVE         = 33,
    ABILITY_LV_DOWN        = 34,
    ABILITY_LV_UP          = 35,
    ABILITY_KAMIKAZE       = 36,
    ABILITY_DEVOUR         = 37,
    ABILITY_MINIMOG        = 38,

    /* Character abilities (39–77) */
    ABILITY_HP_20          = 39,
    ABILITY_HP_40          = 40,
    ABILITY_HP_80          = 41,
    ABILITY_STR_20         = 42,
    ABILITY_STR_40         = 43,
    ABILITY_STR_60         = 44,
    ABILITY_VIT_20         = 45,
    ABILITY_VIT_40         = 46,
    ABILITY_VIT_60         = 47,
    ABILITY_MAG_20         = 48,
    ABILITY_MAG_40         = 49,
    ABILITY_MAG_60         = 50,
    ABILITY_SPR_20         = 51,
    ABILITY_SPR_40         = 52,
    ABILITY_SPR_60         = 53,
    ABILITY_SPD_20         = 54,
    ABILITY_SPD_40         = 55,
    ABILITY_EVA_30         = 56,
    ABILITY_LUCK_50        = 57,
    ABILITY_MUG            = 58,
    ABILITY_MED_DATA       = 59,
    ABILITY_COUNTER        = 60,
    ABILITY_RETURN_DAMAGE  = 61,
    ABILITY_COVER          = 62,
    ABILITY_INITIATIVE     = 63,
    ABILITY_MOVE_HP_UP     = 64,
    ABILITY_HP_BONUS       = 65,
    ABILITY_STR_BONUS      = 66,
    ABILITY_VIT_BONUS      = 67,
    ABILITY_MAG_BONUS      = 68,
    ABILITY_SPR_BONUS      = 69,
    ABILITY_AUTO_PROTECT   = 70,
    ABILITY_AUTO_SHELL     = 71,
    ABILITY_AUTO_REFLECT   = 72,
    ABILITY_AUTO_HASTE     = 73,
    ABILITY_AUTO_POTION    = 74,
    ABILITY_EXPEND_X2_1    = 75,
    ABILITY_EXPEND_X3_1    = 76,
    ABILITY_RIBBON         = 77,

    /* Party abilities (78–82) */
    ABILITY_ALERT          = 78,
    ABILITY_MOVE_FIND      = 79,
    ABILITY_ENC_HALF       = 80,
    ABILITY_ENC_NONE       = 81,
    ABILITY_RARE_ITEM      = 82,

    /* GF abilities (83–91) */
    ABILITY_SUMMAG_10      = 83,
    ABILITY_SUMMAG_20      = 84,
    ABILITY_SUMMAG_30      = 85,
    ABILITY_SUMMAG_40      = 86,
    ABILITY_GFHP_10        = 87,
    ABILITY_GFHP_20        = 88,
    ABILITY_GFHP_30        = 89,
    ABILITY_GFHP_40        = 90,
    ABILITY_BOOST          = 91,

    /* Menu abilities (92–120) */
    ABILITY_HAGGLE         = 92,
    ABILITY_SELL_HIGH      = 93,
    ABILITY_FAMILIAR       = 94,
    ABILITY_CALL_SHOP      = 95,
    ABILITY_JUNK_SHOP      = 96,
    ABILITY_T_MAG_RF       = 97,
    ABILITY_I_MAG_RF       = 98,
    ABILITY_F_MAG_RF       = 99,
    ABILITY_L_MAG_RF       = 100,
    ABILITY_TIME_MAG_RF    = 101,
    ABILITY_ST_MAG_RF      = 102,
    ABILITY_SUPT_MAG_RF    = 103,
    ABILITY_FORBID_MAG_RF  = 104,
    ABILITY_RECOV_MED_RF   = 105,
    ABILITY_ST_MED_RF      = 106,
    ABILITY_AMMO_RF        = 107,
    ABILITY_TOOL_RF        = 108,
    ABILITY_FORBID_MED_RF  = 109,
    ABILITY_GFRECOV_MED_RF = 110,
    ABILITY_GFABL_MED_RF   = 111,
    ABILITY_MID_MAG_RF     = 112,
    ABILITY_HIGH_MAG_RF    = 113,
    ABILITY_MED_LV_UP      = 114,
    ABILITY_CARD_MOD       = 115
};

#define ABILITY_COUNT 116

/** @brief Ability type categories (from Hyne abilityType). */
#define ABILITY_TYPE_JUNCTION  0  /**< IDs 1–19: stat junction slots. */
#define ABILITY_TYPE_COMMAND   1  /**< IDs 20–38: battle commands. */
#define ABILITY_TYPE_CHARACTER 2  /**< IDs 39–77: passive character abilities. */
#define ABILITY_TYPE_PARTY     3  /**< IDs 78–82: passive party abilities. */
#define ABILITY_TYPE_GF        4  /**< IDs 83–91: GF enhancement abilities. */
#define ABILITY_TYPE_MENU      5  /**< IDs 92–120: refinement/shop abilities. */

#endif /* ABILITY_H */
