#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libc.h"
#include "battle.h"

extern BattleDisplayEntity g_battleEntities[];
extern void copyDisplayRect(RECT *dst);

/** @brief Clipped rectangle result: the clipped rect + saved pre-clip position. */
typedef struct {
    RECT rect;       /* 0x00: clipped rectangle */
    s32 savedPos;    /* 0x08: packed original x|y before clipping */
} ClipResult;

/** @brief Scratch workspace for rectangle clipping operations. */
typedef struct {
    ClipResult work;
    ClipResult disp;
} ClipWork;

/** @brief Parameters for a double-blit operation with source rects and destination buffers. */
typedef struct {
    u8 pad[0x08];
    RECT srcRect1;
    RECT srcRect2;
    u8 dstData1[12];
    u8 dstData2[12];
} BlitParams;

/**
 * @brief Set a battle entity's type and compute draw mode from bit 0.
 * @param idx Entity index.
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
 * @brief Set a battle entity's field00.
 * @param idx Entity index.
 * @param val Value to store.
 */
void setBattleEntityField00(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field00 = val;
}


/**
 * @brief Set a battle entity's field04.
 * @param idx Entity index.
 * @param val Value to store.
 */
void setBattleEntityField04(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field04 = val;
}


/**
 * @brief Set a battle entity's field36.
 * @param idx Entity index.
 * @param val Value to store.
 */
void setBattleEntityField36(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field36 = val;
}


/**
 * @brief Get a battle entity's field36.
 * @param idx Entity index.
 * @return Value of field36.
 */
u32 getBattleEntityField36(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->field36;
}


/**
 * @brief Set a battle entity's field35.
 * @param idx Entity index.
 * @param val Value to store.
 */
void setBattleEntityField35(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->field35 = val;
}


/**
 * @brief Get a battle entity's field35.
 * @param idx Entity index.
 * @return Value of field35.
 */
u32 getBattleEntityField35(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->field35;
}


/**
 * @brief Set a battle entity's active flag; if 0, fully deactivate the entity.
 * @param idx Entity index.
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
 * @param idx Entity index.
 * @return Active flag value (0 = inactive).
 */
s32 GetActiveFlag(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->activeFlag;
}


/**
 * @brief Set a battle entity's scale factor.
 * @param idx Entity index.
 * @param val Scale value (0x1000 = 1.0).
 */
void setBattleEntityScale(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->scale = val;
}


/**
 * @brief Get a battle entity's scale factor.
 * @param idx Entity index.
 * @return Scale value (0x1000 = 1.0).
 */
s32 getBattleEntityScale(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->scale;
}


/**
 * @brief Initialize a battle entity to default values.
 *
 * Sets up a default bounding rect (64,64,128,128), entity type 6,
 * clears fields, sets anim speed to 3, scale to 0x1000.
 *
 * @param idx Entity index.
 */
void initBattleEntity(s32 idx) {
    s16 rect[4];
    s32 i;
    rect[0] = 0x40;
    rect[1] = 0x40;
    rect[2] = 0x80;
    rect[3] = 0x80;
    setBattleEntityBoundRect(idx, rect);
    setBattleEntityRectClamp(idx, rect);
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


/**
 * @brief Clip two source rectangles against the display area and write results.
 *
 * For each of the two source rects in @p arg, offsets by the display origin,
 * clips against the display rect via RECT intersection, clamps minimum size to
 * 2x1, and copies the 12-byte result to the output buffers.
 *
 * @param arg Blit parameters with source rects and destination buffers.
 * @return 1 if the first rectangle intersects the display area, 0 otherwise.
 */
s32 clipBlitRects(BlitParams *arg) {
    ClipWork *cw;
    ClipResult *disp;
    s32 result;

    GP_ALLOC(cw, sizeof(ClipWork));

    disp = &cw->disp;

    copyDisplayRect(&disp->rect);
    disp->savedPos = *(s32 *)&disp->rect;

    cw->work.rect = arg->srcRect1;
    cw->work.rect.x += disp->rect.x;
    cw->work.rect.y += disp->rect.y;
    cw->work.savedPos = *(s32 *)&cw->work.rect;

    if (cw->work.rect.w <= 0 || cw->work.rect.h <= 0) {
        result = 0;
    } else {
        result = func_8002B080(&cw->work.rect, &disp->rect, &cw->work.rect) != 0;
    }
    result++; result--; /* Regalloc */

    if (cw->work.rect.w < 2) cw->work.rect.w = 2;
    if (cw->work.rect.h < 2) cw->work.rect.h = 1;

    memcpy(arg->dstData1, cw, 12);

    copyDisplayRect(&disp->rect);
    disp->savedPos = *(s32 *)&disp->rect;

    cw->work.rect = arg->srcRect2;
    cw->work.rect.x += disp->rect.x;
    cw->work.rect.y += disp->rect.y;
    cw->work.savedPos = *(s32 *)&cw->work.rect;

    if (cw->work.rect.w > 0 && cw->work.rect.h > 0) {
        func_8002B080(&cw->work.rect, &disp->rect, &cw->work.rect);
    }

    if (cw->work.rect.w < 2) cw->work.rect.w = 2;
    if (cw->work.rect.h < 2) cw->work.rect.h = 1;

    memcpy(arg->dstData2, cw, 12);

    GP_FREE(sizeof(ClipWork));

    return result;
}
