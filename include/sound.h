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
    /* 0x00 */ s32 voice;
    /* 0x04 */ s32 updateFlags;   /**< Bitmask of pending SPU register updates. */
    /* 0x08 */ u32 startAddr;     /**< SPU sample start address. */
    /* 0x0C */ u32 repeatAddr;    /**< SPU sample repeat/loop address. */
    /* 0x10 */ u16 sampleRate;    /**< Voice pitch / sample rate. */
    /* 0x12 */ u16 adsrLow;       /**< ADSR envelope low halfword. */
    /* 0x14 */ u16 adsrHigh;      /**< ADSR envelope high halfword. */
    /* 0x16 */ u16 volScale;
    /* 0x18 */ s16 volLeft;       /**< Left channel volume. */
    /* 0x1A */ s16 volRight;      /**< Right channel volume. */
} SndVoiceParam; /* 0x1C = 28 bytes */

/**
 * @brief Sound sequence track (D_80070D60, stride 272 bytes).
 *
 * Each track controls one voice/channel of a sound sequence. The array
 * at D_80070D60 holds all active tracks.
 */
typedef struct {
    /* 0x00 */ u8 *seqPtr;
    /* 0x04 */ u8 *loopPoint[4];
    /* 0x14 */ u8 *drumOffset;
    /* 0x18 */ s16 *vibratoWave;
    /* 0x1C */ s16 *tremoloWave;
    /* 0x20 */ s16 *panLfoWave;
    /* 0x24 */ s32 keyOnMask;
    /* 0x28 */ u32 basePitch;
    /* 0x2C */ s32 pitchSlide;
    /* 0x30 */ s32 flags;
    /* 0x34 */ s32 bankPtr;
    /* 0x38 */ s32 keyOffMask;
    /* 0x3C */ s32 pitchMulSound;
    /* 0x40 */ s32 pitchMulSoundSlideStep;
    /* 0x44 */ s32 volumeLevel;
    /* 0x48 */ s32 volSlideStep;
    /* 0x4C */ s32 pitchSlideStep;
    /* 0x50 */ s32 fineTunePitch;
    /* 0x54 */ s32 unk54;
    /* 0x58 */ s32 pitchValue;
    /* 0x5C */ s32 pitchValueSlideStep;
    /* 0x60 */ u16 playingType;
    /* 0x62 */ u16 volume;
    /* 0x64 */ u16 volumeBase;
    /* 0x66 */ u16 instrument;
    /* 0x68 */ u16 pad68;
    /* 0x6A */ u16 pitchModDepth;
    /* 0x6C */ u16 pitchModCounter;
    /* 0x6E */ u16 unk6E;
    /* 0x70 */ u16 loopTimes[4];
    /* 0x78 */ u16 loopUnk6E[4];
    /* 0x80 */ u16 volumeMultiplier;
    /* 0x82 */ u16 volumeMultiplierSlideSteps;
    /* 0x84 */ u16 pitchMulSoundSlideSteps;
    /* 0x86 */ u16 volSlideSteps;
    /* 0x88 */ u16 pitchValueSlideSteps;
    /* 0x8A */ u16 volBalanceSlideSteps;
    /* 0x8C */ u16 volPan;
    /* 0x8E */ u16 volPanSlideSteps;
    /* 0x90 */ u16 pitchSlideSteps;
    /* 0x92 */ u16 panpot;
    /* 0x94 */ u16 durationDelta;
    /* 0x96 */ u16 durationCounter;
    /* 0x98 */ u16 duration;
    /* 0x9A */ u16 timerActive;
    /* 0x9C */ u16 unk9C;
    /* 0x9E */ u16 vibratoDelay;
    /* 0xA0 */ u16 vibratoDelayCur;
    /* 0xA2 */ u16 vibratoRate;
    /* 0xA4 */ u16 vibratoRateCur;
    /* 0xA6 */ u16 vibratoType;
    /* 0xA8 */ u16 vibratoBase;
    /* 0xAA */ u16 vibratoDepth;
    /* 0xAC */ u16 vibratoDepthSlideSteps;
    /* 0xAE */ u16 vibratoDepthSlideStep;
    /* 0xB0 */ u16 unkB0;
    /* 0xB2 */ u16 tremoloDelay;
    /* 0xB4 */ u16 tremoloDelayCur;
    /* 0xB6 */ u16 tremoloRate;
    /* 0xB8 */ u16 tremoloRateCur;
    /* 0xBA */ u16 tremoloType;
    /* 0xBC */ u16 tremoloDepth;
    /* 0xBE */ u16 tremoloDepthSlideSteps;
    /* 0xC0 */ u16 tremoloDepthSlideStep;
    /* 0xC2 */ u16 unkC2;
    /* 0xC4 */ u16 panLfoRate;
    /* 0xC6 */ u16 panLfoRateCur;
    /* 0xC8 */ u16 panLfoType;
    /* 0xCA */ u16 panLfoDepth;
    /* 0xCC */ u16 panLfoDepthSlideSteps;
    /* 0xCE */ u16 panLfoDepthSlideStep;
    /* 0xD0 */ u16 noiseSwitchDelay;
    /* 0xD2 */ u16 pitchLfoSwitchDelay;
    /* 0xD4 */ u16 loopId;
    /* 0xD6 */ u16 volumeDelta;
    /* 0xD8 */ u16 volumeAccum;
    /* 0xDA */ s16 volumeMultiplierSlideStep;
    /* 0xDC */ s16 unkDC;
    /* 0xDE */ u16 volBalance;
    /* 0xE0 */ s16 volBalanceSlideStep;
    /* 0xE2 */ s16 volPanSlideStep;
    /* 0xE4 */ u16 detune;
    /* 0xE6 */ s16 fineTuning;
    /* 0xE8 */ s16 unkE8;
    /* 0xEA */ u16 durationDelta2;
    /* 0xEC */ u16 ecField;
    /* 0xEE */ s16 vibratoPitch;
    /* 0xF0 */ s16 tremoloVol;
    /* 0xF2 */ s16 panLfoVol;
    /* 0xF4 */ SndVoiceParam voiceAttr;
} SoundSeqTrack; /* 0x110 = 272 bytes */

/** @brief 12-entry CD-audio track array (voices 12-23), stride 0x110. */
extern SoundSeqTrack D_80070D60[];
extern SoundSeqTrack D_80072F70[];
extern SoundSeqTrack D_80075080[];

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

/**
 * @brief Pointer to the active sound-engine state / primary sequence track.
 *
 * Paired with the @c D_80070D60 track array as the primary sound source
 * (see @c sndTransferData).
 */
typedef struct {
    /* 0x00 */ u8 instrument;
    /* 0x01 */ u8 key;
    /* 0x02 */ u8 adsr0;
    /* 0x03 */ u8 adsr1;
    /* 0x04 */ u8 mode;
    /* 0x05 */ u8 adsr2;
    /* 0x06 */ u8 volScale;
    /* 0x07 */ u8 pan;
} SndDrumEntry;

typedef struct SndSeqHeader {
    /* 0x00 */ u8 pad00[0x14];
    /* 0x14 */ s32 unk14;
    /* 0x18 */ u8 pad18[8];
    /* 0x20 */ u32 trackMask;
    /* 0x24 */ u32 unk24;
    /* 0x28 */ u32 unk28;
    /* 0x2C */ u8 pad2C[4];
    /* 0x30 */ s32 unk30;
    /* 0x34 */ s32 unk34;
    /* 0x38 */ u8 pad38[8];
    /* 0x40 */ u16 trackOffsets[1];
} SndSeqHeader;

typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ u32 activeMask;
    /* 0x08 */ u32 unk08;
    /* 0x0C */ u32 unk0C;
    /* 0x10 */ u32 unk10;
    /* 0x14 */ u32 unk14;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u32 activeMaskStored;
    /* 0x20 */ s32 tempo;
    /* 0x24 */ s32 tempoSlideStep;
    /* 0x28 */ s32 unk28;
    /* 0x2C */ struct SndSeqHeader *unk2C;
    /* 0x30 */ s16 *unk30;
    /* 0x34 */ SndDrumEntry *drumMap;
    /* 0x38 */ s32 unk38;
    /* 0x3C */ u32 noiseMask;
    /* 0x40 */ u32 reverbMask;
    /* 0x44 */ u32 pitchLfoMask;
    /* 0x48 */ s32 reverbDepth;
    /* 0x4C */ s32 reverbDepthSlideStep;
    /* 0x50 */ s32 volMul;
    /* 0x54 */ s32 volMulSlideStep;
    /* 0x58 */ s16 volMulSlideSteps;
    /* 0x5A */ s16 reverbDepthSlideSteps;
    /* 0x5C */ u16 tempoSlideSteps;
    /* 0x5E */ u16 musicId;
    /* 0x60 */ u16 condition;
    /* 0x62 */ u16 noiseClock;
    /* 0x64 */ u16 timerUpper;
    /* 0x66 */ u16 timerUpperCur;
    /* 0x68 */ u16 timerLower;
    /* 0x6A */ u16 timerLowerCur;
    /* 0x6C */ u16 timerTopCur;
    /* 0x6E */ u16 unk6E;
    /* 0x70 */ u8 unk70[16];
} SoundMusicConfig;

typedef struct {
    /* 0x00 */ u32 activeMask;
    /* 0x04 */ u32 unk04;
    /* 0x08 */ u32 unk08;
    /* 0x0C */ u32 unk0C;
    /* 0x10 */ u32 activeMaskStored;
    /* 0x14 */ s32 tempo;
    /* 0x18 */ u32 tempoUpdate;
    /* 0x1C */ u32 noiseMask;
    /* 0x20 */ u32 reverbMask;
    /* 0x24 */ u32 pitchLfoMask;
    /* 0x28 */ u16 noiseClock;
    /* 0x2A */ u16 unk2A;
    /* 0x2C */ u32 unk2C;
} SoundSfxConfig;

extern SoundSfxConfig D_80075028;

typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ s32 unk08;
    /* 0x0C */ s32 unk0C;
} UnkStruct80074F10;

extern UnkStruct80074F10 D_80074F10;

typedef struct {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s16 envelope;
    /* 0x06 */ s16 unk06;
} SndVoiceEnvelope;

extern SndVoiceEnvelope D_80073CB0[];
extern s32 D_8007506C;
extern s32 D_800772F4;
extern u32 D_800772A4;
extern s32 D_80077280;
extern s32 D_80077284;
extern s32 D_8007507C;
extern s32 D_80077368[];
extern s32 D_80072F60[];
extern s32 D_80072F68;
extern SoundSeqTrack *D_80074EE0;
extern void sndSetReverbMode(s32 mode);
extern s32 D_80073DE4;
extern u8 D_8005182F;
extern SoundMusicConfig *D_800772F8[];
extern u8 D_800517F0[];

extern SoundMusicConfig *g_sndSeqState;
extern SoundMusicConfig D_80073D70;
extern SoundMusicConfig D_80073DF0;
extern SoundMusicConfig *D_80073CA8;
extern SoundSeqTrack *D_80073C34;
extern s32 D_80074EB0;
extern s32 D_80074F18;
extern s16 *D_80052258[];

/** @brief 12-entry SFX play queue, replayed by @c func_800BF718 after a sound reset. */
extern VoicePoolEntry D_80074F20[12];

/**
 * @brief Sound command parameter block.
 */
typedef struct {
    /* 0x00 */ s32 params[3];
    /* 0x0C */ s32 playCount;
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

/**
 * @brief CD audio / streaming sound state (@c g_sndStream).
 *
 * Controls SPU streaming playback, tracking voice indices, pitch,
 * volume/pan, loop counters and IRQ state.
 */
typedef struct {
    /* 0x00 */ s32 src;
    /* 0x04 */ s32 loopSrc;
    /* 0x08 */ s32 flags;          /**< Control flags (bit 24 gates the tick IRQ callback). */
    /* 0x0C */ s32 active;         /**< Active/IRQ voice bitmask (nonzero = playing). */
    /* 0x10 */ s32 voiceIdx;       /**< First SPU voice index for this stream. */
    /* 0x14 */ u32 remaining;
    /* 0x18 */ s32 unk18;
    /* 0x1C */ s32 loopSize;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 tickCounter;    /**< Tick counter (incremented each frame). */
    /* 0x28 */ s32 tickCount;      /**< Running tick accumulator. */
    /* 0x2C */ s32 unk2C;          /**< Engine state set by sndStreamInitEngine. */
    /* 0x30 */ s32 unk30;          /**< Engine state set by sndStreamInitEngine. */
    /* 0x34 */ s32 unk34;          /**< Set to -1 during init. */
    /* 0x38 */ s32 frameCounter;   /**< Frame counter; wraps back to 0 at loopLimit. */
    /* 0x3C */ s32 loopLimit;      /**< Loop/sector limit (sectorCount >> 12). */
    /* 0x40 */ s32 panVolume;      /**< Raw pan/volume value. */
    /* 0x44 */ s32 panStep;
    /* 0x48 */ s32 panTarget;      /**< Pan target (cleared when setting pan). */
    /* 0x4C */ s32 unk4C;
    /* 0x50 */ u8 pad50[8];
    /* 0x58 */ s32 savedPitch;     /**< Saved pitch value for voice restore. */
    /* 0x5C */ u8 pad5C[0x04];
} SoundStream; /* 0x60 = 96 bytes */

typedef struct {
    /* 0x00 */ u32 magic;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ u8 pad08[8];
    /* 0x10 */ u32 size;
    /* 0x14 */ u32 loopOffset;
    /* 0x18 */ s32 flags;
    /* 0x1C */ u16 pitch;
    /* 0x1E */ u16 pad1E;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
} SndStreamHeader;

extern s32 D_800772D8;
extern s32 D_800772F0;
extern s16 D_80051854[];
extern s16 D_80051954;
extern s32 D_800517F4[];

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

/** @brief Toggle the current sound-bank-selector flag and return a pointer to the new bank table. */
extern u8 *toggleSoundBank(void);

/** @brief Sound-bank A: the staging table @c toggleSoundBank returns when the selector reads as @c 0. */
extern u8 D_8005F388[];

/** @brief Sound-bank B: the staging table @c toggleSoundBank returns when the selector reads as non-zero. */
extern u8 D_80063388[];

/** @brief Threshold tested by @c SPUSYNC — top-of-stack values below this
 *         indicate a known SPU sample slot and can be popped immediately. */
extern u32 D_800772B8;



#endif /* SOUND_H */

/** @brief snd_note DMA-completion callback; its address is handed to OpenEvent by snd_dma.c. */
void func_8001AD60(void);
void func_800151C4(SoundSeqTrack *track, u32 bit, s32 isSfx);
void func_8001BAA8(SoundSeqTrack *track, u32 bit);
