# FF8 PS1 Decompilation Build
# Target: SLUS_008.92 (Final Fantasy VIII, USA)

### Toolchain ###
# mipsel = MIPS little-endian (what the PS1 uses)
AS      := mipsel-linux-gnu-as
LD      := mipsel-linux-gnu-ld
OBJCOPY := mipsel-linux-gnu-objcopy

### Paths ###
BUILD_DIR  := build
ASM_DIR    := asm
SRC_DIR    := src
TARGET     := original/SLUS_008.92
LD_SCRIPT  := slus_008.92.ld

### Assembler flags ###
# -march=r3000  : MIPS I (the PS1 CPU)
# -mabi=32      : 32-bit ABI
# -EL           : little-endian
# -no-pad-sections : don't add padding between sections
# -Iinclude     : search include/ for .inc files
ASFLAGS := -march=r3000 -mabi=32 -EL -no-pad-sections -Iinclude

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
# Find all .s files in asm/ (header, code, data)
ASM_SRCS := $(wildcard $(ASM_DIR)/*.s) $(wildcard $(ASM_DIR)/data/*.s)
# Convert source paths to object paths: asm/foo.s -> build/asm/foo.o
ASM_OBJS := $(patsubst $(ASM_DIR)/%.s,$(BUILD_DIR)/$(ASM_DIR)/%.o,$(ASM_SRCS))

### Targets ###

# Default: build and verify
all: verify

# Assemble: .s -> .o
$(BUILD_DIR)/$(ASM_DIR)/%.o: $(ASM_DIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Link: all .o files -> ELF
$(ELF): $(ASM_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(ASM_OBJS)

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

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build verify clean
