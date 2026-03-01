#ifndef LIBGPU_H
#define LIBGPU_H

#include "common.h"

/**
 * @brief GPU primitive tag (ordering table link).
 *
 * Bitfield layout matching the PS1 GPU OT format: 24-bit next-pointer
 * and 8-bit packet length.
 */
typedef struct {
    unsigned addr : 24;
    unsigned len : 8;
    u8 r0, g0, b0, code;
} P_TAG;

/* --- Ordering table macros --- */

#define setlen(p, _len)    (((P_TAG *)(p))->len = (u8)(_len))
#define getlen(p)          (u8)(((P_TAG *)(p))->len)
#define setaddr(p, _addr)  (((P_TAG *)(p))->addr = (u32)(_addr))
#define getaddr(p)         (u32)(((P_TAG *)(p))->addr)
#define setcode(p, _code)  (((P_TAG *)(p))->code = (u8)(_code))
#define getcode(p)         (u8)(((P_TAG *)(p))->code)

#define addPrim(ot, p)     setaddr(p, getaddr(ot)), setaddr(ot, p)
#define addPrims(ot, p0, p1) setaddr(p1, getaddr(ot)), setaddr(ot, p0)
#define termPrim(p)        setaddr(p, 0xFFFFFF)

/* --- GPU function declarations --- */

void ResetGraph(s32 mode);
void SetGraphDebug(s32 level);
s32 DrawSync(s32 mode);
void SetDispMask(s32 mask);
void ClearOTag(u32 *ot, s32 n);
void ClearOTagR(u32 *ot, s32 n);
void DrawOTag(void *p);
void PutDrawEnv(void *env);
void PutDispEnv(void *env);
void ClearImage(void *rect, u8 r, u8 g, u8 b);
void SetDrawStp(u32 *p, s32 dfe);

#endif /* LIBGPU_H */
