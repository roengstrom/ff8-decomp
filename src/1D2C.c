#include "common.h"
#include "psxsdk/libgpu.h"

/** @brief VSync rendering dispatch mode. */
typedef enum {
    RENDER_IDLE    = 0,
    RENDER_FADE    = 1,
    RENDER_BATTLE  = 2,
    RENDER_OVERLAY = 3,
    RENDER_GAME    = 4
} RenderMode;

/** @brief Empty stub, called at the very start of main before initialization.
 *  @note Purpose uncertain — may be a debug hook or placeholder that was
 *        never filled in the retail build.
 */
void func_8001152C(void) {
}

/** @brief CRT0 entry point. Clears BSS, sets up GP register, and calls main. */
INCLUDE_ASM("asm/nonmatchings/1D2C", start);

/** @brief Clears the GPU ordering tables and flushes the GPU pipeline.
 *
 *  Builds a minimal OT, submits it via DrawOTag, and spins on DrawSync
 *  until rendering completes. Used to ensure the display is blank during
 *  transitions (e.g. waiting for a fade to finish).
 */

void func_800115F0(void) {
    u32 prim[4];
    u32 ot[2];

    ClearOTag(ot, 2);
    SetDrawStp(prim, 0);
    addPrim(ot, prim);
    DrawOTag(ot);

    do {
    } while (DrawSync(1) != 0);
}

/** @brief VSync callback handler, registered via VSyncCallback in InitHardware.
 *
 *  Dispatches per-frame rendering based on the game mode (g_renderMode):
 *  - 0: no action (rendering idle)
 *  - 1: calls ProcessFade (normal frame draw/swap with fade)
 *  - 2: calls func_80026D8C (battle VSync handler)
 *  - 3: calls func_800D0608 (overlay-loaded VSync handler)
 *  - 4: calls func_800205D0 (game code VSync handler)
 *
 *  Also maintains two fixed-point frame timing accumulators (D_8005F154 and
 *  D_8005F15C) that increment by 0x88F per VSync. On bit-17 rollover (~every
 *  12 frames), one increments a game frame counter at g_gameState+0xCD0
 *  and the other decrements a countdown timer at g_gameState+0xCD4.
 *
 *  @note Non-matching (3 instructions shorter). Two codegen differences:
 *        1. CC1PSX fills beqz delay slot with `addu v0,v0,-1` (countdown
 *           decrement) instead of original `li v0,1` (early-return value),
 *           eliminating a 3-instruction reload sequence.
 *        2. Switch jump table rodata must be at 0x80010000 (before asm
 *           rodata) — requires linker script reorder: move 1D2C.o(.rodata)
 *           first, add `. = ALIGN(8);` after it.
 *
 *  @code
 *  void VsyncHandler(void) {
 *      extern volatile s32 D_8005F154;
 *      extern volatile s32 D_8005F15C;
 *      extern u16 D_8005F11E;
 *      extern u8 g_gameState[];
 *
 *      switch ((s16)g_renderMode) {
 *      case 0:
 *          break;
 *      case 1:
 *          ProcessFade();
 *          break;
 *      case 2:
 *          func_80026D8C();
 *          break;
 *      case 3:
 *          func_800D0608();
 *          break;
 *      case 4:
 *          func_800205D0();
 *          break;
 *      }
 *
 *      if (func_800283CC() != 0) {
 *          D_8005F11E = 1;
 *          return;
 *      }
 *
 *      if (g_vsyncSkip != 0) {
 *          D_8005F11E = 1;
 *          return;
 *      }
 *
 *      D_8005F154 += 0x88F;
 *      if (D_8005F154 >> 17) {
 *          s32 base = g_gameState;
 *          *(s32 *)(base + 0xCD0) += 1;
 *          asm("");
 *          D_8005F154 &= 0xFFFF;
 *      }
 *
 *      D_8005F15C += 0x88F;
 *      if (D_8005F15C >> 17) {
 *          s32 base = (s32)g_gameState;
 *          if (*(s32 *)(base + 0xCD4) == 0) {
 *              D_8005F11E = 1;
 *              return;
 *          }
 *          *(s32 *)(base + 0xCD4) -= 1;
 *          asm("");
 *          D_8005F15C &= 0xFFFF;
 *      }
 *
 *      D_8005F11E = 1;
 *  }
 *  @endcode
 */
INCLUDE_ASM("asm/nonmatchings/1D2C", VsyncHandler);

void InitGeom(void);
void ResetCallback(void);
void SetMem(u8 a);
void StopCallback(void);
void VSyncCallback(void (*cb)(void));
void InitSpu(void);

extern void VsyncHandler(void);
extern u8 g_vsyncSkip;
extern volatile u16 g_renderMode;

/** @brief Initializes PS1 hardware subsystems.
 *
 *  Called once at the start of main. Sets up memory mode, resets the callback
 *  system, initializes GPU and GTE, starts SPU via InitSpu (SpuInit
 *  wrapper), registers the VSync callback, and disables display output until
 *  the game is ready to show content.
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
