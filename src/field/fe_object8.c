#include "common.h"
#include "field.h"
#include "gamestate.h"
#include "field/fe_object1.h"
#include "field/fe_object8.h"


/**
 * Clear bits @c 0x180000 and set bit @c 0x200000 in @c flags, then call
 * @c func_800A97E4(spatialIndex, 0x2F, 0, 0).
 *
 * @param actor Pointer to the Actor event-script context.
 * @return 2 (advance PC).
 */
s32 opHandler_CLOSEEYES(Actor *actor) {
    actor->context.flags = (actor->context.flags & 0xFFE7FFFF) | 0x200000;
    func_800A97E4(actor->field_0x256, 0x2F, 0, 0);
    return 2;
}

/**
 * Update entity flags by clearing bits @c 0x300000 and setting bit
 * @c 0x80000. Then pop one byte from the script stack and write it to
 * the byte at offset @c 0x263 of the entity. Finally call
 * @c func_800A97E4 with the entity's @c spatialIndex (offset 0x256),
 * opcode @c 0x27, @c 0, and the popped byte.
 *
 * @param actor Pointer to the Actor event-script context.
 * @return 2 (advance PC).
 */
s32 opHandler_BLINKEYES(Actor *actor) {
    u8 byte;

    actor->context.flags = (actor->context.flags & 0xFFCFFFFF) | 0x80000;
    byte = POP_BYTE(&actor->context);
    actor->field_0x263 = byte;
    func_800A97E4(actor->field_0x256, 0x27, 0, actor->field_0x263);
    return 2;
}

/**
 * @brief Dispatch motion command 0xD with the given byte arg.
 *
 * Sends command @c 0xD to the entity at @c field_0x256 (via the global
 * cmd table @c func_800AA46C) carrying the sign-extended low byte of
 * @p a1, mirrors @p a1 into @c field_0x24E, zeroes the live motion
 * halfwords (@c field_0x206/20A), snapshots the render slot's
 * @c unk0C into @c field_0x20C, clears the same render slot's
 * @c unk52, and clears the @c 0xF800 flag band. Called by the RANIME
 * family handlers in this file.
 */
void func_800B912C(Actor *actor, s16 a1) {
    func_800AA46C(actor->field_0x256, 0xD, a1, 0);
    actor->field_0x24E = a1;
    actor->field_0x206 = 0;
    actor->field_0x20A = 0;
    actor->field_0x20C = D_800D9630[actor->field_0x256]->unk0C;
    D_800D9630[actor->field_0x256]->unk52 = actor->field_0x206;
    actor->context.flags &= ~0xF800;
}

/**
 * @brief CANIME-family helper — queue curved animation + dispatch cmd 0xD.
 *
 * Asserts the active entity in @c D_800DE4FC matches @c actor->field_0x256
 * (infinite-loops on mismatch). Encodes the popped halfwords @p a2 and
 * @p a3 as @c (n-1)<<4 into @c field_0x20A and @c field_0x20C, mirrors
 * the byte arg into @c field_0x24E, copies @c field_0x20A into the live
 * motion halfword @c field_0x206 and into the render slot's @c unk52,
 * then dispatches motion command @c 0xD with byte arg @p a1 via
 * @c func_800AA46C and clears flag band @c 0xF800.
 */
void func_800B91D8(Actor *actor, s32 a1, s32 a2, s32 a3) {
    if (D_800DE4FC != actor->field_0x256) {
        while (1) {}
    }
    actor->field_0x20A = (a2 - 1) << 4;
    actor->field_0x20C = (a3 - 1) << 4;
    actor->field_0x24E = a1;
    actor->field_0x206 = actor->field_0x20A;
    D_800D9630[actor->field_0x256]->unk52 = actor->field_0x20A;
    func_800AA46C(actor->field_0x256, 0xD, a1, 0);
    actor->context.flags &= ~0xF800;
}

/**
 * @brief Per-frame motion tick for the entity.
 *
 * Advances @c field_0x206 (current position) by @c field_0x208 (step)
 * each frame. When @c flags & 0x80 is set and the entity is in
 * @c msgActive == 1, fires @c func_800B2864 hooks if the half-range
 * boundary or zero-crossing on @c (val - 0x80) flips during the step.
 *
 * When @c field_0x206 reaches @c field_0x20C, dispatches based on
 * flag bits 0x2000 (loop-rewind), 0x8000 (loop-step-back), or
 * 0x4000 (one-shot via @c func_800B912C), and sets the completion
 * flag 0x800. Otherwise clears flag 0x800. Always writes
 * @c field_0x206 into the render slot's @c unk52 at the end (unless
 * the early @c 0x4 disable bit was set on entry).
 */
void func_800B9288(Actor *actor) {
    s32 flags;
    s32 newPos;
    s32 oldSigned;
    s32 oldMid;
    s32 newMid;
    s32 halfRange;
    s32 s1_v;
    s32 s2_v;

    flags = actor->context.flags;
    if (flags & 0x4) {
        return;
    }
    if (!(flags & 0x1000)) {
        u16 oldUnsigned = actor->field_0x206;
        oldSigned = (s16)actor->field_0x206;
        newPos = oldUnsigned;
        newPos = newPos + (u16)actor->field_0x208;
        actor->field_0x206 = newPos;
        if ((actor->context.flags & 0x80) && actor->msgActive == 1) {
            newMid = (s16)newPos - 0x80;
            oldMid = 0x80;
            oldMid = oldSigned - oldMid;
            halfRange = ((s16)actor->field_0x20C - (s16)actor->field_0x208) >> 1;
            s2_v = newMid - halfRange;
            s1_v = oldMid - halfRange;
            if ((((u32)newMid >> 31) & ((u32)~oldMid >> 31))
                || (((u32)oldMid >> 31) & ((u32)~newMid >> 31))) {
                func_800B2864(actor, 1, 0x40, 0x80);
            }
            if ((((u32)s2_v >> 31) & ((u32)~s1_v >> 31))
                || (((u32)s1_v >> 31) & ((u32)~s2_v >> 31))) {
                func_800B2864(actor, 0, 0x40, 0x80);
            }
        }
    }
    if ((s16)actor->field_0x206 >= (s16)actor->field_0x20C) {
        flags = actor->context.flags;
        if (flags & 0x2000) {
            actor->field_0x206 = actor->field_0x20A;
        } else if (flags & 0x8000) {
            actor->context.flags = (flags & ~0xF800) | 0x1000;
            actor->field_0x20C -= actor->field_0x208;
            actor->field_0x206 = actor->field_0x20C;
        } else if (flags & 0x4000) {
            func_800B912C(actor, actor->field_0x24F);
            actor->context.flags = (actor->context.flags & ~0xF800) | 0x2000;
        }
        actor->context.flags |= 0x800;
    } else {
        actor->context.flags &= ~0x800;
    }
    D_800D9630[actor->field_0x256]->unk52 = actor->field_0x206;
}

/**
 * @brief Snapshot live animation state into the 0x210-0x216 backup slots.
 *
 * Copies the low halfword of @c flags into @c field_0x20E, mirrors
 * @c field_0x24E into @c field_0x24D, and saves the four halfwords
 * @c field_0x206/208/20A/20C into @c field_0x210/212/214/216.
 */
s32 opHandler_PUSHANIME(Actor *actor) {
    actor->field_0x20E = actor->context.flags;
    actor->field_0x24D = actor->field_0x24E;
    actor->field_0x210 = actor->field_0x206;
    actor->field_0x212 = actor->field_0x208;
    actor->field_0x214 = actor->field_0x20A;
    actor->field_0x216 = actor->field_0x20C;
    return 2;
}

/**
 * @brief Restore animation state from the 0x210-0x216 backup slots.
 *
 * Inverse of @c opHandler_PUSHANIME: copies the saved halfwords back into
 * the live motion state (@c field_0x206/208/20A/20C), mirrors
 * @c field_0x24D into @c field_0x24E, dispatches motion command
 * @c 0xD via @c func_800AA46C, copies @c field_0x206 into the render
 * slot's @c unk52, and restores the saved 0xF800 flag band from
 * @c field_0x20E.
 */
s32 opHandler_POPANIME(Actor *actor) {
    actor->field_0x24E = actor->field_0x24D;
    actor->field_0x206 = actor->field_0x210;
    actor->field_0x208 = actor->field_0x212;
    actor->field_0x20A = actor->field_0x214;
    actor->field_0x20C = actor->field_0x216;
    func_800AA46C(actor->field_0x256, 0xD, actor->field_0x24E, 0);
    D_800D9630[actor->field_0x256]->unk52 = actor->field_0x206;
    actor->context.flags &= ~0xF800;
    actor->field_0x20E &= 0xF800;
    actor->context.flags |= actor->field_0x20E;
    return 2;
}

/**
 * @brief Pop the top stack slot as a halfword into @c field_0x208.
 */
s32 opHandler_ANIMESPEED(Actor *actor) {
    actor->field_0x208 = POP(&actor->context);
    return 2;
}

/**
 * @brief Returns 2 if the animation-complete flag (0x800) is set, else 1.
 */
s32 opHandler_ANIMESYNC(ScriptContext *context) {
    if (context->flags & 0x800) {
        return 2;
    }
    return 1;
}

/**
 * Call @c func_800B912C with the entity's byte at offset @c 0x24F as
 * the second arg, then set bit @c 0x2000 in @c flags. Returns @c 3.
 *
 * @param actor Pointer to the Actor event-script context.
 * @return 3.
 */
s32 opHandler_ANIMESTOP(Actor *actor) {
    func_800B912C(actor, actor->field_0x24F);
    actor->context.flags |= 0x2000;
    return 3;
}

/**
 * @brief ANIME opcode 0x02D handler — start animation and wait for completion.
 *
 * While the entity's @c activeMask bit is set: call @c func_800B912C with
 * the sign-extended @p a1, set the animation-active flag (0x4000), and
 * return 1 (yield, retry next frame). On subsequent frames, the entity
 * is no longer active for this script slot; return 3 once flag 0x800
 * (animation complete) is set, else keep yielding with return 1.
 */
s32 opHandler_ANIME(Actor *actor, s32 a1) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        func_800B912C(actor, (s16)a1);
        actor->context.flags |= 0x4000;
    } else if (actor->context.flags & 0x800) {
        return 3;
    }
    return 1;
}

/**
 * @brief ANIMEKEEP opcode 0x02E handler — start animation (keep variant).
 *
 * Same shape as @c opHandler_ANIME (ANIME) but sets flag bit @c 0x8000
 * instead of @c 0x4000 to preserve the final frame after completion.
 */
s32 opHandler_ANIMEKEEP(Actor *actor, s32 a1) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        func_800B912C(actor, (s16)a1);
        actor->context.flags |= 0x8000;
    } else if (actor->context.flags & 0x800) {
        return 3;
    }
    return 1;
}

/**
 * @brief CANIME opcode 0x02F handler — start curved animation, wait for completion.
 *
 * Blocking variant of RCANIME. While the entity's @c activeMask bit is
 * set: pop two signed halfwords from the stack, dispatch via
 * @c func_800B91D8 with the bytecode arg, set the animation-active
 * flag (0x4000), and yield. Once the script slot bit clears, check
 * flag 0x800 (animation complete) to decide between return 3 and
 * return 1.
 */
s32 opHandler_CANIME(Actor *actor, s32 a1) {
    s32 v2;
    s32 v1;
    s32 tmp;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        v2 = POP_HALF(&actor->context);
        v1 = POP_HALF(&actor->context);
        func_800B91D8(actor, a1, v2, v1);
        tmp = actor->context.flags | 0x4000;
        actor->context.flags = tmp;
    } else if (actor->context.flags & 0x800) {
        return 3;
    }
    return 1;
}

/**
 * @brief CANIMEKEEP opcode 0x030 handler — same as CANIME but flag 0x8000.
 */
s32 opHandler_CANIMEKEEP(Actor *actor, s32 a1) {
    s32 v2;
    s32 v1;
    s32 tmp;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        v2 = POP_HALF(&actor->context);
        v1 = POP_HALF(&actor->context);
        func_800B91D8(actor, a1, v2, v1);
        tmp = actor->context.flags | 0x8000;
        actor->context.flags = tmp;
    } else if (actor->context.flags & 0x800) {
        return 3;
    }
    return 1;
}

/**
 * Sign-extend @p a1 to @c s16 and call @c func_800B912C, then set bit
 * @c 0x4000 in @c flags. Returns @c 3.
 */
s32 opHandler_RANIME(Actor *actor, s32 a1) {
    func_800B912C(actor, (s16)a1);
    actor->context.flags |= 0x4000;
    return 3;
}

/**
 * Sign-extend @p a1 to @c s16 and call @c func_800B912C, then set bit
 * @c 0x8000 in @c flags. Returns @c 3.
 */
s32 opHandler_RANIMEKEEP(Actor *actor, s32 a1) {
    func_800B912C(actor, (s16)a1);
    actor->context.flags |= 0x8000;
    return 3;
}

/**
 * @brief RCANIME opcode 0x033 handler — fire curved animation, return 3.
 *
 * Non-blocking variant of CANIME: pops two signed halfwords from the
 * bytecode stack (top → @c v2, next → @c v1), calls @c func_800B91D8
 * with them and the bytecode arg, sets animation-active flag (0x4000),
 * then returns 3 to advance the script PC immediately.
 */
s32 opHandler_RCANIME(Actor *actor, s32 a1) {
    s32 v2;
    s32 v1;
    s32 tmp;

    v2 = POP_HALF(&actor->context);
    v1 = POP_HALF(&actor->context);
    func_800B91D8(actor, a1, v2, v1);
    tmp = actor->context.flags | 0x4000;
    actor->context.flags = tmp;
    return 3;
}

/**
 * @brief RCANIMEKEEP opcode 0x034 handler — like RCANIME but sets flag 0x8000.
 */
s32 opHandler_RCANIMEKEEP(Actor *actor, s32 a1) {
    s32 v2;
    s32 v1;
    s32 tmp;

    v2 = POP_HALF(&actor->context);
    v1 = POP_HALF(&actor->context);
    func_800B91D8(actor, a1, v2, v1);
    tmp = actor->context.flags | 0x8000;
    actor->context.flags = tmp;
    return 3;
}

/**
 * Sign-extend @p a1 to @c s16 and call @c func_800B912C, then set bit
 * @c 0x2000 in @c flags. Returns @c 3.
 */
s32 opHandler_RANIMELOOP(Actor *actor, s32 a1) {
    func_800B912C(actor, (s16)a1);
    actor->context.flags |= 0x2000;
    return 3;
}

/**
 * @brief RCANIMELOOP opcode 0x036 handler — like RCANIME but sets flag 0x2000.
 */
s32 opHandler_RCANIMELOOP(Actor *actor, s32 a1) {
    s32 v2;
    s32 v1;
    s32 tmp;

    v2 = POP_HALF(&actor->context);
    v1 = POP_HALF(&actor->context);
    func_800B91D8(actor, a1, v2, v1);
    tmp = actor->context.flags | 0x2000;
    actor->context.flags = tmp;
    return 3;
}

/**
 * @brief Pop 3 bytes into the 0x18A vector slot and dispatch cmd 0x10.
 *
 * Pops three bytes from the script stack — top goes to @c actor->unk18A
 * byte+2 (= offset 0x18C), then byte+1 (= 0x18B), then byte+0 (= 0x18A).
 * If @c D_800DE8CC bit @c 0x2 is clear, dispatches command @c 0x10 to
 * the entity at @c field_0x256 with the pointer to the 3-byte vector.
 */
s32 opHandler_POLYCOLOR(Actor *actor) {
    ((u8 *)&actor->unk18A)[2] = POP_BYTE(&actor->context);
    ((u8 *)&actor->unk18A)[1] = POP_BYTE(&actor->context);
    ((u8 *)&actor->unk18A)[0] = POP_BYTE(&actor->context);
    if (!(D_800DE8C8[1] & 0x2)) {
        func_800A97E4(actor->field_0x256, 0x10, (s32)&actor->unk18A, 0);
    }
    return 2;
}

/**
 * @brief Broadcast 3 popped bytes into the unk18A vector of every active entity.
 *
 * Pops three bytes from the script stack into a local buffer, then
 * walks @c D_80085224[0..D_80085388-1] writing the same triple into
 * each entry's @c unk18A/18B/18C slot. Each iteration where
 * @c D_800DE8CC & 0x2 is clear also dispatches command @c 0x10 to
 * the entity (with the local buffer as the arg).
 */
s32 opHandler_POLYCOLORALL(ScriptContext *context) {
    u8 bytes[3];
    s32 i;
    Actor *p;

    bytes[2] = POP_BYTE(context);
    bytes[1] = POP_BYTE(context);
    bytes[0] = POP_BYTE(context);

    p = D_80085224;
    for (i = 0; i < D_80085388; i++) {
        if (!(D_800DE8C8[1] & 0x2)) {
            func_800A97E4(i, 0x10, (s32)bytes, 0);
        }
        ((u8 *)&p->unk18A)[0] = bytes[0];
        ((u8 *)&p->unk18A)[1] = bytes[1];
        ((u8 *)&p->unk18A)[2] = bytes[2];
        p++;
    }
    return 2;
}

/**
 * @brief Pop a byte into @c field_0x257 and mirror to the active render slot.
 *
 * Pops one byte from the script stack, stores it into
 * @c actor->field_0x257. If @c D_800DE8CC bit @c 0x2 is clear,
 * also stores the same byte into the active entity's render slot
 * (@c D_800D9630[D_800DE4FC]->unk61).
 */
s32 opHandler_SETGETA(Actor *actor) {
    u8 byte = POP_BYTE(&actor->context);
    actor->field_0x257 = byte;
    if (!(D_800DE8CC & 0x2)) {
        D_800D9630[D_800DE4FC]->unk61 = byte;
    }
    return 2;
}

/**
 * Pop one halfword from the script stack and store it into
 * @c actor->field_0x220. If @c D_800DE8CC bit @c 0x2 is clear, also
 * store the same halfword into the entity's render slot at
 * @c D_800D9630[actor->field_0x256]->unk62.
 *
 * @param actor Pointer to the Actor event-script context.
 * @return 2 (advance PC).
 */
s32 opHandler_SETROOTTRANS(Actor *actor) {
    u16 half = POP(&actor->context);
    *(volatile u16 *)&actor->field_0x220 = half;
    if (!(D_800DE8CC & 0x2)) {
        D_800D9630[actor->field_0x256]->unk62 = half;
    }
    return 2;
}

/**
 * Pop value, divide by 4 (signed, round toward zero), store to 8 entity bytes.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_SHADESET(Actor *actor) {
    s32 val = POP(&actor->context) / 4;
    actor->shadowRadius[3] = val;
    actor->shadowRadius[2] = val;
    actor->shadowRadius[1] = val;
    actor->shadowRadius[0] = val;
    actor->shadowRadius[7] = val;
    actor->shadowRadius[6] = val;
    actor->shadowRadius[5] = val;
    actor->shadowRadius[4] = val;
    return 2;
}

/**
 * Pop 8 values from the script stack, divide each by 4 (signed,
 * round-toward-zero), and store them as bytes into the entity's
 * direction-table fields @c shadowRadius[0]..shadowRadius[7].
 *
 * Store order: @c 0x25C, @c 0x25B, @c 0x25A, @c 0x259, @c 0x260,
 * @c 0x25F, @c 0x25E, @c 0x25D — the 8-entry vertical strip is
 * filled outward from the centre.
 *
 * @param actor Pointer to the Actor event-script context.
 * @return 2 (advance PC).
 */
s32 opHandler_SHADEFORM(Actor *actor) {
    actor->shadowRadius[3] = POP(&actor->context) / 4;
    actor->shadowRadius[2] = POP(&actor->context) / 4;
    actor->shadowRadius[1] = POP(&actor->context) / 4;
    actor->shadowRadius[0] = POP(&actor->context) / 4;
    actor->shadowRadius[7] = POP(&actor->context) / 4;
    actor->shadowRadius[6] = POP(&actor->context) / 4;
    actor->shadowRadius[5] = POP(&actor->context) / 4;
    actor->shadowRadius[4] = POP(&actor->context) / 4;
    return 2;
}

/**
 * @brief Pop a byte from the script stack into @c shadowLevel.
 */
s32 opHandler_SHADELEVEL(Actor *actor) {
    actor->shadowLevel = POP_BYTE(&actor->context);
    return 2;
}

/**
 * @brief Pop a byte from the script stack into @c field_0x241.
 */
s32 opHandler_DIR(Actor *actor) {
    actor->field_0x241 = POP_BYTE(&actor->context);
    return 2;
}

/**
 * @brief Conditionally activate a message based on the entity's active bit.
 *
 * If bit @c scriptSlot of @c activeMask is set, peek the top three
 * stack slots, shift each left by 12, and write them as the message's
 * @c textPtr / @c posX / @c posY (fixed-point). Also clears
 * @c msgState, @c windowId, @c moveSpeed and sets @c msgActive=1.
 * Returns 1 (wait for message) without popping.
 *
 * If the bit is clear, the message is skipped: @c msgState=2,
 * @c msgActive=0, the current @c msgChannel is preserved into
 * @c moveSpeed, and three stack entries are discarded.
 * Returns 2 (advance PC).
 *
 * @param actor Pointer to the Actor event-script context.
 */
s32 opHandler_DIRP(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive    = 1;
        actor->msgState     = 0;
        actor->windowId     = 0;
        actor->moveSpeed = 0;
        actor->msgTextPtr = actor->context.stack[(s8)actor->context.stackPtr - 2] << 12;
        actor->msgPosX    = actor->context.stack[(s8)actor->context.stackPtr - 1] << 12;
        actor->msgPosY    = actor->context.stack[(s8)actor->context.stackPtr]     << 12;
        return 1;
    }
    actor->msgState     = 2;
    actor->msgActive    = 0;
    actor->context.stackPtr    -= 3;
    actor->moveSpeed = actor->msgChannel;
    return 2;
}

/**
 * @brief Pop an entity index, dispatch the bearing-resolver, store byte to @c field_0x241.
 *
 * Pops one s32 from the script stack and uses it as an index into
 * @c D_80085230 to fetch a target @c Actor. Calls @c func_8009E604 with
 * the current entity and the target, and writes the byte result into
 * @c field_0x241 (seeds turn-state for the next CTURN-family helper).
 */
s32 opHandler_DIRA(Actor *actor) {
    actor->field_0x241 = func_8009E604(actor, D_80085230[POP(&actor->context)]);
    return 2;
}

/**
 * @brief Variant of @c opHandler_DIRA that looks up the target via the
 *        SeeD party-member slot table.
 *
 * Pops one s32 from the stack as a slot index, uses it to read
 * @c g_fieldVars->memberSlot[slot], indexes that into the entity
 * array @c D_80085224 to fetch a target @c Actor, dispatches
 * @c func_8009E604 with the current entity and the target, and writes
 * the byte result into @c field_0x241.
 *
 * @param actor Pointer to the Actor event-script context.
 * @return 2 (advance PC).
 */
s32 opHandler_PDIRA(Actor *actor) {
    s32 slot = POP(&actor->context);
    actor->field_0x241 = func_8009E604(actor, &D_80085224[g_fieldVars->memberSlot[slot]]);
    return 2;
}

/**
 * @brief Helper — pop byte + halfword, queue turn (subtract variant, kind 1).
 *
 * Same shape as @c opHandler_OP16C but with reversed comparison: subtracts
 * @c 0x100 from the heading if @c field_0x1DC is less than @c (s16)raw.
 * Sets @c field_0x244 to @c 1.
 */
s32 opHandler_OP16B(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 1;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        if (actor->field_0x1DC < (s16)raw) {
            actor->field_0x1DE = raw - 0x100;
        }
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Helper — pop byte + halfword, queue turn state, wrap to +0x100 if needed.
 *
 * Active path: pops a byte (top), then sets up turn state with
 * @c field_0x244=1, @c field_0x243=0, @c field_0x1DC seeded from
 * @c field_0x241, @c field_0x242 set to the popped byte. Then pops
 * the next halfword as the requested heading, stores it to
 * @c field_0x1DE, and if the signed value is less than @c field_0x1DC
 * adds @c 0x100 (forces forward rotation).
 *
 * Inactive path: return 2 unless @c field_0x244 == 3, otherwise 1.
 */
s32 opHandler_OP16C(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 1;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        if ((s16)raw < actor->field_0x1DC) {
            actor->field_0x1DE = raw + 0x100;
        }
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Helper — pop byte + halfword, queue turn (subtract variant, kind 2).
 *
 * Same as @c opHandler_OP16B but sets @c field_0x244 to @c 2.
 */
s32 opHandler_OP16D(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 2;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        if (actor->field_0x1DC < (s16)raw) {
            actor->field_0x1DE = raw - 0x100;
        }
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Helper — pop byte + halfword, queue turn (add variant, kind 2).
 *
 * Same as @c opHandler_OP16C but sets @c field_0x244 to @c 2.
 */
s32 opHandler_OP16E(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 2;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        if ((s16)raw < actor->field_0x1DC) {
            actor->field_0x1DE = raw + 0x100;
        }
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * Adjust halfword at 0x1DE based on distance from 0x1DC.
 * If abs(0x1DE - 0x1DC) >= 0x81, adjusts by +/- 0x100.
 * Also clears byte at 0x243.
 *
 * @param actor Pointer to the script/object structure.
 */
/**
 * @brief Wrap the target heading to the shorter arc when |target-current|>0x80.
 *
 * Computes the absolute heading delta between @c field_0x1DC (current) and
 * @c field_0x1DE (target). If the gap is more than half a turn (@c >=0x81),
 * adjusts @c field_0x1DE by @c ±0x100 so the rotation takes the short way
 * around. Also clears the per-step kind byte @c field_0x243.
 */
void func_800BA3E0(Actor *actor) {
    s16 de = actor->field_0x1DE;
    s16 dc = actor->field_0x1DC;
    u16 deu = actor->field_0x1DE;
    s32 diff = de - dc;
    if (diff < 0) {
        diff = -diff;
    }
    actor->field_0x243 = 0;
    if (diff >= 0x81) {
        if (dc < de) {
            actor->field_0x1DE = deu - 0x100;
        } else {
            actor->field_0x1DE = deu + 0x100;
        }
    }
}

/**
 * @brief Op 0x082 handler — pop byte+halfword, queue turn (kind 1, shortest path).
 *
 * Same as @c opHandler_OP16C but delegates the heading-wrap math to
 * @c func_800BA3E0 (which picks the shortest rotation direction).
 * Sets @c field_0x244 to @c 1.
 */
s32 opHandler_LTURNR(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 1;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        func_800BA3E0(actor);
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Op 0x083 handler — identical to @c opHandler_LTURNR.
 */
s32 opHandler_LTURNL(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 1;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        func_800BA3E0(actor);
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief CTURNR opcode 0x084 handler — same shape as @c opHandler_LTURNR with kind 2.
 */
s32 opHandler_CTURNR(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 2;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        func_800BA3E0(actor);
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief CTURNL opcode 0x085 handler — identical to CTURNR (@c opHandler_CTURNR).
 */
s32 opHandler_CTURNL(Actor *actor) {
    s32 raw;
    u8 byte1;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        byte1 = POP_BYTE(&actor->context);
        actor->field_0x244 = 2;
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x242 = byte1;
        raw = (u16)POP(&actor->context);
        actor->field_0x1DE = raw;
        func_800BA3E0(actor);
    } else if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Op 0x???  — start an N-step rotation if the entity's bit is set.
 *
 * If the entity's @c activeMask bit (selected by @c scriptSlot) is set,
 * pops two values: the target bearing byte (@c first) and a SeeD-party
 * member index. Looks up the target Actor via @c D_80085230[idx],
 * snapshots @c field_0x241 into @c field_0x1DC, dispatches
 * @c func_8009E604 to compute the target bearing into @c field_0x1DE,
 * and writes @c first into @c field_0x242. If the snapshot matches the
 * new bearing (no turn needed) returns @c 2; otherwise marks
 * @c field_0x244 = 1, dispatches @c func_800BA3E0, and returns @c 1.
 *
 * If the bit is clear and @c field_0x244 == 3, returns @c 2 (turn
 * complete). Otherwise returns @c 1 (wait).
 *
 * @param actor Pointer to the Actor event-script context.
 */
s32 opHandler_LTURN(Actor *actor) {
    s32 first;
    s32 idx;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        first = POP(&actor->context);
        idx = POP(&actor->context);
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x1DE = func_8009E604(actor, D_80085230[idx]) & 0xFF;
        actor->field_0x242 = first;
        if (actor->field_0x1DC == actor->field_0x1DE) {
            return 2;
        }
        actor->field_0x244 = 1;
        func_800BA3E0(actor);
        return 1;
    }
    if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Variant of @c opHandler_LTURN that marks the turn state as @c 2.
 *
 * Same control flow as @c opHandler_LTURN, but writes @c field_0x244 = 2
 * (instead of @c 1) when a turn needs to start. The "turn complete"
 * sentinel is still @c 3 in the bit-clear branch.
 *
 * @param actor Pointer to the Actor event-script context.
 */
s32 opHandler_CTURN(Actor *actor) {
    s32 first;
    s32 idx;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        first = POP(&actor->context);
        idx = POP(&actor->context);
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x1DE = func_8009E604(actor, D_80085230[idx]) & 0xFF;
        actor->field_0x242 = first;
        if (actor->field_0x1DC == actor->field_0x1DE) {
            return 2;
        }
        actor->field_0x244 = 2;
        func_800BA3E0(actor);
        return 1;
    }
    if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Variant of @c opHandler_LTURN that looks up the target via the
 *        SeeD party-member slot table.
 *
 * Same control flow as @c opHandler_LTURN, but the index popped after
 * the bearing byte is treated as a SeeD-party slot. The target Actor
 * is obtained via @c &D_80085224[g_fieldVars->memberSlot[slot]].
 *
 * @param actor Pointer to the Actor event-script context.
 */
s32 opHandler_PLTURN(Actor *actor) {
    s32 first;
    s32 slot;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        first = POP(&actor->context);
        slot = POP(&actor->context);
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x1DE = func_8009E604(actor, &D_80085224[g_fieldVars->memberSlot[slot]]) & 0xFF;
        actor->field_0x242 = first;
        if (actor->field_0x1DC == actor->field_0x1DE) {
            return 2;
        }
        actor->field_0x244 = 1;
        func_800BA3E0(actor);
        return 1;
    }
    if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Variant of @c opHandler_PLTURN that marks the turn state as @c 2.
 *
 * Same as @c opHandler_PLTURN but writes @c field_0x244 = 2 when a turn
 * needs to start.
 *
 * @param actor Pointer to the Actor event-script context.
 */
s32 opHandler_PCTURN(Actor *actor) {
    s32 first;
    s32 slot;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        first = POP(&actor->context);
        slot = POP(&actor->context);
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x1DE = func_8009E604(actor, &D_80085224[g_fieldVars->memberSlot[slot]]) & 0xFF;
        actor->field_0x242 = first;
        if (actor->field_0x1DC == actor->field_0x1DE) {
            return 2;
        }
        actor->field_0x244 = 2;
        func_800BA3E0(actor);
        return 1;
    }
    if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief One-shot bearing-set variant.
 *
 * Similar pop+lookup pattern as @c opHandler_PLTURN / @c opHandler_PCTURN.
 * If the entity's group bit is set, pops the bearing byte and a SeeD
 * party-slot index, computes the target bearing via @c func_8009E604,
 * and either marks @c field_0x244 = 3 (no turn needed) or starts a
 * turn (@c field_0x244 = 2, dispatch @c func_800BA3E0).
 *
 * Always returns @c 2 — the caller never has to wait.
 *
 * @param actor Pointer to the Actor event-script context.
 */
s32 opHandler_HASITEM(Actor *actor) {
    s32 first;
    s32 slot;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        first = POP(&actor->context);
        slot = POP(&actor->context);
        actor->field_0x243 = 0;
        actor->field_0x1DC = actor->field_0x241;
        actor->field_0x1DE = func_8009E604(actor, &D_80085224[g_fieldVars->memberSlot[slot]]) & 0xFF;
        actor->field_0x242 = first;
        if (actor->field_0x1DC == actor->field_0x1DE) {
            actor->field_0x244 = 3;
        } else {
            actor->field_0x244 = 2;
            func_800BA3E0(actor);
        }
    }
    return 2;
}

/**
 * @brief Returns 2 once the turn-state kind byte reaches 3, otherwise 1.
 */
s32 opHandler_CLOCKWISETURN(Actor *actor) {
    if (actor->field_0x244 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Returns 2 once the queued facing matches the current facing.
 *
 * @p arg1 is ignored; it exists in the prototype because every field
 * opcode handler is called with the dispatcher's @c (actor, arg1)
 * pair, and several wrappers in this file forward their own @c arg1
 * straight through (e.g. @c opHandler_FACEDIRA) so the function pointer
 * call lands with a real value in @c a1.
 */
s32 opHandler_FACEDIRSYNC(Actor *actor, s32 arg1) {
    if (actor->turnLen == actor->turnTick) {
        return 2;
    }
    return 1;
}

/**
 * @brief Op 0x108 handler — like @c opHandler_RFACEDIRI then dispatch via @c opHandler_FACEDIRSYNC.
 *
 * Pops 4 halfwords into @c turnPitchDst/0x22E/0x232/0x234 (clearing
 * @c turnTick and @c turnMode), then tail-calls
 * @c opHandler_FACEDIRSYNC to apply the queued state.
 */
s32 opHandler_FACEDIRI(Actor *actor, s32 arg1) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        actor->turnYawDst = POP(&actor->context);
        actor->turnRollDst = POP(&actor->context);
        actor->turnPitchDst = POP(&actor->context);
        actor->turnTick = 0;
        actor->turnMode = 0;
    }
    return opHandler_FACEDIRSYNC(actor, arg1);
}

/**
 * @brief Op 0x0FD handler — pop 4 halfwords into facing slot then dispatch.
 *
 * Like @c opHandler_RFACEDIR (active path) but ends by calling
 * @c opHandler_FACEDIRSYNC to apply the queued facing state.
 */
s32 opHandler_FACEDIR(Actor *actor, s32 arg1) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        actor->turnTgtZ = POP(&actor->context);
        actor->turnTgtY = POP(&actor->context);
        actor->turnTgtX = POP(&actor->context);
        actor->turnTick = 0;
        actor->turnMode = 1;
    }
    return opHandler_FACEDIRSYNC(actor, arg1);
}

/**
 * @brief Snapshot the target entity's grid-cell position into the
 *        queued turn-state fields, then dispatch @c opHandler_FACEDIRSYNC.
 *
 * If the entity's group bit is set, pops a bearing halfword (saved to
 * @c turnLen) and a field-entity index, queries the navigation
 * helper @c func_800A8DAC at the target's spatial index, divides the
 * target's @c posX / @c posY / @c posZ by 4096 (signed, round toward
 * zero) into @c turnTgtX / @c turnTgtY, and combines posZ with
 * the queried halfword (@c buf[2]) into @c turnTgtZ. Clears
 * @c turnTick and sets @c turnMode to @c 1 to mark the data ready.
 *
 * Always dispatches @c opHandler_FACEDIRSYNC at the end (which compares the
 * queued bearing against the current one) and returns its result.
 *
 * @param actor Pointer to the Actor event-script context.
 * @param arg1  Forwarded as the second argument to @c opHandler_FACEDIRSYNC.
 */
s32 opHandler_FACEDIRA(Actor *actor, s32 arg1) {
    s32 idx;
    s16 buf[4];

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        idx = POP(&actor->context);
        func_800A8DAC(D_80085230[idx]->field_0x256, 0x1E, (u32)g_curFieldView, buf);
        actor->turnTgtX = D_80085230[idx]->posX / 4096;
        actor->turnTgtY = D_80085230[idx]->posY / 4096;
        actor->turnTgtZ = buf[2] + D_80085230[idx]->posZ / 4096;
        actor->turnMode = 1;
        actor->turnTick = 0;
    }
    return opHandler_FACEDIRSYNC(actor, arg1);
}

/**
 * @brief Variant of @c opHandler_FACEDIRA that resolves the target via the
 *        SeeD party-member slot table.
 *
 * Same body as @c opHandler_FACEDIRA, but the index popped from the stack
 * is treated as a SeeD party-slot. The target entity is the one whose
 * Actor lives at @c &D_80085224[g_fieldVars->memberSlot[slot]], and
 * that same byte index is the spatial argument to @c func_800A8DAC.
 *
 * @param actor Pointer to the Actor event-script context.
 * @param arg1  Forwarded as the second argument to @c opHandler_FACEDIRSYNC.
 */
s32 opHandler_FACEDIRP(Actor *actor, s32 arg1) {
    u8 slot;
    s16 buf[4];

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        slot = g_fieldVars->memberSlot[POP(&actor->context)];
        func_800A8DAC(slot, 0x1E, (u32)g_curFieldView, buf);
        actor->turnTgtX = D_80085224[slot].posX / 4096;
        actor->turnTgtY = D_80085224[slot].posY / 4096;
        actor->turnTgtZ = buf[2] + D_80085224[slot].posZ / 4096;
        actor->turnMode = 1;
        actor->turnTick = 0;
    }
    return opHandler_FACEDIRSYNC(actor, arg1);
}

/**
 * @brief Queue a relative offset turn target.
 *
 * If the entity's group bit is set, pops one halfword as the target
 * bearing (saved to @c turnLen), then queries
 * @c func_800A8DAC(field_0x256, @c 0x20, @c buf, @c 0) to fill three
 * halfwords describing the relative offset. Each entry is divided by
 * @c 16 (signed, round toward zero) and stored to @c turnPitchDst /
 * @c turnRollDst / @c turnYawDst. Clears @c turnTick and
 * @c turnMode to keep this as a step-relative (not snapshot) turn.
 *
 * Always dispatches @c opHandler_FACEDIRSYNC at the end.
 *
 * @param actor Pointer to the Actor event-script context.
 * @param arg1  Forwarded as the second argument to @c opHandler_FACEDIRSYNC.
 */
s32 opHandler_FACEDIROFF(Actor *actor, s32 arg1) {
    s16 buf[4];

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        ((void (*)(u8, s32, void *, void *))func_800A8DAC)(actor->field_0x256, 0x20, buf, 0);
        actor->turnPitchDst = buf[0] / 16;
        actor->turnRollDst = buf[1] / 16;
        actor->turnYawDst = buf[2] / 16;
        actor->turnTick = 0;
        actor->turnMode = 0;
    }
    return opHandler_FACEDIRSYNC(actor, arg1);
}

/**
 * @brief Helper that pops 4 halfwords and stores them as a facing-state block.
 *
 * While the entity's @c activeMask bit is set: pops four halfwords from
 * the script stack (top → @c turnLen, then @c turnYawDst, then
 * @c turnRollDst, then @c turnPitchDst); clears @c turnTick and
 * @c turnMode. Returns 2.
 */
s32 opHandler_RFACEDIRI(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        actor->turnYawDst = POP(&actor->context);
        actor->turnRollDst = POP(&actor->context);
        actor->turnPitchDst = POP(&actor->context);
        actor->turnTick = 0;
        actor->turnMode = 0;
    }
    return 2;
}

/**
 * @brief Helper that pops 4 halfwords into another facing-state slot.
 *
 * Like @c opHandler_RFACEDIRI but stores into @c turnTgtX/0x224/0x226/0x234
 * and sets @c turnMode to 1 (instead of 0). Returns 2.
 */
s32 opHandler_RFACEDIR(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->turnLen = POP(&actor->context);
        actor->turnTgtZ = POP(&actor->context);
        actor->turnTgtY = POP(&actor->context);
        actor->turnTgtX = POP(&actor->context);
        actor->turnTick = 0;
        actor->turnMode = 1;
    }
    return 2;
}
