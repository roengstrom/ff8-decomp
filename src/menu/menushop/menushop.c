#include "common.h"
#include "gamestate.h"
#include "menu.h"
#include "menushop.h"
#include "menumain.h"

/**
 * @brief Look up a shop item byte from a table or item data.
 *
 * If a2 >= 0xC6, returns 0. If a1 is non-zero, reads a base pointer
 * from a0+0x2C and returns the byte at offset a2*2. Otherwise reads
 * from D_801EAA28 at offset a2*2.
 *
 * @param a0 Shop context pointer.
 * @param a1 Source selector (0 = static table, non-zero = dynamic).
 * @param a2 Item index.
 * @return Item byte value, or 0 if out of range.
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5800);

/**
 * @brief Look up a shop item's category byte.
 *
 * If a1 is zero, returns the byte at D_801EAA28[a2*2 + 1] directly.
 * Otherwise, calls func_801E5800 to get an item data pointer, then
 * returns the byte at that offset in D_801EB088, or 0 if the pointer
 * is null.
 *
 * @param a0 Shop context parameter.
 * @param a1 Item category selector (0 = direct lookup).
 * @param a2 Item index.
 * @return Category byte value, or 0 if not found.
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E583C);

/**
 * @brief Look up shop item and get its description string.
 *
 * Calls func_801E583C to validate the item, then if valid, calls
 * func_801E5800 to get item data and getStatDesc to get its
 * description string.
 *
 * @param a0 Shop context parameter
 * @param a1 Item category/type
 * @param a2 Item index
 * @return Description string pointer, or 0 if invalid
 */
s32 func_801E58A0(s32 a0, s32 a1, s32 a2) {
    s32 result = func_801E583C(a0, a1, a2);
    if (result != 0) {
        return getStatDesc(func_801E5800(a0, a1, a2));
    }
    return 0;
}

/**
 * @brief Look up a shop item property via double indirection.
 *
 * Loads a byte from D_801EA70C[a0*4], uses it to index into D_801F7F98,
 * and returns that byte.
 *
 * @param a0 Shop item index.
 * @return Property byte value.
 */
s32 func_801E5904(s32 a0) {
    u8 idx = *(u8 *)(D_801EA70C + a0 * 4);
    return D_801F7F98[idx];
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5930);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E59D8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5A8C);

/**
 * @brief Render a shop item at a position from a decoded table.
 *
 * Decodes a position table from D_801E9B64 into a local buffer via
 * func_801E59D8, then uses the halfword at index a1 (plus 0x24) as
 * the Y position for func_801F0A34.
 *
 * @param a0 Render context / X position parameter.
 * @param a1 Index into the decoded position table.
 */
void func_801E5BA4(s32 a0, s32 a1) {
    s16 buf[36];
    func_801E59D8(D_801E9B64, buf, 3);
    func_801F0A34(a0, 0, buf[a1] + 0x24, 0x22);
}

/**
 * @brief Updates the player's gil and synchronizes the shop inventory.
 *
 * Rebuilds item counts from the temporary purchase table, clears depleted
 * slots, and adds any remaining items to the inventory.
 *
 * @param gil The updated gil amount.
 */
void func_801E5C08(s32 gil) {
    ItemSlot *constPtr;
    ItemSlot *p;
    s32 i;

    if (g_gameState.mainData.partyLockFlag & 1) {
        g_gameState.mainData.party.dreamGil = gil;
    } else {
        g_gameState.mainData.party.gil = gil;
    }

    constPtr = D_80077EBC;
    p = D_80077EBC;
    
    for (i = 0; i < ITEM_SLOT_COUNT; i++, p++) {
        u8 itemId = p->id;
        u8 itemCount = D_801EB088[itemId];

        if (itemCount == 0) {
            p->id = 0;
        }

        p->count = itemCount;
        D_801EB088[itemId] = 0;
    }

    p = constPtr;

    for (i = 0; i < ITEM_PRICE_COUNT; i++) {
        u8 itemCount = D_801EB088[i];

        if (itemCount == 0) {
            continue;
        }

        while (1) {
            if (p->id == 0 || p->count == 0) {
                p->id = i;
                p->count = itemCount;
                func_800370AC(i);
                break;
            }
            p++;
        }
    }
}

/**
 * @brief Loads the current gil amount and rebuilds the available item counts.
 *
 * Item counts are read from the shop inventory table and stored by item ID.
 * 
 * @return The current gil amount (dream gil if party is locked, otherwise normal gil).
 */
s32 func_801E5D28(void) {
    s32 result;
    u8* ptr1;
    u8* ptr2;
    s32 i;
    s32 val1;
    s32 val2;

    if (g_gameState.mainData.partyLockFlag & 1) {
        result = g_gameState.mainData.party.dreamGil;
    } else {
        result = g_gameState.mainData.party.gil;
    }
    
    ptr1 = D_80077EBC;
    
    for (i = 0; i < ITEM_PRICE_COUNT; i++) {
        D_801EB088[i] = 0;
    }

    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        val1 = *ptr1;
        ptr1++;
        
        val2 = *ptr1;
        ptr1++;
        
        if (val1 != 0) {
            D_801EB088[val1] = val2;
        }
    }

    return result;
}

/**
 * @brief Compacts the shop item list in place.
 *
 * Scans the 16 entries in D_801EAA28 and keeps only entries whose
 * itemId and visible fields are both non-zero. Valid entries are moved to
 * the beginning of the table in their original order, while all
 * remaining entries are cleared.
 */
void func_801E5DBC(void) {
    u8 sp[SHOP_ITEM_COUNT];
    s32 counter;
    s32 i;
    ShopItemVisibility *ptr;

    ptr = D_801EAA28;
    counter = 0;

    for (i = 0 ; i < SHOP_ITEM_COUNT; i++, ptr++) {
        if (ptr->itemId != 0 && ptr->visible != 0) {
            sp[counter] = i;
            counter++;
        }
    }

    ptr = D_801EAA28;

    for (i = 0; i < counter; i++) {
        if (i != sp[i]) {
            ptr[i] = ptr[sp[i]];
        }
    }

    for (; i < SHOP_ITEM_COUNT; i++) {
        ptr[i].itemId = 0;
        ptr[i].visible = 0;
    }
}

void func_801E5E88(void) {
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E5E90);

/**
 * @brief Populate shop item visibility data for a shop.
 *
 * Copies item IDs from the shop's rarity table and visibility flags from
 * the shop inventory into the shared visibility table.
 */
void func_801E6A68(s32 shopId) {
    ShopItemVisibility *visibility;
    ShopItemRarity *rarity;
    ShopData *shop;
    s32 i;

    visibility = D_801EAA28;

    rarity = D_801EA170[0];
    rarity += shopId * 16;

    shop = &D_80077CC8[shopId];

    for (i = 0; i < SHOP_ITEM_COUNT; i++) {
        visibility->itemId = rarity[i].itemId;
        visibility->visible = shop->items[i];
        visibility++;
    }
}

/**
 * @brief Calculates and stores shop buy and sell prices.
 *
 * The flags returned by func_801F72B4 determine the price modifiers:
 * - bit 0 reduces the buy price by 25%.
 * - bit 1 increases the sell price by 50%.
 * Buy prices are stored in D_801EAD68, and sell prices in D_801EAA48.
 * Prices are calculated using integer arithmetic and are clamped to a
 * minimum value of 1.
 */
void func_801E6ACC(void) {
    ShopItemPrice *itemPrice;
    s32 result;
    s32 i;
    s32 x;
    s32 y;

    result = func_801F72B4();
    itemPrice = D_801EA3F0;
    
    for (i = 0; i < ITEM_PRICE_COUNT; i++) {
        if (result & 1) {
            D_801EAD68[i] = (itemPrice[i].basePrice * 15) / 2;
        } else {
            D_801EAD68[i] = itemPrice[i].basePrice * 10;
        }

        if (result & 2) {
            x = itemPrice[i].sellRate * 10;
            y = itemPrice[i].basePrice * x;
            D_801EAA48[i] = y * 3 / 40;
        } else {
            x = itemPrice[i].sellRate * 10;
            y = itemPrice[i].basePrice * x;
            D_801EAA48[i] = y / 20;
        }
        
        if (D_801EAA48[i] == 0) {
            D_801EAA48[i] = 1;
        }
        if (D_801EAD68[i] == 0) {
            D_801EAD68[i] = 1;
        }
    }
}

/**
 * @brief Update shop item visibility based on rarity and flags.
 *
 * For each item in the shop, computes a value based on the item's rarity.
 * If the computed value exceeds 0x80, the item is marked as visible;
 * otherwise, it is hidden. The visibility flags are stored in both the shared
 * D_801EAA28 table and the shop's own inventory.
 * 
 * @param shopId Index of the shop to update.
 */
void func_801E6C3C(s32 shopId) {
    ShopItemRarity* rarity;
    ShopItemVisibility* p;
    ShopData *shop;
    s32 i;
    s32 bitMask;

    p = D_801EAA28;
    rarity = D_801EA170[0];
    rarity += shopId * 16;
    shop = &D_80077CC8[shopId];
    bitMask = (func_801F72B4() << 6) & 0x100;
    
    for (i = 0; i< 16; i++, p++) {
        if (p->itemId != 0) {
            s32 aux = rarity[i].rarity;
            s32 val = bitMask + aux;
            
            aux = 0xFF;
            if (val == aux) {
                val = 0x100;
            }
            
            func_801F6A5C();
            
            aux = 0x80;
            if (val > aux) {
                aux = 1;
                p->visible = 1;
            } else {
                p->visible = 0;
            }
            
            if (val == 0) {
                p->visible = 0;
            }
            
            shop->items[i] = p->visible;
        }
    }
}


INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6D54);

void func_801E6E0C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 result;

    result = func_801E5A8C(arg0, arg1, arg2 + 0xC, arg3 + 5, 3, D_801E9B64, 7);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg2;
    g_menuDisplayCfg.y = arg3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.h = 0x15;
    func_801EF9AC(arg0, result, 0x1000, g_menuColor);
}

s32 func_801E6EB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    u8 buffer[0x80];
    s32 msg;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_v0;
    s32 result;

    msg = ((s32 *)(g_menuDisplayCfg.dataPtr))[arg2];
    result = arg1;
    if (msg != 0) {
        temp_v0 = (arg4 + 0xA);
        temp_s0 = g_menuDisplayCfg.x + temp_v0;
        temp_s1 = g_menuDisplayCfg.y + 5;
        decodeMessage(msg, buffer, -1);
        result = func_801F0FEC(arg0, arg1, temp_s0, temp_s1, (s32)buffer, 7);
    }
    return result;
}

/**
 * @brief Configure display parameters and invoke callback for shop list rendering.
 *
 * Sets up the g_menuDisplayCfg display configuration structure with the given
 * position and size values, stores the pointer at a0+0x20 as the data source,
 * reads a halfword at a0+0x3A as the display ID, then calls func_801EFBB4
 * with func_801E6EB0 as the render callback.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter (passed as a0 to func_801EFBB4).
 * @param a2 Second callback parameter (passed as a1 to func_801EFBB4).
 * @param a3 Y position for the display configuration.
 * @param arg5 X position for the display configuration.
 */
void func_801E6F60(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x14;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = arg5;
    g_menuDisplayCfg.scrollOffset = *(u16 *)(a0 + 0x3A);
    g_menuDisplayCfg.dataPtr = (s32)(a0 + 0x20);
    {
        func_801EFBB4(a1, a2, (s32)&func_801E6EB0);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E6FD8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E722C);

s32 func_801E7374(Struct_801E7374* arg0, void *arg1, void* arg2, s32 arg3, s32 arg4) {
    g_menuDisplayCfg.iconType = 0x4C;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 0x100;
    g_menuDisplayCfg.h = 0x77;
    g_menuDisplayCfg.columnCount = 8;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.pageStart = arg0->unk40;
    g_menuDisplayCfg.pageEnd = arg0->unk41;
    g_menuDisplayCfg.scrollOffset = arg0->unk3A;
    g_menuDisplayCfg.dataPtr = (s32)arg0;

    if (arg0->unk46 == 0) {
        arg2 = (void *)func_8002FF34(arg1, arg2, 0x47, arg3 + 0xA8, arg4, g_menuColor);
        arg2 = (void *)func_801F5F30((s32)arg1, (s32)arg2, arg3 + 0x1C, arg4, g_menuColor, (s32) (s8) arg0->unk40);
    }
    else {
        arg2 = (void *)func_8002FF34(arg1, arg2, 0x47, arg3 + 0x80, arg4, g_menuColor);
        arg2 = (void *)func_8002FF34(arg1, arg2, 0x4D, arg3 + 0xD6, arg4, g_menuColor);
        arg2 = (void *)func_801F5EFC((s32)arg1, (s32)arg2, arg3 + 0x1C, arg4, g_menuColor, (s32) (s8) arg0->unk40);
    }
    
    arg2 = (void *)func_801F5F60((s32)arg1, (s32)arg2, g_menuColor, 3);
    return func_801EFBB4((s32)arg1, (s32)arg2, (s32)func_801E6FD8);
}

void func_801E7508(Struct_801E7508 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 s6;
    s32 s5;
    s32 v0;

    s6 = arg4 + 6;
    s5 = arg3 + 0xD0;
    arg2 = func_801F0FEC(arg1, arg2, s5, s6, func_801F6AA4(0x33), 7);

    v0 = s6 << 0x10;
    s5 = arg3 + 0x142;
    arg2 = drawColorByMenuPalette(arg1, arg2, v0 | (s5 & 0xFFFF), arg0->unk28, 7);

    s6 = arg4 + 8;
    arg2 = func_8002FF34(arg1, arg2, 0xB, arg3 + 0x143, s6, g_menuColor);

    g_menuDisplayCfg.iconType = 0x57;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.h = 0x17;

    func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7628);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E77EC);

void func_801E791C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 var_v0;
    MenuDisplayConfig *cfg;

    cfg = &g_menuDisplayCfg;

    var_v0 = arg2;
    if (arg0 != 0) {
        var_v0 = func_801F0FEC(arg1, var_v0, arg3 + 0xC, arg4 + 5, arg0, 7);
    }
    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->w = 0xDA;
    cfg->y = (s16) arg4;
    cfg->h = 0x15;
    func_801EF9AC(arg1, var_v0, 0x1000, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E79D4);

void func_801E7B9C(s32 a0) {
    MenuTask *temp_s0;

    temp_s0 = (MenuTask *)func_801F179C((s32)func_801E5E90, (s32)func_801E79D4);
    func_801F1D2C(0, (s32)"shop.bin", (s32)D_801EA170);
    func_801F1D2C(0, (s32)"price.bin", (s32)D_801EA3F0);
    func_801F1D2C(0, (s32)"mitem.bin", (s32)D_801EA70C);
    if (temp_s0 != NULL) {
        temp_s0->unk2C = D_80077EBC;
        temp_s0->unk36 = 0x1000;
        temp_s0->unk30 = 0;
        temp_s0->unk28 = func_801E5D28();
        temp_s0->unk45 = D_801E9B6C[func_801EFFF0()];
        func_801E6D54(temp_s0->unk45);
        func_801E5E90(temp_s0);
        if (func_801EFFB8() == 0x17) {
            func_801F1D84();
        }
    }
}

/**
 * @brief Process shop state and dispatch to appropriate handler.
 *
 * Calls initialization functions, then reads a state byte from
 * D_801E9B6C at the offset returned by func_801EFFF0. If the byte
 * equals 0x15, dispatches to func_801E9900; otherwise dispatches
 * to func_801E7B9C.
 *
 * @param a0 Shop context parameter passed to the handler.
 */
void func_801E7C8C(s32 a0) {
    s32 off;
    func_801F0948(0);
    func_801F7B60();
    off = func_801EFFF0();
    if (*(u8 *)(off + (s32)D_801E9B6C) == 0x15) {
        func_801E9900(a0);
    } else {
        func_801E7B9C(a0);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7CFC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7D30);

/**
 * @brief Compute shop item price from table.
 *
 * Looks up byte at D_801E9BA0[a0*12 + 3], then returns (byte * 5) * 2.
 *
 * @param a0 Shop item index.
 * @return Computed price value.
 */
INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7E1C);

/**
 * @brief Test if bit a0 is set in D_80077E70.
 *
 * @param a0 Bit index.
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_801E7E4C(s32 a0) {
    s32 mask = 1 << a0;
    s32 val = D_80077E70 & mask;
    return val != 0;
}

/** @brief Return whether a1 >= func_801E7E1C(a0) (unsigned). */
s32 func_801E7E68(s32 a0, u32 a1) {
    return a1 >= (u32)func_801E7E1C(a0);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7E98);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8058);

/**
 * @brief Render shop item entry at computed Y position with width 0x24.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x50 for Y position)
 */
void func_801E8134(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x24, a1 * 13 + 0x50);
}

/**
 * @brief Render shop item quantity at computed Y position with width 0xA9.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x4F for Y position)
 */
void func_801E816C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xA9, a1 * 13 + 0x4F);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E81A4);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8978);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8AB0);

/**
 * @brief Configure display parameters and invoke callback for shop sell rendering.
 *
 * Sets up the g_menuDisplayCfg display configuration structure with the given
 * position and size values, stores the pointer at a0+0x20 as the data source,
 * reads a halfword at a0+0x36 as the display ID, then calls func_801EFBB4
 * with func_801E8AB0 as the render callback.
 *
 * @param a0 Pointer to source data structure.
 * @param a1 First callback parameter (passed as a0 to func_801EFBB4).
 * @param a2 Second callback parameter (passed as a1 to func_801EFBB4).
 * @param a3 Y position for the display configuration.
 * @param arg5 X position for the display configuration.
 */
void func_801E8B60(Struct_801E8B60 *a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x14;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = arg5;
    g_menuDisplayCfg.scrollOffset = a0->unk36;
    g_menuDisplayCfg.dataPtr = (s32)&a0->unk20;
    {
        func_801EFBB4(a1, a2, (s32)&func_801E8AB0);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8BD8);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E8D84);

/** @brief Return color code: 7 (equal), 3 (a0 > a1), 2 (a0 < a1). */
s32 func_801E8FF8(s32 a0, s32 a1) {
    s32 color = 7;
    if (a0 > a1) color = 3;
    if (a0 < a1) color = 2;
    return color;
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9020);

/**
 * @brief Initialize all 30 shop item entries.
 *
 * Calls func_801E9020 for indices 0 through 29.
 */
void func_801E90BC(void) {
    s32 i;
    for (i = 0; i < 30; i++) {
        func_801E9020(i);
    }
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E90F8);

/**
 * @brief Configure shop display and render with g_menuDisplayCfg settings.
 *
 * Calls func_801E90F8 with all parameters, then sets up g_menuDisplayCfg
 * display config (icon 0x57, 0x150 x 0x26, x=a3, y=arg5) and calls
 * func_801EF9AC to render.
 *
 * @param a0 Context pointer for func_801E90F8.
 * @param a1 Render context passed to func_801EF9AC.
 * @param a2 Parameter for func_801E90F8.
 * @param a3 X position for display config.
 * @param arg4 Y position for display config.
 */
void func_801E9554(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    s32 result;

    result = func_801E90F8(a0, a1, a2, a3, arg4);
    g_menuDisplayCfg.iconType = 0x57;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.h = 0x26;
    func_801EF9AC(a1, result, 0x1000, g_menuColor);
}

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E95DC);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9684);

INCLUDE_ASM("asm/ovl/menushop/nonmatchings/menushop", func_801E9900);
