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
    u8 type;    /**< 0x0: item type. */
    u8 flags;   /**< 0x1: targeting and usability bitfield. */
    u8 param1;  /**< 0x2: type-dependent (heal amount, GF id, ability id, magazine start page). */
    u8 param2;  /**< 0x3: type-dependent (status mask, stat mask, compatibility amount, magazine end page). */
} MItemEntry; /* 0x4 = 4 bytes */

typedef struct {
    u8 pad000[0x10];
    u16 state;          /**< 0x10: state machine current state (0-17) */
    u8 pad12[0x8];
    s16 unk1C;
    u8 pad1D[0x3];      /**< 0x12 */
    void *field_20;     /**< 0x20 */
    s32 field_24;       /**< 0x24 */
    u32 gil;            /**< 0x28: gil */
    u8 *unk2C;          /**< 0x2C: pointer to D_80077EBC */
    union {
        s32 unk30_s32;
        s16 unk30_s16[2];
    } union30;          /* 0x30 */
    s16 unk34;          /* 0x34 */
    u16 unk36;          /* 0x36 */
    u16 unk38;          /* 0x38 */
    s16 unk3A;          /**< 0x3A: scroll offset. */
    union {
        s16 unk3C_s16[2];
        s8 unk3C_s8[4];
    } union3C;          /* 0x3C */
    u8 unk40;           /**< 0x40: page start. */
    u8 unk41;           /**< 0x41: page end. */
    s8 unk42;           /* 0x42 */
    u8 unk43;           /* 0x43 */
    u8 unk44;           /* 0x44 */
    u8 unk45;           /* 0x45 */
    u8 unk46;           /**< 0x46: orientation (left if equals 0, right otherwise). */
    s8 unk47;           /* 0x47 */
    u8 unk48;           /* 0x48 */
    u8 unk49;           /**< 0x49: item count */
    u8 unk4A;           /* 0x4A */
    u8 unk4B;           /* 0x4B */
    u16 unk4C;          /* 0x4C */
    u16 unk4E;          /* 0x4E */
} ShopMenuState;

typedef struct {
    u16 unk0;
    u8 unk2;
    u8 unk3;
    u8 unk4[8];
} Struct_func_801E7CFC; /* 12 bytes */

typedef struct {
    u8 pad0[4];
    u8 unk4;
    u8 pad[7];
} Struct_func_801E7F4C; /* 12 bytes */

extern ShopData D_80077CC8[SHOP_COUNT]; /**< Shop data table. */
extern s32 D_80077E70;
extern u8 D_80077EBC[ITEM_SLOT_COUNT]; /**< Item slot inventory. */
extern Struct_func_801E7F4C D_8007C3B8[28];
extern u16 D_801E9B64[4];
extern u8 D_801E9B6C[52];
extern Struct_func_801E7CFC D_801E9BA0[33];
extern u8 D_801E9D2C[68];
extern ShopItemRarity D_801EA170[SHOP_COUNT][SHOP_ITEM_COUNT]; /**< Shop item rarity tables (shop.bin content). */
extern ShopItemPrice D_801EA3F0[ITEM_PRICE_COUNT]; /**< Item price table (price.bin content). */
extern MItemEntry D_801EA70C[199]; /**< Field-menu item table (mitem.bin content). */
extern ShopItemVisibility D_801EAA28[SHOP_ITEM_COUNT]; /**< Shop item visibility table. */
extern s32 D_801EAA48[ITEM_PRICE_COUNT]; /**< Item sell prices. */
extern s32 D_801EAD68[ITEM_PRICE_COUNT]; /**< Item buy prices. */
extern u8 D_801EB088[ITEM_PRICE_COUNT]; /**< Item quantities. */
extern u8 D_801EB150[8];
extern s32 D_801EB160;
extern s32 D_801EB260[];
extern s32 D_801EB2E4;
extern s32 D_801EB2E8;
extern u8 D_801F7F98[];
extern s32 g_menuColor;
extern s32 func_801EFBB4(s32, s32, s32);

/* ======================================================================== */
/* Private prototypes                                                       */
/* ======================================================================== */

s32 func_801E5800(ShopMenuState*, s32, s32);
s32 func_801E583C(ShopMenuState*, s32, s32);
s32 func_801E58A0(ShopMenuState*, s32, s32);
s32 func_801E5904(s32);
void func_801E5930(s32, s32, ShopMenuState*);
s32 func_801E59D8(s16*, s16*, s32);
s32 func_801E5A8C(s32, s32, s32, s32, s32, s16*, s32);
void func_801E5BA4(s32, s32);
void func_801E5C08(s32);
s32 func_801E5D28(void);
void func_801E5DBC(void);
void func_801E5E88(u8);
void func_801E5E90(ShopMenuState*);
void func_801E6A68(s32);
void func_801E6ACC(void);
void func_801E6C3C(s32);
void func_801E6D54(s32);
s32 func_801E6E0C(s32, s32, s32, s32);
s32 func_801E6EB0(s32, s32, s32, s32, s32);
s32 func_801E6F60(ShopMenuState*, s32, s32, s32, s32);
s32 func_801E6FD8(s32, s32, s32, s32, s32);
s32 func_801E722C(ShopMenuState*, s32, s32, s32, s32);
s32 func_801E7374(ShopMenuState*, s32, s32, s32, s32);
s32 func_801E7508(ShopMenuState*, s32, s32, s32, s32);
s32 func_801E7628(ShopMenuState*, s32, s32, s32, s32);
s32 func_801E77EC(s32, s32, s32, s32, s32);
void func_801E791C(s32, s32, s32, s32, s32);
s32 func_801E79D4(ShopMenuState*, s32, s32);
void func_801E7B9C(s32);
void func_801E7C8C(s32);
u8* func_801E7CFC(s32);
void func_801E7D30(u8*, u8*);
s32 func_801E7E1C(s32);
s32 func_801E7E4C(s32);
s32 func_801E7E68(s32, u32);
s32 func_801E7E98(s32, s32);
s32 func_801E7F4C(s32, s32);
s32 func_801E8058(s32);
void func_801E8134(s32, s32);
void func_801E816C(s32, s32);

#endif /* MENUSHOP_H */
