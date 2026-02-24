# ff8-decomp

A decompilation of Final Fantasy VIII (PS1, USA — `SLUS_008.92`).

## Setup

1. **Provide your own ISO** — You need a BIN/CUE of FF8 Disc 1 (USA).
2. **Extract the executable from the ISO**:
   ```bash
   python3 tools/extract_psxexe.py /path/to/ff8-disc1.bin
   ```
   This extracts `SLUS_008.92` to `original/` and verifies the SHA1.
3. **Run splat** — Split the extracted executable into assembly and data segments using [splat](https://github.com/ethteck/splat).
