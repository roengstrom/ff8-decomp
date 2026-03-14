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

/**
 * @brief Handle directional input for battle entity navigation.
 *
 * Checks D_801D332E input flags for up (0x1000), down (0x4000), and
 * confirm (0x2000) buttons. For up/down, searches for a valid entity
 * at the adjusted index and updates the selection. For confirm, checks
 * D_801D3328 flags to set D_801D3358 action mode.
 *
 * @param a0 Pointer to a 4-byte structure with entity index at offset 2.
 */
void func_8009B690(u8 *a0) {
    extern u8 D_801D332E[];
    extern u8 D_801D3328[];
    extern u8 D_801D3358[];

    if (*(u16 *)D_801D332E & 0x1000) {
        s32 idx = *(s16 *)(a0 + 2);
        if (func_8009A7A4(0, 0, idx - 1) >= 0) {
            func_800A233C(1);
            *(s16 *)(a0 + 2) = *(u16 *)(a0 + 2) - 1;
            goto end;
        }
    }
    if (*(u16 *)D_801D332E & 0x4000) {
        s32 idx = *(s16 *)(a0 + 2);
        if (func_8009A7A4(0, 0, idx + 1) >= 0) {
            func_800A233C(1);
            *(s16 *)(a0 + 2) = *(u16 *)(a0 + 2) + 1;
            goto end;
        }
    }
    if (*(u16 *)D_801D332E & 0x2000) {
        s32 flags = *(s32 *)D_801D3328;
        if (flags & 8) {
            D_801D3358[0] = 3;
            return;
        }
        if (flags & 4) {
            D_801D3358[0] = 2;
            return;
        }
    }
end:
    func_8009A878(0, *(s16 *)(a0 + 2));
    func_8009B644(0, 0, *(s16 *)(a0 + 2));
}

/**
 * @brief Handle directional input for battle entity navigation (type 1).
 *
 * Checks D_801D332E input flags for up (0x1000), down (0x4000), and
 * confirm (0x8000) buttons. For up/down, searches for a valid entity
 * at the adjusted index via func_8009A7A4 with type 1 and updates the
 * selection. For confirm, checks D_801D3328 flags to set D_801D3358
 * action mode.
 *
 * @param a0 Pointer to a structure with entity index at offset 2.
 */
void func_8009B7B4(u8 *a0) {
    extern u8 D_801D332E[];
    extern u8 D_801D3328[];
    extern u8 D_801D3358[];

    if (*(u16 *)D_801D332E & 0x1000) {
        s32 idx = *(s16 *)(a0 + 2);
        if (func_8009A7A4(1, 0, idx - 1) >= 0) {
            func_800A233C(1);
            *(s16 *)(a0 + 2) = *(u16 *)(a0 + 2) - 1;
            goto end;
        }
    }
    if (*(u16 *)D_801D332E & 0x4000) {
        s32 idx = *(s16 *)(a0 + 2);
        if (func_8009A7A4(1, 0, idx + 1) >= 0) {
            func_800A233C(1);
            *(s16 *)(a0 + 2) = *(u16 *)(a0 + 2) + 1;
            goto end;
        }
    }
    if (*(u16 *)D_801D332E & 0x8000) {
        s32 flags = *(s32 *)D_801D3328;
        if (flags & 8) {
            D_801D3358[0] = 3;
            return;
        }
        if (flags & 2) {
            D_801D3358[0] = 1;
            return;
        }
    }
end:
    func_8009A878(1, *(s16 *)(a0 + 2));
    func_8009B644(1, 0, *(s16 *)(a0 + 2));
}

/**
 * @brief Handle directional input for battle entity grid navigation (type 2).
 *
 * Processes input flags from D_801D332E to navigate a 2D grid:
 * - 0x8000 (right): decrement column (s0[0])
 * - 0x2000 (left): increment column
 * - 0x1000 (up): decrement row (s0[2])
 * - 0x4000 (down): increment row
 * Column bounds [0,2], row bounds [0,2]. Out-of-bounds column checks
 * D_801D3328 flags to set D_801D3358 action mode.
 *
 * @param a0 Pointer to structure with column at offset 0, row at offset 2.
 */
void func_8009B8D8(u8 *a0) {
    extern u8 D_801D332E[];
    extern u8 D_801D3328[];
    extern u8 D_801D3358[];
    u16 input = *(u16 *)D_801D332E;
    s32 val;

    if (input & 0x8000) {
        val = *(u16 *)(a0 + 0) - 1;
        *(s16 *)(a0 + 0) = val;
        if ((s16)val < 0) {
            goto check_neg;
        }
        goto call_sfx;
    }
    if (input & 0x2000) {
        val = *(u16 *)(a0 + 0) + 1;
        *(s16 *)(a0 + 0) = val;
        if ((s16)val >= 3) {
            goto end_section;
        }
    call_sfx:
        func_800A233C(1);
        goto end_section;
    }
    if (input & 0x1000) {
        if (*(s16 *)(a0 + 2) > 0) {
            func_800A233C(1);
            val = *(u16 *)(a0 + 2) - 1;
            goto store_row;
        }
    }
    if (*(u16 *)D_801D332E & 0x4000) {
        if (*(s16 *)(a0 + 2) < 2) {
            func_800A233C(1);
            val = *(u16 *)(a0 + 2) + 1;
        store_row:
            *(s16 *)(a0 + 2) = val;
        }
    }
end_section:
    val = *(s16 *)(a0 + 0);
    if (val >= 0) {
        if (val < 3) {
            goto done;
        }
        {
            s32 flags = *(s32 *)D_801D3328;
            *(s16 *)(a0 + 0) = 2;
            if (flags & 4) {
                D_801D3358[0] = 2;
            }
        }
        goto done;
    }
check_neg:
    {
        s32 flags = *(s32 *)D_801D3328;
        *(s16 *)(a0 + 0) = 0;
        if (flags & 2) {
            D_801D3358[0] = 1;
        }
    }
done:
    func_8009B644(2, *(s16 *)(a0 + 0), *(s16 *)(a0 + 2));
}

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BA4C);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BAF4);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BD24);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009BDC0);

INCLUDE_ASM("asm/ovl/battle_engine/nonmatchings/be_object2", func_8009C010);

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
