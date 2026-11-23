#include "common.h"
#include "battle_results/number.h"
#include "btl_anim.h"
#include "gamestate.h"
#include "overlay.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menumain2b.h"
#include "menumain3.h"
#include "menututo.h"
#include "numstr.h"
#include "party_stats.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgte.h"
#include "snd_sfx.h"
#include "ui/sprite.h"
#include "ui/text.h"

static void func_801E28A8(s32 a0);
static u8 *func_801E28E4(s32 a0);
static u8 *func_801E2910(s32 a0);
static void func_801E293C(s32 a0, s32 a1);
static void func_801E296C(s32 a0, TutoState *data);
static void func_801E29F8(s32 a0, TutoState *data);
static void func_801E2D3C(void);
static void func_801E2EF0(void);
static void func_801E30C4(TutoState *ctx);
static void func_801E3140(TutoState *ctx);
static s32 func_801E3EC0(s32 renderCtx, s32 cursorY, s32 x, s32 y);
static u32 func_801E3F8C(TutoState *state, s32 renderCtx, s32 cursorY, s32 x, s32 y);
static u32 func_801E4080(void *state, s32 renderCtx, s32 cursorY, s32 x, s32 y);
static s32 func_801E4214(s32 renderCtx, s32 cursorY, s32 index, s32 startY, s32 x);
static u32 func_801E431C(TutoState *state, s32 renderCtx, s32 cursorY, s16 x, s16 y);
static s32 func_801E43D4(TutoState *state, s32 renderCtx, s32 cursorY);
static s32 func_801E4598(TutoState *state, s32 renderCtx, s32 cursorY);
static void func_801E48C0(TutoState *self);
static TSPRT *func_801E4BD0(u32 *ot, TSPRT *prim, s32 fade);
static s32 func_801E4CB0(TutoState *state, s32 renderCtx, s32 cursorY);

/**
 * @brief Read tutorial column index 1.
 * @return Byte value of D_801E4EC1.
 */
s32 func_801E2800(void) {
    return D_801E4EC1;
}

/**
 * @brief Read tutorial column index 2.
 * @return Byte value of D_801E4EC2.
 */
s32 func_801E2810(void) {
    return D_801E4EC2;
}

/**
 * @brief Load a sub-overlay at an offset from the base address.
 * @param index Tutorial section index (0-6).
 * @param base Value the section offset is added to.
 */
void func_801E2820(s32 index, s32 base) {
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
 * @param a0 Tutorial page table index.
 */
static void func_801E28A8(s32 a0) {
    TutoPageEntry *entry = D_801E4EB4;

    entry += a0;
    D_801E4EC1 = entry->unk0;
    D_801E4EC2 = entry->unk1;
}

/**
 * @brief Read tutorial page index.
 * @return Byte value of D_801E4EC0.
 */
s32 func_801E28D4(void) {
    return D_801E4EC0;
}

/** @brief Look up string @p a0 in menu text category 0xD. */
static u8 *func_801E28E4(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/** @brief Look up string @p a0 in menu text category 0xD, with the last flag set. */
static u8 *func_801E2910(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 1);
}

static void func_801E293C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x22, a1 * 15 + 0x3E);
}

/**
 * @brief Draw tutorial text with computed CLUT and Y position.
 * @param a0 OT pointer.
 * @param data Pointer to tutorial state structure.
 */
static void func_801E296C(s32 a0, TutoState *data) {
    s32 xOffset = data->fadeProgress;
    u8 cursorPos = data->cursorPos;

    xOffset = 0x1000 - xOffset;
    xOffset = D_801FA3C8[xOffset / 64];
    xOffset = xOffset * 190 / 4096;
    func_801F0A34(a0, 0, xOffset + 0xA8, cursorPos * 15 + 0x3E);
}

/**
 * @brief Draw tutorial text with table-based X scaling and entry-based Y.
 * @param a0 OT pointer.
 * @param data Pointer to tutorial state structure.
 */
static void func_801E29F8(s32 a0, TutoState *data) {
    s16 scrollPos = data->scrollPos;
    s32 entryIndex = data->entryIndex;
    s32 y = entryIndex % 10 * 15 + 0x3E;
    s32 tableValue = scrollPos;
    s16 xOffset;

    tableValue = D_801FA3C8[tableValue / 64];
    xOffset = tableValue * 190 / 4096;
    func_801F0A34(a0, 0, xOffset + 0xA8, y);
}

/**
 * @brief Initialize game state from save data buffers.
 * @param output Tutorial state receiving character availability data.
 */
void func_801E2ABC(TutoState *output) {
    GameState *save = (GameState *)0x801D9000;
    CharacterData *charSrc = (CharacterData *)0x801DB000;
    GfSaveData *gfSrc = (GfSaveData *)0x801DC000;
    s32 i;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i] = *charSrc;
        charSrc++;
        g_gameState.chars[i].currentHp = 9999;
        g_gameState.chars[i].statusFlags = 0;
        g_gameState.chars[i].characterId = i;
    }

    for (i = 0; i < GF_COUNT; i++) {
        g_gameState.gfs[i] = *gfSrc;
        gfSrc++;
        g_gameState.gfs[i].hp = 9999;
        copyString(g_gameState.gfs[i].name, save->gfs[i].name);
    }

    g_gameState.mainData.party.partyMembers[0] = 0;
    g_gameState.mainData.party.partyMembers[1] = 1;
    g_gameState.mainData.party.partyMembers[2] = 4;
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
        output->pageIndex.b.hi = count;
        g_gameState.mainData.party.partyMembers[0] = 1;
        g_gameState.mainData.party.partyMembers[1] = 0;
        g_gameState.mainData.party.partyMembers[2] = 5;
        g_gameState.battleParty[0] = 0;
        g_gameState.battleParty[1] = 1;
        g_gameState.battleParty[2] = 5;
    }

    g_gameState.mainData.limitBreaks.angeloCompleted = 0x11;
    g_gameState.mainData.limitBreaks.angeloKnown = 0x13;
    g_gameState.mainData.limitBreaks.quistisLimits = 1;
    g_gameState.mainData.limitBreaks.irvineLimits = 1;
    g_gameState.mainData.limitBreaks.selphieLimits = 1;
    g_gameState.config.sealedAbilities = 0;
    g_gameState.mainData.partyLockFlag = 0;
    g_gameState.mainData.party.trickLearning = 0;
    g_gameState.mainData.fieldD20 = 0;
    g_gameState.mainData.limitBreaks.zellLimits = 0x4F;

    for (i = 0; i < 8; i++) {
        g_gameState.mainData.limitBreaks.angeloPoints[i] = 0xFF;
    }

    recalcPartyStats();
    func_801F5440();
}

/** @brief Save game state back to the tutorial save buffer. */
static void func_801E2D3C(void) {
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

/** @brief Restore game state from the tutorial save buffer. */
static void func_801E2EF0(void) {
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

/** @brief Scan tutorial entry table and build list of available entries. */
static void func_801E30C4(TutoState *ctx) {
    s32 i;
    s32 count;

    for (i = 0, count = 0; i < 9; i++) {
        if (testFieldFlag(D_801E4E3C[i].fieldFlagId) != 0) {
            ctx->availSlots[count] = i;
            count++;
        }
    }

    ctx->availCount = count;
}

/**
 * @brief Tutorial page state machine.
 * @param ctx Tutorial state context.
 */
static void func_801E3140(TutoState *ctx) {
    u16 inputRepeat = g_menuDisplayCfg.inputRepeat;
    u16 inputNew = g_menuDisplayCfg.inputNew;
    u16 *statePtr = &ctx->state;
    u16 state = ctx->state;

top:
    switch (state) {
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
        if (ctx->fadePos >= 0x1000) {
            ctx->fadePos = 0x1000;
            *statePtr = 3;
        }
        func_801F0948(ctx->fadePos);
        func_801E293C(1, 0);
        break;

    case 0x03:
        *statePtr = 4;
        // fall through
    case 0x04:
        ctx->sectionIndex = func_801F6768(inputRepeat, 8, ctx->sectionIndex);
        func_801F12F0();

        if (D_801E4E18[ctx->sectionIndex].hasFlag != 0) {
            ctx->panelHandle = func_801E2910(D_801E4E18[ctx->sectionIndex].panelId);
        } else {
            ctx->panelHandle = func_801F6AD0(D_801E4E18[ctx->sectionIndex].panelId);
        }
        func_801E293C(1, ctx->sectionIndex);

        if (inputNew & PADRdown) {
            D_801E4EC0 = 0xFF;

            if (D_801E4E18[ctx->sectionIndex].panelId == 0x3D) {
                if (func_801F7A54() < 0) {
                    sendSpuCommand(5);
                    break;
                }
            }

            if (D_801E4E18[ctx->sectionIndex].panelId == 0x3E) {
                if (g_gameState.mainData.tutoEntryCount == 0) {
                    sendSpuCommand(5);
                    break;
                }
                sendSpuCommand(2);
                state = 5;
                goto top;
            }

            if (D_801E4E18[ctx->sectionIndex].loadCmd != 0) {
                sendSpuCommand(2);
                ctx->returnState = 0x16;
                *statePtr = 0xE;
            } else {
                sendSpuCommand(5);
            }
        }

        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0x20;
        }
        break;

    case 0x05:
        ctx->panelHandle = func_801E28E4(0x19);
        *statePtr = 6;
        // fall through
    case 0x06:
        ctx->scrollPos -= 0x100;
        if (ctx->scrollPos <= 0) {
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

        newRem = func_801F6768(inputRepeat, 10, rem);
        ctx->entryIndex = page * 10;
        ctx->entryIndex = ctx->entryIndex + newRem;
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);

        if (g_gameState.mainData.tutoEntryCount >= 0xB) {
            if (inputRepeat & PADLright) {
                *statePtr = 0xA;
            }
            if (inputRepeat & PADLleft) {
                *statePtr = 8;
            }
        }

        if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0xC;
        }

        if (inputNew & PADRdown) {
            if (ctx->entryIndex < g_gameState.mainData.tutoEntryCount) {
                sendSpuCommand(2);
                D_801E4EC0 = ctx->entryIndex;
                // matching hack
                do {
                    ctx->returnState = 0x16;
                    *statePtr = 0xE;
                } while (0);
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

        val = ctx->entryIndex;
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
        if (ctx->scrollAnim >= 0) {
            ctx->scrollAnim = 0;
            *statePtr = 7;
        }
        if (inputNew & PADLleft) {
            *statePtr = 8;
        }
        if (inputNew & PADLright) {
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

        val = ctx->entryIndex;
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
        if (ctx->scrollAnim <= 0) {
            ctx->scrollAnim = 0;
            *statePtr = 7;
        }
        if (inputNew & PADLleft) {
            *statePtr = 8;
        }
        if (inputNew & PADLright) {
            *statePtr = 0xA;
        }
        break;

    case 0x0C:
        func_801E293C(0, ctx->sectionIndex);
        *statePtr = 0xD;
        // fall through
    case 0x0D:
        ctx->scrollPos += 0x100;
        if (ctx->scrollPos >= 0x1000) {
            ctx->scrollPos = 0x1000;
            *statePtr = 4;
        }
        func_801E293C(0, ctx->sectionIndex);
        func_801E29F8(1, ctx);
        break;

    case 0x0E: {
        u8 cmd;

        cmd = D_801E4E18[ctx->sectionIndex].loadCmd;

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
            loadSubOverlay(D_801E4EAC[0], 0x801CD000);
            break;
        case 0x17:
            loadSubOverlay(0x5F, 0x801CD000);
            break;
        case 0xFF:
            state = 0x18;
            goto top;
        }

        func_801F0BF8(D_801E4E18[ctx->sectionIndex].loadCmd);
        *statePtr = 0xF;

        if (D_801E4E18[ctx->sectionIndex].panelId == 0x3E && D_801E4E18[ctx->sectionIndex].hasFlag == 0) {
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
        if (ctx->fadePos <= 0) {
            ctx->fadePos = 0;
            if (pollCdReadStatus() == 0) {
                ctx->loadParam = D_801E4E18[ctx->sectionIndex].loadCmd;
                *statePtr = 0x11;
            }
        }
        func_801F0948(ctx->fadePos);
        func_801F1DB0(ctx->fadePos);

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
        if (ctx->fadePos <= 0) {
            ctx->fadePos = 0;
            if (pollCdReadStatus() == 0) {
                *statePtr = 0x11;
            }
        }
        func_801F0948(ctx->fadePos);
        func_801F1DB0(ctx->fadePos);
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x11:
        func_801F0C5C(ctx->loadParam, ctx);
        *statePtr = 0x12;
        break;

    case 0x12:
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

        ctx->pageCount = g_gameState.mainData.tutoEntryCount;
        ctx->pageCount = (ctx->pageCount + 9) / 10;

        if (D_801E4E18[ctx->sectionIndex].hasFlag != 0) {
            ctx->panelHandle = func_801E2910(D_801E4E18[ctx->sectionIndex].panelId);
        } else {
            ctx->panelHandle = func_801F6AD0(D_801E4E18[ctx->sectionIndex].panelId);
        }
        *statePtr = 0x13;
        break;

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

    case 0x15:
        if (ctx->isReentry != 0) {
            func_801F12F0();
            func_801E2EF0();
            state = 0x20;
            goto top;
        }
        if (D_801E4E18[ctx->sectionIndex].loadCmd == 0xFF) {
            func_801E2EF0();
        }
        *statePtr = ctx->returnState;
        break;

    case 0x16:
        ctx->fadePos += 0x100;
        func_801F12F0();
        if (ctx->fadePos >= 0x1000) {
            ctx->fadePos = 0x1000;
            *statePtr = 3;
        }
        func_801F0948(ctx->fadePos);
        func_801F1DB0(ctx->fadePos);
        func_801E293C(1, ctx->sectionIndex);
        break;

    case 0x17:
        ctx->fadePos += 0x100;
        func_801F12F0();
        if (ctx->fadePos >= 0x1000) {
            ctx->fadePos = 0x1000;
            *statePtr = 0x1A;
        }
        func_801F0948(ctx->fadePos);
        func_801F1DB0(ctx->fadePos);
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x18:
        ctx->fadeProgress = 0;
        *statePtr = 0x19;
        // fall through
    case 0x19:
        ctx->fadeProgress += 0x100;
        if (ctx->fadeProgress >= 0x1000) {
            ctx->fadeProgress = 0x1000;
            *statePtr = 0x1A;
        }
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);
        break;

    case 0x1A:
        *statePtr = 0x1B;
        // fall through
    case 0x1B: {
        TutoSectionEntry *table = D_801E4E3C;
        u8 entryIdx;

        ctx->cursorPos = func_801F6768(inputRepeat, ctx->availCount, ctx->cursorPos);
        func_801E293C(0, ctx->sectionIndex);
        func_801E296C(1, ctx);

        entryIdx = ctx->availSlots[ctx->cursorPos];
        table = &table[entryIdx];
        ctx->panelHandle = func_801E2910(table->sectionId);

        if (inputNew & PADRdown) {
            sendSpuCommand(2);
            state = 0x1C;
            goto top;
        }
        if (inputNew & PADRup) {
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
        ctx->pageIndex.b.lo = table->overlayCmd;
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
        ctx->pageIndex.b.lo = table->overlayCmd;

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
        // fall through
    case 0x1F:
        ctx->fadeProgress -= 0x100;
        if (ctx->fadeProgress <= 0) {
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
        // fall through
    case 0x21:
        ctx->fadePos -= 0x100;
        if (ctx->fadePos <= 0) {
            ctx->fadePos = 0;
            if (pollCdReadStatus() == 0) {
                func_801F18FC(&ctx->allocNext);
                func_801F0BB0();
            }
        }
        func_801F0948(ctx->fadePos);
        func_801E293C(1, ctx->sectionIndex);
        break;
    }

    if (func_801EFFB8() == 0x1A) {
        if (func_801F0D84() == 0x14) {
            func_801F1DB0(ctx->fadePos);
        }
    }
}

/**
 * @brief Render a tutorial panel header with text and border.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @return Updated OT cursor position.
 */
static s32 func_801E3EC0(s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    u8 *textAddr;
    s32 xOff = x + 0x20;
    s32 yOff = y + 6;

    textAddr = func_801E28E4(0x10);
    cursorY = func_801F0FEC(renderCtx, cursorY, xOff, yOff, textAddr, 7);

    cfg->iconType = 0x57;
    cfg->iconSubType = 0;
    cfg->rect.x = x;
    cfg->rect.w = 0xF4;
    cfg->rect.y = y;
    cfg->rect.h = 0x16;

    return DrawMenuWindow(renderCtx, cursorY, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Render a tutorial content panel with decoded message text.
 * @param state Tutorial state.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @return Updated OT cursor position.
 */
static u32 func_801E3F8C(TutoState *state, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 xOff = x + 0xA;
    s32 yOff = y + 9;
    s32 w = 0x144;
    s32 h = 0x1A;

    if (state->panelHandle != NULL) {
        u8 textBuf[128];
        decodeMessage(state->panelHandle, textBuf, -1);
        cursorY = func_801F0FEC(renderCtx, cursorY, xOff, yOff, textBuf, 7);
    }

    cfg->iconType = 0x55;
    cfg->iconSubType = 0;
    cfg->rect.x = x;
    cfg->rect.y = y;
    cfg->rect.w = w;
    cfg->rect.h = h;

    return DrawMenuWindow(renderCtx, cursorY, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Render the tutorial section list panel.
 * @param state Unused tutorial state pointer.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X coordinate.
 * @param y Panel Y coordinate.
 * @return Updated OT cursor position.
 */
static u32 func_801E4080(void *state, s32 renderCtx, s32 cursorY, s32 x, s32 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    TutoEntry *table = D_801E4E18;
    s32 xOff = x + 0xA;
    s32 yOff = y + 5;
    s32 i;
    s32 color;
    u8 *textAddr;

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

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->rect.x = x;
    cfg->rect.y = y;
    cfg->rect.w = 0x86;
    cfg->rect.h = 0x80;

    cursorY = DrawMenuWindow(renderCtx, cursorY, ONE, g_menuTint[MENU_TINT_NORMAL]);
    return cursorY;
}

/**
 * @brief Render a single tutorial entry in a 10-per-page list.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param index Zero-based page index.
 * @param startY Slot within the page (0-9).
 * @param x Base X coordinate.
 * @return Updated OT cursor position (unchanged if entry out of range).
 */
static s32 func_801E4214(s32 renderCtx, s32 cursorY, s32 index, s32 startY, s32 x) {
    s32 panelX;
    u16 xCoord;
    s32 yPos;
    u8 *text;
    s32 endPos;
    s32 labelX;

    endPos = (index * 10) + startY;
    if (endPos >= g_gameState.mainData.tutoEntryCount) {
        return cursorY;
    }

    endPos += 1;
    text = func_801E28E4(0x17);
    labelX = x + 0xA;
    yPos = g_menuDisplayCfg.rect.y + 5;
    yPos += startY * 15;

    cursorY = func_801F0FEC(renderCtx, cursorY, g_menuDisplayCfg.rect.x + labelX, yPos, text, 7);

    panelX = g_menuDisplayCfg.rect.x;
    xCoord = panelX + (x + 0xC0);
    return drawNumberMenuTint(renderCtx, cursorY, (yPos << 16) | xCoord, endPos, 7);
}

/**
 * @brief Render the tutorial page content panel (text entries + scroll).
 * @param state Tutorial state (scrollAnim/targetPage/prevPage drive display).
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @param x Panel X position.
 * @param y Panel Y position.
 * @return Updated OT cursor position.
 */
static u32 func_801E431C(TutoState *state, s32 renderCtx, s32 cursorY, s16 x, s16 y) {
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->rect.x = x;
    cfg->rect.w = 0xCA;
    cfg->rect.h = 0x9E;
    cfg->columnCount = 0xA;
    cfg->rect.y = y;
    cfg->pageStart = state->targetPage;
    cfg->pageEnd = state->prevPage;
    cfg->scrollOffset = state->scrollAnim;

    if (g_gameState.mainData.tutoEntryCount >= 0xB) {
        cursorY = func_801F5F60(renderCtx, cursorY, g_menuTint[MENU_TINT_NORMAL], 3);
    }

    return func_801EFBB4(renderCtx, cursorY, func_801E4214);
}

/**
 * @brief Render the tutorial section-select panel during fade-in.
 * @param state Tutorial state with fadeProgress and availSlots.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @return Updated OT cursor position.
 */
static s32 func_801E43D4(TutoState *state, s32 renderCtx, s32 cursorY) {
    MenuDisplayConfig *cfg;
    TutoSectionEntry *sectionTable;
    s32 scaleVal;
    s32 index;
    s32 scaled;
    s32 xPos;
    s32 yPos;
    s32 i;
    u8 textBuf[128];
    s16 progress = state->fadeProgress;

    if (progress != 0) {
        scaleVal = 0x1000 - progress;
        index = scaleVal / 64;
        scaleVal = D_801FA3C8[index];
        scaled = (scaleVal * 190) / 4096;
        yPos = 62;
        cfg = &g_menuDisplayCfg;
        xPos = scaled + 168;
        sectionTable = D_801E4E3C;
        i = 0;

        if (state->availCount != 0) {
            u8 *textAddr = textBuf;
            u8 entryIdx;
            u8 *msgPtr;
            do {
                entryIdx = state->availSlots[i];
                msgPtr = func_801E28E4(sectionTable[entryIdx].sectionId);
                decodeMessage(msgPtr, textBuf, -1);
                cursorY = func_801F0FEC(renderCtx, cursorY, xPos, yPos, textAddr, 7);
                i++;
                yPos += 15;
            } while (i < state->availCount);
        }

        cfg->iconType = 0;
        cfg->iconSubType = 0;
        cfg->rect.x = scaled + 158;
        cfg->rect.y = 57;
        cfg->rect.w = 202;
        cfg->rect.h = 143 - (9 - state->availCount) * 15;
        cursorY = DrawMenuWindow(renderCtx, cursorY, ONE, g_menuTint[MENU_TINT_NORMAL]);
    }
    return cursorY;
}

/**
 * @brief Render the tutorial screen while the UI is in state 0x14.
 * @param state Tutorial state pointer.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @return Updated OT cursor position (unchanged if not in state 0x14).
 */
static s32 func_801E4598(TutoState *state, s32 renderCtx, s32 cursorY) {
    s32 tableVal;
    s32 index;
    s32 scaled;
    s32 yArg;
    s32 cursorTmp;
    s16 scrollPos;

    if (func_801F0D84() != 0x14) {
        return cursorY;
    }

    func_801F1AFC();
    setMenuBrightness(state->fadePos);

    cursorY = func_801E3EC0(renderCtx, cursorY, 0x18, 6);
    yArg = 0x1D;
    cursorY = func_801E3F8C(state, renderCtx, cursorY, 0x1E, yArg);
    scrollPos = state->scrollPos;

    if (scrollPos < 0x1000) {
        tableVal = scrollPos;
        index = tableVal / 64;
        tableVal = D_801FA3C8[index];
        yArg = 0x39;
        scaled = (tableVal * 190) / 4096;
        cursorY = func_801E431C(state, renderCtx, cursorY, scaled + 0x9E, yArg);
    }

    cursorTmp = func_801E43D4(state, renderCtx, cursorY);
    cursorY = func_801E4080(state, renderCtx, cursorTmp, 0x18, 0x39);
    func_801F1B10();
    return cursorY;
}

/** @brief Cold-entry to tutorial menu: register callbacks and initialize state. */
void func_801E46DC(void) {
    TutoState *ctx;
    u8 *p;

    ctx = func_801F179C(func_801E3140, func_801E4598);
    NOP();

    ctx->isReentry = 0;
    ctx->fadeProgress = 0;
    ctx->cursorPos = 0;

    setFieldFlag(0x3E);
    func_801E30C4(ctx);
    func_801F0948(0);

    if (func_801EFFB8() == 0x1A) {
        func_801F1D34(D_801E4EA8);
        func_801F1DB0(0);
    }

    if (ctx != NULL) {
        func_801E3140(ctx);
        ctx->scrollPos = 0x1000;
        ctx->fadePos = 0;
        ctx->pageCount = g_gameState.mainData.tutoEntryCount;
        ctx->pageCount = (ctx->pageCount + 9) / 10;
        ctx->fadeAlpha = 0x1B;
        ctx->pageIndex.b.hi = 3;
    }

    p = (u8 *)0x801E5800;
    do {
        *p = 0;
        p++;
    } while (p != (u8 *)0x801EF800);

    p = (u8 *)0x801CD000;
    do {
        *p = 0;
        p++;
    } while (p != (u8 *)0x801E0000);
}

/** @brief Warm-entry to tutorial menu with sibling overlay memory scrub. */
void func_801E47F8(void) {
    TutoState *ctx;
    u8 *p;

    ctx = func_801F179C(func_801E3140, func_801E4598);
    NOP();
    ctx->isReentry = 1;
    ctx->fadeProgress = 0;
    ctx->cursorPos = 0;
    func_801F0948(0);
    func_801F1D34(D_801E4EA8);
    func_801F1DB0(0);

    if (ctx != NULL) {
        func_801E3140(ctx);
        ctx->fadeAlpha = 0x1B;
        ctx->fadePos = 0;
        ctx->pageIndex.b.hi = 3;
    }

    p = (u8 *)0x801E5800;
    do {
        *p++ = 0;
    } while (p != (u8 *)0x801EF800);

    p = (u8 *)0x801CD000;
    do {
        *p++ = 0;
    } while (p != (u8 *)0x801E0000);
}

/**
 * @brief Tutorial page navigation state machine.
 * @param self Pointer to tutorial callback context.
 */
static void func_801E48C0(TutoState *self) {
    u16 buttons = g_menuDisplayCfg.inputNew;
    u16 *state = &self->state;

    switch (*state) {
    case 0:
        *state = 1;
        break;

    case 1:
        if (pollCdReadStatus() != 0) {
            break;
        }
        loadTimImage((u8 *)0x801CD000);
        *state = 2;
        break;

    case 2:
        *state = 3;
        break;

    case 3: {
        s16 val;

        val = self->fadeAlpha + 0x100;
        self->fadeAlpha = val;
        if (val >= 0x1000) {
            self->fadeAlpha = 0x1000;
        }

        if (buttons & (PADL1 | PADLleft)) {
            sendSpuCommand(2);
            val = self->pageIndex.hword - 1;
            self->pageIndex.hword = val;
            if (val < 0) {
                s16 prev;
                do {
                    prev = self->pageIndex.hword;
                    val = prev + 1;
                    self->pageIndex.hword = val;
                } while (D_801E4EAC[val] != 0xFFFF);
                self->pageIndex.hword = prev;
            }
            *state = 4;
        }

        if (buttons & (PADR1 | PADLright)) {
            sendSpuCommand(2);
            val = self->pageIndex.hword + 1;
            self->pageIndex.hword = val;
            if (D_801E4EAC[val] == 0xFFFF) {
                self->pageIndex.hword = 0;
            }
            *state = 4;
        }

        if (buttons & PADRdown) {
            sendSpuCommand(2);
            val = self->pageIndex.hword + 1;
            self->pageIndex.hword = val;
            if (D_801E4EAC[val] == 0xFFFF) {
                *state = 6;
                break;
            }
            *state = 4;
        }

        if (buttons & PADRup) {
            sendSpuCommand(3);
            *state = 6;
        }
        break;
    }

    case 4:
        loadSubOverlay(D_801E4EAC[(s16)self->pageIndex.hword], 0x801CD000);
        *state = 5;
        break;

    case 5: {
        s16 fade = self->fadeAlpha - 0x100;
        self->fadeAlpha = fade;
        if (fade > 0) {
            break;
        }
        self->fadeAlpha = 0;
        *state = 1;
        break;
    }

    case 6:
        *state = 7;
        break;

    case 7: {
        s16 fade = self->fadeAlpha - 0x100;
        self->fadeAlpha = fade;
        if (fade > 0) {
            break;
        }
        self->fadeAlpha = 0;
        *state = 8;
        break;
    }

    case 8:
        SetDispMask(0);
        *state = 9;
        break;

    case 9:
        func_801F010C(0x180);
        *state = 10;
        break;

    case 10:
        *state = 11;
        break;

    case 11:
        SetDispMask(1);
        loadOverlayWithTimCallback(9, 0x801CD000);
        loadOverlayWithTimCallback(10, 0x801D5000);
        *state = 12;
        break;

    case 12:
        if (pollCdReadStatus() != 0) {
            break;
        }
        *state = 13;
        break;

    case 13:
        func_801F18FC(&self->allocNext);
        func_801F0BB0();
        break;
    }
}

/**
 * @brief Build five textured-sprite GPU primitives for the tutorial fade banner.
 * @param ot Ordering table pointer.
 * @param prim TSPRT packet allocation pointer.
 * @param fade Fade alpha value (signed); divided by 32 for the per-sprite tint.
 * @return Updated packet pointer (@c prim + 5*sizeof(TSPRT)).
 */
static TSPRT *func_801E4BD0(u32 *ot, TSPRT *prim, s32 fade) {
    s32 color = fade / 32;
    s32 vramX = 0x340;
    s32 x = 0;
    s32 y = 0;
    s32 i;

    for (i = 0; i < 320; i += 64, x += 64, vramX += 32) {
        setTSprtFull(prim, 1, 0, getTPage(1, 0, vramX, y));
        setRGB0(prim, color, color, color);
        setXY0(prim, x, y);
        setWH(prim, 64, 216);
        setUV0(prim, (vramX % 64) * 2, 0);
        prim->clut = getClut(512, 218);
        addPrimFast(ot, prim);
        prim++;
    }
    return prim;
}

/**
 * @brief Tutorial render callback: forward to fade renderer if fade is active.
 * @param state Tutorial state.
 * @param renderCtx Render context handle.
 * @param cursorY Current OT cursor position.
 * @return Updated OT cursor position (unchanged if fadeAlpha is 0).
 */
static s32 func_801E4CB0(TutoState *state, s32 renderCtx, s32 cursorY) {
    s16 alpha = state->fadeAlpha;

    if (alpha != 0) {
        cursorY = (s32)func_801E4BD0((u32 *)renderCtx, (TSPRT *)cursorY, alpha);
    }
    return cursorY;
}

/** @brief Initialize tutorial menu: register callbacks, clear state, and enter. */
void func_801E4CE4(void) {
    TutoState *ctx = func_801F179C(func_801E48C0, func_801E4CB0);

    if (ctx != NULL) {
        ctx->fadeAlpha = 0;
        ctx->pageIndex.hword = 0;
        func_801F010C(0x140);
        func_801E48C0(ctx);
    }
}
