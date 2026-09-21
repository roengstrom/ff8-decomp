/**
 * @file effect_002.c
 * @brief Fire
 */
#include "common.h"
#include "effect/effect_002.h"

/** @brief Half the frame data the task flips between. */
#define FIRE_HALF 0xA000

/** @brief @ref FireTask::frame -- its low bit picks which half is read. */
#define FIRE_FRAME_ODD 0x1

/** @brief How long the flame plays before the script moves on. */
#define FIRE_LAST_FRAME 29

/** @brief Where on screen the flame is drawn from. */
#define FIRE_ORIGIN_X 160
#define FIRE_ORIGIN_Y 120

/** @brief The shade a sprite is drawn at when it carries no colour of its own. */
#define FIRE_MODEL_SHADE 0x808080

/** @brief The scratch record the flame is posed through. */
typedef struct {
    /* 0x00 */ BattleSpriteAnim *anim;
    /* 0x04 */ u16 frame;
    /* 0x06 */ u16 flags;
    /* 0x08 */ u32 colour;
    /* 0x0C */ u8 clutRow[4];
    /* 0x10 */ u16 frameCount;
    /* 0x12 */ u16 nextOffset;   /**< Offset of the frame after the one drawn. */
    /* 0x14 */ u8 *sprites;      /**< First sprite of the frame drawn. */
    /* 0x18 */ s32 spriteCount;
    /* 0x1C */ s32 uvInset;
    /* 0x20 */ s16 x;
    /* 0x22 */ s16 y;
} FireModel; /* 0x24 */

/** @brief GP0(E1h): the draw mode the flame's sprites are drawn under, dithered. */
#define FIRE_DRAW_MODE 0xE1000200

/** @brief A sprite runs off its texture page past this many texels. */
#define FIRE_TPAGE_EDGE 0x100

/**
 * @brief One sprite of the flame as drawn: the draw mode it needs and the
 *        SPRT behind it, linked into the OT as one five-word packet.
 */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ SPRT sprt;        /**< Its own tag slot carries the @ref FIRE_DRAW_MODE word. */
} FirePrim; /* 0x18 */

/** @brief How many battle slots the flame keeps a word for. */
#define FIRE_SLOTS 7

/** @brief The record the effect's frame task runs in. */
typedef struct {
    /* 0x00 */ u8 pad000[0xC];
    /* 0x0C */ u16 frame;
    /* 0x0E */ u8 pad00E[0x10 - 0xE];
} FireTask; /* 0x10 */

/** @brief The record one of the effect's children runs in. */
typedef struct {
    /* 0x00 */ u8 pad000[0xC];
    /* 0x0C */ s16 frame;      /**< Counts up only once its slot is free. */
    /* 0x0E */ s16 seat;       /**< Which entry of the animation set it plays. */
    /* 0x10 */ u8 pad010[0x20 - 0x10];
    /* 0x20 */ s16 slot;       /**< The battle slot it is cast on. */
    /* 0x22 */ s16 age;        /**< Counts up every frame it runs. */
} FireChild; /* 0x24 */

/** @brief How long one of the flame's sparks plays for. */
#define FIRE_SPARK_LAST_FRAME 15

/** @brief The record one of the flame's sparks runs in. */
typedef struct {
    /* 0x00 */ u8 pad000[0xC];
    /* 0x0C */ u16 frame;
    /* 0x0E */ s16 delay;      /**< Frames to wait before the spark appears. */
    /* 0x10 */ SVECTOR pos;    /**< @c pad carries how fast it is falling. */
    /* 0x18 */ u8 pad018[0x1C - 0x18];
    /* 0x1C */ s16 scale;
} FireSpark;

/** @brief How long one of the flame's bursts plays for. */
#define FIRE_BURST_LAST_FRAME 15

/** @brief How far above the ground the burst's glow has faded to nothing. */
#define FIRE_GLOW_RANGE 2000

/** @brief How bright the glow is drawn when the burst sits on the ground. */
#define FIRE_GLOW_SHADE 160

/** @brief The record one of the flame's bursts runs in. */
typedef struct {
    /* 0x00 */ u8 pad000[0xC];
    /* 0x0C */ s16 frame;
    /* 0x0E */ s16 delay;      /**< Frames to wait before the burst appears. */
    /* 0x10 */ SVECTOR pos;    /**< @c pad carries the ground level beneath it. */
    /* 0x18 */ u8 pad018[0x1C - 0x18];
    /* 0x1C */ s16 scale;
} FireBurst;

/** @brief The record one of the flame's embers runs in. */
typedef struct {
    /* 0x00 */ u8 pad000[0xC];
    /* 0x0C */ u16 frame;      /**< Counts down; the ember ends when it wraps. */
    /* 0x0E */ s16 delay;      /**< Frames to wait before the ember appears. */
    /* 0x10 */ SVECTOR pos;
    /* 0x18 */ SVECTOR vel;
    /* 0x20 */ s16 scale;
} FireEmber;

static s32 func_801A0174(FireEmber *ember);
static s32 func_801A02FC(FireSpark *spark);
static s32 func_801A0434(FireBurst *burst);
static s32 func_801A0A70(FireChild *flame);
static void *func_801A0F3C(FireModel *model, u32 *ot, s32 z, void *head);
static void *func_801A110C(FireModel *model, SVECTOR *at, u32 *ot, void *head);
static s32 func_801A1284(FireTask *task);
static s32 func_801A1340(FireChild *cue);
static s32 func_801A1810(FireTask *task);

/**
 * @brief Start the effect and hand back its task pool.
 *
 * @param animSet Animation set the effect plays.
 * @return The pool the frame task lives in.
 */
void *func_801A0000(EffectAnimSet *animSet) {
    FireTask *task;
    FireChild *child;
    s32 i;

    D_801C2CF4 = animSet;
    D_801C2CFC = animSet->slot;
    for (i = 0; i < FIRE_SLOTS; i++) {
        D_801C2CD8[i] = 1;
    }
    func_800B2A00(&D_801C2D14, &D_801C2D04, sizeof(FireTask), 1);
    task = func_800B2A84(&D_801C2D14, func_801A1810);
    task->frame = 0;
    func_800B2A00(&D_801C4244, &D_801C2D24, sizeof(FireChild), 150);
    if (D_801C2CF4->anims->unk010 >= 2) {
        child = func_800B2A84(&D_801C4244, func_801A0A70);
        child->frame = 0;
        child->seat = 0;
        child->age = 0;
    } else {
        child = func_800B2A84(&D_801C4244, func_801A1340);
        child->frame = 0;
        child->seat = 0;
        child->slot = D_801C2CF4->anims->parts->unk000;
        child->age = 0;
    }
    if (!(D_801C2CF4->flags & EFFECT_ANIMSET_FLAG_LOADED)) {
        func_800C3BE0(&D_801A187C);
        func_800BB084(&D_801A1EAC);
    }
    return &D_801C2D14;
}

/**
 * @brief Draw one of the flame's embers, drifting and shrinking as it goes.
 *
 * @param ember The ember to draw.
 * @return 2 once its frame counter has wrapped past zero.
 */
static s32 func_801A0174(FireEmber *ember) {
    BattleSpritePrim *prim;
    void *head;
    u32 frame;

    if (ember->delay > 0) {
        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }
        ember->delay--;
    } else {
        func_800C96E4(&ember->pos, ember->scale, -(ember->scale >> 3));
        prim = func_800B3698(sizeof(BattleSpritePrim));
        prim->anim = func_800C94B8(5);
        prim->frame = 9 - ember->frame;
        prim->clutRow[0] = 4;
        prim->flags = BATTLE_SPRITE_FLAG_KEEP_CLUT0;
        head = D_801D8254;
        D_801D8254 = func_800C9E10(prim, D_800FA5E8->ot, 2, head);
        func_800B36B8(sizeof(BattleSpritePrim));
        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }
        frame = ember->frame - 1;
        ember->pos.vx += ember->vel.vx;
        ember->vel.vx -= ember->vel.vx >> 4;
        ember->frame = frame;
        ember->pos.vy += ember->vel.vy;
        ember->vel.vy += ember->vel.vy >> 4;
        ember->pos.vz += ember->vel.vz;
        ember->vel.vz -= ember->vel.vz >> 4;
        ember->scale -= 0x40;
        /* bit 15 is set once the counter wraps below zero; this moves it to
         * bit 1, which reads as the 2 that ends the task. */
        return (frame >> 14) & 2;
    }
    return 0;
}

/**
 * @brief Draw one of the flame's sparks, falling and shrinking as it goes.
 *
 * @param spark The spark to draw.
 * @return 2 once it has played every frame of its animation.
 */
static s32 func_801A02FC(FireSpark *spark) {
    BattleSpritePrim *prim;
    s16 frame;
    void *head;

    if (spark->delay > 0) {
        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }
        spark->delay--;
    } else {
        func_800C96E4(&spark->pos, spark->scale, -(spark->scale >> 3));
        prim = func_800B3698(sizeof(BattleSpritePrim));
        prim->anim = &D_801C2824;
        prim->frame = spark->frame;
        prim->flags = 0;
        head = D_801D8254;
        D_801D8254 = func_800C9E10(prim, D_800FA5E8->ot, 2, head);
        func_800B36B8(sizeof(BattleSpritePrim));
        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }
        frame = spark->frame + 1;
        spark->pos.vy += spark->pos.pad;
        spark->pos.pad += spark->pos.pad >> 5;
        spark->scale -= spark->scale >> 6;
        spark->frame = frame;
        return (frame > FIRE_SPARK_LAST_FRAME) * 2;
    }
    return 0;
}

/**
 * @brief Draw one of the flame's bursts and, on its first frame, throw off
 *        the sparks and embers it scatters.
 *
 * The burst is drawn twice: the flame itself facing the camera, then its glow
 * laid flat on the ground beneath it, dimmer the higher the burst sits.
 *
 * @param burst The burst to draw.
 * @return 2 once it has played every frame of its animation.
 */
static s32 func_801A0434(FireBurst *burst) {
    BattleSpritePrim *prim;
    FireSpark *spark;
    FireEmber *ember;
    SVECTOR rot;
    MATRIX m;
    VECTOR scale;
    s32 i;
    s32 shade;
    s16 frame;
    void *head;

    if (burst->delay > 0) {
        if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
            return 0;
        }
        burst->delay--;
        return 0;
    }
    func_800C96E4(&burst->pos, burst->scale, -(burst->scale >> 4));
    prim = func_800B3698(sizeof(BattleSpritePrim));
    prim->anim = &D_801C24CC;
    prim->frame = burst->frame;
    prim->flags = 0;
    head = D_801D8254;
    D_801D8254 = func_800C9E10(prim, D_800FA5E8->ot, 2, head);

    rot.vx = ONE / 4;
    rot.vy = 0;
    rot.vz = 0;
    RotMatrix(&rot, &m);
    m.t[0] = burst->pos.vx;
    m.t[1] = burst->pos.pad;
    m.t[2] = burst->pos.vz;
    scale.vx = scale.vy = burst->scale;
    scale.vz = ONE;
    ScaleMatrix(&m, &scale);
    CompMatrix(&D_800F02C8, &m, &m);
    SetRotMatrix(&m);
    SetTransMatrix(&m);

    /* The height, its fraction of the range and the shade are one variable:
     * split, the height takes a0 away from the prim below. */
    shade = burst->pos.vy;
    if (shade < -FIRE_GLOW_RANGE) {
        shade = -FIRE_GLOW_RANGE;
    }
    shade = shade * ONE / -FIRE_GLOW_RANGE;
    shade = FIRE_GLOW_SHADE - shade * FIRE_GLOW_SHADE / ONE;
    prim->anim = &D_801C2678;
    prim->frame = burst->frame;
    prim->colour.b = shade;
    prim->colour.g = shade;
    prim->colour.r = shade;
    prim->flags = BATTLE_SPRITE_FLAG_COLOUR | BATTLE_SPRITE_FLAG_UV_INSET;
    head = D_801D8254;
    D_801D8254 = func_800C9E10(prim, D_800FA5E8->ot, 2, head);
    func_800B36B8(sizeof(BattleSpritePrim));
    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }
    if (burst->frame == 0) {
        for (i = 0; i < 4; i++) {
            spark = func_800B2A84(&D_801C4244, func_801A02FC);
            spark->frame = 0;
            spark->delay = i + rand() % 6;
            spark->pos = burst->pos;
            spark->pos.vx += -150 + rand() % 300;
            spark->pos.vy -= rand() % 100;
            spark->pos.vz += -150 + rand() % 300;
            spark->pos.pad = -(rand() % 30 + 20);
            spark->scale = rand() % 1280 + 2560;
        }
        for (i = 0; i < 10; i++) {
            ember = func_800B2A84(&D_801C4244, func_801A0174);
            ember->frame = rand() % 4 + 5;
            ember->delay = i + rand() % 4;
            ember->pos = burst->pos;
            ember->pos.vx += -200 + rand() % 400;
            ember->pos.vy -= rand() % 200;
            ember->pos.vz += -200 + rand() % 400;
            ember->vel.vx = rand() % 80 - 40;
            ember->vel.vy = -(rand() % 50 + 20);
            ember->vel.vz = rand() % 80 - 40;
            ember->scale = rand() % 1536 + 2304;
        }
    }
    frame = burst->frame + 1;
    burst->frame = frame;
    return (frame > FIRE_BURST_LAST_FRAME) * 2;
}

/** @brief The frame a cue hands the animation on to the next entry. */
#define FIRE_CUE_HANDOFF_FRAME 12

/** @brief The frame a cue throws its bursts. */
#define FIRE_CUE_BURST_FRAME 1

/** @brief The frame a cue plays its sound. */
#define FIRE_CUE_SOUND_FRAME 6

/** @brief The frame a cue lands its hit. */
#define FIRE_CUE_HIT_FRAME 8

/** @brief The last frame a cue runs; it ends once it is past this. */
#define FIRE_CUE_LAST_FRAME 21

/** @brief How loud a cue's sound is played. */
#define FIRE_CUE_SOUND_VOLUME 0x100

/** @brief How far the bursts scatter round the target, at most. */
#define FIRE_CUE_SPREAD_MAX 1000

/** @brief A burst thrown within this of the ground is lifted clear of it. */
#define FIRE_BURST_GROUND_GAP 550
#define FIRE_BURST_LIFT 500

/** @brief The smallest a burst is thrown at. */
#define FIRE_BURST_SCALE_MIN 3328

/**
 * @brief Run one flame of the effect: throw a burst at every target of its
 *        animation entry, play its sound, land its hits, and hand the
 *        animation on to the next entry.
 *
 * This is the cue an entry covering more than one target runs; @ref
 * func_801A1340 is the single-target one, and the two differ only in that
 * this one walks the entry's parts.
 *
 * @param flame The flame to run.
 * @return 2 once it has run its last frame.
 */
static s32 func_801A0A70(FireChild *flame) {
    SVECTOR at;
    FireChild *next;
    FireBurst *burst;
    EffectAnim *anims;
    EffectAnimSet *set;
    BattleEffectSlot *slots;
    s32 spread;
    s32 seat;
    s32 part;
    s32 slot;
    s32 i;
    s16 frame;

    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }
    if (flame->age == FIRE_CUE_HANDOFF_FRAME) {
        anims = D_801C2CF4->anims;
        seat = flame->seat + 1;
        if (seat <= anims->unk011) {
            if (anims[seat].unk010 >= 2) {
                next = func_800B2A84(&D_801C4244, func_801A0A70);
                next->frame = 0;
                next->seat = seat;
                next->age = 0;
            } else {
                next = func_800B2A84(&D_801C4244, func_801A1340);
                set = D_801C2CF4;
                next->frame = 0;
                next->seat = seat;
                /* The wrap weights the entry offset's last use: unwrapped,
                 * seat takes s0 and the offset s1. */
                do {
                    next->slot = set->anims[seat].parts->unk000;
                    next->age = 0;
                } while (0);
            }
        }
    }
    flame->age++;
    if (flame->frame == FIRE_CUE_BURST_FRAME) {
        for (part = 0; part < D_801C2CF4->anims[flame->seat].unk010; part++) {
            slot = D_801C2CF4->anims[flame->seat].parts[part].unk000;
            slots = D_800EF2D0;
            spread = slots[slot].unk026 * 175 >> 8;
            if (spread > FIRE_CUE_SPREAD_MAX) {
                spread = FIRE_CUE_SPREAD_MAX;
            }
            func_800C6DEC(&slots[slot], &at);
            for (i = 0; i < 1; i++) {
                burst = func_800B2A84(&D_801C4244, func_801A0434);
                burst->frame = 0;
                burst->delay = i * 2 + rand() % 6;
                burst->pos = at;
                burst->pos.vx += rand() % spread - (spread >> 1);
                burst->pos.vy += rand() % spread - (spread >> 1);
                burst->pos.vz += rand() % spread - (spread >> 1);
                burst->pos.pad = D_800EF2D0[slot].unk024;
                if (burst->pos.pad - FIRE_BURST_GROUND_GAP < burst->pos.vy) {
                    burst->pos.vy += -FIRE_BURST_LIFT - rand() % 300;
                }
                burst->scale = rand() % 1536 + FIRE_BURST_SCALE_MIN;
            }
        }
    }
    if (flame->frame == FIRE_CUE_HIT_FRAME) {
        func_800C00D8(D_801C2CF4->anims[flame->seat].parts,
                      D_801C2CF4->anims[flame->seat].unk010);
    }
    if (flame->frame == FIRE_CUE_SOUND_FRAME) {
        func_800C4764(D_801C29D0, FIRE_CUE_SOUND_VOLUME, 0x80);
    }
    frame = flame->frame + 1;
    flame->frame = frame;
    return (frame > FIRE_CUE_LAST_FRAME) * 2;
}

/**
 * @brief Build every sprite of the frame @p model is on into the OT.
 *
 * Each sprite becomes one packet: the texture page it is drawn from, then
 * the SPRT itself, shaded per sprite and placed off the model's origin. A
 * sprite that would run off the right or bottom of its page is drawn one
 * texel narrower. An additive frame links a second draw mode after each
 * sprite that turns the blend on.
 *
 * @param model The record being posed.
 * @param ot The ordering table the sprites are submitted into.
 * @param z Which entry of @p ot they link into.
 * @param head The prim cursor they are built from.
 * @return The prim cursor left afterwards.
 */
static void *func_801A0F3C(FireModel *model, u32 *ot, s32 z, void *head) {
    FirePrim *prim;
    FirePrim *next;
    DR_TPAGE *extra;
    BattleSprite *sprite;
    s32 i;
    s32 n;
    s32 w;
    s32 h;
    s32 value; /* the edge tests, then the tpage word: one local, else w takes v1 */

    prim = head;
    n = model->spriteCount;
    sprite = (BattleSprite *)model->sprites;
    for (i = 0; i < n; i++, sprite++) {
        setlen(prim, 5);
        setcode(&prim->sprt, (sprite->code & BATTLE_SPRITE_CODE_BLEND) | 0x64);
        prim->sprt.r0 = prim->sprt.g0 = prim->sprt.b0 = sprite->shade;
        *(u32 *)&prim->sprt.u0 = *(u32 *)&sprite->u;
        setXY0(&prim->sprt, model->x + sprite->x, model->y + sprite->y);
        w = sprite->w;
        h = sprite->h;
        value = sprite->u + w - model->uvInset;
        if (value >= FIRE_TPAGE_EDGE) {
            w--;
        }
        value = sprite->v + h - model->uvInset;
        if (value >= FIRE_TPAGE_EDGE) {
            h--;
        }
        setWH(&prim->sprt, w, h);
        value = sprite->flags;
        prim->sprt.clut += model->clutRow[(u16)value >> BATTLE_SPRITE_CLUT_ROW_SHIFT] << 6;
        prim->sprt.tag = FIRE_DRAW_MODE | (value & BATTLE_SPRITE_TPAGE_MASK);
        /* next is taken before the links so the cursor is not an induction
         * variable; the copy then folds into the advance. */
        if (model->flags & BATTLE_SPRITE_FLAG_ADDITIVE) {
            extra = (DR_TPAGE *)&prim[1];
            next = (FirePrim *)&extra[1];
            extra->code[0] = FIRE_DRAW_MODE | getTPage(0, 1, 0, 0);
            addPrim(&ot[z], extra);
            addPrim(&ot[z], prim);
            prim = next;
        } else {
            next = &prim[1];
            addPrim(&ot[z], prim);
            prim = next;
        }
    }
    return prim;
}

/**
 * @brief Resolve the frame @p model is on and hand it to the renderer.
 *
 * Reads the frame's sprite list out of the animation, fills in whatever the
 * caller left for the animation to supply, and draws it at @p at.
 *
 * @param model The record being posed.
 * @param at Where it is drawn.
 * @param ot The ordering table it is submitted into.
 * @param head The prim cursor it is built from.
 * @return The prim cursor left afterwards.
 */
static void *func_801A110C(FireModel *model, SVECTOR *at, u32 *ot, void *head) {
    BattleSpriteAnim *anim;
    u16 *offsets;

    anim = model->anim;
    model->frameCount = anim->frameCount;
    offsets = anim->offsets;
    model->sprites = (u8 *)anim + offsets[model->frame];
    model->spriteCount = *(s32 *)model->sprites;
    model->sprites = model->sprites + 4;
    model->nextOffset = offsets[model->frame + 1];
    if (model->spriteCount < 0) {
        model->spriteCount &= ~BATTLE_SPRITE_COUNT_FLAG;
        model->flags |= BATTLE_SPRITE_FLAG_ADDITIVE;
    }
    if (!(model->flags & BATTLE_SPRITE_FLAG_COLOUR)) {
        model->colour = FIRE_MODEL_SHADE;
    }
    if (!(model->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT0)) {
        model->clutRow[0] = anim->clutRow[0];
    }
    if (!(model->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT1)) {
        model->clutRow[1] = anim->clutRow[1];
    }
    if (!(model->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT2)) {
        model->clutRow[2] = anim->clutRow[2];
    }
    if (!(model->flags & BATTLE_SPRITE_FLAG_KEEP_CLUT3)) {
        model->clutRow[3] = anim->clutRow[3];
    }
    if (model->flags & BATTLE_SPRITE_FLAG_UV_INSET) {
        model->uvInset = 1;
    } else {
        model->uvInset = 0;
    }
    model->x = at->vx;
    model->y = at->vy;
    return func_801A0F3C(model, ot, at->vz, head);
}

/**
 * @brief Draw the flame for this frame and report when its time is up.
 *
 * @param task The effect's frame task.
 * @return 2 once the flame has played out.
 */
static s32 func_801A1284(FireTask *task) {
    SVECTOR at;
    FireModel *model;
    s16 frame;

    model = func_800B3698(sizeof(FireModel));
    model->anim = &D_801C2AAC;
    model->frame = 0;
    model->flags = 0;
    at.vx = FIRE_ORIGIN_X;
    at.vy = FIRE_ORIGIN_Y;
    at.vz = 0;
    D_801D8254 = func_801A110C(model, &at, D_800FA5E8->ot, D_801D8254);
    func_800B36B8(sizeof(FireModel));
    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }
    frame = task->frame + 1;
    task->frame = frame;
    return (frame > FIRE_LAST_FRAME) * 2;
}

/**
 * @brief Run one cue of the flame: throw its bursts at its slot, play its
 *        sound, land its hit, and hand the animation on to the next entry.
 *
 * @param cue The cue to run.
 * @return 2 once it has run its last frame.
 */
static s32 func_801A1340(FireChild *cue) {
    SVECTOR at;
    FireChild *next;
    FireBurst *burst;
    EffectAnim *anims;
    EffectAnimSet *set;
    BattleEffectSlot *slots;
    s32 spread;
    s32 seat;
    s32 i;

    if (D_800EEC5C & (BATTLE_STATE_UNK001 | BATTLE_STATE_UNK200)) {
        return 0;
    }
    if (cue->age == FIRE_CUE_HANDOFF_FRAME) {
        anims = D_801C2CF4->anims;
        seat = cue->seat + 1;
        if (seat <= anims->unk011) {
            if (anims[seat].unk010 >= 2) {
                next = func_800B2A84(&D_801C4244, func_801A0A70);
                next->frame = 0;
                next->seat = seat;
                next->age = 0;
            } else {
                next = func_800B2A84(&D_801C4244, func_801A1340);
                set = D_801C2CF4;
                next->frame = 0;
                next->seat = seat;
                /* The wrap weights the entry offset's last use: unwrapped,
                 * seat takes s0 and the offset s1. */
                do {
                    next->slot = set->anims[seat].parts->unk000;
                    next->age = 0;
                } while (0);
            }
        }
    }
    cue->age++;
    if (D_801C2CD8[cue->slot] == 0) {
        if (cue->frame == 0) {
            return 0;
        }
    } else if (cue->frame == 0) {
        D_801C2CD8[cue->slot] = 0;
    }
    if (cue->frame == FIRE_CUE_BURST_FRAME) {
        slots = D_800EF2D0;
        spread = slots[cue->slot].unk026 * 175 >> 8;
        if (spread > FIRE_CUE_SPREAD_MAX) {
            spread = FIRE_CUE_SPREAD_MAX;
        }
        func_800C6DEC(&slots[cue->slot], &at);
        for (i = 0; i < 4; i++) {
            burst = func_800B2A84(&D_801C4244, func_801A0434);
            burst->frame = 0;
            burst->delay = i * 2 + rand() % 6;
            burst->pos = at;
            burst->pos.vx += rand() % spread - (spread >> 1);
            burst->pos.vy += rand() % spread - (spread >> 1);
            burst->pos.vz += rand() % spread - (spread >> 1);
            burst->pos.pad = D_800EF2D0[cue->slot].unk024;
            if (burst->pos.pad - FIRE_BURST_GROUND_GAP < burst->pos.vy) {
                burst->pos.vy += -FIRE_BURST_LIFT - rand() % 300;
            }
            burst->scale = rand() % 1536 + FIRE_BURST_SCALE_MIN;
        }
    }
    if (cue->frame == FIRE_CUE_HIT_FRAME) {
        func_800BFE1C(D_801C2CF4->anims[cue->seat].parts);
    }
    if (cue->frame == FIRE_CUE_SOUND_FRAME) {
        func_800C6DEC(&D_800EF2D0[cue->slot], &at);
        func_800C4784(D_801C29D0, FIRE_CUE_SOUND_VOLUME, &at);
    }
    cue->frame++;
    if (cue->frame <= FIRE_CUE_LAST_FRAME) {
        return 0;
    }
    D_801C2CD8[cue->slot] = 1;
    return 2;
}

/**
 * @brief Run one frame: point the renderer at this frame's half of the data.
 *
 * @param task The effect's frame task.
 * @return 2 once its children have drained.
 */
static s32 func_801A1810(FireTask *task) {
    s32 done;

    D_801D8254 = (task->frame & FIRE_FRAME_ODD) ? D_801C4254 + FIRE_HALF
                                                : D_801C4254;
    done = (func_800B2B68(&D_801C4244) == 0) * 2;
    task->frame++;
    return done;
}
