#include "common.h"
#include "tripletriad.h"
#include "tripletriad/tt_claim.h"

/**
 * @brief Latch the field-VM encounter parameters into the Triple Triad globals.
 *
 * Reads @ref D_80082C90 (the @ref EncounterParams block the field VM fills in
 * before launching a card game) and stores its pieces where the engine reads
 * them for the whole session:
 *  - @ref g_tripleTriadRules = @c encounterPtr (for card games the word is the
 *    TT_RULE_* bitmask rather than a pointer);
 *  - @ref D_801A2C44 = @c field_04 (the trade-rule selector);
 *  - @ref D_801A2C70 per-player layout types: a negative rules word selects
 *    type 2 for both players, bit 30 selects the offset-hand layout (type 3)
 *    for both, otherwise player 0 gets type 3 and player 1 type 2.
 *
 * First initializer run by @ref initTripleTriad; sits alone ahead of
 * tt_main's code in the overlay image.
 */
void func_800981BC(void) {
    s32 rules = D_80082C90.encounterPtr;

    g_tripleTriadRules = rules;
    D_801A2C44 = D_80082C90.field_04;
    if (rules < 0) {
        D_801A2C70[0] = 2;
        D_801A2C70[1] = 2;
    } else if (rules & 0x40000000) {
        D_801A2C70[0] = 3;
        D_801A2C70[1] = 3;
    } else {
        D_801A2C70[0] = 3;
        D_801A2C70[1] = 2;
    }
}
