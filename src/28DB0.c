#include "common.h"
#include "psxsdk/libgpu.h"
#include "overlay.h"

INCLUDE_ASM("asm/nonmatchings/28DB0", func_800385B0);


/** @brief Calls func_8001313C with a0 passed through and fixed constants.
 *  @param a0 First parameter passed through.
 */
void func_8003863C(s32 a0) {
    func_8001313C(a0, 0, 0x80, 0x7F, 0);
}


/**
 * @brief Initialize the CD-ROM subsystem.
 *
 * Clears CD state variables (D_8008A3D8, D_8008A3D0). If a0 is nonzero,
 * spins until CdInit succeeds, disables CD debug output, sends a
 * CdControlB command (0xE = SetMode, double-speed 0x80), waits 3 vsyncs,
 * then reads the disc ID via func_800385B0 and stores it.
 *
 * @param a0 If nonzero, perform full CD hardware initialization.
 */
void func_80038668(s32 a0) {
    extern u8 D_8008A3D8;
    extern s16 D_8008A3D0;
    extern u8 D_8008A3DA;
    s32 result;
    u8 buf;

    D_8008A3D8 = 0;
    (&D_8008A3D8)[1] = 0;
    D_8008A3D0 = 0;

    if (a0 != 0) {
        do { } while (!CdInit());
        CdSetDebug(0);
        buf = 0x80;
        CdControlB(0xE, &buf, 0);
        VSync(3);
        result = func_800385B0();
        D_8008A3DA = result;
        func_80038CF0((s8)result);
    }
}


/** @brief Clears bit 1 in global D_8008A3D8. */
void func_800386F0(void) {
    extern u8 D_8008A3D8;
    D_8008A3D8 &= 0xFD;
}


/** @brief Sets bit 1 in global D_8008A3D8. */
void func_80038708(void) {
    extern u8 D_8008A3D8;
    D_8008A3D8 |= 0x02;
}


/**
 * @brief Flush CD subsystem and wait for completion.
 *
 * Calls the CD clear, flush, and init routines, then polls until
 * the CD subsystem reports ready.
 */
void func_80038720(void) {
    func_800386F0();
    CdFlush();
    func_800389CC();
    do {
    } while (func_800393C8() != 0);
}


INCLUDE_ASM("asm/nonmatchings/28DB0", func_80038760);


INCLUDE_ASM("asm/nonmatchings/28DB0", func_800387F8);


/**
 * @brief Initiate a CD-ROM read operation.
 *
 * Delegates to func_80038760 with mode=3, passing through all four
 * parameters (sector, count, destination, etc.).
 *
 * @param a0 First CD read parameter (e.g. sector number).
 * @param a1 Second CD read parameter (e.g. sector count).
 * @param a2 Third CD read parameter (e.g. destination address).
 * @param a3 Fourth CD read parameter.
 * @return Always 0.
 */
s32 func_8003882C(s32 a0, s32 a1, s32 a2, s32 a3) {
    func_80038760(3, a0, a1, a2, a3);
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/28DB0", func_80038868);


/**
 * @brief Perform a synchronous CD-ROM read with retry.
 *
 * Calls func_800387F8(a0, a1) in a busy-wait loop until it returns 0,
 * then waits for func_800393C8 to complete.
 *
 * @param a0 First CD read parameter.
 * @param a1 Second CD read parameter.
 * @return Always 0.
 */
s32 func_800388CC(s32 a0, s32 a1) {
    do {
    } while (func_800387F8(a0, a1) != 0);
    do {
    } while (func_800393C8() != 0);
    return 0;
}


/**
 * @brief Perform a synchronous (blocking) CD-ROM read.
 *
 * Initiates a CD read via func_8003882C, then busy-waits on func_800393C8
 * until the read is complete.
 *
 * @param a0 First CD read parameter.
 * @param a1 Second CD read parameter.
 * @param a2 Third CD read parameter.
 * @param a3 Fourth CD read parameter.
 * @return Always 0.
 */
s32 func_80038920(s32 a0, s32 a1, s32 a2, s32 a3) {
    do {
    } while (func_8003882C(a0, a1, a2, a3) != 0);
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
s32 func_80038994(void) {
    s32 saved = func_80038868();
    do {
    } while (func_800393C8() != 0);
    return saved;
}


/**
 * @brief Reset CD state unless already idle (0) or in final state (0xB).
 *
 * Checks state byte at D_8008A3D8+1. If it's 0 or 0xB, returns immediately.
 * Otherwise clears three control fields and sets state to 0xC, then calls
 * func_80039388 to finalize.
 */
void func_800389CC(void) {
    extern u8 D_8008A3D8;
    s32 base = (s32)&D_8008A3D8;
    u8 state = *(u8 *)(base + 1);

    if (state == 0xB || state == 0) return;

    *(s32 *)(base + 0x1C) = 0;
    *(s32 *)(base + 0x08) = 0;
    *(s32 *)(base + 0x20) = 0;
    *(u8 *)(base + 1) = 0xC;
    func_80039388();
}


INCLUDE_ASM("asm/nonmatchings/28DB0", func_80038A18);


INCLUDE_ASM("asm/nonmatchings/28DB0", func_80038A60);


extern s8 D_8008A3DA;
/**
 * @brief Get the current disc ID.
 * @return Disc identifier as a signed byte (D_8008A3DA).
 */
s8 func_80038CE0(void) {
    return D_8008A3DA;
}


extern u16 D_8008A3D2;
extern u8 D_8008A3D9;

/**
 * @brief Set the current disc number.
 * @param a0 Disc number to store in D_8008A3D2.
 */
void func_80038CF0(s32 a0) {
    D_8008A3D2 = a0;
}


/**
 * @brief Get the current CD-ROM subsystem state.
 * @return CD state machine phase (D_8008A3D9).
 */
u32 func_80038CFC(void) {
    return D_8008A3D9;
}


/**
 * @brief Stop the CD-ROM drive and flush pending commands.
 *
 * Calls CdFlush to clear the CD command queue, then issues CdControl(9)
 * which is the CdlPause command to halt disc spinning.
 */
void func_80038D0C(void) {
    CdFlush();
    CdControl(9, 0, 0);
}


