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
 * Scratchpad GP macros — swap $gp to point at PS1 scratchpad RAM (0x1F800300)
 * for fast temporary buffer access, then restore original $gp afterward.
 */
#define GP_SAVE(out) \
    asm volatile("addu %0, $gp, $0" : "=r"(out))
#define GP_SET_SCRATCH() \
    asm volatile( \
        "lui   $7, 0x1F80\n" \
        "ori   $7, $7, 0x0300\n" \
        "addu  $gp, $7, $0" \
        : : : "gp", "$7" \
    )
#define GP_GET(out) \
    asm volatile("addu %0, $gp, $0" : "=r"(out))
#define GP_ADVANCE(n) \
    asm volatile("addi $gp, $gp, " #n : : : "gp")
#define GP_RESTORE(in) \
    asm volatile("addu $gp, %0, $0" : : "r"(in) : "gp")

/*
 * Combined GP save+set scratchpad macro — saves $gp to $s2, sets $gp to
 * scratchpad via $a2. Used by btl_anim display list functions.
 */
#define GP_SAVE_SCRATCH() \
    asm volatile( \
        "addu $s2, $gp, $zero\n\t" \
        "lui $a2, 0x1F80\n\t" \
        "ori $a2, $a2, 0x0300\n\t" \
        "addu $gp, $a2, $zero" \
        ::: "$18")

/*
 * Combined GP get return + restore macro — captures $gp (scratchpad pointer)
 * into ret, then restores original $gp from $s2.
 */
#define GP_RESTORE_RET(ret) \
    asm volatile( \
        "addu %0, $gp, $zero\n\t" \
        "addu $gp, $s2, $zero" \
        : "=r"(ret))

#endif /* COMMON_H */
