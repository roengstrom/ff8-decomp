#ifndef WORLD_H
#define WORLD_H

#include "common.h"

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

#endif /* WORLD_H */
