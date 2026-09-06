#ifndef BTL_ENTITY_H
#define BTL_ENTITY_H

#include "common.h"
#include "battle.h"

extern s32 *getEntityTablePtr(s32 idx);
extern s32  func_8002BF24(s32 ot, s32 head);
extern s32  func_8002C734(s32 c);
extern void dispatchBattleEntity(s32 idx);
extern s32  allocBattleEntitySlot(void);
extern void initAllBattleEntities(void);
extern void setBattleEntityBase(s32 val);
extern s32  getMaxBattleEntities(void);
extern u8   getDigitBaseCode(void);
extern void setDigitBaseCode(u8 val);
extern void setSfxEntryParams(s32 idx, s32 val30, s32 val32);
extern void setSfxEntryTimings(s32 idx, s32 val29, s32 val2A, s32 val2C);
extern void setSfxEntryField2B(s32 idx, s32 val);
extern void setSfxEntryField34(s32 idx, s32 val);
extern void setSfxEntryField38(s32 idx, s32 val);
extern void setSfxEntryVolume(s32 idx, s32 val);

/**
 * @brief Emit one sprite per glyph of @p str and link them into @p ot.
 *
 * Walks the string, looks each character up in the font atlas at
 * @c D_80052A68, and writes a 0x14-byte sprite packet per atlas cell into the
 * primitive buffer, adding each to @p ot. Characters below 0x19 are skipped;
 * 0x19..0x1F introduce a two-byte code. The run is closed with a draw-mode
 * primitive, and the pen advances 8 pixels per character.
 *
 * @param ot   Ordering table the sprites are linked into.
 * @param prim Primitive buffer cursor.
 * @param x    Pen X, advanced per character.
 * @param y    Pen Y. Nothing is drawn outside -8 .. 0x100.
 * @param str  NUL-terminated string in the game's own encoding.
 * @param row  Atlas row; becomes the texture page/CLUT field of each sprite.
 * @return The primitive cursor past the last packet written.
 */
void *func_8002C56C(void *ot, void *prim, s32 x, s32 y, u8 *str, s32 row);

#endif
