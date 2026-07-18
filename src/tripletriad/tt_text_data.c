#include "common.h"
#include "psxsdk/libgpu.h"
#include "tripletriad.h"
#include "tripletriad/tt_main.h"
#include "tripletriad/tt_text.h"

/* Initialized data for tt_text.c: the debug/message text engine state, the
   tetrahedral 3D icon model, and the card-flip transform scratch. First block
   of the overlay's shared .data region (0x80182B54..). Definition order is
   load-bearing — it reproduces the original image layout. */

/* -------- debug/message text engine state -------- */

s16 g_textBufferIndex = 0;
s16 g_textCursorX = 0;
s16 g_textCursorY = 8;
s16 g_textLineX = 0;
RGB g_textColor = {0xff, 0xff, 0xff};

/** @brief '0'..'8' escape-code text colours (r|g|b|0 words, indexed digit - '0'). */
u32 g_textPalette[9] = {
    0x00000000, 0x00802020, 0x00202080, 0x00802080, 0x00208020,
    0x00808020, 0x00208080, 0x00808080, 0x00404040
};
u8 g_hexDigits[20] = "0123456789ABCDEF";

/* -------- tetrahedral 3D icon model -------- */

SVECTOR g_triadIconVerts[4] = {
    {0x0, 0x8, 0x0, 0x0}, {0x0, -0x8, 0x8, 0x0},
    {-0x7, -0x8, -0x4, 0x0}, {0x7, -0x8, -0x4, 0x0}
};

TriadFaceDesc g_triadIconFaces[4] = {
    {0, 1, 2, 0, 0x30004040, 0x0000FFFF, 0x0000A0A0},
    {0, 2, 3, 0, 0x30004040, 0x0000FFFF, 0x0000A0A0},
    {0, 3, 1, 0, 0x30004040, 0x0000FFFF, 0x0000A0A0},
    {1, 3, 2, 0, 0x3000FFFF, 0x0000FFFF, 0x0000FFFF}
};

/* -------- card-flip transform scratch -------- */

SVECTOR g_cardFlipUpVec = {0x0, 0x0, 0x100, 0x0};
SVECTOR g_cardFlipTarget = {0x0, -0x5C, 0x200, 0x0};
SVECTOR g_cardFlipAngles = {0x0, 0x0, 0x0, 0x0};

/** @brief Eight packed r|g|b|0 colours. @note Not referenced by any decompiled code. */
static u32 D_80182C10[8] = {
    0x000000FF, 0x00FFFF00, 0x0000FFFF, 0x00000000,
    0x0000FF00, 0x00808080, 0x00FF0000, 0x00FFFFFF
};
