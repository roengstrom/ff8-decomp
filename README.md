# ff8-decomp

A decompilation of Final Fantasy VIII (PS1, USA — `SLUS_008.92`).

## Setup

1. **Clone the repo with submodules**:
   ```bash
   git clone --recursive https://github.com/rengstrom/ff8-decomp.git
   cd ff8-decomp
   ```

2. **Create a Python venv and install splat**:
   ```bash
   python3 -m venv .venv
   .venv/bin/pip install -e "tools/splat[mips]"
   ```

3. **Provide your own disc image** — You need a BIN/CUE of FF8 Disc 1 (USA).

4. **Extract the executable from the disc**:
   ```bash
   python3 tools/extract_psxexe.py /path/to/ff8-disc1.bin
   ```
   This extracts `SLUS_008.92` to `original/` and verifies the SHA1.

5. **Extract overlay binaries** from the disc:
   ```bash
   python3 tools/extract_overlays.py /path/to/ff8-disc1.bin
   ```
   This extracts all menu overlays, code overlays (with LZSS decompression),
   and data files to `original/`.

6. **Run splat** to split the executable and overlays into assembly:
   ```bash
   make split
   ```

7. **Build and verify**:
   ```bash
   make verify
   ```
   This assembles, links, and checks that the output matches the original SHA1.

## Progress

| Binary | C | ASM | Total | % Decomped |
|--------|---|-----|-------|------------|
| `SLUS_008.92` | 614 | 415 | 1029 | 59.7% |
| `menumain.ovl` | 115 | 84 | 199 | 57.8% |
| `menucfg.ovl` | 6 | 12 | 18 | 33.3% |
| `menupty.ovl` | 15 | 34 | 49 | 30.6% |
| `menusts.ovl` | 13 | 31 | 44 | 29.5% |
| `menuabl.ovl` | 8 | 9 | 17 | 47.1% |
| `menushop.ovl` | 12 | 42 | 54 | 22.2% |
| `menuext.ovl` | 17 | 30 | 47 | 36.2% |
| `menuitem.ovl` | 29 | 70 | 99 | 29.3% |
| `menumgc.ovl` | 9 | 42 | 51 | 17.6% |
| `menugf.ovl` | 6 | 16 | 22 | 27.3% |
| `menujnc2.ovl` | 18 | 68 | 86 | 20.9% |
| `menusav.ovl` | 54 | 70 | 124 | 43.5% |
| `menucrd.ovl` | 11 | 10 | 21 | 52.4% |
| `menututo.ovl` | 9 | 19 | 28 | 32.1% |
| `menutmag.ovl` | 8 | 7 | 15 | 53.3% |
| `menutips.ovl` | 4 | 12 | 16 | 25.0% |
| `menutest.ovl` | 9 | 2 | 11 | 81.8% |
| `field_init.bin` | 4 | 5 | 9 | 44.4% |
| `display_init.bin` | 0 | 11 | 11 | 0.0% |
| `field_engine.bin` | 59 | 625 | 684 | 8.6% |
| `battle_engine.bin` | 56 | 1608 | 1664 | 3.4% |
| `battle_render.bin` | 9 | 18 | 27 | 33.3% |
| `battle_code.bin` | 486 | 928 | 1414 | 34.4% |
| `field_engine_alt.bin` | 23 | 328 | 351 | 6.6% |
| **Total** | **1595** | **4495** | **6090** | **26.2%** |

PsyQ SDK libraries (`src/psxsdk/`, ~750 functions) are not tracked — they are third-party code.

All dynamic overlays load to `0x80098000`, overwriting each other. They are
extracted from the disc with LZSS decompression where needed.

## Project Structure

| File | Description |
|------|-------------|
| `config/` | Splat configs, symbol files, and linker scripts for all binaries |
| `asm/` | Disassembled assembly from splat |
| `src/` | Decompiled C source — game code |
| `src/psxsdk/` | PsyQ SDK library source (13 files split from game code) |
| `original/` | Extracted PS-EXE and overlay binaries (gitignored) |
| `build/` | Build output (gitignored) |
| `tools/extract_overlays.py` | Extracts all overlays from disc (with LZSS decompression) |
| `tools/analyze_delay_slots.py` | Toolchain analysis — classifies functions by PsyQ version |

## Decomping a Function

Once you've written the C implementation for a function, follow these steps to
integrate it into the build:

1. **Replace the `INCLUDE_ASM` macro** in the C source file with your C
   implementation.

2. **Update the splat yaml** (`config/splat/SLUS_008.92.yaml` for the main
   binary, or the overlay's yaml in `config/`). If your function is already
   within an existing `c` segment's address range, no yaml changes are needed.
   If you're creating a new C file, add a new `c` segment entry covering the
   function's address range.

3. **Update `config/symbol_addrs.txt`** — Ensure the function is listed with
   `type:func` so splat recognizes it as a function symbol.

4. **Add any new external symbols** — If your function references globals or
   functions not yet declared, add them to the relevant
   `undefined_*_auto.txt` file in `config/` so the linker can resolve them.

5. **Re-split and verify**:
   ```bash
   rm -rf asm && make split && make verify
   ```
   This regenerates assembly (splat will stop emitting `.s` files for
   addresses covered by C), rebuilds everything, and checks that the output
   matches the original binary SHA1. Every target must show `Match`.

## Tools

### analyze_delay_slots.py

Analyzes assembly files to classify each function's PsyQ toolchain version
(4.1 vs 4.3) based on epilogue delay slot patterns. Accepts any asm directory
as argument, so it works on both the main executable and overlays.

The key diagnostic is the `lw $ra` **load** delay slot (not the `jr $ra` branch
delay). PsyQ 4.3 (gcc 2.8.0) leaves a `nop` after `lw $ra`, while PsyQ 4.1
(gcc 2.7.2) fills it with `addiu $sp` or another useful instruction. When
multiple `lw $sN` restores fill the load delay, both toolchains produce
identical epilogues and the function is classified as AMBIGUOUS.

Only non-leaf function returns are diagnostic — leaf functions produce identical
code with both toolchains.

```bash
# Text list — main exe (default dir)
python3 tools/analyze_delay_slots.py asm/nonmatchings/

# Analyze a specific overlay
python3 tools/analyze_delay_slots.py asm/ovl/menumain/nonmatchings/menumain/

# Colored terminal map of the address space
python3 tools/analyze_delay_slots.py --map asm/nonmatchings/

# Adjust terminal width (default 160)
python3 tools/analyze_delay_slots.py --map --width 120 asm/nonmatchings/
```

Classifications:
- **FILLED** — PsyQ 4.3 (gcc 2.8.0), `nop` in `lw $ra` load delay
- **UNFILLED** — PsyQ 4.1 (gcc 2.7.2), useful instruction in `lw $ra` load delay
- **AMBIGUOUS** — non-leaf but `lw $sN` fills load delay (either toolchain)
- **MIXED** — merged block containing both filled and unfilled non-leaf returns
- **LEAF** — no non-leaf returns, toolchain indeterminate
- **NO_JR_RA** — no `jr $ra` found (data, library fragments, noreturn functions)
