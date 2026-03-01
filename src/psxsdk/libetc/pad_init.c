#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad_init", PadInit);

/**
 * @brief Reads the current state of all controller buttons.
 *
 * Polls the controller hardware via PAD_dr() and returns the button state.
 * The raw pad data is inverted so that pressed buttons are represented by
 * set bits (1 = pressed, 0 = released). This is the simplified pad reading
 * API used with PadInit (not the more advanced InitPAD/StartPAD interface).
 *
 * @return Bitmask of currently pressed buttons (active-high).
 */
u32 PadRead(void) {
    extern u32 D_8008A6D0;
    PAD_dr();
    return ~D_8008A6D0;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad_init", PadStop);
