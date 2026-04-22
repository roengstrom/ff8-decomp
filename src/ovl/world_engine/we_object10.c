#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "world.h"

extern u8 D_800786D8;
extern SlotEntry D_800DBFB8[];
extern s32 D_800C5B50;

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BD6EC);

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

extern CmdDesc *D_800C4D64;

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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BD82C);

/**
 * @brief Copy 10 flag bytes from D_800780D8 to destination struct.
 *
 * Copies bytes at D_800780D8 offsets 0x108-0x10F to dst offsets 0x66-0x6D,
 * and 0x100-0x101 to dst offsets 0x6E-0x6F.
 *
 * @param dst Destination buffer pointer.
 */
void func_800BD918(u8 *dst) {
    extern u8 D_800780D8[];
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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BD998);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDA08);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDA78);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDAE4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDB5C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDBE0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDC94);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDDC4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BDEF4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE040);

extern LookupTarget *D_800DDB00[];

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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE1A8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE284);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE3D0);

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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE578);

/** @brief 32-byte record used by func_800BE5F8's table walker. */
typedef struct {
    s16 marker;         /**< 0x00: Scan terminator (-1 = end, -2 = continuation). */
    u8 pad02[0x1E];     /**< 0x02: Rest of the record — payload not yet mapped. */
} Record;

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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE63C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE69C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE720);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE7FC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE8B0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BE958);

extern void func_8009B358(s32 a0, s32 a1, s32 a2);
extern void func_8009D8A8(s32 a0);

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

extern s32 D_800C5D54;
extern void func_80048C50(s32 a0);
extern void fadeOutSfxFast(s32 idx);
extern void renderAndUpdateDisplay(s32 frameCount);
extern s32 renderBattleDisplayList(s32 *colorTag);

/**
 * @brief Tear down the battle scene: clear state flag, fade SFX, render two frames.
 *
 * Typical end-of-battle / scene-transition cleanup sequence:
 *  1. @c func_80048C50(0) — system/display reset.
 *  2. Clear the @c D_800C5D54 flag.
 *  3. @c fadeOutSfxFast(0) — stop channel 0 SFX.
 *  4. @c renderAndUpdateDisplay(2) — push 2 frames.
 *  5. Flush the battle scene's colorTag into the display list.
 */
void func_800BEA34(void) {
    func_80048C50(0);
    D_800C5D54 = 0;
    fadeOutSfxFast(0);
    renderAndUpdateDisplay(2);
    renderBattleDisplayList(&D_800D244C->colorTag);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BEA7C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BEAF4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BEB84);

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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BEC60);

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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BEDF0);

void func_800BEECC(void) {
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BEED4);

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
    u8 val = D_800786D8;
    s32 result = 0;
    if (val & 1) {
        s32 bit = val & 2;
        result = (u32)bit < 1;
    }
    return result;
}

/** Initializes nested array with ramp values. */
void func_800BEFEC(u8 *base) {
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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BF024);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BF20C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BF2E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BF3D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object10", func_800BF5A8);
