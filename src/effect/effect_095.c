/**
 * @file effect_095.c
 * @brief Siren Summon (Silent Voice)
 */
#include "common.h"
#include "battle.h"
#include "effect.h"

typedef struct {
    /* 0x000 */ EffectEntity base;
    /* 0x100 */ u8 pad100[0x288 - 0x100];
    /* 0x288 */ SVECTOR unk288;
    /* 0x290 */ u8 pad290[0x298 - 0x290];
    /* 0x298 */ s16 unk298;  /* Frame at which func_801A12E8 fires. */
} SirenEntity;

typedef struct {
    /* 0x00 */ u8 pad000[0x1C];
    /* 0x1C */ SVECTOR pos;
    /* 0x24 */ s16 unk024;
    /* 0x26 */ u16 flags;
    /* 0x28 */ u8 wait;
    /* 0x29 */ u8 pc;
    /* 0x2A */ u8 pad02A[0x30 - 0x2A];
    /* 0x30 */ VECTOR unk030;
    /* 0x40 */ u8 pad040[0x48 - 0x40];
    /* 0x48 */ s16 yaw;
    /* 0x4A */ u8 pad04A[0x54 - 0x4A];
    /* 0x54 */ s16 driftX;
    /* 0x56 */ u8 pad056[0x5E - 0x56];
    /* 0x5E */ s16 yawStep;
} SirenSpinEntity; /* Reuses EffectEntity's 0x30..0x5F range with its own layout. */

typedef void (*SirenSpinHandler)(SirenSpinEntity *entity);

s32 func_80190350(s16 stage);
s32 func_801961D8(EffectEntity *entity);
s32 func_8019D860(EffectEntity *entity);
void func_8019C078(SirenEntity *entity);
void func_8019C320(SirenEntity *entity);
void func_80196220(SirenSpinEntity *entity);
void func_80196340(SirenSpinEntity *entity);
void func_80191234(void *entity);
void func_801902CC(EffectEntity *entity);
s32 func_801A1978(EffectEntity *entity, u8 *prims, s32 cursor);

/* Opcode handlers dispatched through the step functions' local tables. */
void func_8019DB20(EffectEntity *entity);
void func_8019DBB4(EffectEntity *entity);
void func_8019DCD4(EffectEntity *entity);
void func_8019DD68(EffectEntity *entity);
void func_8019DE78(EffectEntity *entity);
void func_8019DEE0(EffectEntity *entity);
void func_8019DF4C(EffectEntity *entity);
void func_8019DFDC(EffectEntity *entity);
void func_8019E968(EffectEntity *entity);
void func_8019E9C4(EffectEntity *entity);
void func_8019E9F0(EffectEntity *entity);
void func_8019EA30(EffectEntity *entity);
void func_801A0880(EffectEntity *entity);
void func_801A08C8(EffectEntity *entity);
void func_801A0F0C(EffectEntity *entity);
void func_801A0F64(EffectEntity *entity);
void func_801A63CC(EffectEntity *entity);
void func_801A649C(EffectEntity *entity);
void func_801A6504(EffectEntity *entity);
void func_801A664C(EffectEntity *entity);
void func_801A66E0(EffectEntity *entity);
void func_801A6868(EffectEntity *entity);
void func_801A68D0(EffectEntity *entity);
void func_801A6914(EffectEntity *entity);
void func_801A6970(EffectEntity *entity);
void func_801A69CC(EffectEntity *entity);
void func_801A6A28(EffectEntity *entity);
void func_801A6A78(EffectEntity *entity);
void func_801A6AD4(EffectEntity *entity);
void func_801A6B18(EffectEntity *entity);
void func_801A6B74(EffectEntity *entity);
void func_801A6BC8(EffectEntity *entity);
void func_801A6C10(EffectEntity *entity);
void func_801A6C70(EffectEntity *entity);
void func_801A6CB8(EffectEntity *entity);
void func_801A72F8(EffectEntity *entity);
void func_801A7370(EffectEntity *entity);
void func_801A73C0(EffectEntity *entity);
void func_801A743C(EffectEntity *entity);
void func_801A74E4(EffectEntity *entity);
void func_801A7570(EffectEntity *entity);
void func_801A75C0(EffectEntity *entity);
void func_801A7714(EffectEntity *entity);
void func_801A7758(EffectEntity *entity);
void func_801A77D8(EffectEntity *entity);
void func_801A781C(EffectEntity *entity);
void func_801A7870(EffectEntity *entity);
void func_801A78EC(EffectEntity *entity);
void func_801A7924(EffectEntity *entity);
void func_801A7968(EffectEntity *entity);
void func_801A79AC(EffectEntity *entity);
void func_801A79F0(EffectEntity *entity);
void func_801A7A70(EffectEntity *entity);
void func_801A7AEC(EffectEntity *entity);
void func_801A7B40(EffectEntity *entity);
void func_801A7B8C(EffectEntity *entity);
void func_801A7BC4(EffectEntity *entity);
void func_801A7C08(EffectEntity *entity);
void func_801A7C54(EffectEntity *entity);
void func_801A7CA8(EffectEntity *entity);
void func_801A7CEC(EffectEntity *entity);
void func_801A7D68(EffectEntity *entity);
void func_801A7DA0(EffectEntity *entity);
void func_801A7DE4(EffectEntity *entity);
void func_801A7E28(EffectEntity *entity);
void func_801A7E68(EffectEntity *entity);
void func_801A7E9C(EffectEntity *entity);
void func_801A7EF4(EffectEntity *entity);
void func_801A7F38(EffectEntity *entity);
void func_801A4A14(SirenSpinEntity *entity);
void func_801A4C7C(SirenSpinEntity *entity);
void func_801A4CDC(SirenSpinEntity *entity);
void func_801A4D10(SirenSpinEntity *entity);
void func_801A4FD0(SirenSpinEntity *entity);
void func_801A5044(SirenSpinEntity *entity);
void func_801A50A4(SirenSpinEntity *entity);
void func_801A50D8(SirenSpinEntity *entity);

extern s32 D_801C3720;
extern u8 D_801C68F0;

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190000);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190288);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801902CC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190350);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190370);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190390);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801903C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190404);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190464);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801904C0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190528);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019058C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801905EC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190648);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801906B8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190724);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190794);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190800);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190950);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801909FC);

void func_80190A1C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190A24);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190B20);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190C1C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190C54);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190DE4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80190F70);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801910FC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019112C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191234);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019125C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801913E8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019151C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019154C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019157C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801915AC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801916E8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191718);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191748);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191778);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801919C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191A14);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191AB8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191AE8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191B40);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191B64);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191BAC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191BF4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191CAC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191D00);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191D6C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191DA4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191ED4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80191FD4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80192100);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801923E8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80192728);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80192CA8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801932C8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801935EC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80193990);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80193F50);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801945D8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80194804);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801948E4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80194C34);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80195098);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801956FC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80195964);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80195BB0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80195E08);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196090);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801961D8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196220);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196340);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196480);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196614);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196698);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019673C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801967B4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196880);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196914);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019698C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196A1C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196A94);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196AC4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196B20);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196BEC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196D80);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80196E04);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019700C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801970C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801978CC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801979C0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198300);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019841C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198454);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198500);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801985EC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198604);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198C30);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198E98);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80198F58);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80199078);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80199198);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80199200);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80199C9C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80199D9C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_80199E3C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019A0C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019A2CC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019A4DC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019AF4C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019AF88);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019AFC8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019AFE0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B068);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B29C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B350);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B48C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B568);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B780);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B88C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019B9A4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019BD10);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019BFB4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C024);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C078);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C320);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C448);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C4DC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C500);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C57C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C5FC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C678);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C708);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019C80C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019CD6C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019CEEC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019CF54);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019CFBC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019D1C8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019D7E8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019D860);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019D990);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DABC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DB20);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DBB4);

void func_8019DC0C(EffectEntity *entity) {
}

static s32 func_8019DC14(EffectEntity *entity) {
    EffectHandler handlers[3] = {
        func_8019DB20, func_8019DBB4, func_8019DC0C
    };

    handlers[entity->pc](entity);
    entity->unk08C = (entity->unk08C + 4) & 0x7F;
    entity->unk090 = (entity->unk090 - 6) & 0x7F;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DCD4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DD68);

void func_8019DDC0(EffectEntity *entity) {
}

static s32 func_8019DDC8(EffectEntity *entity) {
    EffectHandler handlers[3] = {
        func_8019DCD4, func_8019DD68, func_8019DDC0
    };

    handlers[entity->pc](entity);
    entity->unk08C = (entity->unk08C - 4) & 0x7F;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DE78);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DEE0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DF4C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019DFDC);

void func_8019E044(EffectEntity *entity) {
}

static s32 func_8019E04C(EffectEntity *entity) {
    EffectHandler handlers[5] = {
        func_8019DE78, func_8019DEE0, func_8019DF4C, func_8019DFDC,
        func_8019E044
    };

    handlers[entity->pc](entity);
    entity->unk08C = (entity->unk08C - 1) & 0x7F;
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E10C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E178);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E1A4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E200);

void func_8019E248(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E250);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E300);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E36C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E398);

void func_8019E40C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E414);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E4C0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E52C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E558);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E5D8);

void func_8019E64C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E654);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E704);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E770);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E79C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E83C);

void func_8019E8B0(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E8B8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E968);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E9C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019E9F0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EA30);

static void func_8019EA88(EffectEntity *entity) {
    if (!func_8019D860(entity) || func_80190350(6)) {
        entity->flags |= EFFECT_FLAG_STOP;
        entity->pc++;
    }
}

void func_8019EAE0(EffectEntity *entity) {
}

static s32 func_8019EAE8(EffectEntity *entity) {
    EffectHandler handlers[6] = {
        func_8019E968, func_8019E9C4, func_8019E9F0, func_8019EA30,
        func_8019EA88, func_8019EAE0
    };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EBA0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EC3C);

void func_8019EC84(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EC8C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019ED2C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019ED98);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EDE0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EE14);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EE3C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EE84);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EEB8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EEC8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EED8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EEE8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EEF8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EF08);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EF18);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EF28);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EF38);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019EF8C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F0A8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F0FC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F334);

void func_8019F3D8(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F3E0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F480);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F670);

void func_8019F714(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F71C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F7BC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F86C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019F9FC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_8019FC2C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0148);

void func_801A0170(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0178);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0534);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A05D0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A06C8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0758);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A07F8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0880);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A08C8);

void func_801A0B84(EffectEntity *entity) {
}

static s32 func_801A0B8C(EffectEntity *entity) {
    EffectHandler handlers[3] = {
        func_801A0880, func_801A08C8, func_801A0B84
    };

    handlers[entity->pc](entity);
    if (D_801C3720 == 0) {
        entity->unk038.vz++;
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0C50);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0F0C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A0F64);

void func_801A0F84(EffectEntity *entity) {
}

static s32 func_801A0F8C(EffectEntity *entity) {
    EffectHandler handlers[3] = {
        func_801A0F0C, func_801A0F64, func_801A0F84
    };

    handlers[entity->pc](entity);
    if (D_801C3720 == 0) {
        entity->unk038.vz++;
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1050);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A10AC);

void func_801A10F4(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A10FC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A119C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A11F8);

void func_801A1240(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1248);

static void func_801A12E8(SirenEntity *entity) {
    if (entity->base.unk024 >= entity->unk298) {
        entity->unk288.vx = 0;
        entity->unk288.vy = 0;
        entity->unk288.vz = 3264;
        func_8019C078(entity);
        func_8019C320(entity);
        entity->base.pc++;
    }
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A134C);

void func_801A13B8(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A13C0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1460);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A15D4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1658);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A16F0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1788);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1848);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A187C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1918);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1978);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1C1C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1C34);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1D44);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1E24);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1E7C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1ED4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1EF4);

void func_801A1F3C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A1F44);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2114);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2140);

void func_801A2188(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2190);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A232C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A23E8);

void func_801A2430(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2438);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A25D0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A25E4);

void func_801A2764(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A276C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A280C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A282C);

void func_801A2874(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A287C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A29E0);

static void func_801A2A5C(EffectEntity *entity) {
    if (func_801961D8(entity) || func_80190350(2)) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
        entity->pc++;
    }
}

void func_801A2AB4(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2ABC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2D64);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2DB0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2DC4);

void func_801A2EF0(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2EF8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A2FA4);

static void func_801A3020(EffectEntity *entity) {
    if (func_801961D8(entity) || func_80190350(2)) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
        entity->pc++;
    }
}

void func_801A3078(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3080);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3324);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3370);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3384);

void func_801A34BC(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A34C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3570);

static void func_801A35EC(EffectEntity *entity) {
    if (func_801961D8(entity) || func_80190350(3)) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
        entity->pc++;
    }
}

void func_801A3644(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A364C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A38B8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3904);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3918);

void func_801A3A0C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3A14);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3AC0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3B00);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3B14);

void func_801A3C38(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3C40);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3CEC);

static void func_801A3D5C(EffectEntity *entity) {
    if (func_80190350(3) || func_801961D8(entity)) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
        entity->pc++;
    }
}

void func_801A3DB8(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3DC0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3F58);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A3FD4);

static void func_801A406C(EffectEntity *entity) {
    if (func_80190350(3) || func_801961D8(entity)) {
        entity->flags |= EFFECT_FLAG_STOP | EFFECT_FLAG_DONE;
        entity->pc++;
    }
}

void func_801A40C8(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A40D0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4268);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A43A4);

void func_801A4650(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4658);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A46F8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4738);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A474C);

void func_801A4960(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4968);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4A14);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4C7C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4CDC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4D10);

void func_801A4D58(SirenSpinEntity *entity) {
}

static s32 func_801A4D60(SirenSpinEntity *entity) {
    SirenSpinHandler handlers[5] = {
        func_801A4A14, func_801A4C7C, func_801A4CDC, func_801A4D10,
        func_801A4D58
    };

    handlers[entity->pc](entity);
    entity->yaw = (entity->yaw + entity->yawStep) & 0xFFF;
    entity->pos.vx += entity->driftX;
    func_80196340(entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4E3C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4EAC);

void func_801A4F28(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4F30);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A4FD0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A5044);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A50A4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A50D8);

void func_801A5120(SirenSpinEntity *entity) {
}

static s32 func_801A5128(SirenSpinEntity *entity) {
    SirenSpinHandler handlers[5] = {
        func_801A4FD0, func_801A5044, func_801A50A4, func_801A50D8,
        func_801A5120
    };

    handlers[entity->pc](entity);
    entity->yaw = (entity->yaw + entity->yawStep) & 0xFFF;
    entity->unk030.vx = entity->unk030.vy = entity->unk030.vz += 256;
    func_80196220(entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A5208);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A5264);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A5290);

void func_801A5408(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A5410);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A54BC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A580C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A5908);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A63CC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A649C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6504);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A664C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A66E0);

void func_801A67A8(EffectEntity *entity) {
}

static s32 func_801A67B0(EffectEntity *entity) {
    EffectHandler handlers[6] = {
        func_801A63CC, func_801A649C, func_801A6504, func_801A664C,
        func_801A66E0, func_801A67A8
    };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6868);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A68D0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6914);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6970);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A69CC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6A28);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6A78);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6AD4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6B18);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6B74);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6BC8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6C10);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6C70);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6CB8);

void func_801A6CF8(EffectEntity *entity) {
}

static s32 func_801A6D00(EffectEntity *entity) {
    EffectHandler handlers[15] = {
        func_801A6868, func_801A68D0, func_801A6914, func_801A6970,
        func_801A69CC, func_801A6A28, func_801A6A78, func_801A6AD4,
        func_801A6B18, func_801A6B74, func_801A6BC8, func_801A6C10,
        func_801A6C70, func_801A6CB8, func_801A6CF8
    };

    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

static void func_801A6DD4(EffectEntity *entity) {
    BattleTint *tint = D_800EF738;
    s32 i;

    entity->pos.vx = 1536;
    for (i = 0; i < 4; i++) {
        tint->level = entity->pos.vx;
        tint->b = 0;
        tint->g = 0;
        tint->r = 0;
        tint++;
    }
    entity->pc++;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6E24);

void func_801A6E9C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6EA4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6F44);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A6FC0);

void func_801A701C(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7024);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A70C4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A71DC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A72F8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7370);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A73C0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A743C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A74E4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7570);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A75C0);

void func_801A7610(EffectEntity *entity) {
}

static s32 func_801A7618(EffectEntity *entity) {
    EffectHandler handlers[8] = {
        func_801A72F8, func_801A7370, func_801A73C0, func_801A743C,
        func_801A74E4, func_801A7570, func_801A75C0, func_801A7610
    };

    handlers[entity->pc](entity);
    if (!(entity->flags & EFFECT_FLAG_DONE)) {
        s32 cursor = D_800FA5F0;

        D_800FA5F0 = func_801A1978(entity, &D_801C68F0, cursor);
    }
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7714);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7758);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A77D8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A781C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7870);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A78EC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7924);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7968);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A79AC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A79F0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7A70);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7AEC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7B40);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7B8C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7BC4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7C08);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7C54);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7CA8);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7CEC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7D68);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7DA0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7DE4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7E28);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7E68);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7E9C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7EF4);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A7F38);

void func_801A7F84(EffectEntity *entity) {
}

static s32 func_801A7F8C(EffectEntity *entity) {
    EffectHandler handlers[28] = {
        func_801A7714, func_801A7758, func_801A77D8, func_801A781C,
        func_801A7870, func_801A78EC, func_801A7924, func_801A7968,
        func_801A79AC, func_801A79F0, func_801A7A70, func_801A7AEC,
        func_801A7B40, func_801A7B8C, func_801A7BC4, func_801A7C08,
        func_801A7C54, func_801A7CA8, func_801A7CEC, func_801A7D68,
        func_801A7DA0, func_801A7DE4, func_801A7E28, func_801A7E68,
        func_801A7E9C, func_801A7EF4, func_801A7F38, func_801A7F84
    };

    func_801902CC(entity);
    handlers[entity->pc](entity);
    entity->unk024++;
    if (entity->flags & EFFECT_FLAG_STOP) {
        if (entity->wait != 0) {
            return 0;
        }
        func_80191234(entity);
        return 2;
    }
    return 0;
}

void func_801A8050(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8058);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8658);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A86D0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A87AC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A87EC);

void func_801A8814(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A881C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A88DC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A88F0);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8904);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8974);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A89CC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8A2C);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8A40);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8A68);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8AAC);

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8AD4);

void func_801A8AF0(void) {
}

INCLUDE_ASM("asm/ovl/effect_095/nonmatchings/effect_095", func_801A8AF8);
