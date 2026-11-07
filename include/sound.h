#ifndef SOUND_H
#define SOUND_H

#include "common.h"

/**
 * @brief Sentinel value for an inactive sound channel handle.
 *
 * Follows PsyQ libsnd convention: @c SsUtKeyOn / @c SsSeqPlay and similar
 * APIs return @c -1 to signal that no sound is playing on that channel.
 * FF8 propagates that sentinel through its own command-bus wrappers
 * (e.g. @c sndCmd11) — fields holding sound handles compare against this
 * before issuing stop / fade commands.
 */
#define SND_HANDLE_NONE (-1)

/**
 * @brief SPU voice parameter block passed to func_800150A8.
 *
 * Contains the pending SPU register values for a single voice.
 * Field updateFlags holds a bitmask indicating which registers need writing.
 */
typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s32 updateFlags;   /**< Bitmask of pending SPU register updates. */
    /* 0x08 */ u32 startAddr;     /**< SPU sample start address. */
    /* 0x0C */ u32 repeatAddr;    /**< SPU sample repeat/loop address. */
    /* 0x10 */ u16 sampleRate;    /**< Voice pitch / sample rate. */
    /* 0x12 */ u16 adsrLow;       /**< ADSR envelope low halfword. */
    /* 0x14 */ u16 adsrHigh;      /**< ADSR envelope high halfword. */
    /* 0x16 */ u16 sweep;         /**< Volume sweep / scale value. */
    /* 0x18 */ s16 volLeft;       /**< Left channel volume. */
    /* 0x1A */ s16 volRight;      /**< Right channel volume. */
} SndVoiceParam; /* 0x1C = 28 bytes */

typedef struct {
    /* 0x00 */ u8 unk00;
    /* 0x01 */ u8 unk01;
    /* 0x02 */ u8 unk02;
    /* 0x03 */ u8 unk03;
    /* 0x04 */ u8 unk04;
    /* 0x05 */ u8 unk05;
    /* 0x06 */ u8 unk06;
    /* 0x07 */ u8 unk07;
} func_8001B42C_entry;

typedef struct {
    /* 0x00 */ u8 unk00;
    /* 0x01 */ u8 unk01;
    /* 0x02 */ u8 unk02;
    /* 0x03 */ u8 unk03;
    /* 0x04 */ u8 unk04;
    /* 0x05 */ u8 unk05;
    /* 0x06 */ u8 unk06;
    /* 0x07 */ u8 unk07;
} func_8001B820_entry;

/**
 * @brief Sound sequence track (D_80070D60, stride 272 bytes).
 *
 * Each track controls one voice/channel of a sound sequence. The array
 * at D_80070D60 holds all active tracks.
 */
typedef struct {
    /* 0x00 */ u8 *unk00;
    /* 0x04 */ u8 *unk04[4];
    /* 0x14 */ func_8001B42C_entry *unk14;
    /* 0x18 */ s16 *unk18;
    /* 0x1C */ s16 *unk1C;
    /* 0x20 */ s16 *unk20;
    /* 0x24 */ s32 keyOnMask;
    /* 0x28 */ u32 loopCounter;
    /* 0x2C */ s32 cmdDataPtr;
    /* 0x30 */ s32 flags;
    /* 0x34 */ s32 bankPtr;
    /* 0x38 */ s32 keyOffMask;
    /* 0x3C */ s32 loopLimit;
    /* 0x40 */ s32 panRaw;
    /* 0x44 */ s32 panShifted;
    /* 0x48 */ s32 panTarget;
    /* 0x4C */ s32 unk4C;
    /* 0x50 */ s32 timing;
    /* 0x54 */ s32 unk54;
    /* 0x58 */ s32 pitchValue;
    /* 0x5C */ s32 unk5C;
    /* 0x60 */ u16 voiceActive;
    /* 0x62 */ u16 volume;
    /* 0x64 */ u16 volumeBase;
    /* 0x66 */ u16 instrument;
    /* 0x68 */ u16 unk68;
    /* 0x6A */ u16 pitchModDepth;
    /* 0x6C */ u16 pitchModCounter;
    /* 0x6E */ u16 unk6E;
    /* 0x70 */ u16 unk70[4];
    /* 0x78 */ u16 unk78[4];
    /* 0x80 */ u16 pitchBend;
    /* 0x82 */ u16 unk82;
    /* 0x84 */ u16 pitchFadeCounter;
    /* 0x86 */ u16 panFade;
    /* 0x88 */ u16 panFadeTarget;
    /* 0x8A */ u16 exprFade;
    /* 0x8C */ u16 notePitch;
    /* 0x8E */ u16 notePitchFade;
    /* 0x90 */ u16 unk90;
    /* 0x92 */ u16 panpot;
    /* 0x94 */ u16 durationDelta;
    /* 0x96 */ u16 durationCounter;
    /* 0x98 */ u16 duration;
    /* 0x9A */ u16 timerActive;
    /* 0x9C */ u16 unk9C;
    /* 0x9E */ u16 unk9E;
    /* 0xA0 */ u16 unkA0;
    /* 0xA2 */ u16 unkA2;
    /* 0xA4 */ u16 unkA4;
    /* 0xA6 */ u16 unkA6;
    /* 0xA8 */ u16 unkA8;
    /* 0xAA */ u16 delaySend;
    /* 0xAC */ u16 delaySendTarget;
    /* 0xAE */ u16 unkAE;
    /* 0xB0 */ u16 unkB0;
    /* 0xB2 */ u16 unkB2;
    /* 0xB4 */ u16 unkB4;
    /* 0xB6 */ u16 unkB6;
    /* 0xB8 */ u16 unkB8;
    /* 0xBA */ u16 unkBA;
    /* 0xBC */ u16 volumeLfo;
    /* 0xBE */ u16 volumeLfoTarget;
    /* 0xC0 */ u16 unkC0;
    /* 0xC2 */ u16 unkC2;
    /* 0xC4 */ u16 unkC4;
    /* 0xC6 */ u16 unkC6;
    /* 0xC8 */ u16 unkC8;
    /* 0xCA */ u16 panLfo;
    /* 0xCC */ u16 panLfoTarget;
    /* 0xCE */ u16 unkCE;
    /* 0xD0 */ u16 noiseDuration;
    /* 0xD2 */ u16 reverbDuration;
    /* 0xD4 */ u16 panEnvSpeed;
    /* 0xD6 */ u16 volumeDelta;
    /* 0xD8 */ u16 volumeAccum;
    /* 0xDA */ s16 unkDA;
    /* 0xDC */ s16 unkDC;
    /* 0xDE */ u16 expression;
    /* 0xE0 */ s16 unkE0;
    /* 0xE2 */ s16 unkE2;
    /* 0xE4 */ u16 detune;
    /* 0xE6 */ s16 detuneTarget;
    /* 0xE8 */ s16 unkE8;
    /* 0xEA */ u16 durationDelta2;
    /* 0xEC */ u16 ecField;
    /* 0xEE */ s16 portamento;
    /* 0xF0 */ s16 volumeLfoStop;
    /* 0xF2 */ s16 panLfoStop;
    /* 0xF4 */ SndVoiceParam unkF4;
} SoundSeqTrack; /* 0x110 = 272 bytes */

/** @brief 12-entry CD-audio track array (voices 12-23), stride 0x110. */
extern SoundSeqTrack D_80072F70[];

/**
 * @brief Instrument/sample descriptor (D_80073E68, stride 16 bytes).
 *
 * Each entry describes one SPU instrument with sample address,
 * loop point, and ADSR envelope parameters.
 */
typedef struct {
    /* 0x00 */ s32 sampleAddr;    /**< SPU sample start address. */
    /* 0x04 */ s32 loopAddr;      /**< SPU sample loop address. */
    /* 0x08 */ s16 unk08;
    /* 0x0A */ s16 unk0A;
    /* 0x0C */ u16 adsrLow;       /**< ADSR envelope low halfword. */
    /* 0x0E */ u16 adsrHigh;      /**< ADSR envelope high halfword. */
} SndInstrument; /* 16 bytes */

/** @brief Loaded sound sample bank, parsed by @c sndLoadBank.
 *
 * A 16-byte header (checked by @c sndValidateBank) followed by three
 * regions whose base pointers are cached in @c D_80074ED0 / @c D_80074ED8 /
 * @c D_80074EDC for playback lookups. */
typedef struct {
    /* 0x000 */ u8  header[0x10];            /**< Validated by sndValidateBank. */
    /* 0x010 */ u16 sampleOffsets[0x200];    /**< Sample-offset pairs (base cached in D_80074ED0). */
    /* 0x410 */ u16 instrumentGroups[0x100]; /**< Instrument->group table (base cached in D_80074ED8). */
    /* 0x610 */ u8  sampleData[1];           /**< Variable-length ADPCM sample data (base cached in D_80074EDC). */
} SoundBank;

/** @brief Base of the loaded bank's sample-offset table (points into a @ref SoundBank). */
extern u16 *D_80074ED0;
/** @brief Base of the loaded bank's instrument->group table (points into a @ref SoundBank). */
extern u16 *D_80074ED8;
/** @brief Base of the loaded bank's ADPCM sample data (points into a @ref SoundBank). */
extern u8  *D_80074EDC;

/** @brief Sound-bank streaming/DMA state at @c D_80077358.
 *
 * Tracks an in-progress bank upload driven by @c sndStreamBank: the
 * decode source pointer, the SPU destination address, and the two
 * remaining byte counts (one for the ADPCM decode pass, one for the SPU
 * DMA). All four are seeded on the first chunk and drained as the bank
 * streams out. */
typedef struct {
    /* 0x00 */ s32 decodePtr;    /**< Instrument-table source pointer fed to @c func_800146F0. */
    /* 0x04 */ s32 spuAddr;      /**< Current SPU transfer address. */
    /* 0x08 */ u32 spuBytes;     /**< Bytes still to DMA to the SPU. */
    /* 0x0C */ u32 decodeBytes;  /**< Bytes still to decode through @c func_800146F0. */
} DmaState; /* 16 bytes */

extern DmaState D_80077358;

/** @brief Voice pool entry (D_80074F20, stride 16 bytes).
 *
 * Used by @c func_800BF718 to replay any queued SFX after a sound reset:
 * negative @c cmd → @c sndPlayBankSfx(cmd, arg1, fieldC, field8);
 * positive @c cmd → @c sndPlaySfx(cmd-1, arg1, fieldC, field8). */
typedef struct {
    /* 0x00 */ s32 cmd;           /**< Pending command (negative = bank SFX, zero = empty, positive = direct SFX + 1). */
    /* 0x04 */ s32 arg1;          /**< First passthrough arg. */
    /* 0x08 */ s32 field8;        /**< Passed as the 4th arg to @c sndPlay*. */
    /* 0x0C */ s32 fieldC;        /**< Passed as the 3rd arg to @c sndPlay*. */
} VoicePoolEntry; /* 16 bytes */

typedef struct {
    /* 0x00 */ u32 unk00;
    /* 0x04 */ u16 unk04;
    /* 0x06 */ u16 unk06;
    /* 0x08 */ u16 unk08;
    /* 0x0A */ u8 pad0A[0x0A];
    /* 0x14 */ s32 unk14;
    /* 0x18 */ u8 pad18[0x08];
    /* 0x20 */ u32 unk20;
    /* 0x24 */ u32 unk24;
    /* 0x28 */ u32 unk28;
    /* 0x2C */ u8 pad2C[0x04];
    /* 0x30 */ s32 unk30;
    /* 0x34 */ s32 unk34;
} func_8001A1E8_data;

typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ u32 unk04;
    /* 0x08 */ u32 unk08;
    /* 0x0C */ u32 unk0C;
    /* 0x10 */ u32 unk10;
    /* 0x14 */ u32 unk14;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u32 unk1C;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2C */ func_8001A1E8_data *unk2C;
    /* 0x30 */ s16 *unk30;
    /* 0x34 */ func_8001B820_entry *unk34;
    /* 0x38 */ s8 *unk38;
    /* 0x3C */ u32 unk3C;
    /* 0x40 */ u32 unk40;
    /* 0x44 */ u32 unk44;
    /* 0x48 */ s32 unk48;
    /* 0x4C */ s32 unk4C;
    /* 0x50 */ s32 unk50;
    /* 0x54 */ s32 unk54;
    /* 0x58 */ s16 unk58;
    /* 0x5A */ s16 unk5A;
    /* 0x5C */ u16 unk5C;
    /* 0x5E */ u16 unk5E;
    /* 0x60 */ u16 unk60;
    /* 0x62 */ u16 unk62;
    /* 0x64 */ u16 unk64;
    /* 0x66 */ u16 unk66;
    /* 0x68 */ u16 unk68;
    /* 0x6A */ u16 unk6A;
    /* 0x6C */ u16 unk6C;
    /* 0x6E */ u16 unk6E;
    /* 0x70 */ u8 unk70[16];
} SoundMusicConfig;

typedef struct {
    /* 0x00 */ u32 unk00;
    /* 0x04 */ u32 unk04;
    /* 0x08 */ u32 unk08;
    /* 0x0C */ u32 unk0C;
    /* 0x10 */ u32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u32 unk1C;
    /* 0x20 */ u32 unk20;
    /* 0x24 */ u32 unk24;
    /* 0x28 */ u16 unk28;
    /* 0x2A */ u16 unk2A;
    /* 0x2C */ u32 unk2C;
} SoundSfxConfig;

extern SoundMusicConfig *g_sndSeqState;
extern SoundSfxConfig D_80075028;
extern SoundMusicConfig *D_80073CA8;
extern SoundSeqTrack D_80070D60[];
extern SoundSeqTrack *D_80073C34;
extern SoundMusicConfig D_80073D70;
extern SoundMusicConfig D_80073DF0;
extern SoundSeqTrack *D_80074EE0;
extern s32 D_800772F4;
extern s32 D_80073DE8;
extern s32 D_80073DEC;
extern s32 D_80077280;
extern s32 D_80077284;
extern s32 D_8007507C;
extern u32 D_80077368[];
extern s32 D_80072F60[];
extern s32 D_80072F68;
extern s32 D_80074EB0;
extern s32 D_80074F18;

/** @brief 12-entry SFX play queue, replayed by @c func_800BF718 after a sound reset. */
extern VoicePoolEntry D_80074F20[12];

typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ s32 unk08;
    /* 0x0C */ s32 unk0C;
} UnkStruct80074F10;

extern UnkStruct80074F10 D_80074F10;

typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s16 unk04;
    /* 0x06 */ s16 unk06;
} UnkStruct80073CB0;

extern UnkStruct80073CB0 D_80073CB0[];
extern SoundMusicConfig *D_800772F8[];
extern u32 D_800772A4;
extern s16 *D_80052258[];
extern s32 D_800772D8;
extern s16 D_80051854[];
extern s16 D_80051954;

/**
 * @brief Sound command parameter block.
 */
typedef struct {
    /* 0x00 */ s32 params[3];
    /* 0x0C */ s32 playCount;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
} SndVoice;

/**
 * @brief Sound bank descriptor header (passed to sndValidateBank / sndUploadSample).
 *
 * The first 0x40 bytes form the header; sample data follows immediately after.
 */
typedef struct {
    /* 0x00 */ u32 magic;           /**< Validation magic (checked by sndValidateBank). */
    /* 0x04 */ s32 bankId;          /**< Bank/sample identifier; tracked in load tables (D_80073DE0, D_80074EB8). */
    /* 0x08 */ u8 pad08[0x08];     /**< Unknown header fields. */
    /* 0x10 */ s32 spuAddr;         /**< SPU sample start address / transfer size. */
    /* 0x14 */ u32 dataSize;        /**< Sample data size in bytes (DMA length); banks > 0x20000 are rejected. */
    /* 0x18 */ s32 spuSize;         /**< SPU transfer size in bytes. */
    /* 0x1C */ u32 sampleCount;     /**< Number of 16-byte sample descriptors (header size / 16); must be < 0x21. */
    /* 0x20 */ s32 spuLoadedAddr;   /**< SPU address where sample was uploaded (written after transfer). */
    /* 0x24 */ u8 pad24[0x1C];     /**< Remaining header padding (total header = 0x40 bytes). */
} SndBankDesc; /* 0x40 bytes */

typedef struct {
    /* 0x00 */ u32 unk00;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ u8 pad08[0x08];
    /* 0x10 */ u32 unk10;
    /* 0x14 */ u32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ u16 unk1C;
    /* 0x1E */ u16 pad1E;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
} func_8001E0CC_data;

/**
 * @brief CD audio / streaming sound state (@c g_sndStream).
 *
 * Controls SPU streaming playback, tracking voice indices, pitch,
 * volume/pan, loop counters and IRQ state.
 */
typedef struct {
    /* 0x00 */ u8 *spuBaseAddr;    /**< SPU base address for stream. */
    /* 0x04 */ u8 *unk04;
    /* 0x08 */ s32 flags;          /**< Control flags (bit 24 gates the tick IRQ callback). */
    /* 0x0C */ s32 active;         /**< Active/IRQ voice bitmask (nonzero = playing). */
    /* 0x10 */ s32 voiceIdx;       /**< First SPU voice index for this stream. */
    /* 0x14 */ u32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ u32 unk20;
    /* 0x24 */ s32 tickCounter;    /**< Tick counter (incremented each frame). */
    /* 0x28 */ s32 tickCount;      /**< Running tick accumulator. */
    /* 0x2C */ s32 unk2C;          /**< Engine state set by sndStreamInitEngine. */
    /* 0x30 */ s32 unk30;          /**< Engine state set by sndStreamInitEngine. */
    /* 0x34 */ s32 unk34;          /**< Set to -1 during init. */
    /* 0x38 */ s32 frameCounter;   /**< Frame counter; wraps back to 0 at loopLimit. */
    /* 0x3C */ s32 loopLimit;      /**< Loop/sector limit (sectorCount >> 12). */
    /* 0x40 */ s32 panVolume;      /**< Raw pan/volume value. */
    /* 0x44 */ s32 unk44;
    /* 0x48 */ s32 panTarget;      /**< Pan target (cleared when setting pan). */
    /* 0x4C */ s32 unk4C;
    /* 0x50 */ u8 pad50[0x08];
    /* 0x58 */ s32 savedPitch;     /**< Saved pitch value for voice restore. */
    /* 0x5C */ u8 pad5C[0x04];
} SoundStream; /* 0x60 = 96 bytes */

/** @brief CD audio / streaming sound state instance (at 0x80077298). */
extern SoundStream g_sndStream;

/**
 * @brief Sound sequence table entry (stride 20 bytes).
 *
 * Used by world's sound dispatch wrappers to look up a sequence
 * by index and feed the result to @c sndSeqStartPan and friends.
 */
typedef struct {
    /* 0x00 */ s32 bankHandle;  /**< Non-zero = bank-based playback (passed to sndPlayBankSfx). */
    /* 0x04 */ s32 field04;     /**< Sequence handle / ID (also generic SFX arg 1). */
    /* 0x08 */ s32 field08;     /**< Generic SFX arg 2 (vol). */
    /* 0x0C */ s32 field0C;     /**< Generic SFX arg 3 (pan). */
    /* 0x10 */ s16 field10;     /**< Sequence track / priority. */
    /* 0x12 */ s16 field12;     /**< Sequence runtime state (cleared by stop helper). */
} SeqEntry; /* 0x14 = 20 bytes */

/** @brief Sound-bank A: the staging table @c toggleSoundBank returns when the selector reads as @c 0. */
extern u8 D_8005F388[];

/** @brief Sound-bank B: the staging table @c toggleSoundBank returns when the selector reads as non-zero. */
extern u8 D_80063388[];

/** @brief The stream's frame number: the frame whose sound the sound engine loaded
 *         last. The same address as @c g_sndStream.unk20. */
extern u32 D_800772B8;



#endif /* SOUND_H */
