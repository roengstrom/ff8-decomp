#ifndef NUMSTR_H
#define NUMSTR_H

#include "common.h"

/* Number / message string formatting helpers (numstr.c). */

/** @brief One entry of the 4-byte-stride glyph metrics/UV table at @ref D_8008371C. */
typedef struct {
    u8 width;   /**< Advance width in the low nibble. */
    u8 pad1;
    u16 uv;     /**< Packed texture u/v for the glyph's 12x12 cell. */
} FontGlyph;

/** @brief Glyph metrics/UV table indexed by the ids @ref intToDecString emits
 *  (1-indexed: renderers address it through a `- 1` base). */
extern FontGlyph D_8008371C[];

/* Public prototypes */
extern void intToDecString(u32 value, u8 *buf, s32 digitBase);
extern void intToDecStringShort(u32 value, u8 *buf, s32 digitBase);
extern void replaceLeadingZeros(u8 *buf, s32 count, s32 digitBase, s32 replacement);
extern void lookupHexChar(s32 idx, u8 *dst);
extern void byteToHexString(s32 byte, u8 *buf);
extern void advanceAndDecodeMessage(s32 *stream, s32 arg1);
extern void decodeMessageDirect(s32 *stream, s32 arg1);

u8 *func_8002F548(u8 *src);
void func_8002F294(s32 value, u8 *dst, u8 digits);
void func_8002F2EC(u8 *dst, s32 base, u8 digits, u8 width);

#endif /* NUMSTR_H */
