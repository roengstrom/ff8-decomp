# FF8 PS1 Decompilation Build
# Target: SLUS_008.92 (Final Fantasy VIII, USA)

### Toolchain ###
# mipsel = MIPS little-endian (what the PS1 uses)
CPP     := mipsel-linux-gnu-cpp
CC1     := tools/gcc-2.8.1-psx/cc1
AS      := mipsel-linux-gnu-as
LD      := mipsel-linux-gnu-ld
OBJCOPY := mipsel-linux-gnu-objcopy
MASPSX  := python3 tools/maspsx/maspsx.py

### Paths ###
BUILD_DIR  := build
ASM_DIR    := asm
SRC_DIR    := src
TARGET     := original/SLUS_008.92
LD_SCRIPT  := slus_008.92.ld

### Preprocessor flags ###
CPPFLAGS := -Iinclude -undef -Wall -fno-builtin -lang-c

### Compiler flags (old-gcc cc1) ###
# -O2           : optimization level 2 (matches original)
# -G0           : no GP-relative addressing
# -mcpu=3000    : MIPS I (R3000)
# -mgas         : output GAS syntax
# -msoft-float  : software floating point (PS1 has no FPU)
CC1FLAGS := -quiet -O2 -G0 -mcpu=3000 -mgas -fgnu-linker \
            -fpeephole -ffunction-cse -fpcc-struct-return \
            -fcommon -fverbose-asm -msoft-float

### maspsx flags ###
MASPSXFLAGS := --aspsx-version=2.56 --expand-div

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

# C sources (compiled via old-gcc -> maspsx -> as)
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

# Compile C: cpp -> cc1 -> maspsx -> as -> .o
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) $< | $(CC1) $(CC1FLAGS) | \
		$(MASPSX) $(MASPSXFLAGS) --run-assembler $(ASFLAGS) -o $@

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

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build verify clean
