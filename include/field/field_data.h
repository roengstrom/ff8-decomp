#ifndef FIELD_DATA_H
#define FIELD_DATA_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "snd_cd.h"

/** @brief @c FieldGateway::fieldId of an unused gateway slot. */
#define FIELD_GATEWAY_UNUSED 0x7FFF
/** @brief @c FieldGateway::spawnTriIdx that makes the crossing scan skip a slot. */
#define FIELD_GATEWAY_NO_TRIANGLE 0xFFFF

/**
 * @brief One of the 12 gateways in @ref FieldInfo: an exit line, and the field
 *        and position that crossing it leads to.
 */
typedef struct {
    /* 0x00 */ s16 x0;          /**< Exit line start X. */
    /* 0x02 */ s16 y0;          /**< Exit line start Y. */
    /* 0x04 */ s16 z0;          /**< Exit line start Z. */
    /* 0x06 */ s16 x1;          /**< Exit line end X. */
    /* 0x08 */ s16 y1;          /**< Exit line end Y. */
    /* 0x0A */ s16 z1;          /**< Exit line end Z. */
    /* 0x0C */ u16 position_x;  /**< Arrival X, copied to @c D_800704A8.position_x by @c func_8009AA64. */
    /* 0x0E */ u16 position_y;  /**< Arrival Y, copied to @c D_800704A8.position_y. */
    /* 0x10 */ u16 spawnTriIdx; /**< Arrival triangle, copied to @c D_800704A8.spawnTriIdx;
                                     @c FIELD_GATEWAY_NO_TRIANGLE makes @c func_8009AAC8 skip it. */
    /* 0x12 */ u16 fieldId;     /**< Destination field, copied to @c D_800704A8.counter;
                                     @c FIELD_GATEWAY_UNUSED in an unused slot. */
    /* 0x14 */ u8 pad14[0x08];
    /* 0x1C */ u8 anim_state;   /**< Copied to @c D_800704A8.anim_state (low byte). */
    /* 0x1D */ u8 pad1D[0x03];
} FieldGateway; /* 0x20 = 32 bytes */

/** @brief 8-byte rectangle given by its four edges. */
typedef struct {
    /* 0x00 */ s16 top;
    /* 0x02 */ s16 bottom;
    /* 0x04 */ s16 right;
    /* 0x06 */ s16 left;
} ClampRect;

/**
 * @brief One entry of the field line-trigger table (12 per table, 16-byte
 *        stride) held at @c FieldInfo.triggers, scanned by @c func_800A6100 /
 *        @c func_800A62EC.
 *
 * A 3D line segment from @c (x0,y0,z0) to @c (x1,y1,z1). @c marker @c == @c 0xFF
 * flags an empty slot; @c type selects the @c func_800A5FA4 dispatch behaviour.
 * @c func_8009A2BC reads all three axes, so @c z0 / @c z1 are real Z coordinates.
 *
 * @note Separate storage from the per-entity trigger volume an @ref Eline
 *       carries (the @c SETLINE opcode), which it resembles only in layout.
 *       The @ref FieldInfo is a field data member: its base comes from
 *       @c g_fieldInfo (set up with the other @c 0x800E1000 field data
 *       handles by @c func_8009895C, then latched into @c g_curFieldInfo by
 *       @c func_800983F0). No game code stores into this region, so the 12
 *       trigger lines are field-file data loaded with the field;
 *       @c func_800A6100 / @c func_800A62EC only read them against entity
 *       positions.
 */
typedef struct {
    /* 0x00 */ s16 x0;
    /* 0x02 */ s16 y0;
    /* 0x04 */ s16 z0;
    /* 0x06 */ s16 x1;
    /* 0x08 */ s16 y1;
    /* 0x0A */ s16 z1;
    /* 0x0C */ u8  marker;
    /* 0x0D */ u8  unk0D;
    /* 0x0E */ u8  type;
    /* 0x0F */ u8  unk0F;
} FieldLineTrigger;

/**
 * @brief The field info member of the field data archive, held at
 *        @c g_curFieldInfo: a 0x64-byte header, then 12 gateways and 12 line
 *        triggers.
 */
typedef struct {
    /* 0x000 */ char name[9];         /**< Map name, NUL-padded. */
    /* 0x009 */ u8 unk09[2];         /**< One byte per camera, indexed by @c SystemState::unk1A6: copied into
                                          @c SystemState::unk1A8 and @c unk100 on field entry and on a camera change. */
    /* 0x00B */ u8 slotHeadingBias[2]; /**< Per-party-slot heading bias the field bundle ships;
                                            @c func_8009BEC8 adds it to the global heading before
                                            the analog-stick direction. @note Name inferred from that use. */
    /* 0x00D */ u8 unk0D;            /**< Field-bundle variant flag: selects the @c D_800C315C command table over
                                          @c D_800C311C in @c func_800983F0, and forces the actor-pool install
                                          (@c func_800A1CC0) even for load modes 1 and 6. */
    /* 0x00E */ u8 movieMask;        /**< 1 for a field with a movie mask: gates building and drawing it,
                                          and the palette-strip StoreImage in @c func_800A1BB8. */
    /* 0x00F */ u8 pad0F;
    /* 0x010 */ u8 tpageX;           /**< Texture-page X written into every field sprite's tpage word. */
    /* 0x011 */ u8 pad11;
    /* 0x012 */ u16 baseZ;           /**< Base Z offset added to the per-entity Z when building SVECTOR (func_800A11E0). */
    /* 0x014 */ ClampRect cameraRanges[8]; /**< Per-region clamp rectangles, consumed by @c func_800A0FB8. */
    /* 0x054 */ ClampRect screenRanges[2]; /**< @c screenRanges[0] is the padding margin
                                                @c func_800A0FB8 shrinks @c cameraRanges by. */
    /* 0x064 */ FieldGateway gateways[12];
    /* 0x1E4 */ FieldLineTrigger triggers[12]; /**< Line-trigger table scanned by @c func_800A6100. */
} FieldInfo;

extern FieldInfo *g_curFieldInfo;

/**
 * @brief One field camera: the matrix @c SetRotMatrix / @c SetTransMatrix are
 *        loaded from, followed by projection parameters.
 */
typedef struct {
    /* 0x00 */ MATRIX m;
    /* 0x20 */ u16 viewOfsX;     /**< View's screen-space X offset: added to the draw offset by
                                      @c func_800A15C0 and subtracted from projected positions
                                      elsewhere. @note Purpose inferred from those two uses. */
    /* 0x22 */ u16 viewOfsY;     /**< View's screen-space Y offset; twin of @c viewOfsX. */
    /* 0x24 */ s16 spriteScale;  /**< Numerator of the per-OTZ sprite scale in @c func_800A39D8. */
} FieldView;

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

/** @brief A field's walkmesh: @c count triangles, then one @ref AdjRec per triangle. */
typedef struct {
    /* 0x0 */ s32 count;
    /* 0x4 */ Triangle tris[1];        /* variable length */
} TriangleList;

/** @brief 6-byte per-triangle adjacency record — neighbor triangle index per edge (0xFFFF = none). */
typedef struct {
    u16 neighbor[3];
} AdjRec;

/** @brief The @c x of the tile that ends a field's tile map. */
#define MAP_TILE_END 0x7FFF

/** @brief One 16-byte tile of a field's background tile map, drawn as a 16x16 sprite. */
typedef struct MapTile {
    /* 0x00 */ s16 x;           /**< Screen X; @c MAP_TILE_END ends the map. */
    /* 0x02 */ s16 y;           /**< Screen Y. */
    /* 0x04 */ u16 z;           /**< Depth key. */
    /* 0x06 */ u16 tpage;       /**< GPU texture-page bits: page X in 64-pixel steps, Y base, colour depth. */
    /* 0x08 */ u16 clut;        /**< Palette (CLUT) id for the sprite. */
    /* 0x0A */ u8  u;           /**< Texture u of the 16x16 cell. */
    /* 0x0B */ u8  v;           /**< Texture v of the 16x16 cell. */
    /* 0x0C */ u8  padC;
    /* 0x0D */ u8  blendMode;   /**< @c 4 = opaque, else semi-translucent. */
    /* 0x0E */ u8  padE[2];
} MapTile;

/**
 * @brief One 20-byte movement-command step: an endpoint plus the tick count to
 *        reach it. @c func_800A38B4 lerps between consecutive steps.
 */
typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ s16 z;
    /* 0x06 */ s16 angle;
    /* 0x08 */ u8  spriteX;       /**< Sprite half-width at this waypoint (lerped by func_800A39D8). */
    /* 0x09 */ u8  spriteY;       /**< Sprite half-height at this waypoint. */
    /* 0x0A */ u8  u;             /**< Texture U of the sprite's top-left corner. */
    /* 0x0B */ u8  v;             /**< Texture V of the sprite's top-left corner. */
    /* 0x0C */ u8  w;             /**< Texture width; the far corner is @c u+w-1. */
    /* 0x0D */ u8  h;             /**< Texture height; the far corner is @c v+h-1. */
    /* 0x0E */ u8  stepTotal;     /**< Ticks this step lasts; 0 ends the command. */
    /* 0x0F */ u8  mode;          /**< 4 = opaque; otherwise the low 2 bits are the GPU
                                       semi-transparency mode written into the tpage word. */
    /* 0x10 */ u8  r;             /**< Vertex colour at this waypoint (lerped to the next). */
    /* 0x11 */ u8  g;
    /* 0x12 */ u8  b;
    /* 0x13 */ u8  pad13;
} MoveStep;                       /* 0x14 = 20 bytes */

/** @brief One 372-byte movement command: up to 17 @ref MoveStep waypoints plus
 *         a count of the accumulators still running it. */
typedef struct {
    /* 0x000 */ MoveStep steps[17];
    /* 0x154 */ s16 zBias;        /**< Added to the projected OTZ before the range check. */
    /* 0x156 */ u8  pad156[0x06];
    /* 0x15C */ u16 activeCount;
    /* 0x15E */ u8  pad15E[0x0C];
    /* 0x16A */ s16 flags;        /**< Non-zero above bit 6 halves the sprite scale shift. */
    /* 0x16C */ u8  pad16C[0x08];
} MoveRecord;                     /* 0x174 = 372 bytes */

/**
 * @brief One 32-byte movement accumulator — the @c func_800A38B4 output view
 *        plus the bookkeeping @ref func_800A3FE0 uses to walk its command.
 */
typedef struct {
    /* 0x00 */ s32 posX;
    /* 0x04 */ s32 posY;
    /* 0x08 */ s32 posZ;
    /* 0x0C */ s16 xStart;
    /* 0x0E */ s16 yStart;
    /* 0x10 */ s16 zStart;
    /* 0x12 */ u16 angle;
    /* 0x14 */ u8  pad14[0x02];
    /* 0x16 */ s16 angleStart;
    /* 0x18 */ u8  cmdIndex;      /**< Index into @c FieldParticles.records. */
    /* 0x19 */ u8  stepIndex;     /**< Current waypoint within that command. */
    /* 0x1A */ u8  stepProgress;  /**< Ticks spent on the current waypoint. */
    /* 0x1B */ u8  active;        /**< 1 while this accumulator is running a command. */
    /* 0x1C */ u8  pad1C[0x04];
} MoveAccum;                      /* 0x20 = 32 bytes */

/**
 * @brief One 254-byte animation slot of @ref FieldParticles, walked by
 *        @ref func_800A37A8.
 *
 * The leading @c subscene region is a packed @ref ActorAnim — only its first
 * @c 0xFE bytes are live, so slots pack at 254-byte stride — and is passed to
 * @c func_800A355C as its @c ActorAnim argument. @c h2 mirrors
 * @c ActorAnim.animOffset (offset @c 0xF4).
 */
typedef struct {
    /* 0x00 */ u8 subscene[0xD0]; /**< Packed ActorAnim head (rows/timers/...). */
    /* 0xD0 */ u8 table[0x20];    /**< Animation source bytes, indexed by @c h2. */
    /* 0xF0 */ s16 h0;            /**< State counter (advanced each active tick). */
    /* 0xF2 */ s16 h1;            /**< State counter, compared against @c table[h2]. */
    /* 0xF4 */ s16 h2;            /**< Table cursor (mirror of @c ActorAnim.animOffset). */
    /* 0xF6 */ u16 padF6;
    /* 0xF8 */ s16 frameCount;    /**< Frames this slot plays for; @c func_800A3FE0 runs the
                                       whole buffer for @c max(frameCount) ticks and drops the
                                       slot once the tick passes it. */
    /* 0xFA */ u8 padFA[0x04];
} FieldSubsceneSlot;             /* 0xFE = 254 bytes */

/**
 * @brief The field's particle system: 16 movement-command records (the
 *        emitters), 16 animation slots, 128 movement accumulators (the
 *        particles) and a double-buffered @c POLY_FT4 arena, walked by
 *        @ref func_800A37A8 and fast-forwarded whole by @ref func_800A3FE0.
 *
 * The regions pack exactly: @c 16*0x174 == @c 0x1740 and @c 16*0xFE == @c 0xFE0,
 * putting @c entries at @c 0x2720. Those first 0x2720 bytes are what the field
 * archive's particle member holds; the rest is work space the loader lays over
 * the member after it.
 */
typedef struct {
    /* 0x0000 */ MoveRecord records[16];
    /* 0x1740 */ FieldSubsceneSlot slots[16];
    /* 0x2720 */ MoveAccum entries[128];
    /* 0x3720 */ POLY_FT4 primArena[2][128]; /**< Double-buffered sprite arena; @c primCursor is
                                                  reset to the current buffer's half each frame. */
    /* 0x5F20 */ POLY_FT4 *primCursor; /**< Next free prim in @c primArena. */
} FieldParticles;                      /* 0x5F24 */

/** @brief The 4-byte encounter member; only @c stepRate is read. */
typedef struct {
    /* 0x0 */ u8 stepRate;  /**< Added to the step accumulator on every step. */
    /* 0x1 */ u8 unk1[3];   /**< Three copies of one value: 0, 4, 8 or 12 on disc 1. */
} FieldEncounterRate;

/** @brief The four battle formations a random encounter on this field picks from. */
typedef struct {
    /* 0x0 */ u16 formation[4];
} FieldFormations;

/**
 * @brief Header of the field data archive, which is decompressed whole to
 *        @c 0x800E1000.
 *
 * Each word is the absolute address of one member, and @c end is one past the
 * last. A member's size is the gap to the next address, so two equal
 * addresses mean an empty member.
 */
typedef struct {
    /* 0x00 */ FieldInfo          *info;
    /* 0x04 */ FieldView          *views;
    /* 0x08 */ TriangleList       *walkmesh;
    /* 0x0C */ MapTile            *tileMap;
    /* 0x10 */ TriangleList       *movieMask;     /**< Drawn only while a movie plays, to set the GPU
                                                       mask bit, and only when @c FieldInfo::movieMask is 1. */
    /* 0x14 */ FieldEncounterRate *encounterRate;
    /* 0x18 */ FieldFormations    *formations;
    /* 0x1C */ OffsetTable        *sfxTable;      /**< Entries are AKAO sound sequences. */
    /* 0x20 */ OffsetTable        *messages;      /**< Entries are message strings. */
    /* 0x24 */ FieldParticles     *particles;     /**< The first 0x2720 bytes of @ref FieldParticles, or
                                                       a 4-byte @c 0x2020 placeholder when the field has none. */
    /* 0x28 */ u8                 *script;        /**< Copied out as bytes, after which the loader
                                                       overwrites this slot with the next free address. */
    /* 0x2C */ u8                 *end;           /**< One past the last member. */
} FieldData;

/**
 * @brief Handles on the @ref FieldData slots: each holds the address of its
 *        slot, so a member is read as @c *handle.
 *
 * @note Code reaches members only through these, never through a
 *       @ref FieldData pointer: a struct access addresses the slot
 *       directly, while the original loads the handle and reads offset 0.
 */
extern FieldInfo          **g_fieldInfo;
extern FieldView          **g_fieldViews;
extern TriangleList       **g_fieldWalkmesh;
extern MapTile            **g_fieldTileMap;
extern TriangleList       **g_fieldMovieMask;
extern FieldEncounterRate **g_fieldEncounterRate;
extern FieldFormations    **g_fieldFormations;
extern OffsetTable        **g_fieldSfxTable;
extern OffsetTable        **g_fieldMessages;
extern FieldParticles     **g_fieldParticles;
extern u8                 **g_fieldScript;
extern u8                 **g_fieldDataEnd;

#endif /* FIELD_DATA_H */
