/**
 * @file fe_object1b.c
 *
 * The PsyQ 4.3 (gcc 2.8.0) island that sits between @c func_800A63AC and
 * the end of the original fe_object1 translation unit. The functions in
 * this file are originally compiled with the PsyQ 4.3 toolchain — their
 * FILLED branch-delay-slot epilogues (`jr ra; addiu sp` instead of PsyQ
 * 4.1's `addiu sp; jr ra; nop`) are the dead giveaway. The rest of
 * fe_object1 is PsyQ 4.1, so these had to live in a separate translation
 * unit (added to @c PSYQ43_SRCS in the Makefile).
 *
 * Everything below was originally INCLUDE_ASM in fe_object1.c with notes
 * to "split the file" — this is that split.
 */

#include "common.h"
#include "field.h"
#include "psxsdk/libc.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "field/fe_object1.h"

extern u8 D_800DD6D0[];

/**
 * @brief Per-frame entity render pipeline orchestrator.
 *
 * Three-phase walk over the @c D_800D9630[] entity slot table:
 *
 *  1. **Prepare**: for each active slot (@c slot->base @c != -1), call
 *     @c func_800AA5F8 (per-entity preparation).
 *
 *  2. **Cache update / vertex backup**: for each active slot, either
 *     copy its @c POLY_FT4 / @c POLY_GT4 vertex blocks from the main
 *     buffer to the alt buffer (when @c flags @c & @c 0x10 = DIRTY), or
 *     update the entity's cached @c ctxId / @c frameKey (when not).
 *
 *  3. **Transform** (call @c func_800ACBD4 with the OT base + 1) — runs
 *     the GTE transform pass for all queued vertices.
 *
 *  4. **OT linkage**: for each active slot with @c READY bit set, walk
 *     the slot's quads and link each transformed primitive (via the
 *     classic @c setaddr(prim,getaddr(ot)) + @c setaddr(ot,prim) pair)
 *     into the ordering table at the slot's per-vertex OT index.
 *
 * @note Permuter base at @c permuter/func_800A63AC/base.c sits at 77.02%
 *       — function structure and the three loops match; remaining diff
 *       is register-allocation cascade (reg names s1/s2/s3 shuffled,
 *       t-regs in different slots) and a 0x30-byte stack-frame size
 *       gap. Needs permuter to close the last 23%.
 */
INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A63AC);

/**
 * @brief Per-quad vertex transform + GTE scratch dispatch.
 *
 * For each quad in the entity's @c ref->quads array, iterates over its
 * vertices (from @c quad->h0 up to @c quad->h0 @c + @c quad->h2) and
 * dispatches on @c quad->hE flags:
 *
 *  - @c bit 0 set (path A): full keyframe transform — calls
 *    @c SetRotMatrix to set GTE registers, @c SetTransMatrix to push,
 *    interpolates the @c (h18-h8)/(h1A-hA)/(h1C-hC) position triplet by
 *    @c h4/h1E, runs the GTE transform via @c RotTrans, copies an
 *    8-word vertex block from the source buffer, applies the delta, runs
 *    a second @c RotTrans + @c SetRotMatrix pair, increments
 *    @c h4 and either flips the @c hE bit-0 / sets bit-2 (if exhausted)
 *    or sets @c slot->unk78 = -1 (otherwise).
 *
 *  - @c bit 2 set (path B): same single-step transform without the
 *    second round.
 *
 *  - @c neither set (path C): just @c SetRotMatrix + @c SetTransMatrix
 *    on the vertex buffer.
 *
 * After the per-vertex dispatch, calls @c func_800AD048 (or zeroes the
 * D_800D96B0/D_800D6720 entries if the @c slot->flags @c & @c 0x20
 * cached-keyframe path applies) and advances the counters.
 *
 * After the inner loop completes per-quad, calls @c func_800AD0E8 to
 * link @c POLY_FT4 primitives and @c func_800AD300 for @c POLY_GT4.
 *
 * @note Permuter base at @c permuter/func_800A6A80/base.c sits at 56.85%
 *       — three-path branch + shared tail + final calls are in place,
 *       but path A's vertex copy + double-transform sequence is partial
 *       and the stack-frame size is short (0x68 vs target's 0x88).
 *       Needs permuter to close the gap.
 */
INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A6A80);

/**
 * @brief Reset three field-engine tables.
 *
 * 1. Calls @c memset (@c memset) on each of 64 @c D_800DD6D0
 *    entries (stride 0x30 = 48 bytes), zeroing each in turn.
 * 2. Clears 64 @c s32 entries at @c D_800D6620.
 * 3. Clears 32 @c EntityRenderSlot* pointer entries at @c D_800D9630.
 *
 * The forward for-loops in the scratch source compile to backward
 * @c bgez loops — gcc strength-reduces the index since each iteration
 * is independent.
 */
void func_800A7194(void) {
    s32 i = 0;
    u8 *p = D_800DD6D0;

    do {
        memset(p, 0, 0x30);
        i++;
        p += 0x30;
    } while (i < 64);
    for (i = 0; i < 64; i++) {
        D_800D6620[i] = 0;
    }
    for (i = 0; i < 32; i++) {
        D_800D9630[i] = NULL;
    }
}

/** @brief 8-byte (2-aligned) block copied into an @ref EntityRenderSlot's
 *         @c unk10 region by @c func_800A7224 mode 0. The 2-byte alignment is
 *         what makes the copy emit as an unaligned @c lwl/lwr/swl/swr pair. */
typedef struct {
    u16 a, b, c, d;
} RenderSlotVec4;

/**
 * @brief Per-mode update of an @ref EntityRenderSlot's @c unk10 vector, then a
 *        change-detect that re-snapshots and clears flags when it moved.
 *
 * Dispatches on @p mode over @c D_800D9630[idx]:
 *  - @c mode @c 0: overwrite the 8-byte @c unk10 block from @p vals.
 *  - @c mode @c 1: add @p vals[0..2] into @c unk10 / @c unk12 / @c unk14.
 *  - any other @p mode: leave the vector unchanged.
 *
 * Then (all modes) compares the @c unk10 vector against its snapshot
 * (@c field84 / @c field86 / @c field88); if it changed, clears bits @c 0x18
 * of @c unk60 and refreshes the snapshot.
 *
 * @param idx  Render-slot index into @ref D_800D9630.
 * @param vals Source vector (mode 0 uses 8 bytes; mode 1 uses the first three).
 * @param mode Selects overwrite (0), accumulate (1), or compare-only.
 */
void func_800A7224(s32 idx, u16 *vals, s32 mode) {
    EntityRenderSlot *slot;

    switch (mode) {
    case 0:
        *(RenderSlotVec4 *)&D_800D9630[idx]->unk10 = *(RenderSlotVec4 *)vals;
        break;
    case 1:
        D_800D9630[idx]->unk10 += vals[0];
        D_800D9630[idx]->unk12 += vals[1];
        D_800D9630[idx]->unk14 += vals[2];
        break;
    }

    slot = D_800D9630[idx];
    if ((*(u32 *)&slot->field84 != *(u32 *)&slot->unk10) |
        (slot->field88 != (s16)slot->unk14)) {
        slot->unk60 &= 0xE7;
        D_800D9630[idx]->field84 = D_800D9630[idx]->unk10;
        D_800D9630[idx]->field86 = D_800D9630[idx]->unk12;
        D_800D9630[idx]->field88 = D_800D9630[idx]->unk14;
    }
}

/**
 * @brief Twin of @c func_800A7224 for the @c unk18 vector of an
 *        @ref EntityRenderSlot (same switch + change-detect, offset +8).
 *
 * Dispatches on @p mode over @c D_800D9630[idx]:
 *  - @c mode @c 0: overwrite the 8-byte @c unk18 block from @p vals.
 *  - @c mode @c 1: add @p vals[0..2] into @c unk18 / @c unk1A / @c unk1C.
 *  - any other @p mode: leave the vector unchanged.
 *
 * Then (all modes) compares the @c unk18 vector against its snapshot
 * (@c field8C / @c field8E / @c field90); if it changed, clears bits @c 0x18
 * of @c unk60 and refreshes the snapshot.
 *
 * @param idx  Render-slot index into @ref D_800D9630.
 * @param vals Source vector (mode 0 uses 8 bytes; mode 1 uses the first three).
 * @param mode Selects overwrite (0), accumulate (1), or compare-only.
 */
void func_800A736C(s32 idx, u16 *vals, s32 mode) {
    EntityRenderSlot *slot;

    switch (mode) {
    case 0:
        *(RenderSlotVec4 *)&D_800D9630[idx]->unk18 = *(RenderSlotVec4 *)vals;
        break;
    case 1:
        D_800D9630[idx]->unk18 += vals[0];
        D_800D9630[idx]->unk1A += vals[1];
        D_800D9630[idx]->unk1C += vals[2];
        break;
    }

    slot = D_800D9630[idx];
    if ((*(u32 *)&slot->field8C != *(u32 *)&slot->unk18) |
        (slot->field90 != (s16)slot->unk1C)) {
        slot->unk60 &= 0xE7;
        D_800D9630[idx]->field8C = D_800D9630[idx]->unk18;
        D_800D9630[idx]->field8E = D_800D9630[idx]->unk1A;
        D_800D9630[idx]->field90 = D_800D9630[idx]->unk1C;
    }
}

/**
 * @brief Per-mode write/accumulate of three or four @c s32 values into
 *        an @ref EntityRenderSlot.
 *
 * Dispatches on @p mode over the render slot @c D_800D9630[idx]:
 *  - @c mode @c 0: overwrite the whole @c xform block — a single
 *    @ref EntityRenderXform aggregate copy from @p vals (all four words).
 *  - @c mode @c 1: add @p vals into @c xform.field20 / @c field24 / @c field28
 *    (each read-modify-write reloads the slot pointer; @c field2C untouched).
 *  - any other @p mode: no-op.
 *
 * @param idx   Render-slot index into @ref D_800D9630.
 * @param vals  Transform values to store (mode 0) or accumulate (mode 1).
 * @param mode  Selects overwrite (0), accumulate (1), or no-op.
 */
void func_800A74B4(s32 idx, EntityRenderXform *vals, s32 mode) {
    EntityRenderSlot *slot;

    switch (mode) {
    case 0:
        slot = D_800D9630[idx];
        slot->xform = *vals;
        break;
    case 1:
        D_800D9630[idx]->xform.field20 += vals->field20;
        D_800D9630[idx]->xform.field24 += vals->field24;
        D_800D9630[idx]->xform.field28 += vals->field28;
        break;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A7564);

/**
 * @brief Allocate and initialize the field-object slot @p idx from @p newObj.
 *
 * If the slot is already occupied, returns 0 without touching it. Otherwise it
 * installs @p newObj into @c D_800D6620[idx], stamps the @c 0x12345678 signature
 * and the @c D_800D60E8 seed, invokes @c func_800A7564 to build the object, and
 * clears the per-part @c field0E and per-sub @c field06 accumulators across
 * every part's sub-range.
 *
 * @param idx    Object slot index into @ref D_800D6620 (0..63).
 * @param arg1   Opaque handle forwarded to @c func_800A7564 as its first arg.
 * @param newObj Object instance to install into the slot.
 * @param count  Build count for @c func_800A7564; defaults to 0x10 when zero.
 * @return @c func_800A7564's result, or 0 if the slot was already occupied.
 */
s32 func_800A8058(s32 idx, s32 arg1, FieldObject *newObj, u8 count) {
    s32 ret;
    s32 i, j;

    if (D_800D6620[idx] != NULL) {
        return 0;
    }
    if (count == 0) {
        count = 0x10;
    }
    D_800D6620[idx] = newObj;
    newObj->field5F = 0;
    D_800D6620[idx]->signature = 0x12345678;
    D_800D6620[idx]->field50 = D_800D60E8;
    ret = func_800A7564(arg1, 0x11, count, idx);
    for (i = 0; i < D_800D6620[idx]->partCount; i++) {
        D_800D6620[idx]->parts[i].field0E = 0;
        for (j = D_800D6620[idx]->parts[i].subStart;
             j < D_800D6620[idx]->parts[i].subStart + D_800D6620[idx]->parts[i].subCount;
             j++) {
            D_800D6620[idx]->subs[j].field06 = 0;
        }
    }
    return ret;
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A81AC);

/**
 * @brief Claim a render-slot pool entry and initialize its state.
 *
 * Acquires @c D_800D9630[idx] if currently @c NULL by installing @p slot
 * and writing @p firstWord to the slot's leading @c s32. The slot's state
 * fields (@c unk10/12/14/18/1A/1C/50/52) are zeroed except @c unk12 which
 * gets the default @c 0x190; the three @c field20/24/28 scale factors are
 * set to @c 0x1000 (unit scale); @c unk60 (active flag) is cleared.
 *
 * @return @p slot @c + @c 0x98 on successful claim, or @c NULL if the
 *         render-slot was already occupied.
 */
s32 *func_800A8CDC(s32 idx, s32 firstWord, EntityRenderSlot *slot) {
    if (D_800D9630[idx] != NULL) {
        return NULL;
    }
    D_800D9630[idx] = slot;
    *(s32 *)slot = firstWord;
    D_800D9630[idx]->unk60 = 0;
    D_800D9630[idx]->unk10 = 0;
    D_800D9630[idx]->unk12 = 0x190;
    D_800D9630[idx]->unk14 = 0;
    D_800D9630[idx]->unk18 = 0;
    D_800D9630[idx]->unk1A = 0;
    D_800D9630[idx]->unk1C = 0;
    D_800D9630[idx]->xform.field20 = 0x1000;
    D_800D9630[idx]->xform.field24 = 0x1000;
    D_800D9630[idx]->xform.field28 = 0x1000;
    D_800D9630[idx]->unk50 = 0;
    D_800D9630[idx]->unk52 = 0;
    D_800D9630[idx]->unk60 = 0;
    return &slot->subBuffer;
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A8DAC);

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A91C8);

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A9434);

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800A97E4);

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800AA46C);

/**
 * @brief Per-entity animation tick — advances the frame and stages the next
 *        sprite rectangle for upload.
 *
 * For render slot @p idx this:
 *  - returns 1 immediately if the entity's definition is not animated
 *    (@c def->unk5F bit 0 clear);
 *  - while the retrigger flag (@c animFlags @c 0x80) is set, marks the slot
 *    active (@c 0x10) on the frames where @c rand() is an exact multiple of
 *    the loop period;
 *  - returns 0 while the per-frame countdown @c frameTimer has not expired;
 *  - on expiry reloads @c frameTimer, steps @c frameIdx, and when that wraps
 *    to 0 re-arms the retrigger flag and reloads the frame count;
 *  - builds the destination sprite @ref RECT in @c D_800D5ED8 from the current
 *    frame's coordinates and the animation rect size, then uploads it with
 *    @c MoveImage.
 *
 * Originally emitted as two INCLUDE_ASM stubs (@c func_800AA5F8 plus a
 * spurious @c func_800AA870 tail label) because splat cut the function at the
 * load-delay @c nop preceding the final @c MoveImage call.
 *
 * @param idx Entity render-slot index into @ref D_800D9630.
 * @return 1 if the entity is not animated, otherwise 0.
 */
s32 func_800AA5F8(s32 idx) {
    s16 period;
    u8 flags;
    s16 per;

    period = D_800D9630[idx]->def->anim->frames[0].y;
    if ((D_800D9630[idx]->def->unk5F & 1) == 0) {
        return 1;
    }

    flags = D_800D9630[idx]->animFlags;
    if (flags & 0x80) {
        per = period;
        if (per > 0) {
            if (rand() % per == 0) {
                D_800D9630[idx]->animFlags &= 0x0F;
                D_800D9630[idx]->animFlags |= 0x10;
            }
        } else {
            D_800D9630[idx]->animFlags = flags & 0x0F;
            D_800D9630[idx]->animFlags |= 0x10;
        }
    }

    if ((D_800D9630[idx]->animFlags & 0x10) == 0) {
        return 0;
    }

    D_800D9630[idx]->frameTimer--;
    if (D_800D9630[idx]->frameTimer != 0) {
        return 0;
    }

    if (period < 0) {
        D_800D9630[idx]->frameTimer = period;
    } else {
        D_800D9630[idx]->frameTimer = D_800D9630[idx]->timerReload;
    }

    D_800D9630[idx]->frameIdx--;
    if (D_800D9630[idx]->frameIdx == 0) {
        D_800D9630[idx]->animFlags &= 0x0F;
        D_800D9630[idx]->animFlags |= 0x80;
        D_800D9630[idx]->frameIdx = D_800D9630[idx]->def->anim->frameReload;
    }

    D_800D5ED8.w = D_800D9630[idx]->def->anim->rectW;
    D_800D5ED8.h = D_800D9630[idx]->def->anim->rectH;
    /* frameIdx[frames] is frames[frameIdx]; the swapped index keeps the array
       index in the first addu operand, matching the original codegen. */
    D_800D5ED8.x = D_800D9630[idx]->frameIdx[D_800D9630[idx]->def->anim->frames].x;
    D_800D5ED8.y = D_800D9630[idx]->frameIdx[D_800D9630[idx]->def->anim->frames].y;

    MoveImage(&D_800D5ED8,
              D_800D9630[idx]->def->anim->srcX,
              D_800D9630[idx]->def->anim->frames[0].x);
    return 0;
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1b", func_800AA8A0);
