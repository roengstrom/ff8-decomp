#include "common.h"
#include "psxsdk/libc.h"
#include "psxsdk/libetc.h"
#include "psxsdk/libgpu.h"
#include "gamestate.h"
#include "kernel.h"
#include "battle_render.h"

// A debug text request: x and y count 8-pixel character cells.
typedef struct {
    /** 0x00 */ u32 *ot;
    /** 0x04 */ s16 x;
    /** 0x06 */ s16 y;
    /** 0x08 */ s16 color; // CLUT row above 0xE0
    /** 0x0A */ u16 pad0A;
    /** 0x0C */ u8 *format;
} CmdBlk;

typedef struct {
    /** 0x00000 */ DRAWENV unk0;
    /** 0x0005C */ DISPENV unk5C;
    /** 0x00070 */ u32 unk70[0x1000];
    /** 0x04070 */ u32 unk4070;
    /** 0x04074 */ u32 unk4074;
    /** 0x04078 */ DR_MODE drawModes[0x400];
    /** 0x07078 */ SPRT_8 sprt8[0x400];
    /** 0x0B078 */ SPRT_16 sprt16[0x200];
    /** 0x0D078 */ SPRT sprt[0x200];
    /** 0x0F878 */ POLY_G4 polyG4[0x200];
} BattleRenderCtx;

typedef struct {
    /** 0x00 */ u32 *ot;
    /** 0x04 */ s16 x;
    /** 0x06 */ s16 y;
    /** 0x08 */ u8 u;
    /** 0x09 */ u8 pad09;
    /** 0x0A */ u8 v;
    /** 0x0B */ u8 pad0B;
    /** 0x0C */ s16 color;
} func_8009AE1C_arg0;

typedef struct {
    /** 0x00 */ u32 *ot;
    /** 0x04 */ s16 x;
    /** 0x06 */ s16 y;
    /** 0x08 */ u8 u;
    /** 0x09 */ u8 pad09;
    /** 0x0A */ u8 v;
    /** 0x0B */ u8 pad0B;
    /** 0x0C */ s16 w;
    /** 0x0E */ s16 h;
    /** 0x10 */ u16 clut;
    /** 0x12 */ u16 pad12;
    /** 0x14 */ s32 tpage;
} func_8009A8B4_arg0;

typedef struct {
    /** 0x00 */ char text[0x100];
} func_8009B170_scratch;

extern BattleRenderCtx *D_800D3C58;
extern s32 D_8009B54C[];
extern s32 D_800AB9F8[];
extern s32 D_800ABA00[];
extern u8 D_80098000[];
extern u8 D_8009800C[];
extern u8 D_80098018[];
extern s32 D_800ABA08;
extern void RecordGamePadInput(s32 a0);
extern s32 GetPressedButtons(s32 idx, s32 offset);
// No prototype: the original called it with fewer arguments than it takes.
void func_8009AF64();
extern s16 D_800D3C70;
extern u8 D_8009801C[];
extern u8 D_80098020[];
extern u8 D_8009B5B4[];
extern u8 D_800A37D4[];
extern u8 D_800ABA0C;
extern u8 D_800D3C30[];
extern u8 D_800D3C35;
extern u8 D_800D3C36;
extern u8 D_800D3C38;
extern u8 D_800D3C39;
extern u8 D_800D3C3A;
extern u8 D_800D3C3B;
extern u8 D_800D3C3C;
extern u8 D_800D3C3D;
extern u8 D_800D3C3E;
extern u8 D_800D3C3F;
extern u8 D_800D3C44;
extern u8 D_800D3C45;
extern u8 D_800D3C46;
extern u8 D_800D3C47;
extern u8 D_800D3C48;
extern u8 D_800D3C49;
extern u8 D_800ABA10[][10];
extern s32 D_800ABB34;
extern s32 D_800D3C68;
extern s32 D_800D3C54;
extern s32 D_800D3C6C;
extern u8 D_8009B534[];

static void func_800988EC(s32 idx, u8 *src);
static void func_8009AA18(func_8009A8B4_arg0 *arg);
static void func_8009AC18(func_8009A8B4_arg0 *arg);
static void func_8009AE1C(func_8009AE1C_arg0 *arg);
static void func_8009B170(CmdBlk *cmd, s32 arg1, s32 arg2, s32 arg3);

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098238);

/**
 * @brief Emit three debug-string render commands for the build banner.
 *
 * Stages three CmdBlk entries through the scratchpad and dispatches each
 * via func_8009AF64. The commands draw the build date string ("Jul  4
 * 1999"), the build time ("21:03:43"), and a formatted version field
 * (" %x" with D_800ABA08) into the render context's second ordering
 * table.
 */
void func_80098688(void) {
    CmdBlk *cmd = (CmdBlk *)getScratchAddr(0);

    cmd->x = 0x1A;
    cmd->y = 0x16;
    cmd->color = 1;
    cmd->ot = &D_800D3C58->unk4070;
    cmd->format = D_80098000;
    func_8009AF64(cmd);

    cmd->x = 0x1D;
    cmd->y = 0x17;
    cmd->color = 1;
    cmd->ot = &D_800D3C58->unk4070;
    cmd->format = D_8009800C;
    func_8009AF64(cmd);

    cmd->x = 0x1A;
    cmd->y = 0x1A;
    cmd->ot = &D_800D3C58->unk4070;
    cmd->color = 1;
    cmd->format = D_80098018;
    func_8009AF64(cmd, D_800ABA08);
}

/**
 * @brief Initialize render state flags.
 *
 * Sets flag at offset 0x18 to 1 and clears flags at 0x19-0x1B.
 *
 * @param a0 Pointer to render state structure.
 */
void func_80098768(u8 *a0) {
    a0[0x18] = 1;
    a0[0x19] = 0;
    a0[0x1A] = 0;
    a0[0x1B] = 0;
}

void func_80098780(void) {
}

void func_80098788(void) {
}

void func_80098790(void) {
}

/**
 * @brief Update animation frame state with timer-based debounce.
 *
 * Calls RecordGamePadInput then samples GetPressedButtons for slot @p a0.
 * If the frame param is unchanged from the previous tick, decrements
 * D_800AB9F8 (timer1) or D_800ABA00 (timer2) until both expire, at
 * which point the new state is committed (D_800ABA00 = 0 if bit 0x40
 * set, else 2). On a change, timer1 is reset to 8 and timer2 set per
 * the bit 0x40 flag.
 *
 * @param a0 Animation slot index (0 or 1).
 * @return Last-committed previous-state value (caller may ignore).
 */
s32 func_80098798(s32 a0) {
    s32 prevArr[2];
    s32 currArr[2];
    s32 prevVal;
    s32 v;

    RecordGamePadInput(a0);
    v = GetPressedButtons(a0, 0);
    currArr[a0] = v;
    prevVal = D_8009B54C[a0];

    if (prevVal == v) {
        if (D_800AB9F8[a0] == 0) {
            if (D_800ABA00[a0] == 0) {
                prevArr[a0] = prevVal;
                if (currArr[a0] & 0x40) {
                    D_800ABA00[a0] = 0;
                } else {
                    D_800ABA00[a0] = 2;
                }
            } else {
                D_800ABA00[a0] = D_800ABA00[a0] - 1;
                prevArr[a0] = 0;
            }
        } else {
            D_800AB9F8[a0] = D_800AB9F8[a0] - 1;
            prevArr[a0] = 0;
        }
    } else {
        D_800AB9F8[a0] = 8;
        if (currArr[a0] & 0x40) {
            D_800ABA00[a0] = 0;
        } else {
            D_800ABA00[a0] = 2;
        }
        prevArr[a0] = currArr[a0];
    }
    D_8009B54C[a0] = currArr[a0];
    return prevArr[a0];
}

/**
 * @brief Copy a null-terminated string into the name table.
 *
 * Copies bytes from src to D_800ABA10[idx * 10] until a null byte
 * is encountered (the null byte is also copied).
 *
 * @param idx Entry index in the name table (stride 10).
 * @param src Pointer to the source string.
 */
static void func_800988EC(s32 idx, u8 *src) {
    u8 *dst = D_800ABA10[idx];

    while ((*dst++ = *src++) != 0);
}

/**
 * @brief Set a name table entry from one of two default strings.
 *
 * If flag is zero, copies the string at D_8009801C into the name
 * table at index idx; otherwise copies D_80098020.
 *
 * @param idx Entry index in the name table.
 * @param flag Selects which default string to use.
 */
void func_80098920(s32 idx, s32 flag) {
    if (flag == 0) {
        func_800988EC(idx, D_8009801C);
    } else {
        func_800988EC(idx, D_80098020);
    }
}

/**
 * @brief Decode input flags into battle render configuration bytes.
 *
 * Reads bits from D_8007809A, @c GameConfig.sealedAbilities, and D_80077E92 and writes
 * corresponding flag bytes to the D_800D3C30 region. Then iterates
 * through 0x1D name table slots calling func_80098920 with values
 * from D_800D3C30.
 */
void func_80098958(void) {
    s32 i;

    if (g_gameState.mainData.partyLockFlag & 0x10) {
        D_800D3C35 = 1;
    } else {
        D_800D3C35 = 0;
    }

    if (g_gameState.mainData.partyLockFlag & 0x2) {
        D_800D3C36 = 1;
    } else {
        D_800D3C36 = 0;
    }

    if (g_gameState.mainData.partyLockFlag & 0x20) {
        D_800D3C38 = 1;
    } else {
        D_800D3C38 = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_ITEM) {
        D_800D3C39 = 1;
    } else {
        D_800D3C39 = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_MAGIC) {
        D_800D3C3A = 1;
    } else {
        D_800D3C3A = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_GF) {
        D_800D3C3B = 1;
    } else {
        D_800D3C3B = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_DRAW) {
        D_800D3C3C = 1;
    } else {
        D_800D3C3C = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_COMMAND_ABILITIES) {
        D_800D3C3D = 1;
    } else {
        D_800D3C3D = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_LIMIT_BREAK) {
        D_800D3C3E = 1;
    } else {
        D_800D3C3E = 0;
    }

    if (g_gameState.config.sealedAbilities & SEALED_RESURRECTION) {
        D_800D3C3F = 1;
    } else {
        D_800D3C3F = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & 0x2) {
        D_800D3C44 = 1;
    } else {
        D_800D3C44 = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & 0x4) {
        D_800D3C45 = 1;
    } else {
        D_800D3C45 = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & 0x8) {
        D_800D3C46 = 1;
    } else {
        D_800D3C46 = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & 0x20) {
        D_800D3C47 = 1;
    } else {
        D_800D3C47 = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & 0x40) {
        D_800D3C48 = 1;
    } else {
        D_800D3C48 = 0;
    }

    if (g_gameState.mainData.limitBreaks.angeloCompleted & 0x80) {
        D_800D3C49 = 1;
    } else {
        D_800D3C49 = 0;
    }

    for (i = 0; i < 0x1D; i++) {
        func_80098920(i, D_800D3C30[i]);
    }
}

/**
 * @brief Set battle render mode to active.
 *
 * Sets D_800ABA0C to 1 and D_800D3C70 to -0x18.
 */
void func_80098C68(void) {
    D_800ABA0C = 1;
    D_800D3C70 = -0x18;
}

/**
 * @brief Clear battle render mode.
 *
 * Clears D_800ABA0C and sets D_800D3C70 to 2.
 */
void func_80098C84(void) {
    D_800ABA0C = 0;
    D_800D3C70 = 2;
}

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_80098C9C);

void func_8009A368(void) {
    s32 id;
    s32 i;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i].exists |= CHAR_FLAG_PRESENT | CHAR_FLAG_UNK08;
    }
    g_gameState.mainData.party.partyMembers[0] = CHAR_SQUALL;
    g_gameState.mainData.party.partyMembers[1] = CHAR_ZELL;
    g_gameState.mainData.party.partyMembers[2] = CHAR_IRVINE;
    g_gameState.mainData.party.partyMembers[3] = PARTY_SLOT_EMPTY;
    g_gameState.gfs[0].exists |= GF_EXISTS;
    g_gameState.gfs[1].exists |= GF_EXISTS;
    g_gameState.gfs[2].exists |= GF_EXISTS;
    id = 1;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        g_gameState.mainData.itemSlots[i].id = id++;
        g_gameState.mainData.itemSlots[i].count = 10;
    }
}

void func_8009A40C(void) {
    ChocoboWorldData *chocobo = &g_gameState.chocobo;
    s32 id;
    s32 i;
    s32 j;

    for (i = 0; i < CHARACTER_COUNT; i++) {
        g_gameState.chars[i].exists |= CHAR_FLAG_PRESENT | CHAR_FLAG_UNK08;
    }
    g_gameState.chars[CHAR_SQUALL].weaponId = WEAPON_LION_HEART;
    g_gameState.chars[CHAR_ZELL].weaponId = WEAPON_EHRGEIZ;
    g_gameState.chars[CHAR_IRVINE].weaponId = WEAPON_EXETER;
    g_gameState.chars[CHAR_QUISTIS].weaponId = WEAPON_SAVE_THE_QUEEN;
    g_gameState.chars[CHAR_RINOA].weaponId = WEAPON_SHOOTING_STAR;
    g_gameState.chars[CHAR_SELPHIE].weaponId = WEAPON_STRANGE_VISION;
    for (i = 0; i < GF_COUNT; i++) {
        g_gameState.gfs[i].exists |= GF_EXISTS;
    }
    for (i = 0; i < GF_COUNT; i++) {
        g_gameState.gfs[i].completeAbilities[0] = -2;
        g_gameState.gfs[i].completeAbilities[1] = -1;
        g_gameState.gfs[i].completeAbilities[2] = -1;
        g_gameState.gfs[i].completeAbilities[3] = 0x3FFFFFF;
    }
    g_gameState.mainData.party.partyMembers[0] = CHAR_ZELL;
    g_gameState.mainData.party.partyMembers[1] = CHAR_SQUALL;
    g_gameState.mainData.party.partyMembers[2] = CHAR_SELPHIE;
    g_gameState.mainData.party.partyMembers[3] = PARTY_SLOT_EMPTY;
    g_gameState.mainData.limitBreaks.quistisLimits = 0xFFFF;
    chocobo->level = 10;
    chocobo->bokoAttack = 3;
    chocobo->flags |= CHOCOBO_FLAG_ENABLED | CHOCOBO_FLAG_MINIMOG_FOUND | CHOCOBO_FLAG_DEMON_KING_DEFEATED;
    chocobo->flags &= ~CHOCOBO_FLAG_IN_WORLD;
    for (j = 0; j < CHARACTER_COUNT; j += 2) {
        id = 1;
        for (i = 0; i < 30; i++) {
            g_gameState.chars[j].magic[i].magicId = id++;
            g_gameState.chars[j].magic[i].quantity = 50;
        }
        id = 31;
        for (i = 0; i < 20; i++) {
            g_gameState.chars[j + 1].magic[i].magicId = id++;
            g_gameState.chars[j + 1].magic[i].quantity = 50;
        }
    }
    g_gameState.chars[CHAR_SQUALL].commands[0] = 20;
    g_gameState.chars[CHAR_SQUALL].commands[1] = 22;
    g_gameState.chars[CHAR_SQUALL].commands[2] = 23;
    g_gameState.chars[CHAR_ZELL].commands[0] = 20;
    g_gameState.chars[CHAR_ZELL].commands[1] = 22;
    g_gameState.chars[CHAR_ZELL].commands[2] = 23;
    g_gameState.chars[CHAR_RINOA].commands[0] = 20;
    g_gameState.chars[CHAR_RINOA].commands[1] = 21;
    g_gameState.chars[CHAR_RINOA].commands[2] = 23;
    id = 1;
    for (i = 0; i < ITEM_SLOT_COUNT; i++) {
        g_gameState.mainData.itemSlots[i].id = id++;
        g_gameState.mainData.itemSlots[i].count = 10;
    }
}

/** @brief Call func_8003646C with display mode 0x16 and layer 7. */
void func_8009A60C(void) {
    func_8003646C(0x16, 7);
}

/**
 * @brief Compute checksum of g_kernel buffer.
 *
 * Sums 0x9E08 bytes from g_kernel and returns the total.
 *
 * @return Byte sum of the buffer.
 */
s32 func_8009A630(void) {
    s32 sum;
    u8 *p;
    u32 i;

    sum = 0;
    p = (u8 *)&g_kernel;
    for (i = 0; i < 0x9E08; i++) {
        sum += *p++;
    }
    return sum;
}

void func_8009A664(void) {
}

/**
 * @brief Process render object and dispatch collision checks.
 *
 * Calls OpenTIM to initialize, then ReadTIM to populate
 * a local buffer. Dispatches LoadImage with the third/fourth
 * words, and if bit 3 of the first word is set, also dispatches
 * with the second/third words.
 *
 * @param a0 Render object pointer.
 */
void func_8009A66C(u8 *a0) {
    s32 buf[6];
    OpenTIM(a0);
    ReadTIM(&buf);
    LoadImage(buf[3], buf[4]);
    if (((u32)buf[0] >> 3) & 1) {
        LoadImage(buf[1], buf[2]);
    }
}

/** @brief Call func_8009A66C with D_800A37D4 then D_8009B5B4. */
void func_8009A6CC(void) {
    func_8009A66C(D_800A37D4);
    func_8009A66C(D_8009B5B4);
}

/**
 * @brief Initialize render object with two display functions.
 *
 * Calls SetSemiTrans with mode 0, then SetShadeTex with mode 1,
 * both using the same object pointer.
 *
 * @param a0 Render object pointer.
 */
void func_8009A6FC(void *prim) {
    SetSemiTrans(prim, 0);
    SetShadeTex(prim, 1);
}

void func_8009A734(BattleRenderCtx *ctx) {
    SPRT_16 *sprt16;
    SPRT_8 *sprt8;
    SPRT *sprt;
    POLY_G4 *poly;
    s32 i;

    sprt16 = ctx->sprt16;
    for (i = 0; i < 0x200; i++, sprt16++) {
        setSprt16(sprt16);
        func_8009A6FC(sprt16);
        setRGB0(sprt16, 0x80, 0x80, 0x80);
    }
    sprt8 = ctx->sprt8;
    for (i = 0; i < 0x400; i++, sprt8++) {
        setSprt8(sprt8);
        func_8009A6FC(sprt8);
        setRGB0(sprt8, 0x80, 0x80, 0x80);
    }
    sprt = ctx->sprt;
    for (i = 0; i < 0x200; i++, sprt++) {
        setSprt(sprt);
        func_8009A6FC(sprt);
        setRGB0(sprt, 0x80, 0x80, 0x80);
    }
    poly = ctx->polyG4;
    for (i = 0; i < 0x200; i++, poly++) {
        setPolyG4(poly);
        func_8009A6FC(poly);
        setRGB0(poly, 0x80, 0x80, 0x80);
    }
}

void func_8009A8B4(func_8009A8B4_arg0 *arg) {
    BattleRenderCtx *ctx = D_800D3C58;
    DR_MODE *mode = &ctx->drawModes[D_800ABB34++];
    SPRT *sprt = &ctx->sprt[D_800D3C54++];

    SetDrawMode(mode, 0, 0, arg->tpage, NULL);
    sprt->clut = arg->clut;
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->w = arg->w;
    sprt->h = arg->h;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

static void func_8009AA18(func_8009A8B4_arg0 *arg) {
    BattleRenderCtx *ctx = D_800D3C58;
    DR_MODE *mode = &ctx->drawModes[D_800ABB34++];
    DR_MODE *restore = &ctx->drawModes[D_800ABB34++];
    SPRT *sprt = &ctx->sprt[D_800D3C54++];
    RECT window;
    RECT full;

    window.x = window.y = 0;
    window.w = window.h = 255;
    full.x = full.y = 0;
    full.w = full.h = 255;
    SetDrawMode(mode, 0, 0, arg->tpage, &window);
    SetDrawMode(restore, 0, 0, arg->tpage, &full);
    sprt->clut = arg->clut;
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->w = arg->w;
    sprt->h = arg->h;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, restore);
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

static void func_8009AC18(func_8009A8B4_arg0 *arg) {
    BattleRenderCtx *ctx = D_800D3C58;
    DR_MODE *mode = &ctx->drawModes[D_800ABB34++];
    DR_MODE *restore = &ctx->drawModes[D_800ABB34++];
    SPRT *sprt = &ctx->sprt[D_800D3C54++];
    RECT window;
    RECT full;

    window.x = window.y = 0;
    window.w = window.h = 32;
    full.x = full.y = 0;
    full.w = full.h = 255;
    SetDrawMode(mode, 0, 0, arg->tpage, &window);
    SetDrawMode(restore, 0, 0, arg->tpage, &full);
    sprt->clut = arg->clut;
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->w = arg->w;
    sprt->h = arg->h;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, restore);
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

static void func_8009AE1C(func_8009AE1C_arg0 *arg) {
    BattleRenderCtx *ctx = D_800D3C58;
    DR_MODE *mode = &ctx->drawModes[D_800ABB34++];
    SPRT_8 *sprt = &ctx->sprt8[D_800D3C68++];

    SetDrawMode(mode, 0, 0, getTPage(0, 0, 768, 256), NULL);
    sprt->clut = GetClut(16, arg->color + 0xE0);
    sprt->u0 = arg->u;
    sprt->v0 = arg->v;
    sprt->x0 = arg->x;
    sprt->y0 = arg->y;
    addPrim(arg->ot, sprt);
    addPrim(arg->ot, mode);
}

void func_8009AF64(CmdBlk *cmd, s32 arg1, s32 arg2, s32 arg3) {
    BattleRenderCtx *ctx = D_800D3C58;
    DR_MODE *mode = &ctx->drawModes[D_800ABB34++];
    SPRT_8 *sprt = &ctx->sprt8[D_800D3C68];
    u8 *text = (u8 *)getScratchAddr(0x40);
    s32 col;
    u32 c;

    SetDrawMode(mode, 0, 0, getTPage(0, 0, 768, 256), NULL);
    sprintf((char *)text, cmd->format, arg1, arg2, arg3);
    for (col = 0; text[col] != '\0'; col++) {
        c = text[col];
        if (c != ' ') {
            sprt->x0 = (cmd->x + col) * 8;
            sprt->y0 = cmd->y * 8;
            sprt->u0 = (c & 0xF) * 8;
            sprt->v0 = (c >> 4) * 8;
            sprt->clut = GetClut(16, cmd->color + 0xE0);
            addPrim(cmd->ot, sprt);
            D_800D3C68++;
            sprt++;
        }
    }
    addPrim(cmd->ot, mode);
}

static void func_8009B170(CmdBlk *cmd, s32 arg1, s32 arg2, s32 arg3) {
    BattleRenderCtx *ctx = D_800D3C58;
    DR_MODE *mode = &ctx->drawModes[D_800ABB34++];
    SPRT_16 *sprt = &ctx->sprt16[D_800D3C6C];
    s32 col = 0;
    s32 px;
    u8 c;

    SetDrawMode(mode, 0, 0, getTPage(0, 0, 832, 256), NULL);
    sprintf(((func_8009B170_scratch *)getScratchAddr(0x40))->text, cmd->format, arg1, arg2, arg3);
    px = 0;
    while ((c = ((func_8009B170_scratch *)getScratchAddr(0x40))->text[col]) != '\0') {
        if (c != ' ') {
            sprt->clut = GetClut(0, cmd->color + 0xE0);
            setXY0(sprt, cmd->x * 8 + px, cmd->y * 8);
            setUV0(sprt, (c % 16) * 16, c / 16 * 16);
            addPrim(cmd->ot, sprt);
            D_800D3C6C++;
            sprt++;
        }
        col++;
        px += D_8009B534[c];
    }
    addPrim(cmd->ot, mode);
}

INCLUDE_ASM("asm/ovl/battle_render/nonmatchings/battle_render", func_8009B394);
