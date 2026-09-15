#include "common.h"

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BF80C);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BFBFC);
#include "world.h"
#include "world/we_object10.h"

extern s32 D_800DD8F8[0x20];
extern void func_800C3278(s32 arg0);
extern s32 D_800E1D58[];
extern MATRIX D_800C5DB8;
extern MATRIX D_800C5DD8;
extern void func_800408A4(s32 a, s32 b, s32 c);
extern void func_8003F234(s32 a, s32 b);
extern void func_8003F294(s32 a, s32 b);
extern void func_800406D4(MATRIX *m);

void func_800BFCC0(unsigned long long arg0) {
    s32 i;
    s32 *dst;
    s32 *src;

    i = 0;
    dst = D_800DD8F8;
    src = (s32 *)D_800DDB00;
    do {
        s32 v = *src;
        src++;
        i++;
        *dst = v;
        dst++;
    } while (i < 0x20);
    func_800C3278(arg0 + 4);
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800BFD10);
/**
 * @brief Clear world lookup tables and reset GTE colour/fog state.
 *
 * Zeroes 64 words of @c D_800E1D58 and 32 words of @c D_800DDB00 via
 * countdown loops, then reprograms fog, depth cue, colour matrix and
 * light colour.
 */
void func_800BFFEC(void) {
    s32 i;
    s32 *p;

    i = 0x3F;
    p = D_800E1D58;
    p += 0x3F;
    do {
        *p = 0;
        i -= 1;
        p -= 1;
    } while (i >= 0);

    i = 0x1F;
    p = (s32 *)D_800DDB00;
    p += 0x1F;
    do {
        *p = 0;
        i -= 1;
        p -= 1;
    } while (i >= 0);

    func_800408A4(0, 0x3C, 0x78);
    func_8003F234(0x800, 0x200);
    func_8003F294(0x200, 0x200);
    SetColorMatrix(&D_800C5DB8);
    func_800406D4(&D_800C5DD8);
}

/** @brief 8-byte (2-aligned) block copied into a LookupTarget unk10/unk18
 *         region by mode-0 paths. 2-byte alignment forces lwl/lwr/swl/swr. */
typedef struct {
    u16 a, b, c, d;
} WorldSlotVec4;

/**
 * @brief Per-mode update of a LookupTarget's unk10 vector, then clear unk60 bits.
 *
 * World twin of field func_800A7224 without the snapshot compare:
 *  - mode 0: overwrite the 8-byte unk10 block from vals.
 *  - mode 1: add vals[0..2] into unk10 / unk12 / unk14.
 *  - other modes: no-op (no flag clear).
 * Modes 0/1 then clear bits 0x18 of unk60.
 */
void func_800C0084(s32 idx, u16 *vals, s32 mode) {
    LookupTarget *slot;

    switch (mode) {
    case 0:
        *(WorldSlotVec4 *)&D_800DDB00[idx]->unk10 = *(WorldSlotVec4 *)vals;
        slot = D_800DDB00[idx];
        goto clear_flags;
    case 1:
        D_800DDB00[idx]->unk10 += vals[0];
        D_800DDB00[idx]->unk12 += vals[1];
        D_800DDB00[idx]->unk14 += vals[2];
        slot = D_800DDB00[idx];
    clear_flags:
        slot->unk60 &= 0xE7;
        break;
    }
}

/**
 * @brief Twin of func_800C0084 for the unk18 vector (same switch + flag clear).
 *
 * World twin of field func_800A736C without the snapshot compare.
 */
void func_800C0154(s32 idx, u16 *vals, s32 mode) {
    LookupTarget *slot;

    switch (mode) {
    case 0:
        *(WorldSlotVec4 *)&D_800DDB00[idx]->unk18 = *(WorldSlotVec4 *)vals;
        slot = D_800DDB00[idx];
        goto clear_flags;
    case 1:
        D_800DDB00[idx]->unk18 += vals[0];
        D_800DDB00[idx]->unk1A += vals[1];
        D_800DDB00[idx]->unk1C += vals[2];
        slot = D_800DDB00[idx];
    clear_flags:
        slot->unk60 &= 0xE7;
        break;
    }
}

/**
 * @brief Per-mode write/accumulate of transform values into a world render slot.
 *
 * World twin of field @c func_800A74B4:
 *  - @c mode @c 0: overwrite the whole @c xform block from @p vals.
 *  - @c mode @c 1: add into @c field20/24/28 (@c field2C untouched).
 *  - any other @c mode: no-op.
 */
void func_800C0224(s32 idx, WorldRenderXform *vals, s32 mode) {
    LookupTarget *slot;

    switch (mode) {
    case 0:
        slot = D_800DDB00[idx];
        slot->xform = *vals;
        break;
    case 1:
        D_800DDB00[idx]->xform.field20 += vals->field20;
        D_800DDB00[idx]->xform.field24 += vals->field24;
        D_800DDB00[idx]->xform.field28 += vals->field28;
        break;
    }
}
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C02D8);
/**
 * @brief Claim a D_800E1D58 slot and hand off to @c func_800C02D8.
 *
 * If @c D_800E1D58[idx] is free, installs @p slot, clears @c unk44, and
 * forwards to @c func_800C02D8. Returns 0 when the slot is occupied.
 */
s32 func_800C0CA4(s32 idx, s32 arg1, s32 *slot) {
    s32 *base;
    s32 **slotp;

    base = D_800E1D58;
    slotp = (s32 **)&base[idx];
    if (*slotp != NULL) {
        return 0;
    }
    *slotp = slot;
    *(s32 *)((u8 *)slot + 0x44) = 0;
    return func_800C02D8(arg1, 0x11, idx, idx);
}
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C0D00);
/**
 * @brief Claim a world render-slot pool entry and initialize its state.
 *
 * World twin of field @c func_800A8CDC: acquires @c D_800DDB00[idx] if
 * currently @c NULL by installing @p slot and writing @p firstWord to the
 * slot's leading @c s32. Zeroes the usual state fields (except @c unk12 =
 * @c 0x190), sets the three scale factors to unit (@c 0x1000), and clears
 * @c unk60.
 *
 * @return @p slot @c + @c 0x98 on successful claim, or @c NULL if occupied.
 */
s32 *func_800C1718(s32 idx, s32 firstWord, LookupTarget *slot) {
    if (D_800DDB00[idx] != NULL) {
        return NULL;
    }
    D_800DDB00[idx] = slot;
    *(s32 *)slot = firstWord;
    D_800DDB00[idx]->unk60 = 0;
    D_800DDB00[idx]->unk10 = 0;
    D_800DDB00[idx]->unk12 = 0x190;
    D_800DDB00[idx]->unk14 = 0;
    D_800DDB00[idx]->unk18 = 0;
    D_800DDB00[idx]->unk1A = 0;
    D_800DDB00[idx]->unk1C = 0;
    D_800DDB00[idx]->xform.field20 = 0x1000;
    D_800DDB00[idx]->xform.field24 = 0x1000;
    D_800DDB00[idx]->xform.field28 = 0x1000;
    D_800DDB00[idx]->unk50 = 0;
    D_800DDB00[idx]->field52 = 0;
    D_800DDB00[idx]->unk60 = 0;
    return &slot->subBuffer;
}
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C17E4);
/**
 * @brief Resolve a per-mode pointer into a world render slot.
 *
 * Modes @c 0x1C / @c 0x1D share a @c dataPtr + (byte<<6) + 0x4C form
 * (@c unk70 vs @c unk71); @c 0x1E uses +0x68; @c 0x1F returns the
 * slot's @c +0x30 block.
 */
u8 *func_800C22B0(s32 idx, u32 mode) {
    register LookupTarget *slot __asm__("v1");
    register int n __asm__("v0");
    register u8 *base __asm__("v1");
    u8 *result;

    switch (mode) {
    case 0x1C:
        slot = D_800DDB00[idx];
        n = slot->unk70;
        goto shared;
    case 0x1D:
        slot = D_800DDB00[idx];
        n = slot->unk71;
shared:
        base = slot->dataPtr;
        n <<= 6;
        n += (int)base;
        result = (u8 *)n + 0x4C;
        break;
    case 0x1E:
        slot = D_800DDB00[idx];
        n = slot->unk71;
        base = slot->dataPtr;
        n <<= 6;
        n += (int)base;
        result = (u8 *)n + 0x68;
        break;
    case 0x1F:
        result = (u8 *)D_800DDB00[idx] + 0x30;
        break;
    }
    return result;
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C239C);
/**
 * @brief Mode-0xD setup: accumulate unk68 halfword pairs into a render slot.
 *
 * When the slot is live (@c firstWord != -1) and @p mode == @c 0xD, walks
 * @p count pairs from @c unk68 summing first-halfwords and products, then
 * writes @c unk0C/@c unk0E from the trailing pair, advances @c unk08, and
 * clears @c unk50.
 */
void func_800C2B04(s32 idx, s32 mode, u32 count) {
    register LookupTarget *slot __asm__("t3");
    register s32 pairIdx __asm__("t0");
    register s32 sum __asm__("t2");
    register s32 prodSum __asm__("t1");
    register u32 i __asm__("a3");
    register int p __asm__("a1");
    register int tmp __asm__("v0");
    register LookupTarget **slotp __asm__("a1");
    register LookupTarget *s __asm__("a0");
    register s32 off __asm__("v1");
    register LookupTarget **base __asm__("v0");
    u16 a;
    s32 lo;

    slot = D_800DDB00[idx];
    if (slot->firstWord != -1) {
        pairIdx = 2;
        if (mode == 0xD) {
            sum = 0;
            i = 0;
            prodSum = 0;
            if (count != 0) {
                tmp = (int)slot->unk68;
                p = tmp + 4;
                do {
                    a = *(u16 *)(p - 4);
                    lo = a * *(u16 *)(p - 2);
                    p += 4;
                    pairIdx += 2;
                    i += 1;
                    sum += a;
                    prodSum += lo;
                } while (i < count);
            }
            base = D_800DDB00;
            slotp = &base[idx];
            s = *slotp;
            off = pairIdx * 2;
            tmp = (int)s->unk68;
            tmp = off + tmp;
            s->unk0C = *(u16 *)(tmp - 4) * 0x10;
            s = *slotp;
            tmp = (int)s->unk68;
            off = off + tmp;
            s->unk0E = *(u16 *)(off - 2);
            s = *slotp;
            tmp = prodSum + sum;
            off = (s32)s->unk6C;
            tmp <<= 2;
            off += tmp;
            s->unk08 = off;
            (*slotp)->unk50 = 0;
        }
    }
}

INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C2C00);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3278);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3674);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3714);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3918);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3B74);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3CE4);
INCLUDE_ASM("asm/ovl/world/nonmatchings/we_object11", func_800C3DB0);
