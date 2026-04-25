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

extern ActorRecord D_800DD6A8[];

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B310C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B33D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B368C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object7", func_800B3868);

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
