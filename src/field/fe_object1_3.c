#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "main.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "field/fe_object1.h"
#include "field/fe_object10.h"
#include "field/fe_object1_3.h"

static s32 func_800A5FA4(FieldLineTrigger *seg, s32 sel);

/**
 * @brief Fire or re-arm one field line trigger, once per edge.
 *
 * @c func_800A62EC and @c func_8009A4C0 both funnel their per-segment hits
 * through here. @p sel is the segment's @c type (or a bare 0/1 from the
 * out-of-range path): even selectors arm the trigger, odd ones release it.
 * @c D_80070628 holds one latch byte per entity so a segment that stays
 * satisfied across frames only fires on the transition — the arm path runs
 * only while the latch is @c 0 and the release path only while it is @c 1.
 *
 * Both paths require the target entity's @c activeMarker to be @c 1, and the
 * trigger byte itself is only written when the entity index is inside the
 * live array (@c < @c g_fieldDoorCount); the return value reports the latch flip
 * regardless of that bounds check.
 *
 * @param seg Line-trigger record; its @c marker is the @ref g_fieldDoors entity index.
 * @param sel Selector @c 0..5 — even arms (sets @c trigger6), odd releases
 *            (clears @c trigger7). Any other value is a no-op.
 * @return @c 1 when the latch flipped, @c 0 otherwise.
 */
static s32 func_800A5FA4(FieldLineTrigger *seg, s32 sel) {
    u8 *latch = D_80070628;
    s32 result = 0;

    switch (sel & 0xFF) {
    case 0:
    case 2:
    case 4:
        if (g_fieldDoors[seg->marker].activeMarker == 1 && latch[seg->marker] == 0) {
            latch[seg->marker] = 1;
            if (seg->marker < g_fieldDoorCount) {
                g_fieldDoors[seg->marker].trigger6 = 1;
            }
            result = 1;
        }
        break;
    case 1:
    case 3:
    case 5:
        if (g_fieldDoors[seg->marker].activeMarker == 1 && latch[seg->marker] == 1) {
            latch[seg->marker] = 0;
            if (seg->marker < g_fieldDoorCount) {
                g_fieldDoors[seg->marker].trigger7 = 0;
            }
            result = 1;
        }
        break;
    }
    return result;
}

/**
 * @brief Scan the 12-entry actor segment table and fire per-segment triggers
 *        based on proximity, facing angle, and edge orientation to @p actor.
 *
 * Stages @p actor 's world position (@c posX/Y/Z >> 12) into the scratchpad
 * at @c getScratchAddr(0), then for each non-empty segment (@c marker != 0xFF):
 *  - Runs @c func_8009A2BC (which projects the segment and returns a squared
 *    distance, also writing the projected point to @c getScratchAddr(8)).
 *  - If the point is within @c actor->radius²: the segment fires
 *    (@c func_800A5FA4 with the segment @c type) when either the projected
 *    point coincides with @p actor, or the facing angle from @c func_8009A0E8
 *    lies within a @c +/-64 window of @c actor->unk23F.
 *  - Otherwise (out of range): segments with @c type >= 4 are gated by a
 *    cross-product orientation test against the segment edge, then
 *    @c type 2/4 fire with flag 1 and @c type 3/5 fire with flag 0.
 *
 * @param actor The querying actor entity.
 * @param segs  The 12-entry, 16-byte-stride segment table.
 * @param pt    Query point supplied by @c func_8009D598; unused here, the
 *              scan runs against the actor's own position.
 *
 * @note The empty @c do{}while(0) is a scheduling barrier: it keeps gcc 2.7.2
 *       from reordering the @c posY store ahead of the @c posX store while
 *       staging the scratchpad, matching the original prologue schedule.
 */
void func_800A6100(Actor *actor, FieldLineTrigger *segs, Vec3i *pt) {
    s32 *p = getScratchAddr(0);
    s32 *q;
    FieldLineTrigger *seg;
    s32 i;
    s32 dist;

    seg = segs;
    q = getScratchAddr(8);
    p[0] = actor->posX >> 12;
    do { } while (0);
    p[1] = actor->posY >> 12;
    p[2] = actor->posZ >> 12;

    for (i = 0; i < 12; i++, seg++) {
        if (seg->marker == 0xFF) {
            continue;
        }
        dist = func_8009A2BC(seg, p, q);
        if (dist != -1 && dist < actor->radius * actor->radius) {
            if (p[0] == q[0] && p[1] == q[1]) {
                func_800A5FA4(seg, seg->type);
            } else if ((((func_8009A0E8(p, q, &dist) & 0xFF) - actor->unk23F + 0x40) & 0xFF) < 0x80) {
                func_800A5FA4(seg, seg->type);
            }
        } else {
            if (seg->type >= 4) {
                s32 dx = seg->x1 - seg->x0;
                s32 dy = seg->y1 - seg->y0;
                if (dx * (p[1] - seg->y0) - dy * (p[0] - seg->x0) > 0) {
                    continue;
                }
            }
            if (seg->type == 2 || seg->type == 4) {
                func_800A5FA4(seg, 1);
            }
            if (seg->type == 3 || seg->type == 5) {
                func_800A5FA4(seg, 0);
            }
        }
    }
}

/**
 * @brief Per-frame dispatch over 12 entries, call @c func_800A5FA4
 *        with an even/odd flag based on the entry's @c mode.
 *
 * Iterates 12 16-byte entries. For each entry where @c active != @c 0xFF,
 * switches on @c mode (0..5) and calls @c func_800A5FA4(entry, flag)
 * where @c flag = 1 for even modes (0/2/4) and 0 for odd modes (1/3/5).
 *
 */
void func_800A62EC(FieldLineTrigger *segs) {
    s32 i;
    FieldLineTrigger *p;
    p = segs;
    i = 0;
    do {
        if (p->marker != 0xFF) {
            switch (p->type) {
                case 0:
                case 2:
                case 4:
                    func_800A5FA4(p, 1);
                    break;
                case 1:
                case 3:
                case 5:
                    func_800A5FA4(p, 0);
                    break;
            }
        }
        p++;
        i++;
    } while (i < 12);
}

/* PsyQ 4.3 island (func_800A63AC..func_800AA8A0) lives in fe_object1b.c. */
