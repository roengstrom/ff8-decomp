# FF8 PS1 Decompilation Build
# Target: SLUS_008.92 (Final Fantasy VIII, USA)

### Toolchain ###
WIBO       := tools/wibo
PSYQ_DIR   := tools/psyq4.1
CCPSX      := $(WIBO) $(PSYQ_DIR)/CCPSX.EXE
AS         := mipsel-linux-gnu-as
LD         := mipsel-linux-gnu-ld
OBJCOPY    := mipsel-linux-gnu-objcopy
MASPSX     := python3 tools/maspsx/maspsx.py

### Paths ###
VENV       := .venv
PYTHON     := $(VENV)/bin/python3
SPLAT      := $(PYTHON) -m splat
SPLAT_YAML := slus_008.92.yaml
BUILD_DIR  := build
ASM_DIR    := asm
SRC_DIR    := src
TARGET     := original/SLUS_008.92
LD_SCRIPT  := slus_008.92.ld

### CCPSX flags (PsyQ 4.1 / gcc cygnus-2.7.2-970404) ###
CCPSXFLAGS := -O2 -G0

### maspsx flags ###
MASPSXFLAGS := --aspsx-version=2.67 --expand-div

### Assembler flags ###
# -march=r3000  : MIPS I (the PS1 CPU)
# -mabi=32      : 32-bit ABI
# -EL           : little-endian
# -no-pad-sections : don't add padding between sections
# -Iinclude     : search include/ for .inc files
ASFLAGS := -march=r3000 -mabi=32 -EL -no-pad-sections -O0 -Iinclude

### Linker flags ###
# -T : use these scripts/symbol files to resolve addresses
# --no-check-sections : don't error on overlapping sections
# -Map : generate a map file (shows where everything ended up)
LDFLAGS := -T $(LD_SCRIPT) \
           -T undefined_funcs.txt \
           -T undefined_funcs_auto.txt \
           -T undefined_syms_auto.txt \
           --no-check-sections \
           -Map $(BUILD_DIR)/slus_008.92.map

### Output files ###
ELF       := $(BUILD_DIR)/slus_008.92.elf
BUILT_EXE := $(BUILD_DIR)/SLUS_008.92

### Collect source files ###
# Assembly sources (header + data segments)
ASM_SRCS := $(wildcard $(ASM_DIR)/*.s) $(wildcard $(ASM_DIR)/data/*.s)
ASM_OBJS := $(patsubst $(ASM_DIR)/%.s,$(BUILD_DIR)/$(ASM_DIR)/%.o,$(ASM_SRCS))

# C sources (compiled via CCPSX -S → maspsx → GAS)
C_SRCS := $(wildcard $(SRC_DIR)/*.c)
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/$(SRC_DIR)/%.o,$(C_SRCS))

# All objects for linking
ALL_OBJS := $(ASM_OBJS) $(C_OBJS)

### Targets ###

# Default: build and verify
all: verify

# Assemble: .s -> .o
$(BUILD_DIR)/$(ASM_DIR)/%.o: $(ASM_DIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Compile C: CCPSX -S → maspsx → GAS → .o
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	SN_PATH=$(PSYQ_DIR) $(CCPSX) -S -Iinclude $(CCPSXFLAGS) $< -o $(BUILD_DIR)/$(*F).s
	cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(MASPSXFLAGS) --run-assembler $(ASFLAGS) -o $@

# Link: all .o files -> ELF
$(ELF): $(ALL_OBJS) $(LD_SCRIPT)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJS)

# Convert: ELF -> raw binary (the PS-EXE)
$(BUILT_EXE): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Build everything
build: $(BUILT_EXE)

# Build and compare SHA1 against original
verify: $(BUILT_EXE)
	@echo "Verifying..."
	@BUILT=$$(sha1sum $(BUILT_EXE) | cut -d' ' -f1) && \
	ORIG=$$(sha1sum $(TARGET) | cut -d' ' -f1) && \
	echo "  Original: $$ORIG" && \
	echo "  Built:    $$BUILT" && \
	if [ "$$BUILT" = "$$ORIG" ]; then \
		echo "MATCH!"; \
	else \
		echo "MISMATCH!"; \
		exit 1; \
	fi

# First-time setup: create venv, install dependencies, extract PS-EXE, run splat
setup:
	python3 -m venv $(VENV)
	$(PYTHON) -m pip install -r requirements.txt
	$(SPLAT) split $(SPLAT_YAML)

# Re-run splat (regenerate asm + linker script)
split:
	$(SPLAT) split $(SPLAT_YAML)

clean:
	rm -rf $(BUILD_DIR)

# Run decomp-permuter for a function: make permute FUNC=func_name
permute:
ifndef FUNC
	$(error Usage: make permute FUNC=<function_name>)
endif
	./permute.sh $(FUNC)

# Download PsyQ 4.1 toolchain (wibo + CCPSX binaries)
setup-toolchain:
	curl -L -o tools/wibo https://github.com/decompals/wibo/releases/download/1.0.1/wibo-x86_64
	chmod +x tools/wibo
	curl -L -o /tmp/psyq4.1.tar.gz https://github.com/mkst/esa/releases/download/psyq-binaries/psyq4.1.tar.gz
	mkdir -p tools/psyq4.1
	tar xzf /tmp/psyq4.1.tar.gz --strip-components=1 -C tools/psyq4.1

.PHONY: all build verify setup setup-toolchain split clean permute
