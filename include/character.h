#ifndef CHARACTER_H
#define CHARACTER_H

/**
 * @file character.h
 * @brief Character save data structure (stride 152 bytes).
 *
 * The character data array lives within g_gameState at offset 0x490
 * (absolute address g_characters = 0x80077808). Each of the 8 playable
 * characters (Squall=0 through Edea=7) has a 152-byte entry containing
 * HP, experience, stats, magic inventory, junction config, and status.
 *
 * Several sub-symbols reference offsets within this array:
 *   g_characters       = g_gameState + 0x490  (base, offset 0x00: HP)
 *   g_characterMagic   = g_gameState + 0x4A0  (offset 0x10: magic slots)
 *   g_characterGFs     = g_gameState + 0x4E4  (offset 0x54: GF bitfield)
 *   g_characterAbilities = g_gameState + 0x4EC  (offset 0x5C: junction abilities)
 *
 * Field layout verified against the Hyne save editor (PERSONNAGES struct):
 *   https://github.com/myst6re/hyne  (src/SaveData.h)
 *
 * @note Decomped code typically accesses these fields via raw pointer
 *       arithmetic (e.g. `*(u16 *)(base + a0 * 152 + 0x490)`) to preserve
 *       codegen matching. The struct definition here is for documentation
 *       and future use where struct access doesn't break matching.
 */

#include "common.h"

/** @brief Character IDs (index into CharacterData array). */
typedef enum {
    CHAR_SQUALL  = 0,
    CHAR_ZELL    = 1,
    CHAR_IRVINE  = 2,
    CHAR_QUISTIS = 3,
    CHAR_RINOA   = 4,
    CHAR_SELPHIE = 5,
    CHAR_SEIFER  = 6,
    CHAR_EDEA    = 7
} CharacterId;

/**
 * @brief Weapon IDs stored in CharacterData.weaponId.
 *
 * IDs 0x00-0x06 are Squall's gunblades, 0x07+ are other characters' weapons.
 */
enum WeaponId {
    /* Squall (Gunblades) */
    WEAPON_REVOLVER        = 0x00,
    WEAPON_SHEAR_TRIGGER   = 0x01,
    WEAPON_CUTTING_TRIGGER = 0x02,
    WEAPON_FLAME_SABER     = 0x03,
    WEAPON_TWIN_LANCE      = 0x04,
    WEAPON_PUNISHMENT      = 0x05,
    WEAPON_LION_HEART      = 0x06,
    /* Zell (Gloves) */
    WEAPON_METAL_KNUCKLE   = 0x07,
    WEAPON_MAVERICK        = 0x08,
    WEAPON_GAUNTLET        = 0x09,
    WEAPON_EHRGEIZ         = 0x0A,
    /* Irvine (Guns) */
    WEAPON_VALIANT         = 0x0B,
    WEAPON_ULYSSES         = 0x0C,
    WEAPON_BISMARCK        = 0x0D,
    WEAPON_EXETER          = 0x0E,
    /* Quistis (Whips) */
    WEAPON_CHAIN_WHIP      = 0x0F,
    WEAPON_SLAYING_TAIL    = 0x10,
    WEAPON_RED_SCORPION    = 0x11,
    WEAPON_SAVE_THE_QUEEN  = 0x12,
    /* Rinoa (Blaster Edge) */
    WEAPON_PINWHEEL        = 0x13,
    WEAPON_VALKYRIE        = 0x14,
    WEAPON_RISING_SUN      = 0x15,
    WEAPON_CARDINAL        = 0x16,
    WEAPON_SHOOTING_STAR   = 0x17,
    /* Selphie (Nunchaku) */
    WEAPON_FLAIL           = 0x18,
    WEAPON_MORNING_STAR    = 0x19,
    WEAPON_CRESCENT_WISH   = 0x1A,
    WEAPON_STRANGE_VISION  = 0x1B,
    /* Seifer (Gunblade) */
    WEAPON_HYPERION        = 0x1C,
    /* Edea (no upgrades) */
    WEAPON_EDEA            = 0x1D
};

/**
 * @brief Junction slot indices (CharacterData.junctions[]).
 *
 * 19 slots: stats (HP-Lck), attack element/status, defense element/status.
 */
enum JunctionType {
    JUNCTION_HP          = 0,
    JUNCTION_STR         = 1,
    JUNCTION_VIT         = 2,
    JUNCTION_MAG         = 3,
    JUNCTION_SPR         = 4,
    JUNCTION_SPD         = 5,
    JUNCTION_EVA         = 6,
    JUNCTION_HIT         = 7,
    JUNCTION_LCK         = 8,
    JUNCTION_ATK_ELEM    = 9,
    JUNCTION_ATK_STATUS  = 10,
    JUNCTION_DEF_ELEM_0  = 11,
    JUNCTION_DEF_ELEM_1  = 12,
    JUNCTION_DEF_ELEM_2  = 13,
    JUNCTION_DEF_ELEM_3  = 14,
    JUNCTION_DEF_STATUS_0 = 15,
    JUNCTION_DEF_STATUS_1 = 16,
    JUNCTION_DEF_STATUS_2 = 17,
    JUNCTION_DEF_STATUS_3 = 18,
    JUNCTION_COUNT       = 19
};

/**
 * @brief Persistent status effect flags (CharacterData.statusFlags).
 *
 * These are status effects that persist outside of battle and are
 * saved to the save file. Battle-only statuses (Haste, Slow, etc.)
 * are tracked separately.
 */
#define STATUS_KO       0x01  /**< Knocked out (dead). */
#define STATUS_POISON   0x02  /**< Poisoned. */
#define STATUS_PETRIFY  0x04  /**< Petrified. */
#define STATUS_DARKNESS 0x08  /**< Darkness (blind). */
#define STATUS_SILENCE  0x10  /**< Silenced. */
#define STATUS_BERSERK  0x20  /**< Berserk. */
#define STATUS_ZOMBIE   0x40  /**< Zombie. */

/**
 * @brief Magic spell IDs (stored in MagicSlot.magicId).
 *
 * 56 spells (1–56); 0 = empty slot.
 * Order matches the FF8 save file format (verified against Hyne).
 */
typedef enum {
    MAGIC_NONE        = 0,
    MAGIC_FIRE        = 1,
    MAGIC_FIRA        = 2,
    MAGIC_FIRAGA      = 3,
    MAGIC_BLIZZARD    = 4,
    MAGIC_BLIZZARA    = 5,
    MAGIC_BLIZZAGA    = 6,
    MAGIC_THUNDER     = 7,
    MAGIC_THUNDARA    = 8,
    MAGIC_THUNDAGA    = 9,
    MAGIC_WATER       = 10,
    MAGIC_AERO        = 11,
    MAGIC_BIO         = 12,
    MAGIC_DEMI        = 13,
    MAGIC_HOLY        = 14,
    MAGIC_FLARE       = 15,
    MAGIC_METEOR      = 16,
    MAGIC_QUAKE       = 17,
    MAGIC_TORNADO     = 18,
    MAGIC_ULTIMA      = 19,
    MAGIC_APOCALYPSE  = 20,
    MAGIC_CURE        = 21,
    MAGIC_CURA        = 22,
    MAGIC_CURAGA      = 23,
    MAGIC_LIFE        = 24,
    MAGIC_FULL_LIFE   = 25,
    MAGIC_REGEN       = 26,
    MAGIC_ESUNA       = 27,
    MAGIC_DISPEL      = 28,
    MAGIC_PROTECT     = 29,
    MAGIC_SHELL       = 30,
    MAGIC_REFLECT     = 31,
    MAGIC_AURA        = 32,
    MAGIC_DOUBLE      = 33,
    MAGIC_TRIPLE      = 34,
    MAGIC_HASTE       = 35,
    MAGIC_SLOW        = 36,
    MAGIC_STOP        = 37,
    MAGIC_BLIND       = 38,
    MAGIC_CONFUSE     = 39,
    MAGIC_SLEEP       = 40,
    MAGIC_SILENCE     = 41,
    MAGIC_BREAK       = 42,
    MAGIC_DEATH       = 43,
    MAGIC_DRAIN       = 44,
    MAGIC_PAIN        = 45,
    MAGIC_BERSERK     = 46,
    MAGIC_FLOAT       = 47,
    MAGIC_ZOMBIE      = 48,
    MAGIC_MELTDOWN    = 49,
    MAGIC_SCAN        = 50,
    MAGIC_FULL_CURE   = 51,
    MAGIC_WALL        = 52,
    MAGIC_RAPTURE     = 53,
    MAGIC_PERCENT     = 54,
    MAGIC_CATASTROPHE = 55,
    MAGIC_THE_END     = 56
} MagicId;

#define MAGIC_SLOT_COUNT 32

/**
 * @brief Magic inventory entry (2 bytes).
 *
 * 32 entries per character at offset 0x10 within CharacterData.
 * Each entry is a u16: low byte = magic spell ID, high byte = quantity.
 * func_801F7B10 clears magicId to 0 when quantity is 0.
 */
typedef struct {
    u8 magicId;   /**< +0x00: Magic spell ID (see MAGIC_* defines). */
    u8 quantity;  /**< +0x01: Number of spells stocked (0-100). */
} MagicSlot; /* 2 bytes */

/**
 * @brief Per-character save data (stride 152 = 0x98 bytes).
 *
 * Array of 8 entries at g_gameState + 0x490 (g_characters).
 * Indexed by character ID (see CharacterId enum).
 */
typedef struct {
    /* 0x00 */ u16 currentHp;          /**< Current HP. */
    /* 0x02 */ u16 maxHp;              /**< Maximum HP. */
    /* 0x04 */ u32 experience;         /**< Total experience points. */
    /* 0x08 */ u8 characterId;         /**< Character model/type ID. */
    /* 0x09 */ u8 weaponId;            /**< Current weapon ID (see WeaponId). */
    /* 0x0A */ u8 str;                 /**< Strength (VGR). */
    /* 0x0B */ u8 vit;                 /**< Vitality (DFS). */
    /* 0x0C */ u8 mag;                 /**< Magic (MGI). */
    /* 0x0D */ u8 spr;                 /**< Spirit (PSY). */
    /* 0x0E */ u8 spd;                 /**< Speed (VTS). */
    /* 0x0F */ u8 lck;                 /**< Luck (CHC). */
    /* 0x10 */ MagicSlot magic[MAGIC_SLOT_COUNT]; /**< Magic inventory (32 spell slots). */
    /* 0x50 */ u8 commands[3];         /**< Equipped battle commands. */
    /* 0x53 */ u8 padCommand;          /**< Unused command slot (padding). */
    /* 0x54 */ u8 abilities[4];        /**< Equipped character abilities. */
    /* 0x58 */ u16 junctedGfs;         /**< Bitfield of junctioned GFs (1 bit per GF). */
    /* 0x5A */ u8 unknown5A;           /**< Unknown (used). */
    /* 0x5B */ u8 alternateModel;      /**< Alternate model (SeeD/Galbadia costume). */
    /* 0x5C */ u8 junctions[19];        /**< Junction slots (indexed by JunctionType). */
    /* 0x6F */ u8 pad6F;               /**< Padding. */
    /* 0x70 */ u16 gfCompatibility[16];/**< GF compatibility values (one per GF). */
    /* 0x90 */ u16 kills;              /**< Kill count. */
    /* 0x92 */ u16 kos;                /**< KO count. */
    /* 0x94 */ u8 exists;              /**< Character exists/unlocked flag. */
    /* 0x95 */ u8 unknown95;           /**< Unknown. */
    /* 0x96 */ u16 statusFlags;        /**< Status/condition flags. */
} CharacterData; /* 0x98 = 152 bytes */

#define CHARACTER_COUNT 8
#define PARTY_SLOT_COUNT 3
#define PARTY_SLOT_EMPTY 0xFF

#endif /* CHARACTER_H */
