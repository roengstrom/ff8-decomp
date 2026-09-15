#include "common.h"
#include "psxsdk/libgpu.h"
#include "main.h"
#include "render.h"
#include "gamestate.h"

extern DRAWENV D_80082CD0;
extern DRAWENV D_80082D2C;
extern DISPENV D_80082D90;
extern DISPENV D_80082DA4;
extern DISPENV D_80082DB8;
extern s16 D_80082DC2;
extern s16 D_80082DAE;
extern s16 D_80082D9A;
extern s16 D_80082DC6;
extern s16 D_80082DB2;
extern s16 D_80082D9E;
extern RECT D_8005EC24;
extern u8 D_8005F17C;
extern u8 D_8005F17D;
extern u8 D_8005F17F;
extern void func_80024064(void);
extern void func_80026E70(void);
extern void initMeshRenderer(void);

/**
 * @brief Initialize double-buffered draw/disp envs for the item/mesh render path.
 *
 * Sets up DRAWENV/DISPENV pairs, builds a one-shot OT that copies the active
 * display window into the 0x180,0x100 VRAM staging area via SetDrawMove, presents
 * it, blits D_8005EC24 into both framebuffers, then either runs the item overlay
 * init (func_80024064) or initMeshRenderer based on @p arg0.
 */
void func_80023D60(register s32 arg0) {
    register u32 *ot;
    register u16 *meshFlag;
    register u8 *pkt2;
    register u8 *pkt;
    u8 flag;
    s32 unused1;
    s32 unused2;
    s32 unused3;
    s32 unused4;
    s32 unused5;
    s32 unused6;
    s32 unused7;
    s32 unused8;
    s32 unused9;
    s32 unused10;
    s32 unused11;
    s32 unused12;
    s32 unused13;

    flag = arg0;
    g_renderMode = 0;
    meshFlag = (u16 *)0x801F0000;
    meshFlag[1] = g_bufferIndex & 1;
    VSync(0);
    SetDefDrawEnv(&D_80082CD0, 0, 0, 0x140, 0xE0);
    SetDefDrawEnv(&D_80082D2C, 0x140, 0, 0x140, 0xE0);
    SetDefDispEnv(&D_80082D90, 0x140, 0, 0x140, 0xE0);
    SetDefDispEnv(&D_80082DA4, 0, 0, 0x140, 0xE0);
    SetDefDispEnv(&D_80082DB8, 0x180, 0x100, 0x140, 0xE0);
    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(0x200);
    D_80082DC2 = 8;
    D_80082DAE = 8;
    D_80082D9A = 8;
    D_80082DC6 = 0xE0;
    D_80082DB2 = 0xE0;
    D_80082D9E = 0xE0;
    D_8005F17F = flag;
    ot = (u32 *)0x801DC000;
    ClearOTag(ot, 1);
    pkt = (u8 *)ot + 0x10;
    SetDrawStp((u32 *)pkt, 0);
    AddPrim(ot, pkt);
    pkt += 0xC;
    pkt2 = pkt;
    SetDrawMove((DR_MOVE *)pkt2, (RECT *)D_8005F138, 0x180, 0x100);
    AddPrim(ot, pkt2);
    pkt2 += 0x18;
    pkt = pkt2;
    SetDrawStp((u32 *)pkt, 1);
    AddPrim(ot, pkt);
    pkt += 0xC;
    DrawOTag(ot);
    DrawSync(0);
    VSync(0);
    PutDispEnv(&D_80082DB8);
    MoveImage(&D_8005EC24, 0, 0);
    MoveImage(&D_8005EC24, 0x140, 0);
    if (D_8005F17F == 0) {
        func_80024064();
    } else {
        initMeshRenderer();
    }
    DrawSync(0);
    D_8005F17C = 0;
    D_8005F17D = 0;
    func_80026E70();
    g_renderMode = 2;
}

INCLUDE_ASM("asm/nonmatchings/item_render", func_80024064);

INCLUDE_ASM("asm/nonmatchings/item_render", func_800242C8);
