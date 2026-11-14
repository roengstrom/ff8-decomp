/**
 * @file fe_object1b.h
 * @brief Prototypes owned by fe_object1b.c, the part of the fe_object1 unit
 *        built with PsyQ 4.3.
 */
#ifndef FIELD_FE_OBJECT1B_H
#define FIELD_FE_OBJECT1B_H

#include "common.h"
#include "field.h"
#include "field/fe_object1.h"

extern void func_800A63AC(FieldFrameBuf *arg0, FieldView *arg1, s32 arg2);
extern void func_800A6A80(s32 arg0, s32 arg1, ModelInstance **arg2);
extern void func_800A7194(void);
extern void func_800A7224(s32 idx, u16 *vals, s32 mode);
extern void func_800A736C(s32 idx, u16 *vals, s32 mode);
extern void func_800A74B4(s32 idx, EntityRenderXform *vals, s32 mode);
extern void *func_800A7564(MchModelHeader *arg0, s32 arg1, s32 arg2, s32 arg3);
extern void *func_800A8058(s32 idx, MchModelHeader *arg1, Model *newObj, u8 count);
extern POLY_FT4 *func_800A81AC(s32 arg0, s32 arg1, ModelInstance *arg2);
extern s32 *func_800A8CDC(s32 idx, s32 firstWord, ModelInstance *slot);
extern void *func_800A8DAC(s32 spatialIdx, s32 cmd, void *arg, void *out);
extern void func_800A91C8(s32 arg0, s32 arg1, s32 arg2, SVECTOR *arg3);
extern s32 func_800A9434(s32 arg0, s32 arg1, u32 arg2, s16 *arg3, s32 arg4);
extern s32 func_800A97E4(s32 spatialIdx, s32 cmd, s32 arg2, s32 arg3);
extern void func_800AA46C(s32 spatialIdx, s32 cmd, u32 arg, s32 arg4);
/** @brief Per-entity animation tick: advances the frame and rebuilds the sprite rect. */
extern s32  func_800AA5F8(s32 idx);
extern int  func_800AA8A0();

#endif
