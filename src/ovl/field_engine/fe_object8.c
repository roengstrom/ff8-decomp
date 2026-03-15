#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9078);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B90C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9108);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B912C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B91B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B91D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9288);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9290);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9488);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B94C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9570);

/**
 * Returns 2 if bit 0x800 is set in the flags at offset 0x160, otherwise 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if flag 0x800 is set, else 1.
 */
s32 func_800B95A0(u8 *a0) {
    if (*(s32 *)(a0 + 0x160) & 0x800) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B95C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9604);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9678);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B96EC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9798);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9844);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9888);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B98CC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9944);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B99BC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9A00);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9A78);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9B24);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9C58);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9CBC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9D20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9D7C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9F28);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9F58);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9F88);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA034);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA09C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA120);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA1D0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA280);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA330);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA3E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA424);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA4D4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA584);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA634);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA6E4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA7DC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA8D4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA9E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BAAFC);

/**
 * Returns 2 if the byte at offset 0x244 equals 3, otherwise returns 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if object byte 0x244 is 3, else 1.
 */
s32 func_800BABFC(u8 *a0) {
    if (*(u8 *)(a0 + 0x244) == 3) {
        return 2;
    }
    return 1;
}

/**
 * Returns 2 if the halfwords at offsets 0x234 and 0x236 are equal,
 * otherwise returns 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if values match, else 1.
 */
s32 func_800BAC18(u8 *a0) {
    if (*(u16 *)(a0 + 0x234) == *(u16 *)(a0 + 0x236)) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BAC38);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BAD00);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BADCC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BAF14);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BB05C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BB140);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BB1F0);
