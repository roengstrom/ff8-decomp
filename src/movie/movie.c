#include "common.h"
#include "movie.h"
#include "cdrom.h"
#include "field.h"
#include "field/fe_object1.h"
#include "field/fe_object5.h"
#include "lzs.h"
#include "main.h"
#include "psxsdk/libcd.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libpress.h"
#include "snd_init.h"
#include "sound.h"

#define MOVIE_UNK13_02 0x2

#define MOVIE_FRAME_01 0x01
#define MOVIE_FRAME_02 0x02
#define MOVIE_FRAME_04 0x04
#define MOVIE_FRAME_08 0x08
#define MOVIE_FRAME_10 0x10
#define MOVIE_FRAME_20 0x20
#define MOVIE_FRAME_40 0x40

#define DMA1_CHCR (*(u32 *)0x1F801098)
#define DMA_CHCR_BUSY 0x1000000

typedef struct {
    /** 0x00 */ s32 unk0;
    /** 0x04 */ s32 unk4;
    /** 0x08 */ s32 unk8;
    /** 0x0C */ s32 unkC;
    /** 0x10 */ s32 unk10;
    /** 0x14 */ u32 unk14;
    /** 0x18 */ CdlCB unk18;
    /** 0x1C */ CdlCB unk1C;
} MovieUnkState;

typedef struct {
    /** 0x00 */ s32 unk0;
    /** 0x04 */ u32 unk4;
    /** 0x08 */ u32 unk8;
    /** 0x0C */ u16 unkC;
    /** 0x0E */ u16 unkE;
    /** 0x10 */ u8 unk10;
    /** 0x11 */ u8 unk11;
    /** 0x12 */ u8 unk12;
    /** 0x13 */ u8 unk13;
    /** 0x14 */ u8 unk14;
    /** 0x15 */ u8 unk15;
    /** 0x16 */ u8 volatile unk16;
    /** 0x17 */ u8 volatile unk17;
    /** 0x18 */ u8 volatile unk18;
} MovieState;

typedef struct {
    /** 0x00 */ s16 unk0;
    /** 0x02 */ u8 unk2;
    /** 0x03 */ u8 unk3;
    /** 0x04 */ u16 unk4;
    /** 0x06 */ u16 unk6;
    /** 0x08 */ u32 *unk8[2];
    /** 0x10 */ u32 *unk10;
    /** 0x14 */ RECT unk14;
    /** 0x1C */ RECT unk1C;
} MovieUnkContext;

typedef struct {
    /** 0x0 */ u8 pad0[2];
    /** 0x2 */ u8 unk2;
    /** 0x3 */ u8 unk3;
    /** 0x4 */ u8 unk4;
    /** 0x5 */ u8 unk5;
    /** 0x6 */ u16 unk6;
} MovieUnkTag;

typedef struct {
    /** 0x0 */ u8 unk0;
    /** 0x1 */ u8 unk1;
    /** 0x2 */ u16 unk2;
    /** 0x4 */ u16 unk4;
    /** 0x6 */ u16 unk6;
    /** 0x8 */ s32 unk8;
} MovieEntry;

extern DECDCTTAB D_801E93B4;
extern MovieState D_801FA3B4;
extern MovieUnkContext D_801FA3D4;
extern MovieUnkState D_801FA404;
extern MovieUnkTag D_801FA424;
extern MovieEntry D_801FA434[];
extern FieldView D_801FA4C4;
extern s32 D_801FA4EC;
extern s32 D_801FA4F0;

static u32 *func_801E8370(void);
static void func_801E8304(void);
static void func_801E8658(void);
static void func_801E8BAC(void);
static void func_801E8C88(void);
static void func_801E8CC8(u8 intr, u8 *result);
static void func_801E8FD0(u8 intr, u8 *result);
static s32 func_801E91C0(void);
static void func_801E92AC(void);

void func_801E8000(s32 priority) {
    DecDCTReset(1);
    D_801FA3B4.unk10 = priority;
    D_801FA404.unk0 = 0;
    D_801FA404.unk4 = 0;
    D_801FA404.unkC = 0;
    D_801FA404.unk10 = -1;
    D_801FA404.unk14 = 0;
    D_801FA404.unk18 = NULL;
    D_801FA404.unk1C = NULL;
    D_801FA4F0 = 0;
    sndCmdE4(0x7F);
    D_801FA3B4.unk14 = 0;
    D_801FA3B4.unk15 = 0;
}

// Not static: the binary keeps this out-of-line copy as well as the copy inlined into func_801E8104.
inline s32 func_801E8080(s32 addr) {
    s32 i;

    if (!(D_801FA3B4.unk13 & MOVIE_UNK13_02)) {
        for (i = 0; i < D_801FA3B4.unk10; i++) {
            D_801FA434[i].unk8 = addr;
            addr += 0x4000;
            D_801FA434[i].unk0 = 0;
            D_801FA434[i].unk1 = 0;
            D_801FA434[i].unk2 = 0;
            D_801FA434[i].unk4 = 0;
            D_801FA434[i].unk6 = 0;
        }
    }
    D_801FA4EC = addr;
    return addr + (D_801FA3B4.unk10 << 12);
}

s32 func_801E8104(s32 lba, s32 size, s32 flags, s32 buf) {
    D_8007064F[0] = 1;
    D_801FA3B4.unk16 = 0;
    D_801FA3B4.unk17 = 0;
    D_801FA3B4.unk13 = flags;
    D_801FA3B4.unk4 = lba;
    D_801FA3B4.unk8 = lba + (size + 0x7FF) / 0x800;
    D_801FA3B4.unk11 = 0;
    D_801FA3B4.unk12 = 0;
    D_801FA3B4.unkC = 0;
    D_801FA3B4.unkE = 0;
    D_801FA3B4.unk0 = (size + 0x7FF) / 0x5000;
    buf = func_801E8080(buf);
    if (!(D_801FA3B4.unk13 & MOVIE_UNK13_02)) {
        D_801FA3D4.unk8[0] = (u32 *)buf;
        buf += 0xFA00;
        D_801FA3D4.unk8[1] = (u32 *)buf;
        buf += 0xFA00;
        D_801FA3D4.unk0 = 0;
        D_801FA3D4.unk10 = (u32 *)buf;
        D_801FA3D4.unk4 = 320;
        D_801FA3D4.unk6 = 224;
        D_801FA3D4.unk14.x = 0;
        D_801FA3D4.unk14.y = 32;
        D_801FA3D4.unk14.w = 16;
        D_801FA3D4.unk14.h = 224;
        D_801FA3D4.unk1C.x = 0;
        D_801FA3D4.unk1C.y = 32;
        D_801FA3D4.unk1C.w = 20;
        D_801FA3D4.unk1C.h = 224;
        D_801FA3D4.unk2 = 0;
        D_801FA3D4.unk3 = 0;
        DecDCTvlcBuild(D_801E93B4);
        DecDCToutCallback(func_801E92AC);
    }
    sndInitIrq(D_801FA4EC, D_801FA3B4.unk10 << 12);
    func_800387F8(D_801FA3B4.unk4, func_801E8BAC);
    return buf;
}

s32 func_801E82CC(void) {
    return D_801FA434[D_801FA3B4.unk10 >> 1].unk4 == 2;
}

static void func_801E8304(void) {
    MovieUnkState *state = &D_801FA404;

    if (state->unkC == 0 && CdSync(1, NULL) == CdlComplete && state->unk14 < D_801FA3B4.unk8) {
        func_801E91C0();
    }
}

static u32 *func_801E8370(void) {
    MovieUnkState *state = &D_801FA404;
    s32 value;
    s32 last;

    if (state->unkC == 0 && CdSync(1, NULL) == CdlComplete && state->unk14 < D_801FA3B4.unk8) {
        func_801E91C0();
    }
    func_801E8304();
    if (D_801FA434[D_801FA3B4.unk11].unk4 == 2) {
        if (D_800772B8 == D_801FA3B4.unk0 - 5) {
            sndCmdE5(0x14, 0);
        }
        value = ((u8 *)D_801FA4EC)[D_801FA3B4.unk11 * 0x1000 + 8];
        if (value & MOVIE_FRAME_02) {
            sndCmdE5(0x3C, 0x7F);
        }
        if (value & MOVIE_FRAME_04) {
            sndCmdE5(0x3C, 0);
        }
        if (value & MOVIE_FRAME_08) {
            D_8007064F[0] = 0;
        }
        if (value & MOVIE_FRAME_10) {
            D_8007064F[0] = 1;
        }
        if (value & MOVIE_FRAME_20) {
            D_80070649 = 1;
        } else {
            D_80070649 = 0;
        }
        D_80070655 = value & MOVIE_FRAME_40;
        if (D_801FA4F0 != 0) {
            if (D_801FA3B4.unkE - D_801FA3B4.unkC < (D_801FA3B4.unk10 >> 1)) {
                return NULL;
            }
            sndDisableReverb(3);
            D_801FA4F0 = 0;
        }
        sndTickCounters();
        g_fieldEntity.unk1AC = 4;
        if (D_800772B8 > D_801FA3B4.unkC + 1) {
            last = D_800772B8 - 1;
            value = last - D_801FA3B4.unkC;
            if (value >= 2) {
                func_801E8658();
                return NULL;
            }
            g_fieldEntity.unk1AC = 3;
        }
        D_801FA434[D_801FA3B4.unk11].unk4 = 3;
        D_801FA434[D_801FA3B4.unk11].unk2 = ((u8 *)D_801FA4EC)[D_801FA3B4.unk11 * 0x1000 + 8] & MOVIE_FRAME_01;
        // Indexing through a u8 pointer rather than adding to the s32 gives the original operand order.
        func_80039678(&D_801FA4C4, &((u8 *)D_801FA4EC)[D_801FA3B4.unk11 * 0x1000 + 0x10], sizeof(FieldView));
        g_movieView = &D_801FA4C4;
        return (u32 *)D_801FA434[D_801FA3B4.unk11].unk8;
    }
    sndEnableReverb(3);
    D_801FA4F0 = 1;
    if (CdStatus() & CdlStatShellOpen) {
        D_801FA404.unkC = 0;
        CdControlF(CdlNop, NULL);
    }
    return NULL;
}

static void func_801E8658(void) {
    D_801FA434[D_801FA3B4.unk11].unk0 = 0;
    D_801FA434[D_801FA3B4.unk11].unk1 = 0;
    D_801FA434[D_801FA3B4.unk11].unk2 = 0;
    D_801FA434[D_801FA3B4.unk11].unk4 = 0;
    D_801FA3B4.unkC++;
    if (++D_801FA3B4.unk11 >= D_801FA3B4.unk10) {
        D_801FA3B4.unk11 = 0;
    }
}

void func_801E870C(void) {
    u32 *bs;

    if (!(D_801FA3B4.unk13 & MOVIE_UNK13_02)) {
        sndCmdE4(((u8 *)D_801FA4EC)[D_801FA3B4.unk11 * 0x1000 + 9]);
        DecDCTvlcSize2(0);
        sndTickCounters();
        sndTickCounters();
        while ((bs = func_801E8370()) == NULL) {
        }
        D_801FA3D4.unk3 = D_801FA434[D_801FA3B4.unk11].unk2;
        ((DISPENV *)D_8005F110)->isrgb24 = D_801FA3D4.unk3;
        DecDCTvlc2(bs, D_801FA3D4.unk8[D_801FA3D4.unk0], D_801E93B4);
        func_801E8658();
        D_801FA3B4.unk16 = 1;
    }
    setCdBusyFlag();
}

void func_801E8804(void) {
    func_801E8C88();
    clearCdBusyFlag();
    sndCmdE2();
    if (!(D_801FA3B4.unk13 & MOVIE_UNK13_02)) {
        ((DISPENV *)D_8005F138)->isrgb24 = 0;
        ((DISPENV *)D_8005F110)->isrgb24 = 0;
        func_800B14C8();
        g_fieldEntity.unk1A7 = 1;
        g_fieldEntity.unk1AD = 0;
    }
    D_801FA3B4.unk16 = 0;
    D_801FA3B4.unk17 = 0;
}

void func_801E888C(void) {
    u32 *bs;

    if (!(D_801FA3B4.unk13 & MOVIE_UNK13_02)) {
        if (D_801FA3B4.unk16 == 0) {
            return;
        }
        D_801FA3B4.unk18 = 0;
        D_801FA3D4.unk14.x = g_activeDrawEnv->clip.x;
        D_801FA3D4.unk1C.x = g_activeDrawEnv->clip.x;
        D_801FA3D4.unk14.y = g_activeDrawEnv->clip.y;
        D_801FA3D4.unk1C.y = g_activeDrawEnv->clip.y;
        D_801FA3D4.unk2 = D_801FA3D4.unk3;
        if (D_801FA3D4.unk3 != 0) {
            D_801FA3D4.unk14.w = 24;
            D_801FA3D4.unk1C.w = 30;
        } else {
            D_801FA3D4.unk14.w = 16;
            D_801FA3D4.unk1C.w = 20;
        }
        DecDCTin(D_801FA3D4.unk8[D_801FA3D4.unk0], D_801FA3D4.unk2);
        DecDCTout(D_801FA3D4.unk10, D_801FA3D4.unk14.w * D_801FA3D4.unk14.h / 2);
        while ((bs = func_801E8370()) == NULL) {
            if (D_801FA404.unk14 >= D_801FA3B4.unk8) {
                func_801E8804();
                // A break here adds a second test of bs after the loop, which the original does not have.
                goto wait;
            }
        }
        D_801FA3D4.unk3 = D_801FA434[D_801FA3B4.unk11].unk2;
        ((DISPENV *)D_8005F138)->isrgb24 = D_801FA3D4.unk3;
        D_801FA3D4.unk0 = D_801FA3D4.unk0 == 0;
        DecDCTvlc2(bs, D_801FA3D4.unk8[D_801FA3D4.unk0], D_801E93B4);
        func_801E8658();
    wait:
        while (D_801FA3B4.unk18 == 0) {
            func_801E8304();
        }
        return;
    }
    if (D_801FA3B4.unk17 == 0) {
        return;
    }
    if (D_801FA4F0 != 0) {
        if (D_801FA404.unkC != 0 && g_sndStream.tickCounter - g_sndStream.unk20 >= (D_801FA3B4.unk10 >> 1)) {
            sndDisableReverb(3);
            D_801FA4F0 = 0;
        }
        D_801FA404.unkC = 0;
    } else if (D_801FA3B4.unk12 == D_800772CC) {
        D_801FA404.unkC = 0;
        sndEnableReverb(3);
        D_801FA4F0 = 1;
    }
    if (D_801FA404.unkC == 0) {
        func_801E91C0();
    }
    if (D_801FA404.unk14 >= D_801FA3B4.unk8) {
        func_801E8804();
    }
}

s32 func_801E8B58(void) {
    return D_801FA434[D_801FA3B4.unk11].unk6;
}

s32 func_801E8B84(void) {
    return D_801FA3B4.unk16;
}

s32 func_801E8B98(void) {
    return D_801FA3B4.unk17;
}

static void func_801E8BAC(void) {
    u8 param[4];
    u8 mode;

    D_801FA404.unk4 = CdlModeSpeed | CdlModeSize1;
    D_801FA404.unk0 = D_801FA434[D_801FA3B4.unk12].unk8;
    D_801FA404.unkC = 0;
    D_801FA404.unk14 = D_801FA3B4.unk4;
    D_801FA404.unk18 = CdSyncCallback(NULL);
    D_801FA404.unk1C = CdReadyCallback(NULL);
    if (CdStatus() & (CdlStatPlay | CdlStatSeek)) {
        CdControlB(CdlPause, NULL, NULL);
    }
    while (func_801E91C0() == 0) {
    }
    mode = D_801FA404.unk4;
    param[0] = mode;
    if (mode != CdMode()) {
        while (CdControl(CdlSetmode, param, NULL) == 0) {
        }
    }
}

static void func_801E8C88(void) {
    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    CdFlush();
    CdControl(CdlPause, NULL, NULL);
}

static void func_801E8CC8(u8 intr, u8 *result) {
    u32 header[3];

    if (D_801FA3D4.unk2 != 0 && (DMA1_CHCR & DMA_CHCR_BUSY)) {
        D_801FA3B4.unk14 = 1;
        return;
    }
    intr = CdReady(1, result);
    if (intr == CdlDataReady && D_801FA404.unkC != 0) {
        CdGetSector(header, 3);
        if (CdPosToInt((CdlLOC *)header) != D_801FA404.unk14) {
            D_801FA404.unkC = 0;
        }
        if (D_801FA404.unkC != 0) {
            CdGetSector(&D_801FA424, 2);
            switch (D_801FA424.unk2) {
            case 'J':
                CdGetSector((void *)D_801FA404.unk0, 0x1FE);
                D_801FA404.unk0 += 0x7F8;
                break;
            case 'N':
                CdGetSector(&((u8 *)D_801FA4EC)[D_801FA3B4.unk12 * 0x1000 + 8], 0x1FE);
                break;
            case 'R':
                CdGetSector(&((u8 *)D_801FA4EC)[D_801FA3B4.unk12 * 0x1000 + 0x808], 0x1FE);
                break;
            }
            D_801FA404.unk14++;
            D_801FA434[D_801FA3B4.unk12].unk0 = D_801FA424.unk4;
            D_801FA434[D_801FA3B4.unk12].unk1 = D_801FA424.unk5;
            D_801FA434[D_801FA3B4.unk12].unk6 = D_801FA424.unk6;
            if (D_801FA434[D_801FA3B4.unk12].unk0 == D_801FA434[D_801FA3B4.unk12].unk1) {
                D_801FA434[D_801FA3B4.unk12].unk4 = 2;
                D_801FA3B4.unkE++;
                if (++D_801FA3B4.unk12 >= D_801FA3B4.unk10) {
                    D_801FA3B4.unk12 = 0;
                }
                D_801FA404.unk0 = D_801FA434[D_801FA3B4.unk12].unk8;
                if (D_801FA434[D_801FA3B4.unk12].unk4 != 0 || D_801FA3B4.unk12 == D_800772CC) {
                    func_801E8C88();
                    D_801FA404.unkC = 0;
                    return;
                }
            }
        }
    } else {
        D_801FA404.unkC = 0;
    }
    if (D_801FA404.unkC == 0) {
        func_801E91C0();
    }
    if (D_801FA404.unk14 >= D_801FA3B4.unk8) {
        func_801E8C88();
    }
}

static void func_801E8FD0(u8 intr, u8 *result) {
    u32 header[3];
    s32 last;
    s32 mask;

    // A no-op on a u8; it puts the zero-extension of intr at the top of the function, where the original has it.
    mask = 0xFF;
    intr &= mask;
    if (intr == CdlDataReady && D_801FA404.unkC != 0) {
        CdGetSector(header, 3);
        if (CdPosToInt((CdlLOC *)header) != D_801FA404.unk14) {
            D_801FA404.unkC = 0;
        }
        if (D_801FA404.unkC != 0) {
            D_801FA3B4.unk17 = intr;
            CdGetSector(&D_801FA424, 2);
            switch (D_801FA424.unk2) {
            case 'N':
                last = D_800772CC - 1;
                if (last < 0) {
                    last = D_801FA3B4.unk10 - 1;
                }
                if (last != D_801FA3B4.unk12) {
                    CdGetSector(&((u8 *)D_801FA4EC)[D_801FA3B4.unk12 * 0x1000 + 8], 0x1FE);
                    D_801FA404.unk14++;
                } else {
                    D_801FA404.unkC = 0;
                }
                break;
            case 'R':
                CdGetSector(&((u8 *)D_801FA4EC)[D_801FA3B4.unk12 * 0x1000 + 0x808], 0x1FE);
                D_801FA404.unk14++;
                sndTickCounters();
                if (++D_801FA3B4.unk12 >= D_801FA3B4.unk10) {
                    D_801FA3B4.unk12 = 0;
                }
                D_801FA3B4.unkE++;
                break;
            }
        }
    } else {
        D_801FA404.unkC = 0;
    }
    if (D_801FA404.unkC == 0) {
        func_801E91C0();
    }
    if (D_801FA404.unk14 >= D_801FA3B4.unk8) {
        func_801E8C88();
    }
}

static s32 func_801E91C0(void) {
    u8 param[4];
    CdlLOC loc;
    u8 mode;

    D_801FA404.unkC = 0;
    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    mode = D_801FA404.unk4;
    param[0] = mode;
    if (mode != CdMode()) {
        if (CdControl(CdlSetmode, param, NULL) == 0) {
            return 0;
        }
    }
    CdFlush();
    CdIntToPos(D_801FA404.unk14, &loc);
    if (CdControl(CdlSetloc, (u8 *)&loc, NULL) == 0) {
        return 0;
    }
    if (D_801FA3B4.unk13 & MOVIE_UNK13_02) {
        CdReadyCallback(func_801E8FD0);
    } else {
        CdReadyCallback(func_801E8CC8);
    }
    CdControlF(CdlReadN, NULL);
    D_801FA404.unkC = 1;
    return 1;
}

static void func_801E92AC(void) {
    s32 limit;

    if (D_801FA3B4.unk14 != 0) {
        func_801E8CC8(0xFF, NULL);
        D_801FA3B4.unk14 = 0;
    }
    LoadImage(&D_801FA3D4.unk14, D_801FA3D4.unk10);
    D_801FA3D4.unk14.x += D_801FA3D4.unk14.w;
    if (D_801FA3D4.unk2 == 0) {
        limit = g_activeDrawEnv->clip.x + D_801FA3D4.unk4;
    } else {
        limit = g_activeDrawEnv->clip.x + ((D_801FA3D4.unk4 * 3) >> 1);
    }
    if (D_801FA3D4.unk14.x < limit) {
        DecDCTout(D_801FA3D4.unk10, D_801FA3D4.unk14.w * D_801FA3D4.unk14.h / 2);
    } else {
        D_801FA3B4.unk18 = 1;
    }
}
