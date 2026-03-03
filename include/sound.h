#ifndef SOUND_H
#define SOUND_H

#include "common.h"

/**
 * @brief Sound sequence track (D_80070D60, stride 272 bytes).
 *
 * Each track controls one voice/channel of a sound sequence. The array
 * at D_80070D60 holds all active tracks. This struct is defined for
 * documentation purposes; it is NOT used in source code yet because
 * the complex PsyQ 4.3 scheduling in 34C8.c makes struct access risky
 * for byte-matching.
 *
 * @note Field names are provisional — derived from usage patterns in
 *       34C8.c accessor functions. Many fields are still unknown.
 */
typedef struct {
    u8 field00;         /**< 0x00: Unknown byte. */
    u8 field01;         /**< 0x01: Unknown byte. */
    u8 field02;         /**< 0x02: Unknown byte. */
    u8 field03;         /**< 0x03: Unknown byte. */
    u8 pad04[0x0C];     /**< 0x04..0x0F: Unknown. */
    /* 0x10..0x10F: Large block of track data (256 bytes). */
    u8 trackData[0x100]; /**< 0x10..0x10F: Track control/sequence data. */
} SoundSeqTrack;        /* 0x110 = 272 bytes */

#endif /* SOUND_H */
