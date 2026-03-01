#include "common.h"

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTReset);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTGetEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTPutEnv);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTBufSize);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTin);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_1C0);

/**
 * @brief Starts MDEC (Motion Decoder) output DMA transfer.
 *
 * Initiates the output side of the MDEC decompression pipeline by calling
 * the internal LIBPRESS_OBJ_440 routine. Transfers decoded image data from
 * the MDEC hardware to main memory via DMA channel 1.
 */
void DecDCTout(void) { LIBPRESS_OBJ_440(); }

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTinSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_22C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCToutSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_268);

/**
 * @brief Registers a callback for MDEC input DMA transfer completion.
 *
 * Sets a user-defined callback function to be invoked when an MDEC input
 * DMA transfer (DMA channel 0) completes. The MDEC input channel feeds
 * compressed data into the decoder.
 *
 * @param a0 Pointer to the callback function, or 0 to disable.
 */
void DecDCTinCallback(a0) s32 a0; { DMACallback(0, a0); }

/**
 * @brief Registers a callback for MDEC output DMA transfer completion.
 *
 * Sets a user-defined callback function to be invoked when an MDEC output
 * DMA transfer (DMA channel 1) completes. The MDEC output channel transfers
 * decoded image data to main memory.
 *
 * @param a0 Pointer to the callback function, or 0 to disable.
 */
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

/**
 * @brief Returns the current MDEC (decompression) status value.
 *
 * Reads and returns the internal MDEC status word, which reflects the
 * current state of the decompression hardware (busy, idle, error, etc.).
 *
 * @return The current MDEC status word.
 */
s32 LIBPRESS_OBJ_5F4(void) {
    extern s32 *D_8005DB10;
    return *D_8005DB10;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", LIBPRESS_OBJ_60C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTvlcSize2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTvlc2);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", DecDCTvlcBuild);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libpress", BUILD_OBJ_AC);
