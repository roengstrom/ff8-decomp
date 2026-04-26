#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"
#include "cd.h"

extern CdReadState D_8008A3D8;
extern u8 D_8008A3DC[];
extern CdDriveState D_8008A3C8;
extern u8 D_8001092C[];
extern u8 D_8001093C[];
extern u8 D_8001094C[];
extern u8 D_8001095C[];
extern s16 D_8008A3D0;
extern s8 D_8008A3DA;
extern u16 D_8008A3D2;
extern u8 D_8008A3D9;
extern s32 D_8008A3B8;
extern u8 D_800853B8[];

s32 func_80039444(u8 *buf, u8 *dest);

void resetCdDrive(void);
void setDiscNumber(s32 a0);
s32 func_800393C8(void);
void CdIntToPos(s32 lba, u8 *pos);

/**
 * @brief Detect the disc number by searching for disc-specific files.
 *
 * Calls VSync(30), then tries CdSearchFile with four disc-identifier
 * filenames (D_8001092C–D_8001095C). Returns 1–4 for the first match,
 * or -1 if none found.
 *
 * @return Disc number (1–4), or -1 on failure.
 */
s32 detectDiscNumber(void) {
    u8 buf[24];
    VSync(30);
    if (CdSearchFile(buf, D_8001092C)) {
        return 1;
    }
    if (CdSearchFile(buf, D_8001093C)) {
        return 2;
    }
    if (CdSearchFile(buf, D_8001094C)) {
        return 3;
    }
    if (CdSearchFile(buf, D_8001095C)) {
        return 4;
    }
    return -1;
}


/** @brief Calls sndKeyOn with a0 passed through and fixed constants.
 *  @param a0 First parameter passed through.
 */
void setAudioVolume(s32 a0) {
    sndKeyOn(a0, 0, 0x80, 0x7F, 0);
}


/**
 * @brief Initialize the CD-ROM subsystem.
 *
 * Clears CD state variables (D_8008A3D8, D_8008A3D0). If a0 is nonzero,
 * spins until CdInit succeeds, disables CD debug output, sends a
 * CdControlB command (0xE = SetMode, double-speed 0x80), waits 3 vsyncs,
 * then reads the disc ID via detectDiscNumber and stores it.
 *
 * @param a0 If nonzero, perform full CD hardware initialization.
 */
void initCdSubsystem(s32 a0) {

    s32 result;
    u8 buf;

    D_8008A3D8.flags = 0;
    D_8008A3D8.status = 0;
    D_8008A3D0 = 0;

    if (a0 != 0) {
        do { } while (!CdInit());
        CdSetDebug(0);
        buf = 0x80;
        CdControlB(0xE, &buf, 0);
        VSync(3);
        result = detectDiscNumber();
        D_8008A3DA = result;
        setDiscNumber((s8)result);
    }
}


/** @brief Clears bit 1 in global D_8008A3D8. */
void clearCdBusyFlag(void) {

    D_8008A3D8.flags &= 0xFD;
}


/** @brief Sets bit 1 in global D_8008A3D8. */
void setCdBusyFlag(void) {

    D_8008A3D8.flags |= 0x02;
}


/**
 * @brief Flush CD subsystem and wait for completion.
 *
 * Calls the CD clear, flush, and init routines, then polls until
 * the CD subsystem reports ready.
 */
void flushCdAndWait(void) {
    clearCdBusyFlag();
    CdFlush();
    resetCdDrive();
    do {
    } while (func_800393C8() != 0);
}


/**
 * @brief Stage a CD-ROM read request into the global CD read state.
 *
 * Waits for any in-flight CD operation, converts the LBA to (m,s,sector)
 * directly into the CD parameter buffer, then populates the read state
 * with sector count, destination buffer, callback, and the requested
 * mode/state byte. The drive timeout is reset.
 *
 * @param mode     Mode/state byte to install (typically 3 for cdRead).
 * @param lba      Linear block address of the read.
 * @param size     Read size in bytes (rounded up to 2048-byte sectors).
 * @param dest     Destination buffer pointer (stored at readBuffer/0x1C).
 * @param callback Completion callback (stored at callback/0x20, may be NULL).
 */
void func_80038760(s32 mode, s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    CdReadState *req;

    while (func_800393C8()) {}

    CdIntToPos(lba, &D_8008A3DC[0]);
    req = (CdReadState *)(&D_8008A3DC[0] - 4);
    req->sectorCount = (size + 0x7FF) >> 11;
    req->readBuffer = dest;
    req->callback = callback;
    req->status = mode;
    D_8008A3C8.timeout = 0;
}


/**
 * @brief Stage a CD-ROM read request with mode 2 and a completion callback.
 *
 * Thin wrapper around func_80038760 that issues a mode-2 read with no
 * size, no destination buffer, and the supplied callback.
 *
 * @param lba      Linear block address of the read.
 * @param callback Completion callback (may be NULL).
 * @return Always 0.
 */
s32 func_800387F8(s32 lba, void (*callback)(void)) {
    func_80038760(2, lba, 0, 0, callback);
    return 0;
}


/**
 * @brief Initiate a CD-ROM read operation.
 *
 * Delegates to func_80038760 with mode=3, passing through all four
 * parameters (sector, count, destination, etc.).
 *
 * @param lba       Linear block address.
 * @param size      Read size in bytes.
 * @param dest      Destination buffer.
 * @param callback  Completion callback (may be NULL).
 * @return Always 0.
 */
s32 cdRead(s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    func_80038760(3, lba, size, dest, callback);
    return 0;
}


/**
 * @brief Stage a CD-ROM read request with mode 7, recording the LBA and
 *        priming a downstream handler.
 *
 * Issues a mode-7 read via func_80038760, then stores the LBA into
 * D_8008A3B8 (used by cdread's sector pump as the rolling read cursor),
 * and primes func_80039444 with the staging buffer D_800853B8 and the
 * caller-supplied destination.
 *
 * @param lba      Linear block address of the read.
 * @param size     Read size in bytes.
 * @param dest     Destination buffer.
 * @param callback Completion callback (may be NULL).
 */
s32 func_80038868(s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    func_80038760(7, lba, size, dest, callback);
    D_8008A3B8 = lba;
    return func_80039444(D_800853B8, dest);
}


/**
 * @brief Perform a synchronous CD-ROM read with retry.
 *
 * Calls func_800387F8(a0, a1) in a busy-wait loop until it returns 0,
 * then waits for func_800393C8 to complete.
 *
 * @param lba      Linear block address.
 * @param callback Completion callback (may be NULL).
 * @return Always 0.
 */
s32 cdReadRetry(s32 lba, void (*callback)(void)) {
    do {
    } while (func_800387F8(lba, callback) != 0);
    do {
    } while (func_800393C8() != 0);
    return 0;
}


/**
 * @brief Perform a synchronous (blocking) CD-ROM read.
 *
 * Initiates a CD read via cdRead, then busy-waits on func_800393C8
 * until the read is complete.
 *
 * @param a0 First CD read parameter.
 * @param a1 Second CD read parameter.
 * @param a2 Third CD read parameter.
 * @param a3 Fourth CD read parameter.
 * @return Always 0.
 */
s32 cdReadSync(s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    do {
    } while (cdRead(lba, size, dest, callback) != 0);
    do {
    } while (func_800393C8() != 0);
    return 0;
}


/**
 * @brief Perform a synchronous async-initiated CD-ROM read.
 *
 * Calls func_80038868 to start an async read, saves its return value,
 * then busy-waits on func_800393C8 until the read completes.
 *
 * @return The value returned by func_80038868 (async read handle/status).
 */
s32 cdReadAsyncSync(s32 lba, u32 size, u8 *dest, void (*callback)(void)) {
    s32 saved = func_80038868(lba, size, dest, callback);
    do {
    } while (func_800393C8() != 0);
    return saved;
}


/**
 * @brief Reset CD state unless already idle (0) or in final state (0xB).
 *
 * Checks CdReadState status field. If it's 0 or 0xB, returns immediately.
 * Otherwise clears readBuffer, sectorCount, callback and sets status to
 * 0xC (reset), then calls cdBreakRead to finalize.
 */
void resetCdDrive(void) {
    u8 state = D_8008A3D8.status;

    if (state == 0xB || state == 0) return;

    D_8008A3D8.readBuffer = 0;
    D_8008A3D8.sectorCount = 0;
    D_8008A3D8.callback = 0;
    D_8008A3D8.status = 0xC;
    cdBreakRead();
}


/**
 * @brief Reset CD drive mode and set state to complete.
 *
 * Sends CdControlB command 0xE (SetMode) with null parameters, waits
 * 3 VSync periods, sends command 0x8 (Pause), then marks the CD state
 * byte D_8008A3D9 as 0xB (complete).
 */
void resetCdDriveMode(void) {
    CdControlB(0xE, 0, 0);
    VSync(3);
    CdControlB(8, 0, 0);
    D_8008A3D9 = 0xB;
}


INCLUDE_ASM("asm/nonmatchings/cdrom", func_80038A60);


/**
 * @brief Get the current disc ID.
 * @return Disc identifier as a signed byte (D_8008A3DA).
 */
s8 getDiscId(void) {
    return D_8008A3DA;
}


/**
 * @brief Set the current disc number.
 * @param a0 Disc number to store in D_8008A3D2.
 */
void setDiscNumber(s32 a0) {
    D_8008A3D2 = a0;
}


/**
 * @brief Get the current CD-ROM subsystem state.
 * @return CD state machine phase (D_8008A3D9).
 */
u32 getCdState(void) {
    return D_8008A3D9;
}


/**
 * @brief Stop the CD-ROM drive and flush pending commands.
 *
 * Calls CdFlush to clear the CD command queue, then issues CdControl(9)
 * which is the CdlPause command to halt disc spinning.
 */
void stopCdDrive(void) {
    CdFlush();
    CdControl(9, 0, 0);
}


