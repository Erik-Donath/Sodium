# Sodium OS Development Guide

This guide covers development practices, architecture decisions, and contribution guidelines for the Sodium OS project.

## Architecture Overview

Sodium follows a layered architecture approach:

```
┌─────────────────────────────────────┐
│           User Programs             │
├─────────────────────────────────────┤
│          System Calls               │
├─────────────────────────────────────┤
│         Kernel Core                 │
│  (Process, Memory, IPC, etc.)       │
├─────────────────────────────────────┤
│       Driver Interfaces             │
│   (Display, Input, Storage)         │
├─────────────────────────────────────┤
│    Architecture Abstraction         │
│        (i686 specific)              │
├─────────────────────────────────────┤
│         Hardware Layer              │
│   (CPU, Memory, Devices)            │
└─────────────────────────────────────┘
```

## Code Organization Principles

### 1. Separation of Concerns

- **Generic kernel code** should be architecture-independent
- **Architecture-specific code** goes in `src/kernel/arch/i686/`
- **Driver implementations** are architecture-specific
- **Driver interfaces** are generic

### 2. Minimal Interfaces

- Only expose what's necessary to upper layers
- Keep internal implementation details private
- Use clear, documented function signatures

### 3. Runtime Initialization

- Avoid compile-time dependencies between modules
- Use two-phase initialization when needed
- Initialize in proper dependency order

## Coding Standards

### C Code Style

```c
// Function naming: lowercase with underscores
void memory_allocate_page(void);

// Constants: UPPERCASE with underscores
#define MAX_PROCESSES 256

// Structures: lowercase with _t suffix
typedef struct process {
    uint32_t pid;
    char name[32];
} process_t;

// Architecture-specific functions: prefixed with arch
void i686_gdt_initialize(void);
```

### Assembly Code Style

```asm
; Labels: lowercase with underscores
global start
start:
    ; Instructions: lowercase, aligned
    mov     eax, 0x12345678
    call    function_name

    ; Comments: explain WHY, not WHAT
    cli     ; Disable interrupts for critical section
```

### File Organization

```
src/kernel/
├── core/           # Core kernel functionality
├── drivers/        # Generic driver interfaces
├── memory/         # Generic memory management
├── process/        # Process management
└── arch/i686/      # i686-specific implementations
    ├── cpu/        # CPU setup (GDT, IDT, etc.)
    ├── drivers/    # Hardware drivers
    ├── memory/     # Architecture-specific memory
    └── interrupts/ # Interrupt handling
```

## Development Workflow

### 1. Setting Up Development Environment

```bash
# Clone the repository
git clone https://github.com/Erik-Donath/Sodium.git
cd Sodium

# Open in VSCode (recommended)
code .

# Install recommended extensions when prompted
# Build the project
make all
```

### 2. Making Changes

1. Create a feature branch: `git checkout -b feature/new-feature`
2. Make your changes following the coding standards
3. Test thoroughly in both debug and release modes
4. Update documentation if needed
5. Commit with clear, descriptive messages

### 3. Testing

```bash
# Build and test debug version
make debug
./scripts/run-sodium.sh  # or .bat on Windows

# Build and test release version
make release
./scripts/run-sodium.sh

# Clean build to ensure no artifacts
make clean && make all
```

## Architecture-Specific Guidelines

### Adding New Architecture Support

1. Create `src/kernel/arch/ARCH/` directory
2. Implement required interfaces in `arch.h`
3. Provide architecture-specific drivers
4. Update build system for new architecture

### Driver Development

```c
// Generic driver interface (src/kernel/drivers/display.h)
typedef struct display_driver {
    const char* name;
    bool (*init)(void);
    void (*putc)(char c);
    void (*clear)(void);
} display_driver_t;

// Architecture-specific implementation (src/kernel/arch/i686/drivers/vga.c)
static display_driver_t vga_driver = {
    .name = "VGA Text Mode",
    .init = vga_init,
    .putc = vga_putc,
    .clear = vga_clear
};

void vga_register_driver(void) {
    display_set_driver(&vga_driver);
}
```

## Memory Management Guidelines

### Kernel Memory Layout

```
0x00100000  ┌─────────────────┐
            │  Kernel Code    │
            ├─────────────────┤
            │  Kernel Data    │
            ├─────────────────┤
            │  Kernel BSS     │
            ├─────────────────┤
            │  Heap Start     │
            │                 │
            │  Dynamic Heap   │
            │                 │
            └─────────────────┘
```

### Memory Allocation

- Use `malloc()` and `free()` for dynamic allocation
- Implement proper error handling for allocation failures
- Consider memory alignment requirements
- Document memory ownership clearly

## Debugging Tips

### Using GDB with QEMU

```bash
# Terminal 1: Start QEMU in debug mode
make debug
qemu-system-x86_64 -s -S -debugcon stdio -cdrom dist/Sodium.iso

# Terminal 2: Connect GDB
gdb dist/kernel.bin
(gdb) target remote localhost:1234
(gdb) continue
```

### Common Debug Commands

```gdb
# Set breakpoint at function
break i686_gdt_initialize

# Examine memory
x/10x 0x100000

# Show registers
info registers

# Disassemble current location
disas

# Step through code
step
next
continue
```

### Debugging Kernel Crashes

1. Check for stack overflow (common cause)
2. Verify interrupt handling is correct
3. Check memory alignment issues
4. Use QEMU's built-in debugging features

## Performance Considerations

### Boot Time Optimization

- Minimize initialization overhead
- Use lazy initialization where possible
- Optimize critical boot path

### Runtime Performance

- Avoid unnecessary memory allocations
- Use efficient data structures
- Profile critical code paths
- Consider cache locality

## Contributing

### Before Submitting Changes

- [ ] Code follows style guidelines
- [ ] All tests pass in debug and release modes
- [ ] Documentation is updated
- [ ] No compiler warnings
- [ ] Changes are well-tested

### Commit Message Format

```
component: brief description

Longer explanation of what changed and why.
Include any breaking changes or migration notes.

Fixes #123
```

### Pull Request Guidelines

1. Keep changes focused and atomic
2. Include tests for new functionality
3. Update documentation as needed
4. Ensure CI passes
5. Request review from maintainers

## Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [Intel Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [Multiboot2 Specification](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)
- [GDB Manual](https://sourceware.org/gdb/current/onlinedocs/gdb/)

## License

This project is licensed under the terms specified in the LICENSE file.
