#include "common.h"
#include "menu.h"
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
#include "world/we_object0.h"
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
#include "cpu.h"

#ifdef ADD_OMITTED_INCLUDES
#include "overlay.h"
#endif

typedef enum {
    FADE_NONE = 0,
    FADE_IN = 1,
    FADE_OUT = 2
} FadeMode;

#define PACKET_BUFFER_SIZE 0x6000
#define CHOCOBO_WORLD_MUSIC_SIZE 0x200

extern u8 g_vsyncSkip;
extern volatile s32 g_countdownTicker;
extern u8 g_soundEffectData[];
extern u8 g_packetBuffer[PACKET_BUFFER_SIZE];
extern u8 g_chocoboWorldMusic[CHOCOBO_WORLD_MUSIC_SIZE];

/** @brief Global field ids of the maps a new game and a game over go to. */
#define FIELD_ID_START0 74 /**< start0, the first map of a new game. */
#define FIELD_ID_GOVER 75 /**< gover, the game-over map. */
#define FADE_STEP 16
#define CLOCK_TICKS_PER_VSYNC 2191
#define CLOCK_ONE_SECOND_BIT 17
#define CLOCK_REMAINDER_MASK 0xFFFF

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
static inline void RunBattle(u8 *musicBuffer);

static void VsyncHandler(void);
static void InitHardware(void);
static void LoadFieldOverlay(void);
static void LoadWorldOverlay(void);
static void LoadWm2FieldTable(void);
static void InitSoundEngine(void);
static void LoadBootOverlay(void);
static void LoadSystemFont(void);
static void InitGamePadsAndMemoryCards(void);
static void InitSystemData(void);
static void LoadDiscTableOfContents(void);
static void PrepareFadePass(s32 step);
static void ProcessFade(void);

static CdFileDesc g_fileTableDesc[] = {{IMG_TOC_SECTOR, IMG_TOC_SIZE}};

static void DisableStpBit(void) {
    DR_STP prim;
    u32 ot[2];

    ClearOTag(ot, 2);
    SetDrawStp(&prim, 0); // semi-transparency bit
    addPrim(ot, &prim);
    DrawOTag(ot);

    while (DrawSync(1));
}

static void VsyncHandler(void) {
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
    g_secondTicker += CLOCK_TICKS_PER_VSYNC;
    if (g_secondTicker >> CLOCK_ONE_SECOND_BIT) {
        g_gameState.mainData.playTimeSeconds++;
        g_secondTicker &= CLOCK_REMAINDER_MASK;
    }

    g_countdownTicker += CLOCK_TICKS_PER_VSYNC;
    if (g_countdownTicker >> CLOCK_ONE_SECOND_BIT) {
        if (g_gameState.mainData.countdownTimer == 0) {
            g_vsyncDone = 1;
            return;
        }
        g_gameState.mainData.countdownTimer--;
        g_countdownTicker &= CLOCK_REMAINDER_MASK;
    }

    g_vsyncDone = 1;
}

static void InitHardware(void) {
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
        g_gameState.fieldStateSnapshot.facingDirection[i] =
            g_fieldActors[g_fieldContext.entityIndex[i]].facingDirection;
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

static void LoadFieldOverlay(void) {
    if (g_previousGameMode != GAME_MODE_MENU && g_previousGameMode != GAME_MODE_SCRIPT_MENU) {
        // field.ovl
        LoadCompressedDataFromDisc(g_fieldFileDescs[0].sector, g_fieldFileDescs[0].size, OVERLAY_PTR, NULL);
        while (UpdateCdRead() != CD_READ_IDLE);
    }
    FlushInstructionCache();
}

static void LoadWorldOverlay(void) {
    if (g_previousGameMode != GAME_MODE_MENU && g_previousGameMode != GAME_MODE_SCRIPT_MENU) {
        // world.ovl
        LoadCompressedDataFromDisc(g_worldFileDescs[0].sector, g_worldFileDescs[0].size, OVERLAY_PTR, NULL);
        while (UpdateCdRead() != CD_READ_IDLE);
    }
    FlushInstructionCache();
}

static void LoadWm2FieldTable(void) {
    // wm2field.tbl
    LoadDataFromDisc(g_fieldFileDescs[1].sector, g_fieldFileDescs[1].size, WORLD_TO_FIELD_ENTRANCES, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

static void InitSoundEngine(void) {
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

static void LoadBootOverlay(void) {
    // field_init.ovl
    LoadDataFromDisc(g_bootFileDescs[0].sector, g_bootFileDescs[0].size, OVERLAY_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

static void LoadSystemFont(void) {
    // sysfnt.tdw
    LoadDataFromDisc(g_systemFileDescs[0].sector, g_systemFileDescs[0].size, SCRATCH_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
    func_8002C3AC(SCRATCH_PTR, 0);
    while (DrawSync(1));
}

static void InitGamePadsAndMemoryCards(void) {
    LoadBootOverlay();
    func_80098390();
    initBattleDisplay();
}

static void InitSystemData(void) {
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
static void LoadDiscTableOfContents(void) {
    // img_toc.bin
    LoadDataFromDisc(g_fileTableDesc[0].sector, g_fileTableDesc[0].size, IMG_TOC_PTR, NULL);
    while (UpdateCdRead() != CD_READ_IDLE);
}

static inline void ReturnToTitle(void) {
    g_fieldContext.counter = 0;
    g_fieldContext.mode = FIELD_MODE_TITLE;
    g_fieldContext.spawnTriIdx = SPAWN_UNSET;
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

static inline void RunBattle(u8 *musicBuffer) {
    s16 i;

    func_80038030(musicBuffer);
    RunBattleModes();
    if (g_renderMode != RENDER_BATTLE_RESULTS) {
        DisableStpBit();
    }
    if (g_battleConfig.result == BATTLE_RESULT_SOFT_RESET) {
        // matching hack
        do {
        } while (0);
        ReturnToTitle();
        return;
    }
    if ((g_battleConfig.result == BATTLE_RESULT_GAMEOVER) && (!(g_fieldVars->battleFlags & NO_GAMEOVER_ON_LOSS))) {
        GoToGameOverField();
        return;
    }
    if (g_battleConfig.result == BATTLE_RESULT_TIME_UP) {
        if (g_fieldVars->battleFlags & NO_GAMEOVER_ON_TIME_UP) {
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
        if (g_battleConfig.obtainedGf[i] != GF_NONE) {
            do {
            } while (g_renderMode == RENDER_BATTLE_RESULTS);
            while (DrawSync(1));
            RunMenu(g_battleConfig.obtainedGf[i] + MENU_NAME_GF);
        }
    }
}

void ff8main(void) __asm__("main");
void ff8main(void) {
    WorldToFieldEntrance *entry;
    s16 mode;
    s16 fieldId;
    s32 result;
    u8 *musicBuffer;
    preInitStub();
    InitHardware();
    initCdSubsystem(1);
    LoadDiscTableOfContents();
    InitSoundEngine();
    LoadBootOverlay();
    BootMain();
    InitGamePadsAndMemoryCards();
    LoadWm2FieldTable();
    while (1) {
        LoadIntroOverlay();
        IntroMain(INTRO_SCREEN_BOOT);
        InitSystemData();
        g_gameMode = GAME_MODE_FIELD;
        g_previousGameMode = GAME_MODE_NONE;
        g_fieldContext.userControlOff = 0;
        g_prefetchState = PREFETCH_NONE;
        g_prefetchedDestId = 73; // the debug "menu"
        g_fieldContext.mode = FIELD_MODE_NONE;
        g_fieldContext.mapJumpOff = 0;
        result = RunMenu(MENU_TITLE);

        if (!(result & MENU_RESULT_NEW_GAME)) {
            // Load game
            RestoreFieldSnapshot();
            LoadFieldOverlay();
            SmInitEventAll(0);
        } else {
            // New game
            g_curFieldId = FIELD_ID_START0;
            g_gameState.battleParty[0] = CHAR_SQUALL;
            g_gameState.battleParty[1] = PARTY_SLOT_EMPTY;
            g_gameState.battleParty[2] = PARTY_SLOT_EMPTY;
            g_gameState.battleParty[3] = PARTY_SLOT_EMPTY;
            LoadFieldOverlay();
            SmInitEventAll(1);
        }

        // Check that the game has correct disc before proceeding to loaded or new game
        if (g_fieldVars->expectedDiscId != getDiscId()) {
            LoadIntroOverlay();
            IntroMain(INTRO_SCREEN_INSERT_DISC);
            ReloadDiscTables();
        }

        do {
            // matching hack
            do {
                switch (g_gameMode) {
                case GAME_MODE_FIELD:
                    LoadFieldOverlay();
                    FieldMain();
                    break;
                case GAME_MODE_WORLD:
                    switch (g_previousGameMode) {
                    case GAME_MODE_FIELD:
                        func_800B4F40();
                        g_worldTransfer.mode = WORLD_ENTER_FROM_FIELD;
                        break;
                    case GAME_MODE_BATTLE:
                        g_worldTransfer.mode = WORLD_ENTER_FROM_BATTLE;
                        break;
                    case GAME_MODE_MENU:
                        g_worldTransfer.mode = WORLD_ENTER_FROM_MENU;
                        break;
                    case GAME_MODE_NONE:
                        g_worldTransfer.mode = WORLD_ENTER_FROM_LOAD;
                        break;
                    }
                    // Check if the world overlay has been prefetched into memory and unpack it in that case, otherwise
                    // load it from disc.
                    if (((g_previousGameMode == GAME_MODE_FIELD) && (g_prefetchState != PREFETCH_NONE)) &&
                        (g_prefetchedDestId < 74)) {
                        while (UpdateCdRead() != CD_READ_IDLE);
                        DecompressLzs(g_prefetchBuffer, OVERLAY_PTR);
                    } else {
                        LoadWorldOverlay();
                    }
                    g_prefetchState = PREFETCH_NONE;

                    if (WorldMain() == 1) {
                        // Soft reset from World
                        ReturnToTitle();
                        sndStopAll();
                        break;
                    }
                    g_previousGameMode = GAME_MODE_WORLD;
                    switch (g_worldTransfer.mode) {
                    case WORLD_EXIT_TO_FIELD:
                        entry = &WORLD_TO_FIELD_ENTRANCES[g_worldTransfer.param.entrance.entranceIndex];
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
                    RunBattleModes();
                    g_previousGameMode = GAME_MODE_BATTLE;
                    g_gameMode = GAME_MODE_FIELD;
                    break;
                case GAME_MODE_BATTLE:
                case GAME_MODE_BATTLE_DEBUG_MENU:
                    if (g_previousGameMode == GAME_MODE_FIELD) {
                        g_battleConfig.battleSceneId = g_fieldContext.counter;
                        musicBuffer = g_fieldSoundBuffer;
                    } else {
                        g_battleConfig.battleSceneId =
                            g_worldTransfer.param.battle.sceneLow | (g_worldTransfer.param.battle.sceneHigh << 8);
                        musicBuffer = (u8 *)MUSIC_BUFFER_ADDR;
                    }
                    RunBattle(musicBuffer);
                    break;
                case GAME_MODE_MENU:
                case GAME_MODE_SCRIPT_MENU:
                    while (g_renderMode != RENDER_IDLE);
                    SetDispMask(0);
                    SaveFieldSnapshot();
                    if (g_previousGameMode == GAME_MODE_FIELD) {
                        result = RunMenu(g_fieldContext.counter, g_fieldContext.menuArg);
                        if (result & MENU_RESULT_PARTY_CHANGED) {
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
                        result = RunMenu(MENU_FROM_WORLD_MAP | MENU_MAIN, g_fieldVars->menuPermissions | MENU_ALLOW_SAVE);
                        g_previousGameMode = GAME_MODE_MENU;
                        g_gameMode = GAME_MODE_WORLD;
                    }
                    if (!(result & MENU_RESULT_START_BATTLE)) {
                        break;
                    }
                    g_previousGameMode = g_gameMode;
                    g_gameMode = GAME_MODE_BATTLE;

                    // When menu triggers a battle, RunMenu returns the battle scene id in the upper 16 bits of the result.
                    // So shift it to get the real id.
                    g_battleConfig.battleSceneId = result >> BATTLE_SCENE_ID_SHIFT;
                    if (g_gameMode == GAME_MODE_FIELD) {
                        musicBuffer = g_fieldSoundBuffer;
                    } else {
                        // matching hack
                        result = MUSIC_BUFFER_ADDR;
                        musicBuffer = (u8 *)result;
                    }
                    RunBattle(musicBuffer);
                    break;
                case GAME_MODE_DISC_CHANGE:
                    LoadIntroOverlay();
                    IntroMain(g_fieldContext.introScreenType);
                    mode = g_fieldContext.introScreenType;
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

void SetFadeBlendMode(BlendMode blendMode) {
    u16 tpage;

    tpage = GetTPage(0, blendMode, 0, 0);
    SetDrawMode(&g_clearTiles[0].mode, 0, 0, tpage, 0);
    SetDrawMode(&g_clearTiles[1].mode, 0, 0, tpage, 0);
}

void InitClearTiles(void) {
    s32 i;

    for (i = 0; i < 2; i++) {
        SetTile(&g_clearTiles[i].tile);
        SetSemiTrans(&g_clearTiles[i].tile, 1);
        SetShadeTex(&g_clearTiles[i].tile, 1);
        setRGB0(&g_clearTiles[i].tile, 0, 0, 0);
        setWH(&g_clearTiles[i].tile, 320, 224);
        setXY0(&g_clearTiles[i].tile, 0, 0);
    }
}

void CopyFrameBuffer(void) {
    MoveImage(&g_dispEnvs[g_bufferIndex].disp, g_dispEnvs[(g_bufferIndex + 1) & 1].disp.x,
              g_dispEnvs[(g_bufferIndex + 1) & 1].disp.y);
    while (DrawSync(1));
}

static void PrepareFadePass(s32 step) {
    ClearOTag(&g_orderingTablePtrs[g_bufferIndex], 1);
    setRGB0(&g_clearTiles[g_bufferIndex].tile, step, step, step);
    addPrim(&g_orderingTablePtrs[g_bufferIndex], &g_clearTiles[g_bufferIndex].tile);
    addPrim(&g_orderingTablePtrs[g_bufferIndex], &g_clearTiles[g_bufferIndex].mode);
}

static void ProcessFade(void) {
    if (g_fadeMode == FADE_NONE) {
        return;
    }

    g_bufferIndex++;
    g_bufferIndex &= 1;

    if (g_fadeMode == FADE_IN) {
        g_fadeCounter++;
        if (g_fadeCounter == 128) {
            g_renderMode = RENDER_IDLE;
            g_fadeMode = FADE_NONE;
        }
        if (g_fadeCounter & 0x6) {
            return;
        }
    } else {
        g_fadeCounter++;
        if (g_fadeCounter == 34) {
            g_renderMode = RENDER_IDLE;
            g_fadeMode = FADE_NONE;
        }
    }

    SetFadeBlendMode(BLEND_SUBTRACT);
    PrepareFadePass(FADE_STEP);
    PutDispEnv(&g_dispEnvs[g_bufferIndex]);
    PutDrawEnv(&g_drawEnvs[g_bufferIndex]);
    DrawOTag(&g_orderingTablePtrs[g_bufferIndex]);
}
