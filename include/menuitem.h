/**
 * @file menuitem.h
 * @brief Symbols and types owned by the menuitem overlay.
 *
 * The overlay is split into two translation units, menuitem.c and menuitem2.c
 * (the file boundary is at 0x801E9F94), so these declarations are shared.
 */
#ifndef MENUITEM_H
#define MENUITEM_H

#include "common.h"
#include "gamestate.h"
#include "menumain.h"

/** @brief State of the item menu (partial). */
typedef struct {
    /* 0x00 */ u8 unk0[0x20];
    /* 0x20 */ ItemSlot *items;   /**< Item inventory shown in the list. */
    /* 0x24 */ s32 unk24;
    /* 0x28 */ u8 *itemDesc;      /**< Description of the selected item, NULL for an empty slot. */
    /* 0x2C */ u8 unk2C[0x54 - 0x2C];
    /* 0x54 */ s16 cursor;        /**< Selected slot in the item list. */
    /* 0x56 */ s16 unk56;
    /* 0x58 */ s16 unk58;         /**< Second slot index into the item list. */
    /* 0x5A */ u8 unk5A[0x65 - 0x5A];
    /* 0x65 */ u8 itemId;         /**< Item id of the selected slot. */
} ItemMenu;

extern s32 D_801ECC10;
extern s32 D_801ECE20;
extern s32 D_801ECE24;
extern s32 D_801ECE28;
extern s32 D_801ECE2C;
extern s32 D_801ECE30;
extern s32 D_801ECE34;
extern s32 D_801ECE38;
extern s32 D_801ECEDC;
extern s32 D_801ECEE0;
extern s32 D_801ECEE4;
extern s32 D_801ECEE8;
extern u8 D_801EB17C[];
extern u8 D_801EB188[];
extern u8 D_801EB194[];
extern u8 D_801EB1D8[];
extern u8 D_801EB330[];
extern u8 D_801EB4BC[];
extern u8 D_801EC710[];
extern u8 D_801ECB20[];
extern u8 D_801ECB60[];
extern s32 func_801E2EA8(s32);
extern s32 func_801EFFD4(void);

void func_801E4EA4(s32);
s32 func_801E9C90(u8 *str, s32 max);
s32 func_801E9E10(u8 *str);

#endif /* MENUITEM_H */
