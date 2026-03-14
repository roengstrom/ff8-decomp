#include "common.h"

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A1BE0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A1C6C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A1D68);

/**
 * @brief Start or stop a battle object based on its type flag.
 *
 * Looks up the entry at D_80182E70[a0 * 12], checks bit 0 of byte 0.
 * If set, calls func_8002DD58 (stop). Otherwise calls func_8002DD38 (start).
 *
 * @param a0 Object index.
 */
INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2054);

/**
 * @brief Reset all 7 battle objects and finalize.
 *
 * Calls func_8002DD58 for each of the 7 objects (indices 0-6),
 * then calls func_800A44BC to set D_801D49E2.
 */
void func_800A20B0(void) {
    s32 i = 0;
    do {
        func_8002DD58(i);
        i++;
    } while (i < 7);
    func_800A44BC();
}

/**
 * @brief Wrapper that calls func_8002CE84, passing through all arguments.
 */
void func_800A20F4(void) {
    func_8002CE84();
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2114);

/**
 * @brief Clear all 7 battle objects by calling func_8002C7BC with zero params.
 *
 * Iterates indices 0-6, calling func_8002C7BC(i, 0, 0) for each.
 */
void func_800A21C4(void) {
    s32 i = 0;
    do {
        func_8002C7BC(i, 0, 0);
        i++;
    } while (i < 7);
}

/**
 * @brief Clear D_801D4560 to zero.
 */
void func_800A2208(void) {
    extern s32 D_801D4560;
    D_801D4560 = 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2214);

/**
 * @brief Add an entry to the D_801D4500 effects queue if not full.
 *
 * The queue holds up to 8 entries (index 0..7), tracked by D_801D4560.
 * Each entry is 12 bytes: byte[0]=active, byte[1]=a1, byte[2]=a2,
 * byte[3]=a0, word[4]=a3.
 *
 * @param a0 Effect type (stored at byte 3).
 * @param a1 Parameter 1 (stored at byte 1).
 * @param a2 Parameter 2 (stored at byte 2).
 * @param a3 Parameter 3 (stored at word offset 4).
 */
void func_800A22E8(s32 a0, s32 a1, s32 a2, s32 a3) {
    extern s32 D_801D4560;
    extern u8 D_801D4500[];
    s32 count = D_801D4560;
    if (count < 7) {
        u8 *entry;
        D_801D4560 = count + 1;
        entry = D_801D4500 + count * 12;
        entry[0] = 1;
        entry[3] = a0;
        entry[1] = a1;
        entry[2] = a2;
        *(s32 *)(entry + 4) = a3;
    }
}

/** @brief Call func_800A22E8 with a0, 0x80, 0x7F, 0. */
void func_800A233C(s32 a0) {
    func_800A22E8(a0, 0x80, 0x7F, 0);
}

/** @brief Call func_800A22E8 with a0, 0x80, 0x7F, a1. */
void func_800A2364(s32 a0, s32 a1) {
    func_800A22E8(a0, 0x80, 0x7F, a1);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A238C);

/**
 * @brief Initialize animation handler and attach to D_801D3C58 list.
 *
 * Loads animation data from D_80182EC8 via func_80013EE4, then
 * allocates a node in D_801D3C58 with func_800A238C as callback.
 * Clears the node's fields at offsets 0xC and 0xE.
 */
void func_800A247C(void) {
    extern u8 D_80182EC8[];
    extern s32 func_800A238C();
    u8 *node;
    func_80013EE4(D_80182EC8, 0);
    node = (u8 *)func_8009E248((s32)func_800A238C);
    *(s16 *)(node + 0xC) = 0;
    *(s16 *)(node + 0xE) = 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A24B4);

/**
 * @brief Initialize D_801D4568 and set flag bit 2 in D_801A2C74.
 *
 * Calls func_800A24B4 to initialize D_801D4568, then func_800A1D68
 * with mode 4. Finally sets bit 2 (0x4) in D_801A2C74.
 */
void func_800A26C8(void) {
    extern u8 D_801D4568[];
    extern s32 D_801A2C74;
    u8 *base = D_801D4568;
    func_800A24B4(base);
    func_800A1D68(4, base, 0);
    D_801A2C74 |= 0x4;
}

/**
 * @brief Tear down D_801D4568 and clear flag bit 2 in D_801A2C74.
 *
 * Calls func_800A2054 with mode 4, then clears bit 2 (0x4) in D_801A2C74.
 */
void func_800A271C(void) {
    extern s32 D_801A2C74;
    func_800A2054(4);
    D_801A2C74 &= ~0x4;
}

/**
 * @brief Add a rendering command entry based on the alternate screen index.
 *
 * Reads D_801C2DCA, XORs with 1 to get the alternate index, computes
 * an offset of index * 92 into D_801C2DD0, and calls func_80098A1C
 * with the resulting pointer and D_8012E66C.
 *
 * @return Always 0.
 */
s32 func_800A274C(void) {
    extern u8 D_801C2DCA;
    extern u8 D_801C2DD0[];
    extern u8 D_8012E66C[];
    s32 idx = D_801C2DCA ^ 1;
    func_80098A1C(D_801C2DD0 + idx * 92, D_8012E66C);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A279C);

/**
 * @brief Initialize the D_801D4968 linked list with two render callbacks.
 *
 * Sets up D_801D4968 as a linked list (node size 0x10, capacity 4),
 * then appends func_800A279C and func_800A274C as callbacks.
 * Clears bytes 0xC and 0xD on the first node.
 *
 * @return Pointer to D_801D4968 list header.
 */
u8 *func_800A2968(void) {
    extern u8 D_801D4968[];
    extern u8 D_801D4978[];
    extern s32 func_800A279C();
    u8 *list = D_801D4968;
    u8 *node;
    func_80098BC0(list, D_801D4978, 0x10, 4);
    node = (u8 *)func_80098C44(list, (s32)func_800A279C);
    node[0xC] = 0;
    node[0xD] = 0;
    func_80098C44(list, (s32)func_800A274C);
    return list;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A29D4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2A8C);

/**
 * @brief Return the value at index a0 from array D_801D4B20.
 *
 * @param a0 Array index.
 * @return Value at D_801D4B20[a0].
 */
s32 func_800A2B84(s32 a0) {
    extern s32 D_801D4B20[];
    return D_801D4B20[a0];
}

/**
 * @brief Return the value at index a0 from array D_801D4B30.
 *
 * @param a0 Array index.
 * @return Value at D_801D4B30[a0].
 */
s32 func_800A2BA0(s32 a0) {
    extern s32 D_801D4B30[];
    return D_801D4B30[a0];
}

/**
 * @brief Return the value at index a0 from array D_801D4B28.
 *
 * @param a0 Array index.
 * @return Value at D_801D4B28[a0].
 */
s32 func_800A2BBC(s32 a0) {
    extern s32 D_801D4B28[];
    return D_801D4B28[a0];
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2BD8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2D34);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2E44);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2F78);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A2FCC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A30C8);

/**
 * @brief Wrapper for func_800A30C8 that appends flag=1 as the 7th argument.
 *
 * Passes through a0-a3 plus two stack arguments, appending 1 as
 * the final argument to form a 7-arg call to func_800A30C8.
 */
void func_800A31B8(s32 a0, s32 a1, s32 a2, s32 a3, s32 stack0, s32 stack1) {
    func_800A30C8(a0, a1, a2, a3, stack0, stack1, 1);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A31EC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3248);

/**
 * @brief Temporarily adjust a rect's position/size, call func_800A3248, then restore.
 *
 * Saves the original 8 bytes of the rect at a2, increments x/y by 1,
 * decrements w/h by 2, calls func_800A3248(a0, a1), then restores
 * the original values.
 *
 * @param a0 First parameter passed to func_800A3248.
 * @param a1 Second parameter passed to func_800A3248.
 * @param a2 Pointer to rect structure (4 halfwords: x, y, w, h).
 */
void func_800A3320(s32 a0, s32 a1, u8 *a2) {
    u8 *base = a2;
    s32 saved0 = *(s32 *)(base + 0);
    s32 saved1 = *(s32 *)(base + 4);
    *(u16 *)(base + 0) = *(u16 *)(base + 0) + 1;
    *(u16 *)(base + 2) = *(u16 *)(base + 2) + 1;
    *(u16 *)(base + 4) = *(u16 *)(base + 4) - 2;
    *(u16 *)(base + 6) = *(u16 *)(base + 6) - 2;
    func_800A3248(a0, a1);
    *(s32 *)(base + 0) = saved0;
    *(s32 *)(base + 4) = saved1;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3398);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A343C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3528);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3884);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A390C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3C7C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3D2C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A3EE0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A4098);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A40F0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A4250);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A42D0);

/**
 * @brief Apply func_800A42D0 with VSync lock/unlock.
 *
 * @param a0 Parameter passed through to func_800A42D0.
 * @return Result of func_8002A8B8 (VSync unlock).
 */
s32 func_800A443C(s32 a0) {
    s32 saved = a0;
    s32 lock = func_8002A888();
    s32 result = func_800A42D0(saved, lock);
    return func_8002A8B8(result);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A4478);

/**
 * @brief Store a byte value to D_801D49EC.
 *
 * @param a0 Byte value to store.
 */
void func_800A44B0(s32 a0) {
    extern u8 D_801D49EC;
    D_801D49EC = a0;
}

/**
 * @brief Set D_801D49E2 to -256 (0xFF00).
 */
void func_800A44BC(void) {
    extern s16 D_801D49E2;
    D_801D49E2 = -256;
}

/**
 * @brief Reset D_801D49C8 fields and call func_800A2E44.
 *
 * Initializes field +0x1A to 0x100, clears +0x22, +0x1E, and +0x16,
 * then calls func_800A2E44 for further reset.
 */
void func_800A44CC(void) {
    extern u8 D_801D49C8[];
    u8 *base = D_801D49C8;
    *(s16 *)(base + 0x1A) = 0x100;
    base[0x22] = 0;
    *(s16 *)(base + 0x1E) = 0;
    base[0x16] = 0;
    func_800A2E44();
}

/**
 * @brief Initialize D_801D49C8 battle camera structure.
 *
 * Sets position, dimensions, mode fields, clears various flags,
 * then calls func_800A4478 and func_800A2F78 for further init.
 * Also initializes D_801D4B18 to 0 and D_801D4B1A to 0x180.
 *
 * @param a0 X position (stored at offset 0).
 * @param a1 Y position (stored at offset 2).
 */
void func_800A4504(s32 a0, s32 a1) {
    extern u8 D_801D49C8[];
    extern s16 D_801D4B18;
    extern s16 D_801D4B1A;
    u8 *base;
    *(s16 *)D_801D49C8 = a0;
    base = D_801D49C8;
    *(s16 *)(base + 0x4) = 0xA1;
    *(s16 *)(base + 0x6) = 0x9F;
    base[0x21] = 0xB;
    *(s16 *)(base + 0x2) = a1;
    *(s16 *)(base + 0x1A) = 0;
    *(s16 *)(base + 0x18) = 0;
    *(s16 *)(base + 0x1E) = 0;
    *(s16 *)(base + 0x14) = 0;
    base[0x20] = 0;
    base[0x16] = 0;
    base[0x17] = 0;
    base[0x23] = 0;
    base[0x24] = 1;
    func_800A4478(0x1000);
    func_800A2F78();
    D_801D4B18 = 0;
    D_801D4B1A = 0x180;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object4", func_800A458C);
