# FF8 PS1 Menu Overlay Loading System

How FF8's main executable (SLUS_008.92) loads menu overlay modules from
the game disc at runtime. Addresses are for the USA release.

---

## Overview

FF8 uses standard ISO 9660 (the PS1 disc filesystem) rather than a custom
archive container for its menu overlays. The main executable contains a
table of overlay descriptors (`load_table` at 0x80053C58) that maps
overlay indices to filenames and RAM load addresses. Overlays are loaded
via CD read commands queued into a ring buffer, ultimately using PS1 BIOS
`CdSearchFile()` / `CdRead()` calls to find and read files from the
disc's root directory.

Each overlay is a position-dependent MIPS binary that runs at a fixed
address in the upper portion of RAM (0x801E0000+). Some overlays share
load addresses, so the game tracks a "dependency byte" that specifies a
sub-file index within `mngrp.bin` to be loaded as a prerequisite.

---

## Menu Overlay Filename Strings

19 filename strings are stored consecutively in .rodata at
0x80010828–0x8001091C. These are the files the overlay loader references
by pointer.

| Index | Address | String |
|-------|---------|--------|
| 18 | 0x80010828 | `init.out` |
| 17 | 0x80010834 | `mngrp.bin` |
| 16 | 0x80010840 | `menutest.ovl` |
| 15 | 0x80010850 | `menutips.ovl` |
| 14 | 0x80010860 | `menutmag.ovl` |
| 13 | 0x80010870 | `menututo.ovl` |
| 12 | 0x80010880 | `menucrd.ovl` |
| 11 | 0x8001088C | `menusav.ovl` |
| 10 | 0x80010898 | `menujnc2.ovl` |
| 9 | 0x800108A8 | `menugf.ovl` |
| 8 | 0x800108B4 | `menumgc.ovl` |
| 7 | 0x800108C0 | `menuitem.ovl` |
| 6 | 0x800108D0 | `menuext.ovl` |
| 5 | 0x800108DC | `menushop.ovl` |
| 4 | 0x800108EC | `menuabl.ovl` |
| 3 | 0x800108F8 | `menusts.ovl` |
| 2 | 0x80010904 | `menupty.ovl` |
| 1 | 0x80010910 | `menucfg.ovl` |
| 0 | 0x8001091C | `menumain.ovl` |

The index column corresponds to the overlay's position in the descriptor
table (index 0 = first entry in `load_table`). Filenames are stored in
reverse order relative to their table indices.

### Disc Executable Strings

Four disc-specific executable paths follow immediately after:

| Address | String |
|---------|--------|
| 0x8001092C | `\SLUS_008.92;1` |
| 0x8001093C | `\SLUS_009.08;1` |
| 0x8001094C | `\SLUS_009.09;1` |
| 0x8001095C | `\SLUS_009.10;1` |

These correspond to Discs 1–4. The ISO 9660 `;1` suffix is the file
version number required by the PS1 BIOS file API.

---

## Overlay Descriptor Table (`load_table`)

Located at 0x80053C58 in the .data section (`asm/data/41EE8.data.s`),
renamed from `D_80053C58` to `load_table` in the decomp. 19 entries,
8 bytes each (152 bytes total). Each entry is a pair of words:

```
struct overlay_descriptor {
    u32 addr_and_dep;    /* upper 24 bits = load address, low 8 bits = dependency byte */
    u32 filename_ptr;    /* pointer to filename string in .rodata */
};
```

The packed format of `addr_and_dep`:
- **Load address** = `addr_and_dep & 0xFFFFFF00` (mask off low byte)
- **Dependency byte** = `addr_and_dep & 0xFF`

A dependency byte of 0x00 means no prerequisite. A non-zero value is a
sub-file index within `mngrp.bin` — the game loads that sub-file into
0x801E0000 before loading the overlay itself.

### Full Decoded Table

| Idx | Raw Word | Filename Ptr | Filename | Load Address | Dep |
|-----|----------|-------------|----------|-------------|-----|
| 0 | 0x801EF800 | 0x8001091C | menumain.ovl | 0x801EF800 | 0x00 |
| 1 | 0x801E5801 | 0x80010910 | menucfg.ovl | 0x801E5800 | 0x01 |
| 2 | 0x801E5802 | 0x80010904 | menupty.ovl | 0x801E5800 | 0x02 |
| 3 | 0x801E5802 | 0x800108F8 | menusts.ovl | 0x801E5800 | 0x02 |
| 4 | 0x801E2802 | 0x800108EC | menuabl.ovl | 0x801E2800 | 0x02 |
| 5 | 0x801E5802 | 0x800108DC | menushop.ovl | 0x801E5800 | 0x02 |
| 6 | 0x801E5802 | 0x800108D0 | menuext.ovl | 0x801E5800 | 0x02 |
| 7 | 0x801E2802 | 0x800108C0 | menuitem.ovl | 0x801E2800 | 0x02 |
| 8 | 0x801E5801 | 0x800108B4 | menumgc.ovl | 0x801E5800 | 0x01 |
| 9 | 0x801E5802 | 0x800108A8 | menugf.ovl | 0x801E5800 | 0x02 |
| 10 | 0x801E5802 | 0x80010898 | menujnc2.ovl | 0x801E5800 | 0x02 |
| 11 | 0x801E2801 | 0x8001088C | menusav.ovl | 0x801E2800 | 0x01 |
| 12 | 0x801E5801 | 0x80010880 | menucrd.ovl | 0x801E5800 | 0x01 |
| 13 | 0x801E2802 | 0x80010870 | menututo.ovl | 0x801E2800 | 0x02 |
| 14 | 0x801E5802 | 0x80010860 | menutmag.ovl | 0x801E5800 | 0x02 |
| 15 | 0x801E5802 | 0x80010850 | menutips.ovl | 0x801E5800 | 0x02 |
| 16 | 0x801E5802 | 0x80010840 | menutest.ovl | 0x801E5800 | 0x02 |
| 17 | 0x00000000 | 0x80010834 | mngrp.bin | 0x00000000 | 0x00 |
| 18 | 0x00000000 | 0x80010828 | init.out | 0x00000000 | 0x00 |

### Dependency Groups

The dependency byte is a sub-file index within `mngrp.bin`, not an
overlay index. When an overlay has a non-zero dep byte, the loader
calls `func_800360D0(dep, 0x801E0000)` which queues a CD read of
`mngrp.bin` (overlay_id=0x11) with the dep byte as `param`, loading
the specified sub-file to 0x801E0000 before the overlay itself loads.

Two dependency groups:

- **Dep 0x01** — mngrp.bin sub-file 1. Required by: menucfg, menumgc,
  menusav, menucrd.
- **Dep 0x02** — mngrp.bin sub-file 2. Required by: menupty, menusts,
  menuabl, menushop, menuext, menuitem, menugf, menujnc2, menututo,
  menutmag, menutips, menutest.

The game caches the current dep byte in `D_8008520A` to avoid redundant
loads — if the requested dep matches what's already loaded, the mngrp.bin
read is skipped.

### Load Address Regions

Most overlays load at either 0x801E5800 or 0x801E2800, meaning they are
mutually exclusive (only one can be resident at a time). menumain.ovl
loads higher at 0x801EF800, consistent with it being a persistent "main
menu" module that stays loaded while sub-menus swap in below it.

---

## Loading Functions

The overlay loading call chain lives in `src/10DD0.c` and corresponding
assembly in `asm/nonmatchings/1C38/`. All four core functions are now
fully decompiled.

### `func_80035FF4` — CD read queue (0x80035FF4)

Queues a CD read command into the ring buffer at `D_80085168`.
Decompiled in `src/10DD0.c:1249`.

```c
void func_80035FF4(s32 cmd, s32 overlay_id, s32 param, s32 load_addr,
                   s32 callback1, s32 callback2) {
    s32 write_idx = D_80085140;
    u8 *slot = D_80085168 + write_idx * 20;
    *(s16 *)(slot + 0x0) = cmd;
    *(s16 *)(slot + 0x2) = param;
    *(s16 *)(slot + 0x4) = overlay_id;
    *(s32 *)(slot + 0x8) = load_addr;
    *(s32 *)(slot + 0xC) = callback1;
    *(s32 *)(slot + 0x10) = callback2;
    func_800472E4();                          // enter critical section
    was_equal = D_80085140;
    D_80085140 = (was_equal + 1) & 7;        // advance write index (mod 8)
    was_equal = was_equal == D_80085144;      // check if buffer was full
    func_800472F4();                          // leave critical section
    if (was_equal == 1) {
        func_80035D30(cmd, overlay_id, param, load_addr);  // overflow: process immediately
    }
}
```

Ring buffer slot layout (20 bytes each):

| Offset | Size | Field |
|--------|------|-------|
| 0x00 | s16 | cmd (always 0 for overlay loads) |
| 0x02 | s16 | param (-1 for overlays, dep byte for mngrp.bin) |
| 0x04 | s16 | overlay_id (index into `load_table`) |
| 0x08 | s32 | load_addr (RAM destination address) |
| 0x0C | s32 | callback1 (function pointer, 0 if none) |
| 0x10 | s32 | callback2 (function pointer, 0 if none) |

The write index update is wrapped in `func_800472E4`/`func_800472F4`
(likely interrupt disable/enable) to protect the shared ring buffer
from concurrent access by the CD read consumer.

### `func_800360D0` — mngrp.bin dependency loader (0x800360D0)

Decompiled in `src/10DD0.c:1275`. Loads a sub-file from `mngrp.bin`
(overlay index 0x11 = 17) to a given address. Called by `func_80036104`
when a menu overlay requires a dependency to be loaded first.

```c
void func_800360D0(s32 a0, s32 a1) {
    func_80035FF4(0, 0x11, a0, a1, 0, 0);
}
```

The first arg (`a0`) becomes the `param` field in the ring buffer slot,
identifying which sub-file within mngrp.bin to load. The second arg
(`a1`) is the load address (always 0x801E0000 from callers).

### `func_80036104` — Main overlay loader (0x80036104)

Entry point for loading a menu overlay by index. Looks up the overlay
descriptor in `load_table`, optionally loads an mngrp.bin dependency
first, then queues the overlay itself. Decompiled in `src/10DD0.c:1279`.

```c
void func_80036104(s32 overlay_id, s32 a1, s32 a2) {
    extern u32 load_table[];  // 0x80053C58
    extern s32 D_8008514C;
    extern u8 D_8008520A;
    u32 descriptor;
    s32 dep;
    u32 *p;

    p = load_table + overlay_id * 2;
    descriptor = *p;
    D_8008514C = -2;                          // signal "loading"
    dep = descriptor & 0xFF;
    descriptor &= 0xFFFFFF00;
    if (dep != D_8008520A) {
        if (dep != 0) {
            func_800360D0(dep, 0x801E0000);   // load mngrp.bin sub-file
            D_8008520A = dep;                 // cache loaded dep
        }
    }
    func_80035FF4(0, overlay_id, -1, descriptor, a1, a2);
}
```

### `func_800361C0` — Direct address loader (0x800361C0)

Decompiled in `src/10DD0.c:1301`. Loads an overlay by index and explicit
load address, bypassing the `load_table` descriptor lookup and
dependency check. No callbacks.

```c
void func_800361C0(s32 a0, s32 a1) {
    func_80035FF4(0, a0, -1, a1, 0, 0);
}
```

### `func_80036444` — menumain.ovl loader (0x80036444)

Decompiled in `src/10DD0.c:1331`. Convenience wrapper that loads
overlay index 0 (menumain.ovl) with no callbacks.

```c
void func_80036444(void) {
    func_80036104(0, 0, 0);
}
```

---

## Key Data Addresses

| Address | Symbol | Size | Role |
|---------|--------|------|------|
| 0x80053C58 | `load_table` | 152 bytes (19 x 8) | Overlay descriptor table (addr+dep, filename ptr) |
| 0x80085140 | `D_80085140` | 4 bytes | CD command ring buffer write index |
| 0x80085144 | `D_80085144` | 4 bytes | CD command ring buffer read index |
| 0x8008514C | `D_8008514C` | 4 bytes | Loading status flag (set to -2 during load) |
| 0x80085168 | `D_80085168` | 160 bytes (8 x 20) | CD command ring buffer (8 slots, 20 bytes each) |
| 0x8008520A | `D_8008520A` | 1 byte | Currently loaded dependency byte (avoids redundant loads) |

The ring buffer at `D_80085168` spans 0x80085168–0x80085204 (160 bytes).
Each slot is 20 bytes (5 words). The buffer has 8 slots, indexed modulo 8.

---

## Development Path String

At 0x800562A4 in .data:

```
y:\menu\us\data\areames\areames
```

This is a leftover build path from Square's development environment,
revealing the original source tree structure. The `us` directory confirms
this is the USA localization branch. The `areames` path suggests area
message data was compiled from `y:\menu\us\data\areames\`.

---

## Source File Location

All overlay loading functions live in `src/10DD0.c` (file offset 0x10DD0
in the original binary, corresponding to the code segment starting at
that offset):

| Line | Function | Status |
|------|----------|--------|
| 1249 | `func_80035FF4` | Decompiled — CD read queue |
| 1275 | `func_800360D0` | Decompiled — mngrp.bin dependency loader |
| 1279 | `func_80036104` | Decompiled — main overlay loader |
| 1301 | `func_800361C0` | Decompiled — direct address loader |
| 1331 | `func_80036444` | Decompiled — menumain.ovl convenience wrapper |

---

## Unknown / Future Work

- **Non-menu overlays**: Battle, field, and world map modules likely use
  different loading paths. The battle system has its own overlay
  architecture not yet documented.

- **LGP archives**: Field and battle data use the LGP container format
  (documented by the PC modding community) rather than raw ISO 9660
  files. The PS1 version may use a similar packing. Not yet traced.

- **mngrp.bin sub-file format**: The dependency system loads sub-files
  from mngrp.bin by index (1 or 2), but the internal structure of
  mngrp.bin — how sub-files are indexed and sized — is not yet
  documented.

- **Ring buffer consumer**: The function that processes queued CD commands
  from `D_80085168` (the read side, advancing `D_80085144`) has not been
  identified. It likely runs from a VSync or CD interrupt callback.

- **func_800472E4 / func_800472F4**: Called by `func_80035FF4` around the
  ring buffer write index update. Likely interrupt disable/enable for
  critical section protection. Not yet decompiled.

- **func_80035D30** (0x80035D30): Called by `func_80035FF4` when the ring
  buffer write index equals the read index (overflow). Synchronous CD
  read fallback path. Not yet decompiled.

- **Additional overlay strings**: Search for other `.ovl`, `.out`, `.bin`
  filename strings in the binary to discover non-menu overlay loading
  paths. Trace other `CdSearchFile()` / `firstfile()` call sites.
