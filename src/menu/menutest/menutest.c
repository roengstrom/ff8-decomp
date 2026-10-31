#include "common.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menutest.h"
#include "gamestate.h"
#include "psxsdk/libgte.h"

/**
 * @brief Test menu task state, allocated by func_801F179C.
 *
 * The first 0x10 bytes belong to the menumain allocator (task links and the
 * tick/draw callbacks).
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x10];
    /* 0x10 */ u16 state;
    /* 0x12 */ u8 pad12[0xE];
    /* 0x20 */ u8 *text;         /**< Text drawn centred in the bottom panel. */
    /* 0x24 */ s16 intensity;    /**< Menu color intensity. */
    /* 0x26 */ s16 score;        /**< Questions answered correctly. */
    /* 0x28 */ u8 pad28[2];
    /* 0x2A */ s16 scroll;       /**< Scroll position, looked up in the D_801FA3C8 falloff table. */
    /* 0x2C */ u8 levelPicked;   /**< 1 when the tutorial menu picked a level (func_801E28D4() != 0xFF). */
    /* 0x2D */ u8 level;         /**< Test level, 0-based. */
    /* 0x2E */ u8 unk2E;
    /* 0x2F */ u8 choice;        /**< Choice under the cursor: 0 = Yes, 1 = No. */
} TestMenuState;

/** @brief Number of SeeD test levels. */
#define TEST_LEVEL_COUNT 30

/**
 * @brief mngrp.bin slice holding level 1's questions; level n is this plus n.
 *
 * Each level's slice is a TestQuestionTable.
 */
#define TEST_FIRST_SLICE 0x60

/**
 * @brief A level's questions: a count, then each TestQuestion's byte offset from
 *        the start of the table (the layout of menumain's D_801F8BB8 text table too).
 */
typedef struct {
    u16 count;
    u16 offsets[1];
} TestQuestionTable;

/** @brief A SeeD test question, one entry of a TestQuestionTable. */
typedef struct {
    u8 answer;   /**< Correct answer: 0 = Yes, 1 = No. */
    u8 text[1];  /**< Question text in FF8 encoding; a 0x0B marker precedes each choice. */
} TestQuestion;

/** @brief Position func_801E59B4 records for a choice marker, relative to the text origin. */
typedef struct {
    s16 x;
    s16 y;
    s16 choice;  /**< Choice the marker belongs to: its code minus 0x20. */
    s16 pad06;
} TestChoiceMark;

extern s16 g_testChoiceCount;
extern u8 g_testAnswer;
extern s32 D_801E7ACC[2];
extern u8 D_801E7ADC;
extern u8 g_testQuestionText[];
extern u8 g_testHeaderText[];
extern TestChoiceMark g_testChoiceMarks[];
extern u8 D_801FABD4;
extern u32 D_801E69B8;

/** Center offset: maxW = width, tw = (maxW - tw) / 2 */
#define CalcCenter(maxW, tw, w) \
    do { (maxW) = (w); (tw) = ((maxW) - (tw)) / 2; break; } while (1)

void func_801E5D74(TestMenuState *state);

/** @brief Look up string @p a0 in menu text category 0xD. */
u8 *func_801E5800(s32 a0) {
    return func_801F08D4(1, 0xD, a0, 0);
}

/**
 * Places the cursor on choice @c state->choice: its g_testChoiceMarks position
 * from the text origin (0x22, 0x23), slid left by the D_801FA3C8 falloff of
 * @c state->scroll, passed to func_801F0A34.
 * @param a0 Display mode parameter (passed through to func_801F0A34)
 * @param state Test menu state.
 */
void func_801E582C(s32 a0, TestMenuState *state) {
    TestChoiceMark *marks = g_testChoiceMarks;
    s32 idx = state->choice;
    s32 scroll = state->scroll;
    s32 x;
    s32 y;
    s32 v0;
    s32 v1;

    x = marks[idx].x; // load and add kept apart so the loads go straight into t0/a3
    y = marks[idx].y;
    x += 0x22;
    y += 0x23;
    if (scroll < 0) scroll += 0x3F;
    v1 = D_801FA3C8[scroll >> 6];
    v0 = (v1 * 3) << 7;
    if (v0 < 0) v0 += 0xFFF;
    func_801F0A34(a0, 0, x - (v0 >> 12), y);
}

/**
 * Decodes a string using func_801F7A54 key and intToDecStringShort/replaceLeadingZeros,
 * then copies it to D_801E7ADC via copyString.
 */
void func_801E58B8(void) {
    u8 buf[0x20];
    u8 *p = buf;
    s32 v0;

    v0 = func_801F7A54();
    if (v0 == 0x1F) {
        copyString((s32)&D_801E7ADC, func_801F6AFC(0x39));
        return;
    }
    v0 = func_801F7A54();
    intToDecStringShort(v0, (s32)p, 1);
    replaceLeadingZeros((s32)p, 4, 1, 0x10);
    if (*p == 0x10) {
        s32 c = 0x10;
        do {
            p++;
        } while (*p == c);
    }
    {
        u8 *q = p;
        if (*p != 0) {
            do {
                v0 = getMenuString(0xB);
                v0 = *(u8 *)(v0 + 1) + 0xFF;
                *q += v0;
                q++;
            } while (*q != 0);
        }
    }
    copyString((s32)&D_801E7ADC, (s32)p);
}

/**
 * Text/string parser that processes encoded character data into display buffers.
 * Redirects GP to PS1 scratchpad (0x1F800300) for fast temporary storage,
 * then iterates over encoded input, handling control codes (0x00=end, 0x01=end,
 * 0x02=reset, 0x07=end, 0x0A=special decode, 0x0B=new line), character decoding
 * via intToDecStringShort/replaceLeadingZeros, and glyph width accumulation via getNibbleValue.
 * @param a0 Pointer to encoded input string
 * @param a1 Output text buffer
 * @param a2 Output glyph position buffer (8 bytes per entry)
 * @return Number of lines processed (via $fp counter)
 * @note Handwritten assembly. Uses `addi $gp` (opcode 0x08, traps on overflow)
 *       for GP scratchpad redirection — the C compiler only emits `addiu`.
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E59B4);

/**
 * @brief Lay out question @p index of a level's @p table: its answer goes to
 *        g_testAnswer, its text to g_testQuestionText, its choice markers to
 *        g_testChoiceMarks and their count to g_testChoiceCount.
 * @param table The level's questions.
 * @param index Question to lay out.
 */
void func_801E5D18(TestQuestionTable *table, s32 index) {
    TestQuestion *question = (TestQuestion *)((u8 *)table + table->offsets[index]);

    g_testAnswer = question->answer;
    g_testChoiceCount = func_801E59B4(question->text, g_testQuestionText, g_testChoiceMarks);
}

/**
 * Menu test state machine — drives all menu navigation and display logic.
 * Uses a 28-case switch on @c state->state, with cases
 * handling: initial display (0), VSync wait (1), page transition checks (2),
 * resource allocation (3,6), fade-in animation (4,7,10,15), input polling (5,8,11),
 * text decode (9), scroll animation (13,14,19,20,25,26), page navigation (16,17),
 * confirmation (21,22), and cleanup/exit (27).
 * Reads input via func_801F0948/pollCdReadStatus, fades @c intensity, moves
 * @c scroll, tracks @c level, @c unk2E and @c choice, counts @c score, sets
 * @c text, and dispatches rendering via func_801E582C, func_801E5D18,
 * func_801E58B8, func_801F6800.
 * @param state Test menu state (s1 = state, s2 = &state->state, s3/s0 from g_menuDisplayCfg)
 */
INCLUDE_ASM("asm/ovl/menutest/nonmatchings/menutest", func_801E5D74);

/**
 * Sets up a GPU frame with centered text display for the header area.
 * Measures text width via measureMessage, centers it within 0xF4 pixels,
 * draws via drawMessageText, configures g_menuDisplayCfg display struct
 * (0x18, 0x6, 0xF4, 0x16), and submits via DrawMenuWindow.
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @return Result of DrawMenuWindow
 */
s32 func_801E64B4(s32 a0, s32 a1) {
    s32 disp = a0;
    s32 ot = a1;
    u8 *text = g_testHeaderText;
    s32 maxW;
    s32 v0;
    s32 buf;

    v0 = measureMessage(ot + text - ot); // liveness trick: the extra uses of ot put it in s1
    CalcCenter(maxW, v0, 0xF4); // the macro's do/break/while(1) shape puts maxW in s3
    drawMessageText(disp, v0 + 0x18, 0xC, text);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 6;
    g_menuDisplayCfg.w = maxW;
    g_menuDisplayCfg.h = 0x16;
    return DrawMenuWindow(disp, ot, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * Sets up GPU display for the scrollable text body area.
 * Computes scroll offset from D_801FA3C8 animation table (same pattern as
 * func_801E582C), draws text at the scroll-adjusted X position via drawMessageText,
 * then configures two g_menuDisplayCfg display regions:
 * first (0x1C, 0x21, 0x148, 0x9F) submitted via func_801EF800,
 * second (0x18, 0x1D, 0x150, 0xA7) submitted via DrawMenuWindow.
 * @param state Test menu state.
 * @param a1 Display list pointer
 * @param a2 OT pointer
 * @return Result of DrawMenuWindow
 */
s32 func_801E6570(TestMenuState *state, s32 a1, s32 a2) {
    s32 disp = a1;
    s32 x = 0x22;
    MenuDisplayConfig *cfg = &g_menuDisplayCfg;
    s32 scroll = a2 + state->scroll - a2; // liveness trick: the extra uses of a2 put it in s0
    s32 v0;
    s32 v1;

    if (scroll < 0) scroll += 0x3F;
    v1 = D_801FA3C8[scroll >> 6]; // own variable: puts the lhu result in v0 for the multiply
    v0 = (v1 * 3) << 7;
    if (v0 < 0) v0 += 0xFFF;
    x -= v0 >> 12;

    a2 = func_801EF8D8(disp, a2);
    drawMessageText(disp, x, 0x23, g_testQuestionText);

    cfg->x = 0x1C;
    cfg->y = 0x21;
    cfg->w = 0x148;
    cfg->h = 0x9F;
    v0 = func_801EF800(disp, a2, cfg);

    cfg->iconType = 0;
    cfg->iconSubType = 0;
    cfg->x = 0x18;
    cfg->y = 0x1D;
    cfg->w = 0x150;
    cfg->h = 0xA7;
    return DrawMenuWindow(disp, v0, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * Sets up GPU display for a secondary text area, similar to func_801E64B4.
 * Centers @c state->text within 0x150 pixels, draws via drawMessageText
 * at Y=0xC8, configures g_menuDisplayCfg (0x18, 0xC4, 0x150, 0x14), and submits
 * via DrawMenuWindow.
 * @param state Test menu state.
 * @param a1 Display list pointer
 * @param a2 OT pointer
 * @return Result of DrawMenuWindow
 */
s32 func_801E66A8(TestMenuState *state, s32 a1, s32 a2) {
    s32 disp = a1;
    s32 ot = a2;
    s32 maxW;
    s32 v0;

    v0 = measureMessage(ot + state->text - ot); // liveness trick: the extra uses of ot put it in s0
    CalcCenter(maxW, v0, 0x150);
    drawMessageText(disp, v0 + 0x18, 0xC8, state->text);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = 0x18;
    g_menuDisplayCfg.y = 0xC4;
    g_menuDisplayCfg.w = maxW;
    g_menuDisplayCfg.h = 0x14;
    return DrawMenuWindow(disp, ot, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * Sets up menu display rendering pipeline.
 * @param state Test menu state.
 * @param a1 Display buffer 1
 * @param a2 Display buffer 2
 * @return Final display buffer pointer from func_801F1B10
 */
s32 func_801E6760(TestMenuState *state, s32 a1, s32 a2) {
    s32 v0;

    func_801F1AFC();
    setMenuBrightness(state->intensity);
    setNextPageMarkerBrightness(state->intensity);
    v0 = func_801E64B4(a1, a2);
    v0 = func_801E6570(state, a1, v0);
    v0 = func_801E66A8(state, a1, v0);
    func_801F1B10();
    return v0;
}

/**
 * @brief Test menu overlay entry: allocate the task state and run its first tick.
 *
 * func_801E5D74 is the task's tick callback and func_801E6760 its draw
 * callback. Runs the level the player picked in the tutorial menu, or level
 * tutoEntryCount when none was picked (func_801E28D4() returns
 * 0xFF), and loads that level's questions.
 */
void func_801E67F0(void) {
    TestMenuState *state;
    s32 v0;
    u8 *text;

    state = func_801F179C(func_801E5D74, func_801E6760);
    func_801F0948(0);
    func_801F5440();
    do {
        v0 = pollCdReadStatus();
    } while (v0 != 0);
    if (state == NULL) {
        return;
    }
    state->score = 0;
    func_801F1D34(&D_801E69B8);
    func_801F1DB0(0);
    v0 = func_801E28D4();
    if (v0 == 0xFF) {
        state->levelPicked = 0;
        state->level = g_gameState.mainData.tutoEntryCount;
        text = func_801E5800(0x11);
        func_801E59B4(text, g_testHeaderText, g_testChoiceMarks);
        text = func_801E5800(0x1A);
    } else {
        state->levelPicked = 1;
        v0 = func_801E28D4();
        state->level = v0;
        D_801E7ACC[0] = state->level;
        D_801E7ACC[1] = state->level + 1;
        text = func_801E5800(0x16);
        func_801E59B4(text, g_testHeaderText, g_testChoiceMarks);
        text = func_801E5800(0x1B);
    }
    state->text = text;
    state->unk2E = 0;
    state->score = 0;
    g_testQuestionText[0] = 0;
    if (state->level < TEST_LEVEL_COUNT) {
        loadSubOverlay(state->level + TEST_FIRST_SLICE, MENU_SUBOVERLAY_ADDR);
    }
    state->scroll = 0;
    func_801E5D74(state);
}
