#!/usr/bin/env python3
"""Automated decompilation tool.

Generates initial C from assembly using m2c, then iteratively runs the
decomp-permuter to find a byte-matching decompilation.

Usage:
    python3 tools/autodecomp.py func_80018358
    python3 tools/autodecomp.py --rounds 5 --time 120 -j8 func_80018358
"""

import argparse
import glob
import os
import re
import shutil
import signal
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PERMUTER_DIR = os.path.join(ROOT, "tools", "decomp-permuter")
M2C = os.path.join(ROOT, "tools", "m2c", "m2c.py")
PERMUTE_SH = os.path.join(ROOT, "permute.sh")


def find_asm_file(func_name):
    """Find the .s file for a function across all nonmatchings directories."""
    patterns = [
        os.path.join(ROOT, "asm", "nonmatchings", "**", f"{func_name}.s"),
        os.path.join(ROOT, "asm", "ovl", "**", "nonmatchings", "**", f"{func_name}.s"),
    ]
    for pattern in patterns:
        matches = glob.glob(pattern, recursive=True)
        if matches:
            return matches[0]
    return None


def find_source_file(func_name):
    """Find the .c source file that contains this function (via INCLUDE_ASM)."""
    for pattern in ["src/*.c", "src/ovl/**/*.c"]:
        for f in glob.glob(os.path.join(ROOT, pattern), recursive=True):
            with open(f) as fh:
                if f"INCLUDE_ASM(" in fh.read() and func_name in open(f).read():
                    return f
    return None


def run_m2c(asm_file):
    """Run m2c on an assembly file and return the C output."""
    result = subprocess.run(
        ["python3", M2C, "-t", "mipsel-gcc-c", "--valid-syntax", asm_file],
        capture_output=True, text=True, cwd=ROOT,
    )
    if result.returncode != 0:
        print(f"m2c failed: {result.stderr}", file=sys.stderr)
        return None
    return result.stdout


def make_compilable(m2c_output, func_name):
    """Make m2c output compilable for the permuter.

    The permuter preprocesses base.c with ``cpp -P -nostdinc``, so we cannot
    use #include directives.  Instead we inline every type and macro that the
    generated code needs.
    """
    # Inline preamble: base types + m2c macros (subset of common.h + m2c_macros.h)
    preamble = """\
/* --- inlined types (common.h) --- */
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#ifndef NULL
#define NULL (void *)0
#endif

/* --- inlined m2c macros (m2c_macros.h) --- */
typedef s32 M2C_UNK;
typedef s8  M2C_UNK8;
typedef s16 M2C_UNK16;
typedef s32 M2C_UNK32;

#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) ((type)(expr))
#define M2C_ERROR(desc) (0)
#define MULT_HI(a, b) (0)
#define MULTU_HI(a, b) (0)

"""

    # Separate extern/forward declarations from the function body.
    # Move externs INSIDE the function so the permuter can mutate them
    # (e.g. adding volatile, changing types).
    lines = m2c_output.strip().split("\n")
    externs = []
    func_body = []
    in_func = False
    brace_depth = 0

    for line in lines:
        stripped = line.strip()
        # Remove /* extern */ comments
        stripped = stripped.replace("/* extern */", "").rstrip()

        if not in_func:
            # Detect function definition start (not a declaration ending with ;)
            if (re.match(r"^(void|s8|s16|s32|u8|u16|u32|M2C_UNK)\s+\w+\s*\(", stripped)
                    and not stripped.endswith(";")):
                in_func = True
                func_body.append(stripped)
            elif stripped:
                externs.append(stripped)
        else:
            func_body.append(line)

        # Track braces to find end of function
        if in_func:
            brace_depth += line.count("{") - line.count("}")

    # Build the function with externs inside the opening brace
    result_lines = [preamble.rstrip()]

    if func_body:
        # Find the opening { and insert externs right after it
        output = []
        inserted = False
        for line in func_body:
            output.append(line)
            if not inserted and "{" in line:
                # Insert externs as local declarations inside the function
                for ext in externs:
                    output.append(f"    {ext}")
                if externs:
                    output.append("")
                inserted = True

        result_lines.append("\n".join(output))
    else:
        # Fallback: no function body found, just concatenate everything
        result_lines.extend(externs)

    return "\n".join(result_lines) + "\n"


def setup_permuter(func_name, src_file, asm_dir, psyq_version=None):
    """Run permute.sh to set up the permuter directory."""
    func_dir = os.path.join(ROOT, "permuter", func_name)

    cmd = [PERMUTE_SH]
    if src_file:
        cmd.extend(["--src", src_file])
    if asm_dir:
        cmd.extend(["--asm-dir", asm_dir])
    if psyq_version:
        cmd.extend(["--psyq", psyq_version])
    cmd.append(func_name)

    result = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    if result.returncode != 0:
        print(f"permute.sh failed:\n{result.stdout}\n{result.stderr}", file=sys.stderr)
        return None

    return func_dir


def get_best_score(func_dir):
    """Find the best (lowest) score from permuter output directories."""
    best_score = None
    best_source = None
    for score_file in glob.glob(os.path.join(func_dir, "output-*", "score.txt")):
        with open(score_file) as f:
            try:
                score = int(f.read().strip())
            except ValueError:
                continue
        if best_score is None or score < best_score:
            best_score = score
            best_source = os.path.join(os.path.dirname(score_file), "source.c")
    return best_score, best_source


def clean_outputs(func_dir):
    """Remove all output directories to start fresh."""
    for d in glob.glob(os.path.join(func_dir, "output-*")):
        shutil.rmtree(d)


def run_permuter(func_dir, jobs, timeout_secs):
    """Run the permuter for a given timeout. Returns the process exit code."""
    cmd = [
        "python3", os.path.join(PERMUTER_DIR, "permuter.py"),
        "--stop-on-zero",
        "--better-only",
        f"-j{jobs}",
        func_dir,
    ]

    try:
        proc = subprocess.Popen(
            cmd, cwd=ROOT,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, preexec_fn=os.setsid,
        )
        try:
            stdout, _ = proc.communicate(timeout=timeout_secs)
            print(stdout, end="")
            return proc.returncode
        except subprocess.TimeoutExpired:
            # Kill the entire process group
            os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
                proc.wait()
            return -1
    except KeyboardInterrupt:
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        proc.wait()
        raise


def detect_psyq_version(src_file):
    """Detect PsyQ version from Makefile PSYQ43_SRCS."""
    makefile = os.path.join(ROOT, "Makefile")
    with open(makefile) as f:
        content = f.read()
    # Check if src_file is in PSYQ43_SRCS
    rel_path = os.path.relpath(src_file, ROOT)
    if rel_path in content.split("PSYQ43_SRCS")[1].split("\n\n")[0] if "PSYQ43_SRCS" in content else False:
        return "4.3"
    return None


def main():
    parser = argparse.ArgumentParser(description="Automated decompilation tool")
    parser.add_argument("func_name", help="Function name (e.g. func_80018358)")
    parser.add_argument("--rounds", type=int, default=10,
                        help="Maximum number of permuter rounds (default: 10)")
    parser.add_argument("--time", type=int, default=60,
                        help="Seconds per permuter round (default: 60)")
    parser.add_argument("-j", "--jobs", type=int, default=4,
                        help="Parallel permuter jobs (default: 4)")
    parser.add_argument("--src", help="Source file (auto-detected if omitted)")
    parser.add_argument("--asm-dir", help="ASM directory (auto-detected if omitted)")
    parser.add_argument("--psyq", choices=["4.1", "4.3"],
                        help="PsyQ version (auto-detected if omitted)")
    parser.add_argument("--skip-m2c", action="store_true",
                        help="Skip m2c and use existing base.c")
    args = parser.parse_args()

    os.chdir(ROOT)
    func_name = args.func_name

    # --- Find files ---
    asm_file = find_asm_file(func_name)
    if not asm_file:
        print(f"Error: Cannot find .s file for {func_name}", file=sys.stderr)
        sys.exit(1)
    print(f"Assembly: {os.path.relpath(asm_file, ROOT)}")

    src_file = args.src or find_source_file(func_name)
    if src_file:
        print(f"Source: {os.path.relpath(src_file, ROOT)}")

    # Infer asm-dir from asm file path
    asm_dir = args.asm_dir
    if not asm_dir:
        asm_dir = os.path.dirname(asm_file)

    # --- Generate initial C with m2c ---
    func_dir = os.path.join(ROOT, "permuter", func_name)

    if not args.skip_m2c:
        print(f"\n--- Generating initial C with m2c ---")
        m2c_output = run_m2c(asm_file)
        if not m2c_output:
            print("m2c produced no output, using empty stub", file=sys.stderr)
            m2c_output = f"void {func_name}(void) {{\n    // TODO\n}}\n"
        else:
            print(m2c_output)

        compilable = make_compilable(m2c_output, func_name)
    else:
        compilable = None

    # --- Set up permuter ---
    print(f"\n--- Setting up permuter ---")
    psyq = args.psyq
    if not psyq and src_file:
        psyq = detect_psyq_version(src_file)

    result = setup_permuter(func_name, src_file, asm_dir, psyq)
    if not result:
        sys.exit(1)

    # Copy m2c_macros.h to include/ if not present
    macros_dst = os.path.join(ROOT, "include", "m2c_macros.h")
    macros_src = os.path.join(ROOT, "tools", "m2c", "m2c_macros.h")
    if not os.path.exists(macros_dst) and os.path.exists(macros_src):
        shutil.copy2(macros_src, macros_dst)
        print(f"Copied m2c_macros.h to include/")

    # Replace base.c with m2c output
    if compilable:
        base_c = os.path.join(func_dir, "base.c")
        with open(base_c, "w") as f:
            f.write(compilable)
        print(f"Wrote m2c output to base.c")

    # --- Iterative permuter rounds ---
    print(f"\n--- Running permuter ({args.rounds} rounds, {args.time}s each, {args.jobs} jobs) ---")

    for round_num in range(1, args.rounds + 1):
        print(f"\n{'='*60}")
        print(f"Round {round_num}/{args.rounds}")
        print(f"{'='*60}")

        # Get initial score
        initial_score, _ = get_best_score(func_dir)
        if initial_score is not None and initial_score == 0:
            print("Already at score 0!")
            break

        # Clean previous outputs before each round
        clean_outputs(func_dir)

        # Run permuter
        run_permuter(func_dir, args.jobs, args.time)

        # Check results
        best_score, best_source = get_best_score(func_dir)
        if best_score is None:
            print(f"No improvements found this round")
            continue

        print(f"\nBest score this round: {best_score}")

        if best_score == 0:
            print(f"\nPerfect match found!")
            with open(best_source) as f:
                print(f"\n--- Matching C code ---")
                print(f.read())
            break

        # Use best result as new base for next round
        base_c = os.path.join(func_dir, "base.c")
        shutil.copy2(best_source, base_c)
        print(f"Updated base.c with best result (score {best_score})")
    else:
        # Exhausted all rounds
        best_score, best_source = get_best_score(func_dir)
        if best_source:
            print(f"\n--- Best result after {args.rounds} rounds: score {best_score} ---")
            with open(best_source) as f:
                print(f.read())
        else:
            print(f"\nNo matching candidates found after {args.rounds} rounds")

    # Final summary
    print(f"\nPermuter directory: permuter/{func_name}/")


if __name__ == "__main__":
    main()
