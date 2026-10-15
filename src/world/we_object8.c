#include "common.h"
#include "psxsdk/libgte.h"
#include "world/we_object2.h"

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object8", func_800B8C70);

// Returns the index of the direction with rank which: 0 turns least away from angle, 2 the most.
s32 func_800B97A0(s32 which, s16 angle, SVECTOR *v) {
    SVECTOR rot;
    SVECTOR *r;
    s16 delta[3];
    SVECTOR range;
    s32 rank[3];
    s32 i;

    for (i = 0; i < 3; i++) {
        rot.vx = ratan2(-v[i].vy, SquareRoot0(v[i].vz * v[i].vz + v[i].vx * v[i].vx));
        r = &rot; // writing vy/vz through a pointer is needed for the match
        r->vy = ratan2(v[i].vx, v[i].vz);
        r->vz = 0;
        delta[i] = (getAngleDelta(angle, rot.vy) > 0) ? getAngleDelta(angle, rot.vy)
                                                       : -getAngleDelta(angle, rot.vy);
    }
    range.vx = (delta[0] < delta[1]) ? ((delta[0] < delta[2]) ? delta[0] : delta[2])
                                     : ((delta[1] < delta[2]) ? delta[1] : delta[2]);
    range.vz = (delta[0] > delta[1]) ? ((delta[0] > delta[2]) ? delta[0] : delta[2])
                                     : ((delta[1] > delta[2]) ? delta[1] : delta[2]);
    for (i = 0; i < 3; i++) {
        if (range.vx == delta[i]) {
            rank[0] = i;
        } else if (range.vz == delta[i]) {
            rank[2] = i;
        } else {
            rank[1] = i;
        }
    }
    return rank[which];
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object8", func_800B99A4);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object8", func_800B9D38);
