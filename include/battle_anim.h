#ifndef BATTLE_ANIM_H
#define BATTLE_ANIM_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "ui/dialog.h"
#include "ui/countdown.h"
#include "ui/seed_rank.h"

/* Engine state shared across the battle, field, menu, and Triple Triad
 * code. g_engine is a main-RAM global (0x80082DD0); the Triple Triad overlay has
 * its own copy of the pad auto-repeat code, so it accesses these same structures.
 * Kept here (rather than battle.h) so non-battle translation units can use them
 * without depending on battle.h. */

typedef struct {
    u8 field00;
    u8 field01;
    u16 field02;
    u8 params[4]; /**< 0x04-0x07: Indexed by param in func_80027FDC. */
    u16 field08;
    u16 field0A;
    u16 field0C;
    u16 field0E;
    u16 field10;
    u16 field12;
} AnimFrame; /* 0x14 = 20 bytes */

/** @brief The state of one controller port. */
typedef struct {
    u8 field00;
    u8 field01; /**< motor[0] masked by vibrationMask, shifted right by field08 (stepPadPort). */
    u8 field02; /**< motor[1] masked by vibrationMask, shifted right by field09 (stepPadPort). */
    u8 pad03[3];
    u8 motor[2]; /**< Vibration motor levels, 0-255. */
    u8 field08; /**< Shift amount applied to motor[0] in stepPadPort. */
    u8 field09; /**< Shift amount applied to motor[1] in stepPadPort. */
    u8 field0A;
    u8 field0B;
    u8 field0C;
    u8 field0D;
    u8 field0E;
    u8 field0F;
    u16 unk10[4]; /**< Pad-button mask of each of the four auto-repeat channels. */
    u8 frameCounter;
    s8 field19;   /**< Read back as a signed byte in stepPadPort. */
    s8 field1A;
    u8 vibrationMask; /**< 0xFF with vibration on, 0 with it off. */
    AnimFrame frames[8];
    u8 fieldBC[6];
    u8 linkedIdx;
    u8 fieldC3;
} PadPort;

/** @brief PadPort.fieldC3 bit 7: the analog option (CONFIG_ANALOG); bits 0-6 hold the dead zone. */
#define PAD_ANALOG_FLAG 0x80

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
    /* 0x000 */ PadPort ports[2]; /**< The two controller ports. */
    /* 0x188 */ u8 padBufs[2][0x24]; /**< Each port's raw read from the pad driver: status, type, buttons, sticks. */
    /* 0x1D0 */ s16 globalCoords[2][2];      /**< Per-slot coords [slot][axis]. */
    /* 0x1D8 */ s16 clipLeft;               /**< Clip region left edge. */
    /* 0x1DA */ s16 clipTop;                /**< Clip region top edge. */
    /* 0x1DC */ s16 clipRight;              /**< Clip region right edge. */
    /* 0x1DE */ s16 clipBottom;             /**< Clip region bottom edge. */
    /* 0x1E0 */ U16Split repeatDelays;       /**< Pad auto-repeat timing (autoRepeatPadChannel): restart delay in @c b.lo, repeat interval in @c b.hi. */
    /* 0x1E2 */ u8 padInputOn; /**< While 0, every pad reads as no buttons held. */
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
