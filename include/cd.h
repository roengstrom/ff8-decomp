#ifndef CD_H
#define CD_H

#include "common.h"
#include "psxsdk/libcd.h"

/** @brief CD-ROM file descriptor (sector + size pair).
 *
 *  Used by cdRead / func_80038868 to identify data on disc.
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
    /* 0x0A */ s16 discNumber;    /**< Disc number, 1..4 or -1 (write-only; set by setDiscNumber). */
    /* 0x0C */ u8 pad0C[4];
    /* 0x10 */ u8 busyFlag;       /**< CD busy flag (setCdBusyFlag/clearCdBusyFlag). */
    /* 0x11 */ u8 cdState;        /**< CD state byte (getCdState/resetCdDriveMode). */
    /* 0x12 */ s8 discId;         /**< Disc ID (read signed by getDiscId). */
    /* 0x13 */ u8 pad13;
} CdDriveState; /* 0x14 = 20 bytes */

/**
 * @brief CD-ROM read state (D_8008A3D8).
 *
 * Tracks the state machine for CD read operations, including sector
 * parameters, destination buffer, callback pointer, and error counters.
 */
typedef struct {
    /* 0x00 */ u8 flags;             /**< Bit 1: busy flag. Cleared/set by clearCdBusyFlag/setCdBusyFlag. */
    /* 0x01 */ u8 status;          /**< State machine phase (0=idle, 0xB=complete, 0xC=reset). */
    /* 0x02 */ u8 discNum;         /**< Detected disc number (aliases @c D_8008A3DA / @ref getDiscId). */
    /* 0x03 */ u8 pad03;
    /* 0x04 */ CdlLOC params;      /**< Seek location filled by CdIntToPos; cast to
                                    *   @c u8* when passed to CdControl. */
    /* 0x08 */ u32 sectorCount;    /**< Number of sectors to read. */
    /* 0x0C */ u8 pad0C[0x10];
    /* 0x1C */ u8 *readBuffer;     /**< Destination buffer for CdRead. */
    /* 0x20 */ void (*callback)(void); /**< Completion callback (NULL if none). */
} CdReadState; /* 0x24 = 36 bytes */

/** @brief Async CD read state owned by @c cdread.c; polled by @c func_800393C8. */
extern CdReadState D_8008A3D8;

/* Shared CD driver state / buffers (used by both cdrom.c and cdread.c). */
extern CdDriveState D_8008A3C8; /**< CD drive timeout / state machine. */
extern u8  D_8008A3DC[];        /**< CD command parameter buffer (aliases @c D_8008A3D8.params). */
extern s32 D_8008A3B8;          /**< Rolling read-cursor LBA (set by @c func_80038868). */
extern u8  D_800853B8[];        /**< CD read staging buffer. */

extern s32 cdReadAsyncSync(s32 lba, u32 size, u8 *dest, void (*callback)(void));

/** @brief Initiate an asynchronous CD read with a completion callback. */
extern s32 cdRead(s32 lba, u32 size, u8 *dest, void (*callback)(void));

/** @brief Lower-level async CD read used by @ref cdRead. */
extern s32 func_80038868(s32 lba, u32 size, u8 *dest, void (*callback)(void));

/** @brief Poll the async CD read pipeline; returns @c 0 once the read has completed.
 *         Returns the CD state byte zero-extended to @c s32 — callers compare
 *         against @c 0 and @c -1, so the s32 view matches the codegen the
 *         original 1998 source assumed even though the body's stored field
 *         is byte-typed (@c D_8008A3D8.status). */
extern s32 func_800393C8(void);

#endif /* CD_H */
