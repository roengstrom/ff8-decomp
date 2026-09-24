#ifndef OVERLAY_H
#define OVERLAY_H

#include "common.h"
#include "world/we_object0.h"

/**
 * @brief Overlay command queue entry (g_ovlCmdQueue, 8-slot ring buffer).
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

/**
 * @brief Overlay load result code (set by overlay.c, polled elsewhere).
 *
 * -2 = invalid command, -1 = pending, 0 = success, positive = caller-defined
 * status (e.g. menututo writes 0xD before func_801F010C).
 */
extern volatile s32 D_8008514C;

/** @brief Argument passed to the post-transition overlay entry (set by func_8003646C). */
extern s32 D_80085210;
/** @brief Currently-loaded overlay dependency ID. */
extern u8  D_8008520A;
/** @brief Snapshot of @c g_battleAnims.field703 saved across transition. */
extern u8  D_8008520B;
/** @brief Cleared at the start of a transition (purpose unknown). */
extern u8  D_8008520C;

/* --- overlay.c routines --- */

extern void resetOverlayQueue(void);
extern void loadDefaultOverlay(void);
extern s32  pollCdReadStatus(void);
extern void saveAndClearFramebuffer(s32 a0);
extern void loadOverlayWithTimCallback(s32 a0, s32 a1);

/** @brief Reset card-slot dispatch state for the Triple-Triad mini-game. */
extern void resetCardSlots(s32 mode);

extern s32 func_8003646C(); /* K&R: called with 1 or 2 args */

/* --- Loaded-overlay entry points -----------------------------------------
 * These live inside the swapped overlay at 0x80098xxx (each overlay provides
 * its own implementation at the shared entry address); main invokes them
 * after loading an overlay. */

/** @brief Run the loaded overlay's entry point (init / execute). */
extern void func_80098000(void);

#endif /* OVERLAY_H */
