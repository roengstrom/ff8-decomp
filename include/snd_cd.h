#ifndef SND_CD_H
#define SND_CD_H

#include "common.h"

/**
 * @brief A table of u32 offsets, each measured from the table's own start.
 *
 * The first entry's data follows the table directly, so the first offset is
 * also the table's size in bytes.
 */
typedef struct {
    u32 offsets[1]; /* variable length */
} OffsetTable;

// Public prototypes

/** @brief memset-style clear: zeroes @p size bytes at @p dst. */
extern void func_800396E0(void *dst, s32 size);

/** @brief LZSS-decompress @p src into @p dest. */
s32 func_80039444(u8 *src, u8 *dest);

/** @brief Return entry @p idx of @p table: the table's address plus that entry's offset. */
extern u8 *getOffsetTableEntry(OffsetTable *table, s32 idx);

#endif /* SND_CD_H */
