#include "common.h"
#include "gamestate.h"
#include "item.h"
#include "numstr.h"
#include "sound.h"
#include "snd_init.h"
#include "thread.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgpu.h"
#include "drawbar.h"
#include "battle_anim.h"
#include "btl_anim.h"
#include "tripletriad/tt_main.h"
#include "tripletriad/tt_match.h"
#include "tripletriad/tt_game.h"
#include "tripletriad/tt_script.h"
#include "tripletriad/tt_claim.h"
#include "tripletriad/tt_ui.h"

/**
 * @brief Reset and reconfigure the seven SFX channels.
 *
 * Resets all sound effects, runs a (60, 32) init via @c func_800A4504, then for
 * each of the seven channels applies the per-channel settings from the
 * @c g_sfxConfigs config table: reverb mode = channel index, field 0x2F and pitch
 * from the table entry, and zeroed entry params.
 */
void func_800A1BE0(void)
{
    s32 i;

    resetAllSfx();
    func_800A4504(0x3C, 0x20);
    for (i = 0; i < 7; i++) {
        setSfxReverbMode(i, i);
        setSfxField2F(i, g_sfxConfigs[i].field2F);
        setSfxPitch(i, g_sfxConfigs[i].pitch);
        setSfxEntryParams(i, 0, 0);
    }
}

/**
 * @brief Per-frame Triple Triad hand-build/display update.
 *
 * While hand-building input is active (@ref TT_INPUT_HAND_BUILD), drives the
 * cursor state machine with the current input snapshots and records the
 * resulting card-display slot; otherwise idles the state machine and clears the
 * slot. Then refreshes the display, renders the battle ordering table, and
 * counts down each SFX entry's fade timer — firing a fast or slow fade-out (per
 * the entry's flag bit 0) on the frame a timer reaches zero.
 */
void func_800A1C6C(void)
{
    s32 i;

    if (g_tripleTriadInputFlags & TT_INPUT_HAND_BUILD) {
        g_cardDisplaySlot = func_800A390C(g_padRepeat[2], g_padPressed[2]);
    } else {
        func_800A390C(0, 0);
        g_cardDisplaySlot = -1;
    }

    renderAndUpdateDisplay(1);
    renderBattleDisplayList((s32 *)g_otBase);
    func_800A443C((s32)&g_otBase[1]);

    for (i = 0; i < 7; i++) {
        if (g_sfxConfigs[i].fadeTimer != 0) {
            g_sfxConfigs[i].fadeTimer--;
            if (g_sfxConfigs[i].fadeTimer == 0) {
                if (g_sfxConfigs[i].flags & 1) {
                    fadeOutSfxFast(i);
                } else {
                    fadeOutSfxSlow(i);
                }
            }
        }
    }
}

/**
 * @brief Lay out a Triple Triad message-banner box and trigger its SFX/animation.
 *
 * Measures @p str (and, for @p id 5, the appended "Play / Quit" suffix) to size the
 * box, copies the box rect from @c g_sfxConfigs[id], applies defaults ("size to text"
 * when w/h are 0), then either centers it (flag bit 2) or pulls it in from the
 * right/bottom edge for negative origins.  Registers the rect (func_8002E064),
 * dispatches the banner's audio by @p id (5 = multi-line, 6 = fixed, otherwise the
 * generic path), optionally offsets the SFX entry (flag bit 1), starts it
 * normal/slow (flag bit 0), and records @p param as the entry's fade timer.
 *
 * @param id    Message/SFX slot index into @c g_sfxConfigs.
 * @param str   FF8-encoded message string.
 * @param param Fade-timer / display-duration value stored into the entry.
 */
void func_800A1D68(s32 id, u8 *str, s32 param) {
    GlyphSize dim;   /* text block size from getGlyphWidthA */
    GlyphSize sfx;   /* "Play / Quit" suffix size (id 5 only) */
    RECT rect;

    dim.raw[0] = getGlyphWidthA(str);

    if (id == 5) {
        s16 m;
        sfx.raw[0] = getGlyphWidthA((u8 *)&D_801826E2 - 0x62 + D_801826E2);
        m = (u16)sfx.wh[0] + 0x20;
        sfx.wh[0] = m;
        if (dim.wh[0] < m) {
            dim.wh[0] = m;
        }
    }

    rect = g_sfxConfigs[id].rect;
    if (rect.w == 0) {
        rect.w = (u16)dim.wh[0] + 0x10;
    }
    if (rect.h == 0) {
        rect.h = (u16)dim.wh[1] + 0x10;
    }
    if (g_sfxConfigs[id].flags & 4) {
        rect.x = (u16)rect.x - rect.w / 2;
        rect.y = (u16)rect.y - rect.h / 2;
    } else {
        if (rect.x < 0) {
            rect.x = (u16)rect.x + 0x180 - rect.w;
        }
        if (rect.y < 0) {
            rect.y = (u16)rect.y + 0xE0 - rect.h;
        }
    }
    func_8002E064(id, &rect);

    if (id == 5) {
        goto sfx5;
    }
    if (id != 6) {
        goto sfxDefault;
    }
    func_8002D784(6, str, 1, 2, 1, 2);
    setSfxGlobalFlag(6);
    goto sfxDone;
sfx5:
    {
        s32 lines = dim.wh[1] / 16;
        func_8002D784(5, str, lines - 1, lines, lines - 1, lines);
    }
    setSfxGlobalFlag(5);
    goto sfxDone;
sfxDefault:
    initSfxPlayback(id, str);
sfxDone:;

    if (g_sfxConfigs[id].flags & 2) {
        s32 px = rect.w - 0x10;
        s32 py = rect.h - 0x10;
        setSfxEntryParams(id, (px - dim.wh[0]) / 2, (py - dim.wh[1]) / 2);
    }
    if (g_sfxConfigs[id].flags & 1) {
        startSfxNormal(id);
    } else {
        startSfxSlow(id);
    }

    g_sfxConfigs[id].fadeTimer = param;
}

/**
 * @brief Start or stop an SFX entry based on its type flag.
 *
 * Looks up the entry at g_sfxConfigs[a0 * 12], checks bit 0 of byte 0.
 * If set, calls fadeOutSfxFast (stop). Otherwise calls fadeOutSfxSlow (start).
 *
 * @param a0 Object index.
 */
void func_800A2054(s32 a0) {
    u8 *base = (u8 *)g_sfxConfigs;
    u8 *entry;

    entry = base + a0 * 12;
    if (entry[0] & 1) {
        fadeOutSfxFast();
    } else {
        fadeOutSfxSlow();
    }
}

/**
 * @brief Reset all 7 SFX entries and finalize.
 *
 * Calls fadeOutSfxFast for each of the 7 objects (indices 0-6),
 * then calls func_800A44BC to reset the cursor timer step.
 */
void func_800A20B0(void) {
    s32 i = 0;
    do {
        fadeOutSfxFast(i);
        i++;
    } while (i < 7);
    func_800A44BC();
}

/**
 * @brief Poll a player-input gate; thin wrapper forwarding @p gate to func_8002CE84.
 * @param gate Gate / channel id to poll.
 * @return Gate result: <0 while still waiting, otherwise the player's selection.
 */
s32 func_800A20F4(s32 gate) {
    return func_8002CE84(gate);
}

/**
 * @brief Show the card-detail popup for a card.
 *
 * When the player owns at least one copy (count > 0) it shows the card's name
 * directly. Otherwise it builds the detail message in @c g_cardDetailMsg — a 6
 * header byte, a type byte (@c 0x22 when the owned count is exactly 0, @c 0x25
 * when it is negative), the card name, then @c g_cardDetailSuffix — and shows that.
 *
 * @param cardId Card index (into @c g_tripleTriadCardCounts and the name table).
 */
void showCardDetail(s32 cardId) {
    s32 count = g_tripleTriadCardCounts[cardId];

    if (count > 0) {
        func_800A1D68(0, func_80023A54(cardId), 1);
    } else if (count == 0) {
        g_cardDetailMsg[0] = 6;
        g_cardDetailMsg[1] = 0x22;
        strcpy((char *)&g_cardDetailMsg[2], func_80023A54(cardId));
        func_80047C74(g_cardDetailMsg, g_cardDetailSuffix);
        func_800A1D68(0, g_cardDetailMsg, 1);
    } else {
        g_cardDetailMsg[0] = 6;
        g_cardDetailMsg[1] = 0x25;
        strcpy((char *)&g_cardDetailMsg[2], func_80023A54(cardId));
        func_80047C74(g_cardDetailMsg, g_cardDetailSuffix);
        func_800A1D68(0, g_cardDetailMsg, 1);
    }
}

/**
 * @brief Clear all 7 SFX entries by calling setSfxEntryParams with zero params.
 *
 * Iterates indices 0-6, calling setSfxEntryParams(i, 0, 0) for each.
 */
void clearAllSfx(void) {
    s32 i = 0;
    do {
        setSfxEntryParams(i, 0, 0);
        i++;
    } while (i < 7);
}

/**
 * @brief Clear D_801D4560 to zero.
 */
void func_800A2208(void) {
    D_801D4560 = 0;
}

/**
 * @brief Flush the queued Triple Triad sound effects to the SPU.
 *
 * Walks the SFX request queue (@c D_801D4500, @c D_801D4560 entries) in order and
 * plays each active effect via @c sndPlaySfx, skipping any whose voice-channel
 * mask (@c param) overlaps a request already played this pass — so colliding
 * requests collapse to the first. The accumulated mask grows as it goes. Empties
 * the queue when done.
 */
void flushTriadSfxQueue(void) {
    s32 accumMask = 0;
    s32 i;
    SfxQueueEntry *e = D_801D4500;

    for (i = 0; i < D_801D4560; i++) {
        u32 mask = e->param;
        if ((accumMask & mask) == 0) {
            if (e->active == 1) {
                sndPlaySfx(e->sfxId, mask, e->volume, e->pan);
            }
            accumMask |= e->param;
        }
        e++;
    }
    D_801D4560 = 0;
}

/**
 * @brief Push a sound effect onto the Triple Triad SFX queue (if not full).
 *
 * The queue holds up to 7 pending effects, drained each frame by
 * @c flushTriadSfxQueue.
 *
 * @param sfxId  Sound-effect id.
 * @param volume Volume (callers pass 0x80).
 * @param pan    Stereo pan (callers pass 0x7F).
 * @param param  Per-effect parameter word (also the voice-channel dedup mask).
 */
void queueTriadSfx(s32 sfxId, s32 volume, s32 pan, s32 param) {
    s32 count = D_801D4560;
    if (count < 7) {
        SfxQueueEntry *entry;
        D_801D4560 = count + 1;
        entry = &D_801D4500[count];
        entry->active = 1;
        entry->sfxId = sfxId;
        entry->volume = volume;
        entry->pan = pan;
        entry->param = param;
    }
}

/** @brief Queue a Triple Triad sound effect at center pan / full volume. */
void playTriadSfx(s32 sfxId) {
    queueTriadSfx(sfxId, 0x80, 0x7F, 0);
}

/** @brief Queue a Triple Triad sound effect with a per-effect @p param. */
void playTriadSfxParam(s32 sfxId, s32 param) {
    queueTriadSfx(sfxId, 0x80, 0x7F, param);
}

/**
 * @brief Sound-bank swap task: wait for the engine, then upload and play a bank.
 *
 * Two-state task callback driven off @c node->state:
 *  - 0: wait until the sound engine is idle (@c sndGetEngineState), then issue
 *       @c sndCmd11(0) and advance to state 1.
 *  - 1: copy the Triple Triad sound region @c [g_tripleTriadSoundBank, g_tripleTriadActiveList)
 *       into the inactive bank buffer (@c D_8005F388 / @c D_80063388, chosen by
 *       @c D_80082C11), flip the bank selector @c g_battleConfig[9], then play the
 *       uploaded bank via @c sndCmd10 / @c sndCmdC0.
 *
 * @param node Task node.
 * @return 0 while still running, 2 once the bank has been uploaded and played.
 */
s32 func_800A238C(SndTaskNode *node) {
    void *buf;
    s16  sample;

    switch (node->state) {
    case 0:
        if (sndGetEngineState() == 0) {
            sndCmd11(0);
            node->state = 1;
            node->field0E = 0;
        }
        return 0;
    case 1:
        if (D_80082C11 == 0) {
            buf = D_8005F388;
        } else {
            buf = D_80063388;
        }
        g_battleConfig[9] ^= 1;
        func_800485C4(buf, g_tripleTriadSoundBank, (s32)&g_tripleTriadActiveList - (s32)g_tripleTriadSoundBank);
        sample = sndCmd10((s32)buf);
        D_8005F11C = sample;
        sndCmdC0(sample, 0x7F);
        return 2;
    }
    return 0;
}

/**
 * @brief Spawn the sound-bank swap task and seed its state.
 *
 * Loads the Triple Triad audio data from @c g_tripleTriadAkao via @c sndProcessAudio,
 * then allocates a @c g_taskList node driven by @c func_800A238C with its state
 * and field0E cleared.
 */
void func_800A247C(void) {
    SndTaskNode *node;
    sndProcessAudio((s32)g_tripleTriadAkao, 0);
    node = (SndTaskNode *)spawnTaskNode((ObjNodeFn)func_800A238C);
    node->state = 0;
    node->field0E = 0;
}

/**
 * @brief Build the active Triple Triad rules description into @p dst.
 *
 * Reads @c g_tripleTriadRules and appends, in order, the label for each enabled
 * rule — "Rules", ": Open", ": Sudden Death", ": Random", the "Same"/"Plus"/
 * "Same Wall" clause, ": Elemental" — then ": Trade Rule" and the active
 * trade-rule name (Null/One/Diff/Direct/All, selected by @c D_801A2C44).
 *
 * The labels live in the shared rule-string block at 0x80182680 (a u16 offset
 * table followed by the FF8-encoded strings).  The base is recovered from the
 * title slot (@c &D_8018269E - 0x1E); each label is reached as @c base + offset.
 * The later labels re-derive the base from their own slots — the same access
 * idiom tt_script uses for the card-claim banners.
 *
 * @param dst Destination buffer for the assembled, FF8-encoded string.
 */
void func_800A24B4(u8 *dst) {
    /* Reading the title offset up front forces the %hi(D_8018269E) load, so the base
     * below is materialised from the title slot rather than folded into a slot-relative
     * load (tbl->title) — which is what the original codegen does. */
    u16 title = D_8018269E;
    RuleStrTable *tbl = (RuleStrTable *)((u8 *)&D_8018269E - 0x1E);
    s32 flag;

    dst[0] = 0;
    func_80047C74(dst, (u8 *)tbl + D_8018269E);                  /* "Rules" */

    if (g_tripleTriadRules & TT_RULE_OPEN) {
        func_80047C74(dst, (u8 *)tbl + tbl->openStr);            /* ": Open" */
    }
    if (g_tripleTriadRules & TT_RULE_SUDDEN_DEATH) {
        func_80047C74(dst, (u8 *)tbl + tbl->suddenDeathStr);     /* ": Sudden Death" */
    }
    if (g_tripleTriadRules & TT_RULE_RANDOM) {
        func_80047C74(dst, (u8 *)tbl + tbl->randomStr);          /* ": Random" */
    }
    if (g_tripleTriadRules & (TT_RULE_SAME | TT_RULE_PLUS)) {
        flag = 0;
        func_80047C74(dst, (u8 *)tbl + tbl->sameOrPlus0);        /* clause lead-in */
        func_80047C74(dst, (u8 *)tbl + tbl->sameOrPlus1);        /* ": " */
        if (g_tripleTriadRules & TT_RULE_SAME) {
            func_80047C74(dst, (u8 *)tbl + tbl->sameStr);        /* "Same" */
            flag = 1;
        }
        if (g_tripleTriadRules & TT_RULE_PLUS) {
            if (flag) {
                func_80047C74(dst, (u8 *)tbl + tbl->plusConj);   /* "," */
            }
            func_80047C74(dst, (u8 *)tbl + tbl->plusStr);        /* "Plus" */
            flag = 1;
        }
        if ((g_tripleTriadRules & (TT_RULE_SAME | TT_RULE_SAME_WALL)) == (TT_RULE_SAME | TT_RULE_SAME_WALL)) {
            if (flag) {
                func_80047C74(dst, (u8 *)&D_801826A6 - 0x26 + D_801826A6);   /* "," */
            }
            func_80047C74(dst, (u8 *)&D_801826C2 - 0x42 + D_801826C2);       /* "Same Wall" */
        }
    }
    if (g_tripleTriadRules & TT_RULE_ELEMENTAL) {
        func_80047C74(dst, (u8 *)&D_801826C6 - 0x46 + D_801826C6);           /* ": Elemental" */
    }
    func_80047C74(dst, (u8 *)&D_801826CA - 0x4A + D_801826CA);               /* ": Trade Rule" */
    /* active trade-rule name (Null/One/Diff/Direct/All), indexed by D_801A2C44 */
    func_80047C74(dst, (u8 *)&D_801826CA - 0x4A + *(u16 *)((u8 *)&D_801826CA + D_801A2C44 * 4 + 4));
}

/**
 * @brief Open the Triple Triad in-game menu and freeze card input.
 *
 * Sets up the menu object @c D_801D4568, spawns its handler, and sets
 * @c TT_INPUT_DISABLED so the card-select cursors pause while the menu is up.
 */
void openTriadMenu(void) {
    u8 *base = D_801D4568;
    func_800A24B4(base);
    func_800A1D68(4, base, 0);
    g_tripleTriadInputFlags |= TT_INPUT_DISABLED;
}

/**
 * @brief Close the in-match menu overlay and re-enable card input.
 *
 * Tears down the menu object @c D_801D4568 and clears @c TT_INPUT_DISABLED so
 * card input resumes. Counterpart to the menu-open handler @c openTriadMenu.
 */
void closeMenu(void) {
    func_800A2054(4);
    g_tripleTriadInputFlags &= ~TT_INPUT_DISABLED;
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
s32 func_800A274C(void) {
    s32 idx = g_drawBufferIndex ^ 1;
    queueLoadImage(&g_drawEnvs[idx].clip, &g_tripleTriadBgTim0[0x14]);
    return 0;
}

/** @brief Phase of the rules / "Play / Quit" prompt state machine (@c PromptScreenNode.state). */
enum PlayQuitPhase {
    PLAYQUIT_SHOW   = 0,  /**< Fade to black, then build + show the rules / "Play / Quit" prompt. */
    PLAYQUIT_POLL   = 1,  /**< Poll the player's Play / Quit choice. */
    PLAYQUIT_FINISH = 2   /**< Fade to white, then hand off to TT_STATE_EXIT. */
};

/**
 * @brief Per-frame handler for the post-match rules / "Play / Quit" prompt screen.
 *
 * Three-phase state machine (@ref PlayQuitPhase) clocked once per frame off @c node->state:
 *  - @ref PLAYQUIT_SHOW: on entry start a fade-to-black; after @c TT_HOLD_FRAMES_FADE frames
 *    build the rules description plus its "Play / Quit" suffix into @c D_801D4568 (func_800A24B4
 *    then func_80047C74), show it (func_800A1D68), and advance to @ref PLAYQUIT_POLL.
 *  - @ref PLAYQUIT_POLL: bump the Triple Triad RNG-seed field, then poll the player-input gate
 *    (func_800A20F4). A negative result keeps waiting; otherwise acknowledge it (func_800A2054)
 *    and act on the choice: "play again" (0) re-enters @c TT_STATE_SCRIPT, "quit" (1) advances
 *    to @ref PLAYQUIT_FINISH. Any other non-negative result is ignored.
 *  - @ref PLAYQUIT_FINISH: on entry start a fade-to-white; after @c TT_HOLD_FRAMES_FADE frames
 *    record the quit result (@c D_80082C9C = @c TT_RESULT_QUIT) and hand off to @c TT_STATE_EXIT.
 *
 * @param node Render-list node carrying the phase @c state and frame @c counter.
 * @return Always 0.
 */
s32 updatePlayQuitPrompt(PromptScreenNode *node) {
    TripleTriadData *data = getTripleTriadData();
    s32 r;
    s32 state;

    while (1) {
        state = node->state;
        switch (state) {
        case PLAYQUIT_SHOW:
            if (node->counter == 0) {
                startFadeToBlack(TT_HOLD_FRAMES_FADE);
            }
            node->counter += 1;
            if ((u8)node->counter >= TT_HOLD_FRAMES_FADE) {
                func_800A24B4(D_801D4568);
                /* Suffix string pointer = rule-string block base + offset; the block base is
                   recovered from the carved offset symbol (&D_801826E2 - 0x62 == 0x80182680). */
                func_80047C74(D_801D4568, (u8 *)&D_801826E2 - 0x62 + D_801826E2);
                func_800A1D68(5, D_801D4568, 0);
                node->state = PLAYQUIT_POLL;
                node->counter = 0;
                continue;
            }
            return 0;
        case PLAYQUIT_POLL:
            data->rngState += 1;
            r = func_800A20F4(5);
            if (r >= 0) {
                func_800A2054(5);
                /* Ignore any choice that is neither "play again" (0) nor "quit"; the quit value
                   equals this phase's id, so it is tested against @c state (== PLAYQUIT_POLL). */
                if (r != 0 && r != state) {
                    return 0;
                }
                if (r == 0) {
                    g_tripleTriadState = TT_STATE_SCRIPT;
                } else {
                    node->state = PLAYQUIT_FINISH;
                    node->counter = 0;
                }
            }
            return 0;
        case PLAYQUIT_FINISH:
            if (node->counter == 0) {
                startFadeToWhite(TT_HOLD_FRAMES_FADE);
            }
            node->counter += 1;
            if ((u8)node->counter >= TT_HOLD_FRAMES_FADE) {
                D_80082C9C = TT_RESULT_QUIT;
                g_tripleTriadState = TT_STATE_EXIT;
            }
            return 0;
        }
    }
}

/**
 * @brief Initialize the D_801D4968 linked list with two render callbacks.
 *
 * Sets up D_801D4968 as a linked list (node size 0x10, capacity 4),
 * then appends updatePlayQuitPrompt and func_800A274C as callbacks.
 * Clears the prompt-screen node's state and counter on the first node.
 *
 * @return Pointer to D_801D4968 list header.
 */
u8 *initTripleTriadRenderList(void) {
    ObjList *list = D_801D4968;
    PromptScreenNode *node;
    initObjList(list, D_801D4978, sizeof(PromptScreenNode), 4);
    node = (PromptScreenNode *)allocObjNode(list, (ObjNodeFn)updatePlayQuitPrompt);
    node->state = PLAYQUIT_SHOW;
    node->counter = 0;
    allocObjNode(list, (ObjNodeFn)func_800A274C);
    return list;
}

/**
 * @brief Per-(slot, side) input-edge debounce with keyboard-style auto-repeat,
 *        for one of the four edges of a Triple Triad battle-anim entity.
 *
 * Maintains, per card slot @p entry and side @p side (0..3), a small auto-repeat
 * state machine over a bitmask of edge events:
 *  - Reads last frame's masked bits from @ref D_801D4AF8 [entry][side] and stores
 *    @p newVal there.
 *  - Masks both new and previous bitmasks by the side's relevance mask
 *    (`elem->unk10[side]`): @c result = new active bits, @c prevMasked = old.
 *  - @c base->defaultColor packs the timing: low byte = initial/restart delay,
 *    high byte = repeat interval.
 *  - If the masked new and old bits overlap (a sustained event), ticks the
 *    countdown in @ref D_801D4B08 [entry][side] (reloading the restart delay when
 *    the bits change); on underflow it fires and re-arms the repeat interval,
 *    otherwise it suppresses (returns 0). If they do not overlap, it resets the
 *    timer to the restart delay and fires.
 *
 * Drives keyboard-style auto-repeat for whatever per-side cue the bits represent.
 * Called four times (once per side) by @ref func_800A2A8C, which ORs the results.
 *
 * @param base   Battle-anim state; base->defaultColor packs the two delays.
 * @param elem   Battle-anim entity; elem->unk10[side] is the per-side mask.
 * @param newVal Raw new edge bitmask for this frame.
 * @param side   Card side index, 0..3.
 * @param entry  Card slot index.
 * @return The masked event bits that should fire this frame, or 0 while suppressed.
 *
 * @note Purpose inferred. Decomp scratch: https://decomp.me/scratch/7L33D
 */
s32 func_800A29D4(BattleAnimState *base, BattleAnimEntity *elem, u16 newVal, s32 side, s32 entry)
{
    s32 repeatTimer;
    s32 restartDelay;
    s32 repeatInterval;
    u16 mask;
    u16 prevMasked;

    prevMasked = D_801D4AF8[entry][side];
    D_801D4AF8[entry][side] = newVal;
    restartDelay = *(u16 *)&base->defaultColor;
    repeatTimer = D_801D4B08[entry][side];
    mask = elem->unk10[side];

    repeatInterval = restartDelay >> 8;
    restartDelay &= 0xFF;
    newVal &= mask;
    prevMasked &= mask;
    if (newVal & prevMasked) {
        if (newVal != prevMasked) {
            repeatTimer = restartDelay;
        }
        repeatTimer--;
        if (repeatTimer < 0) {
            repeatTimer = repeatInterval;
        } else {
            newVal = 0;
        }
    } else {
        repeatTimer = restartDelay;
    }
    D_801D4B08[entry][side] = repeatTimer;
    return newVal;
}

/**
 * @brief Evaluate all four edges of battle-anim entity @p entryIndex against its neighbour.
 *
 * Follows the entity's @c linkedIdx to its linked neighbour, then evaluates each
 * of the four edges (0..3) via @c func_800A29D4, OR-ing the per-edge results into
 * a single 16-bit mask. The Triple Triad board reuses the battle-animation
 * entities (@c g_battleAnims) to drive its card animations.
 *
 * @param entryIndex Index of the entity in @c g_battleAnims to evaluate.
 * @param arg1       Per-edge value forwarded to @c func_800A29D4.
 * @return Combined 16-bit result mask from the four edge evaluations. Typed @c s32
 *         (not @c u16) because @c readPads re-masks the value, which requires the
 *         caller to not assume it is already 16-bit-clean.
 */
s32 func_800A2A8C(s32 entryIndex, u16 arg1)
{
    BattleAnimEntity *link = &g_battleAnims.entities[g_battleAnims.entities[entryIndex].linkedIdx];
    u16 result = 0;

    result |= func_800A29D4(&g_battleAnims, link, arg1, 0, entryIndex);
    result |= func_800A29D4(&g_battleAnims, link, arg1, 1, entryIndex);
    result |= func_800A29D4(&g_battleAnims, link, arg1, 2, entryIndex);
    result |= func_800A29D4(&g_battleAnims, link, arg1, 3, entryIndex);

    return result & 0xFFFF;
}

/** @brief Held-button mask for pad @p a0 (buttons currently down). */
s32 getPadHeld(s32 a0) {
    return D_801D4B20[a0];
}

/** @brief Newly-pressed (rising-edge) button mask for pad @p a0. */
s32 getPadPressed(s32 a0) {
    return D_801D4B30[a0];
}

/** @brief Auto-repeat button mask for pad @p a0. */
s32 getPadRepeat(s32 a0) {
    return D_801D4B28[a0];
}

/**
 * @brief Sample both controllers and update the per-pad input state.
 *
 * For each of the two controllers, reads the raw button mask, and when no d-pad
 * direction is held (no 0xF000 bits) and the analog stick is valid, folds the
 * analog stick — recentred around 0x80 — into directional bits. The result is
 * recorded per controller as the current held mask (@c D_801D4B20[n]), the
 * newly-pressed mask (@c D_801D4B30[n], bits set this frame but not last), and
 * the auto-repeat mask (@c D_801D4B28[n], via @c func_800A2A8C).
 *
 * @note The player-2 writes go through the split element symbols
 *       (@c D_801D4B24 / @c D_801D4B2C / @c D_801D4B34, each the [1] element of
 *       its array) so the compiler re-emits a fresh address rather than reusing
 *       player 1's base pointer.
 */
void readPads(void)
{
    s32 padRaw;
    s32 oldPad;
    s32 held;
    s32 repeat;

    func_800275D4();

    padRaw = func_80030F10(getAnimFrameParam(0, 0));
    oldPad = D_801D4B20[0];
    held = func_80027DB4(0, 2, 0);
    if (!(padRaw & 0xF000) && held >= 0) {
        padRaw |= func_80027CF8(0, held - 0x80, func_80027DB4(0, 3, 0) - 0x80);
    }
    D_801D4B20[0] = padRaw;
    D_801D4B30[0] = (padRaw ^ oldPad) & padRaw;
    repeat = func_800A2A8C(0, padRaw & 0xFFFF) & 0xFFFF;
    D_801D4B28[0] = repeat;

    padRaw = func_80030F10(getAnimFrameParam(1, 0));
    oldPad = D_801D4B20[1];
    held = func_80027DB4(1, 2, 0);
    if (!(padRaw & 0xF000) && held >= 0) {
        padRaw |= func_80027CF8(0, held - 0x80, func_80027DB4(1, 3, 0) - 0x80);
    }
    D_801D4B24 = padRaw;
    D_801D4B34 = (padRaw ^ oldPad) & padRaw;
    repeat = func_800A2A8C(1, padRaw & 0xFFFF) & 0xFFFF;
    D_801D4B2C = repeat;
}

/**
 * @brief Reset the Triple Triad per-edge animation state for both entities.
 *
 * Clears the per-(entity, side) bookkeeping tables — previous edge flags
 * (@c D_801D4AF8), edge countdown timers (@c D_801D4B08), and the three
 * @c D_801D4B20 / @c D_801D4B28 / @c D_801D4B30 word tables — for both
 * animation entities, then seeds @c func_800281A4 with the fixed per-side
 * parameters (one set per side 0..3) for each entity.
 */
void func_800A2D34(void)
{
    s32 i;

    for (i = 0; i < 2; i++) {
        D_801D4AF8[i][0] = 0;
        D_801D4AF8[i][1] = 0;
        D_801D4AF8[i][2] = 0;
        D_801D4AF8[i][3] = 0;
        D_801D4B08[i][0] = 0;
        D_801D4B08[i][1] = 0;
        D_801D4B08[i][2] = 0;
        D_801D4B08[i][3] = 0;
        D_801D4B20[i] = 0;
        D_801D4B28[i] = 0;
        D_801D4B30[i] = 0;
    }

    func_800281A4(0, 0, 0xFFF);
    func_800281A4(0, 1, 0x5000);
    func_800281A4(0, 2, 0xA000);
    func_800281A4(0, 3, 0x900);
    func_800281A4(1, 0, 0xFFF);
    func_800281A4(1, 1, 0x5000);
    func_800281A4(1, 2, 0xA000);
    func_800281A4(1, 3, 0x900);
}

/**
 * @brief Rebuild the cursor grid's valid-cell list and clamp the cursor into range.
 *
 * Scans candidate indices 0..0x6D, and for each one accepted by @c func_80023B14
 * (returns > 0) appends the index to @c D_801D4A88, recording the running total in
 * @c D_801D4AF6. The grid is laid out in rows of 11 cells; if the current cursor
 * position now sits past the last populated row, it is clamped back onto the last
 * row at the same column (@ref CursorState::cursorPos and @ref CursorState::row).
 */
void func_800A2E44(void)
{
    CursorState *cs = &D_801D49C8;
    u8 *dst = D_801D4A88;
    s32 i;
    s32 count;
    s32 gridSize;
    s16 idx;

    count = 0;
    for (i = 0; i < 0x6E; i++) {
        if (func_80023B14(i) > 0) {
            *dst = i;
            dst++;
            count++;
        }
    }
    D_801D4AF6 = count;

    gridSize = (D_801D4AF6 + 0xA) / 11 * 11;
    idx = (s16)cs->cursorPos;
    if (idx >= gridSize) {
        s32 col = idx % 11;
        s32 row = gridSize / 11 - 1;
        cs->cursorPos = row * 11 + col;
        cs->row = row;
    }
}

/**
 * @brief Build the geometric falloff table @c D_801D49F8 (each step * 9/10).
 *
 * Fills the s16 entries @c D_801D49F8[0..64], writing index 64 first then
 * 63 down to 0. Index 64 starts at @c 0x1000; the running value is stored and
 * then scaled by @c 9/10 (a ~0.9 attenuation) for the next lower index — so the
 * table decays from @c 0x1000 at the top toward index 0.
 */
void func_800A2F78(void) {
    s16 *p;
    s32 v;
    s32 i;

    p = D_801D49F8;
    v = 0x1000;
    p += 64;
    *p = v;
    for (i = 0; i < 64; i++) {
        p--;
        *p = v;
        v = v * 9 / 10;
    }
}

/**
 * @brief Draw the blinking corner markers around the Triple Triad cursor's view rect.
 *
 * Emits up to two glyphs through @c func_800300F8, selected by @p corners:
 * bit 0 draws the left marker (glyph 0x5C) just inside the view's top-left, and
 * bit 1 draws the right marker (glyph 0x5D) just inside the top-right. Both sit
 * near the bottom of the view (@c view.y + view.h - 10). The markers blink in
 * step with @c CursorState::frameCounter: bit 3 of the counter selects a blink
 * parameter of 0 or 0x140, toggling every 8 frames. The running @p prim cursor
 * is threaded through each call and returned.
 *
 * @param renderCtx Render context forwarded to @c func_800300F8.
 * @param prim      Primitive/cursor threaded through and advanced by each glyph.
 * @param color     Color parameter forwarded to @c func_800300F8.
 * @param corners   Bitmask: bit 0 = left marker, bit 1 = right marker.
 * @return The advanced @p prim cursor.
 */
void *func_800A2FCC(void *renderCtx, void *prim, s32 color, s32 corners)
{
    s32 yc;
    CursorState *cs = &D_801D49C8;
    s32 blink;

    blink = 0x140;
    if ((cs->frameCounter >> 3) & 1) {
        blink = 0;
    }

    if (corners & 1) {
        yc = cs->view.y + cs->view.h - 10;
        prim = func_800300F8(renderCtx, prim, 0x5C, cs->view.x + 2, yc, color, blink);
    }
    if (corners & 2) {
        yc = cs->view.y + cs->view.h - 10;
        prim = func_800300F8(renderCtx, prim, 0x5D, (cs->view.x + cs->view.w) - 9, yc, color, blink);
    }
    return prim;
}

/**
 * @brief Render a number (with a fixed prefix glyph) into the ordering table.
 *
 * Formats @p value @c +1 to a decimal glyph string and blanks its leading zero,
 * then emits a fixed prefix glyph (@c 0x32) followed by the digit(s) via
 * @c func_8002FF34, advancing the glyph position between each. When @p twoDigit
 * is set both the tens and units digits are drawn (advancing 9 then 6);
 * otherwise only the units digit is drawn after the prefix.
 *
 * @param otBase   Ordering-table base for the glyph primitives.
 * @param pkt      Current GPU packet cursor.
 * @param pos      Starting glyph position (passed to @c func_8002FF34; advanced per glyph).
 * @param w        Glyph width forwarded to @c func_8002FF34.
 * @param col      Glyph color/palette forwarded to @c func_8002FF34.
 * @param value    Number to display (rendered as @c value @c + @c 1).
 * @param twoDigit Non-zero to draw the tens digit as well as the units digit.
 * @return The advanced packet cursor.
 */
void *func_800A30C8(void *otBase, void *pkt, s32 pos, s32 w, s32 col, s32 value, s32 twoDigit) {
    u8 buf[16];

    intToDecStringShort(value + 1, buf, 0x28);
    replaceLeadingZeros(&buf[3], 1, 0x28, 7);

    pkt = func_8002FF34(otBase, pkt, 0x32, pos, w, col);
    pos += 9;
    if (twoDigit != 0) {
        pkt = func_8002FF34(otBase, pkt, buf[3], pos, w, col);
        pos += 6;
    }
    pkt = func_8002FF34(otBase, pkt, buf[4], pos, w, col);
    return pkt;
}

/**
 * @brief Two-digit form of @c func_800A30C8 (always draws both digits).
 *
 * Forwards its arguments to @c func_800A30C8 with the @c twoDigit flag set.
 */
void *func_800A31B8(void *otBase, void *pkt, s32 pos, s32 w, s32 col, s32 value) {
    return func_800A30C8(otBase, pkt, pos, w, col, value, 1);
}

/**
 * @brief Build a draw-area primitive from the active draw environment and link it.
 *
 * Packs the @c SetDrawArea GP0 command for @c g_activeDrawEnv's clip rectangle
 * into the @c DR_AREA at @p prim, links @p prim into the ordering-table slot
 * @p ot via @c addPrimFast (hand-picked temp @c $s2), and returns the next
 * packet slot.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @return Cursor for the next primitive (@c prim @c + @c 1).
 *
 * @note @c ++prim (advance-then-return), rather than @c prim @c + @c 1, is what
 *       matches: reassigning @c prim splits its live range so it drops its
 *       cross-call references and @c ot wins the lower saved register.
 */
DR_AREA *func_800A31EC(P_TAG *ot, DR_AREA *prim) {
    SetDrawArea(prim, &g_activeDrawEnv->clip);
    addPrimFast(ot, prim, s2);
    return ++prim;
}

/**
 * @brief Build a clamped, draw-env-relative draw-area primitive and link it.
 *
 * Copies the rectangle at @p srcRect, offsets its origin by the active draw
 * environment's clip origin, and clamps its width and height to a minimum of 2.
 * It then packs the @c SetDrawArea GP0 command for that rectangle into the
 * @c DR_AREA at @p prim, links @p prim into the ordering-table slot @p ot via
 * @c addPrimFast (hand-picked temp @c $s1), and returns the next packet slot.
 *
 * Sibling of @c func_800A31EC, which packs the draw env's clip rectangle
 * directly; this one takes an explicit source rectangle to clamp and offset.
 *
 * @param ot      Ordering-table slot to link the primitive into.
 * @param prim    Storage for the @c DR_AREA primitive.
 * @param srcRect Source rectangle (offset by the clip origin, then clamped).
 * @return Cursor for the next primitive (@c prim @c + @c 1).
 */
DR_AREA *func_800A3248(P_TAG *ot, DR_AREA *prim, RECT *srcRect) {
    RECT local = *srcRect;
    local.x += g_activeDrawEnv->clip.x;
    local.y += g_activeDrawEnv->clip.y;
    if (local.w < 2) local.w = 2;
    if (local.h < 2) local.h = 2;
    SetDrawArea(prim, &local);
    addPrimFast(ot, prim, s1);
    return prim + 1;
}

/**
 * @brief Temporarily shrink a rect by one pixel on each edge, link it, restore.
 *
 * Saves the rect's original 8 bytes, insets it (x/y += 1, w/h -= 2), links a
 * draw-area primitive clamped to that inset rect via @c func_800A3248, then
 * restores the original rect.
 *
 * @param ot   Ordering-table slot passed through to @c func_800A3248.
 * @param prim Primitive storage passed through to @c func_800A3248.
 * @param rect Rect to inset (x, y, w, h); saved and restored before return.
 * @return The advanced packet cursor from @c func_800A3248.
 */
DR_AREA *func_800A3320(P_TAG *ot, DR_AREA *prim, RECT *rect) {
    /* Save/restore the rect as its two 32-bit words (x|y, w|h) so the inset is
       cheaply reversible. */
    s32 *words = (s32 *)rect;
    s32 saved0 = words[0];
    s32 saved1 = words[1];
    DR_AREA *result;
    rect->x += 1;
    rect->y += 1;
    rect->w -= 2;
    rect->h -= 2;
    result = func_800A3248(ot, prim, rect);
    words[0] = saved0;
    words[1] = saved1;
    return result;
}

/**
 * @brief Scale a RECT about its center by a fixed-point factor, into @p out.
 *
 * @p scale is a signed fixed-point multiplier where @c 0x1000 (4096) represents
 * 1.0. When @c abs(scale) is exactly 1.0 the rectangle is copied verbatim.
 * Otherwise each axis is recentered: the half-extent is @c (scale * dimension) /
 * 8192 (unscale the Q12 product by 4096, then halve), the output origin is
 * @c center - halfExtent, and the output dimension is @c halfExtent * 2.
 *
 * Written in the in-place register-reuse style shared by the other FF8 RECT
 * helpers (cf. @c func_8002B080): each field is read once, then progressively
 * transformed in the same variable (x → centerX, w → halfWidth) so gcc 2.7.2
 * keeps it in one register rather than spilling copies.
 *
 * @param scale Fixed-point scale factor (@c 0x1000 == 1.0).
 * @param in    Source rectangle.
 * @param out   Destination rectangle.
 */
void func_800A3398(s32 scale, RECT *in, RECT *out) {
    if (ABS(scale) == 0x1000) {
        *out = *in;
    } else {
        s32 x = in->x;
        s32 y = in->y;
        u32 w = in->w;
        u32 h = in->h;
        u32 prodW = scale * w;
        u32 prodH = scale * h;

        x = x + (w / 2);  /* x -> center x */
        w = prodW / 8192; /* w -> half the scaled width */
        out->x = x - w;
        out->w = w * 2;

        y = y + (h / 2);  /* y -> center y */
        h = prodH / 8192; /* h -> half the scaled height */
        out->y = y - h;
        out->h = h * 2;
    }
}

/**
 * @brief Scale the cursor's view rectangle and render the framed result.
 *
 * Uses @c abs(scale) as a fixed-point factor (@c 0x1000 == 1.0). When non-zero,
 * scales @c D_801D49C8.view into @c D_801D49C8.work via @c func_800A3398, then
 * renders the frame with the packed color: the two fill passes (@c func_8002B898
 * over the work rect, @c func_8002B8BC over the view rect), bracketed — when the
 * scale is below 1.0 — by the inset and clamped draw-area borders
 * (@c func_800A3320 before, @c func_800A3248 after).
 *
 * @param ot    Ordering-table slot for the primitives.
 * @param prim  Current packet cursor.
 * @param scale Fixed-point scale factor (@c 0x1000 == 1.0); its sign is ignored.
 * @return The advanced packet cursor.
 */
DR_AREA *func_800A343C(P_TAG *ot, DR_AREA *prim, s32 scale) {
    s32 absScale = scale;
    s32 color;
    DR_AREA *result;

    color = D_801D49C8.packedColor;
    absScale = absScale >= 0 ? absScale : -absScale;

    if (absScale != 0) {
        func_800A3398(absScale, &D_801D49C8.view, &D_801D49C8.work);
        if (absScale < 0x1000) {
            prim = func_800A3320(ot, prim, &D_801D49C8.work);
        }
        result = func_8002B898(ot, prim, &D_801D49C8.work, color);
        prim = func_8002B8BC(ot, result, &D_801D49C8.view, color, 8);
        if (absScale < 0x1000) {
            prim = func_800A3248(ot, prim, &D_801D49C8.work);
        }
    }
    return prim;
}

/**
 * @brief Render one frame of the Triple Triad board cursor, including its
 *        row-to-row slide animation.
 *
 * Copies the @ref CursorState::view rectangle into @ref CursorState::work, then —
 * while sliding (@ref CursorState::slideOffset != 0) — eases the working rect across
 * the screen via the @c D_801D49F8 falloff curve: it draws the outgoing row sliding
 * away, an 8px highlight bar on the leading edge (@c func_8002B3A0), and finally
 * draws the current row into the clipped working rect plus the cursor timer pass
 * (@c func_800A343C). Individual cells are emitted by @p drawCell, called
 * @ref CursorState::unk21 times per row; @c D_801D4B18 / @c D_801D4B1A are the GPU
 * scissor X bounds set up around each pass.
 *
 * @param otBase   Ordering-table base the primitives link into.
 * @param prim     Running primitive cursor, threaded through and returned advanced.
 * @param drawCell Per-cell callback: (otBase, prim, rowIndex, column, slideFlag) -> prim.
 * @return The advanced @p prim.
 */
void *func_800A3528(void *otBase, void *prim, void *(*drawCell)(void *, void *, s32, s32, s32)) {
    void *(*draw)(void *, void *, s32, s32, s32);
    u16 clipEdge;
    CursorState *cs = &D_801D49C8;
    s32 slide = cs->slideOffset;
    long delta = 0;
    s32 rem = 0;
    u16 x = (u16) cs->view.x;
    u16 y = (u16) cs->view.y;
    u16 w = (u16) cs->view.w;
    u16 h = (u16) cs->view.h;
    s32 color = cs->packedColor;
    s32 i;

    cs->work.x = x;
    cs->work.y = y;
    cs->work.w = w;
    cs->work.h = h;

    /* Copying the callback into a local here — after the work-rect stores — shortens
       its live range so it stays in a callee-saved register across the draw loops,
       matching the original's allocation. */
    draw = drawCell;
    if (slide != 0) {
        s32 ease = ((u16 *) D_801D49F8)[(slide < 0 ? -slide : slide) / 64];
        s32 left;
        s32 right;

        /* From here on, reuse `slide` to hold the signed displacement factor. */
        if (slide < 0) {
            s32 sw = (s16) w;
            s32 sx;
            s32 mag;
            slide = -ease;
            delta = (sw * slide) / 4096;
            sx = (s16) x;
            mag = delta < 0 ? -delta : delta;
            left = sx + mag;
            right = sx + sw - mag;
        } else {
            slide = ease;
            delta = ((s16) w * slide) / 4096;
            left = (s16) x + delta;
            right = (s16) x;
        }
        clipEdge = right - 12;
        {
            s32 leftClip = clipEdge;
            D_801D4B18 = leftClip;
            D_801D4B1A = left;
        }
        i = 0;

        for (; i < cs->unk21; i++)
            prim = draw(otBase, prim, cs->prevRow, i, 0);

        if (slide < 0) {
            rem = delta < 0 ? -delta : delta;
            cs->work.w = rem;
            rem = (s16) (u16) cs->view.w - rem;
            cs->work.x = (u16) cs->view.x + rem;
            prim = func_800A3248(otBase, prim, &cs->work);
            if (rem >= 9) {
                cs->work.w = 8;
                cs->work.x = (u16) cs->view.x + rem - 8;
                prim = func_8002B3A0(otBase, prim, &cs->work, color, 2);
            }
            cs->work.w = rem - 8;
            cs->work.x = (u16) cs->view.x + 8;
        } else {
            cs->work.w = delta;
            rem = (s16) (u16) cs->view.w - delta;
            cs->work.x = (u16) cs->view.x;
            prim = func_800A3248(otBase, prim, &cs->work);
            if (rem >= 9) {
                cs->work.w = 8;
                cs->work.x = (u16) cs->view.x + delta;
                prim = func_8002B3A0(otBase, prim, &cs->work, color, 1);
            }
            cs->work.x = (u16) cs->view.x + delta;
            cs->work.w = (u16) cs->view.w - delta - 8;
        }
    }

    if (cs->work.w > 0) {
        D_801D4B18 = cs->work.x - 12;
        D_801D4B1A = cs->work.x + cs->work.w;

        for (i = 0; i < cs->unk21; i++)
            prim = draw(otBase, prim, cs->row, i, delta);

        if (rem != 0)
            prim = func_800A3248(otBase, prim, &cs->work);
    }

    prim = func_800A343C(otBase, prim, cs->timer);
    D_801D4B18 = 0;
    D_801D4B1A = 0x180;
    return prim;
}

/**
 * @brief Step a wrapping cursor by the D-pad input, playing a click per move.
 *
 * @param pad   Controller button word: @c 0x4000 (down) advances the cursor,
 *              @c 0x1000 (up) retreats it. Each accepted press plays
 *              @c sendSpuCommand(1).
 * @param count Number of selectable entries (the cursor wraps over this range).
 * @param index Current cursor index.
 * @return The updated cursor index.
 *
 * @note @p pad must be a 16-bit type — with a 32-bit param gcc reads the saved
 *       copy for the first mask test instead of the incoming argument register.
 */
s32 func_800A3884(u16 pad, s32 count, s32 index) {
    if (pad & 0x4000) {
        sendSpuCommand(1);
        index++;
        if (index >= count) {
            index = 0;
        }
    }
    if (pad & 0x1000) {
        sendSpuCommand(1);
        index--;
        if (index < 0) {
            index = count - 1;
        }
    }
    return index;
}

/** @brief Per-frame cursor/timer state machine for the Triple Triad grid cursor.
 *
 * Bumps a frame counter and advances a fixed-point timer; the cursor only acts
 * on the frame the timer reaches 0x1000 (1.0). The grid is laid out in rows of
 * 11 cells and the 6-state machine drives it:
 *   - state 0: falls through to state 1.
 *   - state 1: idle. With at least 0xC cells, a 0x8000/0x2000 bit in @p flags0
 *     begins a vertical move (-> state 2/4); otherwise the column is stepped via
 *     func_800A3884 and the cursor position rewritten.
 *   - state 2/4: begin moving up/down one row (wrapping), seed the slide offset
 *     to -/+0xE67, and advance to state 3/5.
 *   - state 3/5: ease the slide offset by +/-0x199 until it reaches 0 (back to
 *     state 1); a 0x8000/0x2000 bit in @p flags1 re-triggers a move.
 *
 * @param flags0 Input flags sampled while idle (state 1).
 * @param flags1 Input flags sampled while sliding (state 3/5).
 * @return D_801D4A88[cursorPos] (with bit 0x100 set while sliding), -1 if the
 *         timer has not fired, or -2 if the cursor position is out of range.
 */
s32 func_800A390C(s32 flags0, s32 flags1) {
    CursorState *p = &D_801D49C8;
    u8 *statePtr;
    s16 acc;
    s16 pos;
    s32 result;

    p->frameCounter++;
    p->timer += p->timerStep;
    acc = (s16)p->timer;
    if (acc <= 0) {
        p->timer = 0;
        return -1;
    }
    if (acc < 0x1000) {
        return -1;
    }
    statePtr = &p->state;
    p->timer = 0x1000;

    switch (p->state) {
    case 0:
        *statePtr = 1;
        /* fall through */
    case 1:
        if (D_801D4AF6 >= 0xC) {
            if (flags0 & 0x8000) {
                *statePtr = 2;
                break;
            }
            if (flags0 & 0x2000) {
                *statePtr = 4;
                break;
            }
        }
        {
            s16 idx = (s16)p->cursorPos;
            s32 row = idx / 11;
            s32 col = func_800A3884(flags0 & 0xFFFF, 11, idx % 11);
            p->cursorPos = row * 11 + col;
        }
        break;
    case 2: {
        s16 idx;
        s32 row, col;
        sendSpuCommand(1);
        p->prevRow = p->row;
        idx = (s16)p->cursorPos;
        row = idx / 11;
        row = row - 1;
        col = idx % 11;
        if (row < 0) {
            row = (D_801D4AF6 + 0xA) / 11 - 1;
        }
        p->cursorPos = row * 11 + col;
        p->row = row;
        p->slideOffset = -0xE67;
        *statePtr = 3;
        break;
    }
    case 3:
        p->slideOffset += 0x199;
        if ((s16)p->slideOffset >= 0) {
            p->slideOffset = 0;
            *statePtr = 1;
        }
        if (flags1 & 0x8000) {
            *statePtr = 2;
        }
        if (flags1 & 0x2000) {
            *statePtr = 4;
        }
        break;
    case 4: {
        s16 idx;
        s32 row, col;
        sendSpuCommand(1);
        p->prevRow = p->row;
        idx = (s16)p->cursorPos;
        row = idx / 11;
        row = row + 1;
        col = idx % 11;
        if (row >= (D_801D4AF6 + 0xA) / 11) {
            row = 0;
        }
        p->cursorPos = row * 11 + col;
        p->row = row;
        p->slideOffset = 0xE67;
        *statePtr = 5;
        break;
    }
    case 5:
        p->slideOffset -= 0x199;
        if ((s16)p->slideOffset <= 0) {
            p->slideOffset = 0;
            *statePtr = 1;
        }
        if (flags1 & 0x8000) {
            *statePtr = 2;
        }
        if (flags1 & 0x2000) {
            *statePtr = 4;
        }
        break;
    }

    pos = p->cursorPos;
    if (pos < D_801D4AF6) {
        result = D_801D4A88[pos];
        if (p->slideOffset != 0) {
            result |= 0x100;
        }
        return result;
    }
    return -2;
}

/**
 * @brief Build a 12x12 font-glyph sprite and prepend it to the ordering table.
 *
 * Fills a free-size @c SPRT (code 0x64, carried in @c g_menuColor) for tile
 * @p tileIdx of a 21-tile-per-row font texture: CLUT from @p palArg's low 3 bits,
 * menu color chosen by its high bits, 12x12 size, position @p xy, and UV from the
 * tile's column/row. Links the primitive at the head of the OT carried in @p ot
 * (addr<<8 form) via @c swl and returns the new head.
 *
 * Hand-optimized in the FF8 sprite/RECT style (cf. @c func_8002B080): batched
 * 32-bit stores, and in-place register reuse (the head register becomes the
 * palette-page test; the palette register becomes the color) to steer gcc 2.7.2.
 *
 * @param ot      Current OT head (packed @c addr<<8).
 * @param prim    Sprite primitive to fill.
 * @param tileIdx Glyph/tile index into the 21-per-row font texture.
 * @param palArg  Bits 2:0 = CLUT palette; bits 7:3 select the menu color.
 * @param xy      Packed screen position (x | y<<16).
 * @return The new OT head (@c prim<<8).
 */
u32 func_800A3C7C(u32 ot, SPRT *prim, s32 tileIdx, s32 palArg, u32 xy) {
    u32 head;
    s32 quo;
    s32 rem;

    ((u8 *)&prim->tag)[3] = 4; /* SPRT length = 4 words */
    memBarrier(); /* keep the head shift after the len store */
    head = (u32)prim << 8;     /* new OT head */
    setAddrFast(prim, ot);     /* link: prim's OT-next = old head */
    ot = head;

    head = (u32)palArg >> 3;   /* head register reused: now the palette page */
    palArg = palArg & 7;
    prim->clut = (palArg << 6) + 0x3812; /* getClut(288, 224 + palette) */
    if (head) {
        palArg = g_menuColor[1]; /* palette register reused: now the color */
    } else {
        palArg = g_menuColor[0];
    }

    *(u32 *)&prim->w = 0xC000C; /* 12 x 12 */
    *(u32 *)&prim->r0 = palArg; /* r, g, b, code (0x64) */
    *(u32 *)&prim->x0 = xy;

    quo = tileIdx / 21;
    rem = tileIdx % 21;
    *(u16 *)&prim->u0 = (rem | (quo << 8)) * 12; /* tile UV, 21/row, 12px cells */

    return ot;
}

/**
 * @brief Render a multi-byte glyph string (arg4) at (x, y) in colour (arg5),
 *        emitting one SPRT per glyph and splicing the chain into otBase's OT.
 *
 * Walks the string: byte 2 = new line (x reset, y += 0x10); a byte < 0x19 ends it;
 * >= 0x20 is a direct glyph (ch - 0x20); 0x19..0x1F are two-byte glyphs decoded as
 * ch*224 + next - 0x1520 (0x19..0x1B) or (ch*224 + next - 0x18A0) | 0x400 (0x1C..0x1F).
 * Each visible glyph emits a SPRT via @ref func_800A3C7C and advances x by
 * @ref getNibbleValue; D_801D4B18 / D_801D4B1A clip the x range. A trailing E100041F
 * tpage primitive is appended and the whole chain is linked into otBase's OT slot via
 * the lwl/swl tag-splice (read at the top, writes at the end — same idiom as
 * @ref func_800A3C7C).
 *
 * @param otBase  OT slot the finished chain is spliced into.
 * @param pkt     Primitive buffer cursor (SPRT-sized steps, one per glyph).
 * @param x       Left edge of the string, in screen pixels.
 * @param y       Top edge of the string, in screen pixels.
 * @param cardImg Glyph string to render (encoded as described above).
 * @param col     Palette/color selector, forwarded to @ref func_800A3C7C.
 * @return The advanced primitive-buffer cursor.
 */
void *func_800A3D2C(void *otBase, void *pkt, s32 x, s32 y, s32 cardImg, s32 col) {
    u8 *str = (u8 *)cardImg;
    s32 startX;
    s32 lo;
    s32 hi;
    s32 ch;
    u32 head;

    if (str == 0) {
        return pkt;
    }

    lo = D_801D4B18;
    hi = D_801D4B1A;
    getAddrNewFast(otBase, head);

    startX = x;
    if (y >= 0x101) {
        return pkt;
    }
    if (y < -8) {
        return pkt;
    }

    for (;;) {
        ch = *str++;
        if (ch == 2) { /* newline: carriage-return x, drop one 16px row */
            x = startX;
            y += 0x10;
            continue;
        }
        if (ch < 0x19) { /* terminator */
            break;
        }
        if (x >= 0x181) { /* past the right screen edge */
            break;
        }
        if (hi < x) { /* past the right clip bound */
            break;
        }
        if (ch >= 0x20) { /* one-byte glyph */
            ch = ch - 0x20;
        } else if (ch < 0x1C) { /* two-byte glyph, first page */
            ch = ch * 224;
            ch += *str++;
            ch -= 0x1520;
        } else { /* two-byte glyph, second page (bit 10 selects it) */
            ch = ch * 224;
            ch += *str++;
            ch -= 0x18A0;
            ch |= 0x400;
        }
        if (lo < x) { /* left clip: advance x but emit nothing before it */
            u32 xy = (y << 16) | (x & 0xFFFF);
            head = func_800A3C7C(head, (SPRT *)pkt, ch, col, xy);
            pkt = (u8 *)pkt + 0x18; /* packet stride: SPRT (0x14) + one pad word */
        }
        x += getNibbleValue(ch);
    }

    ((u8 *)pkt)[3] = 1; /* trailing primitive: 1 word */
    *(u32 *)((u8 *)pkt + 4) = 0xE100041F; /* DR_TPAGE restore, texpage 0x41F */
    addOtFast(pkt, head);
    pkt = (u8 *)pkt + 8;
    setAddrFast(otBase, head);
    return pkt;
}

/* OT splices for func_800A3EE0. Unlike the hand-picked-register sites served by
 * common.h's addPrimFast (register-name argument + clobber), these two link temps
 * are genuine compiler-allocated asm operands: the loop site's in-out operand and
 * the tail site's input-only operand are what rank the temps into $s4/$s7 (the
 * names record where they land). A treg-clobber form instead frees those registers
 * as "free saves" for other pseudos and mismatches. */
#define addPrimFastS4(ot, p) do {                        \
    __asm__ __volatile__(                                \
        ".set push\n"                                    \
        ".set noat\n"                                    \
        "sll $at, %2, 8\n"                               \
        "lwl %0, 2(%1)\n"                                \
        "swl $at, 2(%1)\n"                               \
        "swl %0, 2(%2)\n"                                 \
        ".set pop\n"                                     \
        : "+r"(s4)                                        \
        : "r"(ot), "r"(p)                                 \
        : "memory");                                      \
} while (0)

#define addPrimFastS7(ot, p) do {                        \
    __asm__ __volatile__(                                \
        ".set push\n"                                    \
        ".set noat\n"                                    \
        "sll $at, %2, 8\n"                               \
        "lwl %0, 2(%1)\n"                                \
        "swl $at, 2(%1)\n"                               \
        "swl %0, 2(%2)\n"                                 \
        ".set pop\n"                                     \
        :                                                \
        : "r"(s7), "r"(ot), "r"(p)                       \
        : "memory");                                      \
} while (0)

/**
 * @brief Render a right-aligned decimal number (e.g. a Triple Triad score/count)
 *        as a row of 12x12 textured SPRTs, linked into @p ot.
 *
 * Formats @p number into the digit-string buffer @ref D_801D49B8 via
 * @ref intToDecString, then:
 *  - Skips leading '1'-valued glyphs (the formatter's leading-zero placeholder),
 *    advancing @c str, capped at ~9.
 *  - Sums the per-glyph widths (low nibble of each @ref FontGlyph entry,
 *    table = `D_8008371C - 1`, 1-indexed) over the remaining digits.
 *  - Subtracts that total from the x-coordinate to right-align the number.
 *  - Emits one 12x12 SPRT per glyph: colour = @p color, len = 4, w/h = 0xC000C,
 *    u/v from the glyph entry's `uv` halfword, x/y from @p x (low s16 = x,
 *    high s16 = y), clut = `(clutPage << 6) + 0x3812` (getClut(288, 224+pal)).
 *    Glyphs at or left of the clip edge @ref D_801D4B18 are written but not
 *    linked (skipped). Each linked SPRT splices into @p ot via @c addPrimFastS4;
 *    x advances by the glyph width after each.
 *  - Appends a trailing 0xE100041F tpage primitive (@c addPrimFastS7) and
 *    returns the next packet slot (prim + 8).
 *
 * @param ot       OT slot the SPRT chain links into (splice head).
 * @param prim     Packet buffer cursor (SPRT-sized, 0x14 bytes/glyph).
 * @param x        Packed position: low s16 = x, high s16 = y (unpacked in place).
 * @param number   Value to format and render.
 * @param color    Packed r/g/b/code stored into each SPRT (arg4, on stack).
 * @param clutPage Palette page; clut = (clutPage << 6) + 0x3812 (arg5, on stack).
 * @return Pointer to the next free packet slot (`(u8 *)prim + 8`).
 *
 * @note Matching notes: the memory barrier after the glyph-table setup is the
 *       compiler fence the original demonstrably had (battle-module clone
 *       func_800330F4 shares the byte-identical prologue) — it keeps the two
 *       stack-parameter home loads (color/clutPage) out of the call cluster and
 *       stops gcc folding number's a3 home-copy into the call, which is what
 *       homes @p number in $a0 at the top. Passing @ref D_801D49B8 to the call
 *       and assigning @c str afterwards makes the address's %hi temp cross the
 *       call so it lands in $s0. The `c = *scan++` loads put the increment in
 *       each loop's branch delay slot (leaving the glyph-width load stall as a
 *       nop), matching the original loop shape.
 */
void *func_800A3EE0(void *ot, SPRT *prim, s32 x, s32 number, u32 color, s32 clutPage) {
    u32 s4;
    u32 s7;
    FontGlyph *glyphs;
    u8 *str;
    u8 *scan;
    s32 y;
    s32 width;
    s32 count;

    glyphs = D_8008371C;
    glyphs = glyphs - 1;
    memBarrier(); /* keeps the color/clutPage home loads and number's $a0 home up here
                     (blocks the merge/sinking that would glue them to the call) */
    y = x >> 16;
    x <<= 16;
    x >>= 16;
    intToDecString(number, D_801D49B8, 1);
    str = D_801D49B8;

    /* Drop up to 9 leading pad glyphs (id 1). */
    for (count = 0; count < 9; count++) {
        if (*str != 1) {
            break;
        }
        str++;
    }

    /* Sum the glyph widths, then right-align. */
    width = 0;
    scan = str;
    for (;;) {
        u32 c = *scan++;
        if (c == 0) {
            break;
        }
        width += glyphs[c].width & 0xF;
    }
    x -= width;

    scan = str;
    for (;;) {
        FontGlyph *g;
        u32 c = *scan++;
        if (c == 0) {
            break;
        }
        g = &glyphs[c];
        *(u16 *)&prim->u0 = g->uv;
        *(u32 *)&prim->r0 = color;
        ((u8 *)&prim->tag)[3] = 4;    /* SPRT length = 4 words */
        prim->x0 = x;
        prim->y0 = y;
        prim->clut = (clutPage << 6) + 0x3812;
        *(u32 *)&prim->w = 0xC000C;   /* 12 x 12 */
        if (D_801D4B18 < x) {         /* left clip: emit but don't link */
            addPrimFastS4(ot, prim);
            prim++;
        }
        x += g->width & 0xF;
    }

    /* Cap the list with a texture-page restore primitive. */
    ((u8 *)&prim->tag)[3] = 1;
    *(u32 *)&prim->r0 = 0xE100041F;
    addPrimFastS7(ot, prim);
    return (u8 *)prim + 8;
}

/**
 * @brief Wrapper for func_800A3EE0 that selects a lookup table entry based on the 5th argument.
 *
 * If stack0 >= 8, uses g_menuColor[1] and subtracts 8 from stack0.
 * Otherwise uses g_menuColor[0] with stack0 unchanged.
 * Passes the lookup value and adjusted stack0 as extra args to func_800A3EE0.
 *
 * @param a0-a3 Parameters passed through to func_800A3EE0.
 * @param stack0 Index parameter; if >= 8, adjusted by -8 and table index 1 is used.
 * @return The advanced packet cursor from func_800A3EE0 (returned by the tail call;
 *         func_800A40F0 threads it through). Typed @c void* rather than @c void.
 */
void *func_800A4098(void *a0, void *a1, s32 a2, s32 a3, s32 stack0) {
    s32 idx;
    if (stack0 >= 8) {
        stack0 -= 8;
        idx = 1;
    } else {
        idx = 0;
    }
    return func_800A3EE0(a0, a1, a2, a3, g_menuColor[idx], stack0);
}

/**
 * @brief Render one Triple Triad grid cell (number + card image + frame).
 *
 * Maps the (@p row, @p col) position to a linear grid index (@c row*11+col) and
 * bails — returning @p pkt unchanged — if it is past the populated cell count
 * (@c D_801D4AF6). Otherwise it looks up the cell's card index in
 * @c D_801D4A88, picks a highlight color (7 if the card passes @c func_80023B14,
 * else 1), and emits three primitives at a position derived from the cursor
 * view rect: a glyph (@c func_8002FF34), the card image (@c func_800A3D2C), and
 * a frame (@c func_800A4098). The packet cursor is threaded through and returned.
 *
 * @param otBase  Ordering-table base forwarded to each emitter.
 * @param pkt     Packet cursor, advanced by each primitive and returned.
 * @param row     Grid row (multiplied by 11 for the linear index).
 * @param col     Grid column (drives the vertical position; col*13).
 * @param xOffset Horizontal offset added to the view's left edge.
 * @return The advanced packet cursor (or @p pkt unchanged if out of range).
 */
void *func_800A40F0(void *otBase, void *pkt, s32 row, s32 col, s32 xOffset)
{
    CursorState *cs = &D_801D49C8;
    s32 cell = row * 11 + col;
    s32 valid;
    s32 color;
    s32 y;
    s32 x;
    s32 cardImg;

    if (cell >= D_801D4AF6) {
        return pkt;
    }

    cell = D_801D4A88[cell];
    color = 1;
    valid = func_80023B14(cell);
    if (valid > 0) {
        color = 7;
    }

    /* The X coordinate is staged through cardImg (later reused for the card
       image) so the compiler keeps it in the register the original used. */
    x = (cardImg = D_801D49C8.view.x + xOffset);
    y = cs->view.y + col * 13 + 8;

    pkt = func_8002FF34(otBase, pkt, 0xD7, x + 7, y, cs->packedColor);
    cardImg = (s32)func_80023A54(cell);
    pkt = func_800A3D2C(otBase, pkt, x + 0x15, y, cardImg, color);
    x += 0x9A;
    return func_800A4098(otBase, pkt, (y << 16) | (x & 0xFFFF), valid, color);
}

/**
 * @brief Emit one glyph into the OT at a grid cell derived from a linear index.
 *
 * Forwards to the glyph emitter @c func_8002FF34 (glyph 0) with the position
 * taken from @p ctx and the column @c a3 @c % @c 11 at a 13px pitch. Returns
 * the advanced packet cursor.
 *
 * @note @c w (@c ctx->unk02 @c + @c 0xB) must be a separate local — inlining it
 *       lets gcc reassociate the @c +0xB into the @c *13 term and reorder the
 *       adds away from the target.
 */
void *func_800A4250(s32 *otBase, void *pkt, func_800A4250_arg2 *ctx, s32 a3) {
    s32 w = ctx->unk02 + 0xB;
    return func_8002FF34(otBase, pkt, 0,
                         ctx->unk00 - 0x13,
                         w + (a3 % 11) * 13,
                         ctx->unk10);
}

/**
 * @brief Render the Triple Triad board overlay for the current cursor frame.
 *
 * Does nothing (returns @p pkt) until the cursor timer (@ref CursorState::timer)
 * is positive. While it is, emits the board chrome — at full scale (timer ==
 * 0x1000) and only when @ref CursorState::unk24 is set, the highlighted cursor
 * cell via @c func_800A4250 — then the frame border glyphs and, once at least
 * 0xC cells exist, the blinking corner markers (@c func_800A2FCC) and the count
 * (@c func_800A31B8). Finally it scales the view rect into the work rect by the
 * timer (@c func_800A3398), draws the clamped frame (@c func_800A3320), and
 * renders every populated cell through @c func_800A3528 with @c func_800A40F0
 * as the per-cell callback. The packet cursor is threaded through and returned.
 *
 * @param otBase Ordering-table base forwarded to each emitter.
 * @param pkt    Packet cursor, advanced by each primitive and returned.
 * @return The advanced packet cursor (or @p pkt unchanged if the timer is idle).
 */
void *func_800A42D0(void *otBase, void *pkt)
{
    CursorState *cs = &D_801D49C8;

    if (cs->timer <= 0) {
        return pkt;
    }

    if (cs->timer == 0x1000 && cs->unk24 != 0) {
        pkt = func_800A4250(otBase, pkt, (func_800A4250_arg2 *)cs, cs->cursorPos);
    }

    pkt = func_800A31EC(otBase, pkt);
    pkt = func_8002FF34(otBase, pkt, 0x4D, cs->view.x + 0x7F, cs->view.y, cs->packedColor);

    if (D_801D4AF6 >= 0xC) {
        pkt = func_800A2FCC(otBase, pkt, cs->packedColor, 3);
        pkt = func_800A31B8(otBase, pkt, cs->view.x + 0x28, cs->view.y, cs->packedColor, cs->row);
    }

    pkt = func_8002FF34(otBase, pkt, 0x59, cs->view.x, cs->view.y, cs->packedColor);
    func_800A3398(cs->timer, &cs->view, &cs->work);
    pkt = func_800A3320(otBase, pkt, &cs->work);
    return func_800A3528(otBase, pkt, func_800A40F0);
}

/**
 * @brief Apply func_800A42D0 with VSync lock/unlock.
 *
 * @param a0 Parameter passed through to func_800A42D0.
 * @return Result of storeGpuPacket (VSync unlock).
 */
s32 func_800A443C(s32 a0) {
    s32 saved = a0;
    s32 lock = getDisplayListHead();
    s32 result = (s32)func_800A42D0((void *)saved, (void *)lock);
    return storeGpuPacket(result);
}

/**
 * @brief Compute and store a packed color/brightness value for the camera.
 *
 * Stores @p a0 to @ref CursorState::brightness, then computes a packed 32-bit
 * value by dividing a0 by 32 (rounding toward zero) and replicating the result
 * into bytes 0-2 with byte 3 set to 0x64, storing it to
 * @ref CursorState::packedColor.
 *
 * @param a0 Brightness value (scaled by 32).
 */
void func_800A4478(s32 a0) {
    s32 val;
    D_801D49C8.brightness = a0;
    if (a0 < 0) {
        a0 += 0x1F;
    }
    val = a0 >> 5;
    {
        s32 hi = (val << 8) | (val << 16);
        D_801D49C8.packedColor = (val | hi) | 0x64000000;
    }
}

/**
 * @brief Store a byte value to the cursor state's unk24 field.
 *
 * @param a0 Byte value to store.
 */
void func_800A44B0(s32 a0) {
    D_801D49C8.unk24 = a0;
}

/**
 * @brief Set the cursor state's timer step to -256 (0xFF00).
 */
void func_800A44BC(void) {
    D_801D49C8.timerStep = -256;
}

/**
 * @brief Reset D_801D49C8 fields and call func_800A2E44.
 *
 * Sets @ref CursorState::timerStep to 0x100 and clears @ref CursorState::state,
 * @ref CursorState::cursorPos and @ref CursorState::row, then calls func_800A2E44
 * for further reset.
 */
void func_800A44CC(void) {
    D_801D49C8.timerStep = 0x100;
    D_801D49C8.state = 0;
    D_801D49C8.cursorPos = 0;
    D_801D49C8.row = 0;
    func_800A2E44();
}

/**
 * @brief Initialize the D_801D49C8 camera structure.
 *
 * Sets position, dimensions, mode fields, clears various flags,
 * then calls func_800A4478 and func_800A2F78 for further init.
 * Also initializes D_801D4B18 to 0 and D_801D4B1A to 0x180.
 *
 * @param a0 View X position.
 * @param a1 View Y position.
 */
void func_800A4504(s32 a0, s32 a1) {
    D_801D49C8.view.x = a0;
    D_801D49C8.view.w = 0xA1;
    D_801D49C8.view.h = 0x9F;
    D_801D49C8.unk21 = 0xB;
    D_801D49C8.view.y = a1;
    D_801D49C8.timerStep = 0;
    D_801D49C8.timer = 0;
    D_801D49C8.cursorPos = 0;
    D_801D49C8.slideOffset = 0;
    D_801D49C8.unk20 = 0;
    D_801D49C8.row = 0;
    D_801D49C8.prevRow = 0;
    D_801D49C8.frameCounter = 0;
    D_801D49C8.unk24 = 1;
    func_800A4478(0x1000);
    func_800A2F78();
    D_801D4B18 = 0;
    D_801D4B1A = 0x180;
}

