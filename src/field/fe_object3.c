#include "common.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "gamestate.h"
#include "input/gamepad.h"
#include "input/vibration.h"
#include "ui/icon.h"
#include "ui/window.h"
#include "field/fe_object3.h"

// The original called this without a prototype in scope, so it saw the implicit int return.
extern s32 GetNewlyPressedButtons(s32 idx, s32 offset);
extern u8 D_80053B28[];

static inline void func_800AD7AC_body(void (*callback)(void)) {
    DRAWENV view;
    DRAWENV save;
    RECT frame;
    RECT bar;
    u32 ot[0x202];
    s32 tick;
    s32 enabled;
    s32 connected;
    u16 pressed;
    s32 clut;
    void *prim;
    s32 y;
    s32 iconIdx;
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
    halfW = (disp->disp.w - 0x80) / 2;
    frame.x = halfW - 1;
    frame.y = (disp->disp.h - 0x16) / 2 - 1;
    frame.w = 0x80;
    bar.y = frame.y;
    frame.h = 0x16;
    bar.x = halfW + 0x5E;
    bar.w = 8;
    bar.h = 0x16;

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

        prim = drawIconClut(ot, prim, 0xA0, frame.x + 6, y = frame.y + 8, color, clut);
        iconIdx = 0xA2;
        if (enabled == full) {
            iconIdx = 0xA1;
        }
        prim = drawIconClut(ot, prim, iconIdx, frame.x + 0x65, y, color, clut);
        if (!((tick >> 4) & 1)) {
            prim = drawIcon(ot, prim, 0x4B, frame.x, frame.y, color);
        }
        prim = func_8002B3A0(ot, prim, &bar, color, 1);
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
        startVibration(D_80053B28, 1, 1);
    }
    PutDrawEnv(&save);
    SetPlayTimePaused(0);
    DrawSync(0);
    VSync(0);
}

void func_800AD7AC(void (*callback)(void)) {
    func_800AD7AC_body(callback); // matching hack?
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
