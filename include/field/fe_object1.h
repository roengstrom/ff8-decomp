#ifndef FE_OBJECT1_H
#define FE_OBJECT1_H

#include "common.h"
#include "cd.h"
#include "field.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"

/*
 * Field asset-bundle streaming table and section pointers, all consumed by
 * func_800983F0 when a map is (re)loaded.
 *
 * The streaming table is one 24-byte (six word) entry per map, indexed through
 * D_800C2568. The three symbols below are three views into that same table:
 * D_800C0900 is the primary {sector,size} pair, D_800C0908 the secondary pair,
 * and D_800C0910 the third pair, which is handed to func_800AA8A0 by address.
 */
extern u32 D_800C0900[];         /**< Streaming table, primary {sector,size} pair. */
extern u32 D_800C0908[];         /**< Streaming table, secondary {sector,size} pair. */
extern CdFileDesc D_800C0910[];  /**< Streaming table, third descriptor (passed by address). */


/** @brief Section-pointer table bases published by the loaded field bundle. */
extern u8 **D_800C7208;          /**< Event-queue block; assigned to @c D_8005F0F8. */
extern u8 **D_800C71EC;          /**< Walkmesh/section block; assigned to @c D_800D5EA4. */
extern s32 *D_800D5EAC;          /**< Word copied into @c g_fieldEntity.unk018. */
extern u8 **D_800D5E8C;          /**< End of the script region (used to size the copy). */
extern u8 **D_800D5ED4;          /**< Start of the script region; advanced past the header. */
extern u8 **D_800D5E94;          /**< Field-file header pointer. */
/** @brief Field-file header; @c NULL when the header carries the empty @c 0x2020 tag. */
extern FieldSubsceneBuffer *D_800C7200;

/** @brief Double-buffered prim-chain heads laid out after the field bundle. */
extern u8 *D_800C6D98[2];
extern u8 *D_800D5EC8[2];
extern u8 *D_800D5EB8[2];

/** @brief Field-VM command tables selected by @c EventQueue::unk0D. */
extern u8 D_800C30DC[];
extern u8 D_800C311C[];
extern u8 D_800C315C[];
/** @brief Shared scratch block handed to @c func_800AA8A0. */
extern u8 D_800C06A0[];

/** @brief 12-byte signed integer 3D position (x, y, z). */
typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3i;

/**
 * 3D line-segment coordinate view: six consecutive s16 coordinates
 * (start XYZ, end XYZ). This is the layout shared by the head of
 * @ref FieldLineTrigger and the per-entity trigger segment embedded in
 * @ref Actor at 0x188 — @c func_8009A2BC accepts either through this view.
 */
typedef struct {
    /* 0x00 */ s16 x0, y0, z0;
    /* 0x06 */ s16 x1, y1, z1;
} LineSeg;

/** @brief 4-byte signed 16-bit 2D position (x, y). */
typedef struct {
    s16 x;
    s16 y;
} Vec2s;

/** @brief 6-byte signed 16-bit 3D position (x, y, z). */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
} Vec3s;

/** @brief 8-byte navmesh vertex; the packed (sx, sy) word doubles as a GTE SXY operand. */
typedef struct {
    s16 sx;
    s16 sy;
    s16 sz;
    s16 pad;
} SVert;

/** @brief 24-byte navmesh triangle — three @ref SVert corners. */
typedef struct {
    SVert v[3];
} Triangle;

/**
 * @brief Landing slot for @c gte_stlvnl — MAC1..3 land as words but only the
 *        low halfword of each is read back (the rotated corner offsets are
 *        screen deltas and fit in 16 bits).
 */
typedef struct {
    /* 0x00 */ u16 x, xHi;
    /* 0x04 */ u16 y, yHi;
    /* 0x08 */ u16 z, zHi;
} MacVec;

/**
 * @brief Field view block (at @c D_800C71F8): the camera matrix @c SetRotMatrix /
 *        @c SetTransMatrix are loaded from, followed by projection parameters.
 */
typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ u16 viewOfsX;     /**< View's screen-space X offset: added to the draw offset by
                                      @c func_800A15C0 and subtracted from projected positions
                                      elsewhere. @note Purpose inferred from those two uses. */
    /* 0x22 */ u16 viewOfsY;     /**< View's screen-space Y offset; twin of @c viewOfsX. */
    /* 0x24 */ s16 spriteScale;  /**< Numerator of the per-OTZ sprite scale in @c func_800A39D8. */
} FieldView;

extern FieldView *D_800C71F8;

/** @brief Counted inline triangle list scanned by @ref func_8009AC9C. */
typedef struct {
    /* 0x0 */ s32 count;
    /* 0x4 */ Triangle tris[1];        /* variable length */
} TriangleList;

/** @brief 6-byte per-triangle adjacency record — neighbor triangle index per edge (0xFFFF = none). */
typedef struct {
    u16 neighbor[3];
} AdjRec;

/**
 * @brief Field navmesh vertices (loaded per field), three consecutive @ref SVert
 *        per triangle — triangle @c t owns @c D_800C71F0[t*3 .. t*3+2].
 */
extern SVert *D_800C71F0;
/** @brief Overlay header bytes at the field overlay's load address; 8 are copied to the stack on entry. */
extern u8 D_80098000[];
/** @brief The field overlay's own DrawSync callback, installed by @c func_8009895C. */
extern u8 D_800982F0[];
/** @brief Draw-environment packet arena; @c SetDrawEnv builds into it for both buffers. */
extern u8 D_800CC118[];
/** @brief Per-frame prim arena the field renderers build into. */
extern u8 D_800CD1B0[];
/** @brief Field-data section pointer set from @c 0x800E1004 on load. */
extern FieldView **D_800C71E8;
/** @brief Horizontal centre of the field clamp rect, derived on every load. */
extern s32 D_800C7210;
/** @brief Vertical centre of the field clamp rect, derived on every load. */
extern s32 D_800C7214;
/**
 * @brief Field-exit state code handed back to the engine dispatcher: 2 when the engine
 *        leaves on state 7, and 6 / 8 / 9 / 10 or the engine mode from @c func_80099348 .
 *
 * @c volatile so the stores are not sunk into branch delay slots, which is what
 * @c func_80099348 's original does.
 */
extern volatile s16 g_vsyncRate;
/** @brief Camera/view block used instead of the field's own while @c func_800BE274 reports
 *         the overlay subsystem active; assigned to @c D_800C71F8 . */
extern FieldView *D_8005F108;
/** @brief Previous frame's display environment, snapshotted from @c D_8005F138 each frame. */
extern s32 D_8005F110;

/** @brief The frame buffer currently being built; one of the two in @ref D_800C7218 . */
extern FieldFrameBuf *D_800C71E0;
/** @brief The two per-frame GPU work areas @ref D_800C71E0 alternates between. */
extern FieldFrameBuf D_800C7218[2];
/** @brief Handle returned by @c VSync each frame. */
extern s32 D_800D5EA0;
/** @brief Field render/present request byte; 1 = normal, 9 = post-copy. */
extern volatile u8 g_fadeMode;

/** @brief Field-data section pointer to the navmesh triangle list (@c *D_800C7204 + 4 is @c D_800C71F0). */
extern TriangleList **D_800C7204;
extern AdjRec *D_800D5E98;    /**< Per-triangle edge adjacency table, one entry per triangle. */


/** @brief 32-byte slot stride for indexing into a particle system buffer. */
typedef struct {
    u8 b[32];
} ParticleBlock;


/**
 * @brief Particle system buffer.
 *
 * Modeled as a flat array of 32-byte slots: the first ~313 slots hold the
 * emitter table and other buffer metadata; particle records overlay the
 * remaining slots starting at slot index 313 (byte offset 0x2720).
 * Casting a slot's address to @c Particle* gives access to that slot's
 * particle data via the absolute-offset view above.
 */
typedef struct {
    ParticleBlock slots[1];
} ParticleSystem;

/**
 * @brief Particle emitter record (one of an array within ParticleSystem).
 *
 * Stride 0x174 bytes. Indexed by emitter id from the start of @c sys.
 * Holds the emitter's spawn-rate counters and the velocity/position
 * jitter ranges used to seed each particle.
 */
typedef struct {
    /* 0x000 */ u8 pad000[0x14E];
    /* 0x14E */ u8 unk14E;          /**< Reset to 0 on each call. */
    /* 0x14F */ u8 pad14F[0x0B];
    /* 0x15A */ s16 maxCount;       /**< Cap on simultaneously-active particles. */
    /* 0x15C */ s16 curCount;       /**< Currently active particle count. */
    /* 0x15E */ s16 unk15E;         /**< Cleared together with @c curCount by @c func_800A3018. */
    /* 0x160 */ s16 unk160;         /**< Velocity-Z bias (added * 32). */
    /* 0x162 */ s16 unk162;         /**< Velocity-Z jitter half-range. */
    /* 0x164 */ s16 unk164;         /**< unk16 jitter half-range. */
    /* 0x166 */ s16 unk166;         /**< Position-X jitter (* 256). */
    /* 0x168 */ s16 unk168;         /**< Position-Y jitter (* 256). */
    /* 0x16A */ u16 unk16A;         /**< Position-Z jitter (low 7 bits). */
    /* 0x16C */ s16 unk16C;         /**< Velocity-X jitter half-range. */
    /* 0x16E */ s16 unk16E;         /**< Velocity-Y jitter half-range. */
    /* 0x170 */ s16 unk170;         /**< Velocity-Z jitter half-range. */
    /* 0x172 */ u8 pad172[0x02];
} Emitter; /* 0x174 = 372 bytes */

/**
 * @brief Particle "view" — overlay struct positioned at @c &sys->slots[slot].
 *
 * The view's fields are at the absolute byte offsets (0x2720..0x273B) where
 * each particle's data actually lives. Indexing @c sys->slots[slot] gives a
 * 32-byte slot stride; casting that address to @c Particle* lets field
 * accesses (e.g. @c p->posX) compile to @c sw v0,0x2720(s0) — the original
 * "keep @c sys+slot*32 in a register, full immediate offsets" pattern.
 */
typedef struct {
    /* 0x0000 */ u8 pad0000[0x2720];
    /* 0x2720 */ s32 posX;
    /* 0x2724 */ s32 posY;
    /* 0x2728 */ s32 posZ;
    /* 0x272C */ s16 velX;
    /* 0x272E */ s16 velY;
    /* 0x2730 */ s16 velZ;
    /* 0x2732 */ s16 unk12;
    /* 0x2734 */ u8 pad2734[0x02];
    /* 0x2736 */ s16 unk16;
    /* 0x2738 */ u8 emitterIdx;
    /* 0x2739 */ u8 unk19;
    /* 0x273A */ u8 unk1A;
    /* 0x273B */ u8 active;
} Particle;

/**
 * @brief 16-byte script entry consumed by @c func_800A0640 to populate
 *        an SPRT_16 primitive.
 *
 * The list is terminated by @c terminator == @c 0x7FFF.
 */
typedef struct ScriptEntry {
    /* 0x00 */ s16 terminator;  /**< @c 0x7FFF marks end of list. */
    /* 0x02 */ u8  pad02[6];
    /* 0x08 */ u16 clut;        /**< Palette (CLUT) id for the sprite. */
    /* 0x0A */ u8  u;           /**< Texture u of the 16x16 cell. */
    /* 0x0B */ u8  v;           /**< Texture v of the 16x16 cell. */
    /* 0x0C */ u8  padC;
    /* 0x0D */ u8  kind;        /**< @c 4 = opaque, else semi-translucent. */
    /* 0x0E */ u8  padE[2];
} ScriptEntry;

/** @brief Container for the entry list at @c D_800D5E90. */
typedef struct ScriptList {
    ScriptEntry *entries;
} ScriptList;

/* FieldLineTrigger (field line-trigger table entry) is defined in field.h. */

extern ScriptList *D_800D5E90;

extern void func_80098934(void);
extern void func_80099124(void);
extern void func_8009912C(void);
/**
 * @brief One breadcrumb in the party-follower trail: 12 bytes, 64 per ring.
 *
 * This is how the party follows the leader in a field. The followers do no
 * pathfinding at all — every tick @ref func_8009BD50 records the leader's state
 * into slot @c D_8005F144 of both rings and advances the cursor, and the two
 * followers replay slot @c (D_8005F144 @c - @c lag) @c & @c 0x3F a fixed number
 * of frames later. That is why they track so tightly and take exactly the same
 * line around a corner: they are literally walking the leader's own footsteps.
 * Party slot 1 reads @ref D_80070760 at lag @c D_8005F118, slot 2 reads
 * @ref D_80070A60 at lag @c D_8005F11A; the defaults are 15 and 30 slots.
 *
 * Two independent passes consume each waypoint, which is worth knowing because
 * they fail separately: @ref func_8009BB18 replays the position fields, and
 * @ref func_8009B74C replays the heading and animation fields. Stubbing the
 * first alone leaves the followers rooted to the spot while still turning and
 * cycling their walk animation (verified on hardware).
 */
typedef struct {
    /* 0x00 */ s16 x;       /**< Leader's X when this slot was recorded, in tile units
                                 (recorded @c /4096, replayed @c <<12). */
    /* 0x02 */ s16 y;       /**< Leader's Y. */
    /* 0x04 */ s16 z;       /**< Leader's Z. */
    /* 0x06 */ u16 unk6;    /**< Navmesh triangle the leader stood on; replayed into the
                                 follower's @c triIdx so it inherits the same ground. */
    /* 0x08 */ u8  unk8;    /**< Replayed into the follower's @c unk258. Purpose unknown;
                                 @ref func_8009ECA4 seeds it to 1. */
    /* 0x09 */ s8  field_09; /**< Animation rate: multiplied by the caller's multiplier and
                                 passed to @c func_8009B4A8 as the playback argument. */
    /* 0x0A */ u8  field_0A; /**< Selects which of the entity's animation ids (@c field_0x24F
                                 .. @c field_0x254) to play. @ref func_8009B74C overwrites it
                                 with 2 while the follower is still closing its lag gap —
                                 apparently the catch-up gait, though that is inferred from
                                 the lag comparison rather than observed. */
    /* 0x0B */ u8  field_0B; /**< Leader's heading when recorded; replayed into the follower's
                                 @c field_0x241. This is what makes a stalled follower still
                                 rotate as though it were walking the trail. */
} PathEntry;

extern void func_8009B74C(s16 slotIdx, u16 paramIdx, PathEntry *params, s16 multiplier);
extern void func_8009BB18(void);
extern void func_8009BD50(Actor *actor, s16 mode, s8 b9, u8 b8);
extern s16  func_8009D234(s32 a0);
extern s16  func_8009D254(s32 a0);
extern void func_8009DED8(Vec3i *out, SVert *a, SVert *b);
extern s32  func_8009E468(s16 selfIdx, Vec3i *pos);
extern s32  func_8009E604(Actor *a, Actor *b);

/* INCLUDE_ASM stubs — bodies still in assembly, signatures unknown.
 * Declared K&R-style; refine when these get decomped to C. */
extern void func_80098314(void);
/** @brief Load/refresh the active field map's asset bundle from CD. */
extern s32 *func_800983F0(void);
/** @brief Field engine main loop: loads a field, runs it, and dispatches on the exit state. */
extern void func_8009895C(void);
extern void func_80099180(void);
extern void func_80099348(void);
extern s32  func_8009A0E8(s32 *p0, s32 *p1, s32 *outDist);
extern s32  func_8009A2BC(LineSeg *seg, Vec3i *p, Vec3i *out);
extern s32  func_8009A4C0(Actor *actor, Eline *records, VECTOR *pt);
extern void func_8009A7E8(Actor *actor, Eline *pool);
extern void func_8009A8E0(Eline *eline);
extern void func_8009A920(Actor *actor, Eline *entities);
extern void func_8009AA64(EventEntry *e);
extern void func_8009AAC8(Actor *actor, EventEntry *segs, Vec3i *pt);
extern s16  func_8009AC9C(s16 px, s16 py, s16 pz, TriangleList *list);
/** @brief Place every field entity on the navmesh when a field is entered. */
extern void func_8009AEC0(void);
extern void func_8009BEC8(Actor *ents, s32 flags);
extern void func_8009CEE8(void);
extern s32  func_8009D274(Actor *actor, s16 pad);
extern s32  func_8009D500();  /* arg2 is a file-private scratchpad view in fe_object1.c */
extern s32  func_8009D598(s16 index);
extern s32  func_8009DF18(u16 *pTriIdx, Vec3i *out, s32 *dxy, s32 *aux);
extern s32 func_8009E338(Vec3i *a0, Vec3i *a1, Vec3i *a2, Vec3s *a3);  /* plane-cross intersection */
/** @brief Refill both follower path rings from the player's position on field entry. */
extern void func_8009E660(void);
extern void func_8009ECA4(void);
extern s32  func_8009F74C(Actor *a, Actor *b);
extern void func_8009F7F4(s16 idx, s8 sign, u8 b, s16 mode);
extern void func_8009B4A8(s16 idx, u8 anim, s16 mode, s8 delta);
extern void func_8009F8D0(s16 idx);
extern void func_8009F990(s16 idx, s32 flags);
extern void func_8009FE18(s32 entIdx, Actor *actor, s32 flags);
/** @brief Semi-transparency bit of a primitive code byte — the bit
 *         @c setSemiTrans toggles; func_800A0640 spells the toggle
 *         manually because the macro's arm order does not match. */
#define PRIM_CODE_SEMI_TRANS 0x02

extern SPRT_16 *func_800A0640(SPRT_16 *prim);
extern void func_800A06F0(s32 a, FieldFrameBuf *buf, u8 *b, u8 *c);
extern void func_800A0D6C(u8 *buf);
extern s32  func_800A0E54(s32 start, s32 end, s32 total, s32 progress);
extern s32  func_800A0EB8(s32 start, s32 end, s32 total, s32 angle);
extern s32  func_800A0F34(SVECTOR *v, s32 *sxy);
extern void func_800A0FB8(Vec2s *out, s16 a, s16 b);
extern int  func_800A19B8();
/** @brief Draws each active entity's blob shadow as a flat-shaded 8-triangle fan. */
/**
 * @brief Shape @c func_800A29C0 sees: array of 20-byte items with five
 *        leading bytes that get initialized per item.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x3];
    /* 0x03 */ u8 b3;     /**< Set to @c 4 each iter. */
    /* 0x04 */ u8 b4;     /**< Cleared. */
    /* 0x05 */ u8 b5;     /**< Cleared. */
    /* 0x06 */ u8 b6;     /**< Cleared. */
    /* 0x07 */ u8 b7;     /**< Set to @c 0x22. */
    /* 0x08 */ u8 pad08[0xC];
} func_800A29C0_arg0;  /* 0x14 = 20 bytes */

/**
 * @brief Shape of the prim records @c func_800A2A30 writes — @c 8 bytes
 *        with a @c tag byte and a @c cmd word (GPU command + color).
 */
typedef struct {
    /* 0x00 */ u8  pad00[0x03];
    /* 0x03 */ u8  tag;     /**< Always written as @c 1. */
    /* 0x04 */ s32 cmd;     /**< @c 0xE1000200 | (color & 0x9FF). */
} func_800A2A30_item;  /* 8 bytes */

extern void func_800A2AF8(FieldFrameBuf *buf, u8 *a, u8 *b, FieldView *view);

/** @brief Emit one field sprite for a movement accumulator and link it into the OT. */
/**
 * @brief 8-byte (x, y, z) vertex within a shimmer object's corner array; also
 *        the shape @c func_800A4934 stages its two interpolated points in at
 *        @c getScratchAddr(0) and @c getScratchAddr(2).
 */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
    /* 0x06 */ u16 pad6;
} ObjVertex;




/**
 * @brief Main binary's @c RotTransPers3: perspective-transforms three vertices
 *        at once, writing the three screen XY pairs and returning the OTZ.
 *
 * @note The field overlay links it by address, so it keeps its @c func_ name.
 */
extern s32 RotTransPers3(ObjVertex *v0, ObjVertex *v1, ObjVertex *v2, s32 *sxy0,
                         s32 *sxy1, s32 *sxy2, s32 *p, s32 *flag);

/** @brief Palette selector for the ribbon colour ramp (scaled by 16 to index it). */
extern u8 D_80070657;
/** @brief Ribbon colour ramp, 16 bytes per palette: five RGB triples, one per line strip. */
extern u8 D_800C3720[];
/** @brief @c D_800C3720 + 6 — the third strip's RGB triple. */
extern u8 D_800C3726[];
/** @brief @c D_800C3720 + 9 — the fourth and fifth strips' RGB triples. */
extern u8 D_800C3729[];
extern u32 g_orderingTablePtrs[];        /**< Per-buffer ordering-table heads. */
extern TILE g_clearTiles[];              /**< Per-buffer screen-clear TILEs. */

/** @brief Raised by @c func_800A5898 when the dialog state reaches @c 4. */
extern u8 D_80070649;

extern s32  func_800A5FA4(FieldLineTrigger *seg, s32 sel);
extern void func_800A6100(Actor *actor, FieldLineTrigger *segs, Vec3i *pt);
extern void func_800A62EC(FieldLineTrigger *segs);
extern int  func_800A63AC();
extern int  func_800A6A80();

/**
 * @brief Element of a @ref FieldObject part's sub-range (8-byte stride).
 * @note Purpose uncertain — @c func_800A8058 clears @c field06 for every
 *       element in each part's @c [subStart, @c subStart+subCount) range.
 */
typedef struct {
    /* 0x00 */ u8 pad00[6];
    /* 0x06 */ s16 field06;
} FieldObjectSub;  /* 0x08 */

/**
 * @brief One part of a @ref FieldObject (0x20-byte stride).
 *
 * Each part owns a contiguous @c [subStart, @c subStart+subCount) slice of the
 * object's @ref FieldObjectSub array.
 * @note Field naming reflects only observed usage.
 */
typedef struct {
    /* 0x00 */ s16 subStart;   /**< first sub-element index owned by this part. */
    /* 0x02 */ s16 subCount;   /**< number of sub-elements in the part. */
    /* 0x04 */ u8 pad04[0x0A];
    /* 0x0E */ s16 field0E;    /**< cleared per part by @c func_800A8058. */
    /* 0x10 */ u8 pad10[0x10];
} FieldObjectPart;  /* 0x20 */

/**
 * @brief A field-engine object instance (element of the @c D_800D6620 table).
 *
 * @note Purpose partially understood — holds a part list (@c parts /
 *       @c partCount) indexing a shared sub-element array (@c subs), plus a
 *       @c 0x12345678 signature word and a couple of init fields.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x1C];
    /* 0x1C */ FieldObjectPart *parts;  /**< part array (@c partCount entries). */
    /* 0x20 */ FieldObjectSub *subs;    /**< sub-element array indexed by parts. */
    /* 0x24 */ s32 partCount;
    /* 0x28 */ u8 pad28[0x24];
    /* 0x4C */ u32 signature;           /**< set to @c 0x12345678 on init. */
    /* 0x50 */ u16 field50;             /**< init'd from @c D_800D60E8. */
    /* 0x52 */ u8 pad52[0x0D];
    /* 0x5F */ u8 field5F;              /**< cleared on init. */
} FieldObject;

/** @brief 64-slot field-engine object table (indexed by object id). */
extern FieldObject *D_800D6620[];
/** @brief u16 seed value written into @c FieldObject::field50 at init. */
extern u16 D_800D60E8;

extern void func_800A7194(void);
extern void func_800A7224(s32 idx, u16 *vals, s32 mode);
extern void func_800A736C(s32 idx, u16 *vals, s32 mode);
extern void func_800A74B4(s32 idx, EntityRenderXform *vals, s32 mode);
extern int  func_800A7564();
extern s32  func_800A8058(s32 idx, s32 arg1, FieldObject *newObj, u8 count);
extern int  func_800A81AC();
/** @brief Scratch sprite rectangle built by @c func_800AA5F8 before a @c MoveImage upload. */
extern RECT D_800D5ED8;

extern s32 *func_800A8CDC(s32 idx, s32 firstWord, EntityRenderSlot *slot);
/** @brief Per-entity animation tick: advances the frame and rebuilds the sprite rect. */
extern s32  func_800AA5F8(s32 idx);
extern u8  *func_800A8DAC(s32 spatialIdx, s32 cmd, u32 arg, void *out);
extern int  func_800A91C8();
extern int  func_800A9434();
extern void func_800A97E4(s32 spatialIdx, s32 cmd, s32 arg2, s32 arg3);
extern void func_800AA46C(s32 spatialIdx, s32 cmd, s32 arg, s32 arg4);
extern int  func_800AA8A0();


/* Shared by fe_object1.c, fe_object1_2.c and fe_object1_3.c. */
extern u16 D_8005F118;
extern u16 D_8005F11A;
extern u16 D_8005F144;
extern s16 D_8005F148;
extern volatile u8 g_fadeMode;   /**< Encounter-disable flag (1 = no random battles); also spun on by the field loader. */
extern u16 D_8005F0FE;           /**< Accumulated battle chance; compared against the encounter RNG roll. */
extern s16 D_8005F120;           /**< Previous battle formation id (avoid immediate repeats). */
extern u8 D_8005F130;            /**< Encounter-pending marker set when a battle triggers. */
extern u16 D_8005F164;           /**< Step accumulator; a battle check runs each time it passes 0x100. */
extern u8 D_80078DF8;            /**< Field movement flags: bit 3 halts encounter steps, bit 2 halves the step rate. */
extern u8 **D_800C71F4;          /**< Field-data section pointer: per-field encounter step-rate byte. */
extern u16 **D_800C720C;         /**< Field-data section pointer: 4-entry battle formation table. */
extern u16 D_8005F160;
extern u16 D_8005F162;
extern u8 D_800C319C[];          /**< Arctangent lookup table (byte per 2*|component| step) for func_8009A0E8. */
extern u8 D_800C32A0[];
extern u8 D_800C3320[];
extern u8 D_800C3520[];
extern u8 D_800C6D90;            /**< PRNG counter advanced 13/step by func_800A2EA4 */
extern u8 D_8005F150;            /**< Outer PRNG counter, D_800C3520 lookup offset, advanced 13/step per 256 calls of func_800A5C9C */
extern u8 D_8005F151;            /**< Inner PRNG counter, D_800C3520 lookup index, advanced 1/call by func_800A5C9C */

extern s32 GetClut(s32 a, s32 b);
extern void func_8004D684(void *p);

extern u16 **D_800D5E9C;         /**< Pointer-to-pointer of u16 count for func_800A29C0's iteration */
extern u16 *D_800C71E4;
extern s32 D_800C71FC;           /**< Latched result of @c func_800A0F34 from @c func_800A11E0. */
extern u16 D_800D3E88[];
extern u8 D_800D5F50[];
extern u8 D_800D61A8[];
extern u8 D_8005F168[];
extern volatile s32 D_8005F154;  /**< VSync frame counter (main.c); phase for the @c D_800C3520 perturbation table. */

/**
 * @brief 24-byte draw-point slot holding a 16-bit (x, y, z) position plus the
 *        two derived quad corners built by @c func_800A4758.
 *
 * @c x/y/z is the base position written by @c func_800A4500. @c func_800A4758
 * derives two corner offsets from it: @c field8 / @c fieldA / @c fieldC (base
 * minus a table-perturbed 0x80 bias) and @c field10 / @c field12 / @c field14
 * (base plus fixed 0x40/0x80 offsets).
 */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
    /* 0x06 */ u16 pad06;
    /* 0x08 */ u16 field8;
    /* 0x0A */ u16 fieldA;
    /* 0x0C */ u16 fieldC;
    /* 0x0E */ u16 padE;
    /* 0x10 */ u16 field10;
    /* 0x12 */ u16 field12;
    /* 0x14 */ u16 field14;
    /* 0x16 */ u16 pad16;
} DrawPoint;  /* 0x18 = 24 bytes */
extern DrawPoint D_800706A0[];

/**
 * @brief One 136-byte object slot in the @c D_800C6DA0 table walked by
 *        @c func_800A5224 (8 slots).
 *
 * The two 8-entry vertex arrays @c va / @c vb are seeded by @c func_800A4758
 * from the object's draw-point position (all 8 entries get the same value).
 * @c field86 / @c field87 receive a table-perturbation byte; the trailing tick
 * pair (@c field80 / @c field82) is what @c func_800A5224 later consumes.
 */
typedef struct {
    /* 0x00 */ ObjVertex va[8];
    /* 0x40 */ ObjVertex vb[8];
    /* 0x80 */ s16 field80;   /**< Tick threshold base; slot clears when tick > field80+4. */
    /* 0x82 */ u16 field82;   /**< Per-frame tick counter (incremented while active). */
    /* 0x84 */ u8  pad84[0x02];
    /* 0x86 */ u8  field86;
    /* 0x87 */ u8  field87;
} ObjSlot;  /* 0x88 = 136 bytes */
extern ObjSlot D_800C6DA0[];
extern s16 D_8005F122;
extern s16 D_8005F14A;
extern s16 D_8005F100;
extern s16 D_8005F142;
extern s16 D_800C2568[];         /**< Field id -> streaming-table entry index. */
extern u32 D_800C0904[];         /**< Streaming table: 24-byte (6-word) entries; this symbol
                                      addresses entry 0's size word, with its sector word
                                      in the preceding word. */
extern u8 D_8005F103;
/** Length of the two breadcrumb path rings the party followers replay from.
 *  Indices are taken modulo this, so the mask follows the tables' size. */
#define FIELD_PATH_RING_LEN  64
#define FIELD_PATH_RING_MASK (FIELD_PATH_RING_LEN - 1)

extern PathEntry D_80070A60[FIELD_PATH_RING_LEN];
extern PathEntry D_80070760[FIELD_PATH_RING_LEN];
extern DRAWENV g_drawEnvs[2];   /**< Double-buffered draw environments. */
extern DISPENV g_dispEnvs[2];   /**< Double-buffered display environments. */

#endif
