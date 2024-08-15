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
CFLAGS  = -m32 -ffreestanding -nostartfiles -nodefaultlibs -O2 -Wall -Wextra -I$(SRC_DIR)
LDFLAGS = -m elf_i386
ASFLAGS = -f elf

# Source + Headers
ASM_HEADER := $(wildcard $(SRC_DIR)/**/*.inc)
ASM_SOURCE := $(wildcard $(SRC_DIR)/**/*.asm)
C_HEADER   := $(wildcard $(SRC_DIR)/**/*.h)
C_SOURCE   := $(wildcard $(SRC_DIR)/**/*.c)

# Objects
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/obj/asm/%.obj, $(ASM_SOURCE))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,   $(BUILD_DIR)/obj/c/%.obj,   $(C_SOURCE))

# Create ISO -> dist/Sodium.iso
$(ISO): $(KERNEL_BIN) $(GRUB_DIR)/grub.cfg
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@mkdir -p $(DIST_DIR)

	cp $(KERNEL_BIN) $(BUILD_DIR)/iso/boot/kernel.bin
	cp $(GRUB_DIR)/grub.cfg $(BUILD_DIR)/iso/boot/grub

	grub-mkrescue -o $(ISO) $(BUILD_DIR)/iso
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

# Final Build Rules
all: $(ISO)
	@echo "--> DONE"

clean:
	@rm -rf $(BUILD_DIR) $(DIST_DIR)
	@echo "--> CLEANED"

.PHONY: all clean always
