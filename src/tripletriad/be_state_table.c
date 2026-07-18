#include "common.h"
#include "tripletriad.h"
#include "tripletriad/be_object1.h"
#include "tripletriad/be_object1b.h"
#include "tripletriad/be_object3.h"
#include "tripletriad/be_object3b.h"
#include "tripletriad/be_object4.h"

/**
 * @brief Top-level Triple Triad state-handler table, indexed by
 *        `g_tripleTriadState - 1` (see @ref TripleTriadState).
 *
 * The dispatch loop in @ref tripleTriadMainLoop calls the entry for the
 * current state until it returns a non-zero object list. Sits in its own
 * translation unit so the linker places it at the head of the overlay's
 * data region (offset 0xC58C), directly after the last code byte — the
 * dispatcher's `- 1` folds into the relocation (`%lo(sym - 4)`), which is
 * why the raw disassembly appears to index a table based one word earlier.
 */
TripleTriadStateFn g_tripleTriadStateHandlers[5] = {
    initTripleTriadRenderList,                     /* TT_STATE_INIT */
    initTripleTriadScripts,                        /* TT_STATE_SCRIPT */
    initTripleTriadUpdateList,                     /* TT_STATE_PLAY */
    setupTripleTriadCardClaim,                     /* TT_STATE_CARD_CLAIM */
    tripleTriadRestartScript,                      /* TT_STATE_RESTART */
};
