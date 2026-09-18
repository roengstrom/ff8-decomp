#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "battle.h"
#include "sound.h"
#include "field/fe_object1.h"
#include "field/fe_object9.h"

// Intentionally NOT included — these headers would pull in void prototypes
// for setSfxEntryVolume, setSfxEntityType, updateAnimEntry, setupAnimEntry,
// and setupAnimEntryFull. We need gcc to implicit-int-declare those at the
// call sites below to match the original K&R-style scheduling. Without that,
// four functions mismatch.
// #include "btl_entity.h"
// #include "btl_sfx.h"
// #include "btl_color.h"

/**
 * @brief Snapshot a target entity's grid-cell position into the queued
 *        turn-state fields.
 *
 * Same body as @c opHandler_FACEDIRA in fe_object8 but standalone — no
 * @c opHandler_FACEDIRSYNC tail call, always returns 2.
 */
s32 opHandler_RFACEDIRA(Actor *actor) {
    s16 buf[4];
    s32 idx;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        idx = POP(&actor->context);
        func_800A8DAC(D_80085230[idx]->field_0x256, 0x1E, (u32)g_curFieldView, buf);
        actor->turnTgtX = D_80085230[idx]->posX / 4096;
        actor->turnTgtY = D_80085230[idx]->posY / 4096;
        actor->turnTgtZ = buf[2] + D_80085230[idx]->posZ / 4096;
        actor->turnMode = 1;
        actor->turnTick = 0;
    }
    return 2;
}

/**
 * @brief Variant of @c opHandler_RFACEDIRA that resolves the target via the
 *        SeeD party-member slot table.
 *
 * Same body as @c opHandler_RFACEDIRA but the popped index is treated as a
 * SeeD party slot — look up @c g_fieldVars->memberSlot[slot] to get
 * the entity index into @c D_80085224, and pass that same byte as the
 * spatial argument to @c func_800A8DAC.
 */
s32 opHandler_RFACEDIRP(Actor *actor) {
    s16 buf[4];
    u8 slot;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        slot = g_fieldVars->memberSlot[POP(&actor->context)];
        func_800A8DAC(slot, 0x1E, (u32)g_curFieldView, buf);
        actor->turnTgtX = D_80085224[slot].posX / 4096;
        actor->turnTgtY = D_80085224[slot].posY / 4096;
        actor->turnTgtZ = buf[2] + D_80085224[slot].posZ / 4096;
        actor->turnMode = 1;
        actor->turnTick = 0;
    }
    return 2;
}

/**
 * @brief Queue a relative-offset turn target (standalone variant).
 *
 * Same body as @c opHandler_FACEDIROFF in fe_object8 but doesn't tail-call
 * @c opHandler_FACEDIRSYNC — always returns 2. Queries @c func_800A8DAC with
 * kind @c 0x20 (relative offsets, written to @c buf), divides each
 * entry by 16, and stores into @c turnPitchDst / @c turnRollDst /
 * @c turnYawDst. Clears @c turnTick and @c turnMode.
 */
s32 opHandler_RFACEDIROFF(Actor *actor) {
    s16 buf[4];

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        ((void (*)(u8, s32, void *, void *))func_800A8DAC)(actor->field_0x256, 0x20, buf, 0);
        actor->turnPitchDst = buf[0] / 16;
        actor->turnRollDst = buf[1] / 16;
        actor->turnYawDst = buf[2] / 16;
        actor->turnTick = 0;
        actor->turnMode = 0;
    }
    return 2;
}

/**
 * @brief Pop three bytes into @c turnPitchRate / @c field_0x239 /
 *        @c turnYawRate (top-of-stack first).
 */
s32 opHandler_FACEDIRLIMIT(Actor *actor) {
    actor->turnYawRate = POP_BYTE(&actor->context);
    actor->field_0x239 = POP_BYTE(&actor->context);
    actor->turnPitchRate = POP_BYTE(&actor->context);
    return 2;
}

/**
 * @brief Query relative offset (kind 0x20) and split into entity halfwords.
 *
 * Calls @c func_800A8DAC with mode @c 0x20 to fetch three relative-offset
 * halfwords for the active spatial entity. Stores @c buf[1]/16 into
 * @c turnRollCur, @c buf[2]/16 into @c turnYawCur, and clears
 * @c turnPitchCur. (@c buf[0] is queried but discarded.)
 */
s32 opHandler_FACEDIRINIT(Actor *actor) {
    s16 buf[4];

    func_800A8DAC(actor->field_0x256, 0x20, (u8 *)buf, 0);
    actor->turnPitchCur = 0;
    actor->turnRollCur = buf[1] / 16;
    actor->turnYawCur = buf[2] / 16;
    return 2;
}

/**
 * @brief Snapshot the 12-halfword dialog-state block from @c D_800704A8
 *        into @c g_fieldVars.
 *
 * Copies @c D_800704A8.dialogState..field_0x11E (12 halfwords) into
 * @c g_fieldVars->dialogStateMirror..fieldEE. The first four writes
 * are emitted out of source order (dst@D8, DC, DA, DE) for codegen
 * matching; the remaining eight are sequential.
 */
void func_800BB6C8(void) {
    FieldVars   *dst = g_fieldVars;
    SystemState *src = &D_800704A8;

    dst->dialogStateMirror = src->dialogState;
    dst->fieldDC           = src->dialogCount;
    dst->fieldDA           = src->dialogTimer;
    dst->fieldDE           = src->field_0x10E;
    dst->fieldE0           = src->field_0x110;
    dst->fieldE2           = src->field_0x112;
    dst->fieldE4           = src->field_0x114;
    dst->fieldE6           = src->field_0x116;
    dst->fieldE8           = src->field_0x118;
    dst->fieldEA           = src->field_0x11A;
    dst->fieldEC           = src->field_0x11C;
    dst->fieldEE           = src->field_0x11E;
}

/**
 * @brief Initialise the dialog-state block and mirror it to @c g_fieldVars.
 *
 * Sets the global mode marker @c D_800DE8D2 to @c 2 and seeds the
 * 6-halfword dialog state at @c D_800704A8.dialogState..field_0x112
 * with the boot values (state=2, timer=0xFF, count=0x10, then 0xFF
 * sentinels). @c func_800BB6C8 copies the whole 12-halfword block
 * into @c g_fieldVars->dialogStateMirror..fieldEE.
 */
s32 opHandler_FADEIN(void) {
    D_800DE8D2 = 2;
    D_800704A8.dialogState  = 2;
    D_800704A8.dialogTimer  = 0xFF;
    D_800704A8.dialogCount  = 0x10;
    D_800704A8.field_0x10E  = 0xFF;
    D_800704A8.field_0x110  = 0xFF;
    D_800704A8.field_0x112  = 0xFF;
    func_800BB6C8();
    return 2;
}

/**
 * @brief Initialise the dialog-state block (variant: state=3, count=8).
 *
 * Similar to @c opHandler_FADEIN but with the boot values
 * (D_800DE8D2=3, dialogState=3, dialogTimer=0, dialogCount=8, then
 * three 0xFF sentinels). The empty @c do/while(0) breaks gcc's
 * scheduler at the right point so the @c sh @c zero (dialogTimer)
 * lands before the @c sh @c v1 (dialogCount) — without it gcc emits
 * the count store first and burns an extra @c li.
 */
s32 opHandler_FADEOUT(void) {
    D_800DE8D2 = 3;
    D_800704A8.dialogState = 3;
    D_800704A8.dialogTimer = 0;
    D_800704A8.dialogCount = 8;
    do { } while (0);
    D_800704A8.field_0x10E = 0xFF;
    D_800704A8.field_0x110 = 0xFF;
    D_800704A8.field_0x112 = 0xFF;
    func_800BB6C8();
    return 2;
}

/**
 * @brief Initialise dialog state (variant 7) and pop three halfwords.
 *
 * Sets @c dialogState=7 / @c dialogTimer=0, then pops three halfwords
 * top-down into @c field_0x112 / @c field_0x110 / @c field_0x10E
 * (so the first push lands in @c field_0x10E). Then dispatches
 * @c func_800BB6C8 to mirror the block into @c g_fieldVars.
 *
 * @note Originally split by splat at @c func_800BB888 (the tail of
 * this function) — the symbol was removed from @c symbol_addrs.field
 * so the two halves merge back into one function.
 */
s32 opHandler_DCOLADD(ScriptContext *context) {
    D_800704A8.dialogState = 7;
    D_800704A8.dialogTimer = 0;
    D_800704A8.field_0x112 = POP(context);
    D_800704A8.field_0x110 = POP(context);
    D_800704A8.field_0x10E = POP(context);
    func_800BB6C8();
    return 2;
}


/**
 * @brief Initialise dialog state (variant 8) and pop three halfwords.
 *
 * Same as @c opHandler_DCOLADD but with @c dialogState=8.
 *
 * @note Originally split by splat at @c func_800BB90C — symbol removed
 * from @c symbol_addrs.field so the two halves merge back.
 */
s32 opHandler_DCOLSUB(ScriptContext *context) {
    D_800704A8.dialogState = 8;
    D_800704A8.dialogTimer = 0;
    D_800704A8.field_0x112 = POP(context);
    D_800704A8.field_0x110 = POP(context);
    D_800704A8.field_0x10E = POP(context);
    func_800BB6C8();
    return 2;
}


/**
 * @brief Initialise dialog state 5 + pop 4 halfwords + copy 3.
 *
 * Sets @c dialogState=5, @c dialogTimer=0. Pops 4 halfwords into
 * @c dialogCount / @c field_0x11E / @c field_0x11C / @c field_0x11A
 * (in that order), then copies @c field_0x112 / @c field_0x110 /
 * @c field_0x10E into @c field_0x118 / @c field_0x116 / @c field_0x114
 * (snapshotting the previous values). Dispatches @c func_800BB6C8.
 *
 * @note Originally split by splat at @c func_800BB9A8 — symbol removed
 * from @c symbol_addrs.field so the two halves merge back.
 */
s32 opHandler_TCOLADD(ScriptContext *context) {
    D_800704A8.dialogState = 5;
    D_800704A8.dialogTimer = 0;
    D_800704A8.dialogCount = POP(context);
    D_800704A8.field_0x11E = POP(context);
    D_800704A8.field_0x11C = POP(context);
    D_800704A8.field_0x11A = POP(context);
    D_800704A8.field_0x118 = D_800704A8.field_0x112;
    D_800704A8.field_0x116 = D_800704A8.field_0x110;
    D_800704A8.field_0x114 = D_800704A8.field_0x10E;
    func_800BB6C8();
    return 2;
}


/**
 * @brief Same shape as @c opHandler_TCOLADD with @c dialogState=6.
 */
s32 opHandler_TCOLSUB(ScriptContext *context) {
    D_800704A8.dialogState = 6;
    D_800704A8.dialogTimer = 0;
    D_800704A8.dialogCount = POP(context);
    D_800704A8.field_0x11E = POP(context);
    D_800704A8.field_0x11C = POP(context);
    D_800704A8.field_0x11A = POP(context);
    D_800704A8.field_0x118 = D_800704A8.field_0x112;
    D_800704A8.field_0x116 = D_800704A8.field_0x110;
    D_800704A8.field_0x114 = D_800704A8.field_0x10E;
    func_800BB6C8();
    return 2;
}


/**
 * @brief Initialise dialog state 5 + pop 7 halfwords.
 *
 * Like @c opHandler_TCOLADD but pops three more halfwords into
 * @c field_0x118 / @c field_0x116 / @c field_0x114 instead of
 * copying from the lower triple.
 *
 * @note Originally split by splat into three parts (BBB20, BBBB4,
 * BBC08); the trailing symbols were removed from
 * @c symbol_addrs.field to merge them back.
 */
s32 opHandler_FCOLADD(ScriptContext *context) {
    D_800704A8.dialogState = 5;
    D_800704A8.dialogTimer = 0;
    D_800704A8.dialogCount = POP(context);
    D_800704A8.field_0x11E = POP(context);
    D_800704A8.field_0x11C = POP(context);
    D_800704A8.field_0x11A = POP(context);
    D_800704A8.field_0x118 = POP(context);
    D_800704A8.field_0x116 = POP(context);
    D_800704A8.field_0x114 = POP(context);
    func_800BB6C8();
    return 2;
}



/**
 * @brief Same shape as @c opHandler_FCOLADD with @c dialogState=6.
 */
s32 opHandler_FCOLSUB(ScriptContext *context) {
    D_800704A8.dialogState = 6;
    D_800704A8.dialogTimer = 0;
    D_800704A8.dialogCount = POP(context);
    D_800704A8.field_0x11E = POP(context);
    D_800704A8.field_0x11C = POP(context);
    D_800704A8.field_0x11A = POP(context);
    D_800704A8.field_0x118 = POP(context);
    D_800704A8.field_0x116 = POP(context);
    D_800704A8.field_0x114 = POP(context);
    func_800BB6C8();
    return 2;
}

/**
 * @brief Wait for the dialog-state countdown to match the timer.
 *
 * Returns @c 1 while @c D_800704A8+0x10C (countdown) and @c +0x10A
 * (target/timer) differ. Once they match, copy the current dialog
 * state word at @c +0x108 into @c g_fieldVars->dialogStateMirror and return @c 2.
 */
s32 opHandler_COLSYNC(void) {
    if (D_800704A8.dialogCount != D_800704A8.dialogTimer) {
        return 1;
    }
    g_fieldVars->dialogStateMirror = D_800704A8.dialogState;
    return 2;
}

/**
 * @brief Dialog ready/idle predicate dispatched on @c D_800DE8D2.
 *
 * When @c D_800DE8D2 == 2 the function reports 2 only if
 * @c D_800704A8.dialogState is currently 0 (idle), otherwise 1.
 * For every other value of @c D_800DE8D2 the function waits for
 * @c dialogState == 3 AND @c dialogTimer == 0xFF before reporting 2.
 *
 * The @c dlg=1; return dlg pattern in the first arm forces gcc to
 * reuse @c v0 for both the loaded dialog state and the return value,
 * matching the target's register allocation.
 *
 * @return 2 when the polled state is ready, 1 while still waiting.
 */
s32 opHandler_FADESYNC(void) {
    u16 dlg;
    volatile SystemState *ss;
    if (D_800DE8D2 == 2) {
        dlg = ((volatile SystemState *)&D_800704A8)->dialogState;
        if (dlg != 0) {
            dlg = 1;
            return dlg;
        }
        return 2;
    }
    ss = &D_800704A8;
    if ((s16)ss->dialogState != 3) {
        return 1;
    }
    if ((s16)ss->dialogTimer == 0xFF) {
        return 2;
    }
    return 1;
}

/**
 * @brief Clear the dialog state and mirror it into @c g_fieldVars.
 *
 * Writes 0 into @c D_800704A8.dialogState, then copies the freshly
 * cleared value into @c g_fieldVars->dialogStateMirror. The @c volatile
 * pointer is required to force a real load-after-store rather than
 * letting gcc fold the mirror to a constant 0.
 *
 * @return Always 2 (VM continue).
 */
s32 opHandler_FADENONE(void) {
    volatile SystemState *src = &D_800704A8;
    FieldVars *dst = g_fieldVars;
    src->dialogState = 0;
    dst->dialogStateMirror = src->dialogState;
    return 2;
}

/**
 * @brief Force the dialog state to @c 4 and mirror it into @c g_fieldVars->dialogStateMirror.
 *
 * The @c volatile cast is required for matching codegen: the asm reads
 * the just-written value back through memory rather than reusing the
 * literal @c 4.
 */
s32 opHandler_FADEBLACK(void) {
    SystemState *src = &D_800704A8;
    *(volatile u16 *)&src->dialogState = 4;
    g_fieldVars->dialogStateMirror = *(volatile u16 *)&src->dialogState;
    return 2;
}

/**
 * @brief Pop two stack slots and dispatch @c func_8002E1B4 with them.
 *
 * Pops two values from the script stack and calls
 * @c func_8002E1B4(val2 & 7, val1) — val1 is the top slot, val2 the
 * next. The @c & @c 7 mask suggests @c val2 is a 3-bit selector
 * (e.g. SFX channel id).
 */
s32 opHandler_MESVAR(ScriptContext *context) {
    s32 val1 = POP(context);
    s32 val2 = POP(context);
    func_8002E1B4(val2 & 7, val1);
    return 2;
}

/**
 * @brief Register an SFX entry's volume and type via VM stack args.
 *
 * Pops three values from the Actor stack: @c vol (top), @c kind, then
 * @c idx. Forwards @c vol to @c setSfxEntryVolume and remaps the
 * caller-supplied @c kind through a 3-way switch (0→2, 1→3, 2→0) before
 * calling @c setSfxEntityType. Mirrors both into the per-slot SFX entry
 * at @c D_80085300[idx]. Returns 2 (VM continue).
 */
s32 opHandler_MESMODE(ScriptContext *context) {
    s32 vol  = POP(context);
    s32 kind = POP(context);
    s32 idx  = POP(context);

    setSfxEntryVolume(idx, vol);

    switch (kind) {
    case 0: kind = 2; break;
    case 1: kind = 3; break;
    case 2: kind = 0; break;
    }

    setSfxEntityType(idx, kind);
    D_80085300[idx].volume = vol;
    D_80085300[idx].type   = kind;
    return 2;
}

/**
 * @brief Peek top two stack slots and pass them to @c setSfxPitch.
 *
 * Reads @c stack[ptr-1] and @c stack[ptr] without decrementing
 * @c stackPtr, then calls @c setSfxPitch(stack[ptr-1], stack[ptr]).
 */
s32 opHandler_SETMESSPEED(ScriptContext *context) {
    s8 idx = (s8)context->stackPtr;
    setSfxPitch(context->stack[idx - 1], context->stack[idx]);
    return 2;
}

/**
 * @brief Start an SFX slot if it's not already running on this script.
 *
 * Peeks two stack values: @c val1 (top, the SFX data index) and
 * @c sfxIdx (one below, the playback slot).
 *
 * If the Actor's @c activeMask bit for the current @c scriptSlot is
 * set: returns 5 immediately when the slot bit is already in
 * @c sfxStartMask; otherwise looks up the SFX data via
 * @c getOffsetTableEntry(g_curFieldMessages, val1), kicks off playback
 * (@c initSfxPlayback / @c startSfxSlow), promotes the global flag
 * and marks the slot bit in @c sfxStartMask. Returns 1.
 *
 * If the Actor's @c activeMask bit is NOT set: returns 1 when the
 * slot is already running, otherwise pops two stack slots and returns
 * 3 to fall through to the next opcode.
 *
 * The @c do { } while (0) wrapper around the mask read-modify-write is
 * a scheduling barrier — it keeps gcc from interleaving the
 * @c (1 << sfxIdx) constant load with the @c lw of @c g_fieldVars,
 * matching the target's register allocation.
 */
s32 opHandler_MESW(ScriptContext *context) {
    s32 val1   = context->stack[(s8)context->stackPtr];
    s32 sfxIdx = context->stack[(s8)context->stackPtr - 1];

    if ((context->activeMask >> context->scriptSlot) & 1) {
        u8 mask;
        if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
            return 5;
        }
        initSfxPlayback(sfxIdx, getOffsetTableEntry(g_curFieldMessages, val1));
        startSfxSlow(sfxIdx);
        setSfxGlobalFlag(sfxIdx);
        do {
            mask = g_fieldVars->sfxStartMask;
            g_fieldVars->sfxStartMask = mask | (1 << sfxIdx);
        } while (0);
        return 1;
    }
    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 1;
    }
    context->stackPtr -= 2;
    return 3;
}

/**
 * @brief Write one entry into the @c D_80085300 SFX-entry table.
 *
 * Stores the s32 @c val into @c entry->payload and copies the four
 * input halfwords from @c src into @c entry->rect. The trailing
 * @c volume / @c type fields are left untouched.
 *
 * @param idx  Entry index in @c D_80085300.
 * @param val  s32 payload (typically an SFX data pointer cast to s32).
 * @param src  4-halfword source block, copied into @c entry->rect.
 */
void func_800BC12C(s32 idx, s32 val, u16 *src) {
    FieldSfxSlot *base = D_80085300;
    FieldSfxSlot *entry = base + idx;
    entry->payload = val;
    entry->rect[0] = src[0];
    entry->rect[1] = src[1];
    entry->rect[2] = src[2];
    entry->rect[3] = src[3];
}

/**
 * @brief SFX trigger with sound-data lookup and entry registration.
 *
 * Peeks two stack values: @c val1 (top, used as a 'kind' argument to
 * the sound-data lookup) and @c sfxIdx (one below). If the slot's bit
 * is already set in @c sfxStartMask, return 5 (busy).
 *
 * Otherwise: look up the SFX data via @c getOffsetTableEntry(g_curFieldMessages,
 * val1), kick off playback (@c initSfxPlayback / @c startSfxSlow),
 * promote to the global flag, set both @c sfxStartMask and
 * @c sfxActiveMask bits, pop two stack slots, then register the
 * entry into @c D_80085300 via @c func_800BC12C.
 *
 * Returns 3 on success.
 */
s32 opHandler_MES(ScriptContext *context) {
    u8 buf[8];
    s32 val1   = context->stack[(s8)context->stackPtr];
    s32 sfxIdx = context->stack[(s8)context->stackPtr - 1];
    u8 *data;

    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 5;
    }

    data = getOffsetTableEntry(g_curFieldMessages, val1);
    initSfxPlayback(sfxIdx, data);
    startSfxSlow(sfxIdx);
    setSfxGlobalFlag(sfxIdx);

    g_fieldVars->sfxStartMask  |= (1 << sfxIdx);
    g_fieldVars->sfxEntryMask  |= (1 << sfxIdx);

    context->stackPtr -= 2;
    func_800BC12C(sfxIdx, (s32)data, (u16 *)buf);
    return 3;
}

/**
 * @brief Clip a rect to the visible screen area (8,8 → 0x130x0xE0).
 *
 * Adjusts @c r->x so the rect doesn't run off the right edge
 * (@c x+w < 0x130), then clamps @c x >= 8. Same for @c y / @c h
 * against the bottom edge (0xE0) and top edge (8).
 */
void func_800BC258(Rect *r) {
    if (r->x + r->w >= 0x130) {
        r->x = 0x138 - (u16)r->w;
    }
    if (r->x < 8) {
        r->x = 8;
    }
    if (r->y + r->h >= 0xE0) {
        r->y = 0xE0 - (u16)r->h;
    }
    if (r->y < 8) {
        r->y = 8;
    }
}

/**
 * @brief SFX trigger with text-measured rect and entry registration.
 *
 * Variant of @c opHandler_MES that also constructs a clipped on-screen
 * rectangle around a piece of text: peeks @c sfxIdx / @c textIdx /
 * @c rect.x / @c rect.y from the stack, measures the text via
 * @c func_8002E680 to set @c rect.w / @c rect.h (with +0x10/+0x11
 * padding), clips via @c func_800BC258, and calls @c func_8002E064
 * to install the rect for the SFX slot.
 *
 * @return 5 if slot busy, 1 on success (kicks off SFX + entry),
 *         3 once the slot frees up while inactive (pops 4).
 */
s32 opHandler_AMESW(ScriptContext *context) {
    Rect buf;
    s32 sfxIdx;
    s32 textIdx;
    u8 *data;
    s32 dims;

    sfxIdx  = context->stack[(s8)context->stackPtr - 3];
    textIdx = context->stack[(s8)context->stackPtr - 2];
    buf.x   = (u16)context->stack[(s8)context->stackPtr - 1];
    buf.y   = (u16)context->stack[(s8)context->stackPtr];

    if ((context->activeMask >> context->scriptSlot) & 1) {
        if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
            return 5;
        }
        data = getOffsetTableEntry(g_curFieldMessages, textIdx);
        initSfxPlayback(sfxIdx, data);
        dims = func_8002E680(data);
        buf.w = (dims & 0xFFFF) + 0x10;
        buf.h = (dims >> 16) + 0x11;
        func_800BC258(&buf);
        func_8002E064(sfxIdx, (s16 *)&buf);
        startSfxSlow(sfxIdx);
        setSfxGlobalFlag(sfxIdx);
        g_fieldVars->sfxStartMask |= (1 << sfxIdx);
        func_800BC12C(sfxIdx, (s32)data, (u16 *)&buf);
        return 1;
    }
    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 1;
    }
    context->stackPtr -= 4;
    return 3;
}

/**
 * @brief Unconditional SFX trigger with text-measured rect.
 *
 * Like @c opHandler_AMESW but no @c activeMask gate — always runs once
 * the slot is free. Sets both @c sfxStartMask and @c sfxEntryMask,
 * pops four stack slots, registers the entry, and returns 3.
 *
 * @return 5 if slot busy, 3 otherwise.
 */
s32 opHandler_AMES(ScriptContext *context) {
    Rect buf;
    s32 sfxIdx;
    s32 textIdx;
    u8 *data;
    s32 dims;

    sfxIdx  = context->stack[(s8)context->stackPtr - 3];
    textIdx = context->stack[(s8)context->stackPtr - 2];
    buf.x   = (u16)context->stack[(s8)context->stackPtr - 1];
    buf.y   = (u16)context->stack[(s8)context->stackPtr];

    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 5;
    }

    data = getOffsetTableEntry(g_curFieldMessages, textIdx);
    initSfxPlayback(sfxIdx, data);
    dims = func_8002E680(data);
    buf.w = (dims & 0xFFFF) + 0x10;
    buf.h = (dims >> 16) + 0x11;
    func_800BC258(&buf);
    func_8002E064(sfxIdx, &buf);
    startSfxSlow(sfxIdx);
    setSfxGlobalFlag(sfxIdx);

    g_fieldVars->sfxStartMask |= (1 << sfxIdx);
    g_fieldVars->sfxEntryMask |= (1 << sfxIdx);

    context->stackPtr -= 4;
    func_800BC12C(sfxIdx, (s32)data, (u16 *)&buf);
    return 3;
}

/**
 * @brief SFX trigger that pops 4 stack slots up front (vs peeking).
 *
 * Variant of @c opHandler_AMES that consumes its 4 args with @c POP
 * instead of peeking. Sets only @c sfxStartMask (no @c sfxEntryMask),
 * registers the entry, and returns 2 (vs 3 for the peeking version).
 *
 * @return 5 if slot busy, 2 on success.
 */
s32 opHandler_RAMESW(ScriptContext *context) {
    Rect buf;
    s32 sfxIdx;
    s32 textIdx;
    u8 *data;
    s32 dims;

    buf.y   = POP(context);
    buf.x   = POP(context);
    textIdx = POP(context);
    sfxIdx  = POP(context);

    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 5;
    }

    data = getOffsetTableEntry(g_curFieldMessages, textIdx);
    initSfxPlayback(sfxIdx, data);
    dims = func_8002E680(data);
    buf.w = (dims & 0xFFFF) + 0x10;
    buf.h = (dims >> 16) + 0x11;
    func_800BC258(&buf);
    func_8002E064(sfxIdx, &buf);
    startSfxSlow(sfxIdx);
    setSfxGlobalFlag(sfxIdx);

    g_fieldVars->sfxStartMask |= (1 << sfxIdx);
    func_800BC12C(sfxIdx, (s32)data, (u16 *)&buf);
    return 2;
}

/**
 * @brief Field-VM SFX trigger / state-machine handler (6-arg variant).
 *
 * Same shape as @c opHandler_AASK but without the on-screen rect /
 * text-measure setup, so it takes 6 stack params instead of 8.
 *
 * Bit-set: save global flag (@c D_800DE4D8), dispatch
 * @c func_8002D784(sfxIdx, data, paramY..V), kick off SFX, set both
 * @c sfxStartMask and @c sfxActiveMask, clear @c field_0x204.
 *
 * Bit-clear: 2-state machine on @c field_0x204 — state 0 queries
 * remaining duration and fades, state 1 waits for release then pops 6
 * and restores the saved flag.
 *
 * @return 1 working, 3 done, 5 slot busy.
 */
s32 opHandler_ASK(Actor *actor) {
    s32 sfxIdx;
    s32 textIdx;
    s32 paramY;
    s32 paramZ;
    s32 paramW;
    s32 paramV;
    u8 *data;
    s32 r;
    s32 state;

    paramV  = actor->context.stack[(s8)actor->context.stackPtr];
    paramW  = actor->context.stack[(s8)actor->context.stackPtr - 1];
    paramZ  = actor->context.stack[(s8)actor->context.stackPtr - 2];
    paramY  = actor->context.stack[(s8)actor->context.stackPtr - 3];
    textIdx = actor->context.stack[(s8)actor->context.stackPtr - 4];
    sfxIdx  = actor->context.stack[(s8)actor->context.stackPtr - 5];

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        if ((g_fieldVars->sfxActiveMask >> sfxIdx) & 1) {
            return 5;
        }
        D_800DE4D8 = getSfxGlobalFlag();
        data = getOffsetTableEntry(g_curFieldMessages, textIdx);
        func_8002D784(sfxIdx, data, paramY, paramZ, paramW, paramV);
        startSfxSlow(sfxIdx);
        actor->field_0x204 = 0;
        g_fieldVars->sfxStartMask  |= (1 << sfxIdx);
        g_fieldVars->sfxActiveMask |= (1 << sfxIdx);
    } else {
        state = actor->field_0x204;
        switch (state) {
        case 0:
            setSfxGlobalFlag(sfxIdx);
            r = func_8002CE84(sfxIdx);
            actor->context.resultSlots[0] = r;
            if (r >= 0) {
                fadeOutSfxSlow(sfxIdx);
                actor->field_0x204++;
            }
            break;
        case 1:
            if (getSfxField1C(sfxIdx) == 0) {
                g_fieldVars->sfxActiveMask &= ~(state << sfxIdx);
                actor->context.stackPtr -= 6;
                setSfxGlobalFlag(D_800DE4D8);
                return 3;
            }
            break;
        }
    }
    return 1;
}

/**
 * @brief Field-VM SFX trigger / state-machine handler.
 *
 * Reads 8 stack params (top two as packed u16 halfwords for an x/y rect
 * source, six s32 args including the SFX slot id, a text/data id, and
 * four playback parameters). When the entity's @c activeMask bit for the
 * current @c scriptSlot is set:
 *   - Returns 5 if the SFX slot is already active (bit set in @c field_0xD2).
 *   - Otherwise, saves the current global SFX flag, resolves a text pointer,
 *     measures it via @c func_8002E680, fills the rect's bottom-right corner
 *     (+0x30 X, +0x11 Y), runs the display setup chain, kicks off the slow
 *     SFX, and sets the slot's bits in both @c field_0xD2 and @c field_0xD3.
 * Otherwise, runs a 2-state machine on @c field_0x204:
 *   - State 0: set the global SFX flag, query remaining duration
 *     (@c func_8002CE84), store to @c result; if non-negative, fade out and
 *     advance the state.
 *   - State 1: when the SFX has fully released (@c getSfxField1C == 0),
 *     clear the slot's bit in @c field_0xD2, pop 8 stack slots, restore
 *     the saved SFX flag, and return 3.
 *
 * @return 1 while still working, 3 when the state-1 release completes,
 *         5 when the slot was already active.
 */
s32 opHandler_AASK(Actor *actor) {
    s16 buf[4];
    s32 sfxIdx;
    s32 textIdx;
    s32 paramY;
    s32 paramZ;
    s32 paramW;
    s32 paramV;
    u8 *text;
    s32 dims;
    s32 r;
    s32 state;

    buf[1] = *(u16 *)&actor->context.stack[actor->context.stackPtr];
    buf[0] = *(u16 *)&actor->context.stack[actor->context.stackPtr - 1];
    paramV  = actor->context.stack[actor->context.stackPtr - 2];
    paramW  = actor->context.stack[actor->context.stackPtr - 3];
    paramZ  = actor->context.stack[actor->context.stackPtr - 4];
    paramY  = actor->context.stack[actor->context.stackPtr - 5];
    textIdx = actor->context.stack[actor->context.stackPtr - 6];
    sfxIdx  = actor->context.stack[actor->context.stackPtr - 7];

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        if ((g_fieldVars->sfxActiveMask >> sfxIdx) & 1) {
            return 5;
        }
        D_800DE4DC = getSfxGlobalFlag();
        text = getOffsetTableEntry(g_curFieldMessages, textIdx);
        dims = func_8002E680(text);
        buf[2] = (dims & 0xFFFF) + 0x30;
        buf[3] = (dims >> 16) + 0x11;
        func_800BC258(buf);
        func_8002E064(sfxIdx, buf);
        func_8002D784(sfxIdx, text, paramY, paramZ, paramW, paramV);
        startSfxSlow(sfxIdx);
        actor->field_0x204 = 0;
        g_fieldVars->sfxStartMask |= (1 << sfxIdx);
        g_fieldVars->sfxActiveMask |= (1 << sfxIdx);
    } else {
        state = actor->field_0x204;
        switch (state) {
        case 0:
            setSfxGlobalFlag(sfxIdx);
            r = func_8002CE84(sfxIdx);
            actor->context.resultSlots[0] = r;
            if (r >= 0) {
                fadeOutSfxSlow(sfxIdx);
                actor->field_0x204++;
            }
            break;
        case 1:
            if (getSfxField1C(sfxIdx) == 0) {
                g_fieldVars->sfxActiveMask &= ~(state << sfxIdx);
                actor->context.stackPtr -= 8;
                setSfxGlobalFlag(D_800DE4DC);
                return 3;
            }
            break;
        }
    }
    return 1;
}


/**
 * @brief Tear down an entry-registered SFX slot.
 *
 * Peeks @c sfxIdx from the top of the stack and decides whether the
 * slot can be torn down.
 *
 * If the entry bit (@c sfxEntryMask) is set: also requires
 * @c sfxStartMask to be set (else return 1). Optionally fades out via
 * @c fadeOutSfxSlow when @c D_80070600 has the @c 0xC0 flag bits,
 * @c getSfxField28 reports something, and the slot is no longer active.
 * Waits for @c getSfxField1C to drop to 0 and @c getSfxField28 to
 * become non-zero, then clears both @c sfxStartMask and @c sfxEntryMask
 * bits, pops one stack slot, and returns 2.
 *
 * If the entry bit is clear: only waits for @c sfxStartMask to clear,
 * then pops one and returns 2.
 *
 * @return 1 while waiting, 2 when torn down.
 */
s32 opHandler_MESSYNC(ScriptContext *context) {
    s32 sfxIdx = context->stack[(s8)context->stackPtr];

    if ((g_fieldVars->sfxEntryMask >> sfxIdx) & 1) {
        if (!((g_fieldVars->sfxStartMask >> sfxIdx) & 1)) {
            return 1;
        }
        if ((D_80070600 & 0xC0) && getSfxField28(sfxIdx)
            && !((g_fieldVars->sfxActiveMask >> sfxIdx) & 1)) {
            fadeOutSfxSlow(sfxIdx);
        }
        if (getSfxField1C(sfxIdx)) {
            return 1;
        }
        if (!getSfxField28(sfxIdx)) {
            return 1;
        }
        g_fieldVars->sfxStartMask &= ~(1 << sfxIdx);
        g_fieldVars->sfxEntryMask &= ~(1 << sfxIdx);
        context->stackPtr -= 1;
        return 2;
    }
    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 1;
    }
    context->stackPtr -= 1;
    return 2;
}

/**
 * @brief Pop the top stack slot and pass it to @c setSfxGlobalFlag.
 */
s32 opHandler_MESFORCUS(ScriptContext *context) {
    setSfxGlobalFlag(POP(context));
    return 2;
}

/**
 * @brief Register a pre-built rect entry into @c D_80085300 (no SFX kick).
 *
 * Peeks @c sfxIdx and a 4-halfword rect from the stack. If the slot is
 * busy (@c sfxStartMask set) return 5. Otherwise clip the rect, install
 * it via @c func_8002E064, register an entry with @c data=0 via
 * @c func_800BC12C, pop 5, and return 2.
 */
s32 opHandler_WINSIZE(ScriptContext *context) {
    Rect buf;
    s32 sfxIdx = context->stack[(s8)context->stackPtr - 4];

    if ((g_fieldVars->sfxStartMask >> sfxIdx) & 1) {
        return 5;
    }

    buf.h = (u16)context->stack[(s8)context->stackPtr];
    buf.w = (u16)context->stack[(s8)context->stackPtr - 1];
    buf.y = (u16)context->stack[(s8)context->stackPtr - 2];
    buf.x = (u16)context->stack[(s8)context->stackPtr - 3];

    func_800BC258(&buf);
    func_8002E064(sfxIdx, &buf);
    context->stackPtr -= 5;
    func_800BC12C(sfxIdx, 0, (u16 *)&buf);
    return 2;
}

/**
 * @brief Tear down (or fade) an SFX entry slot — single-arg variant.
 *
 * Peek @c sfxIdx, then if the SFX is still releasing
 * (@c getSfxField1C != 0), kick a slow fade and return 1. Once it's
 * fully done, clear both @c sfxStartMask and @c sfxEntryMask bits,
 * pop one, and return 2.
 */
s32 opHandler_WINCLOSE(ScriptContext *context) {
    s32 sfxIdx = context->stack[(s8)context->stackPtr];

    if (!getSfxField1C(sfxIdx)) {
        g_fieldVars->sfxStartMask &= ~(1 << sfxIdx);
        g_fieldVars->sfxEntryMask &= ~(1 << sfxIdx);
        context->stackPtr -= 1;
        return 2;
    }
    fadeOutSfxSlow(sfxIdx);
    return 1;
}

/**
 * @brief Update the @c field4 slot of a @c D_80085398 entry.
 *
 * Pops @c (idx, val) from the Actor stack, calls @c updateAnimEntry
 * to refresh whatever runtime state the helper tracks, then writes
 * @c val into @c D_80085398[idx].field4 and returns 2.
 */
s32 opHandler_SETBAR(ScriptContext *context) {
    s32 val = POP(context);
    s32 idx = POP(context);
    updateAnimEntry(idx, val);
    D_80085398[idx].field4 = val;
    return 2;
}

/**
 * @brief Set up a 7-arg animation entry in the @c D_80085398 table.
 *
 * Pops 7 stack values: 6 halfwords (v1..v6) plus an entry index. The
 * low bit of @c idx selects the bank, @c v4 is passed as the second
 * arg, and the two leading halfwords (@c v5/v6) are passed via a
 * 4-byte stack-allocated @c buf. The remaining halfwords spread
 * across the @c setupAnimEntry register args.
 *
 * After the helper returns, all six values plus a @c flag=1 marker
 * are recorded into the per-slot entry at @c D_80085398[idx].
 */
s32 opHandler_DISPBAR(ScriptContext *context) {
    u16 buf[2];
    s32 v1 = POP(context);
    s32 v2 = POP(context);
    s32 v3 = POP(context);
    s32 v4 = POP(context);
    s32 v5 = POP(context);
    s32 v6 = POP(context);
    s32 idx = POP(context);

    buf[0] = v6;
    buf[1] = v5;
    setupAnimEntry(idx & 1, v4, buf, v3, v2, v1);

    D_80085398[idx].flag   = 1;
    D_80085398[idx].fieldE = v6;
    D_80085398[idx].fieldC = v5;
    D_80085398[idx].fieldA = v4;
    D_80085398[idx].field8 = v3;
    D_80085398[idx].field6 = v2;
    D_80085398[idx].field4 = v1;
    return 2;
}

/**
 * @brief Set up an 8-arg animation entry in the @c D_80085398 table.
 *
 * Variant of @c opHandler_DISPBAR: pops 8 stack values (7 halfwords plus
 * an entry index), passes them via @c setupAnimEntryFull (the 7th
 * value lands as the 7th register/stack arg), and records all 8 plus
 * a @c flag=2 marker into the per-slot entry at @c D_80085398[idx].
 */
s32 opHandler_BROKEN(ScriptContext *context) {
    u16 buf[2];
    s32 v0 = POP(context);
    s32 v1 = POP(context);
    s32 v2 = POP(context);
    s32 v3 = POP(context);
    s32 v4 = POP(context);
    s32 v5 = POP(context);
    s32 v6 = POP(context);
    s32 idx = POP(context);

    buf[0] = v6;
    buf[1] = v5;
    setupAnimEntryFull(idx & 1, v4, buf, v3, v2, v1, v0);

    D_80085398[idx].flag   = 2;
    D_80085398[idx].fieldE = v6;
    D_80085398[idx].fieldC = v5;
    D_80085398[idx].fieldA = v4;
    D_80085398[idx].field8 = v3;
    D_80085398[idx].field6 = v2;
    D_80085398[idx].field4 = v1;
    D_80085398[idx].field2 = v0;
    return 2;
}

/**
 * @brief Pop value, clear D_80085398 table entry, call sound handler.
 *
 * Pops the stack to get an index, clears the halfword at D_80085398[idx * 16],
 * then calls clearAnimEntryActive with the popped value.
 *
 * @param actor Script entity context.
 * @return 2.
 */
s32 opHandler_KILLBAR(ScriptContext *context) {
    s32 val = POP(context);
    D_80085398[val].flag = 0;
    clearAnimEntryActive(val);
    return 2;
}
