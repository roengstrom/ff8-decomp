#include "common.h"

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A01DC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A0388);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A05E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A1540);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A1678);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A1F10);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A2350);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A246C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A25F8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A26E8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A2920);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A2D50);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A358C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A3870);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A39BC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A3C9C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A3EE4);

typedef struct {
    s32 val;    /* +0x00 */
    s16 hval;   /* +0x04 */
    s16 pad;    /* +0x06 */
} FeaEntry40C0; /* size 0x08 */

extern FeaEntry40C0 D_800D24A8[12];

/** Clears an array of 12 entries. */
void func_800A40C0(void) {
    FeaEntry40C0 *ptr = D_800D24A8;
    FeaEntry40C0 *end = ptr + 12;
    while (ptr < end) {
        ptr->val = 0;
        ptr->hval = 0;
        ptr++;
    }
}

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A40F8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A41E0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4420);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A45D8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4670);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4700);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A475C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A47A4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A4EF4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A50A0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A568C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A581C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A58EC);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5A3C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5B48);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5D10);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5D8C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5DC8);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5E40);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5EC4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5F78);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A5FD4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6030);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A60B4);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A610C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6188);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6254);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A629C);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A62E0);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A6358);

INCLUDE_ASM("asm/ovl/world_engine/nonmatchings/we_object3", func_800A63F0);
