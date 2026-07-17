#ifndef TRIPLETRIAD_BE_TIM_H
#define TRIPLETRIAD_BE_TIM_H

#include "common.h"

/* Raw TIM images generated from assets/tripletriad_*.tim by tools/assets.py.
   Code accesses them through the ResHeader externs in be_object1.h; these
   byte-array declarations exist only for the generated definitions. */
extern u8 g_tripleTriadCardFrames[];
extern u8 g_textBufferRes[];
extern u8 g_tripleTriadCardArt[];
extern u8 g_tripleTriadBgTim0[]; /**< 384x224 16bpp board background 0 (also declared in tripletriad.h). */
extern u8 g_tripleTriadBgTim1[]; /**< 384x224 16bpp board background 1 (also declared in tripletriad.h). */

#endif /* TRIPLETRIAD_BE_TIM_H */
