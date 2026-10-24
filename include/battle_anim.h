#ifndef BATTLE_ANIM_H
#define BATTLE_ANIM_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "ui/dialog.h"
#include "ui/countdown.h"
#include "ui/seed_rank.h"

typedef struct {
    u8 inputReadFailed;
    u8 controllerInputIdentifier; // high nibble is controller type, low is number of byte pairs (16 bit) in input
    u16 currentlyPressedButtons; // buttons currently being pressed down in the frame (1 = pressed)
    u8 analog[4]; // rightX, rightY, leftX, leftY
    u16 autoRepeatedButtons[4]; // used for repeating held down inputs. e.g. useful for scrolling lists by holding up/down
    u16 newlyPressedButtons;
    u16 newlyReleasedButtons;
} ControllerInputFrame; /* 0x14 = 20 bytes */

/** @brief The state of one controller port. */
typedef struct {
    u8 controllerTransmitBuf[6];
    u8 vibrationIntensity[2]; // one for each motor in the controller
    u8 vibrationIntensityShift[2]; // small motor only goes 0-1 and large 0-255, this scales the vibrationIntensity[] level (0-255) so it fits each motor accurately
    u8 actuatorDataChanged;
    u8 actuatorCount; // number of vibration motors (sdk calls it actuator)
    s8 autoRepeatCountdowns[4];
    u16 autoRepeatButtonMasks[4];
    u8 frameCounter;
    s8 actuatorSetupPending;
    s8 controllerState;
    u8 vibrationMask; // 0xFF with vibration on, 0 with it off.
    ControllerInputFrame frames[8];
    u8 actuatorAlign[6]; // actuator parameter transmission bytes (used when calling SDK PadSetActAlign)
    u8 inputSourceIndex; // this controller's input is read from g_engine.controllers[inputSourceIndex]
    u8 analogStickConfig; // bits 0-6 = value of analog deadzone, bit 7 swaps which analog stick should be used
} Controller;

#define PAD_PORT_SWAP_ANALOG_STICKS 0x80

#define OT_SIZE 18

/** @brief Display list double-buffer entry (stride 0x58 = 88 bytes). */
typedef struct {
    u32 pktAlloc;
    u32 pktLimit;
    u32 ot[OT_SIZE];
    u32 pad50;
    u32 pktBase;
} DisplayListBuf;

/* Gauge lives here rather than in ui/gauge.h: fe_object9.c includes this header, and its
 * gauge opcode handlers only match while the gauge functions have no prototype. */
/** @brief A HUD gauge: a bar showing @c value between @c minValue and @c maxValue. */
typedef struct {
    s16 x; /* 0x00 */
    s16 y; /* 0x02 */
    s16 minValue; /* 0x04: the value of an empty bar */
    s16 maxValue; /* 0x06: the value of a full bar */
    s16 value; /* 0x08 */
    s16 width; /* 0x0A: the bar's length in pixels */
    s16 fill; /* 0x0C: the filled length in pixels, moving toward the value's */
    u8 flags; /* 0x0E: GAUGE_* */
    u8 pad; /* 0x0F */
} Gauge; /* 0x10 */

/** @brief @c Gauge.flags bits. */
#define GAUGE_BLINK_LOW 0x01 /**< Blink the gauge while its value is at most a quarter of the maximum. */
#define GAUGE_SNAP 0x02 /**< setGaugeValue sets @c fill straight to the new value's width. */
#define GAUGE_BLINK_ON 0x40 /**< Current blink phase: the bar is drawn white. */
#define GAUGE_ACTIVE 0x80 /**< The gauge is shown. */

#define GAUGE_COUNT 2

/** @brief Engine state every module shares: display lists, message windows, the HUD and more. */
typedef struct {
    /* 0x000 */ Controller controllers[2];
    /* 0x188 */ u8 controllerReceiveBuf[2][0x24]; // what each pad sent back, stored by libpad every VSync
    /* 0x1D0 */ s16 globalCoords[2][2];      /**< Per-slot coords [slot][axis]. */
    /* 0x1D8 */ s16 clipLeft; /**< Clip region left edge. */
    /* 0x1DA */ s16 clipTop; /**< Clip region top edge. */
    /* 0x1DC */ s16 clipRight; /**< Clip region right edge. */
    /* 0x1DE */ s16 clipBottom; /**< Clip region bottom edge. */
    /* 0x1E0 */ U16Split repeatDelays;       /**< Pad auto-repeat timing (autoRepeatPadChannel): restart delay in @c b.lo, repeat interval in @c b.hi. */
    /* 0x1E2 */ u8 pad1E2; /**< Unknown. */
    /* 0x1E3 */ u8 animFlag; /**< Also addressed directly as @c g_animFlag. */
    /* 0x1E4 */ u8 pad1E4[0x3C]; /**< Unknown. */
    /* 0x220 */ DialogSystem dialogs;               /**< Message windows; also addressed directly as @c g_dialogs. */
    /* 0x440 */ u8 pad440[0x200];            /**< Unknown. */
    /* 0x640 */ DisplayListBuf bufs[2];         /**< Double-buffered GPU display lists (2 × 0x58). */
    /* 0x6F0 */ DisplayListBuf *active;      /**< Pointer to active display list buffer. */
    /* 0x6F4 */ s32 halfSize;                /**< Half of total VRAM size. */
    /* 0x6F8 */ s32 nextPageMarkerColor; /**< Tint of a message window's next-page marker. */
    /* 0x6FC */ s32 field6FC;                /**< Cleared during GPU init. */
    /* 0x700 */ CountdownDisplay countdown; /**< The countdown timer; its brightness is the HUD's grey level. */
    /* 0x708 */ u8 pad708[0x26C]; /**< Unknown. */
    /* 0x974 */ s32 palette[3];              /**< RGB888 palette (0x40BBGGRR). */
    /* 0x980 */ s32 vibrationClock; /**< Paces the vibration steps. */
    /* 0x984 */ SeedRankNotification seedRankNotification; /**< The SeeD rank notification. */
    /* 0x9A2 */ Gauge gauges[GAUGE_COUNT]; /**< The two HUD gauges. */
    /* 0x9C2 */ s16 field9C2;               /**< Set to 0x4611 during GPU init. */
    /* 0x9C4 */ s16 cdStreamCounter;         /**< CD stream counter. */
    /* 0x9C6 */ u8 pad9C6[2];                /**< Unknown. */
    /* 0x9C8 */ s32 field9C8;                /**< Cleared during GPU init. */
    /* 0x9CC */ s32 field9CC;                /**< Cleared during GPU init. */
} EngineState;

extern EngineState g_engine;

#endif /* BATTLE_ANIM_H */
