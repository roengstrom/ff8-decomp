#include "common.h"

extern u8 D_8005D674;
extern u8 D_8005D680;
extern u8 D_8005D684;
extern u8 D_8005D685;

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdInit);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", EVENT_OBJ_74);

/**
 * @brief Delivers a CD-ROM "data ready" event.
 *
 * Signals the BIOS event system that CD data is ready by delivering
 * event class 0xF0000003 (HwCdRom) with spec 0x20 (EvSpACK).
 */
void EVENT_OBJ_84(void) {
    DeliverEvent(0xF0000003, 0x20);
}

/**
 * @brief Delivers a CD-ROM "complete" event.
 *
 * Signals the BIOS event system that a CD command has completed by delivering
 * event class 0xF0000003 (HwCdRom) with spec 0x40 (EvSpCOMP).
 */
void EVENT_OBJ_AC(void) {
    DeliverEvent(0xF0000003, 0x40);
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", EVENT_OBJ_D4);

/**
 * @brief Returns the current CD-ROM drive status.
 *
 * Reads the cached CD-ROM status byte. The status reflects the drive's
 * current state (e.g., standby, seeking, reading, playing).
 *
 * @return The CD-ROM status byte.
 */
u8 CdStatus(void) {
    return D_8005D674;
}

/**
 * @brief Returns the current CD-ROM mode setting.
 *
 * Reads the cached CD-ROM mode byte, which reflects the mode most recently
 * set by CdControl(CdlSetmode, ...). Includes flags for double speed,
 * XA-ADPCM, sector size, etc.
 *
 * @return The CD-ROM mode byte.
 */
u8 CdMode(void) {
    return D_8005D684;
}

/**
 * @brief Returns the last CD-ROM command issued.
 *
 * Reads the cached byte identifying the most recent CD-ROM command
 * sent via CdControl/CdControlF/CdControlB.
 *
 * @return The last CD-ROM command code.
 */
u8 CdLastCom(void) {
    return D_8005D685;
}

/**
 * @brief Returns a pointer to the last read CD-ROM position.
 *
 * Provides access to the internal CdlLOC structure containing the last
 * known position (minute/second/sector) reported by the CD-ROM drive.
 *
 * @return Pointer to the internal CdlLOC position structure.
 */
u8 *CdLastPos(void) {
    return &D_8005D680;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReset);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", SYS_OBJ_98);

/**
 * @brief Flushes the CD-ROM subsystem command queue.
 *
 * Clears any pending CD-ROM commands and resets the internal command state
 * by calling the low-level CD_flush routine.
 */
void CdFlush(void) { CD_flush(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdSetDebug);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdComstr);

/**
 * @brief Empty stub in the CD-ROM system object table.
 *
 * Placeholder function in the libcd internal dispatch table. No-op in this
 * SDK version.
 */
void SYS_OBJ_108(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdIntstr);

/**
 * @brief Empty stub in the CD-ROM system object table.
 *
 * Placeholder function in the libcd internal dispatch table. No-op in this
 * SDK version.
 */
void SYS_OBJ_13C(void) {
}

/**
 * @brief Waits for a previously issued CD-ROM command to complete.
 *
 * Blocks until the current CD-ROM command finishes execution by calling
 * the low-level CD_sync routine. Used for synchronous CD command flow.
 */
void CdSync(void) { CD_sync(); }

/**
 * @brief Waits for the CD-ROM data transfer to become ready.
 *
 * Blocks until the CD-ROM data buffer is ready for reading by calling
 * the low-level CD_ready routine.
 */
void CdReady(void) { CD_ready(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdSyncCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadyCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdControl);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdControlF);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdControlB);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", SYS_OBJ_538);

/**
 * @brief Sets the CD-ROM audio mixing volume.
 *
 * Applies the CD audio volume settings specified in the CdlATV structure
 * pointed to by a0, then returns success. Controls the CD-DA and XA-ADPCM
 * audio mixing levels for left/right channels.
 *
 * @param a0 Pointer to volume structure (CdlATV: volumes for L-to-L, L-to-R,
 *           R-to-R, R-to-L mixing).
 * @return Always returns 1 (success).
 */
s32 CdMix(s32 *a0) {
    CD_vol(a0);
    return 1;
}

/**
 * @brief Reads raw sector data from the CD-ROM data buffer.
 *
 * Transfers a1 words of sector data from the CD-ROM hardware buffer into
 * the memory location pointed to by a0. Uses DMA channel 3.
 *
 * @param a0 Pointer to the destination buffer for sector data.
 * @param a1 Number of 32-bit words to transfer.
 * @return 1 on success, 0 on failure.
 */
s32 CdGetSector(void *a0, s32 a1) {
    return CD_getsector(a0, a1) == 0;
}

/**
 * @brief Reads raw sector data from the CD-ROM data buffer (alternate method).
 *
 * Transfers a1 words of sector data from the CD-ROM hardware buffer into
 * the memory location pointed to by a0. Uses programmed I/O instead of DMA.
 *
 * @param a0 Pointer to the destination buffer for sector data.
 * @param a1 Number of 32-bit words to transfer.
 * @return 1 on success, 0 on failure.
 */
s32 CdGetSector2(void *a0, s32 a1) {
    return CD_getsector2(a0, a1) == 0;
}

/**
 * @brief Registers a callback for CD-ROM DMA data transfer completion.
 *
 * Sets a user-defined callback function to be invoked when a CD-ROM DMA
 * transfer (DMA channel 3) completes.
 *
 * @param a0 Pointer to the callback function, or 0 to disable.
 */
void CdDataCallback(a0) s32 a0; { DMACallback(3, a0); }

/**
 * @brief Waits for a CD-ROM data transfer to complete.
 *
 * Blocks until the current CD-ROM DMA data transfer finishes by calling
 * the low-level CD_datasync routine.
 */
void CdDataSync(void) { CD_datasync(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdIntToPos);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdPosToInt);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_25C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_35C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_3A8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_428);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_4A8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_548);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_sync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_694);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_6C4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_7AC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_ready);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_914);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_944);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_A74);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_cw);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_D70);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_DA0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_E88);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_vol);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_flush);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_initvol);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_initintr);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_init);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_1318);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_datasync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_1440);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_getsector);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_getsector2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CD_set_test_parmnum);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_1688);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", BIOS_OBJ_16BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdSearchFile);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_108);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_2A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_2B0);

/**
 * @brief Compares two ISO 9660 filename strings for equality.
 *
 * Performs a case-sensitive comparison of up to 12 characters, which is the
 * maximum length of an 8.3 filename on a PS1 CD-ROM (ISO 9660 Level 1).
 *
 * @param a0 Pointer to the first filename string.
 * @param a1 Pointer to the second filename string.
 * @return 1 if the filenames match (first 12 characters), 0 otherwise.
 */
s32 ISO9660_OBJ_2D8(char *a0, char *a1) {
    return strncmp(a0, a1, 12) == 0;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_2F8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_594);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_5BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_640);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_660);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_7D4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_7F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_8D4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", ISO9660_OBJ_8FC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", func_80045C54);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_9C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_158);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_260);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_32C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_504);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadBreak);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdRead);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_608);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_614);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CDREAD_OBJ_740);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdReadMode);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdDiskReady);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", TYPE_OBJ_108);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", CdGetDiskType);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", TYPE_OBJ_178);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libcd", TYPE_OBJ_250);
