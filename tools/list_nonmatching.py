#!/usr/bin/env python3
"""List remaining nonmatching functions sorted by size.

Scans asm/**/nonmatchings/**/*.s and prints each function's size,
name, and path. Handwritten files are reported with size 0.

Usage:
    python3 tools/list_nonmatching.py [--ascending] [--exclude PATTERN]...
                                      [--limit N] [--offset N] [--all]
"""
import argparse
import os
import re
import sys


HEADER_RE = re.compile(r'^(?:nonmatching|Handwritten)\s+(\S+?)(?:,\s*0x([0-9A-Fa-f]+))?\s*$')


def scan(root):
    """Yield (size, name, path) for every nonmatching .s file under root.

    Handwritten files have no size in the header — they're reported as 0.
    """
    for dirpath, _, filenames in os.walk(root):
        if os.path.sep + 'nonmatchings' + os.path.sep not in dirpath + os.path.sep:
            continue
        for fn in filenames:
            if not fn.endswith('.s'):
                continue
            path = os.path.join(dirpath, fn)
            try:
                with open(path) as f:
                    head = f.readline()
            except OSError:
                continue
            m = HEADER_RE.match(head)
            if not m:
                continue
            size = int(m.group(2), 16) if m.group(2) else 0
            yield size, m.group(1), path


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--root', default='asm', help='asm directory to scan')
    ap.add_argument('--ascending', action='store_true',
                    help='smallest first (default: largest first)')
    ap.add_argument('--exclude', action='append', default=[],
                    help='substring to exclude from path (repeatable)')
    ap.add_argument('--limit', type=int, default=100,
                    help='max rows to print (default 100, use --all to print all)')
    ap.add_argument('--offset', type=int, default=0,
                    help='skip the first N rows (after sort/filter)')
    ap.add_argument('--all', action='store_true', help='print all rows')
    args = ap.parse_args()

    rows = []
    for size, name, path in scan(args.root):
        if any(p in path for p in args.exclude):
            continue
        rows.append((size, name, path))

    rows.sort(key=lambda r: r[0], reverse=not args.ascending)

    total = len(rows)
    if args.offset:
        rows = rows[args.offset:]
    if not args.all:
        rows = rows[:args.limit]

    for size, name, path in rows:
        print(f'{size:6d}  {name}  {path}')

    print(f'\n# showing {len(rows)} of {total} rows '
          f'(--offset={args.offset}, --root={args.root})',
          file=sys.stderr)


if __name__ == '__main__':
    main()
