/**
 * @file effect_022.c
 * @brief Bio
 */
#include "common.h"
#include "battle.h"
#include "effect.h"
#include "battle/bc_object8.h"
#include "battle/bc_object11.h"
#include "battle/bc_object12.h"
#include "battle/bc_object13.h"

typedef struct {
    /* 0x00 */ u8 pad000[0xCC];
    /* 0xCC */ s16 unk0CC;
    /* 0xCE */ s16 unk0CE;
    /* 0xD0 */ u8 pad0D0[0xF2 - 0xD0];
    /* 0xF2 */ s16 unk0F2;
} BioModelState;

typedef struct {
    /* 0x00 */ u8 pad000[0x30];
    /* 0x30 */ VECTOR scale;
    /* 0x40 */ u8 pad040[0x70 - 0x40];
} BioSpark;

extern u8 D_801E3D88[];
extern u8 D_801E3DA8[];
extern s32 D_801EC688;

extern u8 D_801BA9F0;
extern u8 D_801BB308;
extern u8 D_801C7308;
extern u8 *D_801E3DDC;
extern u8 *D_801E3DE0;
extern u8 *D_801E3DE4;
extern s32 D_801E3DE8;
extern s32 D_801E3EB8;
extern s32 D_801E3EC8;
extern s32 D_801E40F8;
extern s32 D_801E4108;
extern s32 D_801E4338;
extern s32 D_801E4348;
extern s32 D_801EC698;
extern s32 D_801ED288;
extern s32 D_801ED308;

extern s32 D_801ED278;
extern s32 D_801ED318;
extern s32 D_801ED348;
extern s32 D_801F3108;
extern BioModelState D_801F3118;

void *func_801A1F64(void *pool, void *task, s32 stride, EffectEntity *owner);
void func_801A1508(BioModelState *state, void *arg1, void *arg2, void *arg3, BattleEffectSlot *slot, s32 slotIndex);
s32 func_801A5644(EffectEntity *entity);
s32 func_801A6AC0(EffectEntity *entity);
static s32 func_801A8CF4(EffectEntity *entity);
s32 func_801A722C(EffectEntity *entity);
s32 func_801A7580(EffectEntity *entity);
void func_801A1E00(BioModelState *state);
void func_801A1F3C(EffectEntity *entity);
void func_801A22B4(EffectEntity *entity);

/* Opcode handlers dispatched through the step functions' local tables. */
void func_801A76FC(EffectEntity *entity);
void func_801A776C(EffectEntity *entity);
void func_801A813C(EffectEntity *entity);
void func_801A8184(EffectEntity *entity);
void func_801A81E0(EffectEntity *entity);
void func_801A8210(EffectEntity *entity);
void func_801A824C(EffectEntity *entity);
void func_801A8C00(EffectEntity *entity);
void func_801A8C14(EffectEntity *entity);
void func_801A8C6C(EffectEntity *entity);
void func_801A8CBC(EffectEntity *entity);

void *func_801A0000(EffectAnimSet *animSet) {
    EffectEntity *entity;
    u8 slot;

    slot = animSet->anims->parts->unk000;
    func_800B2A00(&D_801E3EB8, &D_801E3DE8, 100, 2);
    entity = func_801A1F64(&D_801E3EB8, func_801A8CF4, 100, NULL);
    entity->animSet = animSet;
    entity->unk02D = slot;
    entity->unk05A = animSet->anims->unk010;
    entity->unk058 = animSet->anims->unk011;
    if (!(animSet->flags & EFFECT_ANIMSET_FLAG_LOADED)) {
        func_800C3BE0(&D_801BA9F0);
        func_800BB084(&D_801BB308);
    }
    D_801E3DE0 = &D_801BB308;
    D_801E3DE4 = &D_801C7308;
    func_800B2A00(&D_801E40F8, &D_801E3EC8, 140, 4);
    func_800B2A00(&D_801E4338, &D_801E4108, 56, 10);
    func_800B2A00(&D_801EC688, &D_801E4348, 112, 300);
    func_800B2A00(&D_801ED278, &D_801EC698, 152, 20);
    func_800B2A00(&D_801ED308, &D_801ED288, 60, 2);
    return &D_801E3EB8;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0184);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A02D4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0380);

void func_801A03A0(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A03A8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A04A4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A05A0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A05D8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0768);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A08F4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0A80);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0B8C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0BF8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0D00);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0DD0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A0FEC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A1130);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A11DC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A121C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A1508);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A15C4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A168C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A1E00);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A1F3C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A1F64);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A20F0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2224);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2254);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2284);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A22B4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A23F0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2420);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2450);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2480);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A26CC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A271C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A27C0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A27F0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2848);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A286C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A28B4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A28FC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A29B4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2A08);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2A74);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2AAC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A2D8C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A30C4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A3458);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A3850);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A3B78);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A3F18);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A42EC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A474C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A49A0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A4C7C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A4F6C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5528);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5644);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A568C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5724);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5820);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5A40);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5B2C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5C14);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5DB8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5DF4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5E30);

void func_801A5E4C(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A5E54);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A60B8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6234);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A66D4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A67E8);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6934);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6A7C);

void func_801A6AB8(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6AC0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6BC0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6C48);

void func_801A6CFC(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6D04);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6DD0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6E50);

void func_801A6F18(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6F20);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A6FCC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A700C);

void func_801A7098(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A70A0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7148);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A71DC);

void func_801A7224(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A722C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A72F8);

static void func_801A74CC(EffectEntity *entity) {
    entity->unk058 += entity->scratch.accel.vx;
    entity->unk05A += entity->scratch.accel.vy;
    entity->unk05C += entity->scratch.accel.vz;
    entity->pos.vx += entity->unk058;
    entity->pos.vy += entity->unk05A;
    entity->pos.vz += entity->unk05C;
    if (func_801A5644(entity)) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

void func_801A7578(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7580);

static void func_801A764C(EffectEntity *entity) {
    BattleEffectSlot *slot = &D_800EF2D0[entity->unk02D];

    func_801A1508(&D_801F3118, &D_801F3108, &D_801ED318, &D_801ED348, slot, entity->unk02D);
    entity->flags |= EFFECT_FLAG_UNK08;
    slot->flags |= BATTLE_SLOT_FLAG_UNK04;
    entity->pc++;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A76FC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A776C);

static void func_801A77C8(EffectEntity *entity) {
    BattleEffectSlot *slot;
    EffectModel *model;

    D_801F3118.unk0F2 -= 64;
    if (D_801F3118.unk0F2 < 0) {
        D_801F3118.unk0F2 = 0;
    }
    D_801F3118.unk0CE -= 256;
    if (D_801F3118.unk0CE <= 0) {
        entity->flags &= ~EFFECT_FLAG_UNK08;
        slot = &D_800EF2D0[entity->unk02D];
        model = entity->unk010;
        slot->flags &= ~BATTLE_SLOT_FLAG_UNK04;
        model->unk063 = 0;
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

void func_801A7870(EffectEntity *entity) {
}

static s32 func_801A7878(EffectEntity *entity) {
    EffectHandler handlers[5] = {
        func_801A764C, func_801A76FC, func_801A776C, func_801A77C8,
        func_801A7870
    };
    BioModelState *model = &D_801F3118;

    handlers[entity->pc](entity);
    model->unk0CC = (model->unk0CC + 160) & 0xFFF;
    if (entity->flags & EFFECT_FLAG_UNK08) {
        func_801A1E00(model);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_801A1F3C(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7964);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A79A0);

void func_801A7CC4(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7CCC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7D6C);

static void func_801A7DA8(EffectEntity *entity) {
    BioSpark *spark;
    EffectModel *model;
    s32 count;
    s32 i;

    if (entity->unk024 < 25) {
        count = D_801E3D88[entity->unk024];
        model = entity->unk014;
        for (i = 0; i < count; i++) {
            spark = func_801A1F64(&D_801EC688, func_801A722C, 112, entity);
            if (rand() & 0x1) {
                spark->scale.vx = spark->scale.vy = spark->scale.vz = model->unk086;
            } else {
                spark->scale.vx = spark->scale.vy = spark->scale.vz = model->unk086 / 2;
            }
        }
    } else {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

void func_801A7EAC(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7EB4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A7F54);

static void func_801A7F90(EffectEntity *entity) {
    BioSpark *spark;
    EffectModel *model;
    s32 count;
    s32 i;

    if (entity->unk024 < 21) {
        count = D_801E3DA8[entity->unk024];
        model = entity->unk014;
        for (i = 0; i < count; i++) {
            spark = func_801A1F64(&D_801EC688, func_801A7580, 112, entity);
            if (rand() & 0x1) {
                spark->scale.vx = spark->scale.vy = spark->scale.vz = model->unk086;
            } else {
                spark->scale.vx = spark->scale.vy = spark->scale.vz = model->unk086 / 2;
            }
        }
    } else {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

void func_801A8094(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A809C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A813C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8184);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A81E0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8210);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A824C);

void func_801A82A8(EffectEntity *entity) {
}

static s32 func_801A82B0(EffectEntity *entity) {
    EffectHandler handlers[6] = {
        func_801A813C, func_801A8184, func_801A81E0, func_801A8210,
        func_801A824C, func_801A82A8
    };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_801A1F3C(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8368);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8384);

void func_801A84AC(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A84B4);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8554);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A86BC);

static void func_801A8750(EffectEntity *entity) {
    EffectEntity *child;

    if (entity->unk024 >= 4) {
        child = func_801A1F64(&D_801ED278, func_801A6AC0, 152, entity);
        child->unk08C = 2;
        entity->pc++;
    }
}

static void func_801A87B8(EffectEntity *entity) {
    EffectEntity *child;

    if (entity->unk024 >= 6) {
        child = func_801A1F64(&D_801ED278, func_801A6AC0, 152, entity);
        child->unk08C = 3;
        entity->pc++;
    }
}

static void func_801A8820(EffectEntity *entity) {
    EffectEntity *child;

    if (entity->unk024 >= 8) {
        child = func_801A1F64(&D_801ED278, func_801A6AC0, 152, entity);
        child->unk08C = 4;
        entity->pc++;
    }
}

static void func_801A8888(EffectEntity *entity) {
    EffectEntity *child;

    if (entity->unk024 >= 10) {
        child = func_801A1F64(&D_801ED278, func_801A6AC0, 152, entity);
        child->unk08C = 5;
        entity->pc++;
    }
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A88F0);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A896C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A89F0);

static void func_801A8A50(EffectEntity *entity) {
    if (entity->unk080 == 1) {
        func_800BFE1C(entity->animSet->anims[entity->unk02A].parts);
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

void func_801A8AC4(void) {
}

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8ACC);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8C00);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8C14);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8C6C);

INCLUDE_ASM("asm/ovl/effect_022/nonmatchings/effect_022", func_801A8CBC);

void func_801A8CEC(EffectEntity *entity) {
}

static s32 func_801A8CF4(EffectEntity *entity) {
    EffectHandler handlers[5] = {
        func_801A8C00, func_801A8C14, func_801A8C6C, func_801A8CBC,
        func_801A8CEC
    };

    if (entity->unk05C & 0x1) {
        D_801E3DDC = D_801E3DE0;
    } else {
        D_801E3DDC = D_801E3DE4;
    }
    func_801A22B4(entity);
    handlers[entity->pc](entity);
    entity->unk05E = func_800B2B68(&D_801E40F8);
    entity->unk05E += func_800B2B68(&D_801E4338);
    entity->unk05E += func_800B2B68(&D_801EC688);
    entity->unk05E += func_800B2B68(&D_801ED278);
    entity->unk05E += func_800B2B68(&D_801ED308);
    entity->unk05C++;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_801A1F3C(entity);
        return 2;
    }
    return 0;
}
