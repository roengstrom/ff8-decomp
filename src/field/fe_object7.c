#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "field.h"
#include "sound.h"
#include "psxsdk/libgte.h"
#include "field/fe_object1.h"
#include "field/fe_object1_2.h"
#include "field/fe_object7.h"

/**
 * @brief Pop a key item ID and store its value.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_WHERECARD(ScriptContext *context) {
    context->resultSlots[0] = getKeyItemValue(POP(context));
    return 2;
}

/**
 * @brief Table index 0x14C handler — initiate battle encounter.
 *
 * Pops encounter parameters from the bytecode stack, sets up the battle
 * transition (fade, sound shutdown, animation cleanup), then waits for
 * battle completion. On the return pass (entity inactive), reads the
 * battle result.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 on first pass (battle started), 2 on return (result ready).
 */
s32 opHandler_CARDGAME(ScriptContext *context) {
    u8 *params;
    s32 result;
    s32 i;

    if ((context->activeMask >> context->scriptSlot) & 1) {
        EncounterParams *params = &D_80082C90;
        params->field_08 = POP_BYTE(context);
        params->field_06 = POP_BYTE(context);
        params->field_07 = POP_BYTE(context);
        params->field_09 = POP_BYTE(context);
        params->field_04 = POP_BYTE(context);
        params->encounterPtr = POP(context);
        params->field_05 = POP_BYTE(context);

        result = sumItemQuantities(params);
        context->resultSlots[0] = result;
        context->resultSlots[1] = 0;

        if (result >= 5) {
            if (!(g_fieldVars->stateFlags & FIELD_STATE_FIELD_READY)) {
                initBattleTransition();
            }

            g_fieldEntity.mode = 8;
            g_fieldEntity.counter = 0;

            while (sndGetStatus() == 2) {
                func_800393C8();
            }

            for (i = 0; i < 2; i++) {
                clearAnimEntryActive(i);
            }

            setCameraVibrateState(0);

            if (g_fieldVars->soundHandle0 == SND_HANDLE_NONE) {
                sndCmd11(0);
            }

            sndCmd40();
            func_80037FB0(0, 0x46, D_8005F13C);

            while (g_fieldVars->soundLoadComplete == 0) {
                func_800393C8();
            }

            D_80082C11 = g_fieldVars->soundBankSelector;
            D_8005F11C = sndCmd10(toggleSoundBank());
            sndCmdC0(0, 0x7F);
            sndStopPlayback();
            sndCmdF1();
            sndSetMasterVolume(0x7F);
        }

        return 1;
    } else {
        if (D_80082C90.result == 3) {
            context->resultSlots[1] = -1;
        } else {
            context->resultSlots[1] = D_80082C90.result;
        }
        return 2;
    }
}


/**
 * @brief Copy a null-terminated string to the D_800DE880 buffer.
 *
 * @param src Source string.
 * @return Pointer to D_800DE880 (the copied string).
 */
u8 *func_800B574C(u8 *src) {
    u8 *dst = D_800DE880;

    while (*src) {
        *dst++ = *src++;
    }
    *dst = *src;

    return D_800DE880;
}

/**
 * @brief Search the D_800DE880 buffer for a marker byte and offset the
 *        byte following each match.
 *
 * @param search Marker byte to search for (masked to u8).
 * @param offset Value to add to the byte after each match.
 * @return Pointer to D_800DE880.
 */
u8 *func_800B578C(s32 search, s32 offset) {
    u8 *ptr = D_800DE880;

    if (*ptr) {
        search &= 0xFF;
        do {
            if (*ptr == search) {
                ptr++;
                *ptr += offset;
            }
            ptr++;
        } while (*ptr);
    }

    return D_800DE880;
}


/**
 * @brief Process message control codes for party member display.
 *
 * Scans the D_800DE880 buffer interpreting control codes:
 *   - 0x02: count separator, truncate at maxCount+1
 *   - 0x03: insert party member name offset, check junctioned ability
 *   - 0x06: check ability ownership and status flag
 *
 * @param maxCount Maximum number of entries to show.
 * @param abilityId Ability ID to check for junctioned/owned status.
 * @return Pointer to D_800DE880.
 */
u8 *func_800B57E8(s32 maxCount, s32 abilityId) {
    u8 *ptr = D_800DE880;
    s32 count = 0;
    s32 charId;
    s32 memberIdx;
    u8 *base;

    if (*ptr) {
        base = (u8 *)&g_gameState;
        memberIdx = 0;

        do {
            switch (*ptr) {
            case 2:
                if (count >= maxCount + 1) {
                    *ptr = 0;
                    return D_800DE880;
                }
                count++;
                break;
            case 3:
                ptr++;
                *ptr += base[0xD38 + memberIdx];
                charId = func_80037C6C(base[0xD38 + memberIdx]);
                if (hasJunctionedAbility(charId, abilityId)) {
                    ptr += 2;
                    *ptr = 5;
                    ptr++;
                    *ptr = 0x41;
                }
                memberIdx++;
                break;
            case 6:
                charId = func_80037C6C(base[0xD38 + memberIdx]) & 0xFF;
                if (!func_80021108(base[0xAF4 + charId], abilityId)) {
                    if (g_battleChars.chars[charId].fieldStatusByte & 2) {
                        ptr++;
                        *ptr = 0x27;
                    }
                }
                break;
            default:
                break;
            }
            ptr++;
        } while (*ptr);
    }

    return D_800DE880;
}

/**
 * @brief Recalculate party stats and check if any member has fieldStatusByte bit 1 set.
 *
 * Sets D_80082C10 from WorldContext field_0xF3 (if flag 0x800 is active),
 * calls recalcPartyStats(), then checks each party slot.
 *
 * @return 1 if any active party member has fieldStatusByte bit 1 set, 0 otherwise.
 */
s32 func_800B5990(void) {
    s32 i;

    if (g_fieldVars->stateFlags & FIELD_STATE_PARTY_OVERRIDE) {
        D_80082C10 = g_fieldVars->fieldF3;
    } else {
        D_80082C10 = 0;
    }

    recalcPartyStats();

    for (i = 0; i < 3; i++) {
        if (g_gameState.mainData.party.party[i] != 0xFF) {
            if (g_battleChars.chars[i].fieldStatusByte & 2) {
                return 1;
            }
        }
    }

    return 0;
}

/**
 * @brief Draw point interaction handler (8-state machine).
 *
 * Manages the full draw-point interaction: prompt, ability check,
 * party member selection, magic draw, result display, and used-flag update.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while processing, 2 when complete.
 */
s32 opHandler_DRAWPOINT(Actor *actor) {
    s32 fieldIdx;
    s32 tableResult;
    u8 *text;
    u32 dims;
    s32 i;
    s16 rect[4];

    fieldIdx = PEEK(&actor->context) - 1;
    tableResult = lookupFieldTable(fieldIdx);

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->field_0x204 = 0;
    }

    switch (actor->field_0x204) {
    case 0:
        g_fieldVars->fieldF2 = fieldIdx;
        setSfxEntryVolume(6, 0x1000);
        setSfxEntityType(6, 6);

        if (func_800B5990()) {
            i = getPackedField2Bit(fieldIdx);
            if (i == 3 || getPackedField2Bit(fieldIdx) == 2) {
                text = func_800B574C(func_8003974C(D_8005630C, 3));
            } else {
                text = func_800B574C(func_8003974C(D_8005630C, 2));
                text = func_800B578C(0xC, tableResult & 0x3F);
                sndPlaySfx(0x42, 0, 0x80, 0x7F);
            }
        } else {
            text = func_800B574C(func_8003974C(D_8005630C, 7));
        }

        initSfxPlayback(6, text);
        dims = func_8002E680(text);
        rect[2] = (dims & 0xFFFF) + 0x10;
        rect[3] = (dims >> 16) + 0x11;
        rect[0] = 0xA0 - rect[2] / 2;
        rect[1] = 0x70 - rect[3] / 2;
        func_8002E064(6, rect);
        startSfxSlow(6);
        setSfxGlobalFlag(6);
        g_fieldVars->sfxStartMask |= 0x40;

        if (tableResult & 0x80) {
            D_800DE4D0 = 0x14;
        } else {
            D_800DE4D0 = 0xA;
        }

        if (getPackedField2Bit(fieldIdx) == 1) {
            D_800DE4D0 /= 2;
        }

        i = fieldRandom() & 0xFF;
        D_800DE4D0 = D_800DE4D0 * (i + 0x80) / 512 + 1;
        actor->field_0x204++;
        break;

    case 1:
        if (g_fieldVars->sfxStartMask & 0x40) {
            return 1;
        }

        i = getPackedField2Bit(fieldIdx);
        if (i == 3 || getPackedField2Bit(fieldIdx) == 2) {
            actor->context.resultSlots[0] = 0;
            actor->context.stackPtr--;
            return 2;
        }

        if (func_800B5990()) {
            actor->field_0x204++;
            break;
        }

        actor->context.resultSlots[0] = 0;
        actor->context.stackPtr--;
        return 2;

    case 2:
        for (D_800DE4D4 = 0; D_800DE4D4 < 3; D_800DE4D4++) {
            if (g_gameState.battleParty[D_800DE4D4] == 0xFF) {
                break;
            }
        }

        text = func_800B574C(func_8003974C(D_8005630C, 4));
        text = func_800B57E8(D_800DE4D4, tableResult & 0x3F);
        dims = func_8002E680(text);
        rect[2] = (dims & 0xFFFF) + 0x30;
        rect[3] = (dims >> 16) + 0x11;
        rect[0] = 0xA0 - rect[2] / 2;
        rect[1] = 0x70 - rect[3] / 2;
        func_8002E064(6, rect);
        func_8002D784(6, text, 1, D_800DE4D4 + 1, 2, 1);
        startSfxSlow(6);
        setSfxGlobalFlag(6);
        g_fieldVars->sfxStartMask |= 0x40;
        g_fieldVars->sfxActiveMask |= 0x40;
        actor->field_0x204++;
        break;

    case 3:
        setSfxGlobalFlag(6);
        D_800DE4D2 = func_8002CE84(6);
        if ((s8)D_800DE4D2 < 0) {
            return 1;
        }
        fadeOutSfxSlow(6);
        g_fieldVars->sfxActiveMask &= ~0x40;

        if ((s8)D_800DE4D2 == 0) {
            actor->context.resultSlots[0] = 0;
            actor->context.stackPtr--;
            return 2;
        }

        D_800DE4D2--;
        actor->field_0x204++;
        break;

    case 4:
        D_800DE4D3 = func_80037C6C(g_gameState.battleParty[D_800DE4D2]);
        if (getSfxField1C(6)) {
            return 1;
        }

        if (g_battleChars.chars[D_800DE4D3].fieldStatusByte & 2) {
            func_800A455C(g_fieldVars->memberSlot[D_800DE4D2]);
            sndPlaySfx(0x43, 0, 0x80, 0x7F);
            actor->field_0x204++;
            break;
        }

        actor->field_0x204 += 2;
        break;

    case 5:
        if (func_800A48CC()) {
            return 1;
        }
        actor->field_0x204++;
        break;

    case 6:
        if (g_battleChars.chars[D_800DE4D3].fieldStatusByte & 2) {
            for (i = 0; i < D_800DE4D0; i++) {
                if (func_800211B4(g_gameState.mainData.party.party[D_800DE4D3], tableResult & 0x3F)) {
                    break;
                }
            }

            func_8002E1B4(7, i);

            if (i != 0) {
                text = func_800B574C(func_8003974C(D_8005630C, 5));
            } else {
                text = func_800B574C(func_8003974C(D_8005630C, 6));
                actor->field_0x204 = 0;
            }
        } else {
            text = func_800B574C(func_8003974C(D_8005630C, 6));
            actor->field_0x204 = 0;
        }

        func_800B578C(3, g_gameState.battleParty[D_800DE4D2]);
        text = func_800B578C(0xC, tableResult & 0x3F);
        initSfxPlayback(6, text);
        dims = func_8002E680(text);
        rect[2] = (dims & 0xFFFF) + 0x10;
        rect[3] = (dims >> 16) + 0x11;
        rect[0] = 0xA0 - rect[2] / 2;
        rect[1] = 0x70 - rect[3] / 2;
        func_8002E064(6, rect);
        startSfxSlow(6);
        setSfxGlobalFlag(6);
        g_fieldVars->sfxStartMask |= 0x40;
        actor->field_0x204++;
        break;

    case 7:
        if (g_fieldVars->sfxStartMask & 0x40) {
            return 1;
        }

        if (tableResult & 0x40) {
            func_800383B8(fieldIdx, 2);
        } else {
            func_800383B8(fieldIdx, 3);
        }

        actor->context.stackPtr--;
        return 2;
    }

    return 1;
}

/**
 * @brief Set up a scripted camera/effect using actor position data.
 *
 * Sets WorldContext field_0xF0 to 1 (active), pops a byte parameter
 * into field_0xF1, then calls func_800A4500 with the actor's position
 * fields and func_800A4550 with field_0xF1 | field_0x58.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_SETDRAWPOINT(Actor *actor) {
    g_fieldVars->fieldF0 = 1;
    g_fieldVars->fieldF1 = POP_BYTE(&actor->context);
    func_800A4500(actor->posX, actor->posY, actor->posZ);
    func_800A4550(g_fieldVars->fieldF1 | g_fieldVars->field58);
    return 2;
}

/**
 * @brief Pop a byte from the stack, decrement, and store to WorldContext field_0xF2.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_UNKNOWN10(ScriptContext *context) {
    g_fieldVars->fieldF2 = POP_BYTE(context);
    g_fieldVars->fieldF2--;
    return 2;
}

/**
 * @brief Activate a system slot by index from the script stack.
 *
 * Pops a value, masks to 4 bits (0-15), and sets the corresponding
 * slotActive entry in the system state to 1.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_PARTICLEON(ScriptContext *context) {
    g_fieldEntity.slotActive[POP(context) & 0xF] = 1;
    return 2;
}

/**
 * @brief Deactivate a system slot by index from the script stack.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_PARTICLEOFF(ScriptContext *context) {
    g_fieldEntity.slotActive[POP(context) & 0xF] = 0;
    return 2;
}

/**
 * @brief Activate a system slot with the actor's field_0x256 value | 0x80.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_PARTICLESET(Actor *actor) {
    g_fieldEntity.slotActive[POP(&actor->context) & 0xF] = actor->field_0x256 | 0x80;
    return 2;
}

/**
 * @brief Pop a value; if nonzero call func_800C0384 (set bit 0x20),
 *        otherwise call func_800C03A0 (clear bit 0x20).
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_SETWITCH(ScriptContext *context) {
    if (POP(context)) {
        func_800C0384();
    } else {
        func_800C03A0();
    }
    return 2;
}

/**
 * @brief Delegate to func_800C03F4 and return.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_SETODIN(Actor *actor) {
    func_800C03F4(actor);
    return 2;
}

/**
 * @brief Clear field_0x204 if the entity's script slot is active.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6420(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->field_0x204 = 0;
    }
    return 2;
}

/**
 * @brief Pop a value from the stack and store to D_8007737C.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_SETPLACE(ScriptContext *context) {
    D_8007737C = POP(context);
    return 2;
}

/**
 * @brief Pop a value and store to both D_80082C0A and WorldContext field_0xB6.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_BATTLEMODE(ScriptContext *context) {
    D_80082C0A = g_fieldVars->fieldB6 = POP(context);
    do {} while (0);
    return 2;
}

/**
 * @brief Set system mode to 3 (if idle) and pop scene ID + counter.
 *
 * @param actor Pointer to the actor (script context).
 * @return 3 (special return — triggers mode transition).
 */
s32 opHandler_BATTLE(ScriptContext *context) {
    if (g_fieldEntity.mode == 0) {
        g_fieldEntity.mode = 3;
    }
    D_80082C0A = POP(context);
    g_fieldEntity.counter = POP(context);
    return 3;
}

/**
 * @brief Store D_80082C0F into the actor result field.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_BATTLERESULT(ScriptContext *context) {
    context->resultSlots[0] = D_80082C0F;
    return 2;
}

/**
 * @brief If D_800DE8D0 is set, enable flag 0x400; otherwise clear field_0xCF.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_BATTLEON(void) {
    if (D_800DE8D0) {
        g_fieldVars->stateFlags |= FIELD_STATE_FLAG_400;
    } else {
        g_fieldVars->fieldCF = 0;
    }
    return 2;
}

/**
 * @brief Set field_0xCF to 1 and clear flag 0x400.
 *
 * @return 2 (continue processing).
 */
s32 opHandler_BATTLEOFF(void) {
    FieldVars *ctx = g_fieldVars;

    ctx->fieldCF = 1;
    ctx->stateFlags &= ~FIELD_STATE_FLAG_400;
    return 2;
}

/** @brief No-op handler. Returns 2 (continue). */
s32 opHandler_BATTLECUT(ScriptContext *context) {
    return 2;
}

/**
 * @brief Set system mode to 4 (game render) and yield.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 (yield).
 */
s32 opHandler_GAMEOVER(ScriptContext *context) {
    g_fieldEntity.mode = 4;
    return 1;
}

/** @brief Yield handler. Returns 1 (wait). */
s32 opHandler_ENDING(ScriptContext *context) {
    return 1;
}

/**
 * @brief Pop disc number from stack, set WorldContext and global state.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_DISC(ScriptContext *context) {
    g_fieldVars->expectedDiscId = POP_BYTE(context);
    D_800773C0 = g_fieldVars->expectedDiscId - 1;
    setDiscNumber(g_fieldVars->expectedDiscId);
    return 2;
}

/**
 * @brief Save active message state for async restore.
 *
 * If the async-pending flag (0x10000000) is set and msgActive is 1,
 * backs up current message position/window/channel to the saved fields,
 * clears msgActive, and sets the deferred flag (0x10000).
 * Inverse of func_800B66A8 (restore).
 *
 * @param actor Pointer to the actor (script context).
 */
void func_800B663C(Actor *actor) {
    if (!(actor->context.flags & 0x10000000)) {
        return;
    }
    if (actor->msgActive != 1) {
        return;
    }

    actor->field_0x1C0 = actor->msgTextPtr;
    actor->field_0x1C4 = actor->msgPosX;
    actor->field_0x1C8 = actor->msgPosY;
    actor->field_0x21C = actor->windowId;
    actor->field_0x202 = actor->moveSpeed;
    actor->msgActive = 0;
    actor->context.flags |= 0x10000;
}

/**
 * @brief Restore a pending async message display.
 *
 * If both the async-pending flag (0x10000000) and the deferred-message
 * flag (0x10000) are set, restores the saved message parameters from
 * the backup fields (field_0x1C0..0x1C8, field_0x21C, field_0x202),
 * reactivates the message, and clears the deferred flag. If the
 * async-message flag (0x40000) is still set, re-calls func_800B6738
 * to reinitialize the display.
 *
 * @param actor Pointer to the actor (script context).
 */
void func_800B66A8(Actor *actor) {
    if (!(actor->context.flags & 0x10000000)) {
        return;
    }
    if (!(actor->context.flags & 0x10000)) {
        return;
    }
    actor->msgTextPtr = actor->field_0x1C0;
    actor->msgPosX = actor->field_0x1C4;
    actor->msgPosY = actor->field_0x1C8;
    actor->windowId = actor->field_0x21C;
    actor->moveSpeed = actor->field_0x202;
    actor->msgActive = 1;
    actor->msgState = 0;

    if ((actor->context.flags = actor->context.flags & ~0x10000) & 0x40000) {
        func_800B6738(actor);
    }
}

/**
 * @brief Initialize message display.
 *
 * Compares moveSpeed against a threshold derived from D_800704B2
 * to select between two message source indices (field_0x250 vs
 * field_0x251). If the selected source differs from field_0x24E,
 * calls func_800B912C to set it up and marks the 0x2000 flag.
 * Clears field_0x1DA and sets the async-message flag (0x40000).
 *
 * @param actor Pointer to the actor (script context).
 */
void func_800B6738(Actor *actor) {
    s32 threshold = (D_800704B2 * 69020) >> 9;

    if (actor->moveSpeed >= threshold) {
        if (actor->field_0x24E != actor->field_0x251) {
            func_800B912C(actor, actor->field_0x251);
            actor->context.flags |= 0x2000;
        }
    } else {
        if (actor->field_0x24E != actor->field_0x250) {
            func_800B912C(actor, actor->field_0x250);
            actor->context.flags |= 0x2000;
        }
    }

    actor->field_0x1DA = 0;
    actor->context.flags |= 0x40000;
}

/**
 * @brief Finalize message display.
 *
 * If the async-message flag (0x40000) is set, calls func_800B912C
 * with field_0x24F, then sets bit 0x2000 and clears the async flag.
 * Always clears field_0x240.
 *
 * @param actor Pointer to the actor (script context).
 */
void func_800B67F4(Actor *actor) {
    if (actor->context.flags & 0x40000) {
        func_800B912C(actor, actor->field_0x24F);
        actor->context.flags = (actor->context.flags | 0x2000) & ~0x40000;
    }
    actor->field_0x240 = 0;
}

/**
 * @brief Check if message display is complete and clean up.
 *
 * If the message-pending flag (0x20000) is set in the actor's flags
 * and the message state has reached 2 (complete), calls func_800B67F4
 * to finalize and clears the flag.
 *
 * @param actor Pointer to the actor (script context).
 */
void func_800B6854(Actor *actor) {
    if ((actor->context.flags & 0x20000) && actor->msgState == 2) {
        func_800B67F4(actor);
        actor->context.flags &= ~0x20000;
    }
}

/**
 * @brief Table index 0x04F handler — set message channel.
 *
 * Pops a channel value from the bytecode stack and stores it to both
 * moveSpeed and msgChannel fields of the actor.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_MSPEED(Actor *actor) {
    u16 channel = POP(&actor->context);

    actor->moveSpeed = channel;
    actor->msgChannel = channel;
    return 2;
}

/**
 * @brief MOVE opcode 0x3E handler — start entity movement to a target position.
 *
 * Checks if the entity is active, then pops a window-slot index and three
 * 20.12 fixed-point coordinates (Z, X, Y target) from the bytecode stack
 * and stores them into the actor's target-position fields. Calls
 * func_800B6738 to dispatch the motion command (cmd 0xD) to the entity
 * via func_800B912C. Yields each frame until the motion completes
 * (msgState == 2), at which point func_800B67F4 finalizes.
 *
 * @note Previously misnamed @c opHandler_MES — the three @c <<12 pops are
 *       fixed-point coordinates, not message text/positions; the actor
 *       fields named @c msgPos[XY] / @c msgTextPtr actually hold motion
 *       target coords (see also opHandler_MOVEA which copies them from
 *       @c D_80085230[idx]->pos[XYZ]).
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 if motion is complete, 1 otherwise (yield).
 */
s32 opHandler_MOVE(Actor *actor) {
    s32 new_var;
    u16 saved;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        saved = actor->msgChannel;
        actor->msgActive = 1;
        actor->msgState = 0;

        actor->windowId = POP(&actor->context);
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        new_var = POP(&actor->context);
        actor->moveSpeed = saved;
        actor->msgTextPtr = new_var << 12;

        func_800B6738(actor);
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        return 2;
    }
    return 1;
}

/**
 * @brief Show a message positioned at a map entity's location.
 *
 * On the first pass (entity active): pops window ID, saves channel,
 * calls func_800B6738 to init display. On subsequent ticks: reads
 * the entity's position from D_80085230[PEEK] and updates the message
 * coordinates each frame. When message state reaches 2, finalizes
 * and drops the entity index from the stack.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 opHandler_MOVEA(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->windowId = POP(&actor->context);
        actor->moveSpeed = actor->msgChannel;
        func_800B6738(actor);
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        actor->context.stackPtr--;
        return 2;
    }

    actor->msgTextPtr = D_80085230[PEEK(&actor->context)]->posX;
    actor->msgPosX = D_80085230[PEEK(&actor->context)]->posY;
    actor->msgPosY = D_80085230[PEEK(&actor->context)]->posZ;
    return 1;
}

/**
 * @brief Show a message tracking a party member entity's position.
 *
 * Like opHandler_MOVEA but looks up the entity index through
 * g_fieldVars->memberSlot[] first, then reads position from
 * D_80085224[idx]. Updates message coordinates each frame.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 opHandler_PMOVEA(Actor *actor) {
    u8 idx;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->windowId = POP(&actor->context);
        actor->moveSpeed = actor->msgChannel;
        func_800B6738(actor);
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        actor->context.stackPtr--;
        return 2;
    }

    idx = g_fieldVars->memberSlot[PEEK(&actor->context)];
    actor->msgTextPtr = D_80085224[idx].posX;
    actor->msgPosX = D_80085224[idx].posY;
    actor->msgPosY = D_80085224[idx].posZ;
    return 1;
}

/**
 * @brief ASK opcode handler — display a yes/no prompt.
 *
 * Sets up the message display with position and window ID from
 * the bytecode stack, initializes the prompt state fields,
 * then polls for completion.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while prompt is active, 2 when answered.
 */
s32 opHandler_CMOVE(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->windowId = POP(&actor->context);
        actor->moveSpeed = actor->msgChannel;
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x262 = 0;
        actor->field_0x240 = 1;
        actor->field_0x1DA = 0;
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        return 2;
    }

    return 1;
}

/**
 * @brief Display a positioned message (variant without yes/no prompt setup).
 *
 * Like opHandler_CMOVE (ASK prompt) but without the field_0x262/field_0x240
 * initialization. Pops window ID, Y position, X position, and message text
 * pointer from the bytecode stack; saves the current message channel;
 * resets message state. Then polls for completion.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 opHandler_FMOVE(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->windowId = POP(&actor->context);
        actor->moveSpeed = actor->msgChannel;
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1DA = 0;
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        return 2;
    }

    return 1;
}

/**
 * @brief Position-tracking message variant (clears field_0x1DA, no display init).
 *
 * Like opHandler_MOVEA but clears field_0x1DA instead of calling func_800B6738.
 * Pops window ID and saves the channel on the first pass; afterwards, updates
 * the message coordinates from D_80085230[PEEK]'s position each frame.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 opHandler_FMOVEA(Actor *actor) {
    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->windowId = POP(&actor->context);
        actor->moveSpeed = actor->msgChannel;
        actor->field_0x1DA = 0;
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        actor->context.stackPtr--;
        return 2;
    }

    actor->msgTextPtr = D_80085230[PEEK(&actor->context)]->posX;
    actor->msgPosX = D_80085230[PEEK(&actor->context)]->posY;
    actor->msgPosY = D_80085230[PEEK(&actor->context)]->posZ;
    return 1;
}

/**
 * @brief Party-member position-tracking message variant (clears field_0x1DA).
 *
 * Like opHandler_PMOVEA but clears field_0x1DA instead of calling func_800B6738.
 * Looks up a party slot through g_fieldVars->memberSlot[], then updates the
 * message coordinates from D_80085224[idx]'s position each frame.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 opHandler_FMOVEP(Actor *actor) {
    u8 idx;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->windowId = POP(&actor->context);
        actor->moveSpeed = actor->msgChannel;
        actor->field_0x1DA = 0;
    }

    if (actor->msgState == 2) {
        func_800B67F4(actor);
        actor->context.stackPtr--;
        return 2;
    }

    idx = g_fieldVars->memberSlot[PEEK(&actor->context)];
    actor->msgTextPtr = D_80085224[idx].posX;
    actor->msgPosX = D_80085224[idx].posY;
    actor->msgPosY = D_80085224[idx].posZ;
    return 1;
}

/**
 * @brief Pending-message MES variant — set flag 0x20000, init display, return 3.
 *
 * Unconditionally initializes the message with 4 stack values and sets the
 * pending-message flag (0x20000). Unlike opHandler_MOVE, there is no activeMask
 * guard or msgState==2 check — it schedules the display and yields with 3.
 *
 * @param actor Pointer to the actor (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 opHandler_RMOVE(Actor *actor) {
    s32 new_var;
    u16 saved;

    saved = actor->msgChannel;
    actor->msgActive = 1;
    actor->context.flags |= 0x20000;
    actor->msgState = 0;

    actor->windowId = POP(&actor->context);
    actor->msgPosY = POP(&actor->context) << 12;
    actor->msgPosX = POP(&actor->context) << 12;
    new_var = POP(&actor->context);
    actor->moveSpeed = saved;
    actor->msgTextPtr = new_var << 12;

    func_800B6738(actor);
    return 3;
}

/**
 * @brief Pending message variant with entity-position tracking.
 *
 * Sets the pending-message flag, pops a window ID, then reads initial message
 * coordinates from D_80085230[idx]'s position (two PEEKs + one POP for the
 * final Y). Returns 3 to indicate deferred completion.
 *
 * @param actor Pointer to the actor (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 opHandler_RMOVEA(Actor *actor) {
    actor->msgActive = 1;
    actor->context.flags |= 0x20000;
    actor->msgState = 0;
    actor->windowId = POP(&actor->context);
    actor->moveSpeed = actor->msgChannel;
    func_800B6738(actor);

    actor->msgTextPtr = D_80085230[PEEK(&actor->context)]->posX;
    actor->msgPosX = D_80085230[PEEK(&actor->context)]->posY;
    actor->msgPosY = D_80085230[POP(&actor->context)]->posZ;
    return 3;
}

/**
 * @brief Pending message variant with party-member position tracking.
 *
 * Sets the pending-message flag, pops a window ID, looks up a party slot
 * through g_fieldVars->memberSlot[POP], then reads initial coordinates from
 * D_80085224[idx]'s position. Returns 3 for deferred completion.
 *
 * @param actor Pointer to the actor (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 opHandler_RPMOVEA(Actor *actor) {
    u8 idx;

    actor->msgActive = 1;
    actor->context.flags |= 0x20000;
    actor->msgState = 0;
    actor->windowId = POP(&actor->context);
    actor->moveSpeed = actor->msgChannel;
    func_800B6738(actor);

    idx = g_fieldVars->memberSlot[POP(&actor->context)];
    actor->msgTextPtr = D_80085224[idx].posX;
    actor->msgPosX = D_80085224[idx].posY;
    actor->msgPosY = D_80085224[idx].posZ;
    return 3;
}

/**
 * @brief Pending ASK-prompt variant — leaf function, all 4 POPs, returns 3.
 *
 * Sets the pending-message flag and pops window ID, Y, X, and text pointer
 * from the stack. Also initializes the prompt-specific state fields
 * (field_0x262=0, field_0x240=1, field_0x1DA=0) like opHandler_CMOVE, but
 * unconditionally (no activeMask guard) and returns 3.
 *
 * @param actor Pointer to the actor (script context).
 * @return 3 (prompt scheduled, deferred completion).
 */
s32 opHandler_RCMOVE(Actor *actor) {
    actor->msgActive = 1;
    actor->context.flags |= 0x20000;
    actor->msgState = 0;
    actor->windowId = POP(&actor->context);
    actor->moveSpeed = actor->msgChannel;
    actor->msgPosY = POP(&actor->context) << 12;
    actor->msgPosX = POP(&actor->context) << 12;
    actor->msgTextPtr = POP(&actor->context) << 12;
    actor->field_0x1DA = 0;
    actor->field_0x262 = 0;
    actor->field_0x240 = 1;
    return 3;
}

/**
 * @brief Pending MES variant — leaf function, all 4 POPs, returns 3.
 *
 * Sets the pending-message flag and pops window ID, Y, X, and text pointer
 * from the stack. Returns 3 immediately (no display init call).
 *
 * @param actor Pointer to the actor (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 opHandler_RFMOVE(Actor *actor) {
    actor->msgActive = 1;
    actor->context.flags |= 0x20000;
    actor->msgState = 0;
    actor->windowId = POP(&actor->context);
    actor->moveSpeed = actor->msgChannel;
    actor->msgPosY = POP(&actor->context) << 12;
    actor->msgPosX = POP(&actor->context) << 12;
    actor->msgTextPtr = POP(&actor->context) << 12;
    actor->field_0x1DA = 0;
    return 3;
}

/**
 * @brief Poll whether the current message has finished displaying.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 if msgState == 2 (message complete), 1 otherwise (still displaying).
 */
s32 opHandler_MOVESYNC(Actor *actor) {
    if (actor->msgState == 2) {
        return 2;
    }
    return 1;
}

/**
 * @brief Clean up message state for a referenced entity (pops its index).
 *
 * Looks up an entity from D_80085230[POP], and if the pending-message flag
 * (0x10000000) is set and the entity's message is currently active, clears
 * it, tears down the display via func_800B912C, and repurposes the flags
 * (clear 0xF800 range, set 0x2000).
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_MOVECANCEL(ScriptContext *context) {
    s32 idx = POP(context);

    if (D_80085230[idx]->context.flags & 0x10000000) {
        if (D_80085230[idx]->msgActive != 1) {
            return 2;
        }
        D_80085230[idx]->msgActive = 0;
        func_800B912C(D_80085230[idx], D_80085230[idx]->field_0x24F);
        D_80085230[idx]->context.flags &= 0xFFFF07FF;
        D_80085230[idx]->context.flags |= 0x2000;
    }
    return 2;
}

/**
 * @brief Clean up message state for a referenced party-member entity.
 *
 * Pops a party slot, looks up the entity through g_fieldVars->memberSlot[],
 * then (if the message is currently active) clears it, tears down the display,
 * and marks the flags (clear 0xF800 range and set 0x2000).
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_PMOVECANCEL(ScriptContext *context) {
    u8 idx = g_fieldVars->memberSlot[POP(context)];

    if (D_80085224[idx].msgActive == 1) {
        D_80085224[idx].msgActive = 0;
        func_800B912C(&D_80085224[idx], D_80085224[idx].field_0x24F);
        D_80085224[idx].context.flags = (D_80085224[idx].context.flags & 0xFFFF07FF) | 0x2000;
    }
    return 2;
}

/**
 * @brief Clear the 0x10000 flag bit if the entity is active.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 (continue processing).
 */
s32 opHandler_MOVEFLUSH(ScriptContext *context) {
    if ((context->activeMask >> context->scriptSlot) & 1) {
        context->flags &= ~0x10000;
    }
    return 1;
}

/**
 * @brief Pop a byte off the stack into field_0x262.
 *
 * @param actor Pointer to the actor (script context).
 * @return 2 (continue processing).
 */
s32 opHandler_MLIMIT(Actor *actor) {
    actor->field_0x262 = POP_BYTE(&actor->context);
    return 2;
}

/**
 * @brief 2D distance from the saved message position to the entity's current position.
 *
 * Both coordinate pairs are in fixed-point (Q20.12). The per-axis deltas
 * are truncated toward zero via signed divide-by-4096 before being squared.
 * Used by the scrolling-message handler to scale the channel advance by
 * how far the entity has travelled since the message started.
 *
 * @param actor Pointer to the actor (script context).
 * @return Integer distance between the two positions.
 */
s32 func_800B76A4(Actor *actor) {
    s32 dx = (actor->msgTextPtr - actor->posX) / 4096;
    s32 dy = (actor->msgPosX - actor->posY) / 4096;
    dx = dx * dx;
    dy = dy * dy;
    return SquareRoot0(dx + dy);
}

/**
 * @brief Scrolling positioned-message handler with per-frame channel advance.
 *
 * On the first pass, pops 5 values (field_0x204, window, Y, X, text pointer),
 * initializes display state, and sets up prompt flags. Each subsequent frame,
 * advances the sound channel toward the target (field_0x204) at a rate scaled
 * by the distance travelled (via func_800B76A4) and the global tempo
 * D_800704B2. When the message completes, saves the current channel to
 * field_0x202 and finalizes via func_800B67F4.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while message is animating, 2 when complete.
 */
s32 opHandler_MACCEL(Actor *actor) {
    s32 delta = 0;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        actor->msgActive = 1;
        actor->msgState = 0;
        actor->moveSpeed = actor->msgChannel;
        actor->field_0x204 = POP(&actor->context);
        actor->windowId = POP(&actor->context);
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x262 = 0;
        actor->field_0x240 = 1;
    }

    if (actor->msgState == 2) {
        actor->field_0x202 = actor->moveSpeed;
        func_800B67F4(actor);
        return 2;
    }

    delta = (s32)(actor->field_0x204 - actor->moveSpeed) * D_800704B2;
    delta = delta / func_800B76A4(actor);
    actor->moveSpeed += delta;
    func_800B6738(actor);
    return 1;
}

/**
 * @brief Start a proximity-triggered message anchored at another entity.
 *
 * Computes the planar distance from self to target (Q20.12 fixed-point),
 * then picks one of two sound-channel scalings and one of two script-param
 * byte offsets (field_0x250 vs field_0x251) based on whether the squared
 * distance exceeds 0x3F47F (~509 units). Issues the chosen command via
 * func_800B912C, primes the message at the target's position, and sets
 * flag 0x2001.
 *
 * @param self Pointer to the actor (script context).
 * @param target The entity whose position anchors the message.
 */
void func_800B788C(Actor *self, Actor *target) {
    s32 dx, dy, distSq;

    dx = (target->posX - self->posX) / 4096;
    dy = (target->posY - self->posY) / 4096;
    dx = dx * dx;
    dy = dy * dy;
    dx = dx + dy;
    distSq = dx;

    if (distSq > 0x3F47F) {
        self->moveSpeed = (u32)(D_800704B2 * 25375) >> 6;
        func_800B912C(self, self->field_0x251);
    } else {
        self->moveSpeed = (u32)(D_800704B2 * 17255) >> 7;
        func_800B912C(self, self->field_0x250);
    }
    self->msgActive = 1;
    self->msgState = 0;
    self->windowId = 0x60;
    self->msgTextPtr = target->posX;
    self->msgPosX = target->posY;
    self->msgPosY = target->posZ;
    self->context.flags |= 0x2001;
}

/**
 * @brief Proximity-anchored message from the OTHER two party members.
 *
 * On the active frame, locates the party slot whose @c memberSlot
 * matches @c actor->field_0x255 (the "speaker"), then sets up
 * proximity messages from the other two slots back at the speaker
 * via @c func_800B788C. Saves the chosen entity pointers in
 * @c D_800DE4F0 / @c D_800DE4F4, kicks off the speaker's own animation
 * via @c func_800B912C, and marks flag @c 0x2000.
 *
 * On subsequent inactive frames, waits for both saved entities to
 * reach @c msgState == 2 with a fresh field_0x24E/24F mismatch — for
 * each, triggers @c func_800B912C and sets flag @c 0x8000. Once both
 * have completed, calls @c func_8009ECA4 and clears msgActive + flag
 * bit @c 0x1 on each.
 *
 * @param actor Script context (the speaker).
 * @return 1 while waiting, 2 once both other-party messages finish.
 */
s32 opHandler_JOIN(Actor *actor) {
    s32 i;
    s32 idx;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        for (i = 0; i < 3; i++) {
            if (g_gameState.battleParty[i] == actor->field_0x255) {
                break;
            }
        }

        D_800DE4F8 = 0;
        D_800DE4F4 = 0;
        D_800DE4F0 = 0;

        switch (i) {
        case 0:
            if (g_fieldVars->memberSlot[1] != 0xFF) {
                idx = g_fieldVars->memberSlot[1];
                D_800DE4F0 = &D_80085224[idx];
                func_800B788C(&D_80085224[idx], actor);
            }
            if (g_fieldVars->memberSlot[2] != 0xFF) {
                idx = g_fieldVars->memberSlot[2];
                D_800DE4F4 = &D_80085224[idx];
                func_800B788C(&D_80085224[idx], actor);
            }
            break;
        case 1:
            if (g_fieldVars->memberSlot[0] != 0xFF) {
                idx = g_fieldVars->memberSlot[0];
                D_800DE4F0 = &D_80085224[idx];
                func_800B788C(&D_80085224[idx], actor);
            }
            if (g_fieldVars->memberSlot[2] != 0xFF) {
                idx = g_fieldVars->memberSlot[2];
                D_800DE4F4 = &D_80085224[idx];
                func_800B788C(&D_80085224[idx], actor);
            }
            break;
        case 2:
            if (g_fieldVars->memberSlot[0] != 0xFF) {
                idx = g_fieldVars->memberSlot[0];
                D_800DE4F0 = &D_80085224[idx];
                func_800B788C(&D_80085224[idx], actor);
            }
            if (g_fieldVars->memberSlot[1] != 0xFF) {
                idx = g_fieldVars->memberSlot[1];
                D_800DE4F4 = &D_80085224[idx];
                func_800B788C(&D_80085224[idx], actor);
            }
            break;
        }

        func_800B912C(actor, actor->field_0x24F);
        actor->context.flags |= 0x2000;
    } else {
        if (D_800DE4F0 != NULL && D_800DE4F0->msgState == 2
                && D_800DE4F0->field_0x24E != D_800DE4F0->field_0x24F) {
            func_800B912C(D_800DE4F0, D_800DE4F0->field_0x24F);
            D_800DE4F0->context.flags |= 0x8000;
        }
        if (D_800DE4F4 != NULL && D_800DE4F4->msgState == 2
                && D_800DE4F4->field_0x24E != D_800DE4F4->field_0x24F) {
            func_800B912C(D_800DE4F4, D_800DE4F4->field_0x24F);
            D_800DE4F4->context.flags |= 0x8000;
        }

        if (D_800DE4F0 == NULL || D_800DE4F0->msgState == 2) {
            if (D_800DE4F4 == NULL || D_800DE4F4->msgState == 2) {
                func_8009ECA4();
                if (D_800DE4F0) {
                    D_800DE4F0->context.flags &= ~1;
                    D_800DE4F0->msgActive = 0;
                }
                if (D_800DE4F4) {
                    D_800DE4F4->context.flags &= ~1;
                    D_800DE4F4->msgActive = 0;
                }
                return 2;
            }
        }
    }
    return 1;
}

/**
 * @brief Start a proximity-anchored message at an explicit XYZ position.
 *
 * Sister of @c func_800B788C, but takes the anchor coordinates directly
 * as args rather than via a target entity. Computes the planar distance
 * from @c actor to @c (x, y) (Q20.12 fixed-point), picks one of two
 * sound-channel scalings and one of two script-param byte offsets
 * (field_0x251 for "far" ≥ ~509 units, field_0x250 for "near") based on
 * the squared distance threshold @c 0x3F47F. Issues the chosen command
 * via @c func_800B912C, primes the message at the supplied position,
 * and sets flag @c 0x2000. Window ID is fixed to @c 8.
 *
 * @param actor Script context.
 * @param x     Anchor X (Q19.12 fixed-point).
 * @param y     Anchor Y (Q19.12 fixed-point).
 * @param z     Anchor Z (Q19.12 fixed-point).
 */
void func_800B7D44(Actor *actor, s32 x, s32 y, s32 z) {
    s32 dx, dy, distSq;

    dx = (x - actor->posX) / 4096;
    dy = (y - actor->posY) / 4096;
    dx = dx * dx;
    dy = dy * dy;
    dx = dx + dy;
    distSq = dx;

    if (distSq > 0x3F47F) {
        actor->moveSpeed = (u32)(D_800704B2 * 25375) >> 6;
        func_800B912C(actor, actor->field_0x251);
    } else {
        actor->moveSpeed = (u32)(D_800704B2 * 17255) >> 7;
        func_800B912C(actor, actor->field_0x250);
    }

    actor->msgActive = 1;
    actor->msgState = 0;
    actor->windowId = 8;
    actor->msgTextPtr = x;
    actor->msgPosX = y;
    actor->msgPosY = z;
    actor->context.flags |= 0x2000;
}

/**
 * @brief 3-party walk opcode handler.
 *
 * Pops 9 stack values (3 sets of XYZ coordinates), shifts each into Q19.12
 * fixed-point, and dispatches movement (via func_800B7D44) to up to 3 active
 * party-member entities looked up through g_fieldVars->memberSlot[0..2].
 * Sets the movement-active flag (bit 0) on each non-self entity. On
 * subsequent ticks (when the activeMask bit is clear), waits for all
 * dispatched entities to reach msgState == 2 (movement complete), then
 * clears msgActive and the active flag and returns 2.
 *
 * @param actor Pointer to the actor (script context).
 * @return 1 while still moving, 2 when all entities have completed.
 */
s32 opHandler_SPLIT(Actor *actor) {
    s32 z2, y2, x2, z1, y1, x1, z0, y0, x0;
    Actor *e0, *e4, *e8;

    if ((actor->context.activeMask >> actor->context.scriptSlot) & 1) {
        D_800DE4F8 = 0;
        D_800DE4F4 = 0;
        D_800DE4F0 = 0;

        z2 = POP(&actor->context) << 12;
        y2 = POP(&actor->context) << 12;
        x2 = POP(&actor->context) << 12;
        z1 = POP(&actor->context) << 12;
        y1 = POP(&actor->context) << 12;
        x1 = POP(&actor->context) << 12;
        z0 = POP(&actor->context) << 12;
        y0 = POP(&actor->context) << 12;
        x0 = POP(&actor->context) << 12;

        if (g_fieldVars->memberSlot[0] != 0xFF) {
            D_800DE4F0 = &D_80085224[g_fieldVars->memberSlot[0]];
            func_800B7D44(D_800DE4F0, x0, y0, z0);
            if (g_fieldVars->memberSlot[0] != actor->field_0x256) {
                D_800DE4F0->context.flags |= 1;
            }
        }

        if (g_fieldVars->memberSlot[1] != 0xFF) {
            D_800DE4F4 = &D_80085224[g_fieldVars->memberSlot[1]];
            func_800B7D44(D_800DE4F4, x1, y1, z1);
            if (g_fieldVars->memberSlot[1] != actor->field_0x256) {
                D_800DE4F4->context.flags |= 1;
            }
        }

        if (g_fieldVars->memberSlot[2] != 0xFF) {
            D_800DE4F8 = &D_80085224[g_fieldVars->memberSlot[2]];
            func_800B7D44(D_800DE4F8, x2, y2, z2);
            if (g_fieldVars->memberSlot[2] != actor->field_0x256) {
                D_800DE4F8->context.flags |= 1;
            }
        }

        return 1;
    }

    if (D_800DE4F0->msgState == 2 && D_800DE4F0->field_0x24E != D_800DE4F0->field_0x24F) {
        func_800B912C(D_800DE4F0, D_800DE4F0->field_0x24F);
        D_800DE4F0->context.flags |= 0x8000;
    }
    if (D_800DE4F4->msgState == 2 && D_800DE4F4->field_0x24E != D_800DE4F4->field_0x24F) {
        func_800B912C(D_800DE4F4, D_800DE4F4->field_0x24F);
        D_800DE4F4->context.flags |= 0x8000;
    }
    if (D_800DE4F8->msgState == 2 && D_800DE4F8->field_0x24E != D_800DE4F8->field_0x24F) {
        func_800B912C(D_800DE4F8, D_800DE4F8->field_0x24F);
        D_800DE4F8->context.flags |= 0x8000;
    }

    if (D_800DE4F0 == 0 || D_800DE4F0->msgState == 2) {
        if (D_800DE4F4 == 0 || D_800DE4F4->msgState == 2) {
            if (D_800DE4F8 == 0 || D_800DE4F8->msgState == 2) {
                e0 = D_800DE4F0;
                if (e0 != 0) {
                    e0->msgActive = 0;
                    e0->context.flags &= ~1;
                }
                e4 = D_800DE4F4;
                if (e4 != 0) {
                    e4->msgActive = 0;
                    e4->context.flags &= ~1;
                }
                e8 = D_800DE4F8;
                if (e8 != 0) {
                    e8->msgActive = 0;
                    e8->context.flags &= ~1;
                }
                return 2;
            }
        }
    }
    return 1;
}

/**
 * @brief Positioned-message opcode handler with stored arg.
 *
 * On the active frame, starts a new positioned message: writes
 * @c msgActive = 2, @c msgState = 0, then pops three values from the
 * bytecode stack — a halfword (window/format ID, stored to
 * @c field_0x1D8), and two s32 coords that are scaled into Q19.12 and
 * written to @c msgPosX and @c msgTextPtr. Finally stores the
 * dispatcher arg into @c field_0x1FC.
 *
 * On subsequent inactive frames, waits for @c msgState == 2; once
 * complete, clears @c msgActive and returns 3 (skip-to-next-entity).
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 1 while running, 3 once the message has been read.
 */
s32 opHandler_JUMP(Actor *actor, s32 a1) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState != 2) {
            return 1;
        }
        actor->msgActive = 0;
        return 3;
    }
    do {
        actor->msgActive = 2;
        actor->msgState = 0;
        actor->field_0x1D8 = POP(&actor->context);
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1FC = a1;
    } while (0);
    return 1;
}

/**
 * @brief Positioned-message opcode handler — 4-pop variant (full XYZ).
 *
 * Sister of @c opHandler_JUMP but pops an extra coordinate: pops one
 * halfword (window/format ID, stored to @c field_0x1D8) plus three
 * s32 values that become @c msgPosY, @c msgPosX, and @c msgTextPtr
 * (each scaled into Q19.12). Stores the dispatcher arg into
 * @c field_0x1FC. Same inactive-frame contract: wait for
 * @c msgState == 2 then clear @c msgActive and return 3.
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 1 while running, 3 once the message has been read.
 */
s32 opHandler_JUMP3(Actor *actor, s32 a1) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState != 2) {
            return 1;
        }
        actor->msgActive = 0;
        return 3;
    }
    do {
        actor->msgActive = 2;
        actor->msgState = 0;
        actor->field_0x1D8 = POP(&actor->context);
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1FC = a1;
    } while (0);
    return 1;
}

/**
 * @brief Positioned-message opcode handler — anchor at a party member.
 *
 * Variant of the @c opHandler_JUMP / @c opHandler_JUMP3 pattern that
 * copies the anchor coordinates from a party-member entity instead
 * of taking them as args. Pops a window/format halfword (stored to
 * @c field_0x1D8) and a slot index, looks up the entity through
 * @c g_fieldVars->memberSlot[], and copies the entity's @c posX /
 * @c posY / @c posZ to @c msgTextPtr / @c msgPosX / @c msgPosY plus
 * @c triIdx to @c field_0x1FC.
 *
 * @param actor Script context.
 * @return 1 while running, 3 once the message has been read.
 */
s32 opHandler_PJUMPA(Actor *actor) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState != 2) {
            return 1;
        }
        actor->msgActive = 0;
        return 3;
    }
    do {
        s32 m;
        actor->msgActive = 2;
        actor->msgState = 0;
        actor->field_0x1D8 = POP(&actor->context);
        m = g_fieldVars->memberSlot[POP(&actor->context)];
        actor->msgTextPtr = D_80085224[m].posX;
        actor->msgPosX = D_80085224[m].posY;
        actor->msgPosY = D_80085224[m].posZ;
        actor->field_0x1FC = D_80085224[m].triIdx;
    } while (0);
    return 1;
}

/**
 * @brief Pop a word from the bytecode stack and store its low byte to
 *        @c g_fieldEntity.unk1AE.
 *
 * The @c volatile cast on the load is required to prevent gcc 2.7.2
 * from narrowing the @c lw to @c lbu — the target reads a full s32
 * even though only the low byte is stored.
 *
 * @param actor Script context.
 * @return 2 (advance PC).
 */
s32 opHandler_COUNTERCLOCKWISETURN2(ScriptContext *context) {
    g_fieldEntity.unk1AE = *(volatile s32 *)&POP(context);
    return 2;
}

/**
 * @brief Positioned-message opcode handler — variant with animation
 *        kick and "msgActive = 3" semantics.
 *
 * On the active frame: sets @c msgActive = 3 (distinct from the
 * @c msgActive = 2 variants in @c opHandler_JUMP / @c opHandler_JUMP3),
 * @c windowId = 1, @c msgState = 0; pops a signed halfword and
 * dispatches @c func_800B912C with it (animation trigger); marks
 * @c flags |= 0x2000; pops three coords (Q19.12) into @c msgPosY /
 * @c msgPosX / @c msgTextPtr; stores the opcode arg into @c field_0x1FC.
 *
 * On inactive frames: waits for @c msgState == 2 and returns 2 once
 * the message has been read (no return 3 here — the message stays
 * available rather than skipping the entity).
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 1 while running, 2 once read.
 */
s32 opHandler_LADDERUP(Actor *actor, s32 a1) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState == 2) {
            actor->msgActive = 0;
            return 2;
        }
    } else {
        actor->msgActive = 3;
        actor->windowId = 1;
        actor->msgState = 0;
        func_800B912C(actor, (s16)POP(&actor->context));
        actor->context.flags |= 0x2000;
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1FC = a1;
    }
    return 1;
}

/**
 * @brief Positioned-message handler — windowId=0, msgActive=3 variant
 *        with animation-trigger call.
 *
 * Sister of @c opHandler_LADDERUP with @c windowId = 0 instead of 1.
 * Active path: sets @c msgActive=3 / @c windowId=0 / @c msgState=0,
 * pops a signed halfword and dispatches @c func_800B912C with it
 * (animation trigger), marks @c flags |= 0x2000, pops three Q19.12
 * coords into @c msgPosY / @c msgPosX / @c msgTextPtr, stores the
 * dispatcher arg into @c field_0x1FC. Inactive path: waits for
 * @c msgState == 2 then clears @c msgActive and returns 2.
 *
 * Uses the @c if/else (not @c do{}while(0)) form to get gcc 2.7.2 to
 * allocate @c v0 for the stackPtr load — the do-while wrapper that
 * works for the sibling handlers triggers a v0/v1 swap here because
 * the bnez delay slot is taken by @c move @c s1, @c a1 (saving the
 * arg across the @c func_800B912C call), forcing the @c msgActive
 * constant load into the active block where it competes with the
 * stackPtr load. The @c if/else structure keeps both paths in
 * separate basic blocks so gcc allocates @c v0 to the more-used
 * stackPtr.
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 1 while running, 2 once read.
 */
s32 opHandler_LADDERDOWN(Actor *actor, s32 a1) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState == 2) {
            actor->msgActive = 0;
            return 2;
        }
    } else {
        actor->msgActive = 3;
        actor->windowId = 0;
        actor->msgState = 0;
        func_800B912C(actor, (s16)POP(&actor->context));
        actor->context.flags |= 0x2000;
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1FC = a1;
    }
    return 1;
}

/**
 * @brief 9-pop positioned-message handler — sets up message position
 *        plus three "saved" coords and three additional unk slots.
 *
 * On the active frame: @c msgActive = 4 (distinct from sibling
 * handlers), @c windowId = 1, @c msgState = 0; then pops nine s32
 * values, each scaled into Q19.12, into a contiguous region of the
 * Actor:
 *   - @c msgPosY  / @c msgPosX  / @c msgTextPtr   (current msg coords)
 *   - @c field_0x1C8 / @c field_0x1C4 / @c field_0x1C0 (saved msg coords)
 *   - @c unk1B0 / @c unk1AC / @c unk1A8 (three extra slots)
 * and finally stores the dispatcher arg into @c field_0x1FC.
 *
 * On inactive frames: wait for @c msgState == 2 and return 2 once
 * the message has been read.
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 1 while running, 2 once read.
 */
s32 opHandler_LADDERUP2(Actor *actor, s32 a1) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState == 2) {
            actor->msgActive = 0;
            return 2;
        }
    } else {
        actor->msgActive = 4;
        actor->windowId = 1;
        actor->msgState = 0;
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1C8 = POP(&actor->context) << 12;
        actor->field_0x1C4 = POP(&actor->context) << 12;
        actor->field_0x1C0 = POP(&actor->context) << 12;
        actor->unk1B0 = POP(&actor->context) << 12;
        actor->unk1AC = POP(&actor->context) << 12;
        actor->unk1A8 = POP(&actor->context) << 12;
        actor->field_0x1FC = a1;
    }
    return 1;
}

/**
 * @brief 9-pop full-message handler (msgActive=4, windowId=0).
 *
 * Leaf variant of @c opHandler_LADDERUP2: same 9-pop sequence into the
 * @c msgPosY / @c msgPosX / @c msgTextPtr / @c field_0x1C0..C8 /
 * @c unk1A8..B0 region with the same @c msgActive = 4 marker, but
 * @c windowId = 0 instead of 1.
 *
 * Matching scaffold for gcc 2.7.2: hoisting the active-mask shift
 * into a @c u16 local (@c active_mask) and pre-loading the
 * @c msgActive constant @c 4 into an @c s32 local (@c new_var) before
 * the @c if check forces gcc to pick @c v1 for the constant rather
 * than @c a2 — without this scaffolding the stackPtr load also
 * gets allocated to the wrong register (v0/v1 swap).
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 1 while running, 2 once read.
 */
s32 opHandler_LADDERDOWN2(Actor *actor, s32 a1) {
    if (!((actor->context.activeMask >> actor->context.scriptSlot) & 1)) {
        if (actor->msgState == 2) {
            actor->msgActive = 0;
            return 2;
        }
    } else {
        actor->msgActive = 4;
        actor->windowId = 0;
        actor->msgState = 0;
        actor->msgPosY = POP(&actor->context) << 12;
        actor->msgPosX = POP(&actor->context) << 12;
        actor->msgTextPtr = POP(&actor->context) << 12;
        actor->field_0x1C8 = POP(&actor->context) << 12;
        actor->field_0x1C4 = POP(&actor->context) << 12;
        actor->field_0x1C0 = POP(&actor->context) << 12;
        actor->unk1B0 = POP(&actor->context) << 12;
        actor->unk1AC = POP(&actor->context) << 12;
        actor->unk1A8 = POP(&actor->context) << 12;
        actor->field_0x1FC = a1;
    }
    return 1;
}

/**
 * @brief Pop three halfwords and broadcast each to a pair of fields.
 *
 * Pops three u16 values from the bytecode stack and stores each one to
 * two adjacent halfword fields (current+target pattern for animation
 * interpolation): @c (1EC,1EE), @c (1E6,1E8), and @c (1E0,1E2). Also
 * stores the dispatcher arg into @c field_0x1FC, clears @c unk245 /
 * @c msgState / @c field_0x1F4. Leaf function.
 *
 * @param actor Script context.
 * @param a1    Opcode argument (stored as halfword to field_0x1FC).
 * @return 2 (advance PC).
 */
s32 opHandler_DOFFSET(Actor *actor, s32 a1) {
    u16 a, b, c;
    a = POP(&actor->context);
    actor->posOfsZ = a;
    actor->field_0x1EE = a;
    b = POP(&actor->context);
    actor->posOfsY = b;
    actor->field_0x1E8 = b;
    c = POP(&actor->context);
    actor->field_0x1FC = a1;
    actor->unk245 = 0;
    actor->msgState = 0;
    actor->field_0x1F4 = 0;
    actor->posOfsX = c;
    actor->field_0x1E2 = c;
    return 2;
}

/**
 * @brief Pop seven halfwords into animation fields with @c unk245 = 1.
 *
 * Sister of @c opHandler_DOFFSET but with one more parameter direction:
 * sets @c unk245 = 1, pops seven u16 values into @c field_0x1F2,
 * @c 1F0, @c 1EA, @c 1E4, @c 1EE, @c 1E8, @c 1E2 (last), and zeros
 * @c field_0x1F4. Leaf function.
 *
 * @param actor Script context.
 * @param a1    Ignored.
 * @return 2 (advance PC).
 */
s32 opHandler_LOFFSETS(Actor *actor, s32 a1) {
    actor->unk245 = 1;
    actor->field_0x1F2 = POP(&actor->context);
    actor->field_0x1F0 = POP(&actor->context);
    actor->field_0x1EA = POP(&actor->context);
    actor->field_0x1E4 = POP(&actor->context);
    actor->field_0x1EE = POP(&actor->context);
    actor->field_0x1E8 = POP(&actor->context);
    actor->field_0x1E2 = POP(&actor->context);
    actor->field_0x1F4 = 0;
    return 2;
}

/**
 * @brief Pop seven halfwords into animation fields with @c unk245 = 2.
 *
 * Identical to @c opHandler_LOFFSETS except @c unk245 is set to @c 2 (a
 * different sub-mode marker). Same pop sequence and field layout.
 *
 * @param actor Script context.
 * @param a1    Ignored.
 * @return 2 (advance PC).
 */
s32 opHandler_COFFSETS(Actor *actor, s32 a1) {
    actor->unk245 = 2;
    actor->field_0x1F2 = POP(&actor->context);
    actor->field_0x1F0 = POP(&actor->context);
    actor->field_0x1EA = POP(&actor->context);
    actor->field_0x1E4 = POP(&actor->context);
    actor->field_0x1EE = POP(&actor->context);
    actor->field_0x1E8 = POP(&actor->context);
    actor->field_0x1E2 = POP(&actor->context);
    actor->field_0x1F4 = 0;
    return 2;
}

/**
 * @brief Animation handler — rotate current XY/Z fields into "saved"
 *        slots and pop new values in (unk245 = 1 variant).
 *
 * Sister of @c opHandler_COFFSET with @c unk245 = 1. Same shape and
 * matching scaffold — see @c opHandler_COFFSET for the reg-alloc trick.
 *
 * @param actor Script context.
 * @param a1    Ignored.
 * @return 2 (advance PC).
 */
s32 opHandler_LOFFSET(Actor *actor, s32 a1) {
    s32 s1E4 = actor->field_0x1E4;
    s32 s1F0 = actor->field_0x1F0;
    s32 s1EA;
    actor->unk245 = 1;
    actor->field_0x1F2 = POP(&actor->context);
    s1EA = actor->field_0x1EA;
    actor->field_0x1E2 = s1E4;
    actor->field_0x1EE = s1F0;
    actor->field_0x1E8 = s1EA;
    s1EA = (s8)(actor->context.stackPtr--);
    actor->field_0x1F0 = ((s32 *)actor)[s1EA];
    s1F0 = (s8)(actor->context.stackPtr--);
    actor->field_0x1EA = ((s32 *)actor)[s1F0];
    actor->field_0x1E4 = POP(&actor->context);
    actor->field_0x1F4 = 0;
    return 2;
}

/**
 * @brief Animation handler — rotate current XY/Z fields into "saved"
 *        slots and pop new values in.
 *
 * Sister of @c opHandler_LOFFSET (which uses @c unk245 = 1). Pre-reads
 * the current @c field_0x1E4 / @c 1F0 / @c 1EA into temporaries,
 * stores them into the "saved" trio (@c 1E2 / @c 1EE / @c 1E8),
 * then pops four fresh values into @c 1F2 / @c 1F0 / @c 1EA /
 * @c 1E4 and clears @c field_0x1F4. Sets @c unk245 = 2.
 *
 * The middle two POPs are split into "capture stackPtr index, then
 * index the array" — this forces gcc 2.7.2 to keep @c a1 / @c a2 in
 * the order the target wants (a2 = saved_1E4, a1 = saved_1F0). The
 * normal inline POP form makes gcc swap them.
 *
 * @param actor Script context.
 * @param a1    Ignored.
 * @return 2 (advance PC).
 */
s32 opHandler_COFFSET(Actor *actor, s32 a1) {
    s32 s1E4 = actor->field_0x1E4;
    s32 s1F0 = actor->field_0x1F0;
    s32 s1EA;
    actor->unk245 = 2;
    actor->field_0x1F2 = POP(&actor->context);
    s1EA = actor->field_0x1EA;
    actor->field_0x1E2 = s1E4;
    actor->field_0x1EE = s1F0;
    actor->field_0x1E8 = s1EA;
    s1EA = (s8)(actor->context.stackPtr--);
    actor->field_0x1F0 = ((s32 *)actor)[s1EA];
    s1F0 = (s8)(actor->context.stackPtr--);
    actor->field_0x1EA = ((s32 *)actor)[s1F0];
    actor->field_0x1E4 = POP(&actor->context);
    actor->field_0x1F4 = 0;
    return 2;
}

/**
 * @brief Return 2 if @c unk245 == 3, otherwise return 1.
 *
 * Probes the animation sub-mode byte set by the sibling opcode
 * handlers (@c opHandler_LOFFSETS / @c opHandler_COFFSETS / @c opHandler_LOFFSET /
 * @c opHandler_COFFSET). Used by scripts to wait until the animation
 * reaches sub-mode 3.
 *
 * @param actor Script context.
 * @return 2 if @c unk245 == 3 (advance), 1 otherwise (yield).
 */
s32 opHandler_OFFSETSYNC(Actor *actor) {
    if (actor->unk245 == 3) {
        return 2;
    }
    return 1;
}

/**
 * @brief Raise the run-disable gate, so the player can only walk.
 *
 * @note Effect uncertain -- @c D_8007064C has no reader in decompiled code
 *       yet, so "run" is read off the opcode name rather than observed.
 *
 * @param context Context of the script running the opcode (unused).
 * @return 2 (continue processing).
 */
s32 opHandler_RUNDISABLE(ScriptContext *context) {
    D_8007064C = 1;
    return 2;
}

/**
 * @brief Clear the run-disable gate, restoring the player's run.
 *
 * Counterpart of @ref opHandler_RUNDISABLE; same uncertainty applies.
 *
 * @param context Context of the script running the opcode (unused).
 * @return 2 (continue processing).
 */
s32 opHandler_RUNENABLE(ScriptContext *context) {
    D_8007064C = 0;
    return 2;
}

/**
 * @brief Seed the follower trail from the player's current position.
 *
 * @ref func_8009E660 fills the whole waypoint ring in one go, so the
 * followers have somewhere to stand before any movement has been recorded.
 *
 * @param context Context of the script running the opcode (unused).
 * @return 2 (continue processing).
 */
s32 opHandler_INITTRACE(ScriptContext *context) {
    func_8009E660();
    return 2;
}

/**
 * @brief Wait until @c g_fieldEntity.unk106 catches up with @c unk104.
 *
 * Read by scripts to wait for the animation tick set by
 * @c opHandler_AXIS (POP → @c unk104, @c unk106 = 0) — some other
 * code path advances @c unk106 each frame, and this opcode yields
 * (return 1) until they're equal.
 *
 * @return 2 (advance) when @c unk104 == @c unk106, 1 (yield) otherwise.
 */
s32 opHandler_AXISSYNC(ScriptContext *context) {
    if (g_fieldEntity.unk106 == g_fieldEntity.unk104) {
        return 2;
    }
    return 1;
}

/**
 * @brief Pop two halfwords into g_fieldEntity.unk102/unk104; clear unk106.
 *
 * Writes the popped values into the SystemState block: first POP →
 * @c unk104, second POP → @c unk102. Also zeros @c unk106. Leaf
 * function.
 *
 * @param actor Script context.
 * @return 2 (advance PC).
 */
s32 opHandler_AXIS(ScriptContext *context) {
    g_fieldEntity.unk104 = POP(context);
    g_fieldEntity.unk102 = POP(context);
    g_fieldEntity.unk106 = 0;
    return 2;
}

/**
 * @brief Pop one byte from the bytecode stack and store it to
 *        @c actor->field_0x240.
 *
 * @param actor Script context.
 * @return 2 (advance PC).
 */
s32 opHandler_UNKNOWN4(Actor *actor) {
    actor->field_0x240 = POP_BYTE(&actor->context);
    return 2;
}

/**
 * @brief Animation/sound dispatch for the speaker's voice/SFX slot.
 *
 * Updates @c actor->context.flags by clearing bits @c 0x280000 and setting
 * bit @c 0x100000, then dispatches @c func_800A97E4 with the
 * @c field_0x256 byte as the slot/voice ID, command @c 0x2E, and
 * two zero args. Likely starts a queued voice/SFX cue.
 *
 * @param actor Script context.
 * @return 2 (advance PC).
 */
s32 opHandler_OPENEYES(Actor *actor) {
    actor->context.flags = (actor->context.flags & ~0x280000) | 0x100000;
    func_800A97E4(actor->field_0x256, 0x2E, 0, 0);
    return 2;
}
