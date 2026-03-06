#include "common.h"

void func_80014DE0(u32 val);

INCLUDE_ASM("asm/nonmatchings/4EF0", func_800146F0);

/** @brief Adds magic constant 0xB0BEB4BF to value at pointer. */
u32 func_80014740(u32 *a0) {
    return *a0 + 0xB0BEB4BF;
}

/** @brief Calls func_8003E494(0) and clears D_80074ED4. */
void func_80014754(void) {
    extern s32 D_80074ED4;
    func_8003E494(0);
    D_80074ED4 = 0;
}

/**
 * @brief Sets the SPU DMA active flag and registers the completion callback.
 *
 * Sets D_80074ED4 to 1 (DMA active), then registers func_80014754 as
 * the transfer completion callback via func_8003E494.
 */
INCLUDE_ASM("asm/nonmatchings/4EF0", func_80014778);

INCLUDE_ASM("asm/nonmatchings/4EF0", func_800147A8);

/** @brief Calls func_80014778, then SpuRead with a0/a1 as arguments.
 *  @param a0 SPU read address.
 *  @param a1 SPU read size.
 */
void func_800147F8(s32 a0, s32 a1) {
    func_80014778();
    SpuRead(a0, a1);
}

/**
 * @brief Busy-waits until SPU DMA transfer completes.
 *
 * Spins on volatile flag D_80074ED4 until it is no longer 1, indicating
 * the active SPU transfer has finished.
 */
void func_80014834(void) {
    extern volatile s32 D_80074ED4;
    while (D_80074ED4 == 1) {}
}

/**
 * @brief Initiates a single SPU sample upload step.
 *
 * Validates @p a0 via func_80014740, then reads the sample's transfer
 * size (offset 0x18) and start address (offset 0x10) to call func_800148B0,
 * which performs the actual DMA transfer.
 *
 * @param a0 Pointer to a sound bank entry or sample descriptor.
 * @param a1 Transfer mode or bank index.
 * @return 0 on success, -1 if validation of @p a0 fails.
 */
s32 func_80014860(s32 a0, s32 a1) {
    if (func_80014740(a0) != 0) {
        return -1;
    }
    func_800148B0(a0, a1, *(s32 *)(a0 + 0x18), *(s32 *)(a0 + 0x10));
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/4EF0", func_800148B0);

INCLUDE_ASM("asm/nonmatchings/4EF0", func_80014974);

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
void func_80014C30(void) {
    extern s32 D_80074E88;
    extern u8 D_800516B8[];
    extern s32 D_8001AD60;
    extern s32 D_8005169C;

    SpuStart();
    SpuInitMalloc(4, (s32)&D_80074E88);
    SpuSetTransferMode(0);
    SpuSetTransferStartAddr(0x1010);
    func_800147A8((s32)D_800516B8, 0x40);
    func_80014834();
    func_80014974();
    SpuSetIRQ(0);
    SpuSetIRQCallback(0);

    do {
    } while (SetRCnt(0xF2000002, 0x44E8, 0x1000) == 0);

    do {
    } while (StartRCnt(0xF2000002) == 0);

    do {
        D_8005169C = OpenEvent(0xF2000002, 2, 0x1000, (s32)&D_8001AD60);
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
void func_80014D20(void) {
    extern s32 D_80074ED4;
    extern u8 D_800516B8[];
    extern s32 D_8005169C;

    if (D_80074ED4 == 1) {
        func_800147A8((s32)D_800516B8, 0x40);
        func_80014834();
    }

    do {
    } while (StopRCnt(0xF2000002) == 0);

    UnDeliverEvent(0xF2000002, 2);

    do {
    } while (DisableEvent(D_8005169C) == 0);

    do {
    } while (CloseEvent(D_8005169C) == 0);

    func_80014DE0(0xFFFFFF);
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
void func_80014DC4(u32 val) {
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
void func_80014DE0(u32 val) {
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
void func_80014DFC(u32 val) {
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
void func_80014E18(u32 val) {
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
void func_80014E34(u32 val) {
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
void func_80014E50(s32 voice, s32 vol_l, s32 vol_r, s32 scale) {
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
void func_80014E98(s32 voice, s32 val) {
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
void func_80014EB0(s32 voice, u32 val) {
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
void func_80014ECC(s32 voice, u32 val) {
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
void func_80014EE8(s32 voice, s32 val) {
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
void func_80014F00(s32 voice, s32 val) {
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
void func_80014F18(s32 voice, u32 a1, u32 a2) {
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
void func_80014F48(s32 voice, u32 a1) {
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
void func_80014F70(s32 voice, u32 a1) {
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
void func_80014F98(s32 voice, u32 a1, u32 a2) {
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
void func_80014FCC(s32 voice, u32 a1, u32 a2) {
    s16 *addr = (s16 *)(0x1F801C0A + voice * 16);
    u32 packed = ((a2 >> 2) << 5) | a1;
    s32 val = *(u16 *)addr;
    val = (val & 0xFFC0) | packed;
    *addr = val;
}

