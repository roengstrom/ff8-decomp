#include "common.h"
#include "283B0.h"
#include "gamestate.h"
#include "battle.h"
#include "game.h"
#include "main.h"
#include "sound.h"
#include "imgtoc.h"

extern s32 D_80085220;
extern u8 D_8005644B[];
extern s32 D_800562D4;

static void loadSoundBankA(void);
static void loadSoundBankB(void);

/**
 * @brief Search battle party slots for a matching character ID.
 * @param characterId Character ID to search for.
 * @return Slot index (0-2) if found, 0xFF if not found.
 */
u8 findBattlePartySlot(u8 characterId) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (g_gameState.battleParty[i] == characterId) {
            return i;
        }
    }

    return 0xFF;
}


/**
 * @brief Search active party slots for a matching character ID.
 * @param characterId Character ID to search for.
 * @return Slot index (0-2) if found, 0xFF if not found.
 */
u8 findPartySlot(u8 characterId) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (g_gameState.mainData.party.partyMembers[i] == characterId) {
            return i;
        }
    }

    return 0xFF;
}


/**
 * @brief Search character slots for a matching character ID.
 * @param characterId Character ID to search for.
 * @return Slot index (0-7) if found, 0xFF if not found.
 */
u8 findCharacterSlot(u8 characterId) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (g_gameState.chars[i].characterId == characterId) {
            return i;
        }
    }

    return 0xFF;
}


s32 func_80037C6C(s32 charId) {
    u8 id = charId;
    s32 i;

    for (i = 0; i < PARTY_SLOT_COUNT; i++) {
        if (g_gameState.mainData.party.partyMembers[i] != PARTY_SLOT_EMPTY
            && g_gameState.chars[g_gameState.mainData.party.partyMembers[i]].characterId == id) {
            return (u8)i;
        }
    }
    return PARTY_SLOT_EMPTY;
}


/**
 * @brief Stop all playing sound channels and reset sound state.
 *
 * Reads two sound handles from the entity pointer at g_fieldVars (offsets
 * 0x6C and 0x70), stops each via sndCmdC1. Then calls sndStopPlayback
 * to flush sound state and sndSetChannelVolume to reset channel 0.
 */
void stopAllSounds(void) {
    s32 val;
    sndCmdC1(g_fieldVars->soundHandle0, 15, 0);
    val = g_fieldVars->soundHandle1;
    if (val != -1) {
        sndCmdC1(val, 15, 0);
    }
    sndStopPlayback();
    sndSetChannelVolume(0, 15);
}


void func_80037D40(void) {
    s32 i;

    sndCmdC1(g_fieldVars->soundHandle0, 15, g_fieldVars->musicVolume);
    if (g_fieldVars->soundHandle1 != SND_HANDLE_NONE) {
        sndCmdC1(g_fieldVars->soundHandle1, 15, g_fieldVars->sfxVolume);
    }
    for (i = 0; i < 12; i++) {
        if (D_80074F20[i].cmd < 0) {
            sndPlayBankSfx(D_80074F20[i].cmd, D_80074F20[i].arg1, D_80074F20[i].fieldC, D_80074F20[i].field8);
        } else if (D_80074F20[i].cmd > 0) {
            sndPlaySfx(D_80074F20[i].cmd - 1, D_80074F20[i].arg1, D_80074F20[i].fieldC, D_80074F20[i].field8);
        }
    }
    sndCmd45();
}


/**
 * @brief Toggle the sound bank selector and return the corresponding bank table.
 *
 * XORs byte at g_fieldVars[0xC9] with 1, then reloads and checks:
 * returns D_80063388 if the toggled value is non-zero, D_8005F388 otherwise.
 *
 * @return Pointer to the selected sound bank table.
 */
u8 *toggleSoundBank(void) {

    g_fieldVars->soundBankSelector ^= 1;
    if ((s8)g_fieldVars->soundBankSelector == 0) {
        return D_8005F388;
    }
    return D_80063388;
}


/**
 * @brief Load and apply sound data from disc (variant A).
 *
 * Polls sndGetEngineState until it returns 0, then reads sound data from
 * D_80085220 using func_80039728 and plays it via sndProcessAudio.
 * Reads sound data a second time, selects a bank table (D_8005F388
 * or D_80063388) based on g_fieldVars field 0xC9, and calls
 * FastMemCpy. Sets the completion flag at g_fieldVars + 0xD6.
 */
static void loadSoundBankA(void) {
    s32 size;
    s32 result;
    u8 *table;

    do {
    } while (sndGetEngineState() != 0);
    result = func_80039728(D_80085220, 1, &size);
    sndProcessAudio(result, 1);
    result = func_80039728(D_80085220, 0, &size);
    if ((s8)g_fieldVars->soundBankSelector != 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    FastMemCpy(table, (u8 *)result, size);
    g_fieldVars->soundLoadComplete = 1;
}


/**
 * @brief Load and apply sound data from disc (variant B).
 *
 * Same as func_80037E60, but the bank table selection is inverted:
 * uses D_80063388 when g_fieldVars field 0xC9 is non-zero, and
 * D_8005F388 when it is zero.
 */
static void loadSoundBankB(void) {
    s32 size;
    s32 result;
    u8 *table;

    do {
    } while (sndGetEngineState() != 0);
    result = func_80039728(D_80085220, 1, &size);
    sndProcessAudio(result, 1);
    result = func_80039728(D_80085220, 0, &size);
    if (g_fieldVars->soundBankSelector == 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    FastMemCpy(table, (u8 *)result, size);
    g_fieldVars->soundLoadComplete = 1;
}


void func_80037FB0(s32 variant, s32 bank, s32 buf) {
    D_80085220 = buf;
    g_fieldVars->soundLoadComplete = 0;
    if (variant == 0) {
        LoadDataFromDisc(D_800974F0[bank * 2], D_800974F0[bank * 2 + 1], (u8 *)buf, loadSoundBankA);
    } else {
        LoadDataFromDisc(D_800974F0[bank * 2], D_800974F0[bank * 2 + 1], (u8 *)buf, loadSoundBankB);
    }
}


void func_80038030(s32 arg0) {
    FieldVars *ptr = &g_gameState.fieldVars;

    if (!(g_battleConfig.unk2 & 0x10)) {
        while (sndGetStatus() == 2) {
            UpdateCdRead();
        }

        if (ptr->soundHandle0 == -1) {
            sndCmd11(0);
        }

        sndCmd40();
        D_80085220 = arg0;
        func_80037FB0(0, ptr->battleMusicId, arg0);

        if (ptr->soundLoadComplete == 0) {
            do {
                UpdateCdRead();
            } while (ptr->soundLoadComplete == 0);
        }

        D_8005F11C = sndCmd10(toggleSoundBank());
        sndCmdC0(0, 0x7F);
    }

    g_battleConfig.unk9 = (u8)ptr->soundBankSelector ^ 1;
    sndStopPlayback();
    sndCmdF1();
    sndSetMasterVolume(0x7F);

    if (isBossBattle() == 0) {
        sndPlaySfx(0xA, 0, 0x80, 0x7F);
        sndPlaySfx(0xB, 0, 0x80, 0x7F);
        sndPlaySfx(0xC, 0, 0x80, 0x7F);
    } else {
        sndPlaySfx(0x84, 0, 0x80, 0x7F);
        sndPlaySfx(0x85, 0, 0x80, 0x7F);
        sndPlaySfx(0x86, 0, 0x80, 0x7F);
    }
}


/**
 * @brief Refresh the active-party ↔ battle-entity slot mapping in field state.
 *
 * Part 1: For each of the 3 active party slots, search the battle field
 * entity table for one whose @c partyId matches and record its index in
 * both g_fieldContext.entityIndex and g_fieldVars->memberSlot. Defaults to
 * 0xFF when no entity matches.
 *
 * Part 2: When the bench-list flag (stateFlags & FIELD_STATE_PARTY_OVERRIDE) is set, build the
 * list of character IDs *not* currently in the active battle party
 * (partyOrderA/B at 0xBC/0xBF — initialized identically here).
 */
void func_800381BC(void) {
    s32 i;
    s32 j;
    Actor *ent;

    for (i = 0; i < 3; i++) {
        g_fieldContext.entityIndex[i] = 0xFF;
        g_fieldVars->memberSlot[i] = 0xFF;

        ent = g_fieldActors;
        for (j = 0; j < g_fieldActorCount; j++) {
            if (g_gameState.battleParty[i] == ent->field_0x255) {
                g_fieldVars->memberSlot[i] = j;
                g_fieldContext.entityIndex[i] = j;
                break;
            }
            ent++;
        }
    }

    if (g_fieldVars->stateFlags & FIELD_STATE_PARTY_OVERRIDE) {
        j = 0;
        for (i = 0; i < 6; i++) {
            if (findBattlePartySlot(i) == 0xFF) {
                g_fieldVars->partyOrderA[j] = i;
                g_fieldVars->partyOrderB[j] = i;
                j++;
            }
        }
    }
}


/**
 * @brief Clear entity flag bits 0x44 from active battle entities, then update.
 *
 * Iterates over g_fieldActorCount entries at stride 0x264 (612 bytes) starting
 * from g_fieldActors + 0x160, clearing bits 2 and 6 of the flags word at
 * each entry. Then calls func_800381BC to apply the changes.
 */
void clearEntityFlags(void) {
    s32 i;
    Actor *ent = g_fieldActors;
    u8 count = g_fieldActorCount;

    for (i = 0; i < count; i++, ent++) {
        ent->context.flags &= ~0x44;
    }

    func_800381BC();
}


/** @brief Returns the transition/ready bits of @c g_fieldVars->stateFlags
 *         (@ref FIELD_STATE_TRANSITION | @ref FIELD_STATE_FIELD_READY). */
s32 getFieldStateFlags(void) {
    return g_fieldVars->stateFlags & (FIELD_STATE_TRANSITION | FIELD_STATE_FIELD_READY);
}


/**
 * @brief Extract a 2-bit field from the packed bitfield array at g_fieldVars+0x74.
 *
 * Treats the byte array as a packed 2-bit-per-entry table. Computes byte
 * index (a0/4) and bit position ((a0%4)*2), then extracts and returns
 * the 2-bit value.
 *
 * @param a0 Entry index (low 8 bits used).
 * @return The 2-bit value (0-3) at the given index.
 */
s32 getPackedField2Bit(s32 entryIdx) {
    entryIdx &= 0xFF;
    return (g_fieldVars->drawPointFlag[entryIdx / 4] >> ((entryIdx % 4) * 2)) & 3;
}


void func_800383B8(s32 key, s32 status) {
    s32 shift;
    s32 mask;

    status &= 3;
    key &= 0xFF;
    shift = key % 4;
    shift *= 2;
    status <<= shift;
    mask = 3 << shift;
    g_fieldVars->drawPointFlag[key / 4] &= ~mask;
    g_fieldVars->drawPointFlag[key / 4] |= status;
}


/** @brief Looks up byte from D_8005644B table at index a0 (masked to 8 bits).
 *  @param a0 Table index (only low 8 bits used).
 *  @return The byte value at D_8005644B[a0 & 0xFF].
 */
u8 lookupFieldTable(s32 tableIdx) {
    return D_8005644B[tableIdx & 0xFF];
}


/**
 * @brief Get the dialogs' text speed for the field message-speed setting.
 *
 * @c config.fieldMsgSpeed is the Config menu slider position; @c g_textSpeeds maps
 * it to a text speed, from 0x1C00 (1.75 characters a frame) down to 0x0C00.
 */
u16 getFieldTextSpeed(void) {
    return g_textSpeeds[g_gameState.config.fieldMsgSpeed];
}


/** @brief Linear congruential generator: D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039.
 *  @return Bits 16-30 of the new state (0-32767).
 */
s32 fieldRandom(void) {
    D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039;
    return ((u32)D_800562D4 >> 16) & 0x7FFF;
}
