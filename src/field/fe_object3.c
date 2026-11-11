#include "common.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "gamestate.h"
#include "input/gamepad.h"
#include "input/vibration.h"
#include "ui/icon.h"
#include "ui/window.h"
#include "field/fe_object3.h"

extern u8 g_unpauseVibrationPattern[];

static inline void RunFieldPauseScreen(void (*callback)(void)) {
    DRAWENV view;
    DRAWENV save;
    RECT pauseWindow;
    RECT vibrationSettingBoxDivider;
    u32 ot[0x202];
    s32 tick;
    s32 vibrationMask;
    s32 controllerSupportsVibration;
    u16 newlyPressedButtons;
    s32 clut;
    void *prim;
    s32 y;
    s32 vibrationStateIcon;
    s32 neutralColor;
    s32 vibrationEnabledMask;
    DISPENV *disp;

    neutralColor = 0x64808080;
    tick = 0;
    RequestActuatorSetup(0);
    DrawSync(0);
    VSync(0);

    SetDefDrawEnv(&view, g_activeDispEnv->disp.x, g_activeDispEnv->disp.y, g_activeDispEnv->disp.w, g_activeDispEnv->disp.h);
    view.isbg = 0;
    GetDrawEnv(&save);
    PutDrawEnv(&view);

    disp = g_activeDispEnv;
    pauseWindow.x = (disp->disp.w - 128) / 2 - 1;
    pauseWindow.y = (disp->disp.h - 22) / 2 - 1;
    pauseWindow.w = 128;
    vibrationSettingBoxDivider.y = pauseWindow.y;
    pauseWindow.h = 22;
    vibrationSettingBoxDivider.x = pauseWindow.x + 95;
    vibrationSettingBoxDivider.w = 8;
    vibrationSettingBoxDivider.h = 22;

    IsVibrationEnabled(0);
    SetVibrationIntensity(0, 0, 0);
    SetPlayTimePaused(1);

    vibrationEnabledMask = GAMEPAD_VIBRATION_ENABLED_MASK;
    while (1) {
        DrawSync(0);
        ClearOTag(ot, 2);
        prim = &ot[2];
        RecordGamePadInput();
        newlyPressedButtons = GetNewlyPressedButtons(0, 0);
        if (newlyPressedButtons & PADstart) {
            // Unpause
            break;
        }

        controllerSupportsVibration = 1;
        if (!IsFirstGamePadConnected() || !GetActuatorCountForGamePad(0)) {
            SetVibrationEnabled(0, GAMEPAD_VIBRATION_DISABLED_MASK);
            controllerSupportsVibration = 0;
        }
        vibrationMask = IsVibrationEnabled(0);
        // Toggle vibration ON/OFF when any of below buttons were pressed
        if ((newlyPressedButtons & (PADLup | PADLright | PADLdown | PADLleft | PADRright | PADRdown)) && controllerSupportsVibration) {
            if (vibrationMask == vibrationEnabledMask) {
                vibrationMask = GAMEPAD_VIBRATION_DISABLED_MASK;
            } else {
                vibrationMask = GAMEPAD_VIBRATION_ENABLED_MASK;
            }
            SetVibrationEnabled(0, vibrationMask);
        }
        if (GetActuatorCountForGamePad(0)) {
            clut = 0;
        } else {
            clut = 0x140;
        }

        prim = drawIconClut(ot, prim, ICON_VIBRATION, pauseWindow.x + 6, y = pauseWindow.y + 8, neutralColor, clut);
        vibrationStateIcon = ICON_OFF;
        if (vibrationMask == vibrationEnabledMask) {
            vibrationStateIcon = ICON_ON;
        }
        prim = drawIconClut(ot, prim, vibrationStateIcon, pauseWindow.x + 101, y, neutralColor, clut);
        // blink pause, 16 frames on then 16 frames off
        if (!((tick >> 4) & 1)) {
            prim = drawIcon(ot, prim, ICON_PAUSE, pauseWindow.x, pauseWindow.y, neutralColor);
        }
        prim = func_8002B3A0(ot, prim, &vibrationSettingBoxDivider, neutralColor, WINDOW_FRAME_LEFT);
        prim = func_8002B898(ot, prim, &pauseWindow, neutralColor);
        func_8002B8BC(ot, prim, &pauseWindow, neutralColor, 8);

        VSync(0);
        DrawOTag(ot);
        tick++;
        if (callback != NULL) {
            callback();
        }
    }

    if (IsVibrationEnabled(0) == GAMEPAD_VIBRATION_ENABLED_MASK && !isVibrating()) {
        startVibration(g_unpauseVibrationPattern, 1, 1);
    }
    PutDrawEnv(&save);
    SetPlayTimePaused(0);
    DrawSync(0);
    VSync(0);
}

void FieldPause(void (*callback)(void)) {
    RunFieldPauseScreen(callback); // matching hack?
}

/**
 * Append one entry to the @c D_800DE7B0 command queue. Stores @p a
 * verbatim, rounds @p size up to the next @c 0x800-byte boundary, and
 * scales @p c by @c >>2 (likely a byte-to-stride or fixed-point
 * narrowing). The slot is at queue index @c count and @c count is
 * incremented after the writes.
 *
 * @param a    Identifier/tag stored at @c entries[count].unk0.
 * @param size Byte size; rounded up to @c 0x800 alignment before store.
 * @param c    Misc parameter; scaled by @c >>2 before store.
 */
void func_800ADAF4(s32 a, u32 size, u32 c) {
    size = ((size + 0x7FF) >> 11) << 11;
    c >>= 2;
    D_800DE7B0.entries[D_800DE7B0.count].unk0 = a;
    D_800DE7B0.entries[D_800DE7B0.count].unk4 = size;
    D_800DE7B0.entries[D_800DE7B0.count].unk8 = c;
    D_800DE7B0.count = D_800DE7B0.count + 1;
}
