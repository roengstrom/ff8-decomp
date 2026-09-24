#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"
#include "gamestate.h"
#include "character.h"
#include "tim.h"
#include "field.h"
#include "card.h"
#include "cdread.h"

extern u8 D_8007809B[];
extern u8 g_chocoboWorld;
extern u8 D_80085218;
extern FieldVars *g_fieldVars;
extern u8 D_8005F388[];
extern u8 D_80063388[];
extern s32 D_80085220;
extern u8 D_8005644B[];
extern u16 D_800562C8[];
extern s32 D_800562D4;
extern u8 D_80077EBC;
extern u16 D_80082C0A;
extern u16 D_8005F11C;
extern s32 findNthSetBit(s32, s32, u8 *, s32);
extern s32 func_80021300(void);

/** @brief 0x20-byte free-id table immediately before the D_80077EBC pair list. */
typedef struct {
    /* 0x000 */ u8 freeIds[0x20];
} FreeIdTable; /* sizeof == 0x20 */

#define FREE_ID_TABLE_SIZE 0x20
#define FREE_ID_LIMIT     (FREE_ID_TABLE_SIZE + 1)
#define FREE_ID_ENTRIES   0xC6

void func_800370AC(s32 arg0)
{
    u8 *ptr;
    u8 *base;
    s32 mask;
    s32 i;
    s32 val;
    s32 found;
    s32 one;
    s32 zero;
    zero = 0;
    if (arg0 < FREE_ID_LIMIT) {
        ptr = &D_80077EBC;
        base = ((FreeIdTable *)ptr)[-1].freeIds;
        mask = 0;
        i = 0;
        one = 1;
        do {
            val = *ptr++;
            if (*ptr++ == 0) {
                val = 0;
            }
            if (val != 0 && val < FREE_ID_LIMIT) {
                if (arg0 == val) {
                    return;
                }
                mask |= (one << base[val - 1]);
            }
            i++;
        } while (i < FREE_ID_ENTRIES);
        found = findNthSetBit(~mask, zero, ptr, one);
        arg0 -= 1;
        i = 0;
        do {
            if (found == base[i]) {
                u8 tmp = base[arg0];
                base[arg0] = found;
                base[i] = tmp;
                return;
            }
            i++;
        } while (i < FREE_ID_TABLE_SIZE);
    }
}


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


extern u8 *getCharName(CharacterId charId);
extern CharacterData g_characters[];

/** @brief Variable-width name font: width table + TIM glyph sheet. */
typedef struct {
    s32 widthTableOffset;    /* +0x00 */
    s32 timOffset;           /* +0x04 */
} NameFont;

#define STRIP_STRIDE   0x300
#define STRIP_ROWS     12
#define STRIP_PITCH    0x30

#define SCRATCH_PITCH  0x60
#define SCRATCH_MARGIN 0x0C
#define SCRATCH_USED   (SCRATCH_MARGIN + STRIP_ROWS * SCRATCH_PITCH)

#define GLYPHS_PER_ROW 21
#define GLYPH_WIDTH    6
#define GLYPH_HEIGHT   12
#define SHEET_PITCH    512

void func_80037308(NameFont *font, u8 *out)
{
    u8 scratch[0x490];
    u8 *widths;
    u8 *pixels;
    u8 *dstSlot;
    u8 *pen;
    u8 *scratchPtr;
    u8 *clear;
    u8 *name;
    u8 *src;
    u8 *dst;
    u8 *packSrc;
    CharacterData *ch;
    s32 penX;
    s32 slot;
    s32 width;
    s32 rowOffset;
    s32 colOffset;
    s32 rows;
    s32 count;
    s32 row;
    s32 col;
    u32 code;
    s32 charId;
    u8 b;
    u8 hi;
    u8 lo;

    dstSlot = out;
    clear = dstSlot;

    for (slot = PARTY_SLOT_COUNT * STRIP_STRIDE - 1; slot >= 0; slot--) {
        *clear++ = 0;
    }

    do {
        for (slot = 0; slot < PARTY_SLOT_COUNT; slot++, dstSlot += STRIP_STRIDE) {
            scratchPtr = scratch;
            pen = scratch + SCRATCH_MARGIN;

            charId = g_gameState.mainData.party.party[slot];

            if (charId == PARTY_SLOT_EMPTY) {
                continue;
            }

            penX = 0;

            pixels = (u8 *)font;
            widths = pixels + ((NameFont *)pixels)->widthTableOffset;
            pixels += ((NameFont *)pixels)->timOffset;
            pixels += 8;
            pixels += ((TimSection *)pixels)->len;
            pixels += 0xC;

            for (col = SCRATCH_USED - 1; col >= 0; col--) {
                *scratchPtr++ = 0;
            }

            ch = g_characters;
            ch += charId;
            name = getCharName(ch->characterId);

            while (1) {
                code = *name++;

                if (code == 0) {
                    break;
                }

                if (code < 0x20) {
                    code -= 0x19;
                    code = code * 224;
                    code += *name++;
                }

                code -= 0x20;

                width = widths[code >> 1];

                if (code & 1) {
                    width >>= 4;
                }

                width &= 0xF;

                if (width != 0) {
                    width--;
                }

                dst = pen;
                rows = STRIP_ROWS;

                row = code / GLYPHS_PER_ROW;
                col = code % GLYPHS_PER_ROW;

                code = row * GLYPH_HEIGHT * (SHEET_PITCH / 4);
                colOffset = col * GLYPH_WIDTH;
                src = (pixels + colOffset) + code;

                for (; rows > 0; rows--) {
                    for (col = GLYPH_WIDTH; col > 0; col--) {
                        b = *src++;

                        *dst++ |= b & 0xF;
                        *dst++ |= b >> 4;
                    }

                    dst += SCRATCH_PITCH - GLYPH_WIDTH * 2;
                    src += (SHEET_PITCH / 4) - GLYPH_WIDTH;
                }

                pen += width;
                penX += width;
            }

            pixels = dstSlot + STRIP_PITCH;
            pen++;
            penX = (penX + 2) / 2;

            for (rows = STRIP_ROWS; rows > 0; rows--) {
                packSrc = pen;
                count = penX;
                dst = pixels;

                while (count > 0) {
                    hi = *--packSrc;
                    lo = *--packSrc;
                    *--dst = (lo & 0xF) | (hi << 4);
                    count--;
                }

                pen += SCRATCH_PITCH;
                pixels += STRIP_PITCH;
            }
        }
    } while (0);
}


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


u16 func_800377B4(s32 len, u8 *buf)
{
    u16 table[256];
    u32 crc;
    u32 i;
    u32 val;
    u32 j;
    s32 ci;
    ci = 0xFF;
    do { table[ci] = 0; ci--; } while (ci >= 0);
    crc = 0xFFFF;
    i = 0;
    do {
        val = i << 8;
        j = 0;
        do {
            if (val & 0x8000) val = (val << 1) ^ 0x1021;
            else val <<= 1;
            j++;
            table[i] = val;
        } while (j < 8);
        i++;
    } while (i < 0xFF);
    if (len > 0) {
        do {
            u32 idx;
            idx = *buf++;
            len--;
            idx ^= (crc & 0xFFFF) >> 8;
            crc = table[idx] ^ (crc << 8);
        } while (len > 0);
    }
    return ~crc & 0xFFFF;
}


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


s32 func_800379AC(void *arg0)
{
    u8 buffer[0x80];
    s32 i;
    if (pollCardReady() != 2)
        return 0;
    mcFillFF(buffer);
    if (!writeCardBlock(arg0, buffer, 0))
        return 0;
    mcInitDirUsed(buffer);
    for (i = 0; i < 0xF; i++)
        if (!writeCardBlock(arg0, buffer, i + 1))
            return 0;
    mcInitDirFree(buffer);
    for (i = 0; i < 0x14; i++)
        if (!writeCardBlock(arg0, buffer, i + 0x10))
            return 0;
    mcFillFF(buffer);
    for (i = 0; i < 0x1C; i++)
        if (!writeCardBlock(arg0, buffer, i + 0x24))
            return 0;
    mcInitHeader(buffer);
    if (!writeCardBlock(arg0, buffer, 0))
        return 0;
    markCardBusy(arg0);
    return 1;
}


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


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037D40);


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
 * func_80039678. Sets the completion flag at g_fieldVars + 0xD6.
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
    if ((s8)g_fieldVars->soundBankSelector != 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    func_80039678((s32)table, result, size);
    g_fieldVars->soundLoadComplete = 1;
}


/**
 * @brief Load and apply sound data from disc (variant B).
 *
 * Same as loadSoundBankA, but the bank table selection is inverted:
 * uses D_80063388 when g_fieldVars field 0xC9 is non-zero, and
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
    if ((s8)g_fieldVars->soundBankSelector == 0) {
        table = D_8005F388;
    } else {
        table = D_80063388;
    }
    func_80039678((s32)table, result, size);
    g_fieldVars->soundLoadComplete = 1;
}


INCLUDE_ASM("asm/nonmatchings/gamestate", func_80037FB0);


void func_80038030(s32 arg0) {
    FieldVars *ptr = (FieldVars *)D_800780D8;

    if (!(D_80082C0A & 0x10)) {
        while (sndGetStatus() == 2) {
            func_800393C8();
        }

        if (ptr->soundHandle0 == -1) {
            sndCmd11(0);
        }

        sndCmd40();
        D_80085220 = arg0;
        func_80037FB0(0, ptr->battleMusicId, arg0);

        if (ptr->soundLoadComplete == 0) {
            do {
                func_800393C8();
            } while (ptr->soundLoadComplete == 0);
        }

        D_8005F11C = sndCmd10(toggleSoundBank());
        sndCmdC0(0, 0x7F);
    }

    D_80082C11 = (u8)ptr->soundBankSelector ^ 1;
    sndStopPlayback();
    sndCmdF1();
    sndSetMasterVolume(0x7F);

    if (func_80021300() == 0) {
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
 * both g_fieldEntity.entityIndex and g_fieldVars->memberSlot. Defaults to
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
        g_fieldEntity.entityIndex[i] = 0xFF;
        g_fieldVars->memberSlot[i] = 0xFF;

        ent = D_80085224;
        for (j = 0; j < D_80085388; j++) {
            if (g_gameState.battleParty[i] == ent->field_0x255) {
                g_fieldVars->memberSlot[i] = j;
                g_fieldEntity.entityIndex[i] = j;
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
 * Iterates over D_80085388 entries at stride 0x264 (612 bytes) starting
 * from D_80085224 + 0x160, clearing bits 2 and 6 of the flags word at
 * each entry. Then calls func_800381BC to apply the changes.
 */
void clearEntityFlags(void) {
    s32 i;
    Actor *ent = D_80085224;
    u8 count = D_80085388;

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


INCLUDE_ASM("asm/nonmatchings/gamestate", func_800383B8);


/** @brief Looks up byte from D_8005644B table at index a0 (masked to 8 bits).
 *  @param a0 Table index (only low 8 bits used).
 *  @return The byte value at D_8005644B[a0 & 0xFF].
 */
u8 lookupFieldTable(s32 tableIdx) {
    return D_8005644B[tableIdx & 0xFF];
}


/** @brief Returns halfword from D_800562C8 table indexed by the field message speed. */
u16 getCurrentFieldMusic(void) {
    return D_800562C8[g_gameState.config.fieldMsgSpeed];
}


/** @brief Linear congruential generator: D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039.
 *  @return Bits 16-30 of the new state (0-32767).
 */
s32 fieldRandom(void) {
    D_800562D4 = D_800562D4 * 0x41C64E6D + 0x3039;
    return ((u32)D_800562D4 >> 16) & 0x7FFF;
}


typedef struct {
    s32 payloadSize;
    u8 payload[1];
} LzssData;

/* Taking the typed member address preserves the retail D_80039418+0x28
 * relocation form with gcc 2.7.2; direct member access materializes the
 * struct base and inserts an extra instruction. */
#define LZSS_OUTPUT_SIZE (*(s32 *)&D_80039418.outputSize)

void func_80038490(u8 *src, u8 *dst)
{
  LzssData *data;
  u8 *src_ptr;
  u8 *dst_start;
  s32 bit_cnt;
  s32 flags;
  u8 *src_end;
  u8 *dst_end;
  int new_var;
  u8 *copy_src;
  s32 offset;
  u8 *new_var2;
  s32 temp;
  s32 v0;
  src_ptr = src;
  data = (LzssData *)src;
  bit_cnt = 0;
  flags = bit_cnt;
  dst_start = dst;
  LZSS_OUTPUT_SIZE = 0;
  src_end = src_ptr + data->payloadSize + sizeof(data->payloadSize);
  src_ptr += sizeof(data->payloadSize);
  new_var2 = dst_start;
  while (1)
  {
    if (bit_cnt == 0)
    {
      if (src_ptr >= src_end)
      {
        return;
      }
      bit_cnt = 8;
      flags = *(src_ptr++);
    }
    if (flags & 1)
    {
      if (src_ptr >= src_end)
      {
        return;
      }
      *(dst++) = *(src_ptr++);
      LZSS_OUTPUT_SIZE++;
    }
    else
    {
      if (src_ptr >= src_end)
      {
        return;
      }
      new_var = (dst - new_var2) + 0xFEE;
      offset = *(src_ptr++);
      temp = *(src_ptr++);
      offset |= (temp & 0xF0) << 4;
      v0 = (new_var - offset) & 0xFFF;
      copy_src = dst - v0;
      dst_end = (dst + (temp & 0x0F)) + 3;
      if (copy_src < new_var2)
      {
        do
        {
          *(dst++) = 0;
          copy_src++;
        }
        while (copy_src < new_var2);
      }
      while (dst < dst_end)
      {
        *(dst++) = *(copy_src++);
        LZSS_OUTPUT_SIZE++;
      }

    }
    flags >>= 1;
    bit_cnt--;
  }

}
