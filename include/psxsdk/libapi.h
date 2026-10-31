#ifndef LIBAPI_H
#define LIBAPI_H

#include "common.h"

/* --- libapi function declarations (signatures match PsyQ 4.6 LIBAPI.H) --- */

s32 OpenEvent(u32 desc, s32 spec, s32 mode, s32 (*func)());
s32 EnableEvent(s32 event);
s32 CloseEvent(s32 event);
s32 DisableEvent(s32 event);
s32 GetRCnt(u32 spec);
void SystemError(char type, s32 errorCode);

extern void SetMem(s32 a);

/** @brief Turn interrupts off (BIOS call). */
s32 EnterCriticalSection(void);
/** @brief Turn interrupts back on (BIOS call). */
void ExitCriticalSection(void);
/** @brief Read the CPU's status register. */
u32 GetSr(void);
#endif /* LIBAPI_H */
