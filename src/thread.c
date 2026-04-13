#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

INCLUDE_ASM("asm/nonmatchings/thread", func_80026ADC);


INCLUDE_ASM("asm/nonmatchings/thread", func_80026CA0);


INCLUDE_ASM("asm/nonmatchings/thread", func_80026CF0);


INCLUDE_ASM("asm/nonmatchings/thread", func_80026D10);


INCLUDE_ASM("asm/nonmatchings/thread", func_80026D8C);


INCLUDE_ASM("asm/nonmatchings/thread", func_80026E20);


INCLUDE_ASM("asm/nonmatchings/thread", func_80026E70);


/**
 * @brief Open a thread with interrupt protection.
 * @param a0 Thread program counter / entry point address.
 * @param a1 Thread stack pointer.
 * @return Thread handle from OpenTh.
 * @note Wraps PsyQ OpenTh with func_800472E4/func_800472F4 (likely interrupt disable/enable).
 */
s32 openThreadSafe(s32 a0, s32 a1) {
    s32 result;
    func_800472E4(a0);
    result = OpenTh(a0, a1, 0);
    func_800472F4();
    return result;
}


/**
 * @brief Close a thread with interrupt protection.
 * @param a0 Thread handle to close.
 * @note Wraps PsyQ CloseTh with func_800472E4/func_800472F4 (likely interrupt disable/enable).
 */
void closeThreadSafe(s32 a0) {
    func_800472E4(a0);
    CloseTh(a0);
    func_800472F4();
}


INCLUDE_ASM("asm/nonmatchings/thread", func_80026F4C);


/**
 * @brief Compute an address offset from a 24-bit color value.
 *
 * Masks the input to 24 bits, multiplies by 192, and adds to the base
 * address loaded from kernel memory at 0x110.
 *
 * @param a0 Input value (only lower 24 bits used).
 * @return Base address + masked input * 192.
 */
s32 getThreadControlBlock(s32 a0) {
    s32 base = *(s32 *)0x110;
    a0 &= 0xFFFFFF;
    return base + a0 * 192;
}


/** @brief Wrapper that calls func_80047384 (returns interrupt/thread status). */
void getInterruptStatus(void) { func_80047384(); }


INCLUDE_ASM("asm/nonmatchings/thread", func_80026FD4);


/**
 * @brief Switch to a thread, using a fallback address if a0 is 0.
 * @param a0 Thread handle to switch to; 0 defaults to 0xFF000000.
 * @note If func_80047384 returns bit 2 set, uses func_80026F4C instead of PsyQ ChangeTh.
 */
void switchThread(s32 a0) {
    if (a0 == 0) {
        a0 = (s32)0xFF000000;
    }
    if (func_80047384() & 4) {
        func_80026F4C(a0);
    } else {
        ChangeTh(a0);
    }
}


INCLUDE_ASM("asm/nonmatchings/thread", func_80027038);


INCLUDE_ASM("asm/nonmatchings/thread", func_800270B0);


INCLUDE_ASM("asm/nonmatchings/thread", func_80027220);


INCLUDE_ASM("asm/nonmatchings/thread", func_80027360);


/**
 * @brief Mark a battle animation entity as active.
 * @param idx Index into g_battleAnims array.
 */
void activateBattleAnim(s32 idx) {
    BattleAnimEntity *entry = &g_battleAnims.entities[idx];
    entry->field19 = 1;
    entry->field0A = 1;
}


/**
 * @brief Initialize two consecutive BattleAnimEntity entries via func_80027220.
 * @param a0 Base address of the first entry.
 * @note Initializes the first entry with mode 0, and the second (at +0xC4) with mode 0x10.
 */
void initBattleAnimPair(s32 a0) {
    func_80027220(a0, a0, 0);
    func_80027220(a0, a0 + 0xC4, 0x10);
}


INCLUDE_ASM("asm/nonmatchings/thread", func_80027448);


/**
 * @brief Copy a rectangle's bounds to g_battleAnims clip region.
 *
 * Copies x, y from @p rect to clipLeft/clipTop, and computes
 * x+w-1, y+h-1 for clipRight/clipBottom.
 *
 * @param rect Source clip rectangle.
 */
void setBattleAnimClipRect(RECT *rect) {
    g_battleAnims.clipLeft = rect->x;
    g_battleAnims.clipTop = rect->y;
    g_battleAnims.clipRight = rect->x + rect->w - 1;
    g_battleAnims.clipBottom = rect->y + rect->h - 1;
}


/**
 * @brief Read field0B from a battle animation entity.
 * @param idx Entity index.
 */
s32 getBattleAnimField0B(s32 idx) {
    BattleAnimEntity *entry = &g_battleAnims.entities[idx];
    return entry->field0B;
}


INCLUDE_ASM("asm/nonmatchings/thread", func_800275D4);


/**
 * @brief Get an animation frame parameter from a linked entity's frame buffer.
 * @param idx Entity index (masked to 0 or 1).
 * @param offset Frame offset subtracted from the current frame counter.
 * @return field02 of the resolved AnimFrame.
 * @note Resolves a secondary entry via linkedIdx, then indexes into its
 *       frames[] circular buffer using (frameCounter - offset) & 7.
 */
u16 getAnimFrameParam(s32 idx, s32 offset) {
    BattleAnimEntity *base;
    BattleAnimEntity *entry;
    BattleAnimEntity *linked;
    s32 sub_idx;
    idx &= 1;
    base = g_battleAnims.entities;
    entry = base + idx;
    linked = base + entry->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    return ((AnimFrame *)((u8 *)linked + 0x1C))[sub_idx].field02;
}


/**
 * @brief Get combined status flags from a linked entity's animation frame.
 * @param idx Entity index (masked to 0 or 1).
 * @param offset Frame offset subtracted from the current frame counter.
 * @return Bitwise OR of field08, field0A, field0C, field0E in the resolved AnimFrame.
 * @note Same lookup as getAnimFrameParam but ORs 4 adjacent u16 values.
 */
u16 getAnimFrameStatusFlags(s32 idx, s32 offset) {
    BattleAnimEntity *base;
    BattleAnimEntity *entry;
    BattleAnimEntity *linked;
    s32 sub_idx;
    AnimFrame *frame;
    idx &= 1;
    base = g_battleAnims.entities;
    entry = base + idx;
    linked = base + entry->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    frame = &linked->frames[sub_idx];
    return frame->field08 | frame->field0A | frame->field0C | frame->field0E;
}


/**
 * @brief Get field10 from a linked entity's animation frame.
 *
 * Same lookup pattern as getAnimFrameParam: resolves via linkedIdx,
 * indexes into frames[] circular buffer, but returns field10.
 *
 * @param idx Entity index (only bit 0 used).
 * @param offset Frame offset subtracted from the current frame counter.
 * @return field10 of the resolved AnimFrame.
 */
u16 func_80027A58(s32 idx, s32 offset) {
    BattleAnimEntity *base;
    BattleAnimEntity *entry;
    BattleAnimEntity *linked;
    s32 sub_idx;
    idx &= 1;
    base = g_battleAnims.entities;
    entry = base + idx;
    linked = base + entry->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    return ((AnimFrame *)((u8 *)linked + 0x1C))[sub_idx].field10;
}


/**
 * @brief Get field12 from a linked entity's animation frame.
 *
 * Same lookup pattern as getAnimFrameParam: resolves via linkedIdx,
 * indexes into frames[] circular buffer, but returns field12.
 *
 * @param idx Entity index (only bit 0 used).
 * @param offset Frame offset subtracted from the current frame counter.
 * @return field12 of the resolved AnimFrame.
 */
u16 func_80027AC8(s32 idx, s32 offset) {
    BattleAnimEntity *base;
    BattleAnimEntity *entry;
    BattleAnimEntity *linked;
    s32 sub_idx;
    idx &= 1;
    base = g_battleAnims.entities;
    entry = base + idx;
    linked = base + entry->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    return ((AnimFrame *)((u8 *)linked + 0x1C))[sub_idx].field12;
}


/**
 * @brief Compute angle from (a0, a1) with special-case axis handling.
 *
 * Returns a fixed angle when either component is zero:
 * - (0, <=0): 0xC00 (270 degrees)
 * - (0, >0):  0x400 (90 degrees)
 * - (<0, 0):  0x800 (180 degrees)
 * - (>0, 0):  0x000 (0 degrees)
 * Otherwise delegates to ratan2.
 *
 * @param a0 X component (horizontal).
 * @param a1 Y component (vertical).
 * @return Angle in fixed-point (0x1000 = 360 degrees).
 */
s32 computeAngle(s32 a0, s32 a1) {
    s32 v0;
    if (a0 != 0) goto a0_nonzero;
    v0 = 0xC00;
    if (a1 <= 0) goto end;
    v0 = 0x400;
    goto end;
a0_nonzero:
    v0 = (a0 < 1);
    if (a1 != 0) goto both_nonzero;
    v0 <<= 11;
    goto end;
both_nonzero:
    v0 = ratan2(a0, a1);
end:
    return v0;
}


INCLUDE_ASM("asm/nonmatchings/thread", func_80027B7C);

/**
 * @brief Look up a battle animation byte through a two-level table.
 *
 * Uses the low bit of a0 as an index into g_battleAnims (stride 196).
 * Reads the byte at offset 0xC2 of that entry as a second index,
 * then returns bits [6:0] of offset 0xC3 of the second entry, shifted left 8.
 *
 * @param a0 Slot selector (only bit 0 used).
 * @return (g_battleAnims[slot].byte_C3 & 0x7F) << 8.
 */
s32 getBattleAnimLinkedValue(s32 a0) {
    s32 slot;
    a0 &= 1;
    slot = g_battleAnims.entities[a0].linkedIdx;
    return (g_battleAnims.entities[slot].fieldC3 & 0x7F) << 8;
}


INCLUDE_ASM("asm/nonmatchings/thread", func_80027C00);


/**
 * @brief Set or clear the high bit (0x80) of a linked entity's fieldC3.
 *
 * Looks up g_battleAnims.entities[a0 & 1], follows its linkedIdx to a
 * second entity, then sets bit 7 of fieldC3 if a1 is nonzero, or clears
 * it if a1 is zero.
 *
 * @param a0 Entity index (only bit 0 used).
 * @param a1 If nonzero, set bit 7; if zero, clear bit 7.
 */
void setBattleAnimLinkedHighBit(s32 a0, s32 a1) {
    s32 slot;
    BattleAnimEntity *linked;
    a0 &= 1;
    slot = g_battleAnims.entities[a0].linkedIdx;
    linked = &g_battleAnims.entities[slot];
    if (a1) {
        linked->fieldC3 |= 0x80;
    } else {
        linked->fieldC3 &= 0x7F;
    }
}


INCLUDE_ASM("asm/nonmatchings/thread", func_80027CF8);


INCLUDE_ASM("asm/nonmatchings/thread", func_80027DB4);


/**
 * @brief Get opacity of a battle animation entity.
 * @param idx Entity index (masked to 0 or 1).
 * @return Opacity value (0xFF = visible, 0 = hidden).
 */
s32 getBattleAnimOpacity(s32 idx) {
    BattleAnimEntity *entry = &g_battleAnims.entities[idx & 1];
    return entry->opacity;
}


