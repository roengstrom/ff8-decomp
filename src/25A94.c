#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/**
 * @brief Call func_80026FE0 with a value loaded from scratchpad memory.
 * @note Reads a 32-bit value from PS1 scratchpad address 0x1F80001C.
 */
void func_80035294(void) {
    func_80026FE0(*(s32 *)0x1F80001C);
}


INCLUDE_ASM("asm/nonmatchings/25A94", func_800352BC);


/**
 * @brief Clear both VRAM framebuffers to black.
 *
 * Clears two 384x224 (0x180 x 0xE0) regions in VRAM: the first at (0,0)
 * and the second at (0x200,0). Each clear is followed by DrawSync(0) to
 * wait for completion.
 */
void func_800352EC(void) {
    short rect[4];
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x180;
    rect[3] = 0xE0;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
    rect[0] = 0x200;
    rect[1] = 0;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
}


INCLUDE_ASM("asm/nonmatchings/25A94", func_80035360);


INCLUDE_ASM("asm/nonmatchings/25A94", func_80035A6C);


INCLUDE_ASM("asm/nonmatchings/25A94", func_80035AA4);


extern u8 D_80085138;
// set D_80085138 (u8)

INCLUDE_ASM("asm/nonmatchings/25A94", func_80035AE4);


INCLUDE_ASM("asm/nonmatchings/25A94", func_80035B28);


INCLUDE_ASM("asm/nonmatchings/25A94", func_80035B70);


/** @brief Stores a word to global D_80083798. */
void func_80035BB4(s32 a0) {
    extern s32 D_80083798;
    D_80083798 = a0;
}


/** @brief Stores a word to global D_8008379C. */
void func_80035BC0(s32 a0) {
    extern s32 D_8008379C;
    D_8008379C = a0;
}


/**
 * @brief Dispatch a VSync callback or execute default VSync actions.
 *
 * If D_80083798 is non-NULL, calls it as a function pointer. Otherwise,
 * calls func_80013300 with parameters 1 and 3 sequentially.
 */
void func_80035BCC(void) {
    extern s32 D_80083798;
    void (*fp)(void) = (void (*)(void))D_80083798;
    if (fp != 0) {
        fp();
    } else {
        func_80013300(1);
        func_80013300(3);
    }
}


/**
 * @brief Dispatch a draw callback or execute default draw actions.
 *
 * If D_8008379C is non-NULL, calls it as a function pointer. Otherwise,
 * calls func_8001336C with parameters 3 and 1 sequentially.
 */
void func_80035C10(void) {
    extern s32 D_8008379C;
    void (*fp)(void) = (void (*)(void))D_8008379C;
    if (fp != 0) {
        fp();
    } else {
        func_8001336C(3);
        func_8001336C(1);
    }
}


/**
 * @brief Set the global flag D_80085138.
 * @param val Value to store.
 */
void func_80035C54(u8 val) {
    D_80085138 = val;
}


/**
 * @brief Get the current value of the global flag D_80085138.
 * @return The flag value as an unsigned byte.
 */
u8 func_80035C60(void) {
    return D_80085138;
}


extern s32 D_8008513C;

/**
 * @brief Set a single bit in the global bitmask D_8008513C.
 * @param a0 Bit position (0-31) to set.
 */
void func_80035C70(s32 a0) {
    D_8008513C |= (1 << a0);
}


/**
 * @brief Store a value in the upper 16 bits of D_8008513C and set bit 2.
 *
 * Shifts a0 left by 16 and ORs it into D_8008513C, then calls func_80035C70(2)
 * to also set bit 2 as a "data present" flag.
 *
 * @param a0 Value to store in the upper halfword of D_8008513C.
 */
void func_80035C8C(s32 a0) {
    D_8008513C |= (a0 << 16);
    func_80035C70(2);
}
