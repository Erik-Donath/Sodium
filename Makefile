# Pfade und Dateinamen
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
DIST_DIR = dist

KERNEL_DIR = $(SRC_DIR)/kernel
BOOT_DIR = $(SRC_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub
LINKER_SCRIPT = $(SRC_DIR)/linker.ld

# Dateien
KERNEL_BIN = $(DIST_DIR)/kernel.bin
ISO = $(DIST_DIR)/Sodium.iso

# Compiler und Linker
CC = gcc
LD = ld
ASM = nasm
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR)
LDFLAGS = -m elf_i386
ASFLAGS = -f elf

# Boot-Dateien
BOOT_OBJ = $(patsubst $(BOOT_DIR)/%.asm, $(BUILD_DIR)/%.o, $(wildcard $(BOOT_DIR)/*.asm))
KERNEL_OBJS = $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(KERNEL_DIR)/*.c))

# Regeln
all: $(ISO)

# Erstelle das ISO-Image
$(ISO): $(KERNEL_BIN) $(GRUB_DIR)/grub.cfg
	mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(KERNEL_BIN) $(BUILD_DIR)/iso/boot/kernel.bin
	cp $(GRUB_DIR)/grub.cfg $(BUILD_DIR)/iso/boot/grub
	grub-mkrescue -o $(ISO) $(BUILD_DIR)/iso

# Erstelle die Kernel-Binärdatei
$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_OBJS)
	mkdir -p $(DIST_DIR)
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $(KERNEL_BIN) $(BOOT_OBJ) $(KERNEL_OBJS)

# Assemblierung der Boot-Dateien
$(BUILD_DIR)/%.o: $(BOOT_DIR)/%.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASFLAGS) -o $@ $<

# Kompiliere Kernel-Dateien
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Aufräumen
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

.PHONY: all clean
