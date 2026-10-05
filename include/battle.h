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

/** @brief BattleConfig.unk2 flag: a countdown is running. The menu clock shows it
 * instead of the play time, and a battle ends when it reaches 0. */
#define BATTLE_FLAG_COUNTDOWN 0x04

/** @brief BattleConfig.unk2 flag: the battle keeps the field's music. No battle music is
 * loaded before it and the field's is not reloaded after. */
#define BATTLE_FLAG_KEEP_MUSIC 0x10

#define GET_OFFSET(type, ptr, var) ((type*)(var + (intrptr_t)ptr))

/** @brief Battle command config (g_battleConfig). */
typedef struct {
    u16 battleSceneId;
    u16 unk2;            // Flags?, when first bit is set, escape it not possible (D_80082C0A)
    u8  unk4[3];         // Post battle command queue?
    u8  result;          /**< Battle result (BATTLE_RESULT_*). (D_80082C0F) */
    u8  unk8;
    u8  unk9;            /**< Bit 0 toggles the @c FieldVars.soundBankSelector at field-VM init. */
} BattleConfig;

/** @brief Bits of BattleConfig.unk8 (D_80082C10) that turn battle commands off. */
#define BATTLE_CMDS_OFF_4_13 0x01 /**< Commands 4 and 13. */
#define BATTLE_CMDS_OFF_2 0x02
#define BATTLE_CMDS_OFF_3 0x04
#define BATTLE_CMDS_OFF_6 0x08
#define BATTLE_CMDS_OFF_OTHER 0x10 /**< Every other command except 0. */

/** @brief Clipped rectangle result: the clipped rect + saved pre-clip position. */
typedef struct {
    RECT rect; /* 0x00: clipped rectangle */
    s32 savedPos; /* 0x08: packed original x|y before clipping */
} ClipResult;

/** @brief Scratch workspace for rectangle clipping operations. */
typedef struct {
    ClipResult work;
    ClipResult disp;
} ClipWork;

struct BattleDisplayEntity;
typedef void (*EntityCallback)(struct BattleDisplayEntity *entity, u32 input, u32 repeat);

/** @brief Render hook of a battle entity: draws it at @p pkt, returns the next free packet. */
typedef void *(*EntityRenderCallback)(void *ot, struct BattleDisplayEntity *entity, void *pkt);

typedef struct BattleDisplayEntity {
    EntityCallback callback; /* update function pointer */
    EntityRenderCallback render; /**< Draws the entity's contents; NULL for none. */
    RECT boundRect;
    RECT dispRect;
    ClipResult clipBound; /**< @c boundRect clipped by @ref clipBlitRects. */
    ClipResult clipClamp; /**< @c dispRect clipped by @ref clipBlitRects. */
    s32 drawMode;
    u8 activeFlag;
    u8 unk35;
    u8 unk36;
    u8 animSpeed;
    u8 entityType;
    u8 pad39;
    u8 subFields[2];
    s16 brightness; /**< 0x1000 = full: tint of a window's frame, background and icon. */
    s16 pad3E;
} BattleDisplayEntity;

/** @brief Parameters for a double-blit operation with source rects and destination buffers. */
typedef struct {
    u8 pad0[8];
    RECT srcRect1;
    RECT srcRect2;
    u8 dstData1[12];
    u8 dstData2[12];
} BlitParams;


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
    subStruct sub[4];
} Struct_func_800A8794;

typedef struct{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
} func_800A7D8C_Struct;

typedef struct {
    u8 pad0[0x18 - 0x00];
    func_800A7D8C_Struct unk18;
    func_800A7D8C_Struct unk1C;
    func_800A7D8C_Struct unk20;
    func_800A7D8C_Struct unk24;
    func_800A7D8C_Struct unk28;
    func_800A7D8C_Struct unk2C;
    func_800A7D8C_Struct unk30;
    u8 pad1C[0xF4 - 0x34];
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
    Struct_func_800A8794 unk104[3][3];
    u8 unk14C;
    u8 unk14D;
    u8 pad14E;
    u8 unk14F;          /* byte read by func_800AF988. */
    u16 unk150[8];
    u8 unk160[8]; // size not confirmed
    u8 unk168[40];// possibly size taken from func_800A7FD0 while calling func_800A7EE0
} BattleEntityData;

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 volatile unk4;
    /* 0x08 */ u8* unk8; // stores g_kernel.magic[arg2].gfCompatibility or g_kernel.junctionableGfs[arg2 - 64].gfCompatibility
    /* 0x0C */ u8 volatile timer;
    /* 0x0D */ u8 control;
    /* 0x0E */ u8 unkE;
    /* 0x0F */ u8 entityRef;
} BattleHeader; /* 16 bytes */

typedef struct {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
} Unk4Struct;

typedef struct {
    /* 0x00 */ BattleEntityData** entityData;
    /* 0x04 */ Unk4Struct** monsterAiSection;
    /* 0x08 */ u32 flags;
    /* 0x0C */ u32 flagsBackup;
    /* 0x10 */ s32 volatile maxAtb;
    /* 0x14 */ s32 volatile curAtb;
    /* 0x18 */ s32 currentHp;
    /* 0x1C */ s32 maxHp;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24[8];
    /* 0x44 */ u16 elemDef[8];
    /* 0x54 */ s16 perBit[16];
    /* 0x74 */ u16 animParam1;
    /* 0x76 */ u16 animParam2;
    /* 0x78 */ u16 animParam3;
    /* 0x7A */ u8 pad7A[2];
    /* 0x7C */ ControlFlags volatile controlFlags;
    /* 0x80 */ u16 status;
    /* 0x82 */ u16 statusBackup;
    /* 0x84 */ s16 hpDisplay;     /* 0x84: HP value mirrored from BattleCharData.currentHp. */
    /* 0x86 */ u16 hitStatus1;
    /* 0x88 */ u8 unk88;
    /* 0x89 */ u8 unk89;
    /* 0x8A */ u8 unk8A;
    /* 0x8B */ u8 unk8B;
    /* 0x8C */ u8 unk8C;
    /* 0x8D */ u8 unk8D;
    /* 0x8E */ u8 unk8E;
    /* 0x8F */ u8 unk8F;
    /* 0x90 */ u8 mentalRes[40];
    /* 0xB8 */ u8 unkB8[3];
    /* 0xBB */ u8 comFileId;
    /* 0xBC */ u8 level;
    /* 0xBD */ u8 unkBD[8];
    /* 0xC5 */ u8 unkC5;
    /* 0xC6 */ u8 unkC6;
    /* 0xC7 */ u8 trigType;
    /* 0xC8 */ u8 trigKey;
    /* 0xC9 */ u8 unkC9;
    /* 0xCA */ u8 crisisLevel;
    /* 0xCB */ u8 padCB;
    /* 0xCC */ u16 unkCC;
    /* 0xCE */ u8 padCE[2];
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
} TaskEntry; /* 16 bytes */

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
} Struct_12CC; /* used in func_8009D594 */

// arrayDE8[i][j][k], i++ = 0x108, j++ = 0x18, k++ = 0xC
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
    /* 0x0000 */ BattleHeader header;
    /* 0x0010 */ BattleEntity entities[7];
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
    /* 0x1298 */ s32 unk1298[8];
    /* 0x12B8 */ u16 unk12B8[7];
    /* 0x12C6 */ u8 pad12C7[0x12CC - 0x12C6];
    /* 0x12CC */ Struct_12CC array12CC[1];          /* used in func_8009D594 */
    /* 0x12CF */ u8 pad12CF[0x12D8 - 0x12CF];
    /* 0x12D8 */ s32 unk12D8;                       /**< Cached length argument for callback. */
    /* 0x12DC */ u8* unk12DC;
    /* 0x12E0 */ s16 unk12E0;                       /**< Low 13 bits of a packed s16 field. */
    /* 0x12E2 */ u16 unk12E2;
    /* 0x12E4 */ s16 unk12E4;               
    /* 0x12E5 */ u8 pad12E5[2];                     /**< Misc state. */
    /* 0x12E8 */ u8 volatile unk12E8;               /**< Misc state byte. */
    /* 0x12E9 */ u8 volatile unk12E9;               /**< Misc state byte (touched by 12EA-gated path). */
    /* 0x12EA */ u8 volatile unk12EA;               /**< Misc state gate byte. */
    /* 0x12EB */ u8 volatile unk12EB;               /**< Misc state. */
    /* 0x12EC */ u8 volatile unk12EC;               /**< Misc state byte (init to 0xFF). */
    /* 0x12ED */ u8 volatile unk12ED;               /**< Misc state byte. */
    /* 0x12EE */ u8 volatile unk12EE;               /**< Misc state byte. */
    /* 0x12EF */ u8 volatile unk12EF;
    /* 0x12F0 */ u8 unk12F0;
    /* 0x12F1 */ u8 unk12F1;                        /* used as index for BattleEntity unkC8 in func_800AE414 */
    /* 0x12F2 */ u8 unk12F2;                        /* used as index for unkD64 and unk1100 in func_800A57E0 */
    /* 0x12F3 */ u8 unk12F3;                        /* used as index for entities in func_8009F824 */
    /* 0x12F4 */ u8 unk12F4;
    /* 0x12F5 */ u8 unk12F5;
    /* 0x12F6 */ u8 taskHead;                       /**< Head index of the task queue linked list. */
    /* 0x12F7 */ u8 unk12F7;
    /* 0x12F8 */ u8 unk12F8;
    /* 0x12F9 */ u8 unk12F9;
    /* 0x12FA */ u8 unk12FA;
    /* 0x12FB */ u8 unk12FB; // used as index for entities (max 7)
    /* 0x12FC */ u8 unk12FC; // used as index for unkD54,unkD14 (max 8)
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
    /* 0x132D */ u8 unk132D;
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
    u8 unk3;
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

/** @brief An entry of a party member's battle magic, GF or limit break list (5 bytes). */
typedef struct {
    /* 0x0 */ u8 id; /**< A magic ID, a GF's battle ID, or what the limit break lists (a Blue Magic, an ammo item...). */
    /* 0x1 */ s8 count; /**< The spell's stock or the ammo Irvine holds; 1 for anything else. */
    /* 0x2 */ u8 statusWindowFlags;
    /* 0x3 */ u8 targetInfo;
    /* 0x4 */ u8 flags; /**< MENU_ENTRY_* bits. */
} BattleMenuEntry;

/** @brief A battle command slot (4 bytes). */
typedef struct {
    /* 0x0 */ u8 cmdType; /**< Index into g_kernel.battleCommands. */
    /* 0x1 */ u8 menuFlags;
    /* 0x2 */ u8 targetInfo;
    /* 0x3 */ u8 flags; /**< MENU_ENTRY_* bits. */
} BattleCmdSlot;

/** @brief Bits of BattleMenuEntry.flags and BattleCmdSlot.flags. */
#define MENU_ENTRY_TARGETS_KO 0x01 /**< Can target KO'd units (the kernel's ATTACK_FLAG_TARGET_KO). */
#define MENU_ENTRY_UNAVAILABLE 0x02 /**< Greyed out: turned off by the battle, a GF at 0 HP, ammo Irvine doesn't hold. */
#define MENU_ENTRY_JUNCTIONED 0x04 /**< On a spell: junctioned to a stat. On the first command: a limit break is ready. */
#define MENU_ENTRY_UNK08 0x08 /**< Set on command 0x0D. */
#define MENU_ENTRY_UNK10 0x10 /**< On the Magic command: Double or Triple is active. On a limit entry: the slot is empty. */

/** @brief Battle ID of GF 0; GF n is BATTLE_GF_ID_BASE + n. */
#define BATTLE_GF_ID_BASE 0x40

/** @brief Battle character render data (g_battleChars, stride 0x1D0 = 464 bytes). */
typedef struct {
    /* 0x000 */ u8 pad0[0x008 - 0x000];
    /* 0x008 */ BattleCmdSlot subCmdSlots[3];
    /* 0x014 */ u16 unk14;
    /* 0x016 */ u16 unk16;
    /* 0x018 */ s16 currentHp;          /**< Current HP in battle. */
    /* 0x01A */ u16 unk1A;
    /* 0x01C */ u8 unk1C;
    /* 0x01D */ u8 unk1D;
    /* 0x01E */ BattleCmdSlot cmdSlots[4];
    /* 0x02E */ BattleCmdSlot limitCmdSlot; /**< The limit break command, beside the four in cmdSlots. */
    /* 0x032 */ BattleMenuEntry limitSlots[16]; /**< What the limit break lists, e.g. Quistis' Blue Magic or Irvine's ammo. */
    /* 0x082 */ BattleMenuEntry magicSlots[32];
    /* 0x122 */ BattleMenuEntry gfSlots[16]; /**< The junctioned GFs. */
    /* 0x172 */ s16 unk172;          /**< Mirrored HP cap (set with hpRegenCap when battle HP is reduced). */
    /* 0x174 */ s16 hpRegenCap;        /**< HP regen cap (field-walk tick stops when currentHp reaches this). */
    /* 0x176 */ u8 pad176[0x178 - 0x176];
    /* 0x178 */ u32 exp; /**< Total EXP. */
    /* 0x17C */ s32 xpToNext;          /**< XP needed to reach next level. */
    /* 0x180 */ u32 unk180;
    /* 0x184 */ u32 unk184;
    /* 0x188 */ u32 unk188;          /**< Status/ability mask checked for bit 0x60000. */
    /* 0x18C */ s32 abilityFlags;
    /* 0x190 */ s32 statusFlags; /**< The equipped character abilities: CHAR_ABILITY_* bits. */
    /* 0x194 */ u16 elemResistances[8];/**< Element resistance values (8 × s16). */
    /* 0x1A4 */ u8 statusResistances[13];/**< Status resistance values (13 × u8). */
    /* 0x1B1 */ u8 pad1B1;
    /* 0x1B2 */ u16 displayStatus;       /**< Mirror of BattleEntity.status; bit-flag display state. */
    /* 0x1B4 */ u16 abilityValue;
    /* 0x1B6 */ u16 atkStatusHit;      /**< Attack status hit chance. */
    /* 0x1B8 */ u8 level;              /**< Battle level (from findCharXpLevel). */
    /* 0x1B9 */ u8 unk1B9;
    /* 0x1BA */ u8 classId;            /**< Equipped weapon ID, an index into @c g_kernel.weapons. */
    /* 0x1BB */ u8 stats[8];           /**< Battle stats: STR, VIT, MAG, SPR, SPD, ?, hit (0x1C0), eva (0x1C1). 0x1C2 = ? */
    /* 0x1C3 */ u8 characterId;
    /* 0x1C4 */ u8 atkElemBase;        /**< Attack element base. */
    /* 0x1C5 */ u8 atkElemBonus;       /**< Attack element bonus. */
    /* 0x1C6 */ u8 fieldStatusByte;    /**< Status byte checked by field script (bit 1 = greyed out). */
    /* 0x1C7 */ u8 statCoefs[9];       /**< Stat coefficient table (HP, str, vit, mag, spr, spd, ?, eva, hit). */
} BattleCharData;    /* 0x1D0: 464 bytes */

/** @brief Bits of BattleCharData.statusFlags, from each character ability's kernel entry. */
#define CHAR_ABILITY_MUG 0x01 /**< Mug replaces the Attack command. */
#define CHAR_ABILITY_HP_BONUS 0x80 /**< +30 max HP per level gained. */
#define CHAR_ABILITY_STR_BONUS 0x100 /**< +1 STR per level gained. */
#define CHAR_ABILITY_VIT_BONUS 0x200 /**< +1 VIT per level gained. */
#define CHAR_ABILITY_MAG_BONUS 0x400 /**< +1 MAG per level gained. */
#define CHAR_ABILITY_SPR_BONUS 0x800 /**< +1 SPR per level gained. */

/** @brief GF battle entry (12 bytes, used for GF HP in battle). */
typedef struct {
    u8 unk0[8];
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
    u8 pad5;
    u8 unk6;
    u8 pad7[5];
} BattleLevelEntry;

typedef struct{
    u8 unk0;
    s8 unk1;
} splitStruct;

/** @brief Complete battle character/GF state block. */
typedef struct {
    /* 0x000 */ BattleCharData chars[3];          /* 3 party members × 0x1D0 */
    /* 0x570 */ u16 unk570;
    /* 0x572 */ u16 unk572;
    /* 0x574 */ u16 unk574[3];
    /* 0x57A */ u16 unk57A[3];
    /* 0x580 */ u16 unk580[16];
    /* 0x5A0 */ u16 unk5A0[16];
    /* 0x5C0 */ u16 unk5C0[16];
    /* 0x5E0 */ splitStruct unk5E0[24];
    /* 0x610 */ BattleGfEntry gfEntries[1];       /* hp sub-array (stride 12, 16 entries) */
    /* 0x61C */ u8 pad61C[0x620 - 0x61C];
    /* 0x620 */ BattleLevelEntry levelEntries[16]; /* 16 × 12 bytes */
} BattleCharState; /* 0x6E0 */


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

extern s16             D_8005F11C;
extern u8              D_8005F170;   /**< Cleared once at boot by loadKernel and set only by
                                            battle_render's entry, which only gameStateLoop state 4
                                            reaches; gates the magic menu's refill-all shortcut. */
extern BattleCharState g_battleChars; // 0x80078720
//D_80078DF8 = g_battleChars.levelEntries[15].abilityFlags
extern BattleConfig    g_battleConfig; // 0x80082C08
extern u8              D_80098030[];
extern BattleSceneCtx* D_800D244C;
extern s32             D_800E19B4[];
extern s32             D_800E19BC[];
extern u16             D_800E3CA4[];
extern BattlePosXZ     D_800E3CA8[];
extern BattlePosXZ     D_800E3CB0[];
extern u8              D_800E3CBC[];
extern u8              D_800E3CC5;
extern u8              D_800E3CC6;
extern u8              D_800E3CE8;
extern u8              D_800E3CEC[];
extern BattleSystem    D_800ED148;
extern BattleCmdBuf    D_800EE4C0;
extern BattleAnimTable D_800EE9E8; // (D_800EE9B3 = D_800EE9E8-3)
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
extern s32             D_800EEBD8;
extern s32             D_800EEBDC;
extern u8              D_800EEBE0[7];

/* ---------------------------------------------------------------- *
 *  Battle-overlay function prototypes (battle internals).
 * ---------------------------------------------------------------- */

/** @brief Apply a status flag, ORing it into the flag word. */

/** @brief Set @c timers[lowest-bit-of-a1] = -0x457 on entity @p a0. */

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
void requestPadSetup(s32 idx);

void func_800D0608(void); /* bc_object17: overlay VSync handler (RENDER_OVERLAY) */



/* ---------------------------------------------------------------- *
 * Records the effect overlays share with battle.bin
 * ---------------------------------------------------------------- */

/** @brief A second skeleton hung off a battle slot. */
struct EffectAttachment {
    /* 0x00 */ u8 pad000[0x4];
    /* 0x04 */ struct EffectMesh *mesh;
};

/**
 * @brief One battle entity slot; @ref D_800EF2D0 holds seven of them.
 *
 * Both binaries index the table: battle.bin through the symbol, the effect
 * overlays through a pointer they are handed. Three sites in battle take the
 * table address into a local before indexing -- indexing inline emits the
 * multiply ahead of the address and does not match.
 */
typedef struct BattleEffectSlot {
    /* 0x00 */ u16 flags;          /**< See @c BATTLE_SLOT_FLAG_*. */
    /* 0x02 */ u8 pad002[0xE - 0x2];
    /* 0x0E */ s16 facing;         /**< Angle the slot's model is turned to. */
    /* 0x10 */ u8 pad010[0x1C - 0x10];
    /* 0x1C */ SVECTOR pos;        /**< Where the slot's model stands; @c vy is
                                        the origin a model's Y bounds are summed
                                        with. */
    /* 0x24 */ s16 unk024;
    /* 0x26 */ s16 unk026;
    /* 0x28 */ u32 unk028;         /**< Packed RGB the textured prims are drawn with. */
    /* 0x2C */ u8 pad02C[0x36 - 0x2C];
    /* 0x36 */ u16 unk036;
    /* 0x38 */ u8 pad038[0x3C - 0x38];
    /* 0x3C */ s16 unk03C;         /**< Its distance above @c unk036 sizes the draw list. */
    /* 0x3E */ u8 pad03E[0x40 - 0x3E];
    /* 0x40 */ MATRIX mtx;         /**< Pose the effect's render matrices start from. */
    /* 0x60 */ u8 unk060[0x64 - 0x60];
    /* 0x64 */ struct EffectMesh *mesh;
    /* 0x68 */ u8 pad068[0x6C - 0x68];
    /* 0x6C */ u8 unk06C[0x74 - 0x6C];
    /* 0x74 */ u8 *unk074;         /**< The battle entity behind the slot;
                                        its @c 0x2C halfword carries the flags. */
    /* 0x78 */ struct EffectAttachment *unk078;
    /* 0x7C */ u32 unk07C;         /**< Bit @c n set: mesh part @c n is drawn. */
    /* 0x80 */ u8 pad080[0x9C - 0x80];
} BattleEffectSlot; /* 0x9C */

#define BATTLE_SLOT_FLAG_UNK02 0x2
#define BATTLE_SLOT_FLAG_UNK04 0x4
#define BATTLE_SLOT_FLAG_UNK20 0x20

/**
 * @brief The battle renderer's graphics context.
 *
 * Only the members reached from C are named; the rest of the record is still
 * battle.bin's own.
 */
typedef struct {
    /* 0x00 */ u8 pad000[0x14];
    /* 0x0014 */ u32 frontOT[(0x44 - 0x14) / 4]; /**< Ordering table the screen overlays link into. */
    /* 0x0044 */ u32 ot[(0x4040 - 0x44) / 4];   /**< Ordering table the effects link into. */
    /* 0x4040 */ u8 unk4040[4];
} BattleGfx;

/** @brief Per-entity battle records the effect overlays pose their models from. */
extern BattleEffectSlot D_800EF2D0[];

/**
 * @brief One of the four screen-tint entries battle steps every frame.
 *
 * @c level rises and falls under an effect's control; the three colour bytes
 * are also written together as one word.
 */
typedef struct {
    /* 0x00 */ u16 unk000;
    /* 0x02 */ u16 level;
    /* 0x04 */ u8 pad004[0x28 - 0x4];
    /* 0x28 */ u8 r;
    /* 0x29 */ u8 g;
    /* 0x2A */ u8 b;
    /* 0x2B */ u8 pad02B[0x2C - 0x2B];
} BattleTint; /* 0x2C */

extern BattleTint D_800EF738[];

/**
 * @name Battle state flags -- @ref D_800EEC5C
 *
 * One word the battle loop and its effects both test. The names are
 * placeholders; only the bit positions are established.
 * @{
 */
#define BATTLE_STATE_UNK001 0x1
#define BATTLE_STATE_UNK100 0x100
#define BATTLE_STATE_UNK200 0x200
/** @} */

/** @brief State the battle loop and its effects share; see the flags above. */
extern s32 D_800EEC5C;

/** @brief Angle every sprite prim is rolled by unless it opts out. */
extern s16 D_800F02A0;

extern s16 D_800F02C2;

/** @brief The world matrix for this frame; effect matrices compose onto it. */
extern MATRIX D_800F02C8;

/** @brief The battle renderer's graphics context. */
extern BattleGfx *D_800FA5E8;

/** @brief Head of the prim list a posed battle model links into. */
extern s32 D_800FA5F0;


#endif /* BATTLE_H */
