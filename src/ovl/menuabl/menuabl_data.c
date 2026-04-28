#include "common.h"

/**
 * @brief GF kind table indexed by GF id.
 *
 * Bit per GF: 0 = "summon" GF (Quezacotl..Pandemona), 1 = "guardian" GF
 * (Cerberus..Eden); 4 trailing zeros for unused slots. Used by
 * @c func_801E2990 to filter visible GFs against the party-lock bit.
 */
const u8 D_801E3D70[20] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,    /* GF 0..8 (Quezacotl..Pandemona) */
    1, 1, 1, 1, 1, 1, 1,          /* GF 9..15 (Cerberus..Eden) */
    0, 0, 0, 0,                    /* unused slots */
};
