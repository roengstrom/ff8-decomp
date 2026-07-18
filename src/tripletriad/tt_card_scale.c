#include "common.h"
#include "psxsdk/libgte.h"
#include "tripletriad/tt_game.h"

/**
 * @brief Unit card scale (GTE ONE in x/y): the base scale vector
 *        @ref transformCardEffect starts each card effect from.
 *
 * Sits in its own translation unit so the linker places it between
 * tt_game's and tt_script's jump tables (overlay .rodata offset 0x154).
 */
const VECTOR g_cardScaleVec = {0x1000, 0x1000, 0, 0};
