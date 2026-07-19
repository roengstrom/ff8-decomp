#ifndef TT_MATCH_H
#define TT_MATCH_H

#include "common.h"
#include "psxsdk/libgte.h"  /* SVECTOR (layoutCardSlot) */

/* Declarations for tt_match.c (Triple Triad match-flow controller, the
   per-frame update-list callbacks, and card-object search helpers). */

/* ───────────────────────────── Public ──────────────────────────────────── */

/* Enums / defines / consts */

/** @brief @c layoutCardSlot descriptor type (the byte at @c src[0]). */
typedef enum {
    TT_CARDSLOT_STRIP_L = 0,  /**< Left vertical strip.  */
    TT_CARDSLOT_STRIP_R = 1,  /**< Right vertical strip. */
    TT_CARDSLOT_GRID    = 2,  /**< Board tile grid.      */
} TripleTriadCardSlotLayout;

/* Public prototypes */

/** @brief Build the per-frame update list and return its head. */
extern u8 *initTripleTriadUpdateList(void);

/** @brief Compute a card's screen position (an @c SVECTOR) from a slot descriptor. */
extern SVECTOR *layoutCardSlot(u8 *src, SVECTOR *dst);

/** @brief Find the @c g_tripleTriadCardHands slot matching a search key, or -1. */
extern s32 findCardSlot(s32 groupId, s32 fieldD, s32 priority);

/** @brief Flag the matching card object (sets @c TT_CARD_ROTATE_CW). */
extern void highlightCardSlot(s32 groupId, s32 priority);

/* ───────── Private (only used in tt_match.c; may move into the .c) ────── */

/* Enums / defines / consts */

/** matchFlowHandler hold-frame durations + result-screen cancel bit. */
#define TT_HOLD_FRAMES_RULE  0x1E    /**< Hold after a combo capture (MATCH_FLOW_RULES). */
#define TT_HOLD_FRAMES_TALLY 0x0C    /**< Hold on the card-count tally (MATCH_FLOW_TALLY). */
#define TT_HOLD_FRAMES_FADE  0x0F    /**< Hold during the result fade (MATCH_FLOW_FADE). */
#define PAD_UP               0x4000  /**< Result-screen cancel bit (g_padPressed[2]). */

/** @brief @c D_80082C9C match-result category (also picks the @c TripleTriadData record bumped). */
#define TT_RESULT_VICTORY 0
#define TT_RESULT_DEFEAT  1
#define TT_RESULT_DRAW    2
#define TT_RESULT_QUIT    3   /**< Player quit at the post-match Play/Quit prompt (indexes past win/loss/draw — no record bumped). */

/* Typedefs */

/** @brief Six-word combined DR_TPAGE + SPRT primitive (24 bytes) used by the
 *  cell-marker and score-tally renderers. */
typedef struct {
    /* 0x00 */ u32 tag;        /**< P_TAG: len=5, next=0 (six-dword primitive). */
    /* 0x04 */ u32 tpageCmd;   /**< GP0(0xE1) Draw Mode + tpage attribute. */
    /* 0x08 */ u32 sprtCmd;    /**< GP0(0x66) textured sprite + tint RGB. */
    /* 0x0C */ s16 x0, y0;     /**< Screen-space sprite origin (top-left). */
    /* 0x10 */ u8  u0, v0;     /**< Texture-page UV. */
    /* 0x12 */ u16 clut;       /**< CLUT id. */
    /* 0x14 */ s16 w, h;       /**< Sprite size in pixels. */
} TripleTriadCellPrim;

/** @brief @c HandlerNode.state values for the match-flow controller
 *  (@ref matchFlowHandler). States 2 and 3 are unused. */
typedef enum {
    MATCH_FLOW_INIT     = 0,  /**< Set up the match; pick the starting player.   */
    MATCH_FLOW_TURN     = 1,  /**< A player or the AI places a card.             */
    MATCH_FLOW_RULES    = 4,  /**< Evaluate Same/Plus and animate the captures.  */
    MATCH_FLOW_CHAIN    = 5,  /**< Cascade further capture chains until settled. */
    MATCH_FLOW_TURN_END = 6,  /**< Board full -> tally; else next player's turn. */
    MATCH_FLOW_TALLY    = 7,  /**< Count each side's cards and pick the winner.  */
    MATCH_FLOW_RESULT   = 8,  /**< Play the result jingle; wait for input.       */
    MATCH_FLOW_FADE     = 9,  /**< Fade out, record the result, exit the match.  */
} MatchFlowState;

/**
 * @brief Callback node in the tripletriad list at @c g_gradFadeList.
 *
 * Minimal view: a list-node header followed by a list-head pointer. Other fields
 * (byte flags at 0x0E, 0x22; s16 at 0x20; etc.) are not yet modeled.
 */
typedef struct {
    u8  pad00[0xC];
    u8 *listPtr;    /* 0x0C: pointer to an inner list head */
} CallbackNode;

/* Data */
extern u8  D_80082C9C;  /**< Match-result category byte (a @c TT_RESULT_* value). */
extern s32 g_resultSfxHandle;  /**< Result-screen SFX node handle (from @c spawnGradientFade); dismissed on input. */

#endif /* TT_MATCH_H */
