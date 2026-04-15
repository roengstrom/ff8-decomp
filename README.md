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

PsyQ SDK libraries (`src/psxsdk/`, ~750 functions) are not tracked — they are third-party code.
