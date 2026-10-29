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

extern u8 D_801FA27C;
extern u8 D_801FA280[];
extern s32 D_801FA3C0;

DR_AREA *func_801EF800(P_TAG *, DR_AREA *, u8 *);  /**< Link a custom-rect DR_AREA prim. */
DR_AREA *func_801EF8D8(P_TAG *, DR_AREA *);   /**< Link a clip-restore DR_AREA prim. */
void *DrawMenuWindow(void *, void *, s32, u32);
void func_801EFF64(void);
void func_801EFFE4(s32);
void *func_801EFBB4(void *, void *, MenuRowCallback);

#endif /* MENUMAIN_H */
