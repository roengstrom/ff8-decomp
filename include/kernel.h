#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"

// kernel.bin, loaded whole at g_kernel. Section layout per the FF8 Modding Wiki
// (technical-reference/main/kernel/header); offsets match the disc's kernel.bin.

/** @brief targetInfo bit: one unit of the chosen side. */
#define TARGET_INFO_SINGLE 0x10

/** @brief attackFlags bits 0-1: the damage type. */
#define ATTACK_DAMAGE_TYPE 0x03

/** @brief attackFlags bits. */
#define ATTACK_FLAG_SELECTABLE 0x20  /**< Battle items only: clear greys the item out. */
#define ATTACK_FLAG_TARGET_KO  0x80  /**< May target KO'd units. */

/** @brief Ability entry (8 bytes). */
typedef struct {
    u16 statParam0;   /**< +0x00: Name text offset. */
    u16 statParam1;   /**< +0x02: Description text offset. */
    u8 cap;           /**< +0x04: Ability cap value (checked by GetAbilityCap). */
    u8 typeField;     /**< +0x05: Command type ID or element/status type. */
    u8 bonusField;    /**< +0x06: Bonus value or GF field B value. */
    u8 extraField;    /**< +0x07: Status immunity byte, party flag, or mode selector. */
} AbilityEntry; /* 8 bytes */

/** @brief Battle command entry (8 bytes). */
typedef struct {
    u16 nameOffset;     /**< +0x00: Name text offset. */
    u16 descOffset;     /**< +0x02: Description text offset. */
    u8 abilityDataId;   /**< +0x04 */
    u8 menuFlags;       /**< +0x05 */
    u8 targetInfo;      /**< +0x06 */
    u8 pad07;
} BattleCommandEntry; /* 8 bytes */

/** @brief Battle command IDs, indexing Kernel.battleCommands. */
#define BATTLE_CMD_CAST  9
#define BATTLE_CMD_STOCK 10
#define BATTLE_CMD_SHOT  14

/** @brief Magic entry (60 bytes). */
typedef struct {
    u16 nameOffset;          /**< +0x00: Name text offset. */
    u16 descOffset;          /**< +0x02: Description text offset. */
    u16 animation;           /**< +0x04: Attack animation (a.k.a. magic ID). */
    u8 hitAnimation;         /**< +0x06: The target's hit reaction animation. */
    u8 attackType;           /**< +0x07 */
    u8 power;                /**< +0x08: Spell power, used in the damage formula. */
    u8 statusWindowFlags;    /**< +0x09 */
    u8 targetInfo;           /**< +0x0A */
    u8 attackFlags;          /**< +0x0B */
    u8 drawResist;           /**< +0x0C: How hard the spell is to draw. */
    u8 hitCount;             /**< +0x0D */
    u8 element;              /**< +0x0E */
    u8 pad0F;
    u32 status2;             /**< +0x10 */
    u16 status1;             /**< +0x14 */
    u8 statusAccuracy;       /**< +0x16 */
    u8 statJunction[9];      /**< +0x17: HP..LCK junction values, indexed by JunctionType. */
    u8 atkElement;           /**< +0x20: J-Elem attack element. */
    u8 atkElementValue;      /**< +0x21 */
    u8 defElement;           /**< +0x22: J-Elem defense elements. */
    u8 defElementValue;      /**< +0x23 */
    u8 atkStatusValue;       /**< +0x24 */
    u8 defStatusValue;       /**< +0x25 */
    u16 atkStatuses;         /**< +0x26: J-Status attack statuses. */
    u16 defStatuses;         /**< +0x28: J-Status defense statuses. */
    u8 gfCompatibility[16];  /**< +0x2A: Compatibility change per junctionable GF. */
    u8 pad3A[2];
} MagicEntry; /* 60 bytes */

/** @brief Character entry (36 bytes). */
typedef struct {
    u16 lookupParam;             /**< +0x00: Name text offset (getBattleCharName/getCharName). */
    u8 crisisLevelHpMultiplier;  /**< +0x02 */
    u8 gender;                   /**< +0x03: 0 = male, 1 = female. */
    u8 limitBreakId;             /**< +0x04 */
    u8 limitBreakParam;          /**< +0x05: Power of each Renzokuken hit before the finisher. */
    u8 linearCoeff;              /**< +0x06: XP curve linear coefficient. */
    u8 quadDivisor;              /**< +0x07: XP curve quadratic divisor. */
    u8 hpCurve[4]; /**< +0x08: HP growth: per level, divisor of 10 x level squared, base. */
    u8 strCurve[4]; /**< +0x0C: STR growth; VIT..LCK follow in the same form. */
    u8 vitCurve[4]; /**< +0x10 */
    u8 magCurve[4]; /**< +0x14 */
    u8 sprCurve[4]; /**< +0x18 */
    u8 spdCurve[4]; /**< +0x1C */
    u8 lckCurve[4]; /**< +0x20 */
} CharacterEntry; /* 36 bytes */

/** @brief CharacterEntry.gender of a female character. */
#define GENDER_FEMALE 1

#define GF_ABILITY_SLOT_COUNT 21

/** @brief One of a junctionable GF's learnable-ability slots (4 bytes). */
typedef struct {
    u8 pad00[2];           /**< +0x00..+0x01: Unknown. */
    u8 abilityId;          /**< +0x02: Ability ID (0 = empty). */
    u8 pad03;              /**< +0x03: Unknown. */
} GfAbilitySlot; /* 4 bytes */

/** @brief Junctionable GF entry (132 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 attackType;         /**< +0x06 */
    u8 power;              /**< +0x07: GF power, used in the damage formula. */
    u8 statusWindowFlags;  /**< +0x08 */
    u8 targetInfo;         /**< +0x09 */
    u8 attackFlags;        /**< +0x0A */
    u8 hitAnimation;       /**< +0x0B: The target's hit reaction animation. */
    u8 hitCount;           /**< +0x0C */
    u8 element;            /**< +0x0D */
    u16 status1;           /**< +0x0E */
    u32 status2;           /**< +0x10 */
    u8 xpLinear;           /**< +0x14: Curve linear coefficient (func_8002172C). */
    u8 xpQuadDiv;          /**< +0x15: Curve quadratic divisor. */
    u8 xpConst;            /**< +0x16: Curve constant term. */
    u8 xpParamA;           /**< +0x17: Unknown. */
    u8 xpParamB;           /**< +0x18: Linear coeff for evalAbilityCurve. */
    u8 xpParamC;           /**< +0x19: Quad divisor for evalAbilityCurve. */
    u8 pad1A;              /**< +0x1A: Unknown. */
    u8 statusAccuracy;     /**< +0x1B */
    GfAbilitySlot abilities[GF_ABILITY_SLOT_COUNT]; /**< +0x1C..+0x6F */
    u8 gfCompatibility[16]; /**< +0x70: Compatibility change per junctionable GF. */
    u8 pad80[2];           /**< +0x80..+0x81: Unknown. */
    u8 powerModifier;      /**< +0x82 */
    u8 levelModifier;      /**< +0x83 */
} JunctionableGfEntry; /* 132 bytes */

/** @brief Enemy attack entry (20 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 animation;         /**< +0x02: Attack animation. */
    u8 camera;             /**< +0x04: Camera change (bits 0-6); bit 7 forces it. */
    u8 hitAnimation;       /**< +0x05: The target's hit reaction animation. */
    u8 attackType;         /**< +0x06 */
    u8 power;              /**< +0x07 */
    u8 attackFlags;        /**< +0x08 */
    u8 hitCount;           /**< +0x09: Hit count (bits 0-6); bit 7 shows the attack's name. */
    u8 element;            /**< +0x0A */
    u8 critBonus;          /**< +0x0B */
    u8 statusAccuracy;     /**< +0x0C */
    u8 hitRate;            /**< +0x0D */
    u16 status1;           /**< +0x0E */
    u32 status2;           /**< +0x10 */
} EnemyAttackEntry; /* 20 bytes */

/** @brief EnemyAttackEntry.hitCount bits 0-6: the hit count. */
#define ENEMY_ATTACK_HIT_COUNT 0x7F

/** @brief EnemyAttackEntry.hitCount bit 7: show the attack's name. */
#define ENEMY_ATTACK_SHOW_NAME 0x80

/** @brief Weapon entry (12 bytes). */
typedef struct {
    u16 nameOffset;          /**< +0x00: Name text offset. */
    u8 renzokukenFinishers;  /**< +0x02: Bit per finisher the weapon allows. */
    u8 pad03;
    u8 characterId;          /**< +0x04 */
    u8 attackType;           /**< +0x05 */
    u8 power;                /**< +0x06 */
    u8 hitRate;              /**< +0x07 */
    u8 strBonus;             /**< +0x08 */
    u8 tier;                 /**< +0x09 */
    u8 critBonus;            /**< +0x0A */
    u8 melee;                /**< +0x0B: Bit 0 marks a melee weapon. */
} WeaponEntry; /* 12 bytes */

/** @brief WeaponEntry.melee bit 0: a melee weapon. */
#define WEAPON_MELEE 0x01

/** @brief Battle item entry (24 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 attackType;         /**< +0x06 */
    u8 power;              /**< +0x07 */
    u8 category;           /**< +0x08 */
    u8 targetInfo;         /**< +0x09 */
    u8 attackFlags;        /**< +0x0A: Attack flags; in menus 0x80 = usable in battle. */
    u8 hitAnimation;       /**< +0x0B: The target's hit reaction animation. */
    u8 pad0C;
    u8 statusAccuracy;     /**< +0x0D */
    u16 status1;           /**< +0x0E */
    u32 status2;           /**< +0x10 */
    u8 hitRate;            /**< +0x14 */
    u8 randomSelect;       /**< +0x15: Bit 0 allows random battle-item selection. */
    u8 hitCount;           /**< +0x16 */
    u8 element;            /**< +0x17 */
} BattleItemEntry; /* 24 bytes */

/** @brief Non-battle item entry (4 bytes). */
typedef struct {
    u16 param0;    /**< +0x00: Name text offset (getItemName). */
    u16 param1;    /**< +0x02: Description text offset (getItemDesc). */
} NonBattleItemEntry; /* 4 bytes */

/** @brief Non-junctionable GF attack entry (20 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 animation;         /**< +0x02: Attack animation. */
    u8 attackType;         /**< +0x04 */
    u8 power;              /**< +0x05 */
    u8 statusAccuracy;     /**< +0x06 */
    u8 targetInfo;         /**< +0x07 */
    u8 attackFlags;        /**< +0x08 */
    u8 hitAnimation;       /**< +0x09: The target's hit reaction animation. */
    u8 hitCount;           /**< +0x0A */
    u8 element;            /**< +0x0B */
    u32 status2;           /**< +0x0C */
    u16 status1;           /**< +0x10 */
    u8 powerModifier;      /**< +0x12 */
    u8 levelModifier;      /**< +0x13 */
} NonJunctionableGfAttackEntry; /* 20 bytes */

/** @brief Moogle Dance, the GF attack of the MiniMog command. */
#define GF_ATTACK_MOOGLE_DANCE 6

/** @brief Battle data of a command ability (16 bytes). */
typedef struct {
    u16 animation;         /**< +0x00: Attack animation. */
    u8 pad02;
    u8 hitAnimation;       /**< +0x03: The target's hit reaction animation. */
    u8 attackType;         /**< +0x04 */
    u8 power;              /**< +0x05 */
    u8 attackFlags;        /**< +0x06 */
    u8 hitCount;           /**< +0x07 */
    u8 element;            /**< +0x08 */
    u8 statusAccuracy;     /**< +0x09 */
    u16 status1;           /**< +0x0A */
    u32 status2;           /**< +0x0C */
} CommandAbilityDataEntry; /* 16 bytes */

/** @brief Renzokuken finisher entry (24 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 attackType;         /**< +0x06 */
    u8 pad07;
    u8 power;              /**< +0x08 */
    u8 hitAnimation;       /**< +0x09: The target's hit reaction animation. */
    u8 targetInfo;         /**< +0x0A */
    u8 attackFlags;        /**< +0x0B */
    u8 hitCount;           /**< +0x0C */
    u8 element;            /**< +0x0D */
    u8 elementPercent;     /**< +0x0E */
    u8 statusAccuracy;     /**< +0x0F */
    u16 pad10;
    u16 status1;           /**< +0x12 */
    u32 status2;           /**< +0x14 */
} RenzokukenFinisherEntry; /* 24 bytes */

/** @brief Temporary character limit break entry (24 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 attackType;         /**< +0x06 */
    u8 power;              /**< +0x07 */
    u8 hitAnimation;       /**< +0x08: The target's hit reaction animation. */
    u8 statusWindowFlags;  /**< +0x09 */
    u8 targetInfo;         /**< +0x0A */
    u8 attackFlags;        /**< +0x0B */
    u8 hitCount;           /**< +0x0C */
    u8 element;            /**< +0x0D */
    u8 elementPercent;     /**< +0x0E */
    u8 statusAccuracy;     /**< +0x0F */
    u16 status1;           /**< +0x10 */
    u16 pad12;
    u32 status2;           /**< +0x14 */
} TempLimitBreakEntry; /* 24 bytes */

/** @brief Blue magic entry (16 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 hitAnimation;       /**< +0x06: The target's hit reaction animation. */
    u8 attackType;         /**< +0x07 */
    u8 statusWindowFlags;  /**< +0x08 */
    u8 targetInfo;         /**< +0x09 */
    u8 attackFlags;        /**< +0x0A */
    u8 hitCount;           /**< +0x0B */
    u8 element;            /**< +0x0C */
    u8 statusAccuracy;     /**< +0x0D */
    u8 critBonus;          /**< +0x0E */
    u8 pad0F;
} BlueMagicEntry; /* 16 bytes */

/** @brief Blue magic parameters for one crisis level (8 bytes). */
typedef struct {
    u32 status2;           /**< +0x00 */
    u16 status1;           /**< +0x04 */
    u8 power;              /**< +0x06 */
    u8 hitRate;            /**< +0x07 */
} BlueMagicParamEntry; /* 8 bytes */

/** @brief Shot (Irvine limit break) entry (24 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 attackType;         /**< +0x06 */
    u8 power;              /**< +0x07 */
    u8 hitAnimation;       /**< +0x08: The target's hit reaction animation. */
    u8 statusWindowFlags;  /**< +0x09 */
    u8 targetInfo;         /**< +0x0A */
    u8 attackFlags;        /**< +0x0B */
    u8 hitCount;           /**< +0x0C */
    u8 element;            /**< +0x0D */
    u8 elementPercent;     /**< +0x0E */
    u8 statusAccuracy;     /**< +0x0F */
    u16 status1;           /**< +0x10 */
    u8 ammoItem;           /**< +0x12: Item index of the ammo it uses. */
    u8 critBonus;          /**< +0x13 */
    u32 status2;           /**< +0x14 */
} ShotEntry; /* 24 bytes */

/** @brief Duel (Zell limit break) entry (32 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u16 animation;         /**< +0x04: Attack animation. */
    u8 attackType;         /**< +0x06 */
    u8 power;              /**< +0x07 */
    u8 hitAnimation;       /**< +0x08: The target's hit reaction animation. */
    u8 pad09;
    u8 targetInfo;         /**< +0x0A */
    u8 attackFlags;        /**< +0x0B */
    u8 hitCount;           /**< +0x0C */
    u8 element;            /**< +0x0D */
    u8 elementPercent;     /**< +0x0E */
    u8 statusAccuracy;     /**< +0x0F */
    u16 buttons[5];        /**< +0x10: Input sequence; the first also flags a finisher. */
    u16 status1;           /**< +0x1A */
    u32 status2;           /**< +0x1C */
} DuelEntry; /* 32 bytes */

/** @brief Duel move graph node (4 bytes). */
typedef struct {
    u8 startMove;          /**< +0x00: Duel move; 6 and up are finishers. */
    u8 nextSequence[3];    /**< +0x01: Next node per button input. */
} DuelSequence; /* 4 bytes */

/** @brief Rinoa limit break (part 1) entry (8 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 descOffset;        /**< +0x02: Description text offset. */
    u8 statusWindowFlags;  /**< +0x04 */
    u8 targetInfo;         /**< +0x05 */
    u8 unk06;              /**< +0x06: Unused (0xFF). */
    u8 pad07;
} RinoaLimitBreak1Entry; /* 8 bytes */

/** @brief Rinoa limit break (part 2) entry (20 bytes). */
typedef struct {
    u16 nameOffset;        /**< +0x00: Name text offset. */
    u16 animation;         /**< +0x02: Attack animation. */
    u8 attackType;         /**< +0x04 */
    u8 power;              /**< +0x05 */
    u8 hitAnimation;       /**< +0x06: The target's hit reaction animation. */
    u8 pad07;
    u8 targetInfo;         /**< +0x08 */
    u8 attackFlags;        /**< +0x09 */
    u8 hitCount;           /**< +0x0A */
    u8 element;            /**< +0x0B */
    u8 elementPercent;     /**< +0x0C */
    u8 statusAccuracy;     /**< +0x0D */
    u16 status1;           /**< +0x0E */
    u32 status2;           /**< +0x10 */
} RinoaLimitBreak2Entry; /* 20 bytes */

/** @brief One magic of a Slot (Selphie limit break) set (2 bytes). */
typedef struct {
    u8 magicId;            /**< +0x00 */
    u8 count;              /**< +0x01: Most casts; the count is rolled from 1 to this. */
} SlotSetMagic; /* 2 bytes */

/** @brief Devour entry (12 bytes). */
typedef struct {
    u16 descOffset;        /**< +0x00: Description text offset. */
    u8 hpMode;             /**< +0x02: 0x1E heals, 0x1F damages. */
    u8 hpAmount;           /**< +0x03: HP healed or lost, in sixteenths of max HP. */
    u32 status2;           /**< +0x04 */
    u16 status1;           /**< +0x08 */
    u8 raisedStat;         /**< +0x0A: DEVOUR_RAISE_* bits. */
    u8 raisedMaxHp;        /**< +0x0B: Max HP raised. */
} DevourEntry; /* 12 bytes */

/** @brief DevourEntry.raisedStat bits, one per stat raised. */
#define DEVOUR_RAISE_STR  0x01
#define DEVOUR_RAISE_VIT  0x02
#define DEVOUR_RAISE_MAG  0x04
#define DEVOUR_RAISE_SPR  0x08
#define DEVOUR_RAISE_SPD  0x10
#define DEVOUR_RAISE_LUCK 0x20

/** @brief DevourEntry.hpMode values. */
#define DEVOUR_CURE   0x1E
#define DEVOUR_DAMAGE 0x1F

/** @brief Duel start and timer for one crisis level (2 bytes). */
typedef struct {
    u8 startSequence;      /**< +0x00: First DuelSequence node. */
    u8 timer;              /**< +0x01 */
} DuelCrisisLevel; /* 2 bytes */

/** @brief Miscellaneous battle settings (0x3C bytes). */
typedef struct {
    u8 statusTimers[14];         /**< +0x00: Sleep .. Float. */
    u8 atbSpeed;                 /**< +0x0E: ATB speed multiplier. */
    u8 summonInterval;           /**< +0x0F: Gilgamesh/Angelo summon interval. */
    u8 status1LimitEffects[8];   /**< +0x10: Limit effect per status 1 bit. */
    u8 status2LimitEffects[24];  /**< +0x18: Limit effect per status 2 bit. */
    DuelCrisisLevel duel[4];     /**< +0x30: Per crisis level 1-4. */
    u8 shotTimers[4];            /**< +0x38: Per crisis level 1-4. */
} KernelMisc; /* 0x3C bytes */

/** @brief Miscellaneous text pointer (2 bytes). */
typedef struct {
    u16 param0;    /**< +0x00: Text offset (getMenuString). */
} MiscTextEntry; /* 2 bytes */

/**
 * @brief kernel.bin: a section count and offset table, then the data sections.
 *
 * The @c ...Text fields hold each section's text offset, which resolveKernelPtr
 * adds to the text-relative offsets stored in the entries.
 */
typedef struct {
    /* 0x0000 */ s32 sectionCount;
    /* 0x0004 */ s32 dataOffsets[31];
    /* 0x0080 */ s32 battleCommandsText;
    /* 0x0084 */ s32 magicText;
    /* 0x0088 */ s32 junctionableGfsText;
    /* 0x008C */ s32 enemyAttacksText;
    /* 0x0090 */ s32 weaponsText;
    /* 0x0094 */ s32 renzokukenFinishersText;
    /* 0x0098 */ s32 charactersText;
    /* 0x009C */ s32 battleItemsText;
    /* 0x00A0 */ s32 nonBattleItemsText;
    /* 0x00A4 */ s32 nonJunctionableGfAttacksText;
    /* 0x00A8 */ s32 junctionAbilitiesText;
    /* 0x00AC */ s32 commandAbilitiesText;
    /* 0x00B0 */ s32 statPercentAbilitiesText;
    /* 0x00B4 */ s32 characterAbilitiesText;
    /* 0x00B8 */ s32 partyAbilitiesText;
    /* 0x00BC */ s32 gfAbilitiesText;
    /* 0x00C0 */ s32 menuAbilitiesText;
    /* 0x00C4 */ s32 tempLimitBreaksText;
    /* 0x00C8 */ s32 blueMagicText;
    /* 0x00CC */ s32 shotText;
    /* 0x00D0 */ s32 duelText;
    /* 0x00D4 */ s32 rinoaLimitBreaks1Text;
    /* 0x00D8 */ s32 rinoaLimitBreaks2Text;
    /* 0x00DC */ s32 devourText;
    /* 0x00E0 */ s32 miscText;
    /* 0x00E4 */ BattleCommandEntry battleCommands[39];
    /* 0x021C */ MagicEntry magic[57];
    /* 0x0F78 */ JunctionableGfEntry junctionableGfs[16];
    /* 0x17B8 */ EnemyAttackEntry enemyAttacks[384];
    /* 0x35B8 */ WeaponEntry weapons[33];
    /* 0x3744 */ RenzokukenFinisherEntry renzokukenFinishers[4];
    /* 0x37A4 */ CharacterEntry characters[11];
    /* 0x3930 */ BattleItemEntry battleItems[33];
    /* 0x3C48 */ NonBattleItemEntry nonBattleItems[166];
    /* 0x3EE0 */ NonJunctionableGfAttackEntry nonJunctionableGfAttacks[16];
    /* 0x4020 */ CommandAbilityDataEntry commandAbilityData[12];
    /* 0x40E0 */ AbilityEntry junctionAbilities[20];
    /* 0x4180 */ AbilityEntry commandAbilities[19];
    /* 0x4218 */ AbilityEntry statPercentAbilities[19];
    /* 0x42B0 */ AbilityEntry characterAbilities[20];
    /* 0x4350 */ AbilityEntry partyAbilities[5];
    /* 0x4378 */ AbilityEntry gfAbilities[9];
    /* 0x43C0 */ AbilityEntry menuAbilities[24];
    /* 0x4480 */ TempLimitBreakEntry tempLimitBreaks[5];
    /* 0x44F8 */ BlueMagicEntry blueMagic[16];
    /* 0x45F8 */ BlueMagicParamEntry blueMagicParams[16 * 4];
    /* 0x47F8 */ ShotEntry shot[8];
    /* 0x48B8 */ DuelEntry duel[10];
    /* 0x49F8 */ DuelSequence duelParams[25];
    /* 0x4A5C */ RinoaLimitBreak1Entry rinoaLimitBreaks1[2];
    /* 0x4A6C */ RinoaLimitBreak2Entry rinoaLimitBreaks2[5];
    /* 0x4AD0 */ u8 slotArray[5][12];
    /* 0x4B0C */ SlotSetMagic slotSets[16][8];
    /* 0x4C0C */ DevourEntry devour[16];
    /* 0x4CCC */ KernelMisc misc;
    /* 0x4D08 */ MiscTextEntry miscTextPointers[128];
} Kernel;

extern Kernel g_kernel;

#endif
