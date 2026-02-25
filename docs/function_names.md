# FF8 PS1 Known Function Names

Address-to-name mapping for game functions in SLUS_008.92 that can be derived
from publicly available FF8 reverse-engineering projects and PS1 hardware
documentation.

Our project uses Ghidra for binary analysis and PSY-Q SDK identification.
The names below come from cross-referencing our Ghidra analysis with
community reference projects that have documented FF8's game logic.

## Reference Projects

| Project | URL | What it provides |
|---------|-----|------------------|
| deling | https://github.com/myst6re/deling | JSM opcode names in `src/JsmOpcode.h` and `src/JsmOpcode.cpp` |
| hyne | https://github.com/myst6re/hyne | Save file structs (`SaveData.h`), MC header format, checksum algorithms |
| FF8UltimateEditor | https://github.com/HobbitDur/FF8UltimateEditor | Game data JSON enums in `FF8GameData/Resources/json/` |
| PS1 hardware docs | nocash PSX specs | SPU register map (0x1F801Dxx), GPU commands (GP0), GTE COP2 registers |
| PSY-Q SDK docs | Sony PSY-Q 4.x reference | SDK function prototypes (LIBGTE, LIBGPU, LIBSPU, LIBAPI, etc.) |

---

## JSM Field Script Opcode Handlers

**Primary source**: deling `src/JsmOpcode.h` — enum of 376 opcode names,
and `src/JsmOpcode.cpp` — `opcodes[]` string array mapping index to name.

**How to apply**: The PS1 binary has a 384-entry dispatch table in the data
section. In Ghidra, locate this table and read the function pointer at each
index. The deling opcode enum gives you the name for each index.

To find the dispatch table: search the data section for a large run of
consecutive .text-range pointers (384 entries). Each entry is a function
pointer for that opcode number.

The opcode names from deling (indices 0-375):

```
  0 NOP        1 CAL        2 JMP        3 JPF        4 GJMP
  5 LBL        6 RET        7 PSHN_L     8 PSHI_L     9 POPI_L
 10 PSHM_B    11 POPM_B    12 PSHM_W    13 POPM_W    14 PSHM_L
 15 POPM_L    16 PSHSM_B   17 PSHSM_W   18 PSHSM_L   19 PSHAC
 20 REQ       21 REQSW     22 REQEW     23 PREQ      24 PREQSW
 25 PREQEW    26 UNUSE     27 DEBUG     28 HALT      29 SET
 30 SET3      31 IDLOCK    32 IDUNLOCK  33 EFFECTPLAY2 34 FOOTSTEP
 35 JUMP      36 JUMP3     37 LADDERUP  38 LADDERDOWN 39 LADDERUP2
 40 LADDERDOWN2 41 MAPJUMP 42 MAPJUMP3  43 SETMODEL  44 BASEANIME
 45 ANIME     46 ANIMEKEEP 47 CANIME    48 CANIMEKEEP 49 RANIME
 50 RANIMEKEEP 51 RCANIME  52 RCANIMEKEEP 53 RANIMELOOP 54 RCANIMELOOP
 55 LADDERANIME 56 DISCJUMP 57 SETLINE  58 LINEON    59 LINEOFF
 60 WAIT      61 MSPEED    62 MOVE      63 MOVEA     64 PMOVEA
 65 CMOVE     66 FMOVE     67 PJUMPA    68 ANIMESYNC 69 ANIMESTOP
 70 MESW      71 MES       72 MESSYNC   73 MESVAR    74 ASK
 75 WINSIZE   76 WINCLOSE  77 UCON      78 UCOFF     79 MOVIE
 80 MOVIESYNC 81 SETPC     82 DIR       83 DIRP      84 DIRA
 85 PDIRA     86 SPUREADY  87 TALKON    88 TALKOFF   89 PUSHON
 90 PUSHOFF   91 ISTOUCH   92 MAPJUMPO  93 MAPJUMPON 94 MAPJUMPOFF
 95 SETMESSPEED 96 SHOW    97 HIDE      98 TALKRADIUS 99 PUSHRADIUS
100 AMESW    101 AMES     102 GETINFO  103 THROUGHON 104 THROUGHOFF
105 BATTLE   106 BATTLERESULT 107 BATTLEON 108 BATTLEOFF 109 KEYSCAN
110 KEYON    111 AASK     112 PGETINFO 113 DSCROLL  114 LSCROLL
115 CSCROLL  116 DSCROLLA 117 LSCROLLA 118 CSCROLLA 119 SCROLLSYNC
120 RMOVE    121 RMOVEA   122 RPMOVEA  123 RCMOVE   124 RFMOVE
125 MOVESYNC 126 CLEAR    127 DSCROLLP 128 LSCROLLP 129 CSCROLLP
130 LTURNR   131 LTURNL   132 CTURNR   133 CTURNL   134 ADDPARTY
135 SUBPARTY 136 CHANGEPARTY 137 REFRESHPARTY 138 SETPARTY 139 ISPARTY
140 ADDMEMBER 141 SUBMEMBER 142 ISMEMBER 143 LTURN   144 CTURN
145 PLTURN   146 PCTURN   147 JOIN     148 MESFORCUS 149 BGANIME
150 RBGANIME 151 RBGANIMELOOP 152 BGANIMESYNC 153 BGDRAW 154 BGOFF
155 BGANIMESPEED 156 SETTIMER 157 DISPTIMER 158 SHADETIMER 159 SETGETA
160 SETROOTTRANS 161 SETVIBRATE 162 STOPVIBRATE 163 MOVIEREADY
164 GETTIMER 165 FADEIN   166 FADEOUT  167 FADESYNC 168 SHAKE
169 SHAKEOFF 170 FADEBLACK 171 FOLLOWOFF 172 FOLLOWON 173 GAMEOVER
174 ENDING   175 SHADELEVEL 176 SHADEFORM 177 FMOVEA  178 FMOVEP
179 SHADESET 180 MUSICCHANGE 181 MUSICLOAD 182 FADENONE 183 POLYCOLOR
184 POLYCOLORALL 185 KILLTIMER 186 CROSSMUSIC 187 DUALMUSIC
188 EFFECTPLAY 189 EFFECTLOAD 190 LOADSYNC 191 MUSICSTOP
192 MUSICVOL 193 MUSICVOLTRANS 194 MUSICVOLFADE 195 ALLSEVOL
196 ALLSEVOLTRANS 197 ALLSEPOS 198 ALLSEPOSTRANS 199 SEVOL
200 SEVOLTRANS 201 SEPOS   202 SEPOSTRANS 203 SETBATTLEMUSIC
204 BATTLEMODE 205 SESTOP  206 BGANIMEFLAG 207 INITSOUND
208 BGSHADE  209 BGSHADESTOP 210 RBGSHADELOOP 211 DSCROLL2
212 LSCROLL2 213 CSCROLL2 214 DSCROLLA2 215 LSCROLLA2 216 CSCROLLA2
217 DSCROLLP2 218 LSCROLLP2 219 CSCROLLP2 220 SCROLLSYNC2
221 SCROLLMODE2 222 MENUENABLE 223 MENUDISABLE 224 FOOTSTEPON
225 FOOTSTEPOFF 226 FOOTSTEPOFFALL 227 FOOTSTEPCUT 228 PREMAPJUMP
229 USE      230 SPLIT    231 ANIMESPEED 232 RND     233 DCOLADD
234 DCOLSUB  235 TCOLADD  236 TCOLSUB  237 FCOLADD  238 FCOLSUB
239 COLSYNC  240 DOFFSET  241 LOFFSETS 242 COFFSETS 243 LOFFSET
244 COFFSET  245 OFFSETSYNC 246 RUNENABLE 247 RUNDISABLE
248 MAPFADEOFF 249 MAPFADEON 250 INITTRACE 251 SETDRESS
252 GETDRESS 253 FACEDIR  254 FACEDIRA 255 FACEDIRP
256 FACEDIRLIMIT 257 FACEDIROFF 258 SARALYOFF 259 SARALYON
260 SARALYDISPOFF 261 SARALYDISPON 262 MESMODE 263 FACEDIRINIT
264 FACEDIRI 265 JUNCTION 266 SETCAMERA 267 BATTLECUT
268 FOOTSTEPCOPY 269 WORLDMAPJUMP 270 RFACEDIRI 271 RFACEDIR
272 RFACEDIRA 273 RFACEDIRP 274 RFACEDIROFF 275 FACEDIRSYNC
276 COPYINFO 277 PCOPYINFO 278 RAMESW   279 BGSHADEOFF
280 AXIS     281 AXISSYNC 282 MENUNORMAL 283 MENUPHS
284 BGCLEAR  285 GETPARTY 286 MENUSHOP 287 DISC
288 DSCROLL3 289 LSCROLL3 290 CSCROLL3 291 MACCEL
292 MLIMIT   293 ADDITEM  294 SETWITCH 295 SETODIN
296 RESETGF  297 MENUNAME 298 REST     299 MOVECANCEL
300 PMOVECANCEL 301 ACTORMODE 302 MENUSAVE 303 SAVEENABLE
304 PHSENABLE 305 HOLD    306 MOVIECUT 307 SETPLACE
308 SETDCAMERA 309 CHOICEMUSIC 310 GETCARD 311 DRAWPOINT
312 PHSPOWER 313 KEY     314 CARDGAME 315 SETBAR
316 DISPBAR  317 KILLBAR 318 SCROLLRATIO2 319 WHOAMI
320 MUSICSTATUS 321 MUSICREPLAY 322 DOORLINEOFF 323 DOORLINEON
324 MUSICSKIP 325 DYING   326 SETHP    327 GETHP
328 MOVEFLUSH 329 MUSICVOLSYNC 330 PUSHANIME 331 POPANIME
332 KEYSCAN2 333 KEYON2  334 PARTICLEON 335 PARTICLEOFF
336 KEYSIGHNCHANGE 337 ADDGIL 338 ADDPASTGIL 339 ADDSEEDLEVEL
340 PARTICLESET 341 SETDRAWPOINT 342 MENUTIPS 343 LASTIN
344 LASTOUT  345 SEALEDOFF 346 MENUTUTO 347 OPENEYES
348 CLOSEEYES 349 BLINKEYES 350 SETCARD 351 HOWMANYCARD
352 WHERECARD 353 ADDMAGIC 354 SWAP    355 SETPARTY2
356 SPUSYNC  357 BROKEN  358 ANGELODISABLE 359 UNKNOWN2
360 UNKNOWN3 361 UNKNOWN4 362 HASITEM 363 UNKNOWN6
364 UNKNOWN7 365 UNKNOWN8 366 UNKNOWN9 367 UNKNOWN10
368 UNKNOWN11 369 UNKNOWN12 370 UNKNOWN13 371 UNKNOWN14
372 UNKNOWN15 373 UNKNOWN16 374 PREMAPJUMP2 375 TUTO
```

The PS1 binary has 384 entries (indices 0-383). Opcodes 376-383 are
PSX-specific extensions not in deling's enum.

---

## Memory Card Functions (from hyne)

**Primary source**: hyne save editor source code — documents PS1 memory card
header format, XOR checksum algorithm, and CRC16 algorithm.

hyne documents that:
- MC header is 128 bytes, byte 0x7F is XOR checksum of bytes 0x00-0x7E
- Status byte at +0x00: 0x51=occupied, 0xA0=formatted, 0xA1=deleted
- Magic bytes "MC" at start of card header
- CRC16-CCITT (polynomial 0x1021) is used for save data integrity

To find these functions in Ghidra: search for the polynomial constant 0x1021
(CRC16) or look for XOR accumulation loops over 127 bytes near the memory card
code region. Functions that write "MC" magic bytes or 0xA0/0xFF status bytes
are memory card block initializers.

| What to look for in Ghidra | Likely name | hyne reference |
|-----------------------------|-------------|----------------|
| Function using polynomial 0x1021, builds 256-entry table | CRC16_CCITT | hyne CRC16 checksum algorithm |
| Function XORing 127 consecutive bytes | mc_xor_checksum | hyne MC header checksum at +0x7F |
| Function writing "MC" (0x4D, 0x43) to buffer | mc_block_init_MC | hyne MC header magic bytes |
| Function writing 0xA0 status byte | mc_block_init_A0 | hyne MC slot status format |
| Function writing 0xFF fill | mc_block_init_FF | hyne MC formatted slot status |
| Function zeroing 128 bytes | memzero_128 | hyne MC header size = 128 bytes |

---

## Save Data Structs (from hyne)

**Primary source**: hyne `SaveData.h` — complete struct definitions.

These struct sizes help identify functions in Ghidra by matching array strides:

| Struct | Size | Array count | hyne field names |
|--------|------|-------------|------------------|
| GFORCES | 68 bytes | 16 | name[12], exp, exists, HPs, completedAbilities[16], APs[24], kills, KOs |
| PERSONNAGES | 152 bytes | 8 | currentHP, maxHP, exp, ID, weaponID, str/vit/mag/spr/spd/lck, magies[32], commands[3], abilities[4], gfs, junction fields, compatibility[16], kills, KOs |
| SHOP | 20 bytes | 20 | items[16], visited |
| CONFIG | 20 bytes | 1 | battleSpeed, volume, flags, buttonMap[12] |
| MISC1 | 32 bytes | 1 | party[4], unlocked_weapons, griever[12], gils, dream_gils |
| LIMITB | 16 bytes | 1 | quistis, zell, irvine, selphie, angel fields |
| ITEMS | 428 bytes | 1 | battle_order[32], items[198] |
| MISC2 | 144 bytes | 1 | game_time, countdown, victory_count, position data |
| MISC3 | 256 bytes | 1 | steps, payslip, kills[8], draw_points[64] |
| FIELD | 1024 bytes | 1 | gameMoment, TT rules, card queen, quest flags |
| WORLDMAP | 128 bytes | 1 | char_pos, ragnarok_pos, bgu_pos, vehicle flags |
| TTCARDS | 128 bytes | 1 | cards[77], card_locations[33], victory/defeat counts |
| CHOCOBO | 64 bytes | 1 | level, hp, weapon, items, boko_attack |

When you see a function in Ghidra accessing an array with stride 152, it's
likely operating on PERSONNAGES (character) data. Stride 68 = GF data.
Stride 60 in the battle code region may be SFX entries (not from hyne, but
the stride-matching technique applies).

---

## Game Data Enums (from FF8UltimateEditor)

**Primary source**: FF8UltimateEditor `FF8GameData/Resources/json/` directory.
These enum values are consistent across PS1 and PC versions.

### Character IDs

From `FF8GameData/Resources/json/` character data:

| ID | Name | Notes |
|----|------|-------|
| 0 | Squall | Playable |
| 1 | Zell | Playable |
| 2 | Irvine | Playable |
| 3 | Quistis | Playable |
| 4 | Rinoa | Playable |
| 5 | Selphie | Playable |
| 6 | Seifer | Temporary |
| 7 | Edea | Temporary |
| 8 | Laguna | Dream sequences |
| 9 | Kiros | Dream sequences |
| 10 | Ward | Dream sequences |

### GF IDs

| ID | Name |
|----|------|
| 0 | Quezacotl |
| 1 | Shiva |
| 2 | Ifrit |
| 3 | Siren |
| 4 | Brothers |
| 5 | Diablos |
| 6 | Carbuncle |
| 7 | Leviathan |
| 8 | Pandemona |
| 9 | Cerberus |
| 10 | Alexander |
| 11 | Doomtrain |
| 12 | Bahamut |
| 13 | Cactuar |
| 14 | Tonberry King |
| 15 | Eden |

### Magic IDs (from `magic.json`)

| ID | Name | ID | Name | ID | Name |
|----|------|----|------|----|------|
| 0 | (None) | 19 | Ultima | 38 | Blind |
| 1 | Fire | 20 | Apocalypse | 39 | Confuse |
| 2 | Fira | 21 | Cure | 40 | Sleep |
| 3 | Firaga | 22 | Cura | 41 | Silence |
| 4 | Blizzard | 23 | Curaga | 42 | Break |
| 5 | Blizzara | 24 | Life | 43 | Death |
| 6 | Blizzaga | 25 | Full-life | 44 | Drain |
| 7 | Thunder | 26 | Regen | 45 | Pain |
| 8 | Thundara | 27 | Esuna | 46 | Berserk |
| 9 | Thundaga | 28 | Dispel | 47 | Float |
| 10 | Water | 29 | Protect | 48 | Zombie |
| 11 | Aero | 30 | Shell | 49 | Meltdown |
| 12 | Bio | 31 | Reflect | 50 | Scan |
| 13 | Demi | 32 | Aura | 51 | Full-cure |
| 14 | Holy | 33 | Double | 52 | Wall |
| 15 | Flare | 34 | Triple | 53 | Rapture |
| 16 | Meteor | 35 | Haste | 54 | Percent |
| 17 | Quake | 36 | Slow | 55 | Catastrophe |
| 18 | Tornado | 37 | Stop | 56 | The End |

### Status Bitfield (from `status.json`)

| Bit | Status |
|-----|--------|
| 0 | Death |
| 1 | Poison |
| 2 | Petrify |
| 3 | Blind |
| 4 | Silence |
| 5 | Berserk |
| 6 | Zombie |
| 7 | Sleep |
| 8 | Haste |
| 9 | Slow |
| 10 | Stop |
| 11 | Regen |
| 12 | Reflect |
| 13 | Doom |
| 14 | Petrifying |
| 15 | Float |
| 16 | Drain |
| 17 | Confusion |
| 18 | Ejection |

### Element Bitfield

| Bit | Element |
|-----|---------|
| 0 | Fire |
| 1 | Ice |
| 2 | Thunder |
| 3 | Earth |
| 4 | Poison |
| 5 | Wind |
| 6 | Water |
| 7 | Holy |

---

## SPU Functions (from PS1 hardware documentation)

**Primary source**: PS1 SPU register map (nocash PSX specs).

Functions that write to addresses in the 0x1F801Dxx range are SPU hardware
access functions. Match the register address to name the function:

| HW Register | PS1 SPU Function | What Ghidra will show |
|-------------|------------------|----------------------|
| 0x1F801D88 | Reverb work area start | `lui rX, 0x1F80; sw rY, 0x1D88(rX)` |
| 0x1F801D8C | IRQ address | `lui rX, 0x1F80; sw rY, 0x1D8C(rX)` |
| 0x1F801D90 | Transfer address | `lui rX, 0x1F80; sw rY, 0x1D90(rX)` |
| 0x1F801D94 | Data FIFO | `lui rX, 0x1F80; sw rY, 0x1D94(rX)` |
| 0x1F801D98 | Transfer control | `lui rX, 0x1F80; sw rY, 0x1D98(rX)` |
| 0x1F801DB0 | CD audio volume L/R | `lui rX, 0x1F80; sh rY, 0x1DB0(rX)` |

SPU voice registers are at 0x1F801C00 + voice*0x10 + offset:

| Voice offset | Field |
|-------------|-------|
| +0x04 | Pitch |
| +0x06 | Start address (>>3) |
| +0x08 | ADSR low |
| +0x0A | ADSR high |
| +0x0E | Repeat address (>>3) |

Functions that compute `base + voice_index * 0x10 + offset` and write to
0x1F801Cxx are SPU voice register setters.

---

## GTE Wrapper Functions (from PS1 hardware documentation)

**Primary source**: PS1 GTE (COP2) register documentation.

Functions containing `mtc2`/`mfc2`/`ctc2`/`cfc2` instructions are GTE
register access wrappers. Match the register number to name them:

| COP2 Reg | Name | Type |
|----------|------|------|
| D0/D1 | V0 (vector 0 XY/Z) | Data |
| D2/D3 | V1 (vector 1 XY/Z) | Data |
| D4/D5 | V2 (vector 2 XY/Z) | Data |
| D8 | IR0 (intermediate result 0) | Data |
| D9/D10/D11 | IR1/IR2/IR3 | Data |
| D12/D13/D14 | SXY0/SXY1/SXY2 (screen XY FIFO) | Data |
| D16-D19 | SZ0-SZ3 (screen Z FIFO) | Data |
| D20/D21/D22 | RGB0/RGB1/RGB2 (color FIFO) | Data |
| D25/D26/D27 | MAC1/MAC2/MAC3 | Data |
| D30 | LZCS (leading zero count source) | Data |
| C0/C2/C4 | R11R12/R21R22/R31R32 (rotation matrix) | Control |
| C5/C6/C7 | R13R21/R22R23/R31R32 (rotation matrix) | Control |
| C13/C14/C15 | TRX/TRY/TRZ (translation vector) | Control |
| C21/C22/C23 | RBK/GBK/BBK (background color) | Control |
| C24/C25 | RFC/GFC (far color R/G) | Control |
| C27 | ZSF3 (Z scale factor 3) | Control |
| C28 | ZSF4 (Z scale factor 4) | Control |

---

## GPU Functions (from PS1 hardware documentation)

**Primary source**: PS1 GPU command reference.

Functions that build GP0 command words can be identified by the command byte:

| GP0 Command | Function |
|-------------|----------|
| 0xE1 | Draw mode / texture page |
| 0xE2 | Texture window |
| 0xE3/0xE4 | Draw area top-left / bottom-right |
| 0xE5 | Draw offset |
| 0xE6 | Mask bit setting |

Functions that poll GPU status register bits are GPU busy/ready checks.

---

## PSY-Q SDK Functions (from Ghidra analysis)

**Primary source**: Our own Ghidra analysis of SLUS_008.92, cross-referenced
with PSY-Q SDK documentation.

The SDK functions are already identified in our `symbol_addrs.txt` via Ghidra.
They are pre-compiled library code and should NOT be decompiled.

Game code boundaries (from Ghidra analysis):
- **0x80011E00 - ~0x8003A2B4**: Main game code (before SDK libraries)
- **~0x80043AE4 - ~0x80047894**: Game code (between SDK library regions)

---

## How to Apply This Information

When decompiling a function in Ghidra:

1. **Check the address range** — if it's in SDK territory, it's probably a
   known SDK function already in symbol_addrs.txt.

2. **Check dispatch tables** — if the function is referenced from a jump table,
   check if it's a JSM opcode handler (use deling names) or battle AI handler.

3. **Check hardware register access** — if the function writes to 0x1F801Dxx,
   it's an SPU function. If it uses COP2, it's a GTE wrapper. Name accordingly.

4. **Check struct access patterns** — if the function indexes an array with
   stride 152, it's operating on character (PERSONNAGES) data. Stride 68 = GF
   data. Use hyne field names for the struct offsets.

5. **Check for known constants** — polynomial 0x1021 = CRC16, "MC" magic =
   memory card init, character IDs 0-10 match FF8UE enum, spell IDs match
   magic.json.

When you identify a function, document it in `symbol_addrs.txt` as a comment:
```
func_XXXXXXXX = 0xXXXXXXXX; // descriptive_name (source: project_name)
```
