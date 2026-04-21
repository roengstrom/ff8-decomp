#include "common.h"
#include "field.h"
#include "sound.h"

extern u8 D_800780D8[];
extern u8 D_800D23D8[];
extern FieldEngineState *D_800562C4;

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009CCE8);

extern s32 sndCmd1A(s32 a0, s32 a1, s32 a2);

/**
 * @brief Dispatch SPU cmd 0x1A with (@p a0, 0x78, @p val), stash return as handle.
 *
 * Sister of func_8009CDFC: saves the return value of
 * @c sndCmd1A(a0, 0x78, val) — where @p val is sign-extended from s8 —
 * into the field-engine state's @c soundHandle0 slot.
 *
 * @note @c sndCmd1A is declared here as returning @c s32 for the same reason
 *       as func_8009CDFC's @c sndCmd10 override.
 *
 * @param a0 Parameter for SPU command 0x1A.
 * @param val Signed 8-bit value forwarded as the third arg.
 */
void func_8009CDC4(s32 a0, s8 val) {
    D_800562C4->soundHandle0 = sndCmd1A(a0, 0x78, val);
}

extern s32 sndCmd10(s32 a0);
extern void sndCmdC1(s32 a0, s32 a1, s32 a2);

/**
 * @brief Dispatch SPU cmd 0x10 with @p a0, stash its handle, then issue cmd C1.
 *
 * Saves the return value of @c sndCmd10(a0) into the field-engine state's
 * @c soundHandle0 slot, then calls @c sndCmdC1(0, 0x3C, val) where @p val
 * is sign-extended from s8.
 *
 * @note @c sndCmd10 is declared here as returning @c s32 even though its
 *       definition in snd_init.c is @c void — the return register (v0)
 *       carries the chained return from @c func_8001A1E8 that the caller
 *       uses here. Overlay-local signature override.
 *
 * @param a0 Parameter for SPU command 0x10.
 * @param val Signed 8-bit value forwarded to sndCmdC1 as the third arg.
 */
void func_8009CDFC(s32 a0, s8 val) {
    D_800562C4->soundHandle0 = sndCmd10(a0);
    sndCmdC1(0, 0x3C, val);
}

extern void sndCmd11(s32 a0);

/**
 * @brief Send sound command 0x11 then reset audio channel 0's state byte.
 *
 * Issues @c sndCmd11(0) and marks audio channel 0 as inactive by writing
 * -1 to the field-engine state's @c audioChannel0State byte.
 */
void func_8009CE40(void) {
    sndCmd11(0);
    D_800562C4->audioChannel0State = -1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009CE70);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009CFB4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D0F0);

extern SeqEntry D_800C4FD8[];
extern void func_80099EDC(s32 idx);

/**
 * @brief If sequence @p idx isn't already running, kick it off via func_80099EDC.
 *
 * Checks @c D_800C4FD8[idx].field12 — if it's 0 (idle), calls
 * @c func_80099EDC(idx) to start the sequence and returns 1. If already
 * running (non-zero field12), returns 0 without doing anything.
 *
 * @param idx Index into the @c D_800C4FD8 sequence table.
 * @return 1 if the sequence was started, 0 if it was already running.
 */
s32 func_8009D16C(s32 idx) {
    if (D_800C4FD8[idx].field12 != 0) return 0;
    func_80099EDC(idx);
    return 1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D1B8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D214);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D2D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D3F4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D44C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D510);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D630);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D688);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D760);

/** @brief Compare input against two entity IDs, return 0x29, 0x18, or -1. */
s32 func_8009D7D8(s32 a0) {
    extern s32 D_800C9E68;
    extern s32 D_800C9E70;
    s32 result = -1;

    if (D_800C9E68 == a0) {
        result = 0x29;
    } else if (D_800C9E70 == a0) {
        result = 0x18;
    }
    return result;
}

/** Clears bit 0x40 on two related flag bytes. */
void func_8009D814(void) {
    D_800780D8[0x108] &= ~0x40;
    D_800D23D8[0x66] &= ~0x40;
}

/**
 * @brief Clear two flag bytes at entity-relative offsets.
 * @param a0 Entity slot index offset.
 */
void func_8009D840(s32 a0) {
    u8 *base1 = D_800780D8;
    u8 *base2 = D_800D23D8;

    *(u8 *)(a0 + (s32)base1 + 0x10D) = 0;
    *(u8 *)(a0 + (s32)base2 + 0x6B) = 0;
}

extern SfxSlot D_800C526C[];
extern s32 func_8002CE84(s32 idx);

/**
 * @brief Dispatch a table-driven SFX slot cleanup if the slot is active.
 *
 * Looks up @c D_800C526C[idx]; if its @c field00 isn't -1 (active),
 * forwards its @c field02 to @c func_8002CE84 (an SFX helper in btl_sfx).
 *
 * @param idx Index into the SFX slot table.
 */
void func_8009D864(s32 idx) {
    if (D_800C526C[idx].field00 != -1) {
        func_8002CE84(D_800C526C[idx].field02);
    }
}

extern void fadeOutSfxSlow(s32 idx);

/**
 * @brief Fade out the SFX referenced by slot @p idx and mark the slot inactive.
 *
 * If the slot is active (@c field00 != -1), writes -1 to deactivate it and
 * calls @c fadeOutSfxSlow with the slot's SFX index (@c field02).
 *
 * @param idx Index into the @c D_800C526C SFX slot table.
 */
void func_8009D8A8(s32 idx) {
    s32 field00 = D_800C526C[idx].field00;
    s32 field02 = D_800C526C[idx].field02;
    if (field00 != -1) {
        D_800C526C[idx].field00 = -1;
        fadeOutSfxSlow(field02);
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D8F0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009D954);

extern s32 getSfxField28(s32 idx);

/**
 * @brief Return whether SFX slot @p idx is active AND its SFX has pending state.
 *
 * Checks @c D_800C526C[idx].field00 — if the slot is inactive (-1) returns 0.
 * Otherwise queries @c getSfxField28 with the slot's SFX index and returns
 * 1 iff the result is nonzero, else 0.
 *
 * @param idx Index into the @c D_800C526C SFX slot table.
 * @return 1 if slot is active and its SFX is busy, else 0.
 */
s32 func_8009D9C8(s32 idx) {
    s32 field00 = D_800C526C[idx].field00;
    s32 field02 = D_800C526C[idx].field02;
    s32 result = 0;
    if (field00 != -1) {
        result = getSfxField28(field02) != 0;
    }
    return result;
}

/**
 * @brief Find the first active SFX slot in the high range [9, 13).
 *
 * Scans @c D_800C526C[9..12] looking for a slot whose @c field00 is
 * not -1 (active). Returns the matching slot index, or -1 if none.
 *
 * @return First active slot index in [9, 12], or -1 if all inactive.
 */
s32 func_8009DA10(void) {
    s32 i;
    for (i = 9; i < 13; i++) {
        if (D_800C526C[i].field00 != -1) {
            return i;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009DA54);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009DAA8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009DB88);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009E5C8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009F594);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009F6EC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009FDA4);

extern void func_80040FE4(s32 a0, u8 *buf);
extern void func_80040264(u8 *buf, s32 a1);
extern void func_800406D4(u8 *buf);

/**
 * @brief Initialize, populate and finalize a 32-byte transform buffer on the stack.
 *
 * Allocates a 0x20-byte stack buffer and runs a three-step pipeline:
 *  1. @c func_80040FE4(a0, buf) — seed the buffer from @p a0.
 *  2. @c func_80040264(buf, a1) — apply/combine @p a1 into the buffer.
 *  3. @c func_800406D4(buf) — finalize/commit the buffer.
 *
 * @note Purpose uncertain — the pipeline matches a matrix/transform setup
 *       pattern used elsewhere in battle_code.
 *
 * @param a0 First input to the pipeline (seed).
 * @param a1 Second input (applied during step 2).
 */
void func_8009FE80(s32 a0, s32 a1) {
    u8 buf[0x20];
    func_80040FE4(a0, buf);
    func_80040264(buf, a1);
    func_800406D4(buf);
}

extern void func_800A017C(s32 *vec);

/**
 * @brief Forward @p vec to func_800A017C, ignoring the first arg register.
 *
 * Thin wrapper — takes two register args but only passes the second on.
 *
 * @param unused First arg register, ignored.
 * @param vec Pointer forwarded as the first arg of func_800A017C.
 */
void func_8009FEBC(s32 unused, s32 *vec) {
    func_800A017C(vec);
}

/** @brief Clear a 15-byte animation structure and set type byte at offset 0xA. */
void func_8009FEDC(u8 *a0, u8 a1) {
    a0[0xE] = 0;
    a0[0xD] = 0;
    a0[0xC] = 0;
    a0[0xB] = 0;
    a0[0xA] = 0;
    a0[0x9] = 0;
    a0[0x8] = 0;
    *(u16 *)(a0 + 0x0) = 0;
    *(u16 *)(a0 + 0x2) = 0;
    *(u16 *)(a0 + 0x4) = 0;
    a0[0xA] = a1;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009FF0C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_8009FF70);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_800A0000);

/** Returns a value based on input comparison. */
s32 func_800A009C(s32 val) {
    if (val == 0x32) {
        return 0x1800;
    }
    return 0x1460;
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_800A00B4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object2", func_800A017C);
