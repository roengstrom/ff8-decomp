/**
 * @file menuabl.h
 * @brief Public symbols and types owned by the menuabl overlay.
 *
 * The menuabl overlay (loaded at @c 0x801E2800) implements the in-game
 * "GF abilities" submenu and a sound/music browser sub-mode. This header
 * declares the structs that describe its in-memory state and externs the
 * functions and data that other translation units within the overlay
 * (menuabl.c, menuabl_data.c) link against.
 *
 * Cross-overlay shared types (e.g. @c MenuDisplayConfig) live in
 * @c include/menu.h.
 */
#ifndef MENUABL_H
#define MENUABL_H

#include "common.h"

/* ======================================================================== */
/* Types                                                                    */
/* ======================================================================== */

/** @brief 48-byte ability-menu slot; only @c angle at @c +0x2E is read here. */
typedef struct {
    u8  pad00[0x2E];
    s16 angle;
} MenuSlot;

/** @brief 8-byte ability-menu entry record. */
typedef struct {
    u8 pad00[5];
    u8 status;     /**< 0xFF = empty, 0x80/0x81 = state-specific (see @c func_801E36AC). */
    u8 pad06[2];
} AbilityEntry; /* 0x8 bytes */

/**
 * @brief Context passed to the ability-list panel configurators.
 *
 * Layout pieced together from @c func_801E3630 (uses @c items as dataPtr)
 * and @c func_801E381C (uses the whole ctx as dataPtr, plus @c pageStart
 * / @c pageEnd for scroll bookkeeping).
 */
typedef struct {
    u8  pad00[0x20];
    u8  items[0x12];   /**< 0x20: item table (strings/pointers, passed as cfg.dataPtr). */
    u16 scrollOff;     /**< 0x32: scroll offset source (copied to cfg.scrollOffset). */
    u8  pad34[2];
    u8  pageStart;     /**< 0x36 */
    u8  pageEnd;       /**< 0x37 */
} AbilityListCtx;

/** @brief Ability menu state struct registered via @c func_801F179C. */
typedef struct {
    u8  pad00[0x20];
    s32 dataPtr;       /**< 0x20: cleared at init, filled by state machine. */
    u8  pad24[4];
    s32 abilityMask;   /**< 0x28: result of @c func_801F72B4. */
    s16 state;         /**< 0x2C */
    s16 displaySize;   /**< 0x2E: fixed to 0x1000 at init. */
} AbilityMenuState;

/**
 * @brief Sound/music selection menu state used by @c func_801E2A34.
 *
 * 26-state machine handling track navigation, audio playback, and page
 * transitions. @c field_3A is the primary grid index, @c field_3B the
 * secondary; both are computed via div/mod by 11 throughout.
 */
typedef struct {
    u8  pad_00[0x10];
    u16 state;        /**< 0x10: state machine state (0-25). */
    u8  pad_12[0xE];
    s32 field_20;     /**< 0x20 */
    s32 field_24;     /**< 0x24 */
    u8  pad_28[0x4];
    s16 field_2C;     /**< 0x2C: fade scale (0..0x1000). */
    s16 field_2E;     /**< 0x2E: fade scale (secondary). */
    s16 field_30;     /**< 0x30: countdown timer. */
    s16 field_32;     /**< 0x32: page-scroll accumulator. */
    s16 field_34;     /**< 0x34: secondary scroll accumulator. */
    u8  field_36;     /**< 0x36: current page (primary). */
    u8  field_37;     /**< 0x37: previous page (primary). */
    u8  field_38;     /**< 0x38: current page (secondary). */
    u8  field_39;     /**< 0x39: previous page (secondary). */
    u8  field_3A;     /**< 0x3A: grid index (primary). */
    u8  field_3B;     /**< 0x3B: grid index (secondary). */
    u8  field_3C;     /**< 0x3C: mode/sub-state. */
    u8  field_3D;     /**< 0x3D: last track id. */
} SoundMenuState;

/* ======================================================================== */
/* Data symbols                                                             */
/* ======================================================================== */

/** @brief GF kind table indexed by GF id (defined in @c menuabl_data.c). */
extern const u8 D_801E3D70[20];

/** @brief Ability id list indexed by visible-slot (0..@c D_801E3D9C-1). */
extern u8 D_801E3D84[];

/** @brief Visible ability count (length of @c D_801E3D84). */
extern u8 D_801E3D9C;

/** @brief GF index list filled by @c func_801E2990 (length of @c D_801E3DB8). */
extern u8 D_801E3DA4[];

/** @brief Visible GF count (length of @c D_801E3DA4). */
extern u8 D_801E3DB8;

/* ======================================================================== */
/* Functions                                                                */
/* ======================================================================== */

void          func_801E2800(s32 ctx, s32 i, s32 unused, MenuSlot *slot);
void          func_801E28B4(s32 a0, s32 a1, s32 a2);
AbilityEntry *func_801E2920(s32 idx);
s32           func_801E2934(void);
s32           func_801E2944(s32 a0);
void          func_801E2990(void);
void          func_801E2A34(SoundMenuState *s);
s32           func_801E3530(s32 a0, s32 a1, s16 a2, s16 a3);
s32           func_801E3580(s32 ctx, s32 state, s32 idx, s32 unk3, s32 yOff);
s32           func_801E3630(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg);
s32           func_801E36AC(s32 ctx, s32 pkt, s32 col, s32 row, s32 scrollOffset);
s32           func_801E381C(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg);
s32           func_801E3904(s32 ctx, s32 pkt, s32 col, s32 row, s32 scrollOffset);
s32           func_801E39E0(SoundMenuState *s, s32 ot, s32 a2, s32 a3, s32 stackArg);
s32           func_801E3AE0(SoundMenuState *s, s32 a1, s32 a2);
void          func_801E3C28(void);
void          func_801E3C9C(void);

#endif /* MENUABL_H */
