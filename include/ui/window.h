#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/** @brief @c BattleDisplayEntity.entityType bits, as setBattleEntityType stores them. */
#define BATTLE_ENTITY_SEMI_TRANS 0x01 /**< The box is drawn semi-transparent. */
#define BATTLE_ENTITY_BOX 0x02 /**< The entity has a box: frame and background. */
#define BATTLE_ENTITY_DIALOG 0x08 /**< A dialog window: its render callback draws the box, not the generic renderer. */

/** @brief func_8002B3A0 sides: draw the left corners and edge, the right ones. */
#define WINDOW_FRAME_LEFT 0x1
#define WINDOW_FRAME_RIGHT 0x2

extern struct BattleDisplayEntity *getBattleEntity(s32 idx); /**< Battle entity @p idx. */
extern s32 getBattleEntityAnimSpeed(s32 idx); /**< Anim speed of entity @p idx: the OT entry it draws into. */
extern void setBattleEntityBoundRect(s32 idx, RECT *src); /**< Set an entity's bounding rect from @p src. */
extern void setBattleEntityRectClamp(s32 idx, RECT *src); /**< Set an entity's clamp rect from @p src. */
extern void setBattleEntityType(s32 idx, s32 val);
extern void setBattleEntityField00(s32 idx, s32 val);
extern void setBattleEntityField04(s32 idx, EntityRenderCallback val);
extern void setBattleEntityField36(s32 idx, s32 val);
extern u32 getBattleEntityField36(s32 idx);
extern void setBattleEntityField35(s32 idx, s32 val);
extern u32 getBattleEntityField35(s32 idx);
extern void setBattleEntityActive(s32 idx, s32 value);
extern s32 GetActiveFlag(s32 idx);
extern void setBattleEntityBrightness(s32 idx, s32 val);
extern s32 getBattleEntityBrightness(s32 idx);
extern void initBattleEntity(s32 idx);
extern s32 clipBlitRects(BlitParams *arg);

/* func_8002B3A0 draws a window's frame, func_8002B898 the whole frame; func_8002B8BC draws
   its background, its bgOffset shifting the pattern. */
extern DR_AREA *func_8002B3A0(void *ot, DR_AREA *prim, RECT *rect, s32 color, s32 sides);
extern DR_AREA *func_8002B898(void *ot, DR_AREA *prim, RECT *rect, s32 color);
extern DR_AREA *func_8002B8BC(void *ot, DR_AREA *prim, RECT *rect, s32 color, s32 bgOffset);
extern DR_AREA *drawWindowBackground(void *ot, DR_AREA *prim, RECT *rect, s32 color);

extern ModeSprt *func_8002BAA0(u32 *ot, ModeSprt *p, RECT *rect, u32 color);
extern s32 *getEntityTablePtr(s32 idx);
extern u8 *func_8002BF24(u32 *ot, u8 *head);
extern void dispatchBattleEntity(s32 idx, u32 input, u32 repeat);
extern s32 allocBattleEntitySlot(void);
extern void initAllBattleEntities(void);
extern void SetNameDictionary(s32 val);
extern s32 getMaxBattleEntities(void);

/** @brief The string table func_8002F610 looks strings up in, stored by SetNameDictionary. */
extern s32 g_nameDictionary;

#endif
