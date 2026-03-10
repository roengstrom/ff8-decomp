#include "common.h"

extern u8 D_800F082C[];
extern u8 D_800F085C[];
extern u8 D_800F0830[];
extern u8 D_800F1668[];
extern u8 D_800E3DA8[];
s32 *func_800B88A0(void);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B872C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8740);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8754);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8798);

/**
 * @brief Unpack a pointer pair and call func_800B8B28 with mode 1.
 *
 * @param a0 Pointer to a structure with a word at +0 and data at +4.
 */
void func_800B87E4(s32 *a0) {
    func_800B8B28(*a0, (s32)a0 + 4, 1);
}

/**
 * @brief Unpack a pointer pair and call func_800B8B98.
 *
 * @param a0 Pointer to a structure with a word at +0 and data at +4.
 */
void func_800B8810(s32 *a0) {
    func_800B8B98(*a0, (s32)a0 + 4);
}

/**
 * @brief Unpack a pointer pair, call func_800B8A98, and clear result byte.
 *
 * @param a0 Pointer to a structure with a word at +0 and data at +4.
 */
void func_800B8838(s32 *a0) {
    u8 *result = (u8 *)func_800B8A98(*a0, (s32)a0 + 4, 1);
    if (result != 0) {
        result[1] = 0;
    }
}

/**
 * @brief Initialize a linked list array structure.
 *
 * Clears the head, stores count, then zeroes the data field of each entry (stride 8).
 *
 * @param a0 Pointer to the list structure.
 * @param count Number of entries to initialize.
 */
void func_800B8870(u8 *a0, s32 count) {
    s32 i = 1;
    *(s32 *)a0 = 0;
    *(s32 *)(a0 + 4) = count;
    if (count > 0) {
        a0 += 8;
        do {
            *(s32 *)(a0 + 4) = 0;
            i++;
            a0 += 8;
        } while (i <= count);
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B88A0);

/**
 * @brief Walk a linked list, following count links.
 *
 * @param node Starting node pointer.
 * @param count Number of links to follow.
 * @return The node reached after traversal, or NULL if a null link is hit.
 */
s32 func_800B88E0(s32 node, s32 count) {
    s32 i = 0;
    if (count <= 0) goto end;
loop:
    if (node == 0) goto end;
    node = *(s32 *)node;
    i++;
    if (i < count) goto loop;
end:
    return node;
}

/**
 * @brief Allocate a linked list node and initialize it.
 *
 * Calls func_800B88A0 to allocate. If successful, clears the first word
 * (next pointer) and sets the second word to 1 (active flag).
 *
 * @return Pointer to the allocated node, or NULL if allocation failed.
 */
s32 *func_800B890C(void) {
    s32 *node = func_800B88A0();
    if (node != 0) {
        node[0] = 0;
        node[1] = 1;
    }
    return node;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8944);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B89F4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8A98);

/**
 * @brief Look up entity via func_800B88E0 and return field at +4.
 *
 * Dereferences a1 as a pointer to get the search key, calls func_800B88E0
 * with that key and a2 as arguments. If a match is found, returns the
 * word at offset 4 of the result; otherwise returns 0.
 *
 * @param a0 Unused (passed from caller context).
 * @param a1 Pointer to search key word.
 * @param a2 Mode parameter for func_800B88E0.
 * @return Word at result+4 if found, or 0.
 */
s32 func_800B8B28(s32 a0, s32 *a1, s32 a2) {
    s32 result = (s32)func_800B88E0(*a1, a2);
    if (result != 0) {
        return *(s32 *)(result + 4);
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8B60);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8B98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8BEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8C08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8C6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8CE0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8DB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8E2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8E4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8EF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8F2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8F4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8F84);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8F98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8FBC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8FE4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8FEC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9034);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9048);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9078);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9114);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9174);

/**
 * @brief Align a size up to 4 bytes and store to D_800F082C.
 *
 * @param size Byte count to align.
 */
void func_800B91B4(s32 size) {
    *(s32 *)D_800F082C = (size + 3) & ~3;
}

/**
 * @brief Compute the difference between D_800F085C and D_800F082C.
 *
 * @return D_800F085C - D_800F082C.
 */
s32 func_800B91CC(void) {
    return *(s32 *)D_800F085C - *(s32 *)D_800F082C;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B91E4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B921C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9290);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B94E0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9518);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B953C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B96EC);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B97D8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9BF8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9F34);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BA2D0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BA640);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BA874);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BA9FC);

/**
 * @brief Call func_800B2B68 with D_800F0830.
 */
void func_800BAAA4(void) {
    func_800B2B68(D_800F0830);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BAAC8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BABC0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BAD28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BAE08);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BAE28);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BAE6C);

/**
 * @brief Clear the global D_800F1668 to zero.
 */
void func_800BAEDC(void) {
    *(s32 *)D_800F1668 = 0;
}
