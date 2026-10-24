#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "common.h"

typedef struct {
    u8 inputReadFailed;
    u8 gamePadInputIdentifier; // high nibble is gamepad type, low is number of byte pairs (16 bit) in input
    u16 currentlyPressedButtons; // buttons currently being pressed down in the frame (1 = pressed)
    u8 analog[4]; // rightX, rightY, leftX, leftY
    u16 autoRepeatedButtons[4]; // used for repeating held down inputs. e.g. useful for scrolling lists by holding up/down
    u16 newlyPressedButtons;
    u16 newlyReleasedButtons;
} GamePadInputFrame; /* 0x14 = 20 bytes */

/** @brief The state of one gamepad port. */
typedef struct {
    u8 gamePadTransmitBuf[6];
    u8 vibrationIntensity[2]; // one for each motor in the gamepad
    u8 vibrationIntensityShift[2]; // small motor only goes 0-1 and large 0-255, this scales the vibrationIntensity[] level (0-255) so it fits each motor accurately
    u8 actuatorDataChanged;
    u8 actuatorCount; // number of vibration motors (sdk calls it actuator)
    s8 autoRepeatCountdowns[4];
    u16 autoRepeatButtonMasks[4];
    u8 frameCounter;
    s8 actuatorSetupPending;
    s8 gamePadState;
    u8 vibrationMask; // 0xFF with vibration on, 0 with it off.
    GamePadInputFrame frames[8];
    u8 actuatorAlign[6]; // actuator parameter transmission bytes (used when calling SDK PadSetActAlign)
    u8 inputSourceIndex; // this gamepad's input is read from g_engine.gamePads[inputSourceIndex]
    u8 analogStickConfig; // bits 0-6 = value of analog deadzone, bit 7 swaps which analog stick should be used
} GamePad;

#define PAD_PORT_SWAP_ANALOG_STICKS 0x80

extern void func_800275D4(void);                /**< Refresh the raw gamepad buffers. */
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

extern s32 func_80027DB4(s32 pad, s32 axis, s32 c); /**< Read one analog axis. */

extern s32  func_80027CF8(s32 a, s32 b, s32 c); /**< Fold a recentred analog stick into d-pad bits. */

/* getPadReadButtons (gamepad.c) returns u16, but consumers like be_object4.c's readPads use the
   result as s32 with no widening mask — an inconsistent caller view that can't share a decl here,
   so those callers keep their own `extern s32 getPadReadButtons(...)`. */

extern void settlePadPorts(void);
extern s32 getPadVibration(s32 idx);
extern void setPadDeadZone(s32 a0, s32 a1);
extern void setPadAnalogFlag(s32 a0, s32 a1);
void requestPadSetup(s32 idx);
extern s32 getAnimGlobalState(void);
extern s32 setAnimGlobalState(s32 value);
extern void setPadMotors(s32 idx, s32 motor1, s32 motor0);
extern void setPadVibration(s32 idx, s32 val);
extern void setPadRepeatMask(s32 unused, s32 channel, s32 mask);
extern void initCdAnimSubsystem(void);

#endif /* GAMEPAD_H */
