#include "common.h"

/** @brief Battle entity structure (stride 612 / 0x264 bytes).
 *  Accessed via pointer at D_80085224. Fields named from func_80011870 usage.
 */
typedef struct {
    u8  pad0[0x190];   /* 0x000..0x18F */
    s32 pos_x;         /* 0x190  fixed-point 20.12 */
    s32 pos_y;         /* 0x194  fixed-point 20.12 */
    u8  pad1[0x62];    /* 0x198..0x1F9 */
    u16 rotation;      /* 0x1FA */
    u8  pad2[0x45];    /* 0x1FC..0x240 */
    u8  anim_state;    /* 0x241 */
    u8  pad3[0x22];    /* 0x242..0x263 */
} BattleEntity;        /* total: 0x264 = 612 */

/** @brief Saves the current battle/field camera state to a snapshot buffer.
 *
 *  Records party member positions (fixed-point 20.12 -> integer), rotations,
 *  animation states, and display parameters from the battle entity array
 *  (stride 612 at D_80085224) into g_gameState+0xD40..0xD5C. The snapshot
 *  is later restored by RestoreSnapshot when returning from battle.
 *
 *  @note Non-matching: same instruction count (89) but different register
 *  allocation and scheduling. Pre-loop: compiler skips "through $v0" copies
 *  for g_gameState/g_fieldEntity addresses (addiu directly into target reg
 *  instead of same-reg addiu + addu copy). Can be fixed with register
 *  asm("$2") temp + REGALLOC_BARRIER, but causes scheduling side effects.
 *  Loop body: all 4 temp registers shifted (idx=$a0 vs $a1, eoff=$a3 vs
 *  $a2, stp=$a2 vs $a0, entityPtr=$a1 vs $a3). Loop increment: direct
 *  addiu instead of through-$a0 copy pattern (addiu $a0,$t0,1; addu
 *  $t0,$a0,$zero), preventing $v0 from staying free for delay slot sb.
 *  Epilogue: uses 2 registers (sequential) instead of 3 (interleaved).
 *  Permuter best score: ~4850 (base 6025), not trending toward 0.
 *
 *  @code
 *  void func_80011870(void) {
 *      extern volatile u16 D_8005F14C;
 *      extern u16 g_currentMusicTrack;
 *      extern u8 D_8005F150;
 *      extern u8 D_8005F151;
 *      extern u16 D_800780B8;
 *      extern u8 g_gameState[];
 *      extern u8 g_fieldEntity[];
 *      extern BattleEntity *D_80085224;
 *      s32 i;
 *      s32 buf;
 *      s32 src;
 *
 *      if ((s16)D_8005F14C == 2) {
 *          D_800780B8 = 2;
 *      } else {
 *          D_800780B8 = 1;
 *      }
 *
 *      i = 0;
 *      buf = g_gameState;
 *      src = g_fieldEntity;
 *      *(u16 *)(buf + 0xD42) = g_currentMusicTrack;
 *      *(u16 *)(buf + 0xD44) = *(u16 *)(src + 0x120);
 *
 *  top:
 *      {
 *          s16 idx = (s16)i;
 *          s32 eoff = idx + src;
 *          s32 stp = idx * 2 + buf;
 *
 *          *(u16 *)(stp + 0xD46) = D_80085224[*(u8 *)(eoff + 0x12)].pos_x >> 12;
 *          *(u16 *)(stp + 0xD4C) = D_80085224[*(u8 *)(eoff + 0x12)].pos_y >> 12;
 *          *(u16 *)(stp + 0xD52) = D_80085224[*(u8 *)(eoff + 0x12)].rotation;
 *          i++;
 *          *(u8 *)(idx + buf + 0xD58) = D_80085224[*(u8 *)(eoff + 0x12)].anim_state;
 *      }
 *      if ((s16)i < 3) goto top;
 *
 *      buf = g_gameState;
 *      *(u8 *)(buf + 0xD5B) = D_8005F151;
 *      *(u8 *)(buf + 0xD5C) = D_8005F150;
 *  }
 *  @endcode
 */
INCLUDE_ASM("asm/nonmatchings/2070", func_80011870);
