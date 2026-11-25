#ifndef IMGTOC_H
#define IMGTOC_H

#include "cd.h"

// FIXME
// These are a bit of a mystery. These point to memory addresses in the range where
// img_toc.bin is loaded into. Not sure how the original code would've handled this.
// Possibly through some generated linker script after mastering?

extern CdFileDesc g_bootFileDescs[];
extern CdFileDesc g_fieldFileDescs[];
extern u32 g_menuFileDescs[];
extern CdFileDesc D_800974B8[];
extern CdFileDesc D_800974C0[];
extern CdFileDesc D_800974C8[];
extern CdFileDesc g_worldFileDescs[];
extern CdFileDesc g_soundFileDescs[];
extern u32 D_800974F0[];
extern CdFileDesc g_kernelFileDesc;
extern CdFileDesc g_systemFileDescs[];

// Files of the menu group (g_menuFileDescs), counted from TOC entry 4.
typedef enum {
    DISC_FILE_MENUMAIN = 0,
    DISC_FILE_MENUCFG = 1,
    DISC_FILE_MENUPTY = 2,
    DISC_FILE_MENUSTS = 3,
    DISC_FILE_MENUABL = 4,
    DISC_FILE_MENUSHOP = 5,
    DISC_FILE_MENUEXT = 6,
    DISC_FILE_MENUITEM = 7,
    DISC_FILE_MENUMGC = 8,
    DISC_FILE_MENUGF = 9,
    DISC_FILE_MENUJNC2 = 10,
    DISC_FILE_MENUSAV = 11,
    DISC_FILE_MENUCRD = 12,
    DISC_FILE_MENUTUTO = 13,
    DISC_FILE_MENUTMAG = 14,
    DISC_FILE_MENUTIPS = 15,
    DISC_FILE_MENUTEST = 16,
    DISC_FILE_MNGRP = 17,
    DISC_FILE_INIT_OUT = 18
} DiscFile;

#endif /* IMGTOC_H */
