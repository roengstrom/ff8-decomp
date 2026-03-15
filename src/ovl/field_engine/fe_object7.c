#include "common.h"

/**
 * Pops a parameter, calls func_80023C08, stores result at offset 0x140.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B542C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + 0x140) = func_80023C08(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B5480);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B574C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B578C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B57E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B5990);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B5A30);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6210);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B629C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B62E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6328);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6364);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B63A4);

/**
 * Calls func_800C03F4 and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B6400(u8 *a0) {
    func_800C03F4(a0);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6420);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6448);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6478);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B64B0);

/**
 * Loads global byte D_80082C0F into the object result field at offset 0x140,
 * returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B6524(u8 *a0) {
    extern u8 D_80082C0F;

    *(s32 *)(a0 + 0x140) = D_80082C0F;
    return 2;
}

/**
 * If D_800DE8D0 is nonzero, sets bit 0x400 in the flags at D_800562C4+0x68.
 * Otherwise, clears byte 0xCF of the entity. Returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B653C(u8 *a0) {
    extern u8 D_800DE8D0;
    extern u8 *D_800562C4;

    if (D_800DE8D0 != 0) {
        *(s32 *)(D_800562C4 + 0x68) = *(s32 *)(D_800562C4 + 0x68) | 0x400;
    } else {
        *(u8 *)(D_800562C4 + 0xCF) = 0;
    }
    return 2;
}

/**
 * Sets byte 0xCF of the entity pointed to by D_800562C4 to 1,
 * and clears bit 0x400 in the flags at offset 0x68, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B6588(u8 *a0) {
    extern u8 *D_800562C4;
    u8 *ptr = D_800562C4;

    *(u8 *)(ptr + 0xCF) = 1;
    *(s32 *)(ptr + 0x68) = *(s32 *)(ptr + 0x68) & ~0x400;
    return 2;
}

/**
 * Returns 2, indicating continue processing.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B65B0(u8 *a0) {
    return 2;
}

/**
 * Sets the global byte D_800704A8 to 4, returns 1.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 1.
 */
s32 func_800B65B8(u8 *a0) {
    extern u8 D_800704A8;
    D_800704A8 = 4;
    return 1;
}

/**
 * Returns 1, indicating wait/yield.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 1.
 */
s32 func_800B65CC(u8 *a0) {
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B65D4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B663C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B66A8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6738);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B67F4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6854);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B68B8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B68EC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B69E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6B20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6C28);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6D24);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6E18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B6F4C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7050);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B711C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7228);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7310);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B73D4);

/**
 * Returns 2 if the halfword at offset 0x21E equals 2, otherwise returns 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if value at 0x21E is 2, else 1.
 */
s32 func_800B7490(u8 *a0) {
    if (*(s16 *)(a0 + 0x21E) == 2) {
        return 2;
    }
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B74B0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7578);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7640);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7674);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B76A4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7718);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B788C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B79C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7D44);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B7E78);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8344);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B83FC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B84D8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B85C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B85F8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8608);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8644);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8710);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8824);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B89C0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8B58);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8BE0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8CD4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8DC8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8E74);

/**
 * Returns 2 if the byte at offset 0x245 equals 3, otherwise returns 1.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 if object byte 0x245 is 3, else 1.
 */
s32 func_800B8F20(u8 *a0) {
    if (*(u8 *)(a0 + 0x245) == 3) {
        return 2;
    }
    return 1;
}

/**
 * Sets the global byte D_8007064C to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B8F3C(u8 *a0) {
    extern u8 D_8007064C;
    D_8007064C = 1;
    return 2;
}

/**
 * Clears the global byte D_8007064C, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B8F50(u8 *a0) {
    extern u8 D_8007064C;
    D_8007064C = 0;
    return 2;
}

/**
 * Calls func_8009E660 and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B8F60(u8 *a0) {
    func_8009E660(a0);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8F80);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B8FA8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B9000);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B9030);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object7", func_800B9034);
