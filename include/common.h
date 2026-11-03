#ifndef COMMON_H
#define COMMON_H

#include "include_asm.h"

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef int intrptr_t;
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
#define STRINGIFY(x) #x
#define MTC0(value, reg) __asm__ volatile("mtc0 %0, " STRINGIFY(reg) : : "r"(value))


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
 * One asm statement: when `ptr` lives on the stack, its store lands after
 * both instructions (text/drawDialogText).
 */
#define GP_ALLOC(ptr, size) \
    asm volatile("addu %0, $gp, $zero\n\taddi $gp, $gp, %1" : "=r"(ptr) : "i"(size))

/* Free `size` bytes from the GP stack (reverses GP_ALLOC). */
#define GP_FREE(size) \
    asm volatile("addi $gp, $gp, -%0" : : "i"(size))

/* Combined GP save+set macro — saves $gp to `saved`, then points $gp at `addr`. */
#define GP_SAVE_SET(saved, addr) \
    asm volatile("addu %0, $gp, $zero" : "=r"(saved)); \
    asm volatile("addu $gp, %0, $zero" : : "r"(addr))

/* GP_SAVE_SET onto the scratchpad (0x1F800300). The compiler materializes the address. */
#define GP_SAVE_SCRATCH(saved) GP_SAVE_SET(saved, (u8 *)0x1F800300)

/*
 * Combined GP get return + restore macro — captures $gp (scratchpad pointer)
 * into ret, then restores original $gp from `saved`. Two asm statements: when
 * `saved` lives on the stack, its reload lands between them (text/drawMessageText).
 */
#define GP_RESTORE_RET(saved, ret) \
    asm volatile("addu %0, $gp, $zero" : "=r"(ret)); \
    asm volatile("addu $gp, %0, $zero" : : "r"(saved))

/**
 * optimized version of PsyQ's addPrim macro that saves 10 instructions
 */
#define addPrimFast(ot, p) {                                 \
    u32 t_;                                                  \
    __asm__ __volatile__(                                    \
        ".set push\n"                                        \
        ".set noat\n"                                        \
        "sll $at, %1, 8\n"                                   \
        "lwl %0, 2(%2)\n"                                    \
        "swl $at, 2(%2)\n"                                   \
        "swl %0, 2(%1)\n"                                    \
        ".set pop\n"                                         \
        : "+r"(t_)                                           \
        : "r"(p), "r"(ot)                                    \
        : "at", "memory");                                   \
}

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
 * addPrimFast style).
 * getAddrNewFast has no "memory" clobber: reorg must be able to see that a
 * reload of `ot` made before it is still valid after it (text/drawMessageText).
 * getAddrNewFast writes the result back into its scratch. That is dead code
 * where the macro runs once, but inside a loop the scratch becomes
 * loop-carried: text.c's text renderers reload it from its stack slot into
 * the head register and store it back around the lwl.
 * addOtTagFast is addOtFast without the final copy: it leaves p's tag image in
 * the allocated temp `tag` ("+r", so an uninitialised temp is live from
 * function entry) and the caller moves it into the head itself. */
#define getAddrNewFast(ot, dst) { u32 _pad; __asm__ __volatile__("lwl %0, 2(%2)" : "=r"(dst) : "0"(_pad), "r"(ot)); _pad = dst; }
#define addOtFast(p, head) { u32 _tmp; __asm__ __volatile__("sll %1, %2, 8\n\tswl %0, 2(%2)\n\taddu %0, %1, $0" : "+r"(head) : "r"(_tmp), "r"(p) : "memory"); }
#define addOtTagFast(p, head, tag) __asm__ __volatile__("sll %0, %2, 8\n\tswl %1, 2(%2)" : "+r"(tag) : "r"(head), "r"(p) : "memory")

/** @brief Prepend packet @p p to an OT chain; returns the new chain head. */
static inline u32 linkPacket(u32 head, void *p) {
    u32 tag;

    addOtTagFast(p, head, tag);
    return tag;
}

/** @brief GP0(E2h) word that turns the texture window off. */
#define TEXWINDOW_OFF 0xE2000000

/** @brief A sprite that carries its own draw mode and texture window (tag length 7). */
typedef struct {
    u32 tag;
    u32 drawMode; /* GP0(E1h) */
    u32 texWindow[2]; /* GP0(E2h), then a zero word */
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    u16 w, h;
} ModeSprt;

// GPU semi-transparency modes, the abr argument of GetTPage.
typedef enum {
    BLEND_AVERAGE = 0, // 0.5 x screen + 0.5 x drawn
    BLEND_ADD = 1, // screen + drawn
    BLEND_SUBTRACT = 2, // screen - drawn
    BLEND_ADD_QUARTER = 3 // screen + 0.25 x drawn
} BlendMode;

/* Mark an uninitialised variable as deliberately carrying whatever garbage
 * its register holds. The empty volatile asm is a definition the optimiser
 * can neither delete nor propagate, and it emits no instructions — so the
 * variable stops being live-in at function entry and the register allocator
 * classifies it like any defined local. Needed to reproduce dead code that
 * reads uninitialised locals (func_800A8024 multiplies two of these): with
 * no definition at all, gcc treats the pseudo as global and every
 * callee-saved register shifts by one. volatile is load-bearing — a
 * non-volatile version gets scheduled past the next call, shrinking the
 * live range into a caller-saved register. */
#define uninitReg(var) __asm__ __volatile__("" : "=r"(var))

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


/** @brief Integer square root of @p a (main-binary helper). */

/** @brief Poll the GPU; returns non-zero while it is still busy.
 *         Callers that only need the side effect ignore the result. */

#endif /* COMMON_H */
