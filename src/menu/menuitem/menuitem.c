#include "common.h"
#include "menu.h"
#include "menuitem.h"
#include "game.h"
#include "btl_anim.h"

#define ITEM_LIST_ROWS 11
#define ITEM_LIST_ROW_HEIGHT 13

/**
 * Makes item @p id, of which @p count are held, the selected item of @p menu.
 * An empty slot (no id or none held) selects nothing. The callers only
 * match the original when @p count is read inside the do-block.
 */
#define SET_SELECTED_ITEM(menu, id, count)                \
    do {                                                  \
        s32 n = (count);                                  \
                                                          \
        (menu)->itemId = (id);                            \
        if ((id) != 0 && n != 0) {                        \
            (menu)->itemDesc = getItemDesc(id);           \
        } else {                                          \
            (menu)->itemDesc = NULL;                      \
        }                                                 \
    } while (0)

static s32 func_801E457C(s32 unused, ItemSlot *slots, s32 index);
static void func_801E476C(s32 a0, ItemMenu *menu);
static void func_801E47E0(s32 a0, s32 index);
static void func_801E4BB4(ItemMenu *menu);
static void func_801E4C14(ItemMenu *menu);
s32 func_801E80D0(s32, s32, s32, s32, s32);
s32 func_801E95C4(s32, s32, s32);

/** @brief Store item menu state pointer. */
void func_801E2800(s32 a0) {
    D_801ECE20 = a0;
}

/**
 * @brief Read item menu state pointer.
 *
 * @return Value of D_801ECE20.
 */
s32 func_801E280C(void) {
    return D_801ECE20;
}

/** @brief Look up string @p a0 in menu text category 0xB. */
u8 *func_801E281C(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2848);

/** @brief Store a2 to D_801ECE24 and call func_801E2848. */
void func_801E2BA4(s32 a0, s32 a1, s32 a2) {
    D_801ECE24 = a2;
    func_801E2848();
}

/**
 * @brief Store 4 configuration values to globals and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE30
 * @param a3 Value stored to D_801ECE2C
 * @param arg5 Value stored to D_801ECE38
 * @param arg6 Value stored to D_801ECE28
 */
void func_801E2BC8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {

    D_801ECE30 = a2;
    D_801ECE2C = a3;
    D_801ECE28 = arg6;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Store 3 configuration values and call func_801E2848.
 * @param a0 First parameter passed through to func_801E2848
 * @param a1 Second parameter passed through to func_801E2848
 * @param a2 Value stored to D_801ECE28
 * @param a3 Value stored to D_801ECE34
 * @param arg5 Value stored to D_801ECE38
 */
void func_801E2C0C(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {

    D_801ECE28 = a2;
    D_801ECE34 = a3;
    D_801ECE38 = arg5;
    func_801E2848(a0, a1);
}

/**
 * @brief Search byte pair array for a matching entry.
 *
 * Iterates through up to 198 pairs of consecutive bytes starting at @p a0.
 * For each pair, checks if the first byte equals @p a1 and the second byte
 * is greater than or equal to @p a2. Returns 1 on the first match found.
 *
 * @param a0 Pointer to byte pair array.
 * @param a1 Value to match against the first byte of each pair.
 * @param a2 Minimum threshold for the second byte of the matching pair.
 * @return 1 if a matching pair is found, 0 otherwise.
 */
s32 func_801E2C44(u8 *a0, s32 a1, s32 a2) {
    s32 i = 0;
    do {
        s32 b0 = *a0++;
        s32 b1 = *a0++;
        if (b0 == a1 && b1 >= a2) {
            return 1;
        }
    } while (++i < 0xC6);
    return 0;
}

/**
 * @brief Look up an item name string by index.
 *
 * If @p a0 is within bounds (less than D_801ECC10), uses it to index
 * into D_801ECB60 to get an item ID, then calls getAbilityDesc
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Item list index.
 * @return Pointer to item name string, or NULL if index out of bounds.
 */
u8 *func_801E2C80(s32 a0) {
    if (a0 < D_801ECC10) {
        return getAbilityDesc(D_801ECB60[a0 * 8]);
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2CCC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2D54);

/**
 * @brief Initialize 32 entries using a lookup table of offsets.
 *
 * For each of 32 iterations, reads a byte offset from the table at D_801ECB20
 * (stepping by 2 each iteration), adds a0, and stores the iteration index
 * at (a0 + offset - 1).
 *
 * @param a0 Base address for storing iteration indices.
 */
void func_801E2E04(s32 a0) {
    s32 i = 0;
    u8 *tbl = D_801ECB20;

    do {
        *(u8 *)(tbl[0] + a0 - 1) = i;
        i++;
        tbl += 2;
    } while (i < 0x20);
}

/**
 * @brief Read byte 0 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return First byte of the 4-byte entry.
 */
s32 func_801E2E38(s32 a0) {
    return D_801F889C[a0].b0;
}

/**
 * @brief Read byte 1 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Second byte of the 4-byte entry.
 */
s32 func_801E2E54(s32 a0) {
    return D_801F889C[a0].b1;
}

/**
 * @brief Read byte 2 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Third byte of the 4-byte entry.
 */
s32 func_801E2E70(s32 a0) {
    return D_801F889C[a0].b2;
}

/**
 * @brief Read byte 3 of item entry at index a0 from D_801F889C.
 * @param a0 Item entry index.
 * @return Fourth byte of the 4-byte entry.
 */
s32 func_801E2E8C(s32 a0) {
    return D_801F889C[a0].b3;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E2EA8);

/**
 * @brief Check if a specific item type should trigger an ability menu update.
 *
 * Loads item type from D_801F889C[a0*4] and checks against specific type IDs.
 * First calls func_801F79F8(0x40) as a precondition. For types 1, 2, 4, 5
 * returns 0. For type 6, checks func_801EFFD4 bit 0. Otherwise calls
 * func_801E2EA8 and returns bit 0 of its result.
 *
 * @param a0 Item index.
 * @return 0 for certain types, or bit 0 of func_801E2EA8 result.
 */
s32 func_801E2F88(s32 a0) {
    s32 type = D_801F889C[a0].b0;

    if (func_801F79F8(0x40) != 0) {
        if (type == 1) {
            return 0;
        }
        if (type == 2) {
            return 0;
        }
        if (type == 4) {
            return 0;
        }
        if (type == 5) {
            return 0;
        }
    }
    if (type == 6) {
        if ((func_801EFFD4() & 1) == 0) {
            return 0;
        }
    }
    return func_801E2EA8(a0) & 1;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E302C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3158);

/**
 * @brief Process input and update state with computed page index.
 *
 * Calls func_801F58EC and func_801F57A4. If the action is odd (bit 0 set),
 * divides func_801F58EC's result by 8 (signed, rounding toward zero),
 * calls func_801F5150 with the quotient and original result, then updates
 * via func_801F576C and func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if processed, 0 otherwise.
 */
/**
 * @brief Process input and update state with computed page index.
 *
 * Calls func_801F58EC and func_801F57A4. If the action is odd (bit 0 set),
 * divides func_801F58EC's result by 8 (signed, rounding toward zero),
 * calls func_801F5150 with the quotient and original result, then updates
 * via func_801F576C and func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if processed, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3288);

/**
 * @brief Process input event and update state if odd-numbered action.
 *
 * Calls func_801F58EC then func_801F57A4 to get an action code.
 * If the action is odd (bit 0 set), calls func_801F5150 with the action,
 * passes the result to func_801F576C, and finally calls func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if the action was processed (odd action), 0 otherwise.
 */
/**
 * @brief Process input event and update state if odd-numbered action.
 *
 * Calls func_801F58EC then func_801F57A4 to get an action code.
 * If the action is odd (bit 0 set), calls func_801F5150, passes the result
 * to func_801F576C, and finally calls func_801F5868.
 *
 * @param a0 Context pointer.
 * @return 1 if the action was processed (odd action), 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3314);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E338C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E347C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E35B8);

/**
 * @brief Process scroll input and update item list view state.
 *
 * Reads current and previous scroll positions, then checks button state.
 * If the 0x40 flag is set in func_801F79F8 and the action is odd, returns 0.
 * Otherwise extracts the 0x80 flag from the action, updates the display
 * via func_801F576C and func_801F5868. Returns 1 if either the position
 * or the 0x80 flag changed, 0 otherwise.
 *
 * @param a0 Context pointer.
 * @return 1 if state changed, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E37A4);

/**
 * @brief Set ability bit flag in character's ability table.
 *
 * Searches character a0's ability list (at g_kernel.junctionableGfs[a0]) for
 * ability a1. If found at index i, sets bit (1 << (i+8)) in the
 * corresponding word at D_80077408 + a0*68.
 *
 * @param a0 Character index.
 * @param a1 Ability ID to search for.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3854);

/**
 * @brief Clear ability bit flag in character's ability table.
 *
 * Searches character a0's ability list (at g_kernel.junctionableGfs[a0]) for
 * ability a1. If found at index i, clears bit (1 << (i+8)) in the
 * corresponding word at D_80077408 + a0*68.
 *
 * @param a0 Character index.
 * @param a1 Ability ID to search for.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E38DC);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3968);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3C1C);

/**
 * @brief Reset four item menu state words to -1.
 *
 * Sets D_801ECEDC, D_801ECEE4, D_801ECEE8, and D_801ECEE0 all to -1.
 */
void func_801E3E94(void) {

    D_801ECEDC = -1;
    D_801ECEE4 = -1;
    D_801ECEE8 = -1;
    D_801ECEE0 = -1;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E3EBC);

/**
 * @brief Process ability bits and accumulate results.
 *
 * Iterates through 8 bits of @p a2. For each set bit at position i,
 * calls func_801E3E94 to reset state, then func_801E3EBC(a0, a1, i)
 * and OR's the result into an accumulator. After the loop, calls
 * func_801E3E94 one final time and returns the accumulated result.
 *
 * @param a0 First parameter passed through to func_801E3EBC.
 * @param a1 Second parameter passed through to func_801E3EBC.
 * @param a2 Bitmask of abilities to process (low 8 bits).
 * @return OR'd result of all func_801E3EBC calls.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E42F8);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4394);

/**
 * @brief Uses up one item from an inventory slot.
 *
 * Clears the slot's item id once its count reaches zero.
 *
 * @param unused Unused.
 * @param slots Item inventory.
 * @param index Slot to take the item from.
 * @return 1 if the slot still holds items afterwards, otherwise 0.
 */
static s32 func_801E457C(s32 unused, ItemSlot *slots, s32 index) {
    ItemSlot *slot = &slots[index];
    s32 count = slot->count;
    s32 remaining = 0;

    if (count > 0) {
        count--;
        slot->count = count;
        if (count != 0) {
            remaining = 1;
        } else {
            slot->id = 0;
        }
    }
    return remaining;
}

/**
 * @brief Find and consume an item from the byte-pair table.
 *
 * Scans 198 byte-pair entries at @p a0 for one whose first byte matches @p a1.
 * If found, subtracts @p a2 from the second byte (quantity). If the quantity
 * reaches zero, also clears the first byte (item ID).
 *
 * @param a0 Pointer to byte-pair table (198 entries, 2 bytes each).
 * @param a1 Item ID to search for.
 * @param a2 Quantity to subtract.
 */
void func_801E45B4(u8 *a0, s32 a1, s32 a2) {
    s32 i = 0;
    u8 *p = a0 + 1;
    do {
        s32 id = a0[0];
        s32 qty = p[0];
        if (id != 0 && qty != 0 && id == a1) {
            qty -= a2;
            p[0] = qty;
            if (qty == 0) {
                a0[0] = 0;
            }
            return;
        }
        i++;
        p += 2;
        a0 += 2;
    } while (i < 0xC6);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4608);

/**
 * @brief Render item entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x8D for Y)
 */
void func_801E46B8(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC1, (a1 % 4) * 13 + 0x8D);
}

/**
 * @brief Render item at Y position from lookup table D_801EB1D8.
 *
 * Copies 9 entries from D_801EB1D8 to a local buffer, looks up
 * the entry at index a1, adds 0x32 to get the Y coordinate, then
 * calls func_801F0A34 to render.
 *
 * @param a0 X position parameter passed through.
 * @param a1 Index into the Y-offset table (0-8).
 */
void func_801E4708(s32 a0, s32 a1) {
    s16 buf[36];

    func_801F5984(D_801EB1D8, buf, 9);
    func_801F0A34(a0, 0, buf[a1] + 0x32, 0xD);
}

/**
 * @brief Draws the cursor next to the item list row of @p menu's second slot index.
 * @param a0 Passed through to func_801F0A34.
 * @param menu Item menu state.
 */
static void func_801E476C(s32 a0, ItemMenu *menu) {
    s32 index = menu->unk58;
    s32 y = index % ITEM_LIST_ROWS;

    y *= ITEM_LIST_ROW_HEIGHT;
    func_801F0A34(a0, 0, 205, y + 65);
}

/**
 * @brief Draws the cursor next to the item list row of slot @p index.
 * @param a0 Passed through to func_801F0A34.
 * @param index Slot index in the item list.
 */
static void func_801E47E0(s32 a0, s32 index) {
    s32 y = index % ITEM_LIST_ROWS;

    y *= ITEM_LIST_ROW_HEIGHT;
    func_801F0A34(a0, 0, 39, y + 65);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4848);

/**
 * @brief Render item at computed Y position based on row index.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E4908(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xC8, a1 * 13 + 0x42);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4940);

/**
 * @brief Render item entry at position derived from index parity and half-index
 *
 * Uses the low bit of a1 to select a column (multiplied by 82 + 0xC4 for width),
 * and a1/2 to select a row (multiplied by 13 + 0x40 for Y position).
 *
 * @param a0 X position parameter
 * @param a1 Linear index (bit 0 = column, upper bits / 2 = row)
 */
void func_801E49FC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, (a1 & 1) * 82 + 0xC4, (a1 / 2) * 13 + 0x40);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4A58);

/**
 * @brief Dispatch based on upper/lower 16 bits of flags.
 *
 * If upper 16 bits are zero, calls func_801E4908 (normal render).
 * If upper bits non-zero but lower 16 bits are zero, calls func_801E49FC.
 *
 * @param a0 X position parameter
 * @param a1 Row index parameter
 * @param a2 Combined flags (upper 16 = type, lower 16 = subtype)
 */
void func_801E4B38(s32 a0, s32 a1, s32 a2) {
    if ((a2 & ~0xFFFF) == 0) {
        func_801E4908(a0, a1);
    } else if ((a2 & 0xFFFF) == 0) {
        func_801E49FC(a0, a1);
    }
}

/**
 * @brief Dispatch to different handler based on mode flag.
 * @param a0 Mode flag: non-zero calls func_801E4A58, zero calls func_801E4940
 * @param a1 Parameter passed to the selected handler
 */
void func_801E4B80(s32 a0, s32 a1) {
    if (a0 != 0) {
        func_801E4A58(a1);
    } else {
        func_801E4940(a1);
    }
}

/**
 * @brief Loads the id and description of the item under the cursor.
 * @param menu Item menu state.
 */
static void func_801E4BB4(ItemMenu *menu) {
    ItemSlot *slot = &menu->items[menu->cursor];
    s32 id = slot->id;

    SET_SELECTED_ITEM(menu, id, slot->count);
}

/**
 * @brief Same as func_801E4BB4, for the slot at @p menu's second slot index.
 * @param menu Item menu state.
 */
static void func_801E4C14(ItemMenu *menu) {
    ItemSlot *slot = &menu->items[menu->unk58];
    s32 id = slot->id;

    SET_SELECTED_ITEM(menu, id, slot->count);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4C74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4D40);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E4EA4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7D18);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7E74);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E7F4C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8024);

/**
 * @brief Render an item description text for a specific table entry.
 *
 * Reads a data pointer from g_menuDisplayCfg[0x20] indexed by @p a2. If the pointer
 * is non-null, decodes the string via decodeMessage into a local buffer, then
 * renders it via func_801F0FEC at a position derived from g_menuDisplayCfg fields.
 *
 * @param a0 Rendering context pointer.
 * @param a1 Current rendering state (returned unchanged if entry is null).
 * @param a2 Table entry index.
 * @param arg5 Additional offset added to g_menuDisplayCfg X position.
 * @return Updated rendering state after text is rendered.
 */
INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E80D0);

/**
 * @brief Configure item list display and register render callback.
 *
 * Sets up g_menuDisplayCfg display config with icon 0x55, dimensions 0x144 x 0x1A,
 * scroll enabled, and page mode. Computes total item count from three fields
 * at offsets 0x48, 0x4C, and 0x50 of the source struct. Registers
 * func_801E80D0 as the display callback via func_801EFBB4.
 *
 * @param a0 Source data structure (offset 0x28 stored as data pointer).
 * @param a1 First callback parameter.
 * @param a2 Second callback parameter.
 * @param a3 X position for display config.
 * @param arg4 Y position for display config.
 */
void func_801E8180(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = a3;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x1A;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = arg4;
    g_menuDisplayCfg.scrollOffset = *(u16 *)(a0 + 0x48) + *(u16 *)(a0 + 0x4C) + *(u16 *)(a0 + 0x50);
    g_menuDisplayCfg.dataPtr = (s32)(a0 + 0x28);
    func_801EFBB4(a1, a2, func_801E80D0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E820C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", menuitem_drawGfPanel);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E83B4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E84A4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E859C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8684);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8780);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E88AC);

/**
 * @brief Look up sprite data address from table at 0x801D1000.
 *
 * Reads halfword offset at index a0 from the table, adds to base.
 *
 * @param a0 Sprite index.
 * @return Base address + halfword offset from table entry.
 */
s32 func_801E89A4(s32 a0) {
    s32 base = 0x801D1000;
    return *(u16 *)(base + a0 * 2 + 2) + base;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E89C0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8AF0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8C88);

/** @brief Return base address of item sprite data (0x801CD000). */
s32 func_801E8DA4(void) {
    return 0x801CD000;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8DB0);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8E98);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E8FA8);

/**
 * @brief Render complete item sprite with palette, tiles, text and effects.
 *
 * Chains several rendering functions: func_801E8E98 (palette/icon setup),
 * func_801E89C0 (sprite tiles), func_801E8AF0 (additional graphics),
 * then marks an Angelo trick as known (the item's byte at +0x1B is the
 * trick index, 0xFF meaning the item teaches none). Finally calls func_801E8DB0 (text labels),
 * func_801E8C88 (color overlay), func_8002B898 (border), and
 * func_801E8FA8 (effects). Each chained function receives the return
 * value of the previous one as its second argument.
 *
 * @param a0 Rendering context pointer.
 * @param a1 Initial rendering state.
 * @param a2 Display configuration pointer.
 * @param a3 OT (ordering table) pointer.
 * @param arg5 Item data pointer.
 */
void func_801E90D8(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5) {
    s32 ctx = a0;
    s32 cfg = a2;
    s32 ot = a3;
    s32 data = arg5;
    s32 result;
    s32 bit;

    result = func_801E8E98(a0, a1, a2, a3, data);
    result = func_801E89C0(ctx, result, cfg, ot, data);
    result = func_801E8AF0(ctx, result, cfg, ot, data);

    if (*(u8 *)(data + 0x1B) != 0xFF) {
        s32 trick = *(u8 *)(data + 0x1B);
        s32 known = g_gameState.mainData.limitBreaks.angeloKnown;
        bit = 1 << trick;
        g_gameState.mainData.limitBreaks.angeloKnown = known | bit;
    }

    result = func_801E8DB0(ctx, result, cfg, ot, data);
    result = func_801E8C88(ctx, result, cfg, ot, data);
    result = func_8002B898(ctx, result, cfg, ot);
    func_801E8FA8(ctx, result, cfg, ot, data);
}

/**
 * @brief Configure display rect from template and render item sprite data.
 *
 * Copies a 4-halfword rectangle template from @p arg5, adding @p a2 to the
 * X position and @p a3 to the Y position. Stores the result in g_menuDisplayCfg,
 * then calls func_801E90D8 to render with the display configuration and the
 * menu tint g_menuTint.
 *
 * @param a0 First parameter passed through to func_801E90D8.
 * @param a1 Second parameter passed through to func_801E90D8.
 * @param a2 X offset to add to template X.
 * @param a3 Y offset to add to template Y.
 * @param arg5 Pointer to 4-halfword rectangle template.
 */
void func_801E91E4(s32 a0, s32 a1, s32 a2, s32 a3, u16 *src) {
    g_menuDisplayCfg.x = src[0] + a2;
    g_menuDisplayCfg.y = src[1] + a3;
    g_menuDisplayCfg.w = src[2];
    g_menuDisplayCfg.h = src[3];
    func_801E90D8(a0, a1, &g_menuDisplayCfg, g_menuTint[MENU_TINT_NORMAL], src);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9248);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E934C);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E95C4);

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9AAC);

/**
 * @brief Initialize item menu system.
 *
 * Sets up the main item handler and three data tables, clears six global
 * state words, calls display init, resets menu state. If the handler setup
 * returned a context pointer, initializes it with item data and renders.
 */
void func_801E9B98(void) {
    s32 ctx;

    ctx = func_801F179C(func_801E4EA4, func_801E95C4);
    func_801F1D2C(0, D_801EB17C, D_801EB330);
    func_801F1D2C(0, D_801EB188, D_801EB4BC);
    func_801F1D2C(0, D_801EB194, D_801EC710);
    D_801ECE24 = 0;
    D_801ECE28 = 0;
    D_801ECE2C = 0;
    D_801ECE30 = 0;
    D_801ECE34 = 0;
    D_801ECE38 = 0;
    func_801F0948(0);
    func_801E3E94();
    if (ctx != 0) {
        func_801E9AAC(ctx);
        loadOverlayWithTimCallback(0xC, 0x801D5000);
        *(s16 *)(ctx + 0x40) = 0;
        func_801E4EA4(ctx);
        *(s16 *)(ctx + 0x3C) = 0;
    }
    func_801F7B60();
}

/**
 * @brief Length of a string, scanning at most @p max bytes.
 * @param str String to measure.
 * @param max Size of the buffer holding @p str.
 * @return Number of bytes before the terminator, at most @p max - 1.
 */
s32 func_801E9C90(u8 *str, s32 max) {
    s32 i;
    s32 len = 0;

    for (i = 0; i < max; i++) {
        if (*str++ == 0) {
            break;
        }
        len++;
    }
    if (len >= max) {
        len = max - 1;
    }
    return len;
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9CD4);

/**
 * @brief Find the last non-zero byte in a string and set it to zero.
 *
 * Scans forward through the byte string at a0 until the first zero byte,
 * then writes zero to the byte before the terminator (the last non-zero byte).
 * If the first byte is already zero, does nothing.
 *
 * @param a0 Pointer to a null-terminated byte string.
 */
void func_801E9DE4(u8 *a0) {
    if (*a0++ == 0) {
        return;
    }
    while (*a0++ != 0) {
    }
    a0[-2] = 0;
}

/**
 * @brief Checks whether a string holds anything besides the filler character.
 *
 * The filler is the first character of menu string 11.
 *
 * @param str String to check.
 * @return 1 if @p str has a character other than the filler, 0 if it is
 *         empty or all filler.
 * @note A guess: likely used to reject blank names.
 */
s32 func_801E9E10(u8 *str) {
    s32 c;

    if (btlStrlen(str) == 0) {
        return 0;
    }
    while (1) {
        c = *str++;
        if (c == 0) {
            return 0;
        }
        if (c != *getMenuString(11)) {
            return 1;
        }
    }
}

/** @brief Look up string @p a0 in menu text category 5. */
u8 *func_801E9E7C(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

INCLUDE_ASM("asm/ovl/menuitem/nonmatchings/menuitem", func_801E9EA8);

/**
 * @brief Initialize item sub-menu.
 *
 * Sets up the sub-menu handler via func_801F179C, initializes display state,
 * reads button input to determine item type. If the context pointer is valid,
 * sets up the data table pointer, string, and various byte fields, then
 * calls func_801E9F94 to render.
 */
