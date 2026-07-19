#ifndef TT_TIM_H
#define TT_TIM_H

#include "common.h"

/* Raw binary assets (TIM images + AKAO sound data) generated from
   assets/tripletriad_* by tools/assets.py. Code accesses them through the
   typed externs in the tt_*.h headers; these byte-array declarations
   exist only for the generated definitions. */
extern u8 g_tripleTriadCardFrames[];
extern u8 g_textBufferRes[];
extern u8 g_tripleTriadCardArt[];
extern u8 g_tripleTriadBgTim0[]; /**< 384x224 16bpp board background 0 (also declared in tripletriad.h). */
extern u8 g_tripleTriadBgTim1[]; /**< 384x224 16bpp board background 1 (also declared in tripletriad.h). */
extern u8 g_tripleTriadAkao[];       /**< AKAO audio block fed to sndProcessAudio (also in tt_ui.h). */
extern u8 g_tripleTriadSoundBank[];  /**< Sound-bank image at the end of the overlay (also in tt_ui.h). */

#endif /* TT_TIM_H */
