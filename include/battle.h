#ifndef BATTLE_H
#define BATTLE_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "tim.h"
#include "battle_anim.h"  /* AnimFrame, BattleAnimEntity, BattleAnimState, g_battleAnims */

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
    u8 unk9;            /**< Bit 0 toggles the @c FieldVars.soundBankSelector at field-VM init. */
} BattleConfig;

/* AnimFrame, BattleAnimEntity, DisplayListBuf, BattleAnimState, OT_SIZE and
 * g_battleAnims now live in battle_anim.h (included above) so non-battle code
 * can use them without depending on battle.h. */

/* Tim / TimSection are the canonical PS1 TIM file structs — now in tim.h
 * (included above), shared with the world and tripletriad overlays. */

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

/** @brief Clipped rectangle result: the clipped rect + saved pre-clip position. */
typedef struct {
    RECT rect;       /* 0x00: clipped rectangle */
    s32 savedPos;    /* 0x08: packed original x|y before clipping */
} ClipResult;

/** @brief Scratch workspace for rectangle clipping operations. */
typedef struct {
    ClipResult work;
    ClipResult disp;
} ClipWork;

/** @brief Parameters for a double-blit operation with source rects and destination buffers. */
typedef struct {
    u8 pad[0x08];
    RECT srcRect1;
    RECT srcRect2;
    u8 dstData1[12];
    u8 dstData2[12];
} BlitParams;

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
    CTRL_FLAG_20    = 0x20,
    CTRL_FLAG_40    = 0x40,
    CTRL_FLAG_80    = 0x80,
    CTRL_FLAG_100   = 0x100
} ControlFlags;

/**
 * @brief Data block reached two indirections away through
 *        @c BattleEntity.linkedPtr->data.
 *
 * Size and most fields are unknown; only the byte at offset 0x14F
 * (read by @c func_800AF988) is mapped so far.
 */
typedef struct {
    u8 unk00[0xF7];
    u8 immunityFlags;   /* 0xF7: bit 0 forces status bit 0x40 clear, bit 1 forces flags bit 0x2000 clear (read by @c func_8009AFF0). */
    u8 unkF8;
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    u8 unkFC;
    u8 unkFD;
    u8 unkFE;
    u8 unkFF;
    u8 pad100[0x4];
    u8 unk104[1];       /* size unknown, probably struct, used in func_8009F65C */
    u8 pad105[0x4A];
    u8 unk14F;          /* 0x14F: byte read by func_800AF988. */
} BattleEntityData;

/**
 * @brief Handle wrapping a pointer to @c BattleEntityData.
 *
 * @c BattleEntity.linkedPtr points at one of these. The first word is
 * the actual data pointer that callers ultimately dereference.
 */
typedef struct {
    BattleEntityData *data;  /* 0x00: pointer to the entity's linked data block. */
} BattleEntityLinked;

#define ENTITY_FLAG_1 1
#define ENTITY_FLAG_4 8
typedef struct {
    union{
        s32 unk0;
        struct {
            u8 unk0;
            u8 unk1; 
            u8 unk2; 
            u8 unk3;
        } bytes;
    } stateMachine;            
     /* 0x00: 4-byte field (semantics unknown). */
    /* 0x04: state machine value. Byte 3 (offset 0x07) is also accessed
       as a "trigger type" code (read by @c func_8009A990). */
    union {
        s32 word;
        struct { u8 b0; u8 b1; u8 b2; u8 trigType; } bytes;
    } state;
    /* 0x08: byte view exposes @c trigKey (pending-trigger key matched
       against arg). Word view (@c initFlags) is a 4-byte init-time
       animation/render flag word written by @c func_800A7518. */
    union {
        struct {
            u8 trigKey;     /* 0x08 */
            u8 unk09;       /* 0x09 */
            u8 pad10[2];
        } byteView;
        s32 initFlags;        /* 0x08-0x0B as a single word. */
    } slot8;
    u8 timer;
    u8 control;
    u8 pad0E;
    u8 entityRef;
    BattleEntityLinked *linkedPtr;
    u8 pad14[0x04];
    s32 flags; /* D_800ED160 */
    s32 flagsBackup;
    s32 field20;
    s32 field24;
    s32 field28;
    s32 field2C;
    s32 unk30;
    u8 pad34[0x20];     
    u16 unk54[1];  /* used in  func_8009C598 */
    u8 unk56;
    u8 unk57;
    u16 unk58;
    u8 pad59[0xA];
    /* 0x64: byte-bit-slot view (14 halfwords, indexed by lowest set bit
       of a flag mask). The trailing 4 bytes (@c 0x7C-0x7F) are also
       read/written as a 4-byte slot flag word during init. */
    union {
        s16 perBit[14];                 /* 0x64-0x7F as 14 halfwords. */
        struct {
            s16 perBitLow[12];          /* 0x64-0x7B (12 halfwords). */
            s32 slotFlags;              /* 0x7C-0x7F as a single word. */
        } slotInit;
    } field64;
    /* 0x80: written 16-bit (mirror of @c BattleCharData.displayStatus)
       and later read 32-bit (with a bitmask test). */
    union {
        u16 slotDisplay;     /* 0x80-0x81 (write path). */
        s32 word;            /* 0x80-0x83 (read path). */
    } at0x80;
    u16 animParam1;
    u16 animParam2;
    u16 animParam3;
    u8 pad8A[0x2];
    volatile ControlFlags controlFlags;
    u16 status; /* D_800ED1D8 */
    u16 statusBackup;
    u16 hpDisplay;     /* 0x94: HP value mirrored from BattleCharData.currentHp. */
    u16 unk96;
    u8 pad98;
    u8 unk99;
    u8 pad9A[0x6];
    u8 unkA0[1];
    u8 padA1[0x16];
    u8 unkB7;
    u8 padB8[0x3];
    u8 linkedIdx2;
    u8 padBC[0xE];
    u8 unkCA;
    u8 linkedIdx;
    u8 unkCC;
    u8 fieldCD;        /* 0xCD: stat byte used in case-0 damage formula (squared). */
    u8 unkCE;
    u8 fieldCF;        /* 0xCF: stat byte averaged with arg2 in func_8009DEF0 mode-7. */
} BattleEntity; /* 208 bytes */

/**
 * @brief Battle system block at D_800ED148.
 *
 * Top-level view: 16 BattleEntity slots followed by a region of misc state
 * fields (only @c effectMult mapped here so far).
 *
 * @note @c BattleSystemFlat is an alternative view of the same memory
 *       used in @c bc_object7.c via cast. Files needing @c volatile
 *       semantics for specific accesses should use volatile casts at
 *       the access site rather than redeclaring @c D_800ED148.
 */
/** @brief 6-byte unsigned (x,y,z) triple in @c BattleSystem.unkCE4. */
typedef struct {
    u16 x;
    u16 y;
    u16 z;
} BattleVec3u;

/** @brief 20-byte action-queue entry in @c BattleSystem.entries. */
typedef struct {
    u8 unk0;       
    u8 pad01[13];
    u8 unkE;           
    u8 pad0F[1];
    u8 unk10;
    u8 pad11[3];
} BattleEntry; /*  0x14 (20 byte) */

/** @brief Linked-list node for the @c BattleSystem.taskLinks queue.
 *
 * @c fwd is the next slot index (or 0xFF for end), @c bwd is the previous
 * slot index (or 0xFF for head). Managed by @c func_8009B2A4 / @c func_8009B320. */
typedef struct {
    u8 fwd;     /* 0x00: forward link (next slot, 0xFF = tail) */
    u8 bwd;     /* 0x01: backward link (prev slot, 0xFF = head) */
    u8 unk2;    /* 0x02: cleared on free, written 0 on alloc */
    u8 unk3;    /* 0x03 */
} TaskLink; /* 0x4 */

/** @brief Task slot in @c BattleSystem.taskData (callback + timer + done flag).
 *
 * Allocated/scheduled by @c func_8009B3D0, ticked by callbacks like
 * @c func_8009AAC4, finalized/freed by @c func_8009B520. */
typedef struct {
    s32 callback;   /* 0x00: function pointer (called by @c func_8009B478). */
    u8 pad04[4];    /* 0x04 */
    u16 timer;      /* 0x08: countdown (ticked by callbacks). */
    u8 pad0A[5];    /* 0x0A */
    u8 done;        /* 0x0F: completion flag (1 = ready to free). */
} TaskEntry; /* 0x10 */

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
} Struct_12CC; /* used in func_8009D594 */

typedef struct {
    /* 0x0000 */ BattleEntity entities[7];      /**< 7 × 0xD0 = 0x5B0. Index 0 is also the header proxy. */
    /* 0x05B0 */ u8 pad5B0[0x10];               /**< Pre-control padding. */
    /* 0x05C0 */ u8 unk5C0;                     /**< Action queue head index (used by func_800B06DC). */
    /* 0x05C1 */ u8 pad5C1[0x1];
    /* 0x05C2 */ u8 unk5C2;                     /**< Misc state byte (init to 1 by func_8009A1E0/ACEC). */
    /* 0x05C3 */ u8 unk5C3;                     /**< Misc state byte (init to 1 by func_80099FE8). */
    /* 0x05C4 */ u8 unk5C4;
    /* 0x05C5 */ BattleEntry entries[90];       /**< Action queue (stride 0x14)*/
    /* 0x0CDD */ u8 padCDD[0x17];                /**< Pad to unkCE4. */
    /* 0x0CE4 */ BattleVec3u unkCE4[8];         /**< 8-entry x/y/z position table (read by @c func_8009A528). */
    /* 0x0D14 */ u8 unkD14[0x8];                /**< Hit-type byte table (8 entries). */
    /* 0x0D1C */ u8 padD1C[0x40];               /**< Misc state. */
    /* 0x0D5C */ u8 unkD5C[0x8];                /**< Per-trigger flag array (8 entries). */
    /* 0x0D64 */ u8 padD64[0x39F];              /**< Misc state. */
    /* 0x1103 */ TaskLink taskLinks[16];        /**< Task queue link table (16 × 4 bytes). */
    /* 0x1143 */ u8 pad1143[1];                 /**< Pad to taskData. */
    /* 0x1144 */ TaskEntry taskData[16];        /**< Task queue data slots (16 × 16 bytes). */
    /* 0x1244 */ u8 pad1244[0x48];              /**< Pad to unk128C. */
    /* 0x128C */ s32 unk128C;                   /**< Cached userData for callback. */
    /* 0x1290 */ s16 unk1290;
    /* 0x1292 */ s16 unk1292;
    /* 0x1294 */ s16 unk1294;
    /* 0x1296 */ u8 pad1296[0x36];
    /* 0x12CC */ Struct_12CC array12CC[1];        /* used in func_8009D594 */
    /* 0x12CF */ u8 pad12CF[0x9];
    /* 0x12D8 */ s32 unk12D8;                   /**< Cached length argument for callback. */
    /* 0x12DC */ u8 pad12DC[0x4];               /**< Misc state. */
    /* 0x12E0 */ s16 unk12E0;                   /**< Low 13 bits of a packed s16 field. */
    /* 0x12E2 */ u8 pad12E2[0x6];               /**< Misc state. */
    /* 0x12E8 */ u8 unk12E8;                    /**< Misc state byte. */
    /* 0x12E9 */ u8 unk12E9;                    /**< Misc state byte (touched by 12EA-gated path). */
    /* 0x12EA */ u8 unk12EA;                    /**< Misc state gate byte. */
    /* 0x12EB */ u8 pad12EB[0x1];               /**< Misc state. */
    /* 0x12EC */ u8 unk12EC;                    /**< Misc state byte (init to 0xFF). */
    /* 0x12ED */ u8 unk12ED;                    /**< Misc state byte. */
    /* 0x12EE */ u8 unk12EE;                    /**< Misc state byte. */
    /* 0x12EF */ u8 pad12EF[0x4];               /**< Misc state. */
    /* 0x12F3 */ u8 unk12F3;                    /* used as index in func_8009F824 */
    /* 0x12F4 */ u8 pad12F4;                    /**< Misc state. */
    /* 0x12F5 */ u8 unk12F5;                    /**< Misc state. */
    /* 0x12F6 */ u8 taskHead;                   /**< Head index of the task queue linked list. */
    /* 0x12F7 */ u8 pad12F7[0x1];               /**< Pad. */
    /* 0x12F8 */ u8 unk12F8;                    /**< Misc state counter. */
    /* 0x12F9 */ u8 unk12F9;                    /**< Misc state gate byte. */
    /* 0x12FA */ u8 pad12FA[0x3];               /**< Misc state. */
    /* 0x12FD */ u8 unk12FD;                    /**< Misc state byte. */
    /* 0x12FE */ u8 unk12FE;
    /* 0x12FF */ u8 unk12FF;
    /* 0x1300 */ u8 unk1300;
    /* 0x1301 */ u8 unk1301;
    /* 0x1302 */ u8 unk1302;
    /* 0x1303 */ u8 unk1303;
    /* 0x1304 */ u8 unk1304;
    /* 0x1305 */ u8 unk1305;
    /* 0x1306 */ u8 unk1306;
    /* 0x1307 */ u8 unk1307;
    /* 0x1308 */ u8 unk1308;
    /* 0x1309 */ u8 unk1309;
    /* 0x130A */ u8 unk130A;
    /* 0x130B */ u8 unk130B;
    /* 0x130C */ u8 unk130C;
    /* 0x130D */ u8 unk130D;
    /* 0x130E */ u8 unk130E;
    /* 0x130F */ s8 unk130F;                    /**< Upper 3 bits of a packed s16 field (sign-extended). */
    /* 0x1310 */ u8 unk1310;
    /* 0x1311 */ u8 actionType;                 /**< Queued action type (0=none, 1=stat-up message). */
    /* 0x1312 */ u8 actionByte0;                /**< Queued action arg 0 (stat ID for type 1). */
    /* 0x1313 */ u8 actionByte1;                /**< Queued action arg 1 (count for type 1). */
    /* 0x1314 */ u8 unk1314;  
    /* 0x1315 */ u8 unk1315;  
    /* 0x1316 */ u8 unk1316;
    /* 0x1317 */ u8 unk1317;
    /* 0x1318 */ u8 unk1318;
    /* 0x1319 */ u8 unk1319;                    /**< Misc state byte (init to 0xFF). */
    /* 0x131A */ u8 pad131A[0x8];               /**< More misc state. */
    /* 0x1322 */ u8 unk1322;
    /* 0x1323 */ u8 effectMult;                 /**< Damage/effect multiplier (percent). */
    /* 0x1324 */ u8 unk1324; 
    /* 0x1325 */ u8 unk1325; 
    /* 0x1326 */ u8 unk1326; 
    /* 0x1327 */ u8 unk1327;
    /* 0x1328 */ u8 unk1328; 
    /* 0x1329 */ u8 unk1329;
} BattleSystem; /* 0x132A */

/** @brief 5-byte slot in @c BattleAnimTable.animSlots. */
typedef struct {
    u8 id;          /* 0x00: lookup key / command byte. */
    s8 value;       /* 0x01: signed value byte. */
    u8 unk2[3];     /* 0x02..0x04: unknown. */
} BattleAnimSlot;

/** @brief 0x47-byte sub-entry in @c BattleAnimTable.subEntries. */
typedef struct {
    u8 charId;          /* 0x00: char/scene byte, mirrored from g_gameState[+0xAF4..]. */
    u8 pad01[0x42];     /* 0x01..0x42: padding. */
    u8 unk43;
    u8 pad02[0x3];     /* 0x44..0x46: padding. */
} BattleAnimSubEntry;   /* 0x47 */

/** @brief Battle anim/scene lookup table at @c D_800EE9E8. */
typedef struct {
    /* 0x000 */ BattleAnimSlot animSlots[32];      /**< 32 × 5 = 0xA0 bytes. */
    /* 0x0A0 */ u8 padA0[3];                       /**< 0xA0..0xA2 unknown. */
    /* 0x0A3 */ BattleAnimSubEntry subEntries[3];  /**< 3 × 0x47 = 0xD5 bytes. */
} BattleAnimTable;

extern BattleAnimTable D_800EE9E8;

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
    u8 cmdType;
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
    /* 0x016 */ u8 pad016[2];
    /* 0x018 */ u16 currentHp;          /**< Current HP in battle. */
    /* 0x01A */ u8 pad01A[2];
    /* 0x01C */ u8 field01C;
    /* 0x01D */ u8 field01D;
    /* 0x01E */ BattleCmdSlot cmdSlots[4];
    /* 0x02E */ u8 pad02E[0x54];
    /* 0x082 */ BattleMagicSlot magicSlots[32];
    /* 0x122 */ BattleItemSlot itemSlots[16];
    /* 0x172 */ s16 field172;          /**< Mirrored HP cap (set with hpRegenCap when battle HP is reduced). */
    /* 0x174 */ s16 hpRegenCap;        /**< HP regen cap (field-walk tick stops when currentHp reaches this). */
    /* 0x176 */ u8 pad176[0x06];
    /* 0x17C */ s32 xpToNext;          /**< XP needed to reach next level. */
    /* 0x180 */ u8 pad180[0x08];
    /* 0x188 */ s32 field188;          /**< Status/ability mask checked for bit 0x60000. */
    /* 0x18C */ s32 abilityFlags;
    /* 0x190 */ s32 statusFlags;
    /* 0x194 */ u16 elemResistances[8];/**< Element resistance values (8 × s16). */
    /* 0x1A4 */ u8 statusResistances[13];/**< Status resistance values (13 × u8). */
    /* 0x1B1 */ u8 pad1B1;
    /* 0x1B2 */ u16 displayStatus;       /**< Mirror of BattleEntity.status; bit-flag display state. */
    /* 0x1B4 */ u16 abilityValue;
    /* 0x1B6 */ u16 atkStatusHit;      /**< Attack status hit chance. */
    /* 0x1B8 */ u8 level;              /**< Battle level (from findCharXpLevel). */
    /* 0x1B9 */ u8 pad1B9;
    /* 0x1BA */ u8 classId;            /**< Entity-class index into the @c D_80078E00 ability tables (stride 12 at @c 0x35C1). */
    /* 0x1BB */ u8 stats[8];           /**< Battle stats: STR, VIT, MAG, SPR, SPD, ?, hit (0x1C0), eva (0x1C1). 0x1C2 = ? */
    /* 0x1C3 */ u8 characterId;
    /* 0x1C4 */ u8 atkElemBase;        /**< Attack element base. */
    /* 0x1C5 */ u8 atkElemBonus;       /**< Attack element bonus. */
    /* 0x1C6 */ u8 fieldStatusByte;    /**< Status byte checked by field script (bit 1 = greyed out). */
    /* 0x1C7 */ u8 statCoefs[9];       /**< Stat coefficient table (HP, str, vit, mag, spr, spd, ?, eva, hit). */
    /* 0x1D0 */
} BattleCharData;

/** @brief GF battle entry (12 bytes, used for GF HP in battle). */
typedef struct {
    u8 pad00[0x08];
    u16 maxHp;          /* 0x08: max HP cap (used to restore hp on revive) */
    u16 hp;             /* 0x0A: current HP */
} BattleGfEntry;

/** @brief GF battle level entry (12 bytes). */
typedef struct {
    u8 level;           /* 0x00 */
    u8 unk1;
    u8 pad2[2];
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
} BattleCharState;/* 0x6E0 */

extern BattleCharState g_battleChars;

/**
 * @brief Battle/scene context struct pointed to by D_800D244C.
 *
 * The @c primList array at +0x70 holds per-bone OT chain heads (indexed
 * by bone id via @c D_800C53B8 in we_object4). Two specific slots are
 * also accessed by name:
 *  - @c primList[1] (+0x74) is the @c colorTag consumed by
 *    @c renderBattleDisplayList
 *  - @c primList[3] (+0x7C) is the main @c otHead chain head used by
 *    @c addPrim-style inserts.
 *
 * At least 0x4070 bytes based on observed accesses in world.
 */
typedef struct {
    /* 0x0000 */ DRAWENV drawEnv;       /**< Draw-env template, copied to the active env. */
    /* 0x005C */ DISPENV disp;          /**< Display-env template; copied to D_80082C18 in setupWorldRender. */
    /* 0x0070 */ s32 primList[4];
    /* 0x0080 */ u8  pad0080[0x3FF0];
} BattleSceneCtx;                       /* 0x4070 */

/* Named aliases for the two specifically-purposed primList slots. */
#define BSC_COLORTAG_IDX 1   /**< primList[1] @ +0x74 — renderBattleDisplayList color tag. */
#define BSC_OTHEAD_IDX   3   /**< primList[3] @ +0x7C — main addPrim chain head. */

extern BattleSceneCtx *D_800D244C;

/**
 * @brief Spell record in the battle scene buffer (D_80078E00.spells, stride 60).
 *
 * Only byte 0 (magicId) is read by the known callers.
 */
typedef struct {
    u8 pad[2];
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 pad2; 
    u8 magicId; /**< [0x4 (0x226)] Magic/spell ID byte (input to ability flag funcs) */
    u8 unk7;
    u8 pad3[2];
    u8 unkA;
    u8 pad4;
    u32 unkC;
    u16 unk10;
    u8 unk12;
    u8 pad5[41];
} BattleSpellRow; /* 60 bytes */

/**
 * @brief Ability record in the battle scene buffer (D_80078E00.abilities, stride 24).
 *
 * Only byte 0 (abilityId) is read by the known callers.
 */
typedef struct {    
    u8 pad;              // 0x00
    u8 abilityId;        /**< [0x01] Ability ID byte (input to ability flag funcs) */
    u8 unk2;
    u8 unk3;
    u8 pad1;           // 0x02..0x04 
    u8 unk5;             // 0x05
    u16 unk09;           // 0x06..0x07
    u32 unkA;            // 0x08..0x0B
    u8 unkC;             // 0x0C
    u8 pad3[2];          // 0x0D..0x0E
    u8 unk11;            // 0x0F
    u8 pad4[2];          // 0x10..0x16
    u8 unk13;
    u8 pad14;
    u8 unk15;
    u8 unk16;
    u8 val;              /**< [0x17] used in func_8009BAC4 */
    u8 unk18;             // 0x18
} BattleAbilityRow; /* 24 bytes */

/**
 * @brief 20-byte scene entry indexed by funcs that pass entry.lookupId
 *        plus a sibling word in @c BattleSceneData to @c resolveKernelPtr.
 */
typedef struct {
    u16 lookupId;       /**< 0x00: u16 passed to resolveKernelPtr. */
    u8 pad[3];
    u8 unk5;
    u8 unk6;   
    u8 unk7;
    u8 unk8;
    u8 pad2;
    u8 unkA;         
    u8 unkB;         
    u8 unkC;       
    u8 unkD;          
    u16 unkE;
    u32 unk10;         
} BattleSceneEntry;     /* 20 bytes */

typedef struct {
    u16 lookupId;       /**< 0x00: u16 passed to resolveKernelPtr. */    
    u8 pad[2];
    u8 unk4;
    u8 unk5;
    u8 unk6;           /**< 0x06 */
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 pad2;
    u8 unkB;           /**< 0x0B */
    u32 unkC;           /**< 0x0C */
    u16 unk10;           /**< 0x10 */
    u8 unk12;
    u8 unk13;           /**< 0x13 */
} BattleSceneEntry2;     /* 20 bytes */


/**
 * @brief 132-byte scene row indexed by func_800AFF70 (a0 offset by 0x40).
 */
typedef struct {
    u16 lookupId;       /**< 0x00: u16 passed to resolveKernelPtr. */
    u8 pad2[4];
    u8 unk4;
    u8 unk5;
    u8 pad3[2];
    u8 unk8;
    u8 unk9;
    u8 pad4;
    u8 unkD;
    u16 unkE;
    u32 unk10;
    u8 pad11[7];
    u8 unk1B;
    u8 pad1C[102];
    u8 unk83;
    u8 unk84;
} BattleSceneRow;       /* 132 bytes */

/**
 * @brief 8-byte scene row indexed by func_800B0360.
 */
typedef struct {
    u16 lookupId;       /**< 0x00: u16 passed to resolveKernelPtr. */
    u8 unk02[6];        /**< 0x02..0x07: unknown. */
} BattleSceneRow8;      /* 8 bytes */


/**
 * this struct must be 0xC size, flags and maxHP must be accessed 
 * at address 0x4C16 and 0x4C17 (func_8009F168) 
 */

typedef struct {
    u16 lookupId;   /* 0x4C0C */
    u8 unk3;        /* 0x4C0E */
    u8 unk4;        /* 0x4C0F */
    u32 unk5;
    u16 unk8;
    u8 flags;       /* 0x4C16 */
    u8 maxHP;       /* 0x4C17 */
} Struct_4C0C;     /* 12 bytes */

typedef struct {
    u8 pad[2];
    u8 unk48BE;
    u8 unk48BF;
    u8 unk48C0;
    u8 pad2[2];
    u8 unk48C3;
    u8 pad3;
    u8 unk48C5;
    u8 unk48C6;
    u8 unk48C7;
    u8 pad4[10];
    u16 unk48D2;
    u32 unk48D4;
    u8 pad5[4];
} Struct_48BC;    /* 32 bytes */

typedef struct {
    u8 pad[2];
    u8 unk09;
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 unk0;
    u8 unk1;
    u16 unk2;
    u32 unk4;
} Struct_4020;    /* 16 bytes */

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 pad[2];
    u8 unkunk;
    u8 pad2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u16 unk6;
    u32 unk8;
    u8 pad3[4];
} Struct_4A70;    /* 20 bytes */


typedef struct {
    s32 unk0;    
    u16 unk4;        
    u8 unk6;    
    u8 unk7;       
} Struct_45F8;      /* 8 bytes */

typedef struct {
    u8 pad[2];
    u8 var;
    u8 var1;
    u8 var2;
    u8 pad1[2];
    u8 var3;
    u8 pad2;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u16 unk4;
    u8 unk6;
    u8 unk7;
    u32 unk8;
    u8 pad3[4];
} Struct_446C; /* 24 bytes */


typedef struct {
    u8 pad;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u16 unk4;
    u16 unk6;
    u32 unk8;
    u8 pad1[10];
    u8 unk17;
    u8 unk18;
} Struct_3750; /* 24 bytes */

typedef struct {
    u8 unk1;
    u8 unk2;
    u8 pad[3];
    u8 unk0;
    u8 pad2[6];    
} Struct_35BD; /* 12 bytes */


typedef struct {
    u8 unk0;
    u8 pad[35];      
} Struct_37A9; /* 36 bytes */


/**
 * @brief Battle scene data buffer at D_80078E00 (loaded from disc, ~0x9E08 bytes).
 *
 * Contains kernel-data pointers and various sub-arrays. Many regions remain
 * unidentified — fields will be added as more code is decompiled.
 */
typedef struct {
    /* 0x0000 */ u8 pad0000[0x88 - 0x00];
    /* 0x0088 */ s32 rows132Arg;                /**< resolveKernelPtr arg paired with rows132[]. */
    /* 0x008C */ s32 entries17Arg;              /**< resolveKernelPtr arg paired with entries17[]. */
    /* 0x0090 */ u8 pad0090[0xA4 - 0x90];
    /* 0x00A4 */ s32 entriesA0Arg;              /**< resolveKernelPtr arg paired with entriesA0[]. */
    /* 0x00A8 */ u8 pad00A8[0xD4 - 0xA8];
    /* 0x00D4 */ s32 rows8Arg;                  /**< resolveKernelPtr arg paired with rows8[]. */
    /* 0x00D8 */ u8 pad00D8[0xDC - 0xD8];                
    /* 0x00DC */ s32 unk4C0CArg;                /**< resolveKernelPtr arg paired with unk4C0C[]. */
    /* 0x00E0 */ u8 pad00E0[0x220 - 0xE0]; 
    /* 0x0220 */ BattleSpellRow spells[1];      /**< 60-byte stride (size unknown, index past). */
    /* 0x025C */ u8 pad025C[0x0F78 - 0x025C];
    /* 0x0F78 */ BattleSceneRow rows132[1];     /**< 132-byte stride (size unknown, index past). */
    /* 0x0FFC */ u8 pad0FFC[0x17B8 - 0xFFC];
    /* 0x17B8 */ BattleSceneEntry entries17[1]; /**< stride 20 (size unknown, index past). */
    /* 0x17CC */ u8 pad17CC[0x35BD - 0x17CC];
    /* 0x35BD */ Struct_35BD array35BD[1];
    /* 0x35C9 */ u8 pad35C9[0x3738 - 0x35C9];
    /* 0x3738 */ BattleAbilityRow unk3738[1];
    /* 0x3750 */ Struct_3750 array3750[1];
    /* 0x3768 */ u8 pad3768[0x37A9 - 0x3768];
    /* 0x37A9 */ Struct_37A9 array37A9[1];
    /* 0x37CD */ u8 pad37CD[0x3920 - 0x37CD];
    /* 0x3920 */ Struct_3750 array3920[1];
    /* 0x3938 */ BattleAbilityRow abilities[1]; /**< 24-byte stride (size unknown, index past). */
    /* 0x3950 */ u8 pad3950[0x3EE0 - 0x3950];
    /* 0x3EE0 */ BattleSceneEntry2 entriesA0[1]; /**< stride 20 (size unknown, index past). */
    /* 0x3EF4 */ u8 pad3EF4[0x4020 - 0x3EF4];
    /* 0x4020 */ Struct_4020 array4020[1];
    /* 0x4030 */ u8 pad4034[0x4484 - 0x4030];
    /* 0x4484 */ Struct_446C array4484[1];
    /* 0x449C */ u8 pad449C[0x44FC - 0x449C];
    /* 0x44FC */ Struct_4020 array44FC[1];
    /* 0x450C */ u8 pad4510[0x45F8 - 0x450C];
    /* 0x45F8 */ Struct_45F8 array45F8[1];      /**< stride 8 */
    /* 0x4600 */ u8 pad4600[0x47FC - 0x4600];
    /* 0x47FC */ Struct_446C array47FC[1];
    /* 0x4814 */ u8 pad4814[0x48BC - 0x4814];
    /* 0x48BC */ Struct_48BC array48BC[1];
    /* 0x48DC */ u8 pad48DC[0x4A5E - 0x48DC];
    /* 0x4A5E */ BattleSceneRow8 rows8[1];      /**< stride 8 (size unknown, index past). */
    /* 0x4A66 */ u8 pad4A66[0x4A70 - 0x4A66];
    /* 0x4A70 */ Struct_4A70 array4A70[1];
    /* 0x4A84 */ u8 pad4A84[0x4C0C - 0x4A84];
    /* 0x4C0C */ Struct_4C0C unk4C0C[1];
    /* 0x4C18 */ u8 pad4C18[180];
    /* 0x4CCC */ u8 unk_4CCC[14];
} BattleSceneData;

extern BattleSceneData D_80078E00;

/** @brief Top-level battle config block (g_battleConfig at 0x...). */
extern BattleConfig g_battleConfig;

/** @brief The battle system block at @c 0x800ED148. */
extern BattleSystem D_800ED148;

/** @brief Battle slot data block at @c D_800ED158 (alias for D_800ED148+0x10). */
typedef struct {
    BattleEntity slots[7];
    u8 pad5B0[0x754];
    u8 unkD04[8];
} BattleSlotData;

extern BattleSlotData D_800ED158;

/** @brief Sound-command queue slot returned by @c func_8009B134.
 *
 * The two parameter bytes at +2/+3 are sometimes written as a single u16
 * and sometimes as two separate u8s (callers vary by command id), so they
 * are exposed via U16Split to keep both views available. */
typedef struct {
    u16 unk0;
    U16Split unk2;
} SoundCmd;

/* ---------------------------------------------------------------- *
 *  Battle data symbols (battle overlay region).
 * ---------------------------------------------------------------- */

extern s16 D_8005F11C;      /**< 0x8005F11C: misc sound state (used by bc_object1 / fe_object7). */
extern u16 D_80082C0A;      /**< 0x80082C0A: input/effect flag word; bit 1 gates timed sound, bit 2 gates vibrate. */
extern u8  D_80082C0F;      /**< 0x80082C0F: deferred trigger gate byte (non-zero suppresses processing). */
extern u8 D_800ED157[];     /**< 0x800ED157: misc battle state. */
extern u8 D_800ED1D8[];     /**< 0x800ED1D8: misc battle state. */
extern BattleEntry D_800ED70C[];     /**< 0x800ED70C: entity status table (stride 20). */
extern s32 D_800E19BC[];    /**< 0x800E19BC: CdRead (sector,length) pair table. */
extern u8 D_800E19B4[];     /**< 0x800E19B4: misc state byte. */
/** @brief 4-byte (x,z) position pair used by @c func_8009A74C battle slot layout tables. */
typedef struct {
    u16 x;
    u16 z;
} BattlePosXZ;

extern u16 D_800E3CA4[];          /**< 0x800E3CA4: solo-slot (x,z) position (single entry, 2 halfwords). */
extern BattlePosXZ D_800E3CA8[];  /**< 0x800E3CA8: 2-active-slot layout (2 entries). */
extern BattlePosXZ D_800E3CB0[];  /**< 0x800E3CB0: 3-active-slot layout (3 entries). */
extern u8 D_800EDE24[];     /**< 0x800EDE24: misc state. */
extern u8 D_800EE24B[];     /**< 0x800EE24B: misc state byte. */
extern u8 D_800EE28C[];     /**< 0x800EE28C: misc state. */
extern u8 D_800EE449[];     /**< 0x800EE449: misc state byte. */
extern u8 D_800EE456;     /**< 0x800EE456: status flags byte. */
extern u8 D_800EE45D;
extern u8 D_800EE46E;
extern u8 D_800EE476;     /**< 0x800EE476: entity index latch. */

/**
 * @brief Battle command queue / scratch buffer at @c 0x800EE4C0.
 *
 * Used by the bc_object2 / bc_object4 / bc_object8 paths to stage
 * incoming command bytes (@c unk00 / @c unk01) plus flag state (the
 * @c flags5 / @c flags6 byte pair) and a couple of derived values
 * (@c unk0C, @c statusCode). Fields with @c unkXX names have known
 * offsets but unconfirmed semantics; @c padNN regions cover bytes
 * that haven't been mapped yet.
 */
typedef struct {
    /* 0x00 */ u8 unk0;         /**< Command byte 0 (copied from status[0] during init). */
    /* 0x01 */ u8 unk1;         /**< Command byte 1 (copied from status[1] during init). */
    /* 0x02 */ u8 unk2;         
    /* 0x03 */ u8 unk3;         /**< used in func_8009BBD0*/
    /* 0x04 */ u8 unk4;
    /* 0x05 */ u8 flags5;        /**< Flag byte; bits 0x01 and 0x20 are set by various paths. */
    /* 0x06 */ u8 flags6;        /**< Flag byte; bits 0x01/0x02/0x04/0x10 mark command-completion states. */
    /* 0x07 */ u8 unk7;
    /* 0x08 */ u8 unk08;
    /* 0x08 */ u8 unk09;
    /* 0x0A */ u8 unkA;    
    /* 0x0B */ u8 unkB;
    /* 0x0C */ u32 unk0C;        /**< Scaled by 3/2 when the active entity has controlFlag bit 0x20. */
    /* 0x10 */ u32 unk10;
    /* 0x14 */ u32 unk14;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u16 statusCode;   /**< Status/command code; compared against 0x49 in func_8009D68C. */
    /* 0x1E */ u16 unk1E;
    /* 0x20 */ u16 unk20;
    /* 0x21 */ u8 pad21[0x1E];
} BattleCmdBuf;     /* 64 bytes */

extern BattleCmdBuf D_800EE4C0; /**< 0x800EE4C0: command queue buffer. */
extern u8 D_800EE4C1;     /**< 0x800EE4C1: misc state byte. */
extern u8 D_800EEBA8[];     /**< 0x800EEBA8: misc state. */
extern u8 D_80098030[];
extern u8 D_800EEBB0;     /**< 0x800EEBB0: misc state. */
extern u8 D_800EEBB8;     /**< 0x800EEBB8: misc state byte. */
extern u8 D_800EEBB9;     /**< 0x800EEBB9: misc state byte. */
extern u8 D_800EEBBA;     /**< 0x800EEBBA: misc state byte. */
extern u8 D_800EEBBB;     /**< 0x800EEBBB: misc state byte. */
extern u8 D_800EEBBC;     /**< 0x800EEBBC: stat clamp threshold. */
extern u8 D_800EEBC0;
extern u8 D_800EEBBF;
extern u16 D_800EEBC2;      /**< 0x800EEBC2: status code halfword. */
extern s32 D_800EEBC4;      /**< 0x800EEBC4: status flags word (bit 0x4000000). */
extern u8  D_800EE470;      /*   0x800EE470: used in func_8009F040*/
extern u8  D_800EE471;
extern u8  D_800EE4C4;
extern u16 D_800EE4DC;
extern s32 D_800EE4CC;
extern u8  D_800EE469;
extern u8  D_800EE466;
extern u8  D_800EE461;
extern s8  D_800EE448;
extern s8  D_800EE43D;
extern s16 D_800EE3DC;
extern u8  D_800EEBBD;
extern u8  D_800EEBBE;
extern u16 D_80077E5C;
/* ---------------------------------------------------------------- *
 *  Battle-overlay function prototypes (battle internals).
 * ---------------------------------------------------------------- */

/** @brief Test if bit @p a1 is set in mask @p a0. */
s32 func_8009A514(s32 a0, s32 a1);

/** @brief Set the battle round timer based on the speed setting. */
void func_8009AD7C(void);

/** @brief Dispatch a battle transition command (codes 5..10). */
void func_8009AE08(s32 cmd);

/** @brief Schedule a callback function as a battle task (sound id @c 0xA). */
void func_8009AF14(void *callback);

/** @brief Queue a custom sound command (id @c 8) with entity / flag params. */
void func_8009AF3C(s32 a0, s32 a1, s32 a2, s32 a3, s32 stack_arg);

/** @brief Allocate a slot in the sound-command queue; caller fills the buffer. */
SoundCmd *func_8009B134(s32 cmd, s32 vol, s32 entry);

/** @brief Get the next random value from the shuffle buffer. */
s32 func_8009B15C(void);

/** @brief Remove a task-link slot and relink neighbours. */
void func_8009B320(s32 a0, u8 *a1, u8 *a2);

/** @brief Allocate a task queue slot and store the callback pointer. */
s32 func_8009B3D0(void *callback);

/** @brief Issue a CD or memory read for a sound bank entry, arming the completion callback. */
void func_8009B5C4(s32 idx, s32 dst, s32 dir, s32 userData);

/** @brief Yield execution (second wrapper for @c func_800393C8). */
void func_8009B6B0(void);

/** @brief Conditional add with overflow flag (clamped sum + carry). */
s32 func_8009B79C(s32 a0, s32 a1);

/** @brief Look up cached entity status byte. */
s32 func_8009B7BC(s32 entityIdx);

/** @brief Apply masked status update to an entity slot. */
void func_8009B924(s32 a0, s32 a1, s32 a2);

/** @brief Resolve a battle scene entry pointer by index. */
void func_800A09D0(u32 idx);

/** @brief Count bits in @p mask, writing each bit's index into @p dst. Returns count. */
s32 func_800A4FC4(s32 mask, u8 *dst);

/** @brief Process a queued damage event for the entity. */
void func_800A5210(s32 entityIdx);

/** @brief Apply a status flag, ORing it into the flag word. */
void func_800B0574(s32 arg0, u32 arg1);

/** @brief Set @c field64[lowest-bit-of-a1] = -0x457 on entity @p a0. */
void func_800B0600(s32 a0, s32 a1);

/** @brief Look up auxiliary ability flags by stat byte (low bits). */
s32 func_800B0F7C(s32 stat);

/** @brief Look up auxiliary ability flags by stat byte (high bits). */
s32 func_800B0F9C(s32 stat);

/** @brief Resolve battle scene context pointer. */
void func_800A1760(s32 arg0, BattleCharData* arg1);

/** @brief Apply a stat-effect probe; outputs (a1=stat, a2=count). */
s32 func_800AF134(s32 entityIdx, u8 *outStat, u8 *outCount, s32 typeByte);

/** @brief Format helper that writes into a caller-provided buffer. */
u8 *func_800B04A0(s32 a0, u8 *buf);

/** @brief Concatenate two parts into the @c D_800EEBE8 message buffer. */
u8 *func_800B0248(s32 part1, s32 sepByte, s32 part2);

/** @brief Finalize the @c D_800EEBE8 message buffer. */
u8 *func_800B02AC(u8 *buf);

/** @brief Store a u32 value to @c D_800EE424. */
void func_800A4320(s32 value);

/** @brief Store @c getMenuString(id) result into @c D_800EE424. */
void func_800A432C(s32 stringId);

/** @brief Animated 3D particle/effect entry processed by @c bc_object16.c. */
typedef struct {
    /* 0x00 */ u8  pad00[0xC];
    /* 0x0C */ u16 frame;            /**< Frame counter, increments each tick. */
    /* 0x0E */ s16 delay;            /**< Wait counter; skip render until 0 (used by @c func_800CDF3C). */
    /* 0x10 */ s16 posX;             /**< Translation X. */
    /* 0x12 */ s16 posY;             /**< Translation Y. */
    /* 0x14 */ s16 posZ;             /**< Translation Z. */
    /* 0x16 */ u16 cmdWord;          /**< Per-particle prim cmd word (used by @c func_800CDF3C). */
    /* 0x18 */ u16 angle;            /**< Y rotation angle. */
    /* 0x1A */ u16 angVel;           /**< Angular velocity (decays by >>4 each tick). */
    /* 0x1C */ s16 sizeX;            /**< Scale X (also reused as Z). */
    /* 0x1E */ s16 sizeXVel;         /**< Scale X velocity. */
    /* 0x20 */ s16 sizeY;            /**< Scale Y. */
    /* 0x22 */ s16 sizeYVel;         /**< Scale Y velocity (decays by >>3 each tick). */
} ParticleEntry;

/**
 * @brief 0x58-byte primitive packet built by @c func_800CD35C and processed
 *        by @c func_800CBC68.
 *
 * @c func_800CBC68 dispatches through @c dispatch (an offset list) and feeds
 * the BG color triple at @c bgR/bgG/bgB into the GTE BG color registers
 * (RBK/GBK/BBK at COP2 $21/$22/$23) after a @c <<4 scale. Most of the
 * remaining bytes are still unmapped.
 */
typedef struct {
    /* 0x00 */ s32 *dispatch;        /**< Pointer to dispatch list (handler addr at @c [0]). */
    /* 0x04 */ u8  pad04[0x4];       /**< Tail pointer (set to dispatch+8 in some flag paths). */
    /* 0x08 */ u8  bgR;              /**< GTE background red (loaded into RBK after @c <<4). */
    /* 0x09 */ u8  bgG;              /**< GTE background green (loaded into GBK). */
    /* 0x0A */ u8  bgB;              /**< GTE background blue (loaded into BBK). */
    /* 0x0B */ u8  pad0B;
    /* 0x0C */ s32 depth;            /**< Depth/sort key. */
    /* 0x10 */ u8  pad10[0x4];
    /* 0x14 */ s32 cmd;              /**< Packet command word (set to @c 0x3867 here). */
    /* 0x18 */ u8  pad18[0x4];       /**< Cleared when flag bit @c 0x1000 unset. */
    /* 0x1C */ s32 flags;            /**< Attribute/flag word (bits @c 0x1000 / @c 0x2000 read by @c func_800CBC68). */
    /* 0x20 */ u8  pad20[0x38];      /**< Working pointer + remaining unmapped fields. */
} EffectPrim; /* 0x58 */

/** @brief Battle slot stat/AI hook called by @c func_800A7518. */
void func_800A554C(s32 idx);

/** @brief Battle slot fallback handler called by @c func_800A7518 when
 *         @c charData->statusFlags bit @c 0x10000 is not set. */
void func_800A559C(s32 idx);

/* --- Battle animation lifecycle --- */
extern void activateBattleAnim(s32 idx);

/* --- Spatial / matrix helpers (defined in field overlay) --- */
extern void func_800406A4(u8 *p);
extern void func_80040734(u8 *p);
extern s32  func_80040DE4(SVECTOR *v, s32 *sxy, s32 *p, s32 *flag);

/** @brief Reset battle-transition state (clears @c btl_color flags). */
extern void initBattleTransition(void);
extern void func_800D0608(void); /* bc_object17: overlay VSync handler (RENDER_OVERLAY) */

#endif /* BATTLE_H */
