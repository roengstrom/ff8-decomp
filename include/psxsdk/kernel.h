#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"
#include "psxsdk/r3000.h"
#include "psxsdk/asm.h"

struct ToT {
    u32 *head;
    s32 size;
};

struct TCBH {
    struct TCB *entry;
    s32 flag;
};

struct TCB {
    s32 status;
    s32 mode;
    u32 reg[NREGS];
    s32 system[6];
};

#define TcbStACTIVE 0x4000

#define DescRC 0xf2000000
#define RCntCNT2 (DescRC | 0x02)
#define KERNEL_TOT ((struct ToT *)0x100)

#endif /* KERNEL_H */
