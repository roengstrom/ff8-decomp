#ifndef WORLD_WE_OBJECT9_H
#define WORLD_WE_OBJECT9_H

#include "common.h"
#include "game.h"
#include "world.h"
#include "psxsdk/libgte.h"

/* Particle source: position + velocity (with 4-byte gap between). Stride 0x28.
   func_800AC0A0 takes these two as VECTOR/SVECTOR; the spawner reads .vy of
   each as the heading and .vz of the velocity as the height. */
typedef struct {
    VECTOR  pos;
    u32     pad10;
    SVECTOR vel;
} ParticleSource;

typedef struct {
    s32 a;
    s32 b;
} U64;

/**
 * @brief Second relocatable offset-table pointing at template strings.
 *
 * Same layout as @c StringTable but used by @c func_800BCA74 for template
 * format strings containing @c 0x0A marker bytes.
 */
typedef struct {
    s32 offsets[1];
} TemplateTable;

typedef struct { s32 x, y, z; } Vec3;

/** Lookup table entry: matches an (id, param) pair. */
typedef struct {
    s16 id;
    u8 param;
    u8 pad3;
} LookupEntry;

/** Range into the lookup-entry pool (offsets from blob base). */
typedef struct {
    s32 start_off;
    s32 end_off;
} SubRange;

/** Scatter table: 5-bit scatter key -> up to 4 sub-range indices. */
typedef struct {
    s8 ranges[4];
} ScatterEntry;

/** Lookup blob at @c D_800C96F8: range directory, entry-pool offset, then pool. */
typedef struct {
    SubRange ranges[5];       /* 0x00: table ranges, indexed by scatter entry */
    s32      entry_base_off;  /* 0x28: offset from blob base to first pool entry */
} LookupBlob;

/**
 * @brief 32-byte record with two signed sentinel bytes near its tail.
 */
typedef struct {
    u8  pad00[3];
    u8  byte3;          /**< 0x03: zone byte, must be 1 or 2 for a match. */
    u8  pad04[0x1A];
    s8  sb1E;           /**< 0x1E: signed sentinel byte (-1 = open slot). */
    s8  sb1F;           /**< 0x1F: signed marker byte (0, 1, ...). */
} SlotTarget;

/* File-private byte-array view of the game-state block (unsized to force
 * self-expanded global addressing for matching codegen). */

extern s32             D_800C96CC;
extern TemplateTable  *D_800C9FE8;
extern ScatterEntry    D_800C5D70[];     /* scatter table, 32 entries */
extern LookupBlob     *D_800C96F8;       /* pointer to lookup blob */
extern u32             D_80077E84;
extern VECTOR          D_800DD680;
extern u8              D_800DD690[8];
extern s32             D_800DD698;
extern s32             D_800DD69C;
extern s32             D_800C971C;

/* Per-scene angle bias applied to a placed sprite fan. */
extern s32 func_800BC5E0(s32 ctx);
extern void func_800BFFEC(void);
extern void func_800C2C00(s32 a, s32 b, s32 c, s32 d, s32 e, U64 *f, s32 g, U64 *h);
extern u8 *func_800BC8D8(u8 *buf, s32 magicId);
extern u8 *func_800BC974(u8 *buf, s32 id);
extern void func_80047C74(u8 *dst, u8 *src);
extern u8 *func_800BCE74(u8 *buf, s32 statId);
extern s32 func_800B00D8(s32 a);

extern void func_800BB150(Slot *slot);
extern void func_800BB4E8(void);

extern void func_800BC51C(VECTOR *src, VECTOR *dst);

extern void func_800BC544(VECTOR *src, VECTOR *dst);

#endif /* WORLD_WE_OBJECT9_H */
