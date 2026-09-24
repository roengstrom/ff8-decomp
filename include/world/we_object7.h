#ifndef WORLD_WE_OBJECT7_H
#define WORLD_WE_OBJECT7_H

#include "common.h"
#include "world.h"

/**
 * @brief 0x34-byte tracking entry — one world-space target with screen-space
 * projection state and a relative neighbour link.
 *
 * Populated by @c func_800B01A0 (which fills @c screen plus @c unk30 and
 * @c unk2C from the world transform pipeline) and
 * consumed by @c func_800B3868 to compute pitch/yaw to a chained target.
 */
typedef struct {
    VECTOR pos;          /**< 0x00: world position; func_800B01A0 projects it. */
    SVECTOR screen;      /**< 0x10: projected screen-space position (x, y, z). */
    s16 unk18;           /**< 0x18: roll/twist angle — zeroed when target valid. */
    s16 yaw;             /**< 0x1A: yaw angle to target (atan2 + 0x800 bias). */
    s16 pitch;           /**< 0x1C: pitch angle to target. */
    u8 pad1E[0x0E];
    s16 unk2C;           /**< 0x2C: scratch halfword filled by func_800B01A0. */
    s8 targetIdx;        /**< 0x2E: relative index of the chained target entry (0 = none). */
    s8 status;           /**< 0x2F: -1 = uninitialized, 1 = visible/active. */
    s8 unk30;            /**< 0x30: secondary status byte. */
    u8 pad31[0x03];
} TrackEntry;

/** @brief Header for a TrackEntry array — count plus pointer-to-entries. */
typedef struct {
    u8 pad00[0x02];
    s8 count;            /**< 0x02: number of entries pointed to by @c entries. */
    u8 pad03[0x15];
    TrackEntry *entries; /**< 0x18: pointer to the entry array. */
} TrackObj;

/* File-private world data. */
extern u8 *D_800C96C8;

/* func_8009B358/func_8009B550 are defined in we_object1, func_8009D8A8 in
 * we_object2; addItemToInventory, func_800A5DC8 and ratan2 are
 * main-binary. Caller-local prototypes. */
extern s32 ratan2(s32 y, s32 x);
extern void func_8009B550(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6);
extern void addItemToInventory(s32 itemId, s32 count);

extern s32  func_800B4AA0(void);
extern void func_800B56A0(void);
extern void func_800B7240(s8 v);
extern void func_800B881C(void);
extern void func_800B893C(void);

extern void func_800B3FD4(Slot *slot, s32 mode);

#endif /* WORLD_WE_OBJECT7_H */
