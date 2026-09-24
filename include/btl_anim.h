#ifndef BTL_ANIM_H
#define BTL_ANIM_H

#include "common.h"
#include "psxsdk/libgpu.h"

/* Battle display-list render helpers (btl_anim.c). */

/* Public prototypes */
extern void renderAndUpdateDisplay(s32 frameCount); /**< Advance and render the battle display list. */
extern s32  renderBattleDisplayList(s32 *colorTag); /**< Walk the ordering table and emit its primitives. */
extern u8  *emitDrawEnvPackets(P_TAG *ot, u8 *pkt); /**< Emit SetDrawArea/SetDrawOffset packets, link into OT. */

extern s32 getAnimGlobalState(void);
extern s32 setAnimGlobalState(s32 value);
extern void setAnimEntityParams(s32 idx, s32 param7, s32 param6);
void btlStrcat2(u8 *dst, u8 *src);

#endif /* BTL_ANIM_H */
