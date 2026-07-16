#include "common.h"
#include "item.h"
#include "tripletriad.h"
#include "tripletriad/be_object1.h"
#include "tripletriad/be_object1b.h"
#include "tripletriad/be_object2.h"
#include "tripletriad/be_object4.h"

/**
 * @brief Set up a card-object slot and cancel lower-priority siblings in its group.
 *
 * Stores the three action params in slot @p idx, marks it active
 * (@c CARD_FX_FLIP), then sweeps the 10 slots: any sibling sharing @c groupId
 * with strictly lower @c priority is reset (@c CARD_FX_SHAKE).
 *
 * @param idx     Index of the slot being placed (0..9).
 * @param param0  First action parameter (stored at @c slot.param0).
 * @param param1  Second action parameter (stored at @c slot.param1).
 * @param param2  Third action parameter (stored at @c slot.param2).
 */
void setCardObjectAction(s32 idx, s32 param0, s32 param1, s32 param2) {
    s32 i;

    g_tripleTriadCardHands[idx].param0 = param0;
    g_tripleTriadCardHands[idx].param1 = param1;
    g_tripleTriadCardHands[idx].param2 = param2;
    setCardEntityType(idx, CARD_FX_FLIP);

    for (i = 0; i < 10; i++) {
        if (g_tripleTriadCardHands[i].groupId == g_tripleTriadCardHands[idx].groupId
            && g_tripleTriadCardHands[i].priority < g_tripleTriadCardHands[idx].priority) {
            setCardEntityType(i, CARD_FX_SHAKE);
        }
    }
}

/**
 * @brief Emit a 24x16 textured overlay sprite anchored at a node's screen XY.
 *
 * Builds one textured sprite (the elemental +1/-1 modifier marker) anchored to
 * the card node and links it into @p ot. @p variant picks between the two
 * marker textures.
 *
 * @param node     Animation node providing the screen-space anchor.
 * @param variant  Texture selector — positive selects the first marker.
 * @param ot       OT bucket pointer used by @c AddPrim.
 * @param out      Output primitive buffer (pre-allocated by caller).
 * @return         Pointer to the next free TSPRT slot in @p out.
 */
TSPRT *drawCardOverlaySprite(CardAnimNode *node, s32 variant, void *ot, TSPRT *out) {
    s32 i = 0;

    do {
        out->tag      = 0x05000000;
        out->drawMode = 0xE100060C;
        *(u32 *)&out->r0 = 0x64808080;
        out->x0 = (u16)node->mat.t[0] + 0xB4;
        {
            s32 y0 = (u16)node->mat.t[1];
            out->w  = 24;
            out->h  = 16;
            out->y0 = y0 + 0x68;
        }
        if (variant > 0) {
            out->u0   = 0;
            out->clut = 0x3A80;
        } else {
            out->u0   = 24;
            out->clut = 0x3AC0;
        }
        out->v0 = 0x48;

        AddPrim(ot, out);
        out++;
        i++;
    } while (i <= 0);

    return out;
}

/**
 * @brief Per-frame update for one @c TripleTriadCardObject — rotation, transform, render.
 *
 * Advances the card's spin angle, runs its effect animation
 * (@c animateCardEffect) and layout to build a world transform, then draws the
 * card (@c drawTriadCard / @c transformCardEffect) plus, for a placed card on an
 * elemental cell, its elemental-modifier overlay.
 *
 * @param ctl Handler context whose @c entry points at the card object being driven.
 * @return Always 0.
 */
s32 updateCardObject(CardObjectCtl *ctl) {
    TripleTriadCardObject *entity;
    CardAnimNode *node;

    node = scratchAlloc(sizeof(CardAnimNode));
    entity = ctl->entry;

    if (entity->flags & TT_CARD_ROTATE_CW) {
        if (entity->angle < 0x1000) {
            s16 newAngle = entity->angle + 0x800;
            entity->angle = newAngle;
            if (newAngle > 0x1000) {
                entity->angle = 0x1000;
            }
        }
        entity->flags &= ~TT_CARD_ROTATE_CW;
    } else {
        if (entity->angle != 0) {
            s16 newAngle = entity->angle - 0x800;
            entity->angle = newAngle;
            if (newAngle < 0) {
                entity->angle = 0;
            }
        }
    }

    animateCardEffect(entity);
    layoutCardSlot(&entity->groupId, &node->base);
    RotMatrixYXZ(&entity->posData[0], node);

    node->mat.t[0]  = node->base.vx + entity->offX;
    node->mat.t[1]  = node->base.vy + entity->offY;
    node->mat.t[2]    = node->base.vz + entity->offZ;
    node->base.pad  += entity->offSort;

    if (entity->angle != 0) {
        if (entity->groupId == 0) {
            node->mat.t[0] += (rsin(entity->angle / 4) * 12) >> 12;
        } else {
            node->mat.t[0] -= (rsin(entity->angle / 4) * 12) >> 12;
        }
    }

    if (entity->state == CARD_FX_IDLE && entity->groupId == 2) {
        s32 col = entity->fieldD + 1;
        s8 elementMod = g_tripleTriadBoard.cells[entity->priority + 1][col].elementMod;
        if (elementMod != 0) {
            g_primCursor = drawCardOverlaySprite(node, elementMod,
                                        &g_otBase[node->base.pad], g_primCursor);
        }
    }

    SetRotMatrix(&node->mat);
    SetTransMatrix(&node->mat);

    g_primCursor = drawTriadCard(entity->cardId, entity->initFlags,
                                &g_otBase[node->base.pad], g_primCursor);
    transformCardEffect(entity, node, &g_otBase[node->base.pad]);

    scratchFree(sizeof(CardAnimNode));
    return 0;
}

/**
 * @brief Per-frame update of the card-object list.
 */
void processCardObjects(s32 arg) {
    updateObjectList(g_cardObjList);
}

/**
 * @brief Initialize the 10 hand-card @c TripleTriadCardObject slots for a Triple Triad match.
 *
 * Builds the card-object list, then fills both players' five hand cards in
 * @c g_tripleTriadCardHands from @c D_801A2C48 — setting each card's id, owning
 * seat, hand slot, and active flag, and registering a per-frame
 * @c updateCardObject callback that drives it via @c CardObjectCtl.entry.
 */
void setupTripleTriadHands(void) {
    s32 player;
    s32 slot;
    TripleTriadCardObject *entity;
    CardObjectCtl *node;
    u8 *hand;

    initObjList(g_cardObjList, g_cardObjPool, sizeof(CardObjectCtl), 10);

    entity = g_tripleTriadCardHands;
    for (player = 0; player < 2; player++) {
        u8 *playerType;
        hand = D_801A2C48[player];
        for (slot = 0; slot < 5; slot++) {
            playerType = &D_801A2C70[player];
            node = (CardObjectCtl *)allocObjNode(g_cardObjList, (ObjNodeFn)updateCardObject);
            node->entry = entity;
            entity->cardId = hand[slot];
            entity->state      = CARD_FX_IDLE;
            entity->initFlags  = TT_SHOW_ELEMENT | TT_USE_STATS | player;
            entity->groupId    = player;
            entity->fieldD     = 0;
            entity->priority   = slot;
            entity->posData[0] = 0;
            if ((g_tripleTriadRules & TT_RULE_OPEN) == 0 && *playerType == 3) {
                entity->posData[1] = 0x800;
            } else {
                entity->posData[1] = 0;
            }
            entity->field18 = 0;
            entity->offX    = 0;
            entity->offY    = 0;
            entity->offZ    = 0;
            entity->offSort = 0;
            entity->angle   = 0;
            entity->flags   = 1;
            entity++;
        }
    }
}

/**
 * @brief Render one Triple Triad card to the primitive buffer.
 *
 * Projects the card quad and backface-tests it. A front-facing card emits its
 * rank digits (when @c flags requests them), an element marker, the card body,
 * and a gradient border; a back-facing card emits a single mirrored card back.
 * Returns the advanced @p out so the caller can chain further primitives.
 *
 * @param cardId Index into @c g_tripleTriadCardStats.
 * @param flags  Render flags (the card object's @c initFlags): @c TT_USE_STATS =
 *               show ranks, @c TT_SHOW_LIGHT_OVERLAY = translucent,
 *               @c TT_SHOW_ELEMENT = element marker, @c TT_SHOW_DARK_OVERLAY =
 *               dimmed palette, @c TT_OWNER_MASK = opponent border tint.
 * @param ot     OT bucket pointer for @c AddPrim (AddPrim).
 * @param out    Output primitive buffer (pre-allocated by the caller).
 * @return       Pointer to the next free primitive slot in @p out.
 *
 * @note A clean rewrite compiles to ~99.5%. The separate @c anchor walker in the
 *       rank-digit loop, the empty @c do/while(0) blocks, and the dead
 *       @c argP=gPrim reads are gcc-2.7.2 scheduling/allocation scaffolds that are
 *       load-bearing for the match — they are marked inline; don't simplify them.
 */
void *drawTriadCard(s32 cardId, s32 flags, void *ot, void *out) {
    CardRenderWork *work;
    POLY_FT4 *ftPrim;
    POLY_G4 *gPrim;
    POLY_G4 *argP;
    s32 baseColor;
    s32 transBit;
    s32 eflag;
    TripleTriadCard *card;
    u8 id;

    id = cardId;
    work = (CardRenderWork *)scratchAlloc(sizeof(CardRenderWork));
    baseColor = 0x2C404040;
    if ((flags & TT_SHOW_DARK_OVERLAY) == 0) {
        baseColor = 0x2C808080;
    }
    if (flags & TT_SHOW_LIGHT_OVERLAY) {
        transBit = 0x2000000;
        baseColor |= transBit;
    } else {
        transBit = 0;
    }
    card = &g_tripleTriadCardStats[id];
    RotTransPers4(&g_cardFaceQuad[0], &g_cardFaceQuad[1], &g_cardFaceQuad[2], &g_cardFaceQuad[3],
                  &work->outXY[0], &work->outXY[1], &work->outXY[2], &work->outXY[3], &work->P, &work->flag);
    if (NormalClip(work->outXY[0], work->outXY[1], work->outXY[2]) >= 0) {
        if (flags & TT_USE_STATS) {
            SVECTOR *rowAddr;
            s32 i;
            s32 j;
            POLY_FT4 *anchor = (POLY_FT4 *)out;
            for (i = 0; i < 4; i++) {
                rowAddr = &g_cardDigitCenters[i];
                for (j = 0; j < 4; j++) {
                    argP = gPrim; /* dead read: amplifies gPrim refs to split the %hi temp (see note) */
                    work->digitVerts[j].vx = rowAddr->vx + g_cardDigitOffsets[j].vx;
                    work->digitVerts[j].vy = rowAddr->vy + g_cardDigitOffsets[j].vy;
                    work->digitVerts[j].vz = rowAddr->vz + g_cardDigitOffsets[j].vz;
                }

                RotTransPers4(&work->digitVerts[0], &work->digitVerts[1], &work->digitVerts[2], &work->digitVerts[3],
                              (s32 *)&((POLY_FT4 *)out)->x0, (s32 *)&((POLY_FT4 *)out)->x1,
                              (s32 *)&((POLY_FT4 *)out)->x2, (s32 *)&((POLY_FT4 *)out)->x3, &work->P, &work->flag);
                ((POLY_FT4 *)out)->tag = 0x09000000;
                do { } while (0); /* BB boundary: keep the rgb value built inline */
                *(u32 *)&anchor->r0 = transBit | 0x2C808080;
                do { } while (0); /* BB boundary: keep the rgb store ahead of the out advance */
                ftPrim = (POLY_FT4 *)out;
                out = ftPrim + 1;
                anchor->tpage = 0xC;
                anchor->clut = 0x3800;
                {
                    u8 rank = card->sides[i];
                    setUVWH(anchor, rank * 16, 0, 11, 11);
                }
                AddPrim(ot, ftPrim);
                anchor++;
            }
        }
        /* triple do/while(0): amplify flags' ref count so it wins s7 over baseColor */
        do { do { do { eflag = flags & TT_SHOW_ELEMENT; } while (0); } while (0); } while (0);
        if (eflag) {
            s32 element = card->element;
            if (element != 0) {
                char bits = element;
                s32 bit;
                for (bit = 0; bit < 8; bit++) {
                    if ((bits >> bit) & 1) {
                        break;
                    }
                }

                ftPrim = (POLY_FT4 *)out;
                ftPrim->tag = 0x09000000;
                *(u32 *)&ftPrim->r0 = 0x2C808080;
                RotTransPers4(&g_cardElementQuad[0], &g_cardElementQuad[1], &g_cardElementQuad[2], &g_cardElementQuad[3],
                              (s32 *)&ftPrim->x0, (s32 *)&ftPrim->x1, (s32 *)&ftPrim->x2, (s32 *)&ftPrim->x3,
                              &work->P, &work->flag);
                ftPrim->tpage = 0xC;
                ftPrim->clut = (bit + 0xE1) << 6;
                {
                    s32 uLeft = (bit % 4) * 64;
                    s32 vTop = ((bit / 4) * 16) + 16;
                    setUVWH(ftPrim, uLeft, vTop, 15, 15);
                }
                AddPrim(ot, ftPrim);
                out = ftPrim + 1;
            }
        }
        ftPrim = (POLY_FT4 *)out;
        ftPrim->tag = 0x09000000;
        *(u32 *)&ftPrim->r0 = baseColor | 0x2C000000;
        *(s32 *)&ftPrim->x0 = work->outXY[0];
        *(s32 *)&ftPrim->x1 = work->outXY[1];
        *(s32 *)&ftPrim->x2 = work->outXY[2];
        *(s32 *)&ftPrim->x3 = work->outXY[3];
        setUVWH(ftPrim, (id & 0x1) << 6, ((u8)(id << 5)) & 0xC0, 0x3F, 0x3F);
        ftPrim->clut = (((id >> 1) + 0xC8) << 6) | ((((id & 0x1) << 7) + 0x300) >> 4);
        ftPrim->tpage = ((id >> 3) + 0x10) | 0x80;
        AddPrim(ot, ftPrim);
        out = ftPrim + 1;
        {
            u32 color0;
            u32 color1;
            u32 color2;
            RotTransPers4(&g_cardBorderQuad[0], &g_cardBorderQuad[1], &g_cardBorderQuad[2], &g_cardBorderQuad[3],
                          (s32 *)&((POLY_G4 *)out)->x0, (s32 *)&((POLY_G4 *)out)->x1,
                          (s32 *)&((POLY_G4 *)out)->x2, (s32 *)&((POLY_G4 *)out)->x3, &work->P, &work->flag);
            ((POLY_G4 *)out)->tag = 0x08000000;
            gPrim = (POLY_G4 *)out;
            if (flags & TT_SHOW_DARK_OVERLAY) {
                color0 = 0x38807060;
                color1 = 0x807060;
                color2 = 0x402018;
            } else if ((flags & TT_OWNER_MASK) == 0) {
                color0 = 0x38E0C0FF;
                color1 = 0xE0C0FF;
                color2 = 0x403080;
            } else {
                color0 = 0x38FFE0C0;
                color1 = 0xFFE0C0;
                color2 = 0x804030;
            }
            *(u32 *)&((POLY_G4 *)out)->r0 = color0;
            *(u32 *)&((POLY_G4 *)out)->r1 = color1;
            *(u32 *)&((POLY_G4 *)out)->r2 = color2;
            *(u32 *)&((POLY_G4 *)out)->r3 = color2;
            do { argP = gPrim; } while (0); /* BB boundary: block the front/back AddPrim cross-jump */
            AddPrim(ot, argP);
            out = gPrim + 1;
        }
    } else {
        ftPrim = (POLY_FT4 *)out;
        ftPrim->tag = 0x09000000;
        *(u32 *)&ftPrim->r0 = transBit | 0x2C808080;
        *(s32 *)&ftPrim->x0 = work->outXY[0];
        *(s32 *)&ftPrim->x1 = work->outXY[1];
        *(s32 *)&ftPrim->x2 = work->outXY[2];
        *(s32 *)&ftPrim->x3 = work->outXY[3];
        setUV4(ftPrim, 0x3F, 0xC0, 0, 0xC0, 0x3F, 0xFF, 0, 0xFF);
        ftPrim->tpage = 0x9D;
        ftPrim->clut = 0x3FF0;
        AddPrim(ot, ftPrim);
        out = ftPrim + 1;
    }
    scratchFree(sizeof(CardRenderWork));
    return out;
}

/**
 * @brief Emit a single semi-transparent black quad — a card drop shadow.
 *
 * Projects the four shadow-quad corners through the GTE and links the quad into
 * the OT.
 *
 * @param ot   OT bucket pointer.
 * @param prim Pre-allocated @c POLY_F4 slot in the primitive buffer.
 * @return     @p prim incremented past this primitive.
 */
POLY_F4 *drawCardShadow(u32 *ot, POLY_F4 *prim) {
    CardRenderWork *work;

    work = scratchAlloc(sizeof(CardRenderWork));
    prim->tag = 0x05000000;
    *(u32 *)&prim->r0 = 0x2A000000;

    RotTransPers4(&g_cardShadowQuad[0], &g_cardShadowQuad[1], &g_cardShadowQuad[2], &g_cardShadowQuad[3],
                  (s32 *)&prim->x0, (s32 *)&prim->x1,
                  (s32 *)&prim->x2, (s32 *)&prim->x3,
                  &work->P, &work->flag);

    AddPrim(ot, prim);
    scratchFree(sizeof(CardRenderWork));
    return prim + 1;
}

/**
 * @brief Reset the Triple Triad menu/substate state for a new selection.
 *
 * Clears the subscribed-substate mask and the per-substate cursor slots (the
 * 2D board substate's row/column are seeded to 1).
 */
void resetTriadMenuState(void) {
    g_substateMask = 0;
    g_substatePhase = TT_SUBPHASE_IDLE;
    D_801D3340[1].field2 = 0;
    D_801D3340[2].field2 = 0;
    D_801D3340[3].field0 = 1;
    D_801D3340[3].field2 = 1;
    D_801D3340[4].field0 = 0;
    D_801D3340[5].field0 = 0;
}

/**
 * @brief Emit one HUD primitive at a substate-driven anchor.
 *
 * Dispatches on @p mode (1..5; other values are no-ops); each mode emits a
 * single cursor/label sprite whose screen anchor is derived from the substate
 * slot's @c field0 / @c field2.
 *
 * @param mode  Substate index (1..5; other values are ignored).
 * @param slot  Substate parameter slot supplying @c field0 / @c field2 anchors.
 */
void drawMenuPrim(s32 mode, SubstateSlot *slot) {
    switch (mode) {
    case TT_SUBSTATE_HAND_P0:
        g_primCursor = func_8002FF34(&g_otBase[4], g_primCursor,
                                    1, 0x58,
                                    (slot->field2 << 5) + 0x30, 0x808080);
        break;
    case TT_SUBSTATE_HAND_P1:
        g_primCursor = func_8002FF34(&g_otBase[4], g_primCursor,
                                    0, 0x110,
                                    (slot->field2 << 5) + 0x30, 0x808080);
        break;
    case TT_SUBSTATE_BOARD:
        g_primCursor = func_8002FF34(&g_otBase[4], g_primCursor,
                                    0, (slot->field0 << 6) + 0x68,
                                    (slot->field2 << 6) | 0x30, 0x808080);
        break;
    case TT_SUBSTATE_CONFIG_A:
        g_primCursor = func_8002FF34(&g_otBase[4], g_primCursor,
                                    0, (slot->field0 << 6) + 0x28,
                                    0x4C, 0x808080);
        break;
    case TT_SUBSTATE_CONFIG_B:
        g_primCursor = func_8002FF34(&g_otBase[4], g_primCursor,
                                    0, (slot->field0 << 6) + 0x28,
                                    0x94, 0x808080);
        break;
    }
}

/**
 * @brief Show the card-detail popup for the card object matching a search key.
 *
 * Looks up the slot via @c findCardSlot and, if found, passes its @c cardId to
 * @c showCardDetail.
 *
 * @param groupId  Search group — @c findCardSlot arg 0.
 * @param fieldD   Secondary key — @c findCardSlot arg 1.
 * @param priority Priority key — @c findCardSlot arg 2.
 */
void initMenuObjectHandler(s32 groupId, s32 fieldD, s32 priority) {
    s32 idx = findCardSlot(groupId, fieldD, priority);
    if (idx >= 0) {
        showCardDetail(g_tripleTriadCardHands[idx].cardId);
    }
}

/**
 * @brief Substate-1 handler: left/right cursor over the cpu hand row.
 *
 * Moves @c slot->field2 left/right to the next valid card (or, on select,
 * queues the next substate), then highlights the cursor's card and shows its
 * detail popup.
 *
 * @param slot Substate parameter slot — @c field2 is the row cursor.
 * @param idx  Substate index (unused here; preserved for the dispatcher
 *             callback signature).
 */
void handleCursorSubstate1(SubstateSlot *slot, s32 idx) {
    if ((g_padRepeatLatch & 0x1000) && findCardSlot(0, 0, slot->field2 - 1) >= 0) {
        playTriadSfx(1);
        slot->field2 = slot->field2 - 1;
    } else if ((g_padRepeatLatch & 0x4000) && findCardSlot(0, 0, slot->field2 + 1) >= 0) {
        playTriadSfx(1);
        slot->field2 = slot->field2 + 1;
    } else if (g_padRepeatLatch & 0x2000) {
        if (g_substateMask & 0x8) {
            g_activeSubstate = 3;
            return;
        }
        if (g_substateMask & 0x4) {
            g_activeSubstate = 2;
            return;
        }
    }

    highlightCardSlot(0, slot->field2);
    initMenuObjectHandler(0, 0, slot->field2);
}

/**
 * @brief Substate-2 handler: left/right cursor over the player hand row.
 *
 * Mirror of @ref handleCursorSubstate1 for the other hand (different search
 * group and cancel mapping).
 *
 * @param slot Substate parameter slot — @c field2 is the column cursor.
 * @param idx  Substate index (unused; preserved for the dispatcher
 *             callback signature).
 */
void handleCursorSubstate2(SubstateSlot *slot, s32 idx) {
    if ((g_padRepeatLatch & 0x1000) && findCardSlot(1, 0, slot->field2 - 1) >= 0) {
        playTriadSfx(1);
        slot->field2 = slot->field2 - 1;
    } else if ((g_padRepeatLatch & 0x4000) && findCardSlot(1, 0, slot->field2 + 1) >= 0) {
        playTriadSfx(1);
        slot->field2 = slot->field2 + 1;
    } else if (g_padRepeatLatch & 0x8000) {
        if (g_substateMask & 0x8) {
            g_activeSubstate = 3;
            return;
        }
        if (g_substateMask & 0x2) {
            g_activeSubstate = 1;
            return;
        }
    }

    highlightCardSlot(1, slot->field2);
    initMenuObjectHandler(1, 0, slot->field2);
}

/**
 * @brief Substate-3 handler: 2D board cursor (row × column) with edge wrap.
 *
 * Moves the @c field0 row / @c field2 column cursor within the 3x3 board on the
 * d-pad. Stepping off the top or bottom edge clamps the row and, depending on
 * the subscribed-substate mask, hands control back to a hand-row substate.
 *
 * @param slot Substate parameter slot — @c field0 row, @c field2 column.
 * @param idx  Substate index (unused; preserved for the dispatcher
 *             callback signature).
 */
void handleCursorSubstate3(SubstateSlot *slot, s32 idx) {
    if (g_padRepeatLatch & 0x8000) {
        slot->field0 = slot->field0 - 1;
        if (slot->field0 >= 0) {
            playTriadSfx(1);
        }
    } else if (g_padRepeatLatch & 0x2000) {
        slot->field0 = slot->field0 + 1;
        if (slot->field0 < 3) {
            playTriadSfx(1);
        }
    } else if ((g_padRepeatLatch & 0x1000) && slot->field2 > 0) {
        playTriadSfx(1);
        slot->field2 = slot->field2 - 1;
    } else if ((g_padRepeatLatch & 0x4000) && slot->field2 < 2) {
        playTriadSfx(1);
        slot->field2 = slot->field2 + 1;
    }

    if (slot->field0 < 0) {
        slot->field0 = 0;
        if (g_substateMask & 0x2) {
            g_activeSubstate = 1;
        }
    } else if (slot->field0 >= 3) {
        slot->field0 = 2;
        if (g_substateMask & 0x4) {
            g_activeSubstate = 2;
        }
    }

    initMenuObjectHandler(2, slot->field0, slot->field2);
}

/**
 * @brief Adjust a config parameter (0..4) up or down from controller input.
 *
 * Left/right input nudges @p param within [0, 4] (with a confirm SFX) and
 * mirrors the result into the menu's display state.
 *
 * @param param Pointer to a halfword value to adjust.
 */
void adjustConfigParam(u16 *param) {
    u16 val;

    if (g_padRepeatLatch & 0x8000) {
        if (*(s16 *)param > 0) {
            playTriadSfx(1);
            val = *param - 1;
            *param = val;
            goto store;
        }
    }
    if (g_padRepeatLatch & 0x2000) {
        if (*(s16 *)param < 4) {
            playTriadSfx(1);
            val = *param + 1;
            *param = val;
            goto store;
        }
    }
store:
    D_801D335C.field0 = *param;
}

/**
 * @brief Per-frame Triple Triad menu tick: latch pad input, dispatch substate.
 *
 * Latches the current frame's pad masks, then — if a substate is armed —
 * dispatches to the matching cursor/config handler and draws its HUD primitive.
 * Finally checks the completion triggers that commit or cancel the substate.
 */
void updateTriadMenu(void) {
    s32 state = *(u8 *)&g_menuPadSource;

    switch (state) {
    case TT_PAD_SRC_P0:
    case TT_PAD_SRC_P1:
        g_padHeldLatch = g_padHeld[state];
        g_padRepeatLatch = g_padRepeat[state];
        g_padPressedLatch = g_padPressed[state];
        break;
    case TT_PAD_SRC_BOTH:
        g_padHeldLatch = g_padHeld[0] | g_padHeld[1];
        g_padRepeatLatch = g_padRepeat[0] | g_padRepeat[1];
        g_padPressedLatch = g_padPressed[0] | g_padPressed[1];
        break;
    }

    if (g_substatePhase == TT_SUBPHASE_ACTIVE) {
        s32   idx = g_activeSubstate * 4;
        void *p   = &D_801D3340[g_activeSubstate];

        switch (g_activeSubstate) {
        case TT_SUBSTATE_NONE: break;
        case TT_SUBSTATE_HAND_P0: handleCursorSubstate1(p, idx); break;
        case TT_SUBSTATE_HAND_P1: handleCursorSubstate2(p, idx); break;
        case TT_SUBSTATE_BOARD:   handleCursorSubstate3(p, idx); break;
        case TT_SUBSTATE_CONFIG_A:
        case TT_SUBSTATE_CONFIG_B: adjustConfigParam(p);      break;
        }

        drawMenuPrim(g_activeSubstate, &D_801D3340[g_activeSubstate]);

        if (!(g_substateSuppress & 1)) {
            if (g_padPressedLatch & 0xC0) {
                g_substatePhase = TT_SUBPHASE_CONFIRM;
                memcpy(&D_801D335C, &D_801D3340[g_activeSubstate], 4);
                return;
            }
        }
        if (!(g_substateSuppress & 2) && (g_padPressedLatch & 0x10)) {
            g_substatePhase = TT_SUBPHASE_CANCEL;
        }
    }
}

/**
 * @brief Activate a substate slot and seed its cursor.
 *
 * Arms substate @p idx (recording it in the subscribed-substate mask) and
 * latches the caller's control fields for the next @ref updateTriadMenu tick.
 * For the hand-row substates (1 and 2) it also nudges the column cursor to the
 * nearest valid card.
 *
 * @param idx           Substate index (0..5).
 * @param mask          Caller-supplied subscriber mask (this substate's bit
 *                      is OR'd in before storing).
 * @param stateByte     State byte latched for the dispatcher.
 * @param suppressFlags Completion-suppress flags latched for the dispatcher.
 */
void activateMenuSubstate(s32 idx, s32 mask, u8 stateByte, s32 suppressFlags) {
    g_activeSubstate = idx;
    g_substateMask = mask | (1 << idx);
    g_substatePhase = TT_SUBPHASE_ACTIVE;
    g_menuPadSource = stateByte;
    g_substateSuppress = suppressFlags;

    if (idx >= 3) return;
    if (idx <= 0) return;

    if (findCardSlot(idx - 1, 0, D_801D3340[idx].field2) < 0) {
        D_801D3340[idx].field2 = D_801D3340[idx].field2 + 1;
    }
}

/**
 * @brief Per-frame state machine for the player's Triple Triad card-selection cursor.
 *
 * Walks the player through picking a hand card, choosing a board cell, and
 * placing it:
 *  - pick a card from the hand (re-arming the cursor until a valid one is chosen);
 *  - hand off to the board-cell cursor;
 *  - on confirm, commit the card to the board (rejecting an occupied cell);
 *    on cancel, return to the card pick.
 * If the player opens the pause/options menu instead, it bails immediately.
 *
 * @param p Card-selection cursor node.
 * @return  @c 0 while still selecting; @c 2 once a card has been placed.
 */
s32 updateCardSelectCursor(SubstateMachineNode *p) {
    s32 s1;

    if (!(g_tripleTriadInputFlags & TT_INPUT_DISABLED) && (g_padPressed[2] & 0x20)) {
        openTriadMenu();
        return 0;
    }

    while (1) {
        s1 = p->state;
        switch (s1) {
        case CARD_SEL_BEGIN_PICK:
            activateMenuSubstate(p->fieldD + 1, 0, p->fieldE, 2);
            p->state = CARD_SEL_WAIT_PICK;
            break;
        case CARD_SEL_WAIT_PICK:
            if (g_substatePhase == s1) return 0;
            if (findCardSlot(p->fieldD, 0, D_801D335C.field2) >= 0) {
                p->snapshot = D_801D335C;
                p->state = CARD_SEL_BEGIN_PLACE;
                playTriadSfx(1);
            } else {
                p->state = CARD_SEL_BEGIN_PICK;
            }
            break;
        case CARD_SEL_BEGIN_PLACE:
            activateMenuSubstate(TT_SUBSTATE_BOARD, 0, p->fieldE, 0);
            p->state = CARD_SEL_WAIT_PLACE;
            break;
        case CARD_SEL_WAIT_PLACE: {
            s32 trig;
            if (g_drawBufferIndex != 0) {
                drawMenuPrim(p->fieldD + 1, &p->snapshot);
            }
            highlightCardSlot(p->fieldD, p->snapshot.field2);
            trig = g_substatePhase;
            switch (trig) {
            case TT_SUBPHASE_CONFIRM:
                if (findCardSlot(2, D_801D335C.field0, D_801D335C.field2) < 0) {
                    s32 entIdx = findCardSlot(p->fieldD, 0, p->snapshot.field2);
                    setCardObjectAction(entIdx, 2, D_801D335C.field0, D_801D335C.field2);
                    commitCardToBoard(&g_tripleTriadBoard, entIdx, D_801D335C.field0, D_801D335C.field2);
                    playTriadSfx(1);
                    return 2;
                }
                playTriadSfx(0x10);
                p->state = trig;
                break;
            case TT_SUBPHASE_ACTIVE:
                return 0;
            case TT_SUBPHASE_CANCEL:
                if (trig == s1) {
                    playTriadSfx(9);
                    p->state = CARD_SEL_BEGIN_PICK;
                }
                break;
            }
            break;
        }
        }
    }
}

/**
 * @brief Spawn the player's card-selection cursor handler.
 *
 * Creates a one-node list driving @ref updateCardSelectCursor, seeded with the
 * row/state parameters.
 *
 * @param rowSeed   Cursor row seed (node @c fieldD).
 * @param stateByte State byte forwarded to activateMenuSubstate (node @c fieldE).
 * @return The cursor list head.
 */
u8 *spawnCardSelectCursor(s32 rowSeed, s32 stateByte) {
    ObjList *list = g_cursorList;
    SubstateMachineNode *node;

    initObjList(list, g_cursorPool, sizeof(SubstateMachineNode), 1);
    node = (SubstateMachineNode *)allocObjNode(list, (ObjNodeFn)updateCardSelectCursor);
    node->state = CARD_SEL_BEGIN_PICK;
    node->fieldD = rowSeed;
    node->fieldE = stateByte;
    D_801D3340[3].field0 = 1;
    D_801D3340[3].field2 = 1;
    return list;
}

/**
 * @brief Set a card object's animation @c state and clear its @c field02.
 *
 * A @p type of 2..5 also triggers a capture-effect SFX.
 *
 * @param entityIdx Index into @c g_tripleTriadCardHands.
 * @param type      New animation type, stored as the object's @c state.
 */
void setCardEntityType(s32 entityIdx, s32 type) {
    TripleTriadCardObject *base = g_tripleTriadCardHands;
    TripleTriadCardObject *entry = &base[entityIdx];
    entry->state = type;
    entry->field02 = 0;
    if (type <= CARD_FX_SLIDE_RIGHT) {
        if (type >= CARD_FX_SLIDE_LEFT) {
            playTriadSfxParam(0x5A, 1);
        }
    }
}

/**
 * @brief Report whether any card object is mid-effect (non-zero @c state).
 *
 * @return 1 if any of the 10 card objects has a non-zero @c state, else 0.
 */
s32 anyCardEffectActive(void) {
    s32 i = 0;
    TripleTriadCardObject *entry = g_tripleTriadCardHands;
    do {
        if (entry->state != CARD_FX_IDLE) {
            return 1;
        }
        i++;
        entry++;
    } while (i < 10);
    return 0;
}

/**
 * @brief Per-frame effect animation for a @c TripleTriadCardObject (state machine).
 *
 * Dispatched once per frame from @c updateCardObject; advances the card's
 * position/rotation offsets for its current @ref CardEffectState:
 *  - @c CARD_FX_IDLE / @c CARD_FX_FLASH — no movement this frame.
 *  - @c CARD_FX_FLIP — the open/flip sequence (raise, hold, then drop into place).
 *  - @c CARD_FX_SLIDE_* — slide in from a captured-from direction.
 *  - @c CARD_FX_SHAKE — a short shake, then clear and advance to the next card.
 * Each sub-animation runs for a fixed number of frames (counted in @c field02)
 * and clears @c state back to idle when it finishes.
 *
 * @param entity Card object being driven this frame.
 */
void animateCardEffect(TripleTriadCardObject *entity) {
    s32 state;
    s32 field02;
    s32 t;

    state = entity->state;
    field02 = entity->field02;

    switch (state) {
    case CARD_FX_IDLE:
    case CARD_FX_FLASH:
        break;

    case CARD_FX_FLIP:
        if (field02 < 20) {
            if (field02 == 0) {
                playTriadSfx(0x59);
            }
            t = ((field02 + 1) * 4096) / 20;
            entity->offY = -(rsin(t / 4) * 224) >> 12;
            entity->angle = 4096;
        } else {
            field02 -= 20;
            if (field02 < 5) {
                /* hold */
            } else {
                field02 -= 5;
                if (field02 < 25) {
                    if (field02 == 0) {
                        entity->posData[1] = 0;
                        *(Tetra4 *)&entity->groupId = *(Tetra4 *)&entity->param0;
                        entity->offSort = -9;
                    }
                    t = ((field02 + 1) * 4096) / 25;
                    entity->offY = ((rsin(t / 4) * 224) >> 12) - 224;
                    entity->offZ = (((4096 - rcos(t / 4)) * 128) >> 12) - 128;
                    entity->angle = 0;
                } else {
                    entity->state = CARD_FX_IDLE;
                    entity->offSort = 0;
                }
            }
        }
        entity->field02++;
        break;

    case CARD_FX_SLIDE_LEFT:
    case CARD_FX_SLIDE_UP:
    case CARD_FX_SLIDE_DOWN:
    case CARD_FX_SLIDE_RIGHT:
        state -= CARD_FX_SLIDE_LEFT;
        if (field02 < 25) {
            if (field02 == 12) {
                entity->initFlags ^= TT_OWNER_MASK;
            }
            t = ((field02 + 1) * 4096) / 25;
            entity->offZ = -((rsin(t / 2) * 128) >> 12);
            entity->offSort = -9;
            entity->posData[0] = (D_80182D10[state].vx * t) >> 12;
            entity->posData[1] = (D_80182D10[state].vy * t) >> 12;
            entity->field18   = (D_80182D10[state].vz * t) >> 12;
        } else {
            field02 -= 25;
            if (field02 < 5) {
                entity->offSort = 0;
            } else {
                entity->state = CARD_FX_IDLE;
            }
        }
        entity->field02++;
        break;

    case CARD_FX_SHAKE:
        entity->field02++;
        t = (entity->field02 << 12) / 10;
        t = rsin(t / 4);
        entity->offY = (u32)t >> 7;
        if (entity->field02 >= 10) {
            entity->offY = 0;
            entity->state = CARD_FX_IDLE;
            entity->field02 = 0;
            entity->priority++;
        }
        break;
    }
}

/**
 * @brief Emit a 62x62 semi-transparent gouraud quad over a card-effect node.
 *
 * Builds a translucent gouraud quad (the flip flash) over the card and links it
 * into @p ot. Each corner's brightness is driven out of phase by @p angle, so
 * the quad shimmers as the animation rotates.
 *
 * @param node     Display-list node providing the screen-space anchor.
 * @param angle    Animation phase driving the per-corner brightness.
 * @param ot       Ordering-table bucket to link the primitive into.
 * @param primBuf  Caller's primitive buffer.
 * @return Pointer past the emitted primitives.
 */
u8 *drawCardEffectQuad(CardAnimNode *node, s32 angle, void *ot, u8 *primBuf) {
    POLY_G4 *poly = (POLY_G4 *)primBuf;

    D_801D3390 = ((CVECTOR *)poly) + 1;
    SetFarColor(0xFF, 0xFF, 0xFF);

    {
        s32 sinVal;
        s32 i;
        for (i = 0; i < 4; i++) {
            sinVal = rsin(angle + D_80182D30[i]);
            if (sinVal < 0) sinVal = 0;
            DpqColor(&D_80182D40, sinVal, D_801D3390);
            D_801D3390 += 2;
        }
    }

    poly->tag  = 0x08000000;
    poly->code = 0x3A;

    poly->x0 = poly->x2 = (u16)node->mat.t[0] + 0xA1;
    poly->x1 = poly->x3 = (u16)node->mat.t[0] + 0xDF;
    poly->y0 = poly->y1 = (u16)node->mat.t[1] + 0x51;
    poly->y2 = poly->y3 = (u16)node->mat.t[1] + 0x8F;

    AddPrim(ot, poly);

    {
        DR_TPAGE *tpage = (DR_TPAGE *)(poly + 1);
        SetDrawTPage(tpage, 1, 1, 0x20);
        AddPrim(ot, tpage);
        return (u8 *)(tpage + 1);
    }
}

/**
 * @brief Per-frame transform/render stage for a card-effect @c TripleTriadCardObject.
 *
 * The render-side companion to @c animateCardEffect: for the slide-in states it
 * builds the card's matrix and links its drop shadow into @p otBucket; for the
 * flip state it emits the translucent flash quad. Idle states draw nothing.
 *
 * @note The @c goto dispatch and the @c stateCopy second local are scaffolds
 *       that are load-bearing for the match — keep them.
 *
 * @param entity   The card object being animated.
 * @param node     Its render node (transform matrix + base position).
 * @param otBucket OT layer the slide-in primitive links into.
 */
void transformCardEffect(TripleTriadCardObject *entity, CardAnimNode *node, void *otBucket) {
    VECTOR scaleVec = g_cardScaleVec;
    s32 state = entity->state;
    u8 stateCopy;
    s32 field02 = entity->field02;

    stateCopy = state;
    if (state >= CARD_FX_FLASH) {
        goto flip;
    }
    if (state != CARD_FX_IDLE) {
        goto slide;
    }
    return;

flip:
    if (stateCopy == CARD_FX_FLASH) {
        g_primCursor = drawCardEffectQuad(node, ((field02 + 1) * 4096) / 20, &g_otBase[6], g_primCursor);
        entity->field02++;
        if (entity->field02 >= 20) {
            entity->state = CARD_FX_IDLE;
            entity->field02 = 0;
        }
    }
    return;

slide:
    ScaleMatrix(&node->mat, &scaleVec);
    node->mat.t[2] = 0x200;
    SetRotMatrix(&node->mat);
    SetTransMatrix(&node->mat);
    g_primCursor = drawCardShadow(otBucket, g_primCursor);
}

/**
 * @brief Reset the Triple Triad board for a new game.
 *
 * Clears every slot's @c flags / @c element / @c elementMod across the 5x5
 * grid, marks the sentinel border (row/col 0 and 4) with @c TT_CELL_WALL so
 * edge neighbor lookups see a wall, and — when the Elemental rule is active
 * (@c TT_RULE_ELEMENTAL) — gives each interior cell (rows/cols 1..3) a 30%
 * chance of a random board element (@c 1 << rand%8).
 */
void resetTriadBoard(void) {
    s32 row;
    s32 col;

    for (row = 0; row < 5; row++) {
        for (col = 0; col < 5; col++) {
            g_tripleTriadBoard.cells[row][col].flags = 0;
            g_tripleTriadBoard.cells[row][col].element = 0;
            g_tripleTriadBoard.cells[row][col].elementMod = 0;
        }
    }

    for (col = 0; col < 5; col++) {
        g_tripleTriadBoard.cells[0][col].flags |= TT_CELL_WALL;
        g_tripleTriadBoard.cells[4][col].flags |= TT_CELL_WALL;
    }

    for (row = 1; row < 4; row++) {
        g_tripleTriadBoard.cells[row][0].flags |= TT_CELL_WALL;
        g_tripleTriadBoard.cells[row][4].flags |= TT_CELL_WALL;
    }

    if (g_tripleTriadRules & TT_RULE_ELEMENTAL) {
        for (row = 1; row < 4; row++) {
            for (col = 1; col < 4; col++) {
                if (func_80023D04() % 100 < 30) {
                    g_tripleTriadBoard.cells[row][col].element = 1 << (func_80023D04() % 8);
                }
            }
        }
    }
}

/**
 * @brief Place a Triple Triad card on the board and apply the Elemental rule.
 *
 * Clears the per-turn flags (@c JUST_PLACED / @c SAME_MATCHED / @c PLUS_COMBO)
 * across the playable 3x3 cells, then writes the card into
 * @c board->cells[row+1][col+1]: stores @c cardId and @c owner and marks the
 * cell @c OCCUPIED | @c JUST_PLACED. If the cell carries an element and the
 * Elemental rule (@c TT_RULE_ELEMENTAL) is active, sets @c elementMod to +1
 * when the card's element matches the cell's element, or -1 when it differs
 * (the modifier is later added to each of the placed card's edges).
 *
 * @param board  The Triple Triad board.
 * @param cardId Card stat index (into @c g_tripleTriadCardStats).
 * @param owner  Placing player (0 or 1).
 * @param col    Play-area column (cell column is @c col+1).
 * @param row    Play-area row (cell row is @c row+1).
 * @return The placed board cell.
 */
TripleTriadBoardSlot *placeCard(TripleTriadBoard *board, s32 cardId, s32 owner, s32 col, s32 row) {
    s32 r, c;
    TripleTriadBoardSlot *cell;
    s32 e;

    for (r = 1; r < 4; r++) {
        for (c = 1; c < 4; c++) {
            board->cells[r][c].flags &= ~(TT_CELL_JUST_PLACED | TT_CELL_SAME_MATCHED | TT_CELL_PLUS_COMBO);
        }
    }

    cell = &board->cells[row + 1][col + 1];
    cell->owner = owner;
    e = cell->element;
    cell->cardId = cardId;
    cell->flags |= TT_CELL_OCCUPIED | TT_CELL_JUST_PLACED;
    if (e != 0) {
        if (g_tripleTriadCardStats[cardId & 0xFF].element & e) {
            if (g_tripleTriadRules & TT_RULE_ELEMENTAL) {
                cell->elementMod = 1;
            }
        } else {
            if (g_tripleTriadRules & TT_RULE_ELEMENTAL) {
                cell->elementMod = -1;
            }
        }
    }
    return cell;
}

/**
 * @brief Place a card object onto the Triple Triad board.
 *
 * Looks up the @c TripleTriadCardObject at @p entityIdx, places its
 * @c cardId (owner taken from @c initFlags & @c TT_OWNER_MASK) at board
 * cell (@p col, @p row) via @ref placeCard, then tags the placed cell
 * with @p entityIdx so the cell's flip animation is driven by that card.
 *
 * @param board     The 5x5 Triple Triad board.
 * @param entityIdx Slot index into @c g_tripleTriadCardHands.
 * @param col       Board column.
 * @param row       Board row.
 */
void commitCardToBoard(TripleTriadBoard *board, s32 entityIdx, s32 col, s32 row) {
    TripleTriadBoardSlot *cell;

    cell = placeCard(board, g_tripleTriadCardHands[entityIdx].cardId,
                     g_tripleTriadCardHands[entityIdx].initFlags & TT_OWNER_MASK, col, row);
    cell->entityIdx = entityIdx;
}

/**
 * @brief Triple Triad basic-capture rule (with FF8 Elemental modifier).
 *
 * Runs unconditionally after Same/Plus. For each newly-placed card
 * (@c TT_CELL_JUST_PLACED), checks the 4 neighbors and captures any
 * weaker-edge neighbor of a different owner. Each edge value is adjusted
 * by the cell's @c elementMod (signed +1/-1 from the Elemental rule), so
 * the actual comparison is @c (myEdge + myElementMod) vs
 * @c (nbrEdge + nbrElementMod).
 *
 * Already-flipped neighbors from Same/Plus are skipped automatically via
 * the same-owner check.
 *
 * @param board The 5x5 Triple Triad board.
 * @return Number of cards captured this call.
 */
s32 applyBasicCapture(TripleTriadBoard *board) {
    s32 captureCount;
    s32 row, col, dir;
    s32 nbrCol, nbrRow;
    u8 cellOwner, myEdge, nbrEdge;
    s8 cellMod, nbrMod;
    TripleTriadBoardSlot *cell;
    TripleTriadBoardSlot *neighbor;
    TripleTriadCard *cellCard;
    TripleTriadCard *neighborCard;
    TripleTriadDirection *dirOffset;

    captureCount = 0;

    for (row = 1; row <= 3; row++) {
        for (col = 1; col <= 3; col++) {
            cell = &board->cells[row][col];
            if (!(cell->flags & TT_CELL_JUST_PLACED))
                continue;

            cellOwner = cell->owner;
            cellCard  = &g_tripleTriadCardStats[cell->cardId];

            for (dir = 0; dir < TT_DIR_COUNT; dir++) {
                dirOffset = &g_tripleTriadDirectionOffsets[dir];
                nbrCol = col + dirOffset->dx;
                nbrRow = row + dirOffset->dy;
                neighbor = &board->cells[nbrRow][nbrCol];

                if (!(neighbor->flags & TT_CELL_OCCUPIED))
                    continue;
                neighborCard = &g_tripleTriadCardStats[neighbor->cardId];
                if (neighbor->owner == cellOwner)
                    continue;

                myEdge  = cellCard->sides[dir];
                cellMod = cell->elementMod;
                nbrEdge = neighborCard->sides[dir ^ 1];
                nbrMod  = neighbor->elementMod;

                if (!((myEdge + cellMod) > (nbrEdge + nbrMod)))
                    continue;

                neighbor->owner = cellOwner;
                neighbor->flags |= 1 << (dir + 3);   /* captured-from-dir bit */
                captureCount++;
            }
        }
    }

    return captureCount;
}

/**
 * @brief Triple Triad Same-rule resolver (including the Same-Wall extension).
 *
 * Walks the 3x3 active play area; for each newly-placed card
 * (@c TT_CELL_JUST_PLACED), counts how many neighbors have a matching edge
 * value (or, if Same-Wall is enabled, a rank-A edge facing a wall sentinel).
 * If at least 2 matches occur, captures all weaker-or-equal-edge neighbors
 * (different owner) — flipping the owner and recording the direction in the
 * @c flags field.
 *
 * @param board The 5x5 Triple Triad board.
 * @return Number of cards captured this call.
 */
s32 applySameRule(TripleTriadBoard *board) {
    s32 captureCount;
    s32 row, col, dir;
    s32 matchCount;
    s32 nbrCol, nbrRow;
    u8 cellOwner, myEdge, neighborOpposingEdge;
    TripleTriadBoardSlot *cell;
    TripleTriadBoardSlot *neighbor;
    TripleTriadCard *myCardStats;
    TripleTriadCard *neighborStats;
    TripleTriadDirection *dirOffset;

    captureCount = 0;

    for (row = 1; row <= 3; row++) {
        for (col = 1; col <= 3; col++) {
            cell = &board->cells[row][col];

            if (!(cell->flags & TT_CELL_JUST_PLACED))
                continue;

            cellOwner   = cell->owner;
            myCardStats = &g_tripleTriadCardStats[cell->cardId];
            matchCount  = 0;

            for (dir = 0; dir < TT_DIR_COUNT; dir++) {
                dirOffset = &g_tripleTriadDirectionOffsets[dir];
                nbrCol = col + dirOffset->dx;
                nbrRow = row + dirOffset->dy;
                neighbor = &board->cells[nbrRow][nbrCol];

                if (g_tripleTriadRules & TT_RULE_SAME_WALL) {
                    if (myCardStats->sides[dir] == TT_RANK_A) {
                        if (neighbor->flags & TT_CELL_WALL) {
                            matchCount++;
                            continue;
                        }
                    }
                }

                if (!(neighbor->flags & TT_CELL_OCCUPIED))
                    continue;

                neighborStats        = &g_tripleTriadCardStats[neighbor->cardId];
                myEdge               = myCardStats->sides[dir];
                neighborOpposingEdge = neighborStats->sides[dir ^ 1];

                if (myEdge == neighborOpposingEdge) {
                    neighbor->flags |= TT_CELL_SAME_MATCHED;
                    matchCount++;
                }
            }

            if (matchCount < 2)
                continue;

            /* Same rule fired: capture all weaker-or-equal neighbors */
            for (dir = 0; dir < TT_DIR_COUNT; dir++) {
                dirOffset = &g_tripleTriadDirectionOffsets[dir];
                nbrCol = col + dirOffset->dx;
                nbrRow = row + dirOffset->dy;
                neighbor = &board->cells[nbrRow][nbrCol];

                if (!(neighbor->flags & TT_CELL_OCCUPIED))
                    continue;
                neighborStats = &g_tripleTriadCardStats[neighbor->cardId];
                if (neighbor->owner == cellOwner)
                    continue;

                myEdge               = myCardStats->sides[dir];
                neighborOpposingEdge = neighborStats->sides[dir ^ 1];

                if (myEdge < neighborOpposingEdge)
                    continue;

                neighbor->owner = cellOwner;
                neighbor->flags |= 1 << (dir + 3);   /* captured-from-dir bit */
                cell->flags     |= TT_CELL_SAME_MATCHED;
                captureCount++;
            }
        }
    }

    return captureCount;
}

/**
 * @brief Triple Triad Plus-rule resolver.
 *
 * Walks the 3x3 active play area; for each newly-placed card (@c TT_CELL_JUST_PLACED),
 * builds a histogram of @c (myEdge + neighborOppositeEdge) sums across the
 * 4 cardinal neighbors. If any sum is hit by >=2 neighbors, the Plus rule
 * fires and those neighbors are captured (owner flipped, capture-direction
 * bit set in @c flags) provided they aren't already on the placer's side.
 *
 * The 5-wide board layout with a 1-cell sentinel border means neighbor
 * lookups at the edges fall through cleanly without bounds checks.
 *
 * @param board The 5x5 Triple Triad board.
 * @return Number of cards captured this call.
 */
s32 applyPlusRule(TripleTriadBoardSlot board[][TT_BOARD_COLS]) {
    s32 winningSum;
    s32 captures;
    s32 row, col, i;
    s32 maxCount;
    s32 edgeSum;
    s32 nbrCol;
    u8 cellOwner;
    TripleTriadPlusBucket *bucket;
    TripleTriadBoardSlot *cell;
    TripleTriadBoardSlot *neighbor;
    TripleTriadCard *cellCard;
    TripleTriadDirection *offset;
    TripleTriadPlusBucket sumHist[21];   /* indexed by edge-sum value (0..20) */

    captures = 0;

    for (row = 1; row < 4; row++) {
        for (col = 1; col < 4; col++) {
            cell = &board[row][col];
            if (cell->flags & TT_CELL_JUST_PLACED) {
                cellCard = &g_tripleTriadCardStats[cell->cardId];
                cellOwner = cell->owner;

                for (i = 0; i < 21; i++) {
                    sumHist[i].count = 0;
                    sumHist[i].dirMask = 0;
                }

                maxCount = 0;
                for (i = 0; i < 4; i++) {
                    offset = &g_tripleTriadDirectionOffsets[i];
                    nbrCol = col + offset->dx;
                    neighbor = &board[row + offset->dy][nbrCol];
                    if (neighbor->flags & TT_CELL_OCCUPIED) {
                        edgeSum = cellCard->sides[i] +
                                  g_tripleTriadCardStats[neighbor->cardId].sides[i ^ 1];
                        bucket = &sumHist[edgeSum];
                        bucket->count++;
                        bucket->dirMask |= 1 << i;
                        if (bucket->count > maxCount) {
                            maxCount = bucket->count;
                            winningSum = edgeSum;
                        }
                    }
                }

                if (maxCount >= 2) {
                    for (i = 0; i < 4; i++) {
                        offset = &g_tripleTriadDirectionOffsets[i];
                        nbrCol = col + offset->dx;
                        neighbor = &board[row + offset->dy][nbrCol];
                        if ((neighbor->flags & TT_CELL_OCCUPIED) &&
                            ((sumHist[winningSum].dirMask >> i) & 1)) {
                            neighbor->flags |= TT_CELL_PLUS_COMBO;
                            if (neighbor->owner != cellOwner) {
                                neighbor->owner = cellOwner;
                                neighbor->flags |= 1 << (i + 3);   /* captured-from-dir bit */
                                captures++;
                                cell->flags |= TT_CELL_PLUS_COMBO;
                            }
                        }
                    }
                }
            }
        }
    }

    return captures;
}

/**
 * @brief Triple Triad rule orchestrator — runs the rule cascade for a placed card.
 *
 * Resolves all captures triggered by a newly-placed card. If @p mode bit 0 is
 * set, runs the optional rules (Same and Plus) before the unconditional basic
 * capture; otherwise skips straight to basic. Same/Plus are gated on
 * @c g_tripleTriadRules bits @c TT_RULE_SAME / @c TT_RULE_PLUS being active.
 * After all captures, clears @c TT_CELL_JUST_PLACED from the 3x3 active grid
 * so the rules don't fire again next turn.
 *
 * The bit-coded return value lets callers drive the combo loop: after the
 * initial @c mode=1 call, any flipped neighbors are themselves "just placed"
 * and the caller invokes this function again with @c mode=prev_result (bit 0
 * cleared) to do a basic-rule-only combo sweep.
 *
 * @param board The 5x5 Triple Triad board.
 * @param mode  Bit 0 = run Same/Plus rules (initial call); cleared on combo
 *              re-entry. Other bits carried through from prior call's return.
 * @return Bitmap of what fired:
 *           @c 0x2 — basic capture flipped a card (combo step only),
 *           @c 0x4 — Same rule fired,
 *           @c 0x8 — Plus rule fired.
 */
s32 applyCardRules(TripleTriadBoard *board, s32 mode) {
    s32 result;
    s32 row, col;

    result = 0;

    if (mode & 1) {
        if (g_tripleTriadRules & TT_RULE_SAME) {
            result = (applySameRule(board) != 0) << 2;
        }
        if (g_tripleTriadRules & TT_RULE_PLUS) {
            if (applyPlusRule((TripleTriadBoardSlot (*)[TT_BOARD_COLS])board) != 0) {
                result |= 0x8;
            }
        }
    }

    if (applyBasicCapture(board) != 0) {
        if (!(mode & 1)) {
            result |= 0x2;
        }
    }

    for (row = 1; row <= 3; row++) {
        for (col = 1; col <= 3; col++) {
            board->cells[row][col].flags &= ~TT_CELL_JUST_PLACED;
        }
    }

    return result;
}

/**
 * @brief Resolve captured cells: trigger flip animations and clear capture bits.
 *
 * Walks the playable 3x3 cells. For each cell that was captured this turn
 * (any @c TT_CELL_CAP_FROM_* bit set), finds the capture direction via the
 * @c g_captureDirs table (matching the cell's captured-from bit against
 * @c CaptureDir.capBit) and drives that cell entity's flip animation with
 * @c setCardEntityType. It then clears the captured-from bits and marks the cell
 * @c TT_CELL_JUST_PLACED so the next rule pass re-evaluates it.
 *
 * @param board The Triple Triad board.
 */
void resolveCaptures(TripleTriadBoard *board) {
    s32 row, col, dir;

    for (row = 1; row <= 3; row++) {
        for (col = 1; col <= 3; col++) {
            s32 flags = board->cells[row][col].flags;
            if (flags & TT_CELL_CAP_FROM_MASK) {
                for (dir = 0; dir < 4; dir++) {
                    if (flags & g_captureDirs[dir].capBit) {
                        setCardEntityType(board->cells[row][col].entityIdx, g_captureDirs[dir].animType);
                        break;
                    }
                }
                flags &= ~TT_CELL_CAP_FROM_MASK;
                flags |= TT_CELL_JUST_PLACED;
                board->cells[row][col].flags = flags;
            }
        }
    }
}

/**
 * @brief Score a Triple Triad board for @p player (AI board evaluation).
 *
 * The static evaluator the minimax (@ref searchBestMoveStack) calls at its leaves.
 * Sums three contributions into a signed score (higher = better for @p player):
 *  - **Board material**: each occupied cell scores its card's value (plus a base
 *    weight), added for @p player's cells and subtracted for the opponent's.
 *  - **Hand potential**: a fraction of each still-playable hand card's value.
 *  - **Random tiebreaker**: a small random amount so equal positions don't
 *    always tie (difficulty-tunable).
 *
 * @param board  The 5x5 board (the play area is rows/cols 1..3).
 * @param player The seat (0 or 1) to score for.
 * @return The position's heuristic score for @p player.
 */
s32 evaluateBoard(TripleTriadBoard *board, s32 player) {
    s32 score;
    s32 row, col;
    s32 slot;

    score = 0;
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            TripleTriadBoardSlot *cell = &board->cells[row + 1][col + 1];
            if (cell->flags & TT_CELL_OCCUPIED) {
                s32 val = g_evalCardBaseWeight + g_tripleTriadCardValues[cell->cardId];
                if (cell->owner == player) {
                    score += val;
                } else {
                    score -= val;
                }
            }
        }
    }

    for (slot = 0; slot < 5; slot++) {
        if (g_tripleTriadPlayerHands[player].cards[slot].id != 0xFF) {
            score += (g_tripleTriadCardValues[g_tripleTriadPlayerHands[player].cards[slot].id] * g_evalHandPotentialWeight) >> 12;
        }
    }

    if (g_evalRandomRange != 0) {
        return score + func_80023D04() % (g_evalRandomRange + 1);
    }

    return score;
}

/**
 * @brief Recursive Triple Triad AI move search (resumable depth-limited minimax).
 *
 * Tries each legal (card, cell) move at this ply: places it on a copy of the
 * board, runs the capture cascade, and recurses for the opponent one ply
 * deeper, scoring leaves with @ref evaluateBoard. The AI seat maximizes a
 * difficulty-weighted score while the opponent minimizes the raw one; the best
 * move is recorded in @p node, and a completed node's bound lets later passes
 * prune. The search is time-sliced against a shared placement budget — when it
 * runs out the recursion yields, and because each ply's iterators persist in
 * the workspace the next call resumes exactly where it left off.
 *
 * @param board  Position to search (copied per placement; never modified).
 * @param player Seat to move at this ply (0/1).
 * @param node   This ply's search state; the child ply uses @c node[1].
 * @param depth  Remaining search depth in plies.
 *
 * @return 0 when this node completed a full pass (best move/scores recorded),
 *         1 if @p depth was already exhausted (caller evaluates the position),
 *         2 if the time-slice budget ran out (yield; resume next slice),
 *         3 if the root advanced to its next hand card.
 */
s32 searchBestMoveStack(TripleTriadBoard *board, s32 player, AiMove *node, s32 depth) {
    TripleTriadBoard boardCopy;
    s32 cardId;
    s32 weight;
    s32 weighted;
    s32 score;
    s32 ruleMode;
    s32 result;
    s32 product;

    if (depth <= 0) {
        return AI_RESULT_LEAF;
    }
    while (g_aiPlacementBudget > 0) {
        cardId = g_tripleTriadPlayerHands[player].cards[node->card].id;
        if (cardId != 0xFF) {
            if (!(board->cells[node->row + 1][node->col + 1].flags & TT_CELL_OCCUPIED)) {
                g_aiPlacementBudget--;
                boardCopy = *board;

                placeCard(&boardCopy, cardId, player, node->col, node->row);
                ruleMode = 1;
                weight = 0x1000;
                while (applyCardRules(&boardCopy, ruleMode) != 0) {
                    ruleMode &= -2;
                    if (player == g_tripleTriadCurrentSeat) {
                        weight = g_evalDifficultyWeight;
                    }
                }

                g_tripleTriadPlayerHands[player].cards[node->card].id = 0xFF;
                result = searchBestMoveStack(&boardCopy, player ^ 1, &node[1], depth - 1);
                g_tripleTriadPlayerHands[player].cards[node->card].id = cardId;

                switch (result) {
                case AI_RESULT_DONE:
                    score = node[1].bestScore;
                    product = node[1].bestWeighted * weight;
                    weighted = product >> 12;
                    break;
                case AI_RESULT_LEAF: {
                    s32 eval = evaluateBoard(&boardCopy, g_tripleTriadCurrentSeat);
                    product = eval * weight;
                    score = eval;
                    weighted = product >> 12;
                    break;
                }
                case AI_RESULT_YIELD:
                    return AI_RESULT_YIELD;
                }

                if (player == g_tripleTriadCurrentSeat) {
                    if (node->noBest || node->bestWeighted < weighted) {
                        node->bestWeighted = weighted;
                        node->bestScore = score;
                        node->noBest = 0;
                        node->bestCol = node->col;
                        node->bestRow = node->row;
                        node->bestCard = node->card;
                    }
                } else {
                    if (node->noBest || score < node->bestScore) {
                        node->bestWeighted = weighted;
                        node->bestScore = score;
                        node->noBest = 0;
                        node->bestCol = node->col;
                        node->bestRow = node->row;
                        node->bestCard = node->card;
                    }
                }

                if (node->checkBound) {
                    if (player == g_tripleTriadCurrentSeat) {
                        if (node->bound < score) {
                            node->col = 2;
                            node->row = 2;
                            node->card = 4;
                        }
                    } else {
                        if (weighted < node->bound) {
                            node->col = 2;
                            node->row = 2;
                            node->card = 4;
                        }
                    }
                }
            }
        }

        node->col++;
        if (node->col >= 3) {
            node->col = 0;
            node->row++;
            if (node->row >= 3) {
                node->row = 0;
                node->card++;
                if (node->card >= 5) {
                    node->card = 0;
                    node->noBest = 1;
                    node->checkBound = 1;
                    if (player == g_tripleTriadCurrentSeat) {
                        node->bound = node->bestScore;
                    } else {
                        node->bound = node->bestWeighted;
                    }
                    return AI_RESULT_DONE;
                }
                if (node == g_aiSearchStack) {
                    return AI_RESULT_ROOT_NEXT;
                }
            }
        }
    }
    return AI_RESULT_YIELD;
}

/**
 * @brief Live entry point of the AI move search (pool-allocated twin of
 *        @ref searchBestMoveStack).
 *
 * Identical search to @ref searchBestMoveStack, but each recursion level draws
 * its scratch board from the per-frame work pool instead of the CPU stack. This
 * is the variant @ref updateAiTurn actually runs; @ref searchBestMoveStack (no
 * in-overlay caller) appears to be its unused sibling.
 *
 * @param board  Position to search (copied per placement; never modified).
 * @param player Seat to move at this ply (0/1).
 * @param node   This ply's search state; the child ply uses @c node[1].
 * @param depth  Remaining search depth in plies.
 *
 * @return 0 when this node completed a full pass (best move/scores recorded),
 *         1 if @p depth was already exhausted (caller evaluates the position),
 *         2 if the time-slice budget ran out (yield; resume next slice),
 *         3 if the root advanced to its next hand card.
 */
s32 searchBestMove(TripleTriadBoard *board, s32 player, AiMove *node, s32 depth) {
    TripleTriadBoard *boardCopy;
    s32 cardId;
    s32 weight;
    s32 weighted;
    s32 score;
    s32 ruleMode;
    s32 result;
    s32 product;

    if (depth <= 0) {
        return AI_RESULT_LEAF;
    }
    boardCopy = (TripleTriadBoard *)scratchAlloc(sizeof(TripleTriadBoard));
    while (g_aiPlacementBudget > 0) {
        cardId = g_tripleTriadPlayerHands[player].cards[node->card].id;
        if (cardId != 0xFF) {
            if (!(board->cells[node->row + 1][node->col + 1].flags & TT_CELL_OCCUPIED)) {
                g_aiPlacementBudget--;
                *boardCopy = *board;

                placeCard(boardCopy, cardId, player, node->col, node->row);
                ruleMode = 1;
                weight = 0x1000;
                while (applyCardRules(boardCopy, ruleMode) != 0) {
                    ruleMode &= -2;
                    if (player == g_tripleTriadCurrentSeat) {
                        weight = g_evalDifficultyWeight;
                    }
                }

                g_tripleTriadPlayerHands[player].cards[node->card].id = 0xFF;
                result = searchBestMove(boardCopy, player ^ 1, &node[1], depth - 1);
                g_tripleTriadPlayerHands[player].cards[node->card].id = cardId;

                switch (result) {
                case AI_RESULT_DONE:
                    score = node[1].bestScore;
                    product = node[1].bestWeighted * weight;
                    weighted = product >> 12;
                    break;
                case AI_RESULT_LEAF: {
                    s32 eval = evaluateBoard(boardCopy, g_tripleTriadCurrentSeat);
                    product = eval * weight;
                    score = eval;
                    weighted = product >> 12;
                    break;
                }
                case AI_RESULT_YIELD:
                    scratchFree(sizeof(TripleTriadBoard));
                    return AI_RESULT_YIELD;
                }

                if (player == g_tripleTriadCurrentSeat) {
                    if (node->noBest || node->bestWeighted < weighted) {
                        node->bestWeighted = weighted;
                        node->bestScore = score;
                        node->noBest = 0;
                        node->bestCol = node->col;
                        node->bestRow = node->row;
                        node->bestCard = node->card;
                    }
                } else {
                    if (node->noBest || score < node->bestScore) {
                        node->bestWeighted = weighted;
                        node->bestScore = score;
                        node->noBest = 0;
                        node->bestCol = node->col;
                        node->bestRow = node->row;
                        node->bestCard = node->card;
                    }
                }

                if (node->checkBound) {
                    if (player == g_tripleTriadCurrentSeat) {
                        if (node->bound < score) {
                            node->col = 2;
                            node->row = 2;
                            node->card = 4;
                        }
                    } else {
                        if (weighted < node->bound) {
                            node->col = 2;
                            node->row = 2;
                            node->card = 4;
                        }
                    }
                }
            }
        }

        node->col++;
        if (node->col >= 3) {
            node->col = 0;
            node->row++;
            if (node->row >= 3) {
                node->row = 0;
                node->card++;
                if (node->card >= 5) {
                    node->card = 0;
                    node->noBest = 1;
                    node->checkBound = 1;
                    if (player == g_tripleTriadCurrentSeat) {
                        node->bound = node->bestScore;
                    } else {
                        node->bound = node->bestWeighted;
                    }
                    scratchFree(sizeof(TripleTriadBoard));
                    return AI_RESULT_DONE;
                }
                if (node == g_aiSearchStack) {
                    scratchFree(sizeof(TripleTriadBoard));
                    return AI_RESULT_ROOT_NEXT;
                }
            }
        }
    }
    scratchFree(sizeof(TripleTriadBoard));
    return AI_RESULT_YIELD;
}

/**
 * @brief Advance the AI player's Triple Triad turn: search for a move, play the
 *        selection animation, then commit the chosen card to the board.
 *
 * Runs a small state machine (re-entered once per frame), a no-op while card
 * input is globally disabled:
 *  - reset the move workspace;
 *  - run the (time-sliced) move search, highlighting the candidate card, and
 *    re-search if the search asks to;
 *  - play the selection animation for the chosen card;
 *  - commit it to the board.
 *
 * @param node AI turn/placement controller for the current player.
 * @return 2 once the card has been placed; 0 while still working or when input
 *         is disabled.
 */
s32 updateAiTurn(AiTurnNode *node) {
    TripleTriadBoard board;

    if (g_tripleTriadInputFlags & TT_INPUT_DISABLED) {
        return 0;
    }

    while (1) {
        switch (node->state) {
        case AI_TURN_INIT: {
            s32 i;
            for (i = 0; i < 9; i++) {
                g_aiSearchStack[i].col = 0;
                g_aiSearchStack[i].row = 0;
                g_aiSearchStack[i].card = 0;
                g_aiSearchStack[i].noBest = 1;
                g_aiSearchStack[i].checkBound = 0;
            }
            node->state = AI_TURN_SEARCH;
            node->sub = AI_SEARCH_PREP;
            break;
        }
        case AI_TURN_SEARCH: {
            s32 sub = node->sub;
            switch (sub) {
            case AI_SEARCH_PREP:
                g_aiSearchTimer = 10;
                node->cardSlot = D_801D3462;
                node->sub++;
                break;
            case AI_SEARCH_RUN:
                highlightCardSlot(g_tripleTriadCurrentSeat, node->cardSlot);
                g_aiPlacementBudget = 100;
                node->result = searchBestMove(&g_tripleTriadBoard, g_tripleTriadCurrentSeat, g_aiSearchStack, node->unk10);
                if ((node->result & 0xFF) == AI_RESULT_YIELD) {
                    g_aiSearchTimer--;
                    return 0;
                }
                if (g_tripleTriadPlayerHands[g_tripleTriadCurrentSeat].cards[node->cardSlot].id == 0xFF) {
                    g_aiSearchTimer = 0;
                }
                node->sub++;
                break;
            case AI_SEARCH_WAIT:
                highlightCardSlot(g_tripleTriadCurrentSeat, node->cardSlot);
                if (g_aiSearchTimer > 0) {
                    g_aiSearchTimer--;
                    return 0;
                }
                if (node->result == AI_RESULT_ROOT_NEXT) {
                    node->sub = AI_SEARCH_PREP;  /* re-run the search from the top */
                } else {
                    /* sub == AI_TURN_ANIMATE; using the cached local (not the
                       literal) is load-bearing for the match. */
                    node->state = sub;
                    node->sub = 0;               /* start the ANIMATE frame counter */
                }
                break;
            }
            break;
        }
        case AI_TURN_ANIMATE:
            highlightCardSlot(g_tripleTriadCurrentSeat, D_801D3466);
            node->sub++;
            if ((node->sub & 0xFF) < AI_ANIM_FRAMES) {
                return 0;
            }
            node->state = AI_TURN_PLACE;
            node->sub = 0;
            break;
        case AI_TURN_PLACE:
            setCardObjectAction(
                g_tripleTriadPlayerHands[g_tripleTriadCurrentSeat].cards[g_aiSearchStack[0].bestCard].entityIdx, 2,
                g_aiSearchStack[0].bestCol, g_aiSearchStack[0].bestRow);
            commitCardToBoard(&g_tripleTriadBoard,
                g_tripleTriadPlayerHands[g_tripleTriadCurrentSeat].cards[g_aiSearchStack[0].bestCard].entityIdx,
                g_aiSearchStack[0].bestCol, g_aiSearchStack[0].bestRow);
            return 2;
        }
    }
}

/**
 * @brief Set up the AI player's Triple Triad turn and spawn its turn handler.
 *
 * Runs once when it becomes seat @p seat's turn:
 *  1. Builds both players' working search hands from the live card objects.
 *  2. Unless the Open rule is on, randomizes the AI's guess of the opponent's
 *     hidden hand (biased by each card's level).
 *  3. Spawns the per-frame @ref updateAiTurn handler and loads the difficulty
 *     tuning (search depth, evaluation weights, per-card values).
 *
 * @param seat Player/seat index taking the turn.
 * @return The AI turn-node list head.
 */
u8 *spawnAiTurn(s32 seat) {
    AiTurnNode *node;
    s32 mult;
    s32 slotCount;
    s32 player;
    s32 weight;
    s32 card;
    s32 i;

    slotCount = 9;
    for (player = 0; player < 2; player++) {
        for (card = 0; card < 5; card++) {
            s32 entity;
            entity = findCardSlot(player, 0, card);
            g_tripleTriadPlayerHands[player].cards[card].entityIdx = entity;
            if (entity < 0) {
                slotCount--;
                g_tripleTriadPlayerHands[player].cards[card].id = 0xFF;
            } else {
                g_tripleTriadPlayerHands[player].cards[card].id = g_tripleTriadCardHands[entity].cardId;
            }
        }
    }

    if (!(g_tripleTriadRules & TT_RULE_OPEN) && !(D_80082C97 & 0x10)) {
        s32 opp = seat ^ 1;
        for (i = 0; i < 5; i++) {
            s32 r1, r2, divisor;
            g_tripleTriadPlayerHands[opp].cards[i].entityIdx = -1;
            r1 = func_80023D04();
            r2 = func_80023D04();
            divisor = D_801A2C48[seat][i] / 11 + 1;
            g_tripleTriadPlayerHands[opp].cards[i].id = (r1 % divisor) * 11 + r2 % 11;
        }
    }

    initObjList(g_aiTurnList, g_aiTurnPool, sizeof(AiTurnNode), 1);
    node = (AiTurnNode *)allocObjNode(g_aiTurnList, (ObjNodeFn)updateAiTurn);
    g_tripleTriadCurrentSeat = seat;
    node->seat = seat;
    {
        s32 depth = g_aiSearchDepthTable[D_80082C90.field_07 & 7][slotCount - 1];
        node->state = 0;
        node->sub = 0;
        node->unk10 = depth;
    }

    mult = g_aiWeightTable[D_80082C90.field_06 & 7].w1;
    g_evalCardBaseWeight = g_aiWeightTable[D_80082C90.field_06 & 7].w0;
    g_evalCardValueScale = mult;
    g_evalRandomRange = g_aiWeightTable[D_80082C90.field_06 & 7].w2;
    g_evalDifficultyWeight = g_aiWeightTable[D_80082C90.field_06 & 7].w3;
    g_evalHandPotentialWeight = g_aiWeightTable[D_80082C90.field_06 & 7].w4;
    weight = mult;

    for (i = 0; i < 110; i++) {
        g_tripleTriadCardValues[i] = (g_tripleTriadCardStats[i].pad05[0] * weight / 200) >> 12;
    }

    return g_aiTurnList;
}

/**
 * @brief Initialize the Triple Triad task list.
 */
void initTriadTaskPool(void) {
    initObjList(g_taskList, g_taskPool, 76, 16);
}

/**
 * @brief Per-frame update of the Triple Triad task list.
 */
void processTriadTasks(void) {
    updateObjectList(g_taskList);
}
