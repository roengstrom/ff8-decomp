#include "common.h"
#include "283B0.h"
#include "battle.h"
#include "gamestate.h"
#include "intro.h"
#include "cdrom.h"
#include "snd_init.h"
#include "thread.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libcd.h"

/* s32 view of thread.c's u16 getPadReadButtons; see thread.h. */
extern s32 getPadReadButtons(s32 slot, s32 sub);

/**
 * @brief Display-init overlay entry point — set up display + run 60-frame intro.
 *
 * Boot-time setup for the intro sequence (Square publisher attribution / opening movie
 * preface) before the main menu. Steps:
 *  - Ring the sound side (@c sndCmdF0/F1), stop any GPU drawing in progress
 *    (@c BreakDraw, retried every VSync while it cannot) and, if a list was
 *    being drawn, wait for the GPU to go idle (@c IsIdleGPU).
 *  - Disable the display, sync the CD module to a known idle state, and
 *    issue @c CdControlB(0xE, ...) — CD command 0xE is @c CdlSetmode with
 *    param @c 0x80 (auto-pause + double-speed reporting).
 *  - Set up a 640x480 interlaced display: clear VRAM, build the
 *    @c DRAWENV / @c DISPENV in @c g_introDispCtx, mark dfe on the draw env
 *    and @c isinter on the disp env, install both with @c PutDrawEnv /
 *    @c PutDispEnv. Clear the active OT entry.
 *  - Run the per-frame intro tick @c func_8009818C 60 times (one second
 *    at 60 Hz).
 */
void initIntroOverlay(void) {
    RECT clearRect;
    u32 *status;
    s32 i;

    sndCmdF0();
    sndCmdF1();

    while ((status = BreakDraw()) == (u32 *)-1) {
        VSync(0);
    }
    if (status != NULL) {
        while (IsIdleGPU(1) != 0) {}
    }

    SetDispMask(0);
    CdSync(0, 0);
    CdControlB(0xE, (u8 *)0x80, 0);
    VSync(4);

    clearRect.x = 0;
    clearRect.y = 0;
    clearRect.w = 640;
    clearRect.h = 480;
    ClearImage(&clearRect, 0, 0, 0);
    DrawSync(0);

    SetDefDispEnv(&g_introDispCtx.disp, 0, 0, 640, 480);
    SetDefDrawEnv(&g_introDispCtx.draw, 0, 0, 640, 480);

    g_introDispCtx.disp.isinter = 1;
    g_introDispCtx.draw.dfe = 1;
    PutDispEnv(&g_introDispCtx.disp);
    PutDrawEnv(&g_introDispCtx.draw);
    g_introDispCtx.currBuf = 0;
    g_introDispCtx.unkB0 = 0;
    g_introDispCtx.unkB1 = 0;
    g_introRenderEnable = 0;
    VSync(0);
    g_introOdeLatch = ((u32)GetODE() < 1);
    ClearOTagR(&g_introDispCtx.ot[g_introDispCtx.currBuf], 1);
    for (i = 0; i < 60; i++) {
        func_8009818C();
    }
}

/**
 * @brief One frame of the intro display loop.
 *
 * Called 60 times in a row from @c initIntroOverlay. Per-frame steps:
 *  - @c DrawSync(0) + @c VSync(0): wait for GPU idle and vblank.
 *  - Spin until @c GetODE flips: the busy-wait re-arms the previous
 *    drawing pass.
 *  - If @c g_introRenderEnable is set, dispatch on @c g_introRenderMode to one of the
 *    intro-stage renderers:
 *      - state 0: @c func_80098378 with a full-width 640x400 mode.
 *      - state 1: @c func_80098378 with the cropped 580x406 layout.
 *      - state 2: @c func_800985EC (VRAM scanline copy).
 *  - Submit the current buffer's OT via @c DrawOTag, flip @c currBuf,
 *    install the next frame's @c DRAWENV / @c DISPENV pair, and clear
 *    the newly-active OT for the next pass.
 *  - Sample the gamepads (@c getPadReadButtons slots 0 and 1) and
 *    update the edge-detect mirrors at @c g_introCtrl0Edge (slot 0 rising
 *    edges) and @c g_introCtrl1Edge (slot 1 rising edges) using the
 *    @c g_introCtrl0Inv / @c g_introCtrl1Inv complements latched last frame.
 */
void func_8009818C(void) {
    DrawSync(0);
    VSync(0);

    while (g_introOdeLatch == ((u32)GetODE() < 1)) {}

    g_introOdeLatch = ((u32)GetODE() < 1);

    if (g_introRenderEnable != 0) {
        switch (g_introRenderMode) {
        case 0:
            func_80098378(0, 0, 40, 640, 400);
            break;
        case 1:
            func_80098378(1, 30, 38, 580, 406);
            break;
        case 2:
            func_800985EC();
            break;
        }
    }

    DrawOTag(&g_introDispCtx.ot[g_introDispCtx.currBuf]);
    g_introDispCtx.currBuf = g_introDispCtx.currBuf ^ 1;
    PutDispEnv(&g_introDispCtx.disp);
    PutDrawEnv(&g_introDispCtx.draw);
    ClearOTagR(&g_introDispCtx.ot[g_introDispCtx.currBuf], 1);

    func_800275D4();
    g_introCtrl0 = getPadReadButtons(0, 0);
    g_introCtrl1 = getPadReadButtons(1, 0);

    g_introCtrl0Edge = g_introCtrl0Inv & g_introCtrl0;
    g_introCtrl1Edge = g_introCtrl1Inv & g_introCtrl1;
    g_introCtrl1Inv = ~g_introCtrl1;
    g_introCtrl0Inv = ~g_introCtrl0;
}

/**
 * @brief Kick off an asynchronous CD load for an intro asset.
 *
 * @c g_introAssetTable is a table of (sector, size) pairs (8 bytes per entry,
 * laid out as @c u32 sector then @c u32 size). For each story-stage
 * index used by the intro sequence (e.g. @c 35 Square publisher attribution,
 * @c 4 Squaresoft logo, @c 5..31 SeeD application text pages, @c 32 "The
 * End", @c 33 FF8 logo), the entry is fetched and queued via
 * @c cdReadAsyncSync into VRAM-adjacent main RAM at @c 0x80100000.
 *
 * @param stage Asset table index — selects which (sector, size) pair
 *              from @c g_introAssetTable.
 */
void loadIntroSlide(s32 stage) {
    /* Each table entry is 2 u32s (sector, size); index by stage * 2. */
    cdReadAsyncSync(g_introAssetTable[stage * 2],
                    g_introAssetTable[stage * 2 + 1],
                    (s32)g_introStagedFrame, 0);
}

/**
 * @brief Stream a sub-rectangle of decoded video into VRAM scanline-by-scanline.
 *
 * Generalized version of @c func_800985EC. Pulls raw 16-bpp pixel data from
 * the staged buffer at @c 0x80100008 (the @c +8 skips a small header in the
 * decoded asset). Each call uploads @p height rows of @p width pixels into
 * VRAM starting at @c (x, (u16)g_introOdeLatch + y), advancing by 2 lines per
 * @c LoadImage call (interlaced — odd lines belong to the other field and
 * are filled in by the alternating frame buffer; @c g_introOdeLatch provides
 * the parity offset).
 *
 * Mode 0 (called by @c func_8009818C): full 640x400 area at (0, 40).
 * Mode 1: cropped 580x406 layout at (30, 38).
 *
 * @param mode    Stage layout selector (passed by caller; not read here).
 * @param x       Destination VRAM x.
 * @param y       Vertical offset added to @c g_introOdeLatch for the first line.
 * @param width   Pixels per row (also row stride / 4 in the source buffer).
 * @param height  Total vertical extent; loop runs while @c y < y + height.
 */
void func_80098378(s32 mode, s32 x, s32 y, s32 width, s32 height) {
    RECT rect;
    s32 val;
    s16 curY;
    u8 *src;

    val = g_introOdeLatch * 2;
    rect.x = x;
    rect.w = width;
    rect.h = 1;
    curY = (u16)g_introOdeLatch + y;
    rect.y = curY;
    src = (u8 *)g_introStagedFrame->pixels + (((u32)(width * val)) >> 2 << 2);

    if ((s16)curY < y + height) {
        do {
            LoadImage(&rect, (u32 *)src);
            curY = rect.y + 2;
            rect.y = curY;
            src += width * 4;
        } while ((s16)curY < y + height);
    }
}

/**
 * @brief Queue a semi-transparent fade overlay onto the active buffer's OT.
 *
 * Builds two primitives per call into @c g_introDispCtx's per-buffer slots and
 * adds them to the active buffer's OT:
 *  - A @c TILE primitive at @c tiles[currBuf]: a flat-shaded rectangle
 *    covering @p rect, filled with grey @p brightness on all channels
 *    (r0=g0=b0=brightness), with semi-transparency enabled. This is what
 *    draws the fade — the GPU's semi-trans mode (set via @c mode) blends
 *    it with the underlying scene.
 *  - A @c DR_TPAGE primitive at @c drTPages[currBuf]: switches the GPU
 *    semi-trans operator to @p mode (1 = add, 2 = subtract, etc.).
 *
 * @param brightness Grey level (0..255) applied to all three colour channels.
 * @param mode       Semi-trans blend mode forwarded to @c GetTPage.
 * @param rect       Destination rectangle covered by the fade tile.
 */
void func_80098440(s32 brightness, s32 mode, RECT *rect) {
    DispCtx  *dc = &g_introDispCtx;
    TILE     *tiles  = dc->tiles;
    DR_TPAGE *tpages;

    SetTile(&tiles[dc->currBuf]);
    SetSemiTrans(&tiles[dc->currBuf], 1);
    tiles[dc->currBuf].r0 = brightness;
    tiles[dc->currBuf].g0 = brightness;
    tiles[dc->currBuf].b0 = brightness;
    tiles[dc->currBuf].x0 = rect->x;
    tiles[dc->currBuf].y0 = rect->y;
    tiles[dc->currBuf].w  = rect->w;
    tiles[dc->currBuf].h  = rect->h;
    AddPrim(&dc->ot[dc->currBuf], &tiles[dc->currBuf]);

    tpages = dc->drTPages;
    SetDrawTPage(&tpages[dc->currBuf], 0, 0, GetTPage(0, mode, 0, 0));
    AddPrim(&dc->ot[dc->currBuf], &tpages[dc->currBuf]);
}

/**
 * @brief Pre-load the "CAUTION WRONG DISC" warning frame.
 *
 * Reads the @c INTRO_ASSET_WRONG_DISC entry from @c g_introAssetTable
 * (a 580x406 LZSS-compressed frame) into staging RAM at @c 0x8017D000
 * so that @c func_800985EC can later display it when @c func_8009869C
 * triggers the disc-mismatch flash. Called once at the start of
 * @c waitForCorrectDisc, right after the "insert disc N" prompt is loaded.
 */
void loadWrongDiscWarning(void) {
    cdReadAsyncSync(g_introAssetTable[INTRO_ASSET_WRONG_DISC * 2],
                    g_introAssetTable[INTRO_ASSET_WRONG_DISC * 2 + 1],
                    (s32)0x8017D000, 0);
}

/**
 * @brief Upload "CAUTION WRONG DISC" scanlines into VRAM (mode-2 renderer).
 *
 * Reads from the pre-loaded @c g_wrongDiscFrame staged at @c 0x8017D000,
 * starting at the field-parity row (@c g_introOdeLatch is 0 or 1), and
 * pushes every other line into VRAM via @c LoadImage. The complementary
 * field comes from the previous frame buffer thanks to the GPU's interlaced
 * display mode (see @c initIntroOverlay's @c isinter setup).
 *
 * Geometry matches the 580x406 source: target rect (x=30, w=580, h=1)
 * sweeps from @c y=g_introOdeLatch+38 upward by 2 until @c y >= 444.
 */
void func_800985EC(void) {
    RECT rect;
    s32 field;
    s16 y;
    u16 *row;

    field = g_introOdeLatch;
    rect.x = 30;
    rect.w = 580;
    rect.h = 1;
    y = (u16)g_introOdeLatch + 38;
    rect.y = y;
    row = &g_wrongDiscFrame->pixels[field * WRONG_DISC_FRAME_W];

    if ((s16)y < 444) {
        do {
            LoadImage(&rect, (u32 *)row);
            y = rect.y + 2;
            rect.y = y;
            row += 2 * WRONG_DISC_FRAME_W;   /* advance two lines (interlace) */
        } while ((s16)y < 444);
    }
}

/**
 * @brief Perform a display fade-out and fade-in transition.
 *
 * Fades display brightness from 255 down to 0 (by 4 each step),
 * waits 300 frames, then fades back up from 0 to 255. Updates
 * g_introRenderMode state flag and g_introOdeLatch/g_introRenderEnable during the transition.
 */
void func_8009869C(void) {
    RECT rect;
    s32 brightness;
    s32 i;

    g_introRenderMode = 2;
    VSync(0);

    i = GetODE();
    brightness = 0xFF;
    g_introOdeLatch = (u32)i < 1;

    rect.x = 30;
    rect.y = 38;
    rect.w = 580;
    rect.h = 406;

    /* Fade out: brightness 255 -> 0 by 4 */
    do {
        func_80098440(brightness, 2, &rect);
        brightness -= 4;
        func_8009818C();
        g_introRenderEnable = 1;
        SetDispMask(1);
    } while (brightness >= 0);

    /* Wait 300 frames */
    i = 0;
    do {
        i++;
        func_8009818C();
    } while (i < 300);

    /* Fade in: brightness 0 -> 255 by 4 */
    i = 0;
    do {
        func_80098440(i, 2, &rect);
        i += 4;
        func_8009818C();
    } while (i < 0xFF);

    SetDispMask(0);
    g_introRenderMode = 1;
}

/**
 * @brief Disc-swap intro screen: wait for the right disc to be inserted.
 *
 * Played when @c func_80098FD4 is invoked with mode 1 — typically when
 * loading a save from a different disc than the one inserted. Loops:
 *  - Loads the prompt graphic for the *currently inserted* disc minus one
 *    (entry @c expectedDiscId-1 in @c g_introAssetTable) plus its music track.
 *  - Waits for @c func_800393C8 to drain, then runs a fade-out / wait-
 *    for-tray-open / fade-in / fade-out cycle controlled by
 *    @c CdControlB(0x1, NULL, status) (CD command 0x1 = CdlNop, whose
 *    status byte's @c 0x10 bit indicates the shell is open).
 *  - After fade-in, polls @c getDiscId() and exits when the inserted
 *    disc matches @c g_fieldVars->expectedDiscId.
 *  - On mismatch (or if @c func_80038A60 returns nonzero), calls
 *    @c func_8009869C to flash the screen and restarts the wait loop.
 *  - Any button press on gamepad 1 (high nibble of @c g_introCtrl0Edge)
 *    short-circuits the tray wait and jumps straight to the fade-in.
 */
void waitForCorrectDisc(void) {
    u8   cdStatus[8];
    RECT rect;

    initIntroOverlay();
    loadIntroSlide(g_fieldVars->expectedDiscId - 1);
    loadWrongDiscWarning();

    rect.x = 30;
    rect.y = 38;
    rect.w = 580;
    rect.h = 406;

    while (func_800393C8() != 0) {}

    while (1) {
        s32 brightness;

        resetCdDriveMode();
        VSync(0);
        g_introOdeLatch = ((u32)GetODE() < 1);

        for (brightness = 0xFF; brightness >= 0; brightness -= 4) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            g_introRenderEnable = 1;
            SetDispMask(1);
        }

        do {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto fadein;
            CdControlB(1, 0, cdStatus);
        } while (!(cdStatus[0] & 0x10));

        do {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto fadein;
            CdControlB(1, 0, cdStatus);
        } while (cdStatus[0] & 0x10);

    fadein:
        for (brightness = 0; brightness < 0xFF; brightness += 4) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
        }
        SetDispMask(0);

        if (func_80038A60() == 0 && getDiscId() == g_fieldVars->expectedDiscId) {
            return;
        }
        func_8009869C();
    }
}

/**
 * @brief Boot-time FF8 startup intro sequence.
 *
 * Plays the boot-time intro shown when no save is loaded — Square publisher attribution,
 * Squaresoft logo, then 27 intro slides (stages 5-31, 27
 * pages of text rendered by @c loadIntroSlide), followed by "The End"
 * (stage 32), and the "Final Fantasy VIII" title-screen logo (stage 33 with music fade-out). Any button press on gamepad 1
 * (@c g_introCtrl0Edge high nibble) skips to the cleanup tail.
 *
 * Each segment fades in / holds / fades out via @c func_80098440 (the
 * fade primitive — modes 1 and 2 control how brightness is applied) over
 * a 640x400 (rectY=40, rectW=640, rectH=400) draw area. The per-frame
 * tick @c func_8009818C drives display flipping and input sampling.
 *
 * SeeD sound loading: waits for @c g_fieldVars->soundLoadComplete before
 * starting music (@c sndCmd10(toggleSoundBank()), @c sndCmdC0(0, 0x7F)).
 * On exit, @c sndCmdC1(D_8005F11C, 0x60, 0) fades the music out and a
 * final brightness ramp restores the display to full before returning.
 */
void playBootIntro(void) {
    RECT rect;
    s32 outerCount;
    s32 stage;
    s32 brightness;
    s32 holdFrames;
    s32 rectY, rectW, rectH;
    s32 frame;
    s32 one;

    initIntroOverlay();
    rectY = 40;
    rectW = 640;
    rectH = 400;

    while (1) {
        loadIntroSlide(35);
        rect.x = 0; rect.y = rectY; rect.w = rectW; rect.h = rectH;

        for (brightness = 0xFF, one = 1; brightness >= 0; brightness -= 4) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            g_introRenderEnable = one;
            SetDispMask(1);
        }

        for (frame = 0; frame < 120; frame++) {
            func_8009818C();
        }

        for (frame = 0; frame < 120; frame++) {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        for (brightness = 0; brightness < 0xFF; brightness += 4) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        loadIntroSlide(4);
        rect.x = 0; rect.y = rectY; rect.w = rectW; rect.h = rectH;

        for (brightness = 0xFF, one = 1; brightness >= 0; brightness -= 4) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            g_introRenderEnable = one;
            SetDispMask(1);
        }

        for (frame = 0; frame < 120; frame++) {
            func_8009818C();
        }

        for (frame = 0; frame < 120; frame++) {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        for (brightness = 0; brightness < 0xFF; brightness += 4) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        func_80037FB0(0, 0x4F, 0x80110000);
        if (g_fieldVars->soundLoadComplete == 0) {
            do {
                func_800393C8();
            } while (g_fieldVars->soundLoadComplete == 0);
        }

        D_8005F11C = sndCmd10(toggleSoundBank());
        sndCmdC0(0, 0x7F);

        for (frame = 0; frame < 180; frame++) {
            func_80098440(0xFF, 2, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        outerCount = 0;
        g_introVSyncBase = VSync(-1);

        for (stage = 5; stage < 32; stage++, outerCount++) {
            loadIntroSlide(stage);
            rect.x = 0; rect.y = rectY; rect.w = rectW; rect.h = rectH;

            for (brightness = 0xFF; brightness >= 0; brightness -= 8) {
                func_80098440(brightness, 2, &rect);
                func_8009818C();
                if (g_introCtrl0Edge & 0xF0) goto exit;
            }

            holdFrames = (stage - 4) * 435;
            while ((u32)(VSync(-1) - g_introVSyncBase) < (u32)holdFrames) {
                func_8009818C();
                if (g_introCtrl0Edge & 0xF0) goto exit;
            }

            for (brightness = 0; brightness < 0xFF; brightness += 8) {
                func_80098440(brightness, 2, &rect);
                func_8009818C();
                if (g_introCtrl0Edge & 0xF0) goto exit;
            }
        }

        loadIntroSlide(32);
        rect.x = 0; rect.y = rectY; rect.w = rectW; rect.h = rectH;

        for (brightness = 0xFF; brightness >= 0; brightness -= 8) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        holdFrames = (outerCount + 1) * 435;
        while ((u32)(VSync(-1) - g_introVSyncBase) < (u32)holdFrames) {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        for (brightness = 0; brightness < 0xFF; brightness += 4) {
            func_80098440(brightness, 1, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        for (frame = 0; frame < 120; frame++) {
            func_80098440(0xFF, 1, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        loadIntroSlide(33);
        rect.x = 0; rect.y = rectY; rect.w = rectW; rect.h = rectH;

        for (brightness = 0xFF; brightness >= 0; brightness -= 2) {
            func_80098440(brightness, 1, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        while (sndGetStatus() != 0) {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        for (frame = 0; frame < 3600; frame++) {
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        for (brightness = 0; brightness < 0xFF; brightness++) {
            func_80098440(brightness, 2, &rect);
            func_8009818C();
            if (g_introCtrl0Edge & 0xF0) goto exit;
        }

        SetDispMask(0);
    }

exit:
    sndCmdC1(D_8005F11C, 0x60, 0);

    if (brightness >= 256) {
        brightness = 0xFF;
    }
    if (brightness < 0) {
        brightness = 0;
    }

    for (; brightness < 0xFF; brightness += 2) {
        func_80098440(brightness, 2, &rect);
        func_8009818C();
    }
}

/**
 * @brief Display initialization entry point with mode selection.
 *
 * Configures display buffers, selects a display mode (0 or 1), runs
 * the corresponding intro sequence, then resets the display back.
 *
 * @param mode 0 for the standard intro sequence (playBootIntro),
 *             1 for the alternate intro sequence (waitForCorrectDisc).
 */
void func_80098FD4(s32 mode) {
    RECT rect;

    ResetGraph(1);

    rect.x = 256;
    rect.y = 224;
    rect.w = 64;
    rect.h = 32;
    MoveImage(&rect, 0, 480);
    DrawSync(0);
    VSync(0);

    switch (mode) {
    case 0:
        g_introRenderMode = 0;
        playBootIntro();
        break;
    case 1:
        g_introRenderMode = mode;
        waitForCorrectDisc();
        break;
    }

    SetDispMask(0);

    rect.y = 480;
    rect.w = 64;
    rect.x = 0;
    rect.h = 32;
    MoveImage(&rect, 256, 224);
    DrawSync(0);
    VSync(0);
}
