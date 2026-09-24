#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "world.h"
#include "world/we_object5.h"
#include "world/we_object6.h"
#include "world/we_object10.h"
#include "world/we_object3.h"
#include "btl_sfx.h"
#include "btl_anim.h"
#include "world/we_object1.h"
#include "world/we_object9.h"





static void func_800BEFEC(u8 *base);


/**
 * @brief Search @c D_800DBFB8 for the slot whose @c marker matches the
 *        value returned by @c func_800B0010.
 *
 * Classifies @p kind via @c func_800B0010 to get a marker byte, then scans
 * first @c D_800C5B50 entries of @c D_800DBFB8 for the first slot whose
 * @c marker matches. Returns the slot index or @c -1 if not found.
 *
 * Twin of @c func_800BD754 — same scan loop, but the search key comes
 * from classifying @p kind through @c func_800B0010.
 *
 * @param kind Scene/map kind to classify.
 * @return Matching slot index, or @c -1 on no match.
 */
s32 func_800BD6EC(u32 kind) {
    s32 target = func_800B0010(kind);
    s32 i = 0;
    s32 count = D_800C5B50;
    if (count > 0) {
        s32 bound = count;
        SlotEntry *entry = &D_800DBFB8[0];
        do {
            if (target == entry->marker) return i;
            i++;
            entry++;
        } while (i < bound);
    }
    return -1;
}

/**
 * @brief Linear search D_800DBFB8 for the first SlotEntry whose marker matches @p target.
 *
 * Scans the first @c D_800C5B50 entries of @c D_800DBFB8, comparing each
 * entry's @c marker byte against @p target. Returns the slot index on
 * match or -1 if no match is found (including the empty case when
 * @c D_800C5B50 <= 0).
 *
 * @param target Marker byte value to search for.
 * @return Matching slot index, or -1 on no match.
 */
s32 func_800BD754(s32 target) {
    s32 count = D_800C5B50;
    if (count > 0) {
        s32 i = 0;
        s32 bound = count;
        SlotEntry *entry = &D_800DBFB8[0];
        do {
            if (target == entry->marker) return i;
            i++;
            entry++;
        } while (i < bound);
    }
    return -1;
}


/**
 * @brief Scale @p amount based on the current command's type byte.
 *
 * Peeks the type byte of the current command descriptor (@c D_800C4D64):
 *   - type == 0x1C: halve @p amount.
 *   - type == 0x1B or type == 8: double @p amount.
 *   - anything else: pass through unchanged.
 *
 * @param unused First arg register ignored by the implementation.
 * @param amount Quantity to scale.
 * @return Scaled @p amount.
 */
s32 func_800BD7A4(s32 unused, s32 amount) {
    u8 type = D_800C4D64->type;
    if (type == 0x1C) {
        amount >>= 1;
    } else if (type == 0x1B || type == 8) {
        amount <<= 1;
    }
    return amount;
}

/**
 * @brief Locate the active-party slot whose character matches battleParty[charIdx].
 *
 * Looks up the character id stored in @c g_gameState.battleParty[charIdx]
 * and scans @c g_gameState.mainData.party.party[0..2] for the first slot
 * whose id matches. Returns that slot index (0-2), or 0 if no match.
 *
 * @note A zero return is ambiguous — it's also returned on no-match.
 *
 * @param charIdx Battle-party index (0-3).
 * @return Active-party slot (0-2) whose member matches, else 0.
 */
s32 func_800BD7E4(s32 charIdx) {
    GameState *gs = &g_gameState;
    u8 target = gs->battleParty[charIdx];
    s32 i;
    for (i = 0; i < 3; i++) {
        if (gs->mainData.party.party[i] == target) {
            return i;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BD82C);

/**
 * @brief Copy 10 flag bytes from D_800780D8 to destination struct.
 *
 * Copies bytes at D_800780D8 offsets 0x108-0x10F to dst offsets 0x66-0x6D,
 * and 0x100-0x101 to dst offsets 0x6E-0x6F.
 *
 * @param dst Destination buffer pointer.
 */
void func_800BD918(u8 *dst) {
    u8 *src = D_800780D8;

    dst[0x66] = src[0x108];
    dst[0x67] = src[0x109];
    dst[0x68] = src[0x10A];
    dst[0x69] = src[0x10B];
    dst[0x6A] = src[0x10C];
    dst[0x6B] = src[0x10D];
    dst[0x6C] = src[0x10E];
    dst[0x6D] = src[0x10F];
    dst[0x6E] = src[0x100];
    dst[0x6F] = src[0x101];
}

/**
 * @brief Per-map constant lookup with implicit fall-through return.
 *
 * Dispatches on @p mapId:
 *  - @p mapId @c < @c 0xA or @c == @c 0x80:                   @c 0x10.
 *  - @p mapId @c == @c 0x32:                                  @c 0x78.
 *  - @p mapId in @c [0x20, @c 0x29) or @c == @c 0x84:         @c 0x20.
 *  - @p mapId in @c [0x40, @c 0x43):                          @c 0x78.
 *  - @p mapId @c == @c 0x30:                                  @c 0x20.
 *  - @p mapId @c == @c 0x31:                                  @c 0x78.
 *  - otherwise:                                               @c 0x31.
 *
 * The @c 0x31 "default" is intentionally implicit — there is no
 * trailing @c return statement, and the compiler leaves @c 0x31 in
 * @c v0 from the final @c (mapId @c == @c 0x31) comparison's
 * @c bne-delay-slot @c addiu. Adding @c return @c 0x31; explicitly
 * generates a separate @c addiu and breaks the match.
 *
 * @param mapId Unsigned to make the @c mapId @c - @c 0x20 @c < @c 9 and
 *              @c mapId @c - @c 0x40 @c < @c 3 range tests compile as
 *              @c sltiu (one instruction) rather than a signed two-step
 *              subtract+compare.
 * @return Per-map constant — @c 0x10, @c 0x20, @c 0x78, or @c 0x31.
 */
s32 func_800BD998(u32 mapId) {
    if (mapId < 0xA) return 0x10;
    if (mapId == 0x80) return 0x10;
    if (mapId == 0x32) return 0x78;
    if (mapId - 0x20 < 9) return 0x20;
    if (mapId == 0x84) return 0x20;
    if (mapId - 0x40 < 3) return 0x78;
    if (mapId == 0x30) return 0x20;
    if (mapId == 0x31) return 0x78;
}


/**
 * @brief Tick @c D_800C5DAC down by @c 0x10, clamped to @c [0, @c 0x40],
 *        resetting it to @c 0x40 first if the active command's @c type
 *        byte changed.
 *
 * If the current command (@c D_800C4D64) and the previous command
 * (@c D_800C4D74) have different @c type bytes, the counter is
 * reloaded to @c 0x40 (full). Then the counter is decremented by
 * @c 0x10 and clamped to @c [0, @c 0x40] — the unclamped intermediate
 * is stored briefly so the surrounding code matches the original's
 * "store then clamp then store" sequence.
 *
 * @return The new value of @c D_800C5DAC.
 */
s32 func_800BDA08(void) {
    s32 v;
    s32 *p;

    if (D_800C4D64->type != D_800C4D74->type) {
        D_800C5DAC = 0x40;
    }

    p = &D_800C5DAC;
    v = *p - 0x10;
    *p = v;
    v = CLAMP(v, 0, 0x40);
    *p = v;
    return D_800C5DAC;
}

/**
 * @brief Edge-trigger each of two threshold flags at @p flags[0..1] from
 *        the position @p pos, returning the index that crossed.
 *
 *  - Slot @c 0 (threshold @c 0x110): when @p pos is below the threshold,
 *    the flag is reset to @c 0; otherwise, if the flag was @c 0, the
 *    crossing is registered (@c result = @c 0, flag becomes @c 1).
 *  - Slot @c 1 (threshold @c 0x70): the inverse — when @p pos is below
 *    the threshold the flag is reset and no crossing is reported;
 *    otherwise the same edge-trigger as slot 0.
 *
 * If both slots crossed in the same call the return value is the
 * higher-index one (slot 1 overwrites slot 0).
 *
 * @param pos   Position to test against the two thresholds.
 * @param flags Two-byte array; @c flags[0] is slot 0, @c flags[1] is slot 1.
 * @return The index of the (last) slot that crossed this call, or @c -1.
 */
s32 func_800BDA78(s32 pos, u8 *flags) {
    s32 result = -1;
    s32 i;

    for (i = 0; i < 2; i++, flags++) {
        if (i == 0) {
            if (pos < 0x110) {
                *flags = 0;
                continue;
            }
        } else if (pos < 0x70) {
            goto clear;
        }
        if (*flags == 0) {
            result = i;
            *flags = 1;
        }
        continue;
    clear:
        *flags = 0;
    }
    return result;
}

/**
 * @brief Classify @p pos against two band centres (@c 0x90 and @c 0x130)
 *        within tolerance @p radius.
 *
 * Tests @p pos for proximity to each band centre:
 *  - @c |pos @c - @c 0x130| @c < @c radius: returns @c 0.
 *  - @c |pos @c - @c 0x90|  @c < @c radius: returns @c 1.
 *  - otherwise:                              returns @c -1.
 *
 * @param pos     Position to classify.
 * @param radius  Half-width tolerance for each band.
 * @return Band index @c 0 or @c 1, or @c -1 if @p pos is outside both bands.
 */
s32 func_800BDAE4(s32 pos, s32 radius) {
    s32 result = -1;
    if (ABS(pos - 0x130) < radius) {
        result = 0;
    } else if (ABS(pos - 0x90) < radius) {
        result = 1;
    }
    return result;
}


/**
 * @brief Snapshot an 8-byte payload from @p input, nudge its halfword
 *        at @c +2 by @c ±0x80, then dispatch to @c func_800AC0A0 with
 *        a mode-derived marker.
 *
 * Steps:
 *  1. Pick a marker from @p mode — @c 0x10 / @c 0x11 / @c 0x12 for
 *     @c mode @c == @c 0 / @c 1 / anything else.
 *  2. @c memcpy 8 bytes from @c input+0x14 to a local @c buf
 *     (byte-granular, matches the target's @c lwl/lwr semantics).
 *  3. Add @c +0x80 (when @p flag is non-zero) or @c -0x80 to the s16
 *     at @c buf+2, then store back.
 *  4. Forward @c (marker, @p input, @c &buf, @c 0) to @c func_800AC0A0.
 *
 * The @c do{}while(0) wrapper around steps 2–3 and the ternary chain
 * for the marker are both load-bearing for matching gcc 2.8.0's
 * scheduling — the do-while(0) keeps the payload work in one basic
 * block so @c v1 stays live as the input-pointer save across the
 * dispatch, and the ternary collapses to the right two-branch jump
 * pattern (target's @c bne / @c j / @c addiu sequence) instead of the
 * extra basic blocks an @c if-else if chain would emit.
 */
void func_800BDB5C(u8 *input, s32 flag, s32 mode) {
    s32 marker;
    u8 buf[8];
    s32 val;

    marker = (mode == 0) ? 0x10 : (mode == 1) ? 0x11 : 0x12;

    do {
        memcpy(buf, input + 0x14, 8);
        val = *(s16 *)(buf + 2);
        if (flag != 0) {
            val += 0x80;
        } else {
            val -= 0x80;
        }
        *(s16 *)(buf + 2) = val;
    } while (0);

    func_800AC0A0(marker, (VECTOR *)input, (SVECTOR *)buf, 0);
}


/**
 * @brief Emit @c unitCount*8 randomized samples around the @p target via
 *        @c func_800AC0A0 (marker @c 2).
 *
 * Initializes an @c SVECTOR scratch to zero, then loops @c unitCount*8
 * times: each iteration shifts @c vy by @c i*0x200, perturbs both @c vy
 * and @c vz by two consecutive @c func_8009CC3C draws (each biased by
 * @c -0x80), and dispatches @c (2, target, &buf, 3) to the AC0A0 emitter.
 *
 * @param target Input pointer forwarded to each AC0A0 emit (a0 of caller).
 * @param unitCount Outer multiplier — total iterations are @c unitCount*8.
 */
void func_800BDBE0(u8 *target, s32 unitCount) {
    SVECTOR buf;
    s32 i;
    s32 totalIters;
    memset(&buf, 0, 8);
    totalIters = unitCount * 8;
    for (i = 0; i < totalIters; i++) {
        s32 r1, r2;
        buf.vy += i * 0x200;
        r1 = func_8009CC3C();
        buf.vy += (r1 - 0x80) * 8;
        r2 = func_8009CC3C();
        buf.vz += (r2 - 0x80) * 4;
        func_800AC0A0(2, (VECTOR *)target, &buf, 3);
    }
}


/**
 * @brief Visibility-gated double emit: snapshot @p input+0x14 twice and
 *        dispatch each as marker @c 0xE to @c func_800AC0A0, displaced
 *        @c ±0x320 in @c vy and perturbed by random draws.
 *
 * Early-rejects when @c func_8009CC3C() returns @c >= @c D_800C4D4C @c * @c 2
 * (so the spawn is rejected when no longer within visible range).
 *
 * Each emit:
 *  1. memcpy 8 bytes from @c input+0x14 into a stack scratch (the
 *     unaligned @c lwl/lwr/swl/swr matches the original source's byte-pointer
 *     pattern).
 *  2. Offset @c scratch[+2] by @c ±0x320 (forward / backward of base).
 *  3. Two @c func_8009CC3C draws perturb the @c vy / @c vz fields
 *     (each biased by @c -0x80, scaled by @c 8 and @c 4 respectively).
 *  4. @c func_800AC0A0(0xE, input, scratch, 3) dispatches the result.
 */
void func_800BDC94(SlotEntry *input) {
    SVECTOR buf;
    s32 r;
    s32 threshold = D_800C4D4C * 2;
    if (func_8009CC3C() >= threshold) return;
    buf = input->vec;
    buf.vy += 0x320;
    r = func_8009CC3C();
    buf.vy += (r - 0x80) * 8;
    r = func_8009CC3C();
    buf.vz += (r - 0x80) * 4;
    func_800AC0A0(0xE, &input->position, &buf, 3);
    buf = input->vec;
    buf.vy -= 0x320;
    r = func_8009CC3C();
    buf.vy += (r - 0x80) * 8;
    r = func_8009CC3C();
    buf.vz += (r - 0x80) * 4;
    func_800AC0A0(0xE, &input->position, &buf, 3);
}

/**
 * @brief Sibling of @c func_800BDC94 — same @c ±0x320 vy-displaced double
 *        emit pattern, but with a @c 4x visibility threshold and marker
 *        @c 3 / last-arg @c 3.
 *
 * Early-rejects when @c func_8009CC3C() returns @c >= @c D_800C4D4C @c * @c 4.
 * Otherwise emits twice with the same snapshot + perturb pipeline. See
 * @c func_800BDC94 for the per-emit body details.
 */
void func_800BDDC4(SlotEntry *input) {
    SVECTOR buf;
    s32 r;
    s32 threshold = D_800C4D4C * 4;
    if (func_8009CC3C() >= threshold) return;
    buf = input->vec;
    buf.vy += 0x320;
    r = func_8009CC3C();
    buf.vy += (r - 0x80) * 8;
    r = func_8009CC3C();
    buf.vz += (r - 0x80) * 4;
    func_800AC0A0(3, &input->position, &buf, 3);
    buf = input->vec;
    buf.vy -= 0x320;
    r = func_8009CC3C();
    buf.vy += (r - 0x80) * 8;
    r = func_8009CC3C();
    buf.vz += (r - 0x80) * 4;
    func_800AC0A0(3, &input->position, &buf, 3);
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BDEF4);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BE040);


/**
 * @brief Fetch the @c field52 halfword of the lookup target for slot @p idx.
 *
 * Uses the slot's @c lookupIdx to index into @c D_800DDB00, following the
 * resulting pointer to the target struct and reading its @c field52.
 * Returns -1 if @p idx is negative or the slot isn't mapped (lookupIdx < 0).
 *
 * @param idx Slot index into @c D_800DBFB8.
 * @return @c field52 of the target, or -1 when the slot has no lookup.
 */
s32 func_800BE158(s32 idx) {
    s32 result = -1;
    if (idx >= 0) {
        s8 lookup = D_800DBFB8[idx].lookupIdx;
        if (lookup >= 0) {
            result = D_800DDB00[lookup]->field52;
        }
    }
    return result;
}

/**
 * @brief Visibility-gated single emit with a caller-supplied threshold and a
 *        @c -0x140 displacement on @p input's field-4 word.
 *
 * Sibling of @c func_800BDC94. Early-rejects when
 * @c func_8009CC3C() @c >= @c threshold. Otherwise snapshots @c input's
 * @c position (16-byte aligned @c lw/@c sw) and @c vec (8-byte unaligned
 * @c lwl/@c lwr), drops @c position.vy by @c 0x140, perturbs @c vec.vy
 * and @c vec.vz with two @c func_8009CC3C draws (each biased by @c -0x80
 * and scaled by @c 8 / @c 4 respectively), then dispatches to
 * @c func_800AC0A0 with marker @c marker+4.
 */
void func_800BE1A8(SlotEntry *input, s32 marker, s32 threshold) {
    SVECTOR vec;
    VECTOR pos;
    s32 r;
    if (func_8009CC3C() >= threshold) return;
    vec = input->vec;
    pos = input->position;
    pos.vy -= 0x140;
    r = func_8009CC3C();
    vec.vy += (r - 0x80) * 8;
    r = func_8009CC3C();
    vec.vz += (r - 0x80) * 4;
    func_800AC0A0(marker + 4, &pos, &vec, 3);
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BE284);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BE3D0);

/**
 * @brief Linearly interpolate between two 3D vectors.
 *
 * Computes out[i] = b[i] + (a[i] - b[i]) * t / 4096 for each of 3
 * components, using fixed-point arithmetic with rounding toward zero.
 *
 * @param a Source vector (3 x s32).
 * @param b Base vector (3 x s32).
 * @param t Interpolation factor (s16, fixed-point 12-bit).
 * @param out Output vector (3 x s32). If NULL, no operation.
 */
void func_800BE4D8(s32 *a, s32 *b, s32 t, s32 *out) {
    s32 diff;
    s32 val;
    s16 frac;

    if (out == 0) {
        return;
    }

    diff = a[0] - b[0];
    frac = t;
    val = diff * frac;
    if (val < 0) {
        val += 0xFFF;
    }
    out[0] = b[0] + (val >> 12);

    val = (a[1] - b[1]) * frac;
    if (val < 0) {
        val += 0xFFF;
    }
    out[1] = b[1] + (val >> 12);

    val = (a[2] - b[2]) * frac;
    if (val < 0) {
        val += 0xFFF;
    }
    out[2] = b[2] + (val >> 12);
}


/**
 * @brief Convert a direction vector to (pitch, yaw, 0) Euler angles.
 *
 * Treats @p dir as a 3D direction (PSX y-down convention) and writes
 * the equivalent rotation into @p out:
 *  - @c out->vx (pitch) = @c ratan2(-vy, @c sqrt(vz*vz + vx*vx))
 *  - @c out->vy (yaw)   = @c ratan2(vx, vz)
 *  - @c out->vz         = @c 0
 *
 * Uses the PsyQ GTE helpers @c SquareRoot0 (integer sqrt) and
 * @c ratan2 (returns an angle in the standard PSX 0x1000 = full-circle
 * unit).
 *
 * @param dir Input direction vector.
 * @param out Destination Euler angles (vz is cleared).
 */
void func_800BE578(SVECTOR *dir, SVECTOR *out) {
    s32 horizDist = SquareRoot0(dir->vz * dir->vz + dir->vx * dir->vx);
    out->vx = ratan2(-dir->vy, horizDist);
    out->vy = ratan2(dir->vx, dir->vz);
    out->vz = 0;
}


/**
 * @brief Walk a table of Records to the first -1/-2 marker and return context.
 *
 * Advances through a stride-0x20 record table while the current
 * @c marker is neither -1 nor -2. When a marker is found:
 *   - If @c marker == -2 (continuation), returns the *next* record's
 *     @c marker minus 1 — likely a link/count follow.
 *   - Otherwise (marker == -1, end-of-list), returns -2 as a sentinel.
 *
 * @param rec Pointer into the Record table.
 * @return Adjusted continuation value, or -2 if terminator was hit.
 */
s32 func_800BE5F8(Record *rec) {
    s16 val;
    while (rec->marker != -1 && rec->marker != -2) {
        rec++;
    }
    val = rec->marker;
    if (val == -2) {
        rec++;
        return rec->marker - 1;
    }
    return -2;
}

/**
 * @brief Switch the @c D_800DCE78 subsystem between save and load modes.
 *
 * Dispatches based on @p reset:
 *  - Non-zero @c reset: rebuild the @c D_800DCE78 state from scratch by
 *    calling @c func_800BAC84 (init pass) followed by @c func_800BEFEC
 *    (ramp/table fill). Returns @c 0.
 *  - Zero @c reset: query @c func_800BA870 against the current state and
 *    forward its return value.
 *
 * @param reset Non-zero rebuilds @c D_800DCE78; zero queries it instead.
 * @return @c 0 in the rebuild path; @c func_800BA870's return value
 *         otherwise.
 */
s32 func_800BE63C(s32 reset) {
    s32 result = 0;
    if (reset != 0) {
        func_800BAC84(D_800DCE78);
        func_800BEFEC(D_800DCE78);
    } else {
        result = func_800BA870(D_800DCE78);
    }
    return result;
}



/**
 * @brief Snapshot each entry's @c curr fields into @c prev and invoke
 *        the per-entry handler.
 *
 * Walks @p group->entries for @c group->count iterations. Before
 * dispatching each entry to @c func_800AEB58:
 *  - @c entry->prevHalf is loaded with @c entry->currHalf.
 *  - @c entry->prevWord is loaded with @c entry->currWord.
 *
 * @c group->count is re-read each iteration since @c func_800AEB58 may
 * mutate it (the cached @c i is compared against the fresh count).
 *
 * @param group Transform-entry group container.
 */
void func_800BE69C(XformGroup *group) {
    XformEntry *entry = group->entries;
    s32 i = 0;
    if (group->count > 0) {
        do {
            i++;
            entry->prevHalf = entry->currHalf;
            entry->prevWord = entry->currWord;
            func_800AEB58(entry, group);
            entry++;
        } while (i < group->count);
    }
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BE720);



/**
 * @brief Load each unique object referenced by an object-id list.
 *
 * Walks the @c ids array of @p objList (up to @c OBJ_SLOT_COUNT entries,
 * stopping at the @c OBJ_LIST_END terminator). Ids below @c FIRST_OBJ_ID are
 * skipped, and an id that already appeared earlier in the list is skipped too,
 * so each distinct object is handled exactly once. For every new id, its data
 * offset is read from @p section's leading @c u32 offset table (keyed by
 * @c id @c - @c FIRST_OBJ_ID) and the record at that offset is dispatched to
 * @c func_8009C5FC.
 *
 * @param objList Object-id list (header + id array).
 * @param section Section base; starts with the @c u32 offset table and is the
 *                base that the looked-up byte offsets are relative to.
 * @return @c s32 to match the engine dispatch signature; no value is set.
 */
s32 func_800BE7FC(WorldObjList *objList, u8 *section) {
    s32 i;
    s32 j;
    u32 id;
    u32 off;
    u8 *cur;

    i = 0;
    cur = objList->header;
loop:
    id = cur[sizeof(objList->header)];
    if (id == OBJ_LIST_END) {
        goto done;
    }
    if (id < FIRST_OBJ_ID) {
        goto next;
    }
    /* skip ids already handled earlier in the list */
    for (j = 0; j < i; j++) {
        if (objList->ids[j] == id) {
            goto next;
        }
    }
    off = ((u32 *)section)[cur[sizeof(objList->header)] - FIRST_OBJ_ID];
    func_8009C5FC((s32 *)(section + off));
next:
    i++;
    cur++;
    if (i < OBJ_SLOT_COUNT) {
        goto loop;
    }
done:;
}


/**
 * @brief Pick a rotation source from a one-byte selector, copy/negate via
 *        @c func_800BC51C, then mirror-and-project via @c func_800BC544
 *        and @c worldPosToCell.
 *
 * Reads a byte from @p byteIn and uses it to choose a rotation source:
 *  - @c 1: @c &D_800DD6C0->vecA
 *  - @c 2 or @c 3: @c &D_800DD6C0->vecB
 *  - @c 4: @c &D_800DD6E4->vecB
 *  - anything else: skip the rotation copy
 *
 * The selected source is fed through @c func_800BC51C(src, transform)
 * (swap-Y/Z and negate Z). Then @c func_800BC544(transform, &buf) does
 * the mirror copy into a scratch vector, and @c worldPosToCell(&buf, output)
 * projects/dispatches the final result.
 */
void func_800BE8B0(u8 *byteIn, VECTOR *transform, u8 *output) {
    u8 byte = *byteIn;
    VECTOR buf;
    if (byte == 1) {
        func_800BC51C(&D_800DD6C0->vecA, transform);
    } else if (byte == 2 || byte == 3) {
        func_800BC51C(&D_800DD6C0->vecB, transform);
    } else if (byte == 4) {
        func_800BC51C(&D_800DD6E4->vecB, transform);
    }
    func_800BC544(transform, &buf);
    worldPosToCell(&buf, output);
}


/**
 * @brief Walk the @c D_800C9E34 script and return the last
 *        @c 0xFF15 opcode's @c param (when @c D_800C4D64->flag has
 *        bit 3 set), else @c -1.
 *
 * Steps:
 *  1. If @c D_800C4D64->flag bit 3 isn't set, returns @c -1.
 *  2. Calls @c func_800AF004(@c D_800C9E34, 0) — if no script, returns
 *     @c -1.
 *  3. Walks the script, treating opcodes as:
 *     - @c 0xFF05 (END): stop walking.
 *     - @c 0xFF0E (JUMP): follow the @c param-relative jump.
 *     - @c 0xFF15: snapshot the @c param into @c result.
 *     - other: skip to next op.
 *
 * Returns the last @c 0xFF15 param seen, or @c -1 if none.
 */
s32 func_800BE958(void) {
    s32 result = -1;
    ScriptOp *p;

    if (D_800C4D64->flag & 0x8) {
        p = func_800AF004(D_800C9E34, 0);
        if (p != 0) {
            while (1) {
                u16 op = p->op;
                if (op == 0xFF05) break;
                if (op == 0xFF0E) {
                    p = (ScriptOp *)(D_800C9E34 + p->param);
                    continue;
                }
                if (op == 0xFF15) {
                    result = p->param;
                }
                p++;
            }
        }
    }
    return result;
}


/**
 * @brief Dispatch to one of two kind-2 handlers based on sign of @p arg.
 *
 * If @p arg is non-negative, calls @c func_8009B358(2, arg, 0).
 * Otherwise calls @c func_8009D8A8(2) with no argument beyond the kind.
 *
 * @note Purpose uncertain — appears to route a slot index / handle to a
 *       "present" handler when valid, and a "missing" handler when -1.
 *
 * @param arg Slot index (>=0) or sentinel (<0) indicating absence.
 */
void func_800BE9F8(s32 arg) {
    if (arg >= 0) {
        func_8009B358(2, arg, 0);
    } else {
        func_8009D8A8(2);
    }
}


/**
 * @brief Tear down the battle scene: clear state flag, fade SFX, render two frames.
 *
 * Typical end-of-battle / scene-transition cleanup sequence:
 *  1. @c DrawSync(0) — system/display reset.
 *  2. Clear the @c D_800C5D54 flag.
 *  3. @c fadeOutSfxFast(0) — stop channel 0 SFX.
 *  4. @c renderAndUpdateDisplay(2) — push 2 frames.
 *  5. Flush the battle scene's colorTag into the display list.
 */
void func_800BEA34(void) {
    DrawSync(0);
    D_800C5D54 = 0;
    fadeOutSfxFast(0);
    renderAndUpdateDisplay(2);
    renderBattleDisplayList(&D_800D244C->primList[BSC_COLORTAG_IDX]);
}


/**
 * @brief Pick a per-map screen/mode constant keyed off @c D_800C4D38.
 *
 * Dispatches based on the current map id @c D_800C4D38:
 *  - @c mapId @c < @c 0xA or @c == @c 0x80:
 *      - @c D_800C5C1C @c > @c 0: returns @c 0x31.
 *      - otherwise:               returns @c 0x20.
 *  - @c mapId @c == @c 0x31:                          returns @c 0x20.
 *  - @c mapId in @c [0x20, @c 0x29) or @c == @c 0x84: returns @c 0x32.
 *  - @c mapId @c == @c 0x32:                          returns @c 0.
 *  - otherwise:                                       returns @c -1.
 *
 * Constants look like screen-format / camera-mode selectors; the exact
 * semantics aren't mapped yet.
 *
 * @return Per-map constant — @c 0x31, @c 0x32, @c 0x20, @c 0, or @c -1.
 */
s32 func_800BEA7C(void) {
    s32 mapId = D_800C4D38;

    if ((u32)mapId < 0xA || mapId == 0x80) {
        if (D_800C5C1C > 0) return 0x31;
        return 0x20;
    }
    if (mapId == 0x31) return 0x20;
    if ((u32)(mapId - 0x20) < 9 || mapId == 0x84) return 0x32;
    if (mapId == 0x32) return 0;
    return -1;
}


/**
 * @brief Check if @p val matches any of the 7 entries in the
 *        @c D_800C5C18..D_800C5C30 slot table.
 *
 * Returns @c 0 immediately on negative @p val. Otherwise compares
 * against each of the 7 named slot globals (@c D_800C5C18,
 * @c D_800C5C1C, @c D_800C5C20, @c D_800C5C24, @c D_800C5C28,
 * @c D_800C5C2C, @c D_800C5C30) and returns @c 1 on first match,
 * @c 0 if none.
 *
 * @param val Slot identifier to test.
 * @return @c 1 if @p val is in the slot table, @c 0 otherwise.
 */
s32 func_800BEAF4(s32 val) {
    s32 result;
    if (val < 0) return 0;
    result = 0;
    if (D_800C5C18 == val) result = 1;
    else if (D_800C5C1C == val) result = 1;
    else if (D_800C5C20 == val) result = 1;
    else if (D_800C5C24 == val) result = 1;
    else if (D_800C5C28 == val) result = 1;
    else if (D_800C5C2C == val) result = 1;
    else if (D_800C5C30 == val) result = 1;
    return result;
}

/**
 * @brief Compute a per-actor action code from the marker at
 *        @c D_800D23D8[idx+2] crossed with the actor's @c flag1E.
 *
 * Looks up two pieces of per-actor state and combines them:
 *  - marker = @c D_800D23D8[idx @c + @c 2] (byte from the world-state
 *    flag buffer).
 *  - flag = @c D_800DD6A8[idx].flag1E (s8 in the actor record).
 *
 * Returns:
 *  - marker @c == @c 0 and flag @c == @c 1: @c 0x35.
 *  - marker @c == @c 1 and flag @c == @c 1: @c 0x34.
 *  - marker @c == @c 1 and flag @c == @c -1: @c 0x37.
 *  - any other combination: @c -1.
 *
 * @param idx Actor index.
 * @return Action code, or @c -1 if no rule matches.
 */
s32 func_800BEB84(s32 idx) {
    s32 result = -1;
    u8 marker = D_800D23D8[idx + 2];

    if (marker == 0) {
        if (D_800DD6A8[idx].flag1E == 1) {
            result = 0x35;
        }
    } else if (marker == 1) {
        s8 v = D_800DD6A8[idx].flag1E;
        if (v == 1) {
            result = 0x34;
        } else if (v == -1) {
            result = 0x37;
        }
    }
    return result;
}

/**
 * @brief Map a command-kind code to a duration/timeout in frames.
 *
 * Lookup table flattened into a small branch chain:
 *   - kind 50:              528 frames
 *   - kind 32..40:          256 frames
 *   - kind 132:             256 frames
 *   - kind 48:              700 frames
 *   - anything else:        0
 *
 * @param kind Command type byte.
 * @return Frame count for the command, or 0 if unrecognised.
 */
s32 func_800BEC1C(s32 kind) {
    if (kind == 50) return 528;
    if ((kind >= 32 && kind <= 40) || kind == 132) return 256;
    if (kind == 48) return 700;
    return 0;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BEC60);

/**
 * @brief Search g_gameState array for an active entry with type 0xA2.
 */
s32 func_800BED90(s32 *outIndex, s32 *outCount) {
    s32 found = 0;
    s32 i = found;
    s32 target = 0xA2;
    u8 *ptr = (u8 *)&g_gameState;
    s32 type;
    s32 count;

    top:
        type = *(u8 *)(ptr + 0xB44);
        count = *(u8 *)(ptr + 0xB45);
        if (type != target) goto skip;
        if (count <= 0) goto skip;
        found = 1;
        if (outIndex != 0) {
            *outIndex = i;
        }
        if (outCount != 0) {
            *outCount = count;
            goto end;
        }
        goto end;
    skip:
        i += 1;
        if (i < 0xC6) {
            ptr += 2;
            goto top;
        }
    end:
    return found;
}


/**
 * @brief Scan the @c D_800C96D0 script for non-control opcodes and snapshot
 *        each one's low @c param byte into @p output; gate the result with
 *        the @c D_800C4DA8..@c D_800C4DB4 lock quad.
 *
 * Sets all four lock slots to @c 1 on entry, then walks the script returned
 * by @c func_800AF004(D_800C96D0, 0):
 *  - @c 0xFF05: stop walking.
 *  - @c 0xFF0E: follow the @c param-relative jump.
 *  - other: write the opcode's low @c param byte to @c *output and continue.
 *
 * Clears all four lock slots to @c 0 on the way out. Returns @c 1 if any
 * non-control opcode was processed, else @c 0.
 *
 * @note Each non-control hit overwrites @c *output, so callers see only the
 *       last byte the script emitted before the @c 0xFF05 terminator.
 */
s32 func_800BEDF0(u8 *output) {
    s32 found = 0;
    ScriptOp *p;
    D_800C4DA8 = 1;
    D_800C4DAC = 1;
    D_800C4DB0 = 1;
    D_800C4DB4 = 1;
    p = func_800AF004(D_800C96D0, 0);
    if (p != 0) {
        while (1) {
            u16 op = p->op;
            if (op == 0xFF05) break;
            if (op == 0xFF0E) {
                p = (ScriptOp *)(D_800C96D0 + p->param);
                continue;
            }
            found = 1;
            *output = (u8)p->param;
            p++;
        }
    }
    D_800C4DA8 = 0;
    D_800C4DAC = 0;
    D_800C4DB0 = 0;
    D_800C4DB4 = 0;
    return found;
}

void func_800BEECC(void) {
}


/**
 * @brief When dialogue-mode 0xD is pending and the current scene is active,
 *        post the next dialogue chunk via @c func_800A358C and reflect the
 *        result in the @c D_800C4DC4 ready flag.
 *
 * Always sets @c D_800C4DC4 to 1 on entry (optimistic ready). Then early-outs
 * unless @c D_800D23D8[0] is mode 0xD and @c D_800C4DC0 is non-zero (scene
 * has a valid context). Looks up the current slot via @c D_800C5C24 into
 * @c D_800DBFB8 and calls @c func_800A358C(0x32, &slot, slot.data14, 0). On
 * success (@c ret @>= @c 0) @c D_800C4DC4 stays 1; on failure it's reset to
 * 0 and the mode byte at @c D_800D23D8[0] is cleared.
 *
 * @note The declared @c s32 return type is a matching artifact, not a
 *       meaningful API: the original C declared a return type but used
 *       bare @c return; (and an implicit fall-off at the end), leaving @c v0
 *       as whatever the last computation produced. As a result the
 *       "return value" is path-dependent garbage:
 *           - mode != 0xD path → @c 0xD (residue of the bne compare)
 *           - @c D_800C4DC0 == 0 path → @c 0 (the loaded value)
 *           - success path → @c 1 (the success boolean)
 *           - failure path → @c 0 (the success boolean)
 *       Callers should treat this as effectively @c void. The non-void
 *       declaration is what forces gcc 2.8.0 to schedule the
 *       @c %hi(D_800DBFB8) load into the @c beqz delay slot, matching
 *       the original byte layout — a @c void declaration plateaus at
 *       99.47% with the two %hi lui's swapped.
 */
s32 func_800BEED4(void) {
    s32 ret;

    D_800C4DC4 = 1;
    if (D_800D23D8[0] != 0xD) return;
    if (D_800C4DC0 == 0) return;

    ret = func_800A358C(0x32, &D_800DBFB8[D_800C5C24], &D_800DBFB8[D_800C5C24].vec, 0);
    D_800C4DC4 = (ret >= 0);
    if (!D_800C4DC4) {
        D_800D23D8[0] = 0;
    }
}

/**
 * @brief Return 1 if any active slot's marker is 0x4F, else 0.
 *
 * Scans the first @c D_800C5B50 entries of @c D_800DBFB8 for a slot
 * whose @c marker byte equals 0x4F. Early-exits on first hit.
 *
 * @return 1 if a 0x4F-marker slot exists, 0 otherwise.
 */
s32 func_800BEF6C(void) {
    SlotEntry *entry = &D_800DBFB8[0];
    SlotEntry *end = entry + D_800C5B50;
    if (entry < end) {
        do {
            if (entry->marker == 0x4F) return 1;
            entry++;
        } while (entry < end);
    }
    return 0;
}

/** Checks two flag bits and returns status. */
s32 func_800BEFC4(void) {
    u8 val = g_chocoboWorld;
    s32 result = 0;
    if (val & 1) {
        s32 bit = val & 2;
        result = (u32)bit < 1;
    }
    return result;
}

/**
 * @brief Seed the eight records at @p base with a descending ramp.
 *
 * Each record is @c 0x4C bytes and carries five @c s32 at offsets @c 0x38
 * through @c 0x48. They are written back to front, @c 0x48 first, starting at
 * @c -0x1000 and rising by @c 0x400 a step, so in address order the five read
 * @c 0, @c -0x400, @c -0x800, @c -0xC00, @c -0x1000.
 *
 * @param base First record; the reset path passes @c D_800DCE78.
 * @note The record is still reached by raw offset rather than a struct, so the
 *       0x4C stride and the 0x38..0x48 field range are not yet named.
 */
static void func_800BEFEC(u8 *base) {
    s32 outer = 7;
    s32 inner;
    s32 val;
    s32 *ptr;
    do {
        inner = 4;
        val = -0x1000;
        ptr = (s32 *)(base + 0x10);
        do {
            *(s32 *)((u8 *)ptr + 0x38) = val;
            val += 0x400;
            inner--;
            ptr = (s32 *)((u8 *)ptr - 4);
        } while (inner >= 0);
        outer--;
        base += 0x4C;
    } while (outer >= 0);
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BF024);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BF20C);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object10", func_800BF2E8);


