#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2348);

/**
 * Pops a parameter and calls func_8001344C, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B23F4(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_8001344C(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2434);

/**
 * Pops a parameter and calls func_80013538, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B248C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_80013538(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B24CC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2524);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B257C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B25F0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2648);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B26BC);

/**
 * Pops a parameter, calls func_80013210, stores result at offset 0x140.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B273C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + 0x140) = func_80013210(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2790);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B27C4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2864);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2A40);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2A70);

/**
 * Sets bit 0x80 in the flags at offset 0x160, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B2AC0(u8 *a0) {
    *(s32 *)(a0 + 0x160) = *(s32 *)(a0 + 0x160) | 0x80;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2AD8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2AF0);

/**
 * Sets bytes at offsets 0x188 and 0x189 to 0xFF, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B2B34(u8 *a0) {
    *(s8 *)(a0 + 0x188) = -1;
    *(s8 *)(a0 + 0x189) = -1;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2B48);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2BA0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2D40);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2DC0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2E68);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2EDC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2F50);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2F70);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B2FD8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B301C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3050);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3080);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B31B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3334);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3350);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B33B8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3474);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B348C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B34EC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3574);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B35FC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3650);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B36C8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3740);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3788);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B37F8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3868);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B388C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B38E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3964);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3A04);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3AA4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3B20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3BC0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3C60);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3CD0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3D60);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3DF0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3EA0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3F18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B3F9C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4074);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B414C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B417C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B41B0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B41CC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4288);

/**
 * Pops a parameter and calls func_8003023C, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B42E0(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_8003023C(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4320);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B43FC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B448C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B44BC);

/**
 * Pops a parameter and calls func_800A5A14, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B45CC(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_800A5A14(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B460C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B46E4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B47E4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B48EC);

/**
 * Clears the global byte D_8007064A, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B497C(u8 *a0) {
    extern u8 D_8007064A;
    D_8007064A = 0;
    return 2;
}

/**
 * Sets the global byte D_8007064A to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B498C(u8 *a0) {
    extern u8 D_8007064A;
    D_8007064A = 1;
    return 2;
}

/**
 * Sets the global byte D_8007064D to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B49A0(u8 *a0) {
    extern u8 D_8007064D;
    D_8007064D = 1;
    return 2;
}

/**
 * Clears the global byte D_8007064D, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B49B4(u8 *a0) {
    extern u8 D_8007064D;
    D_8007064D = 0;
    return 2;
}

/**
 * Sets the global byte D_8007064B to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B49C4(u8 *a0) {
    extern u8 D_8007064B;
    D_8007064B = 1;
    return 2;
}

/**
 * Clears the global byte D_8007064B, returns 2.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 2 (continue processing).
 */
s32 func_800B49D8(u8 *a0) {
    extern u8 D_8007064B;
    D_8007064B = 0;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B49E8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4A18);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4A40);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4A88);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4D0C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4D34);

/**
 * Pops a parameter, masks it with 0x7F, and calls func_80037198, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B4D7C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    func_80037198(*(s32 *)(a0 + (s8)idx * 4) & 0x7F);
    return 2;
}

/**
 * Calls func_80037240 and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B4DBC(u8 *a0) {
    func_80037240(a0);
    return 2;
}

/**
 * Calls func_800ADC04 and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B4DDC(u8 *a0) {
    func_800ADC04(a0);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4DFC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4E60);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4EB0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4ED8);

/**
 * Updates bit 2 of the entity byte at offset 0xD1 based on flag 0x200
 * in the flags word at offset 0x68.
 *
 * If bit 0x200 is set in the flags, clears bit 2 of the byte at 0xD1.
 * Otherwise, sets bit 2.
 */
void func_800B4F40(void) {
    extern u8 *D_800562C4;
    u8 *ptr = D_800562C4;

    if (*(s32 *)(ptr + 0x68) & 0x200) {
        *(u8 *)(ptr + 0xD1) = *(u8 *)(ptr + 0xD1) & 0xFD;
    } else {
        *(u8 *)(ptr + 0xD1) = *(u8 *)(ptr + 0xD1) | 0x2;
    }
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4F80);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B4FF8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B505C);

/**
 * Pops a parameter, calls func_800C0410, stores result at offset 0x140.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B5134(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + 0x140) = func_800C0410(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B5188);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B51E0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B5248);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B52B0);

/**
 * Pops a parameter, calls func_800239A8, stores result at offset 0x140.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B5318(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + 0x140) = func_800239A8(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object6", func_800B536C);

/**
 * Pops a parameter, calls func_80023B14, stores result at offset 0x140.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B53D8(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + 0x140) = func_80023B14(*(s32 *)(a0 + (s8)idx * 4));
    return 2;
}
