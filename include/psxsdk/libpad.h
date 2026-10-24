#ifndef PSXSDK_LIBPAD_H
#define PSXSDK_LIBPAD_H

#include "common.h"

#define PadStateDiscon		0
#define PadStateFindPad		1
#define PadStateFindCTP1	2
#define PadStateFindCTP2	3
#define PadStateReqInfo		4
#define PadStateExecCmd		5
#define PadStateStable		6

#define InfoActFunc		1
#define InfoActSub		2
#define InfoActSize		3
#define InfoActCurr		4
#define InfoActSign		5

void PadInitDirect(unsigned char *, unsigned char *);
void PadStartCom(void);
void PadStopCom(void);
unsigned PadEnableCom(unsigned mode);
int PadGetState(int port);
int PadInfoMode(int port, int term, int offs);
int PadInfoAct(int port, int actno, int term);
int PadInfoComb(int port, int listno, int offs);
int PadSetActAlign(int, unsigned char *);
int PadSetMainMode(int socket, int offs, int lock);
void PadSetAct(int, unsigned char *, int);

s32 func_8003AB64(void);
void func_8003AB84(void);
void func_8003ABA4(void);

#endif
