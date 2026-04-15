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

3. **Provide your own disc image** — You need a Redump-verified BIN/CUE of
   FF8 Disc 1 (USA, SLUS-00892).

4. **Extract game data from the disc**:
   ```bash
   python3 tools/extract.py /path/to/ff8-disc1.bin
   ```
   This verifies the disc SHA1, then extracts `SLUS_008.92`, all menu
   overlays, and code overlays (with LZSS decompression where needed)
   to `original/`. The extracted `SLUS_008.92` SHA1 is also verified.

5. **Run splat** to split the executable and overlays into assembly:
   ```bash
   make split
   ```

6. **Build and verify**:
   ```bash
   make verify
   ```
   This assembles, links, and checks that the output matches the original SHA1.

PsyQ SDK libraries (`src/psxsdk/`, ~750 functions) are not tracked — they are third-party code.
