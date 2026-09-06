# FF8 PS1 Decompilation Build
# Target: SLUS_008.92 (Final Fantasy VIII, USA)

### Toolchain ###
CPP        := /usr/bin/cpp
PSYQ41_CC1 := tools/gcc-2.7.2-cdk/cc1
PSYQ43_CC1 := tools/gcc-2.8.0-psx/cc1
AS         := mipsel-linux-gnu-as
LD         := mipsel-linux-gnu-ld
OBJCOPY    := mipsel-linux-gnu-objcopy
MASPSX     := python3 tools/maspsx/maspsx.py

### Paths ###
VENV       := .venv
PYTHON     := $(VENV)/bin/python3
SPLAT      := $(PYTHON) -m splat
BUILD_DIR  := build
ASM_DIR    := asm
SRC_DIR    := src
# The main executable's binary name; every path below derives from it.
MAIN       := SLUS_008.92
SPLAT_CONF := config/ff8.yaml
SPLAT_GEN  := build/splat
# Path template for the generated splat configs; {name} is the binary's name.
SPLAT_YAML_TMPL := $(SPLAT_GEN)/{name}.yaml
BINARIES_MK := $(SPLAT_GEN)/binaries.mk

# Each binary's paths come from the binary map, so the Makefile and splat
# always split and verify the same file. Make remakes this and restarts if
# the config is newer.
-include $(BINARIES_MK)
$(BINARIES_MK): $(SPLAT_CONF) tools/gen_splat_config.py
	@$(PYTHON) tools/gen_splat_config.py $(SPLAT_CONF) --out '$(SPLAT_YAML_TMPL)' --make $@

# The main executable's paths, like every overlay's, come from the config.
TARGET     := $($(MAIN)_TARGET)
SPLAT_YAML := $($(MAIN)_YAML)
LD_SCRIPT  := $($(MAIN)_LD)
ELF        := $($(MAIN)_ELF)
BUILT_EXE  := $(BUILD_DIR)/$(MAIN)

### Compiler flags ###
CC_FLAGS := -O2 -G0

# Set NON_MATCHING=1 to compile C decomps that don't byte-match yet
# (e.g. due to ASPSX vs GAS assembler differences)
ifdef NON_MATCHING
NON_MATCHING_FLAGS := -DNON_MATCHING
endif

### Per-toolchain maspsx settings (aspsx-version controls pseudo-instruction expansion) ###
PSYQ40_MASPSXFLAGS := --aspsx-version=2.56  # used by O0_SRCS
PSYQ41_MASPSXFLAGS := --aspsx-version=2.67  # default for PsyQ 4.1 sources
PSYQ43_MASPSXFLAGS := --aspsx-version=2.77  # used by PSYQ43_SRCS

# Source files compiled with PsyQ 4.3 (default is PsyQ 4.1)
PSYQ43_SRCS := src/snd_init.c src/snd_dma.c src/snd_voice.c src/snd_bank.c src/snd_param.c src/snd_note.c src/snd_track.c src/snd_cmd.c \
               src/world/we_object0.c \
               src/world/we_object1.c src/world/we_object2.c src/world/we_object3.c \
               src/world/we_object4.c src/world/we_object5.c src/world/we_object6.c \
               src/world/we_object7.c src/world/we_object8.c src/world/we_object9.c \
               src/world/we_object10.c \
               src/field/fe_object1b.c src/field/fe_object2.c

# Source files compiled without -G0 (default is -G0)
NO_G0_SRCS := src/main.c src/snd_cmd.c

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
           -T $(SPLAT_GEN)/undefined_funcs_auto.txt \
           -T $(SPLAT_GEN)/undefined_syms_auto.txt \
           --no-check-sections \
           -Map $(ELF:.elf=.map)

### Object lists ###
# The object list is splat's, not ours: `ld_dependencies` makes it write a make
# rule naming every object in the link. Including that rule gives the ELF its
# prerequisites directly, so nothing here has to know which objects exist or
# what kind each one is -- only how to build one, via the pattern rules below.
MAIN_DEP := $(basename $(LD_SCRIPT)).d
-include $(MAIN_DEP)

# Overlay binaries (.ovl menu overlays + .bin code overlays).
MENU_OVERLAYS := menumain menucfg menupty menusts menuabl menushop menuext \
                 menuitem menumgc menugf menujnc2 menusav menucrd menututo \
                 menutmag menutips menutest
CODE_OVERLAYS := field_init intro field \
                 tripletriad battle_render battle world
# One per battle action, from the generated block in config/ff8.yaml; the names
# come back from binaries.mk rather than being repeated here.
EFFECT_OVERLAYS := $(filter effect_%,$(SPLAT_BINARIES))
OVERLAYS      := $(MENU_OVERLAYS) $(CODE_OVERLAYS) $(EFFECT_OVERLAYS)

### Targets ###

# Default: build and verify everything
all: verify

# Full rebuild: clean, split, build assets, and verify
full:
	$(MAKE) clean
	$(MAKE) split
	$(MAKE) build-assets
	$(MAKE) verify

# Assemble: .s -> .o
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Compile C: cpp → cc1 → maspsx → GAS → .o
# PsyQ 4.1 uses gcc-2.7.2-cdk (cygnus-2.7.2-970404 SN32.3.7)
# PsyQ 4.3 uses gcc-2.8.0-psx (gcc 2.8.0)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CPP) -E -lang-c -nostdinc -Iinclude $(NON_MATCHING_FLAGS) $< -o $(BUILD_DIR)/$(*F).i && \
	$(if $(filter $<,$(PSYQ43_SRCS)), \
		$(PSYQ43_CC1) -quiet $(if $(filter $<,$(G4_SRCS)),-O2 -G4,$(if $(filter $<,$(NO_G0_SRCS)),-O2,$(CC_FLAGS))) $(BUILD_DIR)/$(*F).i -o $(BUILD_DIR)/$(*F).s && \
		cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(PSYQ43_MASPSXFLAGS) --run-assembler $(ASFLAGS) -o $@, \
		$(PSYQ41_CC1) -quiet $(if $(filter $<,$(O0_SRCS)),-O0 -G0,$(if $(filter $<,$(G4_SRCS)),-O2 -G4,$(if $(filter $<,$(NO_G0_SRCS)),-O2,$(CC_FLAGS)))) $(BUILD_DIR)/$(*F).i -o $(BUILD_DIR)/$(*F).s && \
		cat $(BUILD_DIR)/$(*F).s | $(MASPSX) $(if $(filter $<,$(O0_EXPAND_LI_SRCS)),,$(if $(filter $<,$(O0_SRCS)),$(PSYQ40_MASPSXFLAGS),$(PSYQ41_MASPSXFLAGS))) --run-assembler $(ASFLAGS) -o $@)

# Link: all .o files -> ELF
$(ELF): $(LD_SCRIPT)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(filter %.o,$^)

# Convert: ELF -> raw binary (the PS-EXE)
$(BUILT_EXE): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Build everything (main + overlays)
build: $(BUILT_EXE) build-overlays

# Build and compare SHA1 against originals (main + overlays).
# Builds each overlay; any build failure or SHA1 mismatch fails the target.
# The name/built/original triples go through a file rather than being expanded
# into the recipe: inline, several hundred overlays exceed the kernel's 128 KB
# limit on a single shell argument and make dies with "Argument list too long".
VERIFY_LIST := $(BUILD_DIR)/verify.list

verify: $(BUILT_EXE) $(foreach ovl,$(OVERLAYS),build-$(ovl))
	@mkdir -p $(BUILD_DIR)
	$(file >$(VERIFY_LIST),$(MAIN) $(BUILT_EXE) $(TARGET))
	$(foreach ovl,$(OVERLAYS),$(file >>$(VERIFY_LIST),$(notdir $($(ovl)_TARGET)) $($(ovl)_BIN) $($(ovl)_TARGET)))
	@FAIL=0; \
	printf "%-20s  %-40s  %-40s  %s\n" "Name" "Expected" "Actual" "State"; \
	printf "%-20s  %-40s  %-40s  %s\n" "--------------------" "----------------------------------------" "----------------------------------------" "--------"; \
	while read -r NAME BUILT ORIG; do \
		B=$$(sha1sum "$$BUILT" | cut -d' ' -f1); \
		O=$$(sha1sum "$$ORIG" | cut -d' ' -f1); \
		if [ "$$B" = "$$O" ]; then \
			printf "%-20s  %s  \033[32m%s\033[0m  \033[32m%s\033[0m\n" "$$NAME" "$$O" "$$B" "Match"; \
		else \
			printf "%-20s  %s  \033[31m%s\033[0m  \033[31m%s\033[0m\n" "$$NAME" "$$O" "$$B" "Mismatch"; \
			FAIL=1; \
		fi; \
	done < $(VERIFY_LIST); \
	if [ "$$FAIL" = "1" ]; then exit 1; fi

# First-time setup: create venv, install dependencies, run splat
setup:
	python3 -m venv $(VENV)
	$(PYTHON) -m pip install -r requirements.txt
	$(MAKE) split

# Expand the compact binary map into full splat configs.
splat-config:
	$(PYTHON) tools/gen_splat_config.py $(SPLAT_CONF) --out '$(SPLAT_YAML_TMPL)' --make $(BINARIES_MK)

# Re-run splat for main binary + all overlays
split: splat-config
	rm -rf asm
	$(SPLAT) split $(SPLAT_YAML)
	$(foreach ovl,$(OVERLAYS),$(SPLAT) split $($(ovl)_YAML);)

# Symbol-hygiene audit (see tools/check_symbols.py and cleanuptask.md).
# Optional args narrow output, e.g. `make check ARGS=world` or `make check ARGS=[A]`.
check:
	$(PYTHON) tools/check_symbols.py $(ARGS)

clean:
	rm -rf $(BUILD_DIR)

# Run decomp-permuter for a function: make permute FUNC=func_name
permute:
ifndef FUNC
	$(error Usage: make permute FUNC=<function_name>)
endif
	./permute.sh $(FUNC)

### Overlays ###
# Template for overlay build rules — $(1) = overlay name
define OVERLAY_TEMPLATE
# $(1)_DIR is splat's build_path, from binaries.mk.
$(1)_BIN := $$($(1)_DIR)/$$(notdir $$($(1)_TARGET))

# Same as the main binary: take the object list from splat's dependency file.
$(1)_DEP := $$(basename $$($(1)_LD)).d
-include $$($(1)_DEP)

$(1)_LDFLAGS  := -T $$($(1)_LD) \
                 -T $$(SPLAT_GEN)/undefined_funcs_auto.$(1).txt \
                 -T $$(SPLAT_GEN)/undefined_syms_auto.$(1).txt \
                 -T config/symbols.extern.txt \
                 --no-check-sections \
                 -Map $$($(1)_DIR)/$(1).map

split-$(1): splat-config
	$$(SPLAT) split $$($(1)_YAML)

# An object's path is the binary's build_path plus the source path, whatever
# directory that source lives in -- so these two rules need no per-overlay paths.
$$($(1)_DIR)/%.o: %.s
	@mkdir -p $$(dir $$@)
	$$(AS) $$(ASFLAGS) -o $$@ $$<

$$($(1)_DIR)/%.o: %.c
	@mkdir -p $$(dir $$@)
	$$(CPP) -E -lang-c -nostdinc -Iinclude $$< -o $$($(1)_DIR)/$$(*F).i && \
	$$(if $$(filter $$<,$$(PSYQ43_SRCS)), \
		$$(PSYQ43_CC1) -quiet $$(CC_FLAGS) $$($(1)_DIR)/$$(*F).i -o $$($(1)_DIR)/$$(*F).s && \
		cat $$($(1)_DIR)/$$(*F).s | $$(MASPSX) $$(PSYQ43_MASPSXFLAGS) --run-assembler $$(ASFLAGS) -o $$@, \
		$$(PSYQ41_CC1) -quiet $$(CC_FLAGS) $$($(1)_DIR)/$$(*F).i -o $$($(1)_DIR)/$$(*F).s && \
		cat $$($(1)_DIR)/$$(*F).s | $$(MASPSX) $$(PSYQ41_MASPSXFLAGS) --run-assembler $$(ASFLAGS) -o $$@)

$$($(1)_DIR)/assets/%.o: assets/%.bin
	@mkdir -p $$(dir $$@)
	$$(OBJCOPY) -I binary -O elf32-tradlittlemips -B mips --rename-section .data=.data $$< $$@

$$($(1)_ELF): $$($(1)_LD)
	@mkdir -p $$(dir $$@)
	$$(LD) $$($(1)_LDFLAGS) -o $$@ $$(filter %.o,$$^)

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

# binaries.mk supplies every <ovl>_DIR/_LD/_ELF, so there is nothing to build
# rules from until it has been read -- on the first parse after a clean the
# template would expand its paths to bare "/". Make regenerates binaries.mk
# above and re-executes, and the rules get defined on that second pass.
ifneq ($(wildcard $(BINARIES_MK)),)
$(foreach ovl,$(OVERLAYS),$(eval $(call OVERLAY_TEMPLATE,$(ovl))))
endif

# field_init: extract font TIM from overlay binary during split
FIELD_INIT_TIM := assets/field_init_font.tim

split-field_init: $(FIELD_INIT_TIM)

$(FIELD_INIT_TIM): original/field_init.bin
	dd if=$< of=$@ bs=1 skip=$$((0x500)) count=$$((0x460)) 2>/dev/null

# Asset pipeline: convert binary assets to C source
build-assets: $(FIELD_INIT_TIM)
	$(PYTHON) tools/assets.py build config/assets.yaml

# field_init font object depends on generated C
build/ovl/field_init/src/ovl/field_init/field_init_tim.o: build-assets

# Internal: build all overlay binaries
build-overlays: $(foreach ovl,$(OVERLAYS),build-$(ovl))

### Progress report (objdiff) ###
OBJDIFF := tools/objdiff/objdiff

expected:
	@python3 tools/objdiff/build_expected.py

objdiff-config:
	@python3 tools/objdiff/objdiff_generate.py

report: objdiff-config
	@$(OBJDIFF) report generate -p . -o $(BUILD_DIR)/report.json
	@python3 tools/objdiff/progress_html.py $(BUILD_DIR)/report.json $(BUILD_DIR)/progress.html

.PHONY: all full build verify check setup split splat-config clean permute build-overlays \
        expected objdiff-config report \
        $(foreach ovl,$(OVERLAYS),split-$(ovl) build-$(ovl) verify-$(ovl))
