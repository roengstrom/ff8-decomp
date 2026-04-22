#ifndef COMMON_H
#define COMMON_H

#include "include_asm.h"

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

#ifndef NULL
#define NULL ((void *)0)
#endif

/**
 * @brief 16-bit slot accessed as either a halfword or its two constituent bytes.
 *
 * Used to model struct fields that are read/written at different widths by
 * different functions — e.g. a u16 counter that one function updates via a
 * halfword store (@c sh) and another treats as two independent u8 flags via
 * byte stores (@c sb). Access the full halfword as @c .hword, the low byte
 * (little-endian @c byte[0]) as @c .b.lo, and the high byte (@c byte[1]) as
 * @c .b.hi.
 */
typedef union {
    u16 hword;
    struct { u8 lo; u8 hi; } b;
} U16Split;

/* No-op barrier to control register allocation for decomp matching.
   Compiles to nothing; safe to remove if byte-matching is not needed. */
#ifdef __GNUC__
#define REGALLOC_BARRIER(x) asm("" : "+r"(x))
#else
#define REGALLOC_BARRIER(x)
#endif
#define NOP() asm("nop")


/* Clamp value to [lo, hi] range. */
#define CLAMP(val, lo, hi) ((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))

/* Keep a variable alive to prevent the compiler from optimizing it away. */
#define KEEP_ALIVE(x) asm volatile("" :: "r"(x))

/*
 * GP stack allocation — allocates `size` bytes from the GP-relative area,
 * returning the current GP in `ptr`. Used for temporary scratchpad structs.
 */
#define GP_ALLOC(ptr, size) \
    asm volatile("addu %0, $gp, $zero" : "=r"(ptr)); \
    asm volatile("addi $gp, $gp, %0" : : "i"(size))

/* Free `size` bytes from the GP stack (reverses GP_ALLOC). */
#define GP_FREE(size) \
    asm volatile("addi $gp, $gp, -%0" : : "i"(size))

/*
 * Combined GP save+set scratchpad macro — saves $gp to `saved`, sets $gp to
 * scratchpad (0x1F800300) via $a2. Used by btl_anim display list functions.
 */
#define GP_SAVE_SCRATCH(saved) \
    asm volatile("addu %0, $gp, $zero" : "=r"(saved)); \
    asm volatile("lui $a2, 0x1F80"); \
    asm volatile("ori $a2, $a2, 0x0300"); \
    asm volatile("addu $gp, $a2, $zero")

/*
 * Combined GP get return + restore macro — captures $gp (scratchpad pointer)
 * into ret, then restores original $gp from `saved`.
 */
#define GP_RESTORE_RET(saved, ret) \
    asm volatile( \
        "addu %0, $gp, $zero\n\t" \
        "addu $gp, %1, $zero" \
        : "=r"(ret) : "r"(saved))

#endif /* COMMON_H */
