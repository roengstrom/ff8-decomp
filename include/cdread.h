#ifndef CDREAD_H
#define CDREAD_H

#include "common.h"

// CD read/seek state-machine handlers (cdread.c), invoked directly and through
// the g_cdReadStateHandlers dispatch table before their definitions appear.

void cdPollReadState(void);
void cdReadSectors(void);
void cdHandleReadSync(void);
void cdPollSeekState(void);
void func_80039140(void);
void func_80039218(void);

#endif /* CDREAD_H */
