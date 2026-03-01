#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _SpuInit);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuStart);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_init);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_D4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_280);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_390);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_424);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_FiDMA);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_4EC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_Fr_);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_t);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_78C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_814);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_Fw);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_894);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_Fr);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_FsetRXX);

/**
 * @brief Empty stub following _spu_FsetRXX in the SPU object table.
 *
 * Placeholder function in the libspu internal register access table.
 * No-op in this SDK version.
 */
void SPU_OBJ_948(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_FsetRXXa);

/**
 * @brief Empty stub following _spu_FsetRXXa in the SPU object table.
 *
 * Placeholder function in the libspu internal register access table.
 * No-op in this SDK version.
 */
void SPU_OBJ_9EC(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_FgetRXXa);

/**
 * @brief Empty stub following _spu_FgetRXXa in the SPU object table.
 *
 * Placeholder function in the libspu internal register access table.
 * No-op in this SDK version.
 */
void SPU_OBJ_A28(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_FsetPCR);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_A7C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_A88);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_AB0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_Fw1ts);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", func_8003DDA0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SPU_OBJ_B18);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", func_8003DDF4);

/**
 * @brief Initializes the SPU (Sound Processing Unit) with default settings.
 *
 * Calls _SpuInit with mode 0, which performs a full SPU hardware reset
 * including clearing all voice attributes, resetting the SPU control
 * register, and initializing the sound RAM management.
 */
void func_8003DE24(void) { _SpuInit(0); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", func_8003DE44);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuInitMalloc);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_M_INIT_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetNoiseClock);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetReverb);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SR_OBJ_B0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SR_OBJ_B4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _SpuIsInAllocateArea);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_M_UTIL_OBJ_2C);

/**
 * @brief Empty stub in the SPU memory utility object table.
 *
 * Placeholder function in the libspu sound RAM allocation utility table.
 * No-op in this SDK version.
 */
void S_M_UTIL_OBJ_78(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _SpuIsInAllocateArea_);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_M_UTIL_OBJ_B8);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_M_UTIL_OBJ_104);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetIRQ);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SI_OBJ_F4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SI_OBJ_12C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetIRQAddr);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SIA_OBJ_30);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetIRQCallback);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", func_8003E314);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuRead);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", func_8003E3A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetTransferStartAddr);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_STSA_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetTransferMode);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_STM_OBJ_1C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", func_8003E494);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuGetVoiceEnvelope);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuSetReverbModeType);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SRMT_OBJ_FC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SRMT_OBJ_10C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_SRMT_OBJ_1B4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", _spu_setReverbAttr);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", SpuClearReverbWorkArea);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_CRWA_OBJ_9C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libspu", S_CRWA_OBJ_170);
