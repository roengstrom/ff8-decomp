/**
 * @file intro_state.c
 * @brief Zero-initialized state globals for the intro overlay.
 *
 * These live in the .data section (not .bss) so that the overlay binary
 * preserves the same byte layout as the original — zeros explicitly
 * stored in ROM, loaded into VRAM along with text and the asset table.
 */
#include "common.h"
#include "intro_state.h"

DispCtx g_introDispCtx     = {0};   /* 0x800991D8 — double-buffer display context. */
s32     g_introOdeLatch    = 0;     /* 0x8009928C — last-seen @c GetODE() < 1 parity. */
s32     g_introRenderMode  = 0;     /* 0x80099290 — sub-renderer dispatch (0..2). */
s32     g_introRenderEnable= 0;     /* 0x80099294 — non-zero runs the per-frame renderer. */
s32     g_introCtrl0Inv    = 0;     /* 0x80099298 — ~previous gamepad-0 sample. */
s32     g_introCtrl1Inv    = 0;     /* 0x8009929C — ~previous gamepad-1 sample. */
s32     g_introCtrl0       = 0;     /* 0x800992A0 — latest gamepad-0 sample. */
s32     g_introCtrl1       = 0;     /* 0x800992A4 — latest gamepad-1 sample. */
s32     g_introCtrl0Edge   = 0;     /* 0x800992A8 — gamepad-0 rising-edge mask (& 0xF0 = ABXY). */
s32     g_introCtrl1Edge   = 0;     /* 0x800992AC — gamepad-1 rising-edge mask. */
s32 g_introVSyncBase = 0;
