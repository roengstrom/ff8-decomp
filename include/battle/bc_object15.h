#ifndef BATTLE_BC_OBJECT15_H
#define BATTLE_BC_OBJECT15_H

#include "common.h"
#include "psxsdk/libgte.h"

/**
 * @file
 * @brief Battle sprite primitives: animated sprite sets drawn from a table.
 */

/**
 * @brief A sprite animation: a CLUT row table, then the frames as byte offsets
 *        from the table itself. Each frame is a sprite count followed by the
 *        sprites; bit 31 of the count is a flag the renderer carries into
 *        @ref BattleSpritePrim::flags.
 */
typedef struct {
    /* 0x00 */ u8 unk000[3];
    /* 0x03 */ s8 flag;            /**< Non-zero while the set is not usable. */
    /* 0x04 */ u8 clutRow[4];      /**< Default for @ref BattleSpritePrim::clutRow. */
    /* 0x08 */ u16 frameCount;
    /* 0x0A */ u16 offsets[1];     /**< One per frame, plus a terminator. */
} BattleSpriteAnim;

/**
 * @brief One sprite of a frame. The first two words are laid out exactly as a
 *        POLY_FT4's colour word and first texture word, and are copied as such.
 */
typedef struct {
    /* 0x00 */ u8 w;               /**< Size in pixels. */
    /* 0x01 */ u8 shade;           /**< Brightness, scaled by the packet's colour per channel. */
    /* 0x02 */ u8 h;
    /* 0x03 */ u8 code;            /**< Primitive code, with the blend bit. */
    /* 0x04 */ u8 u;
    /* 0x05 */ u8 v;
    /* 0x06 */ u16 clut;
    /* 0x08 */ s16 x;              /**< Top-left corner in pixels, in the set's frame. */
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 angle;
    /* 0x0E */ s16 flags;          /**< Texture page, transform enable, CLUT row select. */
    /* 0x10 */ s16 scaleX;         /**< ONE is unscaled. */
    /* 0x12 */ s16 scaleY;
} BattleSprite; /* 0x14 */

/** @brief @ref BattleSprite::code -- the sprite is drawn semi-transparent. */
#define BATTLE_SPRITE_CODE_BLEND 0x2

/** @brief @ref BattleSprite::flags bits 0-8: the texture page. */
#define BATTLE_SPRITE_TPAGE_MASK 0x1FF
/** @brief @ref BattleSprite::flags bits 10-11: the sprite carries its own angle and scale. */
#define BATTLE_SPRITE_TRANSFORMED 0xC00
/** @brief @ref BattleSprite::flags bits 12-15: which @ref BattleSpritePrim::clutRow it uses. */
#define BATTLE_SPRITE_CLUT_ROW_SHIFT 12

/** @brief Bit 31 of a frame's sprite count: draw the set additively. */
#define BATTLE_SPRITE_COUNT_FLAG 0x80000000
/** @brief Colour a packet is reset to unless it keeps its own: mid grey, code clear. */
#define BATTLE_SPRITE_COLOUR_DEFAULT 0x808080

/** @brief @ref BattleSpritePrim::flags -- roll the sprites by @c angle. */
#define BATTLE_SPRITE_FLAG_ROTATE 0x1
/** @brief @ref BattleSpritePrim::flags -- scale the sprites by @c scale. */
#define BATTLE_SPRITE_FLAG_SCALE 0x2
/** @brief @ref BattleSpritePrim::flags -- keep @c colour; otherwise it is reset to mid grey. */
#define BATTLE_SPRITE_FLAG_COLOUR 0x4
/** @brief @ref BattleSpritePrim::flags -- keep the right and bottom texture edges one texel in. */
#define BATTLE_SPRITE_FLAG_UV_INSET 0x8
/** @brief @ref BattleSpritePrim::flags -- keep @c clutRow[0..3]; otherwise each comes from the animation. */
#define BATTLE_SPRITE_FLAG_KEEP_CLUT0 0x10
#define BATTLE_SPRITE_FLAG_KEEP_CLUT1 0x20
#define BATTLE_SPRITE_FLAG_KEEP_CLUT2 0x40
#define BATTLE_SPRITE_FLAG_KEEP_CLUT3 0x80
/** @brief @ref BattleSpritePrim::flags -- the frame's count word carried @ref BATTLE_SPRITE_COUNT_FLAG. */
#define BATTLE_SPRITE_FLAG_ADDITIVE 0x100
/** @brief @ref BattleSpritePrim::flags -- skip the default roll by @ref D_800F02A0. */
#define BATTLE_SPRITE_FLAG_NO_ROLL 0x200

/**
 * @brief The packet an animated sprite set is drawn from; taken from the battle
 *        scratch stack for the call.
 *
 * The caller fills @c anim, @c frame, @c flags and, when it keeps them,
 * @c colour and @c clutRow, and loads the GTE matrices the sprites are placed
 * through; @ref func_800C9E10 fills the rest.
 */
typedef struct {
    /* 0x00 */ BattleSpriteAnim *anim;
    /* 0x04 */ u16 frame;
    /* 0x06 */ u8 pad006[0x8 - 0x6];
    /* 0x08 */ s32 angle;
    /* 0x0C */ VECTOR scale;       /**< @c vz is forced to ONE. */
    /* 0x1C */ CVECTOR colour;     /**< Scales each sprite's shade, per channel out of 128. */
    /* 0x20 */ u8 clutRow[4];      /**< CLUT row offset for each sprite CLUT row select. */
    /* 0x24 */ u16 flags;          /**< See @c BATTLE_SPRITE_FLAG_*. */
    /* 0x26 */ u16 frameCount;
    /* 0x28 */ u16 nextOffset;     /**< Offset of the frame after the one drawn. */
    /* 0x2A */ u8 pad02A[0x2C - 0x2A];
    /* 0x2C */ BattleSprite *sprites; /**< First sprite of the frame drawn. */
    /* 0x30 */ s32 spriteCount;
    /* 0x34 */ u8 pad034[0x3C - 0x34];
    /* 0x3C */ s32 otz;            /**< Depth of the set; -1 until its first sprite is projected. */
    /* 0x40 */ u32 gteFlag;
    /* 0x44 */ SVECTOR corners[4]; /**< The sprite being placed, before its matrix. */
    /* 0x64 */ MATRIX m;           /**< The sprite's own rotation, scale and position, composed onto @c mtx. */
    /* 0x84 */ MATRIX mtx;         /**< The GTE's rotation and translation on entry, rolled and scaled. */
    /* 0xA4 */ s32 cos;            /**< rcos/rsin of @c lastAngle, so a run of equal angles costs one pair. */
    /* 0xA8 */ s32 sin;
    /* 0xAC */ s32 lastAngle;
    /* 0xB0 */ s32 uvInset;        /**< 1 to keep the right and bottom texture edges one texel in. */
} BattleSpritePrim; /* 0xB4 */

/**
 * @brief The animation at @p index of the battle's table.
 * @return That animation, or the table's first if it is not usable yet.
 */
BattleSpriteAnim *func_800C94B8(s32 index);

/** @brief Load the GTE matrices a prim at @p pos is emitted through. */
void func_800C96E4(SVECTOR *pos, s32 scale, s16 angle);

/**
 * @brief Draw one frame of a sprite animation into @p ot.
 * @param otShift Shift from the set's averaged depth to its OT index.
 * @return The prim buffer cursor past the sprites emitted.
 */
void *func_800C9E10(BattleSpritePrim *prim, u32 *ot, s32 otShift, void *head);

#endif /* BATTLE_BC_OBJECT15_H */
