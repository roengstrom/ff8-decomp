#include "battle_results/draw.h"
#include "common.h"
#include "input/gamepad.h"
#include "main.h"
#include "menu.h"
#include "menu_tint.h"
#include "menumain.h"
#include "menumain2.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgte.h"
#include "ui/icon.h"
#include "ui/sprite.h"
#include "ui/window.h"

#define setRGBCWord(p, c) (*(u32 *)&(p)->r0 = (c)) // NOTE: duplicated from sprite.c
#define setUVClutWord(p, uv) (*(u32 *)&(p)->u0 = (uv))
#define setWHWord(p, wh) (*(u32 *)&(p)->w = (wh))

#define setIconCellTexInfo(p, texInfo, color) do { \
    u32 bits_; \
    s32 tpage_; \
    bits_ = (texInfo) & ICON_UVCLUT_MASK; \
    bits_ += getClut(ICON_CLUT_X, ICON_CLUT_Y) << 16; \
    setUVClutWord(p, bits_); \
    bits_ = ((texInfo) >> ICON_ABR_SHIFT) & ICON_ABR_MASK; \
    bits_ = (u8)getTPage(0, bits_, 0, 0); \
    tpage_ = bits_ | getTPage(0, 0, ICON_TPAGE_X, ICON_TPAGE_Y); \
    bits_ = (texInfo) >> ICON_ABE_SHIFT; \
    bits_ &= SPRT_CODE_ABE; \
    bits_ <<= SPRT_CODE_SHIFT; \
    bits_ |= (color); \
    setTSprt(p, 1, 0, tpage_); \
    setRGBCWord(p, bits_); \
} while (0)

#define setIconCellMetrics(p, metrics, x, y) do { \
    u32 bits_; \
    bits_ = (metrics) & ICON_WH_MASK; \
    setWHWord(p, bits_); \
    bits_ = (s8)((metrics) >> 24); \
    (metrics) <<= 16; \
    (metrics) = (s8)((metrics) >> 24); \
    setXY0(p, (x) + (metrics), (y) + bits_); \
} while (0)

extern u8 D_801FA278;
extern u8 D_801FA279;
extern u8 D_801FA27A;
extern u8 D_801FA27B;

static inline void *DrawIconFromTable(void *ot, TSPRT *head, IconTable *table, s32 idx, s32 x, s32 y, u32 color) {
    void *cellAddr = table;
    u32 tableWord;
    s32 cellCount;

    tableWord = table->descriptors[idx];
    cellCount = tableWord >> 16;
    tableWord &= 0xFFFF;
    cellAddr += tableWord;
    while (cellCount > 0) {
        tableWord = ((IconCell *)cellAddr)->texInfo;
        setIconCellTexInfo(head, tableWord, color);
        tableWord = ((IconCell *)cellAddr)->metrics;
        setIconCellMetrics(head, tableWord, x, y);
        addPrimFast(ot, head);
        head++;
        cellCount--;
        cellAddr = (IconCell *)cellAddr + 1;
    }
    return head;
}

/* ======================================================================== */
/* Panel/Window Rendering                                                   */
/* ======================================================================== */

/**
 * @brief Build a draw-area primitive from a caller rectangle and link it.
 *
 * Copies the RECT at @p src, offsets its origin by the
 * active draw environment's clip origin, clamps width/height to at least 2,
 * packs the @c SetDrawArea GP0 command into @p prim, links it via
 * @c addPrimFast, and returns the next packet slot.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @param src  Source RECT.
 * @return Cursor for the next primitive (@c prim + 1).
 */
DR_AREA *func_801EF800(P_TAG *ot, DR_AREA *prim, RECT *src) {
    RECT r;
    DRAWENV *env;

    r = *src;
    env = g_activeDrawEnv;
    r.x += env->clip.x;
    r.y += env->clip.y;
    if (r.w < 2) {
        r.w = 2;
    }
    if (r.h < 2) {
        r.h = 2;
    }
    SetDrawArea(prim, &r);
    addPrimFast(ot, prim);
    return ++prim;
}

/**
 * @brief Build a draw-area primitive for the active draw environment's clip
 *        and link it.
 *
 * Same as func_801F03E8 but keeps the draw environment's own clip rectangle
 * (no size override): packs the @c SetDrawArea GP0 command for
 * @c g_activeDrawEnv->clip into @p prim, links it via @c addPrimFast,
 * and returns the next packet slot.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @return Cursor for the next primitive (@c prim + 1).
 */
DR_AREA *func_801EF8D8(P_TAG *ot, DR_AREA *prim) {
    SetDrawArea(prim, &g_activeDrawEnv->clip);
    addPrimFast(ot, prim);
    return ++prim;
}

/**
 * @brief Draw inner panel with 1px inset border.
 *
 * Shrinks the rect by 1 pixel on each side, draws the panel via
 * func_801EF800, then restores the original rect dimensions.
 */
void *func_801EF934(void *ot, void *prim, RECT *rect) {
    s32 save0 = ((s32 *)rect)[0];
    s32 save1 = ((s32 *)rect)[1];
    rect->x += 1;
    rect->y += 1;
    rect->w -= 2;
    rect->h -= 2;
    prim = func_801EF800(ot, prim, rect);
    ((s32 *)rect)[0] = save0;
    ((s32 *)rect)[1] = save1;
    return prim;
}

/**
 * @brief Draws a menu window.
 *
 * Draws the window background, border, and optional corner icon.
 * When progress is below 1.0, draw a proportional portion of the
 * window.
 *
 * Called continuously while the menu is open.
 *
 * @param ot Ordering table to which the primitives are added.
 * @param prim Pointer to the next GPU primitive to allocate.
 * @param progress Window expansion progress as fixed point
 *                 division where 4096 (int) = 1.0 (float).
 * @param color Color used to draw the window.
 * @return Pointer to the next available GPU primitive.
 */
void *DrawMenuWindow(void *ot, void *prim, s32 progress, u32 color) {
    IconTable *iconTable = &g_iconTable;
    MenuDisplayConfig *window = &g_menuDisplayCfg;
    progress = abs(progress);
    if (progress != 0) {
        scaleWindowRect(progress, &window->x, &window->x2);
        if (progress < ONE) {
            prim = func_801EF934(ot, prim, &window->x2);
        }
        if (window->iconType != 0) {
            prim = DrawIconFromTable(ot, prim, iconTable, window->iconType, window->x, window->y, color);
        }
        prim = func_8002B898(ot, prim, &window->x2, color);
        prim = func_8002B8BC(ot, prim, &window->x, color, 8);
        if (progress < ONE) {
            prim = func_801EF800(ot, prim, &window->x2);
        }
    }
    return prim;
}

/**
 * @brief Renders the rows of a menu list within a window frame.
 *
 * Delegates row rendering to the callback provided as a parameter, invoking
 * it for each row of the list.
 *
 * When a page transition is in progress, renders the horizontal slide
 * animation between the previous and next pages in response to left or right
 * directional input.
 *
 * Called continuously while a menu displaying a paged list is active, as part
 * of the menu rendering loop.
 *
 * @param ot Ordering table used to sort GPU primitives for rendering.
 * @param _prim Pointer to the next available GPU primitive.
 * @param _drawRowCallback Callback function used to render each row of the list.
 * @return Pointer to the next available GPU primitive after rendering.
 */
void *func_801EFBB4(void *ot, void *_prim, MenuRowCallback _drawRowCallback) {
    void *prim = _prim;
    MenuRowCallback drawRowCallback = _drawRowCallback;
    s32 iconType;
    s32 remainingWidth;
    s32 transitionWidth;
    s32 transitionProgress;
    s32 xMin;
    s32 xMax;
    s32 i;
    MenuDisplayConfig *cfg;

    cfg = &g_menuDisplayCfg;
    cfg->x2 = cfg->x;
    cfg->y2 = cfg->y;
    cfg->w2 = cfg->w;
    cfg->h2 = cfg->h;
    transitionWidth = 0;
    remainingWidth = 0;
    transitionProgress = cfg->scrollOffset;

    iconType = cfg->iconType;
    if (iconType != 0) {
        prim = drawIcon(ot, prim, iconType, cfg->x, cfg->y, g_menuTint[MENU_TINT_NORMAL]);
    }

    prim = func_801EF8D8(ot, prim);

    if (transitionProgress != 0) {
        s32 easedProgress = D_801FA3C8[abs(transitionProgress) / 64];

        if (transitionProgress < 0) {
            transitionProgress = -easedProgress;
            transitionWidth = (cfg->w * transitionProgress) / 4096;
            xMax = cfg->x + abs(transitionWidth);
            xMin = cfg->x + cfg->w - abs(transitionWidth);
        } else {
            transitionProgress = easedProgress;
            transitionWidth = (cfg->w * transitionProgress) / 4096;
            xMax = cfg->x + transitionWidth;
            xMin = cfg->x;
        }

        func_801F0FD0(xMin - 12, xMax);

        if (drawRowCallback != NULL) {
            for (i = 0; i < cfg->columnCount; i++) {
                prim = drawRowCallback(ot, prim, cfg->pageEnd, i, 0);
            }
        }

        if (transitionProgress < 0) {
            remainingWidth = abs(transitionWidth);

            cfg->w2 = remainingWidth;
            remainingWidth = cfg->w - remainingWidth;
            cfg->x2 = cfg->x + remainingWidth;

            prim = func_801EF800(ot, prim, &cfg->x2);

            if (remainingWidth >= 9) {
                cfg->w2 = 8;
                cfg->x2 = cfg->x + remainingWidth - 8;
                prim = func_8002B3A0(ot, prim, &cfg->x2, g_menuTint[MENU_TINT_NORMAL], WINDOW_FRAME_RIGHT);
            }

            cfg->x2 = cfg->x + 8;
            cfg->w2 = remainingWidth - 8;
        } else {
            cfg->x2 = cfg->x;
            remainingWidth = cfg->w - transitionWidth;
            cfg->w2 = transitionWidth;
            
            prim = func_801EF800(ot, prim, &cfg->x2);

            if (remainingWidth >= 9) {
                cfg->w2 = 8;
                cfg->x2 = cfg->x + transitionWidth;
                prim = func_8002B3A0(ot, prim, &cfg->x2, g_menuTint[MENU_TINT_NORMAL], WINDOW_FRAME_LEFT);
            }

            cfg->x2 = cfg->x + transitionWidth;
            cfg->w2 = cfg->w - transitionWidth - 8;
        }
    }

    if (cfg->w2 > 0) {
        if (drawRowCallback != NULL) {
            func_801F0FD0(cfg->x2 - 12, cfg->x2 + cfg->w2);
            for (i = 0; i < cfg->columnCount; i++) {
                prim = drawRowCallback(ot, prim, cfg->pageStart, i, transitionWidth);
            }
        }
        if (remainingWidth != 0) {
            prim = func_801EF800(ot, prim, &cfg->x2);
        }
    }

    cfg->iconType = 0;
    prim = DrawMenuWindow(ot, prim, ONE, g_menuTint[MENU_TINT_NORMAL]);
    cfg->iconType = iconType;
    func_801F0FD0(0, 0);
    return prim;
}

/**
 * @brief Build the 65-entry falloff table in D_801FA3C8.
 *
 * Fills D_801FA3C8[0..64] back-to-front with a decay curve starting at
 * 0x1000 (4096 = 1.0 fixed point) and multiplying by 9/10 per step toward
 * index 0, then clears the halfword just before the table.
 *
 * @note Used by menu animations to provide an ease-in effect, starting
 * slowly and accelerating toward the end.
 */
void func_801EFF64(void) {
    u16 *p = D_801FA3C8;
    s32 val = 4096;
    s32 i;

    p += 64;
    *p = val;
    for (i = 0; i < 64; i++) {
        p--;
        *p = val;
        val = val * 9 / 10;
    }
    p[-1] = 0;
}

/* ======================================================================== */
/* Menu State Getters/Setters                                               */
/* ======================================================================== */

/** @brief Get current menu phase/mode. */
s32 func_801EFFB8(void) {
    return D_801FA27C;
}

/** @brief Set menu sub-state. */
void func_801EFFC8(s32 a0) {
    D_801FA27B = a0;
}

/** @brief Get menu sub-state. */
s32 func_801EFFD4(void) {
    return D_801FA27B;
}

/** @brief Set menu state A. */
void func_801EFFE4(s32 a0) {
    D_801FA27A = a0;
}

/** @brief Get menu state A. */
s32 func_801EFFF0(void) {
    return D_801FA27A;
}

/** @brief Set menu state B. */
void func_801F0000(s32 a0) {
    D_801FA278 = a0;
}

/** @brief Get menu state B. */
s32 func_801F000C(void) {
    return D_801FA278;
}

/** @brief Set menu flag. */
void func_801F001C(s32 a0) {
    D_801FA279 = a0;
}

/** @brief Get menu flag. */
s32 func_801F0028(void) {
    return D_801FA279;
}

/* ======================================================================== */
/* Menu Panel/Window Management                                             */
/* ======================================================================== */

/**
 * @brief Switch to the next menu render buffer and initializes its OT.
 *
 * Alternates between the two menu render buffers, clears the selected buffer's
 * ordering table, updates its ordering table pointer, and sets its draw
 * environment as the active global draw environment.
 */
void func_801F0038(void) {
    MenuRenderBuffer *current = D_801FA3C0;
    MenuRenderBuffer *next = &D_801FA280[0];
    if (current == next) {
        next = &D_801FA280[1];
    }
    D_801FA3C0 = next;
    ClearOTag(&D_801FA3C0->otArray[0], 9);
    D_801FA3C0->ot = &D_801FA3C0->otArray[0];
    g_activeDrawEnv = &D_801FA3C0->drawEnv;
    D_801FA3C0->unk9C = D_801FA3C0->unk98;
}

/**
 * @brief Activate the current menu render buffer.
 *
 * Waits for drawing and vertical synchronization to complete, applies the
 * buffer's display and drawing environments, updates the global active
 * display and draw environment pointers, and submits its ordering table
 * for rendering.
 */
void func_801F00A0(void) {
    DrawSync(0);
    VSync(0);
    PutDispEnv(&D_801FA3C0->dispEnv);
    PutDrawEnv(&D_801FA3C0->drawEnv);
    D_8005F138 = &D_801FA3C0->dispEnv;
    g_activeDrawEnv = &D_801FA3C0->drawEnv;
    DrawOTag(D_801FA3C0->ot);
}

/**
 * @brief Initializes the display and draw environments for double buffering.
 *
 * Configures two frame buffers using VRAM regions at horizontal offsets
 * 0 and 512, respectively. Sets the display height to 216 pixels and
 * adjusts the vertical display position based on the screen width and
 * video mode (NTSC/PAL).
 *
 * @param width Display width in pixels.
 */
void func_801F010C(s32 width) {
    s32 i;
    s32 yOffset;
    SetDefDispEnv(&D_801FA280[0].dispEnv, 0, 0, width, 216);
    SetDefDispEnv(&D_801FA280[1].dispEnv, 512, 0, width, 216);
    SetDefDrawEnv(&D_801FA280[0].drawEnv, 512, 0, width, 216);
    SetDefDrawEnv(&D_801FA280[1].drawEnv, 0, 0, width, 216);
    yOffset = 12;
    if (width == 384) {
        yOffset = 8;
    }
    for (i = 0; i < 2; i++) {
        D_801FA280[i].drawEnv.isbg = 1;
        D_801FA280[i].dispEnv.screen.h = 216;
        D_801FA280[i].dispEnv.screen.y += yOffset;
        if (GetVideoMode() == MODE_PAL) {
            D_801FA280[i].dispEnv.screen.y += 24;
        }
    }
}

/**
 * @brief Initializes the main menu display environment.
 *
 * Initializes the display and draw environments, assigns VRAM buffer
 * addresses to each menu render buffer in D_801FA280, and resets the
 * vibration intensity.
 */
void func_801F0224(void) {
    func_801F010C(384);
    D_801FA280[0].unk98 = 0x801B2000;
    D_801FA280[1].unk98 = 0x801B8800;
    SetVibrationIntensity(0, 0, 0);
}
