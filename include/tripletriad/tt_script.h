#ifndef TRIPLETRIAD_BE_OBJECT3_H
#define TRIPLETRIAD_BE_OBJECT3_H

#include "common.h"
#include "tripletriad.h"

/* Declarations for tt_script.c (Triple Triad card-claim / hand-build sequence,
   the script and setup handlers, the per-card slide/scale animations, and the
   full-screen colour-fade effects). */

/* ───────────────────────────── Public ──────────────────────────────────── */

/* Public typedefs */

/**
 * @brief State-machine driver node (state byte at 0x0C).
 *
 * Used by the claim handlers @ref runKeepCardSelect / @ref runAiCaptureSelect /
 * @ref replayHandMoves / @ref runOpponentSideSweep / @ref runCaptureCleanupSweep and spawned by
 * the card-claim controller updateClaimController (in tt_claim.c).
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x0C];
    /* 0x0C */ u8 state;
    /* 0x0D */ u8 field0D;
    /* 0x0E */ u8 field0E;   /**< Side; matched against an object's 0x0A field XOR 1. */
    /* 0x0F */ u8 index;     /**< Per-tick iteration index. */
    /* 0x10 */ u8 pad10[0x12];
    /* 0x22 */ u8 field22;   /**< Pre-set "already finished" flag: updateCardColorFade returns 2 immediately when this is 1. */
} ScriptStateNode;

/** @brief One 0x20-byte entry of the @ref g_activeCardObjs active-object array. */
typedef struct {
    /* 0x00 */ u8  cardId;   /**< Index into g_tripleTriadCardStats. */
    /* 0x01 */ u8  pad01[3];
    /* 0x04 */ s32 flags;    /**< Bit 0 set = active/pending. */
    /* 0x08 */ u8  field8;
    /* 0x09 */ u8  field9;
    /* 0x0A */ u8  fieldA;   /**< Owning side. */
    /* 0x0B */ u8  pad0B[5];
    /* 0x10 */ s16 rotX;     /**< Rotation vector X (passed to RotMatrixYXZ). */
    /* 0x12 */ s16 rotY;     /**< Rotation vector Y (card-flip angle). */
    /* 0x14 */ s16 rotZ;     /**< Rotation vector Z. */
    /* 0x16 */ u8  pad16[2];
    /* 0x18 */ s16 baseX;    /**< Board position fed to the GTE translation. */
    /* 0x1A */ s16 baseY;
    /* 0x1C */ s16 baseZ;
    /* 0x1E */ s16 sort;     /**< OT sort-offset (added to g_otBase base). */
} ActiveObj; /* 0x20 */

/* Public data */
extern ActiveObj g_activeCardObjs[]; /**< Active card-display object array. */

/* Shared by the tt_claim tail (the card-claim controller/setup). */
extern s32 g_sweepTarget;   /**< Claim selector set at setup (>=0 normal, -1 capture-only, <-1 skip);
                                 also the sweep's target object count (it completes when g_sweepProcessed reaches it). */
extern u8  D_801D444C;      /**< Set when the phase-1 claim handler finishes. */
extern u8  g_sweepDone;     /**< Set when the phase-2 cleanup handler finishes. */
extern s32 g_claimSeat;     /**< Acting seat index (0 or 1) for the capture/cleanup sweeps. */
extern s32 D_801D4454;      /**< Cleared at the start of each card-claim setup. */
extern u8  g_claimSetupPool[];  /**< Backing element storage for the D_801D42F8 pool. */

/* Public prototypes */
extern void updateFadeEffects(void);

extern s32  spawnTaskNode(ObjNodeFn callback);
extern void spawnGradientFade(s32 a0);
extern void setNodeDoneFlag(u8 *a0, s32 a1);
extern void initGradientFadeList(void);

/** @brief Spawn a full-screen fade to black over @p duration frames. */
extern void startFadeToBlack(s32 duration);
/** @brief Spawn a full-screen fade to white over @p duration frames. */
extern void startFadeToWhite(s32 duration);
/** @brief Per-frame board render/update loop. */
extern s32  updateClaimBoard(void);
extern s32  reloadClaimBuffer(void);

/* Card-claim script-state handlers (driven by the tt_claim controller). */
extern s32  runKeepCardSelect(ScriptStateNode *node);
extern s32  runAiCaptureSelect(ScriptStateNode *node);
extern s32  replayHandMoves(ScriptStateNode *node);
extern s32  runOpponentSideSweep(ScriptStateNode *node);
extern s32  runCaptureCleanupSweep(ScriptStateNode *node);

/* ───────── Private (only used in tt_script.c; may move into the .c) ─────── */

/* Private enums / defines / consts */

/** @brief Phase of the card scale-in/hold/out animation (@c ScriptStateNode.state in
 *         @ref updateCardScaleSprite / @ref updateCardScaleSpriteShort). */
enum CardScalePhase {
    CARD_SCALE_IN   = 0,  /**< Scale the sprite width up. */
    CARD_SCALE_HOLD = 1,  /**< Hold at full width. */
    CARD_SCALE_OUT  = 2   /**< Scale back down, then finish. */
};

/** @brief Phase of the card colour fade (@c ScriptStateNode.state in @ref updateCardColorFade). */
enum CardColorFadePhase {
    CARD_FADE_IN  = 0,  /**< Fade the colour up. */
    CARD_FADE_OUT = 1   /**< Fade down, then hold solid. */
};

/** @brief @c ScriptEntry.status — the per-card animation queued in @c g_scriptActions,
 *         read by @ref updateScriptCardAnims and written by the hand/script builders. */
enum ScriptCardAnim {
    SCRIPT_ANIM_NONE     = 0,  /**< Idle / no animation. */
    SCRIPT_ANIM_SLIDE_IN = 1,  /**< Slide the card onto the board. */
    SCRIPT_ANIM_SLIDE_OUT= 2,  /**< Slide the card off. */
    SCRIPT_ANIM_FLIP     = 3   /**< Flip the card (also the "complete" marker). */
};

/** @brief @c ActiveObj.field8 — the per-cell claim animation, read by @ref updateClaimBoard
 *         and written by the claim/capture sweeps. */
enum ClaimCellAnim {
    CLAIM_ANIM_NONE      = 0,  /**< No animation pending. */
    CLAIM_ANIM_SLIDE_IN  = 1,  /**< Slide the card in from off-screen. */
    CLAIM_ANIM_FLIP      = 2,  /**< Flip (toggle owner at the halfway point). */
    CLAIM_ANIM_CAPTURE_A = 3,  /**< Capture lift-and-fade, direction A. */
    CLAIM_ANIM_CAPTURE_B = 4   /**< Capture lift-and-fade, direction B. */
};

/** @brief @c ScriptCtx.state of the interactive hand-build sequencer (@ref runHandBuildSequencer). */
enum HandBuildState {
    HAND_BUILD_INIT    = 0,  /**< Clear the hand slots and arm the UI. */
    HAND_BUILD_PICK    = 1,  /**< Add/remove a card per frame until five are held. */
    HAND_BUILD_CONFIRM = 2,  /**< Poll the confirm message gate. */
    HAND_BUILD_FADE    = 3   /**< Run the fade-out, then finish. */
};

/** @brief @c ScriptCtx.state of the match setup sequence (@ref runTriadSetupSequence). */
enum TriadSetupState {
    TRIAD_SETUP_WARMUP = 0,  /**< Fade-in warmup. */
    TRIAD_SETUP_HANDS  = 1,  /**< Per-player hand setup. */
    TRIAD_SETUP_CLEAR  = 2,  /**< Clear-sweep the board columns. */
    TRIAD_SETUP_WAIT   = 3,  /**< Idle wait. */
    TRIAD_SETUP_DONE   = 4   /**< Hand off to TT_STATE_PLAY. */
};

/** @brief @c ScriptStateNode.state of the keep-a-captured-card selection (@ref runKeepCardSelect). */
enum KeepSelectState {
    KEEP_SEL_BANNER  = 0,  /**< Build and show the banner string. */
    KEEP_SEL_PICK    = 1,  /**< Run the card picker. */
    KEEP_SEL_CONFIRM = 2   /**< Poll the confirm gate; commit or return to the picker. */
};

/** @brief Two-phase init/run sweep state shared by @ref runAiCaptureSelect,
 *         @ref replayHandMoves, and @ref runOpponentSideSweep (@c ScriptStateNode.state). */
enum SweepState {
    SWEEP_INIT = 0,  /**< Reset the counter/index. */
    SWEEP_RUN  = 1   /**< Do one unit of work per tick until done. */
};

/** @brief @c ScriptStateNode.state of the post-round capture/cleanup sweep
 *         (@ref runCaptureCleanupSweep). */
enum CleanupState {
    CLEANUP_WAIT      = 0,  /**< Wait for the action gate. */
    CLEANUP_SWEEP_OPP = 1,  /**< Flag the opponent's row for capture. */
    CLEANUP_SWEEP_OWN = 2,  /**< Flag the player's own row. */
    CLEANUP_COLLECT   = 3   /**< Return all own cards to the collection, then finish. */
};

/* Private typedefs */

/**
 * @brief Script-action entry in the @ref g_scriptActions 2x5 table.
 *
 * Initialized in initTripleTriadScripts. Used by runTriadSetupSequence's state-2 sweep
 * to mark queued actions complete and by hasPendingScriptAction to scan for pending actions.
 */
typedef struct {
    /* 0x00 */ u8 marker;       /**< Sentinel; set to 0xFF on init. */
    /* 0x01 */ u8 status;       /**< Action status (set to 3 to mark "complete"). */
    /* 0x02 */ u8 field02;      /**< Cleared when action marked complete; animation frame counter in updateScriptCardAnims. */
    /* 0x03 */ u8 row;          /**< Row index (cached on init). */
    /* 0x04 */ u8 col;          /**< Column index (cached on init). */
    /* 0x05 */ u8 pad05;
    /* 0x06 */ u16 field06;     /**< Rotation-vector base fed to RotMatrixYXZ (rotX); rotY at 0x08, rotZ at 0x0A. */
    /* 0x08 */ s16 actionId;    /**< Pending action ID; also the card-flip Y-rotation in updateScriptCardAnims. */
    /* 0x0A */ u16 field0A;
    /* 0x0C */ u8 pad0C[2];
    /* 0x0E */ s16 posX;        /**< Screen position fed to the GTE translation (updateScriptCardAnims). */
    /* 0x10 */ s16 posY;
    /* 0x12 */ s16 posZ;
    /* 0x14 */ s16 sort;        /**< OT sort key. */
} ScriptEntry; /* 0x16 = 22 bytes */

/**
 * @brief Scratch buffer for updateScriptCardAnims's per-card matrix build (0x2C bytes).
 *
 * Allocated each frame via scratchAlloc; @c f0/@c f2 carry the card's row/col,
 * layoutCardSlot fills @c pos (position + OT sort key), and @c mtx holds the
 * rotation/translation matrix passed to SetRotMatrix / SetTransMatrix.
 */
typedef struct {
    /* 0x00 */ u8      f0;    /**< layoutCardSlot descriptor type. */
    /* 0x01 */ u8      pad1;
    /* 0x02 */ u8      f2;    /**< layoutCardSlot descriptor row. */
    /* 0x03 */ u8      pad3;
    /* 0x04 */ SVECTOR pos;   /**< Position filled by layoutCardSlot (vx/vy/vz/pad). */
    /* 0x0C */ MATRIX  mtx;
} TmpBuf; /* 0x2C */

/**
 * @brief Callback context for state-machine handlers (e.g. runTriadSetupSequence).
 *
 * Allocated via allocObjNode / allocObjNodeFront with state byte at +0x10.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x0C];
    /* 0x0C */ s32 cachedResult;
    /* 0x10 */ u8 state;
    /* 0x11 */ u8 subState;
    /* 0x12 */ u8 counter;
    /* 0x13 */ u8 field13;   /**< Cleared to 0 when runHandBuildSequencer (re)inits the sequence. */
} ScriptCtx;

/**
 * @brief Full-screen colour-fade effect object.
 *
 * Spawned by @c startFadeToBlack (fade to black) and @c startFadeToWhite (fade to
 * white), then driven each frame by @c updateScreenFade: it interpolates a
 * screen-sized sprite's colour from @c startColor toward @c endColor across
 * @c duration frames, advancing @c frame each call. When the fade completes it
 * stages @c endColor back into @c g_stagedFadeColor so the next fade starts there.
 */
typedef struct {
    /* 0x00 */ u8  pad00[0x0C];
    /* 0x0C */ s16 frame;          /**< Current frame counter (0..duration). */
    /* 0x0E */ s16 duration;       /**< Total fade length, in frames. */
    /* 0x10 */ u8  startColor[4];  /**< Starting RGB, copied from the staged g_stagedFadeColor. */
    /* 0x14 */ u8  endColor[4];    /**< Target RGB (written as a word: 0 = black, 0xFFFFFF = white). */
} FadeObject;

/**
 * @brief Card scale/fade animation sprite primitive (0x18 bytes).
 *
 * Built each frame by @c updateCardScaleSprite / @c updateCardScaleSpriteShort at the @c g_primCursor
 * cursor and linked into @c g_otBase[3]; the @c width field is animated to
 * scale the card sprite in/out.
 */
typedef struct {
    /* 0x00 */ u32 tag;       /**< OT-link tag (0x5000000). */
    /* 0x04 */ u32 code;      /**< GPU primitive command word (0xE100060E). */
    /* 0x08 */ u32 color;     /**< Packed command + RGB (0x64808080). */
    /* 0x0C */ s16 width;     /**< Animated sprite width. */
    /* 0x0E */ s16 field0E;
    /* 0x10 */ u8  field10;
    /* 0x11 */ u8  field11;
    /* 0x12 */ s16 field12;
    /* 0x14 */ s16 field14;
    /* 0x16 */ s16 field16;
} CardScaleSprite; /* 0x18 */

/** @brief g_gradFadeList-pool node (0x24 bytes) for the screen gradient-fade effect. */
typedef struct {
    /* 0x00 */ u8  pad00[0x0C];
    /* 0x0C */ u8  state;
    /* 0x0D */ u8  frame;     /**< Brightness ramp counter (0..0xFF); seeds the gradient colours. */
    /* 0x0E */ u8  variant;   /**< Gradient style (2/3/4), set from the callback-table index. */
    /* 0x0F */ u8  pad0F[0x11];
    /* 0x20 */ s16 field20;
    /* 0x22 */ u8  done;      /**< When 1 the effect is finished (buildGradientFade returns 2). */
    /* 0x23 */ u8  pad23;
} GradientFadeNode; /* 0x24 */

/* g_gradFadeCallbacks entries (tt_script.c). */
extern s32 updateCardScaleSprite(ScriptStateNode *node);
extern s32 updateCardScaleSpriteShort(ScriptStateNode *node);
extern s32 updateCardColorFade(ScriptStateNode *node);
extern s32 buildGradientFade(GradientFadeNode *node);

/**
 * @brief Two-layer screen gradient-fade primitive (0x34 bytes), built by buildGradientFade.
 *
 * A 4-vertex gradient quad spanning the play area (x 0x40..0x13F, y 0x58..0x88); the four
 * vertex colours are ramped from @c node->frame and @c pal* carry the palette level. Two of
 * these are linked per frame (different @c field0E / @c field1A blend/tpage words) for a
 * layered fade.
 * @note Exact GPU semantics of @c code* / @c pal* / @c field0E / @c field1A are estimated.
 */
typedef struct {
    /* 0x00 */ u32 tag;
    /* 0x04 */ u32 color0;
    /* 0x08 */ s16 x0;
    /* 0x0A */ s16 y0;
    /* 0x0C */ u8  code0;
    /* 0x0D */ u8  pal0;
    /* 0x0E */ s16 field0E;
    /* 0x10 */ u32 color1;
    /* 0x14 */ s16 x1;
    /* 0x16 */ s16 y1;
    /* 0x18 */ u8  code1;
    /* 0x19 */ u8  pal1;
    /* 0x1A */ s16 field1A;
    /* 0x1C */ u32 color2;
    /* 0x20 */ s16 x2;
    /* 0x22 */ s16 y2;
    /* 0x24 */ u8  code2;
    /* 0x25 */ u8  pal2;
    /* 0x28 */ u32 color3;
    /* 0x2C */ s16 x3;
    /* 0x2E */ s16 y3;
    /* 0x30 */ u8  code3;
    /* 0x31 */ u8  pal3;
} GradientFadeQuad; /* 0x34 */

/**
 * @brief 40-byte display node allocated by @c scratchAlloc and chained via the
 *        GTE matrix helpers (tt_script fade/render path).
 */
typedef struct {
    /* 0x00 */ s16 angle;        /**< Rotation/animation angle. */
    /* 0x02 */ s16 phase;        /**< Phase counter (mirror of @c g_fadePhaseMirror). */
    /* 0x04 */ s16 unk04;        /**< Always zeroed at init. */
    /* 0x06 */ u8  pad06[2];
    /* 0x08 */ u8  subNode[0x14];/**< Sub-node passed to chain calls. */
    /* 0x1C */ s32 charType;     /**< Set to @c 0x44 ('D'). */
    /* 0x20 */ s32 scale;        /**< Scale factor (computed or 0x18). */
    /* 0x24 */ s32 unk24;        /**< Set to 0x200. */
} DispNode;                      /* 0x28 bytes */

/* Private data — object pools / script tables */
extern ObjList  g_gradFadeList[];   /**< Screen gradient-fade effect list (CallbackNode nodes). */
extern u8       g_gradFadePool[];   /**< Backing element storage for the g_gradFadeList pool. */
extern ScriptEntry g_scriptActions[2][5]; /**< Per-player script-action table (initTripleTriadScripts). */
extern ObjList  g_scriptHandlerList[];   /**< Script-handler object pool. */
extern u8       g_scriptHandlerPool[];   /**< Backing element storage for the g_scriptHandlerList pool. */
extern ObjList  g_setupHandlerList[];   /**< Setup-handler object pool (setupPlayerHand). */
extern u8       g_setupHandlerPool[];   /**< Backing element storage for the g_setupHandlerList pool. */
extern ObjNodeFn g_gradFadeCallbacks[];  /**< Gradient-fade variant callback table. */

/* Private data — hand build / card config */
extern u8  D_80082C95;     /**< Card-config byte; the owned-quantity delta for built hands. */
extern u8  D_80078658[];   /**< Card rarity/type table (cards 0x4D+); used to draw rarity-filtered hands. */
extern u8  g_handBuildHands[2][5];/**< Per-player working copy of the hands (card ids), seeded from D_801A2C48 (replayHandMoves). */
extern s32 g_sweepProcessed;     /**< Count of objects processed this sweep (runAiCaptureSelect). */
extern s32 g_gradFadeCount;

/* Private data — fade / banner */
extern u8  g_stagedFadeColor[];   /**< Staged fade colour (RGB); the start colour for the next fade. */
extern s32 g_capturedCount;     /**< Running captured-card counter (runKeepCardSelect). */
extern u8  g_bannerBuf[];   /**< 256-byte global scratch buffer for the built banner string. */
extern u8  g_nameBannerBuf[];   /**< Scratch buffer for the captured-card name banner. */
extern u16 D_80182686;     /**< Relative-pointer string-table offsets (pool base 0x80182680). */
extern u16 D_8018268A;
extern u16 D_8018268E;
extern u16 D_80182692;     /**< Card-claim banner string-table offsets (pool base 0x80182680). */
extern u16 D_80182696;

/* Private data — per-frame input edge flags (refreshed at the top of runHandBuildSequencer
   from g_padRepeat[2] / g_padPressed[2]). */
extern u16 g_removeCardEdge;     /**< Edge flag A: 0x10 == remove-card request. */
extern u16 g_addCardEdge;     /**< Edge flag B: 0x40/0x80 == add-card request. */
extern s32 g_handBuildCount;     /**< Number of cards built into the active hand (0..5). */

/* Private data — display-node spawner state (updateCardDisplaySpawn fade/render path). */
extern s32 g_fadePhase;     /**< Phase counter (incrementing each frame). */
extern s32 g_fadeLastAngle;     /**< Last rotation/angle value (cached). */
extern s32 g_fadePhaseMirror;     /**< Phase mirror counter (running scale). */
extern s32 g_cardDisplaySlot;     /**< Current card-display slot index (must be < 0x6E). */
extern s32 g_lastActiveSlot;     /**< Last-active slot index (-1 = none). */

/* Private prototypes — tt_script.c internal forward declaration */
extern s32 updateScriptCardAnims(void); /**< Per-frame card slide/scale animation sweep. */

/* Prototype imported by tt_script.c / tt_claim.c. */
extern s32  func_800A20F4(s32 a0);           /**< Poll the player-input gate: <0 = still
                                                  waiting, 0 or 1 = the player's decision. */

#endif /* TRIPLETRIAD_BE_OBJECT3_H */
