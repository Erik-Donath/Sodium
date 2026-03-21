# Sodium

[![Demo](https://img.shields.io/badge/demo-online-green)](https://erik-donath.github.io/Sodium/demo/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## Initialization Order
1. Parse multiboot information
2. Initialize heap/memory management
3. Initialize terminal and display drivers
4. Set up CPU tables (FPU, GDT, IDT)
5. Initialize IRQs and register handlers
6. Start kernel

## Architecture
- src/arch/i686: Architecture-specific implementations
- src/kernel: Kernel logic and interfaces
- src/kernel/core: Core functions like terminal, error handling
- src/kernel/drivers: Display drivers and hardware abstraction
- src/kernel/libc: Standard and utility functions
- src/arch/i686/memory/heap: Heap and memory management

## Notes
- Memory management MUST be initialized before using malloc!
- Display drivers (e.g. VGA) must be initialized after heap setup.
- See ARCHITECTURE.md for more details.
# Sodium

A small, hobbyist-grade x86 operating system which uses Multiboot2 with the GRUB bootloader.

> **Warning**: This system is still in early development. Many things can (and probably will) go wrong.

## What is an Operating System

![EX1](EX1.png)

## Quick Start

### Prerequisites

- **Docker**: For consistent build environment
- **QEMU** (optional): For running the OS in emulation
- **VSCode** (recommended): For development with full IDE support

### Installation

#### Windows (10 & 11)

```cmd
winget install Docker.DockerDesktop
```

Visit [QEMU's website](https://www.qemu.org/) and install the binaries by _Stefan Weil_.

#### Linux (Debian/Ubuntu)

```bash
sudo apt install docker.io qemu-system-x86
```

### Building and Running

#### Method 1: Using VSCode (Recommended)

1. Open the project in VSCode
2. Install recommended extensions when prompted
3. Use **Ctrl+Shift+P** → "Tasks: Run Task" → "build-sodium"
4. Use **F5** to run Sodium in QEMU

#### Method 2: Using Scripts

**Windows:**

```cmd
scripts\run-sodium.bat
```

**Linux:**

```bash
./scripts/run-sodium.sh
```

#### Method 3: Manual Build

1. Build the Docker environment:

```bash
docker build buildenv -t sodium-buildenv
```

2. Build the OS:

```bash
# Windows
docker run --rm -v %cd%:/root/env:Z sodium-buildenv

# Linux
sudo docker run --rm -it -v ./:/root/env:Z sodium-buildenv
```

3. Run in QEMU:

```bash
qemu-system-x86_64 -debugcon stdio -cdrom dist/Sodium.iso
```

## Development

### Available Make Targets

```bash
make all      # Build the OS (default)
make debug    # Build with debug symbols
make release  # Build optimized version
make clean    # Clean build artifacts
make info     # Show source file information
make help     # Show all available targets
```

### Project Structure

```
Sodium/
├── .vscode/              # VSCode configuration
├── buildenv/             # Docker build environment
├── scripts/              # Helper scripts
├── src/
│   ├── boot/i686/        # Boot code and GRUB config
│   ├── kernel/           # Generic kernel code
│   │   ├── core/         # Core kernel functionality
│   │   │   └── terminal.*# Terminal abstraction layer
│   │   ├── libc/         # Standard library functions
│   │   │   ├── stdio.*   # Standard I/O functions
│   │   │   └── util.*    # Utility functions and macros
│   │   ├── memory/       # Memory management interfaces
│   │   │   └── memory.h  # Memory allocation interface
│   │   ├── drivers/      # Driver interfaces (future expansion)
│   │   └── process/      # Process management (future expansion)
│   ├── arch/i686/        # i686-specific implementation
│   │   ├── arch.c/h      # Architecture initialization
│   │   ├── cpu/          # CPU management
│   │   │   ├── gdt/      # Global Descriptor Table
│   │   │   ├── idt/      # Interrupt Descriptor Table
│   │   │   ├── fpu/      # Floating Point Unit
│   │   │   └── ports.*   # Port I/O operations
│   │   ├── interrupts/   # Interrupt handling
│   │   │   └── irq/      # IRQ management and PIC
│   │   ├── memory/       # Memory management
│   │   │   └── heap/     # Heap allocator
│   │   ├── drivers/      # Hardware drivers
│   │   │   ├── vga/      # VGA text mode driver
│   │   │   └── debug/    # Debug output driver
│   │   └── boot/         # Multiboot2 support
│   └── linker.ld         # Linker script
├── build/                # Build artifacts (generated)
├── dist/                 # Output files (generated)
│   ├── kernel.bin        # Kernel binary
│   └── Sodium.iso        # Bootable ISO
└── Makefile              # Build system
```

## Architecture Overview

Sodium follows a layered architecture with clear separation between generic kernel code and architecture-specific implementations:

```
┌─────────────────────────────────────┐
│           User Programs             │  (Future)
├─────────────────────────────────────┤
│          System Calls               │  (Future)
├─────────────────────────────────────┤
│         Kernel Core                 │  ← kernel/core/
│  (Terminal, Process, Memory, etc.)  │    kernel/libc/
├─────────────────────────────────────┤
│       Driver Interfaces             │  ← kernel/drivers/
│   (Display, Input, Storage)         │
├─────────────────────────────────────┤
│    Architecture Abstraction         │  ← arch/i686/
│        (i686 specific)              │
├─────────────────────────────────────┤
│         Hardware Layer              │  (Physical Hardware)
│   (CPU, Memory, Devices)            │
└─────────────────────────────────────┘
```

### **Generic Kernel Layer** (`src/kernel/`)

- **Core functionality** that works across different architectures
- **Driver interfaces** and abstractions
- **Standard library** functions (stdio, string utilities)
- **Terminal management** with multi-driver support

### **Architecture Layer** (`src/arch/i686/`)

- **Hardware-specific implementations** for i686 processors
- **CPU management** (GDT, IDT, FPU setup)
- **Interrupt handling** (IRQ routing, PIC management)
- **Memory management** (heap allocation, future paging)
- **Hardware drivers** (VGA, debug output, future keyboard/timer)
- **Boot support** (Multiboot2 parsing and information)

### **Boot Layer** (`src/boot/i686/`)

- **Assembly boot code** and multiboot2 header
- **GRUB configuration** and bootloader setup
- **Early system initialization** before kernel handoff

This structure makes it easy to:

- **Add new architectures** (ARM, x86_64, RISC-V)
- **Implement new drivers** following established interfaces
- **Maintain and debug** with clear module boundaries
- **Test components** independently

## Troubleshooting

### Common Build Issues

**Docker build fails:**

- Ensure Docker Desktop is running
- Try: `docker system prune` to clean up Docker cache
- On Linux: Check if your user is in the `docker` group

**"Permission denied" on Linux:**

- Use `sudo` with Docker commands
- Or add your user to docker group: `sudo usermod -aG docker $USER`

**GRUB errors:**

- Ensure `grub-mkrescue` is available in the Docker container
- Check that `grub.cfg` syntax is correct

**Multiboot2 compliance errors:**

- Verify the multiboot header in `boot.asm`
- Check linker script alignment requirements

### Runtime Issues

**QEMU won't start:**

- Verify QEMU installation: `qemu-system-x86_64 --version`
- Check that `dist/Sodium.iso` exists and is not corrupted
- Try running with `-nographic` flag for text-only output

**Kernel crashes immediately:**

- Use debug build: `make debug`
- Enable QEMU logging: `-d int,cpu_reset`
- Check stack setup in boot code

**No output in QEMU:**

- Verify VGA driver initialization
- Check if terminal is properly set up
- Try serial output as alternative

### Development Tips

- Use `make clean` before rebuilding after major changes
- Test in both debug and release modes
- Keep backup of working versions before major refactoring

### Getting Help

If you encounter issues not covered here:

1. Check the [GitHub Issues](https://github.com/Erik-Donath/Sodium/issues)
2. Ensure you're using the latest version
3. Include build output and error messages when reporting issues

## Development Planning

For detailed development roadmap, feature planning, and project goals, see [ROADMAP.md](ROADMAP.md).
