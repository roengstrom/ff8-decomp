#include "common.h"
#include "world.h"
#include "world/we_object6.h"
#include "psxsdk/libc.h"

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800ACC68);

/**
 * @brief Snapshot @c D_800D2390.tail with an angle-adjusted @c angle
 *        halfword, then forward to @c func_800ACC68.
 *
 * Steps:
 *  1. Compute @c delta = @c getAngleDelta(@c D_800C977A,
 *     @c D_800D2390.tail.angle) @c >> @c 1 — half the signed result.
 *  2. Snapshot @c D_800D2390.tail into a local @ref WorldXformBlock via
 *     @c memcpy. The source is cast to @c (u8 @c *) so the compiler
 *     emits @c lwl / @c lwr (matching the target's byte-granular 8-byte
 *     copy).
 *  3. Add @c delta to the @c angle of the local copy.
 *  4. Forward @c (arg, @c &buf, @c D_800C97F8, @c &D_800D2390) to
 *     @c func_800ACC68.
 *
 * @note Purpose uncertain — appears to apply an angle correction to a
 *       cached transform/rotation payload before dispatching world-render
 *       work.
 *
 * @param arg First arg forwarded verbatim to @c func_800ACC68.
 */
void func_800ACD38(s32 arg) {
    WorldXformBlock buf;
    s32 delta;

    delta = getAngleDelta(D_800C977A, D_800D2390.tail.angle) >> 1;

    memcpy(&buf, (u8 *)&D_800D2390.tail, sizeof(buf));

    buf.angle += delta;

    func_800ACC68(arg, &buf, (u8 *)D_800C97F8, &D_800D2390);
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800ACDC4);

void func_800AD688(void) {
}

void func_800AD690(void) {
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AD698);

/**
 * @brief Walk the placement script and emit slot entries for FF13/FF14 ops.
 *
 * Reads the script returned by @c func_800AF004 (base @c D_800D2288) and
 * dispatches each FF13/FF14 placement opcode to @c func_800BD82C, building
 * the rotation/translation pair from one of:
 *   - @c ctx track A (low byte 0x01)
 *   - @c ctx track B (low byte 0x40 / 0x41)
 *   - @c D_800D2128 entry indexed by the high byte (when high byte >= 0)
 *   - all-zero defaults (when high byte < 0)
 *
 * @param actor Opaque marker buffer with a 0x40-byte slot table at offset 6.
 * @param slot  First @c SlotEntry; advances by one entry per emitted slot.
 * @param ctx   Slot context holding two transform tracks at +0x18 and +0x24.
 * @return 1 if at least one slot was emitted, 0 otherwise.
 */
s32 func_800ADE24(u8 *actor, SlotEntry *slot, Slot *ctx) {
    SVECTOR rot;
    s32 didWork;
    VECTOR trans;
    ScriptOp *p;

    didWork = 0;
    p = func_800AF004(D_800D2288, 0);
    if (p == 0) return didWork;

    while (1) {
        u16 op = p->op;
        if (op == 0xFF05) break;

        if (op == 0xFF0E) {
            p = (ScriptOp *)(D_800D2288 + p->param);
            continue;
        }

        if (op == 0xFF13 || op == 0xFF14) {
            u16 paramVal = p->param;
            s32 lowByte = *(u8 *)&p->param;
            s32 highByte = paramVal >> 8;

            if (lowByte != 3 || func_800BEFC4() != 0) {
                if (lowByte == 1) {
                    rot.vz = 0;
                    rot.vx = 0;
                    rot.vy = ctx->tracks[0].rot_y;
                    trans.vx = ctx->tracks[0].trans_x;
                    trans.vy = ctx->tracks[0].trans_y;
                    trans.vz = -ctx->tracks[0].trans_z;
                    func_800BD82C(actor, slot, 1, 0, &rot, &trans);
                } else if (lowByte == 0x40) {
                    rot.vz = 0;
                    rot.vx = 0;
                    rot.vy = ctx->tracks[1].rot_y;
                    trans.vx = ctx->tracks[1].trans_x;
                    trans.vy = ctx->tracks[1].trans_y;
                    trans.vz = -ctx->tracks[1].trans_z;
                    func_800BD82C(actor, slot, 0x40, 0, &rot, &trans);
                } else if (lowByte == 0x41) {
                    rot.vz = 0;
                    rot.vx = 0;
                    rot.vy = ctx->tracks[1].rot_y;
                    trans.vx = ctx->tracks[1].trans_x;
                    trans.vy = ctx->tracks[1].trans_y;
                    trans.vz = -ctx->tracks[1].trans_z;
                    func_800BD82C(actor, slot, 0x41, 2, &rot, &trans);
                } else if ((s8)highByte >= 0) {
                    s32 rz;
                    trans = *(VECTOR *)&D_800D2128[(s8)highByte];
                    rot.vy = D_800D2128[(s8)highByte].rot_y;
                    rz = D_800D2128[(s8)highByte].rot_z;
                    rot.vz = rz;
                    rot.vx = 0;
                    func_800BD82C(actor, slot, lowByte, (lowByte == 0x50) ? 4 : 0, &rot, &trans);
                } else {
                    rot.vz = 0;
                    rot.vy = 0;
                    rot.vx = 0;
                    func_800BD82C(actor, slot, lowByte, 0, &rot, NULL);
                }

                slot++;
                D_800C5B50++;
                didWork = 1;
            }
        }

        p++;
    }

    return didWork;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AE0C8);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AE31C);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AE518);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AEB58);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AEEB0);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AF004);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800AF28C);

/**
 * @brief Inverse of @c func_800B00D8 — map a marker code back to raw kind.
 *
 * Accepts @p kind in @c [0, 0x84); returns the compact marker byte used
 * by surrounding slot code. Out-of-range or unmapped kinds return @c -1.
 */
s32 func_800B0010(u32 kind) {
    s32 out = -1;
    if (kind < 0x84) {
        switch (kind) {
            case 0x01: out = 4;    break;
            case 0x02: out = 5;    break;
            case 0x06: out = 6;    break;
            case 0x00:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x07:
            case 0x08:
            case 0x09: out = 0;    break;
            case 0x83: out = 0x46; break;
            case 0x22: out = 0x51; break;
            case 0x23: out = 0x52; break;
            case 0x24: out = 0x53; break;
            case 0x25: out = 0x54; break;
            case 0x26: out = 0x55; break;
            case 0x27: out = 0x56; break;
            case 0x28: out = 0x57; break;
            case 0x20: out = 0x4A; break;
            case 0x30: out = 0x40; break;
            case 0x31: out = 2;    break;
            case 0x32: out = 1;    break;
            case 0x41: out = 0x4D; break;
            case 0x42: out = 0x4E; break;
            case 0x21: out = 0x49; break;
        }
    }
    return out;
}

/**
 * @brief Remap a raw slot-kind byte into a compact marker code.
 *
 * Lookup helper used by world-engine slot classifiers. Accepts an 8-bit
 * @p kind; returns @c -1 for values >= @c 0x58 or for cases left blank in
 * the table. Covered cases (partial): menu items @c 0x00-0x06, level
 * markers @c 0x40/0x41, string groups @c 0x49-0x4E, and the @c 0x51-0x57
 * command row.
 */
s32 func_800B00D8(u32 kind) {
    s32 out = -1;
    if (kind < 0x58) {
        switch (kind) {
            case 0x00: out = 0;    break;
            case 0x04: out = 1;    break;
            case 0x05: out = 2;    break;
            case 0x06: out = 6;    break;
            case 0x46: out = 0x83; break;
            case 0x51: out = 0x22; break;
            case 0x52: out = 0x23; break;
            case 0x53: out = 0x24; break;
            case 0x54: out = 0x25; break;
            case 0x55: out = 0x26; break;
            case 0x56: out = 0x27; break;
            case 0x57: out = 0x28; break;
            case 0x4A:
            case 0x4B: out = 0x20; break;
            case 0x40:
            case 0x41: out = 0x30; break;
            case 0x02:
            case 0x03: out = 0x31; break;
            case 0x01: out = 0x32; break;
            case 0x4D: out = 0x41; break;
            case 0x4E: out = 0x42; break;
            case 0x49: out = 0x21; break;
        }
    }
    return out;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B01A0);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B04CC);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B0EAC);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B1174);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B13B8);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B164C);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B18B8);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B1BCC);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B1FD0);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B20E4);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B21EC);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B28C8);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B2B6C);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B2D94);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B2F5C);
