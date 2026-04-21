#include "common.h"
#include "sound.h"

extern u8 D_800780D8[];
extern u8 D_800C4DCC[];
extern u8 D_800C4FD3;
extern u8 D_800C4FD4;
extern u8 D_800C4FD5;
extern u8 D_800C4FD6;
extern u8 D_800C4FD7;
extern u8 D_800D23D8[];

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_800997E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_80099B48);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_80099C84);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_80099EDC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_80099F78);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009A4DC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009A638);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009A7C0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009A954);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009AD3C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009AEE4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009B358);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009B550);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009B748);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009B840);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009B954);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009BFA0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C070);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C1A4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C294);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C478);

extern void func_800471E4(s32 category, s32 rc);

/**
 * @brief Fatal-error thunk that invokes the PSX SDK SystemError with category 0x57.
 *
 * Forwards @p rc to SystemError (func_800471E4) with a fixed category code.
 * Callers pass a line-number-like token so the fault site can be identified
 * post-mortem.
 *
 * @note Category values in use elsewhere: field_engine → 0x53, battle_code
 *       → 0x59, world_engine → 0x57. They happen to be ASCII ('S','Y','W')
 *       but the mapping to subsystem isn't obvious — purpose uncertain.
 *
 * @param rc Diagnostic code (typically a unique line/site identifier).
 */
void func_8009C528(s32 rc) {
    func_800471E4(0x57, rc);
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C54C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C5FC);

extern void sndSeqSetTempoAlt(s32 tempo);
extern void sndSetMasterVolume(s32 vol);
extern void sndCmdF1(void);

/**
 * @brief Silence the audio subsystem: zero tempo, zero master volume, send cmd F1.
 *
 * Likely used when leaving a map or suspending sound playback. The @c sndCmdF1
 * call appears in similar "stop audio" sequences elsewhere in the codebase.
 */
void func_8009C69C(void) {
    sndSeqSetTempoAlt(0);
    sndSetMasterVolume(0);
    sndCmdF1();
}

extern void sndSetChannelVolume(s32 channel, s32 vol);

/**
 * @brief Set a sound channel's volume, narrowing the volume arg to signed 8 bits.
 *
 * Forwards to sndSetChannelVolume with @p vol sign-extended from s8 to s32
 * (the canonical `sll 24 / sra 24` pair). Useful when the caller only has
 * an s8 volume value that needs to be promoted into the s32 SDK signature.
 *
 * @param channel Sound channel index.
 * @param vol Volume level treated as signed 8-bit.
 */
void func_8009C6CC(s32 channel, s8 vol) {
    sndSetChannelVolume(channel, vol);
}

extern SeqEntry D_800C4FD8[];
extern void sndSeqStartPan(s32 a0, s32 a1, s32 a2, s32 a3);

/**
 * @brief Start a sound sequence with pan for entry @p idx of the table.
 *
 * Looks up the sequence-table entry at @c D_800C4FD8[idx] and dispatches
 * @c sndSeqStartPan with its @c field10 and @c field04, plus the caller's
 * @p a1 and @p pan (sign-extended from s8).
 *
 * @param idx Index into the sequence table.
 * @param a1 Sound parameter forwarded as the third arg.
 * @param pan Signed 8-bit pan value (sign-extended to s32 for the ABI).
 */
void func_8009C6F0(s32 idx, s32 a1, s8 pan) {
    sndSeqStartPan(D_800C4FD8[idx].field10, D_800C4FD8[idx].field04, a1, pan);
}

extern void sndCmd21(s32 a0, s32 a1);

/**
 * @brief Stop the sound sequence associated with table entry @p idx.
 *
 * Dispatches @c sndCmd21(entry->field10, entry->field04) then clears the
 * entry's runtime-state halfword at +0x12.
 *
 * @param idx Index into the @c D_800C4FD8 sequence table.
 */
void func_8009C738(s32 idx) {
    sndCmd21(D_800C4FD8[idx].field10, D_800C4FD8[idx].field04);
    D_800C4FD8[idx].field12 = 0;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C780);

/** Sets bit 0x20 on two related flag bytes. */
void func_8009C7DC(void) {
    D_800780D8[0x108] |= 0x20;
    D_800D23D8[0x66] |= 0x20;
}

/** Clears bit 0x20 on two related flag bytes. */
void func_8009C808(void) {
    D_800780D8[0x108] &= ~0x20;
    D_800D23D8[0x66] &= ~0x20;
}

extern s32 D_800C4D94;
extern void fadeOutSfxSlow(s32 idx);

/**
 * @brief Fade out SFX slot tracked by D_800C4D94 (if any) and clear the slot.
 *
 * If D_800C4D94 holds a non-negative value (an active slot handle), calls
 * @c fadeOutSfxSlow(1) to fade channel 1, then resets the tracker to -1
 * (inactive). A no-op when already inactive.
 */
void func_8009C834(void) {
    if (D_800C4D94 >= 0) {
        fadeOutSfxSlow(1);
        D_800C4D94 = -1;
    }
}

/** Advances index and returns difference from table lookup. */
u8 func_8009C870(void) {
    u8 idx;
    D_800C4FD6++;
    idx = D_800C4FD6;
    if (idx == 0) {
        D_800C4FD5 += 0xD;
    }
    return (D_800C4DCC[D_800C4FD6] - D_800C4FD5) & 0xFF;
}

/** Sets two related byte values. */
void func_8009C8CC(s32 val) {
    D_800C4FD6 = val;
    D_800C4FD5 = val;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009C8E0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009CA34);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009CAE0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object1", func_8009CB70);

void func_8009CC34(void) {
}

/** @brief Advance D_800C4FD4 index, add 0xD to D_800C4FD3 on wraparound, return table diff. */
s32 func_8009CC3C(void) {
    extern u8 D_800C4FD4;
    extern u8 D_800C4FD3;
    extern u8 D_800C4DCC[];

    D_800C4FD4++;
    if ((u8)D_800C4FD4 == 0) {
        D_800C4FD3 += 0xD;
    }
    return (u8)(D_800C4DCC[D_800C4FD4] - D_800C4FD3);
}

/** @brief Increment D_800C4FD7 index, return D_800C4DCC table value at new index. */
s32 func_8009CC98(void) {
    extern u8 D_800C4FD7;
    extern u8 D_800C4DCC[];

    D_800C4FD7++;
    return D_800C4DCC[D_800C4FD7];
}

/** Sets two related byte values. */
void func_8009CCC8(s32 val) {
    D_800C4FD4 = val;
    D_800C4FD3 = val;
}

/** Sets a single byte value. */
void func_8009CCDC(s32 val) {
    D_800C4FD7 = val;
}
