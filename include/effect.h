#ifndef EFFECT_H
#define EFFECT_H

#include "common.h"
#include "psxsdk/libgte.h"

/**
 * @file
 * @brief Types shared by the per-action battle effect overlays.
 *
 * One overlay is streamed off the disc into a fixed slot for each battle
 * action -- magic, GF summons, limit breaks, physical and enemy attacks --
 * and entered at its first byte. See docs/battle-effect-overlays.md.
 */

/** @brief A second skeleton hung off a battle slot. */
struct EffectAttachment {
    /* 0x00 */ u8 pad000[0x4];
    /* 0x04 */ struct EffectSkeleton **skeleton;
};

/**
 * @brief Battle-side per-entity record, as the effect overlays index it.
 *
 * battle.bin declares this symbol @c u8[] and indexes it with an explicit
 * @c 0x9C stride. Retyping it there would change that overlay's codegen, so
 * the effect overlays carry their own view of the same record.
 */
typedef struct BattleEffectSlot {
    /* 0x00 */ u16 flags;          /**< See @c BATTLE_SLOT_FLAG_*. */
    /* 0x02 */ u8 pad002[0x1E - 0x2];
    /* 0x1E */ u16 unk01E;         /**< Y origin: summed with a model's Y bounds. */
    /* 0x20 */ u8 pad020[0x24 - 0x20];
    /* 0x24 */ u16 unk024;
    /* 0x26 */ u8 pad026[0x40 - 0x26];
    /* 0x40 */ MATRIX mtx;         /**< Pose the effect's render matrices start from. */
    /* 0x60 */ u8 unk060[0x64 - 0x60];
    /* 0x64 */ struct EffectSkeleton **skeleton;
    /* 0x68 */ u8 pad068[0x6C - 0x68];
    /* 0x6C */ u8 unk06C[0x78 - 0x6C];
    /* 0x78 */ struct EffectAttachment *unk078;
    /* 0x7C */ u8 pad07C[0x9C - 0x7C];
} BattleEffectSlot; /* 0x9C */

extern BattleEffectSlot D_800EF2D0[];

/** @brief @ref EffectEntity::flags -- stop the script after this opcode. */
/** @brief Scratchpad address the per-frame world matrix is staged at. */
#define EFFECT_SCRATCH_MATRIX 0x1F8002E0

#define EFFECT_FLAG_STOP 0x1
#define EFFECT_FLAG_UNK02 0x2
/** @brief @ref EffectEntity::flags -- the script's counter reached its limit. */
#define EFFECT_FLAG_DONE 0x4
#define EFFECT_FLAG_UNK08 0x8
#define BATTLE_SLOT_FLAG_UNK02 0x2
#define BATTLE_SLOT_FLAG_UNK04 0x4
#define BATTLE_SLOT_FLAG_UNK20 0x20

/**
 * @brief The geometry an effect script drives: three vectors and a bounding box.
 *
 * Reached through @ref EffectEntity::unk010 and @ref EffectEntity::unk014.
 */
typedef struct EffectModel {
    /* 0x00 */ u8 pad000[0x30];
    /* 0x30 */ SVECTOR unk030;
    /* 0x38 */ SVECTOR unk038;
    /* 0x40 */ SVECTOR unk040;
    /* 0x48 */ SVECTOR boundsMin; /**< Lower corner of the model's bounding box. */
    /* 0x50 */ SVECTOR boundsMax; /**< Upper corner of the model's bounding box. */
    /* 0x58 */ u8 pad058[0x63 - 0x58];
    /* 0x63 */ u8 unk063;
    /* 0x64 */ u8 pad064[0x100 - 0x64];
} EffectModel; /* >= 0x100 */

/**
 * @brief The battle renderer's graphics context, as the effect overlays use it.
 *
 * Only the two members the effect overlays reach for are named; the record
 * itself belongs to battle.bin.
 */
typedef struct {
    /* 0x00 */ u8 pad000[0x14];
    /* 0x0014 */ u8 font[0x44 - 0x14];   /**< Glyph set handed to the text drawer. */
    /* 0x0044 */ u8 ot[0x4040 - 0x44];   /**< Ordering table the effects link into. */
    /* 0x4040 */ u8 unk4040[4];
} BattleGfx;

/** @brief GPU primitive an effect hands to the battle renderer. */
typedef struct {
    /* 0x00 */ void *unk000;
    /* 0x04 */ u16 unk004;
    /* 0x06 */ u8 pad006[0x1C - 0x6];
    /* 0x1C */ CVECTOR unk01C;
    /* 0x20 */ u8 pad020[0x24 - 0x20];
    /* 0x24 */ u16 unk024;
} EffectPrim;

/** @brief One member of an animated part list; stride 24. */
typedef struct {
    /* 0x00 */ u8 unk000;
    /* 0x01 */ u8 pad001[0x18 - 0x1];
} EffectPart; /* 0x18 */

/** @brief Per-part render state hung off @ref EffectRender. */
typedef struct {
    /* 0x00 */ u8 pad000[0x4];
    /* 0x04 */ void *unk004;
    /* 0x08 */ u8 pad008[0x14 - 0x8];
    /* 0x14 */ s16 unk014;
    /* 0x16 */ s16 unk016;
    /* 0x18 */ s16 unk018;
    /* 0x1A */ s16 unk01A;
    /* 0x1C */ u8 r;
    /* 0x1D */ u8 g;
    /* 0x1E */ u8 b;
    /* 0x1F */ u8 pad01F;
    /* 0x20 */ s32 unk020;
    /* 0x24 */ s16 unk024;
} EffectRenderPart;

/** @brief Render state an effect hands to the battle model renderer. */
typedef struct {
    /* 0x000 */ void *unk000;
    /* 0x004 */ EffectRenderPart *part;
    /* 0x008 */ struct BattleEffectSlot *slot;
    /* 0x00C */ s32 *unk00C;
    /* 0x010 */ void *unk010;
    /* 0x014 */ MATRIX unk014;
    /* 0x034 */ MATRIX unk034;
    /* 0x054 */ MATRIX unk054;
    /* 0x074 */ MATRIX unk074;
    /* 0x094 */ u8 pad094[0xA4 - 0x94];
    /* 0x0A4 */ SVECTOR unk0A4;
    /* 0x0AC */ SVECTOR unk0AC;
    /* 0x0B4 */ SVECTOR unk0B4;
    /* 0x0BC */ SVECTOR unk0BC;
    /* 0x0C4 */ u8 r;
    /* 0x0C5 */ u8 g;
    /* 0x0C6 */ u8 b;
    /* 0x0C7 */ u8 pad0C7;
    /* 0x0C8 */ s32 scale;
    /* 0x0CC */ u8 pad0CC[0xCE - 0xCC];
    /* 0x0CE */ s16 unk0CE;
    /* 0x0D0 */ u8 pad0D0[0xD2 - 0xD0];
    /* 0x0D2 */ s16 unk0D2;
    /* 0x0D4 */ u8 pad0D4[0xD8 - 0xD4];
    /* 0x0D8 */ s16 unk0D8;
    /* 0x0DA */ s16 unk0DA;
    /* 0x0DC */ s16 unk0DC;
    /* 0x0DE */ s16 unk0DE;
    /* 0x0E0 */ s16 unk0E0;
    /* 0x0E2 */ u8 pad0E2[0xE4 - 0xE2];
    /* 0x0E4 */ s16 unk0E4;
    /* 0x0E6 */ s16 unk0E6;
    /* 0x0E8 */ s16 unk0E8;
    /* 0x0EA */ s16 unk0EA;
    /* 0x0EC */ u16 tpage;
    /* 0x0EE */ u16 clut;
    /* 0x0F0 */ u8 pad0F0[0xF2 - 0xF0];
    /* 0x0F2 */ s16 unk0F2;
} EffectRender;

/** @brief A spark task spawned by the scatter opcodes; stride 0x5C. */
typedef struct {
    /* 0x00 */ u8 pad000[0x1C];
    /* 0x1C */ SVECTOR pos;
    /* 0x24 */ s16 unk024;
    /* 0x26 */ u16 flags;          /**< See @c EFFECT_FLAG_*. */
    /* 0x28 */ u8 wait;
    /* 0x29 */ u8 pc;
    /* 0x2A */ u8 pad02A[0x30 - 0x2A];
    /* 0x30 */ SVECTOR unk030;     /**< Spin applied to the spark's quad. */
    /* 0x38 */ s16 unk038;         /**< Half-width of the quad, in world units. */
    /* 0x3A */ s16 unk03A;         /**< Half-height of the quad. */
    /* 0x3C */ s16 unk03C;
    /* 0x3E */ s16 unk03E;
    /* 0x40 */ s16 unk040;
    /* 0x42 */ s16 unk042;
    /* 0x44 */ u8 unk044;           /**< Running colour, ramped by @c r each step. */
    /* 0x45 */ u8 unk045;
    /* 0x46 */ u8 unk046;
    /* 0x47 */ u8 pad047;
    /* 0x48 */ u8 unk048;           /**< Second running colour, ramped by @c unk050. */
    /* 0x49 */ u8 unk049;
    /* 0x4A */ u8 unk04A;
    /* 0x4B */ u8 pad04B;
    /* 0x4C */ u8 r;
    /* 0x4D */ u8 g;
    /* 0x4E */ u8 b;
    /* 0x4F */ u8 pad04F;
    /* 0x50 */ u8 unk050;
    /* 0x51 */ u8 unk051;
    /* 0x52 */ u8 unk052;
    /* 0x53 */ u8 pad053;
    /* 0x54 */ s16 unk054;          /**< Frames left in the current phase. */
    /* 0x56 */ s16 unk056;
    /* 0x58 */ s16 unk058;
    /* 0x5A */ u8 pad05A[0x5C - 0x5A];
} EffectSpark; /* 0x5C */

/** @brief One animated joint: a local matrix at a fixed stride. */
typedef struct {
    /* 0x00 */ u8 pad000[0x2];
    /* 0x02 */ s16 unk002;
    /* 0x04 */ u8 pad004[0x10 - 0x4];
    /* 0x10 */ MATRIX mtx;
} EffectJoint; /* 0x30 */

/** @brief Joint count followed by the joints themselves. */
typedef struct EffectSkeleton {
    /* 0x00 */ u8 count;
    /* 0x01 */ u8 pad001[0x10 - 0x1];
    /* 0x10 */ EffectJoint joints[1];
} EffectSkeleton;

/**
 * @brief Holder of a skeleton pointer.
 *
 * The only evidence for this type is func_801A1960's own accesses; nothing in
 * the overlay calls it, so the shape is inferred rather than corroborated.
 */
typedef struct {
    /* 0x00 */ u8 pad000[0x4];
    /* 0x04 */ struct EffectSkeleton **skeleton;
} EffectSkeletonRef;

/** @brief The pair of matrices an effect is posed through. */
typedef struct {
    /* 0x00 */ u8 pad000[0x34];
    /* 0x34 */ MATRIX view;
    /* 0x54 */ u8 pad054[0x74 - 0x54];
    /* 0x74 */ MATRIX world;
} EffectPose;

/** @brief One animation entry: a part list plus its header; stride 20. */
typedef struct {
    /* 0x00 */ u8 unk000;
    /* 0x01 */ u8 pad001[0x8 - 0x1];
    /* 0x08 */ EffectPart *parts;
    /* 0x0C */ u8 pad00C[0x10 - 0xC];
    /* 0x10 */ u8 unk010;
    /* 0x11 */ u8 unk011;
    /* 0x12 */ u8 pad012[0x14 - 0x12];
} EffectAnim; /* 0x14 */

/** @brief Owner of an effect's animation table. */
typedef struct {
    /* 0x00 */ u8 slot;            /**< Battle slot the effect is attached to. */
    /* 0x01 */ u8 unk001;
    /* 0x02 */ u8 pad002[0x4 - 0x2];
    /* 0x04 */ EffectAnim *anims;
} EffectAnimSet;

/**
 * @brief One running effect script.
 *
 * Handed to every opcode handler in the overlay's dispatch tables. Distinct
 * from @ref EffectModel: no handler ever touches @c pc and the bounding box
 * through the same pointer, which is what separates the two.
 */
typedef struct EffectEntity {
    /* 0x00 */ u8 pad000[0xC];
    /* 0x0C */ EffectAnimSet *animSet;
    /* 0x10 */ struct EffectModel *unk010;
    /* 0x14 */ struct EffectModel *unk014;
    /* 0x18 */ struct EffectEntity *unk018;
    /* 0x1C */ SVECTOR pos;        /**< Advanced each frame by unk058/unk05A/unk05C. */
    /* 0x24 */ s16 unk024;
    /* 0x26 */ u16 flags;          /**< See @c EFFECT_FLAG_*. */
    /* 0x28 */ u8 wait;            /**< Live children; func_801A0B00 bumps it per spawn
                                        and the script only drains at zero. */
    /* 0x29 */ u8 pc;              /**< Advanced by one per opcode consumed. */
    /* 0x2A */ u8 unk02A;          /**< Loop counter, compared against @c unk058. */
    /* 0x2B */ u8 unk02B;          /**< Index into the selected animation's part list. */
    /* 0x2C */ u8 unk02C;
    /* 0x2D */ u8 unk02D;          /**< Battle slot index. */
    /* 0x2E */ u8 unk02E;
    /* 0x2F */ u8 unk02F;
    /* 0x30 */ SVECTOR unk030;
    /* 0x38 */ SVECTOR unk038;    /**< Midpoint of unk040 and the slot's second point. */
    /* 0x40 */ SVECTOR unk040;
    /* 0x48 */ u8 unk048[0x4C - 0x48]; /**< func_801A101C writes a bounding box
                                           across this and @c unk04C. */
    /* 0x4C */ void *unk04C;
    /* 0x50 */ s16 unk050;
    /* 0x52 */ s16 unk052;
    /* 0x54 */ s16 unk054;
    /* 0x56 */ u8 pad056[0x58 - 0x56];
    /* 0x58 */ s16 unk058;
    /* 0x5A */ s16 unk05A;
    /* 0x5C */ s16 unk05C;
    /* 0x5E */ s16 unk05E;
    /* 0x60 */ u8 unk060[0x63 - 0x60]; /**< func_801A3C14 aims an SVECTOR across
                                          this and @c unk063. */
    /* 0x63 */ u8 unk063;
    /* 0x64 */ u8 pad064[0x68 - 0x64];
    /* 0x68 */ s16 unk068;
    /* 0x6A */ u8 pad06A[0x100 - 0x6A];
} EffectEntity; /* >= 0x100 */

/** @brief One step of an effect script: the handler for a single @c pc value. */
typedef void (*EffectHandler)(EffectEntity *);

#endif /* EFFECT_H */
