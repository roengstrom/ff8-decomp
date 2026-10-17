#ifndef UI_TEXT_H
#define UI_TEXT_H

#include "common.h"
#include "psxsdk/libgpu.h"
#include "ui/dialog.h"

/** @brief Characters of a decoded message that the text state machine acts on. */
enum {
    MSG_END = 0x00,
    MSG_NEW_PAGE = 0x01,
    MSG_NEWLINE = 0x02,
    MSG_NEW_PAGE_MARKED = 0x07 /**< Page break that shows the next-page marker. */
};

/** @brief Command bytes returned in bits 8-15 by nextDialogChar. */
enum {
    MSG_CMD_ICON = 0x05, /**< Argument is an icon, drawn as a multi-cell glyph. */
    MSG_CMD_COLOR = 0x06, /**< Argument is the text colour. */
    MSG_CMD_SPEED = 0x08, /**< Argument sets the text speed (guess from its use). */
    MSG_CMD_WAIT = 0x09 /**< Argument is a delay in frames. */
};

enum {
    DIALOG_COLOR_GRAY = 1,
    DIALOG_COLOR_YELLOW = 2,
    DIALOG_COLOR_RED = 3,

    /** @brief Text colour index of white, the colour a message starts in. */
    DIALOG_COLOR_WHITE = 7,
};

extern s32 getIconWidth(s32 idx);
extern s32 getNibbleValue(s32 idx);
extern s32 measureMessage(u8 *str);
extern s32 getTextSize(u8 *str);
extern s32 getTextSizeB(u8 *str);
extern s32 getTextWidth(u8 *str);
extern s32 getFirstLineWidth(u8 *str);
extern void setMenuBrightness(s32 brightness);
extern u32 emitTextGlyph(u32 head, TSPRT *p, s32 glyph, u32 colour, u32 xy);
extern TSPRT *drawDecodedText(P_TAG *ot, TSPRT *p, s32 x, s32 y, u8 *str, s32 colour);
extern u8 *drawMessageText(P_TAG *ot, s32 x, s32 y, u8 *str);
extern void drawDialogText(P_TAG *ot, Dialog *entry);

/** @brief Brightness last passed to setMenuBrightness; menus read it back to restore it. */
extern s32 g_menuBrightness;

#endif
