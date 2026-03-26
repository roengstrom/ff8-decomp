#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

extern BattleDisplayEntity g_battleEntities[];

/**
 * @brief Set a battle entity's type and compute draw mode from bit 0.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Entity type; if bit 0 is set, drawMode = 0x3A000000, else 0x38000000.
 */
void setBattleEntityType(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    s32 v;
    entity->entityType = val;
    v = 0x38;
    if (val & 1) {
        v = 0x3A;
    }
    entity->drawMode = v << 24;
}


/**
 * @brief Set a battle entity's field00 (first word).
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Value to store.
 */
void setBattleEntityField00(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field00 = val;
}


/**
 * @brief Set a battle entity's field04 (second word).
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Value to store.
 */
void setBattleEntityField04(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field04 = val;
}


/**
 * @brief Set a battle entity's field36.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Value to store.
 */
void setBattleEntityField36(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field36 = val;
}


/**
 * @brief Get a battle entity's field36.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Value of field36.
 */
u32 getBattleEntityField36(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->field36;
}


/**
 * @brief Set a battle entity's field35.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Value to store.
 */
void setBattleEntityField35(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field35 = val;
}


/**
 * @brief Get a battle entity's field35.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Value of field35.
 */
u32 getBattleEntityField35(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->field35;
}


/**
 * @brief Set a battle entity's active flag; if 0, fully deactivate the entity.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param value Active flag; if 0, also clears field36, field04, and field00.
 */
void setBattleEntityActive(s32 idx, s32 value) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];

    entity->activeFlag = value;
    if (value == 0) {
        setBattleEntityField36(idx, 0);
        setBattleEntityField04(idx, 0);
        setBattleEntityField00(idx, 0);
    }
}


/**
 * @brief Get a battle entity's active flag.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Active flag value (0 = inactive).
 */
s32 GetActiveFlag(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->activeFlag;
}


/**
 * @brief Set a battle entity's scale factor.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @param val Scale value (0x1000 = 1.0).
 */
void setBattleEntityScale(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->scale = val;
}


/**
 * @brief Get a battle entity's scale factor.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @return Scale value (0x1000 = 1.0).
 */
s32 getBattleEntityScale(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->scale;
}


/**
 * @brief Initialize a battle entity to default values.
 * @param idx Entity index into g_battleEntities (stride 64 bytes).
 * @note Sets up a default rectangle (64,64,128,128), entity type 6, clears pointers and IDs,
 *       sets field 0x37 to 3, initializes sub-fields, sets field 0x3C to 0x1000, clears field 0x36.
 */
void initBattleEntity(s32 idx) {
    s16 rect[4];
    s32 i;
    rect[0] = 0x40;
    rect[1] = 0x40;
    rect[2] = 0x80;
    rect[3] = 0x80;
    setBattleEntityBoundRect(idx, rect);
    func_8002AD3C(idx, rect);
    setBattleEntityType(idx, 6);
    setBattleEntityField04(idx, 0);
    setBattleEntityField00(idx, 0);
    setBattleEntityActive(idx, 0);
    setBattleEntityAnimSpeed(idx, 3);
    for (i = 0; i < 2; i++) {
        setBattleEntitySubField(idx, i, 0);
    }
    setBattleEntitySubField(idx, 1, idx);
    setBattleEntityScale(idx, 0x1000);
    setBattleEntityField36(idx, 0);
}


INCLUDE_ASM("asm/nonmatchings/btl_display", func_8002B080);


INCLUDE_ASM("asm/nonmatchings/btl_display", func_8002B16C);
