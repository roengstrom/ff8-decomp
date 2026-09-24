#ifndef WORLD_WE_OBJECT10_H
#define WORLD_WE_OBJECT10_H

#include "common.h"
#include "world.h"
#include "world/we_object3.h"
#include "psxsdk/libgte.h"

/** @brief 32-byte record used by func_800BE5F8's table walker. */
typedef struct {
    s16 marker;         /**< 0x00: Scan terminator (-1 = end, -2 = continuation). */
    u8 pad02[0x1E];     /**< 0x02: Rest of the record — payload not yet mapped. */
} Record;

/**
 * @brief 0x34-byte transform entry — only the snapshot pair at @c 0x20
 *        and @c 0x28 is mapped here. The trailing fields are consumed
 *        by the per-entry handler @c func_800AEB58.
 */
typedef struct {
    /* 0x00 */ u8 pad0[0x20];
    /* 0x20 */ s32 prevWord;    /**< Snapshot of @c currWord, refreshed each tick. */
    /* 0x24 */ s32 currWord;
    /* 0x28 */ u16 prevHalf;    /**< Snapshot of @c currHalf. */
    /* 0x2A */ u16 currHalf;
    /* 0x2C */ u8 pad2C[0x8];
} XformEntry; /* 0x34 */

/**
 * @brief Header for the transform-entry table walked by @c func_800BE69C.
 *
 * Only the @c count byte at @c 0x02 and the @c entries pointer at
 * @c 0x18 are mapped — the surrounding padding holds fields used by
 * other handlers in the same overlay.
 */
typedef struct {
    /* 0x00 */ u8 pad0[2];
    /* 0x02 */ s8 count;
    /* 0x03 */ u8 pad3[0x15];
    /* 0x18 */ XformEntry *entries;
} XformGroup;

#define OBJ_SLOT_COUNT 0x40 /* id slots scanned */
#define FIRST_OBJ_ID   0x40 /* ids below this are skipped */
#define OBJ_LIST_END   0xFF /* terminator id */

/** Object-id list: a 6-byte header followed by a 0xFF-terminated id array. */
typedef struct {
    u8 header[6];           /* header (purpose not yet identified) */
    u8 ids[OBJ_SLOT_COUNT]; /* object ids present in this list */
} WorldObjList;

extern u8             g_chocoboWorld;   /* main-binary ChocoboWorldData (g_gameState + 0x1360) */
extern u8             D_800DCE78[];
extern s32            D_800C5DAC;
extern LookupTarget  *D_800DDB00[];
extern u8            *D_800C9E34;

extern void func_800BAC84(u8 *p);
extern s32  func_800BA870(u8 *p);
extern void *memset(void *dst, s32 c, u32 n);
extern s32 func_800BEF6C(void);
extern void func_800AEB58(XformEntry *entry, XformGroup *group);
extern void func_8009C5FC(s32 *data);

extern void func_800BEC60(u8 *flags);

extern void func_800BF20C(CmdDesc *p, s32 kind, s32 arg);
extern void func_800BF2E8(GlyphHeader *p, s32 *ot, WorldPrimBank *pools);

#endif /* WORLD_WE_OBJECT10_H */
