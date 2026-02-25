#!/usr/bin/env bash
# Setup and optionally run decomp-permuter for a given function.
#
# Usage:
#   ./permute.sh <function_name>              # setup only
#   ./permute.sh --run [-j N] <function_name> # setup + run permuter
#
# Example:
#   ./permute.sh func_80011438
#   ./permute.sh --run -j4 func_80011438

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PERMUTER_DIR="${SCRIPT_DIR}/tools/decomp-permuter"
WORK_DIR="${SCRIPT_DIR}/permuter"

# Toolchain (must match Makefile)
WIBO="${SCRIPT_DIR}/tools/wibo"
PSYQ_DIR="${SCRIPT_DIR}/tools/psyq4.1"
CCPSX="${WIBO} ${PSYQ_DIR}/CCPSX.EXE"
MASPSX="python3 ${SCRIPT_DIR}/tools/maspsx/maspsx.py"
AS="mipsel-linux-gnu-as"

CCPSXFLAGS="-O2 -G0"
ASFLAGS="-march=r3000 -mabi=32 -EL -no-pad-sections -O0 -Iinclude"

# Default source file and asm directory
SRC_FILE="src/1C38.c"
ASM_SUBDIR="asm/nonmatchings/1C38"

# Parse arguments
RUN=0
PERMUTER_ARGS=()
FUNC_NAME=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --run)
            RUN=1
            shift
            ;;
        -j*)
            PERMUTER_ARGS+=("$1")
            shift
            ;;
        --src)
            SRC_FILE="$2"
            shift 2
            ;;
        --asm-dir)
            ASM_SUBDIR="$2"
            shift 2
            ;;
        -*)
            PERMUTER_ARGS+=("$1")
            shift
            ;;
        *)
            FUNC_NAME="$1"
            shift
            ;;
    esac
done

if [[ -z "${FUNC_NAME}" ]]; then
    echo "Usage: $0 [--run] [-j N] [--src SRC_FILE] [--asm-dir ASM_DIR] <function_name>"
    exit 1
fi

# Verify the assembly file exists
TARGET_ASM="${ASM_SUBDIR}/${FUNC_NAME}.s"
if [[ ! -f "${TARGET_ASM}" ]]; then
    echo "Error: assembly file not found: ${TARGET_ASM}"
    exit 1
fi

# Verify the permuter is available
if [[ ! -f "${PERMUTER_DIR}/permuter.py" ]]; then
    echo "Error: decomp-permuter not found. Run: git submodule update --init tools/decomp-permuter"
    exit 1
fi

echo "Setting up permuter for ${FUNC_NAME}..."

# Create working directory
FUNC_DIR="${WORK_DIR}/${FUNC_NAME}"
mkdir -p "${FUNC_DIR}"

# --- settings.toml ---
cat > "${FUNC_DIR}/settings.toml" <<EOF
func_name = "${FUNC_NAME}"
compiler_type = "gcc"
EOF

# --- compile.sh ---
cat > "${FUNC_DIR}/compile.sh" <<'COMPILE_EOF'
#!/usr/bin/env bash
# Compile script for decomp-permuter
# Usage: ./compile.sh input.c -o output.o
set -euo pipefail

INPUT="$1"
shift
# Parse -o flag
OUTPUT=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        -o) OUTPUT="$2"; shift 2 ;;
        *) shift ;;
    esac
done

if [[ -z "${OUTPUT}" ]]; then
    echo "Error: -o flag required"
    exit 1
fi

COMPILE_EOF

# Append the toolchain paths using the resolved script dir
cat >> "${FUNC_DIR}/compile.sh" <<EOF
DIR="${SCRIPT_DIR}"
WIBO="${WIBO}"
PSYQ_DIR="${PSYQ_DIR}"
CCPSX="${CCPSX}"
MASPSX="python3 ${SCRIPT_DIR}/tools/maspsx/maspsx.py"
AS="${AS}"
EOF

cat >> "${FUNC_DIR}/compile.sh" <<'COMPILE_EOF'

cd "${DIR}"
TMPDIR=$(mktemp -d)
trap "rm -rf ${TMPDIR}" EXIT

# Compile with CCPSX -S → maspsx → GAS → .o
SN_PATH="${PSYQ_DIR}" ${CCPSX} -S -Iinclude -DPERMUTER \
    -O2 -G0 "${INPUT}" -o "${TMPDIR}/out.s"
cat "${TMPDIR}/out.s" | ${MASPSX} --aspsx-version=2.67 --expand-div \
    --run-assembler -march=r3000 -mabi=32 -EL -no-pad-sections -O0 -Iinclude \
    -o "${OUTPUT}"
COMPILE_EOF

chmod +x "${FUNC_DIR}/compile.sh"

# --- target.s ---
# Prepend macro.inc include so that glabel/endlabel/etc. are defined
{
    echo '.include "include/macro.inc"'
    echo ""
    cat "${TARGET_ASM}"
} > "${FUNC_DIR}/target.s"

# --- target.o ---
# Assemble the target so the permuter has a .o to compare against
cd "${SCRIPT_DIR}"
${AS} ${ASFLAGS} -o "${FUNC_DIR}/target.o" "${FUNC_DIR}/target.s"

# --- base.c ---
# Preprocess with -DPERMUTER so INCLUDE_ASM macros become no-ops,
# then strip to just the target function
mipsel-linux-gnu-cpp -Iinclude -DPERMUTER -undef -Wall -fno-builtin -lang-c "${SRC_FILE}" > "${FUNC_DIR}/base.c.tmp"

# Use the permuter's strip_other_fns.py to isolate the function
# Note: strip_other_fns.py modifies the file in-place
cp "${FUNC_DIR}/base.c.tmp" "${FUNC_DIR}/base.c"
rm "${FUNC_DIR}/base.c.tmp"
if [[ -f "${PERMUTER_DIR}/strip_other_fns.py" ]]; then
    python3 "${PERMUTER_DIR}/strip_other_fns.py" "${FUNC_DIR}/base.c" "${FUNC_NAME}"
else
    echo "Warning: strip_other_fns.py not found, using full preprocessed source"
fi

echo "Permuter directory ready: ${FUNC_DIR}"
echo "  settings.toml  - permuter settings"
echo "  compile.sh     - compilation pipeline"
echo "  target.s       - target assembly"
echo "  target.o       - assembled target"
echo "  base.c         - preprocessed C source"

# --- Optionally run the permuter ---
if [[ "${RUN}" -eq 1 ]]; then
    echo ""
    echo "Running permuter..."
    python3 "${PERMUTER_DIR}/permuter.py" "${PERMUTER_ARGS[@]}" "${FUNC_DIR}"
fi
