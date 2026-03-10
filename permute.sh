#!/usr/bin/env bash
# Setup and optionally run decomp-permuter for a given function.
#
# Usage:
#   ./permute.sh <function_name>              # auto-finds .s, infers source
#   ./permute.sh --run [-j N] <function_name> # setup + run permuter
#
# Example:
#   ./permute.sh func_8009B6D0
#   ./permute.sh --run -j4 func_80021358

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
NO_G0_SRCS="src/main.c"

# Sources compiled with -G4 (must match Makefile G4_SRCS)
G4_SRCS="src/game.c"

# Sources compiled with PsyQ 4.3 (must match Makefile PSYQ43_SRCS)
PSYQ43_SRCS="src/snd_init.c src/snd_dma.c src/snd_voice.c src/snd_bank.c src/snd_param.c src/snd_note.c src/snd_track.c"

SRC_FILE=""
ASM_SUBDIR=""
PSYQ_VER=""

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
    echo "Usage: $0 [--run] [-j N] [--psyq 4.1|4.3] <function_name>"
    exit 1
fi

# Auto-find the .s file if --asm-dir not given
if [[ -z "${ASM_SUBDIR}" ]]; then
    S_FILE="$(find asm -name "${FUNC_NAME}.s" 2>/dev/null | head -1)"
    if [[ -z "${S_FILE}" ]]; then
        echo "Error: ${FUNC_NAME}.s not found under asm/"
        exit 1
    fi
    ASM_SUBDIR="$(dirname "${S_FILE}")"
fi

TARGET_ASM="${ASM_SUBDIR}/${FUNC_NAME}.s"
if [[ ! -f "${TARGET_ASM}" ]]; then
    echo "Error: assembly file not found: ${TARGET_ASM}"
    exit 1
fi

# Auto-detect source file from asm path if --src not given
if [[ -z "${SRC_FILE}" ]]; then
    C_BASENAME="$(basename "${ASM_SUBDIR}")"
    if [[ "${ASM_SUBDIR}" == asm/ovl/* ]]; then
        OVERLAY="$(echo "${ASM_SUBDIR}" | cut -d/ -f3)"
        SRC_FILE="src/ovl/${OVERLAY}/${C_BASENAME}.c"
    else
        SRC_FILE="src/${C_BASENAME}.c"
    fi
fi

if [[ ! -f "${SRC_FILE}" ]]; then
    echo "Error: inferred source file not found: ${SRC_FILE}"
    echo "Use --src to specify explicitly."
    exit 1
fi

# Verify the permuter is available
if [[ ! -f "${PERMUTER_DIR}/permuter.py" ]]; then
    echo "Error: decomp-permuter not found. Run: git submodule update --init tools/decomp-permuter"
    exit 1
fi

# Auto-detect PsyQ version from source file if not specified
if [[ -z "${PSYQ_VER}" ]]; then
    PSYQ_VER="4.1"
    for p43 in ${PSYQ43_SRCS}; do
        if [[ "${SRC_FILE}" == "${p43}" ]]; then
            PSYQ_VER="4.3"
            break
        fi
    done
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
echo "  Source:  ${SRC_FILE}"
echo "  Asm dir: ${ASM_SUBDIR}"

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
ASPSX="${WIBO} ${PSYQ_DIR}/ASPSX.EXE"
PSYQ_OBJ_PARSER="${SCRIPT_DIR}/tools/psyq-obj-parser"
COMPILE_FLAGS="${COMPILE_FLAGS}"
EOF

cat >> "${FUNC_DIR}/compile.sh" <<'COMPILE_EOF'

cd "${DIR}"
TMPDIR=$(mktemp -d)
trap "rm -rf ${TMPDIR}" EXIT

# Compile with CCPSX -S → ASPSX → psyq-obj-parser → .o
SN_PATH="${PSYQ_DIR}" ${CCPSX} -S -Iinclude -DPERMUTER \
    ${COMPILE_FLAGS} "${INPUT}" -o "${TMPDIR}/out.s"
${ASPSX} -q "${TMPDIR}/out.s" -o "${TMPDIR}/out.obj"
${PSYQ_OBJ_PARSER} "${TMPDIR}/out.obj" -o "${OUTPUT}" > /dev/null 2>&1
COMPILE_EOF

chmod +x "${FUNC_DIR}/compile.sh"

# --- target.s ---
# Prepend macro.inc include + .set noreorder (matches INCLUDE_ASM wrapper)
{
    echo '.include "include/macro.inc"'
    echo '.set noreorder'
    echo '.set noat'
    echo ""
    cat "${TARGET_ASM}"
} > "${FUNC_DIR}/target.s"

# --- target.o ---
# Assemble the target so the permuter has a .o to compare against
cd "${SCRIPT_DIR}"
${AS} ${ASFLAGS} -o "${FUNC_DIR}/target.o" "${FUNC_DIR}/target.s"

# --- base.c ---
# Try to extract the function from the source file. If the function isn't
# decomped yet (INCLUDE_ASM or absent), generate an empty stub instead.
HAVE_DECOMP=0
if grep -q "${FUNC_NAME}" "${SRC_FILE}" 2>/dev/null && \
   ! grep -q "INCLUDE_ASM.*${FUNC_NAME}" "${SRC_FILE}" 2>/dev/null; then
    HAVE_DECOMP=1
fi

if [[ "${HAVE_DECOMP}" -eq 1 ]]; then
    mipsel-linux-gnu-cpp -Iinclude -DPERMUTER -undef -Wall -fno-builtin -lang-c "${SRC_FILE}" > "${FUNC_DIR}/base.c.tmp"
    cp "${FUNC_DIR}/base.c.tmp" "${FUNC_DIR}/base.c"
    rm "${FUNC_DIR}/base.c.tmp"
    if [[ -f "${PERMUTER_DIR}/strip_other_fns.py" ]]; then
        python3 "${PERMUTER_DIR}/strip_other_fns.py" "${FUNC_DIR}/base.c" "${FUNC_NAME}"
    fi
    BASE_C_DESC="extracted from source"
else
    cat > "${FUNC_DIR}/base.c" <<BASEC_EOF
void ${FUNC_NAME}(void) {
    // TODO: decompile this function
}
BASEC_EOF
    BASE_C_DESC="empty stub"
fi

echo ""
echo "Permuter directory ready: ${FUNC_DIR}"
echo "  settings.toml  - permuter settings"
echo "  compile.sh     - compilation pipeline"
echo "  target.s       - target assembly"
echo "  target.o       - assembled target"
echo "  base.c         - ${BASE_C_DESC}"

# --- Optionally run the permuter ---
if [[ "${RUN}" -eq 1 ]]; then
    echo ""
    echo "Running permuter..."
    python3 "${PERMUTER_DIR}/permuter.py" "${PERMUTER_ARGS[@]}" "${FUNC_DIR}"
fi
