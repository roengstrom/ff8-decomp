#include "common.h"
#include "menu.h"

extern u8  D_801E3D84[];
extern u8  D_801E3D9C;
extern u8  D_801E3DA4[];
extern u8  D_801E3DB8;
extern u8  g_menuDisplayCfg[];
extern s32 g_menuColor;

extern void decodeMessage(u8 *src, u8 *dst, s32 mode);
extern s32  func_801F0FEC(s32 ctx, s32 state, s32 x, s32 y, u8 *buf, s32 mode);

extern void func_801E2A34();
extern void func_801E36AC();
extern void func_801E3AE0();

/**
 * @brief Render ability entry at computed grid position with table lookup.
 *
 * Divides index by 11 to get column and remainder. Computes y = rem * 13 + 0x42.
 * Looks up a value from D_801FA3C8 table using a3->0x2E divided by 64,
 * multiplies by 240/4096 to compute x position, adds 0xAD. Draws via
 * func_801F0A78.
 *
 * @param a0 Display context pointer.
 * @param a1 Ability list index.
 * @param a2 Unused (passed through to func_801F0A78).
 * @param a3 Pointer to data (reads halfword at offset 0x2E).
 */
typedef struct {
    u8  pad00[0x2E];
    s16 angle;
} MenuSlot;

extern u16  D_801FA3C8[];
extern void func_801F0A78(s32 ctx, s32 idx, s32 unused, s32 x, s32 y);

/**
 * @brief Render one cell of an ability grid at a per-slot X offset.
 *
 * Computes the cell's screen Y from its list index (row = @p i % 11;
 * @c y = row*13 + 0x42). Reads @p slot->angle, divides by 64 to index
 * @c D_801FA3C8 for a half-width, scales that by @c 240/4096 into MDC pixel
 * space (here written as @c 120*(w*2) to match the compiler's preferred
 * strength-reduction order), and offsets by @c 0xAD to get the final X.
 * Dispatches the cell to @c func_801F0A78 with @c (ctx, 0, unused, x, y).
 *
 * @param ctx    Rendering context, forwarded.
 * @param i      Zero-based list index; row is @c i%11.
 * @param unused Forwarded to @c func_801F0A78 untouched.
 * @param slot   Source object; @c slot->angle drives the X shift.
 */
void func_801E2800(s32 ctx, s32 i, s32 unused, MenuSlot *slot) {
    s32 lookup;
    s32 tableIdx;
    s32 y = (i % 11) * 13 + 0x42;

    lookup   = slot->angle;
    tableIdx = lookup / 64;
    lookup   = D_801FA3C8[tableIdx];
    func_801F0A78(ctx, 0, unused, 120 * (lookup * 2) / 4096 + 0xAD, y);
}

/**
 * @brief Render ability entry label at computed grid position (simple variant).
 *
 * Divides index by 11 to get remainder, computes y = remainder * 13 + 0x42,
 * then calls func_801F0A78 to draw.
 *
 * @param a0 Display context pointer.
 * @param a1 Ability list index.
 * @param a2 Unused.
 * @param a3 Unused.
 */
/**
 * @brief Render ability entry label at computed grid position (simple variant).
 *
 * Divides index by 11 to get remainder, computes y = remainder * 13 + 0x42,
 * then calls func_801F0A78 to draw.
 *
 * @param a0 Display context pointer.
 * @param a1 Ability list index.
 *
 */
void func_801E28B4(s32 a0, s32 a1, s32 a2) {
    s32 col = a1 / 11;
    s32 rem = a1 - col * 11;
    s32 y = rem * 13 + 0x42;
    func_801F0A78(a0, 0, a2, 0x23, y);
}

/** 8-byte ability-menu entry record; the byte at @c +5 is an id/flag
 *  (0xFF = empty, 0x80/0x81 = state-specific — see @c func_801E36AC). */
typedef struct {
    u8 data[8];
} AbilityEntry;

extern AbilityEntry D_8007CEE0[];

/**
 * @brief Address of the ability entry at index @p idx.
 */
AbilityEntry *func_801E2920(s32 idx) {
    return &D_8007CEE0[idx];
}

/**
 * @brief Read ability menu display count.
 * @return Value of D_801E3DB8.
 */
s32 func_801E2934(void) {
    return D_801E3DB8;
}

/**
 * @brief Look up an ability name string by index.
 *
 * If @p a0 is within bounds (less than D_801E3D9C), uses it to index
 * into D_801E3D84 to get an ability ID, then calls getAbilityDesc
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Ability list index.
 * @return Pointer to ability name string, or NULL if index out of bounds.
 */
s32 func_801E2944(s32 a0) {
    if (a0 >= D_801E3D9C) {
        return 0;
    }
    return getAbilityDesc(D_801E3D84[a0]);
}

/**
 * @brief Build list of equipped GF abilities for display.
 *
 * Scans 20 GF slots. For each slot, checks if the GF is unlocked
 * (D_80077CC8 offset 0xD22 bit 0) and whether the ability filter
 * (D_801E3D70) allows it. If valid and the ability count (offset 0x10)
 * is non-zero, adds the slot index to D_801E3DA4 and increments
 * the count in D_801E3DB8.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2990);

/**
 * @brief Main ability menu state machine handler.
 *
 * Handles a 26-case switch for ability menu states including navigation,
 * selection, GF ability assignment, and menu transitions. Uses division
 * by 11 for grid position calculations throughout. Manages cursor
 * movement, ability equip/unequip, and input processing.
 *
 * @param a0 Menu state pointer.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2A34);

/**
 * @brief Configure and draw an ability menu panel border.
 *
 * Sets up g_menuDisplayCfg with no icon, position from a2/a3,
 * fixed size (0xF4 x 0x12), then draws via func_801EF9AC.
 *
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @param a2 X position
 * @param a3 Y position
 */
void func_801E3530(s32 a0, s32 a1, s16 a2, s16 a3) {
    s32 cfg = (s32)g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a2;
    *(s16 *)(cfg + 0x4) = 0xF4;
    *(s16 *)(cfg + 0x6) = 0x12;
    *(s16 *)(cfg + 0x2) = a3;
    func_801EF9AC(a0, a1, 0x1000, g_menuColor);
}

/**
 * @brief Render ability list entry callback for scrolling panel.
 *
 * Called by the panel rendering system for each visible entry. Loads
 * a pointer from g_menuDisplayCfg+0x20, decodes it via decodeMessage into
 * a text buffer, then draws the text using func_801F0FEC with
 * color code 7.
 *
 * @param a0 Display context pointer.
 * @param a1 OT pointer.
 * @param a2 X position.
 * @param a3 Y position.
 * @param stackArg Display list pointer (5th arg on stack).
 * @return Updated display list pointer.
 */
/**
 * @brief Render an ability-list entry's name from @c g_menuDisplayCfg.dataPtr.
 *
 * Loads @c dataPtr[idx] from the list config (offset @c 0x20). If non-NULL,
 * decodes the name string via @c decodeMessage into a local buffer, then
 * renders it via @c func_801F0FEC at @c (cfg.x + yOff + 0xA, cfg.y + 9).
 * Returns the new display state from @c func_801F0FEC, or @p state unchanged
 * if the entry was NULL.
 *
 * @param ctx   Rendering context.
 * @param state Current display state (returned unchanged on NULL).
 * @param idx   Entry index into @c dataPtr.
 * @param unk3  Unused 4th register arg.
 * @param yOff  Extra Y offset (5th arg, passed on stack).
 */
s32 func_801E3580(s32 ctx, s32 state, s32 idx, s32 unk3, s32 yOff) {
    MenuDisplayConfig *cfg = (MenuDisplayConfig *)g_menuDisplayCfg;
    u8 **table = (u8 **)cfg->dataPtr;
    u8  buf[0x80];
    u8 *entry = table[idx];
    s32 result = state;
    s32 x, y;
    if (entry != NULL) {
        s32 adj = yOff + 0xA;
        x = cfg->x + adj;
        y = cfg->y + 9;
        decodeMessage(entry, buf, -1);
        result = func_801F0FEC(ctx, state, x, y, buf, 7);
    }
    return result;
}

/**
 * @brief Configure and draw an ability list panel with scrolling.
 *
 * Sets up g_menuDisplayCfg with icon type 0x55, position and scroll
 * parameters from the source data, then registers func_801E3580
 * as the rendering callback via func_801EFBB4.
 *
 * @param a0 Source data pointer (ability list state)
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Panel X position
 * @param stackArg Panel Y position (5th arg on stack)
 */
void func_801E3630(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    s32 cfg = (s32)g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0x55;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a3;
    *(s16 *)(cfg + 0x4) = 0x144;
    *(s16 *)(cfg + 0x6) = 0x1A;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x2) = stackArg;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x32);
    *(s32 *)(cfg + 0x20) = a0 + 0x20;
    func_801EFBB4(a1, a2, (s32)func_801E3580);
}

/**
 * @brief Render ability entry with conditional highlight in ability list.
 *
 * Computes x/y position from g_menuDisplayCfg base coordinates plus column/row
 * offsets derived from a2 (column * 11) and a3 (row * 13). If the computed
 * index is within bounds (D_801E3D9C), looks up the ability from D_801E3D84,
 * checks its type (0xFF = disabled, 0x80 = conditional, 0x81 = flag-based),
 * and determines the color code. Draws the entry via func_801F0FEC with
 * the ability name from getAbilityName.
 *
 * @param a0 Display context pointer.
 * @param a1 OT pointer.
 * @param a2 Column offset.
 * @param a3 Row offset.
 * @param stackArg X base offset.
 * @return Updated display list pointer.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E36AC);

/**
 * @brief Configure ability list panel with character ability data and render.
 *
 * Sets up g_menuDisplayCfg with icon type 0x5E, size 0x8A x 0xA0, 11 rows,
 * copies scroll parameters from source data (offsets 0x36, 0x37),
 * stores the total ability count and source pointer. If the count exceeds
 * 12, calls func_801F5F30/func_801F5F60 for scrollbar rendering.
 * Finally registers func_801E36AC as the rendering callback.
 *
 * @param a0 Source data pointer (character ability state).
 * @param a1 OT pointer for rendering.
 * @param a2 Column position parameter.
 * @param a3 Panel X position.
 * @param stackArg Panel Y position (5th arg on stack).
 */
void func_801E381C(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    s32 src = a0;
    s32 cfg = (s32)g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0x5E;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a3;
    *(s16 *)(cfg + 0x4) = 0x8A;
    *(s16 *)(cfg + 0x6) = 0xA0;
    *(u8 *)(cfg + 0x13) = 0xB;
    *(s16 *)(cfg + 0x2) = stackArg;
    *(u8 *)(cfg + 0x16) = *(u8 *)(src + 0x36);
    *(u8 *)(cfg + 0x17) = *(u8 *)(src + 0x37);
    *(s16 *)(cfg + 0x14) = *(u16 *)(src + 0x32);
    *(s32 *)(cfg + 0x20) = src;

    if (D_801E3D9C >= 0xC) {
        s32 scrollbar = func_801F5F30(a1, a2, a3 + 0x28, stackArg, g_menuColor, *(u8 *)(src + 0x36));
        a2 = func_801F5F60(a1, scrollbar, g_menuColor, 3);
    }
    func_801EFBB4(a1, a2, (s32)func_801E36AC);
}

/**
 * @brief Render GF ability name at computed position in ability list.
 *
 * Computes x/y position from g_menuDisplayCfg base coordinates plus column/row
 * offsets. If the computed index is within bounds (D_801E3DB8), looks up
 * the ability ID from D_801E3DA4, gets the name via func_801F6AA4,
 * and draws it using func_801F0FEC.
 *
 * @param a0 Display context pointer.
 * @param a1 OT pointer.
 * @param a2 Column index.
 * @param a3 Row index (multiplied by 13 for y offset).
 * @param stackArg X base offset added to g_menuDisplayCfg.x.
 * @return Updated display list pointer.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3904);

/**
 * @brief Configure GF ability list panel with scroll support and render.
 *
 * Sets up g_menuDisplayCfg with icon type 0x49, size 0xC6 x 0xA0, 11 rows,
 * copies scroll parameters from source data (offsets 0x38, 0x39),
 * stores the GF ability count and source pointer. If the count exceeds
 * 12, renders scrollbar via func_801F5F30/func_801F5F60. Scrollbar
 * arrow count depends on whether src+0x38 is zero (2 arrows) or not (1).
 * Registers func_801E3904 as the rendering callback.
 *
 * @param a0 Source data pointer (GF ability state).
 * @param a1 OT pointer for rendering.
 * @param a2 Column position parameter.
 * @param a3 Panel X position.
 * @param stackArg Panel Y position (5th arg on stack).
 */
/**
 * @brief Configure GF ability list panel with scroll support and render.
 *
 * Sets up g_menuDisplayCfg with icon type 0x49, size 0xC6 x 0xA0, 11 rows,
 * copies scroll parameters from source data (offsets 0x38, 0x39),
 * stores the GF ability count and source pointer. If the count exceeds
 * 12, renders scrollbar via func_801F5F30/func_801F5F60.
 * Registers func_801E3904 as the rendering callback.
 *
 * @param a0 Source data pointer (GF ability state).
 * @param a1 OT pointer for rendering.
 * @param a2 Column position parameter.
 * @param a3 Panel X position.
 * @param stackArg Panel Y position (5th arg on stack).
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E39E0);

/**
 * @brief Render complete ability menu page with panels and scrollbars.
 *
 * Called when the menu state is 0xE (active display). Saves GPU state,
 * loads character data, draws three panels (header, ability list, GF list),
 * with optional GF ability scrollbar if the GF ability count is under 0x1000.
 * Restores GPU state after rendering.
 *
 * @param a0 Menu state pointer (reads 0x2C, 0x2E offsets).
 * @param a1 OT/display list pointer.
 * @param a2 Column offset parameter.
 * @return Updated display list pointer, or a2 if state != 0xE.
 */
/**
 * @brief Render complete ability menu page with panels and scrollbars.
 *
 * Called when the menu state is 0xE (active display). Saves GPU state,
 * loads character data, draws three panels (header, ability list, GF list),
 * with optional GF ability scrollbar if the GF ability count is under 0x1000.
 * Restores GPU state after rendering.
 *
 * @param a0 Menu state pointer (reads 0x2C, 0x2E offsets).
 * @param a1 OT/display list pointer.
 * @param a2 Column offset parameter.
 * @return Updated display list pointer, or a2 if state != 0xE.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3AE0);

/**
 * @brief Scan ability bitmask and build list of available ability indices.
 *
 * Calls func_801F72B4 to get an ability bitmask, then iterates bits 0-23.
 * For each set bit, stores (bit_index + 0x5C) into D_801E3D84 array and
 * increments the count in D_801E3D9C.
 */
void func_801E3C28(void) {
    s32 mask = func_801F72B4();
    u8 *dst = D_801E3D84;
    D_801E3D9C = 0;
    {
        s32 i = 0;
        s32 one = 1;

        do {
            if (mask & (one << i)) {
                *dst = i + 0x5C;
                D_801E3D9C = D_801E3D9C + 1;
                dst++;
            }
            i++;
        } while (i < 0x18);
    }
}

/**
 * @brief Initialize ability menu: register callbacks and set up display state.
 *
 * Registers func_801E2A34 and func_801E3AE0 as callbacks via func_801F179C.
 * If registration succeeds, clears state fields (0x20, 0x2C), stores the
 * ability bitmask (0x28) and display size (0x2E = 0x1000), builds the ability
 * list, initializes data, and enters via func_801E2A34.
 */
void func_801E3C9C(void) {
    s32 result = func_801F179C((s32)func_801E2A34, (s32)func_801E3AE0);

    if (result != 0) {
        *(s16 *)(result + 0x2C) = 0;
        *(s32 *)(result + 0x20) = 0;
        *(s32 *)(result + 0x28) = func_801F72B4();
        *(s16 *)(result + 0x2E) = 0x1000;
        func_801E3C28();
        func_801E2990();
        func_801E2A34(result);
    }
}
