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

## Setup Workflow

Before any development can happen, a contributor must:

1. **Provide your own ISO** — Place your FF8 Disc 1 ISO at a known path (e.g. `disc/ff8.bin`).
2. **Extract data from the ISO** — Run a setup step that pulls the game executable and relevant files out of the disc image.
3. **Run splat** — Use [splat](https://github.com/ethteck/splat) to split the extracted executable into assembly (.s) files and data segments, which form the baseline to decompile against.
