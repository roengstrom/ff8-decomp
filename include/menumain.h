/**
 * @file menumain.h
 * @brief Symbols and types owned by the menumain overlay unit.
 *
 * The menumain overlay (loaded at @c 0x801EF800) hosts the main in-game
 * menu: panel/window rendering, the shared text lookup/render helpers used
 * by the menu sub-overlays, party management and the menu state machine.
 *
 * Cross-overlay shared types (e.g. @c MenuDisplayConfig) live in
 * @c include/menu.h. Menu sub-overlays that call into menumain at fixed
 * addresses keep their own file-local prototypes (overlay-conflict rule).
 */
#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "common.h"
#include "character.h"
#include "psxsdk/libgpu.h"

/* ======================================================================== */
/* Public typedefs/structs                                                  */
/* ======================================================================== */

/* (none yet) */

/* ======================================================================== */
/* Public prototypes                                                        */
/* ======================================================================== */

DR_AREA *func_801EF800(P_TAG *, DR_AREA *, u8 *);  /**< Link a custom-rect DR_AREA prim. */
DR_AREA *func_801EF8D8(P_TAG *, DR_AREA *);   /**< Link a clip-restore DR_AREA prim. */
s32  func_801EF9AC(s32, s32, s32, s32);       /**< Panel/window render helper. */
s32 func_801EFBB4(s32, s32, s32);            /**< Panel/window render helper. */
s32  func_801F179C(s32, s32);                 /**< Allocate a menu state task (tickCb, drawCb). */
s32  func_801F6AD0(s32);                      /**< Party/character query helper. */
void func_801F7B60(void);                     /**< Menu teardown/exit helper. */

/* ======================================================================== */
/* Private typedefs/structs                                                 */
/* ======================================================================== */

/** @brief 16-byte block used to copy stat tables word-wise (func_801F537C). */
typedef struct { s32 w0, w1, w2, w3; } CopyBlock16;

/**
 * @brief Menu task node: pool slot linked into the active-task ring.
 *
 * Allocated from the 10-slot pool D_801FA550 by func_801F179C/func_801F1850
 * and walked by two drivers: func_801F1584 calls @c tickCb and the frame
 * driver func_801F16AC calls @c drawCb.
 */
typedef struct MenuTask {
    struct MenuTask *next;                        /* 0x00 */
    struct MenuTask *prev;                        /* 0x04 */
    s32 tickCb;                                   /* 0x08: update callback (func_801F1584). */
    s32 (*drawCb)(struct MenuTask *, s32, s32);   /* 0x0C: (task, ctx, dl) -> dl (func_801F16AC). */
    u16 state;                                    /* 0x10: per-task state word, zeroed on alloc. */
    s8 inUse;                                     /* 0x12: pool slot occupied. */
    u8 pad13[0x15];
    u32 unk28;
    u8 *unk2C;
    u32 unk30;
    u16 pad34;
    u16 unk36;
    u16 unk38;
    u8 pad3A[0xB];
    u8 unk45;
    u8 pad46[0x32];
} MenuTask; /* 0x78 bytes */

/**
 * @brief Queued text-glyph draw request (see func_801F0954).
 * @note Field meanings inferred from the render-call chain; @c code is the
 *       character/glyph code and @c color the D_801FAAE0 color/font parameter.
 */
typedef struct {
    s16 x;      /**< 0x0: screen X (stored pre-offset by -26). */
    s16 y;      /**< 0x2: screen Y (stored pre-offset by +3). */
    u8  code;   /**< 0x4: character/glyph code. */
    u8  pad05;
    s16 color;  /**< 0x6: color/font parameter. */
} TextQueueEntry; /* 0x8 bytes */

/** @brief Face-grid members: the switchable party (not Squall or Edea). */
#define FACE_GRID_MEMBERS ((1 << CHAR_ZELL) | (1 << CHAR_IRVINE) | (1 << CHAR_QUISTIS) | \
                           (1 << CHAR_RINOA) | (1 << CHAR_SELPHIE) | (1 << CHAR_SEIFER))

/* ======================================================================== */
/* Private prototypes                                                       */
/* ======================================================================== */

void func_801EFF64(void);
DR_AREA *func_801F03E8(P_TAG *, DR_AREA *);
u8  *func_801F08AC(u8 *, s32);
s32  func_801F0AC8(s32, s32);
s32  func_801F0F20(s32, SPRT *, s32, u32, s32);
s32  func_801F16AC(s32, s32);
MenuTask *func_801F1850(s32, s32);
s32  func_801F2FAC(s32, s32);
void func_801F39D0(s32, s32, s32, s32, s32, s32);
void func_801F7AD4(s32);
void func_801F0954(s32, s32, s32, s32);
s32  func_801F1A40(s32);
s32  func_801F42A4();
s32  func_801F3FE8();
s32  func_801F4744();
s32  func_801F6234(s32, s32, s32, s32, s32);
s32  func_801F6358(s32, s32, s32, s32, s32);
void func_801F66B0(s32, s32, s32, s32, s32);
s32  func_801F6B54(s32, s32, s32, s32, s32);
void func_801F6C9C(s32, s32, s32, s32, s32, u16);
u16  func_801F22F4(void);
void func_801F23D0(s32, s32, u8 *);
u16  func_801F2370(void);
void func_801F2458(s32);
void func_801F798C(void);
s32  func_801F64A4(s32, s32, s32, s32, s32, s32, s32);
s32  func_801F3DE4(s32, s32, s32, s32, s32, s32, s32);
s32  func_801F5E0C(s32, s32, s32, s32, s32, s32, s32);
s32  func_801F5F60(s32, s32, s32, s32);
s32  func_801F605C(s32, s32, s32, s32, s32);
void func_801F4A98();
void func_801F5490(s32);
s32  func_801F5B54(s32, s32, s32, s32, s32, u16 *, s32);
s32  func_801F5C84(s32, s32, s32, s32, s32, u16 *, u32);
s32  func_801F5D5C(s32, DR_AREA *, s32, s32, s32, u16 *, s32, s32);
void func_801F202C(void);
void func_801F72C4(void);
s32  func_801F738C(s32);
s32  func_801F7394(s32);

/* Main-executable helpers without an owner header yet. */
void func_80027C00(s32, s32);
void func_80027C90(s32, s32);
s32  getDisplayListHead(void);
void storeGpuPacket(u32);

/* ======================================================================== */
/* Data (unit-owned, menumain overlay region)                               */
/* ======================================================================== */

extern u8  D_801F7DF4;
extern u8  D_801F7E00;
extern u8  D_801F7E0C;
extern u8  D_801F7E6C[];
extern u8  D_801F7F74[];        /**< Fallback string for failed text lookups. */
extern u8  D_801F7F78[];
extern u8  D_801F7F98[];
extern u8  D_801F7FB0[];        /**< String table base (two-level offset table). */
extern u8  D_801F87B8;
/** @brief Item table entry: four bytes read individually by the item menu. */
typedef struct {
    u8 b0, b1, b2, b3;
} ItemEntry4;

extern ItemEntry4 D_801F889C[];
extern u8  D_801F8BB8[];
extern u8  D_801FA278;
extern u8  D_801FA279;
extern u8  D_801FA27A;
extern u8  D_801FA27B;
extern u8  D_801FA27C;
extern u8  D_801FA280[];
extern MenuTask *D_801FA450;    /**< Head of the active menu-task list. */
extern MenuTask D_801FA4D0;     /**< Menu-task list head node. */
extern MenuTask D_801FA550[10]; /**< Menu-task pool. */
extern s32 D_801FA3C0;
extern u8  D_801FAA10;
extern s8  D_801FAA11;
extern u16 D_801FAA12;
extern u16 D_801FAA14;
extern u8  D_801FAA18[];
extern s16 D_801FAA1E;          /**< Text queue count / blink phase (added to g_menuDisplayCfg.animCounter). */
extern TextQueueEntry D_801FAA20[]; /**< Queued text-glyph draw requests. */
extern s16 D_801FAAE0;          /**< Current text color/font parameter. */
extern s32 D_801FAAE4;
extern s32 D_801FAAE8;
extern u8  D_801FAAF0;
extern u16 D_801FAAF2;
extern u8  D_801FAAF8;
extern s16 D_801FAB28;
extern s16 D_801FAB2A;
extern s32 D_801FAB2C;
extern u8  D_801FAB30;
extern u8  D_801FAB31;
extern u8  D_801FAB38[];
extern s16 D_801FAB78;
extern u8  D_801FAB7A;
extern u8  D_801FAB7B;
extern u8  D_801FAB7C;
extern u8  D_801FAB88[];
extern u8  D_801FABC4[4];
extern u8  D_801FABC8[];
extern u16 D_801FACE2;
extern u16 D_801FACE4;
extern s32 D_801FACE8;
extern u16 g_menumain_partyMemberMask;

#endif /* MENUMAIN_H */
