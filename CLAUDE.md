# ff8-decomp

## Core Rules

- **No hack build steps.** The build must be a straightforward compile-and-link. Do not add extra build steps (fixup scripts, binary patching, post-processing, etc.) to work around mismatched output. If the output doesn't match, the decompiled code itself needs to be fixed — not papered over with build pipeline hacks. This is the #1 rule of the project.
- **No proprietary data in the repo.** This is an open source project. ROMs, ISOs, and any extracted proprietary content must never be committed. Users provide their own copy of FF8 Disc 1.
- **Document code with Doxygen.** When decomping or touching functions, add Doxygen-style comments (`/** ... */`) describing what the function does, its parameters, and return value. If you're not fully certain of a function's purpose, give your best estimation and note the uncertainty (e.g. `@note Purpose uncertain — appears to ...`).

## Target Binary

- **Executable**: SLUS_008.92 (USA, identical across all 4 discs)
- **SHA1**: 40706b4e0553fc6cbeb044ca1e0e9004d5ac2561
- **MD5**: 7221fbc76308ff82aeb90bcd6220c49f
- **PsyQ SDK Version**: 4.2.0
- **Architecture**: MIPS I (R3000), PS1, Little Endian, 32-bit
- **Load address**: 0x80010000
- **GP value**: 0x8005EC24
- **Functions**: ~1300 (Ghidra), ~868 (ff8decomp, game code only)
- **Instructions**: ~50602 (Ghidra)

## Decomp Tools

### permute.sh (decomp-permuter)

Automatically tries C source permutations to find a byte-matching decomp. Use this when a function is close but has register allocation, scheduling, or instruction ordering differences that are hard to solve by hand.

```bash
# Setup only (creates permuter/<func_name>/ with base.c, target.s, compile.sh):
./permute.sh --src src/ovl/menutest/menutest.c \
    --asm-dir asm/ovl/menutest/nonmatchings/menutest \
    func_801E64B4

# Setup + run (tries random permutations):
./permute.sh --run -j4 --src src/10DD0.c --asm-dir asm/nonmatchings/10DD0 func_80022B04

# PsyQ 4.3 functions:
./permute.sh --psyq 4.3 --src src/34C8.c --asm-dir asm/nonmatchings/34C8 func_80014E98
```

The permuter modifies variable ordering, casts, expression structure, etc. in `base.c` and compiles each variant, looking for one that matches `target.o`. When it finds a match (score 0), copy the winning C back into the source file.

### decomp-index (cross-project search)

Searches 9+ PS1 decompilation projects (~10K functions) for assembly patterns similar to a given function. Use this when stuck on a decomp pattern — other projects may have solved the same idiom.

```bash
# Find functions with similar assembly to a given .s file:
python3 -m decomp_index.cli --db ~/source/decomp-index/decomp.db \
    search asm/ovl/menutest/nonmatchings/menutest/func_801E64B4.s

# Show a specific function's assembly + C source:
python3 -m decomp_index.cli --db ~/source/decomp-index/decomp.db \
    show <project> <func_name>

# Find recurring instruction idioms:
python3 -m decomp_index.cli --db ~/source/decomp-index/decomp.db idioms
```

## Decomp Pattern Reference

Detailed documentation of compiler quirks, matching techniques, and known pitfalls:

- **[docs/decomp-patterns.md](docs/decomp-patterns.md)** — Scheduling, optimization prevention, register allocation, GPU/PsyQ patterns, loop patterns, and known non-matching patterns.
- **[docs/s-reg-allocation.md](docs/s-reg-allocation.md)** — S-reg allocation tricks: `+ var - var` liveness extension, `do{break}while(1)` CFG manipulation, and the full checklist for scrambled prologues.

## Decomp Methodology

- **Start from human-written code.** When decomping a new function, first write it as a human developer would have — clean, idiomatic C with proper structs, meaningful variable names, and natural control flow. Use defined types (e.g. `u8`, `s16`, `u32`) and existing structs/typedefs from project headers when appropriate, rather than plain `int` or anonymous memory access. Use this as the starting point, then iterate toward matching. Don't start from raw pointer offsets or compiler-oriented patterns.
- **Consult the pattern docs first.** Before decomping a function, read [docs/decomp-patterns.md](docs/decomp-patterns.md) and [docs/s-reg-allocation.md](docs/s-reg-allocation.md). They contain hard-won discoveries about compiler quirks, matching tricks, and known pitfalls that save hours of trial-and-error. Key topics: scheduling barriers (`asm("")`, `REGALLOC_BARRIER`), struct vs pointer aliasing, prologue interleaving, goto-based loops, P_TAG/addPrim, symbol+constant folding prevention, register allocation control, and s-reg scrambling tricks (`+var-var` liveness, `do{break}while(1)` CFG manipulation).
- **Experiment with compiler flags when stuck.** If a function doesn't match with the file's default flags, try different `-G` values (`-G0`, `-G4`, `-G8`, or no `-G` flag) and `-O` levels. The correct flags for a source file may differ from what's currently in the Makefile — the build config was reverse-engineered and may need adjustment. Test on decomp.me or locally.
- **Check the `-G` flag impact.** Without `-G0`, CC1PSX emits pseudo-instructions for global access (e.g. `sh $0, sym`) that GAS expands using `$at`. With `-G0`, CC1PSX does its own expansion using `$v0`/`$v1`. If the original uses `$at` for global stores, the function was likely compiled without `-G0`.
- **Use unsized arrays to prevent GP-relative access.** `extern u8 g_gameState[];` (unsized) forces CC1PSX to self-expand addresses with delay slot filling, matching the original 3-instruction `lui/jr/addiu` pattern. Sized globals may use GP-relative addressing depending on `-G` flag.
- **Use structs and proper data types.** Access data through struct fields rather than raw pointer offsets — this affects compiler scheduling and alias analysis, often producing different (and correct) codegen. See "Struct field access vs raw pointer casts" in decomp-patterns.md.
- **Try goto-based loops.** When `while`/`for`/`do-while` loops produce wrong code (LICM, strength reduction, base-offset folding), try `top: ...; if (cond) goto top;` which GCC 2.7.2 doesn't recognize as a loop. See "Loop Patterns" in decomp-patterns.md.
- **Don't prematurely classify functions as non-matching.** Scrambled prologues, interleaved saves, and unusual register allocation are often solvable with the right variable declaration order, assignment patterns, or casts — they just require more experimentation. func_8009B6D0 was wrongly classified as non-matching for months before a different variable assignment pattern (`new_var = arg0; temp = new_var; if (new_var < 0) temp = new_var + 0x7FF;`) produced the exact prologue interleaving. When stuck, try the permuter or decomp.me before giving up.

## Setup Workflow

Before any development can happen, a contributor must:

1. **Provide your own ISO** — Place your FF8 Disc 1 ISO at a known path (e.g. `disc/ff8.bin`).
2. **Extract data from the ISO** — Run a setup step that pulls the game executable and relevant files out of the disc image.
3. **Run splat** — Use [splat](https://github.com/ethteck/splat) to split the extracted executable into assembly (.s) files and data segments, which form the baseline to decompile against.
