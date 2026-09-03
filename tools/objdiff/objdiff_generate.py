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
# Overlay source .o files pattern: build/<name>/src/.../<name>.o

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
    {"id": "intro", "name": "intro.bin"},
    {"id": "field", "name": "field.bin"},
    {"id": "tripletriad", "name": "tripletriad.bin"},
    {"id": "battle_render", "name": "battle_render.bin"},
    {"id": "battle", "name": "battle.bin"},
    {"id": "world", "name": "world.bin"},
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
                "target_path": str(expected_o.relative_to(ROOT)),
                "base_path": str(o_file.relative_to(ROOT)),
                "metadata": {"progress_categories": ["main"]},
            })

        # Subdirs (skip SDK libraries)
        for subdir in sorted(MAIN_SRC_DIR.iterdir()):
            if subdir.is_dir() and subdir.name not in SDK_DIRS:
                for o_file in sorted(subdir.glob("*.o")):
                    rel = o_file.relative_to(MAIN_SRC_DIR)
                    expected_o = EXPECTED / "build" / "src" / rel
                    rel_str = str(rel).replace('.o', '')
                    units.append({
                        "name": f"src/{rel_str}",
                        "target_path": str(expected_o.relative_to(ROOT)),
                        "base_path": str(o_file.relative_to(ROOT)),
                        "metadata": {"progress_categories": ["main"]},
                    })

    # Overlay .o files
    for cat in CATEGORIES:
        ovl_name = cat["id"]
        if ovl_name == "main":
            continue
        for candidate in (BUILD / ovl_name, BUILD / "ovl" / ovl_name):
            src_root = candidate / "src"
            if src_root.exists():
                ovl_dir = candidate
                break
        else:
            continue
        for o_file in sorted(src_root.rglob("*.o")):
            rel_from_ovl = o_file.relative_to(ovl_dir)
            expected_o = EXPECTED / ovl_dir.relative_to(ROOT) / rel_from_ovl
            src_rel = str(rel_from_ovl).replace(".o", "")
            units.append({
                "name": f"ovl/{ovl_name}/{Path(src_rel).name}",
                "target_path": str(expected_o.relative_to(ROOT)),
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