#include "common.h"
#include "psxsdk/libc.h"
#include "gamestate.h"
#include "world.h"
#include "world/we_object5.h"
#include "world/we_object9.h"
#include "world/we_object3.h"
#include "world/we_object1.h"
#include "world/we_object6.h"



#define SPAWN_FLAG_LIFETIME_JITTER 1
#define SPAWN_FLAG_SIZE_JITTER     2

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object9", func_800BA870);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object9", func_800BAC84);

/*
 * func_800BB150: message dispatch with counter + template expansion.
 *
 * Behavior:
 *  1. Decrement @c D_800C5D84 timer (conditional on @c D_800C4D4C), or call
 *     @c func_8009D8A8(1) when the timer hit zero.
 *  2. Gate on @c D_800C4D38 being in [0x22, 0x28] or == 0x20.
 *  3. Early-return if @c D_800D23D8 flag is set.
 *  4. If @c entity->field70 > 0x1FFFF: scan inventory for an item with id
 *     @c 0xA2 and non-zero count; decrement that count, then either expand
 *     a message template and emit it (@c func_8009B358) or emit a
 *     placeholder.
 *  5. Otherwise: call @c func_800BED90 and apply @c func_800BD7A4 scaling.
 *
 * The reference C below — written as a 1998 author would have — matches
 * at ~77% (base score ~4860, after permuter exploration ~2300). Remaining
 * gaps are gcc basic-block ordering and stack-frame allocation choices
 * that don't yield to clean source-level refactoring. Kept as INCLUDE_ASM
 * until a non-hacky approach is found.
 *
 *   void func_800BB150(Entity *entity) {
 *       s32 cmd, i;
 *       ItemSlot *items;
 *
 *       // Decrement message-display timer; reinit when it expires.
 *       if (D_800C5D84 > 0) {
 *           if (D_800C4D4C != 0) D_800C5D84--;
 *       } else {
 *           func_8009D8A8(1);
 *       }
 *
 *       // Only run for valid command types.
 *       cmd = D_800C4D38;
 *       if ((u32)(cmd - 0x22) >= 7 && cmd != 0x20) return;
 *       if (D_800D23D8 != 0) return;
 *
 *       if (entity->field70 > 0x1FFFF) {
 *           func_800B00D8(D_800DBFB8[D_800C5C18].marker);
 *
 *           // Find first inventory slot holding item 0xA2.
 *           items = g_gameState.mainData.itemSlots;
 *           D_800DBFA4 = -1;
 *           D_800DBFA8 = 0;
 *           for (i = 0; i < ITEM_SLOT_COUNT; i++) {
 *               if (items[i].id == 0xA2 && (s8)items[i].count > 0) {
 *                   D_800DBFA4 = i;
 *                   items[i].count--;
 *                   D_800DBFA8 = items[i].count;
 *                   break;
 *               }
 *           }
 *
 *           if (D_800DBFA8 > 0) {
 *               u8 *itemName = D_800DCC78;
 *               u8 *qtyStr   = D_800DCCF8;
 *               u8 *msgBuf   = D_800DCB78;
 *               TemplateTable *tpl;
 *               u8 *src;
 *               s32 j;
 *
 *               // Build the two substitution strings.
 *               itemName[0] = 0;
 *               func_800BCE74(itemName, 0xA2);
 *               qtyStr[0] = 0;
 *               func_800BC974(qtyStr, D_800DBFA8);
 *               msgBuf[0] = 0;
 *
 *               // Walk the template, expanding 0x0A-prefixed substitutions.
 *               tpl = D_800C9FE8;
 *               src = (u8 *)tpl + ((s32 *)tpl)[0];
 *               j = 0;
 *               while (src[j] != 0x0A || src[j + 1] != 0xFF) {
 *                   if (src[j] == 0x0A) {
 *                       j++;
 *                       if (src[j] >= 0x20) {
 *                           s32 code = src[j] - 0x20;
 *                           if (code == 0 && itemName[0] != 0)
 *                               strcat(msgBuf, itemName);
 *                           else if (code == 1 && qtyStr[0] != 0)
 *                               strcat(msgBuf, qtyStr);
 *                       }
 *                   } else {
 *                       // Append a glyph from the global character table.
 *                       u8 c = src[j];
 *                       StringTable *chtable = D_800C97D4;
 *                       u8 *charStr = (u8 *)chtable + chtable->first[c];
 *                       s32 len = 0, k;
 *                       if (msgBuf[0] != 0) {
 *                           do { len++; } while (msgBuf[len] != 0);
 *                       }
 *                       for (k = 0; (msgBuf[len] = charStr[k]) != 0; k++, len++);
 *                   }
 *                   j++;
 *               }
 *
 *               func_8009B358(1, -2, msgBuf);
 *           } else {
 *               func_8009B358(1, 0x2B, NULL);
 *           }
 *
 *           entity->field70 = 0;
 *           D_800C5D84 = 30;
 *       } else if (func_800BED90(0, 0) != 0) {
 *           entity->field70 += func_800BD7A4(D_800C4D38, D_800C4D4C);
 *       }
 *   }
 */
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object9", func_800BB150);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object9", func_800BB4E8);

/**
 * @brief Spawn a mirrored particle pair (type 0x13) at ±0x320 angle offsets if visible.
 *
 * Reads world camera angle from D_800C97F4 and computes angular delta dx to the
 * source's position.half. If (rnd + dx/4) < visRange AND |dx| < 0x258, emits two
 * particles via func_800AC0A0 with symmetric angle offsets and per-axis RNG jitter.
 * Overwrites the local copy's 4-byte pos slot with the current world camera word
 * before each spawn, so emitted particles share the world's current frame.
 *
 * @param src Particle source (position descriptor + velocity template).
 * @param visRange Visibility threshold (higher = farther allowed).
 */
void func_800BBD74(ParticleSource *src, s32 visRange) {
    SVECTOR localVel;
    VECTOR  localPos;
    s32      jitter;
    s32      absdx;
    s16      dx;
    u32      worldCam;

    worldCam = D_800C97F4;
    localPos = src->pos;
    dx       = ((u16) D_800C97F4) - (u16)src->pos.vy;
    localPos.vy = worldCam;
    if ((func_8009CC3C() + (dx >> 2)) >= visRange) return;
    absdx = (dx >= 0) ? dx : -dx;
    if (absdx >= 0x258) return;

    /* Left-side spawn: +0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.vy += 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.vy += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.vz += jitter;
    func_800AC0A0(0x13, &localPos, &localVel,
                  SPAWN_FLAG_SIZE_JITTER | SPAWN_FLAG_LIFETIME_JITTER);

    /* Right-side spawn: -0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.vy -= 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.vy += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.vz += jitter;
    func_800AC0A0(0x13, &localPos, &localVel,
                  SPAWN_FLAG_SIZE_JITTER | SPAWN_FLAG_LIFETIME_JITTER);
}

/*
 * func_800BBF0C: mirrored particle spawn (type 0xF), near-match.
 *
 * Sibling of @c func_800BBD74 but uses @c D_800C4D4C as threshold (vs
 * caller-supplied visRange), @c 0x1A4 as absdx bound, type @c 0xF, and a
 * simpler height jitter (just @c rand - 0x80, no @c *4 scaling).
 *
 * The reference C below produces 102-instruction asm that differs from the
 * target at two places (~95.8% match): gcc 2.8.0's scheduler chooses to
 * emit @c addiu -0x80 on the @c v0 (rand) register rather than on the
 * @c v1 (height) register. Algebraically identical (@c height - 0x80 +
 * rand vs @c height + (rand - 0x80)) but one `addiu`/`addu` instruction
 * pair swaps sides. No C-level expression I've found flips the scheduler's
 * associativity choice — this is a deep scheduling/RTL pass artifact.
 * Permuter best score is 140 over thousands of iterations; kept as
 * INCLUDE_ASM with this reference C until a cleaner fix is found.
 *
 *   void func_800BBF0C(ParticleSource *src) {
 *       Velocity localVel;
 *       PosDesc  localPos;
 *       s32      jitter;
 *       s32      absdx;
 *       s16      dx;
 *       u32      worldCam;
 *
 *       worldCam = D_800C97F4;
 *       localPos = src->pos;
 *       dx       = ((u16) D_800C97F4) - (u16)src->pos.vy;
 *       localPos.vy = worldCam;
 *       if (D_800C4D4C <= (func_8009CC3C() + (dx >> 2))) return;
 *       absdx = (dx >= 0) ? dx : -dx;
 *       if (absdx >= 0x1A4) return;
 *
 *       localVel = src->vel;
 *       localVel.vy += 0x320;
 *       jitter = (func_8009CC3C() - 0x80) * 8;
 *       localVel.vy += jitter;
 *       localVel.vz += func_8009CC3C() - 0x80;
 *       func_800AC0A0(0xF, &localPos, &localVel, 3);
 *
 *       localVel = src->vel;
 *       localVel.vy -= 0x320;
 *       jitter = (func_8009CC3C() - 0x80) * 8;
 *       localVel.vy += jitter;
 *       localVel.vz += func_8009CC3C() - 0x80;
 *       func_800AC0A0(0xF, &localPos, &localVel, 3);
 *   }
 */
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object9", func_800BBF0C);

/**
 * @brief Spawn a mirrored particle pair (type 0x14) gated on a word-angle delta.
 *
 * Sibling of @c func_800BBD74 but with word-scale angle math instead of
 * halfword: computes @c (D_800C97F4 - src->pos.vy) >> 4 and
 * rejects the spawn when @c (rnd + delta) >= @c (D_800C4D4C / 2). When
 * the visibility gate opens, emits two particles via @c func_800AC0A0
 * with symmetric ±0x320 angle offsets and RNG jitter on angle/height.
 *
 * @note Purpose uncertain — differs from @c func_800BBD74 in particle type
 *       (0x14 vs 0x13), delta scaling (word >> 4 vs half >> 2), threshold
 *       source (global @c D_800C4D4C/2 vs caller-supplied @c visRange), and
 *       the absence of an absolute-delta bound check.
 *
 * @param src Particle source (position descriptor + velocity template).
 */
void func_800BC09C(ParticleSource *src) {
    SVECTOR localVel;
    VECTOR  localPos;
    s32      jitter;
    s32      threshold;
    u32      worldCam;

    worldCam = D_800C97F4;
    localPos = src->pos;
    threshold = D_800C4D4C >> 1;
    localPos.vy = worldCam;
    if ((func_8009CC3C() + ((s32)(D_800C97F4 - src->pos.vy) >> 4)) >= threshold) return;

    /* Left-side spawn: +0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.vy += 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.vy += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.vz += jitter;
    func_800AC0A0(0x14, &localPos, &localVel, 3);

    /* Right-side spawn: -0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.vy -= 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.vy += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.vz += jitter;
    func_800AC0A0(0x14, &localPos, &localVel, 3);
}


/*
 * func_800BC218: project a world-space glyph into screen-space and return a
 * clamped half-angle gating visibility. GTE-heavy — sets rotation/translation
 * matrices, applies RotTrans (mvmva sf=1, mx=rot, v=V0, cv=tr), then projects
 * through a secondary world-to-screen matrix at @c D_800C9838.
 *
 * Currently @c INCLUDE_ASM: reaches ~90% near-match under gcc-2.7.2-cdk, but
 * the last 10% needs non-1998 hacks (@c volatile on a dead-store local,
 * @c register @c asm for @c gte_ldtr reg-alloc, @c u8 @c _pad42[14] for
 * stack layout, @c (s32) casts to block symbol+constant folding).
 *
 * Reference C (clean 1998-style, requires the inline GTE macros in
 * @c include/psxsdk/libgte.h):
 *
 *     typedef struct { u8 pad[0xD]; u8 keyLo, keyMid, keyHi; } GlyphHeader;
 *
 *     extern u16    D_800C5C44[];   // per-kind 8-byte entries
 *     extern MATRIX D_800C9838;     // secondary world-to-screen matrix
 *     extern void   RotMatrix(SVECTOR *angles, MATRIX *out);  // = RotMatrix
 *     extern s32    ratan2(s32 y, s32 x);                     // = ratan2
 *     extern void  *memset(void *dst, s32 val, s32 n);        // = memset
 *     extern s32    project(VECTOR *src, VECTOR *out);        // = worldPosToCell
 *     extern GlyphHeader *glyphAt(VECTOR *v, DVECTOR *out);
 *     extern s32    lookupKey(u32 key);                       // = func_800A45D8
 *
 *     s32 func_800BC218(s32 kind, VECTOR *trans, SVECTOR *angles) {
 *         SVECTOR offset;
 *         SVECTOR localOff;
 *         VECTOR  rotated;
 *         VECTOR  projected;
 *         MATRIX  rotation;
 *         DVECTOR screen;
 *         GlyphHeader *glyph;
 *         s16 projY;
 *         s32 dy, angle, clamped;
 *         u32 key;
 *
 *         memset(&offset, 0, sizeof(offset));
 *         offset.vx = (s16)D_800C5C44[(kind - 0x40) * 4] >> 1;
 *         localOff  = offset;
 *
 *         RotMatrix(angles, &rotation);
 *         gte_SetRotMatrix(&rotation);
 *         gte_SetTransVector(trans);
 *         gte_ldv0(&localOff);
 *         gte_mvmva(1, 0, 0, 0, 0);
 *         gte_stlvnl(&rotated);
 *
 *         projY = (s16)project(&rotated, &projected);
 *         gte_ldtr(0, 0, 0);
 *         glyph = glyphAt(&rotated, &screen);
 *
 *         gte_SetRotMatrix(&D_800C9838);
 *         gte_SetTransMatrix(&D_800C9838);
 *
 *         dy = (s16)trans->vy - screen.vx;
 *         if (dy < 0 ? -dy : dy) >= 0xC8) return 0;
 *
 *         key = (u32)glyph->keyLo
 *             | ((u32)glyph->keyMid << 8)
 *             | ((u32)glyph->keyHi  << 16);
 *         if (lookupKey(key) == 0) return 0;
 *
 *         angle   = ratan2(-dy, offset.vx);
 *         clamped = angle >> 1;
 *         if      (clamped < -0x80) clamped = -0x80;
 *         else if (clamped >  0x80) clamped =  0x80;
 *         return (s16)clamped;
 *     }
 */
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object9", func_800BC218);


/**
 * @brief Thin wrapper that forwards to func_800BFFEC with no arguments.
 *
 * @note Purpose uncertain — appears to be a dispatch hook into another
 *       subsystem, possibly for resetting or (re)initializing state
 *       owned by func_800BFFEC's module.
 */
void func_800BC44C(void) {
    func_800BFFEC();
}


/**
 * @brief Walk the cmd-stream at @c D_800C96D0 to extract a halfword into @p out.
 *
 * Gated on @c (D_800C4D64->flag & 0x8): returns 0 immediately if the flag
 * is clear. Calls @c func_800AF004(D_800C96D0, 0) to get a pointer into
 * the stream, then interprets each halfword opcode:
 *   - @c 0xFF05 — end of stream; stop.
 *   - @c 0xFF0E — follow a link: jump to @c (D_800C96D0 + stream[1]).
 *   - otherwise — record @c stream[1] into @c *out, set @c found, advance.
 *
 * Returns 1 if any value was recorded, 0 otherwise.
 */
s32 func_800BC46C(u16 *out) {
    ScriptOp *stream;
    s32 found = 0;

    if ((D_800C4D64->flag & 0x8) == 0) return 0;

    stream = func_800AF004(D_800C96D0, 0);
    if (stream != 0) {
        while (1) {
            if (stream->op == 0xFF05) break;
            if (stream->op == 0xFF0E) {
                stream = (ScriptOp *)(D_800C96D0 + stream->param);
            } else {
                found = 1;
                *out = stream->param;
                stream++;
            }
        }
    }
    return found;
}

/** Copies and negates vector components (rotation variant A). */
void func_800BC51C(VECTOR *src, VECTOR *dst) {
    dst->vx = src->vx;
    dst->vy = -src->vz;
    dst->vz = src->vy;
}

/** Copies and negates vector components (rotation variant B). */
void func_800BC544(VECTOR *src, VECTOR *dst) {
    dst->vx = src->vx;
    dst->vy = src->vz;
    dst->vz = -src->vy;
}



/**
 * @brief Dispatch to @c func_800C2C00 with the current two-word state blob.
 *
 * Packs @c D_800C9718 / @c D_800C9710 into a pair of 8-byte buffers on the
 * caller stack; forwards @p arg1..@p arg3 alongside @c D_800C96CC and the
 * magic @c 0x80122000 command code. The same pointer is passed as @c arg6
 * and @c arg8, with @c 0 as @c arg7 — @c func_800C2C00 reads both slots.
 */
void func_800BC570(s32 arg1, s32 arg2, s32 arg3) {
    U64 buf;
    U64 tmp;

    tmp.a = D_800C9718;
    tmp.b = D_800C9710;
    buf = tmp;

    func_800C2C00(arg3, D_800C96CC, 0x80122000, arg2, arg1, &buf, 0, &buf);
}

/**
 * @brief Map an @p arg0 slot kind (via @c func_800B00D8) into a hover/offset value.
 *
 * Runs the arg through @c func_800B00D8 to get an inner code, then returns
 * one of: @c -0x800, @c -0x400, @c 0x400, @c 0x800, or @c 0 based on
 * combined @p arg0 / inner-code membership:
 *   - inner code < 10 → @c -0x800 (too low)
 *   - inner code == 0x80 or @p arg0 in {2, 3} → @c -0x800
 *   - inner code in [0x20, 0x28] or == 0x84 → @c -0x400
 *   - inner code == 0x32 → @c 0x800
 *   - inner code == 0x30 or @p arg0 == 0x4F → @c 0x400
 *   - @p arg0 == 0x50 → @c 0x800
 *   - otherwise → @c 0
 *
 * @note Purpose uncertain — appears to be a UI hover-offset or similar
 *       lookup that shifts display based on slot type.
 */
s32 func_800BC5E0(s32 arg0) {
    s32 v = func_800B00D8(arg0);

    if ((u32)v < 0xA) return -0x800;
    if (v == 0x80 || arg0 == 2 || arg0 == 3) return -0x800;
    if ((u32)(v - 0x20) < 9 || v == 0x84) return -0x400;
    if (v == 0x32) return 0x800;
    if (v != 0x30 && arg0 != 0x4F) {
        return (arg0 == 0x50) << 11;
    }
    return 0x400;
}

/**
 * @brief Linear scan of @c D_800DBFB8 starting at @p start for a slot whose
 *        @c marker equals @p key.
 *
 * Returns the matching slot index on hit, or -1 on miss / when @p start is
 * already past @c D_800C5B50.
 *
 * @param key Marker byte value to search for.
 * @param start First index to scan.
 * @return Matching slot index, or -1 on no match.
 */
s32 func_800BC688(s32 key, s32 start) {
    s32 i;

    for (i = start; i < D_800C5B50; i++) {
        if (key == D_800DBFB8[i].marker) {
            return i;
        }
    }
    return -1;
}

/* StringTable and D_800C97D4 are declared in world.h. */

/**
 * @brief Copy a null-terminated string from the first offset table to @p dst.
 *
 * Looks up the byte offset for string @p idx in @c table->first, computes the
 * string's address (@c table + offset), and copies the null-terminated bytes
 * into @p dst.
 *
 * @param dst Destination buffer (must have room for the string + null).
 * @param idx Index into @c table->first.
 * @return @p dst (unchanged).
 */
u8 *func_800BC6E8(u8 *dst, s32 idx) {
    StringTable *table = D_800C97D4;
    s32 *offsets = table->first;
    u8 *src = (u8 *)table + offsets[idx];
    s32 i;

    i = 0;
    while ((dst[i] = src[i]) != 0) {
        i++;
    }
    return dst;
}

/**
 * @brief Append the string at @c table->first[idx] to the end of @p dst.
 *
 * Finds the null terminator of the existing @p dst string, then copies the
 * table-indexed string over it. Mirrors the idiom used by @c func_800BCE74.
 */
u8 *func_800BC744(u8 *dst, s32 idx) {
    StringTable *table = D_800C97D4;
    s32 *offsets = table->first;
    u8 *src = (u8 *)table + offsets[idx];
    s32 len;
    s32 i;
    u8 c;
    u8 new_var;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    i = 0;
    if (c != 0) {
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
        len--;
    }
    return dst;
}

/**
 * @brief Copy a null-terminated string from the second offset table to @p dst.
 *
 * Variant of @c func_800BC6E8 that indexes @c table->second (the second
 * offset array, starting 0x78 bytes into the blob) instead of @c first.
 */
u8 *func_800BC7D0(u8 *dst, s32 idx) {
    StringTable *table = D_800C97D4;
    u8 *src = (u8 *)table + *(table->second + idx);
    s32 i;

    i = 0;
    while ((dst[i] = src[i]) != 0) {
        i++;
    }
    return dst;
}

/**
 * @brief Append a string from the second offset table to @p dst (strcat variant).
 *
 * Computes the length of the existing null-terminated string in @p dst, looks
 * up the source string at @c table->second[idx], and appends it (including the
 * terminating null) to @p dst.
 *
 * @param dst Destination buffer with an existing null-terminated string.
 * @param idx Index into @c table->second.
 * @return @p dst (unchanged).
 */
u8 *func_800BC82C(u8 *dst, s32 idx) {
    u8 new_var;
    StringTable *table = D_800C97D4;
    u8 *src = (u8 *)table + *(table->second + idx);
    s32 len;
    s32 i;
    u8 c;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    if (c != 0) {
        i = 0;
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
    }
    return dst;
}


/**
 * @brief Zero-terminate buf and fill it with a magic spell's display name.
 *
 * Clears the first byte of @p buf before delegating to func_800BC8D8, which
 * resolves the magic name (via getMagicNamePtr) and copies glyphs into @p buf.
 *
 * @param buf Destination string buffer; first byte is zeroed before filling.
 * @param magicId Magic spell ID whose name to look up.
 */
void func_800BC8B8(u8 *buf, s32 magicId) {
    buf[0] = 0;
    func_800BC8D8(buf, magicId);
}

/**
 * @brief Append a magic-name string to @p dst (strcat variant).
 *
 * Looks up the magic name pointer via @c getMagicNamePtr and appends it
 * (including terminating null) to the existing null-terminated string in
 * @p dst.
 *
 * @param dst     Destination buffer with an existing null-terminated string.
 * @param magicId Magic ID to look up via @c getMagicNamePtr.
 * @return @p dst (unchanged).
 */
u8 *func_800BC8D8(u8 *dst, s32 magicId) {
    u8 new_var;
    u8 *src = getMagicNamePtr(magicId);
    s32 len;
    s32 i;
    u8 c;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    i = 0;
    if (c != 0) {
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
        len--;
    }
    return dst;
}

/**
 * @brief Append the decimal string of @p num to @p dst.
 *
 * Converts @p num to its decimal digits, looks up the digit-0 character from
 * @c table->first[29] (the game's digit-character base), and appends the
 * digits (most significant first) to the null-terminated string in @p dst.
 * Null-terminates the result.
 *
 * @param dst Destination buffer with an existing null-terminated string.
 * @param num Integer to convert to decimal.
 * @return @p dst (unchanged).
 */
u8 *func_800BC974(u8 *dst, s32 num) {
    StringTable *table = D_800C97D4;
    u8 *digitBase = (u8 *)table + table->first[29];
    u8 digits[8];
    s32 len;
    s32 count;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    count = 0;
    if (num != 0) {
        do {
            digits[count++] = num % 10;
            num /= 10;
        } while (num != 0);
    }

    if (count == 0) {
        digits[0] = 0;
        count = 1;
    }

    for (count--; count >= 0; count--, len++) {
        dst[len] = digitBase[0] + digits[count];
    }

    dst[len] = 0;
    return dst;
}


/**
 * @brief Zero-terminate buf and fill it with a name string via func_800BC974.
 *
 * Mirrors func_800BC8B8 but forwards to a different name resolver
 * (func_800BC974 reads from the table referenced by D_800C97D4).
 *
 * @param buf Destination string buffer; first byte is zeroed before filling.
 * @param id Identifier whose name to look up.
 */
void func_800BCA54(u8 *buf, s32 id) {
    buf[0] = 0;
    func_800BC974(buf, id);
}



/**
 * @brief Format a template string with up to four parameter substitutions.
 *
 * Fetches the template at @c D_800C9FE8 index @p idx and walks it character
 * by character. Each byte that isn't an @c 0x0A marker is looked up in the
 * @c D_800C97D4 character table and appended to @p dst. The two-byte sequence
 * @c 0x0A/0xFF terminates the template. When the byte following an @c 0x0A
 * is in the range @c 0x20..0x23, the corresponding parameter
 * (@p p1 / @p p2 / @p p3 / @p p4) is appended via @c strcat.
 *
 * @param dst Destination buffer.
 * @param idx Template index into @c D_800C9FE8.
 * @param p1  Substitution string for marker @c 0x0A @c 0x20.
 * @param p2  Substitution string for marker @c 0x0A @c 0x21.
 * @param p3  Substitution string for marker @c 0x0A @c 0x22.
 * @param p4  Substitution string for marker @c 0x0A @c 0x23.
 * @return @p dst (unchanged).
 */
u8 *func_800BCA74(u8 *dst, s32 idx, u8 *p1, u8 *p2, u8 *p3, u8 *p4) {
    TemplateTable *tpl = D_800C9FE8;
    s32 *toff = (s32 *)tpl;
    u8 *src = (u8 *)tpl + toff[idx];
    s32 i = 0;

    while (src[i] != 0x0A || src[i + 1] != 0xFF) {
        if (src[i] == 0x0A) {
            i++;
            if (src[i] >= 0x20) {
                s32 code = src[i] - 0x20;
                if (code == 0) {
                    if (p1 != 0) strcat(dst, p1);
                } else if (code == 1) {
                    if (p2 != 0) strcat(dst, p2);
                } else if (code == 2) {
                    if (p3 != 0) strcat(dst, p3);
                } else if (code == 3) {
                    if (p4 != 0) strcat(dst, p4);
                }
            }
        } else {
            u8 c = src[i];
            StringTable *chtable = D_800C97D4;
            s32 *offsets = chtable->first;
            u8 *s = (u8 *)chtable + offsets[c];
            s32 len = 0;
            s32 j;
            u8 ch;
            if (dst[0] != 0) {
                len = 1;
                len--;
                do {
                    len++;
                } while (dst[len] != 0);
            }
            j = 0;
            do {
                ch = s[j];
                dst[len] = s[j];
                if (ch == 0) break;
                len++;
                j++;
            } while (1);
        }
        i++;
    }
    return dst;
}

/**
 * @brief Variant of @c func_800BCA74 that clears @c dst[0] before formatting.
 *
 * Identical to @c func_800BCA74 except it writes @c '\0' to @p dst before
 * walking the template, so the destination starts empty regardless of any
 * prior contents.
 */
u8 *func_800BCC70(u8 *dst, s32 idx, u8 *p1, u8 *p2, u8 *p3, u8 *p4) {
    TemplateTable *tpl;
    s32 *toff;
    u8 *src;
    s32 i = 0;

    dst[0] = 0;
    tpl = D_800C9FE8;
    toff = (s32 *)tpl;
    src = (u8 *)tpl + toff[idx];

    while (src[i] != 0x0A || src[i + 1] != 0xFF) {
        if (src[i] == 0x0A) {
            i++;
            if (src[i] >= 0x20) {
                s32 code = src[i] - 0x20;
                if (code == 0) {
                    if (p1 != 0) strcat(dst, p1);
                } else if (code == 1) {
                    if (p2 != 0) strcat(dst, p2);
                } else if (code == 2) {
                    if (p3 != 0) strcat(dst, p3);
                } else if (code == 3) {
                    if (p4 != 0) strcat(dst, p4);
                }
            }
        } else {
            u8 c = src[i];
            StringTable *chtable = D_800C97D4;
            s32 *offsets = chtable->first;
            u8 *s = (u8 *)chtable + offsets[c];
            s32 len = 0;
            s32 j;
            u8 ch;
            if (dst[0] != 0) {
                len = 1;
                len--;
                do {
                    len++;
                } while (dst[len] != 0);
            }
            j = 0;
            do {
                ch = s[j];
                dst[len] = s[j];
                if (ch == 0) break;
                len++;
                j++;
            } while (1);
        }
        i++;
    }
    return dst;
}


/**
 * @brief Append a stat-name string to @p dst (strcat variant).
 *
 * Looks up the stat name via @c getStatName and appends it (including the
 * terminating null) to the existing null-terminated string in @p dst.
 *
 * @param dst    Destination buffer with an existing null-terminated string.
 * @param statId Stat ID passed to @c getStatName.
 * @return @p dst (unchanged).
 */
u8 *func_800BCE74(u8 *dst, s32 statId) {
    u8 new_var;
    u8 *src = getStatName(statId);
    s32 len;
    s32 i;
    u8 c;

    len = 0;
    if (dst[0] != 0) {
        do {
            len++;
        } while (dst[len] != 0);
    }

    c = src[0];
    dst[len] = c;
    new_var = c != 0;
    i = 0;
    if (c != 0) {
        do {
            len++;
            i++;
            new_var = src[i];
            c = new_var;
            dst[len] = c;
        } while (new_var);
        len--;
    }
    return dst;
}


/**
 * @brief Zero-terminate buf and fill it with a stat name via func_800BCE74.
 *
 * Mirrors func_800BC8B8 / func_800BCA54 but targets the stat-name resolver
 * (func_800BCE74 dispatches to getStatName internally).
 *
 * @param buf Destination string buffer; first byte is zeroed before filling.
 * @param statId Stat identifier whose name to look up.
 */
void func_800BCF10(u8 *buf, s32 statId) {
    buf[0] = 0;
    func_800BCE74(buf, statId);
}


/**
 * @brief Convert a 16-bit angle into a Vec3 position on a 128x48 tile grid.
 *
 * Treats @p angle as a signed 16-bit value and splits it into a section
 * index (angle / 128) and an offset within the section (angle % 128).
 * Writes the resulting world-space coordinates to @p out:
 *   - @c x = (offset - 0x40) * 2048     (centered, shifted by 11 bits)
 *   - @c z = (0x30 - section) * 2048
 *   - @c y = 0                          (flat on the XZ plane)
 *
 * Returns silently when @p out is NULL.
 *
 * @note @c angle is declared s32 (not s16) and the @c section=angle prelude
 *       is deliberate — it steers gcc's register allocation to match the
 *       target's @c v1/@c a0 pattern.
 *
 * @param angle s16-style angle (low 16 bits are significant).
 * @param out Output position vector; no-op when NULL.
 */
void func_800BCF30(s32 angle, Vec3 *out) {
    s32 section;
    s32 a;
    s16 offset;
    section = angle;
    if (out != 0) {
        a = (s16)section;
        section = a / 128;
        offset = a - section * 128;
        out->x = (offset - 0x40) << 11;
        out->z = (0x30 - section) << 11;
        out->y = 0;
    }
}



/**
 * @brief Look up an (id, param) pair in a scattered multi-subtable index.
 *
 * Uses the 5-bit scatter key at @c D_800D2440 to select up to 4 sub-ranges
 * from @c D_800C5D70. For each non-negative sub-range index, searches the
 * corresponding entry pool for a matching (id, param) pair.
 *
 * @param id Entry id to match (compared as halfword).
 * @param param Entry param byte to match.
 * @return Entry index (offset from pool base, divided by entry stride) on match,
 *         or -1 if no matching entry was found in any scanned sub-range.
 */
s32 func_800BCF84(s32 id, s32 param) {
    s8 *scatter = D_800C5D70[D_800D2440 & 0x1F].ranges;
    LookupBlob *blob = D_800C96F8;
    u8 *pool_base;
    u8 *entry_base = (u8 *)blob + blob->entry_base_off;
    s32 i;
    s32 t;
    LookupEntry *e;
    LookupEntry *e_end;

    pool_base = (u8 *)blob;
    for (i = 0; i < 4; i++) {
        t = scatter[i];
        if (t < 0) break;
        e = (LookupEntry *)(pool_base + (&blob->ranges[t])->start_off);
        e_end = (LookupEntry *)(pool_base + (&blob->ranges[t])->end_off);
        while (e < e_end) {
            if (id == e->id && param == e->param) {
                return ((u8 *)e - entry_base) >> 2;
            }
            e++;
        }
    }
    return -1;
}


/** Checks if a counter has reached threshold 0xBB8. */
s32 func_800BD040(void) {
    return D_80077E84 >= 0xBB8;
}

/**
 * @brief Subtract a clamped value from the HP-like counter at g_gameState+0xB0C.
 */
s32 func_800BD058(s32 amount) {
    s32 base = (s32)&g_gameState;
    s32 max;

    if (amount >= 0) {
        max = *(s32 *)(base + 0xB0C);
        if ((u32)max < (u32)amount) {
            amount = max;
        }
    } else {
        amount = 0;
    }
    *(s32 *)(base + 0xB0C) = *(s32 *)(base + 0xB0C) - amount;
    return amount;
}



/**
 * @brief Test if a candidate slot fits the current command and insert it.
 *
 * Rejects the slot early when its @c angle is more than 0xC8 away from the
 * world camera @p worldAngle. Then requires both cmd-tag checks (against
 * @p arg1 and @c D_800C4D38) to pass via @c func_800A4670. On all checks
 * passing, dispatches @c func_800B00D8 on the slot's marker and forwards
 * the result to @c func_800A358C for insertion. Returns 1 when insertion
 * reports "not already present" (negative), 0 otherwise.
 *
 * @note Purpose uncertain — the return semantic is inverted from the
 * surrounding early-exit 0s, suggesting @c func_800A358C returns a
 * negative sentinel for "new".
 *
 * @param slot Candidate slot entry (read-only in this function).
 * @param arg1 First tag selector passed to the cmd check.
 * @param cmd Command descriptor (type/flag/param packed into 24-bit key).
 * @param worldAngle Current world camera angle for proximity test.
 * @return 1 if the slot was newly inserted, 0 otherwise or on rejection.
 */
s32 func_800BD09C(SlotEntry *slot, s32 arg1, CmdDesc *cmd, s32 worldAngle) {
    s32 delta = slot->position.vy - worldAngle;
    s32 insertResult;
    s32 trigger;

    if (delta > 0) {
        if (delta >= 0xC8) return 0;
    } else if (worldAngle - slot->position.vy >= 0xC8) {
        return 0;
    }

    trigger = 1;
    if (func_800A4670(cmd->type | (cmd->flag << 8) | (cmd->param << 16), arg1) == 0) {
        return 0;
    }
    if (func_800A4670(cmd->type | (cmd->flag << 8) | (cmd->param << 16), D_800C4D38) == 0) {
        if (trigger) return 0;
    }
    insertResult = func_800A358C(func_800B00D8(slot->marker), slot, &slot->vec, trigger);
    if (insertResult < 0) return trigger;
    return 0;
}



/**
 * @brief Initialize scene-relative state from the template at @c D_800DD680 et al.
 *
 * No-op when the @c D_800C4DC8 flag is zero. Otherwise copies:
 *   - @c D_800DD698 → @c D_800C4D38 (primary cmd byte)
 *   - @c D_800DD680.vx → @c D_800C9868.vx
 *   - @c D_800DD680.vy → @c D_800C9868.vz  (Y/Z swap for PS1 coord system)
 *   - -@c D_800DD680.vz → @c D_800C9868.vy (Z negated)
 *   - @c D_800DD690[0..8] → @c D_800C9770[1] (the rotation slot)
 *   - @c D_800DD69C → @c D_800C4D3C (secondary cmd byte)
 * and calls @c worldPosToCell with the source vector and @c D_800C9770 buffer.
 */
void func_800BD180(void) {
    if (D_800C4DC8 != 0) {
        D_800C4D38 = D_800DD698;
        D_800C9868.vx = D_800DD680.vx;
        D_800C9868.vz = D_800DD680.vy;
        D_800C9868.vy = -D_800DD680.vz;
        worldPosToCell(&D_800DD680, D_800C9770);
        memcpy(&D_800C9770[1], D_800DD690, 8);
        D_800C4D3C = D_800DD69C;
    }
}

/**
 * @brief Arm scene state: stash source data into @c D_800DD680.. for later apply.
 *
 * Counterpart to @c func_800BD180. Sets the "armed" flag at @c D_800C4DC8,
 * copies the caller's 16-byte vector block into @c D_800DD680, the 8-byte
 * rotation blob into @c D_800DD690 (unaligned), and the two cmd bytes into
 * @c D_800DD698 / @c D_800DD69C. @c func_800BD180 consumes this state on
 * its next call.
 *
 * @param src 16-byte vector block (@c x, @c y, @c z, @c pad0C).
 * @param rot 8-byte rotation blob (caller may pass an unaligned address).
 * @param cmd Primary cmd byte, stored at @c D_800DD698.
 * @param cmd2 Secondary cmd byte, stored at @c D_800DD69C.
 */
void func_800BD22C(VECTOR *src, u8 *rot, s32 cmd, s32 cmd2) {
    D_800C4DC8 = 1;
    D_800DD698 = cmd;
    D_800DD680 = *src;
    memcpy(D_800DD690, rot, 8);
    D_800DD69C = cmd2;
}



/**
 * @brief Compute two coarse-angle outputs for the current active slot.
 *
 * Looks up the active slot index at @c D_800C5C2C; returns 0 if no slot
 * is active (index < 0). Otherwise converts the slot's (x, -z) pair to an
 * angle via @c func_800A5DC8, then splits the s16 result into a "fine"
 * (mod 128, doubled) and "coarse" (div 128, doubled) halfword output.
 * Either output pointer may be NULL to skip that write.
 *
 * @param outLow  Destination for the fine angle component (may be NULL).
 * @param outHigh Destination for the coarse angle component (may be NULL).
 * @return 1 when the slot is active, 0 otherwise.
 */
s32 func_800BD2A0(s16 *outLow, s16 *outHigh) {
    s32 slotIdx = D_800C5C2C;
    s32 vec[3];
    s32 angle;
    s16 s16angle;
    s32 result = 0;

    if (slotIdx >= 0) {
        SlotEntry *slot = &D_800DBFB8[slotIdx];
        s32 *vp;
        vec[0] = slot->position.vx;
        vp = vec;
        vp[1] = -slot->position.vz;
        vp[2] = slot->position.vy;
        angle = func_800A5DC8(vec[0], vec[1]);

        s16angle = (s16)angle;
        if (outLow != 0) {
            s16angle = (s16)angle;
            *outLow = (s16angle % 128) << 1;
        }
        if (outHigh != 0) {
            *outHigh = (s16angle / 128) << 1;
        }
        result = 1;
    }
    return result;
}


/**
 * @brief Twin of @c func_800BD2A0 that reads the secondary slot index.
 *
 * Identical logic to @c func_800BD2A0 except the slot index is sourced
 * from @c D_800C5C28 (vs @c D_800C5C2C). Used for the companion slot in
 * the dual-slot tracking scheme.
 *
 * @param outLow  Destination for the fine angle component (may be NULL).
 * @param outHigh Destination for the coarse angle component (may be NULL).
 * @return 1 when the slot is active, 0 otherwise.
 */
s32 func_800BD380(s16 *outLow, s16 *outHigh) {
    s32 slotIdx = D_800C5C28;
    s32 vec[3];
    s32 angle;
    s16 s16angle;
    s32 result = 0;

    if (slotIdx >= 0) {
        SlotEntry *slot = &D_800DBFB8[slotIdx];
        s32 *vp;
        vec[0] = slot->position.vx;
        vp = vec;
        vp[1] = -slot->position.vz;
        vp[2] = slot->position.vy;
        angle = func_800A5DC8(vec[0], vec[1]);

        s16angle = (s16)angle;
        if (outLow != 0) {
            s16angle = (s16)angle;
            *outLow = (s16angle % 128) << 1;
        }
        if (outHigh != 0) {
            *outHigh = (s16angle / 128) << 1;
        }
        result = 1;
    }
    return result;
}


/**
 * @brief Third variant of the slot angle splitter, sourcing @c D_800C5C24.
 *
 * Same logic as @c func_800BD2A0 / @c func_800BD380 but reads the slot
 * index from @c D_800C5C24.
 */
s32 func_800BD460(s16 *outLow, s16 *outHigh) {
    s32 slotIdx = D_800C5C24;
    s32 vec[3];
    s32 angle;
    s16 s16angle;
    s32 result = 0;

    if (slotIdx >= 0) {
        SlotEntry *slot = &D_800DBFB8[slotIdx];
        s32 *vp;
        vec[0] = slot->position.vx;
        vp = vec;
        vp[1] = -slot->position.vz;
        vp[2] = slot->position.vy;
        angle = func_800A5DC8(vec[0], vec[1]);

        s16angle = (s16)angle;
        if (outLow != 0) {
            s16angle = (s16)angle;
            *outLow = (s16angle % 128) << 1;
        }
        if (outHigh != 0) {
            *outHigh = (s16angle / 128) << 1;
        }
        result = 1;
    }
    return result;
}


/**
 * @brief Populate @c field46 from one of two selector tables based on mode.
 *
 * When @c D_800C971C is non-zero, picks from a 4-entry "zone" table using
 * @c (sel69 & 3); otherwise picks from a 5-entry table using
 * @c (sel68 & 0x1F). The final post-pass ORs in 0x40 when the matching
 * @c flag66 bit is set (0x10 in mode A, 0x08 in mode B). Entries outside
 * the covered range leave @c field46 untouched.
 *
 * @note The 5-case switch emits @c jtbl_80098770 at overlay offset 0x770
 *       (mid-rodata between @c we_dispatch and @c we_dispatch2 subsegments
 *       — see @c world.ovl.yaml).
 */
void func_800BD540(Entity *e) {
    if (D_800C971C != 0) {
        switch (e->sel69 & 3) {
            case 0: e->field46 = 0x3F;  break;
            case 1: e->field46 = 0x23E; break;
            case 2: e->field46 = 0x3E;  break;
        }
        if ((e->flag66 & 0x10) != 0) {
            e->field46 |= 0x40;
        }
    } else {
        switch (e->sel68 & 0x1F) {
            case 0: e->field46 = 0x11D; break;
            case 1: e->field46 = 0x10D; break;
            case 2: e->field46 = 0x101; break;
            case 3: e->field46 = 0x100; break;
            case 4: e->field46 = 0x180; break;
        }
        if ((e->flag66 & 0x8) != 0) {
            e->field46 |= 0x40;
        }
    }
}


/**
 * @brief Classify a @c SlotTarget against an expected @p kind.
 *
 * Returns a small result code:
 *   - -1: @p kind is 0xFF, mismatch, or out-of-zone.
 *   - For @p kind == 0: 0 when @c sb1F == 1; 1 when @c sb1E == -1 && @c sb1F == 0.
 *   - For @p kind == 1: 2 when @c sb1F == 1.
 *   - Any other @p kind: -1.
 *
 * The @c byte3 "zone" byte must be 1 or 2 for any non-default return.
 */
s32 func_800BD640(u8 kind, SlotTarget *t) {
    s32 result = -1;

    if (kind == 0xFF) {
        return -1;
    }
    if (kind == 0) {
        if ((u32)(t->byte3 - 1) < 2) {
            if (t->sb1F == 1) {
                result = 0;
            } else if (t->sb1E == -1 && t->sb1F == 0) {
                result = 1;
            }
        }
    } else if (kind == 1) {
        if ((u32)(t->byte3 - 1) < 2 && t->sb1F == 1) {
            result = 2;
        }
    }
    return result;
}
