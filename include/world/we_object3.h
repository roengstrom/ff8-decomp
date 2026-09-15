#ifndef WORLD_WE_OBJECT3_H
#define WORLD_WE_OBJECT3_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "world.h"

/** One entry of the @c D_800D24A8 descriptor cache: a command descriptor and
    the cell key it was resolved for, kept in most-recently-used order. */
typedef struct {
    CmdDesc *val;   /* +0x00 */
    s16 hval;       /* +0x04 — cell key the descriptor was resolved for */
    s16 pad;        /* +0x06 */
} FeaEntry40C0; /* size 0x08 */

/* Object glyph header returned by func_800A5EC4: a count byte followed by an
   array of @c CmdDesc entries (16-byte stride). */
typedef struct {
    u8      count;        /* 0x00 */
    u8      id;           /* 0x01 — glyph kind, dispatched on by func_800BF20C/func_800BFBFC */
    u8      pad02[2];
    CmdDesc entries[1];   /* 0x04 — [count] entries, stride 0x10 */
} GlyphHeader;

typedef struct {
    u16 x;
    u16 y;
} ImageCoord;

/* func_800A6188 loads a TIM with two fixed-size, back-to-back image blocks
 * (the canonical Tim struct from tim.h, reached via world.h -> battle.h). */

extern FeaEntry40C0  D_800D24A8[12];
extern WorldObject   D_800D3320[16];
extern WorldObject  *D_800D3318;
extern WorldObject  *D_800D34E0;
extern WorldObject  *D_800D34E4;
extern WorldObject  *D_800D2284;     /**< Head of the active world-object list. */
extern WorldObject   D_800C9888[];   /**< Pool the visible-cell list is built in. */
/** Section slots the world can hold live at once; @c D_800D34A0 marks each
    one busy, and @c WorldObject.sectionIdx indexes it. */
#define WORLD_SECTION_SLOTS 16
extern u32           D_800D34A0[WORLD_SECTION_SLOTS];
extern u32           D_800D34F0[];  /**< Streamed-record staging buffer. */
extern WorldObject   D_800D33E0[16];
extern WorldObject   D_800C9EF0[16];
extern WorldObject  *D_800CA030;
extern POLY_F4       D_800D3300;
extern ImageCoord    D_800C5388[];
extern ImageCoord    D_800C5378[];
extern RECT          D_800D32F0;

extern s32 func_800A629C(WorldObject *target);

/* Resolve the object glyph header for a cell key (NULL if none). */
extern u32 *func_800A5EC4(s16 id);

/* Point-in-descriptor hit test: returns nonzero and writes a result word to
   @p out when @p point falls inside the region of command descriptor @p cand. */
extern s32 func_800BF024(CmdDesc *cand, SVECTOR *point, AngleSlot *out, CmdDesc *end);

/* Project a world position to a grid-cell index; optionally emit its angle triple. */
extern s32 worldPosToCell(VECTOR *pos, SVECTOR *out);
/** Called from we_object7; see the definition for what the result means. */
extern s32 func_800A3EE4(VECTOR *tr, s16 ang, s16 z);

/* Program the GTE translation vector for world-map rendering from two packed coords. */
extern void setWorldMapTransVector(s16 coord0, s16 coord1);

/* Register master-list objects not yet present in any tracking list onto the active list. */
extern void registerNewWorldObjects(void);

/* Generate up to 5 spread-positioned SVECTOR offsets for the scene @p ctx. */
extern s32  func_800B5ADC(s32 ctx, SVECTOR *out, u16 *outParam, s32 bias);


/* Compute a linear tile index from 2D world coordinates (32x24 grid). */
extern s32 func_800A5E40(s32 x, s32 y);

/* Visibility gate keyed on a packed command code; sibling of func_800A45D8. */
extern s32 func_800A4670(u32 a, s32 b);

/* Linear tile index from 2D world coordinates; sibling of func_800A5E40. */
extern s32 func_800A5DC8(s32 x, s32 y);

/* Rebuild the world-map sprite pool and draw every pending world object. */
extern void renderWorldMapFrame(void);

/* World render callback: advances the streamer and the object lists once per
   frame. Registered with func_800C3DB0; returns 2 busy / 1 armed / 0 idle. */
extern s32 func_800A47A4(void);

/* Probe up to 8 rotations for a free 5-sprite fan placement around the camera;
   returns 1 and writes sprite 0's position to hitPos when one is accepted. */
extern s32 func_800A2D50(s32 code, s32 angZ, SVECTOR *angles, VECTOR *hitPos, s32 arg4, s32 *hitCell);

/* Place a five-sprite fan around a slot and emit it; returns func_800B21EC's
   result (negative when the fan was rejected). */
extern s32 func_800A358C(s32 kind, SlotEntry *slot, SVECTOR *angles, s32 flag);

/* Visibility gate: picks a CMDPAR_VIS_* bit of packed code @p a by dispatch
   code @p b (only b's low 16 bits are examined; D_800C4D20 == 0 force-passes). */
extern s32 func_800A45D8(u32 a, s32 b);

/* Per-frame world setup: advance the frame clock, run the renderers and flip
   the scene context. */
extern void func_800A01DC(s32 skipPresent);

/* Lay out the primitive pools over work RAM and pre-tag every packet. */
extern void func_800A246C(void);

/* Initialise the world-engine subsystem's object pools and free lists. */
extern void func_800A581C(void);

/* Free the WorldObject list at D_800D34E4 back to the free pool. */
extern void func_800A6358(void);

/* Gated table swap: copy one of two source halfword tables by map id. */
extern void func_800A63F0(void);

/* Initialize the world's two double-buffered graphics contexts. */
extern void initWorldDoubleBuffer(void);

/* Program the GTE for world-map rendering: screen offset, back color,
   color matrix. */
extern void setupWorldRenderParams(void);

/* The primitive pools. Public because the world entry loop (we_object0)
   is what points D_800C9720 at a bank each frame; the glyph renderers
   then draw out of whichever one it selected. */
typedef struct {
    POLY_GT3 *cur;       /* 0x00 */
    POLY_GT3 *base;      /* 0x04 */
    POLY_GT3 *end;       /* 0x08 */
} Gt3Pool;               /* 0x0C */

typedef struct {
    POLY_FT3 *cur;       /* 0x00 */
    POLY_FT3 *base;      /* 0x04 */
    POLY_FT3 *end;       /* 0x08 */
} Ft3Pool;               /* 0x0C */

typedef struct {
    POLY_FT4 *cur;       /* 0x00 */
    POLY_FT4 *base;      /* 0x04 */
    POLY_FT4 *end;       /* 0x08 */
} Ft4Pool;               /* 0x0C */

/** The three pools of one bank, one per primitive kind. Both banks are laid out
    at start-up by @c func_800A246C, and the world entry loop (@c func_800987D8
    in we_object0) re-points @c D_800C9720 at one of them every frame: bank 1
    while the active scene context is the no-battle sentinel @c D_800CA040,
    bank 0 otherwise. The glyph renderers then draw out of whichever it picked.
    (Not @c D_800C53A4 — that one indexes the sprite record banks
    @c D_800D2508.) */
typedef struct {
    Gt3Pool gt3;         /* 0x00 */
    Ft3Pool ft3;         /* 0x0C */
    Ft4Pool ft4;         /* 0x18 */
} WorldPrimBank;         /* 0x24 */

extern WorldPrimBank    D_800C9E88[2]; /**< The two banks. @c func_800A246C lays
                                            them out; the world entry loop indexes
                                            this array directly to choose one, and
                                            everything else reaches the chosen bank
                                            through @c D_800C9720. */
extern WorldPrimBank   *D_800C9720;   /**< The bank the glyph renderers allocate
                                           from this frame. The world entry loop
                                           (@c func_800987D8) re-points it every
                                           frame; we_object6 and we_object7
                                           read it. */

extern s32              D_800C972C;   /**< Accumulated glyph-entry count. */
extern s32              D_800C9ED0;   /**< Pad 0's raw frame parameter. */
extern SVECTOR          D_800CA038;   /**< Reference offset fed to the pool placer. */
extern s32              D_800D2238;

extern void func_800A5F78(s32 screen);

extern void func_800A5FD4(s32 screen);

/**
 * @brief One placed world-map sprite produced by @c placeWorldSpriteFan (0x2C stride).
 *
 * @c pos is the final world position; @c cell receives the @c worldPosToCell
 * projection; @c cellId/flag are the projected grid-cell id and a fixed marker.
 * @note Field purpose partly uncertain — named from the access pattern.
 */
typedef struct {
    VECTOR   pos;        /* 0x00 */
    SVECTOR  cell;       /* 0x10 — worldPosToCell output */
    u8       pad18[0x4]; /* 0x18 */
    CmdDesc *cmd;        /* 0x1C — installed as the current descriptor D_800C4D64 */
    s16      cellId;     /* 0x20 — worldPosToCell return */
    s16      flag;       /* 0x22 — one of WORLD_SPRITE_* below */
    s16      angle;      /* 0x24 — heading used to bias the camera track */
    s16      code;       /* 0x26 — packed type | flag<<8 of the installed descriptor */
    u16      unk28;      /* 0x28 — published to D_800C4D48; its readers are still asm */
    u8       pad2A[0x2]; /* 0x2A */
} WorldSprite;           /* 0x2C */

/** @c WorldSprite::flag states. */
#define WORLD_SPRITE_FREE    0  /**< Slot unused. */
#define WORLD_SPRITE_PENDING 1  /**< Re-probed this pass but matched no descriptor. */
#define WORLD_SPRITE_PLACED  2  /**< Given a position this pass. */
#define WORLD_SPRITE_CLAIMED 3  /**< Matched a glyph this frame. */

/** Sprites in one pool record: an anchor plus the four spread around it. */
#define WORLD_FAN_SPRITES 5

/** A pool record: five sprites placed together as one fan. */
typedef struct {
    WorldSprite sprite[WORLD_FAN_SPRITES];
} WorldSpriteRec;        /* 0xDC */

#endif /* WORLD_WE_OBJECT3_H */
