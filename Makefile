# FF8 PS1 Decompilation Build
# Target: SLUS_008.92 (Final Fantasy VIII, USA)

### Toolchain ###
WIBO       := tools/wibo
PSYQ41_DIR := tools/psyq4.1
PSYQ43_DIR := tools/psyq4.3
CCPSX      := $(WIBO) $(PSYQ41_DIR)/CCPSX.EXE
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

### Compiler flags ###
CCPSXFLAGS := -O2 -G0

### Per-toolchain settings ###
# PsyQ 4.1: gcc 2.7.2-970404 + aspsx 2.67
PSYQ41_SN_PATH   := $(PSYQ41_DIR)
PSYQ41_MASPSXFLAGS := --aspsx-version=2.67

# PsyQ 4.3: gcc 2.8.0 + aspsx 2.77
PSYQ43_SN_PATH   := $(PSYQ43_DIR)
PSYQ43_MASPSXFLAGS := --aspsx-version=2.77

# Source files compiled with PsyQ 4.3 (default is PsyQ 4.1)
PSYQ43_SRCS := src/34C8.c src/4EF0.c src/57FC.c src/75B4.c src/A118.c src/AD5C.c src/C9A8.c

# Source files compiled without -G0 (default is -G0)
NO_G0_SRCS := src/1D2C.c src/10DD0.c

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
C_SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/psxsdk/*.c) $(wildcard $(SRC_DIR)/psxsdk/*/*.c)
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
# Select PsyQ 4.1 or 4.3 based on whether the source is in PSYQ43_SRCS
# Select -G0 or no -G0 based on whether the source is in NO_G0_SRCS
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(if $(filter $<,$(PSYQ43_SRCS)), \
		SN_PATH=$(PSYQ43_SN_PATH) $(CCPSX) -S -Iinclude $(CCPSXFLAGS) $< -o $(BUILD_DIR)/$(*F).s && \
		cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(PSYQ43_MASPSXFLAGS) --run-assembler $(ASFLAGS) -o $@, \
		SN_PATH=$(PSYQ41_SN_PATH) $(CCPSX) -S -Iinclude $(if $(filter $<,$(NO_G0_SRCS)),-O2,$(CCPSXFLAGS)) $< -o $(BUILD_DIR)/$(*F).s && \
		cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(PSYQ41_MASPSXFLAGS) --run-assembler $(ASFLAGS) -o $@)

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
	rm -rf asm
	$(SPLAT) split $(SPLAT_YAML)

clean:
	rm -rf $(BUILD_DIR)

# Run decomp-permuter for a function: make permute FUNC=func_name
permute:
ifndef FUNC
	$(error Usage: make permute FUNC=<function_name>)
endif
	./permute.sh $(FUNC)

# Download PsyQ toolchains (wibo + CCPSX binaries)
setup-toolchain:
	curl -L -o tools/wibo https://github.com/decompals/wibo/releases/download/1.0.1/wibo-x86_64
	chmod +x tools/wibo
	curl -L -o /tmp/psyq4.1.tar.gz https://github.com/mkst/esa/releases/download/psyq-binaries/psyq4.1.tar.gz
	mkdir -p tools/psyq4.1
	tar xzf /tmp/psyq4.1.tar.gz --strip-components=1 -C tools/psyq4.1
	curl -L -o /tmp/psyq4.3.tar.gz https://github.com/mkst/esa/releases/download/psyq-binaries/psyq4.3.tar.gz
	mkdir -p tools/psyq4.3
	tar xzf /tmp/psyq4.3.tar.gz --strip-components=1 -C tools/psyq4.3

### Overlays ###
OVERLAYS := menumain menucfg menupty menusts menuabl menushop menuext \
            menuitem menumgc menugf menujnc2 menusav menucrd menututo \
            menutmag menutips menutest

# Template for overlay build rules — $(1) = overlay name
define OVERLAY_TEMPLATE
$(1)_YAML     := config/$(1).ovl.yaml
$(1)_DIR      := build/ovl/$(1)
$(1)_ASM_DIR  := asm/ovl/$(1)
$(1)_LD       := config/$(1).ovl.ld
$(1)_TARGET   := original/$(1).ovl
$(1)_ELF      := $$($(1)_DIR)/$(1).elf
$(1)_BIN      := $$($(1)_DIR)/$(1).ovl

$(1)_ASM_SRCS := $$(wildcard $$($(1)_ASM_DIR)/*.s) $$(wildcard $$($(1)_ASM_DIR)/data/*.s)
$(1)_ASM_OBJS := $$(patsubst $$($(1)_ASM_DIR)/%.s,$$($(1)_DIR)/$$($(1)_ASM_DIR)/%.o,$$($(1)_ASM_SRCS))
$(1)_C_SRCS   := $$(wildcard src/ovl/$(1)/*.c)
$(1)_C_OBJS   := $$(patsubst src/ovl/$(1)/%.c,$$($(1)_DIR)/src/ovl/$(1)/%.o,$$($(1)_C_SRCS))
$(1)_ALL_OBJS := $$($(1)_ASM_OBJS) $$($(1)_C_OBJS)

$(1)_LDFLAGS  := -T $$($(1)_LD) \
                 -T config/undefined_funcs_auto.$(1).txt \
                 -T config/undefined_syms_auto.$(1).txt \
                 --no-check-sections \
                 -Map $$($(1)_DIR)/$(1).map

split-$(1):
	$$(SPLAT) split $$($(1)_YAML)

$$($(1)_DIR)/$$($(1)_ASM_DIR)/%.o: $$($(1)_ASM_DIR)/%.s
	@mkdir -p $$(dir $$@)
	$$(AS) $$(ASFLAGS) -o $$@ $$<

$$($(1)_DIR)/src/ovl/$(1)/%.o: src/ovl/$(1)/%.c
	@mkdir -p $$(dir $$@)
	SN_PATH=$$(PSYQ41_SN_PATH) $$(CCPSX) -S -Iinclude $$(CCPSXFLAGS) $$< -o $$($(1)_DIR)/$$(*F).s && \
	cat $$($(1)_DIR)/$$(*F).s | $$(MASPSX) $$(PSYQ41_MASPSXFLAGS) --run-assembler $$(ASFLAGS) -o $$@

$$($(1)_ELF): $$($(1)_ALL_OBJS) $$($(1)_LD)
	@mkdir -p $$(dir $$@)
	$$(LD) $$($(1)_LDFLAGS) -o $$@ $$($(1)_ALL_OBJS)

$$($(1)_BIN): $$($(1)_ELF)
	$$(OBJCOPY) -O binary $$< $$@

build-$(1): $$($(1)_BIN)

verify-$(1): $$($(1)_BIN)
	@echo "Verifying $(1).ovl..."
	@BUILT=$$$$(sha1sum $$($(1)_BIN) | cut -d' ' -f1) && \
	ORIG=$$$$(sha1sum $$($(1)_TARGET) | cut -d' ' -f1) && \
	echo "  Original: $$$$ORIG" && \
	echo "  Built:    $$$$BUILT" && \
	if [ "$$$$BUILT" = "$$$$ORIG" ]; then \
		echo "MATCH!"; \
	else \
		echo "MISMATCH!"; \
		exit 1; \
	fi

endef

$(foreach ovl,$(OVERLAYS),$(eval $(call OVERLAY_TEMPLATE,$(ovl))))

# Meta-targets for all overlays
split-overlays: $(foreach ovl,$(OVERLAYS),split-$(ovl))
build-overlays: $(foreach ovl,$(OVERLAYS),build-$(ovl))
verify-overlays: $(foreach ovl,$(OVERLAYS),verify-$(ovl))

OVL_SPLIT_TARGETS  := $(foreach ovl,$(OVERLAYS),split-$(ovl))
OVL_BUILD_TARGETS  := $(foreach ovl,$(OVERLAYS),build-$(ovl))
OVL_VERIFY_TARGETS := $(foreach ovl,$(OVERLAYS),verify-$(ovl))

.PHONY: all build verify setup setup-toolchain split clean permute \
        $(OVL_SPLIT_TARGETS) $(OVL_BUILD_TARGETS) $(OVL_VERIFY_TARGETS) \
        split-overlays build-overlays verify-overlays
