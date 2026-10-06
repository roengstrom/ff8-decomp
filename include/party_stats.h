#ifndef PARTY_STATS_H
#define PARTY_STATS_H

#include "common.h"
#include "battle.h"

extern void func_800229FC(s32 slot);
extern void func_80022E08(s32 charIdx, s32 slot);
extern void func_800231E0(s32 charIdx, s32 battleSlot);
extern void func_8002363C(s32 gfIdx);
extern s32 findCommandSlot(BattleCharData *bc, s32 cmdType);
extern s32 clampToByte(s32 value);
extern s32 clampToMaxHp(s32 value);
extern void recalcAllGfStats(void);
extern void recalcPartyStats(void);

#endif
