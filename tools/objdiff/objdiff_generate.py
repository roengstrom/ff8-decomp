#!/usr/bin/env python3
"""Generate objdiff.json for ff8-decomp progress tracking.

Maps each compiled .o file (base) to its expected target .o file,
categorized by binary (main exe or overlay).
"""

import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
BUILD = ROOT / "build"

# Main binary source .o files
MAIN_SRC_DIR = BUILD / "src"
EXPECTED = ROOT / "expected"
# Overlay source .o files pattern: build/ovl/<name>/src/ovl/<name>/<name>.o

CATEGORIES = [
    {"id": "main", "name": "Main Executable"},
    {"id": "menumain", "name": "menumain.ovl"},
    {"id": "menucfg", "name": "menucfg.ovl"},
    {"id": "menupty", "name": "menupty.ovl"},
    {"id": "menusts", "name": "menusts.ovl"},
    {"id": "menuabl", "name": "menuabl.ovl"},
    {"id": "menushop", "name": "menushop.ovl"},
    {"id": "menuext", "name": "menuext.ovl"},
    {"id": "menuitem", "name": "menuitem.ovl"},
    {"id": "menumgc", "name": "menumgc.ovl"},
    {"id": "menugf", "name": "menugf.ovl"},
    {"id": "menujnc2", "name": "menujnc2.ovl"},
    {"id": "menusav", "name": "menusav.ovl"},
    {"id": "menucrd", "name": "menucrd.ovl"},
    {"id": "menututo", "name": "menututo.ovl"},
    {"id": "menutmag", "name": "menutmag.ovl"},
    {"id": "menutips", "name": "menutips.ovl"},
    {"id": "menutest", "name": "menutest.ovl"},
    {"id": "field_init", "name": "field_init.bin"},
    {"id": "display_init", "name": "display_init.bin"},
    {"id": "field_engine", "name": "field_engine.bin"},
    {"id": "battle_engine", "name": "battle_engine.bin"},
    {"id": "battle_render", "name": "battle_render.bin"},
    {"id": "battle_code", "name": "battle_code.bin"},
    {"id": "world_engine", "name": "world_engine.bin"},
]

# Files/dirs to skip
IGNORED = {"header.o", "asm"}
# SDK libraries — third-party code, not tracked for progress
SDK_DIRS = {"psxsdk"}


def find_units():
    units = []

    # Main binary .o files
    if MAIN_SRC_DIR.exists():
        for o_file in sorted(MAIN_SRC_DIR.glob("*.o")):
            if o_file.name in IGNORED:
                continue
            name = o_file.stem
            expected_o = EXPECTED / "build" / "src" / o_file.name
            units.append({
                "name": f"src/{name}",
                "target_path": str(expected_o.relative_to(ROOT)) if expected_o.exists() else str(o_file.relative_to(ROOT)),
                "base_path": str(o_file.relative_to(ROOT)),
                "metadata": {"progress_categories": ["main"]},
            })

        # Subdirs (skip SDK libraries)
        for subdir in sorted(MAIN_SRC_DIR.iterdir()):
            if subdir.is_dir() and subdir.name not in SDK_DIRS:
                for o_file in sorted(subdir.glob("*.o")):
                    name = o_file.stem
                    rel = str(o_file.relative_to(MAIN_SRC_DIR))
                    units.append({
                        "name": f"src/{rel.replace('.o', '')}",
                        "target_path": str(o_file.relative_to(ROOT)),
                        "base_path": str(o_file.relative_to(ROOT)),
                        "metadata": {"progress_categories": ["main"]},
                    })

    # Overlay .o files
    ovl_base = BUILD / "ovl"
    if ovl_base.exists():
        for ovl_dir in sorted(ovl_base.iterdir()):
            if not ovl_dir.is_dir():
                continue
            ovl_name = ovl_dir.name
            src_dir = ovl_dir / "src" / "ovl" / ovl_name
            if not src_dir.exists():
                continue
            for o_file in sorted(src_dir.glob("*.o")):
                name = o_file.stem
                expected_o = EXPECTED / "build" / "ovl" / ovl_name / "src" / "ovl" / ovl_name / o_file.name
                units.append({
                    "name": f"ovl/{ovl_name}/{name}",
                    "target_path": str(expected_o.relative_to(ROOT)) if expected_o.exists() else str(o_file.relative_to(ROOT)),
                    "base_path": str(o_file.relative_to(ROOT)),
                    "metadata": {"progress_categories": [ovl_name]},
                })

    return units


def main():
    units = find_units()

    config = {
        "custom_make": "make",
        "build_base": False,
        "build_target": False,
        "units": units,
        "progress_categories": CATEGORIES,
    }

    out_path = ROOT / "objdiff.json"
    with open(out_path, "w") as f:
        json.dump(config, f, indent=2)

    print(f"Generated {out_path} with {len(units)} units")


if __name__ == "__main__":
    main()
