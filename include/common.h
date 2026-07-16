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

/* Absolute value. Uppercase so it does not shadow libc/builtin abs() (some
 * TUs need that form). Uses `<=` rather than `<` on the sign test so the
 * compiler emits `blez` for the branch, matching the original codegen
 * (`x == 0` falls into the negation arm, which still yields 0). */
#define ABS(x) ((x) <= 0 ? -(x) : (x))

/*
 * Set/clear bit @c bitPos in a 64-bit flag set stored as @c u32 flags[2].
 * @c bitPos must be a signed type (s8 / s32) — this lets gcc 2.8.0 compile
 * the slot index `(bitPos > 0x1F)` as `sll, lui, slt` (without sra), matching
 * the natural codegen for signed-byte comparisons.
 */
#define SET_FLAG(flags, bitPos)   ((flags)[(bitPos) > 0x1F] |=  (1U << ((bitPos) & 0x1F)))
#define CLEAR_FLAG(flags, bitPos) ((flags)[(bitPos) > 0x1F] &= ~(1U << ((bitPos) & 0x1F)))

/* Keep a variable alive to prevent the compiler from optimizing it away.
 * Emits no instructions; forces `x` into a register at this point and
 * counts as a use, so an initialised-but-never-read variable keeps its
 * register (e.g. the $t5 local in menututo/func_801E4BD0). */
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
 * Combined GP save+set scratchpad macro — saves $gp to `saved`, then points
 * $gp at the scratchpad (0x1F800300). The compiler materializes the address.
 */
#define GP_SAVE_SCRATCH(saved) \
    asm volatile("addu %0, $gp, $zero" : "=r"(saved)); \
    asm volatile("addu $gp, %0, $zero" : : "r"((u8 *)0x1F800300))

/*
 * Combined GP get return + restore macro — captures $gp (scratchpad pointer)
 * into ret, then restores original $gp from `saved`.
 */
#define GP_RESTORE_RET(saved, ret) \
    asm volatile( \
        "addu %0, $gp, $zero\n\t" \
        "addu $gp, %1, $zero" \
        : "=r"(ret) : "r"(saved))

/*
 * Hand-tuned addPrim — the 4-instruction sll/lwl/swl/swl sequence that
 * swaps the 24-bit P_TAG.addr field while leaving the adjacent len byte
 * untouched. Equivalent semantics to PsyQ's standard addPrim(ot, p), but
 * 4 instructions instead of 14.
 *
 * Universal asm shape (identical across all 94 occurrences in the FF8
 * binary):
 *   sll $at, prim, 8
 *   lwl tmp, 0x2(ot)
 *   swl $at, 0x2(ot)
 *   swl tmp, 0x2(prim)
 *
 * Only `tmp` varies per site, and it varies in ways no compiler-allocated
 * operand can reproduce (func_80031224 links four prims with ascending
 * $t5,$t6,$t7,$t8 where gcc coalesces any "=&r" temp into one register;
 * $v0 never appears — return-register etiquette; tiny leaves use
 * callee-saved temps). The original developers hand-picked the temp at
 * each call site, so the macro takes it as an explicit parameter.
 */

/* Single parameterized form. The link temp register is named per call
 * site as a bare token, e.g. addPrimFast(ot, p, t4) — reading exactly
 * like the assembler-macro invocation the original almost certainly was.
 *
 * This mirrors how the original code demonstrably worked: the temp varies
 * per site in ways no compiler-allocated operand can reproduce (e.g.
 * func_80031224 links four prims with ascending $t5,$t6,$t7,$t8 — gcc
 * coalesces any allocated temp into one register there), $v0 is never
 * used (return-register etiquette), and tiny leaf functions pick
 * callee-saved temps an allocator would not. The devs hand-picked the
 * register at each site; the third argument reconstructs that choice.
 * See docs/addprim-sites.md for the per-site register map. */
#define addPrimFast(ot, p, treg) do {                    \
    __asm__ __volatile__(                                \
        ".set push\n"                                    \
        ".set noat\n"                                    \
        "sll $at, %1, 8\n"                               \
        "lwl $" #treg ", 2(%0)\n"                         \
        "swl $at, 2(%0)\n"                               \
        "swl $" #treg ", 2(%1)\n"                         \
        ".set pop\n"                                     \
        :                                                \
        : "r"(ot), "r"(p)                                \
        : "memory", #treg);                               \
} while (0)

/** @brief Compiler fence: emits no code, but gcc 2.7.2 treats the volatile asm as a
 *  barrier — it will not move loads/stores across it, schedule around it, or fold a
 *  register copy from one side into the other. The original code demonstrably relied
 *  on such fences (e.g. the number renderers' entry blocks, where it keeps the
 *  stack-parameter home loads and the number argument's $a0 home-copy in the prologue
 *  instead of glued to the intToDecString call). */
#define memBarrier() __asm__ __volatile__("" : : : "memory")

/* Read / write just the 24-bit P_TAG.addr field (the OT-next pointer) at offset 2 —
 * the same lwl/swl idiom as addPrimFast, but split so a spread-out splice (read at the
 * top of a function, glyph chain in between, write at the end) can use it without raw
 * asm in the function body. A bitfield write to P_TAG.addr would compile to
 * lw/and/or/sw, not the single swl, so this must stay inline asm. */
#define getAddrFast(ot, dst) __asm__ __volatile__("lwl %0, 2(%1)" : "+r"(dst) : "r"(ot) : "memory")
#define setAddrFast(p, val)  __asm__ __volatile__("swl %0, 2(%1)" : : "r"(val), "r"(p) : "memory")

/* Chain-builder variants of the pair above (used by the be_object4 glyph
 * renderers). getAddrNewFast reads the OT next-pointer into a FRESH variable:
 * lwl only writes the top 3 bytes, so the macro supplies the untouched low
 * byte from a scratch merge-operand (the value is never inspected).
 * addOtFast prepends packet p to the chain: stores the current head into p's
 * tag and leaves p's own tag image (p << 8) as the new head; the scratch
 * input's register carries the image (clobbered by the template, in the
 * addPrimFast style). */
#define getAddrNewFast(ot, dst) { u32 _pad; __asm__ __volatile__("lwl %0, 2(%2)" : "=r"(dst) : "0"(_pad), "r"(ot) : "memory"); }
#define addOtFast(p, head) { u32 _tmp; __asm__ __volatile__("sll %1, %2, 8\n\tswl %0, 2(%2)\n\taddu %0, %1, $0" : "+r"(head) : "r"(_tmp), "r"(p) : "memory"); }

/**
 * @brief Battle encounter setup parameters at @ref D_80082C90.
 *
 * Resident scratch populated by the field-VM @c initiateBattleEncounter opcode
 * just before it kicks off a battle/card-game transition; the trailing @c result
 * is written by the battle overlay once the fight finishes and read back on the
 * @c return-pass of the same field opcode. The Triple Triad AI setup
 * (@c spawnAiTurn) reads @c field_06 / @c field_07 here as its difficulty
 * selectors (weight-table and search-depth indices).
 */
typedef struct {
    /* 0x00 */ s32 encounterPtr;
    /* 0x04 */ u8 field_04;
    /* 0x05 */ u8 field_05;
    /* 0x06 */ u8 field_06;
    /* 0x07 */ u8 field_07;
    /* 0x08 */ u8 field_08;
    /* 0x09 */ u8 field_09;
    /* 0x0A */ u8 pad0A[2];
    /* 0x0C */ u8 result;
} EncounterParams;

extern EncounterParams D_80082C90;

/** @brief Append string @p src onto @p dst (main-binary string concatenation). */
extern void func_80047C74(u8 *dst, u8 *src);

#endif /* COMMON_H */
