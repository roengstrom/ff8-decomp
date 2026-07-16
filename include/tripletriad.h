#ifndef TRIPLETRIAD_H
#define TRIPLETRIAD_H

#include "common.h"
#include "psxsdk/libgpu.h"  /* TSPRT (drawCardOverlaySprite) */
#include "psxsdk/libgte.h"  /* SVECTOR / MATRIX (CardRenderWork, CardAnimNode) */

/* Types, constants, and globals for the Triple Triad card mini-game. Its code
   is the `tripletriad` overlay, which loads at the same VRAM address
   (0x80098000) as the `battle` overlay — they are mutually exclusive, not
   nested. */

/**
 * @brief One slot on the Triple Triad board (8 bytes).
 *
 * The board is laid out as a 5-cell-wide grid (@c TT_BOARD_COLS) with the
 * 3x3 active play area at rows/cols 1..3 and a 1-cell sentinel border
 * around it. Border slots have @c flags = 0, so neighbor lookups at the
 * edges of the play area fall through cleanly without bounds checks.
 *
 * Multiple bits in @c flags are stateful across a turn:
 *   - @c 0x01 : sentinel/wall slot (set only on border cells; used by the
 *               Same-Wall rule when a rank-A edge faces a wall).
 *   - @c 0x02 : occupied — a card has been placed here.
 *   - @c 0x04 : just-placed — the card was placed this turn, triggering
 *               rule evaluation. Cleared by the orchestrator after each turn.
 *   - @c 0x08..0x40 : captured-from-direction marks (bit @c 0x08<<dir set
 *               when this slot was captured by a neighbor in direction
 *               @c dir, where dir is 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT).
 *   - @c 0x80 : matched in the Same-rule pass-1 (matching edge value).
 *   - @c 0x100 : involved in a Plus-rule combo this turn (set on both the
 *               placer and the captured neighbors).
 */
typedef struct {
    /* 0x00 */ u16 flags;       /**< See @c TT_CELL_* flag table below. */
    /* 0x02 */ u8  cardId;      /**< Index into @c g_tripleTriadCardStats. */
    /* 0x03 */ u8  entityIdx;   /**< @c g_tripleTriadCardHands slot index (card object) driving this cell's animation. */
    /* 0x04 */ u8  owner;       /**< Player 0 or 1. */
    /* 0x05 */ u8  element;     /**< Cell element bitmask (board-tile element; 0 = none). */
    /* 0x06 */ s8  elementMod;  /**< FF8 Elemental rule: +1/-1 added to each edge if card's
                                     element matches/differs from cell's element. */
    /* 0x07 */ u8  pad07;
} TripleTriadBoardSlot;

/** @brief Bits in @c TripleTriadBoardSlot.flags. */
#define TT_CELL_WALL          0x0001  /**< Sentinel border slot (rank-A vs wall triggers Same-Wall). */
#define TT_CELL_OCCUPIED      0x0002  /**< A card has been placed in this slot. */
#define TT_CELL_JUST_PLACED   0x0004  /**< Placed this turn; triggers rule evaluation. */
#define TT_CELL_CAP_FROM_BASE 0x0008  /**< Shift left by @c dir (0..3) for captured-from-direction bit. */
#define TT_CELL_CAP_FROM_UP   0x0008
#define TT_CELL_CAP_FROM_DOWN 0x0010
#define TT_CELL_CAP_FROM_LEFT 0x0020
#define TT_CELL_CAP_FROM_RIGHT 0x0040
#define TT_CELL_CAP_FROM_MASK 0x0078  /**< All four captured-from-direction bits (UP|DOWN|LEFT|RIGHT). */
#define TT_CELL_SAME_MATCHED  0x0080  /**< Matched in Same-rule pass 1, or "Same fired here" on the placer. */
#define TT_CELL_PLUS_COMBO    0x0100  /**< Involved in a Plus-rule combo this turn. */

/* applyCardRules() return flags: which combo rule captured cards this turn. */
#define TT_RESULT_SAME_FIRED  0x04   /**< Same rule fired. */
#define TT_RESULT_PLUS_FIRED  0x08   /**< Plus rule fired. */
#define TT_RESULT_COMBO_MASK  0x0C   /**< Same | Plus — a combo fired (run the capture sweep). */

/**
 * @brief Per-card stat block (8 bytes) — one entry of @c g_tripleTriadCardStats.
 *
 * The four @c sides values are the edge ranks (1..10, where rank 10 = "A")
 * stored in the order {top, bottom, left, right}. This ordering is chosen
 * specifically so @c i^1 yields the opposing edge: 0(top)↔1(bottom),
 * 2(left)↔3(right). The same ordering is used by @c TripleTriadDirection,
 * letting rule evaluators compare @c myCard.sides[dir] against
 * @c neighborCard.sides[dir^1] without a lookup table.
 */
typedef struct {
    /* 0x00 */ u8 sides[4];   /**< Edge ranks 1..10, in {top, bottom, left, right} order. */
    /* 0x04 */ u8 element;     /**< Card element bitmask (0 = none). */
    /* 0x05 */ u8 pad05[3];    /**< Level / other per-card metadata. */
} TripleTriadCard;

/**
 * @brief 4-cardinal neighbor offset (4 bytes).
 *
 * One entry of @c g_tripleTriadDirectionOffsets, which holds the four
 * cardinal direction vectors in this exact order:
 *   index 0: UP    (0, -1)
 *   index 1: DOWN  (0, +1)
 *   index 2: LEFT  (-1, 0)
 *   index 3: RIGHT (+1, 0)
 *
 * The pairing is deliberate: @c i^1 flips a direction to its opposite
 * (0↔1, 2↔3), matching the @c TripleTriadCard.sides[] layout so that
 * "my edge facing direction @c i" lines up with "my neighbor's edge
 * facing direction @c i^1".
 */
typedef struct {
    /* 0x00 */ s16 dx;        /**< Column delta. */
    /* 0x02 */ s16 dy;        /**< Row delta. */
} TripleTriadDirection;

/**
 * @brief One bucket in the Plus-rule edge-sum histogram (2 bytes).
 *
 * Used only inside @c applyPlusRule. After a placed card is identified,
 * one bucket per possible edge sum (0..20) accumulates how many of the
 * four neighbors produced that sum. The Plus rule fires when any bucket
 * reaches @c count >= 2, at which point @c dirMask tells which neighbor
 * directions to flip.
 */
typedef struct {
    /* 0x00 */ u8 count;      /**< Number of neighbors that hit this edge sum. */
    /* 0x01 */ u8 dirMask;    /**< Bitmask of which directions (bit @c i set if dir @c i hit). */
} TripleTriadPlusBucket;

/** @brief Number of columns per row, including the 1-cell sentinel border. */
#define TT_BOARD_COLS  5
/** @brief Number of rows in the board, including sentinel borders. */
#define TT_BOARD_ROWS  5

/**
 * @brief Full 5x5 Triple Triad board (rows × cols, 200 bytes total).
 *
 * The active play area is at rows/cols 1..3; the 0th and 4th rows/cols
 * are sentinel slots with @c flags=0 used to keep neighbor lookups
 * branch-free at the edges of the play area.
 */
typedef struct {
    /* 0x00 */ TripleTriadBoardSlot cells[TT_BOARD_ROWS][TT_BOARD_COLS];
} TripleTriadBoard;

/** @brief Byte strides for addressing the board by raw offset (see @c drawBoardElements). */
#define TT_SLOT_BYTES ((s32)sizeof(TripleTriadBoardSlot))   /**< One board cell. */
#define TT_ROW_BYTES  (TT_SLOT_BYTES * TT_BOARD_COLS)        /**< One board row. */

/** @brief Global 5x5 Triple Triad board (sentinel-padded). */
extern TripleTriadBoard g_tripleTriadBoard;

/**
 * @brief A Triple Triad card in play (36 bytes) — one entry of
 *        @c g_tripleTriadCardHands, the two players' 5-card hands (10 total).
 *
 * This is the runtime, on-screen representation of a single card: it carries
 * the card's identity, owning seat, hand slot, and animation/render state.
 * The static per-card stats (edge ranks + element) live separately in
 * @c TripleTriadCard / @c g_tripleTriadCardStats, indexed by @c cardId.
 *
 * The be_object2 dispatch layer drives each object by @c groupId (category)
 * and @c priority: setting a new high-priority entry in a group resets all
 * lower-priority siblings. When the card is placed (@ref commitCardToBoard), its
 * @c cardId and owner are copied onto the board and the board cell's
 * @c entityIdx points back to this object so its flip animation plays.
 */
typedef struct {
    /* 0x00 */ u8  cardId;       /**< Card id — index into @c g_tripleTriadCardStats. */
    /* 0x01 */ u8  state;        /**< Effect-animation state — a @ref CardEffectState. */
    /* 0x02 */ s16 field02;      /**< Cleared whenever @c state is written. */
    /* 0x04 */ u16 flags;        /**< Bit 0x2 = rotating CW (consumed by handler). */
    /* 0x06 */ s16 angle;        /**< Current animation angle (clamped to 0..0x1000). */
    /* 0x08 */ s32 initFlags;    /**< Init-time flag word; bit 0 (@c TT_OWNER_MASK) = owning seat. */
    /* 0x0C */ u8  groupId;      // 0 = in cpu hand, 1 = in player hand, 2 = on board, 3 = left of board
    /* 0x0D */ u8  fieldD;       /**< Secondary index (e.g. board column). */
    /* 0x0E */ u8  priority;     /**< Slot priority within its group / hand slot 0..4
                                       (also doubles as a row index in some handlers). */
    /* 0x0F */ u8  pad0F;
    /* 0x10 */ u8  param0;       /**< Action parameter 0. */
    /* 0x11 */ u8  param1;       /**< Action parameter 1. */
    /* 0x12 */ u8  param2;       /**< Action parameter 2. */
    /* 0x13 */ u8  pad13;
    /* 0x14 */ s16 posData[2];   /**< Local rotation source passed to @c RotMatrixYXZ. */
    /* 0x18 */ s16 field18;
    /* 0x1A */ s16 field1A;
    /* 0x1C */ s16 offX;         /**< Added to node @c baseX to produce world X. */
    /* 0x1E */ s16 offY;
    /* 0x20 */ s16 offZ;
    /* 0x22 */ s16 offSort;      /**< Added to node @c sortKey. */
} TripleTriadCardObject; /* 36 bytes */

/**
 * @brief @c TripleTriadCardObject.state values — the per-card effect animation
 *        driven each frame by @c animateCardEffect / @c transformCardEffect.
 *
 * The four @c CARD_FX_SLIDE_* directions are contiguous (2..5) because the
 * handler does @c state-=2 and indexes the @c D_80182D10 motion-vector table
 * (LEFT, UP, DOWN, RIGHT). @c setCardEntityType plays the capture SFX for any
 * value in that range.
 */
typedef enum {
    CARD_FX_IDLE        = 0,  /**< No per-frame update. */
    CARD_FX_FLIP        = 1,  /**< Open / flip-over sequence. */
    CARD_FX_SLIDE_LEFT  = 2,  /**< Directional capture, slides toward -X. */
    CARD_FX_SLIDE_UP    = 3,  /**< Directional capture, slides toward -Y. */
    CARD_FX_SLIDE_DOWN  = 4,  /**< Directional capture, slides toward +Y. */
    CARD_FX_SLIDE_RIGHT = 5,  /**< Directional capture, slides toward +X. */
    CARD_FX_FLASH       = 6,  /**< Translucent gouraud-quad flash overlay (~20 frames); no position update. */
    CARD_FX_SHAKE       = 7,  /**< Short shake/twitch, then clears and advances priority. */
} CardEffectState;

/** @brief Bits in @c TripleTriadCardObject.flags. */
#define TT_CARD_ACTIVE    0x0001  /**< Active/selectable card (filtered by @c findCardSlot). */
#define TT_CARD_ROTATE_CW 0x0002  /**< Spin clockwise; set to highlight the card. */

/** @brief @c TripleTriadCardObject.groupId — which collection a card object is in. */
typedef enum {
    TT_GROUP_CPU_HAND    = 0,  /**< In the CPU's hand.    */
    TT_GROUP_PLAYER_HAND = 1,  /**< In the player's hand. */
    TT_GROUP_BOARD       = 2,  /**< Placed on the board.  */
    TT_GROUP_LEFT        = 3,  /**< Left of the board.    */
} TripleTriadCardGroup;

/** @brief The two players' hands as card objects (5 each, 10 total). */
extern TripleTriadCardObject g_tripleTriadCardHands[10];

/** @brief The two players' 5-card hands as raw card ids (match-state region @ 0x801A2C40). */
extern u8 D_801A2C48[2][5];
/* func_80023D04 / modifyItemQuantity prototypes live in item.h (their owner). */

/**
 * @brief One slot of a player's working hand for the AI search (8 bytes).
 *
 * The minimax (@ref searchBestMoveStack) and board evaluator (@ref evaluateBoard)
 * read these card ids; @c id == 0xFF marks a slot whose card has been played
 * (or is otherwise unavailable) during the search.
 */
typedef struct {
    /* 0x00 */ u8  id;          /**< Card id; 0xFF = played/unavailable. */
    /* 0x01 */ u8  pad01[3];
    /* 0x04 */ s32 entityIdx;   /**< Index into @c g_tripleTriadCardHands — the on-screen
                                     card object animated/placed when this hand card is chosen. */
} AiHandCard;                   /* 0x08 */

typedef struct {
    /* 0x00 */ AiHandCard cards[5];
} PlayerHand;                /* 0x28 */

/** @brief The two players' working card-id hands for the AI search. */
extern PlayerHand g_tripleTriadPlayerHands[2];

/* Triple Triad AI board-evaluation weights (read by @ref evaluateBoard). */
extern s32 g_evalCardBaseWeight;    /**< Base weight added to each placed card's value. */
extern s32 g_evalCardValueScale;    /**< Per-card value scale: g_tripleTriadCardValues[i] = level*this/200 >> 12. */
extern s32 g_evalRandomRange;    /**< Random-tiebreaker range: score += rand() % (g_evalRandomRange + 1). */
extern s32 g_evalDifficultyWeight;    /**< AI difficulty weight (set from the per-level weight table). */
extern s32 g_evalHandPotentialWeight;    /**< Hand-card potential weight (cardValue * g_evalHandPotentialWeight >> 12). */
extern s32 g_tripleTriadCardValues[];  /**< Per-card value table, indexed by card id. */

/**
 * @brief 40-byte animation work node allocated by @c scratchAlloc.
 *
 * The first 0x20 bytes are a @c MATRIX (so the GTE setup calls take it
 * directly): @c RotMatrixYXZ fills the rotation @c mat.m, and the translation
 * @c mat.t[0..2] holds the element's world X/Y/Z. The low 16 bits of
 * @c mat.t[0]/@c mat.t[1] double as the screen-space sprite anchor read by
 * @c drawCardOverlaySprite. The trailing @c base is the pre-transform position written
 * by @c layoutCardSlot (its @c pad slot carries the display-list sort key).
 *
 * Per-frame: @c layoutCardSlot writes @c base, then the caller adds
 * @c TripleTriadCardObject.offX/Y/Z into @c mat.t[0..2] and @c offSort into @c base.pad.
 */
typedef struct {
    /* 0x00 */ MATRIX  mat;   /**< Transform: rotation + translation (t[0..2] = world X/Y/Z). */
    /* 0x20 */ SVECTOR base;  /**< Base position from @c layoutCardSlot; @c pad = OT sort key. */
} CardAnimNode;             /* 40 bytes */

/**
 * @brief Per-frame handler context wrapping a @c TripleTriadCardObject.
 *
 * Allocated by @c func_80098C44 with the per-frame callback (e.g.
 * @c updateCardObject) stored at offset @c 0x08. Different handlers in
 * this overlay reuse the node's @c 0x0C slot for different purposes;
 * the be_object2 dispatch stores a back-pointer to the @c TripleTriadCardObject
 * entry being driven.
 */
typedef struct {
    /* 0x00 */ u8           pad00[0x0C];
    /* 0x0C */ TripleTriadCardObject *entry;
} CardObjectCtl;

/* drawCardOverlaySprite / animateCardEffect / transformCardEffect prototypes
   live in be_object2.h (their owner). */

/**
 * @brief 60-byte work buffer staged by @c scratchAlloc for one card
 *        render pass (used by @c drawTriadCard and related helpers).
 *
 * Holds the 4 digit-corner SVECTORs computed for each rank inside the
 * digit loop, the 4 transformed screen positions of the card outline
 * (from @c RotTransPers4), and the GTE P/flag outputs.
 */
typedef struct {
    /* 0x00 */ SVECTOR digitVerts[4];   /**< Per-rank digit quad corners. */
    /* 0x20 */ s32     outXY[4];         /**< Packed @c (s16 x, s16 y) screen verts. */
    /* 0x30 */ s32     P;                /**< RotTransPers4 perspective output. */
    /* 0x34 */ s32     flag;             /**< RotTransPers4 flag output. */
    /* 0x38 */ u8      pad38[4];
} CardRenderWork;                        /* 60 bytes */

/** @brief 4-cardinal direction indices into @c g_tripleTriadDirectionOffsets and
 *         @c TripleTriadCard.sides[]. The pairing @c dir^1 yields the opposite. */
typedef enum {
    TT_DIR_UP    = 0,
    TT_DIR_DOWN  = 1,
    TT_DIR_LEFT  = 2,
    TT_DIR_RIGHT = 3,
    TT_DIR_COUNT = 4
} TripleTriadDir;

/** @brief Rank value that triggers the Same-Wall rule when facing a wall. */
#define TT_RANK_A          0x0A

/** @brief Low bit of @c TripleTriadCardObject.initFlags / a card's owner: the owning
 *         seat (player 0 or 1). This is the seat index only — whether a seat
 *         is human, AI, or demo is the separate per-seat "player type". */
#define TT_OWNER_MASK            0x01 // Blue or pink
#define TT_USE_STATS             0x02 // Show the card's rank digits (drawTriadCard)
#define TT_SHOW_LIGHT_OVERLAY    0x04 // Card gets light purple/blue overlay depending on what bit 1 is set to
#define TT_SHOW_ELEMENT          0x10 // Draw the card's element marker (drawTriadCard)
#define TT_SHOW_DARK_OVERLAY     0x20 // Card turns dark blue


/** @brief Bits in @c g_tripleTriadRules controlling which optional rules are active. */
#define TT_RULE_OPEN       0x01   /**< Open rule: both hands are shown face-up. */
#define TT_RULE_SAME       0x02   /**< Same rule enabled. */
#define TT_RULE_PLUS       0x04   /**< Plus rule enabled. */
#define TT_RULE_RANDOM     0x08   /**< Random rule: the hand is auto-dealt rather than picked. */
#define TT_RULE_SUDDEN_DEATH 0x10 /**< Sudden Death: a drawn match replays the hand with current ownership. */
#define TT_RULE_SAME_WALL  0x40   /**< Same-Wall extension (A facing wall counts as a match). */
#define TT_RULE_ELEMENTAL  0x80   /**< FF8 Elemental rule (tile elements give +1/-1 edge modifiers). */
#define TT_RULE_NO_CLAIM   0x20000000 /**< Engine flag (high bit): no card is won/lost after the match
                                           (the post-match claim sweep is skipped); the hand is auto-dealt. */

/** @brief Bits in @c g_tripleTriadInputFlags. */
#define TT_INPUT_DISABLED   0x04  /**< Card input suspended while a modal sub-screen is active. */
#define TT_INPUT_HAND_BUILD 0x08  /**< Hand-building input active. */

/** @brief Values of @c g_tripleTriadState — selects the next per-phase handler in
 *  @c g_tripleTriadStateHandlers. The dispatch loop calls the handler until it
 *  returns non-zero (-> @c TT_STATE_IDLE) or reaches @c TT_STATE_EXIT. */
typedef enum {
    TT_STATE_IDLE       = 0,  /**< No pending handler. */
    TT_STATE_INIT       = 1,  /**< Startup render-list init (set only by initTripleTriad). */
    TT_STATE_SCRIPT     = 2,  /**< Script-handler phase; replay re-entry point. */
    TT_STATE_PLAY       = 3,  /**< Match play (per-frame update callbacks). */
    TT_STATE_CARD_CLAIM = 4,  /**< Post-match card claim (set after the win/loss tally). */
    TT_STATE_RESTART    = 5,  /**< Redirects to TT_STATE_SCRIPT; no decompiled setter found. */
    TT_STATE_EXIT       = 6   /**< Terminate the main loop. */
} TripleTriadState;

extern TripleTriadCard      g_tripleTriadCardStats[];          /**< Card stats table (~110 cards). */
extern TripleTriadDirection g_tripleTriadDirectionOffsets[4];  /**< UP, DOWN, LEFT, RIGHT (see TripleTriadDirection). */
extern s32                  g_tripleTriadRules;                /**< Active rule flags (TT_RULE_*). */

/** @brief Latch @ref D_80082C90 encounter params into the Triple Triad globals (be_dispatch.c). */
extern void func_800981BC(void);

/* ── Object-list system ──────────────────────────────────────────────────────
 * A per-frame callback list whose nodes are carved from a fixed pool. Every
 * be_objectN TU drives its objects through this (initObjList / allocObjNode /
 * updateObjectList in be_object1.c); the type-specific node structs all extend
 * @c ObjListNode. */

/** @brief Per-frame node callback; a return value with bit 1 set unlinks the node.
 *  Concrete callbacks take their own node-struct pointer type, so call sites cast
 *  to this canonical type when storing them. */
typedef s32 (*ObjNodeFn)(struct ObjListNode *node);

/** @brief Generic pool-backed list-node header (0xC bytes). Type-specific nodes
 *  (e.g. @c HandlerNode) extend this; the pool allocator/iterator only touches
 *  this header. */
typedef struct ObjListNode {
    u16                 flags;     /* 0x0 — bit 0 = active/allocated */
    u16                 field02;   /* 0x2 — cleared on allocation */
    struct ObjListNode *next;      /* 0x4 — next node in the list */
    ObjNodeFn           callback;  /* 0x8 — per-frame callback */
} ObjListNode;

/** @brief Pool-backed singly-linked list header (0x10 bytes). Nodes are carved
 *  from a fixed @c pool of @c count entries, each @c stride bytes. */
typedef struct {
    ObjListNode *head;    /* 0x0 */
    ObjListNode *tail;    /* 0x4 */
    void        *pool;    /* 0x8 — node-pool base */
    s16          stride;  /* 0xC — node size in bytes */
    s16          count;   /* 0xE — node count */
} ObjList;

/* ── Shared overlay globals ──────────────────────────────────────────────────
 * Referenced across the be_objectN translation units of the tripletriad overlay. */
extern s32           g_cardFlipPhase;       /**< Current seat / phase latched at the idle->flip handoff (0/1; -1 = not started). */
extern volatile s32  g_tripleTriadFrameCount;       /**< Free-running frame counter (volatile forces lw, not lbu). */
extern s32           g_tripleTriadInputFlags;       /**< Input-state flags (TT_INPUT_*). */
extern u8            g_tripleTriadState;        /**< Current phase / next handler to dispatch (TripleTriadState). */
extern u8            g_drawBufferIndex;        /**< Active double-buffer index. */
extern DRAWENV      *g_activeDrawEnv;   /**< Draw env of the buffer currently being built. */
extern ObjList            D_801D3028[];      /**< Per-frame update callback list header. */
extern u8            D_801D3038[];      /**< Backing node pool for D_801D3028. */
extern u8            D_801D30FC;        /**< Match winner (0/1, or 2 = draw); also the claim seat. */
#define TT_WINNER_DRAW 2                 /**< @c D_801D30FC value when neither seat won. */
extern u8            D_8012E66C[];      /**< Vblank flip callback. */
extern ObjList            g_taskList[];      /**< Card-claim/AI shared scratch (be_object2/3/4). */

/* Per-pad button-mask buffers (held / pressed / repeat), seeded by @c sampleInput.
   Indexed by the menu pad source (0/1), or OR'd across both; [2] is the
   result-screen pad input. */
extern u16 g_padRepeat[];    /**< Auto-repeat mask. */
extern u16 g_padPressed[];   /**< Newly-pressed (rising-edge) mask; [2] is the result-screen pad input. */
extern u16 g_padHeld[];      /**< Held (currently-down) mask. */

/** @brief Display-list OT base — points at the active buffer's OT
 *         (@c g_orderingTables[g_drawBufferIndex]); index by sort key. */
extern u32  *g_otBase;
extern void *g_primCursor;   /**< Current primitive-pool tail (advanced by display helpers). */

extern u8    D_801A2C70[2];  /**< Per-player layout type; 3 selects the offset-hand layout. */

/**
 * @brief Per-substate parameter slot (4 bytes). One entry of @c D_801D3340 —
 * each substate handler interprets its two halfwords differently (some use only
 * @c field0, some only @c field2; the board substate uses both).
 */
typedef struct {
    /* 0x00 */ s16 field0;
    /* 0x02 */ s16 field2;
} SubstateSlot;

extern SubstateSlot D_801D3340[6]; /**< Per-substate parameter table (one slot per substate 0..5). */
extern u8           g_substatePhase;    /**< Substate completion phase (see TriadSubstatePhase). */
extern SubstateSlot D_801D335C;    /**< 4-byte snapshot of @c D_801D3340[g_activeSubstate]. */

/* Cross-TU function prototypes live in the owning module's header
   (be_object1.h .. be_object4.h), not here. */

#endif /* TRIPLETRIAD_H */
