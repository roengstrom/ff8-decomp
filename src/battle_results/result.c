#include "common.h"
#include "battle_results/result.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "battle_results/display.h"
#include "battle_results/draw.h"
#include "battle_results/update.h"
#include "ability.h"
#include "battle.h"
#include "card.h"
#include "character.h"
#include "ui/dialog.h"
#include "ui/text.h"
#include "game.h"
#include "gamestate.h"
#include "party_stats.h"
#include "gf_curve.h"
#include "thread.h"
#include "kernel.h"

extern u8 D_80083928;

/** displayStatus bit: HP below a quarter of max (func_800A240C sets it with 0x200). */
#define STATUS_HP_CRITICAL 0x100

/* --- Private functions --- */

static void runResultsFrame(void);
static void stepResultsThread(void);
static void resultsThreadMain(void);
static void clearFramebuffers(void);

/**
 * @brief Run one frame of the results screen: make the display visible, draw
 * the frame (drawResultsFrame) and advance the screen (updateResults).
 */
static void runResultsFrame(void) {
    SetDispMask(1);
    drawResultsFrame();
    updateResults();
}


/**
 * @brief Return nonzero once the results thread has finished and been closed
 * (stepResultsThread sets @c D_80083928).
 */
s32 isResultsThreadDone(void) {
    return D_80083928;
}


/**
 * @brief Run one step of the results thread; it runs once per frame.
 *
 * Steps through @c state: blank the display, clear the results display twice
 * (the second time also running updateResults once), run frames
 * (runResultsFrame) until @c D_80083929 reads 1, wait two frames on @c timer,
 * close @c thread if there is one (setting @c D_80083928), then keep the
 * display blanked.
 */
static void stepResultsThread(void) {
    ResultsScreen *ctx = (ResultsScreen *)getScratchAddr(0);
    u16 *state = &ctx->state;
    s32 thread;

    switch (*state) {
    case 0:
        SetDispMask(0);
        *state = 1;
        break;
    case 1:
        clearResultsDisplay();
        *state = 2;
        break;
    case 2:
        clearResultsDisplay();
        updateResults();
        *state = 3;
        break;
    case 3:
        runResultsFrame();
        if (D_80083929 == 1) {
            *state = 4;
        }
        break;
    case 4:
        ctx->timer = 2;
        *state = 5;
        break;
    case 5:
        if (--ctx->timer <= 0) {
            *state = 6;
        }
        break;
    case 6:
        thread = ctx->thread;
        if (thread != 0) {
            ctx->thread = 0;
            closeThreadSafe(thread);
            D_80083928 = 1;
            SetDispMask(0);
        }
        *state = 7;
        break;
    case 7:
        SetDispMask(0);
        break;
    }
}


/** @brief Switch to the results thread, ResultsScreen::thread. */
void switchToResultsThread(void) {
    ResultsScreen *ctx = (ResultsScreen *)getScratchAddr(0);

    switchThread(ctx->thread);
}


/**
 * @brief The results thread's body: run one step (stepResultsThread), switch
 * back to the main thread, repeat. Never returns.
 */
static void resultsThreadMain(void) {
    for (;;) {
        stepResultsThread();
        switchThread(0);
    }
}


/**
 * @brief Clear both VRAM framebuffers to black.
 *
 * Clears two 384x224 (0x180 x 0xE0) regions in VRAM: the first at (0,0)
 * and the second at (0x200,0). Each clear is followed by DrawSync(0) to
 * wait for completion.
 */
static void clearFramebuffers(void) {
    RECT rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x180;
    rect.h = 0xE0;
    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);
    rect.x = 0x200;
    rect.y = 0;
    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);
}


/**
 * @brief Start the battle results screen.
 *
 * Sets up the results display and the scratchpad state, merges the item and
 * card drops into @c rewards, fills the three EXP rows and starts the results
 * thread (resultsThreadMain). Then, for each GF it works out whether it levels up,
 * gives the battle's AP to the ability it is learning and, once that ability
 * is learned, picks the next one to learn.
 */
void startBattleResults(void) {
    ResultsScreen *ctx = (ResultsScreen *)getScratchAddr(0);
    AbilityListEntry list[22];
    u8 *stack;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 count;
    s32 id;
    s32 level;
    u32 prev;
    u32 next;
    s32 exp;
    s32 learning;
    s32 n;
    s32 found;
    s32 learned;
    s32 ability;
    s32 nextAbility;
    s32 status;
    u8 *card;
    JunctionableGfEntry *learnData;
    s32 *bits;
    splitStruct *drop;

    DrawSync(0);
    VSync(0);
    SetDispMask(0);
    g_resultsDisplays[0] = (ResultsDisplay *)getResultsDisplayBase();
    g_resultsDisplays[1] = (ResultsDisplay *)(getResultsDisplayBase() + getResultsDisplaySize());
    clearFramebuffers();
    initResultsDisplays();
    g_resultsDisplay = g_resultsDisplays[0];
    flipResultsDisplay();
    ctx->state = 0;
    D_80083928 = 0;
    ctx->step = 0;
    D_80083929 = 0;
    ctx->wipeProgress = 0;
    ctx->descProgress = 0;
    ctx->pageProgress = 0;
    for (i = 0; i < 32; i++) {
        ctx->rewards[i].id = 0;
        ctx->rewards[i].count = 0;
    }
    drop = g_battleChars.unk5E0;
    count = 0;
    for (i = 0; i < 24; i++, drop++) {
        if (drop->unk0 == 0) {
            break;
        }
        if (drop->unk1 == 0) {
            continue;
        }
        for (j = 0; j < 32; j++) {
            if (ctx->rewards[j].id == 0) {
                ctx->rewards[j].id = drop->unk0;
                ctx->rewards[j].count = drop->unk1;
                count++;
                break;
            }
            if (ctx->rewards[j].id == drop->unk0) {
                ctx->rewards[j].count += drop->unk1;
                break;
            }
        }
    }
    card = g_battleChars.unk610;
    for (i = 0; i < 8; i++) {
        id = *card++;
        if (id == 0xFF) {
            break;
        }
        id |= REWARD_CARD;
        for (m = 0; m < 32; m++) {
            if (ctx->rewards[m].id == 0) {
                count++;
                ctx->rewards[m].id = id;
                ctx->rewards[m].count = 1;
                break;
            }
            if (ctx->rewards[m].id == id) {
                ctx->rewards[m].count++;
                break;
            }
        }
    }
    ctx->reward = ctx->rewards;
    ctx->unk40 = 1;
    ctx->rewardsLeft = count;
    ctx->rewardCount = count;
    ctx->unk44 = 0;
    ctx->message = NULL;
    ctx->unk4C = 0;
    ctx->closeDelay = 0x20;
    ctx->messageProgress = 0;
    ctx->titleProgress = 0;
    ctx->page = 0;
    for (i = 0; i < 3; i++) {
        ctx->levelUpTimer[i] = 0;
    }
    stack = getThreadStackTop();
    for (i = 0; i < 3; i++) {
        ctx->noExpPopup[i] = 0;
        if (g_battleChars.chars[i].characterId != 0xFF) {
            level = g_battleChars.chars[i].level;
            status = g_battleChars.chars[i].displayStatus;
            ctx->nameColor[i] = 7;
            if (status & STATUS_HP_CRITICAL) {
                ctx->nameColor[i] = 2;
            }
            if (status & STATUS_KO) {
                ctx->nameColor[i] = 1;
            }
            if (level == 100) {
                ctx->expAcquired[i] = 0;
                ctx->nextLevelExp[i] = 0;
                ctx->expStep[i] = 0;
                ctx->currentExp[i] = evalEntityXpCurve(i, level);
            } else {
                prev = evalEntityXpCurve(i, level - 1);
                next = evalEntityXpCurve(i, level);
                ctx->expAcquired[i] = g_battleChars.unk574[i] + g_battleChars.unk57A[i];
                ctx->nextLevelExp[i] = g_battleChars.chars[i].xpToNext;
                if (ctx->expAcquired[i] == 0) {
                    ctx->noExpPopup[i] = 0x41;
                    ctx->noExpRows |= 1 << i;
                }
                next = (next - prev) >> 7;
                if (next == 0) {
                    next = 1;
                }
                ctx->expStep[i] = next;
            }
            ctx->names[i] = getBattleCharName(i);
            ctx->currentExp[i] = g_battleChars.chars[i].exp;
            ctx->level[i] = g_battleChars.chars[i].level;
        } else {
            ctx->names[i] = NULL;
            ctx->expStep[i] = 0;
            ctx->noExpRows |= 1 << i;
        }
    }
    ctx->thread = openThreadSafe(resultsThreadMain, stack);
    ctx->timer = 6;
    ctx->title = NULL;
    ctx->prompt = NULL;
    ctx->unk26E = 0;
    setTextBrightness(0x1000);
    ctx->levelUpWidth = getFirstLineWidth(getMenuString(0x30)) + 0x14;
    ctx->gfLevelUp = 0;
    ctx->gfWindows = 0;
    ctx->ap = 0;
    for (i = 0; i < GF_COUNT; i++) {
        ctx->gfLearned[i] = 0xFF;
        if (g_gameState.gfs[i].exists & GF_EXISTS) {
            exp = g_battleChars.unk580[i] + g_battleChars.unk5A0[i];
            if (func_8002274C(i, 0) != func_8002274C(i, exp)) {
                ctx->gfLevelUp |= 1 << i;
                ctx->gfWindows |= 1 << i;
            }
            learning = g_gameState.gfs[i].learning;
            n = func_800369CC(i, list, 1);
            found = 0;
            if (g_battleChars.unk5C0[i] != 0) {
                for (m = 0; m < n; m++) {
                    if (learning == list[m].slotIndex && list[m].type == 1) {
                        ctx->ap = g_battleChars.unk5C0[i];
                        found = 1;
                    }
                }
            }
            /* Stand-in: the binary walks the list again but only the pointer step
               survives, so this loop's body was removed after loop optimisation.
               A test that no u8 passes is removed at the same point. */
            for (j = 0; j < n; j++) {
                if (list[j].type >> 8) {
                    found = 1;
                }
            }
            if (found) {
                if (AddAbilityExp(i, g_battleChars.unk5C0[i])) {
                    learned = 0;
                    for (j = 0; j < n; j++) {
                        if (learning == list[j].slotIndex && list[j].type == 1) {
                            bits = g_gameState.gfs[i].completeAbilities;
                            bits[learning / 32] |= 1 << (learning & 31);
                            ctx->gfLearned[i] = learning;
                            ctx->gfWindows |= 1 << i;
                            learned = 1;
                        }
                    }
                    if (learned) {
                        nextAbility = 0;
                        n = func_800369CC(i, list, 1);
                        learnData = &g_kernel.junctionableGfs[i];
                        for (k = 0; k < 21; k++) {
                            ability = learnData->abilities[k].abilityId;
                            for (j = 0; j < n; j++) {
                                if (list[j].slotIndex == ability && list[j].type == 1) {
                                    nextAbility = ability;
                                    break;
                                }
                            }
                            if (nextAbility != 0) {
                                break;
                            }
                        }
                        g_gameState.gfs[i].learning = nextAbility;
                    }
                }
            }
        }
    }
    ctx->noExpWidth = getFirstLineWidth(getMenuString(0x31));
    ctx->gfWindowCount = 0;
    for (i = 0; i < GF_COUNT; i++) {
        if ((ctx->gfWindows >> i) & 1) {
            ctx->gfWindowCount++;
        }
    }
    recalcPartyStats();
}
