# Folders
SRC_DIR   = src
BUILD_DIR = build
DIST_DIR  = dist

BOOT_DIR   = $(SRC_DIR)/boot
GRUB_DIR   = $(BOOT_DIR)/grub
KERNEL_DIR = $(SRC_DIR)/kernel

# Files
LINKER_SCRIPT = $(SRC_DIR)/linker.ld
KERNEL_BIN    = $(DIST_DIR)/kernel.bin
ISO           = $(DIST_DIR)/Sodium.iso

# Compiler, Linker, Assembler + Flags
CC  = gcc
LD  = ld
ASM = nasm

CFLAGS    = -m32 -ffreestanding -nostartfiles -nodefaultlibs -O2 -Wall -Wextra -I$(SRC_DIR)
LDFLAGS   = -m elf_i386
ASFLAGS   = -f elf
GRUBFLAGS = --product-version="Sodium 0.1"

# Source + Headers
ASM_HEADER := $(shell find $(SRC_DIR) -name '*.inc')
ASM_SOURCE := $(shell find $(SRC_DIR) -name '*.asm')
C_HEADER   := $(shell find $(SRC_DIR) -name '*.h')
C_SOURCE   := $(shell find $(SRC_DIR) -name '*.c')

# Objects
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/obj/asm/%.obj, $(ASM_SOURCE))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,   $(BUILD_DIR)/obj/c/%.obj,   $(C_SOURCE))

# Create ISO -> dist/Sodium.iso
$(ISO): $(KERNEL_BIN) $(GRUB_DIR)/grub.cfg
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
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $(KERNEL_BIN) $(ASM_OBJECTS) $(C_OBJECTS)
	@echo "--> Linked: " $@


# Compiling and Assembling Source + Headers
$(BUILD_DIR)/obj/c/%.obj: $(SRC_DIR)/%.c $(C_HEADER)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "--> Compiled: " $<

$(BUILD_DIR)/obj/asm/%.obj: $(SRC_DIR)/%.asm $(ASM_HEADER)
	@mkdir -p $(@D)
	$(ASM) $(ASFLAGS) -o $@ $<
	@echo "--> Assembled: " $<

info:
	@echo "Info: "
	@echo "C Source:   " $(C_SOURCE)
	@echo "C Header:   " $(C_HEADER)
	@echo "ASM Source: " $(ASM_SOURCE)
	@echo "ASM Header: " $(ASM_HEADER)
	@echo ""

# Final Build Rules
all: $(ISO)
	@echo "--> DONE"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "--> CLEANED"

.PHONY: info all clean
