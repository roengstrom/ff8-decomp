#include "common.h"

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2800);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E28B4);

/**
 * @brief Compute pointer to ability data entry by index.
 * @param a0 Ability entry index (stride 8).
 * @return Address of D_8007CEE0 + a0 * 8.
 */
s32 func_801E2920(s32 a0) {
    extern u8 D_8007CEE0[];
    return D_8007CEE0 + a0 * 8;
}

/**
 * @brief Read ability menu display count.
 * @return Value of D_801E3DB8.
 */
s32 func_801E2934(void) {
    extern u8 D_801E3DB8;
    return D_801E3DB8;
}

/**
 * @brief Look up an ability name string by index.
 *
 * If @p a0 is within bounds (less than D_801E3D9C), uses it to index
 * into D_801E3D84 to get an ability ID, then calls func_80020AD4
 * to get the corresponding string. Returns NULL if out of bounds.
 *
 * @param a0 Ability list index.
 * @return Pointer to ability name string, or NULL if index out of bounds.
 *
 * @note Non-matching: Compiler inverts branch direction (bnez into lookup
 * vs original beqz over lookup).
 */
INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2944);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2990);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E2A34);

/**
 * @brief Configure and draw an ability menu panel border.
 *
 * Sets up D_801FAB00 with no icon, position from a2/a3,
 * fixed size (0xF4 x 0x12), then draws via func_801EF9AC.
 *
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @param a2 X position
 * @param a3 Y position
 */
void func_801E3530(s32 a0, s32 a1, s16 a2, s16 a3) {
    extern u8 D_801FAB00[];
    extern s32 D_80083848;

    s32 cfg = (s32)D_801FAB00;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a2;
    *(s16 *)(cfg + 0x4) = 0xF4;
    *(s16 *)(cfg + 0x6) = 0x12;
    *(s16 *)(cfg + 0x2) = a3;
    func_801EF9AC(a0, a1, 0x1000, D_80083848);
}

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3580);

/**
 * @brief Configure and draw an ability list panel with scrolling.
 *
 * Sets up D_801FAB00 with icon type 0x55, position and scroll
 * parameters from the source data, then registers func_801E3580
 * as the rendering callback via func_801EFBB4.
 *
 * @param a0 Source data pointer (ability list state)
 * @param a1 X position for callback
 * @param a2 Y position for callback
 * @param a3 Panel X position
 * @param stackArg Panel Y position (5th arg on stack)
 */
void func_801E3630(s32 a0, s32 a1, s32 a2, s32 a3, s32 stackArg) {
    extern u8 D_801FAB00[];
    extern void func_801E3580();

    s32 cfg = (s32)D_801FAB00;

    *(u8 *)(cfg + 0x10) = 0x55;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a3;
    *(s16 *)(cfg + 0x4) = 0x144;
    *(s16 *)(cfg + 0x6) = 0x1A;
    *(u8 *)(cfg + 0x13) = 1;
    *(u8 *)(cfg + 0x16) = 0;
    *(u8 *)(cfg + 0x17) = 1;
    *(s16 *)(cfg + 0x2) = stackArg;
    *(s16 *)(cfg + 0x14) = *(u16 *)(a0 + 0x32);
    *(s32 *)(cfg + 0x20) = a0 + 0x20;
    func_801EFBB4(a1, a2, (s32)func_801E3580);
}

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E36AC);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E381C);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3904);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E39E0);

INCLUDE_ASM("asm/ovl/menuabl/nonmatchings/menuabl", func_801E3AE0);

/**
 * @brief Scan ability bitmask and build list of available ability indices.
 *
 * Calls func_801F72B4 to get an ability bitmask, then iterates bits 0-23.
 * For each set bit, stores (bit_index + 0x5C) into D_801E3D84 array and
 * increments the count in D_801E3D9C.
 */
void func_801E3C28(void) {
    extern u8 D_801E3D84[];
    extern u8 D_801E3D9C;

    s32 mask = func_801F72B4();
    u8 *dst = D_801E3D84;
    D_801E3D9C = 0;
    {
        s32 i = 0;
        s32 one = 1;

        do {
            if (mask & (one << i)) {
                *dst = i + 0x5C;
                D_801E3D9C = D_801E3D9C + 1;
                dst++;
            }
            i++;
        } while (i < 0x18);
    }
}

/**
 * @brief Initialize ability menu: register callbacks and set up display state.
 *
 * Registers func_801E2A34 and func_801E3AE0 as callbacks via func_801F179C.
 * If registration succeeds, clears state fields (0x20, 0x2C), stores the
 * ability bitmask (0x28) and display size (0x2E = 0x1000), builds the ability
 * list, initializes data, and enters via func_801E2A34.
 */
void func_801E3C9C(void) {
    extern void func_801E2A34();
    extern void func_801E3AE0();
    s32 result = func_801F179C((s32)func_801E2A34, (s32)func_801E3AE0);

    if (result != 0) {
        *(s16 *)(result + 0x2C) = 0;
        *(s32 *)(result + 0x20) = 0;
        *(s32 *)(result + 0x28) = func_801F72B4();
        *(s16 *)(result + 0x2E) = 0x1000;
        func_801E3C28();
        func_801E2990();
        func_801E2A34(result);
    }
}
