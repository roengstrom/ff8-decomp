#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "main.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "field/fe_object1.h"
#include "field/fe_object1_2.h"
#include "field/fe_object10.h"

/**
 * @brief Arm every camera slot whose move has not started yet.
 *
 * Scans the eight @c g_fieldEntity.slots for ones still in @c submode @c 0 and
 * gets them ready for @c func_800A1318 to tick: the step counter @c unk06 is
 * zeroed and the current position @c q1 / @c q2 is snapshotted into
 * @c savedQ1 / @c savedQ2, which is the "from" end of the interpolation.
 *
 * @c mode then decides the starting sub-mode: the five easing/follow modes
 * (@c 0, @c 1, @c 2, @c 4, @c 5) go to @c submode @c 1 (move running), while
 * @c mode @c 3 is the instant jump — it goes straight to @c submode @c 2
 * (finished) and copies the target @c p1 / @c p2 over @c q1 / @c q2 there and
 * then. Any other @c mode is left alone.
 */
void func_800A10F4(void) {
    s16 i;
    for (i = 0; i < 8; i++) {
        if (g_fieldEntity.slots[i].submode == 0) {
            g_fieldEntity.slots[i].unk06 = 0;
            g_fieldEntity.slots[i].savedQ1 = g_fieldEntity.slots[i].q1;
            g_fieldEntity.slots[i].savedQ2 = g_fieldEntity.slots[i].q2;
            switch (g_fieldEntity.slots[i].mode) {
                case 0:
                case 1:
                case 2:
                case 4:
                case 5:
                    g_fieldEntity.slots[i].submode = 1;
                    break;
                case 3:
                    g_fieldEntity.slots[i].submode = 2;
                    g_fieldEntity.slots[i].q1 = g_fieldEntity.slots[i].p1;
                    g_fieldEntity.slots[i].q2 = g_fieldEntity.slots[i].p2;
                    break;
            }
        }
    }
}

/**
 * @brief Build an @c SVECTOR from the active slot's entity position and
 *        project it through @c func_800A0F34, then call @c func_800A0FB8
 *        with a flag selected by the active-slot index.
 *
 * Reads @c g_fieldEntity.slots[unk1A6].param to pick an @ref Actor entity,
 * fills @c svec.{vx,vy,vz} from its @c posX/posY/posZ shifted right by
 * @c 12, biasing @c vz by @c D_8005F0F8->baseZ. The projection result is
 * latched to @c D_800C71FC. The trailing @c func_800A0FB8 clamp call gets
 * flag @c 0 when @c unk1A6 @c == @c 0 and flag @c 1 otherwise.
 *
 * @param out     Screen-space position, written by @c func_800A0F34 and then
 *                clamped in place by @c func_800A0FB8.
 * @param slotIdx Caller's slot index. Unused — the entity is picked from
 *                @c g_fieldEntity.unk1A6 instead — but @c func_800A1318 passes it.
 */
void func_800A11E0(Vec2s *out, s16 slotIdx) {
    SVECTOR svec;

    svec.vx = D_80085224[g_fieldEntity.slots[g_fieldEntity.unk1A6].param].posX >> 12;
    svec.vy = D_80085224[g_fieldEntity.slots[g_fieldEntity.unk1A6].param].posY >> 12;
    svec.vz = (D_80085224[g_fieldEntity.slots[g_fieldEntity.unk1A6].param].posZ >> 12) +
              D_8005F0F8->baseZ;
    D_800C71FC = func_800A0F34(&svec, (s32 *)out);
    if (g_fieldEntity.unk1A6 == 0) {
        func_800A0FB8(out, 0, 0);
    } else {
        func_800A0FB8(out, 1, 0);
    }
}

/**
 * @brief Advance every camera slot that has a move in progress.
 *
 * Walks the eight @c g_fieldEntity.slots and ticks each one whose @c submode is
 * @c 1 (move running). @c mode picks how the slot's camera position
 * @c q1 / @c q2 — the scroll origin @c func_800A15C0 later turns into the
 * frame's draw offset — is driven towards its target:
 *
 *  - @c 0: hard-lock to the party entity's projected screen position, but only
 *    for the active slot (@c g_fieldEntity.unk1A6); every other slot is zeroed.
 *  - @c 1 / @c 2: ease towards that live projected position, linearly
 *    (@c func_800A0E54) or sine-eased (@c func_800A0EB8).
 *  - @c 3: nothing.
 *  - @c 4 / @c 5: same two curves, but towards the fixed script-supplied point
 *    in @c p1 / @c p2 rather than following an entity.
 *
 * Every easing mode shares one step counter: @c unk06 advances by one per tick
 * until it reaches @c timer, at which point the slot drops to @c submode @c 2
 * to mark the move finished.
 */
void func_800A1318(void) {
    Vec2s pos;
    s16 i;

    for (i = 0; i < 8; i++) {
        if (g_fieldEntity.slots[i].submode != 1) {
            continue;
        }
        switch (g_fieldEntity.slots[i].mode) {
        case 0:
            func_800A11E0(&pos, i);
            if (i == g_fieldEntity.unk1A6) {
                g_fieldEntity.slots[i].q1 = pos.x;
                g_fieldEntity.slots[i].q2 = pos.y;
            } else {
                g_fieldEntity.slots[i].q1 = 0;
                g_fieldEntity.slots[i].q2 = 0;
            }
            break;
        case 1:
            func_800A11E0(&pos, i);
            g_fieldEntity.slots[i].q1 = func_800A0E54((s16)g_fieldEntity.slots[i].savedQ1, pos.x,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            g_fieldEntity.slots[i].q2 = func_800A0E54((s16)g_fieldEntity.slots[i].savedQ2, pos.y,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            if ((s16)g_fieldEntity.slots[i].timer == (s16)g_fieldEntity.slots[i].unk06) {
                g_fieldEntity.slots[i].submode = 2;
            } else {
                g_fieldEntity.slots[i].unk06++;
            }
            break;
        case 2:
            func_800A11E0(&pos, i);
            g_fieldEntity.slots[i].q1 = func_800A0EB8((s16)g_fieldEntity.slots[i].savedQ1, pos.x,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            g_fieldEntity.slots[i].q2 = func_800A0EB8((s16)g_fieldEntity.slots[i].savedQ2, pos.y,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            if ((s16)g_fieldEntity.slots[i].timer == (s16)g_fieldEntity.slots[i].unk06) {
                g_fieldEntity.slots[i].submode = 2;
            } else {
                g_fieldEntity.slots[i].unk06++;
            }
            break;
        case 3:
            break;
        case 4:
            g_fieldEntity.slots[i].q1 = func_800A0E54((s16)g_fieldEntity.slots[i].savedQ1, (s16)g_fieldEntity.slots[i].p1,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            g_fieldEntity.slots[i].q2 = func_800A0E54((s16)g_fieldEntity.slots[i].savedQ2, (s16)g_fieldEntity.slots[i].p2,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            if ((s16)g_fieldEntity.slots[i].timer == (s16)g_fieldEntity.slots[i].unk06) {
                g_fieldEntity.slots[i].submode = 2;
            } else {
                g_fieldEntity.slots[i].unk06++;
            }
            break;
        case 5:
            g_fieldEntity.slots[i].q1 = func_800A0EB8((s16)g_fieldEntity.slots[i].savedQ1, (s16)g_fieldEntity.slots[i].p1,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            g_fieldEntity.slots[i].q2 = func_800A0EB8((s16)g_fieldEntity.slots[i].savedQ2, (s16)g_fieldEntity.slots[i].p2,
                                                   (s16)g_fieldEntity.slots[i].timer, (s16)g_fieldEntity.slots[i].unk06);
            if ((s16)g_fieldEntity.slots[i].timer == (s16)g_fieldEntity.slots[i].unk06) {
                g_fieldEntity.slots[i].submode = 2;
            } else {
                g_fieldEntity.slots[i].unk06++;
            }
            break;
        }
    }
}

/**
 * @brief Point the frame's draw environment at the field camera, plus screen shake.
 *
 * The field renders into two alternating GPU work areas (@ref D_800C7218) that live
 * side by side in VRAM, and each has its own @c DRAWENV — @c env[1] is the one set up
 * at X @c 512 by @c SetDefDrawEnv. @p buf says which work area this frame is being
 * built into, so this picks the matching environment and writes its drawing offset;
 * the @c 0x200 added to @c env[1] 's X is that 512-pixel VRAM separation.
 *
 * The offset is the field's scroll position — the clamp-rect centre
 * (@ref D_800C7210 / @ref D_800C7214) minus the party slot's camera position — plus the
 * two field oscillators' current output as a signed byte, which is what makes the
 * screen shake (@c oscillators[0] on X, @c oscillators[1] on Y), plus the view block's
 * own offset. When @c func_800BE274 reports the overlay subsystem active the field's
 * scroll and shake are skipped and only the centre and the view offset are used.
 *
 * @param buf     The GPU work area this frame is being built into; compared against
 *                @ref D_800C7218 to choose the environment.
 * @param env     The two-element draw-environment array (@c g_drawEnvs).
 * @param slotIdx Party slot whose camera position drives the scroll.
 */
void func_800A15C0(FieldFrameBuf *buf, DRAWENV *env, s16 slotIdx) {
    SetGeomOffset(0, 0);
    if (func_800BE274() == 0) {
        if (buf == D_800C7218) {
            env[0].dispX = (D_800C7210 - g_fieldEntity.slots[slotIdx].q1) + (s8)g_fieldEntity.oscillators[0].output + D_800C71F8->viewOfsX;
            env[0].dispY = (D_800C7214 - g_fieldEntity.slots[slotIdx].q2) + (s8)g_fieldEntity.oscillators[1].output + D_800C71F8->viewOfsY;
        } else {
            env[1].dispX = (D_800C7210 - g_fieldEntity.slots[slotIdx].q1) + (s8)g_fieldEntity.oscillators[0].output + D_800C71F8->viewOfsX + 0x200;
            env[1].dispY = (D_800C7214 - g_fieldEntity.slots[slotIdx].q2) + (s8)g_fieldEntity.oscillators[1].output + D_800C71F8->viewOfsY;
        }
    } else {
        if (buf == D_800C7218) {
            env[0].dispX = D_800C7210 - D_800C71F8->viewOfsX;
            env[0].dispY = D_800C7214 + D_800C71F8->viewOfsY;
        } else {
            env[1].dispX = (D_800C7210 - D_800C71F8->viewOfsX) + 0x200;
            env[1].dispY = D_800C7214 + D_800C71F8->viewOfsY;
        }
    }
}

/**
 * Clears fields at offsets 0, 1 (bytes) and 0xA, 0xC (halfwords) in the structure.
 *
 * @param a0 Pointer to the structure.
 */
void func_800A17A4(u8 *a0) {
    *(u8 *)(a0 + 0x0) = 0;
    *(u8 *)(a0 + 0x1) = 0;
    *(u16 *)(a0 + 0xA) = 0;
    *(u16 *)(a0 + 0xC) = 0;
}

/**
 * @brief Advance one interpolating oscillator / wobble driver.
 *
 * Ticks an @ref Oscillator one step, dispatching on @c mode / @c phase:
 *  - @b mode==1 (continuous): on @c phase==0 it seeds the first target
 *    (@c end = @c (s16)(D_800C3520[tableIdx] * amplitude) / 256) and returns;
 *    otherwise, once @c angle passes @c total it starts a new cycle, the new
 *    @c end takes the @b opposite sign of the previous one (so the value
 *    oscillates), then interpolates.
 *  - @b otherwise: on @c phase==1 it snaps @c start to the previous @c end,
 *    clears @c end, and drops to @c phase==0; on @c phase!=1 it runs until
 *    @c angle reaches @c total, then latches @c output to @c 0.
 *
 * Each non-terminal path interpolates via @c func_800A0EB8(start, end, total,
 * angle), stores the result in @c output, and advances @c angle. Every started
 * cycle also advances @c tableIdx into the @c D_800C3520 waveform table.
 *
 * @param osc The oscillator to advance.
 *
 * @note The empty @c do/while(0) after the target update is a scheduling
 *       barrier: it stops gcc from hoisting the @c tableIdx reload into the
 *       divide, keeping the round-toward-zero @c /256 in @c $v0 to match.
 *       @c delta is @c s32 so the @c (s16) truncation materialises before the
 *       negate (rather than gcc folding @c -(s16)x into @c (s16)(-x)).
 */
void func_800A17B8(Oscillator *osc) {
    if (osc->mode == 1) {
        if (osc->phase == 0) {
            s16 delta;
            osc->angle = 0;
            osc->start = 0;
            delta = (s16)(D_800C3520[osc->tableIdx] * osc->amplitude);
            osc->end = delta / 256;
            osc->phase = 1;
            osc->tableIdx++;
            return;
        }
        if (osc->total < osc->angle) {
            s32 delta;
            s16 old = osc->end;
            osc->angle = 0;
            osc->start = old;
            if (old < 0) {
                delta = (s16)(D_800C3520[osc->tableIdx] * osc->amplitude);
            } else {
                delta = -(s16)(D_800C3520[osc->tableIdx] * osc->amplitude);
            }
            osc->end = delta / 256;
            do {} while (0);
            osc->tableIdx++;
        }
        osc->output = func_800A0EB8(osc->start, osc->end, osc->total, osc->angle);
        osc->angle++;
    } else {
        if (osc->phase == 1) {
            if (osc->total < osc->angle) {
                s16 old = osc->end;
                osc->angle = 0;
                osc->end = 0;
                osc->phase = 0;
                osc->start = old;
                osc->tableIdx++;
            }
            osc->output = func_800A0EB8(osc->start, osc->end, osc->total, osc->angle);
            osc->angle++;
        } else if (osc->total <= osc->angle) {
            osc->output = 0;
        } else {
            osc->output = func_800A0EB8(osc->start, osc->end, osc->total, osc->angle);
            osc->angle++;
        }
    }
}

/**
 * @brief Per-frame field-object vertex transform + clip (hand-written asm).
 *
 * Reads clip bounds from the scratchpad (@c 0x1F800100 / @c 0x1F800104 =
 * min/max X,Y as s16 pairs), walks a stride-0x10 vertex array (@c a3)
 * until @c x == @c 0x7FFF, applies a per-object translate
 * (@c 0x1F800080[idx], idx = byte at @c a3+0xC) with wraparound by the
 * modulus at @c 0x1F800110, writes the clipped X/Y to @c a1+8 / @c a1+0xA,
 * and links GPU primitives via the @c 0xFFFFFF / @c 0x03000000 /
 * @c 0xFF000000 address masks (@c a0 / @c a2, including a @c 0xE1000000
 * draw-mode packet at @c a2).
 *
 * @note Hand-written assembly (own translation unit). Forensics from the
 *       C+asm-macro reconstruction campaign (permuter/func_800A19B8/base.c,
 *       82.93%% plateau, full structural parity): (1) its @c li constants
 *       are ori-form (ASPSX expand_li) while all 166 li sites in the rest
 *       of fe_object1.c are addiu-form (--aspsx-version=2.67), assembled
 *       with different settings than this CU; (2) trapping @c add / @c sub
 *       / @c addi cluster in the wrap/window-test/advance idioms while
 *       addresses use @c addu (the classic hand-asm signed-math/address
 *       convention); (3) the tag-build @c or operand order is
 *       anti-canonical for gcc combine; (4) two branch delay slots are
 *       left as @c nop where dbr had eligible fillers; (5) the register
 *       assignment (mask=t3 allocated before hotter x=t4) is unreachable
 *       by gcc's allocno priority order. Both project compilers emit only
 *       @c addu / @c subu / @c addiu for C arithmetic (re-verified
 *       2026-07-29). @c INCLUDE_ASM builds to a 100%% byte match and is
 *       the faithful source form; the base.c reconstruction documents the
 *       semantics and the residual classes.
 */
INCLUDE_ASM("asm/field/nonmatchings/fe_object1_2", func_800A19B8);

/**
 * @brief Restore a 256x16 VRAM region from a saved buffer and clear
 *        the @c 0x8000 transparency bit on every pixel.
 *
 * Sets @c D_800C71E4 to point at the saved-image buffer @c D_800D3E88,
 * then (only when the current event queue's @c unk0E flag is @c 1)
 * uses @c StoreImage (via @c StoreImage) to write the buffer back
 * to VRAM at @c (0x100, 0x10) with a @c 256x16 RECT, and masks every
 * pixel's @c 0x8000 transparency bit to leave just the colour bits.
 *
 * The two @c DrawSync(1) polls are GPU-busy waits sandwiching
 * the transfer so the buffer isn't read while another command is
 * still being issued.
 */
void func_800A1BB8(void) {
    RECT rect;
    u16 *p;
    s32 i;
    D_800C71E4 = D_800D3E88;
    if (D_8005F0F8->unk0E != 1) return;
    rect.x = 0x200;
    rect.y = 0xF0;
    rect.w = 0x100;
    rect.h = 0x10;
    while (DrawSync(1) != 0) {}
    StoreImage(&rect, (u32 *)D_800C71E4);
    while (DrawSync(1) != 0) {}
    p = D_800C71E4;
    for (i = 0; i < 0x1000; i++) {
        *p &= 0x7FFF;
        p++;
    }
}

/**
 * If D_8005F0F8 byte at offset 0xE is 1, sets up a display region
 * (0x200 x 0xF0 at 0x100, 0x10) and calls LoadImage.
 */
void func_800A1C64(void) {
    u8 *data = (u8 *)D_8005F0F8;

    if (*(u8 *)(data + 0xE) == 1) {
        s16 rect[4];
        rect[0] = 0x200;
        rect[1] = 0xF0;
        rect[2] = 0x100;
        rect[3] = 0x10;
        LoadImage(rect, D_800C71E4);
    }
}

/** @brief Initialize 3 entries in D_800D5F50 and D_800D61A8 arrays to -1. */
void func_800A1CC0(void) {
    s32 i = 0;
    s32 val = -1;
    u8 *a = D_800D5F50;
    u8 *b = D_800D61A8;

    do {
        *(s32 *)b = val;
        *(s32 *)a = val;
        a += 0x70;
        i++;
        b += 0x70;
    } while (i < 3);
}

/**
 * @brief Per-frame turn/aim update for every @ref Actor entity, then flush.
 *
 * For each of the @c D_80085388 entities: publishes the render-slot angle
 * vector (0, 0, @c field_0x241<<4 + 0x400) via @ref func_800A736C and the
 * world position (@c pos>>12 plus @c posOfs) via @ref func_800A7224. When
 * @c turnMode is 1 and a turn is not in progress (@c turnTick == 0), it
 * queries the target height (@ref func_800A8DAC op 0x1E), computes the XY
 * bearing from the entity (with offsets and height) to @c turnTgt via
 * @ref func_8009A0E8 and stores it rate-clamped (+/- @c turnYawRate around
 * the current @c field_0x241 heading) into @c turnYawDst, then likewise the
 * elevation bearing into @c turnPitchDst (clamped by @c turnPitchRate).
 * While @c turnTick < @c turnLen it unwraps both destinations into the
 * +/-0x80 window around their committed angles, advances @c turnTick,
 * interpolates yaw/pitch via @ref func_800A0EB8, emits render op 0x13 with
 * the angle vector, commits Dst->Cur when the turn completes, and emits op
 * 0x25; otherwise, idle entities (@c turnMode == 0) emit ops 0x15 and 0x16.
 * Entities also emit op 0x25 whenever @ref func_800BE274 reports active.
 * Finally @ref func_800A63AC flushes with @p arg1.
 *
 * @param ents Actor entity array (@c D_80085224).
 * @param frame Pass-through context for the @ref func_800A63AC flush.
 */
void func_800A1CFC(Actor *ents, FieldFrameBuf *frame) {
    Vec3i pB;       /* sp+0x10: bearing arg B */
    Vec3i pA;       /* sp+0x20: bearing arg A */
    Vec3s v30;      /* sp+0x30: world-position vector */
    Vec3s v38;      /* sp+0x38: angle vector */
    s16 buf[4];     /* sp+0x40: func_800A8DAC output (buf[2] = target height) */
    s32 dist;       /* sp+0x48: horizontal distance from the first bearing */
    s32 i;
    Actor *ent;
    /* Separate clamp variables per axis: sharing one diff/cmp pair across
       both clamps changes the allocno densities and rotates a0/v1. */
    s32 diff;
    s32 diffB;
    s32 cmp;
    s32 cmpB;
    s32 d2;
    s32 d2B;
    s32 v;

    for (i = 0, ent = ents; i < D_80085388; ent++, i++) {
        v38.z = (ent->field_0x241 << 4) + 0x400;
        v38.x = 0;
        v38.y = 0;
        func_800A736C(i, (u16 *)&v38, 0);
        v30.x = (u16)ent->posOfsX + (ent->posX >> 12);
        v30.y = (u16)ent->posOfsY + (ent->posY >> 12);
        v30.z = (u16)ent->posOfsZ + (ent->posZ >> 12);
        func_800A7224(i, (u16 *)&v30, 0);
        if (ent->turnMode == 1 && ent->turnTick == 0) {
            func_800A8DAC(i, 0x1E, D_800C71F8, buf);
            pB.x = ent->turnTgtX;
            pB.y = ent->turnTgtY;
            pB.z = ent->turnTgtZ;
            pA.x = (ent->posX >> 12) + ent->posOfsX;
            pA.y = (ent->posY >> 12) + ent->posOfsY;
            pA.z = buf[2] + (ent->posZ >> 12) + ent->posOfsZ;
            v = func_8009A0E8((s32 *)&pA, (s32 *)&pB, &dist);
            v &= 0xFF;
            v -= ent->field_0x241;
            diff = (u8)v;
            cmp = ent->turnYawDst = diff;
            if (cmp < 0x80) {
                if (ent->turnYawRate < cmp) {
                    ent->turnYawDst = ent->turnYawRate;
                }
            } else {
                d2 = diff - 0x100;
                ent->turnYawDst = d2;
                if (d2 < -ent->turnYawRate) {
                    ent->turnYawDst = -ent->turnYawRate;
                }
            }
            pB.y = 0;
            pB.x = ent->turnTgtZ;
            pB.z = 0;
            pA.y = dist;
            pA.x = buf[2] + (ent->posZ >> 12) + ent->posOfsZ;
            pA.z = 0;
            v = func_8009A0E8((s32 *)&pA, (s32 *)&pB, &dist);
            diffB = v & 0xFF;
            cmpB = ent->turnPitchDst = diffB;
            if (cmpB < 0x80) {
                if (ent->turnPitchRate < cmpB) {
                    ent->turnPitchDst = ent->turnPitchRate;
                }
            } else {
                d2B = diffB - 0x100;
                ent->turnPitchDst = d2B;
                if (d2B < -ent->turnPitchRate) {
                    ent->turnPitchDst = -ent->turnPitchRate;
                }
            }
        }
        if (ent->turnTick < ent->turnLen) {
            if (ent->turnPitchDst > ent->turnPitchCur) {
                if (ent->turnPitchDst - ent->turnPitchCur > 0x80) {
                    ent->turnPitchDst = (u16)ent->turnPitchDst - 0x100;
                }
            } else {
                if (ent->turnPitchCur - ent->turnPitchDst > 0x80) {
                    ent->turnPitchDst = (u16)ent->turnPitchDst + 0x100;
                }
            }
            if (ent->turnYawDst > ent->turnYawCur) {
                if (ent->turnYawDst - ent->turnYawCur > 0x80) {
                    ent->turnYawDst = (u16)ent->turnYawDst - 0x100;
                }
            } else {
                if (ent->turnYawCur - ent->turnYawDst > 0x80) {
                    ent->turnYawDst = (u16)ent->turnYawDst + 0x100;
                }
            }
            ent->turnTick++;
            v = func_800A0EB8(ent->turnYawCur, ent->turnYawDst, ent->turnLen, ent->turnTick);
            v38.z = -((v & 0xFF) << 4);
            v = func_800A0EB8(ent->turnPitchCur, ent->turnPitchDst, ent->turnLen, ent->turnTick);
            v &= 0xFF;
            v38.y = v << 4;
            v38.x = 0;
            func_800A97E4(i, 0x13, (s32)&v38, 0);
            if (ent->turnTick == ent->turnLen) {
                ent->turnPitchCur = ent->turnPitchDst;
                ent->turnRollCur = ent->turnRollDst;
                ent->turnYawCur = ent->turnYawDst;
            }
            func_800A97E4(i, 0x25, 0, 0);
        } else {
            if (ent->turnMode == 0) {
                func_800A97E4(i, 0x15, 0, 0);
                func_800A97E4(i, 0x16, 0, 0);
            }
        }
        if (func_800BE274() != 0) {
            func_800A97E4(i, 0x25, 0, 0);
        }
    }
    func_800A63AC(frame, D_800C71F8, 0);
}

/**
 * @brief Shape @c func_800A2128 sees: a buffer with a 128-entry
 *        28-byte item table at offset 0x4000 and a 16-entry 8-byte
 *        prim table immediately after.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x7];
    /* 0x07 */ u8 b7;
    /* 0x08 */ u8 pad08[0x4];
    /* 0x0C */ u8 bC;
    /* 0x0D */ u8 bD;
    /* 0x0E */ u8 bE;
    /* 0x0F */ u8 pad0F[0x5];
    /* 0x14 */ u8 b14;
    /* 0x15 */ u8 b15;
    /* 0x16 */ u8 b16;
    /* 0x17 */ u8 pad17[0x5];
} func_800A2128_item1;  /* 0x1C = 28 bytes */

typedef struct {
    /* 0x00 */ u8  pad03[0x3];
    /* 0x03 */ u8  tag;     /**< Always written as @c 1. */
    /* 0x04 */ s32 cmd;     /**< @c 0xE1000200 | (color & 0x9FF). */
} func_800A2128_item2;  /* 8 bytes */

typedef struct {
    /* 0x0000 */ u8 pad0000[0x4000];
    /* 0x4000 */ func_800A2128_item1 items1[128];
    /* 0x4E00 */ func_800A2128_item2 items2[16];
} func_800A2128_arg0;

/**
 * @brief Reset 128 entries in @c items1[] and emit 16 GPU draw-mode
 *        prims into @c items2[].
 *
 * Loop 1: for each of 128 28-byte items, call @c func_8004D684(item)
 *         (which clears/inits it), then clear @c bC/bD/bE and
 *         @c b14/b15/b16 and set bit 1 of @c b7.
 *
 * Loop 2: for each of 16 8-byte items, write @c tag = 1 and
 *         @c cmd = @c 0xE1000200 | (color & 0x9FF), where @c color
 *         comes from @c GetTPage(0, 2, 0, 0).
 *
 * @note The two @c i[t->items1] / @c i[t->items2] uses (instead of
 *       @c t->items1[i] / @c t->items2[i]) are the trick that swaps
 *       the @c addu operand order to match the target, see
 *       @c pattern_i_arr_to_swap_addu in project memory.
 */
void func_800A2128(func_800A2128_arg0 *t) {
    s16 i;
    for (i = 0; i < 128; i++) {
        func_8004D684(&i[t->items1]);
        i[t->items1].bC = 0;
        i[t->items1].bD = 0;
        i[t->items1].bE = 0;
        i[t->items1].b14 = 0;
        i[t->items1].b15 = 0;
        i[t->items1].b16 = 0;
        i[t->items1].b7 |= 2;
    }
    for (i = 0; i < 16; i++) {
        s32 color;
        i[t->items2].tag = 1;
        color = GetTPage(0, 2, 0, 0);
        t->items2[i].cmd = (color & 0x9FF) | 0xE1000200;
    }
}

/**
 * @brief Draws every field entity's blob shadow as a flat-shaded 8-triangle fan.
 *
 * Builds a unit octagon once per call, @c func_8009D234 / @c func_8009D254 sampled
 * at the eight 32-step headings give the cos/sin pair for each ring point, then
 * walks the @ref Actor pool. An entity casts a shadow only when it is not flagged
 * out (@c flags bit 3), is active (@c unk218 @c != @c -1) and is in the state
 * @c unk258 @c == @c 1.
 *
 * The fan centre is the entity's position dropped to integer world units, and ring
 * point @c k sits at that centre offset along octagon direction @c k, scaled by the
 * entity's own @c shadowRadius[k]. Because the eight radii are independent the
 * shadow need not be circular, @c SHADEFORM sets them individually, @c SHADESET
 * makes them uniform. The nine points are projected with one @c RotTransPers (whose
 * return gives the OTZ) plus three @c RTPT batches, and when the centre is in depth
 * range the fan is emitted as eight @ref POLY_G3 triangles, every one flat-shaded in
 * @c shadowLevel, followed by the slot's tpage command.
 *
 * @param ot   Ordering table to link the shadows into.
 * @param m    Camera matrix loaded into the GTE before projecting.
 * @param prim Triangle arena; advanced eight prims per shadow drawn.
 * @param tp   Tpage commands; advanced one per shadow drawn.
 * @param ents The @ref Actor pool (@c D_80085388 entries).
 *
 * @note The scratchpad holds the octagon tables and the nine working points:
 *       cos at @c getScratchAddr(2), sin at @c getScratchAddr(6), points at
 *       @c getScratchAddr(10).
 * @note @c sxy is two words wide because that is the slot the original reserves for
 *       @c RotTransPers's screen-XY output; only the first word is meaningful.
 * @note @c rad is advanced at the end of the ring loop rather than indexed: that is
 *       what makes gcc give it an induction variable of its own alongside the two
 *       octagon tables, which is how the original walks all three.
 */
void func_800A222C(u32 *ot, MATRIX *m, POLY_G3 *prim, DR_TPAGE *tp, Actor *ents) {
    s16 *cosTbl = (s16 *)getScratchAddr(2);
    s16 *sinTbl = (s16 *)getScratchAddr(6);
    SVECTOR *pt = (SVECTOR *)getScratchAddr(10);
    s32 sxy[2];
    s32 p;
    s32 flag;
    s32 otz;
    s32 i;
    s32 k;
    u8 *rad;
    u8 c;

    for (i = 0; i < 8; i++) {
        cosTbl[i] = func_8009D234(i * 32);
        sinTbl[i] = func_8009D254(i * 32);
    }

    PushMatrix();
    SetRotMatrix(m);
    SetTransMatrix(m);

    for (i = 0; i < D_80085388; ents++, i++) {
        if (ents->context.flags & 8) {
            continue;
        }
        if (ents->unk218 == -1) {
            continue;
        }
        if (ents->unk258 != 1) {
            continue;
        }

        pt[0].vx = ents->posX / 4096;
        pt[0].vy = ents->posY / 4096;
        pt[0].vz = ents->posZ / 4096;

        rad = &ents->shadowRadius[0];
        for (k = 0; k < 8; k++) {
            pt[k + 1].vx = pt[0].vx + cosTbl[k] * rad[0] / 512;
            pt[k + 1].vy = pt[0].vy + sinTbl[k] * rad[0] / 512;
            pt[k + 1].vz = pt[0].vz;
            rad++;
        }

        otz = RotTransPers(&pt[0], sxy, &p, &flag);
        gte_ldv3(&pt[0], &pt[1], &pt[2]);
        gte_rtpt();
        gte_stsxy3(&pt[0], &pt[1], &pt[2]);
        gte_ldv3(&pt[3], &pt[4], &pt[5]);
        gte_rtpt();
        gte_stsxy3(&pt[3], &pt[4], &pt[5]);
        gte_ldv3(&pt[6], &pt[7], &pt[8]);
        gte_rtpt();
        gte_stsxy3(&pt[6], &pt[7], &pt[8]);

        if (otz < 0xFFF) {
            c = ents->shadowLevel;

            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[1].vx, pt[1].vy, pt[2].vx, pt[2].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[2].vx, pt[2].vy, pt[3].vx, pt[3].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[3].vx, pt[3].vy, pt[4].vx, pt[4].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[4].vx, pt[4].vy, pt[5].vx, pt[5].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[5].vx, pt[5].vy, pt[6].vx, pt[6].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[6].vx, pt[6].vy, pt[7].vx, pt[7].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[7].vx, pt[7].vy, pt[8].vx, pt[8].vy);
            addPrim(&ot[otz], prim);
            prim++;
            setRGB0(prim, c, c, c);
            setXY3(prim, pt[0].vx, pt[0].vy, pt[8].vx, pt[8].vy, pt[1].vx, pt[1].vy);
            addPrim(&ot[otz], prim);
            prim++;

            addPrim(&ot[otz], tp);
            tp++;
        }
    }

    PopMatrix();
}

/**
 * @brief Initialize a run of items at @p p; return the pointer past the
 *        last item.
 *
 * Iterates @c **D_800D5E9C items, each iteration writing the fixed
 * trio (@c b3 = 4, @c b7 = 0x22, @c b4/b5/b6 = 0). The buffer count
 * @c **D_800D5E9C is reloaded each iteration (gcc can't prove the
 * stores don't alias the indirect chain). Returns the input pointer
 * advanced past the items written, used by @c func_800983F0 to chain
 * multiple init regions into one growing buffer.
 */
func_800A29C0_arg0 *func_800A29C0(func_800A29C0_arg0 *p) {
    s32 i;
    for (i = 0; i < **D_800D5E9C; i++) {
        p->b3 = 4;
        p->b7 = 0x22;
        p->b4 = 0;
        p->b5 = 0;
        p->b6 = 0;
        p++;
    }
    return p;
}

/**
 * @brief Append one GPU draw-mode prim per entry in the @c D_800D5E9C
 *        list; return the advanced output pointer.
 *
 * For each non-sentinel entry (count from @c **D_800D5E9C), calls
 * @c GetTPage(0, 1, 0, 0) to get a color value, masks to 9 bits,
 * ORs with the GPU draw-mode command base @c 0xE1000200, and writes
 * one 8-byte prim with @c tag=1 + @c cmd=combined.
 *
 * Used by @c func_800983F0 's draw-prim chain layout.
 */
func_800A2A30_item *func_800A2A30(func_800A2A30_item *p) {
    s32 i;
    for (i = 0; i < **D_800D5E9C; i++) {
        s32 color;
        p->tag = 1;
        color = GetTPage(0, 1, 0, 0);
        p->cmd = (color & 0x9FF) | 0xE1000200;
        p++;
    }
    return p;
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object1_2", func_800A2AF8);

/** @brief Signed/unsigned halfword view of the split-image header word. */
typedef union {
    s16 s;
    u16 u;
} func_800A2D2C_half;

/**
 * @brief Upload a split field-graphic to VRAM in three blits.
 *
 * @p buf begins with a halfword header: the split row (0x2020, ASCII
 * spaces, means "empty buffer, skip"). The upload sequence, each part
 * preceded by a busy-wait on @ref DrawSync and a @ref VSync
 * reset:
 *  1. a 256x1 strip at (0, 0xE8) from @c buf+2 (the palette row),
 *  2. a 64-wide column at (slot*64, header+0x100) of height
 *     (0x100-header)/2 from @c buf+0x102,
 *  3. the remaining half directly below it, from the matching offset
 *     within @p buf.
 *
 * @param buf  Halfword staging buffer (header, palette row, then pixel rows).
 * @param slot VRAM column slot; x = @p slot * 64.
 *
 * @note The volatile-cast re-read of the header in part 3 keeps gcc from
 *       folding the unsigned reload into the earlier signed load, the
 *       original emits both.
 */
void func_800A2D2C(s16 *buf, s32 slot) {
    RECT rect;
    func_800A2D2C_half *hp;
    s32 y0;
    s32 h2;

    hp = (func_800A2D2C_half *)buf;
    if (buf[0] != 0x2020) {
        while (DrawSync(1) != 0) {}
        VSync(0);
        rect.x = 0;
        rect.y = 0xE8;
        rect.w = 0x100;
        rect.h = 1;
        LoadImage(&rect, (u32 *)(buf + 2));
        while (DrawSync(1) != 0) {}
        VSync(0);
        rect.x = slot << 6;
        rect.y = hp->u + 0x100;
        rect.w = 0x40;
        rect.h = (0x100 - hp->s) / 2;
        LoadImage(&rect, (u32 *)(buf + 0x102));
        while (DrawSync(1) != 0) {}
        VSync(0);
        rect.x = slot << 6;
        y0 = ((volatile func_800A2D2C_half *)hp)->u;
        h2 = (0x100 - hp->s) / 2;
        y0 += 0x100;
        rect.y = h2 + y0;
        rect.w = 0x40;
        rect.h = (0x100 - hp->s) / 2;
        LoadImage(&rect, (u32 *)(buf + ((((0x100 - hp->s) / 2) << 6) + 0x102)));
    }
}

/**
 * @brief Cheap pseudo-random helper: returns @c (table[counter] * range) >> 8.
 *
 * Advances a one-byte counter @c D_800C6D90 by 13 (a prime stride that
 * walks all 256 positions of the lookup table before repeating), reads
 * a byte from the @c D_800C3520 lookup table, multiplies by @p range,
 * and returns the high 24 bits of the product as a signed s16.
 *
 * Used by @c func_800A303C to seed each new particle's position,
 * velocity, and unk fields with a value in roughly @c [-range/2, range/2].
 *
 * @param range Half-range scaler, the table entry (0..255) is multiplied
 *              by @p range and shifted right 8.
 * @return A signed pseudo-random value derived from the lookup table.
 */
s16 func_800A2EA4(s16 range) {
    D_800C6D90 += 13;
    return ((s32)D_800C3520[D_800C6D90] * range) >> 8;
}

/**
 * Initializes an object by calling a sequence of setup functions.
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800A2EE0(u8 *a0) {
    func_800A3534(a0);
    func_800A3018(a0);
    func_800A2F48(a0);
    func_800A2F70(a0 + 0x3720);
    func_800A2F70(a0 + 0x4B20);
}

/**
 * Clears 16 bytes at offset 0x190 (backwards loop).
 *
 * @param a0 Unused parameter.
 * @param a1 Pointer to the object structure base.
 */
void func_800A2F28(s32 a0, u8 *a1) {
    s32 i = 0xF;
    a1 += 0xF;
    do {
        *(u8 *)(a1 + 0x190) = 0;
        i--;
        a1--;
    } while (i >= 0);
}

/**
 * @brief Shape of the buffer @c func_800A2F48 sees: an array of 128
 * 32-byte items beginning at offset 0x2739, each item's first 3 bytes
 * being what gets zeroed.
 *
 * @note Named after the function/arg it describes rather than after a
 *       semantic role, we don't know what the original developer
 *       called this view. The same memory is also reached by
 *       @c func_800A303C through the @c Particle overlay (where these
 *       3 bytes are the per-particle @c unk19, @c unk1A, and @c active
 *       fields), but it's not certain that the original C used the same
 *       struct in both functions.
 */
typedef struct {
    /* 0x0000 */ u8 pad0000[0x2739];
    /* 0x2739 */ struct {
        u8 b0;
        u8 b1;
        u8 b2;
        u8 pad[0x1D];
    } items[128];
} func_800A2F48_arg0;

/**
 * @brief Zero the 3 leading bytes of each of the 128 items in the table.
 *
 * Called from @c func_800A2EE0 during the particle-system init chain on
 * the disc-loaded field-map buffer.
 */
void func_800A2F48(func_800A2F48_arg0 *t) {
    s32 i;
    for (i = 0; i < 128; i++) {
        t->items[i].b0 = 0;
        t->items[i].b1 = 0;
        t->items[i].b2 = 0;
    }
}

/**
 * @brief Shape @c func_800A2F70 sees: array of 128 40-byte items, three
 *        fields per item, @c b3 / @c b7 (constant tags) and @c hE (a
 *        @c GetClut -seeded halfword).
 *
 * @note Named after the function/arg. Called from @c func_800A2EE0 twice
 *       (at base + 0x3720 and base + 0x4B20) on two different sub-regions
 *       of the disc-loaded field-map buffer, so the shape describes
 *       "whichever sub-region was passed" rather than any single
 *       canonical struct.
 */
typedef struct {
    /* 0x00 */ u8  pad00[0x3];
    /* 0x03 */ u8  b3;
    /* 0x04 */ u8  pad04[0x3];
    /* 0x07 */ u8  b7;
    /* 0x08 */ u8  pad08[0x6];
    /* 0x0E */ s16 hE;
    /* 0x10 */ u8  pad10[0x18];
} func_800A2F70_arg0;  /* 0x28 = 40 bytes */

/**
 * @brief Seed 128 items with the constant tags 9 / 0x2C and a
 *        per-item @c GetClut(0, 0xE8) sample at the @c hE field.
 *
 * Called twice from @c func_800A2EE0 on two distinct sub-regions of the
 * disc-loaded field-map buffer; both regions are arrays of 40-byte
 * items, 128 entries each.
 */
void func_800A2F70(func_800A2F70_arg0 *e) {
    s32 i;
    for (i = 0; i < 128; i++) {
        e->b3 = 9;
        e->b7 = 0x2C;
        e->hE = GetClut(0, 0xE8);
        e++;
    }
}

/**
 * @brief Shape of the buffer @c func_800A2FE0 sees: an array of 128
 * 32-byte items beginning at offset 0x2739, each item's third byte
 * being the @c active flag scanned for free slots.
 *
 * @note Named after the function/arg. Same memory layout as
 *       @c func_800A2F48_arg0 (which clears all three leading bytes of
 *       each item); the views weren't unified because we don't know
 *       whether the original C source shared a single typedef.
 */
typedef struct {
    /* 0x0000 */ u8 pad0000[0x2739];
    /* 0x2739 */ struct {
        u8 b0;       /* unk19 in particle terms */
        u8 b1;       /* unk1A */
        u8 active;   /* the alive flag */
        u8 pad[0x1D];
    } items[128];
} func_800A2FE0_arg0;

/**
 * @brief Find the first inactive particle slot (0..127), or @c -1 if none.
 *
 * Scans up to 128 items in @p t for the first one whose @c active flag
 * is zero, returning its index. Called by @c func_800A303C to pick a
 * free slot for a new particle.
 */
s16 func_800A2FE0(func_800A2FE0_arg0 *t) {
    s32 i;
    for (i = 0; i < 128; i++) {
        if (t->items[i].active == 0) return (s16)i;
    }
    return -1;
}

/**
 * @brief Zero @c curCount and @c unk15E across the first 16 emitter slots.
 *
 * Called from @c func_800A2EE0 during particle-system init to reset the
 * active-particle counts (and the small @c unk15E word that travels with
 * them) without touching the static jitter ranges.
 */
void func_800A3018(Emitter *em) {
    s32 i;
    for (i = 0; i < 16; i++) {
        em[i].unk15E = 0;
        em[i].curCount = 0;
    }
}

/**
 * @brief Spawn up to @p count particles for emitter @p emIdx around @p pos.
 *
 * Loops while @p count is positive and the emitter has free slots, calling
 * @c func_800A2FE0 to allocate a particle slot, then seeding its position,
 * velocity, and per-particle counters from the emitter's ranges plus the
 * spawn position. Each lookup uses @c func_800A2EA4 for a signed random
 * value in a half-range about zero.
 *
 * @param emIdx  Emitter index (stride 0x174 within @p sys).
 * @param sys    Particle system buffer.
 * @param pos    Spawn anchor position (3 s16 components: x, y, z).
 * @param count  Maximum particles to spawn this call.
 */
void func_800A303C(s16 emIdx, ParticleSystem *sys, s16 *pos, s16 count) {
    Emitter *em = (Emitter *)sys + emIdx;
    Particle *p;
    s16 slot;

    em->unk14E = 0;

    while (1) {
        if (count <= 0) return;
        if (em->curCount >= em->maxCount) return;

        slot = func_800A2FE0(sys);
        if (slot == -1) return;

        em->curCount++;
        p = (Particle *)&sys->slots[slot];

        p->emitterIdx = emIdx;
        p->active = 1;

        p->unk12 = func_800A2EA4(em->unk162 * 32) + em->unk160 * 32 - em->unk162 * 16;
        p->unk16 = func_800A2EA4(em->unk164 * 2) - em->unk164;
        count--;
        p->posX = pos[0] * 16 + (func_800A2EA4(em->unk166) << 8) - em->unk166 * 128;
        p->posY = pos[1] * 16 + (func_800A2EA4(em->unk168) << 8) - em->unk168 * 128;
        p->posZ = pos[2] * 16 + (func_800A2EA4(em->unk16A & 0x7F) << 8) - (em->unk16A & 0x7F) * 128;
        p->velX = func_800A2EA4(em->unk16C * 32) - em->unk16C * 16;
        p->velY = func_800A2EA4(em->unk16E * 32) - em->unk16E * 16;
        p->velZ = func_800A2EA4(em->unk170 * 32) - em->unk170 * 16;
        p->unk19 = 0;
        p->unk1A = 0;
    }
}

/**
 * @brief View of the Actor stack region used by @c func_800A327C, three
 * @c s16 control points (@c a, @c b, @c c) and a @c num/denom progress ratio.
 *
 * @note Named after the function/arg, mirroring @ref func_800A3488_arg0
 *       (the two-point linear variant): same memory, one more control point.
 */
typedef struct {
    /* 0x00 */ s16 ax, ay, az;          /**< Control point A (at progress @c 0). */
    /* 0x06 */ u8  pad06[0x02];
    /* 0x08 */ s16 bx, by, bz;          /**< Control point B (curve pull). */
    /* 0x0E */ u8  pad0E[0x02];
    /* 0x10 */ s16 cx, cy, cz;          /**< Control point C (at progress @c denom). */
    /* 0x16 */ u8  pad16[0xE0 - 0x16];
    /* 0xE0 */ u8  denom;               /**< Step count denominator. */
    /* 0xE1 */ u8  padE1[0x0F];
    /* 0xF0 */ s16 num;                 /**< Current step. */
} func_800A327C_arg0;

/** @brief Intermediate lerp point of @c func_800A327C (u16 components). */
typedef struct {
    u16 x, y, z, pad;
} func_800A327C_mid;

/**
 * @brief Quadratic-Bezier interpolate a 3D position across three @c s16
 *        control points (de Casteljau evaluation).
 *
 * Computes the two edge midpoints @c m1 = A + (B-A)*num/denom and
 * @c m2 = B + (C-B)*num/denom, then writes @c out->vx/vy/vz with the
 * second-stage lerp @c m1 + (m2-m1)*num/denom.
 *
 * Used by @c func_800A355C dispatch (mode 3) to compute the per-step spawn
 * position for particle bursts along a curved path.
 *
 * @note The midpoint components are @c u16 with @c (s16) casts at the
 *       second stage, this mixed-width view reproduces the original's
 *       lhu/lh load pairing and shift re-extensions.
 */
void func_800A327C(func_800A327C_arg0 *a, SVECTOR *out) {
    func_800A327C_mid m1;
    func_800A327C_mid m2;

    m1.x = ((a->bx - a->ax) * a->num) / a->denom + (u16)a->ax;
    m1.y = ((a->by - a->ay) * a->num) / a->denom + (u16)a->ay;
    m1.z = ((a->bz - a->az) * a->num) / a->denom + (u16)a->az;
    m2.x = ((a->cx - a->bx) * a->num) / a->denom + (u16)a->bx;
    m2.y = ((a->cy - a->by) * a->num) / a->denom + (u16)a->by;
    m2.z = ((a->cz - a->bz) * a->num) / a->denom + (u16)a->bz;

    out->vx = m1.x + ((s16)m2.x - (s16)m1.x) * a->num / a->denom;
    out->vy = m1.y + ((s16)m2.y - (s16)m1.y) * a->num / a->denom;
    out->vz = m1.z + ((s16)m2.z - (s16)m1.z) * a->num / a->denom;
}

/**
 * @brief View of the Actor stack region used by @c func_800A3488, two
 * @c s16 endpoints (@c a, @c b) and a @c num/denom progress ratio.
 *
 * @note Named after the function/arg. The same memory is normally the
 *       Actor bytecode @c stack[]; @c func_800A3488 's caller has
 *       already stashed animation state into specific stack slots
 *       before invoking the interpolation.
 */
typedef struct {
    /* 0x00 */ s16 ax, ay, az;          /**< Endpoint A (at progress @c 0). */
    /* 0x06 */ u8  pad06[0x02];
    /* 0x08 */ s16 bx, by, bz;          /**< Endpoint B (at progress @c denom). */
    /* 0x0E */ u8  pad0E[0xD2];
    /* 0xE0 */ u8  denom;               /**< Step count denominator. */
    /* 0xE1 */ u8  padE1[0x0F];
    /* 0xF0 */ s16 num;                 /**< Current step. */
} func_800A3488_arg0;

/**
 * @brief Linearly interpolate a 3D position between two @c s16 endpoints.
 *
 * Writes @c out->vx/vy/vz with @c a + ((b - a) * num) / denom for each
 * axis, where @c a, @c b, @c num, and @c denom come from specific slots
 * of the actor's bytecode stack region.
 *
 * Used by @c func_800A355C dispatch (mode 2) to compute the per-step
 * spawn position for particle bursts.
 */
void func_800A3488(func_800A3488_arg0 *a, SVECTOR *out) {
    out->vx = a->ax + ((a->bx - a->ax) * a->num) / a->denom;
    out->vy = a->ay + ((a->by - a->ay) * a->num) / a->denom;
    out->vz = a->az + ((a->bz - a->az) * a->num) / a->denom;
}

/**
 * @brief Shape of the buffer @c func_800A3534 sees: an array of 16
 * items beginning at offset 0x1830, each item @c 0xFE bytes with three
 * leading s16 fields that get zeroed.
 *
 * @note Named after the function/arg; we don't know the original name.
 *       Same disc-loaded field-map buffer that the rest of the
 *       @c func_800A2EE0 init chain operates on, but the per-item shape
 *       (stride 0xFE, three s16 fields) doesn't align with any other
 *       struct in this file.
 */
typedef struct {
    /* 0x0000 */ u8 pad0000[0x1830];
    /* 0x1830 */ struct {
        s16 h0;
        s16 h1;
        s16 h2;
        u8  pad[0xF8];   /* pad to 0xFE stride */
    } items[16];
} func_800A3534_arg0;

/**
 * @brief Zero the three leading s16 fields of each of 16 items in the table.
 *
 * Called from @c func_800A2EE0 as the first step of the particle-system
 * init chain on the disc-loaded field-map buffer.
 */
void func_800A3534(func_800A3534_arg0 *t) {
    s32 i;
    for (i = 0; i < 16; i++) {
        t->items[i].h0 = 0;
        t->items[i].h1 = 0;
        t->items[i].h2 = 0;
    }
}


/**
 * @brief Animation slot tick & dispatch, runs the per-frame update for the
 * actor's four animation slots.
 *
 * For each of the 4 slots:
 *   - Skip if slot id is -1 (empty).
 *   - Read a "rate" byte from the source-row table (located at
 *     `&actor->rows[i] + actor->animOffset`); when the slot's tick counter
 *     reaches `rate / 8`, reset the counter and pick a `ratio` value
 *     (`rate` itself if rate < 8, else 1).
 *   - Increment the tick counter.
 *   - Dispatch to func_800A303C with one of three position sources, chosen
 *     by `g_fieldEntity.slotActive[slot]`:
 *       - kind == 1: select by actor->mode, pass the actor itself
 *         (mode 1), or fill `pos` via func_800A3488 (mode 2) or
 *         func_800A327C (mode 3).
 *       - kind != 1: read entity (kind & 0x7F) from D_80085224, divide
 *         posX/Y/Z by 4096, pass as `pos`.
 *
 * @param actor Field entity (with rows[4]/timers[4]/animOffset/mode).
 * @param slot  Index into g_fieldEntity.slotActive (0..15).
 * @param a2    Second arg passed through to func_800A303C.
 */
void func_800A355C(ActorAnim *actor, s32 slot, s32 a2) {
    SVECTOR pos;
    s32 i;

    for (i = 0; i < 4; i++) {
        u8 srcByte;
        s32 ratio;

        if (actor->rows[i].id == -1) {
            continue;
        }

        srcByte = *((u8 *)&actor->rows[i] + actor->animOffset);
        ratio = 0;
        if (actor->timers[i] >= (s32)((u32)srcByte >> 3)) {
            actor->timers[i] = 0;
            if (*((u8 *)&actor->rows[i] + actor->animOffset) < 8) {
                ratio = *((u8 *)&actor->rows[i] + actor->animOffset);
            } else {
                ratio = 1;
            }
        }
        actor->timers[i] = (u16)actor->timers[i] + 1;

        if (g_fieldEntity.slotActive[slot] == 1) {
            switch (actor->mode) {
            case 1:
                func_800A303C(actor->rows[i].id, a2, (SVECTOR *)actor, ratio);
                break;
            case 2:
                func_800A3488((Actor *)actor, &pos);
                func_800A303C(actor->rows[i].id, a2, &pos, ratio);
                break;
            case 3:
                func_800A327C((Actor *)actor, &pos);
                func_800A303C(actor->rows[i].id, a2, &pos, ratio);
                break;
            }
        } else {
            pos.vx = (s16)(D_80085224[g_fieldEntity.slotActive[slot] & 0x7F].posX / 4096);
            pos.vy = (s16)(D_80085224[g_fieldEntity.slotActive[slot] & 0x7F].posY / 4096);
            pos.vz = (s16)(D_80085224[g_fieldEntity.slotActive[slot] & 0x7F].posZ / 4096);
            func_800A303C(actor->rows[i].id, a2, &pos, ratio);
        }
    }
}

/**
 * @brief Per-frame animation tick for all 16 slots of a field subscene buffer.
 *
 * Walks the 16 @ref FieldSubsceneSlot entries of @p buf (stride @c 0xFE). For
 * each slot marked active in @c g_fieldEntity.slotActive[i]:
 *  - if the per-frame counter @c h1 has reached @c table[h2], reset @c h1,
 *    advance the table cursor @c h2, and if the next table entry is @c 0 wrap
 *    @c h2 and @c h0 back to 0;
 *  - dispatch the visual update via @c func_800A355C (the slot's @c subscene is
 *    the @ref ActorAnim argument), then advance @c h0 and @c h1.
 * Inactive slots have all three state halfwords (@c h0 / @c h1 / @c h2) cleared.
 *
 * @param arg0 Unused.
 * @param frame Unused.
 * @param buf  Subscene buffer (from the @c D_800C7200 table).
 *
 * @note @c pos is declared but unused: the original reserves an 8-byte stack
 *       slot here (gcc 2.7.2 keeps an unused struct local), matching the frame.
 */
void func_800A37A8(MATRIX *m, FieldFrameBuf *frame, FieldSubsceneBuffer *buf) {
    s32 i;
    SVECTOR pos;

    for (i = 0; i < 16; i++) {
        if (g_fieldEntity.slotActive[i] != 0) {
            if (buf->slots[i].h1 >= buf->slots[i].table[buf->slots[i].h2]) {
                buf->slots[i].h1 = 0;
                buf->slots[i].h2++;
                if (buf->slots[i].table[buf->slots[i].h2] == 0) {
                    buf->slots[i].h2 = 0;
                    buf->slots[i].h0 = 0;
                }
            }
            func_800A355C((ActorAnim *)&buf->slots[i], i, (s32)buf);
            buf->slots[i].h0++;
            buf->slots[i].h1++;
        } else {
            buf->slots[i].h0 = 0;
            buf->slots[i].h1 = 0;
            buf->slots[i].h2 = 0;
        }
    }
}

/**
 * @brief Advance a 3-axis position+angle lerp accumulator by one tick.
 *
 * Updates @p out 's @c posX/posY/posZ (@c s32) and @c angle (@c u16)
 * accumulators by adding (per-axis) @c (startSnapshot @c + @c origin @c +
 * @c (target @c - @c origin) @c * @c stepProgress @c / @c stepTotal),
 * where the angle term is then @c <<4. Bails early when
 * @p in->stepTotal @c == @c 0 to avoid a divide-by-zero.
 *
 * Caching @c in->angle into an @c s32 local prevents gcc from doing
 * lhu+sll/sra for that field; caching @c out->angleStart into an
 * @c s32 local makes gcc pick @c lh over @c lhu for it. Together
 * these match the target's exact instruction selection.
 */
void func_800A38B4(MoveAccum *out, MoveStep *in, MoveStep *target) {
    s32 a;
    s32 s;
    if (in->stepTotal != 0) {
        a = in->angle;
        s = out->angleStart;
        out->angle += (s + a + (target->angle - a) * out->stepProgress / in->stepTotal) << 4;
        out->posX  += out->xStart + in->x + (target->x - in->x) * out->stepProgress / in->stepTotal;
        out->posY  += out->yStart + in->y + (target->y - in->y) * out->stepProgress / in->stepTotal;
        out->posZ  += out->zStart + in->z + (target->z - in->z) * out->stepProgress / in->stepTotal;
    }
}

/**
 * @brief Emit one field sprite for a movement accumulator and link it into the OT.
 *
 * Advances @p acc one tick along its current waypoint pair (@c func_800A38B4),
 * projects the accumulated position through the GTE, and, when the projected
 * depth lands inside the ordering table, builds a @ref POLY_FT4 for the sprite
 * and chains it in.
 *
 * The quad is a screen-space billboard: RTPS gives the centre, the sprite's
 * half-extents are lerped between the current and next waypoint, scaled by
 * @c FieldView::spriteScale / OTZ so it shrinks with distance, then rotated by
 * @c acc->angle through two @c mvmva passes to give the two corner offsets.
 * Texture coordinates, colour and the semi-transparency bits of the tpage word
 * all come from the current waypoint; colour is lerped like the extents.
 *
 * Finally the tick counter advances, rolling over to the next waypoint once it
 * reaches @c MoveStep::stepTotal.
 *
 * @param acc Accumulator to advance and draw.
 * @param rec Movement command @p acc is playing.
 * @param buf Field bundle header; its @c primCursor is the prim arena cursor.
 * @param ot  Ordering table to link the quad into.
 *
 * @note The scratchpad slots are separate @c getScratchAddr locals rather than
 *       one struct: the original materialises each address independently and
 *       spills two of them, which a single base pointer does not reproduce.
 * @note @c RotMatrix_gte is the main binary's @c RotMatrix_gte and
 *       @c func_80040534 its @c TransMatrix; the field overlay links both by
 *       address, so they keep their @c func_ names here.
 */
void func_800A39D8(MoveAccum *acc, MoveRecord *rec, FieldSubsceneBuffer *buf, u32 *ot) {
    SVECTOR *pos = (SVECTOR *)getScratchAddr(5);
    SVECTOR *rot = (SVECTOR *)getScratchAddr(7);
    SVECTOR *corner = (SVECTOR *)getScratchAddr(9);
    MacVec *mac = (MacVec *)getScratchAddr(11);
    VECTOR *trans = (VECTOR *)getScratchAddr(15);
    MATRIX *m = (MATRIX *)getScratchAddr(19);
    MoveStep *step;
    MoveStep *next;
    s32 otz;
    s16 spriteX, spriteY;
    s32 dx, dy;
    s32 scale;
    u32 tp;
    u32 halfW, halfH;

    step = &rec->steps[acc->stepIndex];
    next = &rec->steps[acc->stepIndex + 1];
    func_800A38B4(acc, step, next);

    pos->vx = acc->posX / 16;
    pos->vy = acc->posY / 16;
    pos->vz = acc->posZ / 16;

    if (step->mode == 4) {
        buf->primCursor->code &= ~2;
        buf->primCursor->tpage = (D_8005F0F8->tpageX & 0xF) | 0x10;
    } else {
        buf->primCursor->code |= 2;
        tp = (((step->mode & 3) << 5) | 0x10);
        tp |= D_8005F0F8->tpageX & 0xF;
        buf->primCursor->tpage = tp;
    }

    gte_ldv0(pos);
    gte_rtps();
    gte_stsxy(&buf->primCursor->x0);
    gte_stszotz(&otz);

    otz += rec->zBias;
    if (otz > 0 && otz < 0x1000) {
        dx = (next->spriteX - step->spriteX) * acc->stepProgress / step->stepTotal;
        dy = (next->spriteY - step->spriteY) * acc->stepProgress / step->stepTotal;
        scale = (D_800C71F8->spriteScale << 14) / otz;
        spriteX = step->spriteX + dx;
        spriteY = step->spriteY + dy;

        if ((rec->flags & 0xFF80) == 0) {
            halfW = (u32)(spriteX * scale) >> 14;
            halfH = (u32)(spriteY * scale) >> 14;
        } else {
            halfW = (u32)(spriteX * scale) >> 13;
            halfH = (u32)(spriteY * scale) >> 13;
        }

        rot->vy = 0;
        rot->vx = 0;
        rot->vz = acc->angle;
        RotMatrix_gte(rot, m);

        trans->vz = 0;
        trans->vy = 0;
        trans->vx = 0;
        func_80040534(m, trans);

        gte_SetRotMatrix(m);
        gte_SetTransMatrix(m);

        corner->vx = halfW;
        corner->vy = -halfH;
        corner->vz = 0;
        gte_ldv0(corner);
        gte_mvmva(1, 0, 0, 0, 0);

        buf->primCursor->u0 = buf->primCursor->u2 = step->u;
        buf->primCursor->v0 = buf->primCursor->v1 = step->v;
        gte_stlvnl(mac);

        buf->primCursor->x1 = buf->primCursor->x0 + mac->x;
        buf->primCursor->y1 = buf->primCursor->y0 + mac->y;
        buf->primCursor->x2 = buf->primCursor->x0 - mac->x;
        buf->primCursor->y2 = buf->primCursor->y0 - mac->y;

        corner->vy = halfH;
        gte_ldv0(corner);
        gte_mvmva(1, 0, 0, 0, 0);

        buf->primCursor->u1 = buf->primCursor->u3 = step->u + step->w - 1;
        buf->primCursor->v2 = buf->primCursor->v3 = step->v + step->h - 1;
        gte_stlvnl(mac);

        buf->primCursor->x3 = buf->primCursor->x0 + mac->x;
        buf->primCursor->y3 = buf->primCursor->y0 + mac->y;
        buf->primCursor->x0 = buf->primCursor->x0 - mac->x;
        buf->primCursor->y0 = buf->primCursor->y0 - mac->y;

        buf->primCursor->r0 = step->r + (next->r - step->r) * acc->stepProgress / step->stepTotal;
        buf->primCursor->g0 = step->g + (next->g - step->g) * acc->stepProgress / step->stepTotal;
        buf->primCursor->b0 = step->b + (next->b - step->b) * acc->stepProgress / step->stepTotal;

        addPrim(&ot[otz], buf->primCursor);
        buf->primCursor = (POLY_FT4 *)((u8 *)buf->primCursor + 0x28);
    }

    acc->stepProgress++;
    if (acc->stepProgress >= step->stepTotal) {
        acc->stepProgress = 0;
        acc->stepIndex++;
    }
}

/**
 * @brief Fast-forward an entire field animation buffer to completion.
 *
 * Runs the per-tick work of @c func_800A37A8 and @c func_800A38B4 in a loop
 * until every subscene slot has played out: the tick count is the largest
 * @c frameCount across the 16 slots, and a slot is dropped (its
 * @c g_fieldEntity.slotActive entry cleared) once the tick passes its own count.
 *
 * Each tick advances two things. Every still-active slot steps its animation
 * table, resetting the cursor to the start when the table byte runs out —
 * and is dispatched through @c func_800A355C. Every running accumulator is
 * lerped one step by @c func_800A38B4 toward the next waypoint of its command;
 * a waypoint with a zero tick count ends the command (clearing @c active and
 * releasing one user of the record), and reaching a waypoint's tick count
 * advances to the next one.
 *
 * The @c slotActive flags are saved on entry and restored on exit, so the
 * caller's live slot set survives the fast-forward.
 *
 * @param buf Animation buffer to run to completion.
 */
void func_800A3FE0(FieldSubsceneBuffer *buf) {
    u8 saved[16];
    s32 i;
    s32 j;
    s32 maxFrames;

    for (i = 0; i < 16; i++) {
        saved[i] = g_fieldEntity.slotActive[i];
    }

    maxFrames = 0;
    for (i = 0; i < 16; i++) {
        if (maxFrames < buf->slots[i].frameCount) {
            maxFrames = buf->slots[i].frameCount;
        }
    }

    for (i = 0; i < maxFrames; i++) {
        for (j = 0; j < 16; j++) {
            if (buf->slots[j].frameCount < i) {
                g_fieldEntity.slotActive[j] = 0;
            }
            if (g_fieldEntity.slotActive[j] != 0) {
                if (buf->slots[j].h1 >= buf->slots[j].table[buf->slots[j].h2]) {
                    buf->slots[j].h1 = 0;
                    buf->slots[j].h2++;
                    if (buf->slots[j].table[buf->slots[j].h2] == 0) {
                        buf->slots[j].h2 = 0;
                        buf->slots[j].h0 = 0;
                    }
                }
                func_800A355C(&buf->slots[j], j, buf);
                buf->slots[j].h0++;
                buf->slots[j].h1++;
            }
        }
        for (j = 0; j < 128; j++) {
            if (buf->entries[j].active == 1) {
                func_800A38B4(&buf->entries[j],
                              &buf->records[buf->entries[j].cmdIndex].steps[buf->entries[j].stepIndex],
                              &buf->records[buf->entries[j].cmdIndex].steps[buf->entries[j].stepIndex + 1]);
                if (buf->records[buf->entries[j].cmdIndex].steps[buf->entries[j].stepIndex].stepTotal == 0) {
                    buf->entries[j].active = 0;
                    buf->records[buf->entries[j].cmdIndex].activeCount--;
                }
                buf->entries[j].stepProgress++;
                if (buf->entries[j].stepProgress >=
                    buf->records[buf->entries[j].cmdIndex].steps[buf->entries[j].stepIndex].stepTotal) {
                    buf->entries[j].stepProgress = 0;
                    buf->entries[j].stepIndex++;
                }
            }
        }
    }

    for (i = 0; i < 16; i++) {
        g_fieldEntity.slotActive[i] = saved[i];
    }
}

/**
 * @brief Initialise the field-object GPU primitive packets and seed the
 *        8-object shimmer state.
 *
 * Runs three passes:
 *  1. **40 gouraud quads** (@p polys): stamp each @ref POLY_G4 with a length
 *     of @c 8 words and code @c 0x3A (gouraud four-point, semi-transparent).
 *  2. **32 draw-mode packets** (@p tpages): stamp each @ref DR_TPAGE with a
 *     length of @c 1 word and a GP0(E1h) draw-mode command whose low bits come
 *     from @c GetTPage() (texture page / semi-transparency selection).
 *  3. **8 shimmer objects**: for each @ref ObjSlot / @ref DrawPoint pair, sample
 *     a perturbation byte from @c D_800C3520 at the current VSync phase
 *     (@c D_8005F154 @c + slot), offset the draw-point corners by it, mirror the
 *     base @c (x,y,z) into all 8 vertices of both corner buffers, and reset the
 *     per-object tick (@c field80 @c = @c 0x10 @c + slot*2) and @c field82.
 *
 * @param polys  Array of 40 @ref POLY_G4 quad primitives to initialise.
 * @param tpages Array of 32 @ref DR_TPAGE draw-mode packets to initialise.
 *
 * @note The @c P_TAG length byte sits 4 bytes before the @c code byte; both
 *       passes write the length through the same walking @c code cursor
 *       (@c c[-4] / @c w[-1]) so gcc keeps the loop induction variable anchored
 *       at the code field, matching the original's cursor.
 */
void func_800A42EC(POLY_G4 *polys, DR_TPAGE *tpages) {
    s32 i, j;
    POLY_G4 *p;
    DR_TPAGE *q;

    i = 0;
    p = polys;
    do {
        u8 *c = &p->code;
        c[-4] = 8;
        *c = 0x3A;
        p++;
    } while (++i < 40);

    i = 0;
    q = tpages;
    do {
        u32 *w = &q->code[0];
        ((u8 *)w)[-1] = 1;
        *w = (GetTPage(0, 1, 0, 0) & 0x9FF) | 0xE1000200;
        q++;
    } while (++i < 32);

    for (i = 0; i < 8; i++) {
        D_800C6DA0[i].field86 = D_800C3520[(D_8005F154 + i) & 0xFF];
        D_800C6DA0[i].field87 = D_800C3520[(D_8005F154 + i) & 0xFF];
        D_800706A0[i].field8 = (D_800706A0[i].x + D_800C3520[(D_8005F154 + i) & 0xFF]) - 0x80;
        D_800706A0[i].fieldA = (D_800706A0[i].y + D_800C3520[(D_8005F154 + i + 8) & 0xFF]) - 0x80;
        D_800706A0[i].fieldC = D_800706A0[i].z + 0x40;
        D_800706A0[i].field10 = D_800706A0[i].x;
        D_800706A0[i].field12 = D_800706A0[i].y;
        D_800706A0[i].field14 = D_800706A0[i].z + 0x80;

        for (j = 0; j < 8; j++) {
            D_800C6DA0[i].va[j].x = D_800C6DA0[i].vb[j].x = D_800706A0[i].x;
            D_800C6DA0[i].va[j].y = D_800C6DA0[i].vb[j].y = D_800706A0[i].y;
            D_800C6DA0[i].va[j].z = D_800C6DA0[i].vb[j].z = D_800706A0[i].z;
        }

        D_800C6DA0[i].field80 = 0x10 + i * 2;
        D_800C6DA0[i].field82 = 0;
    }
}

/**
 * Zero 8 bytes of D_8005F168 (backwards loop).
 */
void func_800A44D8(void) {
    s32 i = 7;
    volatile u8 *base = D_8005F168;
    u8 *ptr = (u8 *)base + 7;
    do {
        *ptr = 0;
        i--;
        ptr--;
    } while (i >= 0);
}

/**
 * @brief Install the same draw-point at all 8 slots and mark them active.
 *
 * Iterates the 8-slot @c D_800706A0 draw-point table and writes the
 * 12.4 fixed-point position (@p x, @p y, @p z) into each slot's
 * @c x/y/z fields (after shifting down by 12 to drop the fractional
 * bits), and sets the matching @c D_8005F168[i] flag to 1 to mark the
 * slot as occupied.
 *
 * Called from the @c SETDRAWPOINT script opcode with the source
 * actor's position.
 */
void func_800A4500(s32 x, s32 y, s32 z) {
    s32 i;
    for (i = 0; i < 8; i++) {
        D_8005F168[i] = 1;
        D_800706A0[i].x = x >> 12;
        D_800706A0[i].y = y >> 12;
        D_800706A0[i].z = z >> 12;
    }
}

/**
 * Stores a halfword value to the global D_8005F122.
 *
 * @param a0 The value to store.
 */
void func_800A4550(s16 a0) {
    D_8005F122 = a0;
}

/**
 * @brief Spawn the 8 shimmer objects aimed at an entity.
 *
 * Computes the facing angle from draw-point 0 (@c D_800706A0[0]) to entity
 * @p entityIdx 's world position (@ref Actor @c posX / @c posY, right-shifted
 * out of 12-bit fixed point) via @c func_8009A0E8, then arms all 8 object slots:
 *  - marks each slot flag @c D_8005F168[i] @c = @c 2 ("spawned"; later detected
 *    by the @c ==2 scan);
 *  - stores the facing angle @c ±0x40 into @c field86 / @c field87;
 *  - seeds the draw-point corners with a table perturbation scaled ×4 and
 *    biased by @c -0x200 (@c field8 / @c fieldA), a rising Z offset
 *    @c 1000 @c + slot*128 (@c fieldC), and the entity position for
 *    @c field10 / @c field12 / @c field14 (Z @c + @c 0xB4);
 *  - mirrors the base @c (x,y,z) into all 8 vertices of both corner buffers;
 *  - resets the per-object tick (@c field80 @c = @c 0x18 @c + slot*2) and
 *    @c field82.
 *
 * @param entityIdx Index into the @ref Actor entity array (@c D_80085224) that
 *                  the shimmer objects are aimed at.
 */
void func_800A455C(s16 entityIdx) {
    s32 i, j;
    s32 angle;
    s32 objPos[3];
    s32 entityPos[3];
    s32 dist[2];

    objPos[0] = (s16)D_800706A0[0].x;
    objPos[1] = (s16)D_800706A0[0].y;
    objPos[2] = 0;
    entityPos[0] = D_80085224[entityIdx].posX >> 12;
    entityPos[1] = D_80085224[entityIdx].posY >> 12;
    entityPos[2] = 0;
    angle = func_8009A0E8(objPos, entityPos, dist);

    for (i = 0; i < 8; i++) {
        D_8005F168[i] = 2;
        D_800C6DA0[i].field86 = angle + 0x40;
        D_800C6DA0[i].field87 = angle - 0x40;
        D_800706A0[i].field8 = (D_800706A0[i].x + D_800C3520[(D_8005F154 + i) & 0xFF] * 4) - 0x200;
        D_800706A0[i].fieldA = (D_800706A0[i].y + D_800C3520[(D_8005F154 + i + 8) & 0xFF] * 4) - 0x200;
        D_800706A0[i].fieldC = D_800706A0[i].z + (1000 + i * 128);
        D_800706A0[i].field10 = D_80085224[entityIdx].posX >> 12;
        D_800706A0[i].field12 = D_80085224[entityIdx].posY >> 12;
        D_800706A0[i].field14 = (D_80085224[entityIdx].posZ >> 12) + 0xB4;

        for (j = 0; j < 8; j++) {
            D_800C6DA0[i].va[j].x = D_800C6DA0[i].vb[j].x = D_800706A0[i].x;
            D_800C6DA0[i].va[j].y = D_800C6DA0[i].vb[j].y = D_800706A0[i].y;
            D_800C6DA0[i].va[j].z = D_800C6DA0[i].vb[j].z = D_800706A0[i].z;
        }

        D_800C6DA0[i].field80 = 0x18 + i * 2;
        D_800C6DA0[i].field82 = 0;
    }
}

/**
 * @brief Seed the 8 active object slots' draw geometry from their base points.
 *
 * Walks the 8 @ref ObjSlot / @ref DrawPoint pairs. For each slot not yet marked
 * ready (@c D_8005F168[i] == 0) it marks it, then:
 *  - stamps a perturbation byte (@c D_800C3520 indexed by the VSync phase
 *    @c D_8005F154 + slot) into @c field86 / @c field87;
 *  - derives the draw-point's two corner offsets: @c field8/A/C = base minus a
 *    table-perturbed 0x80 bias (X and Y perturbed by the table at phases +i and
 *    +i+8; Z a flat +0x40), and @c field10/12/14 = base plus 0/0/0x80;
 *  - replicates the base (x, y, z) into all 8 entries of both vertex arrays
 *    @c va / @c vb;
 *  - resets the slot's tick pair (@c field80 = 0x10 + 2*slot, @c field82 = 0).
 *
 * @note The unused @c frameSlot local reserves the 0x20 stack frame the
 *       original build allocated (its scratch was register-allocated away).
 */
void func_800A4758(void) {
    struct { s32 w[8]; } frameSlot;
    s32 i, j;

    for (i = 0; i < 8; i++) {
        if (D_8005F168[i] == 0) {
            D_8005F168[i] = 1;
            D_800C6DA0[i].field86 = D_800C3520[(D_8005F154 + i) & 0xFF];
            D_800C6DA0[i].field87 = D_800C3520[(D_8005F154 + i) & 0xFF];
            D_800706A0[i].field8 = (D_800706A0[i].x + D_800C3520[(D_8005F154 + i) & 0xFF]) - 0x80;
            D_800706A0[i].fieldA = (D_800706A0[i].y + D_800C3520[(D_8005F154 + i + 8) & 0xFF]) - 0x80;
            D_800706A0[i].fieldC = D_800706A0[i].z + 0x40;
            D_800706A0[i].field10 = D_800706A0[i].x;
            D_800706A0[i].field12 = D_800706A0[i].y;
            D_800706A0[i].field14 = D_800706A0[i].z + 0x80;
            for (j = 0; j < 8; j++) {
                D_800C6DA0[i].va[j].x = D_800C6DA0[i].vb[j].x = D_800706A0[i].x;
                D_800C6DA0[i].va[j].y = D_800C6DA0[i].vb[j].y = D_800706A0[i].y;
                D_800C6DA0[i].va[j].z = D_800C6DA0[i].vb[j].z = D_800706A0[i].z;
            }
            D_800C6DA0[i].field80 = 0x10 + i * 2;
            D_800C6DA0[i].field82 = 0;
        }
    }
}

/**
 * @brief Returns 1 if any of the 8 slot bytes in @c D_8005F168 equals 2.
 *
 * Linear scan over @c D_8005F168[0..7] looking for the value @c 2. Sets
 * a result flag (never early-exits, scans all 8 slots regardless of
 * when the match is found). Used by @c fe_object7 's @c DRAWPOINT
 * dispatcher (case 5) as a predicate.
 *
 * @c D_8005F168 is a 8-slot table whose entries are written elsewhere
 * by the field engine; the value @c 2 here is one of those slot states.
 */
s32 func_800A48CC(void) {
    s32 result = 0;
    s32 i;
    for (i = 0; i < 8; i++) {
        if (D_8005F168[i] == 2) {
            result = 1;
        }
    }
    return result;
}

/**
 * Linear interpolation: a0 + (a1 - a0) * a3 / a2.
 *
 * @param a0 Start value.
 * @param a1 End value.
 * @param a2 Divisor.
 * @param a3 Numerator.
 * @return Interpolated value.
 */
s32 func_800A4910(s32 a0, s32 a1, s32 a2, s32 a3) {
    a1 -= a0;
    return a0 + a1 * a3 / a2;
}

/**
 * @brief Rebuild one shimmer object's two corner vertices for this tick.
 *
 * Re-seeds the slot pool (@c func_800A4758), then stages the object's current
 * mid-point in the scratchpad: once the tick counter @c field82 has passed
 * @c field80 the draw-point's settled position (@c field10 / @c field12 /
 * @c field14) is used directly, otherwise the point is interpolated twice with
 * @c func_800A4910, base to first corner and first corner to second, both at
 * @c field82 / @c field80, and the two results interpolated again.
 *
 * The staged mid-point is then spread into the slot's two corner arrays at
 * index @c field82 & 7: a cosine/sine pair sampled at @c field86 (scaled by
 * @c >> 8) is scaled by the cosine of the tick angle @c (field82 * 8) & 0xF8
 * and applied as @c >> 12, added on one corner and subtracted on the other, so
 * the pair straddles the mid-point along the tick direction. Z is copied
 * unchanged to both.
 *
 * @param slot Object slot to update.
 * @param dp   Draw point holding the object's base and corner positions.
 */
void func_800A4934(ObjSlot *slot, DrawPoint *dp) {
    ObjVertex *mid = (ObjVertex *)getScratchAddr(0);
    ObjVertex *end = (ObjVertex *)getScratchAddr(2);
    s32 idx;

    func_800A4758();
    idx = slot->field82 & 7;
    if (slot->field80 < (s16)slot->field82) {
        mid->x = dp->field10;
        mid->y = dp->field12;
        mid->z = dp->field14;
    } else {
        mid->x = func_800A4910((s16)dp->x, (s16)dp->field8, slot->field80, (s16)slot->field82);
        mid->y = func_800A4910((s16)dp->y, (s16)dp->fieldA, slot->field80, (s16)slot->field82);
        mid->z = func_800A4910((s16)dp->z, (s16)dp->fieldC, slot->field80, (s16)slot->field82);
        end->x = func_800A4910((s16)dp->field8, (s16)dp->field10, slot->field80, (s16)slot->field82);
        end->y = func_800A4910((s16)dp->fieldA, (s16)dp->field12, slot->field80, (s16)slot->field82);
        end->z = func_800A4910((s16)dp->fieldC, (s16)dp->field14, slot->field80, (s16)slot->field82);
        mid->x = func_800A4910((s16)mid->x, (s16)end->x, slot->field80, (s16)slot->field82);
        mid->y = func_800A4910((s16)mid->y, (s16)end->y, slot->field80, (s16)slot->field82);
        mid->z = func_800A4910((s16)mid->z, (s16)end->z, slot->field80, (s16)slot->field82);
    }

    slot->vb[idx].x = mid->x - ((func_8009D234(slot->field86) >> 8) *
                                func_8009D234(((u8)slot->field82 << 3) & 0xF8) >> 12);
    slot->vb[idx].y = mid->y + ((func_8009D254(slot->field86) >> 8) *
                                func_8009D234(((u8)slot->field82 << 3) & 0xF8) >> 12);
    slot->vb[idx].z = mid->z;
    slot->va[idx].x = mid->x + ((func_8009D234(slot->field86) >> 8) *
                                func_8009D234(((u8)slot->field82 << 3) & 0xF8) >> 12);
    slot->va[idx].y = mid->y - ((func_8009D254(slot->field86) >> 8) *
                                func_8009D234(((u8)slot->field82 << 3) & 0xF8) >> 12);
    slot->va[idx].z = mid->z;
}

/**
 * @brief Draws one shimmer object's four-segment trailing ribbon.
 *
 * The slot keeps two 8-entry ring buffers of corner vertices, @c va and @c vb
 * (the left and right edge of the ribbon), with @c field82 as the write cursor.
 * Starting at the newest entry this walks six entries back through the ring and
 * projects the twelve corners in four @c RotTransPers3 calls, three points per
 * call, producing a zigzag @c va[i], @c vb[i], @c va[i-1], @c vb[i-1], ... that
 * reads as a ribbon when stroked.
 *
 * Those twelve points are stroked as five overlapping four-point @c LINE_G4
 * strips, each sharing its first two points with the previous one. Points a
 * call does not write are copied over from the neighbouring strip, so only the
 * projections cost GTE time.
 *
 * Each call's OTZ links that block's strips into the ordering table at their
 * own depth, with a tpage command of their own, so the ribbon sorts correctly
 * against the rest of the scene even when it spans a large depth range. A
 * strip whose OTZ is beyond @c 0x1000 is simply not linked.
 *
 * The head strip is white; the rest fade along the trail through the five RGB
 * triples of @c D_800C3720, selected by @c D_80070657.
 *
 * @param slot  Shimmer object slot holding the two corner ring buffers.
 * @param ot    Ordering table to link the strips into.
 * @param line0 The slot's five @c LINE_G4 strips.
 * @param tp    The slot's four tpage commands, one per block.
 *
 * @note @c line3 and @c line4 are linked as @c &line2[1] / @c &line3[1] rather
 *       than by name: cse rewrites a bare strip pointer used as an address into
 *       whichever equivalent base it has already hashed, and only these
 *       spellings keep the previous strip as that base.
 * @note The @c line0 bump pair is load-bearing. gcc 2.7.2 double-counts
 *       @c reg_live_length for a parameter that is live-in and never
 *       reassigned, which halves its priority in @c allocno_compare and costs
 *       @c line0 the first callee-saved register. A second assignment restores
 *       the true count; the pair folds away before the prologue is emitted.
 */
void func_800A4C14(ObjSlot *slot, u32 *ot, LINE_G4 *line0, DR_TPAGE *tp) {
    LINE_G4 *line1;
    LINE_G4 *line2;
    LINE_G4 *line3;
    LINE_G4 *line4;
    s32 pal;
    s32 otz;
    s32 i;
    s32 p;
    s32 flag;

    line0++;
    line0--;
    i = slot->field82 & 7;
    pal = D_80070657 * 16;

    otz = RotTransPers3(&slot->va[i], &slot->vb[i], &slot->va[(i - 1) & 7],
                        (s32 *)&line0->x0, (s32 *)&line0->x1, (s32 *)&line0->x2, &p, &flag);

    line1 = &line0[1];
    line2 = &line0[2];
    line3 = &line0[3];
    line4 = &line0[4];
    if (otz < 0x1000) {
        line0->r1 = line0->g1 = line0->b1 = 0x80;
        line0->r0 = line0->g0 = line0->b0 = 0x80;
        addPrim(&ot[otz], line0);
        addPrim(&ot[otz], tp);
        tp++;
    }

    line1->x0 = line0->x2;
    line1->y0 = line0->y2;

    otz = RotTransPers3(&slot->vb[(i - 1) & 7], &slot->va[(i - 2) & 7], &slot->vb[(i - 2) & 7],
                        (s32 *)&line1->x1, (s32 *)&line1->x2, (s32 *)&line1->x3, &p, &flag);
    if (otz < 0x1000) {
        line0->r2 = line0->r3 = line1->r0 = line1->r1 = D_800C3720[pal];
        line0->g2 = line0->g3 = line1->g0 = line1->g1 = D_800C3720[pal + 1];
        line0->b2 = line0->b3 = line1->b0 = line1->b1 = D_800C3720[pal + 2];
        line1->r2 = line1->r3 = line2->r0 = line2->r1 = D_800C3720[pal + 3];
        line1->g2 = line1->g3 = line2->g0 = line2->g1 = D_800C3720[pal + 4];
        line1->b2 = line1->b3 = line2->b0 = line2->b1 = D_800C3720[pal + 5];
        addPrim(&ot[otz], line1);
        addPrim(&ot[otz], line2);
        addPrim(&ot[otz], tp);
        tp++;
    }

    line0->x3 = line1->x1;
    line0->y3 = line1->y1;
    line2->x0 = line1->x2;
    line2->y0 = line1->y2;
    line2->x1 = line1->x3;
    line2->y1 = line1->y3;

    otz = RotTransPers3(&slot->va[(i - 3) & 7], &slot->vb[(i - 3) & 7], &slot->va[(i - 4) & 7],
                        (s32 *)&line3->x0, (s32 *)&line3->x1, (s32 *)&line3->x2, &p, &flag);
    if (otz < 0x1000) {
        line2->r2 = line2->r3 = line3->r0 = line3->r1 = D_800C3726[pal];
        line2->g2 = line2->g3 = line3->g0 = line3->g1 = D_800C3726[pal + 1];
        line2->b2 = line2->b3 = line3->b0 = line3->b1 = D_800C3726[pal + 2];
        addPrim(&ot[otz], &line2[1]);
        addPrim(&ot[otz], tp);
        tp++;
    }

    line2->x2 = line3->x0;
    line2->y2 = line3->y0;
    line2->x3 = line3->x1;
    line2->y3 = line3->y1;
    line4->x0 = line3->x2;
    line4->y0 = line3->y2;

    otz = RotTransPers3(&slot->vb[(i - 4) & 7], &slot->va[(i - 5) & 7], &slot->vb[(i - 5) & 7],
                        (s32 *)&line4->x1, (s32 *)&line4->x2, (s32 *)&line4->x3, &p, &flag);
    if (otz < 0x1000) {
        line3->r2 = line3->r3 = line4->r0 = line4->r1 = D_800C3729[pal];
        line3->g2 = line3->g3 = line4->g0 = line4->g1 = D_800C3729[pal + 1];
        line3->b2 = line3->b3 = line4->b0 = line4->b1 = D_800C3729[pal + 2];
        line4->r2 = line4->r3 = D_800C3729[pal + 3];
        line4->g2 = line4->g3 = D_800C3729[pal + 4];
        line4->b2 = line4->b3 = D_800C3729[pal + 5];
        addPrim(&ot[otz], &line3[1]);
        addPrim(&ot[otz], tp);
        tp++;
    }

    line3->x3 = line4->x1;
    line3->y3 = line4->y1;
}

/**
 * @brief 8-iteration script-dispatch loop with per-slot flag-driven
 *        callbacks and a tick counter that auto-clears.
 *
 * Sets the GTE rotation/translation matrix from @p m (guarded by
 * @c PushMatrix / @c PopMatrix), then iterates @c i in @c [0,8) over
 * four parallel arrays: @c D_800C6DA0 (@ref ObjSlot, stride 0x88),
 * @c D_800706A0 (@ref DrawPoint, stride 0x18), @p tpages (stride 0x20), and
 * @p prims (stride 0xB4). When the per-slot flag @c D_8005F168[i] is
 * non-zero, runs @c func_800A4934 on the slot, then conditionally
 * @c func_800A4C14 (when the flag is @c 2, or it is @c 1 and
 * @c D_8005F122 @c == @c 1), then bumps @c ObjSlot.field82 and clears the
 * flag once it exceeds @c field80 + 4.
 *
 * @param m      GTE rotation/translation matrix.
 * @param ot     Ordering table the ribbons are linked into.
 * @param prims  Per-slot ribbon strips.
 * @param tpages Per-slot ribbon tpage commands.
 */
void func_800A5224(MATRIX *m, u32 *ot, FieldRibbonPrims *prims,
                   FieldRibbonTPages *tpages) {
    s32 i;

    PushMatrix();
    SetRotMatrix(m);
    SetTransMatrix(m);
    for (i = 0; i < 8; i++) {
        if (D_8005F168[i] != 0) {
            u8 flag;
            s16 tick;
            func_800A4934(&D_800C6DA0[i], &D_800706A0[i]);
            flag = D_8005F168[i];
            if (flag == 2 || (flag == 1 && D_8005F122 == flag)) {
                func_800A4C14(&D_800C6DA0[i], ot, prims[i].lines, tpages[i].tpages);
            }
            tick = D_800C6DA0[i].field82 + 1;
            D_800C6DA0[i].field82 = tick;
            if (tick > D_800C6DA0[i].field80 + 4) {
                D_8005F168[i] = 0;
            }
        }
    }
    PopMatrix();
}

/**
 * @brief Reset the ordering table and link the double-buffer's clear-tile
 *        primitives, tinted (@p r, @p g, @p b) and faded by @c dialogTimer.
 *
 * The brightness-scaled twin of @ref func_800A553C: it clears the active
 * buffer's ordering table via @c ClearOTagR, then writes each fill-color
 * component scaled by the current dialog fade level
 * (@c dialogTimer, 0..256) into that buffer's clear @ref TILE, and finally
 * prepends both the tile and its preceding @ref DR_MODE primitive (12 bytes
 * before the tile) to the caller-supplied ordering table @p ot.
 *
 * @param ot Ordering-table slot to link the two clear primitives into.
 * @param r  Fill red  , scaled to @c r*dialogTimer/256, low byte to TILE @c r0.
 * @param g  Fill green, scaled to @c g*dialogTimer/256, low byte to TILE @c g0.
 * @param b  Fill blue , scaled to @c b*dialogTimer/256, low byte to TILE @c b0.
 *
 * @note @c g_bufferIndex is @c volatile, so every subscript re-reads it; the
 *       @c dialogTimer read is likewise forced through a @c volatile pointer to
 *       reload each component (its fade value can change between frames).
 *       The first store caches the tile index in @c idx so gcc emits the index
 *       before the shared @c g_clearTiles base, matching the original schedule
 *       in the multiply's delay window; @c g0 / @c b0 recompute it inline (each
 *       is its own @c volatile @c g_bufferIndex read).
 */
void func_800A5360(u32 *ot, s16 r, s16 g, s16 b) {
    ClearOTagR(&g_orderingTablePtrs[(s16)g_bufferIndex], 1);
    {
        volatile SystemState *sys = &g_fieldEntity;
        s32 idx = (s16)g_bufferIndex * 2;
        g_clearTiles[idx].r0 = (s16)sys->dialogTimer * r / 256;
        g_clearTiles[(s16)g_bufferIndex * 2].g0 = (s16)sys->dialogTimer * g / 256;
        g_clearTiles[(s16)g_bufferIndex * 2].b0 = (s16)sys->dialogTimer * b / 256;
    }
    addPrim(ot, &g_clearTiles[(s16)g_bufferIndex * 2]);
    addPrim(ot, (DR_MODE *)&g_clearTiles[(s16)g_bufferIndex * 2] - 1);
}

/**
 * @brief Reset the current frame's ordering table and link the double-buffer's
 *        clear-tile primitives, tinted (@p r, @p g, @p b), into @p ot.
 *
 * A generalized sibling of @c BuildPrimList: it clears the active buffer's
 * ordering table via @c ClearOTagR, writes the fill color into that buffer's
 * clear @ref TILE, then prepends both the tile and its preceding @ref DR_MODE
 * primitive (12 bytes before the tile) to the caller-supplied ordering table
 * @p ot. @c g_bufferIndex is @c volatile, so each subscript re-reads it.
 *
 * @param ot Ordering-table slot to link the two clear primitives into.
 * @param r  Fill red  , low byte stored to TILE @c r0.
 * @param g  Fill green, low byte stored to TILE @c g0.
 * @param b  Fill blue , low byte stored to TILE @c b0.
 *
 * @note @p r / @p g / @p b are @c s16 because the sole caller (@c func_800A5788)
 *       feeds them the signed fade-lerp results; only the low byte reaches each
 *       TILE color component.
 */
void func_800A553C(u32 *ot, s16 r, s16 g, s16 b) {
    ClearOTagR(&g_orderingTablePtrs[(s16)g_bufferIndex], 1);
    g_clearTiles[(s16)g_bufferIndex * 2].r0 = r;
    g_clearTiles[(s16)g_bufferIndex * 2].g0 = g;
    g_clearTiles[(s16)g_bufferIndex * 2].b0 = b;
    addPrim(ot, &g_clearTiles[(s16)g_bufferIndex * 2]);
    addPrim(ot, (DR_MODE *)&g_clearTiles[(s16)g_bufferIndex * 2] - 1);
}

/**
 * @brief Dialog tick that counts the timer DOWN; finalize on expire or
 *        when @c func_800BE274 reports a script gate is open.
 *
 * Decrements @c dialogTimer by @c dialogCount (one count-down step),
 * unconditionally clears the @c unk1A1 flag, then:
 *   - If the new timer is still positive (more frames to wait), polls
 *     @c func_800BE274, if it returns 0 (gate not yet open), we keep
 *     the dialog state intact and return.
 *   - Otherwise (timer expired @em or gate now open) clears both
 *     @c dialogState and @c dialogTimer so the dialog state machine
 *     can advance.
 *
 * Companion to @c func_800A5700 (which counts the timer up); both are
 * driven from the dialog state machine each frame.
 */
void func_800A5698(void) {
    SystemState *sys = &g_fieldEntity;
    sys->dialogTimer -= sys->dialogCount;
    sys->unk1A1 = 0;
    if ((s16)*(volatile u16 *)&sys->dialogTimer > 0) {
        if (func_800BE274() == 0) return;
    }
    sys->dialogState = 0;
    sys->dialogTimer = 0;
}

/**
 * @brief Advance the dialog timer by one step and clamp at 0xFF.
 *
 * Adds @c dialogCount to @c dialogTimer (one tick of the dialog scroll
 * accumulator), unconditionally clears the @c unk1A1 flag, then if the
 * advanced timer overflows 8-bit range (>= 256 as signed) clamps it
 * back down to 0xFF.
 *
 * The @c (s16)*(volatile u16 *)&...->dialogTimer cast forces a reload
 * of the just-stored value (without making the canonical struct member
 * volatile), matches the target's lhu-then-sign-extend pattern instead
 * of letting gcc reuse the post-increment value still in a register.
 */
void func_800A5700(void) {
    SystemState *sys = &g_fieldEntity;
    sys->dialogTimer += sys->dialogCount;
    sys->unk1A1 = 0;
    if ((s16)*(volatile u16 *)&sys->dialogTimer >= 256) {
        sys->dialogTimer = 0xFF;
    }
}

/**
 * @brief Linear interpolation between two s16 endpoints.
 *
 * Returns @c start + ((end - start) * progress) / total, the standard
 * `start * (1 - progress/total) + end * (progress/total)` lerp evaluated
 * in integer arithmetic, with the difference narrowed to s16 before the
 * multiplication so the product fits in s32 even for large @p progress.
 *
 * @param start    Value at @c progress == 0.
 * @param end      Value at @c progress == total.
 * @param progress Current step (typically @c [0, total]).
 * @param total    Step count denominator.
 */
s16 func_800A5748(s16 start, s16 end, s16 progress, s16 total) {
    s16 diff = end - start;
    return start + (diff * progress) / total;
}

/**
 * @brief Companion of @c func_800A5700, advances the dialog-pos animation
 *        and dispatches the per-frame visual update.
 *
 * Increments @c dialogTimer; once it catches up to @c dialogCount, snapshots
 * the destination triple (@c field_0x11A/11C/11E) into the current triple
 * (@c field_0x114/116/118) and resets @c dialogTimer to @c dialogCount (the
 * snapshot is what subsequent ticks lerp back from). Each tick then runs the
 * three-axis safe-lerp via @c func_800A5748 and stores the result back to
 * @c field_0x10E/110/112, finally calling @c func_800A553C with the new
 * (x, y, z) tuple.
 *
 * @param a0 Opaque pointer forwarded as @c func_800A553C's first arg.
 */
void func_800A5788(FieldFrameBuf *buf) {
    SystemState *sys = &g_fieldEntity;

    sys->unk1A1 = 0;
    sys->dialogTimer++;
    if ((s16)*(volatile u16 *)&sys->dialogTimer >= (s16)*(volatile u16 *)&sys->dialogCount) {
        sys->field_0x114 = sys->field_0x11A;
        sys->field_0x116 = sys->field_0x11C;
        sys->field_0x118 = sys->field_0x11E;
        sys->dialogTimer = *(volatile u16 *)&sys->dialogCount;
    }
    sys->field_0x10E = func_800A5748((s16)sys->field_0x114, (s16)sys->field_0x11A,
                                     (s16)*(volatile u16 *)&sys->dialogTimer,
                                     (s16)*(volatile u16 *)&sys->dialogCount);
    sys->field_0x110 = func_800A5748((s16)sys->field_0x116, (s16)sys->field_0x11C,
                                     (s16)*(volatile u16 *)&sys->dialogTimer,
                                     (s16)*(volatile u16 *)&sys->dialogCount);
    func_800A553C(buf->ot, (s16)sys->field_0x10E, (s16)sys->field_0x110,
                  (s16)(sys->field_0x112 = func_800A5748((s16)sys->field_0x118,
                                                         (s16)sys->field_0x11E,
                                                         (s16)*(volatile u16 *)&sys->dialogTimer,
                                                         (s16)*(volatile u16 *)&sys->dialogCount)));
}

/**
 * @brief Render one frame of the dialog/fade overlay for the current dialog state.
 *
 * Dispatches on @c g_fieldEntity.dialogState into @p buf 's ordering table:
 *
 *  - @c 0: idle — clear the fade colour accumulators and the re-arm guard.
 *  - @c 1: nothing (the state is owned by someone else this frame).
 *  - @c 2 / @c 3: dialog is running — tick the timer down (@c func_800A5698) or
 *    up (@c func_800A5700), then draw the window with @c func_800A5360.
 *  - @c 4: raise @c D_80070649 and leave the frame alone.
 *  - @c 5 / @c 6: interpolate the fade colour for this tick (@c func_800A5788).
 *  - @c 7 / @c 8: draw the flat fade tint with @c func_800A553C.
 *
 * The @c SetupDrawMode argument that precedes each draw selects the blend mode,
 * @c 1 or @c 2 depending on the state.
 *
 * @param buf The frame's GPU work area; its ordering table receives the prims.
 */
void func_800A5898(FieldFrameBuf *buf) {
    SystemState *sys = &g_fieldEntity;

    switch ((s16)sys->dialogState) {
    case 0:
        g_fieldEntity.unk1A1 = 0;
        g_fieldEntity.field_0x114 = 0;
        g_fieldEntity.field_0x116 = 0;
        g_fieldEntity.field_0x118 = 0;
        break;
    case 1:
        break;
    case 2:
        SetupDrawMode(2);
        func_800A5698();
        func_800A5360(buf->ot, g_fieldEntity.field_0x10E, g_fieldEntity.field_0x110, g_fieldEntity.field_0x112);
        break;
    case 3:
        SetupDrawMode(2);
        func_800A5700();
        func_800A5360(buf->ot, g_fieldEntity.field_0x10E, g_fieldEntity.field_0x110, g_fieldEntity.field_0x112);
        break;
    case 4:
        D_80070649 = 1;
        break;
    case 7:
        g_fieldEntity.unk1A1 = 0;
        SetupDrawMode(1);
        func_800A553C(buf->ot, g_fieldEntity.field_0x10E, g_fieldEntity.field_0x110, g_fieldEntity.field_0x112);
        break;
    case 8:
        g_fieldEntity.unk1A1 = 0;
        SetupDrawMode(2);
        func_800A553C(buf->ot, g_fieldEntity.field_0x10E, g_fieldEntity.field_0x110, g_fieldEntity.field_0x112);
        break;
    case 5:
        SetupDrawMode(1);
        func_800A5788(buf);
        break;
    case 6:
        SetupDrawMode(2);
        func_800A5788(buf);
        break;
    }
}

/**
 * If D_8005F14A equals 1, calls resetCdDrive. Then clears D_8005F100 and D_8005F14A.
 *
 * @note K&R declarator: the body ignores its argument and callers vary between
 *       passing none (@c func_800A5A20, fe_object5) and one.
 */
void func_800A59D0() {

    if (D_8005F14A == 1) {
        resetCdDrive();
    }
    D_8005F100 = 0;
    D_8005F14A = 0;
}

/**
 * Stores a halfword value to the global D_8005F142.
 *
 * @param a0 The value to store.
 */
void func_800A5A14(s16 a0) {
    D_8005F142 = a0;
}

/** @brief Words per streaming-table entry (24-byte stride). */
#define FIELD_STREAM_STRIDE 6

/**
 * @brief Per-frame background preload of the field nearest the player.
 *
 * Snapshots the player position into the scratchpad (whole units), then, while
 * the map is not suppressed by @c D_800704BD, scans the 12 event-queue entries
 * for the armed one (@c counter != @c 0x7FFF) whose trigger-segment start is
 * nearest in XY, recording its @c counter (the destination field id) in
 * @c D_8005F142.
 *
 * The streaming half then runs unless the movie subsystem is busy or the engine
 * is in mode 3 (both abort through @c func_800A59D0), and unless a load is
 * already pending. The destination pointer @c D_8005F104 is floored to
 * @c 0x801A0000, and when the nearest field differs from the one already loaded
 * (@c D_8005F100) and its data still fits below @c 0x801FE000, the previous load
 * is cancelled and a new @c cdRead is issued: field ids from @c 0x49 up are
 * looked up through @c D_800C2568 into the streaming table, lower ids use the
 * fixed descriptor @c D_800974D0. @c D_8005F14A is left at 1 to mark the read
 * in flight.
 *
 * @param actor    Player entity, read for its 20.12 world position.
 * @param entries Event-queue entry array (12 slots).
 */
void func_800A5A20(Actor *actor, EventEntry *entries) {
    Vec3i *scratch = (Vec3i *)getScratchAddr(0);
    s32 best;
    s32 i;
    s32 dx;
    s32 dy;
    s32 d;
    s32 idx;
    u16 id;

    if (D_8005F14A == 1) {
        if (func_800393C8() == 0) {
            D_8005F14A = 2;
        }
    }
    best = 0x7FFFFFFF;
    scratch->x = actor->posX >> 12;
    scratch->y = actor->posY >> 12;
    scratch->z = actor->posZ >> 12;

    if (D_800704BD == 0) {
        for (i = 0; i < 12; i++, entries++) {
            id = entries->counter;
            if (id != 0x7FFF) {
                dx = entries->x0 - scratch->x;
                dy = entries->y0 - scratch->y;
                d = dx * dx + dy * dy;
                if (d < best) {
                    best = d;
                    D_8005F142 = id;
                }
            }
        }
    }

    if (func_800BE264() != 0 || g_fieldEntity.mode == 3) {
        func_800A59D0();
        return;
    }
    if (func_800393C8() != 0 && D_8005F14A == 0) {
        return;
    }
    if ((u32)D_8005F104 <= 0x8019FFFF) {
        D_8005F104 = 0x801A0000;
    }
    if (D_8005F100 == D_8005F142) {
        return;
    }
    if (D_800C0904[D_800C2568[D_8005F142] * FIELD_STREAM_STRIDE] < 0x801FE000 - D_8005F104) {
        func_800A59D0();
        D_8005F100 = D_8005F142;
        if (D_8005F142 >= 0x49) {
            idx = D_800C2568[D_8005F142];
            cdRead(D_800C0904[idx * FIELD_STREAM_STRIDE - 1],
                   D_800C0904[idx * FIELD_STREAM_STRIDE], (u8 *)D_8005F104, NULL);
        } else {
            cdRead(D_800974D0[0].sector, D_800974D0[0].size, (u8 *)D_8005F104, NULL);
        }
        D_8005F14A = 1;
    }
}

/**
 * @brief Cheap PRNG-style byte generator that mixes a lookup-table read
 *        with a slow-moving outer counter.
 *
 * Two-level counter design:
 *   - @c D_8005F151 is the inner counter, incremented by 1 each call;
 *     it wraps every 256 calls.
 *   - @c D_8005F150 is the outer counter, bumped by 13 only when the
 *     inner counter wraps to 0 (so it advances roughly once per 256 calls).
 *
 * The return is @c D_800C3520[D_8005F151] + @c D_8005F150 truncated to
 * a byte, the lookup-table byte mixed with the slow drift counter.
 *
 * Same @c D_800C3520 lookup table that @c func_800A2EA4 and
 * @c func_800A5CF8 use; this is one of several sibling helpers that
 * produce byte-scale pseudo-randomness for the field engine.
 */
s32 func_800A5C9C(void) {
    D_8005F151++;
    if (D_8005F151 == 0) {
        D_8005F150 += 13;
    }
    return (u8)(D_800C3520[D_8005F151] + D_8005F150);
}

/**
 * Increments the global byte D_8005F103 and returns the value at
 * D_800C3520[D_8005F103].
 *
 * @return The byte from the D_800C3520 lookup table.
 */
s32 func_800A5CF8(void) {

    D_8005F103++;
    return D_800C3520[D_8005F103];
}

/**
 * @brief Per-step random-encounter accumulator and battle trigger.
 *
 * Runs the classic FF8 field encounter formula. Bails when: the engine mode
 * is 1 or 7, @c func_800BE274() reports activity, @c g_fieldVars->fieldCF is
 * set, the dialog state is 2/3/4, encounters are disabled (@c g_fadeMode),
 * or movement flag bit 3 is set. Otherwise adds the field's step-rate byte
 * (halved when movement flag bit 2 is set) to the step accumulator
 * @c D_8005F164; when it passes 0x100 the accumulator wraps (@c &= 0xFF) and
 * the player entity's danger halfword (offset 0x1FE, read as unsigned)
 * divided by 1348 is added to the battle chance @c D_8005F0FE. A random
 * byte below the battle chance triggers an encounter: engine mode 3,
 * chance reset, @c D_8005F130 set, and a formation picked from the field's
 * 4-entry table with thresholds 0x80/0xC0/0xF0, preferring the first
 * bucket whose formation differs from the previous one (@c D_8005F120),
 * falling back to entry 3 unconditionally.
 *
 * @note The three dialog-state reads go through a volatile cast, the
 *       original re-reads the halfword for each compare.
 * @note The first formation store writes @c g_fieldEntity.counter through the
 *       struct; the others use the alias symbol @c D_800704AA (same word,
 *       0x800704AA), both spellings exist in the original.
 * @note The step accumulator advances by the player's @c moveSpeed (0x1FE)
 *       read through a @c (u16) view, so faster movement builds the encounter
 *       counter proportionally faster.
 */
void func_800A5D28(void) {
    u8 *rate;
    s32 r;
    u16 *fm;

    if (g_fieldEntity.mode == 1) {
        return;
    }
    if (g_fieldEntity.mode == 7) {
        return;
    }
    r = func_800BE274();
    if (r != 0) {
        return;
    }
    if (g_fieldVars->fieldCF != 0) {
        return;
    }
    if ((s16)*(volatile u16 *)&g_fieldEntity.dialogState == 4) {
        return;
    }
    if ((s16)*(volatile u16 *)&g_fieldEntity.dialogState == 3) {
        return;
    }
    if ((s16)*(volatile u16 *)&g_fieldEntity.dialogState == 2) {
        return;
    }
    if (g_fadeMode == 1) {
        return;
    }
    if (D_80078DF8 & 8) {
        return;
    }
    rate = *D_800C71F4;
    if (D_80078DF8 & 4) {
        D_8005F164 += *rate >> 1;
    } else {
        D_8005F164 += *rate;
    }
    if (D_8005F164 < 0x101) {
        return;
    }
    D_8005F164 &= 0xFF;
    D_8005F0FE += (s16)(u16)D_80085224[D_8005F148].moveSpeed / 1348;
    if ((u8)func_800A5C9C() < D_8005F0FE) {
        g_fieldEntity.mode = 3;
        D_8005F0FE = 0;
        D_8005F130 = 1;
        r = func_800A5CF8();
        fm = *D_800C720C;
        if ((u8)r < 0x80 && D_8005F120 != (s16)fm[0]) {
            g_fieldEntity.counter = fm[0];
        } else if ((u8)r < 0xC0 && D_8005F120 != (s16)fm[1]) {
            D_800704AA = fm[1];
        } else if ((u8)r < 0xF0 && D_8005F120 != (s16)fm[2]) {
            D_800704AA = fm[2];
        } else {
            D_800704AA = fm[3];
        }
        D_8005F120 = D_800704AA;
    }
}
