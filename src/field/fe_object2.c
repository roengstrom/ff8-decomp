#include "common.h"
#include "field/fe_object2.h"

typedef struct {
    /* 0x00 */ s32 tag;
    /* 0x04 */ u32 size;
    /* 0x08 */ u8 *src;
    /* 0x0C */ u8  pad0C[0x64];
} SoundBufDesc; /* 0x70 = 112 bytes */

extern u8 *D_800D6198[3];
extern SoundBufDesc D_800D61A8[3];

/**
 * Pack three source buffers from their original addresses into a
 * contiguous arena starting at @c 0x80180000. For each slot, validate
 * that the source range fits below the arena base; if not, mark the
 * slot invalid (@c tag = -1). Otherwise, when the slot is already
 * valid, memcpy the source into the next arena position, record the
 * destination in @c D_800D6198[], and advance the running base by the
 * slot's size. Returns the final running base (top of packed data).
 *
 * @return Pointer just past the last packed buffer.
 */
u8 *func_800ACB10(void) {
    s32 i;
    u8 *base = (u8 *)0x80180000;

    for (i = 0; i < 3; i++) {
        SoundBufDesc *d = &D_800D61A8[i];
        if ((u32)d->src + d->size > 0x80180000) {
            d->tag = -1;
        } else {
            if (d->tag != -1) {
                func_80039678(base, d->src, d->size);
            }
            D_800D6198[i] = base;
            base += d->size;
        }
    }
    return base;
}

/**
 * Empty function — returns immediately. Likely an unused dispatch slot
 * or a stub left in place to preserve a function-table layout.
 */
void func_800ACBCC(void) {
}

INCLUDE_ASM("asm/field/nonmatchings/fe_object2", func_800ACBD4);

INCLUDE_ASM("asm/field/nonmatchings/fe_object2", func_800AD048);

INCLUDE_ASM("asm/field/nonmatchings/fe_object2", func_800AD0E8);

INCLUDE_ASM("asm/field/nonmatchings/fe_object2", func_800AD300);

INCLUDE_ASM("asm/field/nonmatchings/fe_object2", func_800AD570);

INCLUDE_ASM("asm/field/nonmatchings/fe_object2", func_800AD6E0);
