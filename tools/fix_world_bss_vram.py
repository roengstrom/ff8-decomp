#!/usr/bin/env python3
"""Force world BSS to VRAM 0x800C8640 so symbols match despite 11-byte short bin."""
from pathlib import Path
import sys
p = Path(sys.argv[1] if len(sys.argv) > 1 else "build/splat/world.ovl.ld")
t = p.read_text()
for align in (1, 4, 2):
    old = f"    world_bss_VRAM = ADDR(.world_bss);\n    .world_bss (NOLOAD) : SUBALIGN({align})"
    new = f"    world_bss_VRAM = 0x800C8640;\n    .world_bss 0x800C8640 (NOLOAD) : SUBALIGN({align})"
    if old in t:
        p.write_text(t.replace(old, new, 1))
        print(f"patched {p} (SUBALIGN {align})")
        break
else:
    if "0x800C8640 (NOLOAD)" in t:
        print(f"already patched {p}")
    else:
        sys.exit(f"no pattern in {p}")
