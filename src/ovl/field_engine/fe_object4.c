#include "common.h"

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800ADB68);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800ADC04);

/**
 * Returns 0, indicating no action taken.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 0.
 */
s32 func_800ADC9C(u8 *a0) {
    return 0;
}

/**
 * Pops the stack index, adds the value at [idx] to [idx+1], stores at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADCA4(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) + *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, subtracts [idx+1] from [idx], stores at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADCD8(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) - *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Negates the top-of-stack value in-place.
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADD0C(u8 *a0) {
    s8 idx;

    idx = *(s8 *)(a0 + 0x184);
    *(s32 *)(a0 + idx * 4) = -*(s32 *)(a0 + idx * 4);
}

/**
 * Pops the stack index, multiplies [idx] by [idx+1], stores at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADD30(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) * *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/** @brief Stack division: [idx] = [idx] / [idx+1]. */
void func_800ADD68(u8 *a0) {
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) / *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/** @brief Stack modulo: [idx] = [idx] % [idx+1]. */
void func_800ADDA0(u8 *a0) {
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) % *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, tests [idx] == [idx+1], stores boolean at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADDD8(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) == *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/** @brief Stack greater-than: [idx] = [idx] > [idx+1]. */
void func_800ADE10(u8 *a0) {
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) > *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, compares [idx] >= [idx+1], stores boolean at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADE44(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) >= *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, compares [idx] < [idx+1], stores boolean at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADE7C(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) < *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, compares [idx] <= [idx+1], stores boolean at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADEB0(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) <= *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, tests [idx] != [idx+1], stores boolean at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADEE8(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) != *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, ANDs [idx] with [idx+1], stores at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADF20(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) & *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, ORs [idx] with [idx+1], stores at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADF54(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) | *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Pops the stack index, XORs [idx] with [idx+1], stores at [idx].
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADF88(u8 *a0) {
    u8 idx;

    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) ^ *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Bitwise NOTs the top-of-stack value in-place.
 *
 * @param a0 Pointer to the script/object structure.
 */
void func_800ADFBC(u8 *a0) {
    s8 idx;

    idx = *(s8 *)(a0 + 0x184);
    *(s32 *)(a0 + idx * 4) = ~*(s32 *)(a0 + idx * 4);
}

/** @brief Stack arithmetic right shift: [idx] = [idx] >> [idx+1]. */
void func_800ADFE0(u8 *a0) {
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) >> *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/** @brief Stack left shift: [idx] = [idx] << [idx+1]. */
void func_800AE014(u8 *a0) {
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + (s8)(idx - 1) * 4) = *(s32 *)(a0 + (s8)(idx - 1) * 4) << *(s32 *)(a0 + (s8)(idx - 1) * 4 + 4);
}

/**
 * Dispatches to a function from the D_800C6760 table based on index a1, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @param a1 Table index.
 * @return 2 (continue processing).
 */
s32 func_800AE048(u8 *a0, s32 a1) {
    extern s32 (*D_800C6760[])(u8 *);
    D_800C6760[a1](a0);
    return 2;
}

/**
 * Adds a1 to the halfword at offset 0x176 in the object, returns 4.
 *
 * @param a0 Pointer to the script/object structure.
 * @param a1 Value to add.
 * @return 4.
 */
s32 func_800AE080(u8 *a0, s32 a1) {
    *(u16 *)(a0 + 0x176) = *(u16 *)(a0 + 0x176) + a1;
    return 4;
}

/** @brief Conditional branch: if top-of-stack is zero, add a1 to PC. */
s32 func_800AE098(u8 *a0, s32 a1) {
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    if (*(s32 *)(a0 + (s8)idx * 4) != 0) {
        return 2;
    }
    *(u16 *)(a0 + 0x176) = *(u16 *)(a0 + 0x176) + a1;
    return 4;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AE0DC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AE124);

/** @brief Pop halfword from stack and store to entity offset 0x176. */
void func_800AE184(u8 *a0) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u16 *)(a0 + 0x176) = *(u16 *)(a0 + (s8)idx * 4);
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AE1AC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AE3A4);

/** @brief Call func_800AE3A4 with a1 and mode 4, push result onto stack. Returns 2. */
s32 func_800AE4C4(u8 *a0, s32 a1) {
    s32 result = func_800AE3A4(a1, 4);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load entity slot value a1*4+0x140, call func_800AE3A4 with mode 4, push result. Returns 2. */
s32 func_800AE518(u8 *a0, s32 a1) {
    s32 result = func_800AE3A4(*(s32 *)(a0 + a1 * 4 + 0x140), 4);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load byte from D_80077378+a1+0xD60, call func_800AE3A4 with mode 5, push result. Returns 2. */
s32 func_800AE574(u8 *a0, s32 a1) {
    extern u8 D_80077378[];
    u8 *base = D_80077378;
    s32 result = func_800AE3A4(*(u8 *)(base + a1 + 0xD60), 5);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load halfword from D_800780D8+a1, call func_800AE3A4 with mode 6, push result. Returns 2. */
s32 func_800AE5D4(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 *base = D_800780D8;
    s32 result = func_800AE3A4(*(u16 *)(base + a1), 6);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load word from D_800780D8+a1, call func_800AE3A4 with mode 7, push result. Returns 2. */
s32 func_800AE634(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 *base = D_800780D8;
    s32 result = func_800AE3A4(*(s32 *)(base + a1), 7);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load signed byte from D_800780D8+a1, call func_800AE3A4 with mode 2, push result. Returns 2. */
s32 func_800AE694(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 *base = D_800780D8;
    s32 result = func_800AE3A4(*(s8 *)(base + a1), 2);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load signed halfword from D_800780D8+a1, call func_800AE3A4 with mode 3, push result. Returns 2. */
s32 func_800AE6F4(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 *base = D_800780D8;
    s32 result = func_800AE3A4(*(s16 *)(base + a1), 3);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Load word from D_800780D8+a1, call func_800AE3A4 with mode 4, push result. Returns 2. */
s32 func_800AE754(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 *base = D_800780D8;
    s32 result = func_800AE3A4(*(s32 *)(base + a1), 4);
    u8 idx = *(u8 *)(a0 + 0x184) + 1;
    *(u8 *)(a0 + 0x184) = idx;
    *(s32 *)(a0 + (s8)idx * 4) = result;
    return 2;
}

/** @brief Pop top-of-stack and store to result slot a1*4+0x140. Returns 2. */
s32 func_800AE7B4(u8 *a0, s32 a1) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(a0 + a1 * 4 + 0x140) = *(s32 *)(a0 + (s8)idx * 4);
    return 2;
}

s32 func_800AE7E4(u8 *a0, s32 a1) {
    extern u8 D_80077378[];
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    a1 = a1 + (s32)D_80077378;
    *(u8 *)(a1 + 0xD60) = *(u8 *)(a0 + (s8)idx * 4);
    return 2;
}

/** @brief Pop halfword from stack and store to D_800780D8[a1]. Returns 2. */
s32 func_800AE81C(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(u16 *)(D_800780D8 + a1) = *(u16 *)(a0 + (s8)idx * 4);
    return 2;
}

/** @brief Pop word from stack and store to D_800780D8[a1]. Returns 2. */
s32 func_800AE854(u8 *a0, s32 a1) {
    extern u8 D_800780D8[];
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    *(s32 *)(D_800780D8 + a1) = *(s32 *)(a0 + (s8)idx * 4);
    return 2;
}

/** @brief Push immediate value a1 onto stack. Returns 2. */
s32 func_800AE88C(u8 *a0, s32 a1) {
    u8 idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx + 1;
    *(s32 *)(a0 + (s8)(idx + 1) * 4) = a1;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AE8B4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AE978);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEA44);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEB0C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEBD8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEC78);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AED9C);

/**
 * Returns 3, indicating skip to next entity.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 3.
 */
s32 func_800AEEC4(u8 *a0) {
    return 3;
}

/**
 * Returns 1, indicating wait/yield.
 *
 * @param a0 Pointer to the script/object structure (unused).
 * @return 1.
 */
s32 func_800AEECC(u8 *a0) {
    return 1;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEED4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEF4C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AEFE8);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF02C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF070);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF0B4);

/**
 * Calls func_80038464 with the object pointer, masks result to byte,
 * stores at offset 0x140, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF0E0(u8 *a0) {
    *(s32 *)(a0 + 0x140) = func_80038464(a0) & 0xFF;
    return 2;
}

/**
 * Stores a1 as a halfword at offset 0x218 in the object, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @param a1 Value to store.
 * @return 2 (continue processing).
 */
s32 func_800AF114(u8 *a0, s32 a1) {
    *(s16 *)(a0 + 0x218) = a1;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF120);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF1AC);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF224);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF274);

/** @brief Pop mask, test against D_800705F8, store boolean at result. Returns 2. */
s32 func_800AF2C4(u8 *a0) {
    extern s32 D_800705F8;
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    if (D_800705F8 & *(s32 *)(a0 + (s8)idx * 4)) {
        *(s32 *)(a0 + 0x140) = 1;
    } else {
        *(s32 *)(a0 + 0x140) = 0;
    }
    return 2;
}

/** @brief Pop mask, test against D_80070600, store boolean at result. Returns 2. */
s32 func_800AF314(u8 *a0) {
    extern s32 D_80070600;
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    if (D_80070600 & *(s32 *)(a0 + (s8)idx * 4)) {
        *(s32 *)(a0 + 0x140) = 1;
    } else {
        *(s32 *)(a0 + 0x140) = 0;
    }
    return 2;
}

/** @brief Pop mask, test against D_800705E8, store boolean at result. Returns 2. */
s32 func_800AF364(u8 *a0) {
    extern s32 D_800705E8;
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    if (D_800705E8 & *(s32 *)(a0 + (s8)idx * 4)) {
        *(s32 *)(a0 + 0x140) = 1;
    } else {
        *(s32 *)(a0 + 0x140) = 0;
    }
    return 2;
}

/** @brief Pop mask, test against D_800705F0, store boolean at result. Returns 2. */
s32 func_800AF3B4(u8 *a0) {
    extern s32 D_800705F0;
    u8 idx;
    idx = *(u8 *)(a0 + 0x184);
    *(u8 *)(a0 + 0x184) = idx - 1;
    if (D_800705F0 & *(s32 *)(a0 + (s8)idx * 4)) {
        *(s32 *)(a0 + 0x140) = 1;
    } else {
        *(s32 *)(a0 + 0x140) = 0;
    }
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF404);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF444);

/** @brief Set bit 0x10 in entity flags word at D_800562C4+0x68. Returns 2. */
s32 func_800AF47C(void) {
    extern s32 D_800562C4;
    *(s32 *)(D_800562C4 + 0x68) |= 0x10;
    return 2;
}

/** @brief Clear bit 0x10 in entity flags word at D_800562C4+0x68. Returns 2. */
s32 func_800AF4A0(void) {
    extern s32 D_800562C4;
    *(s32 *)(D_800562C4 + 0x68) &= ~0x10;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF4C4);

/**
 * Sets the byte at offset 0x194 to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF5A8(u8 *a0) {
    *(u8 *)(a0 + 0x194) = 1;
    return 2;
}

/**
 * Clears the byte at offset 0x194, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF5B8(u8 *a0) {
    *(u8 *)(a0 + 0x194) = 0;
    return 2;
}

/**
 * Sets the byte at offset 0x188 to 1, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF5C4(u8 *a0) {
    *(u8 *)(a0 + 0x188) = 1;
    return 2;
}

/**
 * Clears the byte at offset 0x188, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF5D4(u8 *a0) {
    *(u8 *)(a0 + 0x188) = 0;
    return 2;
}

/**
 * Sets bit 0x2 in the flags at offset 0x160, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF5E0(u8 *a0) {
    *(s32 *)(a0 + 0x160) = *(s32 *)(a0 + 0x160) | 0x2;
    return 2;
}

/**
 * Clears bit 0x2 in the flags at offset 0x160, returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800AF5F8(u8 *a0) {
    *(s32 *)(a0 + 0x160) = *(s32 *)(a0 + 0x160) & ~0x2;
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF610);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AF7E4);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AFD20);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AFD68);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AFE24);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800AFF64);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B002C);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0124);

/**
 * Calls func_800381BC and returns 2.
 *
 * @param a0 Pointer to the script/object structure.
 * @return 2 (continue processing).
 */
s32 func_800B0280(u8 *a0) {
    func_800381BC(a0);
    return 2;
}

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B02A0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0304);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0344);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0444);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0570);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0638);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B06D0);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0784);

INCLUDE_ASM("asm/ovl/field_engine/nonmatchings/fe_object4", func_800B0818);
