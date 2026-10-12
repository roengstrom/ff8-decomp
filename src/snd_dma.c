#include "common.h"
#include "sound.h"
#include "snd_dma.h"
#include "snd_init.h"

extern volatile s32 D_80074ED4;
extern s32 D_80074E88;
extern u8 D_800516B8[];
extern s32 D_8005169C;

void spuSetIrqAddr(u32 val);

void func_800146F0(s32 *src, s32 *dst, s32 base, s32 count) {
    do {
        dst[0] = src[0] + base;
        dst[1] = src[1] + base;
        dst[2] = src[2];
        dst[3] = src[3];
        src += 4;
        dst += 4;
        count--;
    } while (count != 0);
}

/** @brief Adds magic constant 0xB0BEB4BF to value at pointer. */
u32 sndValidateBank(u32 *a0) {
    return *a0 + 0xB0BEB4BF;
}

/** @brief Calls SpuSetTransferCallback(0) and clears D_80074ED4. */
void sndDmaCompleteCallback(void) {
    SpuSetTransferCallback(0);
    D_80074ED4 = 0;
}

/**
 * @brief Sets the SPU DMA active flag and registers the completion callback.
 *
 * Sets D_80074ED4 to 1 (DMA active), then registers sndDmaCompleteCallback as
 * the transfer completion callback via SpuSetTransferCallback.
 */
/**
 * @brief Sets the SPU DMA active flag and registers the completion callback.
 *
 * Sets D_80074ED4 to 1 (DMA active), then registers sndDmaCompleteCallback as
 * the transfer completion callback via SpuSetTransferCallback.
 */
void sndDmaSetActive(void) {
    D_80074ED4 = 1;
    SpuSetTransferCallback((s32)sndDmaCompleteCallback);
}

/**
 * @brief Sets the SPU DMA active flag, registers callback, and initiates SPU write.
 *
 * Sets D_80074ED4 to 1, registers sndDmaCompleteCallback as the completion callback,
 * then calls func_8003E3A4 to begin the SPU write with the given address and size.
 *
 * @param a0 SPU destination address.
 * @param a1 Transfer size in bytes.
 */
void sndDmaWriteSpu(s32 a0, s32 a1) {
    D_80074ED4 = 1;
    SpuSetTransferCallback((s32)sndDmaCompleteCallback);
    func_8003E3A4(a0, a1);
}

/** @brief Calls sndDmaSetActive, then SpuRead with a0/a1 as arguments.
 *  @param a0 SPU read address.
 *  @param a1 SPU read size.
 */
void sndDmaReadSpu(s32 a0, s32 a1) {
    sndDmaSetActive();
    SpuRead(a0, a1);
}

/**
 * @brief Busy-waits until SPU DMA transfer completes.
 *
 * Spins on volatile flag D_80074ED4 until it is no longer 1, indicating
 * the active SPU transfer has finished.
 */
void sndDmaWait(void) {
    while (D_80074ED4 == 1) {}
}

/**
 * @brief Initiates a single SPU sample upload step.
 *
 * Validates the sound bank descriptor, then reads the sample's transfer
 * size and SPU start address to call func_800148B0, which performs the
 * actual DMA transfer.
 *
 * @param a0 Pointer to a sound bank descriptor.
 * @param a1 Transfer mode or bank index.
 * @return 0 on success, -1 if validation fails.
 */
s32 sndUploadSample(SndBankDesc *a0, s32 a1) {
    if (sndValidateBank((u32 *)a0) != 0) {
        return -1;
    }
    func_800148B0((s32)a0, a1, a0->spuSize, a0->spuAddr);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/snd_dma", func_800148B0);

void func_80014974(void) {
    SoundSeqTrack *t;
    u16 i;
    u16 *spu;

    D_80074ED4 = 0;
    g_sndSeqState = &D_80073D70;
    D_80073CA8 = NULL;
    D_80074EE0 = D_80070D60;
    t = D_80074EE0;
    D_80073C34 = NULL;
    D_80073DE0[1] = 0;
    D_80073DE0[0] = 0;
    D_80074EB8[5] = 0;
    D_80074EB8[4] = 0;
    D_80074EB8[3] = 0;
    D_80074EB8[2] = 0;
    D_80074EB8[1] = 0;
    D_80077288[0] = 0;
    D_80074EB8[0] = 0;
    D_80077288[1] = 1;
    D_80075028.activeMask = 0;
    g_sndSeqState->activeMask = 0;
    g_sndSeqState->unk08 = 0;
    g_sndSeqState->musicId = 0;
    D_80075028.activeMaskStored = 0;
    g_sndSeqState->activeMaskStored = 0;
    D_80073DF0.musicId = 0;
    D_80073DF0.activeMask = 0;
    g_sndSeqState->volMul = 0x7F0000;
    D_80075078 = 0x7FFF0000;
    D_80073E60 = 0;
    D_80077280 = 0;
    D_80073E62 = 0;
    D_80077284 = 0;
    g_sndSeqState->volMulSlideSteps = 0;
    D_80074FE4 = 0;
    D_80075028.noiseMask = 0;
    g_sndSeqState->noiseMask = 0;
    D_80075028.reverbMask = 0;
    g_sndSeqState->reverbMask = 0;
    spu = (u16 *)0x1F801D80;
    *spu = 0x3FFF;
    spu = (u16 *)0x1F801D82;
    *spu = 0x3FFF;
    spu = (u16 *)0x1F801DB0;
    *spu = 0x7FFF;
    spu = (u16 *)0x1F801DB2;
    *spu = 0x7FFF;
    spu = (u16 *)0x1F801DAA;
    i = *spu;
    D_80074EB0 = 0;
    D_8007507C = 0;
    D_800772F4 = 0;
    D_80075028.pitchLfoMask = 0;
    g_sndSeqState->pitchLfoMask = 0;
    g_sndSeqState->timerLower = 0;
    g_sndSeqState->timerUpperCur = 0;
    g_sndSeqState->timerUpper = 0;
    g_sndSeqState->timerTopCur = 0;
    g_sndStream.panVolume = 0x7F00;
    g_sndStream.panTarget = 0;
    D_80077368[2] = 0;
    D_80077368[1] = 0;
    D_80077368[0] = 0;
    D_80074F18 = 0;
    *spu = (i & 0xFFFA) | 1;

    for (i = 0; i < 32; i++, t++) {
        t->flags = 0;
        t->voiceAttr.voice = 0x18;
        t->playingType = 0;
        t->unk54 = 0;
    }

    for (t = &D_80072F70[0], i = 12; i < 24; i++, t++) {
        t->flags = 0;
        t->voiceAttr.voice = i;
        t->playingType = 1;
        t->unk54 = 0;
        t->volBalance = 0x7F00;
        t->volBalanceSlideSteps = 0;
        t->pitchMulSoundSlideSteps = 0;
        t->pitchMulSound = 0;
        t->pitchValueSlideSteps = 0;
    }

    g_sndSeqState->unk18 = 0;
    g_sndSeqState->unk14 = 0;
    g_sndSeqState->unk10 = 0;
    D_80075028.tempoUpdate = 1;
    D_80075028.tempo = 0x66A80000;
    D_80075028.unk0C = 0;
    D_80075028.unk08 = 0;
    D_80075028.unk04 = 0;
    g_sndSeqState->reverbDepth = 0x3FFF000;
    g_sndSeqState->reverbDepthSlideStep = 0;
    g_sndSeqState->reverbDepthSlideSteps = 0;
    D_80077288[2] |= 0x80;
    sndSetReverbMode(4);
    SpuSetReverb(1);
    D_80072F60[1] = 0;
    D_80072F60[0] = 0;
    D_80072F68 = 0;
}

/**
 * @brief Initializes the SPU hardware and sets up the sound engine timer.
 *
 * Performs the full SPU startup sequence:
 *  1. Calls SpuStart() to initialize the SPU subsystem.
 *  2. Configures SPU malloc with 4 allocation slots.
 *  3. Sets transfer mode to manual and start address to 0x1010.
 *  4. Sends an initial command buffer and waits for DMA completion.
 *  5. Resets voice state via func_80014974.
 *  6. Disables SPU IRQ.
 *  7. Sets up root counter 2 (RCnt2, 0xF2000002) as a timer at interval 0x44E8.
 *  8. Opens and enables an event on the root counter for the sound engine tick
 *     callback (D_8001AD60).
 */
void sndSpuInit(void) {
    SpuStart();
    SpuInitMalloc(4, (s32)&D_80074E88);
    SpuSetTransferMode(0);
    SpuSetTransferStartAddr(0x1010);
    sndDmaWriteSpu((s32)D_800516B8, 0x40);
    sndDmaWait();
    func_80014974();
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);

    do {
    } while (SetRCnt(0xF2000002, 0x44E8, 0x1000) == 0);

    do {
    } while (StartRCnt(0xF2000002) == 0);

    do {
        D_8005169C = OpenEvent(0xF2000002, 2, 0x1000, (s32)func_8001AD60);
    } while (D_8005169C == -1);

    do {
    } while (EnableEvent(D_8005169C) == 0);
}

/**
 * @brief Shuts down the sound engine and releases SPU resources.
 *
 * If playback is active (D_80074ED4 == 1), sends a stop command and waits
 * for DMA completion. Stops root counter 2, undelivers and disables/closes
 * the associated event, resets all voice IRQ addresses, and calls
 * func_8003DE44 (SpuQuit) for final SPU cleanup.
 */
void sndSpuShutdown(void) {
    if (D_80074ED4 == 1) {
        sndDmaWriteSpu((s32)D_800516B8, 0x40);
        sndDmaWait();
    }

    do {
    } while (StopRCnt(0xF2000002) == 0);

    UnDeliverEvent(0xF2000002, 2);

    do {
    } while (DisableEvent(D_8005169C) == 0);

    do {
    } while (CloseEvent(D_8005169C) == 0);

    spuSetIrqAddr(0xFFFFFF);
    func_8003DE44();
}

/**
 * @brief Writes the reverb work area start address to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D88 and high 16 bits to 0x1F801D8A.
 * (SPU register: Reverb Work Area Start Address)
 *
 * @param val 32-bit reverb work area start address.
 */
void spuSetReverbWorkAddr(u32 val) {
    *(s16 *)0x1F801D88 = val;
    *(s16 *)0x1F801D8A = val >> 16;
}

/**
 * @brief Writes the SPU IRQ address to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D8C and high 16 bits to 0x1F801D8E.
 * (SPU register: Sound RAM IRQ Address)
 *
 * @param val 32-bit SPU IRQ address.
 */
void spuSetIrqAddr(u32 val) {
    *(s16 *)0x1F801D8C = val;
    *(s16 *)0x1F801D8E = val >> 16;
}

/**
 * @brief Writes the SPU transfer control value to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D98 and high 16 bits to 0x1F801D9A.
 * (SPU register: Sound RAM Data Transfer Control)
 *
 * @param val 32-bit transfer control value.
 */
void spuSetTransferControl(u32 val) {
    *(s16 *)0x1F801D98 = val;
    *(s16 *)0x1F801D9A = val >> 16;
}

/**
 * @brief Writes a value to the SPU data FIFO register.
 *
 * Writes low 16 bits to 0x1F801D94 and high 16 bits to 0x1F801D96.
 * (SPU register: Sound RAM Data Transfer FIFO)
 *
 * @param val 32-bit value to push into the SPU data FIFO.
 */
void spuSetDataFifo(u32 val) {
    *(s16 *)0x1F801D94 = val;
    *(s16 *)0x1F801D96 = val >> 16;
}

/**
 * @brief Writes the SPU transfer start address to SPU registers.
 *
 * Writes low 16 bits to 0x1F801D90 and high 16 bits to 0x1F801D92.
 * (SPU register: Sound RAM Data Transfer Address)
 *
 * @param val 32-bit SPU RAM transfer start address.
 */
void spuSetTransferAddr(u32 val) {
    *(s16 *)0x1F801D90 = val;
    *(s16 *)0x1F801D92 = val >> 16;
}

/**
 * @brief Sets the left and right volume for an SPU voice, with optional scaling.
 *
 * If @p scale is non-zero, both volumes are multiplied by @p scale and
 * divided by 128. The resulting volumes are masked to 15 bits and written
 * to the voice's volume registers at 0x1F801C00 + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param vol_l Left volume (0-0x7FFF).
 * @param vol_r Right volume (0-0x7FFF).
 * @param scale Volume scale factor (0 = no scaling, otherwise vol * scale / 128).
 */
void spuSetVoiceVolume(s32 voice, s32 vol_l, s32 vol_r, s32 scale) {
    s16 *p;
    if (scale != 0) {
        vol_l *= scale;
        vol_l = (u32)vol_l >> 7;
        vol_r *= scale;
        vol_r = (u32)vol_r >> 7;
    }
    p = (s16 *)(0x1F801C00 + voice * 16);
    p[0] = vol_l & 0x7FFF;
    p[1] = vol_r & 0x7FFF;
}

/**
 * @brief Sets the sample rate (pitch) for an SPU voice.
 *
 * Writes to voice register offset +0x04 at 0x1F801C04 + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param val Sample rate value (0x1000 = 44100 Hz).
 */
void spuSetVoicePitch(s32 voice, s32 val) {
    *(s16 *)(0x1F801C04 + voice * 16) = val;
}

/**
 * @brief Sets the ADPCM sample start address for an SPU voice.
 *
 * The address is right-shifted by 3 (divided by 8) before writing to
 * voice register offset +0x06 at 0x1F801C06 + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param val SPU RAM byte address of the ADPCM sample start.
 */
void spuSetVoiceStartAddr(s32 voice, u32 val) {
    *(s16 *)(0x1F801C06 + voice * 16) = val >> 3;
}

/**
 * @brief Sets the ADPCM sample loop/repeat address for an SPU voice.
 *
 * The address is right-shifted by 3 (divided by 8) before writing to
 * voice register offset +0x0E at 0x1F801C0E + voice * 16.
 *
 * @param voice SPU voice index (0-23).
 * @param val SPU RAM byte address of the ADPCM loop point.
 */
void spuSetVoiceRepeatAddr(s32 voice, u32 val) {
    *(s16 *)(0x1F801C0E + voice * 16) = val >> 3;
}

/**
 * @brief Sets the ADSR low halfword for an SPU voice.
 *
 * Writes the full 16-bit ADSR low value to voice register offset +0x08
 * at 0x1F801C08 + voice * 16. Contains Attack Mode, Attack Shift, Attack
 * Step, Decay Shift, and Sustain Level fields.
 *
 * @param voice SPU voice index (0-23).
 * @param val 16-bit ADSR low value.
 */
void spuSetVoiceAdsrLow(s32 voice, s32 val) {
    *(s16 *)(0x1F801C08 + voice * 16) = val;
}

/**
 * @brief Sets the ADSR high halfword for an SPU voice.
 *
 * Writes the full 16-bit ADSR high value to voice register offset +0x0A
 * at 0x1F801C0A + voice * 16. Contains Sustain and Release parameters.
 *
 * @param voice SPU voice index (0-23).
 * @param val 16-bit ADSR high value.
 */
void spuSetVoiceAdsrHigh(s32 voice, s32 val) {
    *(s16 *)(0x1F801C0A + voice * 16) = val;
}

/**
 * @brief Sets the attack mode and attack shift fields in ADSR low for an SPU voice.
 *
 * Reads the current ADSR low register value, ORs in the attack mode bit
 * (bit 15, from @p a2 >> 2) and attack shift (bits [12:8], from @p a1),
 * then writes back.
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Attack shift value (written to bits [12:8]).
 * @param a2 Attack mode (bit 2 selects exponential mode in bit 15).
 */
void spuSetVoiceAttack(s32 voice, u32 a1, u32 a2) {
    u8 *addr = (u8 *)(0x1F801C08 + voice * 16);
    s32 val = *addr;
    val |= ((a2 >> 2) << 15) | (a1 << 8);
    *(s16 *)addr = val;
}

/**
 * @brief Sets the decay shift field in ADSR low for an SPU voice.
 *
 * Reads the current ADSR low register, clears bits [7:4], and writes
 * @p a1 into that field. (ADSR low register offset +0x08)
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Decay shift value (4 bits, written to bits [7:4]).
 */
void spuSetVoiceDecayShift(s32 voice, u32 a1) {
    s16 *addr = (s16 *)(0x1F801C08 + voice * 16);
    s32 val = *(u16 *)addr;
    val = (val & 0xFF0F) | (a1 << 4);
    *addr = val;
}

/**
 * @brief Sets the sustain level field in ADSR low for an SPU voice.
 *
 * Reads the current ADSR low register, clears bits [3:0], and writes
 * @p a1 into that field. (ADSR low register offset +0x08)
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Sustain level value (4 bits, written to bits [3:0]).
 */
void spuSetVoiceSustainLevel(s32 voice, u32 a1) {
    s16 *addr = (s16 *)(0x1F801C08 + voice * 16);
    s32 val = *(u16 *)addr;
    val = (val & 0xFFF0) | a1;
    *addr = val;
}

/**
 * @brief Sets the sustain mode/shift and direction fields in ADSR high for an SPU voice.
 *
 * Reads the current ADSR high register, preserves the low 6 bits (release),
 * and packs @p a2 (sustain direction, bit 14 via >>1) and @p a1 (sustain
 * shift, bits [12:6]) into the upper bits.
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Sustain shift value (written to bits [12:6]).
 * @param a2 Sustain direction (bit 1 selects decrease mode in bit 14).
 */
void spuSetVoiceSustainMode(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 1) << 14) | (a1 << 6);
    s32 val = *(u16 *)addr;
    val = (val & 0x3F) | packed;
    *addr = val;
}

/**
 * @brief Sets the release mode and release shift fields in ADSR high for an SPU voice.
 *
 * Reads the current ADSR high register, preserves the upper 10 bits
 * (sustain fields), and packs @p a2 (release mode, bit 5 via >>2) and
 * @p a1 (release shift, bits [4:0]) into the low 6 bits.
 *
 * @param voice SPU voice index (0-23).
 * @param a1 Release shift value (5 bits, written to bits [4:0]).
 * @param a2 Release mode (bit 2 selects exponential mode in bit 5).
 */
void spuSetVoiceReleaseMode(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 2) << 5) | a1;
    s32 val = *(u16 *)addr;
    val = (val & 0xFFC0) | packed;
    *addr = val;
}

