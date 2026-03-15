#include "common.h"

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009A8CC);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009A970);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009AA68);

/**
 * @brief Call func_80098D28 with D_801D3110.
 */
void func_8009AD00(void) {
    extern u8 D_801D3110[];
    func_80098D28(D_801D3110);
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009AD24);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009AE6C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009B3EC);

/**
 * @brief Reset battle state globals D_801D3328, D_801D3359, and D_801D3340 fields.
 *
 * Clears D_801D3328 (word), D_801D3359 (byte), and sets fields in D_801D3340:
 * halfwords at +6, +0xA, +0x10, +0x14 to 0, and +0xC, +0xE to 1.
 */
void func_8009B494(void) {
    extern s32 D_801D3328;
    extern u8 D_801D3359;
    extern u8 D_801D3340[];
    u8 *base;
    D_801D3328 = 0;
    D_801D3359 = 0;
    base = D_801D3340;
    *(s16 *)(base + 0x6) = 0;
    *(s16 *)(base + 0xA) = 0;
    *(s16 *)(base + 0xC) = 1;
    *(s16 *)(base + 0xE) = 1;
    *(s16 *)(base + 0x10) = 0;
    *(s16 *)(base + 0x14) = 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009B4CC);

/**
 * @brief Look up the active object and initialize its handler.
 *
 * Gets an index via func_8009A7A4. If non-negative, uses it to look up
 * the object type from D_801D31C0 (stride 36) and passes it to func_800A2114.
 */
void func_8009B644(void) {
    extern u8 D_801D31C0[];
    s32 idx = func_8009A7A4();
    if (idx >= 0) {
        u8 *base = D_801D31C0;
        u8 type = *(base + idx * 36);
        func_800A2114(type);
    }
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009B690);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009B7B4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009B8D8);

/**
 * @brief Adjust a battle speed/volume parameter based on controller input.
 *
 * If D_801D332E has bit 0x8000 set and the current value at a0 is positive,
 * decrements the value and triggers a sound effect. If D_801D332E has bit
 * 0x2000 set and the value is less than 4, increments it and triggers a
 * sound effect. Stores the final value to D_801D335C.
 *
 * @param a0 Pointer to a halfword value to adjust.
 */
void func_8009BA4C(u16 *a0) {
    extern u16 D_801D332E;
    extern s16 D_801D335C;
    u16 val;

    if (D_801D332E & 0x8000) {
        if (*(s16 *)a0 > 0) {
            func_800A233C(1);
            val = *a0 - 1;
            *a0 = val;
            goto store;
        }
    }
    if (D_801D332E & 0x2000) {
        if (*(s16 *)a0 < 4) {
            func_800A233C(1);
            val = *a0 + 1;
            *a0 = val;
            goto store;
        }
    }
store:
    D_801D335C = *a0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BAF4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BD24);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BDC0);

/**
 * @brief Initialize the D_801D3380 linked list with a battle callback.
 *
 * Sets up D_801D3380 as a linked list (pool at D_801D3360, node size 0x14,
 * capacity 1), then appends func_8009BDC0 as a callback. Sets byte fields
 * 0xC, 0xD, 0xE on the node from the parameters. Resets D_801D3340 fields
 * at +0xC and +0xE to 1.
 *
 * @param a0 Value stored at node byte 0xD.
 * @param a1 Value stored at node byte 0xE.
 * @return Pointer to D_801D3380 list header.
 */
u8 *func_8009C010(s32 a0, s32 a1) {
    extern u8 D_801D3380[];
    extern u8 D_801D3360[];
    extern u8 D_801D3340[];
    extern s32 func_8009BDC0();
    u8 *list = D_801D3380;
    u8 *node;

    func_80098BC0(list, D_801D3360, 0x14, 1);
    node = (u8 *)func_80098C44(list, (s32)func_8009BDC0);
    {
        u8 *base;
        node[0xC] = 0;
        node[0xD] = a0;
        node[0xE] = a1;
        base = D_801D3340;
        *(s16 *)(base + 0xC) = 1;
        *(s16 *)(base + 0xE) = 1;
    }
    return list;
}

/**
 * @brief Set the type for a battle entity in D_801D31C0 and optionally trigger an effect.
 *
 * Computes entry at D_801D31C0 + a0 * 36, sets entry[1] = a1 (type),
 * clears entry[2..3]. If type is in range [2, 5], calls func_800A2364(0x5A, 1).
 *
 * @param a0 Entity index.
 * @param a1 Entity type.
 */
void func_8009C0A0(s32 a0, s32 a1) {
    extern u8 D_801D31C0[];
    u8 *base = D_801D31C0;
    u8 *entry = base + a0 * 36;
    entry[1] = a1;
    *(s16 *)(entry + 2) = 0;
    if (a1 < 6) {
        if (a1 >= 2) {
            func_800A2364(0x5A, 1);
        }
    }
}

/**
 * @brief Check if any battle entity in D_801D31C0 has a non-zero type.
 *
 * Iterates up to 10 entries (stride 36) and checks byte at offset 1.
 *
 * @return 1 if any entity has non-zero type, 0 otherwise.
 */
s32 func_8009C0F4(void) {
    extern u8 D_801D31C0[];
    s32 i = 0;
    u8 *entry = D_801D31C0;
    do {
        if (entry[1] != 0) {
            return 1;
        }
        i++;
        entry += 0x24;
    } while (i < 10);
    return 0;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C12C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C440);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C59C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C6D8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C890);

/**
 * Starts an animation sequence on a battle entity.
 *
 * Looks up entity data from the global entity table, then calls the
 * animation handler with the entity's type flag and callback info.
 *
 * @param a0 Animation command or mode.
 * @param a1 Entity index.
 * @param a2 Animation parameter.
 * @param a3 Additional parameter (passed as 5th stack arg to handler).
 */
void func_8009C978(s32 a0, s32 a1, s32 a2, s32 a3) {
    extern u8 D_801D31C0[];
    u8 *base = D_801D31C0;
    u8 *entry;
    u8 *result;

    entry = base + a1 * 36;
    result = (u8 *)func_8009C890(a0, entry[0], *(s32 *)(entry + 8) & 1, a2, a3);
    result[3] = a1;
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C9D4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009CB10);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009CD38);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009CF5C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009D058);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009D154);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009D2B0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009D72C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009DBE8);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009DECC);

/**
 * @brief Initialize the D_801D3C58 list with node pool D_801D3798.
 *
 * Sets up a linked list with node size 0x4C and capacity 0x10.
 */
void func_8009E1F0(void) {
    extern u8 D_801D3C58[];
    extern u8 D_801D3798[];
    func_80098BC0(D_801D3C58, D_801D3798, 0x4C, 0x10);
}

/**
 * @brief Call func_80098D28 with D_801D3C58.
 */
void func_8009E224(void) {
    extern u8 D_801D3C58[];
    func_80098D28(D_801D3C58);
}
