#include "common.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libc.h"
#include "battle.h"
#include "btl_anim.h"
#include "ui/icon.h"
#include "ui/window.h"

/** @brief A 15-bit texture page is 64 pixels wide and 256 lines tall. */
#define TPAGE_WIDTH 64
#define TPAGE_HEIGHT 256

/** @brief The parts of a colour word a window keeps: RGB and the semi-transparency bit. */
#define WINDOW_COLOUR_MASK ((SPRT_CODE_ABE << SPRT_CODE_SHIFT) | 0xFFFFFF)

/** @brief Frame tiles, and the frame's corners and edges, are this many pixels across. */
#define WINDOW_FRAME_SIZE 8

/** @brief Texel coordinates of the frame's corner tiles in the icon sheet. */
#define WINDOW_FRAME_U_LEFT 16
#define WINDOW_FRAME_U_RIGHT 32
#define WINDOW_FRAME_V_TOP 0
#define WINDOW_FRAME_V_BOTTOM 16

/** @brief A sprite's u and v as the halfword they share. */
#define WINDOW_FRAME_UV(u, v) ((u) | ((v) << 8))

/** @brief GP0(E2h) texture windows that repeat one frame edge tile along an edge. */
#define WINDOW_TW_LEFT_EDGE 0xE2008BFF
#define WINDOW_TW_RIGHT_EDGE 0xE20093FF
#define WINDOW_TW_TOP_EDGE 0xE2000FFF
#define WINDOW_TW_BOTTOM_EDGE 0xE2010FFF

/** @brief A top or bottom edge sprite is at most this wide; longer edges take two. */
#define WINDOW_EDGE_MAX_W 240

/** @brief Texture windows of the window background's tiles, in pattern order, then a zero word. */
extern u32 D_8005295C[];

/** @brief The window background texture repeats in tiles this many texels wide. */
#define WINDOW_BG_TILE_W 128

/** @brief CLUT row of the window background, the row below the icons'. */
#define WINDOW_BG_CLUT_Y (ICON_CLUT_Y + 1)

extern BattleDisplayEntity g_battleEntities[];
extern DisplayListBuf *D_800834C0;

static void *func_8002BC6C(u32 *ot, s32 idx, void *head);
static u8 *func_8002BE48(u32 *ot, u8 *head);
static u8 getBattleEntitySubField(s32 idx, s32 offset);
static void getBattleEntityBoundRect(s32 idx, RECT *dst);
static void getBattleEntityDispRect(s32 idx, RECT *dst);


/**
 * @brief Get a pointer to a battle entity by index.
 * @param idx Entity index.
 * @return Pointer to the entity.
 */
BattleDisplayEntity *getBattleEntity(s32 idx) {
    return &g_battleEntities[idx];
}

/**
 * @brief Set a battle entity's animation speed, clamped to [3, 11].
 * @param idx Entity index.
 * @param val Value to set; clamped to minimum 3 and maximum 11.
 */
void setBattleEntityAnimSpeed(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    s32 v;
    if (val >= 3) {
        if (val < 12) {
            v = val;
        } else {
            v = 11;
        }
    } else {
        v = 3;
    }
    entity->animSpeed = v;
}


/**
 * @brief Get a battle entity's animation speed.
 * @param idx Entity index.
 * @return Animation speed value for the entity.
 */
s32 getBattleEntityAnimSpeed(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->animSpeed;
}


/**
 * @brief Store a byte value into a battle entity's subFields array.
 * @param idx Entity index.
 * @param offset Index into the subFields array (0 or 1).
 * @param val Byte value to store.
 */
void setBattleEntitySubField(s32 idx, s32 offset, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->subFields[offset] = val;
}


/**
 * @brief Get a byte from a battle entity's subFields array.
 *
 * Dead code — never called anywhere in the binary. The compiler shared the
 * g_battleEntities base address (v0) from the preceding setBattleEntitySubField
 * via cross-function register reuse, producing only 4 instructions. This
 * optimization cannot be reproduced from natural struct access, so pointer
 * math with `register` redeclaration is used to match.
 *
 * Original code:
 * @code
 * u8 getBattleEntitySubField(s32 idx, s32 offset) {
 *     BattleDisplayEntity *entity = &g_battleEntities[idx];
 *     return entity->subFields[offset];
 * }
 * @endcode
 *
 * @param idx Entity index (arrives pre-computed as entity pointer in v0).
 * @param offset Index into the subFields array.
 * @return Byte value at the given subField offset.
 */
static u8 getBattleEntitySubField(s32 idx, s32 offset) {
    register idx;
    return *((u8 *)idx + offset + 0x3A);
}


/**
 * @brief Set a battle entity's bounding rectangle.
 * @param idx Entity index.
 * @param src Source RECT to copy.
 */
void setBattleEntityBoundRect(s32 idx, RECT *src) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->boundRect = *src;
}


/**
 * @brief Set a battle entity's display rectangle with minimum size clamping.
 *
 * Copies src RECT into the entity's dispRect, then ensures the height
 * is at least 1 and the width is at least 2.
 *
 * @param idx Entity index.
 * @param src Source RECT to copy.
 */
void setBattleEntityRectClamp(s32 idx, RECT *src) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    BattleDisplayEntity *ent2;
    ent2 = entity;
    ent2->dispRect = *src;
    if (ent2->dispRect.h <= 0) {
        entity->dispRect.h = 1;
    }
    if (entity->dispRect.w < 2) {
        ent2->dispRect.w = 2;
    }
}


/**
 * @brief Get a battle entity's bounding rectangle.
 * @param idx Entity index.
 * @param dst Destination RECT to copy into.
 */
static void getBattleEntityBoundRect(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->boundRect;
}


/**
 * @brief Get a battle entity's display rectangle.
 * @param idx Entity index.
 * @param dst Destination RECT to copy into.
 */
static void getBattleEntityDispRect(s32 idx, RECT *dst) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    *dst = entity->dispRect;
}


/**
 * @brief Get a battle entity's entity type.
 * @param idx Entity index.
 * @return Entity type value.
 */
s32 getBattleEntityType(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->entityType;
}


/**
 * @brief Set a battle entity's type and compute its draw mode from @ref BATTLE_ENTITY_SEMI_TRANS.
 * @param idx Entity index.
 * @param val Entity type; semi-transparent gives drawMode 0x3A000000, else 0x38000000.
 */
void setBattleEntityType(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    s32 v;
    entity->entityType = val;
    v = 0x38;
    if (val & BATTLE_ENTITY_SEMI_TRANS) {
        v = 0x3A;
    }
    entity->drawMode = v << 24;
}


/**
 * @brief Set a battle entity's update callback.
 * @param idx Entity index.
 * @param val Callback function pointer (or 0 to clear).
 */
void setBattleEntityField00(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->callback = (EntityCallback)val;
}


/**
 * @brief Set a battle entity's render hook.
 * @param idx Entity index.
 * @param val Hook to run, or NULL for none.
 */
void setBattleEntityField04(s32 idx, EntityRenderCallback val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->render = val;
}


/**
 * @brief Set a battle entity's field36.
 * @param idx Entity index.
 * @param val Value to store.
 */
void setBattleEntityField36(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->unk36 = val;
}


/**
 * @brief Get a battle entity's field36.
 * @param idx Entity index.
 * @return Value of field36.
 */
u32 getBattleEntityField36(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->unk36;
}


/**
 * @brief Set a battle entity's field35.
 * @param idx Entity index.
 * @param val Value to store.
 */
void setBattleEntityField35(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->unk35 = val;
}


/**
 * @brief Get a battle entity's field35.
 * @param idx Entity index.
 * @return Value of field35.
 */
u32 getBattleEntityField35(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->unk35;
}


/**
 * @brief Set a battle entity's active flag; if 0, fully deactivate the entity.
 * @param idx Entity index.
 * @param value Active flag; if 0, also clears field36, the render hook and field00.
 */
void setBattleEntityActive(s32 idx, s32 value) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];

    entity->activeFlag = value;
    if (value == 0) {
        setBattleEntityField36(idx, 0);
        setBattleEntityField04(idx, NULL);
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
 * @brief Set a battle entity's brightness.
 * @param idx Entity index.
 * @param val Brightness (0x1000 = full).
 */
void setBattleEntityBrightness(s32 idx, s32 val) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    entity->brightness = val;
}


/**
 * @brief Get a battle entity's brightness.
 * @param idx Entity index.
 * @return Brightness (0x1000 = full).
 */
s32 getBattleEntityBrightness(s32 idx) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    return entity->brightness;
}


/**
 * @brief Initialize a battle entity to default values.
 *
 * Sets up a default bounding rect (64,64,128,128), entity type 6,
 * clears fields, sets anim speed to 3, brightness to 0x1000.
 *
 * @param idx Entity index.
 */
void initBattleEntity(s32 idx) {
    RECT rect;
    s32 i;
    rect.x = 64;
    rect.y = 64;
    rect.w = 128;
    rect.h = 128;
    setBattleEntityBoundRect(idx, &rect);
    setBattleEntityRectClamp(idx, &rect);
    setBattleEntityType(idx, 6);
    setBattleEntityField04(idx, NULL);
    setBattleEntityField00(idx, 0);
    setBattleEntityActive(idx, 0);
    setBattleEntityAnimSpeed(idx, 3);
    for (i = 0; i < 2; i++) {
        setBattleEntitySubField(idx, i, 0);
    }
    setBattleEntitySubField(idx, 1, idx);
    setBattleEntityBrightness(idx, 0x1000);
    setBattleEntityField36(idx, 0);
}


/**
 * @brief Compute the rectangular intersection of two RECTs.
 *
 * Reads each RECT as two packed s32 words (xy at +0, wh at +4), sign-extracts
 * the four halves, then takes max(clip.x, src.x) / max(clip.y, src.y) for the
 * top-left and min(clip.right, src.right) / min(clip.bottom, src.bottom) for
 * the bottom-right. If either dimension collapses to <= 0 the intersection is
 * empty: width or height is forced to 0 and the function returns 0;
 * otherwise returns 1.
 *
 * Per-frame battle window clipping hot-path — the original developer
 * hand-optimized to beat gcc 2.7.2's weak optimizer:
 *  - Batched @c lw loads via @c *(s32 *)&rect rather than per-field @c lh
 *    (saves on load-delay scheduling).
 *  - In-place variable reuse: each packed s32 is progressively transformed
 *    (xy → just X → LEFT → overlapLeft) in the same register, no copies.
 *  - The @c new_var alias on @c sx (and @c result = (new_var = 0)) shifts
 *    gcc's reg allocation to put @c srcX in @c v0 (matching the original).
 *  - Ternary order @c cx MAX before @c cy MAX (despite the source-order
 *    reading more naturally as top-then-left) matches the original asm's
 *    actual emission order.
 *
 * @param overlap Output RECT receiving the clipped rectangle.
 * @param clip    Clipping rectangle (input).
 * @param src     Source rectangle to clip (input).
 * @return 1 if @p clip and @p src overlap (positive area), 0 otherwise.
 */
s32 func_8002B080(RECT *overlap, RECT *clip, RECT *src) {
    s32 result = 1;
    s32 cx = *(s32 *)&clip->x;
    s32 sx = *(s32 *)&src->x;
    int new_var;
    s32 cw = *(s32 *)&clip->w;
    s32 sw = *(s32 *)&src->w;
    s32 cy = cx >> 16;
    s32 sy;
    s32 ch;
    s32 sh;

    cx = cx << 16;
    cx = cx >> 16;

    sy = sx >> 16;
    sx = (sx << 16) >> 16;

    ch = cw >> 16;
    cw <<= 16;
    cw >>= 16;

    sh = sw >> 16;
    sw = sw << 16;
    sw = sw >> 16;

    cw += cx;
    sw += sx;
    ch += cy;
    sh += sy;

    new_var = sx;
    cx = (cx < new_var) ? sx : cx;
    cy = (cy < sy)      ? sy : cy;
    cw = (cw > sw)      ? sw : cw;
    ch = (ch > sh)      ? sh : ch;

    if (!(cw > cx)) {
        cw = cx;
        result = (new_var = 0);
    }
    if (!(cy < ch)) {
        ch = cy;
        result = 0;
    }

    overlap->w = cw - cx;
    overlap->h = ch - cy;
    overlap->x = cx;
    overlap->y = cy;

    return result;
}


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


/**
 * @brief Draw a window's frame around @p rect.
 *
 * Builds the frame from the icon sheet's 8x8 frame tiles: a sprite per corner,
 * and per edge a sprite whose texture window repeats one tile. @p sides picks
 * the sides: WINDOW_FRAME_LEFT draws the left corners and edge,
 * WINDOW_FRAME_RIGHT the right ones. The top and bottom edges are always drawn,
 * between the corners drawn, rounded up to an even width. With both sides, a
 * window under 16 pixels wide or tall gets half-size corners, cut so they keep
 * the tile's outer edge. The colour keeps its RGB and semi-transparency bit.
 *
 * @note Load-bearing spellings: the packets go through one byte cursor; one
 * scratch variable carries the left-side flag and then each corner's or edge's
 * UV word (a variable of its own for the flag moves the register allocation);
 * x and w are sign-extended in place, and w and h then become the right column's
 * x and the bottom row's y. The empty do/while(0) before the corner size is a
 * scheduling barrier: gcc 2.7.2 moves no code across a loop's start or end, and
 * without it the corner size is set before the rectangle is unpacked, unlike
 * the original (it may have been a debug macro compiled out to nothing).
 *
 * @param ot Ordering-table slot the packets are linked into.
 * @param prim First free packet.
 * @param rect Window rectangle.
 * @param color Colour word.
 * @param sides WINDOW_FRAME_LEFT and/or WINDOW_FRAME_RIGHT.
 * @return The first free packet after the ones written.
 */
DR_AREA *func_8002B3A0(void *ot, DR_AREA *prim, RECT *rect, s32 color, s32 sides) {
    DR_TWIN *twin;
    TSPRT *corner;
    ModeSprt *p;
    u32 drawMode;
    u32 link;
    s32 x;
    s32 y;
    s32 w;
    s32 h;
    s32 cornerW;
    s32 cornerH;
    u32 uvAdj;
    u32 tmp;
    u32 uAdj;
    s32 edgeX;
    s32 edgeY;
    s32 edgeW;
    s32 edgeH;
    u8 *pkt;

    pkt = (u8 *)prim;
    drawMode = _get_mode(1, 0, getTPage(0, 0, ICON_TPAGE_X, ICON_TPAGE_Y));
    x = *(s32 *)&rect->x;
    w = *(s32 *)&rect->w;
    getAddrNewFast(ot, link);
    y = x >> 16;
    x <<= 16;
    x >>= 16;
    h = w >> 16;
    w <<= 16;
    w >>= 16;
    do {
    } while (0);
    cornerW = WINDOW_FRAME_SIZE;
    cornerH = cornerW;
    uvAdj = 0; /* how far a half-size corner shifts into its tile, packed like a UV */
    if (sides == (WINDOW_FRAME_LEFT | WINDOW_FRAME_RIGHT)) {
        if (w < 2 * WINDOW_FRAME_SIZE) {
            cornerW = w / 2;
            uvAdj = WINDOW_FRAME_SIZE - cornerW;
        }
        if (h < 2 * WINDOW_FRAME_SIZE) {
            cornerH = h / 2;
            uvAdj |= WINDOW_FRAME_UV(0, WINDOW_FRAME_SIZE - cornerH);
        }
    }
    edgeW = w;
    edgeH = h;
    edgeX = x;
    tmp = sides & WINDOW_FRAME_LEFT;
    edgeY = y;
    if (tmp) {
        edgeW -= cornerW;
        edgeH -= cornerH;
        edgeX += cornerW;
        edgeY += cornerH;
    }
    if (sides & WINDOW_FRAME_RIGHT) {
        edgeW -= cornerW;
        edgeH -= cornerH;
    }
    edgeW = (edgeW + 1) / 2 * 2;
    w -= cornerW;
    h -= cornerH;
    w += x;
    h += y;
    color &= WINDOW_COLOUR_MASK;
    color |= SPRT_CODE;
    twin = (DR_TWIN *)pkt;
    setlen(twin, 2);
    twin->code[0] = TEXWINDOW_OFF;
    twin->code[1] = 0;
    link = linkPacket(link, twin);
    pkt += sizeof(DR_TWIN);
    if (tmp) {
        corner = (TSPRT *)pkt;
        *(u32 *)&corner->r0 = color;
        setlen(corner, 5);
        corner->drawMode = drawMode;
        corner->x0 = x;
        corner->y0 = y;
        corner->w = cornerW;
        corner->h = cornerH;
        corner->clut = getClut(ICON_CLUT_X, ICON_CLUT_Y);
        *(u16 *)&corner->u0 = WINDOW_FRAME_UV(WINDOW_FRAME_U_LEFT, WINDOW_FRAME_V_TOP);
        link = linkPacket(link, corner);
        pkt += sizeof(TSPRT);
        p = (ModeSprt *)pkt;
        *(u32 *)&p->r0 = color;
        setlen(p, 7);
        p->drawMode = drawMode;
        p->texWindow[0] = TEXWINDOW_OFF;
        p->texWindow[1] = 0;
        p->x0 = x;
        p->y0 = h;
        p->w = cornerW;
        p->h = cornerH;
        p->clut = getClut(ICON_CLUT_X, ICON_CLUT_Y);
        tmp = (uvAdj & 0xFF00) + WINDOW_FRAME_UV(WINDOW_FRAME_U_LEFT, WINDOW_FRAME_V_BOTTOM);
        *(u16 *)&p->u0 = tmp;
        link = linkPacket(link, p);
        pkt += sizeof(ModeSprt);
        twin = (DR_TWIN *)pkt;
        setlen(twin, 2);
        twin->code[0] = TEXWINDOW_OFF;
        twin->code[1] = 0;
        link = linkPacket(link, twin);
        pkt += sizeof(DR_TWIN);
        if (edgeH > 0) {
            p = (ModeSprt *)pkt;
            *(u32 *)&p->r0 = color;
            setlen(p, 7);
            p->drawMode = drawMode;
            p->texWindow[0] = WINDOW_TW_LEFT_EDGE;
            p->texWindow[1] = 0;
            p->x0 = x;
            p->y0 = edgeY;
            p->w = WINDOW_FRAME_SIZE;
            p->h = edgeH;
            *(u32 *)&p->u0 = getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16;
            link = linkPacket(link, p);
            pkt += sizeof(ModeSprt);
        }
    }
    if (sides & WINDOW_FRAME_RIGHT) {
        uAdj = uvAdj & 0xFF;
        corner = (TSPRT *)pkt;
        *(u32 *)&corner->r0 = color;
        setlen(corner, 5);
        corner->drawMode = drawMode;
        corner->x0 = w;
        corner->y0 = y;
        corner->w = cornerW;
        corner->h = cornerH;
        corner->clut = getClut(ICON_CLUT_X, ICON_CLUT_Y);
        tmp = uAdj + WINDOW_FRAME_UV(WINDOW_FRAME_U_RIGHT, WINDOW_FRAME_V_TOP);
        *(u16 *)&corner->u0 = tmp;
        link = linkPacket(link, corner);
        pkt += sizeof(TSPRT);
        p = (ModeSprt *)pkt;
        *(u32 *)&p->r0 = color;
        setlen(p, 7);
        p->drawMode = drawMode;
        p->texWindow[0] = TEXWINDOW_OFF;
        p->texWindow[1] = 0;
        p->x0 = w;
        p->y0 = h;
        p->w = cornerW;
        p->h = cornerH;
        p->clut = getClut(ICON_CLUT_X, ICON_CLUT_Y);
        /* uvAdj has only 16 bits, so the mask changes no value, but the original has it */
        tmp = (uvAdj & 0xFFFF) + WINDOW_FRAME_UV(WINDOW_FRAME_U_RIGHT, WINDOW_FRAME_V_BOTTOM);
        *(u16 *)&p->u0 = tmp;
        link = linkPacket(link, p);
        pkt += sizeof(ModeSprt);
        twin = (DR_TWIN *)pkt;
        setlen(twin, 2);
        twin->code[0] = TEXWINDOW_OFF;
        twin->code[1] = 0;
        link = linkPacket(link, twin);
        pkt += sizeof(DR_TWIN);
        if (edgeH > 0) {
            p = (ModeSprt *)pkt;
            *(u32 *)&p->r0 = color;
            setlen(p, 7);
            p->drawMode = drawMode;
            p->texWindow[0] = WINDOW_TW_RIGHT_EDGE;
            p->texWindow[1] = 0;
            p->x0 = w;
            p->y0 = edgeY;
            p->w = WINDOW_FRAME_SIZE;
            p->h = edgeH;
            tmp = getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16;
            tmp = uAdj + tmp;
            *(u32 *)&p->u0 = tmp;
            link = linkPacket(link, p);
            pkt += sizeof(ModeSprt);
        }
    }
    if (edgeW > 0) {
        if (edgeW > WINDOW_EDGE_MAX_W) {
            p = (ModeSprt *)pkt;
            *(u32 *)&p->r0 = color;
            setlen(p, 7);
            p->drawMode = drawMode;
            p->texWindow[0] = WINDOW_TW_TOP_EDGE;
            p->texWindow[1] = 0;
            p->x0 = edgeX;
            p->y0 = y;
            p->w = WINDOW_EDGE_MAX_W;
            p->h = WINDOW_FRAME_SIZE;
            tmp = getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16;
            *(u32 *)&p->u0 = tmp;
            link = linkPacket(link, p);
            pkt += sizeof(ModeSprt);
            p = (ModeSprt *)pkt;
            *(u32 *)&p->r0 = color;
            setlen(p, 7);
            p->drawMode = drawMode;
            p->texWindow[0] = WINDOW_TW_BOTTOM_EDGE;
            p->texWindow[1] = 0;
            p->x0 = edgeX;
            p->y0 = h;
            p->w = WINDOW_EDGE_MAX_W;
            p->h = WINDOW_FRAME_SIZE;
            tmp += uvAdj & 0xFF00;
            *(u32 *)&p->u0 = tmp;
            link = linkPacket(link, p);
            pkt += sizeof(ModeSprt);
            edgeW -= WINDOW_EDGE_MAX_W;
            edgeX += WINDOW_EDGE_MAX_W;
        }
        p = (ModeSprt *)pkt;
        *(u32 *)&p->r0 = color;
        setlen(p, 7);
        p->drawMode = drawMode;
        p->texWindow[0] = WINDOW_TW_TOP_EDGE;
        p->texWindow[1] = 0;
        p->x0 = edgeX;
        p->y0 = y;
        p->w = edgeW;
        p->h = WINDOW_FRAME_SIZE;
        *(u32 *)&p->u0 = getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16;
        link = linkPacket(link, p);
        pkt += sizeof(ModeSprt);
        p = (ModeSprt *)pkt;
        *(u32 *)&p->r0 = color;
        setlen(p, 7);
        p->drawMode = drawMode;
        p->texWindow[0] = WINDOW_TW_BOTTOM_EDGE;
        p->texWindow[1] = 0;
        p->x0 = edgeX;
        p->y0 = h;
        p->w = edgeW;
        p->h = WINDOW_FRAME_SIZE;
        tmp = getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16;
        tmp += uvAdj & 0xFF00;
        *(u32 *)&p->u0 = tmp;
        link = linkPacket(link, p);
        pkt += sizeof(ModeSprt);
    }
    setAddrFast(ot, link);
    return (DR_AREA *)pkt;
}

/**
 * @brief Draw a window's whole frame: func_8002B3A0 with both sides.
 *
 * @param ot Ordering-table slot the packets are linked into.
 * @param prim First free packet.
 * @param rect Window rectangle.
 * @param color Colour word.
 * @return The first free packet after the ones written.
 */
DR_AREA *func_8002B898(void *ot, DR_AREA *prim, RECT *rect, s32 color) {
    return func_8002B3A0(ot, prim, rect, color, WINDOW_FRAME_LEFT | WINDOW_FRAME_RIGHT);
}

/**
 * @brief Draw a window's background texture over @p rect.
 *
 * Resets the texture window, then covers @p rect in strips at most one tile
 * (128 texels) wide. Each strip is a sprite from the icon sheet's texture page
 * that carries its own draw mode and its tile's texture window from
 * @c D_8005295C, so the pattern repeats across the window. @p bgOffset shifts
 * the pattern: its low 16 bits are the x offset in texels, and a first strip
 * then covers the rest of that tile; bits 16-19 are the v coordinate. The
 * colour keeps its RGB and semi-transparency bit, so a semi-transparent colour
 * draws a see-through background.
 *
 * @note Load-bearing spellings: @c v is built one operation per statement, and
 * @p bgOffset is reused in place for the offset and the first strip's width.
 * Each word of @p rect is read inside a do/while(0): gcc 2.7.2 moves no code
 * across a loop's start or end, which keeps the reads in source order with
 * their load delays unfilled, as in the original.
 *
 * @param ot Ordering-table slot the packets are linked into.
 * @param prim First free packet.
 * @param rect Window rectangle; its width is rounded down to even.
 * @param color Colour word.
 * @param bgOffset Pattern offset: x in bits 0-15, v in bits 16-19.
 * @return The first free packet after the ones written.
 */
DR_AREA *func_8002B8BC(void *ot, DR_AREA *prim, RECT *rect, s32 color, s32 bgOffset) {
    DR_TWIN *twin;
    ModeSprt *p;
    u32 *tw;
    u32 mode;
    u32 link;
    u32 x;
    u32 y;
    u32 w;
    u32 h;
    u32 v;

    mode = _get_mode(1, 0, getTPage(0, 0, ICON_TPAGE_X, ICON_TPAGE_Y));
    getAddrNewFast(ot, link);
    tw = D_8005295C;
    v = bgOffset;
    v >>= 16;
    v &= 0xF;
    bgOffset &= 0xFFFF;
    tw += bgOffset / WINDOW_BG_TILE_W;
    bgOffset %= WINDOW_BG_TILE_W;
    /* setTexWindow cannot write this: it builds the word from a RECT, a NULL
     * one gives a no-op word, and a zeroed one needs a stack slot the
     * original does not have. */
    twin = (DR_TWIN *)prim;
    setlen(twin, 2);
    twin->code[0] = TEXWINDOW_OFF;
    twin->code[1] = 0;
    link = linkPacket(link, twin);
    p = (ModeSprt *)(twin + 1);
    do {
        x = *(u32 *)&rect->x;
    } while (0);
    y = x >> 16;
    x &= 0xFFFF;
    do {
        w = *(u32 *)&rect->w;
    } while (0);
    h = w >> 16;
    w &= 0xFFFE;
    color &= WINDOW_COLOUR_MASK;
    color |= SPRT_CODE;
    if (bgOffset != 0) {
        p->texWindow[0] = *tw++;
        p->u0 = bgOffset;
        bgOffset = WINDOW_BG_TILE_W - bgOffset;
        setlen(p, 7);
        *(u32 *)&p->r0 = color;
        p->texWindow[1] = 0;
        p->drawMode = mode;
        p->v0 = v;
        p->clut = getClut(ICON_CLUT_X, WINDOW_BG_CLUT_Y);
        bgOffset = (bgOffset < 0) ? 0 : (bgOffset > w) ? w : bgOffset;
        p->x0 = x;
        p->y0 = y;
        p->w = bgOffset;
        p->h = h;
        link = linkPacket(link, p);
        p++;
        w -= bgOffset;
        x += bgOffset;
    }
    if (w != 0) {
        while (1) {
            p->texWindow[0] = *tw++;
            *(u32 *)&p->r0 = color;
            setlen(p, 7);
            p->texWindow[1] = 0;
            p->drawMode = mode;
            p->x0 = x;
            p->y0 = y;
            p->h = h;
            *(u32 *)&p->u0 = getClut(ICON_CLUT_X, WINDOW_BG_CLUT_Y) << 16;
            p->v0 = v;
            link = linkPacket(link, p);
            if (w > WINDOW_BG_TILE_W) {
                p->w = WINDOW_BG_TILE_W;
                p++;
                w -= WINDOW_BG_TILE_W;
                x += WINDOW_BG_TILE_W;
            } else {
                p->w = w;
                p++;
                break;
            }
        }
    }
    setAddrFast(ot, link);
    return (DR_AREA *)p;
}

/**
 * @brief Draw a window's background: func_8002B8BC with a bgOffset of 0.
 *
 * A semi-transparent colour word draws a see-through box.
 *
 * @param ot Ordering table.
 * @param prim Primitive buffer cursor.
 * @param rect The window rect.
 * @param color Colour word.
 * @return The primitive cursor after the packets.
 */
DR_AREA *drawWindowBackground(void *ot, DR_AREA *prim, RECT *rect, s32 color) {
    return func_8002B8BC(ot, prim, rect, color, 0);
}


/**
 * @brief Draw a block of the active draw area again at @p rect.
 *
 * The sprites' texture is VRAM itself: 15-bit texture pages starting at the
 * active draw environment's clip origin, so @p rect shows the block of that
 * size from the draw area's top-left corner. @p rect is covered in strips one
 * texture page wide, each a ModeSprt whose draw mode selects the next page.
 *
 * @param ot OT entry to link into.
 * @param p Packet cursor.
 * @param rect Screen rect to cover.
 * @param color Colour word of the sprites; its semi-transparency bit is cleared.
 * @return The packet cursor after the sprites.
 */
ModeSprt *func_8002BAA0(u32 *ot, ModeSprt *p, RECT *rect, u32 color) {
    RECT clip;
    u32 mode;
    u32 texWindow;
    u32 uv;
    u32 link;
    s32 ty;
    s32 x;
    s32 y;
    s32 n;
    s32 w;
    s32 h;

    copyDisplayRect(&clip);
    color &= ~(SPRT_CODE_ABE << SPRT_CODE_SHIFT);
    /* x and y hold the clip origin, then the rect's: one pair keeps the original's registers */
    y = clip.y;
    x = clip.x;
    ty = y / TPAGE_HEIGHT * TPAGE_HEIGHT;
    mode = _get_mode(1, 0, getTPage(2, 0, x / TPAGE_WIDTH * TPAGE_WIDTH, ty));
    texWindow = TEXWINDOW_OFF; /* a variable: the original loads it before the OT read */
    uv = (x % TPAGE_WIDTH) | ((y - ty) << 8);
    getAddrNewFast(ot, link);
    x = rect->x;
    y = rect->y;
    n = rect->w;
    h = rect->h;
    for (; n > 0; n -= TPAGE_WIDTH) {
        w = n;
        if (w > TPAGE_WIDTH) {
            w = TPAGE_WIDTH;
        }
        setlen(p, 7);
        p->drawMode = mode;
        p->texWindow[0] = texWindow;
        p->texWindow[1] = 0;
        p->code = SPRT_CODE >> SPRT_CODE_SHIFT;
        *(u32 *)&p->r0 = color; /* r, g, b and code in one store, over the code just set */
        setXY0(p, x, y);
        setWH(p, w, h);
        *(u32 *)&p->u0 = uv; /* u, v and CLUT in one store */
        link = linkPacket(link, p);
        p++;
        mode++;
        x += TPAGE_WIDTH;
    }
    setAddrFast(ot, link);
    return p;
}


/**
 * @brief Draw a window box: its whole frame (func_8002B3A0 with both sides),
 * then its background (drawWindowBackground).
 * @param ot Ordering table.
 * @param prim Primitive buffer cursor.
 * @param rect The window rect.
 * @param color Colour word.
 * @return The primitive cursor after the packets.
 */
DR_AREA *func_8002BC10(P_TAG *ot, DR_AREA *prim, RECT *rect, s32 color)
{
    DR_AREA *p;

    p = func_8002B3A0(ot, prim, rect, color, WINDOW_FRAME_LEFT | WINDOW_FRAME_RIGHT);

    return drawWindowBackground(ot, p, rect, color);
}


/**
 * @brief Draw battle entity @p idx into the OT entry of its anim speed.
 *
 * Nothing is drawn while its bound rect is empty. Otherwise its render hook
 * runs, then a draw area and offset for its clamped rect (@c clipClamp) are
 * linked. An entity with a box that is not a dialog (whose hook draws its own)
 * also gets the box's frame and background, grey at its @c brightness with the
 * code byte of its @c drawMode, and a draw area and offset for its bound rect
 * (@c clipBound).
 *
 * @param ot Ordering table.
 * @param idx Entity index.
 * @param head Display-list write head.
 * @return The display-list head after the entity's packets.
 */
static void *func_8002BC6C(u32 *ot, s32 idx, void *head) {
    BattleDisplayEntity *e;
    EntityRenderCallback render;
    DR_AREA *p;
    RECT r;
    RECT *rect;
    u32 link;
    u32 size;
    u32 w;
    u32 h;
    u32 colour;
    u32 grey;
    s32 speed;

    speed = getBattleEntityAnimSpeed(idx);
    e = &g_battleEntities[idx];
    p = head;
    size = *(u32 *)&e->boundRect.w; /* w and h in one load, as the original reads them */
    render = e->render;
    ot = &ot[speed];
    h = size >> 16;
    w = size & 0xFFFF;
    if (w == 0 || h == 0) {
        return p;
    }
    if (render != NULL) {
        p = render(ot, e, p);
    }
    getAddrNewFast(ot, link);
    SetDrawArea(p, &e->clipClamp.rect);
    link = linkPacket(link, p);
    p++;
    SetDrawOffset((DR_OFFSET *)p, (u16 *)&e->clipClamp.savedPos); /* savedPos packs x and y */
    link = linkPacket(link, p);
    p++;
    if ((e->entityType & BATTLE_ENTITY_BOX) && !(e->entityType & BATTLE_ENTITY_DIALOG)) {
        colour = e->brightness;
        grey = colour >> 5;
        colour = grey | ((grey << 16) | (grey << 8));
        setAddrFast(ot, link);
        r.x = 0;
        r.y = 0;
        r.w = e->boundRect.w;
        r.h = e->boundRect.h;
        rect = &r; /* the original keeps &r in a register across both calls */
        colour |= e->drawMode & COLOUR_WORD_CODE;
        p = func_8002B3A0(ot, p, rect, colour, WINDOW_FRAME_LEFT | WINDOW_FRAME_RIGHT);
        p = drawWindowBackground(ot, p, rect, colour);
        getAddrNewFast(ot, link);
        SetDrawArea(p, &e->clipBound.rect);
        link = linkPacket(link, p);
        p++;
        SetDrawOffset((DR_OFFSET *)p, (u16 *)&e->clipBound.savedPos);
        link = linkPacket(link, p);
        p++;
    }
    setAddrFast(ot, link);
    return p;
}


/**
 * @brief Reset the draw area and draw offset to the active draw environment's.
 *
 * Emits a DR_AREA for the environment's clip rect and a DR_OFFSET for its draw
 * offset, and links both into @p ot.
 *
 * @param ot OT entry to link into.
 * @param head Display-list write head.
 * @return The display-list head after the two packets.
 */
static u8 *func_8002BE48(u32 *ot, u8 *head) {
    RECT clip;
    u16 ofs[2];
    u32 link;
    u32 tag;

    getAddrFast(ot, link);
    copyDisplayRect(&clip);
    copyDisplayCoords(ofs);
    SetDrawArea((DR_AREA *)head, &clip);
    link = linkPacket(link, head);
    head += sizeof(DR_AREA);
    SetDrawOffset((DR_OFFSET *)head, ofs);
    tag = linkPacket(link, head);
    head += sizeof(DR_OFFSET);
    setAddrFast(ot, tag);
    return head;
}


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


/**
 * @brief Build the per-frame battle entity display list.
 *
 * For each of the 8 entity slots, runs the visibility test (activeFlag set,
 * field36 clear, field35 set) and clips the entity rect via @c clipBlitRects.
 * Each entity that passes and has a non-empty clip result sets its bit in
 * @p mask. Then @c func_8002BE48 sets up the draw area at @c ot[15], and for
 * every set bit @c func_8002BC6C is called to render that entity into the
 * display list at @p head.
 *
 * @param ot Ordering table.
 * @param head Display-list write head; advanced through nested calls.
 * @return The display-list head after all entity primitives are emitted.
 */
u8 *func_8002BF24(u32 *ot, u8 *head) {
    s32 mask = 0;
    s32 hit;
    s32 i;
    s32 bit;
    s32 slotBit;

    i = 0;
    bit = 1;
    for (; i < 8; i++) {
        BattleDisplayEntity *e = &g_battleEntities[i];
        hit = 0;
        if (e->activeFlag != 0 && e->unk36 == 0 && e->unk35 != 0) {
            hit = (clipBlitRects((BlitParams *)e) != 0);
        }
        if (hit) {
            mask |= bit << i;
        }
    }

    head = func_8002BE48(&ot[15], head);
    if (mask == 0) {
        return head;
    }

    for (i = 0; i < 8; i++) {
        slotBit = 1;
        if (mask & (slotBit << i)) {
            head = func_8002BC6C(ot, i, head);
        }
    }
    return head;
}


/**
 * @brief Dispatch a battle entity's update callback.
 *
 * If the entity has a callback set, invokes it with the entity pointer and this
 * frame's pad input.
 *
 * @param idx Entity index.
 * @param input Buttons pressed this frame, passed on to the callback.
 * @param repeat Buttons auto-repeating this frame, passed on to the callback.
 */
void dispatchBattleEntity(s32 idx, u32 input, u32 repeat) {
    BattleDisplayEntity *entity = &g_battleEntities[idx];
    if (entity->callback) {
        entity->callback(entity, input, repeat);
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
 * position fields, and brightness to default values.
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
void SetNameDictionary(s32 val) {
    g_nameDictionary = val;
}


/** @brief Return the maximum number of battle entity slots (always 8). */
s32 getMaxBattleEntities(void) {
    return 8;
}
