#ifndef OVERLAY_H
#define OVERLAY_H

#include "common.h"

/**
 * @brief Overlay command queue entry (D_80085168, 8-slot ring buffer).
 *
 * Each entry describes a pending overlay load/unload command. The queue
 * is indexed by D_80085140 (write) and D_80085144 (read).
 */
typedef struct {
    s16 cmd;        /**< 0x00: Command type (0 = load, etc.). */
    s16 param;      /**< 0x02: Command-specific parameter. */
    s16 ovlId;      /**< 0x04: Overlay identifier. */
    s16 pad06;      /**< 0x06: Padding / unused. */
    s32 loadAddr;   /**< 0x08: Destination load address for the overlay. */
    s32 callback1;  /**< 0x0C: First callback address (or 0). */
    s32 callback2;  /**< 0x10: Second callback address (or 0). */
} OvlCmdEntry;      /* 0x14 = 20 bytes */

#endif /* OVERLAY_H */
