#ifndef FIELD_H
#define FIELD_H

#include "common.h"

/**
 * @brief Field script entity / VM execution context.
 *
 * Used by the field engine's stack-based virtual machine. Each entity
 * has a stack of s32 values (slots 0-95), a stack pointer, result
 * registers, and various state fields for animation, movement, and
 * script execution.
 */
typedef struct {
    s32 stack[80];          /**< 0x000: Stack slots (s32 each, indexed by stackIdx). */
    s32 result;             /**< 0x140: Result/output register. */
    s32 result2;            /**< 0x144: Secondary result register. */
    u8 pad148[0x18];        /**< 0x148 */
    s32 unk160;             /**< 0x160 */
    u8 pad164[0x12];        /**< 0x164 */
    u16 unk176;             /**< 0x176 */
    u8 pad178[0x0C];        /**< 0x178 */
    u8 stackIdx;            /**< 0x184: Stack pointer index. */
    u8 pad185[0x03];        /**< 0x185 */
    u8 unk188;              /**< 0x188: Script parameter byte. */
    u8 unk189;              /**< 0x189: Script parameter byte. */
    u8 pad18A[0x06];        /**< 0x18A */
    u16 walkSpeed;          /**< 0x190: Walk speed (primary). */
    u16 walkSpeed2;         /**< 0x192: Walk speed (copy). */
    u16 runSpeed;           /**< 0x194: Run speed. */
    u8 pad196[0x06];        /**< 0x196 */
    u8 unk19C;              /**< 0x19C */
    u8 unk19D;              /**< 0x19D */
    u8 pad19E[0x58];        /**< 0x19E */
    u16 unk1F6;             /**< 0x1F6 */
    u16 unk1F8;             /**< 0x1F8 */
    u8 pad1FA[0x1E];        /**< 0x1FA */
    u8 pad218[0x06];        /**< 0x218 */
    u16 unk21E;             /**< 0x21E */
    u8 pad220[0x20];        /**< 0x220 */
    u8 unk240;              /**< 0x240: Animation/display byte. */
    u8 pad241[0x04];        /**< 0x241 */
    u8 unk245;              /**< 0x245 */
    u8 pad246[0x03];        /**< 0x246 */
    u8 unk249;              /**< 0x249 */
    u8 pad24A;              /**< 0x24A */
    u8 unk24B;              /**< 0x24B */
    u8 unk24C;              /**< 0x24C */
} FieldEntity;              /* size >= 0x24D */

/**
 * @brief Field engine global state (pointed to by D_800562C4).
 *
 * Large runtime struct for field map state. Only partially mapped;
 * fields are added as they are identified in decomped code.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x68];
    /* 0x68 */ s32 stateFlags;          /**< Field state flags (bits 3-4 checked by getFieldStateFlags). */
    /* 0x6C */ s32 soundHandle0;        /**< Sound channel handle 0. */
    /* 0x70 */ s32 soundHandle1;        /**< Sound channel handle 1 (-1 = inactive). */
    /* 0x74 */ u8 packedFlags[0x55];    /**< Packed 2-bit-per-entry flag table. */
    /* 0xC9 */ u8 soundBankSelector;    /**< Sound bank toggle (0 or 1). */
    /* 0xCA */ u8 padCA[0x0C];
    /* 0xD6 */ u8 soundLoadComplete;    /**< Set to 1 after sound bank loading finishes. */
} FieldEngineState;

/**
 * @brief Minimal position view of a field entity.
 *
 * Same underlying memory as Eline, narrowed to the 3D position triple.
 * Used by distance/message-positioning helpers that only need the entity's
 * world coordinates.
 */
typedef struct {
    /* 0x000 */ u8 pad000[0x190];
    /* 0x190 */ s32 posX;
    /* 0x194 */ s32 posY;
    /* 0x198 */ s32 posZ;
} Entity;

/**
 * @brief Eline (event line) — opcode handler view of the script context.
 *
 * Same memory as FieldEntity, but with fields named for opcode handler usage.
 * Total size: 416 bytes (0x1A0), confirmed by sizeof(eline) debug print.
 */
typedef struct {
    /* 0x000 */ u8 pad000[0x140];
    /* 0x140 */ s32 field_0x140;
    /* 0x144 */ s32 field_0x144;
    /* 0x148 */ u8 pad148[0x18];
    /* 0x160 */ s32 flags;
    /* 0x164 */ u8 pad164[0x10];
    /* 0x174 */ u8 scriptGroup;     /**< Script group index. */
    /* 0x175 */ u8 activeMask;      /**< Entity active bitmask. */
    /* 0x176 */ u8 pad176[0x0E];
    /* 0x184 */ s8 stackPtr;        /**< Bytecode stack pointer (signed, grows down). */
    /* 0x185 */ u8 pad185[0x0B];
    /* 0x190 */ s32 posX;           /**< Entity X position (fixed-point). */
    /* 0x194 */ s32 posY;           /**< Entity Y position (fixed-point). */
    /* 0x198 */ s32 field_0x198;    /**< Third component (purpose TBD). */
    /* 0x19C */ u8 pad19C[0x18];
    /* 0x1B4 */ s32 msgTextPtr;     /**< Message text pointer (fixed-point). */
    /* 0x1B8 */ s32 msgPosX;        /**< Message X position (fixed-point). */
    /* 0x1BC */ s32 msgPosY;        /**< Message Y position (fixed-point). */
    /* 0x1C0 */ s32 field_0x1C0;    /**< Saved message text pointer. */
    /* 0x1C4 */ s32 field_0x1C4;    /**< Saved message X position. */
    /* 0x1C8 */ s32 field_0x1C8;    /**< Saved message Y position. */
    /* 0x1CC */ u8 pad1CC[0x0E];
    /* 0x1DA */ u16 field_0x1DA;
    /* 0x1DC */ u8 pad1DC[0x22];
    /* 0x1FE */ s16 savedChannel;   /**< Previous message channel. */
    /* 0x200 */ u16 msgChannel;     /**< Current message channel. */
    /* 0x202 */ u16 field_0x202;    /**< Saved channel for async restore. */
    /* 0x204 */ s16 field_0x204;
    /* 0x206 */ u8 pad206[0x14];
    /* 0x21A */ u16 windowId;       /**< Message window ID. */
    /* 0x21C */ u16 field_0x21C;    /**< Saved window ID for async restore. */
    /* 0x21E */ s16 msgState;       /**< Message state (0=init, 2=complete). */
    /* 0x220 */ u8 pad220[0x1C];
    /* 0x23C */ u8 msgActive;       /**< Message active flag. */
    /* 0x23D */ u8 pad23D[0x03];
    /* 0x240 */ u8 field_0x240;
    /* 0x241 */ u8 pad241[0x0D];
    /* 0x24E */ u8 field_0x24E;
    /* 0x24F */ u8 field_0x24F;
    /* 0x250 */ u8 field_0x250;
    /* 0x251 */ u8 field_0x251;
    /* 0x252 */ u8 pad252[0x04];
    /* 0x256 */ u8 field_0x256;
    /* 0x257 */ u8 pad257[0x0B];
    /* 0x262 */ u8 field_0x262;
} Eline;

/** @brief Pop one s32 from the eline's bytecode stack. */
#define POP(eline) (((s32 *)(eline))[(s8)(eline)->stackPtr--])

/** @brief Pop one s32 then read low byte only. */
#define POP_BYTE(eline) (*(u8 *)&POP(eline))

/** @brief Read the top s32 from the eline's bytecode stack without popping. */
#define PEEK(eline) (((s32 *)(eline))[(eline)->stackPtr])

#endif /* FIELD_H */
