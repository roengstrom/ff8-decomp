#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BA870);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BAC84);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BB150);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BB4E8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BBD74);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BBF0C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC09C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC218);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC44C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC46C);

/** Copies and negates vector components (rotation variant A). */
void func_800BC51C(s32 *src, s32 *dst) {
    dst[0] = src[0];
    dst[1] = -src[2];
    dst[2] = src[1];
}

/** Copies and negates vector components (rotation variant B). */
void func_800BC544(s32 *src, s32 *dst) {
    dst[0] = src[0];
    dst[1] = src[2];
    dst[2] = -src[1];
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC570);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC5E0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC688);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC6E8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC744);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC7D0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC82C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC8B8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC8D8);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BC974);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCA54);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCA74);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCC70);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCE74);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCF10);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCF30);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BCF84);

extern u32 D_80077E84;

/** Checks if a counter has reached threshold 0xBB8. */
s32 func_800BD040(void) {
    return D_80077E84 >= 0xBB8;
}

/**
 * @brief Subtract a clamped value from the HP-like counter at D_80077378+0xB0C.
 */
s32 func_800BD058(s32 amount) {
    extern u8 D_80077378[];
    s32 base = (s32)D_80077378;
    s32 max;

    if (amount >= 0) {
        max = *(s32 *)(base + 0xB0C);
        if ((u32)max < (u32)amount) {
            amount = max;
        }
    } else {
        amount = 0;
    }
    *(s32 *)(base + 0xB0C) = *(s32 *)(base + 0xB0C) - amount;
    return amount;
}

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD09C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD180);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD22C);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD2A0);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD380);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD460);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD540);

INCLUDE_ASM("asm/ovl/field_engine_alt/nonmatchings/fea_object9", func_800BD640);
