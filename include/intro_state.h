#ifndef INTRO_STATE_H
#define INTRO_STATE_H

#include "common.h"
#include "psxsdk/libgpu.h"

/**
 * @brief Display-init double-buffer context (180 bytes at 0x800991D8).
 *
 * Per-buffer display state for the intro overlay's double-buffered
 * rendering. The PSX GPU draws to one buffer while the other is
 * displayed; @c currBuf flips each frame.
 */
typedef struct {
    u32      ot[2];        /**< 0x00: double-buffered single-entry OT. */
    DRAWENV  draw;         /**< 0x08: 92-byte drawing environment. */
    DISPENV  disp;         /**< 0x64: 20-byte display environment. */
    TILE     tiles[2];     /**< 0x78: per-buffer fade overlay rectangles. */
    DR_TPAGE drTPages[2];  /**< 0x98: per-buffer SetDrawTPage commands. */
    s32      currBuf;      /**< 0xA8: active buffer index (0 or 1). */
    s32      unkAC;
    u8       unkB0;        /**< 0xB0: flag byte. */
    u8       unkB1;
    u8       pad[2];
} DispCtx;

extern DispCtx g_introDispCtx;       /**< 0x800991D8 — double-buffer display context. */
extern s32     g_introOdeLatch;      /**< 0x8009928C — last-seen @c GetODE() < 1 parity. */
extern s32     g_introRenderMode;    /**< 0x80099290 — sub-renderer dispatch (0..2). */
extern s32     g_introRenderEnable;  /**< 0x80099294 — non-zero runs the per-frame renderer. */
extern s32     g_introCtrl0Inv;      /**< 0x80099298 — ~previous gamepad-0 sample. */
extern s32     g_introCtrl1Inv;      /**< 0x8009929C — ~previous gamepad-1 sample. */
extern s32     g_introCtrl0;         /**< 0x800992A0 — latest gamepad-0 sample. */
extern s32     g_introCtrl1;         /**< 0x800992A4 — latest gamepad-1 sample. */
extern s32     g_introCtrl0Edge;     /**< 0x800992A8 — gamepad-0 rising-edge mask (& 0xF0 = ABXY). */
extern s32     g_introCtrl1Edge;     /**< 0x800992AC — gamepad-1 rising-edge mask. */
extern s32     g_introVSyncBase;     /**< 0x800992B0 — VSync(-1) baseline for story-page hold timing. */

#endif /* INTRO_STATE_H */
