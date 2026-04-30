#include "common.h"

extern u16 *D_8005D554;
extern s32 D_8005D56C[];

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_A4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_164);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_178);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", VSYNC_OBJ_200);

typedef struct {
    void (*f0)();
    void (*f1)();
    void (*f2)();
    void (*f3)();
    void (*f4)();
    void (*f5)();
    void (*f6)();
} CallbackTable;
extern CallbackTable *D_8005D54C;

/**
 * @brief Resets all system callbacks to their default state.
 *
 * Calls the internal callback table's reset handler (index 3) to unregister
 * all user-installed callbacks (VSync, DMA, interrupt, etc.) and restore
 * default behavior. Typically called during system initialization.
 */
void ResetCallback() { D_8005D54C->f3(); }

/**
 * @brief Registers or invokes the hardware interrupt callback handler.
 *
 * Calls the internal callback table's interrupt handler (index 2) to
 * manage hardware interrupt callback registration.
 */
void InterruptCallback() { D_8005D54C->f2(); }

/**
 * @brief Registers a DMA completion callback for a specific DMA channel.
 *
 * Calls the internal callback table's DMA handler (index 1) to install or
 * remove a user callback that fires when a DMA transfer completes on the
 * specified channel.
 */
void DMACallback() { D_8005D54C->f1(); }

/**
 * @brief Registers a VSync callback function.
 *
 * Installs a user-defined function that will be called on every vertical
 * blanking interrupt. The callback is registered via the internal callback
 * table's VSync handler (index 5) with interrupt type 4 (VBlank).
 *
 * @param a0 Pointer to the callback function, or 0 to unregister.
 */
void VSyncCallback(a0) s32 a0; { D_8005D54C->f5(4, a0); }

/**
 * @brief Registers multiple VSync callback functions.
 *
 * Calls the internal callback table's VSync handler (index 5) with default
 * arguments to set up VSync callback processing for all registered handlers.
 */
void VSyncCallbacks() { D_8005D54C->f5(); }

/**
 * @brief Stops and disables all system callbacks.
 *
 * Calls the internal callback table's stop handler (index 4) to temporarily
 * suspend all callback processing. Callbacks can be restarted with
 * RestartCallback().
 */
void StopCallback() { D_8005D54C->f4(); }

/**
 * @brief Restarts system callbacks after they have been stopped.
 *
 * Calls the internal callback table's restart handler (index 6) to resume
 * callback processing that was previously suspended by StopCallback().
 */
void RestartCallback() { D_8005D54C->f6(); }

extern u16 D_8005C4C6;
/**
 * @brief Checks whether system callbacks are currently active.
 *
 * Returns the current callback status flag indicating whether callbacks
 * are enabled and running.
 *
 * @return Non-zero if callbacks are active, 0 if stopped.
 */
u16 CheckCallback() { return D_8005C4C6; }

/**
 * @brief Returns the current interrupt mask register value.
 *
 * Reads the cached interrupt mask (I_MASK) register value, which controls
 * which hardware interrupts are enabled (VBlank, GPU, CD-ROM, DMA, etc.).
 *
 * @return The 16-bit interrupt mask value.
 */
u16 GetIntrMask(void) {
    return *D_8005D554;
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", SetIntrMask);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_194);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_26C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_410);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_43C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_4EC);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_584);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_624);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_68C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_OBJ_69C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", startIntrVSync);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_VB_OBJ_58);

/**
 * @brief Sets a VBlank interrupt handler entry if it has changed.
 *
 * Conditionally updates the VBlank callback table at the given index.
 * Only writes the new value if it differs from the current entry, avoiding
 * unnecessary writes.
 *
 * @param a0 Index into the VBlank handler table.
 * @param a1 New handler value (callback pointer or flag) to store.
 */
void INTR_VB_OBJ_C4(s32 a0, s32 a1) {
    if (a1 != D_8005D56C[a0]) {
        D_8005D56C[a0] = a1;
    }
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_VB_OBJ_F0);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", startIntrDMA);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_DMA_OBJ_4C);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_DMA_OBJ_1CC);

/**
 * @brief Empty stub in the DMA interrupt handler table.
 *
 * Placeholder function in the libetc DMA interrupt dispatch table.
 * No-op in this SDK version.
 */
void INTR_DMA_OBJ_270(void) {
}

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", INTR_DMA_OBJ_278);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", func_800432C4);

INCLUDE_ASM("asm/nonmatchings/psxsdk/libetc", func_800432D8);
