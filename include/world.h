#ifndef WORLD_H
#define WORLD_H

#include "common.h"
#include "psxsdk/libgpu.h"

/**
 * @brief World-engine object node — singly-linked, keyed by s16 id.
 *
 * Callers use the @c sectionIdx at +0x0A to index into the world data
 * region base @c D_800C4D5C (stride 0x9000 bytes).
 */
typedef struct WorldObject {
    struct WorldObject *next;   /**< 0x00: Next node. */
    s16 id;                     /**< 0x04: s16 id used by search. */
    u8 pad06[4];
    u8 sectionIdx;              /**< 0x0A: Section index into the D_800C4D5C region table. */
} WorldObject;

/**
 * @brief One 0x9000-byte section of the world data region table.
 *
 * Pointed-to by @c D_800C4D5C as an array indexed by @c sectionIdx.
 * The section begins with a 1-byte @c key (used by lookup helpers to
 * match sections), followed by a u32 offset/handle array starting at
 * offset 4. The rest of the section contains the data that the offsets
 * index into.
 *
 * Known fields:
 *   - @c key at offset 0x00 — section type/key byte (0xFF when empty).
 *   - @c offsets[] at offset 0x04 — u32 array; low 2 bits may carry flags.
 */
typedef struct {
    /* 0x00 */ u8 key;               /**< Section type/key byte. */
    /* 0x01 */ u8 pad01[3];
    /* 0x04 */ u32 offsets[9215];    /**< Fills section up to 0x9000 total. */
} WorldSection;

/**
 * @brief World-engine command descriptor (16 bytes).
 *
 * Used by dispatch functions that peek the current command's
 * @c type / @c flag / @c param. The current active descriptor is pointed
 * to by D_800C4D64, D_800C4D68, D_800C4D6C (three slots).
 */
typedef struct {
    u8 unk_00[0xD];
    u8 type;        /**< 0x0D: Command type byte. */
    u8 flag;        /**< 0x0E: Command flag byte. */
    u8 param;       /**< 0x0F: Command parameter byte. */
} CmdDesc;

/**
 * @brief Slot entry in the D_800DBFB8 table (stride 40 bytes).
 *
 * Each entry describes one battle/world slot. Fields are added as
 * usages are discovered.
 */
typedef struct {
    /* 0x00 */ s32 x;               /**< Slot world X coordinate. */
    /* 0x04 */ s32 y;               /**< Slot world Y (compared to world camera angle). */
    /* 0x08 */ s32 z;               /**< Slot world Z coordinate. */
    /* 0x0C */ s32 pad0C;
    /* 0x10 */ s8 marker;           /**< Scan terminator / type byte. */
    /* 0x11 */ u8 pad11;
    /* 0x12 */ s8 lookupIdx;        /**< Index into D_800DDB00 when >= 0. */
    /* 0x13 */ u8 pad13;
    /* 0x14 */ u8 data14[0x14];     /**< Tail buffer passed to dispatcher. */
} SlotEntry; /* 0x28 = 40 bytes */

/**
 * @brief Lookup target reached via D_800DDB00[SlotEntry.lookupIdx].
 *
 * Only the @c field52 u16 is known so far.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x52];
    /* 0x52 */ u16 field52;
} LookupTarget;

/**
 * @brief 4-byte scene state block at D_80082C8C.
 *
 * Accessed by several world_engine and field_engine routines; stores
 * the current scene mode, dispatch code, and two marker bytes.
 */
typedef struct {
    /* 0x00 */ s8 mode;         /**< Scene mode / kind. */
    /* 0x01 */ s8 cmd;          /**< Current dispatch code (copy of D_800C4D38). */
    /* 0x02 */ s8 unk02;        /**< Marker — set to -1 on reset. */
    /* 0x03 */ s8 unk03;        /**< Marker — set to -1 on reset. */
} SceneState;

/**
 * @brief 12-byte keyed record used by D_800C9880's packed lookup buffer.
 *
 * Only @c key at +0x02 is known; the surrounding bytes are likely a
 * (key, value) descriptor but remain unmapped.
 */
typedef struct {
    u8 pad00[2];
    u8 key;
    u8 pad03[9];
} Entry12;

/**
 * @brief Packed length-prefixed buffer of Entry12 records.
 *
 * @c length is the total buffer length in bytes including the 4-byte
 * header. Records start at @c entries[0] and run until @c length bytes.
 */
typedef struct {
    u32 length;
    Entry12 entries[1];
} KeyBuffer;

/* world_engine globals (we_object7) */
extern s32 D_800C4D2C;
extern s32 D_800C4D40;
extern s32 D_800C4D44;
extern s32 D_800C4D58;
extern s32 D_800C4D70;
extern s32 D_800C4D98;
extern s32 D_800C4D9C;
extern s32 D_800C4DC0;
extern s32 D_800C4DC4;
extern s32 D_800C4DC8;
extern s32 D_800C5924;
extern s32 D_800C5B50;
extern s32 D_800C5B54;
extern s32 D_800C5B58;
extern s32 D_800C5BFC;
extern s32 D_800C5C04;
extern s32 D_800C5C18;
extern s32 D_800C5C1C;
extern s32 D_800C5C20;
extern s32 D_800C5C24;
extern s32 D_800C5C28;
extern s32 D_800C5C2C;
extern s32 D_800C5C30;
extern s32 D_800C5C38;
extern s32 D_800C5C3C;
extern s32 D_800C5C40;
extern s32 D_800C5D54;
/**
 * @brief Pointer to the dialogue/action table buffer.
 *
 * The buffer begins with a u32 byte-offset table indexed by subject id;
 * each offset points (within the same buffer) to a 0-terminated list of
 * u32 byte-offsets to the actual entry payloads.
 */
extern u8 *D_800C976C;
extern u8  D_800C5984[];
extern u16 D_800C5C44[];
extern s32 D_800C97A4;
extern s32 D_800C9878;
extern s16 D_800C987C;
extern u8  D_800D23D8[];
extern s32 D_800DCB48;

#endif /* WORLD_H */
