#ifndef WORLD_WE_OBJECT4_H
#define WORLD_WE_OBJECT4_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "world.h"

/* we_object4's public surface. Everything else the unit owns is private to
 * src/world/we_object4.c. */

extern s32 D_800D2264;              /**< Counter increment per update (read by we_object3, we_object6). */

extern void func_800A7B38(void);    /**< Step the texture-strip animations and upload changed frames. */

/* Entry points called from the world entry loop WorldMain (we_object0). */
extern void func_800A64DC(void);                 /**< Build the two worldmap strip sub-OTs. */
extern void func_800A6A74(BattleSceneCtx *ctx);  /**< Splice two of @p ctx's bone prims into the strip sub-OTs. */
extern void func_800A6BE0(void);                 /**< Prime every worldmap strip pool from VRAM. */
extern void func_800A7590(BattleSceneCtx *ctx);  /**< Link the worldmap backdrop prims into the scene's OT. */
extern void func_800A7CD0(s32 *block);           /**< Load a VRAM row animation block. */
extern void func_800A8400(void);                 /**< Draw the map-view HUD layer (panel, stars, gradient). */
extern void func_800A8C1C(void);                 /**< Re-blend the world palette for the camera's position. */
extern void func_800A9300(void);                 /**< Draw the D_800D9CB0 particle pool. */
extern void func_800A9F54(VECTOR *pos, s32 x, s32 y);  /**< Draw the world-map inset mesh at @p pos. */

/** The camera-follow reference is written as a word but published as its low
 *  half by we_object3, so both views need a name. */
typedef union {
    s32 word;
    u16 half;
} CameraRef;

/* Camera-follow reference read by we_object1, we_object3 and we_object7.
 * volatile is load-bearing: it keeps the read ordered against the particle
 * position stores, matching the original schedule. */
extern volatile CameraRef D_800C9870;

/**
 * Spawn a kind-0xE particle in the D_800D9CB0 pool at @p pos (rotation
 * zeroed, life/limit RNG-jittered). Its only caller is the ambient spawner
 * func_800B99A4, which the world entry loop (WorldMain) calls.
 */
extern void func_800AB2D4(VECTOR *pos);

extern TILE_1 D_800D4FB0[2][96];    /**< Night-sky star pool, also primed by we_object5 (func_800A84D0). */

/* Steps the texture-strip animations. @p ctx is passed by its only caller but
   the body works entirely off globals and never reads it. */
extern void func_800A7E74(BattleSceneCtx *ctx);

#endif /* WORLD_WE_OBJECT4_H */
