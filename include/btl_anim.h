#ifndef BTL_ANIM_H
#define BTL_ANIM_H

#include "common.h"
#include "psxsdk/libgpu.h"

/* Battle display-list render helpers (btl_anim.c). */

/* Public prototypes */
extern void renderAndUpdateDisplay(s32 frameCount); /**< Advance and render the battle display list. */
extern s32  renderBattleDisplayList(s32 *colorTag); /**< Walk the ordering table and emit its primitives. */
extern u8  *emitDrawEnvPackets(P_TAG *ot, u8 *pkt); /**< Emit SetDrawArea/SetDrawOffset packets, link into OT. */
extern s32  getDisplayListHead(void);              /**< Next free packet in the active display-list buffer. */
extern void copyDisplayRect(RECT *dst); /**< Copy the active draw environment's clip rect to @p dst. */
extern void copyDisplayCoords(u16 *ofs); /**< Copy the active draw environment's draw offset to @p ofs. */

extern s32 getAnimGlobalState(void);
extern s32 setAnimGlobalState(s32 value);
extern void setPadMotors(s32 idx, s32 motor1, s32 motor0);
extern void setPadVibration(s32 idx, s32 val);
extern void setPadRepeatMask(s32 unused, s32 channel, s32 mask);
extern s32 btlStrlen(u8 *str);

/* Public data */
extern u8 g_animCurveFadeOut[]; /**< Easing curve: 65 entries that rise exponentially to 64. */
extern u8 g_animCurveFadeIn[]; /**< g_animCurveFadeOut reversed: falls exponentially from 64. */

#endif /* BTL_ANIM_H */
