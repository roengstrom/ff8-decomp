#!/usr/bin/env python3
"""Inspect FF8's battle effect overlays.

Battle actions -- magic, GF summons, limit breaks, physical and enemy attacks --
each load their own MIPS overlay off the disc. They are not in the IMG's master
file table; battle.bin indexes them through `D_800E19BC`, an array of
(sector, size) pairs, where effect id N is entry N + 0x2FE.

Each overlay's image is loaded verbatim to one of three link bases and entered
at offset 0. battle.bin contains a literal `jal` to each:

    0x80190000   (jal at 800BE438, 800BEAC8)
    0x801A0000   (jal at 800BDBB8, 800BDED0, 800BE078, 800BF228, 800BF37C)
    0x801B0000   (jal at 800BEF54)

Which base a given overlay was linked at is not recorded anywhere on disc, so
`base` recovers it: it segments the image into function bodies at `jr $ra` and
picks the base under which the overlay's own `jal` targets land on those
boundaries. The correct base scores 100%, the other two roughly zero.

Extract the images first with `tools/extract.py <disc.bin> --effects`.

Usage:
    battle_effect_overlays.py list            # the index, from battle.bin
    battle_effect_overlays.py base <id>       # recover one overlay's link base
    battle_effect_overlays.py bases           # ... for every extracted overlay
"""
import os
import struct
import sys

TABLE_OFFSET = 0x499BC       # file offset of D_800E19BC within battle.bin
TABLE_VRAM = 0x800E19BC
TABLE_ENTRIES = 1117
FIRST_INDEX = 767            # first entry that is an effect overlay
ID_BIAS = 0x2FE              # table index = effect id + ID_BIAS
BASES = (0x80190000, 0x801A0000, 0x801B0000)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BATTLE = os.path.join(ROOT, 'original', 'battle.bin')
EFFECT_DIR = os.path.join(ROOT, 'original', 'effect')

JR_RA = 0x03E00008
_OPCODES = set(list(range(0x00, 0x13)) +
               [0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
                0x28, 0x29, 0x2a, 0x2b, 0x2e, 0x32, 0x3a])
_FUNCTS = set([0x00, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0c, 0x0d,
               0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b, 0x20, 0x21,
               0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2a, 0x2b])


def table(path=BATTLE):
    """-> [(sector, size)] for all D_800E19BC entries."""
    with open(path, 'rb') as f:
        d = f.read()
    return [struct.unpack_from('<II', d, TABLE_OFFSET + 8 * i)
            for i in range(TABLE_ENTRIES)]


def image_path(effect_id):
    return os.path.join(EFFECT_DIR, 'effect_%03d.bin' % effect_id)


def _legal(w):
    op = w >> 26
    return op in _OPCODES and (op != 0 or (w & 0x3f) in _FUNCTS)


def segment(img):
    """-> (function start offsets, byte offset where .text stops).

    Each body runs to its `jr $ra` plus the delay slot. The walk stops at the
    first word that is not a legal R3000A encoding, which is where the trailing
    rodata and asset payload begin.
    """
    n = len(img) // 4
    words = struct.unpack('<%dI' % n, img[:n * 4])
    starts = {0}
    i = 0
    end = 0
    while i < n:
        j = i
        while j < n and words[j] != JR_RA:
            if not _legal(words[j]):
                return starts, end
            j += 1
        if j + 1 >= n:
            break
        end = (j + 2) * 4
        starts.add(end)
        i = j + 2
    return starts, end


def link_base(img):
    """-> (base, text_size, hits, targets) for one overlay image.

    `hits` of `targets` internal `jal` destinations land on a function
    boundary under the winning base; a correct base scores every one of them.
    `base` is None when no base gets a single internal target, which happens
    for overlays too small to contain an internal call.
    """
    starts, end = segment(img)
    n = end // 4
    words = struct.unpack('<%dI' % n, img[:end])
    targets = {0x80000000 | ((w & 0x03FFFFFF) << 2)
               for w in words if (w >> 26) == 3}
    best = (None, 0, 0)
    for base in BASES:
        inside = {t - base for t in targets if 0 <= t - base < end}
        hits = len(inside & starts)
        if hits > best[1]:
            best = (base, hits, len(inside))
    return best[0], end, best[1], best[2]


def load(effect_id):
    path = image_path(effect_id)
    if not os.path.isfile(path):
        return None
    with open(path, 'rb') as f:
        return f.read()


def describe(effect_id):
    img = load(effect_id)
    if img is None:
        return 'effect %d: not extracted (run tools/extract.py --effects)' % effect_id
    base, text, hits, targets = link_base(img)
    if base is None:
        return ('effect %3d: image %8d  .text %7d  base undetermined'
                % (effect_id, len(img), text))
    return ('effect %3d: image %8d  .text %7d  bss 0x%08X  base 0x%08X  (%d/%d)'
            % (effect_id, len(img), text, base + len(img), base, hits, targets))


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else 'list'
    if cmd == 'list':
        tbl = table()
        print('%-5s %-6s %-9s %-10s' % ('id', 'idx', 'sector', 'size'))
        for i in range(FIRST_INDEX, TABLE_ENTRIES):
            sector, size = tbl[i]
            print('%-5d %-6d %-9d %-10d%s'
                  % (i - ID_BIAS, i, sector, size, '   (empty)' if size == 0 else ''))
    elif cmd == 'base':
        print(describe(int(sys.argv[2], 0)))
    elif cmd == 'bases':
        tbl = table()
        for i in range(FIRST_INDEX, TABLE_ENTRIES):
            if tbl[i][1]:
                print(describe(i - ID_BIAS))
    else:
        print(__doc__)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
