#include "common.h"
#include "gamestate.h"
#include "menu.h"

extern MenuDisplayConfig g_menuDisplayCfg;
extern s32 g_menuColor;
extern u32 func_801F0FEC(s32, s32, s32, s32, s32, s32);
extern s32 func_801EF9AC(s32, s32, s32, s32);
extern s32 func_801F6AFC(s32);
extern s32 func_801F7A54(void);
extern s32 drawColorByMenuPalette(s32, s32, s32, s32, s32);
extern void decodeMessage(u8 *, u8 *, s32);
extern u8 D_800780AB;

/**
 * @brief Read tutorial column index 1.
 *
 * @return Byte value of D_801E4EC1.
 */
s32 func_801E2800(void) {
    extern u8 D_801E4EC1;
    return D_801E4EC1;
}

/**
 * @brief Read tutorial column index 2.
 *
 * @return Byte value of D_801E4EC2.
 */
s32 func_801E2810(void) {
    extern u8 D_801E4EC2;
    return D_801E4EC2;
}

/**
 * @brief Load a sub-overlay at an offset from the base address.
 *
 * Dispatches to loadSubOverlay with a base pointer adjusted by a
 * fixed offset determined by the tutorial section index.
 *
 * @param index Tutorial section index (0-6).
 * @param base Base address of the overlay data.
 */
void func_801E2820(s32 index, u8 *base) {
    switch (index) {
    case 0:
        base += 0x1C;
        break;
    case 1:
        base += 0x14;
        break;
    case 2:
        base += 0x18;
        break;
    case 3:
        base += 0x2C;
        break;
    case 4:
        base += 0x30;
        break;
    case 5:
        base += 0x47;
        break;
    case 6:
        base += 0xB4;
        break;
    }

    loadSubOverlay(base, 0x801D1000);
}

/**
 * @brief Load tutorial page indices from table.
 *
 * Reads two bytes from D_801E4EB4[a0] (4-byte stride) and stores
 * them into D_801E4EC1 and D_801E4EC2 respectively.
 *
 * @param a0 Tutorial page table index.
 */
void func_801E28A8(s32 a0) {
    extern u8 D_801E4EB4[];
    extern u8 D_801E4EC1;
    extern u8 D_801E4EC2;
    u8 *entry;

    entry = D_801E4EB4;
    entry = entry + a0 * 4;
    D_801E4EC1 = entry[0];
    D_801E4EC2 = entry[1];
}

/**
 * @brief Read tutorial page index.
 *
 * @return Byte value of D_801E4EC0.
 */
s32 func_801E28D4(void) {
    extern u8 D_801E4EC0;
    return D_801E4EC0;
}

/** @brief Draw inner panel with section id 0xD and clear flag. */
s32 func_801E28E4(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/** @brief Draw inner panel with section id 0xD and set flag. */
s32 func_801E2910(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 1);
}

/** @brief Call func_801F0A34 with a0, 0, 0x22, and a1 * 15 + 0x3E. */
void func_801E293C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x22, a1 * 15 + 0x3E);
}

/**
 * @brief Draw tutorial text with computed CLUT and Y position.
 *
 * Computes a CLUT index from 0x1000 minus field 0x2C via lookup table
 * D_801FA3C8, then calls func_801F0A34 with the computed x/y coordinates.
 *
 * @param a0 OT pointer
 * @param a1 Pointer to tutorial state structure
 */
/** @brief Tutorial callback context structure. */
typedef struct {
    /* 0x00 */ u8 pad00[0x10];
    /* 0x10 */ u16 state;
    /* 0x12 */ u8 pad12[4];
    /* 0x16 */ u16 returnState;
    /* 0x18 */ u8 pad18[8];
    /* 0x20 */ u16 fadeAlpha;
    /* 0x22 */ u8 overlayParam;
    /* 0x23 */ u8 pad23;
    /* 0x24 */ s32 panelHandle;
    /* 0x28 */ s16 fadePos;
    /* 0x2A */ s16 scrollPos;
    /* 0x2C */ s16 fadeProgress;
    /* 0x2E */ s16 scrollAnim;
    /* 0x30 */ u8 targetPage;
    /* 0x31 */ u8 prevPage;
    /* 0x32 */ s8 entryIndex;
    /* 0x33 */ s8 pageCount;
    /* 0x34 */ u8 sectionIndex;
    /* 0x35 */ u8 cursorPos;
    /* 0x36 */ u8 availCount;
    /* 0x37 */ u8 loadParam;
    /* 0x38 */ u8 isReentry;
    /* 0x39 */ u8 availSlots[9];
} TutoState;

/** @brief Tutorial entry (4 bytes). */
typedef struct {
    u8 panelId;
    u8 hasFlag;
    u8 loadCmd;
    u8 pad03;
} TutoEntry;

/** @brief Tutorial section entry (12 bytes). */
typedef struct {
    u8 sectionId;
    u8 bgParam;
    u8 overlayCmd;
    u8 gfDataId;
    u8 charDataId;
    u8 subOvlId1;
    u8 subOvlId2;
    u8 saveFlag;
    u8 fieldFlagId;
    u8 pad09[3];
} TutoSectionEntry;

void func_801E296C(s32 a0, TutoState *data) {
    extern u16 D_801FA3C8[];

    s32 index;
    s32 tableVal;
    s32 scaled;
    s32 val = data->fadeProgress;
    u8 param = data->cursorPos;

    scaled = val;
    index = (tableVal = 0x1000 - val);
    index /= 64;

    tableVal = D_801FA3C8[index];
    scaled = (D_801FA3C8[index] * 190) / 4096;
    func_801F0A34(a0, 0, scaled + 0xA8, (param * 15) + 0x3E);
}

/**
 * @brief Draw tutorial text with table-based X scaling and entry-based Y.
 *
 * Computes Y from the entry index modulo 10, and X from a CLUT lookup
 * table indexed by the scroll position divided by 64.
 *
 * @param a0 OT pointer.
 * @param data Pointer to tutorial state structure.
 */
void func_801E29F8(s32 a0, TutoState *data) {
    extern u16 D_801FA3C8[];

    s32 tableVal;
    u32 scaled;
    u32 height;
    u32 mod = 10;
    s16 pos = data->scrollPos;
    s8 val = (s8)data->entryIndex;

    height = ((val % (tableVal = 10)) * 15) + 0x3E;
    tableVal = D_801FA3C8[(tableVal = pos) / 64];
    scaled = (tableVal * 190) / 4096;

    func_801F0A34(a0, 0, scaled + 0xA8, height);
}

/**
 * @brief Initialize game state from save data buffers.
 *
 * Copies character data from 0x801DB000 and GF data from 0x801DC000
 * into g_gameState, sets initial HP to 9999, copies GF names from
 * 0x801D9000, and initializes party/limit break/config fields.
 * If D_801FABC7 is set, scans characters to build an availability
 * mask for the party selection menu.
 *
 * @param output Tutorial state receiving character availability data.
 */
void func_801E2ABC(TutoState *output) {
    extern u8 D_801FABC7;

    s32 nameSrc = 0x801D9000;
    CharacterData *charSrc = (CharacterData *)0x801DB000;
    GfSaveData *gfSrc = (GfSaveData *)0x801DC000;
    s32 i;
    s32 nameOff;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i] = *charSrc;
        charSrc++;
        g_gameState.chars[i].currentHp = 9999;
        g_gameState.chars[i].statusFlags = 0;
        g_gameState.chars[i].characterId = i;
    }

    for (i = 0, nameOff = 0x50; i < GF_COUNT; i++) {
        g_gameState.gfs[i] = *gfSrc;
        gfSrc++;
        g_gameState.gfs[i].hp = 9999;
        copyString((u8 *)&g_gameState.gfs[i], (u8 *)(nameSrc + nameOff));
        nameOff += 0x44;
    }

    g_gameState.mainData.party.party[0] = 0;
    g_gameState.mainData.party.party[1] = 1;
    g_gameState.mainData.party.party[2] = 4;
    g_gameState.battleParty[0] = 0;
    g_gameState.battleParty[1] = 1;
    g_gameState.battleParty[2] = 4;

    if (D_801FABC7 != 0) {
        s32 mask = 0;
        s32 count = 0;

        for (i = 0; i < CHARACTER_COUNT; i++) {
            s32 bit = 1;

            if (g_gameState.chars[i].exists & 1) {
                mask |= bit << i;
                count++;
            }
        }

        output->fadeAlpha = mask;
        output->pad23 = count;
        g_gameState.mainData.party.party[0] = 1;
        g_gameState.mainData.party.party[1] = 0;
        g_gameState.mainData.party.party[2] = 5;
        g_gameState.battleParty[0] = 0;
        g_gameState.battleParty[1] = 1;
        g_gameState.battleParty[2] = 5;
    }

    g_gameState.mainData.limitBreaks.angeloCompleted = 0x11;
    g_gameState.mainData.limitBreaks.angeloKnown = 0x13;
    g_gameState.mainData.limitBreaks.quistisLimits = 1;
    g_gameState.mainData.limitBreaks.irvineLimits = 1;
    g_gameState.mainData.limitBreaks.selphieLimits = 1;
    g_gameState.config.pad07 = 0;
    g_gameState.mainData.partyLockFlag = 0;
    g_gameState.mainData.party.unknown17 = 0;
    g_gameState.mainData.fieldD20 = 0;
    g_gameState.mainData.limitBreaks.zellLimits = 0x4F;

    for (i = 0; i < 8; i++) {
        g_gameState.mainData.limitBreaks.angeloPoints[i] = 0xFF;
    }

    recalcPartyStats();
    func_801F5440();
}

/**
 * @brief Save game state back to the tutorial save buffer.
 *
 * Copies character data, GF data, config, party/inventory/battle data,
 * and battle party from g_gameState back to the 0x801D9000 buffer.
 * This is the reverse of func_801E2ABC which loads from the buffer
 * into g_gameState.
 */
void func_801E2D3C(void) {
    GameState *dst = (GameState *)0x801D9000;
    s32 i;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        dst->chars[i] = g_gameState.chars[i];
    }

    for (i = 0; i < GF_COUNT; i++) {
        dst->gfs[i] = g_gameState.gfs[i];
    }

    dst->config = g_gameState.config;
    dst->mainData = g_gameState.mainData;

    for (i = 0; i < 4; i++) {
        dst->battleParty[i] = g_gameState.battleParty[i];
    }
}

/**
 * @brief Restore game state from the tutorial save buffer.
 *
 * Copies character data, GF data, config, party/inventory/battle data,
 * and battle party from the 0x801D9000 buffer back into g_gameState.
 * This is the reverse of func_801E2D3C which saves to the buffer.
 * Recalculates party stats after restoring.
 */
void func_801E2EF0(void) {
    GameState *save = (GameState *)0x801D9000;
    s32 i;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i] = save->chars[i];
    }

    for (i = 0; i < GF_COUNT; i++) {
        g_gameState.gfs[i] = save->gfs[i];
    }

    g_gameState.config = save->config;
    g_gameState.mainData = save->mainData;

    for (i = 0; i < 4; i++) {
        g_gameState.battleParty[i] = save->battleParty[i];
    }

    recalcPartyStats();
    func_801F5440();
}

/**
 * @brief Scan tutorial entry table and build list of available entries.
 *
 * Iterates over 9 entries in D_801E4E3C (12-byte stride), calls
 * testFieldFlag to check if each entry's item (byte at offset 8) is
 * available. Available entries' indices are stored sequentially at
 * a0+0x39, and the total count is stored at a0+0x36.
 *
 * @param a0 Pointer to tutorial state structure
 */
void func_801E30C4(u8 *a0) {
    extern u8 D_801E4E3C[];
    s32 i = 0;
    s32 count = 0;
    u8 *table = D_801E4E3C;

    do {
        if (testFieldFlag(table[8]) != 0) {
            *(u8 *)(a0 + count + 0x39) = i;
            count++;
        }
        i++;
        table += 0xC;
    } while (i < 9);

    *(u8 *)(a0 + 0x36) = count;
}

/**
 * @brief Tutorial page state machine.
 *
 * Processes tutorial state transitions including page navigation,
 * fade animations, overlay loading, input handling, and CD reads.
 * Uses a 34-entry switch (states 0x00-0x21) with goto-based
 * re-dispatch for immediate state transitions.
 *
 * @param ctx Tutorial state context.
 */
void func_801E3140(TutoState *ctx) {
    extern MenuDisplayConfig g_menuDisplayCfg;
    extern TutoEntry D_801E4E18[];
    extern TutoSectionEntry D_801E4E3C[];
    extern u8 D_801E4EC0;
    extern u16 D_801E4EAC;
    extern u8 D_800780AB;
    volatile extern s32 D_8008514C;
    extern u8 D_801FABC7;

    u16 inputNew = g_menuDisplayCfg.inputNew;
    u16 inputRepeat = g_menuDisplayCfg.inputRepeat;
    u16 *statePtr = &ctx->state;
    u16 state = ctx->state;

top:
    switch (state & 0xFFFF) {
    case 0x00:
        if (ctx->isReentry != 0) {
            *statePtr = 0x1D;
        } else {
            *statePtr = 1;
        }
        break;

    case 0x01:
        if (pollCdReadStatus() != 0) {
            break;
        }
        *statePtr = 2;
        break;

    case 0x02:
        ctx->fadePos += 0x100;
        if ((s16)ctx->fadePos >= 0x1000) {
            ctx->fadePos = 0x1000;
            *statePtr = 3;
        }
        func_801F0948((s16)ctx->fadePos);
        func_801E293C(1, 0);
        break;

    case 0x03:
        *statePtr = 4;
        /* fall through */
    case 0x04: {
        TutoEntry *entry;
        u8 section;

        section = func_801F6768(inputNew, 8, ctx->sectionIndex);
        ctx->sectionIndex = section;
        func_801F12F0();

        if ((&D_801E4E18[ctx->sectionIndex])->hasFlag != 0) {
            ctx->panelHandle = func_801E2910((&D_801E4E18[ctx->sectionIndex])->panelId);
        } else {
            ctx->panelHandle = func_801F6AD0((&D_801E4E18[ctx->sectionIndex])->panelId);
        }
        func_801E293C(1, ctx->sectionIndex);

        if (inputRepeat & 0x40) {
            D_801E4EC0 = 0xFF;

            if ((&D_801E4E18[ctx->sectionIndex])->panelId == 0x3D) {
                if (func_801F7A54() < 0) {
                    sendSpuCommand(5);
                    break;
                }
            }

            if ((&D_801E4E18[ctx->sectionIndex])->panelId == 0x3E) {
                if (D_800780AB == 0) {
                    sendSpuCommand(5);
                    break;
                }
                sendSpuCommand(2);
                state = 5;
                goto top;
            }

            if ((&D_801E4E18[ctx->sectionIndex])->loadCmd != 0) {
                sendSpuCommand(2);
                ctx->returnState = 0x16;
                *statePtr = 0xE;
            } else {
                sendSpuCommand(5);
            }
        }

        if (inputRepeat & 0x10) {
            sendSpuCommand(3);
            *statePtr = 0x20;
        }
        break;
    }

    case 0x05:
        ctx->panelHandle = func_801E28E4(0x19);
        *statePtr = 6;
        /* fall through */
    case 0x06:
        ctx->scrollPos -= 0x100;
        if ((s16)ctx->scrollPos <= 0) {
            ctx->scrollPos = 0;
            *statePtr = 7;
        }
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);
        break;

    case 0x07: {
        s8 val = ctx->entryIndex;
        s32 page = val / 10;
        s8 rem = val % 10;
        s8 newRem;

        newRem = func_801F6768(inputNew, 10, rem);
        ctx->entryIndex = page * 10;
        ctx->entryIndex = ctx->entryIndex + newRem;
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);

        if (D_800780AB >= 0xB) {
            if (inputNew & 0x2000) {
                *statePtr = 0xA;
            }
            if (inputNew & 0x8000) {
                *statePtr = 8;
            }
        }

        if (inputRepeat & 0x10) {
            sendSpuCommand(3);
            *statePtr = 0xC;
        }

        if (inputRepeat & 0x40) {
            if (ctx->entryIndex < D_800780AB) {
                s32 index = ctx->entryIndex;
                sendSpuCommand(2);
                D_801E4EC0 = ctx->entryIndex;
                do { ctx->returnState = 0x16; *statePtr = 0xE; } while (0);
            } else {
                sendSpuCommand(5);
            }
        }
        break;
    }

    case 0x08: {
        s8 val;
        s32 page;
        s32 rem;
        s32 newPage;
        u8 prevPage;

        sendSpuCommand(1);
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);

        val = (s8)ctx->entryIndex;
        page = val / 10;
        rem = val % 10;
        newPage = page;
        newPage--;
        if (newPage < 0) {
            newPage = ctx->pageCount - 1;
        }
        prevPage = ctx->targetPage;
        ctx->entryIndex = newPage * 10 + rem;
        ctx->scrollAnim = -0xE67;
        ctx->targetPage = newPage;
        ctx->prevPage = prevPage;
        *statePtr = 9;
        break;
    }

    case 0x09:
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);
        ctx->scrollAnim += 0x199;
        if ((s16)ctx->scrollAnim >= 0) {
            ctx->scrollAnim = 0;
            *statePtr = 7;
        }
        if (inputRepeat & 0x8000) {
            *statePtr = 8;
        }
        if (inputRepeat & 0x2000) {
            *statePtr = 0xA;
        }
        break;

    case 0x0A: {
        s8 val;
        s32 page;
        s32 rem;
        s32 newPage;
        u8 prevPage;

        sendSpuCommand(1);
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);

        val = (s8)ctx->entryIndex;
        page = val / 10;
        rem = val % 10;
        newPage = page;
        newPage++;
        if (newPage > ctx->pageCount - 1) {
            newPage = 0;
        }
        prevPage = ctx->targetPage;
        ctx->entryIndex = newPage * 10 + rem;
        ctx->scrollAnim = 0xE67;
        ctx->targetPage = newPage;
        ctx->prevPage = prevPage;
        *statePtr = 0xB;
        break;
    }

    case 0x0B:
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);
        ctx->scrollAnim -= 0x199;
        if ((s16)ctx->scrollAnim <= 0) {
            ctx->scrollAnim = 0;
            *statePtr = 7;
        }
        if (inputRepeat & 0x8000) {
            *statePtr = 8;
        }
        if (inputRepeat & 0x2000) {
            *statePtr = 0xA;
        }
        break;

    case 0x0C:
        func_801E293C(0, ctx->sectionIndex);
        *statePtr = 0xD;
        /* fall through */
    case 0x0D:
        ctx->scrollPos += 0x100;
        if ((s16)ctx->scrollPos >= 0x1000) {
            ctx->scrollPos = 0x1000;
            *statePtr = 4;
        }
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);
        break;

    case 0x0E: {
        u8 cmd;

        cmd = (&D_801E4E18[ctx->sectionIndex])->loadCmd;

        switch (cmd) {
        case 0x19:
            loadSubOverlay(7, 0x801CD000);
            loadOverlayWithTimCallback(0xC, 0x801D1000);
            func_801E28A8(0);
            break;
        case 0x1A:
            loadSubOverlay(7, 0x801CD000);
            loadOverlayWithTimCallback(0xC, 0x801D1000);
            func_801E28A8(1);
            break;
        case 0x1F:
            loadSubOverlay(7, 0x801CD000);
            loadOverlayWithTimCallback(0xC, 0x801D1000);
            func_801E28A8(2);
            break;
        case 0x16:
            loadSubOverlay(D_801E4EAC, 0x801CD000);
            break;
        case 0x17:
            loadSubOverlay(0x5F, 0x801CD000);
            break;
        case 0xFF:
            state = 0x18;
            goto top;
        }

        func_801F0BF8((&D_801E4E18[ctx->sectionIndex])->loadCmd);
        *statePtr = 0xF;

        if ((&D_801E4E18[ctx->sectionIndex])->panelId == 0x3E && (&D_801E4E18[ctx->sectionIndex])->hasFlag == 0) {
            func_801E293C(0, ctx->sectionIndex);
            func_801E29F8(1, ctx);
        } else {
            func_801E293C(1, ctx->sectionIndex);
        }
        break;
    }

    case 0x0F: {
        TutoEntry *entry;

        ctx->fadePos -= 0x100;
        if ((s16)ctx->fadePos <= 0) {
            ctx->fadePos = 0;
            if (pollCdReadStatus() == 0) {
                ctx->loadParam = (&D_801E4E18[ctx->sectionIndex])->loadCmd;
                *statePtr = 0x11;
            }
        }
        func_801F0948((s16)ctx->fadePos);
        func_801F1DB0((s16)ctx->fadePos);

        entry = &D_801E4E18[ctx->sectionIndex];
        if (entry->panelId == 0x3E && entry->hasFlag == 0) {
            func_801E293C(0, ctx->sectionIndex);
            func_801E29F8(1, ctx);
        } else {
            func_801E293C(1, ctx->sectionIndex);
        }
        break;
    }

    case 0x10:
        ctx->fadePos -= 0x100;
        if ((s16)ctx->fadePos <= 0) {
            ctx->fadePos = 0;
            if (pollCdReadStatus() == 0) {
                *statePtr = 0x11;
            }
        }
        func_801F0948((s16)ctx->fadePos);
        func_801F1DB0((s16)ctx->fadePos);
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x11:
        func_801F0C5C(ctx->loadParam, ctx);
        *statePtr = 0x12;
        break;

    case 0x12: {
        TutoEntry *entry;

        if (func_801F0D84() != 0x14) {
            if (func_801F0D84() != 0x1E) {
                break;
            }
        }
        D_8008514C = 0xD;
        func_801F010C(0x180);
        func_801F1DBC(0x37);
        ctx->scrollPos = 0x1000;
        func_801F12F0();

        ctx->pageCount = D_800780AB;
        ctx->pageCount = (ctx->pageCount + 9) / 10;

        if ((&D_801E4E18[ctx->sectionIndex])->hasFlag != 0) {
            ctx->panelHandle = func_801E2910((&D_801E4E18[ctx->sectionIndex])->panelId);
        } else {
            ctx->panelHandle = func_801F6AD0((&D_801E4E18[ctx->sectionIndex])->panelId);
        }
        *statePtr = 0x13;
        break;
    }

    case 0x13:
        if (pollCdReadStatus() != 0) {
            break;
        }
        *statePtr = 0x14;
        break;

    case 0x14:
        if (pollCdReadStatus() != 0) {
            break;
        }
        *statePtr = 0x15;
        break;

    case 0x15: {
        TutoEntry *entry;

        if (ctx->isReentry != 0) {
            func_801F12F0();
            func_801E2EF0();
            state = 0x20;
            goto top;
        }
        if ((&D_801E4E18[ctx->sectionIndex])->loadCmd == 0xFF) {
            func_801E2EF0();
        }
        *statePtr = ctx->returnState;
        break;
    }

    case 0x16:
        ctx->fadePos += 0x100;
        func_801F12F0();
        if ((s16)ctx->fadePos >= 0x1000) {
            ctx->fadePos = 0x1000;
            *statePtr = 3;
        }
        func_801F0948((s16)ctx->fadePos);
        func_801F1DB0((s16)ctx->fadePos);
        func_801E293C(1, ctx->sectionIndex);
        break;

    case 0x17:
        ctx->fadePos += 0x100;
        func_801F12F0();
        if ((s16)ctx->fadePos >= 0x1000) {
            ctx->fadePos = 0x1000;
            *statePtr = 0x1A;
        }
        func_801F0948((s16)ctx->fadePos);
        func_801F1DB0((s16)ctx->fadePos);
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x18:
        ctx->fadeProgress = 0;
        *statePtr = 0x19;
        /* fall through */
    case 0x19:
        ctx->fadeProgress += 0x100;
        if ((s16)ctx->fadeProgress >= 0x1000) {
            ctx->fadeProgress = 0x1000;
            *statePtr = 0x1A;
        }
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x1A:
        *statePtr = 0x1B;
        /* fall through */
    case 0x1B: {
        TutoSectionEntry *table = D_801E4E3C;
        u8 slotIdx;
        u8 entryIdx;

        slotIdx = func_801F6768(inputNew, ctx->availCount, ctx->cursorPos);
        ctx->cursorPos = slotIdx;
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);

        entryIdx = ctx->availSlots[ctx->cursorPos];
        table = &table[entryIdx];
        ctx->panelHandle = func_801E2910(table->sectionId);

        if (inputRepeat & 0x40) {
            sendSpuCommand(2);
            state = 0x1C;
            goto top;
        }
        if (inputRepeat & 0x10) {
            sendSpuCommand(3);
            state = 0x1E;
            goto top;
        }
        break;
    }

    case 0x1C: {
        TutoSectionEntry *table = D_801E4E3C;
        u8 entryIdx;

        entryIdx = ctx->availSlots[ctx->cursorPos];
        table = &table[entryIdx];
        func_801E2D3C();
        ctx->overlayParam = table->overlayCmd;
        D_801FABC7 = table->saveFlag;

        loadSubOverlay(table->subOvlId1, 0x801CD000);
        loadSubOverlay(table->subOvlId2, 0x801D1000);
        loadSubOverlay(table->charDataId, 0x801DB000);
        loadSubOverlay(table->gfDataId, 0x801DC000);
        func_801F0BF8(table->bgParam);
        ctx->returnState = 0x17;
        ctx->loadParam = table->bgParam;
        *statePtr = 0x10;
        break;
    }

    case 0x1D: {
        TutoSectionEntry *table = D_801E4E3C;
        s32 idx;

        idx = func_801F000C();
        table = &table[idx];
        D_801FABC7 = table->saveFlag;
        func_801E2D3C();
        ctx->overlayParam = table->overlayCmd;

        loadSubOverlay(table->subOvlId1, 0x801CD000);
        loadSubOverlay(table->subOvlId2, 0x801D1000);
        loadSubOverlay(table->charDataId, 0x801DB000);
        loadSubOverlay(table->gfDataId, 0x801DC000);
        func_801F0BF8(table->bgParam);
        ctx->returnState = 0x17;
        ctx->loadParam = table->bgParam;
        *statePtr = 0x10;
        break;
    }

    case 0x1E:
        *statePtr = 0x1F;
        /* fall through */
    case 0x1F:
        ctx->fadeProgress -= 0x100;
        if ((s16)ctx->fadeProgress <= 0) {
            ctx->fadeProgress = 0;
            *statePtr = 3;
        }
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x20:
        loadOverlayWithTimCallback(9, 0x801CD000);
        loadOverlayWithTimCallback(0xA, 0x801D5000);
        *statePtr = 0x21;
        /* fall through */
    case 0x21:
        ctx->fadePos -= 0x100;
        if ((s16)ctx->fadePos <= 0) {
            ctx->fadePos = 0;
            if (pollCdReadStatus() == 0) {
                func_801F18FC(ctx);
                func_801F0BB0();
            }
        }
        func_801F0948((s16)ctx->fadePos);
        func_801E293C(1, ctx->sectionIndex);
        break;
    }

    /* Common tail — check for menu return signal */
    if (func_801EFFB8() == 0x1A) {
        if (func_801F0D84() == 0x14) {
            func_801F1DB0((s16)ctx->fadePos);
        }
    }
}

/**
 * @brief Render a tutorial panel header with text and border.
 *
 * Loads the text string for entry 0x10, draws it offset from the given
 * position, configures the display panel (icon 0x57), and renders the
 * border via func_801EF9AC.
 *
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @return Updated OT cursor position.
 */
s32 func_801E3EC0(s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 textAddr;
    s32 xOff = x + 0x20;
    s32 yOff = y + 6;

    textAddr = func_801E28E4(0x10);
    cursorY = func_801F0FEC(renderCtx, cursorY, xOff, yOff, textAddr, 7);

    cfg->iconType = 0x57;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->w = 0xF4;
    cfg->y = y;
    cfg->h = 0x16;

    return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Render a tutorial content panel with decoded message text.
 *
 * If the state has a message pointer (panelHandle), decodes the message
 * into a local buffer and renders it. Configures the display panel
 * (icon 0x55) and draws the border.
 *
 * @param state Tutorial state (panelHandle at 0x24 holds message pointer).
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X position.
 * @param y Panel Y position (from stack).
 * @return Updated OT cursor position.
 */
u32 func_801E3F8C(TutoState *state, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 xOff = x + 0xA;
    s32 yOff = y + 9;
    s32 w = 0x144;
    s32 h = 0x1A;

    if (state->panelHandle != 0) {
        u8 textBuf[128];
        decodeMessage((u8 *)state->panelHandle, textBuf, -1);
        cursorY = func_801F0FEC(renderCtx, cursorY, xOff, yOff, (s32)textBuf, 7);
    }

    cfg->iconType = 0x55;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = w;
    cfg->h = h;

    return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Render the tutorial section list panel.
 *
 * Iterates over the 8 tutorial section entries in D_801E4E18 and renders
 * each section name with an appropriate color. Entries with loadCmd==0,
 * Chocobo World unavailable (panelId 0x3D, checked via func_801F7A54), or
 * PocketStation absent (panelId 0x3E, checked via g_gameState + 0xD33 /
 * D_800780AB) are rendered in grey (color 1); normal entries use color 7.
 * Also configures the display panel and draws the border.
 *
 * @param state Unused tutorial state pointer.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X coordinate.
 * @param y Panel Y coordinate (5th arg, passed on stack).
 * @return Updated OT cursor position.
 */
u32 func_801E4080(void *state, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    extern TutoEntry D_801E4E18[];
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    TutoEntry *table = D_801E4E18;
    s32 xOff = x + 0xA;
    s32 yOff = y + 5;
    s32 i;
    s32 color;
    s32 textAddr;

    for (i = 0; i < 8; i++) {
        color = 1;
        if (table[i].loadCmd) {
            color = 7;
        }
        if (table[i].panelId == 0x3D) {
            if (func_801F7A54() < 0) {
                color = 1;
            }
        }
        if (table[i].panelId == 0x3E) {
            if (g_gameState.mainData.tutoEntryCount == 0) {
                color = 1;
            }
        }
        if (table[i].hasFlag) {
            textAddr = func_801E28E4(table[i].panelId);
        } else {
            textAddr = func_801F6AFC(table[i].panelId);
        }
        cursorY = func_801F0FEC(renderCtx, cursorY, xOff, yOff, textAddr, color);
        yOff += 0xF;
    }

    cursorY++; cursorY--; /* regalloc hack: boost cursorY use count above yOff so it claims s2 */

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = x;
    cfg->y = y;
    cfg->w = 0x86;
    cfg->h = 0x80;

    return func_801EF9AC(renderCtx, cursorY, 0x1000, g_menuColor);
}

/**
 * @brief Render a single tutorial entry in a 10-per-page list.
 *
 * Computes the global entry index (index * 10 + startY) and returns
 * cursorY unchanged if it's beyond the total tutorial entry count.
 * Otherwise, renders the entry's label text at a per-slot Y offset
 * and draws the 1-indexed entry number via drawColorByMenuPalette
 * (with packed X/Y coordinate).
 *
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param index Zero-based page index.
 * @param startY Slot within the page (0-9).
 * @param x Base X coordinate (5th arg, passed on stack).
 * @return Updated OT cursor position (unchanged if entry out of range).
 */
u32 func_801E4214(s32 renderCtx, s32 cursorY, s32 index, s32 startY, s32 x) {
    s32 panelX;
    u16 xCoord;
    s32 yPos;
    u8 *text;
    s32 endPos;
    s32 xPos;

    endPos = (index * 10) + startY;
    if (endPos >= D_800780AB) {
        return cursorY;
    }

    endPos += 1;
    text = func_801E28E4(0x17);
    xPos = ((long long)g_menuDisplayCfg.x) + (x + 0xA);
    yPos = g_menuDisplayCfg.y + 5;
    yPos += startY * 15;

    cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, text, 7);

    panelX = g_menuDisplayCfg.x;
    xCoord = panelX + (x + 0xC0);
    return drawColorByMenuPalette(renderCtx, cursorY, ((yPos << 15) << 1) | xCoord, endPos, 7);
}

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E431C); /* 0xB8 */

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E43D4); /* 0x1C4 */

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4598); /* 0x144 */

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E46DC); /* 0x11C */

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E47F8); /* 0xC8 */

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E48C0); /* 0x310 */

INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4BD0); /* 0xE0 */

/**
 * @brief Conditionally call func_801E4BD0 if field 0x20 is non-zero.
 * @param a0 Pointer to structure with s16 field at offset 0x20
 * @param a1 First argument passed to func_801E4BD0
 * @param a2 Second argument passed to func_801E4BD0
 */
INCLUDE_ASM("asm/ovl/menututo/nonmatchings/menututo", func_801E4CB0); /* 0x34 */

/**
 * @brief Initialize tutorial menu: register callbacks, clear state, and enter.
 *
 * Registers func_801E48C0 and func_801E4CB0 as callbacks via func_801F179C.
 * If registration succeeds, clears fadeAlpha and pageIndex,
 * calls func_801F010C(0x140) for display setup, then enters via func_801E48C0.
 */
void func_801E4CE4(void) {
    extern void func_801E48C0();
    extern void func_801E4CB0();
    TutoState *ctx = (TutoState *)func_801F179C((s32)func_801E48C0, (s32)func_801E4CB0);

    if (ctx != NULL) {
        ctx->fadeAlpha = 0;
        *(u16 *)&ctx->overlayParam = 0;
        func_801F010C(0x140);
        func_801E48C0(ctx);
    }
}
