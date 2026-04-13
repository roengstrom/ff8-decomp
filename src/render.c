#include "common.h"
#include "psxsdk/libgpu.h"
#include "battle.h"

/**
 * @brief Call switchThread with a value loaded from scratchpad memory.
 * @note Reads a 32-bit value from PS1 scratchpad address 0x1F80001C.
 */
void dispatchScratchpadThread(void) {
    switchThread(*(s32 *)0x1F80001C);
}


/**
 * @brief Main game loop — runs forever calling render and VSync handlers.
 *
 * Alternates between func_80035158 (render frame) and switchThread(0)
 * (VSync/update) indefinitely. Never returns.
 */
void mainGameLoop(void) {
    for (;;) {
        func_80035158();
        switchThread(0);
    }
}


/**
 * @brief Clear both VRAM framebuffers to black.
 *
 * Clears two 384x224 (0x180 x 0xE0) regions in VRAM: the first at (0,0)
 * and the second at (0x200,0). Each clear is followed by DrawSync(0) to
 * wait for completion.
 */
void clearFramebuffers(void) {
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


INCLUDE_ASM("asm/nonmatchings/render", func_80035360);


/**
 * @brief Counts the number of set bits in a 32-bit bitmask.
 * @param a0 Bitmask to count.
 * @return Number of set bits (0-32).
 */
s32 popcount(s32 a0) {
    s32 count = 0;
    s32 i = count;
    s32 bit = 1;
    do {
        if (a0 & (bit << i)) {
            count++;
        }
        i++;
    } while (i < 32);
    return count;
}


/**
 * @brief Finds the position of the N-th set bit in a 32-bit bitmask.
 *
 * Scans bits 0-31 of @p a0. Each time a set bit is found, decrements @p a1.
 * When @p a1 reaches 0, returns the bit position. Returns -1 if not found.
 *
 * @param a0 Bitmask to search.
 * @param a1 Which set bit to find (0 = first set bit).
 * @return Bit position (0-31) or -1 if not enough set bits.
 */
s32 findNthSetBit(s32 a0, s32 a1) {
    s32 i = 0;
    s32 bit = 1;
    do {
        if (a0 & (bit << i)) {
            if (a1 == 0) {
                return i;
            }
            a1--;
        }
        i++;
    } while (i < 32);
    return -1;
}


extern u8 D_80085138;
extern s32 D_80083798;
extern s32 D_8008379C;

/**
 * @brief Finds the bit position in @p a0 of the @p a1-th set bit (0-indexed).
 *
 * Scans bits 0-31 of @p a0, counting set bits. When the running count
 * equals @p a1, returns that bit position. Returns 0 if not found.
 *
 * @param a0 Bitmask to search.
 * @param a1 Target set-bit index to match.
 * @return Bit position (0-31) or 0 if not found.
 */
s32 getBitRank(s32 a0, s32 a1) {
    s32 count = 0;
    s32 i = count;
    s32 bit = 1;
    do {
        if (a0 & (bit << i)) {
            if (i == a1) {
                return count;
            }
            count++;
        }
        i++;
    } while (i < 32);
    return 0;
}


INCLUDE_ASM("asm/nonmatchings/render", func_80035B28);


INCLUDE_ASM("asm/nonmatchings/render", func_80035B70);


/** @brief Stores a word to global D_80083798. */
void setVsyncCallback(s32 a0) {
    D_80083798 = a0;
}


/** @brief Stores a word to global D_8008379C. */
void setDrawCallback(s32 a0) {
    D_8008379C = a0;
}


/**
 * @brief Dispatch a VSync callback or execute default VSync actions.
 *
 * If D_80083798 is non-NULL, calls it as a function pointer. Otherwise,
 * calls sndEnableReverb with parameters 1 and 3 sequentially.
 */
void dispatchVsyncCallback(void) {
    void (*fp)(void) = (void (*)(void))D_80083798;
    if (fp != 0) {
        fp();
    } else {
        sndEnableReverb(1);
        sndEnableReverb(3);
    }
}


/**
 * @brief Dispatch a draw callback or execute default draw actions.
 *
 * If D_8008379C is non-NULL, calls it as a function pointer. Otherwise,
 * calls sndDisableReverb with parameters 3 and 1 sequentially.
 */
void dispatchDrawCallback(void) {
    void (*fp)(void) = (void (*)(void))D_8008379C;
    if (fp != 0) {
        fp();
    } else {
        sndDisableReverb(3);
        sndDisableReverb(1);
    }
}


/**
 * @brief Set the global flag D_80085138.
 * @param val Value to store.
 */
void setRenderFlag(u8 val) {
    D_80085138 = val;
}


/**
 * @brief Get the current value of the global flag D_80085138.
 * @return The flag value as an unsigned byte.
 */
u8 getRenderFlag(void) {
    return D_80085138;
}


extern s32 D_8008513C;

/**
 * @brief Set a single bit in the global bitmask D_8008513C.
 * @param a0 Bit position (0-31) to set.
 */
void setModeBit(s32 a0) {
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
void setModeData(s32 a0) {
    D_8008513C |= (a0 << 16);
    setModeBit(2);
}
