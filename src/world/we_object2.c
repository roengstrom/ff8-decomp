#include "common.h"
#include "battle.h"
#include "field.h"
#include "gamestate.h"
#include "sound.h"
#include "btl_sfx.h"
#include "world.h"
#include "world/we_object2.h"

/**
 * @brief Walk a world-engine image script and blit each record to VRAM.
 *
 * @p script begins with a 0-terminated table of @c s32 byte-offsets; each
 * non-zero entry points to a streaming-image record at @c (u8*)script+offset.
 * Each record is parsed by @c func_8009CA34 into an @ref ImageDesc, then
 * @c rect1 / @c data1 are blitted through the scratch rect @c D_800C8640 (via
 * @c func_80048EFC). When bit 3 of @c desc.flag is set, @c rect2 / @c data2
 * are blitted the same way.
 *
 * @param script World-engine image script (offset table + image records).
 */
void loadImageScript(s32 *script) {
    s32 i = 0;
    s32 off = script[0];

    while (off != 0) {
        ImageDesc desc;
        func_8009CA34((s32 *)((u8 *)script + off), &desc);
        D_800C8640.x = desc.rect1.x;
        D_800C8640.y = desc.rect1.y;
        D_800C8640.w = desc.rect1.w;
        D_800C8640.h = desc.rect1.h;
        func_80048EFC(&D_800C8640, desc.data1);
        if ((desc.flag >> 3) & 1) {
            D_800C8640.x = desc.rect2.x;
            D_800C8640.y = desc.rect2.y;
            D_800C8640.w = desc.rect2.w;
            D_800C8640.h = desc.rect2.h;
            func_80048EFC(&D_800C8640, desc.data2);
        }
        i++;
        off = script[i];
    }
}


/**
 * @brief Dispatch SPU cmd 0x1A with (@p a0, 0x78, @p val), stash return as handle.
 *
 * Sister of func_8009CDFC: saves the return value of
 * @c sndCmd1A(a0, 0x78, val) — where @p val is sign-extended from s8 —
 * into the field-engine state's @c soundHandle0 slot.
 *
 * @note @c sndCmd1A is declared here as returning @c s32 for the same reason
 *       as func_8009CDFC's @c sndCmd10 override.
 *
 * @param a0 Parameter for SPU command 0x1A.
 * @param val Signed 8-bit value forwarded as the third arg.
 */
void func_8009CDC4(s32 a0, s8 val) {
    g_fieldVars->soundHandle0 = sndCmd1A(a0, 0x78, val);
}

/* sndCmd10 / sndCmdC1 prototypes come from <sound.h>. */

/**
 * @brief Dispatch SPU cmd 0x10 with @p a0, stash its handle, then issue cmd C1.
 *
 * Saves the return value of @c sndCmd10(a0) into the field-engine state's
 * @c soundHandle0 slot, then calls @c sndCmdC1(0, 0x3C, val) where @p val
 * is sign-extended from s8.
 *
 * @note @c sndCmd10 is declared here as returning @c s32 even though its
 *       definition in snd_init.c is @c void — the return register (v0)
 *       carries the chained return from @c func_8001A1E8 that the caller
 *       uses here. Overlay-local signature override.
 *
 * @param a0 Parameter for SPU command 0x10.
 * @param val Signed 8-bit value forwarded to sndCmdC1 as the third arg.
 */
void func_8009CDFC(s32 a0, s8 val) {
    g_fieldVars->soundHandle0 = sndCmd10(a0);
    sndCmdC1(0, 0x3C, val);
}

/**
 * @brief Send sound command 0x11 then reset audio channel 0's state byte.
 *
 * Issues @c sndCmd11(0) and marks audio channel 0 as inactive by writing
 * -1 to the field-engine state's @c audioChannel0State byte.
 */
void func_8009CE40(void) {
    sndCmd11(0);
    g_fieldVars->audioChannel0State = -1;
}

/* Defined later in this file. */
s32 lookupScrollBounds(s8 cmd, s32 *outBase, s32 *outLimit, s32 *outSpan, s32 *outFlag);

/**
 * @brief Query scroll bounds for @p cmd plus a scene-mode bank selector and
 *        the matching data table, distributing the results to out-params.
 *
 * Derives a bank-selector flag (@c outBankSel) from the scene: for scene mode
 * @c 2 it's @c D_80082C11, otherwise it's @c (g_fieldVars->soundBankSelector
 * == 0). It then runs @ref lookupScrollBounds for the (base, limit, span, set)
 * tuple; on a hit it selects a data table (@c D_80063388 when the bank flag is
 * set, else @c D_8005F388). Each non-NULL out-param receives its value and the
 * scroll base is returned.
 *
 * @param cmd       Command / key code.
 * @param outBankSel Receives the bank-selector flag.
 * @param outReady  Receives the lookupScrollBounds "matched" flag.
 * @param outLimit  Receives the scroll limit.
 * @param outTable  Receives the selected data-table pointer (or 0).
 * @param outSpan   Receives the scroll span (limit - base).
 * @return The scroll base.
 */
s32 getScrollState(s8 cmd, s32 *outBankSel, s32 *outReady, s32 *outLimit,
                   s32 *outTable, s32 *outSpan) {
    s32 base, limit, span, set;
    s32 bankSel;
    s32 table = 0;

    span = 0;
    base = 0;
    limit = 0;
    set = 0;

    if (D_80082C8C.mode == 2) {
        bankSel = D_80082C11;
    } else {
        bankSel = (g_fieldVars->soundBankSelector == 0);
    }

    if (lookupScrollBounds(cmd, &base, &limit, &span, &set) != 0) {
        table = (bankSel == 0) ? (s32)D_8005F388 : (s32)D_80063388;
    }

    if (outLimit)   *outLimit   = limit;
    if (outBankSel) *outBankSel = bankSel;
    if (outReady)   *outReady   = set;
    if (outTable)   *outTable   = table;
    if (outSpan)    *outSpan    = span;
    return base;
}

/**
 * @brief Look up per-command screen-scroll bounds for the current scene cmd.
 *
 * For a recognised @p cmd / scene-command (@c D_80082C8C.cmd) pair, fills the
 * requested out-params with a (base, limit, span) triple and a mode flag,
 * returning 1; otherwise leaves them untouched and returns 0. Two cases:
 *  - "set A" (cmd @c 0x29 on most maps, or @c 0x59 / @c 0x51 on the common map
 *    set) → @c D_800C9E68 / @c D_800C97EC / their difference, flag @c 1;
 *  - "set B" (cmd @c 0x18) → @c D_800C9E70 / @c D_800C97F0 / their difference,
 *    flag @c 0.
 *
 * @param cmd     Command / key code (signed).
 * @param outBase Receives the base value (if non-NULL).
 * @param outLimit Receives the limit value (if non-NULL).
 * @param outSpan Receives @c limit-base (if non-NULL).
 * @param outFlag Receives the set flag, @c 1 (A) or @c 0 (B) (if non-NULL).
 * @return @c 1 if @p cmd was recognised, else @c 0.
 */
s32 lookupScrollBounds(s8 cmd, s32 *outBase, s32 *outLimit, s32 *outSpan, s32 *outFlag) {
    s32 map = D_80082C8C.cmd;
    s32 ret = 0;

    if ((cmd == 0x29 && map != 0x31 && map != 0x32) ||
        (((u32)map < 0xA || map == 0x80 || map == 0x30) && (cmd == 0x59 || cmd == 0x51))) {
        if (outBase)  *outBase  = D_800C9E68;
        if (outLimit) *outLimit = D_800C97EC;
        if (outSpan)  *outSpan  = D_800C97EC - D_800C9E68;
        if (outFlag)  *outFlag  = 1;
        ret = 1;
    } else if (cmd == 0x18) {
        if (outBase)  *outBase  = D_800C9E70;
        if (outLimit) *outLimit = D_800C97F0;
        if (outSpan)  *outSpan  = D_800C97F0 - D_800C9E70;
        if (outFlag)  *outFlag  = 0;
        ret = 1;
    }
    return ret;
}


/**
 * @brief Vsync, scroll any partial display window back to origin, then
 *        rebuild the screen-0 DRAWENV/DISPENV and clear the entity model.
 *
 * Steps:
 *  1. @c VSync(0) — wait for vblank.
 *  2. If @c (DISPENV*)D_8005F138->disp.x is non-zero, @c MoveImage
 *     scrolls the live display rect back to @c (0,0) and a @c DrawSync
 *     stalls until the move completes.
 *  3. @c func_800A5F78(0) / @c func_800A5FD4(0) push the screen-0
 *     DRAWENV and DISPENV.
 *  4. @c ClearImage((RECT*)&D_800CA040, 0,0,0) blacks the entity-model
 *     blob (the first 8 bytes of @ref D_800CA040 are interpreted as a
 *     @c RECT here).
 *  5. Final @c DrawSync(0) before returning.
 *
 * The unused @c DRAWENV / @c DISPENV locals match the original stack
 * frame — they live on @c sp but are never written here (a leftover
 * from the helper inlining the per-screen env setup once did).
 */
void func_8009D0F0(void) {
    DRAWENV draw;
    DISPENV disp;
    DISPENV *env;

    VSync(0);
    env = (DISPENV *)D_8005F138;
    if (env->disp.x != 0) {
        MoveImage(&env->disp, 0, 0);
        DrawSync(0);
    }
    func_800A5F78(0);
    func_800A5FD4(0);
    ClearImage(&D_800CA040, 0, 0, 0);
    DrawSync(0);
}

/**
 * @brief If sequence @p idx isn't already running, kick it off via func_80099EDC.
 *
 * Checks @c D_800C4FD8[idx].field12 — if it's 0 (idle), calls
 * @c func_80099EDC(idx) to start the sequence and returns 1. If already
 * running (non-zero field12), returns 0 without doing anything.
 *
 * @param idx Index into the @c D_800C4FD8 sequence table.
 * @return 1 if the sequence was started, 0 if it was already running.
 */
s32 func_8009D16C(s32 idx) {
    if (D_800C4FD8[idx].field12 != 0) return 0;
    func_80099EDC(idx);
    return 1;
}

/**
 * @brief Stop sequence @p idx via sndCmd21 if it's running, and clear its state.
 *
 * If @c D_800C4FD8[idx].field12 is zero (sequence not running), returns 0.
 * Otherwise dispatches @c sndCmd21(entry->field10, entry->field04),
 * clears @c field12, and returns 1.
 *
 * @param idx Index into the @c D_800C4FD8 sequence table.
 * @return 1 if the sequence was stopped, 0 if it wasn't running.
 */
s32 func_8009D1B8(s32 idx) {
    if (D_800C4FD8[idx].field12 == 0) return 0;
    sndCmd21(D_800C4FD8[idx].field10, D_800C4FD8[idx].field04);
    D_800C4FD8[idx].field12 = 0;
    return 1;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object2", func_8009D214);

/**
 * @brief Collect the descriptor pairs of all currently-active key entries.
 *
 * Walks every @ref Entry12 in the packed @c D_800C9880 buffer. For an entry
 * whose @c key indexes a non-empty script record (via the @c D_800C9728 offset
 * table), it publishes @c val0>>1 / @c val1>>1 to @c D_800C4DA0 / @c D_800C4DA4,
 * runs @c func_800BEDF0, and — when that reports an active script whose current
 * key equals the entry's @c key — appends the entry's @c val0 and @c val1 (as
 * halfwords) to @p out and counts it.
 *
 * @param out Destination halfword array; receives (val0, val1) pairs.
 * @return Number of active entries written (low 8 bits).
 */
u8 collectActiveKeyEntries(s16 *out) {
    KeyBuffer *buf = D_800C9880;
    Entry12 *e   = &buf->entries[0];
    Entry12 *end = (Entry12 *)((u8 *)buf + buf->length);
    s32 count = 0;
    u8  keyOut;

    while (e < end) {
        u32 *t   = D_800C9728;
        u8  *rec = (u8 *)t + t[e->key];
        if (rec[0] != 0) {
            D_800C4DA0 = e->val0 >> 1;
            D_800C4DA4 = e->val1 >> 1;
            if (func_800BEDF0(&keyOut) != 0 && keyOut == e->key) {
                out[0] = e->val0;
                out[1] = e->val1;
                count++;
                out += 2;
            }
        }
        e++;
    }
    return count;
}


/**
 * @brief Reset the scene state block and kick off the scene loader.
 *
 * Chains:
 *  1. @c func_8009D630() to tear down prior state.
 *  2. Resets @c D_80082C8C — mode 5, current dispatch code, markers -1.
 *  3. Calls @c func_800B3FD4 with the scene-data pointer @c D_800D226C
 *     and sub-mode 5.
 */
void func_8009D3F4(void) {
    func_8009D630();
    D_80082C8C.mode = 5;
    D_80082C8C.cmd = D_800C4D38;
    D_80082C8C.unk02 = -1;
    D_80082C8C.unk03 = -1;
    func_800B3FD4(D_800D226C, 5);
}


/**
 * @brief Enter scene mode @c 3 with a packed @c marker, then mass-reset
 *        the world @c SfxSlot table and trigger a render flush.
 *
 * Steps:
 *  1. Snapshot @p marker into @c D_800C8CEA, tear down prior scene
 *     state via @c func_8009D630, and reset @c D_80082C8C to mode 3
 *     with the marker's two bytes in @c unk02 / @c unk03 and the
 *     current dispatch code (@c D_800C4D38) in @c cmd.
 *  2. @c func_800B3FD4(D_800D226C, 3) re-enters the scene driver.
 *  3. Walk the first 13 @c D_800C526C @c SfxSlot entries: fade out each
 *     active slot via @c fadeOutSfxFast and mark it inactive (@c -1).
 *  4. Force a full render: @c renderAndUpdateDisplay(2) then dispatch
 *     the world display list at @c D_800D244C+0x74.
 */
void func_8009D44C(s32 marker) {
    s32 i;
    D_800C8CEA = marker;
    func_8009D630();
    D_80082C8C.mode = 3;
    D_80082C8C.unk02 = (s8)marker;
    D_80082C8C.unk03 = (s8)((u32)marker >> 8);
    D_80082C8C.cmd = D_800C4D38;
    func_800B3FD4(D_800D226C, 3);
    D_80082C0A = 0;
    for (i = 0; i < 13; i++) {
        s32 sfx = D_800C526C[i].field02;
        D_800C526C[i].field00 = -1;
        fadeOutSfxFast(sfx);
    }
    renderAndUpdateDisplay(2);
    renderBattleDisplayList(&D_800D244C->primList[BSC_COLORTAG_IDX]);
}

/**
 * @brief World-map sound/display setup: enable reverb, publish the active
 *        display environment, and register the world render callback.
 *
 * After the per-frame display reset (@c func_8009D630 / @c func_80048C50 /
 * @c func_800488D4) and enabling reverb, it copies the scene's disp-env
 * template into @c D_80082C18 and publishes its address via @c D_8005F138 —
 * taking the sentinel's secondary disp (@c CtxDispView at @c +0x40CC) when the
 * active context @c D_800D244C is the no-battle sentinel @c D_800CA040, else
 * the context's own @c disp at @c +0x5C. Finally it installs the world render
 * callback @c func_800A47A4 and disables reverb.
 */
void setupWorldRender(void) {
    func_8009D630();
    func_80048C50(0);
    func_800488D4(3);
    sndEnableReverb(0);
    if (D_800D244C == &D_800CA040) {
        D_80082C18 = ((CtxDispView *)&D_800CA040)->disp;
        D_8005F138 = (s32)&D_80082C18;
    } else {
        D_80082C18 = D_800CA040.disp;
        D_8005F138 = (s32)&D_80082C18;
    }
    func_800C3DB0(func_800A47A4);
    sndDisableReverb(0);
}


/**
 * @brief Tear down the active session tracked by D_800C4D84/D_800C4D88.
 *
 * If either @c D_800C4D84 is non-zero (active handle) or
 * @c D_800C4D88 is non-negative (valid slot), calls @c func_800C4450
 * to release the session, clears @c D_800C4D84, and marks
 * @c D_800C4D88 as -1 (inactive).
 *
 * A no-op when both trackers already report inactive.
 */
void func_8009D630(void) {
    if (D_800C4D84 != 0 || D_800C4D88 >= 0) {
        func_800C4450();
        D_800C4D84 = 0;
        D_800C4D88 = -1;
    }
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object2", func_8009D688);

/**
 * @brief 4-byte slot (two halfwords). Default value is two @c -1s.
 */

/**
 * @brief Fill three adjacent @ref HalfwordPair slots from three lookup
 *        helpers, defaulting each slot to @c (-1, @c -1) on lookup miss.
 *
 * Walks @p s through three consecutive slots, calling one of three
 * fetchers per slot:
 *  - slot @c [0]: @c func_800BD380
 *  - slot @c [1]: @c func_800BD2A0
 *  - slot @c [2]: @c func_800BD460
 *
 * Each fetcher writes into @c (&s->a, @c &s->b) and returns non-zero on
 * success. If it returns @c 0 (no entry available), the slot is filled
 * with @c (-1, @c -1) — the "inactive" sentinel.
 *
 * @param s First slot of a three-slot array.
 */
void func_8009D760(HalfwordPair *s) {
    if (func_800BD380(&s->a, &s->b) == 0) {
        s->b = -1;
        s->a = -1;
    }
    s++;
    if (func_800BD2A0(&s->a, &s->b) == 0) {
        s->b = -1;
        s->a = -1;
    }
    s++;
    if (func_800BD460(&s->a, &s->b) == 0) {
        s->b = -1;
        s->a = -1;
    }
}

/** @brief Compare input against two entity IDs, return 0x29, 0x18, or -1. */
s32 func_8009D7D8(s32 a0) {
    s32 result = -1;

    if (D_800C9E68 == a0) {
        result = 0x29;
    } else if (D_800C9E70 == a0) {
        result = 0x18;
    }
    return result;
}

/** Clears bit 0x40 on two related flag bytes. */
void func_8009D814(void) {
    D_800780D8[0x108] &= ~0x40;
    D_800D23D8[0x66] &= ~0x40;
}

/**
 * @brief Clear two flag bytes at entity-relative offsets.
 * @param a0 Entity slot index offset.
 */
void func_8009D840(s32 a0) {
    u8 *base1 = D_800780D8;
    u8 *base2 = D_800D23D8;

    *(u8 *)(a0 + (s32)base1 + 0x10D) = 0;
    *(u8 *)(a0 + (s32)base2 + 0x6B) = 0;
}


/**
 * @brief Dispatch a table-driven SFX slot cleanup if the slot is active.
 *
 * Looks up @c D_800C526C[idx]; if its @c field00 isn't -1 (active),
 * forwards its @c field02 to @c func_8002CE84 (an SFX helper in btl_sfx).
 *
 * @param idx Index into the SFX slot table.
 */
void func_8009D864(s32 idx) {
    if (D_800C526C[idx].field00 != -1) {
        func_8002CE84(D_800C526C[idx].field02);
    }
}


/**
 * @brief Fade out the SFX referenced by slot @p idx and mark the slot inactive.
 *
 * If the slot is active (@c field00 != -1), writes -1 to deactivate it and
 * calls @c fadeOutSfxSlow with the slot's SFX index (@c field02).
 *
 * @param idx Index into the @c D_800C526C SFX slot table.
 */
void func_8009D8A8(s32 idx) {
    s32 field00 = D_800C526C[idx].field00;
    s32 field02 = D_800C526C[idx].field02;
    if (field00 != -1) {
        D_800C526C[idx].field00 = -1;
        fadeOutSfxSlow(field02);
    }
}

/**
 * @brief Fade out every active SFX slot in @c D_800C526C and mark them inactive.
 *
 * Per-slot loop version of @c func_8009D8A8 — walks all 13 entries of
 * @c D_800C526C, and for each slot whose @c field00 is not @c -1, writes
 * @c -1 into @c field00 and calls @c fadeOutSfxSlow with the slot's
 * @c field02 (SFX index). Used at world-map teardown to silence every
 * field-engine-owned SFX channel in one pass.
 */
void func_8009D8F0(void) {
    s32 i;
    for (i = 0; i < 13; i++) {
        s32 field00 = D_800C526C[i].field00;
        s32 field02 = D_800C526C[i].field02;
        if (field00 != -1) {
            D_800C526C[i].field00 = -1;
            fadeOutSfxSlow(field02);
        }
    }
}


/**
 * @brief Hard tear-down: fade every @c D_800C526C SFX slot fast, then push
 *        a 2-frame display flush.
 *
 * Walks all 13 entries of @c D_800C526C and unconditionally marks each
 * @c field00 inactive (@c -1) and calls @c fadeOutSfxFast on its
 * @c field02 — the harder/faster sibling of @c func_8009D8F0, which
 * only acts on already-active slots. Then pushes @c 2 frames via
 * @c renderAndUpdateDisplay and flushes the battle scene's
 * @c colorTag into the display list.
 *
 * Used at world/battle hand-off when an immediate audio cut and frame
 * push are required.
 */
void func_8009D954(void) {
    s32 i;
    for (i = 0; i < 13; i++) {
        s32 sfxIdx = D_800C526C[i].field02;
        D_800C526C[i].field00 = -1;
        fadeOutSfxFast(sfxIdx);
    }
    renderAndUpdateDisplay(2);
    renderBattleDisplayList(&D_800D244C->primList[BSC_COLORTAG_IDX]);
}


/**
 * @brief Return whether SFX slot @p idx is active AND its SFX has pending state.
 *
 * Checks @c D_800C526C[idx].field00 — if the slot is inactive (-1) returns 0.
 * Otherwise queries @c getSfxField28 with the slot's SFX index and returns
 * 1 iff the result is nonzero, else 0.
 *
 * @param idx Index into the @c D_800C526C SFX slot table.
 * @return 1 if slot is active and its SFX is busy, else 0.
 */
s32 func_8009D9C8(s32 idx) {
    s32 field00 = D_800C526C[idx].field00;
    s32 field02 = D_800C526C[idx].field02;
    s32 result = 0;
    if (field00 != -1) {
        result = getSfxField28(field02) != 0;
    }
    return result;
}

/**
 * @brief Find the first active SFX slot in the high range [9, 13).
 *
 * Scans @c D_800C526C[9..12] looking for a slot whose @c field00 is
 * not -1 (active). Returns the matching slot index, or -1 if none.
 *
 * @return First active slot index in [9, 12], or -1 if all inactive.
 */
s32 func_8009DA10(void) {
    s32 i;
    for (i = 9; i < 13; i++) {
        if (D_800C526C[i].field00 != -1) {
            return i;
        }
    }
    return -1;
}


/**
 * @brief Linear-search the D_800C9880 key table for an entry with matching @p key.
 *
 * Walks the 12-byte records between the 4-byte length header and the
 * buffer's end, comparing each entry's @c key against (@p key & 0xFF).
 * Returns the matching Entry12 pointer on hit, or NULL if exhausted.
 *
 * @param key Lookup key (low 8 bits used).
 * @return Matching Entry12*, or NULL if no match.
 */
Entry12 *func_8009DA54(u8 key) {
    KeyBuffer *buf = D_800C9880;
    Entry12 *entry = &buf->entries[0];
    u8 *end = (u8 *)buf + buf->length;
    key &= 0xFF;
    if ((u8 *)entry < end) {
        do {
            if (key == entry->key) return entry;
            entry++;
        } while ((u8 *)entry < end);
    }
    return NULL;
}

/**
 * @brief Pick an audio-sequence transition code for a (map, command) pair.
 *
 * Compares @p cmd against the current audio-channel-0 state
 * (@c g_fieldVars->audioChannel0State) and classifies @p mapId against the
 * world-map "common" area set, returning a small transition selector:
 *  - @c 2 for map @c 0x31;
 *  - @c 3 / @c 4 for @p cmd @c 0x23 / @c 0x3B (unless it already matches the
 *    current state, which yields @c -1);
 *  - in the common area set: @c 0 unless the current state is @c 0x29;
 *  - @c 5 for map @c 0x32 when the current state is not @c 0x59.
 * Returns @c -1 for a no-op / unhandled case (including @p cmd @c == @c 0).
 *
 * @param mapId World-map area identifier.
 * @param cmd   Requested audio sequence/command code.
 * @return Transition selector (@c 0, @c 2..5), or @c -1 for no transition.
 * @note Exact transition-code semantics are inferred.
 */
s32 pickAudioTransition(s32 mapId, s32 cmd) {
    s32 cur  = g_fieldVars->audioChannel0State;
    s32 same = (cmd == cur);

    if (cmd == 0)      return -1;
    if (mapId == 0x31) return 2;
    if (cmd == 0x23) {
        if (same) return -1;
        return 3;
    }
    if (cmd == 0x3B) {
        if (same) return -1;
        return 4;
    }
    if ((u32)mapId < 0xA || mapId == 0x80 || mapId == 0x30 ||
        (u32)(mapId - 0x20) < 9 || mapId == 0x84 || (u32)(mapId - 0x10) < 7) {
        if (same) {
            if (cmd == 0x29) return -1;
        }
        if (cur != 0x29) return 0;
        return -1;
    }
    if (mapId == 0x32) {
        if (cur != 0x59) return 5;
    }
    return -1;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object2", func_8009DB88);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object2", func_8009E5C8);

/**
 * @brief Smooth-step a 12-bit angle (range @c [0,0x1000) = full revolution)
 *        toward @c target.
 *
 * Normalizes both @c target and @c *current into @c [0,0x1000), wraps
 * the signed delta into roughly @c [-0x800,0x801), and compares the
 * absolute delta against @c limit. If within limit, snaps @c *current
 * to @c target and returns 0 ("arrived"). Otherwise advances @c *current
 * by @c limit (or @c 2*limit if the remaining distance still exceeds
 * @c threshold) toward @c target and returns 1 ("still moving").
 *
 * @param target    desired angle (12-bit-revolution units).
 * @param current   pointer to the angle to step (s16, in/out).
 * @param limit     max single-step magnitude.
 * @param threshold large-step / small-step decision threshold.
 * @return          0 if snapped, 1 if still moving toward target.
 *
 * @note Decompiled to 99.77% in @c permuter/func_8009F594/base.c with a
 *       clean structure (no double-store / hoist hacks). Reusing the
 *       consumed wrapped-target register for @c limit, and reusing the
 *       wrapped-delta register for the magnitude, pins every register but
 *       one: the sole remaining diff is the @c (s16)delta narrow, which
 *       gcc emits in place (@c sra @c $v1) where the target casts into the
 *       freed @c $a2 then copies @c $a2->$v1 for the magnitude. The net
 *       register state past the abs is identical — a pure cast-destination
 *       reg-alloc tie-break (permuter territory).
 *
 * @verbatim
 * s32 func_8009F594(s16 target, s16 *current, s16 limit, s16 threshold) {
 *     s32 t = target;          // wrapped target; reused for limit once consumed
 *     s32 d = *current;        // wrapped current, then the wrapped delta
 *     s32 changed = 0;
 *
 *     while (t >= 0x1000) t -= 0x1000;
 *     while (t < 0)       t += 0x1000;
 *     while (d >= 0x1000) d -= 0x1000;
 *     while (d < 0)       d += 0x1000;
 *
 *     d = t - d;               // shortest-turn delta
 *     if (d <= -0x800) {
 *         while (d < 0)  d += 0x1000;
 *     } else if (d > 0x800) {
 *         while (d >= 0) d -= 0x1000;
 *     }
 *
 *     t = limit;               // reuse the (now-dead) wrapped-target reg for limit
 *     {
 *         s32 delta = (s16)d;  // signed delta
 *         s32 mag = delta;     // |delta|
 *         if (delta < 0) mag = -delta;
 *
 *         if (t < mag) {
 *             if (delta > 0) {
 *                 s32 c = *current;
 *                 if (threshold < mag) *current = c + t * 2;
 *                 else                 *current = c + t;
 *             } else {
 *                 s32 c = *current;
 *                 if (threshold < mag) *current = c - t * 2;
 *                 else                 *current = c - t;
 *             }
 *             changed = 1;
 *         } else {
 *             *current = target;
 *         }
 *     }
 *     return changed;
 * }
 * @endverbatim
 */
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object2", func_8009F594);

/* Map-id classifiers used to pick per-map targets. The three variants differ
   only in whether map 0x30 / 0x32 fall into the "common" set or are handled
   as dedicated cases by the caller. */
#define MAP_COMMON(m)      ((u32)((m) - 0x20) < 9 || (m) == 0x84 || \
                            (u32)((m) - 0x10) < 7 || (u32)((m) - 0x40) < 3)
#define MAP_COMMON_30(m)   ((u32)((m) - 0x20) < 9 || (m) == 0x84 || (m) == 0x30 || \
                            (u32)((m) - 0x10) < 7 || (u32)((m) - 0x40) < 3)
#define MAP_COMMON_3032(m) ((u32)((m) - 0x20) < 9 || (m) == 0x84 || (m) == 0x30 || \
                            (u32)((m) - 0x10) < 7 || (u32)((m) - 0x40) < 3 || (m) == 0x32)

/**
 * @brief World-map camera/projection state machine — one settle step per active flag bit.
 *
 * @c D_800C4D2C holds a bitmask of pending camera adjustments; if zero, returns
 * immediately. The two high bits act as presets: bit 31 reloads the mask with
 * @c 0xB7, bit 30 with @c 0x11. Each remaining low bit then advances one camera
 * value toward a map-id-dependent target by a fixed step, snapping and clearing
 * its own bit on arrival:
 *  - @c 0x1:  projection distance @c D_800C9730 (±8), then @c SetGeomScreen.
 *  - @c 0x2:  zoom/scale @c D_800C4D30 (±0x80).
 *  - @c 0x4:  set @c D_800C4D40 / @c D_800C4D44 from the map id (immediate, no step).
 *  - @c 0x8:  yaw @c D_800D2390.tail.angle — shortest-arc angle step (±0x20).
 *  - @c 0x10: pitch @c D_800D2390.tail.unk0 (±0x10); for map 0x32 the target is
 *             @c -D_800C97F4.word / 24 - 0x100.
 *  - @c 0x20: per-map scroll triple element [2] → @c D_800D2390.head.unk4 (±0x200).
 *  - @c 0x80: per-map scroll triple element [1] → @c D_800D2390.head.angle (±0x200).
 *
 * @c D_800C4D38 is the map id selecting targets; @c D_800C4D3C is a secondary
 * selector. Field roles are partly inferred.
 */
void func_8009F6EC(void) {
    s32 flags = D_800C4D2C;
    u16 sp10[4];
    u16 sp18[4];

    if (flags == 0) {
        return;
    }
    if (flags < 0) {
        D_800C4D2C = 0xB7;
    } else if (flags & 0x40000000) {
        D_800C4D2C = 0x11;
    }

    /* bit 0x1: step projection distance toward target, then SetGeomScreen. */
    if (D_800C4D2C & 0x1) {
        s32 mode = D_800C4D38;
        s32 d3c = D_800C4D3C;
        s32 target;
        s32 cur;
        s32 delta;
        if (MAP_COMMON_3032(mode) || d3c == 0) {
            target = 0x400;
        } else if (d3c == 1) {
            target = 0x280;
        }
        cur = D_800C9730;
        delta = cur - target;
        if (delta < -8) {
            D_800C9730 = cur + 8;
        } else if (delta >= 9) {
            D_800C9730 = cur - 8;
        } else {
            D_800C9730 = target;
            D_800C4D2C ^= 0x1;
        }
        SetGeomScreen(D_800C9730);
    }

    /* bit 0x2: step zoom/scale toward target. */
    if (D_800C4D2C & 0x2) {
        s32 target;
        s32 cur;
        s32 delta;
        target = (D_800C4D38 == 0x32) ? 0x1800 : 0x1460;
        cur = D_800C4D30;
        delta = cur - target;
        if (delta < -0x80) {
            D_800C4D30 = cur + 0x80;
        } else if (delta >= 0x81) {
            D_800C4D30 = cur - 0x80;
        } else {
            D_800C4D30 = target;
            D_800C4D2C ^= 0x2;
        }
    }

    /* bit 0x4: set D_800C4D40 / D_800C4D44 from the map id (immediate). */
    if (D_800C4D2C & 0x4) {
        D_800C4D40 = 0;
        if ((u32)D_800C4D38 < 0xA || D_800C4D38 == 0x80) {
            D_800C4D44 = 0x10;
        }
        {
            s32 mode = D_800C4D38;
            if ((u32)(mode - 0x20) < 9 || mode == 0x84) {
                D_800C4D44 = 0x20;
            } else if (mode == 0x32) {
                D_800C4D44 = 0x78;
            } else if (mode == 0x31) {
                D_800C4D44 = 0x78;
            } else if (mode == 0x30) {
                D_800C4D44 = 0x20;
            } else {
                D_800C4D44 = 0x78;
            }
        }
        D_800C4D2C ^= 0x4;
    }

    /* bit 0x8: shortest-arc yaw step. */
    if (D_800C4D2C & 0x8) {
        u16 target;
        s32 raw;
        s32 d;
        u16 cur;
        if (D_800C4D3C == 1) {
            target = D_800C5346;
        } else {
            target = D_800D239A;
        }
        cur = D_800D2390.tail.angle;
        raw = target - cur;
        d = (s16)raw;
        if ((d < 0 ? -d : d) >= 0x21) {
            if ((u16)(raw - 1) < 0x7FF || d < -0x7FF) {
                D_800D2390.tail.angle = cur + 0x20;
            } else {
                D_800D2390.tail.angle = cur - 0x20;
            }
        } else {
            D_800D2390.tail.angle = target;
            D_800C4D2C ^= 0x8;
        }
    }

    /* bit 0x10: step pitch toward target. */
    if (D_800C4D2C & 0x10) {
        s32 mode = D_800C4D38;
        s32 d3c = D_800C4D3C;
        s32 t = D_800C97F4.word;
        s32 target;
        u16 cur;
        s32 d;
        if (MAP_COMMON_30(mode)) {
            target = D_800C534C;
        } else if (mode == 0x32) {
            target = (-t) / 24 - 0x100;
        } else if (d3c == 0) {
            target = D_800C534C;
        } else if (d3c == 1) {
            target = D_800C5344;
        }
        cur = D_800D2390.tail.unk0;
        d = (s16)(cur - target);
        if (d < -0x10) {
            D_800D2390.tail.unk0 = cur + 0x10;
        } else if (d >= 0x11) {
            D_800D2390.tail.unk0 = cur - 0x10;
        } else {
            D_800D2390.tail.unk0 = target;
            D_800C4D2C ^= 0x10;
        }
        if ((u32)D_800C4D38 < 0xA || D_800C4D38 == 0x80) {
            if (D_800C4D3C == 0 && D_800D2448 != 0) {
                D_800C4D2C ^= 0x10;
            }
        }
    }

    /* bit 0x20: copy per-map scroll triple, step element [2] toward target. */
    if (D_800C4D2C & 0x20) {
        u16 *buf = sp10;
        s32 mode = D_800C4D38;
        u16 cur;
        s32 d;
        u16 target;
        if (MAP_COMMON(mode)) {
            buf[0] = D_800C5354[0];
            buf[1] = D_800C5354[1];
            buf[2] = D_800C5354[2];
        } else if (mode == 0x30) {
            sp10[0] = D_800C5364[0];
            buf[1] = D_800C5364[1];
            buf[2] = D_800C5364[2];
        } else if (mode == 0x32) {
            sp10[0] = D_800C535C[0];
            buf[1] = D_800C535C[1];
            buf[2] = D_800C535C[2];
        } else {
            sp10[0] = D_800C5354[0];
            buf[1] = D_800C5354[1];
            buf[2] = D_800C5354[2];
        }
        target = sp10[2];
        cur = D_800D2390.head.unk4;
        d = (s16)(cur - target);
        if (d < -0x200) {
            D_800D2390.head.unk4 = cur + 0x200;
        } else if (d >= 0x201) {
            D_800D2390.head.unk4 = cur - 0x200;
        } else {
            D_800D2390.head.unk4 = target;
            D_800C4D2C ^= 0x20;
        }
    }

    /* bit 0x80: copy per-map scroll triple, step element [1] toward target. */
    if (D_800C4D2C & 0x80) {
        s32 mode = D_800C4D38;
        u16 cur;
        s32 d;
        u16 target;
        if (MAP_COMMON(mode)) {
            sp18[0] = D_800C5354[0];
            sp18[1] = D_800C5354[1];
            sp18[2] = D_800C5354[2];
        } else if (mode == 0x30) {
            sp18[0] = D_800C5364[0];
            sp18[1] = D_800C5364[1];
            sp18[2] = D_800C5364[2];
        } else if (mode == 0x32) {
            sp18[0] = D_800C535C[0];
            sp18[1] = D_800C535C[1];
            sp18[2] = D_800C535C[2];
        } else {
            sp18[0] = D_800C5354[0];
            sp18[1] = D_800C5354[1];
            sp18[2] = D_800C5354[2];
        }
        target = sp18[1];
        cur = D_800D2390.head.angle;
        d = (s16)(cur - target);
        if (d < -0x200) {
            D_800D2390.head.angle = cur + 0x200;
        } else if (d >= 0x201) {
            D_800D2390.head.angle = cur - 0x200;
        } else {
            D_800D2390.head.angle = target;
            D_800C4D2C ^= 0x80;
        }
    }
}

#undef MAP_COMMON
#undef MAP_COMMON_30
#undef MAP_COMMON_3032

/* Per-map view helpers, defined later in this file. */
s32  func_8009FF0C(s32 mapId, s32 mode);
s32  func_800A009C(s32 mapId, s32 mode);
void func_8009FF70(s32 mapId, s32 mode, u16 *out);
s32  func_800A0000(s32 mapId, s32 mode, s32 scroll);

/**
 * @brief Set up the world-map camera projection and per-map view parameters.
 *
 * Converts the camera position @p pos into GTE space (swap Y/Z and negate the
 * new Z for the PS1 y-down convention) and projects it into @p proj via
 * @c func_800A40F8. Then, keyed by the current map id @c D_800C4D38 and mode
 * @c D_800C4D3C, it latches the projection-plane distance (@c D_800C9730), a
 * zoom constant (@c D_800C4D30), copies a per-map parameter triple into
 * @p view, derives a scroll offset from the projected Y, clears the two view
 * flags, and finally programs the GTE screen distance via @c SetGeomScreen.
 *
 * @param pos  Camera/source position (vx,vy,vz).
 * @param proj Receives the projected screen position; its @c [0x2] feeds the
 *             scroll computation. Flag/scroll fields are cleared here.
 * @param view Receives the per-map parameter triple plus the scroll offset.
 */
void setupWorldMapView(VECTOR *pos, WorldView *proj, WorldView *view) {
    VECTOR vec;

    vec.vx = pos->vx;
    vec.vy = pos->vz;
    vec.vz = -pos->vy;
    func_800A40F8(&vec, (VECTOR *)proj);

    D_800C9730 = func_8009FF0C(D_800C4D38, D_800C4D3C);
    D_800C4D30 = func_800A009C(D_800C4D38, D_800C4D3C);
    func_8009FF70(D_800C4D38, D_800C4D3C, (u16 *)view);
    view->scroll = func_800A0000(D_800C4D38, D_800C4D3C, proj->unk02);
    view->flag   = 0;
    proj->flag   = 0;
    proj->scroll = 0;
    SetGeomScreen(D_800C9730);
}

/**
 * @brief Build a rotation matrix from angles, multiply by another, and set
 *        as the GTE light matrix.
 *
 * Three-step transform pipeline:
 *  1. @c RotMatrix(angles, &m) — fill @c m with a rotation derived from
 *     the @c SVECTOR pointed to by @p a0.
 *  2. @c MulMatrix(&m, a1) — multiply @c m by the matrix at @p a1.
 *  3. @c SetLightMatrix(&m) — load @c m as the GTE LLM (light matrix).
 *
 * @param a0 Input @c SVECTOR with rotation angles.
 * @param a1 Right-hand multiplier matrix.
 */
void func_8009FE80(SVECTOR *a0, MATRIX *a1) {
    MATRIX m;
    RotMatrix(a0, &m);
    MulMatrix(&m, a1);
    SetLightMatrix(&m);
}


/**
 * @brief Forward @p vec to func_800A017C, ignoring the first arg register.
 *
 * Thin wrapper — takes two register args but only passes the second on.
 *
 * @param unused First arg register, ignored.
 * @param vec Pointer forwarded as the first arg of func_800A017C.
 */
void func_8009FEBC(s32 unused, SVECTOR *vec) {
    func_800A017C(vec);
}

/** @brief Clear a 15-byte animation structure and set type byte at offset 0xA. */
void func_8009FEDC(u8 *a0, u8 a1) {
    a0[0xE] = 0;
    a0[0xD] = 0;
    a0[0xC] = 0;
    a0[0xB] = 0;
    a0[0xA] = 0;
    a0[0x9] = 0;
    a0[0x8] = 0;
    *(u16 *)(a0 + 0x0) = 0;
    *(u16 *)(a0 + 0x2) = 0;
    *(u16 *)(a0 + 0x4) = 0;
    a0[0xA] = a1;
}

/**
 * @brief Pick an audio/parameter constant from a map-ID + mode pair.
 *
 * Special-cased @p mapId values return @c 0x400. The set is the union of
 * the ranges @c [0x20, 0x29), @c [0x10, 0x17), @c [0x40, 0x43) and the
 * singletons @c 0x30, @c 0x32, @c 0x84. For non-special @p mapId values,
 * the result is selected by @p mode:
 *  - @p mode == 0: @c 0x400
 *  - @p mode == 1: @c 0x280
 *  - otherwise:    @c 1
 *
 * The exact semantics are uncertain; the constants look like sound /
 * fade-time parameters keyed off the world-map area.
 *
 * @param mapId World-map area identifier.
 * @param mode  Mode selector consulted only on non-special @p mapId.
 * @return Selected constant — @c 0x400, @c 0x280, or @c 1.
 */
s32 func_8009FF0C(s32 mapId, s32 mode) {
    if ((u32)(mapId - 0x20) < 9 ||
        mapId == 0x84 ||
        mapId == 0x30 ||
        (u32)(mapId - 0x10) < 7 ||
        (u32)(mapId - 0x40) < 3 ||
        mapId == 0x32 ||
        mode == 0) {
        return 0x400;
    }
    return mode == 1 ? 0x280 : 1;
}


/**
 * @brief Copy a per-map 3-halfword parameter triple into @p out, keyed by
 *        @p mapId. Sibling of @c func_8009FF0C — same map-ID classifier,
 *        different output payload.
 *
 * The default source is @c D_800C5354. Two single-map cases pick alternate
 * sources: @c mapId @c == @c 0x30 → @c D_800C5364, @c mapId @c == @c 0x32 →
 * @c D_800C535C. All other map IDs in the "common" set (the ranges
 * @c [0x20, 0x29), @c [0x10, 0x17), @c [0x40, 0x43) and @c mapId @c == @c 0x84)
 * also use the default. The triple is three halfwords (likely RGB or a small
 * fade/colour vector — exact meaning uncertain).
 *
 * @param mapId  World-map area identifier.
 * @param mode   Unused at this call site (kept for ABI; sibling reads it).
 * @param out    Destination halfword triple — receives @c src[0..2].
 */
void func_8009FF70(s32 mapId, s32 mode, u16 *out) {
    u16 *src;
    int new_var;

    if ((u32)(mapId - 0x20) < 9 || mapId == 0x84
        || (u32)(mapId - 0x10) < 7 || (u32)(mapId - 0x40) < 3) {
        src = D_800C5354;
        out[0] = src[0];
    } else if (mapId == 0x30) {
        src = D_800C5364;
        new_var = src[0];
        out[0] = new_var;
    } else if (mapId == 0x32) {
        src = D_800C535C;
        out[0] = src[0];
    } else {
        src = D_800C5354;
        out[0] = src[0];
    }
    new_var = src[1];
    out[1] = new_var;
    out[2] = src[2];
}


/**
 * @brief Pick a per-map screen-scroll offset, with special handling for
 *        @c mapId @c == @c 0x32 and a @p mode-keyed fallback.
 *
 * Returns @c D_800C534C for the "common" map set:
 *  - @c mapId in @c [0x20, @c 0x29),
 *  - @c mapId @c == @c 0x84,
 *  - @c mapId @c == @c 0x30,
 *  - @c mapId in @c [0x10, @c 0x17),
 *  - @c mapId in @c [0x40, @c 0x43).
 *
 * For @c mapId @c == @c 0x32 returns @c -scroll @c / @c 24 @c - @c 0x100
 * — a scroll-based offset computed from @p scroll.
 *
 * Otherwise selects on @p mode:
 *  - @c mode @c == @c 0: @c D_800C534C.
 *  - @c mode @c == @c 1: @c D_800C5344.
 *  - else:               @c 1.
 *
 * @param mapId  World-map area identifier.
 * @param mode   Mode selector consulted only on non-special @p mapId.
 * @param scroll Scroll value, only used for @p mapId @c == @c 0x32.
 * @return Selected scroll offset.
 */
s32 func_800A0000(s32 mapId, s32 mode, s32 scroll) {
    if ((u32)(mapId - 0x20) < 9 ||
        mapId == 0x84 ||
        mapId == 0x30 ||
        (u32)(mapId - 0x10) < 7 ||
        (u32)(mapId - 0x40) < 3) {
        return D_800C534C;
    }
    if (mapId == 0x32) {
        return -scroll / 24 - 0x100;
    }
    if (mode == 0) return D_800C534C;
    if (mode == 1) return D_800C5344;
    return 1;
}

/**
 * @brief Per-map zoom/scale constant: @c 0x1800 for map @c 0x32, else @c 0x1460.
 *
 * @param mapId World-map area identifier.
 * @param mode  Unused — kept for the uniform @c (mapId,mode) map-helper ABI;
 *              @c setupWorldMapView passes it the same argument pair as the sibling
 *              helpers (the value is ignored here).
 * @return Selected zoom constant.
 */
s32 func_800A009C(s32 mapId, s32 mode) {
    if (mapId == 0x32) {
        return 0x1800;
    }
    return 0x1460;
}

/**
 * @brief Shortest signed angular difference between two angles.
 *
 * Both inputs are first wrapped into @c [0,0x1000) (where @c 0x1000 is one
 * full revolution), then the difference @c a-b is wrapped into the
 * half-revolution window @c [-0x7FF,0x800] so the result is the shortest
 * turn — in either direction — from @p b to @p a.
 *
 * @param a Target angle (@c 0x1000 == 360°).
 * @param b Reference angle.
 * @return Signed angular delta in @c [-0x7FF,0x800].
 */
s32 getAngleDelta(s32 a, s32 b) {
    while (a >= 0x1000) a -= 0x1000;
    while (a < 0)       a += 0x1000;
    while (b >= 0x1000) b -= 0x1000;
    while (b < 0)       b += 0x1000;

    a -= b;
    if (a <= -0x800) {
        while (a < 0)  a += 0x1000;
    } else if (a > 0x800) {
        while (a >= 0)  a -= 0x1000;
    }
    return a;
}

/**
 * @brief Wrap @c vx into @c [0, 0x7FF] and @c vz into @c [-0x7FF, 0] by ±0x800.
 *
 * Used by the world map system to keep coordinates inside the active tile —
 * when a coordinate spills past the tile boundary, it is shifted by one
 * tile (0x800) so subsequent computations stay in-range.
 *
 *  - @c vx (offset 0): clamp to @c [0, 0x7FF]. If @c vx >= 0x800 subtract
 *    0x800; if @c vx < 0 add 0x800.
 *  - @c vz (offset 4): clamp to @c [-0x7FF, 0]. If @c vz > 0 subtract
 *    0x800; if @c vz <= -0x800 add 0x800.
 *
 * @c vy (offset 2) and @c pad (offset 6) are left alone.
 *
 * @param v Target vector.
 * @return @c 1 if either component was shifted, else @c 0.
 */
s32 func_800A017C(SVECTOR *v) {
    s32 changed = 0;
    if (v->vx >= 0x800) {
        v->vx -= 0x800;
        changed = 1;
    } else if (v->vx < 0) {
        v->vx += 0x800;
        changed = 1;
    }
    if (v->vz > 0) {
        v->vz -= 0x800;
        changed = 1;
    } else if (v->vz <= -0x800) {
        v->vz += 0x800;
        changed = 1;
    }
    return changed;
}
