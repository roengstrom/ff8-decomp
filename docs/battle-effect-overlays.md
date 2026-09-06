# Battle effect overlays (the unindexed 45 MB on disc 1)

FF8DISC1.IMG carries a 45 MB run that the IMG's master file table never points
at. It is 343 LZSS-compressed blocks, one per battle action id, indexed by a
table inside `battle.bin`. **327 are MIPS overlays** that load at one of three
fixed link bases and are entered with a plain `jal` from `bc_object11`; the
other **16 are AKAO sound bundles** (Square's PSX sequence format, magic
`"AKAO"` at offset 8) that share the same table and contain no overlay entry
point. Every one of the 327 starts with `addiu $sp, $sp, -N`, and there are no
ambiguous cases in between.

Tooling: `tools/extract.py <disc.bin> --effects` writes the images to
`original/effect/effect_001.bin` .. `effect_350.bin` (opt-in; 343 files, 77 MB,
and no current target needs them). `tools/battle_effect_overlays.py`
(`list` / `base <id>` / `bases`) then reports the index and recovers each
overlay's link base from the extracted images.

## The index

`D_800E19BC` — file `+0x499BC` in battle.bin, vram `0x800E19BC` — is an array of
1117 `(u32 sector, u32 size)` pairs covering disc sectors 99145..149209. The
repo already carried the symbol and a `FIXME` in `src/battle/bc_object1.c`
noting it is "conceptually an array of (s32 sector, s32 length) pairs".

| index range | contents |
|---|---|
| 0 .. 766 | battle assets in a different, uncompressed container format |
| **767 .. 1116** | **the effect slots — 350 total: 327 code overlays, 16 AKAO sound bundles, 7 empty** |

`size` is the on-disc payload length and matches `compressed_size + 4` exactly
for 342 of the 344 blocks that were inventoried independently.

## The load path

```
bc_object11  func_800BE104 / func_800BE89C
   id = ((BattleAction *)D_800F16A4)->id            /* u16 at +6 */
   func_800BD5B0(id, 0x801A0000)
      -> func_800C5304(id + 0x2FE, dst)             /* 0x2FE = 766 */
         -> func_8009B5C4(idx, dst, 1, cb)
            -> cdRead(D_800E19BC[idx*2], D_800E19BC[idx*2 + 1], dst, cb)
   ...
   jal 0x80190000 / 0x801A0000 / 0x801B0000         /* entry is offset 0 */
```

So **effect id N → table index N + 0x2FE**, giving ids 1..350. The id comes from
the current battle action descriptor, so the id space is battle *actions* —
magic, GF summons, limit breaks, physical and enemy attacks — not magic alone.

## Load addresses

Three link bases, each confirmed three independent ways:

1. **`jal` solve.** Taking each block's internal `jal` targets and asking which
   base makes them land on function boundaries: at the correct base **100% of
   internal targets hit a boundary exactly**, and at the other two bases
   essentially none do.
2. **battle.bin materialises exactly these constants** — `0x80190000`,
   `0x801A0000`, `0x801B0000` — and nothing else in that window.
3. **battle.bin `jal`s them directly**: `0x801A0000` at 800BDBB8 / 800BDED0 /
   800BE078 / 800BF228 / 800BF37C, `0x80190000` at 800BE438 / 800BEAC8,
   `0x801B0000` at 800BEF54.

| base | overlays |
|---|---|
| `0x801A0000` | 285 |
| `0x80190000` | 28 |
| `0x801B0000` | 4 |
| undetermined | 26 |

The undetermined 26 are overlays whose text holds no internal call at all, so
there is nothing to solve against. `bases` prints the evidence as `(hits/targets)`:
298 overlays score 5 or more internal targets, and 297 of those hit every one.

Corroboration: `func_800B3574(0x80190000, 0x801A0000, 0x60000)` runs immediately
before `jal func_80190000`, and `0x801A0000 + 0x60000 = 0x80200000` exactly.
Every one of the 343 images fits under that ceiling — the largest is 0x5E990
against a 0x60000 arena.

**`0x801Cxxxx` is not a link base.** It is each block's own `.data`/`.bss`,
which begins immediately after its loaded image: for 178 of 212 measurable
blocks the lowest above-image global sits within 0x100 of the image end, median
gap 0. Every overlay is linked with its own bss base, and that base moves with
the image size.

## What the code does

Each overlay is a small bytecode engine plus handlers plus assets. Text is at
offset 0, rodata follows it, assets fill the rest — median code fraction 15.4%.

Effect 219 (block 210) is representative: 508 functions in 0x13C8C of text, then
ten function-pointer tables at `0x801B3ED8`..`0x801B4600` (9, 20, 23, 10, 58,
24, 100, 15, 21 and 11 entries). Of its 508 functions, **352 are reached only
through those tables**, 97 by a direct `jal`, 4 by both, and 59 by neither.

Externally it calls the PsyQ
GTE matrix library (`SetRotMatrix`, `RotTransPers`, `NormalClip`, `RotMatrixYXZ`,
`OuterProduct12`, …), the sound commands (`sndCmd21`, `sndEnableReverb`), and
~30 `battle.bin` entry points — including `loadBattleCmd`, `deactivateBattleCmd`
and `func_8009B5C4`, i.e. the overlays chain-load further assets through the same
table they came from.

Pool set-up is legible enough to read straight off:
`func_800B2A00(arrayEnd, arrayBase, stride, count)` — e.g.
`(0x801CD9F0, 0x801C5B60, 0x6C, 0x12C)`, a 300-entry pool of 108-byte records.

## Scope

Segmenting all 327 blocks that hold text, at `jr $ra` boundaries:

| measure | functions | bytes |
|---|---|---|
| total instances | 49,722 | 11,644,948 |
| distinct by exact bytes | 30,268 | 9,297,536 |
| **distinct, relocations masked** | **9,594** | **4,680,652** |
| distinct, all immediates masked | 4,729 | 2,486,836 |

The exact-bytes figure is the misleading one: the same function linked at a
different address hashes differently. Masking only what a linker rewrites
(`j`/`jal` targets and `lui`+`%lo` pairs) collapses it by half. The true new-code
burden is **~4.7 MB**, with ~2.5 MB as a floor.

There is **no single shared runtime**. No function appears in even 75% of
blocks; only 35 appear in 25%. Duplication is clustered into engine families —
grouping blocks that share ≥50% of their functions gives 27 families, of which
two dominate:

| family | blocks | distinct funcs | distinct bytes |
|---|---|---|---|
| 0 | 181 | 4,451 | 2,075,300 |
| 1 | 58 | 3,672 | 1,495,912 |
| 2..26 | 44 | ~1,100 | ~850,000 |

Families cut across link bases, so the base is not what selects the engine.

## The 47,000-returns question

The `jr $ra` count is not inflated; the comparison it was being held against was
not like-for-like.

The count is real: segmenting at `jr $ra` yields exactly as many bodies as there
are `jr $ra` words, 100% of internal `jal` targets land on those boundaries,
and across 283 blocks / 49,147 bodies a median of 79% (mean 75%) are provably
either a `jal` target or stored in a pointer table. Mean body size is 230
bytes, median 108. These are ordinary functions.

Two things account for the 5× gap against 8,979 direct-call targets in the PC's
equivalent x86 region:

- **Duplication.** 49,722 is instances, not distinct functions. Distinct is
  9,594 — within 7% of 8,979.
- **Dispatch.** Most of these functions are only ever reached through
  function-pointer tables (352 of 508 in the block measured above). A
  direct-call-target count structurally cannot see them.

## Note on the PC cross-reference

`magic.fs` is **not** the PC counterpart of this code. Its 355 entries are 239
`.tim` textures plus 116 `.00`/`.01` files across 286 distinct `magNNN` ids — no
executable content. The 343-vs-355 similarity is a coincidence.

The real correspondence is with the statically-linked x86 effect region in
`FF8_JP.exe`, which references **exactly 350 distinct `magNNN` ids, 0..349,
contiguous** (the next referenced id is 999). The PS1 table has **exactly 350
effect slots, ids 1..350.** Same effect count, ±1 numbering convention. On PC
the effect programs are compiled into the executable; on PS1 they are 350
separately streamed overlays, and `magic.fs` holds only their textures.

## Proposed splat onboarding

**Do not onboard 343 overlays.** They are ~4.7 MB of distinct code dominated by
two engines; the leverage is entirely in those two, and everything else is a
long tail of per-effect scripts.

### Step 0 — make them tracked inputs

Nothing here is reachable from a binary the repo currently tracks. Each overlay
must first become a file under `original/`, which
`tools/extract.py <disc.bin> --effects` now does, producing
`original/effect/effect_001.bin` .. `effect_350.bin` (decompressed images;
splat needs the uncompressed form, and the LZSS is not byte-reproducible for a
`make verify` without a matching compressor). That means **the overlays cannot be verified
against the disc end-to-end yet** — the build would verify the decompressed
image, not the on-disc block. Worth settling before committing to the path.

### Step 1 — one target per overlay

Done. All 343 are entries in `config/ff8.yaml`, between the
`# BEGIN/# END battle effect overlays` markers, written by
`tools/gen_effect_config.py` (segment bounds are measured from the images, so
that block is generated; `--check` reports it stale). `make verify` covers
**368 targets — 25 base plus 343 effect overlays — all Match.**

A typical entry is just the measured bounds:

```yaml
- name: effect_001
  sha1: d2269dde125f54b3f00c61eb318b58d3ca3c6f40
  options: {target_path: original/effect/effect_001.bin, src_path: src/effect}
  segments:
  - {name: effect_001, type: code, start: 0, vram: 0x801A0000,
     subsegments: [[0x0, c, effect_001], [0x4614, bin, effect_001]]}
  - [0x258E0]
```

**This is the load-bearing confirmation of the link base.** Given only `vram`,
spimdisasm independently finds **79 functions in effect 001, the same 79
boundaries** that segmenting at `jr $ra` produces, resolves calls against
`symbols.shared.txt` (`jal getMenuString`), and names the bss globals
(`D_801D5D00`) at the addresses predicted above. A wrong base would not produce
a single clean boundary. Nothing about function or jump-table detection is
supplied by hand — pinning `vram` is the whole job.

Three things this cost, all now fixed and worth knowing before adding more
binaries of this shape:

- **`disassemble_all` on an asset-heavy overlay is ruinous.** These images are
  ~85% embedded assets, and disassembling that payload as `data` produced a
  `.word` listing 12x the size of the bytes (1.72 MB for a 136 KB payload).
  Across the set that projected to ~2.3 GB of asm and ~120,000 files. Emitting
  the payload as `bin` gives 404 MB and ~11,000 files.
- **An unnamed `bin` subsegment is written as `assets/<offset>.bin`**, so any
  two overlays sharing a payload offset silently overwrite each other. Every
  one is named: `[0x4614, bin, effect_001]`.
- **`make verify` expanded its comparison inline**, one shell command per
  overlay in a single `$(foreach ...)`. Past roughly 300 binaries that exceeds
  the kernel's 128 KB limit on one shell argument and make dies with
  `Argument list too long` (exit 127) *after* building everything successfully.
  The triples now go through `$(file >)` and a shell loop.

### Step 2 — naming

Name by effect id, not by block or sector: `effect_001` .. `effect_350`, with
symbols in `config/symbols.effect_NNN.txt`. The id is the stable identifier —
it is what the game indexes with, it survives re-extraction, and it maps
directly onto the PC side's `magNNN`. Sector and block numbers are artifacts of
how the region happened to be scanned.

### Step 3 — where the shared code lives

There is no shared *binary* to factor out: every overlay carries its own copy of
its engine. The sharing has to happen in source, the way the repo already shares
code across overlays — one `src/effect/engine_a/*.c` compiled into each of the
181 family-0 targets, `engine_b` into the 58 family-1 targets, and a per-effect
`src/effect/effect_NNN.c` for the script and tables unique to each.

That only works if the engine really is byte-identical modulo relocations
across a family, which the ≥50%-shared clustering suggests but does not prove —
family membership was measured at a 50% threshold, and the within-family
remainder is not yet characterised.

### Suggested first move

Onboard **two** overlays, one from each dominant family — a small family-0 block
such as effect 1 (17,940 bytes of text) and a family-1 block — and try to match
their shared engine. That answers the question that decides everything else:
does one source file actually reproduce the engine across a whole family? If it
does, the 181-block family collapses to one engine plus 181 thin scripts and the
work is tractable. If it does not, this is ~4.7 MB of largely unique code and
the honest answer is that it is not worth pursuing.
