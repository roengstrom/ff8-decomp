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
SPLAT_YAML := config/slus_008.92.yaml
BUILD_DIR  := build
ASM_DIR    := asm
SRC_DIR    := src
TARGET     := original/SLUS_008.92
LD_SCRIPT  := config/slus_008.92.ld

### Compiler flags ###
CCPSXFLAGS := -O2 -G0

# Set NON_MATCHING=1 to compile C decomps that don't byte-match yet
# (e.g. due to ASPSX vs GAS assembler differences)
ifdef NON_MATCHING
NON_MATCHING_FLAGS := -DNON_MATCHING
endif

### Per-toolchain settings ###
# PsyQ 4.0: gcc 2.7.2-970404 + aspsx 2.56
PSYQ40_SN_PATH   := $(PSYQ41_DIR)
PSYQ40_MASPSXFLAGS := --aspsx-version=2.56

# PsyQ 4.1: gcc 2.7.2-970404 + aspsx 2.67
PSYQ41_SN_PATH   := $(PSYQ41_DIR)
PSYQ41_MASPSXFLAGS := --aspsx-version=2.67

# PsyQ 4.3: gcc 2.8.0 + aspsx 2.77
PSYQ43_SN_PATH   := $(PSYQ43_DIR)
PSYQ43_MASPSXFLAGS := --aspsx-version=2.77

# Source files compiled with PsyQ 4.3 (default is PsyQ 4.1)
PSYQ43_SRCS := src/snd_init.c src/snd_dma.c src/snd_voice.c src/snd_bank.c src/snd_param.c src/snd_note.c src/snd_track.c

# Source files compiled without -G0 (default is -G0)
NO_G0_SRCS := src/main.c

# Source files compiled with -G4 (globals ≤4 bytes use assembler pseudo expansion)
G4_SRCS := src/game.c

# Source files compiled with -O0 (unoptimized, uses frame pointer)
O0_SRCS := src/render3d.c src/mesh3d.c

# O0 files that need expand_li ON (no --aspsx-version flag) to match ori encoding
O0_EXPAND_LI_SRCS := src/render3d.c

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
           -T config/undefined_funcs.txt \
           -T config/undefined_funcs_auto.txt \
           -T config/undefined_syms_auto.txt \
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

# Default: build and verify everything
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
		SN_PATH=$(PSYQ43_SN_PATH) $(CCPSX) -S -Iinclude $(NON_MATCHING_FLAGS) $(CCPSXFLAGS) $< -o $(BUILD_DIR)/$(*F).s && \
		cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(PSYQ43_MASPSXFLAGS) --run-assembler $(ASFLAGS) -o $@, \
		SN_PATH=$(if $(filter $<,$(O0_SRCS)),$(PSYQ40_SN_PATH),$(PSYQ41_SN_PATH)) $(CCPSX) -S -Iinclude $(NON_MATCHING_FLAGS) $(if $(filter $<,$(O0_SRCS)),-O0 -G0,$(if $(filter $<,$(G4_SRCS)),-O2 -G4,$(if $(filter $<,$(NO_G0_SRCS)),-O2,$(CCPSXFLAGS)))) $< -o $(BUILD_DIR)/$(*F).s && \
		cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(if $(filter $<,$(O0_EXPAND_LI_SRCS)),,$(if $(filter $<,$(O0_SRCS)),$(PSYQ40_MASPSXFLAGS),$(PSYQ41_MASPSXFLAGS))) --run-assembler $(ASFLAGS) -o $@)

# Link: all .o files -> ELF
$(ELF): $(ALL_OBJS) $(LD_SCRIPT)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJS)

# Convert: ELF -> raw binary (the PS-EXE)
$(BUILT_EXE): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Build everything (main + overlays)
build: $(BUILT_EXE) build-overlays

# Build and compare SHA1 against originals (main + overlays)
# Builds each overlay individually, skipping any that fail to compile
verify: $(BUILT_EXE)
	@FAIL=0; \
	printf "%-20s  %-40s  %-40s  %s\n" "Name" "Expected" "Actual" "State"; \
	printf "%-20s  %-40s  %-40s  %s\n" "--------------------" "----------------------------------------" "----------------------------------------" "--------"; \
	BUILT=$$(sha1sum $(BUILT_EXE) | cut -d' ' -f1); \
	ORIG=$$(sha1sum $(TARGET) | cut -d' ' -f1); \
	if [ "$$BUILT" = "$$ORIG" ]; then \
		printf "%-20s  %s  \033[32m%s\033[0m  \033[32m%s\033[0m\n" "SLUS_008.92" "$$ORIG" "$$BUILT" "Match"; \
	else \
		printf "%-20s  %s  \033[31m%s\033[0m  \033[31m%s\033[0m\n" "SLUS_008.92" "$$ORIG" "$$BUILT" "Mismatch"; \
		FAIL=1; \
	fi; \
	$(foreach ovl,$(OVERLAYS), \
		if $(MAKE) -q build-$(ovl) 2>/dev/null || $(MAKE) build-$(ovl) 2>/dev/null; then \
			BUILT=$$(sha1sum $($(ovl)_BIN) | cut -d' ' -f1); \
			ORIG=$$(sha1sum $($(ovl)_TARGET) | cut -d' ' -f1); \
			if [ "$$BUILT" = "$$ORIG" ]; then \
				printf "%-20s  %s  \033[32m%s\033[0m  \033[32m%s\033[0m\n" "$(notdir $($(ovl)_TARGET))" "$$ORIG" "$$BUILT" "Match"; \
			else \
				printf "%-20s  %s  \033[31m%s\033[0m  \033[31m%s\033[0m\n" "$(notdir $($(ovl)_TARGET))" "$$ORIG" "$$BUILT" "Mismatch"; \
				FAIL=1; \
			fi; \
		else \
			printf "%-20s  %-40s  %-40s  \033[33m%s\033[0m\n" "$(notdir $($(ovl)_TARGET))" "-" "build failed" "Skip"; \
		fi; \
	) \
	if [ "$$FAIL" = "1" ]; then exit 1; fi

# First-time setup: create venv, install dependencies, run splat
setup:
	python3 -m venv $(VENV)
	$(PYTHON) -m pip install -r requirements.txt
	$(MAKE) split

# Re-run splat for main binary + all overlays
# After split, fix jump tables that splat generates with .L labels (which can't
# resolve cross-.o). Replace with absolute addresses until these functions are
# decomped to C.
split:
	rm -rf asm
	$(SPLAT) split $(SPLAT_YAML)
	sed -i 's/\.word \.L800116F4/.word 0x800116F4/;s/\.word \.L800116BC/.word 0x800116BC/;s/\.word \.L800116CC/.word 0x800116CC/;s/\.word \.L800116DC/.word 0x800116DC/;s/\.word \.L800116EC/.word 0x800116EC/' asm/data/800.rodata.s
	sed -i 's/\.word \.L8001204C/.word 0x8001204C/;s/\.word \.L80012064/.word 0x80012064/;s/\.word \.L80012284/.word 0x80012284/;s/\.word \.L8001278C/.word 0x8001278C/;s/\.word \.L80012444/.word 0x80012444/;s/\.word \.L80012264/.word 0x80012264/;s/\.word \.L80012714/.word 0x80012714/' asm/data/800.rodata.s
	$(foreach ovl,$(OVERLAYS),$(SPLAT) split $($(ovl)_YAML);)

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
# Menu overlays (.ovl files in original/)
MENU_OVERLAYS := menumain menucfg menupty menusts menuabl menushop menuext \
                 menuitem menumgc menugf menujnc2 menusav menucrd menututo \
                 menutmag menutips menutest

# Code overlays (.bin files in original/)
CODE_OVERLAYS := field_init display_init field_engine \
                 battle_engine battle_render battle_code field_engine_alt

OVERLAYS := $(MENU_OVERLAYS) $(CODE_OVERLAYS)

# Template for overlay build rules — $(1) = overlay name, $(2) = file extension
define OVERLAY_TEMPLATE
$(1)_YAML     := config/$(1).ovl.yaml
$(1)_DIR      := build/ovl/$(1)
$(1)_ASM_DIR  := asm/ovl/$(1)
$(1)_LD       := config/$(1).ovl.ld
$(1)_TARGET   := original/$(1).$(2)
$(1)_ELF      := $$($(1)_DIR)/$(1).elf
$(1)_BIN      := $$($(1)_DIR)/$(1).$(2)

$(1)_ASM_SRCS := $$(wildcard $$($(1)_ASM_DIR)/*.s) $$(wildcard $$($(1)_ASM_DIR)/data/*.s)
$(1)_ASM_OBJS := $$(patsubst $$($(1)_ASM_DIR)/%.s,$$($(1)_DIR)/$$($(1)_ASM_DIR)/%.o,$$($(1)_ASM_SRCS))
$(1)_C_SRCS   := $$(wildcard src/ovl/$(1)/*.c)
$(1)_C_OBJS   := $$(patsubst src/ovl/$(1)/%.c,$$($(1)_DIR)/src/ovl/$(1)/%.o,$$($(1)_C_SRCS))
$(1)_BIN_SRCS := $$(wildcard assets/*.bin)
$(1)_BIN_OBJS := $$(patsubst assets/%.bin,$$($(1)_DIR)/assets/%.o,$$($(1)_BIN_SRCS))
$(1)_ALL_OBJS := $$($(1)_ASM_OBJS) $$($(1)_C_OBJS) $$($(1)_BIN_OBJS)

$(1)_MANUAL_SYMS := $$(wildcard config/undefined_syms.$(1).txt)
$(1)_LDFLAGS  := -T $$($(1)_LD) \
                 -T config/undefined_funcs_auto.$(1).txt \
                 -T config/undefined_syms_auto.$(1).txt \
                 $$(foreach f,$$($(1)_MANUAL_SYMS),-T $$(f)) \
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

$$($(1)_DIR)/assets/%.o: assets/%.bin
	@mkdir -p $$(dir $$@)
	$$(OBJCOPY) -I binary -O elf32-tradlittlemips -B mips --rename-section .data=.data $$< $$@

$$($(1)_ELF): $$($(1)_ALL_OBJS) $$($(1)_LD)
	@mkdir -p $$(dir $$@)
	$$(LD) $$($(1)_LDFLAGS) -o $$@ $$($(1)_ALL_OBJS)

$$($(1)_BIN): $$($(1)_ELF)
	$$(OBJCOPY) -O binary $$< $$@

build-$(1): $$($(1)_BIN)

verify-$(1): $$($(1)_BIN)
	@echo "Verifying $(notdir $$($(1)_TARGET))..."
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

$(foreach ovl,$(MENU_OVERLAYS),$(eval $(call OVERLAY_TEMPLATE,$(ovl),ovl)))
$(foreach ovl,$(CODE_OVERLAYS),$(eval $(call OVERLAY_TEMPLATE,$(ovl),bin)))

# Internal: build all overlay binaries
build-overlays: $(foreach ovl,$(OVERLAYS),build-$(ovl))

.PHONY: all build verify setup setup-toolchain split clean permute build-overlays \
        $(foreach ovl,$(OVERLAYS),split-$(ovl) build-$(ovl) verify-$(ovl))
