#include "common.h"
#include "battle.h"
#include "battle/bc_object16.h"
#include "psxsdk/libgte.h"
#include "battle/bc_object8.h"
#include "battle/bc_object14.h"

void func_800A5454(void);

extern u8 D_800FB42C[];
extern u8 D_800FB428[];
extern u8 D_800FB434[];
extern u8 D_800FB438[];
extern u8 D_800FB43C[];
extern u8 D_800FB444[];
extern u8 D_800FB448[];
extern u8 D_800F1B80[];
extern u8 D_800FB408[];


u32 func_8009A2E0(void);
void func_800D13CC(void);
void func_800D5E48(void);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CA078);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CA344);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CA668);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CA9F0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CADD4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CB0DC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CB31C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CB464);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CB820);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CBC68);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CBEAC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CBFA8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CC360);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CC4FC);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CC6F0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CCB54);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CCCB8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CD1C0);

/**
 * @brief Draw one particle and step it, unless the battle is held.
 *
 * The draw happens either way. @ref BATTLE_STATE_UNK001 gates only the step,
 * so a held battle keeps redrawing the particle where it stands.
 *
 * @param p Particle entry to draw and step.
 * @return @c 2 once the particle has expired, @c 0 otherwise.
 */
s32 func_800CD35C(ParticleEntry *p) {
    SVECTOR     rot;
    MATRIX      m;
    VECTOR      scale;
    BattleEffectPrim *prim;
    s32         newFrame;

    rot.vx = 0;
    rot.vy = p->angle;
    rot.vz = 0;
    RotMatrix(&rot, &m);

    m.t[0]   = p->posX;
    m.t[1]   = p->posY;
    m.t[2]   = p->posZ;
    scale.vx = p->sizeX;
    scale.vy = p->sizeY;
    scale.vz = p->sizeX;
    ScaleMatrix(&m, &scale);
    CompMatrix(&D_800F02C8, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    prim = (BattleEffectPrim *)func_800B3698(0x58);
    prim->dispatch = (s32 *)func_800C6B1C(4);
    prim->cmd      = 0x3867;
    prim->flags    = 0x230;

    if ((s16)p->frame >= 8) {
        prim->bgB = 0;
        prim->bgG = 0;
        prim->bgR = 0;
        prim->depth = ((s16)p->frame - 8) * 341;
        prim->flags |= 0xC0;
    }

    D_800FA5F0 = func_800CBC68(prim, D_800FA5E8->ot, 2, D_800FA5F0);
    func_800B36B8(0x58);

    if (D_800EEC5C & BATTLE_STATE_UNK001) {
        return 0;
    }

    p->angle  += p->angVel;
    p->angVel -= (s16)p->angVel >> 4;
    p->sizeX  += p->sizeXVel;
    if ((s16)p->frame < 5) {
        p->sizeY    += p->sizeYVel;
        p->sizeYVel -= (s16)p->sizeYVel >> 3;
    } else {
        p->sizeY    -= (s16)p->sizeYVel / 2;
        p->sizeYVel -= (s16)p->sizeYVel >> 3;
    }
    newFrame = p->frame + 1;
    p->frame = newFrame;
    return ((s16)newFrame > 20) * 2;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CD594);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CD98C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CDB94);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CDD30);

/**
 * @brief As @ref func_800CD35C, for a particle that waits before it appears.
 *
 * A positive @c delay counts down in place of the draw. @ref
 * BATTLE_STATE_UNK001 freezes the whole thing: neither the countdown nor the
 * size step runs while it is set. Past the delay it draws with the entry's own
 * command word, and differs from its sibling in its dispatch index, in
 * switching behaviour at frame 4 rather than 8, and in decaying @c sizeYVel
 * with a signed divide rather than a shift.
 *
 * @param p Particle entry to draw and step.
 * @return @c 2 once the particle has expired, @c 0 otherwise.
 */
s32 func_800CDF3C(ParticleEntry *p) {
    SVECTOR     rot;
    MATRIX      m;
    VECTOR      scale;
    BattleEffectPrim *prim;

    if (p->delay > 0) {
        if (D_800EEC5C & BATTLE_STATE_UNK001) {
            return 0;
        }
        p->delay = p->delay - 1;
        return 0;
    }

    rot.vx = 0;
    rot.vy = p->angle;
    rot.vz = 0;
    RotMatrix(&rot, &m);

    m.t[0]   = p->posX;
    m.t[1]   = p->posY;
    m.t[2]   = p->posZ;
    scale.vx = p->sizeX;
    scale.vy = p->sizeY;
    scale.vz = p->sizeX;
    ScaleMatrix(&m, &scale);
    CompMatrix(&D_800F02C8, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    prim = (BattleEffectPrim *)func_800B3698(0x58);
    prim->dispatch = (s32 *)func_800C6B1C(3);
    prim->cmd      = p->cmdWord;
    prim->flags    = 0x233;

    if ((s16)p->frame >= 4) {
        prim->bgB = 0;
        prim->bgG = 0;
        prim->bgR = 0;
        prim->depth = ((s16)p->frame - 4) * 409;
        prim->flags |= 0xC0;
    }

    D_800FA5F0 = func_800CBC68(prim, D_800FA5E8->ot, 2, D_800FA5F0);
    func_800B36B8(0x58);

    if (!(D_800EEC5C & BATTLE_STATE_UNK001)) {
        p->sizeX    = p->sizeX + p->sizeXVel;
        p->sizeY    = p->sizeY + p->sizeYVel;
        p->sizeYVel = p->sizeYVel - p->sizeYVel / 3;
        {
            u32 newFrame = p->frame + 1;
            p->frame = newFrame;
            return ((s16)newFrame > 14) << 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CE158);

/**
 * @brief Register a particle effect with callback func_800CE158.
 *
 * Allocates an entry from D_800FB408, sets initial fields, copies
 * the source entity's word at offset 0x20, and returns 2.
 *
 * @param a0 Source entity pointer.
 * @return Always 2.
 */
s32 func_800CE77C(u8 *a0) {
    u8 *entry = func_800B2A84(D_800FB408, func_800CE158);
    *(u16 *)(entry + 0xC) = 0;
    *(u16 *)(entry + 0xE) = 0;
    *(u16 *)(entry + 0x1C) = 0x3B27;
    *(s32 *)(entry + 0x20) = *(s32 *)(a0 + 0x20);
    return 2;
}

/**
 * @brief Register a particle effect with callback func_800CE158 (variant).
 *
 * Same as func_800CE77C but with constant 0x3C67 at offset 0x1C.
 *
 * @param a0 Source entity pointer.
 * @return Always 2.
 */
s32 func_800CE7D4(u8 *a0) {
    u8 *entry = func_800B2A84(D_800FB408, func_800CE158);
    *(u16 *)(entry + 0xC) = 0;
    *(u16 *)(entry + 0xE) = 0;
    *(u16 *)(entry + 0x1C) = 0x3C67;
    *(s32 *)(entry + 0x20) = *(s32 *)(a0 + 0x20);
    return 2;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CE82C);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CE918);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CEA7C);

/**
 * @brief Empty routine — present in the original build and still called by the
 *        battle code path; the compiler emits a bare @c "jr ra" for it.
 * @note Purpose uncertain: likely a stub left where a debug or platform hook
 *       was compiled out.
 */
void func_800CEBE4(void) {
}

/**
 * @brief Store a byte value to D_800FB42C.
 *
 * @param a0 Value to store (low byte).
 */
void func_800CEBEC(s32 a0) {
    *(u8 *)D_800FB42C = (u8)a0;
}

/**
 * @brief Update D_800FB428 using modular arithmetic.
 *
 * Computes val * 125 + 14, then stores the result modulo 32768
 * back to D_800FB428. Used as a linear congruential random number step.
 */
void func_800CEBF8(void) {
    s32 val = *(s32 *)D_800FB428;
    val = val * 125 + 14;
    *(s32 *)D_800FB428 = val % 32768;
}

/**
 * @brief Wrapper for func_800CEBF8.
 */
void func_800CEC38(void) {
    func_800CEBF8();
}

/**
 * @brief Store a word value to D_800FB428.
 *
 * @param a0 Value to store.
 */
void func_800CEC58(s32 a0) {
    *(s32 *)D_800FB428 = a0;
}

/**
 * @brief Set bit 31 of D_800F1B80 and call enableSoundReverb.
 *
 * @param a0 Argument passed through to enableSoundReverb.
 */
void func_800CEC64(s32 a0) {
    *(s32 *)D_800F1B80 |= 0x80000000;
    enableSoundReverb(a0);
}

/**
 * @brief Call func_800B34B0, then clear bit 31 of D_800F1B80.
 */
void func_800CEC94(void) {
    func_800B34B0();
    *(s32 *)D_800F1B80 &= 0x7FFFFFFF;
}

/**
 * @brief Check scratchpad status bytes and return incremented value.
 *
 * If byte at 0x1F8003BD (offset 0x2D from 0x1F800390) is not 2, returns 0.
 * If it is 2, checks byte at 0x1F8003BB (offset 0x2B): if its signed value
 * is non-negative, returns that value + 1; otherwise returns 0.
 *
 * @return Incremented scratchpad byte value, or 0.
 */
s32 func_800CECC8(void) {
    u8 *spad = (u8 *)0x1F800390;
    if ((s8)spad[0x2D] != 2) {
        return 0;
    }
    if ((s8)spad[0x2B] >= 0) {
        return (s8)spad[0x2B] + 1;
    }
    return 0;
}

/**
 * @brief Set or clear bit 4 of scratchpad control halfword at 0x1F8003AE.
 *
 * If a0 is nonzero, sets bit 4 (OR 0x10). If zero, clears bit 4 (AND ~0x10).
 *
 * @param a0 Nonzero to set, zero to clear.
 */
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CED0C);

/**
 * @brief Return the word value at D_800FB448.
 *
 * @return Current value of D_800FB448.
 */
s32 func_800CED3C(void) {
    return *(s32 *)D_800FB448;
}

/**
 * @brief Return the byte value at D_800FB444.
 *
 * @return Current value of D_800FB444 (unsigned byte).
 */
s32 func_800CED4C(void) {
    return *(u8 *)D_800FB444;
}

/**
 * @brief Increment D_800FB448 counter and set D_800FB444 flag to 1.
 */
void func_800CED5C(void) {
    s32 val = *(s32 *)D_800FB448;
    val += 1;
    *(volatile s32 *)D_800FB448 = val;
    *(volatile u8 *)D_800FB444 = 1;
}

/**
 * @brief Clear D_800FB448 word and D_800FB444 byte.
 */
void func_800CED84(void) {
    *(s32 *)D_800FB448 = 0;
    *(volatile u8 *)D_800FB444 = 0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CED9C);

/**
 * @brief Read bit 1 from scratchpad control register at 0x1F8003AE.
 *
 * @return 1 if bit 1 is set, 0 otherwise.
 */
s32 func_800CEDA4(void) {
    return (*(u16 *)0x1F8003AE >> 1) & 1;
}

/**
 * @brief Read bit 2 from scratchpad control register at 0x1F8003AE.
 *
 * @return 1 if bit 2 is set, 0 otherwise.
 */
s32 func_800CEDBC(void) {
    return (*(u16 *)0x1F8003AE >> 2) & 1;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CEDD4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CEDF8);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CEE34);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CEFA0);

/**
 * @brief Temporarily shrink entry dimensions by 2 and call func_800CEFA0.
 *
 * Saves the word at entry+4, subtracts 2 from halfwords at offsets 4 and 6,
 * calls func_800CEFA0 with the original a0/a1, then restores the word.
 *
 * @param a0 First parameter passed through to func_800CEFA0.
 * @param a1 Second parameter passed through to func_800CEFA0.
 * @param entry Pointer to entry data.
 */
void func_800CF060(s32 a0, s32 a1, u8 *entry) {
    s32 saved = *(s32 *)(entry + 4);
    *(u16 *)(entry + 4) = *(u16 *)(entry + 4) - 2;
    *(u16 *)(entry + 6) = *(u16 *)(entry + 6) - 2;
    func_800CEFA0(a0, a1);
    *(s32 *)(entry + 4) = saved;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF0B0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF164);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF1C0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF2D0);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF300);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF33C);

/**
 * @brief Process frame update with optional visual effect.
 *
 * Returns early if func_8009A2E0 indicates processing is blocked.
 * Otherwise calls func_800D13CC, clears scratchpad halfword at
 * gp+0x1C, and conditionally calls func_800D5E48 if bit 6 of the
 * scratchpad status halfword at gp+0x1E is set. Always calls
 * func_800A5454.
 */
INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF38C);

/**
 * @brief Return the word value at D_800FB434.
 *
 * @return Current value of D_800FB434.
 */
s32 func_800CF3E0(void) {
    return *(s32 *)D_800FB434;
}

/**
 * @brief Store a word value to D_800FB438.
 *
 * @param a0 Value to store.
 */
void func_800CF3F0(s32 a0) {
    *(s32 *)D_800FB438 = a0;
}

/**
 * @brief Return the word value at D_800FB438.
 *
 * @return Current value of D_800FB438.
 */
s32 func_800CF3FC(void) {
    return *(s32 *)D_800FB438;
}

/**
 * @brief Store a word value to D_800FB43C.
 *
 * @param a0 Value to store.
 */
void func_800CF40C(s32 a0) {
    *(s32 *)D_800FB43C = a0;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF418);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF4D4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF4F4);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object16", func_800CF630);
