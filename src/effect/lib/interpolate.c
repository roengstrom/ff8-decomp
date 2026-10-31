/**
 * @file interpolate.c
 * @brief Interpolate a vertex animation between two of its frames.
 */
#include "common.h"
#include "effect.h"
#include "psxsdk/inline_c.h"
#include "effect/lib/interpolate.h"

/**
 * @brief Interpolate every point of @p anim between two of its frames.
 *
 * @param frameA First frame; 0 selects the rest pose.
 * @param frameB Second frame.
 * @param t      Weight of @p frameB, 0x1000 being all of it.
 * @param out    Receives @c anim->count points.
 */
void effectInterpolateFrames(EffectVertexAnim *anim, s32 frameA, s32 frameB,
                             s32 t, SVECTOR *out) {
    SVECTOR *a;
    SVECTOR *b;
    s32 n;
    s32 s = ONE - t;

    if (frameA == 0) {
        a = anim->frames;
    } else {
        a = &anim->frames[frameA * anim->count];
    }
    if (frameB == 0) {
        b = anim->frames;
    } else {
        b = &anim->frames[frameB * anim->count];
    }
    n = anim->count;
    while (--n != -1) {
        gte_lddp(s);
        gte_ldsv(a);
        gte_gpf12();
        gte_lddp(t);
        gte_ldsv(b);
        gte_gpl12();
        gte_stsv(out);
        out++;
        a++;
        b++;
    }
}
