#include "common.h"
#include "menu.h"
#include "gamestate.h"
#include "battle.h"
#include "btl_color.h"
#include "btl_entity.h"
#include "game.h"
#include "numstr.h"
#include "color.h"
#include "gf.h"
#include "ability_list.h"

#include "menujnc2.h"
#include "card.h"
#include "psxsdk/libetc.h"

/*
 * Foreign symbols owned by other units (menumain/gamestate/battle/...). Kept
 * as local externs: their signatures differ per unit (the "same symbol,
 * different types" debt), so they cannot yet share a single owner-header decl.
 */



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
 * @param src Source encoded string list (NULL = decode nothing).
 * @param dst Destination buffer for decoded text.
 * @param charIdx Character index for name substitution.
 * @return Pointer to the scratchpad decode buffer.
 *
 * @note Handwritten function: manually retargets $gp at the PS1 scratchpad
 * (0x1F800300) for the intermediate decode buffer, restoring it on exit
 * (GP_SAVE_SCRATCH / GP_ALLOC / GP_FREE / GP_RESTORE_RET in common.h).
 * The unused @c pad array reproduces the original's oversized stack frame.
 */
u8 *decodeMenuString(u8 *src, u8 *dst, s32 charIdx) {
    s32 ch;
    u8 *name;
    s32 nameId;
    u8 *tempGp;
    u8 *savedGp;
    u8 *buf;
    u8 *ret;
    u8 *pos;
    u8 pad[0x40];

    nameId = charIdx;

    GP_SAVE_SCRATCH(tempGp);
    savedGp = tempGp;
    GP_ALLOC(buf, 0x80);

    if (src == NULL) {
        goto end;
    }

next_string:
    if (src == NULL) {
        goto end;
    }
    decodeMessage(src, buf, -1);
    src = func_8002F548(src);
    pos = buf;

    for (;;) {
        ch = *pos++;
        if (ch == 1 || ch == 7 || ch == 0) {
            goto end;
        }
        if (ch == 0xA) {
            ch = *pos++;
            if (ch == 0x27) {
                for (name = getCharName(nameId); *name != 0; name++) {
                    *dst++ = *name;
                }
                continue;
            }
        }
        *dst++ = ch;
        if (ch >= 0x19) {
            if (ch < 0x20) {
                *dst++ = *pos++;
            }
            continue;
        }
        if (ch == 2) {
            goto next_string;
        }
        if (ch >= 0x10) {
            continue;
        }
        *dst++ = *pos;
        if (ch == 5) {
            ch = *pos++;
            func_8002C734(ch);
        } else if (ch != 6) {
            pos++;
        }
    }

end:
    *dst = 0;
    GP_FREE(0x80);
    GP_RESTORE_RET(savedGp, ret);
    return ret;
}

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
s32 autoJunctionSlot(s32 charIdx, MagicSlot *magicSlots, s32 slotType, s32 flagMask) {
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
                pos = autoJunctionSlot(charIdx, abilityList, slotType, pos);
                count--;
            } while (count > 0);
        }
    } else if (slotType == JUNC_SLOT_DEF_STATUS) {
        count = g_junctionChars[charIdx].abilityCount[0];
        if (count > 0) {
            do {
                pos = autoJunctionSlot(charIdx, abilityList, slotType, pos);
                count--;
            } while (count > 0);
        }
    } else {
        pos = autoJunctionSlot(charIdx, abilityList, slotType, pos);
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
    if (g_junctionChars[charIdx].abilityRows != 0) {
        do {
            s32 abl = g_gameState.chars[charIdx].abilities[i];
            if (abl != 0) {
                g_assignedAbilities[abl / 32] |= (1 << (abl & 0x1F));
            }
        } while (++i < g_junctionChars[charIdx].abilityRows);
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
            return flags & JNC_AVAIL_STATUS_ATK;
        case 15:
        case 16:
        case 17:
        case 18:
            return flags & (JNC_AVAIL_STATUS_DEF | JNC_AVAIL_STATUS_DEF_X2 | JNC_AVAIL_STATUS_DEF_X4);
        case 9:
            return flags & JNC_AVAIL_ELEM_ATK;
        case 11:
        case 12:
        case 13:
        case 14:
            return flags & (JNC_AVAIL_ELEM_DEF | JNC_AVAIL_ELEM_DEF_X2 | JNC_AVAIL_ELEM_DEF_X4);
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

    maxSlots = g_junctionChars[charIdx].abilityRows;
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
    g_junctionChars[charIdx].abilityRows = maxAbl;
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
        g_junctionChars[i].abilityRows = 0;
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

    /*
     * Clear the just-compacted DEF_ELEM slots, i.e.
     * g_gameState.chars[charIdx].junctions[JUNCTION_DEF_ELEM_0 + 0..3].
     * The compiler's base/offset split (base + charIdx*stride + index, store
     * at the junctions offset, walked with q--) is not reproduced by plain
     * struct-field or struct-pointer access (both verified to change codegen),
     * so the pointer walk is matching-required. The stride and store offset
     * are struct-derived (sizeof / offsetof-style) rather than magic literals,
     * so they track any GameState/CharacterData layout change.
     */
    i = 3;
    {
        s32 base = (s32)&g_gameState;
        u8 *q = (u8 *)(base + charIdx * sizeof(CharacterData) + 3);
        do {
            *(u8 *)((s32)q + (s32)&((GameState *)0)->chars[0].junctions[JUNCTION_DEF_ELEM_0]) = 0;
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

    /*
     * Clear the just-compacted DEF_STATUS slots, i.e.
     * g_gameState.chars[charIdx].junctions[JUNCTION_DEF_STATUS_0 + 0..3].
     * Same matching-required pointer walk as compactCommandSlots above; stride
     * and store offset are struct-derived (sizeof / offsetof-style).
     */
    i = 3;
    {
        s32 base = (s32)&g_gameState;
        u8 *q = (u8 *)(base + charIdx * sizeof(CharacterData) + 3);
        do {
            *(u8 *)((s32)q + (s32)&((GameState *)0)->chars[0].junctions[JUNCTION_DEF_STATUS_0]) = 0;
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
            gfData = (u8 *)&D_80078E00;
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
 * (0x00-0x49) via a switch/jump table. Handles all junction menu logic:
 * panel animations, button input, GF/ability selection, auto-junction,
 * stat display, and screen transitions.
 *
 * 11,412 bytes - the largest function in menujnc2.
 *
 * @param ctx Junction menu context (JunctionMenuCtx *).
 */
void junctionMenuUpdate(JunctionMenuCtx *ctx) {
    u16 inputRepeat = g_menuDisplayCfg.inputRepeat;
    u16 inputNew = g_menuDisplayCfg.inputNew;
    u16 *statePtr;
    u16 state;
    s32 var_a2;
    s32 idx;

    if ((g_junctionMenuActive != 0) && (func_801F1200() != 0)) {
        if (ctx->state != 0x49) {
            ctx->state = 0x49;
        }
    }
    statePtr = &ctx->state;
    state = ctx->state;

dispatch:
    switch (state) {
        case 0x0:
            ctx->unk42 = 0;
            ctx->unk58 = 0xA;
            ctx->unk38 = getAbilityScrollOffset(0xA);
            initJunctionBackups(ctx->charIdx);
            renderStatValueBar(ctx, 1, 0);
            *statePtr = 1;
            break;
        case 0x1:
            ctx->unk3A += 0x100;
            if (ctx->unk3A >= 0x1000) {
                ctx->unk3A = 0x1000;
                *statePtr = 2;
            }
            renderStatValueBar(ctx, 1, 0);
            break;
        case 0x2:
            ctx->unk40 = 0;
            ctx->unk42 = 0;
            ctx->unk58 = 0xA;
            ctx->unk38 = getAbilityScrollOffset(0xA);
            *statePtr = 3;
            /* fallthrough */
        case 0x3:
            if (!((ctx->unk5A >> ctx->unk4E) & 1)) {
                ctx->unk4E = 0;
            }
            if (popcount(ctx->parentParam) >= 2) {
                if (inputRepeat & PADL1) {
                    if ((ctx->unk61 == 0) ||
                        (g_junctionChars[ctx->charIdx].currentHp <= g_gameState.chars[ctx->charIdx].currentHp)) {
                        restoreCommandAbilityBackup(ctx->charIdx, 1);
                        syncCharacterHp(ctx->charIdx);
                        applyJunctedGfs(ctx->charIdx);
                        snapshotJunctionPreview(ctx->charIdx);
                        ctx->unk5E = 0;
                        state = 4;
                        goto dispatch;
                    }
                    ctx->returnState = 3;
                    ctx->exitState = 8;
                    *statePtr = 0x2B;
                    break;
                }
                if (inputRepeat & PADR1) {
                    if ((ctx->unk61 == 0) ||
                        (g_junctionChars[ctx->charIdx].currentHp <= g_gameState.chars[ctx->charIdx].currentHp)) {
                        restoreCommandAbilityBackup(ctx->charIdx, 1);
                        syncCharacterHp(ctx->charIdx);
                        applyJunctedGfs(ctx->charIdx);
                        snapshotJunctionPreview(ctx->charIdx);
                        ctx->unk5E = 0;
                        state = 6;
                        goto dispatch;
                    }
                    ctx->returnState = 3;
                    ctx->exitState = 8;
                    *statePtr = 0x2B;
                    break;
                }
            }
            renderStatValueBar(ctx, 1, ctx->unk4E);
            if (inputNew & PADRdown) {
                sendSpuCommand(2);
                switch (ctx->unk4E) {
                case 0:
                    *statePtr = 0x23;
                    break;
                case 1:
                    *statePtr = 0xC;
                    break;
                case 2:
                    *statePtr = 9;
                    break;
                case 3:
                    *statePtr = 0x15;
                    break;
                default:
                    goto block35_fall;
                }
                break;
            }
        block35_fall:
            if (inputNew & PADRup) {
                if ((ctx->unk61 != 0) &&
                    (g_junctionChars[ctx->charIdx].currentHp > g_gameState.chars[ctx->charIdx].currentHp)) {
                    ctx->returnState = 3;
                    ctx->exitState = 8;
                    *statePtr = 0x2B;
                    break;
                } else {
                    sendSpuCommand(3);
                    ctx->unk61 = 0;
                    restoreCommandAbilityBackup(ctx->charIdx, 1);
                    syncCharacterHp(ctx->charIdx);
                    applyJunctedGfs(ctx->charIdx);
                    snapshotJunctionPreview(ctx->charIdx);
                    *statePtr = 0x48;
                    ctx->unk4E = func_801F76E0(inputRepeat, ctx->unk5A, ctx->unk4E);
                    ctx->itemPtr = renderInnerPanelAlt(D_801EEB1C[ctx->unk4E]);
                }
            } else {
                ctx->unk4E = func_801F76E0(inputRepeat, ctx->unk5A, ctx->unk4E);
                ctx->itemPtr = renderInnerPanelAlt(D_801EEB1C[ctx->unk4E]);
            }
            break;
        case 0x4:
            sendSpuCommand(1);
            renderStatValueBar(ctx, 1, ctx->unk4E);
            ctx->prevCharIdx = ctx->charIdx;
            ctx->charIdx = func_801F565C(ctx->charIdx, ctx->parentParam);
            snapshotJunctionPreview(ctx->charIdx);
            buildMagicLookupTable(ctx->charIdx);
            ctx->slideOffset = -0xF80;
            if (g_junctionChars[ctx->charIdx].availFlags == 0) {
                ctx->unk4A = 0;
                ctx->unk4E = 0;
                ctx->itemPtr = renderInnerPanelAlt(D_801EEB1C[0]);
                ctx->itemPtr2 = renderInnerPanelAlt(D_801EEB1C[ctx->unk4E]);
            }
            initJunctionBackups(ctx->charIdx);
            rebuildJunctionFlags(ctx->charIdx);
            *statePtr = 5;
            break;
        case 0x5:
            renderStatValueBar(ctx, 1, ctx->unk4E);
            ctx->slideOffset += 0x80;
            if (ctx->slideOffset >= 0) {
                ctx->slideOffset = 0;
                *statePtr = 3;
            }
            if (inputNew & PADL1) {
                *statePtr = 4;
            }
            if (inputNew & PADR1) {
                *statePtr = 6;
            }
            break;
        case 0x6:
            sendSpuCommand(1);
            renderStatValueBar(ctx, 1, ctx->unk4E);
            ctx->prevCharIdx = ctx->charIdx;
            ctx->charIdx = func_801F56E4(ctx->charIdx, ctx->parentParam);
            snapshotJunctionPreview(ctx->charIdx);
            buildMagicLookupTable(ctx->charIdx);
            ctx->slideOffset = 0xF80;
            if (g_junctionChars[ctx->charIdx].availFlags == 0) {
                ctx->unk4A = 0;
                ctx->unk4E = 0;
                ctx->itemPtr = renderInnerPanelAlt(D_801EEB1C[0]);
                ctx->itemPtr2 = renderInnerPanelAlt(D_801EEB1C[ctx->unk4E]);
            }
            initJunctionBackups(ctx->charIdx);
            rebuildJunctionFlags(ctx->charIdx);
            *statePtr = 7;
            break;
        case 0x7:
            renderStatValueBar(ctx, 1, ctx->unk4E);
            ctx->slideOffset -= 0x80;
            if ((ctx->slideOffset << 0x10) <= 0) {
                ctx->slideOffset = 0;
                *statePtr = 3;
            }
            if (inputNew & PADL1) {
                *statePtr = 4;
            }
            if (inputNew & PADR1) {
                *statePtr = 6;
            }
            break;
        case 0x8:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            ctx->unk40 -= 0x200;
            if ((ctx->unk40 << 0x10) <= 0) {
                ctx->unk40 = 0;
                if (ctx->unk63 != 0) {
                    ctx->unk63 = 0;
                    ctx->unk4E = 0;
                }
                renderStatValueBar(ctx, 0, ctx->unk4E);
                if (ctx->unk62 == 1) {
                    ctx->unk62 = 0;
                    if (ctx->unk5A & 8) {
                        ctx->unk4E = 3;
                        ctx->unk60 |= 1;
                        *statePtr = 0x15;
                    } else {
                        *statePtr = 2;
                    }
                } else {
                    *statePtr = 2;
                }
            } else if (ctx->unk63 != 0) {
                renderStatValueBar(ctx, 0, 0);
            }
            break;
        case 0x9:
            ctx->dataPtr = (s32)D_801EEB38;
            ctx->statInfo[2] = 7;
            ctx->unk40 = 0;
            ctx->unk4C = 0;
            *statePtr = 0xA;
            /* fallthrough */
        case 0xA:
            ctx->unk40 += 0x200;
            if (ctx->unk40 >= 0x1000) {
                ctx->unk40 = 0x1000;
                *statePtr = 0xB;
                var_a2 = ctx->unk4E;
                idx = 0;
                renderStatValueBar(ctx, idx, var_a2);
            } else {
                var_a2 = ctx->unk4E;
                idx = 0;
                renderStatValueBar(ctx, idx, var_a2);
            }
            break;
        case 0xB:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 1, ctx->unk4C);
            ctx->itemPtr = renderInnerPanelAlt(D_801EEB38[ctx->unk4C]);
            ctx->unk4C = func_801F6800(inputRepeat, 3, ctx->unk4C);
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                *statePtr = 8;
            }
            if (inputNew & PADRdown) {
                playSoundEffect(0x11);
                autoJunctionAll(ctx->charIdx, ctx->unk4C);
                previewJunctionChange(ctx->charIdx, -1, -1, -1);
                snapshotJunctionPreview(ctx->charIdx);
                updateJunctionSlotCount(ctx);
                ctx->unk61 = 1;
                *statePtr = 8;
            }
            break;
        case 0xC:
            ctx->dataPtr = (s32)D_801EEB30;
            updateJunctionSlotCount(ctx);
            ctx->unk40 = 0;
            /* fallthrough */
        case 0xD:
            ctx->unk4B = func_80035AA4(ctx->statInfo[1], 0);
            *statePtr = 0xE;
            break;
        case 0xE:
            ctx->unk40 += 0x200;
            if (ctx->unk40 >= 0x1000) {
                ctx->unk40 = 0x1000;
                *statePtr = 0xF;
            }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            break;
        case 0xF:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 1, ctx->unk4B);
            if (ctx->statInfo[1] == 3) {
                ctx->unk4B = func_801F6800(inputRepeat, 2, ctx->unk4B);
            }
            ctx->itemPtr = renderInnerPanelAlt(D_801EEB30[ctx->unk4B]);
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                *statePtr = 8;
            }
            if (inputNew & PADRdown) {
                sendSpuCommand(2);
                if (ctx->unk4B != 0) {
                    *statePtr = 0x10;
                } else {
                    *statePtr = 0x11;
                }
            }
            break;
        case 0x10:
            ctx->unk5C = 1;
            ctx->unk42 = 1;
            func_801F728C(renderInnerPanel(0x23), 0x40);
            ctx->returnState = 0x13;
            *statePtr = 0x12;
            break;
        case 0x11:
            ctx->unk5C = 1;
            ctx->unk42 = 1;
            func_801F728C(renderInnerPanel(0x24), 0x40);
            ctx->returnState = 0x14;
            *statePtr = 0x12;
            break;
        case 0x12:
            ctx->unk5C = func_801F6768(inputRepeat, 2, ctx->unk5C);
            func_801F6F88(ctx->unk5C);
            if (inputNew & PADRdown) {
                ctx->unk42 = 0;
                ctx->unk61 = 1;
                switch (ctx->unk5C) {
                case 0:
                    playSoundEffect(0x11);
                    ctx->unk5C = -1;
                    state = ctx->returnState;
                    goto dispatch;
                case 1:
                    sendSpuCommand(2);
                    ctx->unk5C = -1;
                    *statePtr = 0xE;
                    break;
                default:
                    if (inputNew & PADRup) {
                        sendSpuCommand(3);
                        ctx->unk5C = -1;
                        ctx->unk42 = 0;
                        *statePtr = 0xE;
                    }
                    break;
                }
            } else {
                if (inputNew & PADRup) {
                    sendSpuCommand(3);
                    ctx->unk5C = -1;
                    ctx->unk42 = 0;
                    *statePtr = 0xE;
                }
            }
            break;
        case 0x13:
        {
            s32 i;
            for (i = 0; i < 0x10; i++) {
                unjunctionGf(ctx->charIdx, i);
            }
            snapshotJunctionPreview(ctx->charIdx);
            refreshJunctionState(ctx->charIdx);
            snapshotJunctionPreview(ctx->charIdx);
            previewJunctionChange(ctx->charIdx, -1, -1, -1);
            updateJunctionSlotCount(ctx);
            ctx->unk63 = 1;
            *statePtr = 8;
            break;
        }
        case 0x14:
        {
            s32 i;
            for (i = 0; i < 0x39; i++) {
                func_801F78D8(ctx->charIdx, i);
            }
            snapshotJunctionPreview(ctx->charIdx);
            previewJunctionChange(ctx->charIdx, -1, -1, -1);
            updateJunctionSlotCount(ctx);
            *statePtr = 8;
            break;
        }
        case 0x15:
            ctx->statScale = 0;
            ctx->unk56 = 0;
            ctx->unk42 = 7;
            ctx->unk44 = ctx->statByte[0] / 11;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            ctx->statByte[1] = 0;
            ctx->statByte[0] = 0;
            ctx->statByte[2] = 0;
            ctx->statByte[3] = 0;
            ctx->unk36 = 0;
            ctx->unk5E = 0;
            ctx->unk56 = 1;
            ctx->unk44 = ctx->statByte[ctx->unk56] / 11;
            ctx->unk56 = 1;
            ctx->unk44 = ctx->statByte[ctx->unk56] / 11;
            *statePtr = 0x16;
            break;
        case 0x16:
        {
            s32 col; s32 grp;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            ctx->statScale += 0x100;
            if (ctx->statScale >= 0x1000) {
                ctx->statScale = 0x1000;
                state = 0x17;
                goto dispatch;
            }
            col = 0x1000 - ctx->statScale;
            {
                s32 adj = col;
                if (col < 0) {
                    adj = col + 0x3F;
                }
                col = D_801FA3C8[adj >> 6];
                grp = -(col * 0xC0);
            }
            if (grp < 0) {
                grp += 0xFFF;
            }
            renderStatColumnEntry(1, ctx->unk5E, grp >> 0xC);
            break;
        }
        case 0x17:
            ctx->unk5F = g_junctionChars[ctx->charIdx].abilityRows + 3;
            *statePtr = 0x18;
            /* fallthrough */
        case 0x18:
        {
            s32 col;
            s32 fr;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            fr = func_801F6768(inputRepeat, ctx->unk5F, ctx->unk5E);
            ctx->unk5E = fr;
            renderStatColumnEntry(1, ctx->unk5E, 0);
            if (fr < 3) {
                ctx->unk56 = 1;
                col = g_gameState.chars[ctx->charIdx].commands[ctx->unk5E];
            } else {
                ctx->unk56 = 2;
                col = g_gameState.chars[ctx->charIdx].commands[ctx->unk5E + 1];
            }
            if (col != 0) {
                if (fr < 3) {
                    { s32 b = (s32)&D_80078E00; u8 *p = (u8 *)((col - 0x14) * 8 + b); ctx->itemPtr = getAbilityEntryDesc(p[0x4185]); }
                } else {
                    ctx->itemPtr = getAbilityDesc(col);
                }
            } else {
                ctx->itemPtr = 0;
            }
            ctx->unk44 = ctx->statByte[ctx->unk56] / 11;
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                *statePtr = 0x19;
            }
            if (inputNew & PADRdown) {
                sendSpuCommand(2);
                *statePtr = 0x1B;
            }
            if (inputNew & PADRleft) {
                s32 e5 = ctx->unk5E;
                if (e5 >= 3) {
                    if (g_gameState.chars[ctx->charIdx].commands[e5 + 1] != 0) {
                        playSoundEffect(0x11);
                        g_gameState.chars[ctx->charIdx].commands[e5 + 1] = 0;
                    } else {
                        sendSpuCommand(5);
                    }
                } else if (g_gameState.chars[ctx->charIdx].commands[e5] != 0) {
                    playSoundEffect(0x11);
                    g_gameState.chars[ctx->charIdx].commands[e5] = 0;
                } else {
                    sendSpuCommand(5);
                }
                buildAssignedAbilities(ctx->charIdx);
                snapshotJunctionPreview(ctx->charIdx);
            }
            break;
        }
        case 0x1B:
            if (ctx->unk5E < 3) {
                ctx->unk57 = D_801EEF38;
                ctx->unk56 = 1;
            } else {
                ctx->unk57 = D_801EEF9A;
                ctx->unk56 = 2;
            }
            *statePtr = 0x1C;
            /* fallthrough */
        case 0x1C:
            {
                s32 d = ctx->statByte[ctx->unk56] / 11;
                s32 m = ctx->statByte[ctx->unk56] % 11;
                ctx->statByte[ctx->unk56] = d * 0xB + func_801F6768(inputRepeat, 0xB, m);
            }
            ctx->itemPtr = getAbilityNamePtr(ctx->unk56, ctx->statByte[ctx->unk56]);
            if (ctx->unk57 >= 0xC) {
                if (inputRepeat & PADLleft) {
                    state = 0x1D;
                    goto dispatch;
                }
                if (inputRepeat & PADLright) {
                    state = 0x1F;
                    goto dispatch;
                }
            }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatColumnEntry(0, ctx->unk5E, 0);
            renderStatListEntry(1, ctx->statByte[ctx->unk56]);
            if (inputNew & PADRdown) {
                if (ctx->statByte[ctx->unk56] >= (s32)ctx->unk57) {
                    sendSpuCommand(5);
                    if (inputNew & PADRup) {
                        sendSpuCommand(3);
                        *statePtr = 0x18;
                    }
                } else {
                    state = 0x21;
                    goto dispatch;
                }
            } else if (inputNew & PADRup) {
                sendSpuCommand(3);
                *statePtr = 0x18;
            }
            break;
        case 0x1D:
        {
            s32 col; s32 d; s32 m;
            ctx->itemPtr2 = ctx->itemPtr;
            d = ctx->statByte[ctx->unk56] / 11;
            m = ctx->statByte[ctx->unk56] % 11;
            col = d;
            ctx->unk45 = col;
            col = col - 1;
            if (col < 0) {
                col = ((ctx->unk57 + 0xA) / 11) - 1;
            }
            ctx->statByte[ctx->unk56] = col * 0xB + m;
            ctx->unk44 = col;
            ctx->itemPtr = getAbilityNamePtr(ctx->unk56, ctx->statByte[ctx->unk56]);
            ctx->unk34 = -0xE67;
            sendSpuCommand(1);
            *statePtr = 0x1E;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatColumnEntry(0, ctx->unk5E, 0);
            renderStatListEntry(1, ctx->statByte[ctx->unk56]);
            break;
        }
        case 0x1E:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatColumnEntry(0, ctx->unk5E, 0);
            renderStatListEntry(1, ctx->statByte[ctx->unk56]);
            ctx->unk34 += 0x199;
            if ((s16)ctx->unk34 >= 0) {
                ctx->unk34 = 0;
                *statePtr = 0x1C;
            }
            if (inputNew & PADLright) {
                *statePtr = 0x1F;
            }
            if (inputNew & PADLleft) {
                *statePtr = 0x1D;
            }
            break;
        case 0x1F:
        {
            s32 col; s32 d; s32 m;
            ctx->itemPtr2 = ctx->itemPtr;
            d = ctx->statByte[ctx->unk56] / 11;
            m = ctx->statByte[ctx->unk56] % 11;
            col = d;
            ctx->unk45 = col;
            col = col + 1;
            if (col >= ((ctx->unk57 + 0xA) / 11)) {
                col = 0;
            }
            ctx->statByte[ctx->unk56] = col * 0xB + m;
            ctx->unk44 = col;
            ctx->unk34 = 0xE67;
            ctx->itemPtr = getAbilityNamePtr(ctx->unk56, ctx->statByte[ctx->unk56]);
            sendSpuCommand(1);
            *statePtr = 0x20;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatColumnEntry(0, ctx->unk5E, 0);
            renderStatListEntry(1, ctx->statByte[ctx->unk56]);
            break;
        }
        case 0x20:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatColumnEntry(0, ctx->unk5E, 0);
            renderStatListEntry(1, ctx->statByte[ctx->unk56]);
            if (((ctx->unk34 -= 0x199) << 0x10) <= 0) {
                ctx->unk34 = 0;
                *statePtr = 0x1C;
            }
            if (inputNew & PADLright) {
                *statePtr = 0x1F;
            }
            if (inputNew & PADLleft) {
                *statePtr = 0x1D;
            }
            break;
        case 0x21:
            assignJunctionSlot(ctx->charIdx, ctx->unk5E, ctx->unk56, ctx->statByte[ctx->unk56], 1);
            buildAssignedAbilities(ctx->charIdx);
            snapshotJunctionPreview(ctx->charIdx);
            ctx->unk61 = 1;
            *statePtr = 0x18;
            break;
        case 0x19:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            saveCommandAbilityBackup(ctx->charIdx, 1);
            snapshotJunctionPreview(ctx->charIdx);
            *statePtr = 0x1A;
            /* fallthrough */
        case 0x1A:
        {
            s32 col; s32 grp;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            ctx->statScale -= 0x100;
            if ((ctx->statScale << 0x10) <= 0) {
                if (ctx->unk60 & 1) {
                    ctx->unk4E = 0;
                    ctx->unk60 &= 0xFE;
                }
                *statePtr = 8;
            }
            col = 0x1000 - ctx->statScale;
            {
                s32 adj = col;
                if (col < 0) {
                    adj = col + 0x3F;
                }
                col = D_801FA3C8[adj >> 6];
                grp = -(col * 0xC0);
            }
            if (grp < 0) {
                grp += 0xFFF;
            }
            renderStatColumnEntry(1, ctx->unk5E, grp >> 0xC);
            break;
        }
        case 0x23:
            ctx->dataPtr = (s32)D_801EEB28;
            ctx->unk36 = 0;
            ctx->unk4A = 0;
            ctx->unk58 = 0xA;
            ctx->unk38 = getAbilityScrollOffset(0xA);
            /* fallthrough */
        case 0x24:
            if (g_junctionChars[ctx->charIdx].availFlags != 0) {
                ctx->statInfo[0] = 3;
            } else {
                ctx->statInfo[0] = 1;
                ctx->unk4A = 0;
            }
            *statePtr = 0x25;
            /* fallthrough */
        case 0x25:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            ctx->unk40 += 0x200;
            if (ctx->unk40 >= 0x1000) {
                ctx->unk40 = 0x1000;
                *statePtr = 0x26;
                state = 0x26;
                goto dispatch;
            }
            break;
        case 0x26:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 1, ctx->unk4A);
            if (g_junctionChars[ctx->charIdx].availFlags != 0) {
                ctx->unk4A = func_801F6800(inputRepeat, 2, ctx->unk4A);
            }
            ctx->itemPtr = renderInnerPanelAlt(D_801EEB28[ctx->unk4A]);
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                ctx->unk5A = getJunctionCapabilities(ctx->charIdx);
                if (ctx->unk62 != 0) {
                    if (ctx->unk5A & 4) {
                        ctx->unk4E = 2;
                        ctx->unk40 = 0;
                        saveCommandAbilityBackup(ctx->charIdx, 1);
                        ctx->itemPtr = renderInnerPanelAlt(D_801EEB20);
                        *statePtr = 9;
                    } else if (ctx->unk5A & 8) {
                        ctx->unk4E = 3;
                        ctx->unk40 = 0;
                        ctx->unk62 = 0;
                        ctx->unk60 |= 1;
                        saveCommandAbilityBackup(ctx->charIdx, 1);
                        ctx->itemPtr = renderInnerPanelAlt(D_801EEB22[0]);
                        *statePtr = 0x15;
                    } else {
                        ctx->unk62 = 0;
                        *statePtr = 8;
                    }
                } else {
                    *statePtr = 8;
                }
            }
            if (inputNew & PADRdown) {
                sendSpuCommand(2);
                if (ctx->unk4A != 0) {
                    *statePtr = 0x31;
                } else {
                    *statePtr = 0x27;
                }
            }
            break;
        case 0x27:
            ctx->unk42 = 4;
            ctx->unk62 = 0;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            ctx->statSlot = 0;
            ctx->unk44 = 0;
            ctx->statScale = 0x100;
            ctx->itemPtr = renderInnerPanel(0x25);
            *statePtr = 0x28;
            break;
        case 0x28:
            ctx->statScale += 0x100;
            if (ctx->statScale >= 0x1000) {
                ctx->statScale = 0x1000;
                *statePtr = 0x29;
            }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            break;
        case 0x29:
        {
            s32 col; s32 slot; s32 tmp;
            slot = ctx->statSlot;
            col = slot;
            if (col < 0) {
                col += 3;
            }
            //tmp = (s8)(slot - (col >> 2) * 4);
            tmp = (s8) (slot - (((unsigned long long) (col >> 2)) * 4)); // Fixme
            ctx->statSlot = (col >> 2) * 4 + func_801F6768(inputRepeat, 4, tmp);
            ctx->unk58 = 0xA;
            ctx->unk38 = getAbilityScrollOffset(0xA);
            {
                s32 arg1;
                if (ctx->statSlot < (s32)ctx->discCount) {
                    arg1 = D_801EEDE0[ctx->statSlot];
                } else {
                    arg1 = -1;
                }
                previewJunctionChange(ctx->charIdx, arg1, -1, -1);
            }
            if (ctx->discCount >= 5) {
                if (inputRepeat & PADLleft) {
                    state = 0x2D;
                    goto dispatch;
                }
                if (inputRepeat & PADLright) {
                    state = 0x2F;
                    goto dispatch;
                }
            }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            if (inputNew & PADRdown) {
                if (ctx->statSlot < (s32)ctx->discCount) {
                    state = 0x42;
                    goto dispatch;
                }
                sendSpuCommand(5);
            }
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                previewJunctionChange(ctx->charIdx, -1, -1, -1);
                saveCommandAbilityBackup(ctx->charIdx, 1);
                *statePtr = 0x2A;
            } else if (inputNew & PADRleft) {
                if (ctx->statSlot < (s32)ctx->discCount) {
                    sendSpuCommand(2);
                    *statePtr = 0x45;
                } else {
                    sendSpuCommand(5);
                }
            }
            break;
        }
        case 0x2A:
            ctx->statScale -= 0x100;
            if ((ctx->statScale << 0x10) <= 0) {
                ctx->statScale = 0;
                *statePtr = 0x24;
            }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            break;
        case 0x2B:
            playSoundEffect(0x10);
            ctx->statScale = 0x1000;
            ctx->unk42 = 3;
            ctx->unk5C = 1;
            func_801F7148(func_801F6AFC(0x3A), 0, renderInnerPanel(0x1D), 0x40);
            *statePtr = 0x2C;
            break;
        case 0x2C:
            ctx->unk5C = func_801F6768(inputRepeat, 2, ctx->unk5C);
            func_801F6F88(ctx->unk5C);
            if (inputNew & PADRdown) {
                sendSpuCommand(2);
                syncCharacterHp(ctx->charIdx);
                ctx->statSlot = -1;
                if (ctx->unk5C != 0) {
                    syncCharacterHp(ctx->charIdx);
                    revertJunctionState(ctx->charIdx);
                    rebuildJunctionFlags(ctx->charIdx);
                    saveCommandAbilityBackup(ctx->charIdx, 1);
                    initJunctionGfTable();
                    *statePtr = ctx->exitState;
                } else {
                    applyJunctedGfs(ctx->charIdx);
                    rebuildJunctionFlags(ctx->charIdx);
                    *statePtr = ctx->returnState;
                }
                snapshotJunctionPreview(ctx->charIdx);
                initJunctionBackups(ctx->charIdx);
                ctx->statScale = 0;
            }
            break;
        case 0x2D:
        {
            s32 i; s32 col; s32 grp; s32 slot; s32 gd; s32 tmp;
            sendSpuCommand(6);
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            grp = ctx->discCount + 3;
            if (grp < 0) {
                grp = ctx->discCount + 6;
            }
            slot = ctx->statSlot;
            gd = grp >> 2;
            col = slot;
            if (col < 0) {
                col += 3;
            }
            tmp = (s8)(slot - (col >> 2) * 4);
            i = col >> 2;
            ctx->unk45 = i;
            i = i - 1;
            if (i < 0) {
                i = gd - 1;
            }
            ctx->unk44 = i;
            ctx->statSlot = i * 4 + tmp;
            {
                s32 arg1;
                if ((s8)ctx->statSlot < (s32)ctx->discCount) {
                    arg1 = D_801EEDE0[ctx->statSlot];
                } else {
                    arg1 = -1;
                }
                previewJunctionChange(ctx->charIdx, arg1, -1, -1);
            }
            ctx->unk34 = -0xE67;
            *statePtr = 0x2E;
            break;
        }
        case 0x2E:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            ctx->unk34 += 0x199;
            if ((s16)ctx->unk34 >= 0) {
                ctx->unk34 = 0;
                *statePtr = 0x29;
            }
            if (inputNew & PADLleft) {
                *statePtr = 0x2D;
            }
            if (inputNew & PADLright) {
                *statePtr = 0x2F;
            }
            break;
        case 0x2F:
        {
            s32 i; s32 col; s32 grp; s32 slot; s32 gd; s32 tmp;
            sendSpuCommand(6);
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            grp = ctx->discCount + 3;
            if (grp < 0) {
                grp = ctx->discCount + 6;
            }
            slot = ctx->statSlot;
            gd = grp >> 2;
            col = slot;
            if (col < 0) {
                col += 3;
            }
            tmp = (s8)(slot - (col >> 2) * 4);
            i = col >> 2;
            ctx->unk45 = i;
            i = i + 1;
            if (i >= gd) {
                i = 0;
            }
            ctx->unk44 = i;
            ctx->statSlot = i * 4 + tmp;
            {
                s32 arg1;
                if ((s8)ctx->statSlot < (s32)ctx->discCount) {
                    arg1 = D_801EEDE0[ctx->statSlot];
                } else {
                    arg1 = -1;
                }
                previewJunctionChange(ctx->charIdx, arg1, -1, -1);
            }
            ctx->unk34 = 0xE67;
            *statePtr = 0x30;
            break;
        }
        case 0x30:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            if (((ctx->unk34 -= 0x199) << 0x10) <= 0) {
                ctx->unk34 = 0;
                *statePtr = 0x29;
            }
            if (inputNew & PADLleft) {
                *statePtr = 0x2D;
            }
            if (inputNew & PADLright) {
                *statePtr = 0x2F;
            }
            break;
        case 0x31:
        {
            s32 col;
            ctx->unk42 = 6;
            ctx->unk58 = 0xA;
            ctx->unk38 = getAbilityScrollOffset(0xA);
            ctx->unk59 = getJunctionSlotCount(ctx->charIdx, 2);
            col = ctx->unk50;
            if (col < 0) {
                col += 3;
            }
            ctx->unk44 = col >> 2;
            ctx->statScale = 0;
            ctx->itemPtr = renderInnerPanel(0x26);
            *statePtr = 0x32;
            /* fallthrough */
        }
        case 0x32:
            *statePtr = 0x33;
            /* fallthrough */
        case 0x33:
            *statePtr = 0x34;
            /* fallthrough */
        case 0x34:
        {
            s32 col; s32 grp;
            ctx->itemPtr = renderInnerPanel(0x26);
            ctx->statScale += 0x100;
            if (ctx->statScale >= 0x1000) {
                ctx->statScale = 0x1000;
            }
            col = ctx->unk58 % 5;
            grp = ctx->unk58 / 5;
            if (inputRepeat & PADLdown) {
                ctx->statScale = 0x1000;
                sendSpuCommand(1);
                col += 1;
                if (col >= (s32)ctx->unk59) {
                    col = (grp == 3);
                }
            }
            if (inputRepeat & PADLup) {
                ctx->statScale = 0x1000;
                sendSpuCommand(1);
                col -= 1;
                if (col < (grp == 3)) {
                    col = ctx->unk59 - 1;
                }
            }
            if (grp == 3 && col == 0) {
                col = 1;
            }
            ctx->unk58 = grp * 5 + col;
            ctx->unk38 = getAbilityScrollOffset(ctx->unk58);
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(1, ctx->unk58);
            previewJunctionChange(ctx->charIdx, -1, -1, -1);
            if (inputRepeat & PADLleft) {
                if (grp != 0) {
                    ctx->statScale = 0x1000;
                    state = 0x35;
                    goto dispatch;
                }
            }
            if ((inputRepeat & PADLright) && (grp < 3)) {
                ctx->statScale = 0x1000;
                state = 0x37;
                goto dispatch;
            }
            ctx->dataPtr2 = buildMagicAvailMask(ctx->charIdx, ctx->unk58);
            if (inputNew & PADRup) {
                ctx->statScale = 0x1000;
                sendSpuCommand(3);
                previewJunctionChange(ctx->charIdx, -1, -1, -1);
                *statePtr = 0x39;
            } else {
                if (inputNew & PADRdown) {
                    ctx->statScale = 0x1000;
                    if (getJunctionSlotFlags(ctx->charIdx, ctx->unk58) != 0) {
                        sendSpuCommand(2);
                        *statePtr = 0x3A;
                    } else {
                        sendSpuCommand(5);
                    }
                }
                if (inputNew & PADRleft) {
                    ctx->statScale = 0x1000;
                    if (getJunctionSlotFlags(ctx->charIdx, ctx->unk58) != 0) {
                        s32 idx = D_801EEAC0[ctx->unk58];
                        s32 jv = g_gameState.chars[ctx->charIdx].junctions[idx];
                        if (func_801F1CE8(ctx->charIdx, jv) != 0) {
                            playSoundEffect(0x11);
                            func_801F78D8(ctx->charIdx, jv);
                            ctx->unk61 = 1;
                            snapshotJunctionPreview(ctx->charIdx);
                            previewJunctionChange(ctx->charIdx, -1, -1, -1);
                        } else {
                            sendSpuCommand(5);
                        }
                    } else {
                        sendSpuCommand(5);
                    }
                }
            }
            break;
        }
        case 0x35:
        {
            s32 col; s32 grp;
            sendSpuCommand(1);
            ctx->dataPtr2 = buildMagicAvailMask(ctx->charIdx, ctx->unk58);
            grp = ctx->unk58 / 5;
            col = ctx->unk58 % 5;
            grp = grp - 1;
            ctx->unk59 = getJunctionSlotCount(ctx->charIdx, grp);
            if (col >= (s32)ctx->unk59) {
                col = ctx->unk59 - 1;
            }
            ctx->unk58 = col + grp * 5;
            if (grp < 2) {
                ctx->unk36 = 0xF00;
                *statePtr = 0x36;
            } else {
                ctx->unk38 = getAbilityScrollOffset(ctx->unk58);
            }
            break;
        }
        case 0x36:
        {
            s32 col; s32 grp;
            if (((ctx->unk36 -= 0x100) << 0x10) <= 0) {
                ctx->unk36 = 0;
                *statePtr = 0x34;
            }
            grp = getAbilityScrollOffset(ctx->unk58);
            col = ctx->unk36 * 0xA0;
            if (col < 0) {
                col += 0xFFF;
            }
            ctx->unk38 = grp - (col >> 0xC);
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(1, ctx->unk58);
            break;
        }
        case 0x37:
        {
            s32 col; s32 grp;
            ctx->dataPtr2 = buildMagicAvailMask(ctx->charIdx, ctx->unk58);
            grp = ctx->unk58 / 5;
            col = ctx->unk58 % 5;
            sendSpuCommand(1);
            grp = grp + 1;
            ctx->unk59 = getJunctionSlotCount(ctx->charIdx, grp);
            if (col >= (s32)ctx->unk59) {
                col = ctx->unk59 - 1;
            }
            if (grp == 3 && col == 0) {
                col = 1;
            }
            ctx->unk58 = col + grp * 5;
            if (grp < 3) {
                ctx->unk36 = 0xF00;
                *statePtr = 0x38;
            } else {
                ctx->unk38 = getAbilityScrollOffset(ctx->unk58);
            }
            break;
        }
        case 0x38:
        {
            s32 col; s32 grp;
            if (((ctx->unk36 -= 0x100) << 0x10) <= 0) {
                ctx->unk36 = 0;
                *statePtr = 0x34;
            }
            grp = getAbilityScrollOffset(ctx->unk58);
            col = ctx->unk36 * 0xA0;
            if (col < 0) {
                col += 0xFFF;
            }
            ctx->unk38 = grp + (col >> 0xC);
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(1, ctx->unk58);
            break;
        }
        case 0x39:
            ctx->unk36 = 0x1000;
            ctx->unk58 = 0xA;
            ctx->unk38 = getAbilityScrollOffset(0xA);
            ctx->unk59 = getJunctionSlotCount(ctx->charIdx, 2);
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            ctx->statScale -= 0x100;
            if ((ctx->statScale << 0x10) <= 0) {
                ctx->statScale = 0;
                *statePtr = 0x24;
            }
            break;
        case 0x3A:
        {
            s32 col;
            ctx->dataPtr2 = buildMagicAvailMask(ctx->charIdx, ctx->unk58);
            col = ctx->unk50;
            if (col < 0) {
                col += 3;
            }
            ctx->unk44 = col >> 2;
            ctx->itemPtr = renderInnerPanelAlt(0x26);
            *statePtr = 0x3B;
            /* fallthrough */
        }
        case 0x3B:
        {
            s32 col; s32 grp; s32 slot; s32 tmp;
            slot = ctx->unk50;
            col = slot;
            if (col < 0) {
                col += 3;
            }
            tmp = (s8) (slot - (((unsigned long long) (col >> 2)) * 4));
            ctx->unk50 = (col >> 2) * 4 + func_801F6768(inputRepeat, 4, tmp) ;
            if (inputRepeat & PADLleft) {
                state = 0x3C;
                goto dispatch;
            }
            if (!(inputRepeat & PADLright)) {
                renderStatValueBar(ctx, 0, ctx->unk4E);
                renderStatDeltaEntry(ctx, 0, ctx->unk4A);
                renderAbilityEntry(0, ctx->unk58);
                renderMagicListEntry(1, ctx->unk50);
                grp = g_gameState.chars[ctx->charIdx].magic[ctx->unk50].magicId;
                { s32 cc = D_801EEAC0[ctx->unk58];
                  s32 mm = 1 << ctx->unk50;
                  if (!(ctx->dataPtr2 & mm)) {
                    previewJunctionChange(ctx->charIdx, -1, cc, 0);
                  } else {
                    previewJunctionChange(ctx->charIdx, -1, cc, grp);
                  } }
                if (inputNew & PADRdown) {
                    s32 mm2 = 1 << ctx->unk50;
                    if (ctx->dataPtr2 & mm2) {
                        playSoundEffect(0x11);
                        state = 0x40;
                        goto dispatch;
                    }
                    sendSpuCommand(5);
                }
                if (inputNew & PADRup) {
                    sendSpuCommand(3);
                    previewJunctionChange(ctx->charIdx, -1, -1, -1);
                    *statePtr = 0x34;
                }
            } else {
                state = 0x3E;
                goto dispatch;
            }
            break;
        }
        case 0x3C:
        {
            s32 i; s32 col; s32 grp; s32 slot; s32 tmp;
            sendSpuCommand(1);
            slot = ctx->unk50;
            col = slot;
            if (col < 0) {
                col += 3;
            }
            tmp = (s8)(slot - (col >> 2) * 4);
            i = col >> 2;
            ctx->unk45 = i;
            i = i - 1;
            if (i < 0) {
                i = 7;
            }
            ctx->unk44 = i;
            ctx->unk50 = i * 4 + tmp;
            grp = g_gameState.chars[ctx->charIdx].magic[ctx->unk50].magicId;
            { s32 cc = D_801EEAC0[ctx->unk58];
              s32 mm = 1 << ctx->unk50;
              if (!(ctx->dataPtr2 & mm)) {
                previewJunctionChange(ctx->charIdx, -1, cc, 0);
              } else {
                previewJunctionChange(ctx->charIdx, -1, cc, grp);
              } }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(0, ctx->unk58);
            renderMagicListEntry(1, ctx->unk50);
            ctx->unk34 = -0xE67;
            *statePtr = 0x3D;
            break;
        }
        case 0x3D:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(0, ctx->unk58);
            renderMagicListEntry(1, ctx->unk50);
            ctx->unk34 += 0x199;
            if ((s16)ctx->unk34 >= 0) {
                ctx->unk34 = 0;
                *statePtr = 0x3B;
            }
            if (inputNew & PADLleft) {
                *statePtr = 0x3C;
            }
            if (inputNew & PADLright) {
                *statePtr = 0x3E;
            }
            break;
        case 0x3E:
        {
            s32 i; s32 col; s32 grp; s32 slot; s32 tmp;
            sendSpuCommand(1);
            slot = ctx->unk50;
            col = slot;
            if (col < 0) {
                col += 3;
            }
            tmp = (s8)(slot - (col >> 2) * 4);
            i = col >> 2;
            ctx->unk45 = i;
            i = i + 1;
            if (i >= 8) {
                i = 0;
            }
            ctx->unk44 = i;
            ctx->unk50 = i * 4 + tmp;
            grp = g_gameState.chars[ctx->charIdx].magic[ctx->unk50].magicId;
            { s32 cc = D_801EEAC0[ctx->unk58];
              s32 mm = 1 << ctx->unk50;
              if (!(ctx->dataPtr2 & mm)) {
                previewJunctionChange(ctx->charIdx, -1, cc, 0);
              } else {
                previewJunctionChange(ctx->charIdx, -1, cc, grp);
              } }
            ctx->unk34 = 0xE67;
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(0, ctx->unk58);
            renderMagicListEntry(1, ctx->unk50);
            *statePtr = 0x3F;
            break;
        }
        case 0x3F:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderAbilityEntry(0, ctx->unk58);
            renderMagicListEntry(1, ctx->unk50);
            if (((ctx->unk34 -= 0x199) << 0x10) <= 0) {
                ctx->unk34 = 0;
                *statePtr = 0x3B;
            }
            if (inputNew & PADLleft) {
                *statePtr = 0x3C;
            }
            if (inputNew & PADLright) {
                *statePtr = 0x3E;
            }
            break;
        case 0x40:
        {
            s32 col; s32 grp;
            grp = g_gameState.chars[ctx->charIdx].magic[ctx->unk50].magicId;
            col = D_801EEAC0[ctx->unk58];
            if (g_gameState.chars[ctx->charIdx].junctions[col] == grp) {
                if (func_801F1CE8(ctx->charIdx, grp, ctx->charIdx) != 0) {
                    func_801F78D8(ctx->charIdx, grp);
                    ctx->unk61 = 1;
                }
            } else {
                func_801F78D8(ctx->charIdx, grp, ctx->charIdx);
                ctx->unk61 = 1;
                g_gameState.chars[ctx->charIdx].junctions[col] = grp;
            }
            previewJunctionChange(ctx->charIdx, -1, -1, -1);
            snapshotJunctionPreview(ctx->charIdx);
            *statePtr = 0x34;
            break;
        }
        case 0x42:
        {
            s32 col; s32 grp; s32 ci;
            col = D_801EEDE0[ctx->statSlot];
            ci = ctx->charIdx;
            grp = g_junctionGfTable[col].charIdx;
            if (ci == grp) {
                playSoundEffect(0x11);
                unjunctionGfAndRefresh(ctx->charIdx, col);
                ctx->unk61 = 1;
                ctx->unk62 = 1;
                break;
            }
            if (grp == 0xFF) {
                playSoundEffect(0x11);
                junctionGfToChar(ctx->charIdx, col);
                ctx->unk61 = 1;
                ctx->unk62 = 1;
                break;
            }
            decodeMenuString(renderInnerPanel(0x27), D_801EF1B0,
                g_gameState.chars[g_junctionGfTable[col].charIdx].characterId);
            state = 0x43;
            goto dispatch;
        }
        case 0x43:
            sendSpuCommand(5);
            ctx->unk66 = 0x258;
            func_8002D6AC(0, D_801EF1B0);
            func_801F23D0(0, 0x68, D_801EF1B0);
            func_8002DE74(0, 0x56);
            func_8002CA58(0, 0);
            func_8002DCF4(0);
            *statePtr = 0x44;
            break;
        case 0x44:
            ctx->unk66 -= 1;
            if (inputNew & (PADRup | PADRdown)) {
                func_801F7BEC(inputNew);
                ctx->unk66 = 0;
            }
            if ((s16)ctx->unk66 <= 0) {
                func_8002DD58(0);
                *statePtr = 0x29;
            }
            break;
        case 0x45:
            D_801EED00 = func_800369CC(D_801EEDE0[ctx->statSlot], D_801EEC50, 0);
            *statePtr = 0x46;
            /* fallthrough */
        case 0x46:
            ctx->unk64 += 0xAA;
            if ((s16)ctx->unk64 >= 0x1000) {
                ctx->unk64 = 0x1000;
            }
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            if (inputNew & PADRup) {
                sendSpuCommand(3);
                *statePtr = 0x47;
            }
            if (inputNew & (PADRdown | PADRleft)) {
                sendSpuCommand(2);
                *statePtr = 0x47;
            }
            break;
        case 0x47:
            renderStatValueBar(ctx, 0, ctx->unk4E);
            renderStatDeltaEntry(ctx, 0, ctx->unk4A);
            renderStatEffectBar(1, ctx);
            if (((ctx->unk64 -= 0xAA) << 0x10) <= 0) {
                ctx->unk64 = 0;
                *statePtr = 0x29;
            }
            break;
        case 0x48:
            renderStatValueBar(ctx, 1, ctx->unk4E);
            *statePtr = 0x49;
            break;
        case 0x49:
            ctx->unk3A -= 0x100;
            if (ctx->unk3A < 0) {
                ctx->unk3A = 0;
                func_801F18FC(ctx);
                func_801F0BB0();
                func_801F5340();
                func_80023888();
                *statePtr = 8;
            }
            if (func_801F1200() == 0) {
                renderStatValueBar(ctx, 1, ctx->unk4E);
            }
            break;
        }

    ctx->unk5A = getJunctionCapabilities(ctx->charIdx);
    if (g_junctionMenuActive != 0) {
        func_801F1DB0(ctx->unk3A);
    }
}

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

    if (flags & BTL_ABL_HIT_J)      result |= JNC_AVAIL_HIT;
    if (flags & BTL_ABL_LCK_J)      result |= JNC_AVAIL_LCK;
    if (flags & BTL_ABL_ELEM_ATK_J) result |= JNC_AVAIL_ELEM_ATK;
    if (flags & BTL_ABL_ST_ATK_J)   result |= JNC_AVAIL_STATUS_ATK;
    if (flags & BTL_ABL_ELEM_DEF_J) result |= JNC_AVAIL_ELEM_DEF;
    if (flags & BTL_ABL_ST_DEF_J)   result |= JNC_AVAIL_STATUS_DEF;
    return result;
}

/**
 * @brief Renders the status attack window showing all the % for statuses
 * 
 * Shown when user is navigating junction to ST-A/D > Status attack
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param xBase Base X position (offset +0x88 applied).
 * @param yBase Base Y position (offset +0x7F applied).
 */
void renderStatTableA(s32 renderCtx, s32 cursorY, s32 xBase, s32 yBase) {
    StatTableEntry *entry;
    MenuDisplayConfig *cfg;
    s32 i;
    s32 cy2;
    s32 x, y;
    s32 baseFlags, currentFlags;
    u32 baseVal, currentVal;
    s32 indicator, color;
    s32 fmtResult;
    s32 bit;
    s32 tmp;
    u8 fmtParam;
    u8 buf[12];

    xBase += 0x88;
    yBase += 0x7F;
    entry = D_801EEBA8;
    cfg = &g_menuDisplayCfg;
    fmtResult = func_801F6AFC(0x14);

    for (i = 0; i < 13; i++, entry++) {
        if (i == 10) {
            continue;
        }
        x = entry->col;
        y = entry->row;
        x *= 105;
        y *= 13;
        tmp = x + 8;
        x = tmp + xBase;
        tmp = y + 9;
        y = tmp + yBase;

        baseFlags = encodeBattleAbilityFlags(&g_junctionPreview);
        currentFlags = encodeBattleAbilityFlags((BattleCharData *)&g_battleChars);

        /* Preview stat value when ability slot i is junctioned (else 0). */
        bit = 1 << i;
        if (baseFlags & bit) {
            baseVal = g_junctionPreview.atkStatusHit - 100;
            currentVal = baseVal;
        } else {
            baseVal = 0;
        }
        /* Current stat value for the same slot (overrides the seed above). */
        bit = 1 << i;
        tmp = bit;
        if (currentFlags & tmp) {
            currentVal = ((BattleCharData *)&g_battleChars)->atkStatusHit - 100;
        } else {
            currentVal = 0;
        }

        /* Change indicator arrow + bar color from preview-vs-current delta. */
        color = 7;
        indicator = 7;
        if (baseVal < currentVal) {
            indicator = 0x6E;
            color = 2;
        }
        if (currentVal < baseVal) {
            indicator = 0x6D;
            color = 3;
        }

        cy2 = func_8002FF34(renderCtx, cursorY, entry->labelId, x, y, g_menuColor);
        x += 0x39;
        y += 4;
        if (indicator != 7) {
            cy2 = func_800300F8(renderCtx, cy2, indicator, x, y, g_menuColor, (color * 64) + 2);
        }
        x += 0xA;
        func_8002F294(currentVal, buf, func_80020F84(0xB)[1]);
        fmtParam = func_80020F84(0xB)[1];
        func_8002F2EC(&buf[2], 2, fmtParam, func_80020F84(0xB)[0]);
        func_8002A2C4(buf, fmtResult);
        cursorY = func_8002C56C(renderCtx, cy2, x, y, &buf[2], color);
    }

    if (func_80037ADC() == 4) {
        cfg->iconType = 0xFC;
    } else {
        cfg->iconType = 0xD0;
    }
    cfg->iconSubType = 0;
    cfg->x = xBase;
    cfg->y = yBase;
    cfg->w = 0xDA;
    cfg->h = 0x59;
    func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Renders the status defence window showing all the % for statuses
 * 
 * Shown when user is navigating junction to ST-A/D > Status defence
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param xBase Base X position (offset +0x88 applied).
 * @param yBase Base Y position (offset +0x78 applied).
 */
void renderStatTableB(s32 renderCtx, s32 cursorY, s32 xBase, s32 yBase) {
    StatTableEntry *entry;
    MenuDisplayConfig *cfg;
    s32 fmtResult;
    s32 i;
    s32 cy2;
    s32 x, y;
    u32 baseVal, currentVal;
    s32 indicator, color;
    s32 statOffset;
    s32 tmp;
    u8 fmtParam;
    u8 buf[12];

    xBase += 0x88;
    yBase += 0x78;
    entry = D_801EEB40;
    cfg = &g_menuDisplayCfg;
    fmtResult = func_801F6AFC(0x14);

    for (i = 0; i < 13; i++, entry++) {
        color = 7;
        x = entry->col;
        y = entry->row;
        x *= 105;
        y *= 13;
        tmp = x + 8;
        x = tmp + xBase;
        tmp = y + 3;
        y = tmp + yBase;
        statOffset = entry->statOffset;
        /* Preview byte read first (held), then current byte; both decoded. */
        tmp = ((u8 *)&g_junctionPreview)[statOffset];
        currentVal = ((u8 *)&g_battleChars)[statOffset];
        indicator = 7;
        baseVal = func_801F5104(tmp);
        currentVal = func_801F5104(currentVal);
        if (baseVal < currentVal) {
            indicator = 0x6E;
            color = 2;
        }
        if (currentVal < baseVal) {
            indicator = 0x6D;
            color = 3;
        }
        cy2 = func_8002FF34(renderCtx, cursorY, entry->labelId, x, y, g_menuColor);
        x += 0x39;
        y += 4;
        if (indicator != 7) {
            cy2 = func_800300F8(renderCtx, cy2, indicator, x, y, g_menuColor, (color * 64) + 2);
        }
        x += 0xA;
        func_8002F294(currentVal, buf, func_80020F84(0xB)[1]);
        fmtParam = func_80020F84(0xB)[1];
        func_8002F2EC(&buf[2], 2, fmtParam, func_80020F84(0xB)[0]);
        func_8002A2C4(buf, fmtResult);
        cursorY = func_8002C56C(renderCtx, cy2, x, y, &buf[2], color);
    }

    if (func_80037ADC() == 4) {
        cfg->iconType = 0xFD;
    } else {
        cfg->iconType = 0xD1;
    }
    cfg->iconSubType = 0;
    cfg->x = xBase;
    cfg->y = yBase;
    cfg->w = 0xDA;
    cfg->h = 0x60;
    func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Renders the elemental attack window showing all the % for elements.
 *
 * Shown when the user is navigating junction to EL-A/D > Elemental attack.
 * Iterates the D_801EEC10 table (8 rows). Each row compares the preview
 * element flags/value (g_junctionPreview atkElem fields) against the current ones
 * (D_800788E4/D_800788E5) via a per-row bit test to pick the up/down change
 * indicator, then renders the label, indicator, and the numeric value.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param xBase Base X position (offset +0x88 applied).
 * @param yBase Base Y position (offset +0x93 applied).
 */
void renderStatTableC(s32 renderCtx, s32 cursorY, s32 xBase, s32 yBase) {
    StatTableEntry *entry;
    MenuDisplayConfig *cfg;
    s32 fmtResult;
    s32 i;
    s32 cy2;
    s32 x, y;
    u8 *bufPtr;
    s32 baseFlags, currentFlags;
    u32 baseVal, currentVal;
    s32 indicator, color;
    s32 bit;
    s32 tmp;
    u8 fmtParam;
    u8 buf[12];

    baseFlags = g_junctionPreview.atkElemBase;
    currentFlags = D_800788E4;

    xBase += 0x88;
    yBase += 0x93;
    entry = D_801EEC10;

    cfg = &g_menuDisplayCfg;
    fmtResult = func_801F6AFC(0x14);
    i = 0;
    bufPtr = &buf[2];

    for (; i < 8; i++, entry++) {
        x = entry->col;
        y = entry->row;
        x *= 105;
        y *= 13;
        tmp = x + 0xA;
        x = tmp + xBase;
        tmp = y + 8;
        y = tmp + yBase;
        bit = 1 << i;
        if (baseFlags & bit) {
            baseVal = g_junctionPreview.atkElemBonus;
            currentVal = baseVal;
        } else {
            baseVal = 0;
        }
        bit = 1 << i;
        tmp = bit;
        if (currentFlags & tmp) {
            currentVal = D_800788E5;
        } else {
            currentVal = 0;
        }
        indicator = 7;
        color = 7;
        if (baseVal < currentVal) {
            indicator = 0x6E;
            color = 2;
        }
        if (currentVal < baseVal) {
            indicator = 0x6D;
            color = 3;
        }
        cy2 = func_8002FF34(renderCtx, cursorY, entry->labelId, x, y, g_menuColor);
        x += 0x39;
        y += 4;
        if (indicator != 7) {
            cy2 = func_800300F8(renderCtx, cy2, indicator, x, y, g_menuColor, (color * 64) + 2);
        }
        x += 0xA;
        func_8002F294(currentVal, buf, func_80020F84(0xB)[1]);
        fmtParam = func_80020F84(0xB)[1];
        func_8002F2EC(bufPtr, 2, fmtParam, func_80020F84(0xB)[0]);
        func_8002A2C4(buf, fmtResult);
        cursorY = func_8002C56C(renderCtx, cy2, x, y, bufPtr, color);
    }

    if (func_80037ADC() == 4) {
        cfg->iconType = 0xFE;
    } else {
        cfg->iconType = 0xD2;
    }
    cfg->iconSubType = 0;
    cfg->x = xBase;
    cfg->y = yBase;
    cfg->w = 0xDA;
    cfg->h = 0x43;
    func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Renders the elemental defence window showing the % for each element.
 *
 * Shown when the user is navigating junction to EL-A/D > Elemental defence.
 * Sibling of renderStatTableC (shares the D_801EEC10 table). Iterates 8 rows;
 * each reads a u16 stat value at the row's statOffset from the preview char
 * data (g_junctionPreview) and the current char (g_battleChars), compares them
 * for the up/down change indicator, optionally draws the '%' glyph (0xAF) when
 * func_801F5144 applies, then renders the label, indicator, and formatted value.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param xBase Base X position (offset +0x88 applied).
 * @param yBase Base Y position (offset +0x93 applied).
 */
void renderStatTableD(s32 renderCtx, s32 cursorY, s32 xBase, s32 yBase) {
    StatTableEntry *entry;
    MenuDisplayConfig *cfg;
    s32 fmtResult;
    s32 tmp2;          /* second offset temp; declared here for register allocation */
    s32 i;
    s32 cy2;
    s32 x;
    u8 *previewBase;   /* cached preview-data base; placement fixes the base-load order */
    s32 y;
    u8 *bufPtr;
    s32 statOffset;
    u16 baseVal;
    u32 currentVal;
    s32 indicator, color;
    s32 tmp;
    u8 fmtParam;
    u8 buf[12];

    xBase += 0x88;
    yBase += 0x93;
    entry = D_801EEC10;
    cfg = &g_menuDisplayCfg;
    fmtResult = func_801F6AFC(0x14);
    i = 0;
    bufPtr = &buf[2];

    for (; i < 8; i++, entry++) {
        x = entry->col;
        y = entry->row;
        x *= 105;
        y *= 13;
        tmp = x + 0xA;
        tmp2 = y + 8;
        x = tmp + xBase;
        y = tmp2 + yBase;
        previewBase = (u8 *)&g_junctionPreview;
        statOffset = entry->statOffset;
        baseVal = *(u16 *)(previewBase + statOffset);
        currentVal = *(u16 *)((u8 *)&g_battleChars + statOffset);
        indicator = 7;
        color = 7;
        if (baseVal < currentVal) {
            indicator = 0x6E;
            color = 2;
        }
        if (currentVal < baseVal) {
            indicator = 0x6D;
            color = 3;
        }
        if (func_801F5144(currentVal) != 0) {
            cursorY = func_8002FF34(renderCtx, cursorY, 0xAF, x + 0x12, y, g_menuColor);
        }
        currentVal = func_801F510C(currentVal);
        cy2 = func_8002FF34(renderCtx, cursorY, entry->labelId, x, y, g_menuColor);
        x += 0x39;
        y += 4;
        if (indicator != 7) {
            cy2 = func_800300F8(renderCtx, cy2, indicator, x, y, g_menuColor, (color * 64) + 2);
        }
        x += 0xA;
        func_8002F294(currentVal, buf, func_80020F84(0xB)[1]);
        fmtParam = func_80020F84(0xB)[1];
        func_8002F2EC(bufPtr, 2, fmtParam, func_80020F84(0xB)[0]);
        func_8002A2C4(buf, fmtResult);
        cursorY = func_8002C56C(renderCtx, cy2, x, y, bufPtr, color);
    }

    if (func_80037ADC() == 4) {
        cfg->iconType = 0xFF;
    } else {
        cfg->iconType = 0xD3;
    }
    cfg->iconSubType = 0;
    cfg->x = xBase;
    cfg->y = yBase;
    cfg->w = 0xDA;
    cfg->h = 0x43;
    func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

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
    u8 *namePtr;
    s32 ctx = renderCtx + 0x10;
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    AbilityListEntry *table;

    if (D_801EED00 > 0) {
        table = D_801EEC50;
        do {
            row = i / 11;
            rem = i - row * 11;
            xOff = row * 155 + 13;
            yOff = rem * 13 + 11;
            xPos = x + xOff;
            yPos = y + yOff;
            cursorY = func_8002FF34(ctx, cursorY, table->category + 0xD8, xPos, yPos - 2, g_menuColor);
            xPos += 14;
            namePtr = getAbilityName(table->slotIndex);
            gfInfo = 7;
            table++;
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
 * for computing stat changes from junction previews: the bar X position
 * slides left proportionally to the animation scale and the bar width
 * interpolates between the full width and the stat delta segment.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position (on stack).
 * @return Updated cursor Y position.
 */
s32 renderStatDeltaBarExt(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    s16 statBuf[36];
    s32 scale;
    MenuDisplayConfig *cfg;
    s32 delta;
    s32 prod;
    s32 rem;
    s32 rem244;
    s32 barX;
    s32 barW;
    s32 barY;
    s32 h;
    s32 type5A;
    s32 statIdx;
    s32 count;

    scale = ctx->unk40;
    type5A = ctx->unk5A;
    cfg = &g_menuDisplayCfg;
    statIdx = ctx->unk4E;

    func_801F5984(D_801EEB1C, statBuf, 11);

    barX = -(scale * statBuf[statIdx]) / 4096;
    barX += (0x1A + x);
    barY = y + 6;
    count = 11;
    delta = statBuf[statIdx + 1] - statBuf[statIdx];
    rem = 0x1000 - scale;
    rem244 = 61 * (rem * 4);

    prod = (delta + 2) * scale / 4096;

    cursorY = func_801EF8D8(renderCtx, cursorY);
    cursorY = func_801F5A38(renderCtx, cursorY, barX, barY, count, D_801EEB1C, type5A);

    prod = (delta + 0x1A) * scale;
    barW = (rem244 + prod) / 4096;

    g_menuDisplayCfg.x = x + 0x1A;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = barW - 0x26;
    h = 0x12;
    g_menuDisplayCfg.h = 0x12;

    cursorY = func_801EF800(renderCtx, cursorY, cfg);

    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = barW;
    g_menuDisplayCfg.h = h;

    return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

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
 * @return Updated cursor Y position, unchanged when the item slot is empty.
 */
s32 renderMagicItemCallback(s32 renderCtx, s32 cursorY, s32 itemIdx, s32 columnIdx, s32 xOffset) {
    u8  buf[0x80];
    s32 result = cursorY;
    u8 *itemText = ((u8 **)g_menuDisplayCfg.dataPtr)[itemIdx];

    if (itemText != NULL) {
        s32 deltaX = xOffset + 0xA;
        s32 x = g_menuDisplayCfg.x + deltaX;
        s32 y = g_menuDisplayCfg.y + 9;

        decodeMessage(itemText, buf, -1);
        result = func_801F0FEC(renderCtx, cursorY, x, y, buf, 7);
    }
    return result;
}

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
    g_menuDisplayCfg.dataPtr = (s32)&ctx->itemPtr;

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
    u8 *namePtr;
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
 * @brief Render a single GF magic entry of the junction GF list.
 *
 * Per-item callback registered by renderGfMagicPanel (via func_801EFBB4)
 * for a 4-rows-per-column grid. Looks up the GF for list slot
 * col * 4 + row through the available-GF index list (D_801EEDE0), then
 * renders the GF magic name (id + 0x40) colored 7 when not junctioned,
 * 1 when junctioned to the displayed character (g_menuDisplayCfg.itemAttr),
 * or 0 when junctioned to another character. Junctioned GFs also get the
 * 0xC0 marker icon. Finally draws the GF level via drawColorByMenuPalette
 * at a packed (y << 16 | x) position.
 *
 * @param renderCtx Render context.
 * @param cursorY Current draw cursor position (chained through each call).
 * @param col Grid column index.
 * @param row Grid row index (13 px per row).
 * @param xOff Column X offset (on stack).
 * @return Updated draw cursor position.
 */
s32 renderGfMagicEntry(s32 renderCtx, s32 cursorY, s32 col, s32 row, s32 xOff) {
    s32 idx;
    s32 color;
    s32 ysum;
    s32 xsum;
    JunctionGfEntry *gf;
    u8 *namePtr;

    if (g_menuDisplayCfg.itemId > (idx = col * 4 + row)) {
        idx = D_801EEDE0[idx];
        gf = &g_junctionGfTable[idx];
        color = 7;
        if (gf->charIdx != 0xFF) {
            color = gf->charIdx == g_menuDisplayCfg.itemAttr;
        }
        namePtr = getMagicNamePtr(idx + 0x40);
        cursorY = func_801F0FEC(renderCtx, cursorY, 0xC + g_menuDisplayCfg.x + xOff, 7 + g_menuDisplayCfg.y + row * 13, namePtr, color);
        if (gf->charIdx != 0xFF) {
            ysum = 7 + g_menuDisplayCfg.y + row * 13;
            xsum = 0x6C + g_menuDisplayCfg.x + xOff;
            cursorY = func_8002FF34(renderCtx, cursorY, 0xC0, xsum, ysum, g_menuColor);
        }
        ysum = 7 + g_menuDisplayCfg.y + row * 13;
        xsum = 0x90 + g_menuDisplayCfg.x + xOff;
        cursorY = drawColorByMenuPalette(renderCtx, cursorY, (ysum << 16) | (xsum & 0xFFFF), gf->level, color);
    }
    return cursorY;
}

/**
 * @brief Set up and render the GF magic list panel.
 *
 * Configures g_menuDisplayCfg for a scrollable GF list (icon 0x50,
 * 0x9A x 0x40, 4 columns), copies page/scroll/disc state from @p ctx,
 * renders a header icon via func_8002FF34, optionally draws a disc-count
 * indicator via func_801F5F60 when five or more discs are present, draws
 * the list frame via func_801F5F30, and finally registers
 * renderGfMagicEntry as the per-item callback via func_801EFBB4.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Base X position.
 * @param y Base Y position (on stack).
 */
void renderGfMagicPanel(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    s32 result;

    g_menuDisplayCfg.iconType = 0x50;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.w = 0x9A;
    g_menuDisplayCfg.h = 0x40;
    g_menuDisplayCfg.columnCount = 4;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.pageStart = ctx->unk44;
    g_menuDisplayCfg.pageEnd = ctx->unk45;
    g_menuDisplayCfg.scrollOffset = ctx->unk34;
    g_menuDisplayCfg.itemId = ctx->discCount;
    g_menuDisplayCfg.itemAttr = ctx->charIdx;

    result = func_8002FF34(renderCtx, cursorY, 0x17, x + 0x7F, y, g_menuColor);

    if (ctx->discCount >= 5) {
        result = func_801F5F60(renderCtx, result, g_menuColor, 3);
    }

    result = func_801F5F30(renderCtx, result, x + 0x16, y, g_menuColor, ctx->unk44);
    func_801EFBB4(renderCtx, result, renderGfMagicEntry);
}

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
 * @param wideMode Nonzero to read the stat as u16 (HP row), zero for u8 stats.
 * @param slotIdx Stat slot index (0-8, indexes D_801EEAD4).
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (-1 for default).
 * @return Updated cursor Y position.
 */
s32 renderJunctionSlotDetail(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 wideMode, s32 slotIdx,
                             s32 charIdx, s32 gfIdx) {
    /* Regalloc: availFlags declared before entry — the pair spills, and the
       declaration order sets their stack-slot order. */
    s32 availFlags, gfFlags;
    JunctionSlotDetail *entry;
    s32 available;
    s32 flags;
    s32 baseVal, curVal;
    s32 dispBase, dispCur;
    s32 icon;
    s32 magicId;
    s32 slot;
    u8 *name;
    s32 xPos, yPos;
    s32 val;
    s32 magicBit;
    s32 abilityBit;

    entry = &D_801EEAD4[slotIdx];
    abilityBit = 1 << slotIdx;
    flags = entry->flags;
    availFlags = g_junctionChars[charIdx].availFlags;

    if (gfIdx >= 0) {
        gfFlags = g_junctionGfTable[gfIdx].abilityFlags;
    } else {
        gfFlags = 0;
    }
    available = checkJunctionCompat(availFlags, gfFlags, abilityBit);

    xPos = x;
    cursorY = func_800300F8(renderCtx, cursorY, entry->labelId, xPos, y, g_menuColor,
                            (!available) ? 0x1C0 : 0x80);

    if (wideMode) {
        /* Regalloc: the address goes through `name` (reused as scratch) — a fresh
           pointer variable colors differently. */
        name = (u8 *)&g_junctionPreview + D_801EEAD4[slotIdx].statOffset;
        baseVal = *(u16 *)name;
        name = (u8 *)&g_battleChars + D_801EEAD4[slotIdx].statOffset;
        curVal = *(u16 *)name;
    } else {
        name = (u8 *)&g_junctionPreview + D_801EEAD4[slotIdx].statOffset;
        /* Regalloc: preview byte staged through yPos (dead here) before baseVal. */
        yPos = *name;
        baseVal = yPos;
        name = (u8 *)&g_battleChars + D_801EEAD4[slotIdx].statOffset;
        curVal = *name;
    }
    xPos = x + 0x20;

    slot = entry->junctionSlot;
    magicId = g_gameState.chars[charIdx].junctions[slot];
    yPos = y;
    if (magicId != 0) {
        /* Regalloc: magicBit computed inside the branch so the constant 1 stays
           in the second test's block (delay-slot placement). */
        magicBit = 1 << slot;
        if (availFlags & magicBit) {
            name = getMagicNamePtr(magicId);
        } else {
            name = (u8 *)renderInnerPanel(0x1A);
        }
    } else {
        name = (u8 *)renderInnerPanel(0x1A);
    }
    cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, name, available);

    dispCur = curVal;
    dispBase = baseVal;
    if (flags & 0x40) {
        dispBase = func_801F7BAC(baseVal);
        dispCur = func_801F7BAC(curVal);
    }
    if (dispBase < dispCur) {
        icon = 0x6E;
        available = 2;
    } else if (dispCur < dispBase) {
        icon = 0x6D;
        available = 3;
    } else {
        icon = 7;
        available = icon;
    }
    xPos = x + 0x6A;
    yPos = y + 2;
    if (icon != 7) {
        cursorY = func_800300F8(renderCtx, cursorY, icon, xPos, yPos, g_menuColor, (available * 64) + 2);
    }

    if (flags & 1) {
        xPos = x + 0x90;
    } else {
        xPos = x + 0x98;
    }
    xPos = (y << 16) | (xPos & 0xFFFF);
    val = curVal;
    if (flags & 0x40) {
        val = func_801F7BAC(val);
    }
    if (flags & 0x20) {
        val = func_801F7BE4(val);
    }
    cursorY = drawColorByMenuPalette(renderCtx, cursorY, xPos, val, available);

    if (flags & 0x80) {
        if (entry->flags & 1) {
            xPos = x + 0x90;
        } else {
            xPos = x + 0x98;
        }
        yPos = y + 2;
        name = (u8 *)func_801F6AFC(0x14);
        cursorY = func_8002C56C(renderCtx, cursorY, xPos, yPos, name, available);
    }
    return cursorY;
}

/**
 * @brief Render a junction ability slot with availability coloring.
 *
 * Checks if the HP-J ability (bit 0x400) is available for the given
 * character based on juncted GFs. Renders the label in white (0x80)
 * if available, grayed out (0x1C0) if not.
 *
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x X position.
 * @param y Y position.
 * @param charIdx Character index (0-7).
 * @param gfIdx GF index (-1 for default/none).
 * @return Updated cursor Y position.
 */
s32 renderHpJunctionSlot(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 charIdx, s32 gfIdx) {
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
        /* Regalloc: y++/-- and do{cursorY++/--}while(0) raise reference counts
           to assign cursorY→s0, y→s1 (instead of default renderCtx→s0). */
        y++;
        y--;
        do { cursorY++; cursorY--; } while (0);

        return func_800300F8(renderCtx, cursorY, 0x128, x, y, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/** @brief Render junction ability slot — checks combined ability bits 0x19000. */
s32 renderStatusDefSlot(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 charIdx, s32 gfIdx) {
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
        y++;
        y--;
        do { cursorY++; cursorY--; } while (0);

        return func_800300F8(renderCtx, cursorY, 0x129, x, y, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/** @brief Render junction ability slot — checks ability bit 0x200. */
s32 renderElemAtkSlot(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 charIdx, s32 gfIdx) {
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
        y++;
        y--;
        do { cursorY++; cursorY--; } while (0);

        return func_800300F8(renderCtx, cursorY, 0x12A, x, y, menuCol, (!available) ? 0x1C0 : 0x80);
    }
}

/** @brief Render junction ability slot — checks ability bits 0x6800. */
s32 renderElemDefSlot(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 charIdx, s32 gfIdx) {
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
        y++;
        y--;
        do { cursorY++; cursorY--; } while (0);

        return func_800300F8(renderCtx, cursorY, 0x12B, x, y, menuCol, (!available) ? 0x1C0 : 0x80);
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
s32 renderElemJunctionPanel(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 charIdx, s32 gfIdx) {
    JunctionGfEntry *gfEntry;
    JunctionMenuEntry *junc;
    s32 available;
    s32 charSlots, gfSlots, numSlots;
    s32 magicId;
    u8 *name;
    s32 i;
    s32 xPos, yPos;
    /* Regalloc: keeping the name-column offset in a variable stops CSE from
       merging the header's x + 0x30 with the later rows' (cf. renderJunctionHeader's
       `seven`); combine folds the constant back, so the emitted code is identical. */
    s32 nameCol = 0x30;

    if (gfIdx < 0) {
        gfEntry = &D_801EEDD0;
    } else {
        gfEntry = &g_junctionGfTable[gfIdx];
    }
    junc = &g_junctionChars[charIdx];

    /* Elem-Atk-J header row. */
    available = checkJunctionCompat(junc->availFlags, gfEntry->abilityFlags, 0x200);
    xPos = x + 0x10;
    yPos = y + 4;
    cursorY = func_800300F8(renderCtx, cursorY, 0x12A, xPos, yPos, g_menuColor,
                            (!available) ? 0x1C0 : 0x80);
    magicId = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_ELEM];
    if (magicId == 0) {
        name = (u8 *)renderInnerPanel(0x1A);
    } else {
        name = getMagicNamePtr(magicId);
    }
    cursorY = func_801F0FEC(renderCtx, cursorY, x + nameCol, yPos, name, available);

    /* Elem-Def-J rows: as many as the larger of the character's and GF's slot counts. */
    charSlots = (junc->availFlags & 0x6800) ? junc->abilityCount[1] : 0;
    gfSlots = (gfEntry->abilityFlags & 0x6800) ? gfEntry->ablSlotCount : 0;
    numSlots = (charSlots < gfSlots) ? gfSlots : charSlots;

    if (numSlots == 0) {
        /* No defensive slots available — draw a single empty row. */
        xPos = x + 0x10;
        yPos = y + 0x11;
        cursorY = func_800300F8(renderCtx, cursorY, 0x12B, xPos, yPos, g_menuColor, 0x1C0);
        xPos = x + 0x30;
        name = (u8 *)renderInnerPanel(0x1A);
        cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, name, 0);
    } else {
        for (i = 0; i < numSlots; i++) {
            available = checkJunctionCompat(i < charSlots, i < gfSlots, 1);
            xPos = x + 0x10;
            yPos = y + 0x11 + i * 13;
            cursorY = func_800300F8(renderCtx, cursorY, 0x12B, xPos, yPos, g_menuColor,
                                    (!available) ? 0x1C0 : 0x80);
            xPos = x + 0x30;
            magicId = g_gameState.chars[charIdx].junctions[JUNCTION_DEF_ELEM_0 + i];
            if (magicId == 0) {
                name = (u8 *)renderInnerPanel(0x1A);
            } else {
                name = getMagicNamePtr(magicId);
            }
            cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, name, available);
        }
    }
    return cursorY;
}

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
s32 renderStatusJunctionPanel(s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 charIdx, s32 gfIdx) {
    JunctionGfEntry *gfEntry;
    JunctionMenuEntry *junc;
    s32 available;
    s32 charSlots, gfSlots, numSlots;
    s32 magicId;
    u8 *name;
    s32 i;
    s32 xPos, yPos;
    /* Regalloc: see renderElemJunctionPanel — keeps CSE from merging the header's
       x + 0x30 with the later rows'. */
    s32 nameCol = 0x30;

    if (gfIdx < 0) {
        gfEntry = &D_801EEDD0;
    } else {
        gfEntry = &g_junctionGfTable[gfIdx];
    }
    junc = &g_junctionChars[charIdx];

    /* ST-Atk-J header row. */
    available = checkJunctionCompat(junc->availFlags, gfEntry->abilityFlags, 0x400);
    xPos = x + 0x10;
    yPos = y + 4;
    cursorY = func_800300F8(renderCtx, cursorY, 0x128, xPos, yPos, g_menuColor,
                            (!available) ? 0x1C0 : 0x80);
    magicId = g_gameState.chars[charIdx].junctions[JUNCTION_ATK_STATUS];
    if (magicId == 0) {
        name = (u8 *)renderInnerPanel(0x1A);
    } else {
        name = getMagicNamePtr(magicId);
    }
    cursorY = func_801F0FEC(renderCtx, cursorY, x + nameCol, yPos, name, available);

    /* ST-Def-J rows: as many as the larger of the character's and GF's slot counts. */
    charSlots = (junc->availFlags & 0x19000) ? junc->abilityCount[0] : 0;
    gfSlots = (gfEntry->abilityFlags & 0x19000) ? gfEntry->cmdSlotCount : 0;
    numSlots = (charSlots < gfSlots) ? gfSlots : charSlots;

    if (numSlots == 0) {
        /* No defensive slots available — draw a single empty row. */
        xPos = x + 0x10;
        yPos = y + 0x11;
        cursorY = func_800300F8(renderCtx, cursorY, 0x129, xPos, yPos, g_menuColor, 0x1C0);
        xPos = x + 0x30;
        name = (u8 *)renderInnerPanel(0x1A);
        cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, name, 0);
    } else {
        for (i = 0; i < numSlots; i++) {
            available = checkJunctionCompat(i < charSlots, i < gfSlots, 1);
            xPos = x + 0x10;
            yPos = y + 0x11 + i * 13;
            cursorY = func_800300F8(renderCtx, cursorY, 0x129, xPos, yPos, g_menuColor,
                                    (!available) ? 0x1C0 : 0x80);
            xPos = x + 0x30;
            magicId = g_gameState.chars[charIdx].junctions[JUNCTION_DEF_STATUS_0 + i];
            if (magicId == 0) {
                name = (u8 *)renderInnerPanel(0x1A);
            } else {
                name = getMagicNamePtr(magicId);
            }
            cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, name, available);
        }
    }
    return cursorY;
}

/**
 * @brief Render the main stat junction panel (9 stat slots + ability slots).
 *
 * Resolves the GF index for the currently selected stat slot via the
 * available-GF index list (D_801EEDE0), bounded by the available-GF count
 * (-1 when out of range or when showGf is 0). Renders the header stat slot,
 * the 8 remaining stat slots in a 4-row/2-column grid, the four ability
 * junction slots (HP-J, ST-Def-J, Elem-Atk-J, Elem-Def-J), and the
 * elemental/status junction panels.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @param showGf Whether to show GF junction preview info.
 * @return Updated cursor Y position.
 */
s32 renderJunctionStatPanel(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 showGf) {
    s32 i;
    s32 col;
    s32 row;
    s32 xOff;
    s32 gfIdx;
    s32 charIdx;
    s32 statSlot;
    s32 yBase;

    statSlot = ctx->statSlot;
    charIdx = ctx->charIdx;
    if (statSlot < ctx->discCount && statSlot >= 0) {
        gfIdx = D_801EEDE0[statSlot];
    } else {
        gfIdx = -1;
    }
    if (showGf == 0) {
        gfIdx = -1;
    }

    cursorY = renderJunctionSlotDetail(renderCtx, cursorY, x + 0x150, y + 4, 1, 0, charIdx, gfIdx);
    for (i = 1; i < 9; i++) {
        col = (i - 1) / 4;
        row = (i - 1) % 4;
        xOff = col * 160 + 0x150;
        cursorY = renderJunctionSlotDetail(renderCtx, cursorY, x + xOff,
                                           row * 13 + y + 0x11, 0, i, charIdx, gfIdx);
    }

    yBase = y + 4;
    cursorY = renderHpJunctionSlot(renderCtx, cursorY, x + 0x240, yBase, charIdx, gfIdx);
    cursorY = renderStatusDefSlot(renderCtx, cursorY, x + 0x250, yBase, charIdx, gfIdx);
    cursorY = renderElemAtkSlot(renderCtx, cursorY, x + 0x260, yBase, charIdx, gfIdx);
    cursorY = renderElemDefSlot(renderCtx, cursorY, x + 0x270, yBase, charIdx, gfIdx);
    yBase = y;
    y += 4;
    cursorY = renderElemJunctionPanel(renderCtx, cursorY, x + 0xA0, yBase, charIdx, gfIdx);
    return renderStatusJunctionPanel(renderCtx, cursorY, x, yBase, charIdx, gfIdx);
}

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
s32 renderJunctionHeader(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 p1, p2;
    s32 xPos, yPos;
    s32 icon1, icon2;
    s32 seven = 7;

    switch (ctx->unk58 / 5) {
    case 0:
        p1 = renderInnerPanel(0x12);
        p2 = renderInnerPanel(0x13);
        icon1 = 0x7E;
        icon2 = 0x7F;
        break;
    case 1:
        p1 = renderInnerPanel(0x11);
        p2 = renderInnerPanel(0x13);
        icon1 = 0x7C;
        icon2 = 0x7E;
        break;
    case 2:
    case 3:
        p1 = renderInnerPanel(0x11);
        p2 = renderInnerPanel(0x12);
        icon1 = 0x7D;
        icon2 = 0x7C;
        break;
    }

    xPos = x + 0x10;
    yPos = y + 6;
    cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, (u8 *)p1, seven);
    xPos = x + 0x4B;
    cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, (u8 *)p2, seven);

    xPos = x + 8;
    yPos = y + 7;
    cursorY = func_8002FF34(renderCtx, cursorY, icon1, xPos, yPos, g_menuColor);
    xPos = x + 0x41;
    cursorY = func_8002FF34(renderCtx, cursorY, icon2, xPos, yPos, g_menuColor);

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 0xAE;
    cfg->h = 0x16;
    return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

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
 * @param scale Scale factor (on stack); when 0x1000 a scroll-indicator panel is drawn.
 * @param showGf Whether to show GF index info (passed to renderJunctionStatPanel).
 */
void renderJunctionComposite(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y, s32 scale, s32 showGf) {
    s32 result;

    result = func_801EF8D8(renderCtx, cursorY);
    result = renderJunctionStatPanel(ctx, renderCtx, result, x + ctx->unk38, y, showGf);

    if (scale == 0x1000) {
        struct {
            s16 x;
            s16 y;
            s16 w;
            s16 h;
        } rect;

        rect.x = x + 8;
        rect.y = y;
        rect.w = 0x140;
        rect.h = 0x48;
        result = func_801EF800(renderCtx, result, &rect);
    }

    setupStatBorderPanel(renderCtx, result, x, y, scale);
}

/**
 * @brief Render a single magic junction entry (name + icon + quantity).
 *
 * Per-item callback of the magic junction list (registered via
 * func_801EFBB4) for a 4-rows-per-column grid. Reads the character's
 * magic slot col * 4 + row from g_gameState (the character is
 * g_menuDisplayCfg.itemId), skips empty slots (no spell or zero
 * quantity), and renders the spell name colored 7 when free, 1 when
 * currently junctioned (func_801F1CE8, which also earns the 0xC0
 * junction marker icon), or 0 when unavailable per the availability
 * bitmask in g_menuDisplayCfg.dataPtr. The stocked quantity is drawn
 * via drawColorByMenuPalette at a packed (y << 16 | x) position.
 *
 * @param renderCtx Render context.
 * @param cursorY Current draw cursor position (chained through each call).
 * @param col Grid column index.
 * @param row Grid row index (13 px per row).
 * @param xOff Column X offset (on stack).
 * @return Updated draw cursor position.
 */
s32 renderMagicJunctionEntry(s32 renderCtx, s32 cursorY, s32 col, s32 row, s32 xOff) {
    s32 slot;
    s32 color;
    s32 magicId;
    s32 qty;
    s32 mask;
    s32 bit;
    s32 ysum;
    s32 xsum;
    s32 charIdx;
    u8 *namePtr;

    charIdx = g_menuDisplayCfg.itemId;
    slot = col * 4 + row;
    magicId = g_gameState.chars[charIdx].magic[slot].magicId;
    qty = g_gameState.chars[charIdx].magic[slot].quantity;
    mask = g_menuDisplayCfg.dataPtr;
    color = 7;

    if (magicId != 0 && qty != 0) {
        if (func_801F1CE8(charIdx, magicId) != 0) {
            ysum = 7 + g_menuDisplayCfg.y + row * 13;
            xsum = 0x49 + g_menuDisplayCfg.x + xOff;
            cursorY = func_8002FF34(renderCtx, cursorY, 0xC0, xsum, ysum, g_menuColor);
            color = 1;
        }
        bit = 1 << slot;
        if ((mask & bit) == 0) {
            color = 0;
        }
        namePtr = getMagicNamePtr(magicId);
        cursorY = func_801F0FEC(renderCtx, cursorY, 0xC + g_menuDisplayCfg.x + xOff, 7 + g_menuDisplayCfg.y + row * 13, namePtr, color);
        ysum = 7 + g_menuDisplayCfg.y + row * 13;
        xsum = 0x6C + g_menuDisplayCfg.x + xOff;
        cursorY = drawColorByMenuPalette(renderCtx, cursorY, (ysum << 16) | (xsum & 0xFFFF), qty, color);
    }
    return cursorY;
}

/**
 * @brief Set up and render the magic junction list panel.
 *
 * Configures g_menuDisplayCfg for the scrollable magic list (icon 0x4A,
 * 0x78 x 0x40, 4 columns), copies page/scroll/character state from @p ctx
 * (including the ability data pointer into cfg.dataPtr), optionally draws a
 * disc-count indicator via func_801F5F60 when five or more discs are present,
 * renders a header icon via func_8002FF34, draws the list frame via
 * func_801F5F30, and finally registers renderMagicJunctionEntry as the
 * per-item callback via func_801EFBB4.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
void renderMagicListPanel(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    g_menuDisplayCfg.iconType = 0x4A;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.w = 0x78;
    g_menuDisplayCfg.h = 0x40;
    g_menuDisplayCfg.columnCount = 4;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.pageStart = ctx->unk44;
    g_menuDisplayCfg.pageEnd = ctx->unk45;
    g_menuDisplayCfg.scrollOffset = ctx->unk34;
    g_menuDisplayCfg.itemId = ctx->charIdx;
    g_menuDisplayCfg.dataPtr = ctx->dataPtr2;

    if (ctx->discCount >= 5) {
        cursorY = func_801F5F60(renderCtx, cursorY, g_menuColor, 3);
    }

    cursorY = func_8002FF34(renderCtx, cursorY, 0x4D, x + 0x54, y, g_menuColor);
    cursorY = func_801F5F30(renderCtx, cursorY, x + 0x24, y, g_menuColor, ctx->unk44);
    func_801EFBB4(renderCtx, cursorY, renderMagicJunctionEntry);
}

/**
 * @brief Render a single ability list entry with flag-based categorization.
 *
 * Reads ability data from g_menuDisplayCfg.dataPtr (a (id, iconId) byte-pair
 * array). Checks the g_assignedAbilities bitmap for an initial highlight (1 if set,
 * 7 otherwise), renders the icon if @c iconId != 0xFF, then checks 5 ability
 * category masks via func_801F79F8 (1=character, 2=cmd, 4=cmd, 8=cmd,
 * 0x10=GF/party/menu) — each mask sets highlight=1 when the ability matches
 * a specific id range, and finally renders the ability name.
 *
 * @param ctx Render context.
 * @param cursorY Current cursor Y position (also returned, advanced by render calls).
 * @param row Logical row index (combined with col into list index = row*11 + col).
 * @param col Logical column index (also used for vertical offset col*13).
 * @param panelX Panel X position (5th arg, on stack).
 * @return Updated cursorY after rendering.
 */
s32 renderAbilityListEntry(s32 ctx, s32 cursorY, s32 row, s32 col, s32 panelX) {
    u8 *data;
    s32 idx;
    s32 cfgX;
    s32 cfgY;
    s32 colY;
    s32 textY;
    s32 abilityId;
    s32 iconId;
    s32 highlight;
    s32 stringX;

    data = (u8 *)g_menuDisplayCfg.dataPtr;
    if (data != 0) {
        idx = (row * 11) + col;
        cfgX = g_menuDisplayCfg.x;
        cfgY = g_menuDisplayCfg.y;
        if (idx < g_menuDisplayCfg.itemId) {
            colY = col * 13;
            {
                s32 xOff = panelX + 0x16;
                stringX = cfgX + xOff;
            }
            textY = cfgY + 0xA;
            textY = textY + colY;
            abilityId = data[idx * 2];
            iconId = data[(idx * 2) + 1];
            if (g_assignedAbilities[abilityId / 32] & (1 << (abilityId & 0x1F))) {
                highlight = 1;
            } else {
                highlight = 7;
            }
            if (iconId != 0xFF) {
                cursorY = func_8002FF34(ctx, cursorY, iconId + 0xD8, stringX, textY - 2, g_menuColor);
            }
            {
                s32 xOff = panelX + 0x24;
                stringX = cfgX + xOff;
            }
            textY = cfgY + 0xA;
            textY = textY + colY;
            if (func_801F79F8(1) != 0) {
                if (abilityId == 0x18) {
                    highlight = 1;
                }
                if (abilityId == 0x17) {
                    highlight = 1;
                }
            }
            if (func_801F79F8(2) != 0) {
                if (abilityId == 0x14) {
                    highlight = 1;
                }
            }
            if (func_801F79F8(4) != 0) {
                if (abilityId == 0x15) {
                    highlight = 1;
                }
            }
            if (func_801F79F8(8) != 0) {
                if (abilityId == 0x16) {
                    highlight = 1;
                }
            }
            if (func_801F79F8(0x10) != 0) {
                s32 d = abilityId - 0x14;
                if (((u32)d) < 0x13) {
                    if (((u32)d) >= 3) {
                        if (abilityId != 0x18) {
                            if (abilityId != 0x17) {
                                highlight = 1;
                            }
                        }
                    }
                }
            }
            cursorY = func_801F0FEC(ctx, cursorY, stringX, textY, getAbilityName(abilityId), highlight);
        }
    }
    return cursorY;
}

/**
 * @brief Set up and render the ability list panel.
 *
 * Configures g_menuDisplayCfg for the ability list (icon 0x5E, 0xA2 x 0xA0,
 * 11 columns) at offset (x + 0xC6, y + 0x38). Selects the data table and
 * count byte from ctx->unk56 (1 = commands via D_801EEF10/D_801EEF38,
 * 2 = abilities via D_801EEF40/D_801EEF9A, 0 = none), draws the list frame
 * via func_801F5F30, optionally draws a paging indicator via func_801F5F60
 * when the item count is 0xC or more, and registers renderAbilityListEntry
 * as the per-item callback via func_801EFBB4.
 *
 * @param ctx Junction menu context.
 * @param renderCtx Render context.
 * @param cursorY Current cursor Y position.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 */
void renderAbilityListPanel(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 panelX = x + 0xC6;
    s32 type;

    cfg->iconType = 0x5E;
    cfg->iconSubType = 0;
    cfg->x = panelX;
    cfg->w = 0xA2;
    cfg->h = 0xA0;
    cfg->columnCount = 0xB;
    y = y + 0x38;
    cfg->y = y;
    cfg->pageStart = ctx->unk44;
    cfg->pageEnd = ctx->unk45;
    cfg->scrollOffset = ctx->unk34;

    type = ctx->unk56;
    switch (type) {
    case 0:
        cfg->dataPtr = 0;
        break;
    case 1:
        cfg->dataPtr = (s32)D_801EEF10;
        cfg->itemAttr = type;
        cfg->itemId = D_801EEF38;
        break;
    case 2:
        cfg->dataPtr = (s32)D_801EEF40;
        cfg->itemAttr = type;
        cfg->itemId = D_801EEF9A;
        break;
    }

    cursorY = func_801F5F30(renderCtx, cursorY, panelX + 0x28, y, g_menuColor, ctx->unk44);

    if (cfg->itemId >= 0xC) {
        cursorY = func_801F5F60(renderCtx, cursorY, g_menuColor, 3);
    }

    func_801EFBB4(renderCtx, cursorY, renderAbilityListEntry);
}

/**
 * @brief Render the character's 3 equipped-command rows with junction highlighting.
 *
 * Reads the character index from @c ctx[0x43], renders the character title (looked
 * up via @c func_80020EF4 from @c g_charMenuInfo[charIdx].unk12), then loops over the
 * 3 equipped commands (@c g_gameState.chars[charIdx].commands[i]). Each command is
 * drawn with its category icon (@c getAbilityCategory) and name (@c getAbilityName),
 * highlighted (color 1 instead of 7) when the junction slot being edited — queried
 * per-category via @c func_801F79F8(mask) — matches the command id (0x14-0x18). The
 * surrounding panel is closed via @c func_801EF9AC.
 *
 * @param ctx Junction menu context (byte 0x43 = character index).
 * @param renderCtx Render context.
 * @param cursorY Current display-list cursor.
 * @param x Panel X position.
 * @param y Panel Y position (on stack).
 * @return Updated display-list cursor from @c func_801EF9AC.
 */
s32 renderStatRowGrid(u8 *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    /* FIXME: regalloc hack — the inline base-pointer assignment below (a pointer to the
     * whole array, not an element) is what forces g_charMenuInfo's base into the register
     * the original codegen used; a plain pointer or a separate assignment does not match. */
    CharMenuInfo (*cmiTable)[];
    s32 cmdId;
    s32 highlight;
    s32 i;
    u8 *title;
    s32 xPos;
    s32 yPos;
    s32 tmp;

    func_801F79F8(0x10);
    x += 0x18;
    y += 0x48;
    xPos = x + 0x21;
    yPos = y + 9;
    highlight = 7;
    title = func_80020EF4((*(cmiTable = &g_charMenuInfo))[ctx[0x43]].unk12);
    cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, title, highlight);

    for (i = 0; i < 3; i++) {
        highlight = 7;
        yPos = (y + 0x16) + (i * 0xD);
        cmdId = g_gameState.chars[ctx[0x43]].commands[i];

        tmp = func_801F79F8(2);
        if (tmp && cmdId == 0x14) highlight = 1;
        tmp = func_801F79F8(4);
        if (tmp && cmdId == 0x15) highlight = 1;
        tmp = func_801F79F8(8);
        if (tmp && cmdId == 0x16) highlight = 1;
        tmp = func_801F79F8(1);
        if (tmp) {
            if (cmdId == 0x18) highlight = 1;
            if (cmdId == 0x17) highlight = 1;
        }
        tmp = func_801F79F8(0x10);
        if (tmp && (u32)(cmdId - 0x14) >= 3 && cmdId != 0x18 && cmdId != 0x17) highlight = 1;

        if (cmdId != 0) {
            /* FIXME: regalloc hack — xPos is reused here as scratch for the category icon */
            xPos = getAbilityCategory(cmdId) + 0xD8;
            cursorY = func_8002FF34(renderCtx, cursorY, xPos, x + 0x13, yPos - 2, g_menuColor);
            xPos = x + 0x21;
            cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, getAbilityName(cmdId), highlight);
        }
        xPos = x + 7;
        cursorY = func_8002FF34(renderCtx, cursorY, 0x7E, xPos, yPos + 2, g_menuColor);
    }

    cfg->iconType = 0x48;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 0xAE;
    cfg->h = 0x48;
    return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Render the character's ability grid (icon + name per row).
 *
 * Reads the character index from @c ctx[0x43] and renders up to
 * @c g_junctionChars[charIdx].abilityRows ability rows for that character. Each nonzero
 * ability slot draws a category icon (via @c getAbilityCategory) and the
 * ability name (via @c getAbilityName), plus a fixed separator glyph; the
 * running display-list pointer is threaded through @c func_8002FF34 /
 * @c func_801F0FEC and the surrounding panel is closed with @c func_801EF9AC.
 *
 * @param ctx       Junction menu context (byte 0x43 = character index).
 * @param renderCtx Render context.
 * @param cursorY   Current display-list cursor.
 * @param x         Panel X position.
 * @param y         Panel Y position (on stack).
 * @return Updated display-list cursor from @c func_801EF9AC.
 * @note The @c result++/result-- pair is a no-op that pins the accumulator to
 *       the register the original codegen used; it emits no instructions.
 */
s32 renderGfCompatGrid(u8 *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg;
    s32 result;
    s32 gf, y2, i;
    s32 new_var = 7;
    s32 charIdx = ctx[0x43];

    cfg = &g_menuDisplayCfg;
    result = cursorY;
    result++;
    result--;
    x += 0x18;
    y += 0x90;
    i = 0;
    if (g_junctionChars[charIdx].abilityRows != 0) {
        do {
            y2 = y + 9 + i * 13;
            gf = g_gameState.chars[charIdx].abilities[i];
            if (gf != 0) {
                result = func_8002FF34(renderCtx, result, getAbilityCategory(gf) + 0xD8, x + 0x2E, y2 - 2, g_menuColor);
                cursorY = x + 0x3C;
                result = func_801F0FEC(renderCtx, result, cursorY, y2, getAbilityName(gf), new_var);
            }
            result = func_8002FF34(renderCtx, result, 0x7E, x + 0x22, y2 + 2, g_menuColor);
            i++;
        } while (i < g_junctionChars[charIdx].abilityRows);
    }
    cfg->iconType = 0x5E;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 0xAE;
    cfg->h = 0x48;
    return func_801EF9AC(renderCtx, result, 0x1000, g_menuColor);
}

/**
 * @brief Render the character info panels during a character-switch slide.
 *
 * Drives the junction menu's character-switch animation. The slide offset
 * (ctx->slideOffset, animated ±0xF80 -> 0 by junctionMenuUpdate) is eased
 * through the D_801FA3C8 scale table and converted to a pixel offset
 * (0x180 = one panel width). While the slide is active, the outgoing
 * character's panel (ctx->prevCharIdx) is drawn one panel width to the
 * side, then the current character's panel (ctx->charIdx) is drawn at the
 * slid position. Both panels render via func_801F65F0.
 *
 * @param ctx       Junction menu context.
 * @param renderCtx Render context handle.
 * @param cursorY   Draw cursor position, chained through func_801F65F0.
 * @param x         Panel X position.
 * @param y         Panel Y position (on stack).
 */
void renderCharSwitchPanel(JunctionMenuCtx *ctx, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    s16 slide;
    s32 scale;
    s32 idx;
    s32 x2;
    s32 y2;
    u8 chr;
    CharacterData *rec;
    CharMenuInfo *info;

    x2 = x;
    /* Empty loop: scheduling barrier that keeps the slide load below the
       incoming-argument moves (matches the original instruction order). */
    do { } while (0);
    slide = ctx->slideOffset;

    scale = slide;
    idx = abs(scale);
    y2 = y;
    scale = D_801FA3C8[idx / 64];
    if (slide < 0) {
        scale = -scale;
    }

    if (scale != 0) {
        scale = scale * 0x180 / 0x1000;
        x2 += scale;
        if (slide > 0) {
            x2 -= 0x180;
        } else {
            x2 += 0x180;
        }
        chr = ctx->prevCharIdx;
        rec = &D_80077808[chr];
        info = &g_charMenuInfo[chr];
        cursorY = func_801F65F0(renderCtx, cursorY, x2, y2, rec, info);
        x2 = x + scale;
        y2 = y;
    }
    chr = ctx->charIdx;
    rec = &D_80077808[chr];
    info = &g_charMenuInfo[chr];
    func_801F65F0(renderCtx, cursorY, x2, y2, rec, info);
}

/**
 * @brief Render a character name and status bar in the junction menu.
 *
 * Sets up g_menuDisplayCfg dimensions, optionally renders the character
 * name (looked up via getCharName from the character's ID in g_gameState),
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
            u8 *namePtr = getCharName(g_gameState.chars[charIdx].characterId);
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
void initJunctionGfTable(void) {
    u32 availMask;
    s32 availCount;
    s32 i, j;
    s32 abilityCount;
    JunctionGfEntry *gf;
    u8 level;
    s32 outIdx;

    availMask = getGfAvailabilityMask();
    availCount = popcount(availMask);

    /* Phase 1: init each GF entry, OR ability flags from learned slots. */
    for (i = 0; i < GF_COUNT; i++) {
        gf = &g_junctionGfTable[i];
        gf->charIdx = 0xFF;
        gf->abilityFlags = 0;
        level = g_battleChars.levelEntries[i].level;
        gf->cmdSlotCount = 0;
        gf->ablSlotCount = 0;
        gf->maxAbilitySlots = 2;
        gf->level = level;

        abilityCount = func_800369CC(i, D_801EEC50, 0);
        for (j = 0; j < abilityCount; j++) {
            s32 slot = D_801EEC50[j].slotIndex;
            if (slot < 20) {
                gf->abilityFlags |= D_8007CEE4[slot].flagWord >> 8;
            }
        }
    }

    /* Phase 2: derive slot counts from ability bits.
     * NOTE: outIdx is reused here as a scratch holding abilityFlags before
     * being reset to 0 for phase 4. The reuse pins outIdx to a specific
     * register early, which gcc 2.7.2 carries through to phase 4 — this
     * matches the original code and is required for byte-match. */
    for (gf = g_junctionGfTable, i = 0; i < GF_COUNT; i++, gf++) {
        outIdx = gf->abilityFlags;
        gf->ablSlotCount = 0;
        gf->cmdSlotCount = 0;
        gf->maxAbilitySlots = 2;
        if (outIdx & 0x800)   gf->ablSlotCount = 1;
        if (outIdx & 0x2000)  gf->ablSlotCount = 2;
        if (outIdx & 0x4000)  gf->ablSlotCount = 4;
        if (outIdx & 0x1000)  gf->cmdSlotCount = 1;
        if (outIdx & 0x8000)  gf->cmdSlotCount = 2;
        if (outIdx & 0x10000) gf->cmdSlotCount = 4;
        if (outIdx & 0x20000) gf->maxAbilitySlots = 3;
        if (outIdx & 0x40000) gf->maxAbilitySlots = 4;
    }

    /* Sentinel entry at index GF_COUNT (= D_801EEDD0). */
    g_junctionGfTable[GF_COUNT].abilityFlags = 0;
    g_junctionGfTable[GF_COUNT].charIdx = 0xFF;
    g_junctionGfTable[GF_COUNT].cmdSlotCount = 0;
    g_junctionGfTable[GF_COUNT].ablSlotCount = 0;
    g_junctionGfTable[GF_COUNT].maxAbilitySlots = 0;

    /* Phase 3: map juncted GFs back to characters. */
    for (i = 0; i < CHARACTER_COUNT; i++) {
        u32 junctedGfs = g_junctionChars[i].junctedGfs;
        for (j = 0; j < GF_COUNT; j++) {
            if (junctedGfs & (1 << j)) {
                g_junctionGfTable[j].charIdx = i;
            }
        }
    }

    /* Phase 4: build available GF index list (D_801EEDE0). */
    outIdx = 0;
    for (i = 0; outIdx < availCount; i++) {
        if (availMask & (1 << i)) {
            D_801EEDE0[outIdx++] = i;
        }
    }
}

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
