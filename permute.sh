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
PSYQ41_DIR="${SCRIPT_DIR}/tools/psyq4.1"
PSYQ43_DIR="${SCRIPT_DIR}/tools/psyq4.3"
CCPSX_DRIVER="${WIBO} ${PSYQ41_DIR}/CCPSX.EXE"
MASPSX="python3 ${SCRIPT_DIR}/tools/maspsx/maspsx.py"
AS="mipsel-linux-gnu-as"

CCPSXFLAGS="-O2 -G0"
ASFLAGS="-march=r3000 -mabi=32 -EL -no-pad-sections -O0 -Iinclude"

# Sources compiled without -G0 (must match Makefile NO_G0_SRCS)
NO_G0_SRCS="src/1D2C.c"

# Sources compiled with -G4 (must match Makefile G4_SRCS)
G4_SRCS="src/10DD0.c"

# Default source file and asm directory
SRC_FILE="src/1C38.c"
ASM_SUBDIR="asm/nonmatchings/1C38"

# PsyQ version (4.1 or 4.3)
PSYQ_VER="4.1"

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
        --psyq)
            PSYQ_VER="$2"
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
    echo "Usage: $0 [--run] [-j N] [--src SRC_FILE] [--asm-dir ASM_DIR] [--psyq 4.1|4.3] <function_name>"
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

# Select toolchain based on PsyQ version
case "${PSYQ_VER}" in
    4.1)
        PSYQ_DIR="${PSYQ41_DIR}"
        ASPSX_VER="2.67"
        ;;
    4.3)
        PSYQ_DIR="${PSYQ43_DIR}"
        ASPSX_VER="2.77"
        ;;
    *)
        echo "Error: unsupported PsyQ version '${PSYQ_VER}' (use 4.1 or 4.3)"
        exit 1
        ;;
esac

# Auto-detect compile flags based on source file
COMPILE_FLAGS="-O2 -G0"
for ng0 in ${NO_G0_SRCS}; do
    if [[ "${SRC_FILE}" == "${ng0}" ]]; then
        COMPILE_FLAGS="-O2"
        break
    fi
done
for g4 in ${G4_SRCS}; do
    if [[ "${SRC_FILE}" == "${g4}" ]]; then
        COMPILE_FLAGS="-O2 -G4"
        break
    fi
done

echo "Setting up permuter for ${FUNC_NAME} (PsyQ ${PSYQ_VER}, flags: ${COMPILE_FLAGS})..."

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
CCPSX="${CCPSX_DRIVER}"
MASPSX="python3 ${SCRIPT_DIR}/tools/maspsx/maspsx.py"
AS="${AS}"
ASPSX_VER="${ASPSX_VER}"
COMPILE_FLAGS="${COMPILE_FLAGS}"
EOF

cat >> "${FUNC_DIR}/compile.sh" <<'COMPILE_EOF'

cd "${DIR}"
TMPDIR=$(mktemp -d)
trap "rm -rf ${TMPDIR}" EXIT

# Compile with CCPSX -S → maspsx → GAS → .o
SN_PATH="${PSYQ_DIR}" ${CCPSX} -S -Iinclude -DPERMUTER \
    ${COMPILE_FLAGS} "${INPUT}" -o "${TMPDIR}/out.s"
cat "${TMPDIR}/out.s" | ${MASPSX} --aspsx-version=${ASPSX_VER} --expand-div \
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
