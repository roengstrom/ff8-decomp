#include "common.h"
#include "battle.h"
#include "psxsdk/libgpu.h"
#include "world.h"
#include "world/we_object1.h"
#include "world/we_object3.h"
#include "world/we_object5.h"
#include "world/we_object10.h"
#include "world/we_object4.h"

extern TILE_1   D_800D5430[96];      /**< Second half of the star pool (== &D_800D4FB0[1][0]). */
extern DR_TPAGE D_800D58B0[2];      /**< Draw-mode packet closing the star layer, one per scene. */

/**
 * @brief World-map VRAM row animation slot (12 bytes).
 *
 * Describes one animated strip: the source frames are stacked one VRAM
 * row per frame starting at (@c u, @c v), and the current frame's row is
 * blitted to the fixed destination (@c x, @c y) — CLUT/texture-row
 * cycling driven by the shared counter @c D_800C547C.
 */
typedef struct {
    /* 0x00 */ u8 phase;     /**< Phase offset added to the shared counter. */
    /* 0x01 */ u8 divisor;   /**< Counter divisor (animation speed). */
    /* 0x02 */ u8 frames;    /**< Number of frames (0 = slot disabled). */
    /* 0x03 */ u8 pingpong;  /**< Nonzero = reflect the sequence back and forth. */
    /* 0x04 */ s16 x;        /**< Destination VRAM x. */
    /* 0x06 */ s16 y;        /**< Destination VRAM y. */
    /* 0x08 */ u16 u;        /**< Source strip VRAM x. */
    /* 0x0A */ u16 v;        /**< Source strip VRAM y of frame 0 (one row per frame). */
} WorldTexAnim; /* 0xC bytes */

extern s32 D_800C547C;              /**< Shared animation counter, wraps mod 384. */
extern s32 D_800C5478;              /**< Texture-strip animation counter, wraps mod 384 (func_800A7B38). */

extern s32 D_800C5458[];            /**< Last uploaded frame per texture strip (parallel to the offset table). */
extern WorldTexAnim D_800D4E80[4];  /**< The four VRAM row animation slots. */
extern s32 D_800D4EB0[4];           /**< Last blitted frame per slot. */

extern u16 D_800D2452;              /**< Map-view HUD slide-in coordinate fed to the func_800A84xx drawers. */
extern s32 D_800C5454;              /**< Likely nonzero while the map pointer highlights a named location — enables the name banner (func_800A8524). */

/* Camera-follow reference for the kind-12/13 drift in func_800A9CC0.
 * volatile is load-bearing: it keeps this read ordered against the
 * particle position stores, matching the original schedule. */
extern volatile s32 D_800C974C;

/* Main-binary helper: applies matrix @p m to @p in, writing @p out. */

/**
 * @brief POLY_GT3 view with packed 32-bit vertex words (0x28 bytes).
 *
 * Same layout as the SDK @c POLY_GT3, but the screen coordinates are
 * held as one @c u32 word per vertex — func_800AAD48 copies them from
 * @c WorldVtx::xy with single word moves.
 */
typedef struct {
    /* 0x00 */ u32 tag;              /**< OT link word (see setaddr/getaddr). */
    /* 0x04 */ u8 r0, g0, b0, code;  /**< Vertex 0 colour + primitive code. */
    /* 0x08 */ u32 xy0;              /**< Vertex 0 packed screen x,y. */
    /* 0x0C */ u8 u0, v0;            /**< Vertex 0 texture coords. */
    /* 0x0E */ u16 clut;             /**< Palette (CLUT) id. */
    /* 0x10 */ u8 r1, g1, b1, p1;    /**< Vertex 1 colour. */
    /* 0x14 */ u32 xy1;              /**< Vertex 1 packed screen x,y. */
    /* 0x18 */ u8 u1, v1;            /**< Vertex 1 texture coords. */
    /* 0x1A */ u16 tpage;            /**< Texture page. */
    /* 0x1C */ u8 r2, g2, b2, p2;    /**< Vertex 2 colour. */
    /* 0x20 */ u32 xy2;              /**< Vertex 2 packed screen x,y. */
    /* 0x24 */ u8 u2, v2;            /**< Vertex 2 texture coords. */
    /* 0x26 */ u16 pad26;
} WorldPolyGT3;

/** @brief Transformed world vertex: packed screen word + texture coords. */
typedef struct {
    /* 0x00 */ u32 xy;               /**< Packed screen x,y. */
    /* 0x04 */ u8 u;                 /**< Texture u. */
    /* 0x05 */ u8 pad5;
    /* 0x06 */ u8 v;                 /**< Texture v. */
    /* 0x07 */ u8 pad7;
} WorldVtx;

/** @brief Per-vertex grayscale shade triple for one triangle. */
typedef struct {
    u8 c0, c1, c2;
} TriShade;

extern WorldPolyGT3 *D_800D8804;    /**< Current POLY_GT3 slot being filled. */

/**
 * @brief POLY_GT4 view with packed 32-bit vertex words (0x34 bytes).
 *
 * Same layout as the SDK @c POLY_GT4, but the screen coordinates are
 * held as one @c u32 word per vertex — func_800AAEAC copies them from
 * @c WorldVtx::xy with single word moves.
 */
typedef struct {
    /* 0x00 */ u32 tag;              /**< OT link word (see setaddr/getaddr). */
    /* 0x04 */ u8 r0, g0, b0, code;  /**< Vertex 0 colour + primitive code. */
    /* 0x08 */ u32 xy0;              /**< Vertex 0 packed screen x,y. */
    /* 0x0C */ u8 u0, v0;            /**< Vertex 0 texture coords. */
    /* 0x0E */ u16 clut;             /**< Palette (CLUT) id. */
    /* 0x10 */ u8 r1, g1, b1, p1;    /**< Vertex 1 colour. */
    /* 0x14 */ u32 xy1;              /**< Vertex 1 packed screen x,y. */
    /* 0x18 */ u8 u1, v1;            /**< Vertex 1 texture coords. */
    /* 0x1A */ u16 tpage;            /**< Texture page. */
    /* 0x1C */ u8 r2, g2, b2, p2;    /**< Vertex 2 colour. */
    /* 0x20 */ u32 xy2;              /**< Vertex 2 packed screen x,y. */
    /* 0x24 */ u8 u2, v2;            /**< Vertex 2 texture coords. */
    /* 0x26 */ u16 pad26;
    /* 0x28 */ u8 r3, g3, b3, p3;    /**< Vertex 3 colour. */
    /* 0x2C */ u32 xy3;              /**< Vertex 3 packed screen x,y. */
    /* 0x30 */ u8 u3, v3;            /**< Vertex 3 texture coords. */
    /* 0x32 */ u16 pad32;
} WorldPolyGT4;

/** @brief Per-vertex grayscale shade quad for one quad. */
typedef struct {
    u8 c0, c1, c2, c3;
} QuadShade;

extern WorldPolyGT4 *D_800D8800;    /**< Current POLY_GT4 slot being filled. */


extern s16 D_800C53B4[];            /**< Per-plane depth-cue weight fed to func_800ABDD8. */

/* Per-pool GPU primitive template arrays (file-private to we_object4). */
extern POLY_GT4 D_800D58C0[3];      /**< Map-panel quads for the active scene. */
extern POLY_GT4 D_800D595C[3];      /**< Map-panel quads for the D_800CA040 sentinel scene. */

/** One VRAM strip: 64x16 texels of the worldmap band. */
#define STRIP_W 0x40
#define STRIP_H 0x10
/** Strips per band, and depth planes produced from each. */
#define STRIP_COUNT 4
#define STRIP_PLANES 2
/** Width of a whole band, i.e. its four strips laid side by side. */
#define BAND_W (STRIP_W * STRIP_COUNT)
/** Single-texel uploads primed per pool, one per entry of @c D_80098120. */
#define STRIP_LOAD_COUNT 11
/** VRAM row each band is read from -- and blitted back to. */
#define BAND0_VRAM_Y 0xE0
#define BAND1_VRAM_Y 0xF0
/** Scratch VRAM column the expanded copies are staged in, per band. */
#define BAND0_SCRATCH_X 0x140
#define BAND1_SCRATCH_X 0x2C0

/* Worldmap backdrop prims, one set per scene bank. Each pair is drawn against
 * two bone slots of the ordering table. Storage lives in the splat-owned
 * world_bss_we4 segment (config/ff8.yaml) so the layout does not depend on the
 * assembler's .lcomm alignment; sizes are recorded in config/symbols.world.txt. */
extern OTSubSlot   D_800D3510[2][2][4];  /**< Sub-OT pool spliced by func_800A6A74. */
extern OTSubSlot   D_800D3690[2][2][4];  /**< Second pool spliced by func_800A6A74. */
extern POLY_F4     D_800D3810[2][3];     /**< Band 0's three flat translucent quads,
                                              per scene bank. */
extern POLY_F4     D_800D38A0[2][3];     /**< Band 1's, likewise. */
extern DRAWENV     D_800D3930;           /**< Strip draw-env template, rebuilt per band. */
extern DRAWENV     D_800D3990[2];        /**< Per-scene draw-env, copied from each context. */
extern DR_ENV      D_800D3A50[2][2];
extern DR_ENV      D_800D3B50[2][2];
extern DR_ENV      D_800D3C50[2][2];
extern DR_ENV      D_800D3D50[2][2];
extern OTSubSlot   D_800D3E50[2][3][4];  /**< Primary worldmap sub-OT pool. */
extern OTSubSlot   D_800D4090[2][3][4];  /**< Secondary worldmap sub-OT pool. */
/** One record per upload slot, per bank per bone slot; how many are live at
 *  run time comes from @c func_800BEF6C. */
extern DR_LOAD     D_800D42D0[2][2][STRIP_LOAD_COUNT];

extern POLY_FT4 D_800D88B0[2][64];
extern TILE     D_800DA8D0[2][64];
/* One-past-the-end of each pool's first bank; the particle renderer stops
 * filling a bank when its cursor reaches these. */
extern POLY_FT4 D_800D92B0[];       /**< == &D_800D88B0[1][0]. */
extern TILE     D_800DACD0[];       /**< == &D_800DA8D0[1][0]. */
extern WorldPolyGT4 D_800D5A00[2][64];
extern WorldPolyGT3 D_800D7400[2][64];



/**
 * @brief One world-map colour zone, reached through the @c D_800C9EE8 offset
 *        table (entry 0 is the map-wide default).
 *
 * A zone paints the scene around a point: while the camera is inside
 * @c radius, @ref func_800A8C1C fades the five palette stops and the GTE
 * colour matrix from the default's values towards this zone's, by distance.
 */
typedef struct {
    /* 0x00 */ s32     x;            /**< Zone centre, world x. */
    /* 0x04 */ s32     y;            /**< Zone centre, world y. */
    /* 0x08 */ s32     radius;       /**< Influence radius; also sets the fade rate. */
    /* 0x0C */ CVECTOR colors[5];    /**< Palette stops: background, then the four HUD stops. */
    /* 0x20 */ s16     matrix[3][3]; /**< Colour matrix rows fed to the GTE. */
} WorldZone; /* 0x32 bytes */

/**
 * @brief One vertex of the world-map inset mesh (@ref func_800A9F54).
 *
 * The screen position is the mesh's fixed outline offset biased by the
 * requested corner; the map coordinate indexes the world-map texture and is
 * what @ref func_800AA210 takes the bounding box of.
 */
typedef struct {
    /* 0x00 */ s16 sx;  /**< Screen x. */
    /* 0x02 */ s16 sy;  /**< Screen y. */
    /* 0x04 */ s16 mx;  /**< Map-cell x. */
    /* 0x06 */ s16 my;  /**< Map-cell y. */
} WorldTessVert; /* 0x8 bytes */

/* Private to this unit: no caller outside we_object4 references these. Many are
 * unreferenced entirely in the retail build; each such function says so in an
 * @note on its own definition. */
static void func_800A688C(u16 *src, RECT *area, u16 *dst, s32 count);
static void func_800A7CD0(s32 *block);
static void func_800A8024(void);
static void func_800A8524(s32 scrollX, s16 topY, s32 brightness);
static void func_800A8868(s32 phase, s16 y);
static void func_800A8A28(s16 y);
static void func_800A9F54(WorldPos *pos, s32 x, s32 y);
static void func_800A6A74(BattleSceneCtx *ctx);
static void func_800A9CC0(Slot30 *p, KindParams *q);
static void func_800AAD48(WorldVtx *vtx, TriShade *shade);
static void func_800AAEAC(WorldVtx *vtx, QuadShade *shade);
/* Emits one mesh polygon from the assembled vertices into the current prim
 * slot; @c isQuad selects four vertices over three. */
static void func_800AA210(WorldTessVert *verts, s32 isQuad);
/* Draws the D_800D9CB0 particle pool; defined further down. */
static void func_800A9300(void);
/* Re-blends the world palette for the camera's position; defined below. */
static void func_800A8C1C(void);
/* Links the worldmap backdrop prims into the scene's OT; defined below. */
static void func_800A7590(BattleSceneCtx *ctx);
/* Builds the two worldmap strip sub-OTs; defined below. */
static void func_800A64DC(void);
/* Primes every worldmap strip pool from VRAM; defined below. */
static void func_800A6BE0(void);
/* Spawns a kind-11 particle at a fixed world position; defined below. */
static void func_800AB06C(void);

/** Vertex index per mesh polygon; the first @c MESH_QUAD_COUNT rows are quads
 *  and use all four entries, the rest are triangles and use the first three. */
extern s8 D_800C57F0[][4];
/** Per-vertex screen offset (x, y) from the mesh's top-left corner. */
extern u8 D_800C5884[][2];
/** Per-vertex map-cell offset (x, y), in half cells.
 *  @note @c volatile because the original re-reads it per vertex -- without it
 *        the compiler hoists the table's address out of the mesh loop. */
extern volatile u8 D_800C58D4[][2];

extern s32 D_800D8808;  /**< Mesh centre screen x (corner + @c MESH_CENTRE_BIAS). */
extern s32 D_800D880C;  /**< Mesh centre screen y (corner + @c MESH_CENTRE_BIAS). */

/** Scratch VRAM rows for the two depth planes. */
#define SCRATCH_NEAR_Y 0x40
#define SCRATCH_FAR_Y  0x80

/**
 * @brief Build the two worldmap strip sub-OTs.
 *
 * Each band is four 64x16 strips read back out of VRAM. @ref func_800A688C
 * expands a strip into two depth-cued copies, which are uploaded side by side
 * to scratch VRAM; a @c DR_MOVE is then packed into every sub-slot of both
 * pool rows so that drawing the slot blits its copy to the band's position.
 * Finally each record's four sub-slots are chained head to tail, turning the
 * record into the mini ordering table the renderer splices bone prims into.
 *
 * The second band repeats the whole thing one VRAM row lower, into the second
 * pool.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin.
 */
static void func_800A64DC(void) {
    RECT src;
    RECT near;
    RECT far;
    u16  planes[STRIP_PLANES][STRIP_W * STRIP_H];
    s32  k;
    s32  rec;
    s32  slot;
    s32  i;
    OTSubSlot *prev;
    OTSubSlot *next;

    src.x = 0;
    src.y = BAND0_VRAM_Y;
    src.w = STRIP_W;
    src.h = STRIP_H;
    near.x = BAND0_SCRATCH_X;
    near.y = SCRATCH_NEAR_Y;
    near.w = STRIP_W;
    near.h = STRIP_H;
    far.x = BAND0_SCRATCH_X;
    far.y = SCRATCH_FAR_Y;
    far.w = STRIP_W;
    far.h = STRIP_H;

    for (k = 0; k < STRIP_COUNT; k++) {
        func_800A688C(NULL, &src, planes[0], STRIP_PLANES);
        LoadImage(&near, (u32 *)planes[0]);
        LoadImage(&far, (u32 *)planes[1]);
        /* A sub-slot's 0x18-byte payload is exactly a DR_MOVE; the pool is
         * typed OTSubSlot because the renderer walks it as an OT chain. */
        for (i = 0; i < 2; i++) {
            SetDrawMove((DR_MOVE *)&D_800D3510[i][0][k], &near, k * STRIP_W, BAND0_VRAM_Y);
            SetDrawMove((DR_MOVE *)&D_800D3510[i][1][k], &far, k * STRIP_W, BAND0_VRAM_Y);
        }
        near.y += STRIP_H;
        far.y += STRIP_H;
        src.x += STRIP_W;
    }

    src.x = 0;
    src.y = BAND1_VRAM_Y;
    src.w = STRIP_W;
    src.h = STRIP_H;
    near.x = BAND1_SCRATCH_X;
    near.y = SCRATCH_NEAR_Y;
    near.w = STRIP_W;
    near.h = STRIP_H;
    far.x = BAND1_SCRATCH_X;
    far.y = SCRATCH_FAR_Y;
    far.w = STRIP_W;
    far.h = STRIP_H;

    for (k = 0; k < STRIP_COUNT; k++) {
        func_800A688C(NULL, &src, planes[0], STRIP_PLANES);
        LoadImage(&near, (u32 *)planes[0]);
        LoadImage(&far, (u32 *)planes[1]);
        for (i = 0; i < 2; i++) {
            SetDrawMove((DR_MOVE *)&D_800D3690[i][0][k], &near, k * STRIP_W, BAND1_VRAM_Y);
            SetDrawMove((DR_MOVE *)&D_800D3690[i][1][k], &far, k * STRIP_W, BAND1_VRAM_Y);
        }
        near.y += STRIP_H;
        far.y += STRIP_H;
        src.x += STRIP_W;
    }

    for (i = 0; i < 2; i++) {
        for (rec = 0; rec < 2; rec++) {
            next = D_800D3510[i][rec];
            for (slot = 0, prev = next - 1; slot < 4; slot++) {
                if (slot != 0) {
                    addPrim(prev, next);
                }
                prev++;
                next++;
            }
        }
    }

    for (i = 0; i < 2; i++) {
        for (rec = 0; rec < 2; rec++) {
            next = D_800D3690[i][rec];
            for (slot = 0, prev = next - 1; slot < 4; slot++) {
                if (slot != 0) {
                    addPrim(prev, next);
                }
                prev++;
                next++;
            }
        }
    }
}

/**
 * @brief Expand a VRAM rectangle to depth-cued BGR555, one scanline at a time.
 *
 * Walks @p area row by row. Each row is either taken straight from
 * @p src or, when @p src is @c NULL, read back out of VRAM with
 * @c StoreImage into a scratch line (the copy is fenced with
 * @c DrawSync). The row's 15-bit pixels are widened into @c CVECTOR
 * form -- each 5-bit channel scaled to 8 bits, and @c cd carrying the
 * source STP bit, or @c 0x80 to mark a fully black pixel as
 * transparent. The expanded row is then handed to @ref func_800ABDD8
 * once per output plane, which blends it against the depth-cue colour
 * with the plane's weight from @c D_800C53B4 and packs it back to
 * BGR555. Plane @c n lands @c w*h halfwords into @p dst.
 *
 * @param src   Source pixels, or @c NULL to read the rows from VRAM.
 * @param area  Region to convert; only @c w and @c h bound the loops,
 *              @c x / @c y seed the VRAM readback.
 * @param dst   Destination for the packed planes.
 * @param count Number of depth-cue planes to emit.
 */
static void func_800A688C(u16 *src, RECT *area, u16 *dst, s32 count) {
    RECT rect;
    CVECTOR pixels[256];
    u16 line[256];
    u16 *line16;
    s32 row;
    s32 i;

    memcpy(&rect, area, sizeof(RECT));
    rect.h = 1;

    for (row = 0; row < area->h; row++) {
        line16 = src;
        if (line16 == NULL) {
            line16 = line;
            StoreImage(&rect, line16);
            DrawSync(0);
        }
        for (i = 0; i < area->w; i++) {
            pixels[i].r = ((u32)line16[i] << 3) & 0xF8;
            pixels[i].g = (line16[i] >> 2) & 0xF8;
            pixels[i].b = (line16[i] >> 7) & 0xF8;
            if ((pixels[i].r | pixels[i].g | pixels[i].b) != 0) {
                pixels[i].cd = line16[i] >> 15;
            } else {
                pixels[i].cd = WORLD_CD_DROP;
            }
        }
        for (i = 0; i < count; i++) {
            func_800ABDD8(pixels, dst + i * area->w * area->h, D_800C53B4[i], area->w);
        }
        dst += area->w;
        rect.y++;
    }
}

/**
 * @brief Splice two of @p ctx's bone prims into the sub-OT records of
 *        the @c D_800D3510 and @c D_800D3690 pools.
 *
 * Sibling of @ref func_800A735C: for the bone ids in @c D_800C53B8[1]
 * and @c [2], point the record's last sub-slot ([3]) at the bone's prim
 * (@c ctx->primList entry) and the bone's prim back at the record's
 * first sub-slot ([0]), using the PsyQ @c setaddr / @c getaddr tag
 * operations. The pool row is picked by whether @p ctx is the
 * @c D_800CA040 sentinel.
 *
 * @param ctx Scene context whose @c primList holds the bone prims.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin.
 */
static void func_800A6A74(BattleSceneCtx *ctx) {
    s32 cond;
    s32 i;

    cond = ctx == &D_800CA040;
    for (i = 0; i < 2; i++) {
        setaddr(&D_800D3510[cond][i][3], getaddr(&ctx->primList[D_800C53B8[i + 1]]));
        setaddr(&ctx->primList[D_800C53B8[i + 1]], &D_800D3510[cond][i][0]);
    }
    for (i = 0; i < 2; i++) {
        setaddr(&D_800D3690[cond][i][3], getaddr(&ctx->primList[D_800C53B8[i + 1]]));
        setaddr(&ctx->primList[D_800C53B8[i + 1]], &D_800D3690[cond][i][0]);
    }
}

/** VRAM source point for one strip upload: x in the low half, y in the high. */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
} VramXY; /* 0x4 bytes */

/** Where in VRAM each of the eleven uploads reads its 1x1 source pixel from. */
static const VramXY D_80098120[STRIP_LOAD_COUNT] = {
    { 0x000, 0x0F9 }, { 0x020, 0x0E5 }, { 0x0D0, 0x0EC }, { 0x0C0, 0x0E1 },
    { 0x000, 0x0E2 }, { 0x0B0, 0x0E4 }, { 0x0B0, 0x0E5 }, { 0x0E0, 0x0E6 },
    { 0x000, 0x0EE }, { 0x080, 0x0EF }, { 0x000, 0x0FD },
};

/**
 * @brief Prime every worldmap strip pool from VRAM.
 *
 * Run once at world setup, for each of the two bands in turn. A band's four
 * 64x16 strips are copied out of the map texture into scratch VRAM, and each
 * sub-OT record gets a @c DR_MOVE per slot that blits one strip back, with the
 * record's four slots chained into a mini ordering table. The band's draw
 * environments are then packed: one from a default env covering the band, one
 * from each scene context's own template. Each band also gets three flat
 * semi-transparent quads spanning the strip.
 *
 * Finally every @c DR_LOAD in the @c D_800D42D0 pool is pointed at its 1x1
 * source pixel, so drawing one uploads a single texel.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin. It is the filler for the pools that
 *       @ref func_800A7590 links and @ref func_800A64DC chains, both equally
 *       unreferenced.
 */
static void func_800A6BE0(void) {
    RECT          rect;
    VramXY        coords[STRIP_LOAD_COUNT];
    s32           i, j, k;
    const VramXY *src;

    /* Taking the address through a pointer is load-bearing: copying straight
     * out of the array lets the compiler assume 4-byte alignment and emit only
     * the aligned path, where the original tests alignment at run time. */
    src = D_80098120;
    memcpy(coords, src, sizeof(coords));

    rect.x = 0;
    rect.y = BAND0_VRAM_Y;
    rect.w = STRIP_W;
    rect.h = STRIP_H;
    for (i = 0; i < STRIP_COUNT; i++) {
        MoveImage(&rect, BAND0_SCRATCH_X, i * STRIP_H);
        DrawSync(0);
        rect.x += STRIP_W;
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            rect.x = BAND0_SCRATCH_X;
            rect.y = 0;
            rect.w = STRIP_W;
            rect.h = STRIP_H;
            /* A sub-slot's 0x18-byte payload is exactly a DR_MOVE; the pool is
             * typed OTSubSlot because the renderer walks it as an OT chain. */
            for (k = 0; k < 4; k++) {
                SetDrawMove((DR_MOVE *)&D_800D3E50[i][j][k], &rect,
                            k * STRIP_W, BAND0_VRAM_Y);
                rect.y += STRIP_H;
                if (k != 0) {
                    addPrim(&D_800D3E50[i][j][k - 1], &D_800D3E50[i][j][k]);
                }
            }
        }
    }

    SetDefDrawEnv(&D_800D3930, 0, BAND0_VRAM_Y, BAND_W, STRIP_H);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            SetDrawEnv(&D_800D3A50[i][j], &D_800D3930);
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            setPolyF4(&D_800D3810[i][j]);
            setSemiTrans(&D_800D3810[i][j], 1);
            setXY4(&D_800D3810[i][j], 0, 0, BAND_W, 0, 0, STRIP_H, BAND_W, STRIP_H);
            setRGB0(&D_800D3810[i][j], 0x80, 0x80, 0x80);
        }
    }

    for (i = 0; i < 2; i++) {
        /* The two scene contexts sit back to back with the sentinel first;
         * we_object1 spells the same pair as +0x5C and +0x40CC. */
        D_800D3990[i] = (&D_800CA040)[i].drawEnv;
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            SetDrawEnv(&D_800D3C50[i][j], &D_800D3990[i]);
        }
    }

    rect.x = 0;
    rect.y = BAND1_VRAM_Y;
    rect.w = STRIP_W;
    rect.h = STRIP_H;
    for (i = 0; i < STRIP_COUNT; i++) {
        MoveImage(&rect, BAND1_SCRATCH_X, i * STRIP_H);
        DrawSync(0);
        rect.x += STRIP_W;
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            rect.x = BAND1_SCRATCH_X;
            rect.y = 0;
            rect.w = STRIP_W;
            rect.h = STRIP_H;
            for (k = 0; k < 4; k++) {
                SetDrawMove((DR_MOVE *)&D_800D4090[i][j][k], &rect,
                            k * STRIP_W, BAND1_VRAM_Y);
                rect.y += STRIP_H;
                if (k != 0) {
                    addPrim(&D_800D4090[i][j][k - 1], &D_800D4090[i][j][k]);
                }
            }
        }
    }

    SetDefDrawEnv(&D_800D3930, 0, BAND1_VRAM_Y, BAND_W, STRIP_H);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            SetDrawEnv(&D_800D3B50[i][j], &D_800D3930);
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            setPolyF4(&D_800D38A0[i][j]);
            setSemiTrans(&D_800D38A0[i][j], 1);
            setXY4(&D_800D38A0[i][j], 0, 0, BAND_W, 0, 0, STRIP_H, BAND_W, STRIP_H);
            setRGB0(&D_800D38A0[i][j], 0x80, 0x80, 0x80);
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            SetDrawEnv(&D_800D3D50[i][j], &D_800D3990[i]);
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < STRIP_LOAD_COUNT; k++) {
                rect.x = coords[k].x;
                rect.y = coords[k].y;
                rect.w = 1;
                rect.h = 1;
                D_800D42D0[i][j][k].p[0] = 0;
                SetDrawLoad(&D_800D42D0[i][j][k], &rect);
            }
        }
    }
}

/**
 * @brief Splice three of @c a0's bone prims into the sub-OT records of
 *        the @c D_800D3E50 and @c D_800D4090 pools.
 *
 * For each of the three bone ids in @c D_800C53B8[0], @c [3], @c [4],
 * link the entity's prim pointer between a record's first ([0]) and
 * last ([3]) sub-slots, once per pool. The cond bit picks the @c [0]
 * or @c [1] pool row, switching layout for non-canonical entity models.
 *
 * @param a0 Entity model whose @c primList holds the prims to splice.
 */
void func_800A735C(BattleSceneCtx *a0) {
    s32 cond = (a0 != &D_800CA040) ? 1 : 0;

    addPrims(&a0->primList[D_800C53B8[0]], &D_800D3E50[cond][0][0].link, &D_800D3E50[cond][0][3].link);
    addPrims(&a0->primList[D_800C53B8[3]], &D_800D3E50[cond][1][0].link, &D_800D3E50[cond][1][3].link);
    addPrims(&a0->primList[D_800C53B8[4]], &D_800D3E50[cond][2][0].link, &D_800D3E50[cond][2][3].link);

    addPrims(&a0->primList[D_800C53B8[0]], &D_800D4090[cond][0][0].link, &D_800D4090[cond][0][3].link);
    addPrims(&a0->primList[D_800C53B8[3]], &D_800D4090[cond][1][0].link, &D_800D4090[cond][1][3].link);
    addPrims(&a0->primList[D_800C53B8[4]], &D_800D4090[cond][2][0].link, &D_800D4090[cond][2][3].link);
}

/** Bone slots of the ordering table the worldmap backdrop is drawn against. */
#define BACKDROP_OT_FAR  3
#define BACKDROP_OT_NEAR 4
/** Records always emitted per bone slot, before the marker adds one more. */
#define BACKDROP_BASE_RECORDS 10

/**
 * @brief Link this frame's worldmap backdrop into the scene's ordering table.
 *
 * Everything here goes onto two of the ordering table's bone slots, whose
 * indices come from the bone-id table @c D_800C53B8: a far slot that takes the
 * layer drawn behind, and a near slot for the layer in front. The bulk is the
 * @c D_800D42D0 record pool -- ten per slot, eleven when @c func_800BEF6C
 * reports a marked slot is active -- followed by the fixed prims: a draw
 * environment, a flat quad and a second draw environment per group, twice over.
 *
 * Since @c addPrim pushes onto the head of a chain, the calls read back to
 * front: the last one linked is the first one drawn.
 *
 * @param ctx Scene to link into; the @c D_800CA040 sentinel selects bank 0 and
 *            any live scene selects bank 1.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin. The pools it links are filled by
 *       func_800A6BE0, which is equally unreferenced.
 */
static void func_800A7590(BattleSceneCtx *ctx) {
    s32 bank;
    s32 extra;
    s32 group;
    s32 i;

    bank = (ctx != &D_800CA040);
    extra = func_800BEF6C();

    for (group = 0; group < 2; group++) {
        for (i = 0; i < extra + BACKDROP_BASE_RECORDS; i++) {
            addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR + group]],
                    &D_800D42D0[bank][group][i]);
        }
    }

    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR]],  &D_800D3C50[bank][0]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR]],  &D_800D3810[bank][0]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR]],  &D_800D3A50[bank][0]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D3C50[bank][1]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D3810[bank][1]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D3810[bank][2]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D3A50[bank][1]);

    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR]],  &D_800D3D50[bank][0]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR]],  &D_800D38A0[bank][0]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_FAR]],  &D_800D3B50[bank][0]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D3D50[bank][1]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D38A0[bank][1]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D38A0[bank][2]);
    addPrim(&ctx->primList[D_800C53B8[BACKDROP_OT_NEAR]], &D_800D3B50[bank][1]);
}

/**
 * @brief Step the world texture-strip animations and upload changed frames.
 *
 * @c D_800C9744 points at an animation block that begins with a
 * NULL-terminated table of s32 offsets, each relative to the block start
 * and locating one strip record:
 *
 *   +0  u8  phase      per-strip start offset into the shared counter
 *   +1  u8  divisor    counter ticks per animation frame
 *   +2  u8  frames     frame count
 *   +3  u8  pingpong   nonzero: play forward then backward
 *   +4  s16 x, y       destination VRAM coordinates
 *   +8  s32 frameOffset[frames]   image blobs, relative to this table
 *
 * Advances the shared counter @c D_800C5478 mod 384, derives each
 * strip's current frame (ping-pong reflects over a period of
 * @c frames * 2 - 2), and when it differs from the cached value in
 * @c D_800C5458[i], uploads that frame's image at (x, y) through
 * @ref func_8009C478 and caches the new frame.
 */
void func_800A7B38(void) {
    u32 *p;
    s32 *tbl;
    s32 i;
    s32 phase;
    u32 cfg;
    s32 divisor;
    s32 frames;
    s16 x;
    s16 y;
    s32 frame;
    u8 f;
    u8 n;
    u32 over;

    D_800C5478 = (D_800C5478 + D_800D2264) % 384;

    tbl = D_800C9744;
    for (i = 0; tbl[i] != 0; i++) {
        p = (u32 *)((u8 *)D_800C9744 + tbl[i]);
        phase = *(u8 *)p;
        cfg = *p;
        p++;
        x = ((s16 *)p)[0];
        y = ((s16 *)p)[1];
        divisor = cfg >> 8;
        frames = cfg >> 16;
        p++;
        if ((cfg >> 24) != 0) {
            f = frames;
            n = (phase + D_800C5478) / (divisor & 0xFF) % (u8)(f * 2 - 2);
            if (n >= f) {
                over = n - f + 2;
                frame = f - over;
            } else {
                frame = n;
            }
        } else {
            frame = ((phase + D_800C5478) / (divisor & 0xFF)) % (frames & 0xFF);
        }
        if (frame != D_800C5458[i]) {
            func_8009C478((s32 *)((u8 *)p + p[frame]), x, y);
            D_800C5458[i] = frame;
        }
    }
}

/**
 * @brief Load a VRAM row animation block: set up the four slots and
 *        upload every frame.
 *
 * @p block uses the same layout as the texture-strip block of
 * @ref func_800A7B38: a NULL-terminated table of s32 offsets, each
 * relative to the block start and locating one record:
 *
 *   +0x0  u32 timing     phase, divisor, frames and pingpong packed
 *                       one per byte, read out field by field
 *   +0x4  s16 x, y      destination the animator blits the strip row to
 *   +0x8  u16 u, v      strip storage: one 256x1 VRAM row per frame
 *   +0xC  s32 frameOffset[frames]   @ref StreamImage records, each
 *                       relative to the frame table itself
 *
 * Clears every slot's @c frames (disabling it for @ref func_800A7E74),
 * then copies each record into @c D_800D4E80[i], uploads all its frames
 * as consecutive 256x1 VRAM rows at (u, v + frame) via @c LoadImage, and
 * invalidates the last-frame cache @c D_800D4EB0[i] so the animator
 * repaints on its first update.
 *
 * @param block Animation block (offset table at its start), or a table
 *              whose first entry is 0 to just disable all slots.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin.
 */
static void func_800A7CD0(s32 *block) {
    WorldTexAnim *anim;
    s32 count;
    u16 *p;
    StreamImage *img;
    s32 i;
    s32 j;
    RECT rect;

    for (anim = D_800D4E80; anim < &D_800D4E80[4]; anim++) {
        anim->frames = 0;
    }
    anim = D_800D4E80;
    i = 0;
    while (block[i] != 0) {
        p = (u16 *)((u8 *)block + block[i]);
        anim->phase = ((u8 *)p)[0];
        anim->divisor = *(u32 *)p >> 8;
        count = p[1];
        anim->frames = count;
        anim->pingpong = ((u8 *)p)[3];
        p += 2;
        anim->x = p[0];
        anim->y = p[1];
        p += 2;
        anim->u = p[0];
        anim->v = p[1];
        p += 2;
        for (j = 0; j < anim->frames; j++) {
            img = (StreamImage *)((u8 *)p + ((s32 *)p)[j]);
            rect.x = anim->u;
            rect.y = anim->v + j;
            rect.w = 0x100;
            rect.h = 1;
            LoadImage(&rect, img->data);
        }
        D_800D4EB0[i] = -1;
        i++;
        anim++;
    }
}

/**
 * @brief Advance the four world-map VRAM row animations.
 *
 * Steps the shared animation counter @c D_800C547C by @c D_800D2264,
 * wrapping modulo 384, then walks the four @c WorldTexAnim slots in
 * @c D_800D4E80. Each enabled slot (frames != 0) derives its current
 * frame from the counter: cyclic (`(counter + phase) / divisor % frames`),
 * or reflected over the frame range with period `frames * 2 - 2` when
 * @c pingpong is set (0,1,..,frames-1,frames-2,..,1,..). When the frame
 * differs from the slot's last blitted one (@c D_800D4EB0), the 256x1
 * source row at (u, v + frame) is copied to the slot's destination
 * (x, y) via @c MoveImage and the new frame is cached.
 */
void func_800A7E74(void) {
    WorldTexAnim *anim;
    s32 i;
    u32 n;
    u32 over;
    s32 frame;
    RECT rect;

    D_800C547C = (D_800C547C + D_800D2264) % 384;

    i = 0;
    for (anim = D_800D4E80; anim < &D_800D4E80[4]; anim++, i++) {
        if (anim->frames == 0) {
            continue;
        }
        if (anim->pingpong != 0) {
            n = (u8)(((D_800C547C + anim->phase) / anim->divisor)
                     % (u8)(anim->frames * 2 - 1));
            if (n >= anim->frames) {
                over = n - anim->frames + 2;
                frame = anim->frames - over;
            } else {
                frame = n;
            }
        } else {
            frame = ((D_800C547C + anim->phase) / anim->divisor) % anim->frames;
        }
        if (D_800D4EB0[i] != frame) {
            rect.x = anim->u;
            rect.y = anim->v + frame;
            rect.w = 0x100;
            rect.h = 1;
            MoveImage(&rect, anim->x, anim->y);
            D_800D4EB0[i] = frame;
        }
    }
}

/* The fixed world point the beacon marks (tile 5,6 of the 32x24 grid)
 * and the base the squared camera distance is subtracted from. */
#define MARKER_WORLD_X  0xA000
#define MARKER_WORLD_Y  0xC000
#define MARKER_RANGE_SQ 0x90000000

/**
 * @brief Draw the map-view location beacon as one scaled billboard quad.
 *
 * Dead code: nothing calls it, and it reads @c dx / @c dy before ever
 * writing them — the falloff term multiplies whatever the caller left in
 * the registers. The @ref uninitReg markers reproduce that: they give the
 * variables definitions the optimiser can see (restoring the original's
 * register assignment) without emitting any code. The surviving logic:
 *
 *  -# Compute a proximity falloff @c MARKER_RANGE_SQ - (dx*dx + dy*dy) and
 *     bail when it is 0.
 *  -# Project the fixed world point (@c MARKER_WORLD_X, @c MARKER_WORLD_Y),
 *     offset by the
 *     camera scratch @c D_800C9770[0] and hovering by @c falloff>>18,
 *     through the world-to-screen matrix via @c rtps.
 *  -# Reject when the screen point is outside x in [-0xBF, 0x200) or
 *     y in [-0xBF, 0x1A0).
 *  -# Size a quad from the falloff (wider and taller when close) in the
 *     slot pair @c func_800ABD54 primed, and link it into the marker
 *     layer @c BSC_MARKER_IDX of the active scene.
 *
 * The unused @c d vector and the discarded @c func_800A5E40 result are
 * kept as the original left them.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin.
 */
static void func_800A8024(void) {
    VECTOR   ofs;
    SVECTOR  rot;
    VECTOR   base;
    SVECTOR  pos;
    SVECTOR  screen;
    VECTOR   d;
    s32      x, y;
    s32      dx, dy;
    u32      falloff;
    POLY_FT4 *p;
    s16      w;
    s32      h;

    x = D_800C9868.x;
    y = D_800C9868.y;

    // Matching hack, but it's fine since it's all dead code, i.e. function is never called
    uninitReg(dx);
    uninitReg(dy);

    func_800A5E40(x, y);
    ofs.vz = 0;
    ofs.vx = MARKER_WORLD_X - D_800C9868.x;
    ofs.vy = MARKER_WORLD_Y - D_800C9868.y;
    falloff = MARKER_RANGE_SQ - (dx * dx + dy * dy);
    if (falloff == 0) {
        return;
    }

    rot.vy = -(falloff >> 18) + 0xC00;
    rot.vx = ofs.vx;
    rot.vz = ofs.vy;
    rot.vx += D_800C9770[0].vx;
    rot.vz += D_800C9770[0].vz;

    func_800423DC((VECTOR *)&D_800C9838, D_800C9838.t, &base);
    SetRotMatrix(&D_800C9838);
    gte_ldtr(0, 0, 0);

    pos.vx = base.vx + rot.vx;
    pos.vy = base.vy + rot.vy;
    pos.vz = base.vz + rot.vz;
    gte_ldv0(&pos);
    gte_rtps();
    gte_stsxy(&screen);

    if (screen.vx < -0xBF || screen.vx >= 0x200) {
        return;
    }
    if (screen.vy < -0xBF) {
        return;
    }
    if (screen.vy >= 0x1A0) {
        return;
    }

    p = D_800D4EC0;
    if (D_800D244C == &D_800CA040) {
        p++;
    }
    w = (falloff >> 22) * 3 >> 5;
    w += 0x32;
    h = (w + 0xA) >> 1;
    p->x0 = p->x2 = screen.vx - h;
    p->x1 = p->x3 = screen.vx + h;
    p->y0 = p->y1 = screen.vy - ((u32)w >> 1);
    p->y2 = p->y3 = screen.vy + ((u32)w >> 1);
    setaddr(p, getaddr(&D_800D244C->primList[BSC_MARKER_IDX]));
    setaddr(&D_800D244C->primList[BSC_MARKER_IDX], p);
}

/**
 * @brief Project the world-map view centre to screen space.
 *
 * Builds a rotation matrix from the world transform's tail angle
 * (@c D_800D2390.tail.angle) with no translation, turns the map's zoom
 * distance (@c D_800C4D30 * 4, biased by the transform's head field) into
 * a view-space offset through @c ApplyMatrixSV, shifts it by the camera
 * offset in @c D_800C9770, and runs the result through the GTE with the
 * world-to-screen matrix @c D_800C9838. The projected y is pulled up by
 * one unit per 300 units of head angle, tilting the marker with the map.
 *
 * @param out Receives screen x/y in @c vx/@c vy and depth in @c vz.
 *            May be NULL, in which case the projection is discarded and
 *            only the GTE registers are left loaded.
 */
void func_800A8270(SVECTOR *out) {
    SVECTOR rot;
    MATRIX m;
    SVECTOR screen;
    s32 depth;

    rot.vx = 0;
    rot.vz = 0;
    rot.vy = D_800D2390.tail.angle;
    RotMatrix(&rot, &m);

    m.t[2] = 0;
    m.t[1] = 0;
    m.t[0] = 0;
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = (u16)D_800C4D30 * 4;
    rot.vz = rot.vz + D_800D2390.head.unk4;
    ApplyMatrixSV(&m, &rot, &rot);

    rot.vx += D_800C9770[0].vx;
    rot.vz += D_800C9770[0].vz;

    gte_SetRotMatrix(&D_800C9838);
    gte_SetTransMatrix(&D_800C9838);
    gte_ldv0(&rot);
    gte_rtps();
    gte_stsxy(&screen);
    gte_stsz(&depth);

    screen.vy -= abs(D_800D2390.head.unk4) / 300;
    if (out != NULL) {
        out->vx = screen.vx;
        out->vy = screen.vy;
        out->vz = depth;
    }
}

/**
 * @brief Lay out and draw the world-map HUD trio at slide coordinate
 *        @c D_800D2452.
 *
 * Derives a wrap phase `0x100 - (u8)D_800D239A` from the low byte of the
 * camera heading and passes it to @c func_800A8868 (drawn at
 * `D_800D2452 - 0x30`). When @c D_800C5454 is set, also draws the centred
 * element via @c func_800A8524 at `D_800D2452 - 0xA0`, with x offset
 * `(0x300 - (b0 + b1 + b2 * 2) * 2) >> 1` clamped to [0, 0x200], where
 * b0..b2 are the width bytes of @c D_800DB0D0. Always finishes with
 * @c func_800A8A28 at the raw coordinate.
 *
 * @note Dead code in the retail build: nothing calls this, and its address
 *       appears in no pointer table, so the three drawers below it — the
 *       map panel, the star field and the backdrop gradient — never run
 *       either. They have no other caller.
 *
 * @note Purpose uncertain — appears to drive the world-map map-view HUD:
 *       a map panel that wraps with the camera heading (@c func_800A8868),
 *       a location-name banner shown only while the map pointer highlights
 *       a named point (@c func_800A8524, centred to its text width), and
 *       the pointer/marker sprite (@c func_800A8A28).
 */
void func_800A8400(void) {
    s32 phase;
    s32 pos;
    s32 x;

    phase = 0x100 - (u8)D_800D239A;
    func_800A8868(phase, D_800D2452 - 0x30);
    if (D_800C5454 != 0) {
        pos = (0x300 - (D_800DB0D0[0].r + D_800DB0D0[0].g + D_800DB0D0[0].b * 2) * 2) >> 1;
        if (pos >= 0) {
            x = 0x200;
            if (pos <= 0x200) {
                x = pos;
            }
        } else {
            x = 0;
        }
        func_800A8524(phase, D_800D2452 - 0xA0, x);
    }
    func_800A8A28(D_800D2452);
}


/**
 * @brief Initialise two 96-slot DR_MODE pools with fixed tag fields.
 *
 * For each of the two pools at @c D_800D4FB0[0..1], walks 96 DR_MODE
 * slots and sets @c len = 2 (2-word payload) and @c code = 0x6A via
 * the PsyQ @c setlen/@c setcode macros. Used to prime the primitive
 * templates before per-frame GPU command generation fills in the rest.
 */
void func_800A84D0(void) {
    s32 s, i;
    for (s = 0; s < 2; s++) {
        for (i = 0; i < 96; i++) {
            setTile1(&D_800D4FB0[s][i]);
            setSemiTrans(&D_800D4FB0[s][i], 1);
        }
    }
}

/* The map view the star field is drawn over. */
#define STAR_SCREEN_W    320
#define STAR_SCREEN_H    224

/* It is filled one texture page at a time; each band gets 32 stars, of
 * which the first 16 twinkle and the rest use a plain ramp. */
#define STAR_BAND_W      256
#define STARS_PER_BAND   32
#define STARS_TWINKLING  16

/**
 * @brief Scatter the world-map night sky over the visible columns.
 *
 * For each 256-pixel column band still on screen the RNG is reseeded and
 * 32 stars are placed at random offsets within the band. A star's grey
 * level rises with its index and with how far its twinkle phase sits from
 * the midpoint of @c D_800D23D0; one of its three channels is then swapped
 * for a flicker value so the field shimmers in colour rather than merely
 * pulsing. Stars past index 15 use a plain ramp instead. Each visible star
 * is emitted as a @c TILE_1 on the HUD layer, and a @c DR_TPAGE closes the
 * layer.
 *
 * @note Dead code: reached only from the unreferenced @ref func_800A8400.
 *
 * @note Four spellings here are matching devices for gcc 2.8.0 rather than
 *       intent. The @c nextColX temp lets the column step fill the inner
 *       loop's branch delay slot; repeating @c (clock >> 1) instead of
 *       naming it keeps its hoist after the @p topY spill; and of the three
 *       lines marked "Regalloc hack" the @c do/while(0) is the principled
 *       one -- its loop-end note stops local-alloc's @c optimize_reg_copy_1
 *       rewriting the loop test to read the copy's destination.
 *
 * @param scrollX    Map scroll offset; folded to the column left of screen.
 * @param topY       Screen y of the band's top edge.
 * @param brightness Scale applied to every star colour, 9-bit fixed point.
 */
static void func_800A8524(s32 scrollX, s16 topY, s32 brightness) {
    TILE_1   *tile;
    DR_TPAGE *tpage;
    s32 clock;
    s32 colX, nextColX;
    s32 star;
    s32 starX, starY;
    s32 jitter;
    s32 twinkle, dim, level, shade, flicker;

    if (D_800D244C == &D_800CA040) {
        tile = D_800D5430;
    } else {
        tile = D_800D4FB0[0];
    }

    tpage = D_800D58B0;
    if (D_800D244C == &D_800CA040) {
        tpage++;
    }

    clock = D_800D23D0;

    if ((s16)scrollX == 0) {
        colX = scrollX;
    } else if ((u16)(scrollX - 1) < 0xFF) {
        colX = scrollX - STAR_BAND_W;
    } else if ((u16)(scrollX - STAR_BAND_W) < 0x41) {
        colX = scrollX - 2 * STAR_BAND_W;
    }

    if ((s16)colX < STAR_SCREEN_W) {
        do {
            func_8009CCDC(0);
            for (star = 0; star < STARS_PER_BAND; star++) {
                starX = colX + (func_8009CC98() & 0xFF);
                jitter = func_8009CC98();
                if ((star & 1) == 0) {
                    starY = topY + (jitter & 0x7F);
                } else {
                    starY = topY + (jitter & 0x3F);
                }
                if ((u16)starX < STAR_SCREEN_W && (s16)starY >= 0 && (s16)starY < STAR_SCREEN_H) {
                    if (star < STARS_TWINKLING) {
                        twinkle = (clock >> 1) % (star + 4);
                        dim = twinkle - 10;
                        if (dim <= 0) {
                            dim = 10 - twinkle;
                        }
                        level = star * 4;
                        level += dim << 4;
                        shade = (level * brightness) >> 9;
                        tile->b0 = shade;
                        tile->g0 = shade;
                        tile->r0 = shade;
                        tile->b0 = shade; // Regalloc hack
                        if ((star & 3) == 0) {
                            flicker = (level + (clock >> 1)) & 7;
                            tile->r0 = ((flicker & star) * brightness) >> 9;
                        } else if ((star & 3) == 1) {
                            flicker = (level + (clock >> 2)) & 5;
                            tile->g0 = ((flicker & star) * brightness) >> 9;
                        } else if ((star & 3) == 2) {
                            flicker = (level + (clock >> 3)) & 3;
                            tile->b0 = ((flicker & star) * brightness) >> 9;
                        }
                    } else {
                        shade = ((star * 8 + 0x40) * brightness) >> 9;
                        dim++; // Regalloc hack
                        dim--; // Regalloc hack
                        tile->b0 = shade;
                        tile->g0 = shade;
                        tile->r0 = shade;
                    }
                    tile->x0 = starX;
                    tile->y0 = starY;
                    addPrim(&D_800D244C->primList[BSC_HUD_IDX], tile);
                    tile++;
                }
            }
            nextColX = colX + STAR_BAND_W;
            do { colX = nextColX; } while (0); // Regalloc hack
        } while ((s16)nextColX < STAR_SCREEN_W);
    }

    setlen(tpage, 1);
    tpage->code[0] = 0xE1000220;
    addPrim(&D_800D244C->primList[BSC_HUD_IDX], tpage);
}

/**
 * @brief Draw the wrapping world-map panel as a row of textured quads.
 *
 * The panel is 256 texels wide but the screen is 320, so it is drawn as
 * up to three semi-transparent @c POLY_GT4 quads tiled horizontally
 * until the row covers the screen. @p phase is the camera-derived wrap
 * offset; its low 16 bits are folded back to the tile origin left of the
 * screen so the seam tracks the heading:
 *
 *   - @c 0            -> start at 0
 *   - @c 1..0xFF      -> start at @p phase - 0x100
 *   - @c 0x100..0x140 -> start at @p phase - 0x200
 *
 * Each quad is 0x100 by 0x40, shaded flat 0x80 along its top edge and by
 * the palette stop @c D_800DB0D0[2] along its bottom, textured from
 * the page at VRAM x 0x380 with the CLUT at (0x340, 0xE0), and linked
 * into the HUD layer @c BSC_HUD_IDX of the active scene.
 *
 * @note Dead code: reached only from the unreferenced @ref func_800A8400.
 *
 * @param phase Wrap offset from the camera heading (see above).
 * @param y     Top edge of the panel row, in screen coordinates.
 */
static void func_800A8868(s32 phase, s16 y) {
    s16 pos = phase;
    POLY_GT4 *prim;
    s16 x;

    if (D_800D244C == &D_800CA040) {
        prim = D_800D595C;
    } else {
        prim = D_800D58C0;
    }

    if (pos == 0) {
        x = pos;
    } else if (pos >= 1 && pos < 0x100) {
        x = pos - 0x100;
    } else if (pos >= 0x100 && pos <= 0x140) {
        x = pos - 0x200;
    }

    while (x < 0x140) {
        setPolyGT4(prim);
        setSemiTrans(prim, 1);
        prim->r0 = 0x80;
        prim->g0 = 0x80;
        prim->b0 = 0x80;
        prim->r1 = 0x80;
        prim->g1 = 0x80;
        prim->b1 = 0x80;
        prim->r2 = D_800DB0D0[2].r;
        prim->g2 = D_800DB0D0[2].g;
        prim->b2 = D_800DB0D0[2].b;
        prim->r3 = D_800DB0D0[2].r;
        prim->g3 = D_800DB0D0[2].g;
        prim->b3 = D_800DB0D0[2].b;
        prim->x0 = x;
        prim->x2 = x;
        prim->x1 = x + 0x100;
        prim->x3 = x + 0x100;
        prim->y0 = y;
        prim->y1 = y;
        prim->y2 = y + 0x40;
        prim->y3 = y + 0x40;
        prim->u0 = 0;
        prim->v0 = 0x40;
        prim->u1 = 0xFF;
        prim->v1 = 0x40;
        prim->u2 = 0;
        prim->v2 = 0x7F;
        prim->u3 = 0xFF;
        prim->v3 = 0x7F;
        prim->clut = getClut(0x340, 0xE0);
        prim->tpage = getTPage(0, 0, 0x380, 0);
        x += 0x100;
        setaddr(prim, getaddr(&D_800D244C->primList[BSC_HUD_IDX]));
        setaddr(&D_800D244C->primList[BSC_HUD_IDX], prim);
        prim++;
    }
}

/**
 * @brief Draw the map-view backdrop as two stacked gouraud quads.
 *
 * Fills the screen from the top down to @p y + 0x18 with a vertical
 * three-stop gradient, emitted as two full-width (0..0x140) @c POLY_G4
 * bands sharing their middle colour:
 *
 *   - band 1: @c y-0x50 .. @c y+0x18, palette stop 1 to stop 2
 *   - band 2: @c 0 .. @c y-0x50, palette stop 0 to stop 1
 *
 * Both are linked into the HUD layer @c BSC_HUD_IDX of the active scene.
 * The quads come from the pool @ref func_800ABC98 primed, picking the
 * @c D_800CA040 sentinel's pair when no battle scene is active.
 *
 * @note Dead code: reached only from the unreferenced @ref func_800A8400.
 *
 * @param y Baseline of the gradient, in screen coordinates.
 */
static void func_800A8A28(s16 y) {
    POLY_G4 *prim;

    if (D_800D244C == &D_800CA040) {
        prim = D_800D4F58;
    } else {
        prim = D_800D4F10;
    }

    setRGB0(prim, D_800DB0D0[1].r, D_800DB0D0[1].g, D_800DB0D0[1].b);
    setRGB1(prim, D_800DB0D0[1].r, D_800DB0D0[1].g, D_800DB0D0[1].b);
    setRGB2(prim, D_800DB0D0[2].r, D_800DB0D0[2].g, D_800DB0D0[2].b);
    setRGB3(prim, D_800DB0D0[2].r, D_800DB0D0[2].g, D_800DB0D0[2].b);
    prim->x0 = prim->x2 = 0;
    prim->x1 = prim->x3 = 0x140;
    prim->y0 = prim->y1 = y - 0x50;
    prim->y2 = prim->y3 = y + 0x18;
    setaddr(prim, getaddr(&D_800D244C->primList[BSC_HUD_IDX]));
    setaddr(&D_800D244C->primList[BSC_HUD_IDX], prim);
    prim++;

    setRGB0(prim, D_800DB0D0[0].r, D_800DB0D0[0].g, D_800DB0D0[0].b);
    setRGB1(prim, D_800DB0D0[0].r, D_800DB0D0[0].g, D_800DB0D0[0].b);
    setRGB2(prim, D_800DB0D0[1].r, D_800DB0D0[1].g, D_800DB0D0[1].b);
    setRGB3(prim, D_800DB0D0[1].r, D_800DB0D0[1].g, D_800DB0D0[1].b);
    prim->x0 = prim->x2 = 0;
    prim->x1 = prim->x3 = 0x140;
    prim->y0 = prim->y1 = 0;
    prim->y2 = prim->y3 = y - 0x50;
    setaddr(prim, getaddr(&D_800D244C->primList[BSC_HUD_IDX]));
    setaddr(&D_800D244C->primList[BSC_HUD_IDX], prim);
}

/** Bits of @c D_800D2440 that select which zone list to walk. */
#define WORLD_SCATTER_KEY_MASK 0x1F
/** Walking this far into the list means the zone is a named area, which is
 *  what @c D_800C5454 gates the location banner on. */
#define ZONE_LIST_NAMED_AREA 3
/** World distances are halved five times before the GTE squares them, so the
 *  squared sum stays inside the GTE's 32-bit accumulators. */
#define ZONE_DIST_SHIFT 5

/**
 * Blend one palette stop between the default zone's colour and this zone's,
 * by the fraction already loaded into IR0. The near colour is shifted up by
 * four to reach the GTE's 4.12 fixed point -- the same scaling
 * @c gte_SetFarColor applies to the far end for us.
 */
#define ZONE_BLEND(dst, k)                                                  \
    gte_SetFarColor(def->colors[k].r, def->colors[k].g, def->colors[k].b);  \
    work.vx = zone->colors[k].r << 4;                                       \
    work.vy = zone->colors[k].g << 4;                                       \
    work.vz = zone->colors[k].b << 4;                                       \
    gte_ldlvl(wp);                                                          \
    gte_lddp(dist);                                                         \
    gte_intpl();                                                            \
    gte_strgb(dst)

/**
 * @brief Fade the world palette towards whichever colour zone the camera is in.
 *
 * The world carries a list of colour zones (the @c D_800C9EE8 offset table,
 * whose entry 0 is the map-wide default). This walks the list from the entry
 * the current scatter key selects, and stops at the first zone whose centre is
 * within @c radius of the camera -- distance measured with the GTE's @c SQR on
 * the halved x/y delta.
 *
 * That zone then decides the scene's colour. Its five palette stops and its
 * 3x3 colour matrix are blended against the default's with the GTE's @c INTPL,
 * weighted by how far into the zone the camera has travelled, so the world
 * tints smoothly as you cross the boundary. With no zone in range the default's
 * values are installed unblended. Either way the result is programmed into the
 * GTE as the background colour and colour matrix.
 *
 * @note The @c goto is load-bearing: it is how the matched build skips the
 *       default-install block. Rewriting it as a "matched" flag costs three
 *       instructions and no longer matches.
 * @note Dead code in the retail build: nothing calls this, and its address
 *       appears nowhere in world.bin, so the @c D_800C4D24 early-out never
 *       gets the chance to matter.
 */
static void func_800A8C1C(void) {
    SVECTOR     near;
    VECTOR      work;
    WorldZone  *def;
    WorldZone  *zone;
    s32        *slot;
    s32         camX, camY;
    s32         dist;
    s32         radius;
    s32         idx;
    s32         j;
    s16        *matRows;
    SVECTOR    *np;
    VECTOR     *wp;

    camX = D_800C9868.x;
    camY = D_800C9868.y;
    /* The block is s32 offsets up front and WorldZone records behind them, so
     * reaching a record means stepping the base by a byte count and retyping. */
    def = (WorldZone *)((u8 *)D_800C9EE8 + D_800C9EE8[0]);

    if (D_800C4D24 != 0) {
        return;
    }

    D_800C5454 = 0;
    idx = 1;
    if ((D_800D2440 & WORLD_SCATTER_KEY_MASK) != 4) {
        idx = 2;
    }

    if (D_800C9EE8[idx] != 0) {
        /* Caching these keeps the scratch addresses and the matrix base in
         * saved registers for the whole walk, as the original does. */
        np = &near;
        wp = &work;
        matRows = D_800DA8B0.m[0];
        slot = &D_800C9EE8[idx];
        do {
            zone = (WorldZone *)((u8 *)D_800C9EE8 + *slot);
            near.vx = (zone->x - camX) >> ZONE_DIST_SHIFT;
            near.vy = (zone->y - camY) >> ZONE_DIST_SHIFT;
            near.vz = 0;
            gte_ldsv(np);
            gte_SQR(0);
            gte_stlvnl(wp);
            dist = SquareRoot0(work.vx + work.vy + work.vz);
            radius = zone->radius >> ZONE_DIST_SHIFT;
            if (dist < radius) {
                if (idx == ZONE_LIST_NAMED_AREA) {
                    D_800C5454 = 1;
                }
                /* How far in we are, as a 4.12 fraction for INTPL. The
                 * shift is spelled out again rather than reusing radius --
                 * the original reloads it here, and sharing the local costs
                 * the match. */
                dist *= ONE / (zone->radius >> ZONE_DIST_SHIFT);

                ZONE_BLEND(&D_800DB0E0, 0);
                ZONE_BLEND(&D_800DB0DC, 1);

                for (j = 0; j < 3; j++) {
                    work.vx = def->matrix[j][0];
                    work.vy = def->matrix[j][1];
                    work.vz = def->matrix[j][2];
                    gte_ldfc(wp);
                    gte_lddp(dist);
                    near.vx = zone->matrix[j][0];
                    near.vy = zone->matrix[j][1];
                    near.vz = zone->matrix[j][2];
                    gte_ldsv(np);
                    gte_intpl();
                    gte_stsv(&matRows[j * 3]);
                }

                ZONE_BLEND(&D_800DB0D0[0], 2);
                ZONE_BLEND(&D_800DB0D0[1], 3);
                ZONE_BLEND(&D_800DB0D0[2], 4);

                goto apply;
            }
            slot++;
            idx++;
        } while (*slot != 0);
    }

    /* No zone in range: install the default's colours unblended. */
    D_800DB0E0.r = def->colors[0].r;
    D_800DB0E0.g = def->colors[0].g;
    D_800DB0E0.b = def->colors[0].b;
    D_800DB0DC.r = def->colors[1].r;
    D_800DB0DC.g = def->colors[1].g;
    D_800DB0DC.b = def->colors[1].b;
    D_800DA8B0 = D_800C5428;
    for (idx = 0; idx < 3; idx++) {
        for (j = 0; j < 3; j++) {
            D_800DA8B0.m[idx][j] = def->matrix[idx][j];
        }
    }
    D_800DB0D0[0] = def->colors[2];
    D_800DB0D0[1] = def->colors[3];
    D_800DB0D0[2] = def->colors[4];

apply:
    SetBackColor(D_800DB0E0.r, D_800DB0E0.g, D_800DB0E0.b);
    SetColorMatrix(&D_800DA8B0);
}


/**
 * @brief Initialise three world-render pools in one go:
 *
 *  1. Reset @c limit and @c count to zero on every entry of the 64-slot
 *     @c D_800D9CB0 particle pool (mark all slots inactive/reusable).
 *  2. Prime each @c POLY_FT4 in @c D_800D88B0[2][64] with
 *     @c len = @c 9 and @c code = @c 0x2C.
 *  3. Prime each @c TILE in @c D_800DA8D0[2][64] with
 *     @c len = @c 3 and @c code = @c 0x60.
 *
 * Used at world setup time to prepare the per-frame prim templates.
 */
void func_800A9254(void) {
    Slot30 *p = D_800D9CB0;
    s32 j, i;

    while (p < &D_800D9CB0[64]) {
        p->limit = 0;
        p->count = 0;
        p++;
    }

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 64; i++) {
            setlen(&D_800D88B0[j][i], 9);
            setcode(&D_800D88B0[j][i], 0x2C);
            setlen(&D_800DA8D0[j][i], 3);
            setcode(&D_800DA8D0[j][i], 0x60);
        }
    }
}

/* Projects a slot into camera space. Declared here rather than pulled in from
 * we_object7.h: that unit prototypes it over its own TrackEntry view of the
 * same memory, which conflicts with Slot30. */
extern s32 func_800B01A0(s16 headingA, s16 headingB, Slot30 *slot,
                        SVECTOR *viewOut, s32 unused4, s32 unused5);

/** Depth past which a particle is dropped rather than linked into the OT. */
#define PARTICLE_MAX_OTZ 0x2000
/** Ordering-table slot for a projected depth. */
#define PARTICLE_OT_INDEX(otz) ((otz) >> 1)

/**
 * @brief Draw every live particle in the @c D_800D9CB0 pool.
 *
 * Walks all 64 slots; a slot is live while its @c count is still below its
 * @c limit. Each live slot is stepped by @ref func_800A9CC0, projected into
 * camera space, and then turned into one primitive whose size follows the
 * slot's own 4.12 @c scale times the kind's @c size.
 *
 * The kind's @c shape decides how the sprite sits in the world: the two
 * screen-aligned shapes project the centre point and lay a square around it
 * (@c PARTICLE_SHAPE_ROTATED first measuring the square by projecting a second
 * point one half-width away, so the sprite shrinks with distance), while the
 * three world-aligned shapes hand four corner offsets to @ref func_800ABEF0
 * and use the quad it projects.
 *
 * The finished sprite goes out as a flat @c TILE or, when the kind is
 * animated, a textured @c POLY_FT4 whose u/v pick the current frame out of a
 * 64-texel-wide strip. Both are linked into the scene's ordering table at the
 * projected depth. Each pool holds 64 prims per scene bank and simply stops
 * emitting when full.
 *
 * The GTE rotation matrix and translation vector are saved on entry and put
 * back on exit, so callers see the transform they set up.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears nowhere in world.bin -- the whole particle/backdrop layer
 *       (this, @ref func_800A7590 and the pool filler func_800A6BE0) is
 *       unreachable, like the map-view HUD layer above.
 */
static void func_800A9300(void) {
    DVECTOR     quad[4];
    DVECTOR     centre;
    SVECTOR     corners[4];
    VECTOR      savedTrans;
    MATRIX      savedMat;
    SVECTOR     span[3];
    SVECTOR     noRotation;
    MATRIX      spanMat;
    DVECTOR     spanScr[3];
    s32         bank;
    Slot30     *slot;
    KindParams *kp;
    POLY_FT4   *ft4;
    TILE       *tile;
    u32         otz;
    s32         half;
    s32         wide;
    s16         frame;
    s16         cell;
    s16         tex;
    s16         row;
    s16         headingB, headingA;

    bank = (D_800D244C == &D_800CA040);
    ft4 = &D_800D88B0[bank][0];
    tile = &D_800DA8D0[bank][0];

    headingA = func_800A5DC8(D_800D23C0.x, D_800D23C0.y);
    headingB = func_800A5DC8(D_800C9868.x, D_800C9868.y);

    gte_sttr(&savedTrans);
    gte_ReadRotMatrix(&savedMat);

    for (slot = D_800D9CB0; slot < &D_800D9CB0[64]; slot++) {
        if (slot->count >= slot->limit) {
            continue;
        }
        kp = &D_800C5480[slot->kind];
        func_800A9CC0(slot, kp);

        gte_SetRotMatrix(&D_800C9838);
        gte_SetTransVector(D_800C9838.t);
        if (!func_800B01A0(headingB, headingA, slot, &slot->view, 0, 0)) {
            continue;
        }

        half = slot->scale * kp->size / 4096 >> 1;

        if (kp->shape == PARTICLE_SHAPE_SCREEN) {
            gte_ldv0(&slot->view);
            gte_rtps();
            gte_stsxy(&centre);
            gte_stszotz(&otz);
            quad[3].vx = centre.vx - half;
            quad[0].vx = centre.vx - half;
            quad[2].vx = centre.vx + half;
            quad[1].vx = centre.vx + half;
            quad[1].vy = centre.vy - half;
            quad[0].vy = centre.vy - half;
            quad[3].vy = centre.vy + half;
            quad[2].vy = centre.vy + half;
        } else if (kp->shape == PARTICLE_SHAPE_ROTATED) {
            /* Measure the sprite on screen: project the centre and a point one
             * half-width to its side, and take the gap between them. */
            func_80047CE4(&noRotation, 0, sizeof(SVECTOR));
            span[2] = slot->view;
            span[1] = span[2];
            span[0] = span[1];
            span[1].vx += half;
            RotMatrix(&noRotation, &spanMat);
            gte_SetRotMatrix(&spanMat);
            gte_SetTransVector(D_800C9838.t);
            gte_ldv3c(span);
            gte_rtpt();
            gte_stsxy3c(spanScr);
            /* Read unsigned: the original takes the raw gap between the two
             * projected x's without sign-extending either. */
            wide = (u16)spanScr[1].vx - (u16)spanScr[0].vx;

            gte_SetRotMatrix(&D_800C9838);
            gte_ldv0(&slot->view);
            gte_rtps();
            gte_stsxy(&centre);
            gte_stszotz(&otz);
            quad[3].vx = centre.vx - wide;
            quad[0].vx = centre.vx - wide;
            quad[2].vx = centre.vx + wide;
            quad[1].vx = centre.vx + wide;
            quad[1].vy = centre.vy - wide;
            quad[0].vy = centre.vy - wide;
            quad[3].vy = centre.vy + wide;
            quad[2].vy = centre.vy + wide;
        } else if (kp->shape == PARTICLE_SHAPE_GROUND) {
            corners[3].vy = 0;
            corners[2].vy = 0;
            corners[1].vy = 0;
            corners[0].vy = 0;
            corners[3].vx = -half;
            corners[0].vx = -half;
            corners[2].vx = half;
            corners[1].vx = half;
            corners[1].vz = half;
            corners[0].vz = half;
            corners[3].vz = -half;
            corners[2].vz = -half;
            func_800ABEF0(corners, &slot->rot, &slot->view, quad, &otz);
            otz -= 4;
        } else if (kp->shape == PARTICLE_SHAPE_GROUND_2X) {
            corners[3].vx = -half;
            corners[0].vx = -half;
            corners[3].vy = 0;
            corners[2].vy = 0;
            corners[1].vy = 0;
            corners[0].vy = 0;
            corners[2].vx = half;
            corners[1].vx = half;
            corners[1].vz = half * 2;
            corners[0].vz = half * 2;
            corners[3].vz = -half * 2;
            corners[2].vz = -half * 2;
            func_800ABEF0(corners, &slot->rot, &slot->view, quad, &otz);
        } else if (kp->shape == PARTICLE_SHAPE_UPRIGHT) {
            corners[3].vz = 0;
            corners[2].vz = 0;
            corners[1].vz = 0;
            corners[0].vz = 0;
            corners[3].vx = -half;
            corners[0].vx = -half;
            corners[2].vx = half;
            corners[1].vx = half;
            corners[1].vy = -half;
            corners[0].vy = -half;
            corners[3].vy = half;
            corners[2].vy = half;
            func_800ABEF0(corners, &slot->rot, &slot->view, quad, &otz);
        }

        if (otz >= PARTICLE_MAX_OTZ) {
            continue;
        }

        if (kp->prim == 0 && tile < &D_800DACD0[bank * 64]) {
            setSemiTrans(tile, 0);
            tile->r0 = kp->r;
            tile->g0 = kp->g;
            tile->b0 = kp->b;
            tile->x0 = quad[0].vx;
            tile->y0 = quad[0].vy;
            tile->w = quad[1].vx - quad[0].vx;
            tile->h = quad[1].vx - quad[0].vx;
            tile->w += (tile->w == 0);
            tile->h += (tile->h == 0);
            addPrim(&D_800D244C->primList[PARTICLE_OT_INDEX(otz)], tile);
            tile++;
        } else if (kp->prim == 1 && ft4 < &D_800D92B0[bank * 64]) {
            frame = slot->count / kp->frameDiv & 0xFF;
            cell = frame;
            if (frame >= 0) {
                if (frame > kp->frameCount - 1) {
                    cell = kp->frameCount - 1;
                }
            } else {
                cell = 0;
            }
            setSemiTrans(ft4, 1);
            ft4->r0 = kp->r;
            ft4->g0 = kp->g;
            ft4->b0 = kp->b;
            tex = (kp->u + cell * (kp->w >> 2)) % 64 * 4;
            ft4->u2 = tex;
            ft4->u0 = tex;
            tex = kp->w + (kp->u + cell * (kp->w >> 2)) % 64 * 4 - 1;
            ft4->u3 = tex;
            ft4->u1 = tex;
            tex = kp->v % 256;
            ft4->v1 = tex;
            ft4->v0 = tex;
            row = kp->v % 256;
            tex = row + kp->h - 1;
            ft4->v3 = tex;
            ft4->v2 = tex;
            /* The projected points are already packed x,y pairs, so they go
             * across a word at a time; vertices 2 and 3 swap to give the
             * quad's triangle-strip winding. */
            *(u32 *)&ft4->x0 = *(u32 *)&quad[0];
            *(u32 *)&ft4->x1 = *(u32 *)&quad[1];
            *(u32 *)&ft4->x2 = *(u32 *)&quad[3];
            *(u32 *)&ft4->x3 = *(u32 *)&quad[2];
            ft4->tpage = getTPage(0, kp->tpageBits, kp->u + cell, kp->v);
            ft4->clut = getClut(kp->clutX, kp->clutY);
            addPrim(&D_800D244C->primList[PARTICLE_OT_INDEX(otz)], ft4);
            ft4++;
        }
    }

    gte_SetRotMatrix(&savedMat);
    gte_SetTransVector(&savedTrans);
}

/**
 * @brief Step one world-map effect particle.
 *
 * Advances @p p by one tick: multiplies its 4.12 @c scale by the kind's
 * @c scaleRate, integrates @c pos from the 16-bit @c vel, and then
 * accelerates @c vel by the kind's own @c vel and bumps @c count.
 * From the second tick on, particles of kind 12 or 13 additionally drift
 * with the camera: 5/6 of the per-frame camera deltas (@c D_800C9E38[0],
 * @c D_800C9870 - @c D_800C974C, @c D_800C9E38[2]) is added to the position
 * so the effect roughly follows the view (weather-style particles).
 *
 * @param p Particle slot to update.
 * @param q The kind's parameters, supplying the per-tick scale and velocity
 *          increments.
 *
 * @note The position words are updated through the walking pointer @c w on
 *       purpose: the multi-set pointer defeats gcc's alias base tracking,
 *       which keeps the volatile camera reads ordered after each position
 *       store, matching the original instruction schedule. The offsets
 *       still fold to plain 0x0/0x4/0x8 accesses.
 */
static void func_800A9CC0(Slot30 *p, KindParams *q) {
    s32 *w;

    p->scale = p->scale * q->scaleRate / 4096;
    p->pos.vx += p->vel.vx;
    p->pos.vy += p->vel.vy;
    p->pos.vz += p->vel.vz;
    if (p->count != 0) {
        if (p->kind == 12 || p->kind == 13) {
            w = &p->pos.vx;
            *w += D_800C9E38[0] * 5 / 6;
            w++;
            *w += (D_800C9870 - D_800C974C) * 5 / 6;
            w++;
            *w += D_800C9E38[2] * 5 / 6;
        }
    }
    p->vel.vx += q->vel.vx;
    p->vel.vy += q->vel.vy;
    p->vel.vz += q->vel.vz;
    p->count++;
}

/**
 * @brief Initialise the first record's 4 sub-OT slots in each of two
 *        pools for the entity model in @p a0.
 *
 * Runs @c DrawPrim on all four sub-slots of record 0 in the
 * @c D_800D3E50 and @c D_800D4090 pools, then dispatches
 * @c DrawSync(0). @c cond is the canonical entity bit — @c 0 for
 * @c &D_800CA040, @c 1 otherwise — selecting the pool row.
 */
void func_800A9E24(BattleSceneCtx *a0) {
    s32 cond = (a0 != &D_800CA040) ? 1 : 0;

    DrawPrim(&D_800D3E50[cond][0][0]);
    DrawPrim(&D_800D3E50[cond][0][1]);
    DrawPrim(&D_800D3E50[cond][0][2]);
    DrawPrim(&D_800D3E50[cond][0][3]);
    DrawPrim(&D_800D4090[cond][0][0]);
    DrawPrim(&D_800D4090[cond][0][1]);
    DrawPrim(&D_800D4090[cond][0][2]);
    DrawPrim(&D_800D4090[cond][0][3]);
    DrawSync(0);
}


/**
 * @brief Initialise two double-buffered prim pools with their tag headers.
 *
 * For each of the two pools at @c D_800D5A00[0..1] (POLY_GT4) and
 * @c D_800D7400[0..1] (POLY_GT3), walks all 64 slots and primes each
 * primitive's header:
 *  - @c POLY_GT4: @c len = @c 0xC (12 words), @c code = @c 0x3C.
 *  - @c POLY_GT3: @c len = @c 0x9 (9 words),  @c code = @c 0x34.
 *
 * Used to prime the prim templates before per-frame GPU command
 * generation fills in the colour/uv/xy fields.
 *
 * The @c p4 / @c p3 pointer locals shadow the array-index expressions
 * to force the right per-iteration register layout (a leftover idiom
 * from the original — without the locals the compiler hoists the
 * @c 0xC constant outside the inner loop, which doesn't match).
 */
void func_800A9ED4(void) {
    WorldPolyGT4 *p4;
    WorldPolyGT3 *p3;
    s32 j, i;
    for (j = 0; j < 2; j++) {
        for (i = 0; i < 64; i++) {
            p4 = &D_800D5A00[j][i];
            p3 = &D_800D7400[j][i];
            setPolyGT4(&D_800D5A00[j][i]);
            setPolyGT3(&D_800D7400[j][i]);
        }
    }
}

/* The mesh is a fixed 37-polygon outline: the first 25 rows of the index
 * table are quads, the remaining 12 are triangles. */
#define MESH_POLY_COUNT 37
#define MESH_QUAD_COUNT 25

/* One map cell spans 1024 world units, and world (0, 0) sits at the centre of
 * the 256 x 192 cell map -- so a world position converts to a cell with
 * cell = (world + origin) / MAP_CELL_SIZE. */
#define MAP_CELL_SIZE  1024
#define MAP_ORIGIN_X   (128 * MAP_CELL_SIZE)
#define MAP_ORIGIN_Y   (96 * MAP_CELL_SIZE)

/** Corner-to-centre bias of the mesh, published as its centre. */
#define MESH_CENTRE_BIAS 16

/**
 * @brief Draw the world-map inset mesh for world position @p pos at screen
 *        corner (@p x, @p y).
 *
 * Converts @p pos into map-cell coordinates (see @c MAP_ORIGIN_X and
 * @c MAP_CELL_SIZE), publishes the mesh centre in @c D_800D8808 /
 * @c D_800D880C, and points the two prim cursors at this frame's half of the
 * @c POLY_GT4 / @c POLY_GT3 pools — the second half when the active scene is
 * the worldmap sentinel @c D_800CA040.
 *
 * It then walks the @c MESH_POLY_COUNT rows of the outline table: each row
 * names its vertices in @c D_800C57F0, and every vertex carries a screen
 * offset (@c D_800C5884, biased by the requested corner) and a map-cell
 * offset (@c D_800C58D4, in half cells, biased by the position's cell). The
 * assembled vertices go to @ref func_800AA210 — a quad for the first
 * @c MESH_QUAD_COUNT rows, a triangle for the rest.
 *
 * @note Dead code in the retail build: nothing calls this and its address
 *       appears in no pointer table, so the mesh -- and @ref func_800AA210
 *       below it, which only this reaches -- never runs.
 *
 * @param pos World position the mesh is centred on.
 * @param x   Screen x of the mesh's top-left corner.
 * @param y   Screen y of the mesh's top-left corner.
 */
static void func_800A9F54(WorldPos *pos, s32 x, s32 y) {
    WorldTessVert verts[4];
    BattleSceneCtx *ctx;
    WorldPolyGT4 *gt4;
    WorldPolyGT3 *gt3;
    s32 mapX, mapY;
    s32 i, c;

    mapX = (pos->x + MAP_ORIGIN_X) / MAP_CELL_SIZE;
    mapY = (pos->y + MAP_ORIGIN_Y) / MAP_CELL_SIZE;
    D_800D8808 = x + MESH_CENTRE_BIAS;
    D_800D880C = y + MESH_CENTRE_BIAS;

    ctx = D_800D244C;
    gt4 = D_800D5A00[0];
    if (ctx == &D_800CA040) {
        gt4 = D_800D5A00[1];
    }
    D_800D8800 = gt4;

    gt3 = D_800D7400[0];
    if (ctx == &D_800CA040) {
        gt3 = D_800D7400[1];
    }
    D_800D8804 = gt3;

    for (i = 0; i < MESH_POLY_COUNT; i++) {
        if (i < MESH_QUAD_COUNT) {
            for (c = 0; c < 4; c++) {
                verts[c].sx = (s8)D_800C5884[D_800C57F0[i][c]][0] + x;
                verts[c].sy = (s8)D_800C5884[D_800C57F0[i][c]][1] + y;
                verts[c].mx = (s8)D_800C58D4[D_800C57F0[i][c]][0] * 2 + mapX;
                verts[c].my = (s8)D_800C58D4[D_800C57F0[i][c]][1] * 2 + mapY;
            }
            func_800AA210(verts, 1);
        } else {
            for (c = 0; c < 3; c++) {
                verts[c].sx = (s8)D_800C5884[D_800C57F0[i][c]][0] + x;
                verts[c].sy = (s8)D_800C5884[D_800C57F0[i][c]][1] + y;
                verts[c].mx = (s8)D_800C58D4[D_800C57F0[i][c]][0] * 2 + mapX;
                verts[c].my = (s8)D_800C58D4[D_800C57F0[i][c]][1] * 2 + mapY;
            }
            func_800AA210(verts, 0);
        }
    }
}

/** Width of the world map in cells; map x wraps modulo this. */
#define MAP_CELLS_X 256
/** Height of the world map in cells; map y wraps modulo this. */
#define MAP_CELLS_Y 192

/**
 * @brief Emit one mesh polygon, splitting it first if it straddles a map seam.
 *
 * The world map is a @c MAP_CELLS_X x @c MAP_CELLS_Y torus, so a polygon near
 * an edge can have vertices on both sides of the wrap. This first slides the
 * whole polygon back into range (if every vertex is off the same side), then
 * re-measures its bounding box. With no seam crossing left it shades and emits
 * directly; otherwise a quad is cut along the seam and each half is re-emitted
 * through a recursive call, once per axis. Triangles are never split — they
 * fall through to the emit path with their map coordinates zeroed.
 *
 * Shading is per vertex: the squared distance from the mesh centre
 * (@c D_800D8808 / @c D_800D880C) goes through the GTE's @c SQR, and the
 * resulting length is inverted into a grey level, so the inset fades out
 * towards its edges.
 *
 * @param verts  Polygon vertices, three for a triangle and four for a quad.
 *               Wrapped in place, and the triangle path clears the map
 *               coordinates before emitting.
 * @param isQuad Nonzero to treat @p verts as a quad (four vertices).
 *
 * @note @c loHalf and @c hiHalf are never read; they only reserve the frame
 *       space the retail build allocates.
 * @note The four seam interpolations read the base screen coordinate as
 *       @c u16 on purpose -- the original loads it unsigned and only the
 *       interpolated term is signed.
 * @note Dead code in the retail build: the only caller is
 *       @ref func_800A9F54, which is itself unreachable.
 */
static void func_800AA210(WorldTessVert *verts, s32 isQuad) {
    WorldTessVert loHalf[4];
    WorldTessVert hiHalf[4];
    SVECTOR dist;
    /* Scratch for the two halves of a split: the y split fills part[1..4] and
     * the x split part[0..3], so each recursive call gets four contiguous
     * vertices. On the emit paths the split is over and the same storage
     * carries the shade bytes instead. */
    WorldTessVert part[5];
    s16 minX, minY, maxX, maxY;
    s32 spanX, spanY;
    s32 i;
    s32 seam;

    spanY = spanX = 0;

    minY = 0x7FFF;
    maxY = -0x8000;
    minX = minY;
    maxX = maxY;
    for (i = 0; i < isQuad + 3; i++) {
        if (verts[i].mx < minX) minX = verts[i].mx;
        if (verts[i].my < minY) minY = verts[i].my;
        if (maxX < verts[i].mx) maxX = verts[i].mx;
        if (maxY < verts[i].my) maxY = verts[i].my;
    }

    /* Wholly off one edge: slide the polygon back onto the map. */
    if (maxX < 0) {
        for (i = 0; i < isQuad + 3; i++) verts[i].mx += MAP_CELLS_X;
    }
    if (minX >= MAP_CELLS_X) {
        for (i = 0; i < isQuad + 3; i++) verts[i].mx -= MAP_CELLS_X;
    }
    if (maxY < 0) {
        for (i = 0; i < isQuad + 3; i++) verts[i].my += MAP_CELLS_Y;
    }
    if (minY >= MAP_CELLS_Y) {
        for (i = 0; i < isQuad + 3; i++) verts[i].my -= MAP_CELLS_Y;
    }

    minY = minX = 0x7FFF;
    maxY = maxX = -0x8000;
    for (i = 0; i < isQuad + 3; i++) {
        if (verts[i].mx < minX) minX = verts[i].mx;
        if (verts[i].my < minY) minY = verts[i].my;
        if (maxX < verts[i].mx) maxX = verts[i].mx;
        if (maxY < verts[i].my) maxY = verts[i].my;
    }

    if (minX < 0 || maxX >= MAP_CELLS_X) spanX = 1;
    if (minY < 0 || maxY >= MAP_CELLS_Y) spanY = 1;

    if (!spanX && !spanY) {
        /* The mesh centre is published as a word but only ever holds a screen
         * coordinate, so the distance is taken from its low halfword. */
        for (i = 0; i < isQuad + 3; i++) {
            dist.vx = (u16)D_800D8808 - verts[i].sx;
            dist.vy = (u16)D_800D880C - verts[i].sy;
            dist.vz = 0;
            gte_ldsv(&dist);
            gte_SQR(0);
            gte_stsv(&dist);
            ((u8 *)part)[i] =
                ~(SquareRoot0(dist.vx + dist.vy + dist.vz) << 2);
        }
        if (isQuad) {
            func_800AAEAC((WorldVtx *)verts, (QuadShade *)part);
            D_800D8800++;
            return;
        }
        func_800AAD48((WorldVtx *)verts, (TriShade *)part);
        D_800D8804++;
        return;
    }

    if (isQuad) {
        if (spanY) {
            /* Cut along the horizontal seam: part[1..2] is the half that wraps
             * round to the far edge, part[3..4] the half that stays put, and
             * the shared edge sits on the last row. Its screen y is the linear
             * interpolation of the polygon's leading edge at that row. */
            if (verts[0].my < 0) {
                part[3] = verts[0];
                part[1] = part[3];
                part[4] = verts[1];
                part[2] = part[4];
                part[3].my = MAP_CELLS_Y - 1;
                part[4].my = MAP_CELLS_Y - 1;
                part[1].my += MAP_CELLS_Y;
                part[2].my += MAP_CELLS_Y;
                seam = (u16)part[1].sy
                     + (verts[2].sy - verts[0].sy) * (part[3].my - part[1].my)
                     / (verts[2].my - verts[0].my);
                part[4].sy = seam;
                part[3].sy = seam;
                func_800AA210(&part[1], isQuad);

                part[1] = part[3];
                part[2] = part[4];
                part[2].my = 0;
                part[1].my = 0;
                part[3] = verts[2];
                part[4] = verts[3];
                func_800AA210(&part[1], isQuad);
                return;
            }
            part[3] = verts[0];
            part[1] = part[3];
            part[4] = verts[1];
            part[2] = part[4];
            part[3].my = MAP_CELLS_Y - 1;
            part[4].my = MAP_CELLS_Y - 1;
            seam = (u16)part[1].sy
                 + (verts[2].sy - verts[0].sy) * (part[3].my - part[1].my)
                 / (verts[2].my - verts[0].my);
            part[4].sy = seam;
            part[3].sy = seam;
            func_800AA210(&part[1], isQuad);

            part[1] = part[3];
            part[2] = part[4];
            part[2].my = 0;
            part[1].my = 0;
            part[3] = verts[2];
            part[4] = verts[3];
            part[3].my -= MAP_CELLS_Y;
            part[4].my -= MAP_CELLS_Y;
            func_800AA210(&part[1], isQuad);
            return;
        }
        if (!spanX) {
            return;
        }
        /* Same cut along the vertical seam, on the last column. */
        if (verts[0].mx < 0) {
            part[1] = verts[0];
            part[0] = part[1];
            part[3] = verts[2];
            part[2] = part[3];
            part[1].mx = MAP_CELLS_X - 1;
            part[3].mx = MAP_CELLS_X - 1;
            part[0].mx += MAP_CELLS_X;
            part[2].mx += MAP_CELLS_X;
            seam = (u16)part[0].sx
                 + (verts[1].sx - verts[0].sx) * (part[1].mx - part[0].mx)
                 / (verts[1].mx - verts[0].mx);
            part[3].sx = seam;
            part[1].sx = seam;
            func_800AA210(part, isQuad);

            part[0] = part[1];
            part[2] = part[3];
            part[2].mx = 0;
            part[0].mx = 0;
            part[1] = verts[1];
            part[3] = verts[3];
            func_800AA210(part, isQuad);
            return;
        }
        part[1] = verts[0];
        part[0] = part[1];
        part[3] = verts[2];
        part[2] = part[3];
        part[1].mx = MAP_CELLS_X - 1;
        part[3].mx = MAP_CELLS_X - 1;
        seam = (u16)part[0].sx
             + (verts[1].sx - verts[0].sx) * (part[1].mx - part[0].mx)
             / (verts[1].mx - verts[0].mx);
        part[3].sx = seam;
        part[1].sx = seam;
        func_800AA210(part, isQuad);

        part[0] = part[1];
        part[2] = part[3];
        part[2].mx = 0;
        part[0].mx = 0;
        part[1] = verts[1];
        part[3] = verts[3];
        part[1].mx -= MAP_CELLS_X;
        part[3].mx -= MAP_CELLS_X;
        func_800AA210(part, isQuad);
        return;
    }

    /* Triangle straddling a seam: it is emitted whole, with the split scratch
     * doing double duty -- part[0] as the GTE distance vector and part[1]
     * onward as the shade bytes.
     *
     * Addressing the shade through &part[1] is load-bearing, not laziness: an
     * overlay struct with a shade field at +8 lets gcc fold the displacement
     * into the store, whereas the original computes the shade base once and
     * reuses it for both the loop store and the call below. */
    for (i = 0; i < isQuad + 3; i++) {
        ((SVECTOR *)part)->vx = (u16)D_800D8808 - verts[i].sx;
        ((SVECTOR *)part)->vy = (u16)D_800D880C - verts[i].sy;
        ((SVECTOR *)part)->vz = 0;
        gte_ldsv((SVECTOR *)part);
        gte_SQR(0);
        gte_stsv((SVECTOR *)part);
        ((u8 *)&part[1])[i] = ~(SquareRoot0(((SVECTOR *)part)->vx
                                            + ((SVECTOR *)part)->vy
                                            + ((SVECTOR *)part)->vz) << 2);
    }
    verts[0].my = 0;
    verts[0].mx = 0;
    verts[1].my = 0;
    verts[1].mx = 0;
    verts[2].my = 0;
    verts[2].mx = 0;
    func_800AAD48((WorldVtx *)verts, (TriShade *)&part[1]);
    D_800D8804++;
}

/**
 * @brief Fill the current POLY_GT3 (@c D_800D8804) from a transformed
 *        triangle and splice it onto the main OT chain.
 *
 * Writes a grayscale gouraud colour per vertex (@p shade byte copied to
 * r=g=b), the three packed screen words and texture coordinates from
 * @p vtx[0..2], a fixed CLUT id of @c 0x3A74 and texture page @c 0xC,
 * then links the primitive to @c D_800D244C's main chain head with the
 * PsyQ @c setaddr / @c getaddr tag operations (standard @c addPrim
 * semantics).
 *
 * The prim pointer is re-read from @c D_800D8804 around every byte-wide
 * store because gcc must assume those stores may alias the pointer
 * global — the word/half stores between them reuse the cached value.
 *
 * @param vtx   Three transformed vertices (packed screen word + u/v).
 * @param shade Per-vertex grayscale shade triple.
 */
static void func_800AAD48(WorldVtx *vtx, TriShade *shade) {
    D_800D8804->r0 = shade->c0;
    D_800D8804->g0 = shade->c0;
    D_800D8804->b0 = shade->c0;
    D_800D8804->r1 = shade->c1;
    D_800D8804->g1 = shade->c1;
    D_800D8804->b1 = shade->c1;
    D_800D8804->r2 = shade->c2;
    D_800D8804->g2 = shade->c2;
    D_800D8804->b2 = shade->c2;
    D_800D8804->xy0 = vtx[0].xy;
    D_800D8804->xy1 = vtx[1].xy;
    D_800D8804->xy2 = vtx[2].xy;
    D_800D8804->u0 = vtx[0].u;
    D_800D8804->v0 = vtx[0].v;
    D_800D8804->u1 = vtx[1].u;
    D_800D8804->v1 = vtx[1].v;
    D_800D8804->u2 = vtx[2].u;
    D_800D8804->v2 = vtx[2].v;
    D_800D8804->clut = 0x3A74;
    D_800D8804->tpage = 0xC;
    setaddr(D_800D8804, getaddr(&D_800D244C->primList[BSC_OTHEAD_IDX]));
    setaddr(&D_800D244C->primList[BSC_OTHEAD_IDX], D_800D8804);
}


/**
 * @brief Fill the current POLY_GT4 (@c D_800D8800) from a transformed
 *        quad and splice it onto the main OT chain.
 *
 * Quad twin of @ref func_800AAD48: writes a grayscale gouraud colour per
 * vertex (@p shade byte copied to r=g=b), the four packed screen words
 * and texture coordinates from @p vtx[0..3], a fixed CLUT id of
 * @c 0x3A74 and texture page @c 0xC, then links the primitive to
 * @c D_800D244C's main chain head with the PsyQ @c setaddr / @c getaddr
 * tag operations (standard @c addPrim semantics).
 *
 * @param vtx   Four transformed vertices (packed screen word + u/v).
 * @param shade Per-vertex grayscale shade quad.
 */
static void func_800AAEAC(WorldVtx *vtx, QuadShade *shade) {
    D_800D8800->r0 = shade->c0;
    D_800D8800->g0 = shade->c0;
    D_800D8800->b0 = shade->c0;
    D_800D8800->r1 = shade->c1;
    D_800D8800->g1 = shade->c1;
    D_800D8800->b1 = shade->c1;
    D_800D8800->r2 = shade->c2;
    D_800D8800->g2 = shade->c2;
    D_800D8800->b2 = shade->c2;
    D_800D8800->r3 = shade->c3;
    D_800D8800->g3 = shade->c3;
    D_800D8800->b3 = shade->c3;
    D_800D8800->xy0 = vtx[0].xy;
    D_800D8800->xy1 = vtx[1].xy;
    D_800D8800->xy2 = vtx[2].xy;
    D_800D8800->xy3 = vtx[3].xy;
    D_800D8800->u0 = vtx[0].u;
    D_800D8800->v0 = vtx[0].v;
    D_800D8800->u1 = vtx[1].u;
    D_800D8800->v1 = vtx[1].v;
    D_800D8800->u2 = vtx[2].u;
    D_800D8800->v2 = vtx[2].v;
    D_800D8800->u3 = vtx[3].u;
    D_800D8800->v3 = vtx[3].v;
    D_800D8800->clut = 0x3A74;
    D_800D8800->tpage = 0xC;
    setaddr(D_800D8800, getaddr(&D_800D244C->primList[BSC_OTHEAD_IDX]));
    setaddr(&D_800D244C->primList[BSC_OTHEAD_IDX], D_800D8800);
}




/** Fixed world position the kind-11 particle is spawned at, a shade over six
 *  tiles east and thirteen south of the map origin. */
static const VECTOR D_8009814C = { 6 * WORLD_TILE_SIZE + 29, -800,
                                   13 * WORLD_TILE_SIZE + 50, 0 };

/**
 * @brief Spawn a kind-11 particle at the fixed world position @c D_8009814C.
 *
 * Claims the first inactive slot in the @c D_800D9CB0 pool (count >= limit),
 * seeds it with kind 11, a zeroed rotation, @c scale = @c PARTICLE_SCALE_ONE and the kind's
 * @c limit from @c D_800C5480[11], jitters scale by [-0x80, +0x7F] and limit
 * by [-4, +3] (both jitter flags are set here), then projects the kind's
 * @c offset vector through a rotation matrix built from the zeroed angles and
 * stores the result as the slot's per-tick velocity. Always restores
 * @c D_800C9838 as the GTE rotation and translation matrix on exit.
 *
 * This is @ref func_800AB2D4 with the caller-supplied position replaced by a
 * constant and the one-in-256 spawn gate dropped, so it fires on every call.
 *
 * @note Dead code in the retail build: nothing calls it, and its address
 *       appears nowhere in world.bin.
 *
 * @note The spellings below are load-bearing for the byte-exact match
 *       (gcc 2.8.0 register allocation), not behavior. Each was confirmed by
 *       removing it on its own; the percentage is what the match drops to:
 *       - @c pp carries @c &local_pos into the loop, which is where the
 *         original reads the position from; reading @c local_pos directly
 *         addresses it off @c sp and loses the pointer.
 *       - @c rp is assigned inside the claimed-slot block, after the scan
 *         loop. Assigning it up front lets cse share one register with the
 *         @c memset argument, which then costs a fifth saved register.
 *       - the @c do / @c while(0) around the @c rot_in read (97.14) gives cse
 *         a block boundary so that copy's address temp is not reused as the
 *         @c RotMatrix first argument, which must rematerialize.
 *       - @c t holding @c flags & @c SLOT_FLAG_JITTER_SCALE across that copy
 *         (98.60), and @c proj_p holding @c &projected (96.83).
 *       - @c kind as a variable rather than a literal subscript (99.08).
 *       - the dead @c mp += flags & SLOT_FLAG_UNUSED4 (never set, so it folds
 *         to nothing) invalidates cse's record of @c &m so the
 *         @c gte_SetRotMatrix / @c gte_SetTransMatrix operands rematerialize
 *         @c &m after the @c RotMatrix call instead of holding it across it
 *         (96.98).
 */
static void func_800AB06C(void) {
    VECTOR      local_pos;
    SVECTOR     rot_in;
    VECTOR      projected;
    SVECTOR     vec_copy;
    MATRIX      m;
    Slot30     *slot;
    KindParams *kp;
    SVECTOR    *rp;
    VECTOR     *proj_p;
    VECTOR     *pp;
    s32         flags;
    s32         kind;
    s32         t;
    MATRIX     *mp;

    local_pos = D_8009814C;
    func_80047CE4(&rot_in, 0, sizeof(SVECTOR));

    slot = D_800D9CB0;
    proj_p = &projected;
    pp = &local_pos;
    kind = 11;
    kp = &D_800C5480[kind];
    flags = SLOT_FLAG_JITTER_SCALE | SLOT_FLAG_JITTER_LIMIT;

    while (slot < &D_800D9CB0[64]) {
        if (slot->count >= slot->limit) break;
        slot++;
    }
    if (slot < &D_800D9CB0[64]) {
        slot->kind  = 11;
        slot->count = 0;
        slot->limit = kp->limit;
        slot->scale = PARTICLE_SCALE_ONE;
        slot->pos.vx = pp->vx;
        slot->pos.vy = pp->vy;
        slot->pos.vz = pp->vz;

        rp = &rot_in;
        t = flags & SLOT_FLAG_JITTER_SCALE;
        do {
            vec_copy = *rp;
        } while (0);

        if (t) {
            slot->scale += func_8009CC3C() - 0x80;
        }
        if (flags & SLOT_FLAG_JITTER_LIMIT) {
            slot->limit += (func_8009CC3C() & 7) - 4;
        }

        slot->rot = vec_copy;

        mp = &m;
        RotMatrix(&vec_copy, mp);
        mp += flags & SLOT_FLAG_UNUSED4;
        gte_SetRotMatrix(&m);
        m.t[2] = 0;
        m.t[1] = 0;
        m.t[0] = 0;
        gte_SetTransMatrix(&m);

        gte_ldv0(&kp->offset);
        gte_mvmva(1, 0, 0, 0, 0);
        gte_stlvnl(proj_p);

        slot->vel.vx = projected.vx;
        slot->vel.vy = projected.vy;
        slot->vel.vz = projected.vz;
    }

    SetRotMatrix(&D_800C9838);
    SetTransMatrix(&D_800C9838);
}

/**
 * @brief Spawn a kind-0xE particle in the @c D_800D9CB0 pool at @p pos.
 *
 * Gated to fire on roughly 1 in 256 calls (@c func_8009CC3C returns
 * 0..0x7FFF). Claims the first inactive slot (count >= limit), seeds it
 * with kind 0xE, a zeroed rotation, @c scale = @c PARTICLE_SCALE_ONE and the
 * kind's @c limit from @c D_800C5480[14], jitters scale by [-0x80, +0x7F] and
 * limit by [-4, +3] (both jitter flags always set here), then
 * projects the kind's @c offset vector through a rotation matrix built
 * from the zeroed angles and stores the result as the slot's per-tick
 * velocity. Always restores @c D_800C9838 as the GTE rotation and
 * translation matrix on exit.
 *
 * @note Dead code in the retail build: the only caller, func_800B99A4
 *       (we_object8, an ambient spawner that picks a random world cell
 *       and checks its terrain glyph), is itself never referenced.
 *
 * @note Three spellings below are load-bearing for the byte-exact match
 *       (gcc 2.8.0 register allocation), not behavior:
 *       - @c t + the @c do/while(0) around the @c vec_copy read give
 *         cse a basic-block boundary so the copy's address temp is not
 *         reused for the RotMatrix first argument (which must
 *         rematerialize as a fresh @c addiu a0).
 *       - @c mp carries @c &m into the call argument, and the dead
 *         @c mp += flags & SLOT_FLAG_UNUSED4 (never set, so this folds to
 *         nothing) invalidates cse's record of @c &m so the
 *         @c gte_SetRotMatrix / @c gte_SetTransMatrix operands
 *         rematerialize @c &m in a fresh temp after the call instead of
 *         holding it in a saved register across it.
 *
 * @param pos  World position for the new particle (copied, only x/y/z used).
 */
void func_800AB2D4(VECTOR *pos) {
    SVECTOR     rot_in;
    VECTOR      local_pos;
    VECTOR      projected;
    SVECTOR     vec_copy;
    MATRIX      m;
    Slot30     *slot;
    KindParams *kp;
    SVECTOR    *rp;
    VECTOR     *proj_p;
    s32         flags;
    s32         kind;
    MATRIX     *mp;
    s32         t;

    func_80047CE4(&rot_in, 0, sizeof(SVECTOR));
    local_pos = *pos;

    if (func_8009CC3C() < 0x80) {
        slot = D_800D9CB0;
        proj_p = &projected;
        rp = &rot_in;
        kind = 0xE;
        kp = &D_800C5480[kind];
        flags = SLOT_FLAG_JITTER_SCALE | SLOT_FLAG_JITTER_LIMIT;

        while (slot < &D_800D9CB0[64]) {
            if (slot->count >= slot->limit) break;
            slot++;
        }
        if (slot < &D_800D9CB0[64]) {
            slot->kind  = 0xE;
            slot->count = 0;
            slot->limit = kp->limit;
            slot->scale  = PARTICLE_SCALE_ONE;
            slot->pos.vx = local_pos.vx;
            slot->pos.vy = local_pos.vy;
            slot->pos.vz = local_pos.vz;

            t = flags & SLOT_FLAG_JITTER_SCALE;
            do {
                vec_copy = *rp;
            } while (0);

            if (t) {
                slot->scale += func_8009CC3C() - 0x80;
            }
            if (flags & SLOT_FLAG_JITTER_LIMIT) {
                slot->limit += (func_8009CC3C() & 7) - 4;
            }

            slot->rot = vec_copy;

            mp = &m;
            RotMatrix(&vec_copy, mp);
            mp += flags & SLOT_FLAG_UNUSED4;
            gte_SetRotMatrix(&m);
            m.t[2] = 0;
            m.t[1] = 0;
            m.t[0] = 0;
            gte_SetTransMatrix(&m);

            gte_ldv0(&kp->offset);
            gte_mvmva(1, 0, 0, 0, 0);
            gte_stlvnl(proj_p);

            slot->vel.vx = projected.vx;
            slot->vel.vy = projected.vy;
            slot->vel.vz = projected.vz;
        }
    }

    SetRotMatrix(&D_800C9838);
    SetTransMatrix(&D_800C9838);
}
