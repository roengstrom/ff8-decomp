#include "common.h"

/** @brief Empty stub, called at the very start of main before initialization.
 *  @note Purpose uncertain — may be a debug hook or placeholder that was
 *        never filled in the retail build.
 */
void preInitStub(void) {
}

/** @brief CRT0 entry point. Clears BSS, sets up GP register, and calls main. */
INCLUDE_ASM("asm/nonmatchings/crt0", start);
