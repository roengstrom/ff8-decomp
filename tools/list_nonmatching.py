#!/usr/bin/env python3
"""List remaining nonmatching functions sorted by size.

Discovery is by path: every ``.s`` under a ``nonmatchings/`` directory is one
unmatched function, and the filename is its name. That is the whole backlog --
splat writes exactly one glabel per file, and every stub in ``src`` resolves to
one of these. With ``--include-matched`` the already-decompiled functions under
``matchings/`` are listed too, so you can see a whole object's shape at once.

Do NOT go back to trusting the first line of the file. A function that owns a
jump table has its ``.rodata`` emitted *ahead* of the ``.text``, so the file
opens with the jtbl's header instead of the function's; keying discovery off
that header silently dropped all 136 such functions -- i.e. exactly the jtbl
cases, which are the expensive ones.

The header is still read, but only for the size, and only as a hint: we look for
the ``nonmatching <name>, 0xSIZE`` line belonging to the file's own symbol
anywhere in the file, and fall back to counting instructions when it is absent.
Handwritten functions (splat prepends ``/* Handwritten function */``) are
flagged but still listed.

To work on one object, ``--object <name>`` scopes the listing to that splat
object and sorts by address. Add ``--include-matched`` and you get the layout of
the original .c file: every function, matched and not, in source order.
``--by-object`` is the index for finding the name.

A reachability column labels statically-orphaned functions: ``dead`` means
nothing references the function by any vector -- jal/%lo/%hi/.word in split
asm, raw address words in the built images, or jal-encoded words in excess of
the split-asm jal count (calls from UNSPLIT regions; this last vector is what
proved the 2026-08 world "dead islands" false). ``unreachable`` means it has
referrers, but every reference chain starts at dead code. Liveness propagates
from data-referenced functions, entry seeds (--seed to extend) and
unsplit-region callers. Caveat: disc-streamed dispatch words are invisible, so
both labels mean *statically* dead. ``--no-deep`` skips the binary scans
(build/ images required), ``--no-reach`` skips the analysis entirely.

Usage:
    python3 tools/list_nonmatching.py [--ascending] [--limit N] [--offset N]
                                      [--all] [--include-matched]
                                      [--exclude-dir NAME]... [--exclude SUBSTR]...
                                      [--handwritten | --no-handwritten]
                                      [--color auto|always|never]

    # the game-code backlog, biggest first, no vendored SDK
    python3 tools/list_nonmatching.py --exclude-dir psxsdk

    # one object's backlog, in address order
    python3 tools/list_nonmatching.py --object we_object3

    # ... and the same object as the .c file lays it out, matched (green) too
    python3 tools/list_nonmatching.py --object we_object3 --include-matched

    # which objects still have work in them
    python3 tools/list_nonmatching.py --by-object --exclude-dir psxsdk
"""
import argparse
import os
import re
import sys
from collections import defaultdict


# "nonmatching func_800BD82C, 0xEC" -- may appear after a leading .rodata block.
# splat uses the same header word for matched functions, so this covers both.
HEADER_RE = re.compile(
    r'^nonmatching\s+(\S+?)(?:,\s*0x([0-9A-Fa-f]+))?\s*$', re.MULTILINE)

# Reference forms that tie one function to another in splat asm. The asm tree
# carries the resolved call graph for matched and unmatched functions alike
# (matchings/*.s is the original binary's disassembly), so no C parsing is
# needed. jal covers direct calls; %lo/%hi covers address materialisation
# (jalr callbacks are invisible to jal-greps -- the registration site's %lo is
# their only textual trace); .word covers splat-resolved pointer tables.
JAL_RE = re.compile(r'\bjal\s+([A-Za-z_]\w*)')
LOHI_RE = re.compile(r'%(?:lo|hi)\(([A-Za-z_]\w*)\)')
WORD_RE = re.compile(r'^\s*\.word\s+([A-Za-z_]\w*)\s*$', re.MULTILINE)

# Live-by-construction entry points that nothing in the asm tree references
# textually (boot entry; overlay entries reached through raw address words the
# splitter left unresolved). Extend with --seed.
DEFAULT_SEEDS = ('main', 'start', '__start', 'func_80010000', 'func_801F04E8')

# Built binaries for the deep pass, mapped to the domain whose address space
# they carry. Only raw images -- .elf symtabs would count every symbol once and
# drown the signal. Overlays reuse address space, so a hit only counts inside
# the binary that actually holds that domain (plus the main exe, which stores
# overlay entry addresses in its load tables). The raw SLUS has no extension --
# a *.bin glob misses it, so it is named explicitly.
DEEP_BINARIES = (
    ('build/SLUS_008.92', 'main'),
    ('build/ovl/world/world.bin', 'world'),
    ('build/ovl/battle/battle.bin', 'battle'),
    ('build/ovl/debug/debug.ovl', 'debug'),
    ('build/ovl/tripletriad/tripletriad.bin', 'tripletriad'),
    ('build/ovl/field_init/field_init.bin', 'field_init'),
    ('build/field/field.bin', 'field'),
    ('build/intro/intro.bin', 'intro'),
)


def deep_seeds(defs, addr_by_node, root='asm'):
    """Vector 4 of the island method: little-endian address-word scan over the
    raw built images. A function whose address appears as a data word is
    runtime-dispatchable (callback tables, state machines) and therefore a live
    root even with zero textual references. Code cannot false-positive: lui/
    addiu pairs split the address and j/jal encode target>>2, so a raw 32-bit
    match is a data word. Hits only count in the binary of the function's own
    domain or the main exe (overlays reuse address space)."""
    import glob as _glob
    import struct
    words = {}                       # domain -> set of aligned LE words
    binaries = list(DEEP_BINARIES)
    for p in _glob.glob('build/ovl/*/*.ovl'):
        binaries.append((p, os.path.basename(p)[:-4]))
    for path, dom in binaries:
        try:
            with open(path, 'rb') as f:
                data = f.read()
        except OSError:
            continue
        n = len(data) // 4
        ws = set(struct.unpack('<%dI' % n, data[:n * 4]))
        words.setdefault(dom, set()).update(ws)
    # Overlays with unsplit entry regions (world: everything below 0x800997E8
    # is still raw bytes) make calls no text scan can see and would orphan the
    # whole overlay. jal encodes its target as 0x0C000000 | (addr>>2), so scan
    # the raw images for jal-encoded words and subtract the jal count already
    # visible in split asm: any EXCESS is a call from unsplit code -- a live
    # root. Plain subtraction keeps islands intact: an island's internal jals
    # are all in split asm and cancel exactly.
    import collections
    word_counts = {}                    # domain -> Counter of raw words
    for path, dom in binaries:
        try:
            with open(path, 'rb') as f:
                data = f.read()
        except OSError:
            continue
        n = len(data) // 4
        word_counts.setdefault(dom, collections.Counter()).update(
            struct.unpack('<%dI' % n, data[:n * 4]))

    asm_jals = collections.Counter()    # (domain, target-name) -> jal lines
    for dirpath, _dirnames, filenames in os.walk(root):
        for fn in filenames:
            if not fn.endswith('.s'):
                continue
            path = os.path.join(dirpath, fn)
            dom = domain_of(path)
            try:
                with open(path, errors='replace') as f:
                    text = f.read()
            except OSError:
                continue
            for t in JAL_RE.findall(text):
                asm_jals[(dom, t)] += 1

    seeds = set()
    for node in defs:
        dom, name = node
        addr = addr_by_node.get(node, 0)
        if not addr:
            continue
        if addr in words.get(dom, ()) or addr in words.get('main', ()):
            seeds.add(node)
            continue
        jw = 0x0C000000 | ((addr >> 2) & 0x03FFFFFF)
        for bdom in (dom, 'main'):
            raw = word_counts.get(bdom, {}).get(jw, 0)
            if raw > asm_jals.get((bdom, name), 0):
                seeds.add(node)
                break
    return seeds

# /* 25830 800BD830 1400A28F */  lw  $v0, 0x14($sp)
INSN_RE = re.compile(
    r'^\s*/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s*\*/\s+\S', re.MULTILINE)

RED = '\033[31m'
GREEN = '\033[32m'
DIM = '\033[2m'
RESET = '\033[0m'


def addr_of(text, name):
    """Load address of `name`: the first instruction after its glabel.

    The filename carries the address for a ``func_XXXXXXXX``, but not once the
    function has been named, so read it out of the asm instead.
    """
    body = text.split('glabel ' + name, 1)
    if len(body) == 2:
        m = INSN_RE.search(body[1])
        if m:
            return int(m.group(1), 16)
    return 0


def size_of(text, name):
    """Byte size of `name` in this file: its own header, else instruction count."""
    for sym, size in HEADER_RE.findall(text):
        if sym == name and size:
            return int(size, 16)
    # No header for our symbol (or no size on it) -- count the .text we can see.
    body = text.split('glabel ' + name, 1)
    if len(body) == 2:
        return 4 * len(INSN_RE.findall(body[1]))
    return 0


def scan(root, exclude_dirs=()):
    """Yield a dict per function .s found under root.

    A path is part of the backlog when it has a ``nonmatchings`` component, and
    already decompiled when it has a ``matchings`` one. Both are always walked
    -- the full tree costs well under a second, and reading it whole is what
    lets every view report matched bytes as well as remaining ones.
    ``--include-matched`` then only decides which rows get printed.

    Directories named in `exclude_dirs` are pruned by path component, so
    ``psxsdk`` drops the whole vendored SDK without also matching some unrelated
    substring.
    """
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in exclude_dirs]
        parts = dirpath.split(os.path.sep)
        if 'nonmatchings' in parts:
            matched = False
        elif 'matchings' in parts:
            matched = True
        else:
            continue
        for fn in sorted(filenames):
            if not fn.endswith('.s'):
                continue
            path = os.path.join(dirpath, fn)
            name = fn[:-2]
            try:
                with open(path, errors='replace') as f:
                    text = f.read()
            except OSError:
                continue
            yield {
                'size': size_of(text, name),
                'addr': addr_of(text, name),
                'name': name,
                'obj': os.path.basename(dirpath),
                'matched': matched,
                'handwritten': 'Handwritten function' in text,
                'path': path,
            }


def domain_of(path):
    """Which program a path belongs to: an overlay name, field/intro, or main.

    Overlays are exclusive address spaces that reuse names (the overlay-conflict
    prototypes), so reachability must never leak between two overlays -- but
    every overlay may call into main.
    """
    parts = path.split(os.path.sep)
    try:
        i = parts.index('asm')
    except ValueError:
        return 'main'
    rest = parts[i + 1:]
    if rest and rest[0] == 'ovl' and len(rest) > 1:
        return rest[1]
    if rest and rest[0] in ('field', 'intro'):
        return rest[0]
    return 'main'


def build_reach(root, defs, seeds, extra_live=()):
    """Classify every defined function as '', 'dead' or 'unreachable'.

    Definitions (user's, from the world dead-island hunt): a function nothing
    references at all is *dead*; a function whose every reference chain starts
    only at dead roots is *unreachable* (it belongs to a dead island). Everything
    reachable from a live seed is live ('').

    Live seeds are (a) functions referenced from data/rodata -- pointer tables
    are consumed at runtime -- and (b) DEFAULT_SEEDS/--seed entry points.
    Liveness then propagates along jal/%lo/%hi/.word edges to a fixpoint.

    Caveat carried over from the island hunt: FF8 streams world data from disc,
    so a runtime-loaded dispatch word can never be excluded from the repo alone;
    'dead'/'unreachable' here means *statically* so.
    """
    by_name = defaultdict(set)          # name -> {domain}
    for dom, name in defs:
        by_name[name].add(dom)

    def resolve(dom, name):
        """A reference in `dom` to `name`: same domain first, then main, then
        (for names only defined elsewhere, e.g. main data naming overlay
        symbols) every defining domain."""
        if name in by_name:
            if dom in by_name[name]:
                return [(dom, name)]
            if 'main' in by_name[name]:
                return [('main', name)]
            return [(d, name) for d in by_name[name]]
        return []

    edges = defaultdict(set)            # (dom,func) -> {(dom,func)}
    inbound = defaultdict(int)          # (dom,func) -> textual reference count
    live = set()                        # BFS worklist seeds

    for dirpath, dirnames, filenames in os.walk(root):
        for fn in filenames:
            if not fn.endswith('.s'):
                continue
            path = os.path.join(dirpath, fn)
            dom = domain_of(path)
            owner = fn[:-2]
            is_func_file = (dom, owner) in defs
            try:
                with open(path, errors='replace') as f:
                    text = f.read()
            except OSError:
                continue
            targets = set(JAL_RE.findall(text)) | set(LOHI_RE.findall(text)) \
                | set(WORD_RE.findall(text))
            targets.discard(owner)      # self-recursion must not self-revive
            for t in targets:
                for node in resolve(dom, t):
                    inbound[node] += 1
                    if is_func_file:
                        edges[(dom, owner)].add(node)
                    else:
                        live.add(node)  # data/rodata reference = live root

    for name in seeds:
        for d in by_name.get(name, ()):
            live.add((d, name))
    live.update(extra_live)

    work = list(live)
    while work:
        n = work.pop()
        for m in edges.get(n, ()):
            if m not in live:
                live.add(m)
                work.append(m)

    reach = {}
    for node in defs:
        if node in live:
            reach[node] = ''
        elif inbound.get(node, 0) == 0:
            reach[node] = 'dead'
        else:
            reach[node] = 'unreachable'
    return reach


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--root', default='asm', help='asm directory to scan')
    ap.add_argument('--object', action='append', default=[], metavar='NAME',
                    help='scope to this splat object, e.g. we_object3 '
                         '(repeatable); lists it all, in address order. Add '
                         '--include-matched for the layout of the original .c file')
    ap.add_argument('--by-object', action='store_true',
                    help='summarize per object instead of listing functions')
    ap.add_argument('--sort', choices=('size', 'addr', 'name'), default=None,
                    help='sort key (default: size, or addr under --object)')
    ap.add_argument('--ascending', action='store_true',
                    help='smallest first (default: largest first; address and '
                         'name order are always ascending)')
    ap.add_argument('--include-matched', action='store_true',
                    help='also list already-decompiled functions (matchings/)')
    ap.add_argument('--exclude-dir', action='append', default=[], metavar='NAME',
                    help='prune this directory name anywhere in the tree '
                         "(repeatable); 'psxsdk' drops the vendored SDK")
    ap.add_argument('--exclude', action='append', default=[], metavar='SUBSTR',
                    help='substring to exclude from path (repeatable)')
    ap.add_argument('--limit', type=int, default=100,
                    help='max rows to print (default 100, use --all to print all)')
    ap.add_argument('--offset', type=int, default=0,
                    help='skip the first N rows (after sort/filter)')
    ap.add_argument('--all', action='store_true', help='print all rows')
    ap.add_argument('--color', choices=('auto', 'always', 'never'), default='auto',
                    help='red = nonmatching, green = matched (default auto)')
    hw = ap.add_mutually_exclusive_group()
    hw.add_argument('--handwritten', action='store_true',
                    help='only handwritten functions')
    hw.add_argument('--no-handwritten', action='store_true',
                    help='hide handwritten functions')
    ap.add_argument('--no-reach', action='store_true',
                    help='skip the reachability analysis (dead/unreachable column)')
    ap.add_argument('--no-deep', action='store_true',
                    help='skip the raw-binary address-word scan (build/ images); '
                         'without it, functions reached only through unsplit '
                         'pointer tables are falsely dead')
    ap.add_argument('--seed', action='append', default=[], metavar='NAME',
                    help='treat this function as a live entry point (repeatable); '
                         'extends the built-in boot/overlay-entry seeds')
    args = ap.parse_args()

    # --object scopes to one file's worth of functions; matched rows stay opt-in
    # via --include-matched, exactly as they are for every other view.
    objects = set(args.object)
    if objects:
        args.all = True
    sort = args.sort or ('addr' if objects else 'size')

    color = args.color == 'always' or (args.color == 'auto' and sys.stdout.isatty())
    def paint(line, matched):
        return (GREEN if matched else RED) + line + RESET if color else line

    # The reachability graph must always cover the WHOLE tree (liveness flows
    # through matched functions and other objects), so it is built from an
    # unfiltered scan even when the listing itself is scoped.
    reach = {}
    if not args.no_reach:
        full = list(scan(args.root, set(args.exclude_dir)))
        all_defs = {(domain_of(r['path']), r['name']) for r in full}
        addr_by_node = {(domain_of(r['path']), r['name']): r['addr']
                        for r in full}
        extra = () if args.no_deep else deep_seeds(all_defs, addr_by_node,
                                                   args.root)
        reach = build_reach(args.root, all_defs,
                            tuple(args.seed) + DEFAULT_SEEDS, extra)

    rows = []
    for r in scan(args.root, set(args.exclude_dir)):
        if objects and r['obj'] not in objects:
            continue
        if any(p in r['path'] for p in args.exclude):
            continue
        if args.handwritten and not r['handwritten']:
            continue
        if args.no_handwritten and r['handwritten']:
            continue
        r['reach'] = reach.get((domain_of(r['path']), r['name']), '')
        rows.append(r)

    if sort == 'size':
        rows.sort(key=lambda r: (r['size'], r['name']), reverse=not args.ascending)
    elif sort == 'addr':
        rows.sort(key=lambda r: (r['addr'], r['name']))
    else:
        rows.sort(key=lambda r: r['name'])

    # Totals always cover both halves of the tree -- that is what makes a
    # "bytes matched" figure meaningful even in the backlog-only view.
    # `rows` is only what gets printed.
    done = sum(1 for r in rows if r['matched'])
    hand = sum(1 for r in rows if r['handwritten'])
    todo_bytes = sum(r['size'] for r in rows if not r['matched'])
    done_bytes = sum(r['size'] for r in rows if r['matched'])
    all_bytes = todo_bytes + done_bytes
    pct_bytes = 100.0 * done_bytes / all_bytes if all_bytes else 0.0
    stats = rows
    todo = len(rows) - done
    if not args.include_matched:
        rows = [r for r in rows if not r['matched']]
    total = len(rows)

    if args.by_object:
        by = {}
        for r in stats:
            o = by.setdefault(r['obj'], {'n': 0, 'done': 0, 'todo': 0,
                                         'bytes': 0, 'bytes_done': 0})
            o['n'] += 1
            o['done' if r['matched'] else 'todo'] += 1
            o['bytes_done' if r['matched'] else 'bytes'] += r['size']
        print(f"{'object':<20} {'todo':>5} {'done':>5} {'total':>6} "
              f"{'bytes left':>11} {'bytes done':>11} {'pct':>6}")
        print('-' * 76)
        for name, o in sorted(by.items(), key=lambda kv: -kv[1]['bytes']):
            tot = o['bytes'] + o['bytes_done']
            pct = 100.0 * o['bytes_done'] / tot if tot else 0.0
            line = (f"{name:<20} {o['todo']:>5} {o['done']:>5} {o['n']:>6} "
                    f"{o['bytes']:>11} {o['bytes_done']:>11} {pct:>5.1f}%")
            print(paint(line, o['todo'] == 0))
        print('-' * 76)
    else:
        if args.offset:
            rows = rows[args.offset:]
        if not args.all:
            rows = rows[:args.limit]
        for r in rows:
            mark = ('+' if r['matched'] else '-') if args.include_matched else ' '
            rc = r.get('reach', '')
            line = (f"{mark} {r['addr']:08X} {r['size']:6d} {rc or '':<11} "
                    f"{r['name']}  {r['path']}")
            if r['handwritten']:
                line += '  [handwritten]'
            print(paint(line, r['matched']))

    # --by-object aggregates the whole scan, not just the printable rows.
    if args.by_object:
        shown = total = len(stats)
    else:
        shown = len(rows)
    ndead = sum(1 for r in stats if r.get('reach') == 'dead')
    nunreach = sum(1 for r in stats if r.get('reach') == 'unreachable')
    reach_note = ('' if args.no_reach
                  else f'; {ndead} dead, {nunreach} unreachable (static)')
    summary = (f'# showing {shown} of {total} rows '
               f'({todo} nonmatching, {done} matched, {hand} handwritten'
               f'{reach_note}; '
               f'{done_bytes} of {all_bytes} bytes matched = {pct_bytes:.2f}%, '
               f'{todo_bytes} left) '
               f'(--offset={args.offset}, --root={args.root})')
    if args.color == 'always' or (args.color == 'auto' and sys.stderr.isatty()):
        summary = DIM + summary + RESET
    print('\n' + summary, file=sys.stderr)


if __name__ == '__main__':
    main()
