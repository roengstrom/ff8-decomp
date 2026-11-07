#include "common.h"
#include "movie.h"
#include "cdrom.h"
#include "field.h"
#include "field/fe_object1.h"
#include "field/fe_object5.h"
#include "gamestate.h"
#include "lzs.h"
#include "main.h"
#include "psxsdk/libcd.h"
#include "psxsdk/libgpu.h"
#include "psxsdk/libpress.h"
#include "snd_init.h"
#include "sound.h"

#define MOVIE_FRAME_24BIT 0x01
#define MOVIE_FRAME_FADE_IN 0x02
#define MOVIE_FRAME_FADE_OUT 0x04
#define MOVIE_FRAME_MASK_ON 0x08
#define MOVIE_FRAME_MASK_OFF 0x10
#define MOVIE_FRAME_HIDE_FIELD 0x20
#define MOVIE_FRAME_HIDE_ACTORS 0x40

#define SECTOR_SIZE 0x800
// One frame of a movie takes ten sectors on the disc.
#define MOVIE_FRAME_SIZE (10 * SECTOR_SIZE)
// The data a movie sector carries after its headers, in words.
#define MOVIE_SECTOR_DATA_WORDS 0x1FE
#define MOVIE_SLOT_BUFFER_SIZE 0x4000
#define MOVIE_DECODE_BUFFER_SIZE 0xFA00

/**
 * Each ring slot has a block in g_movieSlotBlocks that receives the frame's 'N' and 'R' sectors.
 * The code indexes it as bytes: a struct for the block reorders the address arithmetic and breaks the match.
 */
#define MOVIE_SLOT_INFO_SIZE 0x1000
#define MOVIE_SLOT_N_SECTOR 0x8
#define MOVIE_SLOT_FLAGS 0x8
#define MOVIE_SLOT_VOLUME 0x9
#define MOVIE_SLOT_VIEW 0x10
#define MOVIE_SLOT_R_SECTOR 0x808

typedef enum {
    MOVIE_SLOT_EMPTY = 0,
    MOVIE_SLOT_FULL = 2,
    MOVIE_SLOT_DECODING = 3
} MovieSlotState;

#define DMA1_CHCR (*(u32 *)0x1F801098)
#define DMA_CHCR_BUSY 0x1000000

typedef struct {
    /** 0x00 */ CdlLOC loc;
    /** 0x04 */ u8 unk4[8];
} MovieSectorHeader;

typedef struct {
    /** 0x00 */ u8 *dest;
    /** 0x04 */ s32 cdMode;
    /** 0x08 */ s32 unk8;
    /** 0x0C */ s32 reading;
    /** 0x10 */ s32 unk10;
    /** 0x14 */ u32 nextSector;
    /** 0x18 */ CdlCB savedSyncCallback;
    /** 0x1C */ CdlCB savedReadyCallback;
} MovieReader;

typedef struct {
    /** 0x00 */ s32 frameCount;
    /** 0x04 */ u32 firstSector;
    /** 0x08 */ u32 endSector;
    /** 0x0C */ u16 framesDecoded;
    /** 0x0E */ u16 framesRead;
    /** 0x10 */ u8 bufferSize;
    /** 0x11 */ u8 decodeSlot;
    /** 0x12 */ u8 readSlot;
    /** 0x13 */ u8 flags;
    /** 0x14 */ u8 readDeferred;
    /** 0x15 */ u8 unk15;
    /** 0x16 */ u8 volatile playing;
    /** 0x17 */ u8 volatile audioStreaming;
    /** 0x18 */ u8 volatile pictureDone;
} MovieState;

typedef struct {
    /** 0x00 */ s16 bufferIndex;
    /** 0x02 */ u8 is24Bit;
    /** 0x03 */ u8 next24Bit;
    /** 0x04 */ u16 width;
    /** 0x06 */ u16 height;
    /** 0x08 */ u32 *mdecInput[2];
    /** 0x10 */ u32 *stripBuffer;
    /** 0x14 */ RECT stripRect;
    /** 0x1C */ RECT unk1C;
} MovieDecoder;

typedef struct {
    /** 0x0 */ u8 magic[2];
    /** 0x2 */ u8 type;
    /** 0x3 */ u8 unk3;
    /** 0x4 */ u8 index;
    /** 0x5 */ u8 lastIndex;
    /** 0x6 */ u16 frame;
} MovieSectorTag;

typedef struct {
    /** 0x0 */ u8 lastSector;
    /** 0x1 */ u8 lastIndex;
    /** 0x2 */ u16 is24Bit;
    /** 0x4 */ u16 state;
    /** 0x6 */ u16 frame;
    /** 0x8 */ u8 *video;
} MovieSlot;

extern DECDCTTAB g_movieVlcTable;
extern MovieState g_movieState;
extern MovieDecoder g_movieDecoder;
extern MovieReader g_movieReader;
extern MovieSectorTag g_movieSectorTag;
extern MovieSlot g_movieSlots[];
extern FieldView g_movieFrameView;
extern u8 *g_movieSlotBlocks;
extern s32 g_movieStalled;

static u32 *MovieNextFrame(void);
static void MovieResumeReading(void);
static void MovieReleaseFrame(void);
static void MovieInitReader(void);
static void MovieStopReading(void);
static void MovieReadCallback(u8 intr, u8 *result);
static void MovieAudioReadCallback(u8 intr, u8 *result);
static s32 MovieStartReading(void);
static void MovieDecodeCallback(void);

void MovieInit(s32 bufferSize) {
    DecDCTReset(1);
    g_movieState.bufferSize = bufferSize;
    g_movieReader.dest = NULL;
    g_movieReader.cdMode = 0;
    g_movieReader.reading = 0;
    g_movieReader.unk10 = -1;
    g_movieReader.nextSector = 0;
    g_movieReader.savedSyncCallback = NULL;
    g_movieReader.savedReadyCallback = NULL;
    g_movieStalled = 0;
    sndCmdE4(0x7F);
    g_movieState.readDeferred = 0;
    g_movieState.unk15 = 0;
}

// Not static: the binary keeps this out-of-line copy as well as the copy inlined into MovieOpen.
inline u8 *MovieInitSlots(u8 *addr) {
    s32 i;

    if (!(g_movieState.flags & MOVIE_AUDIO_ONLY)) {
        for (i = 0; i < g_movieState.bufferSize; i++) {
            g_movieSlots[i].video = addr;
            addr += MOVIE_SLOT_BUFFER_SIZE;
            g_movieSlots[i].lastSector = 0;
            g_movieSlots[i].lastIndex = 0;
            g_movieSlots[i].is24Bit = 0;
            g_movieSlots[i].state = MOVIE_SLOT_EMPTY;
            g_movieSlots[i].frame = 0;
        }
    }
    g_movieSlotBlocks = addr;
    return addr + g_movieState.bufferSize * MOVIE_SLOT_INFO_SIZE;
}

u8 *MovieOpen(s32 lba, s32 size, s32 flags, u8 *buf) {
    g_fieldContext.movieMaskOff = 1;
    g_movieState.playing = 0;
    g_movieState.audioStreaming = 0;
    g_movieState.flags = flags;
    g_movieState.firstSector = lba;
    g_movieState.endSector = lba + (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    g_movieState.decodeSlot = 0;
    g_movieState.readSlot = 0;
    g_movieState.framesDecoded = 0;
    g_movieState.framesRead = 0;
    g_movieState.frameCount = (size + SECTOR_SIZE - 1) / MOVIE_FRAME_SIZE;
    buf = MovieInitSlots(buf);
    if (!(g_movieState.flags & MOVIE_AUDIO_ONLY)) {
        g_movieDecoder.mdecInput[0] = (u32 *)buf;
        buf += MOVIE_DECODE_BUFFER_SIZE;
        g_movieDecoder.mdecInput[1] = (u32 *)buf;
        buf += MOVIE_DECODE_BUFFER_SIZE;
        g_movieDecoder.bufferIndex = 0;
        g_movieDecoder.stripBuffer = (u32 *)buf;
        g_movieDecoder.width = 320;
        g_movieDecoder.height = 224;
        g_movieDecoder.stripRect.x = 0;
        g_movieDecoder.stripRect.y = 32;
        g_movieDecoder.stripRect.w = 16;
        g_movieDecoder.stripRect.h = 224;
        g_movieDecoder.unk1C.x = 0;
        g_movieDecoder.unk1C.y = 32;
        g_movieDecoder.unk1C.w = 20;
        g_movieDecoder.unk1C.h = 224;
        g_movieDecoder.is24Bit = 0;
        g_movieDecoder.next24Bit = 0;
        DecDCTvlcBuild(g_movieVlcTable);
        DecDCToutCallback(MovieDecodeCallback);
    }
    sndInitIrq(g_movieSlotBlocks, g_movieState.bufferSize * MOVIE_SLOT_INFO_SIZE);
    func_800387F8(g_movieState.firstSector, MovieInitReader);
    return buf;
}

s32 MovieIsBuffered(void) {
    return g_movieSlots[g_movieState.bufferSize >> 1].state == MOVIE_SLOT_FULL;
}

static void MovieResumeReading(void) {
    MovieReader *state = &g_movieReader;

    if (state->reading == 0 && CdSync(1, NULL) == CdlComplete && state->nextSector < g_movieState.endSector) {
        MovieStartReading();
    }
}

static u32 *MovieNextFrame(void) {
    MovieReader *state = &g_movieReader;
    s32 value;
    s32 last;

    if (state->reading == 0 && CdSync(1, NULL) == CdlComplete && state->nextSector < g_movieState.endSector) {
        MovieStartReading();
    }
    MovieResumeReading();
    if (g_movieSlots[g_movieState.decodeSlot].state == MOVIE_SLOT_FULL) {
        if (g_sndStream.unk20 == g_movieState.frameCount - 5) {
            sndCmdE5(0x14, 0);
        }
        value = g_movieSlotBlocks[g_movieState.decodeSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_FLAGS];
        if (value & MOVIE_FRAME_FADE_IN) {
            sndCmdE5(0x3C, 0x7F);
        }
        if (value & MOVIE_FRAME_FADE_OUT) {
            sndCmdE5(0x3C, 0);
        }
        if (value & MOVIE_FRAME_MASK_ON) {
            g_fieldContext.movieMaskOff = 0;
        }
        if (value & MOVIE_FRAME_MASK_OFF) {
            g_fieldContext.movieMaskOff = 1;
        }
        if (value & MOVIE_FRAME_HIDE_FIELD) {
            g_fieldContext.skipFieldDraw = 1;
        } else {
            g_fieldContext.skipFieldDraw = 0;
        }
        g_fieldContext.hideActors = value & MOVIE_FRAME_HIDE_ACTORS;
        if (g_movieStalled != 0) {
            if (g_movieState.framesRead - g_movieState.framesDecoded < (g_movieState.bufferSize >> 1)) {
                return NULL;
            }
            sndDisableReverb(3);
            g_movieStalled = 0;
        }
        sndTickCounters();
        g_fieldContext.movieFrameVBlanks = 4;
        if (g_sndStream.unk20 > g_movieState.framesDecoded + 1) {
            last = g_sndStream.unk20 - 1;
            value = last - g_movieState.framesDecoded;
            if (value >= 2) {
                MovieReleaseFrame();
                return NULL;
            }
            g_fieldContext.movieFrameVBlanks = 3;
        }
        g_movieSlots[g_movieState.decodeSlot].state = MOVIE_SLOT_DECODING;
        g_movieSlots[g_movieState.decodeSlot].is24Bit = g_movieSlotBlocks[g_movieState.decodeSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_FLAGS] & MOVIE_FRAME_24BIT;
        FastMemCpy((u8 *)&g_movieFrameView, &g_movieSlotBlocks[g_movieState.decodeSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_VIEW], sizeof(FieldView));
        g_movieView = &g_movieFrameView;
        return (u32 *)g_movieSlots[g_movieState.decodeSlot].video;
    }
    sndEnableReverb(3);
    g_movieStalled = 1;
    if (CdStatus() & CdlStatShellOpen) {
        g_movieReader.reading = 0;
        CdControlF(CdlNop, NULL);
    }
    return NULL;
}

static void MovieReleaseFrame(void) {
    g_movieSlots[g_movieState.decodeSlot].lastSector = 0;
    g_movieSlots[g_movieState.decodeSlot].lastIndex = 0;
    g_movieSlots[g_movieState.decodeSlot].is24Bit = 0;
    g_movieSlots[g_movieState.decodeSlot].state = MOVIE_SLOT_EMPTY;
    g_movieState.framesDecoded++;
    if (++g_movieState.decodeSlot >= g_movieState.bufferSize) {
        g_movieState.decodeSlot = 0;
    }
}

void MoviePlay(void) {
    u32 *bs;

    if (!(g_movieState.flags & MOVIE_AUDIO_ONLY)) {
        sndCmdE4(g_movieSlotBlocks[g_movieState.decodeSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_VOLUME]);
        DecDCTvlcSize2(0);
        sndTickCounters();
        sndTickCounters();
        while ((bs = MovieNextFrame()) == NULL) {
        }
        g_movieDecoder.next24Bit = g_movieSlots[g_movieState.decodeSlot].is24Bit;
        g_prevDispEnv->isrgb24 = g_movieDecoder.next24Bit;
        DecDCTvlc2(bs, g_movieDecoder.mdecInput[g_movieDecoder.bufferIndex], g_movieVlcTable);
        MovieReleaseFrame();
        g_movieState.playing = 1;
    }
    setCdBusyFlag();
}

void MovieStop(void) {
    MovieStopReading();
    clearCdBusyFlag();
    sndCmdE2();
    if (!(g_movieState.flags & MOVIE_AUDIO_ONLY)) {
        g_activeDispEnv->isrgb24 = 0;
        g_prevDispEnv->isrgb24 = 0;
        ExitMovieMode();
        g_fieldContext.movieMaskOff = 1;
        g_fieldContext.hideActors = 0;
    }
    g_movieState.playing = 0;
    g_movieState.audioStreaming = 0;
}

void MovieUpdate(void) {
    u32 *bs;

    if (!(g_movieState.flags & MOVIE_AUDIO_ONLY)) {
        if (g_movieState.playing == 0) {
            return;
        }
        g_movieState.pictureDone = 0;
        g_movieDecoder.stripRect.x = g_activeDrawEnv->clip.x;
        g_movieDecoder.unk1C.x = g_activeDrawEnv->clip.x;
        g_movieDecoder.stripRect.y = g_activeDrawEnv->clip.y;
        g_movieDecoder.unk1C.y = g_activeDrawEnv->clip.y;
        g_movieDecoder.is24Bit = g_movieDecoder.next24Bit;
        if (g_movieDecoder.next24Bit != 0) {
            g_movieDecoder.stripRect.w = 24;
            g_movieDecoder.unk1C.w = 30;
        } else {
            g_movieDecoder.stripRect.w = 16;
            g_movieDecoder.unk1C.w = 20;
        }
        DecDCTin(g_movieDecoder.mdecInput[g_movieDecoder.bufferIndex], g_movieDecoder.is24Bit);
        DecDCTout(g_movieDecoder.stripBuffer, g_movieDecoder.stripRect.w * g_movieDecoder.stripRect.h / 2);
        do {
            bs = MovieNextFrame();
        } while (bs == NULL && g_movieReader.nextSector < g_movieState.endSector);
        if (bs == NULL) {
            MovieStop();
        } else {
            g_movieDecoder.next24Bit = g_movieSlots[g_movieState.decodeSlot].is24Bit;
            g_activeDispEnv->isrgb24 = g_movieDecoder.next24Bit;
            g_movieDecoder.bufferIndex = g_movieDecoder.bufferIndex == 0;
            DecDCTvlc2(bs, g_movieDecoder.mdecInput[g_movieDecoder.bufferIndex], g_movieVlcTable);
            MovieReleaseFrame();
        }
        while (g_movieState.pictureDone == 0) {
            MovieResumeReading();
        }
        return;
    }
    if (g_movieState.audioStreaming == 0) {
        return;
    }
    if (g_movieStalled != 0) {
        if (g_movieReader.reading != 0 && g_sndStream.tickCounter - g_sndStream.unk20 >= (g_movieState.bufferSize >> 1)) {
            sndDisableReverb(3);
            g_movieStalled = 0;
        }
        g_movieReader.reading = 0;
    } else if (g_movieState.readSlot == g_sndStream.unk34) {
        g_movieReader.reading = 0;
        sndEnableReverb(3);
        g_movieStalled = 1;
    }
    if (g_movieReader.reading == 0) {
        MovieStartReading();
    }
    if (g_movieReader.nextSector >= g_movieState.endSector) {
        MovieStop();
    }
}

s32 MovieGetFrame(void) {
    return g_movieSlots[g_movieState.decodeSlot].frame;
}

s32 MovieIsPlaying(void) {
    return g_movieState.playing;
}

s32 MovieIsAudioStreaming(void) {
    return g_movieState.audioStreaming;
}

static void MovieInitReader(void) {
    u8 param[4];
    u8 mode;

    g_movieReader.cdMode = CdlModeSpeed | CdlModeSize1;
    g_movieReader.dest = g_movieSlots[g_movieState.readSlot].video;
    g_movieReader.reading = 0;
    g_movieReader.nextSector = g_movieState.firstSector;
    g_movieReader.savedSyncCallback = CdSyncCallback(NULL);
    g_movieReader.savedReadyCallback = CdReadyCallback(NULL);
    if (CdStatus() & (CdlStatPlay | CdlStatSeek)) {
        CdControlB(CdlPause, NULL, NULL);
    }
    while (MovieStartReading() == 0) {
    }
    mode = g_movieReader.cdMode;
    param[0] = mode;
    if (mode != CdMode()) {
        while (CdControl(CdlSetmode, param, NULL) == 0) {
        }
    }
}

static void MovieStopReading(void) {
    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    CdFlush();
    CdControl(CdlPause, NULL, NULL);
}

static void MovieReadCallback(u8 intr, u8 *result) {
    MovieSectorHeader header;

    if (g_movieDecoder.is24Bit != 0 && (DMA1_CHCR & DMA_CHCR_BUSY)) {
        g_movieState.readDeferred = 1;
        return;
    }
    intr = CdReady(1, result);
    if (intr == CdlDataReady && g_movieReader.reading != 0) {
        CdGetSector(&header, sizeof(header) / 4);
        if (CdPosToInt(&header.loc) != g_movieReader.nextSector) {
            g_movieReader.reading = 0;
        }
        if (g_movieReader.reading != 0) {
            CdGetSector(&g_movieSectorTag, 2);
            switch (g_movieSectorTag.type) {
            case 'J':
                CdGetSector(g_movieReader.dest, MOVIE_SECTOR_DATA_WORDS);
                g_movieReader.dest += MOVIE_SECTOR_DATA_WORDS * 4;
                break;
            case 'N':
                CdGetSector(&g_movieSlotBlocks[g_movieState.readSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_N_SECTOR], MOVIE_SECTOR_DATA_WORDS);
                break;
            case 'R':
                CdGetSector(&g_movieSlotBlocks[g_movieState.readSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_R_SECTOR], MOVIE_SECTOR_DATA_WORDS);
                break;
            }
            g_movieReader.nextSector++;
            g_movieSlots[g_movieState.readSlot].lastSector = g_movieSectorTag.index;
            g_movieSlots[g_movieState.readSlot].lastIndex = g_movieSectorTag.lastIndex;
            g_movieSlots[g_movieState.readSlot].frame = g_movieSectorTag.frame;
            if (g_movieSlots[g_movieState.readSlot].lastSector == g_movieSlots[g_movieState.readSlot].lastIndex) {
                g_movieSlots[g_movieState.readSlot].state = MOVIE_SLOT_FULL;
                g_movieState.framesRead++;
                if (++g_movieState.readSlot >= g_movieState.bufferSize) {
                    g_movieState.readSlot = 0;
                }
                g_movieReader.dest = g_movieSlots[g_movieState.readSlot].video;
                if (g_movieSlots[g_movieState.readSlot].state != MOVIE_SLOT_EMPTY || g_movieState.readSlot == g_sndStream.unk34) {
                    MovieStopReading();
                    g_movieReader.reading = 0;
                    return;
                }
            }
        }
    } else {
        g_movieReader.reading = 0;
    }
    if (g_movieReader.reading == 0) {
        MovieStartReading();
    }
    if (g_movieReader.nextSector >= g_movieState.endSector) {
        MovieStopReading();
    }
}

static void MovieAudioReadCallback(u8 intr, u8 *result) {
    MovieSectorHeader header;
    s32 last;
    s32 mask;

    // A no-op on a u8; it puts the zero-extension of intr at the top of the function, where the original has it.
    mask = 0xFF;
    intr &= mask;
    if (intr == CdlDataReady && g_movieReader.reading != 0) {
        CdGetSector(&header, sizeof(header) / 4);
        if (CdPosToInt(&header.loc) != g_movieReader.nextSector) {
            g_movieReader.reading = 0;
        }
        if (g_movieReader.reading != 0) {
            g_movieState.audioStreaming = intr;
            CdGetSector(&g_movieSectorTag, 2);
            switch (g_movieSectorTag.type) {
            case 'N':
                last = g_sndStream.unk34 - 1;
                if (last < 0) {
                    last = g_movieState.bufferSize - 1;
                }
                if (last != g_movieState.readSlot) {
                    CdGetSector(&g_movieSlotBlocks[g_movieState.readSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_N_SECTOR], MOVIE_SECTOR_DATA_WORDS);
                    g_movieReader.nextSector++;
                } else {
                    g_movieReader.reading = 0;
                }
                break;
            case 'R':
                CdGetSector(&g_movieSlotBlocks[g_movieState.readSlot * MOVIE_SLOT_INFO_SIZE + MOVIE_SLOT_R_SECTOR], MOVIE_SECTOR_DATA_WORDS);
                g_movieReader.nextSector++;
                sndTickCounters();
                if (++g_movieState.readSlot >= g_movieState.bufferSize) {
                    g_movieState.readSlot = 0;
                }
                g_movieState.framesRead++;
                break;
            }
        }
    } else {
        g_movieReader.reading = 0;
    }
    if (g_movieReader.reading == 0) {
        MovieStartReading();
    }
    if (g_movieReader.nextSector >= g_movieState.endSector) {
        MovieStopReading();
    }
}

static s32 MovieStartReading(void) {
    u8 param[4];
    CdlLOC loc;
    u8 mode;

    g_movieReader.reading = 0;
    CdSyncCallback(NULL);
    CdReadyCallback(NULL);
    mode = g_movieReader.cdMode;
    param[0] = mode;
    if (mode != CdMode()) {
        if (CdControl(CdlSetmode, param, NULL) == 0) {
            return 0;
        }
    }
    CdFlush();
    CdIntToPos(g_movieReader.nextSector, &loc);
    if (CdControl(CdlSetloc, (u8 *)&loc, NULL) == 0) {
        return 0;
    }
    if (g_movieState.flags & MOVIE_AUDIO_ONLY) {
        CdReadyCallback(MovieAudioReadCallback);
    } else {
        CdReadyCallback(MovieReadCallback);
    }
    CdControlF(CdlReadN, NULL);
    g_movieReader.reading = 1;
    return 1;
}

static void MovieDecodeCallback(void) {
    s32 limit;

    if (g_movieState.readDeferred != 0) {
        MovieReadCallback(0xFF, NULL);
        g_movieState.readDeferred = 0;
    }
    LoadImage(&g_movieDecoder.stripRect, g_movieDecoder.stripBuffer);
    g_movieDecoder.stripRect.x += g_movieDecoder.stripRect.w;
    if (g_movieDecoder.is24Bit == 0) {
        limit = g_activeDrawEnv->clip.x + g_movieDecoder.width;
    } else {
        limit = g_activeDrawEnv->clip.x + ((g_movieDecoder.width * 3) >> 1);
    }
    if (g_movieDecoder.stripRect.x < limit) {
        DecDCTout(g_movieDecoder.stripBuffer, g_movieDecoder.stripRect.w * g_movieDecoder.stripRect.h / 2);
    } else {
        g_movieState.pictureDone = 1;
    }
}
