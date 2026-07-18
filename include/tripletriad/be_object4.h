#ifndef TRIPLETRIAD_BE_OBJECT4_H
#define TRIPLETRIAD_BE_OBJECT4_H

#include "common.h"
#include "tripletriad.h"
#include "battle_anim.h"  /* BattleAnimState / BattleAnimEntity (func_800A29D4 prototype) */

/* Public interface of be_object4.c: the Triple Triad SFX request queue, the card-detail
   popup buffers, the rule-description string table, and the message-gate / menu helpers
   shared with be_object3 / be_object3b. */

/* ───────────────────────── Public typedefs ───────────────────────── */

/* Triple Triad SFX request queue: queueTriadSfx pushes effects here and
   flushTriadSfxQueue drains them to sndPlaySfx once per frame. */
typedef struct {
    u8  active;   /**< 0x0 — 1 when this slot holds a queued effect. */
    u8  volume;   /**< 0x1 — sndPlaySfx volume. */
    u8  pan;      /**< 0x2 — sndPlaySfx pan. */
    u8  sfxId;    /**< 0x3 — sndPlaySfx sound id. */
    u32 param;    /**< 0x4 — sndPlaySfx addr; also the per-effect voice-channel
                       mask used to drop overlapping requests. */
    u8  pad8[4];  /**< 0x8 */
} SfxQueueEntry;  /* 0xC */

/** @brief Typed view of the Triple Triad rule-description string table @ 0x80182680.
 *
 * One shared block (the be_object3 card-claim banners read its earlier slots): a u16
 * offset table whose entries are byte offsets from the block base to the FF8-encoded
 * rule labels packed later in the block.  func_800A24B4 recovers the base from the title
 * slot (@c &D_8018269E - 0x1E) and reaches each label as @c base + offset.  The intervening
 * @c padXX halfwords are the unused second field of each 4-byte offset record. */
typedef struct {
    /* 0x00 */ u16 pad00;
    /* 0x02 */ u16 sameOrPlus0;     /**< Lead-in control code for the Same/Plus clause. */
    /* 0x04 */ u16 pad04[13];
    /* 0x1E */ u16 title;           /**< "Rules" (carved as D_8018269E). */
    /* 0x20 */ u16 pad20;
    /* 0x22 */ u16 sameOrPlus1;     /**< ": " separator before the Same/Plus list. */
    /* 0x24 */ u16 pad24;
    /* 0x26 */ u16 plusConj;        /**< "," between listed Same/Plus terms (carved as D_801826A6). */
    /* 0x28 */ u16 pad28[3];
    /* 0x2E */ u16 openStr;         /**< ": Open"         (TT_RULE_OPEN). */
    /* 0x30 */ u16 pad30;
    /* 0x32 */ u16 randomStr;       /**< ": Random"       (TT_RULE_RANDOM). */
    /* 0x34 */ u16 pad34;
    /* 0x36 */ u16 suddenDeathStr;  /**< ": Sudden Death" (TT_RULE_SUDDEN_DEATH). */
    /* 0x38 */ u16 pad38;
    /* 0x3A */ u16 sameStr;         /**< "Same"           (TT_RULE_SAME). */
    /* 0x3C */ u16 pad3C;
    /* 0x3E */ u16 plusStr;         /**< "Plus"           (TT_RULE_PLUS). */
} RuleStrTable;

/* ───────────────────────── Public data ───────────────────────── */

extern SfxQueueEntry D_801D4500[];  /**< SFX request queue (up to 7 pending entries). */
extern s32 D_801D4560;              /**< Number of queued SFX requests. */

extern u8 g_cardDetailMsg[];    /**< Work buffer for the card-detail popup message (built by showCardDetail). */
extern u8 g_cardDetailSuffix[]; /**< String appended after the card name in the detail message. */

extern u8 g_tripleTriadRuleStrings[]; /**< Serialized rule/banner string block: u16 offset
                                           table + FF8-encoded labels; the offsets are
                                           block-base-relative (be_rule_strings.c). */

/* Rule-string table @ 0x80182680 (see RuleStrTable): individual u16 offset slots, each
   a linker alias into g_tripleTriadRuleStrings; the block base is recovered as
   @c &<slot> - <slot offset>. */
extern u16 D_8018269E;   /**< Rule-table title slot; the table base is @c &D_8018269E - 0x1E (0x80182680). */
extern u16 D_801826A6;   /**< Rule-table offset: "," conjunction (table + 0x26). */
extern u16 D_801826C2;   /**< Rule-table offset: "Same Wall"     (table + 0x42). */
extern u16 D_801826C6;   /**< Rule-table offset: ": Elemental"   (table + 0x46). */
extern u16 D_801826CA;   /**< Rule-table offset: ": Trade Rule", followed by the trade-rule names (table + 0x4A). */
extern u16 D_801826E2;   /**< Rule-table offset: post-game "Play / Quit" suffix (table + 0x62; used by func_800A1D68/updatePlayQuitPrompt). */

/* ───────────────────────── Public prototypes ───────────────────────── */

/** @brief TT_STATE_INIT handler: build the render list (g_tripleTriadStateHandlers entry). */
extern u8 *initTripleTriadRenderList(void);
/** @brief Build the active Triple Triad rules description string into @p dst. */
extern void func_800A24B4(u8 *dst);
/** @brief Queue a Triple Triad sound effect (center pan, full volume). */
extern void playTriadSfx(s32 sfxId);
/** @brief Queue a Triple Triad sound effect with a per-effect @p param. */
extern void playTriadSfxParam(s32 sfxId, s32 param);
extern void closeMenu(void);
extern void func_800A1C6C(void);
/** @brief Flush the queued Triple Triad SFX to the SPU and empty the queue. */
extern void flushTriadSfxQueue(void);
extern void clearAllSfx(void);
/** @brief Show a card's name, or build its detail popup buffer. */
extern void showCardDetail(s32 cardId);
/** @brief Open the Triple Triad in-game menu and freeze card input. */
extern void openTriadMenu(void);

/* Message-gate / banner + hand-build UI helpers (used by be_object3 / be_object3b). */
extern void func_800A1D68(s32 a0, u8 *a1, s32 a2);  /**< Show a banner/message string. */
extern void func_800A2054(s32 a0);                  /**< Acknowledge/advance a message gate. */
extern void func_800A44CC(void);   /**< Reset the hand-build UI state for a new claim sequence. */
extern void func_800A44B0(s32 a0); /**< Enable (1) / disable (0) the hand-build input prompt. */
extern void func_800A44BC(void);   /**< Tear down the claim UI at the end of the sequence. */

/* ───────────────────── be_object4-internal typedefs ───────────────────── */

/** @brief One 0x0C-byte entry of the g_sfxConfigs per-SFX configuration table. */
typedef struct {
    /* 0x00 */ u8 flags;     /**< bit0 stop/start fade (func_800A2054); bit1 offset-params, bit2 center (func_800A1D68). */
    /* 0x01 */ u8 field2F;   /**< Value written to each SFX entry's field 0x2F. */
    /* 0x02 */ u8 pitch;     /**< Pitch value. */
    /* 0x03 */ u8 fadeTimer; /**< Frame countdown; on reaching 0 the entry is faded out (see func_800A1C6C). */
    /* 0x04 */ RECT rect;    /**< Message-box rect (func_800A1D68); 0 w/h means "size to the text". */
} SfxConfig;

/** @brief The Triple Triad board view + cursor state at D_801D49C8.
 *
 * func_800A4504 initializes the whole block; func_800A4478 sets the color
 * fields; func_800A44CC resets the cursor; func_800A390C runs the per-frame
 * cursor/timer state machine. */
typedef struct {
    /* 0x00 */ RECT view;        /**< Board view rectangle (position + size). */
    /* 0x08 */ RECT work;        /**< Scratch rectangle scaled from @ref view (see func_800A343C). */
    /* 0x10 */ s32 packedColor;  /**< Packed RGB+0x64 brightness (see func_800A4478). */
    /* 0x14 */ s16 slideOffset;  /**< Fixed-point row-slide animation offset. */
    /* 0x16 */ u8 row;           /**< Current cursor row. */
    /* 0x17 */ u8 prevRow;       /**< Previous cursor row (saved on a move). */
    /* 0x18 */ s16 timer;        /**< Fixed-point step timer; fires at 0x1000. */
    /* 0x1A */ s16 timerStep;    /**< Amount added to @ref timer each frame. */
    /* 0x1C */ s16 brightness;   /**< Raw brightness (scaled by 32; see func_800A4478). */
    /* 0x1E */ s16 cursorPos;    /**< Grid position, row*11 + column. */
    /* 0x20 */ u8 unk20;
    /* 0x21 */ u8 unk21;
    /* 0x22 */ u8 state;         /**< State-machine state (0..5). */
    /* 0x23 */ u8 frameCounter;  /**< Incremented every frame. */
    /* 0x24 */ u8 unk24;
} CursorState;

/** @brief getGlyphWidthA's packed {width, height} result, held in an 8-byte stack slot. */
typedef union {
    s32 raw[2];   /**< raw[0] = the packed result word; the pair sizes the 8-byte slot. */
    s16 wh[2];    /**< wh[0] = width, wh[1] = height (overlay of raw[0]). */
} GlyphSize;

/** @brief Per-frame node for the sound-bank swap task (@ref func_800A238C). */
typedef struct {
    u8  pad00[0xC];
    u16 state;      /**< 0xC — task state (0 = wait for engine, 1 = swap bank). */
    u16 field0E;    /**< 0xE — cleared when entering state 1. */
} SndTaskNode;

/** @brief Render-list state node (0x10 bytes) for the rules / "Play / Quit" prompt screen.
 *
 * One node out of the D_801D4968 render list (see initTripleTriadRenderList), driven by
 * updatePlayQuitPrompt. The leading 0xC bytes are the ObjList linkage. */
typedef struct {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ u8 state;    /**< Current @ref PlayQuitPhase. */
    /* 0x0D */ u8 counter;  /**< Per-phase frame counter (acts on reaching TT_HOLD_FRAMES_FADE). */
    /* 0x0E */ u8 pad0E[2];
} PromptScreenNode;  /* 0x10 */

/** @brief Render context for @c func_800A4250 (fields named by offset — role inferred). */
typedef struct {
    /* 0x00 */ s16 unk00;     /**< Forwarded (−0x13) as @c func_8002FF34's @c yPos. */
    /* 0x02 */ s16 unk02;     /**< Base for the @c w arg (+0xB, then +column*13). */
    /* 0x04 */ u8  pad04[0xC];
    /* 0x10 */ s32 unk10;     /**< Forwarded as @c func_8002FF34's @c col. */
} func_800A4250_arg2;

/* ───────────────────── be_object4-internal externs ───────────────────── */

/* Imported functions kept as externs here, each for a concrete reason (numstr, controller-
   input, battle-display and colour-bar GPU helpers were migrated to numstr.h / thread.h /
   btl_anim.h / drawbar.h):
     - sendSpuCommand, func_800300F8 — owned by btl_color.c, whose btl_color.h pulls in battle.h
       (for BattleCmdEntry); tripletriad is decoupled from battle.h, and this header does not
       include battle.h.
     - func_800281A4, func_800485C4, func_80030F10 — no .c in the tree defines them yet, so there
       is no owner translation unit to give a header.
     - getAnimFrameParam — returns u16 (thread.c) but this caller needs the s32 view with no
       widening mask; adopting the true u16 measurably breaks the match (see thread.h). */
extern void sendSpuCommand(s32 idx);
extern void func_800485C4(void *dst, void *src, s32 size);
extern void func_800281A4(s32 entity, s32 side, s32 value);
extern void *func_800300F8(void *renderCtx, void *prim, s32 glyph, s32 x, s32 y, s32 color, s32 blink);
extern s32  getAnimFrameParam(s32 slot, s32 sub);     /**< Per-controller input-frame param. Defined u16 in thread.c, but the original caller uses it as s32 (no widening mask) — match-load-bearing, so kept here rather than via thread.h. */
extern s32  func_80030F10(s32 arg);                   /**< Read a controller's button mask (owner TU not yet identified). */

/* File-scope data: a few globals owned elsewhere (battle config / menu palette) plus
   be_object4-private board / SFX / input state — the D_801D4xxx / D_80182Exx
   symbols are not referenced by any other translation unit. */
extern u8  g_battleConfig[];   /**< Shared battle config; [9] bit 0 = sound-bank selector. */
extern u8  D_80082C11;         /**< Sound-bank selector flag (same byte as g_battleConfig[9]). */
extern s16 D_8005F11C;
extern s32 g_menuColor[];
extern u8  g_tripleTriadSoundBank[];       /**< Start of the Triple Triad sound region uploaded to a bank. */
extern u8  D_801D49B8[];       /**< Digit-glyph string buffer filled by intToDecString (func_800A3EE0). */
extern s16 D_801D49F8[];
extern u16 D_801D4B18;
extern u16 D_801D4B1A;
extern u16 D_801D4AF8[2][4]; /**< Per-(entity,side) previous edge flags (see func_800A29D4). */
extern s16 D_801D4B08[2][4]; /**< Per-(entity,side) edge countdown timer (see func_800A29D4). */
extern s32 D_801D4B20[]; /**< Per-controller current held-button mask. */
extern s32 D_801D4B28[]; /**< Per-controller auto-repeat mask. */
extern s32 D_801D4B30[]; /**< Per-controller newly-pressed mask. */
extern s32 D_801D4B24;   /**< = D_801D4B20[1] (player 2); split symbol for the readPads write. */
extern s32 D_801D4B2C;   /**< = D_801D4B28[1] (player 2). */
extern s32 D_801D4B34;   /**< = D_801D4B30[1] (player 2). */
extern SfxConfig g_sfxConfigs[];
extern u8 g_tripleTriadAkao[];
extern u8 D_801D4568[];
extern u8 D_801D4968[];
extern u8 D_801D4978[];
extern CursorState D_801D49C8;
extern u8 D_801D4A88[]; /**< Per-cell lookup value, indexed by cursor grid position. */
extern u8 D_801D4AF6;   /**< Total cell count of the cursor grid (rows of 11). */

/* Private prototypes — functions defined in be_object4.c. */
extern void *func_800A3EE0(void *ot, SPRT *prim, s32 x, s32 number, u32 color, s32 clutPage); /**< Right-aligned decimal SPRT renderer; tail-called by func_800A4098. */
extern void *func_800A3D2C(void *otBase, void *pkt, s32 x, s32 y, s32 cardImg, s32 col); /**< Card-image primitive. */
extern void *func_800A3528(void *otBase, void *pkt, void *(*drawCell)(void *, void *, s32, s32, s32)); /**< Per-cell slide-render iterator. */
extern s32 func_800A238C();
extern s32 func_800A29D4(BattleAnimState *base, BattleAnimEntity *elem, u16 arg1, s32 side, s32 entryIndex);
extern s32 func_800A390C(s32 flags0, s32 flags1); /**< Cursor/timer state machine. */
extern s32 func_800A443C(s32 a0);                 /**< VSync-locked display-list apply. */
extern void func_800A4504(s32 a0, s32 a1); /**< SFX (60, 32) init. */

#endif /* TRIPLETRIAD_BE_OBJECT4_H */
