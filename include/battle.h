#ifndef BATTLE_H
#define BATTLE_H

#include "common.h"
#include "psxsdk/libgpu.h"

typedef struct {
    u8 field00;
    u8 field01;
    u16 field02;
    u8 field04;
    u8 field05;
    u8 field06;
    u8 field07;
    u16 field08;
    u16 field0A;
    u16 field0C;
    u16 field0E;
    u16 field10;
    u16 field12;
} AnimFrame;

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

/** @brief Complete battle animation state (entities + global coords). */
typedef struct {
    BattleAnimEntity entities[2]; /**< 0x000-0x187: Two animation entities. */
    u8 pad188[0x48];             /**< 0x188-0x1CF: Unknown. */
    s16 globalCoords[2][2];      /**< 0x1D0: Per-slot coords [slot][axis], x=0 y=1. */
} BattleAnimState;

extern BattleAnimState g_battleAnims;

typedef struct {
    s32 field00;
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
    s32 dataPtr;
    s32 dataPtrCopy;
    s16 pitch;
    u16 field12;
    s16 field14;
    u8 state;
    u8 pad17;
    u8 entityIdx;
    u8 field19;
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
    u8 counter;
    u8 pad04[8];
    s8 activeFlag;
    u8 pad0D[7];
    u8 field14;
    u8 field15;
    u8 pad16[2];
    u16 field18;
    u16 field1A;
} SfxGlobalState;

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

/** @brief Display list double-buffer entry (stride 0x58 = 88 bytes). */
typedef struct {
    s32 pktAlloc;
    s32 pktLimit;
    u32 ot[18];
    u8 pad50[4];
    s32 pktBase;
} DisplayListBuf;

/** @brief Battle command table entry (D_80083878, stride 0x24 = 36 bytes). */
typedef struct {
    u8 data[0x20];
    u16 sourceId;
    s8 active;
    u8 index;
} BattleCmdEntry;

/** @brief Memory card subsystem data block (D_80082FB4). */
typedef struct {
    s32 events[8];
    u8 pad20;
    u8 statusByte;
    u8 pad22[2];
    u8 cmdBytes[8];
    u8 status[2][4];
    u8 statusAlt[2][4];
} CardDataBlock;

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

extern BattleCharData g_battleChars[];

#endif /* BATTLE_H */
