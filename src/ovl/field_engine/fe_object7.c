#include "common.h"
#include "battle.h"
#include "gamestate.h"

/** @brief Eline (event line) script context for field engine opcode handlers.
 *  Total size: 416 bytes (0x1A0), confirmed by sizeof(eline) debug print.
 */
typedef struct {
    /* 0x000 */ u8 pad000[0x140];
    /* 0x140 */ s32 field_0x140;
    /* 0x144 */ s32 field_0x144;
    /* 0x148 */ u8 pad148[0x18];
    /* 0x160 */ s32 flags;
    /* 0x164 */ u8 pad164[0x10];
    /* 0x174 */ u8 scriptGroup;     /**< Script group index. */
    /* 0x175 */ u8 activeMask;      /**< Entity active bitmask. */
    /* 0x176 */ u8 pad176[0x0E];
    /* 0x184 */ s8 stackPtr;        /**< Bytecode stack pointer (signed, grows down). */
    /* 0x185 */ u8 pad185[0x0B];
    /* 0x190 */ s32 field_0x190;
    /* 0x194 */ s32 field_0x194;
    /* 0x198 */ s32 field_0x198;
    /* 0x19C */ u8 pad19C[0x18];
    /* 0x1B4 */ s32 msgTextPtr;     /**< Message text pointer (fixed-point). */
    /* 0x1B8 */ s32 msgPosX;        /**< Message X position (fixed-point). */
    /* 0x1BC */ s32 msgPosY;        /**< Message Y position (fixed-point). */
    /* 0x1C0 */ s32 field_0x1C0;      /**< Saved message text pointer. */
    /* 0x1C4 */ s32 field_0x1C4;      /**< Saved message X position. */
    /* 0x1C8 */ s32 field_0x1C8;      /**< Saved message Y position. */
    /* 0x1CC */ u8 pad1CC[0x0E];
    /* 0x1DA */ u16 field_0x1DA;
    /* 0x1DC */ u8 pad1DC[0x22];
    /* 0x1FE */ s16 savedChannel;     /**< Previous message channel. */
    /* 0x200 */ u16 msgChannel;       /**< Current message channel. */
    /* 0x202 */ u16 field_0x202;      /**< Saved channel for async restore. */
    /* 0x204 */ u8 pad204[0x16];
    /* 0x21A */ u16 windowId;         /**< Message window ID. */
    /* 0x21C */ u16 field_0x21C;      /**< Saved window ID for async restore. */
    /* 0x21E */ s16 msgState;         /**< Message state (0=init, 2=complete). */
    /* 0x220 */ u8 pad220[0x1C];
    /* 0x23C */ u8 msgActive;       /**< Message active flag. */
    /* 0x23D */ u8 pad23D[0x03];
    /* 0x240 */ u8 field_0x240;
    /* 0x241 */ u8 pad241[0x0D];
    /* 0x24E */ u8 field_0x24E;
    /* 0x24F */ u8 field_0x24F;
    /* 0x250 */ u8 field_0x250;
    /* 0x251 */ u8 field_0x251;
} Eline;

/** @brief World map / field context pointed to by D_800562C4. */
typedef struct {
    /* 0x00 */ u8 pad000[0x58];
    /* 0x58 */ u8 field_0x58;
    /* 0x59 */ u8 pad059[0x0F];
    /* 0x68 */ s32 field_0x68;
    /* 0x6C */ s32 field_0x6C;
    /* 0x70 */ u8 pad070[0x59];
    /* 0xC9 */ u8 field_0xC9;
    /* 0xCA */ u8 padCA[0x0C];
    /* 0xD6 */ u8 field_0xD6;
    /* 0xD7 */ u8 padD7[0x19];
    /* 0xF0 */ u8 field_0xF0;
    /* 0xF1 */ u8 field_0xF1;
    /* 0xF2 */ u8 field_0xF2;
    /* 0xF3 */ u8 field_0xF3;
} WorldContext;

/** @brief Fade/transition control (at D_800704A8). */
typedef struct {
    /* 0x00 */ u8 mode;
    /* 0x01 */ u8 pad;
    /* 0x02 */ s16 counter;
} FadeControl;

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

/** @brief Pop one s32 from the eline's bytecode stack. */
#define POP(eline) (((s32 *)(eline))[(s8)(eline)->stackPtr--])

/** @brief Pop one s32 then read low byte only. */
#define POP_BYTE(eline) (*(u8 *)&POP(eline))

extern WorldContext *D_800562C4;
extern FadeControl D_800704A8;
extern u8 D_8007064C;
extern u8 D_80070656[];
extern u8 D_8007737C[];
extern u8 D_80082C0F;
extern u8 D_80082C11;
extern EncounterParams D_80082C90;
extern u8 D_80082C10;
extern u8 D_800DE8D0;
extern s32 D_8005F13C;
extern s16 D_8005F11C;
extern s16 D_800704B2;

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

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B5A30);

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
    func_800A4500(eline->field_0x190, eline->field_0x194, eline->field_0x198);
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

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B62E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6328);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6364);

/**
 * Pops a value; if nonzero calls func_800C0384 (set bit 0x20),
 * otherwise calls func_800C03A0 (clear bit 0x20).
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B63A4(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    if (*(s32 *)(a0 + (s8)idx * 4) != 0) {
        func_800C0384();
    } else {
        func_800C03A0();
    }
    return 2;
}

/**
 * Calls func_800C03F4 and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B6400(u8 *a0) {
    func_800C03F4(a0);
    return 2;
}

/** @brief If animation bit set, clear halfword at 0x204. Returns 2. */
s32 func_800B6420(u8 *a0) {
    u8 bit = *(u8 *)(a0 + 0x175);
    u8 shift = *(u8 *)(a0 + 0x174);
    if ((bit >> shift) & 1) {
        *(u16 *)(a0 + 0x204) = 0;
    }
    return 2;
}

/**
 * Pops a halfword from the stack and stores it to D_8007737C.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B6448(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u16 *)D_8007737C = *(u16 *)(a0 + (s8)idx * 4);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6478);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B64B0);

/**
 * Loads global byte D_80082C0F into the object result field at offset 0x140,
 * returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B6524(u8 *a0) {

    *(s32 *)(a0 + 0x140) = D_80082C0F;
    return 2;
}

/**
 * If D_800DE8D0 is nonzero, sets bit 0x400 in the flags at D_800562C4+0x68.
 * Otherwise, clears byte 0xCF of the entity. Returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B653C(u8 *a0) {
    if (D_800DE8D0 != 0) {
        D_800562C4->field_0x68 |= 0x400;
    } else {
        *(u8 *)((u8 *)D_800562C4 + 0xCF) = 0;
    }
    return 2;
}

/**
 * Sets byte 0xCF of the entity pointed to by D_800562C4 to 1,
 * and clears bit 0x400 in the flags at offset 0x68, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B6588(u8 *a0) {
    WorldContext *ctx = D_800562C4;

    *(u8 *)((u8 *)ctx + 0xCF) = 1;
    ctx->field_0x68 &= ~0x400;
    return 2;
}

/**
 * Returns 2, indicating continue processing.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B65B0(u8 *a0) {
    return 2;
}

/**
 * Sets the global byte D_800704A8 to 4, returns 1.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 1.
 */
s32 func_800B65B8(u8 *a0) {
    D_800704A8.mode = 4;
    return 1;
}

/**
 * Returns 1, indicating wait/yield.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 1.
 */
s32 func_800B65CC(u8 *a0) {
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B65D4);

/**
 * If flag 0x10000000 is set and byte at 0x23C is 1, copies position
 * fields (0x1B4/0x1B8/0x1BC to 0x1C0/0x1C4/0x1C8), copies 0x21A to
 * 0x21C and 0x1FE to 0x202, clears byte 0x23C, and sets flag 0x10000.
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800B663C(u8 *a0) {
    if ((*(s32 *)(a0 + 0x160) & 0x10000000) && *(u8 *)(a0 + 0x23C) == 1) {
        *(s32 *)(a0 + 0x1C0) = *(s32 *)(a0 + 0x1B4);
        *(u8 *)(a0 + 0x23C) = 0;
        *(s32 *)(a0 + 0x1C4) = *(s32 *)(a0 + 0x1B8);
        *(s32 *)(a0 + 0x1C8) = *(s32 *)(a0 + 0x1BC);
        *(u16 *)(a0 + 0x21C) = *(u16 *)(a0 + 0x21A);
        *(u16 *)(a0 + 0x202) = *(u16 *)(a0 + 0x1FE);
        *(s32 *)(a0 + 0x160) = *(s32 *)(a0 + 0x160) | 0x10000;
    }
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

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B69E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6B20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6C28);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6D24);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6E18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6F4C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7050);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B711C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7228);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7310);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B73D4);

/**
 * Returns 2 if the halfword at offset 0x21E equals 2, otherwise returns 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if value at 0x21E is 2, else 1.
 */
s32 func_800B7490(u8 *a0) {
    if (*(s16 *)(a0 + 0x21E) == 2) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B74B0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7578);

/** @brief If animation bit set, clear bit 0x10000 in flags. Returns 1. */
s32 func_800B7640(u8 *a0) {
    u8 bit = *(u8 *)(a0 + 0x175);
    u8 shift = *(u8 *)(a0 + 0x174);
    if ((bit >> shift) & 1) {
        *(s32 *)(a0 + 0x160) = *(s32 *)(a0 + 0x160) & ~0x10000;
    }
    return 1;
}

/** @brief Pop byte from stack, store to offset 0x262. Returns 2. */
s32 func_800B7674(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u8 *)(a0 + 0x262) = *(u8 *)(a0 + (s8)idx * 4);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B76A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7718);

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

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8608);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8644);

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

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B9034);
