# FF8 Disc 1 — Complete File Table Reference

This documents every entry in the FF8DISC1.IMG file table on Disc 1 (USA, SLUS_008.92).

## Disc Structure

The PS1 disc image contains only 3 files at the ISO 9660 level:

| File | Sector | Size | Description |
|------|--------|------|-------------|
| SYSTEM.CNF | 23 | 68 B | PS1 boot configuration |
| SLUS_008.92 | 24 | 1,642,496 B (1.6 MB) | Main executable, loaded at 0x80010000 |
| FF8DISC1.IMG | 826 | 635,625,472 B (606.2 MB) | Game data container (all other content) |

Everything except the executable and boot config is packed inside FF8DISC1.IMG.

## FF8DISC1.IMG File Table

The IMG begins with a file table: 133 entries of 8 bytes each (`u32 sector, u32 size`),
totaling 1,064 bytes. At runtime, `func_80011E18()` loads this table from CD sector 826
to RAM address 0x80097400.

Entries are grouped by type:

### Code Overlays (entries 0–2, 23–26)

Dynamic code modules loaded to 0x80098000, overwriting each other during game state
transitions. Entries 1, 2, 23, and 26 are LZSS-compressed on disc.

| Entry | Sector | Raw Size | Decomp Size | Compression | Name | Description |
|------:|-------:|---------:|------------:|-------------|------|-------------|
| 0 | 827 | 2,450 B | — | Raw | field_init.bin | Field engine init stub. First instruction is `addiu sp, sp, -0x28`. Sets up field mode then jumps to field_engine. |
| 1 | 829 | 2,530 B | 4,784 B | LZSS | display_init.bin | Display/GPU init overlay. Initializes rendering state, ordering tables, and display buffers. |
| 2 | 33,241 | 102,523 B | 191,872 B | LZSS | field_engine.bin | Main field engine (321 functions). Handles field map rendering, entity movement, scripting, and NPC interaction. |
| 23 | 963 | 683,741 B | 1,092,672 B | LZSS | battle_engine.bin | Battle engine + data (134 functions + embedded data). Core battle state machine, damage calculation, ATB system. |
| 24 | 98,935 | 80,376 B | — | Raw | battle_render.bin | Battle renderer (18 functions). Contains build timestamp "Jul 4 1999 21:03:43" and debug strings (SCENE, TIME, RATE100, AP*10, EXP*10, DOGOUT, ODIN, FORMATION, WITCH, etc.). |
| 25 | 98,975 | 347,208 B | — | Raw | battle_code.bin | Battle code (854 functions). Starts with a pointer table into 0x8009xxxx address space. Contains battle AI, animations, command processing. |
| 26 | 1,297 | 118,482 B | 198,197 B | LZSS | field_engine_alt.bin | Alternate field engine (226 functions). Used for specific field scenarios (possibly world map transitions or FMV-integrated fields). |

### Entry 3 — Scene Lighting Data

| Entry | Sector | Size | Description |
|------:|-------:|-----:|-------------|
| 3 | 33,292 | 1,728 B | 36 records of 48 bytes each. Each record contains position coordinates (4 × s16), an RGBA color quad (4 identical bytes, e.g. 0x50505050, 0x80808080, 0x44444444), followed by a second set of coordinates and another RGBA quad. Likely per-scene or per-area lighting/ambient color definitions used during field engine init. |

### Menu Overlays (entries 4–20)

Raw MIPS binary overlays for the 17 menu screens. Each loads to 0x801Exxxx–0x801Fxxxx,
overlapping each other. All are compiled with PsyQ 4.1 (with occasional PsyQ 4.3 functions
mixed in).

| Entry | Sector | Size | Name | Description |
|------:|-------:|-----:|------|-------------|
| 4 | 97,664 | 43,640 B | menumain.ovl | Main menu hub — party status, navigation between sub-menus. Largest menu overlay. |
| 5 | 97,686 | 6,380 B | menucfg.ovl | Config/settings menu — controller mapping, sound/stereo options, window colors. |
| 6 | 97,690 | 15,680 B | menupty.ovl | Party select menu — choose active party of 3 from available characters. |
| 7 | 97,698 | 18,148 B | menusts.ovl | Status screen — character stats, elemental affinities, status resistances. |
| 8 | 97,707 | 5,508 B | menuabl.ovl | Ability menu — junction GF abilities, party abilities, character abilities. |
| 9 | 97,710 | 21,032 B | menushop.ovl | Shop menu — buy/sell items, weapons, and magazines. |
| 10 | 97,721 | 13,328 B | menuext.ovl | Extended menu — additional options/commands. |
| 11 | 97,728 | 41,760 B | menuitem.ovl | Item menu — inventory management, item usage, sorting. Second largest menu overlay. |
| 12 | 97,749 | 29,168 B | menumgc.ovl | Magic menu — view/cast magic, magic junctioning display. |
| 13 | 97,764 | 9,684 B | menugf.ovl | GF menu — Guardian Force status, abilities, AP progress, boost settings. |
| 14 | 97,769 | 37,968 B | menujnc2.ovl | Junction menu — stat junctioning interface, auto/manual junction, ability slots. |
| 15 | 97,788 | 39,568 B | menusav.ovl | Save/load menu — memory card save/load, slot selection. |
| 16 | 97,808 | 9,572 B | menucrd.ovl | Card menu — Triple Triad card collection viewer. |
| 17 | 97,813 | 9,920 B | menututo.ovl | Tutorial menu — in-game tutorial/help text viewer. |
| 18 | 97,818 | 7,756 B | menutmag.ovl | T-Mag (Timber Maniacs) menu — magazine collection viewer. |
| 19 | 97,822 | 4,880 B | menutips.ovl | Tips/information menu — gameplay tips display. |
| 20 | 97,825 | 4,540 B | menutest.ovl | Test/debug menu — development test screen (11 functions). |

### Data Files (entries 21–22)

| Entry | Sector | Size | Name | Description |
|------:|-------:|-----:|------|-------------|
| 21 | 97,828 | 2,263,040 B (2.2 MB) | mngrp.bin | Menu graphics resource pack. Largest single file on disc. Contains TIM textures and graphics data for all menu screens. First bytes suggest TIM-like structure (0x10 at byte 0). |
| 22 | 98,933 | 2,812 B | init.out | Game initialization data. Header signature "Uscx_amrj" followed by structured records (each ~80 bytes) with sub-signatures like "Wfgt_". Contains initial game state parameters, screen resolution (240×240 and 256×240 referenced), and configuration defaults. |

### AKAO Sound/Music (entries 27–29, 132)

AKAO is Square's proprietary PS1 audio format, used for both sequenced music and sound
effects across many Square titles (FF7, FF8, FF9, Chrono Cross, etc.).

| Entry | Sector | Size | AKAO ID | Description |
|------:|-------:|-----:|--------:|-------------|
| 27 | 877 | 9,408 B | 0x0000 | Sound effect bank. Contains an offset table in the header (offsets at 0x18: 0x0F, 0x1E, 0x2C, 0x36, 0x45...) pointing to individual instrument/sample definitions. |
| 28 | 882 | 32,672 B | 0x0000 | Sound effect/instrument bank. Larger collection of audio samples. |
| 29 | 898 | 131,424 B | 0x0005 | Music sequence data. Largest standalone AKAO entry. Contains sequenced music tracks with instrument mapping data (sample rate 0xB000, buffer 0x1FF40). |
| 132 | 876 | 492 B | 0x0004 | Small AKAO entry — likely a single sound effect or jingle (e.g., menu cursor sound, confirm/cancel). |

### Field Map Data (entries 30–127)

98 field map archives, each a self-contained package for one game location. All share the
same container format:

**Structure:**
- `u32 section_count` — always 2
- `u32 offsets[section_count + 2]` — byte offsets to each section within the archive
- **Section 0**: AKAO — location-specific background music / ambient sound
- **Section 1**: Short data block (often zeros + small header, ~24 bytes between section offsets)
- **Remaining data**: Field geometry, background tiles, walkmesh, entity placement, camera angles, and event scripts. Bulk of the archive. No executable MIPS code.

Field maps contain **no code overlays** — all field logic is in the field_engine (entry 2).
The field engine interprets the scripting data embedded in each field archive.

| Entry | Sector | Size | AKAO Size | Data Size | Notes |
|------:|-------:|-----:|----------:|----------:|-------|
| 30 | 87,667 | 252,540 B | 4,348 B | 248,168 B | |
| 31 | 87,791 | 130,452 B | 4,260 B | 126,168 B | |
| 32 | 87,855 | 115,628 B | 7,276 B | 108,328 B | |
| 33 | 87,912 | 131,060 B | 948 B | 130,088 B | |
| 34 | 87,976 | 134,908 B | 3,452 B | 131,432 B | |
| 35 | 88,042 | 130,916 B | 8,740 B | 122,152 B | |
| 36 | 88,106 | 124,340 B | 132 B | 124,184 B | Tiny AKAO — possibly silent/ambient-only |
| 37 | 88,167 | 122,780 B | 5,980 B | 116,776 B | |
| 38 | 88,227 | 121,236 B | 3,540 B | 117,672 B | |
| 39 | 88,287 | 118,916 B | 2,516 B | 116,376 B | |
| 40 | 88,346 | 12,692 B | 100 B | 12,568 B | Smallest field — possibly a transition/connector room |
| 41 | 88,353 | 103,788 B | 588 B | 103,176 B | |
| 42 | 88,404 | 137,996 B | 9,356 B | 128,616 B | |
| 43 | 88,472 | 127,156 B | 7,044 B | 120,088 B | |
| 44 | 88,535 | 269,924 B | 8,420 B | 261,480 B | Large field — likely a major story location |
| 45 | 88,667 | 101,476 B | 2,308 B | 99,144 B | |
| 46 | 88,717 | 129,132 B | 3,852 B | 125,256 B | |
| 47 | 88,781 | 142,772 B | 13,444 B | 129,304 B | Large AKAO — complex music |
| 48 | 88,851 | 262,852 B | 4,916 B | 257,912 B | Large field |
| 49 | 88,980 | 257,500 B | 8,108 B | 249,368 B | Large field |
| 50 | 89,106 | 227,812 B | 3,172 B | 224,616 B | |
| 51 | 89,218 | 131,204 B | 3,268 B | 127,912 B | |
| 52 | 89,283 | 258,292 B | 2,212 B | 256,056 B | Large field |
| 53 | 89,410 | 269,748 B | 8,244 B | 261,480 B | Large field |
| 54 | 89,542 | 243,460 B | 3,476 B | 239,960 B | |
| 55 | 89,661 | 227,244 B | 2,412 B | 224,808 B | |
| 56 | 89,772 | 263,916 B | 2,940 B | 260,952 B | Large field |
| 57 | 89,901 | 264,348 B | 2,396 B | 261,928 B | Large field |
| 58 | 90,031 | 269,756 B | 14,764 B | 254,968 B | Largest AKAO in field set — rich musical score |
| 59 | 90,163 | 129,812 B | 3,028 B | 126,760 B | |
| 60 | 90,227 | 231,436 B | 812 B | 230,600 B | |
| 61 | 90,341 | 231,308 B | 684 B | 230,600 B | |
| 62 | 90,454 | 194,380 B | 684 B | 193,672 B | |
| 63 | 90,549 | 194,388 B | 692 B | 193,672 B | |
| 64 | 90,644 | 217,620 B | 596 B | 217,000 B | |
| 65 | 90,751 | 142,988 B | 11,612 B | 131,352 B | Large AKAO |
| 66 | 90,821 | 134,644 B | 2,948 B | 131,672 B | |
| 67 | 90,887 | 113,740 B | 156 B | 113,560 B | Tiny AKAO |
| 68 | 90,943 | 106,140 B | 476 B | 105,640 B | |
| 69 | 90,995 | 131,748 B | 708 B | 131,016 B | |
| 70 | 91,060 | 95,228 B | 156 B | 95,048 B | Tiny AKAO |
| 71 | 91,107 | 133,356 B | 2,892 B | 130,440 B | |
| 72 | 91,173 | 131,076 B | 884 B | 130,168 B | |
| 73 | 91,238 | 271,604 B | 12,516 B | 259,064 B | Large field + large AKAO |
| 74 | 91,371 | 124,396 B | 396 B | 123,976 B | |
| 75 | 91,432 | 176 B | 52 B | 100 B | Minimal field — nearly empty placeholder (has valid AKAO at offset 0x14 but only 176 bytes total) |
| 76 | 91,433 | 247,820 B | 2,156 B | 245,640 B | |
| 77 | 91,555 | 226,788 B | 1,444 B | 225,320 B | |
| 78 | 91,666 | 257,284 B | 2,180 B | 255,080 B | |
| 79 | 91,792 | 191,524 B | 1,876 B | 189,624 B | |
| 80 | 91,886 | 120,788 B | 7,044 B | 113,720 B | |
| 81 | 91,945 | 264,100 B | 2,084 B | 261,992 B | Large field |
| 82 | 92,074 | 218,444 B | 2,572 B | 215,848 B | |
| 83 | 92,181 | 121,252 B | 3,556 B | 117,672 B | |
| 84 | 92,241 | 260,548 B | 1,812 B | 258,712 B | Large field |
| 85 | 92,369 | 238,892 B | 1,356 B | 237,512 B | |
| 86 | 92,486 | 267,452 B | 4,748 B | 262,680 B | Large field |
| 87 | 92,617 | 126,924 B | 3,724 B | 123,176 B | |
| 88 | 92,679 | 134,892 B | 3,228 B | 131,640 B | |
| 89 | 92,745 | 134,676 B | 3,188 B | 131,464 B | |
| 90 | 92,811 | 111,084 B | 1,164 B | 109,896 B | |
| 91 | 92,866 | 109,092 B | 1,236 B | 107,832 B | |
| 92 | 92,920 | 134,420 B | 5,076 B | 129,320 B | |
| 93 | 92,986 | 133,684 B | 3,012 B | 130,648 B | |
| 94 | 93,052 | 42,484 B | 564 B | 41,896 B | Small field |
| 95 | 93,073 | 127,516 B | 1,116 B | 126,376 B | |
| 96 | 93,136 | 264,492 B | 1,340 B | 263,128 B | Large field |
| 97 | 93,266 | 133,124 B | 2,020 B | 131,080 B | |
| 98 | 93,332 | 257,836 B | 2,652 B | 255,160 B | |
| 99 | 93,458 | 120,396 B | 1,500 B | 118,872 B | |
| 100 | 93,517 | 130,732 B | 2,348 B | 128,360 B | |
| 101 | 93,581 | 131,644 B | 1,468 B | 130,152 B | |
| 102 | 93,646 | 261,428 B | 3,540 B | 257,864 B | Large field |
| 103 | 93,774 | 138,924 B | 7,468 B | 131,432 B | |
| 104 | 93,842 | 57,748 B | 100 B | 57,624 B | |
| 105 | 93,871 | 269,524 B | 6,820 B | 262,680 B | Large field |
| 106 | 94,003 | 136,148 B | 4,596 B | 131,528 B | |
| 107 | 94,070 | 116,908 B | 4,028 B | 112,856 B | |
| 108 | 94,128 | 270,516 B | 8,980 B | 261,512 B | Large field |
| 109 | 94,261 | 267,644 B | 5,676 B | 261,944 B | Large field |
| 110 | 94,392 | 124,996 B | 2,100 B | 122,872 B | |
| 111 | 94,454 | 243,444 B | 3,044 B | 240,376 B | |
| 112 | 94,573 | 131,876 B | 3,028 B | 128,824 B | |
| 113 | 94,638 | 134,636 B | 2,972 B | 131,640 B | |
| 114 | 94,704 | 129,860 B | 3,444 B | 126,392 B | |
| 115 | 94,768 | 132,604 B | 3,916 B | 128,664 B | |
| 116 | 94,833 | 240,092 B | 2,380 B | 237,688 B | |
| 117 | 94,951 | 30,700 B | 140 B | 30,536 B | Small field, tiny AKAO |
| 118 | 94,966 | 240,228 B | 2,516 B | 237,688 B | |
| 119 | 95,084 | 191,044 B | 4,804 B | 186,216 B | |
| 120 | 95,178 | 140,124 B | 10,508 B | 129,592 B | Large AKAO |
| 121 | 95,247 | 209,308 B | 1,324 B | 207,960 B | |
| 122 | 95,350 | 165,140 B | 2,308 B | 162,808 B | |
| 123 | 95,431 | 138,284 B | 6,892 B | 131,368 B | |
| 124 | 95,499 | 90,788 B | 244 B | 90,520 B | |
| 125 | 95,544 | 114,164 B | 260 B | 113,880 B | |
| 126 | 95,600 | 260,164 B | 2,676 B | 257,464 B | |
| 127 | 95,728 | 130,180 B | 1,364 B | 128,792 B | |

### Supplemental Data (entries 128–131)

These entries are stored near the beginning of the IMG (sectors 831–876), adjacent to
the code overlays rather than in the field data region.

| Entry | Sector | Size | Description |
|------:|-------:|-----:|-------------|
| 128 | 831 | 37,992 B | **World map / field init data container.** Contains 14 sub-sections accessed via an offset table (first u32 = 0x38 = 56 bytes = 14 × 4-byte offsets). Sub-sections vary widely in size (96 B to 21 KB) and contain: entity/NPC placement tables, camera angle definitions, walkmesh data, trigger zone coordinates, scripted event parameters, color palette data, and TIM textures (sub-section 11 starts with 0x10 = TIM magic). This is the startup field's scene definition data. |
| 129 | 850 | 15,756 B | **4-bit indexed pixel data.** Header: count=8, data offset=0x6C. The payload consists of nibble-packed pixel values (dominant values 0x0, 0x8, 0xF in histogram). Likely a 4-bit texture atlas or font bitmap used during field initialization. |
| 130 | 858 | 33,312 B | **TIM image** (PS1 standard texture format). Type: 4-bit color, no CLUT. Used as a pre-loaded texture resource during early game initialization. |
| 131 | 875 | 408 B | **String/data offset table.** 204 u16 values. The first 36 values are monotonically increasing (32, 66, 75, 82, ... up to ~25K), then the pattern breaks — suggesting a combined offset table + inline data structure. Possibly indexes into init.out (entry 22) or mngrp.bin (entry 21). |

## Disc Sector Map

Entries are not stored sequentially by index. The physical disc layout, sorted by sector:

```
Sector       Size    Entry  Description
───────────────────────────────────────────────────
     23       68 B    —     SYSTEM.CNF
     24     1.6 MB    —     SLUS_008.92 (executable)
    826     1064 B    —     IMG file table (133 entries)
    827      2.4 KB    0    field_init.bin
    829      2.5 KB    1    display_init.bin (LZSS)
    831     37.1 KB  128    World map/field init data
    850     15.4 KB  129    4-bit pixel data
    858     32.5 KB  130    TIM image
    875      0.4 KB  131    Offset table
    876      0.5 KB  132    AKAO sound
    877      9.2 KB   27    AKAO sound bank
    882     31.9 KB   28    AKAO sound bank
    898    128.3 KB   29    AKAO music sequence
    963    667.7 KB   23    battle_engine.bin (LZSS)
  1,297    115.7 KB   26    field_engine_alt.bin (LZSS)
 33,241    100.1 KB    2    field_engine.bin (LZSS)
 33,292      1.7 KB    3    Scene lighting data
             ···           (gap: sectors 33,293–87,666)
 87,667–95,792        30–127  98 field map archives (contiguous)
             ···           (gap: sectors 95,793–97,663)
 97,664–97,828        4–20   17 menu overlays (contiguous)
 97,828–98,933        21     mngrp.bin (menu graphics, 2.2 MB)
 98,933–98,935        22     init.out
 98,935–98,975        24     battle_render.bin
 98,975–99,145        25     battle_code.bin
```

**Notable gaps:**
- Sectors 1,355–33,240 (~62 MB) — between field_engine_alt and field_engine. Likely FMV/movie data not referenced by the file table.
- Sectors 33,293–87,666 (~106 MB) — between scene lighting data and first field map. Another large FMV region.
- Sectors 95,793–97,663 (~3.6 MB) — between last field map and menu overlays. Possible additional FMV or unused space.

## Summary Statistics

| Category | Entries | Total Size |
|----------|--------:|-----------:|
| Code overlays | 7 | 1,337,310 B (1.3 MB raw) |
| Menu overlays | 17 | 318,924 B (311 KB) |
| Data files (mngrp + init) | 2 | 2,265,852 B (2.2 MB) |
| AKAO sound/music | 4 | 173,996 B (170 KB) |
| Field map archives | 98 | ~16.0 MB |
| Scene lighting | 1 | 1,728 B |
| Supplemental data | 4 | 87,468 B (85 KB) |
| **Total indexed** | **133** | **~20.0 MB** |

The remaining ~586 MB of FF8DISC1.IMG (not referenced by the file table) is primarily
FMV video data, accessed by sector address through the game's movie playback system
rather than through the file table.
