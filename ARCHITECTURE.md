
# Sodium OS Architecture

## Module Overview

- **src/arch/i686/**: Architecture-specific implementations (CPU, memory, drivers)
- **src/kernel/**: Kernel logic, interfaces, and core functions
- **src/kernel/core/**: Terminal, error handling
- **src/kernel/drivers/**: Display drivers and hardware abstraction
- **src/kernel/libc/**: Standard and utility functions
- **src/arch/i686/memory/heap/**: Heap and memory management

## Initialization Order

1. Parse multiboot information
2. Initialize heap/memory management
3. Initialize terminal and display drivers
4. Set up CPU tables (FPU, GDT, IDT)
5. Initialize IRQs and register handlers
6. Start kernel

## Notes
- Memory management MUST be initialized before using malloc!
- Display drivers (e.g. VGA) must be initialized after heap setup.
- Remove unused includes and placeholders.
- See README.md for further notes.
