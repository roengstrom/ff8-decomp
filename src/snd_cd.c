#include "common.h"
#include "sound.h"

extern s32 D_80056558;

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039444); /* 0x38 — LZSS init */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003947C); /* 0x54 — LZSS coroutine call */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_800394D0); /* 0x50 — LZSS coroutine yield */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039520); /* 0x158 — LZSS decompress */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039678); /* 0x68 — memcpy */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_800396E0); /* 0x48 — memset */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039728); /* 0x24 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003974C); /* 0x18 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039764); /* 0x118 */

void func_80039910(void);
s32 func_800398A8(void);

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003987C); /* 0x2C */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_800398A8); /* 0x68 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039910); /* 0x190 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039AA0); /* 0x14 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039AB4); /* 0xCC */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039B80); /* 0x44 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039BC4); /* 0x334 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039EF8); /* 0xF0 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_80039FE8); /* 0x210 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A1F8); /* 0x278 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A470); /* 0x90 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A500); /* 0x28 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A534); /* 0x48 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A57C); /* 0xD8 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A654); /* 0xAC */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A700); /* 0xC4 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003A7C4); /* 0x394 */

/** @brief Wrapper that calls func_80039AA0 (read and clear CD status). */
void cdReadStatusWrapper(void) { func_80039AA0(); }

/** @brief Wrapper that calls func_80039AB4 (initialize CD handler). */
void cdInitHandlerWrapper(void) { func_80039AB4(); }

/** @brief Wrapper that calls func_80039B80 (disable CD interrupt handler). */
void cdDisableInterruptWrapper(void) {
    func_80039B80();
}

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003ABC4); /* 0x4C */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003AC10); /* 0xCC */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003ACDC); /* 0xF8 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003ADD4); /* 0xD4 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003AEA8); /* 0xA8 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003AF50); /* 0x38 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003AF88); /* 0x48 */

INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003AFD0); /* 0x48 */

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


INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003B0C4); /* 0x270 */


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


INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003B36C); /* 0xD4 */


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


INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003B4E8); /* 0x3F8 */


INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003B8E0); /* 0x68 */


/** @brief Sets command byte 0x4D, state byte 6, and copies field 0x20 to 0x2C.
 *  @param a0 Pointer to entity structure.
 */
void sndSetTransferCommand(SndVoice *voice) {
    s32 val = voice->dataAddr;
    voice->cmdType = 0x4D;
    voice->cmdSize = 6;
    voice->cmdDataPtr = val;
}


INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003B964); /* 0xC8 */


INCLUDE_ASM("asm/nonmatchings/snd_cd", func_8003BA2C); /* 0x98 */


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


