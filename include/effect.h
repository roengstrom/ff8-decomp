#ifndef EFFECT_H
#define EFFECT_H

#include "common.h"
#include "psxsdk/libgte.h"
#include "battle.h"
#include "battle/bc_object15.h"
#include "battle/bc_object9.h"
#include "battle/bc_object11.h"
#include "battle/bc_object12.h"
#include "battle/bc_object13.h"

/**
 * @file
 * @brief Types shared by the per-action battle effect overlays.
 *
 * One overlay is streamed off the disc into a fixed slot for each battle
 * action -- magic, GF summons, limit breaks, physical and enemy attacks --
 * and entered at its first byte. See docs/battle-effect-overlays.md.
 */

/** @brief Scratchpad address the per-frame world matrix is staged at. */
#define EFFECT_SCRATCH_MATRIX 0x1F8002E0

/** @brief @ref EffectEntity::flags -- stop the script after this opcode. */
#define EFFECT_FLAG_STOP 0x1
#define EFFECT_FLAG_UNK02 0x2
/** @brief @ref EffectEntity::flags -- the script's counter reached its limit. */
#define EFFECT_FLAG_DONE 0x4
#define EFFECT_FLAG_UNK08 0x8
#define EFFECT_FLAG_UNK10 0x10
/**
 * @brief @ref EffectAnimSet::flags -- the effect's own assets are already up.
 *
 * Set, the overlay skips publishing its table and uploading its TIM.
 */
#define EFFECT_ANIMSET_FLAG_LOADED 0x1


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
    /* 0x58 */ u8 pad058[0x5A - 0x58];
    /* 0x5A */ u16 unk05A;        /**< How many strands an effect builds from it. */
    /* 0x5C */ u8 pad05C[0x63 - 0x5C];
    /* 0x63 */ u8 unk063;
    /* 0x64 */ u8 pad064[0x100 - 0x64];
} EffectModel; /* >= 0x100 */

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
    /* 0x00C */ s32 *unk00C;         /**< Head of the textured prim list, kept as a word by battle.bin. */
    /* 0x010 */ void **unk010;       /**< Head of the shaded prim list. */
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
    /* 0x0CC */ s16 unk0CC;
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
    /* 0x0F4 */ s16 unk0F4;          /**< 1 when the mesh is back-face culled against its light matrix. */
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
 * @brief The skeleton and part table an effect's mesh is drawn from.
 *
 * @c parts[0] is the part count; @c parts[1..] are the byte offsets of each
 * part's command stream, measured from the table itself.
 */
typedef struct EffectMesh {
    /* 0x00 */ EffectSkeleton *skeleton;
    /* 0x04 */ u32 *parts;
} EffectMesh;

/**
 * @brief Holder of a skeleton pointer.
 *
 * The only evidence for this type is func_801A1960's own accesses; nothing in
 * the overlay calls it, so the shape is inferred rather than corroborated.
 */
typedef struct {
    /* 0x00 */ u8 pad000[0x4];
    /* 0x04 */ EffectMesh *mesh;
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
    /* 0x01 */ u8 flags;          /**< See @c EFFECT_ANIMSET_FLAG_*. */
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
    /* 0x4C */ BattleSpriteAnim *unk04C;
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
    /* 0x6A */ u8 pad06A[0x6C - 0x6A];
    /* 0x6C */ s16 unk06C;
    /* 0x6E */ u8 pad06E[0x70 - 0x6E];
    /* 0x70 */ BattleSpriteAnim *unk070;
    /* 0x74 */ u8 pad074[0x78 - 0x74];
    /* 0x78 */ s16 unk078;
    /* 0x7A */ s16 unk07A;
    /* 0x7C */ s16 unk07C;
    /* 0x7E */ s16 unk07E;
    /* 0x80 */ u8 pad080[0xC4 - 0x80];
    /* 0xC4 */ s16 unk0C4;
    /* 0xC6 */ u8 pad0C6[0x100 - 0xC6];
} EffectEntity; /* >= 0x100 */

/** @brief One step of an effect script: the handler for a single @c pc value. */
typedef void (*EffectHandler)(EffectEntity *);

/**
 * @name battle.bin
 *
 * An effect overlay is entered from battle.bin and calls back into it. Those
 * services are declared by the battle units that own them, and the state it
 * reads by battle.h; the one exception is below.
 * @{
 */

/**
 * @brief Give the top @p size bytes of the battle scratchpad stack back.
 *
 * @note Not taken from @c battle/bc_object8.h, where it returns the new stack
 *       pointer: declaring a return value here moves the register allocation
 *       in every effect function that releases scratch, and declaring none
 *       there moves battle's own. The two builds saw different declarations.
 */

/** @} */

/**
 * @name Engine scratch
 *
 * Types every effect overlay's copy of the engine shares: the scratchpad
 * records the transform helpers work in, and the mesh stream they draw.
 * @{
 */
/** @brief Scratch a model's bounding box is accumulated in. */
typedef struct {
    /* 0x00 */ SVECTOR v;
    /* 0x08 */ VECTOR pos;
    /* 0x18 */ SVECTOR min;
    /* 0x20 */ SVECTOR max;
    /* 0x28 */ s32 flag;
} EffectBoundsScratch; /* 0x2C */

/** @brief Scratch the billboard basis is built in. */
typedef struct {
    /* 0x00 */ VECTOR delta;
    /* 0x10 */ SVECTOR up;
    /* 0x18 */ SVECTOR dir;
    /* 0x20 */ MATRIX rot;
    /* 0x40 */ MATRIX out;
    /* 0x60 */ s32 distSq;
    /* 0x64 */ s32 dist;
    /* 0x68 */ SVECTOR pos;
} EffectAimScratch; /* 0x70 */

/** @brief Scratch a rotation is built in before being folded into a matrix. */
typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ s16 sin;
    /* 0x22 */ s16 cos;
} EffectRotScratch; /* 0x24 */

/** @brief One posed mesh vertex; only the position is written here. */
typedef struct EffectMeshVertex {
    /* 0x00 */ u8 pad000[0x4];
    /* 0x04 */ SVECTOR pos;
    /* 0x0C */ u8 pad00C[0x10 - 0xC];
} EffectMeshVertex; /* 0x10 */

/** @brief One triangle of a mesh part: three 12-bit vertex indices and UVs. */
typedef struct {
    /* 0x00 */ u16 idx0;
    /* 0x02 */ u16 idx1;
    /* 0x04 */ u16 idx2;
    /* 0x06 */ u16 uv2;
    /* 0x08 */ u32 uv0;
    /* 0x0C */ u16 uv1;
    /* 0x0E */ u16 tpage;   /**< Bit 0x200 selects the semi-transparent blend. */
} EffectMeshTri; /* 0x10 */

/** @brief One quad of a mesh part. */
typedef struct {
    /* 0x00 */ u16 idx0;
    /* 0x02 */ u16 idx1;
    /* 0x04 */ u16 idx2;
    /* 0x06 */ u16 idx3;
    /* 0x08 */ u32 uv0;
    /* 0x0C */ u16 uv1;
    /* 0x0E */ u16 tpage;
    /* 0x10 */ u16 uv2;
    /* 0x12 */ u16 uv3;
} EffectMeshQuad; /* 0x14 */

/** @brief Vertex indices in a mesh part are 12 bits wide. */
#define EFFECT_MESH_INDEX_MASK 0xFFF
/** @brief @ref EffectMeshTri::tpage -- draw the textured prim semi-transparent. */
#define EFFECT_MESH_TPAGE_ABE 0x200
/** @brief Screen centre the clip rect and the projected points are offset by. */
#define EFFECT_SCREEN_CX 0xA0
/** @brief @copybrief EFFECT_SCREEN_CX */
#define EFFECT_SCREEN_CY 0x78
/** @brief A prim's tag word with no link: its length in words. */
#define EFFECT_PRIM_TAG(type) (((sizeof(type) - sizeof(u32)) / sizeof(u32)) << 24)
/** @brief A libgpu prim code in the top byte of an RGB word. */
#define EFFECT_PRIM_CODE(code) ((u32)(code) << 24)
/** @brief The 24 colour bits of that word, with the code byte taken off. */
#define EFFECT_PRIM_RGB 0xFFFFFF

/** @brief Scratch @ref func_801A1EBC works a mesh in; taken from the battle scratchpad. */
typedef struct {
    /* 0x00 */ MATRIX view;
    /* 0x20 */ MATRIX light;
    /* 0x40 */ DVECTOR sxy[4];      /**< Projected vertices, offset into the clip rect. */
    /* 0x50 */ u8 pad050[0x70 - 0x50];
    /* 0x70 */ SVECTOR normal;
    /* 0x78 */ u8 pad078[0x8C - 0x78];
    /* 0x8C */ s32 nclip;
    /* 0x90 */ s32 otz;
    /* 0x94 */ s32 idx0;
    /* 0x98 */ s32 idx1;
    /* 0x9C */ s32 idx2;
    /* 0xA0 */ s32 idx3;
    /* 0xA4 */ u16 unk0A4;
    /* 0xA6 */ u16 unk0A6;
    /* 0xA8 */ u32 colour;          /**< Shaded triangle RGB + code word. */
    /* 0xAC */ u32 unk0AC;          /**< Shaded quad RGB + code word. */
    /* 0xB0 */ u32 unk0B0;          /**< Textured triangle RGB + code word. */
    /* 0xB4 */ u32 unk0B4;          /**< Textured quad RGB + code word. */
    /* 0xB8 */ u32 visible;         /**< Copy of the slot's part mask. */
    /* 0xBC */ u16 tpage;
    /* 0xBE */ u16 clut;
    /* 0xC0 */ s16 clipX0;
    /* 0xC2 */ s16 clipX1;
    /* 0xC4 */ s16 clipY0;
    /* 0xC6 */ s16 clipY1;
    /* 0xC8 */ s16 offX;
    /* 0xCA */ s16 offY;
    /* 0xCC */ s16 reject;          /**< Set once a primitive fails the cull or clip test. */
    /* 0xCE */ s16 unk0CE;          /**< Copy of @ref EffectRender::unk0F4. */
} EffectMeshScratch; /* 0xD0 */
/** @} */

#endif /* EFFECT_H */
