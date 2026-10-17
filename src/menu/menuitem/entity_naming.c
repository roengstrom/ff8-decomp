#include "common.h"
#include "game.h"
#include "gamestate.h"
#include "menu.h"
#include "menumain.h"
#include "party_stats.h"
#include "psxsdk/libetc.h"
#include "snd_sfx.h"
#include "ui/icon.h"
#include "ui/text.h"

/** @brief Entity naming menu options. */
enum {
    ENTITY_NAMING_OPTION_ENGLISH = 0,
    ENTITY_NAMING_OPTION_SYMBOLS = 1,
    ENTITY_NAMING_OPTION_SPACE   = 2,
    ENTITY_NAMING_OPTION_CONFIRM = 3,
    ENTITY_NAMING_OPTION_DELETE  = 4,
    ENTITY_NAMING_OPTION_DEFAULT = 5,
};

/** @brief String IDs passed as argument to func_801E9E7C. */
enum {
    STRING_NAME_INPUT = 6,
    STRING_NAME_THE_CHARACTER = 7,
};

/**
 * @brief Defines the layout and characters of a character selection grid.
 *
 * The strIds field points to a sequence of string IDs terminated by
 * 0xFFFF. Each string ID represents a string of up to 7 characters
 * to be displayed in the character grid.
 */
typedef struct {
    u16 *strIds;        /**< 0x0: Pointer to the null-terminated sequence of string IDs. */
    u16 strXOffset;     /**< 0x4: Horizontal offset applied to strings at odd indexes. */
    u8 minRowIndex;     /**< 0x6: First character row index. */
    u8 maxRowIndex;     /**< 0x7: Last character row index. */
} CharacterGrid; /* 0x8 bytes */

/**
 * @brief Defines the naming configuration for an entity.
 */
typedef struct {
    u8 nameMaxLength;       /**< 0x0: Maximum allowed name length. */
    u8 portraitIdx;         /**< 0x1: Index of the entity's portrait sprite. */
    u16 defaultNameIdx;     /**< 0x2: Index of the entity's default name. */
    u16 assignedNameOffset; /**< 0x4: Offset to the entity's assigned name, relative to g_gameState. */
    u8 pad6[2];
} EntityNamingConfig; /* 0x8 bytes */

/**
 * @brief State for the entity naming menu.
 */
typedef struct {
    u8 pad00[0x10];
    u16 state;                      /**< 0x10: Current menu state (0-7). */
    u8 pad12[0xE];
    EntityNamingConfig *namingCfg;  /**< 0x20: Pointer to the entity naming configuration. */
    u8 *namePtr;                    /**< 0x24: Pointer to the name being edited. */
    s32 menuBrightness;             /**< 0x28: Menu brightness value (0-4096), used for fade-in/out effects. */
    u8 charGridIndex;               /**< 0x2C: Selected character grid index: 0 = English, 1 = Symbols. */
    u8 charGridCursorIndex;         /**< 0x2D: Cursor index within the selected character grid. */
    u8 menuOptionCursorIndex;       /**< 0x2E: Cursor index within the menu options (0-5). */
    u8 cursorPanelIndex;            /**< 0x2F: Index of the panel containing the cursor: 0 = menu options, 1 = character grid. */
    u8 nameLength;                  /**< 0x30: Current name length. */
} EntityNamingMenuState;

#define CHARACTER_GRID_COUNT 2
extern CharacterGrid D_801EB320[CHARACTER_GRID_COUNT]; /**< Character grid table. */

#define ENTITY_NAMING_CONFIG_COUNT 21
extern EntityNamingConfig D_801EB240[ENTITY_NAMING_CONFIG_COUNT]; /**< Entity naming config table. */

static s32 func_801E9C90(u8 *, s32);
static void func_801E9CD4(EntityNamingMenuState *);
static void func_801E9DE4(u8 *);
static s32 func_801E9E10(u8 *);
static u8 *func_801E9E7C(s32);
static void func_801E9EA8(EntityNamingMenuState *, s32);
static void func_801E9F94(EntityNamingMenuState *);
static void *func_801EA500(void *, void *, s32, s32, s32);
static void *func_801EA538(EntityNamingMenuState *, void *, DR_TPAGE *, s32, s32);
static void *func_801EA714(void *, void *, s32, s32);
static void *func_801EA7E0(EntityNamingMenuState *, void *, void *, s32, s32);
static void *func_801EA8F0(void *, DR_TPAGE *, s32, s32, u16, s32);
static void *func_801EAA04(EntityNamingMenuState *, void *, void *, s32, s32);
static void *func_801EAB00(EntityNamingMenuState *, void *, void *, s32, s32);
static void *func_801EAB8C(void *, void *, s32, s32);
static void *func_801EAC54(EntityNamingMenuState *, void *, void *);
static void func_801EAD64(void);

/**
 * @brief Returns the string length, capped at maxLen - 1.
 *
 * Counts consecutive non-null bytes until the null terminator or
 * maxLen bytes have been examined. If the count reaches maxLen,
 * returns maxLen - 1. For non-positive maxLen values, returns
 * maxLength - 1.
 *
 * @param str Pointer to byte string.
 * @param maxLen Maximum number of bytes to scan.
 * @return Number of non-null bytes found, capped at maxLength - 1.
 */
static s32 func_801E9C90(u8 *str, s32 maxLen) {
    s32 i;
    s32 len = 0;

    for (i = 0; i < maxLen; i++) {
        if (*str++ == '\0') {
            break;
        }
        len++;
    }
    if (len >= maxLen) {
        len = maxLen - 1;
    }

    return len;
}

/**
 * @brief Calculates the cursor position and renders it.
 *
 * If the cursor is on the character grid (cursorPanelIndex == 0), calculates
 * its coordinates based on the current character index, accounting for the
 * grid's column and row layout.
 *
 * Otherwise, positions the cursor at a fixed X coordinate and calculates
 * its Y coordinate based on the currently selected menu option.
 *
 * Rendering is delegated to func_801F0994.
 *
 * @param s Pointer to the entity naming menu state.
 */
static void func_801E9CD4(EntityNamingMenuState *s) {
    s32 x;
    s32 y;
    s32 col;
    s32 row;
    s32 div28;
    s32 x1;
    s32 x2;

    if (s->cursorPanelIndex == 0) {
        col = s->charGridCursorIndex % 14;
        row = s->charGridCursorIndex / 14;
        x1 = col / 7;
        div28 = s->charGridCursorIndex / 28;
        
        y = row * 16;
        y += div28 * 8;
        y += 98;
        
        col = s->charGridCursorIndex % 7;
        x2 = x1 * 63;
        x = (col * 9 + x2) * 2;
        x += 114;
    } else {
        x = 34;
        y = s->menuOptionCursorIndex * 16;
        y += 98;
    }

    func_801F0994(0, x, y);
}

/**
 * @brief Removes the last character from a null-terminated string.
 *
 * If the string is not empty, replaces its last character with a null
 * terminator, effectively shortening the string by one character.
 *
 * @param str Pointer to a null-terminated string.
 */
static void func_801E9DE4(u8 *str) {
    if (*str++ == '\0') {
        return;
    }
    while (*str++ != '\0') {
    }
    str[-2] = '\0';
}

/**
 * @brief Checks whether the string contains any non-space characters.
 *
 * Returns 0 if the string is empty or consists entirely of space characters.
 * Otherwise, returns 1 if any character differs from the space character
 * obtained from the first byte of getMenuString(11).
 *
 * @param str Pointer to a null-terminated string.
 * @return 1 if the string contains a non-space character, 0 otherwise.
 */
static s32 func_801E9E10(u8 *str) {
    if (btlStrlen(str) != 0) {
        while (1) {
            s32 ch = *str++;
            if (ch == '\0') {
                break;
            }
            if (ch != getMenuString(11)[0]) {
                return 1;
            }
        }
    }
    return 0;
}

/** @brief Look up string @p strId in menu text category 5. */
static u8 *func_801E9E7C(s32 strId) {
    return func_801F08D4(1, 5, strId, 0);
}

/**
 * @brief Appends a character or space to the name being edited.
 * 
 * Appends the character at the current grid cursor position if
 * `isGridCharacter` is nonzero; otherwise, appends a space.
 * 
 * @param s Pointer to the entity naming menu state.
 * @param isGridCharacter Nonzero to append a grid character, or zero to append a space.
 */
static void func_801E9EA8(EntityNamingMenuState *s, s32 isGridCharacter) {
    u8* nextCharPtr;
    u8* charPtr;
    s32 row;
    s32 col;

    nextCharPtr = &s->namePtr[func_801E9C90(s->namePtr, s->namingCfg->nameMaxLength)];
    col = s->charGridCursorIndex % 7;
    row = s->charGridCursorIndex / 7;
    if (isGridCharacter != 0) {
        charPtr = &func_801E9E7C(D_801EB320[s->charGridIndex].strIds[row])[col];
    } else {
        charPtr = &getMenuString(11)[0];
    }
    *nextCharPtr++ = *charPtr;
    *nextCharPtr = '\0';

    // Dead code added to match with the original game binary.
    if (nextCharPtr) { nextCharPtr++; nextCharPtr--; }
}

/**
 * @brief Updates the entity naming menu state machine.
 *
 * Handles name editing, character grid switching, action selection,
 * and menu fade in/out.
 *
 * @param s Pointer to the entity naming menu state.
 */
static void func_801E9F94(EntityNamingMenuState *s) {
    CharacterGrid *charGrid;
    u16 btnFlags;
    u16 cfgFlags;
    u16 *statePtr;

    func_801F0948(s->menuBrightness);
    btnFlags = g_menuDisplayCfg.inputRepeat;
    cfgFlags = g_menuDisplayCfg.inputNew;
    charGrid = &D_801EB320[s->charGridIndex];
    statePtr = &s->state;

    switch (*statePtr) {
    case 0:
        s->menuBrightness = 0;
        *statePtr = 1;
        break;

    case 1:
        s->menuBrightness += 256;
        if (s->menuBrightness >= 4096) {
            s->menuBrightness = 4096;
            *statePtr = 2;
        }
        func_801E9CD4(s);
        break;

    case 2:
        s->cursorPanelIndex = 0;
        *statePtr = 3;
        func_801E9CD4(s);
        break;

    case 3: {
        s32 row;
        s32 col;
        col = s->charGridCursorIndex % 14;
        row = s->charGridCursorIndex / 14;
        if (btnFlags & PADLup) {
            row--;
            if (row < 0) {
                row = 0;
            } else {
                sendSpuCommand(1);
            }
        }
        if (btnFlags & PADLdown) {
            if (row < charGrid->maxRowIndex) {
                sendSpuCommand(1);
            }
            row++;
        }
        if (row > charGrid->maxRowIndex) {
            row = charGrid->maxRowIndex;
        }
        if (btnFlags & PADLright) {
            col++;
            if (col > 13) {
                col = 13;
            } else {
                sendSpuCommand(1);
            }
        }
        if (btnFlags & PADLleft) {
            sendSpuCommand(1);
            col--;
            if (col < 0) {
                col = 0;
                *statePtr = 4;
            }
        }
        s->charGridCursorIndex = col + row * 14;
        func_801E9CD4(s);
        if (cfgFlags & PADRup) {
            sendSpuCommand(3);
            func_801E9DE4(s->namePtr);
        } else if (btnFlags & PADRdown) {
            sendSpuCommand(2);
            func_801E9EA8(s, 1);
        } else {
            if (cfgFlags & PADR1) {
                sendSpuCommand(1);
                s->charGridIndex++;

                // Dead code added to match with the original game binary.
                if (s) { s->charGridIndex++; s->charGridIndex--; }

                s->charGridIndex &= 1;
            }
            if (cfgFlags & PADL1) {
                s32 charGridIndex;
                sendSpuCommand(1);
                charGridIndex = s->charGridIndex;
                charGridIndex--;
                if (charGridIndex < 0) {
                    charGridIndex += 2;
                }
                charGridIndex = charGridIndex % 2;
                s->charGridIndex = charGridIndex;
            }
            if (cfgFlags & PADstart) {
                sendSpuCommand(1);
                s->menuOptionCursorIndex = ENTITY_NAMING_OPTION_CONFIRM;
                *statePtr = 4;
            }
        }
        break;
    }

    case 4:
        s->cursorPanelIndex = 1;
        *statePtr = 5;
        break;

    case 5: {
        s32 menuOptionCursorIndex;
        menuOptionCursorIndex = s->menuOptionCursorIndex;
        if (cfgFlags & PADR1) {
            sendSpuCommand(1);
            s->charGridIndex++;

            // Dead code added to match with the original game binary.
            if (s) { s->charGridIndex++; s->charGridIndex--; }

            s->charGridIndex &= 1;
        }
        if (cfgFlags & PADL1) {
            s32 charGridIndex;
            sendSpuCommand(1);
            charGridIndex = s->charGridIndex;
            charGridIndex--;
            if (charGridIndex < 0) {
                charGridIndex += 2;
            }
            charGridIndex = charGridIndex % 2;
            s->charGridIndex = charGridIndex;
        }
        if (btnFlags & PADLdown) {
            sendSpuCommand(1);
            menuOptionCursorIndex++;
            if (menuOptionCursorIndex > 5) {
                menuOptionCursorIndex = 0;
            }
        }
        if (btnFlags & PADLup) {
            sendSpuCommand(1);
            menuOptionCursorIndex--;
            if (menuOptionCursorIndex < 0) {
                menuOptionCursorIndex = 5;
            }
        }
        if (cfgFlags & PADRdown) {
            switch (menuOptionCursorIndex) {
            case ENTITY_NAMING_OPTION_SYMBOLS:
                sendSpuCommand(2);
                s->charGridIndex = ENTITY_NAMING_OPTION_SYMBOLS;
                break;
            case ENTITY_NAMING_OPTION_ENGLISH:
                sendSpuCommand(2);
                s->charGridIndex = ENTITY_NAMING_OPTION_ENGLISH;
                break;
            case ENTITY_NAMING_OPTION_SPACE:
                sendSpuCommand(2);
                func_801E9EA8(s, 0);
                break;
            case ENTITY_NAMING_OPTION_DELETE:
                sendSpuCommand(3);
                func_801E9DE4(s->namePtr);
                break;
            case ENTITY_NAMING_OPTION_CONFIRM:
                if (func_801E9E10(s->namePtr) != 0) {
                    sendSpuCommand(2);
                    *statePtr = 6;
                    break;
                }
                sendSpuCommand(5);
                break;
            case ENTITY_NAMING_OPTION_DEFAULT:
                sendSpuCommand(2);
                copyString(s->namePtr, func_801E9E7C(s->namingCfg->defaultNameIdx));
                break;
            }
        }
        if (cfgFlags & PADstart) {
            sendSpuCommand(1);
            menuOptionCursorIndex = ENTITY_NAMING_OPTION_CONFIRM;
        }
        s->menuOptionCursorIndex = menuOptionCursorIndex;
        func_801E9CD4(s);
        if (btnFlags & PADLright) {
            sendSpuCommand(2);
            *statePtr = 1;
        }
        break;
    }

    case 6:
        recalcPartyStats();
        func_801E9CD4(s);
        *statePtr = 7;
        break;

    case 7:
        s->menuBrightness -= 256;
        if (s->menuBrightness < 0) {
            s->menuBrightness = 0;
            func_801F18FC(s);
            func_801F0BB0();
            *statePtr = 2;
        }
        func_801E9CD4(s);
        break;
    }

    s->nameLength = func_801E9C90(s->namePtr, s->namingCfg->nameMaxLength);
    func_801F0948(s->menuBrightness);
}

/**
 * @brief Renders the entity's portrait.
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the portrait.
 * @param y Y coordinate of the portrait.
 * @param portraitIdx Index of the entity's portrait sprite.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EA500(void *ot, void *prim, s32 x, s32 y, s32 portraitIdx) {
    return func_800375A0(ot, prim, portraitIdx, x, y, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Renders the name panel for the entity naming menu.
 *
 * Renders the characters of the name stored in `namePtr`, up to the maximum
 * length specified by `namingCfg->nameMaxLength`. Each character is displayed
 * above a dash icon, with 14 pixels of horizontal spacing. Characters after
 * the first null terminator are not rendered, leaving the remaining positions
 * empty. The character at `nameLength` blinks to indicate the current editing
 * position.
 *
 * @param s Pointer to the entity naming menu state.
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the panel.
 * @param y Y coordinate of the panel.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EA538(EntityNamingMenuState *s, void *ot, DR_TPAGE *prim, s32 x, s32 y) {
    s32 charY;
    s32 nameMaxLength;
    s32 flag;
    s32 i;
    s32 charX;
    MenuDisplayConfig *gs;
    u8* ptr;
    u32 color;
    s32 ch;
    s32 head;
    s32 head2;
    s32 head3;
    s32 primOffset;
    primOffset = 20;
    gs = &g_menuDisplayCfg;
    charX = x + 18;
    charY = y + 10;
    flag = 1;
    nameMaxLength = s->namingCfg->nameMaxLength;
    ptr = s->namePtr;
    for (i = 0; i < nameMaxLength; i++) {
        color = g_menuTint[MENU_TINT_NORMAL];
        if (i == s->nameLength && (gs->animCounter & 1)) {
            color = (color >> 1) & 0xFFFFFF;
        }
        ch = *ptr++;
        if (ch == 0) {
            flag = 0;
        }
        if (flag != 0) {
            getAddrFast(ot, head);
            head2 = func_801F0F20(head, prim, ch - 32, 7, (charY << 0x10) | (charX & 0xFFFF));
            prim = (DR_TPAGE *)((u8 *)prim + primOffset);
            setDrawTPage(prim, 1, 0, 0x1F);

            // Dead code added to match with the original game binary.
            if (head3) { head3++; head3--; }

            head3 = (s32)prim << 8;
            setAddrFast(prim, head2);
            prim++;
            setAddrFast(ot, head3);
        }
        prim = drawIcon(ot, prim, ICON_DASH, charX, charY, color);
        charX += 14;
    }
    gs->iconType = ICON_NAME;
    gs->iconSubType = 0;
    gs->x = x;
    gs->y = y;
    gs->w = 258;
    gs->h = 32;
    return func_801EF9AC(ot, prim, 4096, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Renders the help panel for the entity naming menu.
 * 
 * The panel displays the string "Name the character".
 * 
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the panel.
 * @param y Y coordinate of the panel.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EA714(void *ot, void *prim, s32 x, s32 y) {
    s32 strX = x + 8;
    s32 strY = y + 8;
    prim = func_801F0FEC(ot, prim, strX, strY, func_801E9E7C(STRING_NAME_THE_CHARACTER), DIALOG_COLOR_WHITE);
    g_menuDisplayCfg.iconType = ICON_HELP;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 258;
    g_menuDisplayCfg.h = 26;
    return func_801EF9AC(ot, prim, 4096, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Renders the action panel for the entity naming menu.
 * 
 * Renders the six options: "English", "Symbols", "Space", "Confirm",
 * "Delete" and "Default", highlighting the option corresponding to
 * the selected character grid.
 * 
 * @param s Pointer to the entity naming menu state.
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the panel.
 * @param y Y coordinate of the panel.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EA7E0(EntityNamingMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    MenuDisplayConfig *gs = &g_menuDisplayCfg;
    s32 strX = x + 10;
    s32 strY = y + 10;
    s32 i;
    s32 color;
    for (i = 0; i < 6; i++) {
        color = DIALOG_COLOR_WHITE;
        if (i == s->charGridIndex) {
            color = DIALOG_COLOR_YELLOW;
        }
        prim = func_801F0FEC(ot, prim, strX, strY, func_801E9E7C(i), color);
        strY += 16;
    }
    gs->iconType = 0;
    gs->iconSubType = 0;
    gs->x = x;
    gs->y = y;
    gs->w = 80;
    gs->h = 125;
    prim = func_801EF9AC(ot, prim, 4096, g_menuTint[MENU_TINT_NORMAL]);
    return prim;
}

/**
 * @brief Renders a string of up to seven characters.
 *
 * Retrieves the character sequence associated with the string ID and renders
 * up to seven characters horizontally, advancing the X coordinate by 18 pixels
 * for each character. The generated primitives are linked into the ordering
 * table.
 *
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the first character.
 * @param y Y coordinate of the characters.
 * @param strId Menu string ID for the entry to draw.
 * @param minRowIndex Unused.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EA8F0(void *ot, DR_TPAGE *prim, s32 x, s32 y, u16 strId, s32 minRowIndex) {
    s32 head;
    s32 head2;
    s32 i;
    s32 ch;
    u8* ptr;
    u32 clutFlags;

    ptr = func_801E9E7C(strId);
    getAddrFast(ot, head);
    clutFlags = 7;

    // Dead code added to match with the original game binary.
    x++; x--;

    for (i = 0; i < 7; i++) {
        ch = *ptr++;
        getNibbleValue(ch - 32);
        head = func_801F0F20(head, prim, ch - 32, clutFlags, (y << 0x10) | (x & 0xFFFF));
        prim = (DR_TPAGE *)((s32)prim + 20);
        x += 18;
    }
    setDrawTPage(prim, 1, 0, 0x1F);

    // Dead code added to match with the original game binary.
    if (head2) { head2++; head2--; }

    head2 = (s32)prim << 8;
    setAddrFast(prim, head);
    head = head2;

    // Dead code added to match with the original game binary.
    if (head2) { head2++; head2--; }
    
    prim++;
    setAddrFast(ot, head);
    return prim;
}

/**
 * @brief Renders the character grid.
 *
 * The grid is rendered from a sequence of strings, each containing up to
 * seven characters. Even-indexed strings are placed in the left column,
 * while odd-indexed strings are placed in the right column. Each pair of
 * consecutive strings shares the same row, with 16 pixels of vertical
 * spacing between rows and an additional 8 pixels after every two rows.
 *
 * @param s Pointer to the entity naming menu state.
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the character grid.
 * @param y Y coordinate of the character grid.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EAA04(EntityNamingMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    s32 i;
    CharacterGrid *charGrid;
    u16 *strIds;
    s32 strXOffset;
    s32 minRowIndex;
    u16 strId;
    s32 strX;
    s32 strY;
    i = 0;
    charGrid = &D_801EB320[s->charGridIndex];
    strIds = charGrid->strIds;
    strXOffset = charGrid->strXOffset;
    minRowIndex = charGrid->minRowIndex;
    while (1) {
        strId = *strIds++;
        if (strId == 0xFFFF) {
            break;
        }
        strX = x + strXOffset * (i % 2);
        strY = y + (i / 2) * 16;
        strY += (i / 4) * 8;
        prim = func_801EA8F0(ot, prim, strX, strY, strId, minRowIndex);
        i++;
    }
    return prim;
}

/**
 * @brief Renders the character grid panel for the entity naming menu.
 *
 * Character grid rendering is handled by func_801EAA04.
 *
 * @param s Pointer to the entity naming menu state.
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the panel.
 * @param y Y coordinate of the panel.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EAB00(EntityNamingMenuState *s, void *ot, void *prim, s32 x, s32 y) {
    prim = func_801EAA04(s, ot, prim, x + 8, y + 10);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 258;
    g_menuDisplayCfg.h = 125;
    return func_801EF9AC(ot, prim, 4096, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Draws the title panel for the entity naming menu.
 * 
 * The panel displays the title "Name Input".
 * 
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @param x X coordinate of the panel.
 * @param y Y coordinate of the panel.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EAB8C(void *ot, void *prim, s32 x, s32 y) {
    s32 strX = x + 7;
    s32 strY = y + 6;
    prim = func_801F0FEC(ot, prim, strX, strY, func_801E9E7C(STRING_NAME_INPUT), DIALOG_COLOR_WHITE);
    g_menuDisplayCfg.iconType = 0;
    g_menuDisplayCfg.iconSubType = 0;
    g_menuDisplayCfg.x = x;
    g_menuDisplayCfg.y = y;
    g_menuDisplayCfg.w = 90;
    g_menuDisplayCfg.h = 22;
    return func_801EF9AC(ot, prim, 4096, g_menuTint[MENU_TINT_NORMAL]);
}

/**
 * @brief Renders the entity naming menu.
 *
 * Update the menu brightness from the entity naming state and draws
 * the entity's portrait, editable name, help text, character grid,
 * action options, title, and finally updates the menu brightness.
 * Skips rendering if the display mode is not 15.
 *
 * @param s Pointer to the entity naming menu state.
 * @param ot Pointer to the ordering table.
 * @param prim Next free address in the primitive buffer.
 * @return Updated next free address in the primitive buffer.
 */
static void *func_801EAC54(EntityNamingMenuState *s, void *ot, void *prim) {
    s32 y;
    s32 menuBrightness = g_menuBrightness;

    setMenuBrightness(s->menuBrightness);
    if (func_801F0D84() != 15) {
        return prim;
    }
    prim = func_801EA500(ot, prim, 48, 34, s->namingCfg->portraitIdx);
    y = 55;
    prim = func_801EA538(s, ot, prim, 106, y);
    prim = func_801EA714(ot, prim, 106, 29);
    y = 88;
    prim = func_801EA7E0(s, ot, prim, 24, y);
    prim = func_801EAB00(s, ot, prim, 106, y);
    prim = func_801EAB8C(ot, prim, 270, 6);
    setMenuBrightness(menuBrightness);
    return prim;
}

/**
 * @brief Initializes the entity naming menu.
 *
 * Registers the entity naming menu update and render callbacks,
 * reads the naming configuration index from the current menu value.
 * When the index identifies a GF (3 through 18), sets a field flag.
 * Initializes the name pointer with the current assigned value.
 * If bit 7 flag is set, the name pointer is initialized with the
 * default value instead.
 */
static void func_801EAD64(void) {
    EntityNamingMenuState *s;
    u32 index;
    u8* defaultName;
    GameState *gs;
    s = func_801F179C(func_801E9F94, func_801EAC54);
    func_801F0948(0);
    index = func_801F000C() & 0x7F;
    if (index - 3 < 16) {
        setFieldFlag(index + 70);
    }
    if (s != NULL) {
        gs = &g_gameState;
        s->namingCfg = &D_801EB240[func_801F000C() & 0x7F];
        s->namePtr = (u8 *)gs + s->namingCfg->assignedNameOffset;
        defaultName = func_801E9E7C(s->namingCfg->defaultNameIdx);
        if (!(func_801F000C() & 0x80)) {
            copyString(s->namePtr, defaultName);
        }
        s->charGridIndex = 0;
        s->cursorPanelIndex = 0;
        s->charGridCursorIndex = 0;
        s->menuBrightness = 0;
        s->menuOptionCursorIndex = s->charGridIndex;
        func_801E9F94(s);
    }
}
