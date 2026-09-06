#ifndef BATTLE_H
#define BATTLE_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "tim.h"
#include "battle_anim.h"

/** @brief Battle result values (BattleConfig.result). */
#define BATTLE_RESULT_UNDETERMINED  0
#define BATTLE_RESULT_GAMEOVER      1
#define BATTLE_RESULT_ESCAPED       2
#define BATTLE_RESULT_WIN           4


/** @brief Battle command config (g_battleConfig). */
typedef struct {
    u16 battleSceneId;
    u16 unk2;            // Flags?, when first bit is set, escape it not possible
    u8  unk4[3];         // Post battle command queue?
    u8  result;          /**< Battle result (BATTLE_RESULT_*). */
    u8  unk8;
    u8  unk9;            /**< Bit 0 toggles the @c FieldVars.soundBankSelector at field-VM init. */
} BattleConfig;

/* AnimFrame, BattleAnimEntity, DisplayListBuf, BattleAnimState, OT_SIZE and
 * g_battleAnims now live in battle_anim.h (included above) so non-battle code
 * can use them without depending on battle.h. */

/* Tim / TimSection are the canonical PS1 TIM file structs — now in tim.h
 * (included above), shared with the world and tripletriad overlays. */

struct BattleDisplayEntity;
typedef void (*EntityCallback)(struct BattleDisplayEntity *);

typedef struct BattleDisplayEntity {
    EntityCallback callback; /* update function pointer */
    s32 unk4;
    RECT boundRect;
    RECT dispRect;
    u8 pad18[0x18];
    s32 drawMode;
    u8 activeFlag;
    u8 unk35;
    u8 unk36;
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
    u8 pad0[8];
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
    u8 pad0[3];
    u8 counter;
    u32 color1;         /* flash color (processed) */
    u32 color2;         /* flash color (output) */
    s8 activeFlag;
    u8 padD[7];
    s8 counters[4];     /* per-channel auto-repeat countdown (func_8002CECC) */
    u16 stored[4];      /* per-channel latched edge bits (func_8002CECC) */
} SfxGlobalState;       /* 0x20 */

/** @brief Complete SFX system: 8 entry slots + global state + message display values. */
typedef struct {
    SfxEntry entries[8];       /* 8 × 60 = 480 bytes */
    SfxGlobalState state;      /* global SFX state (0x20 bytes) */
    u32 msgValues[8];          /* numeric values formatted by decodeMessage */
} SfxSystem;

/** @brief Message formatting config (D_80083858). */
typedef struct {
    u8 digitBase;              /* base character code for digit rendering */
    u8 pad01[0xF];             /* padding */
    u8 separator;              /* thousands separator character */
} MsgFormatConfig;



/** @brief Message state struct passed to decode/advance functions.
 *
 * Accessed as s32[] array in some functions, with a u8 skip count at +0x22.
 * Fields at +0x00..+0x07 are unknown; +0x08 is the stream pointer,
 * +0x0C is the stored/output pointer.
 */
typedef struct {
    s32 unk0;
    s32 unk4;
    s32 streamPtr;              /* current stream pointer (a0[2]) */
    s32 storedPtr;              /* stored/output pointer (a0[3]) */
    u8 pad10[0x12];
    u8 skipCount;               /* number of control codes to skip */
    u8 pad23;
} MsgState;

typedef enum {
    CTRL_ACTIVE     = 0x01,
    CTRL_FLAG_02    = 0x02,
    CTRL_FLAG_10    = 0x10,
    CTRL_FLAG_20    = 0x20,
    CTRL_FLAG_30    = 0x30,
    CTRL_FLAG_40    = 0x40,
    CTRL_FLAG_80    = 0x80,
    CTRL_FLAG_100   = 0x100,
    CTRL_FLAG_400   = 0x400
} ControlFlags;


typedef struct {
    u8 unk0;
    u8 unk1; 
} subStruct;

typedef struct {
    subStruct unk0[4];
} Struct_func_800A8794;

/**
* @brief Data block reached two indirections away through
*        @c BattleEntity.linkedPtr->data.
*
* Size and most fields are unknown; only the byte at offset 0x14F
* (read by @c func_800AF988) is mapped so far.
*/
typedef struct {
    u8 pad0[8];
    u32 unk8;
    u8 padC[4];
    volatile s32 unk10;  // volatile because func_800A5688, func_800A559C
    volatile s32 unk14;  // volatile because func_800A5688, func_800A559C
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24[7];
    u8 pad40[4];
    u16 unk44[8];
    s16 unk54[16];
    u8 pad74[0x7C - 0x74];
    volatile s32 unk7C;
    u16 unk80;
    u8 pad82[2];
    u16 unk84;
    u16 unk86;
    u8 unk88;
    u8 unk89;
    u8 unk8A;
    u8 pad8B[5];
    u8 unk90[40];
    u8 unkB8;
    u8 unkB9;
    u8 unkBA;
    u8 unkBB;
    u8 unkBC;
    u8 unkBD;
    u8 unkBE;
    u8 unkBF;
    u8 unkC0;
    u8 unkC1; 
    u8 unkC2;
    u8 unkC3;
    u8 unkC4;
    u8 unkC5;
    u8 unkC6;
    u8 unkC7;
    u8 unkC8;
    u8 padC9;
    u8 unkCA;
    u8 padCB;
    u16 unkCC;
    u8 padCE[0x26];
    u8 unkF4;
    u8 unkF5;
    u8 unkF6;
    u8 immunityFlags;   /* bit 0 forces status bit 0x40 clear, bit 1 forces flags bit 0x2000 clear (read by @c func_8009AFF0). */
    u8 unkF8;
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    u8 unkFC;
    u8 unkFD;
    u8 unkFE;
    u8 unkFF;
    u8 pad100[4];
    Struct_func_800A8794 unk104[1];       /* size unknown, probably struct, used in func_8009F65C */
    u8 pad10C[0x14F - 0x10C];
    u8 unk14F;          /* byte read by func_800AF988. */
    u16 unk150[1];
    u8 pad152[0xE];
    u8 unk160[8]; 
    u8 unk168[40];// possibly size taken from func_800A7FD0 while calling func_800A7EE0
} BattleEntityData;

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
        s32 volatile word;
        struct { u8 b0; u8 b1; u8 b2; u8 trigType; } bytes;
    } state;
    /* 0x08: byte view exposes @c trigKey (pending-trigger key matched
    against arg). Word view (@c initFlags) is a 4-byte init-time
    animation/render flag word written by @c func_800A7518. */
    union {
        struct {
            u8 trigKey;     /* 0x08 */
            u8 unk9;        /* 0x09 */
            u8 unkA;
            u8 padB;
        } byteView;
        s32 initFlags;        /* 0x08-0x0B as a single word. */
    } slot8;
    union {
        struct {
            u8 volatile timer;
            u8 control;
        } SplitTimer;
        u16 bigTimer;
    } timers;
    u8 unkE;
    u8 entityRef;
    BattleEntityData** entityData;
    s32 pad14;
    s32 flags;
    s32 flagsBackup;
    s32 unk20;
    s32 volatile unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    u8 pad34[0x20];     
    u16 unk54[1];  /* used in  func_8009C598 */
    u8 unk56;
    u8 unk57;
    u16 unk58;
    u8 pad59[10];
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
    u8 pad8A[2];
    volatile ControlFlags controlFlags;
    u16 status;
    u16 statusBackup;
    s16 hpDisplay;     /* 0x94: HP value mirrored from BattleCharData.currentHp. */
    u16 unk96;
    u8 unk98;
    u8 unk99;
    u8 pad9A;
    u8 unk9B;
    u8 unk9C;
    u8 unk9D;
    u8 unk9E;
    u8 unk9F;
    u8 unkA0[1];
    u8 padA1[22];
    u8 unkB7;
    u8 padB8[3];
    u8 linkedIdx2;
    u8 padBC[5];
    u8 unkC1;
    u8 padC2[6];
    u8 unkC8[3];
    u8 linkedIdx;
    u8 unkCC;
    u8 unkCD;        /* 0xCD: stat byte used in case-0 damage formula (squared). */
    u8 unkCE;
    u8 unkCF;        /* 0xCF: stat byte averaged with arg2 in func_8009DEF0 mode-7. */
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

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u16 unk4;
    u16 unk6;
    u32 unk8;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u16 unk10;
    u16 unk12;
    s32 unk14;
} SubEntry; // 24 bytes

/** @brief 20-byte action-queue entry in @c BattleSystem.entries. */
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s16 unk4;
    s16 unk6;
    SubEntry* subEntries;
    u8* unkC;
    u8 unk10;
    u8 unk11;
    u8 pad12[2];
} BattleEntry; /*  0x14 (20 bytes) */

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
typedef void (*callback_t)(s32);
typedef struct {
    callback_t callback;   /* function pointer (called by @c func_8009B478). */
    u32 unk4;      
    u16 timer;      /* countdown (ticked by callbacks). */
    u16 unkA;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 done;        /* completion flag (1 = ready to free). */
} TaskEntry; /* 0x10 */

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
} Struct_12CC; /* used in func_8009D594 */

typedef struct {
    TaskLink link;
    u16 unk4;
    u16 unk6[3]; // func_800A5948
} BattleUnkDE8;    /* 12 bytes */

typedef struct{
    u16 unk0;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
} InternalStruct; /* 8 bytes */

typedef struct{
    InternalStruct unk0[3];
} Struct_1244;

typedef struct {
    /* 0x0000 */ BattleEntity entities[7];          /**< 7 × 0xD0 = 0x5B0. Index 0 is also the header proxy. */
    /* 0x05B0 */ u8 pad5B0[0x10];                   /**< Pre-control padding. */
    /* 0x05C0 */ u8 unk5C0;                         /**< Action queue head index (used by func_800B06DC). */
    /* 0x05C1 */ u8 unk5C1;
    /* 0x05C2 */ u8 volatile unk5C2;                /**< Misc state byte (init to 1 by func_8009A1E0/ACEC). */
    /* 0x05C3 */ u8 unk5C3;                         /**< Misc state byte (init to 1 by func_80099FE8). */
    /* 0x05C4 */ BattleEntry entries[32];           /**< Action queue (stride 0x14)*/
    /* 0x0844 */ SubEntry Array844[1];
    /* 0x085C */ u8 pad85C[0x0CDC - 0x085C];
    /* 0x0CDC */ u8 unkCDC;
    /* 0x0CDC */ u8 unkCDD;
    /* 0x0CDE */ u8 padCDE[0x0CE0 - 0x0CDE];
    /* 0x0CE0 */ u8 unkCE0;
    /* 0x0CE1 */ u8 unkCE1;
    /* 0x0CE2 */ u8 unkCE2;
    /* 0x0CE3 */ u8 unkCE3;
    /* 0x0CE4 */ BattleVec3u unkCE4[8];             /**< 8-entry x/y/z position table (read by @c func_8009A528). */
    /* 0x0D14 */ u8 unkD14[8];                      /**< Hit-type byte table (8 entries). */
    /* 0x0D1C */ u8 padD1C[0x0D54 - 0x0D1C];
    /* 0x0D54 */ u8 unkD54[8];
    /* 0x0D5C */ u8 unkD5C[8];                      /**< Per-trigger flag array (8 entries). */
    /* 0x0D64 */ TaskLink unkD64[3][11];
    /* 0x0DE8 */ BattleUnkDE8 arrayDE8[3][11][2];   /**< (792 bytes: 0x318) size tied to func_800A5948 */
    /* 0x1100 */ u8 unk1100[3];                     // indexes used for taskLink
    /* 0x1103 */ TaskLink taskLinks[16];            /**< Task queue link table (16 × 4 bytes). */
    /* 0x1143 */ u8 pad1143;
    /* 0x1144 */ TaskEntry taskData[16];            /**< Task queue data slots (16 × 16 bytes). */
    /* 0x1244 */ Struct_1244 unk1244[3];
    /* 0x128C */ callback_t unk128C;                /**< Cached userData for callback. */
    /* 0x1290 */ s16 unk1290;
    /* 0x1292 */ s16 unk1292;
    /* 0x1294 */ s16 unk1294;
    /* 0x1296 */ s16 unk1296;
    /* 0x1298 */ u8 pad1298[0x12B8 - 0x1298];
    /* 0x12B8 */ u16 array12B8[7];
    /* 0x12C6 */ u8 pad12C7[0x12CC - 0x12C6];
    /* 0x12CC */ Struct_12CC array12CC[1];          /* used in func_8009D594 */
    /* 0x12CF */ u8 pad12CF[0x12D8 - 0x12CF];
    /* 0x12D8 */ s32 unk12D8;                       /**< Cached length argument for callback. */
    /* 0x12DC */ u8* unk12DC;
    /* 0x12E0 */ s16 unk12E0;                       /**< Low 13 bits of a packed s16 field. */
    /* 0x12E2 */ u16 unk12E2;
    /* 0x12E4 */ s16 unk12E4;               
    /* 0x12E5 */ u8 pad12E5[2];                     /**< Misc state. */
    /* 0x12E8 */ u8 unk12E8;                        /**< Misc state byte. */
    /* 0x12E9 */ u8 volatile unk12E9;               /**< Misc state byte (touched by 12EA-gated path). */
    /* 0x12EA */ u8 volatile unk12EA;               /**< Misc state gate byte. */
    /* 0x12EB */ u8 volatile unk12EB;               /**< Misc state. */
    /* 0x12EC */ u8 unk12EC;                        /**< Misc state byte (init to 0xFF). */
    /* 0x12ED */ u8 volatile unk12ED;               /**< Misc state byte. */
    /* 0x12EE */ u8 volatile unk12EE;               /**< Misc state byte. */
    /* 0x12EF */ u8 volatile unk12EF;
    /* 0x12EF */ u8 unk12F0;
    /* 0x12EF */ u8 pad12F1;
    /* 0x12EF */ u8 unk12F2;
    /* 0x12F3 */ u8 unk12F3;                        /* used as index in func_8009F824 */
    /* 0x12F4 */ u8 unk12F4;
    /* 0x12F5 */ u8 unk12F5;
    /* 0x12F6 */ u8 taskHead;                       /**< Head index of the task queue linked list. */
    /* 0x12F7 */ u8 unk12F7;
    /* 0x12F8 */ u8 unk12F8;
    /* 0x12F9 */ u8 unk12F9;
    /* 0x12FA */ u8 pad12FA[3];
    /* 0x12FD */ u8 unk12FD;
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
    /* 0x130F */ s8 unk130F;                       /**< Upper 3 bits of a packed s16 field (sign-extended). */
    /* 0x1310 */ u8 unk1310;
    /* 0x1311 */ u8 actionType;                    /**< Queued action type (0=none, 1=stat-up message). */
    /* 0x1312 */ u8 actionByte0;                   /**< Queued action arg 0 (stat ID for type 1). */
    /* 0x1313 */ u8 actionByte1;                   /**< Queued action arg 1 (count for type 1). */
    /* 0x1314 */ u8 unk1314;  
    /* 0x1315 */ u8 unk1315;  
    /* 0x1316 */ u8 unk1316;
    /* 0x1317 */ u8 unk1317;
    /* 0x1318 */ u8 unk1318;
    /* 0x1319 */ u8 unk1319;                       /**< Misc state byte (init to 0xFF). */
    /* 0x131A */ u8 unk131A;
    /* 0x131B */ u8 unk131B;                       /**< More misc state. */
    /* 0x131C */ u8 unk131C;
    /* 0x131D */ u8 unk131D;
    /* 0x131E */ u8 unk131E;
    /* 0x131F */ u8 unk131F;
    /* 0x1320 */ u8 unk1320;
    /* 0x1321 */ u8 unk1321;
    /* 0x1322 */ u8 unk1322;
    /* 0x1323 */ u8 effectMult;                    /**< Damage/effect multiplier (percent). */
    /* 0x1324 */ u8 unk1324;
    /* 0x1325 */ u8 unk1325;
    /* 0x1326 */ u8 unk1326;
    /* 0x1327 */ u8 unk1327;
    /* 0x1328 */ u8 unk1328;
    /* 0x1329 */ u8 unk1329;
    /* 0x132A */ u8 unk132A;
    /* 0x132B */ u8 unk132B;
    /* 0x132C */ u8 unk132C;
    /* 0x132D */ u8 pad132D;
    /* 0x132E */ u8 unk132E;
    /* 0x132F */ u8 pad132F;
    /* 0x1330 */ u16 unk1330[3];
    /* 0x1338 */ s32 unk1338[1];
} BattleSystem; /* 0x133C */

/** @brief 5-byte slot in @c BattleAnimTable.animSlots. */
typedef struct {
    u8 id;          /* lookup key / command byte. */
    s8 value;       /* signed value byte. */
    u8 unk2;
    u8 unk3;
    u8 unk4;
} BattleAnimSlot;


/** this struct might be related to draw function. Unk0 accessed in func_800A3094 */
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 pad3;
} drawSlot;


/** @brief 0x47-byte sub-entry in @c BattleAnimTable.subEntries. */
typedef struct {
    drawSlot array0[4]; /* size related to the index used in func_800A3094 */
    u8 unk10;
    u8 pad11[0x2F];
    u8 unk40[6];
    u8 unk46;
} BattleAnimSubEntry;   /* 0x47 */

/** @brief Battle anim/scene lookup table at @c D_800EE9E8. */
typedef struct {
    /* 0x000 */ BattleAnimSlot animSlots[32];      /**< 32 × 5 = 0xA0 bytes. */
    /* 0x0A3 */ BattleAnimSubEntry subEntries[3];  /**< 3 × 0x47 = 0xD5 bytes. */
} BattleAnimTable;

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
    u8 pad4[0x6C];
    u32 ot[2];
    s32 pktPtr;
    u8 freeSpace[4];
} BattleOtBuf;

/** @brief Battle magic slot entry (5 bytes). */
typedef struct {
    u8 unk0;
    s8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
} BattleMagicSlot;

/** @brief Battle item slot entry (5 bytes). */
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
} BattleItemSlot;

/** @brief Battle command slot entry (4 bytes). */
typedef struct {
    u8 cmdType;
    u8 unk1;
    u8 unk2;
    u8 unk3;
} BattleCmdSlot;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
} BattleUnkSlot;

/** @brief Battle character render data (g_battleChars, stride 0x1D0 = 464 bytes). */
typedef struct {
    /* 0x000 */ u8 pad0[0x008 - 0x000];
    /* 0x008 */ BattleUnkSlot unkSlots[3];
    /* 0x014 */ u16 unk14;
    /* 0x016 */ u16 unk16;
    /* 0x018 */ s16 currentHp;          /**< Current HP in battle. */
    /* 0x01A */ u16 unk1A;
    /* 0x01C */ u8 unk1C;
    /* 0x01D */ u8 unk1D;
    /* 0x01E */ BattleCmdSlot cmdSlots[4];
    /* 0x02E */ u8 pad2E[0x082 - 0x02E];
    /* 0x082 */ BattleMagicSlot magicSlots[32];
    /* 0x122 */ BattleItemSlot itemSlots[16];
    /* 0x172 */ s16 unk172;          /**< Mirrored HP cap (set with hpRegenCap when battle HP is reduced). */
    /* 0x174 */ s16 hpRegenCap;        /**< HP regen cap (field-walk tick stops when currentHp reaches this). */
    /* 0x176 */ u8 pad176[0x17C - 0x176];
    /* 0x17C */ s32 xpToNext;          /**< XP needed to reach next level. */
    /* 0x180 */ u32 unk180;
    /* 0x184 */ u32 unk184;
    /* 0x188 */ s32 unk188;          /**< Status/ability mask checked for bit 0x60000. */
    /* 0x18C */ s32 abilityFlags;
    /* 0x190 */ s32 statusFlags;
    /* 0x194 */ u16 elemResistances[8];/**< Element resistance values (8 × s16). */
    /* 0x1A4 */ u8 statusResistances[13];/**< Status resistance values (13 × u8). */
    /* 0x1B1 */ u8 pad1B1;
    /* 0x1B2 */ u16 displayStatus;       /**< Mirror of BattleEntity.status; bit-flag display state. */
    /* 0x1B4 */ u16 abilityValue;
    /* 0x1B6 */ u16 atkStatusHit;      /**< Attack status hit chance. */
    /* 0x1B8 */ u8 level;              /**< Battle level (from findCharXpLevel). */
    /* 0x1B9 */ u8 unk1B9;
    /* 0x1BA */ u8 classId;            /**< Entity-class index into the @c D_80078E00 ability tables (stride 12 at @c 0x35C1). */
    /* 0x1BB */ u8 stats[8];           /**< Battle stats: STR, VIT, MAG, SPR, SPD, ?, hit (0x1C0), eva (0x1C1). 0x1C2 = ? */
    /* 0x1C3 */ u8 characterId;
    /* 0x1C4 */ u8 atkElemBase;        /**< Attack element base. */
    /* 0x1C5 */ u8 atkElemBonus;       /**< Attack element bonus. */
    /* 0x1C6 */ u8 fieldStatusByte;    /**< Status byte checked by field script (bit 1 = greyed out). */
    /* 0x1C7 */ u8 statCoefs[9];       /**< Stat coefficient table (HP, str, vit, mag, spr, spd, ?, eva, hit). */
} BattleCharData;    /* 0x1D0: 464 bytes */

/** @brief GF battle entry (12 bytes, used for GF HP in battle). */
typedef struct {
    u8 pad0[8];
    u16 maxHp;          /* max HP cap (used to restore hp on revive) */
    s16 hp;             /* current HP */
} BattleGfEntry;

/** @brief GF battle level entry (12 bytes). */
typedef struct {
    u8 level;
    u8 unk1;
    u8 pad2;
    u8 unk3;
    u8 abilityFlags;    /* party ability flags (used in entry 15). */
    u8 pad5[7];
} BattleLevelEntry;

/** @brief Complete battle character/GF state block. */
typedef struct {
    /* 0x000 */ BattleCharData chars[3];          /* 3 party members × 0x1D0 */
    /* 0x570 */ u8 pad570[0x610 - 0x570];
    /* 0x610 */ BattleGfEntry gfEntries[1];       /* hp sub-array (stride 12, 16 entries) */
    /* 0x61C */ u8 pad61C[0x620 - 0x61C];
    /* 0x620 */ BattleLevelEntry levelEntries[16]; /* 16 × 12 bytes */
} BattleCharState;/* 0x6E0 */



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
 * The array runs the full remaining 0x4000 bytes of the struct, so the
 * far end doubles as the HUD layer: the map-view drawers link into
 * @c primList[0xFFF] and @c primList[0xFFE], the last two slots.
 */
typedef struct {
    /* 0x0000 */ DRAWENV drawEnv;       /**< Draw-env template, copied to the active env. */
    /* 0x005C */ DISPENV disp;          /**< Display-env template; copied to D_80082C18 in setupWorldRender. */
    /* 0x0070 */ s32 primList[0x1000];   /**< Ordering table; see the depth aliases below. */
} BattleSceneCtx;                       /* 0x4070 */

/* Named aliases for the two specifically-purposed primList slots. */
#define BSC_COLORTAG_IDX 1   /**< primList[1] @ +0x74 — renderBattleDisplayList color tag. */
#define BSC_OTHEAD_IDX   3   /**< primList[3] @ +0x7C — main addPrim chain head. */
#define BSC_HUD_IDX      0xFFF /**< primList[0xFFF] @ +0x406C — map-view HUD layer. */
#define BSC_MARKER_IDX   0xFFE /**< primList[0xFFE] @ +0x4068 — map-view marker layer. */



/**
 * @brief Spell record in the battle scene buffer (D_80078E00.spells, stride 60).
 *
 * Only byte 0 (magicId) is read by the known callers.
 */
typedef struct {
    u16 unk0;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5; 
    u8 magicId; /**< Magic/spell ID byte (input to ability flag funcs) */
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 padB;
    u32 unkC;
    u16 unk10;
    u8 unk12;
    u8 pad13[18];
    s16 unk26;
    u8 pad6[20];
} BattleSpellRow; /* 60 bytes */

/**
 * @brief Ability record in the battle scene buffer (D_80078E00.abilities, stride 24).
 *
 * Only byte 0 (abilityId) is read by the known callers.
 */
typedef struct {    
    u8 unk0;             /* 0x3938 */
    u8 abilityId;        /**< 0x3939: Ability ID byte (input to ability flag funcs) */
    u8 unk2;             /* 0x393A */
    u8 unk3;             /* 0x393B */
    u8 pad4;             /* 0x393C */
    u8 unk5;             /* 0x393D */
    u16 unk6;            /* 0x393E */
    u32 unk8;            /* 0x3940 */
    u8 unkC;             /* 0x3944 */
    u8 unkD;             /* 0x3945 */
    u8 unkE;             /* 0x3946 */
    u8 unkF;             /* 0x3947 */
    u16 unk10;           /* 0x3948 */
    u8 unk12;            /* 0x394A */
    u8 pad13;            /* 0x394B */
    u8 unk14;            /* 0x394C */
    u8 unk15;            /* 0x394D */
    u8 val;              /* 0x394E: used in func_8009BAC4 */
    u8 unk17;            /* 0x394F */
} BattleAbilityRow; /* 24 bytes */

/**
 * @brief 20-byte scene entry indexed by funcs that pass entry.lookupId
 *        plus a sibling word in @c BattleSceneData to @c resolveKernelPtr.
 */
typedef struct {
    u16 lookupId;       /**< u16 passed to resolveKernelPtr. */
    u16 unk2;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u16 unkE;
    u32 unk10;
} BattleSceneEntry;     /* 20 bytes */

typedef struct {
    u16 lookupId;    /**< u16 passed to resolveKernelPtr. */    
    u16 unk2;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u32 unkC;
    u16 unk10;
    u8 unk12;
    u8 unk13;
} BattleSceneEntry2;     /* 20 bytes */


/**
 * @brief 132-byte scene row indexed by func_800AFF70 (a0 offset by 0x40).
 */
typedef struct {
    u16 lookupId;       /**< u16 passed to resolveKernelPtr. */
    u8 pad2[2];
    u16 unk4;
    u8 unk6;
    u8 unk7;
    u8 pad8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u16 unkE;
    u32 unk10;
    u8 pad14[7];
    u8 unk1B;
    u8 pad1C[0x70 - 0x1C];
    u8 unk70;
    u8 pad71[0x82 - 0x71];
    u8 unk83;
    u8 unk84;
} BattleSceneRow;       /* 132 bytes */

/**
 * @brief 8-byte scene row indexed by func_800B0360.
 */
typedef struct {
    u16 lookupId;       /**< u16 passed to resolveKernelPtr. */
    u8 pad2[6];
} BattleSceneRow8;      /* 8 bytes */

typedef struct {
    u16 lookupId;    /* 0x4C0C */
    u8  unk2;        /* 0x4C0E */
    u8  unk3;        /* 0x4C0F */
    u32 unk5;        /* 0x4C10 */
    u16 unk8;        /* 0x4C14 */
    u8  flags;       /* 0x4C16 */
    u8  maxHP;       /* 0x4C17 */
} Struct_4C0C;       /* 0xC: 12 bytes */

typedef struct {
    u16 unk48BC;
    u8 unk48BE;
    u8 unk48BF;
    u8 unk48C0;
    u8 pad48C1;
    u8 unk48C2;
    u8 unk48C3;
    u8 unk48C4;
    u8 unk48C5;
    u8 unk48C6;
    u8 unk48C7;
    u8 pad48C8[10];
    u16 unk48D2;
    u32 unk48D4;
    u8 pad48D8[4];
} Struct_48BC;    /* 32 bytes */

typedef struct {
    u16 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk8;
    u8 unk9;
    u16 unkA;
    u32 unkC;
} Struct_4020;    /* 16 bytes */

typedef struct {
    u8 pad4A6C[2];
    u16 unk4A6E;
    u8 unk4A70;
    u8 unk4A71;
    u8 unk4A72;
    u8 pad4A73;
    u8 unk4A74;
    u8 unk4A75;
    u8 unk4A76;
    u8 unk4A77;
    u8 unk4A78;
    u8 unk4A79;
    u16 unk4A7A;
    u32 unk4A7C;
} Struct_4A6C;    /* 20 bytes */


typedef struct {
    s32 unk0;
    u16 unk4;
    u8 unk6;
    u8 unk7;
} Struct_45F8;      /* 8 bytes */

typedef struct {
    u16 unk0;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 pad5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u16 unkC;
    u8 unkE;
    u8 unkF;
    u32 unk10;
    u8 pad14[4];
} Struct_446C; /* 24 bytes */


typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u16 unk4;
    u16 unk6;
    u32 unk8;
    u8 padC[7];
    u16 unk14;
    u8 unk16;
    u8 unk17;
} Struct_3750; /* 24 bytes */

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 pad2[2];
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 pad7[2];
    u8 unk9;
    u8 padA[2]; 
} Struct_35BD; /* 12 bytes */


typedef struct {
    u8 unk0;
    u8 unk1;
    u8 pad2;
    u8 unk3;
    u8 pad4[32];
} Struct_37A6; /* 36 bytes */

typedef struct {
    u8 unk0;
    u8 unk1;    
} Struct4CFC; /* 2 bytes */


typedef struct {
    u8 unk0;
    u8 pad1;
    u8 unk2;
    u8 pad3[5];
} structE8;

/**
 * @brief Battle scene data buffer at D_80078E00 (loaded from disc, ~0x9E08 bytes).
 *
 * Contains kernel-data pointers and various sub-arrays. Many regions remain
 * unidentified — fields will be added as more code is decompiled.
 */
typedef struct {
    /* 0x0000 */ u8 pad0[0x0088 - 0x0000];
    /* 0x0088 */ s32 rows132Arg;                /**< resolveKernelPtr arg paired with rows132[]. */
    /* 0x008C */ s32 entries17Arg;              /**< resolveKernelPtr arg paired with entries17[]. */
    /* 0x0090 */ u8 pad0090[0x00A4 - 0x0090];
    /* 0x00A4 */ s32 entriesA0Arg;              /**< resolveKernelPtr arg paired with entriesA0[]. */
    /* 0x00A8 */ u8 pad00A8[0x00D4 - 0x00A8];
    /* 0x00D4 */ s32 rows8Arg;                  /**< resolveKernelPtr arg paired with rows8[]. */
    /* 0x00D8 */ u8 pad00D8[0x00DC - 0x00D8];                
    /* 0x00DC */ s32 unk4C0CArg;                /**< resolveKernelPtr arg paired with unk4C0C[]. */
    /* 0x00E0 */ u8 pad00E0[0x00E8 - 0x00E0];
    /* 0x00E8 */ structE8 unkE8[36];
    /* 0x0208 */ u8 pad0208[0x0220 - 0x0208];
    /* 0x0220 */ BattleSpellRow spells[1];      /**< 60-byte stride (size unknown, index past). */
    /* 0x025C */ u8 pad025C[0x0F78 - 0x025C];
    /* 0x0F78 */ BattleSceneRow rows132[1];     /**< 132-byte stride (size unknown, index past). */
    /* 0x0FFC */ u8 pad0FFC[0x17B8 - 0xFFC];
    /* 0x17B8 */ BattleSceneEntry entries17[1]; /**< stride 20 (size unknown, index past). */
    /* 0x17CC */ u8 pad17CC[0x35B1 - 0x17CC];
    /* 0x35B1 */ Struct_35BD array35B1[1];
    /* 0x35BD */ Struct_35BD array35BD[1];
    /* 0x35CC */ u8 pad35C9[0x3738 - 0x35CC];
    /* 0x3738 */ BattleAbilityRow unk3738[1];
    /* 0x3750 */ Struct_3750 array3750[1];
    /* 0x3768 */ u8 pad3768[0x37A6 - 0x3768];
    /* 0x37A6 */ Struct_37A6 array37A6[1];
    /* 0x37CA */ u8 pad37CD[0x3920 - 0x37CA];
    /* 0x3920 */ Struct_3750 array3920[1];
    /* 0x3938 */ BattleAbilityRow abilities[1]; /**< 24-byte stride (size unknown, index past). */
    /* 0x3950 */ u8 pad3950[0x3EE0 - 0x3950];
    /* 0x3EE0 */ BattleSceneEntry2 entriesA0[1]; /**< stride 20 (size unknown, index past). */
    /* 0x3EF4 */ u8 pad3EF4[0x3F5A - 0x3EF4];
    /* 0x3F5A */ u16 unk3F5A;
    /* 0x3F5C */ u8 pad3F5C[0x3F62 - 0x3F5C];
    /* 0x3F62 */ u8 unk3F62;
    /* 0x3F63 */ u8 pad3F63[0x4020 - 0x3F63];
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
    /* 0x48DC */ u8 pad48DC[0x49F8 - 0x48DC];
    /* 0x49F8 */ s32 unk49F8[1];
    /* 0x49FC */ u8 pad49FC[0x4A5E - 0x49FC];
    /* 0x4A5E */ BattleSceneRow8 rows8[1];      /**< stride 8 (size unknown, index past). */
    /* 0x4A66 */ u8 pad4A66[0x4A6C - 0x4A66];
    /* 0x4A6C */ Struct_4A6C array4A6C[1];
    /* 0x4A80 */ u8 pad4A80[0x4C0C - 0x4A80];
    /* 0x4C0C */ Struct_4C0C unk4C0C[1];
    /* 0x4C18 */ u8 pad4C18[0x4CCC - 0x4C18];
    /* 0x4CCC */ u8 unk4CCC[16]; // confirmed to be atleast 14
    /* 0x4CDC */ u8 unk4CDC[8];
    /* 0x4CE4 */ u8 unk4CE4[24];
    /* 0x4CFC */ Struct4CFC unk4CFC[3];
    /* 0x4D03 */ u8 unk4D03[2];
} BattleSceneData;


/** @brief Sound-command queue slot returned by @c func_8009B134.
 *
 * The two parameter bytes at +2/+3 are sometimes written as a single u16
 * and sometimes as two separate u8s (callers vary by command id), so they
 * are exposed via U16Split to keep both views available. */
typedef struct {
    u16 unk0;
    U16Split unk2;
} SoundCmd;

/** @brief 4-byte (x,z) position pair used by @c func_8009A74C battle slot layout tables. */
typedef struct {
    u16 x;
    u16 z;
} BattlePosXZ;

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
    /* 0x08 */ u8 unk8;
    /* 0x08 */ u8 unk9;
    /* 0x0A */ u8 unkA;    
    /* 0x0B */ u8 unkB;
    /* 0x0C */ u32 unkC;        /**< Scaled by 3/2 when the active entity has controlFlag bit 0x20. */
    /* 0x10 */ u32 unk10;
    /* 0x14 */ u32 unk14;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u16 statusCode;   /**< Status/command code; compared against 0x49 in func_8009D68C. */
    /* 0x1E */ u16 unk1E;
    /* 0x20 */ u16 unk20;
    /* 0x22 */ u8 pad22[30];
} BattleCmdBuf;   /* 64 bytes */

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
} BattleEffectPrim; /* 0x58 */


/* ---------------------------------------------------------------- *
 *  Battle data symbols (battle overlay region).
 * ---------------------------------------------------------------- */

extern BattleCharState g_battleChars;
extern BattleConfig    g_battleConfig;
extern s16             D_8005F11C;
extern u8              D_80077E58;
extern u8              D_80077E92;
extern u8              D_80077E59;
extern u8              D_800786D9;
extern u8              D_80078DF8;
extern BattleSceneData D_80078E00;
extern u16             D_80082C0A;
extern u8              D_80082C0F;
extern MsgFormatConfig D_80083858;
extern u8              D_80098030[];
extern BattleSceneCtx* D_800D244C;
extern s32             D_800E19B4[];
extern s32             D_800E19BC[];
extern u16             D_800E3CA4[];
extern BattlePosXZ     D_800E3CA8[];
extern BattlePosXZ     D_800E3CB0[];
extern u8              D_800E3CC5;
extern u8              D_800E3CC6;
extern u8              D_800E3CBC[];
extern u8              D_800E3CE8;
extern BattleSystem    D_800ED148;
extern BattleCmdBuf    D_800EE4C0;
extern BattleAnimTable D_800EE9E8;
extern u8              D_800EEBA8[];
extern u8              D_800EEBB0;
extern u8              D_800EEBB8;
extern u8              D_800EEBB9;
extern u8              D_800EEBBA;
extern u8              D_800EEBBB;
extern u8              D_800EEBBC;
extern u8              D_800EEBBD;
extern u8              D_800EEBBE;
extern u8              D_800EEBBF;
extern u8              D_800EEBC0;
extern u16             D_800EEBC2;
extern s32             D_800EEBC4;
extern u8              D_800EEBC8;
extern u8              D_800EEBD0;

/* ---------------------------------------------------------------- *
 *  Battle-overlay function prototypes (battle internals).
 * ---------------------------------------------------------------- */

/** @brief Apply a status flag, ORing it into the flag word. */

/** @brief Set @c field64[lowest-bit-of-a1] = -0x457 on entity @p a0. */

u16 func_800B1050(s32 stat);

/** @brief Look up auxiliary ability flags by stat byte (low bits). */

void func_800B3128(u8 *a0);

/** @brief Look up auxiliary ability flags by stat byte (high bits). */
u16 func_800B0F9C(s32 stat);

/** @brief Resolve battle scene context pointer. */
void func_800A1760(s32 arg0, BattleCharData* arg1);

/** @brief Apply a stat-effect probe; outputs (a1=stat, a2=count). */

/** @brief Format helper that writes into a caller-provided buffer. */
u8 *func_800B04A0(s32 a0, u8 *buf);

/** @brief Concatenate two parts into the @c D_800EEBE8 message buffer. */

/** @brief Finalize the @c D_800EEBE8 message buffer. */
u8 *func_800B02AC(u8 *buf);

/* --- Battle animation lifecycle --- */
void activateBattleAnim(s32 idx);

/* --- Spatial / matrix helpers (defined in field overlay) --- */
void func_800406A4(u8 *p);
void func_80040734(u8 *p);
/* func_80040DE4 (the main binary's RotTransPers) is declared in psxsdk/libgte.h. */

/** @brief Reset battle-transition state (clears @c btl_color flags). */
void initBattleTransition(void);
void func_800D0608(void); /* bc_object17: overlay VSync handler (RENDER_OVERLAY) */


void func_8002A2C4(u8 *, s32);
s32 func_80037ADC(void);
u16 func_800A97FC(s32 arg0);

#endif /* BATTLE_H */
