#include "ability_list.h"
#include "battle_anim.h"
#include "btl_anim.h"
#include "btl_anim_packet.h"
#include "card.h"
#include "common.h"
#include "game.h"
#include "gamestate.h"
#include "gf.h"
#include "main.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menumain2b.h"
#include "menumain3.h"
#include "numstr.h"
#include "overlay.h"
#include "party_stats.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "render.h"
#include "snd_sfx.h"
#include "ui/font.h"
#include "ui/icon.h"
#include "ui/text.h"

/**
 * @brief Queued text-glyph draw request (see func_801F0954).
 * @note Field meanings inferred from the render-call chain; @c code is the
 *       character/glyph code and @c color the D_801FAAE0 color/font parameter.
 */
typedef struct {
    s16 x;      /**< 0x0: screen X (stored pre-offset by -26). */
    s16 y;      /**< 0x2: screen Y (stored pre-offset by +3). */
    s8  code;   /**< 0x4: character/glyph code. */
    u8  pad05;
    u16 color;  /**< 0x6: color/font parameter. */
} TextQueueEntry; /* 0x8 bytes */

/**
 * @brief Menu task node: pool slot linked into the active-task ring.
 *
 * Allocated from the 10-slot pool D_801FA550 by func_801F179C/func_801F1850
 * and walked by two drivers: func_801F1584 calls @c tickCb and the frame
 * driver func_801F16AC calls @c drawCb.
 */
typedef struct MenuTask {
    struct MenuTask *next;                        /* 0x00 */
    struct MenuTask *prev;                        /* 0x04 */
    void (*tickCb)();                             /* 0x08: update callback (func_801F1584). */
    s32 (*drawCb)(struct MenuTask *, s32, s32);   /* 0x0C: (task, ctx, dl) -> dl (func_801F16AC). */
    u16 state;                                    /* 0x10: per-task state word, zeroed on alloc. */
    s8 inUse;                                     /* 0x12: pool slot occupied. */
    u8 pad13[0x15];
    u32 unk28;
    u8 *unk2C;
    u32 unk30;
    u16 pad34;
    u16 unk36;
    u16 unk38;
    u8 pad3A[0xB];
    u8 unk45;
    u8 pad46[0x32];
} MenuTask; /* 0x78 bytes */

typedef void (*UNK_FUNC)(s32);

typedef struct {
    s32 unk0;
    UNK_FUNC unk4;
} ST_D_801F7E6C; /* 8 bytes */

extern RECT D_801F7E64;
extern ST_D_801F7E6C D_801F7E6C[];
extern u8 D_801F7F74[];        /**< Fallback string for failed text lookups. */
extern u8 D_801F7F78[];
extern u8 D_801F7FB0[];        /**< String table base (two-level offset table). */
extern MenuTask *D_801FA450;    /**< Head of the active menu-task list. */
extern MenuTask D_801FA4D0;     /**< Menu-task list head node. */
extern MenuTask D_801FA550[10]; /**< Menu-task pool. */
extern u16 D_801FAA00[];
extern s16 D_801FAA08[];
extern u8 D_801FAA10;
extern s8 D_801FAA11;
extern u16 D_801FAA12;
extern u16 D_801FAA14;
extern u8 D_801FAA18[];
extern s16 D_801FAA1E;          /**< Text queue count / blink phase (added to g_menuDisplayCfg.animCounter). */
extern TextQueueEntry D_801FAA20[]; /**< Queued text-glyph draw requests. */
extern s16 D_801FAAE0;          /**< Current text color/font parameter. */
extern s32 D_801FAAE4;
extern s32 D_801FAAE8;
extern u8 D_801FAAF0;
extern u16 D_801FAAF2;
extern u8 D_801FAAF8;
extern s32 D_801FAB2C;
extern u8 D_801FAB38[];
extern s16 D_801FAB78;
extern u8 D_801FAB88[];
extern u16 g_menumain_partyMemberMask;

static void func_801F0274(s32, u32, s32, s32);
static void func_801F07D8(void *ot, void *prim, s32 packedCoords, u8 *iconIds, s32 color, s32 clut);
static void *func_801F0AC8(void *ot, void *prim);
static u8 *func_801F08AC(u8 *, s32);
static s32 func_801F16AC(s32, s32);
static s32 *func_801F175C(s32 *, s32);
static MenuTask *func_801F1850(MenuTickCallback, MenuDrawCallback);
static s32 func_801F1A40(s32);
static void func_801F22A8(void);

#define gte_ldfcdir_uniform( x ) __asm__ volatile (       \
    "ctc2   %0, $21;"                                     \
    "ctc2   %0, $22;"                                     \
    "ctc2   %0, $23"                                      \
    :                                                     \
    : "r"( x ) )

/* ======================================================================== */
/* VRAM/Display Setup                                                       */
/* ======================================================================== */

/** @brief Menu initialization routine. */
void func_801F0274(s32 arg0, u32 arg1, s32 arg2, s32 arg3) {
    func_801F0C5C(0, NULL);
    arg0 &= 0xFFFF;
    func_801F0000(0);
    D_801FA27C = arg0;
    switch (arg0) {
    case 1:
        setModeBit(0);
        func_801F0C5C(10, NULL);
        break;
    case 22:
        setModeBit(0);
        setModeBit(1);
        func_801F001C(arg1);
        SetModeBit0();
        func_801F0C5C(13, NULL);
        break;
    case 23:
        func_801EFFE4(arg1);
        func_801F0C5C(11, NULL);
        break;
    case 24:
        func_801F1DB0(0);
        func_801F0C5C(6, NULL);
        break;
    case 25:
        func_801F0C5C(16, NULL);
        break;
    case 26:
        func_801F0C5C(20, NULL);
        break;
    case 27:
        func_801F0000(19);
        func_801F0C5C(15, NULL);
        break;
    case 29: {
        u32 val = 8;
        if (arg1 < 9) {
            val = arg1;
        }
        func_801F0000(val);
        func_801F0C5C(30, NULL);
        break;
    }
    case 28:
        func_801F0000(20);
        func_801F0C5C(15, NULL);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        func_801F0000(arg0 - 2);
        func_801F0C5C(15, NULL);
        break;
    case 4:
        func_801F0000(2);
        func_801F0C5C(15, NULL);
        break;
    case 2:
        func_801F0000(0);
        func_801F0C5C(15, NULL);
        break;
    case 3:
        func_801F0000(1);
        func_801F0C5C(15, NULL);
        break;
    case 0:
    default:
        func_801EFFC8(arg1);
        func_801F0C5C(1, NULL);
        break;
    }
}

/**
 * @brief Build a draw-area primitive for the active draw environment and link it.
 *
 * Copies the active draw environment's clip origin into a local RECT, forces
 * the 384x216 menu draw size, packs the @c SetDrawArea GP0 command for it
 * into the @c DR_AREA at @p prim, links @p prim into the ordering-table slot
 * @p tag via @c addPrimFast, and returns the next
 * packet slot.
 *
 * @param ot   Ordering-table slot to link the primitive into.
 * @param prim Storage for the @c DR_AREA primitive.
 * @return Cursor for the next primitive (@c prim + 1).
 *
 * @note The clip reads go through a u16 view of the s16 RECT fields (lhu, not
 *       lh) and w/h are filled by one word store — both match-required.
 */
DR_AREA *func_801F03E8(P_TAG *ot, DR_AREA *prim) {
    RECT r;
    DRAWENV *env = g_activeDrawEnv;

    r.x = *(u16 *)&env->clip.x;
    r.y = *(u16 *)&env->clip.y;
    *(u32 *)&r.w = (216 << 16) | 384;
    SetDrawArea(prim, &r);
    addPrimFast(ot, prim);
    return ++prim;
}

/**
 * @brief Upload two display regions to VRAM.
 *
 * Configures two 0x180-wide rectangular regions (at x=0 and x=0x200)
 * and transfers them via ClearImage/DrawSync. Initializes
 * double-buffered VRAM areas for the menu background.
 */
void func_801F0464(s32 a0) {
    s32 buf[2];
    buf[0] = 0;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    ClearImage(buf, 0, 0, 0);
    DrawSync(0);
    buf[0] = 0x200;
    *(u16 *)((u8 *)buf + 4) = 0x180;
    *(u16 *)((u8 *)buf + 6) = a0;
    ClearImage(buf, 0, 0, 0);
    DrawSync(0);
}

/* ======================================================================== */
/* Text/String Lookup & Rendering                                           */
/* ======================================================================== */

/** @brief Menu initialization routine. */
void func_801F04E8(s32 arg0, u32 arg1, s32 arg2, s32 arg3) {
    s32 i;
    u8 *tempGp;
    u8 *savedGp;
    func_801F22A8();
    GP_SAVE_SCRATCH(tempGp);
    savedGp = tempGp;
    gte_ldfcdir_uniform(0);
    DrawSync(0);
    VSync(0);
    SetDispMask(0);
    func_801F6888();
    func_801F72C4();
    if (g_gameState.mainData.party.gil > 99999999) {
        g_gameState.mainData.party.gil = 99999999;
    }
    if (g_gameState.mainData.party.dreamGil > 99999999) {
        g_gameState.mainData.party.dreamGil = 99999999;
    }
    func_801EFF64();
    func_801F1D84();
    WaitForGamePadDetection();
    func_801F0464(216);
    func_801F1924();
    func_801F0274(arg0, arg1, arg2, arg3);
    DrawSync(0);
    func_801F0224();
    swapDisplayList();
    for (i = 0; i < 2; i++) {
        func_801F0038();
        D_801FA3C0->unk9C = func_801F175C(&D_801FA3C0->ot[0], D_801FA3C0->unk9C);
        renderBattleDisplayList(&D_801FA3C0->ot[6]);
        func_801F00A0();
    } 
    DrawSync(0);
    VSync(0);
    SetDispMask(1);
    while (getRenderFlag() == 0) {
        func_801F0038();
        func_801F1584();
        renderAndUpdateDisplay(1);
        renderBattleDisplayList(&D_801FA3C0->ot[6]);
        D_801FA3C0->unk9C = func_801F175C(&D_801FA3C0->ot[1], D_801FA3C0->unk9C);
        pollCdReadStatus();
        func_801F00A0();
    }
    DrawSync(0);
    if (D_801FA3C0->dispEnv.disp.x != 0) {
        MoveImage(&D_801F7E64, 0, 0);
    }
    D_80085150 = D_801FA280[0].dispEnv;
    D_8005F138 = &D_80085150;
    DrawSync(0);
    VSync(0);
    PutDispEnv(D_8005F138);
    func_801F0038();
    VSync(0);
    func_801F0464(224);
    WaitForGamePadDetection();
    GP_RESTORE_RET(savedGp, tempGp);
}

static void func_801F07D8(void *ot, void *prim, s32 packedCoords, u8 *iconIds, s32 color, s32 clut) {
    s32 x;
    s32 y;

    // Dead code added to match with the original game binary.
    while (0);

    y = packedCoords >> 16;
    x = packedCoords << 16;
    x >>= 16;

    while (1) {
        s32 iconId = *iconIds++;
        if (iconId == 0) {
            break;
        }
        if (iconId != 7) {
            prim = drawIconClut(ot, prim, iconId, x, y, color, clut);
        }
        x += 8;
    }   
}

/** @brief Render text string with default style (last param = 0). */
void func_801F0884(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    func_801F07D8(a0, a1, a2, a3, a4, 0);
}

/**
 * @brief Resolve an entry in a relative-offset table.
 *
 * The table is an array of u16 byte offsets (relative to @p base) with a
 * 2-byte header word at +0; entry @p index lives at base + index * 2 + 2.
 * An offset of 0 marks an empty slot.
 *
 * @param base Table base pointer.
 * @param index Entry index.
 * @return base + offset for the entry, or NULL if the slot is empty.
 */
static u8 *func_801F08AC(u8 *base, s32 index) {
    u16 *p = (u16 *)(base + index * 2);

    /* Separate p++ and two *p reads (not p[1]): keeps the +2 as its own
     * addiu instead of folding into the load offset. */
    p++;
    if (*p == 0) {
        return 0;
    }
    return base + *p;
}

/**
 * @brief Look up a text string by category and ID.
 *
 * Navigates a two-level string table. Uses 0x801E0000 for type 1,
 * D_801F7FB0 otherwise. Returns D_801F7F74 (fallback) on failure.
 */
u8 *func_801F08D4(s32 a0, s32 a1, s32 a2, s32 a3) {
    u8 *ptr;
    if (a0) {
        ptr = (u8 *)0x801E0000;
    } else {
        ptr = D_801F7FB0;
    }
    if (!ptr) return D_801F7F74;
    ptr = func_801F08AC(ptr, a1);
    if (!ptr) return D_801F7F74;
    ptr = func_801F08AC(ptr, a2 * 2 + a3);
    if (ptr) return ptr;
    return D_801F7F74;
}

/* ======================================================================== */
/* Text Rendering Helpers (blink, color, conditional)                       */
/* ======================================================================== */

/** @brief Set current text color/font parameter for rendering. */
void func_801F0948(s32 a0) {
    D_801FAAE0 = a0;
}

/**
 * @brief Queue a text glyph for rendering.
 *
 * Appends a TextQueueEntry to D_801FAA20: position is anchored at
 * (x - 26, y + 3), together with the glyph code and the current
 * color/font parameter. D_801FAA1E is the queue count; the entry index
 * reads it as s16 while the increment goes through a u16 view (both
 * halves of the original's mixed-signedness access).
 *
 * @param code  Character/glyph code (stored as a byte).
 * @param color Color/font parameter (usually D_801FAAE0).
 * @param x     Screen X before the -26 anchor offset.
 * @param y     Screen Y before the +3 anchor offset.
 */
void func_801F0954(s32 code, s32 color, s32 x, s32 y) {
    TextQueueEntry *e = &D_801FAA20[D_801FAA1E];

    e->code = code;
    e->x = x - 26;
    e->y = y + 3;
    e->color = color;
    *(u16 *)&D_801FAA1E += 1;
}

/** @brief Render text at stored Y with default color. */
s32 func_801F0994(s32 a0, s32 a1, s32 a2) {
    func_801F0954(a0, D_801FAAE0, a1, a2);
}

/**
 * @brief Render text with blink effect.
 *
 * If (g_menuDisplayCfg.animCounter + D_801FAA1E) is odd, sets color to -1 (hidden),
 * creating a blinking text effect for highlighted menu items.
 */
void func_801F09C4(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (!((g_menuDisplayCfg.animCounter + D_801FAA1E) & 1)) {
        a0 = -1;
    }
    func_801F0954(a0, a1, a2, a3);
}

/** @brief Render blinking text at stored Y with default color. */
void func_801F0A04(s32 a0, s32 a1, s32 a2) {
    func_801F09C4(a0, D_801FAAE0, a1, a2);
}

/** @brief Render text, normal if a0 is nonzero, blinking otherwise. */
void func_801F0A34(s32 a0, s32 a1, s32 a2, s32 a3) {
    if (a0) {
        func_801F0994(a1, a2, a3);
    } else {
        func_801F0A04(a1, a2, a3);
    }
}

/** @brief Render text with explicit Y, normal if a0 nonzero, blinking otherwise. */
void func_801F0A78(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) {
    if (a0) {
        func_801F0954(a1, a2, a3, a4);
    } else {
        func_801F09C4(a1, a2, a3, a4);
    }
}

/* ======================================================================== */
/* Menu Navigation Stack                                                    */
/* ======================================================================== */

/** @brief Draws a cursor in main menu. */
void *func_801F0AC8(void *ot, void *prim) {
    TextQueueEntry *entry;
    s32 i;
    entry = D_801FAA20;
    ot += 12;
    for (i = 0; i < D_801FAA1E; i++, entry++) {
        s32 tmp;
        s32 color;
        tmp = entry->color;
        tmp /= 32;
        color = 0x64000000 | (tmp << 16) | (tmp << 8) | tmp;
        if (entry->code >= 0) {
            s32 offset = (entry->code >> 2) << 2;
            prim = drawIcon(ot + offset, prim, ICON_CHOICE_CURSOR, entry->x, entry->y, color);
        }
    }
    D_801FAA1E = 0;
    return prim;
}

/**
 * @brief Pop top entry from menu navigation stack.
 *
 * Decrements the stack pointer and returns the popped screen ID.
 * Returns -1 if the stack is empty.
 */
s32 func_801F0BB0(void) {
    s8 val;

    D_801FAA11--;
    val = D_801FAA11;
    if (val < 0) {
        D_801FAA11 = 0;
        return -1;
    }
    return D_801FAA18[val];
}

/**
 * @brief Play music/sound for a menu screen.
 *
 * Looks up audio entry in D_801F7E6C (stride 8). If the track
 * differs from what is currently playing, loads and plays it.
 */
void func_801F0BF8(s32 a0) {
    ST_D_801F7E6C *entry = &D_801F7E6C[a0];
    s32 val = entry->unk4;
    if (val == 0xFF) return;
    if (val <= 0) return;
    if (getOverlayLoadStatus() == val) return;
    loadOverlay(val, 0, 0);
}

void func_801F0C5C(s32 arg0, void *arg1) {
    ST_D_801F7E6C *ptr = &D_801F7E6C[arg0];
    s32 unkn4 = ptr->unk4;
    UNK_FUNC unkn0 = ptr->unk0;
    if (unkn4 == 0xFF) {
        if (unkn0 != NULL) {
            unkn0(arg1);
        }
        return;
    }
    D_801FAA11++;
    D_801FAA18[D_801FAA11] = arg0;
    if (unkn0 == NULL) {
        D_801FAA11--;
        return;
    }
    if (unkn4 > 0) {
        s32 loadStatus = getOverlayLoadStatus();
        if (loadStatus == unkn4) {
            unkn0(arg1);
            return;
        }
        if (loadStatus == -2) {
            while (pollCdReadStatus());
            if (getOverlayLoadStatus() == unkn4) {
                unkn0(arg1);
                return;
            }
            loadOverlay(ptr->unk4, unkn0, arg1);
        } else {
            loadOverlay(unkn4, unkn0, arg1);
        }
        return;
    }
    while (pollCdReadStatus());
    unkn0(arg1);
}

/** @brief Peek at top of menu navigation stack without popping. */
s32 func_801F0D84(void) {
    return D_801FAA18[(s8)D_801FAA11];
}

/** @brief Set current menu screen ID. */
void func_801F0DA4(s32 a0) {
    D_801FAA10 = a0;
}

/** @brief Get current menu screen ID. */
s32 func_801F0DB0(void) {
    return D_801FAA10;
}

/* ======================================================================== */
/* Animation/Transition                                                     */
/* ======================================================================== */

s32 func_801F0DC0(EngineState *engine, GamePad *gamePad, s32 arg2, s32 index) {
    s32 mask;
    s32 val1;
    s32 val2;
    s32 val3;
    s32 val4;
    s32 ret;
    s32 tmp;

    val1 = D_801FAA00[index];
    D_801FAA00[index] = arg2;
    val2 = engine->repeatDelays.hword;
    val3 = D_801FAA08[index];
    mask = gamePad->autoRepeatButtonMasks[index];
    val4 = val2 >> 8;
    val2 &= 0xFF;
    arg2 &= mask;
    
    if (arg2) { arg2++; arg2--; }
    
    ret = arg2;
    tmp = val1 & mask;
    mask = tmp;
    
    if (arg2) { arg2++; arg2--; }

    arg2 &= mask;
    val1 = mask;
    if (arg2 != 0) {
        if (ret != tmp) {
            val3 = val2;
        }
        val3--;
        val1 = 0;
        if (val3 < val1) {
            val3 = val4;
        } else {
            ret = 0;
        }
    } else {
        val3 = val2;
    }
    D_801FAA08[index] = val3;
    return ret;
}

u16 func_801F0E5C(u16 arg0) {
    GamePad *gamePad = &g_engine.gamePads[g_engine.gamePads[0].inputSourceIndex];
    u16 ret = func_801F0DC0(&g_engine, gamePad, arg0, 0);
    ret |= func_801F0DC0(&g_engine, gamePad, arg0, 1);
    ret |= func_801F0DC0(&g_engine, gamePad, arg0, 2);
    ret |= func_801F0DC0(&g_engine, gamePad, arg0, 3);
    return ret;
}

/**
 * @brief Build a 12x12 menu-font sprite primitive and prepend it to the OT chain.
 *
 * Fills the SPRT at @p spr: CLUT selected by the low 3 bits of
 * @p clutFlags (base 0x3812), color from g_menuTint[MENU_TINT_BLINK] when any
 * higher flag bit is set, else g_menuTint[MENU_TINT_NORMAL]; UV computed from
 * the glyph index in the 21-glyphs-per-row 12px font atlas. Links the sprite
 * via addOtFast and returns the new chain head.
 *
 * @note @c clutFlags is reused for the color word after its flag bits are
 *       consumed — the reuse gives color a3 (regalloc match).
 *
 * @param head      Current OT chain head (prim address image << 8).
 * @param spr       Storage for the SPRT primitive.
 * @param glyph     Glyph index into the font atlas.
 * @param clutFlags Bits 0..2: CLUT row; bits 3+: use alternate color.
 * @param xy        Packed x0 | y0 << 16 sprite position.
 * @return New OT chain head (@p spr's address image).
 */
s32 func_801F0F20(s32 head, SPRT *spr, s32 glyph, u32 clutFlags, s32 xy) {
    u32 hi;

    setlen(spr, 4);
    addOtFast(spr, head);
    hi = clutFlags >> 3;
    clutFlags &= 7;
    spr->clut = (clutFlags << 6) + 0x3812;
    if (hi != 0) {
        clutFlags = g_menuTint[MENU_TINT_BLINK];
    } else {
        clutFlags = g_menuTint[MENU_TINT_NORMAL];
    }
    /* Word stores: w/h pair (12x12) and the r0g0b0+code word. */
    *(u32 *)&spr->w = 0xC000C;
    *(u32 *)&spr->r0 = clutFlags;
    *(u32 *)&spr->x0 = xy;
    *(u16 *)&spr->u0 = (glyph % 21 | (glyph / 21) << 8) * 12;
    return head;
}

/** @brief Set menu display dimensions (width defaults to 0x180). */
void func_801F0FD0(s32 a0, s32 a1) {
    if (!a1) {
        a1 = 0x180;
    }
    D_801FAA12 = a1;
    D_801FAA14 = a0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F0FEC);

/** @brief Get animation completion flag. */
s32 func_801F1200(void) {
    return D_801FAAF8;
}

/**
 * @brief Start a timed animation/transition.
 *
 * Sets source value, destination value, resets counter, and enables
 * the animation. D_801FAAF8 will be set when the transition completes.
 */
void func_801F1210(s32 a0, s32 a1) {
    D_801FAAE4 = a0;
    D_801FAAE8 = a1;
    D_801FAAF2 = 0;
    D_801FAAF0 = 1;
    D_801FAAF8 = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F1240);

/** @brief Stop the current animation/transition. */
void func_801F12F0(void) {
    D_801FAAF0 = 0;
}

/* ======================================================================== */
/* Panel Iteration & Callbacks                                              */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F12FC);

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F1584);

/**
 * @brief Per-frame menu render driver.
 *
 * Advances the menu animation counter, resets the color intensity, renders
 * the base menu layer (func_801F0AC8), submits the frame GPU packet
 * (func_801F2FAC over the current display-list head), then walks the
 * MenuTask list at D_801FA450 calling each task's handler; every handler
 * returns the advanced display-list cursor.
 *
 * @param ctx Render context.
 * @param dl  Display-list cursor.
 * @return Display-list cursor after all task handlers ran.
 */
static s32 func_801F16AC(s32 ctx, s32 dl) {
    MenuTask *t;
    MenuTask *cur;
    s32 (*drawCb)(MenuTask *, s32, s32);

    g_menuDisplayCfg.animCounter++;
    setMenuBrightness(BRIGHTNESS_NORMAL);
    t = D_801FA450;
    dl = func_801F0AC8(ctx, dl);
    storeGpuPacket(func_801F2FAC(ctx, getDisplayListHead()));
    while (1) {
        if (t == 0) {
            break;
        }
        cur = t;
        drawCb = t->drawCb;
        t = t->next;
        if (drawCb != 0) {
            dl = drawCb(cur, ctx, dl);
        }
    }
    return dl;
}

/** @brief Process all panel callbacks (wrapper for func_801F16AC).
 *
 * @note Calls through an unspecified-args cast with no arguments — the
 *       original passes whatever $a0/$a1 hold (the callee's ctx/dl inputs
 *       are dead this frame); an ANSI call would emit argument setup. */
static s32 *func_801F175C(s32 *a0, s32 a1) {
    return func_801F16AC(a0, a1);
}

/** @brief Clear 22 panel callback entries starting at a0[8]. */
void func_801F177C(s32 *a0) {
    s32 i;

    for (i = 21; i >= 0; i--) {
        a0[i + 8] = 0;
    }
}

/**
 * @brief Allocate a menu state task and link it at the head of the list.
 *
 * Claims the first free slot of the 10-entry pool D_801FA550, marks it in
 * use, and inserts it at the front of the D_801FA450 list: the head pointer
 * doubles as a sentinel node whose @c next field is the pointer itself, so
 * the insert is the ordinary four-store list splice. Clears the task's
 * callback table via func_801F177C, then stores @p tickCb, @p drawCb and a
 * zeroed state.
 *
 * @param tickCb Update callback stored in the task (func_801F1584).
 * @param drawCb Per-frame draw callback (func_801F16AC).
 * @return The claimed task as void*, or NULL when the pool is full.
 */
void *func_801F179C(MenuTickCallback tickCb, MenuDrawCallback drawCb) {
    MenuTask *node;
    MenuTask *n = D_801FA550;
    MenuTask *first;
    MenuTask *head;
    s32 i;

    i = 0;
    while (1) {
        if (n->inUse == 0) {
            n->inUse = 1;
            node = n;
            break;
        }
        i++;
        if (i >= 10) {
            node = 0;
            break;
        }
        n++;
    }
    if (node == 0) {
        return NULL;
    }
    head = (MenuTask *)&D_801FA450;
    first = head->next;
    node->prev = head;
    node->next = first;
    head->next = node;
    first->prev = node;
    func_801F177C((s32 *)node);
    node->tickCb = tickCb;
    node->drawCb = drawCb;
    node->state = 0;
    return node;
}

/**
 * @brief Allocate a menu task and link it after the list head.
 *
 * Claims the first free slot of the 10-entry pool D_801FA550, marks it in
 * use, splices it in directly after D_801FA4D0 (the list head), clears its
 * callback table via func_801F177C, and stores @p tickCb and @p drawCb.
 *
 * @note The explicit exhaust-break (rather than a for loop with node
 *       pre-initialized) is what places the @c node @c = @c 0 on the
 *       loop-exit path, as the original does.
 *
 * @param tickCb Update callback stored in the task (func_801F1584).
 * @param drawCb Per-frame draw callback (func_801F16AC).
 * @return The claimed task, or NULL when the pool is full.
 */
static MenuTask *func_801F1850(MenuTickCallback tickCb, MenuDrawCallback drawCb) {
    MenuTask *node;
    MenuTask *n = D_801FA550;
    MenuTask *next;
    s32 i;

    i = 0;
    while (1) {
        if (n->inUse == 0) {
            n->inUse = 1;
            node = n;
            break;
        }
        i++;
        if (i >= 10) {
            node = 0;
            break;
        }
        n++;
    }
    if (node == 0) {
        return 0;
    }
    next = node->next;
    D_801FA4D0.prev = node;
    D_801FA4D0.next = next;
    node->next = &D_801FA4D0;
    next->prev = &D_801FA4D0;
    func_801F177C((s32 *)&D_801FA4D0);
    node->tickCb = tickCb;
    node->drawCb = drawCb;
    return node;
}

/**
 * @brief Remove a node from the panel doubly-linked list.
 *
 * Unlinks the node, patches prev/next pointers, and clears the
 * node's status byte and data fields.
 */
void func_801F18FC(s32 *a0) {
    s32 *next = (s32 *)a0[1];
    s32 *prev = (s32 *)a0[0];
    *next = (s32)prev;
    prev[1] = (s32)next;
    *(u8 *)((u8 *)a0 + 0x12) = 0;
    a0[2] = 0;
    a0[3] = 0;
    *(s16 *)((u8 *)a0 + 0x10) = 0;
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F1924);

/* ======================================================================== */
/* Cursor/Input Handling                                                    */
/* ======================================================================== */

/** @brief Panel input state machine: advance from idle to active. */
s32 func_801F1A40(s32 a0) {
    s32 ret = 1;
    switch (*(u16 *)(a0 + 0x10)) {
    case 0:
        *(u16 *)(a0 + 0x10) = ret;
        break;
    case 1:
        if (func_801F0D84() != 0) {
            break;
        }
        setRenderFlag(1);
        break;
    default:
        break;
    }
    return ret;
}

/** @brief Default panel callback (identity, returns a2 unchanged). */
s32 func_801F1AA4(s32 a0, s32 a1, s32 a2) {
    return a2;
}

/** @brief Start a panel task (tick func_801F1A40, draw func_801F1AA4) and run its first tick. */
void func_801F1AAC(void) {
    /* func_801F1A40 returns a status that the tick driver ignores. */
    s32 result = func_801F179C((MenuTickCallback)func_801F1A40, func_801F1AA4);

    if (result != 0) {
        func_801F1A40(result);
    }
}

/* ======================================================================== */
/* Party Member Data & Input                                                */
/* ======================================================================== */

/** @brief Set cursor dimensions (row count, column count). */
void func_801F1AE8(s32 a0, s32 a1) {
    D_801FAB7A = a0;
    D_801FAB7B = a1;
}

/** @brief Save the current menu brightness. */
void func_801F1AFC(void) {
    D_801FAB78 = g_menuBrightness;
}

/** @brief Restore the saved menu brightness, and the GPU colour with it. */
void func_801F1B10(void) {
    setMenuBrightness(D_801FAB78);
    setNextPageMarkerBrightness(D_801FAB78);
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F1B4C);

/** @brief Update all 8 party member cursor/panel states. */
void func_801F1CAC(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        func_801F1B4C(i);
    }
}

/** @brief Test if ability bit a1 is set for character a0. */
s32 func_801F1CE8(s32 a0, s32 a1) {
    u32 *base = (u32 *)(D_801FAB38 + a0 * 8);
    return (base[a1 / 32] & (1 << (a1 & 0x1F))) != 0;
}

/* ======================================================================== */
/* Menu Item List                                                           */
/* ======================================================================== */

/** @brief Empty stub (placeholder for panel init callback). */
void func_801F1D2C(s32 a0, s32 a1, s32 a2) {
}

/**
 * @brief Initialize menu item list from 0xFF-terminated data.
 *
 * Counts entries (stride 2 bytes) and stores the data pointer,
 * entry count, and resets the selection index.
 */
void func_801F1D34(u8 *a0) {
    s32 count;
    s32 sentinel;

    D_801FAB2C = (s32)a0;
    D_801FAB31 = 0;
    if (a0 == 0) {
        D_801FAB30 = 0;
    } else {
        count = 0;
        sentinel = 0xFF;
    top:
        if (*a0 == sentinel) goto out;
        a0 += 2;
        count++;
        goto top;
    out:
        D_801FAB30 = count;
    }
}

/** @brief Reset all menu item list state (scroll, pointer, count, index). */
void func_801F1D84(void) {
    D_801FAB28 = 0;
    D_801FAB2A = 0;
    D_801FAB2C = 0;
    D_801FAB30 = 0;
    D_801FAB31 = 0;
}

/** @brief Set menu list scroll offset. */
void func_801F1DB0(s32 a0) {
    D_801FAB28 = a0;
}

/**
 * @brief Select a menu item list by ID.
 *
 * Searches D_801F7F78 for the matching entry, then initializes the
 * list and sets the selection index to the found position.
 */
void func_801F1DBC(s32 a0) {
    u8 *ptr = D_801F7F78;
    s32 i;

    for (i = 0; ; i++, ptr += 2) {
        if (*ptr == 0xFF) {
            return;
        }
        if (*ptr == a0) {
            func_801F1D34(D_801F7F78);
            D_801FAB31 = i;
            return;
        }
    }
}

/* ======================================================================== */
/* Party Member Switch                                                      */
/* ======================================================================== */

/** @brief Save the 3 active party slot IDs from g_gameState.mainData into @c ctx->party. */
void func_801F1E20(MainMenuCtx *ctx) {
    u8 *src = g_gameState.mainData.party.partyMembers;
    u8 *dst = ctx->party;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

/** @brief Restore the 3 active party slot IDs from @c ctx->party to g_gameState.mainData. */
void func_801F1E54(MainMenuCtx *ctx) {
    u8 *dst = g_gameState.mainData.party.partyMembers;
    u8 *src = ctx->party;
    s32 i;
    for (i = 0; i < 3; i++) {
        *dst++ = *src++;
    }
}

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F1E88);

/** @brief Swap party members (forward direction). */
void func_801F1F58(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 1);
}

/** @brief Swap party members (backward direction). */
void func_801F1F78(s32 a0, s32 a1) {
    func_801F1E88(a0, a1, 0);
}

/**
 * @brief Build sorted available character list from party data.
 *
 * Fills D_801FAB88 with 0xFF, then collects valid (non-0xFF) entries
 * from @c ctx->party and @c ctx->reserve.
 */
void func_801F1F98(MainMenuCtx *ctx) {
    u8 *dst = D_801FAB88;
    s32 i;
    u8 *p;
    s32 fill;

    fill = 0xFF;
    i = 8;
    p = dst + 8;
    do {
        *p = fill;
        i--;
        p--;
    } while (i >= 0);

    for (i = 0; i < 3; i++) {
        u8 val = ctx->party[i];
        if (val != 0xFF) {
            *dst++ = val;
        }
    }

    for (i = 0; i < 8; i++) {
        u8 val = ctx->reserve[i];
        if (val != 0xFF) {
            *dst++ = val;
        }
    }
}

/* ======================================================================== */
/* Character/Entity Data Access                                             */
/* ======================================================================== */

INCLUDE_ASM("asm/ovl/menumain/nonmatchings/menumain2", func_801F202C);

/** @brief Get character status flags. */
s32 func_801F21D0(s32 a0) {
    return g_gameState.chars[a0].statusFlags;
}

/** @brief Set character status flags and sync to secondary table. */
void func_801F21FC(s32 charId, s32 statusFlags) {
    g_gameState.chars[charId].statusFlags = statusFlags;
    D_801FABC8[charId].statusFlags = statusFlags;
}

/** @brief Get GF status (stub, always returns 0). */
s32 func_801F2238(s32 a0) {
    return 0;
}

/**
 * @brief Get a GF's health condition.
 *
 * @param arg0 GF index (0-15).
 * @return 1 if its HP is 0 or less, 0x100 if it's below a quarter of max HP, otherwise 0.
 */
s32 func_801F2240(s32 arg0) {
    BattleGfEntry* temp_v1;
    s32 var;

    temp_v1 = &g_battleChars.gfs[arg0];

    var = 0;
    if (temp_v1->hp < 1) {
        var = 1;
    }
        
    else if (temp_v1->hp < (temp_v1->maxHp >> 2)) {
        var = 256;
    }
    return var;
}

/** @brief Get party presence bitmask. */
s32 menumain_getPartyMemberMask(void) {
    return g_menumain_partyMemberMask;
}

/**
 * @brief Compute party presence bitmask.
 *
 * Iterates 3 active party slots at g_gameState + 0xAF4, sets a bit
 * for each valid (non-0xFF) member ID. Stores result in g_menumain_partyMemberMask.
 */
static void func_801F22A8(void) {
    s32 result = 0;
    s32 i;
    REGALLOC_BARRIER(result);
    i = 0;

    for (; i < 3; i++) {
        u8 val = g_gameState.mainData.party.partyMembers[i];
        if (val != PARTY_SLOT_EMPTY) {
            result |= (1 << val);
        }
    }
    g_menumain_partyMemberMask = result;
}

/* ======================================================================== */
/* Character Panel Rendering                                                */
/* ======================================================================== */

/**
 * @brief Build the bitmask of characters that are alive and available.
 *
 * Sets bit i for every character whose status does not include
 * @c STATUS_KO, then intersects with the character availability mask
 * from func_80036EC0.
 *
 * @return Bitmask of usable characters, bit index = character ID.
 */
s32 func_801F22F4(void) {
    s32 avail = func_80036EC0();
    u16 mask = 0;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (!(func_801F21D0(i) & STATUS_KO)) {
            mask |= 1 << i;
        }
    }
    mask &= avail;
    return mask;
}

/**
 * @brief Build the bitmask of GFs that are alive and available.
 *
 * Sets bit i for every GF whose saved HP is nonzero, then intersects
 * with getGfAvailabilityMask() (story/junction availability).
 *
 * @return u16 bitmask of usable GFs, bit index = GfId.
 */
u16 func_801F2370(void) {
    s32 avail = getGfAvailabilityMask();
    u16 mask = 0;
    s32 i;

    for (i = 0; i < GF_COUNT; i++) {
        if (g_gameState.gfs[i].hp != 0) {
            mask |= 1 << i;
        }
    }
    mask &= avail;
    return mask;
}

/**
 * @brief Register a horizontally centered text region.
 *
 * Measures text @p textId via measureMessage (packed w | h << 16), maps
 * both extents through func_801F738C/func_801F7394, then registers the
 * RECT — x centered on the 384-wide menu screen at row @p y — with
 * setDialogRect under @p idx.
 *
 * @note @c w holds the packed measurement first and is then reassigned to
 *       the mapped width — the reuse is what allocates s0/s1/s2 like the
 *       original.
 *
 * @param idx Region slot index (passed to setDialogRect).
 * @param y Screen Y for the region.
 * @param text Text to measure.
 */
void func_801F23D0(s32 idx, s32 y, u8 *text) {
    RECT r;
    s32 w = measureMessage(text);
    s32 upper = w >> 16;
    s32 h;

    w = func_801F738C(w & 0xFFFF);
    h = func_801F7394(upper);
    r.x = (0x180 - w) / 2;
    r.y = y;
    r.w = w;
    r.h = h;
    setDialogRect(idx, &r);
}
