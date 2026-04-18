#include "common.h"
#include "battle.h"
#include "gamestate.h"
#include "field.h"


/** @brief World map / field context pointed to by D_800562C4. */
typedef struct {
    /* 0x00 */ u8 pad000[0x58];
    /* 0x58 */ u8 field_0x58;
    /* 0x59 */ u8 pad059[0x0F];
    /* 0x68 */ s32 field_0x68;
    /* 0x6C */ s32 field_0x6C;
    /* 0x70 */ u8 pad070[0x46];
    /* 0xB6 */ u16 field_0xB6;
    /* 0xB8 */ u8 pad0B8[0x0A];
    /* 0xC2 */ u8 entityLookup[4];  /**< Maps party slot → entity index. */
    /* 0xC6 */ u8 pad0C6[0x03];
    /* 0xC9 */ u8 field_0xC9;
    /* 0xCA */ u8 pad0CA[0x02];
    /* 0xCC */ u8 field_0xCC;
    /* 0xCD */ u8 pad0CD[0x02];
    /* 0xCF */ u8 field_0xCF;
    /* 0xD0 */ u8 pad0D0[0x02];
    /* 0xD2 */ u8 field_0xD2;
    /* 0xD3 */ u8 field_0xD3;
    /* 0xD4 */ u8 pad0D4[0x02];
    /* 0xD6 */ u8 field_0xD6;
    /* 0xD7 */ u8 padD7[0x19];
    /* 0xF0 */ u8 field_0xF0;
    /* 0xF1 */ u8 field_0xF1;
    /* 0xF2 */ u8 field_0xF2;
    /* 0xF3 */ u8 field_0xF3;
} WorldContext;

/** @brief System state block (at D_800704A8). */
typedef struct {
    /* 0x000 */ u8 mode;
    /* 0x001 */ u8 pad001;
    /* 0x002 */ s16 counter;
    /* 0x004 */ u8 pad004[0x18C];
    /* 0x190 */ u8 slotActive[16];
} SystemState;

/** @brief Battle encounter setup parameters (at D_80082C90). */
typedef struct {
    /* 0x00 */ s32 encounterPtr;
    /* 0x04 */ u8 field_04;
    /* 0x05 */ u8 field_05;
    /* 0x06 */ u8 field_06;
    /* 0x07 */ u8 field_07;
    /* 0x08 */ u8 field_08;
    /* 0x09 */ u8 field_09;
    /* 0x0A */ u8 pad0A[2];
    /* 0x0C */ u8 result;
} EncounterParams;

extern WorldContext *D_800562C4;
extern SystemState D_800704A8;
extern u8 D_8007064C;
extern u8 D_80070656[];
extern s16 D_8007737C;
extern u16 D_80082C0A;
extern Eline *D_80085224;
extern Eline *D_80085230[];
extern u8 D_800773C0;
extern u8 D_80082C0F;
extern u8 D_80082C11;
extern EncounterParams D_80082C90;
extern u8 D_80082C10;
extern u8 D_8005630C[];
extern s32 D_8005F13C;
extern s16 D_8005F11C;
extern s16 D_800704B2;
extern u8 D_800DE8D0;
extern s16 D_800DE4D0;
extern s8 D_800DE4D2;
extern s8 D_800DE4D3;
extern s8 D_800DE4D4;

/**
 * @brief Pop a key item ID and store its value.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B542C(Eline *eline) {
    eline->field_0x140 = getKeyItemValue(POP(eline));
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
 * @param eline Pointer to the event line (script context).
 * @return 1 on first pass (battle started), 2 on return (result ready).
 */
s32 func_800B5480(Eline *eline) {
    u8 *params;
    s32 result;
    s32 i;

    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        EncounterParams *params = &D_80082C90;
        params->field_08 = POP_BYTE(eline);
        params->field_06 = POP_BYTE(eline);
        params->field_07 = POP_BYTE(eline);
        params->field_09 = POP_BYTE(eline);
        params->field_04 = POP_BYTE(eline);
        params->encounterPtr = POP(eline);
        params->field_05 = POP_BYTE(eline);

        result = sumItemQuantities(params);
        eline->field_0x140 = result;
        eline->field_0x144 = 0;

        if (result >= 5) {
            if (!(D_800562C4->field_0x68 & 0x10)) {
                initBattleTransition();
            }

            D_800704A8.mode = 8;
            D_800704A8.counter = 0;

            while (sndGetStatus() == 2) {
                func_800393C8();
            }

            for (i = 0; i < 2; i++) {
                clearAnimEntryActive(i);
            }

            setCameraVibrateState(0);

            if (D_800562C4->field_0x6C == -1) {
                sndCmd11(0);
            }

            sndCmd40();
            func_80037FB0(0, 0x46, D_8005F13C);

            while (D_800562C4->field_0xD6 == 0) {
                func_800393C8();
            }

            D_80082C11 = D_800562C4->field_0xC9;
            D_8005F11C = sndCmd10(toggleSoundBank());
            sndCmdC0(0, 0x7F);
            sndStopPlayback();
            sndCmdF1();
            sndSetMasterVolume(0x7F);
        }

        return 1;
    } else {
        if (D_80082C90.result == 3) {
            eline->field_0x144 = -1;
        } else {
            eline->field_0x144 = D_80082C90.result;
        }
        return 2;
    }
}

extern u8 D_800DE880[];

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

    if (D_800562C4->field_0x68 & 0x800) {
        D_80082C10 = D_800562C4->field_0xF3;
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
 * @param eline Pointer to the event line (script context).
 * @return 1 while processing, 2 when complete.
 */
s32 func_800B5A30(Eline *eline) {
    s32 fieldIdx;
    s32 tableResult;
    u8 *text;
    u32 dims;
    s32 i;
    s16 rect[4];

    fieldIdx = PEEK(eline) - 1;
    tableResult = lookupFieldTable(fieldIdx);

    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->field_0x204 = 0;
    }

    switch (eline->field_0x204) {
    case 0:
        D_800562C4->field_0xF2 = fieldIdx;
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
        D_800562C4->field_0xD3 |= 0x40;

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
        eline->field_0x204++;
        break;

    case 1:
        if (D_800562C4->field_0xD3 & 0x40) {
            return 1;
        }

        i = getPackedField2Bit(fieldIdx);
        if (i == 3 || getPackedField2Bit(fieldIdx) == 2) {
            eline->field_0x140 = 0;
            eline->stackPtr--;
            return 2;
        }

        if (func_800B5990()) {
            eline->field_0x204++;
            break;
        }

        eline->field_0x140 = 0;
        eline->stackPtr--;
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
        D_800562C4->field_0xD3 |= 0x40;
        D_800562C4->field_0xD2 |= 0x40;
        eline->field_0x204++;
        break;

    case 3:
        setSfxGlobalFlag(6);
        D_800DE4D2 = func_8002CE84(6);
        if ((s8)D_800DE4D2 < 0) {
            return 1;
        }
        fadeOutSfxSlow(6);
        D_800562C4->field_0xD2 &= ~0x40;

        if ((s8)D_800DE4D2 == 0) {
            eline->field_0x140 = 0;
            eline->stackPtr--;
            return 2;
        }

        D_800DE4D2--;
        eline->field_0x204++;
        break;

    case 4:
        D_800DE4D3 = func_80037C6C(g_gameState.battleParty[D_800DE4D2]);
        if (getSfxField1C(6)) {
            return 1;
        }

        if (g_battleChars.chars[D_800DE4D3].fieldStatusByte & 2) {
            func_800A455C(D_800562C4->entityLookup[D_800DE4D2]);
            sndPlaySfx(0x43, 0, 0x80, 0x7F);
            eline->field_0x204++;
            break;
        }

        eline->field_0x204 += 2;
        break;

    case 5:
        if (func_800A48CC()) {
            return 1;
        }
        eline->field_0x204++;
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
                eline->field_0x204 = 0;
            }
        } else {
            text = func_800B574C(func_8003974C(D_8005630C, 6));
            eline->field_0x204 = 0;
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
        D_800562C4->field_0xD3 |= 0x40;
        eline->field_0x204++;
        break;

    case 7:
        if (D_800562C4->field_0xD3 & 0x40) {
            return 1;
        }

        if (tableResult & 0x40) {
            func_800383B8(fieldIdx, 2);
        } else {
            func_800383B8(fieldIdx, 3);
        }

        eline->stackPtr--;
        return 2;
    }

    return 1;
}

/**
 * @brief Set up a scripted camera/effect using eline position data.
 *
 * Sets WorldContext field_0xF0 to 1 (active), pops a byte parameter
 * into field_0xF1, then calls func_800A4500 with the eline's position
 * fields and func_800A4550 with field_0xF1 | field_0x58.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6210(Eline *eline) {
    D_800562C4->field_0xF0 = 1;
    D_800562C4->field_0xF1 = POP_BYTE(eline);
    func_800A4500(eline->posX, eline->posY, eline->field_0x198);
    func_800A4550(D_800562C4->field_0xF1 | D_800562C4->field_0x58);
    return 2;
}

/**
 * @brief Pop a byte from the stack, decrement, and store to WorldContext field_0xF2.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B629C(Eline *eline) {
    D_800562C4->field_0xF2 = POP_BYTE(eline);
    D_800562C4->field_0xF2--;
    return 2;
}

/**
 * @brief Activate a system slot by index from the script stack.
 *
 * Pops a value, masks to 4 bits (0-15), and sets the corresponding
 * slotActive entry in the system state to 1.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B62E8(Eline *eline) {
    D_800704A8.slotActive[POP(eline) & 0xF] = 1;
    return 2;
}

/**
 * @brief Deactivate a system slot by index from the script stack.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6328(Eline *eline) {
    D_800704A8.slotActive[POP(eline) & 0xF] = 0;
    return 2;
}

/**
 * @brief Activate a system slot with the eline's field_0x256 value | 0x80.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6364(Eline *eline) {
    D_800704A8.slotActive[POP(eline) & 0xF] = eline->field_0x256 | 0x80;
    return 2;
}

/**
 * @brief Pop a value; if nonzero call func_800C0384 (set bit 0x20),
 *        otherwise call func_800C03A0 (clear bit 0x20).
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B63A4(Eline *eline) {
    if (POP(eline)) {
        func_800C0384();
    } else {
        func_800C03A0();
    }
    return 2;
}

/**
 * @brief Delegate to func_800C03F4 and return.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6400(Eline *eline) {
    func_800C03F4(eline);
    return 2;
}

/**
 * @brief Clear field_0x204 if the entity's script group is active.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6420(Eline *eline) {
    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->field_0x204 = 0;
    }
    return 2;
}

/**
 * @brief Pop a value from the stack and store to D_8007737C.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6448(Eline *eline) {
    D_8007737C = POP(eline);
    return 2;
}

/**
 * @brief Pop a value and store to both D_80082C0A and WorldContext field_0xB6.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6478(Eline *eline) {
    D_80082C0A = D_800562C4->field_0xB6 = POP(eline);
    do {} while (0);
    return 2;
}

/**
 * @brief Set system mode to 3 (if idle) and pop scene ID + counter.
 *
 * @param eline Pointer to the event line (script context).
 * @return 3 (special return — triggers mode transition).
 */
s32 func_800B64B0(Eline *eline) {
    if (D_800704A8.mode == 0) {
        D_800704A8.mode = 3;
    }
    D_80082C0A = POP(eline);
    D_800704A8.counter = POP(eline);
    return 3;
}

/**
 * @brief Store D_80082C0F into the eline result field.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B6524(Eline *eline) {
    eline->field_0x140 = D_80082C0F;
    return 2;
}

/**
 * @brief If D_800DE8D0 is set, enable flag 0x400; otherwise clear field_0xCF.
 *
 * @return 2 (continue processing).
 */
s32 func_800B653C(void) {
    if (D_800DE8D0) {
        D_800562C4->field_0x68 |= 0x400;
    } else {
        D_800562C4->field_0xCF = 0;
    }
    return 2;
}

/**
 * @brief Set field_0xCF to 1 and clear flag 0x400.
 *
 * @return 2 (continue processing).
 */
s32 func_800B6588(void) {
    WorldContext *ctx = D_800562C4;

    ctx->field_0xCF = 1;
    ctx->field_0x68 &= ~0x400;
    return 2;
}

/** @brief No-op handler. Returns 2 (continue). */
s32 func_800B65B0(Eline *eline) {
    return 2;
}

/**
 * @brief Set system mode to 4 (game render) and yield.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 (yield).
 */
s32 func_800B65B8(Eline *eline) {
    D_800704A8.mode = 4;
    return 1;
}

/** @brief Yield handler. Returns 1 (wait). */
s32 func_800B65CC(Eline *eline) {
    return 1;
}

/**
 * @brief Pop disc number from stack, set WorldContext and global state.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B65D4(Eline *eline) {
    D_800562C4->field_0xCC = POP_BYTE(eline);
    D_800773C0 = D_800562C4->field_0xCC - 1;
    setDiscNumber(D_800562C4->field_0xCC);
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
 * @param eline Pointer to the event line (script context).
 */
void func_800B663C(Eline *eline) {
    if (!(eline->flags & 0x10000000)) {
        return;
    }
    if (eline->msgActive != 1) {
        return;
    }

    eline->field_0x1C0 = eline->msgTextPtr;
    eline->field_0x1C4 = eline->msgPosX;
    eline->field_0x1C8 = eline->msgPosY;
    eline->field_0x21C = eline->windowId;
    eline->field_0x202 = eline->savedChannel;
    eline->msgActive = 0;
    eline->flags |= 0x10000;
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
 * @param eline Pointer to the event line (script context).
 */
void func_800B66A8(Eline *eline) {
    if (!(eline->flags & 0x10000000)) {
        return;
    }
    if (!(eline->flags & 0x10000)) {
        return;
    }
    eline->msgTextPtr = eline->field_0x1C0;
    eline->msgPosX = eline->field_0x1C4;
    eline->msgPosY = eline->field_0x1C8;
    eline->windowId = eline->field_0x21C;
    eline->savedChannel = eline->field_0x202;
    eline->msgActive = 1;
    eline->msgState = 0;

    if ((eline->flags = eline->flags & ~0x10000) & 0x40000) {
        func_800B6738(eline);
    }
}

/**
 * @brief Initialize message display.
 *
 * Compares savedChannel against a threshold derived from D_800704B2
 * to select between two message source indices (field_0x250 vs
 * field_0x251). If the selected source differs from field_0x24E,
 * calls func_800B912C to set it up and marks the 0x2000 flag.
 * Clears field_0x1DA and sets the async-message flag (0x40000).
 *
 * @param eline Pointer to the event line (script context).
 */
void func_800B6738(Eline *eline) {
    s32 threshold = (D_800704B2 * 69020) >> 9;

    if (eline->savedChannel >= threshold) {
        if (eline->field_0x24E != eline->field_0x251) {
            func_800B912C(eline, eline->field_0x251);
            eline->flags |= 0x2000;
        }
    } else {
        if (eline->field_0x24E != eline->field_0x250) {
            func_800B912C(eline, eline->field_0x250);
            eline->flags |= 0x2000;
        }
    }

    eline->field_0x1DA = 0;
    eline->flags |= 0x40000;
}

/**
 * @brief Finalize message display.
 *
 * If the async-message flag (0x40000) is set, calls func_800B912C
 * with field_0x24F, then sets bit 0x2000 and clears the async flag.
 * Always clears field_0x240.
 *
 * @param eline Pointer to the event line (script context).
 */
void func_800B67F4(Eline *eline) {
    if (eline->flags & 0x40000) {
        func_800B912C(eline, eline->field_0x24F);
        eline->flags = (eline->flags | 0x2000) & ~0x40000;
    }
    eline->field_0x240 = 0;
}

/**
 * @brief Check if message display is complete and clean up.
 *
 * If the message-pending flag (0x20000) is set in the eline's flags
 * and the message state has reached 2 (complete), calls func_800B67F4
 * to finalize and clears the flag.
 *
 * @param eline Pointer to the event line (script context).
 */
void func_800B6854(Eline *eline) {
    if ((eline->flags & 0x20000) && eline->msgState == 2) {
        func_800B67F4(eline);
        eline->flags &= ~0x20000;
    }
}

/**
 * @brief Table index 0x04F handler — set message channel.
 *
 * Pops a channel value from the bytecode stack and stores it to both
 * savedChannel and msgChannel fields of the eline.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B68B8(Eline *eline) {
    u16 channel = POP(eline);

    eline->savedChannel = channel;
    eline->msgChannel = channel;
    return 2;
}

/**
 * @brief MES opcode 0x50 handler — show dialog message on screen.
 *
 * Checks if the entity is active, then pops window ID, Y position,
 * X position, and message text pointer from the bytecode stack. Saves
 * the current message channel, initializes message state, and calls
 * func_800B6738 to set up the message display. If message state reaches
 * 2 (complete), calls func_800B67F4 to finalize.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 if message display is complete, 1 otherwise (yield).
 */
s32 opHandler_MES(Eline *eline) {
    s32 new_var;
    u16 saved;

    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        saved = eline->msgChannel;
        eline->msgActive = 1;
        eline->msgState = 0;

        eline->windowId = POP(eline);
        eline->msgPosY = POP(eline) << 12;
        eline->msgPosX = POP(eline) << 12;
        new_var = POP(eline);
        eline->savedChannel = saved;
        eline->msgTextPtr = new_var << 12;

        func_800B6738(eline);
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
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
 * @param eline Pointer to the event line (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 func_800B69E8(Eline *eline) {
    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->msgActive = 1;
        eline->msgState = 0;
        eline->windowId = POP(eline);
        eline->savedChannel = eline->msgChannel;
        func_800B6738(eline);
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
        eline->stackPtr--;
        return 2;
    }

    eline->msgTextPtr = D_80085230[PEEK(eline)]->posX;
    eline->msgPosX = D_80085230[PEEK(eline)]->posY;
    eline->msgPosY = D_80085230[PEEK(eline)]->field_0x198;
    return 1;
}

/**
 * @brief Show a message tracking a party member entity's position.
 *
 * Like func_800B69E8 but looks up the entity index through
 * D_800562C4->entityLookup[] first, then reads position from
 * D_80085224[idx]. Updates message coordinates each frame.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 func_800B6B20(Eline *eline) {
    u8 idx;

    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->msgActive = 1;
        eline->msgState = 0;
        eline->windowId = POP(eline);
        eline->savedChannel = eline->msgChannel;
        func_800B6738(eline);
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
        eline->stackPtr--;
        return 2;
    }

    idx = D_800562C4->entityLookup[PEEK(eline)];
    eline->msgTextPtr = D_80085224[idx].posX;
    eline->msgPosX = D_80085224[idx].posY;
    eline->msgPosY = D_80085224[idx].field_0x198;
    return 1;
}

/**
 * @brief ASK opcode handler — display a yes/no prompt.
 *
 * Sets up the message display with position and window ID from
 * the bytecode stack, initializes the prompt state fields,
 * then polls for completion.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 while prompt is active, 2 when answered.
 */
s32 func_800B6C28(Eline *eline) {
    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->msgActive = 1;
        eline->msgState = 0;
        eline->windowId = POP(eline);
        eline->savedChannel = eline->msgChannel;
        eline->msgPosY = POP(eline) << 12;
        eline->msgPosX = POP(eline) << 12;
        eline->msgTextPtr = POP(eline) << 12;
        eline->field_0x262 = 0;
        eline->field_0x240 = 1;
        eline->field_0x1DA = 0;
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
        return 2;
    }

    return 1;
}

/**
 * @brief Display a positioned message (variant without yes/no prompt setup).
 *
 * Like func_800B6C28 (ASK prompt) but without the field_0x262/field_0x240
 * initialization. Pops window ID, Y position, X position, and message text
 * pointer from the bytecode stack; saves the current message channel;
 * resets message state. Then polls for completion.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 func_800B6D24(Eline *eline) {
    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->msgActive = 1;
        eline->msgState = 0;
        eline->windowId = POP(eline);
        eline->savedChannel = eline->msgChannel;
        eline->msgPosY = POP(eline) << 12;
        eline->msgPosX = POP(eline) << 12;
        eline->msgTextPtr = POP(eline) << 12;
        eline->field_0x1DA = 0;
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
        return 2;
    }

    return 1;
}

/**
 * @brief Position-tracking message variant (clears field_0x1DA, no display init).
 *
 * Like func_800B69E8 but clears field_0x1DA instead of calling func_800B6738.
 * Pops window ID and saves the channel on the first pass; afterwards, updates
 * the message coordinates from D_80085230[PEEK]'s position each frame.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 func_800B6E18(Eline *eline) {
    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->msgActive = 1;
        eline->msgState = 0;
        eline->windowId = POP(eline);
        eline->savedChannel = eline->msgChannel;
        eline->field_0x1DA = 0;
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
        eline->stackPtr--;
        return 2;
    }

    eline->msgTextPtr = D_80085230[PEEK(eline)]->posX;
    eline->msgPosX = D_80085230[PEEK(eline)]->posY;
    eline->msgPosY = D_80085230[PEEK(eline)]->field_0x198;
    return 1;
}

/**
 * @brief Party-member position-tracking message variant (clears field_0x1DA).
 *
 * Like func_800B6B20 but clears field_0x1DA instead of calling func_800B6738.
 * Looks up a party slot through D_800562C4->entityLookup[], then updates the
 * message coordinates from D_80085224[idx]'s position each frame.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 while message is active, 2 when complete.
 */
s32 func_800B6F4C(Eline *eline) {
    u8 idx;

    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->msgActive = 1;
        eline->msgState = 0;
        eline->windowId = POP(eline);
        eline->savedChannel = eline->msgChannel;
        eline->field_0x1DA = 0;
    }

    if (eline->msgState == 2) {
        func_800B67F4(eline);
        eline->stackPtr--;
        return 2;
    }

    idx = D_800562C4->entityLookup[PEEK(eline)];
    eline->msgTextPtr = D_80085224[idx].posX;
    eline->msgPosX = D_80085224[idx].posY;
    eline->msgPosY = D_80085224[idx].field_0x198;
    return 1;
}

/**
 * @brief Pending-message MES variant — set flag 0x20000, init display, return 3.
 *
 * Unconditionally initializes the message with 4 stack values and sets the
 * pending-message flag (0x20000). Unlike opHandler_MES, there is no activeMask
 * guard or msgState==2 check — it schedules the display and yields with 3.
 *
 * @param eline Pointer to the event line (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 func_800B7050(Eline *eline) {
    s32 new_var;
    u16 saved;

    saved = eline->msgChannel;
    eline->msgActive = 1;
    eline->flags |= 0x20000;
    eline->msgState = 0;

    eline->windowId = POP(eline);
    eline->msgPosY = POP(eline) << 12;
    eline->msgPosX = POP(eline) << 12;
    new_var = POP(eline);
    eline->savedChannel = saved;
    eline->msgTextPtr = new_var << 12;

    func_800B6738(eline);
    return 3;
}

/**
 * @brief Pending message variant with entity-position tracking.
 *
 * Sets the pending-message flag, pops a window ID, then reads initial message
 * coordinates from D_80085230[idx]'s position (two PEEKs + one POP for the
 * final Y). Returns 3 to indicate deferred completion.
 *
 * @param eline Pointer to the event line (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 func_800B711C(Eline *eline) {
    eline->msgActive = 1;
    eline->flags |= 0x20000;
    eline->msgState = 0;
    eline->windowId = POP(eline);
    eline->savedChannel = eline->msgChannel;
    func_800B6738(eline);

    eline->msgTextPtr = D_80085230[PEEK(eline)]->posX;
    eline->msgPosX = D_80085230[PEEK(eline)]->posY;
    eline->msgPosY = D_80085230[POP(eline)]->field_0x198;
    return 3;
}

/**
 * @brief Pending message variant with party-member position tracking.
 *
 * Sets the pending-message flag, pops a window ID, looks up a party slot
 * through D_800562C4->entityLookup[POP], then reads initial coordinates from
 * D_80085224[idx]'s position. Returns 3 for deferred completion.
 *
 * @param eline Pointer to the event line (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 func_800B7228(Eline *eline) {
    u8 idx;

    eline->msgActive = 1;
    eline->flags |= 0x20000;
    eline->msgState = 0;
    eline->windowId = POP(eline);
    eline->savedChannel = eline->msgChannel;
    func_800B6738(eline);

    idx = D_800562C4->entityLookup[POP(eline)];
    eline->msgTextPtr = D_80085224[idx].posX;
    eline->msgPosX = D_80085224[idx].posY;
    eline->msgPosY = D_80085224[idx].field_0x198;
    return 3;
}

/**
 * @brief Pending ASK-prompt variant — leaf function, all 4 POPs, returns 3.
 *
 * Sets the pending-message flag and pops window ID, Y, X, and text pointer
 * from the stack. Also initializes the prompt-specific state fields
 * (field_0x262=0, field_0x240=1, field_0x1DA=0) like func_800B6C28, but
 * unconditionally (no activeMask guard) and returns 3.
 *
 * @param eline Pointer to the event line (script context).
 * @return 3 (prompt scheduled, deferred completion).
 */
s32 func_800B7310(Eline *eline) {
    eline->msgActive = 1;
    eline->flags |= 0x20000;
    eline->msgState = 0;
    eline->windowId = POP(eline);
    eline->savedChannel = eline->msgChannel;
    eline->msgPosY = POP(eline) << 12;
    eline->msgPosX = POP(eline) << 12;
    eline->msgTextPtr = POP(eline) << 12;
    eline->field_0x1DA = 0;
    eline->field_0x262 = 0;
    eline->field_0x240 = 1;
    return 3;
}

/**
 * @brief Pending MES variant — leaf function, all 4 POPs, returns 3.
 *
 * Sets the pending-message flag and pops window ID, Y, X, and text pointer
 * from the stack. Returns 3 immediately (no display init call).
 *
 * @param eline Pointer to the event line (script context).
 * @return 3 (message scheduled, deferred completion).
 */
s32 func_800B73D4(Eline *eline) {
    eline->msgActive = 1;
    eline->flags |= 0x20000;
    eline->msgState = 0;
    eline->windowId = POP(eline);
    eline->savedChannel = eline->msgChannel;
    eline->msgPosY = POP(eline) << 12;
    eline->msgPosX = POP(eline) << 12;
    eline->msgTextPtr = POP(eline) << 12;
    eline->field_0x1DA = 0;
    return 3;
}

/**
 * @brief Poll whether the current message has finished displaying.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 if msgState == 2 (message complete), 1 otherwise (still displaying).
 */
s32 func_800B7490(Eline *eline) {
    if (eline->msgState == 2) {
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
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B74B0(Eline *eline) {
    s32 idx = POP(eline);

    if (D_80085230[idx]->flags & 0x10000000) {
        if (D_80085230[idx]->msgActive != 1) {
            return 2;
        }
        D_80085230[idx]->msgActive = 0;
        func_800B912C(D_80085230[idx], D_80085230[idx]->field_0x24F);
        D_80085230[idx]->flags &= 0xFFFF07FF;
        D_80085230[idx]->flags |= 0x2000;
    }
    return 2;
}

/**
 * @brief Clean up message state for a referenced party-member entity.
 *
 * Pops a party slot, looks up the entity through D_800562C4->entityLookup[],
 * then (if the message is currently active) clears it, tears down the display,
 * and marks the flags (clear 0xF800 range and set 0x2000).
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B7578(Eline *eline) {
    u8 idx = D_800562C4->entityLookup[POP(eline)];

    if (D_80085224[idx].msgActive == 1) {
        D_80085224[idx].msgActive = 0;
        func_800B912C(&D_80085224[idx], D_80085224[idx].field_0x24F);
        D_80085224[idx].flags = (D_80085224[idx].flags & 0xFFFF07FF) | 0x2000;
    }
    return 2;
}

/**
 * @brief Clear the 0x10000 flag bit if the entity is active.
 *
 * @param eline Pointer to the event line (script context).
 * @return 1 (continue processing).
 */
s32 func_800B7640(Eline *eline) {
    if ((eline->activeMask >> eline->scriptGroup) & 1) {
        eline->flags &= ~0x10000;
    }
    return 1;
}

/**
 * @brief Pop a byte off the stack into field_0x262.
 *
 * @param eline Pointer to the event line (script context).
 * @return 2 (continue processing).
 */
s32 func_800B7674(Eline *eline) {
    eline->field_0x262 = POP_BYTE(eline);
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
 * @param self Pointer to the event line (script context).
 * @return Integer distance between the two positions.
 */
s32 func_800B76A4(Eline *self) {
    s32 dx = (self->msgTextPtr - self->posX) / 4096;
    s32 dy = (self->msgPosX - self->posY) / 4096;
    dx = dx * dx;
    dy = dy * dy;
    return func_8003F4A4(dx + dy);
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
 * @param self Pointer to the event line (script context).
 * @return 1 while message is animating, 2 when complete.
 */
s32 func_800B7718(Eline *self) {
    s32 delta = 0;

    if ((self->activeMask >> self->scriptGroup) & 1) {
        self->msgActive = 1;
        self->msgState = 0;
        self->savedChannel = self->msgChannel;
        self->field_0x204 = POP(self);
        self->windowId = POP(self);
        self->msgPosY = POP(self) << 12;
        self->msgPosX = POP(self) << 12;
        self->msgTextPtr = POP(self) << 12;
        self->field_0x262 = 0;
        self->field_0x240 = 1;
    }

    if (self->msgState == 2) {
        self->field_0x202 = self->savedChannel;
        func_800B67F4(self);
        return 2;
    }

    delta = (s32)(self->field_0x204 - self->savedChannel) * D_800704B2;
    delta = delta / func_800B76A4(self);
    self->savedChannel += delta;
    func_800B6738(self);
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B788C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B79C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7D44);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7E78);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8344);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B83FC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B84D8);

/**
 * Pops a word from the stack and stores the low byte to D_80070656.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B85C8(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u8 *)D_80070656 = *(volatile s32 *)(a0 + (s8)idx * 4);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B85F8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8710);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8824);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B89C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8B58);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8BE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8CD4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8DC8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8E74);

/**
 * Returns 2 if the byte at offset 0x245 equals 3, otherwise returns 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if object byte 0x245 is 3, else 1.
 */
s32 func_800B8F20(u8 *a0) {
    if (*(u8 *)(a0 + 0x245) == 3) {
        return 2;
    }
    return 1;
}

/**
 * Sets the global byte D_8007064C to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B8F3C(u8 *a0) {
    D_8007064C = 1;
    return 2;
}

/**
 * Clears the global byte D_8007064C, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B8F50(u8 *a0) {
    D_8007064C = 0;
    return 2;
}

/**
 * Calls func_8009E660 and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B8F60(u8 *a0) {
    func_8009E660(a0);
    return 2;
}

/** @brief Compare D_800704A8 entries. Returns 1 if different, 2 if same. */
s32 func_800B8F80(u8 *a0) {
    u8 *base = (u8 *)&D_800704A8;
    u16 val1 = *(u16 *)(base + 0x106);
    u16 val2 = *(u16 *)(base + 0x104);
    if (val1 == val2) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8FA8);

/** @brief Pop byte from stack and store to offset 0x240. Returns 2. */
s32 func_800B9000(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u8 *)(a0 + 0x240) = *(u8 *)(a0 + (s8)idx * 4);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B9030);
