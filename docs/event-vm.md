# Field Event VM

FF8's field overlay (`field.bin`) runs a table-driven bytecode VM that
drives map events, NPC interactions, cutscenes, dialog messages, and battle
triggers. The world overlay (`world.bin`) does **not** share this VM — it
dispatches via direct `jal` instructions without a script table.

This doc covers the field VM. Each section cites concrete file:line so
the source-of-truth lives in the assembly/C, not here.

## Instruction Format

One instruction is a 32-bit little-endian word:

```
 31      24 23                            0
+----------+-------------------------------+
|  opcode  |          arg (signed)         |
+----------+-------------------------------+
   1 byte             3 bytes
```

`decodeScriptOpcode` (`asm/nonmatchings/gamestate/decodeScriptOpcode.s`) splits a
word into `(opcode, arg)`. The arg is sign-extended from 24 bits to 32.

The script PC is a `u16` instruction index (not a byte offset) stored at
`Eline+0x176`; the dispatch loop scales it by 4 when computing the
fetch address.

## Eline (script context)

Defined in `include/field.h` as an alias view of `FieldEntity` — the per-
entity record has Eline-shaped field names so script handlers don't need
to know they're walking a `FieldEntity`.

Key fields:

| Offset  | Field          | Purpose                                |
|---------|----------------|----------------------------------------|
| 0x174   | `scriptGroup`  | Active-mask bit index for this entity  |
| 0x176   | `pc`           | Script PC (u16, instruction units)     |
| 0x184   | `stackPtr`     | Bytecode stack top index (s8)          |
| 0x188   | `unk188`       | Script parameter byte                  |
| 0x189   | `unk189`       | Script parameter byte                  |

The bytecode stack reuses the Eline struct as `s32[]`: `POP(eline)` is
`((s32 *)eline)[eline->stackPtr--]`. A handler popping N values produces
`addiu reg, stackPtr, -1, -2, …, -N` access patterns in disassembly.

## Dispatch

`func_800BD9C4` (in `asm/field/nonmatchings/fe_object10/`) is the main
per-frame VM tick. It contains **four** dispatch loops walking four
entity arrays at different strides:

| Array          | Stride  | Purpose                          |
|----------------|---------|----------------------------------|
| `D_80085224`   | 0x264   | Field actors (FieldEntity)       |
| `D_8008538C`   | 0x18C   | (secondary)                      |
| `D_80085384`   | 0x1A0   | (secondary)                      |
| (fourth)       | varies  | (secondary)                      |

`func_800BEBD0` is a **fifth** dispatch loop for camera/scene entities,
walking `D_800852F4` (stride 0x1B4) and `D_8008538C` (stride 0x18C).

Each iteration:

```
lhu  pc,  0x176(eline)
sll  pc,  pc, 2
lw   base, D_80085380(scriptId)
addu fetch, base, pc
jal  decodeScriptOpcode       ; split → (opcode, arg)
lw   opcode, 0x50(sp)
lw   arg,    0x54(sp)
sll  opcode, opcode, 2
addu slot, opcode, &D_800C67A8
lw   handler, 0(slot)
jalr handler                  ; handler(eline, arg)
 addu a0, eline, $zero
```

The return value drives control flow (bitmasks):

| Bit | Returned by         | Behavior                                |
|-----|---------------------|-----------------------------------------|
| 0   | `return 1`          | Yield: keep PC, decrement slot counter  |
| 1   | `return 2`          | Advance PC, mark active, continue       |
| 2   | `return 4`          | Re-enter handler next frame, keep PC    |

Most handlers return 2 (advance) or 1 (wait/yield). A handful return 3
(yield + skip-to-next-entity) or 0 (NOP).

## The Opcode Tables

Two tables, both in the field overlay's `.data` segment (file
`assets/2B898.bin`):

| Table          | Address      | File offset | Entries | Indexed by         |
|----------------|--------------|-------------|---------|--------------------|
| Main           | `0x800C67A8` | `0x2F10`    | 374     | High byte of insn  |
| Extended (ALU) | `0x800C6760` | `0x2EC8`    | 18      | `arg` of opcode 1  |

The extended table sits immediately before the main table in memory.
Opcode `0x01` dispatches to `func_800AE048` (`src/field/fe_object4.c:251`):

```c
s32 func_800AE048(u8 *eline, s32 index) {
    D_800C6760[index](eline);
    return 2;
}
```

The 18 extended entries are stack ALU ops: add, sub, neg, mul, div, mod,
eq, gt, ge, le, lt, ne, and, or, xor, not, shift-left, shift-right.

## The "Opcode 6 / arg < 9" Fast-Skip

In the camera dispatcher (`func_800BEBD0`), if a decoded instruction has
`opcode == 0x06 && arg < 9` the loop **skips the handler call entirely**
and advances to the next entity. This appears in both sub-loops of that
function:

- `asm/field/nonmatchings/fe_object10/func_800BEBD0.s:34-40`
- `asm/field/nonmatchings/fe_object10/func_800BEBD0.s:110-116`

Opcode 0x06 is a sleep/wait primitive. The threshold-9 short-circuit
means scripts can request short waits (< 9 frames) without consuming a
camera-entity tick slot — only longer waits go through the real handler.

This is the only meaningful "9" in the dispatch path. It is *not* an
opcode-base offset.

## Opcode Map (selected)

Full table is 374 entries; only decomped or otherwise notable handlers
are listed here. Status: **C** = decomped, **ASM** = `INCLUDE_ASM`. To
list every entry, dump `assets/2B898.bin[0x2F10..0x34E8]` as 4-byte
little-endian words.

Current C coverage: **174 / 374 = 46.5 %**.

### Control flow (0x00 – 0x06)

| Op   | Handler             | Status | Purpose                                |
|------|---------------------|--------|----------------------------------------|
| 0x00 | `func_800ADC9C`     | C      | NOP (return 0)                         |
| 0x01 | `func_800AE048`     | C      | Extended dispatch — `g_extTable[arg]`  |
| 0x02 | `func_800AE080`     | C      | `PC += arg` (unconditional branch)     |
| 0x03 | `func_800AE098`     | C      | `if (TOS == 0) PC += arg`              |
| 0x04 | `func_800AE0DC`     | C      | Push `PC+1`, `PC = arg` (CALL)         |
| 0x05 | `func_800AE124`     | ASM    | (RET, untested)                        |
| 0x06 | `func_800AE1AC`     | ASM    | Sleep/wait — see fast-skip above       |

### Loads, stores, immediates (0x07 – 0x19)

| Op   | Handler             | Status | Purpose                                |
|------|---------------------|--------|----------------------------------------|
| 0x07 | `func_800AE4C4`     | C      | Push byte-load `eline[arg]`            |
| 0x08 | `func_800AE518`     | C      | Push from result slot `eline[arg*4+0x140]` |
| 0x09 | `func_800AE7B4`     | C      | Pop TOS to result slot                 |
| 0x0A | `func_800AE574`     | C      | Push `g_gameState[arg + 0xD60]` byte   |
| 0x0B–0x12 | …              | C      | Various LOAD/STORE on script vars      |
| 0x13 | `func_800AE88C`     | C      | Push immediate `arg` (PUSH_IMM)        |
| 0x14–0x19 | `func_800AE978`+ | ASM   | Math / compare branches                |

### Flag / sentinel ops

| Op   | Handler             | Status | Purpose                                |
|------|---------------------|--------|----------------------------------------|
| 0x1A | `func_800AF5F8`     | C      | `eline->flags &= ~0x2`                 |
| 0x1B | `func_800AEEC4`     | C      | Return 3 (skip-to-next-entity)         |
| 0x1C | `func_800AEECC`     | C      | Return 1 (yield)                       |

### Messages (0x3D – 0x42)

| Op   | Handler             | Status | Purpose                                |
|------|---------------------|--------|----------------------------------------|
| 0x3D | `func_800B68B8`     | C      | Set message channel                    |
| 0x3E | `opHandler_MES`     | C      | **MES — show dialog message**          |
| 0x3F | `func_800B69E8`     | C      | Show message anchored to map entity    |
| 0x40 | `func_800B6B20`     | C      | Show message tracking party member     |
| 0x41 | `func_800B6C28`     | C      | ASK — yes/no prompt                    |
| 0x42 | `func_800B6D24`     | C      | Positioned message (no prompt)         |

> **Note:** Earlier commit messages and source comments referred to MES
> as opcode 0x47 (commit `156e894`) or 0x50 (source comment). Both are
> wrong. Table-index arithmetic gives `(0x800B68EC - 0x800C67A8) / 4 =
> 0x3E`, and the neighbouring 0x3D/0x3F/0x40 handlers are all message
> handlers — a coherent run that corroborates it.

### Misc

| Op    | Handler             | Status | Purpose                                |
|-------|---------------------|--------|----------------------------------------|
| 0x123 | `func_800B7718`     | C      | Scrolling positioned message           |
| 0x137 | `func_800B5A30`     | C      | Draw-point interaction (8-state FSM)   |
| 0x13A | `func_800B5480`     | C      | **Battle encounter trigger**           |
| 0x14E | `func_800B62E8`     | C      | Activate system slot                   |
| 0x14F | `func_800B6328`     | C      | Deactivate system slot                 |
| 0x175 | `func_800B629C`     | C      | (last entry; pop-decrement-store)      |

## Files of Interest

| File                                                              | Purpose                                                       |
|-------------------------------------------------------------------|---------------------------------------------------------------|
| `assets/2B898.bin`                                                | Field `.data`; opcode tables at offsets 0x2EC8 and 0x2F10     |
| `include/field.h`                                                 | `Eline`, `FieldEntity`, `POP`/`POP_BYTE`/`PEEK` macros        |
| `src/field/fe_object4.c`                                          | Opcodes 0x00..~0x20 (control flow + loads/stores + immediate) |
| `src/field/fe_object5.c`, `fe_object6.c`, `fe_object8.c`          | Most other game-state opcodes                                 |
| `src/field/fe_object7.c:855` (`opHandler_MES`)                    | Canonical message-opcode handler (opcode 0x3E)                |
| `asm/field/nonmatchings/fe_object10/func_800BD9C4.s`              | Main per-frame VM tick (4 dispatch loops)                     |
| `asm/field/nonmatchings/fe_object10/func_800BEBD0.s`              | Camera dispatch loop (5th); contains the opcode-6/arg<9 skip  |
| `asm/nonmatchings/gamestate/decodeScriptOpcode.s`                 | Opcode/arg word splitter                                      |
| `config/symbols.field.txt`                                   | Symbol map — `opHandler_MES = 0x800B68EC` lives here          |

## World Overlay (no VM)

The world overlay does **not** have a table-based VM. Verified by:

- `decodeScriptOpcode` (the opcode unpacker) has zero callers under `asm/ovl/world/`.
- `D_800C67A8` / `D_800C6760` are referenced only from field-overlay code.
- The only `jalr` in the world overlay is an animation callback in
  `asm/ovl/world/nonmatchings/we_object1/func_8009CAE0.s:43`, not a dispatcher.

The world's main per-frame update lives at `0x800987D8` (currently
mis-classified as `.rodata` data in
`asm/ovl/world/data/we_dispatch2.rodata.s:30-1065` — should be re-split
as `.text`). It dispatches subsystems via direct `jal`s and inline
switches.
