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

*Example: InitClearTiles*

### Operand order in addu

The order of operands in C expressions controls which register is rs vs rt in the
generated `addu`:

```c
*(u8 *)(i + base)    // -> addu rd, i_reg, base_reg  (i as rs)
*(u8 *)(base + i)    // -> addu rd, base_reg, i_reg  (base as rs)
```

Check the original's `addu` encoding to pick the right C expression order.

### Explicit intermediate pointer controls addu operand order

When accessing `entity[slot + offset]`, the compiler may put the index register first
in the `addu`. Creating an explicit intermediate pointer forces pointer-first order:

```c
// May produce: addu s1, v0, s2  (slot first)
*(entity + slot)[0x24] = val;

// Produces: addu s1, s2, v0  (entity first)
u8 *slotEntry = entity + slot;
slotEntry[0x24] = val;
```

The intermediate variable makes the pointer addition explicit, and the compiler puts
the base pointer register as rs in the resulting `addu`.

*Example: AddAbilityExp*

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

### Scoped base loads prevent cross-branch CSE

When a function has multiple if-return branches all loading the same symbol, the
compiler may hoist the symbol load to the top (CSE). Use scoped local variables to
create independent loads per branch:

```c
// BAD: compiler hoists D_80078E00 load to function entry
s32 base = (s32)D_80078E00;
if (id < 0x14) {
    return *(u8 *)(base + id * 8 + 0x40E4);
}
// ... more branches using base ...

// GOOD: each branch loads independently, scheduler shares lui via delay slots
if (id < 0x14) {
    s32 b = D_80078E00;
    return *(u8 *)(b + id * 8 + 0x40E4);
}
idx = id - 0x14;
if (idx < 0x13U) {
    s32 b = D_80078E00;
    return *(u8 *)(b + idx * 8 + 0x4184);
}
```

The compiler naturally shares the `lui` across adjacent branch pairs via delay slots,
matching the original's pattern of `lui` in branch delay slots.

*Example: GetAbilityCap*

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

### Declaration order controls register numbering AND instruction order

First-declared local gets the lowest available register, AND the initialization
order controls instruction emission:

```c
s32 a = expr1;  // -> $2 (v0), emitted first
s32 b = expr2;  // -> $3 (v1), emitted second
s32 c = expr3;  // -> $4 (a0), emitted third
```

**Critical:** Declaration+initialization controls BOTH properties simultaneously and
they cannot be separated. Splitting declaration from initialization (e.g.
`s32 a; s32 b; a = expr1; b = expr2;`) changes register allocation entirely.

Match declaration order to desired register numbering.

### REGALLOC_BARRIER on parameter changes other variables' allocation

When declaration order gives the right init order but wrong registers (or vice
versa), adding `REGALLOC_BARRIER(param)` before a parameter's first use can change
the interference graph enough to shift other variables' register assignments:

```c
// Problem: i first → correct init order, wrong register ($a2)
//          off first → correct register ($v1), wrong init order
s32 i = 0;
s32 base = (s32)D_80078E00;
entityIdx *= 132;  // i=$a2, base=$a3 — wrong

// Solution: barrier on param changes interference graph
s32 i = 0;
s32 base = (s32)D_80078E00;
REGALLOC_BARRIER(entityIdx);  // increases entityIdx priority
entityIdx *= 132;  // i=$v1, base=$a2 — correct!
```

The barrier creates an extra use+def of the parameter, changing its priority in the
graph coloring allocator. This ripples through the interference graph and shifts
which registers are assigned to other variables.

Also works well with parameter reuse — modifying a parameter in-place keeps it in
its original register naturally:

```c
entityIdx *= 132;  // stays in $a0, no new variable needed
```

*Example: FindAbilitySlot*

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

### "Through $v0" address copy pattern

The original compiler sometimes loads a symbol address into `$v0` with same-register
`addiu` (`lui $v0; addiu $v0,$v0,%lo`), then copies to the target register
(`addu $t1,$v0,$zero`). Our compiler targets the final register directly
(`lui $v1; addiu $t0,$v1,%lo`), saving an instruction but not matching.

Fix: use `register s32 tmp asm("$2")` to force the address into `$v0`, then copy
to the real variable. A `REGALLOC_BARRIER` between the two prevents the compiler
from merging them:

```c
register s32 tmp asm("$2");
s32 buf;

tmp = (s32)g_gameState;
REGALLOC_BARRIER(tmp);
buf = tmp;
```

If `tmp` is used between the load and the copy (e.g. for a store), no barrier is
needed — the use prevents merging naturally:

```c
tmp = (s32)g_fieldEntity;
*(u16 *)(buf + 0xD44) = *(u16 *)(tmp + 0x120);  // use of tmp prevents merging
src = tmp;
```

*Example: func_80011870 pre-loop address setup*

### REGALLOC_BARRIER increases variable priority

GCC 2.7.2's graph coloring allocator assigns lower register numbers to higher-priority
variables (by reference count / live range). Adding `REGALLOC_BARRIER(var)` inside a
loop body creates an extra use+def, increasing the variable's reference count and
giving it a lower register number.

```c
i = 0;
REGALLOC_BARRIER(i);  // +1 reference count outside loop

top:
    /* loop body */
    i++;
    REGALLOC_BARRIER(i);  // +1 reference count per iteration
    if ((s16)i < 3) goto top;
```

Without barriers, `i` might get `$t1` (higher) while `buf` gets `$t0` (lower).
With barriers, `i` gets higher priority → `$t0`, pushing `buf` to `$t1`.

**Caveat:** Each `REGALLOC_BARRIER` in a goto-based loop generates `#APP`/`#NO_APP`
directives, which maspsx treats as load delay boundaries, potentially inserting extra
`nop` instructions. Only use when the register swap is essential.

*Example: func_80011870 loop counter allocation*

### Separate local for computation order

Declaring a separate local variable for a sub-expression controls when the compiler
evaluates it relative to other computations:

```c
// entity offset computed BEFORE stp (matches original addu before sll)
s16 idx = (s16)i;
s32 eoff = idx + src;     // addu first
s32 stp = idx * 2 + buf;  // sll second
```

Without the separate `eoff` variable, the compiler may evaluate `idx * 2 + buf`
first, producing `sll` before `addu`.

*Example: func_80011870 loop body*

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

### goto-based loops prevent LICM, loop inversion, LUI hoisting, and pre-shifting

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

**Additional benefits of goto-based loops (with -G0):**

- **Prevents LUI hoisting:** With `-G0`, recognized loops cause the compiler to split
  `lui $reg, %hi(sym)` / `lw $reg, %lo(sym)($reg)` across registers — hoisting the
  `lui` out of the loop. goto-based loops keep both `lui` and `lw` inside the loop
  body (same register).

- **Prevents loop counter pre-shifting:** Recognized loops may store the loop counter
  pre-shifted by 16, replacing `sll + sra` sign extension with just `sra`. goto loops
  preserve the correct `sll / sra` pattern and `addiu $t0, $t0, 1` increment.
  `REGALLOC_BARRIER(i)` after the increment also prevents pre-shifting, but adds
  a nop from maspsx.

*Example: InitClearTiles, func_80011870*

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
| u8 return type causes caller andi | Function returning `u8` inserts `andi v0, v0, 0xff` in callers; use `s32` return when `lbu` already zero-extends | GetAbilityCap |
| Increment through temp reg | Original uses `addiu $a0,$t0,1; addu $t0,$a0,$zero` (2 insns); compiler produces direct `addiu $t0,$t0,1` (1 insn) | func_80011870 |
| Loop body temp reg rotation | All 4 loop temp registers ($a0-$a3) consistently shifted from original, regardless of C-level tricks | func_80011870 |
| Struct caches entity index | Struct field access lets compiler prove non-aliasing, caching a `lbu` across 4 uses (1 load vs 4); raw pointers preserve per-use reload | func_80011870 |

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

*Example: func_8001167C — D_8005F14C, g_bufferIndex, g_fadeMode*
