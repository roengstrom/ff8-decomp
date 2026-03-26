#ifndef GF_H
#define GF_H

/**
 * @file gf.h
 * @brief Guardian Force data region layout (g_gfData, loaded at runtime).
 *
 * This header documents the structure of the runtime GF junction/ability data
 * region. The struct definitions here match verified field offsets used by
 * decomped code.
 *
 * g_gfData is BSS (zero-initialized) and populated at runtime by the game's
 * data-loading routines.
 */

#include "common.h"

/**
 * @brief Ability table entry (stride 8 bytes).
 *
 * Seven contiguous ranges of these entries span offsets 0x40E0–0x43FF within
 * g_gfData, covering ability IDs 0x00–0x63+:
 *
 *   Range I:  0x40E0  (20 entries, IDs 0x00–0x13, ptr slot +0xA8)
 *   Range J:  0x4180  (19 entries, IDs 0x14–0x26, ptr slot +0xAC)
 *   Range K:  0x4218  (19 entries, IDs 0x27–0x39, ptr slot +0xB0)
 *   Range L:  0x42B0  (20 entries, IDs 0x3A–0x4D, ptr slot +0xB4)
 *   Range M:  0x4350  ( 5 entries, IDs 0x4E–0x52, ptr slot +0xB8)
 *   Range N:  0x4378  ( 9 entries, IDs 0x53–0x5B, ptr slot +0xBC)
 *   Range O:  0x43C0  ( 8 entries, IDs 0x5C+,     ptr slot +0xC0)
 *
 * GetAbilityCap() reads the cap field (+0x04) from the appropriate range.
 */
typedef struct {
    u16 statParam0;   /**< +0x00: Primary stat parameter. */
    u16 statParam1;   /**< +0x02: Secondary stat parameter. */
    u8 cap;           /**< +0x04: Ability cap value (checked by GetAbilityCap). */
    u8 typeField;     /**< +0x05: Command type ID or element/status type. */
    u8 bonusField;    /**< +0x06: Bonus value or GF field B value. */
    u8 extraField;    /**< +0x07: Status immunity byte, party flag, or mode selector. */
} AbilityEntry; /* 8 bytes */

/**
 * @brief GF junction/stat entry (stride 60 bytes).
 *
 * 16 entries starting at offset 0x21C within g_gfData (ptr slot +0x84).
 * Each entry holds stat multipliers, base values, junction IDs, and flag
 * fields for one GF configuration.
 */
typedef struct {
    u16 nameParam0;   /**< +0x00: Lookup param (getMagicNamePtr). */
    u16 nameParam1;   /**< +0x02: Secondary lookup param (getSpellEntityData). */
    u8 pad04;         /**< +0x04: Unknown. */
    u8 spiritMult;    /**< +0x05: Spirit/magic junction multiplier. */
    u8 magicBase;     /**< +0x06: Magic power base value. */
    u8 pad07;         /**< +0x07: Unknown. */
    u8 statBase;      /**< +0x08: Compatibility/stat modifier base. */
    u8 statMult;      /**< +0x09: Multiplier/junction quantity. */
    u8 flagBits;      /**< +0x0A: Bit-field flags (srav source). */
    u8 juncId;        /**< +0x0B: Junction ID. */
    u8 hitMult;       /**< +0x0C: Hit/accuracy bonus multiplier. */
    u8 juncId2;       /**< +0x0D: Secondary junction ID. */
    u16 flags;        /**< +0x0E: Status/attribute flags. */
    u16 flags2;       /**< +0x10: Secondary flags. */
    u8 pad12[0x0B];   /**< +0x12..+0x1C: Unknown. */
    u8 spiritParam;   /**< +0x1D: Spirit parameter (func_800220E4). */
    u8 magicParam;    /**< +0x1E: Magic parameter (func_80022028). */
    u8 pad1F;         /**< +0x1F: Unknown. */
    u8 statParamA;    /**< +0x20: Stat parameter A (func_8002216C). */
    u8 statParamB;    /**< +0x21: Stat parameter B (func_800221B4). */
    u8 pad22[2];      /**< +0x22..+0x23: Unknown. */
    u8 hitParam;      /**< +0x24: Hit parameter (func_80022404). */
    u8 pad25;         /**< +0x25: Unknown. */
    u16 statusFlags;  /**< +0x26: Status flags bitmask (func_80022328/func_80022370). */
    u8 pad28[0x14];   /**< +0x28..+0x3B: Unknown. */
} GfJunctionEntry; /* 60 bytes: 2+2+1+1+1+1+1+1+1+1+1+1+2+2+11+1+1+1+1+1+2+1+1+2+20 = 60 */

/**
 * @brief GF XP curve entry (stride 36 bytes).
 *
 * 11 entries starting at offset 0x37A4 within g_gfData (ptr slot +0x98).
 * Holds experience curve coefficients used by func_80021A64.
 */
typedef struct {
    u16 lookupParam;  /**< +0x00: Lookup param (getBattleCharName/getCharNamePtr). */
    u8 pad02[4];      /**< +0x02..+0x05: Unknown fields. */
    u8 linearCoeff;   /**< +0x06: XP curve linear coefficient. */
    u8 quadDivisor;   /**< +0x07: XP curve quadratic divisor. */
    u8 constant;      /**< +0x08: XP curve constant term. */
    u8 pad09[3];      /**< +0x09..+0x0B: Unknown. */
    u8 field0C;       /**< +0x0C: Unknown. */
    u8 field0D;       /**< +0x0D: Unknown. */
    u8 subIdx;        /**< +0x0E: func_80021A64 multiplier. */
    u8 divisorField;  /**< +0x0F: func_80021A64 divisor. */
    u8 addend;        /**< +0x10: func_80021A64 addend. */
    u8 pad11[0x13];   /**< +0x11..+0x23: Remaining (stride 36 total). */
} GfCurveEntry; /* 36 bytes */

/**
 * @brief GF ability table entry (stride 132 bytes).
 *
 * 16 entries starting at offset 0xF7A within g_gfData (ptr slot +0x88,
 * which points to 0xF8C = 0xF7A + 0x12, i.e. &entry->xpLinear).
 *
 * func_8002172C reads xpLinear, xpQuadDiv, xpConst as curve coefficients.
 * func_8002166C/func_800216B0 read xpParamB, xpParamC.
 * Ability slots begin at +0x1C with stride 4, up to 21 slots.
 */
typedef struct {
    u16 nameParam0;        /**< +0x00: Lookup param (getSpellEntityData). */
    u8 preamble02[16];     /**< +0x02..+0x11: Preamble (unknown). */
    u8 xpLinear;           /**< +0x12: Linear coefficient (at ptrAbilityTable132+0). */
    u8 xpQuadDiv;          /**< +0x13: Quadratic divisor (at ptrAbilityTable132+1). */
    u8 xpConst;            /**< +0x14: Constant term (at ptrAbilityTable132+2). */
    u8 xpParamA;           /**< +0x15: Additional parameter A. */
    u8 xpParamB;           /**< +0x16: Linear coeff for func_8002166C. */
    u8 xpParamC;           /**< +0x17: Quad divisor for func_8002166C. */
    u8 pad18[4];           /**< +0x18..+0x1B: Unknown. */
    u8 abilitySlots[84];   /**< +0x1C..+0x6F: 21 ability slots, 4 bytes each. */
    u8 pad70[20];          /**< +0x70..+0x83: Remaining (132 bytes total). */
} GfAbilityTableEntry; /* 132 bytes: 2+16+1+1+1+1+1+1+4+84+20 = 132 */

/** @brief LevelCurve12Entry (stride 12, +0x35B8 in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getLevelCurveData). */
    u8 pad02[5];   /**< +0x02..+0x06: Unknown. */
    u8 field07;    /**< +0x07: Used in func_80022028. */
    u8 pad08[4];   /**< +0x08..+0x0B: Unknown. */
} LevelCurve12Entry; /* 12 bytes */

/** @brief StatTable24Entry (stride 24, +0x3930 in GfData, ability id < 0x21) */
typedef struct {
    u16 param0;    /**< +0x00: Stat param (getStatName). */
    u16 param1;    /**< +0x02: Stat param (getStatDesc). */
    u8 pad04[20];  /**< +0x04..+0x17: Unknown. */
} StatTable24Entry; /* 24 bytes */

/** @brief StatTable4Entry (stride 4, +0x3C48 in GfData, ability id >= 0x21) */
typedef struct {
    u16 param0;    /**< +0x00: Stat param (getStatName). */
    u16 param1;    /**< +0x02: Stat param (getStatDesc). */
} StatTable4Entry; /* 4 bytes */

/** @brief ElementData24Entry (stride 24, +0x3744 in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Element param (getElementName). */
    u16 param1;    /**< +0x02: Element param (getElementDesc). */
    u8 pad04[20];  /**< +0x04..+0x17: Unknown. */
} ElementData24Entry; /* 24 bytes */

/** @brief GfSubTablePEntry (stride 24, +0x4480 in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getJuncCategoryName). */
    u16 param1;    /**< +0x02: Lookup param (getJuncCategoryDesc). */
    u8 pad04[20];  /**< +0x04..+0x17: Unknown. */
} GfSubTablePEntry; /* 24 bytes */

/** @brief GfSubTableQEntry (stride 16, +0x44F8 in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getJuncEffectName). */
    u16 param1;    /**< +0x02: Lookup param (getJuncEffectDesc). */
    u8 pad04[12];  /**< +0x04..+0x0F: Unknown. */
} GfSubTableQEntry; /* 16 bytes */

/** @brief GfSubTableREntry (stride 24, +0x47F8 in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getStatusEffectName). */
    u16 param1;    /**< +0x02: Lookup param (getStatusEffectDesc). */
    u8 pad04[20];  /**< +0x04..+0x17: Unknown. */
} GfSubTableREntry; /* 24 bytes */

/**
 * @brief GF sub-table S entry (stride 32, +0x48B8 in GfData).
 *
 * Per-entity ability/status data.
 */
typedef struct {
    u16 param0;        /**< +0x00: Lookup param (getMagicEffectName). */
    u16 param1;        /**< +0x02: Lookup param (getMagicEffectDesc). */
    u8 pad04[6];       /**< +0x04..+0x09: Unknown. */
    u8 abilityFlags;   /**< +0x0A: Ability flags byte (func_8009BA5C). */
    u8 pad0B[0x15];    /**< +0x0B..+0x1F: Remaining fields (unknown). */
} GfSubTableSEntry; /* 0x20 = 32 bytes */

/** @brief GfSubTableTEntry (stride 8, +0x4A5C in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getGfName). */
    u8 pad02[6];   /**< +0x02..+0x07: Unknown. */
} GfSubTableTEntry; /* 8 bytes */

/** @brief GfSubTableUEntry (stride 20, +0x4A6C in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getGfSummonData). */
    u8 pad02[18];  /**< +0x02..+0x13: Unknown. */
} GfSubTableUEntry; /* 20 bytes */

/** @brief GfSubTableVEntry (stride 2, +0x4D08 in GfData) */
typedef struct {
    u16 param0;    /**< +0x00: Lookup param (getMenuString). */
} GfSubTableVEntry; /* 2 bytes */

/**
 * @brief Full GF data region layout (BSS, populated at runtime).
 *
 * The header region (0x00–0xE3) contains s32 pointers into the sub-table
 * regions that follow. These pointers are populated at runtime.
 */
typedef struct {
    /* --- Header region: runtime pointers into sub-tables --- */
    u8 pad00[0x80];                             /**< +0x000..+0x07F: Unknown header fields. */
    s32 ptrStatTable8;                          /**< +0x080: -> statTable8 (stride 8). */
    s32 ptrGfSpellData;                         /**< +0x084: -> junctionData (stride 60). */
    s32 ptrAbilityTable132;                     /**< +0x088: -> abilityTable132+0x12 (runtime 0xF8C). */
    u8 pad8C[4];                                /**< +0x08C: Unknown. */
    s32 ptrLevelCurve12;                        /**< +0x090: -> levelCurve12 (stride 12). */
    s32 ptrElementData24;                       /**< +0x094: -> elementData24 (stride 24). */
    s32 ptrGfCurve36;                           /**< +0x098: -> xpCurves36 (stride 36). */
    s32 ptrStatTable24;                         /**< +0x09C: -> statTable24 (stride 24, id < 0x21). */
    s32 ptrStatTable4;                          /**< +0x0A0: -> statTable4 (stride 4, id >= 0x21). */
    u8 padA4[4];                                /**< +0x0A4: Unknown. */
    s32 ptrAbilityRangeI;                       /**< +0x0A8: -> abilityRangeI (IDs 0x00–0x13). */
    s32 ptrAbilityRangeJ;                       /**< +0x0AC: -> abilityRangeJ (IDs 0x14–0x26). */
    s32 ptrAbilityRangeK;                       /**< +0x0B0: -> abilityRangeK (IDs 0x27–0x39). */
    s32 ptrAbilityRangeL;                       /**< +0x0B4: -> abilityRangeL (IDs 0x3A–0x4D). */
    s32 ptrAbilityRangeM;                       /**< +0x0B8: -> abilityRangeM (IDs 0x4E–0x52). */
    s32 ptrAbilityRangeN;                       /**< +0x0BC: -> abilityRangeN (IDs 0x53–0x5B). */
    s32 ptrAbilityRangeO;                       /**< +0x0C0: -> abilityRangeO (IDs 0x5C+). */
    s32 ptrSubTableP;                           /**< +0x0C4: -> subTableP (stride 24). */
    s32 ptrSubTableQ;                           /**< +0x0C8: -> subTableQ (stride 16). */
    s32 ptrSubTableR;                           /**< +0x0CC: -> subTableR (stride 24). */
    s32 ptrSubTableS;                           /**< +0x0D0: -> subTableS (stride 32). */
    s32 ptrSubTableT;                           /**< +0x0D4: -> subTableT (stride 8). */
    s32 ptrSubTableU;                           /**< +0x0D8: -> subTableU (stride 20). */
    u8 padDC[4];                                /**< +0x0DC: Unknown. */
    s32 ptrSubTableV;                           /**< +0x0E0: -> subTableV (stride 2). */

    /* --- Sub-table body regions --- */
    AbilityEntry statTable8[39];                /**< +0x0E4: AbilityEntry[39] (stride 8). */
    GfJunctionEntry junctionData[16];           /**< +0x21C: GF junction entries (16 × 60 bytes). */
    u8 junctionDataTrailing[0x99E];             /**< +0x5DC: Trailing junction data. */
    GfAbilityTableEntry abilityTable132[16];    /**< +0xF7A: GfAbilityTableEntry[16] (stride 132). */
    u8 abilityTablePad[0x1DFE];                 /**< +0x17BA: Padding to levelCurve12. */
    LevelCurve12Entry levelCurve12[33];         /**< +0x35B8: Level curve entries (stride 12, 33 entries). */
    ElementData24Entry elementData24[4];        /**< +0x3744: Element data entries (stride 24, 4 entries). */
    GfCurveEntry xpCurves36[11];                /**< +0x37A4: XP curve entries (stride 36, 11 entries). */
    StatTable24Entry statTable24[33];           /**< +0x3930: Stat table (stride 24, id < 0x21). */
    StatTable4Entry statTable4[294];            /**< +0x3C48: Stat table (stride 4, id >= 0x21). */
    AbilityEntry abilityRangeI[20];             /**< +0x40E0: AbilityEntry[20] (IDs 0x00–0x13). */
    AbilityEntry abilityRangeJ[19];             /**< +0x4180: AbilityEntry[19] (IDs 0x14–0x26). */
    AbilityEntry abilityRangeK[19];             /**< +0x4218: AbilityEntry[19] (IDs 0x27–0x39). */
    AbilityEntry abilityRangeL[20];             /**< +0x42B0: AbilityEntry[20] (IDs 0x3A–0x4D). */
    AbilityEntry abilityRangeM[5];              /**< +0x4350: AbilityEntry[5] (IDs 0x4E–0x52). */
    AbilityEntry abilityRangeN[9];              /**< +0x4378: AbilityEntry[9] (IDs 0x53–0x5B). */
    AbilityEntry abilityRangeO[24];             /**< +0x43C0: AbilityEntry[24] (IDs 0x5C+). */
    GfSubTablePEntry subTableP[5];              /**< +0x4480: Sub-table P (stride 24, 5 entries). */
    GfSubTableQEntry subTableQ[48];             /**< +0x44F8: Sub-table Q (stride 16, 48 entries). */
    GfSubTableREntry subTableR[8];              /**< +0x47F8: Sub-table R (stride 24, 8 entries). */
    GfSubTableSEntry subTableS[13];             /**< +0x48B8: Sub-table S (stride 32, 13 entries). */
    u8 pad4A58[4];                              /**< +0x4A58: Gap before subTableT. */
    GfSubTableTEntry subTableT[2];              /**< +0x4A5C: Sub-table T (stride 8, 2 entries). */
    GfSubTableUEntry subTableU[33];             /**< +0x4A6C: Sub-table U (stride 20, 33 entries). */
    u8 padSubTableU[8];                         /**< +0x4D00: Padding to subTableV. */
    GfSubTableVEntry subTableV[56];             /**< +0x4D08: Sub-table V (stride 2, 56 entries). */
} GfData;

/** @brief Runtime GF data region (BSS, populated at load time). */
extern GfData g_gfData;

/**
 * @brief Per-magic-spell junction data (stride 60 bytes).
 *
 * Indexed by magic ID (0–56). Each entry holds the stat junction bonus
 * for each of the 9 stats, plus element/status flags and multipliers
 * used to score magic for auto-junction.
 *
 * Lives at D_8007901C (= g_gfData + 0x21C, same region as junctionData
 * but indexed by magic spell, not GF).
 */
typedef struct {
    u8 pad00[0x17];           /**< +0x00: Preamble (name params, base stats, etc.). */
    u8 statJunction[9];       /**< +0x17: Stat junction values [HP,Str,Vit,Mag,Spr,Spd,Eva,Hit,Lck]. */
    u8 atkElemFlags;          /**< +0x20: Attack element flags (popcount → number of elements). */
    u8 atkElemBonus;          /**< +0x21: Attack element bonus multiplier. */
    u8 defElemFlags;          /**< +0x22: Defense element flags (popcount → number of elements). */
    u8 defElemBonus;          /**< +0x23: Defense element bonus multiplier. */
    u8 atkStatusBonus;        /**< +0x24: Attack status bonus multiplier. */
    u8 defStatusBonus;        /**< +0x25: Defense status bonus multiplier. */
    u16 atkStatusFlags;       /**< +0x26: Attack status bitmask (popcount → number of statuses). */
    u16 defStatusFlags;       /**< +0x28: Defense status bitmask (popcount → number of statuses). */
    u8 pad2A[0x12];           /**< +0x2A: Remaining fields (total 60 bytes). */
} MagicJunctionData; /* 60 bytes */

#endif /* GF_H */
