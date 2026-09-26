#include "btl_color.h"
#include "character.h"
#include "common.h"
#include "game.h"
#include "gamestate.h"
#include "gf.h"
#include "menu.h"
#include "menumgc.h"
#include "numstr.h"
#include "psxsdk/libetc.h"

/*
 * menumain.h must not be included here: its `u16 func_801F22F4(void)` and,
 * through card.h, `u16 func_80036EC0(void)` truncate this unit's calls to both
 * (measured: 4 bytes of menumgc.ovl). Its other helpers get the file-scope
 * prototypes below instead.
 *
 * Three of them deliberately have none, because declaring them the way they are
 * defined changes this unit's code: func_801F22F4 and func_80036EC0 for the u16
 * returns above, and func_801F5868 for its `s16` second parameter, which makes
 * the call truncate (measured). Leave those three implicit -- `-Wall` reports
 * them, and that is the cheaper half of the trade.
 */

extern s32 getMagicNamePtr(s32 a0);
extern u8 *getCharName(s32 a0);
extern void copyString(s32 a0, s32 a1);

/** @brief Magic availability entry: one 4-byte record per spell. */
typedef struct {
    u8 flags;
    u8 pad[3];
} FlagEntry;

extern FlagEntry D_801F87B8[];
extern GfData g_gfData;
extern s32 func_801EFBB4(s32 a0, s32 a1, s32 a2);
extern u8 *func_801F08D4(s32 a0, s32 a1, s32 a2, s32 a3);
extern void func_801F0A34(s32 a0, s32 a1, s32 a2, s32 a3);
extern void func_801F0A78(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
extern s32 func_801F0BB0(void);
extern void func_801F1B4C(s32 a0);
extern s32 func_801F5150(s32 a0, s32 a1, s32 a2);
extern void func_801F537C(s32 charIdx, BattleCharData *dst);
extern void func_801F5400(s32 a0);
extern s32 func_801F565C(s32 charIdx, s32 mask);
extern s32 func_801F56E4(s32 charIdx, s32 mask);
extern void func_801F576C(s32 a0, s32 a1);
extern s32 func_801F57A4(s32 a0);
extern s32 func_801F58EC(s32 a0);
extern s32 func_801F6768(u16 flags, s32 max, s32 current);
extern void func_801F6F88(s32 a0);
extern s32 func_801F76E0(s32 flags, s32 a1, s32 a2);
extern void func_801F7B10(s32 a0);
extern void func_801F7BEC(s32 a0);

extern s32 menumain_getPartyMemberMask(void);
extern s32 popcount(s32 a0);
extern s32 findNthSetBit(s32 a0, s32 a1);

/* --- Prototypes for this unit's own functions --- */
s32 func_801E5D64(s32 charA, s32 charB, s32 slotA, s32 slotB);
s32 func_801E5E74(s32 fromChar, s32 toChar);
s32 func_801E5F5C(s32 charIdx, s32 spellId);
s32 func_801E6058(s32 charIdx, s32 spellId);
s32 func_801E626C(s32 charA, s32 charB, s32 slotA, s32 slotB);
s32 func_801E64FC(s32 charIdx, s32 order);
void func_801E66C0(MagicMenuCtx *ctx);
void func_801E6810(MagicMenuCtx *ctx, s32 a1, s32 row);
void func_801E6858(MagicMenuCtx *ctx, s32 a1, s32 row);
void func_801E6940(MagicMenuCtx *ctx, s32 a1, s32 row);
s32 func_801E6DD0(s32 charIdx, s32 spellId, s32 targetMask);
u8 *func_801E6F54(s32 charIdx, s32 slot, s32 page);

/**
 * @brief Format a string with escape sequence substitution.
 *
 * Copies bytes from @p src to @p dst, expanding two escape sequences:
 * - `0x0A 0x26`: substitutes a numeric value derived from @p spellId via
 *   getMagicNamePtr, converted to a string by copyString.
 * - `0x0A 0x27`: substitutes the name of the character identified by
 *   g_gameState.chars[@p charIdx].characterId, looked up via getCharName
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
                u8 *val = getCharName(g_gameState.chars[charIdx].characterId);
                copyString((s32)local_buf, (s32)val);
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

    src = &g_characterMagic[arg0 * sizeof(CharacterData)];
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

    dst = &g_characterAbilities[arg0 * sizeof(CharacterData)];
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
    u8 *ptr = &g_characterMagic[charIdx * sizeof(CharacterData)];
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
 * in g_characterAbilities for the given spell ID.
 *
 * @param charIdx Character index (0-7).
 * @param spellId Spell ID to search for (returns -1 if 0).
 * @return Junction slot index (0-18) if found, -1 if not found or spellId is 0.
 */
s32 func_801E5C50(s32 charIdx, s32 spellId) {
    u8 *ptr = &g_characterAbilities[charIdx * sizeof(CharacterData)];
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
 * Searches through 32 magic slots in g_characterMagic for the given spell ID.
 * Returns the quantity if found, 0 if not found or spellId is 0.
 *
 * @param charIdx Character index (0-7).
 * @param spellId Spell ID to search for.
 * @return Quantity of the spell, or 0 if not found.
 */
s32 func_801E5CAC(s32 charIdx, s32 spellId) {
    u8 *ptr = &g_characterMagic[charIdx * sizeof(CharacterData)];
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
s32 func_801E677C(s32 charIdx, s32 slotIdx) {
    return g_gameState.chars[charIdx].magic[slotIdx].quantity;
}

/**
 * @brief Set a character's magic slot spell ID.
 *
 * @param charIndex Character index (0–7, see CharacterId).
 * @param slot      Magic slot index (0–31).
 * @param value     Magic spell ID to store (see MAGIC_* defines).
 */
void setCharacterMagicSlotId(s32 charIndex, s32 slot, s32 value) {
    g_gameState.chars[charIndex].magic[slot].magicId = value;
}

/**
 * @brief Set the quantity of a magic spell in a character's slot.
 *
 * @param charIdx Character index (0-7).
 * @param slotIdx Magic slot index (0-31).
 * @param value Quantity to store.
 */
void func_801E67E0(s32 charIdx, s32 slotIdx, s32 value) {
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
 * @brief Draw the command-row cursor over command @p a2.
 *
 * Reads the command label x positions out of D_801EC814 and renders the cursor
 * at that x + 0x32, y 0xD, in the menu's current brightness.
 *
 * @param ctx Magic menu context (@c fadeScale is the brightness).
 * @param a1  0 parks the cursor (a sub-screen owns it), 1 draws it active.
 * @param a2  Command index (0-3).
 */
void func_801E69EC(MagicMenuCtx *ctx, s32 a1, s32 a2) {
    s16 buf[32];
    s32 stat;

    stat = ctx->fadeScale;
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

/**
 * @brief Main magic menu per-frame update and state machine.
 *
 * Called every frame while the magic menu is active. Reads the current state
 * from ctx->state (offset 0x10) and dispatches through a jump table spanning
 * 0x00-0x71 (0x21-0x27 have no case). Many states only hand over to another
 * one, so a single frame can run a chain of them; the paired draw callback
 * func_801EBF08 picks its layout from ctx->drawMode.
 *
 * It drives every screen under Magic for one character: the top command row,
 * that character's 32 spell slots in eight pages of four, casting a menu spell
 * on a party member, exchanging or pooling one spell with a second character,
 * handing a whole stock over, the sort popup and the manual rearrange screen,
 * plus the prompts they share. Panel and page movements are fixed-point
 * counters the draw callback eases. L1/R1 switch the character on nearly every
 * screen through the shared slide states, which resume ctx->returnState.
 *
 * State groups:
 *   0x00-0x03  open the screen, then the command row
 *   0x04-0x0B  character-switch slides (0x04-0x07 over the castable party,
 *              0x08-0x0B over the junctionable party)
 *   0x0C-0x16  spell list, page flips, target select and the cast
 *   0x17-0x40  exchange one spell: both lists, the prompts, the quantity editor
 *   0x41-0x44  back to the command row; the timed refusal message
 *   0x45-0x5C  sort popup and the manual rearrange screen
 *   0x5D-0x69  hand a character's whole stock to another character
 *   0x6A-0x6F  the shared yes/no box and the discard prompts
 *   0x70-0x71  close the screen and pop the menu stack
 *
 * @param ctx Magic menu context (MagicMenuCtx *).
 */
void magicMenuUpdate(MagicMenuCtx *ctx) {
    u16 *statePtr;
    s32 slot;
    u16 state;
    u16 inputNew;
    u16 inputRepeat;
    u32 stateId;

    statePtr = &ctx->state;
    inputRepeat = g_menuDisplayCfg.inputRepeat;
    inputNew = g_menuDisplayCfg.inputNew;
    state = ctx->state;
dispatch:
    stateId = state & 0xFFFF;
    switch (stateId) {

    /* 0x00-0x03: fade the screen in, then the top command row */
    case 0x0:
        ctx->itemPtr = 0;
        *statePtr = 1;
        break;
    case 0x1:
        func_801E668C(ctx->charIdx);
        ctx->fadeScale += 0x100;
        func_801E69EC(ctx, 1, ctx->commandIdx);
        if (ctx->fadeScale >= 0x1000) {
            ctx->fadeScale = 0x1000;
            *statePtr = 2;
        }
        break;
    case 0x2:
        ctx->drawMode = 0;
        func_801E69EC(ctx, 1, ctx->commandIdx);
        *statePtr = 3;
        break;
    case 0x3:
        ctx->itemPtr = func_801F08D4(1, 8, D_801EC814[ctx->commandIdx], 1);
        ctx->commandIdx = func_801F76E0(inputRepeat, ctx->commandMask, ctx->commandIdx);
        if (popcount(func_80036EC0()) >= 2) {
            ctx->drawFlags &= ~MAGIC_DRAW_SUB_CURSOR;
            if (inputRepeat & PADL1) {
                ctx->refreshHelpText = 1;
                ctx->headerCursorActive = 0;
                ctx->returnState = 3;
                state = 4;
                goto dispatch;
            }
            if (inputRepeat & PADR1) {
                ctx->refreshHelpText = 1;
                ctx->headerCursorActive = 0;
                ctx->returnState = 3;
                state = 6;
                goto dispatch;
            }
        }
        if (inputRepeat & PADRdown) {
            sendSpuCommand(2);
            switch (ctx->commandIdx) {
            case 0:
                *statePtr = 0xC;
                break;
            case 1:
                *statePtr = 0x17;
                break;
            case 2:
                *statePtr = 0x5D;
                break;
            case 3:
                *statePtr = 0x45;
                break;
            }
            break;
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x70;
        }
        func_801E69EC(ctx, 1, ctx->commandIdx);
        break;

    /* 0x04-0x07: character switch over the castable party (func_80036EC0) and its slide;
     * entered from the command row, the exchange list, the rearrange screen and the transfer
     * screen */
    case 0x4: {
        s32 fromList;

        sendSpuCommand(1);
        fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;
        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->prevCharIdx = ctx->charIdx;
        ctx->charIdx = func_801F565C(ctx->charIdx, func_80036EC0());
        D_801ECD90 = D_801EC9F0;
        func_801F537C(ctx->charIdx, &D_801EC9F0);
        ctx->slideOffset = -0xF80;
        func_801E66C0(ctx);
        if (ctx->refreshHelpText != 0) {
            ctx->itemPtr = func_801F08D4(1, 8, D_801EC814[ctx->commandIdx], 1);
        }
        ctx->upperPage = ctx->slotCursor[ctx->charIdx] / 4;
        if (ctx->headerCursorActive != 0) {
            func_801E6A9C(1);
        }
        *statePtr = 5;
        break;
    }
    case 0x5: {
        s32 fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;

        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->slideOffset += 0x80;
        if (ctx->slideOffset >= 0) {
            ctx->slideOffset = 0;
            *statePtr = ctx->returnState;
        }
        if (ctx->headerCursorActive != 0) {
            func_801E6A9C(1);
            if (inputNew & PADLleft) {
                *statePtr = 4;
            }
            if (inputNew & PADLright) {
                *statePtr = 6;
            }
        }
        if (inputNew & PADL1) {
            *statePtr = 4;
        }
        if (inputNew & PADR1) {
            *statePtr = 6;
        }
        break;
    }
    case 0x6: {
        s32 fromList;

        sendSpuCommand(1);
        fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;
        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->prevCharIdx = ctx->charIdx;
        ctx->charIdx = func_801F56E4(ctx->charIdx, func_80036EC0());
        ctx->upperPage = ctx->slotCursor[ctx->charIdx] / 4;
        D_801ECD90 = D_801EC9F0;
        func_801F537C(ctx->charIdx, &D_801EC9F0);
        ctx->slideOffset = 0xF80;
        func_801E66C0(ctx);
        if (ctx->refreshHelpText != 0) {
            ctx->itemPtr = func_801F08D4(1, 8, D_801EC814[ctx->commandIdx], 1);
        }
        if (ctx->headerCursorActive != 0) {
            func_801E6A9C(1);
        }
        *statePtr = 7;
        break;
    }
    case 0x7: {
        s32 fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;

        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->slideOffset -= 0x80;
        if (ctx->slideOffset <= 0) {
            ctx->slideOffset = 0;
            *statePtr = ctx->returnState;
        }
        if (ctx->headerCursorActive != 0) {
            func_801E6A9C(1);
            if (inputNew & PADLleft) {
                *statePtr = 4;
            }
            if (inputNew & PADLright) {
                *statePtr = 6;
            }
        }
        if (inputNew & PADL1) {
            *statePtr = 4;
        }
        if (inputNew & PADR1) {
            *statePtr = 6;
        }
        break;
    }

    /* 0x08-0x0B: character switch over the junctionable party (_getJunctionableCharMask) and
     * its slide; entered from the spell list */
    case 0x8: {
        s32 fromList;

        sendSpuCommand(1);
        fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;
        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->prevCharIdx = ctx->charIdx;
        ctx->charIdx = func_801F565C(ctx->charIdx, _getJunctionableCharMask());
        D_801ECD90 = D_801EC9F0;
        func_801F537C(ctx->charIdx, &D_801EC9F0);
        ctx->slideOffset = -0xF80;
        func_801E66C0(ctx);
        ctx->upperPage = ctx->slotCursor[ctx->charIdx] / 4;
        *statePtr = 9;
        break;
    }
    case 0x9: {
        s32 fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;

        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->slideOffset += 0x80;
        if (ctx->slideOffset >= 0) {
            ctx->slideOffset = 0;
            *statePtr = ctx->returnState;
        }
        if (inputNew & PADL1) {
            *statePtr = 8;
        }
        if (inputNew & PADR1) {
            *statePtr = 0xA;
        }
        break;
    }
    case 0xA: {
        s32 fromList;

        sendSpuCommand(1);
        fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;
        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->prevCharIdx = ctx->charIdx;
        ctx->charIdx = func_801F56E4(ctx->charIdx, _getJunctionableCharMask());
        D_801ECD90 = D_801EC9F0;
        func_801F537C(ctx->charIdx, &D_801EC9F0);
        ctx->slideOffset = 0xF80;
        func_801E66C0(ctx);
        ctx->upperPage = ctx->slotCursor[ctx->charIdx] / 4;
        *statePtr = 0xB;
        break;
    }
    case 0xB: {
        s32 fromList;

        /* tested through a local: the inline test compiles to srl/xori, not andi/sltiu */
        fromList = ctx->drawFlags & MAGIC_DRAW_SUB_CURSOR;
        func_801E69EC(ctx, !fromList, ctx->commandIdx);
        ctx->slideOffset -= 0x80;
        if (ctx->slideOffset <= 0) {
            ctx->slideOffset = 0;
            *statePtr = ctx->returnState;
        }
        if (inputNew & PADL1) {
            *statePtr = 8;
        }
        if (inputNew & PADR1) {
            *statePtr = 0xA;
        }
        break;
    }

    /* 0x0C-0x0D: the character's 32 spell slots, four rows per page */
    case 0xC:
        ctx->drawMode = 1;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, ctx->slotCursor[ctx->charIdx]);
        *statePtr = 0xD;
        break;
    case 0xD: {
        s32 row;

        slot = ctx->slotCursor[ctx->charIdx];
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, slot);
        /* own statement: inlined into the call, the pad word gets an extra andi 0xffff */
        row = slot % 4;
        slot = ctx->upperPage * 4 + func_801F6768(inputRepeat, 4, row);
        ctx->slotCursor[ctx->charIdx] = slot;
        if (inputRepeat & PADLleft) {
            ctx->pageReturnState = 0xD;
            state = 0xE;
            goto dispatch;
        }
        if (inputRepeat & PADLright) {
            ctx->pageReturnState = 0xD;
            state = 0x10;
            goto dispatch;
        }
        if (popcount(_getJunctionableCharMask()) >= 2) {
            ctx->drawFlags |= MAGIC_DRAW_SUB_CURSOR;
            if (inputRepeat & PADL1) {
                ctx->returnState = 0xD;
                state = 8;
                goto dispatch;
            }
            if (inputRepeat & PADR1) {
                ctx->returnState = 0xD;
                state = 0xA;
                goto dispatch;
            }
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, slot);
        ctx->itemPtr = func_801E6F54(ctx->charIdx, ctx->slotCursor[ctx->charIdx], ctx->upperPage);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 2;
        }
        if (inputRepeat & PADRdown) {
            s32 spellId;
            s32 qty;

            /* both slot bytes are read before the first test, here and below */
            spellId = g_gameState.chars[ctx->charIdx].magic[slot].magicId;
            qty = g_gameState.chars[ctx->charIdx].magic[slot].quantity;
            if (qty != 0 && func_801E599C(spellId)) {
                sendSpuCommand(2);
                *statePtr = 0x12;
            } else {
                sendSpuCommand(5);
            }
        }
        if (inputNew & PADRleft) {
            s32 spellId;
            s32 qty;

            spellId = g_gameState.chars[ctx->charIdx].magic[slot].magicId;
            qty = g_gameState.chars[ctx->charIdx].magic[slot].quantity;
            if (spellId != 0 && qty != 0) {
                sendSpuCommand(2);
                ctx->discardReturnState = 0xD;
                ctx->discardSlot = slot;
                ctx->discardCharIdx = ctx->charIdx;
                state = 0x6C;
                goto dispatch;
            }
            sendSpuCommand(5);
        }
        break;
    }

    /* 0x0E-0x11: upper list page flip, previous page (0x0E/0x0F) and next page (0x10/0x11) */
    case 0xE:
        sendSpuCommand(1);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, ctx->slotCursor[ctx->charIdx]);
        ctx->upperPrevPage = ctx->upperPage;
        ctx->upperPage--;
        if (ctx->upperPage < 0) {
            ctx->upperPage = 7;
        }
        ctx->itemPtr2 = ctx->itemPtr;
        ctx->itemPtr = func_801E6F54(ctx->charIdx, ctx->slotCursor[ctx->charIdx], ctx->upperPage);
        ctx->upperPageSlide = -0xE67;
        *statePtr = 0xF;
        break;
    case 0xF:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, ctx->slotCursor[ctx->charIdx]);
        ctx->upperPageSlide += 0x199;
        if (ctx->upperPageSlide >= 0) {
            ctx->upperPageSlide = 0;
            *statePtr = ctx->pageReturnState;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0xE;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x10;
            return;
        }
        break;
    case 0x10:
        sendSpuCommand(1);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, ctx->slotCursor[ctx->charIdx]);
        ctx->upperPrevPage = ctx->upperPage;
        ctx->upperPage++;
        if (ctx->upperPage >= 8) {
            ctx->upperPage = 0;
        }
        ctx->itemPtr2 = ctx->itemPtr;
        ctx->itemPtr = func_801E6F54(ctx->charIdx, ctx->slotCursor[ctx->charIdx], ctx->upperPage);
        ctx->upperPageSlide = 0xE67;
        *statePtr = 0x11;
        return;
    case 0x11:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, ctx->slotCursor[ctx->charIdx]);
        ctx->upperPageSlide -= 0x199;
        if (ctx->upperPageSlide <= 0) {
            ctx->upperPageSlide = 0;
            *statePtr = ctx->pageReturnState;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0xE;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x10;
            return;
        }
        break;

    /* 0x12-0x16: open the target window, pick a party member, cast the spell */
    case 0x12:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        ctx->drawMode = 2;
        ctx->targetWindowSlide = 0;
        ctx->upperPage = ctx->slotCursor[ctx->charIdx] / 4;
        *statePtr = 0x13;
        return;
    case 0x13:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        ctx->targetWindowSlide -= 0x100;
        if (ctx->targetWindowSlide <= 0) {
            ctx->targetWindowSlide = 0;
            *statePtr = 0x14;
            return;
        }
        break;
    case 0x14:
        if (D_8005F170 && (inputNew & PADj)) {
            s32 i;
            s32 j;

            for (i = 0; i < 8; i++) {
                for (j = 0; j < 32; j++) {
                    g_gameState.chars[i].magic[j].quantity = 1;
                }
            }
        }
        ctx->targetCursor = func_801F6768(inputRepeat, ctx->targetCount, ctx->targetCursor);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6A64(1, ctx->targetCursor);
        if (inputRepeat & PADRdown) {
            state = 0x16;
            goto dispatch;
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x15;
            return;
        }
        break;
    case 0x15:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        ctx->targetWindowSlide = 0x1000;
        *statePtr = 0xC;
        return;
    case 0x16:
    {
        s32 target;

        target = findNthSetBit(ctx->targetMask, ctx->targetCursor);
        if (!func_801E6DD0(ctx->charIdx, g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId, (1 << target) & 0xFFFF)) {
            break;
        }
        g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity--;
        if (g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity == 0) {
            g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId = 0;
        }
        /* the original tests the quantity twice; merging the two ifs costs 16 rows */
        if (g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity == 0) {
            ctx->targetWindowSlide = 0x1000;
            func_801F1B4C(ctx->charIdx);
            *statePtr = 0xC;
        } else {
            *statePtr = 0x14;
        }
        func_801F5400(ctx->charIdx);
        return;
    }

    /* 0x17-0x1A: exchange - pick the spell in this character's list */
    case 0x17:
    {
        s32 slot;

        slot = ctx->slotCursor[ctx->charIdx];    /* int local: the divide is signed */
        ctx->lowerPage = slot / 4;
        ctx->drawMode = 3;
        ctx->partnerCharIdx = func_801F56E4(ctx->charIdx, func_80036EC0() & ~(1 << ctx->charIdx));
        func_801F537C(ctx->partnerCharIdx, &D_801ECBC0);
        *statePtr = 0x18;
    }
        /* fallthrough */
    case 0x18:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, ctx->slotCursor[ctx->charIdx]);
        *statePtr = 0x19;
        return;
    case 0x19:
        *statePtr = 0x1A;
        /* fallthrough */
    case 0x1A:
    {
        s32 column;

        slot = ctx->slotCursor[ctx->charIdx];
        column = slot % 4;    /* computed before the call's other arguments */
        slot = ctx->upperPage * 4 + func_801F6768(inputRepeat, 4, column);
        ctx->slotCursor[ctx->charIdx] = slot;
        if (inputRepeat & PADLleft) {
            ctx->pageReturnState = 0x1A;
            state = 0xE;
            goto dispatch;
        }
        if (inputRepeat & PADLright) {
            ctx->pageReturnState = 0x1A;
            state = 0x10;
            goto dispatch;
        }
        if (popcount(func_80036EC0()) >= 2) {
            ctx->drawFlags |= MAGIC_DRAW_SUB_CURSOR;
            if (inputRepeat & PADL1) {
                ctx->drawMode = 3;
                ctx->headerCursorActive = 0;
                ctx->refreshHelpText = 0;
                ctx->returnState = 0x1A;
                state = 4;
                goto dispatch;
            }
            if (inputRepeat & PADR1) {
                ctx->drawMode = 3;
                ctx->headerCursorActive = 0;
                ctx->refreshHelpText = 0;
                ctx->returnState = 0x1A;
                state = 6;
                goto dispatch;
            }
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(1, slot);
        ctx->itemPtr = func_801E6F54(ctx->charIdx, ctx->slotCursor[ctx->charIdx], ctx->upperPage);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x41;
        }
        if (inputRepeat & PADRdown) {
            sendSpuCommand(2);
            if (ctx->charIdx == ctx->partnerCharIdx) {
                ctx->partnerCharIdx = func_801F56E4(ctx->charIdx, func_80036EC0() & ~(1 << ctx->charIdx));
            }
            *statePtr = 0x1B;
        }
        if (inputNew & PADRleft) {
            s32 spellId;
            s32 qty;

            spellId = g_gameState.chars[ctx->charIdx].magic[slot].magicId;
            qty = g_gameState.chars[ctx->charIdx].magic[slot].quantity;
            if (spellId != 0 && qty != 0) {
                sendSpuCommand(2);
                ctx->discardReturnState = 0x1A;
                ctx->discardSlot = slot;
                ctx->discardCharIdx = ctx->charIdx;
                state = 0x6C;
                goto dispatch;
            }
            sendSpuCommand(5);
            return;
        }
        break;
    }

    /* 0x1B-0x20: exchange - pick the other character, with the partner panel slides */
    case 0x1B:
        ctx->drawFlags |= MAGIC_DRAW_LR_ARROWS;
        ctx->itemPtr = func_801F08D4(1, 8, 3, 0);
        ctx->drawMode = 4;
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AC4(1);
        *statePtr = 0x1C;
        return;
    case 0x1C:
        ctx->itemPtr = func_801F08D4(1, 8, 3, 0);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AC4(1);
        if (popcount(func_80036EC0() & ~(1 << ctx->charIdx)) >= 2) {
            ctx->drawFlags |= MAGIC_DRAW_SUB_CURSOR;
            if (inputRepeat & (PADL1 | PADLleft)) {
                *statePtr = 0x1D;
                return;
            }
            if (inputRepeat & (PADR1 | PADLright)) {
                *statePtr = 0x1F;
                return;
            }
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            ctx->drawFlags &= ~MAGIC_DRAW_LR_ARROWS;
            *statePtr = 0x28;
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawFlags &= ~MAGIC_DRAW_LR_ARROWS;
            *statePtr = 0x1A;
            return;
        }
        break;
    case 0x1D: {
        s32 otherChars;

        D_801ECD90 = D_801ECBC0;
        sendSpuCommand(1);
        otherChars = func_80036EC0() & ~(1 << ctx->charIdx);
        ctx->prevCharIdx = ctx->partnerCharIdx;
        ctx->partnerCharIdx = func_801F565C(ctx->partnerCharIdx, otherChars);
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        ctx->partnerSlideOffset = -0xF80;
        *statePtr = 0x1E;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AC4(1);
        return;
    }
    case 0x1E:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AC4(1);
        ctx->partnerSlideOffset += 0x80;
        if (ctx->partnerSlideOffset >= 0) {
            ctx->partnerSlideOffset = 0;
            *statePtr = 0x1C;
        }
        if (inputNew & (PADL1 | PADLleft)) {
            *statePtr = 0x1D;
        }
        if (inputNew & (PADR1 | PADLright)) {
            *statePtr = 0x1F;
            return;
        }
        break;
    case 0x1F: {
        s32 otherChars;

        sendSpuCommand(1);
        D_801ECD90 = D_801ECBC0;
        otherChars = func_80036EC0() & ~(1 << ctx->charIdx);
        ctx->prevCharIdx = ctx->partnerCharIdx;
        ctx->partnerCharIdx = func_801F56E4(ctx->partnerCharIdx, otherChars);
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        ctx->partnerSlideOffset = 0xF80;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AC4(1);
        *statePtr = 0x20;
        return;
    }
    case 0x20:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AC4(1);
        ctx->partnerSlideOffset -= 0x80;
        if (ctx->partnerSlideOffset <= 0) {
            ctx->partnerSlideOffset = 0;
            *statePtr = 0x1C;
        }
        if (inputNew & (PADL1 | PADLleft)) {
            *statePtr = 0x1D;
        }
        if (inputNew & (PADR1 | PADLright)) {
            *statePtr = 0x1F;
        }
        break;

    /* 0x28-0x2C: the other character's list; 0x28 jumps straight to the prompt when the partner
     * already stocks the spell */
    case 0x28: {
        s32 spellId;
        s32 qty;

        func_801F1B4C(ctx->charIdx);
        func_801F1B4C(ctx->partnerCharIdx);
        spellId = g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId;
        qty = func_801E5CAC(ctx->partnerCharIdx, spellId);
        state = 0x29;
        if (spellId != 0 && qty != 0) {
            ctx->exchangeSpellId = spellId;
            ctx->slotCursor[ctx->partnerCharIdx] = func_801E5C00(ctx->partnerCharIdx, ctx->exchangeSpellId);
            ctx->drawFlags |= MAGIC_DRAW_NO_LOWER_CUR;
            ctx->upperPage = ctx->slotCursor[ctx->charIdx] >> 2;
            ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] >> 2;
            ctx->exchangeMode = MAGIC_EXCHANGE_THREE_CHOICE | MAGIC_EXCHANGE_TO_PARTNER;
            ctx->promptCancelState = 0x1C;
            state = 0x35;
        }
        goto dispatch;
    }
    case 0x29:
        *statePtr = 0x2A;
        /* fallthrough */
    case 0x2A:
        *statePtr = 0x2B;
        /* fallthrough */
    case 0x2B:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(1, ctx->slotCursor[ctx->partnerCharIdx]);
        *statePtr = 0x2C;
        return;
    case 0x2C: {
        s32 slot;
        s32 rowBase;

        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(1, ctx->slotCursor[ctx->partnerCharIdx]);
        slot = ctx->slotCursor[ctx->partnerCharIdx];
        rowBase = slot / 4 * 4;
        slot = rowBase + func_801F6768(inputRepeat, 4, slot % 4);
        ctx->slotCursor[ctx->partnerCharIdx] = slot;
        ctx->itemPtr = func_801E6F54(ctx->partnerCharIdx, ctx->slotCursor[ctx->partnerCharIdx], ctx->lowerPage);
        if (inputRepeat & PADLleft) {
            *statePtr = 0x2D;
            return;
        }
        if (inputRepeat & PADLright) {
            *statePtr = 0x2F;
            return;
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x1B;
            return;
        }
        if (inputNew & PADRdown) {
            s32 spellA;
            s32 spellB;

            spellA = g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId;
            spellB = g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].magicId;
            if (spellA == spellB) {
                if (spellB == 0) {
                    sendSpuCommand(5);
                } else {
                    ctx->exchangeSpellId = spellA + spellB;
                    ctx->promptCancelState = 0x34;
                    ctx->exchangeMode = 0;
                    *statePtr = 0x35;
                    return;
                }
            } else if (spellA == 0 || spellB == 0) {
                s32 merge = 0;
                s32 qtyAinB;
                s32 qtyBinA;

                sendSpuCommand(2);
                ctx->exchangeSpellId = spellA + spellB;
                qtyAinB = func_801E5CAC(ctx->partnerCharIdx, spellA);
                qtyBinA = func_801E5CAC(ctx->charIdx, spellB);
                ctx->promptCancelState = 0x2B;
                if (spellA != 0 && qtyAinB != 0) {
                    ctx->exchangeSpellId = spellA;
                    ctx->slotCursor[ctx->partnerCharIdx] = func_801E5C00(ctx->partnerCharIdx, ctx->exchangeSpellId);
                    ctx->promptCancelState = 0x1B;
                    merge = 1;
                }
                if (spellB != 0 && qtyBinA != 0) {
                    ctx->exchangeSpellId = spellB;
                    ctx->slotCursor[ctx->charIdx] = func_801E5C00(ctx->charIdx, ctx->exchangeSpellId);
                    ctx->promptCancelState = 0x1B;
                    merge = 1;
                }
                ctx->upperPage = ctx->slotCursor[ctx->charIdx] >> 2;
                ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] >> 2;
                ctx->exchangeMode = 0;
                if (merge) {
                    *statePtr = 0x35;
                } else {
                    *statePtr = 0x32;
                }
                return;
            } else {
                func_801E5A28(ctx->charIdx, 0);
                func_801E5A28(ctx->partnerCharIdx, 1);
                if (func_801E5D64(ctx->charIdx, ctx->partnerCharIdx,
                                  ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
                    ctx->messageReturnState = 0x2C;
                    *statePtr = 0x43;
                    return;
                }
                if (func_801E626C(ctx->charIdx, ctx->partnerCharIdx,
                                  ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) != 0) {
                    func_801E5B00(ctx->charIdx, 0);
                    func_801E5B00(ctx->partnerCharIdx, 1);
                    ctx->returnState = 0x31;
                    ctx->exitState = 0x2B;
                    *statePtr = 0x6A;
                } else {
                    sendSpuCommand(2);
                    *statePtr = 0x19;
                }
            }
        }
        if (inputNew & PADRleft) {
            s32 spellId;
            s32 qty;

            spellId = g_gameState.chars[ctx->partnerCharIdx].magic[slot].magicId;
            qty = g_gameState.chars[ctx->partnerCharIdx].magic[slot].quantity;
            if (spellId != 0 && qty != 0) {
                sendSpuCommand(2);
                ctx->discardReturnState = 0x2C;
                ctx->discardCharIdx = ctx->partnerCharIdx;
                ctx->discardSlot = slot;
                state = 0x6C;
                goto dispatch;
            }
            sendSpuCommand(5);
        }
        break;
    }

    /* 0x2D-0x31: partner list page flip, then the committed swap */
    case 0x2D: {
        s32 slot;
        s32 page;
        s32 row;

        sendSpuCommand(1);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(1, ctx->slotCursor[ctx->partnerCharIdx]);
        slot = ctx->slotCursor[ctx->partnerCharIdx];
        page = slot / 4;
        row = slot % 4;
        ctx->lowerPrevPage = page;
        page--;
        if (page < 0) {
            page = 7;
        }
        ctx->lowerPage = page;
        slot = page * 4 + row;
        ctx->slotCursor[ctx->partnerCharIdx] = slot;
        ctx->itemPtr2 = ctx->itemPtr;
        ctx->itemPtr = func_801E6F54(ctx->partnerCharIdx, ctx->slotCursor[ctx->partnerCharIdx], ctx->lowerPage);
        ctx->lowerPageSlide = -0xE67;
        *statePtr = 0x2E;
        return;
    }
    case 0x2E:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(1, ctx->slotCursor[ctx->partnerCharIdx]);
        ctx->lowerPageSlide += 0x199;
        if (ctx->lowerPageSlide >= 0) {
            ctx->lowerPageSlide = 0;
            *statePtr = 0x2C;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0x2D;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x2F;
            return;
        }
        break;
    case 0x2F: {
        s32 slot;
        s32 page;
        s32 row;

        sendSpuCommand(1);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(1, ctx->slotCursor[ctx->partnerCharIdx]);
        slot = ctx->slotCursor[ctx->partnerCharIdx];
        page = slot / 4;
        row = slot % 4;
        ctx->lowerPrevPage = page;
        page++;
        if (page >= 8) {
            page = 0;
        }
        ctx->lowerPage = page;
        slot = page * 4 + row;
        ctx->slotCursor[ctx->partnerCharIdx] = slot;
        ctx->itemPtr2 = ctx->itemPtr;
        ctx->itemPtr = func_801E6F54(ctx->partnerCharIdx, ctx->slotCursor[ctx->partnerCharIdx], ctx->lowerPage);
        ctx->lowerPageSlide = 0xE67;
        *statePtr = 0x30;
        return;
    }
    case 0x30:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(1, ctx->slotCursor[ctx->partnerCharIdx]);
        ctx->lowerPageSlide -= 0x199;
        if (ctx->lowerPageSlide <= 0) {
            ctx->lowerPageSlide = 0;
            *statePtr = 0x2C;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0x2D;
            return;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x2F;
            return;
        }
        break;
    case 0x31:
        ctx->drawFlags = 0;
        if (func_801E5D64(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
            ctx->messageReturnState = 0x2C;
            *statePtr = 0x43;
            return;
        }
        func_801E626C(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]);
        *statePtr = 0x19;
        return;

    /* 0x32-0x37: the two-choice (0x32-0x34) and three-choice (0x35-0x37) transfer prompts */
    case 0x32:
        ctx->promptCursor = 0;
        /* fallthrough */
    case 0x33:
        ctx->itemPtr = func_801F08D4(1, 8, 7, 0);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        *statePtr = 0x34;
        return;
    case 0x34:
        if (ctx->exchangeMode & MAGIC_EXCHANGE_DIR_MASK) {
            state = *statePtr = 0x37;
            goto dispatch;
        }
        ctx->drawFlags = MAGIC_DRAW_TWO_CHOICE;
        ctx->promptCursor = func_801F6768(inputRepeat, 2, ctx->promptCursor);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        func_801E6810(ctx, 1, ctx->promptCursor);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawFlags = 0;
            *statePtr = ctx->promptCancelState;
        }
        if (inputNew & PADRdown) {
            s32 srcSpell;
            s32 dstSpell;

            srcSpell = g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId;
            dstSpell = g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].magicId;
            sendSpuCommand(2);
            if (ctx->promptCursor == 0) {
                if (srcSpell != 0 && dstSpell != 0) {
                    ctx->exchangeMode = 0;
                    state = 0x39;
                } else {
                    ctx->exchangeMode = 0;
                    state = 0x3B;
                }
            } else {
                ctx->exchangeMode = 0;
                ctx->pageReturnState = 0x34;
                state = 0x3C;
            }
            goto dispatch;
        }
        break;
    case 0x35:
        ctx->promptCursor = 0;
        /* fallthrough */
    case 0x36:
        ctx->itemPtr = func_801F08D4(1, 8, 7, 0);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        ctx->drawFlags = MAGIC_DRAW_THREE_CHOICE;
        *statePtr = 0x37;
        return;
    case 0x37:
        ctx->drawFlags = MAGIC_DRAW_THREE_CHOICE;
        ctx->promptCursor = func_801F6768(inputRepeat, 3, ctx->promptCursor);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        func_801E6858(ctx, 1, ctx->promptCursor);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawFlags = 0;
            *statePtr = ctx->promptCancelState;
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            switch (ctx->promptCursor) {
            case 0:
                ctx->exchangeMode = MAGIC_EXCHANGE_THREE_CHOICE;
                state = 0x39;
                break;
            case 1:
                ctx->exchangeMode = MAGIC_EXCHANGE_THREE_CHOICE | MAGIC_EXCHANGE_TO_PARTNER;
                state = 0x39;
                break;
            case 2:
            default:
                ctx->pageReturnState = 0x34;
                state = 0x3C;
                break;
            }
            goto dispatch;
        }
        break;

    /* 0x38-0x3B: pool the two stocks capped at 100, or swap the two slots outright */
    case 0x38: {
        s32 srcQty;
        s32 dstQty;

        ctx->drawFlags = 0;
        ctx->slotCursor[ctx->partnerCharIdx] = func_801E5C00(ctx->partnerCharIdx, g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId);
        srcQty = g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity;
        dstQty = g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity;
        if (ctx->exchangeMode & MAGIC_EXCHANGE_DIR_MASK) {
            dstQty += srcQty;
            srcQty = 0;
            if (dstQty > 100) {
                srcQty = dstQty - 100;
                dstQty = 100;
            }
        } else {
            srcQty += dstQty;
            dstQty = 0;
            if (srcQty > 100) {
                dstQty = srcQty - 100;
                srcQty = 100;
            }
        }
        g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity = srcQty;
        g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity = dstQty;
        func_801E626C(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]);
        *statePtr = 0x19;
        return;
    }
    case 0x39: {
        s32 srcQty;
        s32 dstQty;

        if (func_801E5D64(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
            ctx->messageReturnState = 0x34;
            *statePtr = 0x43;
            return;
        }
        func_801E5A28(ctx->charIdx, 0);
        func_801E5A28(ctx->partnerCharIdx, 1);
        ctx->slotCursor[ctx->partnerCharIdx] = func_801E5C00(ctx->partnerCharIdx, g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId);
        srcQty = g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity;
        dstQty = g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity;
        if (ctx->exchangeMode & MAGIC_EXCHANGE_DIR_MASK) {
            dstQty += srcQty;
            srcQty = 0;
            if (dstQty > 100) {
                srcQty = dstQty - 100;
                dstQty = 100;
            }
        } else {
            srcQty += dstQty;
            dstQty = 0;
            if (srcQty > 100) {
                dstQty = srcQty - 100;
                srcQty = 100;
            }
        }
        g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity = srcQty;
        g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity = dstQty;
        if (func_801E626C(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) != 0) {
            func_801E5B00(ctx->charIdx, 0);
            func_801E5B00(ctx->partnerCharIdx, 1);
            ctx->returnState = 0x38;
            if (ctx->exchangeMode & MAGIC_EXCHANGE_THREE_CHOICE) {
                ctx->exitState = 0x36;
            } else {
                ctx->exitState = 0x33;
            }
            inputRepeat = 0;
            inputNew = 0;
            state = *statePtr = 0x6A;
            goto dispatch;
        }
        sendSpuCommand(2);
        inputRepeat = 0;
        inputNew = 0;
        ctx->drawFlags = 0;
        *statePtr = 0x19;
        state = 0x19;
        goto dispatch;
    }
    case 0x3A: {
        s32 srcQty;
        s32 dstQty;

        sendSpuCommand(2);
        func_801E5A28(ctx->charIdx, 0);
        func_801E5A28(ctx->partnerCharIdx, 1);
        ctx->slotCursor[ctx->partnerCharIdx] = func_801E5C00(ctx->partnerCharIdx, g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId);
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        srcQty = g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity;
        dstQty = g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity;
        srcQty += dstQty;
        dstQty = 0;
        if (srcQty > 100) {
            dstQty = srcQty - 100;
            srcQty = 100;
        }
        g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity = srcQty;
        g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity = dstQty;
        *statePtr = 0x19;
        return;
    }
    case 0x3B:
        if (func_801E5D64(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
            ctx->messageReturnState = 0x34;
            *statePtr = 0x43;
            return;
        }
        func_801E5A28(ctx->charIdx, 0);
        func_801E5A28(ctx->partnerCharIdx, 1);
        if (func_801E626C(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) != 0) {
            func_801E5B00(ctx->charIdx, 0);
            func_801E5B00(ctx->partnerCharIdx, 1);
            ctx->returnState = 0x31;
            if (ctx->exchangeMode & MAGIC_EXCHANGE_THREE_CHOICE) {
                ctx->exitState = 0x36;
            } else {
                ctx->exitState = 0x33;
            }
            *statePtr = 0x6A;
            return;
        }
        sendSpuCommand(2);
        ctx->drawFlags = 0;
        *statePtr = 0x19;
        return;

    /* 0x3C-0x40: quantity editor - split one spell between the two characters */
    case 0x3C:
        if (func_801E5CAC(ctx->charIdx, ctx->exchangeSpellId) != 0) {
            ctx->slotCursor[ctx->charIdx] = func_801E5C00(ctx->charIdx, ctx->exchangeSpellId);
            ctx->upperPage = ctx->slotCursor[ctx->charIdx] / 4;
        }
        if (func_801E5CAC(ctx->partnerCharIdx, ctx->exchangeSpellId) != 0) {
            ctx->slotCursor[ctx->partnerCharIdx] = func_801E5C00(ctx->partnerCharIdx, ctx->exchangeSpellId);
            ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        }
        ctx->charQty = func_801E5CAC(ctx->charIdx, ctx->exchangeSpellId);
        ctx->partnerQty = func_801E5CAC(ctx->partnerCharIdx, ctx->exchangeSpellId);
        /* fallthrough */
    case 0x3D:
        ctx->itemPtr = func_801F08D4(1, 8, 7, 0);
        ctx->drawFlags |= MAGIC_DRAW_QTY_EDITOR;
        func_801E5A28(ctx->charIdx, 0);
        func_801E5A28(ctx->partnerCharIdx, 1);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        if (!(ctx->drawFlags & MAGIC_DRAW_NO_LOWER_CUR)) {
            func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        }
        *statePtr = 0x3F;
        return;
    case 0x3E:
        ctx->itemPtr = func_801F08D4(1, 8, 7, 0);
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        if (!(ctx->drawFlags & MAGIC_DRAW_NO_LOWER_CUR)) {
            func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        }
        *statePtr = 0x3F;
        return;
    case 0x3F:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6B3C(0, ctx->slotCursor[ctx->charIdx]);
        {
            s32 moved;
            s32 step;
            s32 capped;

            moved = 0;
            if (ctx->charQty < 100 && ctx->partnerQty != 0) {
                step = 10;
                if (ctx->partnerQty <= 10) {
                    step = ctx->partnerQty;
                }
                /* clamped in a copy and copied back: clamping step in place costs 5 rows */
                capped = step;
                if (capped >= 100 - ctx->charQty) {
                    capped = 100 - ctx->charQty;
                }
                step = capped;
                if (inputRepeat & PADLleft) {
                    sendSpuCommand(1);
                    moved = 1;
                    ctx->charQty++;
                    ctx->partnerQty--;
                }
                if ((inputRepeat & PADLup) && !moved) {
                    sendSpuCommand(1);
                    moved = 1;
                    ctx->charQty += step;
                    ctx->partnerQty -= step;
                }
            }
            if (ctx->partnerQty < 100 && ctx->charQty != 0) {
                step = 10;
                if (ctx->charQty <= 10) {
                    step = ctx->charQty;
                }
                capped = step;
                if (capped >= 100 - ctx->partnerQty) {
                    capped = 100 - ctx->partnerQty;
                }
                step = capped;
                if ((inputRepeat & PADLright) && !moved) {
                    sendSpuCommand(1);
                    moved = 1;
                    ctx->charQty--;
                    ctx->partnerQty++;
                }
                if ((inputRepeat & PADLdown) && !moved) {
                    sendSpuCommand(1);
                    ctx->charQty -= step;
                    ctx->partnerQty += step;
                }
            }
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawFlags &= ~MAGIC_DRAW_QTY_EDITOR;
            func_801E5B00(ctx->charIdx, 0);
            func_801E5B00(ctx->partnerCharIdx, 1);
            *statePtr = ctx->pageReturnState;
        }
        if (inputNew & PADRdown) {
            if (func_801E5D64(ctx->charIdx, ctx->partnerCharIdx,
                              ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
                ctx->messageReturnState = 0x3F;
                *statePtr = 0x43;
                return;
            }
            g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId = ctx->exchangeSpellId;
            g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity = ctx->partnerQty;
            g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].magicId = ctx->exchangeSpellId;
            g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity = ctx->charQty;
            if (func_801E626C(ctx->charIdx, ctx->partnerCharIdx,
                              ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
                sendSpuCommand(2);
                inputRepeat = 0;
                inputNew = 0;
                ctx->drawFlags = 0;
                *statePtr = 0x19;
                state = 0x19;
                goto dispatch;
            }
            func_801E5B00(ctx->charIdx, 0);
            func_801E5B00(ctx->partnerCharIdx, 1);
            ctx->returnState = 0x40;
            ctx->exitState = 0x3D;
            *statePtr = 0x6A;
        }
        if (!(ctx->drawFlags & MAGIC_DRAW_NO_LOWER_CUR)) {
            func_801E6AEC(0, ctx->slotCursor[ctx->partnerCharIdx]);
        }
        break;
    case 0x40:
        g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].magicId = ctx->exchangeSpellId;
        g_gameState.chars[ctx->charIdx].magic[ctx->slotCursor[ctx->charIdx]].quantity = ctx->partnerQty;
        g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].magicId = ctx->exchangeSpellId;
        g_gameState.chars[ctx->partnerCharIdx].magic[ctx->slotCursor[ctx->partnerCharIdx]].quantity = ctx->charQty;
        func_801F1B4C(ctx->charIdx);
        func_801F1B4C(ctx->partnerCharIdx);
        if (func_801E5D64(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]) == 0) {
            ctx->messageReturnState = 0x3F;
            *statePtr = 0x43;
            return;
        }
        func_801E626C(ctx->charIdx, ctx->partnerCharIdx, ctx->slotCursor[ctx->charIdx], ctx->slotCursor[ctx->partnerCharIdx]);
        ctx->drawFlags = 0;
        *statePtr = 0x19;
        return;

    /* 0x41-0x42: leave the exchange screen for the command row */
    case 0x41:
        *statePtr = 0x42;
        /* fallthrough */
    case 0x42:
        ctx->drawMode = 0;
        *statePtr = 3;
        return;

    /* 0x43-0x44: timed refusal message */
    case 0x43:
        sendSpuCommand(5);
        ctx->messageTimer = 600;
        ctx->drawFlags |= MAGIC_DRAW_MESSAGE;
        *statePtr = 0x44;
        return;
    case 0x44:
        ctx->messageTimer--;
        if (inputNew & (PADRup | PADRdown)) {
            func_801F7BEC(inputNew);
            ctx->messageTimer = 0;
        }
        if (ctx->messageTimer <= 0) {
            ctx->messageTimer = 0;
            ctx->drawFlags &= ~MAGIC_DRAW_MESSAGE;
            *statePtr = ctx->messageReturnState;
        }
        return;

    /* 0x45-0x4A: sort popup - slide in, pick an order, slide out */
    case 0x45:
        ctx->sortOptionList = D_801EC820;
        ctx->sortPopupSlide = 0;
        ctx->sortCursor = 0;
        *statePtr = 0x46;
        /* fallthrough */
    case 0x46:
        ctx->sortPopupSlide += 0x100;
        if (ctx->sortPopupSlide >= 0x1000) {
            ctx->sortPopupSlide = 0x1000;
            *statePtr = 0x47;
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 1, ctx->sortCursor);
        return;
    case 0x47:
        *statePtr = 0x48;
        /* fallthrough */
    case 0x48:
        ctx->sortCursor = func_801F6768(inputRepeat, ctx->sortOptionCount, ctx->sortCursor);
        ctx->itemPtr = func_801F08D4(1, 8, ctx->sortCursor + 15, 1);
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            if (func_801E64FC(ctx->charIdx, ctx->sortCursor) == 0) {
                *statePtr = 0x4B;
            } else {
                *statePtr = 0x49;
            }
        } else if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x49;
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 1, ctx->sortCursor);
        return;
    case 0x49:
        *statePtr = 0x4A;
        /* fallthrough */
    case 0x4A:
        ctx->sortPopupSlide -= 0x100;
        if (ctx->sortPopupSlide <= 0) {
            ctx->sortPopupSlide = 0;
            ctx->sortOptionList = 0;
            *statePtr = 2;
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 1, ctx->sortCursor);
        return;

    /* 0x4B-0x4F: manual rearrange - pick the slot to move (upper list) */
    case 0x4B:
        ctx->drawMode = 5;
        ctx->upperSlot = ctx->slotCursor[ctx->charIdx];
        ctx->lowerSlot = ctx->upperSlot;
        ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->upperSlot));
        ctx->rearrangeSlide = 0x1000;
        ctx->upperPageSlide = 0;
        ctx->lowerPageSlide = 0;
        ctx->upperPage = ctx->upperSlot / 4;
        ctx->lowerPage = ctx->lowerSlot / 4;
        *statePtr = 0x4C;
        /* fallthrough */
    case 0x4C:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        func_801E68A0(1, ctx->upperSlot);
        ctx->rearrangeSlide -= 0x100;
        if (ctx->rearrangeSlide <= 0) {
            ctx->rearrangeSlide = 0;
            *statePtr = 0x4E;
            return;
        }
        break;
    case 0x4D:
        ctx->upperSlot = 0;
        ctx->lowerSlot = 0;
        ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->upperSlot));
        ctx->upperPage = ctx->upperSlot / 4;
        ctx->lowerPage = ctx->lowerSlot / 4;
        *statePtr = 0x4E;
        /* fallthrough */
    case 0x4E:
        *statePtr = 0x4F;
        /* fallthrough */
    case 0x4F:
        {
            s32 page;

            page = ctx->upperSlot / 4;
            ctx->upperSlot = page * 4 + func_801F6768(inputRepeat, 4, ctx->upperSlot % 4);
            ctx->slotCursor[ctx->charIdx] = ctx->upperSlot;
            ctx->upperPage = page;
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        func_801E68A0(1, ctx->upperSlot);
        if (inputRepeat & PADLleft) {
            *statePtr = 0x50;
            return;
        }
        if (inputRepeat & PADLright) {
            *statePtr = 0x52;
            return;
        }
        if (popcount(func_80036EC0()) >= 2) {
            ctx->drawFlags |= MAGIC_DRAW_SUB_CURSOR;
            if (inputRepeat & PADL1) {
                ctx->upperSlot = 0;
                ctx->headerCursorActive = 0;
                ctx->refreshHelpText = 0;
                ctx->lowerSlot = 0;
                ctx->lowerPage = 0;
                ctx->upperPage = 0;
                ctx->slotCursor[ctx->charIdx] = ctx->upperSlot;
                ctx->returnState = 0x4D;
                state = 4;
                goto dispatch;
            }
            if (inputRepeat & PADR1) {
                ctx->upperSlot = 0;
                ctx->headerCursorActive = 0;
                ctx->refreshHelpText = 0;
                ctx->lowerSlot = 0;
                ctx->lowerPage = 0;
                ctx->upperPage = 0;
                ctx->slotCursor[ctx->charIdx] = ctx->upperSlot;
                ctx->returnState = 0x4D;
                state = 6;
                goto dispatch;
            }
        }
        ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->upperSlot));
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x5B;
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            *statePtr = 0x54;
        }
        if (inputNew & PADRleft) {
            s32 slot;
            s32 spellId;
            s32 qty;

            slot = ctx->upperSlot;
            spellId = g_gameState.chars[ctx->charIdx].magic[slot].magicId;
            qty = g_gameState.chars[ctx->charIdx].magic[slot].quantity;
            if (spellId != 0 && qty != 0) {
                sendSpuCommand(2);
                ctx->discardReturnState = 0x4E;
                ctx->discardSlot = slot;
                ctx->discardCharIdx = ctx->charIdx;
                state = 0x6C;
                goto dispatch;
            }
            sendSpuCommand(5);
            return;
        }
        break;

    /* 0x50-0x53: rearrange upper list page flip */
    case 0x50:
        {
            s32 page;
            s32 slot;

            sendSpuCommand(1);
            ctx->itemPtr2 = ctx->itemPtr;
            ctx->upperPrevPage = ctx->upperPage;
            page = ctx->upperSlot / 4;
            slot = ctx->upperSlot % 4;
            page--;
            if (page < 0) {
                page = 7;
            }
            ctx->upperSlot = page * 4 + slot;
            ctx->upperPage = page;
            ctx->upperPageSlide = -0xE67;
            ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->upperSlot));
            *statePtr = 0x51;
        }
        /* fallthrough */
    case 0x51:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        func_801E68A0(1, ctx->upperSlot);
        ctx->upperPageSlide += 0x199;
        if (ctx->upperPageSlide >= 0) {
            ctx->upperPageSlide = 0;
            *statePtr = 0x4E;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x52;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0x50;
        }
        break;
    case 0x52:
        {
            s32 page;
            s32 slot;

            sendSpuCommand(1);
            ctx->itemPtr2 = ctx->itemPtr;
            ctx->upperPrevPage = ctx->upperPage;
            page = ctx->upperSlot / 4;
            slot = ctx->upperSlot % 4;
            page++;
            if (page >= 8) {
                page = 0;
            }
            ctx->upperSlot = page * 4 + slot;
            ctx->upperPage = page;
            ctx->upperPageSlide = 0xE67;
            ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->upperSlot));
            *statePtr = 0x53;
        }
        /* fallthrough */
    case 0x53:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        func_801E68A0(1, ctx->upperSlot);
        ctx->upperPageSlide -= 0x199;
        if (ctx->upperPageSlide <= 0) {
            ctx->upperPageSlide = 0;
            *statePtr = 0x4E;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x52;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0x50;
        }
        break;

    /* 0x54-0x5C: rearrange - pick the slot to swap with (lower list, 0x56-0x59 page flip), swap
     * the pair, slide back out */
    case 0x54:
        *statePtr = 0x55;
        /* fallthrough */
    case 0x55:
        {
            s32 page;
            s32 slot;
            s32 srcSpell;
            s32 dstSpell;

            page = ctx->lowerSlot / 4;
            slot = ctx->lowerSlot % 4;
            ctx->lowerSlot = page * 4 + func_801F6768(inputRepeat, 4, slot);
            func_801E69EC(ctx, 0, ctx->commandIdx);
            func_801E6940(ctx, 0, ctx->sortCursor);
            func_801E68A0(0, ctx->upperSlot);
            func_801E68F0(1, ctx->lowerSlot);
            if (inputRepeat & PADLleft) {
                *statePtr = 0x56;
                return;
            }
            if (inputRepeat & PADLright) {
                *statePtr = 0x58;
                return;
            }
            ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->lowerSlot));
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                *statePtr = 0x4E;
            }
            if (inputNew & PADRdown) {
                srcSpell = func_801E6648(ctx->charIdx, ctx->upperSlot);
                dstSpell = func_801E6648(ctx->charIdx, ctx->lowerSlot);
                if (dstSpell == srcSpell && dstSpell == 0) {
                    sendSpuCommand(5);
                } else {
                    sendSpuCommand(2);
                    *statePtr = 0x5A;
                }
            }
        }
        break;
    case 0x56: {
        s32 page;
        s32 slot;

        sendSpuCommand(1);
        ctx->itemPtr2 = ctx->itemPtr;
        ctx->lowerPrevPage = ctx->lowerPage;
        page = ctx->lowerSlot / 4;
        slot = ctx->lowerSlot % 4;
        page--;
        if (page < 0) {
            page = 7;
        }
        ctx->lowerSlot = page * 4 + slot;
        ctx->lowerPage = page;
        ctx->lowerPageSlide = -0xE67;
        ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->lowerSlot));
        *statePtr = 0x57;
        /* fallthrough */
    }
    case 0x57:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        func_801E68A0(0, ctx->upperSlot);
        func_801E68F0(1, ctx->lowerSlot);
        ctx->lowerPageSlide += 0x199;
        if (ctx->lowerPageSlide >= 0) {
            ctx->lowerPageSlide = 0;
            *statePtr = 0x55;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x58;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0x56;
        }
        break;
    case 0x58: {
        s32 page;
        s32 slot;

        sendSpuCommand(1);
        ctx->itemPtr2 = ctx->itemPtr;
        ctx->lowerPrevPage = ctx->lowerPage;
        page = ctx->lowerSlot / 4;
        slot = ctx->lowerSlot % 4;
        page++;
        if (page >= 8) {
            page = 0;
        }
        ctx->lowerSlot = page * 4 + slot;
        ctx->lowerPage = page;
        ctx->lowerPageSlide = 0xE67;
        ctx->itemPtr = getSpellEntityData(func_801E6648(ctx->charIdx, ctx->lowerSlot));
        *statePtr = 0x59;
        /* fallthrough */
    }
    case 0x59:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        func_801E68A0(0, ctx->upperSlot);
        func_801E68F0(1, ctx->lowerSlot);
        ctx->lowerPageSlide -= 0x199;
        if (ctx->lowerPageSlide <= 0) {
            ctx->lowerPageSlide = 0;
            *statePtr = 0x55;
        }
        if (inputNew & PADLright) {
            *statePtr = 0x58;
        }
        if (inputNew & PADLleft) {
            *statePtr = 0x56;
        }
        break;
    case 0x5A: {
        s32 firstSpell;
        s32 firstQty;
        s32 secondSpell;
        s32 secondQty;

        firstSpell = func_801E6648(ctx->charIdx, ctx->upperSlot);
        firstQty = func_801E677C(ctx->charIdx, ctx->upperSlot);
        secondSpell = func_801E6648(ctx->charIdx, ctx->lowerSlot);
        secondQty = func_801E677C(ctx->charIdx, ctx->lowerSlot);
        setCharacterMagicSlotId(ctx->charIdx, ctx->lowerSlot, firstSpell);
        setCharacterMagicSlotId(ctx->charIdx, ctx->upperSlot, secondSpell);
        func_801E67E0(ctx->charIdx, ctx->lowerSlot, firstQty);
        func_801E67E0(ctx->charIdx, ctx->upperSlot, secondQty);
        func_801F1B4C(ctx->charIdx);
        func_801F5400(ctx->charIdx);
        state = 0x4E;
        goto dispatch;
    }
    case 0x5B:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        ctx->slotCursor[ctx->charIdx] = ctx->upperSlot;
        *statePtr = 0x5C;
        break;
    case 0x5C:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6940(ctx, 0, ctx->sortCursor);
        ctx->rearrangeSlide += 0x100;
        if (ctx->rearrangeSlide >= 0x1000) {
            ctx->rearrangeSlide = 0x1000;
            ctx->drawMode = 0;
            *statePtr = 0x47;
        }
        break;

    /* 0x5D-0x63: whole-inventory transfer - pick the receiving character (0x61), then the
     * giving one (0x63) */
    case 0x5D:
        ctx->partnerCharIdx = func_801F56E4(ctx->charIdx, func_80036EC0() & ~(1 << ctx->charIdx));
        *statePtr = 0x5E;
        /* fallthrough */
    case 0x5E:
        *statePtr = 0x5F;
        /* fallthrough */
    case 0x5F:
        *statePtr = 0x60;
        /* fallthrough */
    case 0x60:
        *statePtr = 0x61;
        /* fallthrough */
    case 0x61:
        ctx->itemPtr = func_801F08D4(1, 8, 12, 0);
        ctx->drawFlags |= MAGIC_DRAW_SUB_CURSOR;
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            if (ctx->charIdx == ctx->partnerCharIdx) {
                ctx->partnerCharIdx = func_801F56E4(ctx->charIdx, func_80036EC0() & ~(1 << ctx->charIdx));
            }
            state = 0x62;
            goto dispatch;
        }
        if (inputRepeat & (PADL1 | PADLleft)) {
            ctx->headerCursorActive = 1;
            ctx->drawMode = 3;
            ctx->refreshHelpText = 0;
            ctx->returnState = 0x61;
            state = 4;
            goto dispatch;
        }
        if (inputRepeat & (PADR1 | PADLright)) {
            ctx->headerCursorActive = 1;
            ctx->drawMode = 3;
            ctx->refreshHelpText = 0;
            ctx->returnState = 0x61;
            state = 6;
            goto dispatch;
        }
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(1);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawMode = 0;
            *statePtr = 3;
        }
        break;
    case 0x62:
        ctx->drawFlags |= MAGIC_DRAW_LR_ARROWS;
        ctx->itemPtr = func_801F08D4(1, 8, 13, 0);
        ctx->drawMode = 4;
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] >> 2;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        *statePtr = 0x63;
        break;
    case 0x63:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        if (popcount(func_80036EC0() & ~(1 << ctx->charIdx)) >= 2) {
            ctx->drawFlags |= MAGIC_DRAW_SUB_CURSOR;
            if (inputRepeat & (PADL1 | PADLleft)) {
                *statePtr = 0x64;
                break;
            }
            if (inputRepeat & (PADR1 | PADLright)) {
                *statePtr = 0x66;
                break;
            }
        }
        if (inputNew & PADRdown) {
            ctx->drawFlags &= ~MAGIC_DRAW_LR_ARROWS;
            *statePtr = 0x68;
        }
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawMode = 3;
            ctx->drawFlags &= ~MAGIC_DRAW_LR_ARROWS;
            *statePtr = 0x61;
        }
        break;

    /* 0x64-0x67: giving-character switch and its slides */
    case 0x64: {
        s32 others;

        D_801ECD90 = D_801ECBC0;
        sendSpuCommand(1);
        /* taken before prevCharIdx is saved: folded into the call below, the store moves ahead
         * of this call */
        others = func_80036EC0() & ~(1 << ctx->charIdx);
        ctx->prevCharIdx = ctx->partnerCharIdx;
        ctx->partnerCharIdx = func_801F565C(ctx->partnerCharIdx, others);
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        ctx->partnerSlideOffset = -0xF80;
        *statePtr = 0x65;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        break;
    }
    case 0x65:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        ctx->partnerSlideOffset += 0x80;
        if (ctx->partnerSlideOffset >= 0) {
            ctx->partnerSlideOffset = 0;
            *statePtr = 0x63;
        }
        if (inputNew & (PADL1 | PADLleft)) {
            *statePtr = 0x64;
        }
        if (inputNew & (PADR1 | PADLright)) {
            *statePtr = 0x66;
        }
        break;
    case 0x66: {
        s32 others;

        sendSpuCommand(1);
        D_801ECD90 = D_801ECBC0;
        /* taken before prevCharIdx is saved, as in state 0x64 */
        others = func_80036EC0() & ~(1 << ctx->charIdx);
        ctx->prevCharIdx = ctx->partnerCharIdx;
        ctx->partnerCharIdx = func_801F56E4(ctx->partnerCharIdx, others);
        ctx->lowerPage = ctx->slotCursor[ctx->partnerCharIdx] / 4;
        ctx->partnerSlideOffset = 0xF80;
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        *statePtr = 0x67;
        break;
    }
    case 0x67:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        ctx->partnerSlideOffset -= 0x80;
        if (ctx->partnerSlideOffset <= 0) {
            ctx->partnerSlideOffset = 0;
            *statePtr = 0x63;
        }
        if (inputNew & (PADL1 | PADLleft)) {
            *statePtr = 0x64;
        }
        if (inputNew & (PADR1 | PADLright)) {
            *statePtr = 0x66;
        }
        break;

    /* 0x68-0x69: check whether max HP would drop, then move every spell across */
    case 0x68:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        func_801E6A9C(0);
        func_801E6AC4(1);
        if (func_801E6058(ctx->partnerCharIdx, ctx->charIdx)) {
            ctx->returnState = 0x69;
            ctx->exitState = 0x62;
            *statePtr = 0x6A;
            break;
        }
        sendSpuCommand(2);
        ctx->drawFlags = 0;
        state = 0x69;
        goto dispatch;
    case 0x69:
        ctx->drawFlags = 0;
        func_801E5E74(ctx->partnerCharIdx, ctx->charIdx);
        *statePtr = 0x60;
        break;

    /* 0x6A-0x6B: the shared yes/no box (returnState on Yes, exitState on No) */
    case 0x6A:
        playSoundEffect(0x10);
        func_801F728C(func_801F6AFC(0x3A), 0x6F);
        ctx->yesNoCursor = 1;
        ctx->drawFlags |= MAGIC_DRAW_YES_NO;
        *statePtr = 0x6B;
        /* fallthrough */
    case 0x6B:
        func_801E69EC(ctx, 0, ctx->commandIdx);
        ctx->yesNoCursor = func_801F6768(inputRepeat, 2, ctx->yesNoCursor);
        func_801F6F88(ctx->yesNoCursor | 0x80);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->drawFlags &= ~MAGIC_DRAW_YES_NO;
            *statePtr = ctx->exitState;
            state = ctx->exitState;
            goto dispatch;
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            ctx->drawFlags &= ~MAGIC_DRAW_YES_NO;
            if (ctx->yesNoCursor != 0) {
                *statePtr = ctx->exitState;
                state = ctx->exitState;
            } else {
                ctx->drawFlags = 0;
                *statePtr = ctx->returnState;
                state = ctx->returnState;
            }
            goto dispatch;
        }
        break;

    /* 0x6C-0x6F: discard the stock under the cursor, with a second confirm when max HP would
     * drop */
    case 0x6C:
        menumgc_formatSpellCharName(func_801F08D4(1, 8, 14, 0), D_801ED020,
                                    g_gameState.chars[ctx->discardCharIdx].magic[ctx->discardSlot].magicId, ctx->discardCharIdx);
        func_801F728C(D_801ED020, 0x6B);
        ctx->yesNoCursor = 1;
        ctx->discardPromptOpen = 1;
        func_801F6F88(ctx->yesNoCursor | 0x80);
        *statePtr = 0x6D;
        return;
    case 0x6D:
        ctx->yesNoCursor = func_801F6768(inputRepeat, 2, ctx->yesNoCursor);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->discardPromptOpen = 0;
            *statePtr = ctx->discardReturnState;
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            if (ctx->yesNoCursor != 0) {
                ctx->discardPromptOpen = 0;
                *statePtr = ctx->discardReturnState;
            } else if (func_801E5F5C(ctx->discardCharIdx, g_gameState.chars[ctx->discardCharIdx].magic[ctx->discardSlot].magicId) == 0) {
                ctx->discardPromptOpen = 0;
                g_gameState.chars[ctx->discardCharIdx].magic[ctx->discardSlot].magicId = 0;
                g_gameState.chars[ctx->discardCharIdx].magic[ctx->discardSlot].quantity = 0;
                func_801F7B10(ctx->discardCharIdx);
                func_801F1B4C(ctx->discardCharIdx);
                func_801F5400(ctx->discardCharIdx);
                *statePtr = ctx->discardReturnState;
            } else {
                state = 0x6E;
                goto dispatch;
            }
        }
        func_801F6F88(ctx->yesNoCursor | 0x80);
        return;
    case 0x6E:
        func_801F728C(func_801F6AFC(0x3A), 0x6F);
        ctx->yesNoCursor = 1;
        func_801F6F88(0x81);
        *statePtr = 0x6F;
        return;
    case 0x6F:
        ctx->yesNoCursor = func_801F6768(inputRepeat, 2, ctx->yesNoCursor);
        if (inputNew & PADRup) {
            sendSpuCommand(3);
            ctx->discardPromptOpen = 0;
            *statePtr = ctx->discardReturnState;
        }
        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            if (ctx->yesNoCursor != 0) {
                ctx->discardPromptOpen = 0;
                *statePtr = ctx->discardReturnState;
            } else {
                ctx->discardPromptOpen = 0;
                g_gameState.chars[ctx->discardCharIdx].magic[ctx->discardSlot].magicId = 0;
                g_gameState.chars[ctx->discardCharIdx].magic[ctx->discardSlot].quantity = 0;
                func_801F7B10(ctx->discardCharIdx);
                func_801F1B4C(ctx->discardCharIdx);
                func_801F5400(ctx->discardCharIdx);
                *statePtr = ctx->discardReturnState;
            }
        }
        func_801F6F88(ctx->yesNoCursor | 0x80);
        return;

    /* 0x70-0x71: fade out, unlink the task and pop the menu stack */
    case 0x70:
        *statePtr = 0x71;
        /* fallthrough */
    case 0x71:
        ctx->fadeScale -= 0x100;
        if (ctx->fadeScale < 0) {
            ctx->fadeScale = 0;
            func_801F18FC(ctx);
            func_801F0BB0();
        }
        func_801E69EC(ctx, 1, ctx->commandIdx);
    }
    return;
}

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
s32 func_801EB1A0(void *ctx, s32 arg1, s32 index, s32 a3_unused, s32 xExtra) {
    u8 buf[0x80];
    s32 result = arg1;
    u8 *itemText = ((u8 **)g_menuDisplayCfg.dataPtr)[index];

    if (itemText != 0) {
        s32 deltaX = xExtra + 0xA;
        s32 x = g_menuDisplayCfg.x + deltaX;
        s32 y = g_menuDisplayCfg.y + 9;

        decodeMessage(itemText, buf, -1);
        result = func_801F0FEC(ctx, arg1, x, y, buf, 7);
    }
    return result;
}

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
    u8 *cfg = (u8 *)&g_menuDisplayCfg;

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
