#ifndef THREAD_H
#define THREAD_H

#include "common.h"

/* Public prototypes */
extern u32 getStatusRegister(void);
extern void setStatusRegister(u32 sr);
extern s32 openThreadSafe(void (*entry)(void), u8 *stack);
extern void closeThreadSafe(s32 thread);
extern void switchThread(s32 thread);
#endif /* THREAD_H */
