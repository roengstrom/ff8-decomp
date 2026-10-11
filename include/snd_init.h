/**
 * @file snd_init.h
 * @brief Public interface for the sound engine driver (src/snd_init.c).
 *
 * Declares every function defined in snd_init.c: engine init/shutdown,
 * the @c sndCmd* SPU command dispatchers, sequence and SFX playback,
 * reverb/volume control, and the sample-bank upload/streaming path.
 * Shared sound types (@c SndBankDesc, @c SoundSeqTrack, @c DmaState, ...)
 * and the shared sound globals live in @c sound.h, which this header pulls in.
 */
#ifndef SND_INIT_H
#define SND_INIT_H

#include "sound.h"
#include "psxsdk/libcd.h"

/* ---- Data symbols referenced by snd_init.c ------------------------- *
 * Declared with snd_init.c's view of each global. Several of these are
 * shared sound-engine state that other snd_* modules access through their
 * own (sometimes differently-typed) declarations; they are kept here as
 * snd_init's interface to that state. */

extern s32  D_8005169C;
extern u8   D_800516B8[];
extern CdlATV g_cdMixVolume;
extern SndStreamHeader D_80073C38;
extern s32  D_80073C58;
extern s32 *D_80073CA8;
extern s32  D_80073DE0[];
extern s16  D_80073E60;
extern s16  D_80073E62;
extern SndInstrument D_80073E68[];
extern s32  D_80074968;
extern s32  D_80074EB8[];
extern s32  D_80074EBC;          /* bank-ID slot for mode 1 (D_80074EB8[1]) */
extern s32  D_80074EC0;          /* bank-ID slot for mode 2 (D_80074EB8[2]) */
extern s32  D_80074EC4;          /* func_80014190: bank-ID slot, default mode */
extern s32  D_80074EC8;          /* func_80014190: bank-ID slot, mode 1 */
extern s32  D_80074ECC;          /* func_80014190: bank-ID slot, mode 2 */
extern volatile s32 D_80074ED4;
extern s32  D_80074FE8[];        /* parsed bank header (level/addr/counts) */
extern s32  D_80074FF8;          /* decode work buffer / scratch pointer */
extern s32  g_sndCmdArgs[4];    /* sound-command argument buffer (filled per command, read by the dispatcher) */
extern s32  D_80077288[];
extern s32  D_8007728C;
extern s32  D_800772CC;
extern s32  D_8007735C;
extern s32  D_80077360;          /* active streaming handle (0 = idle) */

/* ---- Documented public entry points -------------------------------- */

/**
 * @brief Play a one-shot SFX.
 *
 * @param voiceMask Sound effect / voice id (rank-up = 0x5B..0x5D, Angelo learn = 0x83, etc.).
 * @param addr      Channel / mode flag (typically 0).
 * @param volume    Volume (typically 0x80).
 * @param pan       Pan (typically 0x7F).
 */
extern void sndPlaySfx(s32 voiceMask, s32 addr, s32 volume, s32 pan);

/** @brief Play one SFX from a specific bank with explicit volume and pan. */
extern s32 sndPlayBankSfx(s32 bank, s32 addr, s32 volume, s32 pan);

/** @brief Stop a sequence by track pair (bank, channel) packed pair. */
extern void sndCmd21(s32 arg0, s32 addr);

/** @brief Query the current active-channel mask for SFX dispatch. */
extern s32 sndGatherKeyOnMask(void);

/**
 * @brief SPU command-bus wrappers. Each writes its args into the
 *        command buffer at @c g_sndCmdArgs and tail-calls
 *        @c func_8001A1E8 to dispatch the indicated opcode. The
 *        SPU-handle / dispatch result is left in @c $v0 by the inner
 *        @c jal, which is why callers can treat them as returning
 *        @c s32 even though the body has no explicit @c return.
 */
extern s32 sndCmd10(s32 arg0);
extern s32 sndCmd11(s32 arg0);
extern s32 sndCmd12(s32 arg0, s32 arg1);
extern s32 sndCmd14(s32 arg0, s32 arg1, s32 arg2);
extern s32 sndCmd19(s32 arg0, s32 pan);
extern s32 sndCmd1A(s32 voice, s32 arg1, s32 pan);
extern s32 sndCmdC0(s32 channel, s32 value);
extern s32 sndCmdC1(s32 arg0, s32 arg1, s32 value);
extern s32 sndCmdC2(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

/** @brief Send SPU command @c 0x45 (no args). */
extern void sndCmd45(void);

/** @brief Reset SPU command bus (issued by the music-state reset opcode). */
extern void sndCmdF1(void);

/** @brief Set the global SPU master volume (0..0x7F). */
extern void sndSetMasterVolume(s32 volume);

/** @brief Set the global SEQ tempo (0x80 = normal). */
extern void sndSeqSetTempo(s32 tempo);

/** @brief Upload a staged sample bank to the SPU; returns -1 while busy. */
extern s32 sndUploadSamples(s32 srcAddr, s32 mode);

/** @brief Query the sound engine's current busy state (0 = idle). */
extern s32 sndGetEngineState(void);

/* ---- Remaining engine / command / sequence functions --------------- */

extern s32 sndInit(void);
extern s32 sndShutdown(void);
extern s32 sndLoadBank(u8 *bank);
extern s32 sndStopAll(void);
extern s32 sndGetStatus(void);
extern s32 sndGetMaxVolume(s32 mask);
extern void sndCmd40(void);
extern s32 sndGetSeqPosition(void);
extern void sndKeyOn(s32 voiceMask);
extern void sndStopPlayback(void);
extern s32 sndFindKeyOnMask(s32 keyOnMask);
extern void sndSelectMode(s32 mode);
extern void sndCmd90(s32 arg0);
extern void sndCmd92(s32 arg0);
extern void sndEnableReverb(u32 mode);
extern void sndDisableReverb(u32 mode);
extern s32 func_800133D8(s32 arg0);
extern s32 sndSetEngineFlag(s32 enable);
extern void sndSetChannelVolume(s32 channel, s32 volume);
extern void sndSeqPlay7bit(s32 seqId, s32 addr, s32 volume);
extern void sndSeqPlayPan7bit(s32 seqId, s32 addr, s32 volume, s32 pan);
extern void sndSeqSetChannelTempo(s32 channel, s32 tempo);
extern void sndSeqPlay8bit(s32 seqId, s32 addr, s32 volume);
extern void sndSeqPlayPan8bit(s32 seqId, s32 addr, s32 volume, s32 pan);
extern void sndSeqSetTempoAlt(s32 tempo);
extern void sndSeqSetChannelTempoAlt(s32 channel, s32 tempo);
extern void sndSeqStart(s32 seqId, s32 addr, s32 volume);
extern void sndSeqStartPan(s32 seqId, s32 addr, s32 volume, s32 pan);
extern void sndCmdC8(s32 arg0);
extern void sndCmdC9(s32 arg0, s32 arg1);
extern void sndCmdCA(s32 arg0, s32 arg1, s32 arg2);
extern void sndCmdD0(s32 arg0);
extern void sndCmdD1(s32 arg0, s32 arg1);
extern void sndCmdD2(s32 arg0, s32 arg1, s32 arg2);
extern void sndCmdD4(s32 arg0);
extern void sndCmdD5(s32 arg0, s32 arg1);
extern void sndCmdD6(s32 arg0, s32 arg1, s32 arg2);
extern void sndCmdD8(s32 arg0);
extern void sndCmdD9(s32 arg0, s32 arg1);
extern void sndCmdDA(s32 arg0, s32 arg1, s32 arg2);
extern void sndCmdF0(void);
extern s32 sndSelectBankSlot(SndBankDesc *desc, s32 *outLevel, s32 *outAddr);
extern s32 sndResetState(void);
extern s32 sndStreamBank(s32 srcPtr, u32 numBytes, s32 waitDma);
extern s32 sndProcessAudio(s32 bank, s32 arg1);
extern s32 sndUploadBank(SndBankDesc *desc, s32 arg1);
extern void sndSetPlaybackAddr(s32 arg0, s32 arg1);
extern void func_80014094(SndBankDesc *desc, s32 mode, s32 arg2);
extern void func_80014190(SndBankDesc *desc, s32 mode, s32 arg2);
extern s32 sndSetCdMixVolume(s32 volume);
extern void sndCmdE0(s32 bank, s32 instrument, s32 arg2);
extern void sndCmdE2(void);
extern void sndCmdE4(s32 value);
extern void sndCmdE5(s32 arg0, s32 value);
extern void sndCmdE6(s32 value);
extern s32 sndUploadSampleBank(s32 bank, s32 slot);
extern void sndCmdED(s32 volume, s32 arg1);
extern void sndCmdEC(s32 bank, s32 instrument, s32 slot, s32 arg3);
extern s32 sndInitIrq(s32 arg0, s32 value);
extern s32 sndTickCounters(void);

#endif /* SND_INIT_H */
