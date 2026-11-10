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
    RECT frame;
    RECT valueBox;
    u32 ot[0x202];
    s32 tick;
    s32 enabled;
    s32 connected;
    u16 pressed;
    s32 clut;
    void *prim;
    s32 y;
    s32 stateIcon;
    s32 color;
    s32 full;
    s32 halfW;
    DISPENV *disp;

    color = 0x64808080;
    tick = 0;
    RequestActuatorSetup(0);
    DrawSync(0);
    VSync(0);

    SetDefDrawEnv(&view, g_activeDispEnv->disp.x, g_activeDispEnv->disp.y, g_activeDispEnv->disp.w, g_activeDispEnv->disp.h);
    view.isbg = 0;
    GetDrawEnv(&save);
    PutDrawEnv(&view);

    disp = g_activeDispEnv;
    halfW = (disp->disp.w - 128) / 2;
    frame.x = halfW - 1;
    frame.y = (disp->disp.h - 22) / 2 - 1;
    frame.w = 128;
    valueBox.y = frame.y;
    frame.h = 22;
    valueBox.x = halfW + 94;
    valueBox.w = 8;
    valueBox.h = 22;

    IsVibrationEnabled(0);
    SetVibrationIntensity(0, 0, 0);
    SetPlayTimePaused(1);

    full = 0xFF;
    while (1) {
        DrawSync(0);
        ClearOTag(ot, 2);
        prim = &ot[2];
        RecordGamePadInput();
        pressed = GetNewlyPressedButtons(0, 0);
        if (pressed & PADstart) {
            break;
        }

        connected = 1;
        if (!IsFirstGamePadConnected() || !GetActuatorCountForGamePad(0)) {
            SetVibrationEnabled(0, 0);
            connected = 0;
        }
        enabled = IsVibrationEnabled(0);
        if ((pressed & (PADLup | PADLright | PADLdown | PADLleft | PADRright | PADRdown)) && connected) {
            if (enabled == full) {
                enabled = 0;
            } else {
                enabled = 0xFF;
            }
            SetVibrationEnabled(0, enabled);
        }
        if (GetActuatorCountForGamePad(0)) {
            clut = 0;
        } else {
            clut = 0x140;
        }

        prim = drawIconClut(ot, prim, ICON_VIBRATION, frame.x + 6, y = frame.y + 8, color, clut);
        stateIcon = ICON_OFF;
        if (enabled == full) {
            stateIcon = ICON_ON;
        }
        prim = drawIconClut(ot, prim, stateIcon, frame.x + 101, y, color, clut);
        if (!((tick >> 4) & 1)) {
            prim = drawIcon(ot, prim, ICON_PAUSE, frame.x, frame.y, color);
        }
        prim = func_8002B3A0(ot, prim, &valueBox, color, WINDOW_FRAME_LEFT);
        prim = func_8002B898(ot, prim, &frame, color);
        func_8002B8BC(ot, prim, &frame, color, 8);

        VSync(0);
        DrawOTag(ot);
        tick++;
        if (callback == NULL) {
            continue;
        }
        callback();
    }

    if (IsVibrationEnabled(0) == 0xFF && !isVibrating()) {
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
