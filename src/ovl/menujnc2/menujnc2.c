#include "common.h"
#include "menu.h"
#include "gamestate.h"
#include "battle.h"

extern JunctionMenuEntry g_junctionChars[];
extern u8 g_junctionBackup[20];
extern BattleCharData g_junctionPreview;

/** @brief Junction menu layout constants (pixel positions). */
#define JNC_ROW_HEIGHT      13   /**< Row height in pixels. */
#define JNC_ROWS_PER_PAGE   4    /**< Rows visible per page. */
#define JNC_STAT_ROWS       11   /**< Rows in stat junction list. */
#define JNC_ABILITY_PAGES   5    /**< Rows per ability page. */
#define JNC_LEFT_COL_ROWS   3    /**< Rows in left stat column. */

#define JNC_Y_MAGIC_LIST    0x3F /**< Y origin for magic list panel. */
#define JNC_Y_STAT_LIST     0x42 /**< Y origin for stat junction list. */
#define JNC_Y_LEFT_COL      0x51 /**< Y origin for left stat column. */
#define JNC_Y_ABILITY       0x94 /**< Y origin for ability panel. */
#define JNC_Y_RIGHT_COL     0x99 /**< Y origin for right stat column. */

#define JNC_W_MAGIC_LIST    0x5A /**< Width of magic list entries. */
#define JNC_W_STAT_LIST     0xDC /**< Width of stat junction list. */
#define JNC_W_LEFT_COL      0x2B /**< Width of left stat column. */
#define JNC_W_RIGHT_COL     0x46 /**< Width of right stat column. */
#define JNC_W_ABILITY       0x28 /**< Width of ability entries. */
#define JNC_W_ABILITY_WIDE  0xC8 /**< Width of wide ability entries (page 3). */

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5800);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E59A4);

/**
 * @brief Dispatch ability rendering based on slot type, with optional looping.
 *
 * For JUNC_SLOT_DEF_ELEM, renders once per ability in sub-slot 1.
 * For JUNC_SLOT_DEF_STATUS, renders once per ability in sub-slot 0.
 * Otherwise renders once.
 *
 * @param charIdx Character index (0-7).
 * @param abilityList Ability list pointer.
 * @param slotType Type selector (JUNC_SLOT_DEF_ELEM, JUNC_SLOT_DEF_STATUS, or other).
 * @param pos Running output position (pass-through).
 * @return Updated output position.
 */
s32 renderJunctionSlots(s32 charIdx, s32 abilityList, s32 slotType, s32 pos) {
    s32 count;

    if (slotType == JUNC_SLOT_DEF_ELEM) {
        count = g_junctionChars[charIdx].abilityCount[1];
        if (count > 0) {
            do {
                pos = func_801E59A4(charIdx, abilityList, slotType);
                count--;
            } while (count > 0);
        }
    } else if (slotType == JUNC_SLOT_DEF_STATUS) {
        count = g_junctionChars[charIdx].abilityCount[0];
        if (count > 0) {
            do {
                pos = func_801E59A4(charIdx, abilityList, slotType);
                count--;
            } while (count > 0);
        }
    } else {
        pos = func_801E59A4(charIdx, abilityList, slotType);
    }
    return pos;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5D60);

/**
 * @brief Set junction slot count based on character GF compatibility.
 *
 * If the character has GF compatibility, allows 3 junction slots
 * (full). Otherwise limits to 2.
 *
 * @param ctx Junction menu context.
 */
void updateJunctionSlotCount(JunctionMenuCtx *ctx) {
    if (g_junctionChars[ctx->charIdx].gfCompat != 0) {
        ctx->slotCount = 3;
    } else {
        ctx->slotCount = 2;
    }
}

/**
 * @brief Save character junction data to backup buffer.
 *
 * Copies the 19 junction slots + padding byte into g_junctionBackup.
 *
 * @param charIdx Character index (0-7).
 */
void func_801E5F24(s32 charIdx) {
    s32 i;
    for (i = 0; i < 20; i++) {
        g_junctionBackup[i] = g_gameState.chars[charIdx].junctions[i];
    }
}

/**
 * @brief Restore character ability data from buffer.
 *
 * Copies 20 bytes from the junction buffer g_junctionBackup back into
 * character @p a0's ability data (g_gameState at offset 0x4EC, stride 152).
 *
 * @param a0 Character index (0-7).
 *
 * @note Non-matching: Compiler swaps register allocation for g_gameState
 * and g_junctionBackup base addresses (a2/v1 vs v1/a2).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5F78);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E5FCC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E61F8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6350);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E63FC);

/**
 * @brief Render magic list junction entry.
 * @param a0 X position parameter.
 * @param a1 Row index (wrapped to JNC_ROWS_PER_PAGE).
 */
void func_801E6534(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, JNC_W_MAGIC_LIST, (a1 % JNC_ROWS_PER_PAGE) * JNC_ROW_HEIGHT + JNC_Y_MAGIC_LIST);
}

/**
 * @brief Render stat junction entry in two-column layout.
 *
 * Left column (rows 0-2) or right column (rows 3-5).
 *
 * @param a0 X position parameter.
 * @param a1 Row index (0-5).
 * @param a2 Width offset to add.
 */
void func_801E6584(s32 a0, s32 a1, s32 a2) {
    s32 width;
    s32 y;
    if (a1 < JNC_LEFT_COL_ROWS) {
        width = JNC_W_LEFT_COL;
        y = (a1 + 1) * JNC_ROW_HEIGHT + JNC_Y_LEFT_COL;
    } else {
        a1 -= JNC_LEFT_COL_ROWS;
        width = JNC_W_RIGHT_COL;
        y = a1 * JNC_ROW_HEIGHT + JNC_Y_RIGHT_COL;
    }
    func_801F0A34(a0, 0, width + a2, y);
}

/**
 * @brief Render stat junction list entry.
 *
 * @param a0 Entry data pointer.
 * @param slotIdx Slot index (wrapped to JNC_STAT_ROWS).
 */
void func_801E65F0(s32 a0, s32 slotIdx) {
    slotIdx %= JNC_STAT_ROWS;
    func_801F0A34(a0, 0, JNC_W_STAT_LIST, slotIdx * JNC_ROW_HEIGHT + JNC_Y_STAT_LIST);
}

/**
 * @brief Look up junction ability availability mask for a given slot.
 *
 * Indexes into D_801EEAC0 to get an ability type (0-18), then checks
 * corresponding bit(s) in the junction flags word at g_junctionChars[a0 * 28].
 * Cases 0-8 test individual bits via (1 << type), case 9 tests 0x200,
 * case 10 tests 0x400, cases 11-14 test 0x6800, cases 15-18 test 0x19000.
 *
 * @param a0 Character/slot index.
 * @param a1 Ability slot offset into D_801EEAC0.
 * @return Masked flags value, or 0 if type >= 19.
 *
 * @note Non-matching: compiler adds extra andi for u8 masking, uses
 * different register allocation (a2 vs a0 for type), reorders switch
 * case bodies, and generates jump table at different rodata offset.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6658);

/**
 * @brief Render ability junction entry.
 *
 * Uses wide width for the last page (page 3).
 *
 * @param a0 X position parameter.
 * @param a1 Linear row index.
 */
void func_801E66F0(s32 a0, s32 a1) {
    s32 width = JNC_W_ABILITY;
    s32 page = a1 / JNC_ABILITY_PAGES;
    s32 row = a1 % JNC_ABILITY_PAGES;
    s32 y = row * JNC_ROW_HEIGHT + JNC_Y_ABILITY;

    if (page < 0) {
    } else if (page < JNC_LEFT_COL_ROWS) {
    } else if (page == JNC_LEFT_COL_ROWS) {
        width = JNC_W_ABILITY_WIDE;
    }
    func_801F0A34(a0, 0, width, y);
}

/**
 * @brief Get junction slot count based on slot type and character data.
 *
 * For slot type 0: reads g_junctionChars[a0*28 + 8] and returns val+1 if
 * nonzero, else 2. For slot type 1: reads g_junctionChars[a0*28 + 9] and
 * returns val+1 unless val+1 equals a1 (1), in which case returns 2.
 * Default returns 5.
 *
 * @param a0 Character/slot index.
 * @param a1 Slot type (0, 1, or other).
 * @return Slot count value.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E676C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E67EC);

/**
 * @brief Compute negative scroll offset from page index.
 *
 * Divides @p a0 by 5 to get the page number, clamps to a maximum of 2,
 * then returns the negative offset as -(page * 160).
 *
 * @param a0 Linear item index.
 * @return Negative pixel offset for scrolling (0, -160, or -320).
 */
s32 func_801E68AC(s32 a0) {
    s32 page = a0 / 5;
    if (page >= 3) {
        page = 2;
    }
    return -(page * 160);
}

/** @brief Draw inner panel with section id 0xB and clear flag. */
s32 func_801E68EC(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 0);
}

/**
 * @brief Draw inner panel with section id 0xB and set flag.
 * @param a0 Panel position parameter
 * @return Result of func_801F08D4
 */
s32 func_801E6918(s32 a0) {
    return func_801F08D4(1, 0xB, a0, 1);
}

/**
 * @brief Clear unlearned or out-of-range abilities from junction slots.
 *
 * Iterates through 4 ability slots at g_gameState[a0*152 + 0x4E0..0x4E3].
 * For each nonzero ability ID, checks if the ability is available
 * (learned via D_801EEFD0 bitmask) and in range [0x14, 0x27). If not
 * available or out of range, clears the slot to 0.
 *
 * @param a0 Character index.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6944);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E69E0);

/**
 * @brief Full junction menu refresh sequence.
 *
 * Calls func_801E61F8, func_801E6350, func_801E6944, and
 * func_801E69E0 in sequence with the party context.
 *
 * @param a0 Party context pointer.
 */
void func_801E6B88(u8 *a0) {
    func_801E61F8();
    func_801E6350(a0);
    func_801E6944(a0);
    func_801E69E0(a0);
}

/**
 * @brief Copy ability value from junction table to character data and update.
 *
 * Reads a 16-bit value from g_junctionChars[a0 * 28 + 4] and writes it to
 * g_gameState[a0 * 152 + 0x490], then calls func_801F5190 to update.
 *
 * @param a0 Character/slot index
 */
void func_801E6BC8(s32 a0) {


    s32 v0 = (s32)&g_junctionChars;
    s32 v1 = a0 * 28 + v0;
    s32 a1 = (s32)&g_gameState;

    *(s16 *)(a1 + a0 * 152 + 0x490) = *(u16 *)(v1 + 4);
    func_801F5190();
}

/**
 * @brief Preview a junction change and snapshot the resulting battle stats.
 *
 * Temporarily applies the junction menu's GF selection to the character,
 * recalculates stats, copies the result to the preview buffer, then
 * restores the original junction state.
 *
 * @param a0 Character index (0-7).
 */
void func_801E6C24(s32 a0) {
    u16 saved = g_gameState.chars[a0].junctedGfs;

    g_gameState.chars[a0].junctedGfs = g_junctionChars[a0].junctedGfs;
    func_801E6350(a0);
    func_801E6BC8(a0);
    func_8002A318(g_battleChars, &g_junctionPreview, sizeof(BattleCharData));
    g_gameState.chars[a0].junctedGfs = saved;
    func_801F1B4C(a0);
}

/**
 * @brief Copy ability value from junction table to character data and refresh.
 *
 * Reads a 16-bit value from g_junctionChars[a0 * 28 + 6] and writes it to
 * g_gameState[a0 * 152 + 0x4E8], then calls func_801E6B88 to refresh display.
 *
 * @param a0 Character/slot index
 */
void func_801E6CCC(s32 a0) {


    s32 base1 = (s32)&g_gameState;
    s32 off1 = a0 * 152;
    s32 base2 = (s32)&g_junctionChars;

    *(s16 *)(off1 + base1 + 0x4E8) = *(u16 *)(base2 + a0 * 28 + 6);
    func_801E6B88(a0);
}

/**
 * @brief Copy ability halfword from character data to junction table.
 *
 * Reads a 16-bit value from g_gameState[a0 * 152 + 0x4E8] and stores it
 * to g_junctionChars[a0 * 28 + 6].
 *
 * @param a0 Character/slot index.
 */
void func_801E6D28(s32 a0) {


    s32 base1 = (s32)&g_junctionChars;
    s32 v1 = a0 * 28;
    s32 base2 = (s32)&g_gameState;
    *(s16 *)(v1 + base1 + 6) = *(u16 *)(base2 + a0 * 152 + 0x4E8);
}

/**
 * @brief Store a halfword into junction table entry.
 *
 * Indexes into g_junctionChars at stride 28 (a0 * 28), and stores
 * a1 as a halfword at offset +4.
 *
 * @param a0 Junction entry index.
 * @param a1 Value to store.
 */
void func_801E6D6C(s32 a0, s32 a1) {

    s32 base = (s32)&g_junctionChars;
    *(u16 *)(base + a0 * 28 + 4) = a1;
}

/**
 * @brief Copy character ability data to junction table slot.
 *
 * Copies 4 bytes from character data g_gameState[a0*152 + 0x4E0] and
 * g_gameState[a0*152 + 0x4E4] into junction table g_junctionChars[a0*28 + a1*4 + 0xC]
 * and g_junctionChars[a0*28 + a1*4 + 0x14] respectively.
 *
 * @param a0 Character/slot index.
 * @param a1 Junction sub-slot (0 or 1).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6D8C);

/**
 * @brief Copy junction slot data back to character ability data.
 *
 * Copies 4 bytes from junction table g_junctionChars[a0*28 + a1*4 + 0xC] and
 * g_junctionChars[a0*28 + a1*4 + 0x14] into character data g_gameState[a0*152 + 0x4E0]
 * and g_gameState[a0*152 + 0x4E4] respectively.
 *
 * @param a0 Character/slot index.
 * @param a1 Junction sub-slot (0 or 1).
 */
void func_801E6E0C(s32 a0, s32 a1) {


    s32 i = 0;
    s32 charBase = (s32)&g_gameState;
    s32 a7 = charBase + a0 * 152;
    s32 jncBase = (s32)&g_junctionChars;
    s32 jnc = a0 * 28 + jncBase;
    s32 a4;

    a1 = a1 * 4 + jnc;
    a4 = a1 + 0x14;
    do {
        *(u8 *)(a7 + i + 0x4E0) = *(u8 *)(a1 + i + 0xC);
        *(u8 *)(a7 + i + 0x4E4) = *(u8 *)(a4 + i);
        i++;
    } while (i < 4);
}

/**
 * @brief Initialize junction entry and refresh display.
 *
 * Calls func_801E6D28, then func_801E6E0C with the context and
 * flag 0, then func_801E5F78 to refresh.
 *
 * @param a0 Junction context pointer.
 */
void func_801E6E88(u8 *a0) {
    func_801E6D28((s32)a0);
    func_801E6E0C(a0, 0);
    func_801E5F78(a0);
}

/**
 * @brief Reset junction slots and copy ability value from character data.
 *
 * Calls func_801E6D8C twice to clear both junction slots, then reads
 * the ability value from g_gameState[a0*152 + 0x490] and stores it
 * to the junction table via func_801E6D6C. Finally refreshes display
 * via func_801E5F24.
 *
 * @param a0 Character/slot index
 */
void func_801E6EC4(s32 a0) {

    s32 base;
    u16 val;
    func_801E6D8C(a0, 0);
    func_801E6D8C(a0, 1);
    base = (s32)&g_gameState;
    val = *(u16 *)(base + a0 * 152 + 0x490);
    func_801E6D6C(a0, val);
    func_801E5F24(a0);
}

/**
 * @brief Rebuild junction flags and stat limits from GF data.
 *
 * Iterates through 16 GFs, checking each GF bit in g_junctionChars[a0*28 + 6].
 * For each active GF, ORs its flag word from D_801EED10[gf*12] into the
 * combined flags, and updates the maximum stat byte indices from
 * D_801EED10[gf*12 + 6/7/8]. Stores the result back into g_junctionChars.
 *
 * @param a0 Character index.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E6F30);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7004);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E70C8);

/**
 * @brief Render stat delta bar for a junction change.
 *
 * Decodes stat names into two stack buffers, computes the stat difference
 * between current and new values, and renders as a progress bar.
 *
 * @param a0 Pointer to junction context (with stat table at +0x28, index at +0x4E).
 * @param a1 Render context parameter.
 * @param a2 Column index for rendering.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E717C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7228);

/**
 * @brief Toggle a GF junction for a character.
 *
 * Checks if GF bit (1 << a1) is already set in g_junctionChars[a0*28 + 6].
 * If set, returns 0. If not set, ORs the bit in, records the character
 * index at D_801EED10[a1*12 + 5], rebuilds ability table, updates display.
 *
 * @param a0 Character/slot index.
 * @param a1 GF index (0-15).
 * @return 1 if junction was toggled, 0 if already set.
 */
s32 func_801E72CC(s32 a0, s32 a1) {

    extern u8 D_801EED10[];
    s32 one = 1;
    s32 jncBase = (s32)&g_junctionChars;
    s32 jnc = a0 * 28 + jncBase;
    u16 flags = *(u16 *)(jnc + 6);
    s32 mask = one << a1;

    if (flags & mask) {
        return 0;
    }
    {
        s32 tblBase = (s32)D_801EED10;
        *(s16 *)(jnc + 6) = flags | mask;
        *(u8 *)(a1 * 12 + tblBase + 5) = a0;
    }
    func_801E6F30(a0);
    func_801E6E0C(a0, 1);
    func_801E6B88(a0);
    func_801E6C24(a0);
    return 1;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7370);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7498);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E75C0);

/**
 * @brief Conditionally executes a sequence of calls if func_801E75C0 succeeds
 *
 * Calls func_801E75C0 and if the result is non-zero, calls func_801E6C24,
 * func_801E6B88, then func_801E6C24 again, all with the same parameter.
 *
 * @param a0 Parameter passed to the three conditional calls
 * @return Result from func_801E75C0
 */
s32 func_801E76DC(s32 a0) {
    s32 result = func_801E75C0();

    if (result != 0) {
        func_801E6C24(a0);
        func_801E6B88(a0);
        func_801E6C24(a0);
    }
    return result;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7734);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E79A4);

/**
 * @brief Calculate junction menu navigation flags.
 *
 * Calls func_801E63FC to update ability lists, then determines a flag
 * byte based on available abilities (D_801EEF38 + D_801EEF9A counts)
 * and junction table state (g_junctionChars[a0*28 + 6], [+0], [+0xB]).
 *
 * @param a0 Character/slot index.
 * @return Navigation flag byte (combination of 0x1, 0x2, 0x4, 0x9).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7A80);

/**
 * @brief Initialize GF ability assignment table for a character.
 *
 * First fills D_801EEED0[0..0x38] with 0xFF, then iterates through
 * 32 GF ability pairs from D_80077818[a0*152], storing the pair index
 * into the corresponding slot if both ability bytes are nonzero.
 *
 * @param a0 Character index (0-7).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7B1C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801E7BA4);

/**
 * @brief Build junction ability flags from character data.
 *
 * Reads the base ability value (lower 7 bits of halfword at +0x1B4) and
 * tests 6 flag bits in the word at +0x18C to compose a bitmask:
 *   bit 0x01 -> 0x80, bit 0x04 -> 0x100, bit 0x08 -> 0x200,
 *   bit 0x200 -> 0x400, bit 0x4000 -> 0x800, bit 0x8000 -> 0x1000.
 *
 * @param data Pointer to character/junction data block.
 * @return Composed ability flags word.
 */
s32 func_801EA838(u8 *data) {
    s32 result = *(u16 *)(data + 0x1B4) & 0x7F;
    s32 flags = *(s32 *)(data + 0x18C);

    if (flags & 0x1) result |= 0x80;
    if (flags & 0x4) result |= 0x100;
    if (flags & 0x8) result |= 0x200;
    if (flags & 0x200) result |= 0x400;
    if (flags & 0x4000) result |= 0x800;
    if (flags & 0x8000) result |= 0x1000;
    return result;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EA898);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EAB84);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EAE3C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB118);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB3F4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB59C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB740);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB91C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EB9CC);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBA78);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBBF4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBD90);

/**
 * @brief Check junction ability mask compatibility.
 *
 * If a0 has any bits in common with a2, returns 7 (incompatible).
 * Otherwise returns whether a1 shares any bits with a2.
 *
 * @param a0 Current junction mask.
 * @param a1 Available abilities mask.
 * @param a2 Ability bit to check.
 * @return 7 if already junctioned, 1 if available, 0 if not.
 *
 */
s32 func_801EBEBC(s32 a0, s32 a1, s32 a2) {
    s32 result = 7;
    if (!(a0 & a2)) {
        result = (a1 & a2) != 0;
    }
    return result;
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EBED8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC280);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC358);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC434);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC50C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC5E4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EC914);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECC4C);

/**
 * @brief Configure display panel and invoke rendering callback.
 *
 * Sets up D_801FAB00 with the given position, fixed size (0x150 x 0x48),
 * clears icon fields, and calls func_801EF9AC with D_80083848 and a
 * caller-supplied parameter.
 *
 * @param a0 First parameter passed through to func_801EF9AC.
 * @param a1 Second parameter passed through to func_801EF9AC.
 * @param a2 X position for the display panel.
 * @param a3 Y position for the display panel.
 * @param a4 Fifth parameter passed as a2 to func_801EF9AC (on stack).
 */
void func_801ECE2C(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    extern u8 D_801FAB00[];
    extern u8 D_80083848[];
    s32 cfg = (s32)D_801FAB00;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)(cfg + 0) = a2;
    *(s16 *)(cfg + 4) = 0x150;
    *(s16 *)(cfg + 6) = 0x48;
    *(s16 *)(cfg + 2) = a3;
    func_801EF9AC(a0, a1, a4, *(s32 *)D_80083848);
}

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ECE80);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED078);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED150);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED314);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED454);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED68C);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801ED7F4);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDAA0);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDC88);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDDF8);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EDF04);

INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", func_801EE494);

/**
 * @brief Initialize and configure junction menu display.
 *
 * Sets up the main junction menu handler pair (func_801E7BA4/func_801EDF04),
 * allocates display context via func_801F5300, configures character data,
 * initializes ability tables for all 8 characters, and enters the main
 * junction processing loop.
 *
 * @param a0 Menu context pointer with character index at offset 0x20-0x22.
 */
void func_801EE718(s32 a0) {
    extern u8 D_801EEB28[];
    extern void func_801E7BA4();
    extern void func_801EDF04();
    s32 ctx;
    s32 i;

    ctx = func_801F179C((s32)func_801E7BA4, (s32)func_801EDF04);
    func_801F5300();
    if (ctx == 0) {
        goto end;
    }
    *(u16 *)(ctx + 0x30) = *(u16 *)(a0 + 0x20);
    *(u8 *)(ctx + 0x43) = *(u8 *)(a0 + 0x22);
    *(u16 *)(ctx + 0x32) = func_80036F60();
    *(u8 *)(ctx + 0x5B) = func_80035A6C(*(u16 *)(ctx + 0x32));
    *(u16 *)(ctx + 0x64) = 0;
    *(u8 *)(ctx + 0x61) = 0;
    *(u8 *)(ctx + 0x62) = 0;
    *(u8 *)(ctx + 0x42) = 0;
    *(u8 *)(ctx + 0x4E) = 0;
    *(u8 *)(ctx + 0x50) = 0;
    *(u16 *)(ctx + 0x40) = 0;
    *(s32 *)(ctx + 0x28) = (s32)D_801EEB28;
    *(u8 *)(ctx + 0x63) = 0;
    func_801E7004(*(u16 *)(ctx + 0x30));
    func_801EE494();

    i = 0;
    do {
        func_801E6F30(i);
        func_801E6B88(i);
        i++;
    } while (i < 8);

    func_801E6C24(*(u8 *)(ctx + 0x43));
    func_801E7734(*(u8 *)(ctx + 0x43), -1, -1, -1);
    func_801E7B1C(*(u8 *)(ctx + 0x43));
    func_801E7BA4(ctx);
end:
    func_801F0948(0x1000);
}

/**
 * @brief Initialize junction menu: set mode 1, configure display, enable flag.
 *
 * Sets up the junction menu display by calling initialization functions,
 * configuring display areas, setting the active flag D_801EED04 to 1,
 * then entering the main junction menu handler.
 *
 * @param a0 Menu context pointer
 */
void func_801EE82C(s32 a0) {
    extern u8 D_801EED04;

    func_801F1DBC(1);
    func_801E2ABC(a0);
    func_801F1210(0x801D1000, 0x801CD000);
    D_801EED04 = 1;
    func_801EE718(a0);
}

/**
 * @brief Reset junction menu state and reinitialize.
 *
 * Calls func_801F1DBC(1), clears D_801EED04, then calls
 * func_801EE718 with the context.
 *
 * @param a0 Junction context pointer.
 */
void func_801EE888(u8 *a0) {
    extern u8 D_801EED04;
    func_801F1DBC(1);
    D_801EED04 = 0;
    func_801EE718(a0);
}
