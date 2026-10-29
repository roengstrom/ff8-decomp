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

extern u8 D_801FA27C;
extern u8 D_801FA280[];
extern s32 D_801FA3C0;

void func_801EFF64(void);

#endif /* MENUMAIN_H */
