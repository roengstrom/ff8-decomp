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
| `SLUS_008.92` | 626 | 419 | 1045 | 59.9% |
| `menumain.ovl` | 115 | 84 | 199 | 57.8% |
| `menucfg.ovl` | 0 | 18 | 18 | 0.0% |
| `menupty.ovl` | 0 | 49 | 49 | 0.0% |
| `menusts.ovl` | 0 | 44 | 44 | 0.0% |
| `menuabl.ovl` | 0 | 11 | 11 | 0.0% |
| `menushop.ovl` | 1 | 53 | 54 | 1.9% |
| `menuext.ovl` | 0 | 47 | 47 | 0.0% |
| `menuitem.ovl` | 0 | 99 | 99 | 0.0% |
| `menumgc.ovl` | 0 | 51 | 51 | 0.0% |
| `menugf.ovl` | 0 | 22 | 22 | 0.0% |
| `menujnc2.ovl` | 0 | 86 | 86 | 0.0% |
| `menusav.ovl` | 0 | 124 | 124 | 0.0% |
| `menucrd.ovl` | 9 | 12 | 21 | 42.9% |
| `menututo.ovl` | 0 | 28 | 28 | 0.0% |
| `menutmag.ovl` | 0 | 15 | 15 | 0.0% |
| `menutips.ovl` | 0 | 16 | 16 | 0.0% |
| `menutest.ovl` | 10 | 2 | 12 | 83.3% |
| `field_init.bin` | 0 | 7 | 7 | 0.0% |
| `display_init.bin` | 0 | 10 | 10 | 0.0% |
| `field_engine.bin` | 1 | 395 | 396 | 0.3% |
| `battle_engine.bin` | 1 | 385 | 386 | 0.3% |
| `battle_render.bin` | 1 | 15 | 16 | 6.3% |
| `battle_code.bin` | 0 | 1016 | 1016 | 0.0% |
| `field_engine_alt.bin` | 4 | 283 | 287 | 1.4% |
| **Total** | **768** | **3291** | **4059** | **18.9%** |

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

Analyzes assembly files in `asm/nonmatchings/` to classify each function's
PsyQ toolchain version (4.1 vs 4.3) based on epilogue delay slot patterns.

PsyQ 4.1 (gcc 2.7.2) leaves the `jr $ra` branch delay slot unfilled (`nop`),
while PsyQ 4.3 (gcc 2.8.0) fills it with `addiu $sp`. Only non-leaf function
returns are diagnostic — leaf functions produce identical code with both toolchains.

Merged blocks (multiple functions under one splat label) are handled by
classifying each `jr $ra` individually.

```bash
# Text list — one line per function with classification
python3 tools/analyze_delay_slots.py

# Colored terminal map of the address space
python3 tools/analyze_delay_slots.py --map

# Adjust terminal width (default 160)
python3 tools/analyze_delay_slots.py --map --width 120
```

Classifications:
- **FILLED** — PsyQ 4.3 (gcc 2.8.0), delay slot filled with `addiu $sp`
- **UNFILLED** — PsyQ 4.1 (gcc 2.7.2), delay slot is `nop`
- **MIXED** — merged block containing both filled and unfilled non-leaf returns
- **LEAF** — no non-leaf returns, toolchain indeterminate
- **NO_JR_RA** — no `jr $ra` found (data, library fragments, noreturn functions)
