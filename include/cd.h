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
 * @brief CD drive timeout and state (D_8008A3C8).
 *
 * Tracks the timeout counter for CD operations and drive-level state.
 */
typedef struct {
    /* 0x00 */ u32 timeout;       /**< Timeout counter (resets at 0x708 / 1800). */
    /* 0x04 */ u32 readSectors;   /**< Number of sectors for current read. */
    /* 0x08 */ u16 state;         /**< CD drive state machine phase. */
    /* 0x0A */ u16 discNumber;    /**< Disc number (set by setDiscNumber). */
    /* 0x0C */ u8 pad0C[4];
    /* 0x10 */ u8 busyFlag;       /**< CD busy flag (setCdBusyFlag/clearCdBusyFlag). */
    /* 0x11 */ u8 cdState;        /**< CD state byte (getCdState/resetCdDriveMode). */
    /* 0x12 */ u8 discId;         /**< Disc ID (getDiscId). */
    /* 0x13 */ u8 pad13;
} CdDriveState; /* 0x14 = 20 bytes */

/**
 * @brief CD-ROM read state (D_8008A3D8).
 *
 * Tracks the state machine for CD read operations, including sector
 * parameters, destination buffer, callback pointer, and error counters.
 */
typedef struct {
    /* 0x00 */ u8 pad00;
    /* 0x01 */ u8 status;          /**< State machine phase (0=idle, 0xB=complete, 0xC=reset). */
    /* 0x02 */ u8 pad02[2];
    /* 0x04 */ u8 params[4];       /**< CdControl parameters. */
    /* 0x08 */ u32 sectorCount;    /**< Number of sectors to read. */
    /* 0x0C */ u8 pad0C[0x10];
    /* 0x1C */ u8 *readBuffer;     /**< Destination buffer for CdRead. */
    /* 0x20 */ void (*callback)(void); /**< Completion callback (NULL if none). */
} CdReadState; /* 0x24 = 36 bytes */

#endif /* CD_H */
