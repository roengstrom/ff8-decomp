#ifndef SOUND_H
#define SOUND_H

#include "common.h"

/**
 * @brief Sound sequence track (D_80070D60, stride 272 bytes).
 *
 * Each track controls one voice/channel of a sound sequence. The array
 * at D_80070D60 holds all active tracks.
 */
typedef struct {
    /* 0x00 */ u8 field00;
    /* 0x01 */ u8 field01;
    /* 0x02 */ u8 field02;
    /* 0x03 */ u8 field03;
    /* 0x04 */ s32 instParams;
    /* 0x08 */ u8 pad08[4];
    /* 0x0C */ u16 field0C;
    /* 0x0E */ u16 field0E;
    /* 0x10 */ s32 voiceIdx;
    /* 0x14 */ u8 pad14[12];
    /* 0x20 */ s32 tempoRaw;
    /* 0x24 */ s32 keyOnMask;
    /* 0x28 */ s32 loopCounter;
    /* 0x2C */ s32 cmdDataPtr;
    /* 0x30 */ s32 flags;
    /* 0x34 */ s32 bankPtr;
    /* 0x38 */ s32 keyOffMask;
    /* 0x3C */ s32 loopLimit;
    /* 0x40 */ s32 panRaw;
    /* 0x44 */ s32 panShifted;
    /* 0x48 */ s32 panTarget;
    /* 0x4C */ u8 pad4C[4];
    /* 0x50 */ s32 timing;
    /* 0x54 */ u8 pad54[4];
    /* 0x58 */ s32 pitchValue;
    /* 0x5C */ u16 field5C;
    /* 0x5E */ u16 field5E;
    /* 0x60 */ u16 voiceActive;
    /* 0x62 */ u16 volume;
    /* 0x64 */ u16 volumeBase;
    /* 0x66 */ u16 instrument;
    /* 0x68 */ u16 pad68;
    /* 0x6A */ u16 pitchModDepth;
    /* 0x6C */ u16 pitchModCounter;
    /* 0x6E */ u16 pad6E;
    /* 0x70 */ u8 pad70[16];
    /* 0x80 */ u16 pitchBend;
    /* 0x82 */ u8 pad82[2];
    /* 0x84 */ u16 pitchFadeCounter;
    /* 0x86 */ u16 panFade;
    /* 0x88 */ u16 panFadeTarget;
    /* 0x8A */ u16 exprFade;
    /* 0x8C */ u16 notePitch;
    /* 0x8E */ u16 notePitchFade;
    /* 0x90 */ u16 pad90;
    /* 0x92 */ u16 panpot;
    /* 0x94 */ u16 durationDelta;
    /* 0x96 */ u16 durationCounter;
    /* 0x98 */ u16 duration;
    /* 0x9A */ u16 timerActive;
    /* 0x9C */ u8 pad9C[14];
    /* 0xAA */ u16 delaySend;
    /* 0xAC */ u16 delaySendTarget;
    /* 0xAE */ u8 padAE[14];
    /* 0xBC */ u16 volumeLfo;
    /* 0xBE */ u16 volumeLfoTarget;
    /* 0xC0 */ u8 padC0[10];
    /* 0xCA */ u16 panLfo;
    /* 0xCC */ u16 panLfoTarget;
    /* 0xCE */ u8 padCE[2];
    /* 0xD0 */ u16 noiseDuration;
    /* 0xD2 */ u16 reverbDuration;
    /* 0xD4 */ u16 panEnvSpeed;
    /* 0xD6 */ u16 volumeDelta;
    /* 0xD8 */ u16 volumeAccum;
    /* 0xDA */ u8 padDA[4];
    /* 0xDE */ u16 expression;
    /* 0xE0 */ u8 padE0[4];
    /* 0xE4 */ u16 detune;
    /* 0xE6 */ u16 detuneTarget;
    /* 0xE8 */ u8 padE8[2];
    /* 0xEA */ u16 durationDelta2;
    /* 0xEC */ u16 ecField;
    /* 0xEE */ u16 portamento;
    /* 0xF0 */ u16 volumeLfoStop;
    /* 0xF2 */ u16 panLfoStop;
    /* 0xF4 */ s32 voiceMask;
    /* 0xF8 */ s32 updateFlags;
    /* 0xFC */ s32 sampleAddr;
    /* 0x100 */ s32 sampleLoop;
    /* 0x104 */ u16 pad104;
    /* 0x106 */ u16 adsrLow;
    /* 0x108 */ u16 adsrHigh;
    /* 0x10A */ u16 instOverride;
    /* 0x10C */ u8 pad10C[4];
} SoundSeqTrack; /* 0x110 = 272 bytes */

/**
 * @brief Instrument/sample descriptor (D_80073E68, stride 16 bytes).
 *
 * Each entry describes one SPU instrument with sample address,
 * loop point, and ADSR envelope parameters.
 */
typedef struct {
    /* 0x00 */ s32 sampleAddr;    /**< SPU sample start address. */
    /* 0x04 */ s32 loopAddr;      /**< SPU sample loop address. */
    /* 0x08 */ u8 pad08[4];       /**< Unknown. */
    /* 0x0C */ u16 adsrLow;       /**< ADSR envelope low halfword. */
    /* 0x0E */ u16 adsrHigh;      /**< ADSR envelope high halfword. */
} SndInstrument; /* 16 bytes */

/** @brief Voice pool entry (D_80074F20, stride 16 bytes). */
typedef struct {
    /* 0x00 */ s32 field0;
    /* 0x04 */ u8 pad04[12];
} VoicePoolEntry; /* 16 bytes */

/**
 * @brief SPU voice parameter block passed to func_800150A8.
 *
 * Contains the pending SPU register values for a single voice.
 * Field updateFlags holds a bitmask indicating which registers need writing.
 */
typedef struct {
    /* 0x00 */ u8 pad00[4];       /**< Unknown / unused. */
    /* 0x04 */ s32 updateFlags;   /**< Bitmask of pending SPU register updates. */
    /* 0x08 */ s32 startAddr;     /**< SPU sample start address. */
    /* 0x0C */ s32 repeatAddr;    /**< SPU sample repeat/loop address. */
    /* 0x10 */ u16 sampleRate;    /**< Voice pitch / sample rate. */
    /* 0x12 */ u16 adsrLow;       /**< ADSR envelope low halfword. */
    /* 0x14 */ u16 adsrHigh;      /**< ADSR envelope high halfword. */
    /* 0x16 */ u16 sweep;         /**< Volume sweep / scale value. */
    /* 0x18 */ s16 volLeft;       /**< Left channel volume. */
    /* 0x1A */ s16 volRight;      /**< Right channel volume. */
} SndVoiceParam; /* 0x1C = 28 bytes */

/**
 * @brief Sound voice/command structure used by CD sound functions.
 */
typedef struct {
    /* 0x00 */ u8 pad00[0x0C];
    /* 0x0C */ s32 playCount;          /**< Play/repeat count. */
    /* 0x10 */ u8 pad10[0x10];
    /* 0x20 */ s32 dataAddr;
    /* 0x24 */ u8 cmdData[4];        /**< Inline command data buffer. */
    /* 0x28 */ s32 voiceDataPtr;
    /* 0x2C */ s32 cmdDataPtr;
    /* 0x30 */ u8 pad30[4];
    /* 0x34 */ u8 paramByte;
    /* 0x35 */ u8 pad35;
    /* 0x36 */ u8 cmdSize;
    /* 0x37 */ u8 cmdType;
    /* 0x38 */ u8 cmdTypePrev;       /**< Archived (previous) command type. */
    /* 0x39 */ u8 pad39[0x0D];
    /* 0x46 */ u8 actionType;
    /* 0x47 */ u8 actionParam;
    /* 0x48 */ u8 actionMode;
    /* 0x49 */ u8 pad49[8];
    /* 0x51 */ u8 waveData;
    /* 0x52 */ u8 pad52;
    /* 0x53 */ u8 completionFlag;
    /* 0x54 */ u8 pad54[9];
    /* 0x5D */ u8 transferData;
    /* 0x5E */ u8 pad5E[0x85];
    /* 0xE3 */ u8 seqPartCount;
    /* 0xE4 */ u8 program;
    /* 0xE5 */ u8 padE5[4];
    /* 0xE9 */ u8 seqEntryCount;
    /* 0xEA */ u8 padEA[2];
    /* 0xEC */ u16 seqDataSize;
} SndVoice;

/**
 * @brief Sound bank descriptor header (passed to sndValidateBank / sndUploadSample).
 *
 * The first 0x40 bytes form the header; sample data follows immediately after.
 */
typedef struct {
    /* 0x00 */ u32 magic;           /**< Validation magic (checked by sndValidateBank). */
    /* 0x04 */ u8 pad04[0x0C];     /**< Unknown header fields. */
    /* 0x10 */ s32 spuAddr;         /**< SPU sample start address / transfer size. */
    /* 0x14 */ u8 pad14[4];        /**< Unknown. */
    /* 0x18 */ s32 spuSize;         /**< SPU transfer size in bytes. */
    /* 0x1C */ u8 pad1C[4];        /**< Unknown. */
    /* 0x20 */ s32 spuLoadedAddr;   /**< SPU address where sample was uploaded (written after transfer). */
    /* 0x24 */ u8 pad24[0x1C];     /**< Remaining header padding (total header = 0x40 bytes). */
} SndBankDesc; /* 0x40 bytes */

/**
 * @brief CD audio / streaming sound state (D_80077298).
 *
 * Controls SPU streaming playback, tracking voice indices, pitch,
 * volume/pan, loop counters and IRQ state.
 */
typedef struct {
    /* 0x00 */ s32 spuBaseAddr;    /**< SPU base address for stream. */
    /* 0x04 */ s32 unk04;
    /* 0x08 */ s32 unk08;
    /* 0x0C */ s32 active;         /**< Active/IRQ voice bitmask (nonzero = playing). */
    /* 0x10 */ s32 voiceIdx;       /**< First SPU voice index for this stream. */
    /* 0x14 */ u8 pad14[0x0C];
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 tickCounter;    /**< Tick counter (incremented each frame). */
    /* 0x28 */ s32 tickCount;      /**< Running tick accumulator. */
    /* 0x2C */ u8 pad2C[0x08];
    /* 0x34 */ s32 unk34;          /**< Set to -1 during init. */
    /* 0x38 */ s32 unk38;
    /* 0x3C */ s32 loopLimit;      /**< Loop/sector limit (sectorCount >> 12). */
    /* 0x40 */ s32 panVolume;      /**< Raw pan/volume value. */
    /* 0x44 */ u8 pad44[0x04];
    /* 0x48 */ s32 panTarget;      /**< Pan target (cleared when setting pan). */
    /* 0x4C */ u8 pad4C[0x0C];
    /* 0x58 */ s32 savedPitch;     /**< Saved pitch value for voice restore. */
    /* 0x5C */ u8 pad5C[0x04];
} SoundStream; /* 0x60 = 96 bytes */

/**
 * @brief Sound sequence table entry (stride 20 bytes).
 *
 * Used by world_engine's sound dispatch wrappers to look up a sequence
 * by index and feed the result to @c sndSeqStartPan and friends.
 */
typedef struct {
    /* 0x00 */ u8 pad00[4];
    /* 0x04 */ s32 field04;     /**< Sequence handle / ID. */
    /* 0x08 */ u8 pad08[8];
    /* 0x10 */ s16 field10;     /**< Sequence track / priority. */
    /* 0x12 */ s16 field12;     /**< Sequence runtime state (cleared by stop helper). */
} SeqEntry; /* 0x14 = 20 bytes */

/**
 * @brief SFX slot entry (stride 16 bytes).
 *
 * Used by world_engine's SFX slot table to track active SFX with their
 * associated signed index values.
 */
typedef struct {
    /* 0x00 */ s8 field00;      /**< Slot enable flag; -1 = inactive. */
    /* 0x01 */ u8 pad01;
    /* 0x02 */ s8 field02;      /**< Signed SFX index. */
    /* 0x03 */ u8 pad03[13];
} SfxSlot; /* 0x10 = 16 bytes */

#endif /* SOUND_H */
