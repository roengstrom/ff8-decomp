#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad_init", PadInit);

u32 PadRead(void) {
    extern u32 D_8008A6D0;
    PAD_dr();
    return ~D_8008A6D0;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad_init", PadStop);
