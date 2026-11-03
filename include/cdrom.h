#ifndef CDROM_H
#define CDROM_H

#include "common.h"

// Public prototypes

void DecompressLzs(u8 *src, u8 *dst);
/** @brief Initialize the CD subsystem. */
void initCdSubsystem(s32 mode);
/** @brief Perform a synchronous (blocking) CD-ROM read. */
s32 cdReadSync(s32 lba, u32 size, u8 *dest, void (*callback)(void));
/** @brief Reset CD state unless already idle or in its final state. */
void resetCdDrive(void);
/** @brief Reset the CD drive mode (double-speed) and clear the state byte. */
void resetCdDriveMode(void);
/** @brief Verify the inserted disc and report a status code. */
s32 func_80038A60(void);
/** @brief Record the active disc number in the drive state. */
void setDiscNumber(s32 discNumber);

// Private prototypes

s32 detectDiscNumber(void);
void setAudioVolume(s32 voice);
void clearCdBusyFlag(void);
void setCdBusyFlag(void);
void flushCdAndWait(void);
void func_80038760(s32 mode, s32 lba, u32 size, u8 *dest, void (*callback)(void));
s32 func_800387F8(s32 lba, void (*callback)(void));
s32 cdReadRetry(s32 lba, void (*callback)(void));
u32 getCdState(void);
void stopCdDrive(void);

// Private data


#endif /* CDROM_H */
