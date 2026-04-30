#include "common.h"
#include "world.h"

/**
 * @brief Per-actor 36-byte state record indexed by actor id.
 *
 * Used by the dialogue/animation orchestrator. Only @c unk02 and @c flag1E
 * are accessed by func_800B7080 — full layout uncertain.
 */
typedef struct {
    u8 pad00[2];
    s8 unk02;            /**< 0x02: signed counter / clamp value. */
    u8 pad03[0x1B];
    s8 flag1E;           /**< 0x1E: -1 disables actor; otherwise active. */
    u8 pad1F[5];
    /* 0x24 */
} ActorRecord;

/**
 * @brief 0x34-byte tracking entry — one world-space target with screen-space
 * projection state and a relative neighbour link.
 *
 * Populated by @c func_800B01A0 (which fills the screen-space @c posX/Y/Z
 * fields plus @c unk30 and @c unk2C from the world transform pipeline) and
 * consumed by @c func_800B3868 to compute pitch/yaw to a chained target.
 */
typedef struct {
    u8 pad00[0x10];
    u16 posX;            /**< 0x10: projected screen-space X. */
    u16 posY;            /**< 0x12: projected screen-space Y. */
    u16 posZ;            /**< 0x14: projected screen-space Z. */
    u8 pad16[0x02];
    s16 unk18;           /**< 0x18: roll/twist angle — zeroed when target valid. */
    s16 yaw;             /**< 0x1A: yaw angle to target (atan2 + 0x800 bias). */
    s16 pitch;           /**< 0x1C: pitch angle to target. */
    u8 pad1E[0x0E];
    u8 unk2C[0x02];      /**< 0x2C: scratch byte pair filled by func_800B01A0. */
    s8 targetIdx;        /**< 0x2E: relative index of the chained target entry (0 = none). */
    s8 status;           /**< 0x2F: -1 = uninitialized, 1 = visible/active. */
    s8 unk30;            /**< 0x30: secondary status byte. */
    u8 pad31[0x03];
} TrackEntry;

/** @brief Header for a TrackEntry array — count plus pointer-to-entries. */
typedef struct {
    u8 pad00[0x02];
    s8 count;            /**< 0x02: number of entries pointed to by @c entries. */
    u8 pad03[0x15];
    TrackEntry *entries; /**< 0x18: pointer to the entry array. */
} TrackObj;

extern ActorRecord D_800DD6A8[];

extern WorldPos D_800D23C0;
extern WorldPos D_800C9868;
extern MATRIX D_800C9838;

extern s32 func_800A5DC8(s32 x, s32 y);
extern void func_800406A4(MATRIX *m);
extern void func_80040734(MATRIX *m);
extern s8 func_800B01A0(s16 viewY, s16 viewX, TrackEntry *e, u16 *posOut, s8 *unk30Out, u8 *unk2COut);
extern s32 func_8003F4A4(s32 x);
extern s32 func_80041E84(s32 y, s32 x);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B310C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B33D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B368C);

/**
 * @brief Update screen-space projection and chained-target pitch/yaw for
 * each entry in @p obj.
 *
 * Two-pass: the first loop marks all entries as uninitialized (status and
 * @c unk30 set to -1). The second loop, for each entry, lazily resolves its
 * screen-space position via @c func_800B01A0; then if the entry chains to
 * a relative neighbour (@c targetIdx != 0) and is currently visible, it
 * also resolves the target's projection, computes the screen-space delta,
 * and stores pitch (atan2 of -dy / horizDist) and yaw (atan2 of -dz / dx
 * plus a 0x800 bias) back into the entry. View-space angles are computed
 * once at function entry from the global camera position vectors.
 *
 * @param obj Tracking object array header.
 */
void func_800B3868(TrackObj *obj) {
    TrackEntry *current;
    TrackEntry *target;
    s32 viewX;
    s32 viewY;
    SVECTOR diff;
    s32 horizDist;
    s32 i;

    viewX = func_800A5DC8(D_800D23C0.x, D_800D23C0.y);
    viewY = func_800A5DC8(D_800C9868.x, D_800C9868.y);

    func_800406A4(&D_800C9838);
    func_80040734(&D_800C9838);

    for (i = 0, current = obj->entries; i < obj->count; i++, current++) {
        current->status = -1;
        current->unk30 = -1;
    }

    for (i = 0, current = obj->entries; i < obj->count; i++, current++) {
        if (current->status == -1) {
            current->status = func_800B01A0(viewY, viewX, current, &current->posX, &current->unk30, current->unk2C);
        }
        if (current->targetIdx != 0 && current->status == 1) {
            target = current + current->targetIdx;
            if (target->status == -1) {
                target->status = func_800B01A0(viewY, viewX, target, &target->posX, &target->unk30, target->unk2C);
            }
            diff.vx = target->posX - current->posX;
            diff.vy = target->posY - current->posY;
            diff.vz = target->posZ - current->posZ;
            current->unk18 = 0;
            horizDist = func_8003F4A4(diff.vz * diff.vz + diff.vx * diff.vx);
            current->pitch = func_80041E84(-diff.vy, horizDist);
            current->yaw = func_80041E84(-diff.vz, diff.vx) + 0x800;
        }
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B3AB8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B3ED0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B3FD4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B438C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B454C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B4AA0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B56A0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B5974);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B5ADC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B5C60);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B6034);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B63C0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B674C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B6968);

/**
 * @brief Compute a layout offset for an actor based on a 1..4 step counter.
 *
 * Looks up the actor's pair-of-mode-keys from @c D_800C5984 (indexed by
 * @c D_800D23D8[idx+2] * 2). Each key indexes into @c D_800C5C44 (stride
 * 4 u16s = 8 bytes) to retrieve a halfword that is biased by 0x80. The
 * resulting two values @p a and @p b are accumulated into a step-dependent
 * sum: +a@@1, +b@@2, +0x10+b@@3, +0x10+a@@4.
 *
 * @param idx Actor index into @c D_800DD6A8.
 * @param count Step counter (1..4); negative/zero returns 0.
 * @return Accumulated layout offset in pixels.
 */
s32 func_800B7080(s32 idx, s32 count) {
    s32 i = idx;
    ActorRecord *e;
    u16 *cmds;
    u8 *pairs;
    u8 *modes;
    u8 *pair;
    s32 a, b;
    s32 result;
    s32 one = 1;
    s32 four = 4;

    e = &D_800DD6A8[i];
    if (e->flag1E != -1) {
        count = e->unk02 - (count + one);
    }

    cmds  = D_800C5C44;
    pairs = D_800C5984;
    modes = D_800D23D8;

    pair = &pairs[modes[i + 2] * 2];
    a = (s32)cmds[(pair[0] - 0x40) * four] - 0x80;
    b = (s32)cmds[(pair[1] - 0x40) * four] - 0x80;

    result = 0;
    if (count > 0) {
        result = (s16)a + 0x10;
    }
    if (count >= 2) {
        result += (s16)b;
    }
    if (count >= 3) {
        result += 0x10 + (s16)b;
    }
    if (count >= 4) {
        result += 0x10 + (s16)a;
    }
    return result;
}

/** @brief Reset 23 global state variables for the field engine. */
void func_800B7178(void) {
    D_800C5B50 = 0;
    D_800C5B54 = 0;
    D_800C5B58 = 4;
    D_800C5C18 = -1;
    D_800C5C1C = -1;
    D_800C5C20 = -1;
    D_800C5C24 = -1;
    D_800C5C28 = -1;
    D_800C5C2C = -1;
    D_800C5C30 = -1;
    D_800C5C38 = 0;
    D_800C5C3C = 0;
    D_800C5C40 = 0;
    D_800C5D54 = 0;
    D_800C5924 = 0;
    D_800C5BFC = 0;
    D_800C4D98 = 0;
    D_800C4D70 = 0;
    D_800C4D9C = 0;
    D_800C97A4 = -1;
    D_800C4DC0 = 0;
    D_800C4DC4 = 1;
    D_800C4DC8 = 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B7240);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B73A4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B7530);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B76F8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B7C70);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B816C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B8230);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B83B4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B85DC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B881C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B893C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B8C08);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B8C70);
