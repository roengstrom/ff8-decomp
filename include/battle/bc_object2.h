#ifndef BC_OBJECT2_H
#define BC_OBJECT2_H

#include "game.h"


#define BATTLE_ENTITY_FLAG_BIT_0   (1 << 0)  /* 0x00000000 */
#define BATTLE_ENTITY_FLAG_BIT_10  (1 << 10) /* 0x00000400 */
#define BATTLE_ENTITY_FLAG_BIT_11  (1 << 11) /* 0x00000800 */
#define BATTLE_ENTITY_FLAG_BIT_12  (1 << 12) /* 0x00001000 */
#define BATTLE_ENTITY_FLAG_BIT_13  (1 << 13) /* 0x00002000 */
#define BATTLE_ENTITY_FLAG_BIT_14  (1 << 14) /* 0x00004000 */
#define BATTLE_ENTITY_FLAG_BIT_15  (1 << 15) /* 0x00008000 */
#define BATTLE_ENTITY_FLAG_BIT_16  (1 << 16) /* 0x00010000 */
#define BATTLE_ENTITY_FLAG_BIT_17  (1 << 17) /* 0x00020000 */
#define BATTLE_ENTITY_FLAG_BIT_19  (1 << 19) /* 0x00080000 */
#define BATTLE_ENTITY_FLAG_BIT_20  (1 << 20) /* 0x00100000 */ 
#define BATTLE_ENTITY_FLAG_BIT_21  (1 << 21) /* 0x00200000 */
#define BATTLE_ENTITY_FLAG_BIT_22  (1 << 22) /* 0x00400000 */
#define BATTLE_ENTITY_FLAG_BIT_23  (1 << 23) /* 0x00800000 */
#define BATTLE_ENTITY_FLAG_BIT_24  (1 << 24) /* 0x01000000 */
#define BATTLE_ENTITY_FLAG_BIT_25  (1 << 25) /* 0x02000000 */
#define BATTLE_ENTITY_FLAG_BIT_26  (1 << 26) /* 0x04000000 */
                                                



u16 func_8009BAC4(s32 arg0, u16 arg1);

u16 func_8009BB3C(s32 arg0);

s32 func_8009BB98(void);

BattleEntry* func_8009BBD0(void);

void func_8009BC28(void);

void func_8009BCE4(void);

s32 func_8009BD60(s32 arg0);

s32 func_8009BDD0(s32 unused, s32 arg1);

void func_8009BE24(s32 arg0, s32 unused);

s32 func_8009BEA4(s32 arg0, s32 arg1);

s32 func_8009BF50(s32 newFlags, s32 *flagsPtr, s32 mask);

void func_8009BF70(s32 newFlags, s32 *flagsPtr);

s32 func_8009BFE0(s32 arg0, u16* arg1, s32* arg2, s32 arg3);

s32 func_8009C090(s32 arg0, s32 arg1, s32* arg2, u32 arg3);

s32 func_8009C104(s32 unused, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);

s32 func_8009C300(s32 arg0, s32 arg1);

u8 func_8009C390(s32 arg0, s32 arg1, s32 arg2);

u8 func_8009C570(u16 arg0);

s32 func_8009C598(s32 arg0, s32 arg1);

s32 func_8009C610(s32 arg0, s32 arg1, s32 arg2);

s32 func_8009C6CC(s32 val);

void func_8009C6E4(s32 arg0, s32 arg1, s32 arg2);

void func_8009C798(s32 arg0, s32 unused, s32 unused2);

void func_8009C8B8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

void func_8009CA14(s32 arg0);

void func_8009CAD8(s32 srcIdx, s32 dstIdx);

void func_8009CD10(s32 arg0);

void func_8009CD44(s32 arg0);

s32 func_8009CD78(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009CF18(void);

s32 func_8009CF38(s32 attackerIdx, s32 targetIdx, s32 power, u32 type);

s32 func_8009D174(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009D228(s32 arg0, s32 arg1, s32 arg2);

s32 func_8009D420(s32 unused, s32 arg1);

s32 func_8009D474(s32 unused, s32 arg1);

s32 func_8009D508(s32 unused, s32 arg1);

s32 func_8009D594(s32 unused, s32 arg1);

void func_8009D68C(void);

s32 func_8009D6C4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009D7D8(s32 arg0, s32 arg1, s32 arg2, u32 arg3);

s32 func_8009DCCC(s32 unused, s32 arg1, s32 arg2);

void func_8009DD2C(s32 arg0, s32 arg1, u16 sp10, s32 arg3);

s32 func_8009DEF0(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009E110(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009E33C(s32 arg0, s32 arg1, s32 arg2);

s32 func_8009E418(s32 arg0, s32 arg1, s32 arg2);

s32 func_8009E528(s32 arg0, s32 arg1, s32 arg2);

void func_8009E5C0(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009E684(s32 unused, s32 arg1, s32 arg2, s32 arg3);

s32 func_8009E7B0(void);

s32 func_8009E95C(void);

s32 func_8009EA08(s32 unused, s32 arg1);

void func_8009EAEC(s32 charIdx);

s32 func_8009ED2C(s32 arg0);

void func_8009EE44(s32 unused, s32 arg1);

s32 func_8009EF64(s32 arg0);

s32 func_8009F040(s32 arg0, s32 arg1, s32 unused);

void func_8009F168(s32 arg0, s32 arg1);

s32 func_8009F23C(s32 arg0, s32 arg1);

s32 func_8009F350(s32 arg0);

s32 func_8009F3F8(s32 entityIdx);

s32 func_8009F428(s32 arg0);

s32 func_8009F46C(s32 entityIdx);

s32 func_8009F4BC(s32 arg0);

s32 func_8009F52C(s32 arg0);

void func_8009F570(s32 bitIndex);

void func_8009F5B4(s32 arg0);

s32 func_8009F65C(s32 arg0, s32 arg1);

s32 func_8009F6F4(s32 idx);

s32 func_8009F718(s32 arg0, s32 arg1, s32 arg2);

void func_8009F824(void);

s32 func_8009F930(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

void func_8009FCF4(u8 cmd);

s32 func_8009FD28(s32 arg0, s32 arg1);

s32 func_8009FDD4(s32 val);

s32 func_8009FDE0(s32 arg0, s32 arg1);

void func_8009FE14(s32 arg0);

s32 func_800A085C(void);

u8 func_800A08C0(void);

void func_800A08E0(s32 unused, s32 unused2);

void func_800A0978(s32 entityIdx);

void func_800A09D0(s32 arg0);

void func_800A1760(s32 arg0, BattleCharData* arg1);

void func_800A184C(s32 idx, u16 attr, s32 flags);

void func_800A1888(s32 arg0);

#endif