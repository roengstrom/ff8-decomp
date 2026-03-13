#include "common.h"

/**
 * @brief Compute save slot data address.
 *
 * Returns a pointer into the D_801D3000 region at offset a0 * 128.
 *
 * @param a0 Save slot index.
 * @return Pointer to save slot data.
 */
s32 func_801E2800(s32 a0) {
    return (s32)0x801D3000 + (a0 << 7);
}

/**
 * @brief Set D_801EC294 flag to 1.
 */
void func_801E2814(void) {
    extern u8 D_801EC294;
    D_801EC294 = 1;
}

/**
 * @brief Return save data buffer size constant.
 *
 * @return Always 0x180.
 */
s32 func_801E2824(void) {
    return 0x180;
}

/**
 * @brief Read save system state pointer.
 *
 * @return Value of D_8005620C.
 */
s32 func_801E282C(void) {
    extern s32 D_8005620C;
    return D_8005620C;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E283C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2860);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E28C8);

/**
 * @brief Check if save slot halfword equals 0x8FF.
 *
 * Calls func_801E2800 to get save slot address, loads halfword
 * at offset +2, and returns whether it equals 0x8FF.
 *
 * @param a0 Save slot index.
 * @return 1 if halfword equals 0x8FF, 0 otherwise.
 */
s32 func_801E292C(s32 a0) {
    s32 result = func_801E2800(a0);
    return *(u16 *)(result + 2) == 0x8FF;
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2958);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2AB0);

/** @brief Draw inner panel with section id 0x5 and clear flag. */
s32 func_801E2C90(s32 a0) {
    return func_801F08D4(1, 5, a0, 0);
}

/** @brief Draw inner panel with section id 0x5 and set flag. */
s32 func_801E2CBC(s32 a0) {
    return func_801F08D4(1, 5, a0, 1);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2CE8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2D38);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2D78);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2DDC);

/**
 * @brief Check save slot validity flags and store result.
 *
 * Sets a0[0x4B] to 1 if either a0[0x2E] or a0[0x2F] is less than 2,
 * otherwise stores (a0[0x2E] < 2).
 *
 * @param a0 Save context pointer.
 * @return The stored flag value.
 *
 * @note Non-matching: CC1PSX schedules the two lbu loads together
 * and puts sltiu in the beqz delay slot, eliminating the nop between
 * loads that the original has.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2E9C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2ECC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E2F6C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E3098);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5464);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5510);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E55C4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5688);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E579C);

/**
 * @brief Render save menu entry with string ID 0x26 (save slot label).
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E58C0(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x26);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x0F.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E591C(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x0F);
    func_801F4274(a1, a2, v0, 0xC0, 0x87, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x12.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5978(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x12);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x18.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E59D4(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x18);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x27.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5A30(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x27);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x13.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5A8C(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x13);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5AE8);

/**
 * @brief Render save menu entry with string ID 0x17.
 * @param a0 Unused parameter
 * @param a1 X position for rendering
 * @param a2 Y position for rendering
 */
void func_801E5B70(s32 a0, s32 a1, s32 a2) {
    s32 v0 = func_801E2C90(0x17);
    func_801F4274(a1, a2, v0, 0xC0, 0x6B, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5BCC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5C38);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5D04);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5DC4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5E30);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E5EE0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6060);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E613C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E61C0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6204);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E653C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6754);

/**
 * @brief Render save menu entry with string ID 0x0D.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E6804(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x0D);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

/**
 * @brief Render save menu entry with string ID 0x0E.
 * @param a0 X position for rendering
 * @param a1 Y position for rendering
 */
void func_801E6860(s32 a0, s32 a1) {
    s32 v0 = func_801E2C90(0x0E);
    func_801F4274(a0, a1, v0, 0xC0, 0x64, 0x1000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E68BC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6918);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6A80);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6B1C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E6BE0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E700C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7190);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7268);

/** @brief Call func_80020608 with D_80077378 and 0x13A. */
void func_801E73C0(void) {
    extern u8 D_80077378[];
    func_80020608(D_80077378, 0x13A);
}

/**
 * @brief Wrapper that calls func_801E7428 (save menu handler).
 */
void func_801E73E8(void) {
    func_801E7428();
}

/**
 * @brief Wrapper that calls func_80023888 (save menu input handler).
 */
void func_801E7408(void) {
    func_80023888();
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7428);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E74BC);

/** @brief Draw inner panel with section id 0xE and clear flag. */
s32 func_801E74CC(s32 a0) {
    return func_801F08D4(1, 0xE, a0, 0);
}

/** @brief Draw inner panel with section id 0xE and set flag. */
s32 func_801E74F8(s32 a0) {
    return func_801F08D4(1, 0xE, a0, 1);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7524);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7550);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7598);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E760C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E76C4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7830);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E78FC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7938);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E79CC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7A84);

/** @brief Call func_800360D0 with a0 + 0xB4 and 0x801D3000. */
void func_801E7B18(u8 *a0) {
    func_800360D0(a0 + 0xB4, 0x801D3000);
}

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7B40);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7B5C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E7BE8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9804);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E98C8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9988);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9A30);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9AB4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9AD0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9ADC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9BC4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9CD4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9D84);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9DE8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801E9F10);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA034);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA180);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA278);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA37C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA444);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA4E0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA640);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA904);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EA9AC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAD28);

/** @brief Call func_80050BC4 with all arguments set to zero. */
void func_801EAE4C(void) {
    func_80050BC4(0, 0, 0);
}

/**
 * @brief Call func_801EAE4C then clear D_801EC301.
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAE74);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EAE98);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB00C);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB054);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB094);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB0B8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB0F4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB150);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB1AC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB1DC);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB224);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB270);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB2B8);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB2E4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB334);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB408);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB458);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB578);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB5F0);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB630);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB6B4);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB768);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB850);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB890);

INCLUDE_ASM("asm/ovl/menusav/nonmatchings/menusav", func_801EB928);
