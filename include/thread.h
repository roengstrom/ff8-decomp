#ifndef THREAD_H
#define THREAD_H

#include "common.h"

/* Public prototypes */
extern void func_800275D4(void);                /**< Refresh the raw controller buffers. */
/* Which axis func_80027DB4 reads. These are mutually exclusive selectors, not
   flags -- the value is an index, never OR-ed or masked. X/Y are as documented
   below; the other pair is named from the order callers sample them in, not from
   a decoded implementation. */
typedef enum {
    PAD_AXIS_X2 = 0,
    PAD_AXIS_Y2 = 1,
    PAD_AXIS_X  = 2,
    PAD_AXIS_Y  = 3
} PadAxis;

extern s32  func_80027DB4(s32 idx, s32 axis, s32 frameOffset); /**< Read one analog axis (a PadAxis). */

extern s32  func_80027CF8(s32 idx, s32 x, s32 y); /**< Fold a recentred analog stick into d-pad bits. */

/* Inline in thread.c, which expands both into func_80027CF8; the standalone copies stay global. */
extern s32 computeAngle(s32 a0, s32 a1); /**< Angle of a stick offset; 0x1000 = full circle. */
extern s32 func_80027B7C(s32 a0); /**< The d-pad bit (0x1000-0x8000) an angle points at. */

/* getPadReadButtons (thread.c) returns u16, but consumers like be_object4.c's readPads use the
   result as s32 with no widening mask — an inconsistent caller view that can't share a decl here,
   so those callers keep their own `extern s32 getPadReadButtons(...)`. */

extern u32 getStatusRegister(void);
extern void setStatusRegister(u32 status);
extern void settlePadPorts(void);
extern s32 getPadVibration(s32 idx);
extern void setPadDeadZone(s32 a0, s32 a1);
extern void setPadAnalogFlag(s32 a0, s32 a1);
extern s32 openThreadSafe(void (*entry)(void), u8 *stack);
extern void closeThreadSafe(s32 thread);
extern void switchThread(s32 thread);
#endif /* THREAD_H */
