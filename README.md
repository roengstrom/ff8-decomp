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
   This verifies the disc SHA1, then extracts `SLUS_008.92`, all executables and overlays.

5. **Full build**:
   ```bash
   make full
   ```
   This runs `clean`, `split` (runs splat on the executable + overlays),
   `build-assets` (converts binary assets to C source), and `verify`
   (assembles, links, and checks that each output matches the original SHA1).

   For incremental work, the individual targets are also available:
   ```bash
   make split          # re-run splat
   make build-assets   # regenerate asset C source
   make verify         # build and compare SHA1s
   ```

