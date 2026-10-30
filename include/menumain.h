/**
 * @file menumain.h
 * @brief Symbols and types owned by the menumain overlay unit.
 *
 * The menumain overlay (loaded at @c 0x801EF800) hosts the main in-game
 * menu: panel/window rendering, the shared text lookup/render helpers used
 * by the menu sub-overlays, party management and the menu state machine.
 *
 * Cross-overlay shared types (e.g. @c MenuDisplayConfig) live in
 * @c include/menu.h. Menu sub-overlays that call into menumain at fixed
 * addresses keep their own file-local prototypes (overlay-conflict rule).
 */
#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "common.h"
#include "menu.h"
#include "psxsdk/libgpu.h"

/**
 * @brief Stores the display, drawing, and ordering table data for a menu render buffer.
 *
 * Each instance holds the configuration and GPU ordering table for one
 * side of the double-buffered menu rendering system.
 */
typedef struct {
    DISPENV dispEnv; /**< 0x00 */
    DRAWENV drawEnv; /**< 0x14 */
    u32 *ot; /**< 0x70: Pointer to otArray. */
    u32 otArray[9]; /**< 0x74: Reverse-linked GPU ordering table. */
    u32 unk98; /**< 0x98 */
    u32 unk9C; /**< 0x9C */
} MenuRenderBuffer; /* 0xA0 bytes */

extern u8 D_801FA27C;
extern MenuRenderBuffer D_801FA280[2]; /**< Double-buffered menu render environments. */
extern MenuRenderBuffer *D_801FA3C0; /**< Pointer to the currently active menu render buffer. */

DR_AREA *func_801EF800(P_TAG *, DR_AREA *, RECT *);
DR_AREA *func_801EF8D8(P_TAG *, DR_AREA *);
void *DrawMenuWindow(void *, void *, s32, u32);
void func_801EFF64(void);
void func_801EFFE4(s32);
void *func_801EFBB4(void *, void *, MenuRowCallback);
void func_801F0038(void);
void func_801F00A0(void);
void func_801F0224(void);

#endif /* MENUMAIN_H */
