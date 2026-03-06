# PsyQ 4.1 Build Chain Flags

## CCPSX Driver (`CCPSX.EXE` v3.05)

| Flag | Description |
|------|-------------|
| `-c` | Compile to object file (don't link) |
| `-S` | Compile to assembler source |
| `-E` | Preprocess only |
| `-v` | Print executed commands |
| `-o FILE` | Specify output file |
| `-l LIBRARY` | Link a library |
| `-L DIR` | Library search directory |
| `-I DIR` | Include file search directory |
| `-D NAME[=DEF]` | Define preprocessor macro |
| `-U NAME` | Undefine preprocessor macro |
| `-g` | Generate debug info |
| `-O0` / `-O1` / `-O2` / `-O3` | Optimization level (0=none, 3=max) |
| `-G SIZE` | GP-relative optimization threshold (0=none) |
| `-mgpopt` | Improve GP register optimization |
| `-ansi` | ANSI compliance checking |
| `-Wall` | Enable all warnings |
| `-w` | Disable all warnings |
| `-f...` | Compiler options (passed through to CC1PSX) |
| `-m...` | Machine options (passed through to CC1PSX) |
| `-W...` | Warning options (passed through to CC1PSX) |
| `-Wp,...` | Pass options to preprocessor |
| `-Wa,...` | Pass options to assembler |
| `-Wl,...` | Pass options to linker |
| `-X...` | Pass options to linker (alias for `-Wl,...`) |
| `-nostdlib` | Don't link standard libraries |
| `-save-temps` | Preserve intermediate files |

## CC1PSX Compiler Flags (`-f...`)

### Optimization / Codegen

| Flag | Notes |
|------|-------|
| `-fcaller-saves` | Use caller-saves convention |
| `-fcse-follow-jumps` | CSE follows jumps |
| `-fcse-skip-blocks` | CSE skips blocks |
| `-fexpensive-optimizations` | Enable expensive optimizations |
| `-fforce-addr` | Force address into register before use |
| `-fforce-mem` | Force memory operands into registers |
| `-finline-functions` | Integrate simple functions into callers |
| `-fkeep-inline-functions` | Keep inline function bodies |
| `-fomit-frame-pointer` | Don't keep frame pointer in register |
| `-fpeephole` | Enable peephole optimizations |
| `-fregmove` | Enable register move optimization |
| `-frerun-cse-after-loop` | Re-run CSE after loop optimization |
| `-fschedule-insns` | Instruction scheduling (pass 1) |
| `-fschedule-insns2` | Instruction scheduling (pass 2) |
| `-fstrength-reduce` | Loop strength reduction |
| `-fthread-jumps` | Thread jumps optimization |
| `-funroll-loops` | Unroll loops with determinable iteration count |
| `-funroll-all-loops` | Unroll all loops |
| `-fdelayed-branch` | Delayed branch scheduling |
| `-ffast-math` | Allow unsafe FP optimizations |
| `-ffunction-cse` | CSE function addresses |

### Data Layout / ABI

| Flag | Notes |
|------|-------|
| `-fpcc-struct-return` | Return structs via memory (PCC convention) |
| `-freg-struct-return` | Return small structs in registers |
| `-fpack-struct` | Pack all struct members |
| `-fshort-enums` | Use smallest type for enums |
| `-fshort-double` | Make `double` same size as `float` |
| `-fsigned-char` / `-funsigned-char` | Default char signedness |
| `-fsigned-bitfields` / `-funsigned-bitfields` | Default bitfield signedness |
| `-fcommon` | Use common sections for uninitialized globals |
| `-fvolatile` | Treat all memory refs through pointers as volatile |
| `-fvolatile-global` | Treat all global memory refs as volatile |
| `-fwritable-strings` | Put string literals in writable data |
| `-fshared-data` | Share data between processes |
| `-finhibit-size-directive` | Don't emit `.size` directives |
| `-fverbose-asm` | Emit verbose assembly comments |
| `-fgnu-linker` | Use GNU linker conventions |

### Language / Semantics

| Flag | Notes |
|------|-------|
| `-fasm` / `-fno-asm` | Allow `asm` keyword |
| `-fbuiltin` / `-fno-builtin` | Recognize built-in functions |
| `-fcond-mismatch` | Allow mismatched conditional types |
| `-fdollars-in-identifiers` | Allow `$` in identifiers |
| `-ffreestanding` / `-fhosted` | Freestanding vs hosted environment |
| `-fident` | Emit `.ident` directives |
| `-fsyntax-only` | Check syntax only, no output |
| `-ftraditional` / `-fno-traditional` | Traditional C mode |
| `-fkeep-static-consts` | Keep static const variables even if unused |
| `-fallow-single-precision` | Allow single-precision FP operations |
| `-fpretend-float` | Pretend FP available (for cross-compilation) |

### C++ Only

| Flag | Notes |
|------|-------|
| `-faccess-control` | Enforce access control |
| `-fall-virtual` | Make all member functions virtual |
| `-falt-external-templates` | Alternative external template model |
| `-fansi-overloading` | ANSI overloading rules |
| `-fcheck-new` | Check `new` return value |
| `-fconserve-space` | Conserve space in object files |
| `-fdefault-inline` | Inline member functions defined in class body |
| `-felide-constructors` | Elide copy constructors |
| `-fenum-int-equiv` | Allow enum-int implicit conversion |
| `-fexceptions` | Enable exception handling |
| `-fexternal-templates` | External template instantiation |
| `-ffor-scope` | Enforce C++ scoping rules for `for` |
| `-fgnu-keywords` | Recognize GNU C++ keywords |
| `-fhandle-signatures` | Enable signature handling |
| `-fhuge-objects` | Allow huge objects |
| `-fimplement-inlines` | Emit out-of-line copies of inlines |
| `-fimplicit-templates` | Implicit template instantiation |
| `-flabels-ok` | Allow labels at end of compound statements |
| `-fmemoize-lookups` / `-fsave-memoized` | Memoize name lookups |
| `-fnonnull-objects` | Assume references are non-null |
| `-foperator-names` | Recognize `and`, `or`, etc. as operators |
| `-frepo` | Enable template repository |
| `-frtti` | Enable RTTI |
| `-fstats` | Emit compiler statistics |
| `-fstrict-prototype` | Strict prototype checking |
| `-fthis-is-variable` | Allow assignment to `this` |
| `-fvtable-thunks` | Use thunks for vtable dispatch |
| `-fweak` | Use weak symbols |
| `-fxref` | Emit cross-reference info |
| `-ftemplate-depth-N` | Set template recursion depth |

All `-f` flags have `-fno-` negated forms.

## Warning Flags (`-W...`)

`-Wall`, `-Wbad-function-cast`, `-Wcast-qual`, `-Wchar-subscripts`, `-Wcomment`/`-Wcomments`, `-Wconversion`, `-Wextern-inline`, `-Wformat`, `-Wimplicit`, `-Wimport`, `-Wmain`, `-Wmissing-braces`, `-Wmissing-declarations`, `-Wmissing-prototypes`, `-Wnested-externs`, `-Wparentheses`, `-Wpointer-arith`, `-Wredundant-decls`, `-Wreturn-type`, `-Wsign-compare`, `-Wstrict-prototypes`, `-Wtraditional`, `-Wtrigraphs`, `-Wuninitialized` (requires `-O`), `-Wwrite-strings`

C++ only: `-Wctor-dtor-privacy`, `-Wnon-virtual-dtor`, `-Woverloaded-virtual`, `-Wpmf-conversions`, `-Wprotocol`, `-Wreorder`, `-Wselector`, `-Wsynth`, `-Wtemplate-debugging`

All have `-Wno-` negated forms.

## Default Options (no flags)

Enabled by default: `-fpeephole`, `-ffunction-cse`, `-fkeep-static-consts`, `-fpcc-struct-return`, `-fcommon`, `-fverbose-asm`, `-fgnu-linker`, `-mgas`, `-msoft-float`, `-mcpu=3000`

## Options Enabled at `-O2`

`-fthread-jumps`, `-fcse-follow-jumps`, `-fcse-skip-blocks`, `-fexpensive-optimizations`, `-fstrength-reduce`, `-fpeephole`, `-fforce-mem`, `-frerun-cse-after-loop`, `-fschedule-insns`, `-fschedule-insns2`, `-fcaller-saves`, `-fregmove`, `-fomit-frame-pointer`

## This Project's Usage

- Default: `-O2 -G0`
- `NO_G0_SRCS` (1C38.c): `-O2` only (no `-G0`)
