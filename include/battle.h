#ifndef BATTLE_H
#define BATTLE_H

#include "common.h"
#include "psxsdk/libgpu.h"

/**
 * @brief Animation frame sub-entry within BattleAnimEntity (stride 20 bytes).
 *
 * Each BattleAnimEntity has a circular buffer of 8 frames, indexed by
 * (frameCounter - offset) & 7.
 */
typedef struct {
    u8 field00;      /**< 0x00: Active flag (nonzero = occupied). */
    u8 field01;      /**< 0x01: Type/flags (upper 4 bits = animation type). */
    u16 field02;     /**< 0x02: Parameter value. */
    u8 field04;      /**< 0x04: Direction value. */
    u8 field05;      /**< 0x05: Unknown byte. */
    u8 field06;      /**< 0x06: Unknown byte. */
    u8 field07;      /**< 0x07: Unknown byte. */
    u16 field08;     /**< 0x08: Unknown u16. */
    u16 field0A;     /**< 0x0A: Unknown u16. */
    u16 field0C;     /**< 0x0C: Unknown u16. */
    u16 field0E;     /**< 0x0E: Unknown u16. */
    u16 field10;     /**< 0x10: Unknown u16. */
    u16 field12;     /**< 0x12: Unknown u16. */
} AnimFrame;         /* 0x14 = 20 bytes */

/**
 * @brief Battle animation entity (g_battleAnims, stride 196 bytes).
 *
 * Controls animation state for a battle participant. Contains a circular
 * buffer of 8 AnimFrame sub-entries and links to a secondary entity via
 * linkedIdx. The array at g_battleAnims holds 2 active entities (indexed
 * with a0 & 1) plus extra global state beyond offset 0x188.
 */
typedef struct {
    u8 pad00[6];          /**< 0x00..0x05: Unknown fields. */
    u8 field06;           /**< 0x06: Unknown byte. */
    u8 field07;           /**< 0x07: Unknown byte. */
    u8 pad08[2];          /**< 0x08..0x09: Unknown fields. */
    u8 field0A;           /**< 0x0A: Active flag (1 = active). */
    u8 field0B;           /**< 0x0B: Unknown byte. */
    u8 field0C;           /**< 0x0C: Default color value. */
    u8 field0D;           /**< 0x0D: Default color value. */
    u8 field0E;           /**< 0x0E: Default color value. */
    u8 field0F;           /**< 0x0F: Default color value. */
    s16 unk10[4];         /**< 0x10..0x17: s16 array (set via func_800281A4). */
    u8 frameCounter;      /**< 0x18: Frame counter (circular, & 7). */
    u8 field19;           /**< 0x19: Active flag (1 = active). */
    s8 field1A;           /**< 0x1A: State indicator (0, 2, 6 tested). */
    u8 opacity;           /**< 0x1B: Opacity (0xFF = visible, 0 = hidden). */
    AnimFrame frames[8];  /**< 0x1C..0xBB: 8 animation frame sub-entries. */
    u8 padBC[6];          /**< 0xBC..0xC1: Unknown fields. */
    u8 linkedIdx;         /**< 0xC2: Index into same array (linked entity). */
    u8 fieldC3;           /**< 0xC3: Control (bit 7 = mirror, low 7 = intensity). */
} BattleAnimEntity;       /* 0xC4 = 196 bytes */

/**
 * @brief Battle display entity (D_80083210, stride 64 bytes).
 *
 * Each entity represents a visual element in the battle scene (characters,
 * enemies, effects). The array at D_80083210 holds all active entities.
 */
typedef struct {
    s32 field00;        /**< 0x00: Pointer or ID (first word). */
    s32 field04;        /**< 0x04: Pointer or ID (second word). */
    RECT boundRect;     /**< 0x08: Bounding rectangle. */
    RECT dispRect;      /**< 0x10: Display rectangle. */
    u8 pad18[0x18];     /**< 0x18..0x2F: Unknown fields. */
    s32 drawMode;       /**< 0x30: Draw mode (0x38000000 or 0x3A000000). */
    u8 activeFlag;      /**< 0x34: Active/type indicator (0 = inactive). */
    u8 field35;         /**< 0x35: Unknown byte. */
    u8 field36;         /**< 0x36: Unknown byte. */
    u8 animSpeed;       /**< 0x37: Animation speed/priority (clamped 3..11). */
    u8 entityType;      /**< 0x38: Entity type; bit 0 selects drawMode. */
    u8 pad39;           /**< 0x39: Padding. */
    u8 subFields[2];    /**< 0x3A: Sub-fields indexed by offset param. */
    s16 scale;          /**< 0x3C: Scale factor (0x1000 = 1.0). */
    s16 pad3E;          /**< 0x3E: Padding / unused. */
} BattleDisplayEntity;  /* 0x40 = 64 bytes */

/**
 * @brief SFX/visual effect entry (D_80082FF0, stride 60 bytes).
 *
 * Each entry controls a visual effect (animation, sprite, particle) linked
 * to a BattleDisplayEntity via the entityIdx field.
 */
typedef struct {
    RECT rect;       /**< 0x00: Source rectangle for the effect. */
    u8 pad08[8];     /**< 0x08..0x0F: Unknown fields. */
    s16 pitch;       /**< 0x10: Pitch / playback rate. */
    u8 pad12[2];     /**< 0x12..0x13: Padding. */
    s16 field14;     /**< 0x14: Unknown s16 (cleared on init). */
    u8 state;        /**< 0x16: State (0 = inactive, 1 = active). */
    u8 pad17;        /**< 0x17: Padding. */
    u8 entityIdx;    /**< 0x18: Index into BattleDisplayEntity array. */
    u8 field19;      /**< 0x19: Unknown byte (cleared on init). */
    s16 volume;      /**< 0x1A: Volume / intensity (0x1000 = default). */
    s16 field1C;     /**< 0x1C: Unknown s16. */
    s16 rateDelta;   /**< 0x1E: Rate of change (negative = fade out). */
    u8 field20;      /**< 0x20: Data stream position index. */
    u8 field21;      /**< 0x21: Repeat / loop counter. */
    u8 field22;      /**< 0x22: Frame tick counter. */
    u8 field23;      /**< 0x23: Loop count limit (compared against field21). */
    s32 seqState;    /**< 0x24: Sequence interpreter state (0..0x11). */
    u8 field28;      /**< 0x28: Unknown byte. */
    u8 field29;      /**< 0x29: Unknown byte. */
    u8 field2A;      /**< 0x2A: Unknown byte. */
    u8 field2B;      /**< 0x2B: Unknown byte. */
    u8 field2C;      /**< 0x2C: Unknown byte. */
    u8 mode;         /**< 0x2D: Playback mode. */
    u8 pad2E;        /**< 0x2E: Padding. */
    u8 field2F;      /**< 0x2F: Unknown byte (cleared on init). */
    u16 field30;     /**< 0x30: Unknown u16. */
    u8 field32;      /**< 0x32: Unknown byte. */
    u8 pad33;        /**< 0x33: Padding. */
    s32 field34;     /**< 0x34: Unknown s32. */
    s32 field38;     /**< 0x38: Unknown s32. */
} SfxEntry;          /* 0x3C = 60 bytes */

/** @brief Bit flags for BattleEntity::controlFlags. */
typedef enum {
    CTRL_ACTIVE     = 0x01,  /**< Entity is active. */
    CTRL_FLAG_02    = 0x02,  /**< Unknown. */
    CTRL_FLAG_10    = 0x10,  /**< Unknown. */
    CTRL_FLAG_40    = 0x40,  /**< Unknown. */
    CTRL_FLAG_80    = 0x80,  /**< Unknown. */
    CTRL_FLAG_100   = 0x100  /**< Unknown. */
} ControlFlags;

/**
 * @brief Battle entity (D_800ED148 array, stride 0xD0 = 208 bytes).
 *
 * Up to 7 entities in the battle scene. Each entity tracks state,
 * flags, status effects, animation params, and linked references.
 */
typedef struct {
    u8 pad00[0x04];    /**< 0x00..0x03: Unknown. */
    s32 state;         /**< 0x04: Entity state. */
    u8 pad08[0x04];    /**< 0x08..0x0B: Unknown. */
    u8 timer;          /**< 0x0C: Timer. */
    u8 control;        /**< 0x0D: Control byte. */
    u8 pad0E;          /**< 0x0E: Unknown. */
    u8 entityRef;      /**< 0x0F: Entity index reference. */
    s32 linkedPtr;     /**< 0x10: Linked entity pointer. */
    u8 pad14[0x04];    /**< 0x14..0x17: Unknown. */
    s32 flags;         /**< 0x18: Feature flags (bitmask). */
    s32 flagsBackup;   /**< 0x1C: Flags backup. */
    u8 pad20[0x08];    /**< 0x20..0x27: Unknown. */
    s32 field28;       /**< 0x28: Unknown (checked for zero). */
    u8 pad2C[0x58];    /**< 0x2C..0x83: Unknown. */
    u16 animParam1;    /**< 0x84: Animation parameter 1. */
    u16 animParam2;    /**< 0x86: Animation parameter 2. */
    u16 animParam3;    /**< 0x88: Animation parameter 3. */
    u8 pad8A[0x02];    /**< 0x8A..0x8B: Unknown. */
    ControlFlags controlFlags;  /**< 0x8C: Control flags (see ControlFlags enum). */
    u16 status;        /**< 0x90: Status effects (bitmask). */
    u16 statusBackup;  /**< 0x92: Status effects backup. */
    u8 pad94[0x27];    /**< 0x94..0xBA: Unknown. */
    u8 linkedIdx2;     /**< 0xBB: Linked entity index. */
    u8 padBC[0x0F];    /**< 0xBC..0xCA: Unknown. */
    u8 linkedIdx;      /**< 0xCB: Linked index (0xFF = none). */
    u8 padCC[0x04];    /**< 0xCC..0xCF: Unknown. */
} BattleEntity;        /* 0xD0 = 208 bytes */

/**
 * @brief Battle character render data (g_battleChars, stride 0x1D0 = 464 bytes).
 *
 * Contains display/render state for each battle participant.
 * Linked to BattleEntity (D_800ED148) for core battle logic.
 */
typedef struct {
    u8 pad000[0x1C3];     /**< 0x000..0x1C2: Render state, animation, etc. */
    u8 characterId;        /**< 0x1C3: Character ID (see CharacterId). */
    u8 pad1C4[0x0C];      /**< 0x1C4..0x1CF: Unknown. */
} BattleCharData;          /* 0x1D0 = 464 bytes */

extern BattleCharData g_battleChars[];

#endif /* BATTLE_H */
