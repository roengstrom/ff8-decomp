/**
 * @file fe_object1_2.h
 * @brief Prototypes owned by fe_object1_2.c, the field unit's second half.
 *
 * The struct views inside fe_object1_2.c stay file-private on purpose: each is
 * a provisional view of one function's argument, named after that function,
 * and exporting them would spread types that exist only until the real
 * structures are mapped.
 *
 * fe_object6.c deliberately does not include this: its one call into the unit
 * (func_800A5A14) relies on an implicit declaration that the original codegen
 * depends on, and giving it a prototype changes the field overlay's output.
 */
#ifndef FIELD_FE_OBJECT1_2_H
#define FIELD_FE_OBJECT1_2_H

#include "common.h"
#include "field.h"
#include "field/fe_object1.h"

extern void func_800A17A4(u8 *a0);
extern void func_800A1C64(void);
extern void func_800A1CC0(void);
extern void func_800A2EE0(FieldParticles *particles);
extern void func_800A2F28(FieldParticles *particles, SystemState *sys);
extern void func_800A303C(s16 emIdx, ParticleSystem *sys, s16 *pos, s16 count);
extern void func_800A355C(ActorAnim *actor, s32 slot, s32 a2);
extern void func_800A44D8(void);
extern void func_800A4550(s16 a0);
extern s32  func_800A4910(s32 a0, s32 a1, s32 a2, s32 a3);
extern void func_800A59D0();  /* K&R: a0 declared but ignored in body; callers vary 0/1-arg */
extern void func_800A5A14(s16 a0);
extern s32  func_800A5CF8(void);
extern void func_800A10F4(void);
extern void func_800A11E0(Vec2s *out, s16 slotIdx);
extern void func_800A1318(void);
extern void func_800A15C0(FieldFrameBuf *buf, DRAWENV *env, s16 slotIdx);
void func_800A17B8(Oscillator *osc);
extern void func_800A1BB8(void);
extern void func_800A1CFC(Actor *ents, FieldFrameBuf *frame);
extern void func_800A2128();  /* arg is a file-private buffer view in fe_object1.c */
extern void func_800A222C(u32 *ot, MATRIX *m, POLY_G3 *prim, DR_TPAGE *tp, Actor *ents);
extern func_800A29C0_arg0 *func_800A29C0(func_800A29C0_arg0 *p);
extern func_800A2A30_item *func_800A2A30(func_800A2A30_item *p);
extern void func_800A2D2C(s16 *buf, s32 slot);
extern s16  func_800A2EA4(s16 range);
extern s16  func_800A2FE0();  /* arg is a file-private buffer view in fe_object1.c */
extern void func_800A327C();  /* arg0 is a file-private Actor-stack view in fe_object1.c */
extern void func_800A3488();  /* arg0 is a file-private Actor-stack view in fe_object1.c */
extern void func_800A37A8(MATRIX *m, FieldFrameBuf *frame, FieldParticles *buf);
extern void func_800A38B4(MoveAccum *out, MoveStep *in, MoveStep *target);
extern void func_800A39D8(MoveAccum *acc, MoveRecord *rec, FieldParticles *buf, u32 *ot);
extern void func_800A3FE0(FieldParticles *buf);
void func_800A42EC(POLY_G4 *polys, DR_TPAGE *tpages);
extern void func_800A4500(s32 x, s32 y, s32 z);
void func_800A455C(s16 entityIdx);
extern void func_800A4758(void);
extern s32  func_800A48CC(void);
extern void func_800A4934();  /* args are file-private ObjSlot/DrawPoint in fe_object1.c */
extern void func_800A4C14();  /* first arg is the file-private ObjSlot in fe_object1.c */
extern void func_800A5224(MATRIX *m, u32 *ot, FieldRibbonPrims *prims,
                          FieldRibbonTPages *tpages);
extern void func_800A5360(u32 *ot, s16 r, s16 g, s16 b);
extern void func_800A553C(u32 *ot, s16 r, s16 g, s16 b);
extern void func_800A5698(void);
extern void func_800A5700(void);
extern s16  func_800A5748(s16 start, s16 end, s16 progress, s16 total);
extern void func_800A5788(FieldFrameBuf *buf);
extern void func_800A5898(FieldFrameBuf *buf);
extern void func_800A5A20(Actor *actor, FieldGateway *gateways);
extern s32  func_800A5C9C(void);
extern void func_800A5D28(void);

#endif /* FIELD_FE_OBJECT1_2_H */
