#include "common.h"
#include "psxsdk/libgte.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/inline_c.h"
#include "battle.h"
#include "battle/bc_object8.h"
#include "battle/bc_object15.h"

extern BattleSpriteAnim *D_800EBF24[];
extern u8 D_800FB408[];
extern u8 D_800FA5F8[];
extern u8 D_800E6658[];

/* GTE FLAG bits 17 and 18: divide overflow, SZ3/OTZ saturated -- out of range. */
#define BATTLE_SPRITE_GTE_OUT_OF_RANGE ((1 << 18) | (1 << 17))
/* GP0(E1h) draw mode: dither on, texture page blend mode 1 (additive). */
#define BATTLE_SPRITE_ADDITIVE_MODE (0xE1000000 | (1 << 9) | getTPage(0, 1, 0, 0))
/* Texture coordinates stop at the page edge. */
#define BATTLE_SPRITE_UV_MAX 0xFF

static void *func_800C97E4(BattleSpritePrim *prim, u32 *ot, s32 otShift, void *head);

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C7294); /* 0x58 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C72EC); /* 0x68 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C7354); /* 0xF8 */

/**
 * @brief Register a particle effect with a callback from D_800E6658 table.
 *
 * Looks up a callback function pointer from D_800E6658[idx], allocates
 * an entry from D_800FB408, clears initial fields, and stores the
 * source entity pointer.
 *
 * @param a0 Source entity pointer (stored at result offset 0x20).
 * @param idx Index into D_800E6658 callback table.
 */
void func_800C744C(s32 a0, s32 idx) {
    u8 *entry = func_800B2A84(D_800FB408, *(void **)(D_800E6658 + idx * 4));
    *(u16 *)(entry + 0xC) = 0;
    *(u16 *)(entry + 0xE) = 0;
    *(s32 *)(entry + 0x20) = a0;
}

/**
 * @brief Initialize D_800FB408 buffer via func_800B2A00 and return it.
 *
 * Calls func_800B2A00 with D_800FB408, D_800FA5F8, size 0x24, and count 0x64.
 *
 * @return Pointer to D_800FB408.
 */
u8 *func_800C749C(void) {
    u8 *buf = D_800FB408;
    func_800B2A00(buf, D_800FA5F8, 0x24, 0x64);
    return buf;
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C74E0); /* 0x1EC */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C76CC); /* 0xC0 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C778C); /* 0x998 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C8124); /* 0x404 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C8528); /* 0x18C */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C86B4); /* 0x114 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C87C8); /* 0x2B0 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C8A78); /* 0x15C */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C8BD4); /* 0x138 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C8D0C); /* 0x148 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C8E54); /* 0x1C0 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C9014); /* 0xBC */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C90D0); /* 0xB0 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C9180); /* 0xAC */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C922C); /* 0x90 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C92BC); /* 0xB8 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C9374); /* 0xB0 */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C9424); /* 0x94 */

/**
 * @brief Look up a pointer from D_800EBF24 array with fallback.
 *
 * Loads the entry at D_800EBF24[index]. If the entry's flag byte is
 * non-zero, returns D_800EBF24[0] as fallback.
 *
 * @param index Array index.
 * @return Entry from D_800EBF24[index] or D_800EBF24[0] on fallback.
 */
BattleSpriteAnim *func_800C94B8(s32 index) {
    BattleSpriteAnim *entry = D_800EBF24[index];

    if (entry->flag == 0)
        return entry;

    return D_800EBF24[0];
}

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C94EC); /* 0x13C */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C9628); /* 0xBC */

INCLUDE_ASM("asm/ovl/battle/nonmatchings/bc_object15", func_800C96E4); /* 0x100 */

/**
 * @brief Emit one POLY_FT4 per sprite of the frame @p prim holds.
 *
 * Each sprite's own rotation, scale and position are composed onto the set's
 * matrix on the GTE, its corners projected, and the packet filled in between
 * the GTE ops. The set's depth comes from its first sprite: until that one
 * projects @p otShift is the shift from the averaged Z to the OT index, and
 * afterwards it holds the index itself. A sprite the GTE flags as out of
 * range is skipped, and a set drawn additively gets its draw-mode packet
 * linked with the first sprite.
 *
 * @param prim    The packet @ref func_800C9E10 prepared.
 * @param ot      Ordering table.
 * @param otShift Shift from depth to OT index, then the index (see above).
 * @param head    Prim buffer cursor.
 * @return The cursor past the packets emitted.
 *
 * @note The draw-mode packet is reached through @c next before @c tp takes
 *       it: that copy is what keeps @c tp in a0 while the OT slot takes a1.
 *       The depth test is spelled with the known-depth case first so the
 *       plain link is one block.
 */
static void *func_800C97E4(BattleSpritePrim *prim, u32 *ot, s32 otShift, void *head) {
    POLY_FT4 *p = head;
    POLY_FT4 *next;
    BattleSprite *sprite = prim->sprites;
    s32 count = prim->spriteCount;
    s32 i;
    s32 flags;
    s32 value; /* angle, then each scale: one local, the target keeps all three in s0 */
    s32 w;
    s32 h;
    s32 uv;
    u32 shade;
    DR_MODE *tp;

    for (i = 0; i < count; i++, sprite++) {
        prim->m.m[2][1] = 0;
        prim->m.m[2][0] = 0;
        prim->m.m[1][2] = 0;
        prim->m.m[1][0] = 0;
        prim->m.m[0][2] = 0;
        prim->m.m[0][1] = 0;
        prim->m.m[2][2] = ONE;
        flags = sprite->flags;
        if (flags & BATTLE_SPRITE_TRANSFORMED) {
            value = sprite->angle;
            if (value == 0) {
                prim->cos = ONE;
                prim->sin = 0;
                prim->lastAngle = 0;
            } else if (value != prim->lastAngle) {
                prim->cos = rcos(value);
                prim->sin = rsin(value);
                prim->lastAngle = value;
            }
            value = sprite->scaleX;
            prim->m.m[0][0] = prim->cos * value >> 12;
            prim->m.m[1][0] = prim->sin * value >> 12;
            value = sprite->scaleY;
            prim->m.m[0][1] = -prim->sin * value >> 12;
            prim->m.m[1][1] = prim->cos * value >> 12;
        } else {
            prim->m.m[1][1] = ONE;
            prim->m.m[0][0] = ONE;
        }
        w = sprite->w;
        h = sprite->h;
        prim->corners[2].vx = -(w << 3);
        prim->corners[0].vx = -(w << 3);
        prim->corners[3].vx = w << 3;
        prim->corners[1].vx = w << 3;
        prim->corners[1].vy = -(h << 3);
        prim->corners[0].vy = -(h << 3);
        prim->corners[3].vy = h << 3;
        prim->corners[2].vy = h << 3;
        prim->m.t[0] = (sprite->x << 4) + (w << 3);
        prim->m.t[1] = (sprite->y << 4) + (h << 3);
        prim->m.t[2] = 0;
        /* m = mtx * m, a column at a time, with the packet filled in the GTE's shadow. */
        gte_SetRotMatrix(&prim->mtx);
        gte_ldclmv(&prim->m.m[0][0]);
        gte_rtir();
        setlen(p, 9);
        /* One word each: w, shade, h, code and u, v, clut are laid out as the packet's. */
        *(u32 *)&p->r0 = *(u32 *)&sprite->w;
        *(u32 *)&p->u0 = *(u32 *)&sprite->u;
        gte_stclmv(&prim->m.m[0][0]);
        gte_ldclmv(&prim->m.m[0][1]);
        gte_rtir();
        p->r0 = p->b0 = p->g0;
        shade = p->r0 * prim->colour.r;
        p->r0 = shade >> 7;
        gte_stclmv(&prim->m.m[0][1]);
        gte_ldclmv(&prim->m.m[0][2]);
        gte_rtir();
        p->u2 = sprite->u;
        shade = p->g0 * prim->colour.g;
        p->g0 = shade >> 7;
        gte_stclmv(&prim->m.m[0][2]);
        gte_SetTransMatrix(&prim->mtx);
        gte_ldlv0(prim->m.t);
        gte_rt();
        p->v1 = sprite->v;
        shade = p->b0 * prim->colour.b;
        p->b0 = shade >> 7;
        gte_stlvnl(prim->m.t);
        gte_SetRotMatrix(&prim->m);
        gte_SetTransMatrix(&prim->m);
        gte_ldv3(&prim->corners[0], &prim->corners[1], &prim->corners[2]);
        gte_rtpt();
        uv = sprite->u + w - prim->uvInset;
        if (uv > BATTLE_SPRITE_UV_MAX) {
            uv = BATTLE_SPRITE_UV_MAX;
        }
        p->u3 = uv;
        p->u1 = uv;
        /* The row select is the flag word's top nibble, unsigned. */
        p->clut += getClut(0, prim->clutRow[(u16)flags >> BATTLE_SPRITE_CLUT_ROW_SHIFT]);
        gte_stflg(&prim->gteFlag);
        if (!(prim->gteFlag & BATTLE_SPRITE_GTE_OUT_OF_RANGE)) {
            p->tpage = flags & BATTLE_SPRITE_TPAGE_MASK;
            gte_stsxy3(&p->x0, &p->x1, &p->x2);
            gte_ldv0(&prim->corners[3]);
            gte_rtps();
            uv = sprite->v + h - prim->uvInset;
            if (uv > BATTLE_SPRITE_UV_MAX) {
                uv = BATTLE_SPRITE_UV_MAX;
            }
            p->v3 = uv;
            p->v2 = uv;
            gte_stsxy(&p->x3);
            if (prim->otz >= 0) {
                addPrim(&ot[otShift], p);
                p++;
            } else {
                gte_avsz4();
                gte_stotz(&prim->otz);
                otShift = prim->otz >> otShift;
                if (prim->flags & BATTLE_SPRITE_FLAG_ADDITIVE) {
                    next = p + 1;
                    setlen(next, 1);
                    tp = (DR_MODE *)next;
                    next = (POLY_FT4 *)(tp + 1);
                    tp->code[0] = BATTLE_SPRITE_ADDITIVE_MODE;
                    addPrim(&ot[otShift], tp);
                    addPrim(&ot[otShift], p);
                    p = next;
                } else {
                    addPrim(&ot[otShift], p);
                    p++;
                }
            }
        }
    }
    return p;
}

/**
 * @brief Draw one frame of a sprite animation.
 *
 * Resolves the frame out of the animation, carries the frame's flag bit into
 * the packet, takes the GTE's current rotation and translation as the base
 * matrix and rolls and scales it as the flags ask, applies the animation's
 * default CLUT rows, then hands the packet to the emitter.
 *
 * @param prim    Packet the caller filled (see @ref BattleSpritePrim).
 * @param ot      Ordering table the sprites are linked into.
 * @param otShift Shift from the set's averaged depth to its OT index.
 * @param head    Prim buffer cursor.
 * @return The cursor past the sprites emitted.
 */
void *func_800C9E10(BattleSpritePrim *prim, u32 *ot, s32 otShift, void *head) {
    BattleSpriteAnim *anim = prim->anim;
    u16 *offsets = anim->offsets;
    u8 *frame;

    prim->frameCount = anim->frameCount;
    /* Frames are byte offsets into the table; each starts with its count. */
    frame = (u8 *)anim + offsets[prim->frame];
    /* A byte cursor bumped past the count word; re-reading it through the
     * field (not frame + 4) is what the codegen wants. */
    prim->sprites = (BattleSprite *)frame;
    prim->spriteCount = *(s32 *)frame;
    prim->sprites = (BattleSprite *)((s32 *)prim->sprites + 1);
    prim->nextOffset = offsets[prim->frame + 1];
    if (prim->spriteCount < 0) {
        prim->spriteCount &= ~BATTLE_SPRITE_COUNT_FLAG;
        prim->flags |= BATTLE_SPRITE_FLAG_ADDITIVE;
    }
    prim->corners[3].vz = 0;
    prim->corners[2].vz = 0;
    prim->corners[1].vz = 0;
    prim->corners[0].vz = 0;
    gte_ReadRotMatrix(&prim->mtx);
    if (prim->flags & BATTLE_SPRITE_FLAG_ROTATE) {
        RotMatrixZ(prim->angle - D_800F02A0, &prim->mtx);
    } else if (!(prim->flags & BATTLE_SPRITE_FLAG_NO_ROLL)) {
        RotMatrixZ(-D_800F02A0, &prim->mtx);
    }
    if (prim->flags & BATTLE_SPRITE_FLAG_SCALE) {
        prim->scale.vz = ONE;
        ScaleMatrixL(&prim->mtx, &prim->scale);
    }
    if (!(prim->flags & BATTLE_SPRITE_FLAG_COLOUR)) {
        /* One word: the reset does not touch the four channels separately. */
        *(u32 *)&prim->colour = BATTLE_SPRITE_COLOUR_DEFAULT;
    }
    if (!(prim->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT0)) {
        prim->clutRow[0] = anim->clutRow[0];
    }
    if (!(prim->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT1)) {
        prim->clutRow[1] = anim->clutRow[1];
    }
    if (!(prim->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT2)) {
        prim->clutRow[2] = anim->clutRow[2];
    }
    if (!(prim->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT3)) {
        prim->clutRow[3] = anim->clutRow[3];
    }
    if (prim->flags & BATTLE_SPRITE_FLAG_UV_INSET) {
        prim->uvInset = 1;
    } else {
        prim->uvInset = 0;
    }
    prim->cos = ONE;
    prim->lastAngle = 0;
    prim->sin = 0;
    prim->otz = -1;
    return func_800C97E4(prim, ot, otShift, head);
}
