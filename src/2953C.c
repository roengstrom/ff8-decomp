#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"

/**
 * @brief Clear CD status flag and invoke pending callback if set.
 *
 * Clears the status byte at D_8008A3D8+1, then calls the function pointer
 * stored at D_8008A3D8+0x20 if it is non-NULL.
 */
void func_80038D3C(void) {
    extern u8 D_8008A3D8[];
    s32 base = (s32)D_8008A3D8;
    void (*callback)(void) = *(void (**)(void))(base + 0x20);
    *(u8 *)(base + 1) = 0;
    if (callback != 0) {
        callback();
    }
}


/** @brief Empty stub (no-op), placeholder in CD-ROM subsystem. */
void func_80038D74(void) {
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_80038D7C);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80038DD4);


/**
 * @brief Poll CD-ROM read state and handle completion or timeout.
 *
 * Checks CdSync(1, 0) for asynchronous read status:
 * - Case 2 (complete): resets timeout counter, advances to state 5,
 *   calls func_80038ED4 for post-read processing.
 * - Case 5 (error/retry): increments timeout counter; if >= 0x708 (1800)
 *   frames, resets counter and plays an error sound via func_8001313C.
 *   Calls VSync(0) and returns to state 3 to retry.
 */
void func_80038E28(void) {
    extern u32 D_8008A3C8;
    extern u8 D_8008A3D9;

    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8 = 0;
        D_8008A3D9 = 5;
        func_80038ED4();
        break;
    case 5:
        D_8008A3C8++;
        if (D_8008A3C8 >= 0x708) {
            D_8008A3C8 = 0;
            func_8001313C(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D9 = 3;
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_80038ED4);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80038F88);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039040);


/**
 * @brief Poll CD-ROM seek/read state and handle completion or timeout.
 *
 * Similar to func_80038E28 but for a different CD operation phase:
 * - Case 2 (complete): resets timeout counter, advances to state 9,
 *   calls func_80039140 for post-seek processing.
 * - Case 5 (error/retry): increments timeout counter; if >= 0x708 (1800)
 *   frames, resets and plays error sound. Returns to state 7 to retry.
 */
void func_80039094(void) {
    extern u32 D_8008A3C8;
    extern u8 D_8008A3D9;

    switch (CdSync(1, 0)) {
    case 2:
        D_8008A3C8 = 0;
        D_8008A3D9 = 9;
        func_80039140();
        break;
    case 5:
        D_8008A3C8++;
        if (D_8008A3C8 >= 0x708) {
            D_8008A3C8 = 0;
            func_8001313C(0x10, 0, 0x80, 0x7F, 0);
        }
        VSync(0);
        D_8008A3D9 = 7;
        break;
    }
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039140);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039218);


/**
 * @brief Check CD drive status and handle stopped state.
 *
 * Issues CdControl(1) (CdlNop/GetStat), checks if bit 4 (shell open)
 * is set in the result. If not set, calls func_80038A60 to handle it.
 */
void func_80039344(void) {
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
void func_80039388(void) {
    extern u8 D_8008A3D9;
    if (CdSync(1, 0) == 2) {
        CdReadBreak();
        D_8008A3D9 = 0;
    }
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_800393C8);

INCLUDE_ASM("asm/nonmatchings/2953C", func_80039440);

INCLUDE_ASM("asm/nonmatchings/2953C", func_80039444);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003947C);


INCLUDE_ASM("asm/nonmatchings/2953C", func_800394D0);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039520);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039678);


INCLUDE_ASM("asm/nonmatchings/2953C", func_800396E0);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039728);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003974C);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039764);


void func_80039910(void);
void func_800398A8(void);

/**
 * @brief Initialize CD callback table with handler function pointers.
 *
 * Sets up a callback structure centered on D_8008A404:
 * - D_8008A404+0x00 = func_80039910 (primary handler)
 * - D_8008A404+0x04 = func_800398A8 (secondary handler)
 * - D_8008A404-0x04 = 0 (status cleared)
 * - D_8008A404+0x08 = 0 (status cleared)
 *
 * @note Non-matching: scheduler interleaves lui for func_80039910 into
 *       D_8008A404 load delay, and folds %lo(D_8008A404) into sw immediate.
 */
INCLUDE_ASM("asm/nonmatchings/2953C", func_8003987C);


INCLUDE_ASM("asm/nonmatchings/2953C", func_800398A8);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039910);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039AA0);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039AB4);


/**
 * @brief Disable the CD-ROM interrupt handler.
 *
 * Enters a critical section, removes the CD interrupt callback from
 * RCnt(3) via ChangeClearRCnt and SysDeqIntRP, then exits the critical section.
 */
INCLUDE_ASM("asm/nonmatchings/2953C", func_80039B80);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039BC4);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039EF8);


INCLUDE_ASM("asm/nonmatchings/2953C", func_80039FE8);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A1F8);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A470);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A500);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A534);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A57C);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A654);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A700);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003A7C4);


/** @brief Wrapper that calls func_80039AA0. */
void func_8003AB64(void) { func_80039AA0(); }


/** @brief Wrapper that calls func_80039AB4. */
void func_8003AB84(void) { func_80039AB4(); }


/** @brief Calls func_80039B80. */
void func_8003ABA4(void) {
    func_80039B80();
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003ABC4);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003AC10);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003ACDC);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003ADD4);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003AEA8);


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
INCLUDE_ASM("asm/nonmatchings/2953C", func_8003AF50);

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
INCLUDE_ASM("asm/nonmatchings/2953C", func_8003AF88);

/**
 * @brief Calls a function pointer from D_80056568, then passes result to func_8003B024.
 *
 * Loads a function pointer from D_80056568 and calls it with @p a0.
 * Passes the return value, a1, and a2 to func_8003B024.
 *
 * @param a0 Parameter passed to the indirect call.
 * @param a1 Data pointer value.
 * @param a2 Parameter byte value.
 *
 * @note PsyQ 4.3 function (FILLED epilogue) — cannot be decomped from
 *       this file (compiled with PsyQ 4.1).
 */
INCLUDE_ASM("asm/nonmatchings/2953C", func_8003AFD0);


/**
 * @brief Set a sound voice's data pointer and parameter byte.
 * @param a0 Pointer to the voice control structure.
 * @param a1 Value to store at offset +0x28 (data pointer/address).
 * @param a2 Value to store at offset +0x34 (parameter byte).
 */
void func_8003B024(u8 *a0, s32 a1, s32 a2) {
    *(s32 *)(a0 + 0x28) = a1;
    a0[0x34] = a2;
}


/**
 * @brief Set a sound voice's command, data pointer, and payload size.
 * @param a0 Pointer to the voice control structure.
 * @param a1 Command byte to store at offset +0x37.
 * @param a2 Data pointer to store at offset +0x2C.
 * @param a3 Payload size byte to store at offset +0x36.
 */
void func_8003B030(u8 *a0, s32 a1, s32 a2, s32 a3) {
    a0[0x37] = a1;
    *(s32 *)(a0 + 0x2C) = a2;
    a0[0x36] = a3;
}


/**
 * @brief Dispatch a sound voice action based on the command type at offset +0x46.
 *
 * Reads the command type from a0[0x46] and dispatches:
 * - 2: stop voice (func_8003BB98)
 * - 3: set program from a0[0xE4] (func_8003BBAC)
 * - 4: set volume from a0[0x47] (func_8003BBEC)
 *
 * @param a0 Pointer to the voice control structure.
 */
void func_8003B040(u8 *a0) {
    switch (a0[0x46]) {
        case 2:
            func_8003BB98(a0);
            break;
        case 3:
            func_8003BBAC(a0, a0[0xE4]);
            break;
        case 4:
            func_8003BBEC(a0, a0[0x47]);
            break;
    }
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003B0C4);


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
s32 func_8003B334(u8 *a0) {
    s32 v0 = a0[0xE3];
    s32 a1val = a0[0xE9];
    u16 ecval = *(u16 *)(a0 + 0xEC);
    s32 part1 = ((v0 + 1) >> 1) << 2;
    s32 part2 = ((a1val * 5 + 3) & 0xFFC) + 4;
    return part1 + part2 + ecval;
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003B36C);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003B440);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003B4E8);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003B8E0);


/** @brief Sets command byte 0x4D, state byte 6, and copies field 0x20 to 0x2C.
 *  @param a0 Pointer to entity structure.
 */
void func_8003B948(u8 *a0) {
    s32 val = *(s32 *)(a0 + 0x20);
    *(u8 *)(a0 + 0x37) = 0x4D;
    *(u8 *)(a0 + 0x36) = 6;
    *(s32 *)(a0 + 0x2C) = val;
}


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003B964);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003BA2C);


// snd_voice_cmd_play_note - cmd=0x43, sets payload byte

INCLUDE_ASM("asm/nonmatchings/2953C", func_8003BAC4);


INCLUDE_ASM("asm/nonmatchings/2953C", func_8003BB18);


