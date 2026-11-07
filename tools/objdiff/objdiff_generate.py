#!/usr/bin/env python3
"""Generate objdiff.json for ff8-decomp progress tracking.

Maps each compiled .o file (base) to its expected target .o file,
categorized by binary (main exe or overlay).
"""

import argparse
import json
import os
import re
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
    {"id": "movie", "name": "movie.ovl"},
    {"id": "tripletriad", "name": "tripletriad.bin"},
    {"id": "battle_render", "name": "battle_render.bin"},
    {"id": "battle", "name": "battle.bin"},
    {"id": "world", "name": "world.bin"},
]

# Files/dirs to skip
IGNORED = {"header.o", "asm"}
# SDK libraries — third-party code, not tracked for progress
SDK_DIRS = {"psxsdk"}


USE_EXPECTED = False


def target_path(expected_o, o_file):
    """The built object itself -- INCLUDE_ASM's .NON_MATCHING aliases are what
    the report counts, so it needs no other target -- or, for the objdiff GUI
    (--expected, set by `make expected`), the original code in expected/."""
    if USE_EXPECTED and expected_o.exists():
        return str(expected_o.relative_to(ROOT))
    return str(o_file.relative_to(ROOT))


def effect_categories():
    """A category per effect overlay the build produced. The splat config
    carries all 343 of them but the Makefile's EFFECTS list decides which ones
    compile, so ask the build tree rather than the config -- a category with no
    objects behind it would report an empty binary."""
    names = []
    for parent in (BUILD, BUILD / "ovl"):
        for ovl_dir in sorted(parent.glob("effect_*")):
            if (ovl_dir / "src").is_dir() and ovl_dir.name not in names:
                names.append(ovl_dir.name)
    return [{"id": name, "name": f"{name}.bin"} for name in names]


def unit_name(ovl_name, rel_from_ovl):
    """The object's path under the overlay's src_path, so a unit in a
    subdirectory keeps it: effect/lib/entity.o is lib/entity, not entity."""
    src_path = f"src/{ovl_name}"
    yaml_path = ROOT / "build" / "splat" / f"{ovl_name}.yaml"
    if yaml_path.exists():
        m = re.search(r"^\s*src_path:\s*(\S+)", yaml_path.read_text(), re.M)
        if m:
            src_path = m.group(1)
    stem = str(rel_from_ovl)[:-2]
    prefix = src_path.rstrip("/") + "/"
    if stem.startswith(prefix):
        return stem[len(prefix):]
    return Path(stem).name


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
                "target_path": target_path(expected_o, o_file),
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
                        "target_path": target_path(expected_o, o_file),
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
            units.append({
                "name": f"ovl/{ovl_name}/{unit_name(ovl_name, rel_from_ovl)}",
                "target_path": target_path(expected_o, o_file),
                "base_path": str(o_file.relative_to(ROOT)),
                "metadata": {"progress_categories": [ovl_name]},
            })

    return units


def main():
    global USE_EXPECTED
    ap = argparse.ArgumentParser(description="Generate objdiff.json.")
    ap.add_argument("--expected", action="store_true",
                    help="point targets at the objects `make expected` built")
    USE_EXPECTED = ap.parse_args().expected
    CATEGORIES.extend(effect_categories())
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