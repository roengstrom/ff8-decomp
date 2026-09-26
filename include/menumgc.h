/**
 * @file menumgc.h
 * @brief Types and data owned by the menumgc overlay unit (the Magic menu).
 *
 * The menumgc overlay (loaded at @c 0x801E2800) implements everything under
 * Magic: one character's spell list, casting a menu spell, exchanging spells
 * with a second character, the sort popup and manual rearrange.
 *
 * Cross-overlay shared types (e.g. @c MenuDisplayConfig) live in
 * @c include/menu.h; the sibling junction menu's context is @c JunctionMenuCtx
 * there.
 */
#ifndef MENUMGC_H
#define MENUMGC_H

#include "battle.h"
#include "common.h"

/* ======================================================================== */
/* Public typedefs/structs                                                  */
/* ======================================================================== */

typedef struct { u8 unk00[0x14]; } Unk14;
typedef struct { u8 unk00[0x40]; } Unk40;

/**
 * @brief Magic menu context (passed to every menumgc screen function).
 *
 * The magic menu's view of its menu-task slot: func_801EC43C claims one with
 * func_801F179C, registering magicMenuUpdate as the update callback and
 * func_801EBF08 as the draw callback, copies the character index out of the
 * parent menu context and seeds the per-screen fields below; state 0x71 unlinks
 * the task again. The first 0x10 bytes are the shared MenuTask header
 * (include/menumain.h); 0x74 of the slot's 0x78 bytes are mapped here: the
 * state word, the selected character and partner, the per-character list
 * cursors, the panel and page animations, the flags the draw callback reads,
 * and the states its prompts return to.
 * @note Bytes with no reader in menumgc are left as padding; extend as the
 *       remaining renderers are decompiled.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x10];         /**< MenuTask header: list links and the two frame callbacks (include/menumain.h). */
    /* 0x10 */ u16 state;              /**< Current state machine state (0x00-0x71); statePtr points at it. */
    /* 0x12 */ u8 pad12[0x4];          /**< Padding; 0x12 is the task pool's in-use flag, 0x13-0x15 have no reader in menumgc. */
    /* 0x16 */ u16 returnState;        /**< State to resume when a character-switch slide ends, and the state the yes/no box takes on Yes. */
    /* 0x18 */ u16 exitState;          /**< State the yes/no box takes on No or on cancel. */
    /* 0x1A */ u16 pageReturnState;    /**< State to resume when a spell-list page flip ends; the quantity editor reuses it as its cancel target. */
    /* 0x1C */ u16 promptCancelState;  /**< State to resume when a transfer prompt is cancelled. */
    /* 0x1E */ u16 messageReturnState; /**< State to resume when the timed refusal message closes. */
    /* 0x20 */ u8 pad20[0x4];          /**< Padding; func_801EC43C stores the parent menu context pointer here (its 0x22 seeds charIdx). */
    /* 0x24 */ u8 *itemPtr;            /**< Text pointer for the description line: the spell under the cursor, or the current command's help string; func_801EB250 hands &itemPtr to the panel as its two-entry item list. */
    /* 0x28 */ u8 *itemPtr2;           /**< Previous itemPtr, kept so the outgoing row still draws while a page slides. */
    /* 0x2C */ s16 *sortOptionList;   /**< Sort popup option table (D_801EC820, a -1 terminated list of text ids) while the popup is up, 0 once it is closed. */
    /* 0x30 */ s16 sortPopupSlide;     /**< Sort popup slide, stepped by 0x100 from 0 (closed) to 0x1000 (open). */
    /* 0x32 */ u8 discardCharIdx;      /**< Character whose magic slot the discard prompt clears. */
    /* 0x33 */ u8 discardSlot;         /**< Magic slot (0-31) the discard prompt clears. */
    /* 0x34 */ s16 upperPageSlide;     /**< Page-flip slide of the upper spell list, preset to -0xE67 or +0xE67 and stepped by 0x199 back to 0. */
    /* 0x36 */ s16 targetMask;         /**< Bitmask of characters a menu spell may be cast on (func_80036EC0 at entry); the target window lists exactly these. */
    /* 0x38 */ u8 slotCursor[8];       /**< Per-character magic-slot cursor 0-31 (page = slot / 4, row = slot % 4), indexed by character. */
    /* 0x40 */ s16 slideOffset;        /**< Upper character panel slide, preset to -0xF80 or +0xF80 and stepped by 0x80 to 0 while the character switches. */
    /* 0x42 */ s8 upperPage;           /**< Visible page (0-7) of the upper spell list, four slots per page; signed because the flip wraps through -1. */
    /* 0x43 */ u8 upperPrevPage;       /**< Page the upper list showed before the flip, drawn while it slides out. */
    /* 0x44 */ s16 lowerPageSlide;     /**< Page-flip slide of the lower spell list, the upperPageSlide twin. */
    /* 0x46 */ s8 lowerPage;           /**< Visible page (0-7) of the lower spell list. */
    /* 0x47 */ u8 lowerPrevPage;       /**< Page the lower list showed before the flip. */
    /* 0x48 */ s16 rearrangeSlide;     /**< Lower list's slide on the manual rearrange screen: 0x1000 off screen, stepped by 0x100 to 0 as it comes in. */
    /* 0x4A */ s16 partnerSlideOffset; /**< Lower (partner) panel slide, preset to -0xF80 or +0xF80 and stepped by 0x80 to 0 when the partner changes. */
    /* 0x4C */ u8 pad4C[0x4];          /**< Padding; nothing in menumgc reads or writes 0x4C-0x4F. */
    /* 0x50 */ s16 fadeScale;          /**< Menu brightness 0-0x1000, raised by 0x100 per frame as the screen opens and lowered as it closes; the draw callback passes it to setMenuColorIntensity. */
    /* 0x52 */ s16 targetWindowSlide;  /**< Target window slide: 0 while it is fully in, 0x1000 once the spell list has focus again. */
    /* 0x54 */ u8 upperSlot;           /**< Manual rearrange: slot 0-31 picked in the upper list (the one being moved). */
    /* 0x55 */ u8 lowerSlot;           /**< Manual rearrange: slot 0-31 in the lower list it is swapped with; starts equal to upperSlot. */
    /* 0x56 */ u8 drawMode;            /**< Layout the draw callback picks: 0 command row, 1 spell list, 2 target select, 3 exchange source, 4 exchange partner (two panels and the prompts), 5 manual rearrange. */
    /* 0x57 */ s8 targetCursor;        /**< Row of the target cursor; findNthSetBit(targetMask, targetCursor) is the character it points at. */
    /* 0x58 */ s8 charQty;             /**< Quantity on charIdx's side of the quantity editor (0-100); the editor draws charQty on charIdx's row. */
    /* 0x59 */ s8 partnerQty;          /**< Quantity on partnerCharIdx's side; on commit the pair is written to the two slots crossed, because func_801E626C then swaps them. */
    /* 0x5A */ u8 exchangeSpellId;     /**< Spell the transfer prompts and the quantity editor work on. */
    /* 0x5B */ u8 pad5B[0x1];          /**< Padding; func_801EC43C measures the prompt strings and stores the prompt panel's width here (read by func_801E6810, func_801E6858, func_801EBA68, func_801EBBE4). */
    /* 0x5C */ s16 messageTimer;       /**< Frames left on the refusal message (600 when it opens, cut to 0 by a button press). */
    /* 0x5E */ u8 drawFlags;           /**< 0x01 two-choice prompt, 0x02 three-choice prompt, 0x04 yes/no box, 0x08 quantity editor, 0x10 suppress the lower list's cursor, 0x20 L/R character-switch arrows, 0x40 a sub-screen owns the cursor, 0x80 refusal message. */
    /* 0x5F */ u8 promptCursor;        /**< Choice cursor inside the two- and three-choice transfer prompts. */
    /* 0x60 */ u8 targetCount;         /**< Rows in the target window: popcount(targetMask), taken at entry. */
    /* 0x61 */ u8 commandIdx;          /**< Cursor in the top command row; indexes D_801EC814 for the label's x position and for its description text. */
    /* 0x62 */ u8 partnerCharIdx;      /**< The second character on screen: the exchange partner, and the one that gives its stock in the whole-inventory transfer. */
    /* 0x63 */ u8 pad63[0x1];          /**< Padding; no reader in menumgc. */
    /* 0x64 */ u8 charIdx;             /**< Character whose magic list the screen is on, seeded from the parent menu context. */
    /* 0x65 */ u8 prevCharIdx;         /**< Character sliding out of whichever panel is switching; shared by both panels. */
    /* 0x66 */ u8 pad66[0x1];          /**< Padding; no reader in menumgc. */
    /* 0x67 */ u8 commandMask;         /**< Bitmask of selectable top commands, rebuilt by func_801E66C0; func_801F76E0 skips the clear bits. */
    /* 0x68 */ u8 exchangeMode;        /**< Pooling direction plus the prompt that chose it; see the @c MAGIC_EXCHANGE_ bits below. */
    /* 0x69 */ u8 headerCursorActive;  /**< While set, the switch states draw the character panel cursor (func_801E6A9C) and keep taking left/right during the slide. */
    /* 0x6A */ u8 refreshHelpText;     /**< While set, a character switch re-fetches the current command's description into itemPtr. */
    /* 0x6B */ u8 pad6B[0x1];          /**< Padding; no reader in menumgc. */
    /* 0x6C */ u16 discardReturnState; /**< State to resume when the discard prompt closes. */
    /* 0x6E */ u16 discardPromptOpen;  /**< Non-zero while the discard prompt is up; the draw callback then draws its yes/no window. */
    /* 0x70 */ s8 yesNoCursor;         /**< Cursor in the yes/no boxes: 0 accepts, 1 (the preset on open) declines. */
    /* 0x71 */ s8 sortCursor;          /**< Selected line of the Sort popup, also the option handed to func_801E64FC. */
    /* 0x72 */ s8 sortOptionCount;     /**< Number of Sort popup lines, counted from D_801EC820 at entry; it also sets the popup's height. */
    /* 0x73 */ u8 pad73[0x1];          /**< Padding to 0x74; the task pool slot itself runs on to 0x77. */
} MagicMenuCtx; /* 0x74 of the task slot's 0x78 bytes */

/** @brief Bits of MagicMenuCtx.drawFlags, read by the draw callback func_801EBF08. */
#define MAGIC_DRAW_TWO_CHOICE   0x01  /**< Two-choice transfer prompt is up. */
#define MAGIC_DRAW_THREE_CHOICE 0x02  /**< Three-choice transfer prompt is up. */
#define MAGIC_DRAW_YES_NO       0x04  /**< Yes/no box is up. */
#define MAGIC_DRAW_QTY_EDITOR   0x08  /**< Quantity editor is up. */
#define MAGIC_DRAW_NO_LOWER_CUR 0x10  /**< Suppress the lower list's cursor. */
#define MAGIC_DRAW_LR_ARROWS    0x20  /**< Draw the L/R character-switch arrows. */
#define MAGIC_DRAW_SUB_CURSOR   0x40  /**< A sub-screen owns the cursor, so the command row parks its own. */
#define MAGIC_DRAW_MESSAGE      0x80  /**< Timed refusal message is up. */

/** @brief Bits of MagicMenuCtx.exchangeMode, set when a transfer prompt is answered. */
#define MAGIC_EXCHANGE_TO_PARTNER   0x01  /**< Pool into the partner's slot instead of this character's. */
#define MAGIC_EXCHANGE_THREE_CHOICE 0x80  /**< The three-choice prompt chose the direction, so reopen that one. */

/** @brief The direction field of MagicMenuCtx.exchangeMode; only bit 0 is ever set. */
#define MAGIC_EXCHANGE_DIR_MASK 0x7F

/* ======================================================================== */
/* Data owned by this unit                                                  */
/* ======================================================================== */

extern s16 D_801EC814[];          /**< Per-command text id; also the cursor x position (func_801E69EC). */
extern s16 D_801EC820[];          /**< Sort popup option text ids, terminated by -1. */
extern BattleCharData D_801EC9F0; /**< Stats of the character on screen, recomputed by func_801F537C. */
extern BattleCharData D_801ECBC0; /**< Stats of the exchange partner. */
extern BattleCharData D_801ECD90; /**< Stats of the character sliding out, drawn during a switch. */
extern Unk14 D_801ECF60[];
extern Unk40 D_801ECF90[];
extern s16 D_801ED010[];
extern u8 D_801ED020[];           /**< Scratch buffer for the formatted prompt strings. */

#endif /* MENUMGC_H */
