#include "common.h"
#include "item.h"
#include "tripletriad.h"
#include "gamestate.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libc.h"
#include "tripletriad/be_object1.h"
#include "tripletriad/be_object1b.h"
#include "tripletriad/be_object2.h"
#include "tripletriad/be_object3.h"
#include "tripletriad/be_object4.h"

/** @brief Allocate a tripletriad object from the @c g_taskList pool with the
 *         given per-frame callback; returns the new object (0 if pool is full). */
s32 spawnTaskNode(ObjNodeFn a0) {
    return allocObjNode(g_taskList, a0);
}

/**
 * @brief Per-frame card scale animation: scales the sprite in, holds, then scales out.
 *
 * Three states driven by @c node->state: state 0 scales the width up from 0x40 toward
 * 0x180 over 5 frames (via the @c /5 ramp @c ((field0D<<12)/5)); state 1 holds @c width
 * at 0x40 for 0x28 frames; state 2 scales back down over 5 frames. Each frame it rebuilds
 * the @c CardScaleSprite at the @c g_primCursor cursor and links it into @c g_otBase[3].
 *
 * @param node State-machine driver node (@c field0E selects the 0x3802/0x3842 sprite variant).
 * @return 2 once the scale-out completes (state 2), else 0.
 */
s32 updateCardScaleSprite(ScriptStateNode *node) {
    CardScaleSprite *prim;
    s32 q;

    prim = g_primCursor;
    switch (node->state) {
    case CARD_SCALE_IN:
        q = ((++node->field0D & 0xFF) << 12) / 5;
        prim->width = (-((q * 5) << 6) >> 12) + 0x180;
        if (node->field0D >= 5) {
            node->state = CARD_SCALE_HOLD;
            node->field0D = 0;
        }
        break;
    case CARD_SCALE_HOLD:
        prim->width = 0x40;
        if ((++node->field0D & 0xFF) >= 0x28) {
            node->state = CARD_SCALE_OUT;
            node->field0D = 0;
        }
        break;
    case CARD_SCALE_OUT:
        q = ((++node->field0D & 0xFF) << 12) / 5;
        prim->width = ((q - ((q * 5) << 6)) >> 12) + 0x40;
        if (node->field0D >= 5) {
            return 2;
        }
        break;
    }
    prim->tag = 0x5000000;
    prim->code = 0xE100060E;
    prim->color = 0x64808080;
    prim->field0E = 0x50;
    prim->field10 = 0;
    if (node->field0E == 0) {
        prim->field11 = 0;
        prim->field12 = 0x3802;
    } else {
        prim->field11 = 0x40;
        prim->field12 = 0x3842;
    }
    prim->field14 = 0xFF;
    prim->field16 = 0x40;
    AddPrim(g_otBase + 3, prim);
    g_primCursor = (u8 *)prim + 0x18;
    return 0;
}

/**
 * @brief Per-frame card scale animation variant: scale in, hold, scale out.
 *
 * Twin of @c updateCardScaleSprite with a shorter hold (state 1 lasts 0x19 frames) and a
 * fixed sprite variant (no @c field0E branch — always @c field11=0x80 /
 * @c field12=0x3882). Each frame it rebuilds the @c CardScaleSprite at the
 * @c g_primCursor cursor and links it into @c g_otBase[3].
 *
 * @param node State-machine driver node (state at 0x0C, frame counter at 0x0D).
 * @return 2 once the scale-out completes (state 2), else 0.
 *
 * @note The @c color store uses a @c volatile cast as a scheduling barrier: unlike
 *       @c updateCardScaleSprite (whose @c field0E if/else creates a basic-block boundary
 *       after @c field10), this function stores the sprite fields unconditionally,
 *       so without the barrier cc1 floats the @c color store down past @c field10
 *       to fill the @c g_otBase load-delay shadow. The barrier keeps it in source
 *       order, matching the original.
 */
s32 updateCardScaleSpriteShort(ScriptStateNode *node) {
    CardScaleSprite *prim;
    s32 q;

    prim = g_primCursor;
    switch (node->state) {
    case CARD_SCALE_IN:
        q = ((++node->field0D & 0xFF) << 12) / 5;
        prim->width = (-((q * 5) << 6) >> 12) + 0x180;
        if (node->field0D >= 5) {
            node->state = CARD_SCALE_HOLD;
            node->field0D = 0;
        }
        break;
    case CARD_SCALE_HOLD:
        prim->width = 0x40;
        if ((++node->field0D & 0xFF) >= 0x19) {
            node->state = CARD_SCALE_OUT;
            node->field0D = 0;
        }
        break;
    case CARD_SCALE_OUT:
        q = ((++node->field0D & 0xFF) << 12) / 5;
        prim->width = ((q - ((q * 5) << 6)) >> 12) + 0x40;
        if (node->field0D >= 5) {
            return 2;
        }
        break;
    }
    prim->tag = 0x5000000;
    prim->code = 0xE100060E;
    *(volatile u32 *)&prim->color = 0x64808080;
    prim->field0E = 0x50;
    prim->field10 = 0;
    prim->field11 = 0x80;
    prim->field12 = 0x3882;
    prim->field14 = 0xFF;
    prim->field16 = 0x40;
    AddPrim(g_otBase + 3, prim);
    g_primCursor = (u8 *)prim + 0x18;
    return 0;
}

/**
 * @brief Per-frame builder for the two-layer screen gradient fade.
 *
 * Builds two @c GradientFadeQuad primitives at the @c g_primCursor cursor and links each into
 * @c g_otBase[3]. The @c variant (2/3/4) selects the palette base and first-layer blend
 * word; the four vertex colours ramp with @c node->frame (each vertex 5 steps behind the
 * previous, clamped to 0..0x80 then packed as @c 0x3E000000|c|c<<8|c<<16). @c frame advances
 * each frame (capped at 0xFF).
 *
 * @param node Gradient-fade node (variant at 0x0E, ramp counter at 0x0D, done flag at 0x22).
 * @return 2 once @c done is set, else 0.
 *
 * @note Each layer's last colour store (@c color3) uses a @c volatile cast as a scheduling
 *       barrier: it stops cc1 pulling that store into the @c AddPrim jal delay slot, so
 *       the slot instead holds the @c g_otBase+3 pointer arithmetic, matching the original.
 */
s32 buildGradientFade(GradientFadeNode *node) {
    GradientFadeQuad *prim;
    s32 colors[4];
    s32 i;
    s32 seed;
    s32 c;
    s32 lvl;

    if (node->done == 1) {
        return 2;
    }
    prim = g_primCursor;

    switch (node->variant) {
    case 2:
        lvl = 0;
        prim->field0E = 0x3801;
        break;
    case 3:
        lvl = 0x30;
        prim->field0E = 0x3841;
        break;
    case 4:
        lvl = 0x60;
        prim->field0E = 0x3881;
        break;
    }

    prim->field1A = 0x2D;
    prim->tag = 0xC000000;
    prim->x2 = 0x40;
    prim->x0 = 0x40;
    prim->x3 = 0x13F;
    prim->x1 = 0x13F;
    prim->y1 = 0x58;
    prim->y0 = 0x58;
    prim->y3 = 0x88;
    prim->y2 = 0x88;
    prim->code3 = 0xFF;
    prim->code1 = 0xFF;
    prim->code2 = 0;
    prim->code0 = 0;
    prim->pal1 = lvl;
    prim->pal0 = lvl;
    prim->pal3 = lvl + 0x30;
    prim->pal2 = lvl + 0x30;

    seed = node->frame;
    for (i = 0; i < 4; i++) {
        if (seed < 0) {
            colors[i] = 0;
        } else if (seed < 0xF) {
            colors[i] = seed * 128 / 15;
        } else {
            colors[i] = 0x80;
        }
        c = colors[i];
        colors[i] = c | (c << 8) | (c << 16) | 0x3E000000;
        seed -= 5;
    }
    prim->color0 = colors[0];
    prim->color1 = colors[2];
    prim->color2 = colors[1];
    *(volatile u32 *)&prim->color3 = colors[3];
    AddPrim(g_otBase + 3, prim);
    prim++;

    prim->tag = 0xC000000;
    prim->field1A = 0x4D;
    prim->field0E = 0x3FC0;
    prim->x2 = 0x40;
    prim->x0 = 0x40;
    prim->x3 = 0x13F;
    prim->x1 = 0x13F;
    prim->y1 = 0x58;
    prim->y0 = 0x58;
    prim->y3 = 0x88;
    prim->y2 = 0x88;
    prim->code3 = 0xFF;
    prim->code1 = 0xFF;
    prim->code2 = 0;
    prim->code0 = 0;
    prim->pal1 = lvl;
    prim->pal0 = lvl;
    prim->pal3 = lvl + 0x30;
    prim->pal2 = lvl + 0x30;
    prim->color0 = colors[0];
    prim->color1 = colors[2];
    prim->color2 = colors[1];
    *(volatile u32 *)&prim->color3 = colors[3];
    AddPrim(g_otBase + 3, prim);

    if (node->frame < 0xFF) {
        node->frame++;
    }
    g_primCursor = (u8 *)prim + 0x34;
    return 0;
}

/**
 * @brief Per-frame card colour fade-in / fade-out sprite builder.
 *
 * Sibling of @c updateCardScaleSprite / @c updateCardScaleSpriteShort: each frame it rebuilds a
 * @c CardScaleSprite at the @c g_primCursor cursor and links it into @c g_otBase[3],
 * animating the sprite's @c color. A two-state machine drives it:
 *  - state 0 fades the colour up: @c alpha = (v*255)>>12 with
 *    @c v = ((++frame & 0xFF)<<12)/10, packing 0x66-prefixed RGB; advances to
 *    state 1 once @c frame reaches 0xA.
 *  - state 1 fades back down: @c alpha = ((v - 127*v)>>12)+0xFF with @c v = .../20
 *    (the @c v-(v<<7) == -127*v idiom), packing 0x64-prefixed RGB until @c frame
 *    reaches 0x14, after which it holds a solid 0x64808080 colour while
 *    re-initialising the whole primitive.
 *
 * Each branch links its primitive and advances the cursor with @c prim++
 * (a @c CardScaleSprite is 0x18 bytes), writing the new tail back to @c g_primCursor.
 *
 * @param node State-machine driver (state at 0x0C, frame counter at 0x0D).
 * @return 2 when finished (@c node->field22 already 1, or while @c g_gradFadeCount gates
 *         it off returns 0), else 0.
 */
s32 updateCardColorFade(ScriptStateNode *node) {
    CardScaleSprite *prim;
    s32 v;
    s32 a;
    s32 t;
    s32 a2;
    s32 t2;

    if (node->field22 == 1) {
        return 2;
    }
    if (g_gradFadeCount >= 2) {
        return 0;
    }
    prim = g_primCursor;
    prim->tag = 0x5000000;
    prim->code = 0xE100062D;
    prim->width = 0x40;
    prim->field0E = 0x58;
    prim->field10 = 0;
    prim->field11 = 0;
    prim->field14 = 0xFF;
    prim->field16 = 0x30;
    switch (node->state) {
    case CARD_FADE_IN:
        v = ((++node->field0D & 0xFF) << 12) / 10;
        a = (v * 255) >> 12;
        t = (a << 8) | 0x66000000;
        a = a | t | (a << 16);
        *(volatile u32 *)&prim->color = a;
        prim->field12 = 0x3801;
        AddPrim(g_otBase + 3, prim++);
        if (node->field0D >= 0xA) {
            node->state = CARD_FADE_OUT;
            node->field0D = 0;
        }
        break;
    case CARD_FADE_OUT:
        if ((u32)node->field0D < 0x14) {
            v = ((++node->field0D & 0xFF) << 12) / 20;
            a2 = ((v - (v << 7)) >> 12) + 0xFF;
            prim->field12 = 0x3801;
            t2 = (a2 << 8) | 0x64000000;
            a2 = a2 | t2 | (a2 << 16);
            *(volatile u32 *)&prim->color = a2;
            AddPrim(g_otBase + 3, prim++);
        } else {
            prim->color = 0x64808080;
            prim->field12 = 0x3801;
            prim->code = 0xE100062D;
            prim->field10 = 0;
            prim->field11 = 0;
            prim->field16 = 0x30;
            prim->tag = 0x5000000;
            prim->width = 0x40;
            prim->field0E = 0x58;
            prim->field14 = 0xFF;
            AddPrim(g_otBase + 3, prim++);
        }
        break;
    }
    g_primCursor = prim;
    return 0;
}

/**
 * @brief Allocate a node in g_gradFadeList list with a callback from g_gradFadeCallbacks.
 *
 * Looks up a callback pointer from the g_gradFadeCallbacks table at index @p a0,
 * allocates a node via allocObjNodeFront, and initializes several fields.
 *
 * @param a0 Index into the g_gradFadeCallbacks callback table.
 */
void spawnGradientFade(s32 a0) {
    GradientFadeNode *node = allocObjNodeFront(g_gradFadeList, g_gradFadeCallbacks[a0]);
    if (node != 0) {
        node->variant = a0;
        node->field20 = 0;
        node->done = 0;
        node->state = 0;
        node->frame = 0;
    }
}

/**
 * @brief Store a byte value at offset 0x22 of a structure.
 *
 * @param a0 Pointer to structure base.
 * @param a1 Byte value to store at offset 0x22.
 */
void setNodeDoneFlag(u8 *a0, s32 a1) {
    a0[0x22] = a1;
}

/**
 * @brief Initialize the g_gradFadeList list with node pool g_gradFadePool.
 *
 * Sets up a linked list with node size 0x24 and capacity 0x4.
 */
void initGradientFadeList(void) {
    initObjList(g_gradFadeList, g_gradFadePool, 0x24, 0x4);
}

/**
 * @brief Tick the screen-fade effect objects, recording how many remain active.
 *
 * Updates the @c g_gradFadeList gradient-fade effect list and stores the live count
 * in @c g_gradFadeCount.
 */
void updateFadeEffects(void) {
    g_gradFadeCount = updateObjectList(g_gradFadeList);
}

/**
 * @brief Per-frame card slide/scale animation sweep over the g_scriptActions table.
 *
 * Walks all 2x5 @c ScriptEntry slots and, for each active one (@c status 1/2/3),
 * advances a 15-frame animation: status 1 slides the card in (posY eases from
 * @c f6 toward 0xE0), status 2 slides it out, status 3 flips it (Y-rotation
 * ramp + Z-scale). Each finished slot resets to idle (@c status = @c field02 = 0).
 * Active slots build a rotation/translation matrix from the entry's rotation
 * vector + computed position and link a sprite via drawTriadCard.
 *
 * @return 0.
 */
s32 updateScriptCardAnims(void)
{
    s32 col;
    s32 row;
    ScriptEntry *e;
    TmpBuf *tmp;
    s32 frame;
    s32 s0;
    s32 scratch[2];

    tmp = (TmpBuf *)scratchAlloc(0x2C);
    for (row = 0; row < 2; row++) {
        for (col = 0; col < 5; col++) {
            e = &g_scriptActions[row][col];
            switch (e->status) {
            case SCRIPT_ANIM_NONE:
                break;
            case SCRIPT_ANIM_SLIDE_IN:
                if (e->field02 == 0) {
                    playTriadSfx(0x59);
                }
                frame = ++e->field02;
                s0 = ((frame & 0xFF) << 12) / 15;
                s0 = rsin(s0 / 4);
                tmp->f0 = e->row;
                tmp->f2 = e->col;
                layoutCardSlot((u8 *)tmp, &tmp->pos);
                e->posX = tmp->pos.vx;
                e->posY = ((tmp->pos.vy - 0xE0) * s0 >> 12) + 0xE0;
                e->posZ = tmp->pos.vz;
                e->sort = tmp->pos.pad;
                if (e->field02 >= 0xF) {
                    e->status = SCRIPT_ANIM_NONE;
                    e->field02 = 0;
                }
                break;
            case SCRIPT_ANIM_SLIDE_OUT:
                frame = ++e->field02;
                s0 = ((frame & 0xFF) << 12) / 15;
                s0 = rsin(s0 / 4);
                tmp->f0 = e->row;
                tmp->f2 = e->col;
                layoutCardSlot((u8 *)tmp, &tmp->pos);
                e->posX = tmp->pos.vx;
                e->posY = ((0xE0 - tmp->pos.vy) * s0 >> 12) + tmp->pos.vy;
                e->posZ = tmp->pos.vz;
                e->sort = tmp->pos.pad;
                if (e->field02 >= 0xF) {
                    e->status = SCRIPT_ANIM_NONE;
                    e->field02 = 0;
                }
                break;
            case SCRIPT_ANIM_FLIP:
                frame = ++e->field02;
                s0 = ((frame & 0xFF) << 12) / 15;
                e->actionId = (-(s0 << 11) >> 12) + 0x800;
                s0 = rsin(s0 / 2);
                e->posZ = 0x200 - ((s0 << 6) >> 12);
                if (e->field02 >= 0xF) {
                    e->status = SCRIPT_ANIM_NONE;
                    e->field02 = 0;
                }
                break;
            }
            if (e->marker != 0xFF) {
                RotMatrixYXZ((u8 *)&e->field06, &tmp->mtx);
                tmp->mtx.t[0] = e->posX;
                tmp->mtx.t[1] = e->posY;
                tmp->mtx.t[2] = e->posZ;
                SetRotMatrix(&tmp->mtx);
                SetTransMatrix(&tmp->mtx);
                g_primCursor = drawTriadCard(e->marker, e->row | TT_SHOW_ELEMENT | TT_USE_STATS,
                                           g_otBase + e->sort, g_primCursor);
            }
        }
    }
    scratchFree(0x2C);
    return 0;
}

/**
 * Checks if any active slot has a pending action.
 *
 * Searches through a 2D table (2 rows of 5 entries, each 22 bytes apart,
 * rows 110 bytes apart) for an entry where byte 0 is not 0xFF and byte 1
 * is non-zero.
 *
 * @return 1 if a pending action was found, 0 otherwise.
 */
s32 hasPendingScriptAction(void) {
    s32 row = 0;
    u8 *base = (u8 *)g_scriptActions;
    s32 marker = 0xFF;
    s32 rowOff = row;
    s32 col;
    s32 colOff;
    do {
        col = 0;
        colOff = rowOff;
        do {
            u8 *entry = (u8 *)(colOff + (s32)base);
            if (entry[0] != marker) {
                if (entry[1] != 0) {
                    return 1;
                }
            }
            col++;
            colOff += 0x16;
        } while (col < 5);
        row++;
        rowOff += 0x6E;
    } while (row < 2);
    return 0;
}

/**
 * @brief Build a card-list display string for 11 consecutive card entries.
 *
 * For each index @p base + 0..10 whose owned count (@c func_80023B14) is
 * non-negative, appends the card's name (@c func_80023A54), the icon byte
 * 0xF4, the count rendered as decimal digit glyphs (+0x53, 1-3 digits), and
 * the icon byte 0xF5; every entry is terminated by a @c 2 separator. The final
 * separator is overwritten with a NUL.
 *
 * @param out  Destination string buffer.
 * @param base First card index.
 */
void buildCardListString(u8 *out, s32 base) {
    s32 i;
    s32 count;
    u8 *name;
    u8 c;

    for (i = 0; i < 11; i++) {
        count = func_80023B14(base + i);
        if (count >= 0) {
            name = func_80023A54(base + i);
            c = *name;
            while (c != 0) {
                name++;
                *out = c;
                c = *name;
                out++;
            }
            *out++ = 0xF4;
            if (count >= 100) {
                *out++ = (count / 100) % 10 + 0x53;
            }
            if (count >= 10) {
                *out++ = (count / 10) % 10 + 0x53;
            }
            *out++ = count % 10 + 0x53;
            *out++ = 0xF5;
        }
        *out++ = 2;
    }
    *(out - 1) = 0;
}

/**
 * @brief Per-frame interactive card-claim sequencer (state machine, @c node->state 0-3).
 *
 * Drives the post-match "build your hand from the claimed cards" flow. State 0 clears
 * the hand slots and arms the UI; state 1 adds a card (on the @c g_addCardEdge 0x40/0x80
 * input edge) or removes one (on the @c g_removeCardEdge 0x10 edge), one per frame, into
 * @c g_scriptActions / @c D_801A2C48 until the hand holds five; state 2 polls a confirmation
 * message gate; state 3 runs the fade-out. The case-1, @c hasPendingScriptAction and @c r<0
 * "still waiting" paths share a single return via @c ret0.
 *
 * @param node State-machine node (allocated by allocObjNode with state at +0x10).
 * @return 0 while the sequence is still running; the state-3 expression once it ends.
 */
s32 runHandBuildSequencer(ScriptCtx *node) {
    s32 i;
    u8 card;
    s32 r;
    s32 idx;
    u8 *hand;
    u8 *handRow;

    g_removeCardEdge = g_padRepeat[2];
    g_addCardEdge = g_padPressed[2];
    while (1) {
        switch (node->state) {
        case HAND_BUILD_INIT:
            for (i = 0; i < 5; i++) {
                D_801A2C48[node->counter][i] = 0xFF;
            }
            func_800A44CC();
            func_800A44B0(1);
            node->field13 = 0;
            g_handBuildCount = 0;
            node->state = HAND_BUILD_PICK;
            node->subState = 0;
            break;
        case HAND_BUILD_PICK:
            if (node->subState == 0) {
                g_tripleTriadInputFlags |= TT_INPUT_HAND_BUILD;
                node->subState++;
                goto ret0;
            }
            if (g_addCardEdge == 0x40 || g_addCardEdge == 0x80) {
                if (g_cardDisplaySlot >= 0) {
                    card = g_cardDisplaySlot;
                    if (g_handBuildCount >= 5) {
                        node->state = HAND_BUILD_PICK;
                        node->subState = 0;
                        break;
                    }
                    if (func_80023B14(card) <= 0) {
                        node->state = HAND_BUILD_PICK;
                        node->subState = 0;
                        break;
                    }
                    (g_scriptActions[node->counter] + g_handBuildCount)->status = SCRIPT_ANIM_SLIDE_IN;
                    (g_scriptActions[node->counter] + g_handBuildCount)->field02 = 0;
                    g_scriptActions[node->counter][g_handBuildCount].marker = card;
                    hand = D_801A2C48[node->counter];
                    idx = g_handBuildCount;
                    hand[idx] = card;
                    g_handBuildCount = ++idx;
                    modifyItemQuantity(card, D_80082C95);
                    playTriadSfx(1);
                    if (g_handBuildCount != 5) {
                        node->state = HAND_BUILD_PICK;
                        node->subState = 0;
                        break;
                    }
                    g_tripleTriadInputFlags &= ~TT_INPUT_HAND_BUILD;
                    node->state = HAND_BUILD_CONFIRM;
                    node->subState = 0;
                    break;
                }
            }
            if (g_removeCardEdge == 0x10 && g_handBuildCount > 0) {
                handRow = D_801A2C48[node->counter];
                g_handBuildCount--;
                card = handRow[g_handBuildCount];
                D_801A2C48[node->counter][g_handBuildCount] = 0xFF;
                markItemPresent(card);
                node->state = HAND_BUILD_PICK;
                node->subState = 0;
                (g_scriptActions[node->counter] + g_handBuildCount)->status = SCRIPT_ANIM_SLIDE_OUT;
                (g_scriptActions[node->counter] + g_handBuildCount)->field02 = 0;
                playTriadSfx(9);
            }
            return 0;
        case HAND_BUILD_CONFIRM:
            if (node->subState == 0) {
                if (hasPendingScriptAction() != 0) {
                ret0:
                    return 0;
                }
                func_800A44B0(0);
                func_800A1D68(6, (u8 *)&D_80182686 - 6 + D_80182686, 0);
                node->subState++;
            }
            r = func_800A20F4(6);
            if (r < 0) {
                goto ret0;
            }
            func_800A2054(6);
            if (r == 0) {
                node->state = HAND_BUILD_FADE;
                node->subState = 0;
                break;
            }
            func_800A44B0(1);
            node->state = HAND_BUILD_PICK;
            node->subState = 0;
            break;
        case HAND_BUILD_FADE:
            if (node->subState == 0) {
                func_800A44BC();
            }
            node->subState++;
            return ((node->subState & 0xFF) >= 0xF) << 1;
        }
    }
}

/**
 * @brief Build a player's Triple Triad hand by drawing cards.
 *
 * If @p arg1 is non-zero it first deals up to 5 cards of that rarity/type from the
 * @c D_80078658 table (cards 0x4D+), each gated by an RNG roll against the deal
 * threshold @c D_80082C90.field_09 (halved after the first hit). It then fills the
 * remaining slots by drawing from random tiers: @c D_80082C90.field_08 is a 7-bit
 * tier mask whose set bits build @c tierList (tier bases 0, 0xB, 0x16, ...), and each
 * card is @c tierList[rng%numTiers] + rng%11, rejecting the blocked card 0x2F and any
 * duplicate already in the hand. Results are written to @c D_801A2C48[player].
 *
 * @param player Hand index (0 or 1) — selects the 5-slot row in @c D_801A2C48.
 * @param arg1   Rarity/type filter for the seeded deal; 0 skips the seeded phase.
 *
 * @note The reject flag reuses @c rng1, and the @c do{}while(0) wrapping its reset is a
 *       scheduling barrier that keeps the reset after the draw (matches the original).
 *       The rarity probe uses @c *((rarity + i) + 0x4D) — the @c rarity[i + 0x4D] form
 *       reassociates the offset and does not match.
 */
void dealRarityHand(s32 player, s32 arg1) {
    s32 count;
    s32 i;
    s32 threshold;
    s32 tierList[7];
    s32 numTiers;
    s32 bits;
    s32 tierVal;
    s32 card;
    s32 rng1;
    s32 rng2;
    s32 j;
    u8 *rarity;

    count = 0;
    rarity = D_80078658;
    threshold = D_80082C90.field_09;
    if (arg1 != 0) {
        for (i = 0; i < 0x21; i++) {
            if (*((rarity + i) + 0x4D) == arg1) {
                if ((func_80023D04() % 100) < threshold) {
                    D_801A2C48[player][count] = i + 0x4D;
                    count++;
                    threshold = D_80082C90.field_09 >> 1;
                    if (count >= 5) {
                        break;
                    }
                }
            }
        }
    }
    numTiers = 0;
    if (D_80082C90.field_08 == 0) {
        bits = 1;
    } else {
        bits = D_80082C90.field_08;
    }
    i = 0;
    tierVal = i;
    for (; i < 7; i++) {
        if ((bits >> i) & 1) {
            tierList[numTiers] = tierVal;
            numTiers++;
        }
        tierVal += 0xB;
    }
    for (i = count; i < 5; i++) {
        do {
            rng1 = func_80023D04();
            rng2 = func_80023D04();
            card = tierList[rng1 % numTiers] + (rng2 % 11);
            do { rng1 = 0; } while (0);
            if (card == 0x2F) {
                rng1 = 1;
            } else {
                for (j = 0; j < i; j++) {
                    if (card == D_801A2C48[player][j]) {
                        rng1 = 1;
                        break;
                    }
                }
            }
        } while (rng1 != 0);
        D_801A2C48[player][i] = card;
    }
}

/**
 * @brief Build a random 5-card hand for the given player by drawing owned cards.
 *
 * Repeatedly draws a random card id in 0..109 (@c func_80023D04 % 110); whenever
 * the player owns at least one of that card (@c func_80023B14 > 0) it places the id
 * in the player's hand row @c D_801A2C48[hand] and adjusts its owned quantity via
 * @c modifyItemQuantity. Continues until 5 cards have been placed.
 *
 * @param hand Player index (0 or 1); selects the 5-slot row in @c D_801A2C48.
 * @param arg1 Quantity delta passed to @c modifyItemQuantity for each drawn card.
 */
void buildRandomOwnedHand(s32 hand, s32 arg1) {
    u8 *row;
    u8 *b;
    s32 i;
    s32 card;

    b = D_801A2C48[0];
    i = 0;
    row = b + hand * 5;
loop:
    card = func_80023D04() % 110;
    if (func_80023B14(card) > 0) {
        row[i] = card;
        i++;
        modifyItemQuantity(card, arg1);
        do {
        } while (0);
    }
    if (i < 5) {
        goto loop;
    }
}

/**
 * @brief Per-frame handler: copies a player's hand into the script-action table.
 *
 * Drives one card slot per 5 frames. @c node->state is the current slot index
 * (0..4) and doubles as the state; @c node->counter is the player index. On the
 * first frame of each slot (@c subState 0) it copies the hand card
 * @c D_801A2C48[player][slot] into @c g_scriptActions[player][slot]: status 1, the
 * card id as the @c marker, and an @c actionId selected by the layout type
 * (@c 0x800 for the offset-hand layout @c D_801A2C70 >= 3, else 0). After 5
 * frames it advances to the next slot. Once all slots are done (@c state >= 5)
 * it returns 2 if no queued actions remain (@c hasPendingScriptAction == 0), else 0.
 *
 * @param node Handler context (state/slot at 0x10, subState at 0x11, player at 0x12).
 * @return 2 when all hands are queued and drained, else 0.
 *
 * @note The @c status / @c field02 stores use the pointer form
 *       @c (g_scriptActions[player]+slot)->field to force the row index into the lower
 *       register (row-major codegen); the @c marker / @c actionId stores keep the
 *       array form, matching the original register allocation.
 */
s32 updateHandToScriptTable(ScriptCtx *node) {
    u8 card;

    if (node->state >= 5) {
        return (hasPendingScriptAction() == 0) << 1;
    }
    if (node->subState == 0) {
        card = D_801A2C48[node->counter][node->state];
        (g_scriptActions[node->counter] + node->state)->status = SCRIPT_ANIM_SLIDE_IN;
        (g_scriptActions[node->counter] + node->state)->field02 = 0;
        g_scriptActions[node->counter][node->state].marker = card;
        if (D_801A2C70[node->counter] < 3) {
            g_scriptActions[node->counter][node->state].actionId = 0;
        } else {
            g_scriptActions[node->counter][node->state].actionId = 0x800;
        }
    }
    node->subState++;
    if (node->subState >= 5) {
        node->subState = 0;
        node->state++;
    }
    return 0;
}

/**
 * @brief Set up the per-player hand and spawn its setup handler, per the active rules.
 *
 * Initialises the @c g_setupHandlerList pool, then (by @c g_tripleTriadRules) builds the
 * player's hand and chooses the per-frame handler:
 *  - @c TT_RULE_NO_CLAIM: random hand (@c dealRarityHand, delta 0) + @c updateHandToScriptTable.
 *  - @c TT_RULE_RANDOM: auto-dealt hand — @c dealRarityHand if this player uses the offset-hand
 *    layout (@c D_801A2C70 >= 3) else @c buildRandomOwnedHand — + @c updateHandToScriptTable.
 *  - otherwise: @c dealRarityHand + @c updateHandToScriptTable for the offset-hand layout, else the
 *    interactive @c runHandBuildSequencer.
 *
 * The spawned node's @c counter is set to @p arg0 and its state cleared.
 *
 * @param arg0 Player/slot index.
 * @return The initialised pool (@c g_setupHandlerList).
 * @note The third branch's @c D_801A2C70[node->counter] test reads @c node before it is
 *       assigned (the original relies on the stale register value); preserved to match.
 */
s32 setupPlayerHand(s32 arg0) {
    ScriptCtx *node;

    initObjList(g_setupHandlerList, g_setupHandlerPool, 0x14, 1);

    if (g_tripleTriadRules & TT_RULE_NO_CLAIM) {
        dealRarityHand(arg0, 0);
        node = (ScriptCtx *)allocObjNode(g_setupHandlerList, (ObjNodeFn)updateHandToScriptTable);
    } else if (g_tripleTriadRules & TT_RULE_RANDOM) {
        if (D_801A2C70[arg0] >= 3) {
            dealRarityHand(arg0, D_80082C95);
        } else {
            buildRandomOwnedHand(arg0, D_80082C95);
        }
        node = (ScriptCtx *)allocObjNode(g_setupHandlerList, (ObjNodeFn)updateHandToScriptTable);
    } else if (D_801A2C70[node->counter] >= 3) {
        dealRarityHand(arg0, D_80082C95);
        node = (ScriptCtx *)allocObjNode(g_setupHandlerList, (ObjNodeFn)updateHandToScriptTable);
    } else {
        node = (ScriptCtx *)allocObjNode(g_setupHandlerList, (ObjNodeFn)runHandBuildSequencer);
    }

    node->counter = arg0;
    node->state = 0;
    node->subState = 0;

    return (s32)g_setupHandlerList;
}

/**
 * @brief Add a rendering command entry based on the alternate screen index.
 *
 * Reads g_drawBufferIndex, XORs with 1 to get the alternate index, computes
 * an offset of index * 92 into g_drawEnvs, and calls queueLoadImage
 * with the resulting pointer and &g_tripleTriadBgTim0[0x14].
 *
 * @return Always 0.
 */
s32 reloadSetupBuffer(void) {
    s32 idx = g_drawBufferIndex ^ 1;
    queueLoadImage(&g_drawEnvs[idx].clip, &g_tripleTriadBgTim0[0x14]);
    return 0;
}

/**
 * @brief Triple Triad script callback: 5-state machine driving an intro/setup sequence.
 *
 * Registered via initTripleTriadScripts. Each invocation advances the state machine
 * one tick; returns 0 while running, returns from any case.
 *
 * State 0: warmup. Calls startFadeToBlack(0xF) once, ticks 15 frames.
 * State 1: setup. Calls setupPlayerHand(counter) per counter, polls
 *          updateObjectList until ready; tries counter 0..1, then branches
 *          to TRIAD_SETUP_CLEAR (if g_tripleTriadRules & TT_RULE_OPEN) else TRIAD_SETUP_DONE.
 * State 2: clear sweep. Every 5 ticks, marks queued actions in
 *          g_scriptActions[row][col] complete for column = 4 down to 0;
 *          transitions to state 3 once column 0 is processed.
 * State 3: wait. Calls hasPendingScriptAction once, idles 0x1E frames.
 * State 4: done. Sets g_tripleTriadState = TT_STATE_PLAY and exits.
 *
 * @param ctx Callback context (state at +0x10, subState at +0x11).
 * @return 0 while progressing, 0 on completion.
 */
s32 runTriadSetupSequence(ScriptCtx *ctx) {
    while (1) {
        switch (ctx->state) {
        case TRIAD_SETUP_WARMUP:
            if (ctx->subState == 0) {
                startFadeToBlack(0xF);
            }
            ctx->subState++;
            if (ctx->subState < 0xF) {
                return 0;
            }
            ctx->counter = 0;
            ctx->state = TRIAD_SETUP_HANDS;
            ctx->subState = 0;
            break;

        case TRIAD_SETUP_HANDS:
            if (ctx->subState == 0) {
                ctx->cachedResult = setupPlayerHand(ctx->counter);
                ctx->subState++;
            }
            if (updateObjectList((u8 *)ctx->cachedResult) != 0) {
                return 0;
            }
            ctx->counter++;
            if (ctx->counter < 2) {
                ctx->state = TRIAD_SETUP_HANDS;
                ctx->subState = 0;
                break;
            }
            if (g_tripleTriadRules & TT_RULE_OPEN) {
                ctx->state = TRIAD_SETUP_CLEAR;
                ctx->subState = 0;
                break;
            }
            ctx->state = TRIAD_SETUP_DONE;
            ctx->subState = 0;
            break;

        case TRIAD_SETUP_CLEAR: {
            s32 row;
            s32 col;
            if ((u8)(ctx->subState % 5) == 0) {
                col = 4 - (u8)(ctx->subState / 5);
                for (row = 0; row < 2; row++) {
                    if (g_scriptActions[row][col].actionId != 0) {
                        g_scriptActions[row][col].status = SCRIPT_ANIM_FLIP;
                        g_scriptActions[row][col].field02 = 0;
                    }
                }
                if (col == 0) {
                    ctx->state = TRIAD_SETUP_WAIT;
                    ctx->subState = 0;
                }
            }
            ctx->subState++;
            return 0;
        }

        case TRIAD_SETUP_WAIT:
            if (ctx->subState == 0) {
                if (hasPendingScriptAction() != 0) {
                    return 0;
                }
            }
            ctx->subState++;
            if (ctx->subState < 0x1E) {
                return 0;
            }
            ctx->state = TRIAD_SETUP_DONE;
            ctx->subState = 0;
            break;

        case TRIAD_SETUP_DONE:
            g_tripleTriadState = TT_STATE_PLAY;
            return 0;
        }
    }
}

/**
 * @brief Per-frame display-node spawn for some g_cardDisplaySlot-driven animation.
 *
 * Validates the current slot index in g_cardDisplaySlot, allocates a 40-byte
 * @c DispNode via @c scratchAlloc, and initializes it based on the
 * current phase counter (@c g_fadePhase / @c g_fadePhaseMirror):
 * - phase < 10:  rotating-arc setup, angle scaled via @c rsin
 * - phase < 300: simple static node (scale = 0x18)
 * - else:        squashed angle via @c rsin + phase mirror
 *
 * On any failure (slot out of range or @c func_80023B14 returns negative),
 * sets @c g_lastActiveSlot = @c -1 and returns 0.
 *
 * @return Always 0.
 */
s32 updateCardDisplaySpawn(void) {
    DispNode *node;
    s32 cur;

    if ((u32)g_cardDisplaySlot < 0x6E) {
        if (func_80023B14(g_cardDisplaySlot) >= 0) {
            node = (DispNode *)scratchAlloc(0x28);
            if (g_lastActiveSlot != g_cardDisplaySlot) {
                g_fadePhaseMirror = 0;
                g_fadePhase = 0;
            }
            cur = g_fadePhase;
            if (cur < 0xA) {
                s32 newCur = cur + 1;
                s32 v = (newCur << 12) / 10;
                g_fadePhase = newCur;
                g_fadeLastAngle = v;
                g_fadeLastAngle = rsin(v / 4);
                node->unk04 = 0;
                node->phase = 0;
                node->angle = 0;
                RotMatrixYXZ(node, node->subNode);
                {
                    s32 r = g_fadeLastAngle;
                    node->charType = 0x44;
                    node->unk24 = 0x200;
                    node->scale = (-(r * 88) >> 12) + 0x70;
                }
            } else if (cur < 0x12C) {
                g_fadePhase = cur + 1;
                node->unk04 = 0;
                node->phase = 0;
                node->angle = 0;
                RotMatrixYXZ(node, node->subNode);
                node->charType = 0x44;
                node->scale = 0x18;
                node->unk24 = 0x200;
            } else {
                s32 newScale = g_fadePhaseMirror + 0x20;
                s32 rounded;
                s32 v;
                g_fadePhaseMirror = newScale;
                rounded = newScale + (s32)((u32)newScale >> 31);
                v = rsin(rounded >> 1);
                v = -v;
                if (v < 0) v += 7;
                node->angle = v >> 3;
                node->unk04 = 0;
                node->phase = (u16)g_fadePhaseMirror;
                RotMatrixYXZ(node, node->subNode);
                node->charType = 0x44;
                node->scale = 0x18;
                node->unk24 = 0x200;
            }
            SetRotMatrix(node->subNode);
            SetTransMatrix(node->subNode);
            g_primCursor = drawTriadCard((u8)g_cardDisplaySlot, TT_OWNER_MASK | TT_USE_STATS | TT_SHOW_ELEMENT, &g_otBase[3], g_primCursor);
            scratchFree(0x28);
            g_lastActiveSlot = g_cardDisplaySlot;
        } else {
            g_lastActiveSlot = -1;
        }
    } else {
        g_lastActiveSlot = -1;
    }
    return 0;
}

/**
 * @brief Initialise the script-handler subsystem and its object pool.
 *
 * Sets up the @c g_scriptHandlerList pool (10 entries of 0x14 bytes), spawns the
 * persistent handler nodes (state machine @c runTriadSetupSequence, @c updateScriptCardAnims,
 * @c updateCardDisplaySpawn, @c reloadSetupBuffer), and clears the @c g_scriptActions 2x5
 * @c ScriptEntry table (each entry tagged with @c marker 0xFF and its
 * row/column cached).
 *
 * @return The initialised pool (@c g_scriptHandlerList).
 */
u8 *initTripleTriadScripts(void) {
    ScriptCtx *node;
    ScriptEntry *e;
    u8 *base;
    s32 row;
    s32 rowOff;
    s32 marker;
    s32 col;
    s32 colOff;

    initObjList(g_scriptHandlerList, g_scriptHandlerPool, 0x14, 0xA);
    node = (ScriptCtx *)allocObjNode(g_scriptHandlerList, (ObjNodeFn)runTriadSetupSequence);
    node->state = 0;
    node->subState = 0;
    allocObjNode(g_scriptHandlerList, (ObjNodeFn)updateScriptCardAnims);
    row = 0;
    base = (u8 *)g_scriptActions;
    marker = 0xFF;
    rowOff = row;
    do {
        col = 0;
        colOff = rowOff;
        do {
            e = (ScriptEntry *)(colOff + (s32)base);
            e->col = col;
            e->status = SCRIPT_ANIM_NONE;
            e->marker = marker;
            e->row = row;
            e->field06 = 0;
            e->actionId = 0;
            e->field0A = 0;
            col++;
            colOff += 0x16;
        } while (col < 5);
        row++;
        rowOff += 0x6E;
    } while (row < 2);
    allocObjNode(g_scriptHandlerList, (ObjNodeFn)updateCardDisplaySpawn);
    allocObjNode(g_scriptHandlerList, (ObjNodeFn)reloadSetupBuffer);
    return g_scriptHandlerList;
}

/**
 * @brief Per-frame handler for a screen colour-fade (@c FadeObject).
 *
 * Builds a full-screen TILE primitive whose colour is interpolated from
 * @c startColor toward @c endColor by the elapsed fraction, links it (plus a
 * trailing blend-control primitive) into OT bucket 2, and advances the
 * primitive pool. Returns 0 while fading; once @c frame reaches @c duration it
 * stages @c endColor into @c g_stagedFadeColor and returns 2 (fade complete).
 *
 * @param obj The fade object.
 * @return 0 while in progress, 2 when complete.
 */
s32 updateScreenFade(FadeObject *obj) {
    TILE *prim;
    s32 progress;
    s32 frame;

    prim = g_primCursor;
    prim->tag = 0x5000000;
    prim->w = 0x180;
    prim->y0 = 0;
    prim->x0 = 0;
    prim->h = 0xE0;
    progress = (obj->frame << 12) / obj->duration;
    SetFarColor(obj->endColor[0], obj->endColor[1], obj->endColor[2]);
    DpqColor((CVECTOR *)obj->startColor, progress, (CVECTOR *)&prim->r0);
    prim->code = 0x62;
    AddPrim(g_otBase + 2, prim);
    SetDrawTPage((u8 *)prim + sizeof(TILE), 1, 1, 0x40);
    AddPrim(g_otBase + 2, (u8 *)prim + sizeof(TILE));
    g_primCursor = (u8 *)prim + 0x18;
    frame = (u16)obj->frame;
    obj->frame = frame + 1;
    if ((s16)frame >= obj->duration) {
        memcpy(g_stagedFadeColor, obj->endColor, 4);
        return 2;
    }
    return 0;
}

/**
 * @brief Start a full-screen fade to black over @p duration frames.
 *
 * Spawns a @c FadeObject driven by @c updateScreenFade. The fade begins from the
 * currently-staged color (@c g_stagedFadeColor) and ends at black; black is then
 * staged so any following fade starts from black.
 *
 * @param duration Fade length in frames.
 */
void startFadeToBlack(s32 duration) {
    FadeObject *obj;

    obj = (FadeObject *)spawnTaskNode((ObjNodeFn)updateScreenFade);
    if (obj != NULL) {
        obj->frame = 0;
        obj->duration = duration;
        memcpy(obj->startColor, g_stagedFadeColor, 4);
        *(s32 *)g_stagedFadeColor = 0;
        *(s32 *)obj->endColor = 0;
    }
}

/**
 * @brief Start a full-screen fade to white over @p duration frames.
 *
 * Identical to @c startFadeToBlack but ends at white (0xFFFFFF), which is also
 * staged into @c g_stagedFadeColor for the next fade.
 *
 * @param duration Fade length in frames.
 */
void startFadeToWhite(s32 duration) {
    FadeObject *obj;

    obj = (FadeObject *)spawnTaskNode((ObjNodeFn)updateScreenFade);
    if (obj != NULL) {
        obj->frame = 0;
        obj->duration = duration;
        memcpy(obj->startColor, g_stagedFadeColor, 4);
        *(s32 *)g_stagedFadeColor = 0xFFFFFF;
        *(s32 *)obj->endColor = 0xFFFFFF;
    }
}

/**
 * @brief Per-frame board render/update loop for the post-game card-claim board.
 *
 * Walks the ten board cells (@c g_activeCardObjs). For each, it builds a rotation/translation
 * @c MATRIX from the cell's @c rot* and @c base* fields, then runs the cell's animation
 * state (@c field8): 1 = slide in from off-screen; 2 = flip (rotates @c rotY, toggles
 * @c fieldA owner at the half-way point, bobs @c baseZ); 3/4 = capture lift-and-fade
 * (rises, shows the captured card's name banner, then settles) for the two capture
 * directions. Cells with @c flags bit 0 set are transformed (@c SetRotMatrix /
 * @c SetTransMatrix) and drawn via @c drawTriadCard into the OT at @c sort + 9.
 *
 * @return 0 (driven again next frame by the spawning handler).
 */
s32 updateClaimBoard(void) {
    MATRIX *m;
    s32 i;
    s32 s0;
    ActiveObj *cell;
    s32 s7;
    s32 t;
    s32 n;
    s32 inc;

    m = (MATRIX *)scratchAlloc(0x20);
    for (i = 0; i < 10; i++) {
        cell = &g_activeCardObjs[i];
        RotMatrixYXZ(&cell->rotX, m);
        m->t[0] = cell->baseX;
        m->t[1] = cell->baseY;
        m->t[2] = cell->baseZ;
        s7 = cell->fieldA | TT_SHOW_ELEMENT | TT_USE_STATS;
        if (cell->field8) switch (cell->field8) {
        case CLAIM_ANIM_SLIDE_IN:
            s0 = (cell->field9 << 12) / 30;
            s0 = rsin(s0 / 4);
            if (cell->fieldA == 0) {
                m->t[0] = ((cell->baseX + 0x1A0) * s0 >> 12) - 0x1A0;
            } else {
                m->t[0] = ((cell->baseX - 0x1A0) * s0 >> 12) + 0x1A0;
            }
            cell->field9++;
            if ((cell->field9 & 0xFF) >= 0x1E) {
                cell->field8 = CLAIM_ANIM_NONE;
                cell->field9 = 0;
            }
            break;
        case CLAIM_ANIM_FLIP:
            if (cell->field9 == 0) {
                cell->sort -= 5;
                playTriadSfx(0x5A);
            }
            inc = cell->field9 + 1;
            cell->field9 = inc;
            n = inc & 0xFF;
            s0 = (n << 12) / 20;
            if (n == 0xA) {
                cell->fieldA ^= 1;
            }
            cell->rotY = s0;
            t = rsin(s0 / 2);
            cell->baseZ = 0x200 - ((t << 5) >> 12);
            if ((cell->field9 & 0xFF) >= 0x14) {
                cell->field8 = CLAIM_ANIM_NONE;
                cell->field9 = 0;
                cell->sort += 5;
            }
            break;
        case CLAIM_ANIM_CAPTURE_A:
            s0 = cell->field9;
            if (s0 < 0x1E) {
                if (s0 == 0) {
                    cell->sort = 0;
                    playTriadSfx(0x59);
                }
                s0 = (s0 << 12) / 30;
                t = rsin(s0 / 2);
                m->t[0] = cell->baseX + (-cell->baseX * s0 >> 12);
                m->t[1] = cell->baseY + (-cell->baseY * s0 >> 12) + (-((t * 7) << 4) >> 12);
                m->t[2] = cell->baseZ + ((0x100 - cell->baseZ) * s0 >> 12);
                cell->field9++;
            } else {
                s0 -= 0x1E;
                if (s0 < 2) {
                    m->t[0] = 0;
                    m->t[1] = 0;
                    m->t[2] = 0x100;
                    if (s0 == 0) {
                        strcpy(g_nameBannerBuf, func_80023A54(cell->cardId));
                        func_80047C74(g_nameBannerBuf, (u8 *)&D_80182692 - 0x12 + D_80182692);
                        func_800A1D68(1, g_nameBannerBuf, 0);
                        cell->field9++;
                    } else if ((g_padPressed[0] | g_padPressed[1]) != 0) {
                        D_801D4454 = 1;
                        func_800A2054(1);
                        playTriadSfx(isItemPresent(cell->cardId) ? 0x59 : 0x12);
                        cell->field9++;
                    }
                } else {
                    s0 -= 2;
                    if (s0 < 0xF) {
                        m->t[0] = 0;
                        m->t[2] = 0x100;
                        s0 = (s0 << 12) / 15;
                        m->t[1] = ((s0 * 7) << 4) >> 12;
                        cell->field9++;
                    } else {
                        cell->field8 = CLAIM_ANIM_NONE;
                        cell->field9 = 0;
                        cell->flags &= ~1;
                    }
                }
            }
            break;
        case CLAIM_ANIM_CAPTURE_B:
            s0 = cell->field9;
            if (s0 < 0x1E) {
                if (s0 == 0) {
                    cell->sort = 0;
                    playTriadSfx(0x59);
                }
                s0 = (s0 << 12) / 30;
                t = rsin(s0 / 2);
                m->t[0] = cell->baseX + (-cell->baseX * s0 >> 12);
                m->t[1] = cell->baseY + (-cell->baseY * s0 >> 12) + (((t * 7) << 4) >> 12);
                m->t[2] = cell->baseZ + ((0x100 - cell->baseZ) * s0 >> 12);
                cell->field9++;
            } else {
                s0 -= 0x1E;
                if (s0 < 2) {
                    m->t[0] = 0;
                    m->t[1] = 0;
                    m->t[2] = 0x100;
                    if (s0 == 0) {
                        strcpy(g_nameBannerBuf, func_80023A54(cell->cardId));
                        func_80047C74(g_nameBannerBuf, (u8 *)&D_80182696 - 0x16 + D_80182696);
                        func_800A1D68(1, g_nameBannerBuf, 0);
                        cell->field9++;
                    } else if ((g_padPressed[0] | g_padPressed[1]) != 0) {
                        D_801D4454 = 1;
                        func_800A2054(1);
                        playTriadSfx(0x59);
                        cell->field9++;
                    }
                } else {
                    s0 -= 2;
                    if (s0 < 0xF) {
                        m->t[0] = 0;
                        m->t[2] = 0x100;
                        s0 = (s0 << 12) / 15;
                        m->t[1] = -((s0 * 7) << 4) >> 12;
                        cell->field9++;
                    } else {
                        cell->field8 = CLAIM_ANIM_NONE;
                        cell->field9 = 0;
                        cell->flags &= ~1;
                    }
                }
            }
            break;
        }
        if (cell->flags & 1) {
            SetRotMatrix(m);
            SetTransMatrix(m);
            g_primCursor = drawTriadCard(cell->cardId, s7, g_otBase + (cell->sort + 9), g_primCursor);
        }
    }
    scratchFree(0x20);
    return 0;
}

/**
 * @brief Check if any active card object has a pending action.
 *
 * Iterates through 10 entries in g_activeCardObjs (stride 0x20). For each entry
 * where bit 0 of the word at offset +4 is set and the byte at offset +8
 * is non-zero, returns 1. Returns 0 if no such entry is found.
 *
 * @return 1 if any active object has a pending action, 0 otherwise.
 */
s32 hasPendingCardObj(void) {
    s32 i;

    for (i = 0; i < 10; i++) {
        if ((g_activeCardObjs[i].flags & 1) && g_activeCardObjs[i].field8 != 0) {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Add a rendering command for the alternate screen buffer.
 *
 * Reads g_drawBufferIndex, XORs with 1 to get the alternate index, computes
 * an offset of index * 92 into g_drawEnvs, and calls queueLoadImage
 * with the resulting pointer and &g_tripleTriadBgTim1[0x14].
 *
 * @return Always 0.
 */
s32 reloadClaimBuffer(void) {
    s32 idx = g_drawBufferIndex ^ 1;
    queueLoadImage(&g_drawEnvs[idx].clip, &g_tripleTriadBgTim1[0x14]);
    return 0;
}

/**
 * @brief Interactive "keep a captured card" selection sequence (Random/Trade rule).
 *
 * State 0 builds a banner string into @c g_bannerBuf — a prefix (string-pool entry
 * @c D_8018268A), the acting seat as a letter (@c g_sweepTarget + '!'), then a suffix
 * (@c D_8018268E) — and shows it. State 1 runs the picker: it opens a selection
 * substate, then on the player's choice (@c g_substatePhase) either shows a card's detail
 * (1), toggles a card for capture while tracking the remaining budget @c g_capturedCount
 * (2), or deselects one (3). State 2 confirms via @c func_800A20F4: on accept it sets
 * @c D_801D444C and returns 2; on cancel it returns to the picker.
 *
 * @param node State node (state 0x0C, per-pass guard 0x0D, acting seat 0x0E).
 * @return 2 once the selection is confirmed, else 0.
 */
s32 runKeepCardSelect(ScriptStateNode *node) {
    /* 8 bytes of stack the original reserves (frame vars=8); unreferenced on every
       path. Purpose uncertain — likely leftover scratch from the string build above. */
    u8 scratch[4];
    u8 *dst;
    u8 *src;
    u8 c;
    u8 c2;
    ActiveObj *cell;
    s32 r;

    while (1) {
        switch (node->state) {
        case KEEP_SEL_BANNER:
            src = (u8 *)&D_8018268A - 0xA + D_8018268A;
            dst = g_bannerBuf;
            c = *src;
            while (c != 0) {
                src++;
                *dst = c;
                c = *src;
                dst++;
            }
            *dst = g_sweepTarget + 0x21;
            src = (u8 *)&D_8018268E - 0xE + D_8018268E;
            while (dst++, (c2 = *src) != 0) {
                src++;
                *dst = c2;
            }
            *dst = 0;
            func_800A1D68(1, g_bannerBuf, 0);
            g_capturedCount = 0;
            node->state = KEEP_SEL_PICK;
            node->field0D = 0;
            break;
        case KEEP_SEL_PICK:
            if (node->field0D == 0) {
                if (hasPendingCardObj() != 0) {
                    return 0;
                }
                activateMenuSubstate((node->field0E ^ 1) + 4, 0, 2, 0);
                node->field0D++;
                break;
            }
            switch (g_substatePhase) {
            case TT_SUBPHASE_ACTIVE:
                showCardDetail(g_activeCardObjs[D_801D335C.field0].cardId);
                return 0;
            case TT_SUBPHASE_CONFIRM:
                cell = &g_activeCardObjs[D_801D335C.field0];
                if (cell->fieldA != node->field0E) {
                    if (g_capturedCount < g_sweepTarget) {
                        cell->field8 = CLAIM_ANIM_FLIP;
                        g_activeCardObjs[D_801D335C.field0].field9 = 0;
                        playTriadSfx(1);
                        g_capturedCount++;
                    } else {
                        node->state = KEEP_SEL_PICK;
                        node->field0D = 0;
                        playTriadSfx(0x10);
                        continue;
                    }
                } else {
                    if (g_capturedCount <= 0) {
                        break;
                    }
                    cell->field8 = CLAIM_ANIM_FLIP;
                    g_activeCardObjs[D_801D335C.field0].field9 = 0;
                    playTriadSfx(9);
                    g_capturedCount--;
                }
                if (g_capturedCount == g_sweepTarget) {
                    node->state = KEEP_SEL_CONFIRM;
                    node->field0D = 0;
                    continue;
                }
                node->state = KEEP_SEL_PICK;
                node->field0D = 0;
                return 0;
            case TT_SUBPHASE_CANCEL:
                cell = &g_activeCardObjs[D_801D335C.field0];
                if (cell->fieldA == node->field0E) {
                    cell->field8 = CLAIM_ANIM_FLIP;
                    g_activeCardObjs[D_801D335C.field0].field9 = 0;
                    playTriadSfx(9);
                    g_capturedCount--;
                }
                break;
            default:
                continue;
            }
            node->state = KEEP_SEL_PICK;
            node->field0D = 0;
            break;
        case KEEP_SEL_CONFIRM:
            if (node->field0D == 0) {
                if (hasPendingCardObj() != 0) {
                    return 0;
                }
                node->field0D++;
                break;
            }
            if (node->field0D == 1) {
                func_800A1D68(6, (u8 *)&D_80182686 - 6 + D_80182686, 0);
                node->field0D++;
                break;
            }
            r = func_800A20F4(6);
            switch (r) {
            case 0:
                func_800A2054(6);
                func_800A2054(1);
                D_801D444C = 1;
                return 2;
            case 1:
                func_800A2054(6);
                node->state = KEEP_SEL_PICK;
                node->field0D = 0;
                continue;
            }
            return 0;
        }
    }
}

/**
 * @brief Per-frame AI move-selection state machine.
 *
 * State 0 resets the placed-count (@c g_sweepProcessed). State 1 alternates between a
 * gate tick (waits while @c hasPendingCardObj reports a pending action; finishes with
 * @c D_801D444C=1, return 2 once @c g_sweepProcessed reaches @c g_sweepTarget) and a scan
 * tick that picks the capturable object (owner != @c node->field0E, @c flags bit 0
 * set) with the highest card level (@c g_tripleTriadCardStats[cardId] level byte),
 * marks it (0x08=2, 0x09=0), and bumps @c g_sweepProcessed.
 *
 * @param node The driver node.
 * @return 0 while blocked, 2 when the move sweep completes.
 */
s32 runAiCaptureSelect(ScriptStateNode *node) {
    s32 bestVal;
    s32 bestIdx;
    s32 i;

    while (1) {
        switch (node->state) {
        case SWEEP_INIT:
            g_sweepProcessed = 0;
            node->state = SWEEP_RUN;
            node->field0D = 0;
            break;
        case SWEEP_RUN:
            if (node->field0D == 0) {
                if (hasPendingCardObj() != 0) {
                    return 0;
                }
                if (g_sweepProcessed == g_sweepTarget) {
                    D_801D444C = 1;
                    return 2;
                }
                node->field0D++;
                break;
            }
            bestVal = 0;
            bestIdx = 0;
            for (i = 0; i < 10; i++) {
                if (g_activeCardObjs[i].fieldA != node->field0E &&
                    (g_activeCardObjs[i].flags & 1) &&
                    bestVal < g_tripleTriadCardStats[g_activeCardObjs[i].cardId].pad05[0]) {
                    bestVal = g_tripleTriadCardStats[g_activeCardObjs[i].cardId].pad05[0];
                    bestIdx = i;
                }
            }
            g_activeCardObjs[bestIdx].field8 = CLAIM_ANIM_FLIP;
            g_activeCardObjs[bestIdx].field9 = 0;
            g_sweepProcessed++;
            node->state = SWEEP_RUN;
            node->field0D = 0;
            break;
        }
    }
}

/**
 * @brief Per-frame state machine that consumes each played hand card from the
 *        working board/hand state, one move per pass.
 *
 * State 0 seeds the per-player working hand copy @c g_handBuildHands from the match
 * hands @c D_801A2C48 (2 players x 5 cards), then advances to state 1.
 *
 * State 1 walks @c g_tripleTriadCardHands one move at a time (index @c node->index,
 * advanced each pass while @c hasPendingCardObj reports no pending action). For the
 * current move it takes the card id and owning seat (bit 0 of @c initFlags) and
 * marks that card consumed: if the card is still in the owner's working hand
 * @c g_handBuildHands[owner] it is stamped @c 0xFF; otherwise the matching board cell in
 * @c g_activeCardObjs owned by the *other* seat is flagged (@c field8 = 2, @c field9 = 0)
 * to trigger its flip.
 *
 * @param node State-machine driver node.
 * @return 0 while @c hasPendingCardObj reports a pending action; 2 once all 10 moves
 *         have been processed (also sets @c D_801D444C = 1).
 */
s32 replayHandMoves(ScriptStateNode *node) {
    s32 i;
    s32 col;
    s32 owner;
    u8 card;

    while (1) {
        switch (node->state) {
        case SWEEP_INIT:
            for (i = 0; i < 2; i++) {
                for (col = 0; col < 5; col++) {
                    g_handBuildHands[i][col] = D_801A2C48[i][col];
                }
            }
            node->index = 0;
            node->state = SWEEP_RUN;
            node->field0D = 0;
            break;
        case SWEEP_RUN:
            if (node->field0D == 0) {
                if (hasPendingCardObj() != 0) {
                    return 0;
                }
                if (node->index >= 10) {
                    D_801D444C = 1;
                    return 2;
                }
                node->field0D++;
            }
            owner = g_tripleTriadCardHands[node->index].initFlags & 1;
            card = g_tripleTriadCardHands[node->index].cardId;
            for (i = 0; i < 5; i++) {
                if (g_handBuildHands[owner][i] == card) {
                    g_handBuildHands[owner][i] = 0xFF;
                    break;
                }
            }
            if (i >= 5) {
                for (i = 0; i < 10; i++) {
                    if (g_activeCardObjs[i].cardId == card &&
                        g_activeCardObjs[i].fieldA != owner) {
                        g_activeCardObjs[i].field8 = CLAIM_ANIM_FLIP;
                        g_activeCardObjs[i].field9 = 0;
                        break;
                    }
                }
            }
            node->state = SWEEP_RUN;
            node->field0D = 0;
            node->index++;
            break;
        }
    }
}

/**
 * @brief Per-frame state machine that sweeps the 10 g_activeCardObjs cells.
 *
 * State 0 resets the scan (index = 0). State 1 walks one cell per tick:
 * if the cell's 0x0A field equals @c node->field0E XOR 1, the cell is marked
 * (0x08 = 2, 0x09 = 0). When all 10 cells are processed it sets @c D_801D444C
 * and returns 2; if @c hasPendingCardObj reports a pending action it returns 0.
 *
 * @param node The driver node.
 * @return 0 if blocked by a pending action, 2 when the sweep completes.
 */
s32 runOpponentSideSweep(ScriptStateNode *node) {
    while (1) {
        switch (node->state) {
        case SWEEP_INIT:
            node->index = 0;
            node->state = SWEEP_RUN;
            node->field0D = 0;
            break;
        case SWEEP_RUN:
            if (node->field0D == 0) {
                if (hasPendingCardObj() != 0) {
                    return 0;
                }
                if (node->index >= 10) {
                    D_801D444C = 1;
                    return 2;
                }
                node->field0D++;
            }
            if (g_activeCardObjs[node->index].fieldA == (node->field0E ^ 1)) {
                g_activeCardObjs[node->index].field8 = CLAIM_ANIM_FLIP;
                g_activeCardObjs[node->index].field9 = 0;
            }
            node->state = SWEEP_RUN;
            node->field0D = 0;
            node->index++;
            break;
        }
    }
}

/**
 * @brief Post-round capture/cleanup sweep over the board.
 *
 * State machine (one cell advanced per pass while @c hasPendingCardObj reports no pending
 * action). State 0 waits for the action gate. State 1 sweeps the opponent row
 * (@c g_claimSeat ^ 1): each card owned by the acting seat is flagged @c field8 = 3.
 * State 2 sweeps the acting seat's row (@c g_claimSeat): each card NOT owned by it is
 * flagged @c field8 = 4. State 3 returns every acting-seat card to its collection
 * (@c markItemPresent) and finishes, setting @c g_sweepDone and returning 2 — unless held
 * by @c D_801D4454 / @c g_padPressed[2], in which case it returns 0.
 *
 * @param node State node (state 0x0C, per-pass guard 0x0D, cell index 0x0F).
 * @return 0 while a pass is pending or gated; 2 once the cleanup completes.
 */
s32 runCaptureCleanupSweep(ScriptStateNode *node) {
    s32 i;
    ActiveObj *c;

    while (1) {
        switch (node->state) {
        case CLEANUP_WAIT:
            if (hasPendingCardObj() != 0) {
                return 0;
            }
            node->state = CLEANUP_SWEEP_OPP;
            node->field0D = 0;
            node->index = 0;
            break;
        case CLEANUP_SWEEP_OPP:
            if (node->field0D == 0) {
                if (node->index < 5) {
                    c = &g_activeCardObjs[(g_claimSeat ^ 1) * 5 + node->index];
                    if (c->fieldA == g_claimSeat) {
                        c->field8 = CLAIM_ANIM_CAPTURE_A;
                        c->field9 = 0;
                    }
                    node->index++;
                    node->field0D++;
                } else {
                    node->state = CLEANUP_SWEEP_OWN;
                    node->field0D = 0;
                    node->index = 0;
                    break;
                }
            }
            if (hasPendingCardObj() != 0) {
                return 0;
            }
            node->field0D = 0;
            break;
        case CLEANUP_SWEEP_OWN:
            if (node->field0D == 0) {
                if (node->index < 5) {
                    c = &g_activeCardObjs[g_claimSeat * 5 + node->index];
                    if (c->fieldA != g_claimSeat) {
                        c->field8 = CLAIM_ANIM_CAPTURE_B;
                        c->field9 = 0;
                    }
                    node->index++;
                    node->field0D++;
                } else {
                    node->state = CLEANUP_COLLECT;
                    node->field0D = 0;
                    node->index = 0;
                    break;
                }
            }
            if (hasPendingCardObj() != 0) {
                return 0;
            }
            node->field0D = 0;
            break;
        case CLEANUP_COLLECT:
            if (node->field0D == 0) {
                for (i = 0; i < 10; i++) {
                    if (g_activeCardObjs[i].fieldA == g_claimSeat) {
                        markItemPresent(g_activeCardObjs[i].cardId);
                    }
                }
                node->field0D++;
            } else {
                if (D_801D4454 == 0) {
                    if (g_padPressed[2] == 0) {
                        return 0;
                    }
                }
                g_sweepDone = 1;
                return 2;
            }
            break;
        }
    }
}
