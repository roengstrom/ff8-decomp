#ifndef CD_H
#define CD_H

#include "common.h"

/** @brief CD-ROM file descriptor (sector + size pair).
 *
 *  Used by func_8003882C / func_80038868 to identify data on disc.
 *  The master file table at 0x80097400 is an array of these descriptors,
 *  loaded from a hard-coded bootstrap descriptor (g_fileTableDesc).
 */
typedef struct {
    u32 sector; /**< 0x00: Starting CD sector number. */
    u32 size;   /**< 0x04: Data size in bytes. */
} CdFileDesc;   /* 0x08 = 8 bytes */

/**
 * @brief CD-ROM subsystem state (D_8008A3D8).
 *
 * Tracks the state machine for CD read operations, including sector
 * parameters, destination buffer, callback pointer, and error counters.
 */
typedef struct {
    /* 0x00 */ u8 flags;           /**< CD status flags (bit 1 = busy). */
    /* 0x01 */ u8 state;           /**< State machine phase (0=idle, 0xB=complete, 0xC=reset). */
    /* 0x02 */ u8 discId;          /**< Detected disc number (1-4). */
    /* 0x03 */ u8 pad03;
    /* 0x04 */ u8 cdParams[4];     /**< CdControl parameters (D_8008A3DC). */
    /* 0x08 */ s32 sectorCount;    /**< Number of sectors to read. */
    /* 0x0C */ u8 pad0C[0x10];
    /* 0x1C */ s32 destBuffer;     /**< Destination buffer address for CdRead. */
    /* 0x20 */ void (*callback)(void); /**< Completion callback (NULL if none). */
} CdReadState; /* 0x24 = 36 bytes */

#endif /* CD_H */
