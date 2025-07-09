#!/bin/bash
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Sodium OS Runner${NC}"
echo "===================="

# Build first
echo -e "${YELLOW}Building Sodium...${NC}"
make all

# Check if ISO exists
if [ ! -f "dist/Sodium.iso" ]; then
    echo -e "${RED}Error: Sodium.iso not found!${NC}"
    exit 1
fi

# Check if QEMU is available
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo -e "${RED}Error: qemu-system-x86_64 not found!${NC}"
    echo "Please install QEMU to run Sodium."
    exit 1
fi

# Run QEMU
echo -e "${GREEN}Starting Sodium in QEMU...${NC}"
echo "Press Ctrl+Alt+G to release mouse capture"
echo "Press Ctrl+Alt+2 to access QEMU monitor"
echo ""

qemu-system-x86_64 \
    -debugcon stdio \
    -cdrom dist/Sodium.iso \
    -m 128M \
    -display gtk \
    -name "Sodium OS"
