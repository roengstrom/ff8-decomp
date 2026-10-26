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
#include "psxsdk/libapi.h"
#include "psxsdk/libspu.h"
#include "psxsdk/crt0.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libetc.h"
#include "main.h"
#include "btl_transition.h"
#include "wm2field.h"

/** @brief Global field ids of the maps a new game and a game over go to. */
#define FIELD_ID_START0 74 /**< start0, the first map of a new game. */
#define FIELD_ID_GOVER 75 /**< gover, the game-over map. */

/** @brief Turn the GPU's mask-bit setting off with one packet, and wait until the GPU is idle. */
void flushGpuOt(void) {
    DR_STP prim;
    u32 ot[2];

    ClearOTag(ot, 2);
    SetDrawStp(&prim, 0);
    addPrim(ot, &prim);
    DrawOTag(ot);

    do {
    } while (DrawSync(1) != 0);
}

/** @brief VSync callback handler, registered via VSyncCallback in InitHardware.
 *
 *  Dispatches per-frame rendering by the current render mode and advances the
 * play time and the countdown, both counted in seconds.
 */
void VsyncHandler(void) {
    switch (g_renderMode) {
    case 0:
        break;
    case 1:
        ProcessFade();
        break;
    case 2:
        paceBattleTransition();
        break;
    case 3:
        func_800D0608();
        break;
    case 4:
        vsyncGameHandler();
        break;
    }

    if (IsPlayTimePaused() != 0) {
        D_8005F11E = 1;
        return;
    }
    if (g_vsyncSkip != 0) {
        D_8005F11E = 1;
        return;
    }

    D_8005F154 += 0x88F;
    if (D_8005F154 >> 17) {
        g_gameState.mainData.playTimeSeconds++;
        D_8005F154 &= 0xFFFF;
    }

    D_8005F15C += 0x88F;
    if (D_8005F15C >> 17) {
        if (g_gameState.mainData.countdownTimer == 0) {
            D_8005F11E = 1;
            return;
        }
        g_gameState.mainData.countdownTimer--;
        D_8005F15C &= 0xFFFF;
    }

    D_8005F11E = 1;
}


/** @brief Initializes the PS1 hardware (memory, callbacks, GPU, GTE, SPU) and
 *         registers the VSync handler. Called once at startup.
 */
void InitHardware(void) {
    SetMem(2);
    StopCallback();
    ResetCallback();
    ResetGraph(0);
    InitSpu();
    g_vsyncSkip = 0;
    g_renderMode = RENDER_IDLE;
    VSyncCallback(VsyncHandler);
    SetGraphDebug(0);
    SetDispMask(0);
    InitGeom();
}

/** @brief Saves the current battle/field camera state to a snapshot buffer.
 *
 *  Records the party's positions, rotations, animation states, and display
 *  parameters so RestoreSnapshot can restore them when returning from battle.
 */
void func_80011870(void) {
    s16 i;

    if (g_previousGameMode == 2) {
        g_gameState.cameraSnapshot.vsyncRate = 2;
    } else {
        g_gameState.cameraSnapshot.vsyncRate = 1;
    }

    g_gameState.cameraSnapshot.fieldId = g_curFieldId;
    g_gameState.cameraSnapshot.field120 = g_fieldEntity.field_0x120;

    for (i = 0; i < 3; i++) {
        g_gameState.cameraSnapshot.positionsX[i] = D_80085224[g_fieldEntity.entityIndex[i]].posX >> 12;
        g_gameState.cameraSnapshot.positionsY[i] = D_80085224[g_fieldEntity.entityIndex[i]].posY >> 12;
        g_gameState.cameraSnapshot.triIdxs[i]   = D_80085224[g_fieldEntity.entityIndex[i]].triIdx;
        g_gameState.cameraSnapshot.animStates[i] = D_80085224[g_fieldEntity.entityIndex[i]].field_0x241;
    }

    g_gameState.cameraSnapshot.fade1 = D_8005F151;
    g_gameState.cameraSnapshot.fade0 = D_8005F150;
}

/** @brief Restores a previously saved camera/field state snapshot.
 *
 *  Inverse of func_80011870: writes the saved party positions, rotations,
 *  animation states, and display parameters back into the live field state.
 */
void RestoreSnapshot(void) {
    SystemState *entity;

    g_gameMode = g_gameState.cameraSnapshot.vsyncRate;
    if (g_gameMode == 0) {
        g_gameMode = 1;
    }

    g_curFieldId = g_gameState.cameraSnapshot.fieldId;

    entity = &g_fieldEntity;

    entity->field_0x120 = g_gameState.cameraSnapshot.field120;
    entity->position_x = g_gameState.cameraSnapshot.positionsX[0];
    entity->position_y = g_gameState.cameraSnapshot.positionsY[0];
    entity->spawnTriIdx = g_gameState.cameraSnapshot.triIdxs[0];
    entity->facingDirection = g_gameState.cameraSnapshot.animStates[0];

    D_8005F151 = g_gameState.cameraSnapshot.fade1;
    D_8005F150 = g_gameState.cameraSnapshot.fade0;
}


/** @brief Loads field data set A from CD, unless the current field mode
 *         doesn't require it, then resets the sound state.
 */
void loadFieldDataA(void) {
    if (g_previousGameMode != 6 && g_previousGameMode != 0xA) {
        LoadCompressedDataFromDisc(D_80097410[0].sector, D_80097410[0].size, 0x80098000, 0);
        while (func_800393C8() != 0)
            ;
    }
    func_8001F5C8();
}

/** @brief Loads field data set B from CD - same as loadFieldDataA with a
 *         different source.
 *
 *  @note Purpose uncertain - the A/B distinction may correspond to different
 *        field maps or disc configurations.
 */
void loadFieldDataB(void) {
    if (g_previousGameMode != 6 && g_previousGameMode != 0xA) {
        LoadCompressedDataFromDisc(D_800974D0[0].sector, D_800974D0[0].size, 0x80098000, 0);
        while (func_800393C8() != 0)
            ;
    }
    func_8001F5C8();
}

void LoadWm2FieldTable(void) {
    LoadDataFromDisc(D_80097410[1].sector, D_80097410[1].size, WORLD_TO_FIELD_ENTRANCES, NULL);
    while (func_800393C8() != 0) {}
}

/** @brief Initializes the sound engine and loads its audio assets (sound bank
 *         header and sample banks) from CD into SPU RAM.
 */
void initSoundEngine(void) {
    sndInit();

    LoadDataFromDisc(D_800974D8[0].sector, D_800974D8[0].size, (s32)D_80067468, 0);
    while (func_800393C8() != 0)
        ;

    sndLoadBank((s32)D_80067468);

    LoadDataFromDisc(D_800974D8[1].sector, D_800974D8[1].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    sndUploadSamples(0x801B0000, 1);

    LoadDataFromDisc(D_800974D8[2].sector, D_800974D8[2].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    sndUploadSamples(0x801B0000, 1);
}


/** @brief Loads an overlay from CD into the overlay region; which overlay is
 *         loaded depends on the file table set up by loadFileTable.
 */
void loadCdOverlay(void) {
    LoadDataFromDisc(D_80097400[0].sector, D_80097400[0].size, 0x80098000, 0);
    while (func_800393C8() != 0)
        ;
}


/** @brief Loads texture data from CD and uploads it to VRAM. */
void loadAndUploadTextures(void) {
    LoadDataFromDisc(D_80097808[0].sector, D_80097808[0].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;
    func_8002C3AC(0x801B0000, 0);
    while (DrawSync(1) != 0)
        ;
}

/** @brief Loads the field/battle overlay and runs its initialization. */
void initFieldModule(void) {
    loadCdOverlay();
    func_80098390();
    initBattleDisplay();
}

/** @brief Extended battle/visual initialization: loads the overlay, animation
 *         buffers, textures, and battle model/lookup data from CD.
 */
void initBattleAssets(void) {
    loadCdOverlay();
    initBattleAnimSystem((s32)D_8006A468, 0x6000);
    loadAndUploadTextures();

    LoadDataFromDisc(D_80097808[1].sector, D_80097808[1].size, 0x801B0000, 0);
    while (func_800393C8() != 0)
        ;

    loadBattleTimImage(0x801B0000);

    LoadDataFromDisc(D_80097808[3].sector, D_80097808[3].size, 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    func_80039678((s32)D_8005F188, 0x80090000, 0x200);

    LoadDataFromDisc(D_80097808[2].sector, D_80097808[2].size, 0x80090000, 0);
    while (func_800393C8() != 0)
        ;

    setBattleEntityBase(0x80090000);
}


/** @brief Loads the master file descriptor table from CD. Must run before any
 *         other CD load, since every other load descriptor lives in this table.
 */
void loadFileTable(void) {
    LoadDataFromDisc(g_fileTableDesc[0].sector, g_fileTableDesc[0].size, 0x80097400, 0);
    while (func_800393C8() != 0)
        ;
}

/* ff8main's post-battle handling reads several g_fieldVars fields (continue /
 * game-over flags and the expected disc id) to choose the next transition. */


/** @brief Game main function and primary state-machine loop.
 *
 *  Runs one-time hardware, CD, and asset initialization, then loops the field
 *  and battle state machine until the game exits, reinitializing on exit.
 */
/* gcc 2.7.2 injects a __main() C-runtime-init call into any function literally
 * named `main`, which the shipped executable does not have. Writing it as
 * ff8main and aliasing the symbol to `main` emits the correct `main` symbol
 * without that call. */
void ff8main(void) __asm__("main");
void ff8main(void) {
    WorldToFieldEntrance *entry;
    s16 i;
    s16 mode;
    s16 fieldId;
    s32 result;
    s32 mapAddr;
    preInitStub();
    InitHardware();
    initCdSubsystem(1);
    loadFileTable();
    initSoundEngine();
    loadCdOverlay();
    func_80098000();
    initFieldModule();
    LoadWm2FieldTable();
    while (1) {
        LoadCompressedDataFromDisc(D_80097400[1].sector, D_80097400[1].size, 0x80098000, 0);
        while (func_800393C8() != 0) {}

        func_80098FD4(0);
        initBattleAssets();
        g_gameMode = 1;
        g_previousGameMode = 0;
        g_fieldEntity.unk1A3 = 0;
        D_8005F14A = 0;
        D_8005F100 = 0x49;
        g_fieldEntity.mode = 0;
        g_fieldEntity.unk1A2 = 0;
        result = func_8003646C(25);
        if (!(result & 2)) {
            RestoreSnapshot();
            loadFieldDataA();
            SmInitEventAll(0);
        } else {
            g_curFieldId = FIELD_ID_START0;
            g_gameState.battleParty[0] = 0;
            g_gameState.battleParty[1] = 0xFF;
            g_gameState.battleParty[2] = 0xFF;
            g_gameState.battleParty[3] = 0xFF;
            loadFieldDataA();
            SmInitEventAll(1);
        }
        if (g_fieldVars->expectedDiscId != getDiscId()) {
            LoadCompressedDataFromDisc(D_80097400[1].sector, D_80097400[1].size, 0x80098000, 0);
            while (func_800393C8() != 0) {}

            func_80098FD4(1);
            loadFileTable();
            LoadWm2FieldTable();
        }
        do {
            mode = g_gameMode - 1;
            do {
            switch (mode) {
            case 0:
                loadFieldDataA();
                func_8009895C();
                break;
            case 1:
                switch (g_previousGameMode) {
                case 1:
                    func_800B4F40();
                    D_80082C8C.mode = 0;
                    break;
                case 3:
                    D_80082C8C.mode = 2;
                    break;
                case 6:
                    D_80082C8C.mode = 4;
                    break;
                case 0:
                    D_80082C8C.mode = 6;
                    break;
                }
                if (((g_previousGameMode == 1) && (D_8005F14A != 0)) && (D_8005F100 < 0x4A)) {
                    while (func_800393C8() != 0) { do {} while (0); }
                    func_80038490(D_8005F104, 0x80098000);
                } else {
                    loadFieldDataB();
                }
                D_8005F14A = 0;
                if (func_800987D8() == 1) {
                    g_fieldEntity.counter = 0;
                    g_fieldEntity.mode = 4;
                    g_fieldEntity.spawnTriIdx = 0x7FFF;
                    sndStopAll();
                    break;
                }
                g_previousGameMode = 2;
                switch (D_80082C8C.mode) {
                case 1:
                    entry = &WORLD_TO_FIELD_ENTRANCES[D_80082C8C.param.entrance.entranceIndex];
                    fieldId = entry->fieldId;
                    g_fieldEntity.position_x = entry->position_x;
                    g_fieldEntity.position_y = entry->position_y;
                    g_fieldEntity.spawnTriIdx = entry->spawnTriIdx;
                    g_curFieldId = fieldId;
                    g_fieldEntity.facingDirection = entry->facingDirection;
                    g_gameMode = 1;
                    sndCmd21(-1, 0);
                    break;
                case 3:
                    g_gameMode = 3;
                    break;
                case 5:
                    g_gameMode = 6;
                    break;
                }
                break;
            case 7:
                gameStateLoop();
                g_previousGameMode = 3;
                g_gameMode = 1;
                break;
            case 2:
            case 3:
                if (g_previousGameMode == 1) {
                    g_battleConfig.battleSceneId = g_fieldEntity.counter;
                    mapAddr = D_8005F13C;
                } else {
                    g_battleConfig.battleSceneId = D_80082C8C.param.battle.sceneLow | (D_80082C8C.param.battle.sceneHigh << 8);
                    mapAddr = 0x801D0000;
                }
                func_80038030(mapAddr);
                gameStateLoop();
                if (g_renderMode != 4) {
                    flushGpuOt();
                }
                if (g_battleConfig.result == 5) {
                    g_fieldEntity.counter = 0;
                    g_fieldEntity.mode = 4;
                    g_fieldEntity.spawnTriIdx = 0x7FFF;
                    break;
                }
                if ((g_battleConfig.result == 1) && (!(g_fieldVars->fieldB6 & 0x200))) {
                    g_curFieldId = FIELD_ID_GOVER;
                    g_previousGameMode = 0;
                    g_gameMode = 1;
                    break;
                }
                if (g_battleConfig.result == 3) {
                    if (g_fieldVars->fieldB6 & 0x100) {
                        g_fieldVars->stateFlags &= ~FIELD_STATE_COUNTDOWN;
                    } else {
                        g_curFieldId = FIELD_ID_GOVER;
                        g_previousGameMode = 0;
                        g_gameMode = 1;
                        break;
                    }
                }
                if (g_previousGameMode == 1) {
                    g_previousGameMode = 3;
                    g_gameMode = 1;
                } else {
                    g_previousGameMode = 3;
                    g_gameMode = 2;
                }
                for (i = 0; i < 3; i++) {
                    if (g_battleConfig.unk4[i] != 0xFF) {
                        do {} while (g_renderMode == 4);
                        while (DrawSync(1) != 0) {}
                        func_8003646C(g_battleConfig.unk4[i] + 5);
                    }
                }
                break;
            case 5:
            case 9:
                while (g_renderMode != 0) {}
                SetDispMask(0);
                func_80011870();
                if (g_previousGameMode == 1) {
                    result = func_8003646C(g_fieldEntity.counter, g_fieldEntity.unk1AB);
                    if (result & 1) {
                        clearEntityFlags();
                        if (g_gameMode == 6) {
                            g_previousGameMode = 0xA;
                        } else {
                            g_previousGameMode = 6;
                        }
                    } else {
                        g_previousGameMode = 6;
                    }
                    g_gameMode = 1;
                } else {
                    result = func_8003646C(0x80000000, g_fieldVars->fieldD1 | 1);
                    g_previousGameMode = 6;
                    g_gameMode = 2;
                }
                if (!(result & 4)) {
                    break;
                }
                g_previousGameMode = g_gameMode;
                g_gameMode = 3;
                g_battleConfig.battleSceneId = (u16)(result >> 16);
                if (g_gameMode == 1) {
                    mapAddr = D_8005F13C;
                } else {
                    result = 0x801D0000;
                    mapAddr = result;
                }
                func_80038030(mapAddr);
                gameStateLoop();
                if (g_renderMode != 4) {
                    flushGpuOt();
                }
                if (g_battleConfig.result == 5) {
                    do { } while (0);
                    g_fieldEntity.counter = 0;
                    g_fieldEntity.mode = 4;
                    g_fieldEntity.spawnTriIdx = 0x7FFF;
                    break;
                }
                if ((g_battleConfig.result == 1) && (!(g_fieldVars->fieldB6 & 0x200))) {
                    g_curFieldId = FIELD_ID_GOVER;
                    g_previousGameMode = 0;
                    g_gameMode = 1;
                    break;
                }
                if (g_battleConfig.result == 3) {
                    if (g_fieldVars->fieldB6 & 0x100) {
                        g_fieldVars->stateFlags &= ~FIELD_STATE_COUNTDOWN;
                    } else {
                        g_curFieldId = FIELD_ID_GOVER;
                        g_previousGameMode = 0;
                        g_gameMode = 1;
                        break;
                    }
                }
                if (g_previousGameMode == 1) {
                    g_previousGameMode = 3;
                    g_gameMode = 1;
                } else {
                    g_previousGameMode = 3;
                    g_gameMode = 2;
                }
                for (i = 0; i < 3; i++) {
                    if (g_battleConfig.unk4[i] != 0xFF) {
                        do {} while (g_renderMode == 4);
                        while (DrawSync(1) != 0) {}
                        func_8003646C(g_battleConfig.unk4[i] + 5);
                    }
                }
                break;
            case 8:
                LoadCompressedDataFromDisc(D_80097400[1].sector, D_80097400[1].size, 0x80098000, 0);
                while (func_800393C8() != 0) {}
                func_80098FD4(g_fieldEntity.unk1A0);
                mode = g_fieldEntity.unk1A0;
                if (mode == 1) {
                    loadFileTable();
                    LoadWm2FieldTable();
                    g_previousGameMode = 0;
                    g_gameMode = mode;
                    g_curFieldId = g_fieldEntity.counter;
                }
                break;
            }
            } while (0);
        }
        while (g_fieldEntity.mode != 4);
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
    if (++i < 2) goto top;
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

    MoveImage(
        (void *)(ofs1 + base),
        *(s16 *)(((s16)g_bufferIndex + 1 & 1) * 20 + base),
        *(s16 *)(((s16)g_bufferIndex + 1 & 1) * 20 + base + 2)
    );

    do {
    } while (DrawSync(1) != 0);
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
