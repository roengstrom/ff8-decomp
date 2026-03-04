#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/**
 * @brief Look up an ability cap byte from g_gfData based on ability ID range.
 * @param id Ability ID (0x00–0x63+).
 * @return The cap value (u8) from the appropriate table section.
 */
s32 GetAbilityCap(s32 id) {
    extern u8 g_gfData[];
    u32 idx;

    if (id < 0x14) {
        s32 b = g_gfData;
        return *(u8 *)(b + id * 8 + 0x40E4);
    }
    idx = id - 0x14;
    if (idx < 0x13U) {
        s32 b = g_gfData;
        return *(u8 *)(b + idx * 8 + 0x4184);
    }
    idx = id - 0x27;
    if (idx < 0x13U) {
        s32 b = g_gfData;
        return *(u8 *)(b + idx * 8 + 0x421C);
    }
    idx = id - 0x3A;
    if (idx < 0x14U) {
        s32 b = g_gfData;
        return *(u8 *)(b + idx * 8 + 0x42B4);
    }
    idx = id - 0x4E;
    if (idx < 0x5U) {
        s32 b = g_gfData;
        return *(u8 *)(b + idx * 8 + 0x4354);
    }
    idx = id - 0x53;
    if (idx < 0x9U) {
        s32 b = g_gfData;
        return *(u8 *)(b + idx * 8 + 0x437C);
    }
    {
        s32 b = g_gfData;
        s32 d = id - 0x5C;
        return *(u8 *)(b + d * 8 + 0x43C4);
    }
}


/**
 * @brief Search an entity's ability slots for a given ability ID.
 * @param entityIdx Entity index (stride 132 into g_gfData).
 * @param abilityId The ability ID to search for.
 * @return Slot index (0–20) if found, 0xFF if not found.
 */
s32 FindAbilitySlot(s32 entityIdx, s32 abilityId) {
    extern u8 g_gfData[];
    s32 i = 0;
    s32 base = (s32)g_gfData;

    REGALLOC_BARRIER(entityIdx);
    entityIdx *= 132;

    do {
        if (*(u8 *)(entityIdx + base + 0xF96) == abilityId) {
            return i;
        }
        i++;
        entityIdx += 4;
    } while (i < 0x15);
    return 0xFF;
}


/**
 * @brief Add amount to an entity's ability value, clamping at 255.
 * @param entityIdx Index into the entity table at D_800773C8 (stride 68).
 * @param amount Value to add (truncated to u16).
 * @return The ability ID (entity[0x40]) if the new value >= cap, 0 otherwise.
 */
s32 AddAbilityExp(s32 entityIdx, s32 amount) {
    extern u8 D_800773C8[];
    u8 *entity = D_800773C8 + entityIdx * 68;
    s32 slot;
    s32 newVal;
    s32 cap;

    if (entity[0x40] == 0) {
        return 0;
    }

    slot = FindAbilitySlot(entityIdx, entity[0x40]);
    if (slot == 0xFF) {
        return 0;
    }
    {
        u8 *slotEntry = entity + slot;
        newVal = slotEntry[0x24] + (u16)amount;
        cap = GetAbilityCap(entity[0x40]);

        if (newVal >= 0x100) {
            slotEntry[0x24] = 0xFF;
        } else {
            slotEntry[0x24] = newVal;
        }

        if (newVal < cap) {
            return 0;
        }
    }
    return entity[0x40];
}


