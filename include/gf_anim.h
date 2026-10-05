#ifndef GF_ANIM_H
#define GF_ANIM_H

#include "common.h"

extern void func_800229FC(s32 slot);
extern void func_80022E08(s32 charIdx, s32 slot);
extern void func_8002363C(s32 gfIdx);
extern s32 findCommandSlot(u8 *a0, s32 a1);
extern s32 clampToByte(s32 a0);
extern s32 clampToMaxHp(s32 a0);
extern void recalcAllGfStats(void);
extern void recalcPartyStats(void);

#endif
