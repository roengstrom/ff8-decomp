#include "common.h"
#include "psxsdk/libetc.h"
#include "gamestate.h"
#include "menu.h"
#include "menushop.h"
#include "menumain.h"

#define SHOP_ITEM_COUNT 16
#define ITEM_SLOT_COUNT 198

typedef struct {
    u8 pad00[0x10];
    u16 state;          /**< 0x10: state machine current state (0-17). */
    u8 pad12[0xD];      /**< 0x12 */
    u8 *field_20;       /**< 0x20: pointer to a string. */
    u8 *field_24;       /**< 0x24: previus pointer stored by field_20. */
    u32 gil;            /**< 0x28: gil. */
    u8 *unk2C;          /**< 0x2C: pointer to the item slot inventory. */
    u8 *unk30;          /**< 0x30: pointer to a string. */
    u8 pad34[2];        /* 0x34 */
    u16 unk36;          /* 0x36 */
    u16 unk38;          /* 0x38 */
    s16 unk3A;          /**< 0x3A: scroll offset. */
    s16 rowCount[2];    /**< 0x3C: row count for buy and sell list. */
    u8 currentPage;     /**< 0x40: current page. */
    u8 previousPage;    /**< 0x41: previous page. */
    s8 unk42;           /**< 0x42: index to the row count (0 = buy, 1 = sell) */
    u8 pad43[2];        /* 0x43 */
    u8 unk45;           /* 0x45 */
    u8 unk46;           /**< 0x46: previous index to the row count (0 = buy, 1 = sell). */
    s8 pageCount;       /**< 0x47: page count. */
    u8 unk48;           /**< 0x48: item quantity to buy or sell */
    u8 unk49;           /**< 0x49: item quantity in the inventory. */
    u8 unk4A;           /* 0x4A */
    u8 pad4B[1];        /* 0x4B */
    u16 unk4C;          /* 0x4C */
    u16 unk4E;          /* 0x4E */
} ShopMenuState;

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

extern ShopData D_80077CC8[SHOP_COUNT]; /**< Shop data table. */
extern s32 D_80077E70;
extern u8 D_80077EBC[ITEM_SLOT_COUNT]; /**< Item slot inventory. */
extern u8 D_801F7F98[];
extern u16 D_801E9B64[4];
extern u8 D_801E9B6C[52];
extern ShopItemRarity D_801EA170[SHOP_COUNT][SHOP_ITEM_COUNT]; /**< Shop item rarity tables (shop.bin content). */
extern ShopItemPrice D_801EA3F0[ITEM_PRICE_COUNT]; /**< Item price table (price.bin content). */
extern MItemEntry D_801EA70C[199]; /**< Field-menu item table (mitem.bin content). */
extern ShopItemVisibility D_801EAA28[SHOP_ITEM_COUNT]; /**< Shop item visibility table. */
extern s32 D_801EAA48[ITEM_PRICE_COUNT]; /**< Item sell prices. */
extern s32 D_801EAD68[ITEM_PRICE_COUNT]; /**< Item buy prices. */

static s32 func_801E5800(ShopMenuState*, s32, s32);
static s32 func_801E583C(ShopMenuState*, s32, s32);
static s32 func_801E58A0(ShopMenuState*, s32, s32);
static s32 func_801E5904(s32);
static void func_801E5930(s32, s32, ShopMenuState*);
static s32 func_801E59D8(s16*, s16*, s32);
static s32 func_801E5A8C(s32, s32, s32, s32, s32, s16*, s32);
static void func_801E5BA4(s32, s32);
static void func_801E5DBC(void);
static void func_801E5E88(u8);
static void func_801E5E90(ShopMenuState*);
static void func_801E6A68(s32);
static void func_801E6ACC(void);
static void func_801E6C3C(s32);
static void func_801E6D54(s32);
static s32 func_801E6E0C(s32, s32, s32, s32);
static s32 func_801E6EB0(s32, s32, s32, s32, s32);
static s32 func_801E6F60(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E6FD8(s32, s32, s32, s32, s32);
static s32 func_801E722C(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E7374(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E7508(ShopMenuState*, s32, s32, s32, s32);
static s32 func_801E7628(ShopMenuState*, s32, s32, s32, s32);
static void func_801E791C(s32, s32, s32, s32, s32);
static s32 func_801E79D4(ShopMenuState*, s32, s32);
static void func_801E7B9C(s32);
static void func_801E7C8C(s32);
static s32 func_801E7E4C(s32);
static s32 func_801E7E98(s32, s32);

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
static s32 func_801E5800(ShopMenuState *arg0, s32 arg1, s32 arg2) {
    if (arg2 >= 198) {
        return 0;
    }

    if (arg1 == 0) {
        return D_801EAA28[arg2].itemId;
    }

    arg2 *= 2;
    return arg0->unk2C[arg2];
}

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
static s32 func_801E583C(ShopMenuState* a0, s32 a1, s32 a2) {
    s32 itemId;

    if (a1 == 0) {
        return D_801EAA28[a2].visible;
    }

    itemId = func_801E5800(a0, a1, a2);
    if (itemId == 0) {
        return 0;
    }

    return D_801EB088[itemId];
}

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
static s32 func_801E58A0(ShopMenuState *a0, s32 a1, s32 a2) {
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
static s32 func_801E5904(s32 a0) {
    return D_801F7F98[D_801EA70C[a0].type];
}

static void func_801E5930(s32 arg0, s32 arg1, ShopMenuState* arg2) {
    s32 unk36;
    s32 aux2;
    s32 aux1;

    aux1 = (arg1 % 8) * 0xD + 0x52;
    
    unk36 = (s16)arg2->unk36;
    aux2 = D_801FA3C8[(unk36 < 0 ? -unk36 : unk36) / 64];
    aux2 = (aux2 * 0x168) / 4096;
    
    func_801F0A34(arg0, 0, aux2 + 0x23, aux1);
}

static s32 func_801E59D8(s16* arg0, s16* arg1, s32 arg2) {
    s32 sum;
    s32 count;
    s16 val;
    s32 aux;

    sum = 0;
    count = 0;
    *arg1 = 0;
    arg1++;

    while (1) {
        val = *arg0;
        arg0++;

        if (val == -1) {
            break;
        }

        aux = func_801F08D4(1, arg2, (s32)val, 0);
        aux = getGlyphStatusU16(aux);
        aux += 0xA;       
        sum += aux;
        *arg1 = sum;
        arg1++;
        count++;
    }

    return count;
}

static s32 func_801E5A8C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s16* arg5, s32 arg6) {
    s16 buffer[36];
    s32 i;
    s32 count;
    s32 tmp1;
    s32 param;
    s32 tmp2;
    s32 arg3Val;
    s32 mask;

    arg3Val = arg3;
    count = func_801E59D8(arg5, buffer, arg4);
    for (i = 0; i < count; i++) {
        tmp1 = arg2 + buffer[i];
        mask = 1 << i;
        param = 1;
        if (arg6 & mask) {
            param = 7;
        }
        tmp2 = func_801F08D4(1, arg4, arg5[i], 0);
        arg1 = func_801F0FEC(arg0, arg1, tmp1, arg3Val, tmp2, param);
    }
    return arg1;
}

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
static void func_801E5BA4(s32 a0, s32 a1) {
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

    constPtr = (ItemSlot *)D_80077EBC;
    p = constPtr;
    
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
static void func_801E5DBC(void) {
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

static void func_801E5E88(u8 arg0) {
}

static void func_801E5E90(ShopMenuState *s) {
    u16 btnFlags;
    u32 cfgFlags;
    u16 *statePtr;
    u16 state;

    statePtr = &s->state;
    btnFlags = g_menuDisplayCfg.inputNew;
    cfgFlags = g_menuDisplayCfg.inputRepeat;
    state = s->state;

restart:
    switch (state & 0xFFFF) {
    case 0:
        s->unk30 = func_801F6AA4(0x40);
        s->unk38 = 0;
        *statePtr = 1;
        break;

    case 1:
        s->unk38 += 0x100;
        if ((s16)s->unk38 >= 0x1000) {
            s->unk38 = 0x1000;
            *statePtr = 2;
        }
        func_801E5BA4(1, s->unk42);
        break;

    case 2:
        s->unk42 = 0;
        *statePtr = 3;
        func_801E5BA4(1, s->unk42);
        break;

    case 3:
        if (btnFlags & PADLright) {
            sendSpuCommand(1);
            s->unk42 = func_80035B28(7, s->unk42);
        }

        if (btnFlags & PADLleft) {
            sendSpuCommand(1);
            s->unk42 = func_80035B70(7, s->unk42);
        }

        func_801E5BA4(1, s->unk42);

        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x10;
        }

        if (!(cfgFlags & PADRdown)) {
            break;
        }

        sendSpuCommand(2);

        if (s->unk42 == 2) {
            state = 16;
            goto restart;
        }

        s->unk46 = s->unk42;
        if (s->unk46 == 0) {
            s->unk30 = func_801F6AA4(0x42);
            s->pageCount = 2;
        } else {
            s->unk30 = func_801F6AA4(0x41);
            s->pageCount = 25;
        }

        s->currentPage = s->rowCount[s->unk46] / 8;
        func_801E5C08(s->gil);
        s->gil = func_801E5D28();
        *statePtr = 4;
        break;

    case 4:
        s->unk36 = 0xF00;
        *statePtr = 5;
        /* fallthrough */

    case 5:
        s->unk36 -= 0x100;
        if (((s16)s->unk36 << 0x10) <= 0) {
            s->unk36 = 0;
            *statePtr = 6;
        }
        func_801E5BA4(0, s->unk42);
        func_801E5930(1, s->rowCount[s->unk46], s);
        break;

    case 6: {
        s32 param;
        s16 dividend;
        s32 quotient;
        s32 rest;

        if (btnFlags != 0) {
            s->unk4E = 0;
        }

        param = 0x4A;
        if (s->unk4E != 0) {
            s->unk4E--;
            s->unk30 = func_801F6AA4(param);
        } else {
            param = 0x41;
            if (s->unk46 == 0) {
                param = 0x42;
            }
            s->unk30 = func_801F6AA4(param);
        }

        dividend = s->rowCount[s->unk46];
        rest = (s16)(dividend % 8);
        quotient = dividend / 8;
        s->rowCount[s->unk46]  = func_801F6768(btnFlags, 8, rest) + quotient * 8;

        s->field_20 = func_801E58A0(s, s->unk46, s->rowCount[s->unk46]);

        if (btnFlags & PADLleft) {
            state = 7;
            goto restart;
        }
        if (btnFlags & PADLright) {
            state = 9;
            goto restart;
        }
        func_801E5BA4(0, s->unk42);
        func_801E5930(1, s->rowCount[s->unk46], s);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            s->field_20 = 0;
            s->unk30 = func_801F6AA4(0x40);
            *statePtr = 14;
        }
        if (cfgFlags & PADRdown) {
            if (func_801E583C(s, s->unk46, s->rowCount[s->unk46])) {
                if (s->unk46 == 1 && s->rowCount[s->unk46] >= 0xC6) {
                    sendSpuCommand(5);
                    break;
                }
                state = 11;
                goto restart;
            }
            sendSpuCommand(5);
        }
        break;
    }

    case 11: {
        s32 index;
        u32 price;
        u32 count;

        index = func_801E5800(s, s->unk46, s->rowCount[s->unk46]);
        s->unk48 = 1;
        price = D_801EAD68[index];

        if (s->unk46 == 0) {
            if (index == 0) {
                sendSpuCommand(5);
                break;
            }

            if (s->gil < price) {
                s32 tmp;
                sendSpuCommand(5);
                tmp = func_801F6AA4(0x49);
                initSfxPlayback(0, tmp);
                func_801F23D0(0, 0x68, (u8 *)tmp);
                setSfxPitch(0, 0);
                startSfxNormal(0);
                s->unk4C = 0x258;
                *statePtr = 15;
                break;
            }

            if (D_801EB088[index] >= 100) {
                s32 tmp;
                sendSpuCommand(5);
                tmp = func_801F6AA4(0x48);
                func_801F23D0(0, 0x68, (u8 *)tmp);
                initSfxPlayback(0, tmp);
                setSfxPitch(0, 0);
                startSfxNormal(0);
                s->unk4C = 0x258;
                *statePtr = 15;
                break;
            }

            count = s->gil / price;
            if ((s32)count + D_801EB088[index] >= 100) {
                count = 100 - D_801EB088[index];
            }
        } else {
            if (index == 0) {
                sendSpuCommand(5);
                break;
            }
    
            count = D_801EB088[index];
        }

        sendSpuCommand(2);
        s->unk49 = count;
        s->unk4A = 0x40;
        if (s->unk46 == 0) {
            s->unk30 = func_801F6AA4(0x46);
        } else {
            s->unk30 = func_801F6AA4(0x47);
        }
        *statePtr = 12;
        break;
    }

    case 15:
        s->unk4C -= 1;
        if (cfgFlags & (PADRup | PADRdown)) {
            func_801F7BEC(cfgFlags);
            s->unk4C = 0;
        }
        if ((s16)s->unk4C <= 0) {
            fadeOutSfxFast(0);
            *statePtr = 6;
        }
        break;

    case 12:
        func_801E5BA4(0, s->unk42);
        func_801E5930(0, s->rowCount[s->unk46], s);

        if (btnFlags & PADLright) {
            if ((s8)s->unk48 < (s8)s->unk49) {
                s->unk48++;
                sendSpuCommand(1);
            }
        }

        if (btnFlags & PADLleft) {
            if ((s8)s->unk48 >= 2) {
                s->unk48--;
                sendSpuCommand(1);
            }
        }

        if (btnFlags & PADLdown) {
            if ((s8)s->unk48 >= 2) {
                sendSpuCommand(1);
                s->unk48 -= 10;
                if (((s8)s->unk48 << 24) <= 0) {
                    s->unk48 = 1;
                }
            }
        }

        if (btnFlags & PADLup) {
            if ((s8)s->unk48 < (s8)s->unk49) {
                s->unk48 += 10;
                sendSpuCommand(1);
                if ((s8)s->unk48 > (s8)s->unk49) {
                    s->unk48 = s->unk49;
                }
            }
        }

        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            *statePtr = 13;
        }

        if (cfgFlags & PADRdown) {
            s32 index;
            s32 price;
            if (s->unk46 == 0) {
                playSoundEffect(0x14);
                index = func_801E5800(s, s->unk46, s->rowCount[s->unk46]);
                price = D_801EAD68[index];
                price *= (s8)s->unk48;
                s->gil -= price;
                D_801EB088[index] += s->unk48;
            } else {
                if (s->rowCount[s->unk46] >= 0xC6) {
                    sendSpuCommand(5);
                    break;
                }

                playSoundEffect(0x14);
                index = func_801E5800(s, s->unk46, s->rowCount[s->unk46]);
                price = D_801EAA48[index];
                price *= (s8)s->unk48;
                s->gil += price;
                if (s->gil > 99999999) {
                    s->gil = 99999999;
                }

                D_801EB088[index] -= s->unk48;
            }
            s->unk4E = 0x3C;
            *statePtr = 13;
        }
        break;

    case 13:
        func_801E5BA4(0, s->unk42);
        func_801E5930(0, s->rowCount[s->unk46], s);
        s->unk4A = 0;
        *statePtr = 6;
        break;

    case 14:
        func_801E5BA4(0, s->unk42);
        s->unk36 += 0x100;
        if ((s16)s->unk36 >= 0x1000) {
            s->unk36 = 0x1000;
            *statePtr = 3;
        }
        func_801E5BA4(0, s->unk42);
        func_801E5930(1, s->rowCount[s->unk46], s);
        break;

    case 7: {
        s32 dividend;
        s32 rest;
        s32 quotient;
        func_801E5BA4(0, s->unk42);
        func_801E5930(1, s->rowCount[s->unk46], s);
        s->field_24 = s->field_20;
        dividend = s->rowCount[s->unk46];
        rest = (s16)(dividend % 8);
        quotient = dividend / 8;
        s->previousPage = quotient;
        quotient--;
        if (quotient < 0) {
            quotient = (u8)s->pageCount - 1;
        }
        s->rowCount[s->unk46] = rest + quotient * 8;
        s->currentPage = quotient;
        s->field_20 = func_801E58A0(s, s->unk46, s->rowCount[s->unk46]);
        s->unk3A = -0xE67;
        sendSpuCommand(1);
        *statePtr = 8;
        break;
    }

    case 8:
        func_801E5BA4(0, s->unk42);
        func_801E5930(1,  s->rowCount[s->unk46], s);
        s->unk3A += 0x199;
        if (s->unk3A >= 0) {
            s->unk3A = 0;
            *statePtr = 6;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 7;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 9;
        }
        break;

    case 9: {
        s32 dividend;
        s32 rest;
        s32 quotient;
        func_801E5BA4(0, s->unk42);
        func_801E5930(1, s->rowCount[s->unk46], s);
        s->field_24 = s->field_20;
        dividend = s->rowCount[s->unk46];
        rest = (s16)(dividend % 8);
        quotient = dividend / 8;
        s->previousPage = quotient;
        quotient++;
        if (quotient >= (u8)s->pageCount) {
            quotient = 0;
        }
        s->rowCount[s->unk46] = rest + quotient * 8;
        s->currentPage = quotient;
        s->field_20 = func_801E58A0(s, s->unk46, s->rowCount[s->unk46]);
        s->unk3A = 0x0E67;
        sendSpuCommand(1);
        *statePtr = 10;
        break;
    }

    case 10:
        func_801E5BA4(0, s->unk42);
        func_801E5930(1,  s->rowCount[s->unk46], s);
        s->unk3A -= 0x199;
        if (s->unk3A <= 0) {
            s->unk3A = 0;
            *statePtr = 6;
        }
        if (cfgFlags & PADLleft) {
            *statePtr = 7;
        }
        if (cfgFlags & PADLright) {
            *statePtr = 9;
        }
        break;

    case 16:
        s->unk30 = func_801F6AA4(0x44);
        *statePtr = 17;
        /* fallthrough */

    case 17:
        s->unk38 -= 0x100;
        if ((s16)s->unk38 << 0x10 <= 0) {
            s->unk38 = 0;
            func_801E5C08(s->gil);
            func_801E5E88(s->unk45);
            func_801F6888();
            func_801F7B60();
            func_801F18FC(s);
            func_801F0BB0();
        }
        func_801E5BA4(1, s->unk42);
        break;

    }

    func_801F0948((s16)s->unk38);
}

/**
 * @brief Populate shop item visibility data for a shop.
 *
 * Copies item IDs from the shop's rarity table and visibility flags from
 * the shop inventory into the shared visibility table.
 */
static void func_801E6A68(s32 shopId) {
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
static void func_801E6ACC(void) {
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
static void func_801E6C3C(s32 shopId) {
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

static void func_801E6D54(s32 arg0) {
    GameState *gs;
    u32 visited;
    s32 sum;

    if (arg0 != 0x15) {
        gs = &g_gameState;

        sum = gs->mainData.fieldCDC;

        visited = gs->shops[arg0].visited;

        sum += gs->mainData.fieldCE2;
        sum += gs->mainData.fieldCE0;
        sum &= 0xFFFF;
        if (sum == 0) {
            sum = 1;
        }

        gs->shops[arg0].visited = 1;

        // Dead code
        visited &= 0xFFFF;
        if (sum < visited) {
            sum += 0x10000;
        }
        sum -= visited;
        // End of dead code

        func_801E6A68(arg0);
        func_801E6C3C(arg0);
        func_801E6C3C(arg0);
        func_801E5DBC();
        func_801E6ACC();

        /* FIXME: Keep `sum` live here during all function calls (jal), forcing
           the compiler to allocate more space on the stack to reach desired
           0x20. */
        KEEP_ALIVE(sum);
    }
}

static s32 func_801E6E0C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 result;

    result = func_801E5A8C(arg0, arg1, arg2 + 0xC, arg3 + 5, 3, D_801E9B64, 7);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg2;
    g_menuDisplayCfg.y = arg3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.h = 0x15;
    return func_801EF9AC(arg0, result, 0x1000, g_menuColor);
}

static s32 func_801E6EB0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    u8 buffer[128];
    s32 msg;
    s32 x;
    s32 y;
    s32 xOffset;
    s32 result;

    msg = ((s32 *)(g_menuDisplayCfg.dataPtr))[arg2];
    result = arg1;
    if (msg != 0) {
        xOffset = arg4 + 10;
        x = g_menuDisplayCfg.x + xOffset;
        y = g_menuDisplayCfg.y + 5;
        decodeMessage(msg, buffer, -1);
        result = func_801F0FEC(arg0, arg1, x, y, buffer, 7);
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
static s32 func_801E6F60(ShopMenuState *s, s32 a1, s32 a2, s32 a3, s32 arg5) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x14;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = arg5;
    g_menuDisplayCfg.scrollOffset = s->unk3A;
    g_menuDisplayCfg.dataPtr = &s->field_20;
    {
        return func_801EFBB4(a1, a2, (s32)&func_801E6EB0);
    }
}

static s32 func_801E6FD8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    ShopMenuState *s;
    s32 index;
    s32 price;
    s32 count;
    s32 shopItemIdx;
    s32 xBase;
    s32 yBase;

    cfg = &g_menuDisplayCfg;
    shopItemIdx = arg2 * 8 + arg3;
    xBase = cfg->x + arg4;
    yBase = cfg->y + arg3 * 13;

    s = (ShopMenuState *)cfg->dataPtr;
    index = func_801E5800(s, s->unk46, shopItemIdx);

    if (s->unk46 == 1) { // selling
        price = D_801EAA48[index];
        count = D_801EB088[index];
        if (shopItemIdx >= 198) {
            return arg1;
        }
    }

    else { // buying
        price = D_801EAD68[index];
        count = D_801EAA28[shopItemIdx].visible;
    }

    if (index != 0 && count != 0) {
        s32 color;
        s32 statName;
        s32 x;
        s32 y;

        color = 7;
        if (s->gil < D_801EAD68[index] && s->unk46 == 0) {
            color = 1;
        }

        statName = getStatName(index);
        arg2 = func_801E5904(index);

        arg1 = func_8002FF34(arg0, arg1, arg2 + 0xDF, xBase + 0xB, yBase + 8, g_menuColor);

        x = xBase + 0x19;
        y = yBase + 0xA;
        arg1 = func_801F0FEC(arg0, arg1, x, y, statName, color);

        if (s->unk46 == 0) { // buying
            x = xBase + 0xF0;
            arg1 = drawColorByMenuPalette(arg0, arg1, (y << 0x10) | (x & 0xffff), price, color);
        }

        else { // selling
            x = xBase + 0xC8;
            arg1 = drawColorByMenuPalette(arg0, arg1, (y << 0x10) | (x & 0xffff), price, color);
            x = xBase + 0xF0;
            arg1 = drawColorByMenuPalette(arg0, arg1, (y << 0x10) | (x & 0xffff), count, color);
        }
    }

    return arg1;
}

static s32 func_801E722C(ShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    s32 color;

    cfg = &g_menuDisplayCfg;
    color = 7;

    if (s->unk46 == 0) {
        s32 x;
        s32 y;
        s32 result;
        s32 index;

        x = 8 + arg3;
        y = 9 + arg4;
        result = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(0x32), color);

        /* FIXME: Keep `x` live here to force `addiu` between `move` and `lbu`. */
        KEEP_ALIVE(x);

        x = 0x42 + arg3;

        /* FIXME: Keep `s` live here to force allocation to the $v0 register. */
        KEEP_ALIVE(s);

        index = func_801E5800(s, s->unk46, s->rowCount[s->unk46]);
        y = 0x16 + arg4;
        arg2 = drawColorByMenuPalette(arg1, result, (y << 0x10) | (x & 0xFFFF), D_801EB088[index], color);
    }

    cfg->iconType = 0x57;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->w = 0x50;
    cfg->y = arg4;
    cfg->h = 0x77;

    return func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

static s32 func_801E7374(ShopMenuState *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    g_menuDisplayCfg.iconType = 0x4C;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 0x100;
    g_menuDisplayCfg.h = 0x77;
    g_menuDisplayCfg.columnCount = 8;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.pageStart = arg0->currentPage;
    g_menuDisplayCfg.pageEnd = arg0->previousPage;
    g_menuDisplayCfg.scrollOffset = arg0->unk3A;
    g_menuDisplayCfg.dataPtr = arg0;

    if (arg0->unk46 == 0) {
        arg2 = func_8002FF34(arg1, arg2, 0x47, arg3 + 0xA8, arg4, g_menuColor);
        arg2 = func_801F5F30(arg1, arg2, arg3 + 0x1C, arg4, g_menuColor, (s8)arg0->currentPage);
    }
    else {
        arg2 = func_8002FF34(arg1, arg2, 0x47, arg3 + 0x80, arg4, g_menuColor);
        arg2 = func_8002FF34(arg1, arg2, 0x4D, arg3 + 0xD6, arg4, g_menuColor);
        arg2 = func_801F5EFC(arg1, arg2, arg3 + 0x1C, arg4, g_menuColor, (s8)arg0->currentPage);
    }
    
    arg2 = func_801F5F60(arg1, arg2, g_menuColor, 3);
    return func_801EFBB4(arg1, arg2, func_801E6FD8);
}

static s32 func_801E7508(ShopMenuState *s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 x;
    s32 y;

    x = arg3 + 0xD0;
    y = arg4 + 6;
    arg2 = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(0x33), 7);

    x = arg3 + 0x142;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), s->gil, 7);

    x = arg3 + 0x143;
    y = arg4 + 8;
    arg2 = func_8002FF34(arg1, arg2, 0xB, x, y, g_menuColor);

    g_menuDisplayCfg.iconType = 0x57;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = arg3;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.h = 0x17;

    return func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

static s32 func_801E7628(ShopMenuState* s, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 x;
    s32 y;
    s32 index;
    s32 color;
    s32 price;
    MenuDisplayConfig *cfg;

    color = 7;
    cfg = &g_menuDisplayCfg;
    if (s->unk4A == 0) {
        return arg2;
    }

    x = arg3 + 0xB;
    y = arg4 + 9;
    arg2 = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(0x37), color);

    y = arg4 + 0x23;
    arg2 = func_801F0FEC(arg1, arg2, x, y, func_801F6AA4(0x38), color);

    x = arg3 + 0x60;
    y = arg4 + 0x18;
    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), (s8)s->unk48, color);

    y = arg4 + 0x32;

    index = func_801E5800(s, s->unk46, s->rowCount[s->unk46]);
    if (s->unk46 == 0) {
        price = D_801EAD68[index];
    } else {
        price = D_801EAA48[index];
    }

    arg2 = drawColorByMenuPalette(arg1, arg2, (y << 0x10) | (x & 0xFFFF), price * (s8)s->unk48, color);

    cfg->iconType = 0x47;
    cfg->w = 0x68;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->y = arg4;
    cfg->h = 0x42;

    return func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

s32 func_801E77EC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;
    s32 name;
    s32 x;
    s32 y;
    s32 param1;
    s32 param2;
    s32 result;

    result = func_801EFFB8();

    /* FIXME: Keep `arg2` live here to force allocation to the $s2 register. */
    KEEP_ALIVE(arg2);

    if (result == 0x17) {
        param1 = 1;
        param2 = 0x150;
    } else {
        param1 = 0;
        param2 = 0xF4;
    }

    cfg = &g_menuDisplayCfg;

    if (arg0 >= 0) {
        name = func_801F6AA4(arg0 + 1);
    } else {
        name = func_801F6AA4(0x30);
    }

    x = (param2 - getGlyphStatusU16(name)) / 2;
    x += arg3;

    y = 5;
    y += arg4;

    arg2 = func_801F0FEC(arg1, arg2, x, y, name, 7);

    cfg->iconType = 0x49;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->y = arg4;

    if (param1 != 0) {
        cfg->w = 0x150;
        cfg->h = 0x14;
    } else {
        cfg->w = 0xF4;
        cfg->h = 0x14;
    }

    return func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

static void func_801E791C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    MenuDisplayConfig *cfg;

    cfg = &g_menuDisplayCfg;

    if (arg0 != 0) {
        arg2 = func_801F0FEC(arg1, arg2, arg3 + 0xC, arg4 + 5, arg0, 7);
    }
    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = arg3;
    cfg->w = 0xDA;
    cfg->y = (s16) arg4;
    cfg->h = 0x15;
    func_801EF9AC(arg1, arg2, 0x1000, g_menuColor);
}

static s32 func_801E79D4(ShopMenuState *s, s32 arg1, s32 arg2) {
    s32 result;
    s32 x;
    s32 y;
    s32 val;

    result = getDisplayListHead();

    val = (s16)s->unk36;
    val = D_801FA3C8[(val < 0 ? -val : val) / 64];

    func_801F1AFC();
    setMenuColorIntensity((s16)s->unk38);

    x = 0x18;
    y = 0x1D;
    arg2 = func_801E6E0C(arg1, arg2, x, y);

    x = 0x18;
    y = 8;
    arg2 = func_801E77EC(func_801EFFF0(), arg1, arg2, x, y);

    x = 0x1E;
    y = 0x33;
    result = func_801E6F60(s, arg1, result, x, y);

    x = 0x90;
    y = 0x50;
    result = func_801E7628(s, arg1, result, x, y);

    x = (val * 0x68 / 0x1000) + 0x118;
    y = 0x48;
    result = func_801E722C(s, arg1, result, x, y);

    x = (val * 0x168 / 0x1000) + 0x18;
    arg2 = func_801E7374(s, arg1, arg2, x, y);

    x = (val * 0x168 / 0x1000);
    x += 0x18;
    y = 0xC1;
    arg2 = func_801E7508(s, arg1, arg2, x, y);

    func_801F1B10();
    storeGpuPacket(result);
    return arg2;
}

void func_801E7B9C(s32 a0) {
    ShopMenuState *s;

    s = (ShopMenuState *)func_801F179C((s32)func_801E5E90, (s32)func_801E79D4);
    func_801F1D2C(0, "shop.bin", (s32)D_801EA170);
    func_801F1D2C(0, "price.bin", (s32)D_801EA3F0);
    func_801F1D2C(0, "mitem.bin", (s32)D_801EA70C);
    if (s != NULL) {
        s->unk2C = D_80077EBC;
        s->unk36 = 0x1000;
        s->unk30 = 0;
        s->gil = func_801E5D28();
        s->unk45 = D_801E9B6C[func_801EFFF0()];
        func_801E6D54(s->unk45);
        func_801E5E90(s);
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
static void func_801E7C8C(s32 a0) {
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

u8* func_801E7CFC(s32 arg0) {
    WeaponRecipe *ptr1;
    u8 *ptr2;

    ptr1 = D_801E9BA0;
    ptr2 = D_801E9D2C;

    ptr1 += arg0 & ~0xC0;
    ptr2 += ptr1->nameId;

    return ptr2;
}

void func_801E7D30(u8* src, u8* dst) {
    u8 buffer[127];
    u8* bufferPtr;
    s32 srcVal;

    while (1) {
        srcVal = *src;
        src++;

        if (srcVal == 0) {
            break;
        }

        bufferPtr = buffer;

        if (srcVal != 0xA) {
            *dst = srcVal;
            dst++;
        } else {
            srcVal = *src;
            src++;
            *buffer = 0;

            switch (srcVal) {
            case 38:
                copyString(bufferPtr, getLevelCurveData(D_801EB2E4));
                break;
            case 39:
                copyString(bufferPtr, getCharName(D_801EB2E8));
                break;
            }

            while (*bufferPtr != 0) {
                *dst = *bufferPtr;
                dst++;
                bufferPtr++;
            }
        }
    }

    *dst = 0;
}

/**
 * @brief Compute shop item price from table.
 *
 * Looks up byte at D_801E9BA0[a0*12 + 3], then returns (byte * 5) * 2.
 *
 * @param a0 Shop item index.
 * @return Computed price value.
 */
s32 func_801E7E1C(s32 arg0) {
    WeaponRecipe* ptr;
    ptr = D_801E9BA0;
    ptr += arg0;
    return  ptr->basePrice * 10;
}

/**
 * @brief Test if bit a0 is set in D_80077E70.
 *
 * @param a0 Bit index.
 * @return 1 if bit is set, 0 otherwise.
 */
static s32 func_801E7E4C(s32 a0) {
    s32 mask = 1 << a0;
    s32 val = D_80077E70 & mask;
    return val != 0;
}

/** @brief Return whether a1 >= func_801E7E1C(a0) (unsigned). */
s32 func_801E7E68(s32 a0, u32 a1) {
    return a1 >= (u32)func_801E7E1C(a0);
}

static s32 func_801E7E98(s32 arg0, s32 arg1) {
    WeaponRecipe *basePtr;
    u8 *ptr;
    s32 i;

    if (func_801E7E68(arg0, arg1) == 0) {
        return 0;
    }

    basePtr = D_801E9BA0;
    basePtr += arg0;

    ptr = basePtr->items;

    for (i = 0; i < 4; i++) {
        s32 unk0;
        s32 unk1;

        unk0 = *ptr;
        ptr++;
        unk1 = *ptr;
        ptr++;

        if (unk0 != 0 && unk1 > D_801EB088[unk0]) {
            return 0;
        }
    }

    g_gameState.mainData.party.unlockedWeapons |= 1 << arg0;
    return 1;
}

s32 func_801E7F4C(s32 arg0, s32 arg1) {
    s32 count;
    s32 i;
    u8* ptr1;
    u8* ptr2;
    WeaponInfo *ptr3;
    u8 value;

    count = 0;
    value = 0xFF;
    i = 7;

    ptr1 = D_801EB150;
    ptr1 += 7;

    for (; i >= 0; i--) {
        *ptr1 = value;
        ptr1--;
    }

    ptr2 = D_801EB150;
    ptr3 = D_8007C3B8;

    if ((0x3F >> arg0) & 1) {
        for (i = 0; i < 28; i++) {
            if (arg0 == ptr3[i].characterId) {
                s32 mask;
                mask = (func_801E7E4C(i) != 0) << 6;
                if (func_801E7E98(i, arg1) != 0) {
                    mask |= 0x80;
                }
                if (mask != 0) {
                    *ptr2 = i | mask;
                    ptr2++;
                    count++;
                }
            }
        }
    }

    return count;
}

s32 func_801E8058(s32 arg0) {
    WeaponInfo *ptr;
    s32 availableChars;
    s32 charBit;
    s32 ret;
    s32 i;

    if (D_8007809A & 1) {
        return 0;
    }

    ptr = D_8007C3B8;
    availableChars = func_80036EC0() & 0x3F;
    ret = 0;

    for (i = 0; i < 28; i++) {
        charBit = 1 << ptr[i].characterId;
        if (availableChars & charBit) {
            s32 val;
            val = func_801E7E4C(i);
            if (val) {
                ret |= charBit;
                continue;
            } else {
                val = func_801E7E98(i, arg0);
                if (val) {
                    ret |= charBit;
                }
            }
        }
    }

    return ret;
}

/**
 * @brief Render shop item entry at computed Y position with width 36.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 80 for Y position)
 */
void func_801E8134(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 36, a1 * 13 + 80);
}

/**
 * @brief Render shop item quantity at computed Y position with width 0xA9.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x4F for Y position)
 */
void func_801E816C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 169, a1 * 13 + 79);
}
