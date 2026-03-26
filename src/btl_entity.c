#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BAA0);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BC10);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BC6C);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BE48);


/**
 * @brief Compute a pointer into a table indexed by a battle entity's field 0x37.
 * @param a0 Index into the battle entity array (D_80083210, stride 64).
 * @return Address within the table at D_800834C0, offset by 8 + (field_0x37 << 2).
 */
s32 getEntityTablePtr(s32 a0) {
    extern s32 D_800834C0;
    s32 base = D_800834C0 + 8;
    s32 result = getBattleEntityAnimSpeed(a0);
    return base + (result << 2);
}


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BF24);


/**
 * @brief Dispatch a battle entity's function pointer.
 *
 * Looks up the entity at index a0 in g_battleEntities (stride 64). If the
 * first word (function pointer) is non-NULL, calls it with the entity address.
 *
 * @param a0 Entity index.
 */
void dispatchBattleEntity(s32 a0) {
    extern u8 g_battleEntities[];
    s32 entry = (s32)g_battleEntities + a0 * 64;
    void (*fp)(s32) = (void (*)(s32))*(s32 *)entry;
    if (fp != 0) {
        fp(entry);
    }
}


/** @brief Finds the first available battle entity slot (0-7) and marks it as active.
 *  @return Slot index (0-7), or -1 if none available.
 */
s32 allocBattleEntitySlot(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        if (GetActiveFlag(i) == 0) {
            setBattleEntityActive(i, 1);
            return i;
        }
    }
    return -1;
}


/**
 * @brief Initialize all 8 battle entity slots by calling initBattleEntity on each.
 *
 * Iterates slots 0 through 7, resetting each entity's display rects, flags,
 * position fields, and volume to default values.
 */
void initAllBattleEntities(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        initBattleEntity(i);
    }
}


extern s32 D_800834CC;

/**
 * @brief Store a base address into D_800834CC.
 * @param a0 Value to store (typically a memory address such as 0x80090000).
 */
void setBattleEntityBase(s32 a0) {
    D_800834CC = a0;
}


/** @brief Return the maximum number of battle entity slots (always 8). */
s32 getMaxBattleEntities(void) {
    return 8;
}


extern u8 D_80083840;
/**
 * @brief Get the current value of the global flag D_80083840.
 * @return The flag value as an unsigned byte.
 */
u8 getBattleFlag(void) {
    return D_80083840;
}


/**
 * @brief Set the global flag D_80083840.
 * @param val The new flag value to store.
 */
void setBattleFlag(u8 val) {
    D_80083840 = val;
}


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C130);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C3AC);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C56C);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C734);


extern SfxEntry g_sfxEntries[];

/**
 * @brief Set field30 (u16) and field32 (u8) on an SFX entry.
 * @param a0 Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param a1 Value for field30.
 * @param a2 Value for field32.
 */
void setSfxEntryParams(s32 idx, s32 a1, s32 a2) {
    SfxEntry* entry = &g_sfxEntries[idx];
    entry->field30 = a1;
    entry->field32 = a2;
}


/**
 * @brief Set three byte fields on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param a1 Value for field29.
 * @param a2 Value for field2A.
 * @param a3 Value for field2C.
 */
void setSfxEntryTimings(s32 idx, s32 a1, s32 a2, s32 a3) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field29 = a1;
    entry->field2A = a2;
    entry->field2C = a3;
}


/**
 * @brief Set field2B on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Value to store.
 */
void setSfxEntryField2B(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field2B = val;
}


/**
 * @brief Set field34 on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val 32-bit value to store.
 */
void setSfxEntryField34(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field34 = val;
}


/**
 * @brief Set field38 on an SFX entry.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val 32-bit value to store.
 */
void setSfxEntryField38(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->field38 = val;
}


/**
 * @brief Set volume on an SFX entry and propagate to its linked battle entity's scale.
 * @param idx Index into the SFX entry array (g_sfxEntries, stride 60).
 * @param val Volume value (0x1000 = default).
 */
void setSfxEntryVolume(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries[idx];
    entry->volume = val;
    setBattleEntityScale(entry->entityIdx, val);
}


