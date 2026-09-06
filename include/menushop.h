#ifndef MENUSHOP_H
#define MENUSHOP_H

#include "gamestate.h"

#define SHOP_ITEM_COUNT 16
#define ITEM_PRICE_COUNT 200
#define ITEM_SLOT_COUNT 198 

typedef struct {
    u8 itemId;      /**< 0x0: item ID. */
    u8 rarity;      /**< 0x1: item rarity (0x00 = Rare, 0xFF = Common). */
} ShopItemRarity; /* 0x2 = 2 bytes */

typedef struct {
    u8 itemId;      /**< 0x0: item ID. */
    u8 visible;     /**< 0x1: item visibility flag. */
} ShopItemVisibility; /* 0x2 = 2 bytes */

typedef struct {
    u16 basePrice;  /**< 0x0: base price. */
    u8 sellRate;    /**< 0x2: sell rate. */
} ShopItemPrice; /* 0x3 = 3 bytes */

typedef struct {
    u8 pad000[0x20];
    s32 *unk20; /**< 0x20: pointer to data. */
    u8 pad024[0x12];
    u16 unk36; /**< 0x36: scroll offset. */
} Struct_801E8B60;

typedef struct {
    u8 pad000[0x3A];
    s16 unk3A; /**< 0x3A: scroll offset. */
    u8 pad03D[0x4];
    u8 unk40; /**< 0x40: page start. */
    u8 unk41; /**< 0x41: page end. */
    u8 pad041[0x4];
    u8 unk46; /**< 0x46: orientation (left if equals 0, right otherwise). */
} Struct_801E7374;

typedef struct {
    u8 pad000[0x28];
    s32 unk28; /**< 0x28: passed to drawColorByMenuPalette as 4th argument. */
} Struct_801E7508;

extern ShopData D_80077CC8[SHOP_COUNT]; /**< Shop data table. */
extern u8 D_80077EBC[ITEM_SLOT_COUNT]; /**< Item slot inventory. */
extern ShopItemPrice D_801EA3F0[ITEM_PRICE_COUNT]; /**< Item price table. */
extern u8 D_801EA70C[];
extern u8 D_801F7F98[];
extern u8 D_801E9B64[];
extern u8 D_801E9B6C[];
extern ShopItemRarity D_801EA170[SHOP_COUNT][SHOP_ITEM_COUNT]; /**< Shop item rarity tables. */
extern ShopItemVisibility D_801EAA28[SHOP_ITEM_COUNT]; /**< Shop item visibility table. */
extern s32 D_801EAA48[ITEM_PRICE_COUNT]; /**< Item sell prices. */
extern s32 D_801EAD68[ITEM_PRICE_COUNT]; /**< Item buy prices. */
extern u8 D_801EB088[ITEM_PRICE_COUNT]; /**< Item quantities. */
extern s32 g_menuColor;
extern s32 D_80077E70;
extern s32 func_801E8AB0;
extern void func_801E5E90(void*);
extern s32 func_801E79D4(void*, s32, s32);
extern s32 func_801EFBB4(s32, s32, s32);
extern void* func_801E6FD8(void);

#endif /* MENUSHOP_H */
