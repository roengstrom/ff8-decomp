# Decomp Patterns & Compiler Quirks

A reference for CC1PSX (GCC 2.7.2 / PsyQ 4.1) and CC1PSX (GCC 2.8.0 / PsyQ 4.3)
behavior at `-O2 -G0`. These patterns were discovered while decomping ff8-decomp
functions and verified by byte-matching against the original binary.

See also: [s-reg-allocation.md](s-reg-allocation.md) for s-reg specific tricks.

---

## Table of Contents

1. [Scheduling & Instruction Ordering](#scheduling--instruction-ordering)
2. [Preventing Compiler Optimizations](#preventing-compiler-optimizations)
3. [Register Allocation](#register-allocation)
4. [GPU / PsyQ Patterns](#gpu--psyq-patterns)
5. [Loop Patterns](#loop-patterns)
6. [Known Non-Matching Patterns](#known-non-matching-patterns)

---

## Scheduling & Instruction Ordering

### `asm("")` as scheduling barrier

`asm("")` prevents CC1PSX `-fschedule-insns` from interleaving independent operations
across it. Useful when the scheduler fills a load delay slot by pulling in a later
volatile load.

```c
v0 = *(volatile u16 *)addr1;
asm("");   // prevents the next load from being scheduled into v0's delay slot
v1 = *(volatile u16 *)addr2;
```

**Limitations:**
- Does NOT prevent branch delay slot filling (CC1PSX sets branch delays via
  `.set noreorder` before seeing the barrier)
- Does NOT control which register a variable gets

*Example: func_8001167C first accumulator*

### REGALLOC_BARRIER

`REGALLOC_BARRIER(var)` is defined as `asm("" : "+r"(var))`. It creates a use+def
of the variable, which:
- Pins the variable to a specific register ($2 for v0, etc.) at that point
- Prevents the scheduler from moving instructions past it (the variable is "used")
- Does NOT control which register number — it just forces the current allocation to
  be materialized

```c
v0 = func_8002E680(arg);
REGALLOC_BARRIER(v0);  // pins v0 to $2, prevents scheduler from moving arg setup ahead
maxW = 0x150;
```

### Multi-operand asm for store ordering

When the scheduler moves stores before loads, pin all the relevant registers:

```c
v0 = *(type *)addr;
REGALLOC_BARRIER(v0);                              // pin v0 to $2
asm("" : "+r"(v0) : "r"(v1), "r"(a0), "r"(a1));  // prevent store from moving before loads
*(type *)dest = v0;
```

*Example: func_800119D4*

### Initialization order controls prologue interleaving

The compiler interleaves `sw sN` + init for each variable in the order they're
initialized:

```c
s32 i = 0;           // -> sw s1 / move s1, $0    (s1 saved first)
s32 ptr = (s32)SYM;  // -> sw s0 / addiu s0, ...  (s0 saved second)
```

Swap them to get:
```c
s32 ptr = (s32)SYM;  // -> sw s0 / addiu s0, ...  (s0 saved first = ascending)
s32 i = 0;           // -> sw s1 / move s1, $0
```

Use this to match scrambled prologue save orders in the original.

*Example: func_80012870*

### Operand order in addu

The order of operands in C expressions controls which register is rs vs rt in the
generated `addu`:

```c
*(u8 *)(i + base)    // -> addu rd, i_reg, base_reg  (i as rs)
*(u8 *)(base + i)    // -> addu rd, base_reg, i_reg  (base as rs)
```

Check the original's `addu` encoding to pick the right C expression order.

### Struct field access vs raw pointer casts

The compiler treats struct field access and raw pointer casts differently for
scheduling and aliasing:

```c
// Raw pointer: scheduler freely reorders stores before loads
*(u16 *)(base + 0xD46) = val;

// Struct field: different RTL, may prevent reorder
buf->field_D46 = val;
```

**Struct aliasing is double-edged:**
- Can HELP: prevents unwanted store-before-load reordering (func_800119D4)
- Can HURT: allows unwanted pointer caching across stores (func_80011870, where
  the original recomputed `member * 612` four times per iteration because raw
  pointer stores couldn't be proved non-aliasing)

**General principle:** When load-store ordering doesn't match, try replacing raw
pointer casts with struct field access (or vice versa).

### Root struct at symbol base with padding

To access fields at large offsets from a symbol (e.g. 0xD40 into D_80077378):

```c
typedef struct {
    u8 pad0[0xD40];
    s16 positions_x[4];
    // ...
} SnapshotBuf;

s32 base = (s32)D_80077378;  // (s32) cast on a LOCAL prevents symbol+constant folding
SnapshotBuf *buf = (SnapshotBuf *)base;
buf->positions_x[0] = val;
```

*Example: func_800119D4*

---

## Preventing Compiler Optimizations

### Symbol+constant folding

CC1PSX folds `symbol + constant` into `%hi(symbol+const)/%lo(symbol+const)`:

```c
// BAD: compiler folds into single lui/lw with adjusted offset
*(s32 *)(D_80077378 + 0xD40) = val;

// GOOD: (s32) cast on a temp variable prevents folding
s32 base = (s32)D_80077378;
*(s32 *)(base + 0xD40) = val;
```

**Critical:** `(s32)D_80077378` inline does NOT prevent folding. Must be a separate
local variable assignment.

### Offset folding

The compiler folds `constant + N` into memory operation immediates:

```c
// Compiler folds: lhu v0, 2(a1) (one instruction)
v1 = a1 + 2;
v0 = *(u16 *)v1;

// Pointer arithmetic prevents: addiu + lhu (two instructions)
u8 *entry = D_SYM + a0 * stride;
*(s32 *)(entry + 4) = val;  // offset stays as immediate
```

*Example: func_801F08AC, func_8002ACD8*

### Separated index increment prevents folding

```c
// BAD: compiler folds into lhu with offset 2
return table[a0 + 1];

// GOOD: separate addiu + lhu
a0 += 1;
return table[a0] + (s32)table;
```

### Pointer arithmetic prevents offset folding AND fixes instruction ordering

```c
u8 *entry = D_SYM + idx * stride;
```

This produces `sll` (multiply) BEFORE `lui/addiu` (base address load), and chains
additions through the multiply result register. Unlike `(s32)` cast approach which
puts `lui/addiu` first.

For 3-operand case (idx*stride + base + offset):
```c
u8 *entry = D_SYM + idx * stride;
*(u8 *)(entry + offset + const) = val;  // keeps offset addition separate
```

*Example: func_8002ACD8, func_8002C9F0*

### for-loop with i*stride prevents base-offset pre-addition

```c
// BAD: compiler pre-adds 0x82 to base, uses offsets 0-4
do { *(u8 *)(base + 0x84) = 0; base += 5; } while (...);

// GOOD: strength-reduced induction var with correct base and large offsets
for (i = 0; i < N; i++) {
    *(u8 *)(a0 + i * 5 + 0x84) = 0;
}
```

*Example: func_80022D78*

### Implicit pointer-to-int (no cast) keeps same register

```c
// With (s32) cast: addiu $4, $2, %lo(...)  (different src/dst registers)
s32 x = (s32)ARRAY_SYM;

// Without cast: addiu $2, $2, %lo(...)  (same src/dst register)
s32 x = ARRAY_SYM;  // triggers warning but matches original
```

Check the original's `addiu` encoding: if src and dst are the same register, omit
the cast.

*Example: func_800128F8*

---

## Register Allocation

### Declaration order controls register numbering

First-declared local gets the lowest available register:

```c
s32 a = expr1;  // -> $2 (v0)
s32 b = expr2;  // -> $3 (v1)
s32 c = expr3;  // -> $4 (a0)
```

Match declaration order to desired register numbering.

### Don't reuse variables across jal for different register targets

If pre-jal value is in $v1 and post-jal temp is in $a1, they MUST be separate C
variables. Reusing the same C variable makes GCC allocate one register for both
lifetimes:

```c
// BAD: compiler uses one register for both uses
s32 temp = pre_jal_value;
func();
temp = post_jal_value;

// GOOD: separate variables, separate registers
s32 val = pre_jal_value;
func();
s32 temp = post_jal_value;
```

*Example: func_801F0038*

### ANSI narrow param types scramble save order

```c
void f(s32 a0, u8 a1)  // ANSI style with u8
```

Makes the compiler handle u8 truncation (andi) early in the prologue, disrupting
the default ascending save order. Can produce scrambled orders like s0, s2, s1
matching the original.

*Example: func_80028A98, func_80028AF8*

### volatile forces reloads but doesn't control register choice

`volatile u8 *` forces the compiler to reload on every access, but it picks its own
register and scheduling. Don't use volatile to control which register — use
declaration order and `REGALLOC_BARRIER` instead.

### Register allocation diagnostic checklist

For near-match functions where registers are wrong:

1. Split the original asm at jal boundaries — pre-jal and post-jal are allocated
   independently (jal clobbers $2-$15, $24, $25)
2. Map each original register to a C variable
3. If a C variable is used both pre-jal and post-jal, the compiler assigns ONE
   register for its entire lifetime — check whether the original uses different
   registers; if so, split into separate C variables
4. Match declaration order to desired register numbering
5. Check `addiu` src/dst: same register -> no cast on init; different -> `(s32)` cast
6. REGALLOC_BARRIER/volatile do NOT control WHICH register, only liveness/reload

---

## GPU / PsyQ Patterns

### addPrim / OT linking (P_TAG bitfield struct)

Must use PsyQ P_TAG bitfield struct, NOT raw 0xFF000000/0x00FFFFFF masking. The
bitfield approach produces correct register allocation; raw masks produce swapped
registers.

```c
typedef struct {
    unsigned addr:24;
    unsigned len:8;
    u8 r0, g0, b0, code;
} P_TAG;

#define setaddr(p, a) (((P_TAG *)(p))->addr = (u32)(a))
#define getaddr(p)    (u32)(((P_TAG *)(p))->addr)
#define addPrim(ot, p) setaddr(p, getaddr(ot)), setaddr(ot, p)
```

Verified matching in func_800115F0. All PS1 decomps (9 projects, 10K+ functions)
use this pattern.

### Stack struct via s32 buf[2]

For 8-byte stack structs (s32 + 2x u16):

```c
s32 buf[2];
*(u16 *)((u8 *)buf + 4) = value;  // access sub-word fields
```

---

## Loop Patterns

### goto-based loops prevent LICM and loop inversion

`while`/`do-while`/`for` loops trigger Loop-Invariant Code Motion (LICM), which
hoists constants into s-regs and inflates register pressure. goto-based loops are
not recognized as loops by GCC 2.7.2:

```c
// BAD: compiler hoists constants, uses extra s-regs
while (i < N) { *(u8 *)(base + CONST) = val; i++; }

// GOOD: no LICM, no loop inversion
top:
    *(u8 *)(base + CONST) = val;
    if (++i < N) goto top;
```

Sentinel loops:
```c
top:
    if (*p == sentinel) goto out;
    p += 2;
    count++;
    goto top;
out:
```

**Note on `++i` vs `i++`:** `++i` inside the conditional keeps the increment after
stores. Separate `i++` lets the compiler schedule it before stores.

*Example: func_80012870*

### for-loop fixes i++ scheduling

```c
// do-while: compiler moves i++ before stores
do { stores...; i++; } while (i < N);

// for-loop: i++ stays after stores
for (i = 0; i < N; i++) { stores...; }
```

Both generate `slti + bnez` loop termination.

### do { } while (0) vs do { break; } while (1)

See [s-reg-allocation.md](s-reg-allocation.md) for how these affect register
allocation. Beyond allocation, `do { } while (0)` is the standard multi-statement
macro idiom and is the most likely original source for inline computation blocks.

---

## Known Non-Matching Patterns

These are compiler behaviors we cannot reproduce from C and must leave as INCLUDE_ASM:

| Pattern | Description | Example |
|---------|-------------|---------|
| Trailing nop padding | .s files with nops after `endlabel` lose those bytes when decomped to C | func_8003BC0C |
| Dual jr $ra | splat merges adjacent small functions into one | func_80012ED4 |
| aspsx post-branch delay | aspsx moves post-branch instructions into delay slots; maspsx can't | func_801F0BB0 |
| CC1PSX branch delay fill | CC1PSX fills branch delay with fall-through computation, avoiding clobber | func_8001167C |
| Frame pointer ($fp) | Some functions use `addu fp, sp`; -O2 enables -fomit-frame-pointer | func_80026E20 |
| LUI hoisting in loops | -G0 splits lui/lw across registers and hoists lui out of loops | various |
| Cross-jumping optimization | gcc merges switch case bodies sharing the same suffix | func_801F0274 |

### Separate OR ops for bit-expansion

The compiler keeps individual OR operations separate (won't combine into a single
constant):

```c
result |= 0x8000;
result |= mask;
result |= 0x20000;
result |= 0x40000;
// -> individual ori/or/lui+or instructions
```

### Power-of-2 mask != 0

CC1PSX optimizes `(x & power_of_2) != 0` to `srl + andi`. Fix by splitting:

```c
// BAD: compiler optimizes to srl + andi
f((x & mask) != 0);

// GOOD: separate temp produces andi + sltu
s32 val = x & mask;
f(val != 0);
```

### volatile patterns

- `volatile u16` for globals forces `lhu + sll + sra` sign extension and prevents CSE
- `volatile u8` forces reload on every access
- Scope `s32` variables to if-blocks to control register allocation in else-path

*Example: func_8001167C — D_8005F14C, D_8005F114, D_8005F116*
