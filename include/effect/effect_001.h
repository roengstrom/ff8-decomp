#ifndef EFFECT_EFFECT_001_H
#define EFFECT_EFFECT_001_H

#include "effect.h"

/**
 * @brief Entry point of the effect_001 overlay.
 *
 * battle.bin calls this at the overlay's load address once the image is in
 * place; it builds the effect's root entity and its task pools.
 *
 * @param animSet Animation table the effect draws its models from.
 * @return The task pool the effect's scripts run out of.
 */
void *func_801A0000(EffectAnimSet *animSet);

/**
 * @brief Pose each joint of @p skeleton and link its parts into @p ot.
 *
 * @param skeleton Skeleton to walk, as the battle slot holds it.
 * @param ot       Ordering table the posed parts are linked into.
 * @param mode     Prim mode passed through to the renderer.
 * @param render   Render state the joints are posed through.
 */
void func_801A1EBC(EffectSkeleton **skeleton, u8 *ot, s32 mode,
                   EffectRender *render);

/**
 * @name Overlay data
 *
 * The blob carried in the overlay image behind the code, from @c 0x801A4678
 * to the image end.
 * @{
 */

/** @brief AKAO sound sequence, played on the script's first frame. */
extern u8 D_801A4678[];

/** @brief Table of two offsets battle.bin resolves when the effect starts. */
extern u8 D_801A475C;

/**
 * @brief TIM uploaded to VRAM when the effect starts.
 *
 * Once it is in VRAM the image memory is reused: the three 0x10000-byte prim
 * and frame banks the renderer double-buffers through start at this address.
 */
extern u8 D_801A4F0C;

/**
 * @name Prim source tables
 *
 * Packed blobs a script step hands to its entity through
 * @ref EffectEntity::unk04C; the entity's prim carries the pointer straight
 * through to the battle renderer as @ref EffectPrim::unk000.
 * @{
 */

/** @brief Table the aim-at-target step draws through. */
extern s32 D_801C532C;

/** @brief Table the effect switches to once its timer passes 20. */
extern s32 D_801C5380;

/** @brief Table the downward-drift step draws through. */
extern s32 D_801C5410;

/** @brief Table the scatter step draws through. */
extern s32 D_801C55EC;

/** @brief Table drawn with an explicit tint rather than the model's own. */
extern u8 D_801C58B8[];

/** @} */
/** @} */

/**
 * @name Overlay bss
 *
 * Working storage past the end of the overlay image (@c 0x801C58E0). Each task
 * pool's 0x10-byte header sits directly behind the entries it hands out.
 * @{
 */

/** @brief Cleared when the effect starts; nothing in the overlay reads them. */
extern s32 D_801C58E0;
extern s32 D_801C58E4;

/** @brief The frame's world matrix, staged in the scratchpad. */
extern MATRIX *D_801C58E8;
extern MATRIX *D_801C58EC;

/** @brief Head of the space left in the frame banks. */
extern u8 *D_801C58F0;

/** @brief Cursors this frame's prims and frames are written through. */
extern void *D_801C58F4;
extern void *D_801C58F8;

/** @brief The two bank pairs, one per parity of the frame counter. */
extern u8 *D_801C58FC;
extern u8 *D_801C5900;
extern u8 *D_801C5904;
extern u8 *D_801C5908;

/** @brief Root entity pool: 2 entries of 0x64. */
extern s32 D_801C5910;
extern s32 D_801C59E0;

/** @brief Script entity pool: 4 entries of 0x58. */
extern s32 D_801C59F0;
extern s32 D_801C5B50;

/** @brief Child entity pool: 0x12C entries of 0x6C. */
extern s32 D_801C5B60;
extern s32 D_801CD9F0;

/** @brief Render state for the effect's own model. */
extern EffectRenderPart D_801CDA00;
extern s32 D_801CDA30;
extern s32 D_801D37F0;
extern EffectRender D_801D3800;

/** @brief Spark pool: 0x64 entries of 0x5C. */
extern s32 D_801D3900;
extern s32 D_801D5CF0;

/** @brief The sixteen hex digit glyphs, cached from the main string table. */
extern u8 D_801D5D00[];

/** @brief Debug text cursor: X, Y, and the colour the glyphs are drawn in. */
extern s32 D_801D5D14;
extern s32 D_801D5D18;
extern s32 D_801D5D1C;

/** @brief Per-joint world matrices, one per joint of the posed skeleton. */
extern MATRIX D_801D5D20[];

/** @} */

#endif /* EFFECT_EFFECT_001_H */
