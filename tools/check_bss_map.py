#!/usr/bin/env python3
"""Verify the tripletriad BSS layout: every symbol the overlay used to pin at an
absolute address must land at exactly that address in the linker map."""
import re, sys

GROUND_TRUTH = {}
for line in open(sys.argv[1] if len(sys.argv) > 1 else
                 "config/expected_bss.tripletriad.txt"):
    m = re.match(r"\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+)\s*;", line)
    if m:
        a = int(m.group(2), 16)
        if 0x801A2C40 <= a < 0x801E0000:
            GROUND_TRUTH[m.group(1)] = a

mapsyms = {}
for line in open("build/ovl/tripletriad/tripletriad.map"):
    m = re.match(r"\s+0x([0-9a-f]{8,16})\s+(\w+)(\s*=.*)?\s*$", line)
    if m: mapsyms[m.group(2)] = int(m.group(1), 16) & 0xFFFFFFFF

bad = ok = 0
for name, want in sorted(GROUND_TRUTH.items(), key=lambda kv: kv[1]):
    got = mapsyms.get(name)
    if got is None:
        print(f"MISSING {name} (want 0x{want:08X})"); bad += 1
    elif got != want:
        print(f"WRONG   {name}: want 0x{want:08X} got 0x{got:08X} (delta {got-want:+#x})"); bad += 1
    else: ok += 1
print(f"{ok} symbols correct, {bad} wrong/missing")
sys.exit(1 if bad else 0)
