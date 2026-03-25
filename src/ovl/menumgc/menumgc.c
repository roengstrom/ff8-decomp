#include "common.h"
#include "character.h"
#include "gamestate.h"
#include "gf.h"

extern s32 getMagicNamePtr(s32 a0);
extern s32 getCharNamePtr(s32 a0);
extern void copyString(s32 a0, s32 a1);
extern s32 func_801F79F8(s32 a0);

typedef struct {
    u8 flags;
    u8 pad[3];
} FlagEntry;

typedef struct { u8 unk00[0x14]; } Unk14;
typedef struct { u8 unk00[0x40]; } Unk40;
typedef struct { u8 unk00[0x98]; } Unk98;

extern FlagEntry D_801F87B8[];
extern GfData g_gfData;
extern Unk98 D_80077818[];
extern Unk98 D_80077864[];
extern Unk14 D_801ECF60[];
extern Unk40 D_801ECF90[];
extern s16 D_801ED010[];
extern void func_801F1B4C(s32 a0);
extern void func_801F5400(s32 a0);

/**
 * @brief Format a string with escape sequence substitution.
 *
 * Copies bytes from @p src to @p dst, expanding two escape sequences:
 * - `0x0A 0x26`: substitutes a numeric value derived from @p spellId via
 *   getMagicNamePtr, converted to a string by copyString.
 * - `0x0A 0x27`: substitutes the name of the character identified by
 *   g_gameState.chars[@p charIdx].characterId, looked up via getCharNamePtr
 *   and converted to a string by copyString.
 * Unrecognized escape sequences are silently skipped.
 * The output string is null-terminated.
 *
 * @param src     Source string to format (null-terminated, may contain escape sequences).
 * @param dst     Destination buffer to write the formatted string.
 * @param spellId Magic spell ID passed to getMagicNamePtr for the 0x26 substitution.
 * @param charIdx Character index for the 0x27 name substitution.
 */
void menumgc_formatSpellCharName(u8 *src, u8 *dst, s32 spellId, s32 charIdx) {
    u8 local_buf[0x40];
    u8 *p;

    while (1) {
        s32 ch = *src++;
        if (ch == 0)
            break;
        p = local_buf;
        if (ch == 0xA)
            goto escape;
        *dst++ = ch;
        continue;
escape:
        {
            s32 esc = *src++;
            local_buf[0] = 0;
            if (esc == 0x26) goto esc26;
            if (esc == 0x27) goto esc27;
            goto copy_check;
esc26:
            {
                s32 val = getMagicNamePtr(spellId);
                copyString((s32)local_buf, val);
                goto copy_check;
            }
esc27:
            {
                s32 val = getCharNamePtr(g_gameState.chars[charIdx].characterId);
                copyString((s32)local_buf, val);
            }
copy_check:
            while (*p)
                *dst++ = *p++;
        }
    }
    *dst = 0;
}

/**
 * @brief Get available GF mask filtered by character status flags.
 *
 * Calls func_801F22F4 and menumain_getPartyMemberMask to get base masks, ANDs them,
 * then iterates through 8 characters. For each, if the character has
 * STATUS_PETRIFY or STATUS_SILENCE set, clears that character's bit from
 * the result mask.
 *
 * @return Filtered GF availability bitmask.
 */
s32 _getJunctionableCharMask(void) {
    s32 result;
    s32 i;
    s32 bit;

    result = func_801F22F4();
    result &= menumain_getPartyMemberMask();
    i = 0;
    bit = 1;
    do {
        u16 flags = g_gameState.chars[i].statusFlags;
        if (flags & STATUS_PETRIFY) {
            result &= ~(bit << i);
        }
        if (flags & STATUS_SILENCE) {
            result &= ~(bit << i);
        }
        i++;
    } while (i < 8);
    return result;
}

/**
 * @brief Check if a GF slot is available for junctioning.
 *
 * Reads the availability flag from D_801F87B8[idx]. If button 0x40 is held
 * (func_801F79F8), returns the flag directly. Otherwise checks the GF type
 * field in the GfJunctionEntry table (g_gfData + 0x21C): if the type is
 * 5 or 6, returns 0 (unavailable); otherwise returns the flag.
 *
 * @param idx GF/slot index.
 * @return 1 if available, 0 if not.
 */
s32 func_801E599C(s32 idx) {
    s32 result = D_801F87B8[idx].flags & 1;
    if (func_801F79F8(0x40) != 0) {
        s32 gfType = g_gfData.junctionData[idx].pad07;
        if (gfType >= 7) goto done;
        if (gfType < 5) goto done;
        result = 0;
done:;
    }
    return result;
}

/**
 * @brief Copy character junction and magic data into menu display buffers.
 *
 * @param arg0 Character index (0–7).
 * @param arg1 Menu display slot index.
 */
void func_801E5A28(s32 arg0, s32 arg1) {
    s32 i;
    u8 *src;
    u8 *dst;

    D_801ED010[arg1] = g_gameState.chars[arg0].currentHp;

    src = g_gameState.chars[arg0].junctions;
    dst = D_801ECF60[arg1].unk00;
    for (i = 0; i < 0x13; i++) {
        *dst = *src;
        src++;
        dst++;
    }

    src = D_80077818[arg0].unk00;
    dst = D_801ECF90[arg1].unk00;
    for (i = 0; i < 0x40; i++) {
        *dst = *src;
        src++;
        dst++;
    }
}

/**
 * @brief Copy menu display buffers back to character junction and magic data.
 *
 * Reverse of func_801E5A28: writes cached HP, junction magic IDs, and magic
 * inventory from menu buffers into the character save data.
 *
 * @param arg0 Character index (0–7).
 * @param arg1 Menu display slot index.
 */
void func_801E5B00(s32 arg0, s32 arg1) {
    s32 i;
    u8 *src;
    u8 *dst;

    dst = D_80077864[arg0].unk00;
    src = D_801ECF60[arg1].unk00;
    for (i = 0; i < 0x13; i++) {
        *dst = *src;
        src++;
        dst++;
    }

    g_gameState.chars[arg0].currentHp = D_801ED010[arg1];
    dst = (u8 *)g_gameState.chars[arg0].magic;
    src = D_801ECF90[arg1].unk00;
    for (i = 0; i < 0x40; i++) {
        *dst = *src;
        src++;
        dst++;
    }

    func_801F1B4C(arg0);
    func_801F5400(arg0);
}

/**
 * @brief Search for a magic spell by ID in a character's spell table.
 *
 * Iterates through 32 MagicSlot entries (g_characterMagic + charIdx * 152)
 * looking for a matching magicId. Returns the slot index if found, 0 if not.
 *
 * @param charIdx Character index (0-7).
 * @param spellId Magic spell ID to search for.
 * @return Slot index (1-31) if found, 0 if not found.
 */
s32 func_801E5C00(s32 charIdx, s32 spellId) {
    u8 *ptr = D_80077818[charIdx].unk00;
    s32 i = 0;

    do {
        if (spellId == *ptr) return i;
        i++;
        ptr += 2;
    } while (i < MAGIC_SLOT_COUNT);

    return 0;
}

/**
 * @brief Search a character's junction slots for a specific spell ID.
 *
 * Searches through the 19 junction stat slots (HP through DefStatus)
 * in D_80077864 for the given spell ID.
 *
 * @param charIdx Character index (0-7).
 * @param spellId Spell ID to search for (returns -1 if 0).
 * @return Junction slot index (0-18) if found, -1 if not found or spellId is 0.
 */
s32 func_801E5C50(s32 charIdx, s32 spellId) {
    u8 *ptr = D_80077864[charIdx].unk00;
    s32 i;
    s32 result;

    if (spellId == 0) {
        result = -1;
        goto end;
    }
    i = 0;
    do {
        if (*ptr == spellId) { result = i; goto end; }
        i++;
        ptr++;
    } while (i < 0x13);
    result = -1;
end:
    return result;
}

/**
 * @brief Search a character's magic inventory for a spell and return its quantity.
 *
 * Searches through 32 magic slots in D_80077818 for the given spell ID.
 * Returns the quantity if found, 0 if not found or spellId is 0.
 *
 * @param charIdx Character index (0-7).
 * @param spellId Spell ID to search for.
 * @return Quantity of the spell, or 0 if not found.
 */
s32 func_801E5CAC(s32 charIdx, s32 spellId) {
    u8 *ptr = D_80077818[charIdx].unk00;
    s32 i;
    s32 result;

    if (spellId == 0) { result = 0; goto end; }
    i = 0;
    do {
        u8 magicId = *ptr;
        ptr++;
        if (spellId == magicId) {
            result = *ptr;
            goto end;
        }
        i++;
        ptr++;
    } while (i < MAGIC_SLOT_COUNT);
    result = 0;
end:
    return result;
}

/**
 * @brief Check if either of two magic availability tests pass.
 * @param a0 First character context
 * @param a1 Second character context
 * @param a2 Second magic ID
 * @param a3 First magic ID
 * @return 1 if either func_801E5CAC call returns non-zero, 0 otherwise
 */
s32 func_801E5D14(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 r1 = func_801E5CAC(a0, a3);
    s32 r2 = func_801E5CAC(a1, a2);
    return (r1 | r2) != 0;
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5D64);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5E74);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5F5C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6058);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E626C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E64FC);

/**
 * @brief Look up a magic spell ID for a character's slot.
 *
 * Returns the spell ID if the slot has a nonzero quantity, otherwise 0.
 *
 * @param charIdx Character index (0-7).
 * @param slotIdx Magic slot index (0-31).
 * @return Spell ID byte, or 0 if slot is empty.
 */
s32 func_801E6648(s32 charIdx, s32 slotIdx) {
    if (g_gameState.chars[charIdx].magic[slotIdx].quantity == 0) {
        return 0;
    }
    return g_gameState.chars[charIdx].magic[slotIdx].magicId;
}

/**
 * @brief Test if magic bit a0 is available.
 *
 * Computes (1 << a0), calls _getJunctionableCharMask to get available mask,
 * and returns whether the bit is set.
 *
 * @param a0 Bit index to test.
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_801E668C(s32 a0) {
    s32 mask = 1 << a0;
    mask &= _getJunctionableCharMask();
    return mask != 0;
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E66C0);

/**
 * @brief Get the quantity of a magic spell in a character's slot.
 *
 * @param charIdx Character index (0-7).
 * @param slotIdx Magic slot index (0-31).
 * @return Quantity of spells stocked in the slot.
 */
u8 func_801E677C(s32 charIdx, s32 slotIdx) {
    return g_gameState.chars[charIdx].magic[slotIdx].quantity;
}

/**
 * @brief Set a character's magic slot spell ID.
 *
 * @param charIndex Character index (0–7, see CharacterId).
 * @param slot      Magic slot index (0–31).
 * @param value     Magic spell ID to store (see MAGIC_* defines).
 */
void setCharacterMagicSlotId(s32 charIndex, s32 slot, u8 value) {
    g_gameState.chars[charIndex].magic[slot].magicId = value;
}

/**
 * @brief Set the quantity of a magic spell in a character's slot.
 *
 * @param charIdx Character index (0-7).
 * @param slotIdx Magic slot index (0-31).
 * @param value Quantity to store.
 */
void func_801E67E0(s32 charIdx, s32 slotIdx, u8 value) {
    g_gameState.chars[charIdx].magic[slotIdx].quantity = value;
}

/**
 * @brief Render magic entry with adjusted width based on character attribute.
 *
 * Computes width as 0xCA minus half of the byte at a0+0x5B, and Y position
 * as a2 * 13 + 0x78. Calls func_801F0A34 to render.
 *
 * @param a0 Pointer to character data (byte at 0x5B used for width adjustment).
 * @param a1 X position parameter.
 * @param a2 Row index (multiplied by 13 and offset by 0x78 for Y position).
 */
/**
 * @brief Render magic entry with adjusted width based on character attribute.
 *
 * Computes width as 0xCA minus half of the byte at a0+0x5B, and Y position
 * as a2 * 13 + 0x78. Calls func_801F0A34 to render.
 *
 * @param a0 Pointer to character data (byte at 0x5B used for width adjustment).
 * @param a1 X position parameter.
 * @param a2 Row index (multiplied by 13 and offset by 0x78 for Y position).
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6810);

/**
 * @brief Render magic entry with adjusted width based on character attribute.
 *
 * Computes width as 0xCA minus half of the byte at a0+0x5B, and Y position
 * as a2 * 13 + 0x71. Calls func_801F0A34 to render.
 *
 * @param a0 Pointer to character data (byte at 0x5B used for width adjustment).
 * @param a1 X position parameter.
 * @param a2 Row index (multiplied by 13 and offset by 0x71 for Y position).
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6858);

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x43 for Y)
 */
void func_801E68A0(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x43);
}

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x8D for Y)
 */
void func_801E68F0(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x8D);
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6940);

/**
 * @brief Display a stat value from a lookup table with extended rendering.
 *
 * Copies 8 halfwords from D_801EC814 into a local buffer, loads the value
 * at a0+0x50, indexes the table by a2, adds 0x32, and renders via
 * func_801F0A78 with the stat value as width parameter and 0xD as height.
 *
 * @param a0 Character context (offset 0x50 used for stat value).
 * @param a1 Render context pointer.
 * @param a2 Index into the stat table (0-7).
 */
void func_801E69EC(u8 *a0, s32 a1, s32 a2) {
    extern u8 D_801EC814[];
    s16 buf[32];
    s32 stat;

    stat = *(s16 *)(a0 + 0x50);
    func_801F5984(D_801EC814, buf, 8);
    a2 *= 2;
    func_801F0A78(a1, 0, stat, buf[a2 / 2] + 0x32, 0xD);
}

/**
 * @brief Render magic entry at computed Y position with width 0x26.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E6A64(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x26, a1 * 13 + 0x42);
}

/** @brief Call func_801F0A34 with a0, zero, 0x28, 0x39. */
void func_801E6A9C(s32 a0) {
    func_801F0A34(a0, 0, 0x28, 0x39);
}

/** @brief Call func_801F0A34 with a0, zero, 0x28, 0x94. */
void func_801E6AC4(s32 a0) {
    func_801F0A34(a0, 0, 0x28, 0x94);
}

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x9A for Y)
 */
void func_801E6AEC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x9A);
}

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x43 for Y)
 */
void func_801E6B3C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x43);
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6B8C);

/**
 * @brief Toggle a magic junction slot for a character using slot/8 division.
 *
 * Calls func_801F58EC to get the current selection, func_801F57A4 to get
 * flags. If the low bit of the flags is set, divides the selection by 8,
 * recalculates slot via func_801F5150, stores via func_801F576C, updates
 * position via func_801F5868, and returns 1. Otherwise returns 0.
 *
 * @param a0 Menu context pointer.
 * @return 1 if the junction was toggled, 0 otherwise.
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6CCC);

/**
 * @brief Toggle a magic junction slot for a character if bit 0 is set.
 *
 * Calls func_801F58EC to get the current slot index, then func_801F57A4
 * to get flags. If the low bit of the flags is set, recalculates the
 * slot via func_801F5150, stores it via func_801F576C, sets the position
 * via func_801F5868, and returns 1. Otherwise returns 0.
 *
 * @param a0 Menu context pointer.
 * @return 1 if the junction was toggled, 0 otherwise.
 */
s32 func_801E6D58(s32 a0) {
    s32 slot;
    s32 flags;

    slot = func_801F58EC(a0);
    flags = func_801F57A4(a0);
    if ((flags & 1) == 0) {
        return 0;
    }
    flags = func_801F5150(slot, slot, flags);
    func_801F576C(a0, flags);
    func_801F5868(a0, slot);
    return 1;
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6DD0);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6F54);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6FDC);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EAF50);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB0F4);

/**
 * @brief Render a list item with name lookup and positioned text.
 *
 * Loads the item pointer from g_menuDisplayCfg+0x20 at offset a2*4, decodes
 * a name string via decodeMessage and renders it via func_801F0FEC.
 * Position is computed from g_menuDisplayCfg fields + the 5th stack arg + 0xA.
 *
 * @param a0 Render context pointer.
 * @param a1 Current display state value.
 * @param a2 Item index in the list.
 * @param arg4 Additional Y offset from caller (5th stack arg, at sp+0xC0).
 * @return Updated display state from func_801F0FEC, or a1 if item is null.
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB1A0);

/**
 * @brief Register a list-style menu display callback with header config.
 *
 * Configures the g_menuDisplayCfg menu display config struct with icon type 0x55,
 * dimensions (0x144 x 0x1A), scroll enabled, page mode, and registers
 * func_801EB1A0 as the display callback via func_801EFBB4.
 *
 * @param a0 Context pointer (offset 0x24 used for item list, 0x34/0x44 for counts).
 * @param a1 First parameter for func_801EFBB4.
 * @param a2 Second parameter for func_801EFBB4.
 * @param a3 Y position for menu display.
 * @param arg4 Y2 position for menu display.
 */
void func_801EB250(u8 *a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    extern u8 g_menuDisplayCfg[];
    extern void func_801EB1A0();
    u8 *cfg = g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0x55;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)(cfg + 0x00) = a3;
    *(s16 *)(cfg + 0x04) = 0x144;
    *(s16 *)(cfg + 0x06) = 0x1A;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x02) = arg4;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x34) + *(u16 *)(a0 + 0x44);
    *(s32 *)(cfg + 0x20) = (s32)(a0 + 0x24);
    func_801EFBB4(a1, a2, (s32)func_801EB1A0);
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB2D4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB454);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB6A4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB80C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBA68);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBBE4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBD40);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBF08);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EC43C);
