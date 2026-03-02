# PsyQ SDK Cross-Project Decomp References

Cross-reference mapping FF8's `src/psxsdk/` INCLUDE_ASM functions to matching C code
in other PS1 decompilation projects.

**Reference projects:**
- **sotn-decomp**: `~/source/sotn-decomp/` (paths relative to repo root)
- **psy-q-decomp**: `~/source/psy-q-decomp/` (paths relative to repo root)

**Summary:** 149 functions mapped (39 BIOS trampolines + 110 real C functions),
10 with trailing nop warnings.

---

## libapi (29 matches)

FF8 source: `src/psxsdk/libapi.c`

### BIOS Trampolines (22)

3-instruction `jr $t2` stubs. Not decompilable to C — reference shows BIOS table/index.

| FF8 Function | BIOS Call | sotn-decomp | psy-q-decomp |
|---|---|---|---|
| `ChangeClearPAD` | 0xB0 #0x5B | `src/main/psxsdk/libapi/a91.c` | — |
| `ChangeClearRCnt` | 0xC0 #0xA | `src/main/psxsdk/libapi/l10.c` | — |
| `CloseEvent` | 0xB0 #0x9 | `src/main/psxsdk/libapi/a09.c` | — |
| `DeliverEvent` | 0xB0 #0x7 | `src/main/psxsdk/libapi/a07.c` | — |
| `DisableEvent` | 0xB0 #0xD | `src/main/psxsdk/libapi/a13.c` | — |
| `EnableEvent` | 0xB0 #0xC | `src/main/psxsdk/libapi/a12.c` | — |
| `FlushCache` | 0xA0 #0x44 | `src/main/psxsdk/libapi/c68.c` | — |
| `HookEntryInt` | 0xB0 #0x19 | `src/main/psxsdk/libapi/a25.c` | — |
| `InitHeap` | 0xA0 #0x39 | `src/main/psxsdk/libapi/c57.c` | — |
| `OpenEvent` | 0xB0 #0x8 | `src/main/psxsdk/libapi/a08.c` | — |
| `PAD_dr` | 0xB0 #0x16 | `src/main/psxsdk/libapi/a22.c` | — |
| `ResetEntryInt` | 0xB0 #0x18 | `src/main/psxsdk/libapi/a24.c` | — |
| `ReturnFromException` | 0xB0 #0x17 | `src/main/psxsdk/libapi/a23.c` | — |
| `TestEvent` | 0xB0 #0xB | `src/main/psxsdk/libapi/a11.c` | — |
| `WaitEvent` | 0xB0 #0xA | `src/main/psxsdk/libapi/a10.c` | — |
| `close` | 0xB0 #0x36 | `src/main/psxsdk/libapi/a54.c` | — |
| `erase` | 0xB0 #0x45 | `src/main/psxsdk/libapi/a69.c` | — |
| `lseek` | 0xB0 #0x33 | `src/main/psxsdk/libapi/a51.c` | — |
| `nextfile` | 0xB0 #0x43 | `src/main/psxsdk/libapi/a67.c` | — |
| `open` | 0xB0 #0x32 | `src/main/psxsdk/libapi/a50.c` | — |
| `read` | 0xB0 #0x34 | `src/main/psxsdk/libapi/a52.c` | — |
| `write` | 0xB0 #0x35 | `src/main/psxsdk/libapi/a53.c` | — |

### C Functions (7)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `_bu_init` | 6 | | `src/main/psxsdk/libapi/c112.c` | — |
| `ResetRCnt` | 11 | | `src/main/psxsdk/libapi/counter.c` | `src/api/counter.c` |
| `GetRCnt` | 12 | | `src/main/psxsdk/libapi/counter.c` | `src/api/counter.c` |
| `StartRCnt` | 12 | | `src/main/psxsdk/libapi/counter.c` | `src/api/counter.c` |
| `StopRCnt` | 13 | | `src/main/psxsdk/libapi/counter.c` | `src/api/counter.c` |
| `SetRCnt` | 29 | | `src/main/psxsdk/libapi/counter.c` | `src/api/counter.c` |
| `firstfile` | 52 | | `src/main/psxsdk/libapi/a66.c` | — |

---

## libc (14 matches)

FF8 source: `src/psxsdk/libc.c`

### BIOS Trampolines (12)

| FF8 Function | BIOS Call | sotn-decomp | psy-q-decomp |
|---|---|---|---|
| `memchr` | 0xA0 #0x2E | `src/main/psxsdk/libc/c46.c` | — |
| `memcpy` | 0xA0 #0x2A | `src/main/psxsdk/libc/c42.c` | — |
| `memset` | 0xA0 #0x2B | `src/main/psxsdk/libc/c43.c` | — |
| `printf` | 0xA0 #0x3F | `src/main/psxsdk/libc/c63.c` | — |
| `rand` | 0xA0 #0x2F | `src/main/psxsdk/libc/c47.c` | — |
| `setjmp` | 0xA0 #0x13 | `src/main/psxsdk/libc/c19.c` | — |
| `srand` | 0xA0 #0x30 | `src/main/psxsdk/libc/c48.c` | — |
| `strcat` | 0xA0 #0x15 | `src/main/psxsdk/libc/c21.c` | — |
| `strcmp` | 0xA0 #0x17 | `src/main/psxsdk/libc/c23.c` | — |
| `strcpy` | 0xA0 #0x19 | `src/main/psxsdk/libc/c25.c` | — |
| `strlen` | 0xA0 #0x1B | `src/main/psxsdk/libc/c27.c` | — |
| `strncmp` | 0xA0 #0x18 | `src/main/psxsdk/libc/c24.c` | — |

### C Functions (2)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `memmove` | 25 | | `src/main/psxsdk/libc/memmove.c` | `src/c/memmove.c` |
| `sprintf` | 37 | | `src/main/psxsdk/libc/sprintf.c` | — |

---

## libcard (5 matches)

FF8 source: `src/psxsdk/libcard.c`

### BIOS Trampolines (4)

| FF8 Function | BIOS Call | sotn-decomp | psy-q-decomp |
|---|---|---|---|
| `_card_info` | 0xA0 #0xAB | `src/main/psxsdk/libcard/c171.c` | — |
| `_card_load` | 0xA0 #0xAC | `src/main/psxsdk/libcard/c172.c` | — |
| `_card_write` | 0xB0 #0x4E | `src/main/psxsdk/libcard/a78.c` | — |
| `_new_card` | 0xB0 #0x50 | `src/main/psxsdk/libcard/a80.c` | — |

### C Functions (1)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `_card_clear` | 16 | trailing nop | `src/main/psxsdk/libcard/card.c` | — |

---

## libcd (28 matches)

FF8 source: `src/psxsdk/libcd.c`

### C Functions (28)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `CD_set_test_parmnum` | 3 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CdLastPos` | 3 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdReadCallback` | 5 | | `src/main/psxsdk/libcd/cdread.c` | — |
| `CdReadyCallback` | 5 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdSetDebug` | 5 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdSyncCallback` | 5 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdComstr` | 11 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdIntstr` | 11 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CD_initintr` | 19 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CdRead` | 22 | | `src/main/psxsdk/libcd/cdread.c` | — |
| `CdReset` | 23 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdInit` | 29 | | `src/main/psxsdk/libcd/event.c` | — |
| `CdPosToInt` | 32 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CD_vol` | 34 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CdReadSync` | 36 | | `src/main/psxsdk/libcd/cdread.c` | — |
| `CD_flush` | 53 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CD_initvol` | 60 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CD_getsector` | 64 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CdIntToPos` | 65 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdSearchFile` | 66 | | `src/main/psxsdk/libcd/iso9660.c` | — |
| `CD_datasync` | 70 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CdControlB` | 71 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CdControlF` | 77 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CD_ready` | 78 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CD_sync` | 78 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CdControl` | 79 | | `src/main/psxsdk/libcd/sys.c` | — |
| `CD_init` | 116 | | `src/main/psxsdk/libcd/bios.c` | — |
| `CD_cw` | 179 | | `src/main/psxsdk/libcd/bios.c` | — |

---

## libetc (4 matches)

FF8 source: `src/psxsdk/libetc.c`

### C Functions (4)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `SetIntrMask` | 6 | | `src/main/psxsdk/libetc/intr.c` | `src/etc/intr.c` |
| `startIntrDMA` | 19 | | `src/main/psxsdk/libetc/intr_dma.c` | `src/etc/intr_dma.c` |
| `startIntrVSync` | 22 | | — | `src/etc/intr_vb.c` |
| `VSync` | 41 | | `src/main/psxsdk/libetc/vsync.c` | `src/etc/vsync.c` |

---

## libgpu (33 matches)

FF8 source: `src/psxsdk/libgpu.c`

### BIOS Trampolines (1)

| FF8 Function | BIOS Call | sotn-decomp | psy-q-decomp |
|---|---|---|---|
| `GPU_cw` | 0xA0 #0x49 | `src/main/psxsdk/libapi/c73.c` | — |

### C Functions (32)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `GetClut` | 8 | trailing nop | `src/main/psxsdk/libgpu/prim.c` | — |
| `SetPriority` | 10 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `GetODE` | 12 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `GetDispEnv` | 13 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `GetDrawEnv` | 13 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetTexWindow` | 14 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetDefDispEnv` | 15 | | `src/main/psxsdk/libgpu/ext.c` | — |
| `AddPrim` | 16 | trailing nop | `src/main/psxsdk/libgpu/prim.c` | — |
| `AddPrims` | 16 | trailing nop | `src/main/psxsdk/libgpu/prim.c` | — |
| `GetTPage` | 16 | trailing nop | `src/main/psxsdk/libgpu/prim.c` | — |
| `SetDrawOffset` | 16 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetDrawMode` | 21 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `DrawPrim` | 23 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetGraphDebug` | 23 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `LoadImage` | 24 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `StoreImage` | 24 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `LoadClut` | 25 | | `src/main/psxsdk/libgpu/ext.c` | — |
| `DrawSync` | 26 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `DrawOTag` | 28 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetDefDrawEnv` | 30 | | `src/main/psxsdk/libgpu/ext.c` | — |
| `SetDrawArea` | 32 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `ClearImage` | 36 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetDispMask` | 38 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `MoveImage` | 40 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetGraphQueue` | 41 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `LoadTPage` | 42 | | `src/main/psxsdk/libgpu/ext.c` | — |
| `ClearOTagR` | 43 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `PutDrawEnv` | 48 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `ClearOTag` | 50 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `ResetGraph` | 68 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `SetDrawEnv` | 87 | | `src/main/psxsdk/libgpu/sys.c` | — |
| `PutDispEnv` | 104 | | `src/main/psxsdk/libgpu/sys.c` | — |

---

## libgte (5 matches)

FF8 source: `src/psxsdk/libgte.c`

### C Functions (5)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `rsin` | 11 | | `src/main/psxsdk/libgte/geo_00.c` | — |
| `SetFogNear` | 28 | trailing nop | `src/main/psxsdk/libgte/fog_01.c` | — |
| `sin_1` | 34 | | `src/main/psxsdk/libgte/geo_00.c` | — |
| `rcos` | 38 | | `src/main/psxsdk/libgte/geo_01.c` | — |
| `ratan2` | 45 | | `src/main/psxsdk/libgte/ratan.c` | — |

---

## libpress (7 matches)

FF8 source: `src/psxsdk/libpress.c`

### C Functions (7)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `DecDCTBufSize` | 3 | | — | `src/press/libpress.c` |
| `DecDCTinSync` | 11 | | — | `src/press/libpress.c` |
| `DecDCToutSync` | 11 | | — | `src/press/libpress.c` |
| `DecDCTin` | 12 | | — | `src/press/libpress.c` |
| `DecDCTReset` | 13 | | — | `src/press/libpress.c` |
| `DecDCTGetEnv` | 35 | | — | `src/press/libpress.c` |
| `DecDCTPutEnv` | 38 | | — | `src/press/libpress.c` |

---

## libsnd_reverb (1 match)

FF8 source: `src/psxsdk/libsnd_reverb.c`

### C Functions (1)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `SsUtReverbOff` | 8 | | `src/main/psxsdk/libsnd/ut_roff.c` | `src/snd/ut_roff.c` |

---

## libspu (19 matches)

FF8 source: `src/psxsdk/libspu.c`

### C Functions (19)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `SpuSetTransferMode` | 7 | | `src/main/psxsdk/libspu/s_stm.c` | `src/spu/s_stm.c` |
| `_SpuIsInAllocateArea` | 11 | | `src/main/psxsdk/libspu/s_m_util.c` | `src/spu/s_m_util.c` |
| `_spu_FgetRXXa` | 13 | | `src/main/psxsdk/libspu/spu.c` | — |
| `_SpuIsInAllocateArea_` | 14 | | `src/main/psxsdk/libspu/s_m_util.c` | `src/spu/s_m_util.c` |
| `_spu_FsetRXX` | 15 | | `src/main/psxsdk/libspu/spu.c` | — |
| `SpuSetIRQCallback` | 16 | trailing nop | `src/main/psxsdk/libspu/s_sic.c` | `src/spu/s_sic.c` |
| `SpuInitMalloc` | 19 | | `src/main/psxsdk/libspu/s_m_init.c` | `src/spu/s_m_init.c` |
| `SpuSetTransferStartAddr` | 19 | | `src/main/psxsdk/libspu/s_stsa.c` | `src/spu/s_stsa.c` |
| `SpuRead` | 24 | trailing nop | `src/main/psxsdk/libspu/s_r.c` | `src/spu/s_r.c` |
| `SpuStart` | 30 | trailing nop | `src/main/psxsdk/libspu/s_ini.c` | `src/spu/s_ini.c` |
| `SpuClearReverbWorkArea` | 39 | | `src/main/psxsdk/libspu/s_crwa.c` | `src/spu/s_crwa.c` |
| `_spu_FsetRXXa` | 39 | | `src/main/psxsdk/libspu/spu.c` | — |
| `_spu_FiDMA` | 43 | | `src/main/psxsdk/libspu/spu.c` | — |
| `SpuSetReverb` | 44 | | `src/main/psxsdk/libspu/s_sr.c` | `src/spu/s_sr.c` |
| `_spu_init` | 53 | | `src/main/psxsdk/libspu/spu.c` | — |
| `_SpuInit` | 58 | | `src/main/psxsdk/libspu/s_ini.c` | `src/spu/s_ini.c` |
| `SpuSetIRQ` | 61 | | `src/main/psxsdk/libspu/s_si.c` | — |
| `_spu_t` | 122 | | `src/main/psxsdk/libspu/spu.c` | — |
| `_spu_setReverbAttr` | 308 | | `src/main/psxsdk/libspu/s_srmp.c` | — |

---

## pad (2 matches)

FF8 source: `src/psxsdk/libetc/pad.c`

### C Functions (2)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `StopPAD` | 14 | | `src/main/psxsdk/libapi/a20.c` | — |
| `PAD_init` | 36 | | `src/main/psxsdk/libapi/a21.c` | — |

---

## pad_init (2 matches)

FF8 source: `src/psxsdk/libetc/pad_init.c`

### C Functions (2)

| FF8 Function | Insns | Notes | sotn-decomp | psy-q-decomp |
|---|---|---|---|---|
| `PadStop` | 11 | trailing nop | `src/main/psxsdk/libetc/pad.c` | `src/etc/pad.c` |
| `PadInit` | 19 | | `src/main/psxsdk/libetc/pad.c` | `src/etc/pad.c` |

---

## Summary by Category

| Category | BIOS Trampolines | C Functions | Total |
|---|---|---|---|
| libapi | 22 | 7 | 29 |
| libc | 12 | 2 | 14 |
| libcard | 4 | 1 | 5 |
| libcd | — | 28 | 28 |
| libetc | — | 4 | 4 |
| libgpu | 1 | 32 | 33 |
| libgte | — | 5 | 5 |
| libpress | — | 7 | 7 |
| libsnd_reverb | — | 1 | 1 |
| libspu | — | 19 | 19 |
| pad | — | 2 | 2 |
| pad_init | — | 2 | 2 |
| **Total** | **39** | **110** | **149** |

### Trailing Nop Warnings (10)

These functions have trailing nop padding after `jr $ra` in the .s files. Decomping
to C will lose those bytes, causing a size mismatch that shifts all subsequent functions.

| Function | Library | Insns |
|---|---|---|
| `_card_clear` | libcard | 16 |
| `GetClut` | libgpu | 8 |
| `AddPrim` | libgpu | 16 |
| `AddPrims` | libgpu | 16 |
| `GetTPage` | libgpu | 16 |
| `SetFogNear` | libgte | 28 |
| `SpuSetIRQCallback` | libspu | 16 |
| `SpuRead` | libspu | 24 |
| `SpuStart` | libspu | 30 |
| `PadStop` | pad_init | 11 |
