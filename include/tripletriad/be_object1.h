#ifndef TRIPLETRIAD_BE_OBJECT1_H
#define TRIPLETRIAD_BE_OBJECT1_H

#include "common.h"
#include "tripletriad.h"
#include "tim.h"          /* Tim / TimSection (queued VRAM image uploads) */

/* Declarations for be_object1.c (Triple Triad board/card setup, draw-buffer
   init, deferred VRAM transfers, the object-list system, debug text, and the
   tetrahedral 3D icon). */

/* ───────────────────────────── Public ──────────────────────────────────── */

/* Public typedefs */

/** @brief Triple Triad state-handler node: sub-state selector, frame counter, and
 *  phase bit (which side the card is showing). Shared between the card-flip
 *  handler (be_object1.c) and the match-flow driver (be_object1b.c). */
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

/* Card-flip animation handler + per-match card setup. */
extern s32  cardFlipHandler(HandlerNode *node);
extern void initCardHands(void);

/* ───────── Private (only used in be_object1.c; may move into the .c) ─────── */

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

/** @brief @c HandlerNode.state values for the card-flip handler (@ref cardFlipHandler). */
typedef enum {
    CARD_FLIP_INIT   = 0,  /**< Pick the flip phase and seed the transform.   */
    CARD_FLIP_ENTER  = 1,  /**< Entry arc: spin and dip the card into place.  */
    CARD_FLIP_IDLE   = 2,  /**< Settled pose; waits for a phase change.       */
    CARD_FLIP_REFLIP = 3,  /**< Swing to show the other face.                 */
} CardFlipState;

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

/** @brief 0x28 per-frame transform scratch for the card-flip handler: a
 *  scratch position vector followed by the composed rotation+translation
 *  matrix handed to the GTE. Allocated/freed each frame (scratchAlloc/BA0). */
typedef struct {
    SVECTOR vec;   /* 0x00 — scratch position (morph target)        */
    MATRIX  mat;   /* 0x08 — composed YXZ rotation + translation     */
} TransformBuf;    /* 0x28 */

/**
 * @brief One face of the tetrahedral 3D icon model (@c g_triadIconFaces[4]).
 *
 * The 4 entries pair with the 4-vertex SVECTOR table @c g_triadIconVerts to describe
 * a tetrahedral 3D icon (3 yellow faces and one white face). The 3 vertex
 * indices select corners from the transformed vertex output; the three color
 * words pre-pack the @c POLY_G3 @c r/g/b/code byte quartets for direct
 * word-store into the primitive.
 */
typedef struct {
    u8  v0;             /**< Index 0..3 into the transformed vertex table. */
    u8  v1;
    u8  v2;
    u8  pad03;
    u32 color0Word;     /**< Packed @c r0|g0|b0|code (with @c 0x30 = G3 code). */
    u32 color1Word;     /**< Packed @c r1|g1|b1|pad1. */
    u32 color2Word;     /**< Packed @c r2|g2|b2|pad2. */
} TriadFaceDesc;        /* 0x10 bytes */

/* Private data — card-flip transform scratch */
extern SVECTOR       g_cardFlipUpVec;        /* +Z unit scratch vector (morph source)   */
extern SVECTOR       g_cardFlipTarget;        /* scratch target vector                   */
extern SVECTOR       g_cardFlipAngles;        /* per-frame YXZ rotation angles           */
extern s32           g_cardFlipSpin;        /* spin direction delta (+/-0x400)         */
extern TransformBuf *g_cardFlipXform;        /* current frame's transform scratch       */

/* Private data — tetrahedral 3D icon model */
extern SVECTOR       g_triadIconVerts[4];     /**< 4-vertex tetrahedron model. */
extern TriadFaceDesc g_triadIconFaces[4];     /**< 4 G3 face descriptors. */
extern u32          *g_triadIconScratch;        /**< Scratch buffer pointer for RotTransPers4 outputs. */

/* Private data — deferred VRAM transfer pool (flushVramTransfers) */
extern PoolEntry     g_vramQueue[];
extern ResHeader     g_textBufferRes;               /**< Resource header registered by the draw-target setup. */
extern ResHeader     g_tripleTriadCardFrames;  /**< Card frame/border graphics (4bpp TIM, uploaded to VRAM at init). */
extern ResHeader     g_tripleTriadCardArt;     /**< Card face artwork (8bpp TIM, ~110 cards at 64x64, uploaded to VRAM at init). */

/* Private data — draw buffers / VRAM scratch */
extern RECT          g_fbClearRect;
extern RECT          g_texClearRect;
extern u32           g_orderingTables[2][TT_OT_LEN];  /**< Per-buffer ordering tables (OT). */
extern u8            g_primPools[2][0x10000];  /* primitive pool, 64KB per buffer */
extern u8            g_textOTs[2][8];
extern u8            g_textFrameBufs[2][0x8000];
extern u8           *g_textFbPtr;
extern u8           *g_textOtPtr;
extern u8           *g_tripleTriadActiveList;
extern u8            g_hexDigits[];

/* Private data — debug text / misc state */
extern s32           g_vramQueueCount;
extern s32           g_scratchPtr;
extern s16           g_textBufferIndex;
extern s16           g_textLineX;
extern s16           g_textCursorY;
extern s16           g_textCursorX;
extern u8            g_vsyncMode;        /**< VSync() wait mode (0 = wait one vblank). */
extern RGB           g_textColor;        /**< Debug-text rgb color. */
extern u32           g_textPalette[9];      /**< '#0'..'#8' escape colors, indexed digit - '0'. */

/* Private prototypes (be_object1.c entry points forward-declared for earlier callers) */
extern void resetVramQueue(void);
extern void flushVramTransfers(void);
extern u8  *queueTimUpload(ResHeader *res);
extern void queueStoreImage(RECT *rect, void *dst);
extern void queueMoveImage(RECT *rect, s16 dstX, u16 dstY);
extern void *findFreeNode(ObjList *list);

/* SDK / main-binary math helpers used by be_object1.c. */
extern void  func_8003F884(SVECTOR *a, SVECTOR *b, s32 wa, s32 wb, SVECTOR *out);
extern void  func_80041794(s32 angle, MATRIX *m);

#endif /* TRIPLETRIAD_BE_OBJECT1_H */
