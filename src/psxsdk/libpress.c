#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTReset);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTGetEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTPutEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTBufSize);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTin);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_1C0);

void DecDCTout(void) { LIBPRESS_OBJ_440(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTinSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_22C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCToutSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_268);

void DecDCTinCallback(a0) s32 a0; { DMACallback(0, a0); }

void DecDCToutCallback(a0) s32 a0; { DMACallback(1, a0); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_2C0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_394);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_3A0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_3B0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_440);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_4CC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_550);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_560);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_5E4);

s32 LIBPRESS_OBJ_5F4(void) {
    extern s32 *D_8005DB10;
    return *D_8005DB10;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_60C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTvlcSize2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTvlc2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTvlcBuild);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", BUILD_OBJ_AC);
