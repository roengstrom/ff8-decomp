#ifndef MENUSHOP_H
#define MENUSHOP_H

#include "gamestate.h"
#include "menushop2.h"

#define WEAPON_INFO_COUNT 28
#define WEAPON_RECIPE_COUNT 33
#define WEAPON_RECIPE_INGREDIENT_COUNT 4
#define ITEM_PRICE_COUNT 200

/** @brief String codes passed as argument to func_801F6AA4. */
typedef enum {
    STRING_JUNK_SHOP_ABILITY = 48,
    STRING_QUANTITY = 50,
    STRING_MONEY = 51,
    STRING_QUANTITY2 = 55,
    STRING_PRICE = 56,
    STRING_HIT = 58,
    STRING_JUNK_SHOP_REMODEL_WEAPON_TO = 59,
    STRING_JUNK_SHOP_NOT_ENOUGH_ITEMS_COME_BACK_LATER = 61,
    STRING_JUNK_SHOP_NOT_ENOUGH_ITEMS = 62,
    STRING_JUNK_SHOP_NOT_ENOUGH_MONEY = 63,
    STRING_SHOP_WELCOME = 64,
    STRING_SHOP_WHAT_DO_YOU_WANT_TO_SELL = 65,
    STRING_SHOP_WHAT_DO_YOU_WANT_TO_BUY = 66,
    STRING_SHOP_COME_BACK_SOON = 68,
    STRING_JUNK_SHOP_WELCOME_WHO_NEEDS_TO_REMODEL = 69,
    STRING_SHOP_BUY_HOW_MANY = 70,
    STRING_SHOP_SELL_HOW_MANY = 71,
    STRING_SHOP_CANT_CARRY_ANY_MORE = 72,
    STRING_SHOP_NOT_ENOUGH_MONEY = 73,
    STRING_SHOP_WELCOME2 = 74,
    STRING_JUNK_SHOP_REMODEL_TO_WHICH_WEAPON = 75,
    STRING_JUNK_SHOP_YOU_HAVE_IT_ALREADY = 76,
    STRING_JUNK_SHOP_COME_BACK_SOON = 77,
    STRING_JUNK_SHOP_CANT_REMODEL_YOUR_WEAPON = 78,
} StringCode;

/** @brief Colors used to draw text in the menu. */
typedef enum {
    COLOR_GRAY = 1,
    COLOR_RED = 2,
    COLOR_YELLOW = 3,
    COLOR_WHITE = 7,
} Color;

/** @brief Icon types used in the menu. */
typedef enum {
    ICON_NONE = 0,
    ICON_GIL = 11,
    ICON_PRICE = 71,
    ICON_NAME = 73,
    ICON_ITEM = 76,
    ICON_NUM = 77,
    ICON_INFO = 87,
    ICON_ARROW_UP = 109,
    ICON_ARROW_DOWN = 110,
    ICON_STR = 305,
    ICON_HIT = 311,
} IconType;

typedef struct {
    u8 pad0[4];
    u8 characterId; /**< 0x04: character id who uses this weapon. */
    u8 pad5[2];     /* 0x05 */
    u8 hit;         /**< 0x07: weapon hit. */
    u8 pad8[4];     /* 0x08 */
} WeaponInfo; /* 12 bytes */

typedef struct {
    u8 itemId;      /**< 0x00: item id. */
    u8 quantity;    /**< 0x01: item quantity. */
} WeaponRecipeIngredient; /* 2 bytes */

typedef struct {
    u16 nameId;                            /**< 0x00: index of the weapon name. */
    u8 pad3;                               /* 0x02 */
    u8 basePrice;                          /**< 0x03: weapon base price. */
    WeaponRecipeIngredient ingredients[WEAPON_RECIPE_INGREDIENT_COUNT]; /**< 0x04: ingredients required to craft the weapon. */
} WeaponRecipe; /* 12 bytes */

extern WeaponInfo D_8007C3B8[WEAPON_INFO_COUNT]; /**< Weapon attributes. */
extern WeaponRecipe D_801E9BA0[WEAPON_RECIPE_COUNT]; /**< Junk shop weapon recipes (mwepon.bin content). */
extern u8 D_801E9D2C[68]; /**< Weapon names (mwepon.msg content). */
extern u8 D_801EB088[ITEM_PRICE_COUNT]; /**< Item quantities. */
extern u8 D_801EB150[8]; /**< Weapon ID's listed at junk shop for the selected character. */
extern s32 D_801EB2E4; /**< ID of the selected weapon at the junk shop. */
extern s32 D_801EB2E8; /**< ID of the selected character at the junk shop. */

void func_801E5C08(u32);
s32 func_801E5D28(void);
s32 func_801E77EC(s32, s32, s32, s32, s32);
u8* func_801E7CFC(s32);
void func_801E7D30(u8*, u8*);
u32 func_801E7E1C(s32);
s32 func_801E7E68(s32, u32);
s32 func_801E7F4C(s32, u32);
s32 func_801E8058(u32);
void func_801E8134(s32, s32);
void func_801E816C(s32, s32);

#endif /* MENUSHOP_H */
