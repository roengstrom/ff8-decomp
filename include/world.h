#ifndef WORLD_H
#define WORLD_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "battle.h"
#include "gamestate.h"
#include "sound.h"

/** View of the sentinel ctx exposing the DISPENV template that sits past the
 *  tracked @c BattleSceneCtx body (at @c +0x40CC). */
typedef struct { u8 pad[0x40CC]; DISPENV disp; } CtxDispView;

/** @brief World-map tile size in world units (one tile = 2048 units). */
#define WORLD_TILE_SIZE 2048

/**
 * @brief World-space 3-vector used for translation/position state.
 *
 * Stores a world-space integer position. Some callers use the y/z axes
 * with a sign flip to account for the PS1 y-down screen convention
 * (see @c we_object9 for the source-side mapping).
 */
typedef struct {
    s32 x;             /* 0x00 */
    s32 y;             /* 0x04 */
    s32 z;             /* 0x08 */
} WorldPos;

/** 4-byte slot read as either a full signed word (angle compares and deltas
    are signed) or just the low halfword. */
typedef union {
    s32 word;
    u16 half;
} AngleSlot;

/**
 * @brief World-engine object node — singly-linked, keyed by s16 id.
 *
 * Callers use the @c sectionIdx at +0x0A to index into the world data
 * region base @c D_800C4D5C (stride 0x9000 bytes).
 */
typedef struct WorldObject {
    struct WorldObject *next;   /**< 0x00: Next node. */
    s16 id;                     /**< 0x04: s16 id used by search. */
    s16 key;                    /**< 0x06: Search key passed to worldObjectById. */
    /** 0x08: read both ways — as the byte index into the section's offsets[]
        table, and as a halfword row key by the world render callback. */
    union {
        struct {
            u8 idx;             /**< 0x08 */
            u8 pad09;           /**< 0x09 */
        } b;
        s16 row;                /**< 0x08 */
    } slot;
    u8 sectionIdx;              /**< 0x0A: Section index into the D_800C4D5C region table. */
} WorldObject;

/**
 * @brief One 0x9000-byte section of the world data region table.
 *
 * Pointed-to by @c D_800C4D5C as an array indexed by @c sectionIdx.
 * The section begins with a 1-byte @c key (used by lookup helpers to
 * match sections), followed by a u32 offset/handle array starting at
 * offset 4. The rest of the section contains the data that the offsets
 * index into.
 *
 * Known fields:
 *   - @c key at offset 0x00 — section type/key byte (0xFF when empty).
 *   - @c offsets[] at offset 0x04 — u32 array; low 2 bits may carry flags.
 */
typedef struct {
    /* 0x00 */ u8 key;               /**< Section type/key byte. */
    /* 0x01 */ u8 pad01[3];
    /* 0x04 */ u32 offsets[9215];    /**< Fills section up to 0x9000 total. */
} WorldSection;

/**
 * @brief World-engine command descriptor (16 bytes).
 *
 * Used by dispatch functions that peek the current command's
 * @c type / @c flag / @c param. The current active descriptor is pointed
 * to by D_800C4D64, D_800C4D68, D_800C4D6C (three slots).
 */
typedef struct {
    u8 unk_00[0xD];
    u8 type;        /**< 0x0D: Command type byte. */
    u8 flag;        /**< 0x0E: Command flag byte. */
    u8 param;       /**< 0x0F: Command parameter byte. */
} CmdDesc;

/* Per-code visibility bits of CmdDesc.param, tested by the world placement
 * gates (func_800A45D8 / func_800A4670 / func_800A2D50) after packing the
 * descriptor as type | flag << 8 | param << 16. Meanings unknown beyond the
 * code range each bit answers for -- placeholder names. */
#define CMDPAR_VIS_00_09   0x80  /**< codes 0x00..0x09 and 0x80 */
#define CMDPAR_VIS_20_28   0x40  /**< codes 0x20..0x28 and 0x84 (kind gate) */
#define CMDPAR_VIS_30      0x20  /**< code 0x30 (kind gate) */
#define CMDPAR_VIS_31      0x10  /**< code 0x31 (kind gate) */
#define CMDPAR_MODE_20_28  0x04  /**< codes 0x20..0x28 and 0x84 (mode gate) */
#define CMDPAR_MODE_30     0x02  /**< code 0x30 (mode gate) */
#define CMDPAR_MODE_31     0x01  /**< code 0x31 (mode gate) */
/** Mode-gate bit for code 0x32, in CmdDesc.flag. */
#define CMDFLAG_MODE_32    0x80

/**
 * @brief Slot entry in the D_800DBFB8 table (stride 40 bytes).
 *
 * Each entry describes one battle/world slot. Fields are added as
 * usages are discovered.
 */
typedef struct {
    /* 0x00 */ VECTOR position;     /**< Slot world position (vx/vy/vz; vy compared to world camera angle). */
    /* 0x10 */ s8 marker;           /**< Scan terminator / type byte. */
    /* 0x11 */ u8 pad11;
    /* 0x12 */ s8 lookupIdx;        /**< Index into D_800DDB00 when >= 0. */
    /* 0x13 */ u8 pad13;
    /* 0x14 */ SVECTOR vec;         /**< First 8 bytes of the tail — vy/vz used by func_800BDxxx perturb-and-emit helpers. */
    /* 0x1C */ u8 tail[0x0C];       /**< Remaining 12 bytes of the tail. */
} SlotEntry; /* 0x28 = 40 bytes */

/**
 * @brief Four-word transform block at LookupTarget+0x20 (world twin of
 *        field @c EntityRenderXform).
 */
typedef struct {
    s32 field20;
    s32 field24;
    s32 field28;
    s32 field2C;
} WorldRenderXform;

/**
 * @brief Lookup / render-slot target reached via D_800DDB00[idx].
 *
 * World twin of field @c EntityRenderSlot. Layout matches the fields
 * touched by @c func_800C1718 (init) and later world render helpers;
 * @c field52 remains the motion halfword used elsewhere.
 */
typedef struct {
    /* 0x00 */ s32 firstWord;
    /* 0x04 */ u8 *dataPtr;       /**< Base for mode-0x1C/1D/1E indexed buffers. */
    /* 0x08 */ s32 unk08;
    /* 0x0C */ u16 unk0C;
    /* 0x0E */ u16 unk0E;
    /* 0x10 */ u16 unk10;
    /* 0x12 */ u16 unk12;
    /* 0x14 */ u16 unk14;
    /* 0x16 */ u8 pad16[0x02];
    /* 0x18 */ u16 unk18;
    /* 0x1A */ u16 unk1A;
    /* 0x1C */ u16 unk1C;
    /* 0x1E */ u8 pad1E[0x02];
    /* 0x20 */ WorldRenderXform xform;
    /* 0x30 */ u8 pad30[0x20];
    /* 0x50 */ u16 unk50;
    /* 0x52 */ u16 field52;
    /* 0x54 */ u8 pad54[0x0C];
    /* 0x60 */ u8 unk60;
    /* 0x61 */ u8 pad61[0x07];
    /* 0x68 */ u8 *unk68;          /**< Halfword-pair table (mode 0xD setup). */
    /* 0x6C */ u8 *unk6C;
    /* 0x70 */ u8 unk70;
    /* 0x71 */ u8 unk71;
    /* 0x72 */ u8 pad72[0x26];
    /* 0x98 */ s32 subBuffer;
} LookupTarget;

/**
 * @brief 12-byte keyed record used by D_800C9880's packed lookup buffer.
 *
 * @c val0 / @c val1 are the descriptor pair emitted by collectActiveKeyEntries for an
 * active entry; @c key at +0x02 indexes the script offset table @c D_800C9728.
 * The trailing 9 bytes remain unmapped.
 */
typedef struct {
    u8 val0;        /* 0x00 */
    u8 val1;        /* 0x01 */
    u8 key;         /* 0x02 */
    u8 pad03[9];    /* 0x03 */
} Entry12;

/**
 * @brief Packed length-prefixed buffer of Entry12 records.
 *
 * @c length is the total buffer length in bytes including the 4-byte
 * header. Records start at @c entries[0] and run until @c length bytes.
 */
typedef struct {
    u32 length;
    Entry12 entries[1];
} KeyBuffer;

/* world globals (we_object7) */
extern s32 D_800C4D2C;
extern s32 D_800C4D40;
extern s32 D_800C4D44;
extern s32 D_800C4D58;
extern s32 D_800C4D70;
extern s32 D_800C4D98;
extern s32 D_800C4DA0;        /* = Entry12.val0 >> 1 (set by collectActiveKeyEntries) */
extern s32 D_800C4DA4;        /* = Entry12.val1 >> 1 (set by collectActiveKeyEntries) */
extern s32 D_800C4D9C;
extern s32 D_800C4DC0;
extern s32 D_800C4DC4;
extern s32 D_800C4DC8;
extern s32 D_800C5924;
extern s32 D_800C5B50;
extern s32 D_800C5B54;
extern s32 D_800C5B58;
extern s32 D_800C5BFC;
extern s32 D_800C5C04;
extern s32 D_800C5C18;
extern s32 D_800C5C1C;
extern s32 D_800C5C20;
extern s32 D_800C5C24;
extern s32 D_800C5C28;
extern s32 D_800C5C2C;
extern s32 D_800C5C30;
extern s32 D_800C5C38;
extern s32 D_800C5C3C;
extern s32 D_800C5C40;
extern s32 D_800C5D54;
/**
 * @brief Pointer to the dialogue/action table buffer.
 *
 * The buffer begins with a u32 byte-offset table indexed by subject id;
 * each offset points (within the same buffer) to a 0-terminated list of
 * u32 byte-offsets to the actual entry payloads.
 */
extern u8 *D_800C976C;
extern u8  D_800C5984[];
extern u16 D_800C5C44[];
extern s32 D_800C97A4;
extern s32 D_800C9878;
extern s16 D_800C987C;
extern u8  D_800D23D8[];
extern s32 D_800DCB48;

/**
 * @brief 8-byte world-transform block — four packed halfwords.
 *
 * Every field is read/written as @c lh / @c sh at a fixed offset. Only
 */


/**
 * @brief 16-byte world-transform record at @c D_800D2390 (pair of blocks).
 *
 * @c head occupies the first 8 bytes and @c tail the last 8. @c func_800ACD38
 * snapshots @c tail, adjusts its @c angle, and forwards the modified copy.
 */
typedef struct {
    /* 0x00 */ SVECTOR head;
    /* 0x08 */ SVECTOR tail;
} WorldXform; /* 0x10 */

/* Shared world-overlay state referenced by more than one we_object*.c.
 * Consolidated here so each symbol has a single canonical declaration.
 * (Cross-overlay main-binary symbols D_800780D8 and g_fieldVars are owned
 * by field.h / gamestate.h respectively and are not redeclared here.) */
extern s32            D_8005F138;        /**< Active display-env window (holds a DISPENV*). */
extern s32            D_800C4D20;
extern s32            D_800C4D30;        /**< World-map zoom/scale constant (set by setupWorldMapView).
                                               Stored as a word, but @ref func_800A8270 uses only its low
                                               half — hence the @c (u16) cast at that one site. */
extern s32            D_800C4D38;        /**< World dispatch code / map id. */
extern s32            D_800C4D3C;
extern s32            D_800C4D4C;
extern WorldSection  *D_800C4D5C;        /**< World data region table base (stride 0x9000). */
extern u16            D_800C4D60;
extern CmdDesc       *D_800C4D64;        /**< Active command descriptor (slot 0). */
extern CmdDesc       *D_800C4D74;        /**< Active command descriptor (slot 1). */
extern s32            D_800C4D84;
extern s32            D_800C4D88;
extern s32            D_800C4DA8;
extern s32            D_800C4DAC;
extern s32            D_800C4DB0;
extern s32            D_800C4DB4;
extern s32            D_800C4DBC;
extern SeqEntry       D_800C4FD8[];      /**< Sequence/SFX clip table. */
extern SfxSlot        D_800C526C[];      /**< Active SFX voice slots. */
extern s16            D_800C53C4[];      /**< Per-map halfword param table. */
extern s16            D_800C53D0[];
extern s16            D_800C53DC[];
extern s16            D_800C53E4[];
extern s16            D_800C53EC[];
extern u8            *D_800C96D0;
extern s32            D_800C9710;
extern s16            D_800C977A;
extern s32            D_800C97F4;        /**< World camera angle. Stored as a word; the
                                              angular-delta helpers in we_object9 read only its
                                              low half — hence the (u16) cast at those sites. */
extern MATRIX         D_800C9838;        /**< World-to-screen matrix loaded into the GTE. */
extern VECTOR         D_800C9868;        /**< Source camera world position. 16 bytes: the world entry loop copies
                                              it whole into @c D_800C9858 and hands it straight to func_800BC544. */
extern SVECTOR        D_800C9770[2];     /**< Camera scratch: [0] is a position offset, [1] a rotation. */
extern s32           *D_800C9744;        /**< Texture-strip animation block: a NULL-terminated s32 offset
                                              table; each offset, relative to this pointer, locates one
                                              strip record (see func_800A7B38). */
extern SVECTOR        D_800C9E38;     /**< Per-frame camera movement deltas ([0]=x, [2]=z); cleared on world init (we_object1), fed to the particle camera-follow drift (we_object4). */
extern s16            D_800D239A;
extern CVECTOR        D_800DB0D0[3];    /**< Map-view HUD palette (copied from D_800C5448 by
                                               func_800ABC98): three RGB stops, dark to light.
                                               func_800A8A28 spans them as the backdrop gradient
                                               and func_800A8868 tints its panel with the last.
                                               @note func_800A8400 also sums stop 0's channels to
                                               centre the location-name banner; that use is not
                                               understood. */
/** Per-slot message/section key block. A slot holds @c WORLD_SLOT_NONE when
    it is unclaimed; @c func_800A610C also returns that when no section
    qualifies. */
extern u8             D_800C5398[];
#define WORLD_SLOT_NONE   0xFF   /**< Unclaimed slot / no section found. */
#define WORLD_SLOT_COUNT  4      /**< Slots in @c D_800C5398. */
extern s32            D_800C9718;
extern s16            D_800C97E8;        /**< Worldmap screen height reference. */
extern s16            D_800C97EA;        /**< Worldmap screen width reference. */
extern SVECTOR        D_800C97F8[2];     /**< Composed rotation pair fed to func_800ACC68. */
extern WorldXform     D_800D2390;        /**< World-transform record (16 bytes). */
extern WorldPos       D_800D23C0;        /**< Composed camera/world position output. */
extern u8             D_800D2440;        /**< Scatter key (5 bits). */
extern SlotEntry      D_800DBFB8[];       /**< Battle/world slot table (stride 0x28). */

/**
 * @brief Struct view over @c D_800D23D8 (world-state flag buffer).
 *
 * Documents the per-byte layout observed in callers: an opcode-param
 * byte at offset 0, a per-actor mode byte array starting at offset 2,
 * a region-enable halfword at 0x46 and a bit-flag byte at offset 0x66.
 * Use via @c ((WorldFlags *)D_800D23D8)->field — the global itself stays
 * declared as @c u8[] so older callers that still index it as a byte
 * array keep working.
 *
 * @note @c actorMode was previously written as running all the way to
 *       0x65; @ref func_800A568C reads a signed halfword at 0x46, so the
 *       array is capped there. Where it really ends is still unknown —
 *       nothing indexes it yet.
 */
typedef struct {
    /* 0x00 */ u8  opParam;           /**< Set from script opcode @c param. */
    /* 0x01 */ u8  pad01;
    /* 0x02 */ u8  actorMode[0x46 - 0x02]; /**< Per-actor mode byte. */
    /* 0x46 */ s16 regionMask;        /**< Bit @c i enables world region @c i. */
    /* 0x48 */ u8  pad48[0x66 - 0x48];
    /* 0x66 */ u8  flags;             /**< Bit flags (bits 0x20, 0x40 used). */
    /* 0x67 */ u8  pad67;
} WorldFlags;

/**
 * @brief @c CVECTOR::cd flag on the world depth-cue path: set means
 *        "drop this pixel". func_800A688C marks fully black source
 *        pixels with it; func_800ABDD8 skips those and also sets it on
 *        the way out to veto a blended pixel.
 */
#define WORLD_CD_DROP 0x80

/**
 * @brief Streaming-image record as it appears in world data: a fixed
 *        0x14-byte header followed by the pixel payload.
 *
 * This is the wire format @ref func_8009CA34 parses into an @ref ImageDesc;
 * callers that already know a record is single-stage (such as the texture
 * strip loader) skip the parse and blit @c data directly.
 */
typedef struct {
    /* 0x00 */ u32 count;   /**< Leading count/magic word. */
    /* 0x04 */ u32 flag;    /**< Low nibble gates the second stage. */
    /* 0x08 */ u32 size;    /**< Payload size in bytes. */
    /* 0x0C */ RECT rect;   /**< Destination rectangle. */
    /* 0x14 */ u32 data[1]; /**< Pixel payload, as passed to @c LoadImage. */
} StreamImage;

/**
 * @brief Two-stage @c LoadImage descriptor — pair of @c RECT / @c data
 *        slots gated by the low nibble of @c flag. Shared between
 *        @c func_8009C478 and @c func_8009C5FC.
 */
typedef struct {
    RECT     rect1;
    u32 *    data1;
    RECT     rect2;
    u32 *    data2;
    u8       flag;
} ImageDesc;

/* Parses a streaming-image record at @p src into @p desc. */
extern s32  func_8009CA34(s32 *src, ImageDesc *desc);
/* Blits @p data through scratch rect @p r (typically &D_800C8640). */
extern RECT D_800C8640;            /**< Scratch RECT used by the LoadImage blits. */
/* MoveImage-style VRAM copy: source rect @p r to destination (@p dstX, @p dstY). */

/**
 * @brief Two-halfword script opcode entry (4 bytes).
 *
 * The world-engine script consists of a u32 array of segment offsets
 * (terminated by a zero offset), followed by ScriptOp entries pointed-to
 * by those offsets. Markers in the 0xFFxx range gate sub-states; non-FF
 * codes are dispatched to handlers.
 */
typedef struct {
    u16 op;
    u16 param;
} ScriptOp;

/**
 * @brief Large struct (~0x6A bytes) holding per-entity flags and state.
 *
 * Only the fields touched so far are known; remaining bytes are padding.
 * @c field46 is a packed state word whose low bits select a mode:
 * @c func_800BD540 writes it from selector tables, and @c func_80099B48
 * gates its script-image walk on @c (field46 & 1).
 */
typedef struct {
    u8  pad00[0x46];
    u16 field46;        /**< 0x46: packed-word state flags (low bits = mode). */
    u8  pad48[0x66 - 0x48];
    u8  flag66;         /**< 0x66: mode-selector flags consulted after write. */
    u8  pad67;
    u8  sel68;          /**< 0x68: 5-bit selector consumed when D_800C971C == 0. */
    u8  sel69;          /**< 0x69: 2-bit selector consumed when D_800C971C != 0. */
} Entity;

/**
 * @brief Per-actor 36-byte state record indexed by actor id.
 *
 * Used by the dialogue/animation orchestrator.  Currently mapped fields:
 * @c unk02 / @c unk03 (counters), @c flag1E (-1 disables actor), and
 * @c unk1F (signed phase byte read by @c func_800B674C).
 */
typedef struct {
    /* 0x00 */ u8 pad00[2];
    /* 0x02 */ s8 unk02;            /**< Signed counter / clamp value. */
    /* 0x03 */ u8 unk03;            /**< Byte counter (range-checked against 1..2). */
    /* 0x04 */ u8 pad04[0x14];
    /* 0x18 */ void *rotSrc; /**< RotationSources* — vecB lives at +0x68. */
    /* 0x1C */ u8 pad1C[2];
    /* 0x1E */ s8 flag1E;           /**< -1 disables actor; otherwise active. */
    /* 0x1F */ s8 unk1F;            /**< Signed phase byte (-1, 0, 1). */
    /* 0x20 */ u8 pad20[4];
} ActorRecord;                       /* 0x24 bytes */

/**
 * @brief 32-byte node in a sorted keyframe list traversed by @c func_800B674C.
 *
 * The first halfword is a time/key value with sentinels @c -1 (end of
 * list) and @c -2 (skip / continuation).  Remaining 30 bytes hold the
 * keyframe payload, format-dependent on caller.
 */
typedef struct {
    /* 0x00 */ s16 time;
    /* 0x02 */ u8 pad02[0x1E];
} KeyframeNode;                      /* 0x20 bytes */

extern ActorRecord D_800DD6A8[];

/**
 * @brief Slot in the 64-entry world-particle pool at @c D_800D9CB0.
 *
 * Stride 0x30. Used by @c func_800AC0A0 (spawn) and friends. The
 * @c count vs @c limit fields drive slot lifecycle: a slot with
 * @c count @>= @c limit is inactive and reusable.
 */
typedef struct {
    /* 0x00 */ VECTOR  pos;             /**< World position (12 bytes used + pad). */
    /* 0x10 */ SVECTOR view;            /**< Camera-relative position, refreshed every
                                             frame by @c func_800B01A0 and fed to the
                                             GTE by the renderer (@c func_800A9300). */
    /* 0x18 */ SVECTOR rot;             /**< Rotation vector (8 bytes). */
    /* 0x20 */ SVECTOR vel;             /**< Per-tick position delta, seeded by the
                                             spawner from the kind's GTE-projected
                                             offset and added to @c pos each step. */
    /* 0x28 */ u8      limit;           /**< Active-slot kill threshold for @c count. */
    /* 0x29 */ u8      count;           /**< Lifecycle tick counter (the particle's age). */
    /* 0x2A */ u8      kind;            /**< Slot kind (= index into @c D_800C5480). */
    /* 0x2B */ u8      pad2B;
    /* 0x2C */ u16     scale;           /**< Current size, 4.12 fixed (spawns at 0x1000
                                             = 1.0 and is scaled every tick). */
    /* 0x2E */ u8      pad2E[0x02];
} Slot30;                              /* 0x30 bytes */

/**
 * @brief Per-kind particle description at @c D_800C5480.
 *
 * Stride 0x28, indexed by @c Slot30::kind. Carries everything the
 * renderer (@ref func_800A9300) needs to draw one slot: where its sprite
 * lives in VRAM, how it is oriented, and how it animates.
 */
typedef struct {
    /* 0x00 */ s16     u;               /**< Texture u of frame 0, in 4-pixel units. */
    /* 0x02 */ s16     v;               /**< Texture v; bits 8 and 9 also carry tpage flags. */
    /* 0x04 */ u16     clutX;           /**< CLUT VRAM x. */
    /* 0x06 */ u16     clutY;           /**< CLUT VRAM y. */
    /* 0x08 */ u8      w;               /**< Sprite width in texels. */
    /* 0x09 */ u8      h;               /**< Sprite height in texels. */
    /* 0x0A */ u8      prim;            /**< Primitive to emit: 0 = TILE, 1 = POLY_FT4. */
    /* 0x0B */ u8      unk0B;
    /* 0x0C */ u8      r, g, b;         /**< Flat colour applied to the primitive. */
    /* 0x0F */ u8      frameCount;      /**< Number of animation frames. */
    /* 0x10 */ u16     scaleRate;       /**< Per-tick scale multiplier, 4.12 fixed. */
    /* 0x12 */ u8      limit;           /**< Initial @c Slot30::limit value. */
    /* 0x13 */ u8      frameDiv;        /**< Ticks per animation frame. */
    /* 0x14 */ u8      shape;           /**< Billboard shape, see @c PARTICLE_SHAPE_*. */
    /* 0x15 */ u8      tpageBits;       /**< Low 2 bits select the texture page's colour mode. */
    /* 0x16 */ s16     size;            /**< Size multiplier against @c Slot30::scale. */
    /* 0x18 */ SVECTOR offset;          /**< Per-kind GTE-projected offset. */
    /* 0x20 */ SVECTOR vel;             /**< Per-tick velocity increment added to @c Slot30::vel. */
} KindParams;                          /* 0x28 bytes */

/* KindParams::shape — how the sprite is oriented in the world. */
#define PARTICLE_SHAPE_GROUND    0  /**< Flat on the ground plane, w x w. */
#define PARTICLE_SHAPE_SCREEN    1  /**< Screen-aligned square around the projected point. */
#define PARTICLE_SHAPE_ROTATED   2  /**< Screen-aligned, sized by a second projection. */
#define PARTICLE_SHAPE_GROUND_2X 3  /**< Flat on the ground plane, stretched 2x in z. */
#define PARTICLE_SHAPE_UPRIGHT   4  /**< Standing in the x/y plane. */

/** @brief 1.0 in @c Slot30::scale's 4.12 fixed-point format; every spawner
 *         seeds a slot at full size. */
#define PARTICLE_SCALE_ONE 0x1000

/* Spawn-flag bits for the Slot30 pool spawners (func_800AC0A0 and the
 * fixed-kind wrappers around it). */
#define SLOT_FLAG_JITTER_LIMIT 1  /**< RNG-jitter Slot30::limit by [-4, +3]. */
#define SLOT_FLAG_JITTER_SCALE 2  /**< RNG-jitter Slot30::scale by [-0x80, +0x7F]. */
#define SLOT_FLAG_UNUSED4      4  /**< Never set by any caller; meaning unknown. */

extern Slot30     D_800D9CB0[64];
extern MATRIX     D_800DA8B0;          /**< GTE lighting color matrix (sits at &D_800D9CB0[64]). */
extern MATRIX     D_800C5428;          /**< Source color matrix copied into D_800DA8B0. */
extern CVECTOR    D_800C53F8;          /**< Background (ambient) color source. */
extern CVECTOR    D_800DB0DC;          /**< Fourth palette stop; the world-zone blender fills it
                                            alongside D_800DB0D0[0..2] and D_800DB0E0. */
extern CVECTOR    D_800DB0E0;          /**< Active background-color cache (programmed via SetBackColor). */
/** Base of the world zone block: a table of s32 offsets, each relative to
 *  this pointer, locating one record. Entry 0 is the map-wide default. */
extern s32       *D_800C9EE8;
extern VECTOR     D_800DB0E8;          /**< World camera-base translation (GTE TR bias; used by setWorldMapTransVector and func_800423DC). */
extern KindParams D_800C5480[];

/**
 * @brief 12-byte transform track (translation + y-rotation).
 *
 * Two of these live inside @c Slot at offsets 0x18 (track A) and 0x24
 * (track B). The translation z is negated when copied into a VECTOR
 * (PSX y-down convention).
 */
typedef struct {
    /* 0x00 */ s32 trans_x;
    /* 0x04 */ s32 trans_z;
    /* 0x08 */ s16 trans_y;
    /* 0x0A */ u16 rot_y;
} Track;

/**
 * @brief Slot-state record at @c *D_800D226C (used by 0xFF13/0xFF14
 * placement opcodes and 0xFF28 / 0xFF2E flag handlers).
 *
 * Holds two transform tracks and a 64-bit flag set; the trailing
 * @c bytes pair selects per-action data via the 0xFF2E handler.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x18];
    /* 0x18 */ Track tracks[2];   /**< Track A at 0x18, Track B at 0x24. */
    /* 0x30 */ u8 pad30[0x38];
    /* 0x68 */ SceneState scene;  /**< Saved scene state; the world entry loop
                                       saves the whole record here on a cold
                                       start and restores @c cmd from it on a
                                       warm one. */
    /* 0x6C */ s32 unk6C;         /**< Flags word; see SLOT_FLAG_CMD_MIRROR. */
    /* 0x70 */ u8 pad70[0x4];
    /* 0x74 */ u32 flags[2];      /**< 64-bit flag set (low/high). */
    /* 0x7C */ u8 bytes[2];       /**< Action bytes selectable by 0xFF2E. */
    /* 0x7E */ u8 pad7E[2];
} Slot;

/** @c Slot.unk6C bit mirroring @c CmdDesc.flag bit 0x08 (hence the shift by 5). */
#define SLOT_FLAG_CMD_MIRROR 0x100

/**
 * @brief 16-byte transform entry from @c *D_800D2128 — VECTOR translation
 * plus a partial (y, z) rotation pair.
 *
 * The first 12 bytes alias a @c VECTOR's vx/vy/vz translation; the trailing
 * 4 bytes hold a (y-rotation, z-rotation) pair that callers read separately.
 */
typedef struct {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
    /* 0x0C */ u16 rot_y;
    /* 0x0E */ s16 rot_z;
} TransformEntry;

extern u8 *D_800D2288;
extern Slot *D_800D226C;
extern TransformEntry *D_800D2128;

/** @brief One 0x18-byte worldmap sub-OT slot: link tag + prim payload. */
typedef struct {
    P_TAG link;          /**< 0x00: P_TAG with low-24 next-prim addr. */
    u8    pad08[0x10];   /**< 0x08..0x17: slot payload (primed by DrawPrim). */
} OTSubSlot;

extern BattleSceneCtx D_800CA040;       /**< Worldmap "no-battle" sentinel — also functions as an empty BattleSceneCtx. */
extern s16         D_800C53B8[];        /**< Bone-id table (used by we_object4). */
extern s32 func_800AF28C(ScriptOp *p);
extern s32 func_800BEFC4(void);
extern s32 func_800BD82C(u8 *actor, SlotEntry *slot, s32 marker, s32 flag, SVECTOR *rot, VECTOR *trans);

/**
 * @brief Pair of rotation source vectors at known offsets. Pointed to by
 *        @c D_800DD6C0 / @c D_800DD6E4. The leading @c 0x34 bytes and the
 *        gap between the two vectors are not yet modeled.
 */
typedef struct {
    /* 0x00 */ u8     unk00[0x34];
    /* 0x34 */ VECTOR vecA;
    /* 0x44 */ u8     unk44[0x08];
    /* 0x4C */ SVECTOR vec4C;       /**< Rotation snapshot used by func_800BE040. */
    /* 0x54 */ u8     unk54[0x14];
    /* 0x68 */ VECTOR vecB;
} RotationSources;

extern RotationSources *D_800DD6C0;
extern RotationSources *D_800DD6E4;

/**
 * @brief Two back-to-back offset arrays at the start of the string-table blob
 *        @c D_800C97D4 points to. Each entry in @c first / @c second is a byte
 *        offset (from the table base) to a null-terminated string stored later
 *        in the same blob.
 */
typedef struct {
    s32 first[30];
    s32 second[1];
} StringTable;

extern StringTable *D_800C97D4;

#endif /* WORLD_H */
