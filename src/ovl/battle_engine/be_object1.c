#include "common.h"

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009822C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098304);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800984DC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098690);

/**
 * @brief Initialize controller input arrays for battle engine.
 *
 * Reads current controller state for three input types (digital, analog X, analog Y)
 * and fills their respective 3-element history arrays with the initial values.
 */
void func_80098828(void) {
    extern u16 D_801C2EB8[];
    extern u16 D_801C2EC0[];
    extern u16 D_801C2EC8[];
    s32 i;

    func_800A2BD8();
    D_801C2EC8[0] = func_800A2B84(0);
    D_801C2EC0[0] = func_800A2BA0(0);
    D_801C2EB8[0] = func_800A2BBC(0);

    for (i = 1; i < 3; i++) {
        D_801C2EC8[i] = D_801C2EC8[0];
        D_801C2EC0[i] = D_801C2EC0[0];
        D_801C2EB8[i] = D_801C2EB8[0];
    }
}

/**
 * @brief Clear D_801C2FD0 to zero.
 */
void func_800988D4(void) {
    extern s32 D_801C2FD0;
    D_801C2FD0 = 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800988E0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098A1C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098A6C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098AB4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098B08);

void func_80098B68(void) {
}

/**
 * @brief Set D_801C2FD8 to 0x1F800000 (scratchpad RAM base address).
 */
void func_80098B70(void) {
    extern s32 D_801C2FD8;
    D_801C2FD8 = 0x1F800000;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098B80);

/**
 * @brief Align a size up to 4 bytes and subtract from the allocation pointer.
 *
 * Rounds a0 up to the next multiple of 4 and decrements D_801C2FD8 by that amount.
 *
 * @param a0 Size to allocate (will be aligned up to 4).
 */
void func_80098BA0(s32 a0) {
    extern s32 D_801C2FD8;
    D_801C2FD8 -= (a0 + 3) & ~3;
}

/**
 * @brief Initialize a linked list header and clear all node slots.
 *
 * @param a0 Pointer to the list header (head, tail, pool ptr, size, count).
 * @param a1 Pointer to the node pool.
 * @param a2 Size of each node in bytes.
 * @param a3 Number of nodes in the pool.
 */
void func_80098BC0(u8 *a0, u8 *a1, s32 a2, s32 a3) {
    s32 i = 0;
    *(s32 *)(a0 + 0) = 0;
    *(s32 *)(a0 + 4) = 0;
    *(s32 *)(a0 + 8) = (s32)a1;
    *(s16 *)(a0 + 0xC) = a2;
    *(s16 *)(a0 + 0xE) = a3;
    if (a3 > 0) {
        do {
            *(s16 *)a1 = 0;
            i++;
            a1 += a2;
        } while (i < a3);
    }
}

/**
 * @brief Find and return the first free node in the pool.
 *
 * Scans the node pool referenced by the list header for a node whose
 * flags bit 0 is clear (inactive). Returns the first free node found,
 * or NULL if all nodes are in use.
 *
 * @param a0 Pointer to list header (+0x8=pool, +0xC=stride, +0xE=count).
 * @return Pointer to the first free node, or NULL if none available.
 */
void *func_80098BF8(u8 *a0) {
    s32 count = *(s16 *)(a0 + 0xE);
    u8 *pool = (u8 *)*(s32 *)(a0 + 0x8);
    s32 i = 0;
    if (count > 0) {
        s32 n = count;
        do {
            if (!(*(u16 *)pool & 1)) {
                return pool;
            }
            i++;
            pool += *(s16 *)(a0 + 0xC);
        } while (i < n);
    }
    return 0;
}

/**
 * @brief Allocate a node and append it to a doubly-tracked linked list.
 *
 * Calls func_80098BF8 to allocate a node. If successful, initializes
 * flags (sets bit 0), clears fields at +2 and +4, stores @p a1 at +8,
 * and appends the node: if the list is non-empty, links old tail to
 * new node; otherwise sets head. Always updates tail pointer.
 *
 * @param a0 Pointer to list header (head at +0, tail at +4).
 * @param a1 Value to store at offset +8 of the new node (callback pointer).
 * @return Pointer to the new node, or NULL if allocation failed.
 */
void *func_80098C44(u8 *a0, s32 a1) {
    u8 *node = (u8 *)func_80098BF8(a0);
    if (node != 0) {
        s32 tail;
        *(u16 *)(node + 0) |= 1;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 4) = 0;
        *(s32 *)(node + 8) = a1;
        tail = *(s32 *)(a0 + 4);
        if (tail != 0) {
            *(s32 *)(tail + 4) = (s32)node;
        } else {
            *(s32 *)a0 = (s32)node;
        }
        *(s32 *)(a0 + 4) = (s32)node;
    }
    return node;
}

/**
 * @brief Allocate and insert a node into a linked list.
 *
 * Calls func_80098BF8 to allocate a node. If allocation succeeds,
 * initializes flags (sets bit 0), clears field at +2, stores @p a1
 * at offset +8, and prepends the node to the list headed at @p a0.
 *
 * @param a0 Pointer to the list head pointer.
 * @param a1 Value to store at offset +8 of the new node (callback pointer).
 * @return Pointer to the new node, or NULL if allocation failed.
 */
void *func_80098CC0(u8 *a0, s32 a1) {
    u8 *node = (u8 *)func_80098BF8(a0);
    if (node != 0) {
        *(u16 *)(node + 0) |= 1;
        *(u16 *)(node + 2) = 0;
        *(s32 *)(node + 8) = a1;
        *(s32 *)(node + 4) = *(s32 *)a0;
        *(s32 *)a0 = (s32)node;
    }
    return node;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098D28);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098DD4);

/**
 * @brief Set battle viewport dimensions if non-negative.
 *
 * Stores a0 to D_80182B5A and D_80182B56 if a0 >= 0.
 * Stores a1 to D_80182B58 if a1 >= 0.
 *
 * @param a0 Width value (stored if >= 0).
 * @param a1 Height value (stored if >= 0).
 */
void func_80098E54(s32 a0, s32 a1) {
    extern s16 D_80182B5A;
    extern s16 D_80182B56;
    extern s16 D_80182B58;
    if (a0 >= 0) {
        D_80182B5A = a0;
        D_80182B56 = a0;
    }
    if (a1 >= 0) {
        D_80182B58 = a1;
    }
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098E7C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098FD8);

/**
 * Converts an integer to a decimal string representation.
 *
 * @param a0 The integer value to convert.
 * @param a1 Pointer to the output buffer.
 * @return Pointer to the end of the written string.
 */
u8 *func_800990A0(s32 a0, u8 *a1) {
    u8 buf[36];
    u8 *dst = a1;
    u8 *p;

    if (a0 < 0) {
        *dst = 0x2D;
        dst++;
        a0 = -a0;
    }

    p = buf + 33;
    buf[33] = 0;

    do {
        p--;
        *p = (a0 % 10) + 0x30;
        a0 = a0 / 10;
    } while (a0 != 0);

    func_80047CA4(dst, p);
    return dst + func_80047CB4(dst);
}

/**
 * Converts an integer to a hexadecimal string representation.
 *
 * @param a0 The integer value to convert.
 * @param a1 Pointer to the output buffer.
 * @return Pointer to the end of the written string.
 */
u8 *func_80099134(s32 a0, u8 *a1) {
    extern u8 D_80182B84[];
    u8 buf[20];
    u8 *dst = a1;
    u8 *p;
    u8 *table;

    if (a0 < 0) {
        *dst = 0x2D;
        dst++;
        a0 = -a0;
    }

    p = buf + 17;
    buf[17] = 0;
    table = D_80182B84;

    do {
        p--;
        *p = *(u8 *)((a0 & 0xF) + (s32)table);
        a0 >>= 4;
    } while (a0 != 0);

    func_80047CA4(dst, p);
    return dst + func_80047CB4(dst);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800991AC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80099204);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800993F4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80099424);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80099464);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009953C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800995F8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80099798);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80099C78);

/**
 * @brief Execute cleanup via func_8009AD00 and return success.
 *
 * Calls func_8009AD00 to perform cleanup/finalization, then always
 * returns 0 to indicate success.
 *
 * @param a0 Entity or context pointer passed to func_8009AD00.
 * @return Always 0.
 */
s32 func_8009A2F4(s32 a0) {
    func_8009AD00(a0);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009A314);

/**
 * @brief Load pointer at a0+0xC, call func_80098D28, return (result == 0) << 1.
 *
 * @note Non-matching: PsyQ 4.3 filled epilogue in PsyQ 4.1 overlay.
 */
INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009A4E0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009A508);

/**
 * @brief Initialize the D_801D3028 linked list with battle update callbacks.
 *
 * Calls func_8009C6D8 for initial setup, then initializes D_801D3028
 * as a linked list (pool at D_801D3038, node size 0x18, capacity 8).
 * Registers four callback functions as nodes. Clears fields on the
 * first callback's node. Finally calls func_8009AD24 and returns
 * the list pointer.
 *
 * @return Pointer to D_801D3028 list header.
 */
u8 *func_8009A650(void) {
    extern u8 D_801D3028[];
    extern u8 D_801D3038[];
    extern s32 func_80099C78();
    extern s32 func_8009A314();
    extern s32 func_8009A508();
    u8 *list;
    u8 *node;
    func_8009C6D8();
    list = D_801D3028;
    func_80098BC0(list, D_801D3038, 0x18, 8);
    node = (u8 *)func_80098C44(list, (s32)func_80099C78);
    node[0x10] = 0;
    node[0x11] = 0;
    node[0x14] = 0;
    func_80098C44(list, (s32)func_8009A2F4);
    func_80098C44(list, (s32)func_8009A314);
    func_80098C44(list, (s32)func_8009A508);
    func_8009AD24();
    return list;
}

/**
 * Sets up animation rectangle parameters based on the entity type.
 *
 * Configures position and size values in a 4-halfword output structure
 * based on the type byte at a0[0]. Type 0 and 1 use vertical strips,
 * type 2 uses a tile grid layout.
 *
 * @param a0 Pointer to entity data (byte 0 = type, byte 1 = column, byte 2 = row).
 * @param a1 Pointer to output rectangle (4 s16 values: x, y, w, h).
 * @return Pointer to the output rectangle, or a1 unchanged if type is unknown.
 */
u8 *func_8009A6EC(u8 *a0, s16 *a1) {
    u8 type = a0[0];

    switch (type) {
    case 0:
        a1[0] = -0x8C;
        {
            u8 r = a0[2];
            s32 w = 0x200;
            a1[2] = w;
            a1[1] = r * 32 - 0x40;
        }
        a1[3] = -(s32)a0[2] + 0xE;
        break;
    case 1:
        a1[0] = 0x8C;
        {
            u8 r = a0[2];
            s32 w = 0x200;
            a1[2] = w;
            a1[1] = r * 32 - 0x40;
        }
        a1[3] = -(s32)a0[2] + 0xE;
        break;
    case 2: {
        s32 col = a0[1];
        a1[0] = (col - 1) * 64;
        {
            u8 row = a0[2];
            a1[2] = 0x200;
            a1[3] = 0x12;
            a1[1] = (row - 1) * 64;
        }
        break;
    }
    default:
        return (u8 *)a1;
    }
    return (u8 *)a1;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009A7A4);

/**
 * @brief Mark a battle entity's flags with bit 2.
 *
 * Looks up an entity index via func_8009A7A4, then sets bit 1 (0x2)
 * in the flags halfword at offset +4 of the entity's 36-byte entry
 * in D_801D31C0.
 *
 * @param a0 Entity search key passed to func_8009A7A4.
 * @param a1 Secondary parameter passed as third arg to func_8009A7A4.
 */
void func_8009A878(s32 a0, s32 a1) {
    extern u8 D_801D31C0[];
    s32 idx = func_8009A7A4(a0, 0, a1);
    if (idx >= 0) {
        s32 base = (s32)D_801D31C0;
        *(u16 *)(base + idx * 36 + 4) |= 2;
    }
}
