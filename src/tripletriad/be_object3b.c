#include "common.h"
#include "item.h"
#include "tripletriad/be_object1.h"
#include "tripletriad/be_object3.h"
#include "tripletriad/be_object3b.h"
#include "tripletriad/be_object4.h"

/**
 * @brief Per-frame card-claim transition controller.
 *
 * Drives the post-game card-claim sequence as a five-phase state machine on
 * @c node->state:
 *  - 0: fade to black (@c startFadeToBlack), wait 0xF frames, then branch on the
 *       claim selector @c g_sweepTarget to phase 1 (>= 0), 2 (== -1), or 3 (< -1).
 *  - 1: spawn the claim handler chosen by the rule/mode selector @c D_801A2C44
 *       (@c replayHandMoves / @c runKeepCardSelect / @c runAiCaptureSelect / @c runOpponentSideSweep)
 *       onto the @c D_801D42F8 pool for seat @c D_801D30FC; wait for it to signal
 *       via @c D_801D444C, then advance to phase 2. Modes 0 and >4 spawn nothing.
 *  - 2: spawn the capture/cleanup handler @c runCaptureCleanupSweep for acting seat
 *       @c g_claimSeat; wait for @c g_sweepDone, then advance to phase 3.
 *  - 3: poll the message gate @c func_800A20F4(6); stage result 6 (init / accept)
 *       or 2 (gate result 0), or keep waiting (gate < 0). Advance to phase 4.
 *  - 4: fade to white (@c startFadeToWhite), wait 0xF frames, stage @c result into
 *       @c g_tripleTriadState and return 2.
 *
 * @param node Controller state node.
 * @return 0 while the sequence is still running, 2 once it has finished.
 */
s32 updateClaimController(ClaimCtrlNode *node)
{
    ScriptStateNode *spawned;
    s32 mode;
    s32 r;
    u8 ss;
    u8 seat;
    u8 actingSeat;
    u8 nextState;
    short modeMax;

    while (1) {
        switch (node->state) {
        case CLAIM_PHASE_FADE_IN:
            if (node->subState == 0) {
                startFadeToBlack(0xF);
            }
            node->subState++;
            if ((node->subState & 0xFF) < 0xF) {
                return 0;
            }
            if (g_sweepTarget >= 0) {
                nextState = CLAIM_PHASE_CLAIM;
            } else if (g_sweepTarget == -1) {
                nextState = CLAIM_PHASE_CLEANUP;
            } else {
                node->state = CLAIM_PHASE_GATE;
                node->subState = 0;
                break;
            }
            node->state = nextState;
            node->subState = 0;
            break;
        case CLAIM_PHASE_CLAIM:
            if (node->subState == 0) {
                mode = D_801A2C44;
                modeMax = 4;
                if (mode == 3) {
                    goto spawn1080;
                }
                if (mode >= modeMax) {
                    if (mode == 4) {
                        goto spawn1260;
                    }
                    goto fieldSetup;
                }
                if (mode == 0) {
                    goto fieldSetup;
                }
                if (g_sweepTarget < 5) {
                    if (D_801A2C70[D_801D30FC] < 3) {
                        spawned = (ScriptStateNode *)allocObjNode(D_801D42F8, (ObjNodeFn)runKeepCardSelect);
                    } else {
                        spawned = (ScriptStateNode *)allocObjNode(D_801D42F8, (ObjNodeFn)runAiCaptureSelect);
                    }
                    goto fieldSetup;
                }
                goto spawn1260;
            spawn1080:
                spawned = (ScriptStateNode *)allocObjNode(D_801D42F8, (ObjNodeFn)replayHandMoves);
                goto fieldSetup;
            spawn1260:
                spawned = (ScriptStateNode *)allocObjNode(D_801D42F8, (ObjNodeFn)runOpponentSideSweep);
            fieldSetup:
                seat = D_801D30FC;
                spawned->state = 0;
                spawned->field0D = 0;
                spawned->field0E = seat;
                D_801D444C = 0;
                node->subState++;
            }
            if (D_801D444C == 0) {
                return 0;
            }
            node->state = CLAIM_PHASE_CLEANUP;
            node->subState = 0;
            break;
        case CLAIM_PHASE_CLEANUP:
            if (node->subState == 0) {
                spawned = (ScriptStateNode *)allocObjNode(D_801D42F8, (ObjNodeFn)runCaptureCleanupSweep);
                actingSeat = g_claimSeat;
                spawned->state = 0;
                spawned->field0D = 0;
                spawned->field0E = actingSeat;
                g_sweepDone = 0;
                node->subState++;
            }
            if (g_sweepDone == 0) {
                return 0;
            }
            node->state = CLAIM_PHASE_GATE;
            node->subState = 0;
            break;
        case CLAIM_PHASE_GATE:
            if (node->subState == 0) {
                node->result = 6;
            } else {
                r = func_800A20F4(6);
                switch (r) {
                case 0:
                    func_800A2054(6);
                    node->result = 2;
                    break;
                case 1:
                    func_800A2054(6);
                    break;
                default:
                    return 0;
                }
            }
            node->state = CLAIM_PHASE_FADE_OUT;
            node->subState = 0;
            break;
        case CLAIM_PHASE_FADE_OUT:
            if (node->subState == 0) {
                startFadeToWhite(0xF);
            }
            ss = node->subState;
            node->subState = ss + 1;
            if (ss < 0xF) {
                return 0;
            }
            g_tripleTriadState = node->result;
            return 2;
        }
    }
}

/**
 * @brief Card-claim setup: choose the claim amount and build the hand display.
 *
 * Run once when the post-game card-claim flow starts. It:
 *  1. Clears @c D_801D4454 and picks the acting seat @c g_claimSeat (the player
 *     whose hand uses the non-offset layout, @c D_801A2C70[seat] < 3).
 *  2. Computes the claim selector @c g_sweepTarget (default -1 = capture-only). Unless
 *     the @c TT_RULE_NO_CLAIM rule bit is set, the rule/mode @c D_801A2C44 maps to:
 *       1 → claim one card; 2 → claim 2 per owned card on the board (offset from -10);
 *       3 → claim none (0); 4 → claim five. Seat 2 (none) leaves it at -1.
 *  3. If @c g_sweepTarget < 0, returns every card in the acting seat's hand to its owner
 *     (@c markItemPresent), stages result 6 into @c g_tripleTriadState, and returns 0.
 *  4. Otherwise initializes the @c D_801D42F8 handler pool, spawns the claim controller
 *     @c updateClaimController, fills the @c g_activeCardObjs display-object array with both five-card
 *     hands (positions/sort keys mirrored per seat), spawns the board renderer
 *     @c updateClaimBoard and @c reloadClaimBuffer, and returns the pool.
 *
 * @return 0 on the no-claim path, otherwise the @c D_801D42F8 pool pointer.
 */
s32 setupTripleTriadCardClaim(void)
{
    s32 i;
    s32 col;
    s32 xpos;
    s32 scratch[2];   /* reserves the original 8-byte stack slot (frame is 0x28); purpose unclear */
    ActiveObj *cell;
    ScriptStateNode *node;

    D_801D4454 = 0;
    if (D_801A2C70[0] < 3) {
        g_claimSeat = 0;
    } else if (D_801A2C70[1] < 3) {
        g_claimSeat = 1;
    }
    g_sweepTarget = -1;
    if (!(g_tripleTriadRules & TT_RULE_NO_CLAIM)) {
        switch (D_801A2C44) {
        case 0:
            break;
        case 1:
            if (D_801D30FC != 2) {
                g_sweepTarget = 1;
            }
            break;
        case 2:
            if (D_801D30FC != 2) {
                g_sweepTarget = -10;
                for (i = 0; i < 10; i++) {
                    if ((g_tripleTriadCardHands[i].initFlags & 1) == D_801D30FC) {
                        g_sweepTarget += 2;
                    }
                }
            }
            break;
        case 3:
            g_sweepTarget = 0;
            break;
        case 4:
            if (D_801D30FC != 2) {
                g_sweepTarget = 5;
            }
            break;
        }
    }
    if (g_sweepTarget < 0) {
        for (i = 0; i < 5; i++) {
            markItemPresent(D_801A2C48[g_claimSeat][i]);
        }
        g_tripleTriadState = TT_STATE_EXIT;
        return 0;
    }
    initObjList(D_801D42F8, g_claimSetupPool, 0x14, 4);
    node = (ScriptStateNode *)allocObjNode(D_801D42F8, (ObjNodeFn)updateClaimController);
    cell = g_activeCardObjs;
    node->state = 0;
    node->field0D = 0;
    for (i = 0; i < 2; i++) {
        for (col = 0, xpos = -0x80; col < 5; col++) {
            cell->cardId = D_801A2C48[i][col];
            cell->flags = 1;
            cell->rotX = 0;
            cell->rotY = 0;
            cell->rotZ = 0;
            cell->baseX = xpos;
            if (i != 0) {
                cell->baseY = 0x28;
            } else {
                cell->baseY = -0x28;
            }
            cell->baseZ = 0x200;
            if (i == 0) {
                cell->sort = col + 5;
            } else {
                cell->sort = 9 - col;
            }
            cell->fieldA = i;
            cell->field8 = 1;
            cell->field9 = 0;
            cell++;
            xpos += 0x40;
        }
    }
    allocObjNode(D_801D42F8, (ObjNodeFn)updateClaimBoard);
    allocObjNode(D_801D42F8, (ObjNodeFn)reloadClaimBuffer);
    return (s32)D_801D42F8;
}

/**
 * @brief Unconditional hang.
 *
 * @note Not referenced by the @c g_tripleTriadStateHandlers state-handler table or any caller —
 *       appears to be an unused/stub state that simply spins forever.
 */
void hangForever(void) {
    while (1) {}
}

/**
 * @brief Triple Triad state-5 handler (the @c TT_STATE_RESTART entry of
 *        @c g_tripleTriadStateHandlers).
 *
 * Sets the next state to @c TT_STATE_SCRIPT and returns 0 so the
 * state-dispatch loop keeps running.
 */
s32 tripleTriadRestartScript(void) {
    g_tripleTriadState = TT_STATE_SCRIPT;
    return 0;
}
