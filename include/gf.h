#ifndef GF_H
#define GF_H

/**
 * @file gf.h
 * @brief Guardian Force data region layout (g_gfData, 3960 bytes at 0x80078E00).
 *
 * This header documents the structure of the runtime GF junction/ability data
 * region. The struct definitions here are for reference — decomped code uses
 * raw pointer offsets to preserve codegen matching.
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
    u8 pad00[5];      /**< +0x00..0x04: Unknown/padding. */
    u8 spiritMult;    /**< +0x05: Spirit/magic junction multiplier. */
    u8 magicBase;     /**< +0x06: Magic power base value. */
    u8 pad07;         /**< +0x07: Unknown. */
    u8 statBase;      /**< +0x08: Compatibility/stat modifier base. */
    u8 statMult;      /**< +0x09: Multiplier/junction quantity. */
    u8 flagBits;      /**< +0x0A: Bit-field flags (srav source). */
    u8 juncId;        /**< +0x0B: Junction ID (func_800219B8). */
    u8 hitMult;       /**< +0x0C: Hit/accuracy bonus multiplier. */
    u8 juncId2;       /**< +0x0D: Secondary junction ID. */
    u16 flags;        /**< +0x0E: Status/attribute flags (0x7F mask + bits 0x80..0x1000). */
    u16 flags2;       /**< +0x10: Secondary flags. */
    u8 pad12[0x2A];   /**< +0x12..0x3B: Remaining fields (stride 60 total). */
} GfJunctionEntry; /* 60 bytes */

/**
 * @brief GF XP curve entry (stride 36 bytes).
 *
 * 16 entries starting at offset 0x37A4 within g_gfData (ptr slot +0x98).
 * Holds experience curve coefficients used by func_80021A64.
 */
typedef struct {
    u8 pad00[6];      /**< +0x00..0x05: Unknown fields. */
    u8 linearCoeff;   /**< +0x06: XP curve linear coefficient. */
    u8 quadDivisor;   /**< +0x07: XP curve quadratic divisor. */
    u8 constant;      /**< +0x08: XP curve constant term. */
    u8 pad09[3];      /**< +0x09..0x0B: Unknown. */
    u8 field0C;       /**< +0x0C: Unknown. */
    u8 field0D;       /**< +0x0D: Unknown. */
    u8 subIdx;        /**< +0x0E: func_80021A64 multiplier. */
    u8 divisorField;  /**< +0x0F: func_80021A64 divisor. */
    u8 addend;        /**< +0x10: func_80021A64 addend. */
    u8 pad11[0x13];   /**< +0x11..0x23: Remaining (stride 36 total). */
} GfCurveEntry; /* 36 bytes */

/**
 * @brief GF ability table entry (stride 132 bytes).
 *
 * 16 entries starting at offset 0xF8C within g_gfData (ptr slot +0x88).
 * Each entry holds XP curve parameters and 21 ability slots (4 bytes each).
 *
 * func_8002172C reads the first 3 bytes as curve coefficients:
 *   +0x00: linear coeff, +0x01: quadratic divisor, +0x02: constant.
 * Ability slots begin at +0x0A with stride 4, up to 21 slots.
 */
typedef struct {
    u8 xpLinear;      /**< +0x00: XP curve linear coefficient. */
    u8 xpQuadDiv;     /**< +0x01: XP curve quadratic divisor. */
    u8 xpConst;       /**< +0x02: XP curve constant term. */
    u8 xpParamA;      /**< +0x03: Additional XP parameter A. */
    u8 xpParamB;      /**< +0x04: Additional XP parameter B. */
    u8 xpParamC;      /**< +0x05: Additional XP parameter C. */
    u8 pad06[4];      /**< +0x06..0x09: Unknown. */
    u8 abilitySlots[84]; /**< +0x0A: 21 ability slots, 4 bytes each. */
    u8 pad5E[0x26];   /**< +0x5E..0x83: Remaining (stride 132 total). */
} GfAbilityTableEntry; /* 132 bytes */

/**
 * @brief GF sub-table S entry (stride 32, offset 0x48B8 in g_gfData).
 *
 * Per-entity ability/status data. The abilityFlags byte at +0x0A within
 * each entry corresponds to offset 0x48C2 from g_gfData base when slot=0.
 */
typedef struct {
    u8 pad00[0x0A];    /**< 0x00..0x09: Unknown fields. */
    u8 abilityFlags;   /**< 0x0A: Ability flags byte. */
    u8 pad0B[0x15];    /**< 0x0B..0x1F: Remaining fields (unknown). */
} GfSubTableSEntry;    /* 0x20 = 32 bytes */

/**
 * @brief Partial view of g_gfData for struct-based access to sub-table S.
 *
 * Used when struct field access is needed for codegen matching (e.g.
 * func_8009BA5C). Uses exact offsets verified against the binary.
 */
typedef struct {
    u8 pad0[0x48B8];                   /**< 0x0000..0x48B7: Earlier sub-tables. */
    GfSubTableSEntry subTableS[13];    /**< 0x48B8: Per-entity entries (stride 32). */
} GfDataSubS;

/**
 * @brief Full GF data region layout (3960 bytes, BSS).
 *
 * The header region (0x00–0xE3) contains s32 pointers into the sub-table
 * regions that follow. These pointers are populated at runtime.
 *
 * @note Some sub-tables are still accessed via raw pointer arithmetic in older
 *       decomped code. Prefer struct field access (`g_gfData.field[idx].member`)
 *       when decomping new functions.
 */
typedef struct {
    /* --- Header region: runtime pointers into sub-tables --- */
    u8 pad00[0x80];           /**< +0x00..0x7F: Unknown header fields. */
    s32 ptrStatTable8;        /**< +0x80: -> offset 0xE4 (stride 8, ability stats). */
    s32 ptrGfSpellData;       /**< +0x84: -> offset 0x21C (stride 60, junction). */
    s32 ptrAbilityTable132;   /**< +0x88: -> offset 0xF8C (stride 132, GF abilities). */
    u8 pad8C[4];              /**< +0x8C: Unknown. */
    s32 ptrLevelCurve12;      /**< +0x90: -> offset 0x35B8 (stride 12). */
    s32 ptrElementData24;     /**< +0x94: -> offset 0x3744 (stride 24). */
    s32 ptrGfCurve36;         /**< +0x98: -> offset 0x37A4 (stride 36, XP curves). */
    s32 ptrStatTable24;       /**< +0x9C: -> offset 0x3930 (stride 24, id < 0x21). */
    s32 ptrStatTable4;        /**< +0xA0: -> offset 0x3C48 (stride 4, id >= 0x21). */
    u8 padA4[4];              /**< +0xA4: Unknown. */
    s32 ptrAbilityRangeI;     /**< +0xA8: -> offset 0x40E0 (stride 8, IDs 0x00–0x13). */
    s32 ptrAbilityRangeJ;     /**< +0xAC: -> offset 0x4180 (stride 8, IDs 0x14–0x26). */
    s32 ptrAbilityRangeK;     /**< +0xB0: -> offset 0x4218 (stride 8, IDs 0x27–0x39). */
    s32 ptrAbilityRangeL;     /**< +0xB4: -> offset 0x42B0 (stride 8, IDs 0x3A–0x4D). */
    s32 ptrAbilityRangeM;     /**< +0xB8: -> offset 0x4350 (stride 8, IDs 0x4E–0x52). */
    s32 ptrAbilityRangeN;     /**< +0xBC: -> offset 0x4378 (stride 8, IDs 0x53–0x5B). */
    s32 ptrAbilityRangeO;     /**< +0xC0: -> offset 0x43C0 (stride 8, IDs 0x5C+). */
    s32 ptrSubTableP;         /**< +0xC4: -> offset 0x4480 (stride 24). */
    s32 ptrSubTableQ;         /**< +0xC8: -> offset 0x44F8 (stride 16). */
    s32 ptrSubTableR;         /**< +0xCC: -> offset 0x47F8 (stride 24). */
    s32 ptrSubTableS;         /**< +0xD0: -> offset 0x48B8 (stride 32). */
    s32 ptrSubTableT;         /**< +0xD4: -> offset 0x4A5C (stride 8). */
    s32 ptrSubTableU;         /**< +0xD8: -> offset 0x4A6C (stride 20). */
    u8 padDC[4];              /**< +0xDC: Unknown. */
    s32 ptrSubTableV;         /**< +0xE0: -> offset 0x4D08 (stride 2). */

    /* --- Sub-table body regions --- */
    u8 statTable8[0x138];     /**< +0xE4: AbilityEntry[39] (stride 8). */
    GfJunctionEntry junctionData[16]; /**< +0x21C: GF junction entries (16 × 60 bytes). */
    u8 junctionDataTrailing[0x99E]; /**< +0x5DC: Trailing data before ability table. */
    u8 abilityTable132[0x2628]; /**< +0xF7A: GfAbilityTableEntry[~] (stride 132). */
    u8 levelCurve12[0x18C];   /**< +0x35B8: Level curve entries (stride 12). */
    u8 elementData24[0x60];   /**< +0x3744: Element data entries (stride 24). */
    u8 xpCurves36[0x18C];    /**< +0x37A4: GfCurveEntry[11] (stride 36). */
    u8 statTable24[0x318];    /**< +0x3930: Stat table (stride 24, id < 0x21). */
    u8 statTable4[0x498];     /**< +0x3C48: Stat table (stride 4, id >= 0x21). */
    u8 abilityRangeI[0xA0];   /**< +0x40E0: AbilityEntry[20] (IDs 0x00–0x13). */
    u8 abilityRangeJ[0x98];   /**< +0x4180: AbilityEntry[19] (IDs 0x14–0x26). */
    u8 abilityRangeK[0x98];   /**< +0x4218: AbilityEntry[19] (IDs 0x27–0x39). */
    u8 abilityRangeL[0xA0];   /**< +0x42B0: AbilityEntry[20] (IDs 0x3A–0x4D). */
    u8 abilityRangeM[0x28];   /**< +0x4350: AbilityEntry[5] (IDs 0x4E–0x52). */
    u8 abilityRangeN[0x48];   /**< +0x4378: AbilityEntry[9] (IDs 0x53–0x5B). */
    u8 abilityRangeO[0xC0];   /**< +0x43C0: AbilityEntry[~] (IDs 0x5C+). */
    u8 subTableP[0x78];       /**< +0x4480: Sub-table P (stride 24). */
    u8 subTableQ[0x300];      /**< +0x44F8: Sub-table Q (stride 16). */
    u8 subTableR[0xC0];       /**< +0x47F8: Sub-table R (stride 24). */
    GfSubTableSEntry subTableS[13]; /**< +0x48B8: Sub-table S (stride 32, 13 entries). */
    u8 subTableT[0x10];       /**< +0x4A5C: Sub-table T (stride 8). */
    u8 subTableU[0x29C];      /**< +0x4A6C: Sub-table U (stride 20). */
    u8 subTableV[0x70];       /**< +0x4D08: Sub-table V (stride 2). */
} GfData; /* 3960 bytes (0x0F78) expected; see verify note below. */

/**
 * @note The body sub-table sizes above are approximate — some regions may
 *       overlap or have different actual sizes. The total should sum to
 *       3960 bytes (0x0F78). Adjust padding if needed once more offsets
 *       are confirmed.
 */

#endif /* GF_H */
