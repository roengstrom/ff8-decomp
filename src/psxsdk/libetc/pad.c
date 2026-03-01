#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", SetInitPadFlag);

s32 ReadInitPadFlag(void) {
    extern s32 D_8005E97C;
    return D_8005E97C;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", PAD_init);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", InitPAD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", StartPAD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", StopPAD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", PAD_OBJ_1A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", PAD_OBJ_21C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", PAD_OBJ_254);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", PAD_OBJ_2BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", InitPAD2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", StartPAD2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", StopPAD2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", PAD_init2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", EnablePAD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", DisablePAD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", _patch_pad);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", _remove_ChgclrPAD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", func_80047C34);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc/pad", func_80047C3C);
