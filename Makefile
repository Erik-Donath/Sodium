ARCH      ?= i686
BUILD     ?= Debug

DIST_DIR  = dist
BUILD_DIR = build

IMAGE        = sodium-buildenv
DOCKER_STAMP = .docker-stamp

.PHONY: all build run debug clean help

all: build

$(DOCKER_STAMP): buildsystem/Dockerfile
	docker build buildsystem -t $(IMAGE)
	docker inspect --format='{{.Id}}' $(IMAGE) > $(DOCKER_STAMP)

build: $(DOCKER_STAMP)
	docker run --rm \
	    --user $(shell id -u):$(shell id -g) \
	    -v $(PWD):/workspace $(IMAGE) $(ARCH) $(BUILD)

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
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(DOCKER_STAMP)

rebuild-image:
	docker build --no-cache buildsystem -t $(IMAGE)
	docker inspect --format='{{.Id}}' $(IMAGE) > $(DOCKER_STAMP)

help:
	@echo "Sodium OS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  build          - Build the OS (default)"
	@echo "  run            - Run in QEMU"
	@echo "  debug          - Run in QEMU with GDB attached"
	@echo "  clean          - Remove build artifacts and Docker stamp"
	@echo "  rebuild-image  - Force a full Docker image rebuild"
	@echo ""
	@echo "Options:"
	@echo "  ARCH=i686        Target architecture (default: i686)"
	@echo "  BUILD=Debug      Build type: Debug or Release (default: Debug)"
	@echo ""
	@echo "Output:"
	@echo "  $(DIST_DIR)/Sodium.iso   Bootable ISO"
	@echo "  $(DIST_DIR)/sodium.dbg   Debug symbols (Debug builds only)"
