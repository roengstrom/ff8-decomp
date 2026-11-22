#ifndef RENDER_H
#define RENDER_H

#include "common.h"

/* --- render flag bitmask (render.c) --- */

/** @brief Global render-flag bitmask cleared and inspected during transitions. */
extern s32 D_8008513C;

/** @brief Install @p a0 as the per-VSync callback (render.c). */
extern void setVsyncCallback(s32 a0);
/** @brief Install @p a0 as the per-frame draw callback (render.c). */
extern void setDrawCallback(s32 a0);

void setRenderFlag(u8 val);
void dispatchDrawCallback(void);
void SetModeBit0(void);

#endif /* RENDER_H */
