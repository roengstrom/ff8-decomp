#include "common.h"
#include "tripletriad.h"
#include "tripletriad/tt_script.h"

/* Initialized data for tt_script.c: the gradient-fade dispatch table and its
   staged fade state. Third block of the overlay's shared .data region.
   Definition order is load-bearing. */

/* -------- gradient-fade dispatch + staged state -------- */

ObjNodeFn g_gradFadeCallbacks[6] = {
    (ObjNodeFn)&updateCardScaleSprite,
    (ObjNodeFn)&updateCardScaleSprite,
    (ObjNodeFn)&updateCardColorFade,
    (ObjNodeFn)&buildGradientFade,
    (ObjNodeFn)&buildGradientFade,
    (ObjNodeFn)&updateCardScaleSpriteShort
};

s32 g_lastActiveSlot = -1;
u8 g_stagedFadeColor[4] = {0xff, 0xff, 0xff, 0};

/** @note Not referenced by any decompiled code. */
static s32 D_80182E6C = 0x3B;
