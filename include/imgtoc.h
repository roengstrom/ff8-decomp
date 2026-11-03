#ifndef IMGTOC_H
#define IMGTOC_H

#include "cd.h"

// FIXME
// These are a bit of a mystery. These point to memory addresses in the range where
// img_toc.bin is loaded into. Not sure how the original code would've handled this.
// Possibly through some generated linker script after mastering?

extern CdFileDesc g_bootFileDescs[];
extern CdFileDesc g_fieldFileDescs[];
extern u32 D_80097420[];
extern CdFileDesc D_800974B8[];
extern CdFileDesc D_800974C0[];
extern CdFileDesc D_800974C8[];
extern CdFileDesc g_worldFileDescs[];
extern CdFileDesc g_soundFileDescs[];
extern u32 D_800974F0[];
extern CdFileDesc g_kernelFileDesc;
extern CdFileDesc g_systemFileDescs[];

#endif /* IMGTOC_H */
