#ifndef FE_OBJECT10_H
#define FE_OBJECT10_H

#include "common.h"
#include "field.h"

extern s32  opHandler_DISABLEANGELO(ScriptContext *context);
extern void func_800BD250(s32 dir, s16 *out);
extern s32  opHandler_OP167(Actor *actor);
extern s32  opHandler_OP168(Actor *actor);
extern void updateSeedLevel(void);
extern void func_800BD5E0(void);
extern void func_800BD64C(void);
extern void func_800BD6EC(void);
extern void func_800BD794(void);
extern s32  func_800BE264(void);
extern s32  func_800BE274(void);
extern void func_800BE2AC(void);
extern void func_800BE2DC(void);
extern void func_800BE30C(u8 *header);
extern s32  func_800BE44C(s32 val);
extern void func_800BF230(Actor *actor);
extern void func_800BF4A4(void);

extern void func_800BD804(s32 stepDelta);

extern void func_800BD9C4(FieldFrameBuf *frame);

/* INCLUDE_ASM stubs — bodies still in assembly, signatures unknown.
 * Declared K&R-style; refine when these get decomped to C. */
extern Actor *func_800BE36C(u8 *header);
extern s32 *func_800BE4B0(u8 *header, u16 *table);
extern Actor *InitActors(Actor *buf);
extern Eline *func_800BE7F4(Eline *buf);
extern Bganime *func_800BE924(Bganime *buf);
extern Dline *func_800BEA84(Dline *buf);
extern void func_800BEBD0(void);
extern void func_800BF080(void);
extern void func_800BF28C(s32 dispatchEnabled);

#endif
