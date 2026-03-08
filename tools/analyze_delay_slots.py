#!/usr/bin/env python3
"""Analyze .s files to classify epilogue delay slots for PsyQ toolchain detection.

The key diagnostic is the lw $ra LOAD delay slot, not the jr $ra branch delay:
  - PsyQ 4.3 (gcc 2.8.0): lw $ra / nop / jr $ra / addiu $sp
    (doesn't fill load delay, fills branch delay natively)
  - PsyQ 4.1 (gcc 2.7.2): lw $ra / addiu $sp / jr $ra / nop
    (maspsx moves addiu into load delay, branch delay left as nop)
  - PsyQ 4.1 with -O2:    lw $ra / <useful insn> / jr $ra / addiu $sp
    (scheduler fills load delay with computation, addiu goes to branch delay)

Only non-leaf returns (where sw $ra appears earlier) are diagnostic — leaf
functions produce identical code with both toolchains.

Usage:
  python3 tools/analyze_delay_slots.py [asm_dir]   # text list (default)
  python3 tools/analyze_delay_slots.py --map        # terminal color visualization
"""

import argparse
import math
import os
import re
import sys
from pathlib import Path

DEFAULT_ASM_DIR = Path("asm/nonmatchings/1C38")

# Address range of the binary (used for --map mode only)
ADDR_START = 0x80011438
ADDR_END = 0x80051800  # approximate end


def parse_instructions(filepath):
    """Parse .s file into list of (addr, mnemonic, operands)."""
    insns = []
    with open(filepath) as f:
        for line in f:
            m = re.match(
                r"\s*/\*\s+[0-9A-Fa-f]+\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+\*/\s+"
                r"(\S+)\s*(.*)",
                line,
            )
            if m:
                addr = int(m.group(1), 16)
                mnemonic = m.group(2)
                operands = m.group(3).strip()
                insns.append((addr, mnemonic, operands))
    return insns


def parse_size(filepath):
    """Parse function size from the nonmatching directive."""
    with open(filepath) as f:
        first_line = f.readline()
    m = re.search(r"0x([0-9A-Fa-f]+)", first_line)
    return int(m.group(1), 16) if m else 0


def has_trailing_nops(filepath):
    """Check if there are nop instructions after endlabel."""
    found_endlabel = False
    with open(filepath) as f:
        for line in f:
            if "endlabel" in line:
                found_endlabel = True
                continue
            if found_endlabel:
                if re.match(
                    r"\s*/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+00000000\s+\*/\s+nop",
                    line,
                ):
                    return True
    return False


def is_library(name):
    """Check if a function name looks like SDK/library code."""
    if "_OBJ_" in name:
        return True
    if name.startswith("func_") or name in ("main", "start"):
        return False
    # Named functions that aren't func_ are likely SDK
    return True


def _is_sreg_load(mnem, ops):
    """Check if instruction is lw $sN, offset($sp) — a callee-saved reg restore."""
    if mnem != "lw":
        return False
    # Match $s0-$s7 ($16-$23) or $fp ($30)
    m = re.match(r"\$(?:s[0-7]|fp|(?:1[6-9]|2[0-3]|30))\b", ops.strip())
    return m is not None


def classify_returns(insns):
    """Classify each jr $ra in the instruction list.

    For merged blocks (multiple functions under one splat label), splits at
    jr $ra boundaries so each sub-function's leaf/non-leaf status is determined
    only from instructions within its own range. This prevents a non-leaf
    function's sw $ra from leaking into the next merged function and making
    a leaf return look non-leaf.

    The key diagnostic is the instruction after lw $ra (the load delay slot):
      - nop       → PsyQ 4.3 (gcc 2.8.0 doesn't fill load delay)
      - addiu $sp → PsyQ 4.1 (maspsx moved addiu into load delay)
      - lw $sN    → indeterminate (both toolchains produce identical epilogues)
      - other     → PsyQ 4.1 (-O2 scheduler filled load delay with computation)

    Returns a list of (addr, slot_type, is_nonleaf) tuples where:
      slot_type: "FILLED", "UNFILLED", or "OTHER"
      is_nonleaf: True if sw $ra appears within this sub-function's range
    """
    # Find all jr $ra positions
    jr_positions = []
    for i, (addr, mnem, ops) in enumerate(insns):
        if mnem == "jr" and "$ra" in ops and i + 1 < len(insns):
            jr_positions.append(i)

    if not jr_positions:
        return []

    returns = []
    subfunc_start = 0

    for jr_idx in jr_positions:
        delay_idx = jr_idx + 1
        delay_mnem = insns[delay_idx][1]
        delay_ops = insns[delay_idx][2]

        # Check for sw $ra only within this sub-function's instruction range
        is_nonleaf = any(
            insns[j][1] == "sw" and "$ra" in insns[j][2]
            for j in range(subfunc_start, jr_idx)
        )

        if delay_mnem == "nop":
            # Branch delay is nop — classic PsyQ 4.1 (UNFILLED)
            slot_type = "UNFILLED"
        elif delay_mnem == "addiu" and "$sp" in delay_ops:
            # Branch delay is addiu $sp. Check the load delay slot after
            # lw $ra to distinguish toolchains:
            #   4.3 definite:  lw $ra / nop    / jr $ra / addiu $sp
            #   4.1 definite:  lw $ra / addiu $sp / jr $ra / nop (handled above)
            #   4.1 with -O2:  lw $ra / <computation> / jr $ra / addiu $sp
            #   indeterminate: lw $ra / lw $sN / ... / jr $ra / addiu $sp
            slot_type = "UNFILLED"  # default: assume 4.1
            # Search backwards from jr $ra to find the corresponding lw $ra
            for k in range(jr_idx - 1, max(subfunc_start - 1, -1), -1):
                if insns[k][1] == "lw" and "$ra" in insns[k][2]:
                    if k + 1 < len(insns):
                        next_mnem = insns[k + 1][1]
                        next_ops = insns[k + 1][2]
                        if next_mnem == "nop":
                            # nop in load delay → definitely PsyQ 4.3
                            slot_type = "FILLED"
                        elif _is_sreg_load(next_mnem, next_ops):
                            # lw $sN fills load delay — both toolchains
                            # produce identical epilogues, can't distinguish
                            slot_type = "AMBIGUOUS"
                        # else: computation in load delay → PsyQ 4.1 (UNFILLED)
                    break
        else:
            slot_type = "OTHER"

        returns.append((insns[jr_idx][0], slot_type, is_nonleaf))

        # Next sub-function starts after the delay slot
        subfunc_start = delay_idx + 1

    return returns


def classify_function(filepath):
    """Classify a function's toolchain from its epilogue delay slots.

    Returns dict with keys:
      name, addr, size, classification, detail, is_lib
    Classification is one of:
      FILLED    - non-leaf with nop in lw $ra load delay (PsyQ 4.3)
      UNFILLED  - non-leaf with filled lw $ra load delay (PsyQ 4.1)
      AMBIGUOUS - non-leaf but lw $sN fills load delay (either toolchain)
      MIXED     - both filled and unfilled non-leaf returns
      LEAF      - no non-leaf returns, toolchain indeterminate
      NO_JR_RA  - no jr $ra found
    """
    name = filepath.stem
    insns = parse_instructions(filepath)
    if not insns:
        return None

    addr = insns[0][0]
    size = parse_size(filepath)
    trailing = has_trailing_nops(filepath)
    lib = is_library(name)

    returns = classify_returns(insns)
    if not returns:
        return dict(
            name=name, addr=addr, size=size, classification="NO_JR_RA",
            detail="", is_lib=lib, trailing=trailing,
        )

    nonleaf_returns = [(a, s) for a, s, nl in returns if nl]
    leaf_returns = [(a, s) for a, s, nl in returns if not nl]

    filled = sum(1 for _, s in nonleaf_returns if s == "FILLED")
    unfilled = sum(1 for _, s in nonleaf_returns if s == "UNFILLED")
    ambiguous = sum(1 for _, s in nonleaf_returns if s == "AMBIGUOUS")
    other = sum(1 for _, s in nonleaf_returns if s == "OTHER")

    parts = []
    if len(returns) > 1:
        parts.append(f"{len(returns)}x_jr_ra")
    if filled + unfilled + ambiguous > 1 and (filled > 0 or ambiguous > 0):
        parts.append(f"F={filled}")
        parts.append(f"U={unfilled}")
        parts.append(f"A={ambiguous}")
        parts.append(f"L={len(leaf_returns)}")
    if trailing:
        parts.append("+trailing_nops")
    detail = " " + " ".join(parts) if parts else ""

    diagnostic = filled + unfilled  # only definite classifications
    if diagnostic == 0 and ambiguous == 0:
        classification = "LEAF"
    elif diagnostic == 0 and ambiguous > 0:
        # All non-leaf returns have s-reg restores — can't distinguish
        classification = "AMBIGUOUS"
    elif filled > 0 and unfilled > 0:
        classification = "MIXED"
    elif filled > 0:
        classification = "FILLED"
    else:
        classification = "UNFILLED"

    return dict(
        name=name, addr=addr, size=size, classification=classification,
        detail=detail, is_lib=lib, trailing=trailing,
    )


def load_all(asm_dir):
    """Load and classify all .s files. Returns sorted list of function dicts."""
    results = []
    for f in asm_dir.rglob("*.s"):
        result = classify_function(f)
        if result:
            results.append(result)
    results.sort(key=lambda r: r["addr"])
    return results


def cmd_list(results):
    """Print text list output (original behavior)."""
    counts = {}
    for r in results:
        cls = r["classification"]
        counts[cls] = counts.get(cls, 0) + 1
        print(f"0x{r['addr']:08X}  {cls:<10s} {r['name']}{r['detail']}")

    print()
    print("--- Summary ---")
    for cls in sorted(counts.keys()):
        print(f"  {cls:<15s} {counts[cls]:>4d}")
    print(f"  {'TOTAL':<15s} {sum(counts.values()):>4d}")


def cmd_map(results, width=160):
    """Print a colored terminal visualization of the address space."""

    # Build a byte-level map of the address space
    # Each entry covers a range of addresses; we'll bucket into columns
    total_range = ADDR_END - ADDR_START
    # Each row represents a chunk of address space
    # Pick row size so we get a reasonable number of rows
    bytes_per_cell = 64  # each character = 64 bytes of address space
    cols = width - 20  # leave room for address labels
    bytes_per_row = cols * bytes_per_cell
    num_rows = math.ceil(total_range / bytes_per_row)

    # Color codes (256-color)
    # Game:    bold/bright backgrounds
    # Library: dim/muted backgrounds
    RESET = "\033[0m"

    # Game code - bright backgrounds (use bg color only, render with spaces)
    GAME_FILLED = "\033[48;5;27m"      # bright blue bg (4.3)
    GAME_UNFILLED = "\033[48;5;160m"   # bright red bg (4.1)
    GAME_MIXED = "\033[48;5;128m"      # magenta bg
    GAME_LEAF = "\033[48;5;28m"        # green bg
    GAME_NOJR = "\033[48;5;238m"       # dark gray bg

    # Library code - dimmer backgrounds with textured foreground
    LIB_FILLED = "\033[38;5;19;48;5;25m"    # dark blue on medium blue (4.3)
    LIB_UNFILLED = "\033[38;5;88;48;5;124m" # dark red on medium red (4.1)
    LIB_MIXED = "\033[38;5;90;48;5;127m"    # dark magenta on medium magenta
    LIB_LEAF = "\033[38;5;22;48;5;29m"      # dark green on medium green
    LIB_NOJR = "\033[38;5;234;48;5;240m"    # very dark gray on medium gray

    GAP = "\033[48;5;232m"               # near-black bg

    # Ambiguous — yellow/orange (can't determine toolchain)
    GAME_AMBIGUOUS = "\033[48;5;136m"      # dark yellow bg
    LIB_AMBIGUOUS = "\033[38;5;94;48;5;137m"  # dark on medium yellow

    COLOR_MAP = {
        (False, "FILLED"):    GAME_FILLED,
        (False, "UNFILLED"):  GAME_UNFILLED,
        (False, "MIXED"):     GAME_MIXED,
        (False, "AMBIGUOUS"): GAME_AMBIGUOUS,
        (False, "LEAF"):      GAME_LEAF,
        (False, "NO_JR_RA"):  GAME_NOJR,
        (True,  "FILLED"):    LIB_FILLED,
        (True,  "UNFILLED"):  LIB_UNFILLED,
        (True,  "MIXED"):     LIB_MIXED,
        (True,  "AMBIGUOUS"): LIB_AMBIGUOUS,
        (True,  "LEAF"):      LIB_LEAF,
        (True,  "NO_JR_RA"):  LIB_NOJR,
    }

    # Block characters for dense display
    CHAR_GAME = " "        # space — color shown via background only
    CHAR_LIB = "\u2592"    # medium shade for library code (fg+bg)
    CHAR_GAP = " "

    # Build cell array: for each cell, determine what's there
    total_cells = num_rows * cols
    cells = [None] * total_cells  # None = gap

    for r in results:
        func_start = r["addr"] - ADDR_START
        func_size = max(r["size"], 4)  # at least one instruction
        func_end = func_start + func_size

        cell_start = func_start // bytes_per_cell
        cell_end = max(cell_start + 1, func_end // bytes_per_cell)

        for c in range(cell_start, min(cell_end, total_cells)):
            # If cell already has something, prefer the more "interesting" one
            existing = cells[c]
            new_entry = (r["is_lib"], r["classification"])
            if existing is None:
                cells[c] = new_entry
            else:
                # Priority: MIXED > FILLED/UNFILLED > LEAF > NO_JR_RA > gap
                priority = {"MIXED": 6, "FILLED": 5, "UNFILLED": 4, "AMBIGUOUS": 3, "LEAF": 2, "NO_JR_RA": 1}
                if priority.get(new_entry[1], 0) > priority.get(existing[1], 0):
                    cells[c] = new_entry

    # Print header
    print()
    print("Address Space Map: SLUS_008.92")
    print(f"Range: 0x{ADDR_START:08X} - 0x{ADDR_END:08X} ({total_range:,} bytes)")
    print(f"Resolution: {bytes_per_cell} bytes/char, {bytes_per_row:,} bytes/row")
    print()

    # Legend
    G = CHAR_GAME
    L = CHAR_LIB
    print("Legend:")
    print(f"  Game:    {GAME_FILLED}{G}{G}{RESET} 4.3  "
          f"{GAME_UNFILLED}{G}{G}{RESET} 4.1  "
          f"{GAME_MIXED}{G}{G}{RESET} Mixed  "
          f"{GAME_AMBIGUOUS}{G}{G}{RESET} Ambig  "
          f"{GAME_LEAF}{G}{G}{RESET} Leaf  "
          f"{GAME_NOJR}{G}{G}{RESET} Data/NoRet")
    print(f"  Library: {LIB_FILLED}{L}{L}{RESET} 4.3  "
          f"{LIB_UNFILLED}{L}{L}{RESET} 4.1  "
          f"{LIB_MIXED}{L}{L}{RESET} Mixed  "
          f"{LIB_AMBIGUOUS}{L}{L}{RESET} Ambig  "
          f"{LIB_LEAF}{L}{L}{RESET} Leaf  "
          f"{LIB_NOJR}{L}{L}{RESET} Data/NoRet")
    print(f"  {GAP}  {RESET} = gap (no .s file)")
    print()

    # Print rows
    for row in range(num_rows):
        row_addr = ADDR_START + row * bytes_per_row
        line = f"0x{row_addr:08X} "

        prev_color = None
        for col in range(cols):
            cell_idx = row * cols + col
            if cell_idx >= total_cells:
                break

            entry = cells[cell_idx]
            if entry is None:
                color = GAP
                char = CHAR_GAP
            else:
                is_lib, cls = entry
                color = COLOR_MAP.get((is_lib, cls), GAP)
                char = CHAR_LIB if is_lib else CHAR_GAME

            if color != prev_color:
                line += color
                prev_color = color
            line += char

        line += RESET
        print(line)

    print()

    # Summary statistics
    game_filled = sum(1 for r in results if not r["is_lib"] and r["classification"] == "FILLED")
    game_unfilled = sum(1 for r in results if not r["is_lib"] and r["classification"] == "UNFILLED")
    game_mixed = sum(1 for r in results if not r["is_lib"] and r["classification"] == "MIXED")
    game_leaf = sum(1 for r in results if not r["is_lib"] and r["classification"] == "LEAF")
    game_nojr = sum(1 for r in results if not r["is_lib"] and r["classification"] == "NO_JR_RA")
    lib_filled = sum(1 for r in results if r["is_lib"] and r["classification"] == "FILLED")
    lib_unfilled = sum(1 for r in results if r["is_lib"] and r["classification"] == "UNFILLED")
    lib_leaf = sum(1 for r in results if r["is_lib"] and r["classification"] == "LEAF")
    lib_nojr = sum(1 for r in results if r["is_lib"] and r["classification"] == "NO_JR_RA")

    print("--- Function Counts ---")
    print(f"  Game:    FILLED(4.3)={game_filled}  UNFILLED(4.1)={game_unfilled}  "
          f"MIXED={game_mixed}  LEAF={game_leaf}  NO_JR_RA={game_nojr}")
    print(f"  Library: FILLED(4.3)={lib_filled}  UNFILLED(4.1)={lib_unfilled}  "
          f"LEAF={lib_leaf}  NO_JR_RA={lib_nojr}")

    # Byte counts
    game_43_bytes = sum(r["size"] for r in results if not r["is_lib"] and r["classification"] in ("FILLED",))
    game_41_bytes = sum(r["size"] for r in results if not r["is_lib"] and r["classification"] in ("UNFILLED",))
    game_mixed_bytes = sum(r["size"] for r in results if not r["is_lib"] and r["classification"] == "MIXED")
    game_leaf_bytes = sum(r["size"] for r in results if not r["is_lib"] and r["classification"] == "LEAF")
    lib_bytes = sum(r["size"] for r in results if r["is_lib"])

    print()
    print("--- Byte Counts ---")
    print(f"  Game PsyQ 4.3: {game_43_bytes:,} bytes ({game_43_bytes*100/total_range:.1f}%)")
    print(f"  Game PsyQ 4.1: {game_41_bytes:,} bytes ({game_41_bytes*100/total_range:.1f}%)")
    print(f"  Game Mixed:     {game_mixed_bytes:,} bytes ({game_mixed_bytes*100/total_range:.1f}%)")
    print(f"  Game Leaf:      {game_leaf_bytes:,} bytes ({game_leaf_bytes*100/total_range:.1f}%)")
    print(f"  Library:        {lib_bytes:,} bytes ({lib_bytes*100/total_range:.1f}%)")


def main():
    parser = argparse.ArgumentParser(description="Analyze delay slots in assembly files")
    parser.add_argument("asm_dir", nargs="?", default=str(DEFAULT_ASM_DIR),
                        help=f"Directory to scan for .s files (default: {DEFAULT_ASM_DIR})")
    parser.add_argument("--map", action="store_true",
                        help="Show colored terminal visualization")
    parser.add_argument("--width", type=int, default=160,
                        help="Terminal width for --map (default: 160)")
    args = parser.parse_args()

    asm_dir = Path(args.asm_dir)
    if not asm_dir.exists():
        print(f"Error: {asm_dir} not found", file=sys.stderr)
        sys.exit(1)

    results = load_all(asm_dir)

    if args.map:
        cmd_map(results, width=args.width)
    else:
        cmd_list(results)


if __name__ == "__main__":
    main()
