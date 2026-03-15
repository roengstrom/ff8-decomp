#include "common.h"
#include "character.h"

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5800);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5918);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E599C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5A28);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5B00);

/**
 * @brief Search for a magic spell by ID in a character's spell table.
 *
 * Iterates through 32 MagicSlot entries (g_characterMagic + charIdx * 152)
 * looking for a matching magicId. Returns the slot index if found, 0 if not.
 *
 * @param charIdx Character index (0-7).
 * @param spellId Magic spell ID to search for.
 * @return Slot index (1-31) if found, 0 if not found.
 */
s32 func_801E5C00(s32 charIdx, s32 spellId) {
    extern u8 D_80077818[];
    u8 *ptr = D_80077818 + charIdx * 152;
    s32 i = 0;

    do {
        if (spellId == *ptr) return i;
        i++;
        ptr += 2;
    } while (i < 32);

    return 0;
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5C50);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5CAC);

/**
 * @brief Check if either of two magic availability tests pass.
 * @param a0 First character context
 * @param a1 Second character context
 * @param a2 Second magic ID
 * @param a3 First magic ID
 * @return 1 if either func_801E5CAC call returns non-zero, 0 otherwise
 */
s32 func_801E5D14(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 r1 = func_801E5CAC(a0, a3);
    s32 r2 = func_801E5CAC(a1, a2);
    return (r1 | r2) != 0;
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5D64);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5E74);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E5F5C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6058);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E626C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E64FC);

/**
 * @brief Look up a magic spell ID for a character's slot.
 *
 * Indexes into D_80077378 at character @p a0 (stride 152) and slot @p a1
 * (stride 2). Checks if the count byte at offset 0x4A1 is nonzero;
 * if so, returns the spell ID at offset 0x4A0. Otherwise returns 0.
 *
 * @param a0 Character index (0-7).
 * @param a1 Magic slot index.
 * @return Spell ID byte, or 0 if slot is empty.
 */
/**
 * @brief Look up a magic spell ID for a character's slot.
 *
 * Indexes into D_80077378 at character @p a0 (stride 152) and slot @p a1
 * (stride 2). Checks if the count byte at offset 0x4A1 is nonzero;
 * if so, returns the spell ID at offset 0x4A0. Otherwise returns 0.
 *
 * @param a0 Character index (0-7).
 * @param a1 Magic slot index.
 * @return Spell ID byte, or 0 if slot is empty.
 *
 * @note Non-matching: Compiler schedules sll a1,a1,1 before lui/addiu
 * for D_80077378 base address, and uses different accumulation order
 * (v0+v1 then a1+v0 instead of a1+v0 then a1+v1).
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6648);

/**
 * @brief Test if magic bit a0 is available.
 *
 * Computes (1 << a0), calls func_801E5918 to get available mask,
 * and returns whether the bit is set.
 *
 * @param a0 Bit index to test.
 * @return 1 if bit is set, 0 otherwise.
 */
s32 func_801E668C(s32 a0) {
    s32 mask = 1 << a0;
    mask &= func_801E5918();
    return mask != 0;
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E66C0);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E677C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E67B0);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E67E0);

/**
 * @brief Render magic entry with adjusted width based on character attribute.
 *
 * Computes width as 0xCA minus half of the byte at a0+0x5B, and Y position
 * as a2 * 13 + 0x78. Calls func_801F0A34 to render.
 *
 * @param a0 Pointer to character data (byte at 0x5B used for width adjustment).
 * @param a1 X position parameter.
 * @param a2 Row index (multiplied by 13 and offset by 0x78 for Y position).
 *
 * @note Non-matching: register allocation -- compiler puts 0xCA constant
 * in v1 and lbu result in v0, but original uses v0 for constant and v1
 * for lbu. Instruction order is identical otherwise.
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6810);

/**
 * @brief Render magic entry with adjusted width based on character attribute.
 *
 * Computes width as 0xCA minus half of the byte at a0+0x5B, and Y position
 * as a2 * 13 + 0x71. Calls func_801F0A34 to render.
 *
 * @param a0 Pointer to character data (byte at 0x5B used for width adjustment).
 * @param a1 X position parameter.
 * @param a2 Row index (multiplied by 13 and offset by 0x71 for Y position).
 *
 * @note Non-matching: same register allocation issue as func_801E6810.
 */
INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6858);

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x43 for Y)
 */
void func_801E68A0(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x43);
}

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x8D for Y)
 */
void func_801E68F0(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x8D);
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6940);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E69EC);

/**
 * @brief Render magic entry at computed Y position with width 0x26.
 * @param a0 X position parameter
 * @param a1 Row index (multiplied by 13 and offset by 0x42 for Y position)
 */
void func_801E6A64(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0x26, a1 * 13 + 0x42);
}

/** @brief Call func_801F0A34 with a0, zero, 0x28, 0x39. */
void func_801E6A9C(s32 a0) {
    func_801F0A34(a0, 0, 0x28, 0x39);
}

/** @brief Call func_801F0A34 with a0, zero, 0x28, 0x94. */
void func_801E6AC4(s32 a0) {
    func_801F0A34(a0, 0, 0x28, 0x94);
}

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x9A for Y)
 */
void func_801E6AEC(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x9A);
}

/**
 * @brief Render magic entry at Y position computed from row modulo 4.
 * @param a0 X position parameter
 * @param a1 Row index (modulo 4, multiplied by 13, offset by 0x43 for Y)
 */
void func_801E6B3C(s32 a0, s32 a1) {
    func_801F0A34(a0, 0, 0xD4, (a1 % 4) * 13 + 0x43);
}

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6B8C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6CCC);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6D58);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6DD0);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6F54);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801E6FDC);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EAF50);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB0F4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB1A0);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB250);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB2D4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB454);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB6A4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EB80C);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBA68);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBBE4);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBD40);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EBF08);

INCLUDE_ASM("asm/ovl/menumgc/nonmatchings/menumgc", func_801EC43C);
