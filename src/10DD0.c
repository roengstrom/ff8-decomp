#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"
#include "gf.h"

u8 *func_80020FBC(u16 a0, s32 a1);

/** @brief Empty stub at the end of the sound engine region.
 *  Followed by ~4KB of zero padding before the game code resumes.
 */
INCLUDE_ASM("asm/nonmatchings/10DD0", func_8001F5C8);

/**
 * @brief Game code VSync handler. Clears render mode if func_80035148 signals completion.
 *
 * Called from the VSync dispatch (g_renderMode == 4). Invokes func_80035294
 * for per-frame processing, then checks func_80035148's return. If non-zero,
 * sets g_renderMode to 0 (RENDER_IDLE) to signal the main loop.
 */
void func_800205D0(void) {
    extern s16 g_renderMode;
    void func_80035294(void);
    s32 func_80035148(void);

    func_80035294();
    if (func_80035148() != 0) {
        g_renderMode = 0;
    }
}


/**
 * @brief Zero memory in 16-byte (4-word) chunks.
 * @param ptr Pointer to memory to clear.
 * @param count Number of 16-byte iterations.
 */
void func_80020608(s32 *ptr, s32 count) {
    s32 i = 0;
    if (count <= 0) return;
    do {
        *ptr = 0;
        ptr++;
        *ptr = 0;
        ptr++;
        *ptr = 0;
        ptr++;
        *ptr = 0;
        ptr++;
    } while (++i < count);
}


/**
 * @brief Copy a0 bytes from src to dst.
 * @param src Source byte pointer.
 * @param dst Destination byte pointer.
 * @param len Number of bytes to copy.
 */
void func_80020644(u8 *src, u8 *dst, s32 len) {
    s32 i = 0;
    if (len <= 0) return;
    do {
        *dst = *src;
        src++;
        dst++;
    } while (++i < len);
}


/** @brief Sets bit 0x1 in byte at g_gameState[a0 * 68 + 0x61].
 *  @param a0 Table index (stride 68 bytes).
 */
void func_80020670(s32 a0) {
    extern u8 g_gameState[];
    s32 base = (s32)g_gameState;
    *(u8 *)(base + a0 * 68 + 0x61) |= 1;
}


/** @brief Returns a pointer to the Boko name field in the save header (g_gameState + 0x3C). */
u8 *func_8002069C(void) {
    extern u8 g_gameState[];
    return g_gameState + 0x3C;
}


/** @brief Returns a pointer to the Angelo name field in the save header (g_gameState + 0x30). */
u8 *func_800206A8(void) {
    extern u8 g_gameState[];
    return g_gameState + 0x30;
}


/** @brief Resolves param from GfData.subTableU[a0] (stride 20) via func_80020FBC. */
s32 func_800206B4(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableU (+0x4A6C), ptrSubTableU (+0xD8) */
    return func_80020FBC(*(u16 *)(base + a0 * 20 + 0x4A6C), *(s32 *)(base + 0xD8));
}


/**
 * @brief Resolve GF name pointer from GfData.subTableT[a0] (stride 8).
 * @param a0 GF index (0 returns default D_800773A8 pointer).
 * @return Pointer from func_80020FBC lookup, or &D_800773A8 if a0 is 0.
 */
u8 *func_800206F4(s32 a0) {
    extern u8 g_gfData[];
    extern u8 g_gameState[];
    u8 *result;

    if (a0 != 0) {
        s32 base = (s32)g_gfData;
        /* subTableT (+0x4A5C), ptrSubTableT (+0xD4) */
        result = func_80020FBC(*(u16 *)(base + a0 * 8 + 0x4A5C), *(s32 *)(base + 0xD4));
    } else {
        result = g_gameState + 0x30;
    }
    return result;
}


/** @brief Resolves param0 from GfData.subTableS[a0] (stride 32) via func_80020FBC. */
s32 func_80020740(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableS (+0x48B8), ptrSubTableS (+0xD0) */
    return func_80020FBC(*(u16 *)(base + a0 * 32 + 0x48B8), *(s32 *)(base + 0xD0));
}


/** @brief Resolves param1 from GfData.subTableS[a0] (stride 32, +2) via func_80020FBC. */
s32 func_80020778(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableS (+0x48BA), ptrSubTableS (+0xD0) */
    return func_80020FBC(*(u16 *)(base + a0 * 32 + 0x48BA), *(s32 *)(base + 0xD0));
}


/** @brief Resolves param0 from GfData.subTableR[a0] (stride 24) via func_80020FBC. */
s32 func_800207B0(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableR (+0x47F8), ptrSubTableR (+0xCC) */
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x47F8), *(s32 *)(base + 0xCC));
}


/** @brief Resolves param1 from GfData.subTableR[a0] (stride 24, +2) via func_80020FBC. */
s32 func_800207F0(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableR (+0x47FA), ptrSubTableR (+0xCC) */
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x47FA), *(s32 *)(base + 0xCC));
}


/** @brief Resolves param0 from GfData.elementData24[a0] (stride 24) via func_80020FBC. */
s32 func_80020830(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* elementData24 (+0x3744), ptrElementData24 (+0x94) */
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x3744), *(s32 *)(base + 0x94));
}


/** @brief Resolves param1 from GfData.elementData24[a0] (stride 24, +2) via func_80020FBC. */
s32 func_80020870(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* elementData24 (+0x3746), ptrElementData24 (+0x94) */
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x3746), *(s32 *)(base + 0x94));
}


/** @brief Resolves param0 from GfData.subTableQ[a0] (stride 16) via func_80020FBC. */
s32 func_800208B0(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableQ (+0x44F8), ptrSubTableQ (+0xC8) */
    return func_80020FBC(*(u16 *)(base + a0 * 16 + 0x44F8), *(s32 *)(base + 0xC8));
}


/** @brief Resolves param1 from GfData.subTableQ[a0] (stride 16, +2) via func_80020FBC. */
s32 func_800208E8(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableQ (+0x44FA), ptrSubTableQ (+0xC8) */
    return func_80020FBC(*(u16 *)(base + a0 * 16 + 0x44FA), *(s32 *)(base + 0xC8));
}


/** @brief Resolves param0 from GfData.subTableP[a0] (stride 24) via func_80020FBC. */
s32 func_80020920(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableP (+0x4480), ptrSubTableP (+0xC4) */
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x4480), *(s32 *)(base + 0xC4));
}


/** @brief Resolves param1 from GfData.subTableP[a0] (stride 24, +2) via func_80020FBC. */
s32 func_80020960(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* subTableP (+0x4482), ptrSubTableP (+0xC4) */
    return func_80020FBC(*(u16 *)(base + a0 * 24 + 0x4482), *(s32 *)(base + 0xC4));
}


/**
 * @brief Resolve AbilityEntry.statParam0 from the appropriate ability range table.
 * @param a0 Ability ID that selects the sub-table (see AbilityEntry ranges in gf.h).
 * @return Result of func_80020FBC with the entry's statParam0 and the range base pointer.
 */
u8 *func_800209A0(s32 a0) {
    extern u8 g_gfData[];
    u16 param;
    s32 arg2;

    if (a0 < 0x14) {
        s32 base = (s32)g_gfData;
        /* abilityRangeI (+0x40E0), AbilityEntry.statParam0 */
        param = *(u16 *)(base + a0 * 8 + 0x40E0);
        arg2 = *(s32 *)(base + 0xA8);
    } else if ((u32)(a0 - 0x14) < 0x13) {
        s32 idx = a0 - 0x14;
        s32 base = (s32)g_gfData;
        /* abilityRangeJ (+0x4180) */
        param = *(u16 *)(base + idx * 8 + 0x4180);
        arg2 = *(s32 *)(base + 0xAC);
    } else if ((u32)(a0 - 0x27) < 0x13) {
        s32 idx = a0 - 0x27;
        s32 base = (s32)g_gfData;
        /* abilityRangeK (+0x4218) */
        param = *(u16 *)(base + idx * 8 + 0x4218);
        arg2 = *(s32 *)(base + 0xB0);
    } else if ((u32)(a0 - 0x3A) < 0x14) {
        s32 idx = a0 - 0x3A;
        s32 base = (s32)g_gfData;
        /* abilityRangeL (+0x42B0) */
        param = *(u16 *)(base + idx * 8 + 0x42B0);
        arg2 = *(s32 *)(base + 0xB4);
    } else if ((u32)(a0 - 0x4E) < 0x5) {
        s32 idx = a0 - 0x4E;
        s32 base = (s32)g_gfData;
        /* abilityRangeM (+0x4350) */
        param = *(u16 *)(base + idx * 8 + 0x4350);
        arg2 = *(s32 *)(base + 0xB8);
    } else {
        s32 idx = a0 - 0x53;
        if ((u32)idx >= 0x9) {
            s32 base = (s32)g_gfData;
            s32 idx2 = a0 - 0x5C;
            /* abilityRangeO (+0x43C0) */
            param = *(u16 *)(base + idx2 * 8 + 0x43C0);
            arg2 = *(s32 *)(base + 0xC0);
        } else {
            s32 base = (s32)g_gfData;
            /* abilityRangeN (+0x4378) */
            param = *(u16 *)(base + idx * 8 + 0x4378);
            arg2 = *(s32 *)(base + 0xBC);
        }
    }
    return func_80020FBC(param, arg2);
}


/**
 * @brief Resolve AbilityEntry.statParam1 from the appropriate ability range table.
 * @param a0 Ability ID that selects the sub-table (see AbilityEntry ranges in gf.h).
 * @return Result of func_80020FBC with the entry's statParam1 and the range base pointer.
 */
u8 *func_80020AD4(s32 a0) {
    extern u8 g_gfData[];
    u16 param;
    s32 arg2;

    if (a0 < 0x14) {
        s32 base = (s32)g_gfData;
        /* abilityRangeI (+0x40E2), AbilityEntry.statParam1 */
        param = *(u16 *)(base + a0 * 8 + 0x40E2);
        arg2 = *(s32 *)(base + 0xA8);
    } else if ((u32)(a0 - 0x14) < 0x13) {
        s32 idx = a0 - 0x14;
        s32 base = (s32)g_gfData;
        /* abilityRangeJ (+0x4182) */
        param = *(u16 *)(base + idx * 8 + 0x4182);
        arg2 = *(s32 *)(base + 0xAC);
    } else if ((u32)(a0 - 0x27) < 0x13) {
        s32 idx = a0 - 0x27;
        s32 base = (s32)g_gfData;
        /* abilityRangeK (+0x421A) */
        param = *(u16 *)(base + idx * 8 + 0x421A);
        arg2 = *(s32 *)(base + 0xB0);
    } else if ((u32)(a0 - 0x3A) < 0x14) {
        s32 idx = a0 - 0x3A;
        s32 base = (s32)g_gfData;
        /* abilityRangeL (+0x42B2) */
        param = *(u16 *)(base + idx * 8 + 0x42B2);
        arg2 = *(s32 *)(base + 0xB4);
    } else if ((u32)(a0 - 0x4E) < 0x5) {
        s32 idx = a0 - 0x4E;
        s32 base = (s32)g_gfData;
        /* abilityRangeM (+0x4352) */
        param = *(u16 *)(base + idx * 8 + 0x4352);
        arg2 = *(s32 *)(base + 0xB8);
    } else {
        s32 idx = a0 - 0x53;
        if ((u32)idx >= 0x9) {
            s32 base = (s32)g_gfData;
            s32 idx2 = a0 - 0x5C;
            /* abilityRangeO (+0x43C2) */
            param = *(u16 *)(base + idx2 * 8 + 0x43C2);
            arg2 = *(s32 *)(base + 0xC0);
        } else {
            s32 base = (s32)g_gfData;
            /* abilityRangeN (+0x437A) */
            param = *(u16 *)(base + idx * 8 + 0x437A);
            arg2 = *(s32 *)(base + 0xBC);
        }
    }
    return func_80020FBC(param, arg2);
}


/**
 * @brief Resolve a data pointer for a character or GF entity.
 * @param a0 Entity index; < 0x40 resolves via GfJunctionEntry[a0] in GfData.junctionData,
 *           >= 0x40 returns direct pointer into D_800762C8 (stride 68).
 * @return Resolved data pointer.
 */
u8 *func_80020C08(s32 a0) {
    extern u8 g_gfData[];
    extern u8 D_800762C8[];

    if (a0 < 0x40) {
        u8 *gfBase = g_gfData;
        /* junctionData (+0x21C, stride 60), ptrGfSpellData (+0x84) */
        return (u8 *)func_80020FBC(*(u16 *)(gfBase + a0 * 60 + 0x21C), *(s32 *)(gfBase + 0x84));
    }
    return D_800762C8 + a0 * 68;
}


/**
 * @brief Resolve a secondary data pointer for a character or GF entity.
 * @param a0 Entity index; < 0x40 uses GfJunctionEntry[a0]+2 in junctionData,
 *           >= 0x40 uses GfAbilityTableEntry[idx] in abilityTable132.
 * @return Resolved data pointer via func_80020FBC.
 */
s32 func_80020C6C(s32 a0) {
    extern u8 g_gfData[];

    if (a0 < 0x40) {
        u8 *gfBase = g_gfData;
        /* junctionData (+0x21E, stride 60), ptrGfSpellData (+0x84) */
        return func_80020FBC(*(u16 *)(gfBase + a0 * 60 + 0x21E), *(s32 *)(gfBase + 0x84));
    }
    {
        u8 *gfBase = g_gfData;
        s32 idx = a0 - 0x40;
        /* abilityTable132 (+0xF7A, stride 132), ptrAbilityTable132 (+0x88) */
        return func_80020FBC(*(u16 *)(gfBase + idx * 132 + 0xF7A), *(s32 *)(gfBase + 0x88));
    }
}


/**
 * @brief Resolve stat param0 from GfData stat tables.
 * @param a0 Stat index; >= 0x21 uses statTable4 (stride 4), otherwise statTable24 (stride 24).
 * @return Result of func_80020FBC.
 */
s32 func_80020CE0(s32 a0) {
    extern u8 g_gfData[];
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)g_gfData;
        s32 idx = a0 - 0x21;
        /* statTable4 (+0x3C48), ptrStatTable4 (+0xA0) */
        param = *(u16 *)(base + idx * 4 + 0x3C48);
        arg2 = *(s32 *)(base + 0xA0);
    } else {
        s32 base = (s32)g_gfData;
        /* statTable24 (+0x3930), ptrStatTable24 (+0x9C) */
        param = *(u16 *)(base + a0 * 24 + 0x3930);
        arg2 = *(s32 *)(base + 0x9C);
    }
    return func_80020FBC(param, arg2);
}


/**
 * @brief Resolve stat param1 from GfData stat tables (+2 offset from func_80020CE0).
 * @param a0 Stat index; >= 0x21 uses statTable4 (stride 4), otherwise statTable24 (stride 24).
 * @return Result of func_80020FBC.
 */
s32 func_80020D4C(s32 a0) {
    extern u8 g_gfData[];
    u16 param;
    s32 arg2;

    if (a0 >= 0x21) {
        s32 base = (s32)g_gfData;
        s32 idx = a0 - 0x21;
        /* statTable4 (+0x3C4A), ptrStatTable4 (+0xA0) */
        param = *(u16 *)(base + idx * 4 + 0x3C4A);
        arg2 = *(s32 *)(base + 0xA0);
    } else {
        s32 base = (s32)g_gfData;
        /* statTable24 (+0x3932), ptrStatTable24 (+0x9C) */
        param = *(u16 *)(base + a0 * 24 + 0x3932);
        arg2 = *(s32 *)(base + 0x9C);
    }
    return func_80020FBC(param, arg2);
}


/**
 * @brief Resolve a data pointer from D_80078720 entity table (stride 464).
 * @param a0 Entity index. Reads entity type at offset 0x1C3:
 *           0 returns D_80077390, 4 returns D_8007739C,
 *           else looks up GfCurveEntry in GfData.xpCurves36.
 * @return Resolved data pointer.
 */
s32 func_80020DB8(s32 a0) {
    extern u8 D_80078720[];
    extern u8 g_gfData[];
    extern u8 g_gameState[];
    u8 *base = D_80078720;
    u8 *entry;

    entry = base + a0 * 464;

    if (entry[0x1C3] == 0) {
        return (s32)(g_gameState + 0x18);
    }
    if (entry[0x1C3] == 4) {
        return (s32)(g_gameState + 0x24);
    }
    {
        u8 *gfBase = g_gfData;
        /* xpCurves36 (+0x37A4, stride 36), ptrGfCurve36 (+0x98) */
        return func_80020FBC(
            *(u16 *)(gfBase + entry[0x1C3] * 36 + 0x37A4),
            *(s32 *)(gfBase + 0x98));
    }
}


/**
 * @brief Resolve a data pointer based on entity type.
 * @param a0 Entity type; 0 returns D_80077390, 4 returns D_8007739C,
 *           otherwise looks up GfCurveEntry[a0] in GfData.xpCurves36.
 * @return Resolved data pointer.
 */
u8 *func_80020E4C(s32 a0) {
    extern u8 g_gameState[];
    extern u8 g_gfData[];

    if (a0 == 0) {
        return g_gameState + 0x18;
    }
    if (a0 == 4) {
        return g_gameState + 0x24;
    }
    {
        s32 base = (s32)g_gfData;
        /* xpCurves36 (+0x37A4, stride 36), ptrGfCurve36 (+0x98) */
        return func_80020FBC(*(u16 *)(base + a0 * 36 + 0x37A4), *(s32 *)(base + 0x98));
    }
}


/** @brief Resolves param from GfData.levelCurve12[a0] (stride 12) via func_80020FBC. */
s32 func_80020EB4(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* levelCurve12 (+0x35B8, stride 12), ptrLevelCurve12 (+0x90) */
    return func_80020FBC(*(u16 *)(base + a0 * 12 + 0x35B8), *(s32 *)(base + 0x90));
}


/** @brief Resolves AbilityEntry.statParam0 from GfData.statTable8[a0] via func_80020FBC. */
s32 func_80020EF4(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* statTable8 (+0xE4, stride 8), ptrStatTable8 (+0x80) */
    return func_80020FBC(*(u16 *)(base + a0 * 8 + 0xE4), *(s32 *)(base + 0x80));
}


/** @brief Resolves AbilityEntry.statParam1 from GfData.statTable8[a0] via func_80020FBC. */
s32 func_80020F2C(s32 a0) {
    extern u8 g_gfData[];
    s32 base = (s32)g_gfData;
    /* statTable8 (+0xE6, stride 8), ptrStatTable8 (+0x80) */
    return func_80020FBC(*(u16 *)(base + a0 * 8 + 0xE6), *(s32 *)(base + 0x80));
}


/** @brief Wrapper that calls func_80020F84 with argument 3. */
s32 func_80020F64(void) {
    return func_80020F84(3);
}


/**
 * @brief Look up a u16 from GfData.subTableV[a0] (stride 2) and resolve via func_80020FBC.
 * @param a0 Index into subTableV.
 * @return Resolved data pointer.
 */
s32 func_80020F84(s32 a0) {
    extern u8 g_gfData[];
    u8 *base = g_gfData;
    /* subTableV (+0x4D08, stride 2), ptrSubTableV (+0xE0) */
    return func_80020FBC(*(u16 *)(base + a0 * 2 + 0x4D08), *(s32 *)(base + 0xE0));
}


/**
 * @brief Resolve a data pointer for a given entity index.
 * @param a0 Entity index (0xFFFF = no entity, returns default pointer).
 * @param a1 Base offset into the data region.
 * @return Pointer into g_gfData at offset a1+a0, or D_80052898 if a0 is 0xFFFF.
 */
u8 *func_80020FBC(u16 a0, s32 a1) {
    extern u8 g_gfData[];
    extern u8 D_80052898[];
    u8 *result;
    if (a0 != 0xFFFF) {
        result = a1 + (a0 + g_gfData);
    } else {
        result = D_80052898;
    }
    return result;
}


/**
 * @brief Decrement the count byte for all entries matching itemId in the
 * status array at g_gameState + 0xB44.
 * @param itemId The ID to match. If 0, the function returns immediately.
 */
void func_80020FEC(s32 itemId) {
    extern u8 g_gameState[];
    u8 *ptr;
    s32 i;

    if (itemId == 0) {
        return;
    }

    i = 0;
    ptr = g_gameState;
top:
    if (ptr[0xB44] == itemId) {
        ptr[0xB45] = ptr[0xB45] - 1;
    }
    i++;
    ptr += 2;
    if (i < 0xC6) goto top;
}


/**
 * @brief Add an item to the inventory array D_80077EBC.
 * Searches for an existing entry with matching ID to increment count,
 * or places the item in the first empty slot. Count is capped at 100.
 * @param itemId Item identifier to add (0 = no-op).
 * @param amount Quantity to add.
 * @return 0 if added successfully (count < 100), 1 if capped or inventory full.
 */
s32 func_80021034(s32 itemId, s32 amount) {
    extern u8 D_80077EBC[];
    u8 *base = D_80077EBC;
    u8 *ptr;
    s32 i;

    if (itemId == 0) return 0;

    ptr = base;
    i = 0;

    do {
        if (*ptr == itemId) {
            s32 newCount;
            ptr++;
            newCount = *ptr + amount;
            *ptr = newCount;
            if ((u32)(newCount & 0xFF) < 100) return 0;
            *ptr = 100;
            return 1;
        }
        i++;
        ptr += 2;
    } while (i < 0xC6);

    ptr = base;
    i = 0;

    do {
        if (*ptr == 0) {
            s32 newCount;
            *ptr = itemId;
            ptr++;
            newCount = *ptr + amount;
            *ptr = newCount;
            if ((u32)(newCount & 0xFF) < 100) return 0;
            *ptr = 100;
            return 1;
        }
        i++;
        ptr += 2;
    } while (i < 0xC6);

    return 1;
}


/**
 * @brief Check if an entity has a specific ability and whether its count is maxed.
 * Searches 32 ability slots (2 bytes each: ID + count) in g_gameState at offset 0x4A0.
 * @param entityIdx Entity index (stride 152 in g_gameState).
 * @param abilityId Ability ID to search for.
 * @return 0 if ability not maxed or empty slot exists, 1 if ability count >= 100, 2 if all slots full.
 */
INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021108);


/**
 * @brief Add or increment an ability in an entity's ability slot table.
 * Searches 32 slots (2 bytes each: ID + count) in g_gameState at offset 0x4A0.
 * @param entityIdx Entity index (stride 152 in g_gameState).
 * @param abilityId Ability ID to add/increment.
 * @return 0 if added/incremented, 1 if count already >= 100, 2 if all slots full.
 */
s32 func_800211B4(s32 entityIdx, s32 abilityId) {
    extern u8 g_gameState[];
    register s32 i asm("$7") = 0; //FIXME: forces i to a3 for register allocation match
    s32 entry;
    s32 newCount;

    if (abilityId != 0) goto search;
    return 0;

increment:
    *(u8 *)(entry + 0x4A1) = newCount;
    return 0;

new_slot:
    {
        s32 count = *(u8 *)(entityIdx + 0x4A1);
        *(u8 *)(entityIdx + 0x4A0) = abilityId;
        count++;
        *(u8 *)(entityIdx + 0x4A1) = count;
    }
    return 0;

search:
    {
        s32 base = (s32)g_gameState;
        s32 off = entityIdx * 152;
    loop1:
        entry = off + base;
        if (*(u8 *)(entry + 0x4A0) == abilityId) {
            u32 count = *(u8 *)(entry + 0x4A1);
            if (count < 100) {
                newCount = count + 1;
                goto increment;
            }
            return 1;
        }
        i++;
        off += 2;
        if (i < 0x20) goto loop1;
    }

    i = 0;
    {
        s32 base = (s32)g_gameState;
        s32 off = entityIdx * 152;
    loop2:
        entityIdx = off + base;
        if (*(u8 *)(entityIdx + 0x4A0) == 0) goto new_slot;
        i++;
        off += 2;
        if (i < 0x20) goto loop2;
    }

    return 2;
}


/**
 * @brief Check if a character's ability list contains a specific ability ID.
 * @param a0 Character index used to look up a slot ID in g_gameState at offset 0xAF4.
 * @param a1 Ability ID to search for; returns 0 immediately if a1 is 0.
 * @return 1 if the ability is found in the character's 20-entry ability list, 0 otherwise.
 */
s32 func_80021290(a0, a1)

s32 a0;
s32 a1;
{
    extern u8 g_gameState[];
    s32 base;
    u8 slot_id;
    s32 i;
    s32 ptr;

    if (a1 == 0) return 0;

    base = (s32)g_gameState;
    slot_id = *(u8 *)(a0 + base + 0xAF4);
    i = 0;
    ptr = base + slot_id * 152;
    while (i < 20) {
        if (*(u8 *)(ptr + i + 0x4EC) == a1) {
            return 1;
        }
        i++;
    }
    return 0;
}


/**
 * @brief Search D_8005289C array for the value stored at D_80082C08.
 * @return 1 if found, 0 if not found or array is empty (0xFFFF terminated).
 */
INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021300);


/**
 * @brief Main state machine loop driven by g_vsyncRate.
 * Processes rendering/audio states: 4=render, 3=init+render, 5=transition, 8=alt render.
 * Loops until an unhandled state value is encountered, which sets g_vsyncRate=4 and returns.
 */
INCLUDE_ASM("asm/nonmatchings/10DD0", func_80021358);


/**
 * @brief Add a value to a character's stat at offset 0x02, clamping the result to 9999.
 * @param a0 Character index used to resolve a slot ID via g_gameState.
 * @param a1 Amount to add to the stat.
 * @note The stat at ptr+2 (likely HP or experience) is read as u16, added to a1, then clamped by func_800231C8.
 */
void func_800214E0(s32 a0, s32 a1) {
    extern u8 g_gameState[];
    u8 *base = g_gameState + a0;
    u8 idx = base[0xAF4];
    u8 *ptr = g_gameState + 0x490 + idx * 152;
    *(s16 *)(ptr + 2) = func_800231C8(*(u16 *)(ptr + 2) + a1);
}


