/**
 * @file vector.c
 * @brief Random draws, angle jitter and vector helpers for the particle engine.
 */
#include "common.h"
#include "effect.h"
#include "psxsdk/libc.h"
#include "psxsdk/inline_c.h"
#include "effect/lib/common.h"
#include "effect/lib/rotz.h"
#include "effect/lib/pose.h"
#include "effect/lib/particle.h"
#include "effect/lib/vector.h"

static s32 effectJitterAngle(s32 base, s32 spread);
static s32 effectJitterAngleS(s16 base, s16 spread);

/** @brief A random value between @p lo and @p hi. */
s32 effectRandBetween(s32 lo, s32 hi) {
    s32 span;
    s32 r;

    if (lo == hi) {
        return lo;
    }
    span = hi - lo;
    r = rand() % span;
    if (span >= 0) {
        r = r + lo;
    } else {
        r = lo - r;
    }
    return r;
}

/** @brief A random value between @p lo and @p hi, from a product of two draws. */
s32 effectRandProduct(s32 lo, s32 hi) {
    s32 span;
    s32 r;

    if (lo == hi) {
        return lo;
    }
    span = hi - lo;
    r = rand() * rand() % span;
    if (span >= 0) {
        r = r + lo;
    } else {
        r = lo - r;
    }
    return r;
}

/**
 * @brief Combine @p a and @p b component-wise through @ref effectRandProduct.
 */
void effectRandVector(VECTOR *out, VECTOR *a, VECTOR *b) {
    out->vx = effectRandProduct(a->vx, b->vx);
    out->vy = effectRandProduct(a->vy, b->vy);
    out->vz = effectRandProduct(a->vz, b->vz);
}

/** @brief Build @p m from @p angles, applying only the turns that are non-zero. */
void effectMatrixFromAngles(VECTOR *angles, MATRIX *m) {
    SVECTOR a;

    a.vx = angles->vx / 0x10000;
    a.vy = angles->vy / 0x10000;
    a.vz = angles->vz / 0x10000;
    effectMatrixIdentity(m);
    if (a.vz != 0) {
        effectMatrixRotZ(m, a.vz);
    }
    if (a.vx != 0) {
        effectMatrixRotX(m, a.vx);
    }
    if (a.vy != 0) {
        effectMatrixRotY(m, a.vy);
    }
}

/** @brief Scatter @p base by up to half of @p spread, wrapped to one turn. */
static s32 effectJitterAngle(s32 base, s32 spread) {
    s32 half = spread / 2;
    s32 v = effectRandProduct(-half, half) + base;

    if (v >= 0) {
        while (v > 0xFFFFFFF) {
            v -= 0x10000000;
        }
    } else {
        while (v <= 0) {
            v += 0x10000000;
        }
    }
    return v;
}

/**
 * @brief Combine @p a and @p b component-wise through @ref effectJitterAngle.
 */
void effectJitterVector(VECTOR *out, VECTOR *a, VECTOR *b) {
    out->vx = effectJitterAngle(a->vx, b->vx);
    out->vy = effectJitterAngle(a->vy, b->vy);
    out->vz = effectJitterAngle(a->vz, b->vz);
}

/** @brief Scatter @p base by up to half of @p spread, wrapped to one turn. */
static s32 effectJitterAngleS(s16 base, s16 spread) {
    s16 half = spread / 2;
    s32 v = effectRandBetween(-half, half) + base;

    if (v >= 0) {
        while (v >= 0x1000) {
            v -= 0x1000;
        }
    } else {
        while (v <= 0) {
            v += 0x1000;
        }
    }
    return v;
}

/**
 * @brief Combine @p a and @p b component-wise through @ref effectJitterAngleS.
 */
void effectJitterSVector(SVECTOR *out, SVECTOR *a, SVECTOR *b) {
    out->vx = effectJitterAngleS(a->vx, b->vx);
    out->vy = effectJitterAngleS(a->vy, b->vy);
    out->vz = effectJitterAngleS(a->vz, b->vz);
}

/** @brief Copy the three words at @p src to @p dst. */
void effectCopyVector(s32 *src, s32 *dst) {
    /* The original reserved a slot here and never read it; dropping it moves
       every local below and the frame no longer matches. */
    VECTOR unused;

    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

/**
 * @brief Fade @p value towards zero by @p t, a 16.16 fraction of it.
 *
 * @param t     Clamped to the closed range 0 to 1.0.
 * @param value Read and written in place.
 */
void effectFadeValue(s32 t, s32 *value) {
    s32 v;

    if (t > 0x10000) {
        t = 0x10000;
    }
    if (t < 0) {
        t = 0;
    }
    v = *value;
    *value = v - (v / 256 * t / 256);
}

/** @brief Fade all three components of @p v towards zero by @p t. */
void effectFadeVector(s32 t, VECTOR *v) {
    /* The original reserved a slot here and never read it. */
    VECTOR unused;
    s32 x;
    s32 y;
    s32 z;

    if (t > 0x10000) {
        t = 0x10000;
    }
    if (t < 0) {
        t = 0;
    }
    x = v->vx;
    v->vx = x - (x / 256 * t / 256);
    y = v->vy;
    v->vy = y - (y / 256 * t / 256);
    z = v->vz;
    v->vz = z - (z / 256 * t / 256);
}

/** @brief Blend this frame's two source point sets into the particle's target set. */
void effectBlendPointSets(EffectParticle *particle, EffectPoseStep *step) {
    u8 *dstId = &step->targets[particle->unk1CA];
    u8 *srcAId = &step->framesA[particle->unk1CA];
    u8 *srcBId = &step->framesB[particle->unk1CA];
    EffectPointSet *target = g_effectDrawList->targets[*dstId];
    EffectPointSet *a = g_effectDrawList->frames[*srcAId];
    EffectPointSet *b = g_effectDrawList->frames[*srcBId];

    if (a != NULL && b != NULL) {
        s32 *weight = func_800B3698(8);
        SVECTOR *out = target->points;
        SVECTOR *pa = a->points;
        SVECTOR *pb = b->points;
        s32 i = 0;
        s32 t = step->weights[particle->unk1CA];
        s32 n = target->count;

        weight[1] = t;
        weight[0] = ONE - t;
        for (; i < n; i++) {
            gte_lddp(weight[0]);
            gte_ldsv(pa);
            gte_gpf12();
            pa++;
            gte_lddp(weight[1]);
            gte_ldsv(pb);
            gte_gpl12();
            pb++;
            gte_stsv(out);
            out++;
        }
        func_800B36B8(8);
    }
}
