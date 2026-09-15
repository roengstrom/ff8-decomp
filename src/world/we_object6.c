#include "common.h"
#include "world.h"
#include "world/we_object6.h"
#include "psxsdk/libc.h"

extern void func_800B7240(s32 arg0);
extern s32  func_800BE5F8(void *rec);
extern void func_800BC51C(VECTOR *src, VECTOR *dst);

/* Declared in we_object9.h; avoid pulling that header (conflicts with local decls). */
extern void func_800BC544(VECTOR *src, VECTOR *dst);

extern void func_80041274(SVECTOR *angles, MATRIX *m);
extern void func_80040534(MATRIX *m, VECTOR *t);
extern void func_80040E74(SVECTOR *v, VECTOR *out, s32 *flag);
extern void func_80040FA4(MATRIX *src, MATRIX *dst);

/**
 * @brief Build a negated-translation world matrix from angles / rot / offset.
 *
 * Rotates @p angles into a scratch matrix, applies @p rotBuf as translation,
 * transforms @p offset through that matrix, copies the result into @p outMat,
 * then negates all three translation components.
 */
void func_800ACC68(MATRIX *outMat, SVECTOR *angles, SVECTOR *rotBuf, SVECTOR *offset) {
    MATRIX m;
    VECTOR t;
    s32 flag;

    func_80041274(angles, &m);
    SetRotMatrix(&m);
    t.vx = rotBuf->vx;
    t.vy = rotBuf->vy;
    t.vz = rotBuf->vz;
    func_80040534(&m, &t);
    SetTransMatrix(&m);
    func_80040E74(offset, (VECTOR *)m.t, &flag);
    func_80040FA4(&m, outMat);
    func_8003FD84(outMat, (VECTOR *)m.t, (VECTOR *)outMat->t);
    {
        register s32 a __asm__("v0");
        register s32 b __asm__("v1");

        a = outMat->t[0];
        b = outMat->t[2];
        a = -a;
        outMat->t[0] = a;
        a = outMat->t[1];
        b = -b;
        outMat->t[2] = b;
        a = -a;
        outMat->t[1] = a;
    }
}


/**
 * @brief Snapshot @c D_800D2390.tail with an angle-adjusted @c angle
 *        halfword, then forward to @c func_800ACC68.
 *
 * Steps:
 *  1. Compute @c delta = @c getAngleDelta(@c D_800C977A,
 *     @c D_800D2390.tail.vy) @c >> @c 1 — half the signed result.
 *  2. Snapshot @c D_800D2390.tail into a local SVECTOR via
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
 * @param outMat Destination matrix, forwarded to @c func_800ACC68.
 */
void func_800ACD38(MATRIX *outMat) {
    SVECTOR buf;
    s32 delta;

    delta = getAngleDelta(D_800C977A, D_800D2390.tail.vy) >> 1;

    memcpy(&buf, (u8 *)&D_800D2390.tail, sizeof(buf));

    buf.vy += delta;

    func_800ACC68(outMat, &buf, D_800C97F8, &D_800D2390.head);
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

/**
 * @brief Sync active named slots from camera @p pos / @p ang into
 *        @c D_800DBFB8.
 *
 * For each slot index (@c D_800C5C18 .. @c D_800C5C30) that is live
 * (>= 0) and whose world-dispatch gate matches, calls
 * @c func_800BC544(@p pos, &slot->position) then copies @p ang[1]
 * into @c slot->vec via an unaligned 8-byte memcpy (matching the
 * target's @c lwl/@c lwr/@c swl/@c swr pair).
 *
 * Gates:
 *  - @c D_800C5C18: always (when live)
 *  - @c D_800C5C1C: @c D_800C4D38 == 0x31 and @c D_800C5BFC == 0
 *  - @c D_800C5C20: @c D_800C5BFC == 0
 *  - @c D_800C5C28: @c D_800C4D38 in 0x20..0x28 or == 0x84
 *  - @c D_800C5C24: @c D_800C4D38 == 0x32
 *  - @c D_800C5C2C / @c D_800C5C30: @c D_800C4D38 == 0x30
 *
 * @param flags Unused (callers pass @c D_800D23D8).
 * @param pos   Source camera world position.
 * @param ang   Camera scratch (@c D_800C9770); rotation at @c ang[1].
 */
void func_800B18B8(u8 *flags, VECTOR *pos, SVECTOR *ang) {
    SlotEntry *slot;

    if (D_800C5C18 >= 0) {
        slot = &D_800DBFB8[D_800C5C18];
        func_800BC544(pos, &slot->position);
        memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
    }
    if (D_800C5C1C >= 0) {
        if ((D_800C4D38 == 0x31) && (D_800C5BFC == 0)) {
            slot = &D_800DBFB8[D_800C5C1C];
            func_800BC544(pos, &slot->position);
            memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
        }
    }
    if (D_800C5C20 >= 0) {
        if (D_800C5BFC == 0) {
            slot = &D_800DBFB8[D_800C5C20];
            func_800BC544(pos, &slot->position);
            memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
        }
    }
    if (D_800C5C28 >= 0) {
        if (((u32)(D_800C4D38 - 0x20) < 9U) || (D_800C4D38 == 0x84)) {
            slot = &D_800DBFB8[D_800C5C28];
            func_800BC544(pos, &slot->position);
            memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
        }
    }
    if (D_800C5C24 >= 0) {
        if (D_800C4D38 == 0x32) {
            slot = &D_800DBFB8[D_800C5C24];
            func_800BC544(pos, &slot->position);
            memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
        }
    }
    if (D_800C5C2C >= 0) {
        if (D_800C4D38 == 0x30) {
            slot = &D_800DBFB8[D_800C5C2C];
            func_800BC544(pos, &slot->position);
            memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
        }
    }
    if ((D_800C5C30 >= 0) && (D_800C4D38 == 0x30)) {
        slot = &D_800DBFB8[D_800C5C30];
        func_800BC544(pos, &slot->position);
        memcpy((u8 *)&slot->vec, (u8 *)&ang[1], sizeof(SVECTOR));
    }
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B1BCC);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B1FD0);

/**
 * @brief Arm actor @p actorIdx: store @p flag, set dispatch code, seed camera.
 *
 * Writes @p flag through @p out, sets @c D_800C4D38 to @c 0x15 when
 * @p mode == 2 else @c 0x10, forces @c D_800C4D2C = 1, and copies the
 * actor's rotation-source @c +0x68 vector into @c D_800C9868 via
 * @c func_800BC51C.
 *
 * Then dispatches on @p mode:
 *  - @c 1: @c func_800B7240(0x41) and clear @c D_800C987C.
 *  - @c 0: @c func_800B7240(0x40/0x41) by @c flag1E, then refresh
 *    @c D_800C987C from @c func_800BE5F8(@c D_800C9878).
 *  - @c 2: @c func_800B7240(0x42/0x43) by @c flag1E == -1, then the same
 *    @c func_800BE5F8 refresh.
 */
void func_800B20E4(s8 *out, s32 actorIdx, s8 flag, s32 mode) {
    ActorRecord *rec;
    ActorRecord *base;
    s32 code;
    s32 script;
    s32 idx;
    s16 refreshed;

    idx = actorIdx;
    code = 0x10;
    *out = flag;
    if (mode == 2) {
        code = 0x15;
    }
    D_800C4D38 = code;
    D_800C4D2C = 1;
    base = D_800DD6A8;
    rec = &base[idx];
    func_800BC51C((VECTOR *)((u8 *)rec->rotSrc + 0x68), &D_800C9868);
    if (mode == 1) {
        func_800B7240(0x41);
        D_800C987C = 0;
        return;
    }
    if (mode == 0) {
        script = 0x41;
        if (rec->flag1E != -1) {
            script = 0x40;
        }
        goto shared;
    }
    if (mode == 2) {
        script = 0x42;
        if (rec->flag1E == -1) {
            script = 0x43;
        }
    shared:
        func_800B7240(script);
        refreshed = func_800BE5F8((void *)D_800C9878);
        D_800C987C = refreshed;
    }
}


INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B21EC);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B28C8);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B2B6C);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B2D94);

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object6", func_800B2F5C);
