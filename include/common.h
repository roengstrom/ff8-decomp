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

/* No-op barrier to control register allocation for decomp matching.
   Compiles to nothing; safe to remove if byte-matching is not needed. */
#ifdef __GNUC__
#define REGALLOC_BARRIER(x) asm("" : "+r"(x))
#else
#define REGALLOC_BARRIER(x)
#endif


/* Clamp value to [lo, hi] range. */
#define CLAMP(val, lo, hi) ((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))

/* Keep a variable alive to prevent the compiler from optimizing it away. */
#define KEEP_ALIVE(x) asm volatile("" :: "r"(x))

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
