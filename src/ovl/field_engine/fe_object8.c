#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9078);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B90C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9108);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B912C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B91B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B91D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9288);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9290);

/**
 * Copies several entity fields: 0x160->0x20E, 0x24E->0x24D,
 * 0x206->0x210, 0x208->0x212, 0x20A->0x214, 0x20C->0x216.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B9488(u8 *a0) {
    *(u16 *)(a0 + 0x20E) = *(u16 *)(a0 + 0x160);
    *(u8 *)(a0 + 0x24D) = *(u8 *)(a0 + 0x24E);
    *(u16 *)(a0 + 0x210) = *(u16 *)(a0 + 0x206);
    *(u16 *)(a0 + 0x212) = *(u16 *)(a0 + 0x208);
    *(u16 *)(a0 + 0x214) = *(u16 *)(a0 + 0x20A);
    *(u16 *)(a0 + 0x216) = *(u16 *)(a0 + 0x20C);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B94C0);

/**
 * Pops a halfword from the stack and stores it to offset 0x208.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B9570(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u16 *)(a0 + 0x208) = *(u16 *)(a0 + (s8)idx * 4);
    return 2;
}

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

/**
 * Call func_800B912C with entity byte 0x24F, set bit 0x2000 in flags. Returns 3.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 3.
 */
/**
 * Call func_800B912C with entity and byte 0x24F, set bit 0x2000 in flags. Returns 3.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 3.
 */
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

/**
 * Pops a byte from the stack and stores it to offset 0x261.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B9F28(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u8 *)(a0 + 0x261) = *(u8 *)(a0 + (s8)idx * 4);
    return 2;
}

/**
 * Pops a byte from the stack and stores it to offset 0x241.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B9F58(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u8 *)(a0 + 0x241) = *(u8 *)(a0 + (s8)idx * 4);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800B9F88);

/**
 * Pops a value from the stack, looks up a pointer in D_80085230[val],
 * calls func_8009E604, and stores the byte result at offset 0x241.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800BA034(u8 *a0) {
    extern u8 D_80085230[];
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u8 *)(a0 + 0x241) = func_8009E604(a0, *(s32 *)(D_80085230 + *(s32 *)(a0 + (s8)idx * 4) * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA09C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA120);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA1D0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA280);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object8", func_800BA330);

/**
 * Adjust halfword at 0x1DE based on distance from 0x1DC.
 * If abs(0x1DE - 0x1DC) >= 0x81, adjusts by +/- 0x100.
 * Also clears byte at 0x243.
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800BA3E0(u8 *a0) {
    s16 de = *(s16 *)(a0 + 0x1DE);
    s16 dc = *(s16 *)(a0 + 0x1DC);
    u16 deu = *(u16 *)(a0 + 0x1DE);
    s32 diff = de - dc;
    if (diff < 0) {
        diff = -diff;
    }
    *(u8 *)(a0 + 0x243) = 0;
    if (diff >= 0x81) {
        if (dc < de) {
            *(u16 *)(a0 + 0x1DE) = deu - 0x100;
        } else {
            *(u16 *)(a0 + 0x1DE) = deu + 0x100;
        }
    }
}

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
