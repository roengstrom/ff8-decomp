#include "common.h"
#include "memcard.h"
#include "battle.h"
#include "menu.h"
#include "menumain.h"
#include "menumain2.h"
#include "menusts.h"
#include "numstr.h"
#include "gamestate.h"
#include "ui/icon.h"
#include "ui/font.h"
#include "game.h"

/** @brief Look up value from D_801FA3C8 table by dividing input by 64. */
u16 func_801E5800(s32 a0) {
    return D_801FA3C8[a0 / 64];
}

/**
 * @brief Look up status display type byte.
 *
 * @param index Status entry index.
 * @return Type byte.
 */
u8 func_801E582C(s32 index) {
    MenustsStatusEntry *entry = D_801E9964;
    entry += index;
    return entry->type;
}

/**
 * @brief Compute status display x-position.
 *
 * @param index Status entry index.
 * @return Computed address.
 */
s32 func_801E5848(s32 index) {
    MenustsStatusEntry *entry = D_801E9964;
    u8 *coordBase = D_801E99AC;
    entry += index;
    return (s32)coordBase + entry->xOff;
}

/**
 * @brief Compute status display y-position.
 *
 * @param index Status entry index.
 * @return Computed address.
 */
s32 func_801E586C(s32 index) {
    MenustsStatusEntry *entry = D_801E9964;
    u8 *coordBase = D_801E99AC;
    entry += index;
    return (s32)coordBase + entry->yOff;
}

/**
 * @brief Wrapper that calls getMagicNamePtr with @p a0 offset by 0x33.
 */
void func_801E5890(s32 a0) {
    getMagicNamePtr(a0 + 0x33);
}

/**
 * @brief Wrapper that calls getSpellDesc with @p a0 offset by 0x33.
 */
void func_801E58B0(s32 a0) {
    getSpellDesc(a0 + 0x33);
}

/** @brief Look up string @p a0 in menu text category 7. */
u8 *func_801E58D0(s32 a0) {
    return func_801F08D4(1, 7, a0, 0);
}

/** @brief Look up string @p a0 in menu text category 7, with the last flag set. */
u8 *func_801E58FC(s32 a0) {
    return func_801F08D4(1, 7, a0, 1);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5928);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E5D28);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E61B0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6254);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E62A8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E68A4);

/**
 * @brief Configure and draw a status menu panel border.
 *
 * Sets up g_menuDisplayCfg with no icon, position from a2/a3,
 * fixed size (0xF4 x 0x12), then draws via DrawMenuWindow.
 *
 * @param a0 Display list pointer
 * @param a1 OT pointer
 * @param a2 X position
 * @param a3 Y position
 */
void func_801E6994(s32 a0, s32 a1, s16 a2, s16 a3) {

    s32 cfg = (s32)&g_menuDisplayCfg;

    *(u8 *)(cfg + 0x10) = 0;
    *(u8 *)(cfg + 0x11) = 0;
    *(s16 *)cfg = a2;
    *(s16 *)(cfg + 0x4) = 0xF4;
    *(s16 *)(cfg + 0x6) = 0x12;
    *(s16 *)(cfg + 0x2) = a3;
    DrawMenuWindow(a0, a1, ONE, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E69E4);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6B84);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6C68);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E6EA0);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E709C);

/** @brief Build status bitfield from character data flags. */
s32 func_801E7278(BattleCharData *cd) {
    s32 result = cd->atkPersistentStatus & 0x7F;
    s32 flags = cd->atkBattleStatus;

    if (flags & 0x1) result |= 0x80;
    if (flags & 0x4) result |= 0x100;
    if (flags & 0x8) result |= 0x200;
    if (flags & 0x200) result |= 0x400;
    if (flags & 0x4000) result |= 0x800;
    if (flags & 0x8000) result |= 0x1000;
    return result;
}

/**
 * @brief Render character status icons + atkStatusHit values.
 *
 * Walks D_801E95CC (status entry table, sentinel 0xFF) and for each
 * status flag set in the character's bitfield, draws the status text
 * icon and the (atkStatusHit - 100) numeric value, up to 3 entries.
 * Then draws the surrounding panel via DrawMenuWindow if any entries
 * were drawn.
 *
 * @param displayList GPU display list pointer.
 * @param ot          Ordering table pointer (returned chained).
 * @param x           Panel base X position.
 * @param y           Panel base Y position.
 * @param mode        Render mode passed to DrawMenuWindow.
 * @return Updated OT pointer.
 */
s32 func_801E72D8(s32 displayList, s32 ot, s32 x, s32 y, s32 mode) {
    u8 strBuf[16];
    StatusEntry *entry = D_801E95CC;
    s32 i = 0;
    s32 digitBase = g_numberFormat.digits[0];
    s32 drawn;
    s32 yStep;
    MenuDisplayConfig *cfgPtr;
    u8 *strBuf2;
    void *bgBuf;
    s32 statusFlags;
    s32 yPos;

    cfgPtr = &g_menuDisplayCfg;
    bgBuf = func_801F6AFC(0x14);
    drawn = i;
    statusFlags = func_801E7278(&D_801E9EE4);
    strBuf2 = &strBuf[2];
    yStep = 0xA;

    while (1) {
        s32 mask;
        if (entry->statusId == 0xFF) break;
        mask = 1 << i;
        if (statusFlags & mask) {
            if (drawn >= 3) break;
            yPos = y + yStep;
            ot = drawIcon(displayList, ot, entry->statusId, x + 0xA, yPos, g_menuTint[MENU_TINT_NORMAL]);
            drawn++;
            yPos += 4;
            intToDecStringShort(D_801E9EE4.atkStatusHitChance - 100, strBuf, digitBase);
            replaceLeadingZeros(strBuf2, 2, digitBase, *getMenuString(0xB));
            ot = func_8002C56C(displayList, ot, x + 0x5A, yPos, strBuf2, 7);
            ot = func_8002C56C(displayList, ot, x + 0x72, yPos + 1, bgBuf, 7);
            yStep += 0xE;
        }
        i++;
        entry++;
    }

    if (drawn != 0) {
        if (GetGameVersion() == VERSION_SPAIN) {
            *(u8 *)((s32)cfgPtr + 0x10) = 0xFC;
        } else {
            *(u8 *)((s32)cfgPtr + 0x10) = 0xD0;
        }
        cfgPtr->iconSubType = 0;
        cfgPtr->x = x;
        cfgPtr->y = y;
        cfgPtr->w = 0x82;
        cfgPtr->h = 0x38;
        ot = DrawMenuWindow(displayList, ot, mode, g_menuTint[MENU_TINT_NORMAL]);
    }

    return ot;
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E750C);

/**
 * @brief Compute centered X position after subtracting rendered width.
 * @param a0 Base X position
 * @param a1 Total available width
 * @param a2 Text or item to measure via getFirstLineWidth
 * @return Base X + (available width - rendered width) / 2
 */
s32 func_801E7644(s32 a0, s32 a1, u8 *a2) {
    s32 rendered = getFirstLineWidth(a2);
    return a0 + (a1 - rendered) / 2;
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7690);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7B20);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E7D04);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E811C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E81B8);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8598);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E86F0);

/**
 * @brief Status menu render wrapper with stat type 6.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8850(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 6);
}

/**
 * @brief Status menu render wrapper with stat type 7.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8890(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 7);
}

/**
 * @brief Status menu render wrapper with stat type 8.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E88D0(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 8);
}

/**
 * @brief Status menu render wrapper with stat type 9.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8910(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 9);
}

/**
 * @brief Status menu render wrapper with stat type 0xA.
 * @param a0 Unused parameter
 * @param a1 First render parameter
 * @param a2 Second render parameter
 * @param a3 Third render parameter
 * @param arg5 Fourth render parameter
 * @param arg6 Fifth render parameter
 */
void func_801E8950(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6) {
    func_801E750C(a1, a2, a3, arg5, arg6, 0xA);
}

/**
 * @brief Dispatch a status rendering operation through a function table.
 *
 * Looks up a function pointer from D_801E961C[a1], calls it with
 * (a0, a2, a3, stackArg1, stackArg2), then passes the result and
 * other parameters to DrawMenuWindow for GPU display.
 *
 * @param a0 Status entry data pointer
 * @param a1 Function table index
 * @param a2 Parameter passed as a1 to table function, then as a0 to DrawMenuWindow
 * @param a3 Parameter passed as a2 to table function
 * @param stackArg1 Passed as a3 to table function, then as stack arg to DrawMenuWindow
 * @param stackArg2 Passed as stack arg to table function, then as a2 to DrawMenuWindow
 */
void func_801E8990(s32 a0, s32 a1, s32 a2, s32 a3, s32 arg5, s32 arg6, s32 arg7) {
    s32 (*fn)(s32, s32, s32, s32, s32, s32) = (void *)D_801E961C[a1];
    s32 result = fn(a0, a2, a3, arg5, arg6, arg7);
    DrawMenuWindow(a2, result, arg7, g_menuTint[MENU_TINT_NORMAL]);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8A08);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8BAC);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E8CE4);

/**
 * @brief Conditionally transform a value before passing to the status renderer.
 *
 * If the signed byte at a0+0x3B is non-negative, calls func_801F6FE4
 * with (a1, a2, 1, 0x1000) and uses the result as a2. Then calls
 * func_801E8CE4 with the original a0, a1, and the (possibly modified) a2.
 *
 * @param a0 Pointer to status entry structure.
 * @param a1 First render parameter.
 * @param a2 Second render parameter (may be replaced by func_801F6FE4 result).
 */
void func_801E9118(u8 *a0, s32 a1, s32 a2) {
    s8 val = *(s8 *)(a0 + 0x3B);
    if (val >= 0) {
        a2 = func_801F6FE4(a1, a2, 1, 0x1000);
    }
    func_801E8CE4(a0, a1, a2);
}

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E9178);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E923C);

INCLUDE_ASM("asm/ovl/menusts/nonmatchings/menusts", func_801E936C);
