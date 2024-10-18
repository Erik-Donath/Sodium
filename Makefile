# Folders
SRC_DIR    = src
BUILD_DIR  = build
DIST_DIR   = dist
SCRIPT_DIR = script

# Spesific Folders
BOOT_DIR   = $(SRC_DIR)/boot
GRUB_DIR   = $(BOOT_DIR)/i686/grub
KERNEL_DIR = $(SRC_DIR)/kernel

# Files
LINKER_SCRIPT = $(SRC_DIR)/linker.ld
KERNEL_BIN    = $(DIST_DIR)/kernel.bin
ISO           = $(DIST_DIR)/Sodium.iso

# Compiler, Linker, Assembler + Flags
CC  = gcc
LD  = ld
ASM = nasm

CFLAGS    = -m32 -g -ffreestanding -std=c99 -nostartfiles -nodefaultlibs -O2 -Wall -Wextra -I$(SRC_DIR)
LDFLAGS   = -m elf_i386
ASFLAGS   = -f elf
GRUBFLAGS = --product-version="Sodium 0.1"

# Source + Headers
ASM_HEADER = $(shell find $(SRC_DIR) -name '*.inc')
ASM_SOURCE = $(shell find $(SRC_DIR) -name '*.asm')
C_HEADER   = $(shell find $(SRC_DIR) -name '*.h')
C_SOURCE   = $(shell find $(SRC_DIR) -name '*.c')

# Add Generate Sources
once = $(if $(filter $2,$1),$1,$1 $2)
ASM_HEADER := $(call once, $(ASM_HEADER),$(KERNEL_DIR)/arch/i686/isr_gen.inc)
C_SOURCE   := $(call once, $(C_SOURCE),  $(KERNEL_DIR)/arch/i686/isr_gen.c)

# Objects
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/obj/asm/%.obj, $(ASM_SOURCE))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,   $(BUILD_DIR)/obj/c/%.obj,   $(C_SOURCE))

# Create ISO -> dist/Sodium.iso
$(ISO): $(KERNEL_BIN) $(GRUB_DIR)/grub.cfg
	@echo "--> Building $@"
	@mkdir -p $(BUILD_DIR)/iso/bin
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@mkdir -p $(DIST_DIR)

	cp $(KERNEL_BIN) $(BUILD_DIR)/iso/bin/kernel.bin
	cp $(GRUB_DIR)/grub.cfg $(BUILD_DIR)/iso/boot/grub

	grub-mkrescue $(GRUBFLAGS) -o $(ISO) $(BUILD_DIR)/iso
	@echo "--> Created: " $@

# Link Objects into dist/kernel.bin
$(KERNEL_BIN): $(ASM_OBJECTS) $(C_OBJECTS) $(LINKER_SCRIPT)
	@mkdir -p $(DIST_DIR)
	@echo "--> Linking: " $@
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $(KERNEL_BIN) $(ASM_OBJECTS) $(C_OBJECTS)

# Compiling and Assembling Source + Headers
$(BUILD_DIR)/obj/c/%.obj: $(SRC_DIR)/%.c $(C_HEADER)
	@mkdir -p $(@D)
	@echo "--> Compiling: " $<
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/obj/asm/%.obj: $(SRC_DIR)/%.asm $(ASM_HEADER)
	@mkdir -p $(@D)
	@echo "--> Assembling: " $<
	$(ASM) $(ASFLAGS) -o $@ $<

# Sources that are generated
$(KERNEL_DIR)/arch/i686/isr_gen.c: $(SCRIPT_DIR)/generate_isr_c.sh
	@echo "--> Generating: $@"
	$(SCRIPT_DIR)/generate_isr_c.sh $@

$(KERNEL_DIR)/arch/i686/isr_gen.inc: $(SCRIPT_DIR)/generate_isr_inc.sh
	@echo "--> Generating: $@"
	$(SCRIPT_DIR)/generate_isr_inc.sh $@

# Final Build Rules
info:
	@echo "Info: "
	@echo "C Source:   " $(C_SOURCE)
	@echo "C Header:   " $(C_HEADER)
	@echo "ASM Source: " $(ASM_SOURCE)
	@echo "ASM Header: " $(ASM_HEADER)
	@echo ""

all: $(ISO)
	@echo "--> DONE"

clear:
	@echo "--> Clearing builds"
	rm -rf $(BUILD_DIR)
	rm -rf $(KERNEL_BIN)
	rm -f $(KERNEL_DIR)/arch/i686/isr_gen.inc
	rm -f $(KERNEL_DIR)/arch/i686/isr_gen.c

.PHONY: info all clean
