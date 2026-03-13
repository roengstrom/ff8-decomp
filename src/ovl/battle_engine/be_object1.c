#include "common.h"

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009822C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098304);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800984DC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098690);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098828);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800988D4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800988E0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098A1C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098A6C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098AB4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098B08);

void func_80098B68(void) {
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098B70);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098B80);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098BA0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098BC0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098BF8);

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

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098E54);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098E7C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80098FD8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_800990A0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_80099134);

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

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009A650);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object1", func_8009A6EC);

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
