#include "common.h"
#include "field.h"
#include "field/opcodes.h"
#include "gamestate.h"
#include "card.h"
#include "cd.h"
#include "field/fe_object3.h"
#include "field/fe_object4.h"

/**
 * @brief Drain the @c D_800DE7B0 CD-load command queue.
 *
 * For each queued @ref CmdEntry, calls @c cdReadAsyncSync to read the
 * data into @p buf (and advances @p buf by @c entry.unk8 * 4 for the
 * next entry). Once all queued entries have been read, clears the
 * queue's @c count to zero.
 *
 * @param buf Destination buffer; advanced by each entry's stride.
 * @param arg Async-completion callback (passed through to @c cdReadAsyncSync).
 */
void func_800ADB68(u8 *buf, s32 arg) {
    s32 i;
    for (i = 0; i < D_800DE7B0.count; i++) {
        cdReadAsyncSync(D_800DE7B0.entries[i].unk0, D_800DE7B0.entries[i].unk4, buf, (void (*)(void))arg);
        buf += D_800DE7B0.entries[i].unk8 * 4;
    }
    D_800DE7B0.count = 0;
}

/**
 * @brief Revive any KO'd active-party characters with 1 HP.
 *
 * Walks the 3 active party slots and for each member that's currently
 * alive (charId != 0xFF) and has @c currentHp == 0, restores HP to 1
 * and clears status bit 0 (the KO bit).
 */
void func_800ADC04(void) {
    s32 i;
    for (i = 0; i < PARTY_SLOT_COUNT; i++) {
        if (g_gameState.mainData.party.party[i] != PARTY_SLOT_EMPTY
            && g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp == 0) {
            g_gameState.chars[g_gameState.mainData.party.party[i]].currentHp = 1;
            g_gameState.chars[g_gameState.mainData.party.party[i]].statusFlags &= ~1u;
        }
    }
}

/**
 * Returns 0, indicating no action taken.
 *
 * @param actor Pointer to the script/object structure (unused).
 * @return 0.
 */
s32 opHandler_NOP(ScriptContext *context) {
    return 0;
}

/**
 * Pops the stack index, adds the value at [idx] to [idx+1], stores at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_ADD(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] += context->stack[idx + 1];
}

/**
 * Pops the stack index, subtracts [idx+1] from [idx], stores at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_SUB(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] -= context->stack[idx + 1];
}

/**
 * Negates the top-of-stack value in-place.
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_NEG(ScriptContext *context) {
    s32 idx = (s8)context->stackPtr;
    context->stack[idx] = -context->stack[idx];
}

/**
 * Pops the stack index, multiplies [idx] by [idx+1], stores at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_MUL(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] *= context->stack[idx + 1];
}

/** @brief Stack division: [idx] = [idx] / [idx+1]. */
void opHandler_DIV(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] /= context->stack[idx + 1];
}

/** @brief Stack modulo: [idx] = [idx] % [idx+1]. */
void opHandler_MOD(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] %= context->stack[idx + 1];
}

/**
 * Pops the stack index, tests [idx] == [idx+1], stores boolean at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_EQ(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] = context->stack[idx] == context->stack[idx + 1];
}

/** @brief Stack greater-than: [idx] = [idx] > [idx+1]. */
void opHandler_GT(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] = context->stack[idx] > context->stack[idx + 1];
}

/**
 * Pops the stack index, compares [idx] >= [idx+1], stores boolean at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_GE(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] = context->stack[idx] >= context->stack[idx + 1];
}

/**
 * @brief Stack less-than comparison: stack[top-1] = stack[top-1] < stack[top].
 *
 * @param actor Pointer to the actor (script context).
 */
void opHandler_LT(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] = context->stack[idx] < context->stack[idx + 1];
}

/**
 * Pops the stack index, compares [idx] <= [idx+1], stores boolean at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_LE(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] = context->stack[idx] <= context->stack[idx + 1];
}

/**
 * Pops the stack index, tests [idx] != [idx+1], stores boolean at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_NE(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] = context->stack[idx] != context->stack[idx + 1];
}

/**
 * Pops the stack index, ANDs [idx] with [idx+1], stores at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_AND(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] &= context->stack[idx + 1];
}

/**
 * Pops the stack index, ORs [idx] with [idx+1], stores at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_OR(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] |= context->stack[idx + 1];
}

/**
 * Pops the stack index, XORs [idx] with [idx+1], stores at [idx].
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_EOR(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] ^= context->stack[idx + 1];
}

/**
 * Bitwise NOTs the top-of-stack value in-place.
 *
 * @param actor Pointer to the script/object structure.
 */
void opHandler_NOT(ScriptContext *context) {
    s32 idx = (s8)context->stackPtr;
    context->stack[idx] = ~context->stack[idx];
}

/** @brief Stack arithmetic right shift: [idx] = [idx] >> [idx+1]. */
void opHandler_RSH(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] >>= context->stack[idx + 1];
}

/** @brief Stack left shift: [idx] = [idx] << [idx+1]. */
void opHandler_LSH(ScriptContext *context) {
    s32 idx;
    context->stackPtr--;
    idx = (s8)context->stackPtr;
    context->stack[idx] <<= context->stack[idx + 1];
}

/**
 * @brief Meta-dispatcher — invokes a handler from g_fieldOpcodeTable by raw index.
 *
 * Reads the second-byte sub-opcode (@p opcode) from the script bytecode
 * stream, looks up the handler in @c g_fieldOpcodeTable[opcode], and
 * tail-calls it with the current actor. This is itself bound to entry
 * 0x13 of the table (wiki opcode @c 0x001) — the runtime dispatcher
 * hits it when the bytecode contains a "0x01 NN" pair, with @c NN
 * decoded as the sub-opcode and arriving in @c a1.
 *
 * Used both for the 18 stack-arithmetic ops (sub-opcodes 0x00-0x11)
 * and to re-enter the main opcode table by raw index when the bytecode
 * needs an opcode that does not have its own direct entry.
 *
 * @param actor  Pointer to the actor (script context).
 * @param opcode Raw index into @c g_fieldOpcodeTable (0..391).
 * @return 2 (continue processing).
 */
s32 opHandler_CAL(Actor *actor, s32 opcode) {
    g_fieldOpcodeTable[opcode](actor);
    return 2;
}

/**
 * Adds a1 to the halfword at offset 0x176 in the object, returns 4.
 *
 * @param actor Pointer to the script/object structure.
 * @param a1 Value to add.
 * @return 4.
 */
s32 opHandler_JMP(ScriptContext *context, s32 a1) {
    context->pc += a1;
    return 4;
}

/** @brief Conditional branch: if top-of-stack is zero, add a1 to PC. */
s32 opHandler_JPF(ScriptContext *context, s32 a1) {
    if (POP(context) != 0) {
        return 2;
    }
    context->pc += a1;
    return 4;
}

/** @brief Push (halfword at 0x176)+1 onto stack, load new 0x176 from table. Returns 4. */
s32 opHandler_GJMP(ScriptContext *context, s32 a1) {
    PUSH(context, context->pc + 1);
    context->pc = D_800852F0[a1];
    return 4;
}

/**
 * @brief Opcode 0x05 — SAV: spill result-slot register file to the
 *        bytecode stack and reset the slots.
 *
 * Pushes the eight result-register values (@c actor->context.resultSlots[0..7])
 * onto the bytecode stack in order, then clears each slot to zero.
 * After the spill, if @c FLAG_SAV_SUCCESS is set in @c actor->context.flags,
 * slot 0 is re-armed to @c 1 — a "success/default" marker the caller
 * can fall through to.
 *
 * This is the script-VM analogue of a save-registers prologue; a
 * later opcode pops the saved values back to restore the slot file.
 *
 * @param actor Script context.
 * @param a1    Ignored (dispatcher-supplied opcode argument).
 * @return 2 (advance PC).
 */
s32 opHandler_LBL(ScriptContext *context, s32 a1) {
    s32 i;

    for (i = 0; i < 8; i++) {
        PUSH(context, context->resultSlots[i]);
        context->resultSlots[i] = 0;
    }
    if (context->flags & 0x20) {
        context->resultSlots[0] = 1;
    }
    return 2;
}

/** @brief Pop halfword from stack and store to entity offset 0x176. */
void func_800AE184(Actor *actor) {
    actor->context.pc = (u16)POP(&actor->context);
}

/**
 * @brief Script-VM context-switch / "RET" dispatcher.
 *
 * Pops 8 stack words into the result-slot file (saving the running slot's
 * register frame onto @c resultSlots), clears flag bit 0x20, sets the
 * @c activeMask bit for the current script slot, then dispatches:
 *   - case 9: simple pop PC via @c func_800AE184.
 *   - case 8: backward scan of @c savedPc[] looking for the first
 *             non-0xFFFF entry; set @c pc and @c scriptSlot to that slot.
 *   - default: switch to script slot @c a1 — mark the current slot's
 *             @c savedPc as 0xFFFF, scan downward clearing saved PCs and
 *             setting @c activeMask bits, reset @c stackPtr from
 *             @c savedStackPtr[a1], and reload that slot's 8 saved words
 *             into @c resultSlots.
 *
 * Counterpart of @c opHandler_LBL which performs the save half of the
 * context switch (pushes @c resultSlots[0..7] back onto @c stack and
 * zeros them).
 */
s32 opHandler_RET(Actor *actor, s32 a1) {
    s32 i;
    s32 sg;

    for (sg = 0; sg < 8; sg++) {
        actor->context.resultSlots[sg] = POP(&actor->context);
    }

    actor->context.flags &= ~0x20;
    actor->context.activeMask |= (1 << actor->context.scriptSlot);

    switch (a1) {
    case 9:
        func_800AE184(actor);
        break;
    case 8:
        func_800B66A8(actor);
        actor->context.savedPc[actor->context.scriptSlot] = 0xFFFF;
        sg = actor->context.scriptSlot - 1;
        while (sg >= 0) {
            if (actor->context.savedPc[sg] != 0xFFFF) {
                actor->context.pc = actor->context.savedPc[sg];
                actor->context.scriptSlot = sg;
                break;
            }
            sg--;
        }
        break;
    default:
        func_800B66A8(actor);
        actor->context.savedPc[actor->context.scriptSlot] = 0xFFFF;
        for (sg = actor->context.scriptSlot - 1; sg > a1; sg--) {
            if (actor->context.savedPc[sg] != 0xFFFF) {
                actor->context.activeMask |= (1 << sg);
                actor->context.savedPc[sg] = 0xFFFF;
            }
        }

        actor->context.stackPtr = actor->context.savedStackPtr[a1] + 8;

        for (i = 0; i < 8; i++) {
            actor->context.resultSlots[i] = POP(&actor->context);
        }

        actor->context.pc = actor->context.savedPc[a1];
        actor->context.scriptSlot = a1;
        break;
    }

    return 4;
}

/**
 * @brief Helper for the PSH* opcode family — clamps @p value to the
 *        signed/unsigned range selected by @p mode.
 *
 *  - mode 0/1: bit-test (mask to bit 0, then s8 clamp — yields 0 or 1)
 *  - mode 2:   s8  range [-0x80, 0x7F]
 *  - mode 3:   s16 range [-0x8000, 0x7FFF]
 *  - mode 4:   s24 range [-0x800000, 0x7FFFFF]
 *  - mode 5:   u8  range [0, 0xFF]
 *  - mode 6:   u16 range [0, 0xFFFF]
 *  - mode 7:   u24 range [0, 0xFFFFFF]
 *  - mode ≥8:  returns @p value unchanged.
 *
 * Used by all PSHM_B/W/L and PSHSM_B/W/L handlers. The gcc-emitted
 * jump table for this switch lands at @c 0x80098174 thanks to the
 * @c .rodata,fe_object4 split in field.ovl.yaml.
 */
s32 func_800AE3A4(s32 value, s32 mode) {
    if ((u32)mode >= 8) {
        return value;
    }
    switch (mode) {
    case 0:
    case 1:
        value &= 1;
        /* fallthrough */
    case 2:
        if (value < -0x80) {
            value = -0x80;
        } else if (value >= 0x7F) {
            value = 0x7F;
        }
        break;
    case 3:
        if (value < -0x8000) {
            value = -0x8000;
        } else if (value >= 0x7FFF) {
            value = 0x7FFF;
        }
        break;
    case 4:
        if (value < (s32)0xFF800000) {
            value = (s32)0xFF800000;
        } else if (value > 0x7FFFFE) {
            value = 0x7FFFFF;
        }
        break;
    case 5:
        if (value < 0) {
            value = 0;
        } else if (value >= 0xFF) {
            value = 0xFF;
        }
        break;
    case 6:
        if (value < 0) {
            value = 0;
        } else if (value > 0xFFFE) {
            value = 0xFFFF;
        }
        break;
    case 7:
        if (value < 0) {
            value = 0;
        } else if (value > 0xFFFFFE) {
            value = 0xFFFFFF;
        }
        break;
    }
    return value;
}

/** @brief Call func_800AE3A4 with a1 and mode 4, push result onto stack. Returns 2. */
s32 opHandler_PSHN_L(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(a1, 4);
    PUSH(context, result);
    return 2;
}

/** @brief Load entity slot value a1*4+0x140, call func_800AE3A4 with mode 4, push result. Returns 2. */
s32 opHandler_PSHI_L(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(context->resultSlots[a1], 4);
    PUSH(context, result);
    return 2;
}

/** @brief Load a field-variable byte (@c g_gameState + @ref GAMESTATE_MISC3_OFFSET + a1),
 *         call @c func_800AE3A4 with mode 5, push the result. Returns 2. */
s32 opHandler_PSHM_B(ScriptContext *context, s32 a1) {
    /* Script-VM M-memory byte (= fieldVars[a1]). Phrased as g_gameState+a1
     * first and indexed by the block offset second, so gcc keeps that offset
     * in the lbu instead of folding it into the symbol address. */
    u8 *p = (u8 *)&g_gameState + a1;
    s32 result = func_800AE3A4(p[GAMESTATE_MISC3_OFFSET], 5);
    PUSH(context, result);
    return 2;
}

/** @brief Load halfword from D_800780D8+a1, call func_800AE3A4 with mode 6, push result. Returns 2. */
s32 opHandler_PSHM_W(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(*(u16 *)&D_800780D8[a1], 6);
    PUSH(context, result);
    return 2;
}

/** @brief Load word from D_800780D8+a1, call func_800AE3A4 with mode 7, push result. Returns 2. */
s32 opHandler_PSHM_L(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(*(s32 *)&D_800780D8[a1], 7);
    PUSH(context, result);
    return 2;
}

/** @brief Load signed byte from D_800780D8+a1, call func_800AE3A4 with mode 2, push result. Returns 2. */
s32 opHandler_PSHSM_B(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(*(s8 *)&D_800780D8[a1], 2);
    PUSH(context, result);
    return 2;
}

/** @brief Load signed halfword from D_800780D8+a1, call func_800AE3A4 with mode 3, push result. Returns 2. */
s32 opHandler_PSHSM_W(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(*(s16 *)&D_800780D8[a1], 3);
    PUSH(context, result);
    return 2;
}

/** @brief Load word from D_800780D8+a1, call func_800AE3A4 with mode 4, push result. Returns 2. */
s32 opHandler_PSHSM_L(ScriptContext *context, s32 a1) {
    s32 result = func_800AE3A4(*(s32 *)&D_800780D8[a1], 4);
    PUSH(context, result);
    return 2;
}

/** @brief Pop top-of-stack and store to result slot a1*4+0x140. Returns 2. */
s32 opHandler_POPI_L(ScriptContext *context, s32 a1) {
    context->resultSlots[a1] = POP(context);
    return 2;
}

/**
 * @brief Script-VM M-memory byte store: @c fieldVars[a1] = top-of-stack.
 *
 * Writes the popped value as a byte into the field-variable block, which lives
 * at @c g_gameState + @ref GAMESTATE_MISC3_OFFSET (what @c g_fieldVars points at).
 *
 * @note The address is built by raw arithmetic on @c &g_gameState rather than
 *       through the struct because the index is a byte offset into the whole
 *       block, not a named field; the folded form is what the original
 *       codegen emits.
 *
 * @param actor  Script-VM actor whose stack is popped.
 * @param a1 Byte offset into the field-variable block.
 * @return @c 2 (continue processing).
 */
s32 opHandler_POPM_B(ScriptContext *context, s32 a1) {
    u8 *p = (u8 *)&g_gameState + a1;
    p[GAMESTATE_MISC3_OFFSET] = (u8)POP(context);
    return 2;
}

/** @brief Pop halfword from stack and store to D_800780D8[a1]. Returns 2. */
s32 opHandler_POPM_W(ScriptContext *context, s32 a1) {
    *(u16 *)&D_800780D8[a1] = (u16)POP(context);
    return 2;
}

/** @brief Pop word from stack and store to D_800780D8[a1]. Returns 2. */
s32 opHandler_POPM_L(ScriptContext *context, s32 a1) {
    *(s32 *)&D_800780D8[a1] = POP(context);
    return 2;
}

/** @brief Push immediate value a1 onto stack. Returns 2. */
s32 opHandler_PSHAC(ScriptContext *context, s32 a1) {
    PUSH(context, a1);
    return 2;
}

/**
 * @brief Script-VM PC manipulation: enqueue current PC at a per-script
 *        slot and look up the new PC from @c D_800852F0[slot] table.
 *
 * Used by @c opHandler_REQSW / @c opHandler_REQEW to context-switch the
 * actor's script context.  Calls @c func_800B663C to flush pending
 * state.
 *
 * @brief Central VM context-switch helper. If @c newGroup's slot is
 *        free (@c savedPc[newGroup] == @c 0xFFFF), saves the current
 *        group's stack pointer; if @c newGroup is higher than the
 *        current group, also saves the pc and switches to the new
 *        group's pc-table entry (calling @c func_800B663C). Always
 *        writes the new group's @c D_800852F0[pcIdx] into
 *        @c savedPc[newGroup]. Returns 1 on success, 0 if the
 *        slot was already taken.
 */
s32 func_800AE8B4(Actor *actor, u8 newGroup, u16 pcIdx) {
    if (actor->context.savedPc[newGroup] != 0xFFFF) {
        return 0;
    }

    actor->context.savedStackPtr[actor->context.scriptSlot] = actor->context.stackPtr;

    if (actor->context.scriptSlot < newGroup) {
        actor->context.savedPc[actor->context.scriptSlot] = actor->context.pc;
        actor->context.scriptSlot = newGroup;
        actor->context.pc = D_800852F0[pcIdx];
        func_800B663C(actor);
    }

    actor->context.savedPc[newGroup] = D_800852F0[pcIdx];
    return 1;
}

/**
 * @brief Pop @c pcIdx and @c newGroup off the script stack, look up
 *        @c D_80085230[a1] (the target entity); if the target is active
 *        and its @c field_0x256 matches @c D_800DE4FC, bump @c e->context.pc
 *        and return 4. Otherwise return 3. The popped values are passed
 *        to @c func_800AE8B4 which applies the actual group/pc update.
 *
 * Returns 3 if @c D_80085230[a1] is @c NULL (no call made).
 */
s32 opHandler_REQ(ScriptContext *context, s32 a1) {
    s32 pcIdx = POP(context);
    s32 newGroup = POP(context);
    s32 retVal;

    if (D_80085230[a1] != 0) {
        if (D_800DE4FC == D_80085230[a1]->field_0x256) {
            retVal = 4;
            context->pc++;
        } else {
            retVal = 3;
        }
        func_800AE8B4(D_80085230[a1], (u8)newGroup, (u16)pcIdx);
    } else {
        retVal = 3;
    }
    return retVal;
}

/**
 * @brief Script-VM "test entity bit + branch" opcode.
 *
 * Looks up entity @c D_80085230[a1]; if it has bit set in
 * @c activeMask matching @c scriptSlot, calls @c func_800AE8B4 to
 * forward a stack-popped value to it. If the call succeeds and the
 * target entity's @c scriptSlot matches the peeked value, the 2-slot
 * peek gets popped and @c 3 is returned; otherwise @c 1 is returned
 * leaving the stack intact.
 */
s32 opHandler_REQSW(ScriptContext *context, s32 a1) {
    s32 value;
    s32 callResult;

    if (D_80085230[a1] == NULL) {
        context->stackPtr -= 2;
        return 3;
    }

    value = context->stack[(s8)context->stackPtr - 1];

    if ((context->activeMask >> context->scriptSlot) & 1) {
        callResult = func_800AE8B4(D_80085230[a1], (u8)value,
                                   (u16)context->stack[(s8)context->stackPtr]);
        if (callResult == 0) {
            context->stackPtr -= 2;
            return 3;
        }
    }

    if (D_80085230[a1]->context.scriptSlot == value) {
        context->stackPtr -= 2;
        return 3;
    }

    return 1;
}

/**
 * @brief Variant of @c opHandler_REQSW — identical shape but the final
 *        scriptSlot check uses @c < instead of @c == .
 */
s32 opHandler_REQEW(ScriptContext *context, s32 a1) {
    s32 value;
    s32 callResult;

    if (D_80085230[a1] == NULL) {
        context->stackPtr -= 2;
        return 3;
    }

    value = context->stack[(s8)context->stackPtr - 1];

    if ((context->activeMask >> context->scriptSlot) & 1) {
        callResult = func_800AE8B4(D_80085230[a1], (u8)value,
                                   (u16)context->stack[(s8)context->stackPtr]);
        if (callResult == 0) {
            context->stackPtr -= 2;
            return 3;
        }
    }

    if (D_80085230[a1]->context.scriptSlot < value) {
        context->stackPtr -= 2;
        return 3;
    }

    return 1;
}

/**
 * @brief Pop 2 values, look up the entity in slot @c memberSlot[a1] of
 *        @c D_80085224, and forward to @c func_800AE8B4 with derived args.
 *
 * Re-reads @c g_fieldVars->memberSlot[a1] twice (matches the asm's two
 * @c lbu reads) rather than caching it — a cached form re-allocates
 * registers and breaks the match.
 */
s32 opHandler_PREQ(ScriptContext *context, s32 a1) {
    s32 v1_pop = POP(context);
    s32 v2_pop = POP(context);
    if (g_fieldVars->memberSlot[a1] != 0xFF) {
        Actor *entity = &D_80085224[g_fieldVars->memberSlot[a1]];
        func_800AE8B4(entity, v2_pop, entity->context.rangeLo + v1_pop);
    }
    return 3;
}

/**
 * @brief Script-VM "send-to-party-member" opcode: looks up
 *        @c g_fieldVars->memberSlot[a1], finds the resolved entity in
 *        @c D_80085224, and dispatches via @c func_800AE8B4.
 *
 * AEA44-style routing, but the entity comes from the field-actor table
 * @c D_80085224 (stride 612), indexed via @c g_fieldVars->memberSlot[a1].
 * If the slot is empty (0xFF), pops the 2 peeked values and returns 3.
 */
s32 opHandler_PREQSW(ScriptContext *context, s32 a1) {
    s32 value;
    s32 top;
    s32 callResult;
    Actor *entity;

    value = context->stack[(s8)context->stackPtr - 1];
    top = context->stack[(s8)context->stackPtr];

    if (g_fieldVars->memberSlot[a1] != 0xFF) {
        if ((context->activeMask >> context->scriptSlot) & 1) {
            entity = &D_80085224[g_fieldVars->memberSlot[a1]];
            callResult = func_800AE8B4(entity, (u8)value,
                                       (u16)(entity->context.rangeLo + top));
            if (callResult == 0) {
                context->stackPtr -= 2;
                return 3;
            }
        }

        if (D_80085224[g_fieldVars->memberSlot[a1]].context.scriptSlot == value) {
            context->stackPtr -= 2;
            return 3;
        }

        return 1;
    }

    context->stackPtr -= 2;
    return 3;
}

/**
 * @brief Variant of @c opHandler_PREQSW — identical shape but the final
 *        scriptSlot check uses @c < instead of @c == .
 */
s32 opHandler_PREQEW(ScriptContext *context, s32 a1) {
    s32 value;
    s32 top;
    s32 callResult;
    Actor *entity;

    value = context->stack[(s8)context->stackPtr - 1];
    top = context->stack[(s8)context->stackPtr];

    if (g_fieldVars->memberSlot[a1] != 0xFF) {
        if ((context->activeMask >> context->scriptSlot) & 1) {
            entity = &D_80085224[g_fieldVars->memberSlot[a1]];
            callResult = func_800AE8B4(entity, (u8)value,
                                       (u16)(entity->context.rangeLo + top));
            if (callResult == 0) {
                context->stackPtr -= 2;
                return 3;
            }
        }

        if (D_80085224[g_fieldVars->memberSlot[a1]].context.scriptSlot < value) {
            context->stackPtr -= 2;
            return 3;
        }

        return 1;
    }

    context->stackPtr -= 2;
    return 3;
}

/**
 * Returns 3, indicating skip to next entity.
 *
 * @param actor Pointer to the script/object structure (unused).
 * @return 3.
 */
s32 opHandler_DEBUG(ScriptContext *context) {
    return 3;
}

/**
 * Returns 1, indicating wait/yield.
 *
 * @param actor Pointer to the script/object structure (unused).
 * @return 1.
 */
s32 opHandler_HALT(ScriptContext *context) {
    return 1;
}

/**
 * @brief Pop posY, posX (both << 12 fixed-point), set triIdx = arg,
 *        then call func_8009A8E0 with the dereferenced @c D_8008538C.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_SET(Actor *actor, s32 a1) {
    actor->posY = POP(&actor->context) << 12;
    actor->posX = POP(&actor->context) << 12;
    actor->triIdx = a1;
    func_8009A8E0(D_8008538C);
    return 2;
}

/**
 * @brief Pop posZ, posY, posX (all << 12), set triIdx, and call
 *        func_8009A8E0. 3-axis variant of @c opHandler_SET.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_SET3(Actor *actor, s32 a1) {
    actor->posZ = POP(&actor->context) << 12;
    actor->posY = POP(&actor->context) << 12;
    actor->posX = POP(&actor->context) << 12;
    actor->triIdx = a1;
    func_8009A8E0(D_8008538C);
    return 2;
}

/**
 * @brief Set bit @p bit in @c g_fieldEntity.statusBits (512-bit packed array).
 *
 * @p bit indexes a single bit; bits are packed 8-per-byte. Used as a
 * script-VM "set status bit" opcode.
 *
 * @note The @c SystemState* alias keeps gcc from folding the @c statusBits
 *       field offset into the @c g_fieldEntity symbol — the asm uses
 *       @c lbu 0x1b8(a1) with the offset as an immediate.
 */
s32 opHandler_IDLOCK(ScriptContext *context, s32 bit) {
    SystemState *p = &g_fieldEntity;
    s32 byteIdx = bit / 8;
    p->statusBits[byteIdx] |= (u8)(1 << (bit - byteIdx * 8));
    return 2;
}

/**
 * @brief Clear bit @p bit in @c g_fieldEntity.statusBits. Counterpart to
 *        @c opHandler_IDLOCK.
 */
s32 opHandler_IDUNLOCK(ScriptContext *context, s32 bit) {
    SystemState *p = &g_fieldEntity;
    s32 byteIdx = bit / 8;
    p->statusBits[byteIdx] &= ~(1 << (bit - byteIdx * 8));
    return 2;
}

/**
 * Decrement top-of-stack timer. If reaches zero, pop and return 3 (done).
 * Otherwise return 1 (wait).
 *
 * @param actor Pointer to the script/object structure.
 * @return 1 if timer still active, 3 if timer expired.
 */
s32 opHandler_WAIT(ScriptContext *context) {
    s32 idx = (s8)context->stackPtr;
    s32 val = context->stack[idx] - 1;
    context->stack[idx] = val;
    if (val == 0) {
        context->stackPtr--;
        return 3;
    }
    return 1;
}

/**
 * @brief Zero out 0x400 bytes of the field-script variable region at
 *        @c g_gameState + 0xE60.
 *
 * Clears the script-VM "field variables" region (the 1024-byte block
 * documented as @c GAMESTATE_FIELD_OFFSET in gamestate.h). The loop
 * spans seedState's tail through the start of the worldmap region.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_CLEAR(void) {
    s32 i;
    for (i = 0x100; i < 0x500; i++) {
        u8 *p = (u8 *)&g_gameState + i;
        p[GAMESTATE_MISC3_OFFSET] = 0;
    }
    return 2;
}

/**
 * Stores a random byte (0..0xFF) into the entity's @c result register.
 *
 * @param context Script entity.
 * @return 2 (continue processing).
 */
s32 opHandler_RND(ScriptContext *context) {
    context->resultSlots[0] = fieldRandom() & 0xFF;
    return 2;
}

/**
 * Stores a1 as a halfword at offset 0x218 in the object, returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @param a1 Value to store.
 * @return 2 (continue processing).
 */
s32 opHandler_SETMODEL(Actor *actor, s32 a1) {
    actor->unk218 = a1;
    return 2;
}

/**
 * @brief Pop two values: a u8 model id and a character index; store the
 *        model id into @c chars[findCharacterSlot(idx)].alternateModel.
 *
 * Counterpart to @c opHandler_GETDRESS (read-side). Used to set a character's
 * SeeD/Galbadia costume model from a script-VM stack pair.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_SETDRESS(ScriptContext *context) {
    s32 val = POP(context);
    s32 result = findCharacterSlot(POP(context));
    u8 *base = (u8 *)&g_gameState;
    base[result * 152 + 0x4EB] = (u8)val;
    return 2;
}

/**
 * Pops a value, calls findCharacterSlot to look up an index, then loads
 * the byte at g_gameState + index * 152 + 0x4EB into result slot 0x140.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_GETDRESS(ScriptContext *context) {
    /* g_gameState.chars[result].alternateModel — chars[] @ 0x490, stride
     * 152 bytes (sizeof CharacterData), alternateModel @ +0x5B inside
     * each => absolute byte offset 0x4EB for chars[0].  Split base from
     * index so gcc keeps 0x4EB as the lbu's immediate offset. */
    s32 result = findCharacterSlot(POP(context));
    u8 *base = (u8 *)&g_gameState;
    context->resultSlots[0] = base[result * 152 + 0x4EB];
    return 2;
}

/**
 * Store a1 to offset 0x24F, double-pop bytes to 0x250 and 0x251. Returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @param a1 Value to store at offset 0x24F.
 * @return 2 (continue processing).
 */
/**
 * @brief Store @p a1 to field_0x24F, then pop two stack bytes into
 *        field_0x250 and field_0x251.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_BASEANIME(Actor *actor, s32 a1) {
    actor->field_0x24F = (u8)a1;
    actor->field_0x250 = (u8)POP(&actor->context);
    actor->field_0x251 = (u8)POP(&actor->context);
    return 2;
}

/**
 * @brief Same shape as @c opHandler_BASEANIME but for fields 0x252/0x253/0x254
 *        (immediate arg + two popped bytes).
 *
 * @return 2 (VM continue).
 */
s32 opHandler_LADDERANIME(Actor *actor, s32 a1) {
    actor->field_0x252 = (u8)a1;
    actor->field_0x253 = (u8)POP(&actor->context);
    actor->field_0x254 = (u8)POP(&actor->context);
    return 2;
}

/** @brief Pop mask, test against D_800705F8, store boolean at result. Returns 2. */
s32 opHandler_KEYSCAN(ScriptContext *context) {
    if (D_800705F8 & POP(context)) {
        context->resultSlots[0] = 1;
    } else {
        context->resultSlots[0] = 0;
    }
    return 2;
}

/** @brief Pop mask, test against D_80070600, store boolean at result. Returns 2. */
s32 opHandler_KEYON(ScriptContext *context) {
    if (D_80070600 & POP(context)) {
        context->resultSlots[0] = 1;
    } else {
        context->resultSlots[0] = 0;
    }
    return 2;
}

/** @brief Pop mask, test against D_800705E8, store boolean at result. Returns 2. */
s32 opHandler_KEYSCAN2(ScriptContext *context) {
    if (D_800705E8 & POP(context)) {
        context->resultSlots[0] = 1;
    } else {
        context->resultSlots[0] = 0;
    }
    return 2;
}

/** @brief Pop mask, test against D_800705F0, store boolean at result. Returns 2. */
s32 opHandler_KEYON2(ScriptContext *context) {
    if (D_800705F0 & POP(context)) {
        context->resultSlots[0] = 1;
    } else {
        context->resultSlots[0] = 0;
    }
    return 2;
}

/**
 * Sets bits 0x18 in entity flags at g_fieldVars+0x68, then calls
 * setTransitionFlag with the inverted bit 3 value.
 *
 * @param actor Unused.
 * @return 2 (continue processing).
 */
s32 opHandler_SARALYOFF(ScriptContext *context) {
    s32 flags;

    flags = g_fieldVars->stateFlags;
    flags = flags | 0x18;
    g_fieldVars->stateFlags = flags;
    setTransitionFlag(((u32)flags >> 3 ^ 1) & 1);
    return 2;
}

/**
 * @brief Clear seedState bits 0x18 and force transition state to phase 1.
 *
 * Counterpart to @c opHandler_SARALYOFF which sets bits 0x18 + invokes
 * setTransitionFlag with the inverted bit-3 value; this one clears the
 * bits and explicitly calls setTransitionFlag(1).
 *
 * @note The unreachable @c dummy[0]=0 after @c return reserves an extra
 *       8 bytes of stack frame to match the original (0x20 vs the 0x18
 *       gcc would otherwise emit). The original likely had a debug
 *       statement here that was compiled out.
 */
s32 opHandler_SARALYON() {
    s32 dummy[2];
    g_fieldVars->stateFlags &= ~(FIELD_STATE_TRANSITION | FIELD_STATE_FIELD_READY);
    setTransitionFlag(1);
    return 2;
    dummy[0] = 0;
}

/** @brief Set bit 0x10 in @c g_fieldVars->stateFlags. Returns 2. */
s32 opHandler_SARALYDISPOFF(void) {
    g_fieldVars->stateFlags |= FIELD_STATE_FIELD_READY;
    return 2;
}

/** @brief Clear bit 0x10 in @c g_fieldVars->stateFlags. Returns 2. */
s32 opHandler_SARALYDISPON(void) {
    g_fieldVars->stateFlags &= ~FIELD_STATE_FIELD_READY;
    return 2;
}

/**
 * @brief Define this eline's trigger segment and arm it. Returns 2.
 *
 * Pops the two end-points off the script stack in the order z1, y1, x1,
 * z0, y0, x0, arms the record, and stamps the current script-character
 * marker into it. @c func_8009A4C0 scans armed records against the party
 * each frame.
 *
 * @param eline Eline running the script.
 */
s32 opHandler_SETLINE(Eline *eline) {
    u16 lastPop;
    eline->z1 = POP(&eline->context);
    eline->y1 = POP(&eline->context);
    eline->x1 = POP(&eline->context);
    eline->z0 = POP(&eline->context);
    eline->y0 = POP(&eline->context);
    lastPop = POP(&eline->context);
    eline->activeMarker = 1;
    eline->charMarker = D_800DE4FC;
    eline->x0 = lastPop;
    return 2;
}

/**
 * @brief Arm this eline's trigger segment. Returns 2.
 *
 * Counterpart of @ref opHandler_LINEOFF; the segment itself is set up once
 * by @ref opHandler_SETLINE.
 *
 * @param eline Eline running the script.
 */
s32 opHandler_LINEON(Eline *eline) {
    eline->activeMarker = 1;
    return 2;
}

/**
 * @brief Disarm this eline's trigger segment. Returns 2.
 *
 * @param eline Eline running the script.
 */
s32 opHandler_LINEOFF(Eline *eline) {
    eline->activeMarker = 0;
    return 2;
}

/**
 * Sets the byte at offset 0x188 to 1, returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_DOORLINEON(Actor *actor) {
    actor->unk188 = 1;
    return 2;
}

/**
 * Clears the byte at offset 0x188, returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_DOORLINEOFF(Actor *actor) {
    actor->unk188 = 0;
    return 2;
}

/**
 * Sets bit 0x2 in the flags at offset 0x160, returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_USE(ScriptContext *context) {
    context->flags |= 0x2;
    return 2;
}

/**
 * Clears bit 0x2 in the flags at offset 0x160, returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_UNUSE(ScriptContext *context) {
    context->flags &= ~0x2;
    return 2;
}

/**
 * @brief Broadcast "trigger active party" event.
 *
 * If @c D_800DE4FD is clear, also clears @c D_8007064B. Then if the
 * trigger flag @c D_80085390 is non-zero, clears it and walks the 3
 * active party members. For each member whose @c memberSlot is set and
 * whose field-entity is NOT locked (flags bit 0x40 clear), records the
 * member slot into the per-slot entity-index table (@c entityIndex[0]
 * via @c g_fieldEntity, then @c D_800704BB / @c D_800704BC) and sets
 * entity flags bit 0x4. Returns 2.
 */
s32 opHandler_UCON(void) {
    if (D_800DE4FD[0] == 0) {
        D_8007064B = 0;
    }

    if (D_80085390 == 0) {
        return 2;
    }

    D_80085390 = 0;
    g_fieldEntity.unk015 = 0;

    if (g_fieldVars->memberSlot[0] != 0xFF
        && !(D_80085224[g_fieldVars->memberSlot[0]].context.flags & 0x40)) {
        g_fieldEntity.entityIndex[0] = g_fieldVars->memberSlot[0];
        D_80085224[g_fieldVars->memberSlot[0]].context.flags |= 4;
    }

    if (g_fieldVars->memberSlot[1] != 0xFF
        && !(D_80085224[g_fieldVars->memberSlot[1]].context.flags & 0x40)) {
        D_800704BB = g_fieldVars->memberSlot[1];
        D_80085224[g_fieldVars->memberSlot[1]].context.flags |= 4;
    }

    if (g_fieldVars->memberSlot[2] != 0xFF
        && !(D_80085224[g_fieldVars->memberSlot[2]].context.flags & 0x40)) {
        D_800704BC = g_fieldVars->memberSlot[2];
        D_80085224[g_fieldVars->memberSlot[2]].context.flags |= 4;
    }

    return 2;
}

/**
 * @brief "Re-arm active party" — counterpart to @c opHandler_UCON.
 *
 * Always sets @c g_fieldEntity.unk1A3 = 1. If the trigger @c D_80085390 is
 * already set, returns early. Otherwise sets @c D_80085390 / @c unk015
 * and walks the 3 party slots:
 *
 *   - Slot 0: always processed if @c memberSlot[0] is valid.
 *   - Slots 1 / 2: processed only if @c entityIndex[N] was set by
 *     @c opHandler_UCON (i.e. != @c 0xFF), then the index is cleared.
 *
 * Per-slot processing calls @c func_800AA46C, copies the entity's
 * @c field_0x24F into @c field_0x24E, zeroes @c field_0x206 / @c 0x20A,
 * loads @c field_0x20C from @c D_800D9630[slot]->unk0C, masks the
 * entity flags to bits 0x0000F800 → 0x00002000, and clears bit 0x4.
 */
s32 opHandler_UCOFF(void) {
    g_fieldEntity.unk1A3 = 1;
    if (D_80085390 != 0) {
        return 2;
    }
    D_80085390 = 1;
    g_fieldEntity.unk015 = 1;

    if (g_fieldVars->memberSlot[0] != 0xFF) {
        func_800AA46C(g_fieldVars->memberSlot[0], 0xD,
                      D_80085224[g_fieldVars->memberSlot[0]].field_0x24F, 0);
        D_80085224[g_fieldVars->memberSlot[0]].field_0x24E =
            D_80085224[g_fieldVars->memberSlot[0]].field_0x24F;
        D_80085224[g_fieldVars->memberSlot[0]].field_0x206 = 0;
        D_80085224[g_fieldVars->memberSlot[0]].field_0x20A = 0;
        D_80085224[g_fieldVars->memberSlot[0]].field_0x20C =
            D_800D9630[g_fieldVars->memberSlot[0]]->unk0C;
        D_80085224[g_fieldVars->memberSlot[0]].context.flags &= 0xFFFF07FF;
        D_80085224[g_fieldVars->memberSlot[0]].context.flags |= 0x2000;
        D_80085224[g_fieldVars->memberSlot[0]].context.flags &= ~4;
    }

    if (g_fieldEntity.entityIndex[1] != 0xFF) {
        func_800AA46C(g_fieldVars->memberSlot[1], 0xD,
                      D_80085224[g_fieldVars->memberSlot[1]].field_0x24F, 0);
        D_80085224[g_fieldVars->memberSlot[1]].field_0x24E =
            D_80085224[g_fieldVars->memberSlot[1]].field_0x24F;
        D_80085224[g_fieldVars->memberSlot[1]].field_0x206 = 0;
        D_80085224[g_fieldVars->memberSlot[1]].field_0x20A = 0;
        D_80085224[g_fieldVars->memberSlot[1]].field_0x20C =
            D_800D9630[g_fieldVars->memberSlot[1]]->unk0C;
        D_80085224[g_fieldVars->memberSlot[1]].context.flags &= 0xFFFF07FF;
        D_80085224[g_fieldVars->memberSlot[1]].context.flags |= 0x2000;
        g_fieldEntity.entityIndex[1] = 0xFF;
        D_80085224[g_fieldVars->memberSlot[1]].context.flags &= ~4;
    }

    if (g_fieldEntity.entityIndex[2] != 0xFF) {
        func_800AA46C(g_fieldVars->memberSlot[2], 0xD,
                      D_80085224[g_fieldVars->memberSlot[2]].field_0x24F, 0);
        D_80085224[g_fieldVars->memberSlot[2]].field_0x24E =
            D_80085224[g_fieldVars->memberSlot[2]].field_0x24F;
        D_80085224[g_fieldVars->memberSlot[2]].field_0x206 = 0;
        D_80085224[g_fieldVars->memberSlot[2]].field_0x20A = 0;
        D_80085224[g_fieldVars->memberSlot[2]].field_0x20C =
            D_800D9630[g_fieldVars->memberSlot[2]]->unk0C;
        D_80085224[g_fieldVars->memberSlot[2]].context.flags &= 0xFFFF07FF;
        D_80085224[g_fieldVars->memberSlot[2]].context.flags |= 0x2000;
        g_fieldEntity.entityIndex[2] = 0xFF;
        D_80085224[g_fieldVars->memberSlot[2]].context.flags &= ~4;
    }

    return 2;
}

/**
 * @brief Pop a value from the actor stack; set @c D_800704BD to 0 if
 *        nonzero, to 1 if zero. Returns 2 (VM continue).
 */
s32 opHandler_KEY(ScriptContext *context) {
    if (POP(context)) {
        D_800704BD = 0;
    } else {
        D_800704BD = 1;
    }
    return 2;
}

/**
 * @brief Pop a slot id, set actor flag bit 7; if id is in {3,4,5,7}
 *        also set flag bit 0x4000000; if id matches none of the active
 *        battle-party slots (@c battleParty[0..2]), clear flag bit 2.
 */
s32 opHandler_SETPC(Actor *actor) {
    s32 v1;
    actor->context.flags |= 0x80;
    actor->field_0x255 = (u8)POP(&actor->context);
    v1 = actor->field_0x255;
    if (v1 >= 3 && (v1 < 6 || v1 == 7)) {
        actor->context.flags |= 0x4000000;
    }
    if (g_gameState.battleParty[0] != actor->field_0x255
        && g_gameState.battleParty[1] != actor->field_0x255
        && g_gameState.battleParty[2] != actor->field_0x255) {
        actor->context.flags &= ~2;
    }
    return 2;
}

/**
 * @brief Pop a character id; if it's already in the battle party, return 2.
 *        Otherwise add it to the first empty battle slot and mirror in the
 *        first empty party slot. Then either set the corresponding bit in
 *        @c g_fieldVars->stateFlags (popped >= 8) or update the character's
 *        @c exists flags and call @c recalcPartyStats (popped < 8).
 *
 * @note Match needs two tricks:
 *       - @c do{}while(0) wrapper around the two findBattlePartySlot
 *         calls keeps @c newSlot live across the early-return branch.
 *       - chained assignment @c partySlot=(newSlot=findPartySlot(0xFF))
 *         reuses the second findBattlePartySlot's register for the
 *         findPartySlot return, matching how gcc allocates s-regs in
 *         the target.
 */
s32 opHandler_ADDPARTY(ScriptContext *context) {
    s32 popped = POP(context);
    s32 first;
    s32 newSlot;
    s32 partySlot;

    do {
        first = findBattlePartySlot(popped);
        if (first != 0xFF) {
            return 2;
        }
        newSlot = findBattlePartySlot(0xFF);
    } while (0);

    if (newSlot != first) {
        g_gameState.battleParty[newSlot] = popped;
        partySlot = (newSlot = findPartySlot(0xFF));
        g_gameState.mainData.party.party[partySlot] = findCharacterSlot(popped);
    }

    if (popped >= 8) {
        popped -= 8;
        g_fieldVars->stateFlags |= (1 << popped);
        return 2;
    }

    g_gameState.chars[popped].exists |= 1;
    if (popped == 0) {
        g_gameState.chars[0].exists |= 4;
    }
    if (popped != 4) {
        g_gameState.chars[popped].exists |= 8;
    }
    recalcPartyStats();
    return 2;
}

/**
 * @brief Pop a character id; clear it from the battle party (0xD38) and
 *        from the active party (0xAF4); if the party-lock flag is set,
 *        call @c func_80036B90 on the active-party slot first.
 *        Recalc party stats & revive any downed actives.
 */
s32 opHandler_SUBPARTY(ScriptContext *context) {
    s32 charId = POP(context);
    s32 slot;
    slot = findBattlePartySlot(charId);
    if (slot != PARTY_SLOT_EMPTY) {
        g_gameState.battleParty[slot] = PARTY_SLOT_EMPTY;
    }
    slot = func_80037C6C(charId);
    if (slot != PARTY_SLOT_EMPTY) {
        if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
            func_80036B90(g_gameState.mainData.party.party[slot]);
        }
        g_gameState.mainData.party.party[slot] = PARTY_SLOT_EMPTY;
    }
    recalcPartyStats();
    func_800ADC04();
    return 2;
}

/**
 * @brief Swap a character into the active party / misc2 region.
 *
 * Pops a destination slot id and a source character id, then if the
 * source is currently not in the active party, juggles it into the
 * misc2 (0xD38) and party (0xAF4) tables at the popped slot, calling
 * @c findCharacterSlot to translate the character.  Complex bench-swap
 * logic with multiple lookup helpers.
 *
 * @note Match requires three staging tricks that all funnel the call
 *       returns through extra locals so gcc allocates s-regs the way
 *       the original asm does:
 *         - first call:  `new_var2 = call; slot = new_var2;`
 *         - second call: `slot = (new_var = call);`
 *         - explicit `noneSlot = 0xFF` variable instead of literal
 */
s32 opHandler_CHANGEPARTY(ScriptContext *context) {
    s32 popped1 = POP(context);
    s32 popped2 = POP(context);
    s32 slot;
    s32 noneSlot;
    s32 new_var;
    s32 new_var2;

    new_var2 = findBattlePartySlot(popped1);
    slot = new_var2;
    noneSlot = 0xFF;
    if (slot != noneSlot) {
        return 2;
    }

    slot = (new_var = func_80037C6C(g_gameState.battleParty[popped2]));
    if (slot == noneSlot) {
        slot = findPartySlot(noneSlot);
        if (slot == noneSlot) {
            slot = popped2;
        }
    }

    g_gameState.mainData.party.party[slot] = findCharacterSlot(popped1);
    g_gameState.battleParty[popped2] = popped1;
    recalcPartyStats();
    return 2;
}

/**
 * @brief Pop 3 character ids and replace the active battle party (and
 *        mirrored mainData.party) with them.
 *
 * If the party-lock flag is set, any popped value of @c 0xFF triggers a
 * cleanup of the existing battleParty slot (@c findCharacterSlot +
 * @c func_80036B90), then writes the new battleParty entries, and
 * mirrors them into @c mainData.party via @c findCharacterSlot on the
 * just-written battleParty (with slots 0/1 swapped).
 *
 * If the lock flag is clear, just writes the popped values directly
 * (no character-slot translation), still with slots 0/1 swapped in the
 * party mirror.
 *
 * Calls @c recalcPartyStats and @c func_800ADC04 at the end.
 */
s32 opHandler_SETPARTY(ScriptContext *context) {
    s32 slot2 = POP(context);
    s32 slot1 = POP(context);
    s32 slot0 = POP(context);

    if (g_gameState.mainData.partyLockFlag & PARTY_LOCK_LOCKED) {
        if (slot0 == 0xFF) {
            func_80036B90(findCharacterSlot(g_gameState.battleParty[0]));
        }
        if (slot1 == 0xFF) {
            func_80036B90(findCharacterSlot(g_gameState.battleParty[1]));
        }
        if (slot2 == 0xFF) {
            func_80036B90(findCharacterSlot(g_gameState.battleParty[2]));
        }

        g_gameState.battleParty[2] = slot2;
        g_gameState.battleParty[0] = slot0;
        g_gameState.battleParty[1] = slot1;

        g_gameState.mainData.party.party[2] = findCharacterSlot(g_gameState.battleParty[2]);
        g_gameState.mainData.party.party[1] = findCharacterSlot(g_gameState.battleParty[0]);
        g_gameState.mainData.party.party[0] = findCharacterSlot(g_gameState.battleParty[1]);
    } else {
        g_gameState.battleParty[0] = slot0;
        g_gameState.battleParty[1] = slot1;
        g_gameState.battleParty[2] = slot2;

        g_gameState.mainData.party.party[2] = slot2;
        g_gameState.mainData.party.party[1] = slot0;
        g_gameState.mainData.party.party[0] = slot1;
    }

    recalcPartyStats();
    func_800ADC04();
    return 2;
}

/**
 * Calls func_800381BC and returns 2.
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_REFRESHPARTY(Actor *actor) {
    func_800381BC(actor);
    return 2;
}

/**
 * Pops a value, calls findBattlePartySlot, stores result at 0x140
 * (or -1 if result is 0xFF).
 *
 * @param actor Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 opHandler_ISPARTY(ScriptContext *context) {
    s32 result = findBattlePartySlot(POP(context));
    context->resultSlots[0] = result;
    if (result == 0xFF) {
        context->resultSlots[0] = -1;
    }
    return 2;
}

/**
 * @brief Pop a byte offset, read u8 from g_gameState's misc2 region
 *        (offset 0xD38) at that offset, store to result slot 0.
 *
 * Similar to PSHM_B but for the misc2 sub-region of g_gameState (just
 * before seedState). 0xD38 is inside the @c GAMESTATE_MISC2_OFFSET
 * (0xCD0) block — 104 bytes in. The script supplies the byte offset
 * via the stack.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_GETPARTY(ScriptContext *context) {
    s32 a1 = POP(context);
    u8 *p = (u8 *)&g_gameState + a1;
    context->resultSlots[0] = p[0xD38];
    return 2;
}

/**
 * @brief Switch-style set-character-flag opcode. Pops a value, dispatches
 *        via an 11-entry jump table:
 *          - 0:        @c chars[popped].exists |= 0xD
 *          - 4,6,7:    @c chars[popped].exists |= 0x1
 *          - 8,9,10:   @c g_fieldVars->stateFlags |= (1 << (popped-8))
 *          - otherwise (1,2,3,5 via jtbl default, plus @c popped>=11):
 *                      @c chars[popped].exists |= 0x9
 *
 * Symmetric with @c opHandler_SUBMEMBER.
 */
s32 opHandler_ADDMEMBER(ScriptContext *context) {
    s32 popped = POP(context);
    if ((u32)popped < 0xB) {
        switch (popped) {
        case 0:
            g_gameState.chars[popped].exists |= 0xD;
            break;
        case 4:
        case 6:
        case 7:
            g_gameState.chars[popped].exists |= 1;
            break;
        case 8:
        case 9:
        case 10:
            popped -= 8;
            g_fieldVars->stateFlags |= (1 << popped);
            break;
        default:
            g_gameState.chars[popped].exists |= 9;
            break;
        }
    } else {
        g_gameState.chars[popped].exists |= 9;
    }
    return 2;
}

/**
 * @brief Switch-style clear-character-flag opcode. Pops a value, runs
 *        @c findCharacterSlot then @c func_80036B90; dispatches on the
 *        popped value:
 *          - 6:        @c chars[6].exists &= 0xFFF6 + @c func_800C0448
 *          - 7:        @c chars[7].exists &= 0xFFF6 + @c func_800C0634
 *          - 8,9,10:   @c g_fieldVars->stateFlags &= ~(1 << (popped-8))
 *          - otherwise (0..5, 11+): @c chars[popped].exists &= 0xFFF6
 *
 * Symmetric clear-counterpart of @c opHandler_ADDMEMBER. Unlike B0344's
 * jump-table dispatch, B0444 compiles to an if-else chain because the
 * cases are sparse.
 */
s32 opHandler_SUBMEMBER(ScriptContext *context) {
    s32 popped = POP(context);
    func_80036B90(findCharacterSlot(popped));

    switch (popped) {
    case 6:
        g_gameState.chars[popped].exists &= 0xFFF6;
        func_800C0448();
        break;
    case 7:
        g_gameState.chars[popped].exists &= 0xFFF6;
        func_800C0634();
        break;
    case 8:
    case 9:
    case 10:
        popped -= 8;
        g_fieldVars->stateFlags &= ~(1 << popped);
        break;
    default:
        g_gameState.chars[popped].exists &= 0xFFF6;
        break;
    }
    return 2;
}

/**
 * @brief Pop bit index; if >= 8, push bit (idx-8) of @c stateFlags into
 *        @c resultSlots[0]; else look up the character and return bit 0
 *        of @c chars[slot].exists (or 0 if no slot).
 */
s32 opHandler_ISMEMBER(ScriptContext *context) {
    s32 a1 = POP(context);
    s32 slot;
    if (a1 >= 8) {
        a1 -= 8;
        context->resultSlots[0] = g_fieldVars->stateFlags & (1 << a1);
        return 2;
    }
    slot = findCharacterSlot(a1);
    if (slot == PARTY_SLOT_EMPTY) {
        context->resultSlots[0] = 0;
        return 2;
    }
    context->resultSlots[0] = g_gameState.chars[slot].exists & 1;
    return 2;
}

/**
 * @brief Build the bench-character list: walk character ids 0..5 and
 *        record every character that is NOT in the active battle party
 *        into seedState's @c partyOrderA / @c partyOrderB tables.
 *
 * Calls @c recalcPartyStats once the bench is rebuilt.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_SETPARTY2(ScriptContext *context, s32 a1) {
    s32 i, j;
    j = 0;
    for (i = 0; i < 6; i++) {
        if (findBattlePartySlot(i) == 0xFF) {
            g_fieldVars->partyOrderA[j] = (u8)i;
            g_fieldVars->partyOrderB[j] = (u8)i;
            j++;
        }
    }
    recalcPartyStats();
    return 2;
}

/**
 * @brief Swap @c g_fieldVars's bench lists (@c partyOrderA/B) with
 *        @c g_gameState's active party and battle-party slots.
 *
 * For each of the 3 party slots:
 *   - save battleParty[i] and mainData.party[i] into local buffers,
 *   - copy partyOrderA[i] → battleParty[i] and partyOrderB[i] → party[i],
 *   - write the saved values back into partyOrderA[i] / partyOrderB[i].
 *
 * Finishes with @c recalcPartyStats.
 */
s32 opHandler_SWAP(ScriptContext *context) {
    u8 savedBattle[3], savedParty[3];
    s32 i;
    for (i = 0; i < 3; i++) {
        savedBattle[i]                = g_gameState.battleParty[i];
        savedParty[i]                 = g_gameState.mainData.party.party[i];
        g_gameState.battleParty[i]    = g_fieldVars->partyOrderA[i];
        g_gameState.mainData.party.party[i] = g_fieldVars->partyOrderB[i];
        g_fieldVars->partyOrderA[i]   = savedBattle[i];
        g_fieldVars->partyOrderB[i]   = savedParty[i];
    }
    recalcPartyStats();
    return 2;
}

/**
 * @brief Set @ref FIELD_STATE_PARTY_OVERRIDE, then if popped value is nonzero
 *        force fieldF3 to 0xFF; mirror fieldF3 into @c D_80082C10 and
 *        @c GameConfig.sealedFeatures, then tail into @c opHandler_SETPARTY2.
 *
 * @return 2 (VM continue).
 */
s32 opHandler_LASTIN(Actor *actor, s32 a1) {
    g_fieldVars->stateFlags |= FIELD_STATE_PARTY_OVERRIDE;
    if (POP(&actor->context) != 0) {
        g_fieldVars->fieldF3 = 0xFF;
    }
    D_80082C10 = g_fieldVars->fieldF3;
    g_gameState.config.sealedFeatures = g_fieldVars->fieldF3;
    opHandler_SETPARTY2(&actor->context, a1);
    return 2;
}

/**
 * Clears bit 0x800 in entity flags at g_fieldVars+0x68, clears
 * D_80082C10 and @c GameConfig.sealedFeatures, then calls recalcPartyStats.
 *
 * @param actor Unused.
 * @return 2 (continue processing).
 */
s32 opHandler_LASTOUT(ScriptContext *context) {
    /* Take the address of stateFlags so gcc materializes the read & write
     * through one register — keeps the seedState updates together
     * (before D_80082C10/sealedFeatures + recalcPartyStats() in the schedule). */
    s32 *p = &g_fieldVars->stateFlags;
    *p = *p & ~0x800;
    D_80082C10 = 0;
    g_gameState.config.sealedFeatures = 0;
    recalcPartyStats();
    return 2;
}
