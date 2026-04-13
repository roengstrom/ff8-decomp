#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"
#include "gamestate.h"
#include "character.h"

extern u8 D_8007809B[];
extern u8 g_chocoboWorld;
extern u8 D_80085218;
extern u8 *D_800562C4;
extern u8 D_8005F388[];
extern u8 D_80063388[];
extern s32 D_80085220;
extern s32 D_80085224;
extern u8 D_80085388;
extern u8 D_8005644B[];
extern u8 D_80077E5A;
extern u16 D_800562C8[];
extern s32 D_800562D4;

INCLUDE_ASM("asm/nonmatchings/gamestate", func_800370AC);


/**
 * @brief Set a bit in the global bitfield array D_8007809B.
 * @param a0 Bit index to set.
 */
void setFieldFlag(s32 bitIdx) {
    u8 *base = D_8007809B;
    s32 byteIdx = bitIdx / 8;
    base[byteIdx] |= (1 << (bitIdx & 7));
}


/**
 * @brief Clear a bit in the global bitfield array D_8007809B.
 * @param a0 Bit index to clear.
 */
void clearFieldFlag(s32 bitIdx) {
    u8 *base = D_8007809B;
    s32 byteIdx = bitIdx / 8;
    base[byteIdx] &= ~(1 << (bitIdx & 7));
}


/**
 * @brief Test a bit in the global bitfield array D_8007809B.
 * @param a0 Bit index to test.
 * @return Non-zero if bit is set, zero otherwise.
 */
s32 testFieldFlag(s32 bitIdx) {
    u8 *base = D_8007809B;
    s32 byteIdx = bitIdx / 8;
    return base[byteIdx] & (1 << (bitIdx & 7));
}


/**
 * @brief Synchronize HP values for all available characters and GFs to the save data.
 *
 * Gets the character availability bitmask, iterates bits 0-7 calling
 * func_80036FE0 for each set bit. Then gets the GF availability bitmask
 * and iterates bits 0-15 calling copyGfHpToSave for each set bit.
 */
void func_80037240(void) {
    s32 i;
    u16 mask;

    mask = func_80036EC0();
    i = 0;
    do {
        if ((mask >> i) & 1) {
            func_80036FE0(i);
        }
        i++;
    } while (i < 8);

    mask = getGfAvailabilityMask();
    i = 0;
    do {
        if ((mask >> i) & 1) {
            copyGfHpToSave(i);
        }
        i++;
    } while (i < 16);
}


/** @brief Returns a pointer to global g_chocoboWorld. */
u8 *getChocoboWorldPtr(void) {
    return &g_chocoboWorld;
}


/** @brief Sets bit 0x1 in the byte at g_chocoboWorld. */
void enableChocoboWorld(void) {
    u8 *p = getChocoboWorldPtr();
    *p |= 0x1;
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037308);


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800375A0);


/**
 * Wrapper for func_800375A0 with fixed 6th argument 0x64808080.
 *
 * @param a0 First argument passed through
 * @param a1 Second argument passed through
 * @param a2 Third argument passed through
 * @param a3 Fourth argument passed through
 * @param arg4 Fifth argument passed through from caller's stack
 */
void drawSaveIcon(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4) {
    func_800375A0(a0, a1, a2, a3, arg4, 0x64808080);
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800376A8);


// mc_xor_checksum

/**
 * @brief Call func_800376A8 with constant 7th arg (0x64808080).
 *
 * Passes through all 6 caller args and appends 0x64808080
 * as the 7th argument.
 */
void drawSaveIconWithArgs(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg4, s32 arg5) {
    func_800376A8(a0, a1, a2, a3, arg4, arg5, 0x64808080);
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800377B4);


/**
 * @brief Compute an XOR checksum over 127 bytes (memory card frame header).
 *
 * XORs each of the first 127 bytes together and returns the low 8 bits.
 * Used to compute byte 0x7F of a memory card block header.
 *
 * @param a0 Pointer to the 128-byte memory card frame.
 * @return XOR checksum (0-255).
 */
u32 mcXorChecksum(u8 *frame) {
    u32 acc = 0;
    s32 i = 0;
    do {
        acc ^= *frame++;
        i++;
    } while ((u32)i < 0x7F);
    return acc & 0xFF;
}


/**
 * @brief Zero 128 bytes of memory (memory card frame header size).
 * @param ptr Pointer to the buffer to clear.
 */
void mcZeroFrame(u8 *ptr) {
    s32 count = 128;
    do {
        *ptr++ = 0;
    } while (--count > 0);
}


/**
 * @brief Initialize a memory card header frame (block 0, "MC" magic).
 *
 * Zeroes the 128-byte frame, sets bytes 0-1 to 'M','C' (0x4D, 0x43),
 * then computes and stores the XOR checksum at byte 0x7F.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void mcInitHeader(u8 *frame) {
    mcZeroFrame(frame);
    frame[0] = 0x4D;
    frame[1] = 0x43;
    frame[0x7F] = mcXorChecksum(frame);
}


/**
 * @brief Initialize a memory card directory frame (used save slot, type 0xA0).
 *
 * Zeroes the 128-byte frame, sets byte 0 to 0xA0 (in-use flag),
 * bytes 8-9 to 0xFF, then computes and stores the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void mcInitDirUsed(u8 *frame) {
    mcZeroFrame(frame);
    frame[0] = 0xA0;
    frame[8] = 0xFF;
    frame[9] = 0xFF;
    frame[0x7F] = mcXorChecksum(frame);
}


/**
 * @brief Initialize a memory card free/unused directory frame.
 *
 * Zeroes the 128-byte frame, sets bytes 0-3 and 8-9 to 0xFF
 * (marks the directory entry as free), then computes and stores
 * the XOR checksum.
 *
 * @param a0 Pointer to the 128-byte frame buffer.
 */
void mcInitDirFree(u8 *frame) {
    mcZeroFrame(frame);
    frame[0] = 0xFF;
    frame[1] = 0xFF;
    frame[2] = 0xFF;
    frame[3] = 0xFF;
    frame[8] = 0xFF;
    frame[9] = 0xFF;
    frame[0x7F] = mcXorChecksum(frame);
}


/** @brief Fills 128 bytes at a0 with 0xFF.
 *  @param a0 Pointer to buffer.
 */
void mcFillFF(u8 *buf) {
    s32 i = 128;
    do {
        *buf++ = 0xFF;
    } while (--i > 0);
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800379AC);


/** @brief Sets global D_80085218 to 1. */
void setMcBusy(void) {
    D_80085218 = 1;
}


/** @brief Returns the unsigned byte value of global D_80085218. */
u32 isMcBusy(void) {
    return D_80085218;
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037AEC);


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037B44);

INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037B7C);

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
        if (g_gameState.mainData.party.party[i] == characterId) {
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


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037C6C);


/**
 * @brief Stop all playing sound channels and reset sound state.
 *
 * Reads two sound handles from the entity pointer at D_800562C4 (offsets
 * 0x6C and 0x70), stops each via sndCmdC1. Then calls sndStopPlayback
 * to flush sound state and sndSetChannelVolume to reset channel 0.
 */
void stopAllSounds(void) {
    s32 val;
    sndCmdC1(*(s32 *)(D_800562C4 + 0x6C), 15, 0);
    val = *(s32 *)(D_800562C4 + 0x70);
    if (val != -1) {
        sndCmdC1(val, 15, 0);
    }
    sndStopPlayback();
    sndSetChannelVolume(0, 15);
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037D40);


/**
 * @brief Toggle the sound bank selector and return the corresponding bank table.
 *
 * XORs byte at D_800562C4[0xC9] with 1, then reloads and checks:
 * returns D_80063388 if the toggled value is non-zero, D_8005F388 otherwise.
 *
 * @return Pointer to the selected sound bank table.
 */
u8 *toggleSoundBank(void) {

    *(u8 *)(D_800562C4 + 0xC9) ^= 1;
    if (*(s8 *)(D_800562C4 + 0xC9) == 0) {
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
 * or D_80063388) based on D_800562C4 field 0xC9, and calls
 * func_80039678. Sets the completion flag at D_800562C4 + 0xD6.
 */
void loadSoundBankA(void) {
    s32 size;
    s32 result;
    u8 *table;

    do {
    } while (sndGetEngineState() != 0);
    result = func_80039728(D_80085220, 1, &size);
    sndProcessAudio(result, 1);
    result = func_80039728(D_80085220, 0, &size);
    if (*(s8 *)(D_800562C4 + 0xC9) != 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    func_80039678((s32)table, result, size);
    *(u8 *)(D_800562C4 + 0xD6) = 1;
}


/**
 * @brief Load and apply sound data from disc (variant B).
 *
 * Same as func_80037E60, but the bank table selection is inverted:
 * uses D_80063388 when D_800562C4 field 0xC9 is non-zero, and
 * D_8005F388 when it is zero.
 */
void loadSoundBankB(void) {
    s32 size;
    s32 result;
    u8 *table;

    do {
    } while (sndGetEngineState() != 0);
    result = func_80039728(D_80085220, 1, &size);
    sndProcessAudio(result, 1);
    result = func_80039728(D_80085220, 0, &size);
    if (*(s8 *)(D_800562C4 + 0xC9) == 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    func_80039678((s32)table, result, size);
    *(u8 *)(D_800562C4 + 0xD6) = 1;
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037FB0);


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80038030);


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800381BC);


/**
 * @brief Clear entity flag bits 0x44 from active battle entities, then update.
 *
 * Iterates over D_80085388 entries at stride 0x264 (612 bytes) starting
 * from D_80085224 + 0x160, clearing bits 2 and 6 of the flags word at
 * each entry. Then calls func_800381BC to apply the changes.
 */
void clearEntityFlags(void) {
    s32 base = D_80085224;
    s32 count = D_80085388;
    s32 i = 0;

    if (count != 0) {
        s32 mask = ~0x44;
        s32 ptr = base + 0x160;
        do {
            *(s32 *)ptr &= mask;
            i++;
            ptr += 0x264;
        } while (i < count);
    }
    func_800381BC();
}


/** @brief Returns bits 3-4 of the flags word at offset 0x68 through D_800562C4. */
s32 getFieldStateFlags(void) {
    return *(s32 *)(D_800562C4 + 0x68) & 0x18;
}


/**
 * @brief Extract a 2-bit field from the packed bitfield array at D_800562C4+0x74.
 *
 * Treats the byte array as a packed 2-bit-per-entry table. Computes byte
 * index (a0/4) and bit position ((a0%4)*2), then extracts and returns
 * the 2-bit value.
 *
 * @param a0 Entry index (low 8 bits used).
 * @return The 2-bit value (0-3) at the given index.
 */
s32 getPackedField2Bit(s32 entryIdx) {
    u8 *ptr = D_800562C4;
    s32 idx;

    entryIdx &= 0xFF;
    idx = entryIdx / 4;
    return (*(u8 *)(ptr + idx + 0x74) >> ((entryIdx - idx * 4) * 2)) & 3;
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800383B8);


/** @brief Looks up byte from D_8005644B table at index a0 (masked to 8 bits).
 *  @param a0 Table index (only low 8 bits used).
 *  @return The byte value at D_8005644B[a0 & 0xFF].
 */
u8 lookupFieldTable(s32 tableIdx) {
    return D_8005644B[tableIdx & 0xFF];
}


/** @brief Returns halfword from D_800562C8 table indexed by D_80077E5A. */
u16 getCurrentFieldMusic(void) {
    return D_800562C8[D_80077E5A];
}


/** @brief Linear congruential generator: D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039.
 *  @return Bits 16-30 of the new state (0-32767).
 */
s32 fieldRandom(void) {
    D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039;
    return ((u32)D_800562D4 >> 16) & 0x7FFF;
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80038490);


