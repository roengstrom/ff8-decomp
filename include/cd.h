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

#endif /* CD_H */
