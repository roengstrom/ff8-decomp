#include "common.h"
#include "gamestate.h"
#include "input/button_remap.h"

/* --- Private functions --- */

static s32 remapButtonIndex(s32 index);

/**
 * @brief Translate pad button bits through the player's custom button mapping.
 *
 * Returns @p bitmask unchanged unless the Config menu's controller setting is
 * Customize (CONFIG_CONTROLLER). Then each of the 12 remappable buttons in the
 * low bits becomes the button its g_gameState.config.buttons entry names
 * (1-based; 0 drops it), and the D-pad bits (0xF000) pass through.
 *
 * @param bitmask Pad button bits.
 * @return The translated button bits.
 */
u16 applyButtonRemapTranslation(u16 bitmask) {
    s32 top;
    u8* table;
    s32 result;
    s32 i;

    if (!(g_gameState.config.flags & CONFIG_CONTROLLER)) {
        return bitmask;
    }

    top = bitmask & 0xF000;
    table = g_gameState.config.buttons;
    result = 0;
    bitmask &= 0xFFF;

    for (i = 0; i < 12; i++) {
        if ((bitmask >> i) & 1) {
            s32 remap = *table++;
            if (remap) {
                result |= 1 << (remap - 1);
            }
        } else {
            table++;
        }
    }

    return top | result;
}


/**
 * @brief Remap a button index through the gamepad button table.
 *
 * If CONFIG_CONTROLLER is set and @p index is within the 12-button range,
 * returns the remapped button value minus 1. Otherwise returns the index
 * unchanged.
 *
 * @param index Button index to remap.
 * @return Remapped index, or original if remapping inactive or out of range.
 */
static s32 remapButtonIndex(s32 index) {
    if ((g_gameState.config.flags & CONFIG_CONTROLLER) && index < 12) {
        return g_gameState.config.buttons[index] - 1;
    }
    return index;
}


/**
 * @brief Reverse-lookup a logical button index to its physical button.
 *
 * Inverse of remapButtonIndex. Given a logical index, searches the
 * gamepad remap table for which physical button maps to it.
 * Returns the index unchanged if remapping is inactive or out of range,
 * or -1 if no physical button maps to the given logical index.
 *
 * @param index Logical button index to reverse-lookup.
 * @return Physical button index, original index if inactive, or -1 if not found.
 */
s32 reverseButtonRemap(s32 index) {
    u8* table;
    s32 i;
    u8 remap;

    if (!(g_gameState.config.flags & CONFIG_CONTROLLER) || index >= 12) {
        return index;
    }

    table = g_gameState.config.buttons;
    index++;

    for (i = 0; i < 12; i++) {
        remap = *table++;
        if (remap == index) {
            return i;
        }
    }

    return -1;
}


/** @brief Initialize the button remap: nothing to do, as the mapping lives in g_gameState.config. */
void initButtonRemap(void) {
}
