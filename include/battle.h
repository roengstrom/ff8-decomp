#ifndef BATTLE_H
#define BATTLE_H

#include "common.h"
#include "psxsdk/libgpu.h"

/** @brief Battle result values (BattleConfig.result). */
#define BATTLE_RESULT_UNDETERMINED  0
#define BATTLE_RESULT_GAMEOVER      1
#define BATTLE_RESULT_ESCAPED       2
#define BATTLE_RESULT_WIN           4

/** @brief Battle command config (g_battleConfig). */
typedef struct {
    u16 battleSceneId;
    u16 unk2; // Flags?, when first bit is set, escape it not possible
    u8 unk4[3]; // Post battle command queue?
    u8 result;          /**< Battle result (BATTLE_RESULT_*). */
    u8 unk8;
} BattleConfig;

typedef struct {
    u8 field00;
    u8 field01;
    u16 field02;
    u8 params[4]; /**< 0x04-0x07: Indexed by param in func_80027FDC. */
    u16 field08;
    u16 field0A;
    u16 field0C;
    u16 field0E;
    u16 field10;
    u16 field12;
} AnimFrame; /* 0x14 = 20 bytes */

typedef struct {
    u8 pad00[6];
    u8 field06;
    u8 field07;
    u8 pad08[2];
    u8 field0A;
    u8 field0B;
    u8 field0C;
    u8 field0D;
    u8 field0E;
    u8 field0F;
    s16 unk10[4];
    u8 frameCounter;
    u8 field19;
    s8 field1A;
    u8 opacity;
    AnimFrame frames[8];
    u8 padBC[6];
    u8 linkedIdx;
    u8 fieldC3;
} BattleAnimEntity;

#define OT_SIZE 18

/** @brief Display list double-buffer entry (stride 0x58 = 88 bytes). */
typedef struct {
    u32 pktAlloc;
    u32 pktLimit;
    u32 ot[OT_SIZE];
    u32 pad50;
    u32 pktBase;
} DisplayListBuf;

/** @brief Complete battle animation state (entities + global coords). */
typedef struct {
    /* 0x000 */ BattleAnimEntity entities[2]; /**< Two animation entities. */
    /* 0x188 */ u8 cdBufA[0x24];             /**< CD audio buffer A. */
    /* 0x1AC */ u8 cdBufB[0x24];             /**< CD audio buffer B. */
    /* 0x1D0 */ s16 globalCoords[2][2];      /**< Per-slot coords [slot][axis]. */
    /* 0x1D8 */ u16 clipLeft;               /**< Clip region left edge. */
    /* 0x1DA */ u16 clipTop;                /**< Clip region top edge. */
    /* 0x1DC */ u16 clipRight;              /**< Clip region right edge. */
    /* 0x1DE */ u16 clipBottom;             /**< Clip region bottom edge. */
    /* 0x1E0 */ u8 defaultColor;             /**< Default color value for entity init. */
    /* 0x1E1 */ u8 pad1E1[0x45F];            /**< Unknown. */
    /* 0x640 */ DisplayListBuf bufs[2];         /**< Double-buffered GPU display lists (2 × 0x58). */
    /* 0x6F0 */ DisplayListBuf *active;      /**< Pointer to active display list buffer. */
    /* 0x6F4 */ s32 halfSize;                /**< Half of total VRAM size. */
    /* 0x6F8 */ u8 pad6F8[4];                /**< Unknown. */
    /* 0x6FC */ s32 field6FC;                /**< Cleared during GPU init. */
    /* 0x700 */ u8 pad700[0x274];            /**< Unknown. */
    /* 0x974 */ s32 palette[3];              /**< RGB888 palette (0x40BBGGRR). */
    /* 0x980 */ u8 pad980[0x42];             /**< Unknown. */
    /* 0x9C2 */ s16 field9C2;               /**< Set to 0x4611 during GPU init. */
    /* 0x9C4 */ s16 cdStreamCounter;         /**< CD stream counter. */
    /* 0x9C6 */ u8 pad9C6[2];                /**< Unknown. */
    /* 0x9C8 */ s32 field9C8;                /**< Cleared during GPU init. */
    /* 0x9CC */ s32 field9CC;                /**< Cleared during GPU init. */
} BattleAnimState;

extern BattleAnimState g_battleAnims;

/** @brief TIM image section (CLUT or pixel data). */
typedef struct {
    s32 len;            /**< 0x00: Section length in bytes (including this header). */
    RECT rect;          /**< 0x04: Destination rectangle in VRAM. */
    u16 data[0];        /**< 0x0C: Section data (variable length). */
} TimSection;

/** @brief TIM image file header (PS1 standard texture format). */
typedef struct {
    u32 id;             /**< 0x00: Magic ID (0x10). */
    u32 flags;          /**< 0x04: Format flags. */
    TimSection clut;    /**< 0x08: CLUT section (followed by pixel section). */
} Tim;

struct BattleDisplayEntity;
typedef void (*EntityCallback)(struct BattleDisplayEntity *);

typedef struct BattleDisplayEntity {
    EntityCallback callback; /* 0x00: update function pointer */
    s32 field04;
    RECT boundRect;
    RECT dispRect;
    u8 pad18[0x18];
    s32 drawMode;
    u8 activeFlag;
    u8 field35;
    u8 field36;
    u8 animSpeed;
    u8 entityType;
    u8 pad39;
    u8 subFields[2];
    s16 scale;
    s16 pad3E;
} BattleDisplayEntity;

typedef struct {
    RECT rect;
    u8 *dataPtr;
    u8 *dataPtrCopy;
    s16 pitch;
    u16 field12;
    union {
        u32 raw;
        struct {
            s16 field14;
            u8 state;
            u8 field17;
        } fields;
    } flags;
    u8 entityIdx;
    s8 field19;
    s16 volume;
    s16 field1C;
    s16 rateDelta;
    u8 field20;
    u8 field21;
    u8 field22;
    u8 field23;
    s32 seqState;
    u8 field28;
    u8 field29;
    u8 field2A;
    u8 field2B;
    u8 field2C;
    u8 mode;
    u8 pad2E;
    u8 field2F;
    u16 field30;
    u8 field32;
    u8 pad33;
    s32 field34;
    s32 field38;
} SfxEntry;

typedef struct {
    u8 pad00[3];
    u8 counter;         /* 0x03 */
    u32 color1;         /* 0x04: flash color (processed) */
    u32 color2;         /* 0x08: flash color (output) */
    s8 activeFlag;      /* 0x0C */
    u8 pad0D[7];
    u8 field14;
    u8 field15;
    u8 pad16[2];
    u16 field18;
    u16 field1A;
} SfxGlobalState;

/** @brief Complete SFX system: 8 entry slots + global state + message display values. */
typedef struct {
    SfxEntry entries[8];       /* 0x000: 8 × 60 = 480 bytes */
    SfxGlobalState state;      /* 0x1E0: global SFX state */
    u8 pad1FC[4];              /* 0x1FC: padding */
    u32 msgValues[8];          /* 0x200: numeric values formatted by decodeMessage */
} SfxSystem;

/** @brief Message formatting config (D_80083858). */
typedef struct {
    u8 digitBase;              /* 0x00: base character code for digit rendering */
    u8 pad01[0xF];             /* 0x01: padding */
    u8 separator;              /* 0x10: thousands separator character */
} MsgFormatConfig;

/** @brief Message state struct passed to decode/advance functions.
 *
 * Accessed as s32[] array in some functions, with a u8 skip count at +0x22.
 * Fields at +0x00..+0x07 are unknown; +0x08 is the stream pointer,
 * +0x0C is the stored/output pointer.
 */
typedef struct {
    s32 unk00;                  /* 0x00 */
    s32 unk04;                  /* 0x04 */
    s32 streamPtr;              /* 0x08: current stream pointer (a0[2]) */
    s32 storedPtr;              /* 0x0C: stored/output pointer (a0[3]) */
    u8 pad10[0x12];             /* 0x10 */
    u8 skipCount;               /* 0x22: number of control codes to skip */
    u8 pad23[1];                /* 0x23: pad to 0x24 */
} MsgState;

typedef enum {
    CTRL_ACTIVE     = 0x01,
    CTRL_FLAG_02    = 0x02,
    CTRL_FLAG_10    = 0x10,
    CTRL_FLAG_40    = 0x40,
    CTRL_FLAG_80    = 0x80,
    CTRL_FLAG_100   = 0x100
} ControlFlags;

typedef struct {
    u8 pad00[0x04];
    s32 state;
    u8 pad08[0x04];
    u8 timer;
    u8 control;
    u8 pad0E;
    u8 entityRef;
    s32 linkedPtr;
    u8 pad14[0x04];
    s32 flags;
    s32 flagsBackup;
    u8 pad20[0x08];
    s32 field28;
    u8 pad2C[0x58];
    u16 animParam1;
    u16 animParam2;
    u16 animParam3;
    u8 pad8A[0x02];
    ControlFlags controlFlags;
    u16 status;
    u16 statusBackup;
    u8 pad94[0x27];
    u8 linkedIdx2;
    u8 padBC[0x0F];
    u8 linkedIdx;
    u8 padCC[0x04];
} BattleEntity;

/** @brief Battle magic slot entry (5 bytes). */
typedef struct {
    u8 field0;
    u8 field1;
    u8 field2;
    u8 field3;
    u8 field4;
} BattleMagicSlot;

/** @brief Battle item slot entry (5 bytes). */
typedef struct {
    u8 field0;
    u8 field1;
    u8 field2;
    u8 field3;
    u8 field4;
} BattleItemSlot;

/** @brief Battle command slot entry (4 bytes). */
typedef struct {
    u8 field0;
    u8 field1;
    u8 field2;
    u8 field3;
} BattleCmdSlot;

/** @brief Data stream within a battle command (two per entry). */
typedef struct {
    u8 *start;       /* +0x00: pointer to stream data start */
    u8 *end;         /* +0x04: pointer to stream data end */
    s16 cursor;      /* +0x08: current read position (-1 = not started) */
    u16 length;      /* +0x0A: stream length in bytes */
    u8 enabled;      /* +0x0C: 1 if stream has data, 0 if empty */
    u8 pad0D[3];     /* +0x0D: padding */
} CmdStream;

/** @brief Battle command table entry (g_battleCmdTable, stride 0x24 = 36 bytes). */
typedef struct {
    CmdStream streams[2]; /* +0x00: two data streams (0x20 bytes) */
    u16 sourceId;         /* +0x20: source ID (wraps at 0x400, reset to 1) */
    s8 active;            /* +0x22: priority/active flag */
    u8 index;             /* +0x23: slot index (0-3) */
} BattleCmdEntry;

/** @brief Header for packed command stream data within a command data block. */
typedef struct {
    u16 len1;    /* +0x00: length of first stream */
    u16 len2;    /* +0x02: length of second stream */
    u8 data[1];  /* +0x04: stream1 data[len1], then stream2 data[len2] */
} CmdStreamHeader;

/** @brief Memory card subsystem data block (g_cardData). */
typedef struct {
    s32 events[8];
    u8 pad20;
    u8 statusByte;
    u8 pad22[2];
    u8 cmdBytes[2][4];
    u8 status[2][4];
    u8 statusAlt[2][4];
} CardDataBlock;

/** Memory card constants. */
#define CARD_BLOCK_SIZE      0x2000   /**< 8KB per block. */
#define CARD_TOTAL_CAPACITY  0x1E000  /**< 120KB (15 blocks). */
#define CARD_OPEN_CREATE     0x200    /**< Open flag: create file. */
#define CARD_OPEN_READWRITE  1        /**< Open flag: read/write. */

/** @brief Battle OT buffer state. */
typedef struct {
    s32 pktAlloc;
    u8 pad04[0x6C];
    u32 ot[2];
    s32 pktPtr;
    u8 freeSpace[4];
} BattleOtBuf;

/** @brief Battle character render data (g_battleChars, stride 0x1D0 = 464 bytes). */
typedef struct {
    /* 0x000 */ u8 pad000[0x14];
    /* 0x014 */ u16 field014;
    /* 0x016 */ u8 pad016[6];
    /* 0x01C */ u8 field01C;
    /* 0x01D */ u8 field01D;
    /* 0x01E */ BattleCmdSlot cmdSlots[4];
    /* 0x02E */ u8 pad02E[0x54];
    /* 0x082 */ BattleMagicSlot magicSlots[32];
    /* 0x122 */ BattleItemSlot itemSlots[16];
    /* 0x172 */ u8 pad172[0x1A];
    /* 0x18C */ s32 abilityFlags;
    /* 0x190 */ s32 statusFlags;
    /* 0x194 */ u8 pad194[0x20];
    /* 0x1B4 */ u16 abilityValue;
    /* 0x1B6 */ u8 pad1B6[0x0D];
    /* 0x1C3 */ u8 characterId;
    /* 0x1C4 */ u8 pad1C4[0x0C];
} BattleCharData;

/** @brief GF battle entry (12 bytes, used for GF HP in battle). */
typedef struct {
    u8 pad00[0x0A];
    u16 hp;             /* 0x0A */
} BattleGfEntry;

/** @brief GF battle level entry (12 bytes). */
typedef struct {
    u8 level;           /* 0x00 */
    u8 pad01[3];
    u8 abilityFlags;    /* 0x04: party ability flags (used in entry 15). */
    u8 pad05[7];
} BattleLevelEntry;

/** @brief Complete battle character/GF state block. */
typedef struct {
    /* 0x000 */ BattleCharData chars[3];          /* 3 party members × 0x1D0 */
    /* 0x570 */ u8 pad570[0xA0];
    /* 0x610 */ BattleGfEntry gfEntries[1];       /* hp sub-array (stride 12, 16 entries) */
    /* 0x61C */ u8 pad61C[0x04];
    /* 0x620 */ BattleLevelEntry levelEntries[16]; /* 16 × 12 bytes */
    /* 0x6E0 */
} BattleCharState;

extern BattleCharState g_battleChars;

#endif /* BATTLE_H */
