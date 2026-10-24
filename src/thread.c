#include "common.h"
#include "psxsdk/kernel.h"
#include "psxsdk/libapi.h"
#include "psxsdk/r3000.h"
#include "thread.h"

/**
 * @brief Open a thread with interrupt protection.
 * @param entry Function the thread starts in.
 * @param stack Top of the thread's stack.
 * @return Thread handle from OpenTh.
 * @note Wraps PsyQ OpenTh in EnterCriticalSection and ExitCriticalSection.
 */
s32 openThreadSafe(void (*entry)(void), u8 *stack) {
    s32 result;
    EnterCriticalSection();
    result = OpenTh(entry, stack, 0);
    ExitCriticalSection();
    return result;
}


/**
 * @brief Close a thread with interrupt protection.
 * @param a0 Thread handle to close.
 * @note Wraps PsyQ CloseTh in EnterCriticalSection and ExitCriticalSection.
 */
void closeThreadSafe(s32 a0) {
    EnterCriticalSection();
    CloseTh(a0);
    ExitCriticalSection();
}


/**
 * @brief Manually switch the current thread without a syscall.
 *
 * Resolves the thread control block for @p handle through the kernel
 * table of tables and, if that thread is active, stores it as the
 * current thread in the TCB header.
 *
 * @param handle Thread handle (low 24 bits index the TCB table).
 */
void func_80026F4C(s32 handle) {
    ToT *tot = KERNEL_TOT;
    TCB *tcbTable = (TCB *)tot[2].head;
    TCBH *tcbh = (TCBH *)tot[1].head;
    TCB *tcb = &tcbTable[handle & 0xFFFFFF];
    if (tcb->status == TcbStACTIVE) {
        tcbh->entry = tcb;
    }
}


/**
 * @brief Compute an address offset from a 24-bit color value.
 *
 * Masks the input to 24 bits, multiplies by 192, and adds to the base
 * address loaded from kernel memory at 0x110.
 *
 * @param a0 Input value (only lower 24 bits used).
 * @return Base address + masked input * 192.
 */
s32 getThreadControlBlock(s32 a0) {
    s32 base = *(s32 *)0x110;
    a0 &= 0xFFFFFF;
    return base + a0 * 192;
}


/**
 * @brief Read the CPU's status register.
 * @return The register's value.
 */
u32 getStatusRegister(void) { return GetSr(); }


/** @brief Write @p sr to the CPU's status register. */
void setStatusRegister(u32 sr) {
    __asm__ volatile("mtc0 %0, $12" : : "r"(sr));
}


/**
 * @brief Switch to a thread, using a fallback address if a0 is 0.
 * @param a0 Thread handle to switch to; 0 means the main thread (0xFF000000).
 * @note If GetSr returns bit 2 set, uses func_80026F4C instead of PsyQ ChangeTh.
 */
void switchThread(s32 a0) {
    if (a0 == 0) {
        a0 = (s32)0xFF000000;
    }
    if (GetSr() & SR_IEP) {
        func_80026F4C(a0);
    } else {
        ChangeTh(a0);
    }
}
