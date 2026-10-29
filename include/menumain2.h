#ifndef MENUMAIN2_H
#define MENUMAIN2_H

#include "battle.h"
#include "common.h"
#include "menu.h"
#include "psxsdk/libgpu.h"

u8 *func_801F08D4(s32, s32, s32, s32);
void func_801F0954(s32, s32, s32, s32);
void func_801F0A34(s32, s32, s32, s32);
void func_801F0A78(s32, s32, s32, s32, s32);
s32 func_801F0BB0(void);
void func_801F0BF8(s32);
void func_801F0C5C(u8, void *);
s32 func_801F0D84(void);
s32 func_801F0F20(s32, SPRT *, s32, u32, s32);
void func_801F0FD0(s32, s32);
u32 func_801F0FEC(s32, s32, s32, s32, u8 *, s32);
void *func_801F179C(MenuTickCallback, MenuDrawCallback);
void func_801F1AFC(void);
void func_801F1B10(void);
void func_801F1B4C(s32);
void func_801F202C(void);
s32 menumain_getPartyMemberMask(void);
s32 func_801F22F4(void);
u16 func_801F2370(void);
void func_801F23D0(s32, s32, u8 *);
s32 func_801F3DE4(s32, s32, s32, s32, s32, s32, s32);
s32 func_801F3FB4(s32);
s32 func_801F5104(s32);
s32 func_801F510C(s32);
s32 func_801F5144(s32);
s32 func_801F5150(s32, s32, s32);
void func_801F537C(s32, BattleCharData *);
void func_801F5400(s32);
s32 func_801F565C(s32, s32);
s32 func_801F56E4(s32, s32);
void func_801F576C(s32, s32);
s32 func_801F57A4(s32);
void func_801F5868(s32, s32);
s32 func_801F58EC(s32);
s32 func_801F5F30(s32, s32, s32, s32, s32, s32);
s32 func_801F5F60(s32, s32, s32, s32);
s32 func_801F605C(s32, s32, s32, s32, s32);
s32 func_801F6234(s32, s32, s32, s32, s32);
void *MenuDrawPortrait(s32, void *, void *, s32, s32, u32);
s32 func_801F64A4(s32, s32, s32, s32, s32, s32, s32);
s32 func_801F65F0(s32, s32, s32, s32, s32, s32);
void func_801F66B0(s32, s32, s32, s32, s32);
s32 func_801F6768(u16, s32, s32);
u8 *func_801F6AA4(s32);                      /**< Look up a string in menu text category 3. */
u8 *func_801F6AD0(s32);                      /**< Look up a string in menu text category 0. */
s32 func_801F6AFC(s32);
void *func_801F6C9C(s32, s32, s32, s32, s32, u16);
void func_801F6F88(s32);
s32 func_801F72B4(void);
void func_801F72C4(void);
s32 func_801F7394(s32);
s32 func_801F76E0(s32, s32, s32);

#endif /* MENUMAIN2_H */
