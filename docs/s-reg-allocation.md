# S-Reg Allocation Tricks

CC1PSX (GCC 2.7.2) uses a graph coloring register allocator that assigns callee-saved
registers (s0-s7) based on variable liveness and interference. The original binary often
has "scrambled" prologues where s-regs are saved/initialized in non-ascending order.
Getting the compiler to reproduce the exact same assignment requires manipulating the
interference graph through C source structure.

This document covers portable techniques discovered while decomping ff8-decomp functions.

## Background

The default allocation follows parameter order: first param -> s0, second -> s1, etc.
The prologue interleaves saves with initializations in declaration order:

```
sw s0, 0x10(sp)   # save s0
move s0, a0        # state = a0
sw s1, 0x14(sp)   # save s1
move s1, a1        # disp = a1
sw ra, 0x20(sp)
...
```

When the original has a different order (e.g. s1 saved first, or s0 initialized in
the jal delay slot), we need to change how the compiler assigns registers.

## Technique 1: `+ var - var` (extend liveness across a jal)

### Problem

Variable `ot` (third parameter) gets assigned s2, but the original has it in s0.
The allocator picks s0 for the first live-across-jal variable, s1 for the second, etc.

### Solution

Reference `ot` in the first jal's argument expression:

```c
// Before: ot gets s2 (assigned after state=s0, disp=s1)
v0 = func_8002E680(*(s32 *)(state + 0x20));

// After: ot gets s0 (now live BEFORE the jal, gets first s-reg)
v0 = func_8002E680(ot + *(s32 *)(state + 0x20) - ot);
```

### Why it works

- `+ var - var` (or `var + expr - var`) makes `var` live at the point of the jal call
- The compiler must preserve it in a callee-saved register across the jal
- Since `ot` is now the first variable needing preservation, it gets the lowest s-reg
- The arithmetic is completely optimized away by the compiler
  (zero extra instructions in the output)

### Equivalent forms

All of these produce identical output:

| Form | Notes |
|------|-------|
| `expr + ot - ot` | Decomp convention |
| `ot + expr - ot` | Most natural — looks like a developer typo/correction |
| `expr - ot + ot` | Reversed subtraction order |
| `*(s32 *)(addr + ot - ot)` | Inside pointer offset |
| `expr + a2 - a2` | Using raw parameter name instead of local variable |

These do NOT work (compiler optimizes away the reference before register allocation):

| Form | Why it fails |
|------|-------------|
| `ot * 0` | Constant-folded to 0 at compile time |
| `ot ^ ot` | Constant-folded to 0 at compile time |
| `ot & 0` | Constant-folded to 0 at compile time |
| `(ot, expr)` | Comma operator — no data dependency on ot |
| `(void)ot;` before call | Cast to void generates no code |
| Passing ot as extra function arg | Changes all register assignments |

### Plausible original source

The `var + expr - var` form is the most likely original: the developer started typing
`ot` (perhaps intending a different argument), realized the mistake, and subtracted it
back rather than retyping. This is a common developer "correction" pattern that happens
to affect register allocation.

### Verified

func_801E66A8 in menutest.ovl: `ot` shifted from s2 to s0, byte-match verified.

## Technique 2: `do { ... break; } while (1)` (change interference graph)

### Problem

Variable `maxW` gets assigned s0, but the original has it in s3.

### Solution

Wrap the computation in `do { ... break; } while (1)` instead of plain statements
or `do { } while (0)`:

```c
// do-while(0): maxW -> s0
do { maxW = 0x150; v0 = (maxW - v0) / 2; } while (0);

// do-break-while(1): maxW -> s3
do { maxW = 0x150; v0 = (maxW - v0) / 2; break; } while (1);
```

### Why it works

The `break` inside `do { } while (1)` creates a different control flow graph than
`do { } while (0)`. Even though both execute identically at runtime, the compiler's
register allocator sees different interference patterns in the CFG, which changes
the graph coloring result. The `break` makes the loop body a separate basic block
with an explicit exit edge, altering which variables interfere with each other.

### Loop form equivalences

| Loop form                         | CFG type  | Allocation effect |
|-----------------------------------|-----------|-------------------|
| `do { ... } while (0)`           | Type A    | maxW -> s0        |
| `do { ... break; } while (1)`    | Type B    | maxW -> s3        |
| `for (;;) { ... break; }`        | Type B    | Same as above     |
| `while (1) { ... break; }`       | Type A    | Same as do-while(0) |
| `{ ... }` (bare block)           | Type A    | Same as do-while(0) |

Note: `while(1){break}` and `do{}while(0)` produce the same CFG because the compiler
recognizes both as single-execution blocks. `do{break}while(1)` and `for(;;){break}`
produce a different CFG because the `break` is compiled as an explicit jump out of a
loop that would otherwise be infinite.

### Real-world origin

`do { } while (0)` is the standard C idiom for multi-statement macros:
```c
#define COMPUTE_CENTER(maxW, offset, width, textW) \
    do { (maxW) = (width); (offset) = ((maxW) - (textW)) / 2; } while (0)
```

The original developer likely used a macro like this. Whether it was `while(0)` or
`while(1)` with a `break` depends on the macro's structure (e.g. macros with early
exit logic use `do { ... break; } while (0)` or `do { if (...) break; ... } while (0)`).

### Verified

func_801E66A8 in menutest.ovl: `maxW` shifted from s0 to s3, byte-match verified.

## Combining the techniques

func_801E66A8 needed two s-reg swaps simultaneously:
- `ot`: s2 -> s0 (fixed with `ot + expr - ot`)
- `maxW`: s0 -> s3 (fixed with `do { break } while (1)` via `CalcCenter` macro)

Combined:
```c
#define CalcCenter(maxW, tw, w) \
    do { (maxW) = (w); (tw) = ((maxW) - (tw)) / 2; break; } while (1)

s32 func_801E66A8(s32 a0, s32 a1, s32 a2) {
    s32 state = a0;
    s32 disp = a1;
    s32 ot = a2;
    s32 maxW;
    s32 v0;
    s32 buf;

    v0 = func_8002E680(ot + *(s32 *)(state + 0x20) - ot);
    CalcCenter(maxW, v0, 0x150);
    func_8002EAD0(disp, v0 + 0x18, 0xC8, *(s32 *)(state + 0x20));
    buf = (s32)&D_801FAB00;
    *(u8 *)(buf + 0x10) = 0;
    *(u8 *)(buf + 0x11) = 0;
    *(s16 *)&D_801FAB00 = 0x18;
    *(s16 *)(buf + 2) = 0xC4;
    *(s16 *)(buf + 4) = maxW;
    *(s16 *)(buf + 6) = 0x14;
    return func_801EF9AC(disp, ot, 0x1000, D_80083848);
}
```

Result: state->s1, disp->s2, ot->s0, maxW->s3 — exact match with original.

## Approach for scrambled s-regs (checklist)

1. **Declaration/initialization order** — first-declared variable gets lowest s-reg.
   Reorder declarations to match desired allocation.

2. **`+ var - var`** — extend a variable's liveness across an earlier jal to push
   it to a lower s-reg. Compiler optimizes the arithmetic away.

3. **Loop form** — try `do{...}while(0)` vs `do{...break;}while(1)` to shift
   allocation of variables used inside the block.

4. **ANSI narrow param types** — `void f(s32 a0, u8 a1)` makes the compiler handle
   u8 truncation early, disrupting the default ascending allocation order.

5. **`register asm("$16")`** — forces a specific register. MIPS-specific, not
   portable. Use only as last resort and add a FIXME comment.

## Other techniques that DON'T change s-reg allocation

These were tested extensively and do NOT affect which s-reg a variable gets:

- Changing variable type (s32 vs u32 vs void*)
- Using struct pointer parameters instead of s32
- K&R vs ANSI function style (for same-width params)
- `volatile` keyword (changes reload behavior, not register number)
- `register` keyword without `asm` (hint only, ignored at -O2)
- Declaring variables in inner scopes
- Using `(s32)` or `(u32)` casts on initialization
- `asm("")` scheduling barriers (affect instruction order, not allocation)
