/**
 * @file effect_191.c
 * @brief Doomtrain Summon (Runaway Train)
 */
#include "common.h"
#include "battle.h"
#include "effect.h"
#include "battle/bc_object8.h"
#include "battle/bc_object9.h"
#include "battle/bc_object11.h"
#include "psxsdk/libc.h"

typedef struct {
    /* 0x00 */ s32 flags;      /* Bit 0: live. */
    /* 0x04 */ u16 age;
    /* 0x06 */ u16 scale;
    /* 0x08 */ SVECTOR pos;
    /* 0x10 */ u8 pad10[0x18 - 0x10];
} DoomtrainParticle; /* 0x18 */

typedef struct {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s16 timer;
} DoomtrainTask;

typedef struct {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s16 timer;
    /* 0x0E */ s16 delay;      /* Frames before the puff starts. */
    /* 0x10 */ s16 x;
    /* 0x12 */ s16 y;
    /* 0x14 */ s16 z;
    /* 0x16 */ s16 unk16;
    /* 0x18 */ u8 pad18[0x1C - 0x18];
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
    /* 0x20 */ s16 unk20;
    /* 0x22 */ s16 unk22;
} DoomtrainPuff; /* 0x24 */

typedef struct {
    /* 0x00000 */ u8 prims[0x2E000];
    /* 0x2E000 */ DoomtrainParticle bank0[170];
    /* 0x2EFF0 */ u8 pad2EFF0[0x2F000 - 0x2EFF0];
    /* 0x2F000 */ DoomtrainParticle bank1[170];
    /* 0x2FFF0 */ u8 pad2FFF0[0x30000 - 0x2FFF0];
    /* 0x30000 */ DoomtrainParticle bank2[150];
    /* 0x30E10 */ u8 pad30E10[0x31000 - 0x30E10];
    /* 0x31000 */ DoomtrainParticle bank3[120];
    /* 0x31B40 */ u8 pad31B40[0x32000 - 0x31B40];
    /* 0x32000 */ DoomtrainParticle bank4[130];
    /* 0x32C30 */ u8 pad32C30[0x33000 - 0x32C30];
    /* 0x33000 */ DoomtrainParticle bank5[590];
    /* 0x36750 */ u8 pad36750[0x37000 - 0x36750];
    /* 0x37000 */ u8 unk37000[1];
} DoomtrainPrimArea;

extern u8 D_801B2F80;
extern DoomtrainPrimArea *D_801A8678;
extern DoomtrainParticle *D_801A74A8;
extern DoomtrainParticle *D_801A74AC;
extern DoomtrainParticle *D_801A74B0;
extern DoomtrainParticle *D_801A74B4;
extern DoomtrainParticle *D_801A74B8;
extern DoomtrainParticle *D_801A74BC;
extern u8 *D_801A74C0;
extern EffectAnimSet *D_801A7818;
extern s32 D_801A781C;  /* Battle slot the train is posed from. */
extern s32 D_801A7820;  /* slot of the animation set. */
extern s32 D_801A7828;
extern s32 D_801A7848;
extern s32 D_801A7858;
extern s32 D_801A8668;
extern SVECTOR D_801A8680;
extern MATRIX D_801A8688;
extern s32 D_801A87DC;

s32 func_8019DC00(DoomtrainTask *task);
s32 func_8019E1E8(DoomtrainTask *task);
s32 func_8019B2C0(DoomtrainPuff *puff);

void *func_80190000(EffectAnimSet *animSet) {
    SVECTOR vec;
    DoomtrainTask *task;
    s32 i;

    /* The overlay's TIM data area is reused as the prim and particle area. */
    D_801A8678 = (DoomtrainPrimArea *)&D_801B2F80;
    D_801A74A8 = D_801A8678->bank0;
    D_801A74AC = D_801A8678->bank1;
    D_801A74B0 = D_801A8678->bank2;
    D_801A74B4 = D_801A8678->bank3;
    D_801A74B8 = D_801A8678->bank4;
    D_801A74BC = D_801A8678->bank5;
    D_801A74C0 = D_801A8678->unk37000;
    D_801A7818 = animSet;
    D_801A781C = animSet->anims->parts->unk000;
    D_801A7820 = animSet->slot;

    func_800B2A00(&D_801A7848, &D_801A7828, 16, 2);
    task = func_800B2A84(&D_801A7848, func_8019E1E8);
    task->timer = 0;
    func_800B2A00(&D_801A8668, &D_801A7858, 36, 100);
    task = func_800B2A84(&D_801A8668, func_8019DC00);
    task->timer = 0;

    for (i = 0; i < 170; i++) {
        D_801A74A8[i].flags = 0;
    }
    for (i = 0; i < 170; i++) {
        D_801A74AC[i].flags = 0;
    }
    for (i = 0; i < 150; i++) {
        D_801A74B0[i].flags = 0;
    }
    for (i = 0; i < 120; i++) {
        D_801A74B4[i].flags = 0;
    }
    for (i = 0; i < 130; i++) {
        D_801A74B8[i].flags = 0;
    }
    for (i = 0; i < 590; i++) {
        D_801A74BC[i].flags = 0;
    }

    func_800BB084(&D_801B2F80);

    vec.vx = 0;
    vec.vy = D_800EF2D0[D_801A781C].facing + 2048;
    vec.vz = 0;
    RotMatrix(&vec, &D_801A8688);
    func_800B37E0(D_801A7818->unk002, &D_801A8680);
    D_801A8688.t[0] = 0;
    D_801A8688.t[1] = 0;
    D_801A8688.t[2] = D_801A8680.vz;
    vec.vx = 0;
    vec.vy = 0;
    vec.vz = 0;
    ApplyMatrixSV(&D_800EF2D0[D_801A781C].mtx, &vec, &vec);
    D_801A87DC = 0;
    D_801A8688.t[0] += vec.vx;
    D_801A8688.t[1] += vec.vy;
    D_801A8688.t[2] += vec.vz;
    return &D_801A7848;
}

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80190320);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801906B4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80190AA4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80190FB4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801915C4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80191748);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019177C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801917CC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801918C4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801927A4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80192814);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80192BC8);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80192C3C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80192CB8);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80192D78);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80192DEC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019331C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801933F0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801934E0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193564);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193594);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193608);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801936F0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801937F8);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193824);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193848);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193878);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801938AC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193978);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193A64);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193AA8);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193AEC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193B58);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193BC4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193C7C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193D14);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193DCC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193E90);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80193F24);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80194080);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801941D4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80194324);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80194518);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801946C0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019481C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80194B54);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80194C30);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80194FA0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80195304);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80195808);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80195868);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80195C18);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80195C68);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801962EC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801966C0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019672C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019679C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80196BA4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80196D30);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80196E14);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801970F8);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019714C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801971A0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801971F4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019724C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801972A4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019776C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80197F08);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80197F88);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80198004);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_801986F4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019876C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80198FCC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019903C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80199B44);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80199BC0);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_80199C38);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019A5AC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019A620);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019B254);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019B2C0);

static s32 func_8019B604(DoomtrainTask *task) {
    DoomtrainPuff *puff;
    s32 count;
    s32 i;

    if (D_800EEC5C & (BATTLE_STATE_UNK200 | BATTLE_STATE_UNK001)) {
        return 0;
    }
    /* The frame bound is checked unsigned; a signed compare does not match. */
    if ((u16)task->timer < 12 && task->timer % 3 == 1) {
        count = rand() % 2 + 1;
        for (i = 0; i < count; i++) {
            puff = func_800B2A84(&D_801A8668, func_8019B2C0);
            puff->timer = 0;
            puff->delay = i + rand() % 4;
            puff->x = rand() % 400 + 1000;
            puff->y = rand() % 2000 - 1300;
            puff->z = -6000 - rand() % 3000;
            puff->unk16 = 12;
            puff->unk1C = rand() % 3072 + 1152;
            puff->unk1E = puff->unk1C / 6;
            puff->unk20 = puff->unk22 = (rand() % 6144 + 1664) / 2;
        }
    }
    return ++task->timer > 12 ? 2 : 0;
}

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019B8AC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019BE70);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019C514);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019CB5C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019CD28);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019D0B4);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019D48C);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019D4EC);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019DC00);

INCLUDE_ASM("asm/ovl/effect_191/nonmatchings/effect_191", func_8019E1E8);
