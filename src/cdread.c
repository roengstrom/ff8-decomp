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

typedef void (*CdHandler)(void);
extern CdHandler D_800562D8[];

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


/**
 * @brief Dispatch the current CD read state handler.
 *
 * Looks up the handler for the current status in the D_800562D8 dispatch
 * table and calls it. Returns the (potentially updated) status byte.
 *
 * @return Current CD read status after handler execution.
 */
u8 func_800393C8(void) {
    D_800562D8[D_8008A3D8.status]();
    return D_8008A3D8.status;
}


