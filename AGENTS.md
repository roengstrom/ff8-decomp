# AGENTS.md

Read [CONTRIBUTING.md](CONTRIBUTING.md) before you write, change or review any C, header or
config file. It is the project's rulebook. If anything here disagrees with it, CONTRIBUTING.md
wins.

This is a matching decompilation of Final Fantasy VIII (PS1, USA, `SLUS_008.92`). The C has
to compile back to the original binaries byte for byte. A function counts as done only when
it matches 100% and `make verify` reports `Match` for every target. A near match is not a
result: the function stays `INCLUDE_ASM` until it matches.

## Commands

| Command | What it does |
|---------|--------------|
| `make setup` | First-time setup: venv, disc extraction, then `make full`. |
| `make verify` | Build every binary and compare its SHA1 against the original. |
| `make verify-<binary>` | Build and compare one overlay, e.g. `make verify-menumain`. |
| `make full` | `clean`, `split`, `build-assets`, `verify`. CI runs this. Run it before opening a PR. |
| `make split` | Delete `asm/` and re-run splat. Needed after any change under `config/`. |
| `make verify EFFECTS=all` | Also build all 343 battle effect overlays (slow). |
| `make expected` | Build the target objects for objdiff. Re-run after `make split`. |
| `make check` | Audit symbol declarations (`ARGS=<area>` narrows it). |
| `./permute.sh <func>` | Set up `permuter/<func>/` for one function (see below). |

The Makefile does not track header dependencies. After editing a header, delete the built
objects of every binary whose sources include it, or run `make full`. Otherwise `make verify`
can report a match from stale objects.

By default only the effect overlays listed in `EFFECTS` in the Makefile are built. When you
start on a new effect overlay, add it to that list. A change to anything every effect overlay
includes (battle or effect headers) needs `make verify EFFECTS=all`.

## Layout

- `config/ff8.yaml`: the splat config for every binary. Segment and section layout belongs
  here. If a layout problem seems to need a Makefile change, look for the fix here first.
- `config/symbols.main.txt`: names for the main executable. Every binary reads it.
- `config/symbols.<binary>.txt`: names for that binary's own addresses.
- `src/`, `include/`: the C. Every `.c` has a header of the same name (see CONTRIBUTING.md).
- `src/psxsdk/`: the Psy-Q SDK. We don't decompile it; its functions stay `INCLUDE_ASM`.
  Its prototypes live in `include/psxsdk/`, which should be faithful to the original Psy-Q SDK.
  (See https://github.com/sozud/psy-q-decomp for reference)
- Generated, never edit or commit: `asm/`, `build/`, `expected/`, `objdiff.json`, and the
  asset files `make build-assets` writes. Assets come from the user's own disc.
- `permuter/`: scratch space that `permute.sh` creates, one directory per function. Edit
  freely, never commit.

## Toolchain

The Makefile is the only source of truth for how each file is built. Most files use PsyQ 4.1
(gcc 2.7.2, `-O2 -G0`). The exceptions are listed in the Makefile: `PSYQ43_SRCS` (PsyQ 4.3,
gcc 2.8.0), `NO_G0_SRCS`, `G4_SRCS`, `O0_SRCS`, and `NO_EXPAND_DIV` for how the assembler
expands division in each effect overlay. `permute.sh` keeps copies of some of these lists that
can fall behind the Makefile, so check the Makefile and pass `--psyq 4.3` yourself when the
file is in `PSYQ43_SRCS`.

When a file isn't in any list yet, the epilogue usually tells the compiler apart: PsyQ 4.1
ends `lw ra / addiu sp / jr ra / nop`, PsyQ 4.3 ends `lw ra / nop / jr ra / addiu sp`.

## Overlays

Overlays are loaded into the same memory, so two overlays can have different functions at
the same address, and an unnamed function's `func_XXXXXXXX` name can exist in several
overlays. These are different functions. The "declare once" rule in CONTRIBUTING.md applies
per binary.

Some overlays are loaded while another overlay stays in memory, and they call into it. In
`config/ff8.yaml`, such an overlay's `symbol_addrs_path` lists `symbols.main.txt`, then the
symbols file of every overlay loaded under it, then its own:

| Overlay | Reads symbols from |
|---------|--------------------|
| menu screens (`menuabl`, `menucfg`, ...) | main, menumain, own |
| battle stages | main, battle, own |
| battle effects (`effect_NNN`) | main, battle, stage, own |
| movie | main, field, own |

A symbol that belongs to the overlay underneath is named in that overlay's symbols file,
never again in the overlay on top. When a binary from this table is split for the first time,
give it this list, and add its symbols file to the lists of the overlays that run on top of it.

## Naming

Do not make up names, and do not rename anything, based on what you infer from the code.
That covers functions, function parameters, globals, struct fields, flags and enum values.
Reading the asm, a similar name nearby, or how a value seems to be used is not enough. A
function's local variables are the exception: name them after what they hold in that
function. Parameters are not local variables here; they need verifying like everything else.

Before a name goes in, tell the person you're working with what you think the thing does but don't
suggest names to avoid nudging the user to accepting an AI name. They check it in the running game
with a debugger (for example PCSX-Redux's gdb server), and only then does the name go in.
Until it's verified, keep the placeholder: `func_XXXXXXXX`, `D_XXXXXXXX`, `unkXX` fields, placeholder flag names.

When a verified name does go in:

- Function names are PascalCase (`SetLevel`, `IsKnockedOut`), `static` ones too.
- Rename the entry for that address in the owning symbols file (entries are sorted by
  address) and in the C, then run `make split`. Never give one address a second name in
  another file.
- A plain find-and-replace also hits string literals, and struct field renames hit local
  variables with the same name. Rename by whole word and check every hit.
- Don't add entries to `config/reloc_addrs.txt`.

## Matching one function

1. `grep -rn "INCLUDE_ASM.*<func>" src/` finds the source file. The macro's first argument is
   the asm folder, and the target is `<folder>/<func>.s`.
2. `python3 tools/m2c/m2c.py -t mipsel-gcc-c <func>.s` gives a first draft. Treat it as a
   starting point, not as the answer.
3. `./permute.sh <func>` creates `permuter/<func>/` with `base.c`, `target.o` and `diff.sh`.
   Edit `base.c`, run `./permuter/<func>/diff.sh`, and repeat until it matches. Never run two
   `diff.sh` in the same directory at once; they share the compiled object.
4. Use the same symbol names as the target asm. A different name for the same address can
   change the generated code.
5. Move the matching code into `src/`, replacing the `INCLUDE_ASM`, with types and prototypes
   placed as CONTRIBUTING.md says. Then run `make verify-<binary>`.

Work on one function at a time. Don't fan functions out across parallel agents. Run
`make verify-<binary>` while you work and `make full` once at the end, not after every function.

Don't reach for register-allocation tricks (extra temporaries, embedded assignments) while a
function is still far from matching. They're only worth trying when it's nearly there and stuck.

Write simple for-loops counting upwards (`for (i = 0; i < n; i++)`), even when the asm counts
down. gcc often turns an ascending loop into a countdown, so a countdown in the asm doesn't mean
the source had one. Always try the ascending form first. Keep a descending loop only when the
order matters to what the loop does, or when no ascending form matches. In the second case, say
in a comment that the direction is needed for the match.

`./permute.sh --run <func>` starts the permuter, which can use every core for a long time.
Ask before running it.

## Review your change before calling it done

The diff should hold only what was asked for: no extra scripts, Makefile targets or docs
along the way.

Review your whole diff against CONTRIBUTING.md and fix whatever breaks a rule. Then run
`make full`. If a rule can't be kept without breaking the match, keep the match and say which
rule and why.

The rules agents break most often (details in CONTRIBUTING.md): no NONMATCHING, no build
hacks, no inline asm or register pinning, no raw offsets or addresses, no unnecessary casts,
one declaration per symbol, private functions `static`, named single-bit flags, 1998-style C,
and no comments that repeat the code or note status.

Also:

- No new compiler warnings (see below).
- Compare pointers against `NULL`, never `0`.
- A manual `setlen()`/`setcode()`, or a direct write to a primitive's length or code byte,
  probably stands for an SDK macro (`setSprt`, `setPolyFT4`, `setDrawTPage`, ... in
  `include/psxsdk/libgpu.h`). Use the macro when its stores match the binary.
- Making an existing function `static` lets gcc inline it, which can break the match. Convert
  them one at a time and verify each. Check every binary's asm for callers first, including
  ones that still call it by its old `func_XXXXXXXX` name.
- One struct type per global. When a function needs its own view of the same memory, name the
  view after the function (`func_XXXXXXXX_arg0`), not after what you think it holds.
- One flag name per meaning. When a bit means different things in different fields, give each
  meaning its own name.
- No comments that say where a value is read or written ("read by func_X", "set in the
  battle loop").
- No K&R-style function definitions.
- No handwritten `.s` files. A function stays `INCLUDE_ASM` until its C matches.
- Never put an `extern` inside a function body. It usually means the owner's header isn't
  included, so include it instead.
- No alignment padding: one space, never extra spaces to line up `=`, types, trailing
  comments or `#define` values.
- Write an endless loop as `while (1) { ... }`, not `for (;;)` or `do { ... } while (1);`, and a
  loop that only waits as `while (cond) {}`, not `do {} while (cond);`.

## Compiler warnings

A change must not add compiler warnings. The tree already emits hundreds, so count them
instead of reading the build output by eye. Each warning names its file and line
(`src/foo.c:123: warning: ...`). For each file you touched, count before and after the change:

```
touch src/foo.c && make verify 2>&1 | grep -c 'src/foo.c:[0-9]*: warning'
```

For a header change, touch the `.c` files that include it and grep for the header's path.

A warning here almost always means a wrong type:

- "makes pointer from integer", "makes integer from pointer" and "from incompatible pointer
  type": a local, struct field or prototype has the wrong type.
- "type mismatch with previous implicit declaration": a function was called before its
  prototype was in scope, and the prototype disagrees with what the call assumed.

Fix the type. Don't silence the warning with a cast.

The build runs without `-Wall`, so calling a function with no prototype in scope gives no
warning at all. The compiler then assumes it returns `int`, which can change the generated
code. Make sure every function you call has its prototype in scope through its owner's header.

## Comments

Never add documentation (Doxygen blocks, function or file docs, docs files) unless the person
you're working with explicitly asks for it. Don't make any of it up: write it only from their
strict guidance.

Prefer a name to a comment. When a comment is needed, write a plain sentence a newcomer can
follow, not shorthand. Write it the way a person would: no em dashes or arrows, no
"Note that", no "This function ...". Use `//` for inline and single-line comments and `/** */`
only for comments that span several lines. A struct field's offset goes in front of the field:
`/** 0x0D */ u8 count; // spells in stock`.

## Git

- One topic per PR, kept small.
- Commit messages: a short subject in the imperative ("Decompile the gamestate sprite
  drawers"), and at most a sentence or two of body. No match statuses etc.
- No AI attribution: no `Co-Authored-By` trailer naming an AI, no "Generated with" line in a
  PR description, and no mention of the tool in commits, PRs or code. The code is written by
  humans, for humans. AI tools are tools, not authors, and should be used as such in this
  project.
- The person who commits a change must understand the code in it. Make sure the person you're
  working with can explain what the change does and why before it is committed.
- Stage files by name, not with `git add -A`.
- Run `make full` before opening a PR.
