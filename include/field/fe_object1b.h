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

typedef struct {
    /** 0x00 */ s32 unk00;
    /** 0x04 */ u32 unk04;
    /** 0x08 */ s32 unk08;
    /** 0x0C */ s32 unk0C;
    /** 0x10 */ s32 unk10;
    /** 0x14 */ s32 unk14;
    /** 0x18 */ u16 unk18;
    /** 0x1A */ u16 unk1A;
    /** 0x1C */ u16 unk1C;
    /** 0x1E */ u16 unk1E;
    /** 0x20 */ s32 unk20;
    /** 0x24 */ s32 unk24;
    /** 0x28 */ s32 unk28;
    /** 0x2C */ s32 unk2C;
    /** 0x30 */ s32 unk30;
    /** 0x34 */ s32 unk34;
    /** 0x38 */ u16 *unk38;
    /** 0x3C */ u32 unk3C;
} func_800A7564_arg0;

extern void func_800A63AC(FieldFrameBuf *arg0, FieldView *arg1, s32 arg2);
extern void func_800A6A80(s32 arg0, s32 arg1, ModelInstance **arg2);
extern void func_800A7194(void);
extern void func_800A7224(s32 idx, u16 *vals, s32 mode);
extern void func_800A736C(s32 idx, u16 *vals, s32 mode);
extern void func_800A74B4(s32 idx, EntityRenderXform *vals, s32 mode);
extern void *func_800A7564(func_800A7564_arg0 *arg0, s32 arg1, s32 arg2, s32 arg3);
extern void *func_800A8058(s32 idx, func_800A7564_arg0 *arg1, Model *newObj, u8 count);
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
