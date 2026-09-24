#ifndef BATTLE_ANIM_H
#define BATTLE_ANIM_H

#include "common.h"

/* Battle animation state shared across the battle, field, menu, and Triple Triad
 * code. g_battleAnims is a main-RAM global (0x80082DD0); the Triple Triad minigame
 * reuses the battle animation entity system to drive the on-board card animations,
 * so its overlay accesses these same structures. Kept here (rather than battle.h)
 * so non-battle translation units can use them without depending on battle.h. */

typedef struct {
    u8 field00;
    u8 field01;
    u16 field02;
    u8 params[4]; /**< 0x04-0x07: Indexed by param in getAnimFrameSlotParam. */
    u16 field08;
    u16 field0A;
    u16 field0C;
    u16 field0E;
    u16 field10;
    u16 field12;
} AnimFrame; /* 0x14 = 20 bytes */

typedef struct {
    u8 field00;
    u8 field01;   /**< field06 masked by opacity, shifted right by field08 (func_80027220). */
    u8 field02;   /**< field07 masked by opacity, shifted right by field09 (func_80027220). */
    u8 pad03[3];
    u8 field06;
    u8 field07;
    u8 field08;   /**< Shift amount applied to field06 in func_80027220. */
    u8 field09;   /**< Shift amount applied to field07 in func_80027220. */
    u8 field0A;
    u8 field0B;
    u8 field0C;
    u8 field0D;
    u8 field0E;
    u8 field0F;
    s16 unk10[4];
    u8 frameCounter;
    s8 field19;   /**< Read back as a signed byte in func_80027220. */
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
    /* 0x1E1 */ u8 pad1E1[0x59];             /**< Unknown. */
    /* 0x23A */ s16 field23A;                /**< Saved as SFX volume across transition. */
    /* 0x23C */ u8 pad23C[0x14];             /**< Unknown. */
    /* 0x250 */ u16 field250;                /**< Saved/cleared across transition. */
    /* 0x252 */ u8 field252;                 /**< Saved/cleared across transition. */
    /* 0x253 */ u8 pad253[0x3ED];            /**< Unknown. */
    /* 0x640 */ DisplayListBuf bufs[2];         /**< Double-buffered GPU display lists (2 × 0x58). */
    /* 0x6F0 */ DisplayListBuf *active;      /**< Pointer to active display list buffer. */
    /* 0x6F4 */ s32 halfSize;                /**< Half of total VRAM size. */
    /* 0x6F8 */ u8 pad6F8[4];                /**< Unknown. */
    /* 0x6FC */ s32 field6FC;                /**< Cleared during GPU init. */
    /* 0x700 */ u8 pad700[3];                /**< Unknown. */
    /* 0x703 */ u8 field703;                 /**< Saved/cleared across transition. */
    /* 0x704 */ u8 pad704[0x270];            /**< Unknown. */
    /* 0x974 */ s32 palette[3];              /**< RGB888 palette (0x40BBGGRR). */
    /* 0x980 */ u8 pad980[0x30];             /**< Unknown. */
    /* 0x9B0 */ u8 field9B0;                 /**< Saved/cleared across transition. */
    /* 0x9B1 */ u8 pad9B1[0xF];              /**< Unknown. */
    /* 0x9C0 */ u8 field9C0;                 /**< Saved/cleared across transition. */
    /* 0x9C1 */ u8 pad9C1[1];                /**< Unknown. */
    /* 0x9C2 */ s16 field9C2;               /**< Set to 0x4611 during GPU init. */
    /* 0x9C4 */ s16 cdStreamCounter;         /**< CD stream counter. */
    /* 0x9C6 */ u8 pad9C6[2];                /**< Unknown. */
    /* 0x9C8 */ s32 field9C8;                /**< Cleared during GPU init. */
    /* 0x9CC */ s32 field9CC;                /**< Cleared during GPU init. */
} BattleAnimState;

extern BattleAnimState g_battleAnims;

#endif /* BATTLE_ANIM_H */
