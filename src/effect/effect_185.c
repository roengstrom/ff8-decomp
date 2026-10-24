/**
 * @file effect_185.c
 * @brief Shiva Summon (Diamond Dust)
 */
#include "common.h"
#include "battle.h"
#include "battle/bc_object8.h"
#include "battle/bc_object16.h"
#include "psxsdk/libc.h"

#define SHIVA_PRIM_FLAG_UNK40 0x40
#define SHIVA_PRIM_FLAG_UNK80 0x80

typedef struct {
    /* 0x00 */ u8 *dispatch;   /* Draw script the packet runs. */
    /* 0x04 */ u8 pad04[0x8 - 0x4];
    /* 0x08 */ s32 unk08;
    /* 0x0C */ s32 depth;      /* Fade amount, applied when both SHIVA_PRIM_FLAG_UNK40 and SHIVA_PRIM_FLAG_UNK80 are set. */
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;      /* Texture scroll, driven by the frame counter. */
    /* 0x18 */ s16 unk18;
    /* 0x1A */ s16 unk1A;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
    /* 0x20 */ s32 flags;
    /* 0x24 */ u8 pad24[0x7C - 0x24];
} ShivaPrim; /* 0x7C */

extern MATRIX D_801B0E54;
extern u8 D_801A775C[];
extern u8 D_801AA850[];
extern s32 D_8019EDEC[];
extern s32 D_801AE5DC[];
extern ParticleEntry *D_801AFD88;

s32 func_80191940(ShivaPrim *prim, u32 *ot, s32 mode, s32 head);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80190000);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80190288);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80190790);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80190D90);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801912D8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191940);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191BD8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191C60);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191C90);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191D04);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191DF4);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191EFC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80191F90);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80192094);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801920E8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80192294);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801923D4);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801924E8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801925D8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80192724);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801929E4);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80192EAC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80192FCC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193764);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193A20);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193BD0);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193C74);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193CA8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193CF8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80193DF0);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80194CD0);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80194D84);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019514C);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801951CC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80195424);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80195694);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801956FC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80195900);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80195B1C);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80195C20);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80196234);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80196868);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80196928);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80196BB4);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80196BF8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801972CC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80197354);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801975BC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80197600);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019788C);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80197958);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80197984);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801979A8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80197D30);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801983BC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801984FC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80198698);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80198714);

static s32 func_80198A28(ParticleEntry *p) {
    SVECTOR rot;
    MATRIX m;
    VECTOR scale;
    ShivaPrim *prim;
    s32 newFrame;

    rot.vx = 0;
    rot.vy = 64;
    rot.vz = 0;
    RotMatrixYXZ(&rot, &m);

    m.t[0] = p->posX;
    m.t[1] = p->posY;
    m.t[2] = p->posZ;
    scale.vx = scale.vz = p->sizeX;
    scale.vy = p->sizeY;
    ScaleMatrix(&m, &scale);
    CompMatrix(&D_801B0E54, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    prim = func_800B3698(124);
    prim->dispatch = D_801AA850;
    prim->flags = 0x30;
    prim->unk08 = 0;
    prim->unk18 = 0;
    prim->unk1A = 0;
    prim->unk1C = 64;
    prim->unk1E = 128;
    prim->unk10 = 0;
    prim->unk14 = (s16)p->frame << 3;

    if ((s16)p->frame < 4) {
        prim->depth = 4096 - ((s16)p->frame << 10);
        prim->flags |= SHIVA_PRIM_FLAG_UNK40 | SHIVA_PRIM_FLAG_UNK80;
    } else if ((s16)p->frame >= 8) {
        prim->depth = ((s16)p->frame - 8) << 10;
        prim->flags |= SHIVA_PRIM_FLAG_UNK40 | SHIVA_PRIM_FLAG_UNK80;
    }

    D_800FA5F0 = func_80191940(prim, D_800FA5E8->ot, 3, D_800FA5F0);
    func_800B36B8(124);

    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }

    newFrame = p->frame + 1;
    p->frame = newFrame;
    return ((s16)newFrame > 12) * 2;
}

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80198BE4);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80198D08);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80199184);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_801995FC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80199824);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80199A34);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_80199E58);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019A0C8);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019A108);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019A54C);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019A970);

static s32 func_8019AFD4(ParticleEntry *p) {
    SVECTOR rot;
    MATRIX m;
    VECTOR scale;
    BattleEffectPrim *prim;
    s32 newFrame;

    rot.vx = 0;
    rot.vy = 0;
    rot.vz = p->angle;
    RotMatrixYXZ(&rot, &m);

    m.t[0] = p->posX;
    m.t[1] = p->posY;
    m.t[2] = p->posZ;
    scale.vx = scale.vy = p->sizeX;
    scale.vz = p->sizeY;
    ScaleMatrix(&m, &scale);
    CompMatrix(&D_801B0E54, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    prim = func_800B3698(88);
    prim->dispatch = D_8019EDEC;
    prim->bg.word = 0;
    prim->flags = 0x33;
    if ((s16)p->frame >= 22) {
        prim->depth = ((s16)p->frame - 22) * 409;
        prim->flags |= SHIVA_PRIM_FLAG_UNK40 | SHIVA_PRIM_FLAG_UNK80;
    }

    D_800FA5F0 = func_800CBC68(prim, D_800FA5E8->ot, 2, D_800FA5F0);
    func_800B36B8(88);

    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }

    p->angle += p->angVel;
    p->sizeX += p->sizeXVel;
    if (p->sizeX >= 28672) {
        p->sizeX = 28672;
    }
    p->sizeY += p->sizeYVel;
    if (p->sizeY >= 28672) {
        p->sizeY = 28672;
    }
    newFrame = p->frame + 1;
    p->frame = newFrame;
    return ((s16)newFrame > 31) * 2;
}

static s32 func_8019B1AC(ParticleEntry *p) {
    SVECTOR rot;
    MATRIX m;
    VECTOR scale;
    BattleEffectPrim *prim;
    s32 newFrame;

    if (p->delay > 0) {
        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }
        p->delay--;
    } else {
        if ((s16)p->frame == 0) {
            p->sizeX = p->sizeXVel = (D_801AFD88->sizeX + rand() % 2048) / 5;
        }

        rot.vx = 1024;
        rot.vy = p->angle;
        rot.vz = 0;
        RotMatrix(&rot, &m);

        m.t[0] = p->posX;
        m.t[1] = p->posY;
        m.t[2] = p->posZ;
        scale.vx = scale.vz = p->sizeX;
        scale.vy = p->sizeY;
        ScaleMatrix(&m, &scale);
        CompMatrix(&D_801B0E54, &m, &m);
        SetRotMatrix(&m);
        SetTransMatrix(&m);

        prim = func_800B3698(88);
        prim->dispatch = D_801AE5DC;
        prim->bg.word = 0;
        prim->flags = 0x33;
        if ((s16)p->frame >= 12) {
            prim->depth = ((s16)p->frame - 12) * 682;
            prim->flags |= SHIVA_PRIM_FLAG_UNK40 | SHIVA_PRIM_FLAG_UNK80;
        }

        D_800FA5F0 = func_800CBC68(prim, D_800FA5E8->ot, 2, D_800FA5F0);
        func_800B36B8(88);

        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }

        p->angle += p->angVel;
        p->angVel -= (s16)p->angVel >> 3;
        p->sizeX += p->sizeXVel;
        p->sizeXVel -= p->sizeXVel / 3;
        newFrame = p->frame + 1;
        p->frame = newFrame;
        return ((s16)newFrame > 17) * 2;
    }
    return 0;
}

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019B42C);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019BAD0);

static s32 func_8019BE18(ParticleEntry *p) {
    SVECTOR rot;
    MATRIX m;
    VECTOR scale;
    ShivaPrim *prim;
    s32 newFrame;

    rot.vx = 0;
    rot.vy = 2048;
    rot.vz = 0;
    RotMatrixYXZ(&rot, &m);

    m.t[0] = p->posX;
    m.t[1] = p->posY;
    m.t[2] = p->posZ;
    scale.vx = scale.vz = p->sizeX;
    scale.vy = p->sizeY;
    ScaleMatrix(&m, &scale);
    CompMatrix(&D_801B0E54, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    prim = func_800B3698(124);
    prim->dispatch = D_801A775C;
    prim->flags = 0x33;
    prim->unk18 = 64;
    prim->unk1C = 64;
    prim->unk08 = 0;
    prim->unk1A = 0;
    prim->unk1E = 128;
    prim->unk10 = 0;
    prim->unk14 = (s16)p->frame << 4;

    if ((s16)p->frame >= 22) {
        prim->depth = ((s16)p->frame - 22) << 9;
        prim->flags |= SHIVA_PRIM_FLAG_UNK40 | SHIVA_PRIM_FLAG_UNK80;
    }

    D_800FA5F0 = func_80191940(prim, D_800FA5E8->ot, 2, D_800FA5F0);
    func_800B36B8(124);

    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }

    if ((s16)p->frame < 6) {
        p->sizeX += p->sizeXVel;
        p->sizeY += p->sizeYVel;
        p->sizeXVel -= p->sizeXVel / 3;
        p->sizeYVel -= p->sizeYVel / 3;
    }
    newFrame = p->frame + 1;
    p->frame = newFrame;
    return ((s16)newFrame > 29) * 2;
}

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019C024);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019C078);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019C6E0);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019C71C);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019CA98);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019CB50);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019D000);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019D078);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019D4BC);

INCLUDE_ASM("asm/ovl/effect_185/nonmatchings/effect_185", func_8019DA0C);
