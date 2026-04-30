#include "common.h"
#include "field.h"

/**
 * @brief Subset of the @c D_800562C4 context exposing the per-sfx-slot
 * state mask bytes at +0xD2 / +0xD3.
 *
 * Each bit indexed by an SFX slot id (0..7); bits are set when an SFX
 * is active and cleared when it completes.
 */
typedef struct {
    u8 pad00[0xD2];
    u8 field_0xD2;     /**< 0xD2: per-slot active mask (set on play, cleared on completion). */
    u8 field_0xD3;     /**< 0xD3: per-slot start mask (set on play). */
} SfxStateMaskCtx;

extern SfxStateMaskCtx *D_800562C4;
extern u8 *D_800704C0;
extern s32 D_800DE4DC;

extern s32 getSfxGlobalFlag(void);
extern void setSfxGlobalFlag(s32 idx);
extern void startSfxSlow(s32 idx);
extern void fadeOutSfxSlow(s32 idx);
extern s32 getSfxField1C(s32 idx);
extern s32 func_8002CE84(s32 idx);
extern u8 *func_8003974C(u8 *base, s32 idx);
extern s32 func_8002E680(u8 *text);
extern void func_8002E064(s32 idx, s16 *rect);
extern void func_8002D784(s32 a0, u8 *text, s32 a2, s32 a3, s32 a4, s32 a5);
extern void func_800BC258(s16 *rect);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB2A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB3D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB510);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB5E0);

s32 func_800BB650(FieldEntity *entity) { s16 buf[4]; func_800A8DAC(*((u8 *)entity + 0x256), 0x20, (u8 *)buf, 0); *((u16 *)((u8 *)entity + 0x228)) = 0; *((u16 *)((u8 *)entity + 0x22C)) = buf[1] / 16; *((u16 *)((u8 *)entity + 0x230)) = buf[2] / 16; return 2; }

/**
 * @brief Copy 12 animation halfwords from D_800704A8 table to entity.
 *
 * Copies halfwords at D_800704A8 offsets 0x108-0x11E to the entity
 * at offsets 0xD8-0xEE (matching pairs: src+0x108->dst+0xD8, etc).
 */
void func_800BB6C8(void) {
    extern u8 D_800704A8[];
    extern u8 D_800562C4[];
    u8 *dst = (u8 *)*(s32 *)D_800562C4;
    u8 *src = D_800704A8;

    *(u16 *)(dst + 0xD8) = *(u16 *)(src + 0x108);
    *(u16 *)(dst + 0xDC) = *(u16 *)(src + 0x10C);
    *(u16 *)(dst + 0xDA) = *(u16 *)(src + 0x10A);
    *(u16 *)(dst + 0xDE) = *(u16 *)(src + 0x10E);
    *(u16 *)(dst + 0xE0) = *(u16 *)(src + 0x110);
    *(u16 *)(dst + 0xE2) = *(u16 *)(src + 0x112);
    *(u16 *)(dst + 0xE4) = *(u16 *)(src + 0x114);
    *(u16 *)(dst + 0xE6) = *(u16 *)(src + 0x116);
    *(u16 *)(dst + 0xE8) = *(u16 *)(src + 0x118);
    *(u16 *)(dst + 0xEA) = *(u16 *)(src + 0x11A);
    *(u16 *)(dst + 0xEC) = *(u16 *)(src + 0x11C);
    *(u16 *)(dst + 0xEE) = *(u16 *)(src + 0x11E);
}

s32 func_800BB768(void) { extern u8 D_800DE8D2; extern u8 D_800704A8[]; u8 *src = D_800704A8; D_800DE8D2 = 2; *(u16 *)(src + 0x108) = 2; *(u16 *)(src + 0x10A) = 0xFF; *(u16 *)(src + 0x10C) = 0x10; *(u16 *)(src + 0x10E) = 0xFF; *(u16 *)(src + 0x110) = 0xFF; *(u16 *)(src + 0x112) = 0xFF; func_800BB6C8(); return 2; }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB7BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB810);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB888);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB8B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB90C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB958);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BB9A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBA3C);


INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBB20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBBB4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBC08);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBC64);

s32 func_800BBDA8(void) { extern u8 D_800704A8[]; extern u8 D_800562C4[]; u8 *src = D_800704A8; if (*(u16 *)(src + 0x10C) != *(u16 *)(src + 0x10A)) { return 1; } *(u16 *)((u8 *)*(s32 *)D_800562C4 + 0xD8) = *(u16 *)(src + 0x108); return 2; }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBDE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBE50);

s32 func_800BBE78(void) { extern u8 D_800704A8[]; extern u8 D_800562C4[]; u8 *src = D_800704A8; *(volatile u16 *)(src + 0x108) = 4; *(u16 *)((u8 *)*(s32 *)D_800562C4 + 0xD8) = *(volatile u16 *)(src + 0x108); return 2; }

/**
 * Pops two parameters from the stack and calls func_8002E1B4(val2 & 7, val1).
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BBEA4(u8 *a0) {
    u8 idx;
    s32 val1, val2;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    val1 = *(s32 *)(a0 + (s8)idx * 4);
    *(u8 *)(a0 + 0x184) = idx - 2;
    val2 = *(s32 *)(a0 + (s8)(idx - 1) * 4);
    func_8002E1B4(val2 & 7, val1);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BBEFC);

/**
 * Reads two parameters from the stack using the current index at 0x184,
 * calls setSfxPitch with them, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BBFFC(u8 *a0) {
    s8 idx;

    idx = *(s8 *)(a0 + 0x184);
    setSfxPitch(*(s32 *)(a0 + idx * 4 - 4), *(s32 *)(a0 + idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC034);

void func_800BC12C(s32 a0, s32 a1, u8 *a2) { extern u8 D_80085300[]; u8 *base = D_80085300; u8 *entry; a0 <<= 4; entry = base + a0; *(s32 *)(entry + 0x8) = a1; *(u16 *)(entry + 0x0) = *(u16 *)(a2 + 0x0); *(u16 *)(entry + 0x2) = *(u16 *)(a2 + 0x2); *(u16 *)(entry + 0x4) = *(u16 *)(a2 + 0x4); *(u16 *)(entry + 0x6) = *(u16 *)(a2 + 0x6); }

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC170);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC258);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC2E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC44C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC58C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BC6F0);

/**
 * @brief Field-VM SFX trigger / state-machine handler.
 *
 * Reads 8 stack params (top two as packed u16 halfwords for an x/y rect
 * source, six s32 args including the SFX slot id, a text/data id, and
 * four playback parameters). When the entity's @c activeMask bit for the
 * current @c scriptGroup is set:
 *   - Returns 5 if the SFX slot is already active (bit set in @c field_0xD2).
 *   - Otherwise, saves the current global SFX flag, resolves a text pointer,
 *     measures it via @c func_8002E680, fills the rect's bottom-right corner
 *     (+0x30 X, +0x11 Y), runs the display setup chain, kicks off the slow
 *     SFX, and sets the slot's bits in both @c field_0xD2 and @c field_0xD3.
 * Otherwise, runs a 2-state machine on @c field_0x204:
 *   - State 0: set the global SFX flag, query remaining duration
 *     (@c func_8002CE84), store to @c result; if non-negative, fade out and
 *     advance the state.
 *   - State 1: when the SFX has fully released (@c getSfxField1C == 0),
 *     clear the slot's bit in @c field_0xD2, pop 8 stack slots, restore
 *     the saved SFX flag, and return 3.
 *
 * @return 1 while still working, 3 when the state-1 release completes,
 *         5 when the slot was already active.
 */
s32 func_800BC8CC(Eline *e) {
    s16 buf[4];
    s32 sfxIdx;
    s32 textIdx;
    s32 paramY;
    s32 paramZ;
    s32 paramW;
    s32 paramV;
    s32 *stack;
    u8 *text;
    s32 dims;
    s32 r;
    s32 state;

    stack = (s32 *)e;
    buf[1] = *(u16 *)&stack[e->stackPtr];
    buf[0] = *(u16 *)&stack[e->stackPtr - 1];
    paramV = stack[e->stackPtr - 2];
    paramW = stack[e->stackPtr - 3];
    paramZ = stack[e->stackPtr - 4];
    paramY = stack[e->stackPtr - 5];
    textIdx = stack[e->stackPtr - 6];
    sfxIdx = stack[e->stackPtr - 7];

    if ((e->activeMask >> e->scriptGroup) & 1) {
        if ((D_800562C4->field_0xD2 >> sfxIdx) & 1) {
            return 5;
        }
        D_800DE4DC = getSfxGlobalFlag();
        text = func_8003974C(D_800704C0, textIdx);
        dims = func_8002E680(text);
        buf[2] = (dims & 0xFFFF) + 0x30;
        buf[3] = (dims >> 16) + 0x11;
        func_800BC258(buf);
        func_8002E064(sfxIdx, buf);
        func_8002D784(sfxIdx, text, paramY, paramZ, paramW, paramV);
        startSfxSlow(sfxIdx);
        e->field_0x204 = 0;
        D_800562C4->field_0xD3 |= (1 << sfxIdx);
        D_800562C4->field_0xD2 |= (1 << sfxIdx);
    } else {
        state = e->field_0x204;
        switch (state) {
        case 0:
            setSfxGlobalFlag(sfxIdx);
            r = func_8002CE84(sfxIdx);
            e->field_0x140 = r;
            if (r >= 0) {
                fadeOutSfxSlow(sfxIdx);
                e->field_0x204++;
            }
            break;
        case 1:
            if (getSfxField1C(sfxIdx) == 0) {
                D_800562C4->field_0xD2 &= ~(state << sfxIdx);
                e->stackPtr -= 8;
                setSfxGlobalFlag(D_800DE4DC);
                return 3;
            }
            break;
        }
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCB14);

/**
 * Pops a parameter and calls setSfxGlobalFlag, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BCC6C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    setSfxGlobalFlag(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCCAC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCDA0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCE44);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BCECC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object9", func_800BD024);

/**
 * @brief Pop value, clear D_80085398 table entry, call sound handler.
 *
 * Pops the stack to get an index, clears the halfword at D_80085398[idx * 16],
 * then calls clearAnimEntryActive with the popped value.
 *
 * @param entity Script entity context.
 * @return 2.
 */
s32 func_800BD1A4(FieldEntity *entity) {
    extern u8 D_80085398[];
    u8 *a0 = (u8 *)entity;
    u8 idx = entity->stackIdx;
    s32 val;

    entity->stackIdx = idx - 1;
    val = *(s32 *)(a0 + (s8)idx * 4);
    *(u16 *)(D_80085398 + val * 16) = 0;
    clearAnimEntryActive(val);
    return 2;
}
