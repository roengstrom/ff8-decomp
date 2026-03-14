# FF8 Overlay System & CD Data Architecture

## Overview

FF8's executable (SLUS_008.92) is a ~260KB main binary loaded at 0x80010000. The bulk
of game code lives in **dynamically loaded overlays** that occupy a shared region at
0x80098000, overwriting each other as the game transitions between field, battle, and
menu modes.

Total decompilable code across all modules: **~2,700 functions**.

## Disc Layout

The disc image is a raw .bin file with 2,352 bytes per sector (24-byte header + 2,048
bytes user data). All game files are accessed through a central **CD File Table**.

### CD File Table

- **133 entries** of `CdFileDesc` (8 bytes each: `u32 sector; u32 size;`)
- Loaded from CD sector 826 to RAM address **0x80097400** by `func_80011E18()`
- Bootstrap descriptor `g_fileTableDesc` at 0x80051694 in the main binary points to
  sector 826, size 1064 (= 133 × 8)

### File Table Descriptor Aliases

The main binary references file table entries through aliased pointers at fixed offsets
within the table. These are declared as separate `extern CdFileDesc` arrays in the
source, but they all point into the single table at 0x80097400:

| Symbol | Address | Entry | Used By | Purpose |
|--------|---------|-------|---------|---------|
| `D_80097400[0]` | 0x80097400 | 0 | `func_80011C68` | Init overlay (raw) |
| `D_80097400[1]` | 0x80097408 | 1 | `GameMain` | Display init overlay (LZSS) |
| `D_80097410[0]` | 0x80097410 | 2 | `func_80011A60` | Field engine (LZSS) |
| `D_80097410[1]` | 0x80097418 | 3 | `func_80011B60` | Battle scene table |
| `D_800974B8[0]` | 0x800974B8 | 23 | `func_80021358` | Battle engine (LZSS) |
| `D_800974C0[0]` | 0x800974C0 | 24 | `func_80021358` | Battle render (raw) |
| `D_800974C8[0]` | 0x800974C8 | 25 | `func_80021358` | Battle code (raw) |
| `D_800974D0[0]` | 0x800974D0 | 26 | `func_80011AE0` | Alt field engine (LZSS) |
| `D_800974D8[0-2]` | 0x800974D8 | 27-29 | `func_80011BA8` | Sound banks |
| `D_80097808[0-4]` | 0x80097808 | 129-132 | `func_80011D3C` | Textures/models |

### CD Read Modes

Two primary CD read functions with different command bytes:

| Function | Cmd | Type | Description |
|----------|-----|------|-------------|
| `func_8003882C` | 3 | Raw | Read raw data from CD to RAM |
| `func_80038868` | 7 | LZSS | Read + decompress via streaming callback |

Both call `func_80038760` which computes `sector_count = ceil(size / 2048)` and
initiates the CD read. For cmd=7, `func_80039444` installs `func_80039520` as the
completion callback, which performs streaming LZSS decompression.

Blocking wrappers:
- `func_80038920`: blocking raw read (`func_8003882C` + spin on `func_800393C8`)
- `func_80038994`: blocking decompressing read (`func_80038868` + spin)

## LZSS Compression

`func_80039520` implements standard LZSS (Lempel-Ziv-Storer-Szymanski) decompression:

- **Sliding window**: 4,096 bytes (12-bit offset, masked with 0xFFF)
- **Offset bias**: -0xFEE (-4078)
- **Flag byte**: 8 bits, each controlling one token (1 = literal, 0 = back-reference)
- **Literal**: copy 1 byte from source to output
- **Back-reference**: 2 source bytes → (12-bit offset, 4-bit length), copies length+3
  bytes (3-18) from the sliding window
- **Header**: first 4 bytes = token count (decremented: -1 per literal, -2 per ref)
- **Streaming**: calls `func_800394D0` to read next CD sector when buffer exhausts

The token count is NOT the decompressed size — the actual output can be much larger
due to back-reference expansion.

## Overlay Inventory

### Code Overlays (all load to 0x80098000)

| Entry | CD Size | Decompressed | Functions | Type | File in `original/` | Purpose |
|-------|---------|-------------|-----------|------|---------------------|---------|
| 0 | 2,450 B | 2,450 B | 8 | Raw | `field_init.bin` | Init stub |
| 1 | 2,530 B | 4,784 B | 11 | LZSS | `display_init.bin` | Display init |
| **2** | 102,523 B | **191,872 B** | **321** | LZSS | `field_engine.bin` | **Field engine** |
| **23** | 683,741 B | **1,092,672 B** | 134 + data | LZSS | `battle_engine.bin` | Battle engine + data |
| 24 | 80,376 B | 80,376 B | 18 | Raw | `battle_render.bin` | Battle render |
| **25** | 347,208 B | 347,208 B | **854** | Raw | `battle_code.bin` | **Battle code** |
| **26** | 118,482 B | **198,197 B** | **226** | LZSS | `field_engine_alt.bin` | Alt field engine |

### Menu Overlays (load to 0x801Exxxx)

Entries 4-20 in the file table. 17 overlays, all raw MIPS code. Already set up for
decomp with splat configs in `config/` and build rules in the Makefile.

### Data/Asset Entries

| Entry | Size | Purpose |
|-------|------|---------|
| 3 | 1,728 B | Battle scene table (loaded to 0x80097940) |
| 21 | 2,263,040 B | `mngrp.bin` — menu group packed data (~142 resources) |
| 22 | 2,812 B | String data ("Uscx" header) |
| 27-29 | 9-131 KB | Sound banks (AKAO format, loaded to SPU RAM) |
| 30-127 | 30K-270K each | Field map data (98 entries, header word = 0x00000002) |
| 128-130 | 408 B - 38 KB | Small data tables |
| 131-132 | 408-492 B | Image/sound data |

## Overlay Loading Sequence

### Boot / Initialization (GameMain, before main loop)

```
func_80011E18()     →  Load file table (sector 826) to 0x80097400
func_80011BA8()     →  Load sound banks [27-29] to SPU RAM
func_80011C68()     →  Load entry 0 (raw 2450B init stub) to 0x80098000
func_80098000()     →  Run entry 0's init code
func_80011D0C()     →  Reload entry 0, call func_80098390 + func_80028444
```

### Restart Point (re-entered after game over)

```
func_80038868(entry 1)  →  LZSS-decompress entry 1 (4784B) to 0x80098000
func_80098FD4(0)        →  Display init (GPU setup, ClearImage, SetDispMask)
func_80011D3C()         →  Reload entry 0, load textures/models [129-132]
func_80011A60()         →  LZSS-decompress entry 2 (192KB field engine) to 0x80098000
func_800C00C8(0/1)      →  Enter field (within entry 2's code)
```

### Main Game Loop State Machine

The main loop dispatches on `g_vsyncRate`:

| State | Handler | Overlay at 0x80098000 |
|-------|---------|----------------------|
| 1 | Field update | Entry 2 (field engine) |
| 2 | Field-to-battle transition | Entry 2 → loading battle |
| 3-4 | Battle map init + execution | Entries 24/25 (battle code) |
| 5 | Battle alt render | Entry 23 (battle engine + data) |
| 6, 10 | Disc change | Reloads entry 1 + entry 2 or 26 |
| 8 | Post-battle cleanup | Entry 24 (battle render) |
| 9 | Overlay reload | Entry 1 (display init) |

### Battle State Machine (func_80021358 in 10DD0.c)

```
case 4 (render):  func_80038920(entry 24, raw 80KB)   →  calls func_80098238()
case 3 (init):    func_80038920(entry 25, raw 347KB)   →  calls func_80099D30()
case 8 (alt):     func_80038994(entry 23, LZSS 1MB+)   →  calls func_80098304()
```

## Notable Details

### Build Date in Entry 24

Entry 24 (battle render, 80,376 bytes) begins with a build timestamp:

```
"Jul  4 1999\00021:03:43\000"
```

This is the only overlay with an embedded build date. The date (July 4, 1999) is
consistent with FF8's USA release timeline (September 1999).

### Entry 25 Function Pointer Table

Entry 25 (battle code, 347,208 bytes) starts with a dispatch table of function
pointers — addresses within the overlay itself:

```
[0] 0x80099DB8  [1] 0x80099DA8  [2] 0x80099DB8
[3] 0x80099DC8  [4] 0x80099DD8  [5] 0x00000000
[6] 0x8009AE3C  [7] 0x8009AE4C  ...
```

This vtable pattern is typical for PS1 battle engines — the main binary calls overlay
functions through this fixed dispatch table.

### Entry 23 Code vs Data Split

Entry 23 (battle engine + data, 1,092,672 bytes decompressed) is 96% data. Code
occupies only the first ~50KB (0x80098000-0x800A4584), with `jr $ra` instructions
confirming ~171 function returns. The remaining ~1MB is battle graphics, animations,
and configuration data.

### Field Engine Variants

- **Entry 2** (field engine, loaded by `func_80011A60`): Primary field code
- **Entry 26** (alt field engine, loaded by `func_80011AE0`): Used during disc swaps

Both are LZSS-compressed and load to 0x80098000 via the decompressing CD read.
Entry 26 is loaded when `D_8005F14C` indicates a non-field phase and a different
CD file descriptor is needed (different disc data layout).

## Extraction

Decompressed overlays are stored in `original/`:

```
original/field_init.bin       →  Entry 0  (raw)
original/display_init.bin     →  Entry 1  (LZSS decompressed)
original/field_engine.bin     →  Entry 2  (LZSS decompressed)
original/battle_engine.bin    →  Entry 23 (LZSS decompressed)
original/battle_render.bin    →  Entry 24 (raw)
original/battle_code.bin      →  Entry 25 (raw)
original/field_engine_alt.bin →  Entry 26 (LZSS decompressed)
```

These were extracted from disc using the LZSS decompressor that mirrors
`func_80039520`. The extraction script is at `/tmp/ff8_decompress.py`.

## Function Count Summary

| Module | Functions | Notes |
|--------|-----------|-------|
| Main binary (SLUS_008.92) | ~868 | Game init, CD system, state machines |
| Field engine (entry 2) | 321 | World map, field logic |
| Battle code (entry 25) | 854 | Battle system (largest overlay) |
| Battle engine (entry 23) | ~171 | Battle + graphics data (96% data) |
| Alt field engine (entry 26) | 226 | Disc-swap field variant |
| Menu overlays (entries 4-20) | ~200+ | 17 menu screens |
| Small overlays (0, 1, 24) | 37 | Init stubs, display setup |
| **Total** | **~2,700** | |
