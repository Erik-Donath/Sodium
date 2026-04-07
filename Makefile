ARCH  ?= i686
BUILD ?= Debug

DIST_DIR  = dist
BUILD_DIR = build

.PHONY: all build rebuild-image run debug clean help

all: build

build:
	$(MAKE) --no-print-directory -C buildsystem/$(ARCH) build BUILD=$(BUILD)

rebuild-image:
	$(MAKE) --no-print-directory -C buildsystem/$(ARCH) rebuild-image

run:
	qemu-system-i386 -debugcon stdio \
	    -no-reboot \
	    -cdrom $(DIST_DIR)/Sodium.iso

debug:
	qemu-system-i386 -debugcon stdio \
	    -no-reboot \
	    -cdrom $(DIST_DIR)/Sodium.iso -s -S &
	gdb -ex "set architecture i386" \
	    -ex "target remote :1234" \
	    -ex "symbol-file $(DIST_DIR)/sodium.dbg"

clean:
	$(MAKE) --no-print-directory -C buildsystem/$(ARCH) clean
	rm -rf $(BUILD_DIR) $(DIST_DIR)

vnc:
	websockify --web /usr/share/novnc/ 6080 localhost:5900

help:
	@echo "Sodium OS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  build          - Build the OS (default)"
	@echo "  run            - Run in QEMU"
	@echo "  debug          - Run in QEMU with GDB attached"
	@echo "  clean          - Remove build artifacts"
	@echo "  vnc            - Starts a novnc webserver that connects to QEMU"
	@echo "  rebuild-image  - Force a full Docker image rebuild"
	@echo ""
	@echo "Options:"
	@echo "  ARCH=i686        Target architecture (default: i686)"
	@echo "  BUILD=Debug      Build type: Debug or Release (default: Debug)"
	@echo ""
	@echo "Output:"
	@echo "  $(DIST_DIR)/Sodium.iso   Bootable ISO"
	@echo "  $(DIST_DIR)/sodium.dbg   Debug symbols (Debug builds only)"
