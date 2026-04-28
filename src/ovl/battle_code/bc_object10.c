#include "common.h"

extern u8 D_800F082C[];
extern u8 D_800F085C[];
extern u8 D_800F0830[];
extern u8 D_800F1668[];
extern u8 D_800E3DA8[];
extern u8 D_800EF738[];
extern u8 D_800F05F0[];
extern u8 D_800F0854[];
extern u8 D_80170000[];
s32 *func_800B88A0(void);
void func_800B8BEC(void);
void func_800B9078(void);
s32 func_800B2C58(s32);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B872C);


INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8754);

/**
 * @brief Check availability and register entity with tag byte.
 *
 * Calls func_800B8754 to check availability. If non-zero, calls
 * func_800B8944 with the word at a0, a0+4, and the return value,
 * then stores a1 as a byte at offset 1 of the result.
 *
 * @param a0 Pointer to entity data.
 * @param a1 Tag byte to store at result offset 1.
 */
void func_800B8798(s32 *a0, s32 a1) {
    s32 val = func_800B8754();
    if (val != 0) {
        u8 *result = (u8 *)func_800B8944(*a0, (s32)a0 + 4, val);
        result[1] = a1;
    }
}

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

/**
 * @brief Count nodes in a linked list after skipping the first two.
 *
 * Dereferences the pointer at *a1 twice to skip two header nodes,
 * then walks the list counting nodes until NULL.
 *
 * @param a0 Unused first parameter.
 * @param a1 Pointer to the start of the linked list (double indirection).
 * @return Number of nodes after the first two, or 0 if list is too short.
 */
s32 func_800B8B60(s32 a0, s32 *a1) {
    s32 count = 0;
    a1 = *(s32 **)a1;
    if (a1 == 0) {
        return count;
    }
    a1 = *(s32 **)a1;
    if (a1 == 0) {
        return count;
    }
    do {
        a1 = *(s32 **)a1;
        count++;
    } while (a1 != 0);
    return count;
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8B98);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8BEC);


INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8C6C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8CE0);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8DB8);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8E2C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8E4C);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8EF4);

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8F2C);

/**
 * @brief Allocate handler for func_800B8BEC and store initial params.
 *
 * Registers func_800B8BEC as callback via func_800B2C58. If allocation
 * succeeds, clears halfword at +0xC, stores a0 at +0xE, clears +0x10,
 * and sets +0x11 to 0x80.
 *
 * @param a0 Value stored as halfword at handler offset 0xE.
 */
void func_800B8F4C(s32 a0) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800B8BEC);
    if (result != 0) {
        *(u16 *)(result + 0xC) = 0;
        *(u16 *)(result + 0xE) = a0;
        *(u8 *)(result + 0x10) = 0;
        *(u8 *)(result + 0x11) = 0x80;
    }
}

/**
 * @brief Allocate handler for func_800B8BEC with inverted byte layout.
 *
 * Same as func_800B8F4C but with +0x10 set to 0x80 and +0x11 cleared.
 *
 * @param a0 Value stored as halfword at handler offset 0xE.
 */
void func_800B8F98(s32 a0) {
    u8 *result = (u8 *)func_800B2C58((s32)func_800B8BEC);
    if (result != 0) {
        *(u16 *)(result + 0xC) = 0;
        *(u16 *)(result + 0xE) = a0;
        *(u8 *)(result + 0x10) = 0x80;
        *(u8 *)(result + 0x11) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B8FE4);


/**
 * @brief Initialize sound entry fields if D_800E3DA8 pointer is non-null.
 *
 * Loads the pointer from D_800E3DA8. If non-null, sets byte at +0x10
 * to 0xFF, clears halfword at +0xC, stores a0 as halfword at +0xE,
 * and clears byte at +0x11 (re-reading the pointer for the last store).
 *
 * @param a0 Value to store at entry offset 0xE.
 */
void func_800B9048(s32 a0) {
    s32 ptr = *(s32 *)D_800E3DA8;
    if (ptr != 0) {
        s32 ptr2;
        *(u8 *)(ptr + 0x10) = 0xFF;
        ptr2 = *(s32 *)D_800E3DA8;
        *(u16 *)(ptr + 0xC) = 0;
        *(u16 *)(ptr + 0xE) = a0;
        *(u8 *)(ptr2 + 0x11) = 0;
    }
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800B9078);

/**
 * @brief Initialize D_800EF738 color entries and register animation callback.
 *
 * Clears 3 bytes at offset 0x28 for each of 4 entries at stride 0x2C
 * in D_800EF738. Then registers func_800B9078 via func_800B2C58,
 * clears the halfword at offset 0xC of the result, and stores a0
 * at offset 0xE.
 *
 * @param a0 Value to store at result offset 0xE.
 */
void func_800B9114(s32 a0) {
    u8 *ptr;
    s32 i = 0;
    ptr = D_800EF738;
    for (; i < 4; i++) {
        *(volatile u8 *)(ptr + 0x28) = 0;
        *(volatile u8 *)(ptr + 0x29) = 0;
        *(volatile u8 *)(ptr + 0x2A) = 0;
        ptr += 0x2C;
    }
    {
        u8 *result = (u8 *)func_800B2C58((s32)func_800B9078);
        *(u16 *)(result + 0xC) = 0;
        *(u16 *)(result + 0xE) = a0;
    }
}

/**
 * @brief Update max frame count and recompute buffer address.
 *
 * Reads the word at a0-4. If it exceeds D_800F0854, updates D_800F0854
 * and recomputes D_800F085C as D_80170000 minus the new value times 8.
 *
 * @param a0 Pointer (the word at a0-4 is the frame count).
 */
void func_800B9174(u8 *a0) {
    s32 val = *(s32 *)(a0 - 4);
    if (*(u16 *)D_800F0854 < val) {
        *(u16 *)D_800F0854 = val;
        *(s32 *)D_800F085C = (s32)D_80170000 - *(u16 *)D_800F0854 * 8;
    }
}

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

/**
 * @brief Find the first free slot in D_800F05F0 array.
 *
 * Scans 11 entries at stride 0x34 in D_800F05F0. Returns a pointer to the
 * first entry where byte at offset 1 is zero, or NULL if all occupied.
 *
 * @return Pointer to free slot, or 0 if none found.
 */
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

/**
 * @brief Clear the active byte of entries matching a given ID.
 *
 * Iterates 11 entries at stride 0x34 from D_800F05F0. If an entry's
 * first byte matches a0 or a0 + 0x1000, clears byte[1] of that entry.
 *
 * @param a0 ID to match against (also checks a0 + 0x1000).
 */
void func_800BAE28(s32 a0) {
    u8 *base = (u8 *)(s32)D_800F05F0;
    s32 i = 0;
    s32 match2 = a0 + 0x1000;
    do {
        if (base[0] == a0 || base[0] == match2) {
            base[1] = 0;
        }
        i++;
        base += 0x34;
    } while (i < 11);
}

INCLUDE_ASM("asm/ovl/battle_code/nonmatchings/bc_object10", func_800BAE6C);

/**
 * @brief Clear the global D_800F1668 to zero.
 */
void func_800BAEDC(void) {
    *(s32 *)D_800F1668 = 0;
}
