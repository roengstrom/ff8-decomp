#include "common.h"
#include "psxsdk/libpad.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/kernel.h"
#include "psxsdk/libapi.h"
#include "psxsdk/libetc.h"
#include "psxsdk/r3000.h"
#include "battle.h"
#include "thread.h"

static s32 func_80027038(EngineState *a0, GamePad *port, u16 a2, u16 a3, s32 a4);
static void func_800270B0(EngineState *engine, GamePad *gamePad, s32 port);
static void stepPadPort(EngineState *engine, GamePad *gamePad, s32 port);
static void stepPadPorts(EngineState *engine);
static u16 getPadReadReleased(s32 idx, s32 offset);

/**
 * @brief Open a thread with interrupt protection.
 * @param entry Function the thread starts in.
 * @param stack Top of the thread's stack.
 * @return Thread handle from OpenTh.
 * @note Wraps PsyQ OpenTh in EnterCriticalSection and ExitCriticalSection.
 */
s32 openThreadSafe(void (*entry)(void), u8 *stack) {
    s32 result;
    EnterCriticalSection();
    result = OpenTh(entry, stack, 0);
    ExitCriticalSection();
    return result;
}


/**
 * @brief Close a thread with interrupt protection.
 * @param a0 Thread handle to close.
 * @note Wraps PsyQ CloseTh in EnterCriticalSection and ExitCriticalSection.
 */
void closeThreadSafe(s32 a0) {
    EnterCriticalSection();
    CloseTh(a0);
    ExitCriticalSection();
}


/**
 * @brief Manually switch the current thread without a syscall.
 *
 * Resolves the thread control block for @p handle through the kernel
 * table of tables and, if that thread is active, stores it as the
 * current thread in the TCB header.
 *
 * @param handle Thread handle (low 24 bits index the TCB table).
 */
void func_80026F4C(s32 handle) {
    ToT *tot = KERNEL_TOT;
    TCB *tcbTable = (TCB *)tot[2].head;
    TCBH *tcbh = (TCBH *)tot[1].head;
    TCB *tcb = &tcbTable[handle & 0xFFFFFF];
    if (tcb->status == TcbStACTIVE) {
        tcbh->entry = tcb;
    }
}


/**
 * @brief Compute an address offset from a 24-bit color value.
 *
 * Masks the input to 24 bits, multiplies by 192, and adds to the base
 * address loaded from kernel memory at 0x110.
 *
 * @param a0 Input value (only lower 24 bits used).
 * @return Base address + masked input * 192.
 */
s32 getThreadControlBlock(s32 a0) {
    s32 base = *(s32 *)0x110;
    a0 &= 0xFFFFFF;
    return base + a0 * 192;
}


/**
 * @brief Read the CPU's status register.
 * @return The register's value.
 */
u32 getStatusRegister(void) { return GetSr(); }


/** @brief Write @p sr to the CPU's status register. */
void setStatusRegister(u32 sr) {
    __asm__ volatile("mtc0 %0, $12" : : "r"(sr));
}


/**
 * @brief Switch to a thread, using a fallback address if a0 is 0.
 * @param a0 Thread handle to switch to; 0 means the main thread (0xFF000000).
 * @note If GetSr returns bit 2 set, uses func_80026F4C instead of PsyQ ChangeTh.
 */
void switchThread(s32 a0) {
    if (a0 == 0) {
        a0 = (s32)0xFF000000;
    }
    if (GetSr() & SR_IEP) {
        func_80026F4C(a0);
    } else {
        ChangeTh(a0);
    }
}


static s32 func_80027038(EngineState *a0, GamePad *port, u16 a2, u16 a3, s32 a4) {
    s32 counter;
    s32 interval;
    u16 restartDelay;
    u16 repeatInterval;
    u16 mask;

    restartDelay = a0->repeatDelays.hword;
    mask = port->autoRepeatButtonMasks[a4];
    counter = port->autoRepeatCountdowns[a4];

    interval = restartDelay >> 8;
    restartDelay &= 0xFF;
    repeatInterval = interval;
    a3 &= mask;
    a2 &= mask;
    if (a3 & a2) {
        if (a3 != a2) {
            counter = restartDelay;
        }
        counter--;
        if (counter < 0) {
            counter = repeatInterval;
        } else {
            a3 = 0;
        }
    } else {
        counter = restartDelay;
    }
    port->autoRepeatCountdowns[a4] = counter;
    return a3;
}


static void func_800270B0(EngineState *engine, GamePad *gamePad, s32 port) {
    s32 i;
    s32 j;
    s32 info;
    s32 motor1Act = -1;
    s32 motor0Act = -1;
    s32 motor0Bits;
    s32 motor1Bits;
    u8 *align = gamePad->actuatorAlign;
    u8 *p;

    i = PadInfoAct(port, -1, 0);
    gamePad->actuatorCount = i;
    while (--i >= 0) {
        info = PadInfoAct(port, i, InfoActFunc);
        if (info == 1) {
            info = PadInfoAct(port, i, InfoActSub);
            if (info == 1) {
                motor1Act = i;
                info = PadInfoAct(port, motor1Act, InfoActSize);
                if (info == 0) {
                    motor1Bits = 1;
                } else {
                    motor1Bits = info << 3;
                }
            } else if (info == 2) {
                motor0Act = i;
                info = PadInfoAct(port, motor0Act, InfoActSize);
                if (info == 0) {
                    motor0Bits = 1;
                } else {
                    motor0Bits = info << 3;
                }
            }
        }
    }
    p = align;
    for (j = 0; j < 6; j++) {
        *p++ = 0xFF;
    }
    if (motor0Act != -1) {
        motor0Bits = 8 - motor0Bits;
        align[1] = motor0Act;
        gamePad->vibrationIntensityShift[0] = motor0Bits;
    }
    if (motor1Act != -1) {
        motor1Bits = 8 - motor1Bits;
        align[2] = motor1Act;
        gamePad->vibrationIntensityShift[1] = motor1Bits;
    }
}


/**
 * @brief Step a pad port's state machine.
 *
 * @param engine Engine reference.
 * @param gamePad Gamepad to update.
 * @param port Driver port number: 0x00 for port 0, 0x10 for port 1.
 */
static void stepPadPort(EngineState *engine, GamePad *gamePad, s32 port) {
    s32 state;
    GetSr();
    state = PadGetState(port);
    gamePad->gamePadState = state;
    switch (state) {
    case PadStateFindPad:
        gamePad->actuatorCount = 0;
        gamePad->actuatorDataChanged = 1;
        gamePad->actuatorSetupPending = 1;
        break;
    case PadStateFindCTP1:
        gamePad->actuatorCount = 0;
        gamePad->actuatorSetupPending = 1;
        gamePad->actuatorDataChanged = 1;
        gamePad->vibrationMask = 0;
        break;
    case PadStateReqInfo:
    case PadStateExecCmd:
        break;
    case PadStateStable: {
        u8 mask = gamePad->vibrationMask;
        if (gamePad->actuatorSetupPending == 1) {
            func_800270B0(engine, gamePad, port);
            gamePad->actuatorDataChanged = 1;
            gamePad->actuatorSetupPending = 0;
            gamePad->gamePadTransmitBuf[1] = 0;
            gamePad->gamePadTransmitBuf[2] = 0;
            PadSetActAlign(port, gamePad->actuatorAlign);
            PadSetAct(port, gamePad->gamePadTransmitBuf, 6);
        } else {
            u8 rightVibrationIntensity = gamePad->vibrationIntensity[0];
            u8 leftVibrationIntensity = gamePad->vibrationIntensity[1];
            u8 leftShift = gamePad->vibrationIntensityShift[1];
            u8 rightShift;
            s32 rightMasked;
            s32 leftMasked;
            do {
                gamePad->actuatorDataChanged = 1;
                rightMasked = rightVibrationIntensity & mask;
            } while (0); // matching hack
            leftMasked = leftVibrationIntensity & mask;
            rightShift = gamePad->vibrationIntensityShift[0];
            gamePad->gamePadTransmitBuf[2] = leftMasked >> leftShift;
            gamePad->gamePadTransmitBuf[1] = rightMasked >> rightShift;
        }
        break;
    }
    case PadStateDiscon:
    case PadStateFindCTP2:
    default:
        gamePad->actuatorCount = 0;
        gamePad->actuatorDataChanged = 1;
        gamePad->vibrationMask = 0;
        gamePad->actuatorSetupPending = 1;
        break;
    }
}


/**
 * @brief Check the CD stream status, restarting playback if it has ended.
 *
 * @param a0 Stream channel selector.
 * @return 1 if the stream is in state 2, the result of PadSetMainMode if
 *         it is in state 6, 0 otherwise.
 */
s32 func_80027360(s32 a0) {
    s32 status;
    s32 ret;
    status = PadGetState(a0);
    ret = 0;
    switch (status) {
    case PadStateFindCTP1:
        ret = 1;
        break;
    case PadStateStable:
        ret = PadSetMainMode(a0, 1, 1);
        break;
    case PadStateDiscon:
    case PadStateFindPad:
    case PadStateFindCTP2:
    case PadStateReqInfo:
    case PadStateExecCmd:
        break;
    }
    return ret;
}


/**
 * @brief Make stepPadPort rerun a pad port's setup on its next state-6 tick.
 * @param idx Index into g_engine.gamePads.
 */
void requestPadSetup(s32 idx) {
    GamePad *port = &g_engine.gamePads[idx];
    port->actuatorSetupPending = 1;
    port->actuatorDataChanged = 1;
}


/**
 * @brief Step both pad ports: port 0 as driver port 0x00, port 1 as 0x10.
 * @param engine Engine state holding both gamepads.
 */
static void stepPadPorts(EngineState *engine) {
    stepPadPort(engine, &engine->gamePads[0], 0);
    stepPadPort(engine, &engine->gamePads[1], 0x10);
}


void settlePadPorts(void) {
    GamePad *port = &g_engine.gamePads[0];
    EngineState *engine = &g_engine;
    s32 count;
    /* Without this barrier the register allocator gives s0 to count
       instead of port. */
    REGALLOC_BARRIER(port);
    count = 0;
    port->vibrationIntensity[0] = 0;
    port->vibrationIntensity[1] = 0;
    for (;;) {
        while (func_8003AB64() == 0) {}
        stepPadPort(engine, port, 0);
        if (port->gamePadState == PadStateDiscon || port->gamePadState == PadStateFindCTP1) {
            break;
        }
        if (port->gamePadState == PadStateStable) {
            count++;
            if (count >= 2) {
                break;
            }
        }
    }
    port = &engine->gamePads[1];
    count = 0;
    port->vibrationIntensity[0] = 0;
    port->vibrationIntensity[1] = 0;
    for (;;) {
        while (func_8003AB64() == 0) {}
        stepPadPort(engine, port, 0x10);
        if (port->gamePadState == PadStateDiscon || port->gamePadState == PadStateFindCTP1) {
            break;
        }
        if (port->gamePadState == PadStateStable) {
            count++;
            if (count >= 2) {
                break;
            }
        }
    }
}


/**
 * @brief Copy a rectangle's bounds to g_engine clip region.
 *
 * Copies x, y from @p rect to clipLeft/clipTop, and computes
 * x+w-1, y+h-1 for clipRight/clipBottom.
 *
 * @param rect Source clip rectangle.
 */
void setBattleAnimClipRect(RECT *rect) {
    g_engine.clipLeft = rect->x;
    g_engine.clipTop = rect->y;
    g_engine.clipRight = rect->x + rect->w - 1;
    g_engine.clipBottom = rect->y + rect->h - 1;
}


/**
 * @brief Read actuatorCount from a pad port.
 * @param idx Port index.
 */
s32 getPadField0B(s32 idx) {
    GamePad *port = &g_engine.gamePads[idx];
    return port->actuatorCount;
}


void func_800275D4(void) {
    EngineState *engine = &g_engine;
    GamePad *port;
    u8 *buf;
    GamePadInputFrame *frame;
    u32 sr;
    s32 i;
    s32 linked;
    s32 fc;
    u8 type;
    s32 prev;
    s32 held;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;

    stepPadPorts(engine);
    port = engine->gamePads;
    sr = GetSr();
    if (!(sr & SR_IEP)) {
        EnterCriticalSection();
    }
    for (i = 0; i < 2; i++, port++) {
        buf = engine->gamePadReceiveBuf[i];
        linked = i;
        type = buf[1] >> 4;
        fc = port->frameCounter;
        prev = port->frames[fc].currentlyPressedButtons;
        held = 0;
        if (port->gamePadState != PadStateDiscon) {
            if (buf[0] != 0) {
                held = prev;
            } else {
                held = buf[2] << 8;
                held += buf[3];
                held ^= 0xFFFF;
                if ((held & (PADLleft | PADLright)) == (PADLleft | PADLright)) {
                    held &= ~(PADLleft | PADLright);
                }
                if ((held & (PADLup | PADLdown)) == (PADLup | PADLdown)) {
                    held &= ~(PADLup | PADLdown);
                }
                if (engine->animFlag != 0 && i == 0 && held == 0) {
                    if (type == 7 || type == 5) {
                        frame = (GamePadInputFrame *)buf;
                        dx = frame->analog[0] - 0x80;
                        dy = frame->analog[1] - 0x80;
                        if (dx * dx + dy * dy < 40 * 40) {
                            dx = frame->analog[2] - 0x80;
                            dy = frame->analog[3] - 0x80;
                            if (dx * dx + dy * dy < 40 * 40) {
                                linked = 1;
                            }
                        }
                    } else {
                        linked = 1;
                    }
                }
                if (type == 1) {
                    x = (s8)buf[4];
                    y = (s8)buf[5];
                    x += engine->globalCoords[i][0];
                    y += engine->globalCoords[i][1];
                    x = (x < engine->clipLeft) ? engine->clipLeft : (x > engine->clipRight) ? engine->clipRight : x;
                    y = (y < engine->clipTop) ? engine->clipTop : (y > engine->clipBottom) ? engine->clipBottom : y;
                    held &= PADL1 | PADR1;
                    engine->globalCoords[i][0] = x;
                    engine->globalCoords[i][1] = y;
                }
            }
            if (engine->pad1E2 == 0) {
                held = 0;
                prev = 0;
            }
        }
        fc++;
        fc &= 7;
        frame = &port->frames[fc];
        ((u32 *)frame)[0] = ((u32 *)buf)[0];
        ((u32 *)frame)[1] = ((u32 *)buf)[1];
        frame->currentlyPressedButtons = held;
        frame->newlyPressedButtons = (prev ^ held) & held;
        frame->newlyReleasedButtons = (prev ^ held) & ~held;
        frame->autoRepeatedButtons[0] = func_80027038(engine, port, prev, held, 0);
        frame->autoRepeatedButtons[1] = func_80027038(engine, port, prev, held, 1);
        frame->autoRepeatedButtons[2] = func_80027038(engine, port, prev, held, 2);
        frame->autoRepeatedButtons[3] = func_80027038(engine, port, prev, held, 3);
        port->frameCounter = fc;
        port->inputSourceIndex = linked;
    }
    if (!(sr & SR_IEP)) {
        ExitCriticalSection();
    }
}


/**
 * @brief Get the held buttons of a past read on the port linked to port @p idx.
 * @param idx Port index (masked to 0 or 1).
 * @param offset How many reads back to look.
 * @return The held-button word.
 * @note The reads are a ring of eight, indexed by (frameCounter - offset) & 7.
 */
u16 getPadReadButtons(s32 idx, s32 offset) {
    GamePad *ports;
    GamePad *port;
    GamePad *linked;
    s32 sub_idx;
    idx &= 1;
    ports = g_engine.gamePads;
    port = ports + idx;
    linked = ports + port->inputSourceIndex;
    sub_idx = (linked->frameCounter - offset) & 7;
    return (linked->frames + sub_idx)->currentlyPressedButtons;
}


/**
 * @brief Get the auto-repeated buttons of a past read on the port linked to port @p idx.
 * @param idx Port index (masked to 0 or 1).
 * @param offset How many reads back to look.
 * @return The four auto-repeat channels' bits (autoRepeatedButtons), ORed.
 */
u16 getPadReadRepeat(s32 idx, s32 offset) {
    GamePad *ports;
    GamePad *port;
    GamePad *linked;
    s32 sub_idx;
    GamePadInputFrame *frame;
    idx &= 1;
    ports = g_engine.gamePads;
    port = ports + idx;
    linked = ports + port->inputSourceIndex;
    sub_idx = (linked->frameCounter - offset) & 7;
    frame = &linked->frames[sub_idx];
    return frame->autoRepeatedButtons[0] | frame->autoRepeatedButtons[1] | frame->autoRepeatedButtons[2] | frame->autoRepeatedButtons[3];
}


/**
 * @brief Get the buttons newly pressed in a past read on the port linked to port @p idx.
 * @param idx Port index (only bit 0 used).
 * @param offset How many reads back to look.
 * @return field10: the buttons held in that read but not in the one before.
 */
u16 getPadReadPressed(s32 idx, s32 offset) {
    GamePad *ports;
    GamePad *port;
    GamePad *linked;
    s32 sub_idx;
    idx &= 1;
    ports = g_engine.gamePads;
    port = ports + idx;
    linked = ports + port->inputSourceIndex;
    sub_idx = (linked->frameCounter - offset) & 7;
    return (linked->frames + sub_idx)->newlyPressedButtons;
}


/**
 * @brief Get the buttons newly released in a past read on the port linked to port @p idx.
 * @param idx Port index (only bit 0 used).
 * @param offset How many reads back to look.
 * @return field12: the buttons held in the read before but not in that one.
 */
static u16 getPadReadReleased(s32 idx, s32 offset) {
    GamePad *ports;
    GamePad *port;
    GamePad *linked;
    s32 sub_idx;
    idx &= 1;
    ports = g_engine.gamePads;
    port = ports + idx;
    linked = ports + port->inputSourceIndex;
    sub_idx = (linked->frameCounter - offset) & 7;
    return (linked->frames + sub_idx)->newlyReleasedButtons;
}


/**
 * @brief Compute angle from (a0, a1) with special-case axis handling.
 *
 * Returns a fixed angle when either component is zero:
 * - (0, <=0): 0xC00 (270 degrees)
 * - (0, >0):  0x400 (90 degrees)
 * - (<0, 0):  0x800 (180 degrees)
 * - (>0, 0):  0x000 (0 degrees)
 * Otherwise delegates to ratan2.
 *
 * @param a0 X component (horizontal).
 * @param a1 Y component (vertical).
 * @return Angle in fixed-point (0x1000 = 360 degrees).
 */
// Original devs possibly forgot to make it static
inline s32 computeAngle(s32 a0, s32 a1) {
    s32 angle;

    if (a0 == 0) {
        angle = 0xC00;
        if (a1 > 0) {
            angle = 0x400;
        }
    } else if (a1 == 0) {
        angle = (a0 < 1) << 11;
    } else {
        angle = ratan2(a0, a1);
    }
    return angle;
}


/**
 * @brief Map an angle to a quadrant bit mask.
 *
 * Rotates the angle by -0x600, inverts it, and reduces the result to a
 * quadrant index 0-3 (each quadrant spanning 0x400 units), returning a
 * single bit in the range 0x1000-0x8000.
 *
 * @param a0 Angle in fixed-point (0x1000 = 360 degrees).
 * @return 1 << (quadrant + 12).
 */
// Original devs possibly forgot to make it static
inline s32 func_80027B7C(s32 a0) {
    s32 v = a0 + 0x600;
    v = ~v & 0xFFF;
    v = v / 0x400;
    return 1 << (v + 0xC);
}

/**
 * @brief Get the analog stick dead zone of the port linked to port @p a0.
 * @param a0 Port index (only bit 0 used).
 * @return (analogStickConfig & 0x7F) << 8. func_80027CF8 reports no direction while the
 * stick's squared distance from the centre is below it.
 */
s32 getPadDeadZone(s32 a0) {
    s32 slot;
    a0 &= 1;
    slot = g_engine.gamePads[a0].inputSourceIndex;
    return (g_engine.gamePads[slot].analogStickConfig & 0x7F) << 8;
}


/**
 * @brief Set the analog stick dead zone of the port linked to port @p a0.
 *
 * Clamps @p a1 to [0, 0x7F], squares it and divides by 256 into bits 0-6 of
 * analogStickConfig, keeping bit 7 (see setPadAnalogFlag).
 *
 * @param a0 Port index (only bit 0 used).
 * @param a1 Dead-zone level, clamped to [0, 0x7F].
 */
void setPadDeadZone(s32 a0, s32 a1) {
    s32 slot;
    GamePad *linked;
    s32 sq;
    s32 clamped;
    sq = a1;
    a0 &= 1;
    slot = g_engine.gamePads[a0].inputSourceIndex;
    linked = &g_engine.gamePads[slot];
    clamped = (sq < 0) ? 0 : (sq > 0x7F) ? 0x7F : a1;
    sq = clamped * clamped;
    sq = sq / 0x100;
    linked->analogStickConfig = (linked->analogStickConfig & PAD_PORT_SWAP_ANALOG_STICKS) | sq;
}


/**
 * @brief Set or clear bit 7 of analogStickConfig, the CONFIG_ANALOG option, on the port linked to port @p a0.
 * @param a0 Port index (only bit 0 used).
 * @param a1 Nonzero to set the bit, zero to clear it.
 */
void setPadAnalogFlag(s32 a0, s32 a1) {
    s32 slot;
    GamePad *linked;
    a0 &= 1;
    slot = g_engine.gamePads[a0].inputSourceIndex;
    linked = &g_engine.gamePads[slot];
    if (a1) {
        linked->analogStickConfig |= PAD_PORT_SWAP_ANALOG_STICKS;
    } else {
        linked->analogStickConfig &= ~PAD_PORT_SWAP_ANALOG_STICKS;
    }
}


s32 func_80027CF8(s32 a, s32 b, s32 c) {
    s32 zone;
    u16 bits;

    zone = getPadDeadZone(a);
    if (b * b + c * c < zone) {
        return 0;
    }
    bits = func_80027B7C(computeAngle(b, c));
    return bits;
}


s32 func_80027DB4(s32 pad, s32 axis, s32 c) {
    GamePad *port;
    GamePad *linked;
    GamePadInputFrame *frame;
    s32 frameSlot;
    s32 result;
    s32 type;

    pad &= 1;
    port = &g_engine.gamePads[g_engine.gamePads[pad & 1].inputSourceIndex];
    frameSlot = (port->frameCounter - c) & 7;
    result = -1;
    linked = &g_engine.gamePads[g_engine.gamePads[pad].inputSourceIndex];
    frame = &port->frames[frameSlot];
    if (frame->inputReadFailed == 0) {
        if (linked->analogStickConfig & PAD_PORT_SWAP_ANALOG_STICKS) {
            axis ^= 2;
        }
        axis = CLAMP(axis, 0, 3);
        type = frame->gamePadInputIdentifier >> 4;
        if (type == 7 || type == 5) {
            result = frame->analog[axis];
        }
    }
    return result;
}


/**
 * @brief Get a pad port's vibration mask.
 * @param idx Port index (masked to 0 or 1).
 * @return 0xFF with vibration on, 0 with it off.
 */
s32 getPadVibration(s32 idx) {
    GamePad *port = &g_engine.gamePads[idx & 1];
    return port->vibrationMask;
}


