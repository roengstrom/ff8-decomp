#ifndef MENUMAIN2B_H
#define MENUMAIN2B_H

#include "battle.h"
#include "character.h"
#include "common.h"
#include "menumain2.h"

extern ST_D_801FAB98 D_801FAB98[];

DR_AREA *func_801F2FAC(P_TAG *ot, DR_AREA *prim);
void func_801F3270(void *ot, void *prim, s32 x, s32 y, u32 arg4);
void *func_801F3824(void *ot, void *prim, s32 x, s32 y, u32 value);
void *func_801F3994(u8 *text, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
void *func_801F3ABC(s32 value, void *ot, void *prim, s32 x, s32 y, u32 color);
void *func_801F3CE0(s32 value1, s32 value2, void *ot, void *prim, s32 x, s32 y, u32 color);
void *func_801F3DE4(s32 value1, s32 value2, void *ot, void *prim, s32 x, s32 y, u32 color);
s32 func_801F3FB4(s32);
void *func_801F4168(void *ot, void *prim, u8 *text, s32 x, s32 y, s32 progress, s32 iconType);
void *func_801F4D70(void *ot, void *prim, s32 x, s32 y, u32 width, u32 clut);
void *func_801F4EA8(void *ot, LINE_F2 *prim, s32 x, s32 y, s32 arg4);
s32 func_801F5104(s32);
s32 func_801F510C(s32);
s32 func_801F5144(s32);
s32 func_801F5150(s32, s32, s32);
void func_801F537C(s32, BattleCharData *);
void func_801F5400(s32);
void func_801F5440(void);
u32 func_801F565C(u32 charId, u32 otherChars);
u32 func_801F56E4(u32 charId, u32 otherChars);
void func_801F576C(s32, s32);
s32 func_801F57A4(s32);
void func_801F5868(s32, s32);
s32 func_801F58EC(s32);
void *func_801F5A38(void *ot, void *prim, s32 x, s32 y, u32 stringType, s16 *stringIds, u32 availableStrings);
s32 func_801F5F30(s32, s32, s32, s32, s32, s32);
s32 func_801F5F60(s32, s32, s32, s32);
void *func_801F605C(void* ot, void* prim, s32 x, s32 y, CharMenuInfo *info);
s32 func_801F6234(s32, s32, s32, s32, s32);
void *MenuDrawPortrait(s32, void *, void *, s32, s32, u32);
void *func_801F64A4(void *ot, void *prim, s32 x, s32 y, CharacterData *chr, CharMenuInfo *info, s32 arg6);
s32 func_801F65F0(s32, s32, s32, s32, s32, s32);
void func_801F66B0(s32, s32, s32, s32, s32);
s32 func_801F6768(u16, s32, s32);
void func_801F6888(void);
u8 *func_801F6AA4(s32);                      /**< Look up a string in menu text category 3. */
u8 *func_801F6AD0(s32);                      /**< Look up a string in menu text category 0. */
u8 *func_801F6AFC(s32);
void *func_801F6C9C(s32, s32, s32, s32, s32, u16);
void *func_801F6D88(void *ot, void *prim, s32 x, s32 y, s32 gfId);
void func_801F6F88(s32);
s32 func_801F72B4(void);
void func_801F72C4(void);
s32 func_801F738C(s32);
s32 func_801F7394(s32);
s32 func_801F76E0(s32, s32, s32);

#endif /* MENUMAIN2B_H */
