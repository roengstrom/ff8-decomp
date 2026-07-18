#include "common.h"
#include "tripletriad.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "tripletriad/tt_main.h"
#include "tripletriad/tt_match.h"
#include "tripletriad/tt_game.h"
#include "tripletriad/tt_ui.h"
#include "gamestate.h"

/* Private prototypes (functions local to this translation unit). */
s32 matchFlowHandler(HandlerNode *ctl);
s32 updateCardObjects(s32 node);
s32 drawBoardElements(void);
s32 updateChildList(CallbackNode *node);
s32 drawScoreDigits(void);

/* Imported from tt_ui.c (called only here). */
extern void func_800A247C(s32 mode);

/**
 * @brief Per-frame Triple Triad match controller (an update-list callback).
 *
 * Runs the post-game flow as a state machine over @c ctl->state (see
 * @ref MatchFlowState): each call advances until it must wait on an async
 * event (input, a sub-handler, or an animation), then returns 0 so the next
 * frame re-enters. An unrecognized state hangs intentionally.
 *
 * @param ctl The match's handler node.
 * @return Always 0.
 */
s32 matchFlowHandler(HandlerNode *ctl) {
    s32 acc;
    while (1) {
        switch (ctl->state) {
            case MATCH_FLOW_INIT: {
                if (ctl->counter == 0) {
                    HandlerNode *sub = (HandlerNode *)allocObjNode(D_801D3028, (ObjNodeFn)cardFlipHandler);
                    sub->state = CARD_FLIP_INIT;
                    sub->counter = 0;
                    g_cardFlipPhase = -1;
                    D_801D3340[1].field2 = 0;
                    D_801D3340[2].field2 = 0;
                    ctl->counter++;
                }
                if (g_cardFlipPhase < 0) return 0;
                ctl->state = MATCH_FLOW_TURN;
                ctl->counter = 0;
                break;
            }
            case MATCH_FLOW_TURN: {
                if (ctl->counter == 0) {
                    u8 playerType = D_801A2C70[g_cardFlipPhase];
                    switch (playerType) {
                        case 0: ctl->subHandler = (void *)spawnCardSelectCursor(g_cardFlipPhase, 0); break;
                        case 1: ctl->subHandler = (void *)spawnCardSelectCursor(g_cardFlipPhase, 1); break;
                        case 2: ctl->subHandler = (void *)spawnCardSelectCursor(g_cardFlipPhase, 2); break;
                        case 3: ctl->subHandler = (void *)spawnAiTurn(g_cardFlipPhase); break;
                    }
                    ctl->counter++;
                    return 0;
                }
                if (updateObjectList(ctl->subHandler) == 0) {
                    ctl->state = MATCH_FLOW_RULES;
                    ctl->counter = 0;
                    break;
                }
                return 0;
            }
            case MATCH_FLOW_RULES: {
                if (anyCardEffectActive()) return 0;
                if (ctl->counter == 0) {
                    u8 rules = applyCardRules(&g_tripleTriadBoard, 1);
                    ctl->rulesFlags = rules;
                    if (rules & TT_RESULT_COMBO_MASK) {
                        s32 row, col;
                        acc = 0; /* combo cell-mask */
                        if (rules & TT_RESULT_SAME_FIRED) {
                            spawnGradientFade(acc);
                            acc = TT_CELL_SAME_MATCHED;
                        } else if (rules & TT_RESULT_PLUS_FIRED) {
                            spawnGradientFade(1);
                            acc = TT_CELL_PLUS_COMBO;
                        }
                        for (row = 1; row <= 3; row++) {
                            for (col = 1; col <= 3; col++) {
                                if (g_tripleTriadBoard.cells[row][col].flags & acc)
                                    setCardEntityType(g_tripleTriadBoard.cells[row][col].entityIdx, CARD_FX_FLASH);
                            }
                        }
                        playTriadSfx(TT_HOLD_FRAMES_RULE);
                    }
                    ctl->counter++;
                } else {
                    ctl->retryFlag = 1;
                    resolveCaptures(&g_tripleTriadBoard);
                    ctl->state = MATCH_FLOW_CHAIN;
                    ctl->counter = 0;
                }
                break;
            }
            case MATCH_FLOW_CHAIN: {
                if (anyCardEffectActive()) return 0;
                if (ctl->rulesFlags == 0) {
                    ctl->state = MATCH_FLOW_TURN_END;
                    ctl->counter = 0;
                    break;
                }
                {
                    u8 next = applyCardRules(&g_tripleTriadBoard, ctl->rulesFlags);
                    ctl->rulesFlags = next;
                    if (next != 0 && ctl->retryFlag != 0) spawnGradientFade(5);
                    resolveCaptures(&g_tripleTriadBoard);
                }
                break;
            }
            case MATCH_FLOW_TURN_END: {
                s32 row;
                acc = 0; /* empty-cell count */
                for (row = 1; row <= 3; row++) {
                    s32 col;
                    for (col = 1; col <= 3; col++) {
                        if (!(g_tripleTriadBoard.cells[row][col].flags & TT_CELL_OCCUPIED))
                            acc++;
                    }
                }
                row = acc; /* reuse the loop counter for the count test */
                if (row == 0) ctl->state = MATCH_FLOW_TALLY;
                else {
                    g_cardFlipPhase ^= 1;
                    ctl->state = MATCH_FLOW_TURN;
                }
                ctl->counter = 0;
                break;
            }
            case MATCH_FLOW_TALLY: {
                ctl->counter++;
                if (ctl->counter == 1) {
                    u8 cnt[2];
                    s32 i;
                    cnt[1] = 0;
                    cnt[0] = 0;
                    for (i = 0; i < 10; i++) cnt[g_tripleTriadCardHands[i].initFlags & TT_OWNER_MASK]++;
                    if (cnt[0] > cnt[1])      D_801D30FC = 0;
                    else if (cnt[1] > cnt[0]) D_801D30FC = 1;
                    else                      D_801D30FC = TT_WINNER_DRAW;
                }
                if ((u8)ctl->counter < TT_HOLD_FRAMES_TALLY) return 0;
                ctl->state = MATCH_FLOW_RESULT;
                ctl->counter = 0;
                break;
            }
            case MATCH_FLOW_RESULT: {
                if (ctl->counter == 0) {
                    s32 mode;
                    /* result-jingle mode: 4 = draw, 3 = lost; if won, run setup(3) then play mode 2 */
                    if (D_801D30FC == TT_WINNER_DRAW) mode = 4;
                    else if (D_801A2C70[D_801D30FC] >= 3) mode = 3;
                    else { mode = 3; func_800A247C(mode); mode = 2; }
                    g_resultSfxHandle = spawnGradientFade(mode);
                    ctl->counter++;
                }
                if (g_padPressed[2] & PAD_UP) {
                    if (g_resultSfxHandle != 0) { setNodeDoneFlag(g_resultSfxHandle, 1); g_resultSfxHandle = 0; }
                    return 0;
                }
                if (g_padPressed[2] == 0) return 0;
                func_800A2054(3);
                if (D_801D30FC == TT_WINNER_DRAW && (g_tripleTriadRules & TT_RULE_SUDDEN_DEATH)) {
                    if (g_resultSfxHandle != 0) setNodeDoneFlag(g_resultSfxHandle, 1);
                    initCardHands();
                    g_cardFlipPhase ^= 1;
                    ctl->state = MATCH_FLOW_TURN;
                } else {
                    ctl->state = MATCH_FLOW_FADE;
                }
                ctl->counter = 0;
                break;
            }
            case MATCH_FLOW_FADE: {
                if (ctl->counter == 0) startFadeToWhite(TT_HOLD_FRAMES_FADE);
                ctl->counter++;
                if ((u8)ctl->counter < TT_HOLD_FRAMES_FADE) return 0;
                if (g_resultSfxHandle != 0) setNodeDoneFlag(g_resultSfxHandle, 1);
                {
                    TripleTriadData *inv = getTripleTriadData();
                    /* do/while(0) pins each bump after its D_80082C9C store — a
                       bare ++ lets gcc hoist the load ahead of the store. */
                    if (D_801D30FC == TT_WINNER_DRAW) {
                        D_80082C9C = D_801D30FC;
                        do { inv->draws++; } while (0);
                    } else if (D_801A2C70[D_801D30FC] == 3) {
                        D_80082C9C = TT_RESULT_DEFEAT;
                        do { inv->defeats++; } while (0);
                    } else {
                        D_80082C9C = TT_RESULT_VICTORY;
                        do { inv->victories++; } while (0);
                    }
                    g_tripleTriadState = TT_STATE_CARD_CLAIM;
                }
                return 0;
            }
        }
    }
}

/**
 * @brief Per-frame update-list callback that drives the card-object list.
 *
 * Forwards to @c processCardObjects, which updates the card-object list. @p node
 * is passed through but unused. Returns 0 so the update list keeps iterating.
 *
 * @param node Update-list node for this callback (forwarded, unused).
 * @return Always 0.
 */
s32 updateCardObjects(s32 node) {
    processCardObjects(node);
    return 0;
}

/**
 * @brief Render each board cell's element icon, then queue the back-buffer flip.
 *
 * Per-frame renderer over the 3x3 play area: for each cell whose element is set,
 * it emits a textured sprite of that element's icon (its UV scrolling each frame)
 * and links it into the ordering table. Finishes by queueing the back-buffer's
 * load for the next vblank.
 *
 * @return Always 0.
 */
s32 drawBoardElements(void) {
    TripleTriadCellPrim *prim = (TripleTriadCellPrim *)g_primCursor;
    s32 row = 1;
    /* Cells are addressed by byte offset (mirroring the original's hand-coded
       walk) and read through TripleTriadBoardSlot once the address is formed. */
    s32 boardBase = (s32)&g_tripleTriadBoard;
    s32 pixelY = 40;
    s32 rowByteOffset = TT_ROW_BYTES;

    for (; row <= 3; row++) {
        s32 col = 1;
        s32 pixelX = 120;
        s32 colByteOffset = rowByteOffset + TT_SLOT_BYTES;

        for (; col <= 3; col++) {
            TripleTriadBoardSlot *cell = (TripleTriadBoardSlot *)(colByteOffset + boardBase);
            u8 element = cell->element;

            if (element != 0) {
                s32 bit = 0;
                s32 mask = cell->element;
                s32 shifted;

                /* lowest set bit = the cell's element index (0..7) */
                while (1) {
                    shifted = mask >> bit;
                    if (shifted & 1) break;
                    bit++;
                    if (bit >= 8) break;
                    shifted = mask >> bit;
                }

                /* DR_TPAGE + SPRT: a 15x15 element icon from a 4-wide texture
                   atlas, neutral tint, UV scrolled by the frame counter. */
                prim->tag      = 0x05000000;
                prim->tpageCmd = 0xE100060C;
                prim->clut     = (bit + 0xE1) << 6;
                prim->sprtCmd  = 0x66808080;
                prim->x0       = pixelX;
                prim->y0       = pixelY;
                prim->u0       = ((bit % 4) << 6) + ((g_tripleTriadFrameCount << 2) & 0x30);
                prim->v0       = ((bit / 4) << 4) + 0x10;
                prim->h        = 15;
                prim->w        = 15;

                AddPrim(&g_otBase[27], prim);
                prim++;
            }

            pixelX += 64;
            colByteOffset += TT_SLOT_BYTES;
        }

        pixelY += 64;
        rowByteOffset += TT_ROW_BYTES;
    }

    g_primCursor = prim;
    queueLoadImage(&g_drawEnvs[g_drawBufferIndex ^ 1].clip, &g_tripleTriadBgTim0[0x14]);
    return 0;
}

/**
 * @brief Update the child list at @p node->listPtr; return 2 if it is empty, else 0.
 *
 * Runs @c updateObjectList on the node's child list and returns @c 2 once that
 * list is empty, otherwise @c 0.
 *
 * @note Not called from any decompiled code yet.
 */
s32 updateChildList(CallbackNode *node) {
    return (updateObjectList(node->listPtr) == 0) << 1;
}

/**
 * @brief Tally each player's cards and draw the two end-of-game score digits.
 *
 * Counts each player's captured cards, then emits one digit sprite per player at
 * its fixed screen position.
 *
 * @return Always 0.
 */
s32 drawScoreDigits(void) {
    s32 i = 0;
    s32 cnt[2];
    TripleTriadCellPrim *prim;

    cnt[1] = 0;
    cnt[0] = 0;

    for (; i < 10; i++) {
        cnt[g_tripleTriadCardHands[i].initFlags & TT_OWNER_MASK]++;
    }

    prim = (TripleTriadCellPrim *)g_primCursor;

    for (i = 0; i < 2; i++) {
        prim->tag      = 0x05000000;
        prim->tpageCmd = 0xE100060C;
        prim->sprtCmd  = 0x64808080;
        if (!(i & 1)) prim->x0 = 40;
        else          prim->x0 = 320;
        prim->y0       = 192;
        prim->u0       = (cnt[i & 1] * 3 * 8) - 24;
        prim->v0       = 0x30;
        prim->clut     = 0x3A40;
        prim->h        = 24;
        prim->w        = 24;

        AddPrim(&g_otBase[4], prim);
        prim++;
    }

    g_primCursor = prim;
    return 0;
}

/**
 * @brief Build the Triple Triad per-frame update list.
 *
 * Resets the board, then registers the match controller (@c matchFlowHandler)
 * and the three render callbacks as update-list nodes, initializes the two
 * hands, and returns the list.
 *
 * @return The update-list head.
 */
u8 *initTripleTriadUpdateList(void) {
    u8 *list;
    HandlerNode *node;
    resetTriadBoard();
    list = D_801D3028;
    initObjList(list, D_801D3038, sizeof(HandlerNode), 8);
    node = (HandlerNode *)allocObjNode(list, (ObjNodeFn)matchFlowHandler);
    node->state = 0;
    node->counter = 0;
    node->pad14 = 0;
    allocObjNode(list, (ObjNodeFn)updateCardObjects);
    allocObjNode(list, (ObjNodeFn)drawBoardElements);
    allocObjNode(list, (ObjNodeFn)drawScoreDigits);
    setupTripleTriadHands();
    return list;
}

/**
 * @brief Compute a card's screen position from its slot descriptor.
 *
 * Writes a position vector from the descriptor's type: types 0 and 1 are the
 * left/right hand strips, type 2 is the board grid.
 *
 * @param src Descriptor bytes: [0] = type, [1] = column, [2] = row.
 * @param dst Position vector to fill; @c vz is the depth, @c pad the OT sort key.
 * @return @p dst.
 */
SVECTOR *layoutCardSlot(u8 *src, SVECTOR *dst) {
    u8 type = src[0];

    switch (type) {
    case TT_CARDSLOT_STRIP_L:
        dst->vx = -140;
        {
            u8 row = src[2];
            s32 depth = 512;
            dst->vz = depth;
            dst->vy = row * 32 - 64;
        }
        dst->pad = -(s32)src[2] + 14;
        break;
    case TT_CARDSLOT_STRIP_R:
        dst->vx = 140;
        {
            u8 row = src[2];
            s32 depth = 512;
            dst->vz = depth;
            dst->vy = row * 32 - 64;
        }
        dst->pad = -(s32)src[2] + 14;
        break;
    case TT_CARDSLOT_GRID: {
        s32 col = src[1];
        dst->vx = (col - 1) * 64;
        {
            u8 row = src[2];
            dst->vz = 512;
            dst->pad = 18;
            dst->vy = (row - 1) * 64;
        }
        break;
    }
    default:
        return dst;
    }
    return dst;
}

/**
 * @brief Find a card-object slot in @c g_tripleTriadCardHands matching a search key.
 *
 * @p groupId selects the search:
 *  - @c 0 or @c 1: the active card in that group with the given @p priority.
 *  - @c 2: the card with the given @p fieldD and @p priority.
 *  - otherwise: no match.
 *
 * @param groupId  Target group / search-mode selector.
 * @param fieldD   Target @c fieldD (mode 2 only).
 * @param priority Target @c priority.
 * @return Index of the first matching slot, or @c -1 if none / invalid mode.
 */
s32 findCardSlot(s32 groupId, s32 fieldD, s32 priority) {
    s32 i;

    if (groupId < 0) return -1;

    switch (groupId) {
    case TT_GROUP_CPU_HAND:
    case TT_GROUP_PLAYER_HAND:
        for (i = 0; i < 10; i++) {
            if (g_tripleTriadCardHands[i].flags & TT_CARD_ACTIVE) {
                if (g_tripleTriadCardHands[i].groupId == groupId) {
                    if (g_tripleTriadCardHands[i].priority == priority) {
                        return i;
                    }
                }
            }
        }
        return -1;

    case TT_GROUP_BOARD:
        for (i = 0; i < 10; i++) {
            if (g_tripleTriadCardHands[i].groupId == groupId) {
                if (g_tripleTriadCardHands[i].fieldD == fieldD) {
                    if (g_tripleTriadCardHands[i].priority == priority) {
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    return -1;
}

/**
 * @brief Flag the card object matching a search key (sets @c TT_CARD_ROTATE_CW).
 *
 * Looks up the slot via @c findCardSlot and, if found, sets @c TT_CARD_ROTATE_CW
 * in its @c flags.
 *
 * @param groupId  Search-mode selector / target group passed to findCardSlot.
 * @param priority Target priority passed as findCardSlot's third arg.
 */
void highlightCardSlot(s32 groupId, s32 priority) {
    s32 idx = findCardSlot(groupId, 0, priority);
    if (idx >= 0) {
        g_tripleTriadCardHands[idx].flags |= TT_CARD_ROTATE_CW;
    }
}
