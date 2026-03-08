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

| Source File | Toolchain | C Functions | INCLUDE\_ASM | Total | % Decomped |
|-------------|-----------|-------------|-------------|-------|------------|
| `src/1D2C.c` | PsyQ 4.1 | 12 | 5 | 17 | 70.6% |
| `src/34C8.c` | PsyQ 4.3 | 67 | 12 | 79 | 84.8% |
| `src/4EF0.c` | PsyQ 4.3 | 26 | 3 | 29 | 89.7% |
| `src/57FC.c` | PsyQ 4.3 | 5 | 12 | 17 | 29.4% |
| `src/75B4.c` | PsyQ 4.3 | 11 | 36 | 47 | 23.4% |
| `src/A118.c` | PsyQ 4.3 | 11 | 11 | 22 | 50.0% |
| `src/AD5C.c` | PsyQ 4.3 | 2 | 11 | 13 | 15.4% |
| `src/C9A8.c` | PsyQ 4.3 | 87 | 52 | 139 | 62.6% |
| `src/10DD0.c` | PsyQ 4.1 | 28 | 5 | 33 | 84.8% |
| `src/11D3C.c` | PsyQ 4.1 | 18 | 8 | 26 | 69.2% |
| `src/ability.c` | PsyQ 4.1 | 3 | 0 | 3 | 100.0% |
| `src/131FC.c` | PsyQ 4.1 | 10 | 6 | 16 | 62.5% |
| `src/14100.c` | PsyQ 4.1 | 4 | 9 | 13 | 30.8% |
| `src/16120.c` | PsyQ 4.1 | 0 | 7 | 7 | 0.0% |
| `src/172DC.c` | PsyQ 4.1 | 12 | 24 | 36 | 33.3% |
| `src/186F8.c` | PsyQ 4.1 | 70 | 49 | 119 | 58.8% |
| `src/1B630.c` | PsyQ 4.1 | 11 | 2 | 13 | 84.6% |
| `src/1C2A0.c` | PsyQ 4.1 | 14 | 9 | 23 | 60.9% |
| `src/1D0A4.c` | PsyQ 4.1 | 32 | 28 | 60 | 53.3% |
| `src/1FA3C.c` | PsyQ 4.1 | 10 | 8 | 18 | 55.6% |
| `src/20724.c` | PsyQ 4.1 | 23 | 34 | 57 | 40.4% |
| `src/22A8C.c` | PsyQ 4.1 | 2 | 5 | 7 | 28.6% |
| `src/238F4.c` | PsyQ 4.1 | 6 | 21 | 27 | 22.2% |
| `src/25A94.c` | PsyQ 4.1 | 14 | 3 | 17 | 82.4% |
| `src/264E0.c` | PsyQ 4.1 | 17 | 6 | 23 | 73.9% |
| `src/26EE8.c` | PsyQ 4.1 | 5 | 9 | 14 | 35.7% |
| `src/278AC.c` | PsyQ 4.1 | 19 | 25 | 44 | 43.2% |
| `src/28DB0.c` | PsyQ 4.1 | 17 | 5 | 22 | 77.3% |
| `src/2953C.c` | PsyQ 4.1 | 16 | 52 | 68 | 23.5% |
| `src/2C378.c` | PsyQ 4.1 | 6 | 15 | 21 | 28.6% |
| `src/ovl/menumain/menumain.c` | PsyQ 4.1 | 132 | 84 | 216 | 61.1% |
| `src/ovl/menucfg/menucfg.c` | PsyQ 4.1 | 0 | 18 | 18 | 0.0% |
| `src/ovl/menupty/menupty.c` | PsyQ 4.1 | 0 | 49 | 49 | 0.0% |
| `src/ovl/menusts/menusts.c` | PsyQ 4.1 | 0 | 44 | 44 | 0.0% |
| `src/ovl/menuabl/menuabl.c` | PsyQ 4.1 | 0 | 17 | 17 | 0.0% |
| `src/ovl/menushop/menushop.c` | PsyQ 4.1 | 1 | 53 | 54 | 1.9% |
| `src/ovl/menuext/menuext.c` | PsyQ 4.1 | 0 | 47 | 47 | 0.0% |
| `src/ovl/menuitem/menuitem.c` | PsyQ 4.1 | 0 | 99 | 99 | 0.0% |
| `src/ovl/menumgc/menumgc.c` | PsyQ 4.1 | 0 | 51 | 51 | 0.0% |
| `src/ovl/menugf/menugf.c` | PsyQ 4.1 | 0 | 22 | 22 | 0.0% |
| `src/ovl/menujnc2/menujnc2.c` | PsyQ 4.1 | 0 | 86 | 86 | 0.0% |
| `src/ovl/menusav/menusav.c` | PsyQ 4.1 | 0 | 124 | 124 | 0.0% |
| `src/ovl/menucrd/menucrd.c` | PsyQ 4.1 | 14 | 12 | 26 | 53.8% |
| `src/ovl/menututo/menututo.c` | PsyQ 4.1 | 0 | 28 | 28 | 0.0% |
| `src/ovl/menutmag/menutmag.c` | PsyQ 4.1 | 0 | 15 | 15 | 0.0% |
| `src/ovl/menutips/menutips.c` | PsyQ 4.1 | 0 | 16 | 16 | 0.0% |
| `src/ovl/menutest/menutest.c` | PsyQ 4.1 | 10 | 2 | 12 | 83.3% |
| `src/ovl/field_init/field_init.c` | PsyQ 4.1 | 0 | 9 | 9 | 0.0% |
| `src/ovl/display_init/display_init.c` | PsyQ 4.1 | 0 | 11 | 11 | 0.0% |
| `src/ovl/field_engine/field_engine.c` | PsyQ 4.1 | 0 | 601 | 601 | 0.0% |
| `src/ovl/battle_engine/battle_engine.c` | PsyQ 4.1 | 0 | 172 | 172 | 0.0% |
| `src/ovl/battle_render/battle_render.c` | PsyQ 4.1 | 0 | 21 | 21 | 0.0% |
| `src/ovl/battle_code/battle_code.c` | PsyQ 4.1 | 0 | 1 | 1 | 0.0% |
| `src/ovl/field_engine_alt/field_engine_alt.c` | PsyQ 4.1 | 0 | 350 | 350 | 0.0% |
| **Total (game code)** | | **655** | **2454** | **3109** | **21.1%** |

PsyQ SDK libraries (`src/psxsdk/`, 751 functions) are not tracked — they are third-party code.

### Dynamic Code Overlays

All dynamic overlays load to `0x80098000`, overwriting each other. They are
extracted from the disc with LZSS decompression where needed:

| Overlay | Disc Entry | Size | Functions | Compression |
|---------|-----------|------|-----------|-------------|
| field\_init | 0 | 2.4 KB | 9 | raw |
| display\_init | 1 | 4.7 KB | 11 | LZSS |
| field\_engine | 2 | 187 KB | 601 | LZSS |
| battle\_engine | 23 | 1.0 MB | 172 | LZSS |
| battle\_render | 24 | 78 KB | 21 | raw |
| battle\_code | 25 | 339 KB | 1* | raw |
| field\_engine\_alt | 26 | 193 KB | 350 | LZSS |

\*battle\_code needs function addresses in `config/symbol_addrs.battle_code.txt`
for splat to split the binary into individual functions.

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
