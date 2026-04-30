#include "common.h"
#include "menu.h"
#include "gamestate.h"
#include "battle.h"
#include "gf.h"

/** @brief Auto-junction priority tables (Atk/Mag/Def), each a 0xFF-terminated slot type list. */
extern u8 *g_autoJunctionPriority[];

extern u8 D_801EEAC0[];
extern u8 D_80078E00[];
extern JunctionMenuEntry g_junctionChars[];
extern JunctionGfEntry g_junctionGfTable[];
extern u8 g_junctionBackup[20];
extern u8 D_801EEB28[];
extern BattleCharData g_junctionPreview;
extern u8 g_junctionMenuActive;
extern MenuDisplayConfig g_menuDisplayCfg;
extern s32 g_menuColor;
extern void junctionMenuUpdate();
extern void renderJunctionMenu();
extern s32 g_assignedAbilities[];
extern s32 g_availableAbilities[];
extern u8 D_801EEF10[];
extern u8 D_801EEF38;
extern u8 D_801EEF40[];
extern u8 D_801EEF9A;
extern u8 getAbilityCategory(s32 id);
extern u8 g_characterAbilities[];
extern u8 D_801EEED0[];
extern s32 func_801F776C(s32 magicId, s32 slotType);
extern s32 getAbilityEntryDesc(s32 arg);
extern void playSoundEffect(s32 soundId);
extern void sendSpuCommand(s32 soundId);
extern s32 getAbilityDesc(s32 arg);
extern u16 D_801FA3C8[];
extern u8 D_801EEB1C[];
extern void func_801EFBB4(s32 renderCtx, s32 param, void *callback);
extern s32 renderMagicItemCallback();
extern JunctionGfEntry D_801EEDD0;
extern void func_800300F8(s32 renderCtx, s32 x, s32 w, s32 y, s32 color, s32 menuColor, s32 selColor);
extern s32 func_801F3FB4(u16 statusFlags);
extern s32 getCharNamePtr(u8 characterId);
extern u32 func_801F0FEC(s32 renderCtx, s32 cursorY, s32 x, s32 height, s32 namePtr, s32 gfInfo);
extern s32 func_801EF9AC(s32 renderCtx, s32 cursorY, s32 scale, s32 color);
extern s32 func_8002FF34(s32 renderCtx, s32 cursorY, s32 stringId, s32 x, s32 y, s32 color);
extern u8 *getAbilityName(s32 id);
extern s32 D_801EED00;
extern u8 D_801EEC50[];

extern MagicJunctionData g_magicJunctionData[];
extern s32 popcount(s32 flags);

/** @brief Junction menu layout constants (pixel positions). */
#define JNC_ROW_HEIGHT      13   /**< Row height in pixels. */
#define JNC_ROWS_PER_PAGE   4    /**< Rows visible per page. */
#define JNC_STAT_ROWS       11   /**< Rows in stat junction list. */
#define JNC_ABILITY_PAGES   5    /**< Rows per ability page. */
#define JNC_LEFT_COL_ROWS   3    /**< Rows in left stat column. */

#define JNC_Y_MAGIC_LIST    63   /**< Y origin for magic list panel. */
#define JNC_Y_STAT_LIST     66   /**< Y origin for stat junction list. */
#define JNC_Y_LEFT_COL      81   /**< Y origin for left stat column. */
#define JNC_Y_ABILITY       148  /**< Y origin for ability panel. */
#define JNC_Y_RIGHT_COL     153  /**< Y origin for right stat column. */

#define JNC_W_MAGIC_LIST    90   /**< Width of magic list entries. */
#define JNC_W_STAT_LIST     220  /**< Width of stat junction list. */
#define JNC_W_LEFT_COL      43   /**< Width of left stat column. */
#define JNC_W_RIGHT_COL     70   /**< Width of right stat column. */
#define JNC_W_ABILITY       40   /**< Width of ability entries. */
#define JNC_W_ABILITY_WIDE  200  /**< Width of wide ability entries (page 3). */

/**
 * @brief Decode FF8 encoded text string with escape sequence handling.
 *
 * Uses PS1 scratchpad RAM as intermediate decode buffer.
 * Handles escape codes: 0x00/0x01/0x07 = end, 0x02 = next segment,
 * 0x0A+0x27 = character name substitution, 0x05 = control callback,
 * 0x10-0x18 = control codes, 0x19-0x1F = two-byte sequences.
 *
 * @param src Source encoded string (NULL = immediate return).
 * @param dst Destination buffer for decoded text.
 * @param charIdx Character index for name substitution.
 * @return Pointer to scratchpad decode buffer.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", decodeMenuString);

/**
 * @brief Auto-junction the best magic for a given slot type.
 *
 * Scores all 32 magic slots and picks the best for the junction type.
 * For defense element/status, finds the first empty sub-slot.
 *
 * @param charIdx Character index (0-7).
 * @param magicSlots Pointer to magic slot pairs (magicId, quantity).
 * @param slotType Junction slot type (0-12).
 * @param flagMask Available magic bitmask.
 * @return Updated flagMask with selected slot's bit cleared.
 */
s32 autoJunctionSlot(charIdx, magicSlots, slotType, flagMask)
    s32 charIdx;
    MagicSlot *magicSlots;
    s32 slotType;
    s32 flagMask;
{
    s32 bestScore = 0;
    s32 bestMagicId = bestScore; /* Regalloc: chained init for prologue save order */
    MagicJunctionData *tableBase;
    s32 bestBitIdx = -1;
    s32 i = bestMagicId; /* Regalloc: chained init for prologue save order */
    MagicJunctionData *entry;
    s32 magicId; /* s32 not u8: avoids andi truncation on bestMagicId assignment */
    s32 quantity; /* s32 not u8: fixes mult operand order and s-reg numbering */

    tableBase = g_magicJunctionData; /* Regalloc: intermediate var controls addu operand order */
    do {
        s32 mask = 1 << i;
        if (flagMask & mask) {
            s32 score;

            magicId = magicSlots->magicId;
            quantity = magicSlots->quantity;
            entry = tableBase + magicId;

            if (slotType < JUNCTION_ATK_ELEM) {
                score = entry->statJunction[slotType];
            } else if (slotType == JUNCTION_ATK_ELEM) {
                score = entry->atkElemBonus * popcount(entry->atkElemFlags);
            } else if (slotType == JUNCTION_ATK_STATUS) {
                score = entry->atkStatusBonus * popcount(entry->atkStatusFlags);
            } else if (slotType == JUNC_SLOT_DEF_ELEM) {
                score = entry->defElemBonus * popcount(entry->defElemFlags);
            } else if (slotType == JUNC_SLOT_DEF_STATUS) {
                score = entry->defStatusBonus * popcount(entry->defStatusFlags);
            } else {
                score = 0;
            }

            score *= quantity;
            if (score > bestScore) {
                bestMagicId = magicId;
                bestBitIdx = i;
                bestScore = score;
            }
        }
        i++;
        magicSlots++;
    } while (i < MAGIC_SLOT_COUNT);

    if (bestBitIdx >= 0) {
        flagMask &= ~(1 << bestBitIdx);

        if (slotType < JUNC_SLOT_DEF_ELEM) {
            g_gameState.chars[charIdx].junctions[slotType] = bestMagicId;
        }

        if (slotType == JUNC_SLOT_DEF_ELEM) {
            u8 *slot = &g_gameState.chars[charIdx].junctions[JUNCTION_DEF_ELEM_0];
            s32 j;
            for (j = 0; j < 4; j++) {
                if (*slot == 0) {
                    *slot = bestMagicId;
                    break;
                }
                slot++;
            }
        }

        if (slotType == JUNC_SLOT_DEF_STATUS) {
            u8 *slot = &g_gameState.chars[charIdx].junctions[JUNCTION_DEF_STATUS_0];
            s32 j;
            for (j = 0; j < 4; j++) {
                if (*slot == 0) {
                    *slot = bestMagicId;
                    break;
                }
                slot++;
            }
        }
    }

    return flagMask;
}

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
                pos = autoJunctionSlot(charIdx, abilityList, slotType);
                count--;
            } while (count > 0);
        }
    } else if (slotType == JUNC_SLOT_DEF_STATUS) {
        count = g_junctionChars[charIdx].abilityCount[0];
        if (count > 0) {
            do {
                pos = autoJunctionSlot(charIdx, abilityList, slotType);
                count--;
            } while (count > 0);
        }
    } else {
        pos = autoJunctionSlot(charIdx, abilityList, slotType);
    }
    return pos;
}

/**
 * @brief Auto-junction all slots for a character.
 *
 * Implements the "Auto" junction command (Atk/Mag/Def). Builds a bitmask
 * of available magic (nonzero ID and quantity), clears all current junction
 * slots, then iterates through the selected priority table assigning the
 * best available magic to each stat slot.
 *
 * @param charIdx Character index (0-7).
 * @param tableIdx Auto-junction mode (AutoJunctionMode: ATK=0, MAG=1, DEF=2).
 */
void autoJunctionAll(s32 charIdx, s32 tableIdx) {
    s32 flagMask = 0;
    u32 rawFlags;
    u8 *magicSlots = g_gameState.chars[charIdx].magic;
    s32 slotType;
    s32 i = flagMask;
    s32 bit;
    u8 *slotTable;
    u8 *slotTypes;
    s32 availFlags;

    /* Build bitmask of available magic */
    do {
        u8 magicId = *magicSlots++;
        u8 quantity = *magicSlots++;
        if (magicId != 0 && quantity != 0) {
            flagMask |= (1 << i);
        }
    } while (++i < MAGIC_SLOT_COUNT);

    /* Clear all junction slots */
    for (i = 0; i < JUNCTION_COUNT; i++) {
        g_gameState.chars[charIdx].junctions[i] = 0;
    }

    /* Reload magic slots pointer and get slot type table */
    magicSlots = g_gameState.chars[charIdx].magic;
    slotTable = g_autoJunctionPriority[tableIdx];
    rawFlags = g_junctionChars[charIdx].availFlags;
    slotTypes = slotTable;

    /* Compute available junction flags */
    availFlags = rawFlags;
    availFlags &= 0x1FFF;
    if (rawFlags & 0x6800) {
        availFlags |= 0x800;
    }
    if (g_junctionChars[charIdx].availFlags & 0x19000) {
        availFlags |= 0x1000;
    }

    /* Iterate slot types and auto-junction each */
    while (1) {
        slotType = *slotTypes++;
        if (slotType == 0xFF) {
            break;
        }
        bit = 1 << slotType;
        if (availFlags & bit) {
            flagMask = renderJunctionSlots(charIdx, (s32)magicSlots, slotType, flagMask);
        }
    }
}

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
        ctx->statInfo[1] = 3;
    } else {
        ctx->statInfo[1] = 2;
    }
}

/**
 * @brief Stash a character's junction slots into the backup buffer.
 *
 * Saves all junction slot assignments (HP, Str, Vit, ... DefStatus)
 * to g_junctionBackup so they can be restored later (e.g. after a
 * junction preview or swap operation).
 *
 * @param charIdx Character index (0-7).
 */
void stashCharacterJunctions(s32 charIdx) {
    s32 i;
    for (i = 0; i < JUNCTION_SLOT_SIZE; i++) {
        g_junctionBackup[i] = g_gameState.chars[charIdx].junctions[i];
    }
}

/**
 * @brief Restore character junction slots from backup.
 *
 * Copies 20 bytes from g_junctionBackup back into the character's
 * junction slots. Reverses stashCharacterJunctions.
 *
 * @param charIdx Character index (0-7).
 */
void restoreCharacterJunctions(s32 charIdx) {
    s32 i;
    for (i = 0; i < JUNCTION_SLOT_SIZE; i++) {
        g_gameState.chars[charIdx].junctions[i] = g_junctionBackup[i];
    }
}

/**
 * @brief Assign a command or ability to a character's junction slot.
 *
 * Looks up the actual command/ability ID from a lookup table (D_801EEF10
 * for commands, D_801EEF40 for abilities), clears any duplicate assignments
 * in the same slot group, and writes the new value. Plays a success or
 * error sound depending on whether a change was made.
 *
 * @param charIdx Character index (0-7).
 * @param slotIndex Slot index (0-2 = commands, 3-6 = abilities).
 * @param mode Lookup table selector (1 = commands, 2 = abilities).
 * @param selection Menu selection index into the lookup table.
 * @param doWrite If nonzero, write the new value; if zero, just clear duplicates.
 */
void assignJunctionSlot(s32 charIdx, s32 slotIndex, s32 mode, s32 selection, s32 doWrite) {
    s32 id;
    u8 currentVal;
    s32 changed = 1;
    s32 i;

    switch (mode) {
        case 1:
            id = D_801EEF10[selection * 2];
            break;
        case 2:
            id = D_801EEF40[selection * 2];
            break;
        case 0:
        default:
            return;
    }

    if (slotIndex < 3) {
        currentVal = g_gameState.chars[charIdx].commands[slotIndex];
        {
            s32 word = g_assignedAbilities[id / 32]; /* Scheduling: dead but affects codegen */
            s32 mask = 1 << (id & 0x1F);
            if (g_assignedAbilities[id / 32] & mask) {
                for (i = 0; i < 4; i++) {
                    if (g_gameState.chars[charIdx].commands[i] == id) {
                        g_gameState.chars[charIdx].commands[i] = 0;
                    }
                }
            }
        }

        if (doWrite != 0) {
            if (currentVal != id) {
                g_gameState.chars[charIdx].commands[slotIndex] = id;
            }
        } else if (currentVal == 0) {
            changed = 0;
        }
    } else {
        slotIndex -= 3;
        currentVal = g_gameState.chars[charIdx].abilities[slotIndex];
        {
            s32 word = g_assignedAbilities[id / 32]; /* Scheduling: dead but affects codegen */
            s32 mask = 1 << (id & 0x1F);
            if (g_assignedAbilities[id / 32] & mask) {
                for (i = 0; i < 4; i++) {
                    if (g_gameState.chars[charIdx].abilities[i] == id) {
                        g_gameState.chars[charIdx].abilities[i] = 0;
                    }
                }
            }
        }

        if (doWrite != 0) {
            if (currentVal != id) {
                g_gameState.chars[charIdx].abilities[slotIndex] = id;
            }
        } else if (currentVal == 0) {
            changed = 0;
        }
    }

    if (changed != 0) {
        playSoundEffect(0x11);
    } else {
        sendSpuCommand(0x5);
    }
}


/**
 * @brief Build bitmask of currently assigned commands and abilities.
 *
 * Clears g_assignedAbilities (4 words), then sets a bit for each nonzero
 * command (3 slots) and ability (variable count from g_junctionChars).
 *
 * @param charIdx Character index (0-7).
 */
void buildAssignedAbilities(s32 charIdx) {
    s32 i;

    for (i = 3; i >= 0; i--) {
        g_assignedAbilities[i] = 0;
    }

    for (i = 0; i < 3; i++) {
        s32 cmd = g_gameState.chars[charIdx].commands[i];
        if (cmd != 0) {
            g_assignedAbilities[cmd / 32] |= (1 << (cmd & 0x1F));
        }
    }

    i = 0;
    if (g_junctionChars[charIdx].unk0A != 0) {
        do {
            s32 abl = g_gameState.chars[charIdx].abilities[i];
            if (abl != 0) {
                g_assignedAbilities[abl / 32] |= (1 << (abl & 0x1F));
            }
        } while (++i < g_junctionChars[charIdx].unk0A);
    }
}

/**
 * @brief Build bitmask of available abilities from junctioned GFs.
 *
 * Clears g_availableAbilities (4 words), then for each GF junctioned to this
 * character, ORs the GF's completed abilities bitmask into g_availableAbilities.
 *
 * @param charIdx Character index (0-7).
 */
void buildAvailableAbilities(s32 charIdx) {
    s32 junctedGfs = g_junctionChars[charIdx].junctedGfs;
    s32 gfIdx;
    s32 one = 1;

    for (gfIdx = 3; gfIdx >= 0; gfIdx--) {
        g_availableAbilities[gfIdx] = 0;
    }

    gfIdx = 0;
    one = 1;
    for (; gfIdx < GF_COUNT; gfIdx++) {
        if (junctedGfs & (one << gfIdx)) {
            s32 j;
            for (j = 0; j < 4; j++) {
                g_availableAbilities[j] |= g_gameState.gfs[gfIdx].completeAbilities[j];
            }
        }
    }
}

/**
 * @brief Build command and ability lookup tables from available abilities.
 *
 * Scans the ability availability bitfield (g_availableAbilities) for commands
 * (IDs 0x14-0x26) and abilities (IDs 0x27-0x52). For each available
 * entry, stores the ID and type (from getAbilityCategory) into D_801EEF10
 * (commands) or D_801EEF40 (abilities), then updates the counts.
 *
 * @param charIdx Character index (0-7).
 */
void buildAbilityTables(s32 charIdx) {
    s32 id;
    s32 count;
    s32 one;
    s32 *availBits;
    u8 *table;

    buildAvailableAbilities(charIdx);

    count = 0;
    availBits = g_availableAbilities;
    /* Regalloc: boost availBits priority so it gets s3 (lower than one's s4) */
    availBits++;
    availBits--;
    id = 0x14;
    one = 1;
    table = D_801EEF10;
    for (; id < 0x27; id++) {
        if (availBits[id / 32] & (one << (id & 0x1F))) {
            table[0] = id;
            table[1] = getAbilityCategory(id);
            table += 2;
            count++;
        }
    }
    D_801EEF38 = count;

    count = 0;
    id = 0x27;
    one = 1;
    table = D_801EEF40;
    for (; id < 0x53; id++) {
        if (availBits[id / 32] & (one << (id & 0x1F))) {
            table[0] = id;
            table[1] = getAbilityCategory(id);
            table += 2;
            count++;
        }
    }
    D_801EEF9A = count;

    buildAssignedAbilities(charIdx);
}

/**
 * @brief Render magic list junction entry.
 * @param renderCtx Render context.
 * @param row Row index (wrapped to JNC_ROWS_PER_PAGE).
 */
void renderMagicListEntry(s32 renderCtx, s32 row) {
    func_801F0A34(renderCtx, 0, JNC_W_MAGIC_LIST, (row % JNC_ROWS_PER_PAGE) * JNC_ROW_HEIGHT + JNC_Y_MAGIC_LIST);
}

/**
 * @brief Render stat junction entry in two-column layout.
 *
 * Left column (rows 0-2) or right column (rows 3-5).
 *
 * @param renderCtx Render context.
 * @param row Row index (0-5).
 * @param widthOffset Width offset to add.
 */
void renderStatColumnEntry(s32 renderCtx, s32 row, s32 widthOffset) {
    s32 width;
    s32 y;
    if (row < JNC_LEFT_COL_ROWS) {
        width = JNC_W_LEFT_COL;
        y = (row + 1) * JNC_ROW_HEIGHT + JNC_Y_LEFT_COL;
    } else {
        row -= JNC_LEFT_COL_ROWS;
        width = JNC_W_RIGHT_COL;
        y = row * JNC_ROW_HEIGHT + JNC_Y_RIGHT_COL;
    }
    func_801F0A34(renderCtx, 0, width + widthOffset, y);
}

/**
 * @brief Render stat junction list entry.
 *
 * @param renderCtx Render context.
 * @param slotIdx Slot index (wrapped to JNC_STAT_ROWS).
 */
void renderStatListEntry(s32 renderCtx, s32 slotIdx) {
    slotIdx %= JNC_STAT_ROWS;
    func_801F0A34(renderCtx, 0, JNC_W_STAT_LIST, slotIdx * JNC_ROW_HEIGHT + JNC_Y_STAT_LIST);
}

/**
 * @brief Look up junction ability availability mask for a given slot.
 *
 * Indexes into D_801EEAC0 to get an ability type (0-18), then checks
 * corresponding bit(s) in the junction flags word at g_junctionChars.
 * Cases 0-8 test individual bits via (1 << type), case 9 tests 0x200,
 * case 10 tests 0x400, cases 11-14 test 0x6800, cases 15-18 test 0x19000.
 *
 * @param charIdx Character index.
 * @param slotOffset Ability slot offset into D_801EEAC0.
 * @return Masked flags value, or 0 if type >= 19.
 */
s32 getJunctionSlotFlags(s32 charIdx, s32 slotOffset) {
    u32 flags;

    flags = g_junctionChars[charIdx].availFlags;
    charIdx = D_801EEAC0[slotOffset];
    switch (charIdx) {
        case 10:
            return flags & 0x400;
        case 15:
        case 16:
        case 17:
        case 18:
            return flags & 0x19000;
        case 9:
            return flags & 0x200;
        case 11:
        case 12:
        case 13:
        case 14:
            return flags & 0x6800;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            return flags & (1 << charIdx);
        default:
            return 0;
    }
}

/**
 * @brief Render ability junction entry.
 *
 * Uses wide width for the last page (page 3).
 *
 * @param renderCtx Render context.
 * @param index Linear row index.
 */
void renderAbilityEntry(s32 renderCtx, s32 index) {
    s32 width = JNC_W_ABILITY;
    s32 page = index / JNC_ABILITY_PAGES;
    s32 row = index % JNC_ABILITY_PAGES;
    s32 y = row * JNC_ROW_HEIGHT + JNC_Y_ABILITY;

    if (page < 0) {
    } else if (page < JNC_LEFT_COL_ROWS) {
    } else if (page == JNC_LEFT_COL_ROWS) {
        width = JNC_W_ABILITY_WIDE;
    }
    func_801F0A34(renderCtx, 0, width, y);
}

/**
 * @brief Get junction slot count based on slot type and character data.
 *
 * For slot type 0: reads g_junctionChars[charIdx].abilityCount[0] and returns val+1 if
 * nonzero, else 2. For slot type 1: reads g_junctionChars[charIdx].abilityCount[1] and
 * returns val+1 unless val+1 equals slotType (1), in which case returns 2.
 * Default returns 5.
 *
 * @param charIdx Character index.
 * @param slotType Slot type (0, 1, or other).
 * @return Slot count value.
 */
s32 getJunctionSlotCount(s32 charIdx, s32 slotType) {
    s32 result;

    switch (slotType) {
    case 0: {
        s32 count = g_junctionChars[charIdx].abilityCount[0];
        result = count + 1;
        if (count == 0) {
            result = 2;
        }
        break;
    }
    case 1: {
        s32 count = g_junctionChars[charIdx].abilityCount[1];
        result = count + 1;
        if (result == slotType) {
            result = 2;
        }
        break;
    }
    default:
        result = 5;
        break;
    }
    return result;
}

/**
 * @brief Compute magic availability bitmask for a junction slot type.
 *
 * Loops through 32 magic slots, checking each against the given slot
 * type via func_801F776C. Returns a bitmask of which slots have
 * compatible magic.
 *
 * @param charIdx Character index (0-7).
 * @param slotOffset Offset into D_801EEAC0 junction type table.
 * @return 32-bit bitmask of available magic slots.
 */
s32 buildMagicAvailMask(s32 charIdx, s32 slotOffset) {
    u8 *magic = (u8 *)g_gameState.chars[charIdx].magic;
    s32 result = 0;
    u32 type = D_801EEAC0[slotOffset];
    s32 i = result;
    s32 one = 1;

    do {
        u32 magicId = *magic++;
        u8 qty = *magic++;
        if (qty != 0 && magicId != 0) {
            if (func_801F776C(magicId, type) != 0) {
                result |= (one << i);
            }
        }
    } while (++i < MAGIC_SLOT_COUNT);

    return result;
}

/**
 * @brief Compute negative scroll offset from page index.
 *
 * Divides index by 5 to get the page number, clamps to a maximum of 2,
 * then returns the negative offset as -(page * 160).
 *
 * @param index Linear item index.
 * @return Negative pixel offset for scrolling (0, -160, or -320).
 */
s32 getAbilityScrollOffset(s32 index) {
    s32 page = index / 5;
    if (page >= 3) {
        page = 2;
    }
    return -(page * 160);
}

/** @brief Draw inner panel with section id 0xB and clear flag. */
s32 renderInnerPanel(s32 pos) {
    return func_801F08D4(1, 0xB, pos, 0);
}

/**
 * @brief Draw inner panel with section id 0xB and set flag.
 * @param pos Panel position parameter
 * @return Result of func_801F08D4
 */
s32 renderInnerPanelAlt(s32 pos) {
    return func_801F08D4(1, 0xB, pos, 1);
}

/**
 * @brief Clear unlearned or out-of-range abilities from junction slots.
 *
 * Iterates through 4 ability slots at g_gameState.chars[charIdx].commands.
 * For each nonzero ability ID, checks if the ability is available
 * (learned via ability bitmask) and in range [0x14, 0x27). If not
 * available or out of range, clears the slot to 0.
 *
 * @param charIdx Character index.
 */
/**
 * @brief Validate command slots against available GF abilities.
 *
 * Checks each of the 4 command slots. If a command is nonzero but its
 * ability bit isn't set in g_availableAbilities, or the command ID is outside
 * the valid range (20-38), clears the slot to 0.
 *
 * @param charIdx Character index (0-7).
 */

/**
 * @brief Validate command slots against available GF abilities.
 *
 * Checks each of the 4 command slots. If a command is nonzero but not
 * available in g_availableAbilities, or outside the valid command range [20, 39),
 * clears the slot.
 *
 * @param charIdx Character index (0-7).
 */
void validateCommandSlots(s32 charIdx) {
    s32 val;
    s32 i;

    for (i = 0; i < 4; i++) {
        s32 cmd = g_gameState.chars[charIdx].commands[i];
        val = cmd;
        if (val != 0) {
            s32 word = g_availableAbilities[val / 32];
            s32 shift = val & 0x1F;
            s32 mask = 1 << shift;
            cmd = word;
            if (!(cmd & mask) || val < 20 || val >= 39) {
                g_gameState.chars[charIdx].commands[i] = 0;
            }
        }
    }
}

/**
 * @brief Validate and compact ability slots.
 *
 * Checks each of the 4 ability slots. If an ability is nonzero but not
 * available in g_availableAbilities, or outside the valid ability range
 * [39, 83), clears it. Then compacts remaining abilities to the front
 * and zeros out any excess slots beyond the character's max ability count.
 *
 * @param charIdx Character index (0-7).
 */
void validateAbilitySlots(s32 charIdx) {
    s32 i;
    u8 buf[4];
    s32 maxSlots;

    for (i = 0; i < 4; i++) {
        s32 cmd = g_gameState.chars[charIdx].abilities[i];
        s32 val = cmd;
        if (val != 0) {
            s32 word = g_availableAbilities[val / 32];
            s32 shift = val & 0x1F;
            s32 mask = 1 << shift;
            cmd = word;
            if (!(cmd & mask) || val < 39 || val >= 83) {
                g_gameState.chars[charIdx].abilities[i] = 0;
            }
        }
    }

    maxSlots = g_junctionChars[charIdx].unk0A;
    {
        u8 *src = g_gameState.chars[charIdx].abilities;
        u8 *dst = buf;

        for (i = 3; i >= 0; i--) {
            *dst = 0;
            dst++;
        }

        dst = buf;
        for (i = 0; i < 4; i++) {
            u8 abl = *src;
            src++;
            if (abl != 0) {
                *dst = abl;
                dst++;
            }
        }

        dst = buf;
        for (i = 0; i < 4; i++) {
            g_gameState.chars[charIdx].abilities[i] = *dst;
            dst++;
        }
    }

    while (maxSlots < 4) {
        g_gameState.chars[charIdx].abilities[maxSlots] = 0;
        maxSlots++;
    }
}

/**
 * @brief Full junction menu refresh sequence.
 *
 * Calls buildAssignedAbilities, buildAvailableAbilities, validateCommandSlots,
 * and validateAbilitySlots in sequence with the party context.
 *
 * @param charIdx Character index (0-7).
 */
void refreshJunctionState(s32 charIdx) {
    buildAssignedAbilities(charIdx);
    buildAvailableAbilities(charIdx);
    validateCommandSlots(charIdx);
    validateAbilitySlots(charIdx);
}

/**
 * @brief Copy ability value from junction table to character data and update.
 *
 * Copies cached HP from g_junctionChars to g_gameState.chars, then calls func_801F5190 to update.
 *
 * @param charIdx Character index (0-7).
 */
void syncCharacterHp(s32 charIdx) {
    u16 hp = g_junctionChars[charIdx].currentHp;
    g_gameState.chars[charIdx].currentHp = hp;
    func_801F5190();
}

/**
 * @brief Preview a junction change and snapshot the resulting battle stats.
 *
 * Temporarily applies the junction menu's GF selection to the character,
 * recalculates stats, copies the result to the preview buffer, then
 * restores the original junction state.
 *
 * @param charIdx Character index (0-7).
 */
void snapshotJunctionPreview(s32 charIdx) {
    u16 saved = g_gameState.chars[charIdx].junctedGfs;

    g_gameState.chars[charIdx].junctedGfs = g_junctionChars[charIdx].junctedGfs;
    buildAvailableAbilities(charIdx);
    syncCharacterHp(charIdx);
    btlMemcpyForward(&g_battleChars, &g_junctionPreview, sizeof(BattleCharData));
    g_gameState.chars[charIdx].junctedGfs = saved;
    func_801F1B4C(charIdx);
}

/**
 * @brief Copy ability value from junction table to character data and refresh.
 *
 * Copies juncted GFs from g_junctionChars to g_gameState.chars and refreshes display.
 *
 * @param charIdx Character index (0-7).
 */
void applyJunctedGfs(s32 charIdx) {
    g_gameState.chars[charIdx].junctedGfs = g_junctionChars[charIdx].junctedGfs;
    refreshJunctionState(charIdx);
}

/**
 * @brief Copy ability halfword from character data to junction table.
 *
 * Copies juncted GFs from g_gameState.chars to g_junctionChars.
 *
 * @param charIdx Character index (0-7).
 */
void stashJunctedGfs(s32 charIdx) {
    g_junctionChars[charIdx].junctedGfs = g_gameState.chars[charIdx].junctedGfs;
}

/**
 * @brief Store a halfword into junction table entry.
 *
 * Sets the cached HP value in g_junctionChars for a character.
 *
 * @param charIdx Character index (0-7).
 * @param hp HP value to store.
 */
void setJunctionHp(s32 charIdx, s32 hp) {
    g_junctionChars[charIdx].currentHp = hp;
}

/**
 * @brief Copy character ability data to junction table slot.
 *
 * Copies 4 bytes from character data g_gameState.chars[charIdx].commands and abilities into
 * g_junctionChars[charIdx].commandsBackup[subSlot] and abilitiesBackup[subSlot].
 *
 * @param charIdx Character index (0-7).
 * @param subSlot Junction sub-slot (0 or 1).
 */
void saveCommandAbilityBackup(s32 charIdx, s32 subSlot) {
    s32 i;
    for (i = 0; i < 4; i++) {
        g_junctionChars[charIdx].commandsBackup[subSlot][i] =
            g_gameState.chars[charIdx].commands[i];
        g_junctionChars[charIdx].abilitiesBackup[subSlot][i] =
            g_gameState.chars[charIdx].abilities[i];
    }
}

/**
 * @brief Copy junction slot data back to character ability data.
 *
 * Copies 4 bytes from junction table g_junctionChars[charIdx].commandsBackup[subSlot] and abilitiesBackup[subSlot]
 * into g_gameState.chars[charIdx].commands and abilities.
 *
 * @param charIdx Character index (0-7).
 * @param subSlot Junction sub-slot (0 or 1).
 */
void restoreCommandAbilityBackup(s32 charIdx, s32 subSlot) {
    s32 i;
    for (i = 0; i < 4; i++) {
        g_gameState.chars[charIdx].commands[i] = g_junctionChars[charIdx].commandsBackup[subSlot][i];
        g_gameState.chars[charIdx].abilities[i] = g_junctionChars[charIdx].abilitiesBackup[subSlot][i];
    }
}

/**
 * @brief Initialize junction entry and refresh display.
 *
 * Saves juncted GFs, restores commands/abilities from sub-slot 0,
 * then restores junction slots from backup.
 *
 * @param charIdx Character index (0-7).
 */
void revertJunctionState(s32 charIdx) {
    stashJunctedGfs(charIdx);
    restoreCommandAbilityBackup(charIdx, 0);
    restoreCharacterJunctions(charIdx);
}

/**
 * @brief Reset junction slots and copy ability value from character data.
 *
 * Calls saveCommandAbilityBackup twice to save both junction slots, then reads
 * the ability value from g_gameState.chars[charIdx].currentHp and stores it
 * to the junction table via setJunctionHp. Finally refreshes display
 * via stashCharacterJunctions.
 *
 * @param charIdx Character index (0-7).
 */
void initJunctionBackups(s32 charIdx) {
    saveCommandAbilityBackup(charIdx, 0);
    saveCommandAbilityBackup(charIdx, 1);
    setJunctionHp(charIdx, g_gameState.chars[charIdx].currentHp);
    stashCharacterJunctions(charIdx);
}

/**
 * @brief Rebuild junction flags and stat limits from GF data.
 *
 * Iterates through 16 GFs, checking each GF bit in g_junctionChars[charIdx].junctedGfs.
 * For each active GF, ORs its flag word from g_junctionGfTable[gf] into the
 * combined flags, and updates the maximum stat byte indices from
 * g_junctionGfTable[gf] fields. Stores the result back into g_junctionChars.
 *
 * @param charIdx Character index.
 */
void rebuildJunctionFlags(s32 charIdx) {
    s32 flags = 0;
    u16 junctedGfs = g_junctionChars[charIdx].junctedGfs;
    u8 *maxAblPtr;
    s32 cmdSlots = flags;
    s32 ablSlots = cmdSlots;
    s32 maxAbl = 2;
    s32 i = cmdSlots;
    s32 one = 1;
    JunctionGfEntry *gf = g_junctionGfTable;

    do {
        u16 bit = one << i;
        maxAblPtr = &gf->maxAbilitySlots;
        if (junctedGfs & bit) {
            u8 cs = gf->cmdSlotCount;
            flags |= gf->abilityFlags;
            if (cmdSlots < cs)
                cmdSlots = cs;
            if (ablSlots < gf->ablSlotCount)
                ablSlots = gf->ablSlotCount;
            if (maxAbl < *maxAblPtr)
                maxAbl = gf->maxAbilitySlots;
        }
        i++;
        if ((gf->maxAbilitySlots && *maxAblPtr) && *maxAblPtr) {}
        gf++;
    } while (i < 16);

    g_junctionChars[charIdx].availFlags = flags;
    g_junctionChars[charIdx].abilityCount[0] = cmdSlots;
    g_junctionChars[charIdx].abilityCount[1] = ablSlots;
    g_junctionChars[charIdx].unk0A = maxAbl;
}

/**
 * @brief Initialize junction character entries from game state.
 *
 * Iterates over 8 characters. For each, zeroes the JunctionMenuEntry
 * fields, counts non-empty magic slot pairs (into gfCompat), and if
 * the character's bit is set in gfBitmask, copies the HP and juncted
 * GFs values from g_gameState.
 *
 * @param gfBitmask Bitmask indicating which characters are active.
 */
void initJunctionChars(s32 mask) {
    s32 i, j;
    u8 *pairs;
    u8 a, b;
    s32 bit;

    for (i = 0; i < 8; i++) {
        g_junctionChars[i].availFlags = 0;
        g_junctionChars[i].currentHp = 0;
        g_junctionChars[i].junctedGfs = 0;
        g_junctionChars[i].abilityCount[0] = 0;
        g_junctionChars[i].abilityCount[1] = 0;
        g_junctionChars[i].unk0A = 0;
        g_junctionChars[i].gfCompat = 0;

        pairs = (u8 *)g_gameState.chars[i].magic;
        for (j = 0; j < 32; j++) {
            a = *pairs++;
            b = *pairs++;
            if (a != 0 && b != 0) {
                g_junctionChars[i].gfCompat++;
            }
        }

        bit = 1 << i;
        if (mask & bit) {
            g_junctionChars[i].currentHp = g_gameState.chars[i].currentHp;
            g_junctionChars[i].junctedGfs = g_gameState.chars[i].junctedGfs;
        }
    }
}

/**
 * @brief Render stat effectiveness indicator bar.
 *
 * Computes x position from a stat scaling lookup table (D_801FA3C8)
 * and y position from the current stat slot, then calls func_801F0A34
 * to draw the indicator at that position.
 *
 * @param renderCtx Render context parameter.
 * @param ctx Junction menu context.
 */
void renderStatEffectBar(s32 renderCtx, JunctionMenuCtx *ctx) {
    s8 slot = ctx->statSlot;
    s32 tableIdx;

    if (slot >= 0) {
        s32 scaled = ctx->statScale;
        s32 idx;
        short row;

        scaled = 0x1000 - scaled;
        idx = scaled;
        if (scaled < 0) {
            idx = scaled + 63;
        }
        idx >>= 6;
        tableIdx = idx;
        scaled = D_801FA3C8[tableIdx];
        scaled = (scaled * 150) / 4096;

        row = slot - ((slot / 4) * 4);
        row = row * 13;

        /* Regalloc: (tableIdx = 0xD4) keeps tableIdx alive, preventing the
           compiler from eliminating the idx→tableIdx copy above. */
        func_801F0A34(renderCtx, 0, scaled + (tableIdx = 0xD4), row + 0x3F);
    }
}

/**
 * @brief Render stat delta bar for a junction change.
 *
 * Decodes stat names into two stack buffers, computes the stat difference
 * between current and new values, and renders as a progress bar.
 *
 * @param ctx Pointer to JunctionMenuCtx.
 * @param renderCtx Render context parameter.
 * @param column Column index for rendering.
 */
/**
 * @brief Render stat delta bar for a junction change.
 *
 * Decodes stat names from D_801EEB1C and ability data from ctx->dataPtr
 * into two stack buffers, computes the stat difference between adjacent
 * entries, and renders the bar via func_801F0A34.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context parameter.
 * @param column Column index for rendering.
 */
void renderStatDeltaEntry(JunctionMenuCtx *ctx, s32 renderCtx, s32 column) {
    s16 buffer1[36];
    s16 buffer2[36];

    if (ctx->dataPtr == 0) {
        return;
    }

    func_801F5984(D_801EEB1C, buffer1, 11);
    func_801F5984((u8 *)ctx->dataPtr, buffer2, 11);

    {
        s32 statIdx = ctx->unk4E;
        s32 width;

        width = buffer1[statIdx + 1] - buffer1[statIdx];
        width += 86 + buffer2[column];

        func_801F0A34(renderCtx, 0, width, 13);
    }
}

/**
 * @brief Render a stat value bar in the junction menu.
 *
 * Decodes stat names from D_801EEB1C, looks up the value for the
 * given column, scales it by ctx->unk40, and renders the bar
 * via func_801F0A78.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context parameter.
 * @param column Column index into the decoded stat buffer.
 */
void renderStatValueBar(JunctionMenuCtx *ctx, s32 renderCtx, s32 column) {
    s16 statBuf[36];
    s32 scale;
    s32 product;

    scale = ctx->unk40;
    scale = 0x1000 - scale;
    func_801F5984(D_801EEB1C, statBuf, 11);

    product = (scale * statBuf[column]) / 4096;

    func_801F0A78(renderCtx, 0, ctx->unk3A, product + 50, 13);
}

/**
 * @brief Toggle a GF junction for a character.
 *
 * Checks if GF bit (1 << a1) is already set in g_junctionChars[charIdx].junctedGfs.
 * If set, returns 0. If not set, ORs the bit in, records the character
 * index at g_junctionGfTable[gfIdx].charIdx, rebuilds ability table, updates display.
 *
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (0-15).
 * @return 1 if junction was toggled, 0 if already set.
 */
s32 junctionGfToChar(s32 charIdx, s32 gfIdx) {
    s32 one = 1;
    u16 flags = g_junctionChars[charIdx].junctedGfs;
    s32 mask = one << gfIdx;

    if (flags & mask) {
        return 0;
    }
    g_junctionChars[charIdx].junctedGfs = flags | mask;
    g_junctionGfTable[gfIdx].charIdx = charIdx;
    rebuildJunctionFlags(charIdx);
    restoreCommandAbilityBackup(charIdx, 1);
    refreshJunctionState(charIdx);
    snapshotJunctionPreview(charIdx);
    return 1;
}

/**
 * @brief Compact defense-element junction slots, removing gaps.
 *
 * Scans the 4 defense-element junction slots (JUNCTION_DEF_ELEM_0..3)
 * for the given character, collecting non-zero entries into a temp buffer.
 * If the last occupied slot index exceeds the allowed max count (from
 * g_junctionChars abilityCount[1]), clears all 4 slots and copies back
 * only up to the max count, compacting them to the front.
 *
 * @param charIdx Character index (0-7).
 */
void compactCommandSlots(s32 charIdx) {
    u8 tmp[4];
    s32 maxCount;
    s32 i;
    s32 writeIdx;
    s32 lastSrcIdx;
    u8 *p;

    maxCount = g_junctionChars[charIdx].abilityCount[1];
    writeIdx = 0;
    lastSrcIdx = writeIdx;

    i = 3;
    p = &tmp[3];
    do {
        *p-- = 0;
    } while (--i >= 0);

    for (i = 0; i < 4; i++) {
        u8 val = g_gameState.chars[charIdx].junctions[JUNCTION_DEF_ELEM_0 + i];
        if (val != 0) {
            tmp[writeIdx] = val;
            writeIdx++;
            lastSrcIdx = i;
        }
    }

    if (lastSrcIdx < maxCount) {
        return;
    }

    i = 3;
    {
        s32 base = (s32)&g_gameState;
        u8 *q = (u8 *)(base + charIdx * 152 + 3);
        do {
            *(u8 *)((s32)q + 0x4F7) = 0;
            q--;
        } while (--i >= 0);
    }

    if (maxCount == 0) {
        return;
    }

    i = 0;
    do {
        g_gameState.chars[charIdx].junctions[JUNCTION_DEF_ELEM_0 + i] = tmp[i];
    } while (++i < maxCount);
}

/**
 * @brief Compact defense-status junction slots, removing gaps.
 *
 * Same logic as compactCommandSlots but operates on the 4 defense-status
 * junction slots (JUNCTION_DEF_STATUS_0..3) and uses abilityCount[0]
 * as the max.
 *
 * @param charIdx Character index (0-7).
 */
void compactAbilitySlots(s32 charIdx) {
    u8 tmp[4];
    s32 maxCount;
    s32 i;
    s32 writeIdx;
    s32 lastSrcIdx;
    u8 *p;

    maxCount = g_junctionChars[charIdx].abilityCount[0];
    writeIdx = 0;
    lastSrcIdx = writeIdx;

    i = 3;
    p = &tmp[3];
    do {
        *p-- = 0;
    } while (--i >= 0);

    for (i = 0; i < 4; i++) {
        u8 val = g_gameState.chars[charIdx].junctions[JUNCTION_DEF_STATUS_0 + i];
        if (val != 0) {
            tmp[writeIdx] = val;
            writeIdx++;
            lastSrcIdx = i;
        }
    }

    if (lastSrcIdx < maxCount) {
        return;
    }

    i = 3;
    {
        s32 base = (s32)&g_gameState;
        u8 *q = (u8 *)(base + charIdx * 152 + 3);
        do {
            *(u8 *)((s32)q + 0x4FB) = 0;
            q--;
        } while (--i >= 0);
    }

    if (maxCount == 0) {
        return;
    }

    i = 0;
    do {
        g_gameState.chars[charIdx].junctions[JUNCTION_DEF_STATUS_0 + i] = tmp[i];
    } while (++i < maxCount);
}

/**
 * @brief Remove a GF junction from a character.
 *
 * Clears the GF's bit from junctedGfs, marks the GF entry as unassigned
 * (charIdx = 0xFF), recalculates available junction flags, and zeroes
 * any junction slots that were only provided by the removed GF.
 *
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (0-15).
 * @return 1 if the GF was removed, 0 if not junctioned.
 */
s32 unjunctionGf(s32 charIdx, s32 gfIdx) {
    s32 one = 1;
    u32 oldFlags;
    u32 removed;
    s32 i;

    if (!(g_junctionChars[charIdx].junctedGfs & (one << gfIdx))) {
        return 0;
    }

    g_junctionChars[charIdx].junctedGfs &= ~(one << gfIdx);
    oldFlags = g_junctionChars[charIdx].availFlags;
    g_junctionGfTable[gfIdx].charIdx = 0xFF;
    rebuildJunctionFlags(charIdx);
    snapshotJunctionPreview(charIdx);

    i = 0;
    removed = g_junctionChars[charIdx].availFlags;
    removed = (removed ^ oldFlags) & oldFlags;
    do {
        if (removed & (1 << i)) {
            g_gameState.chars[charIdx].junctions[i] = 0;
        }
    } while (++i < 11);

    compactCommandSlots(charIdx);
    compactAbilitySlots(charIdx);
    func_801F1B4C(charIdx);

    return 1;
}

/**
 * @brief Unjunction a GF and refresh ability tables.
 *
 * Calls unjunctionGf and if it succeeds, recalculates the character's
 * junction and ability state.
 *
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (0-15).
 * @return 1 if GF was removed, 0 if not junctioned.
 */
s32 unjunctionGfAndRefresh(s32 charIdx, s32 gfIdx) {
    s32 result = unjunctionGf(charIdx, gfIdx);

    if (result != 0) {
        snapshotJunctionPreview(charIdx);
        refreshJunctionState(charIdx);
        snapshotJunctionPreview(charIdx);
    }
    return result;
}

/**
 * @brief Preview a junction configuration change for stat calculation.
 *
 * Temporarily modifies the character's junctioned GFs and ability slots
 * to compute preview stats. Saves current junction state to local stack,
 * applies the GF/ability change, calls stat recalculation functions,
 * then restores the original state. Used for what-if stat previews
 * when hovering over changes in the junction menu.
 *
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index to toggle, or -1 to skip.
 * @param slot Junction slot to assign, or -1 to skip.
 * @param abilityId Ability/magic ID to assign to the slot.
 */
void previewJunctionChange(s32 charIdx, s32 gfIdx, s32 slot, s32 abilityId) {
    u8 savedJunctions[JUNCTION_COUNT];
    u16 savedGfs;
    u16 previewGfs;
    s32 availFlags;
    s32 one;
    s32 i;

    savedGfs = g_gameState.chars[charIdx].junctedGfs;

    for (i = 0; i < JUNCTION_COUNT; i++) {
        savedJunctions[i] = g_gameState.chars[charIdx].junctions[i];
    }

    previewGfs = g_junctionChars[charIdx].junctedGfs;

    if (gfIdx >= 0) {
        if (g_junctionGfTable[gfIdx].charIdx == 0xFF) {
            /* GF is unassigned — add it to preview */
            previewGfs |= (1 << gfIdx);
        } else if (g_junctionGfTable[gfIdx].charIdx == charIdx) {
            /* GF is assigned to this character — remove it from preview */
            previewGfs &= ~(1 << gfIdx);
        }
    }

    /* Build combined ability flags from all preview-junctioned GFs */
    availFlags = 0;
    for (i = 0; i < GF_COUNT; i++) {
        if ((previewGfs >> i) & 1) {
            availFlags |= g_junctionGfTable[i].abilityFlags;
        }
    }

    availFlags = func_801F7C20(availFlags);

    /* Clear junction slots that lost their backing GF ability */
    i = 0;
    one = 1;
    for (; i < JUNCTION_COUNT; i++) {
        if (!(availFlags & (one << i))) {
            g_gameState.chars[charIdx].junctions[i] = 0;
        }
    }

    /* Apply the slot change if requested */
    if (slot >= 0) {
        if (func_801F1CE8(charIdx, abilityId)) {
            func_801F78D8(charIdx, abilityId);
        }
        g_gameState.chars[charIdx].junctions[slot] = abilityId;
    }

    /* Write preview GFs and recalculate stats */
    g_gameState.chars[charIdx].junctedGfs = previewGfs;
    syncCharacterHp(charIdx);
    func_801F5190(charIdx);

    /* Restore original junction slots */
    for (i = 0; i < JUNCTION_COUNT; i++) {
        g_gameState.chars[charIdx].junctions[i] = savedJunctions[i];
    }

    /* Restore original GFs */
    g_gameState.chars[charIdx].junctedGfs = savedGfs;
}

/**
 * @brief Look up ability/command name string by type and index.
 *
 * For type 1 (commands): looks up command ID from D_801EEF10, finds
 * the GF ability index in g_gfData, and returns the name via getAbilityEntryDesc.
 * For type 2 (abilities): looks up ability ID from D_801EEF40 and
 * returns the name via getAbilityDesc.
 *
 * @param type Lookup type (0=none, 1=command, 2=ability).
 * @param index Index into the lookup table.
 * @return Name string pointer, or 0 if not found.
 */
s32 getAbilityNamePtr(s32 type, s32 index) {
    s32 result;
    u8 *gfData;
    s32 stride;

    switch (type) {
    case 0:
        result = 0;
        break;
    case 1:
        if (index < D_801EEF38) {
            u8 cmdId = D_801EEF10[index * 2];
            gfData = D_80078E00;
            stride = 8;
            /* g_gfData ability range J: typeField at offset 0x4180 + 5 = 0x4185 */
            result = getAbilityEntryDesc(gfData[(cmdId - 0x14) * stride + 0x4185]);
        } else {
            result = 0;
        }
        break;
    case 2:
        if (index < D_801EEF9A) {
            u8 ablId = D_801EEF40[index * 2];
            result = getAbilityDesc(ablId);
        } else {
            result = 0;
        }
        break;
        result = 0;
    default:
        break;
    }
    return result;
}

/**
 * @brief Calculate junction menu navigation flags.
 *
 * Calls buildAbilityTables to update ability lists, then determines a flag
 * byte based on available abilities (D_801EEF38 + D_801EEF9A counts)
 * and junction table state (g_junctionChars[charIdx].junctedGfs, [+0], [+0xB]).
 *
 * @param charIdx Character index (0-7).
 * @return Navigation flag byte (combination of 0x1, 0x2, 0x4, 0x9).
 */
/**
 * @brief Compute junction menu capability flags for a character.
 *
 * Builds lookup tables via buildAbilityTables, then checks what junction
 * features are available based on command/ability counts, junctioned GFs,
 * available flags, and GF compatibility.
 *
 * @param charIdx Character index (0-7).
 * @return Capability flags: bit 0 = always set, bit 1 = has GFs with abilities,
 *         bit 2 = has GF compatibility, bit 3 = has commands/abilities available.
 */
s32 getJunctionCapabilities(s32 charIdx) {
    char flags;

    buildAbilityTables(charIdx);

    if (D_801EEF38 + D_801EEF9A != 0) {
        flags = 9;
    } else {
        flags = 1;
    }

    if (g_junctionChars[charIdx].junctedGfs != 0) {
        flags |= 2;
        if (g_junctionChars[charIdx].availFlags != 0 &&
            g_junctionChars[charIdx].gfCompat != 0) {
            flags |= 4;
        }
    }

    return flags;
}

/**
 * @brief Initialize GF ability assignment table for a character.
 *
 * First fills D_801EEED0[0..0x38] with 0xFF, then iterates through
 * 32 magic slots from g_gameState.chars[charIdx].magic, storing the pair index
 * into the corresponding slot if both ability bytes are nonzero.
 *
 * @param charIdx Character index (0-7).
 */
void buildMagicLookupTable(s32 charIdx) {
    s32 idx;
    s32 i;
    u8 *magic = (u8 *)g_gameState.chars[charIdx].magic;
    u8 fillVal = 0xFF;
    u8 *table;

    for (i = 0x38; i >= 0; i--) {
        do { idx = i; } while (0);
        D_801EEED0[idx] = fillVal;
    }

    i = 0;
    table = D_801EEED0;
    do {
        u8 magicId = *magic++;
        u8 quantity = *magic++;
        s16 id = magicId;
        if (id != 0 && quantity != 0) {
            table[magicId] = i;
        }
    } while (++i < MAGIC_SLOT_COUNT);
}

/**
 * @brief Main junction menu per-frame update and state machine.
 *
 * Called every frame while the junction menu is active. Reads the current
 * state from ctx->state (offset 0x10) and dispatches to one of 74 cases
 * (0x00–0x49) via a switch/jump table. Handles all junction menu logic:
 * panel animations, button input, GF/ability selection, auto-junction,
 * stat display, and screen transitions.
 *
 * 11,412 bytes — the largest function in menujnc2.
 *
 * @param ctx Junction menu context (JunctionMenuCtx *).
 * @see https://decomp.me/scratch/1glKK
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", junctionMenuUpdate);

/**
 * @brief Build junction ability flags from battle character data.
 *
 * Extracts the base ability value (lower 7 bits) and remaps
 * abilityFlags bits into the junction flag format.
 *
 * @param charData Pointer to battle character data.
 * @return Composed ability flags word.
 */
s32 encodeBattleAbilityFlags(BattleCharData *charData) {
    s32 result = charData->abilityValue & 0x7F;
    s32 flags = charData->abilityFlags;

    if (flags & 0x1) result |= 0x80;
    if (flags & 0x4) result |= 0x100;
    if (flags & 0x8) result |= 0x200;
    if (flags & 0x200) result |= 0x400;
    if (flags & 0x4000) result |= 0x800;
    if (flags & 0x8000) result |= 0x1000;
    return result;
}

/**
 * @brief Render stat table panel A (primary stats with change indicators).
 *
 * Iterates over D_801EEBA8 entries, comparing preview vs current ability
 * flags to determine stat change direction. Renders icon, color bar,
 * formatted value, and label for each stat row.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position (offset +0x88 applied).
 * @param y Base Y position (offset +0x7F applied).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatTableA);

/**
 * @brief Render stat table panel B (secondary stats with change indicators).
 *
 * Same structure as renderStatTableA but uses D_801EEB40 table entries
 * and different Y offset (+0x78).
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatTableB);

/**
 * @brief Render stat table panel C (extended stats with flag comparison).
 *
 * Uses D_801EEC10 table with additional D_801EF1A4 and D_800788E4
 * flag bytes for comparison. Y offset +0x93.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatTableC);

/**
 * @brief Render stat table panel D (combined stats with numeric values).
 *
 * Uses D_801EEC10 table. Renders formatted numeric stat values alongside
 * change indicators. Y offset +0x93.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatTableD);

/**
 * @brief Render stat value grid in two-column layout.
 *
 * Iterates over D_801EEC50 entries (count from D_801EED00), dividing
 * by 11 for row/column positioning. Renders stat name string and
 * optional numeric value for each entry.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position.
 */
void renderStatGrid(s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    s32 i = 0;
    s32 row, rem;
    s32 xPos, yPos;
    s32 xOff;
    s32 gfInfo;
    s32 yOff;
    s32 namePtr;
    s32 ctx = renderCtx + 0x10;
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    u8 *table;

    if (D_801EED00 > 0) {
        table = D_801EEC50;
        do {
            row = i / 11;
            rem = i - row * 11;
            xOff = row * 155 + 13;
            yOff = rem * 13 + 11;
            xPos = x + xOff;
            yPos = y + yOff;
            cursorY = func_8002FF34(ctx, cursorY, table[3] + 0xD8, xPos, yPos - 2, g_menuColor);
            xPos += 14;
            namePtr = (s32)getAbilityName(table[0]);
            gfInfo = 7;
            table += 8;
            i++;
            cursorY = func_801F0FEC(ctx, cursorY, xPos, yPos, namePtr, gfInfo);
        } while (i < D_801EED00);
    }

    cfg->iconType = 0x5E;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->w = 0x150;
    cfg->y = y;
    cfg->h = 0xA0;
    func_801EF9AC(ctx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Render stat delta bar showing before/after comparison.
 *
 * Reads D_801EEB1C stat boundary data, renders the stat value bar
 * with optional positive/negative delta indicator based on the
 * junction preview state.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position (on stack).
 */
s32 renderStatDeltaBar(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    s16 statBuf[36];
    MenuDisplayConfig *cfg;
    s32 result;
    s32 barX;
    s32 animScale;
    s32 h;

    if (ctx->dataPtr == 0) {
        return cursorY;
    }

    barX = ctx->unk40;
    if (barX == 0) {
        return cursorY;
    }

    animScale = barX;
    func_801F5984(D_801EEB1C, statBuf, 11);

    {
        s8 statIdx = ctx->unk4E;
        s32 delta = statBuf[statIdx + 1] - statBuf[statIdx];
        s32 remainder = 0x1000 - animScale;
        s32 interp;
        h = animScale;
        cfg = &g_menuDisplayCfg;
        interp = ((61 * (remainder * 4)) + (delta * h)) / 4096;
        barX = x + interp;
        barX = barX + 62;
    }

    {
        s32 barY = y + 6;
        result = func_801EF8D8(renderCtx, cursorY);
        cursorY = barY;
    }

    result = func_801F5A38(renderCtx, result, barX, cursorY,
                           11, ctx->dataPtr, ctx->statInfo[ctx->unk4E]);

    cfg->x = x + 2;
    h = (cursorY = 18);
    cfg->y = y;
    cfg->w = 240;
    cfg->h = h;
    result = func_801EF800(renderCtx, result, cfg);

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 244;
    cfg->h = h;
    return func_801EF9AC(renderCtx, result, 0x1000, g_menuColor);
}

/**
 * @brief Render extended stat delta bar with negative/positive delta computation.
 *
 * Extended version of renderStatDeltaBar with additional scaling math
 * for computing stat changes from junction previews.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatDeltaBarExt);

/**
 * @brief Rendering callback for individual magic list items.
 *
 * Loads an item pointer from g_menuDisplayCfg.dataPtr indexed by itemIdx,
 * decodes the name via decodeMessage, and renders the string at the
 * computed screen position using func_801F0FEC.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param itemIdx Index into the item data pointer array.
 * @param columnIdx Column index (unused).
 * @param xOffset X offset from stack argument.
 * @return Updated cursor Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderMagicItemCallback);

/**
 * @brief Set up menu display config and render a junction panel with callback.
 *
 * Configures g_menuDisplayCfg with panel dimensions, icon type 0x55,
 * scroll offset from ctx, and pointer to item data. Clears scroll
 * offset if ctx->unk42 is 4 or 6. Then calls func_801EFBB4 to render
 * the panel using renderMagicItemCallback as the item rendering callback.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context parameter.
 * @param callbackParam Parameter passed through to rendering callback.
 * @param x Panel x position.
 * @param y Panel y position.
 */
void setupMagicListPanel(JunctionMenuCtx *ctx, s32 renderCtx, s32 callbackParam, s16 x, s16 y) {
    g_menuDisplayCfg.iconType = 0x55;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.w = 0x144;
    g_menuDisplayCfg.h = 0x1A;
    g_menuDisplayCfg.columnCount = 1;
    g_menuDisplayCfg.pageStart = 0;
    g_menuDisplayCfg.pageEnd = 1;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.scrollOffset = ctx->unk34;
    g_menuDisplayCfg.dataPtr = (s32)&ctx->itemData;

    if (ctx->unk42 == 4) {
        g_menuDisplayCfg.scrollOffset = 0;
    }
    if (ctx->unk42 == 6) {
        /* Regalloc: callbackParam++/-- forces renderCtx→t1 instead of
           keeping it in a1, matching original register assignment. */
        callbackParam++;
        callbackParam--;
        g_menuDisplayCfg.scrollOffset = 0;
    }

    func_801EFBB4(renderCtx, callbackParam, renderMagicItemCallback);
}

/**
 * @brief Render magic junction grid for all 16 GFs.
 *
 * Iterates over 16 GF slots. For each GF junctioned to the current
 * character, computes a 2-column grid position, checks GF health via
 * func_801F2240, looks up the magic name, and renders it with the
 * appropriate color. After the loop, sets up g_menuDisplayCfg and
 * calls func_801EF9AC for the panel border.
 *
 * @param ctx Junction menu context (charIdx at +0x43).
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param xBase Base X position.
 * @param yBase Base Y position.
 */
void renderGfMagicGrid(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 xBase, s32 yBase) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 i = 0;
    s32 color;
    s32 row, col;
    s32 x, y;
    s32 xOff, yOff;
    s32 namePtr;
    s32 junctedGfs;
    u16 numGfs;
    s32 mask;
    s32 xPos;

    junctedGfs = g_junctionChars[ctx->charIdx].junctedGfs;
    numGfs = 16;

    do {
        mask = 1 << i;
        if (junctedGfs & mask) {
            row = i / 2;
            col = i % 2;
            xOff = col * 85 + 6;
            xPos = xBase + xOff;
            yOff = row * 11 + 6;
            y = yBase + yOff;
            x = xPos;

            if (func_801F2240(i) & 1) {
                color = 1;
            } else {
                color = 7;
            }

            namePtr = getMagicNamePtr(i + 0x40);
            cursorY = func_8002E8DC(renderCtx, cursorY, x, y - 3, namePtr, color);
        }
    } while (++i < numGfs);

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = xBase;
    cfg->w = 0xB0;
    cfg->y = yBase;
    /* Regalloc: cursorY++/-- boosts cursorY priority to s3 over x(s4) */
    cursorY++;
    cursorY--;
    cfg->h = 0x5E;
    func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Render a single GF magic entry from the junction list.
 *
 * Checks whether the GF at the given list index is junctioned (charIdx
 * != 0xFF) and determines the display color. Renders the magic name,
 * GF level icon (if junctioned), and compatibility indicator.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param itemIdx Index into the GF list (offset by scroll).
 * @param tableBase Base pointer for column/row computation.
 * @param yBase Base Y position (on stack).
 * @return Updated cursor Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderGfMagicEntry);

/**
 * @brief Set up and render the GF magic list panel.
 *
 * Configures g_menuDisplayCfg for a scrollable GF list (icon 0x50,
 * 4 columns), loads ctx fields for page/scroll state, then calls
 * func_8002FF34 for the header and func_801EFBB4 with
 * renderGfMagicEntry as the per-item callback.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param xBase Base X position.
 * @param yBase Base Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderGfMagicPanel);

/**
 * @brief Check junction ability mask compatibility.
 *
 * If currentMask has any bits in common with abilityBit, returns 7
 * (incompatible). Otherwise returns whether availMask has the bit set.
 *
 * @param currentMask Current junction mask.
 * @param availMask Available abilities mask.
 * @param abilityBit Ability bit to check.
 * @return 7 if already junctioned, 1 if available, 0 if not.
 *
 */
s32 checkJunctionCompat(s32 currentMask, s32 availMask, s32 abilityBit) {
    s32 result = 7;
    if (!(currentMask & abilityBit)) {
        result = (availMask & abilityBit) != 0;
    }
    return result;
}

/**
 * @brief Render junction slot assignments with magic names and compatibility.
 *
 * Iterates over junction slots for a stat type, rendering each assigned
 * magic name with availability coloring, compatibility indicators, and
 * stat value formatting. Calls checkJunctionCompat to check slot availability
 * and getMagicNamePtr for spell name lookup.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @param slotType Junction slot type index.
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (-1 for default).
 * @param showValues Whether to show numeric stat values.
 * @return Updated cursor Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderJunctionSlotDetail);

/**
 * @brief Render a junction ability slot with availability coloring.
 *
 * Checks if the HP-J ability (bit 0x400) is available for the given
 * character based on juncted GFs. Renders the label in white (0x80)
 * if available, grayed out (0x1C0) if not.
 *
 * @param renderCtx Render context.
 * @param x X position.
 * @param y Y position.
 * @param color Base text color.
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (-1 for default/none).
 */
void renderHpJunctionSlot(s32 renderCtx, s32 x, s32 y, s32 color, s32 charIdx, s32 gfIdx) {
    JunctionGfEntry *gfEntry;
    s32 available;

    if (gfIdx < 0) {
        gfEntry = &D_801EEDD0;
    } else {
        gfEntry = &g_junctionGfTable[gfIdx];
    }

    available = checkJunctionCompat(g_junctionChars[charIdx].availFlags, gfEntry->abilityFlags, 0x400);

    {
        s32 menuCol = g_menuColor;
        /* Regalloc: color++/-- and do{x++/--}while(0) raise reference counts
           to assign x→s0, color→s1 (instead of default renderCtx→s0). */
        color++;
        color--;
        do { x++; x--; } while (0);

        func_800300F8(renderCtx, x, 0x128, y, color, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/** @brief Render junction ability slot — checks combined ability bits 0x19000. */
void renderStatusDefSlot(s32 renderCtx, s32 x, s32 y, s32 color, s32 charIdx, s32 gfIdx) {
    JunctionGfEntry *gfEntry;
    s32 available;

    if (gfIdx < 0) {
        gfEntry = &D_801EEDD0;
    } else {
        gfEntry = &g_junctionGfTable[gfIdx];
    }

    available = checkJunctionCompat(g_junctionChars[charIdx].availFlags, gfEntry->abilityFlags, 0x19000);

    {
        s32 menuCol = g_menuColor;
        /* Regalloc: see renderHpJunctionSlot comment. */
        color++;
        color--;
        do { x++; x--; } while (0);

        func_800300F8(renderCtx, x, 0x129, y, color, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/** @brief Render junction ability slot — checks ability bit 0x200. */
void renderElemAtkSlot(s32 renderCtx, s32 x, s32 y, s32 color, s32 charIdx, s32 gfIdx) {
    JunctionGfEntry *gfEntry;
    s32 available;

    if (gfIdx < 0) {
        gfEntry = &D_801EEDD0;
    } else {
        gfEntry = &g_junctionGfTable[gfIdx];
    }

    available = checkJunctionCompat(g_junctionChars[charIdx].availFlags, gfEntry->abilityFlags, 0x200);

    {
        s32 menuCol = g_menuColor;
        /* Regalloc: see renderHpJunctionSlot comment. */
        color++;
        color--;
        do { x++; x--; } while (0);

        func_800300F8(renderCtx, x, 0x12A, y, color, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/** @brief Render junction ability slot — checks ability bits 0x6800. */
void renderElemDefSlot(s32 renderCtx, s32 x, s32 y, s32 color, s32 charIdx, s32 gfIdx) {
    JunctionGfEntry *gfEntry;
    s32 available;

    if (gfIdx < 0) {
        gfEntry = &D_801EEDD0;
    } else {
        gfEntry = &g_junctionGfTable[gfIdx];
    }

    available = checkJunctionCompat(g_junctionChars[charIdx].availFlags, gfEntry->abilityFlags, 0x6800);

    {
        s32 menuCol = g_menuColor;
        /* Regalloc: see renderHpJunctionSlot comment. */
        color++;
        color--;
        do { x++; x--; } while (0);

        func_800300F8(renderCtx, x, 0x12B, y, color, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/**
 * @brief Render the elemental junction panel (Elem-Atk-J + Elem-Def-J).
 *
 * Displays the header for Elem-Atk-J (ability bit 0x200), then iterates
 * over elemental defense junction slots showing each junctioned spell
 * name with availability coloring. Uses ability mask 0x6800 to determine
 * the max slot count from both character and GF entries.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (-1 for default).
 * @return Updated cursor Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderElemJunctionPanel);

/**
 * @brief Render the status junction panel (ST-Atk-J + ST-Def-J).
 *
 * Same structure as renderElemJunctionPanel but for status junctions.
 * Header shows ST-Atk-J (ability bit 0x400), loop covers status defense
 * slots with ability mask 0x19000.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (-1 for default).
 * @return Updated cursor Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatusJunctionPanel);

/**
 * @brief Render the main stat junction panel (8 stats + ability slots).
 *
 * Reads the current stat slot and GF index from the junction context,
 * renders the stat header row, 8 stat entries, 4 ability slot types,
 * and the command/GF ability sections. Dispatches to elem/status/stat
 * sub-renderers based on the stat slot type.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @param showGf Whether to show GF index info.
 * @return Updated cursor Y position.
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderJunctionStatPanel);

/**
 * @brief Configure display panel and invoke rendering callback.
 *
 * Sets up the menu display config with the given position and a fixed
 * size of 0x150 x 0x48, clears icon fields, then calls func_801EF9AC.
 *
 * @param ctx Render context passed through to func_801EF9AC.
 * @param mode Render mode passed through to func_801EF9AC.
 * @param x X position for the display panel.
 * @param y Y position for the display panel.
 * @param renderParam Render parameter passed to func_801EF9AC (on stack).
 */
void setupStatBorderPanel(s32 ctx, s32 mode, s32 x, s32 y, s32 renderParam) {
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.w = 0x150;
    g_menuDisplayCfg.h = 0x48;
    g_menuDisplayCfg.y = y;
    func_801EF9AC(ctx, mode, renderParam, g_menuColor);
}

/**
 * @brief Render junction header panel with category-based string pair.
 *
 * Divides ctx->unk58 by 5 to select a category (0-3), calls renderInnerPanel
 * to look up two header strings per category, then renders them with
 * stat icon indicators. Finishes with a bordered panel via func_801EF9AC.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderJunctionHeader);

/**
 * @brief Render composite junction view (stat panel + optional scroll + border).
 *
 * Calls func_801EF8D8 for background, renderJunctionStatPanel for the
 * stat content, optionally func_801EF800 for scroll indicators, then
 * setupStatBorderPanel for the outer border.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 * @param scale Scale factor (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderJunctionComposite);

/**
 * @brief Render a single magic junction entry (name + icon + quantity).
 *
 * Looks up magic data from g_gameState for the given character/slot,
 * renders the spell name, junction icon, and quantity with availability
 * check against the junction GF table.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param itemIdx Item index in the list.
 * @param x Panel X position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderMagicJunctionEntry);

/**
 * @brief Set up and render the magic junction list panel.
 *
 * Configures g_menuDisplayCfg (icon 0x4A, width 0x78), loads page/scroll
 * state from ctx, then calls func_8002FF34 for the header and
 * func_801EFBB4 with renderMagicJunctionEntry as the callback.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderMagicListPanel);

/**
 * @brief Render a single ability list entry with flag-based categorization.
 *
 * Reads ability data from g_menuDisplayCfg.dataPtr, checks 5 ability
 * flag categories (commands, character abilities, GF abilities, party
 * abilities, menu abilities), renders the appropriate icon and name
 * for the entry.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param itemIdx Item index.
 * @param x Panel X position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderAbilityListEntry);

/**
 * @brief Set up and render the ability list panel.
 *
 * Configures g_menuDisplayCfg (icon 0x5E, various widths depending on
 * ability type), determines the list type from ctx->unk56 (commands vs
 * abilities), then calls func_801EFBB4 with renderAbilityListEntry
 * as the callback.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderAbilityListPanel);

/**
 * @brief Render a 3-row stat grid with command/ability icons.
 *
 * Calls func_801F79F8 for initial count, then iterates 3 rows rendering
 * stat name, icon, and optional highlighting via func_800300F8. Uses
 * func_801F1CE8 to check each stat entry's assignment state.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderStatRowGrid);

/**
 * @brief Render GF compatibility grid for junctioned GFs.
 *
 * Iterates over the character's junctioned GFs (up to 16), rendering
 * each GF's compatibility rating in a grid layout with name strings
 * and numeric values.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderGfCompatGrid);

/**
 * @brief Render scaling stat comparison panel with delta visualization.
 *
 * Reads unk3E from ctx as a scale factor, looks up values from
 * D_801FA3C8, and renders dual stat bars comparing current vs preview
 * values with a bordered panel via func_801EF9AC.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderScalingStatPanel);

/**
 * @brief Render a character name and status bar in the junction menu.
 *
 * Sets up g_menuDisplayCfg dimensions, optionally renders the character
 * name (looked up via getCharNamePtr from the character's ID in g_gameState),
 * then draws a color bar via func_801EF9AC.
 *
 * @param renderCtx Render context handle.
 * @param cursorY Current Y cursor position.
 * @param x X position for the panel.
 * @param height Y position / height parameter.
 * @param charIdx Character index (0-7), or 0xFF to skip name rendering.
 * @return Updated Y cursor position.
 */
s32 renderCharNameBar(s32 renderCtx, s32 cursorY, s32 x, s32 height, s32 charIdx) {
    CharMenuInfo *menuInfo;

    g_menuDisplayCfg.x = x;
    x += 9;
    g_menuDisplayCfg.y = height;
    g_menuDisplayCfg.w = 0x9A;
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.h = 0x16;
    height += 7;

    do {
        if (charIdx != 0xFF) {
            s32 gfInfo = func_801F3FB4((*(menuInfo = &g_charMenuInfo[charIdx])).statusFlags);
            s32 namePtr = getCharNamePtr(g_gameState.chars[charIdx].characterId);
            cursorY = func_801F0FEC(renderCtx, cursorY, x, height, namePtr, gfInfo);
        }
        return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
    } while (0);
}

/**
 * @brief Render callback for the junction menu.
 *
 * Main rendering dispatcher for all junction menu states. Computes a
 * stat scale factor from ctx->statScale via a lookup table (D_801FA3C8),
 * then switches on ctx->unk42 to render the appropriate panel:
 *   - Case 1: stat bars at full scale (0x1000)
 *   - Case 3: stat bars at current scale
 *   - Case 4: GF junction panel (scrolling slots, GF-to-character mapping)
 *   - Case 6: ability junction panel (nested switch on ability type 9–18)
 *   - Case 7: summary panel (stat delta bars)
 * After the switch, renders the common bottom panel, cursor overlay,
 * and help text. Returns the updated Y cursor position.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context handle.
 * @param cursorY Current Y cursor position.
 * @return Updated Y cursor position after rendering.
 * @note Non-matching — see https://decomp.me/scratch/xG3NK
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", renderJunctionMenu);

/**
 * @brief Initialize the junction GF table.
 *
 * For each of the 16 GFs: clears ability flags, loads GF compatibility byte,
 * queries learned abilities via func_800369CC and ORs their flag words into
 * abilityFlags. Then derives command/ability/max slot counts from the flag
 * bits. Initializes the sentinel entry at index 16. Maps juncted GFs to
 * characters by scanning each character's junctedGfs bitmask. Finally builds
 * the available GF index list (D_801EEDE0) from the availGfs bitmask.
 *
 * @note Non-matching — see https://decomp.me/scratch/CZM5k
 */
INCLUDE_ASM("asm/ovl/menujnc2/nonmatchings/menujnc2", initJunctionGfTable);

/**
 * @brief Initialize and enter the junction menu.
 *
 * Allocates a JunctionMenuCtx, copies character/disc info from the
 * parent menu, initializes ability tables for all 8 characters,
 * previews the selected character's junction, and enters the main
 * junction menu handler.
 *
 * @param parentCtx Parent menu context with character and parameter info.
 */
void initJunctionMenu(MenuParentCtx *parentCtx) {
    JunctionMenuCtx *ctx;
    s32 i;

    ctx = (JunctionMenuCtx *)func_801F179C((s32)junctionMenuUpdate, (s32)renderJunctionMenu);
    func_801F5300();
    if (ctx != NULL) {
        ctx->parentParam = parentCtx->param;
        ctx->charIdx = parentCtx->charIdx;
        ctx->discId = getGfAvailabilityMask();
        ctx->discCount = popcount(ctx->discId);
        ctx->unk64 = 0;
        ctx->unk61 = 0;
        ctx->unk62 = 0;
        ctx->unk42 = 0;
        ctx->unk4E = 0;
        ctx->unk50 = 0;
        ctx->unk40 = 0;
        ctx->dataPtr = (s32)D_801EEB28;
        ctx->unk63 = 0;
        initJunctionChars(ctx->parentParam);
        initJunctionGfTable();

        for (i = 0; i < CHARACTER_COUNT; i++) {
            rebuildJunctionFlags(i);
            refreshJunctionState(i);
        }

        snapshotJunctionPreview(ctx->charIdx);
        previewJunctionChange(ctx->charIdx, -1, -1, -1);
        buildMagicLookupTable(ctx->charIdx);
        junctionMenuUpdate((s32)ctx);
    }
    func_801F0948(0x1000);
}

/**
 * @brief Initialize junction menu: set mode 1, configure display, enable flag.
 *
 * Sets up the junction menu display by calling initialization functions,
 * configuring display areas, setting the active flag g_junctionMenuActive to 1,
 * then entering the main junction menu handler.
 *
 * @param parentCtx Parent menu context.
 */
void enterJunctionMenu(MenuParentCtx *parentCtx) {

    func_801F1DBC(1);
    func_801E2ABC((s32)parentCtx);
    func_801F1210(0x801D1000, 0x801CD000);
    g_junctionMenuActive = 1;
    initJunctionMenu(parentCtx);
}

/**
 * @brief Reset junction menu state and reinitialize.
 *
 * Calls func_801F1DBC(1), clears g_junctionMenuActive, then calls
 * initJunctionMenu with the context.
 *
 * @param charIdx Character index (0-7).
 */
void resetJunctionMenu(MenuParentCtx *parentCtx) {
    func_801F1DBC(1);
    g_junctionMenuActive = 0;
    initJunctionMenu(parentCtx);
}
