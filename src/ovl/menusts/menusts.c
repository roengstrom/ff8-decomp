#include "common.h"

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5800);

/**
 * @brief Look up status display type byte.
 *
 * Indexes into D_801E9964 (8-byte stride) and returns the byte
 * at offset 4.
 *
 * @param a0 Status entry index.
 * @return Type byte.
 *
 * @note Non-matching: Leaf register allocation puts base address in v0
 * (original) vs a0 (compiled).
 */
INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E582C);

/**
 * @brief Compute status display x-position.
 *
 * Indexes into D_801E9964 (8-byte stride), loads halfword at offset 0,
 * and adds D_801E99AC base.
 *
 * @param a0 Status entry index.
 * @return Computed address.
 *
 * @note Non-matching: Leaf register allocation uses v1 for table base
 * (original) vs v0/a0 (compiled).
 */
INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5848);

/**
 * @brief Compute status display y-position.
 *
 * Indexes into D_801E9964 (8-byte stride), loads halfword at offset 2,
 * and adds D_801E99AC base.
 *
 * @param a0 Status entry index.
 * @return Computed address.
 *
 * @note Non-matching: Same leaf register allocation issue as func_801E5848.
 */
INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E586C);

/**
 * @brief Wrapper that calls func_80020C08 with a0 offset by 0x33.
 *
 * @param a0 Base pointer to a menu entry structure.
 */
void func_801E5890(u8 *a0) {
    func_80020C08(a0 + 0x33);
}

/**
 * @brief Wrapper that calls func_80020C6C with a0 offset by 0x33.
 *
 * @param a0 Base pointer to a menu entry structure.
 */
void func_801E58B0(u8 *a0) {
    func_80020C6C(a0 + 0x33);
}

/** @brief Draw inner panel with section id 0x7 and clear flag. */
s32 func_801E58D0(s32 a0) {
    return func_801F08D4(1, 7, a0, 0);
}

/** @brief Draw inner panel with section id 0x7 and set flag. */
s32 func_801E58FC(s32 a0) {
    return func_801F08D4(1, 7, a0, 1);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5928);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5D28);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E61B0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6254);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E62A8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E68A4);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6994);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E69E4);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6B84);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6C68);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6EA0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E709C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7278);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E72D8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E750C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7644);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7690);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7B20);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7D04);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E811C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E81B8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8598);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E86F0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8850);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8890);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E88D0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8910);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8950);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8990);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8A08);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8BAC);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8CE4);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E9118);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E9178);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E923C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E936C);
