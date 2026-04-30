#include "common.h"

extern u8 D_8005E9EE;
extern s32 *D_8005EAF4;

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", LoadTPage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", EXT_OBJ_A8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", EXT_OBJ_AC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", LoadClut);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", LoadClut2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDefDrawEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", EXT_OBJ_228);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDefDispEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ResetGraph);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_110);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_160);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetGraphDebug);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetGraphQueue);

/**
 * @brief Returns the current GPU debug level.
 *
 * Reads the cached graphics debug level set by SetGraphDebug(). Controls
 * the verbosity of GPU-related debug output.
 *
 * @return The current debug level (0 = no debug output).
 */
u8 GetGraphDebug(void) {
    return D_8005E9EE;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", DrawSyncCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDispMask);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", DrawSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_3E4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_4B4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_4F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ClearImage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ClearImage2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", LoadImage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", StoreImage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", MoveImage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_788);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ClearOTag);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ClearOTagR);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", DrawPrim);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", DrawOTag);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", PutDrawEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", DrawOTagEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", GetDrawEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", PutDispEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_D4C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_F64);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_FF4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_100C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", GetDispEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", GetODE);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetTexWindow);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawArea);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawOffset);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetPriority);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawStp);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1244);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawMode);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1400);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_149C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_15F8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_16F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_170C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_172C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_17A8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_17C4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1840);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_185C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1878);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_18F0);

/**
 * @brief Returns the current GPU system state value.
 *
 * Reads and returns the value from the internal GPU system state pointer,
 * used by libgpu for tracking GPU hardware status.
 *
 * @return The current GPU system state word.
 */
s32 SYS_OBJ_18F8(void) {
    return *D_8005EAF4;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1910);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_19F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1A48);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1A88);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1BF8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1C20);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1C88);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1CC4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1E38);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1E5C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1EBC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_1EF8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_20BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_20DC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2100);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2114);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2154);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_219C);

/**
 * @brief Wrapper for GPU system operation with a default third parameter of 0.
 *
 * Calls the underlying SYS_OBJ_21F0 function, inserting 0 as the third
 * argument. Part of the internal GPU command dispatching mechanism.
 *
 * @param a0 First parameter passed through to SYS_OBJ_21F0.
 * @param a1 Second parameter passed through to SYS_OBJ_21F0.
 * @param a2 Fourth parameter passed through to SYS_OBJ_21F0.
 */
void SYS_OBJ_21CC(s32 a0, s32 a1, s32 a2) {
    SYS_OBJ_21F0(a0, a1, 0, a2);
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_21F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004AAF8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_223C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_233C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_23F4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2484);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_24A0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2700);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_281C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2850);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2870);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2888);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_297C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_298C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_29C0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2AF4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2B04);

/**
 * @brief Empty stub in the GPU system object table.
 *
 * Placeholder function in the libgpu internal dispatch table.
 * No-op in this SDK version.
 */
void SYS_OBJ_2B9C(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", LoadImage2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004B4D0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2C20);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", StoreImage2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004B5BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2D0C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", MoveImage2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004B6B0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2E00);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2EA8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", DrawOTag2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004B800);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2F50);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2FBC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SYS_OBJ_2FE4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", GPU_cw);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", OpenTIM);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ReadTIM);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_64);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", OpenTMD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", ReadTMD);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_35C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_43C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_45C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_474);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_5E8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_1854);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", TMD_OBJ_186C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D174);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D208);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D268);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D3E4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D440);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D474);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D4BC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D4F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", GetTPage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", GetClut);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", AddPrim);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", AddPrims);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetSemiTrans);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetShadeTex);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D664);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D684);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D6A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D6C4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", func_8004D6E4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetTile);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawTPage);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawMove);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libgpu", SetDrawLoad);
