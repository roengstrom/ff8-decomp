# Leaf Functions

A **leaf function** is a function that does not call any other functions (no `jal` or `jalr` instructions in MIPS). These are the simplest functions to decompile since they have no dependencies on other function signatures.

**Total functions**: 1253
**Leaf functions**: 629 (50.2%)

---

## Game Functions (func\_\*)

These are the game-specific functions, the primary targets for decompilation. **213 leaf functions, 146 decompiled (68.5%)**.

| Function | Size | Instructions | Status |
|----------|------|-------------|--------|
| func_80011438 | 0xE8 | 61 |      |
| func_80011870 | 0x164 | 89 |      |
| func_800119D4 | 0x8C | 35 |      |
| func_80012E04 | 0xA8 | 42 |      |
| func_800146F0 | 0x50 | 20 |      |
| func_80014834 | 0x2C | 11 |      |
| func_80014DC4 | 0x1C | 7 | Done |
| func_80014DE0 | 0x1C | 7 | Done |
| func_80014DFC | 0x1C | 7 | Done |
| func_80014E18 | 0x1C | 7 | Done |
| func_80014E34 | 0x1C | 7 | Done |
| func_80014E50 | 0x48 | 18 | Done |
| func_80014E98 | 0x18 | 6 | Done |
| func_80014EB0 | 0x1C | 7 | Done |
| func_80014ECC | 0x1C | 7 | Done |
| func_80014EE8 | 0x18 | 6 | Done |
| func_80014F00 | 0x18 | 6 | Done |
| func_80014F18 | 0x30 | 12 | Done |
| func_80014F48 | 0x28 | 10 | Done |
| func_80014F70 | 0x28 | 10 | Done |
| func_80014F98 | 0x34 | 13 | Done |
| func_80014FCC | 0x30 | 12 | Done |
| func_80014FFC | 0xAC | 43 |      |
| func_800151C4 | 0x4A4 | 297 |      |
| func_80015668 | 0x460 | 280 |      |
| func_80015AC8 | 0x484 | 289 |      |
| func_80016280 | 0x80 | 32 |      |
| func_80016300 | 0x44 | 17 |      |
| func_80016478 | 0x50 | 20 |      |
| func_800164C8 | 0x10 | 4 | Done |
| func_80016A1C | 0x60 | 24 |      |
| func_80016DB4 | 0x54 | 21 |      |
| func_80016E08 | 0x54 | 21 |      |
| func_80016E5C | 0xE0 | 56 |      |
| func_80016F3C | 0x6C | 27 | Done |
| func_80017040 | 0x4C | 19 |      |
| func_80017A2C | 0x80 | 32 |      |
| func_80017C9C | 0x78 | 30 |      |
| func_80017D14 | 0x48 | 18 |      |
| func_80017D5C | 0x54 | 21 |      |
| func_8001A55C | 0x20 | 8 | Done |
| func_8001A57C | 0x80 | 32 |      |
| func_8001A5FC | 0x78 | 30 |      |
| func_8001B1F4 | 0x20C | 131 |      |
| func_8001B400 | 0x2C | 11 | Done |
| func_8001B690 | 0x190 | 100 |      |
| func_8001C1A8 | 0x34 | 13 |      |
| func_8001C214 | 0x6C | 27 |      |
| func_8001C280 | 0x48 | 18 |      |
| func_8001D280 | 0x60 | 24 | Done |
| func_8001D2E0 | 0x68 | 26 | Done |
| func_8001D348 | 0x74 | 29 | Done |
| func_8001D3BC | 0x68 | 26 | Done |
| func_8001D424 | 0x60 | 24 | Done |
| func_8001D484 | 0x164 | 89 |      |
| func_8001D5E8 | 0x34 | 13 | Done |
| func_8001F0C4 | 0x54 | 21 |      |
| func_8001F5C8 | 0x1008 | 1026 |      |
| func_80020FBC | 0x78 | 30 |      |
| func_80021034 | 0x25C | 151 |      |
| func_80021290 | 0x70 | 28 |      |
| func_80021300 | 0x58 | 22 |      |
| func_8002172C | 0x60 | 24 |      |
| func_80021944 | 0x64 | 25 |      |
| func_800219A8 | 0x10 | 4 | Done |
| func_800219B8 | 0x28 | 10 |      |
| func_800219E0 | 0x84 | 33 |      |
| func_80021B58 | 0xB8 | 46 |      |
| func_8002216C | 0x48 | 18 |      |
| func_80022328 | 0x48 | 18 |      |
| func_80022370 | 0x94 | 37 |      |
| func_800227F4 | 0x100 | 64 |      |
| func_800228F4 | 0x48 | 18 |      |
| func_80022B04 | 0x44 | 17 |      |
| func_80022B48 | 0x88 | 34 |      |
| func_80022BD0 | 0x34 | 13 | Done |
| func_80022C5C | 0x80 | 32 |      |
| func_80022CDC | 0x9C | 39 |      |
| func_80022D78 | 0x90 | 36 |      |
| func_80023180 | 0x30 | 12 | Done |
| func_800231B0 | 0x18 | 6 | Done |
| func_800231C8 | 0x18 | 6 |      |
| func_80023900 | 0xC | 3 | Done |
| func_80026F4C | 0x68 | 26 |      |
| func_80026FD4 | unknown | 3 |      |
| func_80027038 | 0x78 | 30 |      |
| func_800273D8 | 0x30 | 12 |      |
| func_8002795C | 0x70 | 28 |      |
| func_800279CC | 0x8C | 35 |      |
| func_80027BA8 | 0x150 | 84 |      |
| func_80027DB4 | 0x144 | 81 |      |
| func_80027EF8 | 0x40 | 16 |      |
| func_800283CC | 0x10 | 4 | Done |
| func_800283DC | 0x1C | 7 |      |
| func_800284BC | 0xA8 | 42 |      |
| func_8002871C | 0x1C | 7 | Done |
| func_80028768 | 0x28 | 10 | Done |
| func_800287B0 | 0xC | 3 | Done |
| func_800287BC | 0x38 | 14 |      |
| func_800287F4 | 0x1C | 7 | Done |
| func_80028810 | 0x1C | 7 | Done |
| func_8002A2A8 | 0x1C | 7 | Done |
| func_8002A2F4 | 0xB4 | 45 |      |
| func_8002A408 | 0x30 | 12 |      |
| func_8002A438 | 0x24 | 9 | Done |
| func_8002A888 | 0x30 | 12 |      |
| func_8002AAC0 | 0x9C | 39 |      |
| func_8002AC74 | 0x14 | 5 | Done |
| func_8002AC88 | 0x34 | 13 | Done |
| func_8002ACBC | 0x1C | 7 | Done |
| func_8002ACD8 | 0x2C | 11 |      |
| func_8002AD04 | 0x38 | 14 |      |
| func_8002AD3C | 0xD8 | 54 |      |
| func_8002AE14 | 0x1C | 7 | Done |
| func_8002AE30 | 0x30 | 12 | Done |
| func_8002AE60 | 0x18 | 6 | Done |
| func_8002AE78 | 0x18 | 6 | Done |
| func_8002AE90 | 0x34 | 13 |      |
| func_8002AEC4 | 0x34 | 13 |      |
| func_8002AF54 | 0x1C | 7 | Done |
| func_8002AF70 | 0x34 | 13 |      |
| func_8002B080 | 0xEC | 59 |      |
| func_8002B3A0 | 0x4F8 | 318 |      |
| func_8002B8BC | 0x1C4 | 113 |      |
| func_8002C100 | 0x14 | 5 |      |
| func_8002C114 | 0x10 | 4 | Done |
| func_8002C124 | 0xC | 3 | Done |
| func_8002C56C | 0x1C8 | 114 |      |
| func_8002C7E0 | 0x28 | 10 | Done |
| func_8002C808 | 0x20 | 8 | Done |
| func_8002C828 | 0x20 | 8 | Done |
| func_8002C848 | 0x20 | 8 | Done |
| func_8002C8A4 | 0x7C | 31 |      |
| func_8002CA10 | 0x24 | 9 | Done |
| func_8002CA34 | 0x24 | 9 | Done |
| func_8002CA58 | 0x24 | 9 | Done |
| func_8002CA7C | 0x20 | 8 | Done |
| func_8002CA9C | 0x44 | 17 |      |
| func_8002CDE4 | 0xE8 | 58 |      |
| func_8002CECC | 0x88 | 34 |      |
| func_8002DE74 | 0x20 | 8 | Done |
| func_8002E298 | 0x10C | 67 |      |
| func_8002E3A4 | 0x84 | 33 |      |
| func_8002E428 | 0x84 | 33 |      |
| func_8002F23C | 0x58 | 22 |      |
| func_8002F294 | 0x58 | 22 |      |
| func_8002F2EC | 0x34 | 13 |      |
| func_8002F320 | 0x64 | 25 |      |
| func_8002F384 | 0x1C | 7 | Done |
| func_8002F488 | 0x28 | 10 | Done |
| func_8002F548 | 0x6C | 27 |      |
| func_8002F5B4 | 0x5C | 23 |      |
| func_8002FF24 | 0x10 | 4 | Done |
| func_8002FF34 | 0x124 | 73 |      |
| func_80030058 | 0xA0 | 40 |      |
| func_800300F8 | 0x11C | 71 |      |
| func_8003023C | 0xC | 3 | Done |
| func_80030248 | 0x40 | 16 |      |
| func_80030288 | 0x54 | 21 |      |
| func_80030720 | 0x28 | 10 | Done |
| func_80030748 | 0xC | 3 | Done |
| func_80030A54 | 0xD8 | 54 |      |
| func_80030F10 | 0xCC | 51 |      |
| func_80030FDC | 0x68 | 26 |      |
| func_8003104C | 0x13C | 79 |      |
| func_80031224 | 0x140 | 80 |      |
| func_800318EC | 0x24 | 9 | Done |
| func_80031910 | 0x44 | 17 |      |
| func_80031DF4 | 0x28 | 10 |      |
| func_80031F2C | 0x24 | 9 | Done |
| func_80031F9C | 0xC | 3 | Done |
| func_80031FA8 | 0x8 | 2 | Done |
| func_8003334C | 0x34 | 13 |      |
| func_80033380 | 0xEC | 59 |      |
| func_80033768 | 0x94 | 37 |      |
| func_80035148 | 0x10 | 4 | Done |
| func_80035C54 | 0xC | 3 | Done |
| func_80035C60 | 0x10 | 4 | Done |
| func_800366E8 | 0x28 | 10 | Done |
| func_80036710 | 0x7C | 31 |      |
| func_8003678C | 0xD0 | 52 |      |
| func_8003685C | 0x11C | 71 |      |
| func_80036978 | 0x54 | 21 |      |
| func_80036EC0 | 0xA0 | 40 |      |
| func_80036F60 | 0x44 | 17 |      |
| func_80036FA4 | 0x3C | 15 |      |
| func_8003786C | 0x28 | 10 | Done |
| func_80037894 | 0x1C | 7 | Done |
| func_80037ACC | unknown | 57 |      |
| func_80038490 | 0x120 | 72 |      |
| func_80038CE0 | 0x10 | 4 | Done |
| func_80039444 | 0x38 | 14 |      |
| func_8003947C | 0x54 | 21 |      |
| func_80039678 | 0xB0 | 44 |      |
| func_80039728 | 0x3C | 15 |      |
| func_80039AA0 | 0x14 | 5 |      |
| func_8003B024 | 0x1C | 7 |      |
| func_8003B334 | 0x38 | 14 |      |
| func_8003BB78 | 0x20 | 8 | Done |
| func_8003BB98 | 0x14 | 5 | Done |
| func_8003BBAC | 0x20 | 8 | Done |
| func_8003BBCC | 0x20 | 8 | Done |
| func_8003BBEC | 0x20 | 8 | Done |
| func_8003BC0C | 0x14 | 6 |      |
| func_8003C744 | 0x20 | 8 |      |
| func_8003C764 | 0xA0 | 40 |      |
| func_8003D0C4 | 0x1C | 8 |      |
| func_8003E494 | 0x24 | 12 |      |
| func_8003ED24 | 0x28 | 12 |      |
| func_8003ED54 | 0x10 | 4 | Done |
| func_800408C4 | unknown | 8 |      |
| func_800408E4 | unknown | 4 |      |
| func_800432D8 | 0x10 | 7 |      |
| func_800471BC | 0xC | 6 |      |
| func_800472E4 | unknown | 4 |      |
| func_800472F4 | unknown | 4 |      |
| func_80047384 | unknown | 4 |      |
| func_80047C3C | 0xC | 6 |      |
| func_8004D174 | 0x94 | 37 |      |
| func_8004D208 | 0x60 | 24 |      |
| func_8004D4F0 | 0x30 | 13 |      |
| func_8004D604 | 0x28 | 12 |      |
| func_8004D634 | 0xC4 | 52 |      |
| func_8004D704 | 0x14 | 8 |      |
| func_8004E4C8 | 0x20 | 8 |      |
| func_8004E4E8 | 0xA0 | 40 |      |
| func_8004E720 | 0x14C | 85 |      |
| func_8004E874 | 0x50 | 20 |      |
| func_80050874 | 0x350 | 212 |      |
| func_800512F4 | 0x60 | 24 |      |
| func_800514D4 | 0x40 | 16 |      |
| func_80051514 | unknown | 92 |      |

### Easiest Game Leaf Functions (10 or fewer instructions)

These are the best starting points for decompilation:

| Function | Size | Instructions | Status |
|----------|------|-------------|--------|
| func_80031FA8 | 0x8 | 2 | Done |
| func_80023900 | 0xC | 3 | Done |
| func_80026FD4 | unknown | 3 |      |
| func_8002C124 | 0xC | 3 | Done |
| func_80030748 | 0xC | 3 | Done |
| func_80031F9C | 0xC | 3 | Done |
| func_80035C54 | 0xC | 3 | Done |
| func_800287B0 | 0xC | 3 | Done |
| func_8003023C | 0xC | 3 | Done |
| func_800164C8 | 0x10 | 4 | Done |
| func_800219A8 | 0x10 | 4 | Done |
| func_800283CC | 0x10 | 4 | Done |
| func_800408E4 | unknown | 4 |      |
| func_800472E4 | unknown | 4 |      |
| func_800472F4 | unknown | 4 |      |
| func_80047384 | unknown | 4 |      |
| func_80035148 | 0x10 | 4 | Done |
| func_80035C60 | 0x10 | 4 | Done |
| func_80038CE0 | 0x10 | 4 | Done |
| func_8002C114 | 0x10 | 4 | Done |
| func_8002FF24 | 0x10 | 4 | Done |
| func_8003ED54 | 0x10 | 4 | Done |
| func_80039AA0 | 0x14 | 5 |      |
| func_8002AC74 | 0x14 | 5 | Done |
| func_8002C100 | 0x14 | 5 |      |
| func_8003BB98 | 0x14 | 5 | Done |
| func_80014E98 | 0x18 | 6 | Done |
| func_80014EE8 | 0x18 | 6 | Done |
| func_80014F00 | 0x18 | 6 | Done |
| func_800231B0 | 0x18 | 6 | Done |
| func_800231C8 | 0x18 | 6 |      |
| func_8002AE60 | 0x18 | 6 | Done |
| func_8002AE78 | 0x18 | 6 | Done |
| func_800471BC | 0xC | 6 |      |
| func_80047C3C | 0xC | 6 |      |
| func_80014DC4 | 0x1C | 7 | Done |
| func_80014DE0 | 0x1C | 7 | Done |
| func_80014DFC | 0x1C | 7 | Done |
| func_80014E18 | 0x1C | 7 | Done |
| func_80014E34 | 0x1C | 7 | Done |
| func_80014EB0 | 0x1C | 7 | Done |
| func_80014ECC | 0x1C | 7 | Done |
| func_800283DC | 0x1C | 7 |      |
| func_8002871C | 0x1C | 7 | Done |
| func_800287F4 | 0x1C | 7 | Done |
| func_80028810 | 0x1C | 7 | Done |
| func_8002A2A8 | 0x1C | 7 | Done |
| func_8002ACBC | 0x1C | 7 | Done |
| func_8002AE14 | 0x1C | 7 | Done |
| func_8002AF54 | 0x1C | 7 | Done |
| func_8002F384 | 0x1C | 7 | Done |
| func_80037894 | 0x1C | 7 | Done |
| func_8003B024 | 0x1C | 7 |      |
| func_800432D8 | 0x10 | 7 |      |
| func_8001A55C | 0x20 | 8 | Done |
| func_8002C808 | 0x20 | 8 | Done |
| func_8002C828 | 0x20 | 8 | Done |
| func_8002C848 | 0x20 | 8 | Done |
| func_8002CA7C | 0x20 | 8 | Done |
| func_8002DE74 | 0x20 | 8 | Done |
| func_8003BB78 | 0x20 | 8 | Done |
| func_8003BBAC | 0x20 | 8 | Done |
| func_8003BBCC | 0x20 | 8 | Done |
| func_8003BBEC | 0x20 | 8 | Done |
| func_8003C744 | 0x20 | 8 |      |
| func_8004E4C8 | 0x20 | 8 |      |
| func_800408C4 | unknown | 8 |      |
| func_8003D0C4 | 0x1C | 8 |      |
| func_8004D704 | 0x14 | 8 |      |
| func_8002A438 | 0x24 | 9 | Done |
| func_80022B04 | 0x44 | 9 |      |
| func_800318EC | 0x24 | 9 | Done |
| func_80031F2C | 0x24 | 9 | Done |
| func_8002CA10 | 0x24 | 9 | Done |
| func_8002CA34 | 0x24 | 9 | Done |
| func_8002CA58 | 0x24 | 9 | Done |
| func_800219B8 | 0x28 | 10 |      |
| func_80014F48 | 0x28 | 10 | Done |
| func_80014F70 | 0x28 | 10 | Done |
| func_80028768 | 0x28 | 10 | Done |
| func_8002C7E0 | 0x28 | 10 | Done |
| func_8002F488 | 0x28 | 10 | Done |
| func_800366E8 | 0x28 | 10 | Done |
| func_8003786C | 0x28 | 10 | Done |
| func_80030720 | 0x28 | 10 | Done |
| func_80031DF4 | 0x28 | 10 |      |

---

## SDK / Library Functions (named)

These are PsyQ SDK, BIOS, and standard library functions. Generally not targets for decompilation. **249 leaf functions**.

| Function | Size | Instructions |
|----------|------|-------------|
| AddPrim | 0x3C | 16 |
| AddPrims | 0x3C | 16 |
| __addsf3 | 0xC4 | 49 |
| ApplyMatrix | unknown | 20 |
| ApplyMatrixLV | unknown | 88 |
| ApplyMatrixSV | unknown | 36 |
| ApplyTransposeMatrixLV | unknown | 110 |
| AverageSZ3 | unknown | 4 |
| AverageSZ4 | unknown | 7 |
| AverageZ3 | unknown | 8 |
| AverageZ4 | unknown | 9 |
| _bu_init | unknown | 6 |
| bzero | 0xC | 4 |
| _card_auto | 0xC | 4 |
| _card_info | 0xC | 4 |
| _card_load | 0xC | 4 |
| _card_read | 0xC | 4 |
| _card_wait | 0xC | 4 |
| _card_write | 0xC | 4 |
| CdComstr | 0x2C | 11 |
| CD_flush | 0xD4 | 53 |
| CD_getsector | 0x100 | 64 |
| CD_getsector2 | 0xEC | 59 |
| CD_initvol | 0xF0 | 60 |
| CdIntstr | 0x2C | 11 |
| CdIntToPos | 0x104 | 65 |
| CdLastCom | 0x10 | 4 |
| CdLastPos | 0xC | 3 |
| CdMode | 0x10 | 4 |
| CdPosToInt | 0x80 | 32 |
| CdRead | 0x58 | 22 |
| CdReadCallback | 0x14 | 5 |
| CdReadMode | 0x18 | 9 |
| CdReadyCallback | 0x14 | 5 |
| CdSetDebug | 0x14 | 5 |
| CD_set_test_parmnum | 0xC | 3 |
| CdStatus | 0x10 | 4 |
| CdSyncCallback | 0x14 | 5 |
| CD_vol | 0x88 | 34 |
| ChangeClearPAD | 0xC | 4 |
| ChangeClearRCnt | 0xC | 4 |
| ChangeTh | 0xC | 4 |
| CheckCallback | 0x10 | 4 |
| close | 0xC | 4 |
| CloseEvent | 0xC | 4 |
| CloseTh | 0xC | 4 |
| ColorCol | 0x24 | 9 |
| ColorDpq | unknown | 10 |
| CompMatrix | unknown | 88 |
| _copy_memcard_patch | 0x34 | 13 |
| _dbl_shift_us | 0x20 | 8 |
| DecDCTBufSize | 0xC | 3 |
| DecDCTGetEnv | 0x8C | 35 |
| DecDCTin | 0x30 | 12 |
| DecDCTvlc2 | unknown | 216 |
| DecDCTvlcBuild | 0xAC | 43 |
| DecDCTvlcSize2 | unknown | 12 |
| DeliverEvent | 0xC | 4 |
| DisableEvent | 0xC | 4 |
| DisablePAD | 0x14 | 5 |
| __divsf3 | 0xF0 | 60 |
| DpqColor | unknown | 7 |
| DpqColor3 | unknown | 15 |
| DpqColorLight | unknown | 10 |
| EnableEvent | 0xC | 4 |
| EnablePAD | 0x14 | 5 |
| erase | 0xC | 4 |
| _err_math | 0x4C | 19 |
| firstfile2 | 0xC | 4 |
| __floatsisf | 0x94 | 37 |
| FlushCache | 0xC | 4 |
| GetClut | 0x18 | 8 |
| GetGraphDebug | 0x10 | 4 |
| GetIntrMask | 0x18 | 6 |
| GetRCnt | 0x30 | 12 |
| GetTPage | 0x3C | 16 |
| GPU_cw | 0xC | 4 |
| HookEntryInt | 0xC | 4 |
| index | unknown | 25 |
| InitCARD2 | 0xC | 4 |
| InitHeap | 0xC | 4 |
| InitPAD2 | 0xC | 4 |
| Intpl | unknown | 9 |
| InvSquareRoot | unknown | 35 |
| LightColor | 0x28 | 10 |
| LoadAverage0 | unknown | 20 |
| LoadAverage12 | unknown | 20 |
| LoadAverageByte | unknown | 24 |
| LoadAverageCol | unknown | 32 |
| LoadAverageShort0 | unknown | 34 |
| LoadAverageShort12 | unknown | 34 |
| LoadTPage | 0xA8 | 42 |
| LocalLight | 0x24 | 9 |
| lseek | 0xC | 4 |
| __ltdf2 | 0xD4 | 53 |
| Lzc | unknown | 7 |
| memchr | 0xC | 4 |
| memcpy | 0xC | 4 |
| memmove | 0x64 | 25 |
| memset | 0xC | 4 |
| MulMatrix | unknown | 68 |
| MulMatrix0 | unknown | 68 |
| MulMatrix2 | unknown | 68 |
| __mulsf3 | 0xEC | 59 |
| nextfile | 0xC | 4 |
| NormalColor | 0x1C | 7 |
| NormalColor3 | 0x3C | 15 |
| NormalColorCol | 0x20 | 8 |
| NormalColorCol3 | 0x44 | 17 |
| NormalColorDpq | unknown | 9 |
| NormalColorDpq3 | 0x48 | 18 |
| open | 0xC | 4 |
| OpenEvent | 0xC | 4 |
| OpenTh | 0xC | 4 |
| OpenTIM | 0x10 | 4 |
| OuterProduct0 | unknown | 22 |
| OuterProduct12 | unknown | 22 |
| PAD_dr | 0xC | 4 |
| PAD_init2 | 0xC | 4 |
| printf | 0xC | 4 |
| rand | 0xC | 4 |
| ratan2 | 0xB4 | 45 |
| rcos | 0x98 | 38 |
| read | 0xC | 4 |
| ReadGeomOffset | unknown | 8 |
| ReadInitPadFlag | 0x10 | 4 |
| _remove_ChgclrPAD | unknown | 30 |
| ResetEntryInt | 0xC | 4 |
| ResetRCnt | 0x2C | 11 |
| ReturnFromException | 0xC | 4 |
| RotColorDpq | unknown | 20 |
| RotMatrix | 0x64 | 25 |
| RotMatrix_gte | unknown | 132 |
| RotMatrixX | 0x64 | 25 |
| RotMatrixY | 0x64 | 25 |
| RotMatrixYXZ | 0x68 | 26 |
| RotMatrixZ | 0x64 | 25 |
| RotMatrixZYX | 0x64 | 25 |
| RotTrans | unknown | 24 |
| RotTransPers | unknown | 12 |
| RotTransPers3 | unknown | 24 |
| RotTransPers4 | unknown | 32 |
| RotTransSV | unknown | 12 |
| ScaleMatrix | 0x138 | 80 |
| ScaleMatrixL | 0x138 | 80 |
| SetBackColor | unknown | 8 |
| SetColorMatrix | unknown | 12 |
| SetData32 | unknown | 3 |
| SetDefDispEnv | 0x3C | 15 |
| SetDQA | unknown | 3 |
| SetDQB | unknown | 3 |
| SetDrawLoad | 0x6C | 28 |
| SetDrawMove | 0x60 | 24 |
| SetDrawStp | 0x1C | 7 |
| SetDrawTPage | 0x2C | 12 |
| SetFarColor | unknown | 8 |
| SetFogNearFar | unknown | 70 |
| SetInitPadFlag | 0xC | 3 |
| SetIntrMask | 0x18 | 6 |
| SetIR0 | unknown | 3 |
| SetIR123 | unknown | 5 |
| setjmp | 0xC | 4 |
| SetLightMatrix | unknown | 12 |
| SetMAC123 | unknown | 5 |
| SetMem | 0xC | 4 |
| SetMulRotMatrix | unknown | 60 |
| SetPriority | 0x28 | 10 |
| SetRCnt | 0x74 | 29 |
| SetRGBfifo | 0x14 | 5 |
| SetRii | unknown | 5 |
| SetRotMatrix | unknown | 12 |
| SetSXSYfifo | unknown | 5 |
| SetSZfifo3 | unknown | 5 |
| SetSZfifo4 | unknown | 6 |
| SetTransMatrix | unknown | 8 |
| SetVertex0 | 0x10 | 4 |
| SetVertex1 | 0x10 | 4 |
| SetVertex2 | 0x10 | 4 |
| SetVertexTri | 0x20 | 8 |
| sin_1 | 0x88 | 34 |
| sprintf | 0x94 | 37 |
| _spu_FgetRXXa | 0x34 | 13 |
| _spu_FsetPCR | 0x4C | 19 |
| _spu_FsetRXX | 0x3C | 15 |
| _spu_FsetRXXa | 0x9C | 39 |
| _spu_Fw1ts | 0x40 | 16 |
| SpuGetVoiceEnvelope | 0x1C | 8 |
| SpuInitMalloc | 0x4C | 19 |
| _SpuIsInAllocateArea | 0x2C | 11 |
| _SpuIsInAllocateArea_ | 0x38 | 14 |
| SpuSetIRQ | 0xF4 | 61 |
| SpuSetNoiseClock | 0x48 | 20 |
| _spu_setReverbAttr | 0x4D0 | 308 |
| SpuSetTransferMode | 0x1C | 7 |
| Square0 | unknown | 10 |
| Square12 | unknown | 10 |
| SquareRoot0 | unknown | 36 |
| SquareRoot12 | unknown | 40 |
| SquareSL0 | unknown | 12 |
| SquareSL12 | unknown | 12 |
| SquareSS0 | unknown | 14 |
| SquareSS12 | unknown | 14 |
| srand | 0xC | 4 |
| start | unknown | 47 |
| StartCARD2 | 0xC | 4 |
| StartPAD2 | 0xC | 4 |
| StartRCnt | 0x30 | 12 |
| StopCARD2 | 0x80 | 32 |
| StopPAD2 | 0xC | 4 |
| StopRCnt | 0x34 | 13 |
| strcat | 0xC | 4 |
| strcmp | 0xC | 4 |
| strcpy | 0xC | 4 |
| strlen | 0xC | 4 |
| strncmp | 0xC | 4 |
| strtol | 0xC | 4 |
| SysDeqIntRP | 0xC | 4 |
| SysEnqIntRP | 0xC | 4 |
| SystemError | 0xC | 4 |
| TestEvent | 0xC | 4 |
| TransposeMatrix | 0x40 | 16 |
| TransRot_32 | unknown | 104 |
| TransRotPers | unknown | 29 |
| TransRotPers3 | unknown | 57 |
| UnDeliverEvent | 0xC | 4 |
| VectorNormalS | 0x14 | 5 |
| VSync | 0xA4 | 41 |
| WaitEvent | 0xC | 4 |
| write | 0xC | 4 |

---

## SDK Object File Fragments (*\_OBJ\_*)

Internal fragments from linked PsyQ SDK object files. Not decompilation targets. **167 leaf functions**.

| Function | Size | Instructions |
|----------|------|-------------|
| ADDSF3_OBJ_190 | 0x4 | 1 |
| ADDSF3_OBJ_194 | 0x10 | 7 |
| BIOS_OBJ_0 | 0x64 | 25 |
| BIOS_OBJ_1318 | 0x10 | 4 |
| BIOS_OBJ_1440 | 0x50 | 20 |
| BIOS_OBJ_1688 | 0x34 | 13 |
| BIOS_OBJ_25C | 0x100 | 64 |
| BIOS_OBJ_35C | 0x4C | 19 |
| BIOS_OBJ_3A8 | 0x80 | 32 |
| BIOS_OBJ_428 | 0x80 | 32 |
| BIOS_OBJ_548 | 0x14 | 5 |
| BIOS_OBJ_7AC | 0x30 | 12 |
| BIOS_OBJ_A74 | 0x30 | 12 |
| BIOS_OBJ_E88 | 0x28 | 10 |
| BUILD_OBJ_AC | 0x38 | 17 |
| CDREAD_OBJ_504 | 0x18 | 6 |
| CDREAD_OBJ_608 | 0xC | 3 |
| COUNTER_OBJ_164 | 0x8 | 3 |
| COUNTER_OBJ_74 | 0x20 | 8 |
| DBSHIFTU_OBJ_20 | 0x9C | 40 |
| DIVSF3_OBJ_130 | 0x4 | 1 |
| DIVSF3_OBJ_134 | 0x10 | 7 |
| EVENT_OBJ_74 | 0x10 | 4 |
| EXT_OBJ_228 | 0x3C | 15 |
| EXT_OBJ_A8 | 0x4 | 1 |
| FERR_OBJ_54 | 0x10 | 7 |
| FGO_01_OBJ_160 | 0x12C | 76 |
| FGO_01_OBJ_64 | 0x68 | 26 |
| FGO_01_OBJ_CC | 0x94 | 37 |
| FGO_02_OBJ_160 | 0x12C | 76 |
| FGO_02_OBJ_68 | 0x64 | 25 |
| FGO_02_OBJ_CC | 0x94 | 37 |
| FGO_03_OBJ_160 | 0x12C | 76 |
| FGO_03_OBJ_64 | 0x68 | 26 |
| FGO_03_OBJ_CC | 0x94 | 37 |
| FGO_04_OBJ_64 | 0x134 | 79 |
| FGO_05_OBJ_64 | 0x134 | 79 |
| FGO_06_OBJ_64 | 0x134 | 79 |
| FIRST_OBJ_180 | 0x1C | 7 |
| FIXSFSI_OBJ_98 | 0xC | 3 |
| FIXSFSI_OBJ_A4 | 0x10 | 7 |
| FLTSIDF_OBJ_DC | 0x24 | 9 |
| FLTSISF_OBJ_94 | 0x18 | 6 |
| FLTSISF_OBJ_AC | 0x8 | 5 |
| FORMAT_OBJ_320 | 0x30 | 12 |
| FORMAT_OBJ_F0 | 0x54 | 21 |
| GEO_00_OBJ_2C | 0x10 | 4 |
| GEO_00_OBJ_C4 | 0x8 | 3 |
| INTR_DMA_OBJ_1CC | 0xA4 | 41 |
| INTR_DMA_OBJ_278 | 0x3C | 15 |
| INTR_OBJ_43C | 0xB0 | 44 |
| INTR_OBJ_68C | 0x10 | 4 |
| INTR_OBJ_69C | 0x24 | 9 |
| INTR_VB_OBJ_C4 | 0x2C | 11 |
| INTR_VB_OBJ_F0 | 0x24 | 12 |
| ISO9660_OBJ_2B0 | 0x28 | 10 |
| ISO9660_OBJ_594 | 0x28 | 10 |
| ISO9660_OBJ_640 | 0x20 | 8 |
| ISO9660_OBJ_8D4 | 0x28 | 10 |
| LIBPRESS_OBJ_194 | 0x2C | 11 |
| LIBPRESS_OBJ_22C | 0x10 | 4 |
| LIBPRESS_OBJ_268 | 0x10 | 4 |
| LIBPRESS_OBJ_3A0 | 0x10 | 4 |
| LIBPRESS_OBJ_550 | 0x10 | 4 |
| LIBPRESS_OBJ_5E4 | 0x10 | 4 |
| LIBPRESS_OBJ_5F4 | 0x18 | 6 |
| LTDF2_OBJ_D4 | 0x8 | 3 |
| MEMMOVE_OBJ_64 | 0x8 | 3 |
| MSC02_OBJ_100 | unknown | 48 |
| MULSF3_OBJ_12C | 0x4 | 1 |
| MULSF3_OBJ_130 | 0x10 | 4 |
| PAD_OBJ_254 | 0x68 | 26 |
| PAD_OBJ_2BC | 0x40 | 17 |
| PATCHGTE_OBJ_DC | 0x4 | 1 |
| PATCH_OBJ_70 | 0x10 | 5 |
| RATAN_OBJ_13C | 0x14 | 5 |
| RATAN_OBJ_150 | 0x24 | 12 |
| RATAN_OBJ_B4 | 0x88 | 34 |
| S_CRWA_OBJ_170 | 0x24 | 12 |
| S_M_INIT_OBJ_4C | 0x8 | 5 |
| S_M_UTIL_OBJ_104 | 0x8 | 3 |
| S_M_UTIL_OBJ_2C | 0x4C | 19 |
| S_M_UTIL_OBJ_B8 | 0x4C | 19 |
| SPRINTF_OBJ_1C0 | 0x9C | 39 |
| SPRINTF_OBJ_25C | 0x44 | 17 |
| SPRINTF_OBJ_2A0 | 0x24 | 9 |
| SPRINTF_OBJ_2C4 | 0xC | 3 |
| SPRINTF_OBJ_2D0 | 0xC | 3 |
| SPRINTF_OBJ_2DC | 0xC | 3 |
| SPRINTF_OBJ_2E8 | 0x20 | 8 |
| SPRINTF_OBJ_308 | 0x58 | 22 |
| SPRINTF_OBJ_360 | 0x34 | 13 |
| SPRINTF_OBJ_394 | 0xEC | 59 |
| SPRINTF_OBJ_480 | 0x100 | 64 |
| SPRINTF_OBJ_580 | 0x14 | 5 |
| SPRINTF_OBJ_594 | 0xC | 3 |
| SPRINTF_OBJ_5A0 | 0x8 | 2 |
| SPRINTF_OBJ_5A8 | 0x100 | 64 |
| SPRINTF_OBJ_6A8 | 0x20 | 8 |
| SPRINTF_OBJ_754 | 0x34 | 13 |
| SPRINTF_OBJ_788 | 0x18 | 6 |
| SPRINTF_OBJ_82C | 0x58 | 25 |
| SPRINTF_OBJ_94 | 0x12C | 75 |
| SPU_OBJ_424 | 0x1C | 7 |
| SPU_OBJ_4EC | 0x10 | 4 |
| SPU_OBJ_78C | 0x88 | 34 |
| SPU_OBJ_814 | 0x10 | 4 |
| SPU_OBJ_894 | 0x14 | 5 |
| SPU_OBJ_A7C | 0xC | 3 |
| SPU_OBJ_A88 | 0x28 | 10 |
| SPU_OBJ_AB0 | 0x28 | 10 |
| SPU_OBJ_B18 | 0x1C | 10 |
| S_SIA_OBJ_30 | 0x10 | 4 |
| S_SI_OBJ_12C | 0x10 | 5 |
| S_SRMT_OBJ_1B4 | 0x18 | 7 |
| S_SRMT_OBJ_FC | 0x10 | 4 |
| S_SR_OBJ_B0 | 0x4 | 1 |
| S_SR_OBJ_B4 | 0x18 | 7 |
| S_STM_OBJ_1C | 0x14 | 5 |
| S_STSA_OBJ_4C | 0x10 | 5 |
| SUBSF3_OBJ_50 | 0x10 | 4 |
| SYS_OBJ_1244 | 0xC | 3 |
| SYS_OBJ_1400 | 0x9C | 39 |
| SYS_OBJ_15F8 | 0xF8 | 62 |
| SYS_OBJ_160 | 0x14 | 5 |
| SYS_OBJ_16F0 | 0x1C | 7 |
| SYS_OBJ_170C | 0x20 | 8 |
| SYS_OBJ_172C | 0x7C | 31 |
| SYS_OBJ_17A8 | 0x1C | 7 |
| SYS_OBJ_17C4 | 0x7C | 31 |
| SYS_OBJ_1840 | 0x1C | 7 |
| SYS_OBJ_185C | 0x1C | 7 |
| SYS_OBJ_1878 | 0x78 | 30 |
| SYS_OBJ_18F0 | 0x8 | 2 |
| SYS_OBJ_18F8 | 0x18 | 6 |
| SYS_OBJ_19F0 | 0x58 | 22 |
| SYS_OBJ_1A48 | 0x40 | 16 |
| SYS_OBJ_1C88 | 0x3C | 15 |
| SYS_OBJ_1E38 | 0x24 | 9 |
| SYS_OBJ_1EBC | 0x3C | 15 |
| SYS_OBJ_20BC | 0x20 | 8 |
| SYS_OBJ_20DC | 0x24 | 9 |
| SYS_OBJ_2100 | 0x14 | 5 |
| SYS_OBJ_2114 | 0x40 | 16 |
| SYS_OBJ_2154 | 0x48 | 18 |
| SYS_OBJ_219C | 0x30 | 12 |
| SYS_OBJ_233C | 0xB8 | 46 |
| SYS_OBJ_2484 | 0x1C | 7 |
| SYS_OBJ_297C | 0x10 | 4 |
| SYS_OBJ_2AF4 | 0x10 | 4 |
| SYS_OBJ_2B04 | 0x98 | 38 |
| SYS_OBJ_2EA8 | 0x18 | 6 |
| SYS_OBJ_2FE4 | 0x24 | 11 |
| SYS_OBJ_3E4 | 0xD0 | 52 |
| SYS_OBJ_4F0 | 0x10 | 4 |
| SYS_OBJ_538 | 0x30 | 12 |
| SYS_OBJ_788 | 0x18 | 6 |
| SYS_OBJ_98 | 0x10 | 4 |
| SYS_OBJ_F64 | 0x90 | 36 |
| SYS_OBJ_FF4 | 0x18 | 6 |
| TMD_OBJ_186C | 0x14 | 5 |
| TMD_OBJ_43C | 0x20 | 8 |
| TMD_OBJ_45C | 0x18 | 6 |
| TMD_OBJ_64 | 0x10 | 4 |
| TYPE_OBJ_108 | 0x14 | 5 |
| TYPE_OBJ_250 | 0x14 | 8 |
| VSYNC_OBJ_164 | 0x14 | 5 |
| VSYNC_OBJ_200 | 0x10 | 4 |
