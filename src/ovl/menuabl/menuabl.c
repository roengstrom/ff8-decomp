#include "common.h"

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
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2800);

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

/**
 * @brief Compute pointer to ability data entry by index.
 * @param a0 Ability entry index (stride 8).
 * @return Address of D_8007CEE0 + a0 * 8.
 */
s32 func_801E2920(s32 a0) {
    extern u8 D_8007CEE0[];
    return D_8007CEE0 + a0 * 8;
}

/**
 * @brief Read ability menu display count.
 * @return Value of D_801E3DB8.
 */
s32 func_801E2934(void) {
    extern u8 D_801E3DB8;
    return D_801E3DB8;
}

/**
 * @brief Look up an ability name string by index.
 *
 * If @p a0 is within bounds (less than D_801E3D9C), uses it to index
 * into D_801E3D84 to get an ability ID, then calls func_80020AD4
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Ability list index.
 * @return Pointer to ability name string, or NULL if index out of bounds.
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2944);

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
    extern u8 g_menuDisplayCfg[];
    extern s32 g_menuColor;

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
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3580);

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
    extern u8 g_menuDisplayCfg[];
    extern void func_801E3580();

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
 * the ability name from func_800209A0.
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
    extern u8 g_menuDisplayCfg[];
    extern u8 D_801E3D9C;
    extern s32 g_menuColor;
    extern void func_801E36AC();

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
    extern u8 D_801E3D84[];
    extern u8 D_801E3D9C;

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
    extern void func_801E2A34();
    extern void func_801E3AE0();
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
