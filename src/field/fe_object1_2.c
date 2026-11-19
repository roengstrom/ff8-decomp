#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "battle.h"
#include "main.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "field/fe_object1.h"
#include "field/fe_object1_2.h"
#include "field/fe_object1b.h"
#include "field/fe_object10.h"
#include "imgtoc.h"

/**
 * @brief Arm every camera slot whose move has not started yet.
 *
 * Scans the eight @c g_fieldContext.slots for ones still in @c submode @c 0 and
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
        if (g_fieldContext.slots[i].submode == 0) {
            g_fieldContext.slots[i].unk06 = 0;
            g_fieldContext.slots[i].savedQ1 = g_fieldContext.slots[i].q1;
            g_fieldContext.slots[i].savedQ2 = g_fieldContext.slots[i].q2;
            switch (g_fieldContext.slots[i].mode) {
                case 0:
                case 1:
                case 2:
                case 4:
                case 5:
                    g_fieldContext.slots[i].submode = 1;
                    break;
                case 3:
                    g_fieldContext.slots[i].submode = 2;
                    g_fieldContext.slots[i].q1 = g_fieldContext.slots[i].p1;
                    g_fieldContext.slots[i].q2 = g_fieldContext.slots[i].p2;
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
 * Reads @c g_fieldContext.slots[unk1A6].param to pick an @ref Actor entity,
 * fills @c svec.{vx,vy,vz} from its @c posX/posY/posZ shifted right by
 * @c 12, biasing @c vz by @c g_curFieldInfo->baseZ. The projection result is
 * latched to @c D_800C71FC. The trailing @c func_800A0FB8 clamp call gets
 * flag @c 0 when @c unk1A6 @c == @c 0 and flag @c 1 otherwise.
 *
 * @param out     Screen-space position, written by @c func_800A0F34 and then
 *                clamped in place by @c func_800A0FB8.
 * @param slotIdx Caller's slot index. Unused — the entity is picked from
 *                @c g_fieldContext.unk1A6 instead — but @c func_800A1318 passes it.
 */
void func_800A11E0(Vec2s *out, s16 slotIdx) {
    SVECTOR svec;

    svec.vx = g_fieldActors[g_fieldContext.slots[g_fieldContext.unk1A6].param].posX >> 12;
    svec.vy = g_fieldActors[g_fieldContext.slots[g_fieldContext.unk1A6].param].posY >> 12;
    svec.vz = (g_fieldActors[g_fieldContext.slots[g_fieldContext.unk1A6].param].posZ >> 12) +
              g_curFieldInfo->baseZ;
    D_800C71FC = func_800A0F34(&svec, (s32 *)out);
    if (g_fieldContext.unk1A6 == 0) {
        func_800A0FB8(out, 0, 0);
    } else {
        func_800A0FB8(out, 1, 0);
    }
}

/**
 * @brief Advance every camera slot that has a move in progress.
 *
 * Walks the eight @c g_fieldContext.slots and ticks each one whose @c submode is
 * @c 1 (move running). @c mode picks how the slot's camera position
 * @c q1 / @c q2 — the scroll origin @c func_800A15C0 later turns into the
 * frame's draw offset — is driven towards its target:
 *
 *  - @c 0: hard-lock to the party entity's projected screen position, but only
 *    for the active slot (@c g_fieldContext.unk1A6); every other slot is zeroed.
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
        if (g_fieldContext.slots[i].submode != 1) {
            continue;
        }
        switch (g_fieldContext.slots[i].mode) {
        case 0:
            func_800A11E0(&pos, i);
            if (i == g_fieldContext.unk1A6) {
                g_fieldContext.slots[i].q1 = pos.x;
                g_fieldContext.slots[i].q2 = pos.y;
            } else {
                g_fieldContext.slots[i].q1 = 0;
                g_fieldContext.slots[i].q2 = 0;
            }
            break;
        case 1:
            func_800A11E0(&pos, i);
            g_fieldContext.slots[i].q1 = func_800A0E54(g_fieldContext.slots[i].savedQ1, pos.x,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            g_fieldContext.slots[i].q2 = func_800A0E54(g_fieldContext.slots[i].savedQ2, pos.y,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            if (g_fieldContext.slots[i].timer == g_fieldContext.slots[i].unk06) {
                g_fieldContext.slots[i].submode = 2;
            } else {
                g_fieldContext.slots[i].unk06++;
            }
            break;
        case 2:
            func_800A11E0(&pos, i);
            g_fieldContext.slots[i].q1 = func_800A0EB8(g_fieldContext.slots[i].savedQ1, pos.x,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            g_fieldContext.slots[i].q2 = func_800A0EB8(g_fieldContext.slots[i].savedQ2, pos.y,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            if (g_fieldContext.slots[i].timer == g_fieldContext.slots[i].unk06) {
                g_fieldContext.slots[i].submode = 2;
            } else {
                g_fieldContext.slots[i].unk06++;
            }
            break;
        case 3:
            break;
        case 4:
            g_fieldContext.slots[i].q1 = func_800A0E54(g_fieldContext.slots[i].savedQ1, g_fieldContext.slots[i].p1,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            g_fieldContext.slots[i].q2 = func_800A0E54(g_fieldContext.slots[i].savedQ2, g_fieldContext.slots[i].p2,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            if (g_fieldContext.slots[i].timer == g_fieldContext.slots[i].unk06) {
                g_fieldContext.slots[i].submode = 2;
            } else {
                g_fieldContext.slots[i].unk06++;
            }
            break;
        case 5:
            g_fieldContext.slots[i].q1 = func_800A0EB8(g_fieldContext.slots[i].savedQ1, g_fieldContext.slots[i].p1,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            g_fieldContext.slots[i].q2 = func_800A0EB8(g_fieldContext.slots[i].savedQ2, g_fieldContext.slots[i].p2,
                                                   g_fieldContext.slots[i].timer, g_fieldContext.slots[i].unk06);
            if (g_fieldContext.slots[i].timer == g_fieldContext.slots[i].unk06) {
                g_fieldContext.slots[i].submode = 2;
            } else {
                g_fieldContext.slots[i].unk06++;
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
            env[0].ofs[0] = (D_800C7210 - g_fieldContext.slots[slotIdx].q1) + g_fieldContext.oscillators[0].output + g_curFieldView->viewOfsX;
            env[0].ofs[1] = (D_800C7214 - g_fieldContext.slots[slotIdx].q2) + g_fieldContext.oscillators[1].output + g_curFieldView->viewOfsY;
        } else {
            env[1].ofs[0] = (D_800C7210 - g_fieldContext.slots[slotIdx].q1) + g_fieldContext.oscillators[0].output + g_curFieldView->viewOfsX + 0x200;
            env[1].ofs[1] = (D_800C7214 - g_fieldContext.slots[slotIdx].q2) + g_fieldContext.oscillators[1].output + g_curFieldView->viewOfsY;
        }
    } else {
        if (buf == D_800C7218) {
            env[0].ofs[0] = D_800C7210 - g_curFieldView->viewOfsX;
            env[0].ofs[1] = D_800C7214 + g_curFieldView->viewOfsY;
        } else {
            env[1].ofs[0] = (D_800C7210 - g_curFieldView->viewOfsX) + 0x200;
            env[1].ofs[1] = D_800C7214 + g_curFieldView->viewOfsY;
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
