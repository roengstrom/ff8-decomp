#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern BattleDisplayEntity g_battleEntities[];
extern s32 D_800834CC;
extern u8 g_digitBaseCode;
extern SfxSystem g_sfxEntries;
extern DisplayListBuf *D_800834C0;


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BAA0);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BC10);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BC6C);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BE48);


/**
 * @brief Get the OT entry for a battle entity based on its anim speed.
 * @param idx Entity index.
 * @return Pointer to the OT entry at active display list's ot[animSpeed].
 */
s32* getEntityTablePtr(s32 idx) {
    u32 *base = D_800834C0->ot;
    s32 speed = getBattleEntityAnimSpeed(idx);
    return &base[speed];
}


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002BF24);


/**
 * @brief Dispatch a battle entity's update callback.
 *
 * If the entity has a callback set, invokes it with the entity pointer.
 *
 * @param idx Entity index.
 */
void dispatchBattleEntity(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    if (entity->callback) {
        entity->callback(entity);
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


/**
 * @brief Store a base address for battle entity data.
 * @param val Value to store (typically a memory address).
 */
void setBattleEntityBase(s32 val) {
    D_800834CC = val;
}


/** @brief Return the maximum number of battle entity slots (always 8). */
s32 getMaxBattleEntities(void) {
    return 8;
}


/**
 * @brief Get the FF8 font code for digit '0'.
 * @return Base character code for rendering digits in battle.
 */
u8 getDigitBaseCode(void) {
    return g_digitBaseCode;
}


/**
 * @brief Set the FF8 font code for digit '0'.
 * @param val Base character code from the menu string table.
 */
void setDigitBaseCode(u8 val) {
    g_digitBaseCode = val;
}


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C130);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C3AC);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C56C);


INCLUDE_ASM("asm/nonmatchings/btl_entity", func_8002C734);


/**
 * @brief Set field30 and field32 on an SFX entry.
 * @param idx SFX entry index.
 * @param val30 Value for field30.
 * @param val32 Value for field32.
 */
void setSfxEntryParams(s32 idx, s32 val30, s32 val32) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field30 = val30;
    entry->field32 = val32;
}


/**
 * @brief Set timing fields on an SFX entry.
 * @param idx SFX entry index.
 * @param val29 Value for field29.
 * @param val2A Value for field2A.
 * @param val2C Value for field2C.
 */
void setSfxEntryTimings(s32 idx, s32 val29, s32 val2A, s32 val2C) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field29 = val29;
    entry->field2A = val2A;
    entry->field2C = val2C;
}


/**
 * @brief Set field2B on an SFX entry.
 * @param idx SFX entry index.
 * @param val Value to store.
 */
void setSfxEntryField2B(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field2B = val;
}


/**
 * @brief Set field34 on an SFX entry.
 * @param idx SFX entry index.
 * @param val Value to store.
 */
void setSfxEntryField34(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field34 = val;
}


/**
 * @brief Set field38 on an SFX entry.
 * @param idx SFX entry index.
 * @param val Value to store.
 */
void setSfxEntryField38(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->field38 = val;
}


/**
 * @brief Set volume on an SFX entry and propagate to its linked entity's scale.
 * @param idx SFX entry index.
 * @param val Volume value (0x1000 = default).
 */
void setSfxEntryVolume(s32 idx, s32 val) {
    SfxEntry *entry = &g_sfxEntries.entries[idx];
    entry->volume = val;
    setBattleEntityScale(entry->entityIdx, val);
}


