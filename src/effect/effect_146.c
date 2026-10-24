/**
 * @file effect_146.c
 * @brief Tornado
 */
#include "common.h"
#include "battle.h"

typedef struct {
    /* 0x00 */ s32 faceOffset;  /* Byte offset of the face list. */
    /* 0x04 */ s32 unk04;
    /* 0x08 */ u8 verts[1];     /* Vertex data runs from here. */
} TornadoMeshData;

typedef struct {
    /* 0x00 */ u8 pad00[0x30];
    /* 0x30 */ TornadoMeshData *data;
    /* 0x34 */ u8 *verts;
    /* 0x38 */ u8 *faces;       /* Cursor through the face list. */
    /* 0x3C */ u8 pad3C[0xC8 - 0x3C];
} TornadoMesh; /* 0xC8 */

typedef struct {
    /* 0x00 */ s32 visible;
    /* 0x04 */ u8 pad04[0xC - 0x4];
} TornadoFace; /* 0xC */

#define TORNADO_GRID_ROWS 8
#define TORNADO_GRID_COLS 9
#define TORNADO_FACE_COUNT 522

extern s32 D_801DDDBC[TORNADO_GRID_ROWS][TORNADO_GRID_COLS];
extern s32 D_801DDEDC;
extern TornadoFace D_801DDEEC[TORNADO_FACE_COUNT];

void func_801A122C(TornadoMesh *mesh, u8 *verts);

typedef struct {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s16 timer;
    /* 0x0E */ s16 pad0E;
    /* 0x10 */ s16 x;
    /* 0x12 */ s16 y;
    /* 0x14 */ s16 z;
} TornadoCameraTask;

extern MATRIX D_801CBDEC;

void func_801A2890(s32 angle, s32 level);
void func_801A2C98(s32 p0, s32 p1, s32 p2, s32 p3, s32 p4, s32 p5);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A0000);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A00DC);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A041C);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A0774);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A0878);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A0CC0);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A1144);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A122C);

static void func_801A140C(TornadoMesh *mesh) {
    s32 i;
    s32 j;

    D_801DDEDC = 0;
    for (i = 0; i < TORNADO_GRID_ROWS; i++) {
        for (j = 0; j < TORNADO_GRID_COLS; j++) {
            D_801DDDBC[i][j] = 1;
        }
    }
    for (i = 0; i < TORNADO_FACE_COUNT; i++) {
        D_801DDEEC[i].visible = 1;
    }
    mesh->verts = mesh->data->verts;
    mesh->faces = (u8 *)mesh->data + mesh->data->faceOffset;
    func_801A122C(mesh, mesh->verts);
}

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A14B0);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A187C);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A2144);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A2468);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A2520);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A2890);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A2C98);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A3060);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A3370);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A382C);

static s32 func_801A3944(TornadoCameraTask *task) {
    MATRIX m;
    s32 t;
    s32 f;
    s32 p0;
    s32 p1;
    s32 p2;
    s32 p3;
    s32 p4;
    s32 p5;

    if (!(D_800EEC5C & (BATTLE_STATE_UNK200 | BATTLE_STATE_UNK001))) {
        t = task->timer;
        if (t < 40) {
            f = (t << 12) / 40;
            p0 = ((5000 - 80) * f >> 12) + 80;
            p1 = ((3000 - 1500) * f >> 12) + 1500;
            p2 = ((1000 - 100) * f >> 12) + 100;
            p3 = ((1664 - 2304) * f >> 12) + 2304;
            p4 = 2048;
            p5 = ((0 - 3328) * f >> 12) + 3328;
        } else if ((t -= 40) < 18) {
            p0 = 5000;
            p1 = 3000;
            p2 = 1000;
            p3 = 1664;
            p4 = 2048;
            p5 = 0;
        } else if ((t -= 18) < 18) {
            f = (t << 12) / 18;
            p0 = ((9000 - 5000) * f >> 12) + 5000;
            p1 = ((400 - 3000) * f >> 12) + 3000;
            p2 = ((800 - 1000) * f >> 12) + 1000;
            p3 = ((0 - 1664) * f >> 12) + 1664;
            p4 = ((1280 - 2048) * f >> 12) + 2048;
            p5 = (768 * f >> 12);
        }
        func_801A2C98(p0, p1, p2, p3, p4, p5);
    }

    D_801CBDEC.t[0] = task->x;
    D_801CBDEC.t[1] = task->y;
    D_801CBDEC.t[2] = task->z;
    CompMatrix(&D_800F02C8, &D_801CBDEC, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    /* p3 is reused for the fade level; a separate variable does not match. */
    if (task->timer < 16) {
        p3 = task->timer * 8;
    } else if (task->timer < 68) {
        p3 = 128;
    } else {
        p3 = 128 - (task->timer - 68) * 16;
    }
    func_801A2890(task->timer * 100, p3);

    if (!(D_800EEC5C & (BATTLE_STATE_UNK200 | BATTLE_STATE_UNK001))) {
        return ++task->timer > 75 ? 2 : 0;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A3BF0);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A3F80);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A4258);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A4314);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A4548);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A4CB4);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A4D10);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A4EA4);

INCLUDE_ASM("asm/ovl/effect_146/nonmatchings/effect_146", func_801A5440);
