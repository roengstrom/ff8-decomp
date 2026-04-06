#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"
#include "sound.h"
#include "cd.h"

extern CdDriveState D_8008A3C8;
extern s32 D_8008A3B8;
extern CdReadState D_8008A3D8;
extern u8 D_8008A3DC[];
extern u8 *D_80039418;
extern u8 D_800853B8[];
extern s32 D_80056558;

/**
 * @brief Clear CD state and invoke completion callback.
 *
 * Resets the CD read state to idle, then calls the completion callback
 * if one is registered.
 */
void cdClearStatusAndCallback(void) {
    D_8008A3D8.status = 0;
    if (D_8008A3D8.callback) {
        D_8008A3D8.callback();
    }
}


/** @brief Empty stub (no-op), placeholder in CD-ROM subsystem. */
void cdStubNoop(void) {
}


/**
 * @brief Initiate a synchronous CD-ROM read command.
 *
 * Waits for the previous CD operation to finish. If ready, issues a
 * CdControl read command with the stored CD parameters, sets state
 * to 1, and invokes the completion callback.
 */
void cdStartSyncRead(void) {
    s32 result = CdSync(1, 0);

    if (result == 2) {
        CdControl(2, D_8008A3D8.params, 0);
        D_8008A3D8.status = 1;
        cdClearStatusAndCallback();
    }
}


/**
 * @brief Initiate an asynchronous CD-ROM read command.
 *
 * Waits for the previous CD operation to finish. If ready, issues an
 * async CdControlF read command with the stored CD parameters, sets
 * state to 4, and begins polling for completion.
 */
void cdStartAsyncRead(void) {
    s32 result = CdSync(1, 0);

    if (result == 2) {
        CdControlF(2, D_8008A3D8.params);
        D_8008A3D8.status = 4;
        cdPollReadState();
    }
}


/**
 * @brief Poll CD-ROM read state and handle completion or timeout.
 *
 * Checks CdSync(1, 0) for asynchronous read status:
 * - Case 2 (complete): resets timeout counter, advances to state 5,
 *   calls cdReadSectors for post-read processing.
 * - Case 5 (error/retry): increments timeout counter; if >= 0x708 (1800)
 *   frames, resets counter and plays an error sound via sndKeyOn.
 *   Calls VSync(0) and returns to state 3 to retry.
 */
void cdPollReadState(void) {
    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8.timeout = 0;
        D_8008A3D8.status = 5;
        cdReadSectors();
        break;
    case 5:
        D_8008A3C8.timeout++;
        if (D_8008A3C8.timeout >= 0x708) {
            D_8008A3C8.timeout = 0;
            sndKeyOn(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D8.status = 3;
        break;
    }
}


/**
 * @brief Issue a CD-ROM sector read and handle the result.
 *
 * Reads sectors using CdRead with parameters from D_8008A3D8. On success
 * (non-zero return), resets the timeout counter D_8008A3C8, sets state to 6,
 * and calls cdHandleReadSync. On failure, increments the timeout counter;
 * if it reaches 0x708 (1800 frames), resets it and plays an error sound.
 * Then waits for VSync, sets state to 3, flushes the CD, and issues
 * a CdControl pause command (type 9).
 */
void cdReadSectors(void) {
    if (CdRead(D_8008A3D8.sectorCount, D_8008A3D8.readBuffer, 0x80) == 0) {
        D_8008A3C8.timeout++;
        if (D_8008A3C8.timeout >= 0x708) {
            D_8008A3C8.timeout = 0;
            sndKeyOn(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D8.status = 3;
        CdFlush();
        CdControl(9, 0, 0);
    } else {
        D_8008A3C8.timeout = 0;
        D_8008A3D8.status = 6;
        cdHandleReadSync();
    }
}


/**
 * @brief Handle CD-ROM read synchronization result.
 *
 * Calls CdReadSync(1, 0) to check read completion status:
 * - On success (0): resets error counter, sets state to 1, calls cdClearStatusAndCallback.
 * - On error (-1): increments error counter, plays error sound if threshold
 *   (0x708) reached, waits for VSync, sets state to 3, flushes and re-seeks.
 * - Otherwise: returns without action (read still in progress).
 */
void cdHandleReadSync(void) {
    s32 result = CdReadSync(1, 0);
    if (result != -1) {
        if (result == 0) {
            D_8008A3C8.timeout = 0;
            D_8008A3D8.status = 1;
            cdClearStatusAndCallback();
        }
    } else {
        D_8008A3C8.timeout++;
        if (D_8008A3C8.timeout >= 0x708) {
            D_8008A3C8.timeout = 0;
            sndKeyOn(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D8.status = 3;
        CdFlush();
        CdControl(9, 0, 0);
    }
}


/**
 * @brief Initiate an asynchronous CD-ROM seek/read command.
 *
 * Calls CdSync(1, 0) to wait for the previous operation to finish.
 * If the result is 2 (ready), issues a CdControlF seek command (type 2)
 * with parameters from D_8008A3DC, sets state byte D_8008A3D9 to 8,
 * and calls cdPollSeekState to continue processing.
 */
void cdStartAsyncSeek(void) {
    s32 result = CdSync(1, 0);

    if (result == 2) {
        CdControlF(2, D_8008A3D8.params);
        D_8008A3D8.status = 8;
        cdPollSeekState();
    }
}


/**
 * @brief Poll CD-ROM seek/read state and handle completion or timeout.
 *
 * Similar to cdPollReadState but for a different CD operation phase:
 * - Case 2 (complete): resets timeout counter, advances to state 9,
 *   calls func_80039140 for post-seek processing.
 * - Case 5 (error/retry): increments timeout counter; if >= 0x708 (1800)
 *   frames, resets and plays error sound. Returns to state 7 to retry.
 */
void cdPollSeekState(void) {
    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8.timeout = 0;
        D_8008A3D8.status = 9;
        func_80039140();
        break;
    case 5:
        D_8008A3C8.timeout++;
        if (D_8008A3C8.timeout >= 0x708) {
            D_8008A3C8.timeout = 0;
            sndKeyOn(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D8.status = 7;
        break;
    }
}


/**
 * @brief Read sectors from CD with timeout and error handling.
 *
 * Reads up to 10 sectors from disc into D_800853B8. On failure, increments
 * the timeout counter; if it reaches 0x708 (1800), plays an error sound
 * and resets. On success, advances to func_80039218 for post-read processing.
 */
void func_80039140(void) {
    u32 sectors;
    CdDriveState *drive = &D_8008A3C8;

    sectors = D_8008A3D8.sectorCount;
    drive->readSectors = sectors;
    if (sectors >= 10) {
        drive->readSectors = 10;
    }

    if (CdRead(drive->readSectors, D_800853B8, 0x80) == 0) {
        drive->timeout++;
        if (drive->timeout >= 0x708) {
            drive->timeout = 0;
            sndKeyOn(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D8.status = 7;
        CdFlush();
        CdControl(9, 0, 0);
    } else {
        drive->timeout = 0;
        D_8008A3D8.status = 10;
        func_80039218();
    }
}


/**
 * @brief Handle post-read synchronization and advance to next chunk.
 *
 * Checks CdReadSync for the result of the previous CdRead:
 * - On success (0): updates the read buffer pointer and remaining sector
 *   count, then either completes (if no more data) or seeks to the next
 *   chunk position on disc.
 * - On error (-1): increments timeout, plays error sound if threshold
 *   reached, flushes and re-seeks.
 */
void func_80039218(void) {
    s32 result = CdReadSync(1, 0);

    switch (result) {
    case 0:
        D_8008A3C8.timeout = 0;
        D_80039418 = D_800853B8;
        D_8008A3D8.sectorCount -= 10;
        D_8008A3B8 += 10;

        if (func_8003947C() == 0) {
            D_8008A3D8.status = 1;
            cdClearStatusAndCallback();
        } else {
            CdIntToPos(D_8008A3B8, D_8008A3D8.params);
            D_8008A3D8.status = 7;
        }
        break;

    case -1:
        D_8008A3C8.timeout++;
        if (D_8008A3C8.timeout >= 0x708) {
            D_8008A3C8.timeout = 0;
            sndKeyOn(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        CdIntToPos(D_8008A3B8, D_8008A3D8.params);
        D_8008A3D8.status = 7;
        CdFlush();
        CdControl(9, 0, 0);
        break;
    }
}


/**
 * @brief Check CD drive status and handle stopped state.
 *
 * Issues CdControl(1) (CdlNop/GetStat), checks if bit 4 (shell open)
 * is set in the result. If not set, calls func_80038A60 to handle it.
 */
void cdCheckDriveStatus(void) {
    u8 buf[8];
    CdControl(1, 0, buf);
    if ((buf[0] & 0x10) == 0) {
        func_80038A60();
    }
}


/**
 * @brief Break (cancel) a CD-ROM read if it has completed.
 *
 * Polls CdSync(1, 0); if the result is 2 (complete), calls CdReadBreak
 * to abort the read and resets the CD state machine to state 0 (idle).
 */
void cdBreakRead(void) {
    if (CdSync(1, 0) == 2) {
        CdReadBreak();
        D_8008A3D8.status = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/cdread", func_800393C8); /* 0x50 */

INCLUDE_ASM("asm/nonmatchings/cdread", func_80039440); /* 0x4 */

INCLUDE_ASM("asm/nonmatchings/cdread", func_80039444); /* 0x38 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003947C); /* 0x54 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_800394D0); /* 0x50 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039520); /* 0x158 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039678); /* 0x68 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_800396E0); /* 0x48 */


/**
 * @brief Look up a table entry with 8-byte stride, return pointer and value.
 *
 * Given a base pointer and index, computes an entry at (base + idx*8 + 4),
 * reads two 32-bit fields: stores the second field to *out, and returns
 * base + first field as an absolute pointer.
 *
 * @param base Base pointer of the table
 * @param idx Index into the table (8-byte stride)
 * @param out Output pointer to receive the second field value
 * @return Absolute pointer computed as base + entry[0]
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_80039728); /* 0x24 */


/**
 * @brief Look up an absolute pointer from a relative offset table.
 *
 * Given a base address and an index, reads a 32-bit relative offset
 * from base[idx] and returns base + that offset as an absolute pointer.
 *
 * @param base Pointer to the start of the relative offset table
 * @param idx Index into the table
 * @return Absolute pointer computed as base + base[idx]
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_8003974C); /* 0x18 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039764); /* 0x118 */


void func_80039910(void);
s32 func_800398A8(void);

/**
 * @brief Initialize CD callback table with handler function pointers.
 *
 * Sets up a callback structure centered on D_8008A404:
 * - D_8008A404+0x00 = func_80039910 (primary handler)
 * - D_8008A404+0x04 = func_800398A8 (secondary handler)
 * - D_8008A404-0x04 = 0 (status cleared)
 * - D_8008A404+0x08 = 0 (status cleared)
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_8003987C); /* 0x2C */


/**
 * @brief Check if the CD subsystem is ready and call a callback if so.
 *
 * Checks bit 0 of D_800565B4[4] and D_800565B4[0]. If both are set,
 * calls the function pointer at D_8005657C (if non-NULL) and returns 1.
 * Otherwise returns 0.
 *
 * @return 1 if ready and callback invoked, 0 otherwise.
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_800398A8); /* 0x68 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039910); /* 0x190 */


/**
 * @brief Read and clear the CD status/result value.
 *
 * Returns the current value of D_800565BC and resets it to zero.
 *
 * @return The previous value of D_800565BC.
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_80039AA0); /* 0x14 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039AB4); /* 0xCC */


/**
 * @brief Disable the CD-ROM interrupt handler.
 *
 * Enters a critical section via func_800472E4, removes the CD interrupt
 * callback from RCnt(3) via ChangeClearRCnt and SysDeqIntRP, then exits
 * the critical section via func_800472F4.
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_80039B80); /* 0x44 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039BC4); /* 0x334 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039EF8); /* 0xF0 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_80039FE8); /* 0x210 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A1F8); /* 0x278 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A470); /* 0x90 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A500); /* 0x28 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A534); /* 0x48 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A57C); /* 0xD8 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A654); /* 0xAC */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A700); /* 0xC4 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003A7C4); /* 0x394 */


/** @brief Wrapper that calls func_80039AA0 (read and clear CD status). */
void cdReadStatusWrapper(void) { func_80039AA0(); }


/** @brief Wrapper that calls func_80039AB4 (initialize CD handler). */
void cdInitHandlerWrapper(void) { func_80039AB4(); }


/** @brief Wrapper that calls func_80039B80 (disable CD interrupt handler). */
void cdDisableInterruptWrapper(void) {
    func_80039B80();
}


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003ABC4); /* 0x4C */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003AC10); /* 0xCC */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003ACDC); /* 0xF8 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003ADD4); /* 0xD4 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003AEA8); /* 0xA8 */


/**
 * @brief Calls a function pointer from D_80056568, then passes result to func_8003B8E0.
 *
 * Loads a function pointer from D_80056568 and calls it with @p a0.
 * Passes the return value and @p a1 to func_8003B8E0.
 *
 * @param a0 Parameter passed to the indirect call.
 * @param a1 Second parameter passed through to func_8003B8E0.
 *
 * @note PsyQ 4.3 function (FILLED epilogue) — cannot be decomped from
 *       this file (compiled with PsyQ 4.1).
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_8003AF50); /* 0x38 */

/**
 * @brief Calls a function pointer from D_80056568, then passes result to func_8003BA2C.
 *
 * Loads a function pointer from D_80056568 and calls it with @p a0.
 * Passes the return value, (u8)a1, and (u8)a2 to func_8003BA2C.
 *
 * @param a0 Parameter passed to the indirect call.
 * @param a1 Note value (truncated to 8-bit).
 * @param a2 Velocity value (truncated to 8-bit).
 *
 * @note PsyQ 4.3 function (FILLED epilogue) — cannot be decomped from
 *       this file (compiled with PsyQ 4.1).
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_8003AF88); /* 0x48 */

/**
 * @brief Calls a function pointer from D_80056568, then passes result to sndSetVoiceData.
 *
 * Loads a function pointer from D_80056568 and calls it with @p a0.
 * Passes the return value, a1, and a2 to sndSetVoiceData.
 *
 * @param a0 Parameter passed to the indirect call.
 * @param a1 Data pointer value.
 * @param a2 Parameter byte value.
 *
 * @note PsyQ 4.3 function (FILLED epilogue) — cannot be decomped from
 *       this file (compiled with PsyQ 4.1).
 */
INCLUDE_ASM("asm/nonmatchings/cdread", func_8003AFD0); /* 0x48 */


/**
 * @brief Set a sound voice's data pointer and parameter byte.
 * @param a0 Pointer to the voice control structure.
 * @param a1 Value to store at offset +0x28 (data pointer/address).
 * @param a2 Value to store at offset +0x34 (parameter byte).
 */
void sndSetVoiceData(SndVoice *voice, s32 dataPtr, s32 param) {
    voice->voiceDataPtr = dataPtr;
    voice->paramByte = param;
}


/**
 * @brief Set a sound voice's command, data pointer, and payload size.
 * @param a0 Pointer to the voice control structure.
 * @param a1 Command byte to store at offset +0x37.
 * @param a2 Data pointer to store at offset +0x2C.
 * @param a3 Payload size byte to store at offset +0x36.
 */
void sndSetVoiceCommand(SndVoice *voice, s32 cmdType, s32 dataPtr, s32 cmdSize) {
    voice->cmdType = cmdType;
    voice->cmdDataPtr = dataPtr;
    voice->cmdSize = cmdSize;
}


/**
 * @brief Dispatch a sound voice action based on the command type at offset +0x46.
 *
 * Reads the command type from a0[0x46] and dispatches:
 * - 2: stop voice (sndVoiceCmdStop)
 * - 3: set program from a0[0xE4] (sndVoiceCmdSetProgram)
 * - 4: set volume from a0[0x47] (sndVoiceCmdSetVolume)
 *
 * @param a0 Pointer to the voice control structure.
 */
void sndDispatchVoiceAction(SndVoice *voice) {
    switch (voice->actionType) {
        case 2:
            sndVoiceCmdStop(voice);
            break;
        case 3:
            sndVoiceCmdSetProgram(voice, voice->program);
            break;
        case 4:
            sndVoiceCmdSetVolume(voice, voice->actionParam);
            break;
    }
}


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003B0C4); /* 0x270 */


/**
 * @brief Calculate the total data size for a sound voice's sequence data.
 *
 * Computes the size from three components at different offsets in the
 * voice structure: a0[0xE3] (part1), a0[0xE9] (part2), and the u16
 * at a0+0xEC (part3). The formula aligns parts to 4-byte boundaries.
 *
 * @param a0 Pointer to the voice control structure.
 * @return Total computed data size in bytes.
 */
s32 sndCalcSequenceSize(SndVoice *voice) {
    s32 v0 = voice->seqPartCount;
    s32 a1val = voice->seqEntryCount;
    u16 ecval = voice->seqDataSize;
    s32 part1 = ((v0 + 1) >> 1) << 2;
    s32 part2 = ((a1val * 5 + 3) & 0xFFC) + 4;
    return part1 + part2 + ecval;
}


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003B36C); /* 0xD4 */


/**
 * @brief Dispatch a CD-ROM command based on the command byte at a0[0x46].
 *
 * @param a0 Pointer to CD command structure.
 */
void sndDispatchCommand(SndVoice *voice) {
    u8 cmd = voice->actionType;
    switch (cmd) {
    case 2:
        sndVoiceCmdSetProgram(voice, voice->actionParam);
        break;
    case 3:
        sndVoiceCmdSetPitch(voice, voice->actionParam);
        break;
    case 4:
        if (voice->actionMode == 0) {
            sndVoiceCmdSetVolume(voice, voice->actionParam);
        } else {
            func_8003BC0C(voice);
        }
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003B4E8); /* 0x3F8 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003B8E0); /* 0x68 */


/** @brief Sets command byte 0x4D, state byte 6, and copies field 0x20 to 0x2C.
 *  @param a0 Pointer to entity structure.
 */
void sndSetTransferCommand(SndVoice *voice) {
    s32 val = voice->dataAddr;
    voice->cmdType = 0x4D;
    voice->cmdSize = 6;
    voice->cmdDataPtr = val;
}


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003B964); /* 0xC8 */


INCLUDE_ASM("asm/nonmatchings/cdread", func_8003BA2C); /* 0x98 */


// snd_voice_cmd_play_note - cmd=0x43, sets payload byte

/**
 * @brief Configure a CD command struct based on its command type.
 *
 * Reads byte 0x46 of the struct. If 2, sets field_37=0x44,
 * field_2C=a0+0x51, field_36=cmd. If 3, sets field_37=0x4D,
 * field_2C=a0+0x5D, field_36=6. Otherwise does nothing.
 *
 * @param a0 Pointer to the CD command struct.
 */
void sndConfigureCommand(SndVoice *voice) {
    s32 cmd = voice->actionType;
    switch (cmd) {
    case 2:
        voice->cmdType = 0x44;
        voice->cmdDataPtr = (s32)&voice->waveData;
        voice->cmdSize = cmd;
        break;
    case 3:
        voice->cmdType = 0x4D;
        voice->cmdDataPtr = (s32)&voice->transferData;
        voice->cmdSize = 6;
        break;
    }
}


/**
 * @brief Process a CD command completion state.
 *
 * If byte 0x53 of the command struct is zero, calls the function pointer
 * stored in D_80056558 and returns 0. If nonzero and byte 0x46 == 2,
 * returns 1 (command complete). Otherwise sets byte 0x46 to 0xFE
 * (error state) and returns 0.
 *
 * @param a0 Pointer to the CD command struct.
 * @return 1 if command complete, 0 otherwise.
 */
s32 sndCheckCompletion(SndVoice *voice) {
    if (voice->completionFlag != 0) {
        if (voice->actionType == 2) {
            return 1;
        }
        voice->actionType = 0xFE;
        return 0;
    }
    ((void (*)(void))D_80056558)();
    return 0;
}


