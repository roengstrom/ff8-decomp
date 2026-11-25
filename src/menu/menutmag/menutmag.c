#include "common.h"
#include "btl_anim.h"
#include "btl_anim_packet.h"
#include "menu.h"
#include "menu_tint.h"
#include "menumain2.h"
#include "menutmag.h"
#include "menututo.h"
#include "overlay.h"
#include "psxsdk/libetc.h"
#include "snd_sfx.h"
#include "ui/dialog.h"
#include "ui/sprite.h"
#include "ui/text.h"
#include "ui/window.h"

typedef struct {
    /** 0x00 */ s32 task[4];
    /** 0x10 */ u16 state;
    /** 0x12 */ u8 pad12[0xE];
    /** 0x20 */ s32 unk20;
    /** 0x24 */ s32 unk24;
    /** 0x28 */ s32 unk28;
    /** 0x2C */ s32 unk2C;
} func_801E5800_arg0;

extern u8 D_801E764C[];
extern char D_801E63EC[];
extern MagazinePageEntry D_801E63F8[];

static void func_801E5800(func_801E5800_arg0 *task);
static void *func_801E5AF0(void *ot, PaperMatPrim *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage);
static u8 *func_801E5C0C(s32 textId);
static SpriteTable *func_801E5C28(void);
static void *func_801E5C34(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage);
static void *func_801E5D1C(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage);
static void *func_801E5E2C(void *ot, void *head, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage);
static void *func_801E5F5C(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage);
static void *func_801E6008(void *ot, void *prim, s32 x, s32 y, MagazinePageEntry *magPage);
static void *func_801E606C(void *ot, void *prim, MagazinePageEntry *magPage);
static void *func_801E6170(func_801E5800_arg0 *task, void *ot, void *prim);
static void func_801E6290(void);

static void func_801E5800(func_801E5800_arg0 *task) {
    u16 inputNew = g_menuDisplayCfg.inputNew;
    u16 *statePtr = &task->state;
    s32 page;
    MagazinePageEntry *entry;

    switch (task->state) {
    case 0:
        *statePtr = 1;
    case 1:
        *statePtr = 5;
    case 2:
        task->unk24 = 0x1000;
        *statePtr = 3;
    case 3:
        if (pollCdReadStatus() == 0) {
            task->unk28 = task->unk2C;
            loadTimImage((u8 *)MENU_SUBOVERLAY_ADDR);
            *statePtr = 4;
        }
        break;
    case 4:
        task->unk20 += 0x100;
        if (task->unk20 >= 0x1000) {
            task->unk20 = 0x1000;
            *statePtr = 8;
        }
        break;
    case 5:
        *statePtr = 6;
    case 6:
        if (pollCdReadStatus() == 0) {
            task->unk28 = task->unk2C;
            loadTimImage((u8 *)MENU_SUBOVERLAY_ADDR);
            *statePtr = 7;
        }
        break;
    case 7:
        task->unk24 += 0x100;
        if (task->unk24 >= 0x1000) {
            task->unk24 = 0x1000;
            *statePtr = 8;
        }
        break;
    case 8:
        *statePtr = 9;
        break;
    case 9:
        if (inputNew & PADRdown) {
            page = task->unk28;
            sendSpuCommand(2);
            page++;
            if (page > func_801E2810()) {
                *statePtr = 0xD;
            } else {
                task->unk2C = page;
                *statePtr = 0xA;
            }
        } else if (inputNew & PADLleft) {
            page = task->unk28;
            sendSpuCommand(2);
            page--;
            if (page < func_801E2800()) {
                page = func_801E2810();
            }
            task->unk2C = page;
            *statePtr = 0xA;
        } else if (inputNew & PADLright) {
            page = task->unk28;
            sendSpuCommand(2);
            page++;
            if (page > func_801E2810()) {
                page = func_801E2800();
            }
            task->unk2C = page;
            *statePtr = 0xA;
        } else if (inputNew & PADRup) {
            sendSpuCommand(3);
            *statePtr = 0xD;
        }
        break;
    case 0xA:
        entry = D_801E63F8;
        entry += task->unk2C;
        func_801E2820(entry->pageTextureCategory, entry->pageTexture);
        *statePtr = 0xB;
        break;
    case 0xB:
        *statePtr = 0xC;
    case 0xC:
        task->unk24 -= 0x100;
        if (task->unk24 <= 0) {
            task->unk24 = 0;
            *statePtr = 6;
        }
        break;
    case 0xD:
        *statePtr = 0xE;
        break;
    case 0xE:
        task->unk20 -= 0x100;
        if (task->unk20 <= 0) {
            task->unk20 = 0;
            loadOverlayWithTimCallback(9, MENU_IMAGE_ADDR);
            loadOverlayWithTimCallback(10, MENU_UNK_801D5000_ADDR);
            *statePtr = 0xF;
        }
        break;
    case 0xF:
        if (pollCdReadStatus() == 0) {
            *statePtr = 0x10;
        }
        break;
    case 0x10:
        func_801F18FC(task->task);
        func_801F0BB0();
        break;
    }
}

static void *func_801E5AF0(void *ot, PaperMatPrim *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 r0;
    s32 g0;
    s32 b0;

    r0 = color & 0xFF;
    g0 = (color >> 8) & 0xFF;
    b0 = (color >> 0x10) & 0xFF;

    r0 = (r0 * magPage->paperMatRed) / 128;
    g0 = (g0 * magPage->paperMatGreen) / 128;
    b0 = (b0 * magPage->paperMatBlue) / 128;

    r0 &= 0xFF;
    g0 &= 0xFF;
    b0 &= 0xFF;

    if (magPage->paperMatHeight == 0 || magPage->paperMatWidth == 0) {
        return prim;
    }

    setPaperMatPrim(prim, 1, 0, 0);
    setRGB0(prim, r0, g0, b0);
    setXY0(prim, cfg->windowBackgroundRect.x + magPage->paperMatX, cfg->windowBackgroundRect.y + magPage->paperMatY);
    setWH(prim, magPage->paperMatWidth, magPage->paperMatHeight);

    addPrimFast(ot, prim);

    return prim + 1;
}

static u8 *func_801E5C0C(s32 textId) {
    Mngrp_string *table = (Mngrp_string *)D_801E764C;
    return D_801E764C + table->offsets[textId];
}

static SpriteTable *func_801E5C28(void) {
    return (SpriteTable *)MENU_IMAGE_ADDR;
}

static void *func_801E5C34(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 i;
    MagazinePageOverlay *picOverlay;

    picOverlay = magPage->pictureOverlays;
    for (i = 0; i < 4; i++, picOverlay++) {
        if (picOverlay->id != INVALID_ID) {
            s32 x = cfg->windowBackgroundRect.x + picOverlay->x;
            s32 y = cfg->windowBackgroundRect.y + picOverlay->y;
            prim = DrawSpriteFromTable(ot, prim, func_801E5C28(), picOverlay->id, x, y, color);
        }
    }
    return prim;
}

static void *func_801E5D1C(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 i;
    u32 pkt;
    MagazinePageOverlay *textOverlay;

    textOverlay = magPage->textOverlays;
    for (i = 0; i < 4; i++, textOverlay++) {
        if (textOverlay->id != INVALID_ID) {
            s32 x = cfg->windowBackgroundRect.x + textOverlay->x;
            s32 y = cfg->windowBackgroundRect.y + textOverlay->y;
            if (i & 1) {
                pkt = getDisplayListHead();
                storeGpuPacket((u32)prim);
            }
            drawMessageText(ot, x, y, func_801E5C0C(textOverlay->id));
            if (i & 1) {
                prim = (void *)getDisplayListHead();
                storeGpuPacket(pkt);
            }
        }
    }
    return prim;
}

static void *func_801E5E2C(void *ot, void *head, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    s32 clut;
    s32 drawModeCmd;
    s32 texWindowCmd;
    s32 winXRaw;
    s32 winXSignAdjust;
    s32 winYSignAdjust;
    s32 sliceXOffset;
    s32 sliceWidth;
    s32 remainingWidth;
    s32 xOffset;
    DR_TWIN *twin;
    PaperMatSlicePrim *prim;

    clut = getClut(512, 219);
    winXRaw = (magPage->paperBgParamA + 896) * 4;
    drawModeCmd = _get_mode(1, 0, getTPage(1, 0, magPage->paperBgParamA + 896, 0));
    winXSignAdjust = winXRaw;
    sliceXOffset = magPage->paperBgParamB;
    if (winXSignAdjust < 0) {
        winXSignAdjust += 255;
    }
    winYSignAdjust = sliceXOffset;
    winXRaw = ((winXRaw - (winXSignAdjust & 0x3F00)) >> 3) << 0xA;
    if (winYSignAdjust < 0) {
        winYSignAdjust += 255;
    }
    texWindowCmd = winXRaw | (((sliceXOffset - (winYSignAdjust & 0x100)) >> 3) << 0xF) | 0xE200039C;
    remainingWidth = magPage->windowWidth;

    twin = head;
    setlen(twin, 2);
    twin->code[0] = TEXWINDOW_OFF;
    twin->code[1] = 0;
    addPrimFast(ot, twin);
    head = twin + 1;
    prim = head;

    xOffset = 0;
    while (remainingWidth > 0) {
        sliceWidth = remainingWidth;
        if (remainingWidth > 64) {
            sliceWidth = 64;
        }
        *(u32 *)&prim->sprt.r0 = color;
        setPaperMatSlicePrim(prim, drawModeCmd, texWindowCmd);
        setXY0(&prim->sprt, cfg->windowBackgroundRect.x + xOffset, cfg->windowBackgroundRect.y);
        setWH(&prim->sprt, sliceWidth, cfg->windowBackgroundRect.h);
        prim->sprt.clut = clut;
        setUV0(&prim->sprt, 0, 0);
        remainingWidth -= sliceWidth;
        addPrimFast(ot, prim);
        prim++;
        xOffset += 64;
    }
    return prim;
}

static void *func_801E5F5C(void *ot, void *prim, MenuDisplayConfig *cfg, u32 color, MagazinePageEntry *magPage) {
    prim = func_801E5D1C(ot, prim, cfg, color, magPage);
    prim = func_801E5C34(ot, prim, cfg, color, magPage);
    prim = func_801E5AF0(ot, prim, cfg, color, magPage);
    prim = func_8002B898(ot, prim, &cfg->windowBackgroundRect, color);
    return func_801E5E2C(ot, prim, cfg, color, magPage);
}

static void *func_801E6008(void *ot, void *prim, s32 x, s32 y, MagazinePageEntry *magPage) {
    g_menuDisplayCfg.windowBackgroundRect.x = magPage->windowX + x;
    g_menuDisplayCfg.windowBackgroundRect.y = magPage->windowY + y;
    g_menuDisplayCfg.windowBackgroundRect.w = magPage->windowWidth;
    g_menuDisplayCfg.windowBackgroundRect.h = magPage->windowHeight;
    return func_801E5F5C(ot, prim, &g_menuDisplayCfg, g_menuTint[MENU_TINT_NORMAL], magPage);
}

static void *func_801E606C(void *ot, void *prim, MagazinePageEntry *magPage) {
    u8 *navigationHint;
    s32 x;
    s32 y;
    u16 width;
    u32 menuTint;

    menuTint = g_menuTint[MENU_TINT_NORMAL];
    navigationHint = func_801F08D4(1, 0xD, 0x1C, 0);
    width = measureMessage(navigationHint);
    x = (336 - width) / 2;
    x += 24;
    y = 200;
    prim = (void *)func_801F0FEC(ot, prim, x, y, navigationHint, DIALOG_COLOR_WHITE);
    g_menuDisplayCfg.windowBackgroundRect.x = 24;
    g_menuDisplayCfg.windowBackgroundRect.y = 196;
    g_menuDisplayCfg.windowBackgroundRect.w = 336;
    g_menuDisplayCfg.windowBackgroundRect.h = 20;
    prim = func_8002B898(ot, prim, &g_menuDisplayCfg.windowBackgroundRect, menuTint);
    return func_801E5E2C(ot, prim, &g_menuDisplayCfg, menuTint, magPage);
}

static void *func_801E6170(func_801E5800_arg0 *task, void *ot, void *prim) {
    MagazinePageEntry *entry;

    if (task->unk20 == 0) {
        return prim;
    }
    func_801F1AFC();
    setMenuBrightness(task->unk20 * task->unk24 / 4096);
    setTextBrightness(task->unk20 * task->unk24 / 4096);
    entry = D_801E63F8;
    entry += task->unk28;
    prim = func_801E6008(ot, prim, 0, 0, entry);
    setMenuBrightness(task->unk20);
    setTextBrightness(task->unk20);
    if (entry->footerFlag != 0) {
        prim = func_801E606C(ot, prim, entry);
    }
    func_801F1B10();
    return prim;
}

static void func_801E6290(void) {
    func_801E5800_arg0 *task;
    s32 page;
    MagazinePageEntry *entry;

    task = func_801F179C(func_801E5800, (MenuDrawCallback)func_801E6170);
    func_801F1D2C(0, (s32)D_801E63EC, (s32)D_801E63F8);
    if (task != NULL) {
        page = func_801E2800();
        task->unk28 = page;
        task->unk2C = page;
        entry = &D_801E63F8[page];
        loadSubOverlay(entry->textFileIndex + 0x57, (s32)D_801E764C);
        func_801E2820(entry->pageTextureCategory, entry->pageTexture);
        func_801E5800(task);
    }
}

void func_801E6348(void) {
    func_801E6290();
}

void func_801E6368(void) {
    func_801E6290();
}

void func_801E6388(void) {
    func_801E6290();
}
