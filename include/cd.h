#ifndef CD_H
#define CD_H

#include "common.h"
#include "psxsdk/libcd.h"

/** @brief CD-ROM file descriptor (sector + size pair).
 *
 *  Used by LoadDataFromDisc / LoadCompressedDataFromDisc to identify data on disc.
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

typedef enum {
    CD_READ_IDLE = 0,
    CD_READ_UNK_1 = 1,
    CD_READ_UNK_2 = 2,
    CD_READ_UNK_3 = 3,
    CD_READ_UNK_4 = 4,
    CD_READ_UNK_5 = 5,
    CD_READ_UNK_6 = 6,
    CD_READ_UNK_7 = 7,
    CD_READ_UNK_8 = 8,
    CD_READ_UNK_9 = 9,
    CD_READ_UNK_10 = 10,
    CD_READ_UNK_11 = 11,
    CD_READ_UNK_12 = 12
} CdReadStatus;

/**
 * @brief CD-ROM read state (g_cdReadState).
 *
 * Tracks the state machine for CD read operations, including sector
 * parameters, destination buffer, callback pointer, and error counters.
 */
typedef struct {
    /* 0x00 */ u8 flags;             /**< Bit 1: busy flag. Cleared/set by clearCdBusyFlag/setCdBusyFlag. */
    /* 0x01 */ u8 status;          /**< A @c CdReadStatus. */
    /* 0x02 */ u8 discNum;         /**< Detected disc number (aliases @c D_8008A3DA / @ref getDiscId). */
    /* 0x03 */ u8 pad03;
    /* 0x04 */ CdlLOC params;      /**< Seek location filled by CdIntToPos; cast to
                                    *   @c u8* when passed to CdControl. */
    /* 0x08 */ u32 sectorCount;    /**< Number of sectors to read. */
    /* 0x0C */ u8 pad0C[0x10];
    /* 0x1C */ u8 *readBuffer;     /**< Destination buffer for CdRead. */
    /* 0x20 */ void (*callback)(void); /**< Completion callback (NULL if none). */
} CdReadState; /* 0x24 = 36 bytes */

extern CdReadState g_cdReadState;

/* Shared CD driver state / buffers (used by both cdrom.c and cdread.c). */
extern CdDriveState D_8008A3C8; /**< CD drive timeout / state machine. */
extern u8  D_8008A3DC[];        /**< CD command parameter buffer (aliases @c g_cdReadState.params). */
extern s32 D_8008A3B8;          /**< Rolling read-cursor LBA. */
extern u8  D_800853B8[];        /**< CD read staging buffer. */

extern s32 cdReadAsyncSync(s32 lba, u32 size, void *dest, void (*callback)(void));

/** @brief Initiate an asynchronous CD read with a completion callback. */
extern s32 LoadDataFromDisc(s32 lba, u32 size, void *dest, void (*callback)(void));

/** @brief Read LZS-compressed data from disc and decompress it into dest. */
extern s32 LoadCompressedDataFromDisc(s32 lba, u32 size, void *dest, void (*callback)(void));

/** @brief Run one step of the CD read state machine and return its status. */
extern CdReadStatus UpdateCdRead(void);

#endif /* CD_H */
