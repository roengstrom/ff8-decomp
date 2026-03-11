#include "common.h"

extern u8 D_800F16A4[];
extern u8 D_800F16A8[];
extern u8 D_800F16AC[];
extern u8 D_800EF724[];
extern u8 D_800F02F4[];
extern u8 D_800F16BC[];
extern u8 D_800F1468[];
extern u8 D_800F1668[];

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BAEE8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB024);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB084);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB0DC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB140);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB180);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB1B0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB1B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB1E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB368);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB3F8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB63C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB828);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BB9A8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBA54);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBBF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBD00);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBD04);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BBE48);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC060);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC3B8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC420);

/**
 * @brief Process entity data and optional extra pointer.
 *
 * Calls func_800BC420 with entity base + 0x60. If the word at
 * entity + 0x78 is non-null, also calls func_800BC420 with that pointer.
 *
 * @param entity Pointer to entity data.
 */
void func_800BC7D4(u8 *entity) {
    func_800BC420(entity + 0x60);
    if (*(s32 *)(entity + 0x78) != 0) {
        func_800BC420(*(s32 *)(entity + 0x78));
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC818);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC88C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC908);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BC9C8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BCA3C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BCF6C);

/**
 * @brief Look up a byte from a nested pointer table.
 *
 * Dereferences a0->0x64->0x8 to get a base pointer, then indexes
 * by a1 to read an offset, and returns the byte at base + offset.
 *
 * @param a0 Entity pointer with table reference at offset 0x64.
 * @param a1 Table index.
 * @return Byte value from the resolved address.
 */
s32 func_800BD040(s32 a0, s32 a1) {
    s32 base = *(s32 *)(*(s32 *)(a0 + 0x64) + 8);
    s32 offset = *(s32 *)(base + a1 * 4 + 4);
    return *(u8 *)(base + offset);
}

/**
 * @brief Call func_800BCA3C for entity fields, optionally for extra pointer.
 *
 * If the word at entity + 0x78 is non-null, calls func_800BCA3C with that
 * pointer and its data at +0xC. Then always calls func_800BCA3C with
 * entity + 0x60 and entity + 0x6C.
 *
 * @param entity Pointer to entity data.
 */
void func_800BD06C(u8 *entity) {
    s32 ptr = *(s32 *)(entity + 0x78);
    if (ptr != 0) {
        func_800BCA3C(ptr, ptr + 0xC);
    }
    func_800BCA3C((s32)entity + 0x60, (s32)entity + 0x6C);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD0B4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD1B4);

/**
 * @brief Call func_800C00D8 with fields from the D_800F16A4 pointer.
 *
 * Loads the pointer stored in D_800F16A4, reads a word at +8 and
 * a byte at +0x10, then passes them to func_800C00D8.
 */
void func_800BD230(void) {
    s32 ptr = *(s32 *)D_800F16A4;
    func_800C00D8(*(s32 *)(ptr + 8), *(u8 *)(ptr + 0x10));
}

/**
 * @brief Advance the D_800F16A8 data pointer and process the old entry.
 *
 * Reads the current pointer from D_800F16A8, advances it by 0x18,
 * calls func_800BFE1C with the old pointer, then copies the first
 * byte at the new pointer to D_800F16BC.
 *
 * @note Non-matching: CC1PSX computes val+0x18 into v1 then copies val→a0;
 * target modifies v0 in-place (a0=v0 first, then v0+=0x18).
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD260);

/**
 * @brief Call func_800BFE1C with the pointer stored in D_800F16A8.
 */
void func_800BD2AC(void) {
    func_800BFE1C(*(s32 *)D_800F16A8);
}

/**
 * @brief Read byte at offset 0x11 from pointer stored in D_800F16A4.
 *
 * @return The byte value.
 */
s32 func_800BD2D4(void) {
    return *(u8 *)(*(s32 *)D_800F16A4 + 0x11);
}

/**
 * @brief Read byte at offset 0x3 from pointer in D_800F16A8, mask with 0x2.
 *
 * @return Bit 1 of byte at offset 3.
 */
s32 func_800BD2EC(void) {
    return *(u8 *)(*(s32 *)D_800F16A8 + 3) & 2;
}

/**
 * @brief Read byte at offset 0x3 from pointer in D_800F16A8, mask with 0x4.
 *
 * @return Bit 2 of byte at offset 3.
 */
s32 func_800BD304(void) {
    return *(u8 *)(*(s32 *)D_800F16A8 + 3) & 4;
}

/**
 * @brief Read byte at offset 0x1 from pointer stored in D_800F16A8.
 *
 * @return The byte value.
 */
s32 func_800BD31C(void) {
    return *(u8 *)(*(s32 *)D_800F16A8 + 1);
}

/**
 * @brief Check entity rendering bit and trigger animation reset if set.
 *
 * Loads the entity pointer from D_800F16A8, checks if bit 3 of byte 3
 * is set. If so, calls func_800BF888 and returns 0. Otherwise returns 1.
 *
 * @return 0 if animation was reset, 1 otherwise.
 */
s32 func_800BD334(void) {
    u8 *entity = *(u8 **)D_800F16A8;
    if (entity[3] & 0x8) {
        func_800BF888(entity);
        return 0;
    }
    return 1;
}

/**
 * @brief Call func_800C5A34 with D_800F16AC pointer, a1=0x1A, a2=0x40.
 */
s32 func_800BD374(void) {
    return func_800C5A34(*(s32 *)D_800F16AC, 0x1A, 0x40);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD3A0);

/**
 * @brief Call func_800BD374 and conditionally call func_800BD3A0.
 *
 * If func_800BD374 returns non-zero, returns 1 immediately.
 * Otherwise calls func_800BD3A0 and returns its result.
 *
 * @return 1 if func_800BD374 succeeded, or func_800BD3A0's result.
 */
s32 func_800BD434(void) {
    if (func_800BD374() != 0) {
        return 1;
    }
    func_800BD3A0();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD464);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD508);

/**
 * @brief Set bit 15 of D_800EF724 halfword and clear D_800F02F4 halfword.
 */
void func_800BD594(void) {
    *(u16 *)D_800EF724 |= 0x8000;
    *(u16 *)D_800F02F4 = 0;
}

/**
 * @brief Initialize entity sound and finalize setup.
 *
 * Calls func_800B3534 with a1, func_800C5304 with a0+0x2FE and a1,
 * then func_8001F5C8.
 *
 * @param a0 Entity base pointer.
 * @param a1 Sound/mode parameter.
 */
void func_800BD5B0(s32 a0, s32 a1) {
    func_800B3534(a1);
    func_800C5304(a0 + 0x2FE, a1);
    func_8001F5C8();
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD5FC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD658);

/**
 * @brief Clear status bits 0x410 from entity flags and update.
 *
 * Loads the word at a0+8, clears bits 4 and 10 (mask 0x410),
 * stores the result back, then calls func_800B5C10.
 *
 * @param a0 Pointer to entity data.
 *
 * @note Non-matching: compiler allocates val→v0, mask→v1 instead of
 * val→a1, mask→v0. Register preference cannot be controlled from pure C.
 */
INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD77C);

/**
 * @brief Propagate a flag mask through a linked list of entities.
 *
 * Traverses a linked list starting from the pointer at a0+0xC. For each
 * node with bit 1 set in its status halfword (offset 0x0), ORs the mask
 * from a0+0x10 into the sub-object's halfword at offset 0x2C. The list
 * terminates when the next pointer (offset 0x8C) is null or loops back
 * to the head.
 *
 * @param a0 Pointer to entity structure (list head at +0xC, mask at +0x10).
 * @return Always returns 2.
 */
s32 func_800BD7A8(u8 *a0) {
    u8 *head;
    u8 *node;

    head = *(u8 **)(a0 + 0xC);
    node = head;
    do {
        if (*(u16 *)node & 2) {
            u8 *sub = *(u8 **)(node + 0x74);
            *(u16 *)(sub + 0x2C) |= *(u16 *)(a0 + 0x10);
        }
        node = *(u8 **)(node + 0x8C);
        if (node == head) {
            break;
        }
    } while (node != 0);
    return 2;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BD804);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BDE28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BDF94);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BE104);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BE89C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BECB0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEE78);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEEC4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEF14);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEFD0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object11", func_800BEFE0);
