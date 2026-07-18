#ifndef TRIPLETRIAD_BE_OBJECT3B_H
#define TRIPLETRIAD_BE_OBJECT3B_H

#include "common.h"
#include "tripletriad.h"

/* Declarations for tt_claim.c (the post-game card-claim transition
   controller and its setup / restart entry points). */

/* ───────────────────────────── Public ──────────────────────────────────── */

/* Public prototypes — Triple Triad state-handler entry points (dispatched by
   address from @c g_tripleTriadStateHandlers). */
extern u8  *setupTripleTriadCardClaim(void);
extern u8  *tripleTriadRestartScript(void);
extern u8  *initTripleTriadScripts(void); /**< TT_STATE_SCRIPT handler (tt_script.c). */
extern void hangForever(void);

/* ───────── Private (only used in tt_claim.c; may move into the .c) ────── */

/* Private enums / defines / consts */

/** @brief Phase of the card-claim sequence (@c ClaimCtrlNode.state in @ref updateClaimController). */
enum TriadClaimPhase {
    CLAIM_PHASE_FADE_IN  = 0,  /**< Fade to black, then branch on the claim selector. */
    CLAIM_PHASE_CLAIM    = 1,  /**< Spawn and run the chosen claim handler. */
    CLAIM_PHASE_CLEANUP  = 2,  /**< Spawn and run the capture/cleanup sweep. */
    CLAIM_PHASE_GATE     = 3,  /**< Poll the confirm message gate; stage the result. */
    CLAIM_PHASE_FADE_OUT = 4   /**< Fade to white, then finish (return 2). */
};

/* Private typedefs */

/**
 * @brief Per-frame state node for the card-claim transition controller.
 *
 * Allocated from a @c allocObjNode pool. @c state at 0x0C selects the phase and
 * @c result at 0x10 carries the outcome code (2 or 6) staged into @c g_tripleTriadState
 * when the whole claim sequence finishes.
 */
typedef struct {
    /* 0x00 */ u8  pad00[0x0C];
    /* 0x0C */ u8  state;     /**< Phase: 0 warmup, 1/2 spawn handlers, 3 poll gate, 4 fade-out. */
    /* 0x0D */ u8  subState;  /**< Per-phase frame counter / sub-step. */
    /* 0x0E */ u8  pad0E[2];
    /* 0x10 */ s32 result;    /**< Outcome code (2 or 6) staged into g_tripleTriadState on completion. */
} ClaimCtrlNode;

/* Private data */
extern ObjList D_801D42F8[];  /**< Card-claim handler object pool. */

#endif /* TRIPLETRIAD_BE_OBJECT3B_H */
