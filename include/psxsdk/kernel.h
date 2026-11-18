#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"

/* --- PS1 kernel structures (PsyQ KERNEL.H, simplified) --- */

typedef struct ToT  { unsigned long *head; long size; } ToT;
typedef struct TCBH { struct TCB *entry;   long flag; } TCBH;
typedef struct TCB  { s32 status; u8 pad04[0xBC]; } TCB;

struct DIRENTRY {
	char name[20];
	long attr;
	long size;
	struct DIRENTRY *next;
	long head;
	char system[4];
};

#define TcbStACTIVE 0x4000

#define DescRC 0xf2000000
#define RCntCNT2 (DescRC | 0x02)
#define KERNEL_TOT  ((ToT *)0x100)

#endif /* KERNEL_H */
