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
| `SLUS_008.92` | 610 | 419 | 1029 | 59.3% |
| `menumain.ovl` | 115 | 8 | 123 | 93.5% |
| `menucfg.ovl` | 0 | 17 | 17 | 0.0% |
| `menupty.ovl` | 0 | 42 | 42 | 0.0% |
| `menusts.ovl` | 0 | 36 | 36 | 0.0% |
| `menuabl.ovl` | 0 | 11 | 11 | 0.0% |
| `menushop.ovl` | 1 | 43 | 44 | 2.3% |
| `menuext.ovl` | 0 | 37 | 37 | 0.0% |
| `menuitem.ovl` | 0 | 80 | 80 | 0.0% |
| `menumgc.ovl` | 0 | 41 | 41 | 0.0% |
| `menugf.ovl` | 0 | 20 | 20 | 0.0% |
| `menujnc2.ovl` | 0 | 67 | 67 | 0.0% |
| `menusav.ovl` | 0 | 104 | 104 | 0.0% |
| `menucrd.ovl` | 9 | 9 | 18 | 50.0% |
| `menututo.ovl` | 0 | 22 | 22 | 0.0% |
| `menutmag.ovl` | 0 | 10 | 10 | 0.0% |
| `menutips.ovl` | 0 | 12 | 12 | 0.0% |
| `menutest.ovl` | 9 | 2 | 11 | 81.8% |
| `field_init.bin` | 0 | 7 | 7 | 0.0% |
| `display_init.bin` | 0 | 10 | 10 | 0.0% |
| `field_engine.bin` | 1 | 395 | 396 | 0.3% |
| `battle_engine.bin` | 1 | 385 | 386 | 0.3% |
| `battle_render.bin` | 1 | 15 | 16 | 6.2% |
| `battle_code.bin` | 319 | 698 | 1017 | 31.4% |
| `field_engine_alt.bin` | 4 | 283 | 287 | 1.4% |
| **Total** | **1070** | **2773** | **3843** | **27.8%** |

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
