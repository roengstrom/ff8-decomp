#ifndef LZS_H
#define LZS_H

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

typedef struct {
    /* 0x00 */ u8 *src;
    /* 0x04 */ u8 pad04[0x24];
    /* 0x28 */ s32 outputSize;
} LzssState; /* 0x2C */

extern LzssState D_80039418;

// Public prototypes

/** @brief memcpy-style copy: copies @p size bytes from @p src to @p dest. */
void FastMemCpy(u8 *dest, u8 *src, u32 size);

/** @brief memset-style clear: zeroes @p size bytes at @p dst. */
void func_800396E0(void *dst, s32 size);

/** @brief LZSS-decompress @p src into @p dest. */
s32 func_80039444(u8 *src, u8 *dest);

s32 func_8003947C(void);

/** @brief Return entry @p idx of @p table: the table's address plus that entry's offset. */
u8 *getOffsetTableEntry(OffsetTable *table, s32 idx);

#endif /* LZS_H */
