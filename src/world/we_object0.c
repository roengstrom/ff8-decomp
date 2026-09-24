#include "common.h"
#include "gamestate.h"
#include "btl_color.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libetc.h"
#include "world.h"
#include "world/we_object0.h"
#include "world/we_object1.h"
#include "world/we_object2.h"
#include "world/we_object3.h"
#include "world/we_object4.h"
#include "world/we_object5.h"
#include "world/we_object6.h"
#include "world/we_object7.h"
#include "world/we_object8.h"
#include "world/we_object9.h"
#include "world/we_object10.h"
#include "world/we_object13.h"
#include "main.h"
#include "thread.h"
#include "btl_anim.h"

#define SP_SAVE_SCRATCH(slot)                                                  \
    asm volatile("addu  $t0, %0, $zero\n\t"                                    \
                 "sw    $sp, 0($t0)\n\t"                                       \
                 "addiu $t0, $t0, -4\n\t"                                      \
                 "addu  $sp, $t0, $zero" : : "r"(slot) : "$8")

#define SP_RESTORE()                                                           \
    asm volatile("addiu $sp, $sp, 4\n\t"                                       \
                 "lw    $sp, 0($sp)")

/* Screen size, used to calculate where to place the maps on screen. */
#define WORLD_SCREEN_W 320
#define WORLD_SCREEN_H 224

/* Frames the streamer is given to settle before the loop starts drawing;
 * mode 4 (a warm re-entry) gets the short count. */
#define WORLD_SETTLE_FRAMES      0x18
#define WORLD_SETTLE_FRAMES_WARM 8

/* D_800C9ED0 holding this value for two consecutive frames ends the loop. */
#define WORLD_STALL_CODE   0x90F
#define WORLD_STALL_FRAMES 2

/** Map display mode held in @c D_800C4D28. Mutually exclusive modes,
 *  not flags: the loop dispatches on the exact value. */
typedef enum {
    MAP_DISPLAY_OFF         = 0, // No map at all
    MAP_DISPLAY_GLOBE       = 1, // Globe in the bottom right corner
    MAP_DISPLAY_TRANSPARENT = 2, // Transparent square map in bottom right corner
    MAP_DISPLAY_FULL        = 3  // Full screen map with hand pointer and locations etc.
} MapDisplayMode;

/* Edge-detected bits of the world pad word @c D_800D2278, tested as
   cur & (cur ^ prev) -- "went set this frame". Which physical buttons these
   are is not established, so each is named for what the loop does when it
   fires; the pad word is FF8's own, not the raw PSX button mask. */
#define WORLD_PAD_HANDOFF 0x20    /* runs the scene-change checks */
#define WORLD_PAD_REDRAW  0x800   /* re-runs setupWorldRender() */

/* Columns the linear tile index from func_800A5DC8 decomposes into. */
#define MAP_DISPLAY_COLS 128

/* Screen position of the corner globe; the other modes derive theirs from
   the current screen size instead. */
#define MAP_DISPLAY_GLOBE_X 0x104
#define MAP_DISPLAY_GLOBE_Y 0xB4

/* The transparent map sits this far in from the screen's right/bottom edge. */
#define MAP_DISPLAY_TRANSPARENT_DX 0x86
#define MAP_DISPLAY_TRANSPARENT_DY 0x64

/* The full-screen map is this size, centred on the screen. */
#define MAP_DISPLAY_FULL_W 0x100
#define MAP_DISPLAY_FULL_H 0xC0

/* Globals this unit is the first C code to touch. Nothing else in the tree
   names them yet, so they are file-local declarations here rather than a
   header's public surface. */
extern s32     D_800C4D08;   /**< Hand-off action code published on the way out. */
extern s32     D_800C4D28;   /**< Map display mode; see MAP_DISPLAY_*. */
extern s32     D_800C4D80;   /**< Last render-callback status. */
extern s32     D_800C4F0C[];
extern s32     D_800C4FC4;   /**< Frames left in the settle countdown. */
extern SVECTOR D_800C5400;
extern MATRIX  D_800C5408;
extern s32     D_800C8638;   /**< Frames spent waiting for the streamer. */
extern s32     D_800C973C;
extern u8      D_800C9762;
extern u8     *D_800C9768;
extern s32     D_800C97D8;
extern s32     D_800C9828[];
extern SVECTOR D_800C9E60;   /**< Scratch copy of the camera's second angle triple. */
extern u8     *D_800C9E74;
extern u8     *D_800C9E78;
extern s32     D_800C9E7C;
extern s32    *D_800C9FEC;
extern s32     D_800C9FF0[];
extern s32     D_800D2130[];
extern s32    *D_800D2230;
extern s32    *D_800D2268;
extern s32     D_800D2270;
extern s32     D_800D2290[];
extern u8      D_800D23DC;   /**< Same byte as @c D_800D23D8[4]. */
extern SVECTOR D_800D2450;
extern s32    *D_800D245C;

extern Slot   D_800785D8;   /**< Slot-state record the loop publishes at D_800D226C. */

s32 func_800987D8(void)
{
    s32 trigger;
    s8 code;
    s32 transparentTile;
    s32 fullMapTile;
    s16 mapX;
    s16 mapY;
    s32 reqKind;
    s32 padBank;
    s32 mode;
    s32 cmd;
    s32 firstPass;
    s32 stall;
    u16 sceneCmd;
    u16 hitCmd;
    u16 objMarker;
    s32 result;

    firstPass = 1;
    stall = 0;
    result = 0;

    if (D_80082C8C.mode == 2) {
        func_8009A7C0();
        func_8009CAE0(D_800C4F2C, NULL);
        while (g_renderMode == 4) {
        }
    }
    func_800C4644();
    sndResetState();
    D_800C97EA = WORLD_SCREEN_W;
    D_800C97E8 = WORLD_SCREEN_H;
    D_800D226C = &D_800785D8;
    if (D_80082C8C.mode != 0) {
        D_80082C8C.cmd = D_800D226C->scene.cmd;
    } else {
        D_800D226C->scene = D_80082C8C;
    }
    D_800D2238 = 0;
    D_800C971C = 0;
    D_800C9762 = 1;
    func_8009CCC8(VSync(-1));
    func_8009C8CC(VSync(-1));
    func_8009CCDC(0);
    InitGeom();
    setupWorldRenderParams();
    initWorldDoubleBuffer();
    func_8009D0F0();

    if (D_80082C8C.mode != 4) {
        func_8009A638();
        func_800B7178();
        func_800AD688();
        func_800BC44C();
        if (D_80082C8C.mode != 2) {
            func_8009A7C0();
            func_8009CAE0(D_800C4F2C, func_800A5D10);
        }
        func_8009A954();
        func_800AD698(&D_80082C8C, D_800D23D8);
        func_80099C84();
        func_800B7240(D_80082C8C.unk02);
        func_800AEEB0(D_800D23D8, D_800D2290, D_800D2130, D_800C9FF0);
        func_800A581C();
        loadImageScript(D_800C9FEC);
        loadImageScript(D_800D2268);
        D_800D2270 = D_800C9E7C;
        func_80099B48(D_800D23D8, D_800D245C);
        func_800A7CD0(D_800D2230);
        func_800BE7FC(D_800D23D8, D_800C9768);
        D_800C9714 = 1;
        D_800D2458 = 1;
        func_8009CB70(D_800D23D8, &D_800C4F0C, D_800C9E74, D_800C9E78);
        func_8009CC34();

        if (D_800C4FBC != 0) {
            if (sndGetStatus() != 0) {
                while (sndGetMaxVolume(1) != 0) {
                    VSync(2);
                }
            }
            sndCmdC0(0, 0);
            sndCmd11(0);
            VSync(2);
            sndProcessAudio(D_800C4FC0, 1);
            func_8009CDFC(D_800C4FBC, 0x7F);
        } else {
            D_800C4D88 = pickAudioTransition(D_80082C8C.cmd, D_800D23D8[0x6A]);
        }

        func_800A40C0();
        func_800AE31C(D_800D23D8);
        func_800ABD54();
        func_800ABC98();
        func_800A9ED4();
        func_800A9254();
        func_800B164C(&D_80082C8C, D_800D226C, D_800D23D8, &D_800C9868);
        setupWorldMapView(&D_800C9868, D_800C9770, &D_800D2390, &D_800C9828);
        D_800C9858 = D_800C9868;
        D_800C97F8[0] = D_800C9770[0];
        D_800C9858.vz -= 0x100;
        func_800AC468(D_800C9770, &D_800D2390, &D_800C9828, 0);
        func_8009C070();
        func_800BC570(&D_800D2290, &D_800D2130, &D_800C9FF0);

        while ((D_800C4D88 >> 31 & 1) < (D_800C4D80 = func_800A47A4())) {
            VSync(2);
            func_800A5D10();
            D_800C8638++;
        }

        func_8009C1A4();
        func_800BEECC();
        D_800C4D54 = (D_800C4D38 == 0x31) ? 5 : 0;
        D_800C4D58 = 0;
        func_800B881C();
        if (D_800C4D88 >= 0) {
            D_800C4D84 = 1;
            do {
                VSync(2);
                func_8009AEE4(D_800C4D88);
            } while (D_800C4D84 != 0);
        }
    }

    func_800A64DC();
    func_800A6BE0();
    func_800A246C();
    SetGeomScreen(D_800C9730);
    setCameraVibrateIntensity(0x1000);
    activateBattleAnim(0);
    VSync(0);
    SetDispMask(1);

    while (1) {
        func_800A01DC(firstPass);

        D_800C9720 = (D_800D244C == &D_800CA040) ? &D_800C9E88[1] : &D_800C9E88[0];
        g_activeDrawEnv = &D_800D244C->drawEnv;

        if (firstPass) {
            D_800C4FC4 = (D_80082C8C.mode == 4) ? WORLD_SETTLE_FRAMES_WARM
                                                : WORLD_SETTLE_FRAMES;
        }
        if (D_800C4FC4 != 0) {
            func_8009A4DC(D_800C4FC4);
            D_800C4FC4--;
        }

        D_800C9E60 = D_800C9770[1];
        if (D_800C4FC4 == 0) {
            func_8009DB88(D_800C9758);
        } else {
            func_8009FEDC(D_800C9758, 1);
        }
        func_800BE8B0(D_800D23D8, &D_800C9868, D_800C9770);
        func_8009E5C8(&D_800C9868, D_800C9758, D_800C9770, &D_800D2390);
        func_8009FEBC(&D_800C9868, D_800C9770);
        func_800AC468(D_800C9770, &D_800D2390, &D_800C9828, 0);
        func_8009C070();
        func_800A8C1C();
        func_8009FE80(&D_800C5400, &D_800C5408);
        func_800A8270(&D_800D2450);

        trigger = func_800AE518(D_800D23D8);
        if (trigger != 0) {
            code = -1;
            if (D_800D23D8[0] == 1 || D_800D23D8[0] == 2) {
                code = D_800D23D8[4];
            } else if (D_800D23D8[0] == 3 && (trigger & 1)) {
                code = func_800BEB84(0);
            } else if (D_800D23D8[0] == 4 && (trigger & 2)) {
                code = func_800BEB84(1);
            }
            if (code >= 0) {
                D_80082C8C.mode = 1;
                D_80082C8C.unk02 = code;
                D_80082C8C.unk03 = -1;
                D_80082C8C.cmd = D_800C4D38;
                func_800B3FD4(D_800D226C, 1);
                D_800C987C = 0;
                D_800C4D08 = D_800D23DC;
                func_8009D954();
                break;
            }
        }

        func_800B8C70();
        D_800C9720->ft3.base = D_800C9720->ft3.cur;
        D_800C9720->ft4.base = D_800C9720->ft4.cur;
        if (D_800D23D8[0] != 12) {
            func_800B04CC(D_800C9770, &D_800C9838, D_800D23D8, D_800C973C);
            if (D_800D23D8[0] != 12) {
                func_800BB4E8();
                func_800A9300();
            }
        }
        if (D_800C4FC4 <= 0) {
            func_800B4AA0();
        }
        func_800BEED4();
        func_800BC544(&D_800C9868, &D_800C9748);

        D_800C97D8 = 0;
        D_800C972C = 0;
        D_800D2448 = 0;
        D_800C9720->gt3.base = D_800C9720->gt3.cur;

        SP_SAVE_SCRATCH(getScratchAddr(255));
        if (D_800D23D8[0] != 12) {
            renderWorldMapFrame();
        }
        SP_RESTORE();

        D_800C4D50 = SquareRoot0(D_800C9E38.vx * D_800C9E38.vx +
                                 D_800C9E38.vy * D_800C9E38.vy +
                                 D_800C9E38.vz * D_800C9E38.vz);
        D_800C4D4C = SquareRoot0(D_800CA038.vx * D_800CA038.vx +
                                 D_800CA038.vy * D_800CA038.vy +
                                 D_800CA038.vz * D_800CA038.vz);

        func_800B18B8(D_800D23D8, &D_800C9868, D_800C9770);
        func_800B893C();
        func_800BEC60(D_800D23D8);
        func_800B9D38();
        func_800BB150(D_800D226C);

        if (D_800C4D84 != 0) {
            func_8009AEE4(D_800C4D88);
        } else {
            D_800C4D80 = func_800A47A4();
            if (D_800C4D80 == 0 && D_800C4D88 >= 0) {
                D_800C4D84 = 1;
            }
        }

        if (D_800C4FC4 <= 0) {
            reqKind = func_800B85DC(&sceneCmd);
            if (reqKind == 1) {
                func_8009C294(sceneCmd);
                break;
            }
            if (reqKind == 3) {
                func_8009D44C(sceneCmd);
                break;
            }
        }

        if (D_800C4FC4 <= 0) {
            if (D_800D23D8[0] == 0) {
                if (func_800BC46C(&hitCmd)) {
                    func_8009C294(hitCmd);
                    break;
                }
                if (func_800997E8(&objMarker) || func_800B99A4(&objMarker)) {
                    func_8009D44C(objMarker);
                    break;
                }
            }
        }

        if (D_800C4FC4 <= 0) {
            func_800BE9F8(func_800B1BCC(D_800D23D8));
            func_800B56A0();

            padBank = D_800C4D04;
            if ((D_800D2278[padBank] & (D_800D2278[padBank] ^ D_800D2278[(padBank + 1) % 2]) & WORLD_PAD_HANDOFF) != 0) {
                if (((u32)D_800C4D38 < 0xA || D_800C4D38 == 0x80) &&
                    D_800C4D88 == -1 && D_800D23D8[0] == 0) {
                    func_8009D3F4();
                    func_8009D954();
                    break;
                }

                padBank = D_800C4D04;
                if ((D_800D2278[padBank] & (D_800D2278[padBank] ^ D_800D2278[(padBank + 1) % 2]) & WORLD_PAD_HANDOFF) != 0) {
                    mode = D_800C4D38;
                    if ((mode == 0x30 || mode == 0x32) &&
                        D_800C4D88 == -1 && D_800D23D8[0] == 0) {
                        func_8009C294(mode == 0x32 ? 0x2C : 0x41);
                        func_8009D954();
                        break;
                    }
                }
            }

            if (D_800C9ED0 == 0x90F) {
                stall++;
            } else {
                stall = 0;
            }
            if (stall >= 2) {
                func_8009D630();
                result = 1;
                func_8009D954();
                break;
            }

            padBank = D_800C4D04;
            if ((D_800D2278[padBank] & (D_800D2278[padBank] ^ D_800D2278[(padBank + 1) % 2]) & WORLD_PAD_REDRAW) != 0 &&
                D_800C4D84 == 0 && D_800C4D88 == -1) {
                setupWorldRender();
            }
        }

        cmd = D_800C4D38;
        if ((u32)cmd < 0xA || cmd == 0x80 || (u32)(cmd - 0x20) < 9 ||
            cmd == 0x84 || cmd == 0x31) {
            func_800C4AE4(D_800C4D50 / 8);
        }

        func_800A63F0();
        func_800A7590(D_800D244C);
        func_800A735C(D_800D244C);
        func_800A6A74(D_800D244C);

        if (D_800C4D28 == MAP_DISPLAY_GLOBE) {
            SP_SAVE_SCRATCH(getScratchAddr(255));
            func_800AB540(MAP_DISPLAY_GLOBE_X, MAP_DISPLAY_GLOBE_Y, (u32)(D_800D23D8[0] - 1) >= 4);
            func_800A9F54(&D_800C9868, MAP_DISPLAY_GLOBE_X, MAP_DISPLAY_GLOBE_Y);
            SP_RESTORE();
        } else if (D_800C4D28 == MAP_DISPLAY_TRANSPARENT) {
            transparentTile = func_800A5DC8(D_800C9868.vx, D_800C9868.vy);
            mapX = D_800C97EA - MAP_DISPLAY_TRANSPARENT_DX;
            mapY = D_800C97E8 - MAP_DISPLAY_TRANSPARENT_DY;
            func_800AB540(transparentTile % MAP_DISPLAY_COLS + mapX, transparentTile / MAP_DISPLAY_COLS + mapY, (u32)(D_800D23D8[0] - 1) >= 4);
            func_8009C8E0(mapX, mapY, 1, 1);
        } else if (D_800C4D28 == MAP_DISPLAY_FULL) {
            fullMapTile = func_800A5DC8(D_800C9868.vx, D_800C9868.vy);
            mapX = (D_800C97EA - MAP_DISPLAY_FULL_W) >> 1;
            mapY = (D_800C97E8 - MAP_DISPLAY_FULL_H) >> 1;
            func_800AB540((fullMapTile % MAP_DISPLAY_COLS) * 2 + mapX, (fullMapTile / MAP_DISPLAY_COLS) * 2 + mapY, 1);
            func_8009B954(mapX, mapY, 0);
            func_8009C8E0(mapX, mapY, 2, 0);
        }

        if (D_800D23D8[0] != 0xC) {
            func_800A8400();
        }
        firstPass = 0;
        func_80099F78();
        func_800AD690();
        func_8009B748();
        renderAndUpdateDisplay(D_800D2264);
        renderBattleDisplayList(&D_800D244C->primList[BSC_COLORTAG_IDX]);
    }

    func_800A9E24(D_800D244C);
    func_8009C6CC(30, 0);
    func_800C4450();
    flushCdAndWait();
    func_800A6358();
    deactivateBattleCmd(-1);
    func_80027448();
    DrawSync(0);
    ResetGraph(3);
    func_8009AD3C();

    return result;
}
