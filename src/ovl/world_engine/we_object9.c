#include "common.h"

/* 4-byte slot read as either a full word or just the low halfword */
typedef union {
    u32 word;
    u16 half;
} AngleSlot;

/* 16-byte position descriptor (first 12 bytes = vec3 used by spawner) */
typedef struct {
    u32       pad0;
    AngleSlot pos;
    u32       pad8;
    u32       padC;
} PosDesc;

/* 8-byte unaligned velocity (copied via lwl/lwr to spawner's slot[0x18..0x1F]) */
typedef struct {
    u16 pad0;
    u16 angle;
    u16 height;
    u16 pad6;
} Velocity;

/* Particle source: position + velocity (with 4-byte gap between). Stride 0x28. */
typedef struct {
    PosDesc  pos;
    u32      pad10;
    Velocity vel;
} ParticleSource;

extern AngleSlot D_800C97F4;

extern s32 func_8009CC3C(void);
extern s32 func_800AC0A0(s32 type, PosDesc *pos, Velocity *vel, s32 flags);

#define SPAWN_FLAG_LIFETIME_JITTER 1
#define SPAWN_FLAG_SIZE_JITTER     2

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BA870);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BAC84);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BB150);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BB4E8);

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
    Velocity localVel;
    PosDesc  localPos;
    s32      jitter;
    s32      absdx;
    s16      dx;
    u32      worldCam;

    worldCam = D_800C97F4.word;
    localPos = src->pos;
    dx       = D_800C97F4.half - src->pos.pos.half;
    localPos.pos.word = worldCam;
    if ((func_8009CC3C() + (dx >> 2)) >= visRange) return;
    absdx = (dx >= 0) ? dx : -dx;
    if (absdx >= 0x258) return;

    /* Left-side spawn: +0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.angle += 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.angle += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.height += jitter;
    func_800AC0A0(0x13, &localPos, &localVel,
                  SPAWN_FLAG_SIZE_JITTER | SPAWN_FLAG_LIFETIME_JITTER);

    /* Right-side spawn: -0x320 angle offset + RNG jitter on angle/height */
    localVel = src->vel;
    localVel.angle -= 0x320;
    jitter = (func_8009CC3C() - 0x80) * 8;
    localVel.angle += jitter;
    jitter = (func_8009CC3C() - 0x80) * 4;
    localVel.height += jitter;
    func_800AC0A0(0x13, &localPos, &localVel,
                  SPAWN_FLAG_SIZE_JITTER | SPAWN_FLAG_LIFETIME_JITTER);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BBF0C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC09C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC218);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC44C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC46C);

/** Copies and negates vector components (rotation variant A). */
void func_800BC51C(s32 *src, s32 *dst) {
    dst[0] = src[0];
    dst[1] = -src[2];
    dst[2] = src[1];
}

/** Copies and negates vector components (rotation variant B). */
void func_800BC544(s32 *src, s32 *dst) {
    dst[0] = src[0];
    dst[1] = src[2];
    dst[2] = -src[1];
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC570);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC5E0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC688);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC6E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC744);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC7D0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC82C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC8B8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC8D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BC974);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCA54);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCA74);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCC70);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCE74);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCF10);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCF30);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BCF84);

extern u32 D_80077E84;

/** Checks if a counter has reached threshold 0xBB8. */
s32 func_800BD040(void) {
    return D_80077E84 >= 0xBB8;
}

/**
 * @brief Subtract a clamped value from the HP-like counter at g_gameState+0xB0C.
 */
s32 func_800BD058(s32 amount) {
    extern u8 g_gameState[];
    s32 base = (s32)g_gameState;
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

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD09C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD180);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD22C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD2A0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD380);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD460);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD540);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object9", func_800BD640);
