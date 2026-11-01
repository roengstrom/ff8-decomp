#include "common.h"
#include "283B0.h"
#include "cd.h"
#include "intro.h"
#include "field/fe_object1.h"
#include "field/fe_object6.h"
#include "field/fe_object12.h"
#include "field.h"
#include "gamestate.h"
#include "battle.h"
#include "input/gamepad.h"
#include "btl_anim.h"
#include "overlay.h"
#include "game.h"
#include "cdrom.h"
#include "snd_init.h"
#include "field_init.h"
#include "ui/font.h"
#include "ui/window.h"
#include "lzs.h"
#include "psxsdk/libapi.h"
#include "psxsdk/libspu.h"
#include "psxsdk/crt0.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libetc.h"
#include "main.h"
#include "btl_transition.h"
#include "wm2field.h"
#include "memlayout.h"
#include "imgtoc.h"

/** @brief Global field ids of the maps a new game and a game over go to. */
#define FIELD_ID_START0 74 /**< start0, the first map of a new game. */
#define FIELD_ID_GOVER 75 /**< gover, the game-over map. */

// The place in the disc image where the table of contents file resides.
// These were likely calculated during the mastering process of the disc.
#define IMG_TOC_SECTOR 826
#define IMG_TOC_SIZE 1064

#define NAME_DICTIONARY_PTR ((void *)NAME_DICTIONARY_ADDR)
#define IMG_TOC_PTR ((void *)IMG_TOC_ADDR)
#define OVERLAY_PTR ((void *)OVERLAY_ADDR)
#define SCRATCH_PTR ((void *)SCRATCH_ADDR)

static void DisableStpBit(void);
static void SaveFieldSnapshot(void);
static void RestoreFieldSnapshot(void);
static inline void LoadIntroOverlay(void);
static inline void ReturnToTitle(void);
static inline void GoToGameOverField(void);
static inline void ReloadDiscTables(void);
static inline void RunBattle(s32 mapAddr);

static CdFileDesc g_fileTableDesc[] = { { IMG_TOC_SECTOR, IMG_TOC_SIZE } };

static void DisableStpBit(void) {
    DR_STP prim;
    u32 ot[2];

    ClearOTag(ot, 2);
    SetDrawStp(&prim, 0); // semi-transparency bit
    addPrim(ot, &prim);
    DrawOTag(ot);

    while (DrawSync(1));
}

void VsyncHandler(void) {
    switch (g_renderMode) {
    case RENDER_IDLE:
        break;
    case RENDER_FADE:
        ProcessFade();
        break;
    case RENDER_BATTLE_TRANSITION:
        paceBattleTransition();
        break;
    case RENDER_BATTLE:
        func_800D0608();
        break;
    case RENDER_BATTLE_RESULTS:
        vsyncGameHandler();
        break;
    }

    if (IsPlayTimePaused()) {
        g_vsyncDone = 1;
        return;
    }
    if (g_vsyncSkip) {
        g_vsyncDone = 1;
        return;
    }

    // This function is called 59.8 times per second (on NTSC).
    // Make use of that fact to tick the in-game clock.
    g_secondTicker += 0x88F;
    if (g_secondTicker >> 17) {
        g_gameState.mainData.playTimeSeconds++;
        g_secondTicker &= 0xFFFF;
    }

    g_countdownTicker += 0x88F;
    if (g_countdownTicker >> 17) {
        if (g_gameState.mainData.countdownTimer == 0) {
            g_vsyncDone = 1;
            return;
        }
        g_gameState.mainData.countdownTimer--;
        g_countdownTicker &= 0xFFFF;
    }

    g_vsyncDone = 1;
}

void InitHardware(void) {
    SetMem(2);
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    SpuInit();
    g_vsyncSkip = 0;
    g_renderMode = RENDER_IDLE;
    VSyncCallback(VsyncHandler);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

static void SaveFieldSnapshot(void) {
    s16 i;

    if (g_previousGameMode == GAME_MODE_WORLD) {
        g_gameState.fieldStateSnapshot.gameMode = GAME_MODE_WORLD;
    } else {
        g_gameState.fieldStateSnapshot.gameMode = GAME_MODE_FIELD;
    }

    g_gameState.fieldStateSnapshot.fieldId = g_curFieldId;
    g_gameState.fieldStateSnapshot.previousFieldId = g_fieldContext.previousFieldId;

    for (i = 0; i < 3; i++) {
        g_gameState.fieldStateSnapshot.positionsX[i] = g_fieldActors[g_fieldContext.entityIndex[i]].posX >> 12;
        g_gameState.fieldStateSnapshot.positionsY[i] = g_fieldActors[g_fieldContext.entityIndex[i]].posY >> 12;
        g_gameState.fieldStateSnapshot.triIdxs[i] = g_fieldActors[g_fieldContext.entityIndex[i]].triIdx;
        g_gameState.fieldStateSnapshot.facingDirection[i] = g_fieldActors[g_fieldContext.entityIndex[i]].facingDirection;
    }

    g_gameState.fieldStateSnapshot.fieldRngIndex = g_fieldRngIndex;
    g_gameState.fieldStateSnapshot.fieldRngOffset = g_fieldRngOffset;
}

static void RestoreFieldSnapshot(void) {
    g_gameMode = g_gameState.fieldStateSnapshot.gameMode;
    if (g_gameMode == GAME_MODE_NONE) {
        g_gameMode = GAME_MODE_FIELD;
    }

    g_curFieldId = g_gameState.fieldStateSnapshot.fieldId;

    g_fieldContext.previousFieldId = g_gameState.fieldStateSnapshot.previousFieldId;
    g_fieldContext.position_x = g_gameState.fieldStateSnapshot.positionsX[0];
    g_fieldContext.position_y = g_gameState.fieldStateSnapshot.positionsY[0];
    g_fieldContext.spawnTriIdx = g_gameState.fieldStateSnapshot.triIdxs[0];
    g_fieldContext.facingDirection = g_gameState.fieldStateSnapshot.facingDirection[0];

    g_fieldRngIndex = g_gameState.fieldStateSnapshot.fieldRngIndex;
    g_fieldRngOffset = g_gameState.fieldStateSnapshot.fieldRngOffset;
}

void LoadFieldOverlay(void) {
    if (g_previousGameMode != GAME_MODE_MENU && g_previousGameMode != GAME_MODE_SCRIPT_MENU) {
        // field.ovl
        LoadCompressedDataFromDisc(g_fieldFileDescs[0].sector, g_fieldFileDescs[0].size, OVERLAY_PTR, NULL);
        while (UpdateCdRead() != CD_READ_IDLE);
    }
    func_8001F5C8();
}

void LoadWorldOverlay(void) {
    if (g_previousGameMode != GAME_MODE_MENU && g_previousGameMode != GAME_MODE_SCRIPT_MENU) {
        // world.ovl
        LoadCompressedDataFromDisc(g_worldFileDescs[0].sector, g_worldFileDescs[0].size, OVERLAY_PTR, NULL);
        while (UpdateCdRead() != CD_READ_IDLE);
    }
    func_8001F5C8();
}

void LoadWm2FieldTable(void) {
    // wm2field.tbl
    LoadDataFromDisc(g_fieldFileDescs[1].sector, g_fieldFileDescs[1].size, WORLD_TO_FIELD_ENTRANCES, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

void InitSoundEngine(void) {
    sndInit();

    // sfx_000.akao
    LoadDataFromDisc(g_soundFileDescs[0].sector, g_soundFileDescs[0].size, g_soundEffectData, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);

    sndLoadBank(g_soundEffectData);

    // sfx_001.akao
    LoadDataFromDisc(g_soundFileDescs[1].sector, g_soundFileDescs[1].size, SCRATCH_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);

    sndUploadSamples(SCRATCH_PTR, 1);

    // sfx_002.akao
    LoadDataFromDisc(g_soundFileDescs[2].sector, g_soundFileDescs[2].size, SCRATCH_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);

    sndUploadSamples(SCRATCH_PTR, 1);
}

void LoadBootOverlay(void) {
    // field_init.ovl
    LoadDataFromDisc(g_bootFileDescs[0].sector, g_bootFileDescs[0].size, OVERLAY_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

void LoadSystemFont(void) {
    // sysfnt.tdw
    LoadDataFromDisc(g_systemFileDescs[0].sector, g_systemFileDescs[0].size, SCRATCH_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
    func_8002C3AC(SCRATCH_PTR, 0);
    while (DrawSync(1));
}

void InitGamePadsAndMemoryCards(void) {
    LoadBootOverlay();
    func_80098390();
    initBattleDisplay();
}

void InitSystemData(void) {
    LoadBootOverlay();
    InitUserInterface(g_packetBuffer, sizeof(g_packetBuffer));
    LoadSystemFont();

    // icon.tim
    LoadDataFromDisc(g_systemFileDescs[1].sector, g_systemFileDescs[1].size, SCRATCH_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);

    loadBattleTimImage(SCRATCH_ADDR);

    // sfx_003.akao - chocobo world song played in the save menu
    LoadDataFromDisc(g_systemFileDescs[3].sector, g_systemFileDescs[3].size, NAME_DICTIONARY_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
    FastMemCpy(g_chocoboWorldMusic, NAME_DICTIONARY_PTR, sizeof(g_chocoboWorldMusic));

    // namedic.bin
    LoadDataFromDisc(g_systemFileDescs[2].sector, g_systemFileDescs[2].size, NAME_DICTIONARY_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);

    SetNameDictionary(NAME_DICTIONARY_ADDR);
}

static inline void LoadIntroOverlay(void) {
    // intro.ovl
    LoadCompressedDataFromDisc(g_bootFileDescs[1].sector, g_bootFileDescs[1].size, OVERLAY_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

/** @brief Loads the master file descriptor table from CD. Must run before any
 *         other CD load, since every other load descriptor lives in this table.
 */
void LoadDiscTableOfContents(void) {
    // img_toc.bin
    LoadDataFromDisc(g_fileTableDesc[0].sector, g_fileTableDesc[0].size, IMG_TOC_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

static inline void ReturnToTitle(void) {
    g_fieldContext.counter = 0;
    g_fieldContext.mode = FIELD_MODE_TITLE;
    g_fieldContext.spawnTriIdx = 0x7FFF;
}

static inline void GoToGameOverField(void) {
    g_curFieldId = FIELD_ID_GOVER;
    g_previousGameMode = GAME_MODE_NONE;
    g_gameMode = GAME_MODE_FIELD;
}

static inline void ReloadDiscTables(void) {
    LoadDiscTableOfContents();
    LoadWm2FieldTable();
}

static inline void RunBattle(s32 mapAddr) {
    s16 i;

    func_80038030(mapAddr);
    gameStateLoop();
    if (g_renderMode != RENDER_BATTLE_RESULTS) {
        DisableStpBit();
    }
    if (g_battleConfig.result == 5) {
        // matching hack
        do {
        } while (0);
        ReturnToTitle();
        return;
    }
    if ((g_battleConfig.result == 1) && (!(g_fieldVars->fieldB6 & 0x200))) {
        GoToGameOverField();
        return;
    }
    if (g_battleConfig.result == 3) {
        if (g_fieldVars->fieldB6 & 0x100) {
            g_fieldVars->stateFlags &= ~FIELD_STATE_COUNTDOWN;
        } else {
            GoToGameOverField();
            return;
        }
    }
    if (g_previousGameMode == GAME_MODE_FIELD) {
        g_previousGameMode = GAME_MODE_BATTLE;
        g_gameMode = GAME_MODE_FIELD;
    } else {
        g_previousGameMode = GAME_MODE_BATTLE;
        g_gameMode = GAME_MODE_WORLD;
    }
    for (i = 0; i < 3; i++) {
        if (g_battleConfig.unk4[i] != 0xFF) {
            // matching hack
            do {
            } while (g_renderMode == RENDER_BATTLE_RESULTS);
            while (DrawSync(1));
            func_8003646C(g_battleConfig.unk4[i] + 5);
        }
    }
}

void ff8main(void) __asm__("main");
void ff8main(void) {
    WorldToFieldEntrance *entry;
    s16 mode;
    s16 fieldId;
    s32 result;
    s32 mapAddr;
    preInitStub();
    InitHardware();
    initCdSubsystem(1);
    LoadDiscTableOfContents();
    InitSoundEngine();
    LoadBootOverlay();
    func_80098000();
    InitGamePadsAndMemoryCards();
    LoadWm2FieldTable();
    while (1) {
        LoadIntroOverlay();

        func_80098FD4(0);
        InitSystemData();
        g_gameMode = GAME_MODE_FIELD;
        g_previousGameMode = GAME_MODE_NONE;
        g_fieldContext.userControlOff = 0;
        g_prefetchState = PREFETCH_NONE;
        g_prefetchedDestId = 73; // the debug "menu"
        g_fieldContext.mode = FIELD_MODE_NONE;
        g_fieldContext.mapJumpOff = 0;
        result = func_8003646C(25);
        if (!(result & 2)) {
            RestoreFieldSnapshot();
            LoadFieldOverlay();
            SmInitEventAll(0);
        } else {
            g_curFieldId = FIELD_ID_START0;
            g_gameState.battleParty[0] = 0;
            g_gameState.battleParty[1] = 0xFF;
            g_gameState.battleParty[2] = 0xFF;
            g_gameState.battleParty[3] = 0xFF;
            LoadFieldOverlay();
            SmInitEventAll(1);
        }
        if (g_fieldVars->expectedDiscId != getDiscId()) {
            LoadIntroOverlay();

            func_80098FD4(1);
            ReloadDiscTables();
        }
        do {
            // matching hack
            do {
                switch (g_gameMode) {
                case GAME_MODE_FIELD:
                    LoadFieldOverlay();
                    func_8009895C();
                    break;
                case GAME_MODE_WORLD:
                    switch (g_previousGameMode) {
                    case GAME_MODE_FIELD:
                        func_800B4F40();
                        D_80082C8C.mode = WORLD_ENTER_FROM_FIELD;
                        break;
                    case GAME_MODE_BATTLE:
                        D_80082C8C.mode = WORLD_ENTER_FROM_BATTLE;
                        break;
                    case GAME_MODE_MENU:
                        D_80082C8C.mode = WORLD_ENTER_FROM_MENU;
                        break;
                    case GAME_MODE_NONE:
                        D_80082C8C.mode = 6;
                        break;
                    }
                    if (((g_previousGameMode == GAME_MODE_FIELD) && (g_prefetchState != PREFETCH_NONE)) && (g_prefetchedDestId < 0x4A)) {
                        while (UpdateCdRead() != CD_READ_IDLE);
                        func_80038490(g_prefetchBuffer, OVERLAY_PTR);
                    } else {
                        LoadWorldOverlay();
                    }
                    g_prefetchState = PREFETCH_NONE;
                    if (func_800987D8() == 1) {
                        ReturnToTitle();
                        sndStopAll();
                        break;
                    }
                    g_previousGameMode = GAME_MODE_WORLD;
                    switch (D_80082C8C.mode) {
                    case WORLD_EXIT_TO_FIELD:
                        entry = &WORLD_TO_FIELD_ENTRANCES[D_80082C8C.param.entrance.entranceIndex];
                        fieldId = entry->fieldId;
                        g_fieldContext.position_x = entry->position_x;
                        g_fieldContext.position_y = entry->position_y;
                        g_fieldContext.spawnTriIdx = entry->spawnTriIdx;
                        g_curFieldId = fieldId;
                        g_fieldContext.facingDirection = entry->facingDirection;
                        g_gameMode = GAME_MODE_FIELD;
                        sndCmd21(-1, 0);
                        break;
                    case WORLD_EXIT_TO_BATTLE:
                        g_gameMode = GAME_MODE_BATTLE;
                        break;
                    case WORLD_EXIT_TO_MENU:
                        g_gameMode = GAME_MODE_MENU;
                        break;
                    }
                    break;
                case GAME_MODE_CARD:
                    gameStateLoop();
                    g_previousGameMode = GAME_MODE_BATTLE;
                    g_gameMode = GAME_MODE_FIELD;
                    break;
                case GAME_MODE_BATTLE:
                case GAME_MODE_BATTLE_DEBUG_MENU:
                    if (g_previousGameMode == GAME_MODE_FIELD) {
                        g_battleConfig.battleSceneId = g_fieldContext.counter;
                        mapAddr = D_8005F13C;
                    } else {
                        g_battleConfig.battleSceneId = D_80082C8C.param.battle.sceneLow | (D_80082C8C.param.battle.sceneHigh << 8);
                        mapAddr = 0x801D0000;
                    }
                    RunBattle(mapAddr);
                    break;
                case GAME_MODE_MENU:
                case GAME_MODE_SCRIPT_MENU:
                    while (g_renderMode != RENDER_IDLE) {}
                    SetDispMask(0);
                    SaveFieldSnapshot();
                    if (g_previousGameMode == GAME_MODE_FIELD) {
                        result = func_8003646C(g_fieldContext.counter, g_fieldContext.unk1AB);
                        if (result & 1) {
                            clearEntityFlags();
                            if (g_gameMode == GAME_MODE_MENU) {
                                g_previousGameMode = GAME_MODE_SCRIPT_MENU;
                            } else {
                                g_previousGameMode = GAME_MODE_MENU;
                            }
                        } else {
                            g_previousGameMode = GAME_MODE_MENU;
                        }
                        g_gameMode = GAME_MODE_FIELD;
                    } else {
                        result = func_8003646C(0x80000000, g_fieldVars->fieldD1 | 1);
                        g_previousGameMode = GAME_MODE_MENU;
                        g_gameMode = GAME_MODE_WORLD;
                    }
                    if (!(result & 4)) {
                        break;
                    }
                    g_previousGameMode = g_gameMode;
                    g_gameMode = GAME_MODE_BATTLE;
                    g_battleConfig.battleSceneId = (u16)(result >> 16);
                    if (g_gameMode == GAME_MODE_FIELD) {
                        mapAddr = D_8005F13C;
                    } else {
                        // matching hack
                        result = 0x801D0000;
                        mapAddr = result;
                    }
                    RunBattle(mapAddr);
                    break;
                case GAME_MODE_DISC_CHANGE:
                    LoadIntroOverlay();
                    func_80098FD4(g_fieldContext.unk1A0);
                    mode = g_fieldContext.unk1A0;
                    if (mode == 1) {
                        ReloadDiscTables();
                        g_previousGameMode = GAME_MODE_NONE;
                        g_gameMode = mode;
                        g_curFieldId = g_fieldContext.counter;
                    }
                    break;
                }
            } while (0);
        } while (g_fieldContext.mode != FIELD_MODE_TITLE);
        sndStopAll();
        SetPlayTimePaused(0);
        WaitForGamePadDetection();
        ResetGraph(3);
    }
}

/** @brief Sets the GPU draw mode (texture page) for both display buffers.
 *
 *  @param a0 Texture-page semi-transparency mode.
 */
void SetupDrawMode(s32 a0) {
    u16 tpage;

    tpage = GetTPage(0, a0, 0, 0);
    SetDrawMode(D_80070468, 0, 0, tpage, 0);
    SetDrawMode(D_80070468 + 0x20, 0, 0, tpage, 0);
}

/** @brief Initializes the two full-screen black TILE primitives used to clear
 *         the screen each frame (one per double-buffered slot).
 */
void InitClearTiles(void) {
    s32 i = 0;
    TILE *tile = &g_clearTiles[0];
top:
    SetTile(tile);
    SetSemiTrans(tile, 1);
    SetShadeTex(tile, 1);
    tile->w = 320;
    tile->r0 = 0;
    tile->g0 = 0;
    tile->b0 = 0;
    tile->h = 224;
    tile->x0 = 0;
    tile->y0 = 0;
    tile += 2;
    if (++i < 2) {
        goto top;
    }
}

/** @brief Copies the current display buffer to the other so both framebuffers
 *         hold the same content.
 *
 *  @note The raw pointer arithmetic is required for codegen matching.
 */
void copyFramebuffer(void) {
    s32 base;
    s32 ofs1;

    ofs1 = (s16)g_bufferIndex * 20;
    base = g_dispEnvs;

    MoveImage((void *)(ofs1 + base), *(s16 *)(((s16)g_bufferIndex + 1 & 1) * 20 + base),
              *(s16 *)(((s16)g_bufferIndex + 1 & 1) * 20 + base + 2));

    while (DrawSync(1) != 0) {}
}

/** @brief Builds the per-frame GPU primitive list (clear tile + draw mode) for
 *         the current buffer.
 *
 *  @param brightness Fill brightness (uniform RGB); 16 (dark grey) during fades.
 */
void BuildPrimList(s32 brightness) {
    ClearOTag(&g_orderingTablePtrs[(s16)g_bufferIndex], 1);
    g_clearTiles[(s16)g_bufferIndex * 2].r0 = brightness;
    g_clearTiles[(s16)g_bufferIndex * 2].g0 = brightness;
    g_clearTiles[(s16)g_bufferIndex * 2].b0 = brightness;
    addPrim(&g_orderingTablePtrs[(s16)g_bufferIndex], &g_clearTiles[(s16)g_bufferIndex * 2]);
    addPrim(&g_orderingTablePtrs[(s16)g_bufferIndex], (DR_MODE *)&g_clearTiles[(s16)g_bufferIndex * 2] - 1);
}

/** @brief Per-frame render: swaps the double buffer, advances the active fade
 *         effect, and submits the frame's primitive list.
 *
 *  Signals completion to the main loop when the fade finishes.
 */
void ProcessFade(void) {
    if (g_fadeMode == FADE_NONE) {
        return;
    }

    g_bufferIndex = g_bufferIndex + 1;
    g_bufferIndex = g_bufferIndex & 1;

    if (g_fadeMode == FADE_IN) {
        s32 fc;
        fc = (u8)g_fadeCounter + 1;
        g_fadeCounter = fc;
        if ((fc & 0xFF) == 0x80) {
            g_renderMode = RENDER_IDLE;
            g_fadeMode = FADE_NONE;
        }
        if (fc & 0x6) {
            return;
        }
    } else {
        g_fadeCounter = (u8)g_fadeCounter + 1;
        if ((u8)g_fadeCounter == 0x22) {
            g_renderMode = RENDER_IDLE;
            g_fadeMode = FADE_NONE;
        }
    }

    SetupDrawMode(2);
    BuildPrimList(16);
    PutDispEnv(&g_dispEnvs[(s16)g_bufferIndex]);
    PutDrawEnv(&g_drawEnvs[(s16)g_bufferIndex]);
    DrawOTag(&g_orderingTablePtrs[(s16)g_bufferIndex]);
}
