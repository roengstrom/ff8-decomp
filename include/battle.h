#ifndef BATTLE_H
#define BATTLE_H

#include "common.h"
#include "psxsdk/libgpu.h"

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
    u8 pad20[8];     /**< 0x20..0x27: Unknown fields. */
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

#endif /* BATTLE_H */
