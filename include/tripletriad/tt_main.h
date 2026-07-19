#ifndef TT_MAIN_H
#define TT_MAIN_H

#include "common.h"
#include "tripletriad.h"
#include "tim.h"          /* Tim / TimSection (queued VRAM image uploads) */

/* Declarations for tt_main.c: the encounter-param latch, session init and
   main loop, draw-buffer/GTE setup, deferred VRAM transfers, the scratch-pool
   allocator, and the pool-backed object-list system. The debug-text overlay,
   3D icon, and card-flip/hand setup live in tt_text.h (tt_text.c). */

/* ───────────────────────────── Public ──────────────────────────────────── */

/* Public typedefs */

/** @brief Triple Triad state-handler node: sub-state selector, frame counter, and
 *  phase bit (which side the card is showing). Shared between the card-flip
 *  handler (tt_text.c) and the match-flow driver (tt_match.c). */
typedef struct {
    u8    pad00[0x0C];
    void *subHandler;  /* 0x0C — spawned per-turn sub-handler node           */
    u8    state;       /* 0x10 — sub-state                                   */
    u8    counter;     /* 0x11 — per-state frame counter                     */
    u8    phase;       /* 0x12 — flip phase / card side                      */
    u8    rulesFlags;  /* 0x13 — applyCardRules result carried across frames */
    u8    pad14;       /* 0x14 */
    u8    retryFlag;   /* 0x15 — set when a rule pass triggered captures     */
    u8    pad16[2];    /* 0x16 */
} HandlerNode;

typedef struct { u8 r, g, b; } RGB;

/** @brief @c HandlerNode.state values for the card-flip handler (@ref cardFlipHandler,
 *  defined in tt_text.c). The match-flow driver (tt_match.c) also arms these. */
typedef enum {
    CARD_FLIP_INIT   = 0,  /**< Pick the flip phase and seed the transform.   */
    CARD_FLIP_ENTER  = 1,  /**< Entry arc: spin and dip the card into place.  */
    CARD_FLIP_IDLE   = 2,  /**< Settled pose; waits for a phase change.       */
    CARD_FLIP_REFLIP = 3,  /**< Swing to show the other face.                 */
} CardFlipState;

/** @brief Triple Triad top-level state-handler signature (entries of @c g_tripleTriadStateHandlers). */
typedef u8 *(*TripleTriadStateFn)(void);

/* Public data */
extern s8 g_tripleTriadCardCounts[];  /**< Per-card owned count (signed; <0 = not in collection). */
extern TripleTriadStateFn g_tripleTriadStateHandlers[];  /**< Triple Triad top-level state-handler table. */

/* Public prototypes */

/* Pool-backed object list (nodes carved from a fixed pool; see ObjList). */
extern void  initObjList(ObjList *list, u8 *pool, s32 stride, s32 count);
extern void *allocObjNode(ObjList *list, ObjNodeFn callback);
extern void *allocObjNodeFront(ObjList *list, ObjNodeFn callback);
extern s32   updateObjectList(ObjList *list);

/** @brief Scratch-pool allocator (LIFO): reserve / release @c size bytes of work RAM. */
extern s32   scratchAlloc(s32 size);
extern void  scratchFree(s32 size);

/** @brief Queue a deferred LoadImage for the next @c flushVramTransfers. */
extern void  queueLoadImage(RECT *rect, void *src);

/* Card-flip animation handler + per-match card setup (defined in tt_text.c;
   declared here as the cross-TU entry points called from tt_match.c). */
extern s32  cardFlipHandler(HandlerNode *node);
extern void initCardHands(void);

/* ───────── Private (only used in tt_main.c; may move into the .c) ─────── */

/* Private enums / defines / consts */

/** @brief OT entries per buffer (g_orderingTables). */
#define TT_OT_LEN 28

/* Display geometry used by initGraphics. The two draw buffers sit side by side
   in VRAM (buffer 1 starts at x = TT_DRAW_W); only a TT_SCREEN_W-wide window is
   shown on screen, and the GTE projection is centered on the draw buffer. */
#define TT_DRAW_W    384  /**< Draw-buffer width in VRAM; also the X offset to buffer 1. */
#define TT_SCREEN_W  256  /**< Visible display width. */
#define TT_SCREEN_H  224  /**< Display / draw-buffer height. */
#define TT_SCREEN_Y    8  /**< Display vertical offset. */
#define TT_PROJ_DIST 512  /**< GTE projection distance (SetGeomScreen). */

/** @brief Deferred VRAM transfer dispatched by @c flushVramTransfers. */
typedef enum {
    POOL_LOAD_IMAGE  = 0,  /**< LoadImage(rect, src). */
    POOL_LOAD_TIM    = 1,  /**< LoadImage twice from a TIM file (CLUT + image). */
    POOL_STORE_IMAGE = 2,  /**< StoreImage(rect, dst). */
    POOL_MOVE_IMAGE  = 3   /**< MoveImage(rect, dstX, dstY) with @c src packed as @c y<<16|x. */
} PoolAction;

/* Private typedefs */

typedef struct {
    u32 type;
    u32 size;
    s32 offset;
} ResHeader;

typedef struct {
    u8   active;        /**< @c PoolAction enum value. */
    u8   pad01[3];
    RECT rect;          /**< Destination/source RECT (unused when @c active == POOL_LOAD_TIM). */
    void *src;          /**< TIM pointer / pixel buffer / packed dest coords (depends on @c active). */
} PoolEntry;

/* Private data — deferred VRAM transfer pool (flushVramTransfers) */
extern PoolEntry     g_vramQueue[];
extern ResHeader     g_tripleTriadCardFrames;  /**< Card frame/border graphics (4bpp TIM, uploaded to VRAM at init). */
extern ResHeader     g_tripleTriadCardArt;     /**< Card face artwork (8bpp TIM, ~110 cards at 64x64, uploaded to VRAM at init). */

/* Private data — draw buffers / VRAM scratch */
extern RECT          g_fbClearRect;
extern RECT          g_texClearRect;
extern u32           g_orderingTables[2][TT_OT_LEN];  /**< Per-buffer ordering tables (OT). */
extern u8            g_primPools[2][0x10000];  /* primitive pool, 64KB per buffer */
extern u8           *g_tripleTriadActiveList;

/* Private data — engine misc state */
extern s32           g_vramQueueCount;
extern s32           g_scratchPtr;
extern u8            g_vsyncMode;        /**< VSync() wait mode (0 = wait one vblank). */

/* Private prototypes (tt_main.c entry points forward-declared for earlier callers) */
extern void resetVramQueue(void);
extern void flushVramTransfers(void);
extern u8  *queueTimUpload(ResHeader *res);
extern void queueStoreImage(RECT *rect, void *dst);
extern void queueMoveImage(RECT *rect, s16 dstX, u16 dstY);
extern void *findFreeNode(ObjList *list);

#endif /* TT_MAIN_H */
