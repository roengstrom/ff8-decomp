#include "common.h"
#include "psxsdk/libpad.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/kernel.h"
#include "psxsdk/libapi.h"
#include "psxsdk/libetc.h"
#include "psxsdk/r3000.h"
#include "battle.h"
#include "thread.h"

static s32 func_80027038(EngineState *a0, PadPort *port, u16 a2, u16 a3, s32 a4);
static void func_800270B0(PadPort *a0, PadPort *port, s32 a2);
static void stepPadPort(PadPort *a0, PadPort *port, s32 a2);
static void stepPadPorts(PadPort *a0);
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


static s32 func_80027038(EngineState *a0, PadPort *port, u16 a2, u16 a3, s32 a4) {
    s32 counter;
    s32 interval;
    u16 restartDelay;
    u16 repeatInterval;
    u16 mask;

    restartDelay = a0->repeatDelays.hword;
    mask = port->unk10[a4];
    counter = port->field0C[a4];

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
    port->field0C[a4] = counter;
    return a3;
}


static void func_800270B0(PadPort *a0, PadPort *port, s32 a2) {
    s32 i;
    s32 j;
    s32 info;
    s32 motor1Act = -1;
    s32 motor0Act = -1;
    s32 motor0Bits;
    s32 motor1Bits;
    u8 *align = port->fieldBC;
    u8 *p;

    i = PadInfoAct(a2, -1, 0);
    port->field0B = i;
    while (--i >= 0) {
        info = PadInfoAct(a2, i, 1);
        if (info == 1) {
            info = PadInfoAct(a2, i, 2);
            if (info == 1) {
                motor1Act = i;
                info = PadInfoAct(a2, motor1Act, 3);
                if (info == 0) {
                    motor1Bits = 1;
                } else {
                    motor1Bits = info << 3;
                }
            } else if (info == 2) {
                motor0Act = i;
                info = PadInfoAct(a2, motor0Act, 3);
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
        port->field08 = motor0Bits;
    }
    if (motor1Act != -1) {
        motor1Bits = 8 - motor1Bits;
        align[2] = motor1Act;
        port->field09 = motor1Bits;
    }
}


/**
 * @brief Step a pad port's state machine.
 *
 * @param a0 Base context passed through to func_800270B0.
 * @param port Port to update.
 * @param a2 Driver port number: 0x00 for port 0, 0x10 for port 1.
 */
static void stepPadPort(PadPort *a0, PadPort *port, s32 a2) {
    s32 status;
    GetSr();
    status = PadGetState(a2);
    port->field1A = status;
    switch (status) {
    case 1:
        port->field0B = 0;
        port->field0A = 1;
        port->field19 = 1;
        break;
    case 2:
        port->field0B = 0;
        port->field19 = 1;
        port->field0A = 1;
        port->vibrationMask = 0;
        break;
    case 4:
    case 5:
        break;
    case 6: {
        u8 mask = port->vibrationMask;
        if (port->field19 == 1) {
            func_800270B0(a0, port, a2);
            port->field0A = 1;
            port->field19 = 0;
            port->controllerTransmitBuf[1] = 0;
            port->controllerTransmitBuf[2] = 0;
            func_8003AF50(a2, port->fieldBC);
            func_8003AFD0(a2, port->controllerTransmitBuf, 6);
        } else {
            u8 f6 = port->motor[0];
            u8 f7 = port->motor[1];
            u8 sh7 = port->field09;
            u8 sh6;
            s32 v6;
            s32 v7;
            /* The do-while(0) wrapper is load-bearing for the byte-match: it
               keeps the scheduler from tearing the statement pair apart,
               and likely mirrors a macro in the original source. */
            do { port->field0A = 1; v6 = f6 & mask; } while (0);
            v7 = f7 & mask;
            sh6 = port->field08;
            port->controllerTransmitBuf[2] = v7 >> sh7;
            port->controllerTransmitBuf[1] = v6 >> sh6;
        }
        break;
    }
    case 0:
    case 3:
    default:
        port->field0B = 0;
        port->field0A = 1;
        port->vibrationMask = 0;
        port->field19 = 1;
        break;
    }
}


/**
 * @brief Check the CD stream status, restarting playback if it has ended.
 *
 * @param a0 Stream channel selector.
 * @return 1 if the stream is in state 2, the result of func_8003AF88 if
 *         it is in state 6, 0 otherwise.
 */
s32 func_80027360(s32 a0) {
    s32 status;
    s32 ret;
    status = PadGetState(a0);
    ret = 0;
    switch (status) {
    case 2:
        ret = 1;
        break;
    case 6:
        ret = func_8003AF88(a0, 1, 1);
        break;
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
        break;
    }
    return ret;
}


/**
 * @brief Make stepPadPort rerun a pad port's setup on its next state-6 tick.
 * @param idx Index into g_engine.controllerPorts.
 */
void requestPadSetup(s32 idx) {
    PadPort *port = &g_engine.controllerPorts[idx];
    port->field19 = 1;
    port->field0A = 1;
}


/**
 * @brief Step both pad ports: port 0 as driver port 0x00, port 1 as 0x10.
 * @param a0 Address of the first port.
 */
static void stepPadPorts(PadPort *a0) {
    stepPadPort(a0, &a0[0], 0);
    stepPadPort(a0, &a0[1], 0x10);
}


void settlePadPorts(void) {
    PadPort *port = &g_engine.controllerPorts[0];
    PadPort *ports = g_engine.controllerPorts;
    s32 count;
    /* Without this barrier the register allocator gives s0 to count
       instead of port. */
    REGALLOC_BARRIER(port);
    count = 0;
    port->motor[0] = 0;
    port->motor[1] = 0;
    for (;;) {
        while (func_8003AB64() == 0) {}
        stepPadPort(ports, port, 0);
        if (port->field1A == 0 || port->field1A == 2) {
            break;
        }
        if (port->field1A == 6) {
            count++;
            if (count >= 2) {
                break;
            }
        }
    }
    port = &ports[1];
    count = 0;
    port->motor[0] = 0;
    port->motor[1] = 0;
    for (;;) {
        while (func_8003AB64() == 0) {}
        stepPadPort(ports, port, 0x10);
        if (port->field1A == 0 || port->field1A == 2) {
            break;
        }
        if (port->field1A == 6) {
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
 * @brief Read field0B from a pad port.
 * @param idx Port index.
 */
s32 getPadField0B(s32 idx) {
    PadPort *port = &g_engine.controllerPorts[idx];
    return port->field0B;
}


void func_800275D4(void) {
    EngineState *engine = &g_engine;
    PadPort *port;
    u8 *buf;
    AnimFrame *frame;
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

    stepPadPorts(engine->controllerPorts);
    port = engine->controllerPorts;
    sr = GetSr();
    if (!(sr & SR_IEP)) {
        EnterCriticalSection();
    }
    for (i = 0; i < 2; i++, port++) {
        buf = engine->controllerReceiveBuf[i];
        linked = i;
        type = buf[1] >> 4;
        fc = port->frameCounter;
        prev = port->frames[fc].field02;
        held = 0;
        if (port->field1A != 0) {
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
                        frame = (AnimFrame *)buf;
                        dx = frame->params[0] - 0x80;
                        dy = frame->params[1] - 0x80;
                        if (dx * dx + dy * dy < 40 * 40) {
                            dx = frame->params[2] - 0x80;
                            dy = frame->params[3] - 0x80;
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
        frame->field02 = held;
        frame->field10 = (prev ^ held) & held;
        frame->field12 = (prev ^ held) & ~held;
        frame->field08 = func_80027038(engine, port, prev, held, 0);
        frame->field0A = func_80027038(engine, port, prev, held, 1);
        frame->field0C = func_80027038(engine, port, prev, held, 2);
        frame->field0E = func_80027038(engine, port, prev, held, 3);
        port->frameCounter = fc;
        port->linkedIdx = linked;
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
    PadPort *ports;
    PadPort *port;
    PadPort *linked;
    s32 sub_idx;
    idx &= 1;
    ports = g_engine.controllerPorts;
    port = ports + idx;
    linked = ports + port->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    return ((AnimFrame *)((u8 *)linked + 0x1C))[sub_idx].field02;
}


/**
 * @brief Get the auto-repeated buttons of a past read on the port linked to port @p idx.
 * @param idx Port index (masked to 0 or 1).
 * @param offset How many reads back to look.
 * @return The four auto-repeat channels' bits (field08-field0E), ORed.
 */
u16 getPadReadRepeat(s32 idx, s32 offset) {
    PadPort *ports;
    PadPort *port;
    PadPort *linked;
    s32 sub_idx;
    AnimFrame *frame;
    idx &= 1;
    ports = g_engine.controllerPorts;
    port = ports + idx;
    linked = ports + port->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    frame = &linked->frames[sub_idx];
    return frame->field08 | frame->field0A | frame->field0C | frame->field0E;
}


/**
 * @brief Get the buttons newly pressed in a past read on the port linked to port @p idx.
 * @param idx Port index (only bit 0 used).
 * @param offset How many reads back to look.
 * @return field10: the buttons held in that read but not in the one before.
 */
u16 getPadReadPressed(s32 idx, s32 offset) {
    PadPort *ports;
    PadPort *port;
    PadPort *linked;
    s32 sub_idx;
    idx &= 1;
    ports = g_engine.controllerPorts;
    port = ports + idx;
    linked = ports + port->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    return ((AnimFrame *)((u8 *)linked + 0x1C))[sub_idx].field10;
}


/**
 * @brief Get the buttons newly released in a past read on the port linked to port @p idx.
 * @param idx Port index (only bit 0 used).
 * @param offset How many reads back to look.
 * @return field12: the buttons held in the read before but not in that one.
 */
static u16 getPadReadReleased(s32 idx, s32 offset) {
    PadPort *ports;
    PadPort *port;
    PadPort *linked;
    s32 sub_idx;
    idx &= 1;
    ports = g_engine.controllerPorts;
    port = ports + idx;
    linked = ports + port->linkedIdx;
    sub_idx = (linked->frameCounter - offset) & 7;
    return ((AnimFrame *)((u8 *)linked + 0x1C))[sub_idx].field12;
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
 * @return (fieldC3 & 0x7F) << 8. func_80027CF8 reports no direction while the
 * stick's squared distance from the centre is below it.
 */
s32 getPadDeadZone(s32 a0) {
    s32 slot;
    a0 &= 1;
    slot = g_engine.controllerPorts[a0].linkedIdx;
    return (g_engine.controllerPorts[slot].fieldC3 & 0x7F) << 8;
}


/**
 * @brief Set the analog stick dead zone of the port linked to port @p a0.
 *
 * Clamps @p a1 to [0, 0x7F], squares it and divides by 256 into bits 0-6 of
 * fieldC3, keeping bit 7 (see setPadAnalogFlag).
 *
 * @param a0 Port index (only bit 0 used).
 * @param a1 Dead-zone level, clamped to [0, 0x7F].
 */
void setPadDeadZone(s32 a0, s32 a1) {
    s32 slot;
    PadPort *linked;
    s32 sq;
    s32 clamped;
    sq = a1;
    a0 &= 1;
    slot = g_engine.controllerPorts[a0].linkedIdx;
    linked = &g_engine.controllerPorts[slot];
    clamped = (sq < 0) ? 0 : (sq > 0x7F) ? 0x7F : a1;
    sq = clamped * clamped;
    sq = sq / 0x100;
    linked->fieldC3 = (linked->fieldC3 & PAD_PORT_FLAG_UNK80) | sq;
}


/**
 * @brief Set or clear bit 7 of fieldC3, the CONFIG_ANALOG option, on the port linked to port @p a0.
 * @param a0 Port index (only bit 0 used).
 * @param a1 Nonzero to set the bit, zero to clear it.
 */
void setPadAnalogFlag(s32 a0, s32 a1) {
    s32 slot;
    PadPort *linked;
    a0 &= 1;
    slot = g_engine.controllerPorts[a0].linkedIdx;
    linked = &g_engine.controllerPorts[slot];
    if (a1) {
        linked->fieldC3 |= PAD_PORT_FLAG_UNK80;
    } else {
        linked->fieldC3 &= ~PAD_PORT_FLAG_UNK80;
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
    PadPort *port;
    PadPort *linked;
    AnimFrame *frame;
    s32 frameSlot;
    s32 result;
    s32 type;

    pad &= 1;
    port = &g_engine.controllerPorts[g_engine.controllerPorts[pad & 1].linkedIdx];
    frameSlot = (port->frameCounter - c) & 7;
    result = -1;
    linked = &g_engine.controllerPorts[g_engine.controllerPorts[pad].linkedIdx];
    frame = &port->frames[frameSlot];
    if (frame->field00 == 0) {
        if (linked->fieldC3 & PAD_PORT_FLAG_UNK80) {
            axis ^= 2;
        }
        axis = CLAMP(axis, 0, 3);
        type = frame->field01 >> 4;
        if (type == 7 || type == 5) {
            result = frame->params[axis];
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
    PadPort *port = &g_engine.controllerPorts[idx & 1];
    return port->vibrationMask;
}


