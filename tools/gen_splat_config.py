#!/usr/bin/env python3
"""Expand a compact multi-binary config into one splat config per binary.

splat reads a single binary per run, so a project with N binaries needs N
configs that differ only in a handful of fields. This tool takes one file
describing those differences and writes the full configs.

The input file looks like this, and nothing here is specific to any game:

    defaults:
      name: "{name}"              # top-level keys of the generated config
      options:                    # splat options shared by every binary
        basename: "{name}"
        target_path: original/{name}.bin

    binaries:
    - name: foo                   # substituted for {name}
      sha1: 0123...
      options:                    # merged over defaults.options
        src_path: src/foo
      segments: [...]             # copied through verbatim

Every string in `defaults` and in a binary's `options` is formatted with that
binary's `name`, so shared path shapes are written once. Per-binary values win
over defaults. Paths are resolved relative to the working directory.

Usage: gen_splat_config.py CONFIG --out TEMPLATE [--make FILE] [--check]
  --out    where to write each generated config, as a path template containing
           {name}, for example build/splat/{name}.yaml
  --make   also write a make fragment defining <name>_TARGET, <name>_YAML,
           <name>_LD, <name>_ELF and <name>_DIR for each binary plus
           SPLAT_BINARIES listing them all, so a Makefile can use the same
           paths this config gives splat instead of deriving its own
  --check  regenerate and exit non-zero if any output file would change
"""
import argparse
import os
import sys

import yaml


def substitute(value, fields):
    """Format {name} through strings, lists and dicts alike."""
    if isinstance(value, str):
        return value.format(**fields)
    if isinstance(value, list):
        return [substitute(item, fields) for item in value]
    if isinstance(value, dict):
        return {key: substitute(item, fields) for key, item in value.items()}
    return value


def expand(entry, defaults):
    """Build one binary's full splat config from the compact entry."""
    fields = {"name": entry["name"]}

    options = substitute(defaults.get("options", {}), fields)
    options.update(substitute(entry.get("options", {}), fields))

    return {
        "name": substitute(defaults.get("name", "{name}"), fields),
        "sha1": entry["sha1"],
        "options": options,
        "segments": entry["segments"],
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("config", help="compact multi-binary config to expand")
    ap.add_argument("--out", required=True, metavar="TEMPLATE",
                    help="output path template containing {name}")
    ap.add_argument("--make", help="also write a make fragment of binary paths")
    ap.add_argument("--check", action="store_true")
    args = ap.parse_args()

    if "{name}" not in args.out:
        sys.exit(f"--out must contain {{name}}: {args.out}")

    with open(args.config) as fh:
        compact = yaml.safe_load(fh)

    defaults = compact.get("defaults", {})
    binaries = compact["binaries"]

    changed = []
    make_lines = []
    for entry in binaries:
        cfg = expand(entry, defaults)
        opts = cfg["options"]
        make_lines += [
            f"{entry['name']}_TARGET := {opts['target_path']}",
            f"{entry['name']}_YAML := {args.out.format(name=entry['name'])}",
            f"{entry['name']}_LD := {opts['ld_script_path']}",
            f"{entry['name']}_ELF := {opts['elf_path']}",
            f"{entry['name']}_DIR := {opts['build_path']}",
        ]
        out_path = args.out.format(name=entry["name"])
        os.makedirs(os.path.dirname(out_path) or ".", exist_ok=True)
        text = yaml.safe_dump(cfg, sort_keys=False, default_flow_style=None)
        old = open(out_path).read() if os.path.exists(out_path) else None
        if old != text:
            changed.append(out_path)
            if not args.check:
                with open(out_path, "w") as fh:
                    fh.write(text)

    if args.make:
        make_lines.append("SPLAT_BINARIES := %s"
                          % " ".join(e["name"] for e in binaries))
        text = "# Generated from %s. Do not edit.\n%s\n" % (
            args.config, "\n".join(make_lines))
        old = open(args.make).read() if os.path.exists(args.make) else None
        if old != text:
            changed.append(args.make)
            if not args.check:
                with open(args.make, "w") as fh:
                    fh.write(text)

    if args.check and changed:
        print("stale generated splat configs:", *changed, sep="\n  ")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
