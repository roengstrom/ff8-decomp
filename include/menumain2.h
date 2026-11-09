#ifndef MENUMAIN2_H
#define MENUMAIN2_H

#include "common.h"
#include "menu.h"
#include "psxsdk/libgpu.h"

/**
 * @brief The main menu's view of its menu-task slot.
 *
 * func_801F6934 claims the slot with func_801F179C, registering func_801F2458 as
 * the update callback and func_801F4A98 as the draw callback. The first 0x10 bytes
 * are the shared MenuTask header (include/menumain.h); bytes of unknown use are
 * padding.
 */
typedef struct {
    u8* pad00[4]; /**< 0x00: MenuTask header, then the update callback's state word. */
    u16 state; /**< 0x10 */
    u8 pad12[0xE]; /**< 0x12 */
    u16 field20; /**< 0x20: func_80036EC0() at start; field23 counts its set bits. */
    u8 unk22; /**< 0x22 */
    u8 field23; /**< 0x23 */
    s32 colonTimer; /**< 0x24: Frames the clock colon stays bright: 30 (25 on PAL) each time the play time ticks, counted down to 0. */
    s32 lastPlayTime; /**< 0x28: The play time colonTimer was last started for. */
    s16 brightness; /**< 0x2C: Menu brightness, 0x1000 = full; raised by 0x100 per frame as the menu opens. */
    u8 pad2E[0x2]; /**< 0x2E */
    u8 unk30; /**< 0x30 */
    u8 unk31; /**< 0x31 */
    u16 field32; /**< 0x32: func_801F22F4() at start. */
    u8 unk34; /**< 0x34 */
    u8 party[3]; /**< 0x35: The active party's slot ids, saved from g_gameState.mainData. */
    u8 reserve[8]; /**< 0x38: The other characters' slot ids, 0xFF for none. */
    s8 field40; /**< 0x40 */
    u8 field41; /**< 0x41: D_801FAB30 at start. */
    u8 unk42; /**< 0x42 */
    s8 field43; /**< 0x43 */
    s16 field44; /**< 0x44 */
    u8 unk46; /**< 0x46 */
    u8 unk47; /**< 0x47 */
    s16 unk48; /**< 0x48 */
    u8 unk4A; /**< 0x4A */
    u8 field4B; /**< 0x4B */
} MainMenuCtx;

extern s16 D_801FAB28;
extern s16 D_801FAB2A;
extern u8 D_801FAB30;
extern u8 D_801FAB31;
extern u8 D_801FAB7A;
extern u8 D_801FAB7B;

DR_AREA *func_801F03E8(P_TAG *, DR_AREA *);
void func_801F04E8(s32, u32, s32, s32);
u8 *func_801F08D4(s32, s32, s32, s32);
void func_801F0948(s32);
void func_801F0954(s32, s32, s32, s32);
s32 func_801F0994(s32, s32, s32);
void func_801F09C4(s32, s32, s32, s32);
void func_801F0A34(s32, s32, s32, s32);
void func_801F0A78(s32, s32, s32, s32, s32);
s32 func_801F0BB0(void);
void func_801F0BF8(s32);
void func_801F0C5C(s32, void *);
s32 func_801F0D84(void);
s32 func_801F0F20(s32, SPRT *, s32, u32, s32);
void func_801F0FD0(s32, s32);
void *func_801F0FEC(void *ot, void *prim, s32 x, s32 y, u8 *iconIds, u32 clutFlags);
void *func_801F179C(MenuTickCallback, MenuDrawCallback);
void func_801F18FC(s32 *);
void func_801F1AFC(void);
void func_801F1B10(void);
void func_801F1B4C(s32);
void func_801F1CAC(void);
void func_801F1D2C(s32, s32, s32);
void func_801F1D84(void);
void func_801F1DB0(s32);
void func_801F1E20(MainMenuCtx *ctx);
void func_801F1E54(MainMenuCtx *ctx);
void func_801F1F58(s32, s32);
void func_801F1F78(s32, s32);
void func_801F1F98(MainMenuCtx *ctx);
void func_801F202C(void);
s32 func_801F21D0(s32);
void func_801F21FC(s32 charId, s32 statusFlags);
s32 func_801F2238(s32);
s32 func_801F2240(s32);
s32 menumain_getPartyMemberMask(void);
s32 func_801F22F4(void);
u16 func_801F2370(void);
void func_801F23D0(s32, s32, u8 *);

#endif /* MENUMAIN2_H */
