#include "common.h"
#include "psxsdk/libgpu.h"
#include "tripletriad/be_object1.h"

/**
 * @brief VRAM rect covering one draw buffer (@c initGraphics clears both
 *        buffers with it, moving @c x to @c TT_DRAW_W for the second).
 */
RECT g_fbClearRect = {0, 0, TT_DRAW_W, TT_SCREEN_H};

/**
 * @brief VRAM rect covering the 256x256 texture area below the draw buffers
 *        (@c initGraphics clears it to white before the TIM uploads).
 */
RECT g_texClearRect = {0, 256, 256, 256};
